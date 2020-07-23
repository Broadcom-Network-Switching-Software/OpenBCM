/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L2 - Broadcom StrataSwitch Layer-2 switch API.
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/scache.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/stack.h>
#if defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>	  
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/virtual.h>
#include <soc/triumph.h>
#endif /* BCM_TRX_SUPPORT */
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw/trident.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
#include <soc/hurricane3.h>
#endif /* BCM_HURRICANE3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
#include <soc/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/esw/flow_db.h>
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
#include <bcm_int/esw/tsn.h>
#endif /* BCM_TSN_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

/****************************************************************
 *
 * L2 software tables, per unit.
 */

int _l2_init[BCM_MAX_NUM_UNITS];

#if defined(BCM_TRX_SUPPORT)
STATIC soc_profile_mem_t *_bcm_l2_port_cbl_profile[BCM_MAX_NUM_UNITS];
#endif /* BCM_TRX_SUPPORT */
_bcm_l2_match_ctrl_t *_bcm_l2_match_ctrl[BCM_MAX_NUM_UNITS];
static sal_sem_t l2_cb_check[BCM_MAX_NUM_UNITS];

/****************************************************************
 *
 * L2 learn limit software representation, per unit.
 */

#define BCM_MAC_LIMIT_MAX   (soc_mem_index_count(unit, L2Xm) - 1)

/* forward declaration */
static int _bcm_l2_bpdu_init(int unit);
STATIC int _bcm_esw_l2_replace(int unit, uint32 flags, uint32 int_flags,
                               bcm_l2_addr_t *match_addr,
                               bcm_module_t new_module, bcm_port_t new_port, 
                               bcm_trunk_t new_trunk,
                               bcm_l2_traverse_cb trav_fn, void *user_data);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
STATIC int _bcm_l2_learn_limit_init(int unit);
STATIC int _bcm_l2_learn_limit_porttrunk_set(int unit, int port_tgid, 
                                             uint32 flags, int limit);
STATIC int _bcm_l2_learn_limit_port_set(int unit, bcm_port_t port,
                                        uint32 flags, int limit);
STATIC int _bcm_l2_learn_limit_trunk_set(int unit, bcm_trunk_t trunk,
                                         uint32 flags, int limit);
#endif /* BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

/* L2 (vlan, port) tuple structure */
typedef struct _bcm_l2_vlan_port_pair_s {
    /* Array of PBMP with max VLAN size, the first 0-4095 PBMP entries 
     in the array are for max number of VLANs supported, the 4096th
     i.e pbm[BCM_VLAN_MAX+1]th entry in the array is to maintain a list
     of PBMPs for invalid VLANs (used for delete by gport operation). */
    bcm_pbmp_t        pbm[BCM_VLAN_MAX+2]; 
} _bcm_l2_vlan_port_pair_t;

typedef struct _bcm_l2_vlan_trunk_pair_s {
    SHR_BITDCL        *tbm[BCM_VLAN_MAX+2];
} _bcm_l2_vlan_trunk_pair_t;

typedef struct _bcm_l2_vlan_vp_pair_s {
    SHR_BITDCL        *vpbm[BCM_VLAN_MAX+2];
} _bcm_l2_vlan_vp_pair_t;

typedef struct _bcm_l2_del_vlan_port_pair_s {
    uint32                    flags;            /* BCM_L2_AUTH_XXX flags. */
    uint32                    replace_flags;        /* BCM_L2_REPLACE_XXX flags */
    bcm_vlan_vector_t         del_by_vlan_vec;  /* Vlan vector represents 
                                                   delete by vlan */
    _bcm_l2_vlan_port_pair_t  *mod_pair_arr;    /* Reference to Array of
                                                   (vlan,port) for max modules */
    _bcm_l2_vlan_trunk_pair_t *unit_pair_arr;   /* Reference to Array of
                                                   (vlan,tid) for each unit */
    _bcm_l2_vlan_vp_pair_t    *vp_pair_arr;     /* Reference to Array of
                                                   (vlan,vp) for each unit */
    SHR_BITDCL                **vfi_pair_arr;     /* Reference to Array of
                                                   (vfi,vp) for each unit */
    SHR_BITDCL                *del_by_vfi_vec;  /* VFI vector */
} _bcm_l2_del_vlan_port_pair_t;

typedef enum l2x_memacc_field_e {
    _BCM_L2_MEMACC_VALID,
    _BCM_L2_MEMACC_KEY_TYPE,
    _BCM_L2_MEMACC_NUM
} l2x_memacc_field_t;

STATIC uint32 _l2_dbg_matched_entries[SOC_MAX_NUM_DEVICES], 
              _l2_dbg_unmatched_entries[SOC_MAX_NUM_DEVICES];

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_esw_l2_wb_alloc(int unit);
STATIC int _bcm_esw_l2_wb_recover(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

/* just need to clear SRC or DES hit bit. Customers may call API bcm_esw_l2_replace
*  with parameter *match_addr which value is NULL */
#define _CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(flag) \
                  (!(flag & (BCM_L2_REPLACE_MATCH_VLAN |\
                             BCM_L2_REPLACE_MATCH_MAC |\
                             BCM_L2_REPLACE_MATCH_DEST)) &&\
                   (flag & (BCM_L2_REPLACE_DES_HIT_CLEAR |\
                            BCM_L2_REPLACE_SRC_HIT_CLEAR)))
/*
 * Function:
 *	_bcm_esw_l2_gport_parse
 * Description:
 *	Parse gport in the l2 table
 * Parameters:
 *	unit -      [IN] StrataSwitch PCI device unit number (driver internal).
 *  l2addr -    [IN/OUT] l2 addr structure to parse and fill
 *  sub_port -  [OUT] Virtual port group (-1 if not supported)
 *  mpls_port - [OUT] MPLS port (-1 if not supported)
 * Returns:
 *	BCM_E_XXX
 */
int 
_bcm_esw_l2_gport_parse(int unit, bcm_l2_addr_t *l2addr, 
                        _bcm_l2_gport_params_t *params)
{
    int             id;
    bcm_port_t      _port;
    bcm_trunk_t     _trunk;
    bcm_module_t    _modid, _mymodid;

    if ((NULL == l2addr) || (NULL == params)){
        return BCM_E_PARAM;
    }

    params->param0 = -1;
    params->param1 = -1;
    params->type = 0;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, l2addr->port, &_modid, &_port, 
                               &_trunk, &id));

    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &_mymodid));


    if ((BCM_TRUNK_INVALID != _trunk) && (-1 == id)) {
        params->param0 = _trunk;
        params->type = _SHR_GPORT_TYPE_TRUNK;
        return BCM_E_NONE;
    }

    if ((-1 != _port) && (-1 == id)) {
        params->param0 = _port;
        if ((_port == CMIC_PORT(unit)) && (_modid == _mymodid)) {
            params->type = _SHR_GPORT_TYPE_LOCAL_CPU;
            return BCM_E_NONE;
        }
        params->param1 = _modid;
        params->type = _SHR_GPORT_TYPE_MODPORT;
        return BCM_E_NONE;
    }

    if (-1 != id) {
        params->param0 = id;
        if (BCM_GPORT_IS_SUBPORT_GROUP(l2addr->port)) {
            params->type = _SHR_GPORT_TYPE_SUBPORT_GROUP;
        } else if (BCM_GPORT_IS_SUBPORT_PORT(l2addr->port)) {
            params->type = _SHR_GPORT_TYPE_SUBPORT_PORT;

#if defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                (soc_feature(unit, soc_feature_channelized_switching))) &&
                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, l2addr->port)) {
                params->param0 = _port;
                params->param1 = _modid;
            }
#endif
#if defined(BCM_KATANA2_SUPPORT)
            if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                l2addr->port)) {
                params->param0 = _port;
                params->param1 = _modid;
            }
#endif
        } else if (BCM_GPORT_IS_MPLS_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_MPLS_PORT;
        } else if (BCM_GPORT_IS_MIM_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_MIM_PORT;
        } else if (BCM_GPORT_IS_WLAN_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_WLAN_PORT;
        } else if (BCM_GPORT_IS_TRILL_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_TRILL_PORT;
        } else if (BCM_GPORT_IS_VLAN_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_VLAN_PORT;
        } else if (BCM_GPORT_IS_NIV_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_NIV_PORT;
        } else if (BCM_GPORT_IS_L2GRE_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_L2GRE_PORT;
        } else if (BCM_GPORT_IS_VXLAN_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_VXLAN_PORT;
        } else if (BCM_GPORT_IS_FLOW_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_FLOW_PORT;
        } else if (BCM_GPORT_IS_EXTENDER_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_EXTENDER_PORT;
        }

        return BCM_E_NONE;
    } else if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
       params->type = _SHR_GPORT_TYPE_BLACK_HOLE;
       params->param0 = 0;
       params->param1 = id;

       return BCM_E_NONE;
    }

    return BCM_E_PORT;
}

/*
 * Function:
 *	bcm_esw_l2_init
 * Description:
 *	Perform required initializations to L2 table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_init(int unit)
{
    int rv; 
#ifdef BCM_XGS_SWITCH_SUPPORT
    int frozen;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_init(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
         BCM_IF_ERROR_RETURN(bcm_tr3_l2_init(unit));
#ifdef BCM_WARM_BOOT_SUPPORT
        if (SOC_WARM_BOOT(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_l2_wb_recover(unit));
        } else {
            BCM_IF_ERROR_RETURN(_bcm_esw_l2_wb_alloc(unit));
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
         _l2_init[unit] = 1;		/* some positive value */
         return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    BCM_IF_ERROR_RETURN(
        soc_l2x_is_frozen(unit, SOC_L2X_FROZEN_WITH_LOCK, &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */
    _l2_init[unit] = 0;

#ifdef BCM_SHADOW_SUPPORT
    if (soc_feature(unit, soc_feature_no_bridging)) {
        return BCM_E_NONE;
    }
#endif

    if (SOC_IS_XGS_FABRIC(unit) && !SOC_IS_TITAN2X(unit)&& !SOC_IS_TOMAHAWK2(unit)) {        
        _l2_init[unit] = BCM_E_UNAVAIL;
        return BCM_E_NONE;
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        /* Turn off arl aging */
        BCM_IF_ERROR_RETURN
            (SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, 0, 0));

        bcm_esw_l2_detach(unit);
    }
    /* Proposal: during warm boot, customer wants to freeze l2x. 
    * After warm boot, customer will restore l2x.
    */
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TD2_TT2(unit) && SOC_WARM_BOOT(unit) &&
        soc_l2x_cml_override_is_enabled(unit)) {
        /* In this condition, soc_l2x_freeze does not change ING_MISC_CONFIG2r.
        * It just wants to Increment  l2 frozen indicator "f_l2->hw_frozen++". When we call
        * bcm_esw_l2_age_timer_set, we won't enable SW aging by mistake.
        */
        BCM_IF_ERROR_RETURN(soc_l2x_selective_freeze(unit, SOC_L2X_FROZEN_WITHOUT_LOCK));
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_l2_init(unit));

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {
        if (!soc_feature(unit, soc_feature_l2_user_table)) {
            /*
             * Init BPDU station address registers.
             */
            rv = _bcm_l2_bpdu_init(unit);
            if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
                return rv;
            }
        }

        /*
         * Init L2 cache
         */
        rv = bcm_esw_l2_cache_init(unit);
        if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
            return rv;
        }
    }

    /*
     * Init L2 learn limit
     */
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    rv = _bcm_l2_learn_limit_init(unit);
    if (BCM_FAILURE(rv) && (rv != BCM_E_UNAVAIL)) {
        return rv;
    }
#endif /* BCM_TRIUMPH_SUPPORT  || BCM_RAPTOR_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    /*
     * Init class based learning
     */
    if (soc_feature(unit, soc_feature_class_based_learning)) {
        soc_mem_t mem;
        int entry_words;
        int index_max;
        int lclass;
        union {
            port_cbl_table_entry_t port_cbl[256];
            uint32 w[1];
        } entry;
        void *entries[1];
        soc_profile_mem_t *profile;
        int module;
        uint32 profile_index;
        int field_len;

        /* Clear CBL attribute register and CBL table */
        if (SOC_REG_IS_VALID(unit, CBL_ATTRIBUTEr) && (!SOC_WARM_BOOT(unit))) {
            for (lclass = 0; lclass < SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr);
                 lclass++) {
                BCM_IF_ERROR_RETURN(WRITE_CBL_ATTRIBUTEr(unit, lclass, 0));
            }
        }

        if (!SOC_WARM_BOOT(unit)) {
            BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLEm,
                                                 COPYNO_ALL, FALSE));
        }
        if (soc_mem_is_valid(unit, PORT_CBL_TABLE_MODBASEm)) {
            /* Create profile for PORT_CBL_TABLE table */
            if (_bcm_l2_port_cbl_profile[unit] == NULL) {
                _bcm_l2_port_cbl_profile[unit] =
                    sal_alloc(sizeof(soc_profile_mem_t),
                              "PORT_CBL_TABLE profile");
                if (_bcm_l2_port_cbl_profile[unit] == NULL) {
                    return BCM_E_MEMORY;
                }
                soc_profile_mem_t_init(_bcm_l2_port_cbl_profile[unit]);
            }
            profile = _bcm_l2_port_cbl_profile[unit];
            mem = PORT_CBL_TABLEm;
            entry_words = sizeof(port_cbl_table_entry_t) / sizeof(uint32);
            field_len = soc_mem_field_length(unit, PORT_CBL_TABLE_MODBASEm,
                                             BASEf);
            index_max = (1 << field_len) - 1;
            entries[0] = &entry;
            sal_memset(&entry.port_cbl[0], 0, sizeof(entry.port_cbl[0]));
            field_len = soc_mem_field_length(unit, PORT_CBL_TABLEm,
                                             PORT_LEARNING_CLASSf);
            soc_mem_field32_set(unit, PORT_CBL_TABLEm, &entry.port_cbl[0],
                                PORT_LEARNING_CLASSf, (1 << field_len) - 1);

            BCM_IF_ERROR_RETURN
                (soc_profile_mem_index_create(unit, &mem, &entry_words, NULL,
                                              &index_max, entries, 1,
                                              profile));

            if (!SOC_WARM_BOOT(unit)) {
                BCM_IF_ERROR_RETURN(soc_mem_clear(unit, PORT_CBL_TABLE_MODBASEm,
                                            COPYNO_ALL, FALSE));
                sal_memset(entry.port_cbl, 0, sizeof(entry.port_cbl));
                BCM_IF_ERROR_RETURN
                    (soc_profile_mem_add(unit, profile, entries,
                                         SOC_PORT_ADDR_MAX(unit) + 1,
                                         &profile_index));
                for (module = 1; module <= SOC_MODID_MAX(unit); module++) {
                    SOC_IF_ERROR_RETURN
                        (soc_profile_mem_reference
                         (unit, profile, profile_index,
                          SOC_PORT_ADDR_MAX(unit) + 1));
                }
            }
#ifdef BCM_WARM_BOOT_SUPPORT
            else {
                port_cbl_table_modbase_entry_t base_entry;

                for (module = 0; module <= SOC_MODID_MAX(unit); module++) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm,
                                      MEM_BLOCK_ANY, module, &base_entry));
                    profile_index =
                        soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm,
                                            &base_entry, BASEf);
                    SOC_IF_ERROR_RETURN
                        (soc_profile_mem_reference
                         (unit, profile, profile_index,
                          SOC_PORT_ADDR_MAX(unit) + 1));
                }
            }
#endif /* BCM_WARM_BOOT_SUPPORT */
        }
    }

#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_l2_wb_recover(unit));
    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_l2_wb_alloc(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
#ifdef BCM_HURRICANE4_SUPPORT
    if (soc_feature(unit, soc_feature_hr4_a0_sobmh_war)) {
        BCM_IF_ERROR_RETURN(
            soc_hr4_sobmh_war_l2_vxlate_entry_update(unit, SOC_BASE_MODID(unit),
                                                 SOC_BASE_MODID(unit)));
    }
#endif /* BCM_HURRICANE4_SUPPORT */

    if (l2_cb_check[unit] == NULL) {
        l2_cb_check[unit] = sal_sem_create("l2 callback check", sal_sem_BINARY, 1);
        if (l2_cb_check[unit] == NULL) {
            return BCM_E_MEMORY;
        }
    }

    _l2_init[unit] = 1;		/* some positive value */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_l2_detach
 * Purpose:
 *      Clean up l2 bcm layer when unit is detached
 * Parameters:
 *      unit - unit being detached
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_detach(int unit)
{
    
#ifdef BCM_XGS_SWITCH_SUPPORT
    int frozen; 

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_detach(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
         BCM_IF_ERROR_RETURN(bcm_tr3_l2_detach(unit));

         _l2_init[unit] = 0;
         return BCM_E_NONE;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    /* Verify the table is not frozen */
    BCM_IF_ERROR_RETURN(
        soc_l2x_is_frozen(unit, SOC_L2X_FROZEN_WITH_LOCK, &frozen));
    if (TRUE == frozen) {
        return BCM_E_BUSY;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

#ifdef BCM_SHADOW_SUPPORT
    if (soc_feature(unit, soc_feature_no_bridging)) {
        return BCM_E_NONE;
    }
#endif

    if (_bcm_l2_match_ctrl[unit] && _bcm_l2_match_ctrl[unit]->preserved) {
        _l2_init[unit] = 0;
        return BCM_E_NONE;
    }

    /*
     * Call chip-dependent finalization.
     */
    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_term(unit));

    if (l2_cb_check[unit] != NULL) {
        sal_sem_destroy(l2_cb_check[unit]);
        l2_cb_check[unit] = NULL;
    }

    _l2_init[unit] = 0;
    return BCM_E_NONE;
}


/*
 * Function:
 *	_bcm_esw_delete_replace_flags_convert
 * Description:
 *	A helper function to all delete_by APIs to use bcm_l2_replace. 
 *  Converts L2 flags to L2 replace flags compatable to use with bcm_l2_replace
 * Parameters:
 *	unit        [IN]- device unit
 *	flags       [IN] - BCM_L2_DELETE_XXX
 *	repl_flags  [OUT]- Vflags BCM_L2_REPLACE_XXX
 * Returns:
 *	BCM_E_XXX	
 */

int 
_bcm_esw_delete_replace_flags_convert(int unit, uint32 flags, uint32 *repl_flags)
{
    uint32 tmp_flags = BCM_L2_REPLACE_DELETE;

    /* Age feature only supported by Bulk Control */
    if (soc_feature(unit, soc_feature_l2_bulk_control)) {

        /* Both Age and Delete flags cannot be specified */ 
        if ( (flags & BCM_L2_REPLACE_AGE) && (flags & BCM_L2_REPLACE_DELETE) ){
            return BCM_E_PARAM;
        }

        if (flags & BCM_L2_REPLACE_AGE){
            tmp_flags = BCM_L2_REPLACE_AGE;
        }
    } else {
        if (flags & BCM_L2_REPLACE_AGE){
            return BCM_E_UNAVAIL;
        }
    }

    if (flags & BCM_L2_DELETE_PENDING) {
        tmp_flags |= BCM_L2_REPLACE_PENDING;
    }
    if (flags & BCM_L2_DELETE_STATIC) {
        tmp_flags |= BCM_L2_REPLACE_MATCH_STATIC; 
    }
    if (flags & BCM_L2_DELETE_NO_CALLBACKS) {
        tmp_flags |= BCM_L2_REPLACE_NO_CALLBACKS;
    }
    if (flags & BCM_L2_DELETE_REMOTE_TRUNK) {
        tmp_flags |= BCM_L2_REPLACE_REMOTE_TRUNK;
    }
    if (flags & BCM_L2_REPLACE_VPN_TYPE) {
        tmp_flags |= BCM_L2_REPLACE_VPN_TYPE;
    }
    *repl_flags = tmp_flags;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_add
 * Description:
 *	Add a MAC address to the Switch Address Resolution Logic (ARL)
 *	port with the given VLAN ID and parameters.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Use CMIC_PORT(unit) to associate the entry with the CPU.
 *	Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 */

int
bcm_esw_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_add(unit, l2addr);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_add(unit, l2addr);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    /* Input parameters check. */
    if (NULL == l2addr) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_l2_drop_mc_mac)) {
        if (l2addr->flags & BCM_L2_DISCARD_DST) {
            if (MAC_IS_MCAST(l2addr->mac)) {
                return (BCM_E_UNAVAIL);
            }
        }
    }

    if (l2addr->flags & BCM_L2_LOCAL_CPU) {
        l2addr->port = CMIC_PORT(unit);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_modid_get(unit, &l2addr->modid));
    }

    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        BCM_IF_ERROR_RETURN(
            _bcm_trunk_id_validate(unit, l2addr->tgid));
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) && 
            !BCM_L2_PRE_SET(l2addr->flags)) {
            if ((l2addr->group > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return (BCM_E_PARAM);
            }
        } else if ((l2addr->group > SOC_ADDR_CLASS_MAX(unit)) || 
                   (l2addr->group < 0)) {
            return (BCM_E_PARAM);
        }
        if (!BCM_PBMP_IS_NULL(l2addr->block_bitmap)) {
            return (BCM_E_PARAM);
        }
    } else {
        if (l2addr->group)  {
            return (BCM_E_PARAM);
        }
    }

    /* Input parameters check for TSN related parameters */
    if (!soc_feature(unit, soc_feature_tsn)) {
        if (l2addr->tsn_flowset) {
            return BCM_E_UNAVAIL;
        }
    }

    /* Input parameters check for SR related parameters */
    if (!soc_feature(unit, soc_feature_tsn_sr)) {
        if (l2addr->flags & BCM_L2_SR_SAN_DEST) {
            return BCM_E_UNAVAIL;
        }
        if (!BCM_PBMP_IS_NULL(l2addr->sa_source_filter_pbmp)) {
            return BCM_E_UNAVAIL;
        }
        if (l2addr->sr_flowset) {
            return BCM_E_UNAVAIL;
        }
    }

    SOC_IF_ERROR_RETURN
	(mbcm_driver[unit]->mbcm_l2_addr_add(unit, l2addr));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete
 * Description:
 *	Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *	unit - device unit
 *	mac  - MAC address to delete
 *	vid  - VLAN id
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete(unit, mac, vid);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete(unit, mac, vid);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return mbcm_driver[unit]->mbcm_l2_addr_delete(unit, mac, vid);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_port
 * Description:
 *	Delete L2 addresses associated with a destination module/port.
 * Parameters:
 *	unit  - device unit
 *	mod   - module id (or -1 for local unit)
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */

int
bcm_esw_l2_addr_delete_by_port(int unit,
                               bcm_module_t mod, bcm_port_t port,
                               uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;
    uint32          int_flags = 0;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int             id = -1;
    bcm_port_t      _port = -1;
    bcm_trunk_t     _trunk = -1;
    bcm_module_t    _modid = -1;
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_port(unit, mod, port, flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_port(unit, mod, port, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    if (port == -1) {
        match_addr.modid = mod;
        match_addr.port  = -1;
        int_flags = _BCM_L2_REPLACE_IGNORE_PORTID;
    } else if (!BCM_GPORT_IS_SET(port) && mod == -1) {
#ifdef BCM_KATANA2_SUPPORT
        if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
            (soc_feature(unit, soc_feature_subtag_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, port))) {
                return BCM_E_PORT;
        } else
#endif
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            BCM_GPORT_IS_SET(port) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                           &_trunk, &id));
                match_addr.modid = _modid;
                port = _port;
        } else 
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                       &_trunk, &id));
            match_addr.modid = _modid;
            port = _port;
        } else
#endif
        {
            match_addr.modid = mod;
        }
    }
    match_addr.port = port;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_esw_l2_replace(unit, repl_flags, int_flags, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_mac
 * Description:
 *	Delete L2 entries associated with a MAC address.
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_mac(int unit, bcm_mac_t mac, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_mac(unit, mac, flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_mac(unit, mac, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));    
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    /* Traverse l2 entry contains MAC address field by key type. */
    /* Currently, L2 Bridge and VFI are suported. */
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_VPN_TYPE;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0, NULL, NULL));
    repl_flags &= ~BCM_L2_REPLACE_VPN_TYPE;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan
 * Description:
 *	Delete L2 entries associated with a VLAN.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan(int unit, bcm_vlan_t vid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_vlan(unit, vid, flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_vlan(unit, vid, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_trunk
 * Description:
 *	Delete L2 entries associated with a trunk.
 * Parameters:
 *	unit  - device unit
 *	tid   - trunk id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_trunk(int unit, bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_trunk(unit, tid, flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_trunk(unit, tid, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_mac_port
 * Description:
 *	Delete L2 entries associated with a MAC address and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	mac   - MAC address
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_mac_port(int unit, bcm_mac_t mac,
                         bcm_module_t mod, bcm_port_t port, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;
#if (defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT))
    int             id = -1;
    bcm_port_t      _port = -1;
    bcm_trunk_t     _trunk = -1;
    bcm_module_t    _modid = -1;
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_mac_port(unit, mac, mod, port,
                                                        flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_mac_port(unit, mac,
                                                  mod, port, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    sal_memcpy(match_addr.mac, mac, sizeof(bcm_mac_t));
    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
#ifdef BCM_KATANA2_SUPPORT
        if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
            (soc_feature(unit, soc_feature_subtag_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, port))) {
            return BCM_E_PORT;
        } else
#endif
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                           &_trunk, &id));
                match_addr.modid = _modid;
                port = _port;
        } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                       &_trunk, &id));
            match_addr.modid = _modid;
            port = _port;
        } else
#endif
        {
            match_addr.modid = mod;
        }
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan_port
 * Description:
 *	Delete L2 entries associated with a VLAN and
 *	a destination module/port
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	mod   - module id
 *	port  - port
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan_port(int unit, bcm_vlan_t vid,
                           bcm_module_t mod, bcm_port_t port, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int             id = -1;
    bcm_port_t      _port = -1;
    bcm_trunk_t     _trunk = -1;
    bcm_module_t    _modid = -1;
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_vlan_port(unit, vid, mod, port,
                                                         flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_vlan_port(unit, vid,
                                                   mod, port, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    if (!BCM_GPORT_IS_SET(port) && mod == -1) {
#ifdef BCM_KATANA2_SUPPORT
        if ((soc_feature(unit, soc_feature_linkphy_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).linkphy_pp_port_pbm, port)) ||
            (soc_feature(unit, soc_feature_subtag_coe) &&
            BCM_PBMP_MEMBER (SOC_INFO(unit).subtag_pp_port_pbm, port))) {
            return BCM_E_PORT;
        } else
#endif
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &match_addr.modid));
    } else {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                           &_trunk, &id));
                match_addr.modid = _modid;
                port = _port;
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &_modid, &_port, 
                                       &_trunk, &id));
            match_addr.modid = _modid;
            port = _port;
        } else
#endif
        {
            match_addr.modid = mod;
        }
    }
    match_addr.port = port;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_esw_l2_addr_delete_by_vlan_trunk
 * Description:
 *	Delete L2 entries associated with a VLAN and a
 *      destination trunk.
 * Parameters:
 *	unit  - device unit
 *	vid   - VLAN id
 *	tid   - trunk group id
 *	flags - BCM_L2_DELETE_XXX
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Static entries are removed only if BCM_L2_DELETE_STATIC flag is used.
 *	L2 aging and learning are disabled during this operation.
 */
int
bcm_esw_l2_addr_delete_by_vlan_trunk(int unit, bcm_vlan_t vid,
                                 bcm_trunk_t tid, uint32 flags)
{
    bcm_l2_addr_t   match_addr;
    uint32          repl_flags;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_vlan_trunk(unit, vid, tid,
                                                          flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_delete_by_vlan_trunk(unit, vid, tid, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    sal_memset(&match_addr, 0, sizeof(bcm_l2_addr_t));
    match_addr.vid = vid;
    match_addr.tgid = tid;
    match_addr.flags = BCM_L2_TRUNK_MEMBER;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_delete_replace_flags_convert(unit, flags, &repl_flags));
    repl_flags |= BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST;
    return _bcm_esw_l2_replace(unit, repl_flags, 0, &match_addr, 0, 0, 0,
                               NULL, NULL);
}

/*
 * Function:
 *	bcm_l2_addr_get
 * Description:
 *	Given a MAC address and VLAN ID, check if the entry is present
 *	in the L2 table, and if so, return all associated information.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	mac - input MAC address to search
 *	vid - input VLAN ID to search
 *	l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *	BCM_E_NONE		Success (l2addr filled in)
 *	BCM_E_PARAM		Illegal parameter (NULL pointer)
 *	BCM_E_INTERNAL		Chip access failure
 *	BCM_E_NOT_FOUND	        Address not found (l2addr not filled in)
 */

int
bcm_esw_l2_addr_get(int unit, sal_mac_addr_t mac,
		bcm_vlan_t vid, bcm_l2_addr_t *l2addr)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_get(unit, mac, vid, l2addr);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_get(unit, mac, vid, l2addr);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    SOC_IF_ERROR_RETURN(
        mbcm_driver[unit]->mbcm_l2_addr_get(unit, mac, vid, l2addr));

    return BCM_E_NONE;
}

/****************************************************************************
 *
 * L2X Message Registration
 */

#define L2_CB_MAX		3

typedef struct l2x_cb_entry_s {
    bcm_l2_addr_callback_t	fn;
    void			*fn_data;
} l2_cb_entry_t;

#define _BCM_L2X_THREAD_STOP      (1 << 0)
typedef struct l2_data_s {
    l2_cb_entry_t		cb[L2_CB_MAX];
    int				cb_count;
    int                 flags;
    sal_mutex_t                 l2_mutex;
} l2_data_t;

static l2_data_t l2_data[SOC_MAX_NUM_DEVICES];


#define L2_LOCK(unit) \
    sal_mutex_take(l2_data[unit].l2_mutex, sal_mutex_FOREVER)

#define L2_UNLOCK(unit) sal_mutex_give(l2_data[unit].l2_mutex)

