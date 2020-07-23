/*

 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stg.c
 * Purpose:     Spanning tree group support
 *
 * Multiple spanning trees (MST) is supported on this chipset
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STG

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stg.h>
#include <bcm/vlan.h>

#include <bcm_int/control.h>
#include <bcm_int/common/lock.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/stg.h>
#include <bcm_int/dpp/vlan.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/alloc_mngr.h>

#include <soc/dpp/PPD/ppd_api_port.h>

#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

#define STG_DB_LOCK(unit) \
        sal_mutex_take(sw_state_sync_db[(unit)].dpp.stg_lock, sal_mutex_FOREVER)

#define STG_DB_UNLOCK(unit) sal_mutex_give(sw_state_sync_db[(unit)].dpp.stg_lock)

#define STG_CNTL(unit)  stg_info[(unit)]

#define STG_ACCESS  sw_state_access[unit].dpp.bcm.stg

#define STG_CHECK_INIT(unit)                                                \
    do {                                                                    \
        int _init;                                                          \
        uint8 _is_allocated;                                                \
        BCM_DPP_UNIT_CHECK(unit);                                           \
        if (unit >= BCM_MAX_NUM_UNITS) return BCM_E_UNIT;                   \
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.is_allocated(unit, &_is_allocated));  \
        if(!_is_allocated) {                                                \
            return BCM_E_INIT;                                              \
        }                                                                   \
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.init.get(unit, &_init));              \
        if (_init == FALSE) return BCM_E_INIT;                              \
        if (_init != TRUE) return _init;                                    \
    } while (0);

 
#define STG_CHECK_STG(_stg)                            \
    {\
        bcm_stg_t       _stg_min;\
        bcm_stg_t       _stg_max;\
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &_stg_min));\
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &_stg_max));\
        if ((_stg) < _stg_min || (_stg) > _stg_max) return BCM_E_BADID;\
    }

#define STG_CHECK_PORT(unit, port)                      \
    if ((port < 0)                                      \
        || (!IS_E_PORT((unit), (port))                  \
            && !IS_HG_PORT((unit), (port))              \
            && !IS_SPI_SUBPORT_PORT((unit), (port)))) { \
        return BCM_E_PORT;                              \
    }

/*
 * Allocation bitmap macros
 */
#define STG_BITMAP_SET(_stg)     BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_bitmap.bit_set(unit, _stg))
#define STG_BITMAP_CLR(_stg)     BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_bitmap.bit_clear(unit, _stg))

#define BCM_PETRA_STG_MIN           (BCM_DPP_STG_FROM_TOPOLOGY_ID(ARAD_PP_STP_TOPOLOGY_MIN))


static bcm_stg_t  _stg_default      = BCM_STG_DEFAULT;
static bcm_stg_t  _stg_min          = BCM_PETRA_STG_MIN;


int
_bcm_petra_stg_sw_dump(int unit)
{
    bcm_error_t     rv;
    bcm_stg_t       stg;
    bcm_vlan_t      vid;
    int             cnt, num_display_vids;
    int             init;
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;
    bcm_stg_t       stg_defl;
    int             stg_count;
    bcm_vlan_t      vlan_first;
    bcm_vlan_t      vlan_next;
    uint8             is_bit_set;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;

    num_display_vids = 8;


    rv = STG_ACCESS.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);
    if(!is_allocated) {
        LOG_CLI((BSL_META_U(unit,
                            "Unit %d STG not initialized\n"), unit));
        BCM_EXIT;
    }
    rv = STG_ACCESS.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);

    if (FALSE == init) {
        LOG_CLI((BSL_META_U(unit,
                            "Unit %d STG not initialized\n"), unit));
        BCM_EXIT;
    }

    rv = STG_ACCESS.stg_min.get(unit, &stg_min);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = STG_ACCESS.stg_max.get(unit, &stg_max);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = STG_ACCESS.stg_defl.get(unit, &stg_defl);
    BCMDNX_IF_ERR_EXIT(rv);
    rv = STG_ACCESS.stg_count.get(unit, &stg_count);
    BCMDNX_IF_ERR_EXIT(rv);

    LOG_CLI((BSL_META_U(unit,
                        "stg_min=%d stg_max=%d stg_default=%d allocated STGs=%d\n"),
             stg_min, stg_max, stg_defl, stg_count));
    LOG_CLI((BSL_META_U(unit,
                        "STG list:\nSTG :  VID list\n")));

    for (stg = stg_min; stg <= stg_max; stg++) {

        rv = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_bit_set) {
            LOG_CLI((BSL_META_U(unit,
                                "%4d: "), stg));

            cnt = 0;
            STG_ACCESS.vlan_first.get(unit, stg, &vlan_first);
            STG_ACCESS.vlan_next.get(unit, vlan_first, &vlan_next);
            for (vid = vlan_first;
                 vid != BCM_VLAN_NONE;
                 vid = vlan_next) {
                
                if (cnt < num_display_vids) {
                    if (cnt==0) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), vid));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), vid));
                    }
                }

                cnt++;
                rv = STG_ACCESS.vlan_next.get(unit, vid, &vlan_next);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            if (cnt > num_display_vids) {
                LOG_INFO(BSL_LS_BCM_STG,
                         (BSL_META_U(unit,
                                     "), ... %d more"), cnt - num_display_vids));
            }

            LOG_INFO(BSL_LS_BCM_STG,
                     (BSL_META_U(unit,
                                 "\n")));
        }
    }
    LOG_INFO(BSL_LS_BCM_STG,
             (BSL_META_U(unit,
                         "\n")));
    
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_stg_map_store(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;
    /* Same logic as in vlan module */
    rv = _bcm_petra_vlan_stg_set(unit, vid, stg);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_map_add
 * Purpose:
 *      Add VLAN to STG linked list
 */
STATIC int
_bcm_petra_stg_map_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_vlan_t      vlan_first;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    assert(BCM_VLAN_NONE != vid);

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, stg, &vlan_first));
    if ((vlan_first == vid) && (BCM_VLAN_NONE != vid)) {
        LOG_ERROR(BSL_LS_BCM_STG, (BSL_META_U(unit,
                  "For stg %d, vlan_next table index %d is not allowed to the same as its content!\n"), stg, vid));
        return BCM_E_INTERNAL;
    }
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.set(unit, vid, vlan_first));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.set(unit, stg, vid));

    BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_store(unit, stg, vid));

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit\n"), FUNCTION_NAME(), unit, stg, vid));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_map_delete
 * Purpose:
 *      Remove VLAN from STG linked list. No action if VLAN is not in list.
 */
