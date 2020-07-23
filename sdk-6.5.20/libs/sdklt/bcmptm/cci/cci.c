/*! \file cci.c
 *
 * Interface functions for CCI
 *
 * This file contains the implementation of  top-level interface func for CCI
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_symbols.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmptm/bcmptm_cci_internal.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmissu/issu_api.h>
#include "cci_cache.h"
#include "cci_col.h"
#include "cci_internal.h"


#define IS_PIPE_ENABLE(map, p) ((map) & (0x01 << p))

#define ACC_FOR_SER FALSE

static cci_context_t   *cci_context[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * Collection infrastructure resource cleanup function
 */
static void
cci_cleanup(int unit, cci_context_t *con)
{
    if (con->hcol) {
        bcmptm_cci_col_cleanup(unit, con->hcol);
        con->hcol = NULL;
    }
    if (con->hcache) {
        bcmptm_cci_cache_cleanup(unit, con->hcache);
        con->hcache = NULL;
    }
    bcmptm_cci_imm_config_cleanup(unit, con);

    SHR_FREE(con);
}

/*!
 * Get  cci context.
 */
cci_handle_t
bcmptm_cci_context_get(int unit)
{
    cci_handle_t   handle;

    handle = cci_context[unit];

    return handle;
}

/*!
 * Register counter with CCI
 */
int
bcmptm_cci_ctr_reg(int unit,
                   bcmdrd_sid_t sid,
                   uint32_t *map_id)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(map_id, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_ctr_add(unit, con->hcache, sid, map_id, con->warm));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Copy the dynamic inormation, can be transformed
 */
static int
cci_dyn_copy(int unit,
             bcmptm_cci_ctr_info_t *info)
{
    bcmbd_pt_dyn_info_t *dyn = NULL;

    dyn = info->in_pt_dyn_info;
    info->dyn_info.tbl_inst = dyn->tbl_inst;
    info->dyn_info.index = dyn->index;

    return SHR_E_NONE;
}

/*!
 * Transform the symbol.
 */
static int
cci_ctr_txfm(int unit,
             cci_handle_t   handle,
             bcmptm_cci_ctr_info_t *info)
{
    bcmdrd_sid_t sid;
    bcmbd_pt_dyn_info_t *dyn = NULL;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bcmptm_cci_cache_ctr_info_t cinfo;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Port transform of port counters */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_lport_to_pport(unit, handle, info));
    /* Symbol and field transform */
    sid = info->sid;
    info->lt_map_id = info->map_id;
    dyn = info->in_pt_dyn_info;
    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_sym_map(unit, info));
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, info->sid,
                                    &ptcache_ctr_info));
    info->map_id = ptcache_ctr_info.cci_map_id;

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_ctr_info_get(unit, handle, info->map_id, &cinfo));

    /* Transformed */
    if (info->txfm_fld >= 0) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "SID = %s, Port = %d, Mapped SID = %s,"
                     "field = %d, MAP index = %d, MAP tbl_inst = %d\n"),
                     bcmdrd_pt_sid_to_name(unit, sid), dyn->tbl_inst,
                     bcmdrd_pt_sid_to_name(unit, info->sid), info->txfm_fld,
                     info->dyn_info.index, info->dyn_info.tbl_inst));
    } else {
       /* Transform the port into tbl_inst in case of port memory tables */
       if ((cinfo.ctrtype == CCI_CTR_TYPE_PORT) && (cinfo.is_mem)) {
             info->dyn_info.tbl_inst =
                bcmdrd_pt_tbl_inst_from_port(unit, info->sid,
                                             info->dyn_info.tbl_inst);
       }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
cci_hw_sync_req_read(int unit,
                     cci_handle_t   handle,
                     bcmptm_cci_ctr_info_t *info,
                     bcmptm_cci_cache_pt_param_t *param)
{

    int tmp_rv;

    SHR_FUNC_ENTER(unit);

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {
        tmp_rv = bcmptm_cci_cache_hw_sync(unit, handle,
                                          bcmptm_cci_cache_pt_req_read,
                                          param, TRUE);
        /* If SET error, retry read */
        if (BCMPTM_SHR_FAILURE_SER(tmp_rv)) {
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_read_cci(unit, ACC_FOR_SER,
                                      BCMLT_ENT_ATTR_GET_FROM_HW,
                                      info->sid, info->in_pt_dyn_info,
                                      info->in_pt_ovrr_info,
                                      param->size, info->req_ltid,
                                      param->buf,
                                      info->rsp_ltid, info->rsp_flags));
            SHR_IF_ERR_EXIT
                (bcmptm_cci_cache_hw_sync(unit, handle,
                                          bcmptm_cci_cache_pt_req_read,
                                          &param, TRUE));
        }

    }
exit:
    SHR_FUNC_EXIT();
}