#define L2_MUTEX(unit) if (l2_data[unit].l2_mutex == NULL &&                 \
        (l2_data[unit].l2_mutex = sal_mutex_create("bcm_l2_lock")) == NULL)  \
            return BCM_E_MEMORY

/*
 * Function:
 *      bcm_esw_l2_port_native
 * Purpose:
 *      Determine if the given port is "native" from the point
 *      of view of L2.
 * Parameters:
 *      unit       - The unit
 *      modid      - Module ID of L2 entry being queried
 *      port       - Module port of L2 entry being queried
 * Returns:
 *      TRUE (> 0) if (modid, port) is front panel/CPU port for unit.
 *      FALSE (0) otherwise.
 *      < 0 on error.
 *
 *      Native means front panel, but also includes the CPU port.
 *      HG ports are always excluded as are ports marked as internal or
 *      external stacking
 */

int
bcm_esw_l2_port_native(int unit, int modid, int port)
{
    int unit_modid;
    int unit_port;  /* In case unit supports multiple module IDs */
    bcm_trunk_t     tgid;
    int             id, isLocal;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_port_native(unit, modid, port);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_port_native(unit, modid, port);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &modid,
            &port, &tgid, &id));

        if (-1 != id || BCM_TRUNK_INVALID != tgid) {
            return FALSE;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_modid_is_local(unit, modid, &isLocal));
    if (isLocal != TRUE) {
        return FALSE;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &unit_modid));
    if (unit_modid != modid) {
        unit_port = port + 32; 
    } else {
        unit_port = port;
    }

    if (IS_ST_PORT(unit, unit_port)) {
        return FALSE;
    } 

    return TRUE;
}


/****************************************************************************
 *
 * L2 Callbacks Registration
 */
static bcm_l2_addr_callback_t _bcm_l2_cbs[SOC_MAX_NUM_DEVICES];
static void *_bcm_l2_cb_data[SOC_MAX_NUM_DEVICES];

#if defined(BCM_TSN_SUPPORT)
/*
 * Function:
 *    bcmi_l2_addr_tsn_sr_flowset_ref_update
 * Purpose:
 *    Update reference count for the specified TSN/SR flowset
 *    associated it to a L2 addition/deletion
 * Parameters:
 *    unit      - unit number.
 *    entry_del - entry deleted from the HW L2_ENTRY memory
 *    entry_add - entry added into the HW L2_ENTRY memory
 * Returns:
 *    BCM_E_NONE
 */
STATIC int
bcmi_l2_addr_tsn_sr_flowset_ref_update(
    int unit,
    uint32 *entry_del,
    uint32 *entry_add)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr_del, l2addr_add;
#if defined(BCM_TSN_SR_SUPPORT)
    int mp_index = 0; /* TSN SR MAC Proxy profile index */
    bcm_pbmp_t mp_pbmp;
#endif /* BCM_TSN_SR_SUPPORT */

    L2_INIT(unit);

    /* Input parameters check */
    if ((entry_del == NULL) && (entry_add == NULL)) {
        return BCM_E_NONE;
    }

    if (entry_del != NULL) {
        /* Parse L2X entry into L2_Addr structure */
        (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_del, entry_del);
    }
    if (entry_add != NULL) {
        /* Parse L2X entry into L2_Addr structure */
        (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_add, entry_add);
    }

    if (soc_feature(unit, soc_feature_tsn)) {
        if ((entry_del != NULL) && (l2addr_del.tsn_flowset != 0)) {
            /* decrease TSN flowset ref_cnt */
            rv = bcmi_esw_tsn_flowset_ref_dec(unit, l2addr_del.tsn_flowset);
            if (BCM_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_ARL,
                         (BSL_META_U(unit,
                                     "bcmi_l2_addr_tsn_sr_flowset_ref_update:"
                                     " can't decrease TSN flowset ref_cnt!!!"
                                     "\n")));
                return BCM_E_NONE;
            }
        }
        if ((entry_add != NULL) && (l2addr_add.tsn_flowset != 0)) {
            /* increase TSN flowset ref_cnt */
            rv = bcmi_esw_tsn_flowset_ref_inc(unit, l2addr_add.tsn_flowset);
            if (BCM_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_ARL,
                         (BSL_META_U(unit,
                                     "bcmi_l2_addr_tsn_sr_flowset_ref_update:"
                                     " can't increase TSN flowset ref_cnt!!!"
                                     "\n")));
                return BCM_E_NONE;
            }
        }
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if ((entry_del != NULL) && (l2addr_del.sr_flowset != 0)) {
            /* decrease TSN SR flowset ref_cnt */
            rv = bcmi_esw_tsn_sr_flowset_ref_dec(unit, l2addr_del.sr_flowset);
            if (BCM_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_ARL,
                         (BSL_META_U(unit,
                                     "bcmi_l2_addr_tsn_sr_flowset_ref_update:"
                                     " can't decrease SR flowset ref_cnt!!!"
                                     "\n")));
                return BCM_E_NONE;
            }
            /* decrease TSN SR MAC Proxy ref_cnt */
            if (!BCM_PBMP_IS_NULL(l2addr_del.sa_source_filter_pbmp)) {
                mp_index = soc_L2Xm_field32_get(unit, entry_del,
                                                SR_MAC_PROXY_PROFILE_PTRf);
                if (mp_index != 0) {
                    rv = bcmi_esw_tsn_sr_mac_proxy_delete(unit, mp_index);
                    if (BCM_FAILURE(rv)) {
                        LOG_WARN(BSL_LS_BCM_ARL,
                                 (BSL_META_U(unit,
                                             "bcmi_esw_tsn_sr_mac_proxy_delete"
                                             ": can't delete SR MAC Proxy!!!"
                                             "\n")));
                        return BCM_E_NONE;
                    }
                }
            }
        }
        if ((entry_add != NULL) && (l2addr_add.sr_flowset != 0)) {
            /* increase TSN SR flowset ref_cnt */
            rv = bcmi_esw_tsn_sr_flowset_ref_inc(unit, l2addr_add.sr_flowset);
            if (BCM_FAILURE(rv)) {
                LOG_WARN(BSL_LS_BCM_ARL,
                         (BSL_META_U(unit,
                                     "bcmi_l2_addr_tsn_sr_flowset_ref_update:"
                                     " can't increase SR flowset ref_cnt!!!"
                                     "\n")));
                return BCM_E_NONE;
            }
            /* increase TSN SR MAC Proxy ref_cnt */
            if (!BCM_PBMP_IS_NULL(l2addr_add.sa_source_filter_pbmp)) {
                /* The value should be inverted between SW and HW view */
                BCM_PBMP_NEGATE(mp_pbmp, l2addr_add.sa_source_filter_pbmp);
                BCM_PBMP_AND(mp_pbmp, PBMP_ALL(unit));
                rv = bcmi_esw_tsn_sr_mac_proxy_insert(unit, mp_pbmp,
                                                      &mp_index);
                if (BCM_FAILURE(rv)) {
                    LOG_WARN(BSL_LS_BCM_ARL,
                             (BSL_META_U(unit,
                                         "bcmi_esw_tsn_sr_mac_proxy_insert"
                                         ": can't insert SR MAC Proxy!!!"
                                         "\n")));
                    return BCM_E_NONE;
                }
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT */

    return rv;
}
#endif /* BCM_TSN_SUPPORT */

/*
 * Function:
 *      _bcm_l2_register_callback
 * Description:
 *      Call back to handle bcm_l2_addr_register clients.
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      entry_del - Entry to be deleted or updated, NULL if none.
 *      entry_add - Entry to be inserted or updated, NULL if none.
 *      fn_data - unused
 * Notes:
 *      Only one callback per unit to the bcm layer is supported here.
 *      Multiple bcm client callbacks per unit are supported in the bcm layer.
 */
void
_bcm_l2_register_callback(int unit,
                          int pflags,
                          l2x_entry_t *entry_del,
                          l2x_entry_t *entry_add,
                          void *fn_data)
{
    bcm_l2_addr_t l2addr_del, l2addr_add;
    _bcm_l2_match_ctrl_t *match_ctrl;
    l2x_entry_t *l2x_entry_buf;
    uint32 flags = 0;
    int buf_index, entry_index;
    int l2_callback_event;

    if (_l2_init[unit] == 0) {
        return;
    }

#if defined(BCM_TSN_SUPPORT)
    if (pflags & SOC_L2X_ENTRY_TSN_SR_FLOWSET_REF_CNT) {
        if (soc_feature(unit, soc_feature_tsn)) {
            (void)bcmi_l2_addr_tsn_sr_flowset_ref_update
                      (unit, (uint32 *)entry_del, (uint32 *)entry_add);
        }
    } else
#endif /* BCM_TSN_SUPPORT */
    if (pflags & (SOC_L2X_ENTRY_DUMMY | SOC_L2X_ENTRY_NO_ACTION)) {
        /* pass l2addr_add info match */
        match_ctrl = _bcm_l2_match_ctrl[unit];
        buf_index = match_ctrl->entry_wr_idx / _BCM_L2_MATCH_ENTRY_BUF_SIZE;
        entry_index = match_ctrl->entry_wr_idx % _BCM_L2_MATCH_ENTRY_BUF_SIZE;
        if (buf_index >= _BCM_L2_MATCH_ENTRY_BUF_COUNT) {
            LOG_WARN(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit,
                                 "Buf_index is overflow!!!\n")));
            return;
        }
        if (match_ctrl->l2x_entry_buf[buf_index] == NULL) {
            match_ctrl->l2x_entry_buf[buf_index] =
                sal_alloc(sizeof(l2x_entry_t) * _BCM_L2_MATCH_ENTRY_BUF_SIZE,
                          "l2_matched_traverse");
            if (match_ctrl->l2x_entry_buf[buf_index] == NULL) {
                LOG_WARN(BSL_LS_BCM_ARL,
                         (BSL_META_U(unit,
                                     "Fail to allocate space "
                                     "for L2 matched traverse!!!\n")));
                return;
            }
        }
        l2x_entry_buf = match_ctrl->l2x_entry_buf[buf_index];

        if (pflags & SOC_L2X_ENTRY_NO_ACTION) {
            l2x_entry_buf[entry_index] = *entry_add;
        } else {
            sal_memcpy(&l2x_entry_buf[entry_index],
                       soc_mem_entry_null(unit, L2Xm), sizeof(l2x_entry_t));
        }
        match_ctrl->entry_wr_idx++;
        sal_sem_give(match_ctrl->sem);
        sal_thread_yield();
    } else if (_bcm_l2_cbs[unit] != NULL) {
        flags = 0; /* Common flags: Move, From/to native */

        sal_memset(&l2addr_del, 0, sizeof(bcm_l2_addr_t));
        sal_memset(&l2addr_add, 0, sizeof(bcm_l2_addr_t));

        /* First, set up the entries:  decode HW entries and set flags */
        if (entry_del != NULL) {
            (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_del, (uint32 *)entry_del);
        }
        if (entry_add != NULL) {
            (void)_bcm_esw_l2_from_l2x(unit, 0, &l2addr_add, (uint32 *)entry_add);
        }

        if ((entry_del != NULL) && (entry_add != NULL)) { /* It's a move */
            flags |= BCM_L2_MOVE;
            if (SOC_USE_GPORT(unit)) {
                if (l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            } else {
                if (l2addr_del.modid != l2addr_add.modid ||
                    l2addr_del.tgid != l2addr_add.tgid ||
                    l2addr_del.port != l2addr_add.port) {
                    flags |= BCM_L2_MOVE_PORT;
                }
            }
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_esw_l2_port_native(unit, l2addr_del.modid,
                                           l2addr_del.port) > 0) {
                    flags |= BCM_L2_FROM_NATIVE;
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
            }
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_esw_l2_port_native(unit, l2addr_add.modid,
                                           l2addr_add.port) > 0) {
                    flags |= BCM_L2_TO_NATIVE;
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
            l2addr_del.flags |= flags;
            l2addr_add.flags |= flags;
        } else if (entry_del != NULL) { /* Age out or simple delete */
            if (!(l2addr_del.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_esw_l2_port_native(unit, l2addr_del.modid,
                                            l2addr_del.port) > 0) {
                    l2addr_del.flags |= BCM_L2_NATIVE;
                }
            }
        } else if (entry_add != NULL) { /* Insert or learn */
            if (!(l2addr_add.flags & BCM_L2_TRUNK_MEMBER)) {
                if (bcm_esw_l2_port_native(unit, l2addr_add.modid,
                                           l2addr_add.port) > 0) {
                    l2addr_add.flags |= BCM_L2_NATIVE;
                }
            }
        }

#ifdef BCM_TRIDENT_SUPPORT
        /* Filter out the erroneous station moves. */
        if (SOC_IS_TRIDENT(unit) &&
            !(sal_memcmp(&l2addr_del, &l2addr_add, sizeof(bcm_l2_addr_t)))) {
            return;
        }
#endif /* BCM_TRIDENT_SUPPORT */


        if (l2_cb_check[unit] != NULL) {
            sal_sem_take(l2_cb_check[unit], sal_sem_FOREVER);
        }

        /* The entries are now set up.  Make the callbacks */
        if (entry_del != NULL) {
            if (pflags & SOC_L2X_ENTRY_DELETE) {
                l2_callback_event = BCM_L2_CALLBACK_DELETE;
            } else if (pflags & SOC_L2X_ENTRY_AGE) {
                l2_callback_event = BCM_L2_CALLBACK_AGE_EVENT;
            } else {
                l2_callback_event = BCM_L2_CALLBACK_DELETE;
            }
            if (_bcm_l2_cbs[unit] != NULL) {
                _bcm_l2_cbs[unit](unit, &l2addr_del,
                                  l2_callback_event,
                                  _bcm_l2_cb_data[unit]);
            }
        }

        if (pflags & SOC_L2X_ENTRY_OVERFLOW) {
            l2addr_add.flags |= BCM_L2_ENTRY_OVERFLOW;
        }

        if (entry_add != NULL) {
            if ((flags & BCM_L2_MOVE) &&
                ((pflags & (SOC_L2X_ENTRY_INSERT | SOC_L2X_ENTRY_LEARN)))) {
                l2_callback_event = BCM_L2_CALLBACK_MOVE_EVENT;
            } else if (pflags & SOC_L2X_ENTRY_INSERT) {
                l2_callback_event = BCM_L2_CALLBACK_ADD;
            } else if (pflags & SOC_L2X_ENTRY_LEARN) {
                l2_callback_event = BCM_L2_CALLBACK_LEARN_EVENT;
            } else {
                l2_callback_event = BCM_L2_CALLBACK_ADD;
            }
            if (_bcm_l2_cbs[unit] != NULL) {
                _bcm_l2_cbs[unit](unit, &l2addr_add,
                                  l2_callback_event,
                                  _bcm_l2_cb_data[unit]);
            }
        }
        if (l2_cb_check[unit] != NULL) {
            sal_sem_give(l2_cb_check[unit]);
        }
    }
}



/*
 * Function:
 * 	_bcm_l2_addr_callback
 * Description:
 *	Callback used with chip addr registration functions.
 *	This callback calls all the top level client callbacks.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	l2addr
 *	insert
 *	userdata
 * Returns:
 */
static void
_bcm_l2_addr_callback(int unit,
		      bcm_l2_addr_t *l2addr,
		      int insert,
		      void *userdata)
{
    l2_data_t		*ad = &l2_data[unit];
    int i;

    if (ad->l2_mutex == NULL) {
        return;
    }

    L2_LOCK(unit);
    for(i = 0; i < L2_CB_MAX; i++) {
	if(ad->cb[i].fn) {
	    ad->cb[i].fn(unit, l2addr, insert, ad->cb[i].fn_data);
	}
    }
    L2_UNLOCK(unit);
}


/*
 * Function:
 *	bcm_esw_l2_addr_register
 * Description:
 *	Register a callback routine that will be called whenever
 *	an entry is inserted into or deleted from the L2 address table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	fn - Callback function of type bcm_l2_addr_callback_t.
 *	fn_data - Arbitrary value passed to callback along with messages
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_register(int unit,
                         bcm_l2_addr_callback_t fn,
                         void *fn_data)
{
    l2_data_t		*ad = &l2_data[unit];
    int i;
#if defined(BCM_XGS_SWITCH_SUPPORT)
    int rv = BCM_E_NONE;
    int usec;
#endif /* BCM_XGS_SWITCH_SUPPORT */

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_register(unit, fn, fn_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_register(unit, fn, fn_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    _bcm_l2_cbs[unit] = _bcm_l2_addr_callback;
    _bcm_l2_cb_data[unit] = NULL;

    L2_MUTEX(unit);
    L2_LOCK(unit);
#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Start L2x thread if it isn't running already. */
    if (!soc_l2x_running(unit, NULL, NULL)) {
        usec = (SAL_BOOT_BCMSIM && !SOC_IS_TRIDENT3X(unit)) ? \
                BCMSIM_L2XMSG_INTERVAL : 3000000;
        usec = soc_property_get(unit, spn_L2XMSG_THREAD_USEC, usec);
        rv = soc_l2x_start(unit, 0, usec);
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_l2_cbs[unit] = NULL;
            _bcm_l2_cb_data[unit] = NULL;
            L2_UNLOCK(unit);
            return(rv);
        }
        ad->flags |= _BCM_L2X_THREAD_STOP;
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_l2_overflow)) {
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            rv = soc_td2_l2_overflow_start(unit);
        }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
        if (SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
            rv = soc_hr3_l2_overflow_start(unit);
        }
#endif /* BCM_HURRICANE3_SUPPORT */
        if ((BCM_FAILURE(rv)) && (rv != BCM_E_UNAVAIL)) {
            _bcm_l2_cbs[unit] = NULL;
            _bcm_l2_cb_data[unit] = NULL;
            L2_UNLOCK(unit);
            return(rv);
        }
    }
#endif

    /* See if the function is already registered (with same data) */
    for (i = 0; i < L2_CB_MAX; i++) {
	if (ad->cb[i].fn == fn && ad->cb[i].fn_data == fn_data) {
            L2_UNLOCK(unit);
            return BCM_E_NONE;
        }
    }

    /* Not found; add to list. */
    for (i = 0; i < L2_CB_MAX; i++) {
	if (ad->cb[i].fn == NULL) {
	    ad->cb[i].fn = fn;
	    ad->cb[i].fn_data = fn_data;
	    ad->cb_count++;
	    break;
	}
    }

    L2_UNLOCK(unit);
    return i >= L2_CB_MAX ? BCM_E_RESOURCE : BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_addr_unregister
 * Description:
 *	Unregister a previously registered callback routine.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	fn - Same callback function used to register callback
 *	fn_data - Same arbitrary value used to register callback
 * Returns:
 *	BCM_E_NONE		Success, handle valid
 *	BCM_E_MEMORY		Out of memory
 *	BCM_E_INTERNAL		Chip access failure
 * Notes:
 *	Both callback and userdata must match from original register call.
 */

int
bcm_esw_l2_addr_unregister(int unit,
		       bcm_l2_addr_callback_t fn,
		       void *fn_data)
{
    l2_data_t		*ad = &l2_data[unit];
    int             rv = BCM_E_NOT_FOUND;
    int			i;

    if (!SOC_IS_XGS_SWITCH(unit)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_unregister(unit, fn, fn_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_addr_unregister(unit, fn, fn_data);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    L2_INIT(unit);
    L2_MUTEX(unit);
    if (l2_cb_check[unit] != NULL) {
        sal_sem_take(l2_cb_check[unit], sal_sem_FOREVER);
    }
    L2_LOCK(unit);

    for (i = 0; i < L2_CB_MAX; i++) {
        if((ad->cb[i].fn == fn) && (ad->cb[i].fn_data == fn_data)) {
            rv = BCM_E_NONE;
            ad->cb[i].fn = NULL;
            ad->cb[i].fn_data = NULL;
            ad->cb_count--;
            if (ad->cb_count == 0) {
#if defined(BCM_XGS_SWITCH_SUPPORT)
                /* Stop l2x thread if callback registration started it. */
                if (ad->flags & _BCM_L2X_THREAD_STOP) {
                    rv = soc_l2x_stop(unit);
                    ad->flags &= ~_BCM_L2X_THREAD_STOP;
                }
#endif /* BCM_XGS_SWITCH_SUPPORT */
                _bcm_l2_cbs[unit] = NULL;
                _bcm_l2_cb_data[unit] = NULL;
            }
        }
    }

    L2_UNLOCK(unit);
    if (l2_cb_check[unit] != NULL) {
        sal_sem_give(l2_cb_check[unit]);
    }
    return (rv);
}
int _bcm_esw_l2_age_timer_set(int unit, int age_seconds, int enabled)
{
    int			rv;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int         frozen = 0;
#endif
    /* Lock ARLm b/c soc_arl_freeze() does read/modify/write of AGE_TIMER */
    SOC_L2X_MEM_LOCK(unit);
#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_is_frozen(unit, SOC_L2X_FROZEN, &frozen);

    if ((!SOC_IS_TRIUMPH3(unit))&& frozen) {
        soc_age_timer_cache_set(unit, age_seconds, enabled);
        enabled = 0; /* L2x is frozen, so we can't enable aging function */
    } 
#endif

    rv = SOC_FUNCTIONS(unit)->soc_age_timer_set(unit, age_seconds, enabled);
    SOC_L2X_MEM_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	bcm_esw_l2_age_timer_set
 * Description:
 *	Set the age timer for all blocks.
 *	Setting the value to 0 disables the age timer.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	age_seconds - Age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_age_timer_set(int unit, int age_seconds)
{
    int			max_value;
    int			enabled;
    int			rv;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_age_timer_set(unit, age_seconds);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_age_timer_set(unit, age_seconds);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    BCM_IF_ERROR_RETURN
	(SOC_FUNCTIONS(unit)->soc_age_timer_max_get(unit, &max_value));

    if (age_seconds < 0 || age_seconds > max_value) {
        return BCM_E_PARAM;
    }

    enabled = age_seconds ? 1 : 0;

    rv = _bcm_esw_l2_age_timer_set(unit, age_seconds, enabled);

    return rv;
}

/*
 * Function: *	bcm_esw_l2_age_timer_get
 * Description:
 *	Returns the current age timer value.
 *	The value is 0 if aging is not enabled.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	age_seconds - Place to store returned age timer value in seconds
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_age_timer_get(int unit, int *age_seconds)
{
    int			seconds, enabled;
    int			rv;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_age_timer_get(unit, age_seconds);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

    if (age_seconds == NULL) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_age_timer_get(unit, age_seconds);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    /*
     * Lock ARLm because soc_arl_freeze() does read/modify/write of
     * AGE_TIMER
     */

    SOC_L2X_MEM_LOCK(unit);
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, &seconds, &enabled);
    SOC_L2X_MEM_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);

    if (enabled) {
        *age_seconds = seconds;
    } else {
        *age_seconds = 0;
    }

    return BCM_E_NONE;
}

