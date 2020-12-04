/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:       bregex.c
 * Purpose:    Regex Eevent notification API
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/drv.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/cosq.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/scorpion.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/hurricane.h>
#include <bcm_int/esw/fb4regex.h>

#include <bcm_int/esw_dispatch.h>

#if defined(INCLUDE_REGEX)

#if defined(BCM_TRIUMPH3_SUPPORT)

#define _BCM_FT_REPORT_MAX_CB   4

typedef enum _bcm_ft_report_cb_state_e {
    _BCM_FT_REPORT_CALLBACK_STATE_INVALID = 0,
    _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE,
    _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED,
    _BCM_FT_REPORT_CALLBACK_STATE_UNREGISTERED
} _bcm_ft_report_cb_state_t;

typedef struct _bcm_ft_report_cb_entry_s {
    _bcm_ft_report_cb_state_t state;
    uint32  reports;
    bcm_regex_report_cb callback;
    void *userdata;
} _bcm_ft_report_cb_entry_t;

typedef struct _bcm_ft_report_ctrl_s {
    _bcm_ft_report_cb_entry_t callback_entry[_BCM_FT_REPORT_MAX_CB];
    sal_thread_t pid;          /* export fifo processing thread process id */
    VOL sal_usecs_t interval;  /* export fifo processing polling interval */
} _bcm_ft_report_ctrl_t;

static _bcm_ft_report_ctrl_t *_bcm_ft_report_ctrl[BCM_MAX_NUM_UNITS];

extern int
_bcm_tr3_get_match_id(int unit, int signature_id, int *match_id);

int
_bcm_ft_key_field_get(uint32 *fld, int bp, int len, uint32 *kfld)
{
    int ii, bval;

    for (ii = 0; ii < (31 + len)/32; ii++) {
        kfld[ii] = 0;
    }

    for (ii = 0; ii < len; ii++) {
        bval = fld[(bp+ii)/32] & (1 << ((bp + ii) % 32));
        if (bval) {
            kfld[ii/32] |= 1 << (ii % 32);
        }
    }
    return 0;
}

int
_bcm_ft_key_field_set(uint32 *fld, int bp, int len, uint32 *kfld)
{
    int ii, bval;

    for (ii = 0; ii < len; ii++) {
        bval = kfld[ii/32] & (1 << (ii % 32));
        if (bval) {
            fld[(bp+ii)/32] |= 1 << ((bp + ii) % 32);
        }
    }
    return 0;
}

int
_bcm_ft_key_field_get_char(uint32 *fld, int bp, int len, uint8 *kfld)
{
    int ii, bval;

    for (ii = 0; ii < (7 + len)/8; ii++) {
        kfld[ii] = 0;
    }

    for (ii = 0; ii < len; ii++) {
        bval = fld[(bp+ii)/32] & (1 << ((bp + ii) % 32));
        if (bval) {
            kfld[ii/8] |= 1 << (ii % 8);
        }
    }
    return 0;
}

int
_bcm_ft_key_field_set_char(uint32 *fld, int bp, int len, uint8 *kfld)
{
    int ii, bval;

    for (ii = 0; ii < len; ii++) {
        bval = kfld[ii/8] & (1 << (ii % 8));
        if (bval) {
            fld[(bp+ii)/32] |= 1 << ((bp + ii) % 32);
        }
    }
    return 0;
}

int
_bcm_ft_ctr_enabled(int unit)
{
    uint32 regval, fval;

    SOC_IF_ERROR_RETURN(READ_FT_CONFIGr(unit, &regval));
    fval = soc_reg_field_get(unit, FT_CONFIGr, regval, CNT_MODEf);

    if ((fval == 1) || (fval == 2)) {
        return fval;
    }
    return 0;
}

