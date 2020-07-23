/*
 * $ID: trunk.c,v 1.0 2017/03/27 01:16:00 erictom Exp $
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Tomahawk 3 Trunking (Ether)
 */
#include <soc/mem.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/link.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/switch.h>

#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>

#include <bcm_int/esw/tomahawk3.h>

#include <bcm_int/esw/portctrl.h>

#ifdef BCM_TOMAHAWK3_SUPPORT

#define _BCM_TH3_TRUNK_NUM_MAX              144
#define _BCM_TH3_PORTS_PER_TRUNK_MAX        64
#define _BCM_TH3_TRUNK_FAILOVER_PORT_NUM    8

/*
 * Trunk Membership Info
 * TH3 supports 64 ports per trunk and does NOT support stacking or HiGig
 */
typedef struct {
    uint8  num_ports;                         /* Number of ports in the trunk */
    uint16 *port;/* Device Ports in the trunk */
    uint32 *member_flags; /* Flags for the port */
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8  recovered_from_scache;
#endif
} _th3_member_info_t;

typedef struct {
    _th3_member_info_t *member_info;
} _th3_trunk_member_info_t;

/* Pointer to array of trunks that will be allocated in init. */
STATIC _th3_trunk_member_info_t *_th3_trunk_member_info[BCM_MAX_NUM_UNITS];

#define MEMBER_INFO(_unit_, _tid_) (_th3_trunk_member_info[_unit_]->member_info[_tid_])

/* Port to the Trunk ID Mapping */
STATIC uint8 *_th3_port_to_tid_map[BCM_MAX_NUM_UNITS];

/*
 * Hardware Trunk Failover:
 *
 * For the devices which support hardware trunk failover, the failover
 * configuration must be retained if it is one of the standard models 
 * for the retrieval function to use the same encoding as the set function.
 *
 * Note: The trunk members stored in _th3_tinfo_t's port array do not 
 *       include those members with the BCM_TRUNK_MEMBER_EGRESS_DISABLE set.
 */
typedef struct {
    uint8         num_ports;
    uint8         *psc; 
    uint16        *port;
    uint32        *flags;
} _th3_hw_tinfo_t;

typedef struct {
    _th3_hw_tinfo_t *hw_tinfo;
} _th3_trunk_hwfail_t;

STATIC _th3_trunk_hwfail_t *_th3_trunk_hwfail[BCM_MAX_NUM_UNITS];

/* ----------------------- */
/* Software Trunk Failover */
/* ----------------------- */

/* Even without hardware failover support, given the high speed link
 * change notification possible with devices, a port that has been
 * indicated as link down can cause the trunk_group table to be immediately
 * updated with the link down port removed.
 *
 * Note: The trunk members stored in _th3_tinfo_t's port array do not
 *       include those members with the BCM_TRUNK_MEMBER_EGRESS_DISABLE set.
 *       If a trunk member port is down, it'll be removed from_th3_tinfo_t's
 *       port array by _bcm_th3_trunk_swfailover_trigger.
 */
typedef struct {
    uint8         rtag;
    uint16        num_ports;
    uint16        *port;
    uint32        *member_flags;
    uint32        flags;
} _th3_tinfo_t;

typedef struct {
    uint8         mymodid;
#if (SOC_MAX_NUM_PP_PORTS > SOC_MAX_NUM_PORTS)
    bcm_trunk_t   trunk[SOC_MAX_NUM_PP_PORTS];
#else
    bcm_trunk_t   trunk[SOC_MAX_NUM_PORTS]; /* If port is not a member of any
                                               trunk group, trunk = 0; else
                                               trunk = trunk id + 1. */
#endif
    _th3_tinfo_t *tinfo;
} _th3_trunk_swfail_t;

STATIC _th3_trunk_swfail_t *_th3_trunk_swfail[BCM_MAX_NUM_UNITS];


#if 1
/* fields from FAST_TRUNK_PORTS_1 and FAST_TRUNK_PORTS_2 */
static soc_field_t      _th3_fast_portf[64] = {
    TRUNK_MEMBER_0f, TRUNK_MEMBER_1f, TRUNK_MEMBER_2f, TRUNK_MEMBER_3f,
    TRUNK_MEMBER_4f, TRUNK_MEMBER_5f, TRUNK_MEMBER_6f, TRUNK_MEMBER_7f,
    TRUNK_MEMBER_8f, TRUNK_MEMBER_9f, TRUNK_MEMBER_10f, TRUNK_MEMBER_11f,
    TRUNK_MEMBER_12f, TRUNK_MEMBER_13f, TRUNK_MEMBER_14f, TRUNK_MEMBER_15f,
    TRUNK_MEMBER_16f, TRUNK_MEMBER_17f, TRUNK_MEMBER_18f, TRUNK_MEMBER_19f,
    TRUNK_MEMBER_20f, TRUNK_MEMBER_21f, TRUNK_MEMBER_22f, TRUNK_MEMBER_23f,
    TRUNK_MEMBER_24f, TRUNK_MEMBER_25f, TRUNK_MEMBER_26f, TRUNK_MEMBER_27f,
    TRUNK_MEMBER_28f, TRUNK_MEMBER_29f, TRUNK_MEMBER_30f, TRUNK_MEMBER_31f,
    TRUNK_MEMBER_32f, TRUNK_MEMBER_33f, TRUNK_MEMBER_34f, TRUNK_MEMBER_35f,
    TRUNK_MEMBER_36f, TRUNK_MEMBER_37f, TRUNK_MEMBER_38f, TRUNK_MEMBER_39f,
    TRUNK_MEMBER_40f, TRUNK_MEMBER_41f, TRUNK_MEMBER_42f, TRUNK_MEMBER_43f,
    TRUNK_MEMBER_44f, TRUNK_MEMBER_45f, TRUNK_MEMBER_46f, TRUNK_MEMBER_47f,
    TRUNK_MEMBER_48f, TRUNK_MEMBER_49f, TRUNK_MEMBER_50f, TRUNK_MEMBER_51f,
    TRUNK_MEMBER_52f, TRUNK_MEMBER_53f, TRUNK_MEMBER_54f, TRUNK_MEMBER_55f,
    TRUNK_MEMBER_56f, TRUNK_MEMBER_57f, TRUNK_MEMBER_58f, TRUNK_MEMBER_59f,
    TRUNK_MEMBER_60f, TRUNK_MEMBER_61f, TRUNK_MEMBER_62f, TRUNK_MEMBER_63f
};
#endif
#if 1

