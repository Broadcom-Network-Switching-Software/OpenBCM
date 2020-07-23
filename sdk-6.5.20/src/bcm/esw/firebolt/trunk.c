/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS3 Trunking (Ether and Fabric)
 */

#include <soc/mem.h>
#include <soc/firebolt.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/l2.h>     /* MAC learn limit interaction */
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/link.h>   /* LAG failover support */
#include <bcm_int/esw/stack.h>

#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/vxlan.h>
#endif /* BCM_GREYHOUND2_SUPPORT */

#define _BCM_FP_TRUNK_NUM_MAX       128
#define _BCM_FABRIC_TRUNK_NUM_MAX   16
#define _BCM_FABRIC_TRUNK_SWF_TID_OFFSET         200
#define _BCM_SCORPION_FABRIC_TRUNK_FAILOVER_PORT_NUM  16
#define _BCM_TRUNK_FAILOVER_PORT_NUM 8
#define _BCM_HURRICANE2_TRUNK_FAILOVER_PORT_NUM  4

/*
 * Trunk Membership Info
 */

typedef struct {
    uint8  num_ports;
    uint16 modport[BCM_XGS3_TRUNK_MAX_PORTCNT];
    uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT];
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8  recovered_from_scache;
#endif /* BCM_WARM_BOOT_SUPPORT */
} _xgs3_member_info_t;

typedef struct {
    _xgs3_member_info_t member_info[1];
} _xgs3_trunk_member_info_t;

STATIC _xgs3_trunk_member_info_t *_xgs3_trunk_member_info[BCM_MAX_NUM_UNITS];

#define MEMBER_INFO(_unit_, _tid_) (_xgs3_trunk_member_info[_unit_]->member_info[_tid_])

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
/*
 * Hardware Trunk Failover:
 *
 * For the devices which support hardware trunk failover, the failover
 * configuration must be retained if it is one of the standard models 
 * for the retrieval function to use the same encoding as the set function.
 *
 * Note: The trunk members stored in _xgs3_tinfo_t's modport array do not 
 *       include those members with the BCM_TRUNK_MEMBER_EGRESS_DISABLE set.
 */

typedef struct {
    uint8         num_ports;
    uint8         psc[BCM_XGS3_TRUNK_MAX_PORTCNT];
    uint16        modport[BCM_XGS3_TRUNK_MAX_PORTCNT];
    uint32        flags[BCM_XGS3_TRUNK_MAX_PORTCNT];
} _xgs3_hw_tinfo_t;

typedef struct {
    _xgs3_hw_tinfo_t hw_tinfo[_BCM_FP_TRUNK_NUM_MAX + _BCM_FABRIC_TRUNK_NUM_MAX];
} _xgs3_trunk_hwfail_t;

STATIC _xgs3_trunk_hwfail_t *_xgs3_trunk_hwfail[BCM_MAX_NUM_UNITS];
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

/*
 * Software Trunk Failover:
 *
 * Even without hardware failover support, given the high speed link
 * change notification possible with XGS3 devices, a port that has been
 * indicated as link down can cause the trunk_group table to be immediately
 * updated with the link down port removed.  This works well for two cases:
 * (1) this is a higig trunk, or (2) the local device is the only device
 * in the system.
 * 
 * Note: The trunk members stored in _xgs3_tinfo_t's modport array do not 
 *       include those members with the BCM_TRUNK_MEMBER_EGRESS_DISABLE set.
 *       If a trunk member port is down, it'll be removed from _xg3_tinfo_t's
 *       modport array by _bcm_xgs3_trunk_swfailover_trigger.
 */
typedef struct {
    uint8         rtag;
    uint8         num_ports;
    uint16        modport[BCM_XGS3_TRUNK_MAX_PORTCNT];
    uint32        member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT];
} _xgs3_tinfo_t;

typedef struct {
    uint8         mymodid;
    bcm_trunk_t   trunk[SOC_MAX_NUM_PORTS];       /* 0: unused;
                                                   1-129: ether;
                                                   200-207: higig */
    _xgs3_tinfo_t tinfo[_BCM_FP_TRUNK_NUM_MAX + _BCM_FABRIC_TRUNK_NUM_MAX];
} _xgs3_trunk_swfail_t;


STATIC _xgs3_trunk_swfail_t *_xgs3_trunk_swfail[BCM_MAX_NUM_UNITS];

STATIC int _bcm_xgs3_trunk_swfailover_set(int unit,
                                          bcm_trunk_t tid,
                                          int rtag,
                                          int nports,
                                          int mods[BCM_XGS3_TRUNK_MAX_PORTCNT],
                                          int ports[BCM_XGS3_TRUNK_MAX_PORTCNT],
                                          uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT]);

STATIC int _bcm_xgs3_trunk_swfailover_fabric_set(int unit,
                                                 bcm_trunk_t hgtid,
                                                 int rtag,
                                                 bcm_pbmp_t ports);

STATIC uint8 *_xgs3_mod_port_to_tid_map[BCM_MAX_NUM_UNITS];

/* fields from TRUNK_GROUPm */
static soc_field_t      _xgs_portf[8] = {
    PORT0f, PORT1f, PORT2f, PORT3f,
    PORT4f, PORT5f, PORT6f, PORT7f
};

static soc_field_t      _xgs_modulef[8] = {
    MODULE0f, MODULE1f, MODULE2f, MODULE3f,
    MODULE4f, MODULE5f, MODULE6f, MODULE7f
};

/* fields from FAST_TRUNK_GROUPm */
static soc_field_t      _xgs_fast_portf[8] = {
    PORT_NUM_0f, PORT_NUM_1f, PORT_NUM_2f, PORT_NUM_3f,
    PORT_NUM_4f, PORT_NUM_5f, PORT_NUM_6f, PORT_NUM_7f
};

static soc_field_t      _xgs_fast_modulef[8] = {
    MODULE_ID_0f, MODULE_ID_1f, MODULE_ID_2f, MODULE_ID_3f,
    MODULE_ID_4f, MODULE_ID_5f, MODULE_ID_6f, MODULE_ID_7f
};

/* fields from HIGIG_TRUNK_GROUPr */
static soc_field_t      _hg_trunk0f[4] = {
    HIGIG_TRUNK_ID0_PORT0f, HIGIG_TRUNK_ID0_PORT1f,
    HIGIG_TRUNK_ID0_PORT2f, HIGIG_TRUNK_ID0_PORT3f
};

static soc_field_t      _hg_trunk1f[4] = {
    HIGIG_TRUNK_ID1_PORT0f, HIGIG_TRUNK_ID1_PORT1f,
    HIGIG_TRUNK_ID1_PORT2f, HIGIG_TRUNK_ID1_PORT3f
};

/* fields from HIGIG_TRUNK_CONTROLr */
static soc_field_t _hg_cntl0f[4] = {
    HIGIG_TRUNK_ID0f, HIGIG_TRUNK_ID1f,
    HIGIG_TRUNK_ID2f, HIGIG_TRUNK_ID3f
};

static soc_field_t _hg_cntl1f[4] = {
    HIGIG_TRUNK0f, HIGIG_TRUNK1f,
    HIGIG_TRUNK2f, HIGIG_TRUNK3f
};

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
/* fields from HG_TRUNK_GROUPr */
/* Enduro has only 4 fields. Not 8. They will be checked in the executable code */
static soc_field_t _hg_trunk[8] = {
    HIGIG_TRUNK_PORT0f, HIGIG_TRUNK_PORT1f,
    HIGIG_TRUNK_PORT2f, HIGIG_TRUNK_PORT3f,
    HIGIG_TRUNK_PORT4f, HIGIG_TRUNK_PORT5f,
    HIGIG_TRUNK_PORT6f, HIGIG_TRUNK_PORT7f
};
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
/* fields from HG_TRUNK_GROUP_HIr */
static soc_field_t _hg_trunk_hi[8] = {
    HIGIG_TRUNK_PORT8f, HIGIG_TRUNK_PORT9f,
    HIGIG_TRUNK_PORT10f, HIGIG_TRUNK_PORT11f,
    HIGIG_TRUNK_PORT12f, HIGIG_TRUNK_PORT13f,
    HIGIG_TRUNK_PORT14f, HIGIG_TRUNK_PORT15f
};
#endif /* BCM_SCORPION_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_trunk_member_init
 * Purpose:
 *      Initialize trunk member info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_xgs3_trunk_member_init(int unit)
{
    int trunk_num_groups = _BCM_FP_TRUNK_NUM_MAX + _BCM_FABRIC_TRUNK_NUM_MAX;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_num_groups = soc_mem_index_count(unit, FAST_TRUNK_GROUPm) +
                           soc_mem_index_count(unit, HG_TRUNK_GROUPm);
    }
#endif

    if (_xgs3_trunk_member_info[unit] == NULL) {
        _xgs3_trunk_member_info[unit] =
            sal_alloc((sizeof(_xgs3_trunk_member_info_t) * trunk_num_groups),
                    "_xgs3_trunk_member_info");

        if (_xgs3_trunk_member_info[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(_xgs3_trunk_member_info[unit], 0,
                (sizeof(_xgs3_trunk_member_info_t) * trunk_num_groups));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_trunk_member_detach
 * Purpose:
 *      De-allocate trunk member info.
 * Parameters:
 *      unit - (IN)SOC unit number. 
 * Returns:
 *      None
 */
void
_bcm_xgs3_trunk_member_detach(int unit)
{
    if (_xgs3_trunk_member_info[unit] != NULL) {
        sal_free(_xgs3_trunk_member_info[unit]);
        _xgs3_trunk_member_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_xgs3_trunk_mod_port_map_set
 * Purpose:
 *      Set  (mod, port) to  trunk id map.
 *      Map implements a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN) BCM device number. 
 *      modid    -  (IN) Member port module id.  
 *      port     -  (IN) Member port number.  
 *      trunk_id -  (IN) Trunk number or negative value for reset.  
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_xgs3_trunk_mod_port_map_set(int unit, bcm_module_t modid, 
                                 bcm_port_t port, bcm_trunk_t trunk_id)  
{

    int   idx;               /* Map array index. */

    if (NULL == _xgs3_mod_port_to_tid_map[unit]) {
        return (BCM_E_INIT);
    }

    /* Trunk id range check. */
    if (trunk_id >= 0xff) {
        return (BCM_E_INTERNAL);
    }

    idx = modid * SOC_MAX_NUM_PORTS + port;
    if (idx >= _BCM_XGS3_TRUNK_MOD_PORT_MAP_IDX_COUNT(unit)) { 
        return (BCM_E_PARAM);
    }

    _xgs3_mod_port_to_tid_map[unit][idx] = (trunk_id < 0) ? 0 : (++trunk_id);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_xgs3_trunk_mod_port_map_get
 * Purpose:
 *      Read mapping (mod, port) to trunk id from software cache 
 *      for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN) BCM device number. 
 *      modid    -  (IN) Member port module id.  
 *      port     -  (IN) Member port number.  
 *      trunk_id -  (OUT) Trunk number or negative value for reset.  
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_xgs3_trunk_mod_port_map_get(int unit, bcm_module_t modid, 
                                 bcm_port_t port, bcm_trunk_t *trunk_id)  
{

    int              idx;      /* Map array index.         */
    bcm_module_t     mymodid;  /* Local module id.         */
    int              isLocal;  /* Local module id flag.    */
    int              rv;       /* Operation return status. */

    if (NULL == _xgs3_mod_port_to_tid_map[unit]) {
        return (BCM_E_INIT);
    }

    rv = _bcm_esw_modid_is_local(unit, modid, &isLocal);
    BCM_IF_ERROR_RETURN(rv);

    if (isLocal) {
        rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
        BCM_IF_ERROR_RETURN(rv);
        if (mymodid != modid) {
            modid = mymodid;
            port += 32;
        }
    }

    idx = modid * SOC_MAX_NUM_PORTS + port;
    if (idx >= _BCM_XGS3_TRUNK_MOD_PORT_MAP_IDX_COUNT(unit)) { 
        return (BCM_E_PARAM);
    }

    *trunk_id = _xgs3_mod_port_to_tid_map[unit][idx] - 1;

    return (_xgs3_mod_port_to_tid_map[unit][idx]) ? BCM_E_NONE : BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      _bcm_xgs3_trunk_mod_port_map_init
 * Purpose:
 *      Initialize a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_xgs3_trunk_mod_port_map_init(int unit) 
{

    int   mem_size;  /* Allocated memory size. */
    uint8 *ptr;      /* Allocation pointer.    */

    if (NULL != _xgs3_mod_port_to_tid_map[unit]) {
        _bcm_xgs3_trunk_mod_port_map_deinit(unit);
    }

    /* Allocated mapping array. */
    mem_size =  _BCM_XGS3_TRUNK_MOD_PORT_MAP_IDX_COUNT(unit) * sizeof(uint8);
    ptr = sal_alloc(mem_size, "Mod_Port_Trunk");
    if (NULL == ptr) {
        return (BCM_E_MEMORY);
    }
    /* Reset allocated buffer. */
    sal_memset(ptr, 0, mem_size);

    _xgs3_mod_port_to_tid_map[unit] = ptr;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_xgs3_trunk_mod_port_map_deinit
 * Purpose:
 *      Deinitialize a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_xgs3_trunk_mod_port_map_deinit(int unit) 
{
    void *ptr;

    ptr = (void *)_xgs3_mod_port_to_tid_map[unit];
	
    _xgs3_mod_port_to_tid_map[unit] = NULL;

    if (NULL != ptr) {
       	sal_free(ptr);
    }
    return (BCM_E_NONE);
}


#if defined (BCM_WARM_BOOT_SUPPORT)
#ifdef BCM_HURRICANE3_SUPPORT
/*
 * Function:
 *      _bcm_xgs3_trunk_mod_port_map_reinit
 * Purpose:
 *      Reinitialize the software cache for the SOURCE_TRUNK_MAP_TABLE.
 *      Recover trunk members with egress disable flag set.
 * Parameters:
 *      unit - (IN) SOC unit number. 
 *      mod - (IN) Module ID.
 *      base_index - (IN) Module ID's base index.
 *      num_ports - (IN) Number of ports in the given module.
 * Returns:
 *      BCM_X_XXX
 * Notes:
 *      This function is called during stacking module reinit, since
 *      the number of ports in a given module is not known until then.
 */
int 
_bcm_xgs3_trunk_mod_port_map_reinit(int unit, bcm_module_t mod,
        int base_index, int num_ports) 
{
    bcm_port_t port;
    int	rv = BCM_E_NONE;
    source_trunk_map_table_entry_t stm; 
    int port_type;
    bcm_trunk_t tid;
    int array_size;
    bcm_module_t *mod_array = NULL;
    bcm_port_t *port_array = NULL;
    uint32 *flags_array = NULL;
    int i, found;

    if (!soc_feature(unit, soc_feature_src_modid_base_index)) {
        return BCM_E_UNAVAIL;
    }

    /*
     * Recover from SOURCE_TRUNK_MAP tables.
     */
    for (port = 0; port < num_ports; port++) {

        rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                base_index + port, &stm);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        port_type = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm,
                PORT_TYPEf);
        if (port_type != 1) {
            continue;
        }

        tid = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm, TGIDf);

        /* Recover source trunk map cache */
        rv = _bcm_xgs3_trunk_mod_port_map_set(unit, mod, port, tid);  
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Allocate arrays that can accommodate an additional member,
         * in case a new member needs to be added to the trunk group.
         */
        array_size = MEMBER_INFO(unit, tid).num_ports + 1;
        mod_array = sal_alloc(sizeof(bcm_module_t) * array_size, "mod_array");
        if (mod_array == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(mod_array, 0, sizeof(bcm_module_t) * array_size); 

        port_array = sal_alloc(sizeof(bcm_port_t) * array_size, "port_array");
        if (port_array == NULL) {
            sal_free(mod_array);
            return BCM_E_MEMORY;
        }
        sal_memset(port_array, 0, sizeof(bcm_port_t) * array_size); 

        flags_array = sal_alloc(sizeof(uint32) * array_size, "flags_array");
        if (flags_array == NULL) {
            sal_free(mod_array);
            sal_free(port_array);
            return BCM_E_MEMORY;
        }
        sal_memset(flags_array, 0, sizeof(uint32) * array_size); 

        /* Try to find mod-port in the trunk group */
        found = 0; 
        for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
            mod_array[i] = MEMBER_INFO(unit, tid).modport[i] >> 8;
            port_array[i] = MEMBER_INFO(unit, tid).modport[i] & 0xff;
            flags_array[i] = MEMBER_INFO(unit, tid).member_flags[i];
            if (port_array[i] == port && mod_array[i] == mod) {
                found = 1;
                break;
            }
        }

        /* If the mod-port was not found, then add it with
         * EGRESS_DISABLE flag set.
         */  
        if (!found && !MEMBER_INFO(unit, tid).recovered_from_scache) {
            /* Find where to insert */
            for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                if (port > port_array[i] || mod != mod_array[i]) {
                    continue;
                } 
                break;
            }

            /* Insert at the required location in sw state */
            mod_array[i] = mod;
            port_array[i] = port;
            flags_array[i] |= BCM_TRUNK_MEMBER_EGRESS_DISABLE;

            /* Copy remaining members as is */
            for (; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                mod_array[i+1] = MEMBER_INFO(unit, tid).modport[i] >> 8;
                port_array[i+1] = MEMBER_INFO(unit, tid).modport[i] & 0xff;
                flags_array[i+1] = MEMBER_INFO(unit, tid).member_flags[i];
            }

            MEMBER_INFO(unit, tid).num_ports++;

            for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                MEMBER_INFO(unit, tid).modport[i] = (mod_array[i] << 8) |
                    port_array[i];
                MEMBER_INFO(unit, tid).member_flags[i] = flags_array[i];
            }
        } 

        sal_free(mod_array);
        mod_array = NULL;
        sal_free(port_array);
        port_array = NULL;
        sal_free(flags_array);
        flags_array = NULL;
    }

    return rv;
}
#endif /* BCM_HURRICANE3_SUPPORT */

/*
 * Function:
 *      _xgs3_trunk_mod_port_map_reinit
 * Purpose:
 *      Reinitialize a software cache for the SOURCE_TRUNK_MAP_TABLE.
 * Parameters:
 *      unit     -  (IN)SOC unit number. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_xgs3_trunk_mod_port_map_reinit(int unit) 
{
    source_trunk_map_table_entry_t  stm;  /* Hw entry buffer.        */
    soc_mem_t        mem;                 /* Hw table memory.        */
    int              idx;                 /* Hw table iterator index.*/
    int              idx_max;             /* Hw table index max.     */
    uint32           valid;               /* Trunk valid indicator.  */
    bcm_trunk_t	     tid;                 /* Trunk id.               */
    bcm_module_t     modid;               /* Member module id.       */
    bcm_port_t       port;                /* Member port id.         */
    int	             rv = BCM_E_NONE;     /* Operation return status.*/
    bcm_module_t     om[BCM_XGS3_TRUNK_MAX_PORTCNT]; /* new mod list */
    bcm_port_t       op[BCM_XGS3_TRUNK_MAX_PORTCNT]; /* new port list */
    uint32           of[BCM_XGS3_TRUNK_MAX_PORTCNT]; /* new flag list */
    int              i, found;
    soc_field_t      tid_fld = TGIDf;

    if (soc_feature(unit, soc_feature_src_tgid_valid)) {
        tid_fld = SRC_TGIDf;
    }
    /* Initialize mod, port to trunk id array. */
    BCM_IF_ERROR_RETURN(_bcm_xgs3_trunk_mod_port_map_init(unit)); 

    /*
     * Iterate over all trunk map entries & populate valid 
     * entries into sw mod, port to trunk id map array.
     */
    mem = SOURCE_TRUNK_MAP_TABLEm;
    idx_max = soc_mem_index_max(unit,mem);

    soc_mem_lock(unit, mem);
    for (idx = soc_mem_index_min(unit, mem); idx <= idx_max; idx++) {
        /* Read  hw entry. */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &stm);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Check if trunk id is valid. */
        valid = soc_mem_field32_get(unit, mem, (uint32 *)&stm, PORT_TYPEf);
        if (0 == valid) {
            continue;
        }

        tid = soc_mem_field32_get(unit, mem, (uint32 *)&stm, tid_fld);

        /* Translate index into modid port.  */
#ifdef BCM_TRX_SUPPORT
        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            modid = idx / (SOC_PORT_ADDR_MAX(unit) + 1);
            port = idx - modid * (SOC_PORT_ADDR_MAX(unit) + 1);
        } else
