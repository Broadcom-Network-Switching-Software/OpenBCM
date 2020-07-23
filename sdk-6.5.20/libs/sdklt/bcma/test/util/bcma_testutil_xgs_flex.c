/*! \file bcma_testutil_xgs_flex.c
 *
 * XGS flex data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcma/test/util/bcma_testutil_drv.h>
#include <bcma/test/util/bcma_testutil_traffic.h>
#include <bcma/test/util/bcma_testutil_common.h>
#include <bcmlt/bcmlt.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_TEST

#define XGS_TRAFFIC_MAX_PIPES               16

#define XGS_TRAFFIC_MAX_FRAME_SIZE          9416

#define XGS_TRAFFIC_MAX_STRING_SIZE         32

#define XGS_TRAFFIC_LPORTS_PER_PIPE         17
#define XGS_TRAFFIC_FP_LPORTS_PER_PIPE      16
#define XGS_TRAFFIC_FP_PPORTS_PER_PIPE      16
#define XGS_TRAFFIC_PIPE0_FP_LPORTS_OFFSET  1
#define XGS_TRAFFIC_MAX_NUM_PMS             64
#define XGS_TRAFFIC_PORTS_PER_PM            4
#define XGS_TRAFFIC_PORTS_PER_HALF_PM       2

/* Port SPEED */
typedef enum xgs_flex_speed_e {
    SPEED_0   = 0,
    SPEED_10G = 10000,
    SPEED_25G = 25000,
    SPEED_40G = 40000,
    SPEED_50G = 50000,
    SPEED_100G = 100000,
    SPEED_200G = 200000,
    SPEED_400G = 400000
} xgs_flex_speed_t;

/* Half PM mode */
typedef enum xgs_flex_hpm_mode_e {
    PM_MODE_0 = 0,
    /* VCO rate is 20.625G. */
    PM_MODE_4X10G,
    PM_MODE_1X40G,
    /* VCO rate is 25.781G. */
    PM_MODE_4X25G,
    PM_MODE_2X25G_1X50G,
    PM_MODE_1X50G_2X25G,
    PM_MODE_1X100G,
    PM_MODE_2X50G,
    /* VCO rate is 26.562G. */
    PM_MODE_4X50G,
    PM_MODE_2X100G,
    PM_MODE_1X200G,
    PM_MODE_1X400G,
    PM_MODE_1X100G_2X50G,
    PM_MODE_2X50G_1X100G,
    PM_MODE_RAND,
    PM_MODE_COUNT
} xgs_flex_hpm_mode_t;

/* PM mode string to value map */
static const shr_enum_map_t xgs_flex_hpm_mode_map[] = {
    {"0G",           PM_MODE_0},
    {"4x10G",        PM_MODE_4X10G},
    {"1x40G",        PM_MODE_1X40G},
    {"4x25G",        PM_MODE_4X25G},
    {"2x25G_1x50G",  PM_MODE_2X25G_1X50G},
    {"1x50G_2x25G",  PM_MODE_1X50G_2X25G},
    {"2x50G",        PM_MODE_2X50G},
    {"1x100G",       PM_MODE_1X100G},
    {"4x50G",        PM_MODE_4X50G},
    {"2x100G",       PM_MODE_2X100G},
    {"1x200G",       PM_MODE_1X200G},
    {"1x400G",       PM_MODE_1X400G},
    {"1x100G_2x50G", PM_MODE_1X100G_2X50G},
    {"2x50G_1x100G", PM_MODE_2X50G_1X100G},
    {"RAND",         PM_MODE_RAND},
    {NULL,           0}
};

/* Port fec mode */
static const shr_enum_map_t xgs_flex_fec_mode_map[]={
    {"PC_FEC_RS544_2XN", SPEED_400G},
    {"PC_FEC_RS544_2XN", SPEED_200G},
    {"PC_FEC_RS544",     SPEED_100G},
    {"PC_FEC_RS544",     SPEED_50G},
    {NULL, 0}
};