STATIC int
_bcm_petra_stg_map_delete(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    int             array; /* pointing to: 0 - first, 1 - next */
    bcm_vlan_t      value;
    bcm_stg_t       index;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    assert(BCM_VLAN_NONE != vid);

    array = 0;
    index = stg;
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, index, &value));

    while (BCM_VLAN_NONE != value) {

        if (value == vid) {
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.get(unit, value, &value));
            if (array == 0) {
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.set(unit, index, value));
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.set(unit, vid, BCM_VLAN_NONE));
            }
            else if (array == 1) {
                if ((index == value) && (BCM_VLAN_NONE != value)) {
                    LOG_ERROR(BSL_LS_BCM_STG, (BSL_META_U(unit,
                              "For stg %d, vlan_next table index %d is not allowed to be the same as its content!\n"), stg, value));
                    return BCM_E_INTERNAL;
                }
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.set(unit, index, value));
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.set(unit, vid, BCM_VLAN_NONE));
            }
        } else {
            array = 1;
            index = value;
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.get(unit, index, &value));
        }
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_store(unit, stg, vid));

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit\n"), FUNCTION_NAME(), unit, stg, vid));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_map_get
 * Purpose:
 *      Get STG that a VLAN is mapped to.
 * Parameters:
 *      unit - device unit number.
 *      vid  - VLAN id to search for
 *      *stg - Spanning tree group id if found
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_stg_map_get(int unit, bcm_vlan_t vid, bcm_stg_t *stg)
{
    int             result = BCM_E_NOT_FOUND;
    int             index;
    bcm_vlan_t      vlan;
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, vid));

    /* this "internal" function is public so check parms */
    STG_CHECK_INIT(unit);

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));


    assert(BCM_VLAN_NONE != vid);
    *stg = 0;

    for (index = stg_min; index <= stg_max; index++) {

        BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, index, &vlan));

        while (BCM_VLAN_NONE != vlan) {
            if (vlan  == vid) {
                /* since a vlan may exist in only one STG, safe to exit */
                *stg = index;
                result = BCM_E_NONE;
                break;
            }

            BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.get(unit, vlan, &vlan));
        }

        if (BCM_E_NONE == result) {
            break;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, *stg, bcm_errmsg(result)));

    if (result != BCM_E_NOT_FOUND) {
        BCMDNX_IF_ERR_EXIT(result);
    } else {
        BCM_RETURN_VAL_EXIT(result);
    }
exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      _bcm_petra_stg_vid_compar
 * Purpose:
 *      Compare routine for sorting on VLAN ID.
 */
STATIC int
_bcm_petra_stg_vid_compare(void *a, void *b)
{
    uint16 a16, b16;

    a16 = *(uint16 *)a;
    b16 = *(uint16 *)b;

    return a16 - b16;
}