#endif
            {
                modid = idx >> SOC_TRUNK_BIT_POS(unit);
                port = idx - (modid << SOC_TRUNK_BIT_POS(unit));
            }
        rv =  _bcm_xgs3_trunk_mod_port_map_set(unit, modid, port, tid);  
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Compare values with trunk groups to recreate egress disable flag */        
        found = 0; 
        for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
            om[i] = MEMBER_INFO(unit, tid).modport[i] >> 8;
            op[i] = MEMBER_INFO(unit, tid).modport[i] & 0xff;
            of[i] = MEMBER_INFO(unit, tid).member_flags[i];
            if (op[i] == port && om[i] == modid) {
                found = 1;
                break;
            }
        }

        /* If the mod-port was not found, and if member info was not recovered
         * from scache, then add the mod-port to member info
         */  
        if (!found && !MEMBER_INFO(unit, tid).recovered_from_scache) {
            /* Find where to insert */
            for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                if (port > op[i] || modid != om[i]) {
                    continue;
                } 
                break;
            }

            /* Insert at the required location in sw state */
            om[i] = modid;
            op[i] = port;
            of[i] = BCM_TRUNK_MEMBER_EGRESS_DISABLE;

            /* Copy remaining values as is */
            for (; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                /* sw state */ 
                op[i+1] = MEMBER_INFO(unit, tid).modport[i] & 0xff;
                om[i+1] = MEMBER_INFO(unit, tid).modport[i] >> 8;
                of[i+1] = MEMBER_INFO(unit, tid).member_flags[i];
            }

            MEMBER_INFO(unit, tid).num_ports++;
            for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                 MEMBER_INFO(unit, tid).modport[i] = (om[i] << 8) | op[i];
                 MEMBER_INFO(unit, tid).member_flags[i] = of[i];
            }
        } 
    }

    soc_mem_unlock(unit, mem);

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Mark a mod/port pair as being in a particular trunk (or not)
 * A negative tid sets the mod/port back to non-trunk status.
 */
STATIC int
_bcm_xgs3_trunk_set_port_property(int unit,
                                  bcm_module_t mod,
                                  bcm_port_t port,
                                  int tid)
{
    source_trunk_map_table_entry_t  stm;
    soc_mem_t        mem;
    int              idx, rv, ptype_get, ptype_set, tid_get, tid_set;
    soc_field_t      tid_fld = TGIDf;

    if (soc_feature(unit, soc_feature_src_tgid_valid)) {
        tid_fld = SRC_TGIDf;
    }

    if (tid < 0) {
        ptype_set = 0;                /* normal port */
        tid_set = 0;
    } else {
        ptype_set = 1;                /* trunk port */
        tid_set = tid;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_src_mod_port_table_index_get(unit, mod, port, &idx));

    mem = SOURCE_TRUNK_MAP_TABLEm;
    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, idx, &stm);
    if (BCM_SUCCESS(rv)) {
        ptype_get = soc_mem_field32_get(unit, mem, (uint32 *)&stm, PORT_TYPEf);
        tid_get = soc_mem_field32_get(unit, mem, (uint32 *)&stm, tid_fld);
        if ((ptype_get != ptype_set) || (tid_get != tid_set)) {
            soc_mem_field32_set(unit, mem, (uint32 *)&stm,
                                PORT_TYPEf, ptype_set);
            soc_mem_field32_set(unit, mem, (uint32 *)&stm,
                                tid_fld, tid_set);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, &stm);
        }
    }
    soc_mem_unlock(unit, mem);

    /* Update SW module, port to tid mapping. */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_xgs3_trunk_mod_port_map_set(unit, mod, port, tid);  
    }

    return rv;
}

int
_bcm_xgs3_trunk_get_port_property(int unit,
                                  bcm_module_t mod,
                                  bcm_port_t port,
                                  int *tid)
{
    return  _bcm_xgs3_trunk_mod_port_map_get(unit, mod, port, tid);  
}

/*
 * Function:
 *      _bcm_xgs3_trunk_fabric_find
 * Description:
 *      Get fabric trunk id that contains the given Higig port
 * Parameters:
 *      unit    - Unit number
 *      port    - Port number
 *      tid     - (OUT) Fabric trunk id
 * Returns:
 *      BCM_E_xxxx
 */
int
_bcm_xgs3_trunk_fabric_find(int unit,
                            bcm_port_t port,
                            int *tid)
{
    bcm_trunk_chip_info_t chip_info;
    int                   num_hg_trunk_groups;
    int                   hgtid;
    pbmp_t                hg_pbmp, pbmp;

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if (chip_info.trunk_fabric_id_min == -1) {
        return BCM_E_NOT_FOUND;
    } else {
        num_hg_trunk_groups = chip_info.trunk_fabric_id_max - 
                              chip_info.trunk_fabric_id_min + 1;
    }

    if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) &&
        soc_feature(unit, soc_feature_hg_trunking)) {
        uint32      val;
        soc_field_t bmapf;

        SOC_IF_ERROR_RETURN(READ_HIGIG_TRUNK_CONTROLr(unit, &val));
        for (hgtid = 0; hgtid < num_hg_trunk_groups; hgtid++) {
            switch (hgtid) {
                case 0:
                    bmapf = HIGIG_TRUNK_BITMAP0f;
                    break;
                case 1:
                    bmapf = HIGIG_TRUNK_BITMAP1f;
                    break;
                default:
                    return BCM_E_INTERNAL;
            }

            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0,
                              soc_reg_field_get(unit, HIGIG_TRUNK_CONTROLr,
                                                val, bmapf));
            SOC_PBMP_CLEAR(pbmp);
            if (SOC_IS_RAVEN(unit)) {
                /* For Raven, use the bitmap directly */
                SOC_PBMP_ASSIGN(pbmp, hg_pbmp);
            } else {
                /* For other FBX, convert to the physical bitmap */
                SOC_IF_ERROR_RETURN
                    (soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbmp, &pbmp));
            } 
            if (SOC_PBMP_MEMBER(pbmp, port)) {
                *tid = hgtid + chip_info.trunk_fabric_id_min;
                return BCM_E_NONE;
            }
        }
        return BCM_E_NOT_FOUND;
    }
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        uint64        val64;

        for (hgtid = 0; hgtid < num_hg_trunk_groups; hgtid++) {
            SOC_IF_ERROR_RETURN(READ_HG_TRUNK_BITMAP_64r(unit, hgtid, &val64));
            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0,
                              soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                    val64, HIGIG_TRUNK_BITMAP_LOf));
            SOC_PBMP_WORD_SET(hg_pbmp, 1,
                              soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                    val64, HIGIG_TRUNK_BITMAP_HIf));
            if (SOC_PBMP_MEMBER(hg_pbmp, port)) {
                *tid = hgtid + chip_info.trunk_fabric_id_min;
                return BCM_E_NONE;
            }
        }
        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_high_portcount_register)) {
        uint64        val64, hg_trunk_bmap_f;

        for (hgtid = 0; hgtid < num_hg_trunk_groups; hgtid++) {
            SOC_IF_ERROR_RETURN(READ_HG_TRUNK_BITMAP_LOr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_LOr,
                                    val64, HIGIG_TRUNK_BITMAPf);

            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0, COMPILER_64_LO(hg_trunk_bmap_f));
            SOC_PBMP_WORD_SET(hg_pbmp, 1, COMPILER_64_HI(hg_trunk_bmap_f));

            SOC_IF_ERROR_RETURN(READ_HG_TRUNK_BITMAP_HIr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_HIr,
                                    val64, HIGIG_TRUNK_BITMAPf);

            SOC_PBMP_WORD_SET(hg_pbmp, 2, COMPILER_64_LO(hg_trunk_bmap_f));

            if (SOC_PBMP_MEMBER(hg_pbmp, port)) {
                *tid = hgtid + chip_info.trunk_fabric_id_min;
                return BCM_E_NONE;
            }
        }
        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
        uint32       val;

        for (hgtid = 0; hgtid < num_hg_trunk_groups; hgtid++) {
            SOC_IF_ERROR_RETURN(READ_HG_TRUNK_BITMAPr(unit, hgtid, &val));
            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0,
                              soc_reg_field_get(unit, HG_TRUNK_BITMAPr,
                                                val, HIGIG_TRUNK_BITMAPf));
            SOC_PBMP_CLEAR(pbmp);
            soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbmp, &pbmp);
            if (SOC_PBMP_MEMBER(pbmp, port)) {
                *tid = hgtid + chip_info.trunk_fabric_id_min;
                return BCM_E_NONE;
            }
        }
        return BCM_E_NOT_FOUND;
    }
#endif /* BCM_BRADLEY_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Update the trunk block mask table.
 * Old ports are cleared and new ports are added to each entry.
 * Each hash index location entry is owned by a specific unit/modid
 * across all units in a stack. Hash index location entry owned by
 * local unit/modid is provided by my_modid_bmap. member_count is
 * the number of NUC forwarding eligible ports in the trunk group.
 * Each eligible entry also has a rolling port that is not masked
 * from among the new NUC forwarding eligible ports (nuc_ports).
 * This is what spreads non-unicast traffic across multiple ports.
 */
STATIC int
_bcm_xgs3_trunk_block_mask(int unit,
                           pbmp_t old_ports,
                           pbmp_t new_ports,
                           pbmp_t nuc_ports,
                           uint32 my_modid_bmap,
                           uint32 member_count)
{
    bcm_port_t        port;
    int                localcount, localports[BCM_XGS3_TRUNK_MAX_PORTCNT];
    int                index_count, max_ports = BCM_SWITCH_TRUNK_MAX_PORTCNT;

    localcount = 0;
#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit)) {
        max_ports = BCM_XGS3_TRUNK_MAX_PORTCNT;
    }
#endif
    if (SOC_PBMP_NOT_NULL(nuc_ports)) {
        PBMP_ITER(nuc_ports, port) {
            localports[localcount++] = port;
            if (localcount >= max_ports) {
                break;
            }
        }
    }

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        int                   i, imax, rv;
        soc_mem_t             mem;
        nonucast_trunk_block_mask_entry_t mask_entry;
        bcm_pbmp_t            pbmp, old_pbmp;

        rv = BCM_E_NONE;
        mem = NONUCAST_TRUNK_BLOCK_MASKm;
        i = soc_mem_index_min(unit, mem);
        imax = soc_mem_index_max(unit, mem);
        soc_mem_lock(unit, mem);
        index_count = 0;
        for (; i <= imax; i++) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, i, &mask_entry);
            if (rv < 0) {
                break;
            }
            soc_mem_pbmp_field_get(unit, mem, &mask_entry, BLOCK_MASKf, &pbmp);
            old_pbmp = pbmp;
            SOC_PBMP_REMOVE(pbmp, old_ports);
            SOC_PBMP_OR(pbmp, new_ports);
            if (localcount > 0) {
                if ((i % 16) == 0) {
                    /* On Firebolt, NONUCAST_TRUNK_BLOCK_MASK contains 16
                     * entries. On Triumph, there are 64 entries, separated
                     * into 4 regions: IPMC, L2MC, broadcast, and unknown
                     * packets. To make Firebolt interoperate with Triumph,
                     * the index_count needs to be reset every 16 entries.
                     */
                    index_count = 0;
                }
                if (my_modid_bmap & (1 << ((i & 0xf) % member_count)))  {
                    SOC_PBMP_PORT_REMOVE(pbmp, localports[index_count % localcount]);
                    index_count++;
                }
            }
            if (SOC_PBMP_EQ(pbmp, old_pbmp)) {
                continue;
            }
            soc_mem_pbmp_field_set(unit, mem, &mask_entry, BLOCK_MASKf, &pbmp);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &mask_entry);
            if (rv < 0) {
                break;
            }
        }
        soc_mem_unlock(unit, mem);
        return rv;
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_psc_to_rtag(int unit, int psc, int *rtag)
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

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
int
_bcm_xgs3_trunk_hwfailover_init(int unit)
{
    if (_xgs3_trunk_hwfail[unit] == NULL) {
        _xgs3_trunk_hwfail[unit] = sal_alloc(sizeof(_xgs3_trunk_hwfail_t), 
                                             "_xgs3_trunk_hwfail");
        if (_xgs3_trunk_hwfail[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_xgs3_trunk_hwfail[unit], 0, sizeof(_xgs3_trunk_hwfail_t));
    return BCM_E_NONE;
}

void
_bcm_xgs3_trunk_hwfailover_detach(int unit)
{
    if (_xgs3_trunk_hwfail[unit] != NULL) {
        sal_free(_xgs3_trunk_hwfail[unit]);
        _xgs3_trunk_hwfail[unit] = NULL;
    }
}

static soc_field_t _hg_trunk_failover_portf[_BCM_SCORPION_FABRIC_TRUNK_FAILOVER_PORT_NUM] = {
    PORT0f, PORT1f, PORT2f, PORT3f,
    PORT4f, PORT5f, PORT6f, PORT7f,
    PORT8f, PORT9f, PORT10f, PORT11f,
    PORT12f, PORT13f, PORT14f, PORT15f
};

static soc_field_t _trunk_failover_portf[_BCM_TRUNK_FAILOVER_PORT_NUM] = {
    PORT0f, PORT1f, PORT2f, PORT3f,
    PORT4f, PORT5f, PORT6f, PORT7f
};

static soc_field_t _trunk_failover_modulef[_BCM_TRUNK_FAILOVER_PORT_NUM] = {
    MODULE0f, MODULE1f, MODULE2f, MODULE3f,
    MODULE4f, MODULE5f, MODULE6f, MODULE7f
};

STATIC int
_bcm_xgs3_trunk_hwfailover_hg_write(int unit, bcm_port_t failport, int rtag,
                                    int hw_count, bcm_port_t *ftp)
{
    hg_trunk_failover_set_entry_t  fail_entry;
    uint32        val, oval = 0;
    bcm_pbmp_t    fail_ports, hgp;
    bcm_port_t    hg_failport;
    int           ix, fs_ports;
#ifdef BCM_GREYHOUND2_SUPPORT
    uint64        val64_hi, oval64_hi;
    uint64        hg_trunk_bmap_f;
#endif /* BCM_GREYHOUND2_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
    uint64        val64, oval64;
    COMPILER_64_ZERO(oval64);
#endif

#ifdef BCM_GREYHOUND2_SUPPORT
    COMPILER_64_ZERO(oval64_hi);
#endif /* BCM_GREYHOUND2_SUPPORT */

    if(SOC_IS_SC_CQ(unit) && !IS_GX_PORT(unit, failport)) {
        /* We should never hit this case for a fabric trunk */
        return BCM_E_PORT;
    }

    sal_memset(&fail_entry, 0, sizeof(fail_entry));

    if (hw_count) { /* Enabling HG trunk failover */
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_HURRICANE3(unit) ||
            soc_feature(unit, soc_feature_high_portcount_register)) {
            /* These devices have a 1-1 mapping for HG port number */
            hg_failport = failport;
        } else {
            SOC_IF_ERROR_RETURN
                (soc_xgs3_port_num_to_higig_port_num(unit, failport,
                                                     &hg_failport));
        }
        if (soc_mem_field_valid(unit, HG_TRUNK_FAILOVER_SETm, 
                                FAILOVER_SET_SIZEf)) {
            soc_mem_field32_set(unit, HG_TRUNK_FAILOVER_SETm,
                               (uint32 *)&fail_entry,
                               FAILOVER_SET_SIZEf, hw_count - 1);
        }
        soc_mem_field32_set(unit, HG_TRUNK_FAILOVER_SETm,
                            (uint32 *)&fail_entry, RTAGf, rtag);

        fs_ports = SOC_IS_SC_CQ(unit) ?
            _BCM_SCORPION_FABRIC_TRUNK_FAILOVER_PORT_NUM :
            (SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) ? 
            _BCM_HURRICANE2_TRUNK_FAILOVER_PORT_NUM :
            _BCM_TRUNK_FAILOVER_PORT_NUM;

        for (ix = 0; ix < fs_ports; ix++) {
            soc_mem_field32_set(unit, HG_TRUNK_FAILOVER_SETm,
                                (uint32 *)&fail_entry,
                                _hg_trunk_failover_portf[ix],
                                ftp[ix % hw_count]);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_HG_TRUNK_FAILOVER_SETm(unit, MEM_BLOCK_ALL, hg_failport,
                                          &fail_entry));
    }

    /* Update HG trunk mode selection bitmap */
    SOC_PBMP_CLEAR(fail_ports);
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_64r(unit, &val64));
        COMPILER_64_SET(oval64, COMPILER_64_HI(val64),
                        COMPILER_64_LO(val64));
        SOC_PBMP_WORD_SET(fail_ports, 0,
                 soc_reg64_field32_get(unit, HG_TRUNK_FAILOVER_ENABLE_64r,
                                                val64, BITMAP_LOf));
        SOC_PBMP_WORD_SET(fail_ports, 1,
                 soc_reg64_field32_get(unit, HG_TRUNK_FAILOVER_ENABLE_64r,
                                                val64, BITMAP_HIf));
    } else
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_high_portcount_register)) {
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_LOr(unit, &val64));
        COMPILER_64_SET(oval64, COMPILER_64_HI(val64),
                        COMPILER_64_LO(val64));
        hg_trunk_bmap_f = soc_reg64_field_get(unit,
            HG_TRUNK_FAILOVER_ENABLE_LOr, val64, BITMAPf);

        SOC_PBMP_WORD_SET(fail_ports, 0, COMPILER_64_LO(hg_trunk_bmap_f));
        SOC_PBMP_WORD_SET(fail_ports, 1, COMPILER_64_HI(hg_trunk_bmap_f));

        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_HIr(unit, &val64_hi));
        COMPILER_64_SET(oval64_hi, COMPILER_64_HI(val64_hi),
                        COMPILER_64_LO(val64_hi));
        hg_trunk_bmap_f = soc_reg64_field_get(unit,
            HG_TRUNK_FAILOVER_ENABLE_HIr, val64_hi, BITMAPf);

        SOC_PBMP_WORD_SET(fail_ports, 2, COMPILER_64_LO(hg_trunk_bmap_f));

    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLEr(unit, &val));
        oval = val;
        SOC_PBMP_CLEAR(hgp);
        SOC_PBMP_WORD_SET(hgp, 0,
                          soc_reg_field_get(unit, HG_TRUNK_FAILOVER_ENABLEr,
                                            val, BITMAPf));
        SOC_IF_ERROR_RETURN
            (soc_xgs3_higig_bitmap_to_bitmap(unit, hgp, &fail_ports));
    }

    if (hw_count) {
        SOC_PBMP_PORT_ADD(fail_ports, failport);
    } else {
        SOC_PBMP_PORT_REMOVE(fail_ports, failport);
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        soc_reg64_field32_set(unit, HG_TRUNK_FAILOVER_ENABLE_64r, &val64,
                              BITMAP_LOf, SOC_PBMP_WORD_GET(fail_ports, 0));
        soc_reg64_field32_set(unit, HG_TRUNK_FAILOVER_ENABLE_64r, &val64,
                              BITMAP_HIf, SOC_PBMP_WORD_GET(fail_ports, 1));
        if (COMPILER_64_NE(val64, oval64)) {
            SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_FAILOVER_ENABLE_64r(unit, val64));
        }
    } else
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_high_portcount_register)) {
        COMPILER_64_SET(hg_trunk_bmap_f, SOC_PBMP_WORD_GET(fail_ports, 1),
                        SOC_PBMP_WORD_GET(fail_ports, 0));
        soc_reg64_field_set(unit, HG_TRUNK_FAILOVER_ENABLE_LOr, &val64,
                              BITMAPf, hg_trunk_bmap_f);
        COMPILER_64_SET(hg_trunk_bmap_f, 0,
                        SOC_PBMP_WORD_GET(fail_ports, 2));
        soc_reg64_field_set(unit, HG_TRUNK_FAILOVER_ENABLE_HIr, &val64_hi,
                              BITMAPf, hg_trunk_bmap_f);
        if (COMPILER_64_NE(val64, oval64)) {
            SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_FAILOVER_ENABLE_LOr(unit, val64));
        }
        if (COMPILER_64_NE(val64_hi, oval64_hi)) {
            SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_FAILOVER_ENABLE_HIr(unit, val64_hi));
        }
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        SOC_PBMP_CLEAR(hgp);
        SOC_IF_ERROR_RETURN
            (soc_xgs3_bitmap_to_higig_bitmap(unit, fail_ports, &hgp));
        soc_reg_field_set(unit, HG_TRUNK_FAILOVER_ENABLEr, &val, BITMAPf,
                          SOC_PBMP_WORD_GET(hgp, 0));
        if (val != oval) {
            SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_FAILOVER_ENABLEr(unit, val));
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_hwfailover_hg_read(int unit, bcm_port_t failport,
                                   int array_size, int *rtag,
                                   bcm_port_t *ftp, bcm_module_t *ftm,
                                   int *array_count)
{
    hg_trunk_failover_set_entry_t  fail_entry;
    uint32        val;
    bcm_pbmp_t    fail_ports, hgp;
    bcm_port_t    hg_failport;
    int           ix, hw_count = 0;
#ifdef BCM_TRIUMPH2_SUPPORT
    uint64        val64;
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    uint64        hg_trunk_bmap_f;
#endif /* BCM_GREYHOUND2_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        /* These devices have a 1-1 mapping for HG port number */
        hg_failport = failport;
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_64r(unit, &val64));
        SOC_PBMP_WORD_SET(fail_ports, 0,
                          soc_reg64_field32_get(unit, HG_TRUNK_FAILOVER_ENABLE_64r,
                                                val64, BITMAP_LOf));
        SOC_PBMP_WORD_SET(fail_ports, 1,
                          soc_reg64_field32_get(unit, HG_TRUNK_FAILOVER_ENABLE_64r,
                                                val64, BITMAP_HIf));
    } else
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_high_portcount_register)) {
        /* These devices have a 1-1 mapping for HG port number */
        hg_failport = failport;
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_LOr(unit, &val64));
        hg_trunk_bmap_f = soc_reg64_field_get(unit,
            HG_TRUNK_FAILOVER_ENABLE_LOr, val64, BITMAPf);

        SOC_PBMP_WORD_SET(fail_ports, 0, COMPILER_64_LO(hg_trunk_bmap_f));
        SOC_PBMP_WORD_SET(fail_ports, 1, COMPILER_64_HI(hg_trunk_bmap_f));

        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLE_HIr(unit, &val64));
        hg_trunk_bmap_f = soc_reg64_field_get(unit,
            HG_TRUNK_FAILOVER_ENABLE_HIr, val64, BITMAPf);

        SOC_PBMP_WORD_SET(fail_ports, 2, COMPILER_64_LO(hg_trunk_bmap_f));
    } else