/*
 * Function:
 *      _bcm_th3_trunk_member_info_deinit
 * Purpose:
 *      De-allocate trunk membership info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void
_bcm_th3_trunk_member_info_deinit(int unit)
{
    int trunk_num_groups;
    int i;

    if (_th3_trunk_member_info[unit]) {
        if (_th3_trunk_member_info[unit]->member_info) {
            trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);
            for (i = 0; i < trunk_num_groups; i++) {
                if (MEMBER_INFO(unit, i).port) {
                    sal_free(MEMBER_INFO(unit, i).port);
                    MEMBER_INFO(unit, i).port = NULL;
                }
                if (MEMBER_INFO(unit, i).member_flags) {
                    sal_free(MEMBER_INFO(unit, i).member_flags);
                    MEMBER_INFO(unit, i).member_flags = NULL;
                }
            }
            sal_free(_th3_trunk_member_info[unit]->member_info);
            _th3_trunk_member_info[unit]->member_info = NULL;
        }
        sal_free(_th3_trunk_member_info[unit]);
        _th3_trunk_member_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th3_trunk_member_info_init
 * Purpose:
 *      Allocate and Initialize trunk member info
 * Parameters:
 *      unit - (IN) SOC unit number
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th3_trunk_member_info_init(int unit)
{
    int trunk_num_groups;

    if (_th3_trunk_member_info[unit] == NULL) {
        _th3_trunk_member_info[unit] = 
            sal_alloc(sizeof(_th3_trunk_member_info_t),
                      "_th3_trunk_member_info");
        if (_th3_trunk_member_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_member_info[unit], 0,
            sizeof(_th3_trunk_member_info_t));

    /* Allocate the array of trunk groups */

    /* Only need to count FAST_TRUNK_PORTS_1m because
     * FAST_TRUNK_PORTS_2m is extension of the same LAG */
    trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);

    if (_th3_trunk_member_info[unit]->member_info == NULL) {
        _th3_trunk_member_info[unit]->member_info = 
            sal_alloc(sizeof(_th3_member_info_t) * trunk_num_groups,
                    "member_info");
        if (_th3_trunk_member_info[unit]->member_info == NULL) {
            _bcm_th3_trunk_member_info_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_member_info[unit]->member_info, 0,
            sizeof(_th3_member_info_t) * trunk_num_groups);

    /* Note: The maximum number of member ports in a trunk group is 64 in TH3.
     *       But it would be quite wasteful to allocate 64 entries to each 
     *       _th3_trunk_member_info[unit]->member_info[x].port/member_flags
     *       arrays.
     *       These arrays will be allocated as member ports are added to
     *       trunk group.
     */


    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_deinit
 * Purpose:
 *      Deallocate _th3_trunk_hwfail
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void
_bcm_th3_trunk_hwfailover_deinit(int unit)
{
    int trunk_num_groups;
    int i;

    if (_th3_trunk_hwfail[unit]) {
        if (_th3_trunk_hwfail[unit]->hw_tinfo) {
            /* Only need to count FAST_TRUNK_PORTS_1m because
             * FAST_TRUNK_PORTS_2m is extension of the same LAG */
            trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);
            for (i = 0; i < trunk_num_groups; i++) {
                if (_th3_trunk_hwfail[unit]->hw_tinfo[i].port) {
                    sal_free(_th3_trunk_hwfail[unit]->hw_tinfo[i].port);
                    _th3_trunk_hwfail[unit]->hw_tinfo[i].port = NULL;
                }
                if (_th3_trunk_hwfail[unit]->hw_tinfo[i].psc) {
                    sal_free(_th3_trunk_hwfail[unit]->hw_tinfo[i].psc);
                    _th3_trunk_hwfail[unit]->hw_tinfo[i].psc = NULL;
                }
                if (_th3_trunk_hwfail[unit]->hw_tinfo[i].flags) {
                    sal_free(_th3_trunk_hwfail[unit]->hw_tinfo[i].flags);
                    _th3_trunk_hwfail[unit]->hw_tinfo[i].flags = NULL;
                }
            }
            sal_free(_th3_trunk_hwfail[unit]->hw_tinfo);
            _th3_trunk_hwfail[unit]->hw_tinfo = NULL;
        }
        sal_free(_th3_trunk_hwfail[unit]);
        _th3_trunk_hwfail[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_init
 * Purpose:
 *      Allocate and initialize _th3_trunk_hwfail
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th3_trunk_hwfailover_init(int unit)
{
    int trunk_num_groups;

    if (_th3_trunk_hwfail[unit] == NULL) {
        _th3_trunk_hwfail[unit] = sal_alloc(sizeof(_th3_trunk_hwfail_t), 
                "_th3_trunk_hwfail");
        if (_th3_trunk_hwfail[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_hwfail[unit], 0, sizeof(_th3_trunk_hwfail_t));

    /* Only need to count FAST_TRUNK_PORTS_1m because
     * FAST_TRUNK_PORTS_2m is extension of the same LAG */
    trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);
    if (_th3_trunk_hwfail[unit]->hw_tinfo == NULL) {
        _th3_trunk_hwfail[unit]->hw_tinfo =
            sal_alloc(sizeof(_th3_hw_tinfo_t) * trunk_num_groups,
                    "_th3_trunk_hwfail_hw_tinfo");
        if (_th3_trunk_hwfail[unit]->hw_tinfo == NULL) {
            _bcm_th3_trunk_hwfailover_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_hwfail[unit]->hw_tinfo, 0, 
               sizeof(_th3_hw_tinfo_t) * trunk_num_groups);
                                                                                
    /* Note: The maximum number of member ports in a trunk group is 64 in TH3.
     *       But it would be quite wasteful to allocate 64 entries to each 
     *       _th3_trunk_hwfail[unit]->hw_tinfo[x].port/psc/flags arrays.
     *       These arrays will be allocated as member ports are added to trunk group.
     */

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_th3_trunk_port_map_init
 * Purpose:
 *      Initialize a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_th3_trunk_port_map_init(int unit)
{
    int mem_size;
    uint8 *ptr;

    if (NULL != _th3_port_to_tid_map[unit]) {
        _bcm_th3_trunk_port_map_deinit(unit);
    }

    /* Allocated mapping array */
    /* FIXME: VERIFY THIS SIZE!!!!! 
     * MODID = 0, Trident's alloc would set to SOC_MAX_NUM_PORTS.
     * Verify uint8 size */
    mem_size = SOC_MAX_NUM_PORTS * sizeof(uint8);
    ptr = sal_alloc(mem_size, "TH3_Port_Trunk");
    if (NULL == ptr) {
        return BCM_E_MEMORY;
    }

    /* Reset buffer */
    sal_memset(ptr, 0, mem_size);

    _th3_port_to_tid_map[unit] = ptr;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_port_map_deinit
 * Purpose:
 *      Deinitialize a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_th3_trunk_port_map_deinit(int unit) 
{
    void *ptr;

    ptr = (void *)_th3_port_to_tid_map[unit];
	
    _th3_port_to_tid_map[unit] = NULL;

    if (NULL != ptr) {
       	sal_free(ptr);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th3_trunk_port_map_set
 * Purpose:
 *      Set  (port) to  trunk id map.
 *      Map implements a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN) BCM device number.
 *      modid    -  (IN) Member port module id (not used, but from api call)
 *      port     -  (IN) Member port number.  
 *      trunk_id -  (IN) Trunk number or negative value for reset.  
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th3_trunk_port_map_set(int unit, bcm_module_t modid,
                            bcm_port_t port, bcm_trunk_t trunk_id)
{
    if (NULL == _th3_port_to_tid_map[unit]) {
        return BCM_E_INIT;
    }

    /* Trunk id range check. */
    if (trunk_id >= _BCM_TH3_TRUNK_NUM_MAX) {
        return BCM_E_INTERNAL;
    }

    if (port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    _th3_port_to_tid_map[unit][port] = (trunk_id < 0) ? 0 : (++trunk_id);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_port_map_get
 * Purpose:
 *      Read mapping (port) to trunk id from software cache 
 *      for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN) BCM device number. 
 *      port     -  (IN) Member port number.  
 *      trunk_id -  (OUT) Trunk number or negative value for reset.  
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_th3_trunk_port_map_get(int unit,
                            bcm_port_t port, bcm_trunk_t *trunk_id) 
{

    int              idx;      /* Map array index.         */

    if (NULL == _th3_port_to_tid_map[unit]) {
        return BCM_E_INIT;
    }

    idx = port;
    if (idx >= SOC_MAX_NUM_PORTS) { 
        return (BCM_E_PARAM);
    }

    *trunk_id = _th3_port_to_tid_map[unit][idx] - 1;

    return (_th3_port_to_tid_map[unit][idx]) ? BCM_E_NONE : BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      _bcm_th3_trunk_get_port_property
 * Purpose:
 *      Get trunk group ID for a port
 * Parameters:
 *      unit - (IN) SOC unit number
 *      port - (IN) Port number
 *      tid  - (OUT) Trunk group ID
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_get_port_property(int unit, bcm_port_t port, int *tid)
{
    return _bcm_th3_trunk_port_map_get(unit, port, tid);
}
/*
 * Function:
 *      _bcm_th3_trunk_swfailover_deinit
 * Purpose:
 *      Deallocate _th3_trunk_swfail
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
STATIC void
_bcm_th3_trunk_swfailover_deinit(int unit)
{
    int trunk_num_groups;
    int i;

    if (_th3_trunk_swfail[unit]) {
        if (_th3_trunk_swfail[unit]->tinfo) {
            /* Only need to count FAST_TRUNK_PORTS_1m because
             * FAST_TRUNK_PORTS_2m is extension of the same LAG */
            trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);
            for (i = 0; i < trunk_num_groups; i++) {
                if (_th3_trunk_swfail[unit]->tinfo[i].port) {
                    sal_free(_th3_trunk_swfail[unit]->tinfo[i].port);
                    _th3_trunk_swfail[unit]->tinfo[i].port = NULL;
                }
            }
            sal_free(_th3_trunk_swfail[unit]->tinfo);
            _th3_trunk_swfail[unit]->tinfo = NULL;
        }
        sal_free(_th3_trunk_swfail[unit]);
        _th3_trunk_swfail[unit] = NULL;
    }
}

/*
 * Function:
 *      _th3_trunk_add_info_member_free
 * Purpose:
 *      Free member_flags, tp, and tm of _esw_trunk_add_info_t.
 * Parameters:
 *      add_info  - (IN) Pointer to trunk add info structure.
 * Returns:
 *      BCM_E_xxx
 */
STATIC void 
_th3_trunk_add_info_member_free(_esw_trunk_add_info_t *add_info)
{
    if (NULL != add_info->member_flags) {
        sal_free(add_info->member_flags);
    }

    if (NULL != add_info->tp) {
        sal_free(add_info->tp);
    }

    if (NULL != add_info->tm) {
        sal_free(add_info->tm);
    }

    if (NULL != add_info->dynamic_scaling_factor) {
        sal_free(add_info->dynamic_scaling_factor);
    }
    if (NULL != add_info->dynamic_load_weight) {
        sal_free(add_info->dynamic_load_weight);
    }
    if (NULL != add_info->dynamic_queue_size_weight) {
        sal_free(add_info->dynamic_queue_size_weight);
    }
}


/*
 * Function:
 *      _th3_trunk_add_info_member_alloc
 * Purpose:
 *      Allocate member_flags, tp, and tm of _esw_trunk_add_info_t.
 * Parameters:
 *      add_info  - (IN/OUT) Pointer to trunk add info structure.
 *      num_ports - (IN) Number of member ports.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_th3_trunk_add_info_member_alloc(_esw_trunk_add_info_t *add_info,
        int num_ports)
{
    int rv = BCM_E_NONE;

    add_info->member_flags = sal_alloc(sizeof(uint32) * num_ports,
            "_th3_trunk_add_info_member_flags");
    if (add_info->member_flags == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->member_flags, 0, sizeof(uint32) * num_ports);

    add_info->tp = sal_alloc(sizeof(bcm_port_t) * num_ports,
            "_th3_trunk_add_info_tp");
    if(add_info->tp == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->tp, 0, sizeof(bcm_port_t) * num_ports);

    /* Following fields are not used by TH3, but add memory for
     * instances where we will fill it with 0 */
    add_info->tm = sal_alloc(sizeof(bcm_module_t) * num_ports,
            "_th3_trunk_add_info_tm");
    if (add_info->tm == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->tm, 0, sizeof(bcm_module_t) * num_ports);

    add_info->dynamic_scaling_factor = sal_alloc(sizeof(int) * num_ports,
            "_trident_trunk_add_info_dynamic_scaling_factor");
    if (add_info->dynamic_scaling_factor == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->dynamic_scaling_factor, 0, sizeof(int) * num_ports);

    add_info->dynamic_load_weight = sal_alloc(sizeof(int) * num_ports,
            "_trident_trunk_add_info_dynamic_load_weight");
    if (add_info->dynamic_load_weight == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->dynamic_load_weight, 0, sizeof(int) * num_ports);

    add_info->dynamic_queue_size_weight = sal_alloc(sizeof(int) * num_ports,
            "_trident_trunk_add_info_dynamic_queue_size_weight");
    if (add_info->dynamic_queue_size_weight == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->dynamic_queue_size_weight, 0, sizeof(int) * num_ports);

    return rv;

error:
    if (NULL != add_info->member_flags) {
        sal_free(add_info->member_flags);
        add_info->member_flags = NULL;
    }
    if (NULL != add_info->tp) {
        sal_free(add_info->tp);
        add_info->tp = NULL;
    }
    if (NULL != add_info->tm) {
        sal_free(add_info->tm);
        add_info->tm = NULL;
    }
    if (NULL != add_info->dynamic_scaling_factor) {
        sal_free(add_info->dynamic_scaling_factor);
        add_info->dynamic_scaling_factor = NULL;
    }
    if (NULL != add_info->dynamic_load_weight) {
        sal_free(add_info->dynamic_load_weight);
        add_info->dynamic_load_weight = NULL;
    }
    if (NULL != add_info->dynamic_queue_size_weight) {
        sal_free(add_info->dynamic_queue_size_weight);
        add_info->dynamic_queue_size_weight = NULL;
    }

    return rv;
}

/*
 * Function:
 *	_bcm_th3_trunk_get
 * Purpose:
 *      Return a trunk information of given trunk ID.
 * Parameters:
 *      unit   - (IN) StrataSwitch PCI device unit number (driver internal).
 *      tid    - (IN) Trunk ID.
 *      t_data - (OUT) Place to store returned trunk add info.
 *      t_info - (IN) Trunk info.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_trunk_get(int unit,
        bcm_trunk_t tid,
        _esw_trunk_add_info_t *t_data,
        trunk_private_t *t_info)
{
    int i;
    bcm_module_t mod;
    bcm_port_t port;

    t_data->flags = t_info->flags;
    t_data->psc = t_info->psc;
    t_data->ipmc_psc = t_info->ipmc_psc;
    t_data->dlf_index = t_info->dlf_index_used;
    t_data->mc_index = t_info->mc_index_used;
    t_data->ipmc_index = t_info->ipmc_index_used;
    t_data->dynamic_size = t_info->dynamic_size;
    t_data->dynamic_age = t_info->dynamic_age;
    t_data->dynamic_load_exponent =
        t_info->dynamic_load_exponent;
    t_data->dynamic_expected_load_exponent =
        t_info->dynamic_expected_load_exponent;

    if (!t_info->in_use) {
        t_data->num_ports = 0;
        return BCM_E_NONE;
    }

    if (t_data->num_ports == 0) {
        t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
    } else {
        if (MEMBER_INFO(unit, tid).num_ports <= t_data->num_ports) {
            t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
        } 

        /* Fill in data for each port in the trunk */
        for (i = 0; i < t_data->num_ports; i++) {
            mod = 0; /* True for TH3 */
            port = MEMBER_INFO(unit, tid).port[i] & 0xff;
            t_data->tm[i] = mod;
            t_data->tp[i] = port;
            t_data->member_flags[i] = MEMBER_INFO(unit, tid).member_flags[i];

            t_data->dynamic_scaling_factor[i] = 0;
            t_data->dynamic_load_weight[i] = 0;
            t_data->dynamic_queue_size_weight[i] = 0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_th3_trunk_get
 * Purpose:
 *      Return a trunk information of given trunk ID.
 * Parameters:
 *      unit   - (IN) StrataSwitch PCI device unit number (driver internal).
 *      tid    - (IN) Trunk ID.
 *      trunk_info   - (OUT) Place to store returned trunk info.
 *      member_max   - (IN) Size of member_array.
 *      member_array - (OUT) Array of trunk member structures.
 *      member_count - (OUT) Number of trunk members returned.
 *      t_info       - (IN) Internal trunk info.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */
int
bcm_th3_trunk_get(int unit,
        bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info,
        int member_max, 
        bcm_trunk_member_t *member_array,
        int *member_count,
        trunk_private_t *t_info)
{
    _esw_trunk_add_info_t add_info;
    int i;
    int rv = BCM_E_NONE;
    bcm_trunk_chip_info_t chip_info;
    int higig_trunk;

    sal_memset(&add_info, 0, sizeof(_esw_trunk_add_info_t));
    add_info.num_ports = member_max;
    if (member_max > 0) {
        BCM_IF_ERROR_RETURN
            (_th3_trunk_add_info_member_alloc(&add_info, member_max));
    }

    rv = _bcm_th3_trunk_get(unit, tid, &add_info, t_info);
    if (BCM_FAILURE(rv)) {
        if (member_max > 0) {
            _th3_trunk_add_info_member_free(&add_info);
        }
        return rv;
    }

    trunk_info->flags = add_info.flags;
    trunk_info->psc = add_info.psc;
    trunk_info->ipmc_psc = add_info.ipmc_psc;
    trunk_info->dlf_index = add_info.dlf_index;
    trunk_info->mc_index = add_info.mc_index;
    trunk_info->ipmc_index = add_info.ipmc_index;
    trunk_info->dynamic_size = add_info.dynamic_size;
    trunk_info->dynamic_age = add_info.dynamic_age;
    trunk_info->dynamic_load_exponent =
        add_info.dynamic_load_exponent;
    trunk_info->dynamic_expected_load_exponent =
        add_info.dynamic_expected_load_exponent;

    *member_count = add_info.num_ports;

    if (member_max > 0) {
        if (member_max < *member_count) {
            *member_count = member_max;
        }

        /* Construct member gport */
        rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
        if (BCM_FAILURE(rv)) {
            _th3_trunk_add_info_member_free(&add_info);
            return rv;
        }
        /* No HiGig in TH3 */
        higig_trunk = 0;
        rv = _bcm_esw_trunk_gport_construct(unit, higig_trunk,
                *member_count,
                add_info.tp, add_info.tm,
                add_info.tp);
        if (BCM_FAILURE(rv)) {
            _th3_trunk_add_info_member_free(&add_info);
            return rv;
        }

        for (i = 0; i < *member_count; i++) {
            bcm_trunk_member_t_init(&member_array[i]);
            member_array[i].flags = add_info.member_flags[i];
            member_array[i].gport = add_info.tp[i];
            member_array[i].dynamic_scaling_factor =
                add_info.dynamic_scaling_factor[i];
            member_array[i].dynamic_load_weight =
                add_info.dynamic_load_weight[i];
            member_array[i].dynamic_queue_size_weight=
                add_info.dynamic_queue_size_weight[i];
        }

        _th3_trunk_add_info_member_free(&add_info);
    }

    return rv;
}

/* Function:
 *      bcm_th3_trunk_mcast_join
 * Purpose:
 *
 * Returns:
 */
int
bcm_th3_trunk_mcast_join(int unit, bcm_trunk_t tid,
        bcm_vlan_t vid, sal_mac_addr_t mac, trunk_private_t *t_info)
{
    trunk_bitmap_entry_t        trunk_bitmap_entry;
    bcm_mcast_addr_t            mc_addr;
    bcm_trunk_chip_info_t chip_info;
  
    /* Initialising and setting the mc_addr by bcm_mcast_addr_t_init() */
    bcm_mcast_addr_t_init(&mc_addr, mac, vid);

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if ((tid >= chip_info.trunk_id_min) && (tid <= chip_info.trunk_id_max)) {
        /* get the ports in the trunk group */
        SOC_IF_ERROR_RETURN
            (READ_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tid, &trunk_bitmap_entry));
        soc_mem_pbmp_field_get(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
                               TRUNK_BITMAPf, &mc_addr.pbmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Add the trunk group's ports to the multicast group. */
    return bcm_esw_mcast_port_add(unit, &mc_addr);
}

/*
 * Function:
 *      _bcm_th3_trunk_swfailover_init
 * Purpose:
 *      Allocate and initialize _th3_trunk_swfail
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_th3_trunk_swfailover_init(int unit)
{
    int trunk_num_groups;

    if (_th3_trunk_swfail[unit] == NULL) {
        _th3_trunk_swfail[unit] = sal_alloc(sizeof(_th3_trunk_swfail_t), 
                "_th3_trunk_swfail");
        if (_th3_trunk_swfail[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_swfail[unit], 0, sizeof(_th3_trunk_swfail_t));
    /* Only need to count FAST_TRUNK_PORTS_1m because
     * FAST_TRUNK_PORTS_2m is extension of the same LAG */
    trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_PORTS_1m);
    if (_th3_trunk_swfail[unit]->tinfo == NULL) {
        _th3_trunk_swfail[unit]->tinfo =
            sal_alloc(sizeof(_th3_tinfo_t) * trunk_num_groups,
                    "_th3_trunk_swfail_tinfo");
        if (_th3_trunk_swfail[unit]->tinfo == NULL) {
            _bcm_th3_trunk_swfailover_deinit(unit);
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_th3_trunk_swfail[unit]->tinfo, 0, 
               sizeof(_th3_tinfo_t) * trunk_num_groups);
                                                                                
    /* Note: The maximum number of member ports in a trunk group is 64 in TH3.
     *       But it would be quite wasteful to allocate 64 entries to each 
     *       _th3_trunk_swfail[unit]->tinfo[x].port array. The port
     *       array will be allocated as member ports are added to trunk group.
     */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_deinit
 * Purpose:
 *      Deallocate global data structures
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
void
_bcm_th3_trunk_deinit(int unit)
{
    _bcm_th3_trunk_member_info_deinit(unit);

    _bcm_th3_trunk_port_map_deinit(unit);


    _bcm_th3_trunk_hwfailover_deinit(unit);

    _bcm_th3_trunk_swfailover_deinit(unit);
}
/*
 * Function:
 *      _bcm_th3_trunk_init
 * Purpose:
 *      Initialize global data structures
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_init(int unit)
{
    int rv;

    _bcm_th3_trunk_deinit(unit);

    rv = _bcm_th3_trunk_member_info_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_th3_trunk_member_info_deinit(unit);
        return rv;
    }

    rv = _bcm_th3_trunk_port_map_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_th3_trunk_deinit(unit);
        return rv;
    }

    rv = _bcm_th3_trunk_hwfailover_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_th3_trunk_deinit(unit);
        return rv;
    }

    rv = _bcm_th3_trunk_swfailover_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_th3_trunk_deinit(unit);
        return rv;
    }

    return BCM_E_NONE;
}
/* Function:
 *      _bcm_th3_trunk_psc_to_rtag
 * Purpose:
 *      Convert PSC to RTAG
 * Parameters:
 *      unit - (IN) unit number
 *      psc  - (IN) Port selection criteria
 *      rtag - (OUT) Pointer to RTAG
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_th3_trunk_psc_to_rtag(int unit, int psc, int *rtag)
{
    switch (psc) {
    case BCM_TRUNK_PSC_SRCMAC:
    case BCM_TRUNK_PSC_DSTMAC:
    case BCM_TRUNK_PSC_SRCDSTMAC:
    case BCM_TRUNK_PSC_SRCIP:
    case BCM_TRUNK_PSC_DSTIP:
    case BCM_TRUNK_PSC_SRCDSTIP:
        /* These BCM_TRUNK_PSC_ values match the hardware RTAG values */
        *rtag = psc;
        break;
    case BCM_TRUNK_PSC_PORTINDEX:
        if (!soc_feature(unit, soc_feature_port_trunk_index)) {
            return BCM_E_PARAM;
        }
        *rtag = 7;
        break;
    case BCM_TRUNK_PSC_RANDOMIZED:
        *rtag = 0;
        break;
    case BCM_TRUNK_PSC_PORTFLOW:
        if (!soc_feature(unit, soc_feature_port_flow_hash)) {
            return BCM_E_PARAM;
        }
        *rtag = 7;
        break;
    case BCM_TRUNK_PSC_DYNAMIC:
    case BCM_TRUNK_PSC_DYNAMIC_ASSIGNED:
    case BCM_TRUNK_PSC_DYNAMIC_OPTIMAL:
        *rtag = 7;
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
               return BCM_E_UNAVAIL;
        } else 
#endif
        if (SOC_IS_ENDURO(unit)) {
           if (!soc_feature(unit, soc_feature_lag_dlb)) {
               return BCM_E_UNAVAIL;
           }
        }
       break;
    default:
        return BCM_E_PARAM;
    }

    if ((*rtag == 7) && 
        (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit))) {
        /* Hurricanex/Greyhound don't support enhanced hashing */
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_nuc_tpbm_get
 * Purpose:
 *      Get non-unicast panel trunk members.
 * Parameters:
 *      unit      - (IN) SOC unit number. 
 *      num_ports - (IN) Number of ports in trunk group.
 *      tm        - (IN) Pointer to array of module IDs.
 *      nuc_tpbm  - (OUT) Bitmap of ports eligible for non-unicast.
 * Returns:
 *      BCM_E_xxx
 */
STATIC
int _bcm_th3_nuc_tpbm_get(int unit,
                      int num_ports,
                      bcm_module_t *tm,
                      SHR_BITDCL *nuc_tpbm)
{
    int    rv = BCM_E_NONE;
    int    i, mod = -1;
    uint32 mod_type;
    int    all_equal = 1;

    SHR_BITDCL *xgs12_tpbm = NULL;
    SHR_BITDCL *xgs3_tpbm = NULL;
    SHR_BITDCL *unknown_tpbm = NULL;

    SHR_BITDCL xgs12_tpbm_result;
    SHR_BITDCL xgs3_tpbm_result;
    SHR_BITDCL unknown_tpbm_result;

    xgs12_tpbm = sal_alloc(SHR_BITALLOCSIZE(num_ports), "xgs12_tpbm");
    if (xgs12_tpbm == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(xgs12_tpbm, 0, SHR_BITALLOCSIZE(num_ports));

    xgs3_tpbm = sal_alloc(SHR_BITALLOCSIZE(num_ports), "xgs3_tpbm");
    if (xgs3_tpbm == NULL) {
        sal_free(xgs12_tpbm);
        return BCM_E_MEMORY;
    }
    sal_memset(xgs3_tpbm, 0, SHR_BITALLOCSIZE(num_ports));

    unknown_tpbm = sal_alloc(SHR_BITALLOCSIZE(num_ports), "unknown_tpbm");
    if (unknown_tpbm == NULL) {
        sal_free(xgs12_tpbm);
        sal_free(xgs3_tpbm);
        return BCM_E_MEMORY;
    }
    sal_memset(unknown_tpbm, 0, SHR_BITALLOCSIZE(num_ports));

    SHR_BITSET(nuc_tpbm, 0);
    for (i = 0; i < num_ports; i++) {
        if (i == 0) {
            mod = tm[i];
        } else if (mod != tm[i]) {
            all_equal = 0;
        }
        rv = _bcm_switch_module_type_get(unit, tm[i], &mod_type);
        if (BCM_FAILURE(rv)) {
            sal_free(xgs12_tpbm);
            sal_free(xgs3_tpbm);
            sal_free(unknown_tpbm);
            return rv;
        }
        switch(mod_type) {
            case BCM_SWITCH_MODULE_XGS1:
                /* passthru */
            case BCM_SWITCH_MODULE_XGS2:
                SHR_BITSET(xgs12_tpbm, i);
                break;
            case BCM_SWITCH_MODULE_XGS3:
                SHR_BITSET(xgs3_tpbm, i);
                break;
            case BCM_SWITCH_MODULE_UNKNOWN:
                /* passthru */
            default:
                SHR_BITSET(unknown_tpbm, i);
                break;
        }
    }

    /* tpbm_result will be 0 if all bits in the range are 0 */
    SHR_BITTEST_RANGE(xgs12_tpbm, 0, num_ports, xgs12_tpbm_result);
    SHR_BITTEST_RANGE(xgs3_tpbm, 0, num_ports, xgs3_tpbm_result);
    SHR_BITTEST_RANGE(unknown_tpbm, 0, num_ports, unknown_tpbm_result);

    if (all_equal) {
        SHR_BITSET_RANGE(nuc_tpbm, 0, num_ports);
    } else if (xgs12_tpbm_result || unknown_tpbm_result) {
        SHR_BITSET(nuc_tpbm, 0);
    } else if (xgs3_tpbm_result) {
        SHR_BITCOPY_RANGE(nuc_tpbm, 0, xgs3_tpbm, 0, num_ports);
    }

    sal_free(xgs12_tpbm);
    sal_free(xgs3_tpbm);
    sal_free(unknown_tpbm);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_trunk_set_port_property
 * Purpose:
 *      Mark a mod/port pair as being in a particular trunk (or not)
 *      A negative tid sets the mod/port back to non-trunk status.
 */
STATIC int
_bcm_th3_trunk_set_port_property(int unit,
                                  bcm_module_t mod,
                                  bcm_port_t port,
                                  int tid)
{
    source_trunk_map_table_entry_t  stm;
    soc_mem_t        mem;
    int              idx, rv, ptype_get, ptype_set, tid_get, tid_set;

    if (tid < 0) {
        ptype_set = 0;                /* normal port */
        tid_set = 0;
    } else {
        ptype_set = 1;                /* trunk port */
        tid_set = tid;
    }
/*
    BCM_IF_ERROR_RETURN
        (_bcm_esw_src_mod_port_table_index_get(unit, mod, port, &idx));
        */
    idx = port;

    mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &stm);
    if (BCM_SUCCESS(rv)) {
        ptype_get = soc_mem_field32_get(unit, mem, (uint32 *)&stm, PORT_TYPEf);
        tid_get = soc_mem_field32_get(unit, mem, (uint32 *)&stm, TGIDf);
        if ((ptype_get != ptype_set) || (tid_get != tid_set)) {
            soc_mem_field32_set(unit, mem, (uint32 *)&stm,
                                PORT_TYPEf, ptype_set);
            soc_mem_field32_set(unit, mem, (uint32 *)&stm,
                                TGIDf, tid_set);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &stm);
        }
    }
    soc_mem_unlock(unit, mem);

    /* Update SW module, port to tid mapping. */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_th3_trunk_port_map_set(unit, mod, port, tid);  
    }

    return rv;
}

/*
 * Function:
 *      _bcm_th3_trunk_block_mask
 * Purpose:
 *      Update the trunk block mask table.
 *      Old ports are cleared and new ports are added to each entry.
 *      Each hash index location entry is owned by a specific unit/modid
 *      across all units in a stack. Hash index location entry owned by
 *      local unit/modid is provided by my_modid_bmap. member_count is
 *      the number of NUC forwarding eligible ports in the trunk group.
 *      Each eligible entry also has a rolling port that is not masked
 *      from among the new NUC forwarding eligible ports (nuc_ports).
 *      This is what spreads non-unicast traffic across multiple ports.
 */
STATIC int
_bcm_th3_trunk_block_mask(int unit,
                           int nuc_type,
                           pbmp_t old_ports,
                           pbmp_t new_ports,
                           int *localport_array,
                           int localport_count,
                           SHR_BITDCL *my_modid_bmap,
                           uint32 member_count,
                           uint32 flags)
{
    int                   i, imin, imax;
    int                   rv, index_count, region_size;
    soc_mem_t             mem;
    nonucast_trunk_block_mask_entry_t *mask_entry;
    nonucast_trunk_block_mask_entry_t *mask_table;
    bcm_pbmp_t            pbmp, old_pbmp;

    rv = BCM_E_NONE;

    /* On TH3 everything should be local port because there is no stacking
     * or HiGig
    if (!((localport_count > 0) && (localport_count == member_count))) {
        return BCM_E_PARAM;
    }*/

    /* The NONUCAST_TRUNK_BLOCK_MASK table is divided into 4 regions:
     * IPMC, L2MC, broadcast, and unknown unicast/multicast.
     */
    mem = NONUCAST_TRUNK_BLOCK_MASKm;
    region_size = soc_mem_index_count(unit, mem) / 4;
    if (nuc_type == TRUNK_BLOCK_MASK_TYPE_IPMC) {
        imin = 0;
    } else if (nuc_type == TRUNK_BLOCK_MASK_TYPE_L2MC) {
        imin = region_size;
    } else if (nuc_type == TRUNK_BLOCK_MASK_TYPE_BCAST) {
        imin = region_size * 2;
    } else if (nuc_type == TRUNK_BLOCK_MASK_TYPE_DLF) {
        imin = region_size * 3;
    } else {
        return BCM_E_PARAM;
    }

    imax = imin + region_size - 1;

    mask_table = soc_cm_salloc(unit,
                            sizeof(nonucast_trunk_block_mask_entry_t) *
                            region_size,
                            "TH3 Block of NONUCAST_TRUNK_BLOCK_MASK entries");

    if (NULL == mask_table) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, mem);

    if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            imin, imax, mask_table)) < 0) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, mask_table);
        return rv;
    }

    index_count = 0;
    for (i = imin; i <= imax; i++) {
        mask_entry = soc_mem_table_idx_to_pointer(unit, mem,
                                                nonucast_trunk_block_mask_entry_t *,
                                                mask_table, i - imin);
        soc_mem_pbmp_field_get(unit, mem, mask_entry, BLOCK_MASKf, &pbmp);
        SOC_PBMP_ASSIGN(old_pbmp, pbmp);
        SOC_PBMP_REMOVE(pbmp, old_ports);

        if (nuc_type == TRUNK_BLOCK_MASK_TYPE_IPMC &&
                (flags & BCM_TRUNK_FLAG_IPMC_CLEAVE)) {
            /* If the flag BCM_TRUNK_FLAG_IPMC_CLEAVE is set,
             * IPMC trunk resolution is disabled, and the bits in
             * pbmp corresponding to new_ports are not set.
             */
        } else {
            SOC_PBMP_OR(pbmp, new_ports);
            if (localport_count > 0) {
                /* All ports in TH3 are going to be local.
                 * Assuming onlhy one region for now, but may need to change to
                 * reflect: TRUNK_BLOCK_MASK_TYPE_IPMC, TRUNK_BLOCK_MASK_TYPE_L2MC,
                 * etc. Compare with _bcm_trident_trunk_block_mask
                 */
                if (SHR_BITGET(my_modid_bmap,
                            ((i % region_size) % member_count))) {
                    SOC_PBMP_PORT_REMOVE(
                            pbmp, localport_array[index_count % localport_count]);
                    index_count++;
                }
            }
        }
        if (SOC_PBMP_EQ(pbmp, old_pbmp)) {
            continue;
        }
        soc_mem_pbmp_field_set(unit, mem, mask_entry, BLOCK_MASKf, &pbmp);
    }
    rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ALL, imin, imax, mask_table);
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, mask_table);

    return rv;
}