/*
 * Function:
 *      _bcm_ppd_stg_stp_port_set
 * Purpose:
 *      Set the spanning tree state for a port in specified STP topology id.
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 *      stp_state - Port STP state.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_ppd_stg_stp_port_set(int unit, bcm_stg_t stg_id, bcm_port_t port, int stp_state)
{
    SOC_PPC_PORT_STP_STATE
      port_stp_state;
    uint32
      soc_sand_rv,
      stp_topology_id;
    SOC_PPC_PORT
      soc_ppd_port;
    int core;
    uint32 flags;
    
    BCMDNX_INIT_FUNC_DEFS;
    
    switch(stp_state) {
    case BCM_STG_STP_BLOCK:
    case BCM_STG_STP_LISTEN:
        port_stp_state = SOC_PPC_PORT_STP_STATE_BLOCK;
        break;
    case BCM_STG_STP_LEARN:
        port_stp_state = SOC_PPC_PORT_STP_STATE_LEARN;
        break;
    case BCM_STG_STP_DISABLE: /* STP is disabled for this port - all traffic should go through */
    case BCM_STG_STP_FORWARD:
        port_stp_state = SOC_PPC_PORT_STP_STATE_FORWARD;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("stp_state is invalid")));
    }

    stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg_id);

    BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags)); 
    if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
    {
        BCM_EXIT;
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &soc_ppd_port, &core)));

    soc_sand_rv = soc_ppd_port_stp_state_set(unit, core, soc_ppd_port, stp_topology_id, port_stp_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_ppd_stg_stp_port_get(int unit, bcm_stg_t stg_id, bcm_port_t port, int *stp_state)
{
    SOC_PPC_PORT_STP_STATE
      port_stp_state;
    uint32
      soc_sand_rv,
      stp_topology_id;
    uint32  pp_port;
    int     core;
    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    
    stp_topology_id = BCM_DPP_STG_TO_TOPOLOGY_ID(stg_id);
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, port, &pp_port, &core)));

    soc_sand_rv = soc_ppd_port_stp_state_get(unit, core, pp_port, stp_topology_id, &port_stp_state);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    switch(port_stp_state) {
    case SOC_PPC_PORT_STP_STATE_BLOCK:
        *stp_state = BCM_STG_STP_BLOCK;
        break;
    case SOC_PPC_PORT_STP_STATE_LEARN:
        *stp_state = BCM_STG_STP_LEARN;
        break;
    case SOC_PPC_PORT_STP_STATE_FORWARD:
        *stp_state = BCM_STG_STP_FORWARD;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port stp_state is invalid")));
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_stp_get
 * Purpose:
 *      Retrieve the spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - Spanning tree group id.
 *      port      - device port number.
 *      stp_state - (OUT)Port STP state int the group.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_petra_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    bcm_error_t     rv;
    int             state = 0;
    uint8           is_member;
    
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, stg, port));

    /* Input parameters check. */
    STG_CHECK_PORT(unit, port);

    rv = STG_ACCESS.stg_enable.pbmp_member(unit, stg, port, &is_member);
    BCMDNX_IF_ERR_EXIT(rv);
    if (is_member) {
        rv = STG_ACCESS.stg_state_h.pbmp_member(unit, stg, port, &is_member);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_member) {
            state |= 0x2;
        }
        rv = STG_ACCESS.stg_state_l.pbmp_member(unit, stg, port, &is_member);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_member) {
            state |= 0x1;
        }
        switch (state) {
        case 0:  *stp_state = BCM_STG_STP_BLOCK;
            break;
        case 1:  *stp_state = BCM_STG_STP_LISTEN;
            break;
        case 2:  *stp_state = BCM_STG_STP_LEARN;
            break;
        case 3:  *stp_state = BCM_STG_STP_FORWARD;
            break;
        /* must default. Otherwise compilation error */
        /* coverity[dead_error_begin:FALSE] */
        default: *stp_state = BCM_STG_STP_DISABLE;
        }
    } else {
        *stp_state = BCM_STG_STP_DISABLE;
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, *stp_state, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_stp_set
 * Purpose:
 *      Set the spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - Spanning tree group id.
 *      port      - device port number.
 *      stp_state - (OUT)Port STP state int the group.
 * Returns:
 *      BCM_E_XXX
 */


#define _STG_VLAN_COUNT BCM_VLAN_COUNT

STATIC int
_bcm_petra_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    bcm_error_t     result = BCM_E_NOT_FOUND; /* local result code */
    
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, port, stp_state));

    /* Input parameters check. */
    STG_CHECK_PORT(unit, port);



    /* Update port with new STG */
    result = _bcm_ppd_stg_stp_port_set(unit, stg, port, stp_state);

    if (BCM_E_NONE == result) {
        if (stp_state == BCM_STG_STP_DISABLE) {
            result = STG_ACCESS.stg_enable.pbmp_port_remove(unit, stg, port);
            BCMDNX_IF_ERR_EXIT(result);
        } else {
            result = STG_ACCESS.stg_enable.pbmp_port_add(unit, stg, port);
            BCMDNX_IF_ERR_EXIT(result);
            
            if ((stp_state == BCM_STG_STP_LEARN) ||
                (stp_state == BCM_STG_STP_FORWARD)) {
                result = STG_ACCESS.stg_state_h.pbmp_port_add(unit, stg, port);
            } else {
                result = STG_ACCESS.stg_state_h.pbmp_port_remove(unit, stg, port);
            }
            BCMDNX_IF_ERR_EXIT(result);
            if ((stp_state == BCM_STG_STP_LISTEN) ||
                (stp_state == BCM_STG_STP_FORWARD)) {
                result = STG_ACCESS.stg_state_l.pbmp_port_add(unit, stg, port);
            } else {
                result = STG_ACCESS.stg_state_l.pbmp_port_remove(unit, stg, port);
            }
            BCMDNX_IF_ERR_EXIT(result);
        }
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_vlan_add
 * Purpose:
 *      Main part of bcm_petra_stg_vlan_add; assumes locks already acquired.
 * Parameters:
 *      unit    - device unit number.
 *      stg     - spanning tree group id.
 *      vid     - vlan to add
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The vlan is removed from it's current STG if necessary.
 */
STATIC int
_bcm_petra_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    bcm_stg_t       stg_cur;
    bcm_error_t     result = BCM_E_NOT_FOUND; /* local result code */
    int exists;
    uint8 is_bit_set;
        
    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

 
    
    exists = (bcm_dpp_am_l2_vpn_vsi_is_alloced(unit, vid) == BCM_E_EXISTS);   
    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    if (!exists) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("vid is not exist")));
    }
    if ((BCM_VLAN_DEFAULT > vid) || (SOC_DPP_DEFS_GET(unit, nof_vsi_lowers) <= vid)) {
        result = BCM_E_PARAM;    

    } else if (is_bit_set) { /* STG must already exist */

        /* Get the STG the VLAN is currently associated to */
        result = _bcm_petra_stg_map_get(unit, vid, &stg_cur);
        if ((BCM_E_NONE == result) || (BCM_E_NOT_FOUND == result)) {

            /* iff found, delete it */
            if (BCM_E_NONE == result) {
                BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_delete(unit, stg_cur, vid));
            }
        }

        /* Set the new STG */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_add(unit, stg, vid));

        result = BCM_E_NONE;
    } else {
        /* Not found in the specified STG */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
	}

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_stg_vlan_remove
 * Purpose:
 *      Main part of bcm_petra_stg_vlan_remove; assumes lock already acquired.
 * Parameters:
 *      unit    - device unit number.
 *      stg     - spanning tree group id.
 *      vid     - vlan id to remove
 *      destroy - boolean flag indicating the VLAN is being destroyed and is
 *                not to be added to the default STG. Also used internally
 *                to supress default STG assignment during transition.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_petra_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid, int destroy)
{
    int             stg_cur;
    bcm_error_t     result;
    bcm_stg_t       stg_defl;
    uint8             is_bit_set;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid, destroy));



    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    /* STG must already exist */
    if (is_bit_set) {

        /* Get the STG the VLAN is currently associated to */
        result = _bcm_petra_stg_map_get(unit, vid, &stg_cur);
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));

        if ((BCM_E_NONE == result) && ((stg == stg_cur))) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_delete(unit, stg, vid));

            /* If the VLAN is not being destroyed, set the VLAN to the default STG */
            if (FALSE == destroy) {
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
                BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_add(unit, stg_defl, vid));
            }
        }
        else if (BCM_E_NONE == result && (stg_cur == stg_defl)) {
            /* Not found in the specified STG */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found vsi(%d) in the specified STG(%d)"), vid, stg));
        }
        else if (BCM_E_NOT_FOUND == result) {
            result = BCM_E_NONE;
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The specified STG(%d) doesn't exist"), stg));
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, destroy, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_stg_vlan_destroy
 * Purpose:
 *      Remove a VLAN from a spanning tree group.
 *      The VLAN is NOT placed in the default spanning tree group.
 *      Intended for use when the VLAN is destroyed.
 * Parameters:
 *      unit - device unit number
 *          stg - STG ID to use
 *      vid - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_petra_stg_vlan_destroy(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    STG_CHECK_INIT(unit);
    STG_CHECK_STG(stg);

    STG_DB_LOCK(unit);

    rv = _bcm_petra_stg_vlan_remove(unit, stg, vid, TRUE);

    STG_DB_UNLOCK(unit);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_vlan_port_add
 * Purpose:
 *      Callout for vlan code to get a port that has been added to a vlan into
 *      the proper STP state
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_stg_vlan_port_add(int unit, bcm_vlan_t vid, bcm_port_t port)
{
    int         result = BCM_E_FAIL; /* local result code */
    bcm_stg_t   stg;
    int         state;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, vid, port));

    result = _bcm_petra_stg_map_get(unit, vid, &stg);

    if (BCM_E_NONE == result) {
        result = _bcm_petra_stg_stp_get(unit, stg, port, &state);
    }

    if (BCM_E_NONE == result) {
        result = _bcm_ppd_stg_stp_port_set(unit, stg, port, state);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, port, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_stg_vlan_port_remove
 * Purpose:
 *      Callout for vlan code to get a port that has been removed from a vlan
 *      into the proper STP state
 * Parameters:
 *      unit      - device unit number.
 *      vid       - VLAN id.
 *      port      - device port number.
 * Returns:
 *      BCM_E_NONE - This function is best effort
 */
int
_bcm_petra_stg_vlan_port_remove(int unit, bcm_vlan_t vid, bcm_port_t port)
{
    int         result = BCM_E_FAIL; /* local result code */
    bcm_stg_t   stg;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, vid, port));

    /* Get STG ID from vid */
    result = _bcm_petra_stg_map_get(unit, vid, &stg);

    if (BCM_E_NONE == result) {
        /* Remove Port from STG ID*/
        (void)_bcm_ppd_stg_stp_port_set(unit, stg, port, BCM_STG_STP_BLOCK);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, vid, port, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_stg_stp_init
 * Purpose:
 *      Initialize spanning tree group on a single device.
 * Parameters:
 *      unit - SOC unit number.
 *      stg  - Spanning tree group id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_petra_stg_stp_init(int unit, bcm_stg_t stg)
{
    bcm_error_t     rv;
    bcm_port_t      port;
    pbmp_t          ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));



    BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
    BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
    BCM_PBMP_ITER(ports_map, port) {
        /* place port in forward */
        rv = STG_ACCESS.stg_enable.pbmp_port_add(unit, stg, port);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = STG_ACCESS.stg_state_h.pbmp_port_add(unit, stg, port);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = STG_ACCESS.stg_state_l.pbmp_port_add(unit, stg, port);      
        BCMDNX_IF_ERR_EXIT(rv);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_list
 * Purpose:
 *      Return a list of VLANs in a specified Spanning Tree Group (STG).
 * Parameters:
 *      unit  - device unit number
 *      stg   - STG ID to list
 *      list  - Place where pointer to return array will be stored.
 *              Will be NULL if there are zero VLANs returned.
 *      count - Place where number of entries in array will be stored.
 *              Will be 0 if there are zero VLANs associated to the STG.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the Spanning Tree Group (STG) requested is not defined in the
 *      system, call returns NULL list and count of zero (0). The caller is
 *      responsible for freeing the memory that is returned, using
 *      bcm_petra_stg_vlan_list_destroy().
 */
int
bcm_petra_stg_vlan_list(int unit, bcm_stg_t stg, bcm_vlan_t **list,
                         int *count)
{
    bcm_vlan_t      vlan;
    int             index;
    bcm_error_t     result;
    uint8             is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, *) - Enter\n"), FUNCTION_NAME(), unit, stg));
    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    *list = NULL;
    *count = 0;

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE; 

    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    if (is_bit_set) {
        /* Traverse list once just to get an allocation count */
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, stg, &vlan));

        while (BCM_VLAN_NONE != vlan) {
            (*count)++;
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.get(unit, vlan, &vlan));
        }

        if (0 == *count) {
            result = BCM_E_NONE;
        }
        else {
            BCMDNX_ALLOC(*list, *count * sizeof (bcm_vlan_t), "bcm_petra_stg_vlan_list");

            if (NULL == *list) {
                result = BCM_E_MEMORY;
            }
            else {
                /* Traverse list a second time to record the VLANs */
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, stg, &vlan));
                index = 0;

                while (BCM_VLAN_NONE != vlan) {
                    (*list)[index++] = vlan;
                    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.get(unit, vlan, &vlan));
                }

                /* Sort the vlan list */
                _shr_sort(*list, *count, sizeof (bcm_vlan_t), _bcm_petra_stg_vid_compare);

                result = BCM_E_NONE;
            }
        }

    } else {
        result = BCM_E_NOT_FOUND;
    }
    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, *count, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_petra_stg_vlan_list.
 * Parameters:
 *      unit  - device unit number
 *      list  - Pointer to VLAN array to be destroyed.
 *      count - Number of entries in array.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_list_destroy(int unit, bcm_vlan_t *list, int count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Enter\n"), FUNCTION_NAME(), unit, count));

    if (NULL != list) {
       BCM_FREE(list);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, count, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_default_get
 * Purpose:
 *      Returns the default STG for the device.
 * Parameters:
 *      unit    - device unit number.
 *      stg_ptr - STG ID for default.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_default_get(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_t       stg_defl;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));

    *stg_ptr = stg_defl;

    STG_CHECK_STG(*stg_ptr);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *stg_ptr, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_default_set
 * Purpose:
 *      Changes the default STG for the device.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number.
 *      stg  - STG ID to become default.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The specified STG must already exist.
 */
int
bcm_petra_stg_default_set(int unit, bcm_stg_t stg)
{
    
    bcm_error_t     result;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    if (is_bit_set) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.set(unit, stg));
        result = BCM_E_NONE;
    } else {
        result = BCM_E_NOT_FOUND;
    }    

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);

exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_add
 * Purpose:
 *      Add a VLAN to a spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to use
 *      vid  - VLAN id to be added to STG
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Spanning tree group ID must have already been created. The
 *      VLAN is removed from the STG it is currently in.
 */
int
bcm_petra_stg_vlan_add(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    
    int             result = BCM_E_BADID;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    if ((vid >= BCM_VLAN_DEFAULT) && (vid < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers))) {
        
        STG_DB_LOCK(unit);
        result = _bcm_petra_stg_vlan_add(unit, stg, vid);
        STG_DB_UNLOCK(unit);

    } else {
        BCM_DPP_VLAN_CHK_ID(unit,vid);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_remove
 * Purpose:
 *      Remove a VLAN from a spanning tree group. The VLAN is placed in the
 *      default spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to use
 *      vid  - VLAN id to be removed from STG
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_remove(int unit, bcm_stg_t stg, bcm_vlan_t vid)
{
    
    int             result = BCM_E_BADID;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, vid));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    if ((vid >= BCM_VLAN_DEFAULT) && (vid < SOC_DPP_DEFS_GET(unit, nof_vsi_lowers))) {

        STG_DB_LOCK(unit);
        result = _bcm_petra_stg_vlan_remove(unit, stg, vid, FALSE);
        STG_DB_UNLOCK(unit);

    } else {
        BCM_DPP_VLAN_CHK_ID(unit,vid);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, vid, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_vlan_remove_all
 * Purpose:
 *      Remove all VLANs from a spanning tree group. The VLANs are placed in
 *      the default spanning tree group.
 * Parameters:
 *      unit - device unit number
 *      stg  - STG ID to clear
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_vlan_remove_all(int unit, bcm_stg_t stg)
{
    bcm_stg_t       stg_defl;
    bcm_error_t     result = BCM_E_NONE;
    bcm_vlan_t      vid;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));

    if (stg != stg_defl) {

        result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
        BCMDNX_IF_ERR_EXIT(result);
        if (!is_bit_set) {   /* STG must already exist */
            result = BCM_E_NOT_FOUND;
        } else {
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, stg, &vid));
            while (BCM_VLAN_NONE != vid) {
                result = _bcm_petra_stg_vlan_remove(unit, stg, vid, FALSE);
                if (BCM_E_NONE != result) {
                    break;
                }

                BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
                result = _bcm_petra_stg_vlan_add(unit, stg_defl, vid);
                if (BCM_E_NONE != result) {
                    break;
                }

                /*
                 * Iterate through list. The vid just removed was popped from
                 * the list. Use the new first.
                 */
                BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.get(unit, stg, &vid));
            }
        }
    }

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken){
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_clear
 * Description:
 *      Destroy all STGs
 * Parameters:
 *      unit - device unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_clear(int unit)
{
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;
    bcm_stg_t       stg;
    bcm_error_t     result = BCM_E_NONE;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);


    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));

    for (stg = stg_min; stg <= stg_max; stg++) {
        result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
        BCMDNX_IF_ERR_EXIT(result);
        if (is_bit_set) {
            STG_DB_UNLOCK(unit);
            stg_db_lock_taken = FALSE;
            /* ignore error code as unit will be 'init'ed later. */
            bcm_petra_stg_destroy(unit, stg);
            
            STG_DB_LOCK(unit);
            stg_db_lock_taken = TRUE;
        }
    }

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    result = bcm_petra_stg_init(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_create_id
 * Description:
 *      Create a STG, using a specified ID.
 * Parameters:
 *      unit - Device unit number
 *      stg -  STG to create
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      In the new STG, all ports are in the DISABLED state.
 */
int
bcm_petra_stg_create_id(int unit, bcm_stg_t stg)
{
    int             stg_count;
    bcm_error_t     result;
    bcm_port_t      port;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    if (!is_bit_set) {
        /* No device action needed */
        result = _bcm_petra_stg_stp_init(unit, stg);
        if (BCM_E_NONE == result) {
            STG_BITMAP_SET(stg);
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.get(unit, &stg_count));
            stg_count++;
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.set(unit, stg_count));
        }
        PBMP_E_ITER(unit, port) {
           (void)_bcm_ppd_stg_stp_port_set(unit, stg, port, BCM_STG_STP_FORWARD);
        }
    } else {
        result  = BCM_E_EXISTS;
    }

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_create
 * Description:
 *      Create a STG, picking an unused ID and returning it.
 * Parameters:
 *      unit - device unit number
 *      stg_ptr - (OUT) the STG ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_create(int unit, bcm_stg_t *stg_ptr)
{
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;
    bcm_stg_t       stg = BCM_STG_INVALID;
    bcm_error_t     result;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));

    for (stg = stg_min; stg <= stg_max; stg++) {
        result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
        BCMDNX_IF_ERR_EXIT(result);
        if (!is_bit_set) {
            break;          /* free id found */
        }
    }

    if (stg_max >= stg) {
        result = bcm_petra_stg_create_id(unit, stg);
    } else {
        result = BCM_E_FULL;
    } 

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    *stg_ptr = stg;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *stg_ptr, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_destroy
 * Description:
 *      Destroy an STG.
 * Parameters:
 *      unit - device unit number.
 *      stg  - The STG ID to be destroyed.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The default STG may not be destroyed.
 */
