/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC utility routines for managing DPP headers
 *  ITMH: Ingress Traffic Management Header
 *  FTMH: Fabric Traffic Management Header
 *  OTMH: Outgoing Traffic Management Header
 */
#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_HEADERS

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <assert.h>
#include <sal/core/libc.h>

#include <soc/dpp/headers.h>
#include <soc/cm.h>
#include <soc/debug.h>

#define FIELD_GET_HI_LO(fld)  ((fld##_hi << 8) | (fld##_lo))

#define FIELD_SET_HI_LO(fld, val, hi_width)                             \
        do {                                                            \
            fld##_hi = ((val) & (((0x1<<(hi_width)) - 1) << 8)) >> 8;   \
            fld##_lo = ((val) & 0xff);                                  \
        } while (0);

static char *soc_dpp_itmh_field_names[] = {
    SOC_ITMH_FIELD_NAMES_INIT
};

static char *soc_dpp_ftmh_field_names[] = {
    SOC_FTMH_FIELD_NAMES_INIT
};

static char *soc_dpp_otmh_field_names[] = {
    SOC_OTMH_FIELD_NAMES_INIT
};

static char *soc_dpp_hdr_type_names[] = {
    SOC_DPP_HDR_NAMES_INIT
};


soc_dpp_itmh_field_t
soc_dpp_itmh_name_to_field(int unit, char *name)
{
    soc_dpp_itmh_field_t   field;
    COMPILER_REFERENCE(unit);
    
    assert((COUNTOF(soc_dpp_itmh_field_names) - 1) == ITMH_COUNT);
    
    for (field = 0; soc_dpp_itmh_field_names[field] != NULL; field++) {
        if (sal_strcmp(name, soc_dpp_itmh_field_names[field]) == 0) {
            return field;
        }
    }
    
    return ITMH_invalid;
}


char *
soc_dpp_itmh_field_to_name(int unit, soc_dpp_itmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert((COUNTOF(soc_dpp_itmh_field_names) - 1) == ITMH_COUNT);

    if (field < 0 || field >= ITMH_COUNT) {
        return "??";
    }

    return soc_dpp_itmh_field_names[field];
}


void
soc_dpp_itmh_field_set(int unit, soc_dpp_itmh_t *itmh, 
                       soc_dpp_itmh_field_t field, uint32 val)
{
    soc_dpp_itmh_base_t     *base;
    soc_dpp_itmh_sp_ext_t   *ext;

    COMPILER_REFERENCE(unit);
    if (itmh == NULL) {
        return;
    }
    base = &itmh->base;
    ext = &itmh->ext;

    switch (field) {
    case ITMH_version:
        if (val) {
           LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                       (BSL_META_U(unit,
                                   "Unit:%d itmh_set: Setting version to "
                                   "non-zero may result in un-defined behavior"), unit));
        }
        base->common.version = val;
        break;
    case ITMH_pph_present:
        base->common.pph_present = val;
        break;
    case ITMH_out_mirr_dis:
        base->common.out_mirr_dis = val;
        break;
    case ITMH_in_mirr_dis:
        base->common.in_mirr_dis = val;
        break;
    case ITMH_snoop_cmd:
        base->common.snoop_cmd = val;
        break;
    case ITMH_exclude_src:
        base->common.exclude_src = val;
        break;
    case ITMH_traffic_class:
        base->common.traffic_class = val;
        break;
    case ITMH_dp:
        base->common.dp = val;
        break;
    case ITMH_fwd_type:
        base->common.fwd_type = val;
        break;
    case ITMH_dest_sys_port:
        FIELD_SET_HI_LO(base->unicast_direct.dest_sys_port, val, 5);
        break;
    case ITMH_flow_id:
        FIELD_SET_HI_LO(base->unicast_flow.flow_id, val, 7);
        break;
    case ITMH_multicast_id:
        FIELD_SET_HI_LO(base->sys_multicast.multicast_id, val, 6);
        break;
    case ITMH_src_sys_port_flag:
        ext->system.src_sys_port_flag = val;
        break;
    case ITMH_src_sys_port:
        FIELD_SET_HI_LO(ext->system.src_sys_port, val, 5);
        break;
    case ITMH_src_local_port:
        ext->local.src_local_port = val;
        break;
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d itmh_set: Unknown ITMH field=%d "
                               "val=0x%x\n"), unit, field, val));
        break;
    }
}