/* Function:
 *      _bcm_th3_trunk_failover_set
 * Purpose:
 *      Disable hardware failover for old trunk ports, and configure
 *      hardware failover for new trunk ports.
 *
 *      This function first deletes any existing trunk failover setting.
 *      It then frees that SDK memory up and reallocates it for
 *      the passed in new failover settings (add_info).
 *
 *      Any existing failover settings on the passed in trunk id will
 *      be deleted.
 * Parameters:
 *      unit        - (IN) SOC unit number
 *      tid         - (IN) Trunk group ID
 *      hg_trunk    - (IN) Indicates if HiGig Trunk, (NO FOR TH3)
 *      add_info    - (IN) Pointer to trunk add info structure
 *  Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_trunk_failover_set(int unit,
                            bcm_trunk_t tid,
                            int hg_trunk,
                            _esw_trunk_add_info_t *add_info)
{
    int trunk_index, failover_flags, num_hw_fail_ports, rv;
    _th3_hw_tinfo_t *hw_fail_trunk;
    bcm_port_t trunk_port;
    bcm_port_t *port_array = NULL;

    failover_flags = 0;
    if (add_info != NULL) {
        if (add_info->flags & BCM_TRUNK_FLAG_FAILOVER_NEXT) {
            failover_flags = BCM_TRUNK_FLAG_FAILOVER_NEXT;
        } else if (add_info->flags & BCM_TRUNK_FLAG_FAILOVER_NEXT_LOCAL) {
            failover_flags = BCM_TRUNK_FLAG_FAILOVER_NEXT_LOCAL;
        } else if (add_info->flags & BCM_TRUNK_FLAG_FAILOVER_ALL) {
            failover_flags = BCM_TRUNK_FLAG_FAILOVER_ALL;
        } else if (add_info->flags & BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL) {
            failover_flags = BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL;
        }
    } 
    
    /* No stacking, so no need to calculate trunk info index with
     * HG.  Just use trunk id passed in */
    hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);

    /* Delete the existing hardware failover configuration */
    if ((NULL == add_info) || (0 != failover_flags)) {
        /*Disable hardware failover for old trunk ports */
        for (trunk_index = 0;
             trunk_index < hw_fail_trunk->num_ports;
             trunk_index++) {
            /* in previous implementations, the lower 8 bits
             * were the port and the upper 8 bits were modid.
             * No modid since no stacking
             */
            trunk_port = hw_fail_trunk->port[trunk_index] & 0xff;

            BCM_IF_ERROR_RETURN
                (_bcm_th3_trunk_hwfailover_set(unit, tid, trunk_port,
                                               hw_fail_trunk->psc[trunk_index],
                                               0, 0, NULL));
        }
    }

    /* Clean up the hardware trunk failover structure */
    if (hw_fail_trunk->port) {
        sal_free(hw_fail_trunk->port);
        hw_fail_trunk->port = NULL;
    }
    if (hw_fail_trunk->psc) {
        sal_free(hw_fail_trunk->psc);
        hw_fail_trunk->psc = NULL;
    }
    if (hw_fail_trunk->flags) {
        sal_free(hw_fail_trunk->flags);
        hw_fail_trunk->flags = NULL;
    }
    hw_fail_trunk->num_ports = 0;

    /* Done if we're just deleting the trunk */
    if (NULL == add_info) {
        return BCM_E_NONE;
    }


    /* Add new trunk data */

    /* Allocate space for the ports */
    if (hw_fail_trunk->port == NULL) {
        hw_fail_trunk->port = sal_alloc(sizeof(uint16) * add_info->num_ports,
                        "hw_tinfo_port");
        if (hw_fail_trunk->port == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(hw_fail_trunk->port, 0, sizeof(uint16) * add_info->num_ports);

    /* Allocate space for the port selection criteria */
    if (hw_fail_trunk->psc == NULL) {
        hw_fail_trunk->psc = sal_alloc(sizeof(uint8) * add_info->num_ports,
                        "hw_tinfo_psc");
        if (hw_fail_trunk->psc == NULL) {
            sal_free(hw_fail_trunk->port);
            hw_fail_trunk->port = NULL;
            return BCM_E_MEMORY;
        }
    }
    sal_memset(hw_fail_trunk->psc, 0, sizeof(uint8) * add_info->num_ports);

    /* Allocate space for flags */
    if (hw_fail_trunk->flags == NULL) {
        hw_fail_trunk->flags = sal_alloc(sizeof(uint32) * add_info->num_ports,
                        "hw_tinfo_flags");
        if (hw_fail_trunk->flags == NULL) {
            sal_free(hw_fail_trunk->port);
            hw_fail_trunk->port = NULL;
            sal_free(hw_fail_trunk->psc);
            hw_fail_trunk->psc = NULL;
            return BCM_E_MEMORY;
        }
    }
    sal_memset(hw_fail_trunk->flags, 0, sizeof(uint32) * add_info->num_ports);

    /* Allocate space for local port array, to be freed at end of function */
    if (port_array == NULL) {
        port_array = sal_alloc(sizeof(bcm_port_t) * add_info->num_ports,
                        "port_array");
        if (port_array == NULL) {
            sal_free(hw_fail_trunk->port);
            hw_fail_trunk->port = NULL;
            sal_free(hw_fail_trunk->psc);
            hw_fail_trunk->psc = NULL;
            sal_free(hw_fail_trunk->flags);
            hw_fail_trunk->flags = NULL;
            return BCM_E_MEMORY;
        }
    }
    sal_memset(port_array, 0, sizeof(bcm_port_t) * add_info->num_ports);

    /* Copy info from the add_info into the local hardware failover
     * structure */
    num_hw_fail_ports = 0;
    for(trunk_index = 0; trunk_index < add_info->num_ports; trunk_index++) {
        if (add_info->member_flags[trunk_index] &
                BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            continue;
        }

        hw_fail_trunk->flags[num_hw_fail_ports] = failover_flags;
        hw_fail_trunk->psc[num_hw_fail_ports] = add_info->psc;
        hw_fail_trunk->port[num_hw_fail_ports] = add_info->tp[trunk_index];

        /* Add all the passed in ports to this array. If there are any
         * flags set for a port, it is updated, else a port from
         * this array is added. */
        port_array[num_hw_fail_ports] = add_info->tp[trunk_index];

        num_hw_fail_ports++;
    }

    hw_fail_trunk->num_ports = num_hw_fail_ports;

    /* No flags in trunk_set means no failover changes */
    if (0 != failover_flags) {
        /* Configure hardware failover for new trunk ports */
        for (trunk_index = 0; trunk_index < hw_fail_trunk->num_ports; 
                trunk_index++) {

            /* modport is always 0 and there is no HiGig, and no
             * cascaded physical ports, so just calling to set
             * hwfailover
             */
            rv = _bcm_th3_trunk_hwfailover_set(unit, tid,
                    hw_fail_trunk->port[trunk_index],
                    hw_fail_trunk->psc[trunk_index],
                    hw_fail_trunk->flags[trunk_index],
                    hw_fail_trunk->num_ports,
                    port_array);
            if (BCM_FAILURE(rv)) {
                sal_free(port_array);
                return rv;
            }
        }
    }

    sal_free(port_array);
    
    return BCM_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_th3_trunk_member_info_set
 * Purpose:
 *      Set trunk member info - used by trunk_reinit
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      num_ports - (IN) Number of trunk members. 
 *      member_port  - (IN) Trunk member port ID array.
 *      member_flags - (IN) Trunk member flags array.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_member_info_set(int unit, bcm_trunk_t tid,
              uint16 num_ports, uint8  *member_port, uint32 *member_flags)
{
    int i, rv;
    bcm_module_t mod = 0;
    /* Save new trunk member info. Free up the old memory and allocate new space
     * since there may be a different number of entries. */

    if (MEMBER_INFO(unit, tid).port) {
        sal_free(MEMBER_INFO(unit, tid).port);
        MEMBER_INFO(unit, tid).port = NULL;
    }
    if (MEMBER_INFO(unit, tid).member_flags) {
        sal_free(MEMBER_INFO(unit, tid).member_flags);
        MEMBER_INFO(unit, tid).member_flags = NULL;
    }

    MEMBER_INFO(unit, tid).port = sal_alloc(
            sizeof(uint16) * num_ports, "member info port");
    if (NULL == MEMBER_INFO(unit, tid).port) {
        return BCM_E_MEMORY ;
    }

    MEMBER_INFO(unit, tid).member_flags = sal_alloc(
            sizeof(uint32) * num_ports, "member info flags");
    if (NULL == MEMBER_INFO(unit, tid).member_flags) {
        sal_free(MEMBER_INFO(unit, tid).port);
        MEMBER_INFO(unit, tid).port = NULL;
        return BCM_E_MEMORY ;
    }
    MEMBER_INFO(unit, tid).num_ports = num_ports;

    for (i = 0; i < num_ports; i++) {
        MEMBER_INFO(unit, tid).port[i] = member_port[i];
        MEMBER_INFO(unit, tid).member_flags[i] = member_flags[i];
        /* restore inverted relationship also */
        if ((rv = _bcm_th3_trunk_port_map_set(unit, mod,
                 member_port[i], tid)) != BCM_E_NONE) {
            return rv;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_member_info_get
 * Purpose:
 *      Return tunk and member info - used by trunk_sync
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      t_data - (OUT) carries trunk and member data filled out
 *      t_info - (IN) Internal trunk info.
 * Returns:
 *      BCM_E_xxx
 */
int
 _bcm_th3_trunk_member_info_get(int unit, bcm_trunk_t tid,
                                _esw_trunk_add_info_t *t_data, trunk_private_t *t_info)
{
    int rv = BCM_E_NONE;
    if (t_data->num_ports != 0) {
        rv = _th3_trunk_add_info_member_alloc(t_data, t_data->num_ports);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }
    rv = _bcm_th3_trunk_get(unit, tid, t_data, t_info);
    if (BCM_FAILURE(rv)) {
        _th3_trunk_add_info_member_free(t_data);
    }
    return rv;
}
/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_num_ports_get
 * Purpose:
 *      Get hwfailover fail to ports
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 * Returns:
 *      hwfailover fail to ports
 */
uint16
_bcm_th3_trunk_hwfailover_num_ports_get(int unit, bcm_trunk_t tid)
{
    uint16 num_ports;

    num_ports = _th3_trunk_hwfail[unit]->hw_tinfo[tid].num_ports;

    return num_ports;
}
/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_num_ports_set
 * Purpose:
 *      Set hwfailover fail to ports
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      num_ports - (IN) hwfailover fail to ports
 * Returns:
 *      hwfailover num ports
 */
void
_bcm_th3_trunk_hwfailover_num_ports_set(int unit, bcm_trunk_t tid, uint16 num_ports)
{
    _th3_trunk_hwfail[unit]->hw_tinfo[tid].num_ports = num_ports;
}
/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_flags_get
 * Purpose:
 *      Get hwfailover per port flags
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      port    - (IN) port.
 * Returns:
 *      hwfailover per port flags
 */
uint8
 _bcm_th3_trunk_hwfailover_flags_get(int unit, bcm_trunk_t tid, int port)
{
    uint8 flags;
    _th3_hw_tinfo_t *hw_fail_trunk;

    hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);

    if (NULL != hw_fail_trunk->flags) {
        flags = hw_fail_trunk->flags[port];
    } else {
        flags = 0;
    }

    return flags;
}
/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_flags_set
 * Purpose:
 *      Set hwfailover per port flags
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      port   - (IN) port.
 *      flags  - (IN) hwfailover per port flags.
 * Returns:
 *      None
 */
void
 _bcm_th3_trunk_hwfailover_flags_set(int unit, bcm_trunk_t tid, int port, uint8 flags)
{
    _th3_hw_tinfo_t *hw_fail_trunk;

    hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);

    if (NULL != hw_fail_trunk->flags) {
        hw_fail_trunk->flags[port] = flags;
    }
}
#endif
/*
 * Function:
 *      _bcm_th3_trunk_swfailover_set
 * Purpose:
 *      Set front panel trunk software failover config.
 * Parameters:
 *      unit   - (IN) SOC unit number. 
 *      tid    - (IN) Trunk ID.
 *      rtag   - (IN) Port selection criteria. 
 *      nports - (IN) Number of trunk members. 
 *      mods   - (IN) Trunk member module ID array.
 *      ports  - (IN) Trunk member port ID array.
 *      member_flags - (IN) Trunk member flags array.
 *      flags  - (IN) Trunk group flags.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_trunk_swfailover_set(int unit,
        bcm_trunk_t tid,
        int rtag,
        int nports,
        int *mods,
        int *ports,
        uint32 *member_flags,
        uint32 flags)
{
    _th3_trunk_swfail_t *swf;
    _th3_tinfo_t        *swft;
    bcm_pbmp_t          localports;
    bcm_port_t          port;
    int                 i;
    bcm_pbmp_t          all_pbmp;

    if (_th3_trunk_swfail[unit] == NULL) {
        return BCM_E_INIT;
    }

    swf = _th3_trunk_swfail[unit];
    swft = &swf->tinfo[tid];

    swf->mymodid = 0;
    swft->rtag = rtag;
    swft->num_ports = nports;
    swft->flags = flags;

    if (swft->port) {
        sal_free(swft->port);
        swft->port = NULL;
    }
    if (swft->member_flags) {
        sal_free(swft->member_flags);
        swft->member_flags = NULL;
    }

    BCM_PBMP_CLEAR(localports);
    if (swft->num_ports > 0) {
        swft->port = sal_alloc(sizeof(uint16) * swft->num_ports,
                "swfail_tinfo_port");
        if (swft->port == NULL) {
            return BCM_E_MEMORY;
        }
        swft->member_flags = sal_alloc(sizeof(uint32) * swft->num_ports,
                "swfail_tinfo_member_flags");
        if (swft->member_flags == NULL) {
            sal_free(swft->port);
            swft->port = NULL;
            return BCM_E_MEMORY;
        }

        for (i = 0; i < nports; i++) {
            swft->port[i] = (mods[i] << 8) | ports[i];
            if (mods[i] == 0) {
                BCM_PBMP_PORT_ADD(localports, ports[i]);
            }

            swft->member_flags[i] = member_flags[i];
        }
    }

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

    PBMP_ITER(all_pbmp, port) {
        if (BCM_PBMP_MEMBER(localports, port)) {
            /* coverity[overrun-local : FALSE] */
            swf->trunk[port] = tid + 1;
        } else if (swf->trunk[port] == tid + 1) {
            swf->trunk[port] = 0;       /* remove stale pointers to tid */
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_lag_reinit
 * Purpose:
 *      Initialize global data structures
 * Parameters:
 *      unit - (IN)SOC unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_lag_reinit(int unit, int ngroups_fp, trunk_private_t *trunk_info)
{
    _th3_hw_tinfo_t *hw_fail_trunk;
    int tid;
    int rv = BCM_E_NONE;
    int i;
    int num_ports, rtag;
    bcm_module_t *egr_hw_mods;
    bcm_port_t *egr_hw_ports;

    for (tid = 0; tid < ngroups_fp; tid++, trunk_info++) {
        num_ports = MEMBER_INFO(unit, tid).num_ports;
        egr_hw_mods = sal_alloc(sizeof(bcm_module_t) * num_ports,
                "egr_hw_mods");
        if (egr_hw_mods == NULL) {
            return BCM_E_MEMORY;
        }
    
        egr_hw_ports = sal_alloc(sizeof(bcm_port_t) * num_ports,
                "egr_hw_ports");
        if (egr_hw_ports == NULL) {
            sal_free(egr_hw_mods);
            return BCM_E_MEMORY;
        }
        sal_memset(egr_hw_mods, 0, sizeof(bcm_module_t) * num_ports);
        for (i = 0; i < num_ports; i++) {
	    egr_hw_ports[i] = MEMBER_INFO(unit, tid).port[i];
        }
	rv = _bcm_th3_trunk_psc_to_rtag(unit, trunk_info->psc, &rtag);
        if (BCM_FAILURE(rv)) {
            sal_free(egr_hw_mods);
            sal_free(egr_hw_ports);
            return rv;
        }
        rv = _bcm_th3_trunk_swfailover_set( unit, tid, 
                                  rtag,
                                  num_ports, 
                                  egr_hw_mods,
                                  egr_hw_ports,
                                  MEMBER_INFO(unit, tid).member_flags,
                                  trunk_info->flags);
        sal_free(egr_hw_mods);
        sal_free(egr_hw_ports);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        
        hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);
        hw_fail_trunk->port = sal_alloc(sizeof(uint16) * hw_fail_trunk->num_ports,
                        "hw_tinfo_port");
        if (hw_fail_trunk->port == NULL) { 
                return BCM_E_MEMORY;
        }
        sal_memset(hw_fail_trunk->port, 0, sizeof(uint16) * hw_fail_trunk->num_ports);

        /* Allocate space for the port selection criteria */
        hw_fail_trunk->psc = sal_alloc(sizeof(uint8) * hw_fail_trunk->num_ports,
                        "hw_tinfo_psc");
        if (hw_fail_trunk->psc == NULL) {
            sal_free(hw_fail_trunk->port);
            hw_fail_trunk->port = NULL;
            return BCM_E_MEMORY;
        }
        sal_memset(hw_fail_trunk->psc, 0, sizeof(uint8) * hw_fail_trunk->num_ports);

        /* Allocate space for flags */
        hw_fail_trunk->flags = sal_alloc(sizeof(uint32) * hw_fail_trunk->num_ports,
                        "hw_tinfo_flags");
        if (hw_fail_trunk->flags == NULL) {
            sal_free(hw_fail_trunk->port);
            hw_fail_trunk->port = NULL;
            sal_free(hw_fail_trunk->psc);
            hw_fail_trunk->psc = NULL;
            return BCM_E_MEMORY;
        }
        sal_memset(hw_fail_trunk->flags, 0, sizeof(uint32) * hw_fail_trunk->num_ports);
        for (i = 0; i < hw_fail_trunk->num_ports; i++) {
            hw_fail_trunk->psc[i] = trunk_info->psc;
            hw_fail_trunk->port[i] = MEMBER_INFO(unit, tid).port[i]; 
            hw_fail_trunk->flags[i] = MEMBER_INFO(unit, tid).member_flags[i]; 
        }


    } /*  for tid */

    return rv;
}

#if 0
/*
 * Function:
 *      _bcm_th3_lag_slb_free_resource
 * Purpose:
 *      Free resources for a LAG static load balancing group.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      tid - (IN) Trunk group ID.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_lag_slb_free_resource(int unit, int tid)
{
    trunk_group_entry_t tg_entry;
    int base_ptr;
    int tg_size;
    int rtag;
    int num_entries;
 
    SOC_IF_ERROR_RETURN
        (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
    base_ptr = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            BASE_PTRf);
    tg_size = 1 + soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            TG_SIZEf);
    rtag = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
            RTAGf);

    if (rtag != 0) {
        num_entries = tg_size;
        if (rtag >= 1 && rtag <= 6) {
#ifdef BCM_KATANA_SUPPORT
            if (SOC_IS_KATANAX(unit)) {
                num_entries = _BCM_KATANA_FP_TRUNK_RTAG1_6_MAX_PORTCNT;
            } else
#endif
            if (soc_feature(unit,
                        soc_feature_rtag1_6_max_portcnt_less_than_rtag7)) { 
                num_entries = _BCM_TD_FP_TRUNK_RTAG1_6_MAX_PORTCNT;
            }
        } 

        _BCM_FP_TRUNK_MEMBER_USED_CLR_RANGE(unit, base_ptr, num_entries);
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_th3_lag_slb_set
 * Purpose:
 *      Configure a LAG static load balancing group.
 *      Memories modified: 
 *          FAST_TRUNK_PORTS_1
 *          FAST_TRUNK_PORTS_2
 *          FAST_TRUNK_SIZE
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      tid   - (IN) Trunk group ID.
 *      num_uc_egr_ports - (IN) Number of trunk member ports eligible for
 *                              unicast egress
 *      uc_egr_hwmod  - (IN) Array of module IDs
 *      uc_egr_hwport - (IN) Array of port IDs
 *      t_info        - (IN) Pointer to trunk info.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_lag_slb_set(int unit, int tid, int num_uc_egr_ports,
        int *uc_egr_hwmod, int *uc_egr_hwport, trunk_private_t *t_info)
{
    int i, pipe;
    uint32 rval;
    uint16 seed = 0;
    soc_reg_t reg;
    fast_trunk_ports_1_entry_t  fast_1;
    fast_trunk_ports_2_entry_t  fast_2;
    fast_trunk_size_entry_t     fast_trunk_size;

    memset(&fast_1, 0, sizeof(fast_trunk_ports_1_entry_t));
    memset(&fast_2, 0, sizeof(fast_trunk_ports_2_entry_t));
    memset(&fast_trunk_size, 0, sizeof(fast_trunk_size_entry_t));

    /* No enabled ports in the trunk. Clear it all out */
    if (num_uc_egr_ports == 0) {

        /* Clear out all 64 ports in this trunk */
        for (i = 0; i < _BCM_TH3_PORTS_PER_TRUNK_MAX; i++) {
            /* Clear the first 32 Fast LAG Ports */
            if (i < _BCM_TH3_PORTS_PER_TRUNK_MAX / 2) {
                soc_FAST_TRUNK_PORTS_1m_field32_set(unit, &fast_1,
                        _th3_fast_portf[i], 0);
            } else {
                /* Clear the last 32 Fast LAG Ports */
                soc_FAST_TRUNK_PORTS_2m_field32_set(unit, &fast_2,
                        _th3_fast_portf[i], 0);
            }
        }

        /* Set the size of the Fast LAG to 0 and reset the trunk mode */
        soc_FAST_TRUNK_SIZEm_field32_set(unit, &fast_trunk_size,
                                        TRUNK_MODEf, 0);
        soc_FAST_TRUNK_SIZEm_field32_set(unit, &fast_trunk_size,
                                        TG_SIZEf, 0);

    } else {
        /* Fill in the Fast LAG Ports */
        for (i = 0; i < num_uc_egr_ports; i++) {
            /* Set the first 32 Fast LAG Ports */
            if (i < _BCM_TH3_PORTS_PER_TRUNK_MAX / 2) {
                soc_FAST_TRUNK_PORTS_1m_field32_set(unit, &fast_1,
                        _th3_fast_portf[i], uc_egr_hwport[i]);
            } else {
                /* Set the last 32 Fast LAG Ports */
                soc_FAST_TRUNK_PORTS_2m_field32_set(unit, &fast_2,
                        _th3_fast_portf[i], uc_egr_hwport[i]);
            }
        }

        
        soc_FAST_TRUNK_SIZEm_field32_set(unit, &fast_trunk_size, TG_SIZEf,
                num_uc_egr_ports - 1);

        /* Set the Trunk Mode */
        if (t_info->psc == BCM_TRUNK_PSC_RANDOMIZED) {
            /* Random */
            soc_FAST_TRUNK_SIZEm_field32_set(unit, &fast_trunk_size,
                                            TRUNK_MODEf, 0x1);
           /* Configure Rand LB Seed */
            seed = (sal_rand()) % 0xFFFF;
            rval = 0;
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                reg = SOC_REG_UNIQUE_ACC(unit, TRUNK_RAND_LB_SEEDr)[pipe];
                soc_reg_field_set(unit, reg, &rval, SEEDf, seed);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }

        } else {
            /* Hash */
            soc_FAST_TRUNK_SIZEm_field32_set(unit, &fast_trunk_size,
                                            TRUNK_MODEf, 0x0);
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_FAST_TRUNK_PORTS_1m(unit, MEM_BLOCK_ALL, tid, &fast_1));
    SOC_IF_ERROR_RETURN
        (WRITE_FAST_TRUNK_PORTS_2m(unit, MEM_BLOCK_ALL, tid, &fast_2));
    SOC_IF_ERROR_RETURN
        (WRITE_FAST_TRUNK_SIZEm(unit, MEM_BLOCK_ALL, tid, &fast_trunk_size));

#if 0



    int i;
    int num_entries;
    int max_base_ptr;
    int base_ptr;
    SHR_BITDCL occupied;
    trunk_member_entry_t trunk_member_entry;
    trunk_group_entry_t  tg_entry;
#ifdef BCM_TOMAHAWK_SUPPORT
    trunk_rr_cnt_entry_t rrlb_cnt_entry;
    uint16 seed=0;
    soc_reg_t reg;
    uint32 rval;
    int pipe;
#endif

    if (num_uc_egr_ports == 0) {
        SOC_IF_ERROR_RETURN
            (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, BASE_PTRf, 0);
        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf, 0);
        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, 0);
        SOC_IF_ERROR_RETURN
            (WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ALL, tid, &tg_entry));
        return BCM_E_NONE;
    }

    /* Find a contiguous region of TRUNK_MEMBERm table that's large
     * enough to hold all of the trunk group's egress member ports.
     */

    num_entries = num_uc_egr_ports;

    /* If RTAG is 1-6, Trident A0 and Katana ignore the TG_SIZE field and 
     * assume the trunk group occupies FP_TRUNK_RTAG1_6_MAX_PORTCNT
     * contiguous entries in TRUNK_MEMBER table.
     */
    if (t_info->rtag >= 1 && t_info->rtag <= 6) {
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            num_entries = _BCM_KATANA_FP_TRUNK_RTAG1_6_MAX_PORTCNT;
        } else