/*!
* bcmptm_cci_ctr_read
* Buffer 64 bit counts
* If DONT_USE_CACHE=0, no change in cache
* If DONT_USE_CACHE=1, Update the cache with HW
*/
int
bcmptm_cci_ctr_read(int unit,
                    bcmptm_cci_ctr_info_t *info,
                    uint32_t *rsp_entry_words,
                    size_t     rsp_entry_size)
{
    cci_handle_t   handle;
    bool cor, emul;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    emul = bcmdrd_feature_enabled(unit, BCMDRD_FT_EMUL);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));
    /* Symbol Transform */
    SHR_IF_ERR_EXIT
        (cci_ctr_txfm(unit, handle, info));

    /* Sync the cache for counters with COR */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_clearon_read_get(unit, handle,
                                           info->map_id, &cor));
    /*
     * Sync CCI Cache with HW in case of flag is set.
     * Sync CCI Cache if COR is set but not emulator.
     */
    if ((info->flags & BCMLT_ENT_ATTR_GET_FROM_HW) || (cor && !emul)) {
        bcmptm_cci_cache_pt_param_t param;
        /* Read PT entry and Update cache */
        param.info = info;
        param.buf = rsp_entry_words;
        param.size = rsp_entry_size;

        SHR_IF_ERR_EXIT
            (cci_hw_sync_req_read(unit, handle, info, &param));

        /* Cache is updated with the latest value clear the buffer */
        sal_memset(rsp_entry_words, 0,
                   rsp_entry_size * sizeof(uint32_t));
    }
    /* Read Counter values from cache */
    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_read(unit, handle, info,
                              rsp_entry_words,
                              rsp_entry_size));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * bcmptm_cci_ctr_config_read
 * Buffer is in HW_entry format, Counter (Config Fileds) set
 * No change in CCI cache,
 * Issue read from HW if PTcache read was not successful.
 */
int
bcmptm_cci_ctr_config_read(int unit,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *rsp_entry_words,
                           size_t   rsp_entry_size)
{
    cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read_cci(unit, ACC_FOR_SER, info->flags, info->sid,
                             info->in_pt_dyn_info, info->in_pt_ovrr_info,
                             rsp_entry_size, info->req_ltid, rsp_entry_words,
                             info->rsp_ltid, info->rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_passthru_read
 * Buffer in HW_entry format, No change in CCI Cache
 * Issue read from HW if PTcache read was not successful
 */
int
bcmptm_cci_ctr_passthru_read(int unit,
                             bcmptm_cci_ctr_info_t *info,
                             uint32_t *rsp_entry_words,
                             size_t   rsp_entry_size)
{
    cci_handle_t   handle;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(rsp_entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Issue read from HW if PTcache read was not successful */

    SHR_IF_ERR_EXIT(
        bcmptm_ireq_read_cci(unit, ACC_FOR_SER, info->flags, info->sid,
                             info->in_pt_dyn_info, info->in_pt_ovrr_info,
                             rsp_entry_size, info->req_ltid, rsp_entry_words,
                             info->rsp_ltid, info->rsp_flags));

exit:
    SHR_FUNC_EXIT();
}

static int
cci_ireq_read(int unit,
              cci_handle_t   handle,
              bcmptm_cci_ctr_info_t *info,
              bcmptm_cci_cache_pt_param_t *param)
{
    bcmptm_cci_cache_ctr_info_t cinfo;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint32_t *rsp_entry_words = NULL;
    size_t rsp_entry_size;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, info->sid,
                                     &ptcache_ctr_info));

    if (ptcache_ctr_info.ctr_is_cci_only == TRUE) {
        info->map_id = ptcache_ctr_info.cci_map_id;
        /* Counter only fields, read HW and restore in read_cache. */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_info_get(unit, handle,
                                       info->map_id, &cinfo));

        rsp_entry_size = cinfo.pt_entry_wsize * sizeof(uint32_t);
        SHR_ALLOC(rsp_entry_words, rsp_entry_size, "bcmPtmCciPtBuff");
        SHR_NULL_CHECK(rsp_entry_words, SHR_E_MEMORY);

        SHR_IF_ERR_EXIT
            (bcmptm_ireq_read_cci(unit, ACC_FOR_SER,
                                  BCMLT_ENT_ATTR_GET_FROM_HW,
                                  info->sid, &info->dyn_info,
                                  info->in_pt_ovrr_info,
                                  cinfo.pt_entry_wsize, info->req_ltid,
                                  rsp_entry_words,
                                  info->rsp_ltid, info->rsp_flags));
         param->read_cache = rsp_entry_words;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_write
 * Buffer 64 bit counts, CCI Will be updated to user-provided value
 * No Change in PT Cache,
 * Zero out counter-only fields in HW by doing Read-modify-Write
 */
int
bcmptm_cci_ctr_write(int unit,
                     bcmptm_cci_ctr_info_t *info,
                     uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));
    /* Symbol Transform */
    SHR_IF_ERR_EXIT
        (cci_ctr_txfm(unit, handle, info));

    param.info = info;
    param.buf = entry_words;
    param.read_cache = NULL;

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {
        SHR_IF_ERR_EXIT
            (cci_ireq_read(unit, handle, info, &param));

        /*
         * Read (PTcache or read_cache)
         * Modify Write PT entry and update cache
         */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, handle,
                                      bcmptm_cci_cache_pt_req_write,
                                      &param, TRUE));
    }

