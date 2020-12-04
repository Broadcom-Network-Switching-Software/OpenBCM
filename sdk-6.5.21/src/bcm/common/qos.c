/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * QoS initializers
 */

#include <sal/core/libc.h>
 
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/port.h>
#include <bcm/error.h> 
#include <bcm/qos.h>

/* 
 * Function:
 *      bcm_qos_map_t_init
 * Purpose:
 *      Initialize the QoS map struct
 * Parameters: 
 *      qos_map - Pointer to the struct to be init'ed
 */
void
bcm_qos_map_t_init(bcm_qos_map_t *qos_map)
{
    if (qos_map != NULL) {
        sal_memset(qos_map, 0, sizeof(*qos_map));
    }
    return;
}

/* 
 * Function:
 *      bcm_qos_ingress_model_t_init
 * Purpose:
 *      Initialize the QoS ingress model struct
 * Parameters: 
 *      qos_ingress_model - Pointer to the struct to be init'ed
 */
void
bcm_qos_ingress_model_t_init(bcm_qos_ingress_model_t *qos_ingress_model)
{
    if (qos_ingress_model != NULL) {
        sal_memset(qos_ingress_model, 0, sizeof(*qos_ingress_model));
    }
    return;
}

/* 
 * Function:
 *      bcm_qos_egress_model_t_init
 * Purpose:
 *      Initialize the QoS egress model struct
 * Parameters: 
 *      qos_egress_model - Pointer to the struct to be init'ed
 */
void
bcm_qos_egress_model_t_init(bcm_qos_egress_model_t *qos_egress_model)
{
    if (qos_egress_model != NULL) {
        sal_memset(qos_egress_model, 0, sizeof(*qos_egress_model));
    }
    return;
}

