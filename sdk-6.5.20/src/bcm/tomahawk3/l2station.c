/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Tomahawk3 L2 Station API internal functions
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <assert.h>
#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/triumph.h>
#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/switch.h>
#ifdef BCM_TOMAHAWK3_SUPPORT
#include <soc/tomahawk3.h>
#include <bcm_int/esw/tomahawk3.h>

#define GPORT_TYPE(_gport) (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

#define _BCM_L2_STATION_ID_INVALID (-1)
#define _BCM_L2_STATION_ID_BASE (1)
#define _BCM_L2_STATION_ID_MAX  (0x1000000)

/* To differentiate between overlay and underlay tcams */
#define _BCM_L2_STATION_ENTRY_UNDERLAY (0)
#define _BCM_L2_STATION_ENTRY_OVERLAY  (1)
#define _BCM_L2_STATION_ENTRY_LEGACY   _BCM_L2_STATION_ENTRY_UNDERLAY

#define _BCM_L2_STATION_ENTRY_INSTALLED      (1 << 0)

#define _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE (1 << 1)

#define _BCM_L2_STATION_ENTRY_TYPE_NON_TCAM  (1 << 2)

#define _BCM_L2_STATION_TERM_FLAGS_MASK    (BCM_L2_STATION_IPV4       \
                                            | BCM_L2_STATION_IPV6     \
                                            | BCM_L2_STATION_ARP_RARP \
                                            | BCM_L2_STATION_MPLS)

/*
 * Macro:
 *     ParamMax
 * Purpose:
 *     Determine maximum value that can be stored in the field.
 */
#define ParamMax(_unit_, _mem_, _field_)                                      \
            ((soc_mem_field_length((_unit_), (_mem_) , (_field_)) < 32) ?     \
            ((1 << soc_mem_field_length((_unit_), (_mem_), (_field_))) - 1) : \
            (0xFFFFFFFFUL))                             
/*
 * Macro:
 *     ParamCheck
 * Purpose:
 *     Check if value can fit in the field and return error if it exceeds
 *     the range. 
 */
#define ParamCheck(_unit_, _mem_, _field_, _value_)                          \
        do {                                                                 \
            if (0 == ((uint32)(_value_) <=                                   \
                (uint32)ParamMax((_unit_), (_mem_), (_field_)))) {           \
                    LOG_ERROR(BSL_LS_BCM_L2, \
                              (BSL_META("L2(unit %d) Error: _value_ %d > %d (max)" \
                               " mem (%d) field (%d).\n"), _unit_, (_value_),    \
                               (uint32)ParamMax((_unit_), (_mem_), (_field_)),  \
                               (_mem_), (_field_)));                            \
                    return (BCM_E_PARAM);                                    \
            }                                                                \
        } while(0)

/*
 * Macro:
 *     SC_LOCK
 * Purpose:
 *     Lock take the Field control mutex
 */
#define SC_LOCK(control) \
            sal_mutex_take((control)->sc_lock, sal_mutex_FOREVER)

/*
 * Macro:
 *     SC_UNLOCK
 * Purpose:
 *     Lock take the station control mutex
 */
#define SC_UNLOCK(control) \
            sal_mutex_give((control)->sc_lock);

static int th3_l2_prio_with_no_free_entries = FALSE;
static _bcm_l2_station_control_t *_th3_l2_station_control[BCM_MAX_NUM_UNITS] = {NULL};

/*
 * Function:
 *     _bcm_th3_l2_station_control_get
 * Purpose:
 *     Get device station control structure pointer.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     sc    - (OUT) Device station structure pointer. 
 * Retruns:
 *     BCM_E_XXX
 * Note:
 * Use this function to replace _bcm_l2_station_control_get
 */