#endif /* BCM_GREYHOUND2_SUPPORT */
    {
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
            hg_failport = failport;
        } else {
            SOC_IF_ERROR_RETURN
                (soc_xgs3_port_num_to_higig_port_num(unit, failport,
                                                     &hg_failport));
        }
        SOC_IF_ERROR_RETURN(READ_HG_TRUNK_FAILOVER_ENABLEr(unit, &val));
        SOC_PBMP_CLEAR(hgp);
        SOC_PBMP_WORD_SET(hgp, 0,
                          soc_reg_field_get(unit, HG_TRUNK_FAILOVER_ENABLEr,
                                            val, BITMAPf));
        SOC_PBMP_CLEAR(fail_ports);
        SOC_IF_ERROR_RETURN
            (soc_xgs3_higig_bitmap_to_bitmap(unit, hgp, &fail_ports));
    }

    if (SOC_PBMP_MEMBER(fail_ports, failport)) {
        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_FAILOVER_SETm(unit, MEM_BLOCK_ALL, hg_failport,
                                         &fail_entry));

        if (soc_mem_field_valid(unit, HG_TRUNK_FAILOVER_SETm, 
                                FAILOVER_SET_SIZEf)) {
            hw_count = soc_mem_field32_get(unit, HG_TRUNK_FAILOVER_SETm,
                                           (uint32 *)&fail_entry,
                                           FAILOVER_SET_SIZEf) + 1;
        } else {
            hw_count = SOC_IS_SC_CQ(unit) ?
            _BCM_SCORPION_FABRIC_TRUNK_FAILOVER_PORT_NUM :
            (SOC_IS_HURRICANE(unit) || SOC_IS_HURRICANE2(unit)) ? 
            _BCM_HURRICANE2_TRUNK_FAILOVER_PORT_NUM :
            _BCM_TRUNK_FAILOVER_PORT_NUM;
        }

        if (((NULL != ftp) || (NULL != ftm)) && (hw_count > array_size)) {
            hw_count = array_size;
        }
        for (ix = 0; ix < hw_count; ix++) {
            if (NULL != ftm) {
                ftm[ix] = 0;
            }
            if (NULL != ftp) {
                ftp[ix] = soc_mem_field32_get(unit, HG_TRUNK_FAILOVER_SETm,
                        (uint32 *)&fail_entry,
                        _hg_trunk_failover_portf[ix]);
            }
        }
        if (NULL != rtag) {
            *rtag = soc_mem_field32_get(unit, HG_TRUNK_FAILOVER_SETm,
                                        (uint32 *)&fail_entry, RTAGf);
        }
        if (NULL != array_count) {
            *array_count = hw_count;
        }
    } else {
        if (NULL != array_count) {
            *array_count = 0;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_hwfailover_write(int unit, bcm_port_t failport, int rtag,
                                 int hw_count,
                                 bcm_port_t *ftp, bcm_module_t *ftm)
{
    port_lag_failover_set_entry_t  fail_entry;
    uint32 val, oval;
    int    ix, mirror_enable;

    if(SOC_IS_SC_CQ(unit) && !IS_GX_PORT(unit, failport)) {
        /* Not supported, but no error due to trunk config implicit use */
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_GXPORT_LAG_FAILOVER_STATUSr(unit, failport, &val));
    if (soc_reg_field_get(unit, GXPORT_LAG_FAILOVER_STATUSr,
                          val, LAG_FAILOVER_LOOPBACKf)) {
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
            return BCM_E_PORT;
        }
    }

    if (hw_count) { /* Enabling trunk failover */
        /* We must be in directed mirroring mode to use this feature */
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

        for (ix = 0; ix < _BCM_TRUNK_FAILOVER_PORT_NUM; ix++) {
            soc_mem_field32_set(unit, PORT_LAG_FAILOVER_SETm,
                                (uint32 *)&fail_entry,
                                _trunk_failover_portf[ix],
                                ftp[ix % hw_count]);
            soc_mem_field32_set(unit, PORT_LAG_FAILOVER_SETm,
                                (uint32 *)&fail_entry,
                                _trunk_failover_modulef[ix],
                                ftm[ix % hw_count]);
        }
        SOC_IF_ERROR_RETURN
            (WRITE_PORT_LAG_FAILOVER_SETm(unit, MEM_BLOCK_ALL, failport,
                                          &fail_entry));
    }

    SOC_IF_ERROR_RETURN
        (READ_GXPORT_LAG_FAILOVER_CONFIGr(unit, failport, &val));
    oval = val;
    soc_reg_field_set(unit, GXPORT_LAG_FAILOVER_CONFIGr, &val,
                      LAG_FAILOVER_ENf, (hw_count != 0) ? 1 : 0);
    if (val != oval) {
        SOC_IF_ERROR_RETURN
            (WRITE_GXPORT_LAG_FAILOVER_CONFIGr(unit, failport, val));
    }

    /* Notify linkscan of failover status */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_link_failover_set(unit, failport, (hw_count != 0)));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_hwfailover_read(int unit, bcm_port_t failport,
                                int array_size, int *rtag,
                                bcm_port_t *ftp, bcm_module_t *ftm,
                                int *array_count)
{
    port_lag_failover_set_entry_t fail_entry;
    uint32 val;
    int    ix, hw_count, enable = FALSE;


    if(IS_GX_PORT(unit, failport)) {
        SOC_IF_ERROR_RETURN
            (READ_GXPORT_LAG_FAILOVER_CONFIGr(unit, failport, &val));
        enable = soc_reg_field_get(unit, GXPORT_LAG_FAILOVER_CONFIGr, val,
                                   LAG_FAILOVER_ENf);
    }

    if (enable) {
        SOC_IF_ERROR_RETURN
            (READ_PORT_LAG_FAILOVER_SETm(unit, MEM_BLOCK_ALL, failport,
                                         &fail_entry));
        hw_count = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
                                       (uint32 *)&fail_entry,
                                       FAILOVER_SET_SIZEf) + 1;
        if (((NULL != ftp) || (NULL != ftm)) && (hw_count > array_size)) {
            hw_count = array_size;
        }
        for (ix = 0; ix < hw_count; ix++) {
            if (NULL != ftp) {
                ftp[ix] = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
                                              (uint32 *)&fail_entry,
                                              _trunk_failover_portf[ix]);
            }
            if (NULL != ftm) {
                ftm[ix] = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
                                              (uint32 *)&fail_entry,
                                              _trunk_failover_modulef[ix]);
            }
        }
        if (NULL != rtag) {
            *rtag = soc_mem_field32_get(unit, PORT_LAG_FAILOVER_SETm,
                                        (uint32 *)&fail_entry, RTAGf);
        }
        if (NULL != array_count) {
            *array_count = hw_count;
        }
    } else {
        if (NULL != array_count) {
            *array_count = 0;
        }
        if (NULL != rtag) {
            *rtag = 0;
        }
    }

    return BCM_E_NONE;
}

int
_bcm_xgs3_trunk_hwfailover_set(int unit, bcm_trunk_t tid, int hg_trunk,
                               bcm_port_t port, bcm_module_t modid,
                               int psc, uint32 flags, int count, 
                               bcm_port_t *ftp, bcm_module_t *ftm)
{
    int ix, tix, tid_ix, tports, next, local, mod_is_local, hw_count, rtag;
    uint8 tp_mod;
    uint16 fail_modport;
    bcm_gport_t gport;
    bcm_port_t local_port, port_list[BCM_XGS3_TRUNK_MAX_PORTCNT];
    bcm_module_t modid_list[BCM_XGS3_TRUNK_MAX_PORTCNT];
    _xgs3_hw_tinfo_t           *hwft;

    /* Need the HW port number for the table write later */
    if (hg_trunk) {
        fail_modport = local_port = port;
    } else {
        BCM_GPORT_MODPORT_SET(gport, modid, port);
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &local_port));

        fail_modport = (modid << 8) | port;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_trunk_psc_to_rtag(unit, psc, &rtag));

    tid_ix = tid + (hg_trunk ? _BCM_FP_TRUNK_NUM_MAX : 0);
    hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix]);

    /* Where is our port in the trunk list? */
    tports = hwft->num_ports;
    for (tix = 0; tix < tports; tix++) {
        if (fail_modport == hwft->modport[tix]) {
            break;
        }
    }

    if ((0 != count) && (tix == tports)) {
        /* Port not in trunk! */
        return BCM_E_NOT_FOUND;
    }

    if (flags) {
        switch (flags) {
        case BCM_TRUNK_FLAG_FAILOVER_NEXT_LOCAL:
            next = TRUE;
            local = TRUE;
            break;
        case BCM_TRUNK_FLAG_FAILOVER_NEXT:
            next = TRUE;
            local = FALSE;
            break;
        case BCM_TRUNK_FLAG_FAILOVER_ALL_LOCAL:
            next = FALSE;
            local = TRUE;
            break;
        case BCM_TRUNK_FLAG_FAILOVER_ALL:
            next = FALSE;
            local = FALSE;
            break;
        default:
            /* Illegal flags setting */
            return BCM_E_PARAM;
        }

        hw_count = 0;

        for (ix = ((tix + 1) % tports);  /* Start after fail port */
            ix != tix;                  /* Stop when we get back */
            ix = ((ix + 1) % tports)) { /* Cycle through trunk port list */
            tp_mod = hwft->modport[ix] >> 8;
            if (local && !hg_trunk) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_modid_is_local(unit, tp_mod,
                                             &mod_is_local));
                if (!mod_is_local) {
                    continue;
                }
            }
            port_list[hw_count] = hwft->modport[ix] & 0xff;
            modid_list[hw_count] = tp_mod;
            hw_count++;
            if (next) {
                break;
            }
        }
    } else {
        if (count) {
            for (ix = 0; ix < count; ix++) {
                port_list[ix] = ftp[ix];
                modid_list[ix] = ftm[ix];
            }
        }
        hw_count = count;
    }

    if (hg_trunk) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_hwfailover_hg_write(unit, local_port, rtag,
                                        hw_count, port_list));
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_hwfailover_write(unit, local_port, rtag,
                                        hw_count, port_list, modid_list));
    }

    if (tix < hwft->num_ports) {
        hwft->flags[tix] = flags;
        hwft->psc[tix] = psc;
    }
    return BCM_E_NONE;
}

int
_bcm_xgs3_trunk_hwfailover_get(int unit, bcm_trunk_t tid, int hg_trunk,
                               bcm_port_t port, bcm_module_t modid,
                               int *psc, uint32 *flags, int array_size,
                               bcm_port_t *ftp, bcm_module_t *ftm, 
                               int *array_count)
{
    int tix, tid_ix, tports;
    uint16 fail_modport;
    bcm_gport_t gport;
    bcm_port_t local_port;
    _xgs3_hw_tinfo_t           *hwft;

    /* Need the HW port number for the table write later */
    if (hg_trunk) {
        fail_modport = local_port = port;
    } else {
        BCM_GPORT_MODPORT_SET(gport, modid, port);
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, &local_port));

        fail_modport = (modid << 8) | port;
    }

    tid_ix = tid + (hg_trunk ? _BCM_FP_TRUNK_NUM_MAX : 0);
    hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix]);

    /* Where is our port in the trunk list? */
    tports = hwft->num_ports;
    for (tix = 0; tix < tports; tix++) {
        if (fail_modport == hwft->modport[tix]) {
            break;
        }
    }

    if (tix == tports) {
        /* Port not in trunk! */
        return BCM_E_NOT_FOUND;
    }

    *psc = hwft->psc[tix];

    if (hwft->flags[tix]) {
        /* Just return the flags value */
        *flags = hwft->flags[tix];
        *array_count = 0;
    } else {
        /* Access the HW to get the list */
        *flags = 0;
        if (hg_trunk) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_hwfailover_hg_read(unit, local_port,
                                 array_size, NULL, ftp, ftm, array_count));
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_hwfailover_read(unit, local_port,
                                 array_size, NULL, ftp, ftm, array_count));
        }
    }

    return BCM_E_NONE;
}

/*
 *Trunk group set/destroy Trunk Failover config
 */
STATIC int
_bcm_xgs3_trunk_failover_set(int unit, bcm_trunk_t tid, int hg_trunk,
                             _esw_trunk_add_info_t *add_info)
{
    int tix, mod_is_local, tid_ix;
    bcm_port_t tp_port;
    bcm_module_t tp_mod = -1;
    _xgs3_hw_tinfo_t           *hwft;
    int num_hwft_ports;
    bcm_port_t   port_array[BCM_XGS3_TRUNK_MAX_PORTCNT];
    bcm_module_t mod_array[BCM_XGS3_TRUNK_MAX_PORTCNT];
    int failover_flags;

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

    tid_ix = tid + (hg_trunk ? _BCM_FP_TRUNK_NUM_MAX : 0);
    hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix]);

    if ((NULL == add_info) || (0 != failover_flags)) {
        for (tix = 0; tix < hwft->num_ports; tix++) {
            if (!hg_trunk) {
                tp_mod = hwft->modport[tix] >> 8;
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_modid_is_local(unit, tp_mod, &mod_is_local));
                if (!mod_is_local) {
                    continue;
                }
            }
            tp_port = hwft->modport[tix] & 0xff;
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_hwfailover_set(unit, tid, hg_trunk,
                                                tp_port, tp_mod,
                                                hwft->psc[tix],
                                                0, 0, NULL, NULL));
        }
    }
    /* Else, updating trunk with add_info->flags == 0,
     * do not change failover configuration */

    if (NULL == add_info) {
        /* We're deleting the trunk, flush the info structure */
        hwft->num_ports = 0;
        return BCM_E_NONE;
    }

    /* Record trunk data */
    num_hwft_ports = 0;
    for (tix = 0; tix < add_info->num_ports; tix++) {
        if (add_info->member_flags[tix] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            continue;
        }
        hwft->flags[num_hwft_ports] = failover_flags;
        hwft->psc[num_hwft_ports] = add_info->psc;
        hwft->modport[num_hwft_ports] = add_info->tp[tix];
        port_array[num_hwft_ports] = add_info->tp[tix];
        if (!hg_trunk) {
            hwft->modport[num_hwft_ports] |= (add_info->tm[tix] << 8);
            mod_array[num_hwft_ports] = add_info->tm[tix];
        }
        num_hwft_ports++; 
    }
    hwft->num_ports = num_hwft_ports;

    /* No flags in trunk_set means no failover changes */
    if (0 != failover_flags) {
        /* Configure new trunk failover info */
        for (tix = 0; tix < hwft->num_ports; tix++) {
            if (!hg_trunk) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_modid_is_local(unit, hwft->modport[tix] >> 8,
                                             &mod_is_local));
                if (!mod_is_local) { 
                    continue;
                }
            }
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_hwfailover_set(unit, tid, hg_trunk, 
                           hwft->modport[tix] & 0xff,
                           hwft->modport[tix] >> 8,
                           hwft->psc[tix], hwft->flags[tix], hwft->num_ports,
                           port_array, mod_array));
        }
    }
    return BCM_E_NONE;
}

#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
/*
 * Update port table for fabric trunks.
 * SOC_IS_HBX only.
 */
STATIC int
_bcm_xgs3_trunk_fabric_port_set(int unit,
				bcm_trunk_t hgtid,
				pbmp_t old_ports,
				pbmp_t new_ports)
{
    bcm_port_t		port;
    int			ctid;

    PBMP_PORT_ITER(unit, port) {
        if (!(IS_HG_PORT(unit, port) || IS_HL_PORT(unit, port))) {
            continue;
        }
        if (BCM_PBMP_MEMBER(new_ports, port)) {
            ctid = hgtid;
        } else if (BCM_PBMP_MEMBER(old_ports, port)) {
            ctid = -1;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
                /* Clear HW failover info for deleted port */
                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_trunk_hwfailover_set(unit, hgtid, TRUE,
                                                    port, -1,
                                                    BCM_TRUNK_PSC_DEFAULT,
                                                    0, 0, NULL, NULL));
            }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
        } else {
            continue;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port,
                                      _bcmPortHigigTrunkId, ctid));
    }
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_BRADLEY_SUPPORT */