int
bcm_petra_stg_destroy(int unit, bcm_stg_t stg)
{
    bcm_stg_t       stg_defl;
    int             stg_count;
    int             result  = BCM_E_PARAM;
    uint8           stg_db_lock_taken = FALSE; 

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));

    if (stg_defl != stg) {
        /* The next call checks if STG exists as well as removing all VLANs */
        result = bcm_petra_stg_vlan_remove_all(unit, stg);

        if (BCM_E_NONE == result) {
            STG_BITMAP_CLR(stg);
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.get(unit, &stg_count));
            stg_count--;
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.set(unit, stg_count));
        }
    } else {
        result = BCM_E_NOT_FOUND;
    }

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_list
 * Purpose:
 *      Return a list of defined Spanning Tree Groups
 * Parameters:
 *      unit  - device unit number
 *      list  - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The caller is responsible for freeing the memory that is returned,
 *      using bcm_petra_stg_list_destroy().
 */
int
bcm_petra_stg_list(int unit, bcm_stg_t **list, int *count)
{
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;
    int             stg_count;    
    bcm_stg_t       stg;
    int             index = 0;
    bcm_error_t     result = BCM_E_NONE;
    uint8           is_bit_set;
    uint8           stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);


    STG_DB_LOCK(unit);
    stg_db_lock_taken = TRUE;

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.get(unit, &stg_count));

    *list = NULL;
    if (0 == stg_count) {
        *count = 0;
    } else {
        *count = stg_count;
        BCMDNX_ALLOC(*list, stg_count * sizeof (bcm_stg_t), "bcm_petra_stg_list");

        if (NULL == *list) {
            result = BCM_E_MEMORY;
        } else {
            for (stg = stg_min; stg <= stg_max; stg++) {
                result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
                BCMDNX_IF_ERR_EXIT(result);
                if (is_bit_set) {
                    assert(index < *count);
                    (*list)[index++] = stg;
                }
            }
        }
    }

    STG_DB_UNLOCK(unit);
    stg_db_lock_taken = FALSE;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *count, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_list_destroy
 * Purpose:
 *      Destroy a list returned by bcm_petra_stg_list.
 * Parameters:
 *      unit  - device unit number
 *      list  - Place where pointer to return array will be stored,
 *              which will be NULL if there are zero STGs returned.
 *      count - Place where number of entries in array will be stored,
 *              which will be 0 if there are zero STGs returned.
 * Returns:
 *      BCM_E_NONE
 */