STATIC int
_bcm_ft_report_process_export_entry(int unit,
                         void *entry, bcm_regex_report_t *data,
                         int *pending, uint32 *pdir)
{
    soc_mem_t mem;
    uint32 val, pk[4], pkl[6];
    bcm_ip6_t sip6, dip6;
    uint8  *p8;
    uint32 type, ip1, ip2, p1, p2, pt;
    int rv = 0, i;
    uint32 dir;
    uint64 tmp_packet_count;          /* Packet count. */
    uint64 tmp_byte_count;            /* Packet count. */

    mem = FT_EXPORT_FIFOm;

    type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
    *pending = 0;

    if ((type == _REGEX_SESSION_KEY_TYPE_IPV4) ||
        (type == _REGEX_SESSION_KEY_TYPE_IPV6_UPPER)) {
        val = soc_mem_field32_get(unit, mem, entry, SIGNATURE_IDf);
        rv = _bcm_tr3_get_match_id(unit, val, &data->match_id);
        if (rv) {
            return BCM_E_NOT_FOUND;
        }

        dir = soc_mem_field32_get(unit, mem, entry, KEY_DIRECTIONf);
        *pdir = dir;
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " KEY_DIRECTION = 0x%x\n"),
                  dir));
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " KEY_TYPE = 0x%x\n"),
                  type));
        val = soc_mem_field32_get(unit, mem, entry, EVENT_IDf);
        data->flags = 0;
        if (val & BCM_REGEX_REPORT_NEW) {
            data->flags |= BCM_REGEX_REPORT_NEW;
        }
        if ((val & BCM_REGEX_REPORT_MATCHED) &&
            (soc_mem_field32_get(unit, mem, entry, SME_MATCHf))) {
            data->flags |= BCM_REGEX_REPORT_MATCHED;
        }
        if (val & BCM_REGEX_REPORT_END) {
            data->flags |= BCM_REGEX_REPORT_END;
            data->flow_end_reason =  soc_mem_field32_get(unit, mem, entry, TCP_FIN_Rf);
            if(!(data->flow_end_reason)) {
                data->flow_end_reason = soc_mem_field32_get(unit, mem, entry, TCP_FIN_Ff);
            }
            if(!(data->flow_end_reason)) {
                data->flow_end_reason = soc_mem_field32_get(unit, mem, entry, TCP_RESETf);
            }
        }
        val = soc_mem_field32_get(unit, mem, entry, FLOW_STATEf);
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " FLOW_STATE = 0x%03x\n"),
                  val));

        soc_mem_field_get(unit, mem, entry, KEYf, pk);
        p8 = (uint8*) pk;
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "pk = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n"),
                  *p8, *(p8+1), *(p8+2), *(p8+3), *(p8+4), *(p8+5), *(p8+6), *(p8+7), *(p8+8), *(p8+9), *(p8+10), *(p8+11), *(p8+12)));

        if (type == _REGEX_SESSION_KEY_TYPE_IPV4) {
            for (i = 0; i < 16; i++) {
                data->sip6[i] = 0;
                data->dip6[i] = 0;
            }

            _bcm_ft_key_field_get(pk, _REGEX_KEY_FIELD_V4_OFFSET_PORT_1,
                                  _REGEX_KEY_FIELD_V4_LENGTH_PORT_1, &p1);
            _bcm_ft_key_field_get(pk, _REGEX_KEY_FIELD_V4_OFFSET_PORT_2,
                                  _REGEX_KEY_FIELD_V4_LENGTH_PORT_2, &p2);
            _bcm_ft_key_field_get(pk, _REGEX_KEY_FIELD_V4_OFFSET_PROTOCOL,
                                  _REGEX_KEY_FIELD_V4_LENGTH_PROTOCOL, &pt);
            _bcm_ft_key_field_get(pk, _REGEX_KEY_FIELD_V4_OFFSET_IP_1,
                                  _REGEX_KEY_FIELD_V4_LENGTH_IP_1, &ip1);
            _bcm_ft_key_field_get(pk, _REGEX_KEY_FIELD_V4_OFFSET_IP_2,
                                  _REGEX_KEY_FIELD_V4_LENGTH_IP_2, &ip2);

            if (dir == _REGEX_SESSION_DIRECTION_REVERSE) {
              data->sip = ip1;
              data->dip = ip2;
              data->src_port = p1;
              data->dst_port = p2;
            } else {
              data->dip = ip1;
              data->sip = ip2;
              data->dst_port = p1;
              data->src_port = p2;
            }
            data->protocol = pt;
        } else if (type == _REGEX_SESSION_KEY_TYPE_IPV6_UPPER) {
            LOG_INFO(BSL_LS_BCM_REGEX,
                     (BSL_META_U(unit,
                                 " Get upper address, direction = %d...\n"),
                      dir));
            *pending = 1;
            data->dip = 0;
            data->sip = 0;
            data->src_port = 0;
            data->dst_port = 0;
            data->protocol = 0;
            p1 = 0;
            p2 = 0;
            pt = 0;
            ip1 = 0;
            ip2 = 0;

            if (dir == _REGEX_SESSION_DIRECTION_FORWARD) {
                _bcm_ft_key_field_get_char(pk, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                           _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                           &sip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
                } else {
                _bcm_ft_key_field_get_char(pk, _REGEX_KEY_UPPER_V6_OFFSET_IP_2,
                                           _REGEX_KEY_UPPER_V6_LENGTH_IP_2,
                                           &dip6[_REGEX_KEY_LOWER_V6_LENGTH_IP_2/8]);
            }
        }

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " FT_EXPORT_FIFOm = 0x%x, mem = 0x%x\n"), FT_EXPORT_FIFOm,
                  mem));

        if (data->flags & BCM_REGEX_REPORT_NEW) {
             data->packet_count_to_server = 0;
             data->byte_count_to_server   = 0;
             data->packet_count_to_client = 0;
             data->byte_count_to_client = 0;
             data->packet_count = 0;
         }

        soc_mem_field64_get(unit, mem, entry, L2_PKT_CNT_Ff,
                            &tmp_packet_count);
        COMPILER_64_ADD_64(data->packet_count_to_server,tmp_packet_count);

        soc_mem_field64_get(unit, mem, entry, L2_BYTE_CNT_Ff,
                            &tmp_byte_count);
        COMPILER_64_ADD_64(data->byte_count_to_server,tmp_byte_count);

        soc_mem_field64_get(unit, mem, entry, L2_PKT_CNT_Rf,
                            &tmp_packet_count);
        COMPILER_64_ADD_64(data->packet_count_to_client,tmp_packet_count);

        soc_mem_field64_get(unit, mem, entry, L2_BYTE_CNT_Rf,
                            &tmp_byte_count);
        COMPILER_64_ADD_64(data->byte_count_to_client,tmp_byte_count);

        data->byte_count = data->byte_count_to_client +
            data->byte_count_to_server;
        data->packet_count = data->packet_count_to_client +
            data->packet_count_to_server;

        if (_bcm_ft_ctr_enabled(unit) != 2)
        {
            data->packet_count_to_server = 0;
            data->byte_count_to_server   = 0;
            data->packet_count_to_client = 0;
            data->byte_count_to_client   = 0;
            if (_bcm_ft_ctr_enabled(unit) == 0) {
               data->packet_count = 0;
               data->byte_count = 0;
            }          
        }

        /*
        These values can look confusing, but ID_TIMESTAMP represents the
        time at which the flow was created, and stays the same while the
        flow exists.

        LAST_TIMESTAMP represents the last time a packet was received on
        the flow.

        START_TIMESTAMP is somewhat confusing, since it seems it should
        represent the time at which the flow started, but actually its
        value can change while the flow exists, and will actually show
        the last time the flow was refreshed.
        */

        data->start_timestamp = soc_mem_field32_get(unit, mem, entry, ID_TIMESTAMPf);
        data->last_timestamp = soc_mem_field32_get(unit, mem, entry, LAST_TIMESTAMPf);
        data->refresh_timestamp = soc_mem_field32_get(unit, mem, entry, START_TIMESTAMPf);
    } else {
        data->dip = 0;
        data->sip = 0;

        dir = *pdir;

        type = soc_mem_field32_get(unit, mem, entry, KEY_TYPEf);
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " KEY_TYPE = 0x%x\n"),
                  type));
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             " Get lower address, direction = %d, ports, proto...\n"),
                  dir));

        soc_mem_field_get(unit, mem, entry, IPV6_LOWER_KEYf, pkl);
        p8 = (uint8*) pkl;
        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "pkl = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n"),
                  *p8, *(p8+1), *(p8+2), *(p8+3), *(p8+4), *(p8+5), *(p8+6), *(p8+7), *(p8+8), *(p8+9), *(p8+10), *(p8+11), *(p8+12),
                  *(p8+13), *(p8+14), *(p8+15), *(p8+16), *(p8+17), *(p8+18), *(p8+19), *(p8+20), *(p8+21), *(p8+22), *(p8+23)));

        _bcm_ft_key_field_get(pkl, _REGEX_KEY_LOWER_V6_OFFSET_PORT_1,
                              _REGEX_KEY_LOWER_V6_LENGTH_PORT_1, &p1);
        _bcm_ft_key_field_get(pkl, _REGEX_KEY_LOWER_V6_OFFSET_PORT_2,
                              _REGEX_KEY_LOWER_V6_LENGTH_PORT_2, &p2);
        _bcm_ft_key_field_get(pkl, _REGEX_KEY_LOWER_V6_OFFSET_PROTOCOL,
                              _REGEX_KEY_LOWER_V6_LENGTH_PROTOCOL, &pt);

        if (dir == _REGEX_SESSION_DIRECTION_REVERSE) {
          data->src_port = p1;
          data->dst_port = p2;
            } else {
          data->dst_port = p1;
          data->src_port = p2;
            }
        data->protocol = pt;

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "p8 = %p, p8+port/proto = %p\n"),
                  p8, p8+_REGEX_PROTOCOL_PORT_LENGTH_V6/8));
        p8 += _REGEX_PROTOCOL_PORT_LENGTH_V6/8; /* skip past ports/proto */

        LOG_INFO(BSL_LS_BCM_REGEX,
                 (BSL_META_U(unit,
                             "pkl+port/proto = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n"),
                  *p8, *(p8+1), *(p8+2), *(p8+3), *(p8+4), *(p8+5), *(p8+6), *(p8+7), *(p8+8), *(p8+9), *(p8+10), *(p8+11), *(p8+12),
                  *(p8+13), *(p8+14), *(p8+15), *(p8+16), *(p8+17), *(p8+18)));

        if (dir == _REGEX_SESSION_DIRECTION_FORWARD) {
            _bcm_ft_key_field_get_char(pkl, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_1, dip6);
        } else {
            _bcm_ft_key_field_get_char(pkl, _REGEX_KEY_LOWER_V6_OFFSET_IP_1,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_1, sip6);
        }
        if (dir == _REGEX_SESSION_DIRECTION_FORWARD) {
            _bcm_ft_key_field_get_char(pkl, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_2, sip6);
            } else {
            _bcm_ft_key_field_get_char(pkl, _REGEX_KEY_LOWER_V6_OFFSET_IP_2,
                                  _REGEX_KEY_LOWER_V6_LENGTH_IP_2, dip6);
            }

        /*
        IPv6 addresses will go into hardware in reverse order, so reverse
        the addresses here before returning them
        */

        for (i = 0; i < _SHR_L3_IP6_ADDRLEN; i++) {
            data->sip6[i] = sip6[_SHR_L3_IP6_ADDRLEN - i - 1];
            data->dip6[i] = dip6[_SHR_L3_IP6_ADDRLEN - i - 1];
        }

        *pdir = dir;
    }

    data->match_flags = 0;
    data->match_flags |= (*pdir == 1) ? BCM_REGEX_MATCH_TO_SERVER :
                                 BCM_REGEX_MATCH_TO_CLIENT;

    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "Flags :0x%x\n"),
              data->flags));
    LOG_INFO(BSL_LS_BCM_REGEX,
             (BSL_META_U(unit,
                         "Match Flags :0x%x\n"),
              data->match_flags));

    return BCM_E_NONE;
}