int
_bcm_th3_l2_station_control_get(int unit, _bcm_l2_station_control_t **sc)
{

    /* Input parameter check */
    if (NULL == sc) {
        return (BCM_E_PARAM);
    }

    if (NULL == _th3_l2_station_control[unit]) {
        return (BCM_E_INIT);
    }

    *sc = _th3_l2_station_control[unit];

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_l2_station_control_deinit
 * Purpose:
 *     Uninitialize device station control information.
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_station_control_deinit(int unit)
{
    _bcm_l2_station_control_t  *sc; /* Station control structure pointer. */

    sc = _th3_l2_station_control[unit];
    if (NULL == sc) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN
        (bcm_th3_l2_station_delete_all(unit));

    if (NULL != sc->entry_arr) {
        sal_free(sc->entry_arr);
        sc->entry_arr = NULL;
    }

    if (NULL != sc->entry_arr_2) {
        sal_free(sc->entry_arr_2);
        sc->entry_arr_2 = NULL;
    }

    if (NULL != sc->sc_lock) {
        sal_mutex_destroy(sc->sc_lock);
    }

    _th3_l2_station_control[unit] = NULL;

    sal_free(sc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_control_init
 * Purpose:
 *     Initialize device station control information
 * Parameters:
 *     unit  - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_station_control_init(int unit)
{
    int                        rv;          /* Operation return status.     */
    _bcm_l2_station_control_t  *sc = NULL;  /* Station control structure.   */
    uint32                     mem_size;    /* Size of entry buffer.        */
    uint32                     max_entries; /* Max no. of entries in table. */
    int                        i;           /* Temporary iterator variable. */

    /* Deinit if control has already been initialized. */
    if (NULL != _th3_l2_station_control[unit]) {
        rv = _bcm_th3_l2_station_control_deinit(unit);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* Allocate the station control memory for this unit. */
    sc = (_bcm_l2_station_control_t *) sal_alloc
            (sizeof(_bcm_l2_station_control_t), "L2 station control");
    if (sc == NULL) {
        return (BCM_E_MEMORY);
    }

    sal_memset(sc, 0, sizeof(_bcm_l2_station_control_t));

    sc->last_allocated_sid = (_BCM_L2_STATION_ID_BASE - 1);

    max_entries = soc_mem_index_count(unit, MY_STATION_TCAMm);

    sc->entries_total = max_entries;

    sc->entries_free = max_entries;

    sc->entry_count = 0;

    mem_size = (max_entries * sizeof(_bcm_l2_station_entry_t *));

    sc->entry_arr = (_bcm_l2_station_entry_t **)
                        sal_alloc(mem_size, "L2 station entry pointers");
    if (sc->entry_arr == NULL) {
        sal_free(sc);
        sc = NULL;
        return (BCM_E_MEMORY);
    }

    for (i = 0; i < max_entries; i++) {
        sc->entry_arr[i] = NULL;
    }

    sc->sc_lock = sal_mutex_create("L2 station control.lock");
    if (NULL == sc->sc_lock) {
        sal_free(sc->entry_arr);
        sc->entry_arr = NULL;
        sal_free(sc);
        sc = NULL;
        return (BCM_E_MEMORY);
    }

    _th3_l2_station_control[unit] = sc;

    return (BCM_E_NONE);
}

STATIC int
_bcm_th3_l2_port_mask_gport_resolve(int unit, bcm_gport_t gport,
                                    bcm_module_t *modid, bcm_port_t *port,
                                    bcm_trunk_t *trunk_id, int *id)
{
    int rv = SOC_E_NONE;

    *modid = -1;
    *port = -1;
    *trunk_id = BCM_TRUNK_INVALID;
    *id = -1;

    if (SOC_GPORT_IS_TRUNK(gport)) {
        *trunk_id = SOC_GPORT_TRUNK_GET(gport);
    } else if (SOC_GPORT_IS_LOCAL(gport)) {
        *modid = SOC_BASE_MODID(unit);
        *port = SOC_GPORT_LOCAL_GET(gport);
    } else if (SOC_GPORT_IS_MODPORT(gport)) {
        *modid = 0; /*SOC_GPORT_MODPORT_MODID_GET(gport); should always be 0 */
        *port = SOC_GPORT_MODPORT_PORT_GET(gport);
    } else {
        rv = SOC_E_PORT;
    }

    return rv;
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_set
 * Purpose:
 *     Set a station entrie's parameters to entry buffer for hardware table
 *     write operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     tcam_mem   - (IN) Hardware memory name.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (IN) Software station entry pointer.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_set(int unit,
                              soc_mem_t tcam_mem,
                              bcm_l2_station_t *station,
                              _bcm_l2_station_entry_t *ent_p)
{
    int gport_id = -1;
    bcm_port_t port_out = 0;
    bcm_module_t mod_out = 0;
    bcm_trunk_t trunk_id = BCM_TRUNK_INVALID;
    uint32 mod_port_data = 0; /* concatenated modid and port */
    bcm_port_t port_out_mask;
    bcm_module_t mod_out_mask;
    bcm_trunk_t trunk_id_mask = -1;  
    int gport_id_mask = -1;
    uint32 mod_port_mask = 0; /* concatenated modid and port */
    int rv=0;
    int num_bits_for_port;

    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent, VALIDf,
        (1 << soc_mem_field_length(unit, tcam_mem, VALIDf)) - 1);

    soc_mem_mac_addr_set(unit, tcam_mem, ent_p->tcam_ent,
                         MAC_ADDRf, station->dst_mac);

    soc_mem_mac_addr_set(unit, tcam_mem, ent_p->tcam_ent,
                         MAC_ADDR_MASKf, station->dst_mac_mask);

    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                        VLAN_IDf, station->vlan);

    soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                        VLAN_ID_MASKf, station->vlan_mask);

    /* validate both src_port and src_port_mask has the same type */
    if (GPORT_TYPE(station->src_port) != GPORT_TYPE(station->src_port_mask)) {
        return BCM_E_PARAM;
    }
 
    if (BCM_GPORT_IS_SET(station->src_port)) {
        rv = _bcm_esw_gport_resolve(unit, station->src_port, 
                      &mod_out, 
                      &port_out, &trunk_id,
                      &gport_id);
        BCM_IF_ERROR_RETURN(rv);

        if (BCM_GPORT_IS_TRUNK(station->src_port)) {
            if (BCM_TRUNK_INVALID == trunk_id) {
                return BCM_E_PORT;
            }
        } else {
            if ((mod_out == -1) || (port_out == -1)) {
                return BCM_E_PORT;
            }
        }

        /* retrieve the port mask */
        rv = _bcm_th3_l2_port_mask_gport_resolve(unit,
                                                 station->src_port_mask,
                                                 &mod_out_mask,
                                                 &port_out_mask,
                                                 &trunk_id_mask,
                                                 &gport_id_mask);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        port_out = station->src_port;
        port_out_mask = station->src_port_mask;
        mod_out = 0;
        mod_out_mask = 0;
    }

    if (trunk_id != BCM_TRUNK_INVALID) {

        /* and the SOURCE_FIELDf supports the trunk operation */
        SOC_IF_ERROR_RETURN(soc_mem_field32_fit(unit, tcam_mem, 
                            SOURCE_FIELD_MASKf,
                            1 << SOC_TRUNK_BIT_POS(unit)));

        mod_port_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
        mod_port_mask = trunk_id_mask & 
            ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
        mod_port_mask |= (1 << SOC_TRUNK_BIT_POS(unit));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent, SOURCE_FIELDf,
                            mod_port_data);
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent, SOURCE_FIELD_MASKf,
                            mod_port_mask);
    } else {
            num_bits_for_port = _shr_popcount(
                             (unsigned int)SOC_PORT_ADDR_MAX(unit));

            mod_port_data = (mod_out << num_bits_for_port) | port_out;
            mod_port_mask = mod_out_mask & SOC_MODID_MAX(unit);
            mod_port_mask <<= num_bits_for_port;
            mod_port_mask |= (port_out_mask & SOC_PORT_ADDR_MAX(unit));

            /* Clear the trunk ID bit. */
            mod_port_data &= ~(1 << SOC_TRUNK_BIT_POS(unit));

            /* Must match on the T bit (which should be 0) for the devices
             * supporting the trunk operation with this field
             */
            if (BCM_GPORT_IS_SET(station->src_port)) {
                if (soc_mem_field32_fit(unit, tcam_mem, SOURCE_FIELD_MASKf, 
                        1 << SOC_TRUNK_BIT_POS(unit)) == SOC_E_NONE) {
                    mod_port_mask |= (1 << SOC_TRUNK_BIT_POS(unit));
                }
            }

            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_FIELDf, mod_port_data);
            soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                                SOURCE_FIELD_MASKf, mod_port_mask);
        }

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            MPLS_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_MPLS) ? 1 : 0));
        
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            IPV4_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_IPV4) ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            IPV6_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_IPV6) ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            ARP_RARP_TERMINATION_ALLOWEDf,
                            ((station->flags & BCM_L2_STATION_ARP_RARP) ? 1 : 0));
        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            COPY_TO_CPUf,
                            ((station->flags & BCM_L2_STATION_COPY_TO_CPU)
                            ? 1 : 0));

        soc_mem_field32_set(unit, tcam_mem, ent_p->tcam_ent,
                            DISCARDf,
                            ((station->flags & BCM_L2_STATION_DISCARD) ? 1 : 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_update
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_update(int unit,
                                 int sid,
                                 bcm_l2_station_t *station,
                                 _bcm_l2_station_entry_t *ent_p)
{
    soc_mem_t   tcam_mem;    /* TCAM memory name.         */
    int         mem_size = 0; /* Entry buffer size.       */

    /* Input parameter check. */
    if (NULL == ent_p || NULL == station) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) (idx=%d) (prio: o=%d n=%d) "
                           "- update.\n"), unit, sid, ent_p->hw_index, ent_p->prio,
               station->priority));

    if (ent_p->prio == station->priority) {
        ent_p->flags |= _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE;
    } else {
        ent_p->prio = station->priority;
    }


    tcam_mem = MY_STATION_TCAMm;
    mem_size = sizeof(my_station_tcam_entry_t);

    if (mem_size == 0) {
        /* Must be a valid mem_size value. */
        return (BCM_E_INTERNAL);
    }

    ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
    if (ent_p->tcam_ent == NULL) {
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p->tcam_ent, 0, mem_size);
    
    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_entry_set(unit, tcam_mem, 
                           station, ent_p));

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_create
 * Purpose:
 *     Allocate an entry buffer and setup entry parameters for hardware write
 *     operation.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     sid        - (IN) Station ID.
 *     station    - (IN) Station parameters to be set in hardware.
 *     ent_p      - (OUT) Pointer to Station entry type data.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_create(int unit,
                             int sid,
                             bcm_l2_station_t *station,
                             _bcm_l2_station_entry_t **ent_pp)
{
    _bcm_l2_station_control_t *sc;           /* Station control structure. */
    int                       index;         /* Entry index.               */
    _bcm_l2_station_entry_t   *ent_p = NULL; /* L2 station entry pointer.  */
    soc_mem_t                 tcam_mem;      /* TCAM memory name.          */ 
    int                       mem_size = 0;  /* Entry buffer size.         */
    int                       rv;            /* Operation return status.   */
    int                       entries_total;
    _bcm_l2_station_entry_t   **entry_arr;

    /* Input parameter check. */
    if (NULL == station || NULL == ent_pp) {
        return (BCM_E_PARAM);
    }
    
    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    if (0 == sc->entries_free) {
        return (BCM_E_RESOURCE);
    }
    entry_arr = (sc->entry_arr);
    entries_total = sc->entries_total;

    ent_p = (_bcm_l2_station_entry_t *)
        sal_alloc(sizeof(_bcm_l2_station_entry_t), "Sw L2 station entry");
    if (NULL == ent_p) {
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p, 0, sizeof(_bcm_l2_station_entry_t));
    ent_p->sid = sid;
    ent_p->prio = station->priority;

    if (0 == sc->entries_free) {
        sal_free(ent_p);
        return (BCM_E_RESOURCE);
    }

    /* Get the first free slot. */
    for (index = 0; index < entries_total; index++) {
        if (NULL == entry_arr[index]) {
            ent_p->hw_index = index;
            break;
        }
    }

    tcam_mem = MY_STATION_TCAMm;
    mem_size = sizeof(my_station_tcam_entry_t);

    if (mem_size == 0) {
        /* mem_size must be greater than zero. */
        sal_free(ent_p);
        return (BCM_E_INTERNAL);
    }

    ent_p->tcam_ent = sal_alloc(mem_size, "L2 station entry buffer");
    if (NULL == ent_p->tcam_ent) {
        sal_free(ent_p);
        return (BCM_E_MEMORY);
    }

    sal_memset(ent_p->tcam_ent, 0, mem_size);

    rv = _bcm_th3_l2_station_entry_set(unit, tcam_mem, station, ent_p);
    if (BCM_FAILURE(rv)) {
        sal_free(ent_p->tcam_ent);
        sal_free(ent_p);
        return (rv);
    }

    /* Decrement free entries count */
    sc->entries_free--;
    sc->entry_count++;
    sc->entry_arr[index] = ent_p;

    *ent_pp = ent_p;

    return (BCM_E_NONE);

}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_prio_cmp
 * Purpose:
 *     Compare station entry priority values.
 * Parameters:
 *     prio_first   - (IN) First entry priority value.
 *     prio_second  - (IN) Second entry priority value.
 * Returns:
 *     0    - if both priority values are equal.
 *     -1   - if priority value of first entry is lower than second.
 *     1    - if priority value of first entry is greater than second.
 */
