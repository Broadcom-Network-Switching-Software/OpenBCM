/*! \file bcm56780_a0_sec_port.c
 *
 * File containing port and PM related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/chip/bcm56780_a0_sec.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include <bcmsec/chip/bcm56780_a0_sec_port.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_PORT
/*******************************************************************************
 * Local definitions
 */
static struct bcm56780_a0_sec_blk_map_s {
    /* -1 for invalid PM. */
    int pm0[2];
    /* -1 for invalid PM. */
    int pm1[2];
} sec_blk_map[TD4_X9_NUM_SEC_BLK] = {
    /* BH0, BH1 */
    {{6, 0},   {7, 1}},     /* SEC  0 */
    {{4, -1},  {5, -1}},    /* SEC  1 */
    {{8, 2},   {9, 3}},     /* SEC  2 */
    {{17, 11}, {16, 10}},   /* SEC  3 */
    {{15, -1}, {14, -1}},   /* SEC  4 */
    {{19, 13}, {18, 12}},   /* SEC  5 */
};

/*******************************************************************************
 * Private functions
 */
/*
 * Description: For a given port macro, identify the Macsec source
 *              and the macsec active bit. Refer to regsfile for details.
 */
static int
bcm56780_a0_sec_pm_source_get (int unit, int pm_id, uint32_t *macsec_src,
                               bcmdrd_fid_t *fid, uint32_t *macsec_active)
{
    switch (pm_id) {
        case 0: *fid = MACSEC_SRC_PM0_OR_PM6f; *macsec_src = 1;
            *macsec_active = 0;
            break;
        case 6: *fid = MACSEC_SRC_PM0_OR_PM6f; *macsec_src = 0;
            *macsec_active = 0;
            break;
        case 1: *fid = MACSEC_SRC_PM1_OR_PM7f; *macsec_src = 1;
            *macsec_active = 1;
            break;
        case 7: *fid = MACSEC_SRC_PM1_OR_PM7f; *macsec_src = 0;
            *macsec_active = 1;
            break;
        case 2: *fid = MACSEC_SRC_PM2_OR_PM8f; *macsec_src = 1;
            *macsec_active = 4;
            break;
        case 8: *fid = MACSEC_SRC_PM2_OR_PM8f; *macsec_src = 0;
            *macsec_active = 4;
            break;
        case 3: *fid = MACSEC_SRC_PM3_OR_PM9f; *macsec_src = 1;
            *macsec_active = 5;
            break;
        case 9: *fid = MACSEC_SRC_PM3_OR_PM9f; *macsec_src = 0;
            *macsec_active = 5;
            break;
        case 10: *fid = MACSEC_SRC_PM10_OR_PM16f; *macsec_src = 1;
            *macsec_active = 7;
            break;
        case 16: *fid = MACSEC_SRC_PM10_OR_PM16f; *macsec_src = 0;
            *macsec_active = 7;
            break;
        case 11: *fid = MACSEC_SRC_PM11_OR_PM17f; *macsec_src = 1;
            *macsec_active = 6;
            break;
        case 17: *fid = MACSEC_SRC_PM11_OR_PM17f; *macsec_src = 0;
            *macsec_active = 6;
            break;
        case 12: *fid = MACSEC_SRC_PM12_OR_PM18f; *macsec_src = 1;
            *macsec_active = 11;
            break;
        case 18: *fid = MACSEC_SRC_PM12_OR_PM18f; *macsec_src = 0;
            *macsec_active = 11;
            break;
        case 13: *fid = MACSEC_SRC_PM13_OR_PM19f; *macsec_src = 1;
            *macsec_active = 10;
            break;
        case 19: *fid = MACSEC_SRC_PM13_OR_PM19f; *macsec_src = 0;
            *macsec_active = 10;
            break;
        case 4: *fid = INVALIDf; *macsec_src = 0;
            *macsec_active = 2;
            break;
        case 5: *fid = INVALIDf; *macsec_src = 0;
            *macsec_active = 3;
            break;
        case 14: *fid = INVALIDf; *macsec_src = 0;
            *macsec_active = 9;
            break;
        case 15: *fid = INVALIDf; *macsec_src = 0;
            *macsec_active = 8;
            break;
        default:
            return SHR_E_PARAM;
    }
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_sec_pm_enable(int unit, bcmltd_sid_t ltid,
                       uint32_t pm_id, uint32_t enable,
                       uint8_t *opcode)
{
    int pm_pair, pm_id_orig, pm_id_base;
    bcmsec_pm_info_t *pm_info;
    bcmsec_dev_info_t *dev_info;
    bcmdrd_sid_t mem;
    bcmsec_pt_info_t pt_info = {0};
    uint32_t ltmbuf[2] = {0};
    uint32_t macsec_src[1], fval[1], macsec_active;
    bcmdrd_fid_t fid;
    bcmdrd_fid_t fid_a[6] =
        {MACSEC0_ACTIVEf, MACSEC1_ACTIVEf, MACSEC2_ACTIVEf,
         MACSEC3_ACTIVEf, MACSEC4_ACTIVEf, MACSEC5_ACTIVEf};
    bcmdrd_fid_t fid_c[] =
        {PM0f, PM1f, PM2f, PM3f, PM4f};


    SHR_FUNC_ENTER(unit);
    if (pm_id >= TD4_X9_NUM_PORT_MACRO) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pm_id_orig = pm_id;
    /*
     * SW WAR: SDKLT-23649
     * For PMs in DP1 and DP3, swap the external
     * PM programming.
     */
    if ((pm_id > 4 && pm_id < 10) ||
        (pm_id > 14 && pm_id < 20)) {
        pm_id_base = (pm_id < 10) ? 5 : 15;
        pm_id = pm_id_base + (4 - (pm_id % 5));
    }

    /* Update the PM credit mode for 64 aligned interfaces. */
    mem = EDB_PM_CREDIT_MODEr;
    sal_memset(&pt_info, 0, sizeof(bcmsec_pt_info_t));
    sal_memset(&ltmbuf, 0, 2 * sizeof(uint32_t));
    pt_info.tbl_inst = pm_id / 5;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, mem, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, mem, fid_c[pm_id % 5], ltmbuf, fval));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, mem, ltid, &pt_info, ltmbuf));
    pm_id = pm_id_orig;
    enable = fval[0];

    sal_memset(&pt_info, 0, sizeof(bcmsec_pt_info_t));
    sal_memset(&ltmbuf, 0, 2 * sizeof(uint32_t));

    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));
    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);

    pm_info = &(dev_info->pm_map_info[pm_id]);
    pm_pair = dev_info->pm_map_info[pm_id].pm_pair;

    /*
     * Among a pair of PM for a Macsec block,
     * only one can be enabled.
     */
    if (enable && (pm_pair >= 0) &&
        dev_info->pm_map_info[pm_pair].enable) {
        LOG_WARN(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Port Macro pair %d for PM %d is already enabled.\n"),
                pm_pair, pm_id));
        *opcode = CONFLICTING_PM_PAIR;
        SHR_EXIT();
    }
    mem = TOP_MACSEC_CTRL0r;
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, mem, ltid, &pt_info, ltmbuf));

    SHR_IF_ERR_EXIT
        (bcm56780_a0_sec_pm_source_get(unit, pm_id, macsec_src, &fid, &macsec_active));

    if (enable) {
        /*
         * Select the Macsec Source for the block.
         */
        if (fid != INVALIDf) {
            SHR_IF_ERR_EXIT
                (bcmsec_field_set(unit, mem, fid, ltmbuf, macsec_src));
        }
        fid = fid_a[macsec_active / 2];
        SHR_IF_ERR_EXIT
            (bcmsec_field_get(unit, mem, fid, ltmbuf, fval));

        /*
         * Enable the Macsec active bit for the block.
         */
        fval[0] |= (1 << (macsec_active % 2));
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, mem, fid, ltmbuf, fval));
        pm_info->enable = 1;
    } else if (dev_info->pm_map_info[pm_pair].enable == 0) {
        /*
         * Both PM of the pairs are disabled.
         */
        fid = fid_a[macsec_active / 2];
        SHR_IF_ERR_EXIT
            (bcmsec_field_get(unit, mem, fid, ltmbuf, fval));

        /*
         * Disable the Macsec active bit for the block.
         */
        fval[0] &= ~(1 << (macsec_active % 2));
        SHR_IF_ERR_EXIT
            (bcmsec_field_set(unit, mem, fid, ltmbuf, fval));
        pm_info->enable = 0;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, mem, ltid, &pt_info, ltmbuf));
    *opcode = VALID;

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_sec_port_info_get(int unit, int pport,
                              sec_pt_port_info_t *info)
{
    int pm_id, sec_local_port, sec_port;
    bcmsec_pm_info_t *pm_info;
    bcmsec_dev_info_t *dev_info;
    int rv;

    SHR_FUNC_ENTER(unit);
    if (!info) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));
    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);
    rv = bcm56780_a0_sec_pport_to_secport(unit, pport, &pm_id, &sec_port,
                                          &sec_local_port);
    if (SHR_FAILURE(rv)) {
        info->oper = PORT_INFO_UNAVAIL;
        SHR_EXIT();
    }
    info->oper = VALID;
    pm_info = &(dev_info->pm_map_info[pm_id]);
    info->sec_blk = pm_info->sec_blk;
    info->sec_port = sec_port;
    info->sec_local_port = sec_local_port;
    if (pm_info->enable) {
        info->valid = pm_info->enable;
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_sec_egr_port_control_set(int unit, bcmltd_sid_t ltid,
                               bcmsec_encrypt_port_control_t *port_control,
                               uint32_t *opcode)
{
    int pport, sec_port;
    bcmdrd_sid_t mem = ESEC_CONFIGr;
    bcmsec_pt_info_t pt_info = {0};
    uint32_t ltmbuf[2] = {0};
    uint32_t fval[1];
    bcmdrd_fid_t fid;
    uint64_t flags = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    pport = port_control->pport;
    rv = bcm56780_a0_sec_pport_to_secport(unit, pport, NULL, &sec_port, NULL);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Unable to get sec_port for physical port %d.\n"),
                pport));
        *opcode = PORT_INFO_UNAVAIL;
        SHR_EXIT();
    }
    BCMSEC_PT_DYN_INFO(pt_info, 0, sec_port, flags);
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_read(unit, mem, ltid, &pt_info, ltmbuf));

    fid = EN_PORT_BASED_SCf;
    fval[0] = port_control->port_based_sc & 0x1;
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, mem, fid, ltmbuf, fval));

    fid = CRC_STRIPf;
    fval[0] = port_control->remove_crc & 0x1;
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, mem, fid, ltmbuf, fval));

    fid = TX_THRESHOLDf;
    fval[0] = port_control->tx_threshold;
    SHR_IF_ERR_EXIT
        (bcmsec_field_set(unit, mem, fid, ltmbuf, fval));

    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, mem, ltid, &pt_info, ltmbuf));
    *opcode = VALID;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_decrypt_port_control_set(int unit, bcmltd_sid_t ltid,
                              int pport, bcmltd_field_t *in, uint32_t *opcode)
{
    int sec_port, rv = SHR_E_NONE;
    bcmdrd_sid_t sid = ISEC_PP_CTRLr;
    bcmdrd_fid_t fid0, fid1;
    bcmsec_pt_info_t pt_info = {0};
    uint64_t flags = 0;
    uint32_t fval0[2] = {0}, fval1[2] = {0}, val, idx, data;

    SHR_FUNC_ENTER(unit);

    rv = bcm56780_a0_sec_pport_to_secport(unit, pport, NULL, &sec_port, NULL);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,(BSL_META_U(unit,
                "Unable to get sec_port for physical port %d.\n"),
                pport));
        *opcode = PORT_INFO_UNAVAIL;
        SHR_ERR_EXIT(SHR_E_NONE);
    }

    BCMSEC_PT_DYN_INFO(pt_info, 0, sec_port, flags);
    if (in) {
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, ISEC_PP_CTRLr, ltid,
                                    &pt_info, fval0));
        SHR_IF_ERR_EXIT
            (bcmsec_pt_indexed_read(unit, ISEC_RUD_MGMT_RULE_CTRLr, ltid,
                                    &pt_info, fval1));
    }

    while (in) {
        sid = ISEC_PP_CTRLr;
        data = (uint32_t)in->data;
        val = data;
        idx = in->idx;
        fid0 = INVALIDf;    /* ISEC_PP_CTRLr */
        fid1 = INVALIDf;    /* ISEC_RUD_MGMT_RULE_CTRLr */
        switch (in->id) {
            case SEC_DECRYPT_PORTt_SEC_MACSEC_SECTAG_ETHERTYPE_IDf:
                fid0 = SECTAG_ETYPE_SELf;
                break;
            case SEC_DECRYPT_PORTt_SECTAG_VERSIONf:
                fid0 = SECTAG_Vf;
                break;
            case SEC_DECRYPT_PORTt_SECTAG_VERSION_MISMATCHf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 0, data);
                break;
            case SEC_DECRYPT_PORTt_SHORT_LENGTH_INVALIDf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 1, data);
                break;
            case SEC_DECRYPT_PORTt_SHORT_LENGTH_NOT_SETf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 2, data);
                break;
            case SEC_DECRYPT_PORTt_SHORT_LENGTH_HIGHf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 3, data);
                break;
            case SEC_DECRYPT_PORTt_SHORT_LENGTH_MISMATCHf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 8, data);
                break;
            case SEC_DECRYPT_PORTt_SECTAG_E0_C1f:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 4, data);
                break;
            case SEC_DECRYPT_PORTt_SECTAG_ES1_SC1f:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 5, data);
                break;
            case SEC_DECRYPT_PORTt_SECTAG_SC1_SCB1f:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 6, data);
                break;
            case SEC_DECRYPT_PORTt_PN_INVALIDf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 7, data);
                break;
            case SEC_DECRYPT_PORTt_AN_INVALIDf:
                sid = ISEC_PP_CTRLr;
                fid0 = SECTAG_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 9, data);
                break;
            case SEC_DECRYPT_PORTt_TPIDf:
                sid = ISEC_PP_CTRLr;
                fid0 = TPID_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, idx, data);
                break;
            case SEC_DECRYPT_PORTt_MPLS_ETHERTYPEf:
                sid = ISEC_PP_CTRLr;
                fid0 = MPLS_ETYPE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, idx, data);
                break;
            case SEC_DECRYPT_PORTt_PBB_TPIDf:
                fid0 = PBB_ENf;
                break;
            case SEC_DECRYPT_PORTt_IPV4_ETHERTYPEf:
                fid0 = IPV4_ETYPE_ENf;
                break;
            case SEC_DECRYPT_PORTt_IPV6_ETHERTYPEf:
                fid0 = IPV6_ETYPE_ENf;
                break;
            case SEC_DECRYPT_PORTt_NIV_ETHERTYPEf:
                fid0 = NIV_ETYPE_ENf;
                break;
            case SEC_DECRYPT_PORTt_PE_ETHERTYPEf:
                fid0 = PE_ETYPE_ENf;
                break;
            case SEC_DECRYPT_PORTt_UDPf:
                fid0 = UDP_PROTO_ENf;
                break;
            case SEC_DECRYPT_PORTt_TCPf:
                fid0 = TCP_PROTO_ENf;
                break;
            case SEC_DECRYPT_PORTt_PTP_ETHERTYPEf:
                fid0 = PTP_ETYPE_ENf;
                break;
            case SEC_DECRYPT_PORTt_PTP_DEST_PORTf:
                fid0 = PTP_DEST_PORT_ENf;
                break;
            case SEC_DECRYPT_PORTt_UNTAGGED_PTP_ETHERTYPEf:
                sid = ISEC_PP_CTRLr;
                fid0 = PTP_MATCH_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 0, data);
                break;
            case SEC_DECRYPT_PORTt_PTP_ETHERTYPE_VLANf:
                sid = ISEC_PP_CTRLr;
                fid0 = PTP_MATCH_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 1, data);
                break;
            case SEC_DECRYPT_PORTt_UDP_IPV4_PTPf:
                sid = ISEC_PP_CTRLr;
                fid0 = PTP_MATCH_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 2, data);
                break;
            case SEC_DECRYPT_PORTt_UDP_IPV6_PTPf:
                sid = ISEC_PP_CTRLr;
                fid0 = PTP_MATCH_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 3, data);
                break;
            case SEC_DECRYPT_PORTt_SECTAG_AFTER_IPV4f:
                fid0 = SECTAG_AFTER_IPV4_HDR_ENf;
                break;
            case SEC_DECRYPT_PORTt_SECTAG_AFTER_IPV6f:
                fid0 = SECTAG_AFTER_IPV6_HDR_ENf;
                break;
            case SEC_DECRYPT_PORTt_SECTAG_AFTER_TCPf:
                fid0 = SECTAG_AFTER_TCP_HDR_ENf;
                break;
            case SEC_DECRYPT_PORTt_SECTAG_AFTER_UDPf:
                fid0 = SECTAG_AFTER_UDP_HDR_ENf;
                break;
            case SEC_DECRYPT_PORTt_IPV4_CHECKSUMf:
                fid0 = IPV4_CHKSUM_CHK_ENf;
                break;
            case SEC_DECRYPT_PORTt_VXLAN_UDF_PAYLOADf:
                fid0 = VXLAN_IPV6_W_UDP_SPTCAM_UDF_PAYLD_SELf;
                break;
            case SEC_DECRYPT_PORTt_STP_FOR_BRIDGES_MAC_ADDRf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 0, data);
                break;
            case SEC_DECRYPT_PORTt_VTP_MAC_ADDRESSf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 1, data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_DST_MACf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, (2 + idx), data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_ETHERTYPEf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, (10 + idx), data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_RANGEf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 18, data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_DST_MAC_AND_ETHERTYPE_0f:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 19, data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_DST_MAC_AND_ETHERTYPE_1f:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 20, data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_TCIE_1_TCIC_0f:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 21, data);
                break;
            case SEC_DECRYPT_PORTt_MGMT_PTPf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 22, data);
                break;
            case SEC_DECRYPT_PORTt_IKE_DEST_PORT_WITH_NONESPf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 23, data);
                break;
            case SEC_DECRYPT_PORTt_IKE_DEST_PORT_WITHOUT_NONESPf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 24, data);
                break;
            case SEC_DECRYPT_PORTt_NAT_KEEPALIVE_DST_PORTf:
                sid = ISEC_RUD_MGMT_RULE_CTRLr;
                fid1 = RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid1, fval1, &val));
                SEC_BITASSIGN(val, 25, data);
                break;
            case SEC_DECRYPT_PORTt_ESP_UDP_SRC_PORTf:
                fid0 = ESP_UDP_SRCPORT_ENf;
                break;
            case SEC_DECRYPT_PORTt_ESP_UDP_DST_PORTf:
                fid0 = ESP_UDP_ENf;
                break;
            case SEC_DECRYPT_PORTt_ESP_SN_ZEROf:
                sid = ISEC_PP_CTRLr;
                fid0 = ESP_HDR_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 0, data);
                break;
            case SEC_DECRYPT_PORTt_ESP_SPI_ZEROf:
                sid = ISEC_PP_CTRLr;
                fid0 = ESP_HDR_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 1, data);
                break;
            case SEC_DECRYPT_PORTt_ESP_SPI_1_255f:
                sid = ISEC_PP_CTRLr;
                fid0 = ESP_HDR_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 2, data);
                break;
            case SEC_DECRYPT_PORTt_IPV4_FRAGMENTf:
                sid = ISEC_PP_CTRLr;
                fid0 = ESP_HDR_VLD_RULE_ENf;
                SHR_IF_ERR_EXIT
                    (bcmsec_field_get(unit, sid, fid0, fval0, &val));
                SEC_BITASSIGN(val, 3, data);
                break;
            case SEC_DECRYPT_PORTt_SECTAG_SHORT_LENGTH_6BITSf:
                fid0 = USE_6BIT_STAG_ONLYf;
                break;
            case SEC_DECRYPT_PORTt_ESPf:
                fid0 = ESP_IP_PROTO_ENf;
                break;
            case SEC_DECRYPT_PORTt_NAT_KEEPALIVE_SRC_PORTf:
                fid0 = ESP_NAT_KA_SRCPORT_ENf;
                break;
            case SEC_DECRYPT_PORTt_MTUf:
                fid0 = MTUf;
                break;
            case SEC_DECRYPT_PORTt_SEC_DECRYPT_SUBPORT_POLICY_IDf:
                fid1 = RULE_SP_NUMf;
                break;
            case SEC_DECRYPT_PORTt_OPERATIONAL_STATEf:
            case SEC_DECRYPT_PORTt_PORT_IDf:
                in = in->next;
                continue;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        if (fid0 != INVALIDf) {
            SHR_IF_ERR_EXIT
                (bcmsec_field_set(unit, ISEC_PP_CTRLr, fid0, fval0, &val));
        } else if (fid1 != INVALIDf) {
            SHR_IF_ERR_EXIT
                (bcmsec_field_set(unit, ISEC_RUD_MGMT_RULE_CTRLr,
                                  fid1, fval1, &val));
        } else {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        in = in->next;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, ISEC_PP_CTRLr, ltid, &pt_info, fval0));
    SHR_IF_ERR_EXIT
        (bcmsec_pt_indexed_write(unit, ISEC_RUD_MGMT_RULE_CTRLr, ltid, &pt_info, fval1));
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_sec_port_init(int unit)
{
    int i, j, *pm;
    bcmsec_pm_info_t *pm_info;
    bcmsec_dev_info_t *dev_info;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_dev_info_get(unit, &dev_info));
    SHR_NULL_CHECK(dev_info, SHR_E_INTERNAL);

    for (i = 0; i < NUM_PORT_MACRO; i++) {
        pm_info = &(dev_info->pm_map_info[i]);
        pm_info->enable = 0;
        pm_info->pm_pair = -1;
        pm_info->sec_blk = -1;
    }
    for (i = 0; i < TD4_X9_NUM_SEC_BLK; i++) {
        for (j = 0; j < 2; j++) {
            pm = (j == 0) ? sec_blk_map[i].pm0 :
                            sec_blk_map[i].pm1;
            if (pm[0] != -1) {
                dev_info->pm_map_info[pm[0]].sec_blk = i;
            }
            if (pm[1] != -1) {
                dev_info->pm_map_info[pm[1]].sec_blk = i;
            }
            if ((pm[0] != -1) && (pm[1] != -1)) {
                dev_info->pm_map_info[pm[0]].pm_pair = pm[1];
                dev_info->pm_map_info[pm[1]].pm_pair = pm[0];
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}
