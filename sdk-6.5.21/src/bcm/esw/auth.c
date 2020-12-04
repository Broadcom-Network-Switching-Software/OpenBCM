/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * 802.1X
 */

#include <sal/core/libc.h>

#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/auth.h>

#include <bcm_int/esw/field.h> /* _bcm_field_setup_post_ethertype_udf() */
#include <bcm_int/esw/port.h>

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif

#define AUTH_INIT(unit) \
        if (!SOC_UNIT_VALID(unit)) { return (BCM_E_UNIT); } \
        else if (!(soc_feature(unit, soc_feature_field))) { \
            return (BCM_E_UNAVAIL); \
        } \
        else if (NUM_E_PORT(unit) <= 0) { return (BCM_E_BADID); } \
        else if (auth_cntl[unit] == NULL) { return (BCM_E_INIT); }
#define AUTH_INIT_NO_PORT(unit) \
        if (!SOC_UNIT_VALID(unit)) { return (BCM_E_UNIT); } \
        else if (!(soc_feature(unit, soc_feature_field))) { \
            return (BCM_E_UNAVAIL); \
        } \
        else if (auth_cntl[unit] == NULL) { return (BCM_E_INIT); }
#define AUTH_PORT(unit, port) \
        if (!SOC_PORT_VALID(unit, port) || !IS_E_PORT(unit, port)) \
        { return (BCM_E_PORT); }

#define AUTH_MODE_MASK \
        (BCM_AUTH_MODE_UNCONTROLLED | BCM_AUTH_MODE_AUTH | BCM_AUTH_MODE_UNAUTH)

#define _AUTH_ETHERTYPE_EAPOL 0x888e
#define _AUTH_ETHERTYPE_MASK  0xffff

typedef struct auth_mac_s *auth_mac_p;

typedef struct auth_mac_s {
    bcm_mac_t         mac;  
    bcm_field_entry_t entry;
    pbmp_t            pbmp;
    auth_mac_p        next;
} auth_mac_t;

typedef struct bcm_auth_cntl_s {
    int              mode;
    int              etmp;
    int              mac_set;
    auth_mac_t       *macList;
} bcm_auth_cntl_t;

typedef struct auth_cb_cntl_s {
    int             registered;
    bcm_auth_cb_t   auth_cbs;
    void            *auth_cb_data;
} auth_cb_cntl_t;

typedef struct auth_field_s {
    int               inited;
    int               count;
    bcm_field_group_t group0;
    auth_mac_t        *macList;
    bcm_field_group_t group1;
    bcm_field_entry_t entry1;
    bcm_field_group_t group2;
    bcm_field_entry_t entry2;
    pbmp_t            group2_pbmp;
} auth_field_cntl_t;

static bcm_auth_cntl_t *auth_cntl[BCM_MAX_NUM_UNITS];
static auth_cb_cntl_t cb_cntl[BCM_MAX_NUM_UNITS];
static auth_field_cntl_t fp_cntl[BCM_MAX_NUM_UNITS];

/* Forward declarations */
STATIC void _auth_linkscan_cb(int unit, bcm_port_t port, bcm_port_info_t *info);
STATIC int _auth_field_install(int unit, int port, bcm_mac_t mac); 
STATIC int _auth_field_remove(int unit, int port, bcm_mac_t mac); 
STATIC int _auth_field_install_all(int unit, int port);
STATIC int _auth_field_remove_all(int unit, int port);
#if defined(BCM_FIELD_SUPPORT)
STATIC int _auth_maclist_insert(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *ins);
STATIC int _auth_maclist_lookup(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *entry);
#endif /* !BCM_FIELD_SUPPORT */
STATIC int _auth_maclist_remove(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *del);
STATIC int _auth_maclist_destroy(auth_mac_t **list);
#ifdef AUTH_DEBUG
STATIC int _auth_maclist_dump(auth_mac_t **list);
#endif

/*
 * Function:
 *	bcm_auth_init
 * Purpose:
 *	Initialize auth module.
 * Parameters:
 *	unit - Device number
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_MEMORY   - Allocation failure
 *
 * Notes:
 *	All ports are marked as being in the uncontrolled state.
 */

int
bcm_esw_auth_init(int unit)
{
    bcm_port_t   port;
    int          rv = BCM_E_NONE, max_num_port = 0;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    if (!(soc_feature(unit, soc_feature_field))) {
        return (BCM_E_UNAVAIL);
    }

    if (auth_cntl[unit] != NULL) {
        rv = bcm_esw_auth_detach(unit);
        BCM_IF_ERROR_RETURN(rv);
    }

    max_num_port = SOC_MAX_NUM_PORTS;

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        max_num_port = SOC_MAX_NUM_PP_PORTS;
    }
