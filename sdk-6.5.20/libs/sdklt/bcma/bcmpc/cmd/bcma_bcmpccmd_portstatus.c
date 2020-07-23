/*! \file bcma_bcmpccmd_portstatus.c
 *
 * CLI command handler for "PortStatus".
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <shr/shr_debug.h>
#include <shr/shr_pb.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_port.h>

#include <bcmlt/bcmlt.h>

#include <bcmpc/bcmpc_lport.h>

#include <bcma/bcmpc/bcma_bcmpccmd_portstatus.h>

#define BSL_LOG_MODULE BSL_LS_APPL_PORTDIAG

/*******************************************************************************
 * Local definitions
 */

#define PS_STR_BUF_SIZE 16

#define PS_HEADER_FMT \
        "%-6s"      /* port number */ \
        "%-8s"      /* enable */ \
        "%-6s"      /* PHY link state */ \
        "%-7s"      /* lanes */ \
        "%-7s"      /* speed */ \
        "%-5s"      /* auto negotiate */ \
        "%-7s"      /* pause tx/rx */ \
        "%-7s"      /* max frame */ \
        "%-12s"     /* FEC */ \
        "%-10s\n"   /* loopback */

#define PS_DATA_FMT \
        "%-6d"      /* port number */ \
        "%-8s"      /* enable */ \
        "%-6s"      /* PHY link state */ \
        "%-7d"      /* lanes */ \
        "%-7s"      /* speed */ \
        "%-5s"      /* auto negotiate */ \
        "%-7s"      /* pause tx/rx */ \
        "%-7d"      /* max frame */ \
        "%-12s"     /* FEC */ \
        "%-10s\n"   /* loopback */

#define PS_ERROR_FMT \
        "%-6d"      /* port number */ \
        "%-8s\n"    /* error text */

/*!
 * \brief Fields of PC_PORT entry.
 */
typedef struct pc_port_entry_s {
    uint64_t enable;
    uint64_t max_frame_size;
} pc_port_entry_t;

/*!
 * \brief Fields of PC_PORT_STATUS entry.
 */
typedef struct pc_port_status_entry_s {
    uint64_t num_lanes;
    uint64_t autoneg;
    uint64_t speed;
    uint64_t pause_tx;
    uint64_t pause_rx;
    const char *lpbk_mode;
    const char *fec_mode;
} pc_port_status_entry_t;

/*!
 * \brief Fields of LM_LINK_STATE entry.
 */
typedef struct lm_link_state_entry_s {
    uint64_t phy_link;
} lm_link_state_entry_t;

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Get speed string from value.
 *
 * \param [in] speed Speed value in Mbps.
 * \param [out] buf String buffer.
 *
 * \return Speed string.
 */
static char *
port_status_speed_str(int speed, char *buf)
{
    if (speed == 0) {
        sal_snprintf(buf, PS_STR_BUF_SIZE, "-");
    } else if (speed >= 1000) {
        if (speed % 1000) {
            sal_snprintf(buf, PS_STR_BUF_SIZE,
                         "%d.%dG",
                         speed / 1000, (speed % 1000) / 100);
        } else {
            sal_snprintf(buf, PS_STR_BUF_SIZE,
                         "%dG",
                         speed / 1000);
        }
    } else {
        sal_snprintf(buf, PS_STR_BUF_SIZE,
                     "%dM",
                     speed);
    }

    return buf;
}

/*!
 * \brief Get pause string from value.
 *
 * \param [in] pause_tx Tx pause is enabled or not.
 * \param [in] pause_rx Rx pause is enabled or not.
 * \param [out] buf String buffer.
 *
 * \return Pause string.
 */
static char *
port_status_pause_str(bool pause_tx, bool pause_rx, char *buf)
{
    if (!pause_tx && !pause_rx) {
        sal_snprintf(buf, PS_STR_BUF_SIZE, "None");
    } else {
        sal_snprintf(buf, PS_STR_BUF_SIZE,
                     "%s%s",
                     pause_tx ? "TX " : "", pause_rx ? "RX" : "");
    }

    return buf;
}

/*!
 * \brief Strip string prefix.
 *
 * \param [in] str Source string.
 * \param [in] prefix Prefix to be stripped.
 *
 * \return Stripped string.
 */
static const char *
port_status_str_strip(const char *str, const char *prefix)
{
    int prefix_len = sal_strlen(prefix);

    if (sal_strncmp(str, prefix, prefix_len) == 0) {
        return (str + prefix_len);
    }

    return str;
}

