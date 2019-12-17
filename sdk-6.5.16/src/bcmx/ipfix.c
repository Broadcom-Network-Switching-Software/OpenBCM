/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/ipfix.c
 * Purpose:     BCMX IP Flow Information Export (IPFIX) APIs
 */

#include <sal/core/libc.h>

#include <bcm/types.h>

#include <bcmx/lport.h>
#include <bcmx/ipfix.h>

#include "bcmx_int.h"

#define BCMX_IPFIX_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_IPFIX_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_IPFIX_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_IPFIX_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_IPFIX_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_ipfix_config_t *)(_config))

#define BCMX_IPFIX_RATE_T_PTR_TO_BCM(_rate_info)    \
    ((bcm_ipfix_rate_t *)(_rate_info))

#define BCMX_IPFIX_MIRROR_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_ipfix_mirror_config_t *)(_config))


/*
 * Function:
 *     bcmx_ipfix_config_t_init
 */
void
bcmx_ipfix_config_t_init(bcmx_ipfix_config_t *config)
{
    if (config != NULL) {
        bcm_ipfix_config_t_init(BCMX_IPFIX_CONFIG_T_PTR_TO_BCM(config));
    }
}

/*
 * Function:
 *     bcmx_ipfix_rate_t_init
 */
void
bcmx_ipfix_rate_t_init(bcmx_ipfix_rate_t *rate_info)
{
    if (rate_info != NULL) {
        bcm_ipfix_rate_t_init(BCMX_IPFIX_RATE_T_PTR_TO_BCM(rate_info));
    }
}
    
/*
 * Function:
 *     bcmx_ipfix_mirror_config_t_init
 */
void
bcmx_ipfix_mirror_config_t_init(bcmx_ipfix_mirror_config_t *config)
{
    if (config != NULL) {
        bcm_ipfix_mirror_config_t_init
            (BCMX_IPFIX_MIRROR_CONFIG_T_PTR_TO_BCM(config));
    }
}


/*
 * Function:
 *     bcmx_ipfix_config_set
 */
int bcmx_ipfix_config_set(bcm_ipfix_stage_t stage, bcmx_lport_t port, 
                          bcmx_ipfix_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_config_set(bcm_unit, stage, bcm_port,
                                BCMX_IPFIX_CONFIG_T_PTR_TO_BCM(config));
}


/*
 * Function:
 *     bcmx_ipfix_config_get
 */
int
bcmx_ipfix_config_get(bcm_ipfix_stage_t stage, bcmx_lport_t port, 
                      bcmx_ipfix_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_config_get(bcm_unit, stage, bcm_port,
                                BCMX_IPFIX_CONFIG_T_PTR_TO_BCM(config));
}


