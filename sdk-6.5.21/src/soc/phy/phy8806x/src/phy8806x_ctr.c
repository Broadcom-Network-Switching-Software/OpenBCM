/*******************************************************************************
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/types.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/rx.h>
#include <bcm/field.h>
#include <bcm/udf.h>
#include <soc/phy/phyctrl.h>

#if defined(INCLUDE_PHY_8806X)

#include "phy8806x_funcs.h"
#include "phy8806x_ctr.h"
#include "../../../libs/phymod/chip/phy8806x/phy8806x_xmod_api.h"

extern bcm_rx_t ctr_rx_callback(int unit, bcm_pkt_t *info, void *cookie);
extern int inband_ctr_intercept(int unit);
extern uint8 * phy8806x_eth_ctr_get(phy_ctrl_t *pc);
extern void phy8806x_led_tx_set(phy_ctrl_t *pc, uint8 val);
extern void phy8806x_led_rx_set(phy_ctrl_t *pc, uint8 val);
#ifdef INCLUDE_FCMAP
extern int bcm_common_fcmap_private_data_set(int unit, bcm_port_t port, uint8 *data, int len);
#endif
extern int
phy_8806x_xmod_command(int unit, int port, int mode_opcode,  uint32 arg[], int arg_size, uint32 result[], int result_size);
extern int phy_is_8806x(int unit, phy_ctrl_t *pc);
extern void format_uint64_decimal(char *buf, uint64 n, int comma);

typedef uint32 bcm_field_udf_t;

/* #define USE_UDF_RULE */

#define MT2_CTR_PREV_OFF    (256 * 8)

#if defined(COMPILER_HAS_LONGLONG)
/* Byte swap a 64 bit value */
#define MT2SWAP64(val) \
    ((val) =  ( \
        (((val) & 0x00000000000000ffULL) << 56) | \
        (((val) & 0x000000000000ff00ULL) << 40) | \
        (((val) & 0x0000000000ff0000ULL) << 24) | \
        (((val) & 0x00000000ff000000ULL) <<  8) | \
        (((val) & 0x000000ff00000000ULL) >>  8) | \
        (((val) & 0x0000ff0000000000ULL) >> 24) | \
        (((val) & 0x00ff000000000000ULL) >> 40) | \
        (((val) & 0xff00000000000000ULL) >> 56) ))
#else
#define MT2SWAP64(val) \
    do {                                                \
        u64_H(val) = ((u64_L(val) & 0x000000ff) << 24) | \
                     ((u64_L(val) & 0x0000ff00) << 8) | \
                     ((u64_L(val) & 0x00ff0000) >> 8) | \
                     ((u64_L(val) & 0xff000000) >>24); \
        u64_L(val) = ((u64_H(val) & 0x000000ff) << 24) | \
                     ((u64_H(val) & 0x0000ff00) << 8) | \
                     ((u64_H(val) & 0x00ff0000) >> 8) | \
                     ((u64_H(val) & 0xff000000) >>24); \
    } while (0)
#endif

