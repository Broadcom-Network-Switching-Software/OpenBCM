/*! \file rx.c
 *
 * Receive APIs support when streamlined PKTIO is enabled.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_PKTIO)
#include <soc/drv.h>
#include <bcm_int/control.h>
#include <bcm_int/esw/tomahawk3.h>
#include <bcm_int/esw/rx.h>

typedef struct th3_rx_queue_s {
    int pps;    /* Packets/sec for queue; 0 -> disabled */
    int burst;  /* Burst for queue */
} th3_rx_queue_t;

typedef struct rx_info_s {
    uint8           inited; /* Module is initialized. */
    bcm_cos_queue_t queue_max; /* Maximum number of cpu queues. */
    th3_rx_queue_t  *pkt_queue; /* Packets to process/cos BCM_RX_COS */
} rx_info_t;

/*
 * Static global variable to hold RX info.
 */
static rx_info_t rx_info[BCM_MAX_NUM_UNITS] = {{0}};

/* RX info. */
#define RXINFO(unit) \
    (&rx_info[unit])

#define TH3_RXQUEUE_MAX(unit)          (RXINFO(unit)->queue_max)
#define TH3_RXQUEUE(unit, cos)         (RXINFO(unit)->pkt_queue[cos])
#define TH3_RXCOS_PPS(unit, cos)       (TH3_RXQUEUE(unit, cos).pps)
#define TH3_RXCOS_BURST(unit, cos)     (TH3_RXQUEUE(unit, cos).burst)

#define TH3_RX_IS_LOCAL(unit)   \
    (BCM_IS_LOCAL(unit) && !SOC_IS_RCPU_UNIT(unit) && SOC_UNIT_VALID(unit))

/* Check if RX module is initialized. */
#define TH3_RXINIT_CHECK(unit) \
    do { \
        rx_info_t *rx = RXINFO(unit); \
        if (rx->inited == FALSE) { \
            return BCM_E_INIT; \
        } \
    } while(0)

int
bcm_tomahawk3x_rx_cos_rate_set(int unit, int cos, int pps)
{
    int i;

    TH3_RXINIT_CHECK(unit);
    if (cos < 0 && cos != BCM_RX_COS_ALL) {
        return BCM_E_PARAM;
    }
    if (cos >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    if (cos == BCM_RX_COS_ALL) {
        for (i = 0; i <= TH3_RXQUEUE_MAX(unit); i++) {
            TH3_RXCOS_PPS(unit, i) = pps;
        }
    } else {
        TH3_RXCOS_PPS(unit, cos) = pps;
    }

    if (TH3_RX_IS_LOCAL(unit)) {
        if (cos == BCM_RX_COS_ALL) {
            for (i = 0; i <= TH3_RXQUEUE_MAX(unit); i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_port_pps_set(unit, CMIC_PORT(unit),
                                               i, pps));
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_port_pps_set(unit, CMIC_PORT(unit), cos,
                                           pps));
        }
    }
    return BCM_E_NONE;
}

int
bcm_tomahawk3x_rx_cos_rate_get(int unit, int cos, int *pps)
{
    TH3_RXINIT_CHECK(unit);
    if ( (cos < 0) || (cos > TH3_RXQUEUE_MAX(unit))) {
        return BCM_E_PARAM;
    }

    if (pps) {
        *pps = TH3_RXCOS_PPS(unit, cos);
    }

    return BCM_E_NONE;
}

int
bcm_tomahawk3x_rx_cos_burst_get(int unit, int cos, int *burst)
{
    TH3_RXINIT_CHECK(unit);
    if ( (cos < 0) || (cos > TH3_RXQUEUE_MAX(unit))) {
        return BCM_E_PARAM;
    }

    if (burst) {
        *burst = TH3_RXCOS_BURST(unit, cos);
    }
    return BCM_E_NONE;
}

int
bcm_tomahawk3x_rx_cos_burst_set(int unit, int cos, int burst)
{
    int i;

    TH3_RXINIT_CHECK(unit);
    if (cos < 0 && cos != BCM_RX_COS_ALL) {
        return BCM_E_PARAM;
    }
    if (cos >= NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }
    if (cos == BCM_RX_COS_ALL) {
        for (i = 0; i <= TH3_RXQUEUE_MAX(unit); i++) {
            TH3_RXCOS_BURST(unit, i) = burst;
        }
    } else {
        TH3_RXCOS_BURST(unit, cos) = burst;
    }
    if (TH3_RX_IS_LOCAL(unit)) {
        if (cos == BCM_RX_COS_ALL) {
            for (i = 0; i <= TH3_RXQUEUE_MAX(unit); i++) {
                BCM_IF_ERROR_RETURN
                    (bcm_th3_cosq_port_burst_set(unit, CMIC_PORT(unit),
                                                 i, burst));
            }
        } else {
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_port_burst_set(unit, CMIC_PORT(unit), cos,
                                             burst));
        }
    }
    return BCM_E_NONE;
}

int
bcm_tomahawk3x_rx_deinit(int unit)
{
    if (RXINFO(unit)->inited) {
        if (RXINFO(unit)->pkt_queue) {
            sal_free(RXINFO(unit)->pkt_queue);
            RXINFO(unit)->pkt_queue = NULL;
        }
        RXINFO(unit)->inited = FALSE;
    }
    return BCM_E_NONE;
}

int
bcm_tomahawk3x_rx_init(int unit)
{
    int queue_size, cosq;

    if (!RXINFO(unit)->inited) {
        BCM_IF_ERROR_RETURN
            (_bcm_common_rx_queue_max_get(unit, &RXINFO(unit)->queue_max));

        queue_size = sizeof(th3_rx_queue_t) * BCM_RX_COS;
        RXINFO(unit)->pkt_queue = sal_alloc(queue_size, "pkt_queue");
        if (RXINFO(unit)->pkt_queue == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(RXINFO(unit)->pkt_queue, 0, queue_size);
        RXINFO(unit)->inited = TRUE;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        if (BCM_CONTROL(unit)->attach_state != _bcmControlStateTxRxInit) {
            for (cosq = 0; cosq <= TH3_RXQUEUE_MAX(unit); cosq++) {
                 BCM_IF_ERROR_RETURN
                     (bcm_th3_cosq_port_pps_get(unit, CMIC_PORT(unit),
                                                cosq,
                                                &TH3_RXCOS_PPS(unit, cosq)));
                 BCM_IF_ERROR_RETURN
                     (bcm_th3_cosq_port_burst_get(unit, CMIC_PORT(unit),
                                                  cosq,
                                                  &TH3_RXCOS_BURST(unit, cosq)));
            }
        }
    } else
#endif /* BCM_WARM_BOOT_SUPPORT */
    {
        BCM_IF_ERROR_RETURN(_bcm_trx_rx_cosq_mapping_init(unit));
    }
    return BCM_E_NONE;
}
#else
typedef int bcm_tomahawk3_rx_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PKTIO */
