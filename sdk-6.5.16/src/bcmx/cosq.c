/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    bcmx/cosq.c
 * Purpose: BCMX Class of Service Queue APIs
 */

#include <bcm/types.h>

#include <bcmx/cosq.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_COSQ_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_COSQ_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_COSQ_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_COSQ_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_COSQ_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM(_discard)    \
    ((bcm_cosq_gport_discard_t *)(_discard))

/*
 * Function:
 *      bcmx_cosq_init
 */

int
bcmx_cosq_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_init(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_detach
 */

int
bcmx_cosq_detach(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_detach(bcm_unit);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_config_set
 */

int
bcmx_cosq_config_set(int numq)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_config_set(bcm_unit, numq);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_config_get
 */

int
bcmx_cosq_config_get(int *numq)
{
    int rv, i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(numq);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_config_get(bcm_unit, numq);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_cosq_mapping_set
 */

int
bcmx_cosq_mapping_set(bcm_cos_t priority,
                      bcm_cos_queue_t cosq)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_mapping_set(bcm_unit, priority, cosq);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_mapping_get
 */

int
bcmx_cosq_mapping_get(bcm_cos_t priority,
                      bcm_cos_queue_t *cosq)
{
    int rv, i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(cosq);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_mapping_get(bcm_unit, priority, cosq);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_cosq_port_mapping_set
 */

int
bcmx_cosq_port_mapping_set(bcmx_lport_t port,
                           bcm_cos_t priority,
                           bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_port_mapping_set(bcm_unit, bcm_port, priority, cosq);
}


/*
 * Function:
 *      bcmx_cosq_port_mapping_get
 */

int
bcmx_cosq_port_mapping_get(bcmx_lport_t port,
                           bcm_cos_t priority,
                           bcm_cos_queue_t *cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(cosq);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_port_mapping_get(bcm_unit, bcm_port, priority, cosq);
}

/*
 * Function:
 *      bcmx_cosq_sched_set
 */

int
bcmx_cosq_sched_set(int mode,
                    const int weights[BCM_COS_COUNT],
                    int delay)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_sched_set(bcm_unit, mode, weights, delay);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_port_sched_set
 */

int
bcmx_cosq_port_sched_set(bcmx_lplist_t lplist,
                         int mode,
                         const int weights[BCM_COS_COUNT],
                         int delay)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;
    bcm_pbmp_t pbmp;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_cosq_port_sched_set(bcm_unit, pbmp, mode,
                                             weights, delay);
            BCM_IF_ERROR_RETURN
                (BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_cosq_sched_get
 */

int
bcmx_cosq_sched_get(int *mode,
                    int weights[BCM_COS_COUNT],
                    int *delay)
{
    int rv, i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_sched_get(bcm_unit, mode, weights, delay);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcmx_cosq_port_sched_get
 */

int
bcmx_cosq_port_sched_get(bcmx_lplist_t lplist,
                         int *mode,
                         int weights[BCM_COS_COUNT],
                         int *delay)
{
    int rv, i, bcm_unit;
    bcm_pbmp_t pbmp;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            rv = bcm_cosq_port_sched_get(bcm_unit, pbmp, mode,
                                         weights, delay);
            if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
                return rv;
            }
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_sched_weight_max_get(int mode,
                               int *weight_max)
{
    int rv, i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(weight_max);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_sched_weight_max_get(bcm_unit, mode, weight_max);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_port_bandwidth_set(bcmx_lport_t port, bcm_cos_queue_t cosq,
                             uint32 kbits_sec_min, uint32 kbits_sec_max,
                             uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_port_bandwidth_set(bcm_unit, bcm_port, cosq,
                                       kbits_sec_min, kbits_sec_max, flags);
}

int
bcmx_cosq_port_bandwidth_get(bcmx_lport_t port, bcm_cos_queue_t cosq,
                             uint32 *kbits_sec_min, uint32 *kbits_sec_max,
                             uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_sec_min);
    BCMX_PARAM_NULL_CHECK(kbits_sec_max);
    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_port_bandwidth_get(bcm_unit, bcm_port, cosq,
                                       kbits_sec_min, kbits_sec_max, flags);
}

/*
 * Function:
 *      bcmx_cosq_discard_set
 */

int
bcmx_cosq_discard_set(uint32 flags)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_discard_set(bcm_unit, flags);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_cosq_discard_get
 */

int
bcmx_cosq_discard_get(uint32 *flags)
{
    int rv, i, bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_discard_get(bcm_unit, flags);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_cosq_discard_port_set(bcmx_lport_t port,
                           bcm_cos_queue_t cosq,
                           uint32 color,
                           int drop_start,
                           int drop_slope,
                           int average_time)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_discard_port_set(bcm_unit, bcm_port, cosq, color,
                                     drop_start, drop_slope, average_time);
}

int
bcmx_cosq_discard_port_get(bcmx_lport_t port,
                           bcm_cos_queue_t cosq,
                           uint32 color,
                           int *drop_start,
                           int *drop_slope,
                           int *average_time)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(drop_start);
    BCMX_PARAM_NULL_CHECK(drop_slope);
    BCMX_PARAM_NULL_CHECK(average_time);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_discard_port_get(bcm_unit, bcm_port, cosq, color,
                                     drop_start, drop_slope, average_time);
}


/*
 * Function:
 *     bcmx_cosq_control_set
 */
int
bcmx_cosq_control_set(bcm_gport_t port, bcm_cos_queue_t cosq, 
                      bcm_cosq_control_t type, int arg)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /* A negative port value indicates all ports */
    if (!BCMX_IS_LPORT(port) && (port >= 0)) {
        return BCM_E_PORT;
    }

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT))) {
            return BCM_E_PORT;
        }
        return bcm_cosq_control_set(bcm_unit, port, cosq, type, arg);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_control_set(bcm_unit, port, cosq, type, arg);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_control_get
 */
int
bcmx_cosq_control_get(bcm_gport_t port, bcm_cos_queue_t cosq, 
                      bcm_cosq_control_t type, int *arg)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    /* A negative port value indicates any port */
    if (!BCMX_IS_LPORT(port) && (port >= 0)) {
        return BCM_E_PORT;
    }

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                                BCMX_DEST_CONVERT_DEFAULT))) {
            return BCM_E_PORT;
        }
        return bcm_cosq_control_get(bcm_unit, port, cosq, type, arg);
    }

    /* Virtual port */
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_control_get(bcm_unit, port, cosq, type, arg);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_cosq_gport_add
 */
