/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Port driver.
 */

#include <shared/bsl.h>

#include <soc/defs.h>

#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/monterey.h>
#include <soc/apache.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trident2.h>
#include <bcm_int/esw/trident2plus.h>
#include <bcm_int/esw/monterey.h>
#include <bcm_int/esw/apache.h>
#include <bcm/error.h>
#include <bcm/port.h>

#define MN_FLEX_MAX_NUM_PORTS        65

#define IS_MN_HSP_PORT(u,p)   \
    ((_soc_apache_port_sched_type_get((u),(p)) == SOC_APACHE_SCHED_HSP) ? 1 : 0)

/*
 * Function:
 *      _bcm_monterey_port_mac_get
 * Description:
 *      Get EGR_PORT/LPORT table mac address
 * Parameters:
 *      unit    - (IN)  BCM device number
 *      port    - (IN)  Port number if valid.
 *      oui     - (IN)  Indicates mac field is OUI/Non-OUI
 *      mac     - (OUT) OUI/Non-OUI part of MAC-ADDRESS
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_monterey_port_mac_get(int unit, bcm_port_t port, int oui, uint32* mac)
{
    egr_port_entry_t   egr_port_entry;
    bcm_module_t       modid;
    bcm_port_t         local_port;
    bcm_trunk_t        tgid;
    int                id;
    int                is_local = FALSE;
    soc_field_t        field;

    if (oui) {
        field = MAC_ADDRESS_UPPERf;
    } else {
        field = MAC_ADDRESS_LOWERf;
    }

    if (BCM_GPORT_IS_SET(port)) {

        /* GPORT Validations */
        BCM_IF_ERROR_RETURN(
           _bcm_esw_gport_resolve(
                          unit, port, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
                           _bcm_esw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            return BCM_E_PARAM;
        }

        /* For subports get from EGR_LPORT_PROFILE table */
        if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
            return bcm_esw_port_egr_lport_field_get(unit,
                                                    port,
                                                    EGR_LPORT_PROFILEm,
                                                    field,
                                                    mac);
        }


    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));
    }

    SOC_IF_ERROR_RETURN(
            READ_EGR_PORTm(unit, MEM_BLOCK_ANY, local_port, &egr_port_entry));

    *mac = soc_EGR_PORTm_field32_get(unit, &egr_port_entry, field);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_monterey_port_mac_set
 * Description:
 *      Configure EGR_PORT/LPORT table mac address
 * Parameters:
 *      unit  - (IN) BCM device number
 *      port  - (IN) Port number if valid.
 *      oui   - (IN) Indicates mac field is OUI/Non-OUI
 *      mac   - (IN) OUI/Non-OUI part of MAC-ADDRESS
 * Return Value:
 *      BCM_E_XXX
 */
int
_bcm_monterey_port_mac_set(int unit, bcm_port_t port, int oui, uint32 mac)
{
    egr_port_entry_t   egr_port_entry;
    bcm_module_t       modid;
    bcm_port_t         local_port;
    bcm_trunk_t        tgid;
    int                id;
    int                is_local = FALSE;
    soc_field_t        field;

    if (oui) {
        field = MAC_ADDRESS_UPPERf;
    } else {
        field = MAC_ADDRESS_LOWERf;
    }

    if (BCM_GPORT_IS_SET(port)) {

        /* GPORT Validations */
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(
                                   unit, port, &modid, &local_port, &tgid, &id));

        if (-1 == local_port) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
                           _bcm_esw_modid_is_local(unit, modid, &is_local));
        if (!is_local) {
            return BCM_E_PARAM;
        }

        /* For subports set the EGR_LPORT_PROFILE table */
        if (BCM_GPORT_IS_SUBPORT_PORT(port)) {
            return bcm_esw_port_egr_lport_field_set(unit,
                                                    port,
                                                    EGR_LPORT_PROFILEm,
                                                    field,
                                                    mac);
        }

    } else {
        BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));
    }


    SOC_IF_ERROR_RETURN(
                 READ_EGR_PORTm(unit, MEM_BLOCK_ANY, port, &egr_port_entry));

    soc_EGR_PORTm_field_set(unit, &egr_port_entry, field, &mac);

    SOC_IF_ERROR_RETURN(
            WRITE_EGR_PORTm(unit, MEM_BLOCK_ALL, port, &egr_port_entry));

    return BCM_E_NONE;
}



/*
 * FlexPort Operations Changes
 *
 * Flags to be used to determine the type of operations required
 * when the FlexPort API multi_set() is called.
 *
 * OP_NONE  - No changes.
 * OP_PMAP  - Change in port mapping.  It requires FlexPort sequence.
 * OP_LANES - Change in lanes.  It requires FlexPort sequence.
 * OP_SPEED - Change in speed.  This is covered in a FlexPort sequence.
 *            A change in speed may require a FlexPort sequence.
 *            If it is not required, then calling the 'speed' set
 *            function is sufficient.
 * OP_ENCAP - Change in encap mode.  It is NOT covered in a FlexPort
 *            sequence, so an explicit call must be made to change
 *            the encap mode.
 * OP_ALL   - All the above.  It requires FlexPort sequence.  
 */
#define BCM_MN_PORT_RESOURCE_OP_NONE     0
#define BCM_MN_PORT_RESOURCE_OP_PMAP    (1 << 0)
#define BCM_MN_PORT_RESOURCE_OP_LANES   (1 << 1)
#define BCM_MN_PORT_RESOURCE_OP_SPEED   (1 << 2)
#define BCM_MN_PORT_RESOURCE_OP_ENCAP   (1 << 3)
#define BCM_MN_PORT_RESOURCE_OP_ALL           \
    (BCM_MN_PORT_RESOURCE_OP_PMAP  |          \
     BCM_MN_PORT_RESOURCE_OP_LANES |          \
     BCM_MN_PORT_RESOURCE_OP_SPEED |          \
     BCM_MN_PORT_RESOURCE_OP_ENCAP) 