STATIC int
_bcm_th3_l2_station_entry_prio_cmp(int prio_first, int prio_second)
{
    int rv; /* Comparison result. */

    if (prio_first == prio_second) {
        rv = 0;
    } else if (prio_first < prio_second) {
        rv = -1;
    } else {
        rv = 1;
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_th3_l2_station_prio_move_required
 * Purpose:
 *     Check if entry move is required based on priority value.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     ent_p  - (IN) Station entry for priority comparison.
 * Returns:
 *     TRUE   - Entry move is required based on priority comparison.
 *     FALSE  - Entry move is not required based on priority comparison.
 */
STATIC int
_bcm_th3_l2_station_prio_move_required(int unit, _bcm_l2_station_entry_t *ent_p)
{
    _bcm_l2_station_control_t *sc;      /* station control structure.   */
    _bcm_l2_station_entry_t **entry_arr;/* station entry array pointer  */
    uint32                    count;    /* Maximum number of entries.   */
    int                       i;        /* Temporary iterator variable. */
    int                       ent_prio; /* Entry priority.              */
    int                       flag;     /* Flag value denotes if we are */
                                        /* before or after s_ent.       */

    /* Input parameter check. */
    if (NULL == ent_p) {
        return (BCM_E_INTERNAL);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    /* Get total number for entries supported. */
    count = sc->entries_total;
    entry_arr = (sc->entry_arr);

    /* Get the entry priority value. */
    ent_prio = ent_p->prio;

    /* Init flag value, we are before s_ent. */
    flag = -1;

    for (i = 0; i < count; i++) {

        if (entry_arr[i] == ent_p) {
            /* Now we are after s_ent. */
            flag = 1;
            continue;
        }

        if (NULL == entry_arr[i]) {
            continue;
        }

        if (-1 == flag) {
            if (_bcm_th3_l2_station_entry_prio_cmp(entry_arr[i]->prio,
                                                   ent_prio) < 0) {

                LOG_DEBUG(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "L2(unit %d) Info: (SID=%d) found lower prio than "
                                       "(prio=%d).\n"), unit, ent_p->sid, ent_p->prio));

                /*
                 * An entry  before s_ent has lower priority.
                 * So, move is required.
                 */
                return (TRUE);
            }
        } else {
            if (_bcm_th3_l2_station_entry_prio_cmp(entry_arr[i]->prio,
                                                   ent_prio) > 0) {
                LOG_DEBUG(BSL_LS_BCM_L2,
                          (BSL_META_U(unit,
                                      "L2(unit %d) Info: (SID=%d) found higher prio than "
                                       "(prio=%d).\n"), unit, ent_p->sid, ent_p->prio));
                /*
                 * An entry after s_ent has higher priority than s_ent prio.
                 * So, move is required.
                 */
                return (TRUE);
            }
        }
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) (prio=%d) no move.\n"),
               unit, ent_p->sid, ent_p->prio));

    return (FALSE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_move_check
 * Purpose:
 *     Calculate number of entries that need to be moved for installing the
 *     new station entry.
 * Parameters:
 *     unit                     - (IN) BCM device number
 *     null_index               - (IN) Free null entry hardware index.
 *     target_index             - (IN) Target hardware index for the entry.
 *     direction                - (IN) Direction of Move.
 *     shifted_entries_count    - (OUT) No. of entries to be moved.
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_bcm_th3_l2_station_entry_move_check(int unit, int null_index,
                                     int target_index, int direction,
                                     int *shifted_entries_count)
{
    *shifted_entries_count += direction * (null_index - target_index);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_dir_get
 * Purpose:
 *     Find out direction in which entries need to be moved for installing
 *     new station entry in hardware.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     ent_p            - (IN) Station entry pointer.
 *     prev_null_index  - (IN) Location of previous null entry in hardware.
 *     next_null_index  - (IN) Location of next null entry in hardware.
 *     target_index     - (IN) Target index where entry needs to be installed.
 *     dir              - (OUT) Direction in which entries need to be moved.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_dir_get(int unit,
                                  _bcm_l2_station_entry_t *ent_p,
                                  int prev_null_index,
                                  int next_null_index,
                                  int target_index,
                                  int *dir)
{
    _bcm_l2_station_control_t  *sc;                   /* Station control    */
                                                      /* strucutre.         */
    int                        shift_up = FALSE;      /* Shift UP status.   */
    int                        shift_down = FALSE;    /* Shift DOWN status. */
    int                        shift_up_amount = 0;   /* Shift UP amount.   */
    int                        shift_down_amount = 0; /* Shift DOWN amount. */
    int                        rv;                    /* Operation return   */
                                                      /* status.            */

    /* Input parameter check. */
    if (NULL == ent_p || NULL == dir) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    if (prev_null_index != -1) {
        rv = _bcm_th3_l2_station_entry_move_check(unit, prev_null_index,
                                                  target_index, -1,
                                                  &shift_up_amount);
        shift_up = BCM_SUCCESS(rv);
    } else {
        shift_up = FALSE;
    }

    if (next_null_index != -1) {
        rv = _bcm_th3_l2_station_entry_move_check(unit, next_null_index,
                                                  target_index, 1,
                                                  &shift_down_amount);
        shift_down = BCM_SUCCESS(rv);
    } else {
        shift_down = FALSE;
    }

    if (shift_up == TRUE) {
        if (shift_down == TRUE) {
            if (shift_up_amount < shift_down_amount) {
                *dir = -1;
            } else {
                *dir = 1;
            }
        } else {
            *dir = -1;
        }
    } else {
        if (shift_down == TRUE) {
            *dir = 1;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_move
 * Purpose:
 *     Move a station entry by a specified amount
 * Parameters:
 *     unit     - (IN) BCM device number
 *     s_ent    - (IN) Station entry pointer.
 *     amount   - (IN) Direction in which entries need to be moved.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_move(int unit,
                               _bcm_l2_station_entry_t *s_ent,
                               int amount)
{
    _bcm_l2_station_control_t   *sc;          /* Station control structure. */
    _bcm_l2_station_entry_t   **entry_arr;    /* Station entry pointer . */
    int                         tcam_idx_old; /* Original entry TCAM index. */
    int                         tcam_idx_new; /* Entry new TCAM index.      */
    int                         rv;           /* Operation return status.   */
    soc_mem_t                   tcam_mem;     /* TCAM memory name.          */
    uint32                      entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry    */
                                                                /* buffer.  */
    int                         count;

    /* Input parameter check. */
    if (s_ent == NULL) {
        return (BCM_E_PARAM);
    }

    /* Input parameter check. */
    if (amount == 0) {
        return (BCM_E_NONE);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    tcam_idx_old =  s_ent->hw_index;

    tcam_idx_new = tcam_idx_old + amount;

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) move (oidx=%d nidx=%d) (amt=%d).\n"),
               unit, s_ent->sid, s_ent->hw_index, tcam_idx_new, amount));

    count = sc->entries_total;
    entry_arr = (sc->entry_arr);

    if (tcam_idx_old < 0 || tcam_idx_old >= count) {
        return (BCM_E_PARAM);
    }

    tcam_mem = MY_STATION_TCAMm;

    if (s_ent->flags & _BCM_L2_STATION_ENTRY_INSTALLED) {
        /* Clear entry buffer */
        sal_memset(entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, tcam_idx_old, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_new, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx_old,
                           soc_mem_entry_null(unit, tcam_mem));
        BCM_IF_ERROR_RETURN(rv);
    }

    if (th3_l2_prio_with_no_free_entries == FALSE) {
        entry_arr[s_ent->hw_index] = NULL;
    }

    entry_arr[tcam_idx_new] = s_ent;

    s_ent->hw_index = tcam_idx_new;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_shift_down
 * Purpose:
 *     Shift entries down by one index to create a free entry at target
 *     index location.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     target_index     - (IN) Target index for entry install operation.
 *     next_null_index  - (IN) Location of free entry after target index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_shift_down(int unit,
                                     int overlay,
                                     int target_index,
                                     int next_null_index)
{
    uint16                      empty_idx;   /* Empty index.               */
    _bcm_l2_station_control_t   *sc;         /* Station control structure. */
    _bcm_l2_station_entry_t     **entry_arr; /* Station entry pointer */
    int                         tmp_idx1;    /* Temporary index.           */
    int                         tmp_idx2;    /* Temporary index.           */ 
    int                         max_entries; /* Maximum entries supported. */
    int                         rv;          /* Operation return status.   */

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: Shift UP (tidx=%d null-idx=%d).\n"),
               unit, target_index, next_null_index));

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    empty_idx = next_null_index;

    max_entries = sc->entries_total;
    entry_arr = (sc->entry_arr);

    /*
     * Move entries one step down
     *     starting from the last entry
     * IDEA: 
     *     Move till empty_idx > target_index  
     */
    while (empty_idx > target_index) {
        if (empty_idx == 0) {
            tmp_idx1 = 0;
            tmp_idx2 = max_entries - 1;

            rv = _bcm_th3_l2_station_entry_move(unit,
                                                entry_arr[max_entries - 1],
                                                (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);
        } else {
            tmp_idx1 = empty_idx;
            tmp_idx2 = (empty_idx - 1);
            rv = _bcm_th3_l2_station_entry_move(unit,
                                                entry_arr[empty_idx - 1],
                                                (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);

            empty_idx--;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_shift_up
 * Purpose:
 *     Shift entries up by one index to create a free entry at target
 *     index location.
 * Parameters:
 *     unit             - (IN) BCM device number
 *     target_index     - (IN) Target index for entry install operation.
 *     prev_null_index  - (IN) Location of free entry before target index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_shift_up(int unit,
                                   int overlay,
                                   int target_index,
                                   int prev_null_index)
{
    uint16                      empty_idx;   /* Empty index.               */
    _bcm_l2_station_control_t   *sc;         /* Station control structure. */
    _bcm_l2_station_entry_t     **entry_arr; /* Station entry pointer */
    int                         tmp_idx1;    /* Temporary index.           */
    int                         tmp_idx2;    /* Temporary index.           */ 
    int                         max_entries; /* Maximum entries supported. */
    int                         rv;          /* Operation return status.   */

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: Shift UP (tidx=%d null-idx=%d).\n"),
               unit, target_index, prev_null_index));

    empty_idx = prev_null_index;

    max_entries = sc->entries_total;
    entry_arr = (sc->entry_arr);


    /*
     * Move entries one step up
     *     starting from the first entry
     * IDEA: 
     *     Move till empty_idx < target_index  
     */
    while (empty_idx < target_index) {
        if (empty_idx == (max_entries - 1)) {
            tmp_idx1 = max_entries - 1;
            tmp_idx2 = 0;
            rv = _bcm_th3_l2_station_entry_move(unit,
                                                entry_arr[0],
                                                (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);
            empty_idx = 0;
        } else {
            tmp_idx1 = empty_idx;
            tmp_idx2 = (empty_idx + 1);
            rv = _bcm_th3_l2_station_entry_move(unit,
                                                entry_arr[empty_idx + 1],
                                                (tmp_idx1 - tmp_idx2));
            BCM_IF_ERROR_RETURN(rv);

            empty_idx++;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_install
 * Purpose:
 *     Write a station entry to hardware
 * Parameters:
 *     unit     - (IN) BCM device number
 *     ent_p    - (IN) Pointer to station entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_install(int unit, _bcm_l2_station_entry_t *ent_p)
{
    int     rv;         /* Operation return status. */ 
    soc_mem_t tcam_mem; /* TCAM memory name.        */

    if (ent_p == NULL) {
        return (BCM_E_PARAM);
    }

    tcam_mem = MY_STATION_TCAMm;

    if (ent_p->tcam_ent == NULL) {
        return (BCM_E_INTERNAL);
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) - install (idx=%d).\n"),
               unit, ent_p->sid, ent_p->hw_index));

    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, ent_p->hw_index,
                       ent_p->tcam_ent);

    sal_free(ent_p->tcam_ent);

    ent_p->tcam_ent = NULL;

    return (rv);
}


/*
 * Function:
 *     _bcm_th3_l2_station_entry_prio_install
 * Purpose:
 *     Install an entry based on priority value.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     ent_p    - (IN) Pointer to station entry.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_prio_install(int unit, _bcm_l2_station_entry_t *ent_p)
{
    _bcm_l2_station_control_t  *sc;                /* Control structure.     */
    _bcm_l2_station_entry_t  **entry_arr;          /* L2 station entry       */
    int                        idx_old;            /* Entry old TCAM index.  */
    int                        idx_target;         /* Entry target index.    */
    int                        prev_null_idx = -1; /* Prev NULL entry index. */
    int                        next_null_idx = -1; /* Next NULL entry index. */
    int                        temp;               /* Temporary variable.    */
    int                        dir = -1;           /* -1 = UP, 1 = DOWN      */
    int                        decr_on_shift_up = TRUE; /* Shift up status   */
    int                        rv;                 /* Return status.         */
    int                        flag_no_free_entries = FALSE; /* No free      */
                                                   /* entries status         */
    int                        max_entries; 
    int                        free_entries; 
    int                        overlay;
    th3_l2_prio_with_no_free_entries = FALSE;

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));


    if ((ent_p->flags & _BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE)
        || (_bcm_th3_l2_station_prio_move_required(unit, ent_p) == FALSE)) {
        goto end;
    }

    idx_old = ent_p->hw_index;

    overlay = _BCM_L2_STATION_ENTRY_UNDERLAY;
    max_entries = sc->entries_total;
    entry_arr = (sc->entry_arr);
    free_entries  = sc->entries_free;

    if (idx_old >= max_entries) {
        return (BCM_E_INTERNAL);
    }

    if (free_entries == 0) {
        if ((ent_p->flags & _BCM_L2_STATION_ENTRY_INSTALLED) == 0) {
            entry_arr[ent_p->hw_index] = NULL;
            flag_no_free_entries = TRUE;
        } else {
            return (BCM_E_CONFIG);
        }
    }

    for (idx_target = 0; idx_target < max_entries; idx_target++) {

        /* Skip the entry itself. */
        if (ent_p == entry_arr[idx_target]) {
            continue;
        }

        if (NULL == entry_arr[idx_target]) {
            /* Store the null slot index. */
            prev_null_idx = idx_target;
            continue;
        }

        if (_bcm_th3_l2_station_entry_prio_cmp(ent_p->prio,
                                               entry_arr[idx_target]->prio) > 0) {
            /* Found the entry with priority lower than ent_p prio. */
            break;
        }
    }

    temp = idx_target;

    if (idx_target != (max_entries - 1)) {
        /*  Find next empty slot after ent_p. */
        for (; temp < max_entries; temp++) {
            if (entry_arr[temp] == NULL) {
                next_null_idx = temp;
                break;
            }
        }
    }

    /* Put the entry back. */
    if (flag_no_free_entries == TRUE) {
        entry_arr[ent_p->hw_index] = ent_p;
    }

    /* No empty slot found. */
    if (prev_null_idx == -1 && next_null_idx == -1) {
        return (BCM_E_CONFIG);
    }

    if (idx_target == max_entries) {
        /* Target location is after the last installed entry. */
        if (prev_null_idx == (max_entries - 1)) {
            idx_target = prev_null_idx;
            goto only_move;
        } else {
            idx_target = (max_entries - 1);
            decr_on_shift_up = FALSE;
        }
    }

    if (_bcm_th3_l2_station_entry_dir_get(unit,
                                         ent_p,
                                         prev_null_idx,
                                         next_null_idx,
                                         idx_target,
                                         &dir) == FALSE) {
        return (BCM_E_PARAM);
    }

    if (dir == 1) {
        /* 
         * Move the entry at the target index to target_index+1. This may
         * mean shifting more entries down to make room. In other words,
         * shift the target index and any that follow it down 1 as far as the
         * next empty index.
         */
        if (entry_arr[idx_target] != NULL) {
            BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_entry_shift_down(unit,
                                    overlay, idx_target, next_null_idx));
        }
    } else {
        if (decr_on_shift_up == TRUE) {
            idx_target--;
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Info: Decr. on Shift UP (idx_tgt=%d).\n"),
                       unit, idx_target));
        }

        if (entry_arr[idx_target] != NULL) {
            /* coverity[negative_returns] */
            BCM_IF_ERROR_RETURN (_bcm_th3_l2_station_entry_shift_up(unit,
                                     overlay, idx_target, prev_null_idx));
        }
    }

    /* Move the entry from old index to new index. */
only_move:
    if ((idx_target - ent_p->hw_index) != 0) {

        if (flag_no_free_entries == TRUE) {
            th3_l2_prio_with_no_free_entries = TRUE;
        }

        rv = _bcm_th3_l2_station_entry_move(unit, ent_p,
                (idx_target - ent_p->hw_index));

        if (BCM_FAILURE(rv)) {
            th3_l2_prio_with_no_free_entries = FALSE;
            return (rv);
        }

        th3_l2_prio_with_no_free_entries = FALSE;
    }

end:

    ent_p->flags &= ~_BCM_L2_STATION_ENTRY_PRIO_NO_CHANGE;

    rv = _bcm_th3_l2_station_entry_install(unit, ent_p);
    if (BCM_FAILURE(rv)) {
        return rv;
    }
    return (BCM_E_NONE);
}