uint32
soc_dpp_itmh_field_get(int unit, soc_dpp_itmh_t *itmh, 
                       soc_dpp_itmh_field_t field)
{
    soc_dpp_itmh_base_t     *base;
    soc_dpp_itmh_sp_ext_t   *ext;

    COMPILER_REFERENCE(unit);
    if (itmh == NULL) {
        return 0;
    }
    base = &itmh->base;
    ext = &itmh->ext;

    switch (field) {
    case ITMH_version:
        return (base->common.version);
    case ITMH_pph_present:
        return (base->common.pph_present);
    case ITMH_out_mirr_dis:
        return (base->common.out_mirr_dis);
    case ITMH_in_mirr_dis:
        return (base->common.in_mirr_dis);
    case ITMH_snoop_cmd:
        return (base->common.snoop_cmd);
    case ITMH_exclude_src:
        return (base->common.exclude_src);
    case ITMH_traffic_class:
        return (base->common.traffic_class);
    case ITMH_dp:
        return (base->common.dp);
    case ITMH_fwd_type:
        return (base->common.fwd_type);
    case ITMH_dest_sys_port:
        return (FIELD_GET_HI_LO(base->unicast_direct.dest_sys_port));
    case ITMH_flow_id:
        return (FIELD_GET_HI_LO(base->unicast_flow.flow_id));
    case ITMH_multicast_id:
        return (FIELD_GET_HI_LO(base->sys_multicast.multicast_id));
    case ITMH_src_sys_port_flag:
        return (ext->system.src_sys_port_flag);
    case ITMH_src_sys_port:
        return (FIELD_GET_HI_LO(ext->system.src_sys_port));
    case ITMH_src_local_port:
        return (ext->local.src_local_port);
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d itmh_get: Unknown ITMH field=%d \n"), 
                               unit, field));
        break;
    }

    return 0;
}


soc_dpp_ftmh_field_t
soc_dpp_ftmh_name_to_field(int unit, char *name)
{
    soc_dpp_ftmh_field_t   field;
    COMPILER_REFERENCE(unit);
    
    assert((COUNTOF(soc_dpp_ftmh_field_names) - 1) == FTMH_COUNT);
    
    for (field = 0; soc_dpp_ftmh_field_names[field] != NULL; field++) {
        if (sal_strcmp(name, soc_dpp_ftmh_field_names[field]) == 0) {
            return field;
        }
    }
    
    return FTMH_invalid;
}


char *
soc_dpp_ftmh_field_to_name(int unit, soc_dpp_ftmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert((COUNTOF(soc_dpp_ftmh_field_names) - 1) == FTMH_COUNT);

    if (field < 0 || field >= FTMH_COUNT) {
        return "??";
    }

    return soc_dpp_ftmh_field_names[field];
}


