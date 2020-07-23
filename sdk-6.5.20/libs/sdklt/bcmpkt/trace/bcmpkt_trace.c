/*! \file bcmpkt_trace.c
 *
 * Interfaces for packet trace instrument.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_timeout.h>
#include <shr/shr_util.h>
#include <sal/sal_sleep.h>
#include <sal/sal_time.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmpkt/bcmpkt_trace.h>
#include <bcmpkt/bcmpkt_trace_internal.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_lbhdr.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmpkt/bcmpkt_internal.h>
#include <bcmlrd/bcmlrd_conf.h>

#define BSL_LOG_MODULE          BSL_LS_BCMPKT_TRACE

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_fget_t _bd##_trace_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_fget,
static const bcmpkt_trace_fget_t *trace_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_figet_t _bd##_trace_figet;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_figet,
static const bcmpkt_trace_figet_t *trace_figet[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_drop_reason_decode,
static void (*drop_reason_fdecode[])(const uint32_t*, bcmpkt_trace_drop_reasons_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_counter_decode,
static void (*counter_fdecode[])(const uint32_t*, bcmpkt_trace_counters_t*) = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_dop_fids_get_t _bd##_trace_dop_fids_get;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_dop_fids_get,
static const bcmpkt_trace_dop_fids_get_t *trace_dop_fids_get[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_dop_fget_t _bd##_trace_dop_fget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_dop_fget,
static const bcmpkt_trace_dop_fget_t *trace_dop_fget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_dop_iget_t _bd##_trace_dop_iget;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_dop_iget,
static const bcmpkt_trace_dop_iget_t *trace_dop_iget[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern const bcmpkt_trace_pt_to_dop_info_get_t _bd##_trace_pt_to_dop_info_get;
#include <bcmdrd/bcmdrd_devlist.h>

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    &_bd##_trace_pt_to_dop_info_get,
static const bcmpkt_trace_pt_to_dop_info_get_t *trace_pt_to_dop_info_get[] = {
    NULL,
#include <bcmdrd/bcmdrd_devlist.h>
    NULL
};

typedef int (*bcmpkkt_trace_drv_f)(bcmpkt_trace_drv_t *drv);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_trace_drv_attach },
static struct {
    bcmpkkt_trace_drv_f attach;
} trace_drv_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcmpkt_trace_drv_t trace_drv[BCMDRD_CONFIG_MAX_UNITS];

static const shr_enum_map_t field_names[] =
{
    BCMPKT_TRACE_FIELD_NAME_MAP_INIT
};

static const shr_enum_map_t drop_reason_names[] =
{
    BCMPKT_TRACE_DROP_REASON_NAME_MAP_INIT
};

static const shr_enum_map_t counter_names[] =
{
    BCMPKT_TRACE_COUNTER_NAME_MAP_INIT
};

static const shr_enum_map_t pkt_resolution_names[] =
{
    BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_NAME_MAP_INIT
};

static const shr_enum_map_t tag_status_names[] =
{
    BCMPKT_TRACE_INCOMING_TAG_STATUS_NAME_MAP_INIT
};

static const shr_enum_map_t fwd_type_names[] =
{
    BCMPKT_TRACE_FORWARDING_TYPE_NAME_MAP_INIT
};

static const shr_enum_map_t fwd_dest_type_names[] =
{
    BCMPKT_TRACE_FWD_DESTINATION_TYPE_NAME_MAP_INIT
};

int
bcmpkt_trace_dop_num_to_dop_info_get (int unit, uint32_t dop_num,
                                      bcmpkt_trace_dop_info_t *dop_info)
{
    bcmdrd_dev_type_t dev_type;
    bcmpkt_trace_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_NULL_CHECK(dop_info, SHR_E_PARAM);

    drv = bcmpkt_trace_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNAVAIL);

    SHR_NULL_CHECK(drv->dop_info_get, SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT(drv->dop_info_get(unit, dop_num, dop_info));

exit:
    SHR_FUNC_EXIT();
}

bcmpkt_trace_drv_t*
bcmpkt_trace_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &trace_drv[unit];
}

int
bcmpkt_trace_init(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    bcmpkt_trace_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        /* Nothing to be done when warm-booting. */
        SHR_EXIT();
    }

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    drv = bcmpkt_trace_drv_get(unit);
    SHR_IF_ERR_EXIT
        (trace_drv_attach[dev_type].attach(drv));

    if ((drv == NULL) || (drv->profile_init == NULL)) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "drvier not attached\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT
        (drv->profile_init(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_dop_data_collect(int unit, int port,
                              uint32_t  dop_num,
                              bcmpkt_trace_data_t *trace_data)
{
    int rv;
    bcmpkt_trace_drv_t *drv;
    shr_timeout_t to;
    sal_usecs_t timeout_usec = 1000000;  /* 1 second */
    uint32_t data_sz;
    bcmpkt_trace_dop_info_t dop_info;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    drv = bcmpkt_trace_drv_get(unit);
    SHR_NULL_CHECK(drv, SHR_E_UNAVAIL);

    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);

    if (bcmdrd_dev_logic_port_pipe(unit, port) < 0) {
        return SHR_E_PARAM;
    }

    rv = bcmpkt_trace_max_dop_phase_get(unit, &data_sz);
    if (SHR_FAILURE(rv)) {
        return SHR_E_FAIL;
    }

    /* Convert word count to byte count */
    data_sz = SHR_WORDS2BYTES(data_sz);

    if (trace_data->buf_size < data_sz) {
        trace_data->len = data_sz;
        return SHR_E_PARAM;
    }

    if (trace_data->buf_size) {
        SHR_NULL_CHECK(trace_data->buf, SHR_E_PARAM);
    } else {
        return SHR_E_PARAM;
    }

    SHR_IF_ERR_EXIT(bcmpkt_trace_dop_num_to_dop_info_get(unit, dop_num,
                                                         &dop_info));
    SHR_NULL_CHECK(drv->dop_data_read, SHR_E_UNAVAIL);

    shr_timeout_init(&to, timeout_usec, 0);
    while (1) {
        rv = drv->dop_data_read(unit, port, dop_info.dop_id,
                                dop_info.group_id,
                                &(trace_data->len), trace_data->buf);
        if (rv == SHR_E_NONE) {
            break;
        }
        if (shr_timeout_check(&to)) {
            rv = SHR_E_TIMEOUT;
            break;
        }
        sal_usleep(timeout_usec/100);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_dop_fids_get(int unit,
                          uint32_t dop_num,
                          uint32_t *fid_list, uint8_t *fid_count)
{
    uint32_t    dev_type;
    uint32_t    len;
    uint32_t    ing_dop_count;
    uint32_t    egr_dop_count;
    uint32_t    dop_count;
    int         rv;
    uint32_t    fid_max_cnt;
    uint32_t    dop_id_idx;
    bcmpkt_trace_dop_info_t dop_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fid_list, SHR_E_PARAM);
    SHR_NULL_CHECK(fid_count, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    rv = bcmpkt_trace_max_dop_field_count_get(unit, &fid_max_cnt);
    if (SHR_FAILURE(rv)) {
        return SHR_E_FAIL;
    }

    if (*fid_count < fid_max_cnt) {
        return SHR_E_PARAM;
    }

    ing_dop_count = trace_dop_iget[dev_type]->\
                        fget[BCMPKT_TRACE_DOP_I_ING_DOP_COUNT]();

    egr_dop_count = trace_dop_iget[dev_type]->\
                        fget[BCMPKT_TRACE_DOP_I_ING_DOP_COUNT]();

    dop_count = ing_dop_count + egr_dop_count;

    SHR_IF_ERR_EXIT(bcmpkt_trace_dop_num_to_dop_info_get(unit, dop_num, &dop_info));

    /* Compute the index in to <CHIP>_trace_dop_fids_get array. */
    dop_id_idx = (ing_dop_count * dop_info.group_id) + dop_info.dop_id - 1;
    if (dop_id_idx >= dop_count) {
        return SHR_E_PARAM;
    }

    len = trace_dop_fids_get[dev_type]->fget[dop_id_idx](fid_list, fid_count);
    if (len == 0) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_pt_lookup_info_get (int unit, const char *pt_name,
                                 uint8_t *data,
                                 int port,
                                 uint32_t *pt_lookup)
{
    uint32_t    dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pt_name, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_lookup, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (bcmdrd_dev_logic_port_pipe(unit, port) < 0) {
        return SHR_E_PARAM;
    }

    if (trace_pt_to_dop_info_get[dev_type] == NULL ||
        trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_LOOKUP_INFO] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_LOOKUP_INFO](unit,
                                                          pt_name,
                                                          data,
                                                          port,
                                                          pt_lookup));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_pt_hit_info_get (int unit, const char *pt_name,
                              uint8_t *data,
                              int port,
                              uint32_t *pt_hit)
{
    uint32_t    dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    SHR_NULL_CHECK(pt_name, SHR_E_PARAM);
    SHR_NULL_CHECK(data, SHR_E_PARAM);
    SHR_NULL_CHECK(pt_hit, SHR_E_PARAM);

    if (bcmdrd_dev_logic_port_pipe(unit, port) < 0) {
        return SHR_E_PARAM;
    }

    if (trace_pt_to_dop_info_get[dev_type] == NULL ||
        trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_HIT_INFO] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_HIT_INFO](unit,
                                                       pt_name,
                                                       data,
                                                       port,
                                                       pt_hit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_pt_to_dop_info_get (int unit, const char *pt_name,
                                 uint8_t *data,
                                 uint32_t *dop_num,
                                 uint32_t *group,
                                 uint32_t *fid)
{
    uint32_t    dev_type;
    uint32_t    port = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pt_name, SHR_E_PARAM);
    SHR_NULL_CHECK(dop_num, SHR_E_PARAM);
    SHR_NULL_CHECK(group, SHR_E_PARAM);
    SHR_NULL_CHECK(fid, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_pt_to_dop_info_get[dev_type] == NULL ||
        trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_NUM] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_NUM](unit, pt_name, data,
                                                        port, dop_num));

    if (trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_GROUP] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_GROUP](unit, pt_name, data,
                                                           port, group));

    if (trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_FID] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT(trace_pt_to_dop_info_get[dev_type]->\
                        fget[BCMPKT_TRACE_PT_TO_DOP_FID](unit, pt_name, data,
                                                         port, fid));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_data_collect(int unit, bcmpkt_trace_pkt_t *trace_pkt,
                          bcmpkt_trace_data_t *trace_data)
{
    int rv, lbport, port, pipe, buf_unit, val, modid = 0;
    bcmpkt_packet_t *pkt = NULL;
    uint32_t *txpmd, *lbhdr, dev_type, modport;
    sal_usecs_t timeout_usec = 1000000;  /* 1 second */
    shr_timeout_t to;
    bcmdrd_pbmp_t pbmp;
    bcmpkt_trace_drv_t *drv;
    bool generic_loopback_is_supported = false;
    bcmlrd_variant_t variant;
    uint32_t data_sz;
    int system_source;
    int fid;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmpkt_dev_type_get(unit, &dev_type));

    SHR_NULL_CHECK(trace_pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_pkt->pkt, SHR_E_PARAM);
    SHR_NULL_CHECK(trace_data, SHR_E_PARAM);
    if (trace_data->buf_size) {
        SHR_NULL_CHECK(trace_data->buf, SHR_E_PARAM);
    }

    drv = bcmpkt_trace_drv_get(unit);
    SHR_NULL_CHECK(drv, SHR_E_UNAVAIL);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_data->buf != NULL) {
        rv = bcmpkt_trace_data_len_get(unit, &data_sz);
        if (SHR_FAILURE(rv)) {
            return SHR_E_FAIL;
        }

        if (trace_data->buf_size < data_sz) {
            trace_data->len = data_sz;
            return SHR_E_MEMORY;
        }
    }

    /* Construct packet */
    buf_unit = BCMPKT_BPOOL_SHARED_ID;
    SHR_IF_ERR_EXIT
        (bcmpkt_alloc(buf_unit, trace_pkt->len, BCMPKT_BUF_F_TX, &pkt));
    SHR_NULL_CHECK(pkt, SHR_E_FAIL);
    if (trace_pkt->len < 64) {
        bcmpkt_put(pkt->data_buf, 64);
    } else {
        bcmpkt_put(pkt->data_buf, trace_pkt->len);
    }

    sal_memcpy(BCMPKT_PACKET_DATA(pkt), trace_pkt->pkt, BCMPKT_PACKET_LEN(pkt));

    drv = bcmpkt_trace_drv_get(unit);
    SHR_NULL_CHECK(drv, SHR_E_UNAVAIL);

    pipe = bcmdrd_dev_logic_port_pipe(unit, trace_pkt->port);
    if (drv->get_lbpipe) {
        drv->get_lbpipe(dev_type, pipe, &pipe);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_dev_lb_pbmp(unit, &pbmp));
    lbport = BCMPC_INVALID_PPORT;
    BCMDRD_PBMP_ITER(pbmp, port) {
        if (pipe == bcmdrd_dev_phys_port_pipe(unit, port)) {
            lbport = port;
            break;
        }
    }
    if (lbport == BCMPC_INVALID_PPORT) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Loopback port not configured\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    lbport = bcmpc_pport_to_lport(unit, lbport);
    if (lbport == BCMPC_INVALID_LPORT) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Loopback port not configured\n")));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Fill txpmd header */
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_get(pkt, &txpmd));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_START, 2));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_HEADER_TYPE, 1));

    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_LOCAL_DEST_PORT, lbport));

    rv = bcmcth_port_modid_get(unit, port, &modid);
    if (rv == SHR_E_NONE) {
        rv = bcmpkt_txpmd_field_set(dev_type, txpmd,
                                    BCMPKT_TXPMD_SRC_MODID, modid);
        if (rv != SHR_E_UNAVAIL) {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    SHR_NULL_CHECK(drv->get_txpmd_val, SHR_E_UNAVAIL);
    drv->get_txpmd_val(dev_type, BCMPKT_TXPMD_SET_L2BM, &val);
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_SET_L2BM, val));

    SHR_NULL_CHECK(drv->get_txpmd_val, SHR_E_UNAVAIL);
    drv->get_txpmd_val(dev_type, BCMPKT_TXPMD_UNICAST, &val);
    SHR_IF_ERR_EXIT
        (bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_UNICAST, val));

    drv->get_txpmd_val(dev_type, BCMPKT_TXPMD_UNICAST_PKT, &val);
    rv = bcmpkt_txpmd_field_set(dev_type, txpmd,
                                BCMPKT_TXPMD_UNICAST_PKT, val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL);

    /* Fill loopback header */
    SHR_IF_ERR_EXIT
        (bcmpkt_lbhdr_get(pkt, &lbhdr));

    SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_is_supported(variant,
                                                           &generic_loopback_is_supported));

    if (!generic_loopback_is_supported) {
        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_START, 0xFB));
    }

    port = bcmpc_lport_to_pport(unit, trace_pkt->port);
    if (port == BCMPC_INVALID_PPORT) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    rv = bcmcth_port_modid_get(unit, port, &modid);
    if (rv != SHR_E_NONE) {
        modid = 0;
    }
    modport = (modid << 8) + trace_pkt->port;

    if (!generic_loopback_is_supported) {
        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_SOURCE, modport));
        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_SOURCE_TYPE, 1));
        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_HEADER_TYPE, 3));
        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_ETH_PP_PORT,
                                    trace_pkt->port));

        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_VISIBILITY_PKT, 1));

        SHR_IF_ERR_EXIT
            (bcmpkt_lbhdr_field_set(dev_type, lbhdr,
                                    BCMPKT_LBHDR_PKT_PROFILE, trace_pkt->options));
    } else {
        SHR_IF_ERR_EXIT
            (bcmcth_port_system_source_get(unit, trace_pkt->port,
                                           &system_source));

        SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("START_BYTE",
                                                               &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, 0x82));

        SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("SOURCE_SYSTEM_PORT",
                                                               &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, system_source));

        SHR_IF_ERR_EXIT(bcmpkt_generic_loopback_t_field_id_get("FLAGS", &fid));
        SHR_IF_ERR_EXIT
            (bcmpkt_generic_loopback_t_field_set(variant, lbhdr, fid, 1));
    }

    /* Send out the packet */
    SHR_IF_ERR_EXIT
        (bcmpkt_tx(unit, trace_pkt->netif_id, pkt));

    if (trace_data->buf == NULL) {
        /* Skip the data read */
        SHR_EXIT();
    }

    SHR_NULL_CHECK(drv->data_read, SHR_E_UNAVAIL);
    shr_timeout_init(&to, timeout_usec, 0);
    while (1) {
        rv = drv->data_read(unit, trace_pkt->port,
                            &(trace_data->len),
                            trace_data->buf);
        if (rv == SHR_E_NONE) {
            break;
        }
        if (shr_timeout_check(&to)) {
            rv = SHR_E_TIMEOUT;
            break;
        }
        sal_usleep(timeout_usec/100);
    }