STATIC int
_bcm_xgs3_trunk_fabric_set(int unit,
                           bcm_trunk_t hgtid,
                           _esw_trunk_add_info_t *add_info,
                           trunk_private_t *t_info)
{
    int                i, local_id;
    int port[BCM_XGS3_TRUNK_MAX_PORTCNT];
    pbmp_t             old_trunk_pbmp, new_trunk_pbmp, hg_pbm, pbm;
    bcm_gport_t gport;
    bcm_module_t mod_id;
    bcm_trunk_t trunk, tid;

    SOC_PBMP_CLEAR(new_trunk_pbmp);
    tid = hgtid + _BCM_FP_TRUNK_NUM_MAX;
    for (i = 0; i < add_info->num_ports; i++) {
        /* if given modid is -1 devport was used */
        if (BCM_MODID_INVALID != add_info->tm[i]) {
            BCM_GPORT_MODPORT_SET(gport, add_info->tm[i], add_info->tp[i]);

            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport, &mod_id,
                        &port[i], &trunk, &local_id));
        } else {
            port[i] = add_info->tp[i];
        }

        if (!SOC_PORT_VALID(unit, port[i])) {
            return BCM_E_PORT;
        }
        if (!IS_ST_PORT(unit, port[i])) {
            return BCM_E_PARAM;
        }
        SOC_PBMP_PORT_ADD(new_trunk_pbmp, port[i]);

        /* Save new trunk member info */
        MEMBER_INFO(unit, tid).modport[i] = port[i];
        MEMBER_INFO(unit, tid).member_flags[i] = add_info->member_flags[i];
    }
    MEMBER_INFO(unit, tid).num_ports = add_info->num_ports;

    if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) && 
        soc_feature(unit, soc_feature_hg_trunking)) {
        uint32              hg_group_entry, hg_cntl_entry;
        uint32              hgp, pword;
        soc_field_t        *trunkf, rtagf, bmapf; 
        bcm_port_t          port;
        int                 cena, ctid;
#if defined(BCM_FIREBOLT2_SUPPORT)
        soc_field_t         sizef;
#endif /* BCM_FIREBOLT2_SUPPORT */

        switch (hgtid) {
        case 0:
            trunkf = _hg_trunk0f;
            rtagf = HIGIG_TRUNK_RTAG0f;
            bmapf = HIGIG_TRUNK_BITMAP0f;
#if defined(BCM_FIREBOLT2_SUPPORT)
            sizef = HIGIG_TRUNK_SIZE0f;
#endif /* BCM_FIREBOLT2_SUPPORT */
            break;
        case 1:
            trunkf = _hg_trunk1f;
            rtagf = HIGIG_TRUNK_RTAG1f;
            bmapf = HIGIG_TRUNK_BITMAP1f;
#if defined(BCM_FIREBOLT2_SUPPORT)
            sizef = HIGIG_TRUNK_SIZE1f;
#endif /* BCM_FIREBOLT2_SUPPORT */
            break;
        default:
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(READ_HIGIG_TRUNK_GROUPr(unit, &hg_group_entry));

        for (i = 0; i < 4; i++) {
            if (SOC_IS_RAVEN(unit)) { 
                /* For Raven, write port number directly */ 
                hgp = add_info->tp[i%add_info->num_ports];
            } else { 
                /* For other FBX, write higig port number */
                BCM_IF_ERROR_RETURN(
                    soc_xgs3_port_to_higig_bitmap(unit, 
                             add_info->tp[i%add_info->num_ports], &hgp));
                BCM_IF_ERROR_RETURN(
                    soc_xgs3_higig_bitmap_to_higig_port_num(hgp, &hgp));
            }
            soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                              trunkf[i], hgp);
        }

#if defined(BCM_FIREBOLT2_SUPPORT)
        if (soc_feature(unit, soc_feature_trunk_group_size)) {
            soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                               sizef, add_info->num_ports-1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
        soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                          rtagf, t_info->rtag);
        SOC_IF_ERROR_RETURN(WRITE_HIGIG_TRUNK_GROUPr(unit, hg_group_entry));

        SOC_IF_ERROR_RETURN(READ_HIGIG_TRUNK_CONTROLr(unit, &hg_cntl_entry));
        SOC_PBMP_CLEAR(old_trunk_pbmp);
        pword = soc_reg_field_get(unit, HIGIG_TRUNK_CONTROLr,
                                  hg_cntl_entry, bmapf);

        if (SOC_IS_RAVEN(unit)) {
            /* Use the bitmap directly */
            SOC_PBMP_WORD_SET(old_trunk_pbmp, 0, pword);
        } else { 
            /* For other FBX, map the higig bitmap to the physical bitmap */
            SOC_PBMP_WORD_SET(hg_pbm, 0, pword);
            SOC_PBMP_CLEAR(pbm);
            soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbm, &pbm);
            SOC_PBMP_ASSIGN(old_trunk_pbmp, pbm);
        }
        PBMP_ST_ITER(unit, port) {
            if (BCM_PBMP_MEMBER(new_trunk_pbmp, port)) {
                cena = 1;
                ctid = hgtid;
            } else if (BCM_PBMP_MEMBER(old_trunk_pbmp, port)) {
                cena = 0;
                ctid = 0;
            } else {
                continue;
            }
            BCM_IF_ERROR_RETURN(
                soc_xgs3_port_to_higig_bitmap(unit, port, &hgp));
            BCM_IF_ERROR_RETURN(
                soc_xgs3_higig_bitmap_to_higig_port_num(hgp, &hgp));
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              _hg_cntl1f[hgp], cena);
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              _hg_cntl0f[hgp], ctid);
        }
        if (SOC_IS_RAVEN(unit)) {
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              bmapf, SOC_PBMP_WORD_GET(new_trunk_pbmp, 0));
        } else { 
            SOC_PBMP_CLEAR(hg_pbm);
            soc_xgs3_bitmap_to_higig_bitmap(unit, new_trunk_pbmp, &hg_pbm);
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              bmapf, SOC_PBMP_WORD_GET(hg_pbm, 0));
        } 
        SOC_IF_ERROR_RETURN(WRITE_HIGIG_TRUNK_CONTROLr(unit, hg_cntl_entry));

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   t_info->rtag,
                                                   new_trunk_pbmp));
        return BCM_E_NONE;
    }
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        uint64          val64;

        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_BITMAP_64r(unit, hgtid, &val64));
        SOC_PBMP_CLEAR(hg_pbm);
        SOC_PBMP_WORD_SET(hg_pbm, 0,
                          soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                val64, HIGIG_TRUNK_BITMAP_LOf));
        SOC_PBMP_WORD_SET(hg_pbm, 1,
                          soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                val64, HIGIG_TRUNK_BITMAP_HIf));
        SOC_PBMP_CLEAR(old_trunk_pbmp);
        SOC_PBMP_ASSIGN(old_trunk_pbmp, hg_pbm);

        COMPILER_64_ZERO(val64);
        SOC_PBMP_CLEAR(hg_pbm);
        SOC_PBMP_ASSIGN(hg_pbm, new_trunk_pbmp);
        soc_reg64_field32_set(unit, HG_TRUNK_BITMAP_64r, &val64,
                              HIGIG_TRUNK_BITMAP_LOf, 
                              SOC_PBMP_WORD_GET(hg_pbm, 0));
        soc_reg64_field32_set(unit, HG_TRUNK_BITMAP_64r, &val64, 
                              HIGIG_TRUNK_BITMAP_HIf, 
                              SOC_PBMP_WORD_GET(hg_pbm, 1));
        SOC_IF_ERROR_RETURN
            (WRITE_HG_TRUNK_BITMAP_64r(unit, hgtid, val64));

        COMPILER_64_ZERO(val64);
        for (i = 0; i < 8; i++) {
            soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                _hg_trunk[i], port[i%add_info->num_ports]);
        }
        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                              HIGIG_TRUNK_SIZEf, add_info->num_ports-1);
        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                              HIGIG_TRUNK_RTAGf, t_info->rtag);
        SOC_IF_ERROR_RETURN
            (WRITE_HG_TRUNK_GROUPr(unit, hgtid, val64));
        COMPILER_64_ZERO(val64);

	BCM_IF_ERROR_RETURN
	    (_bcm_xgs3_trunk_fabric_port_set(unit,
					     hgtid,
					     old_trunk_pbmp,
					     new_trunk_pbmp));

        if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_failover_set(unit, hgtid, TRUE, add_info));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   t_info->rtag,
                                                   new_trunk_pbmp));

        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
        uint32          val;
        uint64          val64;
#if defined(BCM_GREYHOUND2_SUPPORT)
        uint64          hg_trunk_bmap_f;
        if (soc_feature(unit, soc_feature_high_portcount_register)) {
            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAP_LOr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_LOr,
                                    val64, HIGIG_TRUNK_BITMAPf);
            SOC_PBMP_CLEAR(hg_pbm);
            SOC_PBMP_WORD_SET(hg_pbm, 0, COMPILER_64_LO(hg_trunk_bmap_f));
            SOC_PBMP_WORD_SET(hg_pbm, 1, COMPILER_64_HI(hg_trunk_bmap_f));
            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAP_HIr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_HIr,
                                    val64, HIGIG_TRUNK_BITMAPf);
            SOC_PBMP_WORD_SET(hg_pbm, 2, COMPILER_64_LO(hg_trunk_bmap_f));

            SOC_PBMP_CLEAR(old_trunk_pbmp);
            SOC_PBMP_ASSIGN(old_trunk_pbmp, hg_pbm);

            SOC_PBMP_CLEAR(hg_pbm);
            SOC_PBMP_ASSIGN(hg_pbm, new_trunk_pbmp);

            COMPILER_64_ZERO(val64);
            COMPILER_64_SET(hg_trunk_bmap_f, SOC_PBMP_WORD_GET(hg_pbm, 1),
                            SOC_PBMP_WORD_GET(hg_pbm, 0));
            soc_reg64_field_set(unit, HG_TRUNK_BITMAP_LOr, &val64,
                                HIGIG_TRUNK_BITMAPf, hg_trunk_bmap_f);
            SOC_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_BITMAP_LOr(unit, hgtid, val64));

            COMPILER_64_ZERO(val64);
            COMPILER_64_SET(hg_trunk_bmap_f, 0,
                            SOC_PBMP_WORD_GET(hg_pbm, 2));
            soc_reg64_field_set(unit, HG_TRUNK_BITMAP_HIr, &val64,
                                HIGIG_TRUNK_BITMAPf, hg_trunk_bmap_f);
            SOC_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_BITMAP_HIr(unit, hgtid, val64));

        } else
#endif /* BCM_GREYHOUND2_SUPPORT */
        {
            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAPr(unit, hgtid, &val));
            SOC_PBMP_CLEAR(hg_pbm);
            SOC_PBMP_WORD_SET(hg_pbm, 0,
                              soc_reg_field_get(unit, HG_TRUNK_BITMAPr,
                                                val, HIGIG_TRUNK_BITMAPf));
            SOC_PBMP_CLEAR(old_trunk_pbmp);
            soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbm, &old_trunk_pbmp);

            val = 0;
            SOC_PBMP_CLEAR(hg_pbm);
            soc_xgs3_bitmap_to_higig_bitmap(unit, new_trunk_pbmp, &hg_pbm);
            soc_reg_field_set(unit, HG_TRUNK_BITMAPr, &val, HIGIG_TRUNK_BITMAPf,
                              SOC_PBMP_WORD_GET(hg_pbm, 0));
            SOC_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_BITMAPr(unit, hgtid, val));
        }

        COMPILER_64_ZERO(val64);
        if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit) ||
            SOC_IS_HURRICANE2(unit) ||
            soc_feature(unit, soc_feature_hg_trunk_group_members_max_4)) {
            for (i = 0; i < 4; i++) {
                soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                                      _hg_trunk[i], 
                                      add_info->tp[i%add_info->num_ports]);
            }
        } else {
            for (i = 0; i < 8; i++) {
                soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                                      _hg_trunk[i], 
                                      add_info->tp[i%add_info->num_ports]);
            }
        }
        if (SOC_REG_FIELD_VALID(unit, HG_TRUNK_GROUPr, HIGIG_TRUNK_SIZEf)) { /* PR 34459 */
            soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                                  HIGIG_TRUNK_SIZEf, add_info->num_ports-1);
        }
        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &val64,
                              HIGIG_TRUNK_RTAGf, t_info->rtag);
        SOC_IF_ERROR_RETURN
            (WRITE_HG_TRUNK_GROUPr(unit, hgtid, val64));
        COMPILER_64_ZERO(val64);

#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_IS_SC_CQ(unit)) {
            for (i = 8; i < 16; i++) {
                soc_reg64_field32_set(unit, HG_TRUNK_GROUP_HIr, &val64,
                          _hg_trunk_hi[i - BCM_SWITCH_TRUNK_MAX_PORTCNT], 
                                      add_info->tp[i%add_info->num_ports]);
            }
            SOC_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_GROUP_HIr(unit, hgtid, val64));
        }
#endif

	BCM_IF_ERROR_RETURN
	    (_bcm_xgs3_trunk_fabric_port_set(unit,
					     hgtid,
					     old_trunk_pbmp,
					     new_trunk_pbmp));

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_failover_set(unit, hgtid, TRUE, add_info));
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   t_info->rtag,
                                                   new_trunk_pbmp));

        return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_xgs3_trunk_property_migrate
 * Purpose:
 *      Migrate trunk properties from old members to new members of the trunk
 *      group
 * Parameters:
 *      unit        - (IN) SOC unit number. 
 *      num_leaving - (IN) Number of old members leaving the trunk group
 *      leaving_arr - (IN) Array containing leaving members
 *      num_staying - (IN) Number of old members staying in the trunk group
 *      staying_arr - (IN) Array containing staying members
 *      num_joining - (IN) Number of new members joining the trunk group
 *      joining_arr - (IN) Array containing joining members
 * Returns:
 *      BCM_X_XXX
 */
int 
_bcm_xgs3_trunk_property_migrate(int unit,
                int num_leaving, bcm_gport_t *leaving_arr,
                int num_staying, bcm_gport_t *staying_arr,
                int num_joining, bcm_gport_t *joining_arr)
{
    bcm_module_t mod;
    bcm_port_t port;
    int stm_idx;
    source_trunk_map_table_entry_t stm_entry;
    int rv;

    if ((num_leaving > 0) && (NULL == leaving_arr)) {
        return BCM_E_PARAM;
    }
    if ((num_staying > 0) && (NULL == staying_arr)) {
        return BCM_E_PARAM;
    }
    if ((num_joining > 0) && (NULL == joining_arr)) {
        return BCM_E_PARAM;
    }

    if (num_leaving > 0) {
        if (!BCM_GPORT_IS_MODPORT(leaving_arr[0])) {
            return BCM_E_PARAM;
        }
        mod = BCM_GPORT_MODPORT_MODID_GET(leaving_arr[0]);
        port = BCM_GPORT_MODPORT_PORT_GET(leaving_arr[0]);

    } else if (num_staying > 0) {
        if (!BCM_GPORT_IS_MODPORT(staying_arr[0])) {
            return BCM_E_PARAM;
        }
        mod = BCM_GPORT_MODPORT_MODID_GET(staying_arr[0]);
        port = BCM_GPORT_MODPORT_PORT_GET(staying_arr[0]);

    } else {
        /* Both num_leaving and num_staying are zeroes. This is a newly
         * created trunk group. There is no trunk properties to migrate.
         */ 
        return BCM_E_NONE;
    }

    soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);
    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod,
                port, &stm_idx);

    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx,
                                      &stm_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        return rv;
    }

    if (soc_feature(unit, soc_feature_vlan_translation_range)) {
#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(unit)) {
            soc_field_t vlan_range_field;
            soc_field_t inner_vlan_range_field;
            int32 old_inner_vlan_range_idx, old_vlan_range_idx;
            int32 inner_vlan_range_idx, vlan_range_idx;
            int32 i;

            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm,
                        OUTER_VLAN_RANGE_IDXf)) {
                vlan_range_field = OUTER_VLAN_RANGE_IDXf;
            } else if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm,
                                           VLAN_RANGE_IDXf)) {
                vlan_range_field = VLAN_RANGE_IDXf;
            } else {
                vlan_range_field = INVALIDf;
            }

            if (soc_mem_field_valid(unit, SOURCE_TRUNK_MAP_TABLEm,
                        INNER_VLAN_RANGE_IDXf)) {
                inner_vlan_range_field = INNER_VLAN_RANGE_IDXf;
            } else {
                inner_vlan_range_field = INVALIDf;
            }

            /* If VLAN RANGE fields are not valid to the device, return */
            if ((vlan_range_field == INVALIDf) && 
                (inner_vlan_range_field == INVALIDf)) {
                soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                return BCM_E_NONE;
            }

            /* Get trunk group's VLAN_RANGE_IDX */
            vlan_range_idx = (vlan_range_field != INVALIDf) ? 
                             soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                             &stm_entry, vlan_range_field): -1;

            /* Get trunk group's INNER_VLAN_RANGE_IDX */
            inner_vlan_range_idx = (inner_vlan_range_field != INVALIDf) ?
                                   soc_mem_field32_get(unit, 
                                   SOURCE_TRUNK_MAP_TABLEm,
                                   &stm_entry, inner_vlan_range_field): -1;

            /* Update VLAN_RANGE_IDX for members joining the trunk group */
            for (i = 0; i < num_joining; i++) {
                if (!BCM_GPORT_IS_MODPORT(joining_arr[i])) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return BCM_E_PARAM;
                }
                mod = BCM_GPORT_MODPORT_MODID_GET(joining_arr[i]);
                port = BCM_GPORT_MODPORT_PORT_GET(joining_arr[i]);

                rv = _bcm_esw_src_mod_port_table_index_get(unit,
                            mod, port, &stm_idx);
                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }

                rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit,
                            MEM_BLOCK_ANY, stm_idx, &stm_entry);
                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }

                if (vlan_range_field != INVALIDf) { 
                /* Delete port's old VLAN range profile */
                  old_vlan_range_idx = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(
                                       unit, &stm_entry, vlan_range_field);

                  rv = _bcm_trx_vlan_range_profile_entry_delete(
                                       unit, old_vlan_range_idx);

                  if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                  }

                /* Set port to use trunk group's VLAN range profile */
                  _bcm_trx_vlan_range_profile_entry_increment(unit, 
                                                                vlan_range_idx);
                soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry,
                        vlan_range_field, vlan_range_idx);
                rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,
                            MEM_BLOCK_ALL, stm_idx, &stm_entry);
                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }
            }

                if (inner_vlan_range_field != INVALIDf) {
                   /* Delete port's old INNER VLAN range profile */
                   old_inner_vlan_range_idx = 
                       soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry,
                                                        inner_vlan_range_field);
                   rv = _bcm_trx_vlan_range_profile_entry_delete(
                                                     unit,
                                                     old_inner_vlan_range_idx);

                   if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                   }

                   /* Set port to use trunk group's INNER VLAN range profile */
                   _bcm_trx_vlan_range_profile_entry_increment(unit, 
                                                          inner_vlan_range_idx);
                   soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry,
                                  inner_vlan_range_field, inner_vlan_range_idx);
                   rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,
                                           MEM_BLOCK_ALL, stm_idx, &stm_entry);
                   if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                   }
                }
            }

            /* Update VLAN_RANGE_IDX for members leaving the trunk group */
            for (i = 0; i < num_leaving; i++) {
                if (!BCM_GPORT_IS_MODPORT(leaving_arr[i])) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return BCM_E_PARAM;
                }
                mod = BCM_GPORT_MODPORT_MODID_GET(leaving_arr[i]);
                port = BCM_GPORT_MODPORT_PORT_GET(leaving_arr[i]);
                rv = _bcm_esw_src_mod_port_table_index_get(unit,
                            mod, port, &stm_idx);
                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }

                rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit,
                            MEM_BLOCK_ANY, stm_idx, &stm_entry);

                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }

                if (vlan_range_field != INVALIDf) { 
                /* Delete port's old VLAN range profile */
                   old_vlan_range_idx = 
                      soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, 
                                                              &stm_entry, 
                                                              vlan_range_field);
                   rv = _bcm_trx_vlan_range_profile_entry_delete(unit,
                                                            old_vlan_range_idx);
                   if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                   }

                /* Set port to use the default VLAN range profile 0 */
                _bcm_trx_vlan_range_profile_entry_increment(unit, 0);
                   soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, 
                                                         &stm_entry,
                                                         vlan_range_field, 0);
                rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,
                            MEM_BLOCK_ALL, stm_idx, &stm_entry);
                if(BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                    return rv;
                }
            }

                if (inner_vlan_range_field != INVALIDf) {
                   /* Delete port's old INNER VLAN range profile */
                   old_inner_vlan_range_idx = 
                       soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &stm_entry,
                                                        inner_vlan_range_field);
                   rv = _bcm_trx_vlan_range_profile_entry_delete(unit,
                                                     old_inner_vlan_range_idx);
                   if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                   }

                   /* Set port to use trunk group's INNER VLAN range profile */
                   _bcm_trx_vlan_range_profile_entry_increment(unit, 0); 
                   soc_SOURCE_TRUNK_MAP_TABLEm_field32_set(unit, &stm_entry,
                                                          inner_vlan_range_field,
                                                          0);
                   rv = WRITE_SOURCE_TRUNK_MAP_TABLEm(unit,
                                           MEM_BLOCK_ALL, stm_idx, &stm_entry);

                   if(BCM_FAILURE(rv)) {
                       soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
                       return rv;
                   }
                }
            }

        }