#endif
        if (soc_feature(unit,
                    soc_feature_rtag1_6_max_portcnt_less_than_rtag7)) { 
            num_entries = _BCM_TD_FP_TRUNK_RTAG1_6_MAX_PORTCNT;
        }
    } 

    max_base_ptr = soc_mem_index_count(unit, TRUNK_MEMBERm) - num_entries;
    for (base_ptr = 0; base_ptr <= max_base_ptr; base_ptr++) {
        /* Check if the contiguous region of TRUNK_MEMBERm table from
         * index base_ptr to (base_ptr + num_entries - 1) is free. 
         */
        _BCM_FP_TRUNK_MEMBER_TEST_RANGE(unit, base_ptr, num_entries, occupied); 
        if (!occupied) {
            break;
        }
    }

    if (base_ptr > max_base_ptr) {
        /* A contiguous region of the desired size could not be found in
         * TRUNK_MEMBERm table.
         */
        return BCM_E_RESOURCE;
    }

    for (i = 0; i < num_entries; i++) {
        sal_memset(&trunk_member_entry, 0, sizeof(trunk_member_entry_t));
        soc_TRUNK_MEMBERm_field32_set
            (unit, &trunk_member_entry, MODULE_IDf,
             uc_egr_hwmod[i % num_uc_egr_ports]);
        soc_TRUNK_MEMBERm_field32_set
            (unit, &trunk_member_entry, PORT_NUMf,
             uc_egr_hwport[i % num_uc_egr_ports]);
        SOC_IF_ERROR_RETURN(WRITE_TRUNK_MEMBERm(unit, MEM_BLOCK_ALL,
                    base_ptr + i, &trunk_member_entry)); 
    }

    /* Set fp_trunk_member_bitmap */
    _BCM_FP_TRUNK_MEMBER_USED_SET_RANGE(unit, base_ptr, num_entries);

    /* Update TRUNK_GROUP table */

    SOC_IF_ERROR_RETURN
        (READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tid, &tg_entry));
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, BASE_PTRf, base_ptr);
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf,
            num_uc_egr_ports-1);
    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, t_info->rtag);