/* Flexport parameter */
typedef struct xgs_flex_param_s {

    /* Logical port bitmap coming from test CLI */
    bcmdrd_pbmp_t test_lport_bmp;

    /* Physical port bitmap coming from test CLI */
    bcmdrd_pbmp_t test_pport_bmp;

    /* Physical port bitmap for down ports specified from test CLI */
    bcmdrd_pbmp_t test_pport_bmp_down;

    /* Physical port bitmap for up ports specified from test CLI */
    bcmdrd_pbmp_t test_pport_bmp_up;

    /* Physical port speed for initial config */
    uint32_t default_pport_speed[BCMDRD_CONFIG_MAX_PORTS];

    /* PM mode for flexing in string */
    char pm_mode_str[XGS_TRAFFIC_MAX_NUM_PMS]
                    [XGS_TRAFFIC_MAX_STRING_SIZE];

    /* Physical port speed for flexing config */
    uint32_t flex_pport_speed[BCMDRD_CONFIG_MAX_PORTS];

    /* Down port bitmap */
    bcmdrd_pbmp_t down_pport_bmp;

    /* Down port bitmap */
    bcmdrd_pbmp_t down_lport_bmp;

    /* Up port bitmap */
    bcmdrd_pbmp_t up_pport_bmp;

    /* Up port bitmap */
    bcmdrd_pbmp_t up_lport_bmp;

    /* Number of lanes per port */
    uint8_t pport_num_lanes[BCMDRD_CONFIG_MAX_PORTS];

    /* Port fec mode */
    char pport_fec_mode[BCMDRD_CONFIG_MAX_PORTS][XGS_TRAFFIC_MAX_STRING_SIZE];

    /* Logical to physical port map */
    uint32_t pport_lport[BCMDRD_CONFIG_MAX_PORTS];

    /* Logical to physical port map */
    uint32_t default_l2p_map[BCMDRD_CONFIG_MAX_PORTS];

    /* Physical to logical port map */
    uint32_t default_p2l_map[BCMDRD_CONFIG_MAX_PORTS];

    /* Logical to physical port map */
    uint32_t flex_l2p_map[BCMDRD_CONFIG_MAX_PORTS];

    /* Physical to logical port map */
    uint32_t flex_p2l_map[BCMDRD_CONFIG_MAX_PORTS];

} xgs_flex_param_t;

/* Rate calculation struct */
typedef struct xgs_rate_param_s {

    /* Active port bitmap */
    bcmdrd_pbmp_t active_port_bmp;

    /* Packet size */
    uint32_t pkt_size;

    /* Active pipe bitmap */
    uint32_t active_pipe_bmp;

    /* Number of cells per packet */
    uint32_t cells_per_pkt;

    /* PP clock frequency */
    uint32_t pp_clk_freq;

    /* Core clock frequency */
    uint32_t core_clk_freq;

    /* Auxiliary bandwidth in mega packet per second */
    uint32_t pipe_ancl_mpps[XGS_TRAFFIC_MAX_PIPES];

    /* Auxiliary bandwidth in mega cells per second */
    uint32_t pipe_ancl_mcps[XGS_TRAFFIC_MAX_PIPES];

    /* Mgmt port speed in Mbps */
    uint32_t mgmt_port_speed[XGS_TRAFFIC_MAX_PIPES];

    /* Port speed in Mbps */
    uint32_t port_speed[BCMDRD_CONFIG_MAX_PORTS];

    /* Port packet header length in Byte */
    uint32_t port_pkt_hdr_len[BCMDRD_CONFIG_MAX_PORTS];

    /* Expected port rate in Mbps */
    uint64_t exp_port_rate[BCMDRD_CONFIG_MAX_PORTS];

} xgs_rate_param_t;