int
bcm_petra_stg_list_destroy(int unit, bcm_stg_t *list, int count)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(count);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Enter\n"), FUNCTION_NAME(), unit, count));

    if (NULL != list) {
       BCM_FREE(list);
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, count, bcm_errmsg(BCM_E_NONE)));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_stp_set
 * Purpose:
 *      Set the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - STG ID.
 *      port      - device port number.
 *      stp_state - Spanning Tree State of port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_stp_set(int unit, bcm_stg_t stg, bcm_port_t port, int stp_state)
{
    
    bcm_error_t             result = BCM_E_NONE;
    bcm_port_t              local_port;
    _bcm_dpp_gport_info_t   gport_info;
    uint8                   is_bit_set;
    uint8                   stg_db_lock_taken = FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Enter\n"), FUNCTION_NAME(), unit, stg, port, stp_state));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

        
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info)); 

    PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
        STG_CHECK_PORT(unit, local_port);

        if ((stp_state < BCM_STG_STP_DISABLE) || (stp_state > BCM_STG_STP_FORWARD)){
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }

        STG_DB_LOCK(unit);
        stg_db_lock_taken = TRUE;

        result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
        BCMDNX_IF_ERR_EXIT(result);
        if (is_bit_set) {
            result = _bcm_petra_stg_stp_set(unit, stg, local_port, stp_state);
            if (result != BCM_E_NONE) {
                STG_DB_UNLOCK(unit);
                stg_db_lock_taken = FALSE;
                BCMDNX_IF_ERR_EXIT(result);
            }
        } else {
            /* Not found in the specified STG */
            STG_DB_UNLOCK(unit);
            stg_db_lock_taken = FALSE;
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
        }

        STG_DB_UNLOCK(unit);
        stg_db_lock_taken = FALSE;
    }

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, port, stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);

