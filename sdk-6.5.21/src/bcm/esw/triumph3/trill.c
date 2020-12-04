/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW Triumph3 Trill API
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_TRIUMPH3_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <soc/ism_hash.h>

#include <bcm/types.h>
#include <bcm/l3.h>
#include <bcm/multicast.h>
#include <bcm/trill.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/trill.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>

typedef uint32 L2_ENTRY_T[SOC_MAX_MEM_FIELD_WORDS];

/*
 * Function:
 *		_bcm_tr3_trill_transit_entry_key_set
 * Purpose:
 *		Set TRILL Transit forwarding entry key
 * Parameters:
 *		IN :  Unit
 *  IN :  trill_port
 *  IN : next_hop / ecmp group index
 *  IN : MPLS entry pointer
 * Returns:
 *		BCM_E_XXX
 */


STATIC void
_bcm_tr3_trill_transit_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                int nh_ecmp_index, mpls_entry_extd_entry_t *tr_ent, int clean_flag)
{

    if (clean_flag) {
         sal_memset(tr_ent, 0, sizeof(mpls_entry_extd_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x0);
    if (trill_port->flags & _BCM_TRILL_PORT_MULTIPATH) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__ECMPf, 0x1);
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__ECMP_PTRf, nh_ecmp_index);
    } else {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__ECMPf, 0x0);
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__NEXT_HOP_INDEXf, nh_ecmp_index);
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__UCAST_DST_DISCARDf)) { 
        if ((trill_port->flags & BCM_TRILL_PORT_DROP) ||
            !(trill_port->flags & BCM_TRILL_PORT_NETWORK)) {
              soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x1);
        } else if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
              soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x0);
        }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__DST_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__DST_COPY_TO_CPUf, 0x1);
    }
    /* Multicast-traffic discard setting */
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) {
        if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
            soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x0);
        } else {
            soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x1);
        }
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_1f, 0x1);
}


/*
 * Function:
 *      _bcm_tr3_trill_transit_entry_set
 * Purpose:
 *     Set TRILL Transit forwarding entry
 * Parameters:
 *      IN :  Unit
 *      IN :  trill_port
 *      IN : next_hop / ecmp group index
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_tr3_trill_transit_entry_set(int unit, bcm_trill_port_t *trill_port,
                                         int nh_ecmp_index)
{
    int rv = BCM_E_NONE;
    int index=0;
    mpls_entry_extd_entry_t  key_ent;
    mpls_entry_extd_entry_t  return_ent;

    _bcm_tr3_trill_transit_entry_key_set(unit, trill_port, 
                        nh_ecmp_index, &key_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                        &key_ent, &return_ent, 0);
    if (rv == SOC_E_NONE) {
         (void) _bcm_tr3_trill_transit_entry_key_set(unit, trill_port, 
                                  nh_ecmp_index, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                                  MEM_BLOCK_ALL, index,
                                  &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &key_ent);
    }
    return rv;
}

/*
 * Function:
 *		_bcm_tr3_trill_transit_entry_reset
 * Purpose:
 *		Reset TRILL Transit forwarding entry
 * Parameters:
 *		IN :  Unit
 *  IN :  vp
 *  IN : next_hop / ecmp group index
 *  IN : ecmp flag
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_tr3_trill_transit_entry_reset(int unit, int vp, int nh_ecmp_idx, int ecmp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class=0;               /* Interface class ID. */
    mpls_entry_extd_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));
 
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                                         MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                         EGRESS_RBRIDGE_NICKNAMEf);

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                                         MEM_BLOCK_ALL, vp, &svp));

    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);


    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__DECAP_TRILL_TUNNELf, 0x0);
    if (ecmp) {
      soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__ECMPf, 0x1);
      soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__ECMP_PTRf, nh_ecmp_idx);
    } else {
      soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      TRILL__NEXT_HOP_INDEXf, nh_ecmp_idx);
    }
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                      VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         VALID_1f, 0x1);
    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                     &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &tr_ent);
    return rv;
}

/*
 * Function:
 *		_bcm_tr3_trill_learn_entry_key_set
 * Purpose:
 *		Set TRILL learn entry key
 * Parameters:
 *		IN :  Unit
 *  IN :  trill_port
 *  IN :  VP
 *  IN : MPLS entry pointer
 * Returns:
 *		BCM_E_XXX
 */

