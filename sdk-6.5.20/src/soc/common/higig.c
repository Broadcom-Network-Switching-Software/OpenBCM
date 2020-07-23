/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for managing Higig headers
 */

#include <shared/bsl.h>

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/types.h>
#include <soc/higig.h>
#include <soc/enet.h>
#include <soc/cm.h>
#include <soc/debug.h>

#if defined(BCM_XGS_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)

static char *soc_higig_field_names[] = {
    SOC_HIGIG_FIELD_NAMES_INIT
};

soc_higig_field_t
soc_higig_name_to_field(int unit, char *name)
{
    soc_higig_field_t		f;

    assert(COUNTOF(soc_higig_field_names) - 1 == HG_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_higig_field_names[f] != NULL; f++) {
	if (sal_strcmp(name, soc_higig_field_names[f]) == 0) {
	    return f;
	}
    }

    return HG_invalid;
}

char *
soc_higig_field_to_name(int unit, soc_higig_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_higig_field_names) == HG_COUNT);

    if (field < 0 || field >= HG_COUNT) {
	return "??";
    } else {
	return soc_higig_field_names[field];
    }
}

#ifdef BCM_HIGIG2_SUPPORT
void
soc_higig2_field_set(int unit, soc_higig2_hdr_t *hg2,
                      soc_higig_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
        /* FRC fields are the same for all HG2 overlays */
    case HG_start:
        hg2->ppd_overlay1.start = val;
        break;
    case HG_hgi:
        /* Silently accept */
        break;
    case HG_mcst:
        hg2->ppd_overlay1.mcst = val;
        break;
    case HG_cos:
    case HG_tc:
        hg2->ppd_overlay1.tc = val;
        break;
    case HG_dst_mod:
        hg2->ppd_overlay1.dst_mod = val;
        break;
    case HG_dst_port:
        hg2->ppd_overlay1.dst_port = val;
        break;
    case HG_l2mc_ptr:
    case HG_mgid:
        hg2->ppd_overlay1.dst_mod = val >> 8;
        hg2->ppd_overlay1.dst_port = val & 0xff;
	break;
    case HG_src_mod:
        hg2->ppd_overlay1.src_mod = val;
        break;
    case HG_src_port:
    case HG_tgid:
        hg2->ppd_overlay1.src_port = val;
        break;
    case HG_lbid:
        hg2->ppd_overlay1.lbid = val;
        break;
    case HG_cng:
    case HG_dp:
        hg2->ppd_overlay1.dp = val;
        break;
    case HG_ehv:
        hg2->ppd_overlay1.ehv = val;
        break;
    case HG_hdr_format:
    case HG_ppd_type:
        hg2->ppd_overlay1.ppd_type = val;
        break;

    /* All other elements may depend upon the PPD_TYPE, so set it first */
    /* PPD_TYPE = 0 */
    case HG_dst_t:
        hg2->ppd_overlay1.dst_t = val;
        break;
    case HG_ingress_tagged:
	hg2->ppd_overlay1.ingress_tagged = val;
	break;
    case HG_mirror_only:
	hg2->ppd_overlay1.mirror_only = val;
        break;
    case HG_mirror_done:
	hg2->ppd_overlay1.mirror_done = val;
        break;
    case HG_mirror:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            hg2->ppd_overlay1.mirror = val;
            break;
        case 2: 
            hg2->ppd_overlay3.mirror = val;
            break;
        default:
            break;
        }
        break;
     case HG_label_overlay_type:
        hg2->ppd_overlay1.label_overlay_type = val;
        break;
    case HG_l3:
        hg2->ppd_overlay1.l3 = val;
        break;
     case HG_label_present:
        hg2->ppd_overlay1.label_present = val;
        break;
    case HG_vc_label:
        hg2->ppd_overlay1.vc_label_19_16 = (val >> 16) & 0xF;
        hg2->ppd_overlay1.vc_label_15_8 = (val >> 8) & 0xFF;
        hg2->ppd_overlay1.vc_label_7_0 = (val >> 0) & 0xFF;
        break;
    case HG_replication_id:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            hg2->ppd_overlay1.vc_label_19_16 = (val >> 16) & 0x3;
            hg2->ppd_overlay1.vc_label_15_8 = (val >> 8) & 0xFF;
            hg2->ppd_overlay1.vc_label_7_0 = (val >> 0) & 0xFF;
            break;
        case 2: 
            hg2->ppd_overlay3.opcode = (val >> 16) & 0x3;
            hg2->ppd_overlay3.dst_vp_high = (val >> 8) & 0xFF;
            hg2->ppd_overlay3.dst_vp_low = (val >> 0) & 0xFF;
            break;
        default:
            break;
        }
        break;
    case HG_vlan_tag: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.vlan_pri = (val >> 13) & 7;
        hg2->ppd_overlay1.vlan_cfi = (val >> 12) & 1;
        hg2->ppd_overlay1.vlan_id_hi = (val >> 8) & 0xf;
        hg2->ppd_overlay1.vlan_id_lo = val & 0xff;
	break;
    case HG_vlan_pri: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.vlan_pri = val;
        break;
    case HG_vlan_cfi: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.vlan_cfi = val;
        break;
    case HG_vlan_id: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.vlan_id_hi = val >> 8;
        hg2->ppd_overlay1.vlan_id_lo = val & 0xff;
	break;
    case HG_pfm: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.pfm = val;
        break;
    case HG_src_t: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.src_t = val;
        break;
    case HG_preserve_dscp:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            hg2->ppd_overlay1.preserve_dscp = val;
            break;
        case 2: 
            hg2->ppd_overlay3.preserve_dscp = val;
            break;
        case 3: 
            hg2->ppd_overlay4.preserve_dscp = val;
            break;
        default:
            break;
        }
        break;
    case HG_preserve_dot1p:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            hg2->ppd_overlay1.preserve_dot1p = val;
            break;
        case 2: 
            hg2->ppd_overlay3.preserve_dot1p = val;
            break;
        case 3: 
            hg2->ppd_overlay4.preserve_dot1p = val;
            break;
        default:
            break;
        }
        break;
    case HG_opcode: /* Same posistion in all PPDs */
        hg2->ppd_overlay1.opcode = val;
        break;
    case HG_hdr_ext_len: /* Same posistion in PPD 0 & 1 */
        hg2->ppd_overlay1.hdr_ext_len = val;
        break;

    /* PPD_TYPE = 1 */
    case HG_ctag: /* Same posistion in PPD 1 & 3 (offload) */
        hg2->ppd_overlay2.ctag_hi = val >> 8;
        hg2->ppd_overlay2.ctag_lo = val & 0xff;
	break;

    /* PPD_TYPE = 2 */
    case HG_multipoint:
        hg2->ppd_overlay3.multipoint = val;
        break;
    case HG_fwd_type:
        hg2->ppd_overlay3.fwd_type = val;
        break;
    case HG_vni:
        hg2->ppd_overlay3.vni_high = (val >> 10) & 0xF;
        hg2->ppd_overlay3.vni_mid = (val >> 8) & 0x3;
        hg2->ppd_overlay3.vni_low = val & 0xff;
        break;
    case HG_dst_vp:
        hg2->ppd_overlay3.dst_vp_high = (val >> 8) & 0xFF;
        hg2->ppd_overlay3.dst_vp_low = (val >> 0) & 0xFF;
        break;
    case HG_src_vp: /* Same posistion in PPD 2 & 3 */
        hg2->ppd_overlay3.src_vp_high = (val >> 8) & 0xFF;
        hg2->ppd_overlay3.src_vp_low = (val >> 0) & 0xFF;
        break;
    case HG_donot_modify:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            hg2->ppd_overlay1.donot_modify = val;
            break;
        case 2:
            hg2->ppd_overlay3.donot_modify = val;
            break;
        default:
            break;
        }
        break;
    case HG_donot_learn:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            hg2->ppd_overlay1.donot_learn = val;
            break;
        case 2:
        case 3: /* Same posistion in PPD 2 & 3 */
            hg2->ppd_overlay3.donot_learn = val;
            break;
        default:
            break;
        }
        break;
    case HG_lag_failover:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            hg2->ppd_overlay1.lag_failover = val;
            break;
        case 2:
            hg2->ppd_overlay3.lag_failover = val;
            break;
        default:
            break;
        }
        break;
    case HG_protection_status:
        hg2->ppd_overlay3.protection_status = val;
        break;
    case HG_dst_type:
        hg2->ppd_overlay3.dest_type = val;
        break;
    case HG_src_type:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 2:
            hg2->ppd_overlay3.source_type = val;
            break;
        case 3:
            hg2->ppd_overlay4.source_type = val;
        default:
            break;
        }
        break;

    /* PPD_TYPE = 3 */
    case HG_data_container_type:
        hg2->ppd_overlay4.data_container_type = val;
        break;
    case HG_data_container:
        hg2->ppd_overlay4.data_container.abstract.bytes[0] =
            (val >> 24) & 0xFF;
        hg2->ppd_overlay4.data_container.abstract.bytes[1] =
            (val >> 16) & 0xFF;
        hg2->ppd_overlay4.data_container.abstract.bytes[2] =
            (val >> 8) & 0xFF;
        hg2->ppd_overlay4.data_container.abstract.bytes[3] =
            val & 0xFF;
        break;

    /* PPD_TYPE = 3, offload container */
    case HG_deferred_drop:
        hg2->ppd_overlay4.data_container.offload_engine.deferred_drop = val;
        break;
    case HG_vxlt_done:
        hg2->ppd_overlay4.data_container.offload_engine.vxlt_done = val;
        break;
    case HG_deferred_change_pkt_pri:
        hg2->ppd_overlay4.data_container.offload_engine.deferred_change_pkt_pri = val;
        break;
    case HG_new_pkt_pri:
        hg2->ppd_overlay4.data_container.offload_engine.new_pkt_pri = val;
        break;
    case HG_deferred_change_dscp:
        hg2->ppd_overlay4.data_container.offload_engine.deferred_change_dscp = val;
        break;
    case HG_new_dscp:
        hg2->ppd_overlay4.data_container.offload_engine.new_dscp = val;
        break;

    default:
	LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "hg_set: unit %d: Unknown higig2 field=%d val=0x%x\n"),
                  unit, field, val));
	break;
    }
}