/*
 * Internal BCM Port Resource Configuration
 */
typedef struct bcm_monterey_port_resource_s {
    uint32 flags;              /* SOC_PORT_RESOURCE_XXX */
    bcm_gport_t port;          /* Local logical port number to connect to
                                  the given physical port */
    int physical_port;         /* Local physical port, -1 if the logical to
                                  physical mapping is to be deleted */
    int speed;                 /* Initial speed after FlexPort operation */
    int lanes;                 /* Number of PHY lanes for this physical port */
    bcm_port_encap_t encap;    /* Encapsulation mode for port */
    int roe_compression;       /* cpri compression enabled */
} bcm_monterey_port_resource_t;



/*
 * Forward static function declaration
 */
STATIC int
_bcm_monterey_port_resource_multi_set(int unit, 
                                    int nport, bcm_port_resource_t *resource);
STATIC int
_bcm_monterey_port_resource_speed_set(int unit, bcm_port_t port, int speed);


/*
 * Device Specific HW Tables
 */

/* Egr port table */
static bcmi_xgs5_port_redirection_egr_port_table_t
            bcmi_monterey_port_redirection_egr_port_table = {
    /* mem */ 	              EGR_PORTm,
    /* dest_type */           ER_REDIR_DEST_TYPEf,
    /* dest_value */          ER_REDIR_DESTINATIONf,
    /* drop_original */       ER_REDIR_DROP_ORIGINALf,
    /* pkt_priority */        ER_REDIR_NEW_PRIf,
    /* pkt_change_priority */ ER_REDIR_CHANGE_PRIf,
    /* pkt_color */           ER_REDIR_NEW_COLORf,
    /* pkt_change_color */    ER_REDIR_CHANGE_COLORf,
    /* strength */            ER_REDIR_STRENGTHf,
    /* buffer_priority */     ER_REDIR_RDB_PRIORITYf,
    /* action */              ER_REDIR_ACTIONf,
    /* redir_pkt_source */    ER_REDIR_SOURCEf,
    /* redir_pkt_truncate */  ER_REDIR_FIRST_CELL_ONLYf
};

/* HW Definitions */
static bcmi_xgs5_port_hw_defs_t  bcmi_monterey_port_hw_defs;


/*
 * Function Vector
 */
static bcm_esw_port_drv_t bcm_monterey_port_drv = {
    /* fn_drv_init                */ bcmi_monterey_port_fn_drv_init,
    /* resource_set               */ bcmi_xgs5_port_resource_set,
    /* resource_get               */ bcmi_xgs5_port_resource_get,
    /* resource_multi_set         */ _bcm_monterey_port_resource_multi_set,
    /* resource_speed_set         */ _bcm_monterey_port_resource_speed_set,
    /* resource_traverse          */ bcmi_xgs5_port_resource_traverse,
    /* port_redirect_set          */ bcmi_xgs5_port_redirect_config_set,
    /* port_redirect_get          */ bcmi_xgs5_port_redirect_config_get,
    /* port_enable_set            */ NULL,
    /* encap_speed_check          */ NULL,
    /* force_lb_set               */ NULL,
    /* force_lb_check             */ NULL,
    /* resource_status_update     */ bcmi_xgs5_port_resource_status_update

};


/*
 * Function:
 *      bcmi_monterey_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcmi_monterey_port_fn_drv_init(int unit)
{
    /* HW Definition Tables */
    sal_memset(&bcmi_monterey_port_hw_defs, 0,
               sizeof(bcmi_monterey_port_hw_defs));
    bcmi_monterey_port_hw_defs.egr_port =
        &bcmi_monterey_port_redirection_egr_port_table;

    /* Initialize Common XGS5 Port module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_fn_drv_init(unit, &bcm_monterey_port_drv, NULL,
                            &bcmi_monterey_port_hw_defs));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_attach
 * Purpose:
 *      Attach given port to the BCM layer and initialize it
 *      to its default state.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes port is a valid BCM logical port.
 *      - Assumes function is called when a new port is added into the system,
 *        so it is not called during WarmBoot.
 *      - Assumes BCM PORT module initialization routine has
 *        been called.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_monterey_port_attach(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;
     rv = bcm_mn_cosq_default_llfc_profile_attach(unit, port);
     if (BCM_FAILURE(rv)) {
         LOG_ERROR(BSL_LS_BCM_PORT,
         (BSL_META_U(unit,
                    "Unable to add default entries for PRIO2COS_PROFILE"
                     "register profile unit=%d port=%d rv=%d\n"),
                     unit, port, rv));
          return rv;
    }
    rv = bcm_esw_td2p_flexport_port_attach(unit, port);
    BCM_IF_ERROR_RETURN(rv);
    BCM_IF_ERROR_RETURN(

    bcm_mn_cosq_flex_port_update( unit , port , TRUE));
     rv = _bcm_mn_port_hsp_config_set(unit, port);
      if (BCM_FAILURE(rv)) {
          LOG_ERROR(BSL_LS_BCM_PORT,
                   (BSL_META_U(unit,
                             "Error: Unable config HSP node "
                               "unit=%d port=%d rv=%d\n"),
                       unit, port, rv));
            return rv;
        }
        SOC_IF_ERROR_RETURN(
           soc_monterey_port_mmu_tx_enable_set(unit, port, 1));
        
    SOC_IF_ERROR_RETURN(
       soc_monterey_port_edb_reset(unit, port));
 
    rv = soc_monterey_mmu_config_init(unit, 0, port);
    BCM_IF_ERROR_RETURN(rv);


    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_detach
 * Purpose:
 *      Detach given port from the BCM layer and clear any data
 *      that is normally configured when a port is initialized.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes port is a valid BCM logical port.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_monterey_port_detach(int unit, bcm_port_t port)
{
    bcm_gport_t gport;
    int rv = BCM_E_NONE;

    rv = bcm_esw_td2p_flexport_port_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Error: Unable to detach BCM port "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
    
    rv = bcm_mn_cosq_llfc_profile_detach(unit, port);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
        (BSL_META_U(unit,
        "Unable to delete default entries for PRIO2COS_PROFILE"
        "register profile unit=%d port=%d rv=%d\n"),
          unit, port, rv));
        return rv;
   } 
    /* Delete CoS queue tree */
    rv = bcm_esw_port_gport_get(unit, port, &gport);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Error: Unable to retrieve gport "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }

    rv = bcm_mn_cosq_gport_delete(unit, gport);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Error: Unable to destroy LLS tree "
                              "unit=%d port=%d rv=%d\n"),
                   unit, port, rv));
        return rv;
    }
    BCM_IF_ERROR_RETURN(
    bcm_mn_cosq_flex_port_update( unit , port , FALSE));

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_monterey_flexport_pbmp_update
 * Description:
 *      Returns bitmap of max valid ports
 * Parameters:
 *      unit    - (IN) BCM device number
 *      pbmp    - (IN/OUT) port bit map
 */