exit:
    if (param.read_cache) {
        SHR_FREE(param.read_cache);
    }
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_config_write
 * Buufer Counter Config Fileds in HW Entry format
 * HW direct write (Counter fields will be Zero)
 * Clear CCI cache and Update PT Cache
 */
int
bcmptm_cci_ctr_config_write(int unit,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Symbol Transform */
    SHR_IF_ERR_EXIT
        (cci_ctr_txfm(unit, handle, info));

    /* Write  PT entry and Clear cache */
    param.info = info;
    param.buf = entry_words;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_hw_sync(unit, handle,
                                 bcmptm_cci_cache_pt_req_passthru_write,
                                 &param, TRUE));
    /* Update PT Cache */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, ACC_FOR_SER, info->sid,
                                   info->in_pt_dyn_info,
                                   entry_words, TRUE,       /* Cache_valid_set */
                                   info->req_ltid, BCMPTM_DFID_IREQ),
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * bcmptm_cci_ctr_passthru_write
 * buffer is HW_entry format, No change in CCI cache, PT Cache Update and HW Write
 */
int
bcmptm_cci_ctr_passthru_write(int unit,
                              bcmptm_cci_ctr_info_t *info,
                              uint32_t *entry_words)
{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    SHR_IF_ERR_EXIT(
        cci_dyn_copy(unit, info));
    /* Write  PT entry */
    param.info = info;
    param.buf = entry_words;

    SHR_IF_ERR_EXIT(
        bcmptm_cci_cache_hw_sync(unit, handle,
                                 bcmptm_cci_cache_pt_req_passthru_write,
                                 &param, FALSE));

    /* Update PT Cache  */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, ACC_FOR_SER, info->sid,
                                   info->in_pt_dyn_info,
                                   entry_words, TRUE,      /* Cache_valid_set */
                                   info->req_ltid, BCMPTM_DFID_IREQ),
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Request to handle SER event on Counter SID
 */
int
bcmptm_cci_ctr_ser_req(int unit,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info)
{
    cci_context_t   *con;
    SHR_FUNC_ENTER(unit);


    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Request to handle SER event on Counter SID array
 */
int
bcmptm_pt_cci_ser_array_req(int unit,
                            bcmptm_cci_ser_req_info_t *ser_req_info,
                            int array_count)
{
    cci_handle_t handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    bcmptm_cci_ctr_info_t info;
    bcmptm_cci_ser_req_info_t *req;
    bool acc_for_ser = TRUE;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    SHR_NULL_CHECK(ser_req_info, SHR_E_PARAM);

    for (i = 0; i < array_count; i++) {
        req = &ser_req_info[i];
        SHR_NULL_CHECK(req, SHR_E_PARAM);

        if (bcmptm_pt_cci_index_valid(unit, req->sid,
                                      req->dyn_info.tbl_inst,
                                      req->dyn_info.index)) {
            sal_memset(&ptcache_ctr_info, 0, sizeof(bcmptm_ptcache_ctr_info_t));
            /* Get cci_map_id */
            SHR_IF_ERR_EXIT(
                bcmptm_ptcache_ctr_info_get(unit, acc_for_ser, req->sid,
                                            &ptcache_ctr_info));

            sal_memset(&info, 0, sizeof(bcmptm_cci_ctr_info_t));
            info.sid = req->sid;
            info.dyn_info.index = req->dyn_info.index;
            info.dyn_info.tbl_inst = req->dyn_info.tbl_inst;
            info.lt_map_id = info.map_id = ptcache_ctr_info.cci_map_id;
            info.txfm_fld = -1;
            SHR_IF_ERR_EXIT(
                bcmptm_cci_cache_hw_sync(unit, handle,
                                         bcmptm_cci_cache_pt_req_ser_clear,
                                         &info, TRUE));
        } else {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Bypass SER req on non counter SID %s\n"),
                      bcmdrd_pt_sid_to_name(unit, req->sid)));
            continue;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static void
bcmptm_cci_port_add_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    cci_handle_t handle, hcol;
    shr_port_t port = (shr_port_t)ev_data;

    hcol = cci_context[unit]->hcol;
    bcmptm_cci_poll_handle_get(unit, hcol, &handle);
    bcmptm_cci_col_poll_port_config(unit, handle, MSG_CMD_POL_PORT_ADD, port);

}

static void
bcmptm_cci_port_del_event_hdl(int unit, const char *event, uint64_t ev_data)
{
    cci_handle_t handle, hcol;
    shr_port_t port = (shr_port_t)ev_data;

    hcol = cci_context[unit]->hcol;
    bcmptm_cci_poll_handle_get(unit, hcol, &handle);
    bcmptm_cci_col_poll_port_config(unit, handle, MSG_CMD_POL_PORT_DEL, port);

}

static int
bcmptm_cci_port_register(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortAdd",
                                         bcmptm_cci_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_register_published_event(unit,
                                         "bcmpcEvPortDelete",
                                         bcmptm_cci_port_del_event_hdl));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmptm_cci_port_unregister(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortAdd",
                                           bcmptm_cci_port_add_event_hdl));
    SHR_IF_ERR_EXIT
        (bcmevm_unregister_published_event(unit,
                                           "bcmpcEvPortDelete",
                                           bcmptm_cci_port_del_event_hdl));