/*internal function*/
int
_bcm_esw_l2_age_timer_get(int unit, int *age_seconds)
{
    int         rv = BCM_E_NONE;
#ifdef BCM_XGS_SWITCH_SUPPORT
    int         frozen = 0;
#endif
    int         enabled;
    /* Lock ARLm b/c soc_arl_freeze() does read/modify/write of AGE_TIMER */
    SOC_L2X_MEM_LOCK(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    rv = soc_l2x_is_frozen(unit, SOC_L2X_FROZEN, &frozen);

    if (BCM_FAILURE(rv)) {
        frozen = 0;
    }
    if ((!SOC_IS_TRIUMPH3(unit)) && frozen) {
        soc_age_timer_cache_get(unit, age_seconds, &enabled);

		SOC_L2X_MEM_UNLOCK(unit);
        return rv;
    }
#endif
    rv = SOC_FUNCTIONS(unit)->soc_age_timer_get(unit, age_seconds, &enabled);

    SOC_L2X_MEM_UNLOCK(unit);
    return rv;
}


/*
 * Function:
 *	bcm_esw_l2_addr_freeze
 * Description:
 *	Temporarily quiesce ARL from all activity (learning, aging, inserting)
 * include SW operations
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_freeze(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_freeze(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
	return soc_l2x_freeze(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_l2_addr_thaw
 * Description:
 *	Restore normal ARL activity including S/W and H/W activity.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE		Success
 *	BCM_E_INTERNAL		Chip access failure
 */

int
bcm_esw_l2_addr_thaw(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_thaw(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return soc_tr3_l2_thaw(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_arl_hashed)) {
	return soc_l2x_thaw(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *	_bcm_l2_bpdu_init
 * Description:
 *	Initialize all BPDU addresses to recognize the 802.1D
 *      Spanning Tree address on all chips.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 * Notes:
 */

static int
_bcm_l2_bpdu_init(int unit)
{
    int		i, nbpdu;
    sal_mac_addr_t	mac;

    BCM_IF_ERROR_RETURN(bcm_esw_l2_cache_size_get(unit, &nbpdu));

    /* Spanning Tree addr (01:80:c2:00:00:00) used as default entries */
    mac[0] = 0x01;
    mac[1] = 0x80;
    mac[2] = 0xc2;
    mac[3] = mac[4] = mac[5] = 0x00;

    for (i = 0; i < nbpdu; i++) {
	BCM_IF_ERROR_RETURN(soc_bpdu_addr_set(unit, i, mac));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_key_dump
 * Purpose:
 *	Dump the key (VLAN+MAC) portion of a hardware-independent
 *	L2 address for debugging
 * Parameters:
 *	unit - Unit number
 *	pfx - String to print before output
 *	entry - Hardware-independent L2 entry to dump
 *	sfx - String to print after output
 */

int
bcm_esw_l2_key_dump(int unit, char *pfx, bcm_l2_addr_t *entry, char *sfx)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_key_dump(unit, pfx, entry, sfx);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    LOG_CLI((BSL_META_U(unit,
                        "l2: %sVLAN=0x%03x MAC=0x%02x%02x%02x"
             "%02x%02x%02x%s"), pfx, entry->vid,
             entry->mac[0], entry->mac[1], entry->mac[2],
             entry->mac[3], entry->mac[4], entry->mac[5], sfx));

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_l2_conflict_get
 * Purpose:
 *	Given an L2 address, return existing addresses which could conflict.
 * Parameters:
 *	unit		- switch device
 *	addr		- l2 address to search for conflicts
 *	cf_array	- (OUT) list of l2 addresses conflicting with addr
 *	cf_max		- number of entries allocated to cf_array
 *	cf_count	- (OUT) actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *	Given an L2 or L2 multicast address, return any existing L2 or
 *	L2 multicast addresses which might prevent it from being
 *	inserted because a chip resource (like a hash bucket) is full.
 *	This routine could be used if bcm_l2_addr_add or bcm_l2_mcast_add
 *	returns BCM_E_FULL.
 */

int
bcm_esw_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
         bcm_l2_addr_t *cf_array, int cf_max,
         int *cf_count)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_conflict_get(unit, addr, cf_array, cf_max,
                                             cf_count);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);
    /*
     * Call chip-dependent handler.
     */
    SOC_IF_ERROR_RETURN
       (mbcm_driver[unit]->mbcm_l2_conflict_get(unit, addr,
                                       cf_array, cf_max, cf_count));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_l2_tunnel_add
 * Purpose:
 *    Add a (MAC, VLAN) for tunnel/MPLS processing, frames
 *      destined to (MAC, VLAN) is subjected to TUNNEL/MPLS processing.
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
 *      vlan - VLAN ID
 */

int
bcm_esw_l2_tunnel_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_tunnel_add(unit, mac, vlan);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
            return BCM_E_UNAVAIL;
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        VLAN_CHK_ID(unit, vlan);
        return bcm_tr3_l2_tunnel_add(unit, mac, vlan);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

#if defined (BCM_TRIDENT_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
        defined(INCLUDE_L3)
    if (SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) {
        VLAN_CHK_ID(unit, vlan);
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, MY_STATION_TCAMm)) {
            return bcm_td_metro_myStation_add(unit, mac, vlan, -1, 1);
        }
        else {
            return BCM_E_UNAVAIL;
        }
#else
        return bcm_td_metro_myStation_add(unit, mac, vlan, -1, 1);
#endif
    }
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
        defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        VLAN_CHK_ID(unit, vlan);
        return bcm_trx_metro_l2_tunnel_add(unit, mac, vlan);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_tunnel_delete
 * Purpose:
 *      Remove a tunnel processing indicator for an L2 address
 * Parameters:
 *      unit - StrataXGS unit number
 *      mac  - MAC address
      vlan - VLAN ID
 */

int
bcm_esw_l2_tunnel_delete(int unit, bcm_mac_t mac, bcm_vlan_t vlan)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_tunnel_delete(unit, mac, vlan);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
         return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        VLAN_CHK_ID(unit, vlan);
        return bcm_tr3_l2_tunnel_delete(unit, mac, vlan);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

#if defined(BCM_TRIDENT_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) {
        VLAN_CHK_ID(unit, vlan);
        return bcm_td_metro_myStation_delete(unit, mac, vlan, -1, 1);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        VLAN_CHK_ID(unit, vlan);
        return bcm_trx_metro_l2_tunnel_delete(unit, mac, vlan);
    }
#endif
    return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_l2_tunnel_delete_all
 * Purpose:
 *      Remove all tunnel processing indicating L2 addresses
 * Parameters:
 *      unit - StrataXGS unit number
 */

int
bcm_esw_l2_tunnel_delete_all(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_tunnel_delete_all(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

    if (SOC_IS_HURRICANEX(unit) || SOC_IS_GREYHOUND(unit) ||
        SOC_IS_GREYHOUND2(unit)) {
         return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_tunnel_delete_all(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

#if defined(BCM_TRIDENT_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)
    if (SOC_IS_TD_TT(unit)||SOC_IS_KATANAX(unit)) {
        return bcm_td_metro_myStation_delete_all(unit);
    }
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
         defined(INCLUDE_L3)
    if (SOC_IS_TR_VL(unit)) {
        return bcm_trx_metro_l2_tunnel_delete_all(unit);
    }
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_clear
 * Purpose:
 *      Clear the L2 layer
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_l2_clear(int unit)
{
    int         was_running = FALSE;
    uint32      flags;
    sal_usecs_t interval;
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_clear(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_clear(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (_bcm_l2_match_ctrl[unit]) {
        _bcm_l2_match_ctrl[unit]->preserved = 1;
    }

    /* Stop l2x thread before detaching l2. */
    if (soc_l2x_running(unit, &flags, &interval)) {
        was_running = TRUE;
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    bcm_esw_l2_detach(unit);

    if (_bcm_l2_match_ctrl[unit]) {
        _bcm_l2_match_ctrl[unit]->preserved = 0;
    }

    /*
     * Call chip-dependent initialization
     */
    BCM_IF_ERROR_RETURN(mbcm_driver[unit]->mbcm_l2_init(unit));

    /* Restart l2x thread after l2 re-init. */
    if (was_running) {
        interval = (SAL_BOOT_BCMSIM && !SOC_IS_TRIDENT3X(unit)) ? \
                    BCMSIM_L2XMSG_INTERVAL : interval;
        soc_l2x_start(unit, flags, interval);
    }

#if defined(BCM_XGS_SWITCH_SUPPORT)
    /* Stop l2x thread if callback registration started it. */
    if (l2_data[unit].flags & _BCM_L2X_THREAD_STOP) {
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }
#endif /* BCM_XGS_SWITCH_SUPPORT */

    /* Clear l2_data structure */
    l2_data[unit].cb_count = 0;
    l2_data[unit].flags = 0;
    sal_memset(&l2_data[unit].cb, 0, sizeof(l2_data[unit].cb));

    _l2_init[unit] = 1;		/* some positive value */

    return BCM_E_NONE;
}

/*
 * MAC learn limit
 */
#ifdef BCM_TRIUMPH_SUPPORT
/*
 * Function:
 *      _bcm_tr_l2_learn_limit_system_set
 * Purpose:
 *      Set MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_system_set(int unit, uint32 flags, int limit)
{
    uint32 rval, orval;
    int tocpu, drop, enable;
    
    if (limit < 0) {
        tocpu = 0;
        drop = 0;
        limit = soc_mem_index_max(unit, L2Xm);
        enable = 0;
    } else {
        tocpu = flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0;
        drop = flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0;
        enable = 1;
    }

    /* Starting from Triumph this flag is not supported */
    if (flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    orval = rval;
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, 
                      SYS_OVER_LIMIT_TOCPUf, tocpu);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval,
                      SYS_OVER_LIMIT_DROPf, drop);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, SYS_LIMITf, limit);
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, ENABLEf, enable);
    if (rval != orval) {
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_system_get
 * Purpose:
 *      Get MAC learn limit for a system
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask
 *      limit - System limit of MAC addresses to learn
 *              Negative if limit disabled
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_system_get(int unit, uint32 *flags, int *limit)
{
    uint32  rval;

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                          SYS_OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    if (soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval, ENABLEf)) {
        *limit = soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval,
                                   SYS_LIMITf);
    } else {
        *limit = -1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_set
 * Purpose:
 *      Set MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_set(int unit, soc_mem_t mem, int index, uint32 flags,
                           int limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    int rv;
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
    port_or_trunk_mac_limit_entry_t init_entry;
    uint32 max_limit = BCM_MAC_LIMIT_MAX;
#endif /* BCM_ENDURO_SUPPORT || BCM_HURRICANE_SUPPORT */

    if (limit < 0) {
        soc_mem_lock(unit, mem);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                           soc_mem_entry_null(unit, mem));
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANEX(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
            /* Reset Limit value is different than soc_mem_entry_null for different SKUs */
            soc_mem_field32_set(unit, mem, &init_entry, LIMITf, max_limit);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &init_entry);
        }
#endif /* BCM_ENDURO_SUPPORT || BCM_HURRICANE_SUPPORT */
        soc_mem_unlock(unit, mem);
        return rv;
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry);
    if (BCM_SUCCESS(rv)) {
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_TOCPUf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, OVER_LIMIT_DROPf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0);
        soc_mem_field32_set(unit, mem, &entry, LIMITf, limit);
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry);
    }
    soc_mem_unlock(unit, mem);

    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
    soc_reg_field_set(unit, SYS_MAC_LIMIT_CONTROLr, &rval, ENABLEf, 1);
    return (WRITE_SYS_MAC_LIMIT_CONTROLr(unit, rval));
}

/*
 * Function:
 *      _bcm_tr_l2_learn_limit_get
 * Purpose:
 *      Get MAC learn limit for a port/trunk/vlan
 * Parameters:
 *      unit  - BCM unit number
 *      mem   - PORT_OR_TRUNK_MAC_LIMITm or VLAN_OR_VFI_MAC_LIMITm
 *      index - for PORT_OR_TRUNK_MAC_LIMITm
 *                  0 - 127: trunk identifier
 *                  128 - 181: port identifier + 128
 *              for VLAN_OR_VFI_MAC_LIMITm
 *                  0 - 4095: VLAN identifier
 *                  4096 - 5119: virtual forwarding instance + 4096
 *      flags - Action bitmask
 *      limit - Max number of MAC addresses can be learned for the specified
 *              identifier
 *              Negative if limit disabled (i.e. unlimit)
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr_l2_learn_limit_get(int unit, soc_mem_t mem, int index, uint32* flags,
                           int* limit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, index, &entry));
    *flags &= ~(BCM_L2_LEARN_LIMIT_ACTION_DROP |
                BCM_L2_LEARN_LIMIT_ACTION_CPU);
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_DROPf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    if (soc_mem_field32_get(unit, mem, &entry, OVER_LIMIT_TOCPUf)) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }
    *limit = soc_mem_field32_get(unit, mem, &entry, LIMITf);

    return BCM_E_NONE;
}
#endif

#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
/*
 * Function:
 *      _bcm_enduro_l2_learn_limit_init
 * Purpose:
 *      Init the system to support MAC learn limit
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_enduro_l2_learn_limit_init(int unit)
{
    uint32 maxInitVal;
    int rv, idx, idx_min, idx_max;
    uint8 *port_or_trunk_mac_limit_buf = NULL;
    uint8 *vlan_or_vfi_mac_limit_buf = NULL;
    vlan_or_vfi_mac_limit_entry_t   *vlan_or_vfi_mac_limit_entry = NULL;
    port_or_trunk_mac_limit_entry_t *port_or_trunk_mac_limit_entry = NULL;
    maxInitVal = BCM_MAC_LIMIT_MAX; 

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_learn_limit_system_set(unit, 0, -1));
    BCM_IF_ERROR_RETURN
        (soc_mem_clear(unit, PORT_OR_TRUNK_MAC_LIMITm, COPYNO_ALL, FALSE));
    BCM_IF_ERROR_RETURN
        (soc_mem_clear(unit, VLAN_OR_VFI_MAC_LIMITm, COPYNO_ALL, FALSE));

    /* PORT_OR_TRUNK_MAC_LIMIT table*/
    port_or_trunk_mac_limit_buf = soc_cm_salloc(unit,
                   SOC_MEM_TABLE_BYTES(unit,PORT_OR_TRUNK_MAC_LIMITm),
                   "port_or_trunk");
    if (port_or_trunk_mac_limit_buf == NULL) {
        return BCM_E_MEMORY;
    }

    idx_min = soc_mem_index_min(unit, PORT_OR_TRUNK_MAC_LIMITm); 
    idx_max = soc_mem_index_max(unit, PORT_OR_TRUNK_MAC_LIMITm);
    rv = soc_mem_read_range(unit, PORT_OR_TRUNK_MAC_LIMITm,
                            MEM_BLOCK_ANY, idx_min, idx_max,
                            port_or_trunk_mac_limit_buf);
    
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for(idx = idx_min; idx<= idx_max; idx++) {
        port_or_trunk_mac_limit_entry = 
            soc_mem_table_idx_to_pointer(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                        port_or_trunk_mac_limit_entry_t *,
                                        port_or_trunk_mac_limit_buf,
                                        idx);
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit,
                            port_or_trunk_mac_limit_entry,
                            LIMITf, maxInitVal);
    }
    rv = soc_mem_write_range(unit, PORT_OR_TRUNK_MAC_LIMITm, MEM_BLOCK_ALL,
                             idx_min, idx_max, port_or_trunk_mac_limit_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
        
    /* VLAN_OR_VFI_MAC_LIMIT table*/
    vlan_or_vfi_mac_limit_buf = soc_cm_salloc(unit,
                   SOC_MEM_TABLE_BYTES(unit, VLAN_OR_VFI_MAC_LIMITm),
                   "vlan_or_vfi");
    if (vlan_or_vfi_mac_limit_buf == NULL) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
       
    idx_min = soc_mem_index_min(unit, VLAN_OR_VFI_MAC_LIMITm); 
    idx_max = soc_mem_index_max(unit, VLAN_OR_VFI_MAC_LIMITm);
    rv = soc_mem_read_range(unit, VLAN_OR_VFI_MAC_LIMITm,
                            MEM_BLOCK_ANY, idx_min, idx_max,
                            vlan_or_vfi_mac_limit_buf);
    
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for(idx = idx_min; idx<= idx_max; idx++) {
        vlan_or_vfi_mac_limit_entry = 
            soc_mem_table_idx_to_pointer(unit, VLAN_OR_VFI_MAC_LIMITm,
                                        vlan_or_vfi_mac_limit_entry_t *,
                                        vlan_or_vfi_mac_limit_buf,
                                        idx);
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit,
                            vlan_or_vfi_mac_limit_entry,
                            LIMITf, maxInitVal);
    }
    rv = soc_mem_write_range(unit, VLAN_OR_VFI_MAC_LIMITm, MEM_BLOCK_ALL,
                             idx_min, idx_max, vlan_or_vfi_mac_limit_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
        

cleanup:

    if (port_or_trunk_mac_limit_buf) {
        soc_cm_sfree(unit, port_or_trunk_mac_limit_buf);
    }

    if (vlan_or_vfi_mac_limit_buf) {
        soc_cm_sfree(unit, vlan_or_vfi_mac_limit_buf);
    }

    return rv;
}
#endif /* BCM_ENDURO_SUPPORT || BCM_HURRICANE_SUPPORT */

#define PORT_COUNT(unit) (SOC_INFO(unit).port_num)

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
/*
 * Function:
 *      _bcm_l2_learn_limit_porttrunk_set
 * Purpose:
 *      helper function to configure registers
 * Parameters:
 *      unit  - BCM unit number
 *      port_tgid - Port or Trunk number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_l2_learn_limit_porttrunk_set(int unit, int port_tgid, uint32 flags,
                                  int limit)
{
    int     rv = BCM_E_NONE;
    port_or_trunk_mac_limit_entry_t lim_entry;
    port_or_trunk_mac_action_entry_t act_entry;


    if (limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }
    /* Negative limit disables the check */
    if (limit < 0) {
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);

        rv = READ_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ANY, port_tgid, &lim_entry);
        if ( BCM_E_NONE == rv) {
            soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit, &lim_entry, 
                                                     PORT_TRUNK_MAC_LIMITf,
                                                     BCM_MAC_LIMIT_MAX);
            rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                                port_tgid, &lim_entry);
        }
        soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);

        return rv;
    }


    /* If everything is OK - configure the registers */
    soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);
    
    rv = READ_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ANY, port_tgid, &lim_entry);
    if (BCM_E_NONE == rv) {
        soc_PORT_OR_TRUNK_MAC_LIMITm_field32_set(unit, &lim_entry, 
                                                 PORT_TRUNK_MAC_LIMITf,
                                                 limit);
        rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                            port_tgid, &lim_entry);
    }
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);

    BCM_IF_ERROR_RETURN(rv);

    soc_mem_lock(unit, PORT_OR_TRUNK_MAC_ACTIONm);
    
    rv = READ_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ANY, port_tgid, &act_entry);
    if (BCM_E_NONE == rv) {
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_ACTIONm, &act_entry,
                            OVER_LIMIT_DROPf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_DROP ? 1 : 0);
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_ACTIONm, &act_entry,
                            OVER_LIMIT_TOCPUf,
                            flags & BCM_L2_LEARN_LIMIT_ACTION_CPU ? 1 : 0);
        rv = WRITE_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ALL, port_tgid, &act_entry);
    }
    soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_ACTIONm);

    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_porttrunk_get
 * Purpose:
 *      helper function to configure registers
 * Parameters:
 *      unit  - BCM unit number
 *      port_tgid - Port or trunk number 
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_l2_learn_limit_porttrunk_get(int unit, int port_tgid, uint32 *flags,
                                  int *limit)
{
    int     org_action;
    port_or_trunk_mac_limit_entry_t lim_entry;
    port_or_trunk_mac_action_entry_t act_entry;

    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    *flags = 0;

    BCM_IF_ERROR_RETURN
        (READ_PORT_OR_TRUNK_MAC_LIMITm(unit,
                                       MEM_BLOCK_ANY,
                                       port_tgid,
                                       &lim_entry));
    *limit = soc_PORT_OR_TRUNK_MAC_LIMITm_field32_get(unit, &lim_entry, 
                                                 PORT_TRUNK_MAC_LIMITf);
    BCM_IF_ERROR_RETURN
        (READ_PORT_OR_TRUNK_MAC_ACTIONm(unit,
                                        MEM_BLOCK_ANY,
                                        port_tgid,
                                        &act_entry));
    org_action = soc_PORT_OR_TRUNK_MAC_ACTIONm_field32_get(unit, &act_entry,
                                                  OVER_LIMIT_DROPf);
    if (org_action) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
    }
    org_action = soc_PORT_OR_TRUNK_MAC_ACTIONm_field32_get(unit, &act_entry,
                                                  OVER_LIMIT_TOCPUf);
    if (org_action) {
        *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_init
 * Purpose:
 *      Init the system to support MAC learn limit
 * Parameters:
 *      unit  - BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_init(int unit)
{

    bcm_port_t                          port;
    bcm_trunk_t                         trunk;
    bcm_trunk_t                         trunk_index;
    mac_limit_port_map_table_entry_t    entry;
    int                                 rv = BCM_E_NONE;
    uint32                              maxInitVal, zeroInitVal;

    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
#if defined(BCM_ENDURO_SUPPORT) || defined(BCM_HURRICANE_SUPPORT)
        if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANEX(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
            /* Reset Limit value is different than soc_mem_entry_null for different SKUs */
            return _bcm_enduro_l2_learn_limit_init(unit);
        }
#endif /* BCM_ENDURO_SUPPORT || BCM_HURRICANE_SUPPORT */
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            _bcm_tr_l2_learn_limit_system_set(unit, 0, -1);
            BCM_IF_ERROR_RETURN
            (soc_mem_clear(unit, PORT_OR_TRUNK_MAC_LIMITm, COPYNO_ALL, FALSE));
            BCM_IF_ERROR_RETURN
            (soc_mem_clear(unit, VLAN_OR_VFI_MAC_LIMITm, COPYNO_ALL, FALSE));
            return BCM_E_NONE;
        }
#endif
        maxInitVal = BCM_MAC_LIMIT_MAX; 
        zeroInitVal = 0;

        /* Initialize all learning limits to max and disable feature */
        BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, zeroInitVal));
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, maxInitVal));
        /* Map initial port/trunk to counters */
        soc_mem_lock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
        soc_mem_lock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_LIMITm);
        soc_mem_lock(unit, PORT_OR_TRUNK_MAC_ACTIONm);
        PBMP_ALL_ITER(unit, port) {
            /* Initialize the HW tables for port map table*/
            rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL, 
                                                port, &maxInitVal);
            if (BCM_FAILURE(rv)) {
                goto done;
            }

            rv = WRITE_PORT_OR_TRUNK_MAC_ACTIONm(unit, MEM_BLOCK_ALL, 
                                                 port, &zeroInitVal );
            if (BCM_FAILURE(rv)) {
                goto done;
            }

            rv = READ_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                                port, &entry);
            if (BCM_SUCCESS(rv)) {
                soc_MAC_LIMIT_PORT_MAP_TABLEm_field32_set(unit, &entry,
                                                          INDEXf, port);
                rv = WRITE_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                                     port, &entry);
            }
            if (BCM_FAILURE(rv)) {
                goto done;
            }
        }
        /* Initialize the HW tables for trunk map table */
        for (trunk = 0;
             trunk < soc_mem_index_count(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
             trunk ++ ) {
            trunk_index = trunk + PORT_COUNT(unit);
            rv = WRITE_PORT_OR_TRUNK_MAC_LIMITm(unit, MEM_BLOCK_ALL,
                                                trunk_index, &maxInitVal);
            if (BCM_FAILURE(rv)) {
                goto done;
            }
            rv = READ_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL,
                                                 trunk, &entry);
            if (BCM_SUCCESS(rv)) {
                soc_MAC_LIMIT_TRUNK_MAP_TABLEm_field32_set(unit, &entry,
                                                           INDEXf, trunk);
                rv = WRITE_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                                      trunk, &entry);
            }
            if (BCM_FAILURE(rv)) {
                goto done;
            }
        }
done:
        soc_mem_unlock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
        soc_mem_unlock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
        soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_LIMITm);
        soc_mem_unlock(unit, PORT_OR_TRUNK_MAC_ACTIONm);

        return rv;
    } 

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      _bcm_l2_learn_limit_system_set
 * Purpose:
 *      Set MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_system_set(int unit, uint32 flags, int limit)
{
    uint32  limit_enable, limit_action;
    int org_limit = 0;
    
    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    if (limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }

    /* Negative limit disables the check */
    if (limit < 0) {
        BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
        soc_reg_field_set(unit, SYS_MAC_LIMITr, &limit_enable, 
                          SYS_MAC_LIMITf, BCM_MAC_LIMIT_MAX);
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, limit_enable));

        return BCM_E_NONE;
    }

    /* If everything is OK - configure the registers */
    /* First configure the Limit and then enable */
    

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
    org_limit = soc_reg_field_get(unit, SYS_MAC_LIMITr, limit_enable, 
                                  SYS_MAC_LIMITf);
    if (org_limit != limit) {
        soc_reg_field_set(unit, SYS_MAC_LIMITr, &limit_enable, 
                          SYS_MAC_LIMITf, limit);
        BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_LIMITr(unit, limit_enable));
    }

    BCM_IF_ERROR_RETURN(READ_SYS_MAC_ACTIONr(unit, &limit_action));
    /*
     * Need to reset the action bit if the flasg is not set.
     */
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action,
                      OVER_LIMIT_DROPf, 
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_DROP) ? 1 : 0);
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action, 
                      OVER_LIMIT_TOCPUf,
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_CPU) ? 1 : 0);
    soc_reg_field_set(unit,
                      SYS_MAC_ACTIONr,
                      &limit_action, 
                      MAC_LIMIT_USE_SYS_ACTIONf,
                      (flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) ? 1 : 0);

    BCM_IF_ERROR_RETURN(WRITE_SYS_MAC_ACTIONr(unit, limit_action));


    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_system_get
 * Purpose:
 *      Get MAC learn limit for a system 
 * Parameters:
 *      unit  - BCM unit number
 *      flags - Action bitmask 
 *      limit - System limit of MAC addresses to learn
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_system_get(int unit, uint32 *flags, int *limit)
{
       uint32  limit_enable, limit_action;
       int     org_limit, org_action;

       if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
           return BCM_E_UNAVAIL;
       }

       BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
       org_limit = soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, limit_enable, 
                                     ENABLEf);

       *flags = 0;
       if (!org_limit) {
           *limit = -1;
           return BCM_E_NONE;
       }

       BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMITr(unit, &limit_enable));
       *limit = soc_reg_field_get(unit, SYS_MAC_LIMITr, limit_enable, 
                                  SYS_MAC_LIMITf);

       /*
        *  In case of MAC learn limit was disabled but somehow 
        *  enable bit was set 
        */
       if (*limit > BCM_MAC_LIMIT_MAX) {
           *limit = -1 ;
           return BCM_E_NONE;
       }

       BCM_IF_ERROR_RETURN(READ_SYS_MAC_ACTIONr(unit, &limit_action));
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      OVER_LIMIT_DROPf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_DROP;
       }
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      OVER_LIMIT_TOCPUf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_CPU;
       }
       org_action = soc_reg_field_get(unit, SYS_MAC_ACTIONr, limit_action, 
                                      MAC_LIMIT_USE_SYS_ACTIONf);
       if (org_action) {
           *flags |= BCM_L2_LEARN_LIMIT_ACTION_PREFER;
       }

       return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_l2_learn_limit_port_set
 * Purpose:
 *      Set MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      port - Port number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled  
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_port_set(int unit, bcm_port_t port, uint32 flags,
                             int limit)
{
    int                                 rv = BCM_E_NONE;
    mac_limit_port_map_table_entry_t    entry;
    source_trunk_map_table_entry_t      trunk_entry;
    bcm_module_t my_modid;
    int index;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }

    /* Get local module id. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Calculate table index. */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, 
        my_modid, port, &index));

    SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                                     index, &trunk_entry));
    /* Normal port will be encoded as 0 */
    if (soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &trunk_entry,
                                                PORT_TYPEf)) {
        return BCM_E_CONFIG;
    }
    rv = _bcm_l2_learn_limit_porttrunk_set(unit, port, flags, limit);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    soc_mem_lock(unit, MAC_LIMIT_PORT_MAP_TABLEm);
    /* Map port to it's counter */
    rv = READ_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                        port, &entry);
    if (BCM_E_NONE == rv) {
        soc_MAC_LIMIT_PORT_MAP_TABLEm_field32_set(unit, &entry,
                                                  INDEXf, port);
        rv = WRITE_MAC_LIMIT_PORT_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                             port, &entry);
    }
    soc_mem_unlock(unit, MAC_LIMIT_PORT_MAP_TABLEm);

    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_port_get
 * Purpose:
 *      Get MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      port - Port number
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_port_get(int unit, bcm_port_t port, uint32 *flags,
                             int *limit)
{
    int                                 rv = BCM_E_NONE;
    int                                 hw_index;
    source_trunk_map_table_entry_t      trunk_entry;
    bcm_module_t my_modid;
    int index;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }

    /* Get local module id. */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    /* Calculate table index. */
    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, 
        my_modid, port, &index));

    /* If port is part of a trunk  - Error */
    SOC_IF_ERROR_RETURN(READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY,
                                                     index, &trunk_entry));
    /* Normal port will be encoded as 0 */
    if (soc_SOURCE_TRUNK_MAP_TABLEm_field32_get(unit, &trunk_entry,
                                                PORT_TYPEf)) {
        return BCM_E_CONFIG;
    }
    hw_index = port;
    rv = _bcm_l2_learn_limit_porttrunk_get(unit, hw_index, flags, limit);
    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_trunk_set
 * Purpose:
 *      Set MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      trunk - Trunk identifier
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative limit disables the check 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_trunk_set(int unit, bcm_trunk_t trunk, uint32 flags,
                              int limit)
{
    int                                  rv = BCM_E_NONE;
    int                                  hw_index = -1;
    mac_limit_trunk_map_table_entry_t    entry;
    soc_mem_t trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (trunk >= soc_mem_index_count(unit, trunk_mem) || trunk < 0) {
        return BCM_E_PARAM;
    }

    if (0 == soc_feature(unit, soc_feature_mac_learn_limit)) {
        return (BCM_E_UNAVAIL);
    }
    hw_index = trunk + PORT_COUNT(unit);
    rv = _bcm_l2_learn_limit_porttrunk_set(unit, hw_index, flags, limit);
    soc_mem_lock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
    /* Map port to it's counter */
    rv = READ_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                         trunk, &entry);
    if (BCM_E_NONE == rv) {
        soc_MAC_LIMIT_TRUNK_MAP_TABLEm_field32_set(unit, &entry,
                                                   INDEXf, hw_index);
        rv = WRITE_MAC_LIMIT_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ALL, 
                                              trunk, &entry);
    }
    soc_mem_unlock(unit, MAC_LIMIT_TRUNK_MAP_TABLEm);
    return rv;
}

/*
 * Function:
 *      _bcm_l2_learn_limit_trunk_get
 * Purpose:
 *      Get MAC learn limit for a specific port 
 * Parameters:
 *      unit  - BCM unit number
 *      trunk - Trunk identifier
 *      flags - Action bitmask 
 *      limit - Per port limit of MAC addresses to learn 
 *              Negative if limit disabled 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_learn_limit_trunk_get(int unit, bcm_trunk_t tid, uint32 *flags,
                              int *limit)
{
    int rv = BCM_E_NONE;
    int hw_index = -1;
    soc_mem_t trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (tid >= soc_mem_index_count(unit, trunk_mem) || tid < 0) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_mac_learn_limit)) {
        hw_index = tid + PORT_COUNT(unit);
        rv = _bcm_l2_learn_limit_porttrunk_get(unit, hw_index, flags, limit);
        return rv;
    } 

    return BCM_E_UNAVAIL;
}
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRIUMPH_SUPPORT */
/*
 * Function:
 *     bcm_esw_l2_learn_limit_set
 * Description:
 *     Set the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_limit_set(int unit, bcm_l2_learn_limit_t *limit)
{
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    uint32 type, action, limit_enable, org_limit=0;
#if defined(BCM_KATANA2_SUPPORT)
    int             id = -1;
    bcm_port_t      _port = -1;
    bcm_trunk_t     _trunk = -1;
    bcm_module_t    _modid = -1;
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    soc_mem_t trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_learn_limit_set(unit, limit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_limit_set(unit, limit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if ((!soc_feature(unit, soc_feature_mac_learn_limit))) {
        return BCM_E_UNAVAIL;
    }

    if (!limit) {
        return BCM_E_PARAM;
    }

    if (limit->limit > BCM_MAC_LIMIT_MAX){
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);

    action = limit->flags & 
             (BCM_L2_LEARN_LIMIT_ACTION_DROP | BCM_L2_LEARN_LIMIT_ACTION_CPU |
              BCM_L2_LEARN_LIMIT_ACTION_PREFER);

    if (!type) {
        return BCM_E_PARAM;
    }

    if (type != BCM_L2_LEARN_LIMIT_SYSTEM &&
        (action & BCM_L2_LEARN_LIMIT_ACTION_PREFER)) {
        return BCM_E_PARAM;
    }


    if (type & BCM_L2_LEARN_LIMIT_SYSTEM) {
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_system_set(unit, action,
                                                   limit->limit));
        } else
#endif
        if (soc_feature(unit, soc_feature_system_mac_learn_limit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_system_set(unit, action, limit->limit));
        } else {
            return BCM_E_UNAVAIL;
        }
    }

    if (type & BCM_L2_LEARN_LIMIT_PORT) {
        if (BCM_GPORT_IS_SET(limit->port)) {

#if defined(BCM_KATANA2_SUPPORT)
            if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                limit->port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, limit->port, &_modid, &_port,
                               &_trunk, &id));
                BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(
                    unit, _modid, _port, &(limit->port)));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, limit->port, &(limit->port)));
            }
        }

        if (!SOC_PORT_VALID(unit, limit->port)) {
            return BCM_E_PORT;
        }
            
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }

#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            index = limit->port + soc_mem_index_count(unit, trunk_mem);
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                            index, action, limit->limit));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_port_set(unit, limit->port, action,
                                              limit->limit));
        }
    }

    if (type & BCM_L2_LEARN_LIMIT_TRUNK) {
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }

#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (limit->trunk < 0 ||
                limit->trunk >= soc_mem_index_count(unit, trunk_mem)) {
                return BCM_E_PARAM;
            }
            index = limit->trunk;
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                            index, action, limit->limit));
        } else
#endif
        {
            BCM_IF_ERROR_RETURN
                (_bcm_l2_learn_limit_trunk_set(unit, limit->trunk, action,
                                               limit->limit));
        }
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (type & BCM_L2_LEARN_LIMIT_VLAN) {
        /* BCM_L2_LEARN_LIMIT_ACTION_PREFER should be used only in system limit
           and only for Raptor/Raven devices */
        if (limit->flags & BCM_L2_LEARN_LIMIT_ACTION_PREFER) {
            return BCM_E_PARAM;
        }

        if (SOC_IS_TR_VL(unit)) {
            int index, vfi;

            if (_BCM_VPN_VFI_IS_SET(limit->vlan)) {
                _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, limit->vlan);
                if (vfi >= soc_mem_index_count(unit, VFIm)) {
                    return BCM_E_PARAM;
                }
                index = vfi + soc_mem_index_count(unit, VLAN_TABm);
            } else {
                if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                    return BCM_E_PARAM;
                }
                index = limit->vlan;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_tr_l2_learn_limit_set(unit, VLAN_OR_VFI_MAC_LIMITm,
                                            index, action, limit->limit));
        }
    }

    if (SOC_IS_TR_VL(unit)) {
        return BCM_E_NONE;
    }
#endif

    /* Enable the MAC learn limit feature after configuring the indexes */
    /* Negative limit disables the check */
    if (limit->limit < 0) {
        BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
        soc_reg_field_set(unit, MAC_LIMIT_ENABLEr, &limit_enable, 
                          ENABLEf, 0);
        BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, limit_enable));
    } else {
        BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &limit_enable));
        org_limit = soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, limit_enable,
                                      ENABLEf);
        if (!org_limit) {
            soc_reg_field_set(unit, MAC_LIMIT_ENABLEr, &limit_enable, 
                              ENABLEf, 1);
            BCM_IF_ERROR_RETURN(WRITE_MAC_LIMIT_ENABLEr(unit, limit_enable));
        }
    }
    return BCM_E_NONE;