int
bcmx_cosq_gport_add(bcm_gport_t port, int numq, uint32 flags,
                    bcm_gport_t *gport)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(gport);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_add(bcm_unit, port, numq, flags, gport);
}

/*
 * Function:
 *     bcmx_cosq_gport_delete
 */
int
bcmx_cosq_gport_delete(bcm_gport_t gport)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_delete(bcm_unit, gport);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_delete(bcm_unit, gport);
        BCM_IF_ERROR_RETURN
            (BCMX_COSQ_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_bandwidth_set
 */
int
bcmx_cosq_gport_bandwidth_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                              uint32 kbits_sec_min, uint32 kbits_sec_max, 
                              uint32 flags)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_bandwidth_set(bcm_unit, gport, cosq, 
                                            kbits_sec_min, kbits_sec_max, 
                                            flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_bandwidth_set(bcm_unit, gport, cosq, 
                                              kbits_sec_min, kbits_sec_max, 
                                              flags);
        BCM_IF_ERROR_RETURN
            (BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_bandwidth_get
 */
int
bcmx_cosq_gport_bandwidth_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                              uint32 *kbits_sec_min, uint32 *kbits_sec_max, 
                              uint32 *flags)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_sec_min);
    BCMX_PARAM_NULL_CHECK(kbits_sec_max);
    BCMX_PARAM_NULL_CHECK(flags);

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_bandwidth_get(bcm_unit, gport, cosq, 
                                            kbits_sec_min, kbits_sec_max, 
                                            flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_bandwidth_get(bcm_unit, gport, cosq, 
                                          kbits_sec_min, kbits_sec_max, 
                                          flags);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_cosq_gport_sched_set
 */
int
bcmx_cosq_gport_sched_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                          int mode, int weight)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_sched_set(bcm_unit, gport, cosq, 
                                        mode, weight);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_sched_set(bcm_unit, gport, cosq, 
                                          mode, weight);
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_sched_get
 */
int
bcmx_cosq_gport_sched_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                          int *mode, int *weight)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);
    BCMX_PARAM_NULL_CHECK(weight);

    /*
     * 'gport' can be a physical local port or a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_sched_get(bcm_unit, gport, cosq, 
                                        mode, weight);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_sched_get(bcm_unit, gport, cosq, 
                                      mode, weight);
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_cosq_gport_attach
 */
int
bcmx_cosq_gport_attach(bcm_gport_t sched_port, bcm_gport_t input_port, 
                       bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'sched_port' is a system-wide value (scheduler)
     * 'input_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(input_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_attach(bcm_unit, sched_port, input_port, cosq);
}

/*
 * Function:
 *     bcmx_cosq_gport_detach
 */
int
bcmx_cosq_gport_detach(bcm_gport_t sched_port, bcm_gport_t input_port, 
                       bcm_cos_queue_t cosq)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'sched_port' is a system-wide value (scheduler)
     * 'input_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(input_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_gport_detach(bcm_unit, sched_port, input_port, cosq);
}

/*
 * Function:
 *     bcmx_cosq_gport_attach_get
 */