STATIC void
 _bcm_tr3_trill_learn_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                                          int vp, mpls_entry_extd_entry_t   *tr_ent, int clean_flag)
 {
    uint8 trill_tree_id=0;

    if (clean_flag) {
         sal_memset(tr_ent, 0, sizeof(mpls_entry_extd_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__VIRTUAL_PORTf, vp);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__PHB_FROM_OUTER_L2_HEADERf, 0x1);
    if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
         (void) bcm_td_trill_tree_profile_get (unit, trill_port->name, &trill_tree_id);
         if (trill_tree_id < BCM_MAX_NUM_TRILL_TREES) {
              soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__TREE_IDf, trill_tree_id);
         }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__SRC_DISCARDf) && 
        (trill_port->flags & BCM_TRILL_PORT_DROP)) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__SRC_DISCARDf, 0x1);
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__SRC_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__SRC_COPY_TO_CPUf, 0x1);
    }
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_1f, 0x1);
 }

/*
 * Function:
 *		_bcm_tr3_trill_learn_entry_set
 * Purpose:
 *		Set TRILL  Learn Entry 
 * Parameters:
 *		IN :  Unit
 *  IN :  trill_port
 *  IN : vp
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_tr3_trill_learn_entry_set(int unit, bcm_trill_port_t *trill_port, int vp)
{
    mpls_entry_extd_entry_t  key_ent;
    mpls_entry_extd_entry_t  return_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    _bcm_tr3_trill_learn_entry_key_set(unit, trill_port, vp, &key_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                        &key_ent, &return_ent, 0);

    if (rv == SOC_E_NONE) {
         (void) _bcm_tr3_trill_learn_entry_key_set(unit, trill_port, vp, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                                           MEM_BLOCK_ALL, index,
                                           &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &key_ent);
    }

    return rv;
}

/*
 * Function:
 *		_bcm_tr3_trill_learn_entry_reset
 * Purpose:
 *		Reset TRILL RPF entry
 * Parameters:
 *		IN :  Unit
 *  IN :  vp
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_tr3_trill_learn_entry_reset(int unit, int vp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class=0;               /* Interface class ID. */
    mpls_entry_extd_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                             MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                             EGRESS_RBRIDGE_NICKNAMEf);

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                             MEM_BLOCK_ALL, vp, &svp));

    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             TRILL__VIRTUAL_PORTf, vp);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             TRILL__PHB_FROM_OUTER_L2_HEADERf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                             VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         VALID_1f, 0x1);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &tr_ent);
    
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
       return rv;
    } else {
       return BCM_E_NONE;
    }
}


/*
 * Function:
 *		_bcm_tr3_trill_decap_entry_key_set
 * Purpose:
 *		Set TRILL Decap entry key
 * Parameters:
 *		IN :  Unit
 *  IN :  trill_port
 *  IN :  VP
 *  IN : MPLS entry pointer
 * Returns:
 *		BCM_E_XXX
 */

STATIC void
 _bcm_tr3_trill_decap_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                                          mpls_entry_extd_entry_t   *tr_ent, int clean_flag)
 {
    if (clean_flag) {
       sal_memset(tr_ent, 0, sizeof(mpls_entry_extd_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__UCAST_DST_DISCARDf)) { 
       if ((trill_port->flags & BCM_TRILL_PORT_DROP) ||
            !(trill_port->flags & BCM_TRILL_PORT_NETWORK)) {
              soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x1);
       } else if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
              soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x0);
        }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__DST_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         TRILL__DST_COPY_TO_CPUf, 0x1);
    }
    /* Multicast-traffic discard setting */
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) {
        if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
            soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x0);
        } else {
            soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x1);
        }
    }

    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_1f, 0x1);
 }

/*
 * Function:
 *           _bcm_tr3_trill_decap_entry_set
 * Purpose:
 *           Set TRILL  Decap  Entry 
 * Parameters:
 *           IN :  Unit
 *  IN :  trill_port
 * Returns:
 *           BCM_E_XXX
 */

int
_bcm_tr3_trill_decap_entry_set(int unit, bcm_trill_port_t *trill_port)
{
    mpls_entry_extd_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;
    mpls_entry_extd_entry_t  return_ent;

    _bcm_tr3_trill_decap_entry_key_set(unit, trill_port, &tr_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                        &tr_ent, &return_ent, 0);

    if (rv == SOC_E_NONE) {
         (void) _bcm_tr3_trill_decap_entry_key_set(unit, trill_port, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                                  MEM_BLOCK_ALL, index,
                                  &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
        rv = soc_mem_insert(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &tr_ent);
    }
    return rv;
}

/*
 * Function:
 *           _bcm_tr3_trill_decap_entry_reset
 * Purpose:
 *           Reset TRILL Decap entry
 * Parameters:
 *            IN :  Unit
 *  IN :  vp
 * Returns:
 *            BCM_E_XXX
 */

int
_bcm_tr3_trill_decap_entry_reset(int unit,  int vp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class=0;               /* Interface class ID. */
    mpls_entry_extd_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;
 
    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                         MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                         EGRESS_RBRIDGE_NICKNAMEf);
 
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                          MEM_BLOCK_ALL, vp, &svp));
 
    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);
 
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                          KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                          TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                          TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                          VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                                         VALID_1f, 0x1);
 
    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                          &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }

    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, &tr_ent);
    return rv;

}

/*
 * Function:
 *		_bcm_tr3_trill_multicast_transit_entry_key_set
 * Purpose:
 *		Set TRILL Multicast entry key
 * Parameters:
 *  IN :  Unit
 *  IN :  Trill Root name
 *  IN : MPLS entry pointer
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
 _bcm_tr3_trill_multicast_transit_entry_key_set (int unit, bcm_trill_name_t root_name,
                                          mpls_entry_extd_entry_t   *tr_ent)
{
    uint8  trill_tree_id=0;

    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                       TRILL__TREE_IDf, trill_tree_id);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                       KEY_TYPE_0f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         KEY_TYPE_1f, 26);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                       TRILL__RBRIDGE_NICKNAMEf, root_name);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                       VALID_0f, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, tr_ent,
                                         VALID_1f, 0x1);
    return BCM_E_NONE;
}



/*
 * Purpose:
 *      Enable TRILL Multicast traffic   
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 */

int
bcm_tr3_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name)
{
    mpls_entry_extd_entry_t  tr_key, tr_entry;
    int rv = BCM_E_UNAVAIL;
    int entry_index=0;

    sal_memset(&tr_key, 0, sizeof(mpls_entry_extd_entry_t));
    sal_memset(&tr_entry, 0, sizeof(mpls_entry_extd_entry_t));

    (void) _bcm_tr3_trill_multicast_transit_entry_key_set(unit, root_name, &tr_key);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &entry_index,
                        &tr_key, &tr_entry, 0);

    if (rv == SOC_E_NONE) {
         /* Enable Multicast traffic */
         if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) { 
              if (flags & BCM_TRILL_PORT_DROP) {
                   soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x1);
              } else {
                   soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x0);
              }
         }
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                        MEM_BLOCK_ALL, entry_index, &tr_entry);
    } else {
        /* Root Entry Must be present */
        return rv;
    }
    return rv;
}


/*
 * Purpose:
 *      Disable TRILL Multicast traffic   
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 */

int
bcm_tr3_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name)
{
    mpls_entry_extd_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));

    (void) _bcm_tr3_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
   
    /* Disable Multicast Traffic */
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) { 
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                        TRILL__MCAST_DST_DISCARDf, 0x1);
    }
    rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                   MEM_BLOCK_ALL, index, &tr_ent);
    return rv;
}

/*
 * Purpose:
 *      Set TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      mc_index   - (IN) Multicast index
 */

int
bcm_tr3_trill_multicast_transit_entry_set(int unit, uint32 flags, bcm_trill_name_t root_name, 
                                  int mc_index, uint8  trill_tree_id)
{
    mpls_entry_extd_entry_t  tr_key, tr_entry;
    int rv = BCM_E_UNAVAIL;
    int entry_index=0;

    sal_memset(&tr_key, 0, sizeof(mpls_entry_extd_entry_t));
    sal_memset(&tr_entry, 0, sizeof(mpls_entry_extd_entry_t));

    (void) _bcm_tr3_trill_multicast_transit_entry_key_set(unit, root_name, &tr_key);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &entry_index,
                        &tr_key, &tr_entry, 0);
    if (rv == SOC_E_NONE) {
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                        TRILL__L3MC_INDEXf, mc_index);
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                        TRILL__TREE_IDf, (uint32) trill_tree_id);
         /* Enable Multicast traffic */
         if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) { 
              if (flags & BCM_TRILL_PORT_DROP) {
                   soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x1);
              } else {
                   soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x0);
              }
         }
         rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                        MEM_BLOCK_ALL, entry_index, &tr_entry);
    } else {
        /* Root Entry Must be present */
        return rv;
    }
    return rv;

}

/*
 * Purpose:
 *       Reset TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      mc_index   - (IN) Multicast index
 */

int
bcm_tr3_trill_multicast_transit_entry_reset(int unit, bcm_trill_name_t root_name)
{
    mpls_entry_extd_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));

    (void) _bcm_tr3_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
   
    /* Remove l3mc_index from HW entry */
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                   TRILL__L3MC_INDEXf, 0);
    /* Remove Multicast Tree Id */
    soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                        TRILL__TREE_IDf, (uint32) 0);
    /* Disable Multicast Traffic */
    if (soc_mem_field_valid(unit, MPLS_ENTRY_EXTDm, TRILL__MCAST_DST_DISCARDf)) { 
         soc_mem_field32_set(unit, MPLS_ENTRY_EXTDm, &tr_ent,
                        TRILL__MCAST_DST_DISCARDf, 0x1);
    }
    rv = soc_mem_write(unit, MPLS_ENTRY_EXTDm,
                   MEM_BLOCK_ALL, index, &tr_ent);
    return rv;
}

/*
 * Purpose:
 *       Get TRILL Multicast entry for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 *      group   - (OUT) Multicast Group
 */

int
bcm_tr3_trill_multicast_transit_entry_get(int unit, bcm_trill_name_t root_name, bcm_multicast_t *group)
{
    mpls_entry_extd_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_extd_entry_t));

    (void) _bcm_tr3_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
    _BCM_MULTICAST_GROUP_SET(*group, 
                  _BCM_MULTICAST_TYPE_TRILL,  
                  soc_mem_field32_get (unit, MPLS_ENTRY_EXTDm, (uint32 *)&tr_ent,
                    TRILL__L3MC_INDEXf));
    return rv;
}

/*
 * Purpose:
 *       Reset all TRILL Multicast entries for Transit operation
 * Parameters:
 *      unit     - (IN) Device Number
 *      tree_id - (IN) trill Tree ID
 *      mc_index   - (IN) Multicast index
 */

int
bcm_tr3_trill_multicast_transit_entry_reset_all (int unit, uint8 tree_id)
{
    int rv = BCM_E_UNAVAIL, idx=0, num_entries=0;
    mpls_entry_extd_entry_t ment;
    bcm_trill_name_t name;

    /* Tree ID and Root name is 1:1 mapping.
       Match on Rbridge name to match */
    bcm_td_trill_root_name_get(unit, tree_id, &name);

    num_entries = soc_mem_index_count(unit, MPLS_ENTRY_EXTDm);
    for (idx = 0; idx < num_entries; idx++) {
        rv = READ_MPLS_ENTRY_EXTDm(unit, MEM_BLOCK_ANY, idx, &ment);
        if (rv < 0) {
            return rv;
        }
        if (!soc_MPLS_ENTRY_EXTDm_field32_get(unit, &ment, VALID_0f)) {
            continue;
        }
        if (soc_MPLS_ENTRY_EXTDm_field32_get(unit, &ment, KEY_TYPE_0f) != 0x1A) {
            continue;
        }

        if ((soc_MPLS_ENTRY_EXTDm_field32_get(unit, &ment,
                                        TRILL__TREE_IDf) != tree_id) ||
           (soc_MPLS_ENTRY_EXTDm_field32_get(unit, &ment,
                                        TRILL__RBRIDGE_NICKNAMEf) != name)) {
            continue;
        }


        /* Entry found, for a given RBridge Name + Tree ID Key
         * there can be only one entry. Hence stop when entry found.
         */
        break;
    }

    /* Set L3MC index to 0, reserved entry with BITMAP = 0 */
    soc_MPLS_ENTRY_EXTDm_field32_set(unit, &ment, TRILL__L3MC_INDEXf, 0);

    /* Write to  HW */
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, MPLS_ENTRY_EXTDm, MEM_BLOCK_ALL, idx, &ment));

    return BCM_E_NONE;
}

/*
 * Purpose:
 *      Add TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_trill_l2_multicast_entry_add(int unit, uint32 flags, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int mc_index=0,l2_index=0;
    vlan_tab_entry_t  vt;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2_ENTRY_1m;

    if (_BCM_MULTICAST_IS_SET(group)) {

        mc_index = _BCM_MULTICAST_ID_GET(group);

        switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__DEST_TYPEf, 0);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__L2MC_PTRf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                  L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
                mem = L2_ENTRY_2m;
                field_key =  SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS;
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     KEY_TYPE_0f, field_key);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     KEY_TYPE_1f, field_key);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALID_0f, 1);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALID_1f, 1);
                if (flags & BCM_TRILL_MULTICAST_STATIC) { 
                    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                  STATIC_BIT_1f, 0x1);
                    soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                  STATIC_BIT_0f, 0x1);
                }
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     L2__DEST_TYPEf, 0x3);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                     L2__L2MC_PTRf, mc_index);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                     L2__VLAN_IDf, vid);
                soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                     L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT;
            if (flags & BCM_TRILL_MULTICAST_STATIC) { 
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                              STATIC_BITf, 0x1);
            }
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf, trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);
            BCM_IF_ERROR_RETURN(soc_mem_read(unit, VLAN_TABm, 
                                  MEM_BLOCK_ANY, vid, &vt));

            if (soc_mem_field32_get(unit, VLAN_TABm, &vt, 
                    TRILL_ACCESS_RECEIVERS_PRESENTf)) {
                  soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 1);
            }

            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG;
            if (flags & BCM_TRILL_MULTICAST_STATIC) { 
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                              STATIC_BITf, 0x1);
            }
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf, trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TRILL_ACCESS_RECEIVERS_PRESENTf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf, mc_index);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                  TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;

        default:
            break;
        }
    }


    /* See if the entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);

    if (rv == SOC_E_NONE) {
        if (key == TR_L2_HASH_KEY_TYPE_BRIDGE) {
            soc_mem_field32_set(unit, mem, (uint32*)&l2_lookup,
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            rv = soc_mem_write(unit, mem,
                               MEM_BLOCK_ALL, l2_index,
                               &l2_lookup);
        }
    } else if ((rv < 0) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    } else {
        /* Add Entry */
        rv = soc_mem_insert(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
        if ((rv == SOC_E_FULL) &&
            ((key == TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT) ||
            (key == TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG)) &&
            (flags & BCM_TRILL_MULTICAST_REPLACE_DYNAMIC)) {
            /* For TRILL_NW_SHORT and TRILL_NW_LONG, if the table is FULL,
               replace the existing entry */
            _bcm_tr3_l2_entries_t l2_entries;
            sal_memset(&l2_entries, 0, sizeof(l2_entries));
            sal_memcpy((void *)&(l2_entries.l2_entry_1),
                           (void *)&l2_entry,
                           sizeof(l2_entry_1_entry_t));
            l2_entries.entry_flags = _BCM_TR3_L2_SELECT_L2_ENTRY_1;
            rv = _bcm_tr3_l2_hash_dynamic_replace(unit, &l2_entries);
        }
    }
    return rv;
}


/*
 * Purpose:
 *      Update TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - Trill Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_trill_l2_multicast_entry_update(int unit, int key, 
                                    bcm_vlan_t vid, bcm_mac_t mac, 
                                    uint8 trill_tree_id, bcm_multicast_t group)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2_ENTRY_1m;

    if (_BCM_MULTICAST_IS_TRILL(group)) {

        switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                    VALIDf, 1);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                  L2__MAC_ADDRf, mac);
            break;

        }
    }

    /* See if the entry already exists */
    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index,
                        (void *)&l2_entry, (void *)&l2_lookup, 0);
    if (rv == SOC_E_NONE) {
        if (key == TR_L2_HASH_KEY_TYPE_BRIDGE) {
            soc_mem_field32_set(unit, mem, (uint32*)&l2_lookup,
                                 L2__TRILL_NETWORK_RECEIVERS_PRESENTf, 1);
            rv = soc_mem_write(unit, mem,
                               MEM_BLOCK_ALL, l2_index,
                               &l2_lookup);
        }
    } else {
        /* Entry must exist */
        return rv;
    }
    return rv;
}

/*
 * Purpose:
 *      Delete TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_trill_l2_multicast_entry_delete(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id)
{
    L2_ENTRY_T  l2_entry, l2_lookup;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));
    sal_memset(&l2_lookup, 0, sizeof (l2_lookup));

    mem = L2_ENTRY_1m;

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            mem = L2_ENTRY_2m;
            field_key =  SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPE_0f, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPE_1f, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                 L2__MAC_ADDRf, mac);

            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;
                        
        default:
            break;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2_entry, (void *)&l2_lookup, 0);
                 
    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } 

    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ANY, (void *)&l2_entry);
    return rv;
}

/*
 * Description:
 *      Delete all TRILL entries in L2X table
 * Parameters:
 *     unit             - device number
 *     tree_id          - Trill Tree ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_l2x_entry_delete_all (int unit, soc_mem_t mem, uint8 tree_id)
{
    int           ix, key_type, chnk_end;
    int           chunksize, chunk, chunk_max, mem_idx_max;
    L2_ENTRY_T   *l2e, *l2ep;
    soc_control_t *soc = SOC_CONTROL(unit);
    int           rv = BCM_E_NONE;
    int           key_match1, key_match2 = -1;
    int           valid_fld, key_fld;
    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    switch(mem) {
        case L2_ENTRY_1m:
            valid_fld  = VALIDf;
            key_fld    = KEY_TYPEf;
            key_match1 = SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT;
            key_match2 = SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG;
            break;
        case L2_ENTRY_2m:
            valid_fld  = VALID_0f;
            key_fld    = KEY_TYPE_0f;
            key_match1 = SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS;
            break;
        default:
            return BCM_E_INTERNAL;
    }


    l2e = soc_cm_salloc(unit, chunksize * sizeof(*l2e), "l2entrydel_chunk");
    if (l2e == NULL) {
        return BCM_E_MEMORY;
    }
    mem_idx_max = soc_mem_index_max(unit, mem);
    soc_mem_lock(unit, mem);
    for (chunk = soc_mem_index_min(unit, mem); chunk <= mem_idx_max;
         chunk += chunksize) {

        chunk_max = chunk + chunksize - 1;
        if (chunk_max > mem_idx_max) {
            chunk_max = mem_idx_max;
        }

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chunk, chunk_max, l2e);
        if (rv < 0) {
            break;
        }
        chnk_end = (chunk_max - chunk);
        for (ix = 0; ix <= chnk_end; ix++) {
            l2ep = soc_mem_table_idx_to_pointer(unit, mem,
                                                 L2_ENTRY_T *, l2e, ix);
            if (!soc_mem_field32_get(unit, mem, l2ep, valid_fld)) {
                continue;
            }

            /* Match the Key Type to TRILL */
            key_type = soc_mem_field32_get(unit, mem, l2ep, key_fld);

            if (key_match2 != -1) {
                /* Match both LONG and SHORT keys */
                if ((key_type != key_match2) && (key_type != key_match1)) {
                    continue;
                }

                /* Match Tree ID */
                if ((tree_id != soc_mem_field32_get (unit, mem, l2ep,
                                                      TRILL_NONUC_NETWORK_LONG__TREE_IDf)) &&
                    (tree_id != soc_mem_field32_get(unit, mem, l2ep,
                                                     TRILL_NONUC_NETWORK_SHORT__TREE_IDf))){
                    continue;
                }

            } else {
                if (key_type != key_match1) {
                    continue;
                }
            }

            if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
                rv = BCM_E_RESOURCE;
                break;
            }
            if (((rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l2ep)) < 0) ||
                ((rv = soc_tr3_l2x_sync_delete(unit, mem, (uint32 *) l2ep,
                                           (chunk + ix), 0)) < 0)) {
                SOC_L2_DEL_SYNC_UNLOCK(soc);
                break;
            }
            SOC_L2_DEL_SYNC_UNLOCK(soc);
        }
        if (rv < 0) {
            break;
        }
    }
    soc_mem_unlock(unit, mem);
    soc_cm_sfree(unit, l2e);
    return rv;
}

/*
 * Description:
 *      Delete all TRILL tree-sepecific Multicast entries
 * Parameters:
 *     unit             - device number
 *     tree_id          - Trill Tree ID
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_l2_multicast_entry_delete_all (int unit, uint8 tree_id)
{
    int rv = BCM_E_NONE;
    BCM_IF_ERROR_RETURN(
        bcm_tr3_trill_l2x_entry_delete_all(unit, L2_ENTRY_1m, tree_id));

    rv = bcm_tr3_trill_l2x_entry_delete_all(unit, L2_ENTRY_2m, tree_id);
    return rv;
}

/*
 * Purpose:
 *      Get TRILL Multicast entry
 * Parameters:
 *      unit            - Device unit number
 *      key             - defines entry type
 *      vid             - Customer Vlan ID
 *      mac             - Customer MAC
 *      trill_tree_id   - specify Multicast Tree ID
 *      group           - (OUT) Multicast group
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int 
bcm_tr3_trill_l2_multicast_entry_get(int unit, int key, bcm_vlan_t vid, 
                                       bcm_mac_t mac, uint8 trill_tree_id, bcm_multicast_t *group)
{
    L2_ENTRY_T  l2_entry;
    soc_mem_t mem; 
    soc_field_t field_key;
    int rv=BCM_E_NONE;
    int l2_index=0;

    sal_memset(&l2_entry, 0, sizeof (l2_entry));

    mem = L2_ENTRY_1m;

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            field_key = SOC_MEM_KEY_L2_ENTRY_1_L2_BRIDGE;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                L2__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 L2__MAC_ADDRf, mac);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
                mem = L2_ENTRY_2m;
                field_key =  SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS;
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                    KEY_TYPE_0f, field_key);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                    KEY_TYPE_1f, field_key);
                soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                    L2__VLAN_IDf, vid);
                soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry,
                                     L2__MAC_ADDRf, mac);
           break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT;          
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                TRILL_NONUC_NETWORK_SHORT__VLAN_IDf, vid);
            break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
            field_key =  
            SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG;
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry,
                                KEY_TYPEf, field_key);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__TREE_IDf,
                                trill_tree_id);
            soc_mem_field32_set(unit, mem, (uint32 *)&l2_entry, 
                                TRILL_NONUC_NETWORK_LONG__VLAN_IDf, vid);
            soc_mem_mac_addr_set(unit, mem, (uint32 *)&l2_entry, 
                                 TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, mac);
            break;
                        
        default:
            break;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l2_index, 
                        (void *)&l2_entry, (void *)&l2_entry, 0);
                 
    if (BCM_FAILURE(rv)) {
        return rv;
    } 

    switch (key) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_L2,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L2MC_PTRf));
                break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS:
            if (SOC_IS_TRIUMPH3(unit)) {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L2MC_PTRf));
                break;
            }  else if (SOC_IS_TRIDENT2X(unit)) {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    L2__L3MC_PTRf));
                break;
            } else {
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_ACCESS__L3MC_PTRf));
                break;
            }

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf));
                break;

        case TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG:
                _BCM_MULTICAST_GROUP_SET(*group, 
                                  _BCM_MULTICAST_TYPE_TRILL,  
                                  soc_mem_field32_get (unit, mem, (uint32 *)&l2_entry,
                                    TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf));
                break;
        default:
            break;
    }
    return rv;
}


/*
 * Description:
 *      Traverse  TRILL Long and Short Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_l2_network_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    uint16     tree_id = 0;
    bcm_trill_multicast_entry_t trill_mc;


    mem = L2_ENTRY_1m;
    
    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill network multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
          
            /* Match the Key Type to TRILL */
            if (key_type != SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG) {
                if (key_type != SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT) {
                    continue;
                }
            }
           

            switch (key_type) {
                case SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT:
                                tree_id = soc_mem_field32_get(unit, mem, l2_entry,
                                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf);
                                /* Check for Valid Tree_Id */
                                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                                   continue;
                                }
                                (void) bcm_td_trill_root_name_get (unit, tree_id, &trill_mc.root_name);
                                _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                                  _BCM_MULTICAST_TYPE_TRILL,  
                                                  soc_mem_field32_get (unit, mem, l2_entry,
                                                    TRILL_NONUC_NETWORK_SHORT__L3MC_INDEXf));
                                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_SHORT__VLAN_IDf);
                                break;

                case SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_LONG_TRILL_NONUC_NETWORK_LONG:
                                tree_id = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_LONG__TREE_IDf);
                                /* Check for Valid Tree_Id */
                                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                                    continue;
                                }
                                (void) bcm_td_trill_root_name_get (unit, tree_id, &trill_mc.root_name);
                                _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                                  _BCM_MULTICAST_TYPE_TRILL,  
                                                  soc_mem_field32_get (unit, mem, l2_entry,
                                                    TRILL_NONUC_NETWORK_LONG__L3MC_INDEXf));
                                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                                  TRILL_NONUC_NETWORK_LONG__VLAN_IDf);
                                soc_mem_mac_addr_get(unit, mem, l2_entry,
                                                   TRILL_NONUC_NETWORK_LONG__MAC_ADDRESSf, trill_mc.c_dmac);
                                break;

            }
            trill_mc.flags = BCM_TRILL_MULTICAST_STATIC;
            rv = trav_st->user_cb(unit, &trill_mc, trav_st->user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}

/*
 * Description:
 *      Traverse  TRILL Access to Network Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_l2_access_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    bcm_trill_multicast_entry_t  trill_mc;

    mem = L2_ENTRY_2m;
    
    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill access multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

                if (!soc_mem_field32_get(unit, mem, l2_entry, VALID_0f)) {
                    continue;
                }
                key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPE_0f);
                if (key_type != SOC_MEM_KEY_L2_ENTRY_2_L2_TRILL_NONUC_ACCESS) {
                    continue;
                } else {
                    _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                                      _BCM_MULTICAST_TYPE_TRILL,  
                                      soc_mem_field32_get (unit, mem, l2_entry,
                                      L2__L2MC_PTRf));
                    trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                      L2__VLAN_IDf);
                    soc_mem_mac_addr_get(unit, mem, l2_entry,
                                      L2__MAC_ADDRf, trill_mc.c_dmac);
                }
           
            trill_mc.flags = BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK | BCM_TRILL_MULTICAST_STATIC;
            rv = bcm_td_multicast_trill_rootname_get(unit, trill_mc.group, &trill_mc.root_name);
            if (BCM_FAILURE(rv)) {
                 continue;
            }

            rv = trav_st->user_cb(unit, &trill_mc, trav_st->user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}



/*
 * Description:
 *      Traverse  TRILL Unpruned Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx=0, ent_idx=0, chnk_idx_max=0, mem_idx_max=0;
    int             buf_size=0, chunksize=0, chnk_end=0;
    uint32          *unpruned_trill_tbl_chnk;
    mpls_entry_extd_entry_t *ment;
    int             rv = BCM_E_NONE;
    soc_mem_t     mem;
    int    key_type=0;
    uint8   tree_id=0xff;
    bcm_trill_name_t  root_name=0;
    bcm_trill_multicast_entry_t  trill_mc;

    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    mem = MPLS_ENTRY_EXTDm;

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = 4096;

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    unpruned_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill multicast unpruned traverse");
    if (NULL == unpruned_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; chnk_idx += chunksize) {
        sal_memset((void *)unpruned_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, unpruned_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            ment = soc_mem_table_idx_to_pointer(unit, mem, mpls_entry_extd_entry_t *, 
                                             unpruned_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, ment, VALID_0f)) {
                continue;
            }
            if (!soc_mem_field32_get(unit, mem, ment, VALID_1f)) {
                continue;
            }
          
            /* Match the Key Type to TRILL */
            key_type = soc_mem_field32_get(unit, mem, ment, KEY_TYPE_0f);

            if (key_type != 0x1A) { /* RBRIDGE_NICKNAME */
                continue;
            }
            key_type = soc_mem_field32_get(unit, mem, ment, KEY_TYPE_1f);

            if (key_type != 0x1A) { /* RBRIDGE_NICKNAME */
                continue;
            }

            /* Skip entries that discard multicast destinations */
            if (soc_mem_field_valid(unit, mem, 
                                    TRILL__MCAST_DST_DISCARDf)) {
                if (soc_mem_field32_get(unit,
                                        mem, ment, TRILL__MCAST_DST_DISCARDf)) {
                    continue;
                }
            }


            /* Check for Valid Tree_Id */
            tree_id = soc_mem_field32_get(unit, mem, ment, TRILL__TREE_IDf);
            if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                continue;
            }
            root_name = soc_mem_field32_get(unit, mem, ment, TRILL__RBRIDGE_NICKNAMEf);
            (void) bcm_td_trill_root_name_get (unit, tree_id, &trill_mc.root_name);
            if (root_name != trill_mc.root_name) {
                continue;
            }

            _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                              _BCM_MULTICAST_TYPE_TRILL,  
                              soc_mem_field32_get (unit, mem, ment,
                                TRILL__L3MC_INDEXf));

            rv = trav_st->user_cb(unit, &trill_mc, trav_st->user_data);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    soc_cm_sfree(unit, unpruned_trill_tbl_chnk);
    return rv;        
}

/*
 * Description:
 *      Modify  TRILL short Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_tr3_trill_vlan_multicast_entry_modify (int unit, bcm_vlan_t vlan, int set_bit)
{
   /* Indexes to iterate over memories, chunks and entries */
    int        chnk_idx, ent_idx, chnk_idx_max, mem_idx_max;
    int        buf_size, chunksize, chnk_end;
    /* Buffer to store chunk of L2 table we currently work on */
    uint32     *l2_trill_tbl_chnk;
    L2_ENTRY_T *l2_entry;
    int        rv = BCM_E_NONE;
    soc_mem_t  mem;
    int        key_type=-1;
    uint16     tree_id = 0;
    bcm_trill_multicast_entry_t trill_mc;
    int count=0;

    mem = L2_ENTRY_1m;
    
    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = soc_property_get(unit, spn_L2DELETE_CHUNKS,
                                 L2_MEM_CHUNKS_DEFAULT);

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    l2_trill_tbl_chnk = soc_cm_salloc(unit, buf_size, "trill network multicast traverse");
    if (NULL == l2_trill_tbl_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); chnk_idx <= mem_idx_max; 
         chnk_idx += chunksize) {
        sal_memset((void *)l2_trill_tbl_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, l2_trill_tbl_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            l2_entry = soc_mem_table_idx_to_pointer(unit, mem, L2_ENTRY_T *, 
                                             l2_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, l2_entry, VALIDf)) {
                continue;
            }

            key_type = soc_mem_field32_get(unit, mem, l2_entry, KEY_TYPEf);
          
            /* Match the Key Type to TRILL */
            if (key_type != SOC_MEM_KEY_L2_ENTRY_1_TRILL_NONUC_NETWORK_SHORT_TRILL_NONUC_NETWORK_SHORT) {
               continue;
            } else {
                tree_id = soc_mem_field32_get(unit, mem, l2_entry,
                                 TRILL_NONUC_NETWORK_SHORT__TREE_IDf);
                /* Check for Valid Tree_Id */
                if (tree_id >= BCM_MAX_NUM_TRILL_TREES ) {
                   continue;
                }
                trill_mc.c_vlan = soc_mem_field32_get (unit, mem, l2_entry,
                                  TRILL_NONUC_NETWORK_SHORT__VLAN_IDf);
                if (vlan ==  trill_mc.c_vlan) {
                   if (set_bit) {
                       soc_mem_field32_set(unit, mem, (uint32 *)l2_entry, 
                             TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 0x1);
                   } else {
                       soc_mem_field32_set(unit, mem, (uint32 *)l2_entry, 
                             TRILL_NONUC_NETWORK_SHORT__TRILL_ACCESS_RECEIVERS_PRESENTf, 0x0);
                   }
                   count ++;
                }
            }
        }
        if (count > 0) {
            rv = soc_mem_write_range(unit, SOURCE_VPm,
                                     MEM_BLOCK_ANY, chnk_idx,
                                     chnk_idx_max, l2_trill_tbl_chnk);
        } 
    }
    soc_cm_sfree(unit, l2_trill_tbl_chnk);
    return rv;        
}

#endif /* BCM_TRIUMPH3_SUPPORT && INCLUDE_L3 */