exit:
    SHR_FUNC_EXIT();
}

static bool
cci_is_pipe_enable(int unit,  bcmdrd_sid_t sid, int pipe)
{
    int blktype;
    uint32_t pipe_map;
    int rv;

    rv = bcmdrd_pt_sub_pipe_map(unit, sid, pipe, &pipe_map);
    if (SHR_SUCCESS(rv)) {
        if (pipe_map == 0) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    blktype = bcmdrd_pt_blktype_get(unit, sid, 0);
    (void)bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &pipe_map);

    return IS_PIPE_ENABLE(pipe_map, pipe);
}

/*!
 * Initialize CCI (initialize and allocate resources  etc)
 * Called during System Manager INIT state for each unit
 */
int
bcmptm_cci_init(int unit, bool warm)
{
    cci_context_t   *con = NULL;

    SHR_FUNC_ENTER(unit);
    /* check if already initialized */
    if (cci_context[unit] != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* Allocate the context */
    SHR_ALLOC(con, sizeof(cci_context_t), "bcmptmCciContext");
    SHR_NULL_CHECK(con, SHR_E_MEMORY);
    sal_memset(con, 0, sizeof(cci_context_t));

    /* Store warm boot flag */
    con->warm = warm;

    /* Initialize counter cache */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_init(unit, warm, con, &con->hcache));

    /* Initialize counter  collection.*/
    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_init(unit, con, &con->hcol));

    /* Register port change event callback */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_port_register(unit));

    cci_context[unit] = con;

exit:
    if (SHR_FUNC_ERR()) {
        if (con) {
            cci_cleanup(unit, con);
            cci_context[unit] = NULL;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * Configure CCI
 * Called during System Manager CONFIG state for each unit
 */
int
bcmptm_cci_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    switch(phase) {
    case BCMPTM_SUB_PHASE_1:
        SHR_IF_ERR_EXIT
            (bcmptm_cci_imm_init(unit, con));

        /* Initiate Counter Cache phase 1 configuration */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_config(unit, con->hcache, warm, phase));
        con->init_config = 1;
        con->sub_phase = phase;
    break;

    case BCMPTM_SUB_PHASE_2:
        /* Initiate Counter Cache phase 2 configuration */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_config(unit, con->hcache, warm, phase));

        con->sub_phase = phase;

    break;

    case BCMPTM_SUB_PHASE_3:
        /* Initiate Counter Cache phase 3 configuration */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_ctr_config(unit, con->hcache,
                                         warm, phase));

        if (!warm) {
            if (con->init_config) {
                /*  Insert the configuration data */
                SHR_IF_ERR_EXIT
                    (bcmptm_cci_imm_insert(unit, con));
            }
        }
        con->sub_phase = phase;
    break;
    default:
       LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Unknown Config Phase = %d\n"),
                 phase));
    break;
    }

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Allocate DMA resources, configure Hardware, Start threads
 */
int
bcmptm_cci_run(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_run(unit, con->hcol));

    con->warm = FALSE;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Stop (terminate thread , free resources)
 */
int
bcmptm_cci_stop(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Unregister port change event callback */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_port_unregister(unit));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_stop(unit, con->hcol));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Clean up CCI software resources
 */