#endif

    auth_cntl[unit] = sal_alloc(max_num_port * 
                                sizeof(bcm_auth_cntl_t), "auth_cntl");
    if (auth_cntl[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(auth_cntl[unit], 0, max_num_port * 
               sizeof(bcm_auth_cntl_t));

    for (port = 0; port < max_num_port; port++) {
        auth_cntl[unit][port].mode = BCM_AUTH_MODE_UNCONTROLLED;
        if (soc_feature(unit, soc_feature_field)) {
            BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, port));
        }
    }
    rv = bcm_esw_linkscan_register(unit, _auth_linkscan_cb);
    BCM_IF_ERROR_RETURN(rv);
    cb_cntl[unit].registered = TRUE;

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_detach
 * Purpose:
 *	Stop all auth module processing and deallocate resources.
 * Parameters:
 *	unit - Device number
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 * Notes:
 *	All ports are moved to the uncontrolled state.  All internal
 *	callbacks and filters are removed.
 */

int
bcm_esw_auth_detach(int unit)
{
    bcm_port_t        port;
    int rv = BCM_E_NONE, max_num_port = 0;
    bcm_pbmp_t temp_pbmp;
    SOC_PBMP_CLEAR(temp_pbmp);

    if (soc_feature(unit, soc_feature_field) == 0 ||
        auth_cntl[unit] == NULL) { 
        return (BCM_E_NONE);
    } 

    AUTH_INIT_NO_PORT(unit);

    max_num_port = SOC_MAX_NUM_PORTS;
    BCM_PBMP_ASSIGN(temp_pbmp, PBMP_E_ALL(unit));

#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &temp_pbmp);
	max_num_port = SOC_MAX_NUM_PP_PORTS;
    }
#endif

    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        for (port = 0; port < max_num_port ; port++) {
            if (BCM_PBMP_MEMBER(temp_pbmp, port)) {
                if (soc_feature(unit, soc_feature_field)) {
                    BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, port));
                }
                _auth_maclist_destroy(&auth_cntl[unit][port].macList);
                bcm_esw_port_learn_set(unit, port, 
                                       BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
                bcm_esw_port_stp_set(unit, port, BCM_STG_STP_FORWARD);
                auth_cntl[unit][port].mode = BCM_AUTH_MODE_UNCONTROLLED;
                auth_cntl[unit][port].etmp = FALSE;
            }
        }
    }

    for (port = 0; port < max_num_port; port++) {
        _auth_maclist_destroy(&auth_cntl[unit][port].macList);
    }

    fp_cntl[unit].inited = FALSE;
    fp_cntl[unit].count = 0;

    if (cb_cntl[unit].registered) {
        rv = bcm_esw_linkscan_unregister(unit, _auth_linkscan_cb); 
        /* If linkscan thread was restarted callback might be gone. */
        if ((BCM_FAILURE(rv)) && (BCM_E_NOT_FOUND != rv)) {
            return (rv);
        }
        cb_cntl[unit].registered = FALSE;
    }
    sal_free(auth_cntl[unit]);
    auth_cntl[unit] = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_mode_set
 * Purpose:
 *	Set the 802.1X operating mode
 * Parameters:
 *	unit - Device number
 *	port - Port number, -1 to set all non-stack-ports
 *	mode - One of BCM_AUTH_MODE_XXX and other flags (see below)
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 *	While in the uncontrolled state, any packets may flow in or out
 *	of the port and normal L2 learning takes place.
 *
 *	If mode is BCM_AUTH_UNAUTH, a set of flags may be ORed into
 *	the mode word:
 *		BCM_AUTH_BLOCK_IN	allow outgoing packets
 *		BCM_AUTH_BLOCK_INOUT	do not allow in or out packets
 *					(default)
 *	While in the unauthorized state, all L2 MAC addresses
 *	associated with the port are removed, L2 learning is disabled,
 *	and packet transfer is blocked as specified by the
 *	BCM_AUTH_BLOCK_* flags.  Incoming EAPOL frames are allowed
 *	either addressed as BPDUs or to the switch CPU's MAC address.
 *	Some hardware may forward all EAPOL frames regardless of the
 *	destination MAC address.  These will be delivered to the CPU
 *	for processing.
 *
 *	Outgoing EAPOL frames to be sent must be sent with a pair of
 *	bcm_auth_egress_set calls, enabling the egress before the
 *	EAPOL packet and disabling the egress after the packet is
 *	sent.  If the BCM_AUTH_BLOCK_IN flag has been given, then the
 *	bcm_auth_egress_set calls will not do anything.
 *
 *	If mode is BCM_AUTH_MODE_AUTH, a set of flags may be ORed into 
 *      the mode word:
 *		BCM_AUTH_LEARN		allow L2 learning while authorized
 *		BCM_AUTH_IGNORE_LINK	do not unauthorize upon link down
 *		BCM_AUTH_IGNORE_VIOLATION do not unauth upon security
 *					 violation
 *	After moving the port to the authorized state, one or more
 *	static L2 entries may be added using bcm_l2_addr_add with the
 *	auth field set.  These L2 addresses would typically be for
 *	the source MAC address of the requesting EAPOL frame that was
 *	authorized, in as many VLANs as are configured for the
 *	authorized user.
 *
 *	While in the authorized state, any packets incoming from source
 *	MAC addresses set by bcm_l2_addr_add will be accepted for
 *	transfer.  Any other source MAC addresses will be accepted if
 *	BCM_AUTH_LEARN was set.  If BCM_AUTH_LEARN was not set and the
 *	hardware supports it and BCM_AUTH_IGNORE_VIOLATION is not set,
 *	then unknown source MAC addresses will cause a security
 *	violation and move the port to the unauthorized state.
 *
 *	If the link goes down on the port while in the authorized
 *	state and BCM_AUTH_IGNORE_LINK is not set, then the port will
 *	be moved to the unauthorized state.
 */