exit:
    if(stg_db_lock_taken) {
        STG_DB_UNLOCK(unit);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_stp_get
 * Purpose:
 *      Get the Spanning tree state for a port in specified STG.
 * Parameters:
 *      unit      - device unit number.
 *      stg       - STG ID.
 *      port      - device port number.
 *      stp_state - (Out) Pointer to where Spanning Tree State is stored.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_INTERNAL
 */
int
bcm_petra_stg_stp_get(int unit, bcm_stg_t stg, bcm_port_t port, int *stp_state)
{
    
    bcm_error_t             result = BCM_E_NONE;
    bcm_port_t              local_port;
    _bcm_dpp_gport_info_t   gport_info;
    uint8                   is_bit_set;
    BCMDNX_INIT_FUNC_DEFS;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, *) - Enter\n"), FUNCTION_NAME(), unit, stg, port));

    STG_CHECK_INIT(unit);

    STG_CHECK_STG(stg);

    if (BCM_GPORT_IS_SET(port)) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 

        local_port = gport_info.local_port;
    } else {
        local_port = port;
    }

    STG_CHECK_PORT(unit, local_port);

    STG_DB_LOCK(unit);

    result = STG_ACCESS.stg_bitmap.bit_get(unit, stg, &is_bit_set);
    BCMDNX_IF_ERR_EXIT(result);
    if (is_bit_set) {
        result = _bcm_petra_stg_stp_get(unit, stg, local_port, stp_state);
        if (result != BCM_E_NONE) {
            STG_DB_UNLOCK(unit);
            BCMDNX_IF_ERR_EXIT(result);
        }
    } else {
        /* Not found in the specified STG */
        STG_DB_UNLOCK(unit);    
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not found in the specified STG")));
    }

    STG_DB_UNLOCK(unit);    

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d, %d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, stg, local_port, *stp_state, bcm_errmsg(result)));

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *     bcm_petra_stg_count_get
 * Purpose:
 *     Get the maximum number of STG groups the device supports
 * Parameters:
 *     unit    - device unit number.
 *     max_stg - max number of STG groups supported by this unit
 * Returns:
 *     BCM_E_xxx
 */