int
bcmptm_cci_cleanup(int unit)
{
    cci_context_t   *con;

    SHR_FUNC_ENTER(unit);
    con = cci_context[unit];
    SHR_NULL_CHECK(con, SHR_E_INIT);

    cci_cleanup(unit, con);
    cci_context[unit] = NULL;

exit:
    SHR_FUNC_EXIT();

}
/*!
 * Get CCI cache Handle
 */
int
bcmptm_cci_cache_handle_get(int unit,
                            const cci_context_t *con,
                            cci_handle_t *handle)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(con, SHR_E_PARAM);

    *handle = con->hcache;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Set state of the flex counter pool.
 */
extern int
bcmptm_cci_flex_counter_pool_set_state(int unit,
                                       bcmdrd_sid_t sid,
                                       bcmptm_cci_pool_state_t state)
{
    cci_handle_t   handle;
    bool clr_on_read_enabled;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint32_t map_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    clr_on_read_enabled = bcmptm_cci_ctr_evict_is_clr_on_read(unit, sid);
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));
    map_id = ptcache_ctr_info.cci_map_id;

    if (state == DISABLE) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_ctr_evict_enable(unit, sid, FALSE, clr_on_read_enabled));
        /* Clear of the counters in cache and HW for given Symbol */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, handle,
                                      bcmptm_cci_cache_pt_req_all_clear,
                                      &sid, TRUE));
    } else if (state == ENABLE) {
        SHR_IF_ERR_EXIT
            (bcmptm_cci_ctr_evict_enable(unit, sid, TRUE, clr_on_read_enabled));
    }
    /* In case of state set to be SHADOW, counter collection is not needed */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_flex_counter_pool_set_state(unit, handle, map_id, state));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get state of the flex counter pool.
 */
extern int
bcmptm_cci_flex_counter_pool_get_state(int unit,
                                       bcmdrd_sid_t sid,
                                       bcmptm_cci_pool_state_t *state)
{

    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;
    uint32_t map_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));
    map_id = ptcache_ctr_info.cci_map_id;
    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_flex_counter_pool_get_state(unit, handle, map_id, state));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * Eviction counter control set.
 * \retval SHR_E_NONE No errors
 */
extern int
bcmptm_cci_evict_control_set(int unit,
                             bcmdrd_sid_t sid,
                             bool clear_on_read,
                             bcmptm_cci_ctr_evict_mode_t mode)
{
    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);

    /*
     * In sim model, eviction memories might not be cleared on read
     * per device model capability.
     */
    if (bcmptm_pt_cci_evict_cor_supported(unit, sid) == FALSE) {
        clear_on_read = FALSE;
    }

    /* Get cci_map_id */
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_evict_control_set(unit, handle,
                                            ptcache_ctr_info.cci_map_id,
                                            mode));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_hw_sync(unit, handle,
                                  bcmptm_cci_cache_pt_req_clear_on_read,
                                  &sid, clear_on_read));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Set counter mode.
 */
extern int
bcmptm_cci_flex_counter_mode_set(int unit,
                                 bcmdrd_sid_t sid,
                                 uint32_t index_min,
                                 uint32_t index_max,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t size)
{
    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    /* Get cci_map_id */
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_counter_mode_set(unit, handle,
                                           ptcache_ctr_info.cci_map_id,
                                           index_min, index_max, tbl_inst,
                                           ctr_mode, update_mode, size));

exit:
    SHR_FUNC_EXIT();

}

/*
 * Get counter mode.
 */
int
bcmptm_cci_flex_counter_mode_get(int unit,
                                 bcmdrd_sid_t sid,
                                 uint32_t index,
                                 int tbl_inst,
                                 bcmptm_cci_ctr_mode_t *ctr_mode,
                                 bcmptm_cci_update_mode_t *update_mode,
                                 size_t *size)
{
    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_PARAM);
    /* Get cci_map_id */
    SHR_IF_ERR_EXIT
        (bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, sid,
                                     &ptcache_ctr_info));

    SHR_IF_ERR_EXIT
        (bcmptm_cci_cache_counter_mode_get(unit, handle,
                                           ptcache_ctr_info.cci_map_id,
                                           index, tbl_inst,
                                           ctr_mode, update_mode, size));

exit:
    SHR_FUNC_EXIT();



}

bool
bcmptm_cci_port_collect_enabled(int unit, int port)
{
    cci_config_t config;

    bcmptm_cci_config_get(unit, &config);
     if (config.col_enable) {
         if (!BCMDRD_PBMP_MEMBER(config.pbmp, port)) {
             return FALSE;
         } else {
             if (config.multiplier[CCI_CTR_TYPE_PORT] != 0) {
                 return TRUE;
             } else {
                 return FALSE;
             }
         }
     } else {
         return FALSE;
     }
}