#else /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_limit_get
 * Description:
 *     Get the L2 MAC learning limit
 * Parameters:
 *     unit        device number
 *     limit       learn limit control info
 *                 limit->flags - qualifiers bits and action bits
 *                 limit->vlan - vlan identifier
 *                 limit->port - port number
 *                 limit->trunk - trunk identifier
 *                 limit->limit - max number of learned entry, -1 for unlimit
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_limit_get(int unit, bcm_l2_learn_limit_t *limit)
{
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    int rv;
    uint32 type, action;
    int max;
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int             id = -1;
    bcm_port_t      _port = -1;
    bcm_trunk_t     _trunk = -1;
    bcm_module_t    _modid = -1;
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    soc_mem_t   trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_learn_limit_get(unit, limit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_limit_get(unit, limit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!soc_feature(unit, soc_feature_mac_learn_limit)) {
        return BCM_E_UNAVAIL;
    }

    if (!limit) {
        return BCM_E_PARAM;
    }

    type = limit->flags &
           (BCM_L2_LEARN_LIMIT_SYSTEM | BCM_L2_LEARN_LIMIT_VLAN | 
            BCM_L2_LEARN_LIMIT_PORT | BCM_L2_LEARN_LIMIT_TRUNK);
    action = 0;
    max = 0;

    rv = BCM_E_UNAVAIL;
    switch (type) {
    case BCM_L2_LEARN_LIMIT_SYSTEM:
#ifdef BCM_TRIUMPH_SUPPORT
      if (SOC_IS_TR_VL(unit)) {
            return _bcm_tr_l2_learn_limit_system_get(unit, &limit->flags,
                                                     &limit->limit);
        }
#endif
      if (soc_feature(unit, soc_feature_system_mac_learn_limit)) {
        rv = _bcm_l2_learn_limit_system_get(unit, &action, &max);
      } 
      break;

    case BCM_L2_LEARN_LIMIT_PORT:
        if (BCM_GPORT_IS_SET(limit->port)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
            if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
                (soc_feature(unit, soc_feature_channelized_switching))) &&

                _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, limit->port)) {

                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, limit->port, &_modid, &_port,
                               &_trunk, &id));

                BCM_IF_ERROR_RETURN(_bcmi_coe_subport_physical_port_get(unit,
                                        limit->port, &(limit->port)));
            } else 
#endif
#if defined(BCM_KATANA2_SUPPORT)
            if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
                limit->port)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_gport_resolve(unit, limit->port, &_modid, &_port,
                               &_trunk, &id));
                BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(
                    unit, _modid, _port, &(limit->port)));
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_local_get(unit, limit->port, &(limit->port)));
            }
        }

        if (!SOC_PORT_VALID(unit, limit->port)) {
            return BCM_E_PORT;
        }
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;
            index = limit->port + soc_mem_index_count(unit, trunk_mem);
            return _bcm_tr_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        rv = _bcm_l2_learn_limit_port_get(unit, limit->port, &action, &max);
        break;

    case BCM_L2_LEARN_LIMIT_TRUNK:
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index;

            if (limit->trunk < 0 ||
                limit->trunk >= soc_mem_index_count(unit, trunk_mem)) {
                return BCM_E_PARAM;
            }
            index = limit->trunk;
            return _bcm_tr_l2_learn_limit_get(unit, PORT_OR_TRUNK_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        rv = _bcm_l2_learn_limit_trunk_get(unit, limit->trunk, &action, &max);
        break;

    case BCM_L2_LEARN_LIMIT_VLAN:
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            int index, vfi;

            if (_BCM_VPN_VFI_IS_SET(limit->vlan)) {
                _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, limit->vlan);
                if (vfi >= soc_mem_index_count(unit, VFIm)) {
                    return BCM_E_PARAM;
                }
                index = vfi + soc_mem_index_count(unit, VLAN_TABm);
            } else {
                if (limit->vlan >= soc_mem_index_count(unit, VLAN_TABm)) {
                    return BCM_E_PARAM;
                }
                index = limit->vlan;
            }
            return _bcm_tr_l2_learn_limit_get(unit, VLAN_OR_VFI_MAC_LIMITm,
                                              index, &limit->flags,
                                              &limit->limit);
        }
#endif
        break;

    default:
        return BCM_E_PARAM;
    }

    if (rv == BCM_E_NONE) {
        limit->flags |= action;
        limit->limit = max;
    }
    return rv;
#else /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_class_set
 * Description:
 *     To set the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 * Note:
 *     This feature is disabled by default (see bcmPortControlLearnClassEnable).
 *     By default, each learning class is assigned priority 0 (lowest)
 *     and the attribute, BCM_L2_LEARN_CLASS_MOVE, is not set. By default,
 *     each port is assigned to learning class 0.
 */
int
bcm_esw_l2_learn_class_set(int unit, int lclass, int lclass_prio, uint32 flags)
{
#if defined(BCM_TRX_SUPPORT)
    uint32 data;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_learn_class_set(unit, lclass, lclass_prio,
                                                flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_class_set(unit, lclass, lclass_prio, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_prio < 0 ||
        lclass_prio >= (1 << soc_reg_field_length(unit, CBL_ATTRIBUTEr,
                                                  PORT_LEARNING_PRIORITYf))) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, PORT_LEARNING_PRIORITYf,
                      lclass_prio);
    soc_reg_field_set(unit, CBL_ATTRIBUTEr, &data, ALLOW_MOVE_IN_CLASSf,
                      flags & BCM_L2_LEARN_CLASS_MOVE ? 1 : 0);
    BCM_IF_ERROR_RETURN(WRITE_CBL_ATTRIBUTEr(unit, lclass, data));

    return SOC_E_NONE;
#else /* BCM_TRIUMPH_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRIUMPH_SUPPORT */
}
/*
 * Function:
 *     bcm_esw_l2_learn_class_get
 * Description:
 *     To get the attributes of L2 learning class
 * Parameters:
 *     unit        device number
 *     lclass      learning class id (0 - 3)
 *     lclass_prio learning class priority (0 - 3, 3 is the highest)
 *     flags       flags (BCM_L2_LEARN_CLASS_*)
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_class_get(int unit, int lclass, int *lclass_prio, uint32 *flags)
{
#if defined(BCM_TRX_SUPPORT)
    uint32 data;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_learn_class_get(unit, lclass, lclass_prio,
                                                flags);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_class_get(unit, lclass, lclass_prio, flags);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return (BCM_E_UNAVAIL);
    }

    if (lclass < 0 || lclass >= SOC_REG_NUMELS(unit, CBL_ATTRIBUTEr) ||
        lclass_prio == NULL || flags == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_CBL_ATTRIBUTEr(unit, lclass, &data));
    *lclass_prio = soc_reg_field_get(unit, CBL_ATTRIBUTEr, data,
                                     PORT_LEARNING_PRIORITYf);
    *flags = 0;
    if (soc_reg_field_get(unit, CBL_ATTRIBUTEr, data, ALLOW_MOVE_IN_CLASSf)) {
        *flags |= BCM_L2_LEARN_CLASS_MOVE;
    }

    return (BCM_E_NONE);
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_port_class_set
 * Description:
 *     To set the L2 learning class of a port
 * Parameters:
 *     unit        device number
 *     port        Ingress generic port
 *     lclass      learning class id (0 - 3)
 * Return:
 *     BCM_E_XXX
 * Note:
 *     This feature is disabled by default (see bcmPortControlLearnClassEnable).
 *     By default, each port is assigned to learning class 0.
 */
int
bcm_esw_l2_learn_port_class_set(int unit, bcm_gport_t port, int lclass)
{
#if defined(BCM_TRX_SUPPORT)
    bcm_module_t module;
    bcm_port_t local_port;
    bcm_trunk_t trunk;
    int id;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling this function directly.
     * Class based learning is not supported on TH3
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_port_class_set(unit, port, lclass);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return BCM_E_UNAVAIL;
    }

    BCM_L2_LEARN_CLASSID_VALID(lclass);

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &module,
                                               &local_port, &trunk, &id));

    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)  ||
        soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            id = -1;
        }
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            id = -1;
        }
#endif
    }
    if (id != -1) {
        if (!BCM_GPORT_IS_MPLS_PORT(port) &&
            !BCM_GPORT_IS_SUBPORT_GROUP(port) &&
            !BCM_GPORT_IS_SUBPORT_PORT(port) &&
            !BCM_GPORT_IS_MIM_PORT(port) &&
            !BCM_GPORT_IS_VLAN_PORT(port) &&
            !BCM_GPORT_IS_WLAN_PORT(port) &&
            !BCM_GPORT_IS_TRILL_PORT(port) &&
            !BCM_GPORT_IS_NIV_PORT(port) &&
            !BCM_GPORT_IS_L2GRE_PORT(port) &&
            !BCM_GPORT_IS_VXLAN_PORT(port) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port)) {
            return BCM_E_PARAM;
        }
        return soc_mem_field32_modify(unit, PORT_CBL_TABLEm, id,
                                      VIRTUAL_PORT_LEARNING_CLASSf, lclass);
    } else if (trunk != -1) {
        return soc_mem_field32_modify(unit, TRUNK_CBL_TABLEm, trunk,
                                      PORT_LEARNING_CLASSf, lclass);
    } else if (!soc_mem_is_valid(unit, PORT_CBL_TABLE_MODBASEm)) {
        return soc_mem_field32_modify(unit, PORT_CBL_TABLEm,
                                      module * (SOC_PORT_ADDR_MAX(unit) + 1) +
                                      local_port,
                                      PORT_LEARNING_CLASSf, lclass);
    }
#ifdef BCM_TRIDENT_SUPPORT
    else {
        soc_profile_mem_t *profile;
        port_cbl_table_modbase_entry_t base_entry;
        port_cbl_table_entry_t cbl_entries[256];
        void *entries[1];
        uint32 old_index, index;
        int rv;

        profile = _bcm_l2_port_cbl_profile[unit];
        entries[0] = &cbl_entries;

        soc_mem_lock(unit, PORT_CBL_TABLE_MODBASEm);

        rv = soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm, MEM_BLOCK_ALL, module,
                          &base_entry);
        if (SOC_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        old_index = soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm,
                                        &base_entry, BASEf);
        rv = soc_profile_mem_get(unit, profile, old_index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        soc_mem_field32_set(unit, PORT_CBL_TABLEm, &cbl_entries[local_port],
                            PORT_LEARNING_CLASSf, lclass);
        rv = soc_profile_mem_add(unit, profile, entries,
                                 SOC_PORT_ADDR_MAX(unit) + 1, &index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        rv = soc_mem_field32_modify(unit, PORT_CBL_TABLE_MODBASEm, module,
                                    BASEf, index);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);
            return rv;
        }
        rv = soc_profile_mem_delete(unit, profile, old_index);

        soc_mem_unlock(unit, PORT_CBL_TABLE_MODBASEm);

        return rv;
    }
#endif /* BCM_TRIDENT_SUPPORT */
    return BCM_E_NONE;
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

/*
 * Function:
 *     bcm_esw_l2_learn_port_class_get
 * Description:
 *     To get the L2 learning class of a port
 * Parameters:
 *     unit        device number
 *     port        Ingress generic port
 *     lclass      learning class id (0 - 3)
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_learn_port_class_get(int unit, bcm_gport_t port, int *lclass)
{
#if defined(BCM_TRX_SUPPORT)
    bcm_module_t module;
    bcm_port_t local_port;
    bcm_trunk_t trunk;
    int id;
    port_cbl_table_entry_t port_entry;
    trunk_cbl_table_entry_t trunk_entry;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling this function directly. Class based
     * learning is not supported on TH3
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_learn_port_class_get(unit, port, lclass);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!soc_feature(unit, soc_feature_class_based_learning)) {
        return BCM_E_UNAVAIL;
    }

    if (lclass == NULL) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &module,
                                               &local_port, &trunk, &id));

    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)  ||
        soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        soc_feature(unit, soc_feature_channelized_switching)) {

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            id = -1;
        }
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            id = -1;
        }
#endif
    }
    if (id != -1) {
        if (!BCM_GPORT_IS_MPLS_PORT(port) &&
            !BCM_GPORT_IS_SUBPORT_GROUP(port) &&
            !BCM_GPORT_IS_SUBPORT_PORT(port) &&
            !BCM_GPORT_IS_MIM_PORT(port) &&
            !BCM_GPORT_IS_VLAN_PORT(port) &&
            !BCM_GPORT_IS_WLAN_PORT(port) &&
            !BCM_GPORT_IS_TRILL_PORT(port) &&
            !BCM_GPORT_IS_NIV_PORT(port) &&
            !BCM_GPORT_IS_L2GRE_PORT(port) &&
            !BCM_GPORT_IS_VXLAN_PORT(port) &&
            !BCM_GPORT_IS_EXTENDER_PORT(port)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL, id, &port_entry));
        *lclass = soc_mem_field32_get(unit, PORT_CBL_TABLEm, &port_entry,
                                      VIRTUAL_PORT_LEARNING_CLASSf);
    } else if (trunk != -1) {
        BCM_IF_ERROR_RETURN
            (READ_TRUNK_CBL_TABLEm(unit, MEM_BLOCK_ALL, trunk, &trunk_entry));
        *lclass = soc_mem_field32_get(unit, TRUNK_CBL_TABLEm, &trunk_entry,
                                      PORT_LEARNING_CLASSf);
    } else if (!soc_mem_is_valid(unit, PORT_CBL_TABLE_MODBASEm)) {
        BCM_IF_ERROR_RETURN
            (READ_PORT_CBL_TABLEm(unit, MEM_BLOCK_ALL,
                                  module * (SOC_PORT_ADDR_MAX(unit) + 1) +
                                  local_port, &port_entry));
        *lclass = soc_mem_field32_get(unit, PORT_CBL_TABLEm, &port_entry,
                                      PORT_LEARNING_CLASSf);
    }
#ifdef BCM_TRIDENT_SUPPORT
    else {
        soc_profile_mem_t *profile;
        port_cbl_table_modbase_entry_t base_entry;
        port_cbl_table_entry_t cbl_entries[256];
        void *entries[1];
        uint32 index;

        profile = _bcm_l2_port_cbl_profile[unit];
        entries[0] = &cbl_entries;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, PORT_CBL_TABLE_MODBASEm, MEM_BLOCK_ALL, module,
                          &base_entry));
        index = soc_mem_field32_get(unit, PORT_CBL_TABLE_MODBASEm, &base_entry,
                                    BASEf);

        BCM_IF_ERROR_RETURN
            (soc_profile_mem_get(unit, profile, index,
                                 SOC_PORT_ADDR_MAX(unit) + 1, entries));
        *lclass = soc_mem_field32_get(unit, PORT_CBL_TABLEm,
                                      &cbl_entries[local_port],
                                      PORT_LEARNING_CLASSf);
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_NONE;
#else /* BCM_TRX_SUPPORT */
    return (BCM_E_UNAVAIL);
#endif /* BCM_TRX_SUPPORT */
}

#ifdef BCM_HURRICANE3_SUPPORT
/*
 * Function:
 *     _bcm_esw_l2_learn_stat_set
 * Description:
 *     To set or clear the specified L2 hash counter to the indicated value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (IN) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_esw_l2_learn_stat_set(int unit, bcm_l2_learn_stat_t *learn_stat)
{
    uint32 val=0;
    soc_field_t learn_field;
    soc_reg_t learn_reg;

    if (learn_stat == NULL) {
        return (BCM_E_PARAM);
    }

    switch(learn_stat->type) {
        case bcmL2LearnStatL2TableLearned:
            if (SOC_REG_IS_VALID(unit, L2_ENTRY_LEARNED_CNTr)) {
                learn_reg = L2_ENTRY_LEARNED_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        case bcmL2LearnStatOverflowPoolLearned:
            if (SOC_REG_IS_VALID(unit, L2_128_OVF_LEARNED_CNTr)) {
                learn_reg = L2_128_OVF_LEARNED_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        case bcmL2LearnStatHashCollisions:
            if (SOC_REG_IS_VALID(unit, L2_ENTRY_COLLISION_CNTr)) {
                learn_reg = L2_ENTRY_COLLISION_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        default:
            return (BCM_E_PARAM);
    }

     BCM_IF_ERROR_RETURN(soc_reg32_get
         (unit, learn_reg, REG_PORT_ANY, 0, &val));
     soc_reg_field_set(unit, learn_reg, &val, learn_field, learn_stat->value);
     BCM_IF_ERROR_RETURN(soc_reg32_set
         (unit, learn_reg, REG_PORT_ANY, 0, val));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_l2_learn_stat_get
 * Description:
 *     To get the statistic of the specified L2 hash counter from chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (INOUT) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_esw_l2_learn_stat_get(int unit, bcm_l2_learn_stat_t *learn_stat)
{
    uint32 val=0;
    soc_field_t learn_field;
    soc_reg_t learn_reg;

    if (learn_stat == NULL) {
        return (BCM_E_PARAM);
    }

    switch(learn_stat->type) {
        case bcmL2LearnStatL2TableLearned:
            if (SOC_REG_IS_VALID(unit, L2_ENTRY_LEARNED_CNTr)) {
                learn_reg = L2_ENTRY_LEARNED_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        case bcmL2LearnStatOverflowPoolLearned:
            if (SOC_REG_IS_VALID(unit, L2_128_OVF_LEARNED_CNTr)) {
                learn_reg = L2_128_OVF_LEARNED_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        case bcmL2LearnStatHashCollisions:
            if (SOC_REG_IS_VALID(unit, L2_ENTRY_COLLISION_CNTr)) {
                learn_reg = L2_ENTRY_COLLISION_CNTr;
                learn_field = COUNTERf;
            } else {
                return (BCM_E_UNAVAIL);
            }
            break;
        default:
            return (BCM_E_PARAM);
    }

     BCM_IF_ERROR_RETURN(soc_reg32_get
         (unit, learn_reg, REG_PORT_ANY, 0, &val));
     learn_stat->value = soc_reg_field_get(unit, learn_reg, val, learn_field);

    return (BCM_E_NONE);
}
#endif /* BCM_HURRICANE3_SUPPORT */

/*
 * Function:
 *     bcm_esw_l2_learn_stat_set
 * Description:
 *     To set or clear the specified L2 hash counter to the indicated value.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (IN) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 * Note:
 *     The purposes of these counters are used to monitor the L2 hash condition.
 *     When the counter counts to 32'hffff_ffff (maximum value), it will keep this value till
 *     software clear it. Currently these L2 hash counters are system-wise.
 */
int
bcm_esw_l2_learn_stat_set(int unit, bcm_l2_learn_stat_t *learn_stat)
{
#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_l2_learn_stats)) {
        return _bcm_esw_l2_learn_stat_set(unit, learn_stat);
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     bcm_esw_l2_learn_stat_get
 * Description:
 *     To get the statistic of the specified L2 hash counter from chip.
 * Parameters:
 *      unit - (IN) Unit number.
 *      learn_stat - (INOUT) Pointer to the L2 learned statistic structure.
 * Return:
 *     BCM_E_XXX
 * Note:
 *     The purposes of these counters are used to monitor the L2 hash condition.
 *     When the counter counts to 32'hffff_ffff (maximum value), it will keep this value till
 *     software clear it. Currently these L2 hash counters are system-wise.
 */
int
bcm_esw_l2_learn_stat_get(int unit, bcm_l2_learn_stat_t *learn_stat)
{
#ifdef BCM_HURRICANE3_SUPPORT
    if (soc_feature(unit, soc_feature_l2_learn_stats)) {
        return _bcm_esw_l2_learn_stat_get(unit, learn_stat);
    }
#endif /* BCM_HURRICANE3_SUPPORT */

    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *     _bcm_esw_l2_entry_valid
 * Description:
 *      Check if given L2 entry is Valid
 * Parameters:
 *      unit         device number
 *      mem         L2 memory to operate on
 *      l2_entry    L2X entry read from HW
 * Return:
 *     BCM_E_NONE
 *     BCM_E_NOT_FOUND
 */
STATIC int 
_bcm_esw_l2_entry_valid(int unit, soc_memacc_t *l2x_memacc, uint32 *l2_entry)
{
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)){
        int valid;
        if (soc_feature(unit, soc_feature_base_valid)) {
            valid = soc_mem_field32_get(unit, L2Xm, l2_entry, BASE_VALIDf);
        } else {
            valid = soc_memacc_field32_get(&l2x_memacc[_BCM_L2_MEMACC_VALID],
                                           l2_entry);
        }
        if (!valid) {
            return BCM_E_NOT_FOUND;
        }
    }
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        int key_type;
        key_type = soc_memacc_field32_get(&l2x_memacc[_BCM_L2_MEMACC_KEY_TYPE],
                                          l2_entry);
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TD2_TT2(unit)) {
            if (key_type != TD2_L2_HASH_KEY_TYPE_BRIDGE &&
                key_type != TD2_L2_HASH_KEY_TYPE_VFI) {
                return BCM_E_NOT_FOUND;
            }
        } else
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_lite_riot) ||
            SOC_IS_FIRELIGHT(unit)) {
            if ((key_type != GH2_L2_HASH_KEY_TYPE_BRIDGE) &&
                (key_type != GH2_L2_HASH_KEY_TYPE_VFI) &&
                (key_type != GH2_L2_HASH_KEY_TYPE_VFI_MULTICAST)) {
                return BCM_E_NOT_FOUND;
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT */

            /* Suppress coverity warning, because enum values used here
             * for key_type are defined differently based on device type
             */

            /* coverity[mixed_enums] */
            if (key_type != TR_L2_HASH_KEY_TYPE_BRIDGE &&
            key_type != TR_L2_HASH_KEY_TYPE_VFI) {
            return BCM_E_NOT_FOUND;
        }
    }
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_esw_l2_from_l2x
 * Description:
 *      Parser function to parse L2X entry into L2_Addr structure
 *      will call per chip parser
 * Parameters:
 *      unit         device number
 *      mem         L2 memory to operate on
 *      l2addr      L2 address structure to fill
 *      l2_entry    L2X entry read from HW
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_l2_from_l2x(int unit, soc_mem_t mem, bcm_l2_addr_t *l2addr, uint32 *l2_entry)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        if (EXT_L2_ENTRYm == mem) { 
            return _bcm_tr_l2_from_ext_l2(unit, l2addr, 
                                        (ext_l2_entry_entry_t *)l2_entry);
        } else { 
            return _bcm_tr_l2_from_l2x(unit, l2addr, 
                                       (l2x_entry_t *)l2_entry);
        }
    } else
#endif 
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_l2_from_l2x(unit, l2addr, 
                                   (l2x_entry_t *)l2_entry);
    } else
#endif

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_esw_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_esw_l2_traverse to itterate over given memory 
 *      and actually read the table and parse entries.
 * Parameters:
 *     unit         device number
 *      mem         L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_esw_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_l2_traverse_t *trav_st)
{
    /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int             buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32          *l2_tbl_chnk;
    uint32          *l2_entry;
    int             rv = BCM_E_NONE;
    int             i;
    soc_memacc_t    l2x_memacc_list[_BCM_L2_MEMACC_NUM];
    soc_field_t     l2x_fields[_BCM_L2_MEMACC_NUM] = {
        VALIDf,
        KEY_TYPEf
    };
    _bcm_l2_replace_t *rep_st;
    
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    for (i = 0; i < _BCM_L2_MEMACC_NUM; i++) {
        rv = soc_memacc_init(unit, mem, l2x_fields[i], &l2x_memacc_list[i]);
        if (BCM_FAILURE(rv)) {
            SOC_MEMACC_INVALID_SET(&l2x_memacc_list[i]);
        }
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_tbl_chnk = soc_cm_salloc(unit, buf_size, "l2 traverse");
    if (NULL == l2_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
#if defined(BCM_TRIDENT2_SUPPORT)
        if (SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)
            || SOC_IS_TD2P_TT2P(unit)) {
            rv = _bcm_td2_l2_hit_range_retrieve(unit, mem, chnk_idx, 
                                                chnk_idx_max, l2_tbl_chnk);
            if (SOC_FAILURE(rv)) {
                break;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = 
                soc_mem_table_idx_to_pointer(unit, mem, uint32 *, 
                                             l2_tbl_chnk, ent_idx);
            if (BCM_FAILURE(_bcm_esw_l2_entry_valid(unit, l2x_memacc_list,
                                                    l2_entry))) {
                continue;
            }
            trav_st->data = l2_entry;
            trav_st->mem = mem;
            trav_st->mem_idx = chnk_idx + ent_idx;
            rv = trav_st->int_cb(unit, (void *)trav_st);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }

        if (!trav_st->user_cb) {
            /* Enhancement: use memory write range (per chunk) for replacing l2 entry data */
            rep_st = ((_bcm_l2_traverse_t *)trav_st)->user_data;
            if (mem == L2Xm && !(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
                rv = soc_mem_write_range(unit, mem, MEM_BLOCK_ANY,
                                         chnk_idx, chnk_idx_max, l2_tbl_chnk);
                if (SOC_FAILURE(rv)) {
                    break;
                }
            }
        }
    }
    soc_cm_sfree(unit, l2_tbl_chnk);
    return rv;        
}

/*
 * Function:
 *     _bcm_esw_l2_traverse
 * Description:
 *      Helper function to bcm_esw_l2_traverse to itterate over table 
 *      and actually read the momery
 * Parameters:
 *     unit         device number
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int 
_bcm_esw_l2_traverse(int unit, _bcm_l2_traverse_t *trav_st)
{
    int rv = BCM_E_UNAVAIL; 

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        rv = _bcm_esw_l2_traverse_mem(unit, L2Xm, trav_st);
#if defined(BCM_TRIUMPH_SUPPORT)
        if (soc_feature(unit, soc_feature_esm_support) && BCM_SUCCESS(rv)) {
            rv = _bcm_tr_l2_traverse_mem(unit, EXT_L2_ENTRYm, trav_st);
        }
#endif  /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_HURRICANE3_SUPPORT)
        if (soc_feature(unit, soc_feature_l2_overflow_bucket) && BCM_SUCCESS(rv)) {
            rv = _bcm_esw_l2_traverse_mem(unit, L2_ENTRY_OVERFLOWm, trav_st);
        }
#endif  /* BCM_HURRICANE3_SUPPORT */

    }
#endif  /* BCM_FBX_SUPPORT */
    return rv;
}

/*
 * Function:
 *     _bcm_esw_l2_traverse_int_cb
 * Description:
 *      Simple internal callback function for bcm_l2_traverse API just to call
 *      a user provided callback on a given entry.
 * Parameters:
 *      unit         device number
 *      trav_st      traverse structure
 * Return:
 *     BCM_E_XXX
 */
int
_bcm_esw_l2_traverse_int_cb(int unit, void *trav_st)
{
    _bcm_l2_traverse_t *trv;
    bcm_l2_addr_t      l2_addr; 

    trv = (_bcm_l2_traverse_t *)trav_st;

    BCM_IF_ERROR_RETURN(
        _bcm_esw_l2_from_l2x(unit, trv->mem, &l2_addr, trv->data));

    return trv->user_cb(unit, &l2_addr, trv->user_data);
}

/*
 * Function:
 *     bcm_esw_l2_traverse
 * Description:
 *     To traverse the L2 table and call provided callback function with matched entry
 * Parameters:
 *     unit         device number
 *     trav_fn      User specified callback function 
 *     user_data    User specified cookie
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_esw_l2_traverse(int unit, bcm_l2_traverse_cb trav_fn, void *user_data)
{
    _bcm_l2_traverse_t  trav_st;
    
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_traverse(unit, trav_fn, user_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));
    
    trav_st.user_cb = trav_fn;
    trav_st.user_data = user_data;
    trav_st.int_cb = _bcm_esw_l2_traverse_int_cb;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_traverse(unit, &trav_st);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    return (_bcm_esw_l2_traverse(unit, &trav_st));
}

int 
_bcm_esw_l2_match_sw(int unit, bcm_l2_addr_t *info, void *user_data)
{
    int flag_match, vid_match, mac_match, dest_match;
    _bcm_l2_match_t *l2match = user_data;

    flag_match = ((info->flags & l2match->addr_mask->flags) ==
                  (l2match->match_addr->flags & l2match->addr_mask->flags));

    vid_match = ((info->vid & l2match->addr_mask->vid) ==
                  (l2match->match_addr->vid & l2match->addr_mask->vid));

    {
        uint8 *s = l2match->match_addr->mac, *d = info->mac;
        uint8 *m = l2match->addr_mask->mac;

        mac_match = (((( s[0] & m[0] ) ^ (d[0] & m[0])) |
                      (( s[1] & m[1] ) ^ (d[1] & m[1])) |
                      (( s[2] & m[2] ) ^ (d[2] & m[2])) |
                      (( s[3] & m[3] ) ^ (d[3] & m[3])) |
                      (( s[4] & m[4] ) ^ (d[4] & m[4])) |
                      (( s[5] & m[5] ) ^ (d[5] & m[5]))) == 0);
    }

    {
        if (BCM_MAC_IS_MCAST(info->mac)) {
            dest_match = ((info->l2mc_group & l2match->addr_mask->l2mc_group) ==
                          (l2match->match_addr->l2mc_group & l2match->addr_mask->l2mc_group));
        } else if (info->flags & BCM_L2_TRUNK_MEMBER) {
            dest_match = ((info->tgid & l2match->addr_mask->tgid) ==
                          (l2match->match_addr->tgid & l2match->addr_mask->tgid));
        } else if (BCM_GPORT_IS_SET(info->port)) {
            dest_match = ((info->port & l2match->addr_mask->port) ==
                          (l2match->match_addr->port & l2match->addr_mask->port));
        } else {
            dest_match = ((info->port & l2match->addr_mask->port) ==
                          (l2match->match_addr->port & l2match->addr_mask->port));
            dest_match &= ((info->modid & l2match->addr_mask->modid) ==
                          (l2match->match_addr->modid & l2match->addr_mask->modid));
        }
    }

    if (flag_match && vid_match && mac_match && dest_match) {
        l2match->trav_fn(unit, info, l2match->user_data);
    }

    return 0;
}

int
_bcm_esw_l2_matched_traverse(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                             bcm_l2_traverse_cb trav_fn, void *user_data)
{
    bcm_l2_addr_t addr_mask;
    bcm_mac_t mac_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    _bcm_l2_match_t l2match;

    if (!trav_fn) {
        return (BCM_E_PARAM);
    }

    if (!match_addr) {
        return (BCM_E_PARAM);
    }

    bcm_l2_addr_t_init(&addr_mask, mac_mask, 0xFFF);

    if (flags & BCM_L2_TRAVERSE_MATCH_STATIC) {
        addr_mask.flags |= BCM_L2_STATIC;
    } else {
        addr_mask.flags &= ~BCM_L2_STATIC;
    }

    if (flags & BCM_L2_TRAVERSE_MATCH_MAC) {
        sal_memset(addr_mask.mac, 0xFF, sizeof(addr_mask.mac));
    } else {
        sal_memset(addr_mask.mac, 0x0, sizeof(addr_mask.mac));
    }

    if (flags & BCM_L2_TRAVERSE_MATCH_VLAN) {
        addr_mask.vid = ~0;
    } else {
        addr_mask.vid = 0;
    }

    if (flags & BCM_L2_TRAVERSE_MATCH_DEST) {
        addr_mask.port = ~0;
        addr_mask.modid = ~0;
        addr_mask.l2mc_group = ~0;
        addr_mask.tgid = ~0;
    } else {
        addr_mask.port = 0x0;
        addr_mask.modid = 0x0;
        addr_mask.l2mc_group = 0x0;
        addr_mask.tgid = 0x0;
    }

    if (flags & BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC) {
        addr_mask.flags &= ~BCM_L2_DISCARD_SRC;
    } else {
        addr_mask.flags |= BCM_L2_DISCARD_SRC;
    }

    if (flags & BCM_L2_TRAVERSE_IGNORE_DES_HIT) {
        addr_mask.flags &= ~BCM_L2_DES_HIT;
    } else {
        addr_mask.flags |= BCM_L2_DES_HIT;
    }

    l2match.match_addr = match_addr;
    l2match.addr_mask = &addr_mask;
    l2match.trav_fn =trav_fn;
    l2match.user_data = user_data;

    return bcm_esw_l2_traverse(unit, _bcm_esw_l2_match_sw, &l2match);
}

#ifdef BCM_XGS_SWITCH_SUPPORT
/*
 * Function:
 *     _bcm_get_op_from_flags
 * Description:
 *     Helper function to _bcm_fb_age_reg_config to 
 *     decide on a PPA command according to flags
 * Parameters:
 *     flags        flags BCM_L2_REPLACE_* 
 *     op           PPA command to be programmed
 *     sync_op      Search Key for _soc_l2x_sync_delete_by
 * Return:
 *     BCM_E_NONE   - Success
 *     BCM_E_PARAM  - Failure
 */