/*
 * Function: 
 *     _bcm_th3_l2_station_entry_get
 * Purpose:
 *     Get station entry details by performing
 *     lookup using station ID.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     sid   - (IN) Station ID.
 *     ent_p - (OUT) Station look up result.
 * Retruns:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_station_entry_get(int unit,
                              int sid,
                              _bcm_l2_station_entry_t **ent_p)
{
    _bcm_l2_station_control_t *sc;   /* Station control structure pointer. */
    _bcm_l2_station_entry_t **entry_arr;
    int                       index; /* Entry index.                       */
    int                       count;
    /* Input parameter check. */
    if (ent_p == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    count = sc->entries_total;
    entry_arr = (sc->entry_arr);

    for (index = 0; index < count; index++) {

        if (entry_arr[index] == NULL) {
            continue;
        }

        if (sid == entry_arr[index]->sid) {
            *ent_p = entry_arr[index];
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Info: (SID=%d) - found (idx=%d).\n"),
                       unit, sid, index));
            return (BCM_E_NONE);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_L2,
              (BSL_META_U(unit,
                          "L2(unit %d) Info: (SID=%d) - not found (idx=%d).\n"),
               unit, sid, index));

    /* L2 station entry with ID == sid not found. */
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *     _bcm_th3_l2_station_param_check
 * Purpose:
 *     Validate L2 station user input parameters.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station      - (IN) Station information for lookup by hardware.
 *     station_id   - (IN) Station entry ID.
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_param_check(int unit, bcm_l2_station_t *station,
                                int station_id)
{
    soc_mem_t tcam_mem; /* Hardware table name.     */
    bcm_gport_t gport_id;
    bcm_port_t port_out;
    bcm_module_t mod_out;
    bcm_trunk_t trunk_id;
    uint32 mod_port_data = 0; /* concatenated modid and port */
    int num_bits_for_port;

    /* Input parameter check. */
    if (NULL == station) {
        return (BCM_E_PARAM);
    }

    if (station->flags & BCM_L2_STATION_FCOE) {
        /* FCoE Termination is not supported */
        return (BCM_E_UNAVAIL);
    }

    if (station->flags & BCM_L2_STATION_OAM) {
        return (BCM_E_UNAVAIL);
    }

    
    tcam_mem = MY_STATION_TCAMm;
    ParamCheck(unit, tcam_mem, VLAN_IDf, station->vlan);
    ParamCheck(unit, tcam_mem, VLAN_ID_MASKf, station->vlan_mask);

    if (GPORT_TYPE(station->src_port) != GPORT_TYPE(station->src_port_mask)) {
               return BCM_E_PARAM;
    }
       num_bits_for_port = _shr_popcount(
                              (unsigned int)SOC_PORT_ADDR_MAX(unit));
       if (BCM_GPORT_IS_SET(station->src_port)) {
           BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, 
                      station->src_port,
                      &mod_out,
                      &port_out, &trunk_id,
                      &gport_id));
           if (BCM_GPORT_IS_TRUNK(station->src_port)) {
               if (BCM_TRUNK_INVALID == trunk_id) {
                   return BCM_E_PORT;
               }

               mod_port_data = ((1 << SOC_TRUNK_BIT_POS(unit)) | trunk_id);
           } else {
               if ((-1 == mod_out) || (-1 == port_out)) {
                   return BCM_E_PORT;
               }
               mod_port_data = (mod_out << num_bits_for_port) | port_out;
           }
       } else {
           int my_modid;
           BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit,&my_modid));
           port_out = station->src_port;
           mod_port_data = station->src_port | 
                           (my_modid << num_bits_for_port);
       } 

       if (SOC_MEM_FIELD_VALID(unit, tcam_mem, SOURCE_FIELDf)) {
           ParamCheck(unit, tcam_mem, SOURCE_FIELDf, mod_port_data);
       } else {
           ParamCheck(unit, tcam_mem, ING_PORT_NUMf, port_out);
       }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_th3_l2_station_add
 * Purpose:
 *     Create an entry with specified ID or generated ID and install
 *     the entry in hardware.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN/OUT) Station entry ID.
 *     station      - (IN) Station information for lookup by hardware.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_add(int unit,
                      int *station_id,
                      bcm_l2_station_t *station)
{
    _bcm_l2_station_control_t *sc;         /* Control structure.       */
    _bcm_l2_station_entry_t   *ent = NULL; /* Entry pointer.           */
    int                       rv;          /* Operation return status. */
    int                       sid = -1;    /* Entry ID.                */

    /* Input parameter check. */
    if (NULL == station || NULL == station_id)  {
        return (BCM_E_PARAM);
    }

    /* ID needs to be passed for replace operation. */
    if ((station->flags & BCM_L2_STATION_REPLACE)
        && !(station->flags & BCM_L2_STATION_WITH_ID)) {
        return (BCM_E_PARAM);
    }

    rv = _bcm_th3_l2_station_param_check(unit, station, *station_id);
    if (BCM_FAILURE(rv)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    if (station->flags & BCM_L2_STATION_WITH_ID) {
        if ((sc->last_allocated_sid < *station_id) &&
            (*station_id <= _BCM_L2_STATION_ID_MAX)) {
            sc->last_allocated_sid = *station_id;
        }
        sid = *station_id;
    } else {
        sid = ++sc->last_allocated_sid;

        if (_BCM_L2_STATION_ID_MAX < sid) {
            sc->last_allocated_sid = _BCM_L2_STATION_ID_MAX;

            for (sid = _BCM_L2_STATION_ID_BASE;
                 sid <= _BCM_L2_STATION_ID_MAX; sid++) {
                if (BCM_E_NOT_FOUND == 
                        _bcm_th3_l2_station_entry_get(unit, sid, &ent)) {
                    break;
                }
            }
        }

        *station_id = sid;
    }

    rv = _bcm_th3_l2_station_entry_get(unit, sid, &ent);

    /* For replace operation, entry must exist. */
    if (BCM_FAILURE(rv)
        && (station->flags & BCM_L2_STATION_REPLACE)) {
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: Replace (SID=%d) - Invalid (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (rv);
    }

    /* For non-replace operations, entry must not exist. */
    if (BCM_SUCCESS(rv)
        && (0 == (station->flags & BCM_L2_STATION_REPLACE))) {
        LOG_INFO(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: (SID=%d) add - failed (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (BCM_E_EXISTS);
    }

    if (0 == (station->flags & BCM_L2_STATION_REPLACE)) {
        rv = _bcm_th3_l2_station_entry_create(unit, sid, station, &ent);
        if (BCM_FAILURE(rv)) {
            LOG_INFO(BSL_LS_BCM_L2,
                     (BSL_META_U(unit,
                                 "L2(unit %d) Error: Station (SID=%d) add - failed (%s).\n"),
                      unit, sid, bcm_errmsg(rv)));
            SC_UNLOCK(sc);
            return (rv);
        }
    } else {
        rv = _bcm_th3_l2_station_entry_update(unit, sid, station, ent);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                                  "L2(unit %d) Error: Station (SID=%d) update - failed (%s).\n"),
                       unit, sid, bcm_errmsg(rv)));
            SC_UNLOCK(sc);
            return (rv);
        }
    }

    rv = _bcm_th3_l2_station_entry_prio_install(unit, ent);
    if (BCM_FAILURE(rv)) {
        if (NULL != ent->tcam_ent) {
            sal_free(ent->tcam_ent);
        }
        LOG_ERROR(BSL_LS_BCM_L2,
                  (BSL_META_U(unit,
                              "L2(unit %d) Error: (SID=%d) install - failed (%s).\n"),
                   unit, sid, bcm_errmsg(rv)));
        SC_UNLOCK(sc);
        return (rv);
    }

    /* Mark the entry as installed. */
    ent->flags |= _BCM_L2_STATION_ENTRY_INSTALLED;

    SC_UNLOCK(sc);
    return (rv);
}

