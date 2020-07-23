/*! \file bcma_testutil_drv.c
 *
 * Chip-specific driver utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_pt.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sbusdma.h>

#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_pt_common.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

typedef int (*bcma_testutil_drv_attach_f)(int unit);

#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bd##_testutil_drv_attach },
static struct {
    bcma_testutil_drv_attach_f attach;
} testutil_attach[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

static bcma_testutil_drv_t testutil_drv[BCMDRD_CONFIG_MAX_UNITS];

bcma_testutil_drv_t*
bcma_testutil_drv_get(int unit)
{
    if (!bcmdrd_dev_exists(unit)) {
        return NULL;
    }

    return &testutil_drv[unit];
}

int
bcma_testutil_drv_attach(int unit)
{
    bcmdrd_dev_type_t dev_type;
    bcma_testutil_drv_t *drv;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    drv = bcma_testutil_drv_get(unit);
    if (drv == NULL) {
        return SHR_E_UNAVAIL;
    }

    if (drv->dev_type == dev_type) {
        return SHR_E_NONE;
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "New test driver attached\n")));
    drv->dev_type = dev_type;

    return testutil_attach[dev_type].attach(unit);
}

bool
bcma_testutil_drv_pt_skip(int unit, bcmdrd_sid_t sid,
                          bcma_testutil_pt_type_t type)
{
    bcma_testutil_drv_t *drv;

    drv = bcma_testutil_drv_get(unit);
    if (!drv || !drv->pt_skip) {
        return TRUE;
    }

    /*
     * The return value is TRUE/FALSE, it is used to indicate
     * if we need to skip the PT
     */
    return drv->pt_skip(unit, sid, type);
}

int
bcma_testutil_cond_init(int unit, bcmdrd_sid_t sid,
                        bcma_testutil_pt_type_t type)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);

    if (drv->cond_init != NULL) {
        SHR_IF_ERR_EXIT
            (drv->cond_init(unit, sid, type));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_cond_cleanup(int unit, bcmdrd_sid_t sid,
                           bcma_testutil_pt_type_t type)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);

    if (drv->cond_cleanup != NULL) {
        SHR_IF_ERR_EXIT
            (drv->cond_cleanup(unit, sid, type));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_pt_mask_override(int unit, bcmdrd_sid_t sid, int index,
                                   uint32_t *mask, int wsize)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->pt_mask_override) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->pt_mask_override(unit, sid, index, mask, wsize));

exit:
    SHR_FUNC_EXIT();
}

void
bcma_testutil_pkt_cfg_data_get(int unit,
                               const bcma_testutil_pkt_lt_cfg_t **pkt_lt_cfg,
                               int *lt_cfg_num,
                               const bcma_testutil_pkt_data_t **pkt_data)
{
    bcma_testutil_drv_t *drv;

    drv = bcma_testutil_drv_get(unit);
    if ((drv != NULL) && (drv->pkt_cfg_data_get != NULL)) {
        drv->pkt_cfg_data_get(unit, pkt_lt_cfg, lt_cfg_num, pkt_data);
    } else {
        *pkt_lt_cfg = NULL;
        *lt_cfg_num = 0;
        *pkt_data = NULL;
    }
}

