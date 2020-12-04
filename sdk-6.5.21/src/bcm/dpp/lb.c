/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM layer LB APIs
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_LB

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm/error.h>
#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/lb.h>
#include <bcm/lb.h>
#include <sal/core/alloc.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/dpp/mbcm.h>

#define _BCM_LB_ENABLE_CHECK(unit) {                                                                        \
    if (!SOC_IS_QAX(unit)) {                                                                                \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Link Bonding isn't support on this chip\n")));    \
    }                                                                                                       \
    if (0 == SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {                                              \
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("unit %d, Link Bonding is not enabled!\n"), unit)); \
    }                                                                                                       \
}
#define _BCM_LB_SYSTEM_LOCK(unit) \
    sal_mutex_take(lb_data[unit].lb_mutex, sal_mutex_FOREVER)
#define _BCM_LB_SYSTEM_UNLOCK(unit) \
    sal_mutex_give(lb_data[unit].lb_mutex)

typedef struct _bcm_lb_data_s {
    sal_mutex_t   lb_mutex;    /* LB mutex */
} _bcm_lb_data_t;

_bcm_lb_data_t lb_data[BCM_UNITS_MAX];

#ifdef BCM_LB_SUPPORT
int bcm_petra_lb_init(int unit)
{
    int rv = BCM_E_NONE;
    int is_coldboot = TRUE;
    bcm_modem_t modem_id;
    soc_port_t  port;
    _bcm_lb_data_t *plb_data;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        is_coldboot = FALSE;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    plb_data = &(lb_data[unit]);
    sal_memset(plb_data, 0, sizeof(_bcm_lb_data_t));

    plb_data->lb_mutex = sal_mutex_create("LB LOCK");
    if(plb_data->lb_mutex == NULL) {
        BCM_FREE(plb_data->lb_mutex);
        LOG_ERROR(BSL_LS_BCM_LB,
                  (BSL_META_U(unit,
                              "unit %d, Fail to allocate memory for lb_mutex\n"), unit));        
        return BCM_E_MEMORY;
    }

    if (is_coldboot) {
        if (LB_INFO_ACCESS.alloc(unit) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_LB,
                      (BSL_META_U(unit,
                                  "unit %d, failed to allocate warmboot lb memory\n"), unit));
        }

        BCMDNX_IF_ERR_EXIT(LB_INFO_ACCESS.lbg_valid_bmp.alloc_bitmap(unit, SOC_TMC_LB_NOF_LBG));

        for (modem_id = 0; modem_id < (SOC_TMC_LB_MODEM_MAX+1); modem_id++) {
            rv = LB_INFO_ACCESS.modem_to_ports.set(unit, modem_id, BCM_LB_PORT_INVALID);
            rv = LB_INFO_ACCESS.modem_to_ing_lbg.set(unit, modem_id, BCM_LB_LBG_INVALID);
            rv = LB_INFO_ACCESS.modem_to_egr_lbg.set(unit, modem_id, BCM_LB_LBG_INVALID);
        }
        for (port = 0; port < (SOC_MAX_NUM_PORTS+1); port++) {
            rv = LB_INFO_ACCESS.port_to_lbg.set(unit, port, BCM_LB_LBG_INVALID);
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_lb_detach(int unit)
{
    int rv = BCM_E_NONE;
    _bcm_lb_data_t *plb_data;

    rv = BCM_E_NONE;
    plb_data = &(lb_data[unit]);

    if (plb_data->lb_mutex != NULL) {
        sal_mutex_destroy(plb_data->lb_mutex);
		plb_data->lb_mutex = NULL;
    }

    return rv;
}

#endif /* BCM_LB_SUPPORT */

/* Connect Modem to logic port */
 int bcm_petra_lb_modem_to_port_map_set(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_gport_t port)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    bcm_port_t user_port;
    uint32 port_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, %s modem(%d) to port(%d)\n"), 
        unit, ((flags & BCM_LB_FLAG_UNMAP)?("disconnect"):("connect")), modem_id, port));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    if ((flags & (~BCM_LB_FLAG_UNMAP)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_LOCAL(port)) {
            user_port = BCM_GPORT_LOCAL_GET(port);       
        }
        else if (BCM_GPORT_IS_MODPORT(port)) {
            user_port = BCM_GPORT_MODPORT_PORT_GET(port);
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected port(0x%x)"), unit, port));    
        }
    }
    else {
        user_port = port;
    }
    if (!((SOC_PORT_VALID(unit, user_port)) && (IS_PORT(unit, user_port)) && (user_port < SOC_MAX_NUM_PORTS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected port(0x%x)"), unit, port));
    }

    /* check if nif port is used for LB */
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, user_port, &port_flags));
    if (!SOC_PORT_IS_LB_MODEM(port_flags)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("unit %d, unexpected port(0x%x)"), unit, port));
    }

    /* update HW */
    if (flags & BCM_LB_FLAG_UNMAP) {
        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_to_port_unmap_set,(unit, modem_id, port));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_to_port_unmap_set modem(%d) port(%d), error 0x%x\n"), 
                unit, modem_id, port, soc_rc));
        }
    }
    else {
        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_to_port_map_set,(unit, modem_id, port));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_to_port_map_set modem(%d) port(%d), error 0x%x\n"), 
                unit, modem_id, port, soc_rc));
        }  
    }

    /* update SW DB */
    if (flags & BCM_LB_FLAG_UNMAP) {
        port = BCM_LB_PORT_INVALID;
    }
    rv = LB_INFO_ACCESS.modem_to_ports.set(unit, modem_id, port);

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Connect Modem to logic port */
 int bcm_petra_lb_modem_to_port_map_get(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_gport_t *port)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    bcm_port_t user_port;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(port);

    /* get port from SW DB */
    rv = LB_INFO_ACCESS.modem_to_ports.get(unit, modem_id, &user_port);
    BCMDNX_IF_ERR_EXIT(rv);

    *port = user_port;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Enable or disable LBG */
 int bcm_petra_lb_enable_set(
    int unit, 
    bcm_lbg_t lbg_id, 
    uint32 flags, 
    uint32 enable)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, %s lbg(%d)\n"), unit, ((enable)? "enable":"disable"), lbg_id));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    /* check if lbg port is added */
    rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bit_val) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
            (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                             bcm_errmsg(BCM_E_CONFIG), lbg_id));
    }

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_lbg_enable_set,(unit, lbg_id, enable));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_lbg_enable_set lbg_id(%d) enable(%d), error 0x%x\n"), 
            unit, lbg_id, enable, soc_rc));
    } 

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Enable or disable LBG */
 int bcm_petra_lb_enable_get(
    int unit, 
    bcm_lbg_t lbg_id, 
    uint32 flags, 
    uint32 *enable)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    uint32 is_enable = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(enable);

    /* check if lbg port is added */
    rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bit_val) {
        *enable = 0;
        BCM_EXIT;
    }

    /* get info from HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_lbg_enable_get,(unit, lbg_id, &is_enable));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_lbg_enable_get lbg_id(%d), error 0x%x\n"), 
            unit, lbg_id, soc_rc));
    }
    *enable = is_enable;    

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Connect Modem to LBG */
 int bcm_petra_lb_modem_to_lbg_map_set(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lbg_t lbg_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    bcm_lbg_t lbg_id_ing = 0;
    bcm_lbg_t lbg_id_egr = 0;        

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, %s modem(%d) to lbg(%d) with direction(%d)\n"), 
        unit, ((flags & BCM_LB_FLAG_UNMAP)?("disconnect"):("connect")), modem_id, lbg_id, direction));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");
    BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    if ((flags & (~BCM_LB_FLAG_UNMAP)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    /* check if lbg port is added */
    rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bit_val) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
            (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                             bcm_errmsg(BCM_E_CONFIG), lbg_id));
    }

    if ((bcmLbDirectionRx == direction) || (bcmLbDirectionBoth == direction)) {
        rv = LB_INFO_ACCESS.modem_to_ing_lbg.get(unit, modem_id, &lbg_id_ing);
        BCMDNX_IF_ERR_EXIT(rv);

        if (flags & BCM_LB_FLAG_UNMAP) {
            if (lbg_id_ing == lbg_id) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_modem_to_lbg_unmap_set,(unit, modem_id, lbg_id));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_to_lbg_map_set modem(%d) lbg(%d), error 0x%x\n"), 
                        unit, modem_id, lbg_id, soc_rc));
                }

                rv = LB_INFO_ACCESS.modem_to_ing_lbg.set(unit, modem_id, BCM_LB_LBG_INVALID);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Fail(%s) modem_id(%d) don't connect to lbg(%d)\n"),
                                     bcm_errmsg(BCM_E_NOT_FOUND), modem_id, lbg_id));
            }
        }
        else {
            if (BCM_LB_LBG_INVALID == lbg_id_ing) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_modem_to_lbg_map_set,(unit, modem_id, lbg_id));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_to_lbg_map_set modem(%d) lbg(%d), error 0x%x\n"), 
                        unit, modem_id, lbg_id, soc_rc));
                }

                rv = LB_INFO_ACCESS.modem_to_ing_lbg.set(unit, modem_id, lbg_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else if (lbg_id != lbg_id_ing) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                    (_BSL_BCM_MSG("Fail(%s) modem_id(%d) already connect to lbg(%d)\n"),
                                     bcm_errmsg(BCM_E_EXISTS), modem_id, lbg_id_ing));
            }
        }
    }

    if ((bcmLbDirectionTx == direction) || (bcmLbDirectionBoth == direction)) {
        rv = LB_INFO_ACCESS.modem_to_egr_lbg.get(unit, modem_id, &lbg_id_egr);
        BCMDNX_IF_ERR_EXIT(rv);

        if (flags & BCM_LB_FLAG_UNMAP) {
            if (lbg_id_egr == lbg_id) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_to_lbg_unmap_set,(unit, modem_id, lbg_id));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_to_lbg_unmap_set modem(%d) lbg(%d), error 0x%x\n"), 
                        unit, modem_id, lbg_id, soc_rc));
                }

                rv = LB_INFO_ACCESS.modem_to_egr_lbg.set(unit, modem_id, BCM_LB_LBG_INVALID);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                    (_BSL_BCM_MSG("Fail(%s) modem_id(%d) don't connect to lbg(%d)\n"),
                                     bcm_errmsg(BCM_E_NOT_FOUND), modem_id, lbg_id));
            }
        }
        else {
            if (BCM_LB_LBG_INVALID == lbg_id_egr) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_to_lbg_map_set,(unit, modem_id, lbg_id));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_to_lbg_map_set modem(%d) lbg(%d), error 0x%x\n"), 
                        unit, modem_id, lbg_id, soc_rc));
                }

                rv = LB_INFO_ACCESS.modem_to_egr_lbg.set(unit, modem_id, lbg_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            else if (lbg_id != lbg_id_egr) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
                    (_BSL_BCM_MSG("Fail(%s) modem_id(%d) already connect to lbg(%d)\n"),
                                     bcm_errmsg(BCM_E_EXISTS), modem_id, lbg_id_egr));
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Connect Modem to LBG */
 int bcm_petra_lb_modem_to_lbg_map_get(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lbg_t *lbg_id)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    bcm_lbg_t lbg_id_ing = 0, lbg_id_egr = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");
    BCMDNX_NULL_CHECK(lbg_id);

    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    if ((bcmLbDirectionRx == direction) || (bcmLbDirectionBoth == direction)) {
        rv = LB_INFO_ACCESS.modem_to_ing_lbg.get(unit, modem_id, &lbg_id_ing);
        BCMDNX_IF_ERR_EXIT(rv);

        *lbg_id = lbg_id_ing;
    }

    if ((bcmLbDirectionTx == direction) || (bcmLbDirectionBoth == direction)) {
        rv = LB_INFO_ACCESS.modem_to_egr_lbg.get(unit, modem_id, &lbg_id_egr);
        BCMDNX_IF_ERR_EXIT(rv);

        *lbg_id = lbg_id_egr;
    }

    if (bcmLbDirectionBoth == direction) {
        if (lbg_id_ing != lbg_id_egr) {
            *lbg_id = BCM_LB_LBG_INVALID;
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("unit %d, modem(%d) is connected to lbg(%d) in rx and connected to lbg(%d) in tx, error 0x%x\n"), 
                unit, modem_id, lbg_id_ing, lbg_id_egr, soc_rc));
        }
        else {
            *lbg_id = lbg_id_ing;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set sequence number width */
 int bcm_petra_lb_sequence_number_width_set(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags,
    int number_width)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;
    bcm_lbg_t lbg_id_start = 0, lbg_id_end = 0, lbg_id_tmp = 0, lbg_id_egr = 0;
    bcm_modem_t modem_id_start = 0, modem_id_end = SOC_TMC_LB_MODEM_MAX, modem_id_tmp = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set seq number width with lbg(%d) direction(%d) flags(%x) number_width(%d)\n"), 
        unit, lbg_id, direction, flags, number_width));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        if (bcmLbDirectionRx == direction) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) setting sequence number width per LBG in RX side is not support\n"),
                                 bcm_errmsg(BCM_E_CONFIG)));
        }
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");

    /* check if lbg port is added */
    if (!is_global) {
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if ((bcmLbDirectionRx == direction) || (bcmLbDirectionBoth == direction)) {
        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_glb_seqeunce_num_width_set,(unit, number_width));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_glb_seqeunce_num_width_set number_width(%d), error 0x%x\n"), 
                unit, number_width, soc_rc));
        }
    }

    if ((bcmLbDirectionTx == direction) || (bcmLbDirectionBoth == direction)) {
        if (SOC_IS_QUX(unit)) {
            for (modem_id_tmp = modem_id_start; modem_id_tmp < (modem_id_end+1); modem_id_tmp++) {
                rv = LB_INFO_ACCESS.modem_to_egr_lbg.get(unit, modem_id_tmp, &lbg_id_egr);
                BCMDNX_IF_ERR_EXIT(rv);

                if (!is_global && (lbg_id_egr != lbg_id)) continue;

                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_seqeunce_num_width_set,(unit, modem_id_tmp, number_width));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_seqeunce_num_width_set modem(%d) modem(%d) number_width(%d), error 0x%x\n"), 
                        unit, modem_id_tmp, number_width, soc_rc));
                }
            }	
        }
        else {
            lbg_id_start = is_global ? 0 : lbg_id;
            lbg_id_end   = is_global ? SOC_TMC_LB_LBG_MAX : lbg_id;

            for (lbg_id_tmp = lbg_id_start; lbg_id_tmp < (lbg_id_end+1); lbg_id_tmp++) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_seqeunce_num_width_set,(unit, lbg_id_tmp, number_width));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_seqeunce_num_width_set modem(%d) lbg(%d) number_width(%d), error 0x%x\n"), 
                        unit, lbg_id_tmp, number_width, soc_rc));
                }
            }
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get sequence number width */
 int bcm_petra_lb_sequence_number_width_get(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags,
    int *number_width)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;
    bcm_lbg_t lbg_id_start = 0, lbg_id_end = 0, lbg_id_tmp = 0, lbg_id_egr = 0;
    int num_width_ing = -1, num_width_egr_first = -1, num_width_egr_tmp = -1;
	bcm_modem_t modem_id_start = 0, modem_id_end = SOC_TMC_LB_MODEM_MAX, modem_id_tmp = 0, modem_id_first = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        if (bcmLbDirectionRx == direction) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) setting sequence number width per LBG in RX side is not support\n"),
                                 bcm_errmsg(BCM_E_CONFIG)));
        }
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");

    /* check if lbg port is added */
    if (!is_global) {
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if ((bcmLbDirectionRx == direction) || (bcmLbDirectionBoth == direction)) {
        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_glb_seqeunce_num_width_get,(unit, &num_width_ing));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_glb_seqeunce_num_width_get, error 0x%x\n"), 
                unit, soc_rc));
        }
        *number_width = num_width_ing;
    }

    if ((bcmLbDirectionTx == direction) || (bcmLbDirectionBoth == direction)) {
        if (SOC_IS_QUX(unit)) {
            for (modem_id_tmp = modem_id_start; modem_id_tmp < (modem_id_end+1); modem_id_tmp++) {
                rv = LB_INFO_ACCESS.modem_to_egr_lbg.get(unit, modem_id_tmp, &lbg_id_egr);
                BCMDNX_IF_ERR_EXIT(rv);

                if (!is_global && (lbg_id_egr != lbg_id)) continue;

                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_seqeunce_num_width_get,(unit, modem_id_tmp, &num_width_egr_tmp));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_seqeunce_num_width_get modem(%d) modem(%d) number_width(%d), error 0x%x\n"), 
                        unit, modem_id_tmp, number_width, soc_rc));
                }

                if (num_width_egr_first == -1) {
                    num_width_egr_first = num_width_egr_tmp;
                    modem_id_first = modem_id_tmp;
                }
                else {
                    if (num_width_egr_first != num_width_egr_tmp) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("unit %d, number_width for modem(%d) is %d and for modem(%d) is %d, error 0x%x\n"), 
                            unit, modem_id_first, num_width_egr_first, modem_id_tmp, num_width_egr_tmp, soc_rc));
                    }
                }
            }		
        }
        else {
            lbg_id_start = is_global ? 0 : lbg_id;
            lbg_id_end   = is_global ? SOC_TMC_LB_LBG_MAX : lbg_id;

            for (lbg_id_tmp = lbg_id_start; lbg_id_tmp < (lbg_id_end+1); lbg_id_tmp++) {
                _BCM_LB_SYSTEM_LOCK(unit);
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_seqeunce_num_width_get,(unit, lbg_id_tmp, &num_width_egr_tmp));
                _BCM_LB_SYSTEM_UNLOCK(unit);
                if (SOC_FAILURE(soc_rc)) {
                    BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_seqeunce_num_width_set lbg(%d), error 0x%x\n"), 
                        unit, lbg_id_tmp, soc_rc));
                }

                if (num_width_egr_first == -1) {
                    num_width_egr_first = num_width_egr_tmp;
                }
                else {
                    if (num_width_egr_first != num_width_egr_tmp) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("unit %d, number_width for lbg(%d) is %d and for lbg(%d) is %d, error 0x%x\n"), 
                            unit, 0, num_width_egr_first, lbg_id_tmp, num_width_egr_tmp, soc_rc));
                    }
                }
            }
        }
        *number_width = num_width_egr_first;        
    }

    if (bcmLbDirectionBoth == direction) {
        if (num_width_ing != num_width_egr_first) {
            *number_width = 0;
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("unit %d, number_width in rx is %d and number_width in tx is %d, error 0x%x\n"), 
                unit, num_width_ing, num_width_egr_first, soc_rc));
        }
        else {
            *number_width = num_width_ing;
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set configuration */
 int bcm_petra_lb_control_set(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_control_t lb_control, 
    int arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;
    soc_lb_ing_reorder_config_t reorder_config;
    soc_lb_global_packet_config_t soc_glb_pkt;
    soc_lb_lbg_packet_config_t    soc_lbg_pkt;
    soc_lb_segment_mode_t         soc_segment_mode;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set lb control width with lbg(%d) direction(%d) flags(%x) lb_control(%d) arg(%d)\n"), 
        unit, lbg_id, direction, flags, lb_control, arg));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");

    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        
        /* check if lbg port is added */    
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if (is_global) {
        switch (lb_control) {
        case bcmLbControlSegmentationMode:
            if (SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QAX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            BCM_LB_VALUE_CHECK(arg, bcmLbSegmentMode128, (bcmLbSegmentModeCount - 1), "arg");
            soc_segment_mode = arg;

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_segment_config_set,(unit, SOC_TMC_LB_GLOBAL, soc_segment_mode));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_segment_config_set mode(%d), error 0x%x\n"), 
                    unit, arg, soc_rc));
            }
            break;
        case bcmLbControlPacketCrcEnable:
            if (SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QAX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_get,(unit, &soc_glb_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            soc_glb_pkt.packet_crc_enable = (arg ? 1 : 0);

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_set,(unit, &soc_glb_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_set, error 0x%x\n"), 
                    unit, soc_rc));
            }

            break;
        case bcmLbControlNofTotalSharedBuffers:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_total_shared_buffer_config_set,(unit, arg));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_total_shared_buffer_config_set, error 0x%x\n"), 
                    unit, soc_rc));
            }

            break;
        default:
            if (lb_control < bcmLbControlCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, lb_control));
        }
    }
    else {  /* per LBG */
        switch (lb_control) {
        case bcmLbControlTimeoutThreshold:
        case bcmLbControlMaxOutOfOrder:
        case bcmLbControlMaxBuffer:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_reorder_config_get,(unit, lbg_id, &reorder_config));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_reorder_config_get lbg_id(%d), error 0x%x\n"), 
                    unit, lbg_id, soc_rc));
            }

            if (bcmLbControlTimeoutThreshold == lb_control) {
                reorder_config.timeout_thresh = arg;
            }
            else if (bcmLbControlMaxOutOfOrder == lb_control) {
                reorder_config.max_out_of_order = arg;
            }
            else if (bcmLbControlMaxBuffer == lb_control) {
                reorder_config.max_buffer = arg;
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_reorder_config_set,(unit, lbg_id, &reorder_config));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_reorder_config_set lbg_id(%d), error 0x%x\n"), 
                    unit, lbg_id, soc_rc));
            }
            break;
        case bcmLbControlExpectedSequenceNumber:
            _BCM_LB_SYSTEM_LOCK(unit);
            if (bcmLbDirectionRx == direction) {
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_expected_seqeunce_num_set,(unit, lbg_id, arg));
            }
            else {
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_expected_seqeunce_num_set,(unit, lbg_id, arg));
            }
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing/egr_guaranteed_buffer_config_set, error 0x%x\n"), 
                    unit, soc_rc));
            }
            break;
        case bcmLbControlNofGuaranteedBuffers:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_guaranteed_buffer_config_set,(unit, lbg_id, arg));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_guaranteed_buffer_config_set, error 0x%x\n"), 
                    unit, soc_rc));
            }
            break;
        case bcmLbControlSegmentationMode:
            if (!SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QUX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            BCM_LB_VALUE_CHECK(arg, bcmLbSegmentMode128, (bcmLbSegmentModeCount - 1), "arg");
            soc_segment_mode = arg;

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_segment_config_set,(unit, lbg_id, soc_segment_mode));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_segment_config_set mode(%d), error 0x%x\n"), 
                    unit, arg, soc_rc));
            }
            break;
        case bcmLbControlPacketCrcEnable:
            if (!SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QUX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_lbg_packet_config_get,(unit, lbg_id, &soc_lbg_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_lbg_packet_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            soc_lbg_pkt.packet_crc_enable = (arg ? 1 : 0);

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_lbg_packet_config_set,(unit, lbg_id, &soc_lbg_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_lbg_packet_config_set, error 0x%x\n"), 
                    unit, soc_rc));
            }
            break;
        default:
            if (lb_control < bcmLbControlCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, lb_control));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set configuration */
 int bcm_petra_lb_control_get(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_control_t lb_control, 
    int *arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;
    soc_lb_ing_reorder_config_t soc_reorder_config;
    soc_lb_segment_mode_t segment_mode;
    soc_lb_global_packet_config_t soc_glb_pkt;
    soc_lb_lbg_packet_config_t    soc_lbg_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");
    BCMDNX_NULL_CHECK(arg);

    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        
        /* check if lbg port is added */    
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if (is_global) {
        switch (lb_control) {
        case bcmLbControlSegmentationMode:
            if (SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QAX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            segment_mode = socLbSegmentMode128;
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_segment_config_get,(unit, SOC_TMC_LB_GLOBAL, &segment_mode));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_segment_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            *arg = segment_mode;
            break;
        case bcmLbControlPacketCrcEnable:
            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            sal_memset(&soc_glb_pkt, 0, sizeof(soc_glb_pkt));
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_get,(unit, &soc_glb_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            *arg = soc_glb_pkt.packet_crc_enable;
            break;
        case bcmLbControlNofTotalSharedBuffers:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_total_shared_buffer_config_get,(unit, arg));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_total_shared_buffer_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            break;
        default:
            if (lb_control < bcmLbControlCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, lb_control));
        }
    }
    else {  /* per LBG */
        switch (lb_control) {
        case bcmLbControlTimeoutThreshold:
        case bcmLbControlMaxOutOfOrder:
        case bcmLbControlMaxBuffer:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            sal_memset(&soc_reorder_config, 0, sizeof(soc_reorder_config));
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_reorder_config_get,(unit, lbg_id, &soc_reorder_config));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_reorder_config_get lbg_id(%d), error 0x%x\n"), 
                    unit, lbg_id, soc_rc));
            }

            if (bcmLbControlTimeoutThreshold == lb_control) {
                *arg = soc_reorder_config.timeout_thresh;
            }
            else if (bcmLbControlMaxOutOfOrder == lb_control) {
                *arg = soc_reorder_config.max_out_of_order;
            }
            else if (bcmLbControlMaxBuffer == lb_control) {
                *arg = soc_reorder_config.max_buffer;
            }

            break;
        case bcmLbControlExpectedSequenceNumber:
            _BCM_LB_SYSTEM_LOCK(unit);
            if (bcmLbDirectionRx == direction) {
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_expected_seqeunce_num_get,(unit, lbg_id, arg));
            }
            else {
                soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_expected_seqeunce_num_get,(unit, lbg_id, arg));
            }
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing/egr_expected_seqeunce_num_get, error 0x%x\n"), 
                    unit, soc_rc));
            }
            break;
        case bcmLbControlNofGuaranteedBuffers:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in rx: %d"), unit, lb_control));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_guaranteed_buffer_config_get,(unit, lbg_id, arg));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_guaranteed_buffer_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }
            break;
        case bcmLbControlSegmentationMode:
            if (!SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QUX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            segment_mode = socLbSegmentMode128;
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_segment_config_get,(unit, lbg_id, &segment_mode));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_segment_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            *arg = segment_mode;
            break;
        case bcmLbControlPacketCrcEnable:
            if (!SOC_IS_QUX(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("unit %d, control only supported on QUX: %d"), unit, lb_control));
            }

            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, lb_control));
            }

            sal_memset(&soc_lbg_pkt, 0, sizeof(soc_lbg_pkt));
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_lbg_packet_config_get,(unit, lbg_id, &soc_lbg_pkt));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_lbg_packet_config_get, error 0x%x\n"), 
                    unit, soc_rc));
            }

            *arg = soc_lbg_pkt.packet_crc_enable;
            break;
        default:
            if (lb_control < bcmLbControlCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, lb_control));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set configuration */
 int bcm_petra_lb_modem_control_set(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_modem_control_t modem_control, 
    int arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_shaper_config_t soc_shaper_config;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set modem control width with modem(%d) direction(%d) flags(%x) modem_control(%d) arg(%d)\n"), 
        unit, modem_id, direction, flags, modem_control, arg));

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    
    switch (modem_control) {
    case bcmLbModemControlHeaderCompensation:
        if (bcmLbDirectionTx != direction) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, modem_control));
        }

        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_get,(unit, modem_id, &soc_shaper_config));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_get modem_id(%d), error 0x%x\n"), 
                unit, modem_id, soc_rc));
        }

        soc_shaper_config.hdr_compensation = arg;

        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_set,(unit, modem_id, &soc_shaper_config));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_set modem_id(%d), error 0x%x\n"), 
                unit, modem_id, soc_rc));
        }

        break;
    default:
        if (modem_control < bcmLbModemControlCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, modem_control));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get configuration */
 int bcm_petra_lb_modem_control_get(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_modem_control_t modem_control, 
    int *arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_shaper_config_t soc_shaper_config;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(arg);
    
    switch (modem_control) {
    case bcmLbModemControlHeaderCompensation:
        if (bcmLbDirectionTx != direction) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control only supported in tx: %d"), unit, modem_control));
        }

        sal_memset(&soc_shaper_config, 0, sizeof(soc_shaper_config));
        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_get,(unit, modem_id, &soc_shaper_config));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_get modem_id(%d), error 0x%x\n"), 
                unit, modem_id, soc_rc));
        }

        *arg = soc_shaper_config.hdr_compensation;
        break;
    default:
        if (modem_control < bcmLbModemControlCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, control not supported: %d"), unit, modem_control));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get status of LBG or global */
 int bcm_petra_lb_status_get(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_status_type_t status_type, 
    int *value)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");

    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        
        /* check if lbg port is added */    
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if (is_global) {
        if (status_type < bcmLbStatusCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type not supported: %d"), unit, status_type));
    }
    else {  /* per LBG */
        switch (status_type) {
        case bcmLbStatusIsInSync:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type only supported in rx: %d"), unit, status_type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_status_get,(unit, lbg_id, status_type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_status_get lbg_id(%d) status_type(%d), error 0x%x\n"), 
                    unit, lbg_id, status_type, soc_rc));
            }
            break;
        case bcmLbStatusIsEmptyFifo:
            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type only supported in tx: %d"), unit, status_type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_status_get,(unit, lbg_id, status_type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_status_get lbg_id(%d) status_type(%d), error 0x%x\n"), 
                    unit, lbg_id, status_type, soc_rc));
            }
            break;
        default:
            if (status_type < bcmLbStatusCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type not supported: %d"), unit, status_type));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get status of LBG or global */
 int bcm_petra_lb_modem_status_get(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_modem_status_type_t status_type, 
    int *value)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionTx, "direction");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    switch (status_type) {
    case bcmLbModemStatusIsEmptyFifo:
        if (bcmLbDirectionRx != direction) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type only supported in rx: %d"), unit, status_type));
        }

        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_modem_status_get,(unit, modem_id, status_type, value));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_status_get modem_id(%d) status_type(%d), error 0x%x\n"), 
                unit, modem_id, status_type, soc_rc));
        }

        break;
    default:
        if (status_type < bcmLbModemStatusCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type not supported: %d"), unit, status_type));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

    return rv;
}