void
soc_dpp_ftmh_field_set(int unit, soc_dpp_ftmh_t *ftmh, 
                       soc_dpp_ftmh_field_t field, uint32 val)
{
    soc_dpp_ftmh_base_t         *base;
    soc_dpp_ftmh_outlif_ext_t   *outlif_ext;
    soc_dpp_ftmh_mc_lb_ext_t    *mc_lb_ext;

    COMPILER_REFERENCE(unit);
    if (ftmh == NULL) {
        return;
    }
    base = &ftmh->base;
    outlif_ext = &ftmh->outlif_ext;
    mc_lb_ext = &ftmh->mc_lb_ext;

    switch (field) {
    case FTMH_version:
        base->fields.version = val;
        break;
    case FTMH_packet_size:
        FIELD_SET_HI_LO(base->fields.packet_size, val, 6);
        break;
    case FTMH_traffic_class:
        base->fields.traffic_class = val;
        break;
    case FTMH_src_sys_port:
        FIELD_SET_HI_LO(base->fields.src_sys_port, val, 5);
        break;
    case FTMH_out_fap_port:
        base->fields.out_fap_port = val;
        break;
    case FTMH_dp:
        base->fields.dp = val;
        break;
    case FTMH_action_type:
        base->fields.action_type = val;
        break;
    case FTMH_pph_present:
        base->fields.pph_present = val;
        break;
    case FTMH_out_mirror_disable:
        base->fields.out_mirror_disable = val;
        break;
    case FTMH_exclude_src:
        base->fields.exclude_src = val;
        break;
    case FTMH_sys_mc:
        base->fields.sys_mc = val;
        break;
    case FTMH_egr_multicast_id:
        FIELD_SET_HI_LO(outlif_ext->multicast_id.egr_multicast_id, val, 6);
        break;
    case FTMH_cud:
        FIELD_SET_HI_LO(outlif_ext->multicast_cud.cud, val, 8);
        break;
    case FTMH_is_flow:
        outlif_ext->unicast_flow.is_flow = val;
        break;
    case FTMH_flow_id:
        outlif_ext->unicast_flow.is_flow = 1; 
        FIELD_SET_HI_LO(outlif_ext->unicast_flow.flow_id, val, 7);
        break;
    case FTMH_dest_port:
        outlif_ext->unicast_dest_port.is_flow = 0; 
        FIELD_SET_HI_LO(outlif_ext->unicast_dest_port.dest_port, val, 4);
        break;
    case FTMH_bmp_h:
        mc_lb_ext->fields.bmp_h = val;
        break;
    case FTMH_bmp_l:
        mc_lb_ext->fields.bmp_l = val;
        break;
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d ftmh_set: Unknown FTMH field=%d "
                               "val=0x%x\n"), unit, field, val));
        break;
    }
}


uint32
soc_dpp_ftmh_field_get(int unit, soc_dpp_ftmh_t *ftmh, 
                       soc_dpp_ftmh_field_t field)
{
    soc_dpp_ftmh_base_t         *base;
    soc_dpp_ftmh_outlif_ext_t   *outlif_ext;
    soc_dpp_ftmh_mc_lb_ext_t    *mc_lb_ext;

    COMPILER_REFERENCE(unit);
    if (ftmh == NULL) {
        return 0;
    }
    base = &ftmh->base;
    outlif_ext = &ftmh->outlif_ext;
    mc_lb_ext = &ftmh->mc_lb_ext;

    switch (field) {
    case FTMH_version:
        return (base->fields.version);
    case FTMH_packet_size:
        return (FIELD_GET_HI_LO(base->fields.packet_size));
    case FTMH_traffic_class:
        return (base->fields.traffic_class);
    case FTMH_src_sys_port:
        return (FIELD_GET_HI_LO(base->fields.src_sys_port));
    case FTMH_out_fap_port:
        return (base->fields.out_fap_port);
    case FTMH_dp:
        return (base->fields.dp);
    case FTMH_action_type:
        return (base->fields.action_type);
    case FTMH_pph_present:
        return (base->fields.pph_present);
    case FTMH_out_mirror_disable:
        return (base->fields.out_mirror_disable);
    case FTMH_exclude_src:
        return (base->fields.exclude_src);
    case FTMH_sys_mc:
        return (base->fields.sys_mc);
    case FTMH_egr_multicast_id:
        return (FIELD_GET_HI_LO(outlif_ext->multicast_id.egr_multicast_id));
    case FTMH_cud:
        return (FIELD_GET_HI_LO(outlif_ext->multicast_cud.cud));
    case FTMH_is_flow:
        return (outlif_ext->unicast_flow.is_flow);
    case FTMH_flow_id:
        return (FIELD_GET_HI_LO(outlif_ext->unicast_flow.flow_id));
    case FTMH_dest_port:
        return (FIELD_GET_HI_LO(outlif_ext->unicast_dest_port.dest_port));
    case FTMH_bmp_h:
        return (mc_lb_ext->fields.bmp_h);
    case FTMH_bmp_l:
        return (mc_lb_ext->fields.bmp_l);
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d ftmh_get: Unknown FTMH field=%d \n"), 
                               unit, field));
        break;
    }

    return 0;
}


