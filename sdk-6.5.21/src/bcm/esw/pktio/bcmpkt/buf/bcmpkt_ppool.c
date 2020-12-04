/*! \file bcmpkt_ppool.c
 *
 * BCMPKT packet pool library is to provide simple packet pool solution
 * for user to get efficiency.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_alloc.h>
#include <sal/sal_spinlock.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#endif
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmdrd/bcmdrd_dev.h>
#include "bcmpkt_buf_internal.h"

#define BSL_LOG_MODULE              BSL_LS_BCMPKT_PACKET

/*! Packet buffer pool structure. */
typedef struct bcmpkt_ppool_s {
    /* Active means may allocate buffer from it. */
    bool active;
    /* Buffer number. */
    int pkt_count;
    /* Increase for each free from pkt_count. */
    int free_count;
    /* Point to the first buffer in free list. */
    bcmpkt_packet_t *free_list;
    /* Buffer pool protection lock. */
    sal_spinlock_t lock;
} bcmpkt_ppool_t;

/*! Packet pool. */
static bcmpkt_ppool_t pp;

int
bcmpkt_ppool_create(int pkt_count)
{
    bcmpkt_packet_t *packet;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    if (pkt_count < 0) {
        SHR_RETURN_VAL_EXIT(SHR_E_PARAM);
    }

    if (pp.pkt_count) {
        SHR_RETURN_VAL_EXIT(SHR_E_EXISTS);
    }

    while (pkt_count-- > 0) {
        packet = sal_alloc(sizeof(bcmpkt_packet_t), "bcmpktPpoolCreate");
        if (!packet) {
            SHR_RETURN_VAL_EXIT(SHR_E_MEMORY);
        }

        packet->next = pp.free_list;
        pp.free_list = packet;
        pp.free_count++;
        pp.pkt_count++;
    }

    pp.active = true;
    pp.lock = sal_spinlock_create("bcmpktPacketPoolCreate");
    SHR_NULL_CHECK(pp.lock, SHR_E_MEMORY);

exit:
    if (SHR_FUNC_VAL_IS(SHR_E_MEMORY)) {
        while (pp.free_list) {
            packet = pp.free_list;
            pp.free_list = pp.free_list->next;
            sal_free(packet);
        }
        sal_memset(&pp, 0, sizeof(pp));
    }

    SHR_FUNC_EXIT();
}

int
bcmpkt_ppool_destroy(void)
{
    bcmpkt_packet_t *packet;

    if (!pp.active) {
        return SHR_E_NONE;
    }

    sal_spinlock_lock(pp.lock) ;
    pp.active = false;
    sal_spinlock_unlock(pp.lock);

    while (pp.free_list) {
        packet = pp.free_list;
        pp.free_list = pp.free_list->next;
        sal_free(packet);
    }
    if (pp.lock) {
        sal_spinlock_destroy(pp.lock);
        pp.lock= NULL;
    }
    sal_memset(&pp, 0, sizeof(pp));

    return SHR_E_NONE;
}

bcmpkt_packet_t *
bcmpkt_ppool_alloc(void)
{
    bcmpkt_packet_t *packet;

    if (!pp.lock) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Packet pool was not created\n")));
        return sal_alloc(sizeof(bcmpkt_packet_t), "bcmpktPpoolAlloc");
    }
    if (sal_spinlock_lock(pp.lock) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Lock failure\n")));
        sal_spinlock_unlock(pp.lock);
        return sal_alloc(sizeof(bcmpkt_packet_t), "bcmpktPpoolAlloc");
    }
    if (!pp.active || pp.free_count < 1) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Packet pool was not available\n")));
        sal_spinlock_unlock(pp.lock);
        return sal_alloc(sizeof(bcmpkt_packet_t), "bcmpktPpoolAlloc");
    }

    packet = pp.free_list;
    pp.free_list = pp.free_list->next;
    pp.free_count--;

    sal_spinlock_unlock(pp.lock);

    return packet;
}

int
bcmpkt_ppool_free(bcmpkt_packet_t *packet)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(packet, SHR_E_PARAM);

    if (!pp.lock) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Packet pool uninitialized\n")));
        sal_free(packet);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (sal_spinlock_lock(pp.lock) != 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Lock failure\n")));
        sal_free(packet);
        sal_spinlock_unlock(pp.lock);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    if (!pp.active) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META("Not active\n")));
        sal_free(packet);
        sal_spinlock_unlock(pp.lock);
        SHR_RETURN_VAL_EXIT(SHR_E_NONE);
    }

    /* Save up to pp.pkt_count free buffers. */
    if (pp.free_count < pp.pkt_count) {
        packet->next = pp.free_list;
        pp.free_list = packet;
        pp.free_count++;
    } else {
        sal_free(packet);
    }

    sal_spinlock_unlock(pp.lock);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_ppool_status_get(bcmpkt_ppool_status_t *status)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(status, SHR_E_PARAM);

    status->active = pp.active;
    status->pkt_count = pp.pkt_count;
    status->free_count = pp.free_count;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_ppool_info_dump(shr_pb_t *pb)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_NULL_CHECK(pb, SHR_E_PARAM);

    shr_pb_printf(pb, "Packet buffer information:\n");
    shr_pb_printf(pb, "Active: %s\n", pp.active ? "true" : "false");
    shr_pb_printf(pb, "Packet count: %d\n", pp.pkt_count);
    shr_pb_printf(pb, "Free count: %d\n", pp.free_count);

exit:
    SHR_FUNC_EXIT();
}