int 
_bcm_get_op_from_flags(uint32 flags, uint32 *op, uint32 *sync_op)
{
    uint32 cmp_flags = 0, int_op = 0, int_sync_op = 0;
    int    rv = BCM_E_NONE;

    if (NULL == op) {
        return BCM_E_PARAM;
    }
    cmp_flags = flags & (BCM_L2_REPLACE_MATCH_VLAN |
                         BCM_L2_REPLACE_MATCH_DEST);
    switch (cmp_flags) {
        case BCM_L2_REPLACE_MATCH_VLAN: 
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_VLAN : 
                   XGS_PPAMODE_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_VLAN_DEL : 
                           SOC_L2X_NO_DEL);
         break;
        }
        case BCM_L2_REPLACE_MATCH_DEST :
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD :
                   XGS_PPAMODE_PORTMOD_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_DEL : 
                           SOC_L2X_NO_DEL);
            break;
        }
        case BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST:
        {
            int_op = (flags & BCM_L2_REPLACE_DELETE ? XGS_PPAMODE_PORTMOD_VLAN :
                   XGS_PPAMODE_PORTMOD_VLAN_REPLACE);
            int_sync_op = (flags & BCM_L2_REPLACE_DELETE ? SOC_L2X_PORTMOD_VLAN_DEL : 
                           SOC_L2X_NO_DEL);

            break;
        }
        default:
            rv = BCM_E_PARAM;
            break;
    }

    *op = int_op;
    *sync_op = int_sync_op;
    return rv;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */


/*
 * Function:
 *     _bcm_l2_replace_limit_count_update
 * Description:
 *     Helper function to bcm_l2_replace API to update mac learn limit counters
 * Parameters:
 *      unit            device number
 *      mem             memory for wich to update counters
 *      rep_st          structure with information of replaced and new params
 *      entry           The entry that is being updated.
 * Return:
 *     BCM_E_NONE   -   OK 
 *     BCM_E_XXX    -   Error
 */
STATIC int 
_bcm_l2_replace_limit_count_update(int unit, soc_mem_t mem, 
                                   _bcm_l2_replace_t *rep_st, 
                                   uint32 *entry)
{
    int     index; 
    uint32  rval, cnt_val, port_val, mac_cnt_entry;
    soc_mem_t trunk_mem = TRUNK_GROUPm;

    if (soc_feature(unit, soc_feature_fastlag)) {
        trunk_mem = FAST_TRUNK_GROUPm;
    }

    /* If MAC learn limit not supported do nothing */
    if ((!soc_feature(unit, soc_feature_mac_learn_limit))) {
        return BCM_E_NONE;
    }

    /* If MAC learn limit not enabled do nothing */
    if (SOC_REG_IS_VALID(unit, MAC_LIMIT_ENABLEr)) {
        BCM_IF_ERROR_RETURN(READ_MAC_LIMIT_ENABLEr(unit, &rval));
        if (!soc_reg_field_get(unit, MAC_LIMIT_ENABLEr, rval, ENABLEf)) {
            return BCM_E_NONE;
        }
    } else if (SOC_REG_IS_VALID(unit, SYS_MAC_LIMIT_CONTROLr)) {
       BCM_IF_ERROR_RETURN(READ_SYS_MAC_LIMIT_CONTROLr(unit, &rval));
       if (!soc_reg_field_get(unit, SYS_MAC_LIMIT_CONTROLr, rval, ENABLEf)) {
           return BCM_E_NONE;
       }
    }
    
    /* If entry marked as EXEMPT do nothing */
    if (soc_mem_field_valid(unit, mem, LIMIT_COUNTEDf)) {
        if (!soc_mem_field32_get(unit,mem, entry, LIMIT_COUNTEDf)) {
            return BCM_E_NONE;
        }
    }

    if (soc_mem_field_valid(unit, mem, TGID_PORTf)) { /* Draco Style */
        port_val = soc_mem_field32_get(unit, mem, entry, TGID_PORTf);
    } else { /* Triumph style */
        port_val = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
    }
    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
        if (soc_mem_field32_get(unit, mem, entry, Tf)) {
            /* If trunk member - do nothing */
            index = port_val;
        } else {
            index = port_val + soc_mem_index_count(unit, trunk_mem);
        }
    } else {
        if (port_val & BCM_TGID_TRUNK_INDICATOR(unit)) {
            /* If trunk member - do nothing */ 
            index = port_val;
        } else {
            index = port_val + soc_mem_index_count(unit, trunk_mem);
        }
    }

    /* If match_dest contains trunk info it is there where we should do the decrement */
    if ((rep_st->flags & BCM_L2_REPLACE_MATCH_DEST)) {
        if (rep_st->match_dest.trunk != -1) {
            index = rep_st->match_dest.trunk;
        } else {
            index = rep_st->match_dest.port +
                soc_mem_index_count(unit, trunk_mem);
        }
    }

    BCM_IF_ERROR_RETURN(
        READ_PORT_OR_TRUNK_MAC_COUNTm(unit, MEM_BLOCK_ANY, index, 
                                      &mac_cnt_entry));
    cnt_val = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm, 
                                  &mac_cnt_entry, COUNTf);
    if ( 0 != cnt_val) {
        cnt_val--;
    }
    soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm, &mac_cnt_entry, 
                        COUNTf, cnt_val);
    BCM_IF_ERROR_RETURN(
        WRITE_PORT_OR_TRUNK_MAC_COUNTm(unit, MEM_BLOCK_ALL, index,
                                       &mac_cnt_entry));

    /* For replace cases a new index should be incremented. */
    if (!(rep_st->flags & BCM_L2_REPLACE_DELETE)) {
        if (rep_st->new_dest.trunk != -1) {
            index = rep_st->new_dest.trunk;
        } else {
            index = rep_st->new_dest.port +
                soc_mem_index_count(unit, trunk_mem);
        }
        BCM_IF_ERROR_RETURN(
        READ_PORT_OR_TRUNK_MAC_COUNTm(unit, MEM_BLOCK_ANY, index, 
                                          &mac_cnt_entry));
        cnt_val = soc_mem_field32_get(unit, PORT_OR_TRUNK_MAC_COUNTm, 
                                      &mac_cnt_entry, COUNTf);
        if ((++cnt_val) > 0x3FFF) {
            cnt_val = 0x3FFF;
        }
        soc_mem_field32_set(unit, PORT_OR_TRUNK_MAC_COUNTm, &mac_cnt_entry, 
                            COUNTf, cnt_val);
        BCM_IF_ERROR_RETURN(
        WRITE_PORT_OR_TRUNK_MAC_COUNTm(unit, MEM_BLOCK_ALL, index, 
                                       &mac_cnt_entry));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_l2_entry_replace
 * Description:
 *     Helper function to bcm_l2_replace API to be used as a call back for
 *     bcm_l2_traverse API and replace given entry if MAC matches
 * Parameters:
 *     unit         device number
 *     trav_st      structure with information from traverse function
 * Return:
 *     BCM_E_XXX
 */
STATIC int
_bcm_l2_entry_replace(int unit, void *trav_st)
{
    _bcm_l2_replace_t *rep_st;
    soc_mem_t   mem;
    uint32      *entry;
    uint32      *match_data, *match_mask, *new_data, *new_mask;
    int         entry_words, i, idx, rv;
    bcm_mac_t   l2mac;
    int         mod_val, port_val, trunk_val, vp_val;
    soc_control_t   *soc = SOC_CONTROL(unit);
    int         fifo_modified;
    bcm_l2_addr_t l2_addr; 

    if (trav_st == NULL) {
        return BCM_E_PARAM;
    }

    mem = ((_bcm_l2_traverse_t *)trav_st)->mem;
    entry = ((_bcm_l2_traverse_t *)trav_st)->data;
    idx = ((_bcm_l2_traverse_t *)trav_st)->mem_idx;
    rep_st = ((_bcm_l2_traverse_t *)trav_st)->user_data;

    if (rep_st == NULL || entry == NULL || mem == INVALIDm) {
        return BCM_E_PARAM;
    }

    if (mem == L2Xm) {
        match_data = (uint32 *)&rep_st->match_data;
        match_mask = (uint32 *)&rep_st->match_mask;
        new_data = (uint32 *)&rep_st->new_data;
        new_mask = (uint32 *)&rep_st->new_mask;
        entry_words = soc_mem_entry_words(unit, mem);
#if defined(BCM_TRIDENT2_SUPPORT)
         /* Retrieve the hit bit for TD2/TT2 */
        if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
            if (!SOC_IS_TOMAHAWKX(unit) && !SOC_IS_APACHE(unit) &&
                !SOC_IS_TRIDENT3X(unit) &&
                    SOC_IS_TD2_TT2(unit)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_td2_l2_hit_retrieve(unit, (l2x_entry_t *)entry, idx));
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */                  
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                return BCM_E_NONE;
            }
        }
        if (rep_st->int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_l2_from_l2x(unit, mem, &l2_addr, entry));
            return rep_st->trav_fn(unit, &l2_addr, rep_st->user_data);
        }
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO) {
                fifo_modified = FALSE;
                if ((rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS) &&
                    soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                                        L2_MOD_FIFO_ENABLE_L2_DELETEf) &&
                    soc_l2mod_running(unit, NULL, NULL)) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,
                                                REG_PORT_ANY,
                                                L2_MOD_FIFO_ENABLE_L2_DELETEf,
                                                0));
                    fifo_modified = TRUE;
                }
                rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry);
                if (fifo_modified) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,
                                                REG_PORT_ANY,
                                                L2_MOD_FIFO_ENABLE_L2_DELETEf,
                                                1));
                }
            } else {
                MEM_LOCK(unit, mem);
                if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                    MEM_UNLOCK(unit, mem);
                    return BCM_E_RESOURCE;
                }
                rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry);
#ifdef BCM_XGS_SWITCH_SUPPORT
                if (BCM_SUCCESS(rv)) {
                    rv = soc_l2x_sync_delete
                        (unit, entry, idx,
                         rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS ?
                         SOC_L2X_NO_CALLBACKS : 0);
                }
#endif /* BCM_XGS_SWITCH_SUPPORT */
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                MEM_UNLOCK(unit, mem);
            }
            return rv;
        } else {
            for (i = 0; i < entry_words; i++) {
                entry[i] &= ~new_mask[i];
                entry[i] |= new_data[i];
            }
            BCM_IF_ERROR_RETURN
                (_bcm_l2_replace_limit_count_update(unit, mem, rep_st, entry));

            /* Enhancement: use memory write range (per chunk) for replacing l2 entry data */
            return BCM_E_NONE;
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep_st->key_vfi != -1) {
#ifdef BCM_TRIUMPH_SUPPORT
            if (EXT_L2_ENTRYm == mem) {
                if (soc_mem_field32_get(unit, mem, entry, KEY_TYPE_VFIf) !=
                    1) {
                    return BCM_E_NONE;
                }
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            if (soc_mem_field32_get(unit, mem, entry, VFIf) !=
                rep_st->key_vfi) {
                return BCM_E_NONE;
            }
        } else {
#ifdef BCM_TRIUMPH_SUPPORT
            if (EXT_L2_ENTRYm == mem) {
                if (soc_mem_field32_get(unit, mem, entry, KEY_TYPE_VFIf) !=
                    0) {
                    return BCM_E_NONE;
                }
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            if (soc_mem_field32_get(unit, mem, entry, VLAN_IDf) !=
                rep_st->key_vlan) {
                return BCM_E_NONE;
            }
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
        soc_mem_mac_addr_get(unit, mem, entry, MAC_ADDRf, l2mac);
        if (ENET_CMP_MACADDR(rep_st->key_mac, l2mac)) {
            return BCM_E_NONE;
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
        vp_val = -1;
        trunk_val = -1;
        mod_val = -1;
        port_val = -1;
        if (soc_mem_field_valid(unit, mem, DEST_TYPEf) &&
            soc_mem_field32_get(unit, mem, entry, DEST_TYPEf) == 2) {
            vp_val = soc_mem_field32_get(unit, mem, entry, DESTINATIONf);
        } else if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (soc_mem_field32_get(unit, mem, entry, Tf)) {
                trunk_val = soc_mem_field32_get(unit, mem, entry, TGIDf);
            } else {
                mod_val = soc_mem_field32_get(unit, mem, entry, MODULE_IDf);
                port_val = soc_mem_field32_get(unit, mem, entry, PORT_NUMf);
            }
        } else {
            mod_val = soc_mem_field32_get(unit, mem, entry, MODULE_IDf);
            port_val = soc_mem_field32_get(unit, mem, entry, TGID_PORTf);
            if (port_val & BCM_TGID_TRUNK_INDICATOR(unit)) {
                trunk_val = BCM_MODIDf_TGIDf_TO_TRUNK(unit, mod_val, port_val);
            }
        }

        if (rep_st->match_dest.vp != -1) {
            if (rep_st->match_dest.vp != vp_val) {
                return BCM_E_NONE;
            }
        } else if (rep_st->match_dest.trunk != -1) {
            if (rep_st->match_dest.trunk != trunk_val) {
                return BCM_E_NONE;
            }
        } else {
            if (rep_st->match_dest.module != mod_val) {
                return BCM_E_NONE;
            }
            if (rep_st->match_dest.port != port_val) {
                return BCM_E_NONE;
            }
        }
    }

    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
         * non-static entries */
        if (soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) {
            return BCM_E_NONE;
        }
    }

    /* BCM_L2_REPLACE_PENDING means to only replace pending entries,
     * !BCM_L2_REPLACE_PENDING means to only replace non-pending entries. */
    if (soc_mem_field_valid(unit, mem, PENDINGf)) {
        if (soc_mem_field32_get(unit, mem, entry, PENDINGf)) {
            if (!(rep_st->flags & BCM_L2_REPLACE_PENDING)) {
                return (BCM_E_NONE);
            }
        } else {
            if (rep_st->flags & BCM_L2_REPLACE_PENDING) {
                return (BCM_E_NONE);
            }
        }
    }

    if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
        if (mem != EXT_L2_ENTRYm) {
            MEM_LOCK(unit, mem);
            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                MEM_UNLOCK(unit, mem);
                return BCM_E_RESOURCE;
            }
            rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, entry);
            if (BCM_FAILURE(rv)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                MEM_UNLOCK(unit, mem);
                return rv;
            }
#ifdef BCM_XGS_SWITCH_SUPPORT
            {
                uint32      soc_flags = 0; 
                soc_flags = (rep_st->flags & BCM_L2_REPLACE_NO_CALLBACKS) ? 
                            SOC_L2X_NO_CALLBACKS : 0;
                rv = soc_l2x_sync_delete(unit, entry, idx, soc_flags);
            }
#endif /* BCM_XGS_SWITCH_SUPPORT */
            SOC_L2_DEL_SYNC_UNLOCK(soc);
            MEM_UNLOCK(unit,  mem);
            return rv;
        }
    } else {
        if (rep_st->new_dest.vp != -1) { /* New destination is virtual port */
            soc_mem_field32_set(unit, mem, entry, DEST_TYPEf, 0x2);
            soc_mem_field32_set(unit, mem, entry, DESTINATIONf,
                                rep_st->new_dest.vp);
        } else if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (rep_st->new_dest.trunk != -1) { /* New destination is trunk */
                soc_mem_field32_set(unit, mem, entry, DEST_TYPEf, 1);
                soc_mem_field32_set(unit, mem, entry, TGIDf,
                                    rep_st->new_dest.trunk);
            } else { /* New destination is mod port */
                soc_mem_field32_set(unit, mem, entry, DEST_TYPEf, 0);
                soc_mem_field32_set(unit, mem, entry, MODULE_IDf,
                                    rep_st->new_dest.module);
                soc_mem_field32_set(unit, mem, entry, PORT_NUMf,
                                    rep_st->new_dest.port);
            }
        } else {
            if (rep_st->new_dest.trunk != -1) { /* New destination is trunk */
                mod_val = BCM_TRUNK_TO_MODIDf(unit, rep_st->new_dest.trunk);
                port_val = BCM_TRUNK_TO_TGIDf(unit, rep_st->new_dest.trunk);
                soc_mem_field32_set(unit, mem, entry, MODULE_IDf, mod_val);
                soc_mem_field32_set(unit, mem, entry, TGID_PORTf, port_val);
            } else { /* New destination is mod port */
                soc_mem_field32_set(unit, mem, entry, MODULE_IDf,
                                    rep_st->new_dest.module);
                soc_mem_field32_set(unit, mem, entry, TGID_PORTf,
                                    rep_st->new_dest.port);
            }
        }
        BCM_IF_ERROR_RETURN(
            _bcm_l2_replace_limit_count_update(unit, mem, rep_st, entry));
    }
#ifdef BCM_TRIUMPH_SUPPORT
    if (EXT_L2_ENTRYm == mem) {
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            BCM_IF_ERROR_RETURN(
                soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                       entry, entry, NULL));
            /* learn limit counters should not be updated for static entries */
            /* unless specifically instructed by API */
            if ((rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)
                && !(rep_st->flags & BCM_L2_REPLACE_LEARN_LIMIT) ) {
                rv = BCM_E_NONE;
            } else {
                rv = soc_triumph_learn_count_update(unit, (void *)entry,
                                                    TRUE, -1);
            }
        } else {
            rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ALL, 0,
                                        entry, entry, 0);
            if (rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
            }
        }
        return rv;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, entry);
}

/*
 * Function:
 *     _bcm_l2_replace_by_hw
 * Description:
 *     Helper function to bcm_l2_replace API to replace l2 entries by hardware
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_* 
 *     rep_st       structure with information of what to replace 
 * Return:
 *     BCM_E_XXX
 */
STATIC int 
_bcm_l2_replace_by_hw(int unit, uint32 flags, _bcm_l2_replace_t *rep_st)
{
#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        return _bcm_tr_l2_replace_by_hw(unit, rep_st);
    }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        return _bcm_fb_l2_addr_replace_by_vlan_dest(unit, flags, rep_st);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
    
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     _bcm_l2_replace_dest_setup
 * Description:
 *     Helper function to bcm_l2_replace API to setup a destination
 */
STATIC int
_bcm_l2_replace_dest_setup(int unit, bcm_module_t module, bcm_port_t port,
                           bcm_trunk_t trunk, int is_trunk,
                           _bcm_l2_replace_dest_t *dest)
{
    int tid_is_vp_lag;
    int vp;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    int vp_lag_vp;
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */

    /* If all are -1, means ignore the replacement with new destination */
    if (module == -1 && port == -1 && trunk == -1) {
        dest->module = dest->port = dest->trunk = dest->vp = -1;
        return BCM_E_NONE;
    }

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_MPLS_PORT(port) ||
            BCM_GPORT_IS_SUBPORT_GROUP(port) ||
            BCM_GPORT_IS_SUBPORT_PORT(port) ||
            BCM_GPORT_IS_MIM_PORT(port) ||
            BCM_GPORT_IS_VLAN_PORT(port) ||
            BCM_GPORT_IS_WLAN_PORT(port) ||
            BCM_GPORT_IS_TRILL_PORT(port) ||
            BCM_GPORT_IS_NIV_PORT(port) ||
            BCM_GPORT_IS_VXLAN_PORT(port) ||
            BCM_GPORT_IS_L2GRE_PORT(port) ||
            BCM_GPORT_IS_EXTENDER_PORT(port)) {

#ifdef BCM_KATANA2_SUPPORT
            if (BCM_GPORT_IS_SUBPORT_PORT(port) &&
                _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            } else
#endif
            {
                /* all these gport has similar encoding format */
                vp = port & 0xffffff;

                if (!SOC_MEM_IS_VALID(unit, SOURCE_VPm) ||
                    vp > soc_mem_index_max(unit, SOURCE_VPm)) {
                    return BCM_E_PORT;
                }
                dest->vp = vp;
            }
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, port, &dest->module, &dest->port,
                                        &dest->trunk, &dest->vp));
        }

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, port, &dest->module, &dest->port,
                                        &dest->trunk, &dest->vp));
            dest->vp = -1;
        }
#endif

#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, port, &dest->module, &dest->port,
                                        &dest->trunk, &dest->vp));
            dest->vp = -1;
        }
#endif
    } else if (is_trunk) {
        BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk));
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_id_is_vp_lag(unit, trunk, &tid_is_vp_lag));
        if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
            if (soc_feature(unit, soc_feature_vp_lag)) {
                /* Get the VP value representing VP LAG */
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_trunk_tid_to_vp_lag_vp(unit, trunk, &vp_lag_vp));
                dest->vp = vp_lag_vp;
                dest->trunk = -1;
            } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
            {
                return BCM_E_PORT;
            }
        } else {
            dest->trunk = trunk;
            dest->vp = -1;
        }
    } else {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, module, port,
                                     &dest->module, &dest->port));
        if (!SOC_MODID_ADDRESSABLE(unit, dest->module)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, dest->port)) {
            return BCM_E_PORT;
        }
        dest->trunk = -1;
        dest->vp = -1;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_l2_replace_data_mask_setup(int unit, _bcm_l2_replace_t *rep)
{
    bcm_mac_t mac_mask;
    bcm_mac_t mac_data;
    int field_len;
    uint32 module, port;

    sal_memset(&rep->match_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->match_data, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->new_data, 0, sizeof(l2x_entry_t));

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, BASE_VALIDf, 1);
    } else if (soc_mem_field_valid(unit, L2Xm, VALIDf)) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VALIDf, 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, VALIDf, 1);
    }

    if (SOC_IS_TR_VL(unit) && (rep->key_type != -1)) {
        field_len = soc_mem_field_length(unit, L2Xm, KEY_TYPEf);
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, KEY_TYPEf,
                            (1 << field_len) - 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, KEY_TYPEf,
                            rep->key_type);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        if (rep->key_vfi != -1) {
            field_len = soc_mem_field_length(unit, L2Xm, VFIf);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VFIf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, VFIf,
                                rep->key_vfi);
        } else {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VLAN_IDf, 0xfff);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, VLAN_IDf,
                                rep->key_vlan);
        }
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_MAC) {
        sal_memset(&mac_mask, 0xff, sizeof(mac_mask));
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             rep->key_mac);
    }

    if ((rep->flags & BCM_L2_REPLACE_MATCH_UC) && 
        !(rep->flags & BCM_L2_REPLACE_MATCH_MC)) {
        /* The 40th bit of Unicast must be 0 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    } else if ((rep->flags & BCM_L2_REPLACE_MATCH_MC) &&
        !(rep->flags & BCM_L2_REPLACE_MATCH_UC)) {
        /* The 40th bit of Multicast must be 1 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        if (rep->match_dest.vp != -1) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DESTINATIONf, 
                                    (1 << field_len) - 1);
                soc_mem_field32_dest_set(unit, L2Xm, &rep->match_data, 
                    DESTINATIONf, SOC_MEM_FIF_DEST_DVP, rep->match_dest.vp);
            } else {
                field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DEST_TYPEf,
                                    (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data, DEST_TYPEf, 2);

                field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DESTINATIONf,
                                    (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data, DESTINATIONf,
                                    rep->match_dest.vp);
            }
        } else {
            if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                    if (rep->match_dest.trunk != -1) {
                        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DESTINATIONf,
                                            (1 << field_len) - 1);
                        soc_mem_field32_dest_set(unit, L2Xm, &rep->match_data,
                            DESTINATIONf, SOC_MEM_FIF_DEST_LAG, (rep->match_dest.trunk |
                                ((rep->flags & BCM_L2_REPLACE_REMOTE_TRUNK) ? 
                                    (1 << SOC_MEM_FIF_DGPP_RT_SHIFT_BITS) : 0)));
                    } else {
                        if (rep->match_dest.port != -1) {
                            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DESTINATIONf,
                                                (1 << field_len) - 1);
                            soc_mem_field32_dest_set(unit, L2Xm, &rep->match_data,
                                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                                rep->match_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                                rep->match_dest.port);
                        } else {
                            soc_mem_field32_dest_set(unit, L2Xm, &rep->match_mask,
                                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                                0xff << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | 0);
                            soc_mem_field32_dest_set(unit, L2Xm, &rep->match_data,
                                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                                rep->match_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS |
                                0);
                        }

                        if (!(rep->flags & BCM_L2_REPLACE_MATCH_MAC)) {
                            /* Ignore multicast mac from bulk delete */
                            mac_mask[0] = 0x01;
                            mac_mask[1] = mac_mask[2] = mac_mask[3] =
                                          mac_mask[4] = mac_mask[5] = 0x00;
                            soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask,
                                MAC_ADDRf, mac_mask);
                        }
                    }
                } else {
                    field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                    soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DEST_TYPEf,
                                        (1 << field_len) - 1);

                    if (rep->match_dest.trunk != -1) {
                        soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                            DEST_TYPEf, 1);

                        field_len = soc_mem_field_length(unit, L2Xm, TGIDf);
                        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, TGIDf,
                                            (1 << field_len) - 1);
                        soc_mem_field32_set(unit, L2Xm, &rep->match_data, TGIDf,
                                            rep->match_dest.trunk);
                    } else {
                        /* DEST_TYPE field in data is 0 */

                        field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                        soc_mem_field32_set(unit, L2Xm, &rep->match_mask,
                                            MODULE_IDf, (1 << field_len) - 1);
                        soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                            MODULE_IDf, rep->match_dest.module);
                        if (rep->match_dest.port != -1) {
                            field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                            soc_mem_field32_set(unit, L2Xm, &rep->match_mask,
                                                PORT_NUMf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                                PORT_NUMf, rep->match_dest.port);
                        }

                        if (!(rep->flags & BCM_L2_REPLACE_MATCH_MAC)) {
                            /* Ignore multicast mac from bulk delete */
                            mac_mask[0] = 0x01;
                            mac_mask[1] = mac_mask[2] = mac_mask[3] =
                                          mac_mask[4] = mac_mask[5] = 0x00;
                            soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask,
                                MAC_ADDRf, mac_mask);
                        }
                    }
                } /* soc_feature_generic_dest */
            } else {
                if (rep->match_dest.trunk != -1) {
                    module = BCM_TRUNK_TO_MODIDf(unit, rep->match_dest.trunk);
                    port = BCM_TRUNK_TO_TGIDf(unit, rep->match_dest.trunk);
                } else {
                    module = rep->match_dest.module;
                    port = rep->match_dest.port;
                }

                field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask, MODULE_IDf,
                                    (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data, MODULE_IDf,
                                    module);
                if (port != -1) {
                    field_len = soc_mem_field_length(unit, L2Xm, TGID_PORTf);
                    soc_mem_field32_set(unit, L2Xm, &rep->match_mask, TGID_PORTf,
                                        (1 << field_len) - 1);
                    soc_mem_field32_set(unit, L2Xm, &rep->match_data, TGID_PORTf,
                                        port);
                }
            }
        }
    }
    
    /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
    * non-static entries */
    if (!(rep->flags & BCM_L2_REPLACE_MATCH_STATIC)) { 
        if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC) { 
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, STATIC_BITf, 1); 
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, STATIC_BITf, 1); 
        } else { 
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, STATIC_BITf, 1);
            /* STATIC_BIT field in data is 0 */ 
        } 
    }
    /* STATIC_BIT field in mask is 0 */

    /* BCM_L2_FLAGS2_MESH means to replace mesh-bit-set L2 entries along
     * with mesh-bit-unset L2 entries.
     */
    if (!(rep->key_l2_flags2 & BCM_L2_FLAGS2_MESH ||
          rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_MESH)) {
        if (SOC_MEM_FIELD_VALID(unit, L2Xm, MESHf)) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, MESHf, 1);
        } else if (SOC_MEM_FIELD_VALID(unit, L2Xm, EVPN_AGE_DISABLEf)) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, EVPN_AGE_DISABLEf,
                                1);
        }
        /* MESH field in data is 0 */
    }
    /* MESH field in mask is 0 */