STATIC void
_bcm_report_fifo_dma_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    _bcm_ft_report_ctrl_t *rctrl = _bcm_ft_report_ctrl[unit];
    _bcm_ft_report_cb_entry_t *cb_entry;
    bcm_regex_report_t data;
    int rv, entries_per_buf, interval, count, i, j, non_empty;
    int chan, entry_words, pending=0;
    void *host_buf = NULL;
    void *host_entry;
    uint32 dir = 0, *buff_max, rval;
    uint8  overflow, timeout;
    soc_mem_t   ftmem;
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc = SOC_PCI_CMC(unit);
    bcm_regex_config_t config;

    chan = SOC_MEM_FIFO_DMA_CHANNEL_0;
    rv = bcm_esw_regex_config_get(unit, &config);
    if (BCM_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_BCM_INTR,
                    (BSL_META_U(unit,
                                " failed to retrieve configuration, rv = %d\n"), rv));
        goto cleanup_exit;
    }
    entries_per_buf = config.report_buffer_size;

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            " starting _bcm_report_fifo_dma_thread\n")));

    ftmem = FT_EXPORT_FIFOm;
    entry_words = soc_mem_entry_words(unit, ftmem);
    host_buf = soc_cm_salloc(unit, entries_per_buf * entry_words * WORDS2BYTES(1),
                             "FT export fifo DMA Buffer");
    if (host_buf == NULL) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_REGEX_REPORT, __LINE__,
                           BCM_E_MEMORY);
        goto cleanup_exit;
    }

    rv = soc_mem_fifo_dma_start(unit, chan,
                                ftmem, MEM_BLOCK_ANY,
                                entries_per_buf, host_buf);
    if (BCM_FAILURE(rv)) {
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR,
                           SOC_SWITCH_EVENT_THREAD_REGEX_REPORT,
                           __LINE__, rv);
        LOG_VERBOSE(BSL_LS_BCM_INTR,
                    (BSL_META_U(unit,
                                " soc_mem_fifo_dma_start failed, rv = %d\n"), rv));
        goto cleanup_exit;
    }

    host_entry = host_buf;
    buff_max = (uint32 *)host_entry + (entries_per_buf * entry_words);

    while ((interval = rctrl->interval) > 0) {
        overflow = 0; timeout = 0;
        if (soc->ftreportIntrEnb) {
            soc_cmicm_intr0_enable(unit, IRQ_CMCx_FIFO_CH_DMA(chan));
            if (sal_sem_take(SOC_CONTROL(unit)->ftreportIntr, interval) < 0) {
                LOG_VERBOSE(BSL_LS_BCM_INTR,
                            (BSL_META_U(unit,
                                        " polling timeout ft_export_fifo=%d\n"), interval));
            } else {
                /* Disabling the interrupt (CHAN0) as the read process is underway */
                soc_cmicm_intr0_disable(unit, IRQ_CMCx_FIFO_CH_DMA(chan));
                LOG_VERBOSE(BSL_LS_BCM_INTR,
                            (BSL_META_U(unit,
                                        "woken up interval=%d\n"), interval));
                /* check for timeout or overflow and either process or continue */
                rval = soc_pci_read(unit,
                                    CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, chan));
                overflow = soc_reg_field_get(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STATr,
                                             rval, HOSTMEM_TIMEOUTf);
                timeout = soc_reg_field_get(unit,
                                            CMIC_CMC0_FIFO_CH0_RD_DMA_STATr, rval, HOSTMEM_OVERFLOWf);
                overflow |= timeout ? 1 : 0;
            }
        } else {
            sal_usleep(interval);
        }

        if (rctrl->interval <= 0) {
            break;
        }

        /* reconcile the user registered callbacks. */
        for (i = 0; i < _BCM_FT_REPORT_MAX_CB; i++) {
            cb_entry = &rctrl->callback_entry[i];
            switch (cb_entry->state) {
                case _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED:
                    cb_entry->state = _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE;
                    break;
                case _BCM_FT_REPORT_CALLBACK_STATE_UNREGISTERED:
                    cb_entry->state = _BCM_FT_REPORT_CALLBACK_STATE_INVALID;
                    break;
                default:
                    break;
            }
        }

        non_empty = FALSE;
        do {
            rv = soc_mem_fifo_dma_get_num_entries(unit, chan, &count);
            if (SOC_SUCCESS(rv)) {
                non_empty = TRUE;
                for (i = 0; i < count; i++) {
                    rv = _bcm_ft_report_process_export_entry(unit, host_entry,
                                                             &data, &pending, &dir);
                    host_entry = (uint32 *)host_entry + entry_words;
                    /* handle roll over */
                    if ((uint32 *)host_entry >= buff_max) {
                        host_entry = host_buf;
                    }
                    for (j = 0; (j < _BCM_FT_REPORT_MAX_CB) && !pending && !rv; j++) {
                        cb_entry = &rctrl->callback_entry[j];
                        if ((cb_entry->state == _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE) &&
                            ((cb_entry->reports & BCM_REGEX_REPORT_ALL) |
                             (data.flags & cb_entry->reports))) {
                            cb_entry->callback(unit, &data, cb_entry->userdata);
                        }
                    }
                }
                if (overflow) {
                    rval = 0;
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr,
                                      &rval, HOSTMEM_OVERFLOWf, 1);
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr,
                                      &rval, HOSTMEM_TIMEOUTf, 1);
                    soc_pci_write(unit,
                                  CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, chan), rval);
                }
                (void)_soc_mem_sbus_fifo_dma_set_entries_read(unit, chan, i);
            } else {
                if (overflow) {
                    rval = 0;
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr,
                                      &rval, HOSTMEM_OVERFLOWf, 1);
                    soc_reg_field_set(unit, CMIC_CMC0_FIFO_CH0_RD_DMA_STAT_CLRr,
                                      &rval, HOSTMEM_TIMEOUTf, 1);
                    soc_pci_write(unit,
                                  CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_CLR_OFFSET(cmc, chan), rval);
                }
                LOG_VERBOSE(BSL_LS_BCM_INTR,
                            (BSL_META_U(unit,
                                        " soc_mem_fifo_dma_get_num_entries failed, rv=%d\n"), rv));
                non_empty = FALSE;
            }
        } while (non_empty);
    }

