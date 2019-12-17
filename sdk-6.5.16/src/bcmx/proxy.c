/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        proxy.c
 * Purpose:     BCMX Proxy API
 */

#ifdef  INCLUDE_L3

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>
#include "bcmx_int.h"

#include <bcmx/proxy.h>

#define BCMX_PROXY_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_PROXY_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)


/*
 * Function:
 *      bcmx_proxy_client_set
 * Purpose:
 *      Enables redirection for a certain traffic type using either 
 *      FFP or FP rule
 * Parameters:
 *      client_lport -  Logical port (Ingress)for which redirection is applied
 *      proto_type   -  Packet type to classify for redirection
 *      server_lport -  Logical Port where redirected packets are 
 *                      destined to
 *      enable       -  toggle to enable or disable redirection
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_proxy_client_set(bcmx_lport_t client_lport, 
                      bcm_proxy_proto_type_t proto_type,
                      bcmx_lport_t server_lport, int enable)
{
    bcm_port_t client_port, server_port;
    int unit;
    bcm_module_t server_modid;

    BCMX_PROXY_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(client_lport,
                                 &unit, &client_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(server_lport,
                                  &server_modid, &server_port,
                                  BCMX_DEST_CONVERT_DEFAULT));

    return (bcm_proxy_client_set(unit, client_port, proto_type,
                                 server_modid, server_port, enable));
}
                   
/*
 * Function:
 *      bcmx_proxy_server_set
 * Purpose:
 *      Enables various kinds of lookups for packets coming from remote
 *      (proxy client) devices
 * Parameters:
 *      server_lport -  Logical Port to which packets are redirected to
 *      mode         -  Indicates lookup type
 *      enable       -  TRUE to enable lookups
 *                      FALSE to disable lookups
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_proxy_server_set(bcmx_lport_t server_lport, bcm_proxy_mode_t mode,
                      int enable)
{                       
    int unit;
    bcm_port_t server_port;

    BCMX_PROXY_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(server_lport, &unit, &server_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return (bcm_proxy_server_set(unit, server_port, mode, enable));
}

/*
 * Function:
 *      bcmx_proxy_server_get
 * Purpose:
 *      Get the lookup mode of XGS3 device.
 * Parameters:
 *      server_lport -  Logical port to which packets are redirected to
 *      status      -  (OUT) lookup mode
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_proxy_server_get(bcmx_lport_t server_lport, bcm_proxy_mode_t mode, 
                      int *enable)
{
    int unit;
    bcm_port_t server_port;

    BCMX_PROXY_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(server_lport, &unit, &server_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return (bcm_proxy_server_get(unit, server_port, mode, enable));
}

/*
 * Function:
 *      bcmx_proxy_init
 * Purpose:
 *      Initialize Proxy module system-wide
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 */
int
bcmx_proxy_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_PROXY_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_proxy_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_PROXY_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_proxy_cleanup
 * Purpose:
 *     Disable Proxy system-wide
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_proxy_cleanup(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_PROXY_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_proxy_cleanup(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_PROXY_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}



#endif  /* INCLUDE_L3 */

int _bcmx_proxy_not_empty;