#ifdef BCM_APACHE_SUPPORT
    if (t_info->psc == BCM_TRUNK_PSC_ROUND_ROBIN) {
        if (soc_feature(unit, soc_feature_apache_round_robin_fp_lag)) {
           soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RR_LB_ENABLEf, 0x1);
           soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RR_LB_COUNTER_SELECTf, t_info->rr_lag_select);
        }
    }
#endif /* BCM_APACHE_SUPPORT  */

#ifdef BCM_TOMAHAWK_SUPPORT
    if (t_info->psc == BCM_TRUNK_PSC_ROUND_ROBIN) {
        if (soc_feature(unit, soc_feature_round_robin_load_balance)) {
           soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TRUNK_MODEf, 0x2);

            BCM_IF_ERROR_RETURN(READ_TRUNK_RR_CNTm(unit, MEM_BLOCK_ANY,
                    tid, &rrlb_cnt_entry));
            soc_TRUNK_RR_CNTm_field32_set(unit, &rrlb_cnt_entry,
                            RRLB_CNTf, 0);
            BCM_IF_ERROR_RETURN(WRITE_TRUNK_RR_CNTm(unit, MEM_BLOCK_ALL,
                            tid, &rrlb_cnt_entry));
        }
    } else if (t_info->psc == BCM_TRUNK_PSC_RANDOMIZED) {
        if (soc_feature(unit, soc_feature_randomized_load_balance)) {
           soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TRUNK_MODEf, 0x1);
           /* Configure Rand LB Seed */
            seed = (sal_rand()) % 0xFFFF;
            rval = 0;
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                reg = SOC_REG_UNIQUE_ACC(unit, TRUNK_RAND_LB_SEEDr)[pipe];
                soc_reg_field_set(unit, reg, &rval, SEEDf, seed);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }
    }
#endif

    SOC_IF_ERROR_RETURN
        (WRITE_TRUNK_GROUPm(unit, MEM_BLOCK_ALL, tid, &tg_entry));
#endif
    return BCM_E_NONE;
}


/* Macro to free memory and return code */

#define _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(_r_) \
    if (nuc_tpbm) {                                    \
        sal_free(nuc_tpbm);                            \
    }                                                  \
    if (ipmc_tpbm) {                                   \
        sal_free(ipmc_tpbm);                           \
    }                                                  \
    if (l2mc_tpbm) {                                   \
        sal_free(l2mc_tpbm);                           \
    }                                                  \
    if (bcast_tpbm) {                                  \
        sal_free(bcast_tpbm);                          \
    }                                                  \
    if (dlf_tpbm) {                                    \
        sal_free(dlf_tpbm);                            \
    }                                                  \
    if (ipmc_my_modid_bmap) {                          \
        sal_free(ipmc_my_modid_bmap);                  \
    }                                                  \
    if (l2mc_my_modid_bmap) {                          \
        sal_free(l2mc_my_modid_bmap);                  \
    }                                                  \
    if (bcast_my_modid_bmap) {                         \
        sal_free(bcast_my_modid_bmap);                 \
    }                                                  \
    if (dlf_my_modid_bmap) {                           \
        sal_free(dlf_my_modid_bmap);                   \
    }                                                  \
    if (hwmod) {                                       \
        sal_free(hwmod);                               \
    }                                                  \
    if (hwport) {                                      \
        sal_free(hwport);                              \
    }                                                  \
    if (egr_hwmod) {                                   \
        sal_free(egr_hwmod);                           \
    }                                                  \
    if (egr_hwport) {                                  \
        sal_free(egr_hwport);                          \
    }                                                  \
    if (egr_member_flags) {                            \
        sal_free(egr_member_flags);                    \
    }                                                  \
    if (egr_scaling_factor) {                          \
        sal_free(egr_scaling_factor);                  \
    }                                                  \
    if (egr_load_weight) {                             \
        sal_free(egr_load_weight);                     \
    }                                                  \
    if (egr_qsize_weight) {                            \
        sal_free(egr_qsize_weight);                    \
    }                                                  \
    if (uc_egr_hwmod) {                                \
        sal_free(uc_egr_hwmod);                        \
    }                                                  \
    if (uc_egr_hwport) {                               \
        sal_free(uc_egr_hwport);                       \
    }                                                  \
    if (uc_egr_scaling_factor) {                       \
        sal_free(uc_egr_scaling_factor);               \
    }                                                  \
    if (uc_egr_load_weight) {                          \
        sal_free(uc_egr_load_weight);                  \
    }                                                  \
    if (uc_egr_qsize_weight) {                         \
        sal_free(uc_egr_qsize_weight);                 \
    }                                                  \
    if (leaving_members) {                             \
        sal_free(leaving_members);                     \
    }                                                  \
    if (staying_members) {                             \
        sal_free(staying_members);                     \
    }                                                  \
    if (joining_members) {                             \
        sal_free(joining_members);                     \
    }                                                  \
    if (ipmc_localport_array) {                        \
        sal_free(ipmc_localport_array);                \
    }                                                  \
    if (l2mc_localport_array) {                        \
        sal_free(l2mc_localport_array);                \
    }                                                  \
    if (bcast_localport_array) {                       \
        sal_free(bcast_localport_array);               \
    }                                                  \
    if (dlf_localport_array) {                         \
        sal_free(dlf_localport_array);                 \
    }                                                  \
    return _r_;                             