int
bcm_esw_auth_mode_set(int unit, int port, uint32 mode)
{
    pbmp_t   pbmp;
    bcm_port_t p;

    AUTH_INIT(unit);

    if (port < 0) { 
        BCM_PBMP_ASSIGN(pbmp, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &pbmp);
        }
#endif
    } else { /* port >= 0 */
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
        AUTH_PORT(unit, port);
        BCM_PBMP_PORT_SET(pbmp, port);
    }

    /* Remove stack ports from bitmap */
    BCM_PBMP_REMOVE(pbmp, SOC_PBMP_STACK_CURRENT(unit));

    BCM_PBMP_ITER(pbmp, p) {
        switch (mode & AUTH_MODE_MASK) {
        case BCM_AUTH_MODE_UNCONTROLLED:
            if (soc_feature(unit, soc_feature_field)) {
                BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, p));
            }
            bcm_esw_port_learn_set(unit, p, BCM_PORT_LEARN_ARL |
                                   BCM_PORT_LEARN_FWD);
            bcm_esw_port_stp_set(unit, p, BCM_STG_STP_FORWARD);
            auth_cntl[unit][p].etmp = FALSE;
            break;
        case BCM_AUTH_MODE_UNAUTH:
            /* Disable learning on that port */
            bcm_esw_port_learn_set(unit, p, BCM_PORT_LEARN_FWD);

            /* remove all L2 (MAC) addresses associated with the port */
            bcm_esw_l2_addr_delete_by_port(unit, -1, p, BCM_L2_DELETE_STATIC);

            if (mode & BCM_AUTH_BLOCK_IN) {
                if (soc_feature(unit, soc_feature_field)) {
                    BCM_IF_ERROR_RETURN(_auth_field_install_all(unit, p)); 
                }
                /* Set STP state as forward for BLOCK_IN */
                BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(unit, p,
                                                         BCM_STG_STP_FORWARD));
            }
            else {
                /* unauthorized for both directions */
                if (soc_feature(unit, soc_feature_field)) {
                    BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, p));
                }
                mode |= BCM_AUTH_BLOCK_INOUT;
                BCM_IF_ERROR_RETURN(bcm_esw_port_stp_set(unit, p,
                                                         BCM_STG_STP_BLOCK));
            }
            break;
        case BCM_AUTH_MODE_AUTH:
            /* Invalid for both hardware learning and SLF dropping */
            if ((mode & (BCM_AUTH_LEARN | BCM_AUTH_DROP_UNKNOWN)) ==
               (BCM_AUTH_LEARN | BCM_AUTH_DROP_UNKNOWN)) {
                return BCM_E_PARAM;    
            }
            if (mode & BCM_AUTH_DROP_UNKNOWN) {
                /* Drop the packet */
                bcm_esw_port_learn_set(unit, p, 0);
            } else {
                /* No learn, Send to CPU, Drop the packet */
                bcm_esw_port_learn_set(unit, p, BCM_PORT_LEARN_CPU);
            }
            if (soc_feature(unit, soc_feature_field)) {
                BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, p));
            }
            bcm_esw_port_stp_set(unit, p, BCM_STG_STP_FORWARD);
            if (mode & BCM_AUTH_LEARN) {
                bcm_esw_port_learn_set(unit, p, BCM_PORT_LEARN_ARL
                                       | BCM_PORT_LEARN_FWD);
            }
            auth_cntl[unit][p].etmp = FALSE;
            break;
        default:
            return BCM_E_PARAM;
        }
        auth_cntl[unit][p].mode = mode;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_mode_get
 * Purpose:
 *	Get the 802.1X operating mode
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	mode - (OUT) One of BCM_AUTH_MODE_XXX and other flags
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 */

