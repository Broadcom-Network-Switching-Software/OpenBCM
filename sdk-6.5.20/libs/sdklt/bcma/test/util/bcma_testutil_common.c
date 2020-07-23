/*! \file bcma_testutil_common.c
 *
 * Common utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmlt/bcmlt.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcmdrd/bcmdrd_port.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmpc/bcmpc_types.h>
#include <bcmpc/bcmpc_lport.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

const char*
bcma_testutil_id2name(const shr_enum_map_t *name_id_map, int map_len, int id)
{
    int idx;

    if (name_id_map == NULL) {
        return NULL;
    }

    for (idx = 0; idx < map_len; idx++) {
        if (name_id_map[idx].val == id) {
            return name_id_map[idx].name;
        }
    }

    return NULL;
}

int
bcma_testutil_name2id(const shr_enum_map_t *name_id_map, int map_len,
                      const char *name)
{
    int idx;

    if (name_id_map == NULL || name == NULL) {
        return -1;
    }

    for (idx = 0; idx < map_len; idx++) {
        if (sal_strcasecmp(name_id_map[idx].name, name) == 0) {
            return name_id_map[idx].val;
        }
    }

    return -1;
}

int
bcma_testutil_parse_port_bmp(const char *str, bcmdrd_pbmp_t *pbmp)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(str, SHR_E_PARAM);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);

    if (bcmdrd_pbmp_parse(str, pbmp) < 0) {
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_parse_port_list(const char *str, uint32_t *port_list,
                              uint32_t list_size, uint32_t *actual_count)
{
    int port = 0;
    int pstart = -1;
    size_t ch_index = 0;
    uint32_t write_index = 0;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(str, SHR_E_PARAM);
    SHR_NULL_CHECK(port_list, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_count, SHR_E_PARAM);

#define OUTPUT(_num_)                         \
    do {                                      \
        if (list_size <= write_index) {       \
            SHR_ERR_EXIT(SHR_E_PARAM); \
        }                                     \
        port_list[write_index++] = (_num_);   \
    } while (0);

    /* start parsing */
    do {
        char ch = str[ch_index];

        if (ch >= '0' && ch <= '9') {
            port = (port * 10) + (ch - '0');
        } else {
            if (ch_index == 0) {
                /* fisrt character must be a digit */
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            if (pstart >= 0) {
                if (port <= pstart) {
                    /* cannot accept 3-3, 3-2 .. */
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
                while (pstart < port) {
                    OUTPUT(pstart);
                    pstart++;
                }
                pstart = -1;
            }
            if (ch == ',' || ch == 0) {
                OUTPUT(port);
                port = 0;
            } else if (ch == '-') {
                pstart = port;
                port = 0;
            } else {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
        ch_index++;
    } while (ch_index <= sal_strlen(str));

    *actual_count = write_index;

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_default_port_bmp_get(int unit, uint32_t port_type_bmp,
                                   bcmdrd_pbmp_t *pbmp)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pbmp, SHR_E_PARAM);
    BCMDRD_PBMP_CLEAR(*pbmp);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(0, "PC_PORT", &entry_hdl));

    while ((rv = bcmlt_entry_commit(entry_hdl,
                                    BCMLT_OPCODE_TRAVERSE,
                                    BCMLT_PRIORITY_NORMAL)) == SHR_E_NONE) {
        bcmpc_lport_t lport;
        bcmpc_pport_t pport;
        uint64_t field_data;

        if (bcmlt_entry_field_get(entry_hdl, "PORT_ID",
                                  &field_data) != SHR_E_NONE) {
            break;
        }

        lport = (bcmpc_lport_t)field_data;
        if (lport >= BCMDRD_CONFIG_MAX_PORTS) {
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
        }

        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport != BCMPC_INVALID_PPORT &&
            (bcmdrd_port_type_get(unit, (int)pport) & port_type_bmp)) {
            BCMDRD_PBMP_PORT_ADD(*pbmp, lport);
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_default_port_list_get(int unit, uint32_t port_type_bmp,
                                    uint32_t *port_list, uint32_t list_size,
                                    uint32_t *actual_count)
{
    bcmdrd_pbmp_t pbmp;
    int port = 0;
    uint32_t write_index = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(port_list, SHR_E_PARAM);
    SHR_NULL_CHECK(actual_count, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get(unit, port_type_bmp, &pbmp));

    BCMDRD_PBMP_ITER(pbmp, port) {
        if (list_size <= write_index) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        port_list[write_index++] = port;
    }
    *actual_count = write_index;

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_parse_table_index(const char *str, int *tbl_idx)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_NULL_CHECK(str, SHR_E_PARAM);
    SHR_NULL_CHECK(tbl_idx, SHR_E_PARAM);

    if (sal_strcasecmp(str, "min") == 0) {
        *tbl_idx = BCMA_TESTUTIL_TABLE_MIN_INDEX;
    } else if (sal_strcasecmp(str, "max") == 0) {
        *tbl_idx = BCMA_TESTUTIL_TABLE_MAX_INDEX;
    } else if (sal_strcasecmp(str, "mid") == 0) {
        *tbl_idx = BCMA_TESTUTIL_TABLE_MID_INDEX;
    } else {
        *tbl_idx = sal_ctoi(str, NULL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcma_testutil_lt_field_depth_get(int unit, const char *tab, const char *field,
                                 int *depth)
{
    bcmlt_field_def_t *field_def = NULL;
    uint32_t num_of_fields = 0, idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(tab, SHR_E_PARAM);
    SHR_NULL_CHECK(field, SHR_E_PARAM);
    SHR_NULL_CHECK(depth, SHR_E_PARAM);

    /* First call to get the number of the fields. */
    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get(unit, tab, 0, NULL, &num_of_fields));

    SHR_ALLOC(field_def, sizeof(bcmlt_field_def_t) * num_of_fields,
              "bcmaTestUtilTableField");

    /* Second call to get the fields data. */
    SHR_IF_ERR_EXIT
        (bcmlt_table_field_defs_get(unit, tab, num_of_fields, field_def,
                                    &num_of_fields));

    for (idx = 0; idx < num_of_fields; idx++) {
        if (sal_strcasecmp(field_def[idx].name, field) == 0) {
            *depth = field_def[idx].depth;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(SHR_E_NOT_FOUND);

exit:
    SHR_FREE(field_def);

    SHR_FUNC_EXIT();
}

int
bcma_testutil_default_port_speed_list_get(int unit, uint32_t list_size,
                                          uint32_t *speed_list)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t fval;
    uint32_t port;
    bcmdrd_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(speed_list, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get
            (unit, BCMDRD_PORT_TYPE_FPAN, &pbmp));

    for (port = 0; port < list_size; port++) {
        speed_list[port] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

    BCMDRD_PBMP_ITER(pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, "SPEED", &fval));

        if (port < list_size) {
            speed_list[port] = (uint32_t)fval;
        }
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_mgmt_port_speed_list_get(int unit, uint32_t list_size,
                                       uint32_t *speed_list)
{
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;
    uint64_t fval;
    uint32_t port;
    int pipe;
    bcmdrd_pbmp_t mgmt_pbmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(speed_list, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get
            (unit, BCMDRD_PORT_TYPE_MGMT, &mgmt_pbmp));

    /* One or zero mgmt port per pipe */
    for (pipe = 0; pipe < (int)list_size; pipe++) {
        speed_list[pipe] = 0;
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &entry_hdl));

    BCMDRD_PBMP_ITER(mgmt_pbmp, port) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_add(entry_hdl, "PORT_ID", port));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(entry_hdl, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(entry_hdl, "SPEED", &fval));

        pipe = bcmdrd_dev_logic_port_pipe(unit, port);
        if (pipe >= 0 && pipe < (int)list_size) {
            speed_list[pipe] = (uint32_t)fval;
        }

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "pipe %d mgmt port %d speed %d\n"),
                    pipe, port, (uint32_t)fval));
    }

exit:
    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_core_clk_freq_get(int unit, uint32_t str_len,
                                char *core_clk_freq_str)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    const char *clk_freq_str = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(core_clk_freq_str, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DEVICE_CONFIG", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry, "CORE_CLK_FREQ", &clk_freq_str));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "CORE_CLK_FREQ = %s\n"), clk_freq_str));

    if (clk_freq_str !=  NULL && str_len > 0) {
        sal_strncpy(core_clk_freq_str, clk_freq_str, str_len-1);
    } else {
        sal_strcpy(core_clk_freq_str, "*");
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}

int
bcma_testutil_pp_clk_freq_get(int unit, uint32_t str_len,
                              char *pp_clk_freq_str)
{
    bcmlt_entry_handle_t entry = BCMLT_INVALID_HDL;
    const char *clk_freq_str = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pp_clk_freq_str, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "DEVICE_CONFIG", &entry));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(entry, BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(entry, "PP_CLK_FREQ", &clk_freq_str));

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PP_CLK_FREQ = %s\n"), clk_freq_str));

    if (clk_freq_str !=  NULL && str_len > 0) {
        sal_strncpy(pp_clk_freq_str, clk_freq_str, str_len-1);
    } else {
        sal_strcpy(pp_clk_freq_str, "*");
    }

exit:
    if (entry != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry);
    }
    SHR_FUNC_EXIT();
}