#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TRIDENT3X(unit)) {
        if (rep->flags & BCM_L2_REPLACE_MATCH_CLASS_ID) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, CLASS_IDf, 0x3f);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, CLASS_IDf, rep->key_class_id);
        }
        if (rep->flags & BCM_L2_REPLACE_MATCH_INT_PRI) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, RPEf, 0x1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, RPEf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, PRIf, 0x1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, PRIf, rep->key_int_pri);
         }
    }
#endif /*BCM_TRIDENT2PLUS_SUPPORT*/
    if (soc_mem_field_valid(unit, L2Xm, PENDINGf)) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, PENDINGf, 1);
        if (rep->flags & BCM_L2_REPLACE_PENDING) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, PENDINGf, 1);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, SRC_DISCARDf, 1);
        if (rep->key_l2_flags & BCM_L2_DISCARD_SRC) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, SRC_DISCARDf, 1);
        }
    }

    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_SRC_HIT) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITSAf, 1);
        if (rep->key_l2_flags & BCM_L2_SRC_HIT) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITSAf, 1);
        }
    }
    if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_DES_HIT) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITDAf, 1);
        if (rep->key_l2_flags & BCM_L2_DES_HIT) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITDAf, 1);
        }
    }
    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LOCAL_SAf)) {
        if (rep->int_flags & _BCM_L2_REPLACE_INT_MATCH_NATIVE) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, LOCAL_SAf, 1);
            if (rep->key_l2_flags & BCM_L2_NATIVE) {
                soc_mem_field32_set(unit, L2Xm, &rep->match_data, LOCAL_SAf,
                                    1);
            }
        }
    }
    if (_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITDAf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, HITSAf, 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, HITSAf, 1);
        }
    }
    if (!(rep->flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE))) {
        if (!_CLEAR_ALL_ENTRIES_SRC_OR_DES_HIT_BIT(rep->flags)) {
            if (rep->new_dest.vp != -1) {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DESTINATIONf, 
                                    (1 << field_len) - 1);
                    soc_mem_field32_dest_set(unit, L2Xm, &rep->new_data, 
                        DESTINATIONf, SOC_MEM_FIF_DEST_DVP, rep->new_dest.vp);
                } else {
                    field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                        (1 << field_len) - 1);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_data, DEST_TYPEf, 2);
                    field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DESTINATIONf,
                                        (1 << field_len) - 1);
                    soc_mem_field32_set(unit, L2Xm, &rep->new_data, DESTINATIONf,
                                        rep->new_dest.vp);
                }
            } else {
                if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                    if (soc_feature(unit, soc_feature_generic_dest)) {
                        field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
                        soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DESTINATIONf,
                                            (1 << field_len) - 1);
                        if (rep->new_dest.trunk != -1) {
                            soc_mem_field32_dest_set(unit, L2Xm, &rep->new_data, 
                                DESTINATIONf, SOC_MEM_FIF_DEST_LAG, rep->new_dest.trunk);
                        } else {
                            soc_mem_field32_dest_set(unit, L2Xm, &rep->new_data, 
                                DESTINATIONf, SOC_MEM_FIF_DEST_DGPP, 
                                rep->new_dest.module << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | 
                                rep->new_dest.port);
                        }
                    } else {
                        if (rep->new_dest.trunk != -1) {
                            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                                (1 << field_len) - 1);

                            soc_mem_field32_set(unit, L2Xm, &rep->new_data, DEST_TYPEf,
                                                1);

                            /*
                             * Writing to DESTINATION field instead of TGIDf
                             * to ensure the MSB bits of DESTINATION are cleared.
                             */
                            field_len = soc_mem_field_length(unit, L2Xm,
                                    DESTINATIONf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                    DESTINATIONf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                    DESTINATIONf, rep->new_dest.trunk);
                        } else if (rep->new_dest.port != -1 
                                   && rep->new_dest.module != -1) {
                            /* DEST_TYPE field in data is 0 */
                            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                                (1 << field_len) - 1);

                            field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                                MODULE_IDf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                                MODULE_IDf, rep->new_dest.module);

                            field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                                PORT_NUMf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                                PORT_NUMf, rep->new_dest.port);
                        }
                    } /* soc_feature_generic_dest */
                } else {
                    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
                        if (rep->new_dest.trunk != -1) {
                            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                                (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data, DEST_TYPEf,
                                                1);
                            /*
                             * Writing to DESTINATION field instead of TGIDf
                             * to ensure the MSB bits of DESTINATION are cleared.
                             */
                            field_len = soc_mem_field_length(unit, L2Xm,
                                    DESTINATIONf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                    DESTINATIONf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                    DESTINATIONf, rep->new_dest.trunk);
                        } else if (rep->new_dest.port != -1 
                                   && rep->new_dest.module != -1) {
                            /* DEST_TYPE field in data is 0 */
                            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, DEST_TYPEf,
                                                (1 << field_len) - 1);
                            field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                                MODULE_IDf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                                MODULE_IDf, rep->new_dest.module);
                            field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask,
                                                PORT_NUMf, (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data,
                                                PORT_NUMf, rep->new_dest.port);
                        }
                    } else {
                        if (rep->new_dest.trunk != -1) {
                            module = BCM_TRUNK_TO_MODIDf(unit, rep->new_dest.trunk);
                            port = BCM_TRUNK_TO_TGIDf(unit, rep->new_dest.trunk);
                        } else if (rep->new_dest.port != -1 
                                   && rep->new_dest.module != -1) {
                            module = rep->new_dest.module;
                            port = rep->new_dest.port;
                        } else {
                            module = port = -1;
                        }
                        if (module != -1 && port != -1) {
                            field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, MODULE_IDf,
                                                (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data, MODULE_IDf,
                                                module);
                            field_len = soc_mem_field_length(unit, L2Xm, TGID_PORTf);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, TGID_PORTf,
                                                (1 << field_len) - 1);
                            soc_mem_field32_set(unit, L2Xm, &rep->new_data, TGID_PORTf,
                                                port);
                        }
                    }
                }
            }
        }
        if (rep->flags & BCM_L2_REPLACE_DES_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, HITDAf, 1);
        }
        if (rep->flags & BCM_L2_REPLACE_SRC_HIT_CLEAR) {
            soc_mem_field32_set(unit, L2Xm, &rep->new_mask, HITSAf, 1);
        }
    }

    return BCM_E_NONE;
}

STATIC int
_soc_l2_sync_mem_cache(int unit, _bcm_l2_replace_t *rep_st)
{
    uint8 *vmap;
    soc_mem_t mem = L2Xm;
    uint32 *cache, *entry, *match_data, *match_mask;
    uint32 *new_data, *new_mask;
    int index_max, idx, i;
    int entry_dw = soc_mem_entry_words(unit, mem);
    int entry_words = soc_mem_entry_words(unit, mem);
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    soc_field_t vld_fld = VALIDf;
    soc_control_t *soc = SOC_CONTROL(unit);

    cache = SOC_MEM_STATE(unit, mem).cache[copyno];
    if (cache == NULL) {
        return BCM_E_NONE;
    }

    if (soc_feature(unit, soc_feature_base_valid)) {
        vld_fld = BASE_VALIDf;
    }
    vmap = SOC_MEM_STATE(unit, mem).vmap[copyno];
    index_max = soc_mem_index_max(unit, mem);
    for (idx = 0; idx <= index_max; idx++) {
        if (!CACHE_VMAP_TST(vmap, idx)) {
            continue;
        }
        entry = cache + (idx * entry_dw);
        if (!soc_mem_field32_get(unit, mem, entry, vld_fld)) {
            continue;
        }

        /* When not handling a STATIC entry or when l2-fifo-mode station
         * move sw loopup is not enabled, skip the SW cache update
         */
        if ((!soc_mem_field32_get(unit, mem, entry, STATIC_BITf)) &&
            !(soc->l2_fifo_mode_station_move_sw_lookup)) {
            continue;
        }
        match_data = (uint32 *)&rep_st->match_data;
        match_mask = (uint32 *)&rep_st->match_mask;
        for (i = 0; i < entry_words; i++) {
            if ((entry[i] ^ match_data[i]) & match_mask[i]) {
                break;
            }
        }
        if (i != entry_words) {
            continue;
        }
        /* Match found, delete or replace entry */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Match found in L2 bulk cache op: %d\n"), idx));
        if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
            /* Delete op - invalidate the entry */
            CACHE_VMAP_CLR(vmap, idx);
        } else {
            /* Replace op - update entry */
            new_data = (uint32 *)&rep_st->new_data;
            new_mask = (uint32 *)&rep_st->new_mask;
            for (i = 0; i < entry_words; i++) {
                entry[i] ^= (entry[i] ^ new_data[i]) & new_mask[i];
            }
        }
    }
    return BCM_E_NONE;
}

STATIC int
_is_soc_l2_mem_cache_enabled(int unit)
{
    soc_mem_t mem = L2Xm;
    uint32 *cache;
    int copyno = SOC_MEM_BLOCK_ANY(unit, mem);
    
    cache = SOC_MEM_STATE(unit, mem).cache[copyno];

    if (cache == NULL) {
        return FALSE;
    }

    return TRUE;
}

STATIC int
_bcm_esw_l2_replace(int unit, uint32 flags, uint32 int_flags,
                    bcm_l2_addr_t *match_addr, bcm_module_t new_module,
                    bcm_port_t new_port, bcm_trunk_t new_trunk,
                    bcm_l2_traverse_cb trav_fn, void *user_data)
{
    uint32              cmp_flags;
    _bcm_l2_replace_t   rep_st;
    _bcm_l2_traverse_t  trav_st;
    _bcm_l2_match_ctrl_t *match_ctrl;
    int                 entry_rd_idx, buf_index, entry_index, do_trav_cb;
    int                 rv = BCM_E_UNAVAIL;
    int                 use_hw_ppa, do_freeze;
    int                 mode;
    l2x_entry_t         l2x_entry, *l2x_entry_ptr;
    bcm_l2_addr_t       l2_addr;
    int tid_is_vp_lag;
    uint32 rval = 0;
    uint32 l2_memwr = 0;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    source_vp_entry_t svp;
    uint32 etype;
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
#ifdef BCM_TRIDENT3_SUPPORT
    uint32 flow_handle;
#endif /* BCM_TRIDENT3_SUPPORT */
    soc_control_t *soc = NULL;

    soc = SOC_CONTROL(unit);
    sal_memset(&rep_st, 0, sizeof(_bcm_l2_replace_t));
    sal_memset(&trav_st, 0, sizeof(_bcm_l2_traverse_t));

    rep_st.trav_fn = trav_fn;
    rep_st.user_data = user_data;
    rep_st.flags = flags;
    rep_st.int_flags = int_flags;
    if ((int_flags & _BCM_L2_REPLACE_IGNORE_PORTID) &&
        !(flags & BCM_L2_REPLACE_DELETE) &&
        !(flags & BCM_L2_REPLACE_MATCH_DEST)) {
        return BCM_E_PARAM;
    }
    if (!(flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) &&
        !(int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
        BCM_IF_ERROR_RETURN(_bcm_l2_replace_dest_setup
                            (unit, new_module, new_port, new_trunk,
                             flags & BCM_L2_REPLACE_NEW_TRUNK,
                             &rep_st.new_dest));
    }

    if (match_addr == NULL) {
        if (flags & BCM_L2_REPLACE_VPN_TYPE) {
            rep_st.key_type = 3;  /* VFI Type */
        }
    } else {
        rep_st.key_l2_flags = match_addr->flags;
        rep_st.key_l2_flags2 = match_addr->flags2;

        if (flags & BCM_L2_REPLACE_MATCH_DEST) {
            if (int_flags & _BCM_L2_REPLACE_IGNORE_PORTID) {
                if (!SOC_MODID_ADDRESSABLE(unit, match_addr->modid)) {
                    return BCM_E_BADID;
                }
                rep_st.match_dest.vp = -1;
                rep_st.match_dest.trunk = -1;
                rep_st.match_dest.port = -1;
                rep_st.match_dest.module = match_addr->modid;
            } else {
                BCM_IF_ERROR_RETURN(_bcm_l2_replace_dest_setup
                                    (unit, match_addr->modid, match_addr->port,
                                     match_addr->tgid,
                                     match_addr->flags & BCM_L2_TRUNK_MEMBER,
                                     &rep_st.match_dest));
            }
            if (rep_st.match_dest.vp != -1) {
                if(match_addr->flags & BCM_L2_TRUNK_MEMBER) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_id_is_vp_lag(unit, match_addr->tgid,
                                                    &tid_is_vp_lag));
                    if (tid_is_vp_lag) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
                        if (soc_feature(unit, soc_feature_vp_lag)) {
                            BCM_IF_ERROR_RETURN(
                                soc_mem_read(unit, SOURCE_VPm, MEM_BLOCK_ANY, 
                                             rep_st.match_dest.vp, &svp));
                            if (SOC_MEM_FIELD_VALID(unit, 
                                                    SOURCE_VPm, ENTRY_TYPEf)) {
                                etype = soc_SOURCE_VPm_field32_get(unit, &svp, 
                                                                   ENTRY_TYPEf);
                                switch(etype) {
                                case 1: /* VPLS SVP and use VFI */
                                    rep_st.key_type =  3; /* VFI */
                                    break;
                                case 3: /* VLAN-based Forwarding */
                                    rep_st.key_type =  0; /* L2 Bridge */
                                    break;
                                default: /* 0: INVALID */
                                    rep_st.key_type = -1; /* Do not care key type */
                                    break;
                                }
                            }
                        } else 
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
                        {
                            return BCM_E_PORT;
                        }
                    } else {
                        return BCM_E_PARAM;
                    }
                } else {
                    if (!BCM_GPORT_IS_WLAN_PORT(match_addr->port)  &&
                        !BCM_GPORT_IS_NIV_PORT(match_addr->port)   &&
                        !BCM_GPORT_IS_TRILL_PORT(match_addr->port) &&
                        !BCM_GPORT_IS_VLAN_PORT(match_addr->port)  &&
                        !BCM_GPORT_IS_EXTENDER_PORT(match_addr->port)) {
                         /* Note: For MPLS, MiM, L2GRE, VXLAN VFI types only */
                         rep_st.key_type = 3; /* VFI Type */
                    }
                }
            }
        }

        if (flags & BCM_L2_REPLACE_MATCH_VLAN) {
            rep_st.key_vfi = -1;
#ifdef BCM_TRIUMPH_SUPPORT
            if (SOC_MEM_IS_VALID(unit, VFIm)) {
                if (_BCM_VPN_VFI_IS_SET(match_addr->vid)) {
                    _BCM_VPN_GET(rep_st.key_vfi, _BCM_VPN_TYPE_VFI, match_addr->vid);
                }
            }
#endif /* BCM_TRIUMPH_SUPPORT */
            if (rep_st.key_vfi != -1) {
                if (rep_st.key_vfi > soc_mem_index_max(unit, VFIm)) {
                    return BCM_E_PARAM;
                }
                rep_st.key_type = 3;  /* VFI Type */
            } else {
                VLAN_CHK_ID(unit, match_addr->vid);
                rep_st.key_vlan = match_addr->vid;
                rep_st.key_type = 0;  /* L2 Bridge */
            }
        }

        if (flags & BCM_L2_REPLACE_MATCH_MAC) {
            sal_memcpy(&rep_st.key_mac, match_addr->mac, sizeof(bcm_mac_t));
        }

        /* When replace L2 by MAC, key type must be provided. */
            /* Key type is 0 by default for L2 Bridge. */
            /* Here add support for explicitly assigned VFI type */
        if (flags & BCM_L2_REPLACE_VPN_TYPE) {
            rep_st.key_type = 3;  /* VFI Type */
        }
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TRIDENT3X(unit)) {
            if (flags & BCM_L2_REPLACE_MATCH_INT_PRI) {
                rep_st.key_int_pri = match_addr->cos_dst;
            }
            if (flags & BCM_L2_REPLACE_MATCH_CLASS_ID) {
                rep_st.key_class_id= match_addr->group;
            }
        }
#endif
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (match_addr != NULL) {
        flow_handle = match_addr->flow_handle;
    }
    else {
        flow_handle = 0;
    }

    if (soc_feature(unit, soc_feature_flex_flow) &&
        flow_handle != 0) {

        /* Get view id corresponding to the specified flow. */
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit,
                              match_addr->flow_handle,
                              match_addr->flow_option_handle,
                              SOC_FLOW_DB_FUNC_L2_SWITCH_ID,
                              &rep_st.view_id);
        BCM_IF_ERROR_RETURN(rv);

        BCM_IF_ERROR_RETURN(_bcm_td3_flex_l2_replace_data_mask_setup(unit, &rep_st));
    }
    else
#endif
    {
        BCM_IF_ERROR_RETURN(_bcm_l2_replace_data_mask_setup(unit, &rep_st));
    }

    /* Validate matching destination only if match all was not set */
    cmp_flags = flags & (BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN |
                         BCM_L2_REPLACE_MATCH_DEST);
    use_hw_ppa = TRUE;
    switch (cmp_flags) {
    case BCM_L2_REPLACE_MATCH_MAC:
    case BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_DEST:
    case BCM_L2_REPLACE_MATCH_MAC | BCM_L2_REPLACE_MATCH_VLAN:
        if (!soc_feature(unit, soc_feature_l2_bulk_control)) {
            use_hw_ppa = FALSE;
        } else if (soc_feature(unit, soc_feature_l2_bulk_bypass_replace)) {
            if (!( flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) &&
                !(int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
                use_hw_ppa = FALSE;
            }
        }
        break;
    case BCM_L2_REPLACE_MATCH_VLAN:
    case BCM_L2_REPLACE_MATCH_DEST:
    case BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_DEST:
        if (SOC_CONTROL(unit)->l2x_ppa_bypass) {
            use_hw_ppa = FALSE;
        } else if (!soc_feature(unit, soc_feature_l2_bulk_control) &&
                   !soc_feature(unit, soc_feature_ppa_match_vp) &&
                   (rep_st.flags & BCM_L2_REPLACE_MATCH_DEST) &&
                   rep_st.match_dest.vp != -1) {
            use_hw_ppa = FALSE;
        } else if (soc_feature(unit, soc_feature_l2_bulk_bypass_replace)) {
            if (!(flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE)) &&
                !(int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
                if (cmp_flags != BCM_L2_REPLACE_MATCH_DEST ||
                    rep_st.match_dest.vp != -1 ||
                    rep_st.match_dest.trunk != -1) {
                    use_hw_ppa = FALSE;
                }
            }
        } else if (!soc_feature(unit, soc_feature_l2_bulk_control) &&
            (flags & BCM_L2_REPLACE_SRC_HIT_CLEAR || 
             flags & BCM_L2_REPLACE_DES_HIT_CLEAR)) {
            /*
              * For devices without supporting L2 bulk feature:
              * - It doesn't support to reset SRC/DST Hit-bit respectively by Hardware L2 PPA
              * register for L2 replace function.
              * - To traverse the L2 table and replace given entry if any L2 entry matches.
              */
            use_hw_ppa = FALSE;
        }
        break;
    default:
        break;
    }
#ifdef PLISIM
#if defined(BCM_KATANA_SUPPORT)
    /* Forcefully setting hw_ppa flag to false in KatanaSimulationEnvironment */
    if (SOC_IS_KATANAX(unit) && (SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        use_hw_ppa = FALSE;
    }
#endif
#endif

    /* Hardware L2 PPA register don't support to delete by UC, MC entries or both respectively */
    if (use_hw_ppa && 
        (flags & BCM_L2_REPLACE_DELETE) && 
        (flags & (BCM_L2_REPLACE_MATCH_UC | BCM_L2_REPLACE_MATCH_MC))) {
        if (SOC_REG_IS_VALID(unit, PER_PORT_AGE_CONTROLr) || 
            SOC_REG_IS_VALID(unit, PER_PORT_AGE_CONTROL_64r)) {
            return BCM_E_UNAVAIL;
        }
    }

    mode = SOC_CONTROL(unit)->l2x_mode;
    /* Delete operation don't require freeze */
    do_freeze = FALSE;
    if (use_hw_ppa && (int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
        if (soc_feature(unit, soc_feature_l2_bulk_control) &&
            mode == L2MODE_FIFO) {
            if (_bcm_l2_match_ctrl[unit]->entry_wr_idx != 0) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "Critical resource reentry!!!\n")));
            }
            _bcm_l2_match_ctrl[unit]->entry_wr_idx = 0;
        } else {
            use_hw_ppa = FALSE;
        }
    } else if ((!use_hw_ppa &&
                !(flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE))) || 
               mode != L2MODE_FIFO || 
               ((flags & BCM_L2_REPLACE_MATCH_STATIC) &&
                _is_soc_l2_mem_cache_enabled(unit))) {
        do_freeze = TRUE;
    }

    if (do_freeze) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_freeze(unit));
    }
    if (use_hw_ppa) {
        rv = _bcm_l2_replace_by_hw(unit, flags, &rep_st);
        if (BCM_SUCCESS(rv) && (int_flags & _BCM_L2_REPLACE_INT_NO_ACTION)) {
            match_ctrl = _bcm_l2_match_ctrl[unit];
            if (SOC_REG_IS_VALID(unit,L2_MOD_FIFO_ENABLEr)) {
                rv = READ_L2_MOD_FIFO_ENABLEr(unit, &rval);
                l2_memwr = soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, rval,
                                             L2_MEMWRf);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_reg_field32_modify(unit, L2_MOD_FIFO_ENABLEr,
                                     REG_PORT_ANY, L2_MEMWRf, 1);
                }
            } else {
                rv = READ_AUX_ARB_CONTROLr(unit, &rval);
                l2_memwr = soc_reg_field_get(unit, AUX_ARB_CONTROLr, rval,
                                             L2_MOD_FIFO_ENABLE_MEMWRf);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,REG_PORT_ANY,
                                            L2_MOD_FIFO_ENABLE_MEMWRf, 1);
                }
            }
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry);
                if (BCM_SUCCESS(rv)) {
                    (void)soc_mem_write(unit, L2Xm, MEM_BLOCK_ALL, 0,
                                        &l2x_entry);
                }
                if (SOC_REG_IS_VALID(unit,L2_MOD_FIFO_ENABLEr)) {
                    (void)soc_reg_field32_modify(unit, L2_MOD_FIFO_ENABLEr,
                                 REG_PORT_ANY, L2_MEMWRf, l2_memwr);
                } else {
                    (void)soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,
                                             REG_PORT_ANY,
                                             L2_MOD_FIFO_ENABLE_MEMWRf, l2_memwr);
                }
            }
            entry_rd_idx = 0;
            if (trav_fn != NULL) {
                do_trav_cb = TRUE;
            } else {
                do_trav_cb = FALSE;
            }
            while (1) {
                if (entry_rd_idx == match_ctrl->entry_wr_idx) {
                    if (sal_sem_take(match_ctrl->sem, 5000000)) {
                        /* If waiting too long, something happened in HW */
                        if (SOC_CONTROL(unit)->l2x_pid == SAL_THREAD_ERROR) {
                            for (buf_index = entry_rd_idx / _BCM_L2_MATCH_ENTRY_BUF_SIZE;
                                 buf_index >= 0;
                                 buf_index --) {
                                sal_free(match_ctrl->l2x_entry_buf[buf_index]);
                                match_ctrl->l2x_entry_buf[buf_index] = NULL;
                            }
                            match_ctrl->entry_wr_idx = 0;
                        }
                        rv = BCM_E_INTERNAL;
                        break;
                    }

                    continue;                
                }
                buf_index = entry_rd_idx / _BCM_L2_MATCH_ENTRY_BUF_SIZE;
                entry_index = entry_rd_idx % _BCM_L2_MATCH_ENTRY_BUF_SIZE;
                l2x_entry_ptr =
                    &match_ctrl->l2x_entry_buf[buf_index][entry_index];

                if (sal_memcmp(l2x_entry_ptr, soc_mem_entry_null(unit, L2Xm),
                               sizeof(l2x_entry_t)) == 0) {
                    /* Found the event */
                    match_ctrl->entry_wr_idx = 0;
                    /* Does not free buf_index 0. */
                    for (buf_index = 1;
                         buf_index < _BCM_L2_MATCH_ENTRY_BUF_COUNT;
                         buf_index ++) {
                        if (match_ctrl->l2x_entry_buf[buf_index]) {
                            sal_free(match_ctrl->l2x_entry_buf[buf_index]);
                            match_ctrl->l2x_entry_buf[buf_index] = NULL;
                        }
                    }
                    break;
                }
                if (do_trav_cb) {
                    (void)_bcm_esw_l2_from_l2x(unit, 0, &l2_addr,
                                               (uint32 *)l2x_entry_ptr);
                    if (trav_fn(unit, &l2_addr, user_data) < 0) {
                        do_trav_cb = FALSE;
                    }
                }
                if (do_trav_cb) {
                    entry_rd_idx++;
                } else {
                    /* skip to the last buffered entry */
                    entry_rd_idx = match_ctrl->entry_wr_idx - 1;
                }
            }
        }
    } else {
        trav_st.user_data = (void *)&rep_st;
        trav_st.int_cb = _bcm_l2_entry_replace;
        rv = _bcm_esw_l2_traverse(unit, &trav_st);
    }

    if (use_hw_ppa && !(int_flags & _BCM_L2_REPLACE_INT_NO_ACTION) &&
        SOC_SUCCESS(rv) && ((rep_st.flags & BCM_L2_REPLACE_MATCH_STATIC) ||
        soc->l2_fifo_mode_station_move_sw_lookup)) {
        /* update the entries in mem cache with the bulk op results */
        rv = _soc_l2_sync_mem_cache(unit, &rep_st);
    }

    if (do_freeze) {
        BCM_IF_ERROR_RETURN(bcm_esw_l2_addr_thaw(unit));
    }

    return rv;
}

/*
 * Function:
 *     bcm_esw_l2_replace
 * Description:
 *     To replace destination (or delete) multiple L2 entries
 * Parameters:
 *     unit         device number
 *     flags        flags BCM_L2_REPLACE_*
 *     match_addr   L2 parameters to match on delete/replace
 *     new_module   new module ID for a replace
 *     new_port     new port for a replace
 *     new_trunk    new trunk for a replace
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_replace(int unit, uint32 flags, bcm_l2_addr_t *match_addr,
                   bcm_module_t new_module, bcm_port_t new_port,
                   bcm_trunk_t new_trunk)
{
    uint32 int_flags = 0;

    if (match_addr != NULL) {
        if (match_addr->flags & BCM_L2_STATIC) {
            int_flags = _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC;
        }
    }

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_replace(unit, flags, match_addr, new_module,
                                        new_port, new_trunk);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_replace(unit, flags, match_addr,
                                  new_module, new_port, new_trunk);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */    

    if (!flags) {
        return BCM_E_PARAM;
    }
    if (( flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE) ) &&
        (flags & BCM_L2_REPLACE_NEW_TRUNK)) {
        return BCM_E_PARAM;
    }

    if ((flags & BCM_L2_REPLACE_DELETE) && (flags & BCM_L2_REPLACE_MATCH_MAC) &&
        ((flags & BCM_L2_REPLACE_MATCH_UC) || (flags & BCM_L2_REPLACE_MATCH_MC))) {
        return BCM_E_PARAM;
    }

    if (flags & BCM_L2_REPLACE_AGE) {
        if (!soc_feature(unit, soc_feature_l2_bulk_control)) {
            return BCM_E_PARAM;
        }
    }

    /* matching L2 address allowed to be NULL together with */
    /* BCM_L2_REPLACE_DELETE flag indicating to delete all entries. */
    if (NULL == match_addr) {
        if (!(flags & (BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_AGE) ) && 
            !(flags & (BCM_L2_REPLACE_SRC_HIT_CLEAR | BCM_L2_REPLACE_DES_HIT_CLEAR))) {
            return BCM_E_PARAM;
        }
        flags &= ~(BCM_L2_REPLACE_MATCH_VLAN | BCM_L2_REPLACE_MATCH_MAC |
                   BCM_L2_REPLACE_MATCH_DEST);

        if (flags & BCM_L2_REPLACE_VLAN_AND_VPN_TYPE) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_l2_replace(unit, flags, int_flags, match_addr, new_module,
                                     new_port, new_trunk, NULL, NULL));
            flags ^= BCM_L2_REPLACE_VPN_TYPE;
        }
    }
    /* coverity[var_deref_op : FALSE] */
    /* coverity[var_deref_model : FALSE] */
    return _bcm_esw_l2_replace(unit, flags, int_flags, match_addr, new_module,
                               new_port, new_trunk, NULL, NULL);
}

