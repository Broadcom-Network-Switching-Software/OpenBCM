/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     CMICX driver for EP_2_CPU_HDR
 */

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#include <soc/debug.h>
#include <soc/dma.h>
#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/dcbformats.h>
#include <soc/higig.h>

#ifdef BCM_CMICX_SUPPORT

#if defined(BCM_TRIDENT3_SUPPORT)
static void
soc_dma_ep_2_cpu_hdr36_decoded_dump(int unit, char *prefix, void *addr, int skip_mhdr, int hdr_size)
{
    uint8       *c;
    ep_to_cpu_hdr36_t *h = (ep_to_cpu_hdr36_t *)addr;

#if defined(LE_HOST)
    int         word;
    uint32 *hdr_data = (uint32 *)addr;
    for (word = 0; word < BYTES2WORDS(hdr_size); word++) {
        hdr_data[word] = _shr_swap32(hdr_data[word]);
    }
#endif
    c = (uint8 *)addr;

    if (!skip_mhdr) {
        soc_dma_higig_dump(unit, prefix, c, 0, 0, NULL);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%schg_tos %schg_ecn\n"),
             prefix,
             h->chg_tos ? "" : "!",
             h->word8.overlay1.chg_ecn ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sservice_tag %sswitch_pkt %suc_switch_drop %ssrc_hg\n"),
             prefix,
             h->service_tag ? "" : "!",
             h->switch_pkt ? "" : "!",
             h->uc_switch_drop ? "" : "!",
             h->src_hg ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sl3only %sreplicated %sdo_not_change_ttl "
             "%sbpdu\n"),
             prefix,
             h->l3only ? "" : "!",
             h->replicated ? "" : "!",
             h->do_not_change_ttl ? "" : "!",
             h->bpdu ? "" : "!"));

    switch (h->word16.overlay1.eh_type) {
        case 0:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 0: queue_tag=0x%04x "
                     "tag_type=%d seg_sel=%d\n"),
                     prefix,
                     h->word16.overlay1.eh_queue_tag,
                     h->word16.overlay1.eh_tag_type,
                     h->word16.overlay1.eh_seg_sel));
            break;
        case 1:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 1: classid=%d l3_iif=%d "
                     "classid_type=%d\n"),
                     prefix,
                     h->word16.overlay2.classid,
                     h->word16.overlay2.l3_iif,
                     h->word16.overlay2.classid_type));
            break;
        case 2:
            LOG_CLI((BSL_META_U(unit,
                                "%s\tHigig2 Extension type 2: queue_tag=0x%04x classid=%d "
                     "classid_type=%d\n"),
                     prefix,
                     h->word16.overlay3.eh_queue_tag,
                     h->word16.overlay3.classid,
                     h->word16.overlay3.classid_type));
            break;
        default:
            break;
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\treason=%08x_%08x\n"),
             prefix,
             h->reason_hi,
             h->reason));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp=%08x\n"),
             prefix,
             h->timestamp_type,
             h->timestamp));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d cpu_cos=%d pkt_len=%d forwarding_type=%d "
             "ucq=%05x\n"),
             prefix,
             h->word8.overlay1.srcport,
             h->word8.overlay1.cpu_cos,
             h->word8.overlay1.pkt_len,
             h->forwarding_type,
             h->ucq));

    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d otag_action=%d "
             "vntag_action=%d\n"),
             prefix,
             h->outer_vid,
             h->outer_cfi,
             h->word6.overlay1.outer_pri,
             h->otag_action,
             h->vntag_action));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tinner_vid=%d inner_cfi=%d inner_pri=%d "
             "decap = %d itag_action=%d\n"),
             prefix,
             h->inner_vid,
             h->word6.overlay1.inner_cfi,
             h->word6.overlay1.inner_pri,
             h->decap_tunnel_type,
             h->itag_action));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tdscp=%d ecn=%d decap_tunnel_type=%d match_rule=%d "
             "mtp_ind=%d forwarding_zone_id=%d\n"),
             prefix,
             h->dscp,
             h->word8.overlay1.ecn,
             h->decap_tunnel_type,
             h->match_rule,
             h->mtp_index,
             h->forwarding_zone_id));
}
#endif