cleanup_exit:
    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            " stopping _bcm_report_fifo_dma_thread\n")));
    (void)soc_mem_fifo_dma_stop(unit, chan);

    if (host_buf != NULL) {
        soc_cm_sfree(unit, host_buf);
    }
    rctrl->pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

STATIC int
_bcm_regex_report_init(int unit)
{
    _bcm_ft_report_ctrl_t *rctrl = _bcm_ft_report_ctrl[unit];

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            " _bcm_regex_report_init\n")));
    if (rctrl == NULL) {
        rctrl = sal_alloc(sizeof(_bcm_ft_report_ctrl_t), "Re_Report_ctrl Data");
        if (rctrl == NULL) {
            return BCM_E_MEMORY;
        }
        sal_memset(rctrl, 0, sizeof(_bcm_ft_report_ctrl_t));

        _bcm_ft_report_ctrl[unit] = rctrl;

        rctrl->pid = SAL_THREAD_ERROR;
    }

    return BCM_E_NONE;
}

static int
bcm_regex_report_control(int unit, sal_usecs_t interval)
{
    _bcm_ft_report_ctrl_t *rctrl = _bcm_ft_report_ctrl[unit];
    char name[32];

    rctrl = _bcm_ft_report_ctrl[unit];

    sal_snprintf(name, sizeof(name), "bcmFtExportDma.%d", unit);

    rctrl->interval = interval;
    if (interval) {
        if (rctrl->pid == SAL_THREAD_ERROR) {
            rctrl->pid = sal_thread_create(name, SAL_THREAD_STKSZ,
                                                soc_property_get(unit, spn_BCM_FT_REPORT_THREAD_PRI, 50),
                                                _bcm_report_fifo_dma_thread,
                                                INT_TO_PTR(unit));
            if (rctrl->pid == SAL_THREAD_ERROR) {
                LOG_ERROR(BSL_LS_BCM_COMMON,
                          (BSL_META_U(unit,
                                      "Could not start thread %s\n"), name));
                return BCM_E_MEMORY;
            }
        }
    } else {
        /* Wake up thread so it will check the changed interval value */
        sal_sem_give(SOC_CONTROL(unit)->ftreportIntr);
    }

    return BCM_E_NONE;
}