int
bcm_monterey_flexport_pbmp_update(int unit, bcm_pbmp_t *pbmp)
{
    bcm_port_t port;
    if (NULL == pbmp) {
        return BCM_E_PARAM;
    }
    for (port = 0; port < MN_FLEX_MAX_NUM_PORTS; port++) {
        BCM_PBMP_PORT_ADD(*pbmp, port);
    }
    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_monterey_port_resource_attach
 * Purpose:
 *      Attach ports to the BCM layer and initialize them
 *      to the default state.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes caller have detached any ports that will be attached
 *        in this routine.
 *      - Assumes this is NOT called during Warmboot.
 *      - Set current MY_MODID for all newly attached ports.
 */
STATIC int
_bcm_monterey_port_resource_attach(int unit,
                               int nport,
                               bcm_monterey_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_monterey_port_resource_t *pr;
    int i;
    int my_modid;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Attach ---\n")));
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));


    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_ATTACH) {
            rv = _bcm_monterey_port_attach(unit, pr->port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to attach BCM port "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->port, rv));
                return rv;
            }
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_set(unit, my_modid));

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_detach
 * Purpose:
 *      Detach ports from the BCM layer.
 *
 *      It clears any HW or SW state that was programmed
 *      by the SDK initialization routines.  It does NOT clear
 *      references to logical ports that have been programmed by the
 *      application.  Applications are responsible for doing that.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcm_monterey_port_resource_detach(int unit,
                               int nport,
                               bcm_monterey_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_monterey_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Detach ---\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            rv = _bcm_monterey_port_detach(unit, pr->port);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to detach BCM port "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->port, rv));
                return rv;
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_resolve
 * Purpose:
 *      Convert logical port number GPORT to BCM local port format and
 *      validate port numbers:
 *      - Logical and physical port numbers are within the valid range.
 *      - Ports must not be CPU or Loopback ports.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcm_monterey_port_resource_resolve(int unit, 
                                  int nport,
                                  bcm_monterey_port_resource_t *resource)
{
    int i;
    bcm_monterey_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that logical port number is addressable and convert */
        BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_addressable_local_get(unit,
                                                  pr->port,
                                                  &pr->port));

        /*
         * Check that logical port number is valid for Apache
         * (above check is generic)
         */
        SOC_IF_ERROR_RETURN
            (soc_mn_port_addressable(unit, pr->port));

        /* Check that physical port is within the valid range */
        if (pr->physical_port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_mn_phy_port_addressable(unit, pr->physical_port));
        }

        /* Check that ports, logical and physical, are not a management port */
        if ((si->port_l2p_mapping[pr->port] == MN_PHY_PORT_CPU) ||
            (si->port_l2p_mapping[pr->port] == MN_PHY_PORT_LB) ||
            (pr->physical_port == MN_PHY_PORT_CPU) ||
            (pr->physical_port == MN_PHY_PORT_LB)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port cannot be CPU, LB, or RDB port "
                                  "unit=%d port=%d\n"),
                       unit, pr->port));
            return BCM_E_PORT;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_input_validate
 * Purpose:
 *      Validate function input requirements.
 *
 *      This routine checks for function semantics and guidelines
 *      from the API perspective:
 *      - Check the order of elements in array, 'delete' operations must
 *        be placed first in array.
 *      - Ports must not be CPU or Loopback ports.
 *      - Ports must be disabled.
 *      - Ports linkscan mode must be NONE.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 *      - Assumes port values has been validated.
 */