int mt2_eth_ctr_save(phy_ctrl_t *pc, uint8 *sptr, int len)
{
    int i, data_len;
    uint32_t *dst32, *src32;
    uint8 *dptr = phy8806x_eth_ctr_get(pc);
    int tlv_cnt = (int)sptr[42];
    uint8 *pkt_data = (sptr + 44); /* 1st tlv */
    int tot_data_len = 0;
#ifdef BE_HOST 
    uint64_t *dptr64 = (uint64_t *) dptr;
    uint64_t dat64;
#endif
    mt2_eth_ctr_t *eth_ctr = (mt2_eth_ctr_t *)dptr;
    uint64 prev_tpkt, prev_rpkt;
#ifdef INCLUDE_FCMAP
    mt2_fc_ctr_t *fc_ctr = (mt2_fc_ctr_t *)dptr;
    uint64 prev_fc_c2_txbyt, prev_fc_c3_txbyt, prev_fc_cf_txbyt;
    uint64 prev_fc_c2_rxbyt, prev_fc_c3_rxbyt, prev_fc_cf_rxbyt;
#endif
uint8 tx_active,rx_active;

COMPILER_64_ZERO(prev_tpkt);
COMPILER_64_ZERO(prev_rpkt);
#ifdef INCLUDE_FCMAP
COMPILER_64_ZERO(prev_fc_c2_txbyt);
COMPILER_64_ZERO(prev_fc_c3_txbyt);
COMPILER_64_ZERO(prev_fc_cf_txbyt);
COMPILER_64_ZERO(prev_fc_c2_rxbyt);
COMPILER_64_ZERO(prev_fc_c3_rxbyt);
COMPILER_64_ZERO(prev_fc_cf_rxbyt);
#endif

#ifdef INCLUDE_FCMAP
    if (pc->fcmap_enable){
        COMPILER_64_COPY(prev_fc_c2_txbyt, fc_ctr->fc_c2_txbyt);
        COMPILER_64_COPY(prev_fc_c3_txbyt, fc_ctr->fc_c3_txbyt);
        COMPILER_64_COPY(prev_fc_cf_txbyt, fc_ctr->fc_cf_txbyt);
        COMPILER_64_COPY(prev_fc_c2_rxbyt, fc_ctr->fc_c2_rxbyt);
        COMPILER_64_COPY(prev_fc_c3_rxbyt, fc_ctr->fc_c3_rxbyt);
        COMPILER_64_COPY(prev_fc_cf_rxbyt, fc_ctr->fc_cf_rxbyt);
    } else
#endif
    {
        COMPILER_64_COPY(prev_tpkt, eth_ctr->tpkt);
        COMPILER_64_COPY(prev_rpkt, eth_ctr->rpkt);
    }

    for (i=0; i<tlv_cnt; i++) {
        data_len = 0;
#ifdef INCLUDE_FCMAP
        if ((pkt_data[0] == 0) && (pkt_data[1] == 1)) { /* fc mib */
            data_len = sizeof(mt2_fc_ctr_t);
            /* Only Line side counters for now */
        } else  
#endif
        if ((pkt_data[0] == 0) && (pkt_data[1] == 3)) { /* eth mib */
            data_len = sizeof(mt2_eth_ctr_t) * 2;
            /* Both Line & Sys side counters */
        }
        pkt_data += 4;

        dst32 = (uint32 *) dptr;
        src32 = (uint32_t *) pkt_data;
        for (i = 0; i < (data_len/4); i++) {
            *dst32 = _shr_swap32(*src32);
            dst32++;
            src32++;
        }
        pkt_data += data_len;
        tot_data_len += data_len;
        dptr += data_len;
    }

#if BE_HOST 
    for (i=0; i<(tot_data_len/8); i++) {
        dat64 = *dptr64;
        MT2SWAP64(dat64);
        *dptr64 = dat64;
        dptr64++;
    }
#endif

/* Check Previous counter(s) and set flag */
#ifdef INCLUDE_FCMAP
    if (pc->fcmap_enable){
        tx_active = ( COMPILER_64_NE(prev_fc_c2_txbyt, fc_ctr->fc_c2_txbyt) ||
                      COMPILER_64_NE(prev_fc_c3_txbyt, fc_ctr->fc_c3_txbyt) ||
                      COMPILER_64_NE(prev_fc_cf_txbyt, fc_ctr->fc_cf_txbyt) )
            ? 1 : 0;
        rx_active = ( COMPILER_64_NE(prev_fc_c2_rxbyt, fc_ctr->fc_c2_rxbyt) ||
                      COMPILER_64_NE(prev_fc_c3_rxbyt, fc_ctr->fc_c3_rxbyt) ||
                      COMPILER_64_NE(prev_fc_cf_rxbyt, fc_ctr->fc_cf_rxbyt) )
            ? 1 : 0;
    } else
#endif
    {
        tx_active = COMPILER_64_NE(prev_tpkt, eth_ctr->tpkt) ? 1 : 0;
        rx_active = COMPILER_64_NE(prev_rpkt, eth_ctr->rpkt) ? 1 : 0;
    }

    phy8806x_led_tx_set(pc, tx_active);
    phy8806x_led_rx_set(pc, rx_active);

    return SOC_E_NONE;
}