bool
bcmptm_cci_evict_collect_enabled(int unit)
{
    cci_config_t config;

    bcmptm_cci_config_get(unit, &config);
     if (config.col_enable) {
         if (config.multiplier[CCI_CTR_TYPE_EVICT] != 0) {
             return TRUE;
         } else {
             return FALSE;
         }
     } else {
         return FALSE;
     }
}

static int
cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid,
                     bool enable, bool clr_on_read)
{
    bcmptm_cci_ctr_info_t info;
    uint32_t entry[2];
    uint32_t val;
    int pipe, tbl_inst_num;
    bcmdrd_fid_t fid_en;
    bcmdrd_fid_t fid_clr;

    SHR_FUNC_ENTER(unit);
    sal_memset(&info, 0, sizeof(info));
    info.sid = sid;

    SHR_IF_ERR_EXIT(bcmptm_pt_cci_ctr_evict_enable_field_get(unit, &fid_en));
    SHR_IF_ERR_EXIT(bcmptm_pt_cci_ctr_evict_clr_on_read_field_get(unit, &fid_clr));

    tbl_inst_num = bcmdrd_pt_num_tbl_inst(unit, sid);
    for (pipe = 0; pipe < tbl_inst_num; pipe++) {
        if (cci_is_pipe_enable(unit, sid, pipe)) {
            uint32_t rsp_flags;
            info.dyn_info.index = 0;
            info.dyn_info.tbl_inst = pipe;
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_read_cci(unit, ACC_FOR_SER, info.flags, info.sid,
                                      &info.dyn_info, info.in_pt_ovrr_info,
                                      2, info.req_ltid, entry,
                                      &info.req_ltid, &rsp_flags));
            val = (enable == TRUE) ? 1 : 0;
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry, fid_en,
                                     &val));
            val = (clr_on_read == TRUE) ? 1 : 0;
            SHR_IF_ERR_EXIT
                (bcmdrd_pt_field_set(unit, sid, entry, fid_clr,
                                     &val));
            SHR_IF_ERR_EXIT
                (bcmptm_ireq_write_cci(unit, ACC_FOR_SER, info.flags, info.sid,
                                       &info.dyn_info, info.in_pt_ovrr_info,
                                       entry, info.req_ltid, &info.req_ltid,
                                       &rsp_flags));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_cci_ctr_evict_enable(int unit, bcmdrd_sid_t sid,
                            bool enable, bool clr_on_read)
{
    bcmdrd_sid_t ctrl_sid;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_control_get(unit, sid, &ctrl_sid));
    SHR_IF_ERR_EXIT
        (cci_ctr_evict_enable(unit, ctrl_sid, enable, clr_on_read));
exit:
    SHR_FUNC_EXIT();
}

static int
cci_ctr_evict_field_get(int unit, bcmdrd_sid_t sid,
                        bcmdrd_fid_t fid, uint32_t *val)
{
    bcmptm_cci_ctr_info_t info;
    uint32_t entry[2];
    uint32_t fld_val[2];
    bcmdrd_sid_t ctrl_sid;

    SHR_FUNC_ENTER(unit);
    sal_memset(&info, 0, sizeof(info));

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_control_get(unit, sid, &ctrl_sid));
    info.sid = ctrl_sid;
    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read_cci(unit, ACC_FOR_SER, info.flags, info.sid,
                              &info.dyn_info, info.in_pt_ovrr_info,
                              2, info.req_ltid, entry,
                              &info.req_ltid, info.rsp_flags));
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_get(unit, info.sid, entry, fid, fld_val));
    *val = fld_val[0];

exit:
    SHR_FUNC_EXIT();
}

bool
bcmptm_cci_ctr_evict_is_enable(int unit, bcmdrd_sid_t sid)
{
    uint32_t val = 0;
    bool ret = false;
    bcmdrd_fid_t fid;

    if (SHR_SUCCESS(bcmptm_pt_cci_ctr_evict_enable_field_get(unit, &fid))) {
        if (SHR_SUCCESS(cci_ctr_evict_field_get(unit, sid, fid, &val))) {
            if (val) {
                ret = true;
            }
        }
    }

    return ret;
}

bool
bcmptm_cci_ctr_evict_is_clr_on_read(int unit, bcmdrd_sid_t sid)
{
    uint32_t val = 0;
    bool ret = false;
    bcmdrd_fid_t fid;

    if (SHR_SUCCESS(bcmptm_pt_cci_ctr_evict_clr_on_read_field_get(unit, &fid))) {
        if (SHR_SUCCESS(cci_ctr_evict_field_get(unit, sid, fid, &val))) {
            if (val) {
                ret = true;
            }
        }
    }

    return ret;
}

