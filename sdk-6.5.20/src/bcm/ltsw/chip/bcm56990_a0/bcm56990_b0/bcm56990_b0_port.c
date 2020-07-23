/*! \file bcm56990_b0_port.c
 *
 * BCM56990_B0 Port subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/ltsw/port_int.h>
#include <bcm_int/ltsw/xgs/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/port.h>

#include <shr/shr_debug.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */
static const char* decap_key_mode[] = {
    OVID_DST_IPs,
    OVID_IPs,
};

static const ltsw_port_tab_sym_list_t decap_key_mode_map = {
    .len    = 2,
    .syms   = decap_key_mode,
};

static const char* vfi_key_mode[] = {
    OVID_VNIDs,
    OVID_VNID_SRC_IPs,
};

static const ltsw_port_tab_sym_list_t vfi_key_mode_map = {
    .len    = 2,
    .syms   = vfi_key_mode,
};

static const char* port_grp_mode[] = {
    ALL_ONESs,
    PORT_GRP_IDs,
};

static const ltsw_port_tab_sym_list_t port_grp_mode_map = {
    .len    = 2,
    .syms   = port_grp_mode,
};

static const ltsw_port_tab_lt_info_t port_system = {PORT_SYSTEMs, PORT_SYSTEM_IDs};
static const ltsw_port_tab_lt_info_t port = {PORTs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_lb = {PORT_LBs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_ing_ts_ptp = {PORT_ING_TS_PTPs, PORT_IDs};
static const ltsw_port_tab_lt_info_t port_policy = {PORT_POLICYs, PORT_IDs};

static const ltsw_port_tab_info_t port_tab_map_delta[BCMI_PT_CNT] = {
    [BCMI_PT_MPLS] = {&port_system, MPLSs, 0},
    [BCMI_PT_VXLAN_EN] = {&port_system, VXLAN_DECAPs, 0},
    [BCMI_PT_VXLAN_KEY_MODE] = {&port_system, VXLAN_DECAP_KEY_MODEs,
                                FLD_IS_SYMBOL, &decap_key_mode_map},
    [BCMI_PT_VXLAN_USE_PKT_OVID] = {&port_system, VXLAN_DECAP_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_VFI_KEY_MODE] = {&port_system, VXLAN_VFI_ASSIGN_KEY_MODEs,
                                    FLD_IS_SYMBOL, &vfi_key_mode_map},
    [BCMI_PT_VXLAN_VFI_USE_PKT_OVID] = {&port_system, VXLAN_VFI_ASSIGN_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_SVP_KEY_MODE] = {&port_system, VXLAN_SVP_ASSIGN_USE_PKT_OVIDs, 0},
    [BCMI_PT_VXLAN_SVP_DEFAULT_NETWORK] = {&port_system, ASSIGN_DEFAULT_NETWORK_SVPs, 0},

    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP] = {&port, VFI_EGR_ADAPT_PORT_GRP_LOOKUPs, 0},
    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP] = {&port, VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROPs, 0},
    [BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP] = {&port, VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUPs, 0},
    [BCMI_PT_VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROP] = {&port, VXLAN_VFI_EGR_ADAPT_PORT_GRP_LOOKUP_MISS_DROPs, 0},

    [BCMI_PT_VFI_EGR_ADAPT_PORT_GRP] = {&port, VFI_EGR_ADAPT_PORT_GRPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_PORT_GRP] = {&port_system, VFI_ING_ADAPT_PORT_GRPs, 0},

    [BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP] = {&port_system, VFI_ING_ADAPT_FIRST_LOOKUPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_FIRST_LOOKUP_PORT_GRP_MODE] = {&port_system, VFI_ING_ADAPT_FIRST_LOOKUP_PORT_GRP_MODEs,
                                                          FLD_IS_SYMBOL, &port_grp_mode_map},
    [BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP] = {&port_system, VFI_ING_ADAPT_SECOND_LOOKUPs, 0},
    [BCMI_PT_VFI_ING_ADAPT_SECOND_LOOKUP_PORT_GRP_MODE] = {&port_system, VFI_ING_ADAPT_SECOND_LOOKUP_PORT_GRP_MODEs,
                                                           FLD_IS_SYMBOL, &port_grp_mode_map},
    [BCMI_PT_OFFSET_ECMP_RANDOM] = {&port_lb, OFFSET_ECMP_LEVEL1_RANDOMs },
    [BCMI_PT_OFFSET_ECMP_LVL2_RANDOM] = {&port_lb, OFFSET_ECMP_LEVEL2_RANDOMs },
    [BCMI_PT_1588_LINK_DELAY] = {&port_ing_ts_ptp, LINK_DELAYs},
    [BCMI_PT_PASS_ON_PAYLOAD_OUTER_TPID_MATCH] = {&port_policy, PASS_ON_PAYLOAD_OUTER_TPID_MATCHs, FLD_IS_ARRAY },
    [BCMI_PT_DEFAULT_VFI] = {&port_system, VFI_IDs},
    [BCMI_PT_EGR_OPAQUE_TAG] = {&port, EGR_OPAQUE_TAGs }
};

static const ltsw_port_tab_info_t null_info = {NULL, NULL, 0};

/******************************************************************************
 * Private functions
 */
static int
bcm56990_b0_ltsw_port_tab_info_update(int unit)
{
    int type;

    SHR_FUNC_ENTER(unit);

    for (type = 0; type < BCMI_PT_CNT; type ++) {
        if (port_tab_map_delta[type].table) {
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_port_tab_info_update(unit, type, &port_tab_map_delta[type]));
        }
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_port_lport_tab_info_update(unit, BCMI_PT_MPLS, &null_info));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */
int
bcm56990_b0_port_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_b0_ltsw_port_tab_info_update(unit));

exit:
    SHR_FUNC_EXIT();
}