int
bcmx_cosq_gport_attach_get(bcm_gport_t sched_port, bcm_gport_t *input_port, 
                           bcm_cos_queue_t *cosq)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(input_port);
    BCMX_PARAM_NULL_CHECK(cosq);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_attach_get(bcm_unit, sched_port, input_port, 
                                       cosq);
        /*
         * Find the device where the input_port was set, so
         * ignore BCM_E_NOT_FOUND errors (but save it)
         */
        if (tmp_rv == BCM_E_NOT_FOUND) {
            rv = tmp_rv;
            continue;
        }

        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, tmp_rv)) {
            return tmp_rv;
        }
    }

    return rv;
}


/*
 * Function:
 *     bcmx_cosq_gport_discard_t_init
 */
void
bcmx_cosq_gport_discard_t_init(bcmx_cosq_gport_discard_t *discard)
{
    if (discard != NULL) {
        bcm_cosq_gport_discard_t_init
            (BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM(discard));
    }
}

/*
 * Function:
 *     bcmx_cosq_gport_discard_set
 */
int
bcmx_cosq_gport_discard_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcmx_cosq_gport_discard_t *discard)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(discard);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_discard_set(bcm_unit, gport, cosq,
                                          BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                          (discard));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_cosq_gport_discard_set(bcm_unit, gport, cosq,
                                            BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                            (discard));
        BCM_IF_ERROR_RETURN(BCMX_COSQ_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_cosq_gport_discard_get
 */
int
bcmx_cosq_gport_discard_get(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                            bcmx_cosq_gport_discard_t *discard)
{
    int         rv;
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(discard);

    /*
     * If gport is a valid BCMX logical port, then gport refers to
     * a specific physical port (mod/port or dev/port).
     * Else, gport is a system-wide value.
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_cosq_gport_discard_get(bcm_unit, gport, cosq,
                                          BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                          (discard));
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_cosq_gport_discard_get(bcm_unit, gport, cosq,
                                        BCMX_COSQ_GPORT_DISCARD_T_PTR_TO_BCM
                                        (discard));
        if (BCMX_COSQ_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *     bcmx_cosq_gport_destmod_attach/detach
 * Purpose:
 *     Attach/detach gport mapping from ingress port, dest_modid to
 *     fabric egress port.
 * Parameters:
 *     gport              - GPORT ID
 *     ingress_port       - Ingress port
 *     dest_modid         - Destination module ID
 *     fabric_egress_port - Port number on fabric that 
 *                           is connected to dest_modid
 * Returns:
 *     BCM_E_xxx
 */
int
bcmx_cosq_gport_destmod_attach(bcm_gport_t gport, bcm_gport_t ingress_port, 
                               bcm_module_t dest_modid,int fabric_egress_port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'gport' is a system-wide value
     * 'ingress_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(ingress_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_gport_destmod_attach(bcm_unit, gport, bcm_port,
                                         dest_modid, fabric_egress_port);
}

int
bcmx_cosq_gport_destmod_detach(bcm_gport_t gport, bcm_gport_t ingress_port, 
                               bcm_module_t dest_modid, int fabric_egress_port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    /*
     * 'gport' is a system-wide value
     * 'ingress_port' is a physical local port
     */
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(ingress_port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_NON_GPORT));

    return bcm_cosq_gport_destmod_detach(bcm_unit, gport, bcm_port,
                                         dest_modid, fabric_egress_port);
}

/*
 * Function:
 *     bcmx_cosq_stat_get/get32
 * Purpose:
 *     Get MMU drop statistics on a per port per cosq basis.
 * Parameters:
 *     gport - Logical port
 *     cosq  - COS queue
 *     stat  - Statistic to be retrieved
 *     value - (OUT) Returned statistic value
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_cosq_stat_get(bcm_gport_t gport, bcm_cos_queue_t cosq,
                   bcm_cosq_stat_t stat, uint64 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_stat_get(bcm_unit, gport, cosq, stat, value);
}

int
bcmx_cosq_stat_get32(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                     bcm_cosq_stat_t stat, uint32 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_stat_get32(bcm_unit, gport, cosq, stat, value);
}

/*
 * Function:
 *     bcmx_cosq_stat_set/set32
 * Purpose:
 *     Set MMU drop statistics on a per port per cosq basis.
 * Parameters:
 *     gport - Logical port
 *     cosq  - COS queue
 *     stat  - Statistic to be set
 *     value - Statistic value to be set
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int
bcmx_cosq_stat_set(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                   bcm_cosq_stat_t stat, uint64 value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_stat_set(bcm_unit, gport, cosq, stat, value);
}

int
bcmx_cosq_stat_set32(bcm_gport_t gport, bcm_cos_queue_t cosq, 
                     bcm_cosq_stat_t stat, uint32 value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_COSQ_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_cosq_stat_set32(bcm_unit, gport, cosq, stat, value);
}
