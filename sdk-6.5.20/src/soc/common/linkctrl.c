/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Hardware Linkscan module
 *
 * For legacy devices (pre-Arad and pre-XGS5) Hardware linkscan is not recommended
 *
 * If hardware linkscan is used, each MII operation must temporarily
 * disable it and wait for the current scan to complete, increasing the
 * latency.  PHY status register 1 may contain clear-on-read bits that
 * will be cleared by hardware linkscan and not seen later.  Special
 * support is provided for the Serdes MAC.
 *
 * NOTE:
 * Original file is src/soc/common/link.c, version 1.7.
 * That file should eventually be removed, and XGS specific routines
 * should be implemented and set for the 'driver' during module
 * initialization in 'soc_linkctrl_init()'.
 */
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/linkctrl.h>

typedef struct _soc_linkctrl_s {
    pbmp_t                 link_fwd;
    pbmp_t                 link_mask;
    int                    link_pause;  /* Link scan pause count */
    CONST soc_linkctrl_driver_t  *driver;
} _soc_linkctrl_t;

static _soc_linkctrl_t    _link_control[SOC_MAX_NUM_DEVICES];

/*
 * General util macros
 */
#define UNIT_VALID_CHECK(_unit) \
    if (((_unit) < 0) || ((_unit) >= SOC_MAX_NUM_DEVICES)) { \
        return SOC_E_UNIT; \
    }

#define UNIT_INIT_CHECK(_unit) \
    do { \
        UNIT_VALID_CHECK(_unit); \
        if (LINKCTRL_DRV(_unit) == NULL) { return SOC_E_INIT; } \
    } while (0)

#define PARAM_NULL_CHECK(_param) \
    if ((_param) == NULL) { return SOC_E_PARAM; }


#define LINKCTRL(_unit)         (_link_control[_unit])
#define LINKCTRL_DRV(_unit)     (_link_control[_unit].driver)

#define LINK_MIIM_PORT_INFO(_unit)  (LINKCTRL_DRV(_unit)->port_info)

#define _LC_CALL(_ld, _lf, _la) \
    ((_ld) == 0 ? SOC_E_PARAM : \
     ((_ld)->_lf == 0 ? SOC_E_UNAVAIL : (_ld)->_lf _la))

#define LINKSCAN_HW_INIT(_u) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_linkscan_hw_init, (_u))

#define LINKSCAN_CONFIG(_u, _mii, _dir) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_linkscan_config, ((_u), (_mii), (_dir)))

#define LINKSCAN_PAUSE(_u) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_linkscan_pause, (_u))

#define LINKSCAN_CONTINUE(_u) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_linkscan_continue, (_u))

#define LINK_UPDATE(_u) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_update, (_u))

#define LINKSCAN_HW_LINK_GET(_u, _hw_link) \
    _LC_CALL(LINKCTRL_DRV(_u), ld_hw_link_get, ((_u), (_hw_link)))

/*
 * Function:
 *     soc_linkctrl_linkscan_hw_init
 * Purpose:
 *     Initialize device hardware linkscan.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     SOC_E_XXX
 */
int
soc_linkctrl_linkscan_hw_init(int unit)
{
    UNIT_INIT_CHECK(unit);

    return LINKSCAN_HW_INIT(unit);
}

/*
 * Function:
 *     soc_linkctrl_init
 * Purpose:
 *     Initialize SOC link control module.
 * Parameters:
 *     unit   - Device unit number
 *     driver - Device functions
 * Returns:
 *     SOC_E_XXX
 */
int
soc_linkctrl_init(int unit, CONST soc_linkctrl_driver_t *driver)
{
    UNIT_VALID_CHECK(unit);
    PARAM_NULL_CHECK(driver);

#ifdef BCM_LINKSCAN_LOCK_PER_UNIT
    if (soc_feature(unit, soc_feature_linkscan_lock_per_unit)) {
        SOC_CONTROL(unit)->linkscan_mutex = sal_mutex_create("Linkscan Lock");
    }
#endif

    SOC_PBMP_CLEAR(LINKCTRL(unit).link_fwd);
    LINKCTRL(unit).link_mask  = PBMP_ALL(unit);
    LINKCTRL(unit).link_pause = 0;
    LINKCTRL(unit).driver     = driver;

    return SOC_E_NONE;
}

int
soc_linkctrl_deinit(int unit)
{

#ifdef BCM_LINKSCAN_LOCK_PER_UNIT
    if (soc_feature(unit, soc_feature_linkscan_lock_per_unit)) {
        if(NULL != SOC_CONTROL(unit)->linkscan_mutex) {
            sal_mutex_destroy(SOC_CONTROL(unit)->linkscan_mutex);
            SOC_CONTROL(unit)->linkscan_mutex = NULL;
        }
    }
#endif

    return SOC_E_NONE;

}