int
bcm_esw_auth_mode_get(int unit, int port, uint32 *modep)
{

    AUTH_INIT(unit);
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    AUTH_PORT(unit, port);

    *modep = auth_cntl[unit][port].mode;

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_unauth_callback
 * Purpose:
 *	Set the callback function for 802.1X notifications
 * Parameters:
 *	unit - Device number
 *	func - Callback function
 *	cookie - Arbitrary value passed along to callback function
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 * Notes:
 *	Calls func when a port on the unit has been moved from
 *	authorized to unauthorized state.  Reason can be one of:
 *		BCM_AUTH_REASON_UNKNOWN
 *		BCM_AUTH_REASON_LINK
 *		BCM_AUTH_REASON_VIOLATION
 */

int
bcm_esw_auth_unauth_callback(int unit, bcm_auth_cb_t func, void *cookie)
{
    AUTH_INIT_NO_PORT(unit);

    cb_cntl[unit].auth_cbs = func;
    cb_cntl[unit].auth_cb_data = cookie;

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_egress_set
 * Purpose:
 *	Enable/disable the ability of packets to be sent out a port.
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	enable - TRUE to enable, FALSE to disable
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 *	This call should only be used around calls that a CPU uses to
 *	transmit an EAPOL frame out the port.  If the port is in an
 *	unauthorized state with BCM_AUTH_BLOCK_IN set or is not in an
 *	unauthorized state, then this call does nothing.
 */

int
bcm_esw_auth_egress_set(int unit, int port, int enable)
{
    AUTH_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    AUTH_PORT(unit, port);

    if (enable) {
       if ((auth_cntl[unit][port].mode & BCM_AUTH_MODE_UNAUTH) &&
           !(auth_cntl[unit][port].mode & BCM_AUTH_BLOCK_IN)) {
           bcm_esw_port_stp_set(unit, port, BCM_STG_STP_FORWARD);
           if (soc_feature(unit, soc_feature_field)) {
               _auth_field_install_all(unit, port); 
           }
           auth_cntl[unit][port].mode &= ~BCM_AUTH_BLOCK_INOUT;
           auth_cntl[unit][port].mode |= BCM_AUTH_BLOCK_IN;
           auth_cntl[unit][port].etmp= TRUE;
       }
    }
    else {
       if ((auth_cntl[unit][port].mode & BCM_AUTH_MODE_UNAUTH) &&
           (auth_cntl[unit][port].mode & BCM_AUTH_BLOCK_IN)) {
           if (soc_feature(unit, soc_feature_field)) {
               BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, port)); 
           }
           bcm_esw_port_stp_set(unit, port, BCM_STG_STP_BLOCK);
           auth_cntl[unit][port].mode &= ~BCM_AUTH_BLOCK_IN;
           auth_cntl[unit][port].mode |= BCM_AUTH_BLOCK_INOUT;
           auth_cntl[unit][port].etmp= FALSE;
       
       }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_auth_egress_get
 * Purpose:
 *	Return enable/disable state of packets being sent out a port.
 * Parameters:
 *	unit - Device number
 *	port - Port number
 *	enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 * Notes:
 */

int
bcm_esw_auth_egress_get(int unit, int port, int *enable)
{
    AUTH_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    AUTH_PORT(unit, port);

    *enable = auth_cntl[unit][port].etmp;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_auth_mac_add
 * Purpose:
 *      Add switch's MAC addresses 
 * Parameters:
 *      unit - Device number
 *	port - Port number, -1 to all ports
 *      mac -  Switch's MAC address
 * Returns:
 *     BCM_E_NONE     - Success
 *     BCM_E_UNIT     - Invalid unit number
 *     BCM_E_UNAVAIL  - Insufficient hardware support
 *     BCM_E_PORT     - Invalid port
 *     BCM_E_PARAM    - bad mac
 * Notes:
 */

int
bcm_esw_auth_mac_add(int unit, int port, bcm_mac_t mac)
{
    bcm_mac_t mac_zero = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    bcm_mac_t mac_resv = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x03}; 
    pbmp_t   pbm;
#if defined(BCM_FIELD_SUPPORT)
    auth_mac_p entry = NULL;
    int rv;
    bcm_port_t p;
#endif /* BCM_FIELD_SUPPORT */

    AUTH_INIT(unit);

    if ((!sal_memcmp(mac, mac_zero, sizeof(bcm_mac_t))) ||
        (!sal_memcmp(mac, mac_resv, sizeof(bcm_mac_t)))) { 
        return BCM_E_PARAM;
    }

    if (port < 0) { 
        BCM_PBMP_ASSIGN(pbm, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &pbm);
        }
#endif
    } else { /* port >= 0 */
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
        AUTH_PORT(unit, port);

        BCM_PBMP_PORT_SET(pbm, port);
    }

#if defined(BCM_FIELD_SUPPORT)
    BCM_PBMP_ITER(pbm, p) {
        if ((rv = _auth_maclist_insert(&auth_cntl[unit][p].macList, 
                                       mac, &entry)) < 0) {
            return rv;
        }

        if (auth_cntl[unit][p].mac_set) {
            if (soc_feature(unit, soc_feature_field)) {
#ifdef BCM_FIELD_SUPPORT
                if ((rv = _auth_field_install(unit, p, mac)) < 0) {
                    _auth_maclist_remove(&auth_cntl[unit][p].macList, 
                                         mac, &entry);    
                    sal_free(entry);
                    return rv;
                }
#endif /* BCM_FIELD_SUPPORT */
            }
        }
    }
   
    return BCM_E_NONE;
#else /* !BCM_FIELD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* !BCM_FIELD_SUPPORT */
}

/*
 * Function:
 *      bcm_auth_mac_delete
 * Purpose:
 *      Delete switch's MAC address.
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mac  - Switch's MAC address 
 * Returns:
 *     BCM_E_NONE       - Success
 *     BCM_E_UNIT       - Invalid unit number
 *     BCM_E_UNAVAIL    - Insufficient hardware support
 *     BCM_E_PORT       - Invalid port
 *     BCM_E_PARAM      - bad mac
 *     BCM_E_NOT_FOUND  - MAC address not found
 * Notes:
 */

int
bcm_esw_auth_mac_delete(int unit, int port, bcm_mac_t mac)
{
    auth_mac_p entry = NULL;
    int rv;

    AUTH_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    AUTH_PORT(unit, port);

    rv = _auth_maclist_remove(&auth_cntl[unit][port].macList, mac, &entry);

    if (rv < 0) {
        return rv;
    }

    if (auth_cntl[unit][port].mac_set) {
        if (soc_feature(unit, soc_feature_field)) {
            BCM_IF_ERROR_RETURN(_auth_field_remove(unit, port, mac)); 
        }
    }

    sal_free(entry);
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_auth_mac_delete_all
 * Purpose:
 *      Delete all switch's MAC addresses.
 * Parameters:
 *      unit - Device number
 *	port - Port number 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_esw_auth_mac_delete_all(int unit, int port)
{
    int tmp, rv;

    AUTH_INIT(unit);

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    AUTH_PORT(unit, port);

    tmp = auth_cntl[unit][port].mac_set;
    if (soc_feature(unit, soc_feature_field)) {
        BCM_IF_ERROR_RETURN(_auth_field_remove_all(unit, port));
    }
    rv = _auth_maclist_destroy(&auth_cntl[unit][port].macList);
    auth_cntl[unit][port].mac_set = tmp;

    return rv;
}

/*
 * Function:
 *      _auth_linkscan_cb
 * Description:
 *      Put authorized state to unauthorized state if link down,
 *      given BCM_AUTH_IGNORE_LINK not set
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      info - pointer to structure giving status
 * Returns:
 *      None
 * Notes:
 */

STATIC void
_auth_linkscan_cb(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    if ((auth_cntl[unit] != NULL) && (IS_E_PORT(unit, port))
        && !(auth_cntl[unit][port].mode & BCM_AUTH_MODE_UNCONTROLLED)) {
        if (info->linkstatus != BCM_PORT_LINK_STATUS_UP) {
            if ((auth_cntl[unit][port].mode & BCM_AUTH_MODE_AUTH) &&
                !(auth_cntl[unit][port].mode & BCM_AUTH_IGNORE_LINK)) {
                bcm_esw_auth_mode_set(unit, port, BCM_AUTH_MODE_UNAUTH);
                if (cb_cntl[unit].auth_cbs) {
                    cb_cntl[unit].auth_cbs(cb_cntl[unit].auth_cb_data,
                                  unit, port, BCM_AUTH_REASON_LINK);
                }
            }
        }
    }
}

/*
 * Function:
 *      _auth_field_install
 * Description:
 *      Install FP for accepting EAPOL frames
 *      destinated for switch CPU's MAC address
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mac -  MAC address
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_field_install(int unit, int port, bcm_mac_t mac) 
{
#ifdef BCM_FIELD_SUPPORT
    _field_stage_t         *stage_fc=NULL;  /* Stage Field control structure.   */
    bcm_port_config_t      pc;              /* Port Configuration structure.    */
    bcm_field_group_config_t fg0;           /* Group configuration structure.   */
    bcm_field_group_config_t fg2;           /* Group configuration structure.   */
    int                    instance=0;      /* Pipe instance.                   */
    bcm_pbmp_t             mask_pbmp;       /* IPBM mask.                       */
    int rv = BCM_E_NONE;
    int rv2 = BCM_E_NONE;
    pbmp_t   pbm;
    bcm_field_group_t group0;
    bcm_field_entry_t entry0;
    bcm_mac_t mac_all_ones;
    auth_mac_p entry = NULL;

    /* In Tomahawk, In global mode, Group is created with port bitmaps of all pipes. 
     * In PerPipe mode, Group is created with port bitmaps of only the 
     * pipe to which the input port belongs to.
     */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc));
    
    bcm_port_config_t_init(&pc);
    BCM_IF_ERROR_RETURN(bcm_esw_port_config_get(unit, &pc));

    bcm_field_group_config_t_init(&fg0);
    bcm_field_group_config_t_init(&fg2);
    instance = SOC_INFO(unit).port_pipe[port];   
    mask_pbmp = PBMP_E_ALL(unit); 

    if (SOC_IS_TOMAHAWKX(unit)) {
        switch (stage_fc->oper_mode) {
            case bcmFieldGroupOperModeGlobal:
                break;
            case bcmFieldGroupOperModePipeLocal:
                fg0.ports = pc.per_pipe[instance];
                fg0.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT; 
                fg2.ports = pc.per_pipe[instance];
                fg2.flags |= BCM_FIELD_GROUP_CREATE_WITH_PORT;
                mask_pbmp = PBMP_PIPE(unit,instance);
                break;
            default: 
                return (BCM_E_INTERNAL);
        }
    }
           
    sal_memset(mac_all_ones, 0xff, sizeof(bcm_mac_t));

    if (!fp_cntl[unit].inited) {
        bcm_field_group_t group2;
        bcm_field_entry_t entry2;

        BCM_FIELD_QSET_INIT(fg0.qset);
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPort);
        } else {
            BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPorts);
        }
        BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyDstMac);
        BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyEtherType);

        fg0.priority = 14;
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_config_create(unit, &fg0));
        group0 = fg0.group;

        BCM_IF_ERROR_RETURN(
            bcm_esw_field_entry_create(unit, group0, &entry0));
        BCM_PBMP_PORT_SET(pbm, port);
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPort(unit, entry0, port, -1));
        } else {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPorts(unit, entry0, pbm, mask_pbmp));
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_field_qualify_DstMac(unit, entry0, mac, 
                                     (uint8 *)mac_all_ones));
        BCM_IF_ERROR_RETURN(
            bcm_esw_field_qualify_EtherType(unit, entry0, _AUTH_ETHERTYPE_EAPOL,
                                        _AUTH_ETHERTYPE_MASK));
        BCM_IF_ERROR_RETURN(
            bcm_esw_field_action_add(unit, entry0, bcmFieldActionCopyToCpu, 0, 0));

        BCM_FIELD_QSET_INIT(fg2.qset);
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_FIELD_QSET_ADD(fg2.qset, bcmFieldQualifyInPort);
        } else {
            BCM_FIELD_QSET_ADD(fg2.qset, bcmFieldQualifyInPorts);
        }

        fg2.priority = 13;
        BCM_IF_ERROR_RETURN(bcm_esw_field_group_config_create(unit, &fg2));
        group2 = fg2.group;

        BCM_IF_ERROR_RETURN(
            bcm_esw_field_entry_create(unit, group2, &entry2));
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPort(unit, entry2, port, -1));
        } else {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPorts(unit, entry2, pbm, mask_pbmp));
        }

        BCM_IF_ERROR_RETURN(
            bcm_esw_field_action_add(unit, entry2, bcmFieldActionDrop, 0, 0));

        rv = bcm_esw_field_entry_install(unit, entry0);
        rv2 = bcm_esw_field_entry_install(unit, entry2);
        if (BCM_FAILURE(rv) || BCM_FAILURE(rv2)) {
            BCM_IF_ERROR_RETURN(bcm_esw_field_entry_remove(unit, entry0));
            BCM_IF_ERROR_RETURN(bcm_esw_field_entry_destroy(unit, entry0));
            BCM_IF_ERROR_RETURN(bcm_esw_field_group_destroy(unit, group0));
            BCM_IF_ERROR_RETURN(bcm_esw_field_entry_remove(unit, entry2));
            BCM_IF_ERROR_RETURN(bcm_esw_field_entry_destroy(unit, entry2));
            BCM_IF_ERROR_RETURN(bcm_esw_field_group_destroy(unit, group2));
            return (rv != BCM_E_NONE ? rv : rv2);
        }

        fp_cntl[unit].group0 = group0;
        rv2 = _auth_maclist_insert(&fp_cntl[unit].macList, mac, &entry);
        if (rv2 == BCM_E_NONE) {
            entry->entry = entry0;
            BCM_PBMP_PORT_SET(entry->pbmp, port);
        }
        fp_cntl[unit].group2 = group2;
        fp_cntl[unit].entry2 = entry2;
        BCM_PBMP_PORT_SET(fp_cntl[unit].group2_pbmp, port);
        fp_cntl[unit].inited = TRUE; 
        fp_cntl[unit].count++; 
    } else {
        if ((rv = _auth_maclist_lookup(&fp_cntl[unit].macList, mac, &entry)) > 0) {
            BCM_FIELD_QSET_INIT(fg0.qset);
            if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPort);
            } else {
                BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPorts);
            }
            pbm = entry->pbmp; 
            BCM_PBMP_PORT_ADD(pbm, port);
            if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_field_qualify_InPort(unit, entry->entry, port, -1)); 
            } else {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_field_qualify_InPorts(unit, entry->entry, pbm, 
                                            mask_pbmp));
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_entry_reinstall(unit, entry->entry));
            entry->pbmp = pbm;
        } else {
            BCM_FIELD_QSET_INIT(fg0.qset);
            if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPort);
            } else {
                BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyInPorts);
            }
            BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyDstMac);
            BCM_FIELD_QSET_ADD(fg0.qset, bcmFieldQualifyEtherType);
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_entry_create(unit, fp_cntl[unit].group0, &entry0));
            BCM_PBMP_PORT_SET(pbm, port);
            if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_field_qualify_InPort(unit, entry0, port, -1));
            } else {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_field_qualify_InPorts(unit, entry0, pbm, mask_pbmp));
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_DstMac(unit, entry0, mac,
                                             (uint8 *) mac_all_ones));
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_EtherType(unit, entry0, _AUTH_ETHERTYPE_EAPOL,
                                                _AUTH_ETHERTYPE_MASK));
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_action_add(unit, entry0, bcmFieldActionCopyToCpu, 
                                        0, 0));
            rv = bcm_esw_field_entry_install(unit, entry0);
            if (BCM_SUCCESS(rv)) {
                auth_mac_p entry = NULL;
                rv2 = _auth_maclist_insert(&fp_cntl[unit].macList, mac, &entry);
                if (rv2 == BCM_E_NONE) {
                    entry->entry = entry0;
                    entry->pbmp = pbm;
                }
                fp_cntl[unit].count++; 
            } else {
                BCM_IF_ERROR_RETURN(bcm_esw_field_entry_destroy(unit, entry0));
            }
        }

        /* Add port to Drop all group */
        pbm = fp_cntl[unit].group2_pbmp; 
        BCM_PBMP_PORT_ADD(pbm, port);
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPort(unit, fp_cntl[unit].entry2, port, -1));
        } else {
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_qualify_InPorts(unit, fp_cntl[unit].entry2, pbm, 
                                            mask_pbmp));
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_field_entry_reinstall(unit, fp_cntl[unit].entry2));
        fp_cntl[unit].group2_pbmp = pbm;
        
    }

    return rv;
