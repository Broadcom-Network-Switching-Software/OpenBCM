/*! \file bcma_testcase_ser_buffer_bus.c
 *
 *  Test case for SER internal buffers and buses.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/cli/bcma_cli.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_net.h>
#include <bcmpkt/bcmpkt_knet.h>
#include <bcmpkt/bcmpkt_buf.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/bcma_testcase.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <sal/sal_sleep.h>
#include <bcma/test/testcase/bcma_testcase_ser_buffer_bus.h>
#include <bcma/test/util/bcma_testutil_ser.h>
#include <bcmdrd/bcmdrd_pt_ser.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmptm/bcmptm_ser_testutil.h>
#include <bcmptm/bcmptm_ser_chip_internal.h>
#include <bcmmgmt/bcmmgmt.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

/*! Default netif MAC address. */
#define BCMA_BCMPKT_DEF_NETIF_MAC {0x00, 0xbc, 0x00, 0x00, 0x00, 0x00}

typedef struct bcma_testcase_ser_tr_stats_s {
    /* Number of bus/buffer tested by TR 143 */
    uint32_t pt_num_test;
    /* Number of bus/buffer which do not pass TR 143 */
    uint32_t pt_num_fail;
    /* Number of bus/buffer which need to be skipped by TR 143 */
    uint32_t pt_num_skip;
} bcma_testcase_ser_tr_stats_t;

static bcma_testcase_ser_tr_stats_t tr143_stats[BCMDRD_CONFIG_MAX_UNITS];

static int
bcma_testcase_ser_bb_select(int unit)
{
    return bcma_testutil_ser_enabled(unit);
}

static int
bcma_testcase_ser_bb_param_parse(int unit, bcma_cli_t *cli, bcma_cli_args_t *a, uint32_t flags, void **bp)
{
    return SHR_E_NONE;
}

static void
bcma_testcase_ser_bb_help(int unit, void *bp)
{
    cli_out("TR 143\n");

    return;
}

static int
bcma_testcase_ser_buf_bus_enable(int unit, bcmdrd_ser_rmr_id_t rid,
                                 bcmdrd_ser_en_type_t ctrl_type, int enable)
{
    int rv = SHR_E_NONE;
    bcmdrd_pt_ser_en_ctrl_t ctrl_info;
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE] = {0}, fval;

    SHR_FUNC_ENTER(unit);

    sal_memset(&ctrl_info, 0, sizeof(ctrl_info));

    rv = bcmdrd_pt_ser_rmr_en_ctrl_get(unit, ctrl_type, rid, &ctrl_info);
    if (SHR_FAILURE(rv)) {
         cli_out("Fail to get buffer or bus [%s][%d] controlling information.\n",
                 bcmdrd_pt_ser_rmr_to_name(unit, rid), rid);
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }
     if (!bcmdrd_pt_is_valid(unit, ctrl_info.sid)) {
         cli_out("Controlling SID [%d] is invalid.\n", ctrl_info.sid);
         SHR_ERR_EXIT(SHR_E_FAIL);
     }
     rv = bcmptm_ser_testutil_pt_read(unit, ctrl_info.sid, 0, -1, -1,
                                      entry_data, BCMDRD_MAX_PT_WSIZE, 1);
     if (SHR_FAILURE(rv)) {
         cli_out("Fail to read controlling register.\n");
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }

     if (enable) {
         rv = bcmptm_ser_force_error_value_get(unit, ctrl_info.sid,
                                               ctrl_info.fid, &fval);
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     } else {
         fval = 0;
     }

     if (bcmdrd_pt_field_set(unit, ctrl_info.sid, entry_data, ctrl_info.fid,
                             &fval) < 0) {
         cli_out("Fail to configure field of controlling register.\n");
         SHR_ERR_EXIT(SHR_E_FAIL);
     }

     rv = bcmptm_ser_testutil_pt_hw_write(unit, ctrl_info.sid, 0, -1, -1,
                                          entry_data, 1);
     if (SHR_FAILURE(rv)) {
         cli_out("Fail to write controlling register.\n");
         SHR_IF_ERR_VERBOSE_EXIT(rv);
     }