/*
 * Function:
 *     bcm_th3_l2_station_delete
 * Purpose:
 *     Deallocate the memory used to track an entry and remove the entry
 *     from hardware
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station entry ID
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_delete(int unit, int station_id)
{
    _bcm_l2_station_entry_t     *s_ent = NULL; /* Entry pointer.           */
    _bcm_l2_station_entry_t     **entry_arr = NULL; /* Entry pointer.      */
    _bcm_l2_station_control_t   *sc = NULL;    /* Control structure.       */
    int                         rv;            /* Operation return status. */
    soc_mem_t                   tcam_mem;      /* TCMA memory name.        */

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    /* Get the entry by ID. */
    rv = _bcm_th3_l2_station_entry_get(unit, station_id, &s_ent);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    entry_arr = (sc->entry_arr);

    tcam_mem = MY_STATION_TCAMm;

    /* Remove the entry from hardware by clearing the index. */
    rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, s_ent->hw_index,
                       soc_mem_entry_null(unit, tcam_mem));
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    /* Free tcam buffer. */
    if (NULL != s_ent->tcam_ent) {
        sal_free(s_ent->tcam_ent);
    }

    /* Remove the entry pointer from the list. */
    entry_arr[s_ent->hw_index] = NULL;

    /* Increment free entries count. */
    sc->entries_free++;
    sc->entry_count--;

    /* Free the entry. */
    sal_free(s_ent);
    
    /* Rset the last_allocated_sid */
    if (station_id == sc->last_allocated_sid) {
        --sc->last_allocated_sid;
    }

    SC_UNLOCK(sc);

    return (rv);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_delete_all
 * Purpose:
 *     Deallocate all memory used to track entries and remove entries from
 *     hardware.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     overlay  - (IN) Overlay/Underlay identification
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_delete_all(int unit, int overlay)
{
    _bcm_l2_station_control_t   *sc = NULL; /* Control strucutre.       */
    _bcm_l2_station_entry_t    **entry_arr = NULL; /* Entry array.      */
    int                         rv;         /* Operation return status. */
    int                         index;      /* Entry index.             */
    int                         max_entries;

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    max_entries = (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) ?
                      sc->entries_total_2 : sc->entries_total;
    entry_arr = (overlay == _BCM_L2_STATION_ENTRY_OVERLAY) ?
                    (sc->entry_arr_2) : (sc->entry_arr);

    if (NULL == entry_arr) {
        SC_UNLOCK(sc);
        return (BCM_E_NONE);
    }

    for (index = 0; index < max_entries; index++) {

        if (NULL == entry_arr[index]) {
            continue;
        }

        rv = bcm_th3_l2_station_delete(unit, entry_arr[index]->sid);

        if (BCM_FAILURE(rv)) {
            SC_UNLOCK(sc);
            return (rv);
        }
    }

    SC_UNLOCK(sc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_th3_l2_station_delete_all
 * Purpose:
 *     Deallocate all memory used to track entries and remove entries from
 *     hardware.
 * Parameters:
 *     unit     - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_delete_all(int unit)
{
    int rv;

    rv = _bcm_th3_l2_station_entry_delete_all(unit,
             _BCM_L2_STATION_ENTRY_UNDERLAY);

   return rv;
}

/*
 * Function:
 *     bcm_th3_l2_station_get
 * Purpose:
 *     Get station information for a given station ID.
 * Parameters:
 *     unit         - (IN) BCM device number
 *     station_id   - (IN) Station entry ID.
 *     station      - (OUT) Station entry lookup information.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_get(int unit, int station_id, bcm_l2_station_t *station)
{
    _bcm_l2_station_entry_t     *s_ent = NULL; /* Entry pointer           */
    _bcm_l2_station_control_t   *sc = NULL;    /* Control structure       */
    int                         rv;            /* Operation return status */
    soc_mem_t                   tcam_mem;      /* TCAM memory name        */
    uint32                      entry[SOC_MAX_MEM_FIELD_WORDS]; /* Entry   */
                                                                /* buffer  */
    int src_field;
    int src_mask_field;
    _bcm_gport_dest_t src;
    _bcm_gport_dest_t src_mask;
    int num_bits_for_port;

    /* Input parameter check. */
    if (station == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));
    SC_LOCK(sc);

    /* Get the entry by ID. */
    rv = _bcm_th3_l2_station_entry_get(unit, station_id, &s_ent);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    tcam_mem = MY_STATION_TCAMm;

    /* Clear entry buffer. */
    sal_memset(entry, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY, s_ent->hw_index, entry);
    if (BCM_FAILURE(rv)) {
        SC_UNLOCK(sc);
        return (rv);
    }

    station->priority = s_ent->prio;

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDRf, station->dst_mac);

    soc_mem_mac_addr_get(unit, tcam_mem, entry,
                         MAC_ADDR_MASKf, station->dst_mac_mask);

    station->vlan = soc_mem_field32_get(unit, tcam_mem, entry, VLAN_IDf);

    station->vlan_mask = soc_mem_field32_get(unit, tcam_mem, entry,
                                             VLAN_ID_MASKf);

    _bcm_gport_dest_t_init(&src);
    _bcm_gport_dest_t_init(&src_mask);

    src_field= soc_mem_field32_get(unit, tcam_mem, entry,
                   SOURCE_FIELDf);
    src_mask_field = soc_mem_field32_get(unit, tcam_mem, entry,
                        SOURCE_FIELD_MASKf);

    if ((src_field >> SOC_TRUNK_BIT_POS(unit)) & 0x1) {
        src.tgid = src_field & ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
        src.gport_type = _SHR_GPORT_TYPE_TRUNK;
        src_mask.tgid = src_mask_field & 
        ((1 << SOC_TRUNK_BIT_POS(unit)) - 1);
            src_mask.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else {
        src.port = src_field & SOC_PORT_ADDR_MAX(unit);
        src.modid = (src_field - src.port) /
                        (SOC_PORT_ADDR_MAX(unit) + 1);
        src.gport_type = _SHR_GPORT_TYPE_MODPORT;
        num_bits_for_port = _shr_popcount(
                            (unsigned int)SOC_PORT_ADDR_MAX(unit));
        src_mask.port = src_mask_field & SOC_PORT_ADDR_MAX(unit);
        src_mask.gport_type = _SHR_GPORT_TYPE_MODPORT;
        src_mask.modid = (src_mask_field >> num_bits_for_port) & 
                             SOC_MODID_MAX(unit);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src,
                        &(station->src_port)));
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &src_mask,
                        &(station->src_port_mask)));

    if (soc_mem_field32_get(unit, tcam_mem, entry,
                            MPLS_TERMINATION_ALLOWEDf)) {
            station->flags |= BCM_L2_STATION_MPLS;
    }

    if (soc_mem_field32_get(unit, tcam_mem, entry,
                            IPV4_TERMINATION_ALLOWEDf)) {
        station->flags |= BCM_L2_STATION_IPV4;
    }

    if (soc_mem_field32_get(unit, tcam_mem, entry,
                            IPV6_TERMINATION_ALLOWEDf)) {
        station->flags |= BCM_L2_STATION_IPV6;
    }

    if (soc_mem_field32_get(unit, tcam_mem, entry,
                            ARP_RARP_TERMINATION_ALLOWEDf)) {
        station->flags |= BCM_L2_STATION_ARP_RARP;
    }

    if (soc_mem_field32_get(unit, tcam_mem, entry,
                            COPY_TO_CPUf)) {
        station->flags |= BCM_L2_STATION_COPY_TO_CPU;
    }

    if (soc_mem_field32_get(unit, tcam_mem, entry, DISCARDf)) {
        station->flags |= BCM_L2_STATION_DISCARD;
    }

    SC_UNLOCK(sc);

    return (rv);
}