#if defined(BCM_TOMAHAWK3_SUPPORT)
static void
soc_dma_ep_2_cpu_hdr38_decoded_dump(int unit, char *prefix, void *addr, int skip_mhdr, int hdr_size)
{
    uint8       *c;
    ep_to_cpu_hdr38_t *h = (ep_to_cpu_hdr38_t *)addr;

#if defined(LE_HOST)
    int         word;
    uint32 *hdr_data = (uint32 *)addr;
    for (word = 0; word < BYTES2WORDS(hdr_size); word++) {
        hdr_data[word] = _shr_swap32(hdr_data[word]);
    }
#endif
    c = (uint8 *)addr;

    if (!skip_mhdr) {
        soc_dma_higig_dump(unit, prefix, c, 0, 0, NULL);
    }

    LOG_CLI((BSL_META_U(unit,
                        "%s\treason=%08x_%08x\n"),
             prefix,
             h->reason_hi,
             h->reason));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tts_type=%d timestamp_hi=%08x timestamp=%08x\n"),
             prefix,
             h->timestamp_type,
             h->timestamp_hi,
             h->timestamp));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tdecap_tunnel_type=%d %suc_switch_drop %sucq\n"),
             prefix,
             h->decap_tunnel_type,
             h->uc_switch_drop ? "" : "!",
             h->ucq ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tsrcport=%d %sswitch_pkt %sspecial_pkt_indicator\n"),
             prefix,
             h->srcport,
             h->switch_pkt ? "" : "!",
             h->special_pkt_indicator ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tpkt_len=%d %sregen_crc replication_or_nhi=%d\n"),
             prefix,
             h->pkt_len,
             h->regen_crc ? "" : "!",
             h->replication_or_nhi));

    LOG_CLI((BSL_META_U(unit,
                        "%s\touter_vid=%d outer_cfi=%d outer_pri=%d mtp_index=%d "
             "match_rule=%d loopback_pkt_type=%d\n"),
             prefix,
             h->outer_vid,
             h->outer_cfi,
             h->outer_pri,
             h->mtp_index,
             h->match_rule,
             h->loopback_pkt_type));

    LOG_CLI((BSL_META_U(unit,
                        "%s\tcpu_opcode_type=%d dscp=%d ecn=%d tag_status=%d "
             "%sdo_not_change_ttl\n"),
             prefix,
             h->word3.overlay1.cpu_opcode_type,
             h->word3.overlay1.dscp,
             h->word3.overlay1.ecn,
             h->word3.overlay1.tag_status,
             h->word3.overlay1.do_not_change_ttl ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ting_l3_intf=%d otag_action=%d %sip_routed %sl3only\n"),
             prefix,
             h->word3.overlay1.ing_l3_intf,
             h->word3.overlay1.otag_action,
             h->word3.overlay1.ip_routed ? "" : "!",
             h->word3.overlay1.l3only ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\t%sbpdu %schg_dscp %schg_ecn\n"),
             prefix,
             h->word0.overlay1.bpdu ? "" : "!",
             h->word0.overlay1.chg_dscp ? "" : "!",
             h->word0.overlay1.chg_ecn ? "" : "!"));

    LOG_CLI((BSL_META_U(unit,
                        "%s\trx_bfd_session_index=%d rx_bfd_start_offset_type=%d rx_bfd_start_offset=%d "
             "queue_num=%d\n"),
             prefix,
             h->word0.overlay1.rx_bfd_session_index,
             h->word0.overlay1.rx_bfd_start_offset_type,
             h->word0.overlay1.rx_bfd_start_offset,
             h->word0.overlay1.queue_num));

    LOG_CLI((BSL_META_U(unit,
                        "%s\ti2e_classid=%d i2e_classid_type=%d "
             "cpu_cos=%d\n"),
             prefix,
             h->word0.overlay2.i2e_classid,
             h->word0.overlay2.i2e_classid_type,
             h->word0.overlay2.cpu_cos));
}
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
static void
soc_dma_ep_2_cpu_hdr40_decoded_dump(int unit, char *prefix, void *addr, int skip_mhdr, int hdr_size)
{
    uint8       *c;
    ep_to_cpu_hdr40_t *h = (ep_to_cpu_hdr40_t *)addr;

#if defined(LE_HOST)
    int         word;
    uint32 *hdr_data = (uint32 *)addr;
    for (word = 0; word < BYTES2WORDS(hdr_size); word++) {
        hdr_data[word] = _shr_swap32(hdr_data[word]);
    }
#endif
    c = (uint8 *)addr;

    if (!skip_mhdr) {
        soc_dma_higig_dump(unit, prefix, c, 0, 0, NULL);
    }

    LOG_CLI((BSL_META_U(unit,
        "%s  %sdo_not_change_ttl %sbpdu %sl3routed %schg_tos %semirror %simirror\n"),
        prefix,
        h->do_not_change_ttl ? "" : "!",
        h->bpdu ? "" : "!",
        h->l3routed ? "" : "!",
        h->chg_tos ? "" : "!",
        h->emirror ? "" : "!",
        h->imirror ? "" : "!"
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  %sreplicated %sl3only %soam_pkt %ssrc_hg\n"),
        prefix,
        h->replicated ? "" : "!",
        h->l3only ? "" : "!",
        h->oam_pkt ? "" : "!",
        h->src_hg ? "" : "!"
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  %sswitch_pkt %sregen_crc %sservice_tag %sing_untagged\n"),
        prefix,
        h->switch_pkt ? "" : "!",
        h->word6.overlay2.regen_crc ? "" : "!",
        h->service_tag ? "" : "!",
        !(((soc_higig2_hdr_t *)h)->ppd_overlay1.ingress_tagged) ? "" : "!"
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  %svfi_valid %schg_ecn ecn=%d\n"),
        prefix,
        h->vfi_valid ? "" : "!",
        h->chg_ecn ? "" : "!",
        h->ecn
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  cpu_cos=%d cos=%d mtp_index=%d reason=%08x_%08x\n"),
        prefix,
        h->word6.overlay2.cpu_cos,
        h->cos,
        h->mtp_index,
        h->reason_hi,
        h->reason
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  reason_type=%d match_rule=%d hg_type=%d mtp_index=%d\n"),
        prefix,
        h->word6.overlay1.cpu_opcode_type,
        h->match_rule,
        h->hg_type,
        h->mtp_index
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  srcport=%d dscp=%d outer_pri=%d outer_cfi=%d outer_vid=%d\n"),
        prefix,
        h->srcport,
        h->dscp,
        h->outer_pri,
        h->outer_cfi,
        h->word6.overlay1.outer_vid
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  inner_pri=%d inner_cfi=%d inner_vid=%d\n"),
        prefix,
        h->inner_pri,
        h->inner_cfi,
        h->inner_vid
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  hgi=%d itag_status=%d otag_action=%d itag_action=%d\n"),
        prefix,
        h->hgi,
        h->itag_status,
        h->otag_action,
        h->itag_action
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  repl_nhi=%05x ts_type=%d timestamp=%08x_%08x\n"),
        prefix,
        h->repl_nhi,
        h->timestamp_type,
        h->timestamp_hi,
        h->word2.overlay1.timestamp
        ));
    LOG_CLI((BSL_META_U(unit,
        "%s  vfi=%d em_mtp_index=%d vntag_action=%d\n"),
        prefix,
        h->word2.overlay2.vfi,
        h->em_mtp_index,
        h->vntag_action
        ));
    }
#endif

void soc_dma_ep_to_cpu_hdr_decoded_dump(int unit, char *pfx, void *addr,
                                int skip_mhdr, int hdr_size)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit)) {
        soc_dma_ep_2_cpu_hdr36_decoded_dump(unit, pfx, addr, skip_mhdr, hdr_size);
    }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        soc_dma_ep_2_cpu_hdr38_decoded_dump(unit, pfx, addr, skip_mhdr, hdr_size);
    }
#endif
#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        soc_dma_ep_2_cpu_hdr40_decoded_dump(unit, pfx, addr, skip_mhdr, hdr_size);
    }
#endif
}

#endif /* BCM_CMICX_SUPPORT */