exit:
    SHR_FUNC_EXIT();
}


static int
bcma_testcase_ser_pkt_send(int unit)
{
    int rv = SHR_E_NONE;
    bcmpkt_packet_t *packet = NULL;
    const bcma_testutil_pkt_lt_cfg_t *lt_cfg = NULL;
    int lt_cfg_num;
    int port_num = 0, i = 0;
    int count = 2;
    const bcma_testutil_pkt_data_t *pkt_data = NULL;

    bcma_testutil_pkt_cfg_data_get(unit, &lt_cfg, &lt_cfg_num, &pkt_data);
    if (pkt_data == NULL) {
        cli_out("No lt configuration and packet data.\n");
        return SHR_E_INTERNAL;
    }
    port_num = pkt_data->txpmd_len / sizeof(pkt_data->txpmd[0]);
    while (count--) {
        for (i = 0; i < port_num; i++) {
            rv = bcmpkt_alloc(BCMPKT_BPOOL_SHARED_ID, pkt_data->raw_data_byte_len,
                              BCMPKT_BUF_F_TX, &packet);
            if (SHR_FAILURE(rv) || packet == NULL) {
                cli_out("Allocate packet failed (%d)\n", rv);
                return rv;
            }

            if (pkt_data->higig) {
                sal_memcpy(packet->pmd.higig, pkt_data->higig,
                           pkt_data->higig_len);
            }
            if (pkt_data->txpmd) {
                sal_memcpy(packet->pmd.txpmd, pkt_data->txpmd[i],
                           sizeof(pkt_data->txpmd[i]));
            }
            if (pkt_data->lbhdr) {
                sal_memcpy(packet->pmd.lbhdr, pkt_data->lbhdr,
                           pkt_data->lbhdr_len);
            }
            if (pkt_data->raw_data) {
                sal_memcpy(packet->data_buf->data, pkt_data->raw_data,
                           pkt_data->raw_data_byte_len);
                packet->data_buf->data_len = pkt_data->raw_data_byte_len;
            }
            packet->unit = BCMPKT_BPOOL_SHARED_ID;
            packet->flags = 0;
            rv = bcmpkt_tx(unit, 1, packet);

            /* Free space of packet */
            (void)bcmpkt_free(packet->unit, packet);
            packet = NULL;
        }
    }
    if (SHR_FAILURE(rv)) {
        cli_out("Send packet failed (%d)\n", rv);
    }
    return rv;
}