/*
 * Function:
 *     bcm_th3_l2_station_size_get
 * Purpose:
 *     Get number of entries supported by L2 station API
 * Parameters:
 *     unit   - (IN) BCM device number
 *     size   - (OUT) Station entry ID
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_size_get(int unit, int *size)       
{
    _bcm_l2_station_control_t   *sc = NULL; /* Station control structure. */
    
    /* Input parameter check */
    if (size == NULL) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));
    SC_LOCK(sc);

    *size = sc->entries_total;

    SC_UNLOCK(sc);

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_th3_l2_station_entry_last_sid_set
 * Purpose:
 *     Set the current last station id for L2 station.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     overlay - (IN) Overlay/Underlay identification (not used in the rev)
 *     sid     - (IN) Station ID.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_station_entry_last_sid_set(int unit, int overlay, int sid)
{
    _bcm_l2_station_control_t *sc = NULL;

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    if (sid) {
        sc->last_allocated_sid = sid;
    } else {
        sc->last_allocated_sid = _BCM_L2_STATION_ID_BASE - 1;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_th3_l2_station_traverse
 * Purpose:
 *     Walk through station entries and executed user supplied callback function
 * Parameters:
 *     unit      - (IN) BCM device number
 *     trav_fn   - (IN) Caller supplied function to execute for each entry
 *     user_data - (IN) Argument list for the callback function
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_th3_l2_station_traverse(int unit, bcm_l2_station_traverse_cb trav_fn,
                            void *user_data) 
{
    _bcm_l2_station_control_t *sc = NULL; /* Station control structure */
    int                       index; /* Entry index */
    int                       count;
    int                       rv = BCM_E_NONE;
    _bcm_l2_station_entry_t   *s_ent = NULL;
    bcm_l2_station_t          l2_station;
    int                       station_id;

    /* Input parameter check */
    if (!trav_fn) {
        return BCM_E_PARAM;
    }
    
    memset((void *)(&l2_station), 0, sizeof(bcm_l2_station_t));
    
    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    count = sc->entries_total;
    
    for (index = 0; index < count; index++) {

        s_ent = sc->entry_arr[index]; /* H/W index */
        
        if (s_ent == NULL) {
            continue;
        }

        station_id = sc->entry_arr[index]->sid; /* Get S/W handle */
            
        rv = bcm_th3_l2_station_get(unit, station_id, &l2_station);

        if (BCM_FAILURE(rv)) {
            break;
        }
        
        rv = trav_fn(unit, &l2_station, user_data);

        if (BCM_FAILURE(rv)) {
            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                          "L2(unit %d) Info: Traverse (SID=%d)/(idx=%d) entry failed.\n"),
                       unit, station_id, index));
            break;
        }
    }

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */
/*
 * Function:
 *     _bcm_th3_l2_station_entry_reload
 * Purpose:
 *     Re-construct L2 station control structure software state
 *     from installed hardware entries
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_th3_l2_station_entry_reload(int unit, int overlay)
{
    _bcm_l2_station_control_t *sc;               /* Pointer to station        */
                                                 /* control structure.        */
    int                       rv;                /* Operation return status.  */
    uint32                    *tcam_buf = NULL;  /* Buffer to store TCAM      */
                                                 /* table entries.            */
    int                       entry_mem_size;    /* Size of TCAM table entry. */
    int                       index;             /* Table index iterator.     */
    _bcm_l2_station_entry_t   *s_ent_p;          /* Station table entry.      */
    my_station_tcam_entry_t   *my_station_ent;   /* Table entry pointer.      */


    entry_mem_size = sizeof(my_station_tcam_entry_t);

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_get(unit, &sc));

    SC_LOCK(sc);

    /* Allocate buffer to store the DMAed table entries. */
    tcam_buf = soc_cm_salloc(unit, entry_mem_size * sc->entries_total,
                              "STATION TCAM buffer");
    if (NULL == tcam_buf) {
        SC_UNLOCK(sc);
        return (BCM_E_MEMORY);
    }
    /* Initialize the entry buffer. */
    sal_memset(tcam_buf, 0, entry_mem_size * sc->entries_total);

    /* Read the table entries into the buffer. */
    rv = soc_mem_read_range(unit, MY_STATION_TCAMm, MEM_BLOCK_ALL,
                            0, (sc->entries_total - 1), tcam_buf);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Iterate over the table entries. */
    for (index = 0; index < sc->entries_total; index++) {

        my_station_ent = soc_mem_table_idx_to_pointer(unit, MY_STATION_TCAMm,
                             my_station_tcam_entry_t *, tcam_buf, index);

        if (0 == soc_MY_STATION_TCAMm_field32_get(unit, my_station_ent,
                                                  VALIDf)) {
            continue;
        }

        s_ent_p = (_bcm_l2_station_entry_t *)sal_alloc(
                                             sizeof(_bcm_l2_station_entry_t),
                                             "Sw L2 station entry");
        if (NULL == s_ent_p) {
            rv = BCM_E_MEMORY;
            goto cleanup;
        }

        sal_memset(s_ent_p, 0, sizeof(_bcm_l2_station_entry_t));

        s_ent_p->sid = ++sc->last_allocated_sid;
        s_ent_p->prio = (sc->entries_total - index);
        sc->entries_free--;
        sc->entry_count++;
        sc->entry_arr[index] = s_ent_p;
        
        s_ent_p->hw_index = index;
        s_ent_p->flags |= _BCM_L2_STATION_ENTRY_INSTALLED;
    }

cleanup:

    SC_UNLOCK(sc);

    if (tcam_buf) {
        soc_cm_sfree(unit, tcam_buf);
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_th3_l2_station_reload
 * Purpose:
 *     Re-construct L2 station control structure software state
 *     from installed hardware entries
 * Parameters:
 *     unit       - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_th3_l2_station_reload(int unit)
{
    int rv;

    rv = _bcm_th3_l2_station_entry_reload(unit, _BCM_L2_STATION_ENTRY_UNDERLAY);

    return rv;
}


#endif /* BCM_TRIUMPH_SUPPORT */
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_TOMAHAWK3_SUPPORT */