uint32
soc_higig2_field_get(int unit, soc_higig2_hdr_t *hg2,
                      soc_higig_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case HG_start:
        return hg2->ppd_overlay1.start;
    case HG_hgi:
        return 2;
    case HG_mcst:
        return hg2->ppd_overlay1.mcst;
    case HG_cos:
    case HG_tc:
        return hg2->ppd_overlay1.tc;
    case HG_dst_mod:
        return hg2->ppd_overlay1.dst_mod;
    case HG_dst_port:
        return hg2->ppd_overlay1.dst_port;
    case HG_l2mc_ptr:
    case HG_mgid:
        return ((hg2->ppd_overlay1.dst_mod << 8) |
                (hg2->ppd_overlay1.dst_port));
    case HG_src_mod:
        return hg2->ppd_overlay1.src_mod;
    case HG_tgid:
    case HG_src_port:
        return hg2->ppd_overlay1.src_port;
    case HG_lbid:
        return hg2->ppd_overlay1.lbid;
    case HG_cng:
    case HG_dp:
        return hg2->ppd_overlay1.dp;
    case HG_ehv:
        return hg2->ppd_overlay1.ehv;
    case HG_hdr_format:
    case HG_ppd_type:
        return hg2->ppd_overlay1.ppd_type;

    /* PPD_TYPE = 0 */
    case HG_dst_t:
        return hg2->ppd_overlay1.dst_t;
    case HG_ingress_tagged:
	return hg2->ppd_overlay1.ingress_tagged;
    case HG_mirror_only:
	return hg2->ppd_overlay1.mirror_only;
    case HG_mirror_done:
	return hg2->ppd_overlay1.mirror_done;
    case HG_mirror:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            return hg2->ppd_overlay1.mirror;
        case 2: 
            return hg2->ppd_overlay3.mirror;
        default:
            return 0;
        }
    case HG_label_overlay_type:
        return hg2->ppd_overlay1.label_overlay_type;
    case HG_l3:
        return hg2->ppd_overlay1.l3;
    case HG_label_present:
        return hg2->ppd_overlay1.label_present;
    case HG_vc_label:
        return ((hg2->ppd_overlay1.vc_label_19_16 << 16) |
                (hg2->ppd_overlay1.vc_label_15_8 << 8) |
                (hg2->ppd_overlay1.vc_label_7_0)); 
    case HG_replication_id:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            return (((hg2->ppd_overlay1.vc_label_19_16 & 0x3) << 16) |
                    (hg2->ppd_overlay1.vc_label_15_8 << 8) |
                    (hg2->ppd_overlay1.vc_label_7_0)); 
        case 2: 
            return (((hg2->ppd_overlay3.opcode & 0x3) << 16) |
                    (hg2->ppd_overlay3.dst_vp_high << 8) |
                    (hg2->ppd_overlay3.dst_vp_low)); 
        default:
            return 0;
        }
    case HG_vlan_tag:
        return ((hg2->ppd_overlay1.vlan_pri << 13) |
                (hg2->ppd_overlay1.vlan_cfi << 12) |
                (hg2->ppd_overlay1.vlan_id_hi << 8) |
                (hg2->ppd_overlay1.vlan_id_lo));
    case HG_vlan_pri:
        return hg2->ppd_overlay1.vlan_pri;
    case HG_vlan_cfi:
        return hg2->ppd_overlay1.vlan_cfi;
    case HG_vlan_id:
        return ((hg2->ppd_overlay1.vlan_id_hi << 8) |
                (hg2->ppd_overlay1.vlan_id_lo));
    case HG_pfm:
        return hg2->ppd_overlay1.pfm;
    case HG_src_t:
        return hg2->ppd_overlay1.src_t;
    case HG_preserve_dscp:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            return hg2->ppd_overlay1.preserve_dscp;
        case 2: 
            return hg2->ppd_overlay3.preserve_dscp;
        case 3: 
            return hg2->ppd_overlay4.preserve_dscp;
        default:
            return 0;
        }
    case HG_preserve_dot1p:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0: 
            return hg2->ppd_overlay1.preserve_dot1p;
        case 2: 
            return hg2->ppd_overlay3.preserve_dot1p;
        case 3: 
            return hg2->ppd_overlay4.preserve_dot1p;
        default:
            return 0;
        }
   case HG_opcode:
        return hg2->ppd_overlay1.opcode;
    case HG_hdr_ext_len:
        return hg2->ppd_overlay1.hdr_ext_len;

    /* PPD_TYPE = 1 */
    case HG_ctag:
        return ((hg2->ppd_overlay2.ctag_hi << 8) |
                (hg2->ppd_overlay2.ctag_lo));

    /* PPD_TYPE = 2 */
    case HG_multipoint:
        return hg2->ppd_overlay3.multipoint;
    case HG_fwd_type:
        return hg2->ppd_overlay3.fwd_type;
    case HG_vni:
        return ((hg2->ppd_overlay3.vni_high << 10) |
                (hg2->ppd_overlay3.vni_mid << 8) |
                (hg2->ppd_overlay3.vni_low));
    case HG_dst_vp:
        return ((hg2->ppd_overlay3.dst_vp_high << 8) |
                (hg2->ppd_overlay3.dst_vp_low));
    case HG_src_vp:
        return ((hg2->ppd_overlay3.src_vp_high << 8) |
                (hg2->ppd_overlay3.src_vp_low));
    case HG_donot_modify:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            return hg2->ppd_overlay1.donot_modify;
        case 2:
            return hg2->ppd_overlay3.donot_modify;
        default:
            return 0;
        }
    case HG_donot_learn:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            return hg2->ppd_overlay1.donot_learn;
        case 2:
        case 3: /* Same posistion in PPD 2 & 3 */
            return hg2->ppd_overlay3.donot_learn;
        default:
            return 0;
        }
    case HG_lag_failover:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 0:
            return hg2->ppd_overlay1.lag_failover;
        case 2:
            return hg2->ppd_overlay3.lag_failover;
        default:
            return 0;
        }
    case HG_protection_status:
        return hg2->ppd_overlay3.protection_status;
    case HG_dst_type:
        return hg2->ppd_overlay3.dest_type;
    case HG_src_type:
        switch (hg2->ppd_overlay1.ppd_type) {
        case 2:
            return hg2->ppd_overlay3.source_type;
        case 3:
            return hg2->ppd_overlay4.source_type;
        default:
            return 0;
        }

    /* PPD_TYPE = 3 */
    case HG_data_container_type:
        return hg2->ppd_overlay4.data_container_type;
    case HG_data_container:
        return ((hg2->ppd_overlay4.data_container.abstract.bytes[0] << 24) |
                (hg2->ppd_overlay4.data_container.abstract.bytes[1] << 16) |
                (hg2->ppd_overlay4.data_container.abstract.bytes[2] << 8) |
                (hg2->ppd_overlay4.data_container.abstract.bytes[3]));

    /* PPD_TYPE = 3, offload container */
    case HG_deferred_drop:
        return hg2->ppd_overlay4.data_container.offload_engine.deferred_drop;
    case HG_vxlt_done:
        return hg2->ppd_overlay4.data_container.offload_engine.vxlt_done;
    case HG_deferred_change_pkt_pri:
        return hg2->ppd_overlay4.data_container.offload_engine.deferred_change_pkt_pri;
    case HG_new_pkt_pri:
        return hg2->ppd_overlay4.data_container.offload_engine.new_pkt_pri;
    case HG_deferred_change_dscp:
        return hg2->ppd_overlay4.data_container.offload_engine.deferred_change_dscp;
    case HG_new_dscp:
        return hg2->ppd_overlay4.data_container.offload_engine.new_dscp;
    default:
	LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "hg_get: unit %d: Unknown higig2 field=%d\n"),
                  unit, field));
	return 0;
    }
}

