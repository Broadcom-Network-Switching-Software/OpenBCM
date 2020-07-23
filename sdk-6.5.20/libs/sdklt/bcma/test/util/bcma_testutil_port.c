/*! \file bcma_testutil_port.c
 *
 * Port operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <sal/sal_sleep.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_port.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmdrd/bcmdrd_feature.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static shr_enum_map_t lpbk_name_id_map[] = {
    {"PC_LPBK_MAC", LB_TYPE_MAC},
    {"PC_LPBK_PMD", LB_TYPE_PHY},
    {"PC_LPBK_NONE", LB_TYPE_NONE}
};

static int
port_pause_set(int unit, int port, bool enable)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, PC_MAC_CONTROLs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    /* insert entry if need */
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL),
         SHR_E_EXISTS);

    if (enable == true) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, PAUSE_TXs, 1));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, PAUSE_RXs, 1));
    } else {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, PAUSE_TXs, 0));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, PAUSE_RXs, 0));
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
port_loopback_set(int unit, int port, testutil_port_lb_type_t lb_mode)
{
    const char *lb_mode_str;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (lb_mode != LB_TYPE_MAC && lb_mode != LB_TYPE_PHY &&
        lb_mode != LB_TYPE_NONE) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Set new loopback mode */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    lb_mode_str = bcma_testutil_id2name(lpbk_name_id_map,
                                        COUNTOF(lpbk_name_id_map), lb_mode);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "LOOPBACK_MODE", lb_mode_str));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    /* Set pause */
    if (lb_mode == LB_TYPE_NONE) {
        SHR_IF_ERR_EXIT(port_pause_set(unit, port, false));
    } else {
        SHR_IF_ERR_EXIT(port_pause_set(unit, port, true));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
port_linkscan_enable(int unit)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "SCAN_ENABLE", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

/*
 * return SHR_E_EXISTS : this port has already existed in linkscan list
 *        SHR_E_NONE   : insert this new port into linkscan list successfully
 */
static int
port_linkscan_insert(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add(entry_hdl, "LINKSCAN_MODE",
                                      "SOFTWARE"));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
port_linkscan_remove(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_PORT_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL), SHR_E_NOT_FOUND);

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
port_link_status_get(int unit, int port, uint64_t *link_status)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(link_status, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_LINK_STATE", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "LINK_STATE", link_status));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_link_status_get(int unit, int port, uint64_t *status)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(port_linkscan_enable(unit));

    rv = port_linkscan_insert(unit, port);

    if (rv == SHR_E_NONE) {
        /* if this is a new port in the linkscan list */
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(port_link_status_get(unit, port, status));
        SHR_IF_ERR_EXIT(port_linkscan_remove(unit, port));
    } else if (rv == SHR_E_EXISTS) {
        /* just get link status if this is a existing port in linkscan */
        SHR_IF_ERR_EXIT(port_link_status_get(unit, port, status));
    } else {
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_setup(int unit, testutil_port_lb_type_t lb_type,
                         bcmdrd_pbmp_t pbmp)
{
    int port;

    SHR_FUNC_ENTER(unit);

    /* Enable linkscan first */
    SHR_IF_ERR_EXIT(port_linkscan_enable(unit));

    /* For each port, add it into linkscan list and setup loopback mode */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (port_linkscan_insert(unit, port), SHR_E_EXISTS);
        if (lb_type == LB_TYPE_MAC || lb_type == LB_TYPE_PHY) {
            SHR_IF_ERR_EXIT
                (port_loopback_set(unit, port, lb_type));
        }
    }

    /* Give link scan a chance to update port status. */
    sal_sleep(1);

    /* need not get link status for LB_TYPE_AUTO */
    if (lb_type == LB_TYPE_AUTO) {
        SHR_EXIT();
    }

    /* check link status to be up */
    BCMDRD_PBMP_ITER(pbmp, port) {
        /*
         * The first time is to clear sw latched state
         * see bcmpc_pmgr_link_status_get/bcmpc_pmgr_link_state_get
         *
         * The second time is to clear hw latched state
         *  ex. Assume the user MAC loopback then EXT loopback
         *      (And assume that system does not reinit between these two cases)
         *      If the port has been connencted by cable,
         *      hw latched state would be setup after MAC loopback test
         *      We need to clear this bit when check link status before EXT test
         */
#define WAIT_LINKUP_TRY_LIMIT (3)
        int try_count = 0;
        uint64_t link;

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Wait port %"PRId32" link up\n"), port));

        while (try_count < WAIT_LINKUP_TRY_LIMIT) {
            SHR_IF_ERR_EXIT(port_link_status_get(unit, port, &link));
            if (link == 1) {
                break;
            }
            try_count++;
        }
        if (!link) {
            cli_out("failed: Port %"PRId32" does not link up\n", port);
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_clear(int unit, testutil_port_lb_type_t lb_type,
                         bcmdrd_pbmp_t pbmp, int link_check)
{
    int port;

    SHR_FUNC_ENTER(unit);

    /* For MAC or PHY lb, setup loopback mode back to none */
    if (lb_type == LB_TYPE_MAC || lb_type == LB_TYPE_PHY) {
        BCMDRD_PBMP_ITER(pbmp, port) {
            SHR_IF_ERR_EXIT
                (port_loopback_set(unit, port, LB_TYPE_NONE));
        }
    }

    /* Give link scan a chance to update port status. */
    sal_sleep(1);

    /* For MAC or PHY lb, check link status to be down */
    if (lb_type == LB_TYPE_MAC || lb_type == LB_TYPE_PHY) {
        /* Simulator assumes port always up, skip the check for link down. */
        if (bcmdrd_feature_is_real_hw(unit) && link_check) {
            BCMDRD_PBMP_ITER(pbmp, port) {
                uint64_t link;
                SHR_IF_ERR_EXIT
                    (port_link_status_get(unit, port, &link));
                if (link) {
                    cli_out("Failed: Port %"PRId32" does not link down\n", port);
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
            }
        }
    }

    /* remove the port from linkscan list */
    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (port_linkscan_remove(unit, port));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
port_mac_control_set(int unit, int port, bool rx_enable, bool tx_enable)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;
    int enable;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_MAC_CONTROL", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    if (rx_enable) {
        enable = 1;
    } else {
        enable = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "RX_ENABLE", enable));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "RX_ENABLE_AUTO", 0));

    if (tx_enable) {
        enable = 1;
    } else {
        enable = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "TX_ENABLE", tx_enable));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "TX_ENABLE_AUTO", 0));

    rv = bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_mac_control_set(int unit, int port, bool rx_enable, bool tx_enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (port_mac_control_set(unit, port, rx_enable, tx_enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_timestamp_enable(int unit, int port,
                                    bool igr_ts, bool egr_ts)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* PC_PORT_TIMESYNC */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_TIMESYNC", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "IEEE_1588", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ONE_STEP_TIMESTAMP", 1));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* PORT_ING_VISIBILITY*/
    if (igr_ts == true) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_ING_VISIBILITY", &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TIMESTAMP", 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TIMESTAMP_ORIGIN", port));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }

    /* PORT_EGR_VISIBILITY*/
    if (egr_ts == true) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(unit, "PORT_EGR_VISIBILITY", &entry_hdl));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_INSERT,
                                BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TIMESTAMP", 1));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "TIMESTAMP_ORIGIN", port));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                                BCMLT_PRIORITY_NORMAL));
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_timestamp_disable(int unit, int port)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    /* PC_PORT_TIMESYNC */
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_TIMESYNC", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "IEEE_1588", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "ONE_STEP_TIMESTAMP", 0));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    /* PORT_[ING/EGR]_VISIBILITY*/
    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_ING_VISIBILITY", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL), SHR_E_NOT_FOUND);
    bcmlt_entry_free(entry_hdl);
    entry_hdl = BCMLT_INVALID_HDL;

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PORT_EGR_VISIBILITY", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_DELETE,
                            BCMLT_PRIORITY_NORMAL), SHR_E_NOT_FOUND);

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
        entry_hdl = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_oversize_pkt_set(int unit, int port, int oversize_pkt)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, PC_MAC_CONTROLs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, OVERSIZE_PKTs, oversize_pkt));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_UPDATE,
                            BCMLT_PRIORITY_NORMAL));

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_oversize_pkt_get(int unit, int port, int *oversize_pkt)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t val;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(oversize_pkt, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, PC_MAC_CONTROLs, &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, PORT_IDs, port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, OVERSIZE_PKTs, &val));

    *oversize_pkt = (int)val;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_speed_get(int unit, int port, uint64_t *speed)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(speed, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(entry_hdl, "SPEED", speed));

    /* if retruned speed = 40000
     *    ==> 40000 Mb/s = 40000 * 1000 * 1000 b/s
     */
    *speed = *speed * 1000 * 1000;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_port_cpu_setup_cb(int unit, void *bp, uint32_t option)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    bcmlt_entry_handle_t entry_hdl_2 = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(0, "PC_PORT", &entry_hdl));

    while ((rv = bcmlt_entry_commit(entry_hdl,
                                    BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        bcmpc_lport_t lport;
        uint64_t field_data;

        if (bcmlt_entry_field_get(entry_hdl, "PORT_ID",
                                  &field_data) != SHR_E_NONE) {
            break;
        }

        lport = (bcmpc_lport_t)field_data;
        if (lport >= BCMDRD_CONFIG_MAX_PORTS) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        if (bcmdrd_lport_type_get(unit, lport) == BCMDRD_PORT_TYPE_CPU) {
            int rv_2;
            const char *tbl_desc = NULL;
            uint64_t port_arr[BCMDRD_CONFIG_MAX_PORTS] = {0};
            uint32_t count = COUNTOF(port_arr), act_size;

            rv_2 = bcmlt_table_desc_get(unit, "PORT_ING_EGR_BLOCK_CPU", &tbl_desc);
            if (rv_2 == SHR_E_NOT_FOUND) {
                continue;
            }

            SHR_IF_ERR_EXIT
                (bcmlt_entry_allocate(unit, "PORT_ING_EGR_BLOCK_CPU",
                                      &entry_hdl_2));

            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_INSERT,
                                    BCMLT_PRIORITY_NORMAL), SHR_E_EXISTS);
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_LOOKUP,
                                    BCMLT_PRIORITY_NORMAL));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_get(entry_hdl_2, "MASK_PORTS", 0, port_arr,
                                             count, &act_size));

            port_arr[lport] = 1;

            SHR_IF_ERR_EXIT
                (bcmlt_entry_field_array_add(entry_hdl_2, "MASK_PORTS", 0, port_arr,
                                             act_size));
            SHR_IF_ERR_EXIT
                (bcmlt_entry_commit(entry_hdl_2, BCMLT_OPCODE_UPDATE,
                                    BCMLT_PRIORITY_NORMAL));
            bcmlt_entry_free(entry_hdl_2);
            entry_hdl_2 = BCMLT_INVALID_HDL;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (entry_hdl_2 != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl_2);
    }
    SHR_FUNC_EXIT();
}