soc_dpp_otmh_field_t
soc_dpp_otmh_name_to_field(int unit, char *name)
{
    soc_dpp_otmh_field_t   field;
    COMPILER_REFERENCE(unit);
    
    assert((COUNTOF(soc_dpp_otmh_field_names) - 1) == OTMH_COUNT);
    
    for (field = 0; soc_dpp_otmh_field_names[field] != NULL; field++) {
        if (sal_strcmp(name, soc_dpp_otmh_field_names[field]) == 0) {
            return field;
        }
    }
    
    return OTMH_invalid;
}


char *
soc_dpp_otmh_field_to_name(int unit, soc_dpp_otmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert((COUNTOF(soc_dpp_otmh_field_names) - 1) == OTMH_COUNT);

    if (field < 0 || field >= OTMH_COUNT) {
        return "??";
    }

    return soc_dpp_otmh_field_names[field];
}


void
soc_dpp_otmh_field_set(int unit, soc_dpp_otmh_t *otmh, 
                       soc_dpp_otmh_field_t field, uint32 val)
{
    soc_dpp_otmh_base_t         *base;
    soc_dpp_otmh_outlif_ext_t   *outlif_ext;
    soc_dpp_otmh_sp_ext_t       *sp_ext;
    soc_dpp_otmh_dp_ext_t       *dp_ext;

    COMPILER_REFERENCE(unit);
    if (otmh == NULL) {
        return;
    }
    base = &otmh->base;
    outlif_ext = &otmh->outlif_ext;
    sp_ext = &otmh->sp_ext;
    dp_ext = &otmh->dp_ext;

    switch (field) {
    case OTMH_version:
        base->fields.version = val;
        break;
    case OTMH_pph_present:
        base->fields.pph_present = val;
        break;
    case OTMH_action_type:
        base->fields.action_type = val;
        break;
    case OTMH_multicast:
        base->fields.multicast = val;
        break;
    case OTMH_dp:
        base->fields.dp = val;
        break;
    case OTMH_class:
        base->fields._class = val;
        break;
    case OTMH_cud_h:
        base->fields.cud_h = val;
        break;
    case OTMH_out_lif:
        FIELD_SET_HI_LO(outlif_ext->fields.out_lif, val, 8);
        break;
    case OTMH_src_sys_port:
        FIELD_SET_HI_LO(sp_ext->fields.src_sys_port, val, 5);
        break;
    case OTMH_dest_sys_port:
        FIELD_SET_HI_LO(dp_ext->fields.dest_sys_port, val, 5);
        break;
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d otmh_set: Unknown OTMH field=%d "
                               "val=0x%x\n"), unit, field, val));
        break;
    }
}


uint32
soc_dpp_otmh_field_get(int unit, soc_dpp_otmh_t *otmh, 
                       soc_dpp_otmh_field_t field)
{
    soc_dpp_otmh_base_t         *base;
    soc_dpp_otmh_outlif_ext_t   *outlif_ext;
    soc_dpp_otmh_sp_ext_t       *sp_ext;
    soc_dpp_otmh_dp_ext_t       *dp_ext;

    COMPILER_REFERENCE(unit);
    if (otmh == NULL) {
        return 0;
    }
    base = &otmh->base;
    outlif_ext = &otmh->outlif_ext;
    sp_ext = &otmh->sp_ext;
    dp_ext = &otmh->dp_ext;

    switch (field) {
    case OTMH_version:
        return (base->fields.version);
    case OTMH_pph_present:
        return (base->fields.pph_present);
    case OTMH_action_type:
        return (base->fields.action_type);
    case OTMH_multicast:
        return (base->fields.multicast);
    case OTMH_dp:
        return (base->fields.dp);
    case OTMH_class:
        return (base->fields._class);
    case OTMH_cud_h:
        return (base->fields.cud_h);
    case OTMH_out_lif:
        return (FIELD_GET_HI_LO(outlif_ext->fields.out_lif));
    case OTMH_src_sys_port:
        return (FIELD_GET_HI_LO(sp_ext->fields.src_sys_port));
    case OTMH_dest_sys_port:
        return (FIELD_GET_HI_LO(dp_ext->fields.dest_sys_port));
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d otmh_get: Unknown OTMH field=%d \n"), 
                               unit, field));
        break;
    }

    return 0;
}