STATIC int
_bcm_monterey_port_resource_input_validate(int unit, 
                                         int nport,
                                         bcm_monterey_port_resource_t *resource)
{
    int i;
    bcm_monterey_port_resource_t *pr;
    int delete = 1;
    int enable;
    soc_info_t *si = &SOC_INFO(unit);

    /* Check array order and port state disable */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Skip check for speed change */
        if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
            continue;
        }

        /* Check that delete operations are first */
        if (pr->physical_port != -1) {  /* First non-delete found */
            delete = 0;
        } else if ((pr->physical_port == -1) && !delete) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Delete operations must be "
                                  "first in array\n")));
            return BCM_E_CONFIG;
        }

        if ((pr->encap != BCM_PORT_ENCAP_IEEE) &&
            (pr->encap != BCM_PORT_ENCAP_HIGIG2) &&
            (pr->encap != BCM_PORT_ENCAP_CPRI) &&
            (pr->encap != BCM_PORT_ENCAP_RSVD4)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Encap modes can only be IEEE or HIGIG2 or CPRI "
                                  "unit=%d port=%d\n"),
                       unit, pr->port));
            return BCM_E_CONFIG;
        }

        /* Skip ports that are not currently defined */
        if (si->port_l2p_mapping[pr->port] == -1) {
            continue;
        }

        /* Check that ports are disabled */
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_enable_get(unit, pr->port, &enable));
        if (enable) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Port %d needs to be disabled\n"),
                       pr->port));
            return BCM_E_BUSY;
        }

        /*
         * Check that ports to be 'detached' do not
         * have linkscan mode set.  See _bcm_monterey_port_resource_pmap_get().
         */
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            if (bcm_esw_linkscan_enable_port_get(unit, pr->port) !=
                BCM_E_DISABLED) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Linkscan mode needs to be "
                                      "disabled on ports to be detached, "
                                      "port=%d\n"),
                           pr->port));
                return BCM_E_BUSY;
            }
        }
    }

    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_monterey_port_resource_pmap_get
 * Purpose:
 *      Determine the type of port mapping changes, if any,
 *      as a result of the FlexPort operation.  In addition, it
 *      determines whether the port needs to go through the attach
 *      and/or detach process.
 *
 *      All ports that are being flexed will be detached except for those
 *      that will become inactive (this is only for legacy mode).
 *      All ports that are active after flexed are always attached (this is
 *      for new and legacy mode).
 *
 *      When a port is 'marked' as 'delete' (physical port is -1),
 *      this does not mean the logical port will be destroyed.  Depending
 *      on the rest of the information in the array, the port mapping
 *      may be the same, remapped, or it may no longer exist after FlexPort.
 *
 *      Likewise, when a valid port mapping is given, the port may
 *      not be necessarily new.
 *
 *      Example:
 *
 *      Main FlexPort array ('resource') (application input)
 *               L1 --> -1 (current mapped to P1), mapping is SAME
 *               L2 --> -1 (current mapped to P2), mapping is REMAP
 *               L3 --> -1 (current mapped to P3), mapping is DESTROY
 *               L1 --> P1                       , mapping is SAME
 *               L2 --> P5                       , mapping is REMAP
 *               L9 --> P9                       , mapping is NEW
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes all 'delete' elements are first in array.
 */