exit:
    if (pkt) {
        bcmpkt_free(pkt->unit, pkt);
        pkt = NULL;
    }
    SHR_FUNC_EXIT();
}


int
bcmpkt_trace_dop_field_get(int unit, const uint8_t *raw_data,
                           int fid, uint32_t *field_data,
                           uint8_t *field_data_len)
{
    bcmdrd_dev_type_t dev_type;
    uint32_t data_sz = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(raw_data, SHR_E_PARAM);
    SHR_NULL_CHECK(field_data, SHR_E_PARAM);
    SHR_NULL_CHECK(field_data_len, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    rv = bcmpkt_trace_max_dop_phase_get(unit, &data_sz);
    if (SHR_FAILURE(rv)) {
        return SHR_E_FAIL;
    }

    /* Convert word count to byte count */
    data_sz *= sizeof(uint32_t);

    if (*field_data_len < data_sz) {
        *field_data_len = 0;
        return SHR_E_PARAM;
    }

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (fid < 0 || fid >= BCMPKT_TRACE_DOP_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (trace_dop_fget[dev_type] == NULL ||
        trace_dop_fget[dev_type]->fget[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *field_data_len = trace_dop_fget[dev_type]->fget[fid]((uint32_t *)raw_data,
                                                          (uint32_t *)field_data,
                                                          *field_data_len);
exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_field_get(int unit, const uint8_t *raw_data,
                       int fid, uint32_t *val)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(raw_data, SHR_E_PARAM);
    SHR_NULL_CHECK(val, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (fid < 0 || fid >= BCMPKT_TRACE_FID_COUNT) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (trace_fget[dev_type] == NULL ||
        trace_fget[dev_type]->fget[fid] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *val = trace_fget[dev_type]->fget[fid]((uint32_t *)raw_data);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_drop_reasons_get(int unit, const uint8_t *raw_data,
                              bcmpkt_trace_drop_reasons_t *bitmap)
{
    uint32_t *reason = NULL;
    int len;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(raw_data, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_figet[dev_type] == NULL ||
        trace_figet[dev_type]->fget[BCMPKT_TRACE_I_DROP_REASON] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    BCMPKT_TRACE_DROP_REASON_CLEAR_ALL(*bitmap);
    len = trace_figet[dev_type]->fget[BCMPKT_TRACE_I_DROP_REASON]((uint32_t *)raw_data, &reason);
    if (len <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    drop_reason_fdecode[dev_type](reason, bitmap);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_counter_get(int unit, const uint8_t *raw_data,
                         bcmpkt_trace_counters_t *bitmap)
{
    uint32_t *reason = NULL;
    uint32_t counter[2];
    int len;
    bcmdrd_dev_type_t dev_type;
    bcmpkt_trace_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(raw_data, SHR_E_PARAM);
    SHR_NULL_CHECK(bitmap, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_figet[dev_type] == NULL ||
        trace_figet[dev_type]->fget[BCMPKT_TRACE_I_COUNTER] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    BCMPKT_TRACE_COUNTER_CLEAR_ALL(*bitmap);
    len = trace_figet[dev_type]->fget[BCMPKT_TRACE_I_COUNTER]((uint32_t *)raw_data, &reason);
    if (len <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    drv = bcmpkt_trace_drv_get(unit);
    SHR_NULL_CHECK(drv, SHR_E_UNAVAIL);
    if (drv->counter_shift) {
        drv->counter_shift(reason, counter);
        reason = counter;
    }

    counter_fdecode[dev_type](reason, bitmap);

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_ing_dop_cnt_get(int unit, uint32_t *count)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_dop_iget[dev_type] == NULL ||
        trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_ING_DOP_COUNT] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *count = trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_ING_DOP_COUNT]();

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_egr_dop_cnt_get(int unit, uint32_t *count)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_dop_iget[dev_type] == NULL ||
        trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_EGR_DOP_COUNT] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *count = trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_EGR_DOP_COUNT]();

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_max_dop_phase_get(int unit, uint32_t *size)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(size, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_dop_iget[dev_type] == NULL ||
        trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *size = trace_dop_iget[dev_type]->fget[BCMPKT_TRACE_DOP_I_MAX_DOP_PHASE]();

exit:
    SHR_FUNC_EXIT();
}


int
bcmpkt_trace_max_dop_field_count_get(int unit, uint32_t *count)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(count, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (trace_dop_iget[dev_type] == NULL ||
        trace_dop_iget[dev_type]->\
            fget[BCMPKT_TRACE_DOP_I_MAX_DOP_FIELD_COUNT] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *count = trace_dop_iget[dev_type]->\
                fget[BCMPKT_TRACE_DOP_I_MAX_DOP_FIELD_COUNT]();

exit:
    SHR_FUNC_EXIT();
}


int
bcmpkt_trace_data_len_get(int unit, uint32_t *len)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(len, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    *len = 0;

    if (trace_figet[dev_type] == NULL ||
        trace_figet[dev_type]->fget[BCMPKT_TRACE_I_SIZE] == NULL) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *len =  trace_figet[dev_type]->fget[BCMPKT_TRACE_I_SIZE](NULL, NULL) * 4;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpkt_trace_fields_dump(int unit, const uint8_t *data, shr_pb_t *pb)
{
    int i, val;
    bcmdrd_dev_type_t dev_type;
    bcmpkt_trace_drop_reasons_t drop_bitmap;
    bcmpkt_trace_counters_t counter_bitmap;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(data, SHR_E_PARAM);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
       SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    for (i = 0; i < BCMPKT_TRACE_FID_COUNT; i++) {
        if (trace_fget[dev_type]->fget[i] != NULL) {
            val = trace_fget[dev_type]->fget[i]((uint32_t *)data);
            switch (i) {
                case BCMPKT_TRACE_PKT_RESOLUTION_VECTOR:
                    shr_pb_printf(pb, "\t%-32s:%s\n", field_names[i].name,
                                  pkt_resolution_names[val].name);
                    break;
                case BCMPKT_TRACE_INCOMING_TAG_STATUS:
                    shr_pb_printf(pb, "\t%-32s:%s\n", field_names[i].name,
                                  tag_status_names[val].name);
                    break;
                case BCMPKT_TRACE_FORWARDING_TYPE:
                    shr_pb_printf(pb, "\t%-32s:%s\n", field_names[i].name,
                                  fwd_type_names[val].name);
                    break;
                case BCMPKT_TRACE_FWD_DESTINATION_TYPE:
                    shr_pb_printf(pb, "\t%-32s:%s\n", field_names[i].name,
                                  fwd_dest_type_names[val].name);
                    break;
                default:
                    shr_pb_printf(pb, "\t%-32s:%d\n", field_names[i].name, val);
            }
        }
    }

    bcmpkt_trace_drop_reasons_get(unit, data, &drop_bitmap);
    shr_pb_printf(pb, "DROP_REASON:\n");
    for (i = 0; i < BCMPKT_TRACE_DROP_REASON_COUNT; i++) {
        if (SHR_BITGET(drop_bitmap.pbits, i)) {
            shr_pb_printf(pb, "\t%-32s\n", drop_reason_names[i].name);
        }
    }

    bcmpkt_trace_counter_get(unit, data, &counter_bitmap);
    shr_pb_printf(pb, "COUNTER:\n");
    for (i = 0; i < BCMPKT_TRACE_COUNTER_COUNT; i++) {
        if (SHR_BITGET(counter_bitmap.pbits, i)) {
            shr_pb_printf(pb, "\t%-32s\n", counter_names[i].name);
        }
    }
exit:
    SHR_FUNC_EXIT();
}