#endif /* BCM_TRX_SUPPORT */
    }

    soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_trunk_set
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
_bcm_xgs3_trunk_set(int unit,
        bcm_trunk_t tid,
        _esw_trunk_add_info_t *add_info,
        trunk_private_t *t_info)
{
    int rv = BCM_E_NONE;
    trunk_bitmap_entry_t        trunk_bitmap_entry;
    uint32                      tg_entry[SOC_MAX_MEM_WORDS];
    pbmp_t                      old_trunk_pbmp;
    pbmp_t                      new_trunk_pbmp;
    pbmp_t                      nuc_trunk_pbmp;
    bcm_gport_t                 gport;
    bcm_port_t                  loc_port;
    int                         port, mod, mod_out, port_out;
    int                         i, j, k, remove_trunk_port;
    int                         hwmod[BCM_XGS3_TRUNK_MAX_PORTCNT];
    int                         egr_hwmod[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
    int                         uc_egr_hwmod[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
    int                         hwport[BCM_XGS3_TRUNK_MAX_PORTCNT];
    int                         egr_hwport[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
    int                         uc_egr_hwport[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
    uint32                      egr_member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
    int                         max_ports = BCM_SWITCH_TRUNK_MAX_PORTCNT;
    bcm_trunk_chip_info_t       chip_info;
    uint32                      my_modid_bmap = 0;
    uint32                      nuc_member_count = 0;
    uint32                      nuc_tpbm = 0;
    uint32                      negr_disable_port = 0;
    uint32                      num_uc_egr_disable_port = 0;
    int                         num_leaving_members = 0;
    bcm_gport_t                 leaving_members[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    int                         match_leaving_members;
    int                         num_staying_members = 0;
    bcm_gport_t                 staying_members[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    int                         match_staying_members;
    int                         num_joining_members = 0;
    bcm_gport_t                 joining_members[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    int                         match_joining_members;
    soc_mem_t                   trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if (chip_info.trunk_fabric_id_min >= 0 &&
        tid >= chip_info.trunk_fabric_id_min) {        /* fabric trunk */
        tid -= chip_info.trunk_fabric_id_min;
        /* coverity[stack_use_callee] */
        /* coverity[stack_use_overflow] */
        return _bcm_xgs3_trunk_fabric_set(unit, tid, add_info, t_info);
    }

    if (add_info->flags & BCM_TRUNK_FLAG_IPMC_CLEAVE) {
        return BCM_E_UNAVAIL;
    }

    /* Audit unsupported flag combination */
    for (i = 0; i < add_info->num_ports; i++) {
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE &&
            add_info->member_flags[i] & BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                return BCM_E_PARAM;
        }
    }

    if (add_info->dlf_index == -1) {
        BCM_IF_ERROR_RETURN(_bcm_nuc_tpbm_get(unit, 
                                              add_info->num_ports, 
                                              add_info->tm, 
                                              &nuc_tpbm));
    } else {
        nuc_tpbm = (0x1 << add_info->dlf_index);
    }

    SOC_PBMP_CLEAR(new_trunk_pbmp);
    SOC_PBMP_CLEAR(nuc_trunk_pbmp);
    for (i = 0; i < add_info->num_ports; i++) {
        mod = add_info->tm[i];
        port = add_info->tp[i];
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                     mod, port,
                                     &mod_out, &port_out));
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }

        /** Don't add the port in trunk group if egress disable is set */
        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
            nuc_tpbm &= ~(1 << i); 
            negr_disable_port++;
        } else if (add_info->member_flags[i] &
                BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
            num_uc_egr_disable_port++;
        }

        /* Save mapped version */
        hwmod[i] = mod_out;
        hwport[i] = port_out;

        BCM_GPORT_MODPORT_SET(gport, mod_out, port_out);
        if (BCM_SUCCESS(bcm_esw_port_local_get(unit, gport, &loc_port))) { 
            if (!IS_E_PORT(unit, loc_port)) {
                return BCM_E_PARAM;
            }
            SOC_PBMP_PORT_ADD(new_trunk_pbmp, loc_port);
            if (nuc_tpbm & (1 << i)) {
                /* Eligible for NUC forwarding */
                SOC_PBMP_PORT_ADD(nuc_trunk_pbmp, loc_port);
                my_modid_bmap |= (1 << nuc_member_count);
            }
        }

        if (nuc_tpbm & (1 << i)) {
            /* Eligible for NUC forwarding */
            nuc_member_count++;
        }
    }

    /*
     * Clean up old trunk membership 
     */
    SOC_PBMP_CLEAR(old_trunk_pbmp);
    if (t_info->in_use) {
        for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
            mod = MEMBER_INFO(unit, tid).modport[i] >> 8;
            port = MEMBER_INFO(unit, tid).modport[i] & 0xff;
            BCM_GPORT_MODPORT_SET(gport, mod, port);
            if (BCM_SUCCESS(bcm_esw_port_local_get(unit, gport, &loc_port))) {
                if (SOC_PORT_VALID(unit, loc_port)) {
                    SOC_PBMP_PORT_ADD(old_trunk_pbmp, loc_port);
                }
            } else {
                loc_port = BCM_GPORT_INVALID;
            }
            /* Invalidate port only if it is not a part of the new trunk group */
            remove_trunk_port = TRUE;
            for (j = 0; j < add_info->num_ports; j++) {
                /* Note: Compare with the mapped version as thats the one set in h/w */
                if (mod == hwmod[j] && port == hwport[j]) { 
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

                BCM_IF_ERROR_RETURN
                    (_bcm_xgs3_trunk_set_port_property(unit, mod, port, -1));
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                if (soc_feature(unit, soc_feature_port_lag_failover) &&
                    (BCM_GPORT_INVALID != loc_port) &&
                    IS_E_PORT(unit, loc_port)) 
                {
                    /* Clear HW failover info for deleted port */
                    BCM_IF_ERROR_RETURN
                        (_bcm_xgs3_trunk_hwfailover_set(unit, tid, FALSE,
                                                        port, mod,
                                                        add_info->psc,
                                                        0, 0, NULL, NULL));
                }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
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

    /* Generate array containing new members joining the trunk group */
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

    sal_memset(&tg_entry, 0, sizeof(tg_entry));

    if (soc_feature(unit, soc_feature_fastlag)) {
        soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, t_info->rtag);
    } else {
        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, t_info->rtag);
    }

    if (soc_feature(unit, soc_feature_trunk_group_size)) { /* TG_SIZE field exists */
        if ( (add_info->num_ports - negr_disable_port -
                    num_uc_egr_disable_port) == 0 ) {
            if (soc_feature(unit, soc_feature_fastlag)) {
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf, 0);
            } else {
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf, 0);
            }
        } else {
            if (soc_feature(unit, soc_feature_fastlag)) {
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf,
                                                  add_info->num_ports - 1
                                                  - negr_disable_port
                                                  - num_uc_egr_disable_port);
            } else {
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                                             TG_SIZEf, add_info->num_ports - 1
                                             - negr_disable_port
                                             - num_uc_egr_disable_port);
            }
        }
    }

    if ((add_info->num_ports - negr_disable_port - num_uc_egr_disable_port) > 0 ) {
        for (i = 0, j = 0; i < add_info->num_ports; i++) {
            if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
                continue;
	    }
            egr_hwmod[j] = hwmod[i];
            egr_hwport[j] = hwport[i];
            egr_member_flags[j] = add_info->member_flags[i];
            j++;
        }
        for (i = 0, k = 0; i < j; i++) {
            if (egr_member_flags[i] & BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                continue;
	    }
            uc_egr_hwmod[k] = egr_hwmod[i];
            uc_egr_hwport[k] = egr_hwport[i];
            k++;
        }
        if (k != 0) {
            for (i = 0; i < max_ports; i++) {
                mod = uc_egr_hwmod[i%k];
                port = uc_egr_hwport[i%k];
                if (soc_feature(unit, soc_feature_fastlag)) {
                    soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_fast_modulef[i], mod);
                    soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_fast_portf[i], port);
                } else {
                    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_modulef[i], mod);
                    soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_portf[i], port);
                }
            }
        }
    } else {
        /** no ports are enabled in the trunk */
        for ( i = 0; i < max_ports; i++ ) {
	    /** drop packets */
            if (soc_feature(unit, soc_feature_fastlag)) {
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_fast_modulef[i], 0);
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_fast_portf[i], 0);
            } else {
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_modulef[i], 0);
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, _xgs_portf[i], 0);
            }
        }
    }

    /** Don't add the port in the source trunk map if ingress disable is set */
    for (i = 0; i < add_info->num_ports; i++) {
        mod = hwmod[i%add_info->num_ports];
        port = hwport[i%add_info->num_ports];

        if (add_info->member_flags[i] & BCM_TRUNK_MEMBER_INGRESS_DISABLE) {
            BCM_IF_ERROR_RETURN(_bcm_xgs3_trunk_set_port_property(unit, mod, 
                                                                  port, -1));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_xgs3_trunk_set_port_property(unit, mod, 
                                                                  port, tid));
        }
    }    

    /*
     * Update TRUNK_BITMAP table
     */
    BCM_IF_ERROR_RETURN(
        soc_mem_read(unit, TRUNK_BITMAPm, MEM_BLOCK_ANY, tid,
                     &trunk_bitmap_entry));
    soc_mem_pbmp_field_set(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
                           TRUNK_BITMAPf, &new_trunk_pbmp);
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, TRUNK_BITMAPm, MEM_BLOCK_ALL, tid,
                      &trunk_bitmap_entry));
    /*
     * Update TRUNK_GROUP table
     */
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, trunk_mem, MEM_BLOCK_ALL, tid, &tg_entry));

    /*
     * Update the trunk block mask
     */
    if (SOC_PBMP_NOT_NULL(new_trunk_pbmp) ||
        SOC_PBMP_NOT_NULL(old_trunk_pbmp)) {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_trunk_block_mask(unit,
                                       old_trunk_pbmp,
                                       new_trunk_pbmp,
                                       nuc_trunk_pbmp,
                                       my_modid_bmap,
                                       nuc_member_count));
    }
    /*
     * Trunk Failover
     */
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_failover_set(unit, tid, FALSE, add_info));
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    BCM_IF_ERROR_RETURN
       (_bcm_xgs3_trunk_swfailover_set(unit,
                                       tid,
                                       t_info->rtag,
                                       add_info->num_ports - negr_disable_port,
                                       egr_hwmod,
                                       egr_hwport,
                                       egr_member_flags));

    /* Migrate trunk properties from old members to new members */
    BCM_IF_ERROR_RETURN(_bcm_xgs3_trunk_property_migrate(unit,
                num_leaving_members, leaving_members,
                num_staying_members, staying_members,
                num_joining_members, joining_members));

    /* Save new trunk member info */
    for (i = 0; i < add_info->num_ports; i++) {
        MEMBER_INFO(unit, tid).modport[i] = ((hwmod[i] & 0xff) << 8) |
                                            (hwport[i] & 0xff);
        MEMBER_INFO(unit, tid).member_flags[i] = add_info->member_flags[i];
    }
    MEMBER_INFO(unit, tid).num_ports = add_info->num_ports;

#ifdef BCM_GREYHOUND2_SUPPORT
#ifdef INCLUDE_L3
    /* Call VXLAN module to clear states for local leaving members */
    if (soc_feature(unit, soc_feature_vxlan_lite)) {
        if (num_leaving_members > 0) {
            rv = _bcm_esw_vxlan_trunk_member_delete(unit, tid,
                                                    num_leaving_members,
                                                    leaving_members);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }
#endif /* INCLUDE_L3 */
#endif /* BCM_GREYHOUND2_SUPPORT */

    return rv;
}

/*
 * Function:
 *      _xgs3_trunk_add_info_member_alloc
 * Purpose:
 *      Allocate member_flags, tp, and tm of _esw_trunk_add_info_t.
 * Parameters:
 *      add_info  - (IN) Pointer to trunk add info structure.
 *      num_ports - (IN) Number of member ports.
 * Returns:
 *      BCM_E_xxx
 */
STATIC int
_xgs3_trunk_add_info_member_alloc(_esw_trunk_add_info_t *add_info,
        int num_ports)
{
    int rv = BCM_E_NONE;

    add_info->member_flags = sal_alloc(sizeof(uint32) * num_ports,
            "_xgs3_trunk_add_info_member_flags");
    if (add_info->member_flags == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->member_flags, 0, sizeof(uint32) * num_ports);

    add_info->tp = sal_alloc(sizeof(bcm_port_t) * num_ports,
            "_xgs3_trunk_add_info_tp");
    if (add_info->tp == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->tp, 0, sizeof(bcm_port_t) * num_ports);

    add_info->tm = sal_alloc(sizeof(bcm_module_t) * num_ports,
            "_xgs3_trunk_add_info_tm");
    if (add_info->tm == NULL) {
        rv = BCM_E_MEMORY;
        goto error;
    }
    sal_memset(add_info->tm, 0, sizeof(bcm_module_t) * num_ports);

    return rv;

error:
    if (NULL != add_info->member_flags) {
        sal_free(add_info->member_flags);
    }
    if (NULL != add_info->tp) {
        sal_free(add_info->tp);
    }
    if (NULL != add_info->tm) {
        sal_free(add_info->tm);
    }

    return rv;
}

/*
 * Function:
 *      _xgs3_trunk_add_info_member_free
 * Purpose:
 *      Free member_flags, tp, and tm of _esw_trunk_add_info_t.
 * Parameters:
 *      add_info  - (IN) Pointer to trunk add info structure.
 * Returns:
 *      BCM_E_xxx
 */
STATIC void 
_xgs3_trunk_add_info_member_free(_esw_trunk_add_info_t *add_info)
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
}

/*
 * Function:
 *      bcm_xgs3_trunk_modify
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
bcm_xgs3_trunk_modify(int unit,
        bcm_trunk_t tid,
        bcm_trunk_info_t *trunk_info,
        int member_count,
        bcm_trunk_member_t *member_array,
        trunk_private_t *t_info,
        int op,
        bcm_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_chip_info_t chip_info;
    _esw_trunk_add_info_t add_info;
    int i;
    int higig_trunk;

    if (trunk_info->psc <= 0) {
        trunk_info->psc = BCM_TRUNK_PSC_DEFAULT;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_trunk_psc_to_rtag(unit, trunk_info->psc, &(t_info->rtag)));

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
                (bcm_xgs3_trunk_destroy(unit, tid, t_info));
        }
    } else {
        sal_memset(&add_info, 0, sizeof(_esw_trunk_add_info_t));

        add_info.flags = trunk_info->flags;
        add_info.num_ports = member_count;
        add_info.psc = trunk_info->psc;
        add_info.ipmc_psc = trunk_info->ipmc_psc;
        add_info.dlf_index = trunk_info->dlf_index;
        add_info.mc_index = trunk_info->mc_index;
        add_info.ipmc_index = trunk_info->ipmc_index;

        BCM_IF_ERROR_RETURN
            (_xgs3_trunk_add_info_member_alloc(&add_info, member_count));

        for (i = 0; i < member_count; i++) {
            add_info.member_flags[i] = member_array[i].flags;
            add_info.tp[i] = member_array[i].gport;
            add_info.tm[i] = -1;
        }

        /* Convert member gport to module and port */
        BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
        if (chip_info.trunk_fabric_id_min >= 0 &&
            tid >= chip_info.trunk_fabric_id_min) {
            higig_trunk = 1;
        } else {
            higig_trunk = 0;
        }
        rv = _bcm_esw_trunk_gport_array_resolve(unit,
                higig_trunk,
                member_count,
                add_info.tp,
                add_info.tp,
                add_info.tm);
        if (BCM_FAILURE(rv)) {
            _xgs3_trunk_add_info_member_free(&add_info);
            return rv;
        }

        rv = _bcm_xgs3_trunk_set(unit, tid, &add_info, t_info);
        _xgs3_trunk_add_info_member_free(&add_info);
    }

    return rv;
}

STATIC int
_bcm_xgs3_trunk_fabric_destroy(int unit,
                               bcm_trunk_t hgtid,
                               trunk_private_t *t_info)
{
    int i, tid;

    /* Clear trunk member info */
    tid = hgtid + _BCM_FP_TRUNK_NUM_MAX;
    for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
        MEMBER_INFO(unit, tid).modport[i] = 0;
        MEMBER_INFO(unit, tid).member_flags[i] = 0;
    }
    MEMBER_INFO(unit, tid).num_ports = 0;

    if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) &&
        soc_feature(unit, soc_feature_hg_trunking)) {
        uint32                hg_group_entry, hg_cntl_entry;
        pbmp_t                pbmp, hg_pbmp;
        uint32                pword, hgp;
        soc_field_t          *trunkf, rtagf, bmapf; 
        bcm_port_t            port;
#if defined(BCM_FIREBOLT2_SUPPORT)
        soc_field_t           sizef;
#endif /* BCM_FIREBOLT2_SUPPORT */

        switch (hgtid) {
        case 0:
            trunkf = _hg_trunk0f;
            rtagf = HIGIG_TRUNK_RTAG0f;
            bmapf = HIGIG_TRUNK_BITMAP0f;
#if defined(BCM_FIREBOLT2_SUPPORT)
            sizef = HIGIG_TRUNK_SIZE0f;
#endif /* BCM_FIREBOLT2_SUPPORT */
            break;
        case 1:
            trunkf = _hg_trunk1f;
            rtagf = HIGIG_TRUNK_RTAG1f;
            bmapf = HIGIG_TRUNK_BITMAP1f;
#if defined(BCM_FIREBOLT2_SUPPORT)
            sizef = HIGIG_TRUNK_SIZE1f;
#endif /* BCM_FIREBOLT2_SUPPORT */
            break;
        default:
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(READ_HIGIG_TRUNK_CONTROLr(unit, &hg_cntl_entry));
        pword = soc_reg_field_get(unit, HIGIG_TRUNK_CONTROLr, hg_cntl_entry,
                                  bmapf);
        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_CLEAR(hg_pbmp);
        SOC_PBMP_WORD_SET(hg_pbmp, 0, pword);
        if (SOC_IS_RAVEN(unit)) {
            /* Use the bitmap directly for Raven */
            SOC_PBMP_ASSIGN(pbmp, hg_pbmp); 
        } else {
            /* For other FBX, map the read higig bitmap to the physical bitmap */
            soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbmp, &pbmp);
        }
        soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                          bmapf, 0);

        PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(
                soc_xgs3_port_to_higig_bitmap(unit, port, &hgp)); 
            BCM_IF_ERROR_RETURN(
                soc_xgs3_higig_bitmap_to_higig_port_num(hgp, &hgp));
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              _hg_cntl0f[hgp], 0);
            soc_reg_field_set(unit, HIGIG_TRUNK_CONTROLr, &hg_cntl_entry,
                              _hg_cntl1f[hgp], 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_HIGIG_TRUNK_CONTROLr(unit, hg_cntl_entry));

        SOC_IF_ERROR_RETURN(READ_HIGIG_TRUNK_GROUPr(unit, &hg_group_entry));
        for (i = 0; i < 4; i++) {
            soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                              trunkf[i], 0);
        }