#else /* !BCM_FIELD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* !BCM_FIELD_SUPPORT */
}

/*
 * Function:
 *      _auth_field_install_all
 * Description:
 *      Install FP for accepting EAPOL frames
 *      destinated for switch CPU's MAC addresses
 * Parameters:
 *      unit - Device number
 *      port - Port number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_field_install_all(int unit, int port)
{
    auth_mac_t **list=&auth_cntl[unit][port].macList;
    auth_mac_p entry = NULL;
    int rv = BCM_E_NONE;
    bcm_mac_t mac_zero = {0};

    if (auth_cntl[unit][port].mac_set) {
        return BCM_E_EXISTS;
    }

    /* No accepted MAC addresses, drop all for default */
    if (*list == NULL) {
        rv = _auth_field_install(unit, port, mac_zero);
        if (rv < 0) {
            return rv;
        }
    }

    while (*list != NULL) {
        rv = _auth_field_install(unit, port, (*list)->mac);
        if (rv < 0) {
            _auth_maclist_remove(list, (*list)->mac, &entry);
            sal_free(entry);
        } else {
            list = &(*list)->next;
        }
    }
    auth_cntl[unit][port].mac_set = TRUE;

    return rv;
}

/*
 * Function:
 *      _auth_field_remove_all
 * Description:
 *      Removed all installed FP in controlled state
 * Parameters:
 *      unit - Device number
 *      port - Port number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_field_remove_all(int unit, int port)
{
    bcm_pbmp_t pbmp_mask;
    auth_mac_t **list=&fp_cntl[unit].macList;

    if (!auth_cntl[unit][port].mac_set) {
        return BCM_E_NONE;
    }

    BCM_PBMP_CLEAR(pbmp_mask);
    BCM_PBMP_ASSIGN (pbmp_mask, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_mask);
    }
#endif

    while (*list != NULL) {
        pbmp_t   pbm;
        pbm = (*list)->pbmp;
        BCM_PBMP_PORT_REMOVE(pbm, port); 
        if (BCM_PBMP_IS_NULL(pbm)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_entry_remove(unit, (*list)->entry)); 
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_entry_destroy(unit, (*list)->entry));
            fp_cntl[unit].count--;
            (*list)->entry = -1;
        } else {
            if (!soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_field_qualify_InPorts(unit, (*list)->entry, pbm, pbmp_mask));
            }
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_entry_reinstall(unit, (*list)->entry));
            (*list)->pbmp = pbm;
        }
        list = &(*list)->next;
    }

    if ((fp_cntl[unit].count==0) && (fp_cntl[unit].inited)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_entry_remove(unit, fp_cntl[unit].entry2)); 
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_entry_destroy(unit, fp_cntl[unit].entry2));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_group_destroy(unit, fp_cntl[unit].group0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_group_destroy(unit, fp_cntl[unit].group2));
        _auth_maclist_destroy(&fp_cntl[unit].macList);
        fp_cntl[unit].inited = FALSE;
    } else {
        auth_mac_p    entry;
        list = &fp_cntl[unit].macList;
        while (*list != NULL) {
            if ((*list)->entry == -1) {   
               entry = *list;
               *list = entry->next;
               sal_free(entry);
            } 
            list = &(*list)->next;
        }
    }

    auth_cntl[unit][port].mac_set = FALSE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _auth_field_remove
 * Description:
 *      Removed installed FP
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      mac -  Switch's MAC address 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_field_remove(int unit, int port, bcm_mac_t mac)
{
#ifdef BCM_FIELD_SUPPORT
    pbmp_t   pbm;
    bcm_field_qset_t qset0;
    auth_mac_p entry = NULL;
    bcm_pbmp_t pbmp_mask;

    BCM_PBMP_CLEAR(pbmp_mask);
    BCM_PBMP_ASSIGN (pbmp_mask, PBMP_E_ALL(unit));
#ifdef BCM_KATANA2_SUPPORT
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        _bcm_kt2_subport_pbmp_update(unit, &pbmp_mask);
    }
#endif

    if (_auth_maclist_lookup(&fp_cntl[unit].macList, mac, &entry) > 0) {
        BCM_FIELD_QSET_INIT(qset0);
        if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
            BCM_FIELD_QSET_ADD(qset0, bcmFieldQualifyInPort);
        } else {
            BCM_FIELD_QSET_ADD(qset0, bcmFieldQualifyInPorts);
        }
        pbm = entry->pbmp;
        BCM_PBMP_PORT_REMOVE(pbm, port);
        if (BCM_PBMP_IS_NULL(pbm)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_entry_remove(unit, entry->entry)); 
            BCM_IF_ERROR_RETURN
                (bcm_esw_field_entry_destroy(unit, entry->entry));
            fp_cntl[unit].count--;
            _auth_maclist_remove(&fp_cntl[unit].macList, mac, &entry);    
            sal_free(entry);
        } else {
            if (soc_feature(unit, soc_feature_ifp_no_inports_support)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_field_qualify_InPort(unit, entry->entry, port, -1));
            } else {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_field_qualify_InPorts(unit, entry->entry, pbm, pbmp_mask));
            }
            BCM_IF_ERROR_RETURN(
                bcm_esw_field_entry_reinstall(unit, entry->entry));
            entry->pbmp = pbm;
        }
    }

    if ((fp_cntl[unit].count==0) && (fp_cntl[unit].inited)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_entry_remove(unit, fp_cntl[unit].entry2)); 
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_entry_destroy(unit, fp_cntl[unit].entry2));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_group_destroy(unit, fp_cntl[unit].group0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_field_group_destroy(unit, fp_cntl[unit].group2));
        _auth_maclist_destroy(&fp_cntl[unit].macList);
        fp_cntl[unit].inited = FALSE;
    }

    return BCM_E_NONE;
#else /* !BCM_FIELD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* !BCM_FIELD_SUPPORT */
}