/*
 * Function:
 *      _bcm_th3_trunk_set
 * Purpose:
 *      Set trunk members.
 * Parameters:
 *      unit     - (IN) SOC unit number. 
 *      tid      - (IN) Trunk group ID.
 *      add_info - (IN) Pointer to trunk add info structure.
 *      t_info   - (IN) Pointer to trunk info.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_trunk_set(int unit,
        bcm_trunk_t tid,
        _esw_trunk_add_info_t *add_info,
        trunk_private_t *t_info)
{
#if 1
    int                 rv = BCM_E_NONE;
    SHR_BITDCL          *nuc_tpbm = NULL;
    SHR_BITDCL          *ipmc_tpbm = NULL;
    SHR_BITDCL          *l2mc_tpbm = NULL;
    SHR_BITDCL          *bcast_tpbm = NULL;
    SHR_BITDCL          *dlf_tpbm = NULL;
    SHR_BITDCL           *ipmc_my_modid_bmap = NULL;
    SHR_BITDCL           *l2mc_my_modid_bmap = NULL;
    SHR_BITDCL           *bcast_my_modid_bmap = NULL;
    SHR_BITDCL           *dlf_my_modid_bmap = NULL;
    int                  *hwmod = NULL;
    int                  *hwport = NULL;
    int                  num_egr_ports = 0;
    int                  *egr_hwmod = NULL;
    int                  *egr_hwport = NULL;
    uint32               *egr_member_flags = NULL;
    int                  *egr_scaling_factor = NULL;
    int                  *egr_load_weight = NULL;
    int                  *egr_qsize_weight = NULL;
    int                  num_uc_egr_ports = 0;
    int                  *uc_egr_hwmod = NULL;
    int                  *uc_egr_hwport = NULL;
    int                  *uc_egr_scaling_factor = NULL;
    int                  *uc_egr_load_weight = NULL;
    int                  *uc_egr_qsize_weight = NULL;
    int                  negr_disable_port = 0;
    int                  num_uc_egr_disable_port = 0;
    pbmp_t               new_trunk_pbmp;
    uint32               ipmc_member_count;
    uint32               l2mc_member_count;
    uint32               bcast_member_count;
    uint32               dlf_member_count;
    int                  mod, port, mod_out, port_out;
    bcm_gport_t          gport;
    bcm_port_t           loc_port = BCM_GPORT_INVALID;
    int                  i;
    trunk_bitmap_entry_t trunk_bitmap_entry;
    pbmp_t               old_trunk_pbmp;
    int                  j, k, remove_trunk_port;
    int                  num_leaving_members = 0;
    bcm_gport_t          *leaving_members = NULL;
    int                  match_leaving_members;
    int                  num_staying_members = 0;
    bcm_gport_t          *staying_members = NULL;
    int                  match_staying_members;
    int                  num_joining_members = 0;
    bcm_gport_t          *joining_members = NULL;
    int                  match_joining_members;
    int                  is_local;
    int                  is_local_subport = 0;
    int                  *ipmc_localport_array = NULL;
    int                  *l2mc_localport_array = NULL;
    int                  *bcast_localport_array = NULL;
    int                  *dlf_localport_array = NULL;
    int                  ipmc_localport_count;
    int                  l2mc_localport_count;
    int                  bcast_localport_count;
    int                  dlf_localport_count;

    if (add_info->num_ports > _BCM_TH3_PORTS_PER_TRUNK_MAX) {
        return BCM_E_PARAM;
    }

    /* Audit unsupported flag combination */
    for (i = 0; i < add_info->num_ports; i++) {
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_INGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_IPMC_EGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_L2MC_EGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_BCAST_EGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_DLF_EGRESS_DISABLE) {
                return BCM_E_PARAM;
            }
        }
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_IPMC_EGRESS_DISABLE) {
            if (add_info->ipmc_index != BCM_TRUNK_UNSPEC_INDEX) {
                return BCM_E_PARAM;
            }
        }
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_L2MC_EGRESS_DISABLE) {
            if (add_info->mc_index != BCM_TRUNK_UNSPEC_INDEX) {
                return BCM_E_PARAM;
            }
        }
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_BCAST_EGRESS_DISABLE) {
            if (add_info->dlf_index != BCM_TRUNK_UNSPEC_INDEX) {
                return BCM_E_PARAM;
            }
        }
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_DLF_EGRESS_DISABLE) {
            if (add_info->dlf_index != BCM_TRUNK_UNSPEC_INDEX) {
                return BCM_E_PARAM;
            }
        }
    }

    if (add_info->dlf_index >= add_info->num_ports) {
        return BCM_E_PARAM;
    }

#if 1
    /* Derive a member bitmap of non-unicast eligible members */
    nuc_tpbm = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports), "nuc_tpbm");
    if (nuc_tpbm == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(nuc_tpbm, 0, SHR_BITALLOCSIZE(add_info->num_ports));
    if (add_info->dlf_index == -1) {
        rv = _bcm_th3_nuc_tpbm_get(unit, add_info->num_ports, 
                add_info->tm, nuc_tpbm);
        if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
    } else {
        SHR_BITSET(nuc_tpbm, add_info->dlf_index);
    }

    /* Copy the non-unicast eligible member bitmap to IPMC, L2MC, broadcast,
     * and DLF eligible member bitmaps.
     */
    ipmc_tpbm = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports), "ipmc_tpbm");
    if (ipmc_tpbm == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memcpy(ipmc_tpbm, nuc_tpbm, SHR_BITALLOCSIZE(add_info->num_ports));

    l2mc_tpbm = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports), "l2mc_tpbm");
    if (l2mc_tpbm == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memcpy(l2mc_tpbm, nuc_tpbm, SHR_BITALLOCSIZE(add_info->num_ports));

    bcast_tpbm = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports), "bcast_tpbm");
    if (bcast_tpbm == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memcpy(bcast_tpbm, nuc_tpbm, SHR_BITALLOCSIZE(add_info->num_ports));

    dlf_tpbm = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports), "dlf_tpbm");
    if (dlf_tpbm == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memcpy(dlf_tpbm, nuc_tpbm, SHR_BITALLOCSIZE(add_info->num_ports));

    /* Iterate through the trunk member list to get module ID (SHOULD BE 0)
     * and port number of each member, a port bitmap of local members, and
     * port bitmaps of IPMC/L2MC/BCAST/DLF eligible local members.
     */
    hwmod = sal_alloc(sizeof(int) * (add_info->num_ports), "hwmod");
    if (hwmod == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(hwmod, 0, sizeof(int) * (add_info->num_ports));

    hwport = sal_alloc(sizeof(int) * (add_info->num_ports), "hwport");
    if (hwport == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(hwport, 0, sizeof(int) * (add_info->num_ports));

    ipmc_my_modid_bmap = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports),
            "ipmc_my_modid_bmap");
    if (ipmc_my_modid_bmap == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(ipmc_my_modid_bmap, 0, SHR_BITALLOCSIZE(add_info->num_ports));

    l2mc_my_modid_bmap = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports),
            "l2mc_my_modid_bmap");
    if (l2mc_my_modid_bmap == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(l2mc_my_modid_bmap, 0, SHR_BITALLOCSIZE(add_info->num_ports));

    bcast_my_modid_bmap = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports),
            "bcast_my_modid_bmap");
    if (bcast_my_modid_bmap == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(bcast_my_modid_bmap, 0, SHR_BITALLOCSIZE(add_info->num_ports));

    dlf_my_modid_bmap = sal_alloc(SHR_BITALLOCSIZE(add_info->num_ports),
            "dlf_my_modid_bmap");
    if (dlf_my_modid_bmap == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(dlf_my_modid_bmap, 0, SHR_BITALLOCSIZE(add_info->num_ports));
    ipmc_localport_array = sal_alloc(add_info->num_ports * sizeof(int),
                                     "ipmc_localport_array");
    if (ipmc_localport_array == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(ipmc_localport_array, 0, add_info->num_ports * sizeof(int));
    l2mc_localport_array = sal_alloc(add_info->num_ports * sizeof(int),
                                     "l2mc_localport_array");
    if (l2mc_localport_array == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(l2mc_localport_array, 0, add_info->num_ports * sizeof(int));
    bcast_localport_array = sal_alloc(add_info->num_ports * sizeof(int),
                                      "bcast_localport_array");
    if (bcast_localport_array == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(bcast_localport_array, 0, add_info->num_ports * sizeof(int));
    dlf_localport_array = sal_alloc(add_info->num_ports * sizeof(int),
                                    "dlf_localport_array");
    if (dlf_localport_array == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(dlf_localport_array, 0, add_info->num_ports * sizeof(int));

    SOC_PBMP_CLEAR(new_trunk_pbmp);
    ipmc_member_count = 0;
    l2mc_member_count = 0;
    bcast_member_count = 0;
    dlf_member_count = 0;
    negr_disable_port = 0;
    num_uc_egr_disable_port = 0;
    ipmc_localport_count = 0;
    l2mc_localport_count = 0;
    bcast_localport_count = 0;
    dlf_localport_count = 0;

    for (i = 0; i < add_info->num_ports; i++) {
        mod = 0; /* Set modid to 0 */
        port = add_info->tp[i];
        /*In case of feature not supported on chip return E_PARAM*/
        /*if (!soc_feature(unit, soc_feature_shared_trunk_member_table)) {
             _BCM_TRIDENT_TRUNK_FP_SET_FREE_MEM_RETURN(BCM_E_PARAM);
        }*/

        mod_out = mod;
        port_out = port;

        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_BADID);
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_PORT);
        }
        hwmod[i] = mod_out; /* SHOULD BE 0! */
        hwport[i] = port_out;

        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            SHR_BITCLR(ipmc_tpbm, i);
            SHR_BITCLR(l2mc_tpbm, i);
            SHR_BITCLR(bcast_tpbm, i);
            SHR_BITCLR(dlf_tpbm, i);
            negr_disable_port++;
        } else {
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                num_uc_egr_disable_port++;
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_IPMC_EGRESS_DISABLE) {
                SHR_BITCLR(ipmc_tpbm, i);
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_L2MC_EGRESS_DISABLE) {
                SHR_BITCLR(l2mc_tpbm, i);
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_BCAST_EGRESS_DISABLE) {
                SHR_BITCLR(bcast_tpbm, i);
            }
            if (add_info->member_flags[i] &
                    BCM_TRUNK_MEMBER_DLF_EGRESS_DISABLE) {
                SHR_BITCLR(dlf_tpbm, i);
            }
        }

        BCM_GPORT_MODPORT_SET(gport, mod_out, port_out);
        loc_port = BCM_GPORT_INVALID;
        if (BCM_SUCCESS(bcm_esw_port_local_get(unit, gport, &loc_port))) {
#if 0
            if (soc_feature(unit, soc_feature_hgproxy_subtag_coe)) {
#if defined(BCM_HGPROXY_COE_SUPPORT)
                if(_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out,
                                                        port_out)) {

                    is_local_subport = 1;
                }
#endif
            } else

            if (soc_feature(unit, soc_feature_linkphy_coe) ||
                soc_feature(unit, soc_feature_subtag_coe)) {
#if defined(BCM_KATANA2_SUPPORT)
                BCM_IF_ERROR_RETURN(_bcm_kt2_modport_is_local_coe_subport(unit,
                    mod_out, port_out, &is_local_subport));
#endif
            }
            if (!is_local_subport) {
                if (!(IS_E_PORT(unit, loc_port) || IS_HG_PORT(unit, loc_port)) ||
                    /* Cascaded physical ports are not considered as front-panel port */
                    (soc_feature(unit, soc_feature_linkphy_coe) &&
                        IS_LP_PORT(unit, loc_port)) ||
                    (soc_feature(unit, soc_feature_subtag_coe) &&
                        IS_SUBTAG_PORT(unit, loc_port)) || 
                    (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
                     IS_SUBTAG_PORT (unit, loc_port))) {
                    _BCM_TRIDENT_TRUNK_FP_SET_FREE_MEM_RETURN(BCM_E_PARAM);
                }
            }
#endif
            SOC_PBMP_PORT_ADD(new_trunk_pbmp, loc_port);
            if (SHR_BITGET(ipmc_tpbm, i)) {
                ipmc_localport_array[ipmc_localport_count++] = loc_port;
                SHR_BITSET(ipmc_my_modid_bmap, ipmc_member_count);
            }
            if (SHR_BITGET(l2mc_tpbm, i)) {
                l2mc_localport_array[l2mc_localport_count++] = loc_port;
                SHR_BITSET(l2mc_my_modid_bmap, l2mc_member_count);
            }
            if (SHR_BITGET(bcast_tpbm, i)) {
                bcast_localport_array[bcast_localport_count++] = loc_port;
                SHR_BITSET(bcast_my_modid_bmap, bcast_member_count);
            }
            if (SHR_BITGET(dlf_tpbm, i)) {
                dlf_localport_array[dlf_localport_count++] = loc_port;
                SHR_BITSET(dlf_my_modid_bmap, dlf_member_count);
            }
        }

        
        if (SHR_BITGET(ipmc_tpbm, i)) {
            /* Eligible for IPMC forwarding */
            ipmc_member_count++;
        }
        if (SHR_BITGET(l2mc_tpbm, i)) {
            /* Eligible for L2MC forwarding */
            l2mc_member_count++;
        }
        if (SHR_BITGET(bcast_tpbm, i)) {
            /* Eligible for broadcast forwarding */
            bcast_member_count++;
        }
        if (SHR_BITGET(dlf_tpbm, i)) {
            /* Eligible for DLF forwarding */
            dlf_member_count++;
        }
    }

    /* Figure out which trunk ports are eligible for egress and
     * unicast egress
     */
    num_egr_ports = add_info->num_ports - negr_disable_port;
    if (num_egr_ports > 0) {
        egr_hwmod = sal_alloc(sizeof(bcm_module_t) * num_egr_ports,
                "egr_hwmod");
        if (egr_hwmod == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        egr_hwport = sal_alloc(sizeof(bcm_port_t) * num_egr_ports,
                "egr_hwport");
        if (egr_hwport == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        egr_member_flags = sal_alloc(sizeof(uint32) * num_egr_ports,
                "egr_member_flags");
        if (egr_member_flags == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        egr_scaling_factor = sal_alloc(sizeof(int) * num_egr_ports,
                "egr_scaling_factor");
        if (egr_scaling_factor == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        egr_load_weight = sal_alloc(sizeof(int) * num_egr_ports,
                "egr_load_weight");
        if (egr_load_weight == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        egr_qsize_weight = sal_alloc(sizeof(int) * num_egr_ports,
                "egr_qsize_weight");
        if (egr_qsize_weight == NULL) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
        }
        for (i = 0, j = 0; i < add_info->num_ports; i++) {
            if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
                continue;
            }
            egr_hwmod[j] = hwmod[i];
            egr_hwport[j] = hwport[i];
            egr_member_flags[j] = add_info->member_flags[i];
            egr_scaling_factor[j] = add_info->dynamic_scaling_factor[i];
            egr_load_weight[j] = add_info->dynamic_load_weight[i];
            egr_qsize_weight[j] = add_info->dynamic_queue_size_weight[i];
            j++;
        }

        /* Figure out which of these egress ports are unicast */
        num_uc_egr_ports = num_egr_ports - num_uc_egr_disable_port;
        if (num_uc_egr_ports > 0 ) {
            uc_egr_hwmod = sal_alloc(sizeof(bcm_module_t) * num_uc_egr_ports,
                    "uc_egr_hwmod");
            if (uc_egr_hwmod == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            uc_egr_hwport = sal_alloc(sizeof(bcm_port_t) * num_uc_egr_ports,
                    "uc_egr_hwport");
            if (uc_egr_hwport == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            uc_egr_scaling_factor = sal_alloc(sizeof(int) * num_uc_egr_ports,
                    "uc_egr_scaling_factor");
            if (uc_egr_scaling_factor == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            uc_egr_load_weight = sal_alloc(sizeof(int) * num_uc_egr_ports,
                    "uc_egr_load_weight");
            if (uc_egr_load_weight == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            uc_egr_qsize_weight = sal_alloc(sizeof(int) * num_uc_egr_ports,
                    "uc_egr_qsize_weight");
            if (uc_egr_qsize_weight == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            for (i = 0, k = 0; i < j; i++) {
                if (egr_member_flags[i] &
                        BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                    continue;
                }
                uc_egr_hwmod[k] = egr_hwmod[i];
                uc_egr_hwport[k] = egr_hwport[i];
                uc_egr_scaling_factor[k] = egr_scaling_factor[i];
                uc_egr_load_weight[k] = egr_load_weight[i];
                uc_egr_qsize_weight[k] = egr_qsize_weight[i];
                k++;
            }
        }
    } else {
        
#if 0
        /* Time to use entry 0 reserved for this purpose.
         * Set mod and port to inavlid values in entry */
        if (trunk_mbr_entry_zero_rsvd == TRUE) {
            trunk_member_entry_t trunk_member_entry;
            sal_memset(&trunk_member_entry, 0, sizeof(trunk_member_entry_t));
            soc_TRUNK_MEMBERm_field32_set(unit,&trunk_member_entry,
                                          MODULE_IDf,0xff);
            soc_TRUNK_MEMBERm_field32_set(unit,&trunk_member_entry,
                                          PORT_NUMf, 0x7f);
            SOC_IF_ERROR_RETURN
                (WRITE_TRUNK_MEMBERm(unit, MEM_BLOCK_ALL,
                                     0,&trunk_member_entry));
            trunk_mbr_entry_zero_rsvd = FALSE;
        }
#endif
    }
    
#if 0
    if (t_info->in_use) {
        rv = _bcm_th3_lag_slb_free_resource(unit, tid);
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
    }
#endif

    /* Write TH3 memory for Fast LAG settings. Static load
     * balancing.  There is no dynamic load balancing in TH3. */
    rv = _bcm_th3_lag_slb_set(unit, tid, num_uc_egr_ports,
            uc_egr_hwmod, uc_egr_hwport, t_info);
    if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
    }

    /* Update TRUNK_BITMAP table */
    rv = READ_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tid, &trunk_bitmap_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
    }
    SOC_PBMP_CLEAR(old_trunk_pbmp);
    soc_mem_pbmp_field_get(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
            TRUNK_BITMAPf, &old_trunk_pbmp);
    soc_mem_pbmp_field_set(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
            TRUNK_BITMAPf, &new_trunk_pbmp);
    rv = WRITE_TRUNK_BITMAPm(unit, MEM_BLOCK_ALL, tid, &trunk_bitmap_entry);
    if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
    }

    /* Update source trunk map configuration */
    for (i = 0; i < add_info->num_ports; i++) {
        mod = hwmod[i%add_info->num_ports]; /* SHOULD ALL BE 0 */
        port = hwport[i%add_info->num_ports];

        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_INGRESS_DISABLE) {
            rv = _bcm_th3_trunk_set_port_property(unit, mod, port, -1);
        } else {
            rv = _bcm_th3_trunk_set_port_property(unit, mod, port, tid);
        }
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
    }    

    /* Update the trunk block mask */
    if (SOC_PBMP_NOT_NULL(new_trunk_pbmp) ||
            SOC_PBMP_NOT_NULL(old_trunk_pbmp)) {
        rv = _bcm_th3_trunk_block_mask(unit,
                TRUNK_BLOCK_MASK_TYPE_IPMC,
                old_trunk_pbmp,
                new_trunk_pbmp,
                ipmc_localport_array,
                ipmc_localport_count,
                ipmc_my_modid_bmap,
                ipmc_member_count,
                add_info->flags);
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
        rv = _bcm_th3_trunk_block_mask(unit,
                TRUNK_BLOCK_MASK_TYPE_L2MC,
                old_trunk_pbmp,
                new_trunk_pbmp,
                l2mc_localport_array,
                l2mc_localport_count,
                l2mc_my_modid_bmap,
                l2mc_member_count,
                add_info->flags);
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
        rv = _bcm_th3_trunk_block_mask(unit,
                TRUNK_BLOCK_MASK_TYPE_BCAST,
                old_trunk_pbmp,
                new_trunk_pbmp,
                bcast_localport_array,
                bcast_localport_count,
                bcast_my_modid_bmap,
                bcast_member_count,
                add_info->flags);
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
        rv = _bcm_th3_trunk_block_mask(unit,
                TRUNK_BLOCK_MASK_TYPE_DLF,
                old_trunk_pbmp,
                new_trunk_pbmp,
                dlf_localport_array,
                dlf_localport_count,
                dlf_my_modid_bmap,
                dlf_member_count,
                add_info->flags);
        if (BCM_FAILURE(rv)) {
            _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
        }
    }

    /* Hardware trunk failover */
    rv = _bcm_th3_trunk_failover_set(unit, tid, FALSE, add_info);
    if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
    }

    /* Software failover; called from linkscan */
    rv = _bcm_th3_trunk_swfailover_set(unit, tid, t_info->rtag,
                                num_egr_ports, egr_hwmod, egr_hwport,
                                egr_member_flags, add_info->flags);


    /* Figure out which members are leaving, staying or joining
     * the trunk group
     */
    if (t_info->in_use) {
        if (MEMBER_INFO(unit, tid).num_ports > 0) {
            leaving_members = sal_alloc(sizeof(bcm_gport_t) *
                    MEMBER_INFO(unit, tid).num_ports, "leaving members");
            if (leaving_members == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            sal_memset(leaving_members, 0,
                    sizeof(bcm_gport_t) * MEMBER_INFO(unit, tid).num_ports);

            staying_members = sal_alloc(sizeof(bcm_gport_t) *
                    MEMBER_INFO(unit, tid).num_ports, "staying members");
            if (staying_members == NULL) {
                _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
            }
            sal_memset(staying_members, 0,
                    sizeof(bcm_gport_t) * MEMBER_INFO(unit, tid).num_ports);

            for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                mod = 0; /*MEMBER_INFO(unit, tid).modport[i] >> 8; */
                port = MEMBER_INFO(unit, tid).port[i] & 0xff;
                BCM_GPORT_MODPORT_SET(gport, mod, port);

                /* Invalidate port only if it is not a part from new trunk group */
                remove_trunk_port = TRUE;
                for (j = 0; j < add_info->num_ports; j++) {
                    if (mod == add_info->tm[j] && port == add_info->tp[j]) {
                        remove_trunk_port = FALSE;
                    }
                }
                if (remove_trunk_port) {
                    /* Add port to array containing members leaving the trunk
                     * group if it is different from previous leaving members
                     */
                    match_leaving_members = FALSE;
                    for (k = 0; k < num_leaving_members; k++) {
                        if (gport == leaving_members[k]) {
                            match_leaving_members = TRUE;
                            break;
                        }
                    }
                    if (!match_leaving_members) {
                        leaving_members[num_leaving_members++] = gport;
                    }

                    rv = _bcm_th3_trunk_set_port_property(unit, mod, port, -1);
                    if (BCM_FAILURE(rv)) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                    }

                    is_local = is_local_subport = 0;
                    /* If deleted port is local, clear HW failover info */
                    rv = _bcm_esw_modid_is_local(unit, mod, &is_local);
                    if (BCM_FAILURE(rv)) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                    }

                    if (!is_local && !is_local_subport) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                        /* continue; */
                    }

                    BCM_GPORT_MODPORT_SET(gport, mod, port);
                    rv = bcm_esw_port_local_get(unit, gport, &loc_port);
                    if (BCM_FAILURE(rv)) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                    }
                    if (IS_HG_PORT(unit, loc_port)) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                        /* continue; */
                    }
                    /* ERROR on Cascaded Physical Ports. No Support */
                    if ((soc_feature(unit, soc_feature_linkphy_coe) &&
                        IS_LP_PORT(unit, loc_port)) ||
                        (soc_feature(unit, soc_feature_subtag_coe) &&
                        IS_SUBTAG_PORT(unit, loc_port)) ||
                        ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                         (soc_feature(unit, soc_feature_channelized_switching))) &&
                        IS_SUBTAG_PORT (unit, loc_port))) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                        /* continue; */
                    }
                    rv = _bcm_th3_trunk_hwfailover_set(unit, tid,
                            port, add_info->psc,
                            0, 0, NULL);
                    if (BCM_FAILURE(rv)) {
                        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
                    }
                } else {
                    /* Add port to array containing members staying in the trunk
                     * group if it is different from previous staying members
                     */
                    match_staying_members = FALSE;
                    for (k = 0; k < num_staying_members; k++) {
                        if (gport == staying_members[k]) {
                            match_staying_members = TRUE;
                            break;
                        }
                    }
                    if (!match_staying_members) {
                        staying_members[num_staying_members++] = gport;
                    }

                }
            }
        }
    }

    /* allocate space and set joining members for comparison. If a joining
     * member matches a staying member, don't need to add it again. */
    joining_members = sal_alloc(sizeof(bcm_gport_t) * add_info->num_ports,
            "joining members");
    if (joining_members == NULL) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }
    sal_memset(joining_members, 0, sizeof(bcm_gport_t) * add_info->num_ports);
    for (i = 0; i < add_info->num_ports; i++) {
        BCM_GPORT_MODPORT_SET(gport, hwmod[i], hwport[i]); 
        match_staying_members = FALSE;
        for (j = 0; j < num_staying_members; j++) {
            if (gport == staying_members[j]) {
                match_staying_members = TRUE;
                break;
            }
        }
        if (!match_staying_members) {
            match_joining_members = FALSE;
            for (j = 0; j < num_joining_members; j++) {
                if (gport == joining_members[j]) {
                    match_joining_members = TRUE;
                    break;
                }
            }
            if (!match_joining_members) {
                joining_members[num_joining_members++] = gport;
            }
        }
    }

    

    rv = _bcm_xgs3_trunk_property_migrate(unit,
                num_leaving_members, leaving_members,
                num_staying_members, staying_members,
                num_joining_members, joining_members);
    if (BCM_FAILURE(rv)) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
    }

    /* Save new trunk member info. Free up the old memory and allocate new space
     * since there may be a different number of entries. */
    if (MEMBER_INFO(unit, tid).port) {
        sal_free(MEMBER_INFO(unit, tid).port);
        MEMBER_INFO(unit, tid).port = NULL;
    }
    if (MEMBER_INFO(unit, tid).member_flags) {
        sal_free(MEMBER_INFO(unit, tid).member_flags);
        MEMBER_INFO(unit, tid).member_flags = NULL;
    }

    MEMBER_INFO(unit, tid).port = sal_alloc(
            sizeof(uint16) * add_info->num_ports, "member info port");
    if (NULL == MEMBER_INFO(unit, tid).port) {
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }

    MEMBER_INFO(unit, tid).member_flags = sal_alloc(
            sizeof(uint32) * add_info->num_ports, "member info flags");
    if (NULL == MEMBER_INFO(unit, tid).member_flags) {
        sal_free(MEMBER_INFO(unit, tid).port);
        MEMBER_INFO(unit, tid).port = NULL;
        _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(BCM_E_MEMORY);
    }

    /* Save the information to our memory */
    for (i = 0; i < add_info->num_ports; i++) {
        MEMBER_INFO(unit, tid).port[i] = ((hwmod[i] & 0xff) << 8) |
                                            (hwport[i] & 0xff);
        MEMBER_INFO(unit, tid).member_flags[i] = add_info->member_flags[i];
    }
    MEMBER_INFO(unit, tid).num_ports = add_info->num_ports;