#if defined(BCM_FIREBOLT2_SUPPORT)
        if (soc_feature(unit, soc_feature_trunk_group_size)) {
            soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                              sizef, 0);
        }          
#endif /* BCM_FIREBOLT2_SUPPORT */
        soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hg_group_entry,
                          rtagf, 0);
        SOC_IF_ERROR_RETURN(WRITE_HIGIG_TRUNK_GROUPr(unit, hg_group_entry));

        SOC_PBMP_CLEAR(hg_pbmp);
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   0,
                                                   hg_pbmp));

        t_info->in_use = FALSE;
        return BCM_E_NONE;
    }
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        uint64		val64;
        pbmp_t		old_trunk_pbmp, new_trunk_pbmp, hg_pbm;

        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_BITMAP_64r(unit, hgtid, &val64));
        SOC_PBMP_CLEAR(hg_pbm);
        SOC_PBMP_WORD_SET(hg_pbm, 0,
                          soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                val64, HIGIG_TRUNK_BITMAP_LOf));
        SOC_PBMP_WORD_SET(hg_pbm, 1,
                          soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                                val64, HIGIG_TRUNK_BITMAP_HIf));
        SOC_PBMP_CLEAR(old_trunk_pbmp);
        SOC_PBMP_ASSIGN(old_trunk_pbmp, hg_pbm);

        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_BITMAP_64r(unit, hgtid, val64));
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_GROUPr(unit, hgtid, val64));

        SOC_PBMP_CLEAR(new_trunk_pbmp);
	BCM_IF_ERROR_RETURN
	    (_bcm_xgs3_trunk_fabric_port_set(unit,
					     hgtid,
					     old_trunk_pbmp,
					     new_trunk_pbmp));

       if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_failover_set(unit, hgtid, TRUE, NULL));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   0,
                                                   new_trunk_pbmp));

        t_info->in_use = FALSE;
        return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_high_portcount_register)) {
        uint64 val64;
        uint64 hg_trunk_bmap_f;
        pbmp_t old_trunk_pbmp, new_trunk_pbmp, hg_pbm;

        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_BITMAP_LOr(unit, hgtid, &val64));
        hg_trunk_bmap_f =
            soc_reg64_field_get(unit, HG_TRUNK_BITMAP_LOr,
                                val64, HIGIG_TRUNK_BITMAPf);

        SOC_PBMP_CLEAR(hg_pbm);
        SOC_PBMP_WORD_SET(hg_pbm, 0, COMPILER_64_LO(hg_trunk_bmap_f));
        SOC_PBMP_WORD_SET(hg_pbm, 1, COMPILER_64_HI(hg_trunk_bmap_f));

        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_BITMAP_HIr(unit, hgtid, &val64));
        hg_trunk_bmap_f =
            soc_reg64_field_get(unit, HG_TRUNK_BITMAP_HIr,
                                val64, HIGIG_TRUNK_BITMAPf);
        SOC_PBMP_WORD_SET(hg_pbm, 2, COMPILER_64_LO(hg_trunk_bmap_f));

        SOC_PBMP_CLEAR(old_trunk_pbmp);
        SOC_PBMP_ASSIGN(old_trunk_pbmp, hg_pbm);

        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_BITMAP_LOr(unit, hgtid, val64));
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_BITMAP_HIr(unit, hgtid, val64));
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_GROUPr(unit, hgtid, val64));

        SOC_PBMP_CLEAR(new_trunk_pbmp);
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_trunk_fabric_port_set(unit, hgtid,
                                            old_trunk_pbmp, new_trunk_pbmp));

       if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_failover_set(unit, hgtid, TRUE, NULL));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit, hgtid,
                                                   0, new_trunk_pbmp));

        t_info->in_use = FALSE;
        return BCM_E_NONE;
    }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
        uint32		val;
        uint64		val64;
        pbmp_t		old_trunk_pbmp, new_trunk_pbmp, hg_pbm;

        SOC_IF_ERROR_RETURN
            (READ_HG_TRUNK_BITMAPr(unit, hgtid, &val));
        SOC_PBMP_CLEAR(hg_pbm);
        SOC_PBMP_WORD_SET(hg_pbm, 0,
                          soc_reg_field_get(unit, HG_TRUNK_BITMAPr,
                                            val, HIGIG_TRUNK_BITMAPf));
        SOC_PBMP_CLEAR(old_trunk_pbmp);
        soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbm, &old_trunk_pbmp);

        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_BITMAPr(unit, hgtid, 0));
        COMPILER_64_ZERO(val64);
        SOC_IF_ERROR_RETURN(WRITE_HG_TRUNK_GROUPr(unit, hgtid, val64));

#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_IS_SC_CQ(unit)) {
            SOC_IF_ERROR_RETURN
                (WRITE_HG_TRUNK_GROUP_HIr(unit, hgtid, val64));
        }
#endif
        SOC_PBMP_CLEAR(new_trunk_pbmp);
	BCM_IF_ERROR_RETURN
	    (_bcm_xgs3_trunk_fabric_port_set(unit,
					     hgtid,
					     old_trunk_pbmp,
					     new_trunk_pbmp));
        
        if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_trunk_failover_set(unit, hgtid, TRUE, NULL));
        }

        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_swfailover_fabric_set(unit,
                                                   hgtid,
                                                   0,
                                                   new_trunk_pbmp));

        t_info->in_use = FALSE;
        return BCM_E_NONE;
    }
#endif /* BCM_BRADLEY_SUPPORT */


    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_xgs3_trunk_destroy
 * Purpose:
 *      Multiplexed trunking function for XGS3
 */
int
bcm_xgs3_trunk_destroy(int unit,
                       bcm_trunk_t tid,
                       trunk_private_t *t_info)
{
    bcm_module_t                mod;
    bcm_port_t                  port, loc_port;
    bcm_gport_t                 gport;
    pbmp_t                      old_ports, new_ports;
    int                         i;
    bcm_trunk_chip_info_t       chip_info;
    int                         num_leaving_members = 0;
    bcm_gport_t                 leaving_members[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    soc_mem_t                   trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    SOC_PBMP_CLEAR(new_ports);
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if (chip_info.trunk_fabric_id_min >= 0 &&
        tid >= chip_info.trunk_fabric_id_min) {        /* fabric trunk */
        tid -= chip_info.trunk_fabric_id_min;
        return _bcm_xgs3_trunk_fabric_destroy(unit, tid, t_info);
    }

    /* front panel trunks */

    SOC_PBMP_CLEAR(old_ports);
    for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
        mod = MEMBER_INFO(unit, tid).modport[i] >> 8;
        port = MEMBER_INFO(unit, tid).modport[i] & 0xff;
        BCM_GPORT_MODPORT_SET(gport, mod, port);
        if (BCM_SUCCESS(bcm_esw_port_local_get(unit, gport, &loc_port))) { 
            SOC_PBMP_PORT_ADD(old_ports, loc_port);
        }
        leaving_members[num_leaving_members++] = gport;
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_set_port_property(unit, mod, port, -1));
    }

    /*
     * Remove ports from the block mask table
     */
    if (SOC_PBMP_NOT_NULL(old_ports)) {
        SOC_PBMP_CLEAR(new_ports);
        /* coverity[divide_by_zero] */
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_trunk_block_mask(unit, old_ports, new_ports, 
                                       new_ports, 0, 0));
    }

    /*
     * Reset table entries to empty state
     */
    if (soc_feature(unit, soc_feature_trunk_egress)) {
        BCM_IF_ERROR_RETURN(
            soc_mem_write(unit, TRUNK_EGR_MASKm, MEM_BLOCK_ALL, tid,
                          soc_mem_entry_null(unit, TRUNK_EGR_MASKm)));
    }
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, TRUNK_BITMAPm, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, TRUNK_BITMAPm)));
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, trunk_mem, MEM_BLOCK_ALL, tid,
                      soc_mem_entry_null(unit, trunk_mem)));
    /*
     * Remove ports from failover
     */
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_trunk_failover_set(unit, tid, FALSE, NULL));
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_trunk_swfailover_set(unit,
                                        tid,
                                        0,
                                        0,
                                        (int *)0,
                                        (int *)0,
                                        NULL));

    /* Clear trunk properties from members */
    BCM_IF_ERROR_RETURN(_bcm_xgs3_trunk_property_migrate(unit,
                num_leaving_members, leaving_members,
                0, NULL, 0, NULL));

    /* Clear trunk member info */
    for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
        MEMBER_INFO(unit, tid).modport[i] = 0;
        MEMBER_INFO(unit, tid).member_flags[i] = 0;
    }
    MEMBER_INFO(unit, tid).num_ports = 0;

    t_info->in_use = FALSE;
    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_fabric_get(int unit,
                           bcm_trunk_t hgtid,
                           _esw_trunk_add_info_t *t_data,
                           trunk_private_t *t_info)
{
    bcm_trunk_t tid;
    bcm_module_t modid;
    bcm_port_t port;
    int i;

    tid = hgtid + _BCM_FP_TRUNK_NUM_MAX;

    if (t_data->num_ports == 0) {
        t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
    } else {
        if (MEMBER_INFO(unit, tid).num_ports <= t_data->num_ports) {
            t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
        } 

        if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &modid))) {
            modid = 0;
        }

        for (i = 0; i < t_data->num_ports; i++) {
            port = MEMBER_INFO(unit, tid).modport[i] & 0xff;
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit,
                        BCM_STK_MODMAP_GET, modid, port,
                        &t_data->tm[i], &t_data->tp[i]));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_trunk_get
 * Purpose:
 *      Multiplexed trunking function for XGS3
 */
STATIC int
_bcm_xgs3_trunk_get(int unit,
                   bcm_trunk_t tid,
                   _esw_trunk_add_info_t *t_data,
                   trunk_private_t *t_info)
{
    bcm_module_t                mod_out;
    bcm_port_t                  port_out;
    int                         i;
    bcm_trunk_chip_info_t        chip_info;

    t_data->flags = t_info->flags;
    t_data->psc = t_info->psc;
    t_data->ipmc_psc = t_info->ipmc_psc;
    t_data->dlf_index = t_info->dlf_index_used;
    t_data->mc_index = t_info->mc_index_used;
    t_data->ipmc_index = t_info->ipmc_index_used;

    if (!t_info->in_use) {
        t_data->num_ports = 0;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if (chip_info.trunk_fabric_id_min >= 0 &&
        tid >= chip_info.trunk_fabric_id_min) { /* fabric trunk */
        tid -= chip_info.trunk_fabric_id_min;
        return _bcm_xgs3_trunk_fabric_get(unit, tid, t_data, t_info);
    }

    if (t_data->num_ports == 0) {
        t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
    } else {
        if (MEMBER_INFO(unit, tid).num_ports <= t_data->num_ports) {
            t_data->num_ports = MEMBER_INFO(unit, tid).num_ports;
        } 

        for (i = 0; i < t_data->num_ports; i++) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                         MEMBER_INFO(unit, tid).modport[i] >> 8,
                                         MEMBER_INFO(unit, tid).modport[i] & 0xff,
                                         &mod_out, &port_out));
            t_data->tm[i] = mod_out;
            t_data->tp[i] = port_out;
            /* Copy member flags */
            t_data->member_flags[i] = MEMBER_INFO(unit, tid).member_flags[i];
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_xgs3_trunk_get
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
bcm_xgs3_trunk_get(int unit,
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
            (_xgs3_trunk_add_info_member_alloc(&add_info, member_max));
    }

    rv = _bcm_xgs3_trunk_get(unit, tid, &add_info, t_info);
    if (BCM_FAILURE(rv)) {
        if (member_max > 0) {
            _xgs3_trunk_add_info_member_free(&add_info);
        }
        return rv;
    }

    trunk_info->flags = add_info.flags;
    trunk_info->psc = add_info.psc;
    trunk_info->ipmc_psc = add_info.ipmc_psc;
    trunk_info->dlf_index = add_info.dlf_index;
    trunk_info->mc_index = add_info.mc_index;
    trunk_info->ipmc_index = add_info.ipmc_index;

    *member_count = add_info.num_ports;

    if (member_max > 0) {
        if (member_max < *member_count) {
            *member_count = member_max;
        }

        /* Construct member gport */
        rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
        if (BCM_FAILURE(rv)) {
            _xgs3_trunk_add_info_member_free(&add_info);
            return rv;
        }
        if (chip_info.trunk_fabric_id_min >= 0 &&
            tid >= chip_info.trunk_fabric_id_min) {
            higig_trunk = 1;
        } else {
            higig_trunk = 0;
        }
        rv = _bcm_esw_trunk_gport_construct(unit, higig_trunk,
                *member_count,
                add_info.tp, add_info.tm,
                add_info.tp);
        if (BCM_FAILURE(rv)) {
            _xgs3_trunk_add_info_member_free(&add_info);
            return rv;
        }

        for (i = 0; i < *member_count; i++) {
            bcm_trunk_member_t_init(&member_array[i]);
            member_array[i].flags = add_info.member_flags[i];
            member_array[i].gport = add_info.tp[i];
        }

        _xgs3_trunk_add_info_member_free(&add_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_xgs3_trunk_mcast_join
 * Purpose:
 *      Multiplexed function for XGS3
 * Returns:
 *     BCM_E_NONE - Success
 *     BCM_E_INTERNAL - Read failure
 *     BCM_E_XXX      - from bcm_mcast_port_get() or bcm_mcast_port_add()
 */
int
bcm_xgs3_trunk_mcast_join(int unit, bcm_trunk_t tid, bcm_vlan_t vid,
                          sal_mac_addr_t mac, trunk_private_t *t_info)
{
    trunk_bitmap_entry_t        trunk_bitmap_entry;
    bcm_mcast_addr_t            mc_addr;
    bcm_trunk_chip_info_t chip_info;
    
    /* initialising and setting the mc_addr struct by bcm_mcast_addr_t_init() */
    bcm_mcast_addr_t_init(&mc_addr,mac,vid);

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
    if ((tid >= chip_info.trunk_id_min) && (tid <= chip_info.trunk_id_max)) {
        /* get the ports in the trunk group */
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, TRUNK_BITMAPm, MEM_BLOCK_ANY, tid,
                          &trunk_bitmap_entry));
        soc_mem_pbmp_field_get(unit, TRUNK_BITMAPm, &trunk_bitmap_entry,
                               TRUNK_BITMAPf, &mc_addr.pbmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Add the trunk group's ports to the multicast group. */
    return bcm_esw_mcast_port_add(unit, &mc_addr);
}

int
_bcm_xgs3_trunk_swfailover_init(int unit)
{
    if (_xgs3_trunk_swfail[unit] == NULL) {
        _xgs3_trunk_swfail[unit] = sal_alloc(sizeof(_xgs3_trunk_swfail_t), 
                                             "_xgs3_trunk_swfail");
        if (_xgs3_trunk_swfail[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_xgs3_trunk_swfail[unit], 0, sizeof(_xgs3_trunk_swfail_t));
    return BCM_E_NONE;
}

void
_bcm_xgs3_trunk_swfailover_detach(int unit)
{
    if (_xgs3_trunk_swfail[unit] != NULL) {
        sal_free(_xgs3_trunk_swfail[unit]);
        _xgs3_trunk_swfail[unit] = NULL;
    }
}

STATIC int
_bcm_xgs3_trunk_swfailover_set(int unit,
                               bcm_trunk_t tid,
                               int rtag,
                               int nports,
                               int mods[BCM_XGS3_TRUNK_MAX_PORTCNT],
                               int ports[BCM_XGS3_TRUNK_MAX_PORTCNT],
                               uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT])
{
    _xgs3_trunk_swfail_t       *swf;
    _xgs3_tinfo_t *swft;
    bcm_pbmp_t          localports;
    bcm_port_t          port;
    bcm_module_t        my_modid;
    int                 i;

    if (_xgs3_trunk_swfail[unit] == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_stk_my_modid_get(unit, &my_modid));

    swf = _xgs3_trunk_swfail[unit];
    swft = &swf->tinfo[tid];

    swf->mymodid = my_modid;
    swft->rtag = rtag;
    swft->num_ports = nports;
    BCM_PBMP_CLEAR(localports);
    for (i = 0; i < nports; i++) {
        swft->modport[i] = (mods[i] << 8) | ports[i];
        swft->member_flags[i] = member_flags[i];
        if (mods[i] == my_modid) {
            BCM_PBMP_PORT_ADD(localports, ports[i]);
        }
    }
    PBMP_ALL_ITER(unit, port) {
        if (BCM_PBMP_MEMBER(localports, port)) {
            swf->trunk[port] = tid + 1;
        } else if (swf->trunk[port] == tid + 1) {
            swf->trunk[port] = 0;       /* remove stale pointers to tid */
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_xgs3_trunk_swfailover_fabric_set(int unit,
                                      bcm_trunk_t hgtid,
                                      int rtag,
                                      bcm_pbmp_t ports)
{
    _xgs3_trunk_swfail_t       *swf;
    _xgs3_tinfo_t              *swft;
    bcm_port_t                 port;
    int                        nports;
 
    if (_xgs3_trunk_swfail[unit] == NULL) {
        return BCM_E_INIT;
    }

    swf = _xgs3_trunk_swfail[unit];
    swft = &swf->tinfo[hgtid + _BCM_FP_TRUNK_NUM_MAX];

    swft->rtag = rtag;
    nports = 0;
    PBMP_ITER(ports, port) {
        swft->modport[nports] = port;
        nports += 1;
    }
    swft->num_ports = nports;
    PBMP_ALL_ITER(unit, port) {
        if (BCM_PBMP_MEMBER(ports, port)) {
            swf->trunk[port] = hgtid + _BCM_FABRIC_TRUNK_SWF_TID_OFFSET;
        } else if (swf->trunk[port] ==
                   (hgtid + _BCM_FABRIC_TRUNK_SWF_TID_OFFSET)) {
            swf->trunk[port] = 0;       /* remove stale pointers to tid */
        }
    }
    return BCM_E_NONE;
}   

#ifdef BCM_TRUNK_FAILOVER_SUPPORT

/* fields from HIGIG_TRUNK_GROUPr */
static soc_field_t      _hg_trunkf[2][4] = {
  {
    HIGIG_TRUNK_ID0_PORT0f, HIGIG_TRUNK_ID0_PORT1f,
    HIGIG_TRUNK_ID0_PORT2f, HIGIG_TRUNK_ID0_PORT3f},
  {
    HIGIG_TRUNK_ID1_PORT0f, HIGIG_TRUNK_ID1_PORT1f,
    HIGIG_TRUNK_ID1_PORT2f, HIGIG_TRUNK_ID1_PORT3f}
};

static soc_field_t _hg_trunk_rtagf[2] = {
    HIGIG_TRUNK_RTAG0f, HIGIG_TRUNK_RTAG1f
};

/*
 * Re-programs fabric trunk
 */
STATIC void
_bcm_xgs3_trunk_fabric_swfailover_trigger(int unit,
                                          bcm_port_t port,
                                          int hgtid,
                                          _xgs3_trunk_swfail_t *swf)
{
    int i, j, k, nmp, p;
    _xgs3_tinfo_t *swft;

    swft = &swf->tinfo[hgtid + _BCM_FP_TRUNK_NUM_MAX];

    /* update the _xgs3_trunk_swfail entry */
    nmp = swft->num_ports;
    for (i = j = 0; i < nmp; i++) {
         if (swft->modport[i] != port) {
             if (i != j) {
                 swft->modport[j] = swft->modport[i];
             }
             j += 1;
         }
    }
           
    if (j == nmp) {
        return; /* no changes, so no need to write out */
    }

    swft->num_ports = nmp = j;

    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) {
        uint32  hgtrunk;
        hgtrunk = 0;

        for (k = j = 0; k < 2; k++) {
             swft = &swf->tinfo[_BCM_FP_TRUNK_NUM_MAX + k];
             nmp = swft->num_ports;
             soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hgtrunk,
                               _hg_trunk_rtagf[k], swft->rtag);
             if (nmp == 0) {
                 p = 0;
             } else {
                 if (SOC_IS_RAVEN(unit)) {
                     p = swft->modport[0];
                 } else {
                     if(soc_xgs3_port_to_higig_bitmap(unit, 
                                      swft->modport[0], (uint32 *)&p) != BCM_E_NONE) {
                        return;
                     }
                     if(soc_xgs3_higig_bitmap_to_higig_port_num(p, 
                                      (uint32 *)&p) != BCM_E_NONE) {
                        return;
                     }
                 }
             }

             for (i = 0; i < 4; i++) {
                  soc_reg_field_set(unit, HIGIG_TRUNK_GROUPr, &hgtrunk,
                                    _hg_trunkf[k][i], p);
                  if (nmp > 1) {
                      j += 1;
                      if (j >= nmp) {
                          j = 0;
                      }
                      if (SOC_IS_RAVEN(unit)) {
                          p = swft->modport[j];
                      } else {
                         if(soc_xgs3_port_to_higig_bitmap(unit, 
                                     swft->modport[j], (uint32 *)&p) != BCM_E_NONE) {
                            return;
                         }
                         if(soc_xgs3_higig_bitmap_to_higig_port_num(p, 
                                    (uint32 *)&p) != BCM_E_NONE) {
                            return;
                         }
                      }
                  }
             }
        }

        (void)WRITE_HIGIG_TRUNK_GROUPr(unit, hgtrunk);
    } else if (SOC_IS_HB_GW(unit)) {
#if defined(BCM_BRADLEY_SUPPORT)
        uint64  hgtrunk;

        COMPILER_64_ZERO(hgtrunk);
        if (nmp == 0) {
            p = 31;     /* just drop it */
        } else {
            p = swft->modport[0];
        }
        for (i = j = 0; i < 8; i++) {
             soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                                   _hg_trunk[i], p);
             if (nmp > 1) {
                 j += 1;
                 if (j >= nmp) {
                     j = 0;
                 }
                 p = swft->modport[j];
             }
        }

        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                              HIGIG_TRUNK_SIZEf, nmp ? nmp-1 : 0);
        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                              HIGIG_TRUNK_RTAGf, swft->rtag);
        (void)WRITE_HG_TRUNK_GROUPr(unit, hgtid, hgtrunk);
#endif  /* BCM_BRADLEY_SUPPORT */
    } else if (SOC_IS_SC_CQ(unit)) {
#if defined(BCM_SCORPION_SUPPORT)
        uint64  hgtrunk;
        

        COMPILER_64_ZERO(hgtrunk);
        if (nmp == 0) {
            p = 31;     /* just drop it */
        } else {
            p = swft->modport[0];
        }
        for (i = j = 0; i < 8; i++) {
             soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                                   _hg_trunk[i], p);
             if (nmp > 1) {
                 j += 1;
                 if (j >= nmp) {
                     j = 0;
                 }
                 p = swft->modport[j];
             }
        }

        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                              HIGIG_TRUNK_SIZEf, nmp ? nmp-1 : 0);
        soc_reg64_field32_set(unit, HG_TRUNK_GROUPr, &hgtrunk,
                              HIGIG_TRUNK_RTAGf, swft->rtag);
        (void)WRITE_HG_TRUNK_GROUPr(unit, hgtid, hgtrunk);
#endif  /* BCM_SCORPION_SUPPORT */
    }
}

