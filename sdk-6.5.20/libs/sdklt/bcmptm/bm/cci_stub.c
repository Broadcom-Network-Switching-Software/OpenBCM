/*! \file cci_stub.c
 *
 * Stub implementation of Interface functions for CCI.
 *
 * This file contains the stub of interface func for CCI for behavioral model.
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
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_ptcache_internal.h>
#include <bcmptm/bcmptm_cci.h>
#include <bcmptm/bcmptm_cci_internal.h>


/*!
 * BSL_LOG_MODULE used for SHR_FUNC_ENTER, EXIT
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_CCI

#define ACC_FOR_SER FALSE

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
 * Enable or disable flex counter pool
 */
int
bcmptm_cci_flex_counter_pool_enable(int unit,
                                        bcmdrd_sid_t sid,
                                        bool enable)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Set counter mode.
 */
int bcmptm_cci_flex_counter_mode_set(int unit,
                                     bcmdrd_sid_t sid,
                                     uint32_t index_min,
                                     uint32_t index_max,
                                     int tbl_inst,
                                     bcmptm_cci_ctr_mode_t ctr_mode,
                                     bcmptm_cci_update_mode_t *update_mode,
                                     size_t size)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Copy field  bits between two word arrays.
 */
void
bcmptm_cci_buff32_field_copy(uint32_t *d, int offd, const uint32_t *s,
                             int offs, int n)
{
    return;
}

/*!
 * Check collection on port counters is enabled.
 */
bool
bcmptm_cci_port_collect_enabled(int unit, int port)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Check collection on evict counters is enabled.
 */
bool
bcmptm_cci_evict_collect_enabled(int unit)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Eviction counter control set.
 */
int
bcmptm_cci_evict_control_set(int unit,
                             bcmdrd_sid_t sid,
                             bool clr_on_read,
                             bcmptm_cci_ctr_evict_mode_t mode)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Initialize CCI (initialize and allocate resources  etc)
 */
int
bcmptm_cci_init(int unit,
                    bool warm)
{
    return SHR_E_NONE;
}

/*!
 * Register counter with CCI.
 */
int
bcmptm_cci_ctr_reg(int unit,
                    bcmdrd_sid_t sid,
                    uint32_t *map_id)
{
    return SHR_E_NONE;
}


/*!
 * Configure CCI
 */
int
bcmptm_cci_comp_config(int unit,
                       bool warm,
                       bcmptm_sub_phase_t phase)
{
    return SHR_E_NONE;
}


/*!
 * Allocate DMA resources, configure Hardware, Start threads.
 */
int
bcmptm_cci_run(int unit)
{
    return SHR_E_NONE;
}

/*!
 * Read Dynamic fileds of Counter SID.
 */
int
bcmptm_cci_ctr_read(int unit,
                    bcmptm_cci_ctr_info_t *info,
                    uint32_t *rsp_entry_words,
                    size_t   rsp_entry_size)
{
    return SHR_E_NONE;
}

/*!
 * Read Configuration fields Counter HW Entry.
 */
int
bcmptm_cci_ctr_config_read(int unit,
                           bcmptm_cci_ctr_info_t *info,
                           uint32_t *rsp_entry_words,
                           size_t   rsp_entry_size)
{
    return SHR_E_NONE;
}

/*!
 * Read Counter HW Entry in pass through mode.
 */
int
bcmptm_cci_ctr_passthru_read(int unit,
                             bcmptm_cci_ctr_info_t *info,
                             uint32_t *rsp_entry_words,
                             size_t   rsp_entry_size)
{

    SHR_FUNC_ENTER(unit);
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

/*!
 * Write Counter value.
 */
int
bcmptm_cci_ctr_write(int unit,
                     bcmptm_cci_ctr_info_t *info,
                     uint32_t *entry_words)
{
    return SHR_E_NONE;
}

/*!
 * Write Configuration fields Counter HW Entry.
 */
int
bcmptm_cci_ctr_config_write(int unit,
                            bcmptm_cci_ctr_info_t *info,
                            uint32_t *entry_words)
{
    return SHR_E_NONE;
}

/*!
 * Counter HW Entry in pass through mode.
 */
int
bcmptm_cci_ctr_passthru_write(int unit,
                              bcmptm_cci_ctr_info_t *info,
                              uint32_t *entry_words)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(entry_words, SHR_E_PARAM);

    /* Update PT Cache  */
    SHR_IF_ERR_EXIT_EXCEPT_IF(
        bcmptm_ptcache_update_ireq(unit, ACC_FOR_SER, info->sid,
                                   info->in_pt_dyn_info, info->in_pt_ovrr_info,
                                   entry_words, TRUE,      /* Cache_valid_set */
                                   info->req_ltid, BCMPTM_DFID_IREQ),
        SHR_E_UNAVAIL); /* ok, if cache is not alloc for this sid */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Request to handle SER event on Counter SID.
 */
int
bcmptm_cci_ctr_ser_req(int unit,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info)
{
    return SHR_E_NONE;
}

/*!
 * Request to handle SER event on Counter SIDs to clear counters.
 */
int
bcmptm_pt_cci_ser_array_req(int unit,
                            bcmptm_cci_ser_req_info_t *ser_req_info,
                            int array_count)
{
    return SHR_E_UNAVAIL;
}

/*!
 * Stop (terminate thread , free resources).
 */
int
bcmptm_cci_stop(int unit)
{
    return SHR_E_NONE;
}

/*!
 * De-Initialize CCI software resources.
 */
int
bcmptm_cci_cleanup(int unit)
{
    return SHR_E_NONE;
}