int
bcm_petra_stg_count_get(int unit, int *max_stg)
{
    bcm_stg_t       stg_min;
    bcm_stg_t       stg_max;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, *) - Enter\n"), FUNCTION_NAME(), unit));

    STG_CHECK_INIT(unit);

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));

    *max_stg = stg_max - stg_min + 1;

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d, %d) - Exit(%s)\n"),
                          FUNCTION_NAME(), unit, *max_stg, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stg_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));
    if(sw_state_sync_db[unit].dpp.stg_lock) {
        sal_mutex_destroy(sw_state_sync_db[unit].dpp.stg_lock);
    }

    sw_state_sync_db[unit].dpp.stg_lock = NULL;


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_petra_stg_init(int unit)
{
    SOC_PPC_TRAP_CODE trap_code;
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO trap_info;
    uint32 soc_sand_rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    /* init drop trap to drop packets*/

    /* for block state */
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
    trap_code = SOC_PPC_TRAP_CODE_STP_STATE_BLOCK;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.processing_info.enable_learning = FALSE;
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    /* for learn state */
    SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(&trap_info);
    trap_code = SOC_PPC_TRAP_CODE_STP_STATE_LEARN;
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_get(unit,trap_code,&trap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    trap_info.strength = (SOC_DPP_CONFIG(unit)->pp.default_trap_strength);
    trap_info.processing_info.enable_learning = TRUE;
    trap_info.dest_info.frwrd_dest.dest_id = 0;
    SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &(trap_info.dest_info.frwrd_dest), soc_sand_rv);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    soc_sand_rv = soc_ppd_trap_frwrd_profile_info_set(unit,trap_code,&trap_info,BCM_CORE_ALL);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_stg_init
 * Description:
 *      Initialize the STG module according to Initial Configuration.
 * Parameters:
 *      unit - Device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_petra_stg_init(int unit)
{
    int                 sz_e, sz_sh, sz_sl, sz_vfirst;
    bcm_port_t          port;
    int                 result = BCM_E_NONE;
    bcm_stg_t           stg_max;
    bcm_stg_t           stg_min;
    bcm_stg_t           stg_defl;
    int                 stg_count;
    uint8               is_allocated;

    BCMDNX_INIT_FUNC_DEFS;


    BCM_DPP_UNIT_CHECK(unit);

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "%s(%d) - Enter\n"), FUNCTION_NAME(), unit));

    if (NULL == sw_state_sync_db[(unit)].dpp.stg_lock) {
        if (NULL == (sw_state_sync_db[(unit)].dpp.stg_lock = sal_mutex_create("soc_petra_stg_lock"))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("memory allocation failed")));
        }
    }

    if (SOC_WARM_BOOT(unit))
    {
        /* in this specific module it is OK to exit immediately in wb */
        BCM_EXIT;
    }

    /* Set the device properties */
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.alloc(unit));
    }
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.set(unit, _stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.set(unit, SOC_DPP_DEFS_GET(unit, nof_topology_ids)));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.set(unit, _stg_default));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.set(unit, 0));

    /* Get the device properties (for the assertion statement below) */
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_min.get(unit, &stg_min));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_max.get(unit, &stg_max));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.get(unit, &stg_count));

    assert(stg_defl >= stg_min && stg_defl <= stg_max &&
           stg_min <= stg_max);

    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_bitmap.alloc_bitmap(unit, stg_max+1));
    }
    /* array of port bitmaps indicating whether the port+stg has STP enabled,
     * 0 = BCM_STG_STP_DISABLE, 1= Enabled.
     */
    sz_sh = sz_sl = sz_e = (stg_max + 1);
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_enable.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_enable.alloc(unit, sz_e));
    }


    /* array of port bitmaps indicating STP state for the port+stg combo.
     * Only valid if stg_enable = TRUE. Use high (_h) and low (_l) for
     * each port to represent one of four states in bcm_stg_stp_t, i.e
     * BLOCK(_h,_l = 0,0), LISTEN(0,1), LEARN(1,0), FORWARD(1,1).
     */
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_state_h.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_state_h.alloc(unit, sz_sh));
    }

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_state_l.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_state_l.alloc(unit, sz_sl));
    }

    sz_vfirst = (stg_max + 1);
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_first.alloc(unit, sz_vfirst));
    }

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.is_allocated(unit, &is_allocated));
    if(!is_allocated) {
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.vlan_next.alloc(unit, SOC_DPP_DEFS_GET(unit, nof_vsi_lowers)));
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Set trap configuration */
        result = _bcm_petra_stg_init(unit);
        BCMDNX_IF_ERR_EXIT(result);
    }

    /*
     * Create default STG and add all VLANs to it.  Use private calls.
     * This creates a slight maintenance issue but allows delayed setting
     * of the init flag. This will prevent any public API functions
     * from executing.
     */
    BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
    if (!SOC_WARM_BOOT(unit)) {
        bcm_stg_t stg_cur;
	    result = _bcm_petra_stg_stp_init(unit, stg_defl);
	    BCMDNX_IF_ERR_EXIT(result);

        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
	    STG_BITMAP_SET(stg_defl);
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.get(unit, &stg_count));
	    stg_count++;
        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_count.set(unit, stg_count));

        BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
        result = _bcm_petra_stg_map_get(unit, BCM_VLAN_DEFAULT, &stg_cur);
        if ((BCM_E_NONE == result) || (BCM_E_NOT_FOUND == result)) {
            /* iff found, delete it */
            if (BCM_E_NONE == result) {
                BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_delete(unit, stg_cur, BCM_VLAN_DEFAULT));
            }
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_stg_map_add(unit, stg_defl, BCM_VLAN_DEFAULT));

        
        PBMP_E_ITER(unit, port) {
            BCMDNX_IF_ERR_EXIT(STG_ACCESS.stg_defl.get(unit, &stg_defl));
            (void)_bcm_ppd_stg_stp_port_set(unit, stg_defl, port, BCM_STG_STP_FORWARD);
        }
    }

    BCMDNX_IF_ERR_EXIT(STG_ACCESS.init.set(unit, TRUE));

    LOG_DEBUG(BSL_LS_BCM_STG,
              (BSL_META_U(unit,
                          "bcm_petra_stg_init: unit=%d rv=%d(%s)\n"),
                          unit, BCM_E_NONE, bcm_errmsg(BCM_E_NONE)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