#endif
#endif

    /* Free all dynamically allocated memory used in this function */
    _BCM_TH3_TRUNK_SET_FREE_MEM_RETURN(rv);
}


/*
 * Function:
 *      bcm_th3_trunk_modify
 * Purpose:
 *      Set trunk members.
 * Parameters:
 *      unit       - (IN) SOC unit number. 
 *      tid        - (IN) Trunk group ID.
 *      trunk_info - (IN) Pointer to user provided trunk info.
 *      member_count - (IN) Number of trunk members.
 *      member_array - (IN) Array of trunk members.
 *      t_info       - (IN) Pointer to internal trunk info.
 *      op           - (IN) Not used by this procedure.
 *      member       - (IN) Not used by this procedure.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th3_trunk_modify(int unit,
        bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info,
        int member_count,
        bcm_trunk_member_t *member_array,
        trunk_private_t *t_info,
        int op,
        bcm_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    int rtag;
#if 1
/*    bcm_trunk_chip_info_t chip_info; */
    _esw_trunk_add_info_t add_info;
    int i;
    int higig_trunk;

    if (trunk_info->psc <= 0) {
        trunk_info->psc = BCM_TRUNK_PSC_PORTFLOW;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_th3_trunk_psc_to_rtag(unit, trunk_info->psc, &rtag));
    t_info->rtag = rtag;
    t_info->flags = trunk_info->flags;

    if ((trunk_info->dlf_index != trunk_info->mc_index) ||
        (trunk_info->dlf_index != trunk_info->ipmc_index)) {
        return BCM_E_PARAM;
    }

    t_info->dlf_index_spec =
        t_info->dlf_index_used =
        t_info->mc_index_spec = 
        t_info->mc_index_used =
        t_info->ipmc_index_spec =
        t_info->ipmc_index_used = trunk_info->dlf_index;

    if (member_count < 1) {
        if (t_info->in_use) {
            /* Delete existing trunk member ports */
            BCM_IF_ERROR_RETURN
                (bcm_th3_trunk_destroy(unit, tid, t_info));
        }
    } else {
        sal_memset(&add_info, 0, sizeof(_esw_trunk_add_info_t));

        /* Fill in fields.  Don't need to fill in DLB fields because
         * TH3 does not support DLB and already initialized mem to 0 */
        add_info.flags = trunk_info->flags;
        add_info.num_ports = member_count;
        add_info.psc = trunk_info->psc;
        add_info.ipmc_psc = trunk_info->ipmc_psc;
        add_info.dlf_index = trunk_info->dlf_index;
        add_info.mc_index = trunk_info->mc_index;
        add_info.ipmc_index = trunk_info->ipmc_index;

        BCM_IF_ERROR_RETURN
            (_th3_trunk_add_info_member_alloc(&add_info, member_count));

        for (i = 0; i < member_count; i++) {
            add_info.member_flags[i] = member_array[i].flags;
            add_info.tp[i] = member_array[i].gport;
            add_info.tm[i] = -1;
        }

        /* Convert member gport to module and port */
        
        higig_trunk = 0;
        rv = _bcm_esw_trunk_gport_array_resolve(unit,
                higig_trunk,
                member_count,
                add_info.tp,
                add_info.tp,
                add_info.tm);


        if (BCM_FAILURE(rv)) {
            _th3_trunk_add_info_member_free(&add_info);
            return rv;
        }

        /* Check modids of passed in gports in member_array.  Return error
         * if nonzero
         */
        for (i = 0; i < member_count; i++) {
            if (!SOC_MODID_ADDRESSABLE(unit, add_info.tm[i])) {
                rv = BCM_E_BADID;
                _th3_trunk_add_info_member_free(&add_info);
                break;
            }
        }
        if (BCM_SUCCESS(rv)) {
            /* The new psc in trunk_info is the one to be programmed */
            t_info->psc = trunk_info->psc;
            rv = _bcm_th3_trunk_set(unit, tid, &add_info, t_info);
            _th3_trunk_add_info_member_free(&add_info);
        }
    }
#endif
    return rv;
}



/*
 * Function:
 *      bcm_th3_trunk_destroy
 * Purpose:
 *      Deletes the trunk specified
 */