int
_bcm_esw_regex_report_register(int unit, uint32 reports,
                       bcm_regex_report_cb callback, void *userdata)
{
    _bcm_ft_report_ctrl_t *rctrl;
    _bcm_ft_report_cb_entry_t *entry;
    int free_index, i;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ft_report_ctrl[unit] == NULL) {
        BCM_IF_ERROR_RETURN(_bcm_regex_report_init(unit));
    }
    rctrl = _bcm_ft_report_ctrl[unit];

    if (callback == NULL) {
        return BCM_E_PARAM;
    }

    free_index = -1;
    for (i = 0; i < _BCM_FT_REPORT_MAX_CB; i++) {
        entry = &rctrl->callback_entry[i];
        if (entry->state == _BCM_FT_REPORT_CALLBACK_STATE_INVALID ||
            (rctrl->pid == SAL_THREAD_ERROR &&
             entry->state == _BCM_FT_REPORT_CALLBACK_STATE_UNREGISTERED)) {
            if (free_index < 0) {
                free_index = i;
            }
        } else if (entry->state == _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE ||
                   entry->state == _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED) {
            if (entry->callback == callback && entry->userdata == userdata) {
                return BCM_E_NONE;
            }
        }
    }

    if (free_index < 0) {
        return BCM_E_RESOURCE;
    }
    entry = &rctrl->callback_entry[free_index];
    entry->callback = callback;
    entry->userdata = userdata;
    entry->reports = reports;
    entry->state = _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED;

    return bcm_regex_report_control(unit, 100000);
}