#if defined(BCM_FIELD_SUPPORT)
/*
 * Function:
 *      _auth_maclist_insert 
 * Description:
 *      Add a MAC address to the list 
 * Parameters:
 *      list - list to be inserted 
 *      mac -  MAC address 
 *      ins -  (OUT) entry inserted  
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_maclist_insert(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *ins)
{
    auth_mac_p    entry = NULL;

    if (_auth_maclist_lookup(list, mac, &entry) > 0) {
        return BCM_E_EXISTS;
    } 

    if ((entry = sal_alloc(sizeof(auth_mac_t), "maclist")) == NULL) {
        return BCM_E_MEMORY;
    }

    sal_memset(entry, 0, sizeof(auth_mac_t));
    sal_memcpy(entry->mac, mac, sizeof(bcm_mac_t)); 
    entry->next = *list;
    *list = entry;
    *ins = entry;

    return BCM_E_NONE;
}
#endif /* !BCM_FIELD_SUPPORT */

/*
 * Function:
 *      _auth_maclist_remove
 * Description:
 *      Remove a MAC address from the list
 * Parameters:
 *      list - list to be removed 
 *      mac -  MAC address
 *      ins -  (OUT) entry deleted 
 * Returns:
 *      BCM_E_NONE       - Success
 *      BCM_E_NOT_FOUND  - MAC not found in list
 * Notes:
 */

