/*! \file bcm5699x_flow.c
 *
 * BCM5699x Flow management driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/common/multicast.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>

#include <bcm_int/ltsw/mbcm/flow.h>
#include <bcm_int/ltsw/xgs/flow.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/flow.h>
#include <bcm_int/ltsw/flow_int.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/ha.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>



/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLOW

/*!
 * \brief Flow handle mapping.
 */
static bcmint_flow_handle_rec_t bcm5699x_ltsw_flow_handle_map[] = {
    {"CLASSIC_VXLAN", BCMINT_FLOW_ID_CLASSIC_VXLAN},
    {"VXLAN",         BCMINT_FLOW_ID_VXLAN},
    {"INT",           BCMINT_FLOW_ID_INT},
};

/*!
 * \brief Flow logical field mapping.
 */
static bcmint_flow_lf_map_rec_t bcm5699x_ltsw_flow_lf_map[] = {
    {"DEVICE_ID", BCMINT_FLOW_LF_ID_INT_DEVICE_ID, SWITCH_IDENTIFIERs},
};

/*!
 * \brief Flow option mapping.
 */
static bcmint_flow_op_map_rec_t bcm5699x_ltsw_flow_op_map[] = {
    {"LOOKUP_VNID_ASSIGN_VFI", BCMINT_FLOW_OPT_ID_LOOKUP_VNID_ASSIGN_VFI},
    {"LOOKUP_IPV6_SIP_ASSIGN_SVP", BCMINT_FLOW_OPT_ID_LOOKUP_IPV6_SIP_ASSIGN_SVP},
    {"LOOKUP_DVP_VFI_ASSIGN_VNID", BCMINT_FLOW_OPT_ID_LOOKUP_DVP_VFI_ASSIGN_VNID},
    {"LOOKUP_VFI_ASSIGN_VNID", BCMINT_FLOW_OPT_ID_LOOKUP_VFI_ASSIGN_VNID},
    {"IPV6_EVXLT_KEY_TYPE_VFI", BCMINT_FLOW_OPT_ID_IPV6_EVXLT_KEY_TYPE_VFI},
    {"IPV6", BCMINT_FLOW_OPT_ID_IPV6},
    {"LOOKUP_IPV6_DIP_TERM_TUNNEL", BCMINT_FLOW_OPT_ID_LOOKUP_IPV6_DIP_TERM_TUNNEL},
    {"IPV6_FLOW_LABEL_USE_ENTROPY", BCMINT_FLOW_OPT_ID_IPV6_FLOW_LABEL_USE_ENTROPY},
    {"ASSIGN_DEVICE_ID_TEMPLATE_ID", BCMINT_FLOW_OPT_ID_ASSIGN_DEVICE_ID_TEMPLATE_ID},
};

/*!
 * \brief Internal flow option ID to logical table mapping.
 */
static bcmint_flow_op_lt_map_rec_t bcm5699x_ltsw_flow_op_lt_map[] = {
    {BCMINT_FLOW_OPT_ID_ASSIGN_DEVICE_ID_TEMPLATE_ID, MON_INBAND_TELEMETRY_METADATA_FIELD_CONTROLs},
};

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Set the default configuration.
 *
 * \param [in]     unit          Unit Number.
 *
 * \retval SHR_E_NONE    No errors.
 * \retval !SHR_E_NONE   Failure.
 */
static int
bcm5699x_ltsw_flow_default_config_set(int unit)
{
    int dunit;
    bcmlt_entry_handle_t ent_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, TNL_L2_VXLAN_CONTROLs, &ent_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, DECAP_DST_L4_PORTs, 4789));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, FLAGSs, 0x08));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, DECAP_IPV4_PAYLOADs, 1));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, DECAP_IPV6_PAYLOADs, 1));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, DECAP_ARP_PAYLOADs, 1));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmlt_entry_field_add(ent_hdl, DECAP_RARP_PAYLOADs, 1));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, ent_hdl, BCMLT_PRIORITY_NORMAL));

exit:
    if (BCMLT_INVALID_HDL != ent_hdl) {
        (void)bcmlt_entry_free(ent_hdl);
    }
    SHR_FUNC_EXIT();
}


static int
bcm5699x_ltsw_flow_init(int unit)
{
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (!warm) {
       SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcm5699x_ltsw_flow_default_config_set(unit), SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_flow_init(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_flow_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_flow_detach(unit));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm5699x_ltsw_flow_mapping_info_get(int unit,
                                    bcmint_flow_mapping_info_type_t info_type,
                                    uint8_t **info_ptr,
                                    uint32_t *rec_num)
{
    SHR_FUNC_ENTER(unit);

    switch (info_type) {
        case INFO_SEL_HANDLE:
            *info_ptr = (uint8_t *)bcm5699x_ltsw_flow_handle_map;
            *rec_num =  COUNTOF(bcm5699x_ltsw_flow_handle_map);
            break;
        case INFO_SEL_OP:
            *info_ptr = (uint8_t *)bcm5699x_ltsw_flow_op_map;
            *rec_num =  COUNTOF(bcm5699x_ltsw_flow_op_map);
            break;
        case INFO_SEL_LOGICAL_FIELD:
            *info_ptr = (uint8_t *)bcm5699x_ltsw_flow_lf_map;
            *rec_num =  COUNTOF(bcm5699x_ltsw_flow_lf_map);
            break;
        case INFO_SEL_OP_LOGICAL_TABLE:
            *info_ptr = (uint8_t *)bcm5699x_ltsw_flow_op_lt_map;
            *rec_num =  COUNTOF(bcm5699x_ltsw_flow_op_lt_map);
            break;
        default:
            *info_ptr = NULL;
            *rec_num =  0;
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flow sub driver functions for bcm5699x.
 */
static mbcm_ltsw_flow_drv_t bcm5699x_flow_sub_drv = {
    .flow_init = bcm5699x_ltsw_flow_init,
    .flow_detach = bcm5699x_ltsw_flow_detach,
    .flow_mapping_info_get = bcm5699x_ltsw_flow_mapping_info_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_flow_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flow_drv_set(unit, &bcm5699x_flow_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