int
bcma_testutil_drv_pt_hw_update(int unit, const char *name,
                               bcma_testutil_pt_type_t type, bool enable)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(name, SHR_E_PARAM);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->pt_hw_update) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->pt_hw_update(unit, name, type, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_cambist_info(int unit, bcmdrd_sid_t sid, bool *is_bist,
                                   bcma_testutil_mem_cambist_entry_t *info)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_cambist_info) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_cambist_info(unit, sid, is_bist, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_address_adjust(int unit, bcmdrd_sid_t sid, int *index_start, int *index_end,
             bcma_testutil_pt_type_t type)
{
    bcma_testutil_drv_t *drv;
    bcmdrd_sym_info_t sinfo;
    int index_min, index_max;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_address_adjust) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (bcmdrd_pt_info_get(unit, sid, &sinfo));

    index_min = sinfo.index_min;
    index_max = sinfo.index_max;

    drv->mem_address_adjust(unit, sid, &index_min, &index_max, type);

    if (*index_start == BCMA_TESTUTIL_TABLE_MIN_INDEX) {
        *index_start = index_min;
    } else if (*index_start == BCMA_TESTUTIL_TABLE_MAX_INDEX) {
        *index_start = index_max;
    } else if (*index_start == BCMA_TESTUTIL_TABLE_MID_INDEX) {
        *index_start = (index_max - index_min) / 2;
    }

    if (*index_end == BCMA_TESTUTIL_TABLE_MIN_INDEX) {
        *index_end = index_min;
    } else if (*index_end == BCMA_TESTUTIL_TABLE_MAX_INDEX) {
        *index_end = index_max;
    } else if (*index_end == BCMA_TESTUTIL_TABLE_MID_INDEX) {
        *index_end = (index_max - index_min) / 2;
    }

    if (*index_start < index_min) {
        *index_start = index_min;
    }

    if (*index_end > index_max) {
        *index_end = index_max;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_hash_data_adjust(int unit, bcmdrd_sid_t sid,
                                       uint32_t *data, int wsize)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_hash_data_adjust) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    if (bcmdrd_pt_is_valid(unit, sid) == false) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if ((uint32_t)wsize < bcmdrd_pt_entry_wsize(unit, sid)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_hash_data_adjust(unit, sid, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_range_write(int unit, bcmdrd_sid_t sid, int index_start,
                                  int index_end, int tbl_inst, uint64_t buf)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_range_write) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_range_write(unit, sid, index_start, index_end, tbl_inst, buf));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_range_read(int unit, bcmdrd_sid_t sid, int index_start,
                                 int index_end, int tbl_inst, uint64_t buf)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_range_read) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_range_read(unit, sid, index_start, index_end, tbl_inst, buf));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_mbist_preconfig(int unit)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_mbist_preconfig) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_mbist_preconfig(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_mbist_info(
    int unit,
    const bcma_testutil_mem_mbist_entry_t **test_array,
    int *test_count)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_mbist_info) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_mbist_info(unit, test_array, test_count));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_mem_mbist_dma_info(
    int unit,
    const bcma_testutil_mem_mbist_dma_entry_t **test_array,
    int *test_count)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->mem_mbist_dma_info) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->mem_mbist_dma_info(unit, test_array, test_count));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_cmc_chan_get(int unit, int *cmc_num, int *cmc_chan_num)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->cmc_chan_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->cmc_chan_get(unit, cmc_num, cmc_chan_num));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_traffic_default_pktcnt_get(int unit, uint32_t *ret_pktcnt)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    SHR_NULL_CHECK(ret_pktcnt, SHR_E_PARAM);

    *ret_pktcnt = drv->traffic_default_pktcnt;

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_traffic_expeceted_rate_get(
    int unit, bcmdrd_pbmp_t pbmp, bcmdrd_pbmp_t pbmp_oversub, int pkt_size,
    int array_size, uint64_t *ret_exp_rate)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->traffic_expeceted_rate_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->traffic_expeceted_rate_get(unit, pbmp, pbmp_oversub, pkt_size,
                                         array_size, ret_exp_rate));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_traffic_fixed_packet_list_get(
    int unit, uint32_t pkt_len, uint32_t *ret_pkt_cnt)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->traffic_fixed_packet_list_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->traffic_fixed_packet_list_get(unit, pkt_len, ret_pkt_cnt));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_traffic_random_packet_list_get(
    int unit, uint32_t cellnums_limit,
    uint32_t array_size, uint32_t *ret_pkt_size, uint32_t *ret_pkt_cnt)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->traffic_random_packet_list_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->traffic_random_packet_list_get(unit, cellnums_limit, array_size,
                                             ret_pkt_size, ret_pkt_cnt));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_traffic_worstcase_pktlen_get(
    int unit, int port, uint32_t *ret_pkt_size)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->traffic_worstcase_pktlen_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->traffic_worstcase_pktlen_get(unit, port, ret_pkt_size));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_chip_blktype_get(int unit, const char *blktype_name,
                                   int *blktype_id)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(blktype_name, SHR_E_PARAM);
    SHR_NULL_CHECK(blktype_id, SHR_E_PARAM);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->chip_blktype_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->chip_blktype_get(unit, blktype_name, blktype_id));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_benchmark_reglist_get(int unit, const bcmdrd_sid_t **reg_arr,
                                        int *reg_cnt)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(reg_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(reg_cnt, SHR_E_PARAM);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->benchmark_reglist_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->benchmark_reglist_get(unit, reg_arr, reg_cnt));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_benchmark_memlist_get(int unit, const bcmdrd_sid_t **mem_arr,
                                        int *mem_cnt)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mem_arr, SHR_E_PARAM);
    SHR_NULL_CHECK(mem_cnt, SHR_E_PARAM);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->benchmark_memlist_get) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->benchmark_memlist_get(unit, mem_arr, mem_cnt));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_fifodma_instance_setup(int unit)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->fifodma_instance_setup) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->fifodma_instance_setup(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_fifodma_instance_trigger(int unit)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->fifodma_instance_trigger) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->fifodma_instance_trigger(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_feature_get(int unit, bcma_testutil_ft_t feature,
                              bool *enabled)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(enabled, SHR_E_PARAM);

    if (feature < 0 || feature >= BCMA_TESTUTIL_FT_COUNT) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    drv = bcma_testutil_drv_get(unit);
    SHR_NULL_CHECK(drv, SHR_E_UNIT);

    if (SHR_BITGET(drv->features, feature)) {
        *enabled = true;
    } else {
        *enabled = false;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_check_health(int unit, bool *test_result)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->check_health) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->check_health(unit, test_result));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_pt_write(int unit,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           void *ovrr_info,
                           uint32_t *data)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->pt_write) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->pt_write(unit, sid, dyn_info, ovrr_info, data));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_drv_pt_read(int unit,
                          bcmdrd_sid_t sid,
                          bcmbd_pt_dyn_info_t *dyn_info,
                          void *ovrr_info,
                          uint32_t *data,
                          size_t wsize)
{
    bcma_testutil_drv_t *drv;

    SHR_FUNC_ENTER(unit);

    drv = bcma_testutil_drv_get(unit);

    SHR_NULL_CHECK(drv, SHR_E_UNIT);
    if (!drv->pt_read) {
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT
        (drv->pt_read(unit, sid, dyn_info, ovrr_info, data, wsize));

exit:
    SHR_FUNC_EXIT();
}

const uint32_t *
bcma_testutil_drv_pt_default_value_get(int unit, bcmdrd_sid_t sid)
{
    bcma_testutil_drv_t *drv;

    drv = bcma_testutil_drv_get(unit);
    if (!drv || !drv->pt_default_value_get) {
        return NULL;
    }

    return drv->pt_default_value_get(unit, sid);
}