STATIC int
_auth_maclist_remove(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *del)
{
    auth_mac_p    entry;

    while (*list != NULL) {
        if (!sal_memcmp((*list)->mac, mac, sizeof(bcm_mac_t))) { 
            entry = *list;
            *list = entry->next;
            *del = entry;
            return BCM_E_NONE;
        }
        list = &(*list)->next;
    }

    return BCM_E_NOT_FOUND;
}

#if defined(BCM_FIELD_SUPPORT)
/*
 * Function:
 *      _auth_maclist_lookup
 * Description:
 *      Lookup a MAC address in the list
 * Parameters:
 *      list - list to be lookuped 
 *      mac -  MAC address
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_maclist_lookup(auth_mac_t **list, bcm_mac_t mac, auth_mac_p *entry)
{
    while (*list != NULL) {
        if (!sal_memcmp((*list)->mac, mac, sizeof(bcm_mac_t))) { 
            *entry = *list;  
            return TRUE;
        }
        list = &(*list)->next;
    }

    return FALSE;
}
#endif /* !BCM_FIELD_SUPPORT */

/*
 * Function:
 *      _auth_maclist_destroy
 * Description:
 *      Destroy all MAC addresses in the list
 * Parameters:
 *      list - list to be destroyed 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_maclist_destroy(auth_mac_t **list)
{
    auth_mac_p entry = NULL;

    if (*list == NULL) {
        return BCM_E_EMPTY;
    }

    while (*list != NULL) {
        _auth_maclist_remove(list, (*list)->mac, &entry);
        sal_free(entry);
    }
    return BCM_E_NONE;
}

#ifdef AUTH_DEBUG
/*
 * Function:
 *      _auth_maclist_dump
 * Description:
 *      Dump all MAC addresses in the list
 * Parameters:
 *      list - list to be dumped 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

STATIC int
_auth_maclist_dump(auth_mac_t **list)
{
    while (*list != NULL) {
        LOG_CLI((BSL_META_U(unit,
                            "%02x:%02x:%02x:%02x:%02x:%02x\n"),
                 (*list)->mac[0], (*list)->mac[1],
                 (*list)->mac[2], (*list)->mac[3],
                 (*list)->mac[4], (*list)->mac[5]));
        list = &(*list)->next;
    }

    return BCM_E_NONE;
}
#endif /* AUTH_DEBUG */