STATIC int
_bcm_monterey_port_resource_pmap_get(int unit, int nport,
                                   bcm_monterey_port_resource_t *resource)
{
    int i;
    int j;
    bcm_monterey_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        if (pr->physical_port == -1) {
            /*
             * Find if port is given in later elements.
             * If so, check if port is remapped.
             * If not found, port is marked to be destroyed
             * (port will no longer exist after FlexPort)
             *
             * Note that all the 'delete' ports must always first
             * in array.
             */
            for (j = i; j < nport; j++) {
                if (resource[j].physical_port == -1) {
                    continue;
                }
                if (resource[j].port == pr->port) {  /* Found */
                    /* Check if mapping is same */
                    if (resource[j].physical_port !=
                        si->port_l2p_mapping[pr->port]) {
                        pr->flags |= SOC_PORT_RESOURCE_REMAP;
                    }
                    break;
                }
            }

            if (j >= nport) {  /* Not found */
                pr->flags |= SOC_PORT_RESOURCE_DESTROY;
            }

            /*
             * Detach only if the port is not becoming inactive (legacy).
             *
             * A port that is becoming inactive after flex is indicated
             * when both of the following flags are set:
             *     DESTROY
             *     I_MAP
             */
            if (!(pr->flags & SOC_PORT_RESOURCE_DESTROY) ||
                !(pr->flags & SOC_PORT_RESOURCE_I_MAP)) {
                pr->flags |= SOC_PORT_RESOURCE_DETACH;
            }

        } else {

            /*
             * Check if logical port is new (port does exist), or
             * port is being remapped.
             */
            if (si->port_l2p_mapping[pr->port] == -1) {
                pr->flags |= SOC_PORT_RESOURCE_NEW;
            } else if (pr->physical_port != si->port_l2p_mapping[pr->port]) {
                pr->flags |= SOC_PORT_RESOURCE_REMAP;
            }

            /* All active ports after flexed are always attached */
            pr->flags |= SOC_PORT_RESOURCE_ATTACH;
        }

    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_monterey_port_resource_op_get
 * Purpose:
 *      Get the type of FlexPort operations/changes:
 *          none, port-mapping, lanes, speed, encap.
 *
 *      Some operations only need speed or encap changes.  In this
 *      case, the actual FlexPort operation is not necessary and
 *      only calls to set to speed and/or encap mode are made.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      op       - (OUT) FlexPort operation BCM_MN_PORT_RESOURCE_OP_...
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Resource is not NULL.
 *      Logical port in 'resource' is in BCM port format (non GPORT).
 */
STATIC int
_bcm_monterey_port_resource_op_get(int unit, int nport,
                                bcm_monterey_port_resource_t *resource, int *op)
{
    int i;
    bcm_monterey_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);
    int speed = 0;
    int encap = 0;

    *op = BCM_MN_PORT_RESOURCE_OP_NONE;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check port speed */
        if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
            /*
             * NOTE: speed_set sequence
             *
             * pr->flags & SOC_PORT_RESOURCE_SPEED is set by speed_set API.
             * No logical-physical port mapping changes.
             * OP_PMAP flag doesn't need to be set in this case.
             * _bcm_mn_port_resource_speed_execute will be called.
             */
            *op |= BCM_MN_PORT_RESOURCE_OP_SPEED;
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                                    "Speed change detected: "
                                    "logical=%d physical=%d\n"),
                         pr->port, pr->physical_port));
            continue;
        }

        /* Check port mapping change */
        if (pr->flags &
            (SOC_PORT_RESOURCE_NEW | SOC_PORT_RESOURCE_DESTROY |
             SOC_PORT_RESOURCE_REMAP)) {
            *op |= BCM_MN_PORT_RESOURCE_OP_PMAP;

            LOG_VERBOSE(BSL_LS_BCM_PORT,
                        (BSL_META_U(unit,
                                    "Port map change detected: "
                                    "%s %s %s l=%d p=%d\n"),
                         (pr->flags & SOC_PORT_RESOURCE_NEW) ? "new" : "",
                         (pr->flags & SOC_PORT_RESOURCE_DESTROY) ?
                         "destroy" : "",
                         (pr->flags & SOC_PORT_RESOURCE_REMAP) ? "remap" : "",
                         pr->port, pr->physical_port));
        }

        /* Skip ports that are being deleted or not defined */
        if ((pr->physical_port == -1) ||
            (si->port_l2p_mapping[pr->port] == -1)) {
            continue;
        }

        /*
         * NOTE: Execute full flex operation if speed is different
         */
        if (!(*op & BCM_MN_PORT_RESOURCE_OP_SPEED)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, pr->port, &speed));
            if (pr->speed != speed) {
                *op |= BCM_MN_PORT_RESOURCE_OP_PMAP;
            }
        }

        /* Check port lanes */
        if (!(*op & BCM_MN_PORT_RESOURCE_OP_LANES)) {
            if (pr->lanes != si->port_num_lanes[pr->port]) {
                *op |= BCM_MN_PORT_RESOURCE_OP_LANES;
            }
        }

        /* Check port encap */
        if (!(*op & BCM_MN_PORT_RESOURCE_OP_ENCAP)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, pr->port, &encap));
            if (pr->encap != encap) {
                *op |= BCM_MN_PORT_RESOURCE_OP_ENCAP;
                if((encap == BCM_PORT_ENCAP_CPRI) || (encap == BCM_PORT_ENCAP_RSVD4)) {
                    *op |= BCM_MN_PORT_RESOURCE_OP_PMAP;
                }
            }
        }

        /* If all are set, no more checking is necessary */
        if (*op == BCM_MN_PORT_RESOURCE_OP_ALL) {
            break;
        }
    }
    
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_loopback_clear
 * Purpose:
 *      Clear the loopback mode for the given set of ports.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes all 'delete' elements are first in array.
 */
STATIC int
_bcm_monterey_port_resource_loopback_clear(int unit,
                                         int nport,
                                         bcm_monterey_port_resource_t *resource)
{
    int i;
    int loopback;
    bcm_monterey_port_resource_t *pr;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_loopback_get(unit,
                                                pr->port, &loopback));
            if (loopback != BCM_PORT_LOOPBACK_NONE) {
                BCM_IF_ERROR_RETURN(bcmi_esw_portctrl_loopback_set(unit,
                                                pr->port,
                                                BCM_PORT_LOOPBACK_NONE));
            }
        }
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_monterey_port_resource_speed_execute
 * Purpose:
 *      Perform a speed set operation.
 *      This includes speed change only
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated (basic API semantics)
 *
 * Details:
 *     This function is pretty similar as _bcm_monterey_port_resource_execute.
 *     But speed set doesn't change logical-physical port mapping.
 *     So this function should skip detach, attach function calls.
 *       _bcm_monterey_port_resource_detach
 *       _bcm_monterey_port_resource_attach
 *     Also this calls soc_monterey_port_resource_configure with
 *     the flag:SOC_PORT_RESOURCE_CONFIGURE_SPEED_SET to make slightly
 *     different sequence from full flex operation.
 */
STATIC int
_bcm_monterey_port_resource_speed_execute(int unit,
                                  int nport, bcm_monterey_port_resource_t *resource)
{
    int rv;
    int i;
    soc_port_resource_t *soc_resource;

    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for SOC FlexPort "
                              "array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].roe_compression = resource[i].roe_compression;
    }

    rv = soc_mn_port_resource_validate(unit, nport, soc_resource);
    if (SOC_FAILURE(rv)) {
        sal_free(soc_resource);
        return rv;
    }

    /* SOC Port Resource Configure */
    rv = soc_mn_port_resource_configure(unit, nport, soc_resource,
                        SOC_PORT_RESOURCE_CONFIGURE_SPEED_ONLY);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    sal_free(soc_resource);

    return rv;
}