int
bcmptm_cci_evict_fifo_enable(int unit, bool enable)
{
    bcmdrd_sid_t    sid;
    bcmdrd_fid_t    fid;
    uint32_t        *entry_buf = NULL;
    uint32_t        entry_sz, value;
    bcmbd_pt_dyn_info_t pt_dyn_info;
    bcmltd_sid_t    lt_id = -1;
    bcmltd_sid_t    rsp_ltid;
    uint32_t        rsp_flags;
    uint32_t        wsize;
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmptm_pt_cci_ctr_evict_fifo_enable_sym_get(unit, &sid, &fid);
    /* Return without error in case eviction is not supported */
    if (rv == SHR_E_UNAVAIL) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    entry_sz = bcmdrd_pt_entry_wsize(unit, sid);
    wsize = entry_sz;
    entry_sz *= 4;
    SHR_ALLOC(entry_buf, entry_sz, "bcmptmCciEvictFifo");
    SHR_NULL_CHECK(entry_buf, SHR_E_MEMORY);

    sal_memset(&pt_dyn_info, 0, sizeof(bcmbd_pt_dyn_info_t));

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_read_cci(unit, ACC_FOR_SER, 0, sid,
                              &pt_dyn_info, NULL,
                              wsize, lt_id, entry_buf,
                              &rsp_ltid, &rsp_flags));
    value = (enable == TRUE) ? 1 : 0;
    SHR_IF_ERR_EXIT
        (bcmdrd_pt_field_set(unit, sid, entry_buf, fid, &value));

    SHR_IF_ERR_EXIT
        (bcmptm_ireq_write_cci(unit, ACC_FOR_SER, 0, sid,
                               &pt_dyn_info, NULL,
                               entry_buf, lt_id,
                               &rsp_ltid, &rsp_flags));
exit:
    if (entry_buf) {
        SHR_FREE(entry_buf);
    }
    SHR_FUNC_EXIT();
}

/*
 * Set Flex counter hit bit.
 */

int
bcmptm_cci_hit_bit_set(int unit,
                       bcmdrd_sid_t sid,
                       uint32_t pipe,
                       uint32_t index,
                       uint32_t bit)
{
    cci_context_t   *con = (cci_context_t *)cci_context[unit];
    col_thread_msg_t msg;
    cci_handle_t hpol;
    size_t width;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(con, SHR_E_INIT);

    /* Get polling collection context */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));

    /* Check sid, pipe, index */
    if (!bcmptm_pt_cci_index_valid(unit, sid, (int)pipe, index)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmptm_pt_cci_ctr_evict_field_width_get(unit,
                                                 WIDE_MODE, &width));
    /* Check bit position */
    if (bit > width - 1 ) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }


    msg.cmd = MSG_CMD_HIT_BIT_SET;
    msg.data[0] = sid;
    msg.data[1] = pipe;
    msg.data[2] = index;
    msg.data[3] = bit;

    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_poll_msg(unit, &msg, hpol));

exit:
    SHR_FUNC_EXIT();
}

/*
 * Read Slim Counter.
 */
int
bcmptm_cci_slim_ctr_read(int unit,
                         bcmptm_cci_ctr_info_t *info,
                         uint32_t *value,
                         size_t wsize)
{

    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {

        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, info->sid,
                                    &ptcache_ctr_info));
        info->map_id = ptcache_ctr_info.cci_map_id;
        /* First Sync CCI Cache with HW */
        if (info->flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
            bcmptm_cci_cache_pt_param_t param;
            /* Read PT entry and Update cache */
            param.info = info;
            param.buf = value;
            param.size = wsize;

            SHR_IF_ERR_EXIT
                (cci_hw_sync_req_read(unit, handle, info, &param));
            sal_memset(value, 0, sizeof(uint32_t) * wsize);

        }

        /* Read Counter values from cache */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_slim_cache_read(unit, handle, info, value, wsize));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Write Slim Counter.
 */
int
bcmptm_cci_slim_ctr_write(int unit,
                          bcmptm_cci_ctr_info_t *info,
                          uint32_t *value)

{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param = {0};
    bcmltd_sid_t    rsp_ltid;
    uint32_t        rsp_flags;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));

    info->rsp_ltid = &rsp_ltid;
    info->rsp_flags = &rsp_flags;
    param.info = info;
    param.buf = value;
    param.read_cache = NULL;

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {
        SHR_IF_ERR_EXIT
            (cci_ireq_read(unit, handle, info, &param));

        /*
         * Modify Write PT entry and update cache
         */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, handle,
                                      bcmptm_cci_slim_cache_pt_req_write,
                                      &param, TRUE));
    }

exit:
    if (param.read_cache) {
        SHR_FREE(param.read_cache);
    }
    SHR_FUNC_EXIT();
}

/*
 * Read Normal double Counter.
 */
int
bcmptm_cci_normal_double_ctr_read(int unit,
                         bcmptm_cci_ctr_info_t *info,
                         uint32_t *value,
                         size_t wsize)
{

    cci_handle_t   handle;
    bcmptm_ptcache_ctr_info_t ptcache_ctr_info;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {

        SHR_IF_ERR_EXIT(
            bcmptm_ptcache_ctr_info_get(unit, ACC_FOR_SER, info->sid,
                                    &ptcache_ctr_info));
        info->map_id = ptcache_ctr_info.cci_map_id;
        /* First Sync CCI Cache with HW */
        if (info->flags & BCMLT_ENT_ATTR_GET_FROM_HW) {
            bcmptm_cci_cache_pt_param_t param;
            /* Read PT entry and Update cache */
            param.info = info;
            param.buf = value;
            param.size = wsize;

            SHR_IF_ERR_EXIT
                (cci_hw_sync_req_read(unit, handle, info, &param));
            sal_memset(value, 0, sizeof(uint32_t) * wsize);

        }

        /* Read Counter values from cache */
        SHR_IF_ERR_EXIT(
            bcmptm_cci_normal_double_cache_read(unit, handle, info, value, wsize));
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Write Normal double Counter.
 */
int
bcmptm_cci_normal_double_ctr_write(int unit,
                          bcmptm_cci_ctr_info_t *info,
                          uint32_t *value)

{
    cci_handle_t   handle;
    bcmptm_cci_cache_pt_param_t param = {0};
    bcmltd_sid_t    rsp_ltid;
    uint32_t        rsp_flags;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(cci_context[unit], SHR_E_INIT);
    handle = cci_context[unit]->hcache;
    SHR_NULL_CHECK(handle, SHR_E_INIT);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (cci_dyn_copy(unit, info));

    info->rsp_ltid = &rsp_ltid;
    info->rsp_flags = &rsp_flags;
    param.info = info;
    param.buf = value;
    param.read_cache = NULL;

    if (bcmptm_pt_cci_index_valid(unit, info->sid,
                                  info->dyn_info.tbl_inst,
                                  info->dyn_info.index)) {
        SHR_IF_ERR_EXIT
            (cci_ireq_read(unit, handle, info, &param));

        /*
         * Modify Write PT entry and update cache
         */
        SHR_IF_ERR_EXIT
            (bcmptm_cci_cache_hw_sync(unit, handle,
                                      bcmptm_cci_normal_double_cache_pt_req_write,
                                      &param, TRUE));
    }

exit:
    if (param.read_cache) {
        SHR_FREE(param.read_cache);
    }
    SHR_FUNC_EXIT();
}

/*!
 * Move counter table entry.
 */
int
bcmptm_cci_ctr_flex_entry_move(int unit,
                               bcmptm_cci_ctr_info_t *src_info,
                               bcmptm_cci_ctr_info_t *dst_info)
{
    cci_context_t   *con = (cci_context_t *)cci_context[unit];
    col_thread_msg_t msg;
    cci_handle_t hpol;
    bcmptm_cci_ctr_info_t *msg_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(con, SHR_E_INIT);
    SHR_NULL_CHECK(src_info, SHR_E_PARAM);
    SHR_NULL_CHECK(dst_info, SHR_E_PARAM);

    /* Get polling collection context. */
    SHR_IF_ERR_EXIT
        (bcmptm_cci_poll_handle_get(unit, con->hcol, &hpol));

    SHR_IF_ERR_EXIT (cci_dyn_copy(unit, src_info));
    SHR_IF_ERR_EXIT (cci_dyn_copy(unit, dst_info));

    /* Send entry move message. */
    SHR_ALLOC(msg_info, sizeof(bcmptm_cci_ctr_info_t) * 2, "bcmPtmCciEntryMove");
    SHR_NULL_CHECK(msg_info, SHR_E_MEMORY);
    msg.cmd = MSG_CMD_CTR_ENTRY_MOVE;
    msg_info[0] = *src_info;
    msg_info[1] = *dst_info;
    sal_memcpy(msg.data, &msg_info, sizeof(bcmptm_cci_ctr_info_t *));
    SHR_IF_ERR_EXIT
        (bcmptm_cci_col_poll_msg(unit, &msg, hpol));

exit:
    if (SHR_FUNC_ERR()) {
        if (msg_info != NULL) {
            SHR_FREE(msg_info);
        }
    }
    SHR_FUNC_EXIT();
}