/*
 * Function:
 *     soc_linkctrl_link_fwd_set
 * Purpose:
 *     Set link forward for given bitmap.
 * Parameters:
 *     unit - Device unit number
 *     pbmp - Link forward bitmap
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *     Link bitmap should be manipulated only through this routine and
 *     soc_linkctrl_link_mask_set.
 */
int
soc_linkctrl_link_fwd_set(int unit, pbmp_t fwd)
{
    UNIT_INIT_CHECK(unit);

    LINKCTRL(unit).link_fwd = fwd;

    return LINK_UPDATE(unit);
}

/*
 * Function:
 *     soc_linkctrl_link_fwd_get
 * Purpose:
 *     Get link forward bitmap.
 * Parameters:
 *     unit - Device unit number
 *     pbmp - (OUT) Link forward bitmap
 * Returns:
 *     SOC_E_XXX
 */
int
soc_linkctrl_link_fwd_get(int unit, pbmp_t *fwd)
{
    UNIT_INIT_CHECK(unit);
    PARAM_NULL_CHECK(fwd);

    *fwd = LINKCTRL(unit).link_fwd;

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_linkctrl_link_mask_set
 * Purpose:
 *     Mask bits in link bitmap independent of link_fwd value.
 * Parameters:
 *     unit - Device unit number
 *     mask - Link mask bitmap
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *     This routine is used to clear bits in the link bitmap to support
 *     the mac_fe/ge_enable_set() calls.
 */
int
soc_linkctrl_link_mask_set(int unit, pbmp_t mask)
{
    UNIT_INIT_CHECK(unit);

    LINKCTRL(unit).link_mask = mask;

    return LINK_UPDATE(unit);
}

/*
 * Function:
 *     soc_linkctrl_link_mask_get
 * Purpose:
 *     Counterpart to soc_linkctrl_mask_set
 * Parameters:
 *     unit - Device unit number
 *     mask - (OUT) Link mask bitmap
 * Returns:
 *     SOC_E_XXX
 */
int
soc_linkctrl_link_mask_get(int unit, pbmp_t *mask)
{
    UNIT_INIT_CHECK(unit);
    PARAM_NULL_CHECK(mask);

    *mask = LINKCTRL(unit).link_mask;

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_linkctrl_linkscan_register
 * Purpose:
 *     Provide a callout made when link scanning detects a link change.
 * Parameters:
 *     unit - Device unit number
 *     f    - Function called when link status change is detected
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *     Handler called in interrupt context.
 */
int
soc_linkctrl_linkscan_register(int unit, void (*f)(int))
{
    soc_control_t  *soc;

    UNIT_INIT_CHECK(unit);

    soc = SOC_CONTROL(unit);    
    if (f != NULL && soc->soc_link_callout != NULL) {
        return SOC_E_EXISTS;
    }

    soc->soc_link_callout = f;

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_linkctrl_linkscan_config
 * Purpose:
 *     Set ports to scan in CMIC.
 * Parameters:
 *     unit       - Device unit number
 *     mii_pbm    - Port bitmap of ports to scan with MIIM registers
 *     direct_pbm - Port bitmap of ports to scan using NON MII
 * Returns:
 *     SOC_E_XXX
 */
int
soc_linkctrl_linkscan_config(int unit, pbmp_t hw_mii_pbm, pbmp_t hw_direct_pbm)
{
    int     rv;
    int     s, has_mge, has_dge;
    pbmp_t  pbm;

    UNIT_INIT_CHECK(unit);

    SOC_PBMP_ASSIGN(pbm, hw_mii_pbm);
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    if (SOC_PBMP_NOT_NULL(pbm)) {    /* !(hw_mii_pbm & hw_direct_pbm) */
        return SOC_E_CONFIG;
    }

    /*
     * Hardware (direct) scanning is NOT supported on 10/100 ports.
     */
    SOC_PBMP_ASSIGN(pbm, hw_direct_pbm);
    SOC_PBMP_AND(pbm, PBMP_FE_ALL(unit));
    if (SOC_PBMP_NOT_NULL(pbm)) {
        return SOC_E_UNAVAIL;
    }

    /*
     * The LINK_SCAN_GIG control affects ALL ports. Thus, all ports
     * being scanned by H/W must be either MIIM scanned or scanned
     * using the direct connection.
     */
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_mii_pbm);
    has_mge = SOC_PBMP_NOT_NULL(pbm);
    SOC_PBMP_ASSIGN(pbm, PBMP_GE_ALL(unit));
    SOC_PBMP_AND(pbm, hw_direct_pbm);
    has_dge = SOC_PBMP_NOT_NULL(pbm);
    if (has_mge && has_dge) {
        return SOC_E_UNAVAIL;
    }

    /*
     * soc_linkctrl_linkscan_pause/continue combination will result in the
     * registers being setup and started properly if we are enabling for
     * the first time.
     */

    SOC_LINKSCAN_LOCK(unit, s);

    soc_linkctrl_linkscan_pause(unit);

    rv = LINKSCAN_CONFIG(unit, hw_mii_pbm, hw_direct_pbm);
    
    soc_linkctrl_linkscan_continue(unit);

    SOC_LINKSCAN_UNLOCK(unit, s);

    return rv;
}

/*
 * Function:
 *     soc_linkctrl_linkscan_pause
 * Purpose:
 *     Pauses link scanning, without disabling it.
 *     This call is used to pause scanning temporarily.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     SOC_E_NONE
 * Notes:
 *     Nesting pauses is provided for.
 *     Software must ensure every pause is accompanied by a continue
 *     or linkscan will never resume.
 */
int
soc_linkctrl_linkscan_pause(int unit)
{
    int            rv = SOC_E_NONE;
    int            s;
    soc_control_t  *soc;

    UNIT_INIT_CHECK(unit);

    soc = SOC_CONTROL(unit);    
    SOC_LINKSCAN_LOCK(unit, s);    /* Manipulate flags & regs atomically */

    if ((LINKCTRL(unit).link_pause++ == 0) &&
        (soc->soc_flags & SOC_F_LSE)) {
        /* Stop link scan and wait for current pass to finish */
        rv = LINKSCAN_PAUSE(unit);
    }

    SOC_LINKSCAN_UNLOCK(unit, s);

    return rv;
}

/*
 * Function:
 *     soc_linkctrl_linkscan_continue
 * Purpose:
 *     Continue link scanning after it has been paused.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     SOC_E_NONE
 * Notes:
 *     This routine is designed so if soc_linkctrl_linkscan_config is called,
 *     it won't be confused whether or not a pause is in effect.
 */
int
soc_linkctrl_linkscan_continue(int unit)
{
    int            rv = SOC_E_NONE;
    int            s;
    soc_control_t  *soc;

    UNIT_INIT_CHECK(unit);

    soc = SOC_CONTROL(unit);    
    SOC_LINKSCAN_LOCK(unit, s);    /* Manipulate flags & regs atomically */

    if (LINKCTRL(unit).link_pause <= 0) {
        SOC_LINKSCAN_UNLOCK(unit, s);
        return SOC_E_INTERNAL;    /* Continue not preceded by a pause */
    }

    if ((--LINKCTRL(unit).link_pause == 0) &&
        (soc->soc_flags & SOC_F_LSE)) {
        rv = LINKSCAN_CONTINUE(unit);
    }

    SOC_LINKSCAN_UNLOCK(unit, s);

    return rv;
}

/*
 * Function:
 *     soc_linkctrl_miim_port_get
 * Purpose:
 *     Return the MIIM port number for a given port.
 * Parameters:
 *     unit      - Device unit number
 *     port      - Device port number
 *     miim_port - Returns port number in MIIM
 * Returns:
 *     SOC_E_XXX
 * Notes:
 *     When dealing with ports, the MIIM has a fixed port number
 *     assigned to a physical port.  Since the user can remap
 *     the port numbers in some devices,
 *     it is necessary to translate the 'user' port number to
 *     the actual device physical port.
 */
int
soc_linkctrl_miim_port_get(int unit, soc_port_t port, int *miim_port)
{
    soc_linkctrl_port_info_t  *port_info;
    int                       block_type;
    int                       block_number;
    int                       block_index;

    UNIT_INIT_CHECK(unit);

    port_info = LINK_MIIM_PORT_INFO(unit);
    if (port_info == NULL) {
        *miim_port = port;
        return SOC_E_NONE;
    }

    /* Get physical block type, number and port offset */
    block_type   = SOC_PORT_BLOCK_TYPE(unit, port);
    block_number = SOC_PORT_BLOCK_NUMBER(unit, port);
    block_index  = SOC_PORT_BLOCK_INDEX(unit, port);

    while (port_info->block_type != -1) {
        if ((port_info->block_type   == block_type)   &&
            (port_info->block_number == block_number) &&
            (port_info->block_index  == block_index)) {
            /* Found */
            *miim_port = port_info->port;
            return SOC_E_NONE;
        }

        port_info++;
    }

    return SOC_E_NOT_FOUND;
}

int
soc_linkctrl_hw_link_get(int unit, soc_pbmp_t *hw_link)
{

    UNIT_INIT_CHECK(unit);

    return LINKSCAN_HW_LINK_GET(unit, hw_link);

}