/*
 * Function:
 *      _bcm_monterey_port_resource_execute
 * Purpose:
 *      Perform a FlexPort operation.
 *      This includes changes that involve:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated (basic API semantics)
 *
 * Details:
 *
 * Port Mapping
 * ------------
 * A logical to physical port mapping is given as part of the port resource
 * array.  This can lead to the following operations:
 * - Port Mapping Delete (or clear)
 *   This is indicated when the physical port is set to (-1) and
 *   the port to be flexed is considered an 'old' port.
 *   The logical port in the array entry must always be valid.
 *   A 'delete' may result in the following actions for a port mapping:
 *      (1) Destroyed
 *      (2) Same
 *      (3) Remapped
 *
 * - Port Mapping Add (or set)
 *   This is indicated when the physical port is set to a valid
 *   physical port number (>=0).
 *   The logical port in the array entry must always be valid.
 *   An 'add' may result in the following actions for a port mapping:
 *      (1) New
 *      (2) Same
 *      (3) Remapped
 *
 * Destroyed: Logical port no longer exist after FlexPort.
 * Remapped:  Logical port will continue to exist after FlexPort
 *            but is mapped to a different physical port.
 * New:       Logical port did not exist before FlexPort.
 * Same:      Logical to physical port mapping remains the same.
 */
STATIC int
_bcm_monterey_port_resource_execute(int unit, 
                                  int nport, bcm_monterey_port_resource_t *resource)
{
    int rv;
    int i;
    soc_port_resource_t *soc_resource;

    /*
     * Main FlexPort Sequence
     *
     * - SOC Validation:
     *     + More validation takes place in the SOC layer.
     *     + These check for abilities and configurations based on
     *       the physical aspects of the device.
     *
     * NOTE: Up to this point, no changes has taken place in SW
     *       data structures or HW.
     *       From this point forward, HW and SW data will be modified,
     *       any errors will leave the system unstabled.
     *
     * - Clear any port related settings in BCM layer as needed.
     *
     * - Detach 'old' ports from BCM layer:
     *     + These are all ports to be cleared/deleted (indicated by physical
     *       port -1), EXCEPT those that will become inactive after the
     *       FlexPort operation (this is only true with legacy API).
     *     + The port state should be the same as the state of a
     *       port that was never configured during SDK initialization.
     *     + This involves SW and HW changes.
     *
     * - SOC Port Resource Configure:
     *     Takes place in the SOC layer.
     *     It consists primarliy of these steps:
     *       + Delete ports in SOC layer (SOC_INFO SW, HW update).
     *       + Add ports in SOC layer (SOC_INFO SW, HW update).
     *       + Execute FlexPort operation as defined by HW specs.
     *
     * - Attach 'new' ports to BCM layer:
     *     + These are all ports that are active after the FlexPort operation.
     *     + Ports are initialized to a default state
     *       (same state when a port is brought up after SDK is initialized).
     *     + This involves SW and HW changes.
     *
     * - Set BCM properties on flexed ports as needed.
     */

    /*
     * SOC Port Structure initialization
     */
    /* Allocate memory for SOC Port Resource array data structure */
    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for SOC FlexPort "
                              "array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        soc_resource[i].flags = resource[i].flags;
        soc_resource[i].logical_port = resource[i].port;
        soc_resource[i].physical_port = resource[i].physical_port;
        soc_resource[i].speed = resource[i].speed;
        soc_resource[i].num_lanes = resource[i].lanes;
        soc_resource[i].encap = resource[i].encap;
        soc_resource[i].roe_compression = resource[i].roe_compression;
    }

    /* SOC Validation */
    rv = soc_mn_port_resource_validate(unit, nport, soc_resource);
    if (SOC_FAILURE(rv)) {
        sal_free(soc_resource);
        return rv;
    }

    /* Detach ports from BCM layer */
    rv = _bcm_monterey_port_resource_detach(unit, nport, resource);

    /* SOC Port Resource Configure */
    if (BCM_SUCCESS(rv)) {
        rv = soc_mn_port_resource_configure(unit, nport, soc_resource,
                       SOC_PORT_RESOURCE_CONFIGURE_FLEX);
    }

    /* Attach ports to BCM layer */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_monterey_port_resource_attach(unit, nport, resource);
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    sal_free(soc_resource);

    return rv;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_configure
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - The data is modified by the function.
 *      - It takes the internal bcm_monterey_port_resource_t data type.
 */
