/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        vlan.c
 * Purpose:     Provide low-level access to Triumph VLAN resources
 */
#include <sal/core/boot.h>

#include <soc/defs.h>
#if defined(BCM_TRX_SUPPORT)
#include <soc/drv.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/profile_mem.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/stg.h>
#include <bcm/port.h>
#include <bcm/trunk.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/port.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
#include <bcm_int/esw/policer.h>
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */
#include <bcm_int/esw_dispatch.h>

#if defined(BCM_TRIDENT_SUPPORT)
#include <bcm_int/esw/trident.h>
#endif

#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm/fcoe.h>
#include <bcm_int/esw/trident2.h>
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#include <soc/esw/flow_db.h>
#endif

#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
#include <soc/td2_td2p.h>
#include <soc/format.h>
#endif

#if (defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)) && defined(INCLUDE_L3)
#include <bcm_int/esw/virtual.h>
#endif

#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/tomahawk3.h>
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
#include <bcm_int/esw/greyhound2.h>
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/qos.h>
#endif /* BCM_METROLITE_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
STATIC int
_bcm_fast_egr_vlan_port_egress_default_action_set(
            int unit,
            bcm_port_t port,
            bcm_vlan_action_set_t *action);

STATIC int
_bcm_fast_egr_vlan_port_egress_default_action_get(
            int unit,
            bcm_port_t port,
            bcm_vlan_action_set_t *action);
#endif

/* Cache of Ingress Vlan Translate Action Profile Table */
static soc_profile_mem_t *ing_action_profile[BCM_MAX_NUM_UNITS] = {NULL};
static int ing_action_profile_def[BCM_MAX_NUM_UNITS] = {0};

/* Cache of Engress Vlan Translate Action Profile Table */
static soc_profile_mem_t *egr_action_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define EGR_ACTION_PROFILE_DEFAULT  BCM_TRX_EGR_VLAN_ACTION_PROFILE_DEFAULT

/* Cache of Ingress Vlan Range Profile Table */
static soc_profile_mem_t *vlan_range_profile[BCM_MAX_NUM_UNITS] = {NULL};
#define VLAN_RANGE_PROFILE_DEFAULT  0

typedef struct _bcm_trx_vlan_subnet_entry_s {
    bcm_ip6_t   ip;
    bcm_ip6_t   mask;
    int         prefix;
    bcm_vlan_t  ovid;
    bcm_vlan_t  ivid;
    uint8       opri;
    uint8       ocfi;
    uint8       ipri;
    uint8       icfi;
    int         profile_idx;
}_bcm_trx_vlan_subnet_entry_t;

#if defined(BCM_TRIUMPH3_SUPPORT)
typedef int (*tr3_vxlate_extd_entry_update_func_t)(int unit,
                   vlan_xlate_entry_t *vent_in,
                   void *ctxt,
                   vlan_xlate_entry_t *vent_out,
                   vlan_xlate_extd_entry_t *vxent_out,
                   int *use_extd_tbl);
#endif

#define PKT_VLAN_OUTER_ACTION_SET(unit, mem, vent_ptr, _action) \
   do { \
            if ((_action->ot_outer == bcmVlanActionAdd ) ||\
                (_action->ot_outer == bcmVlanActionReplace ) ||\
                (_action->ot_outer == bcmVlanActionOuterAdd ) ||\
                (_action->dt_outer == bcmVlanActionAdd ) ||\
                (_action->dt_outer == bcmVlanActionReplace ) ||\
                (_action->dt_outer == bcmVlanActionOuterAdd ) ||\
                (_action->it_outer == bcmVlanActionAdd ) ||\
                (_action->it_outer == bcmVlanActionReplace ) ||\
                (_action->it_outer == bcmVlanActionOuterAdd ) ||\
                (_action->ut_outer == bcmVlanActionAdd ) ||\
                (_action->ut_outer == bcmVlanActionReplace )||\
                (_action->ut_outer == bcmVlanActionOuterAdd )) {\
                soc_mem_field32_set(unit, mem, vent_ptr, OPRI_CFI_SELf, 1);\
            }\
   } while(0)

#define PKT_VLAN_INNER_ACTION_SET(unit, mem, vent_ptr, _action) \
   do { \
            if ((_action->ot_inner == bcmVlanActionAdd ) ||\
                (_action->ot_inner == bcmVlanActionReplace ) ||\
                (_action->ot_inner == bcmVlanActionInnerAdd ) ||\
                (_action->dt_inner == bcmVlanActionAdd ) ||\
                (_action->dt_inner == bcmVlanActionReplace ) ||\
                (_action->dt_inner == bcmVlanActionInnerAdd ) ||\
                (_action->it_inner == bcmVlanActionAdd ) ||\
                (_action->it_inner == bcmVlanActionReplace ) ||\
                (_action->it_inner == bcmVlanActionInnerAdd ) ||\
                (_action->ut_inner == bcmVlanActionAdd ) ||\
                (_action->ut_inner == bcmVlanActionReplace )||\
                (_action->ut_inner == bcmVlanActionInnerAdd )) {\
                soc_mem_field32_set(unit, mem, vent_ptr, IPRI_CFI_SELf, 1);\
            }\
   } while(0)

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
#define SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(m_unit, m_mem, m_entry_data, m_field, m_field_data) \
        soc_meminfo_fieldinfo_field_get((uint32*)(m_entry_data), (&SOC_MEM_INFO(m_unit, m_mem)), \
            (m_field), (m_field_data))
#define SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(m_unit, m_mem, m_entry_data, m_field, m_field_data) \
        soc_meminfo_fieldinfo_field_set((uint32*)(m_entry_data), (&SOC_MEM_INFO(m_unit, m_mem)), \
            (m_field), (m_field_data))
#endif

#define ING_VLAN_TAG_ACTION_LOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, ING_VLAN_TAG_ACTION_PROFILEm)) {\
                soc_mem_lock(unit, ING_VLAN_TAG_ACTION_PROFILEm); \
            } else if (soc_mem_is_valid(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {\
                soc_mem_lock(unit, ING_VLAN_TAG_ACTION_PROFILE_2m); \
            }\
        } while (0)

#define ING_VLAN_TAG_ACTION_UNLOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, ING_VLAN_TAG_ACTION_PROFILEm)) {\
                soc_mem_unlock(unit, ING_VLAN_TAG_ACTION_PROFILEm); \
            } else if (soc_mem_is_valid(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {\
                soc_mem_unlock(unit, ING_VLAN_TAG_ACTION_PROFILE_2m); \
            }\
        } while (0)

#define EGR_VLAN_TAG_ACTION_LOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, EGR_VLAN_TAG_ACTION_PROFILEm)) {\
                soc_mem_lock(unit, EGR_VLAN_TAG_ACTION_PROFILEm); \
            } else if (soc_mem_is_valid(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m)) {\
                soc_mem_lock(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m); \
            }\
        } while (0)

#define EGR_VLAN_TAG_ACTION_UNLOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, EGR_VLAN_TAG_ACTION_PROFILEm)) {\
                soc_mem_unlock(unit, EGR_VLAN_TAG_ACTION_PROFILEm); \
            } else if (soc_mem_is_valid(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m)) {\
                soc_mem_unlock(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m); \
            }\
        } while (0)

#define VLAN_RANGE_LOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, ING_VLAN_RANGEm)) {\
                soc_mem_lock(unit, ING_VLAN_RANGEm); \
            }\
        } while (0)

#define VLAN_RANGE_UNLOCK(unit) \
        do {\
            if (soc_mem_is_valid(unit, ING_VLAN_RANGEm)) {\
                soc_mem_unlock(unit, ING_VLAN_RANGEm); \
            }\
        } while (0)

STATIC int _bcm_trx_vlan_range_profile_init(int unit);

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int _bcm_trx_vlan_port_protocol_action_reinit(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function : 
 *   _bcm_tr3_vxlate_extd2vxlate
 *
 * Purpose  : 
 *   convert extended vlan translation table to regular vlan translation
 *   table for all applicable fields.  
 *
 * Parameters:
 *   vxxent  -  (IN) extended vlan translation table 
 *   vxent   -  (OUT) regular vlan translation table
 *   use_svp -  (IN) Is SVP used in vxxent
 */

int
_bcm_tr3_vxlate_extd2vxlate(int unit,
                vlan_xlate_extd_entry_t *vxxent,
                vlan_xlate_entry_t *vxent,
                int use_svp)
{
    uint32 key[2];
    uint32 value;

    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, VALID_0f);
    soc_mem_field32_set(unit, VLAN_XLATEm, vxent, VALIDf,value);

    soc_mem_field_get(unit, VLAN_XLATE_EXTDm, 
                      (uint32 *)vxxent, XLATE__KEY_0f, key);
    soc_mem_field_set(unit, VLAN_XLATEm,
                    (uint32 *)vxent,  KEYf, key);

    /* key type value for VLAN_XLATE_EXTDm: VLAN_XLATE_1 value + 1 */
    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, KEY_TYPE_0f);
    value--;
    soc_VLAN_XLATEm_field32_set(unit, vxent, KEY_TYPEf,value);

    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                    XLATE__TAG_ACTION_PROFILE_PTRf);
    soc_VLAN_XLATEm_field32_set(unit, vxent, 
                               TAG_ACTION_PROFILE_PTRf,value);
    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                    XLATE__NEW_OVIDf);
    soc_VLAN_XLATEm_field32_set(unit, vxent, 
                               NEW_OVIDf,value);
    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                    XLATE__NEW_OPRIf);
    soc_VLAN_XLATEm_field32_set(unit, vxent, 
                               NEW_OPRIf,value);
    value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                    XLATE__NEW_OCFIf);
    soc_VLAN_XLATEm_field32_set(unit, vxent, 
                               NEW_OCFIf,value);

    /* VLAN_XLATE source_vp and new_ivid are overlay fields while
     * VLAN_XLATE_EXTD's are not
     */
    if (use_svp) {
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__SOURCE_VPf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, SOURCE_VPf,value);
        soc_VLAN_XLATEm_field32_set(unit, vxent, SVP_VALIDf,1);
    } else {
        soc_VLAN_XLATEm_field32_set(unit, vxent, SVP_VALIDf,0);
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__NEW_IVIDf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, 
                                   NEW_IVIDf,value);
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__NEW_IPRIf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, 
                                   NEW_IPRIf,value);
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__NEW_ICFIf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, 
                                   NEW_ICFIf,value);
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm,
                            ESM_SEARCH_OFFSETf)) {
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__ESM_SEARCH_OFFSETf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, 
                                   ESM_SEARCH_OFFSETf,value);
    }
    if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm,
                            ESM_SEARCH_PRIORITYf)) {
        value = soc_VLAN_XLATE_EXTDm_field32_get(unit, vxxent, 
                        XLATE__ESM_SEARCH_PRIORITYf);
        soc_VLAN_XLATEm_field32_set(unit, vxent, 
                                   ESM_SEARCH_PRIORITYf,value);
    }

    return BCM_E_NONE;    
}

/*
 * Function :
 *   _bcm_tr3_vxlate2vxlate_extd
 *
 * Purpose  :
 *   convert the regular vlan translation table to the extended vlan translation
 *   table for all applicable fields.
 *
 * Parameters:
 *   vxent   -  (IN) regular vlan translation table
 *   vxxent  -  (OUT) extended vlan translation table
 */

int
_bcm_tr3_vxlate2vxlate_extd(int unit,
                vlan_xlate_entry_t *vxent,
                vlan_xlate_extd_entry_t *vxxent)
{
    uint32 key[2];
    uint32 value;
    int svp_valid;

    value = soc_mem_field32_get(unit, VLAN_XLATEm, vxent, VALIDf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, VALID_0f, value);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, VALID_1f, value);

    soc_mem_field_get(unit, VLAN_XLATEm,
                    (uint32 *)vxent,  KEYf, key);
    soc_mem_field_set(unit, VLAN_XLATE_EXTDm, 
                      (uint32 *)vxxent, XLATE__KEY_0f, key);

    value = soc_VLAN_XLATEm_field32_get(unit, vxent, KEY_TYPEf);

    /* key type value for VLAN_XLATE_EXTDm: VLAN_XLATE_1 value + 1 */
    value++;    
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, KEY_TYPE_0f, value);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, KEY_TYPE_1f, value);

    value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   TAG_ACTION_PROFILE_PTRf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__TAG_ACTION_PROFILE_PTRf, value);
    value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_OVIDf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_OVIDf, value);
    value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_OPRIf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_OPRIf, value);
    value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_OCFIf);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_OCFIf, value);
    svp_valid = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   SVP_VALIDf);
    if (svp_valid) {
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   SOURCE_VPf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                            XLATE__SOURCE_VPf, value);
    } else {
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_IVIDf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_IVIDf, value);
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_IPRIf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_IPRIf, value);
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   NEW_ICFIf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                        XLATE__NEW_ICFIf, value);
    }

    if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm,
                                    ESM_SEARCH_OFFSETf)) {
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   ESM_SEARCH_OFFSETf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                            XLATE__ESM_SEARCH_OFFSETf, value);
    }
    if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm,
                                    ESM_SEARCH_PRIORITYf)) {
        value = soc_VLAN_XLATEm_field32_get(unit, vxent, 
                                   ESM_SEARCH_PRIORITYf);
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxxent, 
                            XLATE__ESM_SEARCH_PRIORITYf, value);
    }
    return BCM_E_NONE;    
}


/*
 * Function :
 *   _bcm_tr3_vxlate_entry_add
 *
 * Purpose  :
 *   Add or update a vlan translation entry to the appropriate table. 
 *   BCM56640 has two type of vlan translation tables, a regular sized 
 *   table VLAN_XLATE and a extended sized table VLAN_XLATE_EXTD. This
 *   function takes a regular sized entry as input, add or update 
 *   either in regular sized table or extended sized table if fields 
 *   only in extended table are used.   
 *
 * Parameters:
 *  vent   -    the regular sized entry to be added/updated
 *  ctxt   -    context used by update_func 
 *  update_func - function determines whether to use regular or extd entry
 */                   
int
_bcm_tr3_vxlate_entry_add(int unit,
                vlan_xlate_entry_t *vent,
                void *ctxt,
                tr3_vxlate_extd_entry_update_func_t update_func,
                int *existing_updated)
{
    vlan_xlate_entry_t vent_old;
    vlan_xlate_extd_entry_t vxent;
    vlan_xlate_extd_entry_t vxent_old;
    vlan_xlate_extd_entry_t vxent_tmp;
    int idx;
    int use_extd_table = 0;
    int rv;
 
    sal_memset(&vent_old, 0, sizeof(vent_old));
    sal_memset(&vxent, 0, sizeof(vxent));
    sal_memset(&vxent_old, 0, sizeof(vxent_old));

    /*
     * first searches the regular entry table VLAN_XLATEm, if a match is found,
     * update and write it back if the entry can be added to this table. 
     * If not, delete this entry and add it to VLAN_XLATE_EXTDm. If no match is
     * found, check VLAN_XLATE_EXTDm, if a match is found, update and write it
     * back, if not, add it to either VLAN_XLATEm or VLAN_XLATE_EXTDm 
     * appropriately. 
     */ 
    
    /* look up existing entry */
    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &idx, vent, 
                        &vent_old, 0);
    soc_mem_unlock(unit, VLAN_XLATEm);

    /* regular entry exist */
    if (rv == SOC_E_NONE) {

        if (update_func) {
            BCM_IF_ERROR_RETURN
                (update_func(unit,vent,ctxt,&vent_old,&vxent_old,
                      &use_extd_table));

            if (use_extd_table) {
                /* remove the existing entry */
                soc_mem_lock(unit, VLAN_XLATEm);
                rv = (soc_mem_delete(unit, VLAN_XLATEm, MEM_BLOCK_ANY, 
                                     &vent_old));
                soc_mem_unlock(unit, VLAN_XLATEm);

                if (rv != SOC_E_NONE) {
                    return rv;
                }
 
                /* search for the extd entry. It should not exist because
                 * we already found a regular one
                 */
                soc_mem_lock(unit, VLAN_XLATE_EXTDm);
                rv = soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                             &idx, &vxent, &vxent_tmp, 0);
                soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
    
                if (rv == SOC_E_NONE) {
                    return BCM_E_EXISTS;  /* XXX ERROR message */
                } else if (rv != SOC_E_NOT_FOUND) {
                    return rv;
                } else { 
                    soc_mem_lock(unit, VLAN_XLATE_EXTDm);

                    /* no extry exists, insert */
                    rv = soc_mem_insert(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, 
                                    &vxent_old);    
                    soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
                    if (existing_updated != NULL) {
                        *existing_updated = 1;
                    }
                    return rv;
                }
            }  /* use_extd_table */
        } /*  update_func */ 

        /* regular entry */
        soc_mem_lock(unit, VLAN_XLATEm);
        rv = soc_mem_write(unit, VLAN_XLATEm,  MEM_BLOCK_ALL, 
                    idx, update_func == NULL? vent: &vent_old);
        soc_mem_unlock(unit, VLAN_XLATEm);
        return BCM_E_NONE;
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {  /* no regular entry found */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate2vxlate_extd(unit,vent,&vxent));
        soc_mem_lock(unit, VLAN_XLATE_EXTDm);
        rv = soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, &idx,
                            &vxent, &vxent_old, 0);
        soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
        if (rv == SOC_E_NONE) {
            /* found extd entry */

            if (update_func) {
                /* need update the entry before writing it back */
                BCM_IF_ERROR_RETURN
                    (update_func(unit,vent,ctxt,NULL,&vxent_old,
                                      &use_extd_table));
            }
            soc_mem_lock(unit, VLAN_XLATE_EXTDm);
            rv = soc_mem_write(unit, VLAN_XLATE_EXTDm,
                          MEM_BLOCK_ALL, idx, &vxent_old);
            soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
            if (existing_updated != NULL) {
                *existing_updated = 1;
            }
        } else if (rv != SOC_E_NOT_FOUND) {
            return rv;
        } else {
            /* both regular and extd table are not found */
            /* need update the entry before insert */
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_vxlate2vxlate_extd(unit,vent,&vxent));
            if (update_func) {
                BCM_IF_ERROR_RETURN
                    (update_func(unit,vent,ctxt,NULL,&vxent,
                                      &use_extd_table));
            }

            if (use_extd_table) {
                soc_mem_lock(unit, VLAN_XLATE_EXTDm);
                rv = soc_mem_insert(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, 
                                &vxent);    
                soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
            } else {
                soc_mem_lock(unit, VLAN_XLATEm);
                rv = soc_mem_insert(unit, VLAN_XLATEm, MEM_BLOCK_ALL, 
                                vent);    
                soc_mem_unlock(unit, VLAN_XLATEm);
            }
        }
    }
    return rv;    
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function : _bcm_trx_vlan_action_verify
 *
 * Purpose  : to validate all members of action structure
 */
int
_bcm_trx_vlan_action_verify(int unit, bcm_vlan_action_set_t *action)
{
    if (NULL == action) {
        return BCM_E_PARAM;
    }
    VLAN_CHK_ID(unit, action->new_inner_vlan);
    VLAN_CHK_ID(unit, action->new_outer_vlan);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority < -1 || action->priority > 7 ||
            action->new_inner_pkt_prio > 7 ||
            action->new_outer_cfi > 1 || action->new_inner_cfi > 1) {
            return BCM_E_PARAM;
        }
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_vlan_xlate_has_class_id)) {
        /* The max value of CLASS_ID should not be greater than 0xff since
         * length of IFP_KEY to qualify CLASS_ID is 8 bits.
         */
        if(action->class_id) {
            if(action->class_id > 0xff) {
                return BCM_E_PARAM;
            }
        }
    }
#endif

    switch (action->dt_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;

#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_outer_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_outer_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_outer_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_inner_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_inner_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->dt_inner_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_outer_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_outer_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_outer_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_inner_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ot_inner_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_copy_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_outer_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_outer_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
    case bcmVlanActionCopy:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_inner_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionReplace:
    case bcmVlanActionDelete:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_inner_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->it_inner_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionReplace:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_outer_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_outer_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_inner) {
    case bcmVlanActionNone:
    case bcmVlanActionAdd:
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_inner_pkt_prio) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
#if defined(BCM_TRIDENT2_SUPPORT)
    case bcmVlanActionCopyVsan:
    case bcmVlanActionReplaceVsan:
        if (!soc_feature(unit, soc_feature_fcoe)) {
            return BCM_E_UNAVAIL;
        }
        break;
#endif
    default:
        return BCM_E_PARAM;
    }

    switch (action->ut_inner_cfi) {
    case bcmVlanActionNone:
        break;
    case bcmVlanActionAdd:
        if (!soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            return BCM_E_UNAVAIL;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function : _bcm_trx_egr_vlan_action_verify
 *
 * Purpose  : to validate all members of action structure for egress vlan 
 *            more strict validation than ingress  
 */
STATIC int 
_bcm_trx_egr_vlan_action_verify(int unit, bcm_vlan_action_set_t *action)
{
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_action_verify(unit, action));

    if (action->dt_outer == bcmVlanActionAdd ||
        action->dt_outer_prio == bcmVlanActionAdd ||
        action->ot_outer == bcmVlanActionAdd ||
        action->ot_outer_prio == bcmVlanActionAdd) {
        return BCM_E_PARAM;
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    /*Ensure that class_id doesnt overcross CLASS_IDf*/
    if(soc_feature(unit, soc_feature_vlan_xlate_has_class_id)) {
        /* The max value of CLASS_ID should not be greater than 0xff since
         * length of IFP_KEY to qualify CLASS_ID is 8 bits.
         */
        if(action->class_id) {
            if(action->class_id > 0xff) {
                return BCM_E_PARAM;
            }
        }
    }
#endif
    /* Single inner tag modification is available for egress vlan actions */
    if (!soc_feature(unit, soc_feature_vlan_egr_it_inner_replace)) {
        if (action->dt_outer_prio != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->ot_outer_prio != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->it_outer != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->it_inner != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->it_inner_prio != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->ut_inner != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
        if (action->ut_outer != bcmVlanActionNone) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function : _bcm_trx_vlan_ip_verify
 *
 * Purpose  : to validate members of vlan_ip structure
 */
STATIC int 
_bcm_trx_vlan_ip_verify(int unit, bcm_vlan_ip_t *vlan_ip)
{
    if (NULL == vlan_ip) {
        return BCM_E_PARAM;
    }
    VLAN_CHK_ID(unit, vlan_ip->vid);
    VLAN_CHK_PRIO(unit, vlan_ip->prio);

    return BCM_E_NONE;
}

/*
 * Function : bcm_trx_vlan_action_profile_detach
 *
 * Purpose  : to initialize hardware ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and allocate memory
 *            to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
int
_bcm_trx_vlan_action_profile_detach(int unit)
{
    int rv;
    soc_mem_t mem = ING_VLAN_TAG_ACTION_PROFILEm;
    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem = ING_VLAN_TAG_ACTION_PROFILE_2m;
    }
    if (SOC_MEM_IS_VALID(unit, mem)) {
        /* De-initialize the ING_VLAN_TAG_ACTION_PROFILE table */
        ING_VLAN_TAG_ACTION_LOCK(unit);
        rv = soc_profile_mem_destroy(unit, ing_action_profile[unit]);
        if (BCM_SUCCESS(rv)) {
            sal_free(ing_action_profile[unit]);
            ing_action_profile[unit] = NULL;
        }
        ING_VLAN_TAG_ACTION_UNLOCK(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    EGR_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_destroy(unit, egr_action_profile[unit]);
    if (BCM_SUCCESS(rv)) {
        sal_free(egr_action_profile[unit]);
        egr_action_profile[unit] = NULL;
    }
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    VLAN_RANGE_LOCK(unit);
    rv = soc_profile_mem_destroy(unit, vlan_range_profile[unit]);
    if (BCM_SUCCESS(rv)) {
        sal_free(vlan_range_profile[unit]);
        vlan_range_profile[unit] = NULL;
    }
    VLAN_RANGE_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function : bcm_trx_vlan_action_profile_init
 *
 * Purpose  : to initialize hardware ING_VLAN_TAG_ACTION_PROFILE and
 *            EGR_VLAN_TAG_ACTION_PROFILE tables and allocate memory
 *            to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
int
_bcm_trx_vlan_action_profile_init(int unit)
{
    int i, idx, num_ports;
    vlan_protocol_data_entry_t proto_entry;
    vlan_subnet_entry_t subnet_entry;
    vlan_mac_entry_t vlan_mac_entry;
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    egr_vlan_xlate_entry_t evx_ent;
    egr_vlan_xlate_1_double_entry_t evx1d_ent;
    ing_vlan_tag_action_profile_entry_t ing_profile;
    egr_vlan_tag_action_profile_entry_t egr_profile_entry;
    uint32 rval, temp_index;
    soc_mem_t mem[2];
    soc_mem_t mem_cnt = 1;
    int table_cnt = 1;
    int entry_idxmin[2], entry_idxmax[2];
    int entry_words[2];
    void *entries[2];
    int key_type_value, key_type;
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t handle;
    vlan_xlate_entry_t vent;
    egr_vlan_xlate_entry_t event;
    uint32 key[2];
    int fs_idx;
#endif
    ing_vlan_tag_action_profile_2_entry_t ing_profile_2;
    void *ing_profile_entry;
    uint32 ing_ent_sz;
    int rv;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_info_t *field_info = NULL, *field_info_1 = NULL;
    int type;
    soc_field_t type_fld = ENTRY_TYPEf;
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TOMAHAWK_SUPPORT)
    int vlan_prot_entries;
    bcm_port_t port;
    bcm_pbmp_t all_pbmp;
    _bcm_port_info_t *pinfo;
#endif

    soc_mem_t ing_mem = VLAN_XLATEm;
    soc_mem_t egr_mem = EGR_VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *xlate_entry = &vx_ent;
    void *egr_xlate_entry = &evx_ent;
    ing_vlan_tag_action_profile_entry_t ing_vlan_tag_action_profile_mask;
    void *entry_mask[1];

    if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
        ing_action_profile_def[unit] = 1;
    }

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        ing_mem = VLAN_XLATE_1_DOUBLEm;
        egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f;
        xlate_entry = &vx1d_ent;
        egr_xlate_entry = &evx1d_ent;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        type_fld = KEY_TYPEf;
#endif
    }

    /* Create profile table cache (or re-init if it already exists) */
    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem[0] = ING_VLAN_TAG_ACTION_PROFILE_2m;
        mem[1] = ING_VLAN_TAG_ACTION_PROFILE_1m;
        ing_ent_sz = sizeof(ing_vlan_tag_action_profile_2_entry_t);
        ing_profile_entry = &ing_profile_2;
        table_cnt = 2;
        entry_idxmin[0] = entry_idxmin[1] = 1;
    } else {
        mem[0] = ING_VLAN_TAG_ACTION_PROFILEm;
        ing_ent_sz = sizeof(ing_vlan_tag_action_profile_entry_t);
        ing_profile_entry = &ing_profile;
        entry_idxmin[0] = entry_idxmin[1] = 0;
        sal_memset(&ing_vlan_tag_action_profile_mask, 0xff,
                   sizeof(ing_vlan_tag_action_profile_mask));
    }
    /* Create profile table cache (or re-init if it already exists) */
    entry_words[0] = entry_words[1] = ing_ent_sz / sizeof(uint32);

    if (SOC_MEM_IS_VALID(unit, mem[0])) {
        /* Initialize the ING_VLAN_TAG_ACTION_PROFILE table */
    
        if (ing_action_profile[unit] == NULL) {
            ing_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                                 "Ing Action Profile Mem");
            if (ing_action_profile[unit] == NULL) {
                return BCM_E_MEMORY;
            }
            soc_profile_mem_t_init(ing_action_profile[unit]);
        }
    
        entry_idxmax[0] = entry_idxmax[1] = soc_mem_index_max(unit, mem[0]);
        if (mem[0] == ING_VLAN_TAG_ACTION_PROFILEm) {
            entry_mask[0] = &ing_vlan_tag_action_profile_mask;
            if (SOC_MEM_FIELD_VALID(unit, mem[0], ECCPf)) {
                soc_mem_field32_set(unit, mem[0], entry_mask[0], ECCPf, 0);
            } else if (SOC_MEM_FIELD_VALID(unit, mem[0], EVEN_PARITYf)) {
                soc_mem_field32_set(unit, mem[0], entry_mask[0], EVEN_PARITYf,
                                    0);
            }
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_index_create(unit, mem, entry_words,
                                             entry_idxmin, entry_idxmax, entry_mask,
                                             table_cnt, ing_action_profile[unit]));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_index_create(unit, mem, entry_words,
                                             entry_idxmin, entry_idxmax, NULL,
                                             table_cnt, ing_action_profile[unit]));
        }

        if (SOC_WARM_BOOT(unit)) {
            soc_mem_t port_mem = PORT_TABm;
            if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
                port_mem = ING_DEVICE_PORTm;
            }

            /* Increment the ref count for all ports */
            for (i = 0; i < soc_mem_index_count(unit, port_mem); i++) {
                if (!SOC_PORT_VALID(unit, i)) {
                    continue;
                }
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(unit) && IS_MACSEC_PORT(unit, i)) {
                    continue;
                }
#endif
                    SOC_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, i,
                                        TAG_ACTION_PROFILE_PTRf, &idx));
                    SOC_IF_ERROR_RETURN(
                        soc_profile_mem_reference(unit, ing_action_profile[unit],
                        idx, 1));
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                                                    idx - entry_idxmin[0], 1);
                }
    
            /* TH/TH2/TD3/KT2 and some other chips need use another method to revocer
             * the reference count in _bcm_th_port_vlan_action_profile_reinit().
             */
            if (!SOC_IS_TOMAHAWK(unit) && !SOC_IS_TOMAHAWK2(unit) &&
                !SOC_IS_KATANA2(unit)) {
                /* Increment the ref count for all Protocol-based VLANs */
                for (i = 0; i < soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm);
                     i++) {
                    SOC_IF_ERROR_RETURN(
                        READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, i,
                        &proto_entry));
                    idx = soc_mem_field32_get(unit, VLAN_PROTOCOL_DATAm,
                                              &proto_entry,
                                              TAG_ACTION_PROFILE_PTRf);
                    if (!SOC_IS_TRIDENT3X(unit)) {
                        SOC_IF_ERROR_RETURN(
                            soc_profile_mem_reference(unit,
                            ing_action_profile[unit], idx, 1));
                        SOC_PROFILE_MEM_ENTRIES_PER_SET(
                            unit, ing_action_profile[unit],
                            idx - entry_idxmin[0], 1);
                    }
                    if (idx < ing_action_profile_def[unit]) {
                        SOC_IF_ERROR_RETURN(
                            soc_profile_mem_reference(unit,
                            ing_action_profile[unit],
                            ing_action_profile_def[unit], 1));
                    }
                }
            }

            /* Increment the ref count for all Subnet-based VLANs */
            for (i = 0; i < soc_mem_index_count(unit, VLAN_SUBNETm); i++) {
                SOC_IF_ERROR_RETURN
                    (READ_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, &subnet_entry));
                if (!soc_mem_field32_get(unit, VLAN_SUBNETm, 
                                         &subnet_entry, VALIDf)) {
                    continue;
                }
                idx = soc_mem_field32_get(unit, VLAN_SUBNETm,
                                          &subnet_entry, TAG_ACTION_PROFILE_PTRf);
                if (idx >= entry_idxmin[0]) {
                    SOC_IF_ERROR_RETURN(
                        soc_profile_mem_reference(unit, ing_action_profile[unit],
                        idx, 1));
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                                                    idx - entry_idxmin[0], 1);
                }
            }
    
            /* Increment the ref count for all Mac-based VLANs & vlan translations */
            if (SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
                for (i = 0; i < soc_mem_index_count(unit, VLAN_MACm); i++) {
                    SOC_IF_ERROR_RETURN
                        (READ_VLAN_MACm(unit, MEM_BLOCK_ANY, i,
                                        &vlan_mac_entry));
                    if (!soc_mem_field32_get(unit, VLAN_MACm,
                                &vlan_mac_entry, VALIDf)) {
                        continue;
                    }
                    key_type_value = soc_mem_field32_get(unit, VLAN_MACm,
                            &vlan_mac_entry, KEY_TYPEf);
                    (void) _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                            &key_type);
                    if (VLXLT_HASH_KEY_TYPE_VLAN_MAC == key_type) {
                        idx = soc_mem_field32_get(unit, VLAN_MACm, &vlan_mac_entry,
                                MAC__TAG_ACTION_PROFILE_PTRf);
                    } else if (VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT == key_type) {
                        idx = soc_mem_field32_get(unit, VLAN_MACm, &vlan_mac_entry,
                                MAC_PORT__TAG_ACTION_PROFILE_PTRf);
                    } else {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(
                            soc_profile_mem_reference(unit, ing_action_profile[unit],
                                idx, 1));
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit,
                            ing_action_profile[unit], idx - entry_idxmin[0], 1);
                }
            }

            for (i = 0; i < soc_mem_index_count(unit, ing_mem); i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, ing_mem, MEM_BLOCK_ANY, i, xlate_entry));
                if (!soc_mem_field32_get(unit, ing_mem, xlate_entry, vf)) {
                    continue;
                }
                key_type_value = soc_mem_field32_get(unit, ing_mem, xlate_entry,
                                                     KEY_TYPEf);
    
                /* Here return value is not being checked (that is intentional); in case
                   it returns BCM_E_UNAVAIL, will proceed with else condition below */
                /* Coverity[unchecked_value] */
                _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value, &key_type);
                if (key_type == VLXLT_HASH_KEY_TYPE_VIF ||
                    key_type == VLXLT_HASH_KEY_TYPE_VIF_VLAN ||
                    key_type == VLXLT_HASH_KEY_TYPE_VIF_CVLAN ||
                    key_type == VLXLT_HASH_KEY_TYPE_VIF_OTAG ||
                    key_type == VLXLT_HASH_KEY_TYPE_VIF_ITAG) {
                    idx = soc_mem_field32_get(unit, ing_mem, xlate_entry,
                                              VIF__TAG_ACTION_PROFILE_PTRf);
                } else {
                    idx = soc_mem_field32_get(unit, ing_mem, xlate_entry,
                                              TAG_ACTION_PROFILE_PTRf);
#ifdef BCM_TRIDENT3_SUPPORT
                    if (soc_feature(unit, soc_feature_flex_flow)) {
                        soc_mem_t mem_view_id;
                        uint32 data_type;
                        int rv;

                        data_type = 0;
                        if (SOC_MEM_FIELD_VALID(unit, ing_mem, DATA_TYPEf)) {
                            data_type = soc_mem_field32_get(unit, ing_mem, 
                                          xlate_entry,DATA_TYPEf);
                        }
                        rv = soc_flow_db_mem_to_view_id_get (unit, ing_mem, 
                                key_type_value, data_type, 0, NULL, 
                                (uint32 *)&mem_view_id);
                        if (SOC_SUCCESS(rv)) {
                            idx = 0;
                            if (SOC_MEM_FIELD_VALID(unit, mem_view_id, 
                                                TAG_ACTION_SETf)) {
                                idx = soc_mem_field32_get(unit, mem_view_id, 
                                        xlate_entry, TAG_ACTION_SETf);
                            }
                        }
                    }
#endif
                }
                rv = soc_profile_mem_reference(unit, ing_action_profile[unit], 
                                           idx, 1);
                if (SOC_SUCCESS(rv)) {
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                                                idx - entry_idxmin[0], 1);
                } else {
                    /* TAG_ACTION_PROFILE_PTR may not be used for all key types.
                     * In this case, it has default value 0. Ignore it.
                     */
                    if (idx) {
                        return rv;
                    }
                }
    
    #if defined(BCM_TRIUMPH2_SUPPORT)
                if (soc_feature(unit, soc_feature_gport_service_counters)) {
                    fs_idx =
                        soc_mem_field32_get(unit, ing_mem, xlate_entry, VINTF_CTR_IDXf);
                    if (SOC_MEM_FIELD_VALID(unit, ing_mem, USE_VINTF_CTR_IDXf)) {
                        if (0 == soc_mem_field32_get(unit, ing_mem,
                                         xlate_entry, USE_VINTF_CTR_IDXf)) {
                            fs_idx = 0;
                        }
                    }
                    if (0 != fs_idx) { /* Recover flex stat counter */
                        sal_memset(&vent, 0, sizeof(vent));
                        /* Construct key-only entry, copy to FS handle */
                        soc_mem_field32_set(unit, ing_mem, &vent, KEY_TYPEf,
                                 soc_mem_field32_get(unit, ing_mem, xlate_entry,
                                                             KEY_TYPEf));
                        soc_mem_field_get(unit, ing_mem, (uint32 *) xlate_entry,
                                          KEYf, (uint32 *) key);
                        soc_mem_field_set(unit, ing_mem, (uint32 *) &vent,
                                          KEYf, (uint32 *) key);
    
                        _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
                        _BCM_FLEX_STAT_HANDLE_COPY(handle, vent);
                        _bcm_esw_flex_stat_ext_reinit_add(unit,
                                      _bcmFlexStatTypeVxlt, fs_idx, handle);
                    }
                }
    #endif
            }
    
            /* One extra increment to preserve location
             * ING_ACTION_PROFILE_DEFAULT */
            SOC_IF_ERROR_RETURN(
                soc_profile_mem_reference(unit, ing_action_profile[unit],
                ing_action_profile_def[unit], 1));
        } else {
            /* Initialize the ING_ACTION_PROFILE_DEFAULT. For untagged and
             * inner-tagged packets, always add an outer tag.
             */
            soc_mem_t port_mem = PORT_TABm;
            sal_memset(ing_profile_entry, 0, ing_ent_sz);
            soc_mem_field32_set(unit, mem[0], ing_profile_entry, UT_OTAG_ACTIONf,
                                _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionAdd));
            if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                soc_mem_field32_set(unit, mem[0], ing_profile_entry, UT_OPRI_ACTIONf,
                                    _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
                soc_mem_field32_set(unit, mem[0], ing_profile_entry, UT_OCFI_ACTIONf,
                                    _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
            }
            soc_mem_field32_set(unit, mem[0], ing_profile_entry, SIT_OTAG_ACTIONf,
                                _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionAdd));
            if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                soc_mem_field32_set(unit, mem[0], ing_profile_entry, SIT_OPRI_ACTIONf,
                                    _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
                soc_mem_field32_set(unit, mem[0], ing_profile_entry, SIT_OCFI_ACTIONf,
                                    _BCM_TRX_PRI_CFI_ACTION_ENCODE(bcmVlanActionAdd));
            }
            soc_mem_field32_set(unit, mem[0], ing_profile_entry, SIT_PITAG_ACTIONf,
                                _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionNone));
            soc_mem_field32_set(unit, mem[0], ing_profile_entry, SOT_POTAG_ACTIONf,
                                _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionReplace));
            soc_mem_field32_set(unit, mem[0], ing_profile_entry, DT_POTAG_ACTIONf,
                                _BCM_TRX_TAG_ACTION_ENCODE(bcmVlanActionReplace));
            entries[0] = ing_profile_entry;
            entries[1] = ing_profile_entry;
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, ing_action_profile[unit],
                                     (void *) &entries, 1, &temp_index));
    
            /* Increment the ref count for all ports */
            if (SOC_MEM_IS_VALID(unit, ING_DEVICE_PORTm)) {
                port_mem = ING_DEVICE_PORTm;
            }

            for (i = 0; i < soc_mem_index_count(unit, port_mem); i++) {
                if (SOC_PORT_VALID(unit, i)) {
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, ing_action_profile[unit],
                    ing_action_profile_def[unit], 1));
            }
            }

#if defined(BCM_TOMAHAWK_SUPPORT)
            if (!SOC_WARM_BOOT(unit) &&
                (SOC_IS_TOMAHAWK(unit) || SOC_IS_TOMAHAWK2(unit))) {
                vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
                BCM_PBMP_CLEAR(all_pbmp);
                BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

                PBMP_ITER(all_pbmp, port) {
                    if (!IS_LB_PORT(unit, port)) {
                        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit,
                                                               port, &pinfo));
                        for (i = 0; i < vlan_prot_entries; i++) {
                            rv = READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                     pinfo->vlan_prot_ptr + i,
                                     &proto_entry);
                            if (BCM_FAILURE(rv)) {
                                return (rv);
                            }
                            idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit,
                                      &proto_entry, TAG_ACTION_PROFILE_PTRf);
                            SOC_IF_ERROR_RETURN(soc_profile_mem_reference(unit,
                                                    ing_action_profile[unit],
                                                    idx, 1));
                        }
                    }
                }
            }
#endif /* BCM_TOMAHAWK_SUPPORT */

            /* Increment the ref count for all Protocol-based VLANs */
            for (i = 0; i < soc_mem_index_count(unit, VLAN_PROTOCOL_DATAm);
                 i++) {
#if 0
                vlan_protocol_data_entry_t vp_ent;
                sal_memset(&vp_ent, 0, sizeof(vp_ent));
                if (ing_action_profile_def[unit] != 0) {
                    soc_mem_field32_set(unit, VLAN_PROTOCOL_DATAm, &vp_ent,
                                        TAG_ACTION_PROFILE_PTRf,
                                        ing_action_profile_def[unit]);
                    SOC_IF_ERROR_RETURN(
                        soc_mem_write(unit, VLAN_PROTOCOL_DATAm, SOC_BLOCK_ALL,
                                      i, &vp_ent));
                }
#endif
                SOC_IF_ERROR_RETURN(
                    soc_profile_mem_reference(unit, ing_action_profile[unit],
                    ing_action_profile_def[unit], 1));
            }
        }
    }

    /* Initialize the EGR_VLAN_TAG_ACTION_PROFILE table */
    if (egr_action_profile[unit] == NULL) {
        egr_action_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Egr Action Profile Mem");
        if (egr_action_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(egr_action_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem[0] = EGR_VLAN_TAG_ACTION_PROFILEm;
    entry_words[0] = sizeof(egr_vlan_tag_action_profile_entry_t) / sizeof(uint32);
    entry_idxmin[0] = soc_mem_index_min(unit, mem[0]);
    entry_idxmax[0] = soc_mem_index_max(unit, mem[0]);

    mem[1] = EGR_VLAN_TAG_ACTION_PROFILE_2m;
    if (soc_mem_is_valid(unit, mem[1])) {
        entry_words[1] = sizeof(egr_vlan_tag_action_profile_entry_t) / sizeof(uint32);
        entry_idxmin[1] = soc_mem_index_min(unit, mem[1]);
        entry_idxmax[1] = soc_mem_index_max(unit, mem[1]) - 1;
        /* For EGR_VLAN_TAG_ACTION_PROFILE_2m device,
         * last entry (index 63) is reserved for VT_MISS_UNTAG */
        entry_idxmax[0] --;
        mem_cnt = 2;
    }

    SOC_IF_ERROR_RETURN(
        soc_profile_mem_index_create(unit, mem, entry_words,
                                     entry_idxmin, entry_idxmax, NULL,
                                     mem_cnt, egr_action_profile[unit]));

    SOC_PBMP_COUNT(PBMP_ALL(unit), num_ports);
    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        PBMP_ALL_ITER(unit, i) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
                if (soc_feature(unit, soc_feature_egr_all_profile)) {
                    soc_field_t fld = TAG_ACTION_PROFILE_PTRf;
                    BCM_IF_ERROR_RETURN(
                        bcm_esw_port_egr_lport_fields_get(unit, i,
                        EGR_VLAN_CONTROL_3m, 1, &fld, (uint32 *)&idx));
                } else {
                egr_vlan_control_3_entry_t entry;
                SOC_IF_ERROR_RETURN(
                            READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY,
                            i, &entry));
                idx = soc_EGR_VLAN_CONTROL_3m_field32_get(unit, &entry,
                        TAG_ACTION_PROFILE_PTRf);
                }
            } else
#endif
            {
                SOC_IF_ERROR_RETURN
                    (READ_EGR_VLAN_CONTROL_3r(unit, i, &rval));
                idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                        rval, TAG_ACTION_PROFILE_PTRf);
            }
            soc_profile_mem_reference(unit, egr_action_profile[unit], idx, 1);
        }

        if (SOC_MEM_IS_VALID(unit, egr_mem)) {
           /* Increment the ref count for all egress vlan translations */
            for (i = 0; i < soc_mem_index_count(unit, egr_mem); i++) {
               SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, i, egr_xlate_entry));
                if (!soc_mem_field32_get(unit, egr_mem, egr_xlate_entry, vf)) {
                   continue;
               }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
                type = soc_mem_field32_get(unit, egr_mem, egr_xlate_entry,
                                           type_fld);
                if (type == TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP) {
                    field_info = soc_mem_fieldinfo_get(
                                     unit, egr_mem,
                                     VFI_DVP_GROUP__XLATE_DATAf);
                } else if (type ==
                           TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI) {
                    field_info = soc_mem_fieldinfo_get(
                                     unit, egr_mem,
                                     VLAN_XLATE_VFI__XLATE_DATAf);
#if defined(BCM_TRIDENT3_SUPPORT)
                } else if (type ==
                             (int)TD3_ELVXLT_1_HASH_KEY_TYPE_DGPP_OVID_IVID) {
                    field_info = soc_mem_fieldinfo_get(
                                     unit, egr_mem,
                                     VXLAN_SUB_CHANNEL__VXLAN_SUB_CHANNEL_DATAf);
#endif
                }
                if (field_info) {
                    sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
                    SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(
                        unit, egr_mem, egr_xlate_entry, field_info, entry);
                    field_info_1 = soc_mem_fieldinfo_get(unit, egr_mem,
                                                         XLATE_DATAf);
                    if (!field_info_1) {
                        return BCM_E_UNAVAIL;
                    }
                    SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(
                        unit, egr_mem, egr_xlate_entry, field_info_1, entry);
                }
#endif
                idx = soc_mem_field32_get(unit, egr_mem,
                                          egr_xlate_entry, TAG_ACTION_PROFILE_PTRf);
                soc_profile_mem_reference(unit, egr_action_profile[unit],
                                          idx, 1);
   #if defined(BCM_TRIUMPH2_SUPPORT)
               if (soc_feature(unit, soc_feature_gport_service_counters)) {
                   fs_idx =
                        soc_mem_field32_get(unit, egr_mem, egr_xlate_entry,
                                                   VINTF_CTR_IDXf);
                    if (SOC_MEM_FIELD_VALID(unit, egr_mem,
                                           USE_VINTF_CTR_IDXf)) {
                        if (0 == soc_mem_field32_get(unit, egr_mem,
                                         egr_xlate_entry, USE_VINTF_CTR_IDXf)) {
                           fs_idx = 0;
                       }
                   }
                   if (0 != fs_idx) { /* Recover flex stat counter */
                       sal_memset(&event, 0, sizeof(event));
                       /* Construct key-only entry, copy to FS handle */
                        soc_mem_field32_set(unit, egr_mem, &event, ENTRY_TYPEf,
                                 soc_mem_field32_get(unit, egr_mem,
                                                     egr_xlate_entry, ENTRY_TYPEf));
                        soc_mem_field_get(unit, egr_mem,
                                          (uint32 *) egr_xlate_entry,
                                         KEYf, (uint32 *) key);
                        soc_mem_field_set(unit, egr_mem,
                                         (uint32 *) &event,
                                         KEYf, (uint32 *) key);
   
                       _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
                       _BCM_FLEX_STAT_HANDLE_COPY(handle, event);
                       _bcm_esw_flex_stat_ext_reinit_add(unit,
                                     _bcmFlexStatTypeVxlt, fs_idx, handle);
                   }
               }
   #endif
           }
        }
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm)) {            
            egr_vlan_xlate_2_double_entry_t evx2d_ent;
            void *egr_xlate_2_entry = &evx2d_ent;
			
            egr_mem = EGR_VLAN_XLATE_2_DOUBLEm;
           /* Increment the ref count for all egress vlan translations */
            for (i = 0; i < soc_mem_index_count(unit, egr_mem); i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, i, egr_xlate_2_entry));
                if (!soc_mem_field32_get(unit, egr_mem, egr_xlate_2_entry, vf)) {
                    continue;
                }
                idx = soc_mem_field32_get(unit, egr_mem,
                                          egr_xlate_2_entry, TAG_ACTION_PROFILE_PTRf);
                soc_profile_mem_reference(unit, egr_action_profile[unit],
                                          idx, 1);
           }
        }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
        if (soc_feature(unit, soc_feature_vxlan_decoupled_mode)) {
            uint32 entry_type;
            egr_l3_next_hop_entry_t egr_nh;
            egr_vlan_xlate_1_single_entry_t egr_single_entry;
            egr_mem = EGR_VLAN_XLATE_1_SINGLEm;
            for (i = 0; i < soc_mem_index_count(unit, egr_mem); i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, i, &egr_single_entry));
                    if (soc_mem_field32_get(unit, egr_mem, &egr_single_entry, BASE_VALIDf)) {
                        idx = soc_mem_field32_get(unit, egr_mem,
                                          &egr_single_entry, TAG_ACTION_PROFILE_PTRf);
                        soc_profile_mem_reference(unit, egr_action_profile[unit], idx, 1);
                    }
            }

            egr_mem = EGR_L3_NEXT_HOPm;
            for (i = 0; i < soc_mem_index_count(unit, egr_mem); i++) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_read(unit, egr_mem, MEM_BLOCK_ANY, i, &egr_nh));
                entry_type = soc_mem_field32_get(unit, egr_mem, &egr_nh, DATA_TYPEf);
                if (entry_type == _BCM_VXLAN_EGR_NEXT_HOP_L2OTAG_VIEW) {
                    idx = soc_mem_field32_get(unit, egr_mem, &egr_nh,
                        L2_OTAG__TAG_ACTION_PROFILE_PTRf);
                    soc_profile_mem_reference(unit, egr_action_profile[unit], idx, 1);
                }
            }
        }
#endif
        /* One extra increment to preserve location EGR_ACTION_PROFILE_DEFAULT */
        soc_profile_mem_reference(unit, egr_action_profile[unit],
                              EGR_ACTION_PROFILE_DEFAULT, 1);
    } else {
        /* Initialize the EGR_ACTION_PROFILE_DEFAULT to have all
         * actions as NOP (0).
         */
        sal_memset(&egr_profile_entry, 0, sizeof(egr_vlan_tag_action_profile_entry_t));
        entries[0] = &egr_profile_entry;
        entries[1] = &egr_profile_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, egr_action_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        /* Increment the ref count for all ports */
        for (i = 0; i < num_ports; i++) {
            soc_profile_mem_reference(unit, egr_action_profile[unit],
                                      EGR_ACTION_PROFILE_DEFAULT, 1);
        }
    }

    /* Initialize the VLAN range profile table */
    _bcm_trx_vlan_range_profile_init(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    /* Reload VLAN protocol action data */
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_port_protocol_action_reinit(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    return BCM_E_NONE;
}

#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) || \
    defined(BCM_KATANA2_SUPPORT)
/*
 * Function : _bcm_th_port_vlan_action_profile_reinit
 *
 * Purpose  :
 *      Recover 'vlan_prot_ptr' in port_info structure.
 *      Recover reference count of ING_VLAN_TAG_ACTION_PROFILE
 *      for all Protocol-based VLANs.
 *
 * Note:
 *      'vlan_prot_ptr' in port_info structure was recovered
 *      from scache for TH2.
 */
int
_bcm_th_port_vlan_action_profile_reinit(int unit)
{
    int rv = BCM_E_NONE;
    int i, idx;
    int vlan_prot_entries;
    bcm_port_t port;
    uint32 vlan_protocol_data_index = 0;
    _bcm_port_info_t *pinfo;
    bcm_pbmp_t all_pbmp;
    uint32 action_profile_idx;
    vlan_protocol_data_entry_t vde;

    BCM_PBMP_CLEAR(all_pbmp);
    BCM_PBMP_ASSIGN(all_pbmp, PBMP_ALL(unit));

    vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    PBMP_ITER(all_pbmp, port) {
        if (!IS_LB_PORT(unit, port)) {
            /* update vlan port protocol data memory */
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));

            /* TH2/TD3X/KT2 have already recovered it from scache */
            if (!SOC_IS_TOMAHAWK2(unit) && !SOC_IS_TRIDENT3X(unit) &&
                !SOC_IS_KATANA2(unit)) {
                /* Get VLAN_PROTOCOL_DATA_INDEXf  in PORT_TAB. */
                SOC_IF_ERROR_RETURN(_bcm_esw_port_tab_get(unit, port,
                                    VLAN_PROTOCOL_DATA_INDEXf, &idx));
                vlan_protocol_data_index = idx * vlan_prot_entries;
                pinfo->vlan_prot_ptr = vlan_protocol_data_index;

                BCM_IF_ERROR_RETURN(
                        _bcm_port_vlan_protocol_data_entry_reference (unit,
                            vlan_protocol_data_index, 16));
            }

            for (i = 0; i < vlan_prot_entries; i++) {
                rv = READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                              pinfo->vlan_prot_ptr + i, &vde);
                if (BCM_FAILURE(rv)) {
                    return (rv);
                }
                action_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit,
                                     &vde, TAG_ACTION_PROFILE_PTRf);
                if (SOC_IS_TRIDENT3X(unit)) {
                    if (action_profile_idx >= ing_action_profile_def[unit]) {
                        SOC_IF_ERROR_RETURN(
                            soc_profile_mem_reference(unit, ing_action_profile[unit],
                            action_profile_idx, 1));
                        SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                            action_profile_idx - ing_action_profile_def[unit], 1);
                    }
                } else {
                    SOC_IF_ERROR_RETURN(
                        soc_profile_mem_reference(unit, ing_action_profile[unit],
                        action_profile_idx, 1));
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, ing_action_profile[unit],
                        action_profile_idx, 1);
                }
            }
        }
    }

    return (rv);
}
#endif

/*
 * Function : _bcm_trx_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to vlan action profile table
 */
int
_bcm_trx_vlan_action_profile_entry_add(int unit,
                                      bcm_vlan_action_set_t *action,
                                      uint32 *index)
{
    int rv = BCM_E_NONE;
    ing_vlan_tag_action_profile_entry_t entry1;
    ing_vlan_tag_action_profile_2_entry_t entry2;
    void *entries[2];
    void *entry;
    soc_mem_t vtap_mem = ING_VLAN_TAG_ACTION_PROFILEm;

    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {
        vtap_mem = ING_VLAN_TAG_ACTION_PROFILE_2m;
        sal_memset(&entry2, 0, sizeof(ing_vlan_tag_action_profile_2_entry_t));
        entry = &entry2;
    } else {
        sal_memset(&entry1, 0, sizeof(ing_vlan_tag_action_profile_entry_t));
        entry = &entry1;
    }

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));

    soc_mem_field32_set(unit, vtap_mem, entry, DT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_outer));
    soc_mem_field32_set(unit, vtap_mem, entry, DT_POTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_outer_prio));
    soc_mem_field32_set(unit, vtap_mem, entry, DT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_inner));
    soc_mem_field32_set(unit, vtap_mem, entry, DT_PITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_inner_prio));
    soc_mem_field32_set(unit, vtap_mem, entry, SOT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer));
    soc_mem_field32_set(unit, vtap_mem, entry, SOT_POTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer_prio));
    soc_mem_field32_set(unit, vtap_mem, entry, SOT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_inner));
    soc_mem_field32_set(unit, vtap_mem, entry, SIT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->it_outer));
    soc_mem_field32_set(unit, vtap_mem, entry, SIT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->it_inner));
    soc_mem_field32_set(unit, vtap_mem, entry, SIT_PITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->it_inner_prio));
    soc_mem_field32_set(unit, vtap_mem, entry, UT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ut_outer));
    soc_mem_field32_set(unit, vtap_mem, entry, UT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ut_inner));

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set
            (unit, vtap_mem, entry, DT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, DT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_outer_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, DT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_inner_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, DT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_inner_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SOT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SOT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SOT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_inner_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SOT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_inner_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SIT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SIT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_outer_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SIT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_inner_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, SIT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_inner_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, UT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, UT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_cfi));
        soc_mem_field32_set
            (unit, vtap_mem, entry, UT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_inner_pkt_prio));
        soc_mem_field32_set
            (unit, vtap_mem, entry, UT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_inner_cfi));

#if defined(BCM_TRIDENT2_SUPPORT)
        if (soc_feature(unit, soc_feature_fcoe)) {

            /* overwrite just fcoe related fields (pri)*/
            soc_mem_field32_set
                (unit, vtap_mem, entry, DT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->dt_outer_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, DT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->dt_inner_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, SOT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->ot_outer_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, SOT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->ot_inner_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, SIT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->it_outer_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, SIT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->it_inner_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, UT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->ut_outer_pkt_prio));
            soc_mem_field32_set
                (unit, vtap_mem, entry, UT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                   action->ut_inner_pkt_prio));
        }
#endif  /* BCM_TRIDENT2_SUPPORT */
    }

    entries[0] = entry;
    entries[1] = entry;
    ING_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_add(unit, ing_action_profile[unit],
                             (void *) &entries, 1, index);
    ING_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}

#if defined(BCM_TRIDENT2_SUPPORT)

/*
 * add a new entry to ingress vlan action profile table(without modification)
 */
int
_bcm_trx_ing_vlan_action_profile_entry_no_mod_add(
                               int                                 unit, 
                               ing_vlan_tag_action_profile_entry_t *entry, 
                               uint32                              *index)
{
    int rv = BCM_E_NONE;
    void *entries[2];

    entries[0] = entry;
    entries[1] = entry;
    ING_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_add(unit, ing_action_profile[unit],
                             (void *) &entries, 1, index);
    ING_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}

/*
 * add a new entry to egress vlan action profile table(without modification)
 */
int
_bcm_trx_egr_vlan_action_profile_entry_no_mod_add(
                               int                                 unit, 
                               egr_vlan_tag_action_profile_entry_t *entry, 
                               uint32                              *index)
{
    int rv = BCM_E_NONE;
    void *entries[2];

    entries[0] = entry;
    entries[1] = entry;
    EGR_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_add(unit, egr_action_profile[unit],
                               (void *) &entries, 1, index);
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}

#endif


/*
 * Function : _bcm_trx_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for a vlan action profile table entry
 */
void
_bcm_trx_vlan_action_profile_entry_increment(int unit, uint32 index)
{   
    /* In case other module try to increase default index */
    if (index == 0) {
        index = ing_action_profile_def[unit];
    }
    ING_VLAN_TAG_ACTION_LOCK(unit);
    (void)soc_profile_mem_reference(unit, ing_action_profile[unit], index, 1);
    ING_VLAN_TAG_ACTION_UNLOCK(unit);
}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_get
 *
 * Purpose  : get a copy of cached vlan action profile table
 */
void
_bcm_trx_vlan_action_profile_entry_get(int unit,
                                      bcm_vlan_action_set_t *action, 
                                      uint32 index)
{
    ing_vlan_tag_action_profile_entry_t p1;
    ing_vlan_tag_action_profile_2_entry_t p2;
    void *entries[2];
    uint32 *entry;
    uint32 val;
    soc_mem_t mem = ING_VLAN_TAG_ACTION_PROFILEm;

    if (SOC_MEM_IS_VALID(unit, ING_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem = ING_VLAN_TAG_ACTION_PROFILE_2m;
        entries[0] = &p2;
        entries[1] = &p2;
    } else {
        entries[0] = &p1;
    }

    ING_VLAN_TAG_ACTION_LOCK(unit);
    (void) soc_profile_mem_get(unit, ing_action_profile[unit],
                               index, 1, entries);
    entry = entries[0];

    val = soc_mem_field32_get(unit, mem, entry, DT_OTAG_ACTIONf);
    action->dt_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, DT_POTAG_ACTIONf);
    action->dt_outer_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, DT_ITAG_ACTIONf);
    action->dt_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, DT_PITAG_ACTIONf);
    action->dt_inner_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SOT_OTAG_ACTIONf);
    action->ot_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SOT_POTAG_ACTIONf);
    action->ot_outer_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SOT_ITAG_ACTIONf);
    action->ot_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SIT_OTAG_ACTIONf);
    action->it_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SIT_ITAG_ACTIONf);
    action->it_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SIT_PITAG_ACTIONf);
    action->it_inner_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, UT_OTAG_ACTIONf);
    action->ut_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, UT_ITAG_ACTIONf);
    action->ut_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        val = soc_mem_field32_get(unit, mem, entry, DT_OPRI_ACTIONf);
        action->dt_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_OCFI_ACTIONf);
        action->dt_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_IPRI_ACTIONf);
        action->dt_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_ICFI_ACTIONf);
        action->dt_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_OPRI_ACTIONf);
        action->ot_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_OCFI_ACTIONf);
        action->ot_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_IPRI_ACTIONf);
        action->ot_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_ICFI_ACTIONf);
        action->ot_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_OPRI_ACTIONf);
        action->it_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_OCFI_ACTIONf);
        action->it_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_IPRI_ACTIONf);
        action->it_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_ICFI_ACTIONf);
        action->it_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_OPRI_ACTIONf);
        action->ut_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_OCFI_ACTIONf);
        action->ut_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_IPRI_ACTIONf);
        action->ut_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_ICFI_ACTIONf);
        action->ut_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

#if defined(BCM_TRIDENT2_SUPPORT)

        if (soc_feature(unit, soc_feature_fcoe)) {

            /* overwrite only fcoe related values (pri) */

            val = soc_mem_field32_get(unit, mem, entry, DT_OPRI_ACTIONf);
            action->dt_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, DT_IPRI_ACTIONf);
            action->dt_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SOT_OPRI_ACTIONf);
            action->ot_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SOT_IPRI_ACTIONf);
            action->ot_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SIT_OPRI_ACTIONf);
            action->it_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SIT_IPRI_ACTIONf);
            action->it_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, UT_OPRI_ACTIONf);
            action->ut_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, UT_IPRI_ACTIONf);
            action->ut_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);
                
        }
#endif /* BCM_TRIDENT2_SUPPORT */

    } else {
        action->dt_outer_pkt_prio = bcmVlanActionNone;
        action->dt_outer_cfi = bcmVlanActionNone;
        action->dt_inner_pkt_prio = bcmVlanActionNone;
        action->dt_inner_cfi = bcmVlanActionNone;
        action->ot_outer_pkt_prio = bcmVlanActionNone;
        action->ot_outer_cfi = bcmVlanActionNone;
        action->ot_inner_pkt_prio = bcmVlanActionNone;
        action->ot_inner_cfi = bcmVlanActionNone;
        action->it_outer_pkt_prio = bcmVlanActionNone;
        action->it_outer_cfi = bcmVlanActionNone;
        action->it_inner_pkt_prio = bcmVlanActionNone;
        action->it_inner_cfi = bcmVlanActionNone;
        action->ut_outer_pkt_prio = bcmVlanActionNone;
        action->ut_outer_cfi = bcmVlanActionNone;
        action->ut_inner_pkt_prio = bcmVlanActionNone;
        action->ut_inner_cfi = bcmVlanActionNone;
    }
    ING_VLAN_TAG_ACTION_UNLOCK(unit);

}

/*
 * Function : _bcm_trx_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from vlan action profile table
 */
int
_bcm_trx_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    int rv = BCM_E_NONE;

    if (index >= ing_action_profile_def[unit]) {
        ING_VLAN_TAG_ACTION_LOCK(unit);
        rv = soc_profile_mem_delete(unit, ing_action_profile[unit], index);
        ING_VLAN_TAG_ACTION_UNLOCK(unit);
    }
    return rv;
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_add
 *
 * Purpose  : add a new entry to egress vlan action profile table
 */
int
_bcm_trx_egr_vlan_action_profile_entry_add(int unit,
                                          bcm_vlan_action_set_t *action,
                                          uint32 *index)
{
    int rv = BCM_E_NONE;
    egr_vlan_tag_action_profile_entry_t entry1;
    egr_vlan_tag_action_profile_2_entry_t entry2;
    void *entries[2];
    void *entry;
    soc_mem_t mem = EGR_VLAN_TAG_ACTION_PROFILEm;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem = EGR_VLAN_TAG_ACTION_PROFILE_2m;
        sal_memset(&entry2, 0, sizeof(entry2));
        entry = &entry2;
    } else {
        sal_memset(&entry1, 0, sizeof(entry1));
        entry = &entry1;
    }

    soc_mem_field32_set(unit, mem, entry, DT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_outer));
    soc_mem_field32_set(unit, mem, entry, DT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_inner));
    soc_mem_field32_set(unit, mem, entry, DT_PITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->dt_inner_prio));
    soc_mem_field32_set(unit, mem, entry, SOT_OTAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer));
    soc_mem_field32_set(unit, mem, entry, SOT_ITAG_ACTIONf,
                        _BCM_TRX_TAG_ACTION_ENCODE(action->ot_inner));

#if defined(BCM_TRIDENT2_SUPPORT)

    if (soc_feature(unit, soc_feature_fcoe)) {
        soc_mem_field32_set(unit, mem, entry, DT_OTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE_EGR_FCOE_EXT(
                                                           action->dt_outer));
        soc_mem_field32_set(unit, mem, entry, DT_ITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE_EGR_FCOE_EXT(
                                                           action->dt_inner));
        soc_mem_field32_set(unit, mem, entry, DT_PITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE_EGR_FCOE_EXT(
                                                       action->dt_inner_prio));
        soc_mem_field32_set(unit, mem, entry, SOT_OTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE_EGR_FCOE_EXT(
                                                           action->ot_outer));
        soc_mem_field32_set(unit, mem, entry, SOT_ITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE_EGR_FCOE_EXT(
                                                           action->ot_inner));
    }

#endif  /* BCM_TRIDENT2_SUPPORT */

    if (soc_feature(unit, soc_feature_vlan_egr_it_inner_replace)) {
        soc_mem_field32_set(unit, mem, entry, DT_POTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->dt_outer_prio));
        soc_mem_field32_set(unit, mem, entry, SOT_POTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->ot_outer_prio));
        soc_mem_field32_set(unit, mem, entry, SIT_OTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->it_outer));
        soc_mem_field32_set(unit, mem, entry, SIT_ITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->it_inner));
        soc_mem_field32_set(unit, mem, entry, SIT_PITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->it_inner_prio));
        soc_mem_field32_set(unit, mem, entry, UT_OTAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->ut_outer));
        soc_mem_field32_set(unit, mem, entry, UT_ITAG_ACTIONf,
                            _BCM_TRX_TAG_ACTION_ENCODE(action->ut_inner));
    }

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_mem_field32_set
            (unit, mem, entry, DT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_outer_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, DT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_outer_cfi));
        soc_mem_field32_set
            (unit, mem, entry, DT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_inner_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, DT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->dt_inner_cfi));
        soc_mem_field32_set
            (unit, mem, entry, SOT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, SOT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_cfi));
        soc_mem_field32_set
            (unit, mem, entry, SOT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_inner_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, SOT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_inner_cfi));
        soc_mem_field32_set
            (unit, mem, entry, SIT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_outer_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, SIT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_outer_cfi));
        soc_mem_field32_set
            (unit, mem, entry, SIT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_inner_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, SIT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->it_inner_cfi));
        soc_mem_field32_set
            (unit, mem, entry, UT_OPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, UT_OCFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer_cfi));
        soc_mem_field32_set
            (unit, mem, entry, UT_IPRI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_inner_pkt_prio));
        soc_mem_field32_set
            (unit, mem, entry, UT_ICFI_ACTIONf,
             _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_inner_cfi));

#if defined(BCM_TRIDENT2_SUPPORT)

        if (soc_feature(unit, soc_feature_fcoe)) {

            /* overwrite fcoe related fields (pri) */
            soc_mem_field32_set
                (unit, mem, entry, DT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->dt_outer_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, DT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->dt_inner_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, SOT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->ot_outer_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, SOT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->ot_inner_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, SIT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->it_outer_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, SIT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->it_inner_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, UT_OPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->ut_outer_pkt_prio));
            soc_mem_field32_set
                (unit, mem, entry, UT_IPRI_ACTIONf,
                 _BCM_TRX_PRI_CFI_ACTION_ENCODE_FCOE_EXT(
                                                action->ut_inner_pkt_prio));
        }
#endif  /* BCM_TRIDENT2_SUPPORT */
    }

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_mem_field_valid(unit, mem, OTPID_ACTIONf)) {
        soc_mem_field32_set
            (unit, mem, entry, OTPID_ACTIONf,
             _BCM_TD3_TPID_ACTION_ENCODE(action->outer_tpid_action));
    }
#endif

    entries[0] = entry;
    entries[1] = entry;
    EGR_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_add(unit, egr_action_profile[unit],
                             (void *) &entries, 1, index);
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_increment
 *
 * Purpose  : increment the refcount for an egress vlan action profile table entry
 */
void
_bcm_trx_egr_vlan_action_profile_entry_increment(int unit, uint32 index)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        _bcm_th3_egr_vlan_action_profile_entry_increment(unit, index);
        return;
    }
#endif/* BCM_TOMAHAWK3_SUPPORT */
    EGR_VLAN_TAG_ACTION_LOCK(unit);
    soc_profile_mem_reference(unit, egr_action_profile[unit],
                              index, 1);
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_get
 *
 * Purpose  : get a copy of cached egress vlan action profile table
 */
void
_bcm_trx_egr_vlan_action_profile_entry_get(int unit,
                                          bcm_vlan_action_set_t *action, 
                                          uint32 index)
{
    egr_vlan_tag_action_profile_entry_t p1;
    egr_vlan_tag_action_profile_2_entry_t p2;
    void *entries[2];
    uint32 *entry;
    uint32 val;
    soc_mem_t mem = EGR_VLAN_TAG_ACTION_PROFILEm;

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_TAG_ACTION_PROFILE_2m)) {
        mem = EGR_VLAN_TAG_ACTION_PROFILE_2m;
        entries[0] = &p1;
        entries[1] = &p2;
    } else {
        entries[0] = &p1;
    }

    EGR_VLAN_TAG_ACTION_LOCK(unit);
    (void) soc_profile_mem_get(unit, egr_action_profile[unit],
                               index, 1, entries);
    entry = entries[0];

    val = soc_mem_field32_get(unit, mem, entry, DT_OTAG_ACTIONf);
    action->dt_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, DT_ITAG_ACTIONf);
    action->dt_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, DT_PITAG_ACTIONf);
    action->dt_inner_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SOT_OTAG_ACTIONf);
    action->ot_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

    val = soc_mem_field32_get(unit, mem, entry, SOT_ITAG_ACTIONf);
    action->ot_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

#if defined(BCM_TRIDENT3_SUPPORT)
    if (soc_mem_field_valid(unit, mem, OTPID_ACTIONf)) {
        val = soc_mem_field32_get(unit, mem, entry, OTPID_ACTIONf);
        action->outer_tpid_action = _BCM_TD3_TPID_ACTION_DECODE(val);
    }
#endif

#if defined(BCM_TRIDENT2_SUPPORT)

    if (soc_feature(unit, soc_feature_fcoe)) {
        val = soc_mem_field32_get(unit, mem, entry, DT_OTAG_ACTIONf);
        action->dt_outer = _BCM_TRX_TAG_ACTION_DECODE_EGR_FCOE_EXT(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_ITAG_ACTIONf);
        action->dt_inner = _BCM_TRX_TAG_ACTION_DECODE_EGR_FCOE_EXT(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_PITAG_ACTIONf);
        action->dt_inner_prio = _BCM_TRX_TAG_ACTION_DECODE_EGR_FCOE_EXT(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_OTAG_ACTIONf);
        action->ot_outer = _BCM_TRX_TAG_ACTION_DECODE_EGR_FCOE_EXT(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_ITAG_ACTIONf);
        action->ot_inner = _BCM_TRX_TAG_ACTION_DECODE_EGR_FCOE_EXT(val);
    }

#endif  /* BCM_TRIDENT2_SUPPORT */

    if (soc_feature(unit, soc_feature_vlan_egr_it_inner_replace)) {
        val = soc_mem_field32_get(unit, mem, entry, DT_POTAG_ACTIONf);
        action->dt_outer_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_POTAG_ACTIONf);
        action->ot_outer_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_OTAG_ACTIONf);
        action->it_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_ITAG_ACTIONf);
        action->it_inner = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_PITAG_ACTIONf);
        action->it_inner_prio = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_OTAG_ACTIONf);
        action->ut_outer = _BCM_TRX_TAG_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_ITAG_ACTIONf);
        action->ut_inner = _BCM_TRX_TAG_ACTION_DECODE(val);
    } else {
        action->it_outer = bcmVlanActionNone;
        action->it_inner = bcmVlanActionNone;
        action->it_inner_prio = bcmVlanActionNone;
        action->ut_outer = bcmVlanActionNone;
        action->ut_inner = bcmVlanActionNone;
    }

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        val = soc_mem_field32_get(unit, mem, entry, DT_OPRI_ACTIONf);
        action->dt_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_OCFI_ACTIONf);
        action->dt_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_IPRI_ACTIONf);
        action->dt_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, DT_ICFI_ACTIONf);
        action->dt_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_OPRI_ACTIONf);
        action->ot_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_OCFI_ACTIONf);
        action->ot_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_IPRI_ACTIONf);
        action->ot_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SOT_ICFI_ACTIONf);
        action->ot_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_OPRI_ACTIONf);
        action->it_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_OCFI_ACTIONf);
        action->it_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_IPRI_ACTIONf);
        action->it_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, SIT_ICFI_ACTIONf);
        action->it_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_OPRI_ACTIONf);
        action->ut_outer_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_OCFI_ACTIONf);
        action->ut_outer_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_IPRI_ACTIONf);
        action->ut_inner_pkt_prio = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

        val = soc_mem_field32_get(unit, mem, entry, UT_ICFI_ACTIONf);
        action->ut_inner_cfi = _BCM_TRX_PRI_CFI_ACTION_DECODE(val);

#if defined(BCM_TRIDENT2_SUPPORT)

        if (soc_feature(unit, soc_feature_fcoe)) {

            /* overwrite fcoe related values (pri) */
            val = soc_mem_field32_get(unit, mem, entry, DT_OPRI_ACTIONf);
            action->dt_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, DT_IPRI_ACTIONf);
            action->dt_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SOT_OPRI_ACTIONf);
            action->ot_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SOT_IPRI_ACTIONf);
            action->ot_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SIT_OPRI_ACTIONf);
            action->it_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, SIT_IPRI_ACTIONf);
            action->it_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry, UT_OPRI_ACTIONf);
            action->ut_outer_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

            val = soc_mem_field32_get(unit, mem, entry,
                                      UT_IPRI_ACTIONf);
            action->ut_inner_pkt_prio = 
                _BCM_TRX_PRI_CFI_ACTION_DECODE_FCOE_EXT(val);

        }

#endif  /* BCM_TRIDENT2_SUPPORT */

    } else {
        action->dt_outer_pkt_prio = bcmVlanActionNone;
        action->dt_outer_cfi = bcmVlanActionNone;
        action->dt_inner_pkt_prio = bcmVlanActionNone;
        action->dt_inner_cfi = bcmVlanActionNone;
        action->ot_outer_pkt_prio = bcmVlanActionNone;
        action->ot_outer_cfi = bcmVlanActionNone;
        action->ot_inner_pkt_prio = bcmVlanActionNone;
        action->ot_inner_cfi = bcmVlanActionNone;
        action->it_outer_pkt_prio = bcmVlanActionNone;
        action->it_outer_cfi = bcmVlanActionNone;
        action->it_inner_pkt_prio = bcmVlanActionNone;
        action->it_inner_cfi = bcmVlanActionNone;
        action->ut_outer_pkt_prio = bcmVlanActionNone;
        action->ut_outer_cfi = bcmVlanActionNone;
        action->ut_inner_pkt_prio = bcmVlanActionNone;
        action->ut_inner_cfi = bcmVlanActionNone;
    }
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
}

/*
 * Function : _bcm_trx_egr_vlan_action_profile_entry_delete
 *
 * Purpose  : remove an entry from egress vlan action profile table
 */
int
_bcm_trx_egr_vlan_action_profile_entry_delete(int unit, uint32 index)
{
    int rv = BCM_E_NONE;

    EGR_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_delete(unit, egr_action_profile[unit], index);
    EGR_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}

STATIC soc_field_t _tr_range_min_f[] = {VLAN_MIN_0f, VLAN_MIN_1f, 
                                        VLAN_MIN_2f, VLAN_MIN_3f,
                                        VLAN_MIN_4f, VLAN_MIN_5f, 
                                        VLAN_MIN_6f, VLAN_MIN_7f};
STATIC soc_field_t _tr_range_max_f[] = {VLAN_MAX_0f, VLAN_MAX_1f, 
                                        VLAN_MAX_2f, VLAN_MAX_3f,
                                        VLAN_MAX_4f, VLAN_MAX_5f, 
                                        VLAN_MAX_6f, VLAN_MAX_7f}; 

/*
 * Function : bcm_trx_vlan_range_profile_init
 *
 * Purpose  : to initialize hardware ING_VLAN_RANGE table
 *            allocate memory to cache hardware tables in RAM.
 *
 * Note:
 *      Allocate memory to cache the profile tables and initialize.
 *      If memory to cache the profile table is already allocated, just
 *      initialize the table.
 */
STATIC int
_bcm_trx_vlan_range_profile_init(int unit)
{
    int i, idx, inner_idx;
    source_trunk_map_table_entry_t stm_entry;
    trunk32_port_table_entry_t trunk32_entry;
    ing_vlan_range_entry_t profile_entry;
    uint32 temp_index, temp_inner_index;
    soc_mem_t mem;
    int entry_words;
    void *entries[1];
    soc_field_t field;

    /* Initialize the ING_VLAN_RANGE table */

    if (vlan_range_profile[unit] == NULL) {
        vlan_range_profile[unit] = sal_alloc(sizeof(soc_profile_mem_t),
                                             "Vlan Range Profile Mem");
        if (vlan_range_profile[unit] == NULL) {
            return BCM_E_MEMORY;
        }
        soc_profile_mem_t_init(vlan_range_profile[unit]);
    }

    /* Create profile table cache (or re-init if it already exists) */
    mem = ING_VLAN_RANGEm;
    entry_words = sizeof(ing_vlan_range_entry_t) / sizeof(uint32);
    SOC_IF_ERROR_RETURN(soc_profile_mem_create(unit, &mem, &entry_words, 1,
                                               vlan_range_profile[unit]));

    if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                OUTER_VLAN_RANGE_IDXf)) {
        field = OUTER_VLAN_RANGE_IDXf;
    } else {
        field = VLAN_RANGE_IDXf;
    }
    if (SOC_WARM_BOOT(unit)) {
        /* Increment the ref count for all ports */
        for (i = 0;
             i < soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm); i++) {
            SOC_IF_ERROR_RETURN
                (READ_SOURCE_TRUNK_MAP_TABLEm(unit,
                                              MEM_BLOCK_ANY, i, &stm_entry));
       
            idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, field);
            SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], idx, 1);
            SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, vlan_range_profile[unit],
                                            idx, 1);
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                   INNER_VLAN_RANGE_IDXf)) {
                inner_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                         &stm_entry, INNER_VLAN_RANGE_IDXf);
                SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit],
                                         inner_idx, 1);
                SOC_PROFILE_MEM_ENTRIES_PER_SET(unit, vlan_range_profile[unit],
                                         inner_idx, 1);
            }
        }

        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            if (!soc_property_get(unit, spn_TRUNK_EXTEND, 1)) {
                for (i = 0;
                     i < soc_mem_index_count(unit, TRUNK32_PORT_TABLEm); i++) {
                    SOC_IF_ERROR_RETURN
                        (READ_TRUNK32_PORT_TABLEm(unit,
                                            MEM_BLOCK_ANY, i, &trunk32_entry));
                    idx = soc_mem_field32_get(unit, TRUNK32_PORT_TABLEm,
                            &trunk32_entry, VLAN_RANGE_IDXf);
                    SOC_PROFILE_MEM_REFERENCE(unit,
                            vlan_range_profile[unit], idx, 1);
                    SOC_PROFILE_MEM_ENTRIES_PER_SET(unit,
                            vlan_range_profile[unit], idx, 1);
                }
            }
        }

        /* One extra increment to preserve location VLAN_RANGE_PROFILE_DEFAULT */
        SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], 
                                  VLAN_RANGE_PROFILE_DEFAULT, 1);

    } else {
        int rv, size;
        source_trunk_map_table_entry_t *ent;
        uint8 *mbuf;

        sal_memset(&profile_entry, 0, sizeof(ing_vlan_range_entry_t));
        for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
            soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                                &profile_entry, _tr_range_min_f[i], 1);
            soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                                &profile_entry, _tr_range_max_f[i], 0);
        }

        /* Initialize the VLAN_RANGE_PROFILE_DEFAULT to have 
         * all ranges unused (min == 1, max == 0).
         */
        entries[0] = &profile_entry;
        SOC_IF_ERROR_RETURN
            (soc_profile_mem_add(unit, vlan_range_profile[unit],
                                 (void *) &entries, 1, &temp_index));

        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                INNER_VLAN_RANGE_IDXf)) {
            SOC_IF_ERROR_RETURN
                (soc_profile_mem_add(unit, vlan_range_profile[unit],
                                 (void *) &entries, 1, &temp_inner_index));
        }

        size = SOC_MEM_TABLE_BYTES(unit, SOURCE_TRUNK_MAP_TABLEm);
        mbuf = soc_cm_salloc(unit, size, "SOURCE_TRUNK_MAP_TABLE");
        if (NULL == mbuf) {
            return BCM_E_MEMORY;
        }

        soc_mem_lock(unit, SOURCE_TRUNK_MAP_TABLEm);
        rv = soc_mem_read_range(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                                soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm),
                                soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm),
                                mbuf);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, mbuf);
            soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
            return rv;
        }
        /* Increment the ref count for all ports */
        for (i = 0;
             i < soc_mem_index_count(unit, SOURCE_TRUNK_MAP_TABLEm); i++) {
            ent = soc_mem_table_idx_to_pointer(unit, 
                        SOURCE_TRUNK_MAP_TABLEm, 
                        source_trunk_map_table_entry_t *, mbuf, i);

            soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, ent, field,
                                temp_index);

            SOC_PROFILE_MEM_REFERENCE(unit,
                    vlan_range_profile[unit], temp_index, 1);

            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                    INNER_VLAN_RANGE_IDXf)) {
                soc_mem_field32_set(unit, SOURCE_TRUNK_MAP_TABLEm, ent,
                                    INNER_VLAN_RANGE_IDXf, temp_inner_index);
                SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], 
                                          temp_inner_index, 1);
            }
        }
        rv = soc_mem_write_range(unit, SOURCE_TRUNK_MAP_TABLEm, MEM_BLOCK_ANY,
                                 soc_mem_index_min(unit, SOURCE_TRUNK_MAP_TABLEm),
                                 soc_mem_index_max(unit, SOURCE_TRUNK_MAP_TABLEm),
                                 mbuf);
        soc_cm_sfree(unit, mbuf);
        soc_mem_unlock(unit, SOURCE_TRUNK_MAP_TABLEm);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            if (!soc_property_get(unit, spn_TRUNK_EXTEND, 1)) {
                for (i = 0;
                     i < soc_mem_index_count(unit, TRUNK32_PORT_TABLEm); i++) {
                    if (soc_mem_field32_modify(unit, TRUNK32_PORT_TABLEm, i, 
                            VLAN_RANGE_IDXf, temp_index) != BCM_E_NONE) {
                        continue;
                    }
                    SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit],
                            temp_index, 1);
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_add
 *
 * Purpose  : add a new entry to vlan range profile table
 */
int
_bcm_trx_vlan_range_profile_entry_add(int unit, bcm_vlan_t *min_vlan,
                                     bcm_vlan_t *max_vlan, uint32 *index)
{   
    ing_vlan_range_entry_t profile_entry;
    void *entries[1];
    int i, rv = BCM_E_NONE;

    sal_memset(&profile_entry, 0, sizeof(ing_vlan_range_entry_t));

    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
        soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                            &profile_entry, _tr_range_min_f[i], min_vlan[i]);
        soc_mem_field32_set(unit, ING_VLAN_RANGEm,
                            &profile_entry, _tr_range_max_f[i], max_vlan[i]);
    }
    entries[0] = &profile_entry;
    VLAN_RANGE_LOCK(unit);
    rv = soc_profile_mem_add(unit, vlan_range_profile[unit],
                             (void *) &entries, 1, index);
    VLAN_RANGE_UNLOCK(unit);
    return rv;
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_increment
 *
 * Purpose  : increment the refcount for a vlan range profile table entry
 */
void
_bcm_trx_vlan_range_profile_entry_increment(int unit, uint32 index)
{
    VLAN_RANGE_LOCK(unit);
    SOC_PROFILE_MEM_REFERENCE(unit, vlan_range_profile[unit], index, 1);
    VLAN_RANGE_UNLOCK(unit);
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_get
 *
 * Purpose  : get a copy of cached vlan range profile table
 */
void
_bcm_trx_vlan_range_profile_entry_get(int unit, bcm_vlan_t *min_vlan, 
                                     bcm_vlan_t *max_vlan, uint32 index)
{
    int i;
    ing_vlan_range_entry_t *profile_entry;

    VLAN_RANGE_LOCK(unit);
    profile_entry = SOC_PROFILE_MEM_ENTRY(unit, vlan_range_profile[unit],
                                          ing_vlan_range_entry_t *,
                                          index);
    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
        min_vlan[i] = soc_mem_field32_get(unit, ING_VLAN_RANGEm,
                                          profile_entry, _tr_range_min_f[i]);
        max_vlan[i] = soc_mem_field32_get(unit, ING_VLAN_RANGEm,
                                          profile_entry, _tr_range_max_f[i]);
    }
    VLAN_RANGE_UNLOCK(unit);
}

/*
 * Function : _bcm_trx_vlan_range_profile_entry_delete
 *
 * Purpose  : remove an entry from vlan range profile table
 */
int
_bcm_trx_vlan_range_profile_entry_delete(int unit, uint32 index)
{
    int rv = BCM_E_NONE;
    VLAN_RANGE_LOCK(unit);
    rv = soc_profile_mem_delete(unit, vlan_range_profile[unit], index);
    VLAN_RANGE_UNLOCK(unit);
    return rv;
}

/*
 * On XGS3, VLAN_SUBNET.IP_ADDR stores only a top half (64 bits) of
 * the IPV6 address. Comparing it against a full length of a parameter
 * will have certainly result in a mismatch. There should be a
 * special type for a stored part of the address; meanwhile, a
 * symbolic constant is used.
 */

#define VLAN_SUBNET_IP_ADDR_LENGTH 8

/*
 * Function:
 *      _trx_vlan_ip_addr_mask_get
 * Purpose:
 *      Get IPV6 address and mask from vlan_ip structure
 * Parameters:
 *      vlan_ip -   structure specifying IP address and other info
 *      ip6addr -   IPv6 address to retrieve
 *      ip6mask -   IPv6 mask to retrieve
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_trx_vlan_ip_addr_mask_get(bcm_vlan_ip_t *vlan_ip, bcm_ip6_t ip6addr, 
                           bcm_ip6_t ip6mask)
{
    if (vlan_ip->flags & BCM_VLAN_SUBNET_IP6) {
        if (vlan_ip->prefix > 64) {
            return BCM_E_PARAM;
        }

        bcm_ip6_mask_create(ip6mask, vlan_ip->prefix); 
        sal_memcpy(ip6addr, vlan_ip->ip6, sizeof(bcm_ip6_t));
    } else { /* IPv4 entry */
        sal_memcpy(ip6addr, "\xff\xff\x00\x00", 4);
        ip6addr[4] = vlan_ip->ip4 >> 24;
        ip6addr[5] = (vlan_ip->ip4 >> 16) & 0xff;
        ip6addr[6] = (vlan_ip->ip4 >> 8) & 0xff;
        ip6addr[7] = vlan_ip->ip4 & 0xff;

        sal_memcpy(ip6mask, "\xff\xff\xff\xff", 4);
        ip6mask[4] = vlan_ip->mask >> 24;
        ip6mask[5] = (vlan_ip->mask >> 16) & 0xff;
        ip6mask[6] = (vlan_ip->mask >> 8) & 0xff;
        ip6mask[7] = vlan_ip->mask & 0xff;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _tr_vlan_subnet_mem_read
 * Purpose:
 *      Helper function to allocated memory and read by range all 
 *      VLAN_SUBNET memory
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      vstab   -       (OUT) pointer to memory
 *      nent    -       (OUT) total number of entries in the memory
 * Returns:
 *      BCM_E_XXX
 * Note: 
 *      This routine will allocate a memory, it is up to caller responsibility
 *      to free it 
 */
STATIC int 
_tr_vlan_subnet_mem_read(int unit, vlan_subnet_entry_t **vstab, int *nent)
{
    int imin, imax, vsbytes, rv;

    imin = soc_mem_index_min(unit, VLAN_SUBNETm);
    imax = soc_mem_index_max(unit, VLAN_SUBNETm);
    *nent = soc_mem_index_count(unit, VLAN_SUBNETm);
    vsbytes = soc_mem_entry_words(unit, VLAN_SUBNETm);
    vsbytes = WORDS2BYTES(vsbytes);
    *vstab = soc_cm_salloc(unit, *nent * sizeof(vlan_subnet_entry_t), "vlan_subnet");
    if (*vstab == NULL) {
        return BCM_E_MEMORY;
    }

    rv = soc_mem_read_range(unit, VLAN_SUBNETm, MEM_BLOCK_ANY,
                            imin, imax, *vstab);
    return rv;
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_parse
 * Purpose:
 *      Helper function to parse all information fields from 
 *      VLAN_SUBNET memory entry
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      ventry   -      (IN) pointer to a vlan subnet entry 
 *      subnet_fields - (OUT) Structure contains all entry fields
 * Returns:
 *      none
 * Note: 
 */
STATIC void 
_trx_vlan_subnet_entry_parse(int unit, vlan_subnet_entry_t *ventry, 
                           _bcm_trx_vlan_subnet_entry_t *subnet_fields)
{
    bcm_ip6_t   ip6tmp;
    vlan_subnet_entry_t entry;
    uint32      fval[SOC_MAX_MEM_WORDS];

    soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, ventry, IP_ADDRf,
                         subnet_fields->ip, SOC_MEM_IP6_UPPER_ONLY);

    if (soc_mem_field_valid(unit, VLAN_SUBNETm, KEYf)) {
        soc_mem_field_get(unit, VLAN_SUBNETm, (uint32 *)ventry, MASKf, fval);
        soc_mem_field_set(unit, VLAN_SUBNETm, (uint32 *)&entry, KEYf, fval);
        soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, &entry, IP_ADDRf,
                             ip6tmp, SOC_MEM_IP6_UPPER_ONLY);
    } else {
        soc_mem_ip6_addr_get(unit, VLAN_SUBNETm, ventry, MASKf,
                             ip6tmp, SOC_MEM_IP6_UPPER_ONLY);
    }
    subnet_fields->prefix = bcm_ip6_mask_length(ip6tmp);

    subnet_fields->ovid = soc_VLAN_SUBNETm_field32_get(unit, ventry, OVIDf);
    subnet_fields->ivid = soc_VLAN_SUBNETm_field32_get(unit, ventry, IVIDf);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        subnet_fields->opri =
            soc_VLAN_SUBNETm_field32_get(unit, ventry, OPRIf);
        subnet_fields->ocfi =
            soc_VLAN_SUBNETm_field32_get(unit, ventry, OCFIf);
        subnet_fields->ipri =
            soc_VLAN_SUBNETm_field32_get(unit, ventry, IPRIf);
        subnet_fields->icfi =
            soc_VLAN_SUBNETm_field32_get(unit, ventry, ICFIf);
    } else {
        subnet_fields->opri = soc_VLAN_SUBNETm_field32_get(unit, ventry, PRIf);
    }
    subnet_fields->profile_idx = soc_VLAN_SUBNETm_field32_get(unit, ventry, 
                                                    TAG_ACTION_PROFILE_PTRf);
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_set
 * Purpose:
 *      Helper function to write all fields to 
 *      VLAN_SUBNET memory entry
 * Parameters:
 *      unit    -       (IN) BCM Device unit
 *      ventry   -      (IN) pointer to a vlan subnet entry 
 *      subnet_fields - (IN) Structure contains all entry fields
 * Returns:
 *      none
 * Note: 
 *      This routine assumes that all rovided fields has correct values
 */

STATIC void 
_trx_vlan_subnet_entry_set(int unit, vlan_subnet_entry_t *ventry, 
                           _bcm_trx_vlan_subnet_entry_t *subnet_fields)
{
    uint32      fval[SOC_MAX_MEM_WORDS];

    sal_memset(fval, 0, sizeof(fval));
    if (soc_mem_field_valid(unit, VLAN_SUBNETm, KEYf)) {
        soc_mem_field_set(unit, VLAN_SUBNETm, (uint32 *)ventry, KEYf, fval);
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, ventry, IP_ADDRf,
                             subnet_fields->mask, SOC_MEM_IP6_UPPER_ONLY);
        soc_mem_field_get(unit, VLAN_SUBNETm, (uint32 *)ventry, KEYf, fval);
        soc_mem_field_set(unit, VLAN_SUBNETm, (uint32 *)ventry, MASKf, fval);
    } else {
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, ventry, MASKf,
                             subnet_fields->mask, SOC_MEM_IP6_UPPER_ONLY);
    }
    soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, ventry, IP_ADDRf,
                         subnet_fields->ip, SOC_MEM_IP6_UPPER_ONLY);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, OVIDf, subnet_fields->ovid);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, IVIDf, subnet_fields->ivid);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_VLAN_SUBNETm_field32_set(unit, ventry, OPRIf, subnet_fields->opri);
        soc_VLAN_SUBNETm_field32_set(unit, ventry, OCFIf, subnet_fields->ocfi);
        soc_VLAN_SUBNETm_field32_set(unit, ventry, IPRIf, subnet_fields->ipri);
        soc_VLAN_SUBNETm_field32_set(unit, ventry, ICFIf, subnet_fields->icfi);
    } else {
        soc_VLAN_SUBNETm_field32_set(unit, ventry, PRIf, subnet_fields->opri);
    }
    soc_VLAN_SUBNETm_field32_set(unit, ventry, TAG_ACTION_PROFILE_PTRf, 
                                 subnet_fields->profile_idx);
    soc_VLAN_SUBNETm_field32_set(unit, ventry, VALIDf, 1);
}

/*
 * Function:
 *      _trx_vlan_subnet_lookup
 * Purpose:
 *      Get index into table with settings matching to subnet
 * Parameters:
 *      unit            (IN) BCM Device unit
 *      vstab           (IN) pointer to memory
 *      nent            (IN) number of entries in the memory
 *      ip              (IN) IP address
 *      ip_mask         (IN) IP address mask
 *      entry_index     (OUT) target entry index (if BCM_E_NONE)
 *                            index to be used for insert or -1 when full
 *                            (if BCM_E_NOT_FOUND)
 *      free_index      (OUT) (optional) index of last used entry + 1
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_trx_vlan_subnet_lookup(int unit, vlan_subnet_entry_t *vstab, int nent,
                        bcm_ip6_t ip, bcm_ip6_t ip_mask, int *entry_index,
                        int *free_index)
{
    bcm_ip6_t           ip_key, ip6_full_mask;
    vlan_subnet_entry_t *vstabp, entry0, entry1, entry2, mask0, mask1;
    int                 index_min, index_max;
    int                 index, i, entry_words, rv;
    uint32              fval[SOC_MAX_MEM_WORDS];

    sal_memcpy(ip_key, ip, VLAN_SUBNET_IP_ADDR_LENGTH);
#ifdef BCM_TRIDENT_SUPPORT
    
    if (soc_feature(unit, soc_feature_xy_tcam)) {
        for (i = 0; i < VLAN_SUBNET_IP_ADDR_LENGTH; i++) {
            ip_key[i] &= ip_mask[i];
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    sal_memset(&ip6_full_mask, 0xff, sizeof(ip6_full_mask));
    entry_words = soc_mem_entry_words(unit, VLAN_SUBNETm);

    /* entry2 is for checking VALID bit */
    sal_memset(&entry2, 0, sizeof(entry2));
    soc_mem_field32_set(unit, VLAN_SUBNETm, &entry2, VALIDf, 1);

    /* entry1 and mask1 are for prefix length check */
    entry1 = entry2;
    if (soc_mem_field_valid(unit, VLAN_SUBNETm, KEYf)) {
        sal_memset(&entry0, 0, sizeof(entry0));
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &entry0, IP_ADDRf, ip_mask,
                             SOC_MEM_IP6_UPPER_ONLY);
        soc_mem_field_get(unit, VLAN_SUBNETm, (uint32 *)&entry0, KEYf, fval);
        soc_mem_field_set(unit, VLAN_SUBNETm, (uint32 *)&entry1, MASKf, fval);
        mask1 = entry1;
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &entry0, IP_ADDRf,
                             ip6_full_mask, SOC_MEM_IP6_UPPER_ONLY);
        soc_mem_field_get(unit, VLAN_SUBNETm, (uint32 *)&entry0, KEYf, fval);
        soc_mem_field_set(unit, VLAN_SUBNETm, (uint32 *)&mask1, MASKf, fval);
    } else {
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &entry1, MASKf, ip_mask,
                             SOC_MEM_IP6_UPPER_ONLY);
        mask1 = entry1;
        soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &mask1, MASKf,
                             ip6_full_mask, SOC_MEM_IP6_UPPER_ONLY);
    }

    /* entry0 and mask0 are for target entry matching */
    entry0 = entry1;
    soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &entry0, IP_ADDRf, ip_key,
                         SOC_MEM_IP6_UPPER_ONLY);
    mask0 = mask1;
    soc_mem_field32_set(unit, VLAN_SUBNETm, &mask0, VALIDf, 1);
    soc_mem_ip6_addr_set(unit, VLAN_SUBNETm, &mask0, IP_ADDRf, ip_mask,
                         SOC_MEM_IP6_UPPER_ONLY);

    /* Do binary search to find the first entry whose prefix length
     * is shorter than or equal to the target entry */
    index_min = 0;
    index_max = nent - 1;
    while (index_min < index_max) {
        index = (index_min + index_max) / 2;
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *,
                                              vstab, index);

        /* Check if the entry has longer prefix */
        for (i = 0; i < entry_words; i++) {
            if ((vstabp->entry_data[i] & mask1.entry_data[i]) >
                entry1.entry_data[i]) {
                break;
            }
        }
        if (i != entry_words) { /* prefix length is longer than target entry */
            index_min = index + 1;
        } else { /* prefix length is shorter than or equal to target entry */
            index_max = index;
        }
    }

    /* Match against entries with the same prefix length as target entry */
    rv = BCM_E_NOT_FOUND;
    *entry_index = -1;
    for (index = index_min; index < nent; index++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *,
                                              vstab, index);
        /* Try matching the entry */
        for (i = 0; i < entry_words; i++) {
            if ((vstabp->entry_data[i] ^ entry0.entry_data[i]) &
                mask0.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) {
            rv = BCM_E_NONE;
            *entry_index = index;
            break;
        }

        /* Find the first shorter prefix entry */
        for (i = 0; i < entry_words; i++) {
            if ((vstabp->entry_data[i] & mask1.entry_data[i]) <
                entry1.entry_data[i]) {
                break;
            }
        }
        if (i != entry_words) {
            *entry_index = index;
            break;
        }
    }


    if (free_index == NULL) {
        return rv;
    }

    /* Do binary search to find the index after the last used entry */
    index_min = index;
    index_max = nent - 1;
    while (index_min <= index_max) {
        index = (index_min + index_max) / 2;
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *,
                                              vstab, index);

        /* Check if the entry is valid */
        for (i = 0; i < entry_words; i++) {
            if (vstabp->entry_data[i] & entry2.entry_data[i]) {
                break;
            }
        }
        if (i == entry_words) { /* free entry */
            if (index_min == index_max) {
                break;
            }
            index_max = index;
        } else { /* entry in used */
            index_min = index + 1;
        }
    }

    *free_index = index_min;
    return rv;
}

/*
 * Function:
 *      _trx_vlan_subnet_entry_add
 * Purpose:
 *      Add vlan settings for given subnet
 * Parameters:
 *      unit            - (IN) BCM Device unit
 *      entry_fields    - (IN) Entry fields to add
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_entry_add(int unit,
                           _bcm_trx_vlan_subnet_entry_t *entry_fields)
{
    int                         rv, nent, i;
    vlan_subnet_entry_t         *vstab, *vstabp;
    uint32                      old_profile_idx;
    int                         entry_index, free_index;

    soc_mem_lock(unit, VLAN_SUBNETm);

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }

    rv = _trx_vlan_subnet_lookup(unit, vstab, nent, entry_fields->ip,
                                 entry_fields->mask, &entry_index,
                                 &free_index);
    if (rv == BCM_E_NOT_FOUND && entry_index == -1) {
        rv = BCM_E_FULL;
    }

    if (BCM_SUCCESS(rv)) {   /* found an exact match */
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *, vstab,
                                              entry_index);

        /* retrieve old vlan action profile pointer */
        old_profile_idx = soc_VLAN_SUBNETm_field32_get
            (unit, vstabp, TAG_ACTION_PROFILE_PTRf);   
        _trx_vlan_subnet_entry_set(unit, vstabp, entry_fields);
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, entry_index, vstabp);
        if (BCM_SUCCESS(rv)) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                           old_profile_idx);
        }
    } else if (rv == BCM_E_NOT_FOUND) {
        /* shift down entries starting at the insert point to create hole */
        for (i = free_index - 1; i >= entry_index; i--) {
            vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                                  vlan_subnet_entry_t *, vstab,
                                                  i);
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i + 1, vstabp);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (i < entry_index) {
            /* insert new entry at the created hole */
            vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                                  vlan_subnet_entry_t *, vstab,
                                                  entry_index);
            sal_memset(vstabp, 0,
                       WORDS2BYTES(soc_mem_entry_words(unit, VLAN_SUBNETm)));
            _trx_vlan_subnet_entry_set(unit, vstabp, entry_fields);
            rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ALL, entry_index, vstabp);
        }
    }
    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}



/*
 * Function:
 *      _trx_vlan_subnet_entry_get
 * Purpose:
 *      Get vlan settings for given subnet
 * Parameters:
 *      unit -          (IN) BCM Device unit
 *      subnet_fields - (IN/OUT) Structure contains all entry fields
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_trx_vlan_subnet_entry_get(int unit,
                           _bcm_trx_vlan_subnet_entry_t *entry_fields)
{
    vlan_subnet_entry_t         *vstab, *vstabp;
    int                         entry_index, rv, nent;

    if (NULL == entry_fields) {
        return (BCM_E_PARAM);
    }
    soc_mem_lock(unit, VLAN_SUBNETm);

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }
    
    rv = _trx_vlan_subnet_lookup(unit, vstab, nent, entry_fields->ip,
                                 entry_fields->mask, &entry_index, NULL);
    if (BCM_SUCCESS(rv)) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm, 
                                              vlan_subnet_entry_t *, vstab,
                                              entry_index);
        _trx_vlan_subnet_entry_parse(unit, vstabp, entry_fields);
    }

    soc_cm_sfree(unit, vstab);
    soc_mem_unlock(unit, VLAN_SUBNETm);
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _trx_vlan_subnet_entry_delete
 * Purpose:
 *      Delete vlan settings for given subnet
 * Parameters:
 *      unit -        (IN) BCM Device unit
 *      ip -          (IN) IP address
 *      mask -        (IN) IP mask
 * Returns:
 *      BCM_E_XXX
 */
static int
_trx_vlan_subnet_entry_delete(int unit, bcm_ip6_t ip, bcm_ip6_t mask)
{
    int                         rv, nent, i;
    vlan_subnet_entry_t         *vstab, *vstabp;
    uint32                      old_profile_idx;
    int                         entry_index, free_index;

    soc_mem_lock(unit, VLAN_SUBNETm);
    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return rv;
    }

    rv = _trx_vlan_subnet_lookup(unit, vstab, nent, ip, mask, &entry_index,
                                 &free_index);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vstab);
        soc_mem_unlock(unit, VLAN_SUBNETm);
        return (rv);
    }

    /* Get the old vlan action profile pointer */
    vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                          vlan_subnet_entry_t *, vstab,
                                          entry_index);
    old_profile_idx = soc_mem_field32_get(unit, VLAN_SUBNETm, vstabp,
                                          TAG_ACTION_PROFILE_PTRf);

    for (i = entry_index; i < free_index - 1; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *, vstab,
                                              (i + 1));
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vstabp);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    if (i >= free_index - 1) {
        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, free_index - 1,
                                soc_mem_entry_null(unit, VLAN_SUBNETm));
        if (BCM_SUCCESS(rv)) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                           old_profile_idx);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_add
 * Purpose:
 *      Add a subnet lookup to select vlan and priority for
 *      untagged packets
 * Parameters:
 *      unit    -   device number
 *      vlan_ip -   structure specifying IP address and other info
 *      action  -   structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_add(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
    int rv;
    _bcm_trx_vlan_subnet_entry_t entry_fields;

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_ip_verify(unit, vlan_ip));

    sal_memset(&entry_fields, 0, sizeof(_bcm_trx_vlan_subnet_entry_t));
    BCM_IF_ERROR_RETURN(_trx_vlan_ip_addr_mask_get(vlan_ip, entry_fields.ip,
                                                   entry_fields.mask));

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_add
                        (unit, action, (uint32 *)&entry_fields.profile_idx));

    entry_fields.ovid = action->new_outer_vlan;
    entry_fields.ivid = action->new_inner_vlan;
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        entry_fields.opri = action->priority;
        entry_fields.ocfi = action->new_outer_cfi;
        entry_fields.ipri = action->new_inner_pkt_prio;
        entry_fields.icfi = action->new_inner_cfi;
    } else {
        entry_fields.opri = action->priority;
    }

    rv = _trx_vlan_subnet_entry_add(unit, &entry_fields);
    if (BCM_FAILURE(rv)) {
        /* Add failed, back out the new profile entry */
        (void)_bcm_trx_vlan_action_profile_entry_delete
            (unit, entry_fields.profile_idx);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_get
 * Purpose:
 *      Helper funtion for the API, implementation for TRX
 * Parameters:
 *      unit    -   (IN) device number
 *      vlan_ip -   (IN) structure specifying IP address and other info
 *      action  -   (OUT) structure VLAN tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_get(int unit, bcm_vlan_ip_t *vlan_ip,
                           bcm_vlan_action_set_t *action)
{
    _bcm_trx_vlan_subnet_entry_t    entry_fields;

    /* Parameter check */
    if (NULL == action) {
        return (BCM_E_PARAM);
    }
    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_ip_verify(unit, vlan_ip));

    sal_memset(&entry_fields, 0, sizeof(_bcm_trx_vlan_subnet_entry_t));
    BCM_IF_ERROR_RETURN(
        _trx_vlan_ip_addr_mask_get(vlan_ip, entry_fields.ip, entry_fields.mask));  
    
    BCM_IF_ERROR_RETURN(
        _trx_vlan_subnet_entry_get(unit, &entry_fields));

    action->new_outer_vlan = entry_fields.ovid; 
    action->new_inner_vlan = entry_fields.ivid;
    action->priority = entry_fields.opri;
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->new_outer_cfi = entry_fields.ocfi;
        action->new_inner_pkt_prio = entry_fields.ipri;
        action->new_inner_cfi = entry_fields.icfi;
    }

    _bcm_trx_vlan_action_profile_entry_get(unit, action, 
                                           entry_fields.profile_idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_ip_action_traverse
 * Purpose:
 *      Helper funtion for the API, implementation for TRX
 * Parameters:
 *      unit    -   (IN) device number
 *      cb      -   (IN) user specified call back function
 *      user_data - (IN) pointer to user_data
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_action_traverse(int unit, 
                                 bcm_vlan_ip_action_traverse_cb cb,
                                 void * user_data)
{
    vlan_subnet_entry_t         *vstab, *vstabp;
    int                         nent, rv, i;
    bcm_vlan_action_set_t       action;
    bcm_vlan_ip_t               vlan_ip;
    _bcm_trx_vlan_subnet_entry_t entry_fields;

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }
    sal_memset(&entry_fields, 0, sizeof(_bcm_trx_vlan_subnet_entry_t));

    
    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        return rv;
    }
    
    for(i = 0; i < nent; i++) {
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        sal_memset(&vlan_ip, 0, sizeof(bcm_vlan_ip_t));
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                                              vlan_subnet_entry_t *, 
                                              vstab, i);
        if (0 == soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)){
            continue;
        }
        _trx_vlan_subnet_entry_parse(unit, vstabp, &entry_fields);

        if (*(uint32*)entry_fields.ip == 0xffff0000) {
            /* IPv4 case */
            bcm_ip6_t   ip6tmp;

            vlan_ip.ip4 = (entry_fields.ip[4] << 24) |
                (entry_fields.ip[5] << 16) | (entry_fields.ip[6] << 8) |
                entry_fields.ip[7];
            bcm_ip6_mask_create(ip6tmp, entry_fields.prefix);
            vlan_ip.mask = (ip6tmp[4] << 24) | (ip6tmp[5] << 16) |
                (ip6tmp[6] << 8) | ip6tmp[7];
        } else {
            /* IPv6 case */
            sal_memcpy(vlan_ip.ip6, entry_fields.ip, sizeof(bcm_ip6_t));
            vlan_ip.prefix = entry_fields.prefix;
            vlan_ip.flags |= BCM_VLAN_SUBNET_IP6;
        }

        vlan_ip.prefix = entry_fields.prefix;
        vlan_ip.vid = entry_fields.ovid;
        action.new_outer_vlan = entry_fields.ovid;
        action.new_inner_vlan = entry_fields.ivid;
        action.priority = entry_fields.opri;
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.new_outer_cfi = entry_fields.ocfi;
            action.new_inner_pkt_prio = entry_fields.ipri;
            action.new_inner_cfi = entry_fields.icfi;
        }
         
         /* Read action profile data. */
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, 
                                               entry_fields.profile_idx);
        /* Call traverse callback with the data. */
        rv = cb(unit, &vlan_ip, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_cm_sfree(unit, vstab);
            return rv; 
        }
    }

    soc_cm_sfree(unit, vstab);
    return rv;

}

/*
 * Function:
 *      bcm_vlan_ip_delete
 * Purpose:
 *      Delete a subnet lookup entry.
 * Parameters:
 *      unit -          device number
 *      vlan_ip -       structure specifying IP address and other info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_delete(int unit, bcm_vlan_ip_t *vlan_ip)
{
    bcm_ip6_t   ip6addr, ip6mask;

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_ip_verify(unit, vlan_ip));

    BCM_IF_ERROR_RETURN(
        _trx_vlan_ip_addr_mask_get(vlan_ip, ip6addr, ip6mask));
    return _trx_vlan_subnet_entry_delete(unit, ip6addr, ip6mask); 
}

/*
 * Function:
 *      bcm_vlan_ip_delete_all
 * Purpose:
 *      Delete all subnet lookup entries.
 * Parameters:
 *      unit -          device number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_ip_delete_all(int unit)
{
    int i, nent,  rv;
    uint32 old_profile_idx;
    vlan_subnet_entry_t *vstab, *vsnull,  *vstabp;

    rv = _tr_vlan_subnet_mem_read(unit, &vstab, &nent);
    if (BCM_FAILURE(rv)) {
        if (NULL != vstab) {
            soc_cm_sfree(unit, vstab);
        }
        return rv;
    }

    vsnull = soc_mem_entry_null(unit, VLAN_SUBNETm);

    soc_mem_lock(unit, VLAN_SUBNETm);
    for(i = 0; i < nent; i++) {
        vstabp = soc_mem_table_idx_to_pointer(unit, VLAN_SUBNETm,
                        vlan_subnet_entry_t *, vstab, i);

        if (!soc_VLAN_SUBNETm_field32_get(unit, vstabp, VALIDf)) {
            continue;
        }
        old_profile_idx = soc_VLAN_SUBNETm_field32_get(unit, vstabp,
                                                       TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_SUBNETm(unit, MEM_BLOCK_ANY, i, vsnull);

        /* Increment the vlan action profile refcount for the cleared entry */
        _bcm_trx_vlan_action_profile_entry_increment(unit, ing_action_profile_def[unit]);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }

    soc_mem_unlock(unit, VLAN_SUBNETm);
    soc_cm_sfree(unit, vstab);
    return rv;
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int 
_bcm_tr3_vlan_mac_action_add(int                   unit,
                                 bcm_mac_t              mac,
                                 bcm_vlan_action_set_t *action) 
{
    int rv;
    uint32 profile_idx;
    vlan_xlate_entry_t vxent;
    
    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority == -1) {
            return BCM_E_PARAM;  /* no default priority action to take*/
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

    sal_memset(&vxent, 0, sizeof(vxent));
    soc_mem_mac_addr_set(unit, VLAN_XLATEm, (uint32 *)(&vxent),
                       VLAN_MAC__MAC_ADDRf, mac);

    soc_VLAN_XLATEm_field32_set(unit, &vxent, KEY_TYPEf, 
             TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OVIDf, 
                                action->new_outer_vlan);
    soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__IVIDf, 
                                action->new_inner_vlan);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OPRIf, 
                                    action->priority);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OCFIf, 
                                    action->new_outer_cfi);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__IPRIf,
                                    action->new_inner_pkt_prio);
        soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__ICFIf, 
                                    action->new_inner_cfi);
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_VLAN_XLATEm_field32_set(unit, &vxent, VLAN_MAC__OPRIf, 
                                        action->priority);
        }
    }
    soc_VLAN_XLATEm_field32_set(unit, &vxent, VALIDf, 1);
    soc_VLAN_XLATEm_field32_set(unit, &vxent, 
                    VLAN_MAC__TAG_ACTION_PROFILE_PTRf, profile_idx);

    rv = soc_mem_insert_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL, 
                                      &vxent, &vxent);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &vxent,
                                      VLAN_MAC__TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      bcm_vlan_mac_action_add
 * Description   :
 *      Add association from MAC address to VLAN.
 *      If the entry already exists, update the action.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (IN) Action for outer and inner tag
 * Note:
 *   Program VLAN_XLATEm.
 */
int _bcm_trx_vlan_mac_action_add(int                   unit,
                                 bcm_mac_t              mac,
                                 bcm_vlan_action_set_t *action) 
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vlan_mac_action_add(unit,mac,action));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
    int rv;
    uint32 profile_idx;
    vlan_mac_entry_t vment, vment_old;

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority == -1) {
            return BCM_E_PARAM;  /* no default priority action to take*/
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

    sal_memset(&vment, 0, sizeof(vment));
    sal_memset(&vment_old, 0, sizeof(vment_old));
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_VLAN_MACm_field32_set(unit, &vment, OVIDf, action->new_outer_vlan);
    soc_VLAN_MACm_field32_set(unit, &vment, IVIDf, action->new_inner_vlan);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_VLAN_MACm_field32_set(unit, &vment, OPRIf, action->priority);
        soc_VLAN_MACm_field32_set(unit, &vment, OCFIf, action->new_outer_cfi);
        soc_VLAN_MACm_field32_set(unit, &vment, IPRIf,
                                  action->new_inner_pkt_prio);
        soc_VLAN_MACm_field32_set(unit, &vment, ICFIf, action->new_inner_cfi);
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_VLAN_MACm_field32_set(unit, &vment, PRIf, action->priority);
        }
    }
    soc_VLAN_MACm_field32_set(unit, &vment, TAG_ACTION_PROFILE_PTRf, 
                              profile_idx);
    if (SOC_MEM_FIELD_VALID(unit, VLAN_MACm, VLAN_ACTION_VALIDf)) {
        soc_VLAN_MACm_field32_set(unit, &vment, VLAN_ACTION_VALIDf, 1);
    }
    soc_VLAN_MACm_field32_set(unit, &vment, VALIDf, 1);

    rv = soc_mem_insert_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL, &vment,
                                   &vment_old);

    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        profile_idx = soc_VLAN_MACm_field32_get(unit, &vment_old,
                                                TAG_ACTION_PROFILE_PTRf);
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    return rv;
    }
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int 
_bcm_tr3_vlan_mac_action_get(int unit, bcm_mac_t  mac,
                             bcm_vlan_action_set_t *action) 
{
    vlan_xlate_entry_t vxent;             /* Lookup key hw buffer.      */
    vlan_xlate_entry_t res_vxent;         /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int rv;                             /* Operation return status.   */
    int idx = 0;                        /* Lookup result entry index. */

    /* Input parameters check. */
    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    /* Reset lookup key and result destination buffer. */
    sal_memset(&vxent, 0, sizeof(vlan_xlate_entry_t));
    sal_memset(&res_vxent, 0, sizeof(vlan_xlate_entry_t));

    /* Initialize lookup key. */
    soc_mem_mac_addr_set(unit, VLAN_XLATEm, (uint32 *)(&vxent),
                       VLAN_MAC__MAC_ADDRf, mac);

    soc_VLAN_XLATEm_field32_set(unit, &vxent, KEY_TYPEf,
                              TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC);

    /* Perform VLAN_MAC table search by mac address. */
    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_search(unit, VLAN_XLATEm, MEM_BLOCK_ALL, &idx, 
                        &vxent, &res_vxent, 0);
    soc_mem_unlock(unit, VLAN_XLATEm);
    BCM_IF_ERROR_RETURN(rv);

    action->new_outer_vlan =
        soc_VLAN_XLATEm_field32_get(unit, &res_vxent, VLAN_MAC__OVIDf);
    action->new_inner_vlan =
        soc_VLAN_XLATEm_field32_get(unit, &res_vxent, VLAN_MAC__IVIDf);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority = soc_VLAN_XLATEm_field32_get(unit, &res_vxent, 
                                    VLAN_MAC__OPRIf);
        action->new_outer_cfi =
            soc_VLAN_XLATEm_field32_get(unit, &res_vxent, VLAN_MAC__OCFIf);
        action->new_inner_pkt_prio =
            soc_VLAN_XLATEm_field32_get(unit, &res_vxent, VLAN_MAC__IPRIf);
        action->new_inner_cfi =
            soc_VLAN_XLATEm_field32_get(unit, &res_vxent, VLAN_MAC__ICFIf);
    } else {
        action->priority = soc_VLAN_XLATEm_field32_get(unit, &res_vxent, 
                                   VLAN_MAC__OPRIf);
    }

    /* Read action profile data. */
    profile_idx = soc_VLAN_XLATEm_field32_get(unit, &res_vxent, 
                                   VLAN_MAC__TAG_ACTION_PROFILE_PTRf);
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    return (BCM_E_NONE);
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      _bcm_trx_vlan_mac_action_get
 * Description   :
 *      Get association from MAC address to VLAN tag actions.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      action    (OUT) Action for outer and inner tag
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_mac_action_get(int unit, bcm_mac_t  mac,
                             bcm_vlan_action_set_t *action) 
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vlan_mac_action_get(unit,mac,action));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
    vlan_mac_entry_t vment;             /* Lookup key hw buffer.      */
    vlan_mac_entry_t res_vment;         /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int rv;                             /* Operation return status.   */
    int idx = 0;                        /* Lookup result entry index. */

    /* Input parameters check. */
    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    /* Reset lookup key and result destination buffer. */
    sal_memset(&vment, 0, sizeof(vlan_mac_entry_t));
    sal_memset(&res_vment, 0, sizeof(vlan_mac_entry_t));

    /* Initialize lookup key. */
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);

    /* Perform VLAN_MAC table search by mac address. */
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_search(unit, VLAN_MACm, MEM_BLOCK_ALL, &idx, 
                        &vment, &res_vment, 0);
    soc_mem_unlock(unit, VLAN_MACm);
    BCM_IF_ERROR_RETURN(rv);

    action->new_outer_vlan =
        soc_VLAN_MACm_field32_get(unit, &res_vment, OVIDf);
    action->new_inner_vlan =
        soc_VLAN_MACm_field32_get(unit, &res_vment, IVIDf);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority = soc_VLAN_MACm_field32_get(unit, &res_vment, OPRIf);
        action->new_outer_cfi =
            soc_VLAN_MACm_field32_get(unit, &res_vment, OCFIf);
        action->new_inner_pkt_prio =
            soc_VLAN_MACm_field32_get(unit, &res_vment, IPRIf);
        action->new_inner_cfi =
            soc_VLAN_MACm_field32_get(unit, &res_vment, ICFIf);
    } else {
        action->priority = soc_VLAN_MACm_field32_get(unit, &res_vment, PRIf);
    }

    /* Read action profile data. */
    profile_idx =
        soc_VLAN_MACm_field32_get(unit, &res_vment, TAG_ACTION_PROFILE_PTRf);
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    return (BCM_E_NONE);
    }
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int
_bcm_tr3_vlan_mac_delete(int unit, bcm_mac_t mac)
{
    vlan_xlate_entry_t  vxent;
    int rv;
    uint32 profile_idx;

    sal_memset(&vxent, 0, sizeof(vxent));
    soc_mem_mac_addr_set(unit, VLAN_XLATEm, (uint32 *)(&vxent),
                       VLAN_MAC__MAC_ADDRf, mac);
    soc_VLAN_XLATEm_field32_set(unit, &vxent, KEY_TYPEf,
                              TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_delete_return_old(unit, VLAN_XLATEm, MEM_BLOCK_ALL,
                                   &vxent, &vxent);
    soc_mem_unlock(unit, VLAN_XLATEm);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) && soc_VLAN_XLATEm_field32_get(unit, 
               &vxent, VALIDf)) {
        profile_idx = soc_VLAN_XLATEm_field32_get(unit, &vxent,
                                                VLAN_MAC__TAG_ACTION_PROFILE_PTRf);
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      bcm_vlan_mac_delete
 * Purpose:
 *      Delete a vlan mac lookup entry.
 * Parameters:
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_mac_delete(int unit, bcm_mac_t mac) 
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vlan_mac_delete(unit,mac));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
    vlan_mac_entry_t  vment;
    int rv; 
    uint32 profile_idx;

    sal_memset(&vment, 0, sizeof(vment));
    soc_VLAN_MACm_mac_addr_set(unit, &vment, MAC_ADDRf, mac);
    soc_VLAN_MACm_field32_set(unit, &vment, KEY_TYPEf,
                              TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC);
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_delete_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL,
                                   &vment, &vment);
    soc_mem_unlock(unit, VLAN_MACm);
    if (rv == SOC_E_NOT_FOUND) {
        rv = SOC_E_NONE;
    } else if ((rv == SOC_E_NONE) && soc_VLAN_MACm_field32_get(unit, &vment, VALIDf)) {
        profile_idx = soc_VLAN_MACm_field32_get(unit, &vment,
                                                TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
    return rv;
    }
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int
_bcm_tr3_vlan_mac_delete_all(int unit) 
{
    int i, imin, imax, nent, vmbytes, rv;
    uint32 old_profile_idx;
    vlan_xlate_entry_t *vxtab, *vmnull, *vxtabp;
    
    imin = soc_mem_index_min(unit, VLAN_XLATEm);
    imax = soc_mem_index_max(unit, VLAN_XLATEm);
    nent = soc_mem_index_count(unit, VLAN_XLATEm);
    vmbytes = soc_mem_entry_words(unit, VLAN_XLATEm);
    vmbytes = WORDS2BYTES(vmbytes);

    vxtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_xlate");

    if (vxtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vmnull = soc_mem_entry_null(unit, VLAN_XLATEm);

    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_read_range(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                            imin, imax, vxtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_XLATEm);
        soc_cm_sfree(unit, vxtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        vxtabp = soc_mem_table_idx_to_pointer(unit, VLAN_XLATEm,
                        vlan_xlate_entry_t *, vxtab, i);

        if (!soc_VLAN_XLATEm_field32_get(unit, vxtabp, VALIDf) ||
            (soc_VLAN_XLATEm_field32_get(unit, vxtabp, KEY_TYPEf) !=
             TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }
        old_profile_idx = soc_VLAN_XLATEm_field32_get(unit, vxtabp,
                                                      VLAN_MAC__TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_XLATEm(unit, MEM_BLOCK_ANY, i, vmnull);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }
    
    soc_mem_unlock(unit, VLAN_XLATEm);
    soc_cm_sfree(unit, vxtab);
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      bcm_vlan_mac_delete_all
 * Purpose:
 *      Delete all vlan mac lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_mac_delete_all(int unit) 
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vlan_mac_delete_all(unit));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_td3_vlan_mac_delete_all(unit));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIDENT3_SUPPORT */

    {
    int i, imin, imax, nent, vmbytes, rv;
    uint32 old_profile_idx;
    vlan_mac_entry_t *vmtab, *vmnull, *vmtabp;
    
    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vmnull = soc_mem_entry_null(unit, VLAN_MACm);

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
                            imin, imax, vmtab);
    soc_mem_unlock(unit, VLAN_MACm);

    if (rv < 0) {
        soc_cm_sfree(unit, vmtab);
        return rv; 
    }

    soc_mem_lock(unit, VLAN_MACm);
    for(i = 0; i < nent; i++) {
        vmtabp = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                        vlan_mac_entry_t *, vmtab, i);

        if (!soc_VLAN_MACm_field32_get(unit, vmtabp, VALIDf) ||
            (soc_VLAN_MACm_field32_get(unit, vmtabp, KEY_TYPEf) !=
             TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC)) {
            continue;
        }
        old_profile_idx = soc_VLAN_MACm_field32_get(unit, vmtabp,
                                                    TAG_ACTION_PROFILE_PTRf);

        rv = WRITE_VLAN_MACm(unit, MEM_BLOCK_ANY, i, vmnull);

        if (rv >= 0) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }
    }
    
    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);
    return rv;
    }
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int 
_bcm_tr3_vlan_mac_action_traverse(int unit, 
                                  bcm_vlan_mac_action_traverse_cb cb, 
                                  void *user_data)
{
    int idx, imin, imax, nent, vmbytes, rv;
    uint32 profile_idx;
    bcm_mac_t  mac;
    bcm_vlan_action_set_t action;
    vlan_xlate_entry_t * vxtab, *vxtabp;
    
    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    imin = soc_mem_index_min(unit, VLAN_XLATEm);
    imax = soc_mem_index_max(unit, VLAN_XLATEm);
    nent = soc_mem_index_count(unit, VLAN_XLATEm);
    vmbytes = soc_mem_entry_words(unit, VLAN_XLATEm);
    vmbytes = WORDS2BYTES(vmbytes);
    vxtab = soc_cm_salloc(unit, nent * sizeof(*vxtab), "vlan_xlate");

    if (vxtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    soc_mem_lock(unit, VLAN_XLATEm);
    rv = soc_mem_read_range(unit, VLAN_XLATEm, MEM_BLOCK_ANY,
                            imin, imax, vxtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_XLATEm);
        soc_cm_sfree(unit, vxtab);
        return rv; 
    }
    
    for(idx = 0; idx < nent; idx++) {
        sal_memset(mac, 0, sizeof(bcm_mac_t));
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        vxtabp = soc_mem_table_idx_to_pointer(unit, VLAN_XLATEm,
                                              vlan_xlate_entry_t *, 
                                              vxtab, idx);

        if ((0 == soc_VLAN_XLATEm_field32_get(unit, vxtabp, VALIDf)) ||
            (TR3_VLXLT_HASH_KEY_TYPE_VLAN_MAC !=
             soc_VLAN_XLATEm_field32_get(unit, vxtabp, KEY_TYPEf))) {
            continue;
        }

        /* Get entry mac address. */
        soc_mem_mac_addr_get(unit, VLAN_XLATEm, (uint32 *)vxtabp, 
                     VLAN_MAC__MAC_ADDRf, mac);

        action.new_outer_vlan =
            soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__OVIDf);
        action.new_inner_vlan =
            soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.priority = soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__OPRIf);
            action.new_outer_cfi =
                soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__OCFIf);
            action.new_inner_pkt_prio =
                soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__IPRIf);
            action.new_inner_cfi =
                soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__ICFIf);
        } else {
            action.priority =
                soc_VLAN_XLATEm_field32_get(unit, vxtabp, VLAN_MAC__OPRIf);
        }

        /* Read action profile data. */
        profile_idx =
            soc_VLAN_XLATEm_field32_get(unit, vxtabp, 
                                      VLAN_MAC__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);

        /* Call traverse callback with the data. */
        rv = cb(unit, mac, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_XLATEm);
            soc_cm_sfree(unit, vxtab);
            return rv; 
        }
    }
    
    soc_mem_unlock(unit, VLAN_XLATEm);
    soc_cm_sfree(unit, vxtab);
    return rv;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      _bcm_trx_vlan_mac_action_traverse
 * Description   :
 *      Traverse over vlan mac actions, which are used for VLAN
 *      tag/s assignment to untagged packets.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) Call back function
 *      user_data       (IN) User provided data to pass to a call back
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_mac_action_traverse(int unit, 
                                  bcm_vlan_mac_action_traverse_cb cb, 
                                  void *user_data)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vlan_mac_action_traverse(unit,cb,user_data));
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
    int idx, imin, imax, nent, vmbytes, rv;
    uint32 profile_idx;
    bcm_mac_t  mac;
    bcm_vlan_action_set_t action;
    vlan_mac_entry_t * vmtab, *vmtabp;
    
    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);
    vmtab = soc_cm_salloc(unit, nent * sizeof(*vmtab), "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
                            imin, imax, vmtab);
    if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, VLAN_MACm);
        soc_cm_sfree(unit, vmtab);
        return rv; 
    }
    
    for(idx = 0; idx < nent; idx++) {
        sal_memset(mac, 0, sizeof(bcm_mac_t));
        sal_memset(&action, 0, sizeof(bcm_vlan_action_set_t));
        vmtabp = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                                              vlan_mac_entry_t *, 
                                              vmtab, idx);

        if ((0 == soc_VLAN_MACm_field32_get(unit, vmtabp, VALIDf)) ||
            (TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC !=
             soc_VLAN_MACm_field32_get(unit, vmtabp, KEY_TYPEf))) {
            continue;
        }

        /* Get entry mac address. */
        soc_VLAN_MACm_mac_addr_get(unit, vmtabp, MAC_ADDRf, mac);

        action.new_outer_vlan =
            soc_VLAN_MACm_field32_get(unit, vmtabp, OVIDf);
        action.new_inner_vlan =
            soc_VLAN_MACm_field32_get(unit, vmtabp, IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action.priority = soc_VLAN_MACm_field32_get(unit, vmtabp, OPRIf);
            action.new_outer_cfi =
                soc_VLAN_MACm_field32_get(unit, vmtabp, OCFIf);
            action.new_inner_pkt_prio =
                soc_VLAN_MACm_field32_get(unit, vmtabp, IPRIf);
            action.new_inner_cfi =
                soc_VLAN_MACm_field32_get(unit, vmtabp, ICFIf);
        } else {
            action.priority =
                soc_VLAN_MACm_field32_get(unit, vmtabp, PRIf);
        }

        /* Read action profile data. */
        profile_idx =
            soc_VLAN_MACm_field32_get(unit, vmtabp, TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);

        /* Call traverse callback with the data. */
        rv = cb(unit, mac, &action, user_data);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, VLAN_MACm);
            soc_cm_sfree(unit, vmtab);
            return rv; 
        }
    }
    
    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);
    return rv;
    }
}

/*
 * Function:
 *      _bcm_trx_vif_vlan_translate_entry_parse
 * Purpose:
 *      Parses vlan translate entry by given key_type, for inner and outer vlans
 * Parameters: 
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) vlan translate entry to parse
 *      action          (OUT) Action to fill
 * Returns:
 *      BCM_E_XXX
 */     
STATIC int
_bcm_trx_vif_vlan_translate_entry_parse(int unit, soc_mem_t mem, uint32 *vent,
                                   bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_niv)) {
        action->new_outer_vlan = soc_mem_field32_get(unit, VLAN_XLATEm,
                         vent, VIF__NEW_OVIDf);
        action->new_inner_vlan = soc_mem_field32_get (unit, VLAN_XLATEm,
                         vent, VIF__NEW_IVIDf);
    
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action->priority = soc_mem_field32_get(unit, VLAN_XLATEm,
                                vent, VIF__NEW_OPRIf);
            action->new_outer_cfi = soc_mem_field32_get(unit, VLAN_XLATEm,
                     vent, VIF__NEW_OCFIf);
            action->new_inner_pkt_prio = soc_mem_field32_get(unit, VLAN_XLATEm,
                     vent, VIF__NEW_IPRIf);
            action->new_inner_cfi = soc_mem_field32_get(unit, VLAN_XLATEm,
                     vent, VIF__NEW_ICFIf);
        } else {
            action->priority = soc_mem_field32_get(unit, VLAN_XLATEm,
                           vent, VIF__PRIf);
        }

        if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm, VIF__L3_IIF_VALIDf) &&
            soc_mem_field32_get(unit, VLAN_XLATEm, vent, VIF__L3_IIF_VALIDf)) {
            action->ingress_if = soc_mem_field32_get(unit, VLAN_XLATEm,
                                                     vent, VIF__L3_IIFf);
        }
        return BCM_E_NONE;
    }
#endif   /* defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_entry_parse
 * Purpose:
 *      Parses vlan translate entry by given key_type, for inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) vlan translate entry to parse
 *      action          (OUT) Action to fill
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_trx_vlan_translate_entry_parse(int unit, soc_mem_t mem, uint32 *vent,
                                   bcm_vlan_action_set_t *action)
{
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int offset = 0, index = 0;
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_APACHE_SUPPORT || BCM_GREYHOUND2_SUPPORT */
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    int hw_index = -1;
#endif
    if ((NULL == vent) || (NULL == action) || (INVALIDm == mem)) {
        return BCM_E_PARAM;
    }

    action->new_outer_vlan = soc_mem_field32_get(unit, mem, vent, NEW_OVIDf);
    action->new_inner_vlan = soc_mem_field32_get (unit, mem, vent, NEW_IVIDf);

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (mem == EGR_VLAN_XLATE_2_DOUBLEm){ 
        action->flags |= BCM_VLAN_ACTION_SET_EGRESS_TUNNEL_OVID;
    }
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority = soc_mem_field32_get(unit, mem, vent, NEW_OPRIf);
        action->new_outer_cfi =
            soc_mem_field32_get(unit, mem, vent, NEW_OCFIf);
        action->new_inner_pkt_prio =
            soc_mem_field32_get(unit, mem, vent, NEW_IPRIf);
        action->new_inner_cfi =
            soc_mem_field32_get(unit, mem, vent, NEW_ICFIf);
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)
            && ((mem == EGR_VLAN_XLATEm) || ((mem == EGR_VLAN_XLATE_1_DOUBLEm)))) {
            if (soc_mem_field32_get(unit, mem, vent, IPRI_CFI_SELf)) {
                hw_index =
                    soc_mem_field32_get(unit, mem, vent, IPRI_MAPPING_PTRf);
                _bcm_tr2_qos_idx2id(unit, hw_index, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                        &action->inner_qos_map_id);
                action->flags |= BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID;
            }
            if (soc_mem_field32_get(unit, mem, vent, OPRI_CFI_SELf)) {
                hw_index =
                    soc_mem_field32_get(unit, mem, vent, OPRI_MAPPING_PTRf);
                _bcm_tr2_qos_idx2id(unit, hw_index, _BCM_QOS_MAP_TYPE_EGR_MPLS_MAPS,
                        &action->outer_qos_map_id);
                action->flags |= BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID;
            }
        }
#endif
    } else {
        action->priority = soc_mem_field32_get(unit, mem, vent, PRIf);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, DISABLE_VLAN_CHECKSf) &&
        soc_mem_field32_get(unit, mem, vent, DISABLE_VLAN_CHECKSf)) {
        action->flags |= BCM_VLAN_ACTION_SET_VLAN_CHECKS_DISABLE;
    }

#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if (soc_mem_field32_get(unit, mem, vent, SR_ENABLEf)) {
            action->flags |= BCM_VLAN_ACTION_SET_SR_ENABLE;
            if (soc_mem_field32_get(unit, mem, vent, SR_LAN_IDf)) {
                action->flags |= BCM_VLAN_ACTION_SET_SR_LAN_ID;
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT*/

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_taf) &&
        SOC_MEM_FIELD_VALID(unit, mem, TAF_GATE_ID_PROFILEf)) {
        uint32 value =
            soc_mem_field32_get(unit, mem, vent, TAF_GATE_ID_PROFILEf);
        if (0 != value) {
            bcm_tsn_pri_map_t sw_index;

            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_pri_map_map_id_get(unit, bcmTsnPriMapTypeTafGate,
                                                value, &sw_index));
            action->taf_gate_primap = sw_index;
        } else {
            action->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
        }
    } else
#endif /* BCM_TSN_SUPPORT */
    {
        action->taf_gate_primap = BCM_TSN_PRI_MAP_INVALID;
    }

    if (SOC_IS_TR_VL(unit) && (!(SOC_IS_HURRICANE(unit) || 
        SOC_IS_HURRICANE2(unit)))) {
        if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
            if (soc_mem_field32_get(unit, mem, vent, MPLS_ACTIONf) == 0x2) {
                action->ingress_if = 
                   soc_mem_field32_get(unit, mem, vent, L3_IIFf);
            }
#if defined(BCM_TRIDENT2_SUPPORT)
            else if (soc_mem_field32_get(unit, mem, vent, MPLS_ACTIONf) == 0x3) {
                if (soc_feature(unit, soc_feature_vlan_xlate_iif_with_dummy_vp) &&
                    soc_property_get(unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0) &&
                    soc_mem_field32_get(unit, mem, vent, SOURCE_VPf) == _BCM_VLAN_XLATE_DUMMY_PORT) {
                    action->flags |= BCM_VLAN_ACTION_SET_DUMMY_VP;
                    action->ingress_if =
                       soc_mem_field32_get(unit, mem, vent, L3_IIFf);
                }

            }
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            else if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf)) {
                action->class_id = soc_mem_field32_get(unit, mem, vent, CLASS_IDf);
            }
#endif
        } 
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        else if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_ID_VALIDf)) {
            if (soc_mem_field32_get(unit, mem, vent, CLASS_ID_VALIDf)) {
                action->class_id = soc_mem_field32_get(unit, mem, vent, CLASS_IDf);
            }
        }
#endif
    }
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_APACHE_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter)) {
        if (SOC_IS_KATANAX(unit) || SOC_IS_GREYHOUND2(unit) ||
            (SOC_IS_APACHE(unit) && !SOC_IS_MONTEREY(unit))) {

            if (SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_OFFSET_MODEf)) {
                offset = soc_mem_field32_get(unit, mem, vent,
                                                  SVC_METER_OFFSET_MODEf);
            }
            if (SOC_MEM_FIELD_VALID(unit, mem, SVC_METER_INDEXf)) {
                index = soc_mem_field32_get(unit, mem, vent, SVC_METER_INDEXf);
            }
        } else if (SOC_IS_TRIUMPH3(unit)) {
            if (SOC_MEM_FIELD_VALID(unit, mem,
                                         XLATE__SVC_METER_OFFSET_MODEf)) {
                offset = soc_mem_field32_get(unit, mem, vent,
                                         XLATE__SVC_METER_OFFSET_MODEf);
            }
            if (SOC_MEM_FIELD_VALID(unit, mem, XLATE__SVC_METER_INDEXf)) {
                index = soc_mem_field32_get(unit, mem, vent, 
                                                  XLATE__SVC_METER_INDEXf);
            }
        }
        _bcm_esw_get_policer_id_from_index_offset(unit, index, offset, 
                                                     &action->policer_id);
       }
#endif /* BCM_KATANA_SUPPORT || BCM_TRIUMPH3_SUPPORT ||
          BCM_APACHE_SUPPORT || BCM_GREYHOUND2_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_trx_vif_vlan_translate_entry_assemble
 * Purpose:
 *      Constructs vif vlan translate entry from given key_type, inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      vent            (IN/OUT) vlan translate entry to construct
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      inner_vlan      (IN) inner VLAN ID
 *      outer_vlan      (IN) outer VLAN ID
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vif_vlan_translate_entry_assemble(int unit, void *vent,
                                      bcm_gport_t niv_port_id,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t inner_vlan,
                                      bcm_vlan_t outer_vlan)
{
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_niv)) {
        int             tmp_id;
        bcm_module_t    mod_out;
        bcm_port_t      port_out;
        bcm_trunk_t     trunk_out;
        int key_type_value;
        bcm_niv_port_t niv_port;
        bcm_niv_egress_t niv_egress;
        int              rv                   = BCM_E_NONE;
        uint16           virtual_interface_id = 0;
        bcm_gport_t      gport;
        int              count;

        soc_mem_t mem = VLAN_XLATEm;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }

        if (!BCM_GPORT_IS_NIV_PORT(niv_port_id)) {
            return BCM_E_PORT;
        }
    
        switch (key_type) {
            case bcmVlanTranslateKeyPortOuterTag:
                key_type = VLXLT_HASH_KEY_TYPE_VIF_OTAG;
                soc_mem_field32_set(unit, mem, vent, VIF__OTAGf, outer_vlan);
                break;
    
            case bcmVlanTranslateKeyPortInnerTag:
                key_type = VLXLT_HASH_KEY_TYPE_VIF_ITAG;
                soc_mem_field32_set(unit, mem, vent, VIF__ITAGf, inner_vlan);
                break;
    
            case bcmVlanTranslateKeyPortOuter:
                key_type = VLXLT_HASH_KEY_TYPE_VIF_VLAN;
                soc_mem_field32_set(unit, mem, vent, VIF__VLANf, outer_vlan);
                break;
    
            case bcmVlanTranslateKeyPortInner:
                key_type = VLXLT_HASH_KEY_TYPE_VIF_CVLAN;
                soc_mem_field32_set(unit, mem, vent, VIF__CVLANf,inner_vlan);
                break;
    
            default:
                return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                   key_type, &key_type_value));
        soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
        if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
        }
    
        niv_port.niv_port_id = niv_port_id;
        BCM_IF_ERROR_RETURN(bcm_esw_niv_port_get(unit,&niv_port));
        if (niv_port.flags & BCM_NIV_PORT_MATCH_NONE) {
            bcm_niv_egress_t_init(&niv_egress);
            rv = bcm_trident_niv_egress_get(unit, niv_port.niv_port_id,
                                            1, &niv_egress, &count);
            if (BCM_FAILURE(rv)) {
                return BCM_E_PARAM;
            }

            if (niv_egress.flags & BCM_NIV_EGRESS_MULTICAST) {
                return BCM_E_PARAM;
            } else {
                virtual_interface_id = niv_egress.virtual_interface_id;
                gport                = niv_egress.port;
            }
        } else {
            virtual_interface_id = niv_port.virtual_interface_id;
            gport                = niv_port.port;
        }
    
        soc_mem_field32_set(unit, mem, vent, VIF__SRC_VIFf,
                                    virtual_interface_id);
    
        if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport, 
                      &mod_out, &port_out, &trunk_out, &tmp_id));
        if (BCM_GPORT_IS_TRUNK(gport)) {
            soc_mem_field32_set(unit, mem, vent, VIF__Tf, 1);
            soc_mem_field32_set(unit, mem, vent, VIF__TGIDf, trunk_out);
        } else {
            soc_mem_field32_set(unit, mem, vent, VIF__MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, VIF__PORT_NUMf, port_out);
        }
        return BCM_E_NONE;
    }
#endif   /* defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_trx_lltag_vlan_translate_entry_assemble
 * Purpose:
 *      Constructs lltag vlan translate entry from given key_type, lltag vlan and other vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      vent            (IN/OUT) vlan translate entry to construct
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      lltag_vlan      (IN) LLTAG VLAN ID
 *      other_vlan      (IN) other VLAN ID denpends on the key_type
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_lltag_vlan_translate_entry_assemble(int unit, void *vent,
                                      bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t lltag_vlan,
                                      bcm_vlan_t other_vlan)
{
#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_lltag)) {
        int             use_glp = 1;    /* By default incoming port used in lookup key */
        int             tmp_id;
        bcm_module_t    mod_out;
        bcm_port_t      port_out;
        bcm_trunk_t     trunk_out;
        int key_type_value;
        soc_mem_t       mem = VLAN_XLATEm;

        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }

        switch (key_type) {
            case bcmVlanTranslateKeyPortPonTunnel:
                use_glp = 1;
                VLAN_CHK_ID(unit, lltag_vlan);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                           VLXLT_HASH_KEY_TYPE_LLVID,
                           &key_type_value));
                soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
                if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
                }
                soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
                break;
            case bcmVlanTranslateKeyPortPonTunnelOuter:
                use_glp = 1;      
                VLAN_CHK_ID(unit, lltag_vlan);
                VLAN_CHK_ID(unit, other_vlan);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                           VLXLT_HASH_KEY_TYPE_LLVID_OVID,
                           &key_type_value));
                soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
                if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
                }
                soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
                soc_mem_field32_set(unit, mem, vent, LLTAG__OVIDf, other_vlan);
                break;
            case bcmVlanTranslateKeyPortPonTunnelInner:
                use_glp = 1;      
                VLAN_CHK_ID(unit, lltag_vlan);
                VLAN_CHK_ID(unit, other_vlan);
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                           VLXLT_HASK_KEY_TYPE_LLVID_IVID,
                           &key_type_value));
                soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
                if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
                }
                soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
                soc_mem_field32_set(unit, mem, vent, LLTAG__IVIDf, other_vlan);
                break;
            default:
                return BCM_E_PARAM;
        }

        if (use_glp) {
            if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                                        &trunk_out, &tmp_id));
            if (BCM_GPORT_IS_TRUNK(port)) {
                soc_mem_field32_set(unit, mem, vent, LLTAG__Tf, 1);
                soc_mem_field32_set(unit, mem, vent, LLTAG__TGIDf, trunk_out);
            } else {
                soc_mem_field32_set(unit, mem, vent, LLTAG__MODULE_IDf, mod_out);
                soc_mem_field32_set(unit, mem, vent, LLTAG__PORT_NUMf, port_out);
            }
        } else {
            /* SOURCE_TYPE field in vent is 0 */

            /* incoming port not used in lookup key, set field to all 1's */
            /* coverity[dead_error_line : FALSE] */
            soc_mem_field32_set(unit, mem, vent, LLTAG__GLPf,
                                        SOC_VLAN_XLATE_GLP_WILDCARD(unit));
        }
        return BCM_E_NONE;
    } 
#endif
    return BCM_E_UNAVAIL;
}

#define PORT_GROUP_MAX  0xffff

/*
 * Function:
 *      _bcm_trx_vlan_translate_entry_assemble
 * Purpose:
 *      Constructs vlan translate entry from given key_type, inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      vent            (IN/OUT) vlan translate entry to construct
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      inner_vlan      (IN) inner VLAN ID
 *      outer_vlan      (IN) outer VLAN ID
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_translate_entry_assemble(int unit, void *vent,
                                      bcm_gport_t port,
                                      bcm_vlan_translate_key_t key_type,
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_t outer_vlan)
{
    int             use_glp = 1;    /* By default incoming port used in lookup key */
    int             tmp_id;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
    bcm_trunk_t     trunk_out = -1;
    int key_type_value;
    soc_mem_t       mem = VLAN_XLATEm;

#if defined(BCM_TRIDENT2_SUPPORT)
    int             use_port_group = 0;
    uint32          port_group;
#endif
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    switch (key_type) {
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupDouble:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyDouble:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortDouble:
            VLAN_CHK_ID(unit, inner_vlan);
            VLAN_CHK_ID(unit, outer_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupOuterTag:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyOuterTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuterTag:
            /* allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OTAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupInnerTag:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyInnerTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInnerTag:
            /* Allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_ITAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, ITAGf, inner_vlan);
            break;
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupOuter:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyOuter:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuter:
            VLAN_CHK_ID(unit, outer_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            break;
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupInner:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyInner:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortInner:
            VLAN_CHK_ID(unit, inner_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
#if defined(BCM_TRIDENT2_SUPPORT)
        case bcmVlanTranslateKeyPortGroupOuterPri:
            use_port_group = 1;
            /* Fall through */
#endif
        case bcmVlanTranslateKeyOuterPri:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortOuterPri:
            /* Allow Hi 4 bits in 16 bit VLAN tag */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PRI_CFI,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            /* outer_vlan: Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_VLAN_NONE */
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyCapwapPayloadDouble:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortCapwapPayloadDouble:
            VLAN_CHK_ID(unit, inner_vlan);
            VLAN_CHK_ID(unit, outer_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyCapwapPayloadOuter:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortCapwapPayloadOuter:
            VLAN_CHK_ID(unit, outer_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyCapwapPayloadInner:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortCapwapPayloadInner:
            VLAN_CHK_ID(unit, inner_vlan);
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyCapwapPayloadOuterTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortCapwapPayloadOuterTag:
            /* allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyCapwapPayloadInnerTag:
            use_glp = 0;
            /* Fall through */
        case bcmVlanTranslateKeyPortCapwapPayloadInnerTag:
            /* Allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf, key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, ITAGf, inner_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT2_SUPPORT)
    if (use_port_group) {
        if (!soc_feature(unit, soc_feature_port_group_for_ivxlt)) {
            return BCM_E_UNAVAIL;
        }
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 3);
        if((port < 0) || (port > PORT_GROUP_MAX)) {
            return BCM_E_PARAM;
        }
        port_group = port;
        soc_mem_field32_set(unit, mem, vent, SOURCE_FIELDf, port_group);
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
    if (use_glp) {
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)        
        if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        }
#endif
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out,
                                    &trunk_out, &tmp_id));
        /* In case of extended queuing on KTX, tgid_out gets updated with
         * extended queue value. So set it to invalid value.
         */
#ifdef BCM_KATANA_SUPPORT
       if (soc_feature(unit, soc_feature_extended_queueing)) {
           if ((BCM_GPORT_IS_UCAST_QUEUE_GROUP(port)) ||
               (BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(port)) ||
               (BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(port))) {
               trunk_out = BCM_TRUNK_INVALID;
           }
       }
#endif
        if (trunk_out >= 0) {
            soc_mem_field32_set(unit, mem, vent, Tf, 1);
            soc_mem_field32_set(unit, mem, vent, TGIDf, trunk_out);
        } else {
            soc_mem_field32_set(unit, mem, vent, MODULE_IDf, mod_out);
            soc_mem_field32_set(unit, mem, vent, PORT_NUMf, port_out);
        }
    } else {
        /* SOURCE_TYPE field in vent is 0 */

        /* incoming port not used in lookup key, set field to all 1's */
        soc_mem_field32_set(unit, mem, vent, GLPf,
                                    SOC_VLAN_XLATE_GLP_WILDCARD(unit));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_trx_vlan_translate_action_entry_update
 * Purpose:
 *		Update Vlan_xlate Entry.
 *              Currently it only updates the fields by the actions
 *              specified in bcm_vlan_action_set_t.
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_entry
 *  OUT :  vlan_xlate_entry

 */

STATIC int
_bcm_trx_vlan_translate_action_entry_update(int unit, void *vent,
                                            void *return_ent)
{
    uint32  key_type, value;
    uint32 l3_iif_valid  = 0;
    uint32 ret_svp_valid = 0;
#if defined(BCM_TRIDENT2_SUPPORT)
    uint32 svp_valid     = 0;
    uint32 ret_svp       = 0;
    uint32 svp           = 0;
    uint32 dummy_svp_chk = 0;
    uint32 svp_dummy     = 0;
#endif /* BCM_TRIDENT2_SUPPORT */

    bcm_vlan_t outer_vlan, inner_vlan;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
    bcm_policer_t  current_policer = 0;
    int rv = BCM_E_NONE;
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */

    soc_mem_t mem = VLAN_XLATEm;
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Check if Key_Type identical */
    key_type = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
    value = soc_mem_field32_get(unit, mem, return_ent, KEY_TYPEf);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    /* Retain original Keys -- Update data only */
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        value = soc_mem_field32_get(unit, mem, vent, NEW_OPRIf);
        soc_mem_field32_set(unit, mem, return_ent, NEW_OPRIf, value);
        value = soc_mem_field32_get(unit, mem, vent, NEW_OCFIf);
        soc_mem_field32_set(unit, mem, return_ent, NEW_OCFIf, value);
        value = soc_mem_field32_get(unit, mem, vent, NEW_IPRIf);
        soc_mem_field32_set(unit, mem, return_ent, NEW_IPRIf, value);
        value = soc_mem_field32_get(unit, mem, vent, NEW_ICFIf);
        soc_mem_field32_set(unit, mem, return_ent, NEW_ICFIf, value);
    } else {
        value = soc_mem_field32_get(unit, mem, vent, RPEf);
        soc_mem_field32_set(unit, mem, return_ent, RPEf, value);
        if (value) {
            value = soc_mem_field32_get(unit, mem, vent, PRIf);
            soc_mem_field32_set(unit, mem, return_ent, PRIf, value);
        } 
    }

    value = soc_mem_field32_get(unit, mem, vent, TAG_ACTION_PROFILE_PTRf);
    soc_mem_field32_set(unit, mem, return_ent, TAG_ACTION_PROFILE_PTRf, value);
    if (SOC_MEM_FIELD_VALID(unit, mem, VLAN_ACTION_VALIDf)) {
        value = soc_mem_field32_get(unit, mem, vent, VLAN_ACTION_VALIDf);
        soc_mem_field32_set(unit, mem, return_ent, VLAN_ACTION_VALIDf,value);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, DISABLE_VLAN_CHECKSf)) {  
        value = soc_mem_field32_get(unit, mem, vent, DISABLE_VLAN_CHECKSf);
        soc_mem_field32_set(unit, mem, return_ent,
                                    DISABLE_VLAN_CHECKSf, value);
    }

    inner_vlan = soc_mem_field32_get(unit, mem, vent, NEW_IVIDf);
    soc_mem_field32_set(unit, mem, return_ent, NEW_IVIDf, inner_vlan);
    outer_vlan = soc_mem_field32_get(unit, mem, vent, NEW_OVIDf);
    soc_mem_field32_set(unit, mem, return_ent, NEW_OVIDf, outer_vlan);

#if defined(BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_vlan_xlate_iif_with_dummy_vp) &&
        soc_property_get(unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0)) {
        svp = soc_mem_field32_get(unit, mem, vent, SOURCE_VPf);
        ret_svp = soc_mem_field32_get(unit, mem, return_ent, SOURCE_VPf);
        if (svp ==  _BCM_VLAN_XLATE_DUMMY_PORT ||
            ret_svp == _BCM_VLAN_XLATE_DUMMY_PORT) {
            dummy_svp_chk = 1;
        }
    }
#endif /* defined(BCM_TRIDENT2_SUPPORT) */

    if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
        /* Only carry forward the L3_IIF encoding and leave L2_SVP as is */
        l3_iif_valid = soc_mem_field32_get(unit, mem, vent, MPLS_ACTIONf) & 0x2;
        ret_svp_valid = 0x1 &
            soc_mem_field32_get(unit, mem, return_ent, MPLS_ACTIONf);

        /* If the device supports MPLS_ACTION value 0x3 i.e. both L3_IIF and
         * L2_SVP simultaneously then retain the SVP_VALID bit. To check if
         * the device supports both actions simultaneously, use existence
         * of L3_IIF_VALID field as this field is only present in chips which
         * has the support.
         */
        if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIF_VALIDf)) {
#if defined(BCM_TRIDENT2_SUPPORT)
            if (soc_feature(unit, soc_feature_vlan_xlate_iif_with_dummy_vp) &&
                soc_property_get(unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0) &&
                dummy_svp_chk) {

                svp_valid =
                       soc_mem_field32_get(unit, mem, vent, MPLS_ACTIONf) & 0x1;
                if ((svp_valid == 1) && (svp == _BCM_VLAN_XLATE_DUMMY_PORT)) {
                    /* svp is either invalid or a dummy VP */
                    svp_dummy = 1;
                }

                if (svp_dummy &&
                   (ret_svp_valid && ret_svp != _BCM_VLAN_XLATE_DUMMY_PORT)) {
                    /* svp information conflicts */
                    return BCM_E_PARAM;
                }

                if (!ret_svp_valid || (ret_svp == _BCM_VLAN_XLATE_DUMMY_PORT)) {
                    /*
                     * In this case, the ret_svp is either invalid or a dummy vp
                     * configured with L3 IIF, it should be updated.
                     */
                    value = l3_iif_valid | svp_valid;
                    soc_mem_field32_set(unit, mem, return_ent, SOURCE_VPf, svp);
                }
            } else
#endif /* defined(BCM_TRIDENT2_SUPPORT) */
            {
                value = l3_iif_valid | ret_svp_valid;
            }
        } else if (l3_iif_valid) {
            /* The device does not support setting simultaneously L3_IIF and
             * L2_SVP, and if L3_IIF_VALID bit is set then it takes precedence
             * over L2_SVP.
             */
            value = l3_iif_valid;
        } else {
            /* If L3_IIF_VALID bit is not set then retain the SVP_VALID */
            value = ret_svp_valid;
        }

        soc_mem_field32_set(unit, mem, return_ent, MPLS_ACTIONf, value);
    }

    /* Retreive the resolved L3_IIF_VALID field status */
    if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
        /* The second bit as it corresponds to L3_IIF_VALID */
        l3_iif_valid =
            0x2 & soc_mem_field32_get(unit, mem, return_ent, MPLS_ACTIONf);
    } else {
        /* L3_IIF_VALID bit couldnt be retrieved hence assume not valid */
        l3_iif_valid = 0;
    }


    if (l3_iif_valid) {
        if (SOC_MEM_FIELD_VALID(unit, mem, L3_IIFf)) {
            /* Update the L3_IIF field if the corresponding valid bit is set */
            value = soc_mem_field32_get(unit, mem, vent, L3_IIFf);
            soc_mem_field32_set(unit, mem, return_ent, L3_IIFf, value);
        }
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    else if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf)) {
        /* If the L3_IIF is not valid and CLASS_ID field exists, though
         * overlaid on L3_IIF field, then perform update on CLASS_ID field.
         */
        value = soc_mem_field32_get(unit, mem, vent, CLASS_IDf);
        soc_mem_field32_set(unit, mem, return_ent, CLASS_IDf, value);
    }
#endif

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_esw_get_policer_from_table(unit, mem, 0, return_ent,
                                             &current_policer, 1);
        BCM_IF_ERROR_RETURN(rv);

        /* decrement current policer ref count - if any */
        rv = _bcm_esw_policer_validate(unit, &current_policer);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_policer_decrement_ref_count(unit, current_policer);
            BCM_IF_ERROR_RETURN(rv);
        }

        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_OFFSET_MODEf);
        soc_mem_field32_set(unit, mem, return_ent,
                            SVC_METER_OFFSET_MODEf, value);
        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_INDEXf);
        soc_mem_field32_set(unit, mem, return_ent, SVC_METER_INDEXf, value);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        SOC_IS_GREYHOUND2(unit)) {
        rv = _bcm_esw_get_policer_from_table(unit, mem, 0, return_ent,
                                             &current_policer, 1);
        BCM_IF_ERROR_RETURN(rv);

        /* decrement current policer ref count - if any */
        rv = _bcm_esw_policer_validate(unit, &current_policer);
        if (BCM_SUCCESS(rv)) {
            rv = _bcm_esw_policer_decrement_ref_count(unit, current_policer);
            BCM_IF_ERROR_RETURN(rv);
        }

        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_OFFSET_MODEf);
        soc_mem_field32_set(unit, mem, return_ent,
                            SVC_METER_OFFSET_MODEf, value);
        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_INDEXf);
        soc_mem_field32_set(unit, mem, return_ent, SVC_METER_INDEXf, value);
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        (SOC_IS_KATANAX(unit) || (SOC_IS_APACHE(unit)
                                  && !SOC_IS_MONTEREY(unit)))) {
        rv = _bcm_esw_get_policer_from_table(unit, mem, 0, return_ent,
                                             &current_policer, 1);
        BCM_IF_ERROR_RETURN(rv);
        /* decrement current policer ref count - if any */
        if ((current_policer & BCM_POLICER_GLOBAL_METER_INDEX_MASK) > 0) {
            rv = _bcm_esw_policer_decrement_ref_count(unit, current_policer);
            BCM_IF_ERROR_RETURN(rv);
        }
        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_OFFSET_MODEf);
        soc_mem_field32_set(unit, mem, return_ent,
                                                SVC_METER_OFFSET_MODEf, value);
        value = soc_mem_field32_get(unit, mem, vent, SVC_METER_INDEXf);
        soc_mem_field32_set(unit, mem, return_ent, SVC_METER_INDEXf, value);
    }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT */

#if defined(BCM_TSN_SUPPORT)
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        value = soc_mem_field32_get(unit, mem, vent, SR_ENABLEf);
        soc_mem_field32_set(unit, mem, return_ent, SR_ENABLEf, value);
        value = soc_mem_field32_get(unit, mem, vent, SR_LAN_ID_VALIDf);
        soc_mem_field32_set(unit, mem, return_ent, SR_LAN_ID_VALIDf, value);
        value = soc_mem_field32_get(unit, mem, vent, SR_LAN_IDf);
        soc_mem_field32_set(unit, mem, return_ent, SR_LAN_IDf, value);
    }
#endif /* BCM_TSN_SR_SUPPORT*/
    if (soc_feature(unit, soc_feature_tsn_taf) &&
        SOC_MEM_FIELD_VALID(unit, mem, TAF_GATE_ID_PROFILEf)) {
        value = soc_mem_field32_get(unit, mem, vent, TAF_GATE_ID_PROFILEf);
        soc_mem_field32_set(unit, mem, return_ent, TAF_GATE_ID_PROFILEf, value);
    }
#endif /* BCM_TSN_SUPPORT */
   return BCM_E_NONE;
}

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int
_bcm_tr3_vxlate_extd_action_entry_update(int unit,
               vlan_xlate_entry_t *vent_in,  /* update entry */
               void *ctxt,
               vlan_xlate_entry_t *vent_out, /* entry read from mem */
               vlan_xlate_extd_entry_t *vxent_out, /* entry read from mem or
                                   converted from vent_out if not null */
               int *use_extd_tbl) 
{
    vlan_xlate_entry_t vent_new;
    bcm_vlan_action_set_t *action;
#ifdef INCLUDE_L3
    int intf_map_mode = 0;
    int min_l3_iif;
    int max_l3_iif;
#endif

    *use_extd_tbl = FALSE;
    action = (bcm_vlan_action_set_t *)ctxt;
    sal_memset(&vent_new, 0, sizeof(vent_new));

    /* if a regular entry from memory table is given, then update it */
    if (vent_out) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_action_entry_update (unit,
                    vent_in, vent_out));

        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate2vxlate_extd(unit,vent_out,vxent_out));
    } else {
        /* if a regular entry isn't given instead a extd entry, then
         * first convert to a regular entry, update it and convert back
         * to a extd entry. This is to utilize the existing regular entry
         * update function 
         */
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate_extd2vxlate(unit,vxent_out,&vent_new,0));
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_action_entry_update (unit,
                    vent_in, &vent_new));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate2vxlate_extd(unit,&vent_new,vxent_out));
    }
#ifdef INCLUDE_L3
    max_l3_iif = BCM_XGS3_L3_ING_IF_TBL_SIZE(unit);

#if defined(BCM_TRIUMPH_SUPPORT)
    if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
        BCM_IF_ERROR_RETURN (bcm_xgs3_l3_ingress_intf_map_get(unit,
                           &intf_map_mode));
    }
#endif

    if (intf_map_mode) {
        min_l3_iif = 0;
    } else {
        min_l3_iif = BCM_VLAN_MAX + 1;
    }

    if ((action->ingress_if >= min_l3_iif) &&
                      (action->ingress_if < max_l3_iif)) {
        *use_extd_tbl = TRUE;
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxent_out,
                                    XLATE__MPLS_ACTIONf, 0x2); /* L3_IIF */
        soc_VLAN_XLATE_EXTDm_field32_set(unit, vxent_out, XLATE__L3_IIFf,
                                    action->ingress_if);
    }
#endif  /* INCLUDE_L3 */

    if (SOC_IS_TRIUMPH3(unit) && action->policer_id) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_add_policer_to_table(unit, action->policer_id,
                                     VLAN_XLATE_EXTDm, 0, vxent_out));
        *use_extd_tbl = TRUE;
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *		_bcm_tr3_vlan_translate_action_entry_set
 * Purpose:
 *		Set Vlan Translate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_entry
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_tr3_vlan_translate_action_entry_set(int unit, vlan_xlate_entry_t *vent,
                            bcm_vlan_action_set_t *action)
{

    BCM_IF_ERROR_RETURN(
         _bcm_tr3_vxlate_entry_add(unit, vent,(void *)action,
                _bcm_tr3_vxlate_extd_action_entry_update, NULL));
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *		_bcm_trx_vlan_translate_action_entry_set
 * Purpose:
 *		Set Vlan Translate Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_entry
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_trx_vlan_translate_action_entry_set(int unit, void *vent)
{
    vlan_xlate_entry_t return_vxent;
    vlan_xlate_1_double_entry_t return_vx1dent;
    int rv = BCM_E_UNAVAIL;
    int index;
    uint32 old_profile_idx = 0;
    soc_mem_t mem = VLAN_XLATEm;
    void *return_ent = &return_vxent;

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        return_ent = &return_vx1dent;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                        vent, return_ent, 0);
	
    if (rv == SOC_E_NONE) {
         /* Keep the old vlan translate profile index */
         old_profile_idx = soc_mem_field32_get(unit, mem, return_ent,
                                                  TAG_ACTION_PROFILE_PTRf);
         BCM_IF_ERROR_RETURN(
             _bcm_trx_vlan_translate_action_entry_update (unit, 
                     vent, return_ent));
         rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, return_ent);

         /* Delete the old vlan translate profile entry */
         if ((rv == SOC_E_NONE) &&
             (old_profile_idx != ing_action_profile_def[unit])) {
             rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);

             if (BCM_FAILURE(rv)) {
                 /* ignore if the non-used hardware default value */
                 if ((!old_profile_idx) && (rv == SOC_E_PARAM ||
                                            rv == SOC_E_NOT_FOUND)) {
                     rv = BCM_E_NONE;
                 } else {
                     BCM_IF_ERROR_RETURN(rv);
                 }
             }
         }
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent);
    }

    return rv;
}


/*
 * Function   :
 *      _bcm_trx_vif_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
STATIC int 
_bcm_trx_vif_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_niv)) {
        uint32 profile_idx; 
        int rv = BCM_E_NONE;
        int search_rv;
        vlan_xlate_entry_t vx_ent;
        vlan_xlate_entry_t search_vx_ent;
        vlan_xlate_1_double_entry_t vx1d_ent;
        vlan_xlate_1_double_entry_t search_vx1d_ent;
        int key_type_value;
        int old_profile_idx = 0;
        int index;
    
        soc_mem_t mem = VLAN_XLATEm;
        soc_field_t vf = VALIDf;
        void *vent = &vx_ent;
        void *search_vent = &search_vx_ent;
        uint32 ent_sz = sizeof(vlan_xlate_entry_t);

        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
            vf = BASE_VALID_0f; /* TD3TBD */
            vent = &vx1d_ent;
            search_vent= &search_vx1d_ent;
            ent_sz = sizeof(vlan_xlate_1_double_entry_t);
        }

        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    
        sal_memset(vent, 0, ent_sz);
        BCM_IF_ERROR_RETURN(_bcm_trx_vif_vlan_translate_entry_assemble(unit, 
                vent, port, key_type, inner_vlan, outer_vlan));
    
        sal_memcpy(search_vent, vent, ent_sz);
     
        /* NIV port vxlate key type */
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_VIF, &key_type_value));
        soc_mem_field32_set(unit, mem, search_vent, KEY_TYPEf,
                                          key_type_value);
        if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, mem, search_vent, DATA_TYPEf, key_type_value);
        }
        soc_mem_field32_set(unit, mem, search_vent, VIF__VLANf, 0);
    
        search_rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            search_vent, vent, 0);
    
        /* an entry should already exist */
        if (search_rv != SOC_E_NONE) {
            return search_rv;
        }
    
        /* re-initialize the key */
        BCM_IF_ERROR_RETURN(_bcm_trx_vif_vlan_translate_entry_assemble(unit, 
               vent, port, key_type, inner_vlan, outer_vlan));
        sal_memcpy(search_vent, vent, ent_sz);
        search_rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                            search_vent, vent, 0);
    
        if (!(search_rv == SOC_E_NONE || search_rv == SOC_E_NOT_FOUND)) {
            return search_rv;
        }
        if (search_rv == SOC_E_NONE) {
            old_profile_idx = soc_mem_field32_get(unit, mem, vent,
                                   VIF__TAG_ACTION_PROFILE_PTRf);
        }
    
        /* update or fill in the vlan xlate entry*/
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_add(unit, 
                       action, &profile_idx));
        soc_mem_field32_set(unit, mem, vent, VIF__TAG_ACTION_PROFILE_PTRf,
                                    profile_idx);
        if (SOC_MEM_FIELD_VALID(unit, mem, VIF__VLAN_ACTION_VALIDf)) {
            soc_mem_field32_set(unit, mem, vent, VIF__VLAN_ACTION_VALIDf, 1);
        }
    
        soc_mem_field32_set(unit, mem, vent, VIF__NEW_IVIDf,
                                    action->new_inner_vlan);
        soc_mem_field32_set(unit, mem, vent, VIF__NEW_OVIDf,
                                    action->new_outer_vlan);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (action->priority >= BCM_PRIO_MIN &&
                action->priority <= BCM_PRIO_MAX) {
                soc_mem_field32_set(unit, mem, vent, VIF__NEW_OPRIf,
                          action->priority);
            }
            soc_mem_field32_set(unit, mem, vent, VIF__NEW_OCFIf,
                                        action->new_outer_cfi);
            soc_mem_field32_set(unit, mem, vent, VIF__NEW_IPRIf,
                                        action->new_inner_pkt_prio);
            soc_mem_field32_set(unit, mem, vent, VIF__NEW_ICFIf,
                                        action->new_inner_cfi);
        } 
        if (mem == VLAN_XLATEm) {
            soc_mem_field32_set(unit, mem, vent, vf, 1);
        } else {
            soc_mem_field32_set(unit, mem, vent, vf, 3);
            soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
        }

        /* Program L3_IIF */
        if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm, VIF__L3_IIF_VALIDf)) {
            int intf_map_mode = 0;
            int min_l3_iif;
            int max_l3_iif;
            max_l3_iif = BCM_XGS3_L3_ING_IF_TBL_SIZE(unit) - 1;
            if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
                BCM_IF_ERROR_RETURN(
                    bcm_xgs3_l3_ingress_intf_map_get(unit, &intf_map_mode));
            }

            if (intf_map_mode) {
                if (soc_feature(unit, soc_feature_l3_iif_zero_invalid)){
                    min_l3_iif = 1;
                } else {
                    min_l3_iif = 0;
                }
            } else {
                min_l3_iif = BCM_VLAN_MAX + 1;
            }
            if ((action->ingress_if >= min_l3_iif) &&
                (action->ingress_if <= max_l3_iif)) {
                soc_mem_field32_set(unit, mem, vent, VIF__L3_IIF_VALIDf, 1);
                soc_mem_field32_set(unit, mem, vent, VIF__L3_IIFf,
                                            action->ingress_if);
            }
        }
    
        if (search_rv == SOC_E_NONE) {
            /* overrides existing one */
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, vent);
        } else { /* case search_rv == SOC_E_NOT_FOUND */
           rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent);
        }
    
        if (BCM_FAILURE(rv)) {
            /* Delete the allocated vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                             VIF__TAG_ACTION_PROFILE_PTRf);
            (void) _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        } else {
            /* delete t the old profile entry */
            if ((search_rv == SOC_E_NONE) &&
                (old_profile_idx != ing_action_profile_def[unit])) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                         old_profile_idx);
            }
        }
        return rv;
    }
#endif   /* defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) */
    return BCM_E_UNAVAIL;

}


/*
 * Function   :
 *      _bcm_trx_lltag_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table for LLTAG based VLAN port.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int 
_bcm_trx_lltag_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
    uint32 profile_idx; 
    int rv;
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    ing_dvp_table_entry_t dvp_entry;
    egr_l3_next_hop_entry_t egr_nh;
    uint16 llvid, other_vlan;
    int vp, nh_index;
    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    uint32 ent_sz = sizeof(vlan_xlate_entry_t);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        ent_sz = sizeof(vlan_xlate_1_double_entry_t);
    }

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));

    if (!BCM_GPORT_IS_VLAN_PORT(port)) {
        return BCM_E_PARAM;
    }

    vp = BCM_GPORT_VLAN_PORT_ID_GET(port);

    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }

    /* For existing vlan vp, NH entry already exists */
    BCM_IF_ERROR_RETURN
        (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

    nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
           NEXT_HOP_INDEXf);

    BCM_IF_ERROR_RETURN
        (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh));

    llvid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
           SD_TAG__LLTAG_VIDf);

    sal_memset(vent, 0, ent_sz);

    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            other_vlan = 0;
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            other_vlan = outer_vlan;
            break;
        case  bcmVlanTranslateKeyPortPonTunnelInner:
            other_vlan = inner_vlan;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }
    
    BCM_IF_ERROR_RETURN(
        _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, other_vlan));

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));
    soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf, profile_idx);
    if (SOC_MEM_FIELD_VALID(unit, VLAN_XLATEm, VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VLAN_ACTION_VALIDf, 1);
    }

    soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, action->new_inner_vlan);
    soc_mem_field32_set(unit, mem, vent, NEW_OVIDf, action->new_outer_vlan);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, vent, NEW_OPRIf, action->priority);
        }
        soc_mem_field32_set(unit, mem, vent, NEW_OCFIf, action->new_outer_cfi);
        soc_mem_field32_set(unit, mem, vent, NEW_IPRIf, action->new_inner_pkt_prio);
        soc_mem_field32_set(unit, mem, vent, NEW_ICFIf, action->new_inner_cfi);
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, vent, RPEf, 1);
            soc_mem_field32_set(unit, mem, vent, PRIf, action->priority);
        }
    }

    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

#ifdef INCLUDE_L3
    if (SOC_IS_TR_VL(unit) && (!SOC_IS_HURRICANEX(unit))) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf) &&
                (action->class_id)) {
            soc_mem_field32_set(unit, mem, vent, CLASS_IDf, action->class_id);
        } else
#endif
            if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
            int intf_map_mode = 0; 
            int min_l3_iif; 
            int max_l3_iif; 

            max_l3_iif = BCM_XGS3_L3_ING_IF_TBL_SIZE(unit); 
#if defined(BCM_TRIUMPH_SUPPORT)
            if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
                BCM_IF_ERROR_RETURN (bcm_xgs3_l3_ingress_intf_map_get(unit,
                                   &intf_map_mode));
    }
#endif

            if (intf_map_mode) { 
                min_l3_iif = 0;
            } else {
                min_l3_iif = BCM_VLAN_MAX + 1;
            }
            
            if ((action->ingress_if >= min_l3_iif) && 
                      (action->ingress_if < max_l3_iif)) {
                /* L3_IIF */
                soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x2);
                soc_mem_field32_set(unit, mem, vent, L3_IIFf, action->ingress_if);
            } 
        }
    }
#endif /* INCLUDE_L3 */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_KATANAX(unit)) {
        rv = _bcm_esw_add_policer_to_table(unit, action->policer_id,
                                           mem, 0, vent);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT */
    rv = _bcm_trx_vlan_translate_action_entry_set(unit, vent);

    if (BCM_FAILURE(rv)) {
        /* Delete the old vlan translate profile entry */
        profile_idx = soc_mem_field32_get(unit, mem, vent,
                                                  TAG_ACTION_PROFILE_PTRf);
        (void) _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }
   
    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_add
 * Description   :
 *      Add an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int 
_bcm_trx_vlan_translate_action_add(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  bcm_vlan_action_set_t *action)
{
    uint32 profile_idx; 
    int rv, vp = 0;
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    uint16 llvid = 0;
    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    uint32 ent_sz = sizeof(vlan_xlate_entry_t);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        ent_sz = sizeof(vlan_xlate_1_double_entry_t);
    }

    if ((key_type == bcmVlanTranslateKeyPortPonTunnel || 
        key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
        key_type == bcmVlanTranslateKeyPortPonTunnelInner) && 
        !soc_feature(unit, soc_feature_lltag)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TRIDENT2_SUPPORT)
    if ((!soc_feature(unit, soc_feature_vlan_xlate_iif_with_dummy_vp) ||
         !soc_property_get(unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0)) &&
         (action->flags & BCM_VLAN_ACTION_SET_DUMMY_VP)) {
        return BCM_E_UNAVAIL;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* process the NIV type of gport */
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        int rv;
        rv = _bcm_trx_vif_vlan_translate_action_add(unit,
             port,key_type,outer_vlan,inner_vlan,action);
        return rv;
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        if (key_type == bcmVlanTranslateKeyPortPonTunnel || 
            key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
            key_type == bcmVlanTranslateKeyPortPonTunnelInner) {
            ing_dvp_table_entry_t dvp_entry;
            egr_l3_next_hop_entry_t egr_nh;
            int nh_index;

            if (!soc_feature(unit, soc_feature_lltag)) {
                return BCM_E_UNAVAIL;
            }

            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
            /* For existing vlan vp, NH entry already exists */
            BCM_IF_ERROR_RETURN
                    (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                    NEXT_HOP_INDEXf);

            BCM_IF_ERROR_RETURN
                    (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh));

            llvid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__LLTAG_VIDf);
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));

    sal_memset(vent, 0, ent_sz);
    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, 0); 
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelInner:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, inner_vlan);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               inner_vlan, outer_vlan);
            break;        
    }
    
    BCM_IF_ERROR_RETURN(rv);
	    
    if (key_type == bcmVlanTranslateKeyPortPonTunnel ||
        key_type == bcmVlanTranslateKeyPortPonTunnelOuter ||
        key_type == bcmVlanTranslateKeyPortPonTunnelInner) {
        soc_mem_field32_set(unit, mem, vent, LLTAG__SOURCE_VPf, vp);
        if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
            soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
        } 
        if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
            soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 1);
        }
    }
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));
    soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf,
                                profile_idx);
    if (SOC_MEM_FIELD_VALID(unit, mem, VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VLAN_ACTION_VALIDf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, DISABLE_VLAN_CHECKSf)) {
        if (action->flags & BCM_VLAN_ACTION_SET_VLAN_CHECKS_DISABLE) {
            soc_mem_field32_set(unit, mem, vent, DISABLE_VLAN_CHECKSf, 1);
        }
    }
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if ((!(action->flags & BCM_VLAN_ACTION_SET_SR_ENABLE)) &&
              (action->flags & BCM_VLAN_ACTION_SET_SR_LAN_ID)) {
           return BCM_E_PARAM;
        }

        if (action->flags & BCM_VLAN_ACTION_SET_SR_ENABLE) {
            soc_mem_field32_set(unit, mem, vent, SR_ENABLEf, 1);
            soc_mem_field32_set(unit, mem, vent, SR_LAN_ID_VALIDf, 1);
            if (action->flags & BCM_VLAN_ACTION_SET_SR_LAN_ID) {
                soc_mem_field32_set(unit, mem, vent, SR_LAN_IDf, 1);
            }
        }
    }
#endif /* BCM_TSN_SR_SUPPORT*/

#if defined(BCM_TSN_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_taf) &&
        SOC_MEM_FIELD_VALID(unit, mem, TAF_GATE_ID_PROFILEf)) {
        uint32 hw_index;

        if (BCM_TSN_PRI_MAP_INVALID != action->taf_gate_primap) {
            bcm_tsn_pri_map_type_t map;

            BCM_IF_ERROR_RETURN(
                bcmi_esw_tsn_pri_map_hw_index_get(unit, action->taf_gate_primap,
                                                  &map, &hw_index));

            if (map != bcmTsnPriMapTypeTafGate) {
                return BCM_E_PARAM;
            }
        } else {
            hw_index = 0;
        }
        soc_mem_field32_set(unit, mem, vent,
                            TAF_GATE_ID_PROFILEf, hw_index);
    }
#endif /* BCM_TSN_SUPPORT */

    soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, action->new_inner_vlan);
    soc_mem_field32_set(unit, mem, vent, NEW_OVIDf, action->new_outer_vlan);
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, vent, NEW_OPRIf, action->priority);
        }
        soc_mem_field32_set(unit, mem, vent, NEW_OCFIf, action->new_outer_cfi);
        soc_mem_field32_set(unit, mem, vent, NEW_IPRIf,
                                    action->new_inner_pkt_prio);
        soc_mem_field32_set(unit, mem, vent, NEW_ICFIf, action->new_inner_cfi);
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, vent, RPEf, 1);
            soc_mem_field32_set(unit, mem, vent, PRIf, action->priority);
        }
    }
    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = _bcm_tr3_vlan_translate_action_entry_set(unit, vent, action);
        if (BCM_FAILURE(rv)) {
            /* Delete the old vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                                  TAG_ACTION_PROFILE_PTRf);
            (void) _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
#ifdef INCLUDE_L3
        if (SOC_IS_TR_VL(unit) &&
            (!SOC_IS_HURRICANE(unit) || !SOC_IS_HURRICANE2(unit))) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf) && action->class_id) {
                soc_mem_field32_set(
                    unit, mem, vent, CLASS_IDf, action->class_id);
            } else
#endif
            if (SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
                int intf_map_mode = 0;
                int min_l3_iif;   /* the minimum valid l3_iif index */
                int max_l3_iif;   /* the maximum valid l3_iif index */

                max_l3_iif = BCM_XGS3_L3_ING_IF_TBL_SIZE(unit) - 1;
#if defined(BCM_TRIUMPH_SUPPORT)
                if (soc_feature(unit, soc_feature_l3_ingress_interface)) {
                    BCM_IF_ERROR_RETURN (
                        bcm_xgs3_l3_ingress_intf_map_get(unit, &intf_map_mode));
                }
#endif

                if (intf_map_mode) {
                    if (soc_feature(unit, soc_feature_l3_iif_zero_invalid)){
                        min_l3_iif = 1;
                    } else {
                        min_l3_iif = 0;
                    }
                } else {
                    min_l3_iif = BCM_VLAN_MAX + 1;
                }

                if ((action->ingress_if >= min_l3_iif) &&
                      (action->ingress_if <= max_l3_iif)) {
                    /* L3_IIF */
#if defined(BCM_TRIDENT2_SUPPORT)
                    if ((action->flags & BCM_VLAN_ACTION_SET_DUMMY_VP) &&
                        soc_feature(unit,
                            soc_feature_vlan_xlate_iif_with_dummy_vp) &&
                        soc_property_get(
                            unit, spn_VLAN_ACTION_DUMMY_VP_RESERVED, 0)) {
                        soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x3);
                        soc_mem_field32_set(unit, mem, vent, L3_IIFf,
                                            action->ingress_if);
                        soc_mem_field32_set(unit, mem, vent, SOURCE_VPf,
                                            _BCM_VLAN_XLATE_DUMMY_PORT);
                    } else
#endif /* BCM_TRIDENT2_SUPPORT */
                    {
                        soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x2);
                        soc_mem_field32_set(unit, mem, vent, L3_IIFf,
                                            action->ingress_if);
                    }
                }
            }
        }
#endif /* INCLUDE_L3 */

#if defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter) &&
            SOC_IS_GREYHOUND2(unit)) {
            rv = _bcm_esw_policer_validate(unit, &action->policer_id);
            if (BCM_SUCCESS(rv)) {
                rv = _bcm_esw_add_policer_to_table(
                        unit, action->policer_id,
                        mem, 0, vent);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter) &&
            (SOC_IS_KATANAX(unit) || SOC_IS_APACHE(unit))) {
            rv = _bcm_esw_add_policer_to_table(unit, action->policer_id,
                                               mem, 0, vent);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT */
        rv = _bcm_trx_vlan_translate_action_entry_set(unit, vent);

        if (BCM_FAILURE(rv)) {
            /* Delete the old vlan translate profile entry */
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                                      TAG_ACTION_PROFILE_PTRf);
            (void) _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_get
 * Purpose:
 *      Gets a vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      old_vlan        (IN) Packet old VLAN ID
 *      new_vlan        (OUT) Packet new VLAN ID
 *      prio            (OUT) Packet prio
 */
int 
_bcm_trx_vlan_translate_action_get (int unit, 
                                   bcm_gport_t port, 
                                   bcm_vlan_translate_key_t key_type, 
                                   bcm_vlan_t outer_vid,
                                   bcm_vlan_t inner_vid,
                                   bcm_vlan_action_set_t *action)
{
    vlan_xlate_entry_t  vx_ent, res_vx_ent;
    vlan_xlate_1_double_entry_t  vx1d_ent, res_vx1d_ent;
    int                 rv;
    int                 idx = 0;
    uint32              profile_idx = 0;
    uint16 llvid = 0;
    
    soc_mem_t mem = VLAN_XLATEm;
    void *vent = &vx_ent;
    void *res_vent = &res_vx_ent;
    uint32 ent_sz = sizeof(vlan_xlate_entry_t);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vent = &vx1d_ent;
        res_vent = &res_vx1d_ent;
        ent_sz = sizeof(vlan_xlate_1_double_entry_t);
    }

    if ((key_type == bcmVlanTranslateKeyPortPonTunnel || 
       key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
       key_type == bcmVlanTranslateKeyPortPonTunnelInner) &&
       !soc_feature(unit, soc_feature_lltag)) {
        return BCM_E_PARAM;
    }

    sal_memset(vent, 0, ent_sz);
    sal_memset(res_vent, 0, ent_sz);
    if (BCM_GPORT_IS_NIV_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vif_vlan_translate_entry_assemble(unit, vent, port,
                         key_type,inner_vid, outer_vid));

        rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
                            vent, res_vent, 0);
        BCM_IF_ERROR_RETURN(rv);
        profile_idx = soc_mem_field32_get(unit, mem, res_vent,
                                      VIF__TAG_ACTION_PROFILE_PTRf);       
        _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);
        return _bcm_trx_vif_vlan_translate_entry_parse(unit, mem,
                                               (uint32 *)res_vent, action);
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        if (key_type == bcmVlanTranslateKeyPortPonTunnel || 
            key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
            key_type == bcmVlanTranslateKeyPortPonTunnelInner) {
            ing_dvp_table_entry_t dvp_entry;
            egr_l3_next_hop_entry_t egr_nh;
            int vp, nh_index;

            if (!soc_feature(unit, soc_feature_lltag)) {
                return BCM_E_UNAVAIL;
            }

            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);;
            /* For existing vlan vp, NH entry already exists */
            BCM_IF_ERROR_RETURN
                    (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                    NEXT_HOP_INDEXf);

            BCM_IF_ERROR_RETURN
                    (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh));

            llvid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__LLTAG_VIDf);
        }
    }

    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, 0); 
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, outer_vid);
            break;
        case bcmVlanTranslateKeyPortPonTunnelInner:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, inner_vid);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               inner_vid, outer_vid);
            break;        
    }
    
    BCM_IF_ERROR_RETURN(rv);
    
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, vent, res_vent, 0);
    soc_mem_unlock(unit, mem);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        vlan_xlate_extd_entry_t vxent;
        vlan_xlate_extd_entry_t res_vxent;

        /* check the extd table */
        if (rv == SOC_E_NOT_FOUND) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_vxlate2vxlate_extd(unit, vent, &vxent));

            soc_mem_lock(unit, VLAN_XLATE_EXTDm);
            rv = soc_mem_search(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL, &idx, 
                            &vxent, &res_vxent, 0);
            soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
            BCM_IF_ERROR_RETURN(rv);
            profile_idx = soc_VLAN_XLATE_EXTDm_field32_get(unit, &res_vxent,
                                          TAG_ACTION_PROFILE_PTRf);       
            _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

             return _bcm_trx_vlan_translate_entry_parse(unit, VLAN_XLATE_EXTDm,
                                               (uint32 *)&res_vxent, action);  
            
        }
    } 
#endif
    BCM_IF_ERROR_RETURN(rv);
    profile_idx = soc_mem_field32_get(unit, mem, res_vent,
                                              TAG_ACTION_PROFILE_PTRf);       
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    return _bcm_trx_vlan_translate_entry_parse(unit, mem,
                                               res_vent, action);
}


/*
 * Function:
 *      bcm_vlan_translate_action_delete
 * Purpose:
 *      Delete a vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      key_type        (IN) Key Type : bcmVlanTranslateKey*
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int 
_bcm_trx_vlan_translate_action_delete(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_translate_key_t key_type,
                                     bcm_vlan_t outer_vlan,
                                     bcm_vlan_t inner_vlan)
{
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    uint32 profile_idx;
    int rv;
#if defined(BCM_TRIUMPH3_SUPPORT)
    int rv_vxlate_delete;
#endif
    uint16 llvid = 0;

    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    uint32 ent_sz = sizeof(vx_ent);

#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t handle;
    _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
#endif


    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        ent_sz = sizeof(vx1d_ent);
    }

    if ((key_type == bcmVlanTranslateKeyPortPonTunnel || 
       key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
       key_type == bcmVlanTranslateKeyPortPonTunnelInner) &&
       !soc_feature(unit, soc_feature_lltag)) {
        return BCM_E_PARAM;
    }
        
    sal_memset(vent, 0, ent_sz);

    if (BCM_GPORT_IS_NIV_PORT(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vif_vlan_translate_entry_assemble(unit, vent, port,
                         key_type,inner_vlan, outer_vlan));
        rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL,
                                       vent, vent);
        if ((rv == SOC_E_NONE) && 
            (soc_mem_field32_get(unit, mem, vent, vf) == 3) &&
            (soc_mem_field32_get(unit, mem, vent, BASE_VALID_1f) == 7)) {
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                        VIF__TAG_ACTION_PROFILE_PTRf);       
            /* Delete the old vlan action profile entry */
            if (profile_idx != ing_action_profile_def[unit]) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                               profile_idx);
            }
        }
        return rv;
    } else if (BCM_GPORT_IS_VLAN_PORT(port)) {
        if (key_type == bcmVlanTranslateKeyPortPonTunnel || 
            key_type == bcmVlanTranslateKeyPortPonTunnelOuter || 
            key_type == bcmVlanTranslateKeyPortPonTunnelInner) {
            ing_dvp_table_entry_t dvp_entry;
            egr_l3_next_hop_entry_t egr_nh;
            int vp, nh_index;

            if (!soc_feature(unit, soc_feature_lltag)) {
                return BCM_E_UNAVAIL;
            }

            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
            /* For existing vlan vp, NH entry already exists */
            BCM_IF_ERROR_RETURN
                    (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp_entry));

            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp_entry,
                    NEXT_HOP_INDEXf);

            BCM_IF_ERROR_RETURN
                    (READ_EGR_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY, nh_index, &egr_nh));

            llvid = soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh,
                    SD_TAG__LLTAG_VIDf);
        }
    }

    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, 0); 
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelInner:
            rv = _bcm_trx_lltag_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               llvid, inner_vlan);
            break;
        default:
            rv = _bcm_trx_vlan_translate_entry_assemble(unit, vent, port, key_type,
                                               inner_vlan, outer_vlan);
            break;        
    }
    
    BCM_IF_ERROR_RETURN(rv);


#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_gport_service_counters)) {
            /* Record flex stat info before the returned entry
             * includes non-key info. */

            /* _BCM_FLEX_STAT_HANDLE_COPY(handle, vent); */
            memcpy(&handle, vent, sizeof(_bcm_flex_stat_handle_t));
        }
#endif

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL, vent, vent);
#if defined(BCM_TRIUMPH3_SUPPORT)
    rv_vxlate_delete = rv;
#endif
    if ((rv == SOC_E_NONE) && soc_mem_field32_get(unit, mem, vent, vf)) {
        profile_idx = soc_mem_field32_get(unit, mem, vent,
                                                  TAG_ACTION_PROFILE_PTRf);       
        /* Delete the old vlan action profile entry */
        if (profile_idx != ing_action_profile_def[unit]) {
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }

#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            (0 != soc_mem_field32_get(unit, mem, vent, VINTF_CTR_IDXf))) {
            /* Release Service counter */
            _bcm_esw_flex_stat_ext_handle_free(unit, _bcmFlexStatTypeVxlt,
                                               handle);
        }
#endif
    }
#if defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        SOC_IS_GREYHOUND2(unit)) {
        bcm_policer_t current_policer_id;
        int policer_rv = 0;
        policer_rv = _bcm_esw_get_policer_from_table(unit, VLAN_XLATEm, 0, vent,
                                             &current_policer_id, 1);
        BCM_IF_ERROR_RETURN(policer_rv);
        policer_rv = _bcm_esw_policer_validate(unit, &current_policer_id);
        if (BCM_SUCCESS(policer_rv)) {
            policer_rv = _bcm_esw_policer_decrement_ref_count(
                    unit, current_policer_id);
            BCM_IF_ERROR_RETURN(policer_rv);
        }
    }
#endif /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
       (SOC_IS_KATANAX(unit) || SOC_IS_APACHE(unit))) {
        bcm_policer_t current_policer;
        int policer_rv = 0;
        policer_rv = _bcm_esw_get_policer_from_table(unit, VLAN_XLATEm, 0, vent,
                                             &current_policer, 1);
        if (current_policer != 0) {
            if ((current_policer & BCM_POLICER_GLOBAL_METER_INDEX_MASK) > 0) {
                policer_rv = _bcm_esw_policer_decrement_ref_count(unit, current_policer);
                BCM_IF_ERROR_RETURN(policer_rv);
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        vlan_xlate_extd_entry_t vxent;

        /* check the extend table */
        sal_memset(&vxent, 0, sizeof(vxent));
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate2vxlate_extd(unit, vent, &vxent));
        rv = soc_mem_delete_return_old(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ALL,
                     &vxent, &vxent);
        if ((rv == SOC_E_NONE) && soc_VLAN_XLATE_EXTDm_field32_get(unit, 
                        &vxent, VALID_0f)) {
            profile_idx = soc_VLAN_XLATE_EXTDm_field32_get(unit, &vxent,
                                                  TAG_ACTION_PROFILE_PTRf); 
            /* Delete the old vlan action profile entry */
            if (profile_idx != ing_action_profile_def[unit]) {
                rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                                                               profile_idx);
            }
        } else {
            /* if an entry is already found in regular table, then reset ret code*/
            if (rv == BCM_E_NOT_FOUND) {
                if (rv_vxlate_delete == BCM_E_NONE) {
                    rv = BCM_E_NONE;
                }
            }
        }
    }
#endif

    return rv;
}

#if defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _bcm_tr3_vxlate_action_delete_extd_tbl
 * Purpose:
 *      Delete all vlan translate lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr3_vxlate_action_delete_extd_tbl(int unit) 
{
    int i, imin, imax, nent, vbytes, rv, mpls;
    vlan_xlate_extd_entry_t * vtab, * vnull,  *vtabp;
    
    imin = soc_mem_index_min(unit, VLAN_XLATE_EXTDm);
    imax = soc_mem_index_max(unit, VLAN_XLATE_EXTDm);
    nent = soc_mem_index_count(unit, VLAN_XLATE_EXTDm);
    vbytes = soc_mem_entry_words(unit, VLAN_XLATE_EXTDm);
    vbytes = WORDS2BYTES(vbytes);
    vtab = soc_cm_salloc(unit, nent * sizeof(*vtab), "vlan_xlate_extd");

    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vnull = soc_mem_entry_null(unit, VLAN_XLATE_EXTDm);

    soc_mem_lock(unit, VLAN_XLATE_EXTDm);
    rv = soc_mem_read_range(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
        soc_cm_sfree(unit, vtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        vtabp = soc_mem_table_idx_to_pointer(unit, VLAN_XLATE_EXTDm,
                                     vlan_xlate_extd_entry_t *, vtab, i);

        mpls = soc_VLAN_XLATE_EXTDm_field32_get(unit, vtabp, 
                            XLATE__MPLS_ACTIONf);

        if (!soc_VLAN_XLATE_EXTDm_field32_get(unit, vtabp, VALID_0f) ||
            (mpls != 0)) {
            continue;
        }

        rv = WRITE_VLAN_XLATE_EXTDm(unit, MEM_BLOCK_ANY, i, vnull);
    }
    
    soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
    soc_cm_sfree(unit, vtab);
    return rv;
}
#endif   /*BCM_TRIUMPH3_SUPPORT */

/*
 * Function:
 *      _bcm_trx_vlan_translate_action_delete_all
 * Purpose:
 *      Delete all vlan translate lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_translate_action_delete_all(int unit) 
{
    int i, imin, imax, nent, vbytes, rv, mpls;
    uint32 old_profile_idx;
    void * vtab, * vnull,  *vtabp;
    int vx_key_type;
    int vx_key_value;
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t handle;
    vlan_xlate_entry_t vent;
    uint32 key[2];
#endif
    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    uint32 ent_sz = sizeof(vlan_xlate_entry_t);
    
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        ent_sz = sizeof(vlan_xlate_1_double_entry_t);
    }

    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);
    vbytes = soc_mem_entry_words(unit, mem);
    vbytes = WORDS2BYTES(vbytes);
    vtab = soc_cm_salloc(unit, nent * ent_sz, "vlan_xlate");

    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vnull = soc_mem_entry_null(unit, mem);

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                        vlan_xlate_1_double_entry_t *, vtab, i);
        } else {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                                             vlan_xlate_entry_t *, vtab, i);
        }

#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            mpls = 0;
        } else
#endif
        {
            if (SOC_IS_TR_VL(unit) && (!(SOC_IS_HURRICANE(unit) || 
                SOC_IS_HURRICANE2(unit))) &&
                SOC_MEM_FIELD_VALID(unit, mem, MPLS_ACTIONf)) {
                mpls = soc_mem_field32_get(unit, mem, vtabp, MPLS_ACTIONf);
            } else {
                mpls = 0;
            }
        }

        vx_key_value = soc_mem_field32_get(unit, mem, vtabp, KEY_TYPEf);
        rv =  _bcm_esw_vlan_xlate_key_type_get(unit,vx_key_value,&vx_key_type);
        if (rv != BCM_E_NONE) {
            /* not valid key type, must be for extend entry */
            continue;
        }

        if (!soc_mem_field32_get(unit, mem, vtabp, vf) ||
            (mpls != 0) ||
            (vx_key_type == VLXLT_HASH_KEY_TYPE_VLAN_MAC) ||
            (vx_key_type == VLXLT_HASH_KEY_TYPE_LLVID) ||
            (vx_key_type == VLXLT_HASH_KEY_TYPE_LLVID_OVID) ||
            (vx_key_type == VLXLT_HASK_KEY_TYPE_LLVID_IVID)) {
            continue;
        }

        old_profile_idx = soc_mem_field32_get(unit, mem, vtabp,
                                                      TAG_ACTION_PROFILE_PTRf);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, i, vnull);

        if ((rv >= 0) && (old_profile_idx != ing_action_profile_def[unit])) {
            /* Delete the old vlan action profile entry */
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx);
        }

#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            (0 != soc_mem_field32_get(unit, mem, vtabp, VINTF_CTR_IDXf))) {
            sal_memset(&vent, 0, sizeof(vent));
            /* Construct key-only entry, copy to FS handle */
            soc_mem_field32_set(unit, mem, &vent, KEY_TYPEf,
                     soc_mem_field32_get(unit, mem, vtabp, KEY_TYPEf));
            soc_mem_field_get(unit, mem, (uint32 *) vtabp,
                               KEYf, (uint32 *) key);
            soc_mem_field_set(unit, mem, (uint32 *) &vent,
                               KEYf, (uint32 *) key);

            _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
            _BCM_FLEX_STAT_HANDLE_COPY(handle, vent);

            /* Release Service counter */
            _bcm_esw_flex_stat_ext_handle_free(unit, _bcmFlexStatTypeVxlt,
                                               handle);
        }
#endif
    }
    
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vtab);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate_action_delete_extd_tbl(unit));
    }
#endif
    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_add
 * Description   :
 *      Add a range of VLANs and an entry to ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (IN) Action for outer and inner tag
 */
int
_bcm_trx_vlan_translate_action_range_add(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    int i, key_type = 0, rv, gport_id, old_idx = 0, stm_idx = 0;
    uint32 new_idx = 0;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id, tid;
    bcm_vlan_t min_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t max_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t min_inner_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t max_inner_vlan[BCM_VLAN_RANGE_NUM];
    int port_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_module_t *mod_array = NULL;
    bcm_port_t   *port_array = NULL;
    int trunk128;
    soc_field_t field;
    int old_inner_idx = 0;
    uint32 new_inner_idx = 0;
#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    int pon_tunnel = FALSE;
    int nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;
#endif
    int dtrc_or_siv = 0;
    vlan_range_bk_t *old_range_bk_array = NULL;
    vlan_range_bk_t *new_range_bk_array = NULL;
    vlan_range_bk_ref_cnt_t *ref_cnt_array = NULL;

    /* Decvice supports double tag range compression, or
     * the config is for single inner vid.
     */
    dtrc_or_siv =
        (soc_feature(unit, soc_feature_vlan_double_tag_range_compress) ||
         (outer_vlan_low == BCM_VLAN_INVALID)) ? TRUE : FALSE;

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            key_type = bcmVlanTranslateKeyPortDouble;
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
        } else {
            if (inner_vlan_low == inner_vlan_high) {
                /* If hardware does not support double tag range compression,
                 * but inner_vlan_low is the same as inner_vlan_high.
                 * Set outer VLAN range but inner VLAN as a fixed value.
                 */
                key_type = bcmVlanTranslateKeyPortDouble;
                VLAN_CHK_ID(unit, inner_vlan_low);
                VLAN_CHK_ID(unit, inner_vlan_high);
                VLAN_CHK_ID(unit, outer_vlan_low);
                VLAN_CHK_ID(unit, outer_vlan_high);
            } else {
                /* For an incoming double tagged packet, hardware currently
                 * supports VLAN range matching for outer VLAN ID only, not
                 * for inner VLAN ID. Hence, outer VLAN range and inner VLAN
                 * range cannot be specified simultaneously.
                 */
                return BCM_E_PARAM;
            }
        }
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        VLAN_CHK_ID(unit, outer_vlan_low);
        VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        VLAN_CHK_ID(unit, inner_vlan_low);
        VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        return BCM_E_PARAM;
    }

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_lltag)) {
        if (BCM_GPORT_IS_VLAN_PORT(port)) {
            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
            BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY,
                        vp, &dvp));
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                    NEXT_HOP_INDEXf);
            BCM_IF_ERROR_RETURN (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                        nh_index, &ing_nh));

            if ((soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf)
                        == 0x3)) {
                pon_tunnel = TRUE;
            }
        }
    }

    if (pon_tunnel == TRUE) {
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
        } else if (outer_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
        }
    }
#endif

    if (action != NULL) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_verify(unit, action));
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 0, NULL,
                               &port_count));

        if (port_count <= 0) {
            return BCM_E_PORT;
        }

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * port_count,
                "trunk member array");
        if (member_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * port_count);

        rv = bcm_esw_trunk_get(unit, trunk_id, NULL, port_count,
                member_array, &port_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(mod_array, 0, sizeof(bcm_module_t) * port_count);

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(port_array, 0, sizeof(bcm_port_t) * port_count);

        for (i = 0; i < port_count; i++) {
            rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                    &mod_array[i], &port_array[i], &tid, &gport_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }

        port_count = 1;

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        mod_array[0] = mod_out;

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        port_array[0] = port_out;
    }

    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[0],
            port_array[0], &stm_idx);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }

    if (dtrc_or_siv) {
        /* For devices supporting vlan double tag range compression
         * and if inner_vlan_low is valid
         */
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
                field = INNER_VLAN_RANGE_IDXf;
            } else {
                field = VLAN_RANGE_IDXf;
            }
            old_inner_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                &stm_entry, field);

            /* Get the profile table entry for this port/trunk */
            _bcm_trx_vlan_range_profile_entry_get(unit,
                                                  min_inner_vlan,
                                                  max_inner_vlan,
                                                  old_inner_idx);

            /* Find the first unused min/max range. Unused ranges are
             * identified by { min == 1, max == 0 }
             */
            for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                if ((min_inner_vlan[i] == 1) && (max_inner_vlan[i] == 0)) {
                    break;
                } else if (min_inner_vlan[i] == inner_vlan_low) {
                    /* Can't have multiple ranges with the same min */
                    rv = BCM_E_EXISTS;
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }
            if (i == BCM_VLAN_RANGE_NUM) {
                /* All ranges are taken */
                rv = BCM_E_FULL;
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }

            /* Insert the new range into the table entry sorted by min VID */
            for ( ; i > 0 ; i--) {
                if (min_inner_vlan[i - 1] > inner_vlan_low) {
                    /* Move existing min/max down */
                    min_inner_vlan[i] = min_inner_vlan[i - 1];
                    max_inner_vlan[i] = max_inner_vlan[i - 1];
                } else {
                    break;
                }
            }
            min_inner_vlan[i] = inner_vlan_low;
            max_inner_vlan[i] = inner_vlan_high;

            /* Adding the new profile table entry and update profile pointer */
            for (i = 0; i < port_count; i++) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit,
                                                           min_inner_vlan,
                                                           max_inner_vlan,
                                                           &new_inner_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[i],
                                                           port_array[i],
                                                           &stm_idx);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                stm_idx, field, new_inner_idx);
                }

                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_inner_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }

            /* Bookkepping for range without adding entry to XLATE table */
            if (action == NULL) {

                old_range_bk_array =
                    &(vlan_info[unit].vlan_range_inner_no_act_array[old_inner_idx]);
                new_range_bk_array =
                    &(vlan_info[unit].vlan_range_inner_no_act_array[new_inner_idx]);

                /* Point to the relevant ref count array */
                ref_cnt_array = vlan_info[unit].vlan_range_inner_no_act_ref_cnt_array;

                /* If ref count of new index is 0, copy the old index array */
                if (ref_cnt_array[new_inner_idx].ref_cnt == 0) {
                    /* Copy vlan_low from old_inner_idx to new_inner_idx */
                    sal_memcpy(new_range_bk_array->vlan_low,
                               old_range_bk_array->vlan_low,
                               sizeof(bcm_vlan_t) * BCM_VLAN_RANGE_NUM);

                    /* Insert new bookkeepping vlan_low */
                    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                        if (new_range_bk_array->vlan_low[i] == BCM_VLAN_INVALID) {
                            new_range_bk_array->vlan_low[i] = inner_vlan_low;
                            break;
                        }
                    }
                    if (i == BCM_VLAN_RANGE_NUM) {
                        /* All ranges are taken */
                        rv = BCM_E_FULL;
                        soc_mem_unlock(unit, ING_VLAN_RANGEm);
                        goto cleanup;
                    }
                }

                /* Update the ref count for the new index */
                ref_cnt_array[new_inner_idx].ref_cnt += 1;

                /* Update the ref count for the old index */
                if (ref_cnt_array[old_inner_idx].ref_cnt > 0) {
                    ref_cnt_array[old_inner_idx].ref_cnt -= 1;
                }

                /* Clear up the old profile index if no longer in use */
                if (ref_cnt_array[old_inner_idx].ref_cnt == 0) {
                    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                        old_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                    }
                }
            }
        }
    }

    /* For devices supporting vlan double tag range compression
     * and entry is for outer tag or double tag vlan range compression
     * OR
     * Other devices not supporting double tag range compression feature
     */
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        field = VLAN_RANGE_IDXf;
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, 
                              OUTER_VLAN_RANGE_IDXf)) {  
                field = OUTER_VLAN_RANGE_IDXf;
            }
        }

        old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                  &stm_entry, field);

        /* Get the profile table entry for this port/trunk */
        _bcm_trx_vlan_range_profile_entry_get(unit,
                                          min_vlan, max_vlan, old_idx);
        
        /* Find the first unused min/max range. Unused ranges are
         * identified by { min == 1, max == 0 }
         */
        for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
            if ((min_vlan[i] == 1) && (max_vlan[i] == 0)) {
                break;
            } else if (min_vlan[i] == outer_vlan_low) {
                /* Can't have multiple ranges with the same min */
                rv = BCM_E_EXISTS;
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
        }
        if (i == BCM_VLAN_RANGE_NUM) {
            /* All ranges are taken */
            rv = BCM_E_FULL;
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
       
        /* Insert the new range into the table entry sorted by min VID */
        for ( ; i > 0 ; i--) {
            if (min_vlan[i - 1] > outer_vlan_low) {
                /* Move existing min/max down */
                min_vlan[i] = min_vlan[i - 1];
                max_vlan[i] = max_vlan[i - 1];
            } else {
                break;
            }
        }
        min_vlan[i] = outer_vlan_low;
        max_vlan[i] = outer_vlan_high;
        
        /* Adding the new profile table entry and update profile pointer */
        for (i = 0; i < port_count; i++) {
            rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan,
                    &new_idx);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
            rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[i],
                    port_array[i], &stm_idx);
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_field32_modify(unit,
                        SOURCE_TRUNK_MAP_TABLEm, stm_idx, field, new_idx);
            }
            if (BCM_FAILURE(rv)) {
                _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
        }

        /* Bookkepping for range without adding entry to XLATE table */
        if (action == NULL) {
            old_range_bk_array =
                &(vlan_info[unit].vlan_range_no_act_array[old_idx]);
            new_range_bk_array =
                &(vlan_info[unit].vlan_range_no_act_array[new_idx]);

            /* Point to the relevant ref count array */
            ref_cnt_array = vlan_info[unit].vlan_range_no_act_ref_cnt_array;

            /* If ref count of new index is 0, copy the old index array */
            if (ref_cnt_array[new_idx].ref_cnt == 0) {
                sal_memcpy(new_range_bk_array->vlan_low,
                           old_range_bk_array->vlan_low,
                           sizeof(bcm_vlan_t) * BCM_VLAN_RANGE_NUM);

                /* Insert new bookkeepping vlan_low */
                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    if (new_range_bk_array->vlan_low[i] == BCM_VLAN_INVALID) {
                        new_range_bk_array->vlan_low[i] = outer_vlan_low;
                        break;
                    }
                }
                if (i == BCM_VLAN_RANGE_NUM) {
                    /* All ranges are taken */
                    rv = BCM_E_FULL;
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }

            /* Update the ref count for the new index */
            ref_cnt_array[new_idx].ref_cnt += 1;

            /* Update the ref count for the old index */
            if (ref_cnt_array[old_idx].ref_cnt > 0) {
                ref_cnt_array[old_idx].ref_cnt -= 1;
            }

            /* Clear up the old profile index if no longer in use */
            if (ref_cnt_array[old_idx].ref_cnt == 0) {
                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    old_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                }
            }
        }
    }

    /* If action is NULL, do not add entry to vlan translate table */
    if (action != NULL) {
        /* Add an entry in the vlan translate table for the low VID */
        rv = _bcm_trx_vlan_translate_action_add(unit, port, key_type,
                                                outer_vlan_low, inner_vlan_low,
                                                action);

        if (rv != BCM_E_NONE) {
            for (i = 0; i < port_count; i++) {
                if (outer_vlan_low != BCM_VLAN_INVALID) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                }
                if (dtrc_or_siv) {
                    if (inner_vlan_low != BCM_VLAN_INVALID) {
                        _bcm_trx_vlan_range_profile_entry_delete(unit, new_inner_idx);
                    }
                }
            }
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
    }

    /* Delete the old profile entry */
    for (i = 0; i < port_count; i++) {
        if (outer_vlan_low != BCM_VLAN_INVALID) {
            rv = _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
        }
        if (dtrc_or_siv) {
            if (inner_vlan_low != BCM_VLAN_INVALID) {
                rv = _bcm_trx_vlan_range_profile_entry_delete(unit,
                                                              old_inner_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }
        }
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 1);
            if (!trunk128) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit,
                        min_vlan, max_vlan, &new_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = soc_mem_field32_modify(unit,
                        TRUNK32_PORT_TABLEm, trunk_id,
                        VLAN_RANGE_IDXf, new_idx);
                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                if (outer_vlan_low != BCM_VLAN_INVALID) {
                    rv = _bcm_trx_vlan_range_profile_entry_delete(unit,
                            old_idx);
                    if (BCM_FAILURE(rv)) {
                        soc_mem_unlock(unit, ING_VLAN_RANGEm);
                        goto cleanup;
                    }
                }
                if (dtrc_or_siv) {
                    if (inner_vlan_low != BCM_VLAN_INVALID) {
                        rv = _bcm_trx_vlan_range_profile_entry_delete(
                                unit,
                                old_inner_idx);
                        if (BCM_FAILURE(rv)) {
                            soc_mem_unlock(unit, ING_VLAN_RANGEm);
                            goto cleanup;
                        }
                    }
                }
            }
        }
    }
    
    soc_mem_unlock(unit, ING_VLAN_RANGEm);

cleanup:
    if (NULL != member_array) {
        sal_free(member_array);
    }
    if (NULL != mod_array) {
        sal_free(mod_array);
    }
    if (NULL != port_array) {
        sal_free(port_array);
    }

    return rv;
}

/*
 * Function   :
 *      _bcm_trx_vlan_range_profile_index_get
 * Description   :
 *      Get the index for profile entry in the ING_VLAN_RANGE memory.
 * Parameters   :
 *      unit        (IN) BCM unit number
 *      gport       (IN) Ingress generic port
 *      profile_idx (OUT) index to profile entry
 */
int 
_bcm_trx_vlan_range_profile_index_get(int unit, bcm_gport_t gport, 
                                     int *profile_idx)
{
    int                             idx = 0, gport_id = 0;
    bcm_port_t                      port_out;
    bcm_module_t                    mod_out;
    bcm_trunk_t                     trunk_id;
    int                             stm_idx;     /* Source Trunk Map index */
    source_trunk_map_table_entry_t  stm_entry;
    bcm_trunk_member_t              trunk_member;
    int                             port_count;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(gport)) {
        if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 1, &trunk_member,
                                      &port_count));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, trunk_member.gport,
                                    &mod_out, &port_out, &trunk_id, &gport_id));
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, mod_out,
            port_out, &stm_idx));

    BCM_IF_ERROR_RETURN(
       READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry));

    if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                          OUTER_VLAN_RANGE_IDXf)) {
        idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                              &stm_entry, OUTER_VLAN_RANGE_IDXf);
    } else {
        idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                              &stm_entry, VLAN_RANGE_IDXf);
    }
        *profile_idx = idx;
    return BCM_E_NONE;
    }

/*
 * Function   :
 *      _bcm_trx_vlan_range_profile_inner_index_get
 * Description   :
 *      Get the inner index for profile entry in the ING_VLAN_RANGE memory.
 * Parameters   :
 *      unit        (IN) BCM unit number
 *      gport       (IN) Ingress generic port
 *      profile_idx (OUT) inner vlan range index to profile entry
 */
int 
_bcm_trx_vlan_range_profile_inner_index_get(int unit, bcm_gport_t gport, 
                                     int *profile_idx)
{
    int                             idx = 0, gport_id = 0;
    bcm_port_t                      port_out;
    bcm_module_t                    mod_out;
    bcm_trunk_t                     trunk_id;
    int                             stm_idx;     /* Source Trunk Map index */
    source_trunk_map_table_entry_t  stm_entry;
    bcm_trunk_info_t                tinfo;
    bcm_trunk_member_t              trunk_member;
    int                             port_count;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(gport)) {
        if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }
        BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, trunk_id, &tinfo, 1,
                    &trunk_member, &port_count));
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, trunk_member.gport,
                    &mod_out, &port_out, &trunk_id, &gport_id));
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_src_mod_port_table_index_get(unit, mod_out,
            port_out, &stm_idx));

    BCM_IF_ERROR_RETURN(
       READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry));

    if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                        INNER_VLAN_RANGE_IDXf)) {
        idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                          &stm_entry, INNER_VLAN_RANGE_IDXf);
        *profile_idx = idx;
    }
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_get
 * Description   :
 *      Get a range of VLANs and an entry to ingress VLAN translation table.
 *      for Triumph
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 *      action          (OUT) Action for outer and inner tag
 */
int
_bcm_trx_vlan_translate_action_range_get(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,
                                        bcm_vlan_action_set_t *action)
{
    int         key_type = 0, profile_idx, i;
    bcm_vlan_t  min_vlan[BCM_VLAN_RANGE_NUM], max_vlan[BCM_VLAN_RANGE_NUM];
    int rv;
#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    int pon_tunnel = FALSE;
    int nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;
#endif
    
    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            key_type = bcmVlanTranslateKeyPortDouble;
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
        } else {
            /* For an incoming double tagged packet, hardware currently supports
             * VLAN range matching for outer VLAN ID only, not for inner VLAN ID
             * Hence, outer VLAN range and inner VLAN range cannot be specified
             * simultaneously.
             */
            return BCM_E_PARAM;
        }
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        VLAN_CHK_ID(unit, outer_vlan_low);
        VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        VLAN_CHK_ID(unit, inner_vlan_low);
        VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        return BCM_E_PARAM;
    }

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_lltag)) {
        if (BCM_GPORT_IS_VLAN_PORT(port)) {
            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
            BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY,
                        vp, &dvp));
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                    NEXT_HOP_INDEXf);
            BCM_IF_ERROR_RETURN (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                        nh_index, &ing_nh));

            if ((soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf)
                        == 0x3)) {
                pon_tunnel = TRUE;
            }
        }
    }

    if (pon_tunnel == TRUE) {
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
        } else if (outer_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
        }
    }
#endif

    /* For devices supporting vlan double tag range compression
     * and if inner_vlan_low is valid
     */
    if (inner_vlan_low != BCM_VLAN_INVALID) {
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            rv = _bcm_trx_vlan_range_profile_inner_index_get(unit,
                port, &profile_idx);
        } else {
            rv = _bcm_trx_vlan_range_profile_index_get(unit, port, &profile_idx);
        }
        BCM_IF_ERROR_RETURN(rv);

        /* Get the profile table entry for this port/trunk */
        _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, 
                max_vlan, profile_idx);
        /* Find range match */
        for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
            if ((min_vlan[i] == inner_vlan_low) &&
                (max_vlan[i] == inner_vlan_high)) {
                break;
            }
        }
        if (i == 8) {
            return BCM_E_NOT_FOUND;
        }
    }

    /* For devices supporting vlan double tag range compression
     * and entry is for outer tag or double tag range compression
     * OR
     * Other devices not supporting double tag range compression feature 
     */
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        rv = _bcm_trx_vlan_range_profile_index_get(unit, port, &profile_idx);
        BCM_IF_ERROR_RETURN(rv);
        
        /* Get the profile table entry for this port/trunk */
        _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, 
                max_vlan, profile_idx);
        /* Find range match */
        for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
            if ((min_vlan[i] == outer_vlan_low) &&
                (max_vlan[i] == outer_vlan_high)) {
                break;
            }
        }
        if (i == BCM_VLAN_RANGE_NUM) {
            return BCM_E_NOT_FOUND;
        }
    }
    /* Get an entry in the vlan translate table for the low VID */
    rv = _bcm_trx_vlan_translate_action_get(unit,port, key_type,
                                outer_vlan_low, inner_vlan_low, action);  
    if (rv == BCM_E_NOT_FOUND) {
        int mod_out;
        int port_out;
        int trunk_id;
        int gport_id;
        int    stm_idx = 0;
        source_trunk_map_table_entry_t  stm_entry;
        int port_type;
        soc_mem_t mem = VLAN_XLATEm;
        soc_field_t tgid_fld = TGIDf;
        if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
            mem = VLAN_XLATE_1_DOUBLEm;
        }

        /* check if the port belongs to a trunk group and whether 
         * a xlat entry for that TGID exist
         */
        if (SOC_MEM_FIELD_VALID(unit, mem, Tf)) {
            if (BCM_GPORT_IS_MODPORT(port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_gport_resolve(unit, port, &mod_out, 
                          &port_out, &trunk_id, &gport_id));

                /* Get index to source trunk map table */
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_src_mod_port_table_index_get(unit, 
                        mod_out, port_out, &stm_idx));

                /* read the entry */
                BCM_IF_ERROR_RETURN(
                   READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, 
                                stm_idx, &stm_entry));

                if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                                        SRC_TGIDf)) {
                    tgid_fld = SRC_TGIDf;
                }

                trunk_id = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                         &stm_entry, tgid_fld);
                port_type = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                         &stm_entry, PORT_TYPEf);

                if (port_type == 1) {
                    BCM_GPORT_TRUNK_SET(gport_id, trunk_id);
                    rv = _bcm_trx_vlan_translate_action_get(unit,gport_id,
                         key_type,outer_vlan_low, inner_vlan_low, action);
                }
            } 
        }
    } 
    return (rv);
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_delete
 * Description   :
 *      Delete a range of VLANs and an entry from ingress VLAN translation table
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Ingress generic port
 *      outer_vlan_low  (IN) Packet outer VLAN ID low 
 *      outer_vlan_high (IN) Packet outer VLAN ID high
 *      inner_vlan_low  (IN) Packet inner VLAN ID low 
 *      inner_vlan_high (IN) Packet inner VLAN ID high
 */
int
_bcm_trx_vlan_translate_action_range_delete(int unit,
                                           bcm_gport_t port,
                                           bcm_vlan_t outer_vlan_low,
                                           bcm_vlan_t outer_vlan_high,
                                           bcm_vlan_t inner_vlan_low,
                                           bcm_vlan_t inner_vlan_high)
{
    int i, key_type = 0, rv, gport_id, old_idx = 0, stm_idx = 0;
    uint32 new_idx = 0;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id, tid;
    bcm_vlan_t min_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t max_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t min_inner_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t max_inner_vlan[BCM_VLAN_RANGE_NUM];
    int port_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_module_t *mod_array = NULL;
    bcm_port_t   *port_array = NULL;
    int trunk128;
    soc_field_t field;
    int old_inner_idx = 0;
    uint32 new_inner_idx = 0;
#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    int pon_tunnel = FALSE;
    int nh_index, vp;
    ing_l3_next_hop_entry_t ing_nh;
    ing_dvp_table_entry_t dvp;
#endif
    int dtrc_or_siv = 0;
    int update_range_only = FALSE;
    vlan_range_bk_t *old_range_bk_array = NULL;
    vlan_range_bk_t *new_range_bk_array = NULL;
    vlan_range_bk_ref_cnt_t *ref_cnt_array = NULL;

    /* Decvice supports double tag range compression, or
     * the config is for single inner vid.
     */
    dtrc_or_siv =
        (soc_feature(unit, soc_feature_vlan_double_tag_range_compress) ||
         (outer_vlan_low == BCM_VLAN_INVALID)) ? TRUE : FALSE;

    if ((outer_vlan_low != BCM_VLAN_INVALID) &&
        (inner_vlan_low != BCM_VLAN_INVALID)) {
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            key_type = bcmVlanTranslateKeyPortDouble;
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
        } else {
            if (inner_vlan_low == inner_vlan_high) {
                /* If hardware does not support double tag range compression,
                 * but inner_vlan_low is the same as inner_vlan_high.
                 * Set outer VLAN range but inner VLAN as a fixed value.
                 */
                key_type = bcmVlanTranslateKeyPortDouble;
                VLAN_CHK_ID(unit, inner_vlan_low);
                VLAN_CHK_ID(unit, inner_vlan_high);
                VLAN_CHK_ID(unit, outer_vlan_low);
                VLAN_CHK_ID(unit, outer_vlan_high);
            } else {
                /* For an incoming double tagged packet, hardware currently
                 * supports VLAN range matching for outer VLAN ID only, not
                 * for inner VLAN ID. Hence, outer VLAN range and inner VLAN
                 * range cannot be specified simultaneously.
                 */
                return BCM_E_PARAM;
            }
        }
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
        VLAN_CHK_ID(unit, outer_vlan_low);
        VLAN_CHK_ID(unit, outer_vlan_high);
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
        VLAN_CHK_ID(unit, inner_vlan_low);
        VLAN_CHK_ID(unit, inner_vlan_high);
    } else {
        return BCM_E_PARAM;
    }

#if defined(BCM_KATANA2_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_lltag)) {
        if (BCM_GPORT_IS_VLAN_PORT(port)) {
            vp = BCM_GPORT_VLAN_PORT_ID_GET(port);
            if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeVlan)) {
                return BCM_E_NOT_FOUND;
            }
            BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY,
                        vp, &dvp));
            nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                    NEXT_HOP_INDEXf);
            BCM_IF_ERROR_RETURN (READ_ING_L3_NEXT_HOPm(unit, MEM_BLOCK_ANY,
                        nh_index, &ing_nh));

            if ((soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf)
                        == 0x3)) {
                pon_tunnel = TRUE;
            }
        }
    }

    if (pon_tunnel == TRUE) {
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelInner;
            VLAN_CHK_ID(unit, inner_vlan_low);
            VLAN_CHK_ID(unit, inner_vlan_high);
        } else if (outer_vlan_low != BCM_VLAN_INVALID) {
            key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
            VLAN_CHK_ID(unit, outer_vlan_low);
            VLAN_CHK_ID(unit, outer_vlan_high);
        }
    }
#endif

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
       if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 0, NULL, &port_count));

        if (port_count > 0) {
        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * port_count,
                "trunk member array");
        if (member_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * port_count);

        rv = bcm_esw_trunk_get(unit, trunk_id, NULL, port_count,
                member_array, &port_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(mod_array, 0, sizeof(bcm_module_t) * port_count);

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(port_array, 0, sizeof(bcm_port_t) * port_count);
        }

        for (i = 0; i < port_count; i++) {
            rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                    &mod_array[i], &port_array[i], &tid, &gport_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }

        port_count = 1;

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        mod_array[0] = mod_out;

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        port_array[0] = port_out;
    }

    if (port_count > 0) {
    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[0],
            port_array[0], &stm_idx);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);
    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }

    if (dtrc_or_siv) {
        /* If vlan double tag range compression feature is supported and
         * entry type is for inner tag range translation */
        if (inner_vlan_low != BCM_VLAN_INVALID) {
            if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
                field = INNER_VLAN_RANGE_IDXf;
            } else {
                field = VLAN_RANGE_IDXf;
            }

            old_inner_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                &stm_entry, field);

            /* Get the profile table entry for this port/trunk */
            _bcm_trx_vlan_range_profile_entry_get(unit,
                                                  min_inner_vlan,
                                                  max_inner_vlan,
                                                  old_inner_idx);

            /* Find the min/max range. */
            for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                if ((min_inner_vlan[i] == inner_vlan_low) &&
                    (max_inner_vlan[i] == inner_vlan_high)) {
                    break;
                }
            }
            if (i == BCM_VLAN_RANGE_NUM) {
                rv = BCM_E_NOT_FOUND;
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }

            /* Remove the range from the table entry and fill in the gap */
            for ( ; i < BCM_VLAN_RANGE_NUM - 1; i++) {
                /* Move existing min/max UP */
                min_inner_vlan[i] = min_inner_vlan[i + 1];
                max_inner_vlan[i] = max_inner_vlan[i + 1];
            }
            /* Mark last min/max range as unused. Unused ranges are
             * identified by { min == 1, max == 0 }
             */
            min_inner_vlan[i] = 1;
            max_inner_vlan[i] = 0;

            /* Adding the new profile table entry and update profile pointer */
            for (i = 0; i < port_count; i++) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit,
                                                           min_inner_vlan,
                                                           max_inner_vlan,
                                                           &new_inner_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[i],
                                                           port_array[i],
                                                           &stm_idx);
                if (BCM_SUCCESS(rv)) {
                    rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                stm_idx, field, new_inner_idx);
                }

                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_inner_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }

            ref_cnt_array = vlan_info[unit].vlan_range_inner_no_act_ref_cnt_array;

            /* If the old_inner_idx is non-zero, adjust the internal SW
               struct used for no-action range tracking */
            if (ref_cnt_array[old_inner_idx].ref_cnt != 0) {

                /* Checks whether the range is added without adding entry to
                 * vlan XLATE table.
                 */

                old_range_bk_array =
                    &(vlan_info[unit].vlan_range_inner_no_act_array[old_inner_idx]);
                new_range_bk_array =
                    &(vlan_info[unit].vlan_range_inner_no_act_array[new_inner_idx]);

                sal_memcpy(new_range_bk_array->vlan_low,
                           old_range_bk_array->vlan_low,
                           sizeof(bcm_vlan_t) * BCM_VLAN_RANGE_NUM);

                /* Update the ref count for the old index */
                ref_cnt_array[old_inner_idx].ref_cnt -= 1;

                /* Update the ref count for the new index */
                ref_cnt_array[new_inner_idx].ref_cnt += 1;

                /* Clear up the old profile index if no longer in use */
                if (ref_cnt_array[old_inner_idx].ref_cnt == 0) {
                    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                        old_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                    }
                }

                update_range_only = FALSE;
                /* Find and invalidate the specified vlan_low if it exists */
                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    if (new_range_bk_array->vlan_low[i] == inner_vlan_low) {
                        new_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                        update_range_only = TRUE;
                        break;
                    }
                }
            }
        }
    }

    /* If vlan double tag range compression feature is supported and
     * entry type is for double tag range compression */
    if (outer_vlan_low != BCM_VLAN_INVALID) {
        field = VLAN_RANGE_IDXf;
        if (soc_feature (unit, soc_feature_vlan_double_tag_range_compress)) {
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm, 
                              OUTER_VLAN_RANGE_IDXf)) {  
                field = OUTER_VLAN_RANGE_IDXf;
            }
        }

        old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                &stm_entry, field);

        /* Get the profile table entry for this port/trunk */
        _bcm_trx_vlan_range_profile_entry_get(unit,
                                              min_vlan, max_vlan, old_idx);


        /* Find the min/max range. */
        for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
            if ((min_vlan[i] == outer_vlan_low) &&
                (max_vlan[i] == outer_vlan_high)) {
                        break;
            }
        }
        if (i == BCM_VLAN_RANGE_NUM) {
            rv = BCM_E_NOT_FOUND;
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }

        /* Remove the range from the table entry and fill in the gap */
        for ( ; i < BCM_VLAN_RANGE_NUM - 1; i++) {
            /* Move existing min/max UP */
            min_vlan[i] = min_vlan[i + 1];
            max_vlan[i] = max_vlan[i + 1];
        }
        /* Mark last min/max range as unused. Unused ranges are
         * identified by { min == 1, max == 0 }
         */
        min_vlan[i] = 1;
        max_vlan[i] = 0;

        /* Adding the new profile table entry and update profile pointer */
        for (i = 0; i < port_count; i++) {
            rv = _bcm_trx_vlan_range_profile_entry_add(unit,
                    min_vlan, max_vlan, &new_idx);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
            rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[i],
                    port_array[i], &stm_idx);
            if (BCM_SUCCESS(rv)) {
                rv = soc_mem_field32_modify(unit,
                        SOURCE_TRUNK_MAP_TABLEm, stm_idx,
                        field, new_idx);
            }
            if (BCM_FAILURE(rv)) {
                _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
        }

        ref_cnt_array = vlan_info[unit].vlan_range_no_act_ref_cnt_array;

        /* If the old_idx is non-zero, adjust the internal SW
           struct used for no-action range tracking */
        if (ref_cnt_array[old_idx].ref_cnt != 0) {

            /* Checks whether the range is added without adding entry to
             * vlan XLATE table.
             */
            old_range_bk_array =
                &(vlan_info[unit].vlan_range_no_act_array[old_idx]);
            new_range_bk_array =
                &(vlan_info[unit].vlan_range_no_act_array[new_idx]);

            sal_memcpy(new_range_bk_array->vlan_low,
                       old_range_bk_array->vlan_low,
                       sizeof(bcm_vlan_t) * BCM_VLAN_RANGE_NUM);

            /* Update the ref count for the old index */
            ref_cnt_array[old_idx].ref_cnt -= 1;

            /* Update the ref count for the new index */
            ref_cnt_array[new_idx].ref_cnt += 1;

            /* Clear up the old profile index if no longer in use */
            if (ref_cnt_array[old_idx].ref_cnt == 0) {
                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    old_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                }
            }

           /* Find and invalidate the specified vlan_low if it exists */
            update_range_only = FALSE;
            for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                if (new_range_bk_array->vlan_low[i] == outer_vlan_low) {
                    new_range_bk_array->vlan_low[i] = BCM_VLAN_INVALID;
                    update_range_only = TRUE;
                    break;
                }
            }
        }
    }
    }

    /* Delete the entry from the vlan translate table for the low VID */
    if (!update_range_only) {
        rv = _bcm_trx_vlan_translate_action_delete(unit, port, key_type,
                                                   outer_vlan_low,
                                                   inner_vlan_low);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)){
            for (i = 0; i < port_count; i++) {
                if (outer_vlan_low != BCM_VLAN_INVALID) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                }
                if (dtrc_or_siv) {
                    if (inner_vlan_low != BCM_VLAN_INVALID) {
                        _bcm_trx_vlan_range_profile_entry_delete(
                            unit,
                            new_inner_idx);
                    }
                }
            }
            if (port_count > 0) {
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            }
            goto cleanup;
        }
    }

    /* Delete the old profile entry */
    for (i = 0; i < port_count; i++) {
        if (outer_vlan_low != BCM_VLAN_INVALID) {
            _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
        }
        if (dtrc_or_siv) {
            if (inner_vlan_low != BCM_VLAN_INVALID) {
                _bcm_trx_vlan_range_profile_entry_delete(unit, old_inner_idx);
            }
        }
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 1);
            if (!trunk128) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit,
                        min_vlan, max_vlan, &new_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = soc_mem_field32_modify(unit, TRUNK32_PORT_TABLEm, trunk_id,
                        VLAN_RANGE_IDXf, new_idx);
                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                if (outer_vlan_low != BCM_VLAN_INVALID) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
                }
                if (dtrc_or_siv) {
                    if (inner_vlan_low != BCM_VLAN_INVALID) {
                        _bcm_trx_vlan_range_profile_entry_delete(unit, old_inner_idx);
                    }
                }
            }
        }
    }
    if (port_count > 0) {
    soc_mem_unlock(unit, ING_VLAN_RANGEm);
    }
    
cleanup:
    if (NULL != member_array) {
        sal_free(member_array);
    }
    if (NULL != mod_array) {
        sal_free(mod_array);
    }
    if (NULL != port_array) {
        sal_free(port_array);
    }

    return rv;
}

/*
 * Function   :
 *      bcm_vlan_translate_action_range_delete_all
 * Description   :
 *      Delete all ranges of VLANs and entries from ingress 
 *      VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 */
int
_bcm_trx_vlan_translate_action_range_delete_all(int unit)
{
    int rv;

    rv = _bcm_trx_vlan_range_profile_init(unit);
    if (rv == BCM_E_NONE) {
        rv = _bcm_trx_vlan_translate_action_delete_all(unit);
    }
    return rv;
}

/*
 * Function:
 *     _bcm_trx_vlan_translate_stm_index_to_gport
 * Description:
 *      Translate index of SOURCE_TRUNK_MAP_TABLE into gport
 * Parameters:
 *      unit         device number
 *      stm_index    Index to SOURCE_TRUNK_MAP_TABLE
 *      gport       (OUT) gport
 * Return:
 *     BCM_E_XXX
 */
STATIC int  
_bcm_trx_vlan_translate_stm_index_to_gport(int unit, int stm_index, 
                                          bcm_gport_t *gport)
{
    bcm_module_t    modid;
    bcm_port_t      port;
    _bcm_gport_dest_t   gport_dest;

    if (NULL == gport) {
        return BCM_E_PARAM;
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_HURRICANE3_SUPPORT)
    if (soc_feature(unit, soc_feature_src_modid_base_index)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_src_modid_port_get(unit, stm_index, 
                                                 &modid,&port));
    } else
#endif
    {
        modid = stm_index / (SOC_PORT_ADDR_MAX(unit) + 1) ; 
        port = stm_index - modid * (SOC_PORT_ADDR_MAX(unit) + 1);
    }

    if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_ADDRESSABLE(unit, port)) {
        return BCM_E_PARAM;
    }

    gport_dest.port = port;
    gport_dest.modid = modid;
    gport_dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &gport_dest, gport));

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_action_range_traverse
 * Description   :
 *      Traverses over range of ingress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      trvs_info       (IN/OUT) Traverse Structure to opertae on
 */
int
_bcm_trx_vlan_translate_action_range_traverse(int unit, 
                                    _bcm_vlan_translate_traverse_t *trvs_info)
{
    /* Indexes to iterate over memories, chunks and entries */
    int                             chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int                             buf_size, chunksize, chnk_end;
    /* Buffer to store chunks of memory table we currently work on */
    uint32                          *stm_tbl_chnk;
    source_trunk_map_table_entry_t  *stm_ent;
    soc_mem_t                       mem;
    int                             stop, rv = BCM_E_NONE;
    /* Index to point to table of vlan ranges. */
    int                             range_idx, stm_index, i;
    bcm_gport_t                     gport;
    bcm_vlan_t                      min_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t                      max_vlan[BCM_VLAN_RANGE_NUM];
    soc_field_t                     field;
    bcm_vlan_t                      min_inner_vlan[BCM_VLAN_RANGE_NUM];
    bcm_vlan_t                      max_inner_vlan[BCM_VLAN_RANGE_NUM];
    int                             range_inner_idx, j;

    mem = SOURCE_TRUNK_MAP_TABLEm;
    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_VLANDELETE_CHUNKS,
                                 VLAN_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    stm_tbl_chnk = soc_cm_salloc(unit, buf_size, 
                                 "vlan translate range traverse");
    if (NULL == stm_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)stm_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) <= mem_idx_max) ? 
            chnk_idx + chunksize - 1: mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, stm_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);

        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    OUTER_VLAN_RANGE_IDXf)) {
            field = OUTER_VLAN_RANGE_IDXf;
        } else {
            field = VLAN_RANGE_IDXf;
        }

        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            stm_ent = soc_mem_table_idx_to_pointer(unit, mem, 
                                             source_trunk_map_table_entry_t *, 
                                             stm_tbl_chnk, ent_idx);

            if (soc_feature(unit, soc_feature_vlan_double_tag_range_compress)) {
                range_idx = soc_mem_field32_get(unit, mem, stm_ent, field);
                _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan,
                        range_idx);

                if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                            INNER_VLAN_RANGE_IDXf)) {
                    range_inner_idx = soc_mem_field32_get(unit, mem, stm_ent, 
                            INNER_VLAN_RANGE_IDXf);
                    _bcm_trx_vlan_range_profile_entry_get(unit, min_inner_vlan,
                            max_inner_vlan, range_inner_idx);
                    if ((1 == min_vlan[0]) && (0 == max_vlan[0]) &&
                        (1 == min_inner_vlan[0]) && (0 == max_inner_vlan[0])) {
                        continue;
                    }
                } else {
                    if ((1 == min_vlan[0]) && (0 == max_vlan[0])) {
                        continue;
                    }
                }
                stm_index = chnk_idx + ent_idx; 
                rv = _bcm_trx_vlan_translate_stm_index_to_gport(unit, stm_index,
                        &gport);
                if (BCM_FAILURE(rv)) {
                    break;
                }
                trvs_info->gport = gport;

                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    if ((1 == min_vlan[i]) && (0 == max_vlan[i])) {
                        continue;
                    }
                    trvs_info->inner_vlan = BCM_VLAN_INVALID;
                    trvs_info->inner_vlan_high = BCM_VLAN_INVALID;
                    trvs_info->outer_vlan = BCM_VLAN_INVALID;
                    trvs_info->outer_vlan_high = BCM_VLAN_INVALID;
                    /* check if entry key_type is 
                       bcmVlanTranslateKeyPortOuter */
                    rv = _bcm_trx_vlan_translate_action_range_get(
                            unit, gport,
                            min_vlan[i], max_vlan[i],
                            BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                            trvs_info->action);
                    if (BCM_SUCCESS (rv)) {
                        trvs_info->key_type = bcmVlanTranslateKeyPortOuter;
                        trvs_info->outer_vlan = min_vlan[i];
                        trvs_info->outer_vlan_high = max_vlan[i];
                        rv = trvs_info->int_cb(unit, trvs_info, &stop);
                        if (BCM_FAILURE(rv) || TRUE == stop) {
                            break;
                        }
                    } else {
                        if (BCM_E_NOT_FOUND == rv) {
                            if (SOC_MEM_FIELD_VALID(unit,
                                        SOURCE_TRUNK_MAP_TABLEm,
                                        INNER_VLAN_RANGE_IDXf)) {
                                for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                                    if ((1 == min_inner_vlan[j]) && 
                                        (0 == max_inner_vlan[j])) {
                                        continue;
                                    }
                                    trvs_info->inner_vlan = BCM_VLAN_INVALID;
                                    trvs_info->inner_vlan_high =
                                        BCM_VLAN_INVALID;
                                    trvs_info->outer_vlan = BCM_VLAN_INVALID;
                                    trvs_info->outer_vlan_high =
                                        BCM_VLAN_INVALID;
                                    /* check if entry key_type is 
                                     * bcmVlanTranslateKeyPortDouble */
                                    rv =
                                    _bcm_trx_vlan_translate_action_range_get(
                                            unit, gport,
                                            min_vlan[i], max_vlan[i],
                                            min_inner_vlan[j],
                                            max_inner_vlan[j],
                                            trvs_info->action);
                                    if (BCM_SUCCESS (rv)) {
                                        trvs_info->key_type =
                                            bcmVlanTranslateKeyPortDouble;
                                        trvs_info->outer_vlan = min_vlan[i];
                                        trvs_info->outer_vlan_high =
                                            max_vlan[i];
                                        trvs_info->inner_vlan =
                                            min_inner_vlan[j];
                                        trvs_info->inner_vlan_high =
                                            max_inner_vlan[j];
                                        rv = trvs_info->int_cb(unit,
                                                trvs_info, &stop);
                                        if (BCM_FAILURE(rv) || TRUE == stop) {
                                            break;
                                        }
                                    } else {
                                        if (BCM_E_NOT_FOUND != rv) {
                                            soc_cm_sfree(unit, stm_tbl_chnk);
                                            return rv;
                                        }
                                    }
                                } /* endof for j=0;j<BCM_VLAN_RANGE_NUM;j++ */
                            }
                        } else {
                            soc_cm_sfree(unit, stm_tbl_chnk);
                            return rv;
                        }
                    }
                } /* endof for i=0;i<BCM_VLAN_RANGE_NUM;i++ */
                
                if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                            INNER_VLAN_RANGE_IDXf)) {
                    for (j = 0; j < BCM_VLAN_RANGE_NUM; j++) {
                        if ((1 == min_inner_vlan[j]) && 
                            (0 == max_inner_vlan[j])) {
                            continue;
                        }
                        trvs_info->inner_vlan = BCM_VLAN_INVALID;
                        trvs_info->inner_vlan_high = BCM_VLAN_INVALID;
                        trvs_info->outer_vlan = BCM_VLAN_INVALID;
                        trvs_info->outer_vlan_high = BCM_VLAN_INVALID;
                        /* check if entry key_type is
                           bcmVlanTranslateKeyPortInner */
                        rv = _bcm_trx_vlan_translate_action_range_get(
                                unit, gport, 
                                BCM_VLAN_INVALID, BCM_VLAN_INVALID,
                                min_inner_vlan[j], max_inner_vlan[j], 
                                trvs_info->action);
                        if (BCM_SUCCESS (rv)) {
                            trvs_info->key_type = bcmVlanTranslateKeyPortInner;
                            trvs_info->inner_vlan = min_inner_vlan[j];
                            trvs_info->inner_vlan_high = max_inner_vlan[j];
                            rv = trvs_info->int_cb(unit, trvs_info, &stop);
                            if (BCM_FAILURE(rv) || TRUE == stop) {
                                break;
                            }
                        } else {
                            if (BCM_E_NOT_FOUND != rv) {
                                soc_cm_sfree(unit, stm_tbl_chnk);
                                return rv;
                            }
                        }
                    }
                }
            } else {
                range_idx = soc_mem_field32_get(unit, mem, stm_ent, field);
                _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan,
                        range_idx);

                if ((1 == min_vlan[0]) && (0 == max_vlan[0])) {
                    continue;
                }

                stm_index = chnk_idx + ent_idx; 
                rv = _bcm_trx_vlan_translate_stm_index_to_gport(unit, stm_index,
                        &gport);
                if (BCM_FAILURE(rv)) {
                    break;
                }
                trvs_info->gport = gport;

                for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
                    if ((1 == min_vlan[i]) && (0 == max_vlan[i])) {
                        continue;
                    }
                    trvs_info->inner_vlan = BCM_VLAN_INVALID;
                    trvs_info->inner_vlan_high = BCM_VLAN_INVALID;
                    trvs_info->outer_vlan = BCM_VLAN_INVALID;
                    trvs_info->outer_vlan_high = BCM_VLAN_INVALID;

                    /* check if entry key_type is
                     * bcmVlanTranslateKeyPortOuter */
                    rv = _bcm_trx_vlan_translate_action_range_get(unit, 
                            gport, 
                            min_vlan[i], 
                            max_vlan[i], 
                            BCM_VLAN_INVALID,
                            BCM_VLAN_INVALID,
                            trvs_info->action);
                    if (BCM_SUCCESS (rv)) {
                        trvs_info->key_type = bcmVlanTranslateKeyPortOuter;
                        trvs_info->outer_vlan = min_vlan[i];
                        trvs_info->outer_vlan_high = max_vlan[i];
                    } else {
                        if (BCM_E_NOT_FOUND == rv) {
                            /* check if entry key_type is
                               bcmVlanTranslateKeyPortInner */
                            rv = _bcm_trx_vlan_translate_action_range_get(unit, 
                                    gport, 
                                    BCM_VLAN_INVALID,
                                    BCM_VLAN_INVALID,
                                    min_vlan[i], 
                                    max_vlan[i], 
                                    trvs_info->action);
                            if (BCM_FAILURE(rv)) {
                                soc_cm_sfree(unit, stm_tbl_chnk);
                                return rv;
                            }
                            trvs_info->key_type = bcmVlanTranslateKeyPortInner;
                            trvs_info->inner_vlan = min_vlan[i];
                            trvs_info->inner_vlan_high = max_vlan[i];
                        } else {
                            soc_cm_sfree(unit, stm_tbl_chnk);
                            return rv;
                        }
                    }

                    rv = trvs_info->int_cb(unit, trvs_info, &stop);
                    if (BCM_FAILURE(rv) || TRUE == stop) {
                        break;
                    }
                }
            }
        }
    }
    soc_cm_sfree(unit, stm_tbl_chnk);
    return rv;        

}

/*
 * Function:
 *      _bcm_trx_vlan_translate_egress_entry_assemble
 * Purpose:
 *      Constructs vlan translate egress entry from given port class,
 *      inner and outer vlans
 * Parameters:
 *      unit            (IN) BCM unit number
 *      vent            (IN/OUT) vlan translate egress entry to construct
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_translate_egress_entry_assemble(int unit,
                                              void *vent,
                                              int port_class,
                                              bcm_vlan_t outer_vlan,
                                              bcm_vlan_t inner_vlan,
                                              bcm_vlan_translate_key_t key_type)
{
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    int id = -1;
    bcm_module_t modid = -1;
    bcm_trunk_t tgid = -1;
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
    int tunnel_id = 0;
    int dvp_id = BCM_GPORT_INVALID;
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    int vfi_index = 0;
#endif /* BCM_TRIDENT2PLUS_SUPPORT &&  INCLUDE_L3 */
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
    int rv = BCM_E_NONE;
    int ecmp = -1, nh_ecmp_index = -1;
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */

    soc_mem_t egr_mem = EGR_VLAN_XLATEm;
    uint32 ent_sz = sizeof(egr_vlan_xlate_entry_t);

#ifdef BCM_TRIUMPH2_SUPPORT
    soc_field_t kt_fld = ENTRY_TYPEf;
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        ent_sz = sizeof(egr_vlan_xlate_1_double_entry_t);
        kt_fld = KEY_TYPEf;
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (soc_feature(unit, soc_feature_vp_sharing)) {
        if (_BCM_VPN_VFI_IS_SET(outer_vlan)) 
        {   
            /* only invalid inner VLAN is allowed for VFI case */
            if (inner_vlan == BCM_VLAN_INVALID) {             
                _BCM_VPN_GET(vfi_index, _BCM_VPN_TYPE_VFI, outer_vlan);
                if (!_bcm_vfi_used_get(unit, vfi_index, _bcmVfiTypeAny)) {
                    return BCM_E_NOT_FOUND;
                } 
            } else {
                return BCM_E_PARAM;
            } 
        } else {         
            VLAN_CHK_ID(unit, outer_vlan);
            VLAN_CHK_ID(unit, inner_vlan);
        }
    } else 
#endif
    {
        VLAN_CHK_ID(unit, outer_vlan);
        VLAN_CHK_ID(unit, inner_vlan);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3) 
    if (inner_vlan == BCM_VLAN_INVALID) {
        if (BCM_GPORT_IS_VXLAN_PORT(port_class)) {
            if (soc_feature(unit, soc_feature_vxlan)) {
                dvp_id = BCM_GPORT_VXLAN_PORT_ID_GET(port_class);
            } else {
                return BCM_E_PORT;                
            }
        } else if (BCM_GPORT_IS_FLOW_PORT(port_class)) {
            if (soc_feature(unit, soc_feature_flex_flow)) {
                dvp_id = BCM_GPORT_FLOW_PORT_ID_GET(port_class);
            } else {
                return BCM_E_PORT;
            }
        } else if (BCM_GPORT_IS_MIM_PORT(port_class)) {
            if (soc_feature(unit, soc_feature_mim)) {
                dvp_id = BCM_GPORT_MIM_PORT_ID_GET(port_class);
            } else {
                return BCM_E_PORT;
            }
        } else if (BCM_GPORT_IS_MPLS_PORT(port_class)) {
            if (soc_feature(unit, soc_feature_mpls)) {
                dvp_id = BCM_GPORT_MPLS_PORT_ID_GET(port_class);
            } else {
                return BCM_E_PORT;
            }
        } else if (BCM_GPORT_IS_L2GRE_PORT(port_class)) {
            if (soc_feature(unit, soc_feature_l2gre)) {
                dvp_id = BCM_GPORT_L2GRE_PORT_ID_GET(port_class);
            } else {
                return BCM_E_PORT;
            }
        } else if (BCM_GPORT_IS_TUNNEL(port_class) || 
                   BCM_GPORT_IS_SUBPORT_PORT(port_class) || 
                   BCM_GPORT_IS_WLAN_PORT(port_class) ||
                   BCM_GPORT_IS_VLAN_PORT(port_class) ||
                   BCM_GPORT_IS_NIV_PORT(port_class)){
            return BCM_E_PORT; 
        }
    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    if (BCM_GPORT_IS_TUNNEL(port_class)) {
        if (soc_feature(unit, soc_feature_wlan)) {
            tunnel_id = BCM_GPORT_TUNNEL_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_SUBPORT_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_subport)) {
            bcm_module_t modid = -1;
            bcm_trunk_t tgid = -1;
            int port = -1;
            /* coverity[copy_paste_error] */
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port_class,
                                &modid, &port, &tgid, &dvp_id));
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_MIM_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_mim)) {
            dvp_id = BCM_GPORT_MIM_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
        
    } else if (BCM_GPORT_IS_WLAN_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_wlan)) {
            dvp_id = BCM_GPORT_WLAN_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_VLAN_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_vlan_vp)) {
            dvp_id = BCM_GPORT_VLAN_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_NIV_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_niv)) {
            dvp_id = BCM_GPORT_NIV_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_EXTENDER_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_port_extension)) {
            dvp_id = BCM_GPORT_EXTENDER_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        } 
    } else if (BCM_GPORT_IS_MPLS_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_mpls)) {
            dvp_id = BCM_GPORT_MPLS_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_FLOW_PORT(port_class)) {
        if (soc_feature(unit, soc_feature_flex_flow)) {
            dvp_id = BCM_GPORT_FLOW_PORT_ID_GET(port_class);
        } else {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_VXLAN_PORT(port_class)) {
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vxlan)) {
            ing_dvp_table_entry_t dvp;
            dvp_id = BCM_GPORT_VXLAN_PORT_ID_GET(port_class);
            BCM_IF_ERROR_RETURN(
                READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, dvp_id, &dvp));
            if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf) == 0x3) {
                return BCM_E_PORT;
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vxlan_lite)) {
            dvp_id = BCM_GPORT_VXLAN_PORT_ID_GET(port_class);
            rv = bcmi_gh2_vxlan_port_to_nh_ecmp_index(unit, dvp_id,
                                                      &ecmp, &nh_ecmp_index);
            if (BCM_FAILURE(rv) || (ecmp)) {
                return BCM_E_PORT;
            }
        } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
        {
            return BCM_E_PORT;
        }
    } else if (BCM_GPORT_IS_L2GRE_PORT(port_class)) {
#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_l2gre)) {
            ing_dvp_table_entry_t dvp;
            dvp_id = BCM_GPORT_L2GRE_PORT_ID_GET(port_class);
            BCM_IF_ERROR_RETURN(
                READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, dvp_id, &dvp));
            if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf) == 0x2) {
                return BCM_E_PORT;
            }
        } else
#endif
        {
            return BCM_E_PORT;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    sal_memset(vent, 0, ent_sz);

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (key_type == bcmVlanTranslateEgressKeyVxlanSubChannel) {
        if (!soc_feature(unit, soc_feature_vxlan_tunnel_vlan_egress_translation)) {
            return BCM_E_UNAVAIL;
        }
        if (inner_vlan == BCM_VLAN_INVALID ||
            outer_vlan == BCM_VLAN_INVALID ||
            !BCM_GPORT_IS_MODPORT(port_class)) {
            return BCM_E_PARAM;
        }
        soc_mem_field32_set(unit, egr_mem, vent, kt_fld,
            TD3_ELVXLT_1_HASH_KEY_TYPE_DGPP_OVID_IVID);
        soc_mem_field32_set(unit, egr_mem, vent, VXLAN_SUB_CHANNEL__OVIDf, outer_vlan);
        soc_mem_field32_set(unit, egr_mem, vent, VXLAN_SUB_CHANNEL__IVIDf, inner_vlan);
        soc_mem_field32_set(unit, egr_mem, vent, VXLAN_SUB_CHANNEL__DGPPf,
            (_SHR_GPORT_MODPORT_MODID_GET(port_class) << 8) |
            _SHR_GPORT_MODPORT_PORT_GET(port_class));

        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (inner_vlan == BCM_VLAN_INVALID) {
        soc_mem_field32_set(unit, egr_mem, vent,
                           VLAN_XLATE_VFI__VFIf, vfi_index);   

    } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    {
        soc_mem_field32_set(unit, egr_mem, vent, OVIDf, outer_vlan);
        soc_mem_field32_set(unit, egr_mem, vent, IVIDf, inner_vlan);
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (BCM_GPORT_IS_TUNNEL(port_class)) {
        soc_mem_field32_set(unit, egr_mem, vent, kt_fld, 2); /* WLAN */
        if (soc_mem_field_valid(unit, egr_mem, DATA_TYPEf)) {
            soc_mem_field32_set(unit, egr_mem, vent, DATA_TYPEf, 2);
        }
        soc_mem_field32_set(unit, egr_mem, vent, TUNNEL_IDf, tunnel_id);
    } else if (BCM_GPORT_INVALID != dvp_id) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (inner_vlan == BCM_VLAN_INVALID) {
            soc_mem_field32_set(unit, egr_mem, vent, kt_fld,
                           TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI); /* VFI + DVP */
#ifdef BCM_TRIDENT3_SUPPORT
            if (soc_mem_field_valid(unit, egr_mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, egr_mem, vent, DATA_TYPEf, 
                    TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI);
            }
#endif
            soc_mem_field32_set(unit, egr_mem, vent,
                               VLAN_XLATE_VFI__DVPf, dvp_id);                               
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */ 
#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vxlan_lite)) {
            /* ENTRY_TYPEf: NHI */
            soc_mem_field32_set(unit, egr_mem, vent, ENTRY_TYPEf, 0x7);
            /* NHI for NHI view */
            soc_mem_field32_set(unit, egr_mem, vent, NHI__NHIf, nh_ecmp_index);
        } else
#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
        {
#ifdef BCM_TRIUMPH3_SUPPORT
            if(SOC_IS_TRIUMPH3(unit)) {
                soc_mem_field32_set(unit, egr_mem, vent, KEY_TYPEf, 1); /* DVP */
            } else
#endif /* BCM_TRIUMPH3_SUPPORT */
            {
                soc_mem_field32_set(unit, egr_mem, vent, kt_fld, 1); /* DVP */
                if (soc_mem_field_valid(unit, egr_mem, DATA_TYPEf)) {
                    soc_mem_field32_set(unit, egr_mem, vent, DATA_TYPEf, 1);
            }
            }
            soc_mem_field32_set(unit, egr_mem, vent, DVPf, dvp_id);
        }
    } else 
#endif
    {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (inner_vlan == BCM_VLAN_INVALID) {             
            soc_mem_field32_set(unit, egr_mem, vent, kt_fld, /* VFI + DVP Group*/
                                TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP);
            if (soc_mem_field_valid(unit, egr_mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, egr_mem, vent, DATA_TYPEf,
                        TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP);
            }
            soc_mem_field32_set(unit, egr_mem, vent,
                        VFI_DVP_GROUP__VT_DVP_GROUP_IDf, port_class);

            if (soc_mem_field_valid(unit, egr_mem,
                        VFI_DVP_GROUP__DATA_OVERLAY_TYPEf)) {
                soc_mem_field32_set(unit, egr_mem, vent,
                        VFI_DVP_GROUP__DATA_OVERLAY_TYPEf, 1);
            } else if (soc_mem_field_valid(unit, egr_mem,
                        VFI_DVP_GROUP__DATA_TYPEf)) {
            soc_mem_field32_set(unit, egr_mem, vent,
                        VFI_DVP_GROUP__DATA_TYPEf, 1);  
            }
        } else
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */ 
        {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
            if ((BCM_GPORT_IS_MODPORT(port_class) ||
                BCM_GPORT_IS_SUBPORT_PORT(port_class)) &&
                soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port_class,
                            &modid, &port_class, &tgid, &id));
                if (modid != -1 && port_class != -1) {
                    soc_mem_field32_set(unit, egr_mem, vent, DST_MODIDf,
                            modid);
                    soc_mem_field32_set(unit, egr_mem, vent, DST_PORTf,
                            port_class);
                    if (SOC_MEM_FIELD_VALID(unit, egr_mem,
                            PORT_TYPE_DGPPf)) {
                        soc_mem_field32_set(unit, egr_mem, vent,
                            PORT_TYPE_DGPPf, TRUE);
                    }
                } else {
                    return BCM_E_PORT;
                }
            } else
#endif
            {
                soc_mem_field32_set(unit, egr_mem, vent, PORT_GROUP_IDf,
                        port_class);
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function   :
 *      bcm_vlan_translate_egress_action_add
 * Description   :
 *      Add an entry to egress VLAN translation table.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 *      action          (IN) Action for outer and inner tag
 */
int 
_bcm_trx_vlan_translate_egress_action_add(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_translate_key_t key_type,
                                         bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 profile_idx;
    uint32 old_profile_idx = 0;
    egr_vlan_xlate_entry_t evx_ent;
    egr_vlan_xlate_entry_t evx_ent_old;

    egr_vlan_xlate_1_double_entry_t evx1d_ent;
    egr_vlan_xlate_1_double_entry_t evx1d_ent_old;
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    egr_vlan_xlate_2_double_entry_t evx2d_ent;
    egr_vlan_xlate_2_double_entry_t evx2d_ent_old;
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */

    void *vent = &evx_ent, *vent_old = &evx_ent_old;

    void *vent_ptr;

    int index;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_info_t *field_info = NULL, *field_info_1 = NULL;
    void *vent_tmp_p = NULL;
    egr_vlan_xlate_entry_t evx_ent_tmp;
    egr_vlan_xlate_1_double_entry_t evx1d_ent_tmp;
    void *vent_tmp = &evx_ent_tmp;
    uint32 ent_sz = sizeof(egr_vlan_xlate_entry_t);
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */ 
    
    soc_mem_t mem = EGR_VLAN_XLATEm;
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    int priority_base_index = -1;
#endif
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        vent = &evx1d_ent;
        vent_old = &evx1d_ent_old;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        vent_tmp = &evx1d_ent_tmp;
        ent_sz = sizeof(egr_vlan_xlate_1_double_entry_t);
#endif
    }

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (action->flags & BCM_VLAN_ACTION_SET_EGRESS_TUNNEL_OVID) {
        vent = &evx2d_ent;
        vent_old = &evx2d_ent_old;
        /* This entry resides in EVXLT_2 */
        mem = EGR_VLAN_XLATE_2_DOUBLEm;
        ent_sz = sizeof(egr_vlan_xlate_2_double_entry_t);
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_trx_egr_vlan_action_verify(unit, action));

    vent_ptr = vent;
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_egress_entry_assemble(unit, vent_ptr,
                                                       port_class,
                                                       outer_vlan,
                                                       inner_vlan,
                                                       key_type));

    soc_mem_lock(unit, mem);
    /* check if the entry with same key already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &index,
                        vent_ptr, vent_old, 0);

    if (rv == SOC_E_NONE) {
        /* alredy exists, update on the existing one, so the flex
         * counter related fields in TR3/Katana can be preserved.
         */
        vent_ptr = vent_old;
        old_profile_idx = soc_mem_field32_get(unit, mem, vent_ptr,
                                               TAG_ACTION_PROFILE_PTRf); 
    } else if (rv != SOC_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;  /* error condition */
    } else {
        /* doesn't exist, update the new entry */
        vent_ptr = vent;
    }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (inner_vlan == BCM_VLAN_INVALID) {     
        sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
        if (BCM_GPORT_IS_SET(port_class) &&
            !BCM_GPORT_IS_MODPORT(port_class)) {
            field_info = soc_mem_fieldinfo_get(unit, mem, 
                                          VLAN_XLATE_VFI__XLATE_DATAf);
        } else {             
            field_info = soc_mem_fieldinfo_get(unit, mem,
                                          VFI_DVP_GROUP__XLATE_DATAf);
        }             
        if (!field_info) {
                return BCM_E_UNAVAIL;        
        }
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(unit, mem, vent_ptr,
                                         field_info, entry);
        /* Reuse the format of field XLATE_DATA in XLATE view
            * for VLAN_XLATE_VFI and VFI_DVP view. 
            */                                 
        vent_tmp_p = vent_ptr;
        field_info_1 = soc_mem_fieldinfo_get(unit, mem,
                                             XLATE_DATAf);         
        if (!field_info_1) {
            return BCM_E_UNAVAIL;        
        }         
        sal_memset(vent_tmp, 0, ent_sz);
        vent_ptr = vent_tmp;
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(unit, mem, vent_ptr,
                                         field_info_1, entry); 
        if ( rv == SOC_E_NONE) {
            old_profile_idx = soc_mem_field32_get(unit, mem, vent_ptr,
                                              TAG_ACTION_PROFILE_PTRf);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    soc_mem_field32_set(unit, mem, vent_ptr, NEW_IVIDf,
                                    action->new_inner_vlan);
    soc_mem_field32_set(unit, mem, vent_ptr, NEW_OVIDf,
                                    action->new_outer_vlan);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        bcm_vlan_action_set_t action1;
        sal_memcpy(&action1, action, sizeof(bcm_vlan_action_set_t));
        if (action->priority == -1) {
            /* point to the default mapping table(0) for default priority action */
            PKT_VLAN_OUTER_ACTION_SET(unit, mem, vent_ptr, action);
            PKT_VLAN_INNER_ACTION_SET(unit, mem, vent_ptr, action);

#define PKT_PRI_DFLT_ACTION(_action_tag) \
   do { \
      if (((_action_tag) == bcmVlanActionReplace) || \
                ((_action_tag) == bcmVlanActionAdd) ) { \
                _action_tag ## _pkt_prio = _action_tag; \
      } \
   } while(0)
            PKT_PRI_DFLT_ACTION(action1.dt_outer);
            PKT_PRI_DFLT_ACTION(action1.dt_inner);
            PKT_PRI_DFLT_ACTION(action1.ot_outer);
            PKT_PRI_DFLT_ACTION(action1.ot_inner);
            PKT_PRI_DFLT_ACTION(action1.it_outer);
            PKT_PRI_DFLT_ACTION(action1.it_inner);
            PKT_PRI_DFLT_ACTION(action1.ut_outer);
            PKT_PRI_DFLT_ACTION(action1.ut_inner);
#undef PKT_PRI_DFLT_ACTION
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) {
                if ((action->inner_qos_map_id != -1) &&
                    (action->flags & BCM_VLAN_ACTION_SET_INNER_QOS_MAP_ID)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr2_qos_egr_mpls_external_ref_add(unit,
                             action->inner_qos_map_id, &priority_base_index));
                    soc_mem_field32_set(unit, mem, vent_ptr,
                            IPRI_MAPPING_PTRf, priority_base_index);
                }
                if ((action->outer_qos_map_id != -1) &&
                    (action->flags & BCM_VLAN_ACTION_SET_OUTER_QOS_MAP_ID)){
                    BCM_IF_ERROR_RETURN
                        (_bcm_tr2_qos_egr_mpls_external_ref_add(unit,
                             action->outer_qos_map_id, &priority_base_index));
                    soc_mem_field32_set(unit, mem, vent_ptr,
                            OPRI_MAPPING_PTRf, priority_base_index);
                }
            }
#endif
        } else {
            soc_mem_field32_set(unit, mem, vent_ptr, NEW_OPRIf,
                                        action->priority);
            soc_mem_field32_set(unit, mem, vent_ptr, NEW_OCFIf,
                                        action->new_outer_cfi);
            soc_mem_field32_set(unit, mem, vent_ptr, NEW_IPRIf,
                                        action->new_inner_pkt_prio);
            soc_mem_field32_set(unit, mem, vent_ptr, NEW_ICFIf,
                                        action->new_inner_cfi);
        }
        BCM_IF_ERROR_RETURN
            (_bcm_trx_egr_vlan_action_profile_entry_add(unit, &action1,
                                                    &profile_idx));
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            soc_mem_field32_set(unit, mem, vent_ptr, RPEf, 1);
            soc_mem_field32_set(unit, mem, vent_ptr, PRIf,
                                            action->priority);
            if (bcmVlanActionNone != action->it_inner_prio ||
                bcmVlanActionNone != action->dt_inner_prio) {                                           
                soc_mem_field32_set(unit, mem, vent_ptr, NEW_IRPEf, 1);
                soc_mem_field32_set(unit, mem, vent_ptr, NEW_IPRIf,
                                            action->priority);
            }
        }
        BCM_IF_ERROR_RETURN
            (_bcm_trx_egr_vlan_action_profile_entry_add(unit, action,
                                                    &profile_idx));
    }
    soc_mem_field32_set(unit, mem, vent_ptr, TAG_ACTION_PROFILE_PTRf,
                                    profile_idx);
    soc_mem_field32_set(unit, mem, vent_ptr, NEW_OTAG_VPTAG_SELf, 0);
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, mem, CLASS_IDf) &&
            (action->class_id)) {
        soc_mem_field32_set(unit, mem, vent_ptr, CLASS_IDf, action->class_id);
        soc_mem_field32_set(unit, mem, vent_ptr, CLASS_ID_VALIDf,1);
    }
#endif
    if (soc_feature(unit, soc_feature_base_valid)) {
        soc_mem_field32_set(unit, mem, vent_ptr, BASE_VALID_0f, 3);
        soc_mem_field32_set(unit, mem, vent_ptr, BASE_VALID_1f, 7);
    } else {
        soc_mem_field32_set(unit, mem, vent_ptr, VALIDf, 1);
    }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (inner_vlan == BCM_VLAN_INVALID) {         
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(unit, mem, vent_ptr,
                                         field_info_1, entry);            
        /* Restore the old pointer */
        vent_ptr = vent_tmp_p;
        /* Update the whole XLATE_DATAf for VLAN_XLATE_VFI and VFI_DVP view. */
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(unit, mem, vent_ptr,
                                         field_info, entry);                                 
        if (soc_feature(unit, soc_feature_base_valid)) {
            soc_mem_field32_set(unit, mem, vent_ptr, BASE_VALID_0f, 3);
            soc_mem_field32_set(unit, mem, vent_ptr, BASE_VALID_1f, 7);
        } else {
            soc_mem_field32_set(unit, mem, vent_ptr, VALIDf, 1);
        }
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TSN_SR_SUPPORT)
    if (soc_feature(unit, soc_feature_tsn_sr)) {
        if ((!(action->flags & BCM_VLAN_ACTION_SET_SR_ENABLE)) &&
              (action->flags & BCM_VLAN_ACTION_SET_SR_LAN_ID)) {
           return BCM_E_PARAM;
        }

        if (action->flags & BCM_VLAN_ACTION_SET_SR_ENABLE) {
            soc_mem_field32_set(unit, mem, vent_ptr, SR_ENABLEf, 1);
            soc_mem_field32_set(unit, mem, vent_ptr, SR_LAN_ID_VALIDf, 1);
            if (action->flags & BCM_VLAN_ACTION_SET_SR_LAN_ID) {
                soc_mem_field32_set(unit, mem, vent_ptr, SR_LAN_IDf, 1);
            }
        } else {
            soc_mem_field32_set(unit, mem, vent_ptr, SR_ENABLEf, 0);
            soc_mem_field32_set(unit, mem, vent_ptr, SR_LAN_ID_VALIDf, 0);
        }
    }
#endif /* BCM_TSN_SR_SUPPORT*/
    if (rv == SOC_E_NONE) { 
        /* write back the existing one */
        rv = soc_mem_write(unit, mem,
                             MEM_BLOCK_ALL, index, vent_ptr);
        if (rv == SOC_E_NONE) {
            /* Preserve ref_count for default profile in case deleting old profile */
            if (old_profile_idx == EGR_ACTION_PROFILE_DEFAULT) {
                soc_profile_mem_reference(unit, egr_action_profile[unit],
                                          EGR_ACTION_PROFILE_DEFAULT, 1);
            }
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                           old_profile_idx);
        }
    } else {
        /* insert new one */
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent_ptr);
    }
    soc_mem_unlock(unit, mem);

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_translate_egress_action_get
 * Purpose:
 *      Get an egress vlan translate entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      old_vid         (IN) Packet old VLAN ID to match
 *      new_vid         (OUT) Packet new VLAN ID
 *      prio            (OUT) Translation priority
 */
extern int 
_bcm_trx_vlan_translate_egress_action_get(int unit, int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan,
                                         bcm_vlan_translate_key_t key_type,
                                         bcm_vlan_action_set_t *action)
{
    egr_vlan_xlate_entry_t  evx_ent;
    egr_vlan_xlate_entry_t  res_evx_ent;

    egr_vlan_xlate_1_double_entry_t evx1d_ent;
    egr_vlan_xlate_1_double_entry_t res_evx1d_ent;
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    egr_vlan_xlate_2_double_entry_t res_evx2d_ent;
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */

    int                     rv;
    int                     idx = 0, profile_idx; 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_info_t *field_info = NULL, *field_info_1 = NULL;
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */

    void *vent = &evx_ent;
    void *res_vent = &res_evx_ent;
    soc_mem_t mem = EGR_VLAN_XLATEm;
    uint32 ent_sz = sizeof(egr_vlan_xlate_entry_t);
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        ent_sz = sizeof(egr_vlan_xlate_1_double_entry_t);
        vent = &evx1d_ent;
        res_vent = &res_evx1d_ent;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_egress_entry_assemble(unit, vent,
                                                       port_class,
                                                       outer_vlan,
                                                       inner_vlan,
                                                       key_type));

    sal_memset(res_vent, 0, ent_sz);
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
                        vent, res_vent, 0);
    soc_mem_unlock(unit, mem);

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (rv == BCM_E_NOT_FOUND &&
        SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm) &&
        (inner_vlan == BCM_VLAN_NONE) &&
        (BCM_GPORT_IS_MODPORT(port_class) || !BCM_GPORT_IS_SET(port_class))) {
        res_vent = &res_evx2d_ent;

        /* Search EVXLT_2 for entry */
        mem = EGR_VLAN_XLATE_2_DOUBLEm;
        ent_sz = sizeof(egr_vlan_xlate_2_double_entry_t);
        sal_memset(res_vent, 0, ent_sz);
        soc_mem_lock(unit, mem);
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx,
                            vent, res_vent, 0);
        soc_mem_unlock(unit, mem);
    }
#endif
    BCM_IF_ERROR_RETURN(rv);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (inner_vlan == BCM_VLAN_INVALID) { 
        sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));        
        if (BCM_GPORT_IS_SET(port_class) &&
            !BCM_GPORT_IS_MODPORT(port_class)) {
            field_info = soc_mem_fieldinfo_get(unit, mem, 
                                          VLAN_XLATE_VFI__XLATE_DATAf);
        } else {             
            field_info = soc_mem_fieldinfo_get(unit, mem,
                                          VFI_DVP_GROUP__XLATE_DATAf);
        }        
        if (!field_info) {
            return BCM_E_UNAVAIL;        
        }          
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(unit, mem, res_vent,
                                         field_info, entry);
        field_info_1 = soc_mem_fieldinfo_get(unit, mem,
                                             XLATE_DATAf);         
        if (!field_info_1) {
            return BCM_E_UNAVAIL;        
        }         
        SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(unit, mem, res_vent,
                                         field_info_1, entry); 
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    profile_idx = soc_mem_field32_get(unit, mem, res_vent,
                                              TAG_ACTION_PROFILE_PTRf);       
    _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
   
    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_translate_entry_parse(unit, mem,
                                            (uint32 *)res_vent, action));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete
 * Purpose:
 *      Delete an egress vlan translate lookup entry.
 * Parameters:
 *      unit            (IN) BCM unit number
 *      port_class      (IN) Group ID of ingress port
 *      outer_vlan      (IN) Packet outer VLAN ID
 *      inner_vlan      (IN) Packet inner VLAN ID
 */
int 
_bcm_trx_vlan_translate_egress_action_delete(int unit,
                                            int port_class,
                                            bcm_vlan_t outer_vlan,
                                            bcm_vlan_t inner_vlan,
                                            bcm_vlan_translate_key_t key_type)
{
    egr_vlan_xlate_entry_t evx_ent;
    egr_vlan_xlate_1_double_entry_t evx1d_ent;
    int rv; 
    uint32 profile_idx;
    void *vent = (void *)&evx_ent;
    soc_mem_t mem = EGR_VLAN_XLATEm;
    soc_field_t vld_fld = VALIDf;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 evxlt_data_type = 0;
    soc_field_info_t *field_info = NULL, *field_info_1 = NULL;
    soc_format_t evxlt_format = EGR_VLAN_XLATE_XLATE_DATA_DOUBLEfmt;
    uint32 xlate_data[3];
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    uint32 use_ipri_mapping_table = 0;
    uint32 use_opri_mapping_table = 0;
    uint32 ipri_mapping_profile = 0;
    uint32 opri_mapping_profile = 0;
#endif
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t handle;
    _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
#endif

    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        vld_fld = BASE_VALID_0f;
        vent = (void *)&evx1d_ent;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_translate_egress_entry_assemble(unit, vent,
                                                       port_class,
                                                       outer_vlan,
                                                       inner_vlan,
                                                       key_type));

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_gport_service_counters)) {
        /* Record flex stat info before the returned entry
         * includes non-key info. */

        memcpy(&handle, vent, sizeof(_bcm_flex_stat_handle_t));
    }
#endif

    rv = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL, vent, vent);
#if defined(BCM_METROLITE_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
    if ((rv == SOC_E_NONE) &&
        soc_feature(unit, soc_feature_egress_vlan_xlate_with_pci_dei_mapping)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        sal_memset(xlate_data, 0, sizeof(xlate_data));
        if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
            evxlt_data_type = soc_mem_field32_get(unit, mem, vent, DATA_TYPEf);
        }
        if (evxlt_data_type == TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI) {
            soc_mem_field_get(unit, mem, vent,
                VLAN_XLATE_VFI__XLATE_DATAf, xlate_data);
            if (!SOC_FORMAT_IS_VALID(unit, evxlt_format))  {
                evxlt_format = EGR_VLAN_XLATE_XLATE_DATAfmt;
            }
            use_ipri_mapping_table = soc_format_field32_get(unit, evxlt_format, xlate_data, IPRI_CFI_SELf);
            ipri_mapping_profile = soc_format_field32_get(unit, evxlt_format, xlate_data, IPRI_MAPPING_PTRf);
            use_opri_mapping_table = soc_format_field32_get(unit, evxlt_format, xlate_data, OPRI_CFI_SELf);
            opri_mapping_profile = soc_format_field32_get(unit, evxlt_format, xlate_data, OPRI_MAPPING_PTRf);
        } else
#endif
        {
            use_ipri_mapping_table = soc_mem_field32_get(unit, mem, vent, IPRI_CFI_SELf);
            ipri_mapping_profile = soc_mem_field32_get(unit, mem, vent, IPRI_MAPPING_PTRf);
            use_opri_mapping_table = soc_mem_field32_get(unit, mem, vent, OPRI_CFI_SELf);
            opri_mapping_profile = soc_mem_field32_get(unit, mem, vent, OPRI_MAPPING_PTRf);
        }
        if (use_ipri_mapping_table) {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_qos_egr_mpls_external_ref_del(unit,ipri_mapping_profile));
        }
        if (use_opri_mapping_table)
        {
            BCM_IF_ERROR_RETURN
                (_bcm_tr2_qos_egr_mpls_external_ref_del(unit,opri_mapping_profile));
        }
    }
#endif
    if ((rv == SOC_E_NONE) && soc_mem_field32_get(unit, mem, vent, vld_fld)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (inner_vlan == BCM_VLAN_INVALID) {
            sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
            if (BCM_GPORT_IS_SET(port_class) &&
                !BCM_GPORT_IS_MODPORT(port_class)) {
                field_info = soc_mem_fieldinfo_get(unit, mem,
                                              VLAN_XLATE_VFI__XLATE_DATAf);
            } else {
                field_info = soc_mem_fieldinfo_get(unit, mem,
                                              VFI_DVP_GROUP__XLATE_DATAf);
            }
            if (!field_info) {
                return BCM_E_UNAVAIL;
            }
            SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(unit, mem, vent,
                                                  field_info, entry);
            field_info_1 = soc_mem_fieldinfo_get(unit, mem,
                                                 XLATE_DATAf);
            if (!field_info_1) {
                return BCM_E_UNAVAIL;
            }
            SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(unit, mem, vent,
                                                  field_info_1, entry);
        }
#endif
        profile_idx = soc_mem_field32_get(unit, mem, vent,
                                                      TAG_ACTION_PROFILE_PTRf);       
        /* Preserve ref_count for default profile in case deleting old profile */
        if (profile_idx == EGR_ACTION_PROFILE_DEFAULT) {
            soc_profile_mem_reference(unit, egr_action_profile[unit],
                                      EGR_ACTION_PROFILE_DEFAULT, 1);
        }
        /* Delete the old vlan action profile entry */
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);

#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            (0 != soc_mem_field32_get(unit, mem, vent, VINTF_CTR_IDXf))) {
            /* Release Service counter */
            _bcm_esw_flex_stat_ext_handle_free(unit, _bcmFlexStatTypeVxlt,
                                               handle);
        }
#endif
    }

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm) &&
        (inner_vlan == BCM_VLAN_NONE) &&
        (BCM_GPORT_IS_MODPORT(port_class) || !BCM_GPORT_IS_SET(port_class))) {
        int rv2;
        /* This entry may reside in EVXLT_2 */
        mem = EGR_VLAN_XLATE_2_DOUBLEm;

        rv2 = soc_mem_delete_return_old(unit, mem, MEM_BLOCK_ALL, vent, vent);

        if ((rv2 == SOC_E_NONE) && soc_mem_field32_get(unit, mem, vent, vld_fld)) {
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                TAG_ACTION_PROFILE_PTRf);
            if (profile_idx != EGR_ACTION_PROFILE_DEFAULT) {
                /* Delete the old vlan action profile entry */
                rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx);
            }
        }
    }
#endif

    return rv;
}

/*
 * Function:
 *      bcm_vlan_translate_egress_action_delete_all
 * Purpose:
 *      Delete all egress vlan translate lookup entries.
 * Parameters:
 *      unit      (IN) BCM unit number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_translate_egress_action_delete_all(int unit) 
{
    int i, imin, imax, nent, vbytes, rv;
    uint32 old_profile_idx;
    void *vtab, *vtabp, *vnull;
#if defined(BCM_TRIUMPH2_SUPPORT)
    _bcm_flex_stat_handle_t handle;
    egr_vlan_xlate_entry_t vent;
    uint32 key[2];
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_field_info_t *field_info = NULL, *field_info_1 = NULL;
    int type;
    soc_field_t type_fld = ENTRY_TYPEf;
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
    int rv1 = BCM_E_NONE; /* To capture first failure in loop */
    soc_mem_t mem = EGR_VLAN_XLATEm;
    soc_field_t vld_fld = VALIDf;
    uint32 ent_sz = sizeof(egr_vlan_xlate_entry_t);
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_1_DOUBLEm;
        vld_fld = BASE_VALID_0f;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        type_fld = KEY_TYPEf;
#endif
        ent_sz = sizeof(egr_vlan_xlate_1_double_entry_t);
    }
    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);
    nent = soc_mem_index_count(unit, mem);
    vbytes = soc_mem_entry_words(unit, mem);
    
    vbytes = WORDS2BYTES(vbytes);
    vtab = soc_cm_salloc(unit, nent * ent_sz, "egr_vlan_xlate");

    if (vtab == NULL) {
        return BCM_E_MEMORY;
    }
    
    vnull = soc_mem_entry_null(unit, mem);

    soc_mem_lock(unit, mem);
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                            imin, imax, vtab);
    if (rv < 0) {
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vtab);
        return rv; 
    }
    
    for(i = 0; i < nent; i++) {

        if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                        egr_vlan_xlate_1_double_entry_t *, vtab, i);
        } else {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                        egr_vlan_xlate_entry_t *, vtab, i);
        }

        if (!soc_mem_field32_get(unit, mem, vtabp, vld_fld)) {
            continue;
        }
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (soc_mem_field_valid(unit, mem, type_fld)) {
            type = soc_mem_field32_get(unit, mem, vtabp, type_fld);
            if (type == TD2PLUS_EVLXLT_HASH_KEY_TYPE_VFI_DVP_GROUP) {
                field_info = soc_mem_fieldinfo_get(unit, mem,
                                                  VFI_DVP_GROUP__XLATE_DATAf);
            } else if (type == TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI) {
                field_info = soc_mem_fieldinfo_get(unit, mem,
                                                  VLAN_XLATE_VFI__XLATE_DATAf);
            }

            if (field_info) {
                sal_memset(entry, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));
                SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_GET(unit, mem, vtabp,
                                                      field_info, entry);
                field_info_1 = soc_mem_fieldinfo_get(unit, mem,
                                                     XLATE_DATAf);
                if (!field_info_1) {
                    return BCM_E_UNAVAIL;
                }
                SOC_MEM_EGR_VLAN_XLATE_FIELD_INFO_SET(unit, mem, vtabp,
                                                      field_info_1, entry);
            }
        }
#endif
        old_profile_idx =
            soc_mem_field32_get(unit, mem, vtabp,
                                            TAG_ACTION_PROFILE_PTRf);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, i, vnull);

        /* Delete the old vlan action profile entry */
        if (rv == SOC_E_NONE) {
            /* Preserve ref_count for default profile in case deleting old profile*/
            if (old_profile_idx == EGR_ACTION_PROFILE_DEFAULT) {
                soc_profile_mem_reference(unit, egr_action_profile[unit],
                                          EGR_ACTION_PROFILE_DEFAULT, 1);
            }
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit, 
                                                        old_profile_idx);
            if (BCM_FAILURE(rv) && BCM_SUCCESS(rv1)) {
                rv1 = rv;
            }
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (soc_feature(unit, soc_feature_gport_service_counters) &&
            (0 != soc_mem_field32_get(unit, mem, vtabp, VINTF_CTR_IDXf))) {
            sal_memset(&vent, 0, sizeof(vent));
            /* Construct key-only entry, copy to FS handle */
            soc_mem_field32_set(unit, mem, &vent, ENTRY_TYPEf,
                soc_mem_field32_get(unit, mem, vtabp, ENTRY_TYPEf));
            soc_mem_field_get(unit, mem, (uint32 *) vtabp,
                               KEYf, (uint32 *) key);
            soc_mem_field_set(unit, mem, (uint32 *) &vent,
                               KEYf, (uint32 *) key);

            _BCM_FLEX_STAT_HANDLE_CLEAR(handle);
            _BCM_FLEX_STAT_HANDLE_COPY(handle, vent);

            /* Release Service counter */
            _bcm_esw_flex_stat_ext_handle_free(unit, _bcmFlexStatTypeVxlt,
                                               handle);
        }
#endif
        } else if (BCM_SUCCESS(rv1)) {
            rv1 = rv;
        }
    }
    
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, vtab);

#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm)) {
        mem = EGR_VLAN_XLATE_2_DOUBLEm;
        vld_fld = BASE_VALID_0f;
        type_fld = KEY_TYPEf;
        ent_sz = sizeof(egr_vlan_xlate_2_double_entry_t);
        imin = soc_mem_index_min(unit, mem);
        imax = soc_mem_index_max(unit, mem);
        nent = soc_mem_index_count(unit, mem);
        vbytes = soc_mem_entry_words(unit, mem);

        vbytes = WORDS2BYTES(vbytes);
        vtab = soc_cm_salloc(unit, nent * ent_sz, "egr_vlan_xlate_2");

        if (vtab == NULL) {
            return BCM_E_MEMORY;
        }

        vnull = soc_mem_entry_null(unit, mem);

        soc_mem_lock(unit, mem);
        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                imin, imax, vtab);
        if (rv < 0) {
            soc_mem_unlock(unit, mem);
            soc_cm_sfree(unit, vtab);
            return rv;
        }

        for(i = 0; i < nent; i++) {
            vtabp = soc_mem_table_idx_to_pointer(unit, mem,
                        egr_vlan_xlate_2_double_entry_t *, vtab, i);

            if (!soc_mem_field32_get(unit, mem, vtabp, vld_fld)) {
                continue;
            }
            if (soc_mem_field_valid(unit, mem, type_fld)) {
                type = soc_mem_field32_get(unit, mem, vtabp, type_fld);
            }
            old_profile_idx =
                soc_mem_field32_get(unit, mem, vtabp,
                TAG_ACTION_PROFILE_PTRf);

            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, i, vnull);

            /* Delete the old vlan action profile entry */
            if (rv == SOC_E_NONE) {
                /* Preserve ref_count for default profile in case deleting old profile*/
                if (old_profile_idx == EGR_ACTION_PROFILE_DEFAULT) {
                    soc_profile_mem_reference(unit, egr_action_profile[unit],
                                              EGR_ACTION_PROFILE_DEFAULT, 1);
                }
                rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                                                            old_profile_idx);
                if (BCM_FAILURE(rv) && BCM_SUCCESS(rv1)) {
                    rv1 = rv;
                }
            } else if (BCM_SUCCESS(rv1)) {
                rv1 = rv;
            }
        }
        soc_mem_unlock(unit, mem);
        soc_cm_sfree(unit, vtab);
    }
#endif

    return rv1;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_old_vlan_get
 * Description   :
 *      Helper function to get an old vid from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      key_type        (IN) Key type to determine which old vid to get
 *      outer_vlan      (OUT) outer vlan to retrieve
 *      inner_vlan      (OUT) inner vlan to retrieve
 */

STATIC int 
_bcm_trx_vlan_translate_old_vlan_get(int unit, soc_mem_t mem, 
                                     uint32 *vent, uint32 key_type,
                                     bcm_vlan_t *outer_vlan, 
                                     bcm_vlan_t *inner_vlan)
{
    bcm_vlan_t  tmp_o_vlan = BCM_VLAN_INVALID;
    bcm_vlan_t  tmp_i_vlan = BCM_VLAN_INVALID;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    soc_field_t type_field = ENTRY_TYPEf;
    soc_mem_t egr_mem = EGR_VLAN_XLATEm;
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        type_field = KEY_TYPEf;
    }
#endif

    /* Input parameters check. */ 
    if ((NULL  == vent) || (NULL == outer_vlan) || 
        (INVALIDm == mem) || (NULL == inner_vlan)) {
        return (BCM_E_PARAM);
    }

    switch (key_type) {
      case bcmVlanTranslateKeyDouble:
      case bcmVlanTranslateKeyPortDouble:
      case bcmVlanTranslateKeyPortGroupDouble:
      case bcmVlanTranslateKeyCapwapPayloadDouble:
      case bcmVlanTranslateKeyPortCapwapPayloadDouble:
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
          if ((mem == egr_mem) && soc_feature(unit, soc_feature_vp_sharing) &&
             (soc_mem_field32_get_def(unit, mem, vent, type_field, FALSE) ==
                             TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI)) {
              _BCM_VPN_SET(tmp_o_vlan, _BCM_VPN_TYPE_VFI,
                           soc_mem_field32_get(unit, mem, vent, VLAN_XLATE_VFI__VFIf));
              tmp_i_vlan = BCM_VLAN_INVALID;
          } else
#endif
          {
              tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OVIDf);
              tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, IVIDf);
          }
          break;
      case bcmVlanTranslateKeyOuterTag:
      case bcmVlanTranslateKeyPortOuterTag:
      case bcmVlanTranslateKeyPortGroupOuterTag:
      case bcmVlanTranslateKeyCapwapPayloadOuterTag:
      case bcmVlanTranslateKeyPortCapwapPayloadOuterTag:
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OTAGf);
          break;
      case bcmVlanTranslateKeyInnerTag:
      case bcmVlanTranslateKeyPortInnerTag:
      case bcmVlanTranslateKeyPortGroupInnerTag:
      case bcmVlanTranslateKeyCapwapPayloadInnerTag:
      case bcmVlanTranslateKeyPortCapwapPayloadInnerTag:
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, ITAGf);
          break;
      case bcmVlanTranslateKeyOuter:
      case bcmVlanTranslateKeyPortOuter:
      case bcmVlanTranslateKeyPortGroupOuter:
      case bcmVlanTranslateKeyCapwapPayloadOuter:
      case bcmVlanTranslateKeyPortCapwapPayloadOuter:
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, OVIDf);
          break;
      case bcmVlanTranslateKeyInner:
      case bcmVlanTranslateKeyPortInner:
      case bcmVlanTranslateKeyPortGroupInner:
      case bcmVlanTranslateKeyCapwapPayloadInner:
      case bcmVlanTranslateKeyPortCapwapPayloadInner:
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, IVIDf);
          break;    
      case bcmVlanTranslateKeyPortPonTunnel:
      case bcmVlanTranslateKeyPortPonTunnelOuter:
      case bcmVlanTranslateKeyPortPonTunnelInner:
          if (!soc_feature(unit, soc_feature_lltag)) {
              return BCM_E_PARAM;
          }
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, LLTAG__LLVIDf);
          if (key_type == bcmVlanTranslateKeyPortPonTunnelOuter) {
              tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, LLTAG__OVIDf);
          } else if (key_type == bcmVlanTranslateKeyPortPonTunnelInner) {
              tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, LLTAG__IVIDf);
          }     
          break;
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
      case bcmVlanTranslateEgressKeyVxlanSubChannel:
          if (!soc_feature(unit, soc_feature_vxlan_tunnel_vlan_egress_translation)) {
              return BCM_E_PARAM;
          }
          tmp_o_vlan = soc_mem_field32_get(unit, mem, vent, VXLAN_SUB_CHANNEL__OVIDf);
          tmp_i_vlan = soc_mem_field32_get(unit, mem, vent, VXLAN_SUB_CHANNEL__IVIDf);
          break;
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */
      default:
          return BCM_E_PARAM;
    }

    *outer_vlan = tmp_o_vlan;
    *inner_vlan = tmp_i_vlan;

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_gport_get
 * Description   :
 *      Helper function to get a gport from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      gport           (OUT) gport to retrieve
 */

STATIC int
_bcm_trx_vlan_translate_gport_get(int unit, soc_mem_t mem,
                                  uint32 *vent, bcm_gport_t *gport)
{
    bcm_gport_t     tmp_gport = 0;
    uint32          modid;
    uint32          port;
    int             glp_wildcard;


    /* Input parameters check. */
    if ((NULL == vent) || (NULL == gport) || (INVALIDm == mem)) {
        return (BCM_E_PARAM);
    }

    glp_wildcard = (soc_mem_field32_get(unit, mem, vent, GLPf) == 
                    SOC_VLAN_XLATE_GLP_WILDCARD(unit));
    if (glp_wildcard) {
        tmp_gport = BCM_GPORT_INVALID;
    } else {
        if (soc_mem_field32_get(unit, mem, vent, Tf)) {
            port = soc_mem_field32_get(unit, mem, vent, TGIDf); 
            BCM_GPORT_TRUNK_SET(tmp_gport, port); 
        } else {
            port = soc_mem_field32_get(unit, mem, vent, PORT_NUMf); 
            modid = soc_mem_field32_get(unit, mem, vent, MODULE_IDf); 
            BCM_GPORT_MODPORT_SET(tmp_gport, modid, port); 
        }
    }

    *gport = tmp_gport;
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_key_type_get
 * Description   :
 *      Helper function to get a key_type from vlan translate entry 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translate memory id.
 *      vent            (IN) Vlan translate entry
 *      key_type        (OUT) Key type to retrieve
 */

STATIC int 
_bcm_trx_vlan_translate_key_type_get(int unit, soc_mem_t mem, 
                                     uint32 *vent, uint32 *key_type)
{
    uint32 key_val, glp_wildcard;
    int vt_key;
    uint32 source_type = 0;

    /* Input parameters check. */
    if ((NULL == vent) || (NULL == key_type) || (INVALIDm == mem)) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        if (mem ==  VLAN_XLATE_EXTDm) {
            key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPE_0f);
            key_val &= ~1; /*remove extd entry indication */ 
        } else {
            key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf); 
        }
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
    } 
    glp_wildcard = (soc_mem_field32_get(unit, mem, vent, GLPf) == 
                    SOC_VLAN_XLATE_GLP_WILDCARD(unit));
    if ((mem == VLAN_XLATEm) && SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
        source_type = soc_mem_field32_get(unit, mem, vent, SOURCE_TYPEf);
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_vlan_xlate_key_type_get(unit,key_val,&vt_key));
    switch (vt_key) {
      case VLXLT_HASH_KEY_TYPE_IVID_OVID:
          {
              if (source_type == 3) {
                  *key_type = bcmVlanTranslateKeyPortGroupDouble;
              } else if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyDouble;
              } else {
                  *key_type = bcmVlanTranslateKeyPortDouble;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_OTAG:
          {
              if (source_type == 3) {
                  *key_type = bcmVlanTranslateKeyPortGroupOuterTag;
              } else if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyOuterTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortOuterTag;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_ITAG:
          {
              if (source_type == 3) {
                  *key_type = bcmVlanTranslateKeyPortGroupInnerTag;
              } else if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyInnerTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortInnerTag;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_OVID:
          {
              if (source_type == 3) {
                  *key_type = bcmVlanTranslateKeyPortGroupOuter;
              } else if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyOuter;
              } else {
                  *key_type = bcmVlanTranslateKeyPortOuter;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_IVID: 
          {
              if (source_type == 3) {
                  *key_type = bcmVlanTranslateKeyPortGroupInner;
              } else if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyInner;
              } else {
                  *key_type = bcmVlanTranslateKeyPortInner;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_LLVID:
          {
              if (glp_wildcard) {
                  return BCM_E_NOT_FOUND;
              } else {
              *key_type = bcmVlanTranslateKeyPortPonTunnel;
              }
              break;
          }
      case VLXLT_HASH_KEY_TYPE_LLVID_OVID:
          {
              if (glp_wildcard) {
                  return BCM_E_NOT_FOUND;
              } else {
                  *key_type = bcmVlanTranslateKeyPortPonTunnelOuter;
              }
              break;
          }
      case VLXLT_HASK_KEY_TYPE_LLVID_IVID:
          {
              if (glp_wildcard) {
                  return BCM_E_NOT_FOUND;
              } else {
                  *key_type = bcmVlanTranslateKeyPortPonTunnelInner;
              }
              break;
          }
          break;
      case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID_OVID:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyCapwapPayloadDouble;
              } else {
                  *key_type = bcmVlanTranslateKeyPortCapwapPayloadDouble;
              }
              break;
          }
          break;
      case VLXLT_HASH_KEY_TYPE_PAYLOAD_OTAG:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyCapwapPayloadOuterTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortCapwapPayloadOuterTag;
              }
              break;
          }
          break;
      case VLXLT_HASH_KEY_TYPE_PAYLOAD_ITAG:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyCapwapPayloadInnerTag;
              } else {
                  *key_type = bcmVlanTranslateKeyPortCapwapPayloadInnerTag;
              }
              break;
          }
          break;
      case VLXLT_HASH_KEY_TYPE_PAYLOAD_OVID:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyCapwapPayloadOuter;
              } else {
                  *key_type = bcmVlanTranslateKeyPortCapwapPayloadOuter;
              }
              break;
          }
          break;
      case VLXLT_HASH_KEY_TYPE_PAYLOAD_IVID:
          {
              if (glp_wildcard) {
                  *key_type = bcmVlanTranslateKeyCapwapPayloadInner;
              } else {
                  *key_type = bcmVlanTranslateKeyPortCapwapPayloadInner;
              }
              break;
          }
          break;
      default:
          return BCM_E_NOT_FOUND;
          break;
    }

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_translate_parse
 * Description   :
 *      Helper function for an API to parse a vlan translate 
 *      entry for Triumph and call given call back.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      mem             (IN) Vlan translation memory id.
 *      vent            (IN) HW entry.
 *      trvs_info       (IN/OUT) Traverse structure that contain all relevant info
 */
int 
_bcm_trx_vlan_translate_parse(int unit, soc_mem_t mem, uint32 *vent,
                             _bcm_vlan_translate_traverse_t *trvs_info)
{
    uint32          profile_idx = 0;     /* Vlan profile index.      */
    soc_mem_t egr_mem = EGR_VLAN_XLATEm;
    soc_mem_t egr_mem2 = EGR_VLAN_XLATEm;
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
    soc_field_t type_field = ENTRY_TYPEf;
#endif
    int egr_key_type = 0;
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
    soc_mem_t ing_mem = VLAN_XLATEm;
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        ing_mem = VLAN_XLATE_1_DOUBLEm;
    }

    if (soc_feature(unit, soc_feature_niv) && (mem == ing_mem)) {
        uint32 key_val;
        int vt_key;
        int vif_hit = TRUE;
        int niv_port_id;
        int vp;

        key_val = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_xlate_key_type_get(unit,
              key_val,&vt_key));
        switch (vt_key) {
            case VLXLT_HASH_KEY_TYPE_VIF_OTAG:
                trvs_info->key_type = bcmVlanTranslateKeyPortOuterTag;
                trvs_info->outer_vlan = soc_mem_field32_get(unit, mem, 
                           vent, VIF__OTAGf);
                break;

            case VLXLT_HASH_KEY_TYPE_VIF_ITAG:
                trvs_info->key_type = bcmVlanTranslateKeyPortInnerTag;
                trvs_info->inner_vlan = soc_mem_field32_get(unit, mem, 
                           vent, VIF__ITAGf);
                break;

            case VLXLT_HASH_KEY_TYPE_VIF_VLAN:
                trvs_info->key_type = bcmVlanTranslateKeyPortOuter;
                trvs_info->outer_vlan = soc_mem_field32_get(unit, mem, 
                           vent, VIF__VLANf);
                break;

            case VLXLT_HASH_KEY_TYPE_VIF_CVLAN:
                trvs_info->key_type = bcmVlanTranslateKeyPortInner;
                trvs_info->inner_vlan = soc_mem_field32_get(unit, mem, 
                           vent, VIF__CVLANf);
                break;
            default:
                vif_hit = FALSE;
                break;
        }
        if (vif_hit == TRUE) {
            vp = soc_mem_field32_get(unit, mem, vent, VIF__SOURCE_VPf);
            BCM_GPORT_NIV_PORT_ID_SET(niv_port_id, vp);
            profile_idx = soc_mem_field32_get(unit, mem, vent,
                                      VIF__TAG_ACTION_PROFILE_PTRf); 
            _bcm_trx_vlan_action_profile_entry_get(unit, trvs_info->action,
                                               profile_idx);
            trvs_info->gport = niv_port_id;
            BCM_IF_ERROR_RETURN(_bcm_trx_vif_vlan_translate_entry_parse(unit, 
                     mem, vent, trvs_info->action));
            return BCM_E_NONE;
        }
    }
#endif   /* defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3) */
 
    profile_idx = soc_mem_field32_get(unit, mem, vent,
                                      TAG_ACTION_PROFILE_PTRf); 
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_1_DOUBLEm)) {
        egr_mem = EGR_VLAN_XLATE_1_DOUBLEm;
        type_field = KEY_TYPEf;
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_MEM_IS_VALID(unit, EGR_VLAN_XLATE_2_DOUBLEm)) {
        egr_mem2 = EGR_VLAN_XLATE_2_DOUBLEm;
    }
#endif

    if (mem == egr_mem || mem == egr_mem2) {
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, 
                                                   trvs_info->action, 
                                                   profile_idx);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vp_sharing) &&
           (soc_mem_field32_get_def(unit, mem, vent, type_field, FALSE) ==
                             TD2PLUS_EVLXLT_HASH_KEY_TYPE_VLAN_XLATE_VFI)) {
            int dvp = 0;
            dvp = soc_mem_field32_get(unit, mem, vent, VLAN_XLATE_VFI__DVPf);
            if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeVxlan)) {
                BCM_GPORT_VXLAN_PORT_ID_SET(trvs_info->port_class, dvp);
            }
        } else
#endif
#if defined(BCM_TRIDENT2_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vxlan) &&
            (soc_mem_field32_get_def(unit, mem, vent, type_field, FALSE) ==
                                     1)) {
            int dvp = 0;
            dvp = soc_mem_field32_get(unit, mem, vent, DVPf);
            if (_bcm_vp_used_get(unit, dvp, _bcmVpTypeVxlan)) {
                BCM_GPORT_VXLAN_PORT_ID_SET(trvs_info->port_class, dvp);
            }
        } else
#endif /* BCM_TRIDENT2_SUPPORT && INCLUDE_L3 */
#if defined(BCM_TRIDENT3_SUPPORT) && defined(INCLUDE_L3)
        if (soc_feature(unit, soc_feature_vxlan_tunnel_vlan_egress_translation) &&
            (soc_mem_field32_get_def(unit, mem, vent, type_field, FALSE) ==
                                     TD3_ELVXLT_1_HASH_KEY_TYPE_DGPP_OVID_IVID)) {
            bcm_module_t mod_out;
            bcm_port_t port_out;
            uint32_t dgpp = soc_mem_field32_get(unit, mem, vent,
                                 VXLAN_SUB_CHANNEL__DGPPf);
            port_out = dgpp & SOC_MEM_FIF_DGPP_PORT_MASK;
            mod_out = (dgpp & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
                        SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
            _SHR_GPORT_MODPORT_SET(trvs_info->port_class, mod_out, port_out);
            egr_key_type = bcmVlanTranslateEgressKeyVxlanSubChannel;
        }
        else
#endif /* BCM_TRIDENT3_SUPPORT && INCLUDE_L3 */
        {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_vxlate_supports_dgpp)) {
                if (soc_mem_field32_get(unit, mem, vent, PORT_TYPE_DGPPf)) {
                    bcm_module_t modid =
                        soc_mem_field32_get(unit, mem, vent, DST_MODIDf);
                    bcm_port_t port =
                        soc_mem_field32_get(unit, mem, vent, DST_PORTf);

                    BCM_GPORT_MODPORT_SET(trvs_info->port_class, modid, port);
                } else {
                    trvs_info->port_class =
                        soc_mem_field32_get(unit, mem, vent, PORT_GROUP_IDf);
                }
            } else
#endif
            {
                trvs_info->port_class = 
                    soc_mem_field32_get(unit, mem, vent, PORT_GROUP_IDf);
            }
        }
        trvs_info->gport = BCM_GPORT_INVALID;
        if (egr_key_type == 0) {
            egr_key_type = bcmVlanTranslateKeyPortDouble;
        }
        trvs_info->key_type = (egr_key_type == 0) ?
                                 bcmVlanTranslateKeyPortDouble : egr_key_type;
    } else {
        _bcm_trx_vlan_action_profile_entry_get(unit, trvs_info->action, 
                                               profile_idx);

        BCM_IF_ERROR_RETURN(
             _bcm_trx_vlan_translate_key_type_get(unit, mem, vent,
                                                  &(trvs_info->key_type)));
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_translate_gport_get(unit, mem, vent, 
                                              &(trvs_info->gport)));
        trvs_info->port_class = BCM_GPORT_INVALID;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_translate_entry_parse(unit, mem, vent,
                                            trvs_info->action));

    return _bcm_trx_vlan_translate_old_vlan_get(unit, mem, vent, 
                                              trvs_info->key_type,
                                              &(trvs_info->outer_vlan), 
                                              &(trvs_info->inner_vlan));
}

/*
*       VLAN PORT PROTOCOL 
*/

/*
 * Function:
 *      _bcm_trx_vlan_port_protocol_entry_parse
 * Purpose:
 *      Parses VLAN protocol entry and extracts frame type and ethertype
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      vpe         -     (IN) VLAN protocol entry to parse
 *      frame       -     (OUT) Frame type from the entry
 *      ether       -     (IN) Ethertype from the entry
 * Returns:
 *      None
 */
STATIC void
_bcm_trx_vlan_port_protocol_entry_parse(int unit, vlan_protocol_entry_t *vpe,
                                        bcm_port_frametype_t *frame,
                                        bcm_port_ethertype_t *ether)
{
    *frame = 0;
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, ETHERIIf)) {
        *frame |= BCM_PORT_FRAMETYPE_ETHER2;
    }
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, SNAPf)) {
        *frame |= BCM_PORT_FRAMETYPE_8023;
    }
    if (soc_VLAN_PROTOCOLm_field32_get(unit, vpe, LLCf)) {
        *frame |= BCM_PORT_FRAMETYPE_LLC;
    }
    *ether = soc_VLAN_PROTOCOLm_field32_get(unit, vpe, ETHERTYPEf);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_prot_match_get
 * Purpose:
 *      Get index into table with settings matching to port protocol
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      frame       -     (IN) Frmae to match
 *      ether       -     (IN) Ethertype to match
 *      match_idx   -     (OUT) index to the table entry if match, -1 otherwise
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_port_prot_match_get(int unit, bcm_port_frametype_t frame, 
                                  bcm_port_ethertype_t ether, int *match_idx)
{
    int                         i, idxmin, idxmax;
    vlan_protocol_entry_t       vpe;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
    
    *match_idx = -1;

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN
            (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        _bcm_trx_vlan_port_protocol_entry_parse(unit, &vpe, &ft, &et);
        if (ft == frame && et == ether) {
            *match_idx = i;
            break;
        }
    }

    return (*match_idx < 0) ?  BCM_E_NOT_FOUND : BCM_E_NONE ;
}

/*
 * Function:
 *      _bcm_trx_vlan_port_prot_match_get
 * Purpose:
 *      Get index into first empty entry in the table 
 * Parameters:
 *      unit        -     (IN) BCM Device unit
 *      empty_idx   -     (OUT) index to the table entry if match, -1 otherwise
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_trx_vlan_port_prot_empty_get(int unit, int *empty_idx)
{
    int                         i, idxmin, idxmax;
    vlan_protocol_entry_t       vpe;
    bcm_port_frametype_t        ft;
    
    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);
    
    *empty_idx = -1;

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN
            (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        ft = 0;
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, ETHERIIf)) {
            ft |= BCM_PORT_FRAMETYPE_ETHER2;
        }
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, SNAPf)) {
            ft |= BCM_PORT_FRAMETYPE_8023;
        }
        if (soc_VLAN_PROTOCOLm_field32_get(unit, &vpe, LLCf)) {
            ft |= BCM_PORT_FRAMETYPE_LLC;
        }
        if (0 == ft) {
            *empty_idx = i;
            break;
        }
    }

    return (*empty_idx < 0) ?  BCM_E_FULL : BCM_E_NONE ;
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_set
 * Description   :
 *      Helper function to set vlan_protocol_data entry
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      ventry          (IN / OUT) Entry to setup
 *      action          (IN) vlan action to be used
 *      profile_idx     (IN) profile index to set
 */
STATIC void 
_bcm_trx_vlan_protocol_data_entry_set(int unit, 
                                      vlan_protocol_data_entry_t *ventry, 
                                      bcm_vlan_action_set_t *action,
                                      int profile_idx)
{
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, OVIDf, 
                                        action->new_outer_vlan);
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, IVIDf, 
                                        action->new_inner_vlan);

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, OPRIf,
                                            action->priority);
        soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, OCFIf,
                                            action->new_outer_cfi);
        soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, IPRIf,
                                            action->new_inner_pkt_prio);
        soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, ICFIf,
                                            action->new_inner_cfi);
    } else {
        soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, PRIf,
                                            action->priority);
    }
    soc_VLAN_PROTOCOL_DATAm_field32_set(unit, ventry, TAG_ACTION_PROFILE_PTRf,
                                        profile_idx);
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_parse
 * Description   :
 *      Helper function to parse vlan_protocol_data entry into action
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      ventry          (IN) Entry to setup
 *      action          (OUT) vlan action to be used
 *      profile_idx     (OUT) profile index 
 */
STATIC void 
_bcm_trx_vlan_protocol_data_entry_parse(int unit, 
                                        vlan_protocol_data_entry_t *ventry, 
                                        bcm_vlan_action_set_t *action,
                                        int *profile_idx)
{
    action->new_outer_vlan = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, OVIDf);

    action->new_inner_vlan = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, IVIDf); 

    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, OPRIf);
        action->new_outer_cfi =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, OCFIf);
        action->new_inner_pkt_prio =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, IPRIf);
        action->new_inner_cfi =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, ICFIf);
    } else {
        action->priority =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, PRIf);
    }
    *profile_idx = 
        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, ventry, 
                                            TAG_ACTION_PROFILE_PTRf);
}

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_trx_vlan_port_protocol_action_reinit
 * Description:
 *      Recover data for 'vd_pbvl' in port_info structure.
 * Parameters:
 *      unit            (IN) BCM unit number
 * Notes:
 *      Assumes:
 *      - 'vd_pbvl' data in port_info structure is clear
 *      - 'vlan_prot_ptr' in port_info structure is initialized
 */
STATIC int
_bcm_trx_vlan_port_protocol_action_reinit(int unit)
{
    int                         idxmin, idxmax, i, data_idx;
    vlan_protocol_entry_t       vpe;
    vlan_protocol_data_entry_t  vde;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;
    bcm_pbmp_t                  pbmp;
    bcm_port_t                  port;
    bcm_vlan_action_set_t       def_action;
    bcm_vlan_t                  def_ovid, def_ivid;
    bcm_port_config_t           pconf;
    _bcm_port_info_t            *pinfo;

#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        /* KT2 devices recovery happens from scache in _bcm_esw_port_wb_recover */
        return BCM_E_NONE;
    }
#endif
    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);

    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pconf));
    pbmp = pconf.e; 
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(pbmp, pconf.cpu);
    }

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN
            (READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        _bcm_trx_vlan_port_protocol_entry_parse(unit, &vpe, &ft, &et);
        if (0 == ft) {
            continue;
        }

        PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_vlan_port_default_action_get(unit, port, &def_action));
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            data_idx = pinfo->vlan_prot_ptr + i;
            SOC_IF_ERROR_RETURN(
                READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, data_idx, &vde));
            def_ovid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OVIDf);
            def_ivid = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, IVIDf);
            if (def_ivid == def_action.new_inner_vlan &&
                def_ovid == def_action.new_outer_vlan) {
                continue;
            }

            _BCM_PORT_VD_PBVL_SET(pinfo, i);
        }
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */


/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_update
 * Description   :
 *      Helper function for an API to update lan port protocol data memory
 *      for specific ports.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      update_pbm      (IN) BCM port bitmap to update
 *      prot_idx        (IN) Index to Port Protocol table
 *      action          (IN) VLAN action to be used to update all ports in pbmp
 */
STATIC int
_bcm_trx_vlan_protocol_data_update(int unit, bcm_pbmp_t update_pbm, int prot_idx,
                                  bcm_vlan_action_set_t *action)
{
    bcm_port_t                  p;
    vlan_protocol_data_entry_t  vde;
    int                         data_idx, profile_idx;
    int                         c_profile_idx, use_default = 0;
    bcm_vlan_action_set_t       def_action, *action_p;
    _bcm_port_info_t            *pinfo;
    

    /* if action is NULL then default action should be used for each port */
    if (NULL == action) {
        use_default = 1;
        action_p = &def_action;
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_verify(unit, action));
        action_p = action;
    }

    BCM_PBMP_ITER(update_pbm, p) {
        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
        data_idx = pinfo->vlan_prot_ptr + prot_idx;

        if (use_default) {
        BCM_IF_ERROR_RETURN(
                bcm_esw_vlan_port_default_action_get(unit, p, action_p));
        }
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_profile_entry_add(unit, action_p, 
                                                   (uint32 *)&profile_idx));
        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, data_idx, &vde));

        c_profile_idx =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                                                TAG_ACTION_PROFILE_PTRf);
        sal_memset(&vde, 0, sizeof(vde));
        _bcm_trx_vlan_protocol_data_entry_set(unit, &vde, action_p, 
                                              profile_idx);
        BCM_IF_ERROR_RETURN(
            WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL, data_idx, &vde));
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_profile_entry_delete(unit, c_profile_idx));
    }
    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_entry_delete_by_idx
 * Description   :
 *      Helper function to delete a vlan_protocol_data entry from memory by 
 *      protocol index
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) Port
 *      vp_idx          (IN) Index to protcol id
 */
STATIC int 
_bcm_trx_vlan_protocol_data_entry_delete_by_idx(int unit, bcm_port_t port, 
                                                int vp_idx) 
{
    bcm_pbmp_t                  switched_pbm, update_pbm;
    bcm_port_config_t           pconf;
    _bcm_port_info_t            *pinfo;
    bcm_port_t                  p;
    int                         in_use = 0;
    vlan_protocol_entry_t       *vpnull;

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pconf));

    BCM_PBMP_CLEAR(update_pbm);
    BCM_PBMP_PORT_ADD(update_pbm, port);
    
    switched_pbm = pconf.e; 
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(switched_pbm, pconf.cpu);
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &switched_pbm));
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &switched_pbm);
    }
#endif

    BCM_PBMP_ITER(switched_pbm, p) {
        if (p == port) {    /* skip the port we just added */
            continue;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
        /* Check for explicit VID entry */
        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, vp_idx)) {
            BCM_PBMP_PORT_ADD(update_pbm, p);
        } else {
            in_use = 1;
        }
    }

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_protocol_data_profile_update(unit, update_pbm, vp_idx, NULL));
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_protocol_data_update(unit, update_pbm, vp_idx, NULL));
    }
    if (!in_use) {
        vpnull = soc_mem_entry_null(unit, VLAN_PROTOCOLm); 
        WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, vp_idx, vpnull);
    }

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_add
 * Description   :
 *      Helper function for an API to add vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 *      action          (IN) vlan action to be used
 */
int
_bcm_trx_vlan_port_protocol_action_add(int unit,
                                      bcm_port_t port,
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    
    bcm_port_t                  p;
    vlan_protocol_entry_t       vpe;
    int                         vpentry, empty, rv_m, rv_e;
    bcm_pbmp_t                  switched_pbm, update_pbm, clear_pbm;
    _bcm_port_info_t            *pinfo;
    bcm_port_config_t           pconf;

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    
    /* Verifying supported frame types */
    if ((0 == frame) || (BCM_PORT_FRAMETYPE_SNAP_PRIVATE < frame) ||
            (frame & (frame - 1))) {
        return BCM_E_PARAM;
    }
    rv_m = _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &vpentry);
    rv_e = _bcm_trx_vlan_port_prot_empty_get(unit, &empty);

    if ((BCM_E_NOT_FOUND == rv_m)  && (BCM_E_FULL == rv_e)) {
        return BCM_E_FULL;
    }
    if (BCM_E_NOT_FOUND == rv_m) {
        sal_memset(&vpe, 0, sizeof(vpe));
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, ETHERTYPEf, ether);
        if (frame & BCM_PORT_FRAMETYPE_ETHER2) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, ETHERIIf, 1);
        }
        if (frame & BCM_PORT_FRAMETYPE_8023) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, SNAPf, 1);
        }
        if (frame & BCM_PORT_FRAMETYPE_LLC) {
            soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, LLCf, 1);
        }
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, MATCHUPPERf, 1);
        soc_VLAN_PROTOCOLm_field32_set(unit, &vpe, MATCHLOWERf, 1);
        /* Actual writing into the memory will be last in this routine */
        vpentry = empty;
    }

    /*
     * Set VLAN ID for target port. For all other ethernet ports,
     * make sure entries indexed by the matched entry in VLAN_PROTOCOL
     * have initialized values (either default or explicit VID).
     */
    BCM_PBMP_CLEAR(update_pbm);
    BCM_PBMP_CLEAR(clear_pbm);
    BCM_IF_ERROR_RETURN(
        bcm_esw_port_config_get(unit, &pconf));
    switched_pbm = pconf.e;
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(switched_pbm, pconf.cpu);
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &switched_pbm));
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &switched_pbm);
    }
#endif

    BCM_PBMP_ITER(switched_pbm, p) {
        BCM_IF_ERROR_RETURN(
            _bcm_port_info_get(unit, p, &pinfo));
        if (p == port) {
            if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, vpentry)) {
                return BCM_E_EXISTS;
            }
            /* Set as explicit VID */
            _BCM_PORT_VD_PBVL_SET(pinfo, vpentry);
            BCM_PBMP_PORT_ADD(update_pbm, p);
        } else {
            if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, vpentry)) {
                continue;
            }
            /* Set to default VLAN ID and vlan actions */
            BCM_PBMP_PORT_ADD(clear_pbm, p);
        }
    }

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_KATANA2(unit) || SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_protocol_data_profile_update(unit,
            clear_pbm, vpentry, NULL));
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_protocol_data_profile_update(unit,
            update_pbm, vpentry, action));
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_protocol_data_update(unit,
            clear_pbm, vpentry, NULL));
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_protocol_data_update(unit,
            update_pbm, vpentry, action));
    }

    if (BCM_E_NOT_FOUND == rv_m) {
        BCM_IF_ERROR_RETURN(
            WRITE_VLAN_PROTOCOLm(unit, MEM_BLOCK_ALL, vpentry, &vpe));
}

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_get
 * Description   :
 *      Helper function for an API to get vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 *      action          (OUT) vlan action to get
 */
int
_bcm_trx_vlan_port_protocol_action_get(int unit, bcm_port_t port, 
                                      bcm_port_frametype_t frame,
                                      bcm_port_ethertype_t ether,
                                      bcm_vlan_action_set_t *action)
{
    int                             match_prot_idx, match_data_idx, profile_idx; 
    vlan_protocol_data_entry_t      vde;
    _bcm_port_info_t                *pinfo;

    if (NULL == action) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &match_prot_idx));

    BCM_IF_ERROR_RETURN(
        _bcm_port_info_get(unit, port, &pinfo));

    if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, match_prot_idx)) {
        return BCM_E_NOT_FOUND;
    }

    match_data_idx = pinfo->vlan_prot_ptr + match_prot_idx;
    
    BCM_IF_ERROR_RETURN(
        READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, match_data_idx, &vde));

    _bcm_trx_vlan_protocol_data_entry_parse(unit, &vde, action, &profile_idx);
    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx); 

    return BCM_E_NONE;
}

/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_delete
 * Description   :
 *      Helper function for an API to delete vlan port protocol action
 *      For TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 *      frame           (IN) Frame type
 *      ethr            (IN) Ethertype 
 */

int
_bcm_trx_vlan_port_protocol_delete(int unit, bcm_port_t port,
                                  bcm_port_frametype_t frame,
                                  bcm_port_ethertype_t ether)
{
    int                 vpentry, rv;
    _bcm_port_info_t    *pinfo;

    BCM_IF_ERROR_RETURN(
        _bcm_trx_vlan_port_prot_match_get(unit, frame, ether, &vpentry));

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));

    if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, vpentry)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_trx_vlan_protocol_data_entry_delete_by_idx(unit, port, vpentry);
    if (BCM_SUCCESS(rv)) {
        _BCM_PORT_VD_PBVL_CLEAR(pinfo, vpentry);
    }

    return rv;
}


/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_delete_all
 * Description   :
 *      Helper function for an API to delete all actions for all 
 *      vlan port protocols for TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      port            (IN) BCM port number
 */
int
_bcm_trx_vlan_port_protocol_delete_all(int unit, bcm_port_t port)
{
    int      i, idxmin, idxmax, rv;
    _bcm_port_info_t    *pinfo;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    
    rv = BCM_E_NONE;
    
    for (i = idxmin; i <= idxmax; i++) {
        if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) {
            rv = _bcm_trx_vlan_protocol_data_entry_delete_by_idx(unit, port, i);
            if (BCM_FAILURE(rv)) {
                break;
            } else {
                _BCM_PORT_VD_PBVL_CLEAR(pinfo, i);
            }
        }
    }

    return rv;
}


/*
 * Function   :
 *      _bcm_trx_vlan_port_protocol_action_traverse
 * Description   :
 *      Helper function for an API to traverse over all  
 *      vlan port protocols and call given callback routine for TRX 
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      cb              (IN) user callback function
 *      user_data       (IN) pointer to user data
 */
int 
_bcm_trx_vlan_port_protocol_action_traverse(int unit,
                                   bcm_vlan_port_protocol_action_traverse_cb cb,
                                           void *user_data)
{
    int                         idxmin, idxmax, i, data_idx, profile_idx;
    vlan_protocol_entry_t       vpe;
    vlan_protocol_data_entry_t  vde;
    bcm_port_frametype_t        ft;
    bcm_port_ethertype_t        et;
    bcm_pbmp_t                  pbmp;
    bcm_port_t                  port;
    bcm_vlan_action_set_t       action;
    bcm_port_config_t           pconf;
    _bcm_port_info_t            *pinfo;

    idxmin = soc_mem_index_min(unit, VLAN_PROTOCOLm);
    idxmax = soc_mem_index_max(unit, VLAN_PROTOCOLm);

    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pconf));
    pbmp = pconf.e; 
    if (soc_feature(unit, soc_feature_cpuport_switched)) {
        BCM_PBMP_OR(pbmp, pconf.cpu);
    }
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit) && soc_feature(unit, soc_feature_flex_port)) {
        BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update(unit, &pbmp));
    }
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp);
    }
#endif

    for (i = idxmin; i <= idxmax; i++) {
        SOC_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOLm(unit, MEM_BLOCK_ANY, i, &vpe));
        _bcm_trx_vlan_port_protocol_entry_parse(unit, &vpe, &ft, &et);
        if (0 == ft) {
            continue;
        }
        PBMP_ITER(pbmp, port) {
            BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
            if (_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) {
                data_idx = pinfo->vlan_prot_ptr + i;
                SOC_IF_ERROR_RETURN(READ_VLAN_PROTOCOL_DATAm(unit,
                            MEM_BLOCK_ANY, data_idx, &vde));
                profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                        TAG_ACTION_PROFILE_PTRf);
                action.new_inner_vlan =
                    soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, IVIDf);
                action.new_outer_vlan =
                    soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OVIDf);
                if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                    action.priority =
                        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OPRIf);
                    action.new_outer_cfi =
                        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, OCFIf);
                    action.new_inner_pkt_prio =
                        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, IPRIf);
                    action.new_inner_cfi =
                        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, ICFIf);
                } else {
                    action.priority =
                        soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde, PRIf);
                }
                /* Read action profile data. */
                _bcm_trx_vlan_action_profile_entry_get(unit, &action, profile_idx);
                BCM_IF_ERROR_RETURN(cb(unit, port, ft, et, &action, user_data));
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_default_action_set
 * Purpose:
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_default_action_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    uint32 profile_idx, old_profile_idx, vp_profile_idx;
    bcm_port_cfg_t pcfg;
    vlan_protocol_data_entry_t vde;
    int vlan_prot_entries, vlan_data_prot_start, i;
    _bcm_port_info_t *pinfo;
#ifdef BCM_HGPROXY_COE_SUPPORT
    int rv = BCM_E_NONE;
#endif

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_add(unit, action, &profile_idx));

#ifdef BCM_HGPROXY_COE_SUPPORT
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        uint32 values[5];
        soc_field_t fields[] = {
            TAG_ACTION_PROFILE_PTRf,
            PORT_PRIf,
            OCFIf,
            IPRIf,
            ICFIf
        };

        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_get(unit, port, LPORT_PROFILE_LPORT_TAB,
                                           5, fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        old_profile_idx = values[0];
        pcfg.pc_new_opri = values[1];
        pcfg.pc_new_ocfi = values[2];
        pcfg.pc_new_ipri = values[3];
        pcfg.pc_new_icfi = values[4];
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
        old_profile_idx = pcfg.pc_vlan_action;
    }

    pcfg.pc_vlan = action->new_outer_vlan;
    pcfg.pc_ivlan = action->new_inner_vlan;
    pcfg.pc_vlan_action = profile_idx;
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            pcfg.pc_new_opri = action->priority;
        }
        pcfg.pc_new_ocfi = action->new_outer_cfi;
        pcfg.pc_new_ipri = action->new_inner_pkt_prio;
        pcfg.pc_new_icfi = action->new_inner_cfi;
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            pcfg.pc_new_opri = action->priority;
        }
    }

#ifdef BCM_HGPROXY_COE_SUPPORT
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        uint32 values[7];
        soc_field_t fields[] = {
            PORT_VIDf,
            IVIDf,
            TAG_ACTION_PROFILE_PTRf,
            PORT_PRIf,
            OCFIf,
            IPRIf,
            ICFIf
        };

        values[0] = pcfg.pc_vlan;
        values[1] = pcfg.pc_ivlan;
        values[2] = pcfg.pc_vlan_action;
        values[3] = pcfg.pc_new_opri;
        values[4] = pcfg.pc_new_ocfi;
        values[5] = pcfg.pc_new_ipri;
        values[6] = pcfg.pc_new_icfi;

        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_set(unit, port, LPORT_PROFILE_LPORT_TAB,
                                           7, fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }else
#endif
    {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
    }

    SOC_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    /*
     * Update default VLAN ID in VLAN_PROTOCOL_DATA
     */
    vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    vlan_data_prot_start = pinfo->vlan_prot_ptr;

    if (pinfo->p_vd_pbvl == 0) {
        /* Avoid one crash in kt2 tr 18 test case in linux environment */
        return (BCM_E_NONE);
    }

    
    for (i = 0; i < vlan_prot_entries; i++) {

        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                                     vlan_data_prot_start + i, &vde));
        vp_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                             TAG_ACTION_PROFILE_PTRf);

        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) { 

            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_action_profile_entry_add(unit, action, 
                                                       &profile_idx));
            _bcm_trx_vlan_protocol_data_entry_set(unit, &vde, action, 
                                                  profile_idx);

            BCM_IF_ERROR_RETURN(
                WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL,
                                           vlan_data_prot_start + i, &vde));
            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_action_profile_entry_delete(unit, 
                                                          vp_profile_idx));
        }
    }
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_port_default_action_get
 * Purpose:
 *      Get the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */ 
int 
_bcm_trx_vlan_port_default_action_get(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    uint32 profile_idx;
    bcm_port_cfg_t pcfg;
    int vid;
    int ivid;
#ifdef BCM_HGPROXY_COE_SUPPORT
    int rv = BCM_E_NONE;
#endif

    bcm_vlan_action_set_t_init(action);

#ifdef BCM_HGPROXY_COE_SUPPORT
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        uint32 values[7];
        soc_field_t fields[] = {
            PORT_VIDf,
            IVIDf,
            TAG_ACTION_PROFILE_PTRf,
            PORT_PRIf,
            OCFIf,
            IPRIf,
            ICFIf
        };

        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_get(unit, port, LPORT_PROFILE_LPORT_TAB,
                                           7, fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        vid = values[0];
        ivid = values[1];
        profile_idx = values[2];
        pcfg.pc_new_opri = values[3];
        pcfg.pc_new_ocfi = values[4];
        pcfg.pc_new_ipri = values[5];
        pcfg.pc_new_icfi = values[6];
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
        profile_idx = pcfg.pc_vlan_action;
        vid = pcfg.pc_vlan;
        ivid = pcfg.pc_ivlan;
    }

    _bcm_trx_vlan_action_profile_entry_get(unit, action, profile_idx);

    action->new_outer_vlan = vid;
    action->new_inner_vlan = ivid;
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        action->priority = pcfg.pc_new_opri; 
        action->new_outer_cfi = pcfg.pc_new_ocfi;
        action->new_inner_pkt_prio = pcfg.pc_new_ipri;
        action->new_inner_cfi = pcfg.pc_new_icfi;
    } else {
        action->priority = pcfg.pc_new_opri;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_tr_vlan_port_default_action_delete
 * Purpose:
 *      Deletes the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */ 
int 
_bcm_trx_vlan_port_default_action_delete(int unit, bcm_port_t port)
{
    uint32                      old_profile_idx;
    uint32                      vp_profile_idx;
    bcm_port_cfg_t              pcfg;
    _bcm_port_info_t            *pinfo;
    int                         num_ent, start, i;
    vlan_protocol_data_entry_t  vde;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    old_profile_idx = pcfg.pc_vlan_action;

    pcfg.pc_vlan = BCM_VLAN_DEFAULT;
    pcfg.pc_ivlan = 0;
    pcfg.pc_vlan_action = ing_action_profile_def[unit];
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    _bcm_trx_vlan_action_profile_entry_increment(unit,
            ing_action_profile_def[unit]);
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_action_profile_entry_delete(unit, old_profile_idx));
    
    BCM_IF_ERROR_RETURN(
        _bcm_port_info_get(unit,port, &pinfo));

    num_ent = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    start = pinfo->vlan_prot_ptr;
    for (i = 0; i < num_ent; i++) {
        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) { 
            BCM_IF_ERROR_RETURN(READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                        start + i, &vde));
            vp_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                    TAG_ACTION_PROFILE_PTRf);

            sal_memset(&vde, 0, sizeof(vlan_protocol_data_entry_t));
            soc_VLAN_PROTOCOL_DATAm_field32_set(unit, &vde, OVIDf,
                    BCM_VLAN_DEFAULT); 
            soc_VLAN_PROTOCOL_DATAm_field32_set(unit, &vde,
                    TAG_ACTION_PROFILE_PTRf, ing_action_profile_def[unit]);
            BCM_IF_ERROR_RETURN(WRITE_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ALL,
                        start + i, &vde));

            _bcm_trx_vlan_action_profile_entry_increment(unit,
                    ing_action_profile_def[unit]);
            BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_delete(unit, 
                        vp_profile_idx));
        }
    }
    return (BCM_E_NONE);
}

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * Function:
 *      _bcm_td2p_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action,
                                            uint32 profile_idx)
{
    int rv;
    uint32 old_profile_idx;
    egr_vlan_control_3_entry_t entry_3;
    egr_vlan_control_2_entry_t entry_2;

    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        int field_cnt = 0;
        soc_field_t fields[6];
        uint32 values[6];

        if (-1 == action->priority) {
            fields[field_cnt] = OPRI_CFI_SELf;
            values[field_cnt++] = 1;
        } else {
            fields[field_cnt] = OPRI_CFI_SELf;
            values[field_cnt++] = 0;
            fields[field_cnt] = OPRIf;
            values[field_cnt++] = action->priority;
            fields[field_cnt] = OCFIf;
            values[field_cnt++] = action->new_outer_cfi;
        }
        fields[field_cnt] = OVIDf;
        values[field_cnt++] = action->new_outer_vlan;
        rv = bcm_esw_port_egr_lport_fields_set(unit, port, EGR_VLAN_CONTROL_2m,
                field_cnt, fields, values);
        if (rv < 0) {
            goto error;
        }

        /* EGR_VLAN_CONTROL_3m */
        field_cnt = 0;
        rv = bcm_esw_port_egr_lport_field_get(unit, port, EGR_VLAN_CONTROL_3m,
                TAG_ACTION_PROFILE_PTRf, &old_profile_idx);
        if (rv < 0) {
            goto error;
        }
        fields[field_cnt] = TAG_ACTION_PROFILE_PTRf;
        values[field_cnt++] = profile_idx;
        fields[field_cnt] = IVIDf;
        values[field_cnt++] = action->new_inner_vlan;
        if (-1 == action->priority) {
            fields[field_cnt] = IPRI_CFI_SELf;
            values[field_cnt++] = 1;
        } else {
            fields[field_cnt] = IPRI_CFI_SELf;
            values[field_cnt++] = 0;
            fields[field_cnt] = IPRIf;
            values[field_cnt++] = action->new_inner_pkt_prio;
            fields[field_cnt] = ICFIf;
            values[field_cnt++] = action->new_inner_cfi;
        }
        rv = bcm_esw_port_egr_lport_fields_set(unit, port, EGR_VLAN_CONTROL_3m,
                field_cnt, fields, values);
        if (rv < 0) {
            goto error;
        }
        /* Delete the old profile only if it is different the a new one. */
        if (profile_idx != old_profile_idx) {
            rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                    old_profile_idx);
            if (rv < 0) {
                goto error;
            }
        }

        return BCM_E_NONE;
    }

    rv = READ_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry_2);
    if (rv < 0) {
        goto error;
    }
    if (-1 == action->priority) {
        /* point to default map profile(0) for default priority action */
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_2m, 
                &entry_2, OPRI_CFI_SELf, 1);
    } else {
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_2m, 
                &entry_2, OPRI_CFI_SELf, 0);
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_2m, 
                &entry_2, OPRIf, action->priority);
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_2m, &entry_2, OCFIf,
                action->new_outer_cfi);
    }
    soc_mem_field32_set(unit, EGR_VLAN_CONTROL_2m, &entry_2,
            OVIDf, action->new_outer_vlan);
    rv = WRITE_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry_2);
    if (rv < 0) {
        goto error;
    }

    rv = READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3);
    if (rv < 0) {
        goto error;
    }   
    old_profile_idx = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m,
            &entry_3, TAG_ACTION_PROFILE_PTRf);
    soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry_3,
            TAG_ACTION_PROFILE_PTRf, profile_idx);
    soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry_3,
            IVIDf, action->new_inner_vlan);

    if ( -1 == action->priority) {
        /* point to default map profile(0) for default priority action */
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m,
                &entry_3, IPRI_CFI_SELf, 1);
    } else {
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m,
                &entry_3, IPRI_CFI_SELf, 0);
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry_3, IPRIf,
                action->new_inner_pkt_prio);
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_3m, &entry_3, ICFIf,
                action->new_inner_cfi);
    }
    rv = WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3);
    if (rv < 0) {
        goto error;
    }

    /* Delete the old profile only if it is different the a new one. */
    if (profile_idx != old_profile_idx) {
        rv = _bcm_trx_egr_vlan_action_profile_entry_delete(unit,
                old_profile_idx);
        if (rv < 0) {
            goto error;
        }
    }

    return BCM_E_NONE;

error:
    /* Undo action profile entry addition */
    if (_bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx) != BCM_E_NONE) {
        LOG_INFO(BSL_LS_SOC_VLAN,
                (BSL_META_U(unit,
                            "Failed to undo profile entry addition\n")));
    }
    return rv;
}

/*
 * Function:
 *      _bcm_td2pvlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    uint32 profile_idx;
    egr_vlan_control_3_entry_t entry_3;
    egr_vlan_control_2_entry_t entry_2;

    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        int field_count = 5;
        soc_field_t fields[5] = {
            TAG_ACTION_PROFILE_PTRf,
            IVIDf,
            IPRI_CFI_SELf,
            IPRIf,
            ICFIf
        };
        uint32 values[5];
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_fields_get(unit, port, EGR_VLAN_CONTROL_3m,
            field_count, fields, values));
        profile_idx = values[0];
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
        action->new_inner_vlan = values[1];
        if (!values[2]) {
            action->new_inner_pkt_prio = values[3];
            action->new_inner_cfi = values[4];
        }

        /* EGR_VLAN_CONTROL_2m */
        field_count = 4;
        fields[0] = OVIDf;
        fields[1] = OPRI_CFI_SELf;
        fields[2] = OPRIf;
        fields[3] = OCFIf;
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_fields_get(unit, port, EGR_VLAN_CONTROL_2m,
            field_count, fields, values));
        action->new_outer_vlan = values[0];
        if (!values[1]) {
            action->priority = values[2];
            action->new_outer_cfi = values[3];
        }

        return BCM_E_NONE;
    }
    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));
    profile_idx = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m,
            &entry_3, TAG_ACTION_PROFILE_PTRf);
    _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
    action->new_inner_vlan = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m,
            &entry_3, IVIDf);
    if (!soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m, &entry_3,
                IPRI_CFI_SELf)) {
        action->new_inner_pkt_prio =
            soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m, &entry_3, IPRIf);
        action->new_inner_cfi =
            soc_mem_field32_get(unit, EGR_VLAN_CONTROL_3m, &entry_3, ICFIf);
    }

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry_2));
    action->new_outer_vlan = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_2m,
            &entry_2, OVIDf);
    if (!soc_mem_field32_get(unit, EGR_VLAN_CONTROL_2m, &entry_2,
                OPRI_CFI_SELf)) {
        action->priority =
            soc_mem_field32_get(unit, EGR_VLAN_CONTROL_2m, &entry_2, OPRIf);
        action->new_outer_cfi =
            soc_mem_field32_get(unit, EGR_VLAN_CONTROL_2m, &entry_2, OCFIf);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vlan_port_egress_default_action_delete
 * Purpose:
 *      Deletes the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    uint32 profile_idx;
    egr_vlan_control_3_entry_t entry_3;
    egr_vlan_control_2_entry_t entry_2;

    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_field_get(unit, port, EGR_VLAN_CONTROL_3m,
            TAG_ACTION_PROFILE_PTRf, &profile_idx));
    } else {
    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));
    profile_idx = soc_EGR_VLAN_CONTROL_3m_field32_get(unit, &entry_3,
            TAG_ACTION_PROFILE_PTRf);
    }

    if (!SOC_IS_TRIDENT3X(unit)) {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx));
    }

    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_fields_clear(unit, port, EGR_VLAN_CONTROL_2m));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_egr_lport_fields_clear(unit, port, EGR_VLAN_CONTROL_3m));
    } else {
    sal_memset(&entry_3, 0, sizeof(entry_3));
    sal_memset(&entry_2, 0, sizeof(entry_2));
    BCM_IF_ERROR_RETURN(
            WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));
    BCM_IF_ERROR_RETURN(
            WRITE_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry_2));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td2p_vlan_port_egress_action_profile_clear
 * Purpose:
 *      Clear the egress port's default vlan tag profile pointer
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td2p_vlan_port_egress_action_profile_clear(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
            port, EGR_VLAN_CONTROL_3m, TAG_ACTION_PROFILE_PTRf, 0));
    } else {
    egr_vlan_control_3_entry_t entry_3;

    BCM_IF_ERROR_RETURN
        (READ_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));
    soc_EGR_VLAN_CONTROL_3m_field32_set(unit, &entry_3,
                                        TAG_ACTION_PROFILE_PTRf, 0);
    BCM_IF_ERROR_RETURN(
            WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_set(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    int rv;
    uint32 rval, profile_idx, old_profile_idx;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return(
            _bcm_th3_vlan_port_egress_default_action_set(unit, port, action));
    }
#endif/* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
    /* Tomahawk low latency mode */
    if (soc_feature(unit, soc_feature_fast_egr_vlan_action)) {
        rval = _bcm_fast_egr_vlan_port_egress_default_action_set(unit,
                                                               port, action);
        return rval;
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_trx_egr_vlan_action_verify(unit, action));
    BCM_IF_ERROR_RETURN(
        _bcm_trx_egr_vlan_action_profile_entry_add(unit, action, &profile_idx));

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
        return _bcm_td2p_vlan_port_egress_default_action_set(unit, port,
                action, profile_idx);
    } else
#endif
    {
        rv = READ_EGR_VLAN_CONTROL_2r(unit, port, &rval);
        if (rv < 0) {
            goto error;
        }

        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (-1 == action->priority) {
                /* point to default map profile(0) for default priority action */
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRI_CFI_SELf, 1);
            } else {
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRI_CFI_SELf, 0);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRIf,
                          action->priority);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OCFIf,
                          action->new_outer_cfi);
            }
        } else {
            /* -1 reserved value */
            if (-1 == action->priority) {
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, ORPEf, 0);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRIf, 0);
            } else if (action->priority <= BCM_PRIO_MAX){
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, ORPEf, 1);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval, OPRIf,
                              action->priority);
            } else {
                rv = BCM_E_PARAM;
                goto error;
            }
        }
    
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_2r, &rval,
                      OVIDf, action->new_outer_vlan);
        rv = WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval);
        if (rv < 0) {
            goto error;
        }

        rv = READ_EGR_VLAN_CONTROL_3r(unit, port, &rval);
        if (rv < 0) {
            goto error;
        }   
        old_profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                        rval, TAG_ACTION_PROFILE_PTRf);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                      TAG_ACTION_PROFILE_PTRf, profile_idx);
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval,
                      IVIDf, action->new_inner_vlan);

        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if ( -1 == action->priority) {
                /* point to default map profile(0) for default priority action */
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRI_CFI_SELf, 1);
            } else {
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRI_CFI_SELf, 0);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRIf,
                          action->new_inner_pkt_prio);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, ICFIf,
                          action->new_inner_cfi);
            }
        } else {
            if ( -1 == action->priority) {
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IRPEf, 0);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRIf, 0);
            } else if (action->priority <= BCM_PRIO_MAX){
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IRPEf, 1);
                soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r, &rval, IPRIf,
                              action->priority);
            } else {
                rv = BCM_E_PARAM;
                goto error;
            }
        }
    
        rv = WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval);
        if (rv < 0) {
            goto error;
        }
        /* Delete the old profile only if it is different the a new one. */
        if (profile_idx != old_profile_idx) {
            BCM_IF_ERROR_RETURN
                (_bcm_trx_egr_vlan_action_profile_entry_delete(unit, old_profile_idx));
        }

        return BCM_E_NONE;

error:
        /* Undo action profile entry addition */
        BCM_IF_ERROR_RETURN
            (_bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx));
        return rv;
    }
}

/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_get(int unit, bcm_port_t port,
                                            bcm_vlan_action_set_t *action)
{
    uint32 profile_idx, rval;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return(
            _bcm_th3_vlan_port_egress_default_action_get(unit, port, action));
    }
#endif/* BCM_TOMAHAWK3_SUPPORT */

    bcm_vlan_action_set_t_init(action);
#if defined(BCM_TOMAHAWK_SUPPORT)
    /* Tomahawk low latency mode */
    if (soc_feature(unit, soc_feature_fast_egr_vlan_action)) {
        rval = _bcm_fast_egr_vlan_port_egress_default_action_get(unit,
                                                               port, action);
        return rval;
    }
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_egr_vlan_control_is_memory)){
        return _bcm_td2p_vlan_port_egress_default_action_get(unit, port,
                action);
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
        profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                    rval, TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_egr_vlan_action_profile_entry_get(unit, action, profile_idx);
        action->new_inner_vlan = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                                               rval, IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (!soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r, rval,
                               IPRI_CFI_SELf)) {
                action->new_inner_pkt_prio =
                    soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r, rval, IPRIf);
                action->new_inner_cfi =
                    soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r, rval, ICFIf);
            }
        }

        BCM_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_2r(unit, port, &rval));
        action->new_outer_vlan = soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r,
                                               rval, OVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (!soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval,
                               OPRI_CFI_SELf)) {
                action->priority =
                    soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OPRIf);
                action->new_outer_cfi =
                    soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, OCFIf);
            }
        } else {
            if (soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r, rval, ORPEf)) {
                action->priority = soc_reg_field_get(unit, EGR_VLAN_CONTROL_2r,
                                                 rval, OPRIf);
            } else {
                action->priority = -1;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr_vlan_port_egress_default_action_delete
 * Purpose:
 *      Deletes the egress port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_egress_default_action_delete(int unit, bcm_port_t port)
{
    uint32                  profile_idx, rval;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_egr_vlan_control_is_memory)){
        return _bcm_td2p_vlan_port_egress_default_action_delete(unit, port);
    } else
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return(_bcm_th3_vlan_port_egress_default_action_delete(unit, port));
    } else
#endif/* BCM_TOMAHAWK3_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(
                READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
        profile_idx = soc_reg_field_get(unit, EGR_VLAN_CONTROL_3r,
                rval, TAG_ACTION_PROFILE_PTRf);

        if (!SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_trx_egr_vlan_action_profile_entry_delete(unit, profile_idx));
        }
        sal_memset(&rval, 0, sizeof(uint32));
        BCM_IF_ERROR_RETURN(
                WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval));
        BCM_IF_ERROR_RETURN(
                WRITE_EGR_VLAN_CONTROL_2r(unit, port, rval));
    }

    return BCM_E_NONE;
}

int
_bcm_trx_vlan_port_egress_action_profile_clear(int unit, bcm_port_t port)
{
    uint32                  rval;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
        return _bcm_td2p_vlan_port_egress_action_profile_clear(unit, port);
    } else
#endif
    {
        BCM_IF_ERROR_RETURN(
                READ_EGR_VLAN_CONTROL_3r(unit, port, &rval));
        soc_reg_field_set(unit, EGR_VLAN_CONTROL_3r,
                &rval, TAG_ACTION_PROFILE_PTRf, 0);

        BCM_IF_ERROR_RETURN(
                WRITE_EGR_VLAN_CONTROL_3r(unit, port, rval));
    }

    return BCM_E_NONE;
}

#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * Function:
 *      _bcm_fast_egr_vlan_port_egress_default_action_set
 * Purpose:
 *      Set the egress port's default vlan tag actions in Low latency mode
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fast_egr_vlan_port_egress_default_action_set(
            int unit,
            bcm_port_t port,
            bcm_vlan_action_set_t *action)
{
    uint32 rval;

    /* Verify vlan_action */
    if (NULL == action)
        return BCM_E_PARAM;

    switch(action->ot_outer) {
    case bcmVlanActionNone:
    case bcmVlanActionDelete:
        break;
    default:
        return BCM_E_PARAM;
    }
    switch(action->ot_outer_prio) {
    case bcmVlanActionNone:
    case bcmVlanActionDelete:
        break;
    default:
        return BCM_E_PARAM;
    }
    switch(action->ut_outer) {
    case bcmVlanActionNone:
        break;
    default:
        return BCM_E_PARAM;
    }

    /* Check for non-zero value on other fields */
    if ((action->new_outer_vlan) | (action->new_inner_vlan) | (action->new_inner_pkt_prio)
        | (action->new_outer_cfi) | (action->new_inner_cfi) | (action->ingress_if)
        | (action->priority) | (action->dt_outer) | (action->dt_outer_prio)
        | (action->dt_outer_pkt_prio) | (action->dt_outer_cfi) | (action->dt_inner)) {
        return BCM_E_UNAVAIL;
    }
    if ((action->dt_inner_prio) | (action->dt_inner_pkt_prio) | (action->dt_inner_cfi)
        | (action->ot_outer_pkt_prio) | (action->ot_outer_cfi) | (action->ot_inner)
        | (action->ot_inner_pkt_prio) | (action->ot_inner_cfi) | (action->it_outer)
        | (action->it_outer_pkt_prio) | (action->it_outer_cfi) | (action->it_inner)
        | (action->it_inner_prio) | (action->it_inner_pkt_prio) | (action->it_inner_cfi)) {
        return BCM_E_UNAVAIL;
    }
    if ((action->ut_outer_pkt_prio) | (action->ut_outer_cfi) | (action->ut_inner)
        | (action->ut_inner_pkt_prio) | (action->ut_inner_cfi) | (action->outer_pcp)
        | (action->inner_pcp) | (action->policer_id) | (action->outer_tpid)
        | (action->inner_tpid) | (action->outer_tpid_action) | (action->inner_tpid_action)
        | (action->action_id) | (action->class_id) | (action->flags)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(
        READ_EGR_VLAN_TAG_ACTION_FOR_BYPASSr(unit, port, &rval));

    soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr, &rval,
        SOT_OTAG_ACTIONf, _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer));
    soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr, &rval,
        SOT_POTAG_ACTIONf, _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ot_outer_prio));
    soc_reg_field_set(unit, EGR_VLAN_TAG_ACTION_FOR_BYPASSr, &rval,
        UT_OTAG_ACTIONf, _BCM_TRX_PRI_CFI_ACTION_ENCODE(action->ut_outer));

    SOC_IF_ERROR_RETURN(
                WRITE_EGR_VLAN_TAG_ACTION_FOR_BYPASSr(unit, port, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fast_egr_vlan_port_egress_default_action_get
 * Purpose:
 *      Get the egress port's default vlan tag actions in Low latency mode
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_fast_egr_vlan_port_egress_default_action_get(
            int unit,
            bcm_port_t port,
            bcm_vlan_action_set_t *action)
{
    uint32 rval;

    /* Verify output param */
    if (NULL == action)
        return BCM_E_PARAM;

    SOC_IF_ERROR_RETURN(
        READ_EGR_VLAN_TAG_ACTION_FOR_BYPASSr(unit, port, &rval));

    action->ot_outer = _BCM_TRX_PRI_CFI_ACTION_ENCODE(soc_reg_field_get(unit,
        EGR_VLAN_TAG_ACTION_FOR_BYPASSr, rval, SOT_OTAG_ACTIONf));
    action->ot_outer_prio = _BCM_TRX_PRI_CFI_ACTION_ENCODE(soc_reg_field_get(unit,
        EGR_VLAN_TAG_ACTION_FOR_BYPASSr, rval, SOT_POTAG_ACTIONf));
    action->ut_outer = _BCM_TRX_PRI_CFI_ACTION_ENCODE(soc_reg_field_get(unit,
        EGR_VLAN_TAG_ACTION_FOR_BYPASSr, rval, UT_OTAG_ACTIONf));

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *		_bcm_tr_vlan_translate_entry_update
 * Purpose:
 *		Update VLAN XLATE Entry 
 * Parameters:
 *  IN :  Unit
 *  IN :  vlan_xlate_entry
 *  OUT :  vlan_xlate_entry

 */

STATIC int
_bcm_tr_vlan_translate_entry_update(int unit, void *vent, void *return_ent)
{
    uint32  vp, key_type, value;
    soc_mem_t mem = VLAN_XLATEm;

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
    }

    /* Check if Key_Type identical */
    key_type = soc_mem_field32_get(unit, mem, vent, KEY_TYPEf);
    value = soc_mem_field32_get(unit, mem, return_ent, KEY_TYPEf);
    if (key_type != value) {
         return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, return_ent, MPLS_ACTIONf, 0x1);
    soc_mem_field32_set(unit, mem, return_ent, DISABLE_VLAN_CHECKSf, 1);
    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_VPf)) {
        vp = soc_mem_field32_get(unit, mem, vent, SOURCE_VPf);
        soc_mem_field32_set(unit, mem, return_ent, SOURCE_VPf, vp);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *              _bcm_tr3_vlan_translate_entry_update
 * Purpose:
 *              Update VLAN XLATE Entry
 * Parameters:  
 *  IN :  Unit
 *  IN :  vlan_xlate_extd_entry
 *  OUT : vlan_xlate_extd_entry
 */

#if defined(BCM_TRIUMPH3_SUPPORT)
STATIC int
_bcm_tr3_vxlate_extd_entry_vp_update(int unit, 
               vlan_xlate_entry_t *vent_in,  /* update entry */
               void *ctxt,
               vlan_xlate_entry_t *vent_out, /* entry read from mem */
               vlan_xlate_extd_entry_t *vxent_out, /* entry read from mem or
                                      converted from vent_out if not null */
               int *use_extd_tbl)
{
    uint32  vp;
  
    vp = PTR_TO_INT(ctxt);
 
    if (vent_out) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate2vxlate_extd(unit,vent_out,vxent_out));
    }

    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxent_out,XLATE__MPLS_ACTIONf, 0x1);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxent_out, DISABLE_VLAN_CHECKSf, 1);
    soc_VLAN_XLATE_EXTDm_field32_set(unit, vxent_out, SOURCE_VPf, vp);
    *use_extd_tbl = TRUE;
    return BCM_E_NONE;
}
#endif


int
_bcm_tr_vlan_translate_vp_add(int unit,
                               bcm_gport_t port,
                               bcm_vlan_translate_key_t key_type,
                               bcm_vlan_t outer_vlan,
                               bcm_vlan_t inner_vlan,
                               int vp, bcm_vlan_action_set_t *action,
                               int *existing_updated)
{
    int rv, gport_id;
    vlan_xlate_entry_t vx_ent, vx_ent_old;
    vlan_xlate_1_double_entry_t vx1d_ent, vx1d_ent_old;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int idx;
    int key_type_value;
    uint32 vxlate_action_profile_idx;
    uint32 vxlate_action_profile_idx_old;
   
    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    void *vent_old = &vx_ent_old;
    uint32 ent_sz = sizeof(vx_ent);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        vent_old = &vx1d_ent_old;
        ent_sz = sizeof(vx1d_ent);
    }

    rv = BCM_E_NONE; 
    sal_memset(vent, 0, ent_sz);
    switch (key_type) {
        case bcmVlanTranslateKeyPortOuter:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortInner:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyPortDouble:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyPortOuterPri:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PRI_CFI,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            /* outer_vlan: Bits 12-15 contains VLAN_PRI + CFI, vlan=BCM_E_NONE */
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortOuterTag:
            /* allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OTAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, vent, Tf, 1);
        soc_mem_field32_set(unit, mem, vent, TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, PORT_NUMf, port_out);
    }
        
    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_VPf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_VPf, vp);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
        /* set default vlan_xlate key source_type as SGLP */
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }

    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

    if (action) {
        vxlate_action_profile_idx = 0;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_verify(unit, action));
        vxlate_action_profile_idx_old = 
         soc_mem_field32_get(unit, mem, vent, TAG_ACTION_PROFILE_PTRf);
        /* Add new tag action profile table ptr */
        BCM_IF_ERROR_RETURN
          (_bcm_trx_vlan_action_profile_entry_add(unit, action, 
                                  &vxlate_action_profile_idx));
        soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf,
                                              vxlate_action_profile_idx);
        /* Delete old tag action profile table ptr expect entry zero
         * It contains initialize cache hardware ING_VLAN_TAG_ACTION_PROFILE
         * and EGR_VLAN_TAG_ACTION_PROFILE tables in RAM.
         * ING_VLAN profile contains PORT_TAB and VLAN_PROTOCOL_DATA
         * EGR_VLAN profile contains all ports, sub ports and vlan subnets */
        if (vxlate_action_profile_idx_old != ING_ACTION_PROFILE_DEFAULT) {
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_action_profile_entry_delete(unit,
                            vxlate_action_profile_idx_old));
        }
        /* Set new OVID and IVID from the 'action' specified */
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf, action->new_outer_vlan);
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, action->new_inner_vlan);

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_feature(unit, soc_feature_global_meter) &&
            (SOC_IS_KATANAX(unit) || SOC_IS_APACHE(unit) ||
             SOC_IS_GREYHOUND2(unit))) {
            rv = _bcm_esw_add_policer_to_table(unit, action->policer_id,
                                               mem,0, vent);
            BCM_IF_ERROR_RETURN(rv);
        }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_tr3_vxlate_entry_add(unit,vent,
                          (void *)INT_TO_PTR(vp),
                          _bcm_tr3_vxlate_extd_entry_vp_update,
                          existing_updated));
    } else
#endif
    {
    soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x1); /* SVP */
    if (action) {
        if (action->flags & BCM_VLAN_ACTION_SET_VLAN_CHECKS_DISABLE) {
            soc_mem_field32_set(unit, mem, vent, DISABLE_VLAN_CHECKSf, 1);
        }
    }
    if (SOC_MEM_FIELD_VALID(unit, mem, VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VLAN_ACTION_VALIDf, 1);
    }

    /* look up existing entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, vent, vent_old, 0);
    if(rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr_vlan_translate_entry_update (unit, vent, vent_old));
         rv = soc_mem_write(unit, mem,
                                       MEM_BLOCK_ALL, idx, vent_old);
        if (existing_updated != NULL) {
            *existing_updated = 1;
        }
        soc_mem_unlock(unit, mem);
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;
    } 

    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent);
    soc_mem_unlock(unit, mem);
    }
    return rv;
}

int
_bcm_tr_lltag_vlan_translate_vp_add(int unit,
                               bcm_gport_t port,
                               bcm_vlan_translate_key_t key_type,
                               bcm_vlan_t lltag_vlan,
                               bcm_vlan_t other_vlan,
                               int vp, bcm_vlan_action_set_t *action)
{
    int rv, gport_id;
    vlan_xlate_entry_t vx_ent, vx_ent_old;
    vlan_xlate_1_double_entry_t v1dent, v1dent_old;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int idx;
    int key_type_value;
    uint32 vxlate_action_profile_idx;
    uint32 vxlate_action_profile_idx_old;
    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    void *vent_old = &vx_ent_old;
    uint32 ent_sz = sizeof(vx_ent);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &v1dent;
        vent_old = &v1dent_old;
        ent_sz = sizeof(v1dent);
    }
   
    rv = BCM_E_NONE; 

    if (!soc_feature(unit, soc_feature_lltag)) {
        return BCM_E_UNAVAIL;
    }
        
    sal_memset(vent, 0, ent_sz);
    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_LLVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_LLVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            soc_mem_field32_set(unit, mem, vent, LLTAG__OVIDf, other_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelInner:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASK_KEY_TYPE_LLVID_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            soc_mem_field32_set(unit, mem, vent, LLTAG__IVIDf, other_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, vent, Tf, 1);
        soc_mem_field32_set(unit, mem, vent, TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, PORT_NUMf, port_out);
    }
        
    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_VPf)) {
        soc_mem_field32_set(unit, mem, vent, SOURCE_VPf, vp);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
        /* set default vlan_xlate key source_type as SGLP */
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }

    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

    if (action) {
        vxlate_action_profile_idx = 0;
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_verify(unit, action));
        vxlate_action_profile_idx_old = 
         soc_mem_field32_get(unit, mem, vent, TAG_ACTION_PROFILE_PTRf);
        /* Add new tag action profile table ptr */
        BCM_IF_ERROR_RETURN
          (_bcm_trx_vlan_action_profile_entry_add(unit, action, 
                                  &vxlate_action_profile_idx));
        soc_mem_field32_set(unit, mem, vent, TAG_ACTION_PROFILE_PTRf,
                                              vxlate_action_profile_idx);
        /* Delete old tag action profile table ptr expect entry zero
         * It contains initialize cache hardware ING_VLAN_TAG_ACTION_PROFILE
         * and EGR_VLAN_TAG_ACTION_PROFILE tables in RAM.
         * ING_VLAN profile contains PORT_TAB and VLAN_PROTOCOL_DATA
         * EGR_VLAN profile contains all ports, sub ports and vlan subnets
         */
        if (vxlate_action_profile_idx_old != 0) {
            BCM_IF_ERROR_RETURN
              (_bcm_trx_vlan_action_profile_entry_delete(unit,
                              vxlate_action_profile_idx_old));
        }
        /* Set new OVID and IVID from the 'action' specified */
        soc_mem_field32_set(unit, mem, vent, NEW_OVIDf, action->new_outer_vlan);
        soc_mem_field32_set(unit, mem, vent, NEW_IVIDf, action->new_inner_vlan);
    }

    soc_mem_field32_set(unit, mem, vent, MPLS_ACTIONf, 0x1); /* SVP */
    soc_mem_field32_set(unit, mem, vent, DISABLE_VLAN_CHECKSf, 1);
    if (SOC_MEM_FIELD_VALID(unit, mem, VLAN_ACTION_VALIDf)) {
        soc_mem_field32_set(unit, mem, vent, VLAN_ACTION_VALIDf, 1);
    }

    /* look up existing entry */
    soc_mem_lock(unit, mem);
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ALL, &idx, vent, vent_old, 0);
    if(rv == SOC_E_NONE) {
         BCM_IF_ERROR_RETURN(
              _bcm_tr_vlan_translate_entry_update (unit, vent, vent_old));
         rv = soc_mem_write(unit, mem,
                                       MEM_BLOCK_ALL, idx, vent_old);
        soc_mem_unlock(unit, mem);
        return BCM_E_EXISTS;
    } else if (rv != SOC_E_NOT_FOUND) {
        soc_mem_unlock(unit, mem);
        return rv;
    } 

    rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, vent);
    soc_mem_unlock(unit, mem);
   
    return rv;
}


int
_bcm_tr_vlan_translate_vp_delete(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan,
                                  bcm_vlan_t inner_vlan,
                                  int vp)
{
    int rv, gport_id;
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int key_type_value;

    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    uint32 ent_sz = sizeof(vx_ent);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
    vlan_xlate_entry_t current_vent;
    bcm_policer_t current_policer = 0;
    int idx;
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */
 
    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        ent_sz = sizeof(vx1d_ent);
    }

    sal_memset(vent, 0, ent_sz);
    switch (key_type) {
        case bcmVlanTranslateKeyPortOuter:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortInner:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
        case bcmVlanTranslateKeyPortDouble:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_IVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OVIDf, outer_vlan);
            soc_mem_field32_set(unit, mem, vent, IVIDf, inner_vlan);
            break;
       case bcmVlanTranslateKeyPortOuterPri:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_PRI_CFI,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        case bcmVlanTranslateKeyPortOuterTag:
            /* allow 16 bit VLAN id */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_OTAG,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, OTAGf, outer_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, vent, Tf, 1);
        soc_mem_field32_set(unit, mem, vent, TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, PORT_NUMf, port_out);
    }

    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
        /* set default vlan_xlate key source_type as SGLP */
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_APACHE_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
    if (soc_feature(unit, soc_feature_global_meter) &&
        (SOC_IS_KATANAX(unit) || SOC_IS_APACHE(unit) ||
         SOC_IS_GREYHOUND2(unit))) {
        rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &idx, vent, &current_vent, 0);
        if (rv == BCM_E_NONE) {
            rv = _bcm_esw_get_policer_from_table(unit, mem, 0, &current_vent,
                                                        &current_policer, 1);
            if (current_policer) {
                rv = _bcm_esw_policer_decrement_ref_count(unit,
                                                        current_policer);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
    }
#endif /* BCM_KATANA_SUPPORT || BCM_APACHE_SUPPORT ||
          BCM_GREYHOUND2_SUPPORT */

    soc_mem_lock(unit, mem);
    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
    soc_mem_unlock(unit, mem);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        vlan_xlate_extd_entry_t vxent;

        if (rv == BCM_E_NOT_FOUND) { /* serarch for extend table */
            sal_memset(&vxent, 0, sizeof(vxent));
            BCM_IF_ERROR_RETURN
                (_bcm_tr3_vxlate2vxlate_extd(unit,vent,&vxent));
            soc_mem_lock(unit, VLAN_XLATE_EXTDm);
            rv = soc_mem_delete(unit, VLAN_XLATE_EXTDm, MEM_BLOCK_ANY, &vxent);  
            soc_mem_unlock(unit, VLAN_XLATE_EXTDm);
            return rv;
        } 
    }
#endif
    return rv;
}

int
_bcm_tr_lltag_vlan_translate_vp_delete(int unit,
                                  bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t lltag_vlan,
                                  bcm_vlan_t other_vlan,
                                  int vp)
{
    int rv, gport_id;
    vlan_xlate_entry_t vx_ent;
    vlan_xlate_1_double_entry_t vx1d_ent;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    bcm_trunk_t trunk_id;
    int key_type_value;

    soc_mem_t mem = VLAN_XLATEm;
    soc_field_t vf = VALIDf;
    void *vent = &vx_ent;
    uint32 ent_sz = sizeof(vx_ent);

    if (SOC_MEM_IS_VALID(unit, VLAN_XLATE_1_DOUBLEm)) {
        mem = VLAN_XLATE_1_DOUBLEm;
        vf = BASE_VALID_0f; /* TD3TBD */
        vent = &vx1d_ent;
        ent_sz = sizeof(vx1d_ent);
    }

    if (!soc_feature(unit, soc_feature_lltag)) {
        return BCM_E_UNAVAIL;
    }
    
    sal_memset(vent, 0, ent_sz);
    switch (key_type) {
        case bcmVlanTranslateKeyPortPonTunnel:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_LLVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelOuter:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASH_KEY_TYPE_LLVID_OVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            soc_mem_field32_set(unit, mem, vent, LLTAG__OVIDf, other_vlan);
            break;
        case bcmVlanTranslateKeyPortPonTunnelInner:
            BCM_IF_ERROR_RETURN
                (_bcm_esw_vlan_xlate_key_type_value_get(unit,
                       VLXLT_HASK_KEY_TYPE_LLVID_IVID,
                       &key_type_value));
            soc_mem_field32_set(unit, mem, vent, KEY_TYPEf, key_type_value);
            if (soc_mem_field_valid(unit, mem, DATA_TYPEf)) {
                soc_mem_field32_set(unit, mem, vent, DATA_TYPEf,
                                        key_type_value);
            }
            soc_mem_field32_set(unit, mem, vent, LLTAG__LLVIDf, lltag_vlan);
            soc_mem_field32_set(unit, mem, vent, LLTAG__OVIDf, other_vlan);
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        soc_mem_field32_set(unit, mem, vent, Tf, 1);
        soc_mem_field32_set(unit, mem, vent, TGIDf, trunk_id);
    } else {
        soc_mem_field32_set(unit, mem, vent, MODULE_IDf, mod_out);
        soc_mem_field32_set(unit, mem, vent, PORT_NUMf, port_out);
    }

    if (mem == VLAN_XLATEm) {
        soc_mem_field32_set(unit, mem, vent, vf, 1);
    } else {
        soc_mem_field32_set(unit, mem, vent, vf, 3);
        soc_mem_field32_set(unit, mem, vent, BASE_VALID_1f, 7);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_TYPEf)) {
        /* set default vlan_xlate key source_type as SGLP */
        soc_mem_field32_set(unit, mem, vent, SOURCE_TYPEf, 1);
    }

    soc_mem_lock(unit, mem);
    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, vent);
    soc_mem_unlock(unit, mem);

    return rv;
}


#ifdef BCM_TRIUMPH2_SUPPORT
#define _BCM_OUTER_VLAN_RANGE_TYPE 0
#define _BCM_INNER_VLAN_RANGE_TYPE 1

int
_bcm_tr2_vlan_tranlate_range_data_insert(bcm_vlan_t vlan_low, bcm_vlan_t vlan_high, 
                                         bcm_vlan_t *min_vlan, bcm_vlan_t *max_vlan)
{
    int i = 0;
    /* Find the first unused min/max range. Unused ranges are
     * identified by { min == 1, max == 0 }
     */
    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
        if ((min_vlan[i] == 1) && (max_vlan[i] == 0)) {
            break;
        } else if (min_vlan[i] == vlan_low) {
            /* Can't have multiple ranges with the same min */
            return BCM_E_EXISTS;
        }
    }
    if (i == BCM_VLAN_RANGE_NUM) {
        /* All ranges are taken */
        return BCM_E_FULL;
    }

    /* Insert the new range into the table entry sorted by min VID */
    for ( ; i > 0 ; i--) {
        if (min_vlan[i - 1] > vlan_low) {
            /* Move existing min/max down */
            min_vlan[i] = min_vlan[i - 1];
            max_vlan[i] = max_vlan[i - 1];
        } else {
            break;
        }
    }
    min_vlan[i] = vlan_low;
    max_vlan[i] = vlan_high;
    return BCM_E_NONE;
}

int 
_bcm_tr2_vlan_tranlate_range_data_delete(bcm_vlan_t vlan_low, bcm_vlan_t vlan_high, 
                                         bcm_vlan_t *min_vlan, bcm_vlan_t *max_vlan)
{
    int i = 0;
    /* Find the min/max range. */
    for (i = 0; i < BCM_VLAN_RANGE_NUM; i++) {
        if ((min_vlan[i] == vlan_low) && (max_vlan[i] == vlan_high)) {
            break;
        }
    }
    if (i == BCM_VLAN_RANGE_NUM) {
        return BCM_E_NOT_FOUND;
    }

    /* Remove the range from the table entry and fill in the gap */
    for (; i < 7; i++) {
        /* Move existing min/max UP */
        min_vlan[i] = min_vlan[i + 1];
        max_vlan[i] = max_vlan[i + 1];
    }
    /* Mark last min/max range as unused. Unused ranges are
     * identified by { min == 1, max == 0 }
     */
    min_vlan[i] = 1;
    max_vlan[i] = 0;
    return BCM_E_NONE;
}


int 
_bcm_tr2_port_vlan_translate_range_profile_update(int unit, bcm_vlan_t *min_vlan, bcm_vlan_t *max_vlan, 
                                                  bcm_module_t module, bcm_port_t port, int vt_range_type, 
                                                  uint32 *new_idx)
{
    int rv = BCM_E_NONE;
    uint32 new_idx_local;
    int stm_idx;
        rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan,
            &new_idx_local);
        if (BCM_FAILURE(rv)) {
        return rv;
        }
    rv = _bcm_esw_src_mod_port_table_index_get(unit, module, port, &stm_idx);
        if (BCM_SUCCESS(rv)) {
            soc_field_t field;
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    OUTER_VLAN_RANGE_IDXf) & (vt_range_type == _BCM_OUTER_VLAN_RANGE_TYPE)) {
                field = OUTER_VLAN_RANGE_IDXf;
        } else if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    INNER_VLAN_RANGE_IDXf) & (vt_range_type == _BCM_INNER_VLAN_RANGE_TYPE)) {
            field = INNER_VLAN_RANGE_IDXf;
            } else {
                field = VLAN_RANGE_IDXf;
            }
            rv = soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm, stm_idx,
                field, new_idx_local);
        }
        if (BCM_FAILURE(rv)) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx_local);
        return rv;
    }
    *new_idx = new_idx_local;
    return rv;
}

int
_bcm_tr2_vlan_translate_range_vp_add(int unit,
                                     bcm_gport_t port,
                                     bcm_vlan_t outer_vlan_low,
                                     bcm_vlan_t outer_vlan_high,
                                     bcm_vlan_t inner_vlan_low,
                                     bcm_vlan_t inner_vlan_high,                                     
                                     int vp,
                                     bcm_vlan_action_set_t *action)
{
    int i, key_type = 0, rv, gport_id, old_idx = -1, old_idx_inner = -1, stm_idx = 0;
    uint32 new_idx = -1, new_idx_inner = -1;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id, tid;
    bcm_vlan_t min_vlan[BCM_VLAN_RANGE_NUM], max_vlan[BCM_VLAN_RANGE_NUM];
    int port_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_module_t *mod_array = NULL;
    bcm_port_t   *port_array = NULL;
    int trunk128;
    if (outer_vlan_low != BCM_VLAN_INVALID && 
        inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
        if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 0, NULL, &port_count));

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * port_count,
                "trunk member array");
        if (member_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * port_count);

        rv = bcm_esw_trunk_get(unit, trunk_id, NULL, port_count,
                member_array, &port_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(mod_array, 0, sizeof(bcm_module_t) * port_count);

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(port_array, 0, sizeof(bcm_port_t) * port_count);

        for (i = 0; i < port_count; i++) {
            rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                    &mod_array[i], &port_array[i], &tid, &gport_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }

        port_count = 1;

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        mod_array[0] = mod_out;

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        port_array[0] = port_out;
    }

    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[0],
            port_array[0], &stm_idx);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);

    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }
    if (outer_vlan_low != BCM_VLAN_INVALID && outer_vlan_high != BCM_VLAN_INVALID) {
        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    OUTER_VLAN_RANGE_IDXf)) {
            old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, OUTER_VLAN_RANGE_IDXf);
        } else {
            old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, VLAN_RANGE_IDXf);
        }

        /* Get the profile table entry for this port/trunk */
        _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx);

        rv = _bcm_tr2_vlan_tranlate_range_data_insert(outer_vlan_low, outer_vlan_high, 
                                                     min_vlan, max_vlan);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
        /* Adding the new profile table entry and update profile pointer */
        for (i = 0; i < port_count; i++) {
            rv = _bcm_tr2_port_vlan_translate_range_profile_update(unit, min_vlan, 
                                                                   max_vlan, mod_array[i], 
                                                                   port_array[i], 
                                                                   _BCM_OUTER_VLAN_RANGE_TYPE, 
                                                                   &new_idx);
            if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
    }
    }
    if (inner_vlan_low != BCM_VLAN_INVALID && inner_vlan_high != BCM_VLAN_INVALID) {
        if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    INNER_VLAN_RANGE_IDXf)) {
            old_idx_inner = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, INNER_VLAN_RANGE_IDXf);
        } else {
            old_idx_inner = -1;
        }
        if (old_idx_inner >= 0 && inner_vlan_low != BCM_VLAN_INVALID && inner_vlan_high != BCM_VLAN_INVALID) {
            /* Get the profile table entry for this port/trunk */
            _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx_inner);

            rv =_bcm_tr2_vlan_tranlate_range_data_insert(inner_vlan_low, inner_vlan_high, 
                                                         min_vlan, max_vlan);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
            }
            /* Adding the new profile table entry and update profile pointer */
            for (i = 0; i < port_count; i++) {
                rv = _bcm_tr2_port_vlan_translate_range_profile_update(unit, min_vlan, 
                                                                       max_vlan, mod_array[i], 
                                                                       port_array[i], 
                                                                       _BCM_INNER_VLAN_RANGE_TYPE, 
                                                                       &new_idx_inner);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
            }
        }
    }
    if (BCM_GPORT_IS_TRUNK(port)) {
        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 1);
            if (!trunk128) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan,
                        &new_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = soc_mem_field32_modify(unit, TRUNK32_PORT_TABLEm, trunk_id, 
                        VLAN_RANGE_IDXf, new_idx);
                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
            }
        }
    }

    /* Add an entry in the vlan translate table for the low VID */
    rv = _bcm_tr_vlan_translate_vp_add(unit, port, key_type, 
                                        outer_vlan_low, inner_vlan_low,
                                        vp, action, NULL);
    if (rv != BCM_E_NONE) {
        for (i = 0; i < port_count; i++) {
            /* coverity[unsigned_compare] */
            if (new_idx >= 0) {
            _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        }
            /* coverity[unsigned_compare] */
            if (new_idx_inner >= 0) {
                _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx_inner);
            }
        }
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }

    /* Delete the old profile entry */
    for (i = 0; i < port_count; i++) {
        if (new_idx >= 0) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
    }
        if (new_idx_inner >= 0) {
            _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx_inner);
        }
    }
    soc_mem_unlock(unit, ING_VLAN_RANGEm);

cleanup:
    if (NULL != member_array) {
        sal_free(member_array);
    }
    if (NULL != mod_array) {
        sal_free(mod_array);
    }
    if (NULL != port_array) {
        sal_free(port_array);
    }

    return rv;
}

int
_bcm_tr2_vlan_translate_range_vp_delete(int unit,
                                        bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low,
                                        bcm_vlan_t outer_vlan_high,
                                        bcm_vlan_t inner_vlan_low,
                                        bcm_vlan_t inner_vlan_high,                                        
                                        int vp)
{
    int i, key_type = 0, rv, gport_id, old_idx = -1, old_idx_inner= -1, stm_idx = 0;
    uint32 new_idx = 0, new_idx_inner = 0;
    source_trunk_map_table_entry_t stm_entry;
    bcm_module_t mod_out;
    bcm_port_t   port_out;
    bcm_trunk_t  trunk_id, tid;
    bcm_vlan_t min_vlan[BCM_VLAN_RANGE_NUM], max_vlan[BCM_VLAN_RANGE_NUM];
    int port_count;
    bcm_trunk_member_t *member_array = NULL;
    bcm_module_t *mod_array = NULL;
    bcm_port_t   *port_array = NULL;
    int trunk128;

    if (outer_vlan_low != BCM_VLAN_INVALID && 
        inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortDouble;
    } else if (outer_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortOuter;
    } else if (inner_vlan_low != BCM_VLAN_INVALID) {
        key_type = bcmVlanTranslateKeyPortInner;
    } else {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, port, &mod_out, &port_out, &trunk_id,
                                &gport_id));

    if (BCM_GPORT_IS_TRUNK(port)) {
       if (BCM_TRUNK_INVALID == trunk_id) {
            return BCM_E_PORT;
        }

        BCM_IF_ERROR_RETURN
            (bcm_esw_trunk_get(unit, trunk_id, NULL, 0, NULL, &port_count));

        member_array = sal_alloc(sizeof(bcm_trunk_member_t) * port_count,
                "trunk member array");
        if (member_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(member_array, 0, sizeof(bcm_trunk_member_t) * port_count);

        rv = bcm_esw_trunk_get(unit, trunk_id, NULL, port_count,
                member_array, &port_count);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(mod_array, 0, sizeof(bcm_module_t) * port_count);

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        sal_memset(port_array, 0, sizeof(bcm_port_t) * port_count);

        for (i = 0; i < port_count; i++) {
            rv = _bcm_esw_gport_resolve(unit, member_array[i].gport,
                    &mod_array[i], &port_array[i], &tid, &gport_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
    } else {
        if ((-1 == mod_out) || (-1 == port_out)) {
            return BCM_E_PORT;
        }

        port_count = 1;

        mod_array = sal_alloc(sizeof(bcm_module_t) * port_count,
                "module ID array");
        if (mod_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        mod_array[0] = mod_out;

        port_array = sal_alloc(sizeof(bcm_port_t) * port_count,
                "port ID array");
        if (port_array == NULL) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }
        port_array[0] = port_out;
    }

    rv = _bcm_esw_src_mod_port_table_index_get(unit, mod_array[0],
            port_array[0], &stm_idx);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    soc_mem_lock(unit, ING_VLAN_RANGEm);
    rv = READ_SOURCE_TRUNK_MAP_TABLEm(unit, MEM_BLOCK_ANY, stm_idx, &stm_entry);
    if (rv < 0) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }
    if (outer_vlan_low != BCM_VLAN_INVALID && outer_vlan_high != BCM_VLAN_INVALID) {
    if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                OUTER_VLAN_RANGE_IDXf)) {
            old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                          &stm_entry, OUTER_VLAN_RANGE_IDXf);
    } else {
            old_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                          &stm_entry, VLAN_RANGE_IDXf);
    }

    /* Get the profile table entry for this port/trunk */
    _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx);

        rv = _bcm_tr2_vlan_tranlate_range_data_delete(outer_vlan_low, outer_vlan_high, 
                                                     min_vlan, max_vlan);
        if (BCM_FAILURE(rv)) {
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }
    /* Adding the new profile table entry and update profile pointer */
    for (i = 0; i < port_count; i++) {
            rv = _bcm_tr2_port_vlan_translate_range_profile_update(unit, min_vlan, 
                                                                   max_vlan, mod_array[i], 
                                                                   port_array[i], 
                                                                   _BCM_OUTER_VLAN_RANGE_TYPE, 
                &new_idx);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
        }
    }
    if (inner_vlan_low != BCM_VLAN_INVALID && inner_vlan_high != BCM_VLAN_INVALID) {
            if (SOC_MEM_FIELD_VALID(unit, SOURCE_TRUNK_MAP_TABLEm,
                    INNER_VLAN_RANGE_IDXf)) {
            old_idx_inner = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_TABLEm,
                                      &stm_entry, INNER_VLAN_RANGE_IDXf);
            } else {
            old_idx_inner = -1;
            }
        if (old_idx_inner >= 0 && inner_vlan_low != BCM_VLAN_INVALID && inner_vlan_high != BCM_VLAN_INVALID) {
            /* Get the profile table entry for this port/trunk */
            _bcm_trx_vlan_range_profile_entry_get(unit, min_vlan, max_vlan, old_idx_inner);

            rv =_bcm_tr2_vlan_tranlate_range_data_delete(inner_vlan_low, inner_vlan_high, 
                                                         min_vlan, max_vlan);
            if (BCM_FAILURE(rv)) {
                soc_mem_unlock(unit, ING_VLAN_RANGEm);
                goto cleanup;
        }
            /* Adding the new profile table entry and update profile pointer */
            for (i = 0; i < port_count; i++) {
                rv = _bcm_tr2_port_vlan_translate_range_profile_update(unit, min_vlan, 
                                                                       max_vlan, mod_array[i], 
                                                                       port_array[i], 
                                                                       _BCM_INNER_VLAN_RANGE_TYPE, 
                                                                       &new_idx_inner);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, ING_VLAN_RANGEm);
            goto cleanup;
        }
    }
        }
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        if (SOC_MEM_FIELD_VALID(unit, TRUNK32_PORT_TABLEm, VLAN_RANGE_IDXf)) {
            trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 1);
            if (!trunk128) {
                rv = _bcm_trx_vlan_range_profile_entry_add(unit, min_vlan, max_vlan,
                        &new_idx);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                rv = soc_mem_field32_modify(unit, TRUNK32_PORT_TABLEm, trunk_id, 
                        VLAN_RANGE_IDXf, new_idx);
                if (BCM_FAILURE(rv)) {
                    _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
                    soc_mem_unlock(unit, ING_VLAN_RANGEm);
                    goto cleanup;
                }
                _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
            }
        }
    }

    /* Delete the entry from the vlan translate table for the low VID */
    rv = _bcm_tr_vlan_translate_vp_delete(unit, port, key_type, 
                                           outer_vlan_low, inner_vlan_low, vp);
    if (rv != BCM_E_NONE) {
        for (i = 0; i < port_count; i++) {
            /* coverity[unsigned_compare] */
            if (new_idx >= 0) {
            _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx);
        }
            /* coverity[unsigned_compare] */
            if (new_idx_inner >= 0) {
                _bcm_trx_vlan_range_profile_entry_delete(unit, new_idx_inner);
            }
        }
        soc_mem_unlock(unit, ING_VLAN_RANGEm);
        goto cleanup;
    }

    /* Delete the old profile entry */
    for (i = 0; i < port_count; i++) {
        /* coverity[unsigned_compare] */
        if (new_idx >= 0) {
        _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx);
    }
        /* coverity[unsigned_compare] */
        if (new_idx_inner >= 0) {
            _bcm_trx_vlan_range_profile_entry_delete(unit, old_idx_inner);
        }
    }
    soc_mem_unlock(unit, ING_VLAN_RANGEm);

cleanup:
    if (NULL != member_array) {
        sal_free(member_array);
    }
    if (NULL != mod_array) {
        sal_free(mod_array);
    }
    if (NULL != port_array) {
        sal_free(port_array);
    }

    return rv;
}
#endif

/*
 * Function:
 *     _bcm_trx_system_reserved_vlan_ing_set
 * Purpose:
 *     Set ingress system reserved VLAN
 * Parameters:
 *     unit  - Device unit number
 *     flags - Operation flags
 *     vlan  - VLAN
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_trx_system_reserved_vlan_ing_set(int unit, int arg)
{
    uint32 rval;
    int rv;
    int valid = (arg != BCM_VLAN_INVALID);
    int vid = valid ? arg : 0;

    rval = 0;
    soc_reg_field_set(unit, ING_SYS_RSVD_VIDr, &rval, VALIDf, valid);
    soc_reg_field_set(unit, ING_SYS_RSVD_VIDr, &rval, VIDf, vid);
    rv = WRITE_ING_SYS_RSVD_VIDr(unit, rval);

    return rv;
}

/*
 * Function:
 *     _bcm_trx_system_reserved_vlan_egr_set
 * Purpose:
 *     Set egress system reserved VLAN
 * Parameters:
 *     unit  - Device unit number
 *     flags - Operation flags
 *     vlan  - VLAN
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_trx_system_reserved_vlan_egr_set(int unit, int arg)
{
    uint32 rval;
    int rv;
    int vid = (arg != BCM_VLAN_INVALID) ? arg : 0;

    rval = 0;
    soc_reg_field_set(unit, EGR_SYS_RSVD_VIDr, &rval, VIDf, vid);
    rv = WRITE_EGR_SYS_RSVD_VIDr(unit, rval);

    return rv;
}

/*
 * Function:
 *     _bcm_trx_system_reserved_vlan_ing_get
 * Purpose:
 *     Get ingress system reserved VLAN
 * Parameters:
 *     unit  - Device unit number
 *     vlan  - VLAN
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_trx_system_reserved_vlan_ing_get(int unit, int *arg)
{
    uint32 rval;
    int rv = BCM_E_UNAVAIL;

    rv = READ_ING_SYS_RSVD_VIDr(unit, &rval);
    if (SOC_SUCCESS(rv)) {
        if (soc_reg_field_get(unit, ING_SYS_RSVD_VIDr, rval, VALIDf)) {
            bcm_vlan_t vlan;

            vlan = soc_reg_field_get(unit, ING_SYS_RSVD_VIDr, rval, VIDf);
            *arg = vlan;
        } else {
            *arg = BCM_VLAN_INVALID;
        }
    }
    return rv;
}


#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
#define _BCM_VLAN_PROTOCOL_DATA_CHUNK    16
#define _BCM_VLAN_ACTION_PROFILE_DEFAULT  0
#define _BCM_VLAN_PROTOCOL_DATA_PROFILE_DEFAULT 0

STATIC
int
_bcm_trx_port_vlan_protocol_data_profile_index_set(int unit,
    bcm_port_t port, int vlan_prot_offset, int action_profile_idx,
    bcm_vlan_action_set_t *action)
{
    int i, alloc_size = 0;
    int vlan_prot_idx, rv = BCM_E_NONE;
    vlan_protocol_data_entry_t *vlan_prot_data;
    vlan_protocol_data_entry_t data[_BCM_VLAN_PROTOCOL_DATA_CHUNK];
    void *entries[1];
    _bcm_port_info_t            *pinfo;
    int vlan_prot_idx_new;


    if ((vlan_prot_offset < 0) ||
        (vlan_prot_offset >= _BCM_VLAN_PROTOCOL_DATA_CHUNK)) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    vlan_prot_idx = pinfo->vlan_prot_ptr;

    if (SOC_IS_TOMAHAWKX(unit)) {
        sal_memset(data, 0, sizeof(data));
        for (i = 0; i < _BCM_VLAN_PROTOCOL_DATA_CHUNK; i++) {
            BCM_IF_ERROR_RETURN(READ_VLAN_PROTOCOL_DATAm(
                unit, MEM_BLOCK_ANY, vlan_prot_idx + i, &data[i]));
        }
        vlan_prot_data = data;
    } else {
        /* Allocate memory for DMA */
        alloc_size = _BCM_VLAN_PROTOCOL_DATA_CHUNK *
                           sizeof(vlan_protocol_data_entry_t);
        vlan_prot_data = soc_cm_salloc(unit, alloc_size,
                                   "VLAN protocol data table");
        if (NULL == vlan_prot_data) {
            return (BCM_E_MEMORY);
        }
        sal_memset(vlan_prot_data, 0, alloc_size);

        /* Read the old profile chunk */

    rv = soc_mem_read_range(unit, VLAN_PROTOCOL_DATAm, 
                            MEM_BLOCK_ANY, vlan_prot_idx, 
                            vlan_prot_idx + (_BCM_VLAN_PROTOCOL_DATA_CHUNK - 1),
                            vlan_prot_data);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vlan_prot_data);
        return (rv);
    }
    }

    /* Modify what's needed */
    /* coverity[uninit_use_in_call: FALSE] */
    _bcm_trx_vlan_protocol_data_entry_set(unit,
        &vlan_prot_data[vlan_prot_offset], action, action_profile_idx);

    /* Add new chunk and store new HW index */
    entries[0] = vlan_prot_data;
    rv = _bcm_port_vlan_protocol_data_entry_add(unit, entries,
             _BCM_VLAN_PROTOCOL_DATA_CHUNK, (uint32 *)&vlan_prot_idx_new);
    /* Free the DMA memory */
    if (alloc_size > 0) {
    soc_cm_sfree(unit, vlan_prot_data);
    }
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    rv = _bcm_esw_port_tab_set(unit, port,
        _BCM_CPU_TABS_ETHER, VLAN_PROTOCOL_DATA_INDEXf, vlan_prot_idx_new / 16);
    if (BCM_SUCCESS(rv)) {
        pinfo->vlan_prot_ptr = vlan_prot_idx_new;
    } else {
        (void)_bcm_port_vlan_protocol_data_entry_delete(unit, vlan_prot_idx_new);
        return rv;
    }

    /* Delete the old profile chunk */
    rv = _bcm_port_vlan_protocol_data_entry_delete(unit, vlan_prot_idx);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    return (rv);
}

STATIC
int
_bcm_trx_port_vlan_protocol_data_profile_set(int unit,
    bcm_port_t port, void **entries)
{
    int               vlan_prot_idx, rv = BCM_E_NONE;
    _bcm_port_info_t  *pinfo;
    bcm_port_t port_in = port;
    PORT_LOCK(unit);

    rv = _bcm_esw_port_gport_validate(unit, port, &port);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Delete the old profile chunk */
    rv = _bcm_port_info_get(unit, port, &pinfo);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv = _bcm_port_vlan_protocol_data_entry_delete(unit, pinfo->vlan_prot_ptr);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Add new chunk and store new HW index */
    rv = _bcm_port_vlan_protocol_data_entry_add(unit, entries,
             _BCM_VLAN_PROTOCOL_DATA_CHUNK, (uint32 *)&vlan_prot_idx);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Set VLAN_PROTOCOL_DATA_INDEXf  in PORT_TAB. */
    rv = _bcm_esw_port_tab_set_without_portlock(unit, port_in,
        _BCM_CPU_TABS_ETHER, VLAN_PROTOCOL_DATA_INDEXf, vlan_prot_idx / 16);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }
    /* Update vlan_prot_ptr in SW port info */
    pinfo->vlan_prot_ptr = vlan_prot_idx;

cleanup:
    PORT_UNLOCK(unit);
    return (rv);

}


/*
 * Function   :
 *      _bcm_trx_vlan_protocol_data_profile_update
 * Description   :
 *      Helper function for an API to update vlan port protocol data memory
 *      for specific ports.
 * Parameters   :
 *      unit            (IN) BCM unit number
 *      update_pbm      (IN) BCM port bitmap to update
 *      prot_idx        (IN) Index to Port Protocol table
 *      action          (IN) VLAN action to be used to update all ports in pbmp
 */
int
_bcm_trx_vlan_protocol_data_profile_update(
    int unit, bcm_pbmp_t update_pbm, int prot_idx,
    bcm_vlan_action_set_t *action)
{
    bcm_port_t                  p;
    int                         data_idx;
    uint32                      action_profile_idx, old_action_profile_idx;
    int                         use_default = 0;
    bcm_vlan_action_set_t       def_action, *action_p;
    _bcm_port_info_t            *pinfo;
    vlan_protocol_data_entry_t  vde;

    /* if action is NULL then default action should be used for each port */
    if (NULL == action) {
        use_default = 1;
        action_p = &def_action;
    } else {
        BCM_IF_ERROR_RETURN(
            _bcm_trx_vlan_action_verify(unit, action));
        action_p = action;
    }

    BCM_PBMP_ITER(update_pbm, p) {
        BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, p, &pinfo));
        data_idx = pinfo->vlan_prot_ptr + prot_idx;

        if (use_default) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_vlan_port_default_action_get(unit, p, action_p));
            if(soc_feature(unit, soc_feature_decouple_protocol_vlan)){
                action_p->new_outer_vlan = BCM_VLAN_DEFAULT;
            }
        }

        BCM_IF_ERROR_RETURN(
            READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY, data_idx, &vde));

        old_action_profile_idx =
            soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                TAG_ACTION_PROFILE_PTRf);

        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_add(unit,
            action_p, &action_profile_idx));

        /* Update  VLAN_PROTOCOL_DATA profile and
        * PORT.VLAN_PROTOCOL_DATA_INDEX */
        BCM_IF_ERROR_RETURN(
            _bcm_trx_port_vlan_protocol_data_profile_index_set(unit,
                p, prot_idx, action_profile_idx, action_p));

        if (old_action_profile_idx >= ing_action_profile_def[unit]) {
            BCM_IF_ERROR_RETURN(
                _bcm_trx_vlan_action_profile_entry_delete(unit,
                    old_action_profile_idx));
        }
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_trx_vlan_port_default_action_profile_set
 * Purpose:
 *      Set the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 *      action     - (IN) Vlan tag actions
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_port_default_action_profile_set(int unit, bcm_port_t port,
                                     bcm_vlan_action_set_t *action)
{
    uint32 action_profile_idx, old_action_profile_idx;
    bcm_port_cfg_t pcfg;
    vlan_protocol_data_entry_t vde;
    int vlan_prot_entries, vlan_data_prot_start, i, rv;
    int alloc_size = 0;
    _bcm_port_info_t *pinfo;
    vlan_protocol_data_entry_t *vlan_prot_data;
    vlan_protocol_data_entry_t data[_BCM_VLAN_PROTOCOL_DATA_CHUNK];
    void *entries[1];

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action));
    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_add(unit,
        action, &action_profile_idx));

#ifdef BCM_HGPROXY_COE_SUPPORT
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        uint32 values[5];
        soc_field_t fields[] = {
            TAG_ACTION_PROFILE_PTRf,
            PORT_PRIf,  
            OCFIf,
            IPRIf,
            ICFIf
        };

        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_get(unit, port, LPORT_PROFILE_LPORT_TAB,
                                           5, fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        old_action_profile_idx = values[0];
        pcfg.pc_new_opri = values[1];
        pcfg.pc_new_ocfi = values[2];
        pcfg.pc_new_ipri = values[3];
        pcfg.pc_new_icfi = values[4];
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
        old_action_profile_idx = pcfg.pc_vlan_action;
    }

    pcfg.pc_vlan = action->new_outer_vlan;
    pcfg.pc_ivlan = action->new_inner_vlan;
    pcfg.pc_vlan_action = action_profile_idx;
    if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            pcfg.pc_new_opri = action->priority;
        }
        pcfg.pc_new_ocfi = action->new_outer_cfi;
        pcfg.pc_new_ipri = action->new_inner_pkt_prio;
        pcfg.pc_new_icfi = action->new_inner_cfi;
    } else {
        if (action->priority >= BCM_PRIO_MIN &&
            action->priority <= BCM_PRIO_MAX) {
            pcfg.pc_new_opri = action->priority;
        }
    }

#ifdef BCM_HGPROXY_COE_SUPPORT
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        uint32 values[7];
        soc_field_t fields[] = {
            PORT_VIDf,
            IVIDf,
            TAG_ACTION_PROFILE_PTRf,
            PORT_PRIf,
            OCFIf,
            IPRIf,
            ICFIf
        };

        values[0] = pcfg.pc_vlan;
        values[1] = pcfg.pc_ivlan;
        values[2] = pcfg.pc_vlan_action;
        values[3] = pcfg.pc_new_opri;
        values[4] = pcfg.pc_new_ocfi;
        values[5] = pcfg.pc_new_ipri;
        values[6] = pcfg.pc_new_icfi;

        PORT_LOCK(unit);
        rv = bcm_esw_port_lport_fields_set(unit, port, LPORT_PROFILE_LPORT_TAB,
                                           7, fields, values);
        PORT_UNLOCK(unit);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }else
#endif
    {
        BCM_IF_ERROR_RETURN
            (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));
    }

    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_delete(unit,
        old_action_profile_idx));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_port_gport_validate(unit, port, &port));

    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    vlan_data_prot_start = pinfo->vlan_prot_ptr;

    if (pinfo->p_vd_pbvl == 0) {
        /* Avoid one crash in kt2 tr 18 test case in linux environment */
        return (BCM_E_NONE);
    }

    if (SOC_IS_TOMAHAWKX(unit)) {
        sal_memset(data, 0, sizeof(data));
        for (i = 0; i < _BCM_VLAN_PROTOCOL_DATA_CHUNK; i++) {
            BCM_IF_ERROR_RETURN(READ_VLAN_PROTOCOL_DATAm(
                unit, MEM_BLOCK_ANY, vlan_data_prot_start + i, &data[i]));
        }
        vlan_prot_data = data;
    } else {
    /* Allocate memory for DMA */
    alloc_size = _BCM_VLAN_PROTOCOL_DATA_CHUNK * 
                       sizeof(vlan_protocol_data_entry_t);
    vlan_prot_data = soc_cm_salloc(unit, alloc_size,
                               "VLAN protocol data table");
    if (NULL == vlan_prot_data) {
        return (BCM_E_MEMORY);
    }
    sal_memset(vlan_prot_data, 0, alloc_size);

    /* Read the old profile chunk */
    rv = soc_mem_read_range(unit, VLAN_PROTOCOL_DATAm, 
             MEM_BLOCK_ANY, vlan_data_prot_start, 
             vlan_data_prot_start + (_BCM_VLAN_PROTOCOL_DATA_CHUNK - 1),
             vlan_prot_data);
    if (BCM_FAILURE(rv)) {
        soc_cm_sfree(unit, vlan_prot_data);
        return (rv);
    }
    }

    /* Update TAG_ACTION_PROFILE_PTR in VLAN_PROTOCOL_DATA */
    vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    for (i = 0; i < vlan_prot_entries; i++) {
        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) { 
            rv = READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                     vlan_data_prot_start + i, &vde);
            if (BCM_FAILURE(rv)) {
                if (alloc_size > 0) {
                soc_cm_sfree(unit, vlan_prot_data);
                }
                return (rv);
            }

            old_action_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit,
                                         &vde, TAG_ACTION_PROFILE_PTRf);

            rv = _bcm_trx_vlan_action_profile_entry_add(unit,
                    action, &action_profile_idx);
            if (BCM_FAILURE(rv)) {
                if (alloc_size > 0) {
                soc_cm_sfree(unit, vlan_prot_data);
                }
                return (rv);
            }

            /* Modify what's needed */
            /* coverity[uninit_use_in_call: FALSE] */
            _bcm_trx_vlan_protocol_data_entry_set(unit,
                &vlan_prot_data[i], action, action_profile_idx);

            rv = _bcm_trx_vlan_action_profile_entry_delete(unit,
                     old_action_profile_idx);
            if (BCM_FAILURE(rv)) {
                if (alloc_size > 0) {
                soc_cm_sfree(unit, vlan_prot_data);
                }
                return (rv);
            }
        }
    }
    entries[0] = vlan_prot_data;

    /* Update  VLAN_PROTOCOL_DATA profile and
    * PORT.VLAN_PROTOCOL_DATA_INDEX */
    rv  =_bcm_trx_port_vlan_protocol_data_profile_set(unit, port, entries);
    if (alloc_size > 0) {
    soc_cm_sfree(unit, vlan_prot_data);
    }
    return (rv);
}


/*
 * Function:
 *      _bcm_tr_vlan_port_default_action_delete
 * Purpose:
 *      Deletes the port's default vlan tag actions
 * Parameters:
 *      unit       - (IN) BCM unit.
 *      port       - (IN) Port number.
 * Returns:
 *      BCM_E_XXX
 */ 
int 
_bcm_trx_vlan_port_default_action_profile_delete(int unit, bcm_port_t port)
{
    uint32                      old_action_profile_idx;
    uint32                      action_profile_idx;
    bcm_port_cfg_t              pcfg;
    int                         num_ent, start, i;
    _bcm_port_info_t            *pinfo;
    vlan_protocol_data_entry_t  vde;
    int                         rv = BCM_E_NONE;

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_get(unit, port, &pcfg));
    old_action_profile_idx = pcfg.pc_vlan_action;

    pcfg.pc_vlan = BCM_VLAN_DEFAULT;
    pcfg.pc_ivlan = 0;
    pcfg.pc_vlan_action = ing_action_profile_def[unit];
    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_set(unit, port, &pcfg));

    _bcm_trx_vlan_action_profile_entry_increment(unit,
         _BCM_VLAN_ACTION_PROFILE_DEFAULT);
    BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_delete(unit,
        old_action_profile_idx));

    num_ent = soc_mem_index_count(unit, VLAN_PROTOCOLm);
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    start = pinfo->vlan_prot_ptr;

    if (start == 0) {
        /* Decrease ref count of default profile and return */
        rv = _bcm_port_vlan_protocol_data_entry_delete(unit, start);
        return rv;
    }

    for (i = 0; i < num_ent; i++) {
        if (!_BCM_PORT_VD_PBVL_IS_SET(pinfo, i)) { 
            BCM_IF_ERROR_RETURN(READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                        start + i, &vde));
            action_profile_idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit, &vde,
                    TAG_ACTION_PROFILE_PTRf);

            _bcm_trx_vlan_action_profile_entry_increment(unit,
                 _BCM_VLAN_ACTION_PROFILE_DEFAULT);
            BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_profile_entry_delete(unit, 
                        action_profile_idx));
        }
    }

    /* Delete the VLAN_PROTOCOL_DATA profile */
    BCM_IF_ERROR_RETURN(_bcm_port_vlan_protocol_data_entry_delete(unit, start));

    /* Increase ref count of default profile and return */
    BCM_IF_ERROR_RETURN(_bcm_port_vlan_protocol_data_entry_reference(unit,
        _BCM_VLAN_PROTOCOL_DATA_PROFILE_DEFAULT,
        _BCM_VLAN_PROTOCOL_DATA_CHUNK));
    pinfo->vlan_prot_ptr = _BCM_VLAN_PROTOCOL_DATA_PROFILE_DEFAULT;

    return rv;
}

#endif /*BCM_KATANA2_SUPPORT */

/*
 * Function:
 *     _bcm_trx_vlan_action_profile_ref_count_get
 * Purpose:
 *     Get vlan action profile entry use count.
 * Parameters:
 *     unit      - (IN)  BCM device number.
 *     index     - (IN)  Profile entry index.
 *     ref_count - (OUT) Vlan Action profile use count.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_trx_vlan_action_profile_ref_count_get(int unit, int index, int *ref_count)
{
    int rv = BCM_E_NONE;
    if (NULL == ref_count) {
        return (BCM_E_PARAM);
    }

    ING_VLAN_TAG_ACTION_LOCK(unit);
    rv = soc_profile_mem_ref_count_get(unit, ing_action_profile[unit],
                                       index, ref_count);
    ING_VLAN_TAG_ACTION_UNLOCK(unit);
    return rv;
}


#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_trx_vlan_sw_dump
 * Purpose:
 *     Displays VLAN information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_trx_vlan_sw_dump(int unit)
{
    int     num_entries;
    int     ref_count;
    int     entries_per_set;
    int     index;
    ing_vlan_tag_action_profile_entry_t *ing_entry;
    egr_vlan_tag_action_profile_entry_t *egr_entry;
    ing_vlan_range_entry_t              *range_entry;

    /* Display entries with reference count > 0 */

    if (ing_action_profile[unit] != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "\nSW Information Ingress VLAN Action Profile - "
                 "Unit %d\n"), unit));

        num_entries = soc_mem_index_count
            (unit, ing_action_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            " Number of entries: %d\n\n"), num_entries));

        for (index = 0; index < num_entries; index ++) {
            ING_VLAN_TAG_ACTION_LOCK(unit);
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               ing_action_profile[unit],
                                               index, &ref_count))) {
                ING_VLAN_TAG_ACTION_UNLOCK(unit);
                break;
            }

            if (ref_count <= 0) {
                ING_VLAN_TAG_ACTION_UNLOCK(unit);
                continue;
            }

            entries_per_set =
                ing_action_profile[unit]->tables[0].entries[index].entries_per_set;
            ing_entry =
                SOC_PROFILE_MEM_ENTRY(unit,
                                      ing_action_profile[unit],
                                      ing_vlan_tag_action_profile_entry_t *,
                                      index);
            LOG_CLI((BSL_META_U(unit,
                                " Index           = 0x%x\n"), index));
            LOG_CLI((BSL_META_U(unit,
                                " Reference count = %d\n"), ref_count));
            LOG_CLI((BSL_META_U(unit,
                                " Entries per set = %d\n"), entries_per_set));
            soc_mem_entry_dump(unit, ing_action_profile[unit]->tables[0].mem,
                               ing_entry, BSL_LSS_CLI);
            LOG_CLI((BSL_META_U(unit, "\n\n")));
            ING_VLAN_TAG_ACTION_UNLOCK(unit);
        }
    }

    if (egr_action_profile[unit] != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "\nSW Information Egress VLAN Action Profile - "
                 "Unit %d\n"), unit));

        num_entries = soc_mem_index_count
            (unit, egr_action_profile[unit]->tables[0].mem);
        LOG_CLI((BSL_META_U(unit,
                            " Number of entries: %d\n\n"), num_entries));

        for (index = 0; index < num_entries; index ++) {
            EGR_VLAN_TAG_ACTION_LOCK(unit);
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               egr_action_profile[unit],
                                               index, &ref_count))) {
                EGR_VLAN_TAG_ACTION_UNLOCK(unit);
                break;
            }

            if (ref_count <= 0) {
                EGR_VLAN_TAG_ACTION_UNLOCK(unit);
                continue;
            }

            entries_per_set =
                egr_action_profile[unit]->tables[0].entries[index].entries_per_set;
            egr_entry =
                SOC_PROFILE_MEM_ENTRY(unit,
                                      egr_action_profile[unit],
                                      egr_vlan_tag_action_profile_entry_t *,
                                      index);
            LOG_CLI((BSL_META_U(unit,
                                " Index           = 0x%x\n"), index));
            LOG_CLI((BSL_META_U(unit,
                                " Reference count = %d\n"), ref_count));
            LOG_CLI((BSL_META_U(unit,
                                " Entries per set = %d\n"), entries_per_set));
            soc_mem_entry_dump(unit, egr_action_profile[unit]->tables[0].mem,
                               egr_entry, BSL_LSS_CLI);
            LOG_CLI((BSL_META_U(unit, "\n\n")));
            EGR_VLAN_TAG_ACTION_UNLOCK(unit);
        }
    }

    if (vlan_range_profile[unit] != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "\nSW Information VLAN Range Profile - Unit %d\n"),
                 unit));

        num_entries = soc_mem_index_count
            (unit, vlan_range_profile[unit]->tables[0].mem);

        LOG_CLI((BSL_META_U(unit,
                            " Number of entries: %d\n\n"), num_entries));

        for (index = 0; index < num_entries; index ++) {
            VLAN_RANGE_LOCK(unit);
            if (SOC_FAILURE
                (soc_profile_mem_ref_count_get(unit,
                                               vlan_range_profile[unit],
                                               index, &ref_count))) {
                VLAN_RANGE_UNLOCK(unit);
                break;
            }

            if (ref_count <= 0) {
                VLAN_RANGE_UNLOCK(unit);
                continue;
            }

            entries_per_set =
                vlan_range_profile[unit]->tables[0].entries[index].entries_per_set;
            range_entry =
                SOC_PROFILE_MEM_ENTRY(unit,
                                      vlan_range_profile[unit],
                                      ing_vlan_range_entry_t *,
                                      index);
            LOG_CLI((BSL_META_U(unit,
                                " Index           = 0x%x\n"), index));
            LOG_CLI((BSL_META_U(unit,
                                " Reference count = %d\n"), ref_count));
            LOG_CLI((BSL_META_U(unit,
                                " Entries per set = %d\n"), entries_per_set));
            soc_mem_entry_dump(unit, vlan_range_profile[unit]->tables[0].mem,
                               range_entry, BSL_LSS_CLI);
            LOG_CLI((BSL_META_U(unit, "\n\n")));
            VLAN_RANGE_UNLOCK(unit);
        }
    }

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      _bcm_trx_vlan_mac_port_key_fields_set
 * Description   :
 *      Set Key Fields in VLAM_MACm entry hw buffer for MAC+PORT View.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      mac       (IN) MAC address
 *      port      (IN) gport number
 *      vment     (OUT) VLAN_MACm entry hw buffer
 */
int _bcm_trx_vlan_mac_port_key_fields_set(
    int unit,
    bcm_vlan_match_info_t *match_info,
    vlan_mac_entry_t *vment)
{
    int rv = BCM_E_NONE;
    int gport_id;
    int source_type;
    int max_bits;
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;

    /* Set Key Type */
    soc_VLAN_MACm_field32_set(unit, vment, KEY_TYPEf,
            TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT);

    /* Set entry MAC address */
    soc_VLAN_MACm_mac_addr_set(unit, vment, MAC_PORT__MAC_ADDRf,
            match_info->src_mac);

    switch(match_info->match) {
        case BCM_VLAN_MATCH_MAC_PORT:
            if (BCM_GPORT_IS_SET(match_info->port)) {
                BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                            match_info->port, &mod_out, &port_out,
                            &trunk_id, &gport_id));
                if (BCM_GPORT_IS_TRUNK(match_info->port)) {
                    rv = _bcm_trunk_id_validate(unit, trunk_id);
                    if (BCM_FAILURE(rv)) {
                        return BCM_E_PORT;
                    }
                    source_type = 1;
                    soc_VLAN_MACm_field32_set(unit, vment, MAC_PORT__Tf, 1);
                    soc_VLAN_MACm_field32_set(unit, vment, MAC_PORT__TGIDf,
                            trunk_id);
                } else if BCM_GPORT_IS_MODPORT(match_info->port) {
                    if ((-1 == mod_out) || (-1 == port_out)) {
                        return BCM_E_PORT;
                    }
                    source_type = 1;
                    soc_VLAN_MACm_field32_set(unit, vment,
                            MAC_PORT__PORT_NUMf, port_out);
                    soc_VLAN_MACm_field32_set(unit, vment,
                            MAC_PORT__MODULE_IDf, mod_out);
                } else {
                    if (gport_id == -1) {
                        return BCM_E_PORT;
                    }
                    source_type = 2;
                    soc_VLAN_MACm_field32_set(unit, vment,
                            MAC_PORT__SOURCE_FIELDf, gport_id);
                }
            } else {
                return BCM_E_PORT;
            }
            break;
        case BCM_VLAN_MATCH_MAC_PORT_CLASS:
            max_bits =soc_mem_field_length(unit,
                    VLAN_MACm, MAC_PORT__SOURCE_FIELDf);
            if (match_info->port_class < (1 << max_bits)) {
                source_type = 3;
                soc_VLAN_MACm_field32_set(unit, vment, MAC_PORT__SOURCE_FIELDf,
                        match_info->port_class);
            } else {
                return BCM_E_PORT;
            }
            break;
        default:
            return BCM_E_PARAM;
    }

    /* Set entry Source Field */
    if (SOC_MEM_FIELD_VALID(unit, VLAN_MACm, SOURCE_TYPEf)) {
        soc_VLAN_MACm_field32_set(unit, vment, SOURCE_TYPEf, source_type);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_mac_port_key_fields_get
 * Description   :
 *      Get Key Fields from VLAM_MACm entry hw buffer for MAC+PORT View.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      vment     (IN) VLAN_MACm entry hw buffer
 *      mac       (OUT) MAC address
 *      port      (OUT) gport number
 */
int _bcm_trx_vlan_mac_port_key_fields_get(int             unit,
                                        vlan_mac_entry_t *vment,
                                        bcm_vlan_match_info_t *match_info)
{
    int rv = BCM_E_NONE;
    uint32 port_in, mod_in;
    uint32 trunk_id;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
    uint32 vp;
    _bcm_gport_dest_t gport_dest;
#endif

    /* Get entry mac address. */
    soc_VLAN_MACm_mac_addr_get(unit, vment, MAC_PORT__MAC_ADDRf, match_info->src_mac);

    /* Get Source Type */
    switch(soc_VLAN_MACm_field32_get(unit, vment, SOURCE_TYPEf)) {
        case 1: /* Port or Trunk */
            match_info->match = BCM_VLAN_MATCH_MAC_PORT;
            if (soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__Tf)) {
                trunk_id =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__TGIDf);
                BCM_GPORT_TRUNK_SET(match_info->port, trunk_id);
            } else {
                port_in =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__PORT_NUMf);
                mod_in =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__MODULE_IDf);
                BCM_GPORT_MODPORT_SET(match_info->port, mod_in, port_in);
            }
            break;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
        case 2: /* Virtual Port */
            match_info->match = BCM_VLAN_MATCH_MAC_PORT;
            vp = soc_VLAN_MACm_field32_get(unit,
                    vment, MAC_PORT__SOURCE_FIELDf);
            _bcm_gport_dest_t_init(&gport_dest);
            rv = _bcm_vp_gport_dest_fill(unit, vp, &gport_dest);
            BCM_IF_ERROR_RETURN(rv);

            rv = _bcm_esw_gport_construct(unit, &gport_dest, &match_info->port);
            BCM_IF_ERROR_RETURN(rv);
            break;
#endif /* BCM_TRIDENT2PLUS_SUPPORT && INCLUDE_L3 */
        case 3: /* Port Group */
            match_info->match = BCM_VLAN_MATCH_MAC_PORT_CLASS;
            match_info->port_class =
                soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__SOURCE_FIELDf);
            break;
        default:
            match_info->match = BCM_VLAN_MATCH_INVALID;
            break;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_match_action_add
 * Description   :
 *      Add action for match criteria for Vlan assignment.
 * Parameters   :
 *      unit      (IN) BCM unit number
 *      options   (IN) Options for creating vlan match and assignment.
 *      svp_port  (IN) Source VP gport.
 *      match_info (IN) Vlan match criteria.
 *      action_set (IN) Action Set.
 */
int
_bcm_trx_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_gport_t port,
    bcm_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int rv;
    int idx = 0;
    int gport_id;
    int svp_valid;
    bcm_trunk_t trunk_id;
    bcm_module_t mod_out;
    bcm_port_t port_out;
    uint32 profile_idx;
    vlan_mac_entry_t vment;
    vlan_mac_entry_t res_vment;

    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_UNAVAIL;
    }

    if (action_set != NULL) {
        BCM_IF_ERROR_RETURN(_bcm_trx_vlan_action_verify(unit, action_set));
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            if (action_set->priority == -1) {
                return BCM_E_PARAM;  /* no default priority action to take*/
            }
        }
    }

    sal_memset(&vment, 0, sizeof(vlan_mac_entry_t));
    sal_memset(&res_vment, 0, sizeof(vlan_mac_entry_t));

    /* Set Key Fields */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_mac_port_key_fields_set(unit, match_info, &vment));

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_search(unit, VLAN_MACm, MEM_BLOCK_ALL, &idx,
            &vment, &res_vment, 0);
    soc_mem_unlock(unit, VLAN_MACm);

    /* If Entry exists, check svp configuration */
    if (rv == BCM_E_NONE) {
        svp_valid = FALSE;
        if (soc_VLAN_MACm_field32_get(unit, &vment, MAC_PORT__SVP_VALIDf)) {
            svp_valid = TRUE;
        }
        if (((port == BCM_GPORT_INVALID) && (svp_valid == TRUE)) ||
                ((port != BCM_GPORT_INVALID) && (svp_valid == FALSE))) {
            return BCM_E_EXISTS;
        }
        if (!options & BCM_VLAN_MATCH_ACTION_REPLACE) {
            return BCM_E_EXISTS;
        }
    }

    /* If Entry does not exists and want to replace */
    if ((rv == BCM_E_NOT_FOUND) && (options & BCM_VLAN_MATCH_ACTION_REPLACE)) {
        return rv;
    }

    /* Set SVP Port */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                    port, &mod_out, &port_out, &trunk_id, &gport_id));
        if (SOC_MEM_FIELD_VALID(unit, VLAN_MACm, MAC_PORT__SVPf)) {
            soc_VLAN_MACm_field32_set(unit, &vment, MAC_PORT__SVP_VALIDf, 1);
            soc_VLAN_MACm_field32_set(unit, &vment, MAC_PORT__SVPf, gport_id);
        }
    }

    if (action_set != NULL) {
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_action_profile_entry_add(unit, action_set, &profile_idx));

        /* Set Data Fields */
        soc_VLAN_MACm_field32_set(unit, &vment,
                MAC_PORT__OVIDf, action_set->new_outer_vlan);
        soc_VLAN_MACm_field32_set(unit, &vment,
                MAC_PORT__IVIDf, action_set->new_inner_vlan);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            soc_VLAN_MACm_field32_set(unit, &vment,
                    MAC_PORT__OPRIf, action_set->priority);
            soc_VLAN_MACm_field32_set(unit, &vment,
                    MAC_PORT__OCFIf, action_set->new_outer_cfi);
            soc_VLAN_MACm_field32_set(unit, &vment,
                    MAC_PORT__IPRIf, action_set->new_inner_pkt_prio);
            soc_VLAN_MACm_field32_set(unit, &vment,
                    MAC_PORT__ICFIf, action_set->new_inner_cfi);
        } else {
            if (action_set->priority >= BCM_PRIO_MIN &&
                    action_set->priority <= BCM_PRIO_MAX) {
                soc_VLAN_MACm_field32_set(unit, &vment,
                        MAC_PORT__PRIf, action_set->priority);
            }
        }
        soc_VLAN_MACm_field32_set(unit, &vment,
                MAC_PORT__TAG_ACTION_PROFILE_PTRf, profile_idx);
        if (SOC_MEM_FIELD_VALID(unit, VLAN_MACm, MAC_PORT__VLAN_ACTION_VALIDf)) {
            soc_VLAN_MACm_field32_set(unit, &vment,
                    MAC_PORT__VLAN_ACTION_VALIDf, 1);
        }
    }
    soc_VLAN_MACm_field32_set(unit, &vment, VALIDf, 1);

    /* Insert to VLAN_MACm */
    rv = soc_mem_insert_return_old(unit, VLAN_MACm,
            MEM_BLOCK_ALL, &vment, &vment);
    if (rv == SOC_E_EXISTS) {
        /* Delete the old vlan action profile entry */
        if (soc_VLAN_MACm_field32_get(unit, &vment,
                    MAC_PORT__VLAN_ACTION_VALIDf)) {
            profile_idx = soc_VLAN_MACm_field32_get(unit, &vment,
                    MAC_PORT__TAG_ACTION_PROFILE_PTRf);
            rv = _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }
    }

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_match_action_get
 * Purpose:
 *      Get action for given match criteria.
 * Parameters:
 *      unit         (IN)  Device Number
 *      svp_port     (IN) source vp port
 *      match_info - (IN) Match criteria info
 *      action_set - (OUT) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_match_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int matched, svp_assigned, vp;
    vlan_mac_entry_t vment;             /* Lookup key hw buffer.      */
    vlan_mac_entry_t res_vment;         /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int rv;                             /* Operation return status.   */
    int idx = 0;                        /* Lookup result entry index. */

    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check. */
    if (NULL == match_info) {
        return (BCM_E_PARAM);
    }

    sal_memset(&vment, 0, sizeof(vlan_mac_entry_t));
    sal_memset(&res_vment, 0, sizeof(vlan_mac_entry_t));

    /* Set Key fields */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_mac_port_key_fields_set(unit, match_info, &vment));

    /* Perform VLAN_MAC table search by mac address and port. */
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_search(unit, VLAN_MACm, MEM_BLOCK_ALL, &idx,
            &vment, &res_vment, 0);
    soc_mem_unlock(unit, VLAN_MACm);
    BCM_IF_ERROR_RETURN(rv);

    /* Evaluate svp */
    matched = FALSE;
    svp_assigned = soc_VLAN_MACm_field32_get(unit, &vment, MAC_PORT__SVP_VALIDf);

    if ((port != BCM_GPORT_INVALID) && (svp_assigned == TRUE)) {
        vp = soc_VLAN_MACm_field32_get(unit, &vment, MAC_PORT__SVPf);
        matched = _bcm_esw_port_vp_gport_check(port, vp);
    } else if ((port == BCM_GPORT_INVALID) && (svp_assigned == FALSE)) {
        matched = TRUE;
    } else {
        /* (((port != BCM_GPORT_INVALID) && (svp_assigned == FALSE)) ||
           ((port == BCM_GPORT_INVALID) && (svp_assigned == TRUE))) */
        matched = FALSE;
    }
    if (matched == FALSE) {
        return BCM_E_NOT_FOUND;
    }


    /* Read Action Fields */
    if (soc_VLAN_MACm_field32_get(unit, &res_vment,
                MAC_PORT__VLAN_ACTION_VALIDf)) {
        if (NULL == action_set) {
            return BCM_E_PARAM;
        }

        action_set->new_outer_vlan =
            soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__OVIDf);
        action_set->new_inner_vlan =
            soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__IVIDf);
        if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
            action_set->priority =
                soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__OPRIf);
            action_set->new_outer_cfi =
                soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__OCFIf);
            action_set->new_inner_pkt_prio =
                soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__IPRIf);
            action_set->new_inner_cfi =
                soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__ICFIf);
        } else {
            action_set->priority =
                soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__PRIf);
        }

        /* Read action profile data. */
        profile_idx = soc_VLAN_MACm_field32_get(unit,
                &res_vment, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_get(unit, action_set, profile_idx);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_match_action_delete
 * Purpose:
 *      Remove action for a match criteria
 * Parameters:
 *      unit       (IN)  Device Number
 *      match_info (IN)  Match criteria info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_match_action_delete(
    int unit,
    bcm_gport_t port,
    bcm_vlan_match_info_t *vlan_match_info)
{
    int rv, vp, svp_valid;
    int matched;
    vlan_mac_entry_t vment;             /* Lookup key hw buffer.      */
    vlan_mac_entry_t res_vment;         /* Lookup result buffer.      */
    uint32 profile_idx;                 /* Vlan action profile index. */
    int idx = 0;                        /* Lookup result entry index. */

    sal_memset(&vment, 0, sizeof(vlan_mac_entry_t));
    sal_memset(&res_vment, 0, sizeof(vlan_mac_entry_t));

    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_UNAVAIL;
    }

    /* Set Key fields */
    BCM_IF_ERROR_RETURN
        (_bcm_trx_vlan_mac_port_key_fields_set(unit, vlan_match_info, &vment));

    /* Perform VLAN_MAC table search by mac address and port. */
    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_search(unit, VLAN_MACm, MEM_BLOCK_ALL, &idx,
            &vment, &res_vment, 0);
    soc_mem_unlock(unit, VLAN_MACm);
    BCM_IF_ERROR_RETURN(rv);

    /* Evaluate SVP */
    matched = FALSE;
    svp_valid = soc_VLAN_MACm_field32_get(unit, &res_vment,
            MAC_PORT__SVP_VALIDf);
    if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
        vp = soc_VLAN_MACm_field32_get(unit, &res_vment, MAC_PORT__SVPf);
        matched = _bcm_esw_port_vp_gport_check(port, vp);
    } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
        matched = TRUE;
    } else {
        /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
           ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
        matched = FALSE;
    }

    if (matched == FALSE) {
        return BCM_E_NOT_FOUND;
    }

    /* Read action profile data. */
    if (soc_VLAN_MACm_field32_get(unit, &res_vment,
                MAC_PORT__VLAN_ACTION_VALIDf)) {
        profile_idx = soc_VLAN_MACm_field32_get(unit,
                &res_vment, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
        _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
    }

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_delete_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL,
            &vment, &vment);
    soc_mem_unlock(unit, VLAN_MACm);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_trx_vlan_match_action_multi_get
 * Purpose:
 *      Get all actions and match criteria for given vlan match.
 * Parameters:
 *      unit       (IN)  Device Number
 *      match      (IN)  vlan match
 *      port       (IN)  gport number
 *      size       (IN)  Number of elements in match_info_array
 *      match_info_array (OUT) Vlan match info array
 *      action_set (OUT) Action Set
 *      count      (OUT) Actual number of elements in match_info_array
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_match_action_multi_get(int unit, bcm_vlan_match_t match,
    bcm_gport_t port, int size, bcm_vlan_match_info_t *match_info_array,
    bcm_vlan_action_set_t *action_set, int *count)
{
    int vp, svp_valid;
    int rv, i, matched, ctr;
    int imin, imax, nent, vmbytes;
    int key_type_value, key_type;
    uint32 profile_idx;
    bcm_vlan_match_info_t match_info;
    vlan_mac_entry_t *vmtab = NULL;
    vlan_mac_entry_t *vment = NULL;

    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter checks */
    if ((size < 0) || (count == NULL)) {
        return BCM_E_PARAM;
    }
    if ((size > 0) && ((match_info_array == NULL) ||
                (action_set == NULL))) {
        return BCM_E_PARAM;
    }

    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
            imin, imax, vmtab);
    soc_mem_unlock(unit, VLAN_MACm);

    if (rv < 0) {
        soc_cm_sfree(unit, vmtab);
        return rv;
    }

    soc_mem_lock(unit, VLAN_MACm);
    for(i = 0, ctr = 0; i < nent; i++) {
        if ((size != 0) && (size <= ctr)) {
            break;
        }

        vment = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                vlan_mac_entry_t *, vmtab, i);

        if (!soc_VLAN_MACm_field32_get(unit, vment, VALIDf)) {
            continue;
        }

        /* Read KeyType */
        key_type_value = soc_VLAN_MACm_field32_get(unit, vment, KEY_TYPEf);
        (void) _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                &key_type);
        switch(key_type) {
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                /* Read Key Fields */
                bcm_vlan_match_info_t_init(&match_info);
                (void) _bcm_trx_vlan_mac_port_key_fields_get(unit,
                        vment, &match_info);
                if (match_info.match == BCM_VLAN_MATCH_INVALID) {
                    matched = FALSE;
                    break;
                }

                /* Evaluate SVP */
                matched = FALSE;
                svp_valid = soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__SVP_VALIDf);
                if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
                    vp = soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__SVPf);
                    matched = _bcm_esw_port_vp_gport_check(port, vp);
                } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
                    matched = TRUE;
                } else {
                    /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
                       ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
                    matched = FALSE;
                }
                /* Evaluate match criteria */
                if ((match != BCM_VLAN_MATCH_INVALID) &&
                        (match != match_info.match)) {
                    matched = FALSE;
                }
                break;
            default:
                matched = FALSE;
                break;
        }

        if (matched == FALSE) {
            continue;
        }

        ctr++;
        if (size >= ctr) {
            /* Copy Key Fields */
            sal_memcpy(match_info_array, &match_info,
                    sizeof(bcm_vlan_match_info_t));

            /* Read Data Fields */
            action_set->new_outer_vlan =
                soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__OVIDf);
            action_set->new_inner_vlan =
                soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__IVIDf);
            if (soc_feature(unit, soc_feature_vlan_pri_cfi_action)) {
                action_set->priority =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__OPRIf);
                action_set->new_outer_cfi =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__OCFIf);
                action_set->new_inner_pkt_prio =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__IPRIf);
                action_set->new_inner_cfi =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__ICFIf);
            } else {
                action_set->priority =
                    soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__PRIf);
            }

            profile_idx = soc_VLAN_MACm_field32_get(unit, vment,
                    MAC_PORT__TAG_ACTION_PROFILE_PTRf);

            _bcm_trx_vlan_action_profile_entry_get(unit, action_set, profile_idx);

            action_set++;
            match_info_array++;
        }
    }
    *count = ctr;

    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);

    return rv;
}

/*
 * Function:
 *      _bcm_trx_vlan_match_action_delete_all
 * Purpose:
 *      Remove all actions for vlan match
 * Parameters:
 *      unit       (IN)  Device Number
 *      match      (IN)  vlan match
 *      port       (IN)  port number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_trx_vlan_match_action_delete_all(int unit,
                bcm_vlan_match_t match, bcm_gport_t port)
{
    int vp, svp_valid;
    int rv, i, matched;
    int imin, imax, nent, vmbytes;
    uint32 profile_idx;
    vlan_mac_entry_t *vmtab = NULL;
    vlan_mac_entry_t *vment = NULL;
    int key_type_value, key_type;
    bcm_vlan_match_info_t match_info;

    if (!SOC_MEM_IS_VALID(unit, VLAN_MACm)) {
        return BCM_E_UNAVAIL;
    }

    imin = soc_mem_index_min(unit, VLAN_MACm);
    imax = soc_mem_index_max(unit, VLAN_MACm);
    nent = soc_mem_index_count(unit, VLAN_MACm);
    vmbytes = soc_mem_entry_words(unit, VLAN_MACm);
    vmbytes = WORDS2BYTES(vmbytes);

    vmtab = soc_cm_salloc(unit, nent * vmbytes, "vlan_mac");

    if (vmtab == NULL) {
        return BCM_E_MEMORY;
    }

    soc_mem_lock(unit, VLAN_MACm);
    rv = soc_mem_read_range(unit, VLAN_MACm, MEM_BLOCK_ANY,
            imin, imax, vmtab);
    soc_mem_unlock(unit, VLAN_MACm);

    if (rv < 0) {
        soc_cm_sfree(unit, vmtab);
        return rv;
    }

    soc_mem_lock(unit, VLAN_MACm);
    for(i = 0; i < nent; i++) {

        vment = soc_mem_table_idx_to_pointer(unit, VLAN_MACm,
                vlan_mac_entry_t *, vmtab, i);

        if (!soc_VLAN_MACm_field32_get(unit, vment, VALIDf)) {
            continue;
        }

        /* Read KeyType */
        key_type_value = soc_VLAN_MACm_field32_get(unit, vment, KEY_TYPEf);
        (void) _bcm_esw_vlan_xlate_key_type_get(unit, key_type_value,
                &key_type);
        switch(key_type) {
            case VLXLT_HASH_KEY_TYPE_VLAN_MAC_PORT:
                matched = TRUE;
                /* Read Key Fields */
                bcm_vlan_match_info_t_init(&match_info);
                (void) _bcm_trx_vlan_mac_port_key_fields_get(unit,
                        vment, &match_info);
                if (match_info.match == BCM_VLAN_MATCH_INVALID) {
                    matched = FALSE;
                    break;
                }

                /* Evaluate SVP */
                matched = FALSE;
                svp_valid = soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__SVP_VALIDf);
                if ((port != BCM_GPORT_INVALID) && (svp_valid == TRUE)) {
                    vp = soc_VLAN_MACm_field32_get(unit, vment, MAC_PORT__SVPf);
                    matched = _bcm_esw_port_vp_gport_check(port, vp);
                } else if ((port == BCM_GPORT_INVALID) && (svp_valid == FALSE)) {
                    matched = TRUE;
                } else {
                    /* (((port != BCM_GPORT_INVALID) && (svp_valid == FALSE)) ||
                       ((port == BCM_GPORT_INVALID) && (svp_valid == TRUE))) */
                    matched = FALSE;
                }
                /* Evaluate match criteria */
                if ((match != BCM_VLAN_MATCH_INVALID) &&
                        (match != match_info.match)) {
                    matched = FALSE;
                }
                break;
            default:
                matched = FALSE;
                break;
        }

        if (matched == FALSE) {
            continue;
        }

        if (soc_VLAN_MACm_field32_get(unit, &vment,
                    MAC_PORT__VLAN_ACTION_VALIDf)) {
            /* Read action profile data. */
            profile_idx = soc_VLAN_MACm_field32_get(unit,
                    vment, MAC_PORT__TAG_ACTION_PROFILE_PTRf);
            _bcm_trx_vlan_action_profile_entry_delete(unit, profile_idx);
        }

        rv = soc_mem_delete_return_old(unit, VLAN_MACm, MEM_BLOCK_ALL,
                vment, vment);
        if (rv != BCM_E_NONE) {
            break;
        }
    }

    soc_mem_unlock(unit, VLAN_MACm);
    soc_cm_sfree(unit, vmtab);

    return rv;
}

#else /* BCM_TRX_SUPPORT */
typedef int _triumph_vlan_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_TRX_SUPPORT */