/*
 * Function:
 *     bcmx_ipfix_rate_create
 * Purpose:
 *     Create an IPFIX flow rate meter entry.
 * Parameters:
 *     rate_info  - (IN/OUT) Pointer to rate information
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_create(bcmx_ipfix_rate_t *rate_info)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;
    uint32  flags_orig;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(rate_info);

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = rate_info->flags & BCM_IPFIX_RATE_VIOLATION_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_create(bcm_unit,
                                       BCMX_IPFIX_RATE_T_PTR_TO_BCM
                                       (rate_info));
        if (BCM_FAILURE(BCMX_IPFIX_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if rate ID
         * is not specified.
         */
        if (!(rate_info->flags & BCM_IPFIX_RATE_VIOLATION_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                rate_info->flags |= BCM_IPFIX_RATE_VIOLATION_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    rate_info->flags &= ~BCM_IPFIX_RATE_VIOLATION_WITH_ID;
    rate_info->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_ipfix_rate_destroy
 * Purpose:
 *     Destroy an IPFIX flow rate meter entry.
 * Parameters:
 *     rate_id - Rate identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_destroy(bcm_ipfix_rate_id_t rate_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_destroy(bcm_unit, rate_id);
        BCM_IF_ERROR_RETURN
            (BCMX_IPFIX_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_ipfix_rate_get
 * Purpose:
 *     Get IPFIX flow rate meter entry for the specified id.
 * Parameters:
 *     rate_info - (IN/OUT) Rate information
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_get(bcmx_ipfix_rate_t *rate_info)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(rate_info);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_ipfix_rate_get(bcm_unit,
                                BCMX_IPFIX_RATE_T_PTR_TO_BCM(rate_info));
        if (BCMX_IPFIX_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_ipfix_rate_destroy_all
 * Purpose:
 *     Destroy all IPFIX flow rate meter entries.
 * Parameters:
 *     None
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_destroy_all(void)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_destroy_all(bcm_unit);
        BCM_IF_ERROR_RETURN
            (BCMX_IPFIX_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_ipfix_rate_mirror_add
 * Purpose:
 *     Add a mirror destination to the IPFIX flow rate meter entry.
 * Parameters:
 *     rate_id        - Rate identifier
 *     mirror_dest_id - Pre-allocated mirror destination identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_mirror_add(bcm_ipfix_rate_id_t rate_id,
                           bcm_gport_t mirror_dest_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_mirror_add(bcm_unit, rate_id, mirror_dest_id);
        BCM_IF_ERROR_RETURN(BCMX_IPFIX_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_ipfix_rate_mirror_delete
 * Purpose:
 *     Delete a mirror destination from the IPFIX flow rate meter entry.
 * Parameters:
 *     rate_id        - Rate identifier
 *     mirror_dest_id - Pre-allocated mirror destination identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_mirror_delete(bcm_ipfix_rate_id_t rate_id, 
                              bcm_gport_t mirror_dest_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_mirror_delete(bcm_unit, rate_id,
                                              mirror_dest_id);
        BCM_IF_ERROR_RETURN
            (BCMX_IPFIX_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}     

/*
 * Function:
 *     bcmx_ipfix_rate_mirror_delete_all
 * Purpose:
 *     Delete all mirror destination associated with the IPFIX flow
 *     rate meter entry.
 * Parameters:
 *     rate_id - Rate identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_mirror_delete_all(bcm_ipfix_rate_id_t rate_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_ipfix_rate_mirror_delete_all(bcm_unit, rate_id);
        BCM_IF_ERROR_RETURN
            (BCMX_IPFIX_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}     

/*
 * Function:
 *     bcmx_ipfix_rate_mirror_get
 * Purpose:
 *     Get all mirror destination associated with the IPFIX flow
 *     rate meter entry.
 * Parameters:
 *     rate_id           - Rate identifier
 *     mirror_dest_size  - Size of mirror_dest_id array
 *     mirror_dest_id    - Array to store mirror destination identifier
 *     mirror_dest_count - Number of entries stored in the mirror_dest_id array
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_rate_mirror_get(bcm_ipfix_rate_id_t rate_id,
                           int mirror_dest_size,
                           bcm_gport_t *mirror_dest_id, 
                           int *mirror_dest_count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mirror_dest_id);
    BCMX_PARAM_NULL_CHECK(mirror_dest_count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_ipfix_rate_mirror_get(bcm_unit, rate_id, mirror_dest_size,
                                       mirror_dest_id, mirror_dest_count);
        if (BCMX_IPFIX_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_ipfix_mirror_config_set
 * Purpose:
 *     Set IPFIX mirror control configuration of the specified port.
 * Parameters:
 *     stage  - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port   - Physical port
 *     config - Pointer to ipfix mirror configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_config_set(bcm_ipfix_stage_t stage,
                             bcm_gport_t port, 
                             bcmx_ipfix_mirror_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_config_set(bcm_unit, stage, port,
                                       BCMX_IPFIX_MIRROR_CONFIG_T_PTR_TO_BCM
                                       (config));
}

/*
 * Function:
 *     bcmx_ipfix_mirror_config_get
 * Purpose:
 *     Get IPFIX mirror control configuration of the specified port.
 * Parameters:
 *     stage  - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port   - Physical port
 *     config - Pointer to ipfix mirror configuration
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_config_get(bcm_ipfix_stage_t stage, 
                             bcm_gport_t port, 
                             bcmx_ipfix_mirror_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_config_get(bcm_unit, stage, port,
                                       BCMX_IPFIX_MIRROR_CONFIG_T_PTR_TO_BCM
                                       (config));
}

/*
 * Function:
 *     bcmx_ipfix_mirror_port_dest_add
 * Purpose:
 *     Add an IPFIX mirror destination to the specified port.
 * Parameters:
 *     stage          - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port           - Physical port
 *     mirror_dest_id - Pre-allocated mirror destination identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_port_dest_add(bcm_ipfix_stage_t stage, 
                                bcm_gport_t port, 
                                bcm_gport_t mirror_dest_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_port_dest_add(bcm_unit, stage, port,
                                          mirror_dest_id);
}

/*
 * Function:
 *     bcmx_ipfix_mirror_port_dest_delete
 * Purpose:
 *     Delete an IPFIX mirror destination from the specified port.
 * Parameters:
 *     stage          - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port           - Physical port
 *     mirror_dest_id - Pre-allocated mirror destination identifier
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_port_dest_delete(bcm_ipfix_stage_t stage, 
                                   bcm_gport_t port, 
                                   bcm_gport_t mirror_dest_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_port_dest_delete(bcm_unit, stage, port,
                                             mirror_dest_id);
}
     
/*
 * Function:
 *     bcmx_ipfix_mirror_port_dest_delete_all
 * Purpose:
 *     Delete all IPFIX mirror destinations from the specified port.
 * Parameters:
 *     stage - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port  - Physical port
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_port_dest_delete_all(bcm_ipfix_stage_t stage, 
                                       bcm_gport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_port_dest_delete_all(bcm_unit, stage, port);
}

/*
 * Function:
 *     bcmx_ipfix_mirror_port_dest_get
 * Purpose:
 *     Get all IPFIX mirror destinations for the specified port.
 * Parameters:
 *     stage             - BCM_IPFIX_STAGE_INGRESS or BCM_IPFIX_STAGE_EGRESS
 *     port              - Physical port
 *     mirror_dest_size  - Size of mirror_dest_id array
 *     mirror_dest_id    - Array to store mirror destination identifier
 *     mirror_dest_count - Number of entries stored in the mirror_dest_id array
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_ipfix_mirror_port_dest_get(bcm_ipfix_stage_t stage, 
                                bcm_gport_t port, 
                                int mirror_dest_size, 
                                bcm_gport_t *mirror_dest_id, 
                                int *mirror_dest_count)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_IPFIX_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_ipfix_mirror_port_dest_get(bcm_unit, stage, port,
                                          mirror_dest_size, mirror_dest_id,
                                          mirror_dest_count);
}