int
bcm_th3_trunk_destroy(int unit,
                       bcm_trunk_t tid,
                       trunk_private_t *t_info)
{
    
    bcm_module_t                mod;
    bcm_port_t                  port, loc_port;
    bcm_gport_t                 gport;
    pbmp_t                      old_ports, new_ports;
    int                         i, rv;
    int                         num_leaving_members = 0;
    bcm_gport_t                 *leaving_members = NULL;

    SOC_PBMP_CLEAR(new_ports);

    /* front panel trunks */

    SOC_PBMP_CLEAR(old_ports);

    /* Dynamically allocate this because 64 * 4 bytes
     * is a lot to put on the stack */
    leaving_members = sal_alloc(MEMBER_INFO(unit, tid).num_ports * sizeof(int),
                                "bcm_th3_drunk_destroy leaving_members");
    if (NULL == leaving_members) {
        return BCM_E_MEMORY;
    }
    sal_memset(leaving_members, 0, MEMBER_INFO(unit, tid).num_ports * sizeof(int));

    for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
        mod = 0; /* MEMBER_INFO(tid).port[i] >> 8; */
        port = MEMBER_INFO(unit, tid).port[i] & 0xff;
        BCM_GPORT_MODPORT_SET(gport, mod, port);
        if (BCM_SUCCESS(bcm_esw_port_local_get(unit, gport, &loc_port))) { 
            SOC_PBMP_PORT_ADD(old_ports, loc_port);
        }
        leaving_members[num_leaving_members++] = gport;
        rv = _bcm_th3_trunk_set_port_property(unit, mod, port, -1);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /*
     * Remove ports from the block mask table
     */
    if (SOC_PBMP_NOT_NULL(old_ports)) {
        SOC_PBMP_CLEAR(new_ports);
        /* coverity[divide_by_zero] */
        rv = _bcm_th3_trunk_block_mask(unit, TRUNK_BLOCK_MASK_TYPE_IPMC,
                old_ports, new_ports, NULL, 0, NULL, 0, t_info->flags);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        rv = _bcm_th3_trunk_block_mask(unit, TRUNK_BLOCK_MASK_TYPE_L2MC,
                old_ports, new_ports, NULL, 0, NULL, 0, t_info->flags);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        rv = _bcm_th3_trunk_block_mask(unit, TRUNK_BLOCK_MASK_TYPE_BCAST,
                old_ports, new_ports, NULL, 0, NULL, 0, t_info->flags);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        rv = _bcm_th3_trunk_block_mask(unit, TRUNK_BLOCK_MASK_TYPE_DLF,
                old_ports, new_ports, NULL, 0, NULL, 0, t_info->flags);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
    }

    /*
     * Reset table entries to empty state
     */
    rv = soc_mem_write(unit, TRUNK_BITMAPm, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, TRUNK_BITMAPm));
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    rv = soc_mem_write(unit, FAST_TRUNK_PORTS_1m, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, FAST_TRUNK_PORTS_1m));
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    rv = soc_mem_write(unit, FAST_TRUNK_PORTS_2m, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, FAST_TRUNK_PORTS_2m));
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    rv = soc_mem_write(unit, FAST_TRUNK_SIZEm, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, FAST_TRUNK_SIZEm));
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /*
     * Remove ports from failover
     */
    rv = _bcm_th3_trunk_failover_set(unit, tid, FALSE, NULL);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv =  _bcm_th3_trunk_swfailover_set(unit,
                                        tid,
                                        0,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        0);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    
    rv = _bcm_xgs3_trunk_property_migrate(unit,
                num_leaving_members, leaving_members,
                0, NULL, 0, NULL);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    

    /* Free memory allocated for ports and member flags in the trunk_set
     * function */
    if (MEMBER_INFO(unit, tid).port) {
        sal_free(MEMBER_INFO(unit, tid).port);
        MEMBER_INFO(unit, tid).port = NULL;
    }
    if (MEMBER_INFO(unit, tid).member_flags) {
        sal_free(MEMBER_INFO(unit, tid).member_flags);
        MEMBER_INFO(unit, tid).member_flags = NULL;
    }
    MEMBER_INFO(unit, tid).num_ports = 0;

    t_info->in_use = FALSE;

cleanup:
    sal_free(leaving_members);

    return rv;
}

static soc_field_t _th3_trunk_failover_portf[8] = {
    PORT0f, PORT1f, PORT2f, PORT3f,
    PORT4f, PORT5f, PORT6f, PORT7f
};

/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_write
 * Purpose:
 *      Set front panel trunk hardware failover config
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      failport    - (IN) Fail port
 *      rtag        - (IN) Port selection criteria for failover port list
 *      hw_count    - (IN) Number of ports in failover port list
 *      ftp         - (IN) Pointer to failover port list port ID
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_trunk_hwfailover_write(int unit,
        bcm_port_t failport, int rtag,
        int hw_count, bcm_port_t *ftp)
{
    int failover_status, failover_max_portcnt, mirror_enable, i;
    port_lag_failover_set_entry_t fail_entry;


    if (SOC_PORT_USE_PORTCTRL(unit, failport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_hwfailover_status_get(unit, failport,
                                                     &failover_status));
    } else {
        /* TH3 uses portmod and portctrl */
        return BCM_E_INIT;
    }

    if (failover_status) {
        /*
         * Already in failover mode.  Do not change HW settings!
         * This should be dealt with as part of the port link processing.
         */
        if (hw_count == 0) {
            /* Notify linkscan of port removal from trunk */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_link_failover_set(unit, failport, FALSE));
            return BCM_E_NONE;
        } else {
            /* Failed port has not yet been removed from the trunk
             * -- do not change the failover configuration
             */
            return BCM_E_NONE;
        }
    }

    /* Enabling trunk failover */
    if (hw_count) {
        /* Must be in directed mirroring mode to use this feature */
        BCM_IF_ERROR_RETURN
            (bcm_esw_switch_control_get(unit, bcmSwitchDirectedMirroring,
                                        &mirror_enable));
        if (!mirror_enable) {
            return BCM_E_CONFIG;
        }

        sal_memset(&fail_entry, 0, sizeof(fail_entry));
        soc_mem_field32_set(unit, PORT_LAG_FAILOVER_SETm,
                (uint32 *)&fail_entry,
                FAILOVER_SET_SIZEf, hw_count - 1);
        soc_mem_field32_set(unit, PORT_LAG_FAILOVER_SETm,
                (uint32 *)&fail_entry, RTAGf, rtag);

        failover_max_portcnt = 1 << soc_mem_field_length(unit,
                PORT_LAG_FAILOVER_SETm, FAILOVER_SET_SIZEf);

        /* Set each port to failover to for failport */
        for(i = 0; i < failover_max_portcnt; i++) {
            soc_mem_field32_set(unit, PORT_LAG_FAILOVER_SETm,
                    (uint32 *)&fail_entry,
                    _th3_trunk_failover_portf[i],
                    ftp[i % hw_count]);
        }

        SOC_IF_ERROR_RETURN
            (WRITE_PORT_LAG_FAILOVER_SETm(unit, MEM_BLOCK_ALL, failport,
                                          &fail_entry));
    }

    if (SOC_PORT_USE_PORTCTRL(unit, failport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_hwfailover_enable_set(unit, failport,
                                                     (hw_count != 0) ? 1 : 0));
    } else {
        /* TH3 uses portmod and portctrl */
        return BCM_E_INIT;
    }

    /* Notify linkscan of failover status */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_link_failover_set(unit, failport, (hw_count != 0)));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_read
 * Purpose:
 *      Set front panel trunk hardware failover config
 * Parameters:
 *      unit        - (IN) SOC unit number.
 *      failport    - (IN) Fail port
 *      max_ports   - (IN) max ports to copy out
 *      ftp         - (OUT) Pointer to port IDs in failover port list
 *      hw_count    - (OUT) Number of failover ports copied out.
 *      rtag        - (OUT) pointer to failover ports rtag
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_bcm_th3_trunk_hwfailover_read(int unit, bcm_port_t failport, int max_ports,
                           bcm_port_t *ftp, int *hw_count, int *rtag)
{
    int enable = FALSE, mirror_enable, i;
    port_lag_failover_set_entry_t fail_entry;
    uint32 failover_set_size;

    *hw_count = 0;

    if (SOC_PORT_USE_PORTCTRL(unit, failport)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_portctrl_hwfailover_enable_get(unit, failport,
                                                     &enable));
    } else {
        /* TH3 uses portmod and portctrl */
        return BCM_E_INIT;
    }

    if (!enable) {
        return BCM_E_NONE;
    }

    /* Must be in directed mirroring mode to use this feature */
    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchDirectedMirroring,
                                    &mirror_enable));
    if (!mirror_enable) {
        return BCM_E_NONE;
    }

    sal_memset(&fail_entry, 0, sizeof(fail_entry));
    SOC_IF_ERROR_RETURN
        (READ_PORT_LAG_FAILOVER_SETm(unit, MEM_BLOCK_ALL, failport,
                                      &fail_entry));

    failover_set_size = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
            (uint32 *)&fail_entry, FAILOVER_SET_SIZEf) + 1;
    *rtag = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
            (uint32 *)&fail_entry, RTAGf);
    *hw_count = failover_set_size > max_ports? max_ports: failover_set_size;

    /* Set each port to failover to for failport */
    for(i = 0; i < *hw_count; i++) {
        ftp[i] = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
                (uint32 *)&fail_entry,
                _th3_trunk_failover_portf[i]);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_set
 * Purpose:
 *      Derive hardware failover port list and set hardware
 *      failover config.
 * Parameters:
 *      unit        - (IN) unit number
 *      tid         - (IN) Trunk Group ID
 *      port        - (IN) Port ID of the fail port
 *      psc         - (IN) Port selection criteria for the failover port list
 *      flags       - (IN) BCM_TRUNK_FLAG_FAILOVER_xxx
 *      count       - (IN) Number of ports in failover port list
 *      ftp         - (IN) Pointer to port IDs in failover port list
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_hwfailover_set(int unit, bcm_trunk_t tid,
                        bcm_port_t port, int psc, uint32 flags,
                        int count, bcm_port_t *ftp)
{
    int rtag, trunk_ports, trunk_index, failover_max_portcnt;
    int hw_count, i, rv, next;
    uint16 fail_port;
    bcm_port_t *port_list = NULL;
    _th3_hw_tinfo_t *hw_fail_trunk;

    /* Passed in port is the fail port. No HG/stacking calc */
    fail_port = port;

    BCM_IF_ERROR_RETURN(_bcm_th3_trunk_psc_to_rtag(unit, psc, &rtag));

    hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);
    trunk_ports = hw_fail_trunk->num_ports;

    for(trunk_index = 0; trunk_index < trunk_ports; trunk_index++) {
        if (fail_port == hw_fail_trunk->port[trunk_index]) {
            break;
        }
    }

    if ((0 != count) && (trunk_index == trunk_ports)) {
        /* Port not in trunk! */
        return BCM_E_NOT_FOUND;
    }

    /* TH3 allows a max of 8 failover ports */
    failover_max_portcnt = 1 << soc_mem_field_length(unit,
            PORT_LAG_FAILOVER_SETm, FAILOVER_SET_SIZEf);

    port_list = sal_alloc(sizeof(bcm_port_t) * failover_max_portcnt,
                    "failover port list");

    if (port_list == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(port_list, 0, sizeof(bcm_port_t) * failover_max_portcnt);

    if (flags) {
        switch (flags) {
            case BCM_TRUNK_FLAG_FAILOVER_NEXT_LOCAL:
                next = TRUE;
                break;
            case BCM_TRUNK_FLAG_FAILOVER_NEXT:
                next = TRUE;
                break;
            case BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL:
                next = FALSE;
                break;
            case BCM_TRUNK_FLAG_FAILOVER_ALL:
                next = FALSE;
                break;
            default:
                /* Illegal flags setting */
                sal_free(port_list);
                return BCM_E_PARAM;
        }
        
        hw_count = 0;

        for (i = ((trunk_index + 1) % trunk_ports); /* Start after fail port */
            i != trunk_index;                   /* Stop after cycling around */
            i = ((i + 1) % trunk_ports)) { /* Cycle through trunk port list */

            /* All ports are local; modid is 0, so only add port */
            port_list[hw_count] = hw_fail_trunk->port[i];
            hw_count++;
            if (hw_count == failover_max_portcnt) {
                break;
            }
            if (next) {
                break;
            }
        }
    } else {
        hw_count = 0;

        if (count) {
            for (i = 0; i < count && i < failover_max_portcnt; i++) {
                port_list[i] = BCM_GPORT_MODPORT_PORT_GET(ftp[i]);

            }
            hw_count = i;
        }
    }

    rv = _bcm_th3_trunk_hwfailover_write(unit, port, rtag,
            hw_count, port_list);

    sal_free(port_list);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Set the port selection criteria and the flags in the
     * port that hardware failover was just configured for
     */
    if (trunk_index < hw_fail_trunk->num_ports) {
        hw_fail_trunk->flags[trunk_index] = flags;
        hw_fail_trunk->psc[trunk_index] = psc;
    }

    return rv;
}
/*
 * Function:
 *      _bcm_th3_trunk_hwfailover_get
 * Purpose:
 *      Derive hardware failover port list and set hardware
 *      failover config.
 * Parameters:
 *      unit        - (IN) unit number
 *      tid         - (IN) Trunk Group ID
 *      port        - (IN) Port ID of the fail port
 *      array_size  - (IN) numbers of max failovers ports to copy out
 *      psc         - (OUT) Port selection criteria for the failover port list
 *      flags       - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx
 *      array_count - (OUT) Number of failover ports copied out
 *      ftp         - (OUT) Pointer to port IDs in failover port list
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_th3_trunk_hwfailover_get(int unit, bcm_trunk_t tid,
                         bcm_port_t port, int *psc, uint32 *flags,
                         int array_size, bcm_port_t *ftp, int *array_count)
{
    int trunk_ports, trunk_index;
    int rv = BCM_E_NONE;
    uint16 fail_port;
    _th3_hw_tinfo_t *hw_fail_trunk;
    int rtag;

    /* Passed in port is the fail port. No HG/stacking calc */
    fail_port = port;

    if (array_size != 0 && ftp == NULL) {
        return BCM_E_PARAM;
    }
    hw_fail_trunk = &(_th3_trunk_hwfail[unit]->hw_tinfo[tid]);
    trunk_ports = hw_fail_trunk->num_ports;

    for(trunk_index = 0; trunk_index < trunk_ports; trunk_index++) {
        if (fail_port == hw_fail_trunk->port[trunk_index]) {
            break;
        }
    }

    if (trunk_index == trunk_ports) {
        /* Port not in trunk! */
        return BCM_E_NOT_FOUND;
    }
    *psc = hw_fail_trunk->psc[trunk_index];
    *flags = hw_fail_trunk->flags[trunk_index];

    if (array_size > 0) {
        sal_memset(ftp, 0, sizeof(bcm_port_t) * array_size);
        rv = _bcm_th3_trunk_hwfailover_read(unit, port, array_size, ftp, array_count, &rtag);
    }

    return rv;
}

#endif
#endif /* BCM_TOMAHAWK3_SUPPORT */