void
soc_higig2_dump(int unit, char *pfx, soc_higig2_hdr_t *hg2)
{
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<START=0x%x MCST=0x%x TC=%d DST_MOD=%d DST_PORT=%d\n"),
             pfx,
             hg2->overlay0.bytes[0],
             hg2->overlay0.bytes[1],
             hg2->overlay0.bytes[2],
             hg2->overlay0.bytes[3],
             soc_higig2_field_get(unit, hg2, HG_start),
             soc_higig2_field_get(unit, hg2, HG_mcst),
             soc_higig2_field_get(unit, hg2, HG_tc),
             soc_higig2_field_get(unit, hg2, HG_dst_mod),
             soc_higig2_field_get(unit, hg2, HG_dst_port)));
    LOG_CLI((BSL_META_U(unit,
                        "%s            OVERLAY: MGID=%d>\n"),
             pfx,
             soc_higig2_field_get(unit, hg2, HG_mgid)));
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<SRC_MOD=%d SRC_PORT=%d LBID=%d DP=%d EHV=%d PPD_TYPE=%d>\n"),
             pfx,
             hg2->overlay0.bytes[4],
             hg2->overlay0.bytes[5],
             hg2->overlay0.bytes[6],
             hg2->overlay0.bytes[7],
             soc_higig2_field_get(unit, hg2, HG_src_mod),
             soc_higig2_field_get(unit, hg2, HG_src_port),
             soc_higig2_field_get(unit, hg2, HG_lbid),
             soc_higig2_field_get(unit, hg2, HG_dp),
             soc_higig2_field_get(unit, hg2, HG_ehv),
             soc_higig2_field_get(unit, hg2, HG_ppd_type)));
    if (soc_higig2_field_get(unit, hg2, HG_ppd_type) == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<DST_T=%d DONOT_MODIFY=%d DONOT_LEARN=%d\n"),
                 pfx,
                 hg2->overlay0.bytes[8],
                 hg2->overlay0.bytes[9],
                 hg2->overlay0.bytes[10],
                 hg2->overlay0.bytes[11],
                 soc_higig2_field_get(unit, hg2, HG_dst_t),
                 soc_higig2_field_get(unit, hg2, HG_donot_modify),
                 soc_higig2_field_get(unit, hg2, HG_donot_learn)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             LEG_FAILOVER=%d IT=%d MO=%d MD=%d M=%d "
                 "L3=%d\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_lag_failover),
                 soc_higig2_field_get(unit, hg2, HG_ingress_tagged),
                 soc_higig2_field_get(unit, hg2, HG_mirror_only),
                 soc_higig2_field_get(unit, hg2, HG_mirror_done),
                 soc_higig2_field_get(unit, hg2, HG_mirror),
                 soc_higig2_field_get(unit, hg2, HG_l3)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            LP=%d LABEL_TYPE=%d VC_LABEL=0x%05x\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_label_present),
                 soc_higig2_field_get(unit, hg2, HG_label_overlay_type),
                 soc_higig2_field_get(unit, hg2, HG_vc_label)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            REPLICATION_ID=0x%x>\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_replication_id)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<VLAN_PRI=%d VLAN_CFI=%d VLAN_ID=%d\n"),
                 pfx,
                 hg2->overlay0.bytes[12],
                 hg2->overlay0.bytes[13],
                 hg2->overlay0.bytes[14],
                 hg2->overlay0.bytes[15],
                 soc_higig2_field_get(unit, hg2, HG_vlan_pri),
                 soc_higig2_field_get(unit, hg2, HG_vlan_cfi),
                 soc_higig2_field_get(unit, hg2, HG_vlan_id)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            PFM=%d SRC_T=%d PRESERVE_DSCP=%d\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_pfm),
                 soc_higig2_field_get(unit, hg2, HG_src_t),
                 soc_higig2_field_get(unit, hg2, HG_preserve_dscp)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            PRESERVE_DOT1P=%d OPCODE=%d HXL=%d>\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_preserve_dot1p),
                 soc_higig2_field_get(unit, hg2, HG_opcode),
                 soc_higig2_field_get(unit, hg2, HG_hdr_ext_len)));
    } else if (soc_higig2_field_get(unit, hg2, HG_ppd_type) == 1) {
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<CTAG=0x%x>\n"),
                 pfx,
                 hg2->overlay0.bytes[8],
                 hg2->overlay0.bytes[9],
                 hg2->overlay0.bytes[10],
                 hg2->overlay0.bytes[11],
                 soc_higig2_field_get(unit, hg2, HG_ctag)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<VLAN_PRI=%d VLAN_CFI=%d VLAN_ID=%d\n"),
                 pfx,
                 hg2->overlay0.bytes[12],
                 hg2->overlay0.bytes[13],
                 hg2->overlay0.bytes[14],
                 hg2->overlay0.bytes[15],
                 soc_higig2_field_get(unit, hg2, HG_vlan_pri),
                 soc_higig2_field_get(unit, hg2, HG_vlan_cfi),
                 soc_higig2_field_get(unit, hg2, HG_vlan_id)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            PFM=%d SRC_T=%d OPCODE=%d HXL=%d>\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_pfm),
                 soc_higig2_field_get(unit, hg2, HG_src_t),
                 soc_higig2_field_get(unit, hg2, HG_opcode),
                 soc_higig2_field_get(unit, hg2, HG_hdr_ext_len)));
    } else if (soc_higig2_field_get(unit, hg2, HG_ppd_type) == 2) {
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<MULTIPOINT=%d FWD_TYPE=0x%x VNI=0x%x DST_VP=0x%x>\n"),
                 pfx,
                 hg2->overlay0.bytes[8],
                 hg2->overlay0.bytes[9],
                 hg2->overlay0.bytes[10],
                 hg2->overlay0.bytes[11],
                 soc_higig2_field_get(unit, hg2, HG_multipoint),
                 soc_higig2_field_get(unit, hg2, HG_fwd_type),
                 soc_higig2_field_get(unit, hg2, HG_vni),
                 soc_higig2_field_get(unit, hg2, HG_dst_vp)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<SRC_VP=0x%x MIRROR=%d DONOT_MODIFY=%d DONOT_LEARN=%d\n"),
                 pfx,
                 hg2->overlay0.bytes[12],
                 hg2->overlay0.bytes[13],
                 hg2->overlay0.bytes[14],
                 hg2->overlay0.bytes[15],
                 soc_higig2_field_get(unit, hg2, HG_src_vp),
                 soc_higig2_field_get(unit, hg2, HG_mirror),
                 soc_higig2_field_get(unit, hg2, HG_donot_modify),
                 soc_higig2_field_get(unit, hg2, HG_donot_learn)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             LAG_FAILOVER=%d OPCODE=%d PRESERVE_DSCP=%d\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_lag_failover),
                 soc_higig2_field_get(unit, hg2, HG_opcode),
                 soc_higig2_field_get(unit, hg2, HG_preserve_dscp)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             REPLICATION_ID=0x%x PROTECTION_STATUS=%d\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_replication_id),
                 soc_higig2_field_get(unit, hg2, HG_protection_status)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             PRESERVE_DOT1P=%d DST_TYPE=%d SRC_TYPE=%d>\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_preserve_dot1p),
                 soc_higig2_field_get(unit, hg2, HG_dst_type),
                 soc_higig2_field_get(unit, hg2, HG_src_type)));
    } else if (soc_higig2_field_get(unit, hg2, HG_ppd_type) == 3) {
        if (soc_higig2_field_get(unit, hg2, HG_data_container_type) == 1) {
            LOG_CLI((BSL_META_U(unit,
                                "%s0x%02x%02x%02x%02x "
                     "<CTAG=0x%x DEFERRED_DROP=%d VXLT_DONE=%d\n"),
                     pfx,
                     hg2->overlay0.bytes[8],
                     hg2->overlay0.bytes[9],
                     hg2->overlay0.bytes[10],
                     hg2->overlay0.bytes[11],
                     soc_higig2_field_get(unit, hg2, HG_ctag),
                     soc_higig2_field_get(unit, hg2, HG_deferred_drop),
                     soc_higig2_field_get(unit, hg2, HG_vxlt_done)));
            LOG_CLI((BSL_META_U(unit,
                                "%s             DEFERRED_CHANGE_PKT_PRI=%d, NEW_PKT_PRI=%d\n"),
                     pfx,
                     soc_higig2_field_get(unit, hg2, HG_deferred_change_pkt_pri),
                     soc_higig2_field_get(unit, hg2, HG_new_pkt_pri)));
            LOG_CLI((BSL_META_U(unit,
                                "%s             DEFERRED_CHANGE_DSCP=%d, NEW_DSCP=%d>\n"),
                     pfx,
                     soc_higig2_field_get(unit, hg2, HG_deferred_change_dscp),
                     soc_higig2_field_get(unit, hg2, HG_new_dscp)));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s0x%02x%02x%02x%02x "
                     "<DATA_CONTAINER=0x%08x>\n"),
                     pfx,
                     hg2->overlay0.bytes[8],
                     hg2->overlay0.bytes[9],
                     hg2->overlay0.bytes[10],
                     hg2->overlay0.bytes[11],
                     soc_higig2_field_get(unit, hg2,
                     HG_data_container)));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<SOURCE_TYPE=%d SOURCE_VP=0x%x PRESERVE_DSCP=%d\n"),
                 pfx,
                 hg2->overlay0.bytes[12],
                 hg2->overlay0.bytes[13],
                 hg2->overlay0.bytes[14],
                 hg2->overlay0.bytes[15],
                 soc_higig2_field_get(unit, hg2, HG_src_type),
                 soc_higig2_field_get(unit, hg2, HG_src_vp),
                 soc_higig2_field_get(unit, hg2, HG_preserve_dscp)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             PRESERVE_DOT1P=%d DONOT_LEARN=%d\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_preserve_dot1p),
                 soc_higig2_field_get(unit, hg2, HG_donot_learn)));
        LOG_CLI((BSL_META_U(unit,
                            "%s             OPCODE=%d CONTAINER_TYPE=%d>\n"),
                 pfx,
                 soc_higig2_field_get(unit, hg2, HG_opcode),
                 soc_higig2_field_get(unit, hg2, HG_data_container_type)));
    }
}
#endif /* BCM_HIGIG2_SUPPORT */