/* Get stat of LBG or global */
 int bcm_petra_lb_stat_get(
    int unit, 
    bcm_lbg_t lbg_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_stat_val_t type, 
    uint64 *value)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    uint8 bit_val = 0;
    int is_global = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionBoth, "direction");

    if (flags & BCM_LB_FLAG_GLOBAL) {
        is_global = 1;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        
        /* check if lbg port is added */    
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
    }

    if (is_global) {
        lbg_id = SOC_TMC_LB_GLOBAL;

        switch (type) {
        case bcmLbStatsDiscardFragments:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type only supported in rx: %d"), unit, type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_glb_stat_get,(unit, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_glb_stat_get type(%d), error 0x%x\n"), 
                    unit, type, soc_rc));
            }
            break;
        case bcmLbStatsErrorPkts:
            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type only supported in tx: %d"), unit, type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_stat_get,(unit, lbg_id, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_stat_get lbg_id(%d) type(%d), error 0x%x\n"), 
                    unit, lbg_id, type, soc_rc));
            }
            break;
        default:
            if (type < bcmLbStatsCount) {
                return BCM_E_UNAVAIL;
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type not supported: %d"), unit, type));
        }
    }
    else {  /* per LBG */
        switch (type) {
        case bcmLbStatsUnicastPkts:
        case bcmLbStatsMulticastPkts:
        case bcmLbStatsBroadcastPkts:
        case bcmLbStatsPkts64Octets:
        case bcmLbStatsPkts65to127Octets:
        case bcmLbStatsPkts128to255Octets:
        case bcmLbStatsPkts256to511Octets:
        case bcmLbStatsPkts512to1023Octets:
        case bcmLbStatsPkts1024to1518Octets:
        case bcmLbStatsPkts1519Octets:
        case bcmLbStatsPkts:
        case bcmLbStatsOctets:         
            if (bcmLbDirectionTx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type only supported in tx: %d"), unit, type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_stat_get,(unit, lbg_id, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_stat_get lbg_id(%d) type(%d), error 0x%x\n"), 
                    unit, lbg_id, type, soc_rc));
            }
            break;
        case bcmLbStatsDiscardFragments:
            if (bcmLbDirectionRx != direction) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type only supported in rx: %d"), unit, type));
            }

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_lbg_stat_get,(unit, lbg_id, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_lbg_stat_get lbg_id(%d) type(%d), error 0x%x\n"), 
                    unit, lbg_id, type, soc_rc));
            }
            break;
        default:
            if (type < bcmLbStatsCount) {
                return BCM_E_UNAVAIL;
            }

            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type not supported: %d"), unit, type));
        }
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get stat of Modem */
 int bcm_petra_lb_modem_stat_get(
    int unit, 
    bcm_modem_t modem_id, 
    bcm_lb_direction_type_t direction, 
    uint32 flags, 
    bcm_lb_modem_stat_val_t type, 
    uint64 *value)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    BCM_LB_VALUE_MAX_CHECK(direction, bcmLbDirectionTx, "direction");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    switch (type) {
    case bcmLbModemStatsPkts:
    case bcmLbModemStatsOctets:
        if (bcmLbDirectionRx == direction) {
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_modem_stat_get,(unit, modem_id, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_stat_get modem_id(%d) type(%d), error 0x%x\n"), 
                    unit, modem_id, type, soc_rc));
            }
        }
        else if (bcmLbDirectionTx == direction) {
            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_stat_get,(unit, modem_id, type, value));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_stat_get modem_id(%d) type(%d), error 0x%x\n"), 
                    unit, modem_id, type, soc_rc));
            }
        }
        break;
    default:
        if (type < bcmLbModemStatsCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, type not supported: %d"), unit, type));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set mapping of port to modem */
 int bcm_petra_lb_rx_modem_map_set(
    int unit, 
    bcm_lb_rx_modem_map_index_t *map_index, 
    uint32 flags, 
    bcm_lb_rx_modem_map_config_t *map_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    bcm_port_t user_port, mapped_port;
    soc_lb_rx_modem_map_index_t soc_map_index;
    soc_lb_rx_modem_map_config_t soc_map_config;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCMDNX_NULL_CHECK(map_index);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(map_config);
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set rx modem map with port(%d) vlan(%d) flags(%x) lbg_type(%x) modem(%d)\n"), 
        unit, map_index->port, map_index->vlan, flags, map_config->lbg_type, map_config->modem_id));

    if (BCM_GPORT_IS_SET(map_index->port)) {
        if (BCM_GPORT_IS_LOCAL(map_index->port)) {
            user_port = BCM_GPORT_LOCAL_GET(map_index->port);       
        }
        else if (BCM_GPORT_IS_MODPORT(map_index->port)) {
            user_port = BCM_GPORT_MODPORT_PORT_GET(map_index->port);
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected map_index->port(0x%x)"), unit, (map_index->port)));    
        }
    }
    else {
        user_port = map_index->port;
    }
    if (!((SOC_PORT_VALID(unit, user_port)) && (IS_PORT(unit, user_port)) && (user_port < SOC_MAX_NUM_PORTS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected map_index->port(0x%x)"), unit, (map_index->port)));
    }

    BCM_LB_VALUE_MAX_CHECK(map_config->lbg_type, (bcmLbFormatCount-1), "map_config->lbg_type");
    BCM_LB_VALUE_MAX_CHECK(map_config->modem_id, SOC_TMC_LB_MODEM_MAX, "map_config->modem_id");

    /* check if modem can be connected to port */
    rv = LB_INFO_ACCESS.modem_to_ports.get(unit, map_config->modem_id, &mapped_port);
    BCMDNX_IF_ERR_EXIT(rv);

    if (mapped_port != user_port) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
            (_BSL_BCM_MSG("Fail(%s) modem(%d) can't be connect to port(0x%x)\n"),
                             bcm_errmsg(BCM_E_CONFIG), (map_config->modem_id), (map_index->port)));
    }

    /* update info */
    soc_map_index.port = map_index->port;
    soc_map_index.vlan = map_index->vlan;
    soc_map_config.lbg_type = map_config->lbg_type;
    soc_map_config.modem_id = map_config->modem_id;

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_packet_to_modem_map_set,(unit, &soc_map_index, &soc_map_config));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_map_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Get mapping of port to modem */
 int bcm_petra_lb_rx_modem_map_get(
    int unit, 
    bcm_lb_rx_modem_map_index_t *map_index, 
    uint32 flags, 
    bcm_lb_rx_modem_map_config_t *map_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    bcm_port_t user_port;
    soc_lb_rx_modem_map_index_t soc_map_index;
    soc_lb_rx_modem_map_config_t soc_map_config;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCMDNX_NULL_CHECK(map_index);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(map_config);

    if (BCM_GPORT_IS_SET(map_index->port)) {
        if (BCM_GPORT_IS_LOCAL(map_index->port)) {
            user_port = BCM_GPORT_LOCAL_GET(map_index->port);       
        }
        else if (BCM_GPORT_IS_MODPORT(map_index->port)) {
            user_port = BCM_GPORT_MODPORT_PORT_GET(map_index->port);
        }
        else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected map_index->port(0x%x)"), unit, (map_index->port)));    
        }
    }
    else {
        user_port = map_index->port;
    }
    if (!((SOC_PORT_VALID(unit, user_port)) && (IS_PORT(unit, user_port)) && (user_port < SOC_MAX_NUM_PORTS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, unexpected map_index->port(0x%x)"), unit, (map_index->port)));
    }

    /* get info from HW */
    soc_map_index.port = map_index->port;
    soc_map_index.vlan = map_index->vlan;
    sal_memset(&soc_map_config, 0, sizeof(soc_map_config));

    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_packet_to_modem_map_get,(unit, &soc_map_index, &soc_map_config));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_modem_map_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    map_config->lbg_type = soc_map_config.lbg_type;
    map_config->modem_id = soc_map_config.modem_id;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set packet configuration */
 int bcm_petra_lb_packet_config_set(
    int unit, 
    uint32 flags, 
    bcm_lb_packet_config_t *packet_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_global_packet_config_t soc_glb_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(packet_config);
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set rx modem map with flags(%x) outer_vlan_tpid(%x) lbg_tpid(%x)\n"), 
        unit, flags, packet_config->outer_vlan_tpid, packet_config->lbg_tpid));

    /* get info from HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_get,(unit, &soc_glb_pkt));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    soc_glb_pkt.lb_tpid = packet_config->lbg_tpid;
    soc_glb_pkt.outer_vlan_tpid = packet_config->outer_vlan_tpid;

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_set,(unit, &soc_glb_pkt));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set packet configuration */
 int bcm_petra_lb_packet_config_get(
    int unit, 
    uint32 flags, 
    bcm_lb_packet_config_t *packet_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_global_packet_config_t soc_glb_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(packet_config);

    /* get info from HW */
    sal_memset(&soc_glb_pkt, 0, sizeof(soc_glb_pkt));

    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_global_packet_config_get,(unit, &soc_glb_pkt));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_global_packet_config_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    packet_config->lbg_tpid = soc_glb_pkt.lb_tpid;
    packet_config->outer_vlan_tpid = soc_glb_pkt.outer_vlan_tpid;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set packet configuration */
 int bcm_petra_lb_modem_packet_config_set(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_lb_modem_packet_config_t *packet_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_packet_config_t soc_modem_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(packet_config);
    BCM_LB_VALUE_CHECK(packet_config->pkt_format, bcmLbFormatTypeNonChannelize, bcmLbFormatTypechannelize, "pkt_format");

    /* update info */
    soc_modem_pkt.pkt_format = packet_config->pkt_format;
    sal_memcpy(soc_modem_pkt.dst_mac, packet_config->dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(soc_modem_pkt.src_mac, packet_config->src_mac, sizeof(bcm_mac_t));
    soc_modem_pkt.vlan = packet_config->vlan;
    soc_modem_pkt.use_global_priority_map = packet_config->use_global_priority_map ? 1 : 0;

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_modem_packet_config_set,(unit, modem_id, &soc_modem_pkt));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_modem_packet_config_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Get packet configuration */
 int bcm_petra_lb_modem_packet_config_get(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_lb_modem_packet_config_t *packet_config)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_packet_config_t soc_modem_pkt;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(packet_config);

    /* get info from HW */
    sal_memset(&soc_modem_pkt, 0, sizeof(soc_modem_pkt));

    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_modem_packet_config_get,(unit, modem_id, &soc_modem_pkt));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_modem_packet_config_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    packet_config->pkt_format = soc_modem_pkt.pkt_format;
    sal_memcpy(packet_config->dst_mac, soc_modem_pkt.dst_mac, sizeof(bcm_mac_t));
    sal_memcpy(packet_config->src_mac, soc_modem_pkt.src_mac, sizeof(bcm_mac_t));
    packet_config->vlan = soc_modem_pkt.vlan;
    packet_config->use_global_priority_map = soc_modem_pkt.use_global_priority_map;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set mapping for tc,dp to priority */
 int bcm_petra_lb_tc_dp_to_priority_config_set(
    int unit, 
    uint32 flags, 
    bcm_lb_tc_dp_t *tc_dp, 
    bcm_lb_pkt_pri_t *pkt_pri)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_tc_dp_t soc_tc_dp;
    soc_lb_pkt_pri_t soc_pkt_pri;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(tc_dp);
    BCMDNX_NULL_CHECK(pkt_pri);
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, map tc(%d) dp(%d) to packet priority(%d) cfi(%d) dp(%d)\n"), 
        unit, tc_dp->tc, tc_dp->dp, pkt_pri->pkt_pri, pkt_pri->pkt_cfi, pkt_pri->pkt_dp));

    /* update info */
    soc_tc_dp.tc = tc_dp->tc;
    soc_tc_dp.dp = tc_dp->dp;
    soc_pkt_pri.pkt_pri = pkt_pri->pkt_pri;
    soc_pkt_pri.pkt_cfi = pkt_pri->pkt_cfi;
    soc_pkt_pri.pkt_dp = pkt_pri->pkt_dp;

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_tc_dp_to_packet_priority_config_set,(unit, &soc_tc_dp, &soc_pkt_pri));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_tc_dp_to_packet_priority_config_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Get mapping for tc,dp to priority */
 int bcm_petra_lb_tc_dp_to_priority_config_get(
    int unit, 
    uint32 flags, 
    bcm_lb_tc_dp_t *tc_dp, 
    bcm_lb_pkt_pri_t *pkt_pri)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_tc_dp_t soc_tc_dp;
    soc_lb_pkt_pri_t soc_pkt_pri;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(tc_dp);
    BCMDNX_NULL_CHECK(pkt_pri);

    /* get info from HW */
    soc_tc_dp.tc = tc_dp->tc;
    soc_tc_dp.dp = tc_dp->dp;
    sal_memset(&soc_pkt_pri, 0, sizeof(soc_pkt_pri));

    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_tc_dp_to_packet_priority_config_get,(unit, &soc_tc_dp, &soc_pkt_pri));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_tc_dp_to_packet_priority_config_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    pkt_pri->pkt_pri = soc_pkt_pri.pkt_pri;
    pkt_pri->pkt_cfi = soc_pkt_pri.pkt_cfi;
    pkt_pri->pkt_dp = soc_pkt_pri.pkt_dp;

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set scheduler between lbg */
 int bcm_petra_lb_tx_sched_set(
    int unit, 
    uint32 flags, 
    int lbg_count, 
    bcm_lb_lbg_weight_t *lbg_weights)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(lbg_weights);

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_sched_set,(unit, lbg_count, (soc_lb_lbg_weight_t*)lbg_weights));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_sched_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set scheduler between lbg */
 int bcm_petra_lb_tx_sched_get(
    int unit, 
    uint32 flags, 
    int max_lbg_count, 
    bcm_lb_lbg_weight_t *lbg_weights, 
    int *lbg_count)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(lbg_weights);

    /* get info from HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_sched_get,(unit, max_lbg_count, (soc_lb_lbg_weight_t*)lbg_weights, (uint32 *)lbg_count));
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_sched_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif
}

/* Set modem shaper */
 int bcm_petra_lb_modem_shaper_set(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_lb_modem_shaper_config_t *shaper)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_shaper_config_t         soc_bit_shaper;
    soc_lb_modem_segment_shaper_config_t soc_segment_shaper;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    if ((flags & (~BCM_LB_FLAG_MODEM_SHAPER_SEGMENT)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(shaper);
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, set modem shaper flags(%x) enable(%d) rate(%d) max_burst(%d)\n"), 
        unit, flags, shaper->enable, shaper->rate, shaper->max_burst));

    /* get info from HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    if (flags & BCM_LB_FLAG_MODEM_SHAPER_SEGMENT) {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_segment_shaper_get,(unit, modem_id, &soc_segment_shaper));
    }
    else {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_get,(unit, modem_id, &soc_bit_shaper));
    }
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    if (flags & BCM_LB_FLAG_MODEM_SHAPER_SEGMENT) {
        soc_segment_shaper.enable = shaper->enable;
        soc_segment_shaper.rate = shaper->rate;
        soc_segment_shaper.max_burst= shaper->max_burst;
    }
    else {
        soc_bit_shaper.enable = shaper->enable;
        soc_bit_shaper.rate = shaper->rate;
        soc_bit_shaper.max_burst= shaper->max_burst;
    }

    /* update HW */
    _BCM_LB_SYSTEM_LOCK(unit);
    if (flags & BCM_LB_FLAG_MODEM_SHAPER_SEGMENT) {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_segment_shaper_set,(unit, modem_id, &soc_segment_shaper));
    }
    else {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_set,(unit, modem_id, &soc_bit_shaper));
    }
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_set, error 0x%x\n"), 
            unit, soc_rc));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set modem shaper */
 int bcm_petra_lb_modem_shaper_get(
    int unit, 
    bcm_modem_t modem_id, 
    uint32 flags, 
    bcm_lb_modem_shaper_config_t *shaper)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lb_modem_shaper_config_t         soc_bit_shaper;
    soc_lb_modem_segment_shaper_config_t soc_segment_shaper;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    BCM_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    if ((flags & (~BCM_LB_FLAG_MODEM_SHAPER_SEGMENT)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    BCMDNX_NULL_CHECK(shaper);

    /* get info from HW */
    sal_memset(&soc_bit_shaper, 0, sizeof(soc_bit_shaper));
    sal_memset(&soc_segment_shaper, 0, sizeof(soc_segment_shaper));
    _BCM_LB_SYSTEM_LOCK(unit);
    if (flags & BCM_LB_FLAG_MODEM_SHAPER_SEGMENT) {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_segment_shaper_get,(unit, modem_id, &soc_segment_shaper));
    }
    else {
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_egr_modem_shaper_get,(unit, modem_id, &soc_bit_shaper));
    }
    _BCM_LB_SYSTEM_UNLOCK(unit);
    if (SOC_FAILURE(soc_rc)) {
        BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_egr_modem_shaper_get, error 0x%x\n"), 
            unit, soc_rc));
    }

    /* update info */
    if (flags & BCM_LB_FLAG_MODEM_SHAPER_SEGMENT) {
        shaper->enable = soc_segment_shaper.enable;
        shaper->rate = soc_segment_shaper.rate;
        shaper->max_burst = soc_segment_shaper.max_burst;
    }
    else {
        shaper->enable = soc_bit_shaper.enable;
        shaper->rate = soc_bit_shaper.rate;
        shaper->max_burst = soc_bit_shaper.max_burst;
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Set flush operation */
int bcm_petra_lb_flush_configure_set(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    bcm_lb_flush_type_t type,
    int arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    bcm_lbg_t lbg_id_start = 0, lbg_id_end = 0, lbg_id_tmp = 0;
    uint8 bit_val = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if ((flags & (~BCM_LB_FLAG_FLUSH_GLOBAL)) != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }
    LOG_VERBOSE(BSL_LS_BCM_LB, (BSL_META_U(unit, "unit %d, config lb flush with lbg(%d) flags(%x) flush type(%d) arg(%d)\n"), 
        unit, lbg_id, flags, type, arg));

    if (flags & BCM_LB_FLAG_FLUSH_GLOBAL) {
        lbg_id_start = 0;
        lbg_id_end = SOC_TMC_LB_LBG_MAX;
    }
    else {
        BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        
        /* check if lbg port is added */    
        rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!bit_val) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                                 bcm_errmsg(BCM_E_CONFIG), lbg_id));
        }
        lbg_id_start = lbg_id_end = lbg_id;
    }

    switch (type) {
    case bcmLbFlushTypeForceFlush:
    case bcmLbFlushTypeFlushOnGroupOverflow:
    case bcmLbFlushTypeFlushOnFifoOverflow:
    case bcmLbFlushTypeFlushOnSharedOverflow:
        for (lbg_id_tmp = lbg_id_start; lbg_id_tmp <= lbg_id_end; lbg_id_tmp++) {
            rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id_tmp, &bit_val);
            BCMDNX_IF_ERR_EXIT(rv);
            if (!bit_val) continue;

            _BCM_LB_SYSTEM_LOCK(unit);
            soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_flush_config_set,(unit, lbg_id_tmp, type, arg));
            _BCM_LB_SYSTEM_UNLOCK(unit);
            if (SOC_FAILURE(soc_rc)) {
                BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_flush_config_get lbg_id(%d) type(%d), error 0x%x\n"), 
                    unit, lbg_id, type, soc_rc));
            }
        }
        break;
    default:
        if (type < bcmLbFlushTypeCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type not supported: %d"), unit, type));
    }

    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