STATIC bcm_rx_t mt2_ctr_rx_callback(int unit, bcm_pkt_t *info, void *cookie)
{
    uint8 *pkt_data;
    int    data_len;
    phy_ctrl_t *pc;
#ifdef INCLUDE_FCMAP
    int rv = 0;
#endif

    pkt_data = BCM_PKT_DMAC(info); /* Pointer to Destination MAC */
    pkt_data += 12;
    data_len = info->tot_len - 12; 

    if (BCM_PKT_TAG_PROTOCOL(info) == 0x8100) { 
        pkt_data += 4;
        data_len -= 4;
    }

    if ((pkt_data[0] == 0x89) && (pkt_data[1] == 0x06) && (pkt_data[16] == 0xe4))
    {
        pc = EXT_PHY_SW_STATE(info->rx_unit, info->rx_port);
        if (pc == NULL) {
            return BCM_RX_HANDLED;
        }
        mt2_eth_ctr_save(pc, pkt_data, data_len);
#ifdef INCLUDE_FCMAP
        /* If Not Eth mibs, then also send it to fcmap driver to process */
        if (!((pkt_data[44] == 0) && (pkt_data[45] == 3))) {
            pkt_data += 16 + 24;
            data_len -= 16 + 24 + 4 + 4; /* 16 = FCoE, 24 = FC header, 4 = FC CRC, 4 = EOF + padding */
/*            cli_out("CTR packet from unit=%d port=%d pkt_len=%d tot_len=%d payload_len = %d\n",
                        info->rx_unit, info->rx_port, info->pkt_len, info->tot_len, data_len);
*/
            if ((rv = bcm_common_fcmap_private_data_set(info->rx_unit, info->rx_port, pkt_data, data_len)) < 0) {
                cli_out("u=%d p=%d call to bcm_common_fcmap_private_data_set() failed. rv = %d\n",
                info->rx_unit, info->rx_port, rv);
            }
        }
#endif /* INCLUDE_FCMAP */
        return BCM_RX_HANDLED;
    }
    return BCM_RX_NOT_HANDLED;
}

 
STATIC int mt2_inband_ctr_intercept(int unit)
{
#ifndef USE_UDF_RULE
    int qual_id;
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    bcm_field_action_t action;
    bcm_field_data_qualifier_t data_qualifier;
    bcm_field_data_packet_format_t packet_format;
    uint8 data = 0xe4;
    uint8 mask = 0xff;
    int length = sizeof(data);
    bcm_field_stat_t stat_entry = bcmFieldStatPackets;
    int stat_id;

    action = bcmFieldActionCopyToCpu;
    BCM_FIELD_QSET_INIT(qset);

    bcm_field_data_qualifier_t_init(&data_qualifier);

    data_qualifier.offset_base = bcmFieldDataOffsetBaseFcoeHeader;
    data_qualifier.offset = 14;
    data_qualifier.length = 1;

    BCM_IF_ERROR_RETURN(
        bcm_field_data_qualifier_create(unit, &data_qualifier));
    qual_id = data_qualifier.qual_id;


    bcm_field_data_packet_format_t_init(&packet_format);

    packet_format.relative_offset = 0;
    packet_format.l2 = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
    packet_format.vlan_tag = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
    packet_format.tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    packet_format.fibre_chan_outer = BCM_FIELD_DATA_FORMAT_FIBRE_CHAN;

    BCM_IF_ERROR_RETURN(
        bcm_field_data_qualifier_packet_format_add(unit, qual_id, &packet_format));

    BCM_IF_ERROR_RETURN(
        bcm_field_qset_data_qualifier_add(unit, &qset, qual_id));
    BCM_IF_ERROR_RETURN(
        bcm_field_group_create(unit, qset, 1, &group));
    BCM_IF_ERROR_RETURN(
        bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN(
        bcm_field_qualify_data(unit, entry, qual_id, &data, &mask, length));
    BCM_IF_ERROR_RETURN(
        bcm_field_action_add(unit, entry, action, 0, 0));

    action = bcmFieldActionDrop;
    BCM_IF_ERROR_RETURN(
        bcm_field_action_add(unit, entry, action, 0, 0));
    BCM_IF_ERROR_RETURN(
        bcm_field_stat_create(0, group, 1, &stat_entry, &stat_id));
    BCM_IF_ERROR_RETURN(
        bcm_field_entry_stat_attach(0, entry, stat_id));
    cli_out("EID:%d STATID:%d\n", entry, stat_id);
    BCM_IF_ERROR_RETURN(
        bcm_field_entry_install(unit, entry));
#else
    int start_offset = 14;
    int match_width = 1;
    uint8 DATA[1]={0xe4};
    uint8 MASK[1]={0xff};
    bcm_udf_alloc_hints_t hints;
    bcm_udf_t udf_info;
    bcm_udf_id_t udf_id;
    bcm_udf_pkt_format_info_t pkt_format;
    bcm_udf_pkt_format_id_t pkt_format_id;

    /* Allocate field group entry */
    bcm_field_qset_t qset;
    bcm_field_group_t group;
    bcm_field_entry_t entry;
    int stat_id;
    bcm_field_stat_t stat_arr[1];
    stat_arr[0] = bcmFieldStatPackets;
    uint64 stat_value = 0;

    /* Hints For UDF Allocation */
    bcm_udf_alloc_hints_t_init(&hints);
    hints.flags |= BCM_UDF_CREATE_O_FIELD_INGRESS;
    BCM_FIELD_QSET_INIT(hints.qset);
    BCM_FIELD_QSET_ADD(hints.qset, bcmFieldQualifyStageIngress);

    /* UDF Offset Allocation */
    bcm_udf_t_init(&udf_info);
    udf_info.layer = bcmUdfLayerL3OuterHeader;
    udf_info.start = (start_offset * 8); 
    udf_info.width = (match_width * 8); 


    BCM_IF_ERROR_RETURN( 
        bcm_udf_create(unit, &hints, &udf_info, &udf_id));

    /* UDF Packet Format */
    bcm_udf_pkt_format_info_t_init(&pkt_format);

    pkt_format.l2 = BCM_PKT_FORMAT_L2_ETH_II;
    pkt_format.vlan_tag = BCM_PKT_FORMAT_VLAN_TAG_SINGLE;
    pkt_format.tunnel = BCM_PKT_FORMAT_TUNNEL_NONE;
    pkt_format.fibre_chan_outer = BCM_PKT_FORMAT_FIBRE_CHAN;

    BCM_IF_ERROR_RETURN( 
        bcm_udf_pkt_format_create(unit,BCM_UDF_PKT_FORMAT_CREATE_O_NONE,&pkt_format,&pkt_format_id));

    /* Adds packet format specification to the UDF object */
    BCM_IF_ERROR_RETURN( 
        bcm_udf_pkt_format_add(unit,udf_id,pkt_format_id));

    /* Configure data qualifier ID in the qset structure */
    BCM_FIELD_QSET_INIT(qset);
    BCM_IF_ERROR_RETURN( 
        bcm_field_qset_id_multi_set(unit, bcmFieldQualifyUdf, 1, &udf_id, &qset));
    BCM_IF_ERROR_RETURN( 
        bcm_field_group_create_mode(unit, qset, 0, bcmFieldGroupModeAuto, &group));
    BCM_IF_ERROR_RETURN( 
        bcm_field_entry_create(unit, group, &entry));
    BCM_IF_ERROR_RETURN( 
        bcm_field_qualify_clear(unit, entry));
    BCM_IF_ERROR_RETURN( 
        bcm_field_qualify_udf(unit, entry, udf_id, match_width, DATA, MASK));
    BCM_IF_ERROR_RETURN( 
        bcm_field_action_add(unit, entry, bcmFieldActionCopyToCpu,0x0,0x0));
    BCM_IF_ERROR_RETURN( 
        bcm_field_stat_create(unit,group,1,stat_arr,&stat_id));

    BCM_IF_ERROR_RETURN( 
        bcm_field_entry_stat_attach(unit,entry,stat_id));
    BCM_IF_ERROR_RETURN( 
        bcm_field_entry_prio_set(unit,entry,BCM_FIELD_ENTRY_PRIO_HIGHEST));
    BCM_IF_ERROR_RETURN( 
        bcm_field_entry_install(unit,entry));

    cli_out("MT2 counters using UDF\n");

    BCM_IF_ERROR_RETURN( 
        bcm_field_stat_get(unit,stat_id,bcmFieldStatPackets,&stat_value));

#endif

    return BCM_E_NONE;
}


STATIC int mt2_ctr_rx_init(int unit)
{
    int rv = SOC_E_NONE;
    bcm_rx_cfg_t rx_cfg;

    if ( !bcm_rx_active( unit ) ) {
        /* Get current configuration */
        bcm_rx_cfg_init(unit);   /* Re-init in case in bad state */
        bcm_rx_cfg_get(unit, &rx_cfg);
        rx_cfg.global_pps = BCM_RX_RATE_NOLIMIT;
        rx_cfg.chan_cfg[BCM_RX_CHAN_DFLT].rate_pps = BCM_RX_RATE_NOLIMIT;
        rv = bcm_rx_start(unit, &rx_cfg);
        if (BCM_E_NONE != rv) {
            cli_out("mt2_ctr_rx_init:" "rx_start failed: %s\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Register handler */
    rv = bcm_rx_register(unit, "MT2 STAT Diag", mt2_ctr_rx_callback,
                         BCM_RX_PRIO_MAX, (void *)&unit,
                         BCM_RCO_F_ALL_COS);

    if (BCM_E_NONE != rv) {
        cli_out("ctr_rx_init:"
                "rx_register failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /*  All FCoE based ctr packets go to cpu */
    cli_out("Mt2 Ctrs handled by CPU\n");
    if (mt2_inband_ctr_intercept(unit) != BCM_E_NONE) {
        cli_out("Failed to add ctr interception rules....\n");
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}


STATIC int mt2_ctr_rx_deinit(int unit)
{
    int rv = SOC_E_NONE;

    rv = bcm_rx_stop(unit, NULL);
    if (BCM_E_NONE != rv) {
        cli_out("mt2_ctr_rx_deinit:" "rx_stop failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* Unregister handler */
    if ((rv = bcm_rx_unregister(unit, mt2_ctr_rx_callback, BCM_RX_PRIO_MAX)) < 0) {
        cli_out("mt2_ctr_rx_deinit: bcm_rx_unregister failed: %s\n",
                bcm_errmsg(rv));
        return SOC_E_FAIL;
    }

    return SOC_E_NONE;
}

STATIC void mt2_ctr_validate_show(char *name, char *sfx, uint64 data, uint64 odata, uint32 flags)
{
    uint64 diff;
    char        buf_val[32], buf_diff[32];

    if (COMPILER_64_IS_ZERO(data)) {
        if (!(flags & MT2_CTR_ZERO)) {
            return;
        }
    } else {
        if (!(flags & MT2_CTR_NONZERO)) {
            return;
        }
    }

    diff = data;
    COMPILER_64_SUB_64(diff, odata);
    if (COMPILER_64_IS_ZERO(diff)) {
        if (!(flags & MT2_CTR_SAME)) {
            return;
        }
    } else {
        if (!(flags & MT2_CTR_CHANGED)) {
            return;
        }
    }
    if (flags & MT2_CTR_HEX) {
        cli_out("%s.%s = 0x%08x%08x  (+0x%08x%08x)\n", name, sfx, COMPILER_64_HI(data), COMPILER_64_LO(data), COMPILER_64_HI(diff), COMPILER_64_LO(diff));
    } else {
        format_uint64_decimal(buf_val, data, (int)',');
        format_uint64_decimal(buf_diff, diff, (int)',');
        cli_out("%s.%s = %s  (+%s)\n", name, sfx, buf_val, buf_diff);
    }
    return;
}

STATIC int mt2_eth_ctr_show(mt2_eth_ctr_t *data, mt2_eth_ctr_t *odata, char *sfx, uint32 flags)
{
    mt2_ctr_validate_show("R64", sfx, data->r64, odata->r64, flags);
    mt2_ctr_validate_show("R127", sfx, data->r127, odata->r127, flags);
    mt2_ctr_validate_show("R255", sfx, data->r255, odata->r255, flags);
    mt2_ctr_validate_show("R511", sfx, data->r511, odata->r511, flags);
    mt2_ctr_validate_show("R1023", sfx, data->r1023, odata->r1023, flags);
    mt2_ctr_validate_show("R1518", sfx, data->r1518, odata->r1518, flags);
    mt2_ctr_validate_show("RMGV", sfx, data->rmgv, odata->rmgv, flags);
    mt2_ctr_validate_show("R2047", sfx, data->r2047, odata->r2047, flags);
    mt2_ctr_validate_show("R4095", sfx, data->r4095, odata->r4095, flags);
    mt2_ctr_validate_show("R9216", sfx, data->r9216, odata->r9216, flags);
    mt2_ctr_validate_show("R16383", sfx, data->r16383, odata->r16383, flags);
    mt2_ctr_validate_show("RPKT", sfx, data->rpkt, odata->rpkt, flags);
    mt2_ctr_validate_show("RUCA", sfx, data->ruca, odata->ruca, flags);
    mt2_ctr_validate_show("RMCA", sfx, data->rmca, odata->rmca, flags);
    mt2_ctr_validate_show("RBCA", sfx, data->rbca, odata->rbca, flags);
    mt2_ctr_validate_show("RFCS", sfx, data->rfcs, odata->rfcs, flags);
    mt2_ctr_validate_show("RXCF", sfx, data->rxcf, odata->rxcf, flags);
    mt2_ctr_validate_show("RXPF", sfx, data->rxpf, odata->rxpf, flags);
    mt2_ctr_validate_show("RXPP", sfx, data->rxpp, odata->rxpp, flags);
    mt2_ctr_validate_show("RXUO", sfx, data->rxuo, odata->rxuo, flags);
    mt2_ctr_validate_show("RXUDA", sfx, data->rxuda, odata->rxuda, flags);
    mt2_ctr_validate_show("RXWSA", sfx, data->rxwsa, odata->rxwsa, flags);
    mt2_ctr_validate_show("RALN", sfx, data->raln, odata->raln, flags);
    mt2_ctr_validate_show("RFLR", sfx, data->rflr, odata->rflr, flags);
    mt2_ctr_validate_show("RERPKT", sfx, data->rerpkt, odata->rerpkt, flags);
    mt2_ctr_validate_show("RFCR", sfx, data->rfcr, odata->rfcr, flags);
    mt2_ctr_validate_show("ROVR", sfx, data->rovr, odata->rovr, flags);
    mt2_ctr_validate_show("RJBR", sfx, data->rjbr, odata->rjbr, flags);
    mt2_ctr_validate_show("RMTUE", sfx, data->rmtue, odata->rmtue, flags);
    mt2_ctr_validate_show("RMCRC", sfx, data->rmcrc, odata->rmcrc, flags);
    mt2_ctr_validate_show("RPRM", sfx, data->rprm, odata->rprm, flags);
    mt2_ctr_validate_show("RVLN", sfx, data->rvln, odata->rvln, flags);
    mt2_ctr_validate_show("RDVLN", sfx, data->rdvln, odata->rdvln, flags);
    mt2_ctr_validate_show("RTRFU", sfx, data->rtrfu, odata->rtrfu, flags);
    mt2_ctr_validate_show("RPOK", sfx, data->rpok, odata->rpok, flags);
    mt2_ctr_validate_show("RPFCOFF0", sfx, data->rpfcoff0, odata->rpfcoff0, flags);
    mt2_ctr_validate_show("RPFCOFF1", sfx, data->rpfcoff1, odata->rpfcoff1, flags);
    mt2_ctr_validate_show("RPFCOFF2", sfx, data->rpfcoff2, odata->rpfcoff2, flags);
    mt2_ctr_validate_show("RPFCOFF3", sfx, data->rpfcoff3, odata->rpfcoff3, flags);
    mt2_ctr_validate_show("RPFCOFF4", sfx, data->rpfcoff4, odata->rpfcoff4, flags);
    mt2_ctr_validate_show("RPFCOFF5", sfx, data->rpfcoff5, odata->rpfcoff5, flags);
    mt2_ctr_validate_show("RPFCOFF6", sfx, data->rpfcoff6, odata->rpfcoff6, flags);
    mt2_ctr_validate_show("RPFCOFF7", sfx, data->rpfcoff7, odata->rpfcoff7, flags);
    mt2_ctr_validate_show("RPFC0", sfx, data->rpfc0, odata->rpfc0, flags);
    mt2_ctr_validate_show("RPFC1", sfx, data->rpfc1, odata->rpfc1, flags);
    mt2_ctr_validate_show("RPFC2", sfx, data->rpfc2, odata->rpfc2, flags);
    mt2_ctr_validate_show("RPFC3", sfx, data->rpfc3, odata->rpfc3, flags);
    mt2_ctr_validate_show("RPFC4", sfx, data->rpfc4, odata->rpfc4, flags);
    mt2_ctr_validate_show("RPFC5", sfx, data->rpfc5, odata->rpfc5, flags);
    mt2_ctr_validate_show("RPFC6", sfx, data->rpfc6, odata->rpfc6, flags);
    mt2_ctr_validate_show("RPFC7", sfx, data->rpfc7, odata->rpfc7, flags);
    mt2_ctr_validate_show("RSCHCRC", sfx, data->rschcrc, odata->rschcrc, flags);
    mt2_ctr_validate_show("RUND", sfx, data->rund, odata->rund, flags);
    mt2_ctr_validate_show("RFRG", sfx, data->rfrg, odata->rfrg, flags);
    mt2_ctr_validate_show("RX_EEE_LPI_EVENT_COUNTER", sfx, data->rx_eee_lpi_event_counter, odata->rx_eee_lpi_event_counter, flags);
    mt2_ctr_validate_show("RX_EEE_LPI_DURATION_COUNTER", sfx, data->rx_eee_lpi_duration_counter, odata->rx_eee_lpi_duration_counter, flags);
    mt2_ctr_validate_show("RX_LLFC_PHY_COUNTER", sfx, data->rx_llfc_phy_counter, odata->rx_llfc_phy_counter, flags);
    mt2_ctr_validate_show("RX_LLFC_LOG_COUNTER", sfx, data->rx_llfc_log_counter, odata->rx_llfc_log_counter, flags);
    mt2_ctr_validate_show("RX_LLFC_CRC_COUNTER", sfx, data->rx_llfc_crc_counter, odata->rx_llfc_crc_counter, flags);
    mt2_ctr_validate_show("RX_HCFC_COUNTER", sfx, data->rx_hcfc_counter, odata->rx_hcfc_counter, flags);
    mt2_ctr_validate_show("RBYT", sfx, data->rbyt, odata->rbyt, flags);
    mt2_ctr_validate_show("RRBYT", sfx, data->rrbyt, odata->rrbyt, flags);
    mt2_ctr_validate_show("RRPKT", sfx, data->rrpkt, odata->rrpkt, flags);
    mt2_ctr_validate_show("T64", sfx, data->t64, odata->t64, flags);
    mt2_ctr_validate_show("T127", sfx, data->t127, odata->t127, flags);
    mt2_ctr_validate_show("T255", sfx, data->t255, odata->t255, flags);
    mt2_ctr_validate_show("T511", sfx, data->t511, odata->t511, flags);
    mt2_ctr_validate_show("T1023", sfx, data->t1023, odata->t1023, flags);
    mt2_ctr_validate_show("T1518", sfx, data->t1518, odata->t1518, flags);
    mt2_ctr_validate_show("TMGV", sfx, data->tmgv, odata->tmgv, flags);
    mt2_ctr_validate_show("T2047", sfx, data->t2047, odata->t2047, flags);
    mt2_ctr_validate_show("T4095", sfx, data->t4095, odata->t4095, flags);
    mt2_ctr_validate_show("T9216", sfx, data->t9216, odata->t9216, flags);
    mt2_ctr_validate_show("T16383", sfx, data->t16383, odata->t16383, flags);
    mt2_ctr_validate_show("TPOK", sfx, data->tpok, odata->tpok, flags);
    mt2_ctr_validate_show("TPKT", sfx, data->tpkt, odata->tpkt, flags);
    mt2_ctr_validate_show("TUCA", sfx, data->tuca, odata->tuca, flags);
    mt2_ctr_validate_show("TMCA", sfx, data->tmca, odata->tmca, flags);
    mt2_ctr_validate_show("TBCA", sfx, data->tbca, odata->tbca, flags);
    mt2_ctr_validate_show("TXPF", sfx, data->txpf, odata->txpf, flags);
    mt2_ctr_validate_show("TXPP", sfx, data->txpp, odata->txpp, flags);
    mt2_ctr_validate_show("TJBR", sfx, data->tjbr, odata->tjbr, flags);
    mt2_ctr_validate_show("TFCS", sfx, data->tfcs, odata->tfcs, flags);
    mt2_ctr_validate_show("TXCF", sfx, data->txcf, odata->txcf, flags);
    mt2_ctr_validate_show("TOVR", sfx, data->tovr, odata->tovr, flags);
    mt2_ctr_validate_show("TDFR", sfx, data->tdfr, odata->tdfr, flags);
    mt2_ctr_validate_show("TEDF", sfx, data->tedf, odata->tedf, flags);
    mt2_ctr_validate_show("TSCL", sfx, data->tscl, odata->tscl, flags);
    mt2_ctr_validate_show("TMCL", sfx, data->tmcl, odata->tmcl, flags);
    mt2_ctr_validate_show("TLCL", sfx, data->tlcl, odata->tlcl, flags);
    mt2_ctr_validate_show("TXCL", sfx, data->txcl, odata->txcl, flags);
    mt2_ctr_validate_show("TFRG", sfx, data->tfrg, odata->tfrg, flags);
    mt2_ctr_validate_show("TERR", sfx, data->terr, odata->terr, flags);
    mt2_ctr_validate_show("TVLN", sfx, data->tvln, odata->tvln, flags);
    mt2_ctr_validate_show("TDVLN", sfx, data->tdvln, odata->tdvln, flags);
    mt2_ctr_validate_show("TRPKT", sfx, data->trpkt, odata->trpkt, flags);
    mt2_ctr_validate_show("TUFL", sfx, data->tufl, odata->tufl, flags);
    mt2_ctr_validate_show("TPFC0", sfx, data->tpfc0, odata->tpfc0, flags);
    mt2_ctr_validate_show("TPFC1", sfx, data->tpfc1, odata->tpfc1, flags);
    mt2_ctr_validate_show("TPFC2", sfx, data->tpfc2, odata->tpfc2, flags);
    mt2_ctr_validate_show("TPFC3", sfx, data->tpfc3, odata->tpfc3, flags);
    mt2_ctr_validate_show("TPFC4", sfx, data->tpfc4, odata->tpfc4, flags);
    mt2_ctr_validate_show("TPFC5", sfx, data->tpfc5, odata->tpfc5, flags);
    mt2_ctr_validate_show("TPFC6", sfx, data->tpfc6, odata->tpfc6, flags);
    mt2_ctr_validate_show("TPFC7", sfx, data->tpfc7, odata->tpfc7, flags);
    mt2_ctr_validate_show("TX_EEE_LPI_EVENT_COUNTER", sfx, data->tx_eee_lpi_event_counter, odata->tx_eee_lpi_event_counter, flags);
    mt2_ctr_validate_show("TX_EEE_LPI_DURATION_COUNTER", sfx, data->tx_eee_lpi_duration_counter, odata->tx_eee_lpi_duration_counter, flags);
    mt2_ctr_validate_show("TX_LLFC_LOG_COUNTER", sfx, data->tx_llfc_log_counter, odata->tx_llfc_log_counter, flags);
    mt2_ctr_validate_show("TX_HCFC_COUNTER", sfx, data->tx_hcfc_counter, odata->tx_hcfc_counter, flags);
    mt2_ctr_validate_show("TNCL", sfx, data->tncl, odata->tncl, flags);
    mt2_ctr_validate_show("TBYT", sfx, data->tbyt, odata->tbyt, flags);

    return SOC_E_NONE;
}

#ifdef INCLUDE_FCMAP
STATIC int mt2_fc_ctr_show(mt2_fc_ctr_t *data, mt2_fc_ctr_t *odata, char *sfx, uint32 flags)
{
    mt2_ctr_validate_show("FC_C2_RXGOODFRAMES", sfx, data->fc_c2_rxgoodframes, odata->fc_c2_rxgoodframes, flags);
    mt2_ctr_validate_show("FC_C2_RXINVALIDCRC", sfx, data->fc_c2_rxinvalidcrc, odata->fc_c2_rxinvalidcrc, flags);
    mt2_ctr_validate_show("FC_C2_RXFRAMETOOLONG", sfx, data->fc_c2_rxframetoolong, odata->fc_c2_rxframetoolong, flags);
    mt2_ctr_validate_show("FC_C3_RXGOODFRAMES", sfx, data->fc_c3_rxgoodframes, odata->fc_c3_rxgoodframes, flags);
    mt2_ctr_validate_show("FC_C3_RXINVALIDCRC", sfx, data->fc_c3_rxinvalidcrc, odata->fc_c3_rxinvalidcrc, flags);
    mt2_ctr_validate_show("FC_C3_RXFRAMETOOLONG", sfx, data->fc_c3_rxframetoolong, odata->fc_c3_rxframetoolong, flags);
    mt2_ctr_validate_show("FC_CF_RXGOODFRAMES", sfx, data->fc_cf_rxgoodframes, odata->fc_cf_rxgoodframes, flags);
    mt2_ctr_validate_show("FC_CF_RXINVALIDCRC", sfx, data->fc_cf_rxinvalidcrc, odata->fc_cf_rxinvalidcrc, flags);
    mt2_ctr_validate_show("FC_CF_RXFRAMETOOLONG", sfx, data->fc_cf_rxframetoolong, odata->fc_cf_rxframetoolong, flags);
    mt2_ctr_validate_show("FC_RXDELIMITERERR", sfx, data->fc_rxdelimitererr, odata->fc_rxdelimitererr, flags);
    mt2_ctr_validate_show("FC_RXBBC0DROP", sfx, data->fc_rxbbc0drop, odata->fc_rxbbc0drop, flags);
    mt2_ctr_validate_show("FC_RXBBCREDIT0", sfx, data->fc_rxbbcredit0, odata->fc_rxbbcredit0, flags);
    mt2_ctr_validate_show("FC_RXLINKFAIL", sfx, data->fc_rxlinkfail, odata->fc_rxlinkfail, flags);
    mt2_ctr_validate_show("FC_RXSYNCFAIL", sfx, data->fc_rxsyncfail, odata->fc_rxsyncfail, flags);
    mt2_ctr_validate_show("FC_RXLOSSSIG", sfx, data->fc_rxlosssig, odata->fc_rxlosssig, flags);
    mt2_ctr_validate_show("FC_RXPRIMSEQERR", sfx, data->fc_rxprimseqerr, odata->fc_rxprimseqerr, flags);
    mt2_ctr_validate_show("FC_RXINVALIDSET", sfx, data->fc_rxinvalidset, odata->fc_rxinvalidset, flags);
    mt2_ctr_validate_show("FC_RXLPICOUNT", sfx, data->fc_rxlpicount, odata->fc_rxlpicount, flags);
    mt2_ctr_validate_show("FC_RXENCODEDISPARITY", sfx, data->fc_rxencodedisparity, odata->fc_rxencodedisparity, flags);
    mt2_ctr_validate_show("FC_RXBADXWORD", sfx, data->fc_rxbadxword, odata->fc_rxbadxword, flags);
    mt2_ctr_validate_show("FC_RX_C2_RUNTFRAMES", sfx, data->fc_rx_c2_runtframes, odata->fc_rx_c2_runtframes, flags);
    mt2_ctr_validate_show("FC_RX_C3_RUNTFRAMES", sfx, data->fc_rx_c3_runtframes, odata->fc_rx_c3_runtframes, flags);
    mt2_ctr_validate_show("FC_RX_CF_RUNTFRAMES", sfx, data->fc_rx_cf_runtframes, odata->fc_rx_cf_runtframes, flags);
    mt2_ctr_validate_show("FC_C2_RXBYT", sfx, data->fc_c2_rxbyt, odata->fc_c2_rxbyt, flags);
    mt2_ctr_validate_show("FC_C3_RXBYT", sfx, data->fc_c3_rxbyt, odata->fc_c3_rxbyt, flags);
    mt2_ctr_validate_show("FC_CF_RXBYT", sfx, data->fc_cf_rxbyt, odata->fc_cf_rxbyt, flags);
    mt2_ctr_validate_show("FC_TX_C2_FRAMES", sfx, data->fc_tx_c2_frames, odata->fc_tx_c2_frames, flags);
    mt2_ctr_validate_show("FC_TX_C3_FRAMES", sfx, data->fc_tx_c3_frames, odata->fc_tx_c3_frames, flags);
    mt2_ctr_validate_show("FC_TX_CF_FRAMES", sfx, data->fc_tx_cf_frames, odata->fc_tx_cf_frames, flags);
    mt2_ctr_validate_show("FC_TX_C2_OVERSIZED_FRAMES", sfx, data->fc_tx_c2_oversized_frames, odata->fc_tx_c2_oversized_frames, flags);
    mt2_ctr_validate_show("FC_TX_C3_OVERSIZED_FRAMES", sfx, data->fc_tx_c3_oversized_frames, odata->fc_tx_c3_oversized_frames, flags);
    mt2_ctr_validate_show("FC_TX_CF_OVERSIZED_FRAMES", sfx, data->fc_tx_cf_oversized_frames, odata->fc_tx_cf_oversized_frames, flags);
    mt2_ctr_validate_show("FC_TXBBCREDIT0", sfx, data->fc_txbbcredit0, odata->fc_txbbcredit0, flags);
    mt2_ctr_validate_show("FC_C2_TXBYT", sfx, data->fc_c2_txbyt, odata->fc_c2_txbyt, flags);
    mt2_ctr_validate_show("FC_C3_TXBYT", sfx, data->fc_c3_txbyt, odata->fc_c3_txbyt, flags);
    mt2_ctr_validate_show("FC_CF_TXBYT", sfx, data->fc_cf_txbyt, odata->fc_cf_txbyt, flags);

    return SOC_E_NONE;
}
#endif /* INCLUDE_FCMAP */

int phy8806x_ctr_interval_set(phy_ctrl_t *pc, uint32_t val)
{
    uint32_t xmodtxbuff[XMOD_BUFFER_MAX_LEN/4];
    uint8_t *buftxptr;
    int xmodtxlen, rv;

    /* write args to xmod buffer */
    buftxptr = (uint8_t *)xmodtxbuff;
    cli_out("setting Interval to %d on port %d\n", val, pc->port);

#if BE_HOST 
    val =  _shr_swap32(val);
#endif
    WRITE_XMOD_ARG_BUFF(buftxptr, &val, sizeof(uint32_t));
    xmodtxlen = GET_XMOD_ETHERNET_CMD_IN_LEN(XMOD_DEV_CTR_INTERVAL_SET);
    xmodtxlen = GET_XMOD_BUF_WORD_LEN(xmodtxlen);

    /* call xmod */
    rv = phy_8806x_xmod_command(pc->unit, pc->port, XMOD_CMD_MODE_CORE(XMOD_DEV_CTR_INTERVAL_SET),
                             xmodtxbuff, xmodtxlen, NULL, 0);

    return rv;
}

int
phy8806x_ctr_start(int unit)
{
    if (mt2_ctr_rx_init(unit) != SOC_E_NONE) {
        cli_out("MT2 Ctr Start Failed u=%d\n", unit);
        return SOC_E_FAIL;
    } else {
        cli_out("MT2 Ctr Routine Started u=%d\n", unit);
        return SOC_E_NONE;
    }
}

int
phy8806x_ctr_stop(int unit)
{
    if (mt2_ctr_rx_deinit(unit) != SOC_E_NONE) {
        cli_out("MT2 Ctr Stop Failed u=%d\n", unit);
        return SOC_E_FAIL;
    } else {
        cli_out("MT2 Ctr Routine Stopped u=%d\n", unit);
        return SOC_E_NONE;
    }
}

int
phy8806x_ctr_show(phy_ctrl_t *pc, uint32_t flags)
{
    uint8 *datptr, *oldptr;
    char suffix[32];
    uint32_t ctrflags = flags;

    if (flags == 0) {
        ctrflags = MT2_CTR_DEFAULT;
    }
    if (ctrflags & MT2_CTR_ALL) {
        ctrflags = (MT2_CTR_SYS | MT2_CTR_LINE | MT2_CTR_CHANGED | MT2_CTR_SAME | MT2_CTR_ZERO | MT2_CTR_NONZERO);
    }
    if ((ctrflags & (MT2_CTR_SYS | MT2_CTR_LINE)) == 0) {
        ctrflags |= (MT2_CTR_SYS | MT2_CTR_LINE);
    }
    if ((ctrflags & (MT2_CTR_ZERO | MT2_CTR_NONZERO)) == 0) {
        ctrflags |= (MT2_CTR_ZERO | MT2_CTR_NONZERO);
    }
    if ((ctrflags & (MT2_CTR_CHANGED | MT2_CTR_SAME)) == 0) {
        ctrflags |= MT2_CTR_CHANGED;
    }

    datptr = phy8806x_eth_ctr_get(pc);
    oldptr = datptr + MT2_CTR_PREV_OFF;

    if (ctrflags & MT2_CTR_LINE) {
        sal_sprintf(suffix, "%s.Line",SOC_PORT_NAME(pc->unit, pc->port));
#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable){
            mt2_fc_ctr_show((mt2_fc_ctr_t *)datptr, (mt2_fc_ctr_t *)oldptr, suffix, ctrflags);
            sal_memcpy(oldptr, datptr, sizeof(mt2_fc_ctr_t));
        } else
#endif
        {
            mt2_eth_ctr_show((mt2_eth_ctr_t *)datptr, (mt2_eth_ctr_t *)oldptr, suffix, ctrflags);
            sal_memcpy(oldptr, datptr, sizeof(mt2_eth_ctr_t));
        }
    }

    if (ctrflags & MT2_CTR_SYS) {
#if defined(INCLUDE_FCMAP)
        if (pc->fcmap_enable){
            datptr += sizeof(mt2_fc_ctr_t);
            oldptr += sizeof(mt2_fc_ctr_t);
            cli_out("Sys Side Yet to implement FC Port...\n");
        } else
#endif
        {
            datptr += sizeof(mt2_eth_ctr_t);
            oldptr += sizeof(mt2_eth_ctr_t);
            sal_sprintf(suffix, "%s.Sys",SOC_PORT_NAME(pc->unit, pc->port));
            mt2_eth_ctr_show((mt2_eth_ctr_t *)datptr, (mt2_eth_ctr_t *)oldptr, suffix, ctrflags);
            sal_memcpy(oldptr, datptr, sizeof(mt2_eth_ctr_t));
        }
    }

    return SOC_E_NONE;
}

#else
typedef int _8806x_ctr_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_PHY_8806X */

