/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr_glif.c
 * Purpose:     Global lif allocation manager for DNX chips.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>

#include <bcm_int/dpp/gport_mgmt.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#define _BCM_DPP_GLOBAL_LIF_MAPPING_HASH_SIZE               (128 * 1024) /* We need a power of 2, which is equal to or greater
                                                                            than the number of entries. */


#define _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED                (-1)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_MASK    (0X1f)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT   (27)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_MASK     (0Xffffff)
#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_SHIFT    (0)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(_global_lif, _type)     \
    ((_global_lif) | ((_type) << _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT))

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_GET(_global_lif)     \
    (((_global_lif) & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_MASK) >> _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SHIFT)

#define _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(_global_lif)     \
    (((_global_lif) & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_MASK) >> _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_SHIFT)

#define GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB 0
#define GLIF_EGRESS_LIF_TO_GLOBAL_LIF_HTB  1

#define ALLOC_MNGR_GLIF_ACCESS                          sw_state_access[unit].dpp.bcm.alloc_mngr_glif

/*
 * GLOBAL LIF ALLOCATION DEFINES AND MACROS
 */
#define _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(_object_id) (_object_id / _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_GLOBAL_LIF_BANK_LOW_ID(bank_id) ((bank_id) * _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK)

#define BCM_DPP_AM_FLAG_DEALLOC_VERIFY  0x100000

/* 
 * GLOBAL LIF ALLOCATION DEFINES AND MACROS - END 
 */

typedef enum _bcm_dpp_global_lif_mapping_type_s {
    _bcm_dpp_global_lif_mapping_type_none,
    _bcm_dpp_global_lif_mapping_type_count
} _bcm_dpp_global_lif_mapping_type_t;


typedef struct {
    int     is_htb;
    int     max_entries;    /* How many entries can be in this mapping. */
    int     max_key;        /* Maximum key of the hash table */
    int     max_val;        /* Maimum value to map to */
    int     (*get_function)(int unit, int index, int *value); /* Wb variable get function */
    int     (*set_function)(int unit, int index, int value); /* Wb variable set function */
    int     htb_handle; 
    int     htb_type;
} _bcm_dpp_global_lif_mapping_data_t;

typedef int (*_bcm_dpp_global_lif_map_iterate_cb)(int unit, uint32 flags);

STATIC int
_bcm_dpp_global_lif_map_iterate(int unit, _bcm_dpp_global_lif_map_iterate_cb cb);

STATIC int 
_bcm_dpp_global_lif_map_create(int unit, uint32 flags);

STATIC int 
_bcm_dpp_global_lif_map_destroy(int unit, uint32 flags);

STATIC int 
_bcm_dpp_global_lif_map_get(int unit, uint32 flags, _bcm_dpp_global_lif_mapping_data_t *data);

STATIC int
_bcm_dpp_global_lif_mapping_find_internal(int unit, uint32 flags, int key, int *data, uint8 remove);

STATIC int 
_bcm_dpp_global_lif_mapping_insert_internal(int unit, uint32 flags, int key, int data);

/* Decode global lif flags */
STATIC int
_bcm_dpp_global_lif_mapping_decode(uint32 flags, uint8 *is_global_to_local, _bcm_dpp_global_lif_mapping_type_t *type);


/*
 * GLOBAL LIF ALLOCATION UTILITIES DECLARATIONS. 
 */

STATIC uint32
_bcm_dpp_am_global_lif_update_count(int unit, uint32 sync_flags, int diff);

STATIC uint32 
_bcm_dpp_am_global_lif_alloc_count_verify(int unit, uint32 sync_flags);

STATIC uint32 
_bcm_dpp_am_global_lif_is_legal(int unit, int global_lif, uint8 is_ingress, uint8 *is_legal);

STATIC int
bcm_dpp_global_lif_update_sync_indication(int unit, int global_lif, uint8 is_sync);


/* 
 * GLOBAL LIF ALLOCATION UTILITIES DECLARATIONS - END.
 */



/* 
 *  GLOBAL LIF MAPPING - START
 */ 


/*
 *  GLOBAL LIF WRAPPER FUNCTIONS - START
 */

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_decode
 * Purpose:
 *      Given global lif mapping flags, decodes them and returns the relevant flags.
 *  
 * Parameters:
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_*
 *     is_global_to_local-(OUT) Optional (can be null).
 *                                  0: local to global. 1: global to local
 *     type             - (OUT) Optional (can be null). The global lif type.
 * Returns: 
 *     BCM_E_NONE       - If all flags are valid 
 *     BCM_E_INTERNAL   - If no direction flag was selected for ingress/egress or global->local/local->global
 */