/*
 *      HG_start should always be set first before settings any other fields. 
 */
void
soc_higig_field_set(int unit, soc_higig_hdr_t *hg,
		    soc_higig_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

#ifdef BCM_HIGIG2_SUPPORT
    if ((hg->overlay1.start == SOC_HIGIG2_START) ||
        (SOC_IS_RAPTOR(unit)) || (SOC_IS_RAVEN(unit))) {
        soc_higig2_field_set(unit, (soc_higig2_hdr_t *)hg, field, val);
        return;
    }
#endif /* BCM_HIGIG2_SUPPORT */

    switch (field) {
    case HG_start:		hg->overlay1.start = val;		break;
    case HG_hgi:		hg->overlay1.hgi = val & SOC_HIGIG_HGI_MASK; break;
    case HG_vlan_tag:		hg->overlay1.vlan_pri = (val >> 13) & 7;
				hg->overlay1.vlan_cfi = (val >> 12) & 1;
				hg->overlay1.vlan_id_hi = (val >> 8) & 0xf;
				hg->overlay1.vlan_id_lo = val & 0xff;	break;
    case HG_vlan_pri:		hg->overlay1.vlan_pri = val;		break;
    case HG_vlan_cfi:		hg->overlay1.vlan_cfi = val;		break;
    case HG_vlan_id:		hg->overlay1.vlan_id_hi = val >> 8;
				hg->overlay1.vlan_id_lo = val & 0xff;	break;
    case HG_src_mod:		hg->overlay1.src_mod = val & 0x1f;
                                hg->hgp_overlay1.src_mod_5 = (val >> 5) & 1;
                                hg->hgp_overlay1.src_mod_6 = (val >> 6) & 1;break;
    case HG_opcode:		hg->overlay1.opcode = val;		break;
    case HG_pfm:		hg->overlay1.pfm = val;			break;
    case HG_src_port:		hg->overlay1.src_port = val;		break;
    case HG_dst_port:		hg->overlay1.dst_port = val;		break;
    case HG_cos:		hg->overlay1.cos = val;			break;
    case HG_hdr_format:		hg->overlay1.hdr_format = val;		break;
    case HG_cng:		hg->overlay1.cng = val & 1;
                                hg->hgp_overlay1.cng_hi = (val >> 1) & 1;break;
    case HG_dst_mod:		hg->overlay1.dst_mod = val & 0x1f;
                                hg->hgp_overlay1.dst_mod_5 = (val >> 5) & 1;
                                hg->hgp_overlay1.dst_mod_6 = (val >> 6) & 1;break;
    case HG_dst_t:		hg->overlay1.dst_t = val;		break;
    case HG_dst_tgid:		hg->overlay1.dst_tgid = val;		break;
    case HG_donot_modify:       hg->hgp_overlay2.donot_modify = val;    break;
    case HG_donot_learn:        hg->hgp_overlay2.donot_learn = val;     break;
    case HG_lag_failover:       hg->hgp_overlay2.lag_failover = val;    break;
    case HG_ingress_tagged:	hg->overlay1.ingress_tagged = val;	break;
    case HG_mirror_only:	hg->overlay1.mirror_only = val;		break;
    case HG_mirror_done:	hg->overlay1.mirror_done = val;		break;
    case HG_mirror:		hg->overlay1.mirror = val;		break;
    case HG_tgid:		hg->overlay2.tgid = val;		break;
    case HG_l2mc_ptr:		hg->hgp_overlay2.dst_mod_6 = (val >> 12) & 1;
				hg->hgp_overlay2.dst_t = (val >> 11) & 1;
				hg->hgp_overlay2.dst_mod_5 = (val >> 10) & 1;
				hg->overlay2.l2mc_ptr_hi = (val >> 5) & 0x1f;
				hg->overlay2.l2mc_ptr_lo = val & 0x1f;	break;
    case HG_ctag:		hg->overlay2.ctag_hi = val >> 8;
				hg->overlay2.ctag_lo = val & 0xff;	break;
    case HG_hdr_ext_len:        hg->hgp_overlay1.hdr_ext_len = val; break;
    case HG_l3:                 hg->hgp_overlay1.l3 = val; break;
    case HG_label_present:      hg->hgp_overlay1.label_present = val; break;
    case HG_vc_label:           hg->hgp_overlay1.vc_label_19_16
                                                        = (val >> 16) & 0xF;
                                hg->hgp_overlay1.vc_label_15_8
                                                        = (val >> 8) & 0xFF;
                                hg->hgp_overlay1.vc_label_7_0
                                                        = (val >> 0) & 0xFF;
                                break;
    default:
	LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "hg_set: unit %d: Unknown higig field=%d val=0x%x\n"),
                  unit, field, val));
	break;
    }
}

uint32
soc_higig_field_get(int unit, soc_higig_hdr_t *hg,
		    soc_higig_field_t field)
{
    COMPILER_REFERENCE(unit);

#ifdef BCM_HIGIG2_SUPPORT
    if ((hg->overlay1.start == SOC_HIGIG2_START) ||
        (SOC_IS_RAPTOR(unit)) || (SOC_IS_RAVEN(unit))) {
        return soc_higig2_field_get(unit, (soc_higig2_hdr_t *)hg, field);
    }
#endif /* BCM_HIGIG2_SUPPORT */

    switch (field) {
    case HG_start:		return hg->overlay1.start;
    case HG_hgi:		return hg->overlay1.hgi & SOC_HIGIG_HGI_MASK;
    case HG_vlan_tag:		return (hg->overlay1.vlan_pri << 13 |
					hg->overlay1.vlan_cfi << 12 |
					hg->overlay1.vlan_id_hi << 8 |
					hg->overlay1.vlan_id_lo);
    case HG_vlan_pri:		return hg->overlay1.vlan_pri;
    case HG_vlan_cfi:		return hg->overlay1.vlan_cfi;
    case HG_vlan_id:		return (hg->overlay1.vlan_id_hi << 8 |
					hg->overlay1.vlan_id_lo);
    case HG_src_mod:		return (hg->overlay1.src_mod |
                                   (hg->hgp_overlay1.src_mod_5 << 5) |
                                   (hg->hgp_overlay1.src_mod_6 << 6));
    case HG_opcode:		return hg->overlay1.opcode;
    case HG_pfm:		return hg->overlay1.pfm;
    case HG_src_port:		return hg->overlay1.src_port;
    case HG_dst_port:		return hg->overlay1.dst_port;
    case HG_cos:		return hg->overlay1.cos;
    case HG_hdr_format:		return hg->overlay1.hdr_format;
    case HG_cng:		return hg->overlay1.cng |
                                   (hg->hgp_overlay1.cng_hi << 1);
    case HG_dst_mod:		return (hg->overlay1.dst_mod |
                                   (hg->hgp_overlay1.dst_mod_5 << 5) |
                                   (hg->hgp_overlay1.dst_mod_6 << 6));
    case HG_dst_t:		return hg->overlay1.dst_t;
    case HG_dst_tgid:		return hg->overlay1.dst_tgid;
    case HG_donot_modify:       return hg->hgp_overlay2.donot_modify;
    case HG_donot_learn:        return hg->hgp_overlay2.donot_learn;
    case HG_lag_failover:       return hg->hgp_overlay2.lag_failover;
    case HG_ingress_tagged:	return hg->overlay1.ingress_tagged;
    case HG_mirror_only:	return hg->overlay1.mirror_only;
    case HG_mirror_done:	return hg->overlay1.mirror_done;
    case HG_mirror:		return hg->overlay1.mirror;
    case HG_tgid:		return hg->overlay2.tgid;
    case HG_l2mc_ptr:		return (hg->hgp_overlay2.dst_mod_6 << 12 |
					hg->hgp_overlay2.dst_t << 11 |
					hg->hgp_overlay2.dst_mod_5 << 10 |
					hg->overlay2.l2mc_ptr_hi << 5 |
					hg->overlay2.l2mc_ptr_lo);
    case HG_ctag:		return (hg->overlay2.ctag_hi << 8 |
					hg->overlay2.ctag_lo);
    case HG_hdr_ext_len:        return hg->hgp_overlay1.hdr_ext_len;
    case HG_l3:                 return hg->hgp_overlay1.l3;
    case HG_label_present:      return hg->hgp_overlay1.label_present;
    case HG_vc_label:           return ((hg->hgp_overlay1.vc_label_19_16 << 16) |
                                        (hg->hgp_overlay1.vc_label_15_8 << 8) |
                                        (hg->hgp_overlay1.vc_label_7_0)); 
    default:
	LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "hg_get: unit %d: Unknown higig field=%d\n"),
                  unit, field));
	return 0;
    }
}

void
soc_higig_dump(int unit, char *pfx, soc_higig_hdr_t *hg)
{
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<START=0x%x HXL=%d "
                        "VLAN_PRI=%d VLAN_CFI=%d VLAN_ID=%d>\n"),
             pfx,
             hg->overlay0.bytes[0],
             hg->overlay0.bytes[1],
             hg->overlay0.bytes[2],
             hg->overlay0.bytes[3],
             soc_higig_field_get(unit, hg, HG_start),
             soc_higig_field_get(unit, hg, HG_hdr_ext_len),
             soc_higig_field_get(unit, hg, HG_vlan_pri),
             soc_higig_field_get(unit, hg, HG_vlan_cfi),
             soc_higig_field_get(unit, hg, HG_vlan_id)));
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<SRC_MOD=%d OPCODE=%d PFM=%d SRC_PORT=%d\n"),
             pfx,
             hg->overlay0.bytes[4],
             hg->overlay0.bytes[5],
             hg->overlay0.bytes[6],
             hg->overlay0.bytes[7],
             soc_higig_field_get(unit, hg, HG_src_mod),
             soc_higig_field_get(unit, hg, HG_opcode),
             soc_higig_field_get(unit, hg, HG_pfm),
             soc_higig_field_get(unit, hg, HG_src_port)));
    LOG_CLI((BSL_META_U(unit,
                        "%s            DST_PORT=%d COS=%d HDR_FMT=%d "
             "CNG=%d DST_MOD=%d\n"),
             pfx,
             soc_higig_field_get(unit, hg, HG_dst_port),
             soc_higig_field_get(unit, hg, HG_cos),
             soc_higig_field_get(unit, hg, HG_hdr_format),
             soc_higig_field_get(unit, hg, HG_cng),
             soc_higig_field_get(unit, hg, HG_dst_mod)));
    LOG_CLI((BSL_META_U(unit,
                        "%s            OVERLAY: TGID=%d L2MC_PTR=0x%x>\n"),
             pfx,
             soc_higig_field_get(unit, hg, HG_tgid),
             soc_higig_field_get(unit, hg, HG_l2mc_ptr)));
    LOG_CLI((BSL_META_U(unit,
                        "%s0x%02x%02x%02x%02x "
             "<DST_T=%d DST_TGID=%d IT=%d MO=%d MD=%d M=%d L3=%d\n"),
             pfx,
             hg->overlay0.bytes[8],
             hg->overlay0.bytes[9],
             hg->overlay0.bytes[10],
             hg->overlay0.bytes[11],
             soc_higig_field_get(unit, hg, HG_dst_t),
             soc_higig_field_get(unit, hg, HG_dst_tgid),
             soc_higig_field_get(unit, hg, HG_ingress_tagged),
             soc_higig_field_get(unit, hg, HG_mirror_only),
             soc_higig_field_get(unit, hg, HG_mirror_done),
             soc_higig_field_get(unit, hg, HG_mirror),
             soc_higig_field_get(unit, hg, HG_l3)));
    LOG_CLI((BSL_META_U(unit,
                        "%s            LP=%d VC_LABEL=0x%05x\n"),
             pfx,
             soc_higig_field_get(unit, hg, HG_label_present),
             soc_higig_field_get(unit, hg, HG_vc_label)));
    LOG_CLI((BSL_META_U(unit,
                        "%s            OVERLAY: CTAG=0x%x>\n"),
             pfx,
             soc_higig_field_get(unit, hg, HG_ctag)));
}


#endif /* BCM_XGS_SUPPORT */