static int
xgs_flex_pm_mode_name2id(int unit, char *name, uint32_t *id)
{
    int map_len, pm_mode;
    SHR_FUNC_ENTER(unit);

    map_len = COUNTOF(xgs_flex_hpm_mode_map);
    pm_mode = bcma_testutil_name2id(xgs_flex_hpm_mode_map,
                                    map_len, name);
    if (pm_mode == -1) {
        cli_out("Invalid PM_MODE %s\n", name);
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    *id = pm_mode;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_avail_lport_get(int unit, uint32_t array_size,
                         uint32_t *l2p_map, uint32_t pport,
                         uint32_t *lport, uint32_t *status)
{
    uint32_t lport_tmp = 0;
    uint32_t pipe, port, lport_offset, fp_lport_lo, fp_lport_hi, find = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(l2p_map, SHR_E_PARAM);
    SHR_NULL_CHECK(lport, SHR_E_PARAM);
    SHR_NULL_CHECK(status, SHR_E_PARAM);

    *lport = 0;
    /* Set valid logical port numbering for fp physical port */
    pipe = (pport - 1) / XGS_TRAFFIC_FP_PPORTS_PER_PIPE;
    lport_offset= (pipe == 0) ? XGS_TRAFFIC_PIPE0_FP_LPORTS_OFFSET : 0;
    fp_lport_lo = lport_offset + pipe * XGS_TRAFFIC_LPORTS_PER_PIPE;
    fp_lport_hi = fp_lport_lo + XGS_TRAFFIC_FP_LPORTS_PER_PIPE - 1;

    /* Get first available logical port number */
    for (port = fp_lport_lo; port <= fp_lport_hi; port++) {
        if (port < array_size) {
            if (l2p_map[port] == pport) {
                cli_out("Error: Physical port %d is already used.\n", pport);
                SHR_ERR_EXIT(SHR_E_PARAM);;
            } else if ((l2p_map[port] == 0) && (find == 0)) {
                lport_tmp = port;
                find = 1;
            }
        }
    }

    *lport = lport_tmp;
    *status = (*lport == 0) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_pm_speed_get(int unit, uint32_t hpm_mode, uint32_t array_size,
                      uint32_t *speed_array)
{
    uint32_t pm_port_speed[XGS_TRAFFIC_PORTS_PER_PM];
    uint32_t i;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(speed_array, SHR_E_PARAM);

    sal_memset(pm_port_speed, 0,
               XGS_TRAFFIC_PORTS_PER_PM * sizeof(uint32_t));

    switch (hpm_mode) {
        case PM_MODE_4X10G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                pm_port_speed[i] = SPEED_10G;
            }
            break;
        case PM_MODE_1X40G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i % XGS_TRAFFIC_PORTS_PER_HALF_PM) == 0) {
                    pm_port_speed[i] = SPEED_40G;
                }
            }
            break;
        case PM_MODE_4X25G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                pm_port_speed[i] = SPEED_25G;
            }
            break;
        case PM_MODE_2X25G_1X50G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i == 0) || (i == 1)) {
                    pm_port_speed[i] = SPEED_25G;
                } else if (i == 2) {
                    pm_port_speed[i] = SPEED_50G;
                }
            }
            break;
        case PM_MODE_1X50G_2X25G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i == 2) || (i == 3)) {
                    pm_port_speed[i] = SPEED_25G;
                } else if (i == 0) {
                    pm_port_speed[i] = SPEED_50G;
                }
            }
            break;
        case PM_MODE_4X50G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                pm_port_speed[i] = SPEED_50G;
            }
            break;
        case PM_MODE_1X100G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i % XGS_TRAFFIC_PORTS_PER_HALF_PM) == 0) {
                    pm_port_speed[i] = SPEED_100G;
                }
            }
            break;
        case PM_MODE_2X100G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                pm_port_speed[i] = SPEED_100G;
            }
            break;
        case PM_MODE_1X200G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i % XGS_TRAFFIC_PORTS_PER_HALF_PM) == 0) {
                    pm_port_speed[i] = SPEED_200G;
                }
            }
            break;

        case PM_MODE_1X400G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i % XGS_TRAFFIC_PORTS_PER_PM) == 0) {
                    pm_port_speed[i] = SPEED_400G;
                }
            }
            break;
        case PM_MODE_1X100G_2X50G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if (i == 0) {
                    pm_port_speed[i] = SPEED_100G;
                }
                else if ((i == 2) || (i == 3)) {
                    pm_port_speed[i] = SPEED_50G;
                }
            }
            break;
        case PM_MODE_2X50G_1X100G:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i == 0) || (i == 1)) {
                    pm_port_speed[i] = SPEED_50G;
                }
                else if (i == 2) {
                    pm_port_speed[i] = SPEED_100G;
                }
            }
            break;
        default:
            for (i = 0; i < XGS_TRAFFIC_PORTS_PER_PM; i++) {
                if ((i % XGS_TRAFFIC_PORTS_PER_PM) == 0) {
                    pm_port_speed[i] = SPEED_400G;
                }
            }
            break;
    }

    for (i = 0; i < array_size; i++) {
        speed_array[i] = i < XGS_TRAFFIC_PORTS_PER_PM ?
                         pm_port_speed[i] : 0;
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_speed_by_pm_mode_set(int unit, char *pm_mode_str,
                              uint32_t pm_num, uint32_t array_size,
                              uint32_t *speed_array)
{
    uint32_t subp, pport, pm_mode = 0;
    uint32_t pm_port_speed[XGS_TRAFFIC_PORTS_PER_PM];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(speed_array, SHR_E_PARAM);

    /* Get pm speed by pm_mode */
    sal_memset(pm_port_speed, 0,
               XGS_TRAFFIC_PORTS_PER_PM * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (xgs_flex_pm_mode_name2id(unit, pm_mode_str, &pm_mode));
    SHR_IF_ERR_EXIT
        (xgs_flex_pm_speed_get
            (unit, pm_mode, XGS_TRAFFIC_PORTS_PER_PM, pm_port_speed));

    /* Set output port speed array */
    for (subp = 0; subp < XGS_TRAFFIC_PORTS_PER_PM; subp++) {
        pport = pm_num * XGS_TRAFFIC_PORTS_PER_PM + subp + 1;
        if (pport < array_size) {
            speed_array[pport] = pm_port_speed[subp];
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_pport2pm_get(int unit, uint32_t pport, uint32_t *pm)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm, SHR_E_PARAM);

    *pm = (pport - 1) / XGS_TRAFFIC_PORTS_PER_PM;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_num_ports_per_pm_get(int unit, uint32_t *num_ports)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(num_ports, SHR_E_PARAM);

    *num_ports = XGS_TRAFFIC_PORTS_PER_PM;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_vco_by_pm_mode_get(int unit, char *pm_mode_str,
                                 bcmpc_vco_type_t *port_vco)
{
    bcmpc_vco_type_t vco;
    uint32_t pm_mode = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm_mode_str, SHR_E_PARAM);
    SHR_NULL_CHECK(port_vco, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (xgs_flex_pm_mode_name2id(unit, pm_mode_str, &pm_mode));

    switch (pm_mode) {
        /*! VCO rate is 20.625G. */
        case PM_MODE_4X10G:
        case PM_MODE_1X40G:
            vco = BCMPC_VCO_RATE_20P625G;
            break;
        /*! VCO rate is 25.781G. */
        case PM_MODE_4X25G:
        case PM_MODE_2X25G_1X50G:
        case PM_MODE_1X50G_2X25G:
        case PM_MODE_1X100G:
            vco = BCMPC_VCO_RATE_25P781G;
            break;
        /*! VCO rate is 26.562G. */
        case PM_MODE_4X50G:
        case PM_MODE_2X100G:
        case PM_MODE_1X200G:
        case PM_MODE_1X400G:
            vco = BCMPC_VCO_RATE_26P562G;
            break;
        default:
            vco = BCMPC_VCO_RATE_26P562G;
            break;
    }
    *port_vco = vco;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_num_lanes_get(int unit, char *pm_mode_str,
                            uint32_t speed, uint8_t *num_lanes)
{
    uint8_t lanes = 0;
    bcmpc_vco_type_t vco = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm_mode_str, SHR_E_PARAM);
    SHR_NULL_CHECK(num_lanes, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (xgs_flex_port_vco_by_pm_mode_get(unit, pm_mode_str, &vco));

    switch (speed) {
        case SPEED_0:
            lanes = 0;
            break;
        case SPEED_10G:
            lanes = 1;
            break;
        case SPEED_25G:
            lanes = 1;
            break;
        case SPEED_40G:
            lanes = 4;
            break;
        case SPEED_50G:
            switch (vco) {
                case BCMPC_VCO_RATE_25P781G:
                    lanes = 2;
                    break;
                case BCMPC_VCO_RATE_26P562G:
                    lanes = 1;
                    break;
                default :
                    lanes = 1;
                    break;
            }
            break;
        case SPEED_100G:
            switch (vco) {
                case BCMPC_VCO_RATE_25P781G:
                    lanes = 4;
                    break;
                case BCMPC_VCO_RATE_26P562G:
                    lanes = 2;
                    break;
                default :
                    lanes = 2;
                    break;
            }
            break;
        case SPEED_200G:
            lanes = 4;
            break;
        case SPEED_400G:
            lanes = 8;
            break;
        default:
            /* should not happen */
            SHR_IF_ERR_EXIT(SHR_E_INTERNAL);
    }

    *num_lanes = lanes;

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_fec_mode_get(int unit, uint32_t speed,
                           uint32_t str_size, char *fec_mode_str)
{
    uint32_t idx = 0;
    char *local_fec_mode_str;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fec_mode_str, SHR_E_PARAM);

    local_fec_mode_str = NULL;
    while (xgs_flex_fec_mode_map[idx].name != NULL) {
        if (xgs_flex_fec_mode_map[idx].val == (int) speed) {
            local_fec_mode_str = xgs_flex_fec_mode_map[idx].name;
            break;
        }
        idx++;
    }

    if (local_fec_mode_str != NULL) {
        sal_strncpy(fec_mode_str, local_fec_mode_str, str_size-1);
    } else {
        sal_strcpy(fec_mode_str, "*");
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_down(int unit, bcmdrd_pbmp_t down_pport_bmp,
                   uint32_t array_size, uint32_t *p2l_map_array)
{
    uint32_t pport, lport;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p2l_map_array, SHR_E_PARAM);

    cli_out("flex port down: \n");

    /* Program LT PC_PORT */
    BCMDRD_PBMP_ITER(down_pport_bmp, pport) {
        if (pport < array_size) {
            lport = p2l_map_array[pport];
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_flex_pc_port_delete(unit, lport));
        }
    }

    /* Program LT PC_PORT_PHYS_MAP */
    BCMDRD_PBMP_ITER(down_pport_bmp, pport) {
        if (pport < array_size) {
            lport = p2l_map_array[pport];
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_flex_pc_port_phys_map_delete(unit, lport));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_up(int unit, bcmdrd_pbmp_t up_pport_bmp,
                 uint32_t array_size, uint32_t *p2l_map_array,
                 uint32_t *pport_speed_array,
                 uint8_t *pport_num_lanes_array,
                 uint32_t row_size, uint32_t col_size,
                 char *pport_fec_mode_array)
{
    uint32_t pport, lport, speed, num_lanes;
    uint32_t idx_max, idx;
    char *fec_mode;
    char *speed_vco = "PC_SPEED_VCO_NONE";
    uint32_t max_frame_size = XGS_TRAFFIC_MAX_FRAME_SIZE;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(p2l_map_array, SHR_E_PARAM);
    SHR_NULL_CHECK(pport_speed_array, SHR_E_PARAM);
    SHR_NULL_CHECK(pport_num_lanes_array, SHR_E_PARAM);
    SHR_NULL_CHECK(pport_fec_mode_array, SHR_E_PARAM);

    cli_out("flex port up: \n");

    /* Program LT PC_PORT_PHYS_MAP */
    BCMDRD_PBMP_ITER(up_pport_bmp, pport) {
        if (pport < array_size) {
            lport = p2l_map_array[pport];
            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_flex_pc_port_phys_map_add
                    (unit, lport, pport));
        }
    }

    /* Program LT PC_PORT */
    idx_max = row_size * col_size;
    BCMDRD_PBMP_ITER(up_pport_bmp, pport) {
        if (pport < array_size) {
            lport     = p2l_map_array[pport];
            speed     = pport_speed_array[pport];
            num_lanes = pport_num_lanes_array[pport];
            idx       = pport * col_size;
            fec_mode  = idx < idx_max ? &pport_fec_mode_array[idx] :
                                        &pport_fec_mode_array[0];

            SHR_IF_ERR_EXIT
                (bcma_testutil_drv_flex_pc_port_add
                    (unit, lport, speed, num_lanes, max_frame_size,
                     fec_mode, speed_vco));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_init_flex_port_bmp(int unit,
                            xgs_flex_param_t *flex_param)
{
    uint32_t pm, subp, port, pport, lport, curr_speed, next_speed, num_ports_per_pm;
    uint32_t status;
    bcmdrd_pbmp_t down_pport_bmp, down_lport_bmp, up_pport_bmp, up_lport_bmp;
    bcmdrd_pbmp_t visit_pbmp;
    bcmdrd_pbmp_t test_pport_bmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flex_param, SHR_E_PARAM);

    /* Get number of lanes per PM */
    SHR_IF_ERR_EXIT
        (xgs_flex_num_ports_per_pm_get(unit, &num_ports_per_pm));

    /* Set port speed by PM mode */
    for (pm = 0; pm < XGS_TRAFFIC_MAX_NUM_PMS; pm++) {
        SHR_IF_ERR_EXIT
            (xgs_flex_speed_by_pm_mode_set
                (unit, flex_param->pm_mode_str[pm], pm,
                 BCMDRD_CONFIG_MAX_PORTS, flex_param->flex_pport_speed));
    }

    /* Set down port bitmap */
    BCMDRD_PBMP_CLEAR(down_pport_bmp);
    BCMDRD_PBMP_CLEAR(down_lport_bmp);
    BCMDRD_PBMP_CLEAR(visit_pbmp);
    if (!BCMDRD_PBMP_IS_NULL(flex_param->test_pport_bmp_down)) {
        BCMDRD_PBMP_ASSIGN(test_pport_bmp, flex_param->test_pport_bmp_down);
        cli_out("down pbmp generation: using physical port config from CLI\n");
    } else {
        BCMDRD_PBMP_ASSIGN(test_pport_bmp, flex_param->test_pport_bmp);
        cli_out("down pbmp generation: using logical port config from CLI\n");
    }

    BCMDRD_PBMP_ITER(test_pport_bmp, pport) {
        if (BCMDRD_PBMP_MEMBER(visit_pbmp, pport)) {
            continue;
        }

        xgs_flex_pport2pm_get(unit, pport, &pm);
        for (subp = 0; subp < num_ports_per_pm; subp++) {
            pport = pm * num_ports_per_pm + 1 + subp;
            curr_speed = flex_param->default_pport_speed[pport];
            next_speed = flex_param->flex_pport_speed[pport];
            if (curr_speed > 0 && curr_speed != next_speed) {
                lport = flex_param->default_p2l_map[pport];
                BCMDRD_PBMP_PORT_ADD(down_pport_bmp, pport);
                BCMDRD_PBMP_PORT_ADD(down_lport_bmp, lport);
                BCMDRD_PBMP_PORT_ADD(visit_pbmp, pport);
            }
        }
    }

    BCMDRD_PBMP_ASSIGN(flex_param->down_pport_bmp, down_pport_bmp);
    BCMDRD_PBMP_ASSIGN(flex_param->down_lport_bmp, down_lport_bmp);
    BCMDRD_PBMP_ITER(flex_param->down_pport_bmp, pport) {
        lport = flex_param->default_p2l_map[pport];
        flex_param->flex_l2p_map[lport] = 0;
        flex_param->flex_p2l_map[pport] = 0;
    }

    /* Set up port bitmap */
    BCMDRD_PBMP_CLEAR(up_pport_bmp);
    BCMDRD_PBMP_CLEAR(up_lport_bmp);
    BCMDRD_PBMP_CLEAR(visit_pbmp);
    if (BCMDRD_PBMP_NOT_NULL(flex_param->test_pport_bmp_down) ||
        BCMDRD_PBMP_NOT_NULL(flex_param->test_pport_bmp_up)) {
        BCMDRD_PBMP_ASSIGN(test_pport_bmp, flex_param->test_pport_bmp_up);
        cli_out("up   pbmp generation: using physical port config from CLI\n");
    } else {
        BCMDRD_PBMP_ASSIGN(test_pport_bmp, flex_param->test_pport_bmp);
        cli_out("up   pbmp generation: using logical port config from CLI\n");
    }

    BCMDRD_PBMP_ITER(test_pport_bmp, port) {
        if (BCMDRD_PBMP_MEMBER(visit_pbmp, port)) {
            continue;
        }

        xgs_flex_pport2pm_get(unit, port, &pm);
        for (subp = 0; subp < num_ports_per_pm; subp++) {
            pport = pm * num_ports_per_pm + 1 + subp;
            if (BCMDRD_PBMP_NOT_NULL(flex_param->test_pport_bmp_up) &&
                !BCMDRD_PBMP_MEMBER(flex_param->test_pport_bmp_up, pport)) {
                continue;
            }
            curr_speed = flex_param->default_pport_speed[pport];
            next_speed = flex_param->flex_pport_speed[pport];
            if (next_speed > 0 && next_speed != curr_speed ) {
                lport = flex_param->default_p2l_map[pport];
                BCMDRD_PBMP_PORT_ADD(up_pport_bmp, pport);
                BCMDRD_PBMP_PORT_ADD(up_lport_bmp, lport);
            }
            BCMDRD_PBMP_PORT_ADD(visit_pbmp, pport);
        }
    }

    BCMDRD_PBMP_ASSIGN(flex_param->up_pport_bmp, up_pport_bmp);
    BCMDRD_PBMP_ASSIGN(flex_param->up_lport_bmp, up_lport_bmp);
    BCMDRD_PBMP_ITER(flex_param->up_pport_bmp, pport) {
        SHR_IF_ERR_EXIT
            (xgs_flex_avail_lport_get
                (unit, BCMDRD_CONFIG_MAX_PORTS, flex_param->flex_l2p_map,
                 pport, &lport, &status));

        /* If is able to find an available lport, update l2p/p2l port map,
         * otherwise, remove port from up-port bitmap.
         */
        if (status == 1) {
            flex_param->flex_l2p_map[lport] = pport;
            flex_param->flex_p2l_map[pport] = lport;
        } else {
            BCMDRD_PBMP_PORT_REMOVE(flex_param->up_pport_bmp, pport);
        }
    }

    /* print */
    BCMDRD_PBMP_ITER(flex_param->down_pport_bmp, pport) {
        cli_out("down_port: pport=%3d, lport=%3d, default_speed=%3dG, \
                 flex_speed=%3dG\n",
                pport, flex_param->default_p2l_map[pport],
                flex_param->default_pport_speed[pport] / 1000,
                flex_param->flex_pport_speed[pport] / 1000);
    }
    BCMDRD_PBMP_ITER(flex_param->up_pport_bmp, pport) {
        cli_out("up_pport: pport=%3d, default_lport=%3d, \
                 default_speed=%3dG, flex_speed=%3dG\n",
                pport, flex_param->flex_p2l_map[pport],
                flex_param->default_pport_speed[pport] / 1000,
                flex_param->flex_pport_speed[pport] / 1000);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_init(int unit, xgs_flex_param_t *flex_param,
              bcmdrd_pbmp_t lport_bmp, bcmdrd_pbmp_t phy_pbmp_down,
              bcmdrd_pbmp_t phy_pbmp_up, uint32_t num_pms,
              uint32_t str_size, char *pm_mode_str)
{
    uint32_t pport, lport/* , pm_mode */;
    uint32_t pm, copy_str_size;
    uint32_t l2p_map[BCMDRD_CONFIG_MAX_PORTS];
    uint32_t lport_speed[BCMDRD_CONFIG_MAX_PORTS];
    bcmdrd_pbmp_t pbmp, default_lport_bmp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flex_param, SHR_E_PARAM);
    SHR_NULL_CHECK(pm_mode_str, SHR_E_PARAM);

    /* Init testing port bitmap from input parameters */
    BCMDRD_PBMP_ASSIGN(flex_param->test_lport_bmp, lport_bmp);
    BCMDRD_PBMP_ASSIGN(flex_param->test_pport_bmp_down, phy_pbmp_down);
    BCMDRD_PBMP_ASSIGN(flex_param->test_pport_bmp_up, phy_pbmp_up);

    /* Init PM mode to be flexed */
    for (pm = 0; pm < XGS_TRAFFIC_MAX_NUM_PMS; pm++) {
        sal_strncpy(flex_param->pm_mode_str[pm], "0G",
                    XGS_TRAFFIC_MAX_STRING_SIZE);
    }
    copy_str_size = str_size < XGS_TRAFFIC_MAX_STRING_SIZE ?
                    str_size : XGS_TRAFFIC_MAX_STRING_SIZE;
    for (pm = 0; pm < num_pms && pm < XGS_TRAFFIC_MAX_NUM_PMS; pm++) {
        sal_strncpy(flex_param->pm_mode_str[pm], &pm_mode_str[pm * str_size],
                    copy_str_size - 1);
    }

    /* Init default logical port bitmap from config */
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_bmp_get
            (unit, BCMDRD_PORT_TYPE_FPAN, &default_lport_bmp));

    /* Init port mapping for l2p_map and p2l_map */
    sal_memset(l2p_map, 0, sizeof(uint32_t) * BCMDRD_CONFIG_MAX_PORTS);
    SHR_IF_ERR_EXIT
        (bcma_testutil_drv_flex_l2p_map_get
            (unit, default_lport_bmp, BCMDRD_CONFIG_MAX_PORTS,
             l2p_map));

    BCMDRD_PBMP_ITER(default_lport_bmp, lport) {
        pport = l2p_map[lport];
        flex_param->default_l2p_map[lport] = pport;
        flex_param->default_p2l_map[pport] = lport;

        flex_param->flex_l2p_map[lport] = flex_param->default_l2p_map[lport];
        flex_param->flex_p2l_map[pport] = flex_param->default_p2l_map[pport];
    }

    /* Init testing physical port bitmap */
    BCMDRD_PBMP_CLEAR(pbmp);
    BCMDRD_PBMP_ITER(flex_param->test_lport_bmp, lport) {
        pport = flex_param->default_l2p_map[lport];
        if (pport == 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        BCMDRD_PBMP_PORT_ADD(pbmp, pport);
    }
    BCMDRD_PBMP_ASSIGN(flex_param->test_pport_bmp, pbmp);

    /* Init physical port speed list from config */
    sal_memset(lport_speed, 0, BCMDRD_CONFIG_MAX_PORTS * sizeof(uint32_t));
    SHR_IF_ERR_EXIT
        (bcma_testutil_default_port_speed_list_get
            (unit, BCMDRD_CONFIG_MAX_PORTS, lport_speed));
    BCMDRD_PBMP_ITER(default_lport_bmp, lport) {
        pport = flex_param->default_l2p_map[lport];
        flex_param->default_pport_speed[pport] = lport_speed[lport];
    }

    /* Print config info before flex */
    BCMDRD_PBMP_ITER(default_lport_bmp, lport) {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "%s lport=%3d, pport=%3d, speed=%0dG\n"),
                     "all_port_info:", lport,
                     flex_param->default_l2p_map[lport],
                     lport_speed[lport] / 1000));
    }

    /* Init down_port bitmap and up_port bitmap */
    SHR_IF_ERR_EXIT
        (xgs_flex_init_flex_port_bmp(unit, flex_param));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_down_wrap(int unit, xgs_flex_param_t *flex_param)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flex_param, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (xgs_flex_port_down
            (unit, flex_param->down_pport_bmp, BCMDRD_CONFIG_MAX_PORTS,
             flex_param->default_p2l_map));

exit:
    SHR_FUNC_EXIT();
}

static int
xgs_flex_port_up_wrap(int unit, xgs_flex_param_t *flex_param)
{
    uint32_t pport, pm;
    uint8_t num_lanes = 0;
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(flex_param, SHR_E_PARAM);

    /* Prepare up port setting: speed, vco, num_lanes, lport */
    BCMDRD_PBMP_ITER(flex_param->up_pport_bmp, pport) {
        flex_param->pport_lport[pport] = flex_param->flex_p2l_map[pport];
        SHR_IF_ERR_EXIT(xgs_flex_pport2pm_get(unit, pport, &pm));
        SHR_IF_ERR_EXIT
            (xgs_flex_port_num_lanes_get
                (unit, flex_param->pm_mode_str[pm],
                 flex_param->flex_pport_speed[pport], &num_lanes));
        flex_param->pport_num_lanes[pport] = num_lanes;

        SHR_IF_ERR_EXIT
            (xgs_flex_port_fec_mode_get
                (unit, flex_param->flex_pport_speed[pport],
                 XGS_TRAFFIC_MAX_STRING_SIZE - 1,
                 flex_param->pport_fec_mode[pport]));

        cli_out("update flex_param for up_port: pport=%3d, lport=%3d, \
                 num_lanes=%0d, speed=%0dG, fec_mode= %s \n",
                pport,
                flex_param->pport_lport[pport],
                flex_param->pport_num_lanes[pport],
                flex_param->flex_pport_speed[pport] / 1000,
                flex_param->pport_fec_mode[pport]);
    }
    BCMDRD_PBMP_CLEAR(flex_param->up_lport_bmp);
    BCMDRD_PBMP_ITER(flex_param->up_pport_bmp, pport) {
        BCMDRD_PBMP_PORT_ADD(flex_param->up_lport_bmp,
                             flex_param->pport_lport[pport]);
    }

    /* Flex port up */
    SHR_IF_ERR_EXIT
        (xgs_flex_port_up
            (unit, flex_param->up_pport_bmp, BCMDRD_CONFIG_MAX_PORTS,
            flex_param->pport_lport, flex_param->flex_pport_speed,
            flex_param->pport_num_lanes, BCMDRD_CONFIG_MAX_PORTS,
            XGS_TRAFFIC_MAX_STRING_SIZE,
            &(flex_param->pport_fec_mode[0][0])));

exit:
    SHR_FUNC_EXIT();
}

int
xgs_flex_do_flex(int unit, bcmdrd_pbmp_t lport_bmp,
                 bcmdrd_pbmp_t phy_pbmp_down,
                 bcmdrd_pbmp_t phy_pbmp_up,
                 uint32_t num_pms, uint32_t str_size,
                 char *pm_mode_str,
                 bcmdrd_pbmp_t *down_lport_bmp,
                 bcmdrd_pbmp_t *up_lport_bmp)
{
    xgs_flex_param_t *flex_param = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm_mode_str, SHR_E_PARAM);
    SHR_NULL_CHECK(down_lport_bmp, SHR_E_PARAM);
    SHR_NULL_CHECK(up_lport_bmp, SHR_E_PARAM);

    SHR_ALLOC(flex_param, sizeof(xgs_flex_param_t),
              "bcmaXgsUtilTrafficFlexPrm");
    SHR_NULL_CHECK(flex_param, SHR_E_MEMORY);
    sal_memset(flex_param, 0, sizeof(xgs_flex_param_t));

    /* Init */
    SHR_IF_ERR_EXIT
        (xgs_flex_init(unit, flex_param, lport_bmp, phy_pbmp_down,
                       phy_pbmp_up, num_pms, str_size,
                       &pm_mode_str[0]));

    /* Port down */
    SHR_IF_ERR_EXIT
        (xgs_flex_port_down_wrap(unit, flex_param));

    /* Port up */
    SHR_IF_ERR_EXIT
        (xgs_flex_port_up_wrap(unit, flex_param));

    /* Set return value */
    BCMDRD_PBMP_ASSIGN((*down_lport_bmp), flex_param->down_lport_bmp);
    BCMDRD_PBMP_ASSIGN((*up_lport_bmp), flex_param->up_lport_bmp);

exit:
    if (flex_param != NULL) {
        SHR_FREE(flex_param);
    }
    SHR_FUNC_EXIT();
}

static bcma_testutil_flex_traffic_op_t xgs_flex_traffic_op = {
    .flex_do_flex = xgs_flex_do_flex
};

bcma_testutil_flex_traffic_op_t *xgs_flex_traffic_op_get(int unit)
{
    return &xgs_flex_traffic_op;
}

