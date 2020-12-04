/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        chipsim.c
 *
 *     The main pcid file; simulates BCM 5670 and 5690.
 *
 * Requires:
 *     util.c routines
 *     soc_internal_read/write
 *     
 * Provides:
 *     event_init
 *     event_enqueue
 *     event_peek
 *     event_process_through
 *     pcid_counter_activity
 *     pcid_check_packet_input
 */

#include <shared/bsl.h>

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/higig.h>

#include <soc/cmic.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <bde/pli/verinet.h>

#include "pcid.h"
#include "mem.h"
#include "cmicsim.h"
#include "dma.h"
#include "chipsim.h"

sal_usecs_t     event_past;
event_t        *event_q;

void
event_init(void)
{
    event_t        *ev;

    /* Clear queue in case called more than once */

    while ((ev = event_q) != NULL) {
        event_q = ev->next;
        sal_free(ev);
    }

    event_past = sal_time_usecs();
}

void
event_enqueue(pcid_info_t *pcid_info, event_handler_t handler,
              sal_usecs_t abs_time)
{
    event_t        *ev, **pp;

    if ((ev = sal_alloc(sizeof(event_t), "event_t")) == NULL) {
        LOG_CLI((BSL_META("pcid: Out of memory\n")));
        abort();
    }

    /* Disallow enqueuing events in the past */

    ev->pcid_info = pcid_info;
    ev->abs_time = (SAL_USECS_SUB(abs_time, event_past) > 0) ?
        abs_time : event_past;
    ev->handler = handler;

    for (pp = &event_q; *pp != NULL; pp = &(*pp)->next) {
        if (SAL_USECS_SUB((*pp)->abs_time, abs_time) > 0) {
            break;
        }
    }

    ev->next = *pp;

    *pp = ev;
}

event_t        *
event_peek(void)
{
    return event_q;
}

void
event_process_through(sal_usecs_t abs_time)
{
    event_t        *ev;

    while ((ev = event_q) != NULL &&
           SAL_USECS_SUB(abs_time, ev->abs_time) >= 0) {

        event_q = ev->next;
        ev->next = NULL;

        (*ev->handler) (ev);

        sal_free(ev);
    }

    event_past = abs_time;
}

/* 
 * pcid_counter_activity
 *
 *    Simulate random counter activity.
 *
 *    The lower-numbered ports are most active and the higher-numbered
 *    ports are the least active.  There is an exponential drop-off of
 *    activity based on the port number.
 */

void
pcid_counter_activity(event_t * ev)
{
    soc_port_t      port;
    soc_reg_t       ctr_reg;
    int             i, incr, offset;
    uint32          mask;
    uint32          val[SOC_MAX_MEM_WORDS];
    pcid_info_t    *pcid_info;
    soc_cmap_t     *cmap;
    int             ctype;

    pcid_info = ev->pcid_info;

    PBMP_ITER(PBMP_PORT_ALL(pcid_info->unit), port) {
        if (IS_FE_PORT(pcid_info->unit, port)) {
            ctype = SOC_CTR_TYPE_FE;
        } else if (IS_GE_PORT(pcid_info->unit, port)) {
            ctype = SOC_CTR_TYPE_GE;
        } else if (IS_HG_PORT(pcid_info->unit, port)) {
            ctype = SOC_CTR_TYPE_HG;
        } else if (IS_XE_PORT(pcid_info->unit, port)) {
            ctype = SOC_CTR_TYPE_XE;
        } else {
            ctype = 0;
        }

        cmap = &SOC_CTR_DMA_MAP(pcid_info->unit, ctype);

        for (i = 0; i < cmap->cmap_size; i++) {
            incr = 0x100000;       /* Lower-numbered counters are active */

            ctr_reg = cmap->cmap_base[i].reg;

            if (ctr_reg == INVALIDr) {
                continue;
	    }

            offset = soc_reg_addr(pcid_info->unit, ctr_reg, port, 0);

            mask = soc_reg_datamask(pcid_info->unit, ctr_reg, 0);

            soc_internal_read_reg(pcid_info, offset, val);
            val[0] = (val[0] + (sal_rand() % incr)) & mask;
            soc_internal_write_reg(pcid_info, offset, val);
        }
    }

    event_enqueue(pcid_info, ev->handler,
                  ev->abs_time + COUNTER_ACT_INTERVAL);
}

void
pcid_check_packet_input(event_t * ev)
{
    FILE           *fp;
    int             c1, c2, byte;
    packet_t       *pkt;
    packet_t       *pp;
    pcid_info_t    *pcid_info;
    int            i;
    int            word_count=8; /* FB/ER 3 + 5 words of HG + PBI */

    pcid_info = ev->pcid_info;

    if ((fp = fopen("pkt", "r")) != 0) {
        pkt = sal_alloc(sizeof(packet_t), "packet_t");

        pkt->length = 0;
        pkt->consum = 0;
        pkt->next = 0;

#ifdef BCM_TRX_SUPPORT
        if (SOC_IS_TRX(pcid_info->unit)) {
            word_count = 13; /* 4 + 7 words of HG + PBE */
        }
#endif /* BCM_TRX_SUPPORT */

        for (i = 0; i < word_count; i++) {
            pkt->dcbd[i] = 0x0; /* FB/ER 3 + 5 words of HG + PBI */
        }
#ifdef BCM_XGS_SUPPORT
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_start, SOC_HIGIG_START);
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_hgi, SOC_HIGIG_HGI);
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_vlan_id, 1);
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_opcode, SOC_HIGIG_OP_CPU);
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_hdr_format, SOC_HIGIG_HDR_DEFAULT);
        soc_higig_field_set(pcid_info->unit, (soc_higig_hdr_t *)(pkt->dcbd),
                            HG_src_port, CMIC_PORT(pcid_info->unit));
#endif /* BCM_XGS_SUPPORT */

        for (;;) {
            do {
                if ((c1 = getc(fp)) == EOF)
                    goto eof;
            } while (!isxdigit(c1));

            do {
                if ((c2 = getc(fp)) == EOF)
                    goto eof;
            } while (!isxdigit(c2));

            byte = pcid_x2i(c1) * 16 + pcid_x2i(c2);

            /* Add byte to current packet (up to max length) */

            if (pkt->length < PKT_SIZE_MAX) {
                pkt->data[pkt->length++] = byte;
            }
        }

      eof:

        fclose(fp);

        unlink("pkt");

#if 0
        LOG_CLI((BSL_META("RX PKT LEN %d:"), pkt->length));
        for (c1 = 0; c1 < pkt->length; c1++) {
            LOG_CLI((BSL_META(" %02x"), pkt->data[c1]));
        }
        LOG_CLI((BSL_META("\n")));
#endif

        /* Queue current packet on end of list */
        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        if (!pcid_info->pkt_list) {
            pcid_info->pkt_list = pkt;
        } else {
            for (pp = pcid_info->pkt_list; pp->next; pp = pp->next)
                ;
            pp->next = pkt;
        }
        sal_mutex_give(pcid_info->pkt_mutex);
    }

    event_enqueue(pcid_info, ev->handler,
                  ev->abs_time + PACKET_CHECK_INTERVAL);
}