STATIC int
_bcm_monterey_port_resource_configure(int unit, 
                                    int nport,
                                    bcm_monterey_port_resource_t *resource)
{
    int op;
    int i;

    /* Convert port format and validate logical and physical port numbers */
    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_resolve(unit,
                                         nport, resource));

    /* Get port mapping change */
    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_pmap_get(unit, nport, resource));

    /* clear loopback */
    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_loopback_clear(unit, nport, resource));

    /*
     * Validate function input requirements
     *
     * This steps checks for function semantics and guidelines
     * from the API perspective.
     */
    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_input_validate(unit, nport, resource));

    /*
     * FlexPort operations types:
     *     NONE                             => Nothing to do
     *     PMAP or LANES                    => FlexPort sequence
     *     ENCAP                            => Encap set
     *     SPEED, but not PMAP or LANES     => Speed set
     */
    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_op_get(unit, nport, resource, &op));


    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "FlexPort operations 0x%x: %s %s %s %s\n"),
                 op,
                 (op & BCM_MN_PORT_RESOURCE_OP_PMAP) ? "pmap" : "",
                 (op & BCM_MN_PORT_RESOURCE_OP_SPEED) ? "speed" : "",
                 (op & BCM_MN_PORT_RESOURCE_OP_LANES) ? "lanes" : "",
                 (op & BCM_MN_PORT_RESOURCE_OP_ENCAP) ? "encap" : ""));

    if (op == BCM_MN_PORT_RESOURCE_OP_NONE) {
        return BCM_E_NONE;     /* No changes, just return */
    }

    /*
     * Port Mapping / Lane
     *
     * These changes always require a FlexPort operation.
     */
    if ((op & BCM_MN_PORT_RESOURCE_OP_PMAP) ||
        (op & BCM_MN_PORT_RESOURCE_OP_LANES)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "--- Execute FlexPort sequence ---\n")));
        BCM_IF_ERROR_RETURN
            (_bcm_monterey_port_resource_execute(unit,
                                           nport, resource));
        for (i = 0; i < nport; i++) {
            if (resource[i].physical_port == -1) {
                continue;
            }

            /* Bring new ports down to ensure port state is proper */
            BCM_IF_ERROR_RETURN
                (bcmi_esw_portctrl_enable_set(unit, resource[i].port, 0));
       }
    } else if (op & BCM_MN_PORT_RESOURCE_OP_SPEED) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "--- Execute Speed Set sequence ---\n")));
        BCM_IF_ERROR_RETURN
            (_bcm_monterey_port_resource_speed_execute(unit,
                                                     nport, resource));
    }

    /*
     * Encap
     */
    if (op & BCM_MN_PORT_RESOURCE_OP_ENCAP) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Execute Encap change\n")));
        for (i = 0; i < nport; i++) {
            if (resource[i].physical_port == -1) {
                continue;
            }

            BCM_IF_ERROR_RETURN
                (bcmi_esw_portctrl_encap_set(unit, resource[i].port,
                                             resource[i].encap, TRUE));
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_monterey_port_resource_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 *      - It takes the public bcm_monterey_port_resource_t data type.
 */
STATIC int
_bcm_monterey_port_resource_multi_set(int unit, 
                                    int nport,
                                    bcm_port_resource_t *resource)
{
    int rv;
    int i;
    bcm_monterey_port_resource_t *port_resource;

    if (resource == NULL) {
        return BCM_E_PARAM;
    }

    /*
     * Use internal port resource structure type.
     * Called routines will be modifying the data, so
     * having a copy avoids modifying user's data.
     */
    port_resource = sal_alloc(sizeof(bcm_monterey_port_resource_t) * nport,
                              "port_resource");
    if (port_resource == NULL) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for internal "
                              "FlexPort array\n")));
        return BCM_E_MEMORY;
    }

    /* Clear data structure */
    sal_memset(port_resource, 0, sizeof(bcm_monterey_port_resource_t) * nport);

    /* Copy initial information */
    for (i = 0; i < nport; i++) {
        /*
         * No current flags defined in bcm_monterey_port_resource_t, otherwise
         * need to translate from public to internal flags.
         */
        port_resource[i].flags = resource[i].flags;
        port_resource[i].port = resource[i].port;
        port_resource[i].physical_port = resource[i].physical_port;
        port_resource[i].speed = resource[i].speed;
        port_resource[i].lanes = resource[i].lanes;
        port_resource[i].encap = resource[i].encap;
        port_resource[i].roe_compression = resource[i].roe_compression;
    }

    rv = _bcm_monterey_port_resource_configure(unit, nport, port_resource);

    sal_free(port_resource);

    return rv;
}

/*
 * Function:
 *      _bcm_monterey_port_resource_speed_set
 * Purpose:
 *      Changes the speed but keeps the same number of lanes
 * Parameters:
 *      unit     - (IN) Unit number
 *      port     - (IN) Logical port number(bcm_port_t, not bcm_gport_t)
 *      speed    - (IN) Port speed
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 */
STATIC int
_bcm_monterey_port_resource_speed_set(int unit, bcm_port_t port, int speed)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_monterey_port_resource_t resource[2]; /* resource for pre and post */
    int cur_lanes,  cur_encap;
    int phy_port;

 /* si->port_speed_max is the current speed at which MMU/TDM is configured */
    if (si->port_speed_max[port] == speed  || (speed == 0)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                    "Speed is already configured as %d\n"), speed));
        return BCM_E_NONE;
    }

    /* Check current number of lanes */
    BCM_IF_ERROR_RETURN(bcmi_monterey_port_lanes_get(unit, port, &cur_lanes));

    /* Get current port encap */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &cur_encap));

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     */
    phy_port = si->port_l2p_mapping[port];
    /* Check valid physical port */
    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port "
                              "for logical port %d\n"),
                   port));
        return BCM_E_INTERNAL;
    }

    resource[0].flags = SOC_PORT_RESOURCE_SPEED;
    resource[0].port = port;
    resource[0].physical_port = -1;

    resource[1].flags = SOC_PORT_RESOURCE_SPEED;
    resource[1].port = port;
    resource[1].physical_port = phy_port;
    resource[1].speed = speed;
    resource[1].lanes = cur_lanes;
    resource[1].encap = cur_encap;
    resource[1].roe_compression = 0;

    BCM_IF_ERROR_RETURN(_bcm_monterey_port_resource_configure(unit, 2, resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_monterey_port_lanes_get
 * Purpose:
 *      Get the number of PHY lanes for the given port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (OUT) Returns number of lanes for port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_monterey_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    bcm_port_resource_t resource;

    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &resource));

    *lanes = resource.lanes;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_monterey_port_lanes_set
 * Purpose:
 *      Set the number of PHY lanes for the given port.
 *
 *      This function should only be used to support the legacy
 *      FlexPort API bcm_port_control_set(... , bcmPortControlLanes).
 *
 *      The legacy API does not delete the port mappings when flexing
 *      to fewer lanes.  Ports which lanes are used by the base
 *      physical port becomes inactive.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of lanes to set on given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Only to be used by legacy API to support TD2 behavior
 *        bcm_port_control_set(... , bcmPortControlLanes).
 *      - Supports only up to 4 lanes (TSC-4).
 *      - 'port' must be in BCM port format (non-GPORT) and mapped to
 *        the base physical port.
 *      - Several calls to legacy API may be required to
 *        achieve desired configuration.
 */