int
_bcm_xgs3_trunk_swfailover_trigger(int unit,
                                   bcm_pbmp_t ports_active,
                                   bcm_pbmp_t ports_status)
{
    bcm_port_t           port;
    int                  tid;
    _xgs3_trunk_swfail_t *swf;
    _xgs3_tinfo_t        *swft;
    soc_mem_t            trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    swf = _xgs3_trunk_swfail[unit];

    if (swf == NULL) {
        return BCM_E_NONE;
    }

    PBMP_ITER(ports_active, port) {
        if (BCM_PBMP_MEMBER(ports_status, port)) {
            continue;   /* link up */
        }

        /* link down */
        tid = swf->trunk[port];
        swf->trunk[port] = 0;
        if (tid <= 0) {
            continue;   /* already removed */
        }

        if (tid >= 1 && tid < _BCM_FP_TRUNK_NUM_MAX + 1) {
            uint32               tg_entry[SOC_MAX_MEM_WORDS];
            uint16               modport;
            int                  i, j, nmp;
            int                  uc_nmp;
            pbmp_t               old_trunk_pbmp;
            pbmp_t               new_trunk_pbmp;
            pbmp_t               nuc_trunk_pbmp;
            trunk_bitmap_entry_t tb_entry;
            bcm_module_t         mods[BCM_XGS3_TRUNK_MAX_PORTCNT];
            uint16               uc_modport[BCM_XGS3_TRUNK_MAX_PORTCNT];
            int                  max_ports = BCM_SWITCH_TRUNK_MAX_PORTCNT;
            uint32               my_modid_bmap = 0;
            uint32               nuc_member_count = 0;
            uint32               nuc_tpbm = 0;

            tid = tid - 1;
            swft = &swf->tinfo[tid];
            SOC_PBMP_CLEAR(old_trunk_pbmp);
            SOC_PBMP_CLEAR(new_trunk_pbmp);

            /* update _xgs3_trunk_swfail: remove matching ports, update size */
            modport = (swf->mymodid << 8) | port;
            nmp = swft->num_ports;

            for (i = j = 0; i < nmp; i++) {
                if (swft->modport[i] != modport) {
                    if (i != j) {
                        swft->modport[j] = swft->modport[i];
                        swft->member_flags[j] = swft->member_flags[i];
                    }
                    j++;
                }
                if (swf->mymodid == (swft->modport[i] >> 8)) {
                    SOC_PBMP_PORT_ADD(old_trunk_pbmp, swft->modport[i] & 0xff);
                }
            }
            if (j == nmp) {
                continue; /* no changes, so no need to write out */
            }

            swft->num_ports = nmp = j;

            uc_nmp = 0;
            for (i = 0; i < nmp; i++) {
                if (swft->member_flags[i] &
                        BCM_TRUNK_MEMBER_UNICAST_EGRESS_DISABLE) {
                    continue;
                }
                uc_modport[uc_nmp++] = swft->modport[i];
            }

            /* convert _xgs3_trunk_swfail into trunk_group entry */
            if (soc_feature(unit, soc_feature_fastlag)) {
                soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, swft->rtag);
            } else {
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, RTAGf, swft->rtag);
            }
#if defined(BCM_BRADLEY_SUPPORT)
            if (SOC_IS_HBX(unit)) {
                soc_TRUNK_GROUPm_field32_set(unit, &tg_entry, TG_SIZEf,
                                             uc_nmp ? uc_nmp-1 : 0);
            }
#endif /* BCM_BRADLEY_SUPPORT */
            if (uc_nmp == 0) {
                for (i = 0; i < max_ports; i++) {
                    if (soc_feature(unit, soc_feature_fastlag)) {
                        soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_fast_modulef[i], swf->mymodid);
                        soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_fast_portf[i], SOC_PORT_ADDR_MAX(unit));
                    } else {
                        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_modulef[i], swf->mymodid);
                        soc_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_portf[i], SOC_PORT_ADDR_MAX(unit));
                    }
                    /* The assumption here is that SOC_PORT_ADDR_MAX is an
                     * invalid port number. Hardware will drop packets destined
                     * to an invalid port number.
                     */
                }
            } else {
                for (i = 0; i < max_ports; i++) {
                    if (soc_feature(unit, soc_feature_fastlag)) {
                        soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                             _xgs_fast_modulef[i], uc_modport[i%uc_nmp] >> 8);
                        soc_FAST_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                             _xgs_fast_portf[i], uc_modport[i%uc_nmp] & 0xff);
                    } else {
                       soc_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_modulef[i], uc_modport[i%uc_nmp] >> 8);
                       soc_TRUNK_GROUPm_field32_set(unit, &tg_entry,
                            _xgs_portf[i], uc_modport[i%uc_nmp] & 0xff);
                    }
                }
            }

            /* write out the trunk_group entry */
            (void)soc_mem_write(unit, trunk_mem, MEM_BLOCK_ALL,
                                tid, &tg_entry);

            /* update TRUNK_BITMAP table */
            SOC_PBMP_CLEAR(new_trunk_pbmp);
            for (i = 0; i < nmp; i++) {
                 if ((swft->modport[i] >> 8) == swf->mymodid) {
                     SOC_PBMP_PORT_ADD(new_trunk_pbmp, swft->modport[i] & 0xff);
                 }
            }
            (void)soc_mem_read(unit, TRUNK_BITMAPm, MEM_BLOCK_ANY, tid,
                               &tb_entry);
            soc_mem_pbmp_field_set(unit, TRUNK_BITMAPm, &tb_entry,
                                   TRUNK_BITMAPf, &new_trunk_pbmp);
            (void)soc_mem_write(unit, TRUNK_BITMAPm, MEM_BLOCK_ALL, tid,
                                &tb_entry);

            /* update non uc trunk block mask table */
            for (i = 0; i < nmp; i++) {
                mods[i] = swft->modport[i] >> 8;
            }
            BCM_IF_ERROR_RETURN(_bcm_nuc_tpbm_get(unit, nmp, mods, &nuc_tpbm));

            SOC_PBMP_CLEAR(nuc_trunk_pbmp);
            for (i= 0; i < nmp; i++) {
                 if (swf->mymodid == (swft->modport[i] >> 8)) {
                     /* local port */
                     if (nuc_tpbm & (1 << i)) {
                         /* Eligible for NUC forwarding */
                         SOC_PBMP_PORT_ADD(nuc_trunk_pbmp, 
                                           swft->modport[i] & 0xff);
                         my_modid_bmap |= (1 << nuc_member_count);
                     }
                 }

                 if (nuc_tpbm & (1 << i)) {
                     /* Eligible for NUC forwarding */
                     nuc_member_count++;
                 }
            }

            BCM_IF_ERROR_RETURN
               (_bcm_xgs3_trunk_block_mask(unit, 
                                           old_trunk_pbmp,
                                           new_trunk_pbmp,   
                                           nuc_trunk_pbmp,
                                           my_modid_bmap,
                                           nuc_member_count));
        } else if ((tid >= _BCM_FABRIC_TRUNK_SWF_TID_OFFSET) && 
                   (tid < (_BCM_FABRIC_TRUNK_NUM_MAX +
                           _BCM_FABRIC_TRUNK_SWF_TID_OFFSET))) {
            /* higig trunk */
            _bcm_xgs3_trunk_fabric_swfailover_trigger(unit,
                                                      port,
                            tid - _BCM_FABRIC_TRUNK_SWF_TID_OFFSET,
                                                      swf);
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRUNK_FAILOVER_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
/* Set psc argument to trunk group default first, then call this */
STATIC void
_bcm_xgs3_trunk_rtag_to_psc(int unit, int rtag, int *psc)
{
    if (7 == rtag) {
        if (soc_feature(unit,
                        soc_feature_port_flow_hash)) {
            *psc = BCM_TRUNK_PSC_PORTFLOW;
        } else if (soc_feature(unit,
                               soc_feature_port_trunk_index)) {
            *psc = BCM_TRUNK_PSC_PORTINDEX;
        } /* Else, unknown, use default */
    } else if (0 < rtag) {
        /* HW and SW values match */
        *psc = rtag;
    } else {
        /* Use trunk group value as the default */
    }
}

/*
 * Handle state recovery for front panel trunks
 */
int
_xgs3_trunk_reinit(int unit, int ngroups_fp, trunk_private_t *trunk_info)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        bcm_trunk_t tid, tid_get;
        trunk_private_t *t_info; 
        uint32 tg_entry[SOC_MAX_MEM_WORDS];
        bcm_module_t egr_hw_mods[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
        bcm_port_t egr_hw_ports[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
        uint32 egr_member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT] = { 0 };
        bcm_module_t om[BCM_XGS3_TRUNK_MAX_PORTCNT];
        pbmp_t op[BCM_XGS3_TRUNK_MAX_PORTCNT];
        int max_ports = BCM_SWITCH_TRUNK_MAX_PORTCNT;
        source_trunk_map_table_entry_t  stm; /* Hw entry buffer. */
        int i, j, num_mod, tgsize, ptype, idx;
        int psc, num_egr_ports;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        _xgs3_hw_tinfo_t  *hwft;
        int               tix, rtag, psc_local;
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
        soc_mem_t trunk_mem = TRUNK_GROUPm;
        soc_field_t tid_fld = TGIDf;

        if (soc_feature(unit, soc_feature_src_tgid_valid)) {
            tid_fld = SRC_TGIDf;
        }

        if (soc_feature(unit, soc_feature_fastlag)) {
            trunk_mem = FAST_TRUNK_GROUPm;
        }

        rv = BCM_E_NONE;
        tgsize = soc_mem_entry_words(unit, trunk_mem);
        tgsize = WORDS2BYTES(tgsize);

        for (tid = 0; tid < ngroups_fp; tid++, trunk_info++) {
             SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, trunk_mem, MEM_BLOCK_ANY, tid,
                              &tg_entry));
             if (sal_memcmp(&tg_entry, soc_mem_entry_null(unit, trunk_mem),
                            tgsize) == 0) {
                 /*
                  * Invalid trunk ID
                  */
                 continue;
             }

             if (MEMBER_INFO(unit, tid).recovered_from_scache) {
                 for (i = 0, j = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
                     if (MEMBER_INFO(unit, tid).member_flags[i] &
                             BCM_TRUNK_MEMBER_EGRESS_DISABLE) {
                         continue;
                     }
                     egr_hw_mods[j] = MEMBER_INFO(unit, tid).modport[i] >> 8;
                     egr_hw_ports[j] = MEMBER_INFO(unit, tid).modport[i] & 0xff;
                     egr_member_flags[j] = MEMBER_INFO(unit, tid).member_flags[i];
                     j++;
                 }
                 num_egr_ports = j;

             } else {
                 /* In level 1 warm boot, not all egress trunk members can be
                  * recovered from TRUNK_GROUP table, as those members with
                  * the UNICAST_EGRESS_DISABLE flag set will not be in
                  * TRUNK_GROUP table.
                  */

                 num_mod = 0;
                 sal_memset(om, 0, sizeof(om));
                 sal_memset(op, 0, sizeof(op));

                 for (i = 0; i < max_ports; i++) {
                     int found = 0;

                     if (soc_feature(unit, soc_feature_fastlag)) {
                         egr_hw_mods[i] = soc_FAST_TRUNK_GROUPm_field32_get(unit,
                            &tg_entry, _xgs_fast_modulef[i]);
                         egr_hw_ports[i] = soc_FAST_TRUNK_GROUPm_field32_get(unit,
                            &tg_entry, _xgs_fast_portf[i]);
                     } else {
                          egr_hw_mods[i] = soc_TRUNK_GROUPm_field32_get(unit,
                             &tg_entry, _xgs_modulef[i]);
                          egr_hw_ports[i] = soc_TRUNK_GROUPm_field32_get(unit,
                             &tg_entry, _xgs_portf[i]);
                     }
                     for (j = 0; j < num_mod; j++) {
                         if (om[j] == egr_hw_mods[i]) {
                             SOC_PBMP_PORT_ADD(op[j], egr_hw_ports[i]);
                             found = 1;
                             break;
                         }
                     }
                     if (!found) {
                         SOC_PBMP_PORT_ADD(op[num_mod], egr_hw_ports[i]);
                         om[num_mod++] = egr_hw_mods[i];
                     }
                 }

                 num_egr_ports = 0;
                 for (i = 0; i < num_mod; i++) {
                     int pbmp_count;
                     SOC_PBMP_COUNT(op[i], pbmp_count);
                     num_egr_ports += pbmp_count;
                 }

                 for (i = 0; i < num_egr_ports; i++) {
                     /* Compare values with source trunk map to recreate
                      * ingress disable flag
                      */
#ifdef BCM_TRX_SUPPORT
                     if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                         idx = (egr_hw_mods[i] * (SOC_PORT_ADDR_MAX(unit) + 1)) +
                             egr_hw_ports[i];
                     } else
#endif
                     {
                         idx = (egr_hw_mods[i] << SOC_TRUNK_BIT_POS(unit)) +
                             egr_hw_ports[i];
                     }

                     rv = soc_mem_read(unit, SOURCE_TRUNK_MAP_TABLEm,
                             MEM_BLOCK_ANY, idx, &stm);
                     if (BCM_SUCCESS(rv)) {
                         ptype = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit,
                                 &stm, PORT_TYPEf);
                         tid_get = soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit,
                                   &stm, tid_fld);
                         if (ptype != 1 || tid_get != tid) {
                             egr_member_flags[i] |=
                                 BCM_TRUNK_MEMBER_INGRESS_DISABLE;
                         }
                     }
                 } 

                 MEMBER_INFO(unit, tid).num_ports = num_egr_ports;
                 for (i = 0; i < num_egr_ports; i++) {
                     MEMBER_INFO(unit, tid).modport[i] =
                         (egr_hw_mods[i] << 8) | egr_hw_ports[i];
                     MEMBER_INFO(unit, tid).member_flags[i] =
                         egr_member_flags[i];
                 }
             }

             if (soc_feature(unit, soc_feature_fastlag)) {
                 psc = soc_FAST_TRUNK_GROUPm_field32_get(unit, &tg_entry,
                                                               RTAGf);
             } else {
                 psc = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry,
                                                          RTAGf);
             }
             if (psc <= 0) {
                 psc = BCM_TRUNK_PSC_DEFAULT;
             }

             /*
              * Update trunk control state
              */
             TRUNK_LOCK(unit);
             t_info = trunk_info; 
             t_info->tid = tid;
             t_info->in_use = TRUE;
             t_info->psc = psc;
             _bcm_xgs3_trunk_rtag_to_psc(unit, psc, &(t_info->psc));

             /* Recover software trunk failover internal state */
             rv = _bcm_xgs3_trunk_swfailover_set(unit, 
                                                 tid, 
                                                 psc,
                                                 num_egr_ports, 
                                                 egr_hw_mods, 
                                                 egr_hw_ports,
                                                 egr_member_flags);

             /* Recover hardware trunk failover internal state */
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
             if (BCM_SUCCESS(rv) &&
                 soc_feature(unit, soc_feature_port_lag_failover)) {
                 /* Record trunk info for HW failover */
                 hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[tid]);
                 hwft->num_ports = num_egr_ports;
                 for (tix = 0; tix < num_egr_ports; tix++) {
                     hwft->modport[tix] =
                         (egr_hw_mods[tix] << 8) | egr_hw_ports[tix];
                     rv = _bcm_xgs3_trunk_hwfailover_read(unit,
                                                          egr_hw_ports[tix],
                                                          0, &rtag,
                                                          NULL, NULL, NULL);

                     if (BCM_SUCCESS(rv)) {
                         psc_local = BCM_TRUNK_PSC_DEFAULT;
                         _bcm_xgs3_trunk_rtag_to_psc(unit, rtag,
                                                     &psc_local);
                         hwft->psc[tix] = psc_local;
                         /* Flags already handled */
                     } else {
                         /* Propagate error */
                         break;
                     }
                 }
             }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

             TRUNK_UNLOCK(unit); 
        } /* for each tid */

        /* Recover source mod port trunk cache and trunk members with
         * egress disable flag set.
         */
        /*
         * For source module ID base indexing supported, 
         * _bcm_xgs3_trunk_mod_port_map_reinit is called during stacking 
         * module reinit, since the number of ports in a given module is not known until then.
         */
        if (BCM_SUCCESS(rv) && !soc_feature(unit, soc_feature_src_modid_base_index)) {
            rv = _xgs3_trunk_mod_port_map_reinit(unit);
        }

    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
}