int
_bcm_esw_regex_report_unregister(int unit, uint32 reports,
                         bcm_regex_report_cb callback,
                         void *userdata)
{
    _bcm_ft_report_ctrl_t *rctrl;
    _bcm_ft_report_cb_entry_t *entry;
    int i, num_active = 0;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ft_report_ctrl[unit] == NULL) {
        return BCM_E_INIT;
    }
    rctrl = _bcm_ft_report_ctrl[unit];

    for (i = 0; i < _BCM_FT_REPORT_MAX_CB; i++) {
        entry = &rctrl->callback_entry[i];
        if (entry->callback == callback && entry->userdata == userdata) {
            entry->reports &= ~reports;
            if (!entry->reports) {
                entry->state = _BCM_FT_REPORT_CALLBACK_STATE_UNREGISTERED;
                continue;
            }
        }
        if ((entry->state == _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE) ||
            (entry->state == _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED)) {
            num_active++;
        }
    }
    if (num_active == 0) {
        bcm_regex_report_control(unit, 0);
    }
    return BCM_E_NONE;
}

int _bcm_esw_regex_report_reset(int unit)
{
    _bcm_ft_report_ctrl_t *rctrl;
    _bcm_ft_report_cb_entry_t *entry;
    int i;

    if (!soc_feature(unit, soc_feature_regex)) {
        return BCM_E_UNAVAIL;
    }

    if (_bcm_ft_report_ctrl[unit] == NULL) {
        return BCM_E_NONE;
    }

    rctrl = _bcm_ft_report_ctrl[unit];

    for (i = 0; i < _BCM_FT_REPORT_MAX_CB; i++) {
        entry = &rctrl->callback_entry[i];
        if ((entry->state == _BCM_FT_REPORT_CALLBACK_STATE_ACTIVE) ||
            (entry->state == _BCM_FT_REPORT_CALLBACK_STATE_REGISTERED)) {
            _bcm_esw_regex_report_unregister(unit, entry->reports,
                                             entry->callback, entry->userdata);
        }
    }
    return BCM_E_NONE;
}

#else
int
_bcm_esw_regex_report_register(int unit, uint32 reports,
                       bcm_regex_report_cb callback, void *userdata)
{
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_regex_report_unregister(int unit, uint32 reports,
                         bcm_regex_report_cb callback,
                         void *userdata)
{
    return BCM_E_UNAVAIL;
}

int _bcm_esw_regex_report_reset(int unit)
{
    return BCM_E_UNAVAIL;
}


#endif /* BCM_TRIUMPH3_SUPPORT */

#endif /* INCLUDE_REGEX */