int
soc_dpp_hdr_len_get(int unit, DPP_HDR_hdr_type_t type)
{
    switch (type) {
    case DPP_HDR_none:
        return 0;
    case DPP_HDR_itmh_base:
        return DPP_HDR_ITMH_BASE_LEN + 15; 
    case DPP_HDR_itmh_sp_ext:
        return DPP_HDR_ITMH_SP_EXT_LEN;
    case DPP_HDR_ftmh_base:
        return DPP_HDR_FTMH_BASE_LEN;
    case DPP_HDR_ftmh_outlif_ext:
        return DPP_HDR_FTMH_OUTLIF_EXT_LEN;
    case DPP_HDR_ftmh_mc_lb_ext:
        return DPP_HDR_FTMH_MC_LB_EXT_LEN;
    case DPP_HDR_ftmh_outlif_mc_lb_ext:
        return DPP_HDR_FTMH_OUTLIF_MC_LB_EXT_LEN;
    case DPP_HDR_otmh_base:
        return DPP_HDR_OTMH_BASE_LEN;
    case DPP_HDR_otmh_outlif_ext:
        return DPP_HDR_OTMH_OUTLIF_EXT_LEN;
    case DPP_HDR_otmh_sp_ext:
        return DPP_HDR_OTMH_SP_EXT_LEN;
    case DPP_HDR_otmh_dp_ext:
        return DPP_HDR_OTMH_DP_EXT_LEN;
    case DPP_HDR_otmh_outlif_sp_ext:
        return DPP_HDR_OTMH_OUTLIF_SP_EXT_LEN;
    case DPP_HDR_otmh_outlif_dp_ext:
        return DPP_HDR_OTMH_OUTLIF_DP_EXT_LEN;
    case DPP_HDR_otmh_outlif_sp_dp_ext:
        return DPP_HDR_OTMH_OUTLIF_SP_DP_EXT_LEN;
    default:
       LOG_VERBOSE(BSL_LS_SOC_HEADERS,
                   (BSL_META_U(unit,
                               "Unit:%d soc_dpp_hdr_len_get: Unknown header"
                               " type =%d \n"), unit, type));
        break;
    }

    return 0;
}


DPP_HDR_hdr_type_t
soc_dpp_hdr_type_get(int unit, char *name)
{
    DPP_HDR_hdr_type_t  type;
    COMPILER_REFERENCE(unit);
    
    assert((COUNTOF(soc_dpp_hdr_type_names) - 1) == DPP_HDR_COUNT);
    
    for (type = 0; soc_dpp_hdr_type_names[type] != NULL; type++) {
        if (sal_strcmp(name, soc_dpp_hdr_type_names[type]) == 0) {
            return type;
        }
    }
    
    return DPP_HDR_none;
}


char *
soc_dpp_hdr_type_to_name(int unit, DPP_HDR_hdr_type_t type)
{
    COMPILER_REFERENCE(unit);

    assert((COUNTOF(soc_dpp_hdr_type_names) - 1) == DPP_HDR_COUNT);

    if (type < 0 || type >= DPP_HDR_COUNT) {
        return "??";
    }

    return soc_dpp_hdr_type_names[type];
}
