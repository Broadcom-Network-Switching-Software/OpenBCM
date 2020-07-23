/*! \file bcma_testutil_stat.c
 *
 * Statistic operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcma/test/util/bcma_testutil_stat.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

static shr_enum_map_t ctr_mac_name_id_map[] = {
    {"RX_UC_PKT", SNMP_IF_IN_UCAST_PKTS},
    {"TX_UC_PKT", SNMP_IF_OUT_UCAST_PKTS},
    {"RX_FRAGMENT_PKT", SNMP_ETHER_STATS_FRAGMENTS},
    {"RX_BYTES", SNMP_IF_IN_UCAST_OCTETS},
    {"TX_BYTES", SNMP_IF_OUT_UCAST_OCTETS},
    {"RX_PKT", SNMP_IF_DOT1P_IN_FRAMES},
    {"TX_PKT", SNMP_IF_DOT1P_OUT_FRAMES}
};

static shr_enum_map_t ctr_mac_err_name_id_map[] = {
    {"TX_ERR_PKT", SNMP_IF_OUT_ERRORS},
    {"TX_JABBER_PKT", SNMP_IF_OUT_STATS_JABBERS},
    {"TX_FCS_ERR_PKT", SNMP_IF_OUT_STATS_FCS_ERR_PKTS},
    {"TX_FIFO_UNDER_RUN_PKT", SNMP_IF_OUT_STATS_UFL_PKTS},
    {"TX_OVER_SIZE_PKT", SNMP_IF_OUT_OVERSZ_PKTS},
    {"RX_UNDER_SIZE_PKT", SNMP_ETHER_STATS_UNDERSIZE_PKTS},
    {"RX_JABBER_PKT", SNMP_ETHER_STATS_JABBERS},
    {"RX_ALIGN_ERR_PKT", SNMP_ETHER_STATS_CRC_ALIGN_ERRORS},
    {"RX_FCS_ERR_PKT", SNMP_ETHER_STATS_FCS_ERR_PKTS},
    {"RX_OVER_SIZE_PKT", SNMP_ETHER_STATS_OVRSZ_PKTS},
    {"RX_MTU_CHECK_ERR_PKT", SNMP_ETHER_STATS_MTU_ERR_PKTS},
    {"RX_LEN_OUT_OF_RANGE_PKT", SNMP_ETHER_STATS_LEN_OORANGE_PKTS},
    {"RX_CODE_ERR_PKT", SNMP_ETHER_CODE_ERR_PKTS},
    {"RX_MC_SA_PKT", SNMP_ETHER_MC_SA_PKTS},
    {"RX_PROMISCUOUS_PKT", SNMP_ETHER_PROMISCUOUS_PKTS}
};


static bcma_testutil_stat_t stat_ctr_mac[] = {
    {RX_UC_PKT,         "RX_UC_PKT",       0},
    {TX_UC_PKT,         "TX_UC_PKT",       0},
    {RX_FRAGMENT_PKT,   "RX_FRAGMENT_PKT", 0},
    {RX_BYTES,          "RX_BYTES",        0},
    {TX_BYTES,          "TX_BYTES",        0},
    {RX_PKT,            "RX_PKT",          0},
    {TX_PKT,            "TX_PKT",          0}
};

static bcma_testutil_stat_t stat_ctr_macerr[] = {
    {TX_ERR_PKT,                "TX_ERR_PKT",               0},
    {TX_JABBER_PKT,             "TX_JABBER_PKT",            0},
    {TX_FCS_ERR_PKT,            "TX_FCS_ERR_PKT",           0},
    {TX_FIFO_UNDER_RUN_PKT,     "TX_FIFO_UNDER_RUN_PKT",    0},
    {TX_OVER_SIZE_PKT,          "TX_OVER_SIZE_PKT",         0},
    {RX_UNDER_SIZE_PKT,         "RX_UNDER_SIZE_PKT",        0},
    {RX_JABBER_PKT,             "RX_JABBER_PKT",            0},
    {RX_ALIGN_ERR_PKT,          "RX_ALIGN_ERR_PKT",         0},
    {RX_FCS_ERR_PKT,            "RX_FCS_ERR_PKT",           0},
    {RX_OVER_SIZE_PKT,          "RX_OVER_SIZE_PKT",         0},
    {RX_MTU_CHECK_ERR_PKT,      "RX_MTU_CHECK_ERR_PKT",     0},
    {RX_LEN_OUT_OF_RANGE_PKT,   "RX_LEN_OUT_OF_RANGE_PKT",  0},
    {RX_CODE_ERR_PKT,           "RX_CODE_ERR_PKT",          0},
    {RX_MC_SA_PKT,              "RX_MC_SA_PKT",             0},
    {RX_PROMISCUOUS_PKT,        "RX_PROMISCUOUS_PKT",       0}
};

static int
stat_table_entry_handle(int unit, const char *table, int port, int opcode,
                        const char *stat_name, uint64_t *stat_val)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmlt_entry_allocate(unit, table, &entry_hdl));

    if (opcode == BCMLT_OPCODE_LOOKUP) {
        bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW);
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    rv = bcmlt_entry_commit(entry_hdl, opcode, BCMLT_PRIORITY_NORMAL);
    if (SHR_FAILURE(rv)) {
        /* report error, except for the case that entry not found when delete */
        if (opcode == BCMLT_OPCODE_DELETE && rv == SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    if (opcode == BCMLT_OPCODE_LOOKUP) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, stat_name, stat_val));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_stat_enable(int unit, int port, bool en)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t value[1];

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_CONTROL", &entry_hdl));

    value[0] = (uint64_t)en;
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_array_add(entry_hdl, "PORT_ID", port,
                                     value, 1));

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
bcma_testutil_stat_clear(int unit, int port)
{
    const char *table[2] = {"CTR_MAC", "CTR_MAC_ERR"};
    int rv, i;

    SHR_FUNC_ENTER(unit);

    /*
     * Step 1: delete old entry for the port
     * Step 2: create a new one
     */

    for (i = 0; i < 2; i++) {
        rv = stat_table_entry_handle(unit, table[i], port, BCMLT_OPCODE_DELETE,
                 NULL, NULL);
        if (rv != SHR_E_NONE && rv != SHR_E_NOT_FOUND) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    for (i = 0; i < 2; i++) {
        SHR_IF_ERR_EXIT
            (stat_table_entry_handle(unit, table[i], port, BCMLT_OPCODE_INSERT,
                NULL, NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_stat_get(int unit, int port, bcma_testutil_stat_type_t stat_type,
                       uint64_t *value)
{
    const char *table = NULL;
    const char *field = NULL;
    uint64_t field_data = 0;
    int count = COUNTOF(ctr_mac_name_id_map);

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    field = bcma_testutil_id2name(ctr_mac_name_id_map, count, stat_type);
    if (field != NULL) {
        table = "CTR_MAC";
    } else {
        count = COUNTOF(ctr_mac_err_name_id_map);
        field = bcma_testutil_id2name(ctr_mac_err_name_id_map, count, stat_type);
        if (field != NULL) {
            table = "CTR_MAC_ERR";
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    SHR_IF_ERR_EXIT
        (stat_table_entry_handle(unit, table, port, BCMLT_OPCODE_LOOKUP,
            field, &field_data));

    *value = field_data;

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_stat_mac_get(int unit, int port, bcma_testutil_stat_t **stat,
                           int *size)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    char *field = NULL;
    uint64_t field_data = 0;
    uint8_t field_idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(stat, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_MAC", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    for (field_idx = 0; field_idx < STAT_ID_MAC_MAX; field_idx++) {
        field = stat_ctr_mac[field_idx].name;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, field, &field_data));

        stat_ctr_mac[field_idx].value = field_data;
    }

    *size = STAT_ID_MAC_MAX;
    *stat = (bcma_testutil_stat_t *)stat_ctr_mac;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

int
bcma_testutil_stat_macerr_get(int unit, int port, bcma_testutil_stat_t **stat,
                              int *size)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    char *field = NULL;
    uint64_t field_data = 0;
    uint8_t field_idx;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(size, SHR_E_PARAM);
    SHR_NULL_CHECK(stat, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "CTR_MAC_ERR", &entry_hdl));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_attrib_set(entry_hdl, BCMLT_ENT_ATTR_GET_FROM_HW));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    for (field_idx = 0; field_idx < STAT_ID_MACERR_MAX; field_idx++) {
        field = stat_ctr_macerr[field_idx].name;

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, field, &field_data));

        stat_ctr_macerr[field_idx].value = field_data;
    }

    *size = STAT_ID_MACERR_MAX;
    *stat = (bcma_testutil_stat_t *)stat_ctr_macerr;

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

/*
 *   ex. 9121, 100, false
 *        --> 9121 / 100 = 91
 *            9121 * 1000 / 100 % 1000 = 210   ==> 91.21
 *
 *   ex. 9121, 2, false
 *        --> 9121 / 2 = 4560
 *            9121 * 1000 / 2 % 1000 = 500   ==> 4560.5
 *
 *   ex. 9120, 1, true
 *        --> 9120 / 1000 = 9
 *            9120 % 1000 = 120   ==> 9.120 K
 *
 *   ex. 3129120, 1, true
 *        --> 3129120 / 1000 = 3129
 *        --> 3129 / 1000 = 3
 *            3129 % 1000 = 129   ==> 3.129 M
 */
void
bcma_testutil_stat_show(uint64_t numer, uint64_t denom, bool need_suffix)
{
#define SUFFIX_LIMIT 5

    const char *suffix[SUFFIX_LIMIT] = {"", "K", "M", "G", "T"};
    int suffix_idx = 0;
    uint64_t value, remainder;

    if (denom == 0) {
        cli_out("(divided by zero)");
        return;
    }

    value = numer / denom;
    remainder = 0;

    if (value < 1000 || need_suffix == false) {
        remainder = ((numer * 1000) / denom) % 1000;
    } else {
        while (value >= 1000) {
            suffix_idx++;
            remainder = value % 1000;
            value = value / 1000;
        }
    }

    if (suffix_idx >= SUFFIX_LIMIT) {
        return; /* the number is too large */
    }

    cli_out("%"PRIu64".%03"PRIu64"%s", value, remainder, suffix[suffix_idx]);

}