static int
bcma_testcase_lt_field_add(int unit, const bcma_testutil_pkt_fld_cfg_t *fld_data,
                           bcmlt_entry_handle_t entry)
{
    int index_num, start_index, rv;
    uint64_t fld_val;
    const char *field_nm;

    SHR_FUNC_ENTER(unit);

    index_num = fld_data->fld_index_num;
    if (index_num == 0) {
        if (fld_data->field_str_val == NULL) {
            rv = bcmlt_entry_field_add(entry, fld_data->field_name,
                                       fld_data->field_val);
        } else {
            rv = bcmlt_entry_field_symbol_add(entry, fld_data->field_name,
                                              fld_data->field_str_val);
        }
        SHR_IF_ERR_EXIT(rv);
    } else {
        start_index = fld_data->fld_start_index;
        for (; start_index < index_num; start_index++) {
            if (fld_data->field_str_val) {
                field_nm = fld_data->field_str_val;
                rv = bcmlt_entry_field_array_symbol_add
                    (entry, fld_data->field_name, start_index, &field_nm, 1);
            } else {
                fld_val = fld_data->field_val;
                rv = bcmlt_entry_field_array_add
                    (entry, fld_data->field_name, start_index, &fld_val, 1);
            }
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        cli_out("Fail to add data to field [%s]\n", fld_data->field_name);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testcase_lts_config(int unit)
{
    int rv;
    bcmlt_entry_handle_t entry = 0;
    const bcma_testutil_pkt_lt_cfg_t *lt_cfg = NULL;
    const bcma_testutil_pkt_fld_cfg_t *fld_data = NULL;
    int i, lt_cfg_num, j;
    const bcma_testutil_pkt_data_t *pkt_data = NULL;

    SHR_FUNC_ENTER(unit);

    bcma_testutil_pkt_cfg_data_get(unit, &lt_cfg, &lt_cfg_num, &pkt_data);

    for (i = 0; i < lt_cfg_num; i++ ) {
        rv = bcmlt_entry_allocate(unit, lt_cfg[i].table_name, &entry);
        SHR_IF_ERR_EXIT(rv);
        for (j = 0; j < BCMA_TESTUTIL_CFG_FLD_MAX_NUM; j++) {
            fld_data = &(lt_cfg[i].fld_data[j]);
            if (fld_data->field_name == NULL) {
                break;
            }
            rv = bcma_testcase_lt_field_add(unit, fld_data, entry);
            SHR_IF_ERR_EXIT(rv);
        }
        rv = bcmlt_entry_commit(entry, (bcmlt_opcode_t)lt_cfg[i].opcode,
                                BCMLT_PRIORITY_NORMAL);
        if (rv == SHR_E_EXISTS && lt_cfg[i].opcode == BCMLT_OPCODE_INSERT) {
            rv = bcmlt_entry_commit(entry, BCMLT_OPCODE_UPDATE,
                                    BCMLT_PRIORITY_NORMAL);
        }
        SHR_IF_ERR_EXIT(rv);

        rv = bcmlt_entry_free(entry);
        entry = 0;
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    if (SHR_FUNC_ERR()) {
        cli_out("Fail to configure LTs (%d)\n", rv);
    }
    if (entry != 0) {
        (void)bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testcase_dev_init(int unit)
{
    static bcmpkt_dev_init_t cfg = {
        .cgrp_bmp = 0x1,
        .cgrp_size = 4,
        .mac_addr = BCMA_BCMPKT_DEF_NETIF_MAC,
    };
    int rv;
    bcmpkt_dev_drv_types_t drv_type;
    bool initialized;
    bcmpkt_dev_drv_types_t dev_drv_type;
    int max_frame_size = 1536;
    bcmpkt_dma_chan_t chan = {
        .dir = BCMPKT_DMA_CH_DIR_TX,
        .ring_size = 64,
        .max_frame_size = max_frame_size,
    };
    bcmpkt_netif_t netif = {
        .mac_addr = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00},
        .max_frame_size = max_frame_size,
        .flags = BCMPKT_NETIF_F_RCPU_ENCAP,
    };

    rv = bcmpkt_dev_drv_type_get(unit, &drv_type);
    if (SHR_FAILURE(rv)) {
        cli_out("Get device driver failed (%d)\n", rv);
        return rv;
    }
    if (drv_type == BCMPKT_DEV_DRV_T_NONE) {
        cli_out("Driver not attached: %d\n", unit);
        return SHR_E_FAIL;
    }
    rv = bcmpkt_dev_initialized(unit, &initialized);
    if (SHR_FAILURE(rv)) {
        cli_out("Get device initialization status failed (%d)\n", rv);
        return rv;
    }
    if (initialized) {
        cli_out("Already initialized\n");
        return SHR_E_NONE;
    }
    sal_snprintf(cfg.name, BCMPKT_DEV_NAME_MAX, "bcm%d", unit);
    rv = bcmpkt_dev_init(unit, &cfg);
    if (SHR_FAILURE(rv)) {
        cli_out("Network device init failed (%d)\n", rv);
        return rv;
    }
    rv = bcmpkt_dev_drv_type_get(unit, &dev_drv_type);
    if (SHR_FAILURE(rv)) {
        cli_out("Get device driver type failed (%d)\n", rv);
        return rv;
    }
    /* Create per device buffer pool for UNET only. */
    if (dev_drv_type == BCMPKT_DEV_DRV_T_UNET) {
        bcmpkt_bpool_create(unit, max_frame_size, BCMPKT_BPOOL_BCOUNT_DEF);
    }
    /* Configure TX channel*/
    chan.id = 0;
    chan.dir = BCMPKT_DMA_CH_DIR_TX;
    rv = bcmpkt_dma_chan_set(unit, &chan);
    if (SHR_FAILURE(rv)) {
        cli_out("Configure TX channel failed (%d)\n", rv);
        return rv;
    }
    /* Configure RX channel*/
    chan.id = 1;
    chan.dir = BCMPKT_DMA_CH_DIR_RX;
    rv = bcmpkt_dma_chan_set(unit, &chan);
    if (SHR_FAILURE(rv)) {
        cli_out("Configure RX channel failed (%d)\n", rv);
        return rv;
    }
    /* Bringup network device. */
    rv = bcmpkt_dev_enable(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("Pull up network device failed (%d)\n", rv);
        return rv;
    }
    if (dev_drv_type == BCMPKT_DEV_DRV_T_KNET) {
        /* Create netif. */
        rv = bcmpkt_netif_create(unit, &netif);
        if (SHR_FAILURE(rv)) {
            cli_out("Create default network interface failed (%d)\n", rv);
            return rv;
        }
        /* Setup SOCKET. */
        rv = bcmpkt_socket_create(unit, netif.id, NULL);
        if (SHR_FAILURE(rv)) {
            cli_out("Create default UNET failed (%d)\n", rv);
            return rv;
        }
    }
    return SHR_E_NONE;
}

static int
bcma_testcase_dev_cleanup(int unit)
{
    int rv;

    if (!bcmmgmt_dev_attached(unit)) {
        return SHR_E_NONE;
    }
    rv = bcmmgmt_dev_detach(unit);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to do dev detach\n");
        return rv;
    }

    if (bcmmgmt_dev_attached(unit)) {
        return SHR_E_NONE;
    }
    rv = bcmmgmt_dev_attach(unit, false);
    if (SHR_FAILURE(rv)) {
        cli_out("Failed to do dev attach\n");
        return rv;
    }

    return rv;
}

static int
bcma_testcase_ser_bb_stats_cnt_validate(int unit, bcmdrd_ser_rmr_id_t rid)
{
    static uint64_t total_cnt = 0;
    bcmlt_entry_handle_t ser_stats = 0;
    uint64_t int_bus_cnt = 0, int_buffer_cnt = 0, total_cnt_cur = 0;
    int increase = 0;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_entry_allocate(unit, "SER_STATS", &ser_stats);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(ser_stats, "BLK_TYPE", "SER_BLK_ALL");
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_symbol_add(ser_stats, "RECOVERY_TYPE", "SER_RECOVERY_ALL");
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_commit(ser_stats, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(ser_stats, "ECC_PARITY_ERR_INT_BUS_CNT", &int_bus_cnt);
    SHR_IF_ERR_EXIT(rv);

    rv = bcmlt_entry_field_get(ser_stats, "ECC_PARITY_ERR_INT_MEM_CNT", &int_buffer_cnt);
    SHR_IF_ERR_EXIT(rv);

    total_cnt_cur = int_bus_cnt + int_buffer_cnt;
    increase = total_cnt_cur - total_cnt;
    /* Reset */
    if (increase < 0) {
        total_cnt = 0;
        increase = total_cnt_cur - total_cnt;
    }
    if (increase > 0) {
        (void)bcma_testcase_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_EC, 1);
        cli_out("SER func of [%s][%d] is ok.\n",
            bcmdrd_pt_ser_rmr_to_name(unit, rid), rid);
    } else {
        tr143_stats[unit].pt_num_fail++;
        cli_out("Fail to test SER func of [%s][%d].\n",
            bcmdrd_pt_ser_rmr_to_name(unit, rid), rid);
        rv = SHR_E_INTERNAL;
    }
    SHR_IF_ERR_EXIT(rv);

    total_cnt = total_cnt_cur;

exit:
    if (ser_stats) {
        bcmlt_entry_free(ser_stats);
    }
    SHR_FUNC_EXIT();
}

static int
bcma_testcase_ser_bb_inject_validate(int unit, bcmdrd_ser_rmr_id_t rid)
{
    int rv = SHR_E_NONE;

    /* Inject SER error to one bus or buffer */
    rv = bcma_testcase_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_FE, 1);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    /* Trigger it */
    rv = bcma_testcase_ser_pkt_send(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    /* Clear SER error of the bus or buffer */
    rv = bcma_testcase_ser_buf_bus_enable(unit, rid, BCMDRD_SER_EN_TYPE_FE, 0);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    sal_sleep(1);
    rv = bcma_testcase_ser_bb_stats_cnt_validate(unit, rid);
    return rv;
}

static int
bcma_testcase_ser_bb_test(int unit, void *bp, uint32_t option)
{
    bcmdrd_ser_rmr_id_t rid;
    int rv = SHR_E_NONE;
    int skip = 0;
    size_t num_rid = 0, i;
    bcmdrd_ser_rmr_id_t *rid_list = NULL;

    rv = bcmdrd_pt_ser_rmr_id_list_get(unit, 0, NULL, &num_rid);
    if (SHR_FAILURE(rv)) {
        return SHR_E_INTERNAL;
    }
    rid_list = sal_alloc(sizeof(bcmdrd_ser_rmr_id_t) * (num_rid + 1), "bcmaSerRmrIds");
    if (rid_list == NULL) {
        return SHR_E_MEMORY;
    }
    (void)bcmdrd_pt_ser_rmr_id_list_get(unit, num_rid, rid_list, &num_rid);

    for (i = 0; i < num_rid; i++) {
        rid = rid_list[i];
        skip = bcma_testutil_drv_pt_skip(unit, rid, BCMA_TESTUTIL_PT_MEM_TR_143);
        if (skip == TRUE) {
            tr143_stats[unit].pt_num_skip++;
            continue;
        }
        (void)bcma_testcase_ser_bb_inject_validate(unit, rid);
    }
    sal_free(rid_list);

    if (tr143_stats[unit].pt_num_fail == 0) {
        return SHR_E_NONE;
    } else {
        tr143_stats[unit].pt_num_fail = 0;
        return SHR_E_FAIL;
    }
}

static int
bcma_testcase_ser_bb_prepare(int unit, void *bp, uint32_t option)
{
    int rv;

    rv = bcma_testcase_dev_init(unit);
    if (SHR_FAILURE(rv)) {
        return rv;
    }
    return bcma_testcase_lts_config(unit);
}

static int
bcma_testcase_ser_bb_revert(int unit, void *bp, uint32_t option)
{
    return bcma_testcase_dev_cleanup(unit);
}

static bcma_test_proc_t bcma_testcase_ser_bb_proc[] = {
    {
        .desc = "Setup an environment for sending packets.\n",
        .cb = bcma_testcase_ser_bb_prepare,
    },
    {
        .desc = "Inject or validate SER error in internal buffer and bus.\n",
        .cb = bcma_testcase_ser_bb_test,
    },
    {
        .desc = "Revert configurations.\n",
        .cb = bcma_testcase_ser_bb_revert,
    }
};

static bcma_test_op_t bcma_test_case_ser_bb_op = {
    .select = bcma_testcase_ser_bb_select,
    .parser = bcma_testcase_ser_bb_param_parse,
    .help = bcma_testcase_ser_bb_help,
    .recycle = NULL,
    .procs = bcma_testcase_ser_bb_proc,
    .proc_count = COUNTOF(bcma_testcase_ser_bb_proc)
};

bcma_test_op_t *
bcma_testcase_ser_bb_op_get(void)
{
    return &bcma_test_case_ser_bb_op;
}