/*
 * Handles state recovery for fabric trunks
 */
int
_xgs3_trunk_fabric_reinit(int unit, int min_tid, int max_tid,
                          trunk_private_t *trunk_info)
{
    int rv = BCM_E_NONE;
    uint32 hg_cntl_entry;
    uint32 hg_group_entry;
    pbmp_t hg_pbmp, pbmp;
    uint32 higig_bitmap;
    bcm_trunk_t tid, hgtid;
    trunk_private_t *t_info;
    bcm_port_t tp[BCM_XGS3_TRUNK_MAX_PORTCNT] = {0};
    int num_ports;
    int psc = 0;
    bcm_port_t port;
    int i=0;
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    uint64 hg_trunk_group, hg_trunk_group_hi;
    _xgs3_hw_tinfo_t  *hwft;
    int               tix, rtag;
#endif

    for (tid = min_tid; tid <= max_tid; tid++, trunk_info++) {
 
        hgtid = tid - min_tid;
        SOC_PBMP_CLEAR(hg_pbmp);
        if (SOC_IS_FB_FX_HX(unit)) { 
            SOC_IF_ERROR_RETURN
                (READ_HIGIG_TRUNK_CONTROLr(unit, &hg_cntl_entry));
            SOC_PBMP_WORD_SET(hg_pbmp, 
                              0,
                              soc_reg_field_get(unit, 
                                                HIGIG_TRUNK_CONTROLr, 
                                                hg_cntl_entry,
                                                (tid == min_tid) ?
                                                HIGIG_TRUNK_BITMAP0f :
                                                HIGIG_TRUNK_BITMAP1f));
        }
#if defined(BCM_TRIUMPH2_SUPPORT)
        else if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                 SOC_IS_VALKYRIE2(unit)) {
            uint64		val64;
            /* Triumph2 / Apollo / Valkyrie2 */
            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAP_64r(unit, hgtid, &val64));
            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0,
                     soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                           val64, HIGIG_TRUNK_BITMAP_LOf));
            SOC_PBMP_WORD_SET(hg_pbmp, 1,
                     soc_reg64_field32_get(unit, HG_TRUNK_BITMAP_64r,
                                           val64, HIGIG_TRUNK_BITMAP_HIf));
        }
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
        else if (soc_feature(unit, soc_feature_high_portcount_register)) {
            uint64 val64, hg_trunk_bmap_f;

            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAP_LOr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_LOr,
                                    val64, HIGIG_TRUNK_BITMAPf);
            SOC_PBMP_CLEAR(hg_pbmp);
            SOC_PBMP_WORD_SET(hg_pbmp, 0, COMPILER_64_LO(hg_trunk_bmap_f));
            SOC_PBMP_WORD_SET(hg_pbmp, 1, COMPILER_64_HI(hg_trunk_bmap_f));

            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAP_HIr(unit, hgtid, &val64));
            hg_trunk_bmap_f =
                soc_reg64_field_get(unit, HG_TRUNK_BITMAP_HIr,
                                    val64, HIGIG_TRUNK_BITMAPf);
            SOC_PBMP_WORD_SET(hg_pbmp, 2, COMPILER_64_LO(hg_trunk_bmap_f));
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        else if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            /* Humv / Bradley / Scorpion / Conqueror / Triumph / Valkyrie */
            SOC_IF_ERROR_RETURN
                (READ_HG_TRUNK_BITMAPr(unit, hgtid, &hg_cntl_entry));
            SOC_PBMP_WORD_SET(hg_pbmp, 
                              0,
                              soc_reg_field_get(unit, 
                                                HG_TRUNK_BITMAPr, 
                                                hg_cntl_entry,
                                                HIGIG_TRUNK_BITMAPf));
        }
#endif

        SOC_PBMP_CLEAR(pbmp);
        if (SOC_IS_RAVEN(unit) || SOC_IS_TRIUMPH2(unit) ||
            SOC_IS_APOLLO(unit) ||  SOC_IS_VALKYRIE2(unit) ||
            soc_feature(unit, soc_feature_high_portcount_register)) {
            /* Use the bitmap directly for Raven from HIGIG_TRUNK_CONTROL */
            SOC_PBMP_ASSIGN(pbmp, hg_pbmp);
        } else {
            /* For other FBX, map the read higig bitmap to the physical bitmap */
            soc_xgs3_higig_bitmap_to_bitmap(unit, hg_pbmp, &pbmp);
        }

        num_ports = 0;
        if (SOC_IS_FB_FX_HX(unit)) {
            PBMP_ITER(pbmp, port) {
                BCM_IF_ERROR_RETURN
                    (soc_xgs3_port_to_higig_bitmap(unit, port, &higig_bitmap));
                BCM_IF_ERROR_RETURN
                    (soc_xgs3_higig_bitmap_to_higig_port_num(higig_bitmap,
                                                             (uint32 *)&(tp[num_ports])));
                num_ports += 1;
            }
        } 
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        else if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            int port_limit;
            COMPILER_64_ZERO(hg_trunk_group);
            COMPILER_64_ZERO(hg_trunk_group_hi);

            COMPILER_REFERENCE(hg_trunk_group_hi);

            if (SOC_PBMP_NOT_NULL(pbmp)) {
                SOC_IF_ERROR_RETURN
                    (READ_HG_TRUNK_GROUPr(unit, hgtid, &hg_trunk_group));
                if (SOC_REG_FIELD_VALID(unit,
                    HG_TRUNK_GROUPr, HIGIG_TRUNK_SIZEf)) { /* PR 34459 */
                    i = soc_reg64_field32_get(unit, HG_TRUNK_GROUPr,
                                              hg_trunk_group,
                                              HIGIG_TRUNK_SIZEf);
                    num_ports = i + 1;
                } else {
                    BCM_PBMP_COUNT(pbmp, num_ports);
                }
                port_limit = num_ports;
#if defined(BCM_SCORPION_SUPPORT)
                if (SOC_IS_SC_CQ(unit) &&
                    (port_limit > BCM_SWITCH_TRUNK_MAX_PORTCNT)) {
                    port_limit = BCM_SWITCH_TRUNK_MAX_PORTCNT;   
                }
#endif
                for (i = 0; i < port_limit; i++) {
                    tp[i] = 
                        soc_reg64_field32_get(unit, HG_TRUNK_GROUPr,
                                              hg_trunk_group, _hg_trunk[i]);
                }
#if defined(BCM_SCORPION_SUPPORT)
                if (SOC_IS_SC_CQ(unit) &&
                    (num_ports > BCM_SWITCH_TRUNK_MAX_PORTCNT)) {
                    SOC_IF_ERROR_RETURN
                        (READ_HG_TRUNK_GROUP_HIr(unit, hgtid,
                                                 &hg_trunk_group_hi));
                    for (i = BCM_SWITCH_TRUNK_MAX_PORTCNT;
                         i < num_ports; i++) {
                        tp[i] =
                            soc_reg64_field32_get(unit, HG_TRUNK_GROUP_HIr,
                                                  hg_trunk_group_hi, 
                                                  _hg_trunk_hi[i - BCM_SWITCH_TRUNK_MAX_PORTCNT]);
                    }
                }
#endif
            }
        }
#endif

        /* Recover trunk member info internal state */
        for (i = 0; i < num_ports; i++) {
            MEMBER_INFO(unit, (hgtid + _BCM_FP_TRUNK_NUM_MAX)).modport[i] = tp[i];
        }
        MEMBER_INFO(unit, (hgtid + _BCM_FP_TRUNK_NUM_MAX)).num_ports = num_ports;

        if (num_ports == 0) {
            continue;
        }
         
        if (SOC_IS_FB_FX_HX(unit)) {
            SOC_IF_ERROR_RETURN
                (READ_HIGIG_TRUNK_GROUPr(unit, &hg_group_entry));

            psc = soc_reg_field_get(unit, 
                                            HIGIG_TRUNK_GROUPr,
                                            hg_group_entry,
                                            (tid == min_tid) ?           \
                                            HIGIG_TRUNK_RTAG0f : \
                                            HIGIG_TRUNK_RTAG1f);
        } 
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
        else {
            /* Humv / Bradley / Scorpion / Conqueror / Triumph / Valkyrie */
            psc = soc_reg64_field32_get(unit, 
                                                HG_TRUNK_GROUPr,
                                                hg_trunk_group,
                                                HIGIG_TRUNK_RTAGf);
        }
#endif
        /*
         * Update trunk control state
         */
        TRUNK_LOCK(unit);
        t_info = trunk_info;
        t_info->tid = tid;
        t_info->psc = psc;
        t_info->in_use = TRUE;
        rv = _bcm_xgs3_trunk_swfailover_fabric_set(unit, hgtid,
                                                   psc, pbmp);
        TRUNK_UNLOCK(unit);

        if (BCM_FAILURE(rv)) {
            break;
        }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        if (soc_feature(unit, soc_feature_hg_trunk_failover)) {
            /* Record trunk info for HW failover */
            hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[hgtid +
                                               _BCM_FP_TRUNK_NUM_MAX]);
            hwft->num_ports = num_ports;
            for (tix = 0; tix < num_ports; tix++) {
                hwft->modport[tix] = tp[tix];
                rv = _bcm_xgs3_trunk_hwfailover_hg_read(unit,
                                                        tp[tix],
                                                   0, &rtag, NULL, NULL, NULL);

                if (BCM_SUCCESS(rv)) {
                    TRUNK_LOCK(unit);
                    if (7 == rtag) {
                        if (soc_feature(unit, soc_feature_port_flow_hash)) {
                            hwft->psc[tix] = BCM_TRUNK_PSC_PORTFLOW;
                        } else if (soc_feature(unit,
                                       soc_feature_port_trunk_index)) {
                            hwft->psc[tix] = BCM_TRUNK_PSC_PORTINDEX;
                        } /* Else, unknown */
                    } else if (0 != rtag) {
                        /* HW and SW values match */
                        hwft->psc[tix] = rtag;
                    } else {
                        /* Use trunk group value as the default */
                        hwft->psc[tix] = psc;
                    }
                    /* Flags already handled */
                    TRUNK_UNLOCK(unit);
                }
            }
        }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    }

    return rv;
}

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
void
_bcm_xgs3_hw_failover_flags_set(int unit, bcm_trunk_t tid,
                                int port_index, int hg_trunk, uint8 flags)
{
    int tid_ix;

    tid_ix = tid + (hg_trunk ? _BCM_FP_TRUNK_NUM_MAX : 0);

    TRUNK_LOCK(unit);
    _xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix].flags[port_index] = flags;
    TRUNK_UNLOCK(unit);

    return;
}

uint8
_bcm_xgs3_hw_failover_flags_get(int unit, bcm_trunk_t tid,
                                int port_index, int hg_trunk)
{
    int tid_ix;
    uint8 flags;

    tid_ix = tid + (hg_trunk ? _BCM_FP_TRUNK_NUM_MAX : 0);

    TRUNK_LOCK(unit);
    flags = _xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix].flags[port_index];
    TRUNK_UNLOCK(unit);

    return flags;
}
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

void
_bcm_xgs3_trunk_member_info_set(int unit, bcm_trunk_t tid,
                                uint8 num_ports,
                                uint16 modport[BCM_XGS3_TRUNK_MAX_PORTCNT],
                                uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT])
{
    int i;

    MEMBER_INFO(unit, tid).num_ports = num_ports;
    for (i = 0; i < MEMBER_INFO(unit, tid).num_ports; i++) {
        MEMBER_INFO(unit, tid).modport[i] = modport[i];
        MEMBER_INFO(unit, tid).member_flags[i] = member_flags[i];
    }
    MEMBER_INFO(unit, tid).recovered_from_scache = 1;

    return;
}

void
_bcm_xgs3_trunk_member_info_get(int unit, bcm_trunk_t tid,
                                uint8 *num_ports,
                                uint16 modport[BCM_XGS3_TRUNK_MAX_PORTCNT],
                                uint32 member_flags[BCM_XGS3_TRUNK_MAX_PORTCNT])
{
    int i;

    *num_ports = MEMBER_INFO(unit, tid).num_ports;
    for (i = 0; i < BCM_XGS3_TRUNK_MAX_PORTCNT; i++) {
        modport[i] = MEMBER_INFO(unit, tid).modport[i];
        member_flags[i] = MEMBER_INFO(unit, tid).member_flags[i];
    }

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_xgs3_trunk_sw_dump
 * Purpose:
 *     Displays trunk information maintained by software.
 * Parameters:
 *     unit - Device unit number
 *      tid - Trunk id
 * Returns:
 *     None
 */
void
_bcm_xgs3_trunk_sw_dump(int unit)
{
    int rv, i, j, tid_ix, tid_min = -1, tid_max;
    bcm_trunk_chip_info_t       chip_info;
    _xgs3_trunk_swfail_t       *swf;
    _xgs3_tinfo_t *swft;

    swf = _xgs3_trunk_swfail[unit];

    rv = bcm_esw_trunk_chip_info_get(unit, &chip_info);
    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit,
                            "Unable to retrieve unit %d chip info %d\n"),
                 unit, rv));
    }

    if (chip_info.trunk_id_min < 0) {
        if (chip_info.trunk_fabric_id_min < 0) {
            /* No trunks!  Abort */
            LOG_CLI((BSL_META_U(unit,
                                "No trunk groups on unit %d\n"), unit));
            return;
        } else {
            tid_min = chip_info.trunk_fabric_id_min;
            tid_max = chip_info.trunk_fabric_id_max;
        }
    } else {
        tid_min = chip_info.trunk_id_min;
        if (chip_info.trunk_fabric_id_min < 0) {
            tid_max = chip_info.trunk_id_max;
        } else {
            tid_max = chip_info.trunk_fabric_id_max;
        }
    }

    if (NULL != swf) {
        LOG_CLI((BSL_META_U(unit,
                            "Software failover parameters:\n")));
        LOG_CLI((BSL_META_U(unit,
                            "   Local modid:  %d\n"), swf->mymodid));
        LOG_CLI((BSL_META_U(unit,
                            "   Port:  Trunk\n")));
        for (j = 0; j < SOC_MAX_NUM_PORTS; j++) {
            if (0 != swf->trunk[j]) {
                LOG_CLI((BSL_META_U(unit,
                                    "   %d:  %d\n"),
                         j, swf->trunk[j]));
            }
        }
        LOG_CLI((BSL_META_U(unit,
                            "   Trunk SW failover parameters\n")));
        for (i = tid_min; i <= tid_max; i++) {
            if (i >= chip_info.trunk_fabric_id_min) {
                tid_ix = (i - chip_info.trunk_fabric_id_min) +
                    _BCM_FP_TRUNK_NUM_MAX;
            } else {
                tid_ix = i;
            }

            swft = &swf->tinfo[tid_ix];
            if (0 != swft->num_ports) {
                LOG_CLI((BSL_META_U(unit,
                                    "   Trunk %d, index %d\n"), i, tid_ix));
                LOG_CLI((BSL_META_U(unit,
                                    "      RTAG value %d\n"), swft->rtag));
                LOG_CLI((BSL_META_U(unit,
                                    "      Port count %d\n"), swft->num_ports));
                for (j = 0; j < swft->num_ports; j++) {
                    LOG_CLI((BSL_META_U(unit,
                                        "      ModPort 0x%04x\n"), swft->modport[j]));
                }
            }
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "SW trunk failover not initialized on unit %d\n"),
                 unit));
    }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_port_lag_failover) ||
        soc_feature(unit, soc_feature_hg_trunk_failover)) {
        _xgs3_hw_tinfo_t           *hwft;

        if (NULL == _xgs3_trunk_hwfail[unit]) {
            LOG_CLI((BSL_META_U(unit,
                                "HW trunk failover not initialized on unit %d\n"),
                     unit));
            return;
        }

        LOG_CLI((BSL_META_U(unit,
                            "   Trunk HW failover parameters\n")));
        for (i = tid_min; i <= tid_max; i++) {
            if (i >= chip_info.trunk_fabric_id_min) {
                tid_ix = (i - chip_info.trunk_fabric_id_min) +
                    _BCM_FP_TRUNK_NUM_MAX;
            } else {
                tid_ix = i;
            }

            hwft = &(_xgs3_trunk_hwfail[unit]->hw_tinfo[tid_ix]);
            if (0 != hwft->num_ports) {
                LOG_CLI((BSL_META_U(unit,
                                    "   Trunk %d, index %d\n"), i, tid_ix));
                LOG_CLI((BSL_META_U(unit,
                                    "      Port count %d\n"), hwft->num_ports));
                for (j = 0; j < hwft->num_ports; j++) {
                    LOG_CLI((BSL_META_U(unit,
                                        "      PSC value %d\n"), hwft->psc[j]));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Flags 0x%08x\n"), hwft->flags[j]));
                    LOG_CLI((BSL_META_U(unit,
                                        "      ModPort 0x%04x\n"), hwft->modport[j]));
                }
            }
        }
    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */

    if (NULL != _xgs3_trunk_member_info[unit]) {
        LOG_CLI((BSL_META_U(unit,
                            "Trunk membership info:\n")));
        for (i = tid_min; i <= tid_max; i++) {
            if (i >= chip_info.trunk_fabric_id_min) {
                tid_ix = (i - chip_info.trunk_fabric_id_min) +
                    _BCM_FP_TRUNK_NUM_MAX;
            } else {
                tid_ix = i;
            }

            if (0 != MEMBER_INFO(unit, tid_ix).num_ports) {
                LOG_CLI((BSL_META_U(unit,
                                    "   Trunk %d, index %d\n"), i, tid_ix));
                LOG_CLI((BSL_META_U(unit,
                                    "      Port count %d\n"),
                         MEMBER_INFO(unit, tid_ix).num_ports));
                for (j = 0; j < MEMBER_INFO(unit, tid_ix).num_ports; j++) {
                    LOG_CLI((BSL_META_U(unit,
                                        "      ModPort 0x%04x\n"),
                             MEMBER_INFO(unit, tid_ix).modport[j]));
                    LOG_CLI((BSL_META_U(unit,
                                        "      Member flags 0x%x\n"),
                             MEMBER_INFO(unit, tid_ix).member_flags[j]));
                }
            }
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "Trunk membership info not initialized on unit %d\n"),
                 unit));
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */
