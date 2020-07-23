/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ESW Trill API
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIDENT_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/util.h>
#include <soc/hash.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm/types.h>
#include <bcm/l3.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <soc/ism_hash.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#include <bcm/multicast.h>
#include <bcm/trill.h>
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm/trunk.h>
#endif

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/triumph3.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/trill.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/trx.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0
#endif /* BCM_WARM_BOOT_SUPPORT */

_bcm_td_trill_bookkeeping_t   *_bcm_td_trill_bk_info[BCM_MAX_NUM_UNITS] = { 0 };

#define TRILL_INFO(_unit_)   (_bcm_td_trill_bk_info[_unit_])
#define L3_INFO(_unit_)   (&_bcm_l3_bk_info[_unit_])

#define BCM_TRILL_CHECK_ERROR_RETURN(_op, _u_, _vp)  do { \
    int _rv = _op; \
    if (_rv < 0) { \
         (void) _bcm_vp_free(_u_, _bcmVpTypeTrill, 1, _vp); \
         return (_rv); \
    } \
} while (0)


#define BCM_TRILL_L2MC_CHECK( _u_ , index) \
    if ((index < 0) ||\
         (index >= soc_mem_index_count(_u_, L2MCm))) { \
          return BCM_E_PARAM; \
    } \

#define BCM_TRILL_L3MC_CHECK( _u_ , index) \
    if ((index < 0) ||\
         (index >= soc_mem_index_count(_u_, L3_IPMCm))) { \
         return BCM_E_PARAM; \
    } \

typedef uint32 L3_ENTRY_T[SOC_MAX_MEM_FIELD_WORDS];

STATIC int _bcm_td_trill_adjacency_set(int unit, int nh_index);
STATIC int _bcm_td_trill_adjacency_reset(int unit, int nh_index);

/*
 * Function:
 *      _bcm_trill_check_init
 * Purpose:
 *      Check if Trill is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


STATIC int 
_bcm_trill_check_init (int unit)
{
    _bcm_td_trill_bookkeeping_t *trill_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    if ((trill_info == NULL) || (trill_info->initialized == FALSE)) { 
         return BCM_E_INIT;
    } else {
         return BCM_E_NONE;
    }
}



/*
 * Function:
 *      bcm_trill_lock
 * Purpose:
 *      Take the Trill Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


 int 
 bcm_td_trill_lock (int unit)
{
    int rv = BCM_E_NONE;

   rv = _bcm_trill_check_init (unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(TRILL_INFO((unit))->trill_mutex, sal_mutex_FOREVER);
   }
   return rv; 
}



/*
 * Function:
 *      bcm_trill_unlock
 * Purpose:
 *      Release  the Trill Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


void
bcm_td_trill_unlock(int unit)
{
    int rv = BCM_E_NONE;

    rv = _bcm_trill_check_init (unit);
    if ( rv == BCM_E_NONE ) {
         sal_mutex_give(TRILL_INFO((unit))->trill_mutex);
    }
}



/*
 * Function:
 *      _bcm_td_trill_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */


STATIC void
_bcm_td_trill_free_resource(int unit)
{
    _bcm_td_trill_bookkeeping_t *trill_info;

    trill_info = TRILL_INFO(unit);

    /* If software tables were not allocated we are done. */ 
    if (NULL == TRILL_INFO(unit)) {
        return;
    }

    if (trill_info->rBridge) {
        sal_free(trill_info->rBridge);
        trill_info->rBridge = NULL;
    }

    if (trill_info->multicast_count) {
        sal_free(trill_info->multicast_count);
        trill_info->multicast_count = NULL;
    }

    /* Free module data. */
    sal_free(TRILL_INFO(unit));
    TRILL_INFO(unit) = NULL;
}

/*
 * Function:
 *      bcm_td_trill_tpid_add
 * Purpose:
 *      Add Tpid for TRILL 
 * Parameters:
 *      unit - SOC unit number.
 *      port - Trill gport
 *      tpid -  Tag protocol ID
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_tpid_add(int unit, bcm_gport_t port, int tpid_idx)
{
    int rv = BCM_E_NONE;
    uint32 ena_f;
    ing_trill_payload_parse_control_entry_t tpid_ent;

    sal_memset(&tpid_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                    MEM_BLOCK_ALL, 0, &tpid_ent));
    ena_f = soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_get
                                (unit, &tpid_ent, TPID_ENABLEf);

    if (!(ena_f & (1 << tpid_idx))) {
        ena_f |= 1 << tpid_idx;
        soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_set
                        (unit, &tpid_ent, TPID_ENABLEf, ena_f);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                              MEM_BLOCK_ALL, 0, &tpid_ent));
    }
    return rv;
}


/*
 * Function:
 *      bcm_td_trill_tpid_set
 * Purpose:
 *      Set Tpid for TRILL 
 * Parameters:
 *      unit - SOC unit number.
 *      port - Trill gport
 *      tpid -  Tag protocol ID
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_tpid_set(int unit, bcm_gport_t port, int tpid_idx)
{
    int rv = BCM_E_NONE;
    uint32 ena_f;
    ing_trill_payload_parse_control_entry_t tpid_ent;

    sal_memset(&tpid_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                    MEM_BLOCK_ALL, 0, &tpid_ent));
    ena_f = soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_get
                                (unit, &tpid_ent, TPID_ENABLEf);

    if (!(ena_f & (1 << tpid_idx))) {
        ena_f = 1 << tpid_idx;
        soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_set
                        (unit, &tpid_ent, TPID_ENABLEf, ena_f);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                              MEM_BLOCK_ALL, 0, &tpid_ent));
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_tpid_get
 * Purpose:
 *      Get TRILL TPID
 * Parameters:
 *      unit - SOC unit number
 *      port - Trill gport
 *      tpid - Tag protocol ID
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_tpid_get(int unit, bcm_gport_t port, int *tpid_index)
{
    int rv = BCM_E_NONE;
    ing_trill_payload_parse_control_entry_t tpid_ent;

    sal_memset(&tpid_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                    MEM_BLOCK_ALL, 0, &tpid_ent));

    *tpid_index = soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_get
                                    (unit, &tpid_ent, TPID_ENABLEf);
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_tpid_delete
 * Purpose:
 *      Delete Tpid for TRILL 
 * Parameters:
 *      unit - SOC unit number.
 *      port - Trill gport
 *      tpid -  Tag protocol ID
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_tpid_delete(int unit, bcm_gport_t port, int tpid_idx)
{
    int rv = BCM_E_NONE;
    uint32 ena_f;
    ing_trill_payload_parse_control_entry_t tpid_ent;

    sal_memset(&tpid_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                    MEM_BLOCK_ALL, 0, &tpid_ent));
    ena_f = soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_get
                                (unit, &tpid_ent, TPID_ENABLEf);

    if (ena_f & (1 << tpid_idx)) {
        ena_f &= ~(1 << tpid_idx);
        soc_ING_TRILL_PAYLOAD_PARSE_CONTROLm_field32_set
                        (unit, &tpid_ent, TPID_ENABLEf, ena_f);
        BCM_IF_ERROR_RETURN(
                soc_mem_write(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                              MEM_BLOCK_ALL, 0, &tpid_ent));
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_ethertype_set
 * Purpose:
 *      Enable TRILL functionality
 * Parameters:
 *      unit - SOC unit number.
 *      etherType -  Non-zero value: enable TRILL support
 *                          Zero vlaue: Disable TRILL support
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_ethertype_set(int unit, uint16 etherType)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;
    egr_trill_parse_control_entry_t egr_ent;
    ing_trill_payload_parse_control_entry_t tpid_ent;

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));	
    sal_memset(&egr_ent, 0, sizeof(egr_trill_parse_control_entry_t));
    sal_memset(&tpid_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));

    if (etherType > 0) {

         if (SOC_MEM_IS_VALID(unit, ING_TRILL_PARSE_CONTROLm)) {
             soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                                         TRILL_ETHERTYPE_ENABLEf, 0x1);
             soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                                         TRILL_ETHERTYPEf, etherType);
         }

         soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent,
                                         TRILL_ETHERTYPE_ENABLEf, 0x1);
         soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent,
                                         TRILL_ETHERTYPEf, etherType);
         if (SOC_MEM_IS_VALID(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm)) {
             soc_mem_field32_set(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm, &tpid_ent,
                                             TPID_ENABLEf, 0x1);
         }
    }

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                            MEM_BLOCK_ALL, 0, &ing_ent));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROLm,
                                            MEM_BLOCK_ALL, 0, &egr_ent));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                                            MEM_BLOCK_ALL, 0, &tpid_ent));
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_ethertype_get
 * Purpose:
 *      Get TRILL EtherType
 * Parameters:
 *      unit - SOC unit number.
 *      etherType - return value
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_ethertype_get(int unit, int *etherType)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));

    *etherType = soc_mem_field32_get(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                                         TRILL_ETHERTYPEf);
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_ISIS_ethertype_set
 * Purpose:
 *      Set TRILL ISIS EtherType
 * Parameters:
 *      unit - SOC unit number.
 *      etherType -  Non-zero value: enable Ethertype
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_ISIS_ethertype_set(int unit, uint16 etherType)
{
    int rv = BCM_E_NONE;
    int en = 0;
    ing_trill_parse_control_entry_t  ing_ent;
    egr_trill_parse_control_entry_t egr_ent;    
    egr_trill_parse_control_2_entry_t egr2_ent;

    if (!soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             L2_IS_IS_ETHERTYPE_ENABLEf) ||
        !soc_mem_field_valid(unit, EGR_TRILL_PARSE_CONTROLm, 
                        TRILL_L2_IS_IS_ETHERTYPE_ENABLEf) ||
        !soc_mem_field_valid(unit, EGR_TRILL_PARSE_CONTROL_2m, 
                        TRILL_L2_IS_IS_ETHERTYPE_ENABLEf)) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                  MEM_BLOCK_ALL, 0, &ing_ent));
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_TRILL_PARSE_CONTROLm,
                  MEM_BLOCK_ALL, 0, &egr_ent));
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_TRILL_PARSE_CONTROL_2m,
                  MEM_BLOCK_ALL, 0, &egr2_ent));

    /* enable if ethertype > 0 */
    en = (etherType > 0) ? 1 : 0;

    soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                              L2_IS_IS_ETHERTYPE_ENABLEf, en);
    soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                              L2_IS_IS_ETHERTYPEf, etherType);
    soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent,
                              TRILL_L2_IS_IS_ETHERTYPE_ENABLEf, en);
    soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent,
                              TRILL_L2_IS_IS_ETHERTYPEf, etherType);
    soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROL_2m, &egr2_ent,
                              TRILL_L2_IS_IS_ETHERTYPE_ENABLEf, en);
    soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROL_2m, &egr2_ent,
                               TRILL_L2_IS_IS_ETHERTYPEf, etherType);

    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &egr_ent));
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROL_2m,
                                       MEM_BLOCK_ALL, 0, &egr2_ent));

    return rv;
}

/*
 * Function:
 *      bcm_td_trill_ISIS_ethertype_get
 * Purpose:
 *      Get TRILL ISIS EtherType
 * Parameters:
 *      unit - SOC unit number.
 *      etherType - return value
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_td_trill_ISIS_ethertype_get(int unit, int *etherType)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));

    if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             L2_IS_IS_ETHERTYPEf)) {
         *etherType = soc_mem_field32_get(unit, ING_TRILL_PARSE_CONTROLm, 
                                  &ing_ent, L2_IS_IS_ETHERTYPEf);
    } else {
         *etherType = 0;
         rv = BCM_E_UNAVAIL;  /* BCM_E_UNAVAIL if the field is not supported */
    }

    return rv;
}


/*
 * Function:
 *      _bcm_td_trill_mac_set
 * Description:
 *      flag = 0:  Set the lower 24 bits of MAC address field for TRILL specific MAC
 *      flag = 1:  Set the higher 24 bits of MAC address field for TRILL specific MAC
 * Parameters:
 *      unit       - unit number (driver internal).
 *      type      - The required switch control type to set upper MAC for
 *      mac      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC void
_bcm_td_trill_mac_set(int unit, uint32 mac_field, soc_mem_t mem, 
                        void *entry, soc_field_t field, sal_mac_addr_t mac, uint8 flag) 
{
    soc_mem_mac_addr_get(unit, mem, entry, 
                        field, mac);
    if (!flag) {
         /* Set the low-order bytes */
         mac[3] = (uint8) (mac_field >> 16 & 0xff);
         mac[4] = (uint8) (mac_field >> 8 & 0xff);
         mac[5] = (uint8) (mac_field & 0xff);
    } else {
         /* Set the High-order bytes */
         mac[0] = (uint8) (mac_field >> 16 & 0xff);
         mac[1] = (uint8) (mac_field >> 8 & 0xff);
         mac[2] = (uint8) (mac_field & 0xff);
    }
    soc_mem_mac_addr_set(unit, mem, entry,
                        field, mac);
}

/*
 * Function:
 *      bcm_td_trill_mac_set
 * Description:
 *      flag = 0:  Set the lower 24 bits of MAC address field for TRILL specific MAC
 *      flag = 1:  Set the higher 24 bits of MAC address field for TRILL specific MAC
 * Parameters:
 *      unit      - unit number (driver internal).
 *      type     - The required switch control type to set upper MAC for
 *      mac     - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_td_trill_mac_set(int unit, 
                            bcm_switch_control_t type,
                            uint32 mac_field,
                            uint8   flag)
{
    ing_trill_parse_control_entry_t  ing_ent;
    egr_trill_parse_control_entry_t egr_ent;
    egr_trill_parse_control_2_entry_t egr2_ent;
    ing_trill_payload_parse_control_entry_t ing_payload_ent;
    bcm_mac_t   mac;   

    sal_memset(mac, 0, sizeof(bcm_mac_t));    

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, EGR_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &egr_ent));
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, EGR_TRILL_PARSE_CONTROL_2m,
                       MEM_BLOCK_ALL, 0, &egr2_ent));
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_payload_ent));
   
    switch (type) {
         case bcmSwitchTrillISISDestMacOui:         
         case bcmSwitchTrillISISDestMacNonOui:
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));
              _bcm_td_trill_mac_set(unit, mac_field, ING_TRILL_PARSE_CONTROLm, 
                                       &ing_ent, TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) { 
                soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));

              BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &egr_ent));
              sal_memset(mac, 0, sizeof(bcm_mac_t));
              _bcm_td_trill_mac_set(unit, mac_field, EGR_TRILL_PARSE_CONTROLm, 
                                       &egr_ent, TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, EGR_TRILL_PARSE_CONTROLm, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) { 
                soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &egr_ent));

              BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_TRILL_PARSE_CONTROL_2m,
                                       MEM_BLOCK_ALL, 0, &egr2_ent));
              sal_memset(mac, 0, sizeof(bcm_mac_t));
              _bcm_td_trill_mac_set(unit, mac_field, EGR_TRILL_PARSE_CONTROL_2m, 
                                       &egr2_ent, TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, EGR_TRILL_PARSE_CONTROL_2m, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) {
                soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROL_2m, &egr2_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROL_2m,
                                       MEM_BLOCK_ALL, 0, &egr2_ent));
              break;
        
        case bcmSwitchTrillBroadcastDestMacOui:
        case bcmSwitchTrillBroadcastDestMacNonOui: 
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));
              _bcm_td_trill_mac_set(unit, mac_field, ING_TRILL_PARSE_CONTROLm, 
                                       &ing_ent, TRILL_ALL_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) { 
                soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));

              BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &egr_ent));
              sal_memset(mac, 0, sizeof(bcm_mac_t));
              _bcm_td_trill_mac_set(unit, mac_field, EGR_TRILL_PARSE_CONTROLm, 
                                       &egr_ent, TRILL_ALL_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, EGR_TRILL_PARSE_CONTROLm, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) { 
                soc_mem_field32_set(unit, EGR_TRILL_PARSE_CONTROLm, &egr_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &egr_ent));
#ifdef BCM_TOMAHAWK_SUPPORT
              if (SOC_IS_TOMAHAWKX(unit)) {
                  BCM_IF_ERROR_RETURN(bcm_th_ipmc_trill_mac_update(unit,
                              mac_field, flag));
              } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TRIUMPH3_SUPPORT
              if (SOC_IS_TRIUMPH3(unit)) {
                  BCM_IF_ERROR_RETURN(bcm_tr3_ipmc_trill_mac_update(unit,
                              mac_field, flag));
              }
#endif /* BCM_TRIUMPH3_SUPPORT */
              break;

        case bcmSwitchTrillEndStationDestMacOui:        
        case bcmSwitchTrillEndStationDestMacNonOui:
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_payload_ent));
              _bcm_td_trill_mac_set(unit, mac_field, ING_TRILL_PAYLOAD_PARSE_CONTROLm, 
                                       &ing_payload_ent, TRILL_ALL_ESADI_RBRIDGES_MAC_ADDRESSf, mac, flag);
              if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf)) { 
                soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent, 
                                         TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESS_ENABLEf, 0x1); 
              } 
              BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_payload_ent));
              break;

        default:
              return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_trill_mac_get
 * Description:
 *      flag = 0:  Get the lower 24 bits of MAC address field for TRILL specific MAC
 *      flag = 1:  Get the higher 24 bits of MAC address field for TRILL specific MAC
 * Parameters:
 *      unit       - unit number (driver internal).
 *      type      - The required switch control type to set upper MAC for
 *      mac      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC void
_bcm_td_trill_mac_get(int unit, soc_mem_t mem, 
                        void *entry, soc_field_t field, uint32 *mac_field, uint8 flag) 
{
    bcm_mac_t mac;

    soc_mem_mac_addr_get(unit, mem, entry, 
                        field, mac);

    if (flag) {
         /* read the high-order bytes from Memory */
         *mac_field = ((mac[0] << 16) | (mac[1] << 8)  | (mac[2] << 0));
    } else {
         /* read the low-order bytes from Memory */
         *mac_field = ((mac[3] << 16) | (mac[4] << 8)  | (mac[5] << 0));
    }
}


/*
 * Function:
 *      bcm_td_trill_mac_get
 * Description:
 *      flag = 0:  Get the lower 24 bits of MAC address field for TRILL specific MAC
 *      flag = 1:  Get the higher 24 bits of MAC address field for TRILL specific MAC
 * Parameters:
 *      unit      - unit number (driver internal).
 *      type     - The required switch control type to set upper MAC for
 *      mac     - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_td_trill_mac_get(int unit, 
                            bcm_switch_control_t type,
                            uint32 *mac_field,
                            uint8   flag)
{
    ing_trill_parse_control_entry_t  ing_ent;
    egr_trill_parse_control_entry_t egr_ent;
    egr_trill_parse_control_2_entry_t egr2_ent;
    ing_trill_payload_parse_control_entry_t ing_payload_ent;

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));	
    sal_memset(&egr_ent, 0, sizeof(egr_trill_parse_control_entry_t));    
    sal_memset(&egr2_ent, 0, sizeof(egr_trill_parse_control_2_entry_t));
    sal_memset(&ing_payload_ent, 0, sizeof(ing_trill_payload_parse_control_entry_t));

    switch (type) {
         case bcmSwitchTrillISISDestMacOui:         
         case bcmSwitchTrillISISDestMacNonOui:
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));
              _bcm_td_trill_mac_get(unit, ING_TRILL_PARSE_CONTROLm, 
                                       &ing_ent, TRILL_ALL_IS_IS_RBRIDGES_MAC_ADDRESSf, mac_field, flag);

              break;
        
        case bcmSwitchTrillBroadcastDestMacOui:         
        case bcmSwitchTrillBroadcastDestMacNonOui: 
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_ent));
              _bcm_td_trill_mac_get(unit, ING_TRILL_PARSE_CONTROLm, 
                                       &ing_ent, TRILL_ALL_RBRIDGES_MAC_ADDRESSf, mac_field, flag);

              break;

        case bcmSwitchTrillEndStationDestMacOui:        
        case bcmSwitchTrillEndStationDestMacNonOui:
              BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm,
                                       MEM_BLOCK_ALL, 0, &ing_payload_ent));
              _bcm_td_trill_mac_get(unit, ING_TRILL_PAYLOAD_PARSE_CONTROLm, 
                                       &ing_payload_ent, TRILL_ALL_ESADI_RBRIDGES_MAC_ADDRESSf, mac_field, flag);
              break;

        default:
              return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td_trill_MinTtl_set
 * Description:
 *       Set minimum TTL value for Transit and Egress Rbridge
 * Parameters:
 *      unit      - unit number (driver internal).
 *      min_ttl     - Minimum TTL value
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_td_trill_MinTtl_set(int unit, int min_ttl)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    if (!soc_feature(unit, soc_feature_trill_ttl)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    if (SOC_MEM_IS_VALID(unit, ING_TRILL_PARSE_CONTROLm)) {              
      if ((min_ttl > 0) && (min_ttl < 64)) {
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));
    
         if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             MIN_INCOMING_HOPCOUNTf)) {
              soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                                         MIN_INCOMING_HOPCOUNTf, min_ttl);
         }

         BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                            MEM_BLOCK_ALL, 0, &ing_ent));
      }
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_MinTtl_get
 * Description:
 *       Get minimum TTL value for Transit and Egress Rbridge
 * Parameters:
 *      unit      - unit number (driver internal).
 *      min_ttl     - Minimum TTL value
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_td_trill_MinTtl_get(int unit, int *min_ttl)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    if (!soc_feature(unit, soc_feature_trill_ttl)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    if (SOC_MEM_IS_VALID(unit, ING_TRILL_PARSE_CONTROLm)) {
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));
    
         if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             MIN_INCOMING_HOPCOUNTf)) {
              *min_ttl = soc_mem_field32_get(unit, ING_TRILL_PARSE_CONTROLm, 
                                            &ing_ent,
                                            MIN_INCOMING_HOPCOUNTf);
         }
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_TtlCheckEnable_set
 * Description:
 *       Set enable for Egress RBridge TTL Check 
 * Parameters:
 *      unit      - unit number (driver internal).
 *      enable     - TTL Check Enable
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_td_trill_TtlCheckEnable_set(int unit, int enable)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    if (!soc_feature(unit, soc_feature_trill_ttl)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    if (SOC_MEM_IS_VALID(unit, ING_TRILL_PARSE_CONTROLm)) {              
      if ((enable == TRUE) || (enable == FALSE)) {  
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));
    
         if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             EGRESS_RBRIDGE_CHECK_HOPCOUNTf)) {
              soc_mem_field32_set(unit, ING_TRILL_PARSE_CONTROLm, &ing_ent,
                                         EGRESS_RBRIDGE_CHECK_HOPCOUNTf, enable);
         }

         BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_TRILL_PARSE_CONTROLm,
                                            MEM_BLOCK_ALL, 0, &ing_ent));
      }
    }
    return rv;

}

/*
 * Function:
 *      bcm_td_trill_TtlCheckEnable_get
 * Description:
 *       Get enable for Egress RBridge TTL Check 
 * Parameters:
 *      unit      - unit number (driver internal).
 *      enable     - TTL Check Enable
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_td_trill_TtlCheckEnable_get(int unit, int *enable)
{
    int rv = BCM_E_NONE;
    ing_trill_parse_control_entry_t  ing_ent;

    if (!soc_feature(unit, soc_feature_trill_ttl)) {
        return BCM_E_UNAVAIL;
    }

    sal_memset(&ing_ent, 0, sizeof(ing_trill_parse_control_entry_t));

    if (SOC_MEM_IS_VALID(unit, ING_TRILL_PARSE_CONTROLm)) {
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, ING_TRILL_PARSE_CONTROLm,
                       MEM_BLOCK_ALL, 0, &ing_ent));
    
         if (soc_mem_field_valid(unit, ING_TRILL_PARSE_CONTROLm, 
                             EGRESS_RBRIDGE_CHECK_HOPCOUNTf)) {
              *enable = soc_mem_field32_get(unit, ING_TRILL_PARSE_CONTROLm, 
                                            &ing_ent,
                                            EGRESS_RBRIDGE_CHECK_HOPCOUNTf);
         }
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_allocate_bk
 * Purpose:
 *      Initialize the Trill software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_td_trill_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == TRILL_INFO(unit)) {
        BCM_TD_TRILL_ALLOC(TRILL_INFO(unit), sizeof(_bcm_td_trill_bookkeeping_t),
                          "trill_bk_module_data");
        if (NULL == TRILL_INFO(unit)) {
            return (BCM_E_MEMORY);
        } else {
            TRILL_INFO(unit)->initialized = FALSE;
        }
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
/* 
 * Function:
 *      bcm_td_trill_sync
 * Purpose:
 *      Store the multicast count Trill book keeping structure 
 *      onto persistent memory.
 * Parameters:
 *      unit    : (IN) Device Unit Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_trill_sync(int unit)
{
    int rv=BCM_E_NONE, alloc_sz = 0, num_ipmc=0;
    uint8 *trill_state;
    soc_scache_handle_t scache_handle;
    _bcm_td_trill_bookkeeping_t *trill_info;

    if (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
        return BCM_E_NONE;        
    }
    trill_info = TRILL_INFO(unit);
    num_ipmc = soc_mem_index_count(unit, L3_IPMCm);

    alloc_sz = sizeof(_bcm_td_trill_multicast_count_t) * num_ipmc;   

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRILL, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 alloc_sz, &trill_state, 
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }
    
    sal_memcpy(trill_state, trill_info->multicast_count, alloc_sz);

    return rv;
}

/*
 * Function:
 *      _bcm_td_trill_reinit
 * Purpose:
 *      Perform recovery of Trill s/w state during warmboot.
 * Parameters:
 *      unit - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_td_trill_reinit(int unit)
{
    uint8 *trill_state;
    int alloc_sz=0, stable_size = 0, num_ipmc=0;
    int idx=0, idx_min=0, idx_max=0;
    soc_scache_handle_t scache_handle;
    _bcm_td_trill_bookkeeping_t *trill_info;
    egr_dvp_attribute_entry_t egr_dvp_attribute;
    egr_trill_tree_profile_entry_t  egr_trill_profile;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size != 0) && (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) { 

        trill_info = TRILL_INFO(unit);
    
        /* Recover rBridge book keeping info */    
        idx_min = soc_mem_index_min(unit, EGR_DVP_ATTRIBUTEm);
        idx_max = soc_mem_index_max(unit, EGR_DVP_ATTRIBUTEm);

        for (idx = idx_min; idx <= idx_max; idx++) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                                    MEM_BLOCK_ANY, idx, &egr_dvp_attribute));
            if (soc_EGR_DVP_ATTRIBUTEm_field32_get(unit,
                         &egr_dvp_attribute, VP_TYPEf) != 0x1) {
                continue;
            }
            trill_info->rBridge[idx] = soc_EGR_DVP_ATTRIBUTEm_field32_get(unit,
                                  &egr_dvp_attribute, EGRESS_RBRIDGE_NICKNAMEf);
        }

        /* Recover rootBridge book keeping info */    
        idx_min = soc_mem_index_min(unit, EGR_TRILL_TREE_PROFILEm);
        idx_max = soc_mem_index_max(unit, EGR_TRILL_TREE_PROFILEm);

        for (idx = idx_min; idx <= idx_max; idx++) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, EGR_TRILL_TREE_PROFILEm,
                                    MEM_BLOCK_ANY, idx, &egr_trill_profile));
            /* coverity[overrun-local : FALSE] */
            trill_info->rootBridge[idx] =
                           soc_EGR_TRILL_TREE_PROFILEm_field32_get(unit,
                                    &egr_trill_profile, ROOT_RBRIDGE_NICKNAMEf);
        }

        /* Recover multicast ref count book keeping info */    
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRILL, 0);
        num_ipmc = soc_mem_index_count(unit, L3_IPMCm);

        alloc_sz = sizeof(_bcm_td_trill_multicast_count_t) * num_ipmc;   
        SOC_IF_ERROR_RETURN
            (_bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                     alloc_sz, &trill_state, 
                                     BCM_WB_DEFAULT_VERSION, NULL));

        sal_memcpy(trill_info->multicast_count, trill_state, alloc_sz);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_td_trill_init
 * Purpose:
 *      Initialize the Trill software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


int
bcm_td_trill_init(int unit)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int idx=0, num_vp=0, num_ipmc=0;
#ifdef BCM_WARM_BOOT_SUPPORT
    uint8 *trill_state = 0;
    soc_scache_handle_t scache_handle;
    int  rv = BCM_E_NONE, alloc_sz = 0;
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_TRILL,
                  (BSL_META_U(unit,
                              "L3 module must be initialized prior to TRILL_init\n")));
        return BCM_E_CONFIG;
    }

    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(bcm_td_trill_allocate_bk(unit));
    trill_info = TRILL_INFO(unit);

    /*
     * allocate resources
     */
    if (trill_info->initialized) {
        BCM_IF_ERROR_RETURN(bcm_td_trill_cleanup(unit));
        BCM_IF_ERROR_RETURN(bcm_td_trill_allocate_bk(unit));
        trill_info = TRILL_INFO(unit);
    }

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_ipmc = soc_mem_index_count(unit, L3_IPMCm);

    trill_info->rBridge =
        sal_alloc(sizeof(bcm_trill_name_t) * num_vp, "trill rBridge store");
    if (trill_info->rBridge == NULL) {
        _bcm_td_trill_free_resource(unit);
        return BCM_E_MEMORY;
    }

    trill_info->multicast_count =
        sal_alloc(sizeof(_bcm_td_trill_multicast_count_t) * num_ipmc, "trill multicast store");
    if (trill_info->multicast_count == NULL) {
        _bcm_td_trill_free_resource(unit);
        return BCM_E_MEMORY;
    }


    /* Create Trill protection mutex. */
    trill_info->trill_mutex = sal_mutex_create("trill_mutex");
    if (!trill_info->trill_mutex) {
         _bcm_td_trill_free_resource(unit);
         return BCM_E_MEMORY;
    }

    for (idx=0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
         trill_info->rootBridge[idx] = 0;
    }

    sal_memset(trill_info->rBridge, 0, sizeof(bcm_trill_name_t) * num_vp);
    sal_memset(trill_info->multicast_count, 0, sizeof(_bcm_td_trill_multicast_count_t) * num_ipmc);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        if (BCM_FAILURE(_bcm_td_trill_reinit(unit))) {
            _bcm_td_trill_free_resource(unit);
        }
    } else { /* Coldboot */
        alloc_sz = sizeof(_bcm_td_trill_multicast_count_t) * num_ipmc;   
        SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_TRILL, 0);
        if (!SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit)) {
            rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                         alloc_sz, &trill_state, 
                                         BCM_WB_DEFAULT_VERSION, NULL);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Mark the state as initialized */
    trill_info->initialized = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_trill_hw_clear
 * Purpose:
 *     Perform hw tables clean up for Trill module. 
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
 
STATIC int
_bcm_td_trill_hw_clear(int unit)
{
    int rv = BCM_E_NONE, rv_error = BCM_E_NONE;
    bcm_port_t    port;
    bcm_trill_stat_t stat;

    if (!SOC_HW_RESET(unit)) {
        PBMP_E_ITER(unit, port) {
            if (SOC_IS_STACK_PORT(unit, port)) {
                continue;
            }
            for (stat=bcmTrillInPkts; stat < bcmTrillErrorPkts; stat++) {
                rv = bcm_td_trill_stat_clear(unit, port, stat);
                if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
                    rv_error = rv;
                }
            }
        }

        PBMP_HG_ITER(unit, port) {
            if (SOC_IS_STACK_PORT(unit, port)) {
                continue;
            }
            for (stat=bcmTrillInPkts; stat < bcmTrillErrorPkts; stat++) {
                rv = bcm_td_trill_stat_clear(unit, port, stat);
                if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
                    rv_error = rv;
                }
            }
        }
    }

    rv = bcm_td_trill_port_delete_all(unit);
    if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
        rv_error = rv;
    }
    if (!SOC_HW_RESET(unit)) {
        rv = bcm_td_trill_multicast_source_delete_all(unit);
        if (BCM_FAILURE(rv) && (BCM_E_NONE == rv_error)) {
            rv_error = rv;
        }
    }

    return rv_error;
}

/*
 * Function:
 *      bcm_td_trill_cleanup
 * Purpose:
 *      DeInit  the Trill software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_td_trill_cleanup(int unit)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv = BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    if (FALSE == trill_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcm_td_trill_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) { 
        rv = _bcm_td_trill_hw_clear(unit);
    }

    /* Mark the state as uninitialized */
    trill_info->initialized = FALSE;

    sal_mutex_give(trill_info->trill_mutex);

    /* Destroy protection mutex. */
    sal_mutex_destroy(trill_info->trill_mutex );

    /* Free software resources */
    (void) _bcm_td_trill_free_resource(unit);

    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_tree_profile_set
 * Purpose:
 *		Set TRILL Tree Profile
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_name
 *   IN :  hopcount
 *   OUT: trill_tree_profile_idx
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_td_trill_tree_profile_set (int unit, bcm_trill_name_t trill_name, int hopcount, uint8 *trill_tree_profile_idx)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    egr_trill_tree_profile_entry_t  egr_trill_profile;
    int rv=BCM_E_UNAVAIL;
    uint8 idx=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    if ((hopcount < 0) || (hopcount > 63)) {
         return BCM_E_PARAM;
    }

    trill_info = TRILL_INFO(unit);

    /* Check if root already exists */
    for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
        if (trill_info->rootBridge[idx] == trill_name) {
              *trill_tree_profile_idx = idx;
              break;
         }
    }

    /* Root does not exist, Check for free entry */
    if (idx == BCM_MAX_NUM_TRILL_TREES) {
        for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
            if (trill_info->rootBridge[idx] == 0) {
                  *trill_tree_profile_idx = idx;
                  break;
             }
        }
    }

    if (idx == BCM_MAX_NUM_TRILL_TREES) {
         return  BCM_E_RESOURCE;
    }
    trill_info->rootBridge[*trill_tree_profile_idx] = trill_name;

    sal_memset(&egr_trill_profile, 0, sizeof(egr_trill_tree_profile_entry_t));	
    soc_mem_field32_set(unit, EGR_TRILL_TREE_PROFILEm, &egr_trill_profile,
                                            ROOT_RBRIDGE_NICKNAMEf, trill_name);
    soc_mem_field32_set(unit, EGR_TRILL_TREE_PROFILEm, &egr_trill_profile,
                                            HOPCOUNTf, hopcount);
    rv = soc_mem_write(unit, EGR_TRILL_TREE_PROFILEm,
                                            MEM_BLOCK_ALL, *trill_tree_profile_idx, &egr_trill_profile);

    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_tree_profile_reset
 * Purpose:
 *		Reset TRILL Tree Profile
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_Rbridge_nickname
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_td_trill_tree_profile_reset (int unit, bcm_trill_name_t trill_name)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    egr_trill_tree_profile_entry_t  egr_trill_profile;
    uint8 idx=0;
    int rv=BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
         if (trill_info->rootBridge[idx] == trill_name) {
              trill_info->rootBridge[idx] = 0;
              sal_memset(&egr_trill_profile, 0, sizeof(egr_trill_tree_profile_entry_t));  
              rv = soc_mem_write(unit, EGR_TRILL_TREE_PROFILEm,
                                                 MEM_BLOCK_ALL, 
                                                 idx, &egr_trill_profile);
              return rv;
         }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *		bcm_td_trill_tree_profile_get
 * Purpose:
 *		Get TRILL Tree Profile index from trill_name
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_tree_profile_idx
 * Returns:
 *		BCM_E_XXX
 */

void
bcm_td_trill_tree_profile_get (int unit, bcm_trill_name_t trill_name, uint8 *trill_tree_profile_idx)
{
    _bcm_td_trill_bookkeeping_t *trill_info = TRILL_INFO(unit);
    uint8 idx=0;

    for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
         if (trill_info->rootBridge[idx] == trill_name) {
              *trill_tree_profile_idx = idx;
              break;
         }
    }
    if (idx == BCM_MAX_NUM_TRILL_TREES) {
         *trill_tree_profile_idx = BCM_MAX_NUM_TRILL_TREES;
    }
}


/*
 * Function:
 *         bcm_td_trill_root_name_get
 * Purpose:
 *		Get TRILL Root name from Tree Profile
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_tree_profile_idx
 *   OUT:  trill_name
 * Returns:
 *		BCM_E_XXX
 */

void
bcm_td_trill_root_name_get (int unit, uint8 trill_tree_profile_idx, bcm_trill_name_t *trill_name)
{
    _bcm_td_trill_bookkeeping_t *trill_info = TRILL_INFO(unit);

     *trill_name = trill_info->rootBridge[trill_tree_profile_idx];
}

/*
 * Function:
 *      _bcm_td_trill_port_resolve
 * Purpose:
 *      Get the modid, port, trunk values for TRILL port
 * Parameters:
 *   IN  :  Unit
 *   IN  :  trill_port_id
 *   IN  :  encap_id
 *  OUT:  module_id
 *  OUT:  port
 *  OUT:  trunk_id
 *  OUT:  vp
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td_trill_port_resolve(int unit, bcm_gport_t trill_port_id, 
                          bcm_if_t encap_id, bcm_module_t *modid, 
                          bcm_port_t *port, bcm_trunk_t *trunk_id, int *id)

{
    int rv = BCM_E_NONE;
    int ecmp=0, nh_index=0, nh_ecmp_index=0, vp=0;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    ing_dvp_table_entry_t dvp;
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry. */
    int  idx=0, max_ent_count=0, base_idx=0;
    BCM_IF_ERROR_RETURN(_bcm_trill_check_init (unit));

    if (!BCM_GPORT_IS_TRILL_PORT(trill_port_id)) {
        return (BCM_E_BADID);
    }

    vp = BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
        return BCM_E_NOT_FOUND;
    }
    BCM_IF_ERROR_RETURN(
         READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (!ecmp) {
         nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp,
                                       NEXT_HOP_INDEXf);

         if (nh_index == 0) { /* Local My RBridge */
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, modid));
         } else if (nh_index > 0) {
            BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                      nh_index, &ing_nh));

            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, ENTRY_TYPEf) != 0x2) {
                 /* Entry type is not  TRILL DVP  */
                 return BCM_E_NOT_FOUND;
            }
            if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                 *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
            } else {
                 /* Only add this to replication set if destination is local */
                *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
            }
         }
    } else {
         /* Select the desired nhop from ECMP group - pointed by encap_id */
         nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);

         BCM_IF_ERROR_RETURN(
             _bcm_xgs3_l3_ecmp_grp_info_get(unit, nh_ecmp_index,
                                            &max_ent_count, &base_idx));

        if(encap_id == -1) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY, 
                        base_idx, hw_buf));
              nh_index = soc_mem_field32_get(unit, L3_ECMPm, 
                   hw_buf, NEXT_HOP_INDEXf);

              /* Multipath group should have at least one member.
               * If not then nh_index points to black hole for which
               * index to ING_L3_NEXT_HOPm is not valid.
               */
              if ((max_ent_count > 0) && nh_index) {
                   BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                     MEM_BLOCK_ANY, nh_index, &egr_nh));
                   BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm,
                          MEM_BLOCK_ANY, nh_index, &ing_nh));

                   if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh,
                               ENTRY_TYPEf) != 0x2) {
                          /* Entry type is not  TRILL DVP  */
                          return BCM_E_NOT_FOUND;
                   }
                   if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                       *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
                   } else {
                       /* Only add this to replication set if destination is local */
                       *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                       *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
                  }
              }
        } else {
            for (idx = 0; idx < max_ent_count; idx++) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY, 
                        (base_idx+idx), hw_buf));
              nh_index = soc_mem_field32_get(unit, L3_ECMPm, 
                   hw_buf, NEXT_HOP_INDEXf);
              BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                   MEM_BLOCK_ANY, nh_index, &egr_nh));
              if (encap_id == soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                                               &egr_nh, INTF_NUMf)) {
                 BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                        MEM_BLOCK_ANY, nh_index, &ing_nh));

                 if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, 
                             ENTRY_TYPEf) != 0x2) {
                        /* Entry type is not  TRILL DVP  */
                        return BCM_E_NOT_FOUND;
                 }
                 if (soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, Tf)) {
                     *trunk_id = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, TGIDf);
                 } else {
                     /* Only add this to replication set if destination is local */
                     *modid = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, MODULE_IDf);
                     *port = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, PORT_NUMf);
                }
                break;
              }
            }
        }
    }

    *id = vp;
    return rv;
}

/*
 * Function:
 *      _bcm_td_trill_port_cnt_update
 * Purpose:
 *      Update port's VP count.
 * Parameters:
 *      unit  - (IN) SOC unit number. 
 *      gport - (IN) GPORT ID.
 *      vp    - (IN) Virtual port number.
 *      incr  - (IN) If TRUE, increment VP count, else decrease VP count.
 * Returns:
 *      BCM_X_XXX
 */

STATIC int
_bcm_td_trill_port_cnt_update(int unit, bcm_gport_t gport,
        int vp, int incr_decr_flag)
{
    int mod_out=-1, port_out=-1, tgid_out=-1, vp_out=-1;
    bcm_port_t local_member_array[SOC_MAX_NUM_PORTS];
    int local_member_count=0;
    int idx=-1;
    int mod_local=-1;
    _bcm_port_info_t *port_info;
    uint32 port_flags;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, gport, &mod_out, &port_out, &tgid_out,
                                &vp_out));

    if (vp_out == -1) {
       return BCM_E_PARAM;
    }

    /* Update the physical port's SW state. If associated with a trunk,
     * update each local physical port's SW state.
     */

    if (BCM_TRUNK_INVALID != tgid_out) {

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid_out, 
                    SOC_MAX_NUM_PORTS, local_member_array, &local_member_count));

        for (idx = 0; idx < local_member_count; idx++) {
            _bcm_port_info_access(unit, local_member_array[idx], &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, local_member_array[idx], &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, local_member_array[idx], port_flags));
        }
    } else {
        if (-1 == port_out) {
            return BCM_E_NONE;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod_out, &mod_local));
        if (mod_local) {
            if (soc_feature(unit, soc_feature_sysport_remap)) { 
                BCM_XLATE_SYSPORT_S2P(unit, &port_out); 
            }
            _bcm_port_info_access(unit, port_out, &port_info);
            if (incr_decr_flag) {
                port_info->vp_count++;
            } else {
                port_info->vp_count--;
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_get(
                    unit, port_out, &port_flags));
            BCM_IF_ERROR_RETURN(
                bcm_esw_port_vlan_member_set(
                    unit, port_out, port_flags));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *		_bcm_td_trill_transit_entry_key_set
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
_bcm_td_trill_transit_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                                         int nh_ecmp_index, mpls_entry_entry_t   *tr_ent, int clean_flag)
{

    if (clean_flag) {
         sal_memset(tr_ent, 0, sizeof(mpls_entry_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x0);
    if (trill_port->flags & _BCM_TRILL_PORT_MULTIPATH) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__ECMPf, 0x1);
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__ECMP_PTRf, nh_ecmp_index);
    } else {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__ECMPf, 0x0);
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__NEXT_HOP_INDEXf, nh_ecmp_index);
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__UCAST_DST_DISCARDf)) { 
        if ((trill_port->flags & BCM_TRILL_PORT_DROP) ||
            !(trill_port->flags & BCM_TRILL_PORT_NETWORK)) {
              soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x1);
        } else if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
              soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x0);
        }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__DST_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__DST_COPY_TO_CPUf, 0x1);
    }
    /* Multicast-traffic discard setting */
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) {
        if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
            soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x0);
        } else {
            soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x1);
        }
    }

    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         VALIDf, 0x1);
}


/*
 * Function:
 *      _bcm_td_trill_transit_entry_set
 * Purpose:
 *     Set TRILL Transit forwarding entry
 * Parameters:
 *      IN :  Unit
 *      IN :  trill_port
 *      IN : next_hop / ecmp group index
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_td_trill_transit_entry_set(int unit, bcm_trill_port_t *trill_port,
                                         int nh_ecmp_index)
{
    int rv = BCM_E_NONE;
    int index=0;
    mpls_entry_entry_t  key_ent;
    mpls_entry_entry_t  return_ent;

    _bcm_td_trill_transit_entry_key_set(unit, trill_port, 
                        nh_ecmp_index, &key_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &key_ent, &return_ent, 0);
    if (rv == SOC_E_NONE) {
         (void) _bcm_td_trill_transit_entry_key_set(unit, trill_port, 
                                  nh_ecmp_index, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRYm,
                                  MEM_BLOCK_ALL, index,
                                  &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &key_ent);
    }
    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_transit_entry_reset
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

STATIC int
_bcm_td_trill_transit_entry_reset(int unit, int vp, int nh_ecmp_idx, int ecmp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class;               /* Interface class ID. */
    mpls_entry_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));
 
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                                         MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                         EGRESS_RBRIDGE_NICKNAMEf);

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                                         MEM_BLOCK_ALL, vp, &svp));

    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);


    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__DECAP_TRILL_TUNNELf, 0x0);
    if (ecmp) {
      soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__ECMPf, 0x1);
      soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__ECMP_PTRf, nh_ecmp_idx);
    } else {
      soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      TRILL__NEXT_HOP_INDEXf, nh_ecmp_idx);
    }
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                      VALIDf, 0x1);
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                     &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &tr_ent);
    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_learn_entry_key_set
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
 _bcm_td_trill_learn_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                                          int vp, mpls_entry_entry_t   *tr_ent, int clean_flag)
 {
    uint8 trill_tree_id=0;

    if (clean_flag) {
         sal_memset(tr_ent, 0, sizeof(mpls_entry_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__VIRTUAL_PORTf, vp);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__PHB_FROM_OUTER_L2_HEADERf, 0x1);
    if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
         (void) bcm_td_trill_tree_profile_get (unit, trill_port->name, &trill_tree_id);
          if (trill_tree_id < BCM_MAX_NUM_TRILL_TREES) {
               soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__TREE_IDf, trill_tree_id);
          }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__SRC_DISCARDf) && 
        (trill_port->flags & BCM_TRILL_PORT_DROP)) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__SRC_DISCARDf, 0x1);
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__SRC_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__SRC_COPY_TO_CPUf, 0x1);
    }
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         VALIDf, 0x1);
 }

/*
 * Function:
 *		_bcm_td_trill_learn_entry_set
 * Purpose:
 *		Set TRILL  Learn Entry 
 * Parameters:
 *		IN :  Unit
 *  IN :  trill_port
 *  IN : vp
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_learn_entry_set(int unit, bcm_trill_port_t *trill_port, int vp)
{
    mpls_entry_entry_t  key_ent;
    mpls_entry_entry_t  return_ent;
    int rv = BCM_E_UNAVAIL;
    int index;

    _bcm_td_trill_learn_entry_key_set(unit, trill_port, vp, &key_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &key_ent, &return_ent, 0);

    if (rv == SOC_E_NONE) {
         (void) _bcm_td_trill_learn_entry_key_set(unit, trill_port, vp, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRYm,
                                           MEM_BLOCK_ALL, index,
                                           &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &key_ent);
    }

    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_learn_entry_reset
 * Purpose:
 *		Reset TRILL RPF entry
 * Parameters:
 *		IN :  Unit
 *  IN :  vp
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_learn_entry_reset(int unit, int vp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class;               /* Interface class ID. */
    mpls_entry_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                             MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                             EGRESS_RBRIDGE_NICKNAMEf);

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                             MEM_BLOCK_ALL, vp, &svp));

    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);

    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             TRILL__VIRTUAL_PORTf, vp);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             TRILL__PHB_FROM_OUTER_L2_HEADERf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                             VALIDf, 0x1);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
         return rv;
    }
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &tr_ent);
    
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
       return rv;
    } else {
       return BCM_E_NONE;
    }
}


/*
 * Function:
 *		_bcm_td_trill_decap_entry_key_set
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
 _bcm_td_trill_decap_entry_key_set(int unit, bcm_trill_port_t *trill_port,
                                          mpls_entry_entry_t   *tr_ent, int clean_flag)
 {
    if (clean_flag) {
       sal_memset(tr_ent, 0, sizeof(mpls_entry_entry_t));
    }

    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__CLASS_IDf, trill_port->if_class);
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__UCAST_DST_DISCARDf)) { 
       if ((trill_port->flags & BCM_TRILL_PORT_DROP) ||
            !(trill_port->flags & BCM_TRILL_PORT_NETWORK)) {
              soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x1);
       } else if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
              soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__UCAST_DST_DISCARDf, 0x0);
        }
    }
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__DST_COPY_TO_CPUf) && 
        (trill_port->flags & BCM_TRILL_PORT_COPYTOCPU)) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         TRILL__DST_COPY_TO_CPUf, 0x1);
    }
    /* Multicast-traffic discard setting */
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) {
        if (trill_port->flags & BCM_TRILL_PORT_MULTICAST) {
            soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x0);
        } else {
            soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                     TRILL__MCAST_DST_DISCARDf, 0x1);
        }
    }
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                         VALIDf, 0x1);
 }

/*
 * Function:
 *           _bcm_td_trill_decap_entry_set
 * Purpose:
 *           Set TRILL  Decap  Entry 
 * Parameters:
 *           IN :  Unit
 *  IN :  trill_port
 * Returns:
 *           BCM_E_XXX
 */

STATIC int
_bcm_td_trill_decap_entry_set(int unit, bcm_trill_port_t *trill_port)
{
    mpls_entry_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;
    mpls_entry_entry_t  return_ent;

    _bcm_td_trill_decap_entry_key_set(unit, trill_port, &tr_ent, 1);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                        &tr_ent, &return_ent, 0);
    if (rv == SOC_E_NONE) {
         (void) _bcm_td_trill_decap_entry_key_set(unit, trill_port, &return_ent, 0);
         rv = soc_mem_write(unit, MPLS_ENTRYm,
                                  MEM_BLOCK_ALL, index,
                                  &return_ent);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
        rv = soc_mem_insert(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &tr_ent);
    }
    return rv;
}

/*
 * Function:
 *           _bcm_td_trill_decap_entry_reset
 * Purpose:
 *           Reset TRILL Decap entry
 * Parameters:
 *            IN :  Unit
 *  IN :  vp
 * Returns:
 *            BCM_E_XXX
 */

STATIC int
_bcm_td_trill_decap_entry_reset(int unit,  int vp)
{
    int rv = BCM_E_UNAVAIL;
    int index=0;
    bcm_trill_name_t rb_name;      /* Destination RBridge Nickname. */
    int if_class=0;               /* Interface class ID. */
    mpls_entry_entry_t  tr_ent;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;
 
    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));

    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                         MEM_BLOCK_ALL, vp, &egr_dvp_attribute));

    rb_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                         EGRESS_RBRIDGE_NICKNAMEf);
 
    BCM_IF_ERROR_RETURN(
         soc_mem_read(unit, SOURCE_VPm,
                          MEM_BLOCK_ALL, vp, &svp));
 
    if_class = soc_mem_field32_get(unit, SOURCE_VPm, &svp, CLASS_IDf);
 
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                          KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                          TRILL__RBRIDGE_NICKNAMEf, rb_name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                          TRILL__CLASS_IDf, if_class);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                                         TRILL__DECAP_TRILL_TUNNELf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                          VALIDf, 0x1);
 
    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                          &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }

    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, &tr_ent);
    return rv;

}


/*
 * Function:
 *            _bcm_td_trill_ingress_next_hop_set
 * Purpose:
 *           Set ING_L3_NEXT_HOP Entry 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 *           IN : drop
 * Returns:
 *           BCM_E_XXX
 */

STATIC int
_bcm_td_trill_next_hop_set(int unit, int nh_index, uint32 flags)
{
    int rv=BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t   port=0;
    bcm_module_t modid=0, local_modid=0;
    bcm_trunk_t tgid=0;
    int  num_local_ports=0, idx=0;
    int  old_nh_index=-1;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    uint32 replace=0;
    int gport;
    replace = flags & BCM_L3_REPLACE;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
         tgid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                            &ing_nh, TGIDf);
         rv = _bcm_trunk_id_validate(unit, tgid);
         if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
         }
         BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_ports, &num_local_ports));
    } else {
        modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                    &ing_nh, MODULE_IDf);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));
        if (modid != local_modid) {
            /* Ignore EGR_PORT_TO_NHI_MAPPINGs */
            return BCM_E_NONE;
        }
         port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                            &ing_nh, PORT_NUMf);
        local_ports[num_local_ports++] = port;
    }
    if (!(flags & BCM_L3_IPMC)) { /* Only for Unicast Nexthops */
        for (idx = 0; idx < num_local_ports; idx++) {
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &modid));
            BCM_GPORT_MODPORT_SET(gport, modid, local_ports[idx]);
            BCM_IF_ERROR_RETURN(
                _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
            if (old_nh_index && !replace) {
               if (old_nh_index != nh_index) {
                    /* Limitation: For TD/TR3, 
                    Trill egress port can be configured with one NHI*/
                    return BCM_E_RESOURCE;    
                }
            } else {
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_gport_to_nhi_set(unit, gport, nh_index));
            }
        }
        BCM_IF_ERROR_RETURN(
             _bcm_td_trill_adjacency_set(unit, nh_index));
    }
    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_ingress_next_hop_reset
 * Purpose:
 *		Reset ING_L3_NEXT_HOP Entry 
 * Parameters:
 *		IN :  Unit
 *           IN :  nh_index
 *           IN : drop
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_next_hop_reset(int unit, int nh_index)
{
    int rv=BCM_E_NONE;
    ing_l3_next_hop_entry_t ing_nh;
    bcm_port_t   port=0;
    bcm_module_t modid=0, local_modid=0;
    bcm_trunk_t tgid=0;
    int  num_local_ports=0, idx=0;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    int  old_nh_index=-1;
    int  gport;
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));
    if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
         tgid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                            &ing_nh, TGIDf);
         rv = _bcm_trunk_id_validate(unit, tgid);
         if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
         }
         BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, tgid,
                SOC_MAX_NUM_PORTS, local_ports, &num_local_ports));
    } else {
        modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                    &ing_nh, MODULE_IDf);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));

        if (modid != local_modid) {
            /* Ignore reset for non local modid */
            return BCM_E_NONE;
        }
        port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                            &ing_nh, PORT_NUMf);
        local_ports[num_local_ports++] = port;
    }

    for (idx = 0; idx < num_local_ports; idx++) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &modid));
        BCM_GPORT_MODPORT_SET(gport, modid, local_ports[idx]);
        BCM_IF_ERROR_RETURN(
            _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
        if (old_nh_index == nh_index) {
            BCM_IF_ERROR_RETURN(
                _bcm_trx_gport_to_nhi_set(unit, gport, 0));
        }
    }
    BCM_IF_ERROR_RETURN(
          _bcm_td_trill_adjacency_reset(unit, nh_index));
    return rv;
}

/*
 * Function:
 *           bcm_td_trill_egress_set
 * Purpose:
 *           Set TRILL NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 *           IN :  flags
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td_trill_egress_set(int unit, int nh_index, uint32 flags)
{
    ing_l3_next_hop_entry_t ing_nh;
    int rv=BCM_E_NONE;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                  ENTRY_TYPEf, 0x2); /* L2 DVP */
    if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MTU_SIZEf, 0x3fff);
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    else if (SOC_IS_TRIUMPH3(unit)) {
        /* Set default ING_L3_NEXT_HOP_ATTRIBUTE_1 MTU_SIZE 
         * Note: Assumption that first entry has MTU size of 0x3fff
         */
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, DVP_ATTRIBUTE_1_INDEXf, 0);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_L3_NEXT_HOPm,
                                  MEM_BLOCK_ALL, nh_index, &ing_nh));

    rv = _bcm_td_trill_next_hop_set(unit, nh_index, flags);
    return rv;
}

/*
 * Function:
 *           bcm_td_trill_egress_reset
 * Purpose:
 *           Reset TRILL NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 *           IN :  flags
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td_trill_egress_reset(int unit, int nh_index)
{
    ing_l3_next_hop_entry_t ing_nh;
    int rv=BCM_E_NONE;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));

    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm, &ing_nh,
                                  ENTRY_TYPEf, 0x0); /* L2 DVP */
    if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm, MTU_SIZEf)) {
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                &ing_nh, MTU_SIZEf, 0x0);
    }
#if defined(BCM_TRIUMPH3_SUPPORT)
    else if (SOC_IS_TRIUMPH3(unit)) {
        /* Set default ING_L3_NEXT_HOP_ATTRIBUTE_1 MTU_SIZE 
         * Note: Assumption that first entry has MTU size of 0x3fff
         */
        soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                            &ing_nh, DVP_ATTRIBUTE_1_INDEXf, 0);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_L3_NEXT_HOPm,
                                  MEM_BLOCK_ALL, nh_index, &ing_nh));

    rv = _bcm_td_trill_next_hop_reset(unit, nh_index);
    return rv;
}

/*
 * Function:
 *           bcm_td_trill_egress_get
 * Purpose:
 *           Get TRILL Egress NextHop 
 * Parameters:
 *           IN :  Unit
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_td_trill_egress_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    int int_l3_flag;
    int_l3_flag = BCM_XGS3_L3_ENT_FLAG(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                                       nh_index);
    if (int_l3_flag == _BCM_L3_TRILL_ONLY) {
        egr->flags |= BCM_L3_TRILL_ONLY;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td_trill_egress_name_set
 * Purpose:
 *     Set My RBridge Nickname
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_name
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td_trill_egress_name_set(int unit, bcm_trill_name_t  trill_name)
{
    egr_trill_rbridge_nicknames_entry_t  egr_name_entry;
    int idx=0;
    bcm_trill_name_t  temp_name;

      for(idx=0; idx < 4; idx++) {
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                       MEM_BLOCK_ALL, idx, &egr_name_entry));

         temp_name = 
              soc_mem_field32_get(unit, EGR_TRILL_RBRIDGE_NICKNAMESm, 
                                         &egr_name_entry,
                                         RBRIDGE_NICKNAMEf);
         /* If name exists, return */
         if (temp_name == trill_name) {
             return BCM_E_NONE;
         }

         if (!temp_name) {
              soc_mem_field32_set(unit, EGR_TRILL_RBRIDGE_NICKNAMESm, 
                                         &egr_name_entry,
                                         RBRIDGE_NICKNAMEf, trill_name);

              BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                                            MEM_BLOCK_ALL, idx, &egr_name_entry));
              return BCM_E_NONE;
         }
      }
      return BCM_E_FULL;
}

/*
 * Function:
 *     _bcm_td_trill_egress_name_reset
 * Purpose:
 *     Reset My RBridge Nickname
 * Parameters:
 *   IN :  Unit
 *   IN :  trill_name
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td_trill_egress_name_reset(int unit, bcm_trill_name_t trill_name)
{
  egr_trill_rbridge_nicknames_entry_t egr_name_entry;
  int idx=0;
  bcm_trill_name_t temp_name;

  for(idx=0; idx < 4; idx++) {
      BCM_IF_ERROR_RETURN(
           soc_mem_read(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                    MEM_BLOCK_ALL, idx, &egr_name_entry));
      temp_name =
            soc_mem_field32_get(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                                        &egr_name_entry,
                                        RBRIDGE_NICKNAMEf);
      if (temp_name == trill_name) {
            soc_mem_field32_set(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                                       &egr_name_entry,
                                       RBRIDGE_NICKNAMEf, 0x0);

            BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                                          MEM_BLOCK_ALL, idx, &egr_name_entry));
            return BCM_E_NONE;
      }
  }
  return BCM_E_NOT_FOUND;
} 
/*
 * Purpose:
 *       Obtain Trill Nickname index from name
 * Parameters:
 *       unit  - Device Number
 *       source_trill_name  - (IN) Trill Root_RBridge
 */

int
bcm_td_trill_source_trill_name_idx_get(int unit, 
                                                                   bcm_trill_name_t source_trill_name,
                                                                   int *trill_name_index)
{
    egr_trill_rbridge_nicknames_entry_t  egr_name_entry;
    int idx=0;

      for(idx=0; idx < 4; idx++) {
         BCM_IF_ERROR_RETURN(
              soc_mem_read(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                       MEM_BLOCK_ALL, idx, &egr_name_entry));

         if ( source_trill_name ==  soc_mem_field32_get(unit, 
                                         EGR_TRILL_RBRIDGE_NICKNAMESm,
                                         &egr_name_entry,
                                         RBRIDGE_NICKNAMEf) ) {
              *trill_name_index =  idx;
              return BCM_E_NONE;
         }
    }
    return BCM_E_NOT_FOUND;

}

/*
 * Purpose:
 *       Obtain Trill Nickname from index
 * Parameters:
 *       unit  - Device Number
 *       source_trill_name  - (IN) Trill Root_RBridge
 */

int
bcm_td_trill_source_trill_idx_name_get(int  unit,
                                       int  trill_name_index,
                                       bcm_trill_name_t  *source_trill_name)
{
    egr_trill_rbridge_nicknames_entry_t  egr_name_entry;

    if (SOC_MEM_IS_VALID(unit, EGR_TRILL_RBRIDGE_NICKNAMESm)) {
        /* TD3TBD */
        BCM_IF_ERROR_RETURN(
                  soc_mem_read(unit, EGR_TRILL_RBRIDGE_NICKNAMESm,
                           MEM_BLOCK_ALL, trill_name_index, &egr_name_entry));

        *source_trill_name =  soc_mem_field32_get(unit,
                                             EGR_TRILL_RBRIDGE_NICKNAMESm,
                                             &egr_name_entry,
                                             RBRIDGE_NICKNAMEf);
        return BCM_E_NONE;
    } else {
        return BCM_E_NOT_FOUND;
    }
}


/*
 * Function:
 *     _bcm_td_trill_egress_dvp_set
 * Purpose:
 *     Set Egress DVP tables
 * Parameters:
 *   IN :  Unit
 *   IN :  vp
 *   IN :  trill_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td_trill_egress_dvp_set(int unit, int vp, bcm_trill_port_t *trill_port)
{
    int rv=BCM_E_UNAVAIL;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    
    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            VP_TYPEf, 0x1);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            EGRESS_RBRIDGE_NICKNAMEf, trill_port->name);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            HOPCOUNTf, trill_port->hopcount);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            MTU_VALUEf, trill_port->mtu);
    soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTEm, &egr_dvp_attribute,
                                            MTU_ENABLEf, 0x1);
    rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm,
                                            MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (soc_feature(unit,soc_feature_trill_egr_dvp_classid)) {
        egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
        sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        soc_mem_field32_set(unit, EGR_DVP_ATTRIBUTE_1m, &egr_dvp_attribute_1,
                                             TRILL__CLASS_IDf, trill_port->if_class);
        rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTE_1m,
                                        MEM_BLOCK_ALL, vp, &egr_dvp_attribute_1);
    }
    return rv;

}

/*
 * Function:
 *		_bcm_td_trill_egress_dvp_reset
 * Purpose:
 *		Set Egress DVP tables
 * Parameters:
 *		IN :  Unit
 *           IN :  nh_index
 *           IN :  trill_port
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_egress_dvp_reset(int unit, int vp)
{
    int rv=BCM_E_UNAVAIL;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;

    
    sal_memset(&egr_dvp_attribute, 0, sizeof(egr_dvp_attribute_entry_t));	
    rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTEm,
                                            MEM_BLOCK_ALL, vp, &egr_dvp_attribute);
    if (soc_feature(unit,soc_feature_trill_egr_dvp_classid)) {
        egr_dvp_attribute_1_entry_t  egr_dvp_attribute_1;
        sal_memset(&egr_dvp_attribute_1, 0, sizeof(egr_dvp_attribute_1_entry_t));
        rv = soc_mem_write(unit, EGR_DVP_ATTRIBUTE_1m,
                                            MEM_BLOCK_ALL, vp, &egr_dvp_attribute_1);
    }
    return rv;
}

/*
 * Purpose:
 *		Set Egress TRILL Header fields
 * Parameters:
 *		IN :  Unit
 *           IN :  trill_name
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_header_set(int unit, bcm_trill_name_t  trill_name)
{
    uint32  reg_val=0;
    int rv = BCM_E_NONE;

    soc_reg_field_set(unit, EGR_TRILL_HEADER_ATTRIBUTESr, 
                             &reg_val, RESERVED_FIELDf, 0);
    soc_reg_field_set(unit, EGR_TRILL_HEADER_ATTRIBUTESr, 
                             &reg_val, VERSIONf, 0);
    if (soc_mem_field_valid(unit, EGR_TRILL_RBRIDGE_NICKNAMESm, RBRIDGE_NICKNAMEf)) {
        rv = _bcm_td_trill_egress_name_set(unit, trill_name);
    } else {
        soc_reg_field_set(unit, EGR_TRILL_HEADER_ATTRIBUTESr, 
                             &reg_val, RBRIDGE_NICKNAMEf, trill_name);
        SOC_IF_ERROR_RETURN(
              WRITE_EGR_TRILL_HEADER_ATTRIBUTESr(unit, reg_val));
    }

    return rv;
}

/*
 * Purpose:
 *		Reset Egress TRILL Header fields
 * Parameters:
 *		IN :  Unit
 *           IN :  trill_name
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_header_reset(int unit, bcm_trill_name_t  trill_name)
{
    uint32  reg_val=0;
    int rv = BCM_E_NONE;

     if (soc_mem_field_valid(unit, EGR_TRILL_RBRIDGE_NICKNAMESm, RBRIDGE_NICKNAMEf)) {
            rv = _bcm_td_trill_egress_name_reset(unit, trill_name);
     } else {
         if (trill_name == soc_reg_field_get(unit, EGR_TRILL_HEADER_ATTRIBUTESr, 
                         reg_val, RBRIDGE_NICKNAMEf)) {
              soc_reg_field_set(unit, EGR_TRILL_HEADER_ATTRIBUTESr, 
                         &reg_val, RBRIDGE_NICKNAMEf, 0x0);
              SOC_IF_ERROR_RETURN(
                  WRITE_EGR_TRILL_HEADER_ATTRIBUTESr(unit, reg_val));

         }
     }
     return rv;
}

/*
 * Function:
 *		_bcm_td_trill_adjacency_register_set
 * Purpose:
 *		Set Ingress TRILL Adjacency register
 * Parameters:
 *		IN :  Unit
 *   IN :  port
 *   IN :  mac_addr
 *   IN : vid
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_adjacency_register_set(int unit, bcm_port_t  port, bcm_mac_t dst_mac_addr, bcm_vlan_t vid)
{
    uint64 reg_val;  
    uint64 mac_field;
#if defined(BCM_TOMAHAWK2_SUPPORT)
    ing_trill_adjacency_entry_t adjacency_entry;
#endif
    COMPILER_64_ZERO(reg_val);
    COMPILER_64_ZERO(mac_field);

    SAL_MAC_ADDR_TO_UINT64(dst_mac_addr, mac_field);
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_ing_trill_adjacency_is_memory)) {
        SOC_IF_ERROR_RETURN(READ_ING_TRILL_ADJACENCYm(unit, MEM_BLOCK_ANY,
                            port, &adjacency_entry));
        soc_ING_TRILL_ADJACENCYm_field32_set(unit, &adjacency_entry, 
                                             VLANf, vid);
        soc_mem_mac_addr_set(unit, ING_TRILL_ADJACENCYm, &adjacency_entry, 
                             MAC_ADDRESSf, dst_mac_addr);        
        SOC_IF_ERROR_RETURN(WRITE_ING_TRILL_ADJACENCYm(unit, MEM_BLOCK_ALL,
                            port, &adjacency_entry));
    } else
#endif
    {
        soc_reg64_field32_set(unit, ING_TRILL_ADJACENCYr, &reg_val, 
                                      VLANf, vid);
        soc_reg64_field_set(unit, ING_TRILL_ADJACENCYr, &reg_val, 
                                      MAC_ADDRESSf, mac_field);
        SOC_IF_ERROR_RETURN
                  (WRITE_ING_TRILL_ADJACENCYr(unit, port, reg_val));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_td_trill_adjacency_register_reset
 * Purpose:
 *   Reset Ingress TRILL Adjacency register
 * Parameters:
 *   IN :  Unit
 *   IN :  port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td_trill_adjacency_register_reset(int unit, bcm_port_t  port)
{
    uint64 reg_val;  
    uint64 mac_field;
    int id=0;
    bcm_mac_t dst_mac_addr;
#if defined(BCM_TOMAHAWK2_SUPPORT)
        ing_trill_adjacency_entry_t adjacency_entry;
#endif
    COMPILER_64_ZERO(reg_val);
    COMPILER_64_ZERO(mac_field);
    for (id=0; id<6; id++) {
        dst_mac_addr[id] = 0x0;
    }

    SAL_MAC_ADDR_TO_UINT64(dst_mac_addr, mac_field);
#if defined(BCM_TOMAHAWK2_SUPPORT)
    if (soc_feature(unit, soc_feature_ing_trill_adjacency_is_memory)) {
        SOC_IF_ERROR_RETURN(READ_ING_TRILL_ADJACENCYm(unit, MEM_BLOCK_ANY,
                            port, &adjacency_entry));
        soc_ING_TRILL_ADJACENCYm_field32_set(unit, &adjacency_entry, 
                                             VLANf, 0x0);
        soc_mem_mac_addr_set(unit, ING_TRILL_ADJACENCYm, &adjacency_entry, 
                             MAC_ADDRESSf, dst_mac_addr);
        SOC_IF_ERROR_RETURN(WRITE_ING_TRILL_ADJACENCYm(unit, MEM_BLOCK_ALL,
                            port, &adjacency_entry));
    } else
#endif
    {
        soc_reg64_field32_set(unit, ING_TRILL_ADJACENCYr, &reg_val, 
                                      VLANf, 0x0);
        soc_reg64_field_set(unit, ING_TRILL_ADJACENCYr, &reg_val, 
                                      MAC_ADDRESSf, mac_field);
        SOC_IF_ERROR_RETURN
                  (WRITE_ING_TRILL_ADJACENCYr(unit, port, reg_val));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   _bcm_td_trill_adjacency_set
 * Purpose:
 *   Set TRILL Adjacency
 * Parameters:
 *   IN :  Unit
 *   IN :  nh_index
 *   IN :  trill_port
 * Returns:
 *   BCM_E_XXX
 */

STATIC int
_bcm_td_trill_adjacency_set(int unit, int nh_index)
{
    int rv=BCM_E_UNAVAIL;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t if_entry;
    bcm_if_t  intf_idx;
    bcm_port_t      port=0;
    bcm_module_t modid=0, local_modid=0;
    bcm_mac_t src_mac_addr;
    bcm_mac_t dst_mac_addr;
    bcm_vlan_t vid=0;
    bcm_trunk_t trunk_id=0;
    int  num_local_ports=0, idx=0;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    uint32 trunk_id_data = 0;
    uint32 mod_port_data = 0;
    int num_bits_for_port;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &ing_nh));

    if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
        trunk_id = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                  &ing_nh, TGIDf);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }
        if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
            trunk_id_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
            SOC_MAX_NUM_PORTS, local_ports, &num_local_ports));
    } else { 
        modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                    &ing_nh, MODULE_IDf);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));

        if (modid != local_modid) {
            /* Ignore Adjacency set for non local modid */
            return BCM_E_NONE;
        }
        port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                           &ing_nh, PORT_NUMf);
        local_ports[num_local_ports++] = port;
        if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
            num_bits_for_port =
                _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));
            mod_port_data = (modid << num_bits_for_port) | port;
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                  MEM_BLOCK_ANY, nh_index, &egr_nh));

    intf_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                                   &egr_nh, INTF_NUMf);

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, 
                                 MEM_BLOCK_ANY, intf_idx, &if_entry));

    /* Obtain Source-MAC, Vlan from EGR_L3_INTF */
    soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &if_entry, 
                                             MAC_ADDRESSf, src_mac_addr);
    vid = soc_mem_field32_get(unit, EGR_L3_INTFm, 
                                             &if_entry,  VIDf);

    /* Obtain vlan and Destination-MAC from EGR_L3_NEXT_HOP */
    soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                             MAC_ADDRESSf, dst_mac_addr);
    /* Trunk supported in MY_STATION_TCAM */ 
    if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
        /* Configure MY_STATION entry for trunk */
        if (trunk_id_data) {
            BCM_IF_ERROR_RETURN(
                bcm_td_metro_myStation_add(unit, src_mac_addr, vid, trunk_id_data, 1));
            for (idx = 0; idx < num_local_ports; idx++) { 
                /* Configure ING_TRILL_ADJACENCY */
                rv =  _bcm_td_trill_adjacency_register_set(unit, local_ports[idx], dst_mac_addr, vid);
            }
            return rv;
        } else if (mod_port_data) {
            BCM_IF_ERROR_RETURN(
                bcm_td_metro_myStation_add(unit, src_mac_addr, vid,
                mod_port_data, 1));
            for (idx = 0; idx < num_local_ports; idx++) {
                 /* Configure ING_TRILL_ADJACENCY */
                 rv = _bcm_td_trill_adjacency_register_set(
                         unit, local_ports[idx], dst_mac_addr, vid);
            }
            return rv;
        }
    } 
    /* Trunk not supported in MY_STATION_TCAM */
    for (idx = 0; idx < num_local_ports; idx++) {
        /* Configure MY_STATION entry */
        BCM_IF_ERROR_RETURN(
             bcm_td_metro_myStation_add(unit, src_mac_addr, vid, local_ports[idx], 1));
        
        /* Configure ING_TRILL_ADJACENCY */
        rv =  _bcm_td_trill_adjacency_register_set(unit, local_ports[idx], dst_mac_addr, vid);
    }
    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_adjacency_reset
 * Purpose:
 *		Reset TRILL Adjacency
 * Parameters:
 *		IN :  Unit
 *   IN :  nh_index
 * Returns:
 *		BCM_E_XXX
 */

STATIC int
_bcm_td_trill_adjacency_reset(int unit, int nh_index)
{
    int rv=BCM_E_UNAVAIL;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t if_entry;
    bcm_if_t  intf_idx;
    bcm_port_t      port=0;
    bcm_module_t modid=0, local_modid=0;
    bcm_mac_t src_mac_addr;
    bcm_vlan_t vid=0;
    bcm_trunk_t trunk_id=0;
    int  num_local_ports=0, idx=0;
    bcm_port_t   local_ports[SOC_MAX_NUM_PORTS];
    uint32 trunk_id_data = 0;
    uint32 mod_port_data = 0;
    int num_bits_for_port;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_L3_NEXT_HOPm, 
                               MEM_BLOCK_ANY, nh_index, &ing_nh));

    if (soc_mem_field32_get(unit, ING_L3_NEXT_HOPm, &ing_nh, Tf)) {
        trunk_id = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                                  &ing_nh, TGIDf);
        rv = _bcm_trunk_id_validate(unit, trunk_id);
        if (BCM_FAILURE(rv)) {
            return BCM_E_PORT;
        }

        if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
            trunk_id_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
        }

        BCM_IF_ERROR_RETURN(_bcm_esw_trunk_local_members_get(unit, trunk_id,
            SOC_MAX_NUM_PORTS, local_ports, &num_local_ports));
    } else { 
        modid = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                    &ing_nh, MODULE_IDf);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get (unit, &local_modid));

        if (modid != local_modid) {
            /* Ignore Adjacency reset for non local modid */
            return BCM_E_NONE;
        }

        port = soc_mem_field32_get(unit, ING_L3_NEXT_HOPm,
                                           &ing_nh, PORT_NUMf);
        local_ports[num_local_ports++] = port;
        if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
            num_bits_for_port =
                _shr_popcount((unsigned int)SOC_PORT_ADDR_MAX(unit));
            mod_port_data = (modid << num_bits_for_port) | port;
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                               MEM_BLOCK_ANY, nh_index, &egr_nh));
 
    intf_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                                &egr_nh, INTF_NUMf);
 
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, 
                               MEM_BLOCK_ANY, intf_idx, &if_entry));
 
    /* Obtain Source-MAC from EGR_L3_INTF */
    soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &if_entry, 
                                          MAC_ADDRESSf, src_mac_addr);
 
    /* Obtain vlan and Destination-MAC from EGR_L3_NEXT_HOP */
    vid = soc_mem_field32_get(unit, EGR_L3_INTFm, 
                                          &if_entry, OVIDf);
    /* Trunk supported in MY_STATION_TCAM */ 
    if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {
        /* Delete MY_STATION entry for trunk */
        if (trunk_id_data) {
            rv = bcm_td_metro_myStation_delete(unit, src_mac_addr, vid, trunk_id_data, 1);
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
                if (rv != BCM_E_NONE) {
                    return rv;
                }
            } else {
                rv = BCM_E_NONE;
            }
            for (idx = 0; idx < num_local_ports; idx++) { 
                /* reset ING_TRILL_ADJACENCY */
                rv =  _bcm_td_trill_adjacency_register_reset(unit, local_ports[idx]);
            }
            return rv;
        } else if (mod_port_data) {
            rv = bcm_td_metro_myStation_delete(unit, src_mac_addr, vid,
                                               mod_port_data, 1);
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
                if (rv != BCM_E_NONE) {
                    return rv;
                }
            } else {
                rv = BCM_E_NONE;
            }
            for (idx = 0; idx < num_local_ports; idx++) {
                /* reset ING_TRILL_ADJACENCY */
                rv = _bcm_td_trill_adjacency_register_reset(unit,
                                                            local_ports[idx]);
            }
            return rv;
        }
    } 
    /* Trunk not supported in MY_STATION_TCAM */ 
    for (idx = 0; idx < num_local_ports; idx++) {
         /* Delete MY_STATION entry */
         rv = bcm_td_metro_myStation_delete(unit, src_mac_addr, vid, local_ports[idx], 1);
         if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
             if (rv != BCM_E_NONE) {
               return rv;
             }
         }

         /* Reset ING_TRILL_ADJACENCY */
         rv = _bcm_td_trill_adjacency_register_reset(unit, local_ports[idx]);
    }
    return rv; 
}


/*
 * Function:
 *		_bcm_td_trill_port_validate
 * Purpose:
 *		Validate Trill Port Parameters
 * Parameters:
 *   IN :  trill_port
 * Returns:
 *		BCM_E_XXX
 */

int
_bcm_td_trill_port_validate(bcm_trill_port_t *trill_port)
{

    if (!trill_port->name) {
         return BCM_E_PARAM;
    }

    /* Check for unsupported Flag */
    if (trill_port->flags & (~(BCM_TRILL_PORT_WITH_ID |
                        BCM_TRILL_PORT_DROP |
                        BCM_TRILL_PORT_COUNTED |                        
                        BCM_TRILL_PORT_NETWORK |
                        BCM_TRILL_PORT_REPLACE |
                        BCM_TRILL_PORT_MULTICAST |
                        BCM_TRILL_PORT_LOCAL  ))) {
         return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

 /*
  * Function:
  *     _bcm_td_trill_root_rbridge_check
  * Purpose:
  *     Check if Root RBridge already exists
  * Parameters:
  *     IN : unit
  * Returns:
  *     BCM_E_XXX
  */
STATIC int
_bcm_td_trill_root_rbridge_check(int unit, bcm_trill_port_t *trill_port)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int idx=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
         if (trill_info->rootBridge[idx] == trill_port->name) {
              return (BCM_E_EXISTS); 
         }
    }
    /* TRILL DVP to Root RBridge do NOT exist */
    return BCM_E_NONE;
}

 /*
  * Function:
  *     _bcm_td_trill_root_bridge_set
  * Purpose:
  *     Configure Root RBridge
  * Parameters:
  *     IN : unit
  * Returns:
  *     BCM_E_XXX
  */
 
STATIC int
_bcm_td_trill_root_bridge_set(int unit, bcm_trill_port_t *trill_port, int vp)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv=BCM_E_NONE;
    uint8 trill_tree_profile_idx=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    /* Verify whether Root Rbridge already exists? */
    rv = _bcm_td_trill_root_rbridge_check(unit, trill_port);
    if (rv == BCM_E_EXISTS) {
        if (!(trill_port->flags & BCM_TRILL_PORT_REPLACE)) {
             (void) _bcm_vp_free(unit, _bcmVpTypeTrill, 1, vp);
             return rv;
        }
    }

    /* Set Tree profile */
    rv = _bcm_td_trill_tree_profile_set (unit, trill_port->name, 
    trill_port->multicast_hopcount, &trill_tree_profile_idx);
    if (rv < 0) {
         trill_info->rootBridge[trill_tree_profile_idx] = 0;
         (void) _bcm_vp_free(unit, _bcmVpTypeTrill, 1, vp);
         return rv;
    }
    return rv;
}

 /*
  * Function:
  *     _bcm_td_trill_root_bridge_clear
  * Purpose:
  *     Configure Root RBridge
  * Parameters:
  *     IN : unit
  * Returns:
  *     BCM_E_XXX
  */
 
STATIC int
_bcm_td_trill_root_bridge_clear(int unit, bcm_trill_port_t *trill_port)
{
    int rv=BCM_E_NONE;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    /* Verify whether Root Rbridge already exists? */
    rv = _bcm_td_trill_root_rbridge_check(unit, trill_port);
    if (rv == BCM_E_EXISTS) {
        /* Clear Tree profile */
        rv = _bcm_td_trill_tree_profile_reset (unit, trill_port->name);
    }
    return rv;
}

 /*
  * Function:
  *     _bcm_td_trill_rbridge_name_exists
  * Purpose:
  *     Configure Root RBridge
  * Parameters:
  *     IN : unit
  * Returns:
  *     BCM_E_XXX
  */
 
STATIC int
_bcm_td_trill_rbridge_name_exists(int unit, bcm_trill_port_t *trill_port)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int idx=0, num_vp=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);

    for (idx = 0; idx < num_vp; idx++) {
         if (trill_info->rBridge[idx] == trill_port->name) {
              return (BCM_E_EXISTS); 
         }
    }
    /* TRILL DVP to given RBridge_name  do NOT exist */
    return BCM_E_NONE;
}

 /*
  * Function:
  *     bcm_td_multicast_trill_rootname_get
  * Purpose:
  *     Get  Root RBridge name
  * Parameters:
  *     IN : unit
  * Returns:
  *     BCM_E_XXX
  */

int
bcm_td_multicast_trill_rootname_get(int unit, bcm_multicast_t group, bcm_trill_name_t  *root_name)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int l3mc_index=-1;
    uint8  trill_tree_id=0xFF;
    int rv = BCM_E_NONE;

    trill_info = TRILL_INFO(unit);

    l3mc_index = _BCM_MULTICAST_ID_GET(group);
    rv = bcm_td_multicast_trill_group_get(unit, l3mc_index, &trill_tree_id);
    if (BCM_SUCCESS(rv)) {
       *root_name = trill_info->rootBridge[trill_tree_id];
    }
    return rv;
}



/*
 * Purpose:
 *      Activate Trunk Port-membership from 
 *     ING_TRILL_ADJACENY, EGR_PORT_TO_NHI_MAPPING
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
*/

int
bcm_td_trill_trunk_member_add (int unit, bcm_trunk_t trunk_id, 
                int trunk_member_count, bcm_port_t *trunk_member_array) 
{ 
    int idx=0;
    int rv=BCM_E_NONE;
    int  nh_index=0, old_nh_index=-1;
    bcm_l3_egress_t egr; 
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t if_entry;
    bcm_if_t  intf_idx;
    bcm_vlan_t vid=0;
    bcm_mac_t dst_mac_addr;
    int gport = 0;
    int modid = 0;
    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }
    
    /* Obtain nh_index for trunk_id */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        return rv;
    }
    bcm_l3_egress_t_init(&egr);
    BCM_IF_ERROR_RETURN(bcm_td_trill_egress_get(unit, &egr, nh_index));

    if (egr.flags & BCM_L3_TRILL_ONLY) {
        for (idx = 0; idx < trunk_member_count; idx++) {
            if (soc_mem_field_valid(unit, MY_STATION_TCAMm, SOURCE_FIELDf)) {                         
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                                                 MEM_BLOCK_ANY, nh_index, &egr_nh));
                intf_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                                               &egr_nh, INTF_NUMf);
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, 
                                                 MEM_BLOCK_ANY, intf_idx, &if_entry));
                /* Obtain Vlan from EGR_L3_INTF */
                vid = soc_mem_field32_get(unit, EGR_L3_INTFm, 
                                          &if_entry,  VIDf);
                /* Obtain Destination-MAC from EGR_L3_NEXT_HOP */
                soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm, &egr_nh, 
                                     MAC_ADDRESSf, dst_mac_addr);
                /* Set ING_TRILL_ADJACENCY */
                BCM_IF_ERROR_RETURN(
                    _bcm_td_trill_adjacency_register_set(unit, trunk_member_array[idx], 
                                                         dst_mac_addr, vid));
            } else {
                BCM_IF_ERROR_RETURN(
                    _bcm_td_trill_adjacency_set(unit, nh_index));
            }
            BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
            BCM_GPORT_MODPORT_SET(gport, modid, trunk_member_array[idx]);
            BCM_IF_ERROR_RETURN(
                _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));
            if (!old_nh_index) {
                BCM_IF_ERROR_RETURN(
                    _bcm_trx_gport_to_nhi_set(unit, gport, nh_index));
             }

        }
    }
    return rv;
}


/*
 * Purpose:
 *      Clear Trunk Port-membership from 
 *     ING_TRILL_ADJACENY, EGR_PORT_TO_NHI_MAPPING
 * Parameters:
 *     unit  - (IN) Device Number
 *     tgid - (IN)  Trunk Group ID
 *     trunk_member_count - (IN) Count of Trunk members to be deleted
 *     trunk_member_array - (IN) Trunk member ports to be deleted
*/

int
bcm_td_trill_trunk_member_delete (int unit, bcm_trunk_t trunk_id, 
                int trunk_member_count, bcm_port_t *trunk_member_array) 
{ 
   int idx=0;
   int rv=BCM_E_NONE;
    int  old_nh_index=0;
    bcm_mac_t src_mac_addr;
    bcm_vlan_t vid=0;
    bcm_l3_egress_t egr;
    egr_l3_next_hop_entry_t egr_nh;
    egr_l3_intf_entry_t if_entry;
    bcm_if_t  intf_idx;
    int nh_index = 0;
    int gport = 0;
    int modid = 0;

    rv = _bcm_trunk_id_validate(unit, trunk_id);
    if (BCM_FAILURE(rv)) {
        return BCM_E_PORT;
    }

    /* Obtain nh_index for trunk_id  */
    rv = _bcm_xgs3_trunk_nh_store_get(unit, trunk_id, &nh_index);
    if (nh_index == 0) {
        return rv;
    }
    bcm_l3_egress_t_init(&egr);
    BCM_IF_ERROR_RETURN(bcm_td_trill_egress_get(unit, &egr, nh_index));

    if (egr.flags & BCM_L3_TRILL_ONLY) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        for (idx = 0; idx < trunk_member_count; idx++) { 
            /* Reset ING_TRILL_ADJACENCY */
            rv = _bcm_td_trill_adjacency_register_reset(unit, trunk_member_array[idx]);
            if (BCM_FAILURE(rv)) {
                return rv;
            }           
            BCM_GPORT_MODPORT_SET(gport, modid, trunk_member_array[idx]);
            BCM_IF_ERROR_RETURN(
                _bcm_trx_gport_to_nhi_get(unit, gport, &old_nh_index));  

            if (0 != old_nh_index) {
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm, 
                            MEM_BLOCK_ANY, old_nh_index, &egr_nh));
                intf_idx = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, 
                            &egr_nh, INTF_NUMf);
                BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_INTFm, 
                            MEM_BLOCK_ANY, intf_idx, &if_entry));
                /* Obtain Source-MAC from EGR_L3_INTF */
                soc_mem_mac_addr_get(unit, EGR_L3_INTFm, &if_entry, 
                        MAC_ADDRESSf, src_mac_addr);
                /* Obtain vlan and Destination-MAC from EGR_L3_NEXT_HOP */
                vid = soc_mem_field32_get(unit, EGR_L3_INTFm, 
                        &if_entry, OVIDf);

                /* Delete MY_STATION entry */
                rv = bcm_td_metro_myStation_delete(unit, src_mac_addr, vid, trunk_member_array[idx], 1);
                if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
                    if (rv != BCM_E_NONE) {
                       return rv;
                    }
                }

                rv = _bcm_trx_gport_to_nhi_set(unit, gport, 0);
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
            }
        } 
    }
    return rv;
}

/*
 * Purpose:
 *	 Add Trill port to TRILL Cloud
 * Parameters:
 *	 unit	 - (IN) Device Number
 *	 trill_port - (IN/OUT) trill port information (OUT : trill_port_id)
*/
int 
bcm_td_trill_port_add(int unit, bcm_trill_port_t *trill_port)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv=BCM_E_NONE;
    int nh_ecmp_index = -1;
    int old_nh_ecmp_index = -1, old_nh_index = -1;
    int ref_count = 0;
    int  vp, num_vp=0;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    uint32 flags=0;
    bcm_if_t intf;
    int cml_default_enable=0, cml_default_new=0, cml_default_move=0;
    _bcm_vp_info_t vp_info;
    int bit_num = 0;
    int network_group=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    /* Validate paramters */
    if ((trill_port->name < 0)     || (trill_port->name > 0xffff)  ||
        (trill_port->mtu < 0)      || (trill_port->mtu > 0x3fff)   ||
        (trill_port->hopcount < 0) || (trill_port->hopcount > 0x3f)) {
        return BCM_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_extended_address_class)) {
        if ((trill_port->if_class > SOC_EXT_ADDR_CLASS_MAX(unit)) ||
            (trill_port->if_class < 0)) {
            return (BCM_E_PARAM);
        }
    } else {
        bit_num = soc_mem_field_length(unit, MPLS_ENTRYm, TRILL__CLASS_IDf);
        if ((trill_port->if_class > (( 1 << bit_num ) - 1)) ||
            (trill_port->if_class < 0)) {
            return BCM_E_PARAM;
        }
    }

    trill_info = TRILL_INFO(unit);
    BCM_IF_ERROR_RETURN(
                _bcm_td_trill_port_validate(trill_port));

    /* 
     * Extract next-hop index from egress object  OR
     * Extract ecmp group index from multipath egress object. */

    if (!(trill_port->flags & BCM_TRILL_PORT_LOCAL)) {
         BCM_IF_ERROR_RETURN(
                bcm_xgs3_get_nh_from_egress_object(unit,
                           trill_port->egress_if, &flags, 1, &nh_ecmp_index));
        if (nh_ecmp_index == BCM_XGS3_L3_INVALID_INDEX) {
            return BCM_E_PARAM;
        }
    }

    if (flags == BCM_L3_MULTIPATH) {
         trill_port->flags |= _BCM_TRILL_PORT_MULTIPATH;
    }

    _bcm_vp_info_init(&vp_info);
    vp_info.vp_type = _bcmVpTypeTrill;
    if (trill_port->flags & BCM_TRILL_PORT_NETWORK) {
        vp_info.flags |= _BCM_VP_INFO_NETWORK_PORT;
    } else {
        vp_info.flags &= ~(_BCM_VP_INFO_NETWORK_PORT);
    }
 
    if (trill_port->flags & BCM_TRILL_PORT_REPLACE) {

         vp = BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id);
         if (vp == -1) {
            return BCM_E_PARAM;
         }
         if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
              return BCM_E_NOT_FOUND;
         }
         if (trill_info->rBridge[vp] != trill_port->name) {
           return BCM_E_PARAM; 
         }

         BCM_IF_ERROR_RETURN(
              READ_SOURCE_VPm(unit, MEM_BLOCK_ANY, vp, &svp));
         BCM_IF_ERROR_RETURN(
              READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

         /* Decrement port's VP count */
         rv = _bcm_td_trill_port_cnt_update(unit, trill_port->trill_port_id, vp, FALSE);
         if (rv < 0) {
            return rv;
         }

         /* Get replaced egress NHI/ECMP index */ 
         if (soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf)) {
            old_nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
         } else {
            old_nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
         }
    } else if (trill_port->flags & BCM_TRILL_PORT_WITH_ID ) {
    
         if (!BCM_GPORT_IS_TRILL_PORT(trill_port->trill_port_id)) {
              return (BCM_E_BADID);
         }

         /* Check if rbridge_name already exists */
         BCM_IF_ERROR_RETURN(_bcm_td_trill_rbridge_name_exists(unit, trill_port));

         num_vp = soc_mem_index_count(unit, SOURCE_VPm);
         vp = BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id);
         if (vp >= num_vp) {
              return (BCM_E_BADID);
         }
         rv = _bcm_vp_used_get(unit, vp, _bcmVpTypeTrill);
         if (rv) {
              return (BCM_E_EXISTS);
         }
         BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
         sal_memset(&svp, 0, sizeof(source_vp_entry_t));
   
    } else {

         /* Check if rbridge_name already exists */
         BCM_IF_ERROR_RETURN(_bcm_td_trill_rbridge_name_exists(unit, trill_port));

         /* allocate a new VP index */
         num_vp = soc_mem_index_count(unit, SOURCE_VPm);
         rv = _bcm_vp_alloc(unit, 0, (num_vp - 1), 1, SOURCE_VPm, vp_info, &vp);
         if (rv < 0) {
              return rv;
         }
         sal_memset(&svp, 0, sizeof(source_vp_entry_t));
  
         BCM_IF_ERROR_RETURN(_bcm_vp_used_set(unit, vp, vp_info));
    }

    /* Set Virtual Port */
    soc_SOURCE_VPm_field32_set(unit, &svp, CLASS_IDf, 
                                  trill_port->if_class);
    soc_SOURCE_VPm_field32_set(unit, &svp, 
                                  ENTRY_TYPEf, 0x3); /* TRILL VP */

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        network_group = trill_port->network_group_id;
        rv = _bcm_validate_splithorizon_network_group(unit,
                    trill_port->flags & BCM_TRILL_PORT_NETWORK,
                    &network_group);
        BCM_IF_ERROR_RETURN(rv);
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_GROUPf,
                        network_group);
    } else {
        soc_SOURCE_VPm_field32_set(unit, &svp, NETWORK_PORTf, 0x1);
        network_group=_bcmVpIngDvpPortTypeNetwork;
    }

        /* Set the CML */
       BCM_TRILL_CHECK_ERROR_RETURN(
            _bcm_vp_default_cml_mode_get (unit, 
                           &cml_default_enable, &cml_default_new, 
                           &cml_default_move), unit, vp);

        if (cml_default_enable) {
            /* Set the CML to default values */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, cml_default_new);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, cml_default_move);
        } else {
            /* Set the CML to PVP_CML_SWITCH by default (hw learn and forward) */
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_NEWf, 0x8);
            soc_SOURCE_VPm_field32_set(unit, &svp, CML_FLAGS_MOVEf, 0x8);
        }

    soc_SOURCE_VPm_field32_set(unit, &svp, TPID_SOURCEf, 0x3);
    BCM_TRILL_CHECK_ERROR_RETURN(
                   WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp), unit, vp);
   
    if (trill_port->flags & BCM_TRILL_PORT_LOCAL) { /* My RBridge */
        intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_ecmp_index;
        BCM_IF_ERROR_RETURN(
            _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigSet, vp, 
                                   _bcmVpIngDvpVpTypeTRILL, intf, 
                                   network_group));

        BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_td_trill_egress_dvp_set(unit, vp, trill_port), unit, vp);
        BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_td_trill_header_set(unit, trill_port->name), unit, vp);

         /* ******** TRILL Decap Entry  ******************** */
#if   defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            rv =   _bcm_tr3_trill_decap_entry_set(unit,  trill_port);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            rv =   _bcm_td_trill_decap_entry_set(unit,  trill_port);
        }
         if (rv < 0) {
              return rv;
         }
         trill_info->rBridge[vp] = trill_port->name;

         /* Root RBridge case */
         if (trill_port->flags &  BCM_TRILL_PORT_MULTICAST) {
              /* Add Tree profile */
              rv = _bcm_td_trill_root_bridge_set(unit, trill_port, vp);
         } else if (trill_port->flags &  BCM_TRILL_PORT_REPLACE) {
              /* Update as Regular RBridge  - if Root Rbridge already exists */
              rv = _bcm_td_trill_root_bridge_clear(unit, trill_port);
         }
         if (rv < 0) {
              return rv;
         }
    } else {

         /* Root RBridge */
         if (trill_port->flags &  BCM_TRILL_PORT_MULTICAST) {
              /* Add Tree profile */
              rv = _bcm_td_trill_root_bridge_set(unit, trill_port, vp);
         } else if (trill_port->flags &  BCM_TRILL_PORT_REPLACE) {
              /* Update as Regular RBridge  - if Root Rbridge already exists */
              rv = _bcm_td_trill_root_bridge_clear(unit, trill_port);
         }
         if (rv < 0) {
              return rv;
         }
         /* ******** TRILL Access ECMP/Next-Hop  ********** */
         if (trill_port->flags&_BCM_TRILL_PORT_MULTIPATH) {
             intf = BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit) + nh_ecmp_index;
         } else {
             intf = BCM_XGS3_DVP_EGRESS_IDX_MIN(unit) + nh_ecmp_index;   
         }
         BCM_IF_ERROR_RETURN(
             _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigSet, vp, 
                                    _bcmVpIngDvpVpTypeTRILL, 
                                    intf, network_group));


         BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_td_trill_egress_dvp_set(unit, vp, trill_port), unit, vp);

#if   defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
         /* ******** TRILL Transit Forwarding Entry  ********** */
            BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_tr3_trill_transit_entry_set(unit, trill_port, nh_ecmp_index), unit, vp);

         /* ******** TRILL Decap Learn Entry  ********** */
            BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_tr3_trill_learn_entry_set(unit, trill_port, vp), unit, vp);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
         /* ******** TRILL Transit Forwarding Entry  ********** */
            BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_td_trill_transit_entry_set(unit, trill_port, nh_ecmp_index), unit, vp);
         /* ******** TRILL Decap Learn Entry  ********** */
            BCM_TRILL_CHECK_ERROR_RETURN(
              _bcm_td_trill_learn_entry_set(unit, trill_port, vp), unit, vp);
        }
        trill_info->rBridge[vp] = trill_port->name;
    }
    /* Set TRILL  port ID */
    BCM_GPORT_TRILL_PORT_ID_SET(trill_port->trill_port_id, vp);
    trill_port->encap_id = nh_ecmp_index;
    /* Increment new-port's VP count */
    BCM_IF_ERROR_RETURN(
       _bcm_td_trill_port_cnt_update(unit, trill_port->trill_port_id, vp, TRUE));

    if (old_nh_ecmp_index != -1) {
          BCM_IF_ERROR_RETURN(
               bcm_xgs3_get_ref_count_from_nhi(
                    unit, BCM_L3_MULTIPATH, &ref_count, old_nh_ecmp_index));
    }
    if (old_nh_index != -1) {
      BCM_IF_ERROR_RETURN(
               bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count, old_nh_index));
    }

    return BCM_E_NONE;
}

 /*
  * Purpose:
  *      Get a trill port 
  * Parameters:
  *      unit    - (IN) Device Number
  *      trill_port - (IN/OUT) Trill port information (IN : trill_port_id)
  */

int 
bcm_td_trill_port_get(int unit, bcm_trill_port_t *trill_port)
{
    uint32 vp=0;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    source_vp_entry_t svp;
    egr_trill_tree_profile_entry_t  egr_trill_profile;
    ing_dvp_table_entry_t dvp;
    int  nh_ecmp_index=0;
    int ecmp=0;
    int network_port=0;
    uint8  trill_tree_id=0;
    _bcm_vp_info_t vp_info;

    vp = BCM_GPORT_TRILL_PORT_ID_GET(trill_port->trill_port_id);
    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
         return BCM_E_NOT_FOUND;
    }

    /* Initialize the structure */
    bcm_trill_port_t_init(trill_port);
    BCM_GPORT_TRILL_PORT_ID_SET(trill_port->trill_port_id, vp);

    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                       MEM_BLOCK_ALL, vp, &egr_dvp_attribute));
  
    trill_port->name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, 
                                            &egr_dvp_attribute, EGRESS_RBRIDGE_NICKNAMEf);
    trill_port->mtu = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, 
                                            &egr_dvp_attribute, MTU_VALUEf);
    trill_port->hopcount = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, 
                                            &egr_dvp_attribute, HOPCOUNTf);
  
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, SOURCE_VPm,
                        MEM_BLOCK_ALL, vp, &svp));
  
    trill_port->if_class = soc_mem_field32_get(unit, SOURCE_VPm, 
                                            &svp, CLASS_IDf);

    BCM_IF_ERROR_RETURN(
         READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
         nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
         trill_port->egress_if  =  nh_ecmp_index + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
    } else {
         nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
         /* Extract next hop index from unipath egress object. */
         if (nh_ecmp_index) {
              trill_port->egress_if  =  nh_ecmp_index + BCM_XGS3_EGRESS_IDX_MIN(unit);
         } else {
              trill_port->flags |=  BCM_TRILL_PORT_LOCAL;
         }
    }

    BCM_IF_ERROR_RETURN(_bcm_vp_info_get(unit, vp, &vp_info));
    if (vp_info.flags & _BCM_VP_INFO_NETWORK_PORT) {
        network_port = TRUE;
    }

    if (network_port) {
         trill_port->flags |=  BCM_TRILL_PORT_NETWORK;
    }

    if (soc_feature(unit, soc_feature_multiple_split_horizon_group)) {
        trill_port->network_group_id = soc_ING_DVP_TABLEm_field32_get
                                         (unit, &dvp, NETWORK_GROUPf);
    }
    /* Check Tree-ID */
    (void) bcm_td_trill_tree_profile_get (unit, trill_port->name, &trill_tree_id);
    if (trill_tree_id < BCM_MAX_NUM_TRILL_TREES) {
          trill_port->flags |=  BCM_TRILL_PORT_MULTICAST;
          BCM_IF_ERROR_RETURN(
                 soc_mem_read(unit, EGR_TRILL_TREE_PROFILEm,
                        MEM_BLOCK_ALL, trill_tree_id, &egr_trill_profile));
          trill_port->multicast_hopcount = soc_mem_field32_get(unit, EGR_TRILL_TREE_PROFILEm, 
                                            &egr_trill_profile, HOPCOUNTf);
    }

    return BCM_E_NONE;
}

/*
 * Purpose:
 *      Get all trill ports from TRILL Cloud
 * Parameters:
 *      unit     - (IN) Device Number
 *      port_max   - (IN) Maximum number of Trill ports in array
 *      port_array - (OUT) Array of Trill ports
 *      port_count - (OUT) Number of Trill ports returned in array
 *
 * Returns:
 *      BCM_E_XXX
 * Notes: If port_max == 0 and port_array == NULL, actual number of
 *        trill ports in the TRILL cloud will be returned.
 */

int 
bcm_td_trill_port_get_all(int unit, int port_max, bcm_trill_port_t *port_array, int *port_count )
{
    int rv = BCM_E_NONE;
    int vp=0;
    uint32 num_vp=0;
    ing_dvp_table_entry_t dvp;
 
    /* Input parameters check. */
    if (port_max < 0) {
        return BCM_E_PARAM;
    }

    if ((port_max > 0) && (NULL == port_array)) {
        return BCM_E_PARAM;
    }

    if (NULL == port_count) {
        return (BCM_E_PARAM);
    }

    *port_count = 0;

    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    for (vp = 0; vp < num_vp; vp++) {
         if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
              continue;
         }
         if (0 == port_max) {
             (*port_count)++;
         } else {
             if (*port_count == port_max) {
                  break;
             }
             BCM_IF_ERROR_RETURN(
                  READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
     
             if (0x1 == soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf)){
                  BCM_GPORT_TRILL_PORT_ID_SET(port_array[*port_count].trill_port_id, vp);
                  rv = bcm_td_trill_port_get(unit,
                                        &port_array[*port_count]);
                  if (rv<0) {
                       return rv;
                  }
                  (*port_count)++;
             }
         }
    }
    return rv;
}


/*
 * Purpose:
 *	 Delete Trill port from TRILL Cloud
 * Parameters:
 *	 unit		- (IN) Device Number
 *		trill_port_id - (IN) Trill port ID
 */
int
bcm_td_trill_port_delete(int unit, bcm_gport_t trill_port_id)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int vp=0;
    source_vp_entry_t svp;
    ing_dvp_table_entry_t dvp;
    egr_dvp_attribute_entry_t  egr_dvp_attribute;
    int rv=BCM_E_NONE;
    int nh_ecmp_index=-1;
    int ecmp =-1;
    bcm_trill_name_t trill_name;
    uint32  flags=0;
    int  ref_count=0;
    bcm_trill_port_t  trill_port;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);
    vp = BCM_GPORT_TRILL_PORT_ID_GET(trill_port_id);
    if (vp == -1) {
        return BCM_E_PARAM;
    }

    if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {
         return BCM_E_NOT_FOUND;
    }

    bcm_trill_port_t_init(&trill_port);
    trill_port.trill_port_id = trill_port_id;
    BCM_IF_ERROR_RETURN(
         bcm_td_trill_port_get(unit, &trill_port));
    
    BCM_IF_ERROR_RETURN(
       soc_mem_read(unit, EGR_DVP_ATTRIBUTEm,
                       MEM_BLOCK_ALL, vp, &egr_dvp_attribute));
  
    trill_name = soc_mem_field32_get(unit, EGR_DVP_ATTRIBUTEm, 
                                            &egr_dvp_attribute, EGRESS_RBRIDGE_NICKNAMEf);

    /* Decrement port's VP count */
    rv = _bcm_td_trill_port_cnt_update(unit, trill_port_id, vp, FALSE);
    if (rv < 0) {
        return rv;
    }


    BCM_IF_ERROR_RETURN(
         READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
    ecmp = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMPf);
    if (ecmp) {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, ECMP_PTRf);
        flags = BCM_L3_MULTIPATH;
        BCM_IF_ERROR_RETURN(
             bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
    } else {
        nh_ecmp_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
        if(nh_ecmp_index != 0) {
              BCM_IF_ERROR_RETURN(
                   bcm_xgs3_get_ref_count_from_nhi(unit, flags, &ref_count, nh_ecmp_index));
        }
    }
    if (nh_ecmp_index) {
#if   defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            BCM_IF_ERROR_RETURN(
              _bcm_tr3_trill_transit_entry_reset(unit, vp, nh_ecmp_index, ecmp));
            BCM_IF_ERROR_RETURN(
              _bcm_tr3_trill_learn_entry_reset(unit, vp));

        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(
              _bcm_td_trill_transit_entry_reset(unit, vp, nh_ecmp_index, ecmp));
            BCM_IF_ERROR_RETURN(
              _bcm_td_trill_learn_entry_reset(unit, vp));
        }
         BCM_IF_ERROR_RETURN(
              _bcm_td_trill_egress_dvp_reset(unit, vp));
    } else {
#if   defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            BCM_IF_ERROR_RETURN(
              _bcm_tr3_trill_decap_entry_reset(unit,  vp));
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(
              _bcm_td_trill_decap_entry_reset(unit,  vp));
        }
         BCM_IF_ERROR_RETURN(
              _bcm_td_trill_egress_dvp_reset(unit, vp));
    }
    /* Clear the SVP and DVP table entries */
    sal_memset(&svp, 0, sizeof(source_vp_entry_t));
    BCM_IF_ERROR_RETURN(
         WRITE_SOURCE_VPm(unit, MEM_BLOCK_ALL, vp, &svp));

    BCM_IF_ERROR_RETURN(
        _bcm_vp_ing_dvp_config(unit, _bcmVpIngDvpConfigClear, vp, 
                               ING_DVP_CONFIG_INVALID_VP_TYPE, 
                               ING_DVP_CONFIG_INVALID_INTF_ID, 
                               ING_DVP_CONFIG_INVALID_PORT_TYPE));

    if (trill_port.flags & BCM_TRILL_PORT_LOCAL) {
         BCM_IF_ERROR_RETURN(
             _bcm_td_trill_header_reset(unit, trill_name));
    } 

    if (trill_port.flags &  BCM_TRILL_PORT_MULTICAST) {
         BCM_IF_ERROR_RETURN(
              _bcm_td_trill_tree_profile_reset (unit, trill_name));
    }
    /* Free RBridge */
    trill_info->rBridge[vp] = 0;

    /* Free the VP */
    (void) _bcm_vp_free(unit, _bcmVpTypeTrill, 1, vp);

    return rv;
}

 /*
  * Purpose:
  *	  Delete all Trill ports from TRILL Cloud
  * Parameters:
  *	  unit - Device Number
  */

int 
bcm_td_trill_port_delete_all(int unit)
{
    int rv=BCM_E_NONE;
    uint32 vp=0, num_vp=0;
    ing_dvp_table_entry_t dvp;
    bcm_gport_t trill_port_id;

    num_vp = soc_mem_index_count(unit, ING_DVP_TABLEm);
    for (vp = 0; vp < num_vp; vp++) {
         /* Check for validity of the VP */
         if (!_bcm_vp_used_get(unit, vp, _bcmVpTypeTrill)) {  
              continue;
         }
         BCM_IF_ERROR_RETURN(
              READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));
         if ((soc_ING_DVP_TABLEm_field32_get(unit, &dvp, VP_TYPEf) == 0x1)) {
              BCM_GPORT_TRILL_PORT_ID_SET(trill_port_id, vp);
              rv = bcm_td_trill_port_delete(unit, trill_port_id);
              if (rv < 0) {
                   return rv;
              }
         }
    }
    return rv;
}

/*
 * Function:
 *		_bcm_td_trill_multicast_transit_entry_key_set
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
 _bcm_td_trill_multicast_transit_entry_key_set (int unit, bcm_trill_name_t root_name,
                                          mpls_entry_entry_t   *tr_ent)
{
    uint8  trill_tree_id=0;

    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id == BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                       TRILL__TREE_IDf, trill_tree_id);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                       KEY_TYPEf, 0x5);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                       TRILL__RBRIDGE_NICKNAMEf, root_name);
    soc_mem_field32_set(unit, MPLS_ENTRYm, tr_ent,
                                       VALIDf, 0x1);
    return BCM_E_NONE;
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
bcm_td_trill_multicast_transit_entry_set(int unit, int flags,
                   bcm_trill_name_t root_name, int mc_index, uint8  trill_tree_id)
{
    mpls_entry_entry_t  tr_key, tr_entry;
    int rv = BCM_E_UNAVAIL;
    int entry_index=0;

    sal_memset(&tr_key, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&tr_entry, 0, sizeof(mpls_entry_entry_t));

    (void) _bcm_td_trill_multicast_transit_entry_key_set(unit, root_name, &tr_key);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &entry_index,
                        &tr_key, &tr_entry, 0);
    if (rv == SOC_E_NONE) {
         soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                        TRILL__L3MC_INDEXf, mc_index);
         soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                        TRILL__TREE_IDf, (uint32) trill_tree_id);
         /* Enable Multicast traffic */
         if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) { 
              if (flags & BCM_TRILL_PORT_DROP) {
                   soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x1);
              } else {
                   soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x0);
              }
         }
         rv = soc_mem_write(unit, MPLS_ENTRYm,
                        MEM_BLOCK_ALL, entry_index, &tr_entry);
    } else {
        /* Root Entry Must be present */
        return rv;
    }
    return rv;

}


/*
 * Purpose:
 *      Enable TRILL Multicast traffic   
 * Parameters:
 *      unit     - (IN) Device Number
 *      root_name  - (IN) trill Root_RBridge
 */

int
bcm_td_trill_multicast_enable(int unit, int flags, bcm_trill_name_t root_name)
{
    mpls_entry_entry_t  tr_key, tr_entry;
    int rv = BCM_E_UNAVAIL;
    int entry_index=0;

    sal_memset(&tr_key, 0, sizeof(mpls_entry_entry_t));
    sal_memset(&tr_entry, 0, sizeof(mpls_entry_entry_t));

    (void) _bcm_td_trill_multicast_transit_entry_key_set(unit, root_name, &tr_key);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &entry_index,
                        &tr_key, &tr_entry, 0);
    if (rv == SOC_E_NONE) {
         /* Enable Multicast traffic */
         if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) { 
              if (flags & BCM_TRILL_PORT_DROP) {
                   soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x1);
              } else {
                   soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_entry,
                                         TRILL__MCAST_DST_DISCARDf, 0x0);
              }
         }
         rv = soc_mem_write(unit, MPLS_ENTRYm,
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
bcm_td_trill_multicast_disable(int unit, int flags, bcm_trill_name_t root_name)
{
    mpls_entry_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));

    (void) _bcm_td_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
   
    /* Disable Multicast Traffic */
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) { 
         soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                        TRILL__MCAST_DST_DISCARDf, 0x1);
    }
    rv = soc_mem_write(unit, MPLS_ENTRYm,
                   MEM_BLOCK_ALL, index, &tr_ent);

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
bcm_td_trill_multicast_transit_entry_reset(int unit, bcm_trill_name_t root_name)
{
    mpls_entry_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));

    (void) _bcm_td_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
   
    /* Remove l3mc_index from HW entry */
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                   TRILL__L3MC_INDEXf, 0);
    /* Remove Multicast Tree Id */
    soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                        TRILL__TREE_IDf, (uint32) 0);
    /* Disable Multicast Traffic */
    if (soc_mem_field_valid(unit, MPLS_ENTRYm, TRILL__MCAST_DST_DISCARDf)) { 
         soc_mem_field32_set(unit, MPLS_ENTRYm, &tr_ent,
                        TRILL__MCAST_DST_DISCARDf, 0x1);
    }
    rv = soc_mem_write(unit, MPLS_ENTRYm,
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
bcm_td_trill_multicast_transit_entry_get(int unit, bcm_trill_name_t root_name, bcm_multicast_t *group)
{
    mpls_entry_entry_t  tr_ent;
    int rv = BCM_E_UNAVAIL;
    int index=0;

    sal_memset(&tr_ent, 0, sizeof(mpls_entry_entry_t));

    (void) _bcm_td_trill_multicast_transit_entry_key_set(unit, root_name, &tr_ent);

    rv = soc_mem_search(unit, MPLS_ENTRYm, MEM_BLOCK_ANY, &index,
                             &tr_ent, &tr_ent, 0);
    if (rv < 0) {
         return rv;
    }
   
    _BCM_MULTICAST_GROUP_SET(*group, 
                      _BCM_MULTICAST_TYPE_TRILL,  
                      soc_mem_field32_get (unit, MPLS_ENTRYm, (uint32 *)&tr_ent,
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
bcm_td_trill_multicast_transit_entry_reset_all (int unit, uint8 tree_id)
{
    int idx=0, num_entries=0;
    int rv = BCM_E_UNAVAIL;
    mpls_entry_entry_t ment;
    int name;

    _bcm_td_trill_bookkeeping_t *trill_info = TRILL_INFO(unit);
    /* Tree ID and Root name is 1:1 mapping.
       Match on Rbridge name to match */
    name = trill_info->rootBridge[tree_id];

    num_entries = soc_mem_index_count(unit, MPLS_ENTRYm);
    for (idx = 0; idx < num_entries; idx++) {
        rv = READ_MPLS_ENTRYm(unit, MEM_BLOCK_ANY, idx, &ment);
        if (rv < 0) {
            return rv;
        }
        if (!soc_MPLS_ENTRYm_field32_get(unit, &ment, VALIDf)) {
            continue;
        }
        if (soc_MPLS_ENTRYm_field32_get(unit, &ment, KEY_TYPEf) != 0x5) {
            continue;
        }

        if ((soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        TRILL__TREE_IDf) != tree_id) ||
           (soc_MPLS_ENTRYm_field32_get(unit, &ment,
                                        TRILL__RBRIDGE_NICKNAMEf) != name)) {
            continue;
        }

        /* Entry found, for a given RBridge Name + Tree ID Key
         * there can be only one entry. Hence stop when entry found.
         */
        break;
    }

    /* Set L3MC index to 0, reserved entry with BITMAP = 0 */
    soc_MPLS_ENTRYm_field32_set(unit, &ment, TRILL__L3MC_INDEXf, 0);

    /* Write to  HW */
    BCM_IF_ERROR_RETURN(
        soc_mem_write(unit, MPLS_ENTRYm, MEM_BLOCK_ALL, idx, &ment));

    return BCM_E_NONE;
}

/*
 * Purpose:
 *       Add TRILL multicast entry
 *       root_name = 0: Add Access -> Network entry
 *       root_name > 0: Add Network -> Access entry
 * Parameters:
 *           unit   - Device Number
 *           flags
 *           root_name  - (IN) trill Root_RBridge
 *           vid- (IN) Customer VLAN
 *           c_dmac	- (IN) Customer D-MAC
 *           group  - (IN) Multicast Group ID
 */

int
bcm_td_trill_multicast_add( int unit, uint32 flags,
                                                     bcm_trill_name_t  root_name, 
                                                     bcm_vlan_t vid, 
                                                     bcm_mac_t c_dmac, 
                                                     bcm_multicast_t group)
{
    int l3mc_index=0;
    int rv = BCM_E_UNAVAIL;
    uint8  trill_tree_id=0;
    bcm_mac_t mac_zero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    /* Check Vlan range */
    if (vid >= BCM_VLAN_INVALID) {
         return BCM_E_PARAM;
    }

    /* Check DA */
    if (c_dmac == NULL) {
         return BCM_E_PARAM;
    }

    /* Check Tree-ID */
    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }

    flags |= BCM_TRILL_MULTICAST_STATIC;

    if (flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) {

       l3mc_index = _BCM_MULTICAST_ID_GET(group);
       BCM_TRILL_L3MC_CHECK (unit, l3mc_index);
        if (vid != BCM_VLAN_NONE){
             /* TRILL_Network Domain entry */
             BCM_IF_ERROR_RETURN(
                        _bcm_esw_trill_multicast_entry_add(unit, flags,
                                               TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS, 
                                               vid, c_dmac, trill_tree_id, group));

              /* Access-domain Entry - Update */
             BCM_IF_ERROR_RETURN(
                        _bcm_esw_trill_multicast_entry_update(unit, TR_L2_HASH_KEY_TYPE_BRIDGE, 
                                               vid, c_dmac, trill_tree_id, group));
         }

        /* Set EGR_IPMC.trill_tree_id */
        rv = bcm_td_multicast_trill_group_update(unit, l3mc_index, trill_tree_id);
    } else {

        /* TRILL-Network Domain - Unpruned entry */
        if ((vid == BCM_VLAN_NONE) && (!sal_memcmp(c_dmac, mac_zero, sizeof(bcm_mac_t)))) {

              l3mc_index = _BCM_MULTICAST_ID_GET(group);
              BCM_TRILL_L3MC_CHECK (unit, l3mc_index);

              rv = _bcm_esw_trill_multicast_transit_entry_set(unit, flags,
                                                     root_name, l3mc_index, trill_tree_id);

        } else if ((!sal_memcmp(c_dmac, mac_zero, sizeof(bcm_mac_t))) && (vid != BCM_VLAN_NONE)) {

              l3mc_index = _BCM_MULTICAST_ID_GET(group);
              BCM_TRILL_L3MC_CHECK (unit, l3mc_index);

              rv = _bcm_esw_trill_multicast_enable(unit, flags, root_name);
              BCM_IF_ERROR_RETURN (rv);

              /* TRILL-Network Domain - Pruned entry - Short */
              rv = _bcm_esw_trill_multicast_entry_add(unit, flags,
                                                 TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT, 
                                                 vid, c_dmac, trill_tree_id, group);
        } else if (vid != BCM_VLAN_NONE) {

              l3mc_index = _BCM_MULTICAST_ID_GET(group);
              BCM_TRILL_L3MC_CHECK (unit, l3mc_index);

              rv = _bcm_esw_trill_multicast_enable(unit, flags, root_name);
              BCM_IF_ERROR_RETURN (rv);

              /* TRILL-Network Domain - Pruned entry - Long */
              rv = _bcm_esw_trill_multicast_entry_add(unit, flags,
                                                 TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG,
                                                 vid, c_dmac, trill_tree_id, group);
        }
    }
    return rv;
}

/*
 * Purpose:
 *      Delete TRILL multicast entry
 * Parameters:
 *           unit   - Device Number
 *           flags
 *           root_name  - (IN) trill Root_RBridge
 *           vid- (IN) Customer VLAN
 *           c_dmac	  (IN) Customer D-MAC
 *           access_domain_group  - (IN) Multicast Group ID
 *           trill_domain_group  - (IN) Multicast Group ID
 */

int
bcm_td_trill_multicast_delete(int unit, uint32 flags,
                                                         bcm_trill_name_t root_name,
                                                         bcm_vlan_t vid,
                                                         bcm_mac_t c_dmac)
{
    int rv = BCM_E_NONE;
    uint8  trill_tree_id=0;
    bcm_mac_t mac_zero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    /* Check Vlan range */
    if (vid >= BCM_VLAN_INVALID) {
         return BCM_E_PARAM;
    }

    /* Check DA */
    if (c_dmac == NULL) {
         return BCM_E_PARAM;
    }

    /* Check Tree-ID */
    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }

    if (flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) {

         if (vid != BCM_VLAN_NONE){
             /* TRILL_Network Domain entry */
             rv = _bcm_esw_trill_multicast_entry_delete(unit, 
                                            TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS, 
                                            vid, c_dmac, trill_tree_id);
         }
    } else {
         /* TRILL-Network Domain - Unpruned entry */
         if ((vid == BCM_VLAN_NONE) &&  (!sal_memcmp(c_dmac, mac_zero, sizeof(bcm_mac_t)))) {       
                rv = _bcm_esw_trill_multicast_transit_entry_reset(unit, 
                                       root_name);
         } else if ((!sal_memcmp(c_dmac, mac_zero, sizeof(bcm_mac_t))) && (vid != BCM_VLAN_NONE)) {
              BCM_IF_ERROR_RETURN (rv);
              /* TRILL-Network Domain - Pruned entry - Short */
              rv = _bcm_esw_trill_multicast_entry_delete(unit, 
                                                 TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT, 
                                                 vid, c_dmac, trill_tree_id);
         } else if (vid != BCM_VLAN_NONE) {
              BCM_IF_ERROR_RETURN (rv);
              /* TRILL-Network Domain - Pruned entry - Long */
              rv = _bcm_esw_trill_multicast_entry_delete(unit, 
                                                 TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG,
                                                 vid, c_dmac, trill_tree_id);
         }  
    }
    return rv;
}

/*
 * Purpose:
 *      Delete all TRILL multicast entries
 * Parameters:
 *		unit   - Device Number
 */

int
bcm_td_trill_multicast_delete_all(int unit, bcm_trill_name_t  root_name)
{
    int rv=BCM_E_UNAVAIL;
    uint8  trill_tree_id=0;

   (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
         _bcm_esw_trill_multicast_transit_entry_reset_all (unit, trill_tree_id));

   rv = _bcm_esw_trill_multicast_entry_delete_all(unit, trill_tree_id);

   return rv;
}

/*
 * Purpose:
 *      Get TRILL multicast entry
 * Parameters:
 *           unit   - Device Number
 *    trill_mc_entry - Trill Multicast Entry
 */

int
bcm_td_trill_multicast_entry_get( int unit, 
    bcm_trill_multicast_entry_t *trill_mc_entry)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv = BCM_E_NONE;
    uint8  trill_tree_id=0;
    bcm_mac_t mac_zero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    int l3mc_index=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }
    trill_info = TRILL_INFO(unit);

    /* Check Vlan range */
    if (trill_mc_entry->c_vlan >= BCM_VLAN_INVALID) {
         return BCM_E_PARAM;
    }

    /* Check DA */
    /* coverity[array_null] */
    if (trill_mc_entry->c_dmac == NULL) {
         return BCM_E_PARAM;
    }

    trill_mc_entry->flags  |= BCM_TRILL_MULTICAST_STATIC;

    if (trill_mc_entry->flags & BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK) {

        if (trill_mc_entry->c_vlan != BCM_VLAN_NONE){
             rv = _bcm_esw_trill_multicast_entry_get(unit, TR_L2_HASH_KEY_TYPE_TRILL_NONUC_ACCESS, 
                              trill_mc_entry->c_vlan, trill_mc_entry->c_dmac, 
                              trill_tree_id, &trill_mc_entry->group);
            l3mc_index = _BCM_MULTICAST_ID_GET(trill_mc_entry->group);
            BCM_TRILL_L3MC_CHECK (unit, l3mc_index);
            rv = bcm_td_multicast_trill_group_get(unit, l3mc_index, &trill_tree_id);
            trill_mc_entry->root_name = trill_info->rootBridge[trill_tree_id];
        } else if (trill_mc_entry->c_vlan == BCM_VLAN_NONE) {
            /* Get EGR_IPMC.trill_tree_id */
            l3mc_index = _BCM_MULTICAST_ID_GET(trill_mc_entry->group);
            BCM_TRILL_L3MC_CHECK (unit, l3mc_index);
            rv = bcm_td_multicast_trill_group_get(unit, l3mc_index, &trill_tree_id);
            trill_mc_entry->root_name = trill_info->rootBridge[trill_tree_id];
        }
    } else {
         /* Check Tree-ID */
         (void) bcm_td_trill_tree_profile_get (unit, trill_mc_entry->root_name, &trill_tree_id);
         if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
               return BCM_E_PARAM;
         }

         if ((trill_mc_entry->c_vlan == BCM_VLAN_NONE) &&  
                     (!sal_memcmp(trill_mc_entry->c_dmac, mac_zero, sizeof(bcm_mac_t)))) {   
                     rv = _bcm_esw_trill_multicast_transit_entry_get(unit, 
                              trill_mc_entry->root_name, &trill_mc_entry->group);
         } else if ((!sal_memcmp(trill_mc_entry->c_dmac, mac_zero, sizeof(bcm_mac_t))) && 
                  (trill_mc_entry->c_vlan != BCM_VLAN_NONE)) {
              /* TRILL-Network Domain - Unpruned entry */
              rv = _bcm_esw_trill_multicast_entry_get(unit, TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_SHORT, 
                              trill_mc_entry->c_vlan, trill_mc_entry->c_dmac, 
                              trill_tree_id, &trill_mc_entry->group);
         } else if (trill_mc_entry->c_vlan != BCM_VLAN_NONE) {
              /* TRILL-Network Domain - Pruned entry - Long */
              rv = _bcm_esw_trill_multicast_entry_get(unit, TR_L2_HASH_KEY_TYPE_TRILL_NONUC_NETWORK_LONG, 
                              trill_mc_entry->c_vlan, trill_mc_entry->c_dmac, 
                              trill_tree_id, &trill_mc_entry->group);
         }
    }
    return rv;
}

/*
 * Purpose:
 *      Verify Trill Multicast group
 * Parameters:
 *      unit         - (IN) Unit number.
 *      l3mc_group        - (IN) Multicast Group
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_trill_multicast_check(int unit, bcm_multicast_t l3mc_group)
{
    _bcm_td_trill_bookkeeping_t *trill_info = TRILL_INFO(unit);
    int rv=BCM_E_UNAVAIL;
    int l3mc_index=0;

    if (_BCM_MULTICAST_IS_TRILL(l3mc_group)) {
          l3mc_index = _BCM_MULTICAST_ID_GET(l3mc_group);
          if (trill_info->multicast_count[l3mc_index].network_port_count) {
              BCM_TRILL_L3MC_CHECK (unit, l3mc_index);  
              return BCM_E_NONE;
          }
    }
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
bcm_td_trill_unpruned_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx=0, ent_idx=0, chnk_idx_max=0, mem_idx_max=0;
    int             buf_size=0, chunksize=0, chnk_end=0;
    uint32          *unpruned_trill_tbl_chnk;
    mpls_entry_entry_t *ment;
    int             rv = BCM_E_NONE;
    soc_mem_t     mem;
    int    key_type=0;
    uint8   tree_id=0xff;
    bcm_trill_name_t  root_name=0;
    bcm_trill_multicast_entry_t  trill_mc;

    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    mem = MPLS_ENTRYm;

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
            ment = soc_mem_table_idx_to_pointer(unit, mem, mpls_entry_entry_t *, 
                                             unpruned_trill_tbl_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, ment, VALIDf)) {
                continue;
            }
          
            /* Match the Key Type to TRILL */
            key_type = soc_mem_field32_get(unit, mem, ment, KEY_TYPEf);

            if (key_type != 0x5) { /* RBRIDGE_NICKNAME */
                continue;
            }

            /* Skip entries that discard multicast destinations */
            if (soc_mem_field_valid(unit, MPLS_ENTRYm, 
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
 *      Traverse  TRILL Access Default Multicast entries
 * Parameters:
 *     unit             - device number
 *     trav_st        - Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
int
bcm_td_trill_access_default_multicast_entry_traverse (int unit, _bcm_td_trill_multicast_entry_traverse_t *trav_st)
{
   /* Indexes to iterate over memories, chunks and entries */
    int             chnk_idx=0, ent_idx=0, chnk_idx_max=0, mem_idx_max=0;
    int             buf_size=0, chunksize=0, chnk_end=0;
    uint32          *access_trill_vlan_chnk;
    vlan_tab_entry_t   *vlant;
    int             rv = BCM_E_NONE;
    soc_mem_t     mem;
    int    bc_f=0, uuc_f=0, umc_f=0;
    bcm_trill_multicast_entry_t  trill_mc;

    sal_memset (&trill_mc, 0, sizeof (bcm_trill_multicast_entry_t));

    mem = VLAN_TABm;

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    chunksize = 1024;

    buf_size = 4 * SOC_MAX_MEM_FIELD_WORDS * chunksize;
    access_trill_vlan_chnk = soc_cm_salloc(unit, buf_size, "trill access default multicast traverse");
    if (NULL == access_trill_vlan_chnk) {
        return BCM_E_MEMORY;
    }

    mem_idx_max = soc_mem_index_max(unit, mem);
    for (chnk_idx = soc_mem_index_min(unit, mem); 
         chnk_idx <= mem_idx_max; chnk_idx += chunksize) {
        sal_memset((void *)access_trill_vlan_chnk, 0, buf_size);

        chnk_idx_max = 
            ((chnk_idx + chunksize) < mem_idx_max) ? 
            (chnk_idx + chunksize - 1) : mem_idx_max;

        rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                chnk_idx, chnk_idx_max, access_trill_vlan_chnk);
        if (SOC_FAILURE(rv)) {
            break;
        }
        chnk_end = (chnk_idx_max - chnk_idx);
        for (ent_idx = 0 ; ent_idx <= chnk_end; ent_idx ++) {
            vlant = soc_mem_table_idx_to_pointer(unit, mem, vlan_tab_entry_t *, 
                                             access_trill_vlan_chnk, ent_idx);

            if (!soc_mem_field32_get(unit, mem, vlant, VALIDf)) {
                continue;
            }
          
           if (SOC_MEM_FIELD_VALID(unit, mem, 
                BC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                     bc_f = soc_mem_field32_get(unit, mem, vlant,
                               BC_TRILL_NETWORK_RECEIVERS_PRESENTf);
           }
           if (SOC_MEM_FIELD_VALID(unit, mem, 
                UMC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                     umc_f = soc_mem_field32_get(unit, VLAN_TABm, vlant,
                               UMC_TRILL_NETWORK_RECEIVERS_PRESENTf);
           }
           if (SOC_MEM_FIELD_VALID(unit, mem, 
                UUC_TRILL_NETWORK_RECEIVERS_PRESENTf)) {
                     uuc_f = soc_mem_field32_get(unit, mem, vlant,
                               UUC_TRILL_NETWORK_RECEIVERS_PRESENTf);
           }

            if ((!bc_f) && (!uuc_f) && (!umc_f) ) {
                continue;
            }

            _BCM_MULTICAST_GROUP_SET(trill_mc.group, 
                              _BCM_MULTICAST_TYPE_TRILL,  
                              soc_mem_field32_get (unit, mem, vlant,
                                TRILL_DOMAIN_NONUC_REPL_INDEXf));

            trill_mc.flags = BCM_TRILL_MULTICAST_ACCESS_TO_NETWORK;
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
    soc_cm_sfree(unit, access_trill_vlan_chnk);
    return rv;        

}

/*
 * Purpose:
 *      Traverse all valid TRILL Multicast entries and call the
 * 	 supplied callback routine.
 * Parameters:
 *     unit   - Device Number
 *     cb  - User callback function, called once per Trill Multicast entry.
 *     user_data - cookie
 */

int
bcm_td_trill_multicast_traverse(int unit, bcm_trill_multicast_entry_traverse_cb  cb,
                                                        void *user_data)
{
    _bcm_td_trill_multicast_entry_traverse_t  trav_st;
    int rv = BCM_E_NONE;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }
     /* If no callback provided, we are done.  */ 
     if (NULL == cb) { 
          return BCM_E_NONE; 
     } 

    sal_memset(&trav_st, 0, sizeof(_bcm_td_trill_multicast_entry_traverse_t));
    
    trav_st.user_cb = cb;
    trav_st.user_data = user_data;

    /* Traverse Unpruned Trill Multicast entry */
    rv = _bcm_esw_trill_unpruned_entry_traverse(unit, &trav_st);
    if (BCM_SUCCESS(rv)) {
       /* Traverse Pruned Trill Network Multicast entry */
       rv = _bcm_esw_trill_network_multicast_entry_traverse(unit, &trav_st);
       if (BCM_SUCCESS(rv)) {
           /* Traverse Trill Access Multicast entry */
           rv = _bcm_esw_trill_access_multicast_entry_traverse(unit, &trav_st);
           if (BCM_SUCCESS(rv)) {
               /* Traverse Trill default Multicast entry */
               rv = bcm_td_trill_access_default_multicast_entry_traverse(unit, &trav_st);
           }
       }
    }
    return rv;
}


/*
 * Purpose:
 *      Add C-VLAN multicast_idx when enabled for TRILL
 * Parameters:
 *      unit         - (IN) Unit number.
 *      vlan         - (IN) C-VLAN
 *      vtab         - (IN/OUT) Vlan Table entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_trill_vlan_multicast_group_add(int unit, bcm_vlan_t vlan, vlan_tab_entry_t   *vtab)
{
    bcm_pbmp_t vlan_pbmp, vlan_ubmp, pbmp, l3_pbmp;
    soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
    int i=0, mc_idx=0;
    int rv = BCM_E_NONE;
    bcm_multicast_t mc_grp;

    /* Enable VP switching on the VLAN */
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
       if (!soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                                     TRILL_ACCESS_RECEIVERS_PRESENTf)) {
            soc_mem_field32_set(unit, VLAN_TABm, vtab, 
                       TRILL_ACCESS_RECEIVERS_PRESENTf, 1);

            rv = bcm_td_l2_trill_vlan_multicast_entry_modify (unit, vlan, 0x1);
            if (rv < 0) {
                return rv;
            } 

            if (SHR_BITGET(vlan_info[unit].vp_mode, vlan)) {
                return rv;
            }

            soc_mem_field32_set(unit, VLAN_TABm, vtab, VIRTUAL_PORT_ENf, 1);
            
            /* Also need to copy the physical port members to the L2_BITMAP of
             * the IPMC entry for each group once we've gone virtual */
            rv = mbcm_driver[unit]->mbcm_vlan_port_get
                (unit, vlan, &vlan_pbmp, &vlan_ubmp, NULL);
                                                   
            if (rv < 0) {
                return rv;
            }

            /* Deal with each group */
            for (i = 0; i < 3; i++) {
                BCM_PBMP_CLEAR(l3_pbmp);
                mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, vtab, group_type[i]);
                if (BCM_FAILURE(_bcm_tr_multicast_ipmc_group_type_get(unit,
                                                        mc_idx, &mc_grp))) {
                    continue;
                }
                if ((mc_idx >= 0) && 
                    (mc_idx < soc_mem_index_count(unit, L3_IPMCm))) {
                    rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &pbmp, 
                                                 &l3_pbmp);
                    if (rv < 0) {
                        return rv;
                    }
                    rv = _bcm_esw_multicast_ipmc_write(unit, mc_idx, vlan_pbmp, 
                                                  l3_pbmp, TRUE);
                    if (rv < 0) {
                        return rv;
                    }
                }
            }
        }
    }
    return rv;
}

/*
 * Purpose:
 *      Delete C-VLAN multicast_idx when enabled for TRILL
 * Parameters:
 *      unit         - (IN) Unit number.
 *      vlan         - (IN) C-VLAN
 *      vtab         - (IN/OUT) Vlan Table entry
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_trill_vlan_multicast_group_delete(int unit, bcm_vlan_t vlan, vlan_tab_entry_t *vtab)
{
    bcm_pbmp_t vlan_pbmp, pbmp, l3_pbmp;
    soc_field_t group_type[3] = {BC_IDXf, UMC_IDXf, UUC_IDXf};
    int i=0, mc_idx=0;
    int rv = BCM_E_NONE;
    bcm_multicast_t mc_grp;

    /* Enable VP switching on the VLAN */
    if (SOC_MEM_FIELD_VALID(unit, VLAN_TABm, VIRTUAL_PORT_ENf)) {
        if (soc_mem_field32_get(unit, VLAN_TABm, vtab, 
                            TRILL_ACCESS_RECEIVERS_PRESENTf)) {
            soc_mem_field32_set(unit, VLAN_TABm, vtab, 
                            TRILL_ACCESS_RECEIVERS_PRESENTf, 0);

            rv = bcm_td_l2_trill_vlan_multicast_entry_modify(unit, vlan, 0x0);
            if (rv < 0) {
                return rv;
            }

            if (SHR_BITGET(vlan_info[unit].vp_mode, vlan)) {
                return rv;
            }   

            soc_mem_field32_set(unit, VLAN_TABm, vtab, VIRTUAL_PORT_ENf, 0);

            /* Deal with each group */
            for (i = 0; i < 3; i++) {
                BCM_PBMP_CLEAR(pbmp);
                mc_idx = _bcm_xgs3_vlan_mcast_idx_get(unit, vtab, group_type[i]);
                if (BCM_FAILURE(_bcm_tr_multicast_ipmc_group_type_get(unit,
                                                            mc_idx, &mc_grp))) {
                    continue;
                }

                if ((mc_idx >= 0) && (mc_idx < soc_mem_index_count(unit, L3_IPMCm))) {
                    rv = _bcm_esw_multicast_ipmc_read(unit, mc_idx, &vlan_pbmp, 
                                                 &l3_pbmp);
                    if (rv < 0) {
                       return rv;
                    }
                    rv = _bcm_esw_multicast_ipmc_write(unit, mc_idx, pbmp, 
                                               l3_pbmp, TRUE);
                    if (rv < 0) {
                       return rv;
                    }
                }
            }
        }
    }
    return rv;

}
/*
 * Purpose:
 *      Add TRILL Gports to L2MC
 * Parameters:
 *      unit         - (IN) Unit number.
 *      port         - (IN) Gport.
 *      mc_index        - (IN) Multicast Index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_trill_egress_add(int unit, bcm_multicast_t l3mc_group)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv = BCM_E_NONE;
    int l3mc_index=0;
    
    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);
    l3mc_index = _BCM_MULTICAST_ID_GET(l3mc_group);

    if (trill_info->multicast_count[l3mc_index].network_port_count == 0) {
        trill_info->multicast_count[l3mc_index].l3mc_group = l3mc_group;
        trill_info->multicast_count[l3mc_index].network_port_count++;
    } else if (trill_info->multicast_count[l3mc_index].network_port_count > 0) {
        trill_info->multicast_count[l3mc_index].network_port_count++;
    }
    return rv;
}

/*
 * Purpose:
 *      Delete TRILL Gports from L2MC
 * Parameters:
 *      unit         - (IN) Unit number.
 *      port         - (IN) Gport.
 *      mc_index        - (IN) Multicast Index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_td_trill_egress_delete(int unit, bcm_multicast_t l3mc_group)
{
    _bcm_td_trill_bookkeeping_t *trill_info;
    int rv = BCM_E_NONE;
    int l3mc_index=0;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    trill_info = TRILL_INFO(unit);

    l3mc_index = _BCM_MULTICAST_ID_GET(l3mc_group);
    l3mc_group = trill_info->multicast_count[l3mc_index].l3mc_group;
    trill_info->multicast_count[l3mc_index].network_port_count--;

    return rv;
}

/*
 * Function:
 *      _bcm_td_trill_multicast_source_entry_key_set
 * Purpose:
 *      Set TRILL Multicast RPF entry key
 * Parameters:
 *  IN :  Unit
 *  IN :  root_name
 *  IN :  source_name
 *  IN :  gport to desired-neighbor
 *  IN :  entry pointer
 * Returns:
 *       BCM_E_XXX
 */

STATIC int
 _bcm_td_trill_multicast_source_entry_key_set(int unit, 
                                         bcm_trill_name_t root_name, bcm_trill_name_t source_name, 
                                         bcm_gport_t gport,  bcm_if_t encap_intf, L3_ENTRY_T *l3_key)
 {
    int rv = BCM_E_NONE;
    uint8  trill_tree_id=0;
    bcm_module_t  local_modid=-1;
    bcm_port_t    local_port=-1;
    bcm_trunk_t  local_tgid=BCM_TRUNK_INVALID;
    int local_id=-1;
    soc_mem_t mem;

    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }
    rv = _bcm_esw_gport_resolve(unit, gport, &local_modid,
                                       &local_port, &local_tgid, &local_id);
    if (rv < 0) {
         return SOC_E_UNAVAIL;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
        soc_mem_field32_set(unit, mem, l3_key, KEY_TYPEf,
                            SOC_MEM_KEY_L3_ENTRY_1_TRILL_TRILL);
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
        mem = L3_ENTRY_IPV4_UNICASTm;
        soc_mem_field32_set(unit, mem, l3_key, KEY_TYPEf,
                            TD2_L3_HASH_KEY_TYPE_TRILL);
    } else 
#endif /* BCM_TRIDENT2_SUPPORT */ 
   {
        mem = L3_ENTRY_IPV4_UNICASTm;
        soc_mem_field32_set(unit, mem, l3_key, KEY_TYPEf,
                            TR_L3_HASH_KEY_TYPE_TRILL);

    }

    soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__INGRESS_RBRIDGE_NICKNAMEf, source_name);
    soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__TREE_IDf, trill_tree_id);
    if (BCM_TRUNK_INVALID != local_tgid) {
         soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__EXPECTED_TGIDf, local_tgid);
         soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__EXPECTED_Tf, 0x1);
    } else {
         soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__EXPECTED_MODULE_IDf, local_modid);
         soc_mem_field32_set(unit, mem, l3_key, 
                   TRILL__EXPECTED_PORT_NUMf, local_port);
    }

    soc_mem_field32_set(unit, mem, l3_key, VALIDf, 1);
    return rv;
 }

/*
 * Purpose:
 *       Add TRILL multicast RPF  entry
 * Parameters:
 *      unit  - Device Number
 *      root_name  - (IN) Trill Root_RBridge
 *      source_name - (IN) Source Rbridge
 *      port - (IN) Ingress port
 */

int
bcm_td_trill_multicast_source_add( int unit,
                                                                     bcm_trill_name_t root_name, 
                                                                     bcm_trill_name_t source_name, 
                                                                     bcm_gport_t gport,
                                                                     bcm_if_t   encap_intf)
{
    L3_ENTRY_T l3_key, l3_entry;
    int rv = BCM_E_NONE;
    int l3_entry_index=0;
    soc_mem_t mem;

    sal_memset(&l3_key, 0, sizeof(L3_ENTRY_T));
    sal_memset(&l3_entry, 0, sizeof(L3_ENTRY_T));

    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
    } else {
        mem = L3_ENTRY_IPV4_UNICASTm;
    }

     /* Check for Gport type - Mod Port or TRUNK Port */
     if ( (SOC_GPORT_IS_TRUNK(gport)) ||
           (SOC_GPORT_IS_LOCAL(gport)) || 
           (SOC_GPORT_IS_DEVPORT(gport)) || 
           (SOC_GPORT_IS_MODPORT(gport))) {
         BCM_IF_ERROR_RETURN(
              _bcm_td_trill_multicast_source_entry_key_set(unit, root_name, 
                        source_name, gport, encap_intf, &l3_key));
    } else {
            return BCM_E_PARAM;
    }


    rv = soc_mem_search(unit, mem, 
                        MEM_BLOCK_ANY, &l3_entry_index,
                        &l3_key, &l3_entry, 0);
    if (rv == SOC_E_NONE) {
         rv = soc_mem_write(unit, mem,
                        MEM_BLOCK_ALL, l3_entry_index, &l3_key);
    } else if (rv != SOC_E_NOT_FOUND) {
        return rv;
    } else {
         rv = soc_mem_insert(unit, mem, MEM_BLOCK_ALL, &l3_key);
    }
    return rv;
}

/*
 * Purpose:
 *       Delete TRILL multicast RPF  entry
 * Parameters:
 *             unit  - Device Number
 *             root_name  - (IN) Trill Root_RBridge
 *             source_name - (IN) Source Rbridge
 *             port - (IN) Ingress port
 */

int
bcm_td_trill_multicast_source_delete( int unit,
                                                                         bcm_trill_name_t root_name, 
                                                                         bcm_trill_name_t source_name, 
                                                                         bcm_gport_t gport,
                                                                         bcm_if_t  encap_intf)
{
    L3_ENTRY_T l3_entry;
    int rv = BCM_E_UNAVAIL;
    int l3_entry_index=0;
    soc_mem_t mem;

    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
    } else {
        mem = L3_ENTRY_IPV4_UNICASTm;
    }

    sal_memset(&l3_entry, 0, sizeof(L3_ENTRY_T));
    if ( (SOC_GPORT_IS_TRUNK(gport)) ||
           (SOC_GPORT_IS_LOCAL(gport)) || 
           (SOC_GPORT_IS_DEVPORT(gport)) || 
           (SOC_GPORT_IS_MODPORT(gport))) {
         (void) _bcm_td_trill_multicast_source_entry_key_set(unit, root_name, 
                        source_name, gport, encap_intf, &l3_entry);
    } else {
            return BCM_E_PARAM;
    }

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, &l3_entry_index,
                             &l3_entry, &l3_entry, 0);
    if (rv < 0) {
         return rv;
    }
   
    /* Delete the entry from HW */
    rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, &l3_entry);
    
    if ( (rv != BCM_E_NOT_FOUND) && (rv != BCM_E_NONE) ) {
       return rv;
    } else {
       return BCM_E_NONE;
    }
}

/*
 * Purpose:
 *       Get TRILL multicast RPF  entry
 * Parameters:
 *             unit - Device Number
 *             root_name  - (IN) Trill Root_RBridge
 *             source_name - (IN) Source Rbridge
 *             port  - (OUT) Ingress port
 */

int
bcm_td_trill_multicast_source_get( int unit,
                                                                    bcm_trill_name_t root_name, 
                                                                    bcm_trill_name_t source_name, 
                                                                    bcm_gport_t *gport)
{
    L3_ENTRY_T l3_key, l3_entry;
    int l3_entry_index=0;
    uint8  trill_tree_id=0;
    bcm_module_t  module_id=0, mod_out=0;
    bcm_port_t    port_num=0, port_out=0;
    bcm_trunk_t  tgid=0;
    soc_mem_t mem;

    sal_memset(&l3_key, 0, sizeof(L3_ENTRY_T));

    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
    } else {
        mem = L3_ENTRY_IPV4_UNICASTm;
    }
 
    (void) bcm_td_trill_tree_profile_get (unit, root_name, &trill_tree_id);
    if (trill_tree_id >= BCM_MAX_NUM_TRILL_TREES) {
         return BCM_E_PARAM;
    }

    soc_mem_field32_set(unit, mem, &l3_key, TRILL__INGRESS_RBRIDGE_NICKNAMEf, source_name);

    soc_mem_field32_set(unit, mem, &l3_key, TRILL__TREE_IDf, trill_tree_id);

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
        soc_mem_field32_set(unit, mem, &l3_key, KEY_TYPEf,
                                 TD2_L3_HASH_KEY_TYPE_TRILL);
    } else 