/* Get configuration */
int bcm_petra_lb_flush_configure_get(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    bcm_lb_flush_type_t type,
    int *arg)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_LB_SUPPORT
    soc_error_t soc_rc = SOC_E_NONE;
    soc_lbg_t   soc_lbg_id = 0;
    uint8 bit_val = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = BCM_E_NONE;

    /* validates parameters */
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_LB_ENABLE_CHECK(unit);
    if (flags != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid flags (0x%x)"), unit, flags));
    }

    BCM_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    /* check if lbg port is added */    
    rv = LB_INFO_ACCESS.lbg_valid_bmp.bit_get(unit, lbg_id, &bit_val);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bit_val) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
            (_BSL_BCM_MSG("Fail(%s) LB port for LBG(%d) should be added first\n"),
                         bcm_errmsg(BCM_E_CONFIG), lbg_id));
    }

    switch (type) {
    case bcmLbFlushTypeForceFlush:
    case bcmLbFlushTypeFlushOnGroupOverflow:
    case bcmLbFlushTypeFlushOnFifoOverflow:
    case bcmLbFlushTypeFlushOnSharedOverflow:

        _BCM_LB_SYSTEM_LOCK(unit);
        soc_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_lb_ing_flush_config_get,(unit, soc_lbg_id, type, arg));
        _BCM_LB_SYSTEM_UNLOCK(unit);
        if (SOC_FAILURE(soc_rc)) {
            BCMDNX_ERR_EXIT_MSG(soc_rc, (_BSL_BCM_MSG("unit %d, error in mbcm_dpp_lb_ing_flush_config_get lbg_id(%d) type(%d), error 0x%x\n"), 
                unit, lbg_id, type, soc_rc));
        }
        break;
    default:
        if (type < bcmLbFlushTypeCount) {
            return BCM_E_UNAVAIL;
        }
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, status_type not supported: %d"), unit, type));
    }

    BCMDNX_IF_ERR_EXIT(rv);


    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
#else
    return rv;
#endif

}