int 
bcm_esw_l2_matched_traverse(int unit, uint32 flags, bcm_l2_addr_t *match_addr, 
                            bcm_l2_traverse_cb trav_fn, void *user_data)
{
    uint32 replace_flags, int_flags;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_matched_traverse(unit, flags, match_addr,
                                                 trav_fn, user_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (soc_feature(unit, soc_feature_ism_memory)) {
        return _bcm_esw_l2_matched_traverse(unit, flags, match_addr, trav_fn,
                                            user_data);
    }

    if (!trav_fn) {
        return BCM_E_PARAM;
    }

    if (!match_addr) {
        return BCM_E_PARAM;
    }

    replace_flags = 0;
    int_flags = _BCM_L2_REPLACE_INT_NO_ACTION;
    /*
    * 1/ If BCM_L2_TRAVERSE_MATCH_DYNAMIC and BCM_L2_TRAVERSE_MATCH_STATIC 
    *     are passed simultaneously to API bcm_l2_matched_traverse, 
    *     this API will match all entries.
    * 2/ If both BCM_L2_TRAVERSE_MATCH_DYNAMIC and BCM_L2_TRAVERSE_MATCH_STATIC 
    *     are not passed to API bcm_l2_matched_traverse,
    *     this API will also match all entries as before. This is the legacy behavior.
    * 3/  If only BCM_L2_TRAVERSE_MATCH_DYNAMIC is passed to 
    *     API bcm_l2_matched_traverse, this API will only match dynamic entries.
    * 4/ If only BCM_L2_TRAVERSE_MATCH_STATIC is passed to 
    *     API bcm_l2_matched_traverse, this API will only match static entries.  
    */
    if (((flags & BCM_L2_TRAVERSE_MATCH_STATIC)?1:0) ==
        ((flags & BCM_L2_TRAVERSE_MATCH_DYNAMIC)?1:0)) {
        /* BCM_L2_REPLACE_MATCH_STATIC means to replace static as well as
       * non-static entries */
        replace_flags |= BCM_L2_REPLACE_MATCH_STATIC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_MAC) {
        replace_flags |= BCM_L2_REPLACE_MATCH_MAC;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_VLAN) {
        replace_flags |= BCM_L2_REPLACE_MATCH_VLAN;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_DEST) {
        replace_flags |= BCM_L2_REPLACE_MATCH_DEST;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_DISCARD_SRC)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_DISCARD_SRC;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_DES_HIT)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_DES_HIT;
    }
    if (!(flags & BCM_L2_TRAVERSE_IGNORE_SRC_HIT)) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_SRC_HIT;
    }
    if (flags & BCM_L2_TRAVERSE_MATCH_NATIVE) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_NATIVE;
    }

    if ((flags & BCM_L2_TRAVERSE_MATCH_STATIC) && 
         !(flags & BCM_L2_TRAVERSE_MATCH_DYNAMIC)){
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_ONLY_STATIC;
    }

    if (flags & BCM_L2_TRAVERSE_MATCH_MESH) {
        int_flags |= _BCM_L2_REPLACE_INT_MATCH_MESH;
    }

    return _bcm_esw_l2_replace(unit, replace_flags, int_flags, match_addr, 0,
                               0, 0, trav_fn, user_data);
}

/*
 * Function:
 *     bcm_esw_l2_station_add
 * Description:
 *     Add an entry to L2 Station Table      
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN/OUT) Station ID
 *     station      - (IN) Pointer to station address information
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_add(int unit, int *station_id, bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_add(unit, station_id, station);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */


#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) ||  SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_add(unit, station_id, station);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_delete
 * Description:
 *     Delete an entry from L2 Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_delete(int unit, int station_id)
{

    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_delete(unit, station_id);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_delete(unit, station_id);
    }
#endif
    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_delete_all
 * Description:
 *     Clear all L2 Station Table entries
 * Parameters:
 *     unit - (IN) BCM device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_delete_all(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_delete_all(unit);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_get
 * Description:
 *     Get L2 station entry detail from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station ID
 *     station      - (OUT) Pointer to station address information.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_get(int unit, int station_id, bcm_l2_station_t *station)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_get(unit, station_id, station);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_get(unit, station_id, station);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_size_get
 * Description:
 *     Get size of L2 Station Table
 * Parameters:
 *     unit - (IN) BCM device number
 *     size - (OUT) L2 Station table size
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_station_size_get(int unit, int *size)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_size_get(unit, size);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_size_get(unit, size);
    }
#endif

    return (rv);
}

/*
 * Function:
 *     bcm_esw_l2_station_traverse
 * Description:
 *     Traverse L2 station entry from Station Table
 * Parameters:
 *     unit         - (IN) BCM device number
 *     trav_fn   -  (IN) Customer defines function is used to traverse
 *     user_data - (IN) Pointer to information of customer.
 * Return:
 *     BCM_E_XXX
 */
int 
bcm_esw_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn, void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_station_traverse(unit, trav_fn, user_data);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)
        || SOC_IS_KATANAX(unit) || SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT3X(unit)
        || soc_feature(unit, soc_feature_gh2_my_station)) {
        rv = bcm_tr_l2_station_traverse(unit, trav_fn, user_data);
    }
#endif

    return (rv);
}

#if defined(BCM_MONTEREY_SUPPORT)
STATIC int
_bcm_mn_l2_ring_replace_data_mask_setup(int unit,
                                        bcm_gport_t port,
                                        _bcm_l2_replace_t *rep,
                                        bcm_l2_ring_t *l2_ring)
{
    int                 vp_id;
    int                 field_len;
    bcm_mac_t           mac_data;
    bcm_mac_t           mac_mask;
    bcm_port_t          port_out;
    bcm_trunk_t         trunk_id;
    bcm_module_t        mod_out;

    sal_memset(&rep->match_mask, 0, sizeof(l2x_entry_t));
    sal_memset(&rep->match_data, 0, sizeof(l2x_entry_t));

    rep->match_dest.vp = -1;
    rep->match_dest.trunk = -1;
    rep->match_dest.port = -1;
    rep->match_dest.module = -1;

    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, BASE_VALIDf, 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, BASE_VALIDf, 1);
    } else if (soc_mem_field_valid(unit, L2Xm, VALIDf)) {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VALIDf, 1);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, VALIDf, 1);
    }

    if (rep->flags & BCM_L2_REPLACE_VPN_TYPE) {
        field_len = soc_mem_field_length(unit, L2Xm, VFIf);
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VFIf,
                            l2_ring->vpn_mask & ((1 << field_len) - 1));
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, VFIf,
                            l2_ring->vpn & ((1 << field_len) - 1));
        rep->key_type = 3;
    } else {
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, VLAN_IDf,
                            l2_ring->vlan_mask & 0xFFF);
        soc_mem_field32_set(unit, L2Xm, &rep->match_data, VLAN_IDf,
                            l2_ring->vlan & 0xFFF);
        rep->key_type = 0;
    }

    field_len = soc_mem_field_length(unit, L2Xm, KEY_TYPEf);
    soc_mem_field32_set(unit, L2Xm, &rep->match_mask, KEY_TYPEf,
                        (1 << field_len) - 1);
    soc_mem_field32_set(unit, L2Xm, &rep->match_data, KEY_TYPEf,
                        rep->key_type);

    if (rep->flags & BCM_L2_REPLACE_MATCH_DEST) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &mod_out,
                                    &port_out, &trunk_id, &vp_id));
        rep->match_dest.module = mod_out;
        if (-1 != vp_id) {
            rep->match_dest.vp = vp_id;
        } else if (BCM_TRUNK_INVALID != trunk_id) {
            rep->match_dest.trunk = trunk_id;
        } else if ((-1 != port_out) && (-1 != mod_out)) {
            rep->match_dest.port = port_out;
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "MATCH DEST Mod[%d] VP[%d][%d] Trunk[%d][%d] "\
                                "Port[%d][%d]\n"),
                     rep->match_dest.module,
                     rep->match_dest.vp, vp_id,
                     rep->match_dest.trunk, trunk_id,
                     rep->match_dest.port, port_out));

        if (rep->match_dest.vp != -1) {
            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DEST_TYPEf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, DEST_TYPEf, 2);

            field_len = soc_mem_field_length(unit, L2Xm, DESTINATIONf);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DESTINATIONf,
                                (1 << field_len) - 1);
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, DESTINATIONf,
                                rep->match_dest.vp);
        } else {
            field_len = soc_mem_field_length(unit, L2Xm, DEST_TYPEf);
            soc_mem_field32_set(unit, L2Xm, &rep->match_mask, DEST_TYPEf,
                                (1 << field_len) - 1);

            if (rep->match_dest.trunk != -1) {
                soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                    DEST_TYPEf, 1);

                field_len = soc_mem_field_length(unit, L2Xm, TGIDf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask, TGIDf,
                                    (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data, TGIDf,
                                    rep->match_dest.trunk);
            } else {
                /* DEST_TYPE field in data is 0 */
                field_len = soc_mem_field_length(unit, L2Xm, MODULE_IDf);
                soc_mem_field32_set(unit, L2Xm, &rep->match_mask,
                                    MODULE_IDf, (1 << field_len) - 1);
                soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                    MODULE_IDf, rep->match_dest.module);
                if (rep->match_dest.port != -1) {
                    field_len = soc_mem_field_length(unit, L2Xm, PORT_NUMf);
                    soc_mem_field32_set(unit, L2Xm, &rep->match_mask,
                                        PORT_NUMf, (1 << field_len) - 1);
                    soc_mem_field32_set(unit, L2Xm, &rep->match_data,
                                        PORT_NUMf, rep->match_dest.port);
                }
            }
        }
    }

    /*
     * By default flush both static and non static entries.
     * BCM_L2_REPLACE_DYNAMIC to flush only dynamic entries.
     * BCM_L2_REPLACE_MATCH_STATIC to flush only static entries.
     */
    if ((rep->flags & BCM_L2_REPLACE_MATCH_STATIC) ||
        (rep->flags & BCM_L2_REPLACE_DYNAMIC)) {
        if (rep->flags & BCM_L2_REPLACE_MATCH_STATIC) {
            soc_mem_field32_set(unit, L2Xm, &rep->match_data, STATIC_BITf, 1);
        }
        soc_mem_field32_set(unit, L2Xm, &rep->match_mask, STATIC_BITf, 1);
    }

    if (rep->flags & BCM_L2_REPLACE_MATCH_UC) {
        /* The 40th bit of Unicast must be 0 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    } else if (rep->flags & BCM_L2_REPLACE_MATCH_MC) {
        /* The 40th bit of Multicast must be 1 */
        sal_memset(&mac_mask, 0x00, sizeof(mac_mask));
        mac_mask[0] = 0x01;
        sal_memset(&mac_data, 0x00, sizeof(mac_data));
        mac_data[0] = 0x01;
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_mask, MAC_ADDRf,
                             mac_mask);
        soc_mem_mac_addr_set(unit, L2Xm, &rep->match_data, MAC_ADDRf,
                             mac_data);
    }

    return BCM_E_NONE;
}

STATIC int
bcm_mn_l2_ring_replace(int unit, bcm_l2_ring_t *l2_ring)
{
    uint32              valid_flags;
    _bcm_l2_replace_t   rep_st;

    L2_INIT(unit);

    if (l2_ring == NULL) {
        return BCM_E_PARAM;
    }

    /* Port should be GPORT */
    if (((l2_ring->port0 != BCM_GPORT_INVALID) &&
        !BCM_GPORT_IS_SET(l2_ring->port0)) ||
        ((l2_ring->port1 != BCM_GPORT_INVALID) &&
        !BCM_GPORT_IS_SET(l2_ring->port1))) {
        return BCM_E_PARAM;
    }

    /* Flag Validation Check */
    if (((l2_ring->flags & BCM_L2_REPLACE_DYNAMIC) &&
         (l2_ring->flags & BCM_L2_REPLACE_MATCH_STATIC)) ||
        ((l2_ring->flags & BCM_L2_REPLACE_MATCH_UC) &&
         (l2_ring->flags & BCM_L2_REPLACE_MATCH_MC))) {
        return BCM_E_PARAM;
    }

    /* API to support only below flags */
    valid_flags = BCM_L2_REPLACE_PROTECTION_RING | BCM_L2_REPLACE_DELETE;
    valid_flags |= BCM_L2_REPLACE_NO_CALLBACKS | BCM_L2_REPLACE_VPN_TYPE;
    valid_flags |= BCM_L2_REPLACE_MATCH_DEST;
    valid_flags |= BCM_L2_REPLACE_MATCH_STATIC | BCM_L2_REPLACE_DYNAMIC;
    valid_flags |= BCM_L2_REPLACE_MATCH_UC | BCM_L2_REPLACE_MATCH_MC;

    if (l2_ring->flags & (~valid_flags)) {
        return BCM_E_PARAM;
    }

    sal_memset(&rep_st, 0, sizeof(_bcm_l2_replace_t));

    rep_st.trav_fn = NULL;
    rep_st.user_data = NULL;
    rep_st.flags = l2_ring->flags;
    rep_st.flags |= BCM_L2_REPLACE_DELETE;
    if ((l2_ring->port0 != BCM_GPORT_INVALID) ||
        (l2_ring->port1 != BCM_GPORT_INVALID)) {
        rep_st.flags |= BCM_L2_REPLACE_MATCH_DEST;
    }

    /* PORT0 valid or both ports not valid */
    if ((l2_ring->port0 != BCM_GPORT_INVALID) ||
        ((l2_ring->port0 == BCM_GPORT_INVALID) &&
         (l2_ring->port1 == BCM_GPORT_INVALID))) {
        BCM_IF_ERROR_RETURN
            (_bcm_mn_l2_ring_replace_data_mask_setup(unit,
                                                     l2_ring->port0,
                                                     &rep_st, l2_ring));
        BCM_IF_ERROR_RETURN
            (_bcm_l2_replace_by_hw(unit, 0, &rep_st));

        if (!(rep_st.flags & BCM_L2_REPLACE_DYNAMIC)) {
            BCM_IF_ERROR_RETURN
                (_soc_l2_sync_mem_cache(unit, &rep_st));
        }
    }

    /* PORT 1 valid */
    if (l2_ring->port1 != BCM_GPORT_INVALID) {
        BCM_IF_ERROR_RETURN
            (_bcm_mn_l2_ring_replace_data_mask_setup(unit,
                                                     l2_ring->port1,
                                                     &rep_st, l2_ring));
         BCM_IF_ERROR_RETURN
            (_bcm_l2_replace_by_hw(unit, 0, &rep_st));

        if (!(rep_st.flags & BCM_L2_REPLACE_DYNAMIC)) {
            BCM_IF_ERROR_RETURN
                (_soc_l2_sync_mem_cache(unit, &rep_st));
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_MONTEREY_SUPPORT */

int
bcm_esw_l2_ring_replace( int unit, bcm_l2_ring_t *l2_ring)
{
    int rv = BCM_E_UNAVAIL;

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_failover) &&
        (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit))) {
         return bcm_tr3_l2_ring_replace(unit, l2_ring);
    }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_KATANA2_SUPPORT */

#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit)) {
         return bcm_mn_l2_ring_replace(unit, l2_ring);
    }
#endif /* BCM_MONTEREY_SUPPORT */

    return rv;
}

/*
 * Function:
 *     bcm_esw_l2_mac_port_create
 * Description:
 *     Create a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port - (IN/OUT) MAC port info
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_mac_port_create(int unit, bcm_l2_mac_port_t *mac_port)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_l2_mac_port_destroy
 * Description:
 *     Destroy a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port_id - (IN) MAC port GPORT ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_mac_port_destroy(int unit, bcm_gport_t mac_port_id)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_l2_mac_port_destroy_all
 * Description:
 *     Destroy all MAC ports.
 * Parameters:
 *     unit - (IN) BCM device number
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_mac_port_destroy_all(int unit)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_l2_mac_port_get
 * Description:
 *     Get info about a MAC port.
 * Parameters:
 *     unit - (IN) BCM device number
 *     mac_port - (IN/OUT) MAC port info
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_mac_port_get(int unit, bcm_l2_mac_port_t *mac_port)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcm_esw_l2_mac_port_traverse
 * Description:
 *     Traverse MAC ports.
 * Parameters:
 *     unit - (IN) BCM device number
 *     cb   - (IN) Callback
 *     user_data - (IN) User data
 * Return:
 *     BCM_E_XXX
 */
int
bcm_esw_l2_mac_port_traverse(int unit, bcm_l2_mac_port_traverse_cb cb,
        void *user_data)
{
    return BCM_E_UNAVAIL;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE

/*
 * Function:
 *     _bcm_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 * Note:
 *    The l2_data structure maintains registration of callbacks routines.
 *    Dump of l2_data is skipped.
 */
void
_bcm_l2_sw_dump(int unit)
{

    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information L2 - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Initialized : %d\n"), _l2_init[unit]));

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        _bcm_tr_l2_sw_dump(unit); 
    }
#endif

#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FB_FX_HX(unit)) {
        _bcm_fb_l2_sw_dump(unit);
    }
#endif
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

STATIC void
_bcm_esw_l2_vlan_gport_free(int unit, _bcm_l2_del_vlan_port_pair_t *l2_pair)
{
    int i;
    
    if (l2_pair) {
        if (l2_pair->mod_pair_arr) {
            sal_free(l2_pair->mod_pair_arr);
            l2_pair->mod_pair_arr = NULL;
        } 

        if (l2_pair->unit_pair_arr) {
            for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
                sal_free(l2_pair->unit_pair_arr->tbm[i]);
                l2_pair->unit_pair_arr->tbm[i] = NULL;
            }
            sal_free(l2_pair->unit_pair_arr);
            l2_pair->unit_pair_arr = NULL;
        }

        if (soc_feature(unit, soc_feature_vlan_vp)) {
            if (l2_pair->vp_pair_arr) {
                for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
                    sal_free(l2_pair->vp_pair_arr->vpbm[i]);
                    l2_pair->vp_pair_arr->vpbm[i] = NULL;
                }
                sal_free(l2_pair->vp_pair_arr);
                l2_pair->vp_pair_arr = NULL;
            }
        }

        if (l2_pair->vfi_pair_arr) {
            for (i = 0; i < soc_mem_index_count(unit,VFIm); i++) {
                if (l2_pair->vfi_pair_arr[i]) {
                    sal_free(l2_pair->vfi_pair_arr[i]);
                    l2_pair->vfi_pair_arr[i] = NULL;
                }
            }
            sal_free(l2_pair->vfi_pair_arr);
            l2_pair->vfi_pair_arr = NULL;
        }

        if (l2_pair->del_by_vfi_vec) {
            sal_free(l2_pair->del_by_vfi_vec);
            l2_pair->del_by_vfi_vec = NULL;
        }
    }
    return;
}

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
/*
 * Function:
 *     _bcm_is_gport_valid_vp_type
 * Purpose:
 *     Returns TRUE if supplied gport is a valid VP type
 * Parameters:
 *     gport_type    -  (IN) Gport type to be validated
 * Returns:
 *     TRUE if 'gport' is a valid VP type, FALSE otherwise
 */

int _bcm_is_gport_valid_vp_type(int gport_type)
{
    switch(gport_type) {
        case BCM_GPORT_MPLS_PORT:
        case BCM_GPORT_SUBPORT_GROUP:
        case BCM_GPORT_SUBPORT_PORT:
        case BCM_GPORT_MIM_PORT:
        case BCM_GPORT_VLAN_PORT:
        case BCM_GPORT_TRILL_PORT:
        case BCM_GPORT_NIV_PORT:
        case BCM_GPORT_L2GRE_PORT:
        case BCM_GPORT_VXLAN_PORT:
        case BCM_GPORT_EXTENDER_PORT:
            return 1;
        default:
            return 0;
    }
}

/*
 * Function:
 *     _bcm_is_gport_vp_in_use
 * Purpose:
 *     Returns TRUE if supplied gport-VP type is in use
 * Parameters:
 *     unit       -  (IN) Unit number
 *     gport      -  (IN) Gport to check if in use
 * Returns:
 *     TRUE if 'gport-vp' is a valid and in use, FALSE otherwise.
 */
int _bcm_is_gport_vp_in_use(int unit, int gport)
{
    int vp;

    /* Extract the vp */
    if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        vp = _SHR_GPORT_MPLS_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeMpls);
    } else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        vp = _SHR_GPORT_MIM_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeMim);
    } else if (BCM_GPORT_IS_L2GRE_PORT(gport)) {
        vp = _SHR_GPORT_L2GRE_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeL2Gre);
    } else if (BCM_GPORT_IS_SUBPORT_GROUP(gport)){
        vp = _SHR_GPORT_SUBPORT_GROUP_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeSubport);
    } else if (BCM_GPORT_IS_WLAN_PORT(gport)) {
        vp = _SHR_GPORT_WLAN_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeWlan);
    } else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        vp = _SHR_GPORT_VLAN_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeVlan);
    } else if (BCM_GPORT_IS_TRILL_PORT(gport)) {
        vp = _SHR_GPORT_TRILL_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeTrill);
    } else if (BCM_GPORT_IS_NIV_PORT(gport)) {
        vp = _SHR_GPORT_NIV_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeNiv);
    } else if (BCM_GPORT_IS_EXTENDER_PORT(gport)) {
        vp = _SHR_GPORT_EXTENDER_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeExtender);
    } else if (BCM_GPORT_IS_VXLAN_PORT(gport)) {
        vp = _SHR_GPORT_VXLAN_PORT_ID_GET(gport);
        return _bcm_vp_used_get(unit, vp, _bcmVpTypeVxlan);
    } else {
        return 0;
    }
}

#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */

/*
 * Function:
 *     _bcm_esw_l2_vlan_gport_matched_delete
 * Purpose:
 *     L2 Traverse callback function to Purge L2 entry belongs
 *     to set of (vlan,port) tuple.
 * Parameters:
 *     unit       -  (IN) Device unit number
 *     info       -  (IN) Reference to L2 entry, delete on match
 *     user_data  -  (IN) User specified cookie
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     If one of the elements of the (vlan,gport) pair is invalid
 *     (if vlan value is BCM_VLAN_INVALID or port value is BCM_GPORT_INVALID),
 *     it is a wildcard. 
 */


int
_bcm_esw_l2_vlan_gport_matched_delete(int unit, bcm_l2_addr_t *info, 
                                      void *user_data)
{
    int                            rv = BCM_E_NONE;
    int                            entry_found=FALSE;
    _bcm_l2_del_vlan_port_pair_t  *l2_pair;
    int                            useGport;
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    int                            gport_type;
    bcm_port_t                     local_port;
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */
    int vfi_num;

    l2_pair = (void *)user_data;

    if (l2_pair == NULL) {
        return BCM_E_NOT_FOUND;
    }

    if ((info->flags & BCM_L2_STATIC) &&  
        (!(l2_pair->replace_flags & BCM_L2_REPLACE_MATCH_STATIC))) {
        _l2_dbg_unmatched_entries[unit]++;  
        return BCM_E_NONE;
    }

    if ((info->flags & BCM_L2_PENDING) &&  
        (!(l2_pair->replace_flags & BCM_L2_REPLACE_PENDING))) {
        _l2_dbg_unmatched_entries[unit]++;
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &useGport));

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
    /* Extract the gport_type */
    gport_type = (((info->port) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK);
    if (_bcm_is_gport_valid_vp_type(gport_type)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_local_get(unit, info->port, &local_port));
    }
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */
    if (_BCM_VPN_VFI_IS_SET(info->vid)) {
        _BCM_VPN_GET(vfi_num, _BCM_VPN_TYPE_VFI, info->vid);
        if (l2_pair->del_by_vfi_vec &&
               (SHR_BITGET(l2_pair->del_by_vfi_vec, vfi_num))) {
            entry_found = TRUE;
        }
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
        else if (_bcm_is_gport_valid_vp_type(gport_type)) {
            if (l2_pair->vfi_pair_arr &&
                    (vfi_num < soc_mem_index_count(unit,VFIm)) &&
                    l2_pair->vfi_pair_arr[vfi_num]) {
                if (SHR_BITGET(l2_pair->vfi_pair_arr[vfi_num], local_port)) {
                    entry_found = TRUE;
                }
            }
        }
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */

    } else if (BCM_VLAN_VEC_GET(l2_pair->del_by_vlan_vec, info->vid)) {
        /* Delete by vlan operation */
        entry_found = TRUE;
    } else if ((useGport) && (BCM_GPORT_IS_MODPORT(info->port)) &&
               BCM_PBMP_MEMBER (l2_pair->mod_pair_arr[info->modid].pbm[BCM_VLAN_MAX+1],
                                _SHR_GPORT_LOCAL_GET(info->port))) {
        /* Delete by (mod, gport) operation */
        entry_found = TRUE;
    } else if ((!useGport) && (!BCM_GPORT_IS_SET(info->port)) &&
               BCM_PBMP_MEMBER (l2_pair->mod_pair_arr[info->modid].pbm[BCM_VLAN_MAX+1],
                                info->port)) {
        /* Delete by (mod, port) operation */
        entry_found = TRUE;
    } else if ((useGport) && BCM_GPORT_IS_MODPORT(info->port) &&
               BCM_PBMP_MEMBER(l2_pair->mod_pair_arr[info->modid].pbm[info->vid],
                _SHR_GPORT_LOCAL_GET(info->port))) {
        /* Delete by (vlan, (mod, gport)) operation */
        entry_found = TRUE;
    } else if (((!useGport) && (!BCM_GPORT_IS_SET(info->port)) &&
                BCM_PBMP_MEMBER(l2_pair->mod_pair_arr[info->modid].pbm[info->vid],
                info->port))) {
        /* Delete by (vlan, (mod, port)) operation */
        entry_found = TRUE;
    } else if (info->flags & BCM_L2_TRUNK_MEMBER) {
        if (SHR_BITGET(l2_pair->unit_pair_arr[0].tbm[BCM_VLAN_MAX+1], info->tgid)) {
            /* Delete by (tid) operation */
            entry_found = TRUE;
        } else if (SHR_BITGET(l2_pair->unit_pair_arr[0].tbm[info->vid], info->tgid)) {
            /* Delete by (vlan, tid) operation */
            entry_found = TRUE;
        }
    } else { 

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
        if (_bcm_is_gport_valid_vp_type(gport_type)) {
            if (SHR_BITGET(l2_pair->vp_pair_arr[0].vpbm[BCM_VLAN_MAX+1],
                           local_port)) {
                /* Delete by (vp) operation */
                entry_found = TRUE;
            } else if (SHR_BITGET(l2_pair->vp_pair_arr[0].vpbm[info->vid], 
                                  local_port)) {
                /* Delete by (vlan, vp) operation */
                entry_found = TRUE;
            }
        }
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */
    }

    if (entry_found == TRUE) {
       /* Entry found - Delete the entry */
#if defined(BCM_TRIUMPH3_SUPPORT)
       if (soc_feature(unit, soc_feature_ism_memory)) {
           rv = bcm_tr3_l2_addr_delete(unit, info->mac, info->vid);
       } else
#endif /* BCM_TRIUMPH3_SUPPORT */    
       rv = mbcm_driver[unit]->mbcm_l2_addr_delete(unit, info->mac,
                                                   info->vid);
       if (rv == BCM_E_NONE) {
           _l2_dbg_matched_entries[unit]++;
       }
   } else {
       _l2_dbg_unmatched_entries[unit]++;
   }

   return rv;
}

/*
 * Function:
 *     bcm_esw_l2_addr_delete_by_vlan_gport_multi
 * Purpose:
 *     Purge L2 entries belong to set of (vlan,port) tuple.
 * Parameters:
 *     unit       -  (IN) Device unit number
 *     flags      -  (IN) Only accept BCM_L2_REPLACE_DELETE, BCM_L2_REPLACE_NO_CALLBACKS.
 *     num_pairs  -  (IN) Size of (vlan,port) pair array.
 *     vlan       -  (IN) Reference to array of VLAN list.
 *     gport      -  (IN) Reference to array of gport list.
 * Returns:
 *     BCM_E_XXX
 * Note:
 *     If one of the elements of the (vlan,gport) pair is invalid
 *     (if vlan value is BCM_VLAN_INVALID or port value is BCM_GPORT_INVALID),
 *     it is a wildcard.
 *     Entries are removed only if BCM_L2_REPLACE_DELETE flag is used. With
 *     flag BCM_L2_REPLACE_NO_CALLBACKS, l2 aging and learning are disabled 
 *     during this operation.
 */