STATIC int
_bcm_dpp_global_lif_mapping_decode(uint32 flags, uint8 *is_global_to_local, _bcm_dpp_global_lif_mapping_type_t *type){
    uint8 is_local_to_global;
    uint8 is_ingress, is_egress;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    is_ingress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS;
    is_egress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS;
    
    /* Check that at least one of is_ingress or is_egress is set */
    if ((!is_ingress && !is_egress)
        || (is_ingress && is_egress)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global lif mapping flags must contain either ingress or egress.")));
    }

    /* Check that at least one of global_to_local or local_to_global is set.
       This argument is optional because not all functions use it. */
    if (is_global_to_local) {
        *is_global_to_local = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL;
        is_local_to_global = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL;
        if ((!*is_global_to_local && !is_local_to_global)
            || (*is_global_to_local && is_local_to_global)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Global lif mapping flags must contain either global_to_local or local_to_global.")));
        }
    }

    /* When there are types available, add them here */
    DPP_IF_NOT_NULL_FILL(type, _bcm_dpp_global_lif_mapping_type_none);

exit:
    BCMDNX_FUNC_RETURN_NO_UNIT;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_egress_global_to_local_get
 * Purpose:
 *      Egress global to local lif mapping is done in the HW, by the GLEM. The GLEM already has an efficient
 *          shadow implemented, so we'd use it instead of shadowing in this layer like we do with other mappings.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     global lif       - (IN) Egress global lif to be searched.
 *     local_lif        - (IN) Found local lif.
 * Returns: 
 *     BCM_E_NONE       - If the mapping was successful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_global_lif_mapping_egress_global_to_local_get(int unit, int egress_global_lif, int *egress_local_lif){
    int rv;
    uint8 is_found;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lif_glem_access_entry_by_key_get, 
                                (unit, egress_global_lif, egress_local_lif, NULL, &is_found));
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_found) {
        *egress_local_lif = _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_create
 * Purpose:
 *      Create mapping between a global lif, and a local in lif and/or local out lif.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS and/or _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS
 *     global lif       - (IN) Global lif to be mapped. 
 *     local_lif        - (IN) Local lif to be mapped.
 * Returns: 
 *     BCM_E_NONE       - If the mapping was successful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_create(int unit, uint32 flags, int global_lif, int local_lif){
    int rv;
    _bcm_dpp_global_lif_mapping_type_t type;
    uint32 internal_flags;
    int global_lif_with_type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get global lif mapping type. */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, &type);
    BCMDNX_IF_ERR_EXIT(rv);

    global_lif_with_type = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type);

    internal_flags = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_IN_EG_DIRECTIONS;
    
    /* Global to local */
    rv = _bcm_dpp_global_lif_mapping_insert_internal(unit, internal_flags | _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, global_lif_with_type, local_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Local to global */
    rv = _bcm_dpp_global_lif_mapping_insert_internal(unit, internal_flags | _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL, local_lif, global_lif_with_type);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_remove
 * Purpose:
 *      Remove mapping between a global lif, and a local in lif and/or local out lif.
 *      Use the _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL flag and provide the global lif argument 
 *      or use the _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL with INGRESS/EGRESS flag and provide
 *      ingress/egress local lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_*
 *     global lif       - (IN) Global lif to be unmapped (with _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL flag). 
 *     local_lif        - (IN) Local lif to be unmapped (with _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL flag).
 * Returns: 
 *     BCM_E_NONE       - If the unmapping was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_remove(int unit, uint32 flags, int global_lif, int local_lif){
    int rv;
    uint8 is_global_to_local;
    int given_lif, mapped_lif;
    _bcm_dpp_global_lif_mapping_type_t type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get type and global/local direction */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, &is_global_to_local, &type);
    BCMDNX_IF_ERR_EXIT(rv);

    given_lif = (is_global_to_local) ? _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type) : local_lif;
    
    /* Remove */
    /* Find the mapping for the given global/local lif, and remove it. */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags, given_lif, &mapped_lif, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Use the mapped lif we found above and remove the opposite mapping. */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags ^ _BCM_DPP_GLOBAL_LIF_MAPPING_GLOB_LOC_DIRECTIONS /* Will return the opposite direction */, mapped_lif, &given_lif, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_local_to_global_get
 * Purpose:
 *      Find mapping from local lif to global lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS OR _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS
 *     local_lif        - (IN) Local lif to be matched.
 *     global_lif       - (OUT) Found global lif.
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully..
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_local_to_global_get(int unit, uint32 flags, int local_lif, int *global_lif){
    int rv;
    uint8 is_mapped;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check and analyze input */
    BCMDNX_NULL_CHECK(global_lif); 

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *global_lif = local_lif;
        BCM_EXIT;
    }

    /* In case lif is invalid return */
    if (local_lif == BCM_GPORT_INVALID) {
        *global_lif = BCM_GPORT_INVALID;
        BCM_EXIT;
    }

    /* Verify flags */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If lif is a direct lif, just return it. */
    if (flags & _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS) {
        rv = _bcm_dpp_lif_id_is_mapped(unit, local_lif, &is_mapped);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_mapped) {
            *global_lif = local_lif;
            BCM_EXIT;
        }
    }


    /* Find mapping */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags | _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL, 
                                                   local_lif, global_lif, FALSE);

    /* A not found result doesn't necessarily indicate an error, and therefore shouldn't print an error. */
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

    *global_lif = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(*global_lif);
    BCM_RETURN_VAL_EXIT(rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_global_to_local_get
 * Purpose:
 *      Find mapping from global lif to local lif.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS OR _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS
 *     global_lif       - (IN) Global lif to be matched.
 *     local_lif        - (OUT) Found local lif.
 * Returns: 
 *     BCM_E_NONE       - If the value was retrieved successfully.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_global_to_local_get(int unit, int flags, int global_lif, int *local_lif){
    int rv;
    _bcm_dpp_global_lif_mapping_type_t type;
    uint8 is_mapped;
    int global_lif_with_type;
    BCMDNX_INIT_FUNC_DEFS;

    /* Check and analyze input. */

    BCMDNX_NULL_CHECK(local_lif); 

    /* Global and local lif are indentical in arad. */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *local_lif = global_lif;
        BCM_EXIT;
    }

    /* In case lif is invalid return */
    if (global_lif == BCM_GPORT_INVALID) {
        *local_lif = BCM_GPORT_INVALID;
        BCM_EXIT;
    }

    /* Verify input */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, NULL, &type);
    BCMDNX_IF_ERR_EXIT(rv);


    /* If lif is not a global lif, just return it. */
    if (flags & _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS) {
        rv = _bcm_dpp_lif_id_is_mapped(unit, global_lif, &is_mapped);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_mapped) {
            *local_lif = global_lif;
            BCM_EXIT;
        }
    }
    
    global_lif_with_type = _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_TYPE_SET(global_lif, type);

    /* Get local lif */
    rv = _bcm_dpp_global_lif_mapping_find_internal(unit, flags | _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, 
                                                   global_lif_with_type, local_lif, FALSE);
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_init
 * Purpose:
 *      Initiates all global-local lif mappings on the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_init(int unit){
    int rv;
    uint8 is_allocated;
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        /* Allocate the inlif local to global array. */
        rv = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_allocated) {
            rv = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_local_lifs));
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Iterate over all maps and create them.*/
        rv = _bcm_dpp_global_lif_map_iterate(unit, _bcm_dpp_global_lif_map_create);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_deinit
 * Purpose:
 *      Destroys all global-local lif mappings on the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