int
bcmi_monterey_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_monterey_port_resource_t resource[8];  /* Max size to support up to 4 lanes */
    int max_array_cnt;
    int i;
    int cur_lanes;
    int cur_speed;
    int cur_encap = 0;
    int phy_port;
    int speed;
    int num_ports_clear;
    int num_ports_new;
    int falconport = 0; 

    /* Support legacy API up to 4 lanes */
    if ((lanes != 1) && (lanes != 2) && (lanes != 4)) {
        return BCM_E_PARAM;
    }

    /* Check current number of lanes */
    BCM_IF_ERROR_RETURN(bcmi_monterey_port_lanes_get(unit, port, &cur_lanes));
    if (cur_lanes == lanes) {
        return BCM_E_NONE;
    }

    /* Check that port can support number of lanes specified */
    SOC_IF_ERROR_RETURN(soc_mn_port_lanes_valid(unit, port, lanes));

    /* Get current port speed */
    BCM_IF_ERROR_RETURN(bcm_esw_port_speed_get(unit, port, &cur_speed));

    /* Get current port encap */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &cur_encap));

    /* Clear array */
    max_array_cnt = COUNTOF(resource);
    sal_memset(resource, 0, sizeof(resource));

    /*
     * Build port resource array
     * Certain information is derived from 'best' guess,
     * similar to the legacy behaviour.
     */

    /*
     * Select speed based on number of lanes
     *
     * For HG, if the current HG speed is the higher speed
     * (i.e. 42 vs 40) select the higher HG speed for the number of lanes.
     * Example
     *    1xHG[42] --> 4xHG[11]
     *    1xHG[40] --> 4xHG[10]
     *
     * Notes:  Assumes encap is HIGIG2 if it is not IEEE
     */
   if ( MONTEREY_PORT_IS_PM4x25_PORT(unit, port) 
        || MONTEREY_PORT_IS_CPM4x25_PORT(unit, port)) { 
        falconport  = 1; 
    }
    switch(lanes) {
    case 1:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = falconport ? 25000: 10000;
         } else if (cur_encap == BCM_PORT_ENCAP_CPRI) {    /* IEEE */
            speed = 10000;
        } else {                                   /* HG2  */
            speed = falconport ? 27000: 11000;
        }
        break;
    case 2:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = falconport ? 50000: 20000;
        } else {                                   /* HG2  */
            speed = falconport ? 53000: 21000;
        }

        break;
    case 4:
        if (cur_encap == BCM_PORT_ENCAP_IEEE) {    /* IEEE */
            speed = falconport ? 100000: 40000;
        } else {                                   /* HG2  */
            speed = PORT_IS_PM4x25_PORT(unit, port) ? 106000: 42000;
        }
        break;
    /*
     * COVERITY
     *
     * Case for default is intentional as a defensive check.
     */
    /* coverity[dead_error_begin] */

    default:
        return BCM_E_PARAM;
        break;
    }

    switch (cur_lanes) {
    case 1:
        /*
         * 2x10 flex to: 2 lanes -> 1x20
         * 4x10 flex to: 4 lanes -> 1x40
         */
        if (lanes == 2) {
            num_ports_clear = 2;
            num_ports_new = 1;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 2:
        /*
         * 1x20 flex to: 1 lane  -> 2x10
         * 2x20 flex to: 4 lanes -> 1x40
         */
        if (lanes == 1) {
            num_ports_clear = 2;
            num_ports_new = 2;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 4:
        /*
         * 1x40 flex to: 1 lane  -> 4x10
         *               2 lanes -> 2x20
         */
        if (lanes == 1) {
            num_ports_clear = 4;
            num_ports_new = 4;
        } else {
            num_ports_clear = 4;
            num_ports_new = 2;
        }
        break;
    default:
        return BCM_E_CONFIG;
        break;
    }

    if ((num_ports_clear + num_ports_new) > max_array_cnt) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid array FlexPort calculation "
                              "num_ports_clear=%d num_ports_new=%d "
                              "max_array=%d\n"),
                   num_ports_clear, num_ports_new, max_array_cnt));
        return BCM_E_INTERNAL;
    }

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     * Assume physical ports are numbered consecutively in device.
     */
    phy_port = si->port_l2p_mapping[port];
    for (i = 0; i < num_ports_clear; i++) {
        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port++];
        resource[i].physical_port = -1;
    }


    /*
     * Map new ports
     *
     * Legacy API requires that logical-physical port mapping
     * is valid in the SOC_INFO SW data.
     */
    phy_port = si->port_l2p_mapping[port];
    for (; i < (num_ports_clear + num_ports_new); i++) {
        /* Check valid physical port */
        if (phy_port == -1) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid physical port "
                                  "for logical port %d\n"),
                       port));
            return BCM_E_INTERNAL;
        }

        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port];
        resource[i].physical_port = phy_port;
        resource[i].lanes = lanes;
        resource[i].speed = speed;
        resource[i].encap = cur_encap;
        phy_port += lanes;
    }


    BCM_IF_ERROR_RETURN
        (_bcm_monterey_port_resource_configure(unit,
                                             (num_ports_clear + num_ports_new),
                                             resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_monterey_port_drain_cells
 * Purpose:
 *     To drain cells associated to the port.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) Port
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_monterey_port_drain_cells(int unit, int port)
{
    return bcmi_esw_portctrl_egress_queue_drain(unit, port);
}

int
bcm_monterey_port_drain_cells(int unit, int port)
{
    return (_bcm_monterey_port_drain_cells(unit, port));
}

#else /* BCM_MONTEREY_SUPPORT */
typedef int bcm_esw_mn_port_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_MONTEREY_SUPPORT */