#endif /* BCM_TRIDENT2_SUPPORT */
    {
        soc_mem_field32_set(unit, mem, &l3_key, KEY_TYPEf,
                                 TR_L3_HASH_KEY_TYPE_TRILL);
    }

    BCM_IF_ERROR_RETURN (
    soc_mem_search(unit, mem, MEM_BLOCK_ANY,
                             &l3_entry_index, &l3_key, &l3_entry, 0));

    if (soc_mem_field32_get(unit, mem, &l3_entry, TRILL__EXPECTED_Tf)) {
         tgid = soc_mem_field32_get(unit, mem,
                             &l3_entry, TRILL__EXPECTED_TGIDf);
         SOC_GPORT_TRUNK_SET(*gport, tgid);
    } else {
         module_id = soc_mem_field32_get(unit, mem,
                             &l3_entry, TRILL__EXPECTED_MODULE_IDf);
         port_num = soc_mem_field32_get(unit, mem,
                             &l3_entry, TRILL__EXPECTED_PORT_NUMf);
         BCM_IF_ERROR_RETURN (
                   _bcm_gport_modport_hw2api_map(unit, module_id,
                                                                          port_num, &mod_out,
                                                                          &port_out));
         SOC_GPORT_MODPORT_SET(*gport, mod_out, port_out);
    }
    return BCM_E_NONE;
}

/* Purpose:
*	   Traverse all valid TRILL Multicast RPF entries and call the
*	supplied callback routine.
* Parameters:
*	   unit   - Device Number
*	   cb		 - User callback function, called once per Trill Multicast entry.
*	   user_data - cookie
*/

int
bcm_td_trill_multicast_source_traverse(int unit, bcm_trill_multicast_source_traverse_cb cb,
                                       void *user_data)
{
    uint32 *l3_entry_p;
    uint8  tree_id=0;
    bcm_module_t  module_id=0, mod_out=0;
    bcm_port_t    port_num=0, port_out=0;
    bcm_trunk_t  tgid=0;
    bcm_gport_t gport=0;
    soc_mem_t mem;
    uint32 trill_key_type=0xff;
    bcm_trill_name_t root_name=0;
    bcm_trill_name_t source_name=0;

    int table_ent_size=0;         /* Table entry size. */
    int table_size=0;             /* Number of entries in the table. */
    int idx_max=0;                /* Maximum iteration index.        */
    int idx_min=0;                /* Minimum iteration index.        */
    int idx=0;                    /* Iterator index.                 */
    char *l3_tbl_ptr = NULL;    /* Table dma pointer.              */
    int rv = BCM_E_NONE;        /* Operation return value.         */
    _bcm_td_trill_bookkeeping_t *trill_info;

    trill_info = TRILL_INFO(unit);

    /* If no callback provided, we are done.  */
    if (NULL == cb) {
        return (BCM_E_NONE);
    }

    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
    } else {
        mem = L3_ENTRY_IPV4_UNICASTm;
    }

    /* Get table boundaries. */
    idx_max = soc_mem_index_max(unit, mem);
    idx_min =  soc_mem_index_min(unit, mem);

    /* Get single entry size. */
    table_ent_size = BCM_XGS3_L3_ENT_SZ(unit, v4);

    soc_mem_lock(unit, mem);
    /* Dma the table - to speed up operation. */
    rv = bcm_xgs3_l3_tbl_dma(unit, mem, table_ent_size, 
                              "trill_rpf_tbl", &l3_tbl_ptr, &table_size);
    soc_mem_unlock(unit, mem);

    if (BCM_FAILURE(rv)) {
        if(l3_tbl_ptr) {
            soc_cm_sfree(unit, l3_tbl_ptr);
        }
        return rv; 
    }

    /* Iterate over all the entries - show matching ones. */
    for (idx = idx_min; idx <= idx_max; idx++) {
        l3_entry_p =
            soc_mem_table_idx_to_pointer(unit, mem,
                                         uint32 *,
                                         l3_tbl_ptr, idx);

        /* Check if this is an TRILL RPF entry */
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TOMAHAWKX(unit)) {
            trill_key_type = TD2_L3_HASH_KEY_TYPE_TRILL;
        } else 
#endif /* BCM_TRIDENT2_SUPPORT */
        {
            trill_key_type = TR_L3_HASH_KEY_TYPE_TRILL;
        }
        if (soc_mem_field32_get(unit, mem, l3_entry_p, KEY_TYPEf) != trill_key_type) {
            continue;
        }

        tree_id = soc_mem_field32_get(unit, mem, l3_entry_p, TRILL__TREE_IDf);
        if (!(tree_id < BCM_MAX_NUM_TRILL_TREES)) {
            /* Not a vaild Root tree: entry shouldn't exist */
            continue;
        }

        root_name = trill_info->rootBridge[tree_id];
        source_name = soc_mem_field32_get(unit, mem, l3_entry_p, 
                                          TRILL__INGRESS_RBRIDGE_NICKNAMEf);

        if (soc_mem_field32_get(unit, mem, l3_entry_p, TRILL__EXPECTED_Tf)) {
             tgid = soc_mem_field32_get(unit, mem,
                                 l3_entry_p, TRILL__EXPECTED_TGIDf);
             SOC_GPORT_TRUNK_SET(gport, tgid);
        } else {
             module_id = soc_mem_field32_get(unit, mem,
                                 l3_entry_p, TRILL__EXPECTED_MODULE_IDf);
             port_num = soc_mem_field32_get(unit, mem,
                                 l3_entry_p, TRILL__EXPECTED_PORT_NUMf);
             rv = _bcm_gport_modport_hw2api_map(unit, module_id, port_num, &mod_out,
                                                &port_out);
             if (BCM_FAILURE(rv)) {
                 break;
             }
             SOC_GPORT_MODPORT_SET(gport, mod_out, port_out);
        }

        rv = (*cb) (unit, root_name, source_name, gport, user_data);
#ifdef BCM_CB_ABORT_ON_ERR
        if (BCM_FAILURE(rv) && SOC_CB_ABORT_ON_ERR(unit)) {
            break;
        }
#endif
    }

    if(l3_tbl_ptr) {
        soc_cm_sfree(unit, l3_tbl_ptr);
    }

    /* Reset last read status. */
    if (BCM_E_NOT_FOUND == rv) {
        rv = BCM_E_NONE;
    }

    return (rv);
}

/*
 * Function:
 *      bcm_td_trill_multicast_source_delete_all
 * Purpose:
 *      Delete all RFP entries in L3 Table
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td_trill_multicast_source_delete_all(int unit)
{
    uint32 *l3_entry_p;
    soc_mem_t mem;
    uint32 trill_key_type=0xff;   /* L3 Table KEY_TYPE */

    int table_ent_size=0;         /* Table entry size. */
    int table_size=0;             /* Number of entries in the table. */
    int idx_max=0;                /* Maximum iteration index.        */
    int idx_min=0;                /* Minimum iteration index.        */
    int idx=0;                    /* Iterator index.                 */
    char *l3_tbl_ptr = NULL;      /* Table dma pointer.              */
    int rv = BCM_E_NONE;          /* Operation return value.         */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TRIDENT2X(unit)|| SOC_IS_TOMAHAWKX(unit)) {
        mem = L3_ENTRY_IPV4_UNICASTm;
        trill_key_type = TD2_L3_HASH_KEY_TYPE_TRILL;
    } else
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        mem = L3_ENTRY_ONLY_SINGLEm;
        trill_key_type = TD2_L3_HASH_KEY_TYPE_TRILL;
    } else
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        mem = L3_ENTRY_1m;
        trill_key_type = SOC_MEM_KEY_L3_ENTRY_1_TRILL_TRILL;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        mem = L3_ENTRY_IPV4_UNICASTm;
        trill_key_type = TR_L3_HASH_KEY_TYPE_TRILL;
    }

    /* Get table boundaries. */
    idx_max = soc_mem_index_max(unit, mem);
    idx_min =  soc_mem_index_min(unit, mem);

    /* Get single entry size. */
    table_ent_size = BCM_XGS3_L3_ENT_SZ(unit, v4);

    soc_mem_lock(unit, mem);
    /* Dma the table - to speed up operation. */
    rv = bcm_xgs3_l3_tbl_dma(unit, mem, table_ent_size,
                              "trill_rpf_tbl", &l3_tbl_ptr, &table_size);

    if (BCM_FAILURE(rv)) {
        if(l3_tbl_ptr) {
            soc_cm_sfree(unit, l3_tbl_ptr);
        }

        soc_mem_unlock(unit, mem);
        return rv;
    }

    /* Iterate over all the entries - show matching ones. */
    for (idx = idx_min; idx <= idx_max; idx++) {
        l3_entry_p =
            soc_mem_table_idx_to_pointer(unit, mem,
                                         uint32 *,
                                         l3_tbl_ptr, idx);


        /* Skip entry if its not a Trill RFP entry */
        if (soc_mem_field32_get(unit, mem, l3_entry_p, KEY_TYPEf) != trill_key_type) {
            continue;
        }

        rv = soc_mem_delete(unit, mem, MEM_BLOCK_ALL, l3_entry_p);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    /* Free resources */
    if(l3_tbl_ptr) {
        soc_cm_sfree(unit, l3_tbl_ptr);
    }

    soc_mem_unlock(unit, mem);
    return (rv);
}


/*
 * Function:
 *      bcm_tr_trill_stat_get
 * Purpose:
 *      Get TRILL Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port   - (IN) Physical port
 *      stat   - (IN)  specify the Stat type
 *      val    - (OUT) 64-bit Stats value
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_td_trill_stat_get(int unit, bcm_port_t port,
                           bcm_trill_stat_t stat, uint64 *val)
{
    int rv=BCM_E_NONE;
    uint32 reg_val=0;
    uint32  val_low=0;
    trill_drop_stats_entry_t  drop;
    bcm_port_t    local_port=-1;
    int local_id=-1;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
         if ( (SOC_GPORT_IS_LOCAL(port)) || 
               (SOC_GPORT_IS_DEVPORT(port)) || 
               (SOC_GPORT_IS_MODPORT(port))) {
                   BCM_IF_ERROR_RETURN
                      (bcm_esw_port_local_get(unit, port, &local_port));
         } else if (SOC_GPORT_IS_TRILL_PORT(port)) {
              /* Flex stats not supported for TRILL port */
              return BCM_E_UNAVAIL;
         } else {
              return BCM_E_PARAM;
         }
   } else if (SOC_GPORT_INVALID == port) {
         local_id = 0;
   } else {
        local_port = port;
   }

    COMPILER_64_ZERO(*val);

    switch (stat) {

         case bcmTrillInPkts:
           if (local_port != -1) {
               if (SOC_REG_IS_VALID(unit, ING_TRILL_RX_PKTSr)) {
                   SOC_IF_ERROR_RETURN
                       (READ_ING_TRILL_RX_PKTSr(unit, local_port, &reg_val));
                   val_low = 
                       soc_reg_field_get(unit, ING_TRILL_RX_PKTSr, 
                                         reg_val, COUNTf);
                   COMPILER_64_SET(*val, 0, val_low);
               } else {
                   uint64 rval64;

                   SOC_IF_ERROR_RETURN
                       (READ_ING_TRILL_RX_PKTS_64r(unit, local_port, &rval64));
                   *val = soc_reg64_field_get(unit, ING_TRILL_RX_PKTS_64r,
                                              rval64, COUNTf);       
               }
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillOutPkts:
           if (local_port != -1) {
               if (SOC_REG_IS_VALID(unit, EGR_TRILL_TX_PKTSr)) {
                   SOC_IF_ERROR_RETURN
                       (READ_EGR_TRILL_TX_PKTSr(unit, local_port, &reg_val));
                   val_low =
                       soc_reg_field_get(unit, EGR_TRILL_TX_PKTSr, 
                                         reg_val, COUNTf);
                   COMPILER_64_SET(*val, 0, val_low);
               } else {
                   uint64 rval64;

                   SOC_IF_ERROR_RETURN
                       (READ_EGR_TRILL_TX_PKTS_64r(unit, local_port, &rval64));
                   *val = soc_reg64_field_get(unit, EGR_TRILL_TX_PKTS_64r,
                                              rval64, COUNTf);       
               }
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillErrorPkts:
           if (local_id == 0) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, TRILL_DROP_STATSm,
                        MEM_BLOCK_ALL, local_id, &drop));
              soc_mem_field64_get(unit, TRILL_DROP_STATSm, 
                                  &drop, TRILL_ERROR_DROPSf, val);    
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillNameMissPkts:             
           if (local_id == 0) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, TRILL_DROP_STATSm,
                        MEM_BLOCK_ALL, local_id, &drop));             
              soc_mem_field64_get(unit, TRILL_DROP_STATSm, &drop, 
                                  TRILL_RBRIDGE_LOOKUP_MISS_DROPSf, val);             
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillRpfFailPkts:
           if (local_id == 0) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, TRILL_DROP_STATSm,
                        MEM_BLOCK_ALL, local_id, &drop));
              soc_mem_field64_get(unit, TRILL_DROP_STATSm, &drop, 
                                  TRILL_RPF_CHECK_FAIL_DROPSf, val);             
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillTtlFailPkts:            
           if (local_id == 0) {
              BCM_IF_ERROR_RETURN(
                   soc_mem_read(unit, TRILL_DROP_STATSm,
                        MEM_BLOCK_ALL, local_id, &drop));
              soc_mem_field64_get(unit, TRILL_DROP_STATSm, &drop, 
                                  TRILL_HOPCOUNT_CHECK_FAIL_DROPSf, val);               
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         default:
                   break;

    }

    return rv;
}

int
bcm_td_trill_stat_get32(int unit, bcm_port_t port,
                             bcm_trill_stat_t stat, uint32 *val)
{
    int rv=BCM_E_NONE;
    uint64 val64;

    rv = bcm_td_trill_stat_get(unit, port, stat, &val64);
    if (rv == BCM_E_NONE) {
        *val = COMPILER_64_LO(val64);
    }
    return rv;
}

/*
 * Function:
 *      bcm_td_trill_stat_clear
 * Purpose:
 *      Clear TRILL Stats
 * Parameters:
 *      unit   - (IN) SOC unit #
 *      port   - (IN) Physcial port
 *      stat   - (IN)  specify the Stat type
 * Returns:
 *      BCM_E_XXX
 */     

int
bcm_td_trill_stat_clear(int unit, bcm_port_t port,
                             bcm_trill_stat_t stat)
{
    int rv=BCM_E_NONE;
    uint32 reg_val=0;
    trill_drop_stats_entry_t  drop;
    bcm_port_t    local_port=-1;
    int local_id=-1;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
         if ( (SOC_GPORT_IS_LOCAL(port)) || 
               (SOC_GPORT_IS_DEVPORT(port)) || 
               (SOC_GPORT_IS_MODPORT(port))) {
                   BCM_IF_ERROR_RETURN
                      (bcm_esw_port_local_get(unit, port, &local_port));
         } else if (SOC_GPORT_IS_TRILL_PORT(port)) {
              /* Flex stats not supported for TRILL port */
              return BCM_E_UNAVAIL;
         } else {
              return BCM_E_PARAM;
         }
   } else if (SOC_GPORT_INVALID == port) {
         local_id = 0;

         /* Global stats , initialize and read entry */
         sal_memset(&drop, 0, sizeof(trill_drop_stats_entry_t));
         BCM_IF_ERROR_RETURN(
             soc_mem_read(unit, TRILL_DROP_STATSm, MEM_BLOCK_ALL, 
                  0, &drop));
   } else {
        local_port = port;
   }

    switch (stat) {

         case bcmTrillInPkts:

           if (local_port != -1) {
               if (SOC_REG_IS_VALID(unit, ING_TRILL_RX_PKTSr)) {
                   soc_reg_field_set(unit, ING_TRILL_RX_PKTSr, 
                                                   &reg_val, COUNTf, 0);
                   SOC_IF_ERROR_RETURN
                     (WRITE_ING_TRILL_RX_PKTSr(unit, local_port, reg_val));
               } else {
                   uint64 cntval;
                   uint64 rval64;      /* Current 64 bit register data.  */

                   COMPILER_64_ZERO(cntval);
                   COMPILER_64_ZERO(rval64);

                   soc_reg64_field_set(unit, ING_TRILL_RX_PKTS_64r, 
                                             &rval64, COUNTf, cntval);
                   SOC_IF_ERROR_RETURN
                     (WRITE_ING_TRILL_RX_PKTS_64r(unit, local_port, rval64));
               }
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillOutPkts:

           if (local_port != -1) {
               if (SOC_REG_IS_VALID(unit, EGR_TRILL_TX_PKTSr)) {
                   soc_reg_field_set(unit, EGR_TRILL_TX_PKTSr, 
                                                   &reg_val, COUNTf, 0);
                   SOC_IF_ERROR_RETURN
                       (WRITE_EGR_TRILL_TX_PKTSr(unit, local_port, reg_val));
               } else {
                   uint64 cntval;
                   uint64 rval64;      /* Current 64 bit register data.  */

                   COMPILER_64_ZERO(cntval);
                   COMPILER_64_ZERO(rval64);

                   soc_reg64_field_set(unit, EGR_TRILL_TX_PKTS_64r, 
                                             &rval64, COUNTf, cntval);
                   SOC_IF_ERROR_RETURN
                     (WRITE_EGR_TRILL_TX_PKTS_64r(unit, local_port, rval64));
               }
           } else {
              return BCM_E_UNAVAIL;
           }
           break;

         case bcmTrillErrorPkts:

            if (local_id == 0) {
                uint64 mem_val;
                COMPILER_64_ZERO(mem_val);
                soc_mem_field64_set(unit, TRILL_DROP_STATSm, &drop,
                                    TRILL_ERROR_DROPSf, mem_val);

                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, TRILL_DROP_STATSm,
                                   MEM_BLOCK_ALL, local_id, &drop));
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

         case bcmTrillNameMissPkts:
              
            if (local_id == 0) {  
                uint64 mem_val;
                COMPILER_64_ZERO(mem_val);
                soc_mem_field64_set(unit, TRILL_DROP_STATSm, &drop,
                                    TRILL_RBRIDGE_LOOKUP_MISS_DROPSf, mem_val);
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, TRILL_DROP_STATSm,
                                   MEM_BLOCK_ALL, local_id, &drop));
            } else {
              return BCM_E_UNAVAIL;
            }
            break;

         case bcmTrillRpfFailPkts:
              
            if (local_id == 0) {
                uint64 mem_val;
                COMPILER_64_ZERO(mem_val);
                soc_mem_field64_set(unit, TRILL_DROP_STATSm, &drop,
                                    TRILL_RPF_CHECK_FAIL_DROPSf, mem_val);                
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, TRILL_DROP_STATSm,
                                   MEM_BLOCK_ALL, local_id, &drop));
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

         case bcmTrillTtlFailPkts:
              
            if (local_id == 0) {
                uint64 mem_val;
                COMPILER_64_ZERO(mem_val);
                soc_mem_field64_set(unit, TRILL_DROP_STATSm, &drop,
                                    TRILL_HOPCOUNT_CHECK_FAIL_DROPSf, mem_val);
                BCM_IF_ERROR_RETURN
                    (soc_mem_write(unit, TRILL_DROP_STATSm,
                                   MEM_BLOCK_ALL, local_id, &drop));
            } else {
                return BCM_E_UNAVAIL;
            }
            break;

         default:
                   break;

    }

    return rv;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_td_trill_sw_dump
 * Purpose:
 *     Displays Trill State information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_td_trill_sw_dump(int unit)
{
    int idx=0, num_vp=0, num_ipmc=0;
    _bcm_td_trill_bookkeeping_t *trill_info = TRILL_INFO(unit);
    
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_ipmc = soc_mem_index_count(unit, L3_IPMCm);

    LOG_CLI((BSL_META_U(unit,
                        "\nRoot Bridges:\n")));
    for (idx = 0; idx < BCM_MAX_NUM_TRILL_TREES; idx++) {
        LOG_CLI((BSL_META_U(unit,
                            "%d "), trill_info->rootBridge[idx]));
    }
    
    LOG_CLI((BSL_META_U(unit,
                        "\n\nrBridges:\n")));
    for (idx = 0; idx < num_vp; idx++) {
        if (trill_info->rBridge[idx] != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "Index:%d rBridge nickname:%d\n"), idx,
                     trill_info->rBridge[idx]));
        }
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\nMulticast use count:\n")));
    for (idx = 0; idx < num_ipmc; idx++) {
        if (trill_info->multicast_count[idx].network_port_count != 0) {
            LOG_CLI((BSL_META_U(unit,
                                "    Multicast group %d use count:%d\n"),
                     trill_info->multicast_count[idx].l3mc_group,
                     trill_info->multicast_count[idx].network_port_count));
        }
    }
    
    return;
}

#endif /* BCM_SW_STATE_DUMP_DISABLE */

#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