int 
bcm_esw_l2_addr_delete_by_vlan_gport_multi(int unit, uint32 flags, 
                                           int num_pairs, bcm_vlan_t *vlan,
                                           bcm_gport_t *gport)
{
    sal_usecs_t                   interval=0;
    bcm_module_t                  mod_in, modid = 0;
    bcm_port_t                    port_in, port = 0;
    bcm_trunk_t                   trunk_id = 0;
    int                           id = 0, ct, i;
    int                           rv2 = 0, rv = BCM_E_UNAVAIL;
    soc_control_t                 *soc = SOC_CONTROL(unit);
    uint32                        max_modid;
    _bcm_l2_vlan_port_pair_t      *mod_l2_pair_arr = NULL;
    _bcm_l2_vlan_trunk_pair_t     *l2_trunk_pair_arr = NULL;
    _bcm_l2_vlan_vp_pair_t        *l2_vp_pair_arr = NULL;
    _bcm_l2_del_vlan_port_pair_t  l2_pair;
    int                           alloc_size;
    int                           fifo_modified = FALSE;
    int                           vfi_num; 
    int                           vfi_max;
    uint32                        rval = 0;
    uint32                        l2_delete = 0;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_addr_delete_by_vlan_gport_multi(unit, flags,
                                                                num_pairs, vlan,
                                                                gport);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    /* Validate the input parameters */
    if (vlan == NULL || gport == NULL || num_pairs == 0) {
        return BCM_E_PARAM;
    }

    /* Validate flag parameter, no action will be proceeded with extra flags */
    if ((flags & ~(BCM_L2_REPLACE_DELETE | BCM_L2_REPLACE_NO_CALLBACKS | 
                   BCM_L2_REPLACE_MATCH_STATIC |
                   BCM_L2_REPLACE_PENDING )) != 0) {
        return BCM_E_NONE;
    }

    if (SOC_MEM_IS_VALID(unit, VFIm)) {
        vfi_max = soc_mem_index_count(unit,VFIm);
    } else {
        vfi_max = 0;
    }
    sal_memset(&l2_pair, 0x0, sizeof(l2_pair));
    
    /* Prepare (vlan, (mod, port)) pair */
    max_modid = SOC_MODID_MAX(unit) + 1;  
    alloc_size = sizeof(_bcm_l2_vlan_port_pair_t) * max_modid;
    mod_l2_pair_arr = sal_alloc(alloc_size, "l2_addr_vlan_gport_multi_delete");
    if (mod_l2_pair_arr == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(mod_l2_pair_arr, 0x0, alloc_size);

    /* Prepare (vlan, tid) pair */
    l2_pair.mod_pair_arr = mod_l2_pair_arr;
    alloc_size = sizeof(_bcm_l2_vlan_trunk_pair_t);
    l2_trunk_pair_arr = sal_alloc(alloc_size, "l2_addr_vlan_gport_multi_delete");
    if (l2_trunk_pair_arr == NULL) {
        rv = BCM_E_MEMORY;
        goto _free_exit;
    }
    sal_memset(l2_trunk_pair_arr, 0x0, alloc_size);
    l2_pair.unit_pair_arr = l2_trunk_pair_arr;
    
    alloc_size = SHR_BITALLOCSIZE(_bcm_esw_trunk_group_num(unit));
    for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
        l2_trunk_pair_arr->tbm[i] = sal_alloc(alloc_size, \
            "l2_addr_vlan_gport_multi_delete");
        if (l2_trunk_pair_arr->tbm[i] == NULL) {
            rv = BCM_E_MEMORY;
            goto _free_exit;
        }
        sal_memset(l2_trunk_pair_arr->tbm[i], 0x0, alloc_size);
    }

    if (soc_feature(unit, soc_feature_vlan_vp)) {
        /* Prepare (vlan, vp) pair */
        alloc_size = sizeof(_bcm_l2_vlan_vp_pair_t);
        l2_vp_pair_arr = sal_alloc(alloc_size, "l2_addr_vlan_gport_multi_delete");
        if (l2_vp_pair_arr == NULL) {
            rv = BCM_E_MEMORY;
            goto _free_exit;
        }
        sal_memset(l2_vp_pair_arr, 0x0, alloc_size);
        l2_pair.vp_pair_arr = l2_vp_pair_arr;

        alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, SOURCE_VPm));
        for (i = 0; i < BCM_VLAN_MAX + 2; i++) {
            l2_vp_pair_arr->vpbm[i] = sal_alloc(alloc_size, \
                "l2_addr_vlan_gport_multi_delete");
            if (l2_vp_pair_arr->vpbm[i] == NULL) {
                rv = BCM_E_MEMORY;
                goto _free_exit;
            }
            sal_memset(l2_vp_pair_arr->vpbm[i], 0x0, alloc_size);
        }
    }

    if (SOC_MEM_IS_VALID(unit, VFIm) &&
        SOC_MEM_IS_VALID(unit,SOURCE_VPm)) {
         alloc_size = sizeof(SHR_BITDCL *) *  vfi_max;
         l2_pair.vfi_pair_arr = sal_alloc(alloc_size, 
                       "l2_addr_vlan_gport_multi_delete:vfi_pair_arr");
         if (l2_pair.vfi_pair_arr == NULL) {
             rv = BCM_E_MEMORY;
             goto _free_exit;
         }
         
         sal_memset(l2_pair.vfi_pair_arr, 0x0, alloc_size);

         alloc_size = SHR_BITALLOCSIZE(vfi_max); 
         l2_pair.del_by_vfi_vec = sal_alloc(alloc_size, 
                        "l2_addr_vlan_gport_multi_delete:del_by_vfi_vec");
         if (l2_pair.del_by_vfi_vec == NULL) {
             rv = BCM_E_MEMORY;
             goto _free_exit;
         }
         sal_memset(l2_pair.del_by_vfi_vec, 0x0, alloc_size);
         
    }
         
    /* Validate vlan & gport */
    for (ct = 0; ct < num_pairs; ct++) {
        if (vlan[ct] == BCM_VLAN_INVALID && 
            gport[ct] == BCM_GPORT_INVALID) {
            rv = BCM_E_PARAM;
            goto _free_exit;
        }

        if (vlan[ct] != BCM_VLAN_INVALID) {
            if ((vlan[ct] > BCM_VLAN_MAX) && 
                (!_BCM_VPN_VFI_IS_SET(vlan[ct]))) {
                rv = BCM_E_PARAM;
                goto _free_exit;
            }
        }

        if (_BCM_VPN_VFI_IS_SET(vlan[ct])) {
            if (gport[ct] != BCM_GPORT_INVALID) {
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
                if(!(_bcm_is_gport_vp_in_use(unit, gport[ct]))) {
                    rv = BCM_E_PARAM;
                    goto _free_exit;
                }
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */
            }
        }
        if (gport[ct] != BCM_GPORT_INVALID) { 

            if (BCM_GPORT_IS_MODPORT(gport[ct])) {
                mod_in = BCM_GPORT_MODPORT_MODID_GET(gport[ct]);
                port_in = BCM_GPORT_MODPORT_PORT_GET(gport[ct]);
                rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                           mod_in, port_in, &modid, &port);
                if (BCM_FAILURE(rv)) {
                    goto _free_exit;
                }
            } else {            
                rv = _bcm_esw_gport_resolve(unit, gport[ct],
                                       &modid, &port, &trunk_id, &id);
                if (BCM_FAILURE(rv)) {
                    goto _free_exit;
                }
            
                if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                    rv = _bcm_trunk_id_validate(unit, trunk_id);
                    if (BCM_FAILURE(rv)) {
                        rv = BCM_E_PARAM;
                        goto _free_exit;
                    }
                } else {
                    /* If the VP type is not in use, return error */
#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
                    if(!(_bcm_is_gport_vp_in_use(unit, gport[ct]))) {
                        rv = BCM_E_PARAM;
                        goto _free_exit;
                    }
#else
                    rv = BCM_E_PARAM;
                    goto _free_exit;
#endif /* defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3) */
                }
            }
        }

        if (vlan[ct] == BCM_VLAN_INVALID) {
            if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                SHR_BITSET(l2_pair.unit_pair_arr[0].tbm[BCM_VLAN_MAX+1], \
                    trunk_id);
            } else if (BCM_GPORT_IS_MODPORT(gport[ct])){
                BCM_PBMP_PORT_ADD( \
                    l2_pair.mod_pair_arr[modid].pbm[BCM_VLAN_MAX+1], port);
            } else {
                if (soc_feature(unit, soc_feature_vlan_vp)) {
                    /* VP case, set relevant bit */
                    SHR_BITSET(l2_pair.vp_pair_arr[0].vpbm[BCM_VLAN_MAX+1], \
                        id);
                }
            }
        } else if (gport[ct] == BCM_GPORT_INVALID) {
            if (_BCM_VPN_VFI_IS_SET(vlan[ct])) {
                _BCM_VPN_GET(vfi_num, _BCM_VPN_TYPE_VFI, vlan[ct]);
                if (l2_pair.del_by_vfi_vec == NULL ||
                    vfi_num >= vfi_max) {
                    rv = BCM_E_PARAM;
                    goto _free_exit;
                }
                SHR_BITSET(l2_pair.del_by_vfi_vec,vfi_num); 
            } else {
                BCM_VLAN_VEC_SET(l2_pair.del_by_vlan_vec, vlan[ct]);
            }
        } else {
            if (BCM_GPORT_IS_TRUNK(gport[ct])) {
                SHR_BITSET(l2_pair.unit_pair_arr[0].tbm[vlan[ct]], \
                    trunk_id);                
            } else if (BCM_GPORT_IS_MODPORT(gport[ct])){
                BCM_PBMP_PORT_ADD(l2_pair.mod_pair_arr[modid].pbm[vlan[ct]], \
                    port);
            } else {
                if (_BCM_VPN_VFI_IS_SET(vlan[ct])) {
                    _BCM_VPN_GET(vfi_num, _BCM_VPN_TYPE_VFI, vlan[ct]);
                    if (l2_pair.vfi_pair_arr == NULL ||
                        vfi_num >= vfi_max) {
                        rv = BCM_E_PARAM;
                        goto _free_exit;
                    }
                    if (l2_pair.vfi_pair_arr[vfi_num] == NULL) {
                        alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit,SOURCE_VPm));
                        l2_pair.vfi_pair_arr[vfi_num] = sal_alloc(alloc_size,
                                     "l2_addr_vlan_gport_multi_delete");
                        if (l2_pair.vfi_pair_arr[vfi_num] == NULL) {
                            rv = BCM_E_MEMORY;
                            goto _free_exit;
                        }
                        sal_memset(l2_pair.vfi_pair_arr[vfi_num], 0x0, alloc_size);
                    }    
                    SHR_BITSET(l2_pair.vfi_pair_arr[vfi_num], id);

                } else if (soc_feature(unit, soc_feature_vlan_vp)) {
                    /* Vlan VP case, set relevant bit */
                    SHR_BITSET(l2_pair.vp_pair_arr[0].vpbm[vlan[ct]], \
                        id);
                }
            }
        }
    }

    if (flags & BCM_L2_REPLACE_DELETE) {
        _l2_dbg_matched_entries[unit] = 0;
        _l2_dbg_unmatched_entries[unit] = 0;

        if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
            interval = soc->l2x_interval;
            rv = bcm_esw_l2_addr_freeze(unit);
            if (BCM_FAILURE(rv)) {
                goto _free_exit;
            }

            if (SOC_CONTROL(unit)->l2x_mode == L2MODE_FIFO &&
                soc_l2mod_running(unit, NULL, NULL)) {
                if (SOC_REG_IS_VALID(unit, L2_MOD_FIFO_ENABLEr)) {
                    rv2 = READ_L2_MOD_FIFO_ENABLEr(unit, &rval);
                    if (rv2 < 0) {
                        goto _free_exit;
                    }
                    l2_delete = soc_reg_field_get(unit, L2_MOD_FIFO_ENABLEr, rval,
                                                  L2_DELETEf);
                    rv2 = soc_reg_field32_modify(unit, L2_MOD_FIFO_ENABLEr,
                            REG_PORT_ANY, L2_DELETEf, 0);
                } else {
                    if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                                        L2_MOD_FIFO_ENABLE_L2_DELETEf)) {
                        rv2 = READ_AUX_ARB_CONTROLr(unit, &rval);
                        if (rv2 < 0) {
                            goto _free_exit;
                        }
                        l2_delete = soc_reg_field_get(unit, AUX_ARB_CONTROLr, rval,
                                                      L2_MOD_FIFO_ENABLE_L2_DELETEf);
                        rv2 = soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,
                                REG_PORT_ANY, L2_MOD_FIFO_ENABLE_L2_DELETEf, 0);
                    }
                }

                if (rv2 < 0) {
                    goto _free_exit;
                }
                fifo_modified = TRUE;
            } else if (SOC_CONTROL(unit)->l2x_mode != L2MODE_FIFO) {
                rv2 = soc_l2x_stop(unit);
                if (rv2 < 0) {
                    goto _free_exit;
                }
            }

            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit, "L2 Freeze and stop L2 thread\n")));
        }

        l2_pair.replace_flags   = flags;
        rv = bcm_esw_l2_traverse(unit, _bcm_esw_l2_vlan_gport_matched_delete,
                                 &l2_pair);
        if (rv == BCM_E_NONE) {
            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit,
                                 "Total number of L2 Entries: %d [Deleted:%d Left:%d]\n\r"),
                      _l2_dbg_matched_entries[unit] + _l2_dbg_unmatched_entries[unit],
                      _l2_dbg_matched_entries[unit], _l2_dbg_unmatched_entries[unit]));
        }

        if (flags & BCM_L2_REPLACE_NO_CALLBACKS) {
            LOG_INFO(BSL_LS_BCM_ARL,
                     (BSL_META_U(unit, "L2 Thaw and start L2 thread\n")));

            if (fifo_modified) {
                if (SOC_REG_IS_VALID(unit, L2_MOD_FIFO_ENABLEr)) {
                    rv2 = soc_reg_field32_modify(unit, L2_MOD_FIFO_ENABLEr,
                            REG_PORT_ANY, L2_DELETEf, l2_delete);
                } else {
                    if (soc_reg_field_valid(unit, AUX_ARB_CONTROLr,
                                        L2_MOD_FIFO_ENABLE_L2_DELETEf)) {
                        rv2 = soc_reg_field32_modify(unit, AUX_ARB_CONTROLr,
                                REG_PORT_ANY, L2_MOD_FIFO_ENABLE_L2_DELETEf, l2_delete);
                    }
                }

                if (rv2 < 0) {
                    goto _free_exit;
                }
            } else if (SOC_CONTROL(unit)->l2x_mode != L2MODE_FIFO) {
                rv2 = soc_l2x_start(unit, soc->l2x_flags, interval);
                if (rv2 < 0) {
                    goto _free_exit;
                }
            }

            rv2 = bcm_esw_l2_addr_thaw(unit);
            if (rv2 < 0) {
                goto _free_exit;
            }
        }
    }

_free_exit:
    _bcm_esw_l2_vlan_gport_free(unit, &l2_pair);

    if (rv < 0) {
        return rv;
    } else if (rv2 < 0) {
        return rv2;
    } else {
        return rv;
    }
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *  bcm_esw_l2_change_fields_add
 * Description:
 *	Add a Change L2 fields entry to the egress xlate table.
 * Parameters:
 *	unit - (IN) Device number.
 *  l2_change_fields - (IN)
 *      IN: l2 header info: rewrite L2 fields
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_l2_change_fields_add(
    int unit,
    bcm_l2_change_fields_t *l2_info)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vfi_switched_l2_change_fields)) {
        /* validate input */
        if (NULL == l2_info) {
            return BCM_E_PARAM;
        }

        /* validate opaque object id */
        if (l2_info->change_l2_fields_class_id > 0xffff) {
            return BCM_E_PARAM;
        }

        VLAN_CHK_ID(unit, l2_info->vlan);

        return _bcm_td3_l2_change_fields_add(unit, l2_info);
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/* Function:
 *  bcm_esw_l2_change_fields_delete
 * Description:
 *  Delete the l2 change fields from EVXLT table.
 * Parameters:
 *  unit     - (IN) Device number.
 *  l2_info  - (IN) key fields to be deleted.
 */
int
bcm_esw_l2_change_fields_delete(
    int unit,
    bcm_l2_change_fields_t *l2_info)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    soc_mem_t mem = EGR_VLAN_XLATE_2_DOUBLEm;
    soc_field_t tag_act_f = CHANGE_L2_FIELDS__TAG_ACTION_PROFILE_PTRf;
    soc_field_t tpid_f = CHANGE_L2_FIELDS__TPID_INDEXf;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 return_entry[SOC_MAX_MEM_WORDS];
    uint32 profile_index = 0;
    uint32 tpid_inx = 0;
    int index = 0;
    int action_present=0, action_not_present=0;

    if (soc_feature(unit, soc_feature_vfi_switched_l2_change_fields)) {
        /* validate input */
        if (NULL == l2_info) {
            return BCM_E_PARAM;
        }

        /* validate opaque object id */
        if (l2_info->change_l2_fields_class_id > 0xffff) {
            return BCM_E_PARAM;
        }

        sal_memset(entry, 0, sizeof(entry));

        BCM_IF_ERROR_RETURN(
            _bcm_td3_l2_change_fields_egress_key_set(unit,
                 mem, l2_info, entry));

        /* first free any tag action profile entry */
        BCM_IF_ERROR_RETURN(
            soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
             entry, return_entry, 0));

        profile_index = soc_mem_field32_get(unit, mem,
            return_entry, tag_act_f);

        BCM_IF_ERROR_RETURN(_bcm_td3_sd_tag_action_get(unit,
            profile_index, &action_present, &action_not_present));

        if (profile_index != 0) {
            BCM_IF_ERROR_RETURN(
                _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                 profile_index));
        }

        if ((action_not_present == 0x1) || (action_present == 0x1) ||
            (action_present == 0x4) || (action_present == 0x7)) {
            tpid_inx = soc_mem_field32_get(unit, mem,
                return_entry, tpid_f);

            BCM_IF_ERROR_RETURN(_bcm_fb2_outer_tpid_entry_delete(unit,
                tpid_inx));
        }

        /* now delete entry */
        BCM_IF_ERROR_RETURN(soc_mem_delete(unit,
                 mem, MEM_BLOCK_ANY, entry));

        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT3_SUPPORT */

    return BCM_E_UNAVAIL;
}

/* Function:
 *  bcm_esw_l2_change_fields_get
 * Description:
 *  Retrieve the change l2 entry given the key fields
 * Parameters:
 *  unit    - (IN) Device number.
 *  nat_id  - IN Index of entry to get.
 */
int
bcm_esw_l2_change_fields_get(
    int unit,
    bcm_l2_change_fields_t *l2_info)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    soc_mem_t mem = EGR_VLAN_XLATE_2_DOUBLEm;
    uint32 entry[SOC_MAX_MEM_WORDS];

    if (soc_feature(unit, soc_feature_vfi_switched_l2_change_fields)) {
        /* validate input */
        if (NULL == l2_info) {
            return BCM_E_PARAM;
        }

        /* validate opaque object id */
        if (l2_info->change_l2_fields_class_id > 0xffff) {
            return BCM_E_PARAM;
        }

        sal_memset(entry, 0, sizeof(entry));

        BCM_IF_ERROR_RETURN(
            _bcm_td3_l2_change_fields_egress_key_set(unit,
                 mem, l2_info, entry));

        return _bcm_td3_l2_change_fields_egress_entry_get(unit,
                    mem, entry, l2_info);
    }
#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}

/* Function:
 *  bcm_esw_l2_change_fields_traverse
 *
 * Description:
 *  Traverse all valid change L2 entries in the egress translate table
 *  and call the supplied callback routine for each.
 *
 * Parameters:
 *  unit    - (IN) Device number.
 *  cb      - (IN) User call-back function
 */
int
bcm_esw_l2_change_fields_traverse(
    int unit,
    bcm_l2_change_fields_traverse_cb cb,
    void *user_data)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_feature(unit, soc_feature_vfi_switched_l2_change_fields)) {
        if (NULL == cb) {
            return BCM_E_NONE;
        }

        return _bcm_td3_change_l2_fields_entry_traverse(unit,
                    EGR_VLAN_XLATE_2_DOUBLEm, cb, user_data);
    }

#endif /* BCM_TRIDENT3_SUPPORT */
    return BCM_E_UNAVAIL;
}
#endif /* INCLUDE_L3 */

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_1

/*
 * Function:
 *      _bcm_esw_l2_wb_alloc
 * Purpose:
 *      Allocated L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_l2_wb_alloc(int unit)
{
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int                         sz = 0;
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr;
    int                         rv = BCM_E_NONE;
    int                         stable_size = 0;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         sw_aging_support; /* SW Learning supported */
    int                         underlay_max_entries = 0;
#ifdef BCM_RIOT_SUPPORT
    int                         overlay_max_entries = 0;
#endif /* BCM_RIOT_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    sw_aging_support = SOC_IS_TRIUMPH3(unit);
#ifdef BCM_TOMAHAWK_SUPPORT
    /* TH/TD3 does not support SW aging. Added only for warmboot upgrade purpose. */
    sw_aging_support = sw_aging_support || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    sw_aging_support = sw_aging_support || SOC_CONTROL(unit)->l2x_sw_aging;
#endif /* BCM_TRIDENT2_SUPPORT */

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);

        if (sw_aging_support) {
            /* aging time */
            sz += sizeof(int);
        }

        if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
            /* Underlay L2 station entry - sid/prio */
            BCM_IF_ERROR_RETURN(
                _bcm_tr_l2_station_entry_max_size_get(unit, FALSE,
                                                      &underlay_max_entries));
            sz += (underlay_max_entries * (sizeof(int) * 2));

#ifdef BCM_RIOT_SUPPORT
            /* Overlay L2 station entry - sid/prio */
            BCM_IF_ERROR_RETURN(
                _bcm_tr_l2_station_entry_max_size_get(unit, TRUE,
                                                      &overlay_max_entries));

            sz += (overlay_max_entries * (sizeof(int) * 2));
#endif /* BCM_RIOT_SUPPORT */

        }

        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                     sz, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_l2_wb_sync
 * Purpose:
 *      Record L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_l2_wb_sync(int unit)
{
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int                         sz = 0;
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr = NULL;
    int                         aging_time = 0;
    _bcm_l2_station_entry_t     **entry_arr;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         stable_size = 0;
    int                         rv = BCM_E_NONE;
    int                         index = 0;
    int                         init_value = 0;
    int                         sw_aging_support; /* SW Learning supported */
    int                         underlay_max_entries = 0;
#ifdef BCM_RIOT_SUPPORT
    int                         overlay_max_entries = 0;
#endif /* BCM_RIOT_SUPPORT */
    SOC_SCACHE_DUMP_DECL;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return _bcm_th3_l2_wb_sync(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    sw_aging_support = SOC_IS_TRIUMPH3(unit);
#ifdef BCM_TOMAHAWK_SUPPORT
    /* TH/TD3 does not support SW aging. Added only for warmboot upgrade purpose. */
    sw_aging_support = sw_aging_support || SOC_IS_TOMAHAWKX(unit)  || SOC_IS_TRIDENT3X(unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    sw_aging_support = sw_aging_support || SOC_CONTROL(unit)->l2x_sw_aging;
#endif /* BCM_TRIDENT2_SUPPORT */

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     0, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, NULL);

        if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
            return rv;
        }

        SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L2),
                              scache_ptr);
        if (sw_aging_support) {
            /* aging time */
            rv = bcm_esw_l2_age_timer_get(unit, &aging_time);
            if (BCM_FAILURE(rv)) {
                SOC_SCACHE_DUMP_STOP();
                return rv;
            }
            sz = sizeof(int);
            SOC_SCACHE_DUMP_DATA_BEGIN("aging_time");
            sal_memcpy(scache_ptr, &aging_time, sz);
            SOC_SCACHE_DUMP_DATA_INT32d_V(scache_ptr);
            scache_ptr += sz;
            SOC_SCACHE_DUMP_DATA_END();
        }

        if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
            rv = _bcm_tr_l2_station_entry_max_size_get(unit, FALSE,
                                                       &underlay_max_entries);
            if (BCM_FAILURE(rv)) {
                SOC_SCACHE_DUMP_STOP();
                return rv;
            }
            /* Underlay L2 station entry - sid/prio */
            entry_arr = (station_control->entry_arr);

            SOC_SCACHE_DUMP_DATA_BEGIN("underlay_entry_idx;sid;prio");
            for (index = 0; index < underlay_max_entries; index++) {
                uint8 *scache_ptr_tmp = scache_ptr;
                if (entry_arr[index]) {
                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->sid), sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->prio), sizeof(int));
                    scache_ptr += sizeof(int);
                } else {
                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);
                }
                SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr_tmp, index);
            }
            SOC_SCACHE_DUMP_DATA_END();

#ifdef BCM_RIOT_SUPPORT
            rv = _bcm_tr_l2_station_entry_max_size_get(unit, TRUE,
                                                       &overlay_max_entries);
            if (BCM_FAILURE(rv)) {
                SOC_SCACHE_DUMP_STOP();
                return rv;
            }
            /* Overlay L2 station entry - sid/prio */
            entry_arr = (station_control->entry_arr_2);

            SOC_SCACHE_DUMP_DATA_BEGIN("overlay_entry_idx;sid;prio");
            for (index = 0; index < overlay_max_entries; index++) {
                uint8 *scache_ptr_tmp = scache_ptr;
                if (entry_arr[index]) {
                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->sid), sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr,
                               &(entry_arr[index]->prio), sizeof(int));
                    scache_ptr += sizeof(int);
                } else {
                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);

                    sal_memcpy(scache_ptr, &init_value, sizeof(int));
                    scache_ptr += sizeof(int);
                }
                SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr_tmp, index);
            }
            SOC_SCACHE_DUMP_DATA_END();
#endif /* BCM_RIOT_SUPPORT */
        }
        SOC_SCACHE_DUMP_STOP();
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_l2_wb_recover
 * Purpose:
 *      Recover L2 module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_l2_wb_recover(int unit)
{
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    soc_scache_handle_t         scache_handle;
    uint8                       *scache_ptr = NULL;
    uint16                      recovered_ver = 0;
    int                         aging_time = 0;
    int                         rv = BCM_E_NONE;
    int                         additional_scache_size = 0;
    int                         sw_aging_support; /* SW Learning supported */
#ifdef BCM_RIOT_SUPPORT
    int                         overlay_max_entries = 0;
#endif /* BCM_RIOT_SUPPORT */
    int                         underlay_max_entries = 0;
    _bcm_l2_station_entry_t     **entry_arr;
    _bcm_l2_station_control_t   *station_control = NULL;
    int                         index = 0;
    int                         sid = 0;
    int                         pri = 0;
    int                         cur_last_sid = 0;
    int                         stable_size = 0;
    SOC_SCACHE_DUMP_DECL;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    /* Requires extended scache support warmboot */
    if (stable_size == 0) {
        return BCM_E_NONE;
    }

    rv = _bcm_l2_station_control_get(unit, &station_control);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INIT)) {
        return rv;
    }

    sw_aging_support = SOC_IS_TRIUMPH3(unit);
#ifdef BCM_TOMAHAWK_SUPPORT
    /* TH/TD3 does not support SW aging. Added only for warmboot upgrade purpose. */
    sw_aging_support = sw_aging_support || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIDENT2_SUPPORT
    sw_aging_support = sw_aging_support || SOC_CONTROL(unit)->l2x_sw_aging;
#endif /* BCM_TRIDENT2_SUPPORT */

    if (sw_aging_support ||
        ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control)) {

        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_L2, 0);
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     0, &scache_ptr,
                                     BCM_WB_DEFAULT_VERSION, &recovered_ver);
        if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
            return rv;
        } else if (BCM_E_NOT_FOUND == rv) {
            return _bcm_esw_l2_wb_alloc(unit);
        }

        if (scache_ptr != NULL) {
            SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L2),
                                  scache_ptr);
            if (sw_aging_support) {
                if (recovered_ver >= BCM_WB_VERSION_1_0) {
                    /* aging time */
                    SOC_SCACHE_DUMP_DATA_BEGIN("aging_time");
                    sal_memcpy(&aging_time, scache_ptr, sizeof(int));
                    SOC_SCACHE_DUMP_DATA_INT32d_V(scache_ptr);
                    rv = _bcm_esw_l2_age_timer_set(unit, aging_time, aging_time > 0);
                    if (BCM_FAILURE(rv)) {
                        SOC_SCACHE_DUMP_STOP();
                        return rv;
                    }
                    scache_ptr += sizeof(int);
                    SOC_SCACHE_DUMP_DATA_END();
                } else {
                    additional_scache_size += sizeof(int);
                }
            }

            if ((!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) && station_control) {
                rv = _bcm_tr_l2_station_entry_max_size_get(
                        unit, FALSE, &underlay_max_entries);
                if (BCM_FAILURE(rv)) {
                    SOC_SCACHE_DUMP_STOP();
                    return rv;
                }
#ifdef BCM_RIOT_SUPPORT
                rv = _bcm_tr_l2_station_entry_max_size_get(
                        unit, TRUE, &overlay_max_entries);
                if (BCM_FAILURE(rv)) {
                    SOC_SCACHE_DUMP_STOP();
                    return rv;
                }
#endif /* BCM_RIOT_SUPPORT */

                /* Recover overlay/underlay L2 station entry */
                if (recovered_ver >= BCM_WB_VERSION_1_1) {
                    entry_arr = (station_control->entry_arr);

                    SOC_SCACHE_DUMP_DATA_BEGIN("underlay_entry_idx;sid;prio");
                    for (index = 0; index < underlay_max_entries; index++) {
                        SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr, index);
                        sal_memcpy(&sid, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        sal_memcpy(&pri, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        if (sid > cur_last_sid) {
                            cur_last_sid = sid;
                        }

                        if (entry_arr[index]) {
                            if (0 == sid) {
                                /*
                                 * Only recover the L2 station entries created
                                 * by bcm_l2_station APIs
                                 */
                                if (index < station_control->entries_total) {
                                    if (NULL != entry_arr[index]->tcam_ent) {
                                        sal_free(entry_arr[index]->tcam_ent);
                                    }

                                    sal_free(entry_arr[index]);

                                    entry_arr[index] = NULL;
                                    station_control->entry_count--;
                                    station_control->entries_free++;
                                }
                            } else {
                                entry_arr[index]->sid = sid;
                                entry_arr[index]->prio = pri;
                            }
                        }
                    }
                    SOC_SCACHE_DUMP_DATA_END();

                    rv = _bcm_tr_l2_station_entry_last_sid_set(unit, FALSE,
                                                          cur_last_sid);
                    if (BCM_FAILURE(rv)) {
                        SOC_SCACHE_DUMP_STOP();
                        return rv;
                    }
#ifdef BCM_RIOT_SUPPORT
                    cur_last_sid = 0;

                    entry_arr = (station_control->entry_arr_2);

                    SOC_SCACHE_DUMP_DATA_BEGIN("overlay_entry_idx;sid;prio");
                    for (index = 0; index < overlay_max_entries; index++) {
                        SOC_SCACHE_DUMP_DATA_INT32d_TVV(scache_ptr, index);
                        sal_memcpy(&sid, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        sal_memcpy(&pri, scache_ptr, sizeof(int));
                        scache_ptr += sizeof(int);

                        if (sid > cur_last_sid) {
                            cur_last_sid = sid;
                        }

                        if (entry_arr[index]) {
                            if (0 == sid) {
                                /*
                                 * Only recover the L2 station entries created
                                 * by bcm_l2_station APIs
                                 */
                                if (index < station_control->entries_total_2) {
                                    if (NULL != entry_arr[index]->tcam_ent) {
                                        sal_free(entry_arr[index]->tcam_ent);
                                    }
                                    sal_free(entry_arr[index]);
                                    entry_arr[index] = NULL;
                                    station_control->entry_count_2--;
                                    station_control->entries_free_2++;
                                }
                            } else {
                                entry_arr[index]->sid = sid;
                                entry_arr[index]->prio = pri;
                            }
                        }
                    }
                    SOC_SCACHE_DUMP_DATA_END();

                    rv = _bcm_tr_l2_station_entry_last_sid_set(unit, TRUE,
                                                          cur_last_sid);
                    if (BCM_FAILURE(rv)) {
                        SOC_SCACHE_DUMP_STOP();
                        return rv;
                    }
#endif /* BCM_RIOT_SUPPORT */
                } else {
                    /* Underlay L2 station entry - sid/prio */
                    additional_scache_size +=
                        (underlay_max_entries *(sizeof(int) * 2));
#ifdef BCM_RIOT_SUPPORT
                    /* Overlay L2 station entry - sid/prio */
                    additional_scache_size +=
                        (overlay_max_entries * (sizeof(int) * 2));
#endif /* BCM_RIOT_SUPPORT */
                }
            }

            SOC_SCACHE_DUMP_STOP();

            if (additional_scache_size > 0) {
                rv = soc_scache_realloc(unit, scache_handle,
                                        additional_scache_size);
                if(BCM_FAILURE(rv)) {
                   return rv;
                }
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