int
_bcm_dpp_global_lif_mapping_deinit(int unit){
    int rv;
    BCMDNX_INIT_FUNC_DEFS

    /* Iterate over all maps and destroy them.*/
    rv = _bcm_dpp_global_lif_map_iterate(unit, _bcm_dpp_global_lif_map_destroy);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_iterate
 * Purpose:
 *      Iterates over all ingress/egress and global_to_local/local_to_global mappings
 *      and applies a callback to each of them.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     cb               - (IN) Callback to be applied to the mapping.
 * Returns: 
 *     BCM_E_NONE       - If the iteration and all the callbacks were succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_global_lif_map_iterate(int unit, _bcm_dpp_global_lif_map_iterate_cb cb){
    int rv;
    uint32  in_eg_flags[2] = {_BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS};
    uint32  mapping_flags[2] = {_BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_TO_LOCAL, _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL};
    uint32 i, j;
    BCMDNX_INIT_FUNC_DEFS

    /* Iterate over the flags and apply the callback. */
    for (i = 0 ; i < 2 ; i++) {
        for (j = 0 ; j < 2 ; j++) {
            rv = cb(unit, in_eg_flags[i] | mapping_flags[j]);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_get
 * Purpose:
 *      Given an INGRESS/EGRESS direction flag and a local_to_global/global_to_local
 *      direction flag, returns the relevant map data: Array or hash, maximum entries, maximum value, etc.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     data             - (OUT) Metadata for the table / array.
 * Returns: 
 *     BCM_E_NONE       - If the data was retrieved successfully.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_get(int unit, uint32 flags, _bcm_dpp_global_lif_mapping_data_t *data){
    int rv;
    uint8 is_ingress, is_global_to_local;
    BCMDNX_INIT_FUNC_DEFS;

    /* Decode the direction flags */
    rv = _bcm_dpp_global_lif_mapping_decode(flags, &is_global_to_local, NULL);
    BCMDNX_IF_ERR_EXIT(rv);

    is_ingress = flags & _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS;

    sal_memset(data, 0, sizeof(_bcm_dpp_global_lif_mapping_data_t));


    if (is_ingress) {
        if (is_global_to_local) { /* Ingress global to local - use hash table. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_key = _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(unit);
            data->max_val = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->htb_type =  GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB;
            data->is_htb = 1;
            rv = ALLOC_MNGR_GLIF_ACCESS.ingress_global_lif_to_lif_htb_handle.get(unit, &data->htb_handle);
            BCMDNX_IF_ERR_EXIT(rv);
        } else { /* Ingress local to global - use sw state array. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_key = SOC_DPP_DEFS_GET(unit, nof_local_lifs);
            data->max_val = _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(unit);
            data->get_function = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.get;
            data->set_function = ALLOC_MNGR_GLIF_ACCESS.ingress_lif_to_global_lif.set;
            data->is_htb = 0;
        }
    } else { /* Egress */
        if (is_global_to_local) { /* Egress global to local - use hash table. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_glem_lines);
            data->max_key = _BCM_DPP_AM_NOF_GLOBAL_OUT_LIFS(unit);
            data->max_val  = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs));;
            data->get_function = _bcm_dpp_global_lif_mapping_egress_global_to_local_get;
            data->set_function = NULL;
            data->is_htb = 0;
        } else { /* Egress local to global - use WB array. */
            data->max_entries = SOC_DPP_DEFS_GET(unit, nof_out_lifs);
            data->max_key  = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_DPP_DEFS_GET(unit, nof_out_lifs));
            data->max_val = _BCM_DPP_AM_NOF_GLOBAL_OUT_LIFS(unit);
            data->htb_type = GLIF_EGRESS_LIF_TO_GLOBAL_LIF_HTB;
            data->is_htb = 1;
            rv = ALLOC_MNGR_GLIF_ACCESS.egress_lif_to_global_lif_htb_handle.get(unit, &data->htb_handle);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_map_create
 * Purpose:
 *      Initiate a single hash table/ array according to the direction flags.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* to indicate the mapping direction.
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_create(int unit, uint32 flags){
    int rv;
    int htb_idx;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;
    int (*htb_idx_set_cb) (int unit, int htb_index);
    char *name;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping data. */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If map is a hash table, initiate it. */
    if (mapping_data.is_htb) {
        switch (mapping_data.htb_type) {
        case GLIF_INGRESS_GLOBAL_LIF_TO_LIF_HTB:
            htb_idx_set_cb = ALLOC_MNGR_GLIF_ACCESS.ingress_global_lif_to_lif_htb_handle.set;
            name = "_bcm_dpp_global_lif_egress_global_lif_to_local";
            break;
        case GLIF_EGRESS_LIF_TO_GLOBAL_LIF_HTB:
            htb_idx_set_cb = ALLOC_MNGR_GLIF_ACCESS.egress_lif_to_global_lif_htb_handle.set;
            name = "_bcm_dpp_global_lif_egress_local_lif_to_global";
            break;
        default:
            BCMDNX_IF_ERR_EXIT(BCM_E_INTERNAL);
        }

        rv = sw_state_htb_create(unit, &htb_idx, _BCM_DPP_GLOBAL_LIF_MAPPING_HASH_SIZE, sizeof(int), sizeof(int),
               name);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = htb_idx_set_cb(unit, htb_idx);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (!SOC_WARM_BOOT(unit)) {
        /*If map is an array, and we're not in warmboot, then set all entries to unmapped. */
        int i;
        int illegal = _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED;

        if (mapping_data.set_function) {
            for (i = 0 ; i < mapping_data.max_entries ; i++) {
                rv = mapping_data.set_function(unit, i, illegal);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_map_destroy
 * Purpose:
 *      Destroys a single hash table/ array according to the direction flags.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* to indicate the mapping direction.
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_map_destroy(int unit, uint32 flags){
    int rv;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping. */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mapping_data.is_htb) {
        rv = sw_state_htb_destroy(unit, mapping_data.htb_handle, NULL);
        BCMDNX_IF_ERR_EXIT(rv);
    } 
    /* If not hash table (array) then nothing to do here */

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_find_internal
 * Purpose:
 *      Find mapped lif by direction flags.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     key              - (IN) Lif to be looked up.
 *     data             - (OUT) Found lif.
 *     remove           - (IN) Delete found key.
 * Returns: 
 *     BCM_E_NONE       - If the value was found or removed successfully.
 *     BCM_E_*          - Otherwise.
 */
STATIC int
_bcm_dpp_global_lif_mapping_find_internal(int unit, uint32 flags, int key, int *data, uint8 remove){
    int rv;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping info */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check if the key is legal */
    if (_BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(key) > mapping_data.max_key || key < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal: %d"), key));
    }

    /* Find (and delete, if required) the entry. */ 
    if (mapping_data.is_htb) {

        /* If the map is a hash table, find the entry, remove if required. */
        rv = sw_state_htb_find(unit, mapping_data.htb_handle, (sw_state_htb_key_t) &key, (sw_state_htb_data_t) (uint8*) data, remove);
        if (rv != BCM_E_NONE) {
            BCM_RETURN_VAL_EXIT(rv);
        }

    } else {
        /* If the map is an array, find it in the WB array. */
        rv = mapping_data.get_function(unit, key, data);
        BCMDNX_IF_ERR_EXIT(rv);

        /* If entry is not initialized, return not found */
        if (*data == _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED) {
            BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
        }

        /* If remove is set, set the entry to unmapped */
        if (remove && mapping_data.set_function) {
            int illegal = _BCM_DPP_GLOBAL_LIF_MAPPING_UNMAPPED;
            rv = mapping_data.set_function(unit, key, illegal);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_global_lif_mapping_insert_internal
 * Purpose:
 *      Insert mapped lif by direction flags.
 *  
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) _BCM_DPP_GLOBAL_LIF_MAPPING_* flags.
 *     key              - (IN) Lif to be mapped from.
 *     data             - (IN) Lif to be mapped to.
 *  
 * Returns: 
 *     BCM_E_NONE       - If the mapping was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC int 
_bcm_dpp_global_lif_mapping_insert_internal(int unit, uint32 flags, int key, int data){
    int rv;
    _bcm_dpp_global_lif_mapping_data_t mapping_data;

    BCMDNX_INIT_FUNC_DEFS;

    /* Get mapping info */
    rv = _bcm_dpp_global_lif_map_get(unit, flags, &mapping_data);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Verify that both key and data are in legal range */
    if (key < 0 || data < 0 
        || _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(key) > mapping_data.max_key 
        || _BCM_DPP_GLOBAL_LIF_MAPPING_GLOBAL_LIF_LIF_GET(data) > mapping_data.max_val) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given lif id is illegal")));
    }

    /* Map hash table or insert to WB array. */
    if (mapping_data.is_htb) {
        rv = sw_state_htb_insert(unit, mapping_data.htb_handle, (sw_state_htb_key_t) &key, (sw_state_htb_data_t) &data);
        BCMDNX_IF_ERR_EXIT(rv);
    } else if (mapping_data.set_function) {
        rv = mapping_data.set_function(unit, key, data);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        /* No SW shadow for the mapping on this layer. Do nothing. */
        BCM_EXIT;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  MAPPING MODULE UTILITIES - END
 */ 

/*
 * GLOBAL LIF MAPPING - END
 */



/* Global LIF ALLOCATION - Start */


/*
 * GLOBAL LIF INTERNAL FUNCTIONS - START
 */


/*
 * Function:
 *     _bcm_dpp_am_global_lif_init
 * Purpose:
 *     Init the global lif variables - the bank types and lif counters.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32 
_bcm_dpp_am_global_lif_init(int unit){
    uint32 rv;
    int count;
    int illegal_egress_lif;
    uint8  system_headers_mode;
    uint8 is_allocated;
    BCMDNX_INIT_FUNC_DEFS;

    /* Initialize global lif databases. If warmboot is done, then it will be restored automatically. */
    if (!SOC_WARM_BOOT(unit)) {
        /* Initialize global lif database. */
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr_glif.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* Reset global lif counters. */
        count = 0;
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_egress_count.set(unit, count);
        BCMDNX_IF_ERR_EXIT(rv);
        
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_ingress_count.set(unit, count);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         *  Allocate sync indication bitmap.
         *  Since the number of global inlifs is identical to the number of global lifs, we will use it.
         */ 
        count = _BCM_DPP_AM_NOF_GLOBAL_IN_LIFS(unit);
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_sync_indication.alloc_bitmap(unit, count);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Allocate the illegal egress global lif. */
        if (SOC_IS_JERICHO_A0(unit) || SOC_IS_QMX_A0(unit)) {
            rv = sw_state_access[unit].dpp.soc.arad.pp.header_data.system_headers_mode.get(unit, &system_headers_mode);
            BCMDNX_IF_ERR_EXIT(rv);

            if (system_headers_mode != ARAD_PP_SYSTEM_HEADERS_MODE_ARAD) {
                /* No need to allocate illegal global lif in arad system headers mode, because the illegal global lif can't be allocated anyway.*/
                illegal_egress_lif = SOC_PPC_INVALID_GLOBAL_LIF;
                rv = _bcm_dpp_am_global_lif_non_sync_internal_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, FALSE, &illegal_egress_lif);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_alloc_verify
 * Purpose:
 *     Verify global lif before allocation / dealloction:
 *     1. If allocating - make sure there are enough free IDs.
 *     2. If deallocating / with id - make sure the id is legal.
 *     3. If deallocating - make sure sync lifs are deallocated on both sides, and non sync on one side.
 * Parameters:
 *     unit       - Device number
 *     alloc_flags- BCM_DPP_AM_FLAG_ALLOC_WITH_ID/BCM_DPP_AM_FLAG_DEALLOC_VERIFY
 *     sync_flags - _BCM_DPP_AM_SYNC_LIF_EGRESS/INGRESS
 *     global_lif - Global lif id to be checked.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
int 
_bcm_dpp_am_global_lif_alloc_verify(int unit, int alloc_flags, int sync_flags, int global_lif){
    int rv = BCM_E_NONE;
    uint8 with_id, is_legal, is_alloc;
    uint8 is_ingress, is_egress, is_sync;
    BCMDNX_INIT_FUNC_DEFS;

    is_alloc = !DPP_IS_FLAG_SET(alloc_flags, BCM_DPP_AM_FLAG_DEALLOC_VERIFY);
    with_id = (!is_alloc) || DPP_IS_FLAG_SET(alloc_flags, BCM_DPP_AM_FLAG_ALLOC_WITH_ID);
    is_ingress = DPP_IS_FLAG_SET(sync_flags, _BCM_DPP_AM_SYNC_LIF_INGRESS);
    is_egress = DPP_IS_FLAG_SET(sync_flags, _BCM_DPP_AM_SYNC_LIF_EGRESS);

    if (is_alloc) {
        /* Verify that there are enough free egress global lifs. */
        rv = _bcm_dpp_am_global_lif_alloc_count_verify(unit, sync_flags);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Verify that given id is legal. */
    if (with_id) {
        /* Check egress side. */
        if (is_egress) {
            /* Verify that given id is legal. */
            rv = _bcm_dpp_am_global_lif_is_legal(unit, global_lif, FALSE, &is_legal);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_legal) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given egress global lif id is illegal, or in a direct bank.")));
            }
        }

        /* Check ingress side. */
        if (is_ingress) {
            /* Verify that given id is legal. */
            rv = _bcm_dpp_am_global_lif_is_legal(unit, global_lif, TRUE, &is_legal);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_legal) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given ingress global lif id is illegal.")));
            }
        }
    }

    /* If deallocate, make sure that the sync indication is correct. */
    if (!is_alloc) {
        rv = _bcm_dpp_global_lif_id_is_sync(unit, global_lif, &is_sync);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_ingress && is_egress && !is_sync) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't deallocate both sides of an entry that wasn't allocated as a sync entry.")));
        } else if (!(is_ingress && is_egress) && is_sync) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can't deallocate one side of an entry that was allocated as a sync entry.")));
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_sync_lif_internal_alloc
 * Purpose:
 *     Allocate a sync lif:
 *     0. Verify that the ingress and egress lifs can be allocated (enough free lifs, and legal id).
 *     1. Allocate the lif in the sync lif pool.
 *     2. Allocate the same lif WITH_ID in the ingress and egress pools to block non sync allocation in this id.
 *     3. Set sync indication for this entry in the internal database.
 *     4. Update lif count for ingress and egress lifs.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_sync_lif_internal_alloc(int unit, uint32 flags, int *global_lif){
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* 0. Verify that the ingress and egress lifs are legal. */
    rv = _bcm_dpp_am_global_lif_alloc_verify(unit, flags, _BCM_DPP_AM_SYNC_LIF_INGRESS | _BCM_DPP_AM_SYNC_LIF_EGRESS, *global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  1. allocate in the sync lif pool.
     */
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_sync_lif, flags, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * 2. Allocate the same lif WITH_ID in the ingress and egress pools to block non sync allocation in this id.
     */
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_outlif, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_inlif, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * 3. Set sync indication for this entry in the internal database.
     */
    rv = bcm_dpp_global_lif_update_sync_indication(unit, *global_lif, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * 4. Update lif count for ingress and egress lifs.
     */
    rv = _bcm_dpp_am_global_lif_update_count(unit, _BCM_DPP_AM_SYNC_LIF_EGRESS | _BCM_DPP_AM_SYNC_LIF_INGRESS, 1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_sync_lif_internal_dealloc
 * Purpose:
 *     Deallocate a sync lif:
 *     0. Verify that lif id is legal on both ingress and egress, and that entry was allocated as a sync entry.
 *     1. Deallocate in the sync lif pool.
 *     2. Deallocate in the ingress and egress pools.
 *     3. Clear sync indication.
 *     4. Update lif count on both sides.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_sync_lif_internal_dealloc(int unit, int global_lif){
    uint32 rv;
    BCMDNX_INIT_FUNC_DEFS;

    /* 0. Verify that lif id is legal on both ingress and egress, and that entry was allocated as a sync entry. */
    rv = _bcm_dpp_am_global_lif_alloc_verify(unit, BCM_DPP_AM_FLAG_DEALLOC_VERIFY, 
                                             _BCM_DPP_AM_SYNC_LIF_INGRESS | _BCM_DPP_AM_SYNC_LIF_EGRESS, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 1. Deallocate in the sync lif pool. */
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_sync_lif, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 2. Deallocate in the ingress and egress pools. */
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_outlif, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_inlif, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 3. Clear sync indication. */
    rv = bcm_dpp_global_lif_update_sync_indication(unit, global_lif, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 4. Update lif count on both sides. */
    rv = _bcm_dpp_am_global_lif_update_count(unit, _BCM_DPP_AM_SYNC_LIF_EGRESS | _BCM_DPP_AM_SYNC_LIF_INGRESS, -1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_non_sync_internal_alloc
 * Purpose:
 *     Allocate a non sync lif:
 *     0. Verify that the lif can be allocated (enough free lifs, and legal id).
 *     1. Allocate the lif in the non sync lif pool.
 *     2. Check if this lif id is legal on the other side as well.
 *     2a. If it is, allocate in sync pool to prevent sync lifs from allocating in this index.
 *     2b. If it's not, do nothing. Sync lifs shouldn't exist in this range.
 *     3. Update lif count for the lif.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32 
_bcm_dpp_am_global_lif_non_sync_internal_alloc(int unit, uint32 alloc_flags, uint8 is_ingress, int *global_lif){
    uint32 rv;
    uint8 is_legal;
    int pool_idx;
    uint32 sync_flags;
    BCMDNX_INIT_FUNC_DEFS;

    /* 0. Verify that the lif can be allocated (enough free lifs, and legal id). */
    sync_flags = (is_ingress) ? _BCM_DPP_AM_SYNC_LIF_INGRESS : _BCM_DPP_AM_SYNC_LIF_EGRESS;
    rv = _bcm_dpp_am_global_lif_alloc_verify(unit, alloc_flags, sync_flags, *global_lif);
    BCMDNX_IF_ERR_EXIT(rv);


    /* 1. Allocate the lif in the non sync lif pool. */
    pool_idx = (is_ingress) ? dpp_am_res_global_inlif : dpp_am_res_global_outlif;
    rv = dpp_am_res_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, pool_idx, alloc_flags, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     * 2. Check if this lif id is legal on the other side as well.
     */
    rv = _bcm_dpp_am_global_lif_is_legal(unit, *global_lif, !is_ingress, &is_legal);
    BCMDNX_IF_ERR_EXIT(rv);

    /*
     * 2a. If it is, allocate in sync pool to prevent sync lifs from allocating in this index.
     */
    if (is_legal) {
        /* Use silent alloc, because this id may have been used by the other side, so we don't care about failure indication. */
        rv = dpp_am_res_silent_alloc(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_sync_lif, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, 1, global_lif);
        if (rv != BCM_E_RESOURCE) {
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    /* 
     *  3. Update lif count for the lif.
     */
    rv = _bcm_dpp_am_global_lif_update_count(unit, sync_flags, 1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_non_sync_internal_dealloc
 * Purpose:
 *     Deallocate a non sync lif:
 *     0. Verify that lif id is legal on this side, and that entry was allocated as a non sync entry.
 *     1. Deallocate in the ingress or egress pool.
 *     2. Check if this lif id is legal on the other side as well.
 *     2a. If it is, check if this lif id is allocated on the other side.
 *     2a1. If it's not , deallocate the sync lif for this id, so it can be used in the future for sync lifs.
 *     2a2. If it is, do nothing. The other side is blocking sync lifs from being allocated in this entry.
 *     2b. If it's not legal on the other side, do nothing. Sync lifs shouldn't exist in this range.
 *     3. Update lif count on both sides.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32
_bcm_dpp_am_global_lif_non_sync_internal_dealloc(int unit, int is_ingress, int global_lif){
    int rv, tmp_rv;
    uint8 is_legal;
    int pool_idx;
    uint32 sync_flags;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     *  0. Verify that lif id is legal on this side, and that entry was allocated as a non sync entry.
     */
    sync_flags = (is_ingress) ? _BCM_DPP_AM_SYNC_LIF_INGRESS : _BCM_DPP_AM_SYNC_LIF_EGRESS;
    rv = _bcm_dpp_am_global_lif_alloc_verify(unit, BCM_DPP_AM_FLAG_DEALLOC_VERIFY, sync_flags, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);


    /* 
     *  1. Deallocate in the ingress or egress pool.
     */

    pool_idx = (is_ingress) ? dpp_am_res_global_inlif : dpp_am_res_global_outlif;
    rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, pool_idx, 1, global_lif);
    BCMDNX_IF_ERR_EXIT(rv);

    /* 
     *  2. Check if this lif id is legal on the other side as well.
     */
    rv = _bcm_dpp_am_global_lif_is_legal(unit, global_lif, !is_ingress, &is_legal);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_legal) {
        /* 2a. If it is legal on the other side, check if this lif id is allocated on the other side. */
        if (is_ingress) {
            tmp_rv = _bcm_dpp_am_global_egress_lif_internal_is_allocated(unit, global_lif);
        } else {
            tmp_rv = _bcm_dpp_am_global_ingress_lif_internal_is_allocated(unit, global_lif);
        }

        if (tmp_rv == BCM_E_NOT_FOUND) {
            /* 
             *  2a1. If it's not allocated on the other side,
             *       deallocate the sync lif for this id, so it can be used in the future for sync lifs.
             */ 
            rv = dpp_am_res_free(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_sync_lif, 1, global_lif);
            BCMDNX_IF_ERR_EXIT(rv);
        } else if (tmp_rv != BCM_E_EXISTS) {
            /* Unexepected error. */
            BCMDNX_IF_ERR_EXIT(tmp_rv);
        }
    }

    /* 3. Update lif count for the lif. */
    rv = _bcm_dpp_am_global_lif_update_count(unit, sync_flags, -1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_egress_lif_internal_is_alloc
 * Purpose:
 *     See description of bcm_dpp_am_global_egress_lif_is_alloc
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_egress_lif_internal_is_allocated(int unit, int global_lif){
    uint32 rv;
    uint8 is_mapped = TRUE;

    BCMDNX_INIT_FUNC_DEFS;

    /* 1. Check if it's a global lif, or local (direct) lif. */
    rv = _bcm_dpp_lif_id_is_mapped(unit, global_lif, &is_mapped);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_mapped) {
        /* 1a. If it's a direct lif, it won't be allocated in the global lif banks. Check the local outlif bank. */
        rv = _bcm_dpp_am_local_outlif_is_alloc(unit, global_lif);
        BCM_RETURN_VAL_EXIT(rv);
    } else {
        /* 1b. If it's a mapped lif, check the global inlif resource. */
        rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_outlif, 1, global_lif);
        BCM_RETURN_VAL_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_ingress_lif_internal_is_alloc
 * Purpose:
 *     Checks wether a global inlif is allocated or not.
 * Parameters:
 *     unit       - Device number
 * Returns:
 *     BCM_E_XXX
 */
uint32             
_bcm_dpp_am_global_ingress_lif_internal_is_allocated(int unit, int global_lif){
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check the global inlif resource. */
    rv = dpp_am_res_check(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_inlif, 1, global_lif);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * GLOBAL LIF INTERNAL FUNCTIONS 
 */


/*
 * Function:
 *     _bcm_dpp_global_lif_id_is_sync
 * Purpose:
 *     Given a global lif id, returns whether it's a sync global lif id, or if the ingress and egress global lifs are unrelated.
 * Parameters:
 *     unit       - Device number
 *     global_lif - Global lif id to be checked.
 *     is_sync    - Will be filled with TRUE if lif is sync and FALSE is lif is not sync.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
int 
_bcm_dpp_global_lif_id_is_sync(int unit, int global_lif, uint8 *is_sync){
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_dpp_lif_id_is_sync is unavailable for ARADPLUS and below")));
    }

    rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_sync_indication.bit_get(unit, global_lif, is_sync);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_dpp_global_lif_update_sync_indication
 * Purpose:
 *     Updates a global lif's sync indication in the internal database.
 * Parameters:
 *     unit       - Device number
 *     global_lif - Global lif id to be changed.
 *     is_sync    - TRUE for sync, FALSE for non sync / not allocated.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
STATIC int
bcm_dpp_global_lif_update_sync_indication(int unit, int global_lif, uint8 is_sync){
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    if (is_sync) {
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_sync_indication.bit_set(unit, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_sync_indication.bit_clear(unit, global_lif);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     _bcm_dpp_am_global_lif_is_legal
 * Purpose:
 *      Check whether the given ingress/egress lif can currently be allocated.
 *      This includes permanent constraints and dynamic constraints.
 *  
 *      See the allocation algorithm above for full details.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Bank id to be checked.
 *     is_ingress       - (IN) Whether to check ingress bank (FALSE means egress bank).
 *     is_legal         - (OUT)Whether the bank can be currently allocated.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32 
_bcm_dpp_am_global_lif_is_legal(int unit, int global_lif, uint8 is_ingress, uint8 *is_legal){
    uint32 rv;
    uint8 is_mapped_lif;
    uint8 is_legal_id;
    BCMDNX_INIT_FUNC_DEFS;

    if (is_ingress) {
        /* Lif is legal if it's in the legal range. */
        *is_legal = _BCM_DPP_AM_GLOBAL_IN_LIF_ID_IS_LEGAL(unit, global_lif);
    } else {
        /* First, the lif id must be legal. */
        is_legal_id = _BCM_DPP_AM_GLOBAL_OUT_LIF_ID_IS_LEGAL(unit, global_lif);

        if (!is_legal_id) {
            *is_legal = FALSE;
            BCM_EXIT;
        }

        /* Second, egress global lifs must be mapped (not direct). */
        rv = _bcm_dpp_lif_id_is_mapped(unit, global_lif, &is_mapped_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_mapped_lif) {
            *is_legal = FALSE;
            BCM_EXIT;
        }

        /* If we got here, it means the lif is legal.*/
        *is_legal = TRUE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_egress_bank_is_direct_bank
 * Purpose:
 *      Checks whether the given egress bank is direct mapped. Egress global lif can't be allocated in these banks.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Egress bank id to be checked.
 *     is_direct_bank   - (OUT)Will be TRUE if direct mapped, false otherwise.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_egress_bank_is_direct_bank(int unit, int bank_id, uint8 *is_direct_bank) {
    int rv;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    int eedb_bank_id, rif_max_id, nof_rif_half_banks;
    BCMDNX_INIT_FUNC_DEFS;

    eedb_bank_id = bank_id;

    /* Rifs are always direct. No need for further checks. */
    rif_max_id = SOC_DPP_CONFIG(unit)->l3.nof_rifs;
    nof_rif_half_banks = ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id);

    if (bank_id < nof_rif_half_banks) {
        *is_direct_bank = TRUE;
        BCM_EXIT;
    }

    /* 
     * In QAX, rifs are not part of the eedb. Therefore, the eedb bank is the outlif bank - max rif bank.
     */
    if (SOC_IS_JERICHO_PLUS(unit)) {
        eedb_bank_id -= nof_rif_half_banks;
    }

    if (bank_id < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS(unit)) {
        /* If the bank is in the range of real EEDB lines, then its mode should be checked. */
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr_local_lif.eedb_info.banks.get(unit, bank_id, &eedb_bank_info);
        BCMDNX_IF_ERR_EXIT(rv);

        *is_direct_bank = (eedb_bank_info.type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT);
    } else {
        /* If the bank is above the range, then it can't be direct lif, because it's not a legal EEDB index. */
        *is_direct_bank = FALSE;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_lif_id_is_mapped
 * Purpose:
 *     Given a global lif id and a direction, returns whether it's a mapped lif, or a direct lif,
 *      according to HW setting.
 *      Not that in ARAD, lifs are always direct.
 * Parameters:
 *     unit       - Device number
 *     is_ingress - TRUE if ingress lif id, FALSE if egress.
 *     global_lif - Global lif id to be checked.
 *     is_mapped  - Will be filled with TRUE if lif is mapped and FALSE is lif is direct.
 * Returns:
 *     BCM_E_*      - If there was an error.
 *     BCM_E_NONE   - Otherwise.
 */
int
_bcm_dpp_lif_id_is_mapped(int unit, int global_lif, uint8 *is_mapped){
    int rv = BCM_E_NONE;
    uint8 is_direct_bank=0; 
    int bank_id;
    BCMDNX_INIT_FUNC_DEFS;

    /* 
     *  Aradplus and below don't use global lifs.
     */
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        *is_mapped = FALSE;
        BCM_EXIT;
    }

    bank_id = _BCM_DPP_AM_GLOBAL_LIF_ID_TO_BANK_ID(global_lif);

    rv = _bcm_dpp_am_egress_bank_is_direct_bank(unit, bank_id, &is_direct_bank);
    BCMDNX_IF_ERR_EXIT(rv);
    *is_mapped = !is_direct_bank;
    
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_lif_set_direct_bank
 * Purpose:
 *      Use this function to mark banks as "direct" or "mapped" and block / allow global lifs to be allcated in them.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     bank_id          - (IN) Egress bank id to be checked.
 *     is_direct_bank   - (OUT)Will be TRUE if direct mapped, false otherwise.
 * Returns: 
 *     BCM_E_NONE       - If the check was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_global_lif_set_direct_bank(int unit, int eedb_bank_id, uint8 is_direct_bank){
    int rv;
    int rif_max_id, nof_rif_half_banks;
    int global_lif_bank_id;
    BCMDNX_INIT_FUNC_DEFS;

    global_lif_bank_id = eedb_bank_id;
    /* 
     * In QAX, rifs are not part of the eedb. Therefore, the global lif bank is the eedb bank + max rif bank.
     */
    if (SOC_IS_JERICHO_PLUS(unit)) {
        rif_max_id = SOC_DPP_CONFIG(unit)->l3.nof_rifs;
        nof_rif_half_banks = ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id);
        eedb_bank_id += nof_rif_half_banks;
    }



    rv = dpp_am_res_tag_set(unit, BCM_DPP_AM_DEFAULT_POOL_IDX, dpp_am_res_global_outlif, 0, 
                            _BCM_DPP_AM_GLOBAL_LIF_BANK_LOW_ID(global_lif_bank_id), _BCM_DPP_AM_GLOBAL_LIF_NOF_ENTRIES_PER_BANK, &is_direct_bank);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * GLOBAL LIF COUNTER FUNCTIONS - START
 */
    
/*
 * Function:
 *     _bcm_dpp_am_global_lif_alloc_count_verify
 * Purpose:
 *      Checks if the maximum number of global lifs (ingress or egress) was not exceeded in the device.
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) _BCM_DPP_AM_SYNC_LIF_EGRESS/INGRESS
 * Returns: 
 *     BCM_E_NONE       - If the allocation was succesful.
 *     BCM_E_RESOURCE   - If no more global lifs can be allocated. 
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32 
_bcm_dpp_am_global_lif_alloc_count_verify(int unit, uint32 sync_flags){
    bcm_error_t rv;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_egress_count.get(unit, &count);
        BCMDNX_IF_ERR_EXIT(rv);

        if (count >= SOC_DPP_DEFS_GET(unit, nof_glem_lines)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No more egress global lifs available.")));
        }
    }

    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
        rv = ALLOC_MNGR_GLIF_ACCESS.global_lif_ingress_count.get(unit, &count);
        BCMDNX_IF_ERR_EXIT(rv);

        if (count >= SOC_DPP_DEFS_GET(unit, nof_local_lifs)){
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("No more ingress global lifs available.")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *     _bcm_dpp_am_global_lif_update_count
 * Purpose:
 *      Updates the global ingress/egress lif counter. 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     sync_flags       - (IN) _BCM_DPP_AM_SYNC_LIF_EGRESS/INGRESS
 *     diff             - (IN) The number of lifs allocated / deallocated. 
 * Returns: 
 *     BCM_E_NONE       - If the count was updated successfully.
 *     BCM_E_INTERNAL   - If the new count is less than 0.
 *     BCM_E_*          - Otherwise.
 */

STATIC uint32
_bcm_dpp_am_global_lif_update_count_internal(int unit, int (*get_func)(int, int*), int (*set_func)(int, int), int diff) {
    uint32 rv;
    int count;
    BCMDNX_INIT_FUNC_DEFS;

    /* Get old count. */
    rv = get_func(unit, &count);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Calculate new count. */
    count += diff;

    /* If new count is less than 0, return error. */
    if (count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("New global lif count is less than 0.")));
    }

    rv = set_func(unit, count);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_global_lif_update_count(int unit, uint32 sync_flags, int diff){
    bcm_error_t rv;
    BCMDNX_INIT_FUNC_DEFS;

    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
        rv = _bcm_dpp_am_global_lif_update_count_internal(unit,
                                                 ALLOC_MNGR_GLIF_ACCESS.global_lif_egress_count.get,
                                                 ALLOC_MNGR_GLIF_ACCESS.global_lif_egress_count.set,
                                                 diff);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
        rv = _bcm_dpp_am_global_lif_update_count_internal(unit,
                                                 ALLOC_MNGR_GLIF_ACCESS.global_lif_ingress_count.get,
                                                 ALLOC_MNGR_GLIF_ACCESS.global_lif_ingress_count.set,
                                                 diff);
        BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * GLOBAL LIF COUNTER FUNCTIONS - END 
 * GLOBAL LIF ALLOCATION - END 
 */