/*!
 * \brief Get PC_PORT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 * \param [out] ed Entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pc_port_entry_get(int unit, int port, pc_port_entry_t *ed)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT", &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, "PORT_ID", (uint64_t)port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "ENABLE", &ed->enable));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "MAX_FRAME_SIZE", &ed->max_frame_size));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get PC_PORT_STATUS entry.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 * \param [out] ed Entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
pc_port_status_entry_get(int unit, int port, pc_port_status_entry_t *ed)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "PC_PORT_STATUS", &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, "PORT_ID", (uint64_t)port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "NUM_LANES", &ed->num_lanes));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "AUTONEG", &ed->autoneg));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "SPEED", &ed->speed));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "PAUSE_TX", &ed->pause_tx));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "PAUSE_RX", &ed->pause_rx));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, "LOOPBACK", &ed->lpbk_mode));
    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_get(eh, "FEC_MODE", &ed->fec_mode));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get LM_LINK_STATE entry.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 * \param [out] ed Entry data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lm_link_state_entry_get(int unit, int port, lm_link_state_entry_t *ed)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate(unit, "LM_LINK_STATE", &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_add(eh, "PORT_ID", (uint64_t)port));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_get(eh, "PHY_LINK", &ed->phy_link));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Print port status.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 */
static void
port_status_print(int unit, int port)
{
    int rv;
    bool lm_unavail = false;
    char sbuf[PS_STR_BUF_SIZE], pbuf[PS_STR_BUF_SIZE];
    pc_port_entry_t ep;
    pc_port_status_entry_t eps;
    lm_link_state_entry_t el;

    if (SHR_FAILURE(pc_port_entry_get(unit, port, &ep)) ||
        SHR_FAILURE(pc_port_status_entry_get(unit, port, &eps))) {
        cli_out(PS_ERROR_FMT,
                port, "failed to get port status");
        return;
    }

    rv = lm_link_state_entry_get(unit, port, &el);
    if (SHR_FAILURE(rv)) {
        if (rv == SHR_E_NOT_FOUND) {
            lm_unavail = true;
        } else {
            cli_out(PS_ERROR_FMT,
                    port, "failed to get link state");
            return;
        }
    }

    cli_out(PS_DATA_FMT,
            port,
            ep.enable > 0 ? "Yes" : "No",
            lm_unavail ? "NA" : (el.phy_link > 0 ? "Up" : "Down"),
            (int)eps.num_lanes,
            port_status_speed_str(eps.speed, sbuf),
            eps.autoneg > 0 ? "Yes" : "No",
            port_status_pause_str(eps.pause_tx > 0, eps.pause_rx > 0, pbuf),
            (int)ep.max_frame_size,
            port_status_str_strip(eps.fec_mode, "PC_FEC_"),
            port_status_str_strip(eps.lpbk_mode, "PC_LPBK_"));
}

/*!
 * \brief Print header for ps command.
 */
static void
port_status_header_print(void)
{
    cli_out(PS_HEADER_FMT,
            "port",             /* port number */
            "enable",           /* enable */
            "link",             /* PHY link state */
            "lanes",            /* number of lanes */
            "speed",            /* speed */
            "AN",               /* auto negotiate */
            "pause",            /* pause tx/rx */
            "frame",            /* max frame */
            "FEC",              /* FEC */
            "loopback");        /* loopback */
}

/*******************************************************************************
 * Public functions
 */

int
bcma_bcmpccmd_portstatus(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit, port;
    const char *arg;
    bcmdrd_pbmp_t pbmp;

    unit = cli->cmd_unit;
    if (!bcmdrd_dev_exists(unit)) {
        return BCMA_CLI_CMD_BAD_ARG;
    }

    /* Get the valid ports */
    BCMDRD_PBMP_CLEAR(pbmp);
    for (port = 0; port < BCMDRD_CONFIG_MAX_PORTS; port++) {
        uint32_t port_type;
        port_type = bcmdrd_lport_type_get(unit, port);
        if (port_type & ~(BCMDRD_PORT_TYPE_FPAN | BCMDRD_PORT_TYPE_MGMT)) {
            continue;
        }
        if (bcmpc_lport_to_pport(unit, port) == BCMPC_INVALID_PPORT) {
            continue;
        }
        BCMDRD_PBMP_PORT_ADD(pbmp, port);
    }

    /* Parse the input ports if any */
    arg = BCMA_CLI_ARG_GET(args);
    if (arg) {
        bcmdrd_pbmp_t pbmp_input;
        BCMDRD_PBMP_CLEAR(pbmp_input);
        if (bcmdrd_pbmp_parse(arg, &pbmp_input) < 0) {
            cli_out("%sUnknown argument: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            return BCMA_CLI_CMD_BAD_ARG;
        }
        BCMDRD_PBMP_AND(pbmp, pbmp_input);
    }

    /* Show header */
    port_status_header_print();

    /* Show port status */
    BCMDRD_PBMP_ITER(pbmp, port) {
        port_status_print(unit, port);
    }

    return BCMA_CLI_CMD_OK;
}
