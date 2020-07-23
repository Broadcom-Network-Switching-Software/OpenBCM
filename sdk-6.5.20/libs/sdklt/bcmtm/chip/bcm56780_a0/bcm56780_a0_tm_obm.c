/*! \file bcm56780_a0_tm_obm.c
 *
 * File containing obm related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_alloc.h>
#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmdrd/bcmdrd_dev.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/chip/bcm56780_a0_tm.h>
#include "bcm56780_a0_tm_obm.h"
#include "bcm56780_a0_tm_idb_flexport.h"

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief OBM PC PM packet parser configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] pm_num Port macro number.
 * \param [in] obm_pc_pm_pkt_parse_cfg Packet parser configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_pc_pm_pkt_parse_set(int unit,
                          bcmltd_sid_t ltid,
                          int pm_num,
                          bcmtm_obm_pc_pm_pkt_parse_cfg_t *obm_pc_pm_pkt_parse_cfg)
{
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0}, fval;
    uint32_t fval_arr[2] = {0};
    bcmdrd_sid_t upt0[] = {
        IDB_OBM0_PROTOCOL_CONTROL_0r, IDB_OBM1_PROTOCOL_CONTROL_0r,
        IDB_OBM2_PROTOCOL_CONTROL_0r, IDB_OBM3_PROTOCOL_CONTROL_0r,
        IDB_OBM4_PROTOCOL_CONTROL_0r, MGMT_OBM_PROTOCOL_CONTROL_0r};
    bcmdrd_sid_t upt1[] = {
        IDB_OBM0_PROTOCOL_CONTROL_1r, IDB_OBM1_PROTOCOL_CONTROL_1r,
        IDB_OBM2_PROTOCOL_CONTROL_1r, IDB_OBM3_PROTOCOL_CONTROL_1r,
        IDB_OBM4_PROTOCOL_CONTROL_1r, MGMT_OBM_PROTOCOL_CONTROL_1r};
    bcmdrd_sid_t upt2[] = {
        IDB_OBM0_PROTOCOL_CONTROL_2r, IDB_OBM1_PROTOCOL_CONTROL_2r,
        IDB_OBM2_PROTOCOL_CONTROL_2r, IDB_OBM3_PROTOCOL_CONTROL_2r,
        IDB_OBM4_PROTOCOL_CONTROL_2r, MGMT_OBM_PROTOCOL_CONTROL_2r};
    bcmdrd_sid_t etageth[] = {
        IDB_OBM0_PE_ETHERTYPEr, IDB_OBM1_PE_ETHERTYPEr,
        IDB_OBM2_PE_ETHERTYPEr, IDB_OBM3_PE_ETHERTYPEr,
        IDB_OBM4_PE_ETHERTYPEr, MGMT_OBM_PE_ETHERTYPEr};
    bcmdrd_sid_t gsheth[] = {
        IDB_OBM0_GSH_ETHERTYPEr, IDB_OBM1_GSH_ETHERTYPEr,
        IDB_OBM2_GSH_ETHERTYPEr, IDB_OBM3_GSH_ETHERTYPEr,
        IDB_OBM4_GSH_ETHERTYPEr, MGMT_OBM_GSH_ETHERTYPEr};
    bcmdrd_sid_t vntageth[] = {
        IDB_OBM0_NIV_ETHERTYPEr, IDB_OBM1_NIV_ETHERTYPEr,
        IDB_OBM2_NIV_ETHERTYPEr, IDB_OBM3_NIV_ETHERTYPEr,
        IDB_OBM4_NIV_ETHERTYPEr, MGMT_OBM_NIV_ETHERTYPEr};
    bcmdrd_sid_t innertp[] = {
        IDB_OBM0_INNER_TPIDr, IDB_OBM1_INNER_TPIDr, IDB_OBM2_INNER_TPIDr,
        IDB_OBM3_INNER_TPIDr, IDB_OBM4_INNER_TPIDr, MGMT_OBM_INNER_TPIDr};
    bcmdrd_sid_t outertp0[] = {
        IDB_OBM0_OUTER_TPID_0r, IDB_OBM1_OUTER_TPID_0r, IDB_OBM2_OUTER_TPID_0r,
        IDB_OBM3_OUTER_TPID_0r, IDB_OBM4_OUTER_TPID_0r, MGMT_OBM_OUTER_TPID_0r};
    bcmdrd_sid_t outertp1[] = {
        IDB_OBM0_OUTER_TPID_1r, IDB_OBM1_OUTER_TPID_1r, IDB_OBM2_OUTER_TPID_1r,
        IDB_OBM3_OUTER_TPID_1r, IDB_OBM4_OUTER_TPID_1r, MGMT_OBM_OUTER_TPID_1r };
    bcmdrd_sid_t outertp2[] = {
        IDB_OBM0_OUTER_TPID_2r, IDB_OBM1_OUTER_TPID_2r, IDB_OBM2_OUTER_TPID_2r,
        IDB_OBM3_OUTER_TPID_2r, IDB_OBM4_OUTER_TPID_2r, MGMT_OBM_OUTER_TPID_2r};
    bcmdrd_sid_t outertp3[] = {
        IDB_OBM0_OUTER_TPID_3r, IDB_OBM1_OUTER_TPID_3r, IDB_OBM2_OUTER_TPID_3r,
        IDB_OBM3_OUTER_TPID_3r, IDB_OBM4_OUTER_TPID_3r, MGMT_OBM_OUTER_TPID_3r };
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    int i = 0;
    int reg_idx, inst;
    SHR_FUNC_ENTER(unit);

    if (pm_num == BCM56780_A0_MGMT_PM) {
        reg_idx = 5; /* Management port index */
        inst = 0;
    } else {
        /* Front panel ports. */
        reg_idx = pm_num % TD4_X9_PBLKS_PER_PIPE;
        inst = pm_num / TD4_X9_PBLKS_PER_PIPE;
    }

    for (i = 0 ; i < 4 ;i++) {
         sid = upt0[reg_idx]; /* get the OBM number. */
         BCMTM_PT_DYN_INFO(pt_info, i, inst);
         fid = MAC_ADDRf;
         fval_arr[0] = (uint32_t)obm_pc_pm_pkt_parse_cfg->mac_addr[i];
         fval_arr[1] = (uint32_t)(obm_pc_pm_pkt_parse_cfg->mac_addr[i] >> 32);
         SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, fval_arr));
         SHR_IF_ERR_EXIT
           (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
         sal_memset(ltmbuf, 0, sizeof(ltmbuf));

         sid = upt1[reg_idx];
         BCMTM_PT_DYN_INFO(pt_info, i, inst);
         fid = MAC_ADDR_MASKf;

         fval_arr[0] = (uint32_t)obm_pc_pm_pkt_parse_cfg->mac_addr_mask[i];
         fval_arr[1] = (uint32_t)(obm_pc_pm_pkt_parse_cfg->mac_addr_mask[i] >> 32);
         SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, fval_arr));
         SHR_IF_ERR_EXIT
           (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
         sal_memset(ltmbuf, 0, sizeof(ltmbuf));

         sid = upt2[reg_idx];
         BCMTM_PT_DYN_INFO(pt_info, i, inst);
         fid = ENABLEf;
         fval = obm_pc_pm_pkt_parse_cfg->user_protocol_matching[i];
         BCMTM_PT_DYN_INFO(pt_info, i, inst);
         SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
         fid = OB_PRIORITYf;
         fval = obm_pc_pm_pkt_parse_cfg->traffic_class;
         SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
         fid = ETHERTYPEf;
         fval = obm_pc_pm_pkt_parse_cfg->ethertype[i];
         SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
         fid = ETHERTYPE_MASKf;
         fval = obm_pc_pm_pkt_parse_cfg->ethertype_mask[i];
         SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
         SHR_IF_ERR_EXIT
           (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
         sal_memset(ltmbuf, 0, sizeof(ltmbuf));
    }

    sid = innertp[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = TPIDf;
    fval = obm_pc_pm_pkt_parse_cfg->inner_tpid;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = outertp0[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = TPIDf;
    fval = obm_pc_pm_pkt_parse_cfg->outer_tpid[0];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = outertp1[reg_idx];
    fid = TPIDf;
    fval = obm_pc_pm_pkt_parse_cfg->outer_tpid[1];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = outertp2[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = TPIDf;
    fval = obm_pc_pm_pkt_parse_cfg->outer_tpid[2];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = outertp3[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = TPIDf;
    fval = obm_pc_pm_pkt_parse_cfg->outer_tpid[3];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = etageth[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = ETHERTYPEf;
    fval = obm_pc_pm_pkt_parse_cfg->etag_ethertype;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    fid = ENABLEf;
    fval = obm_pc_pm_pkt_parse_cfg->etag_parsing;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = vntageth[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = ETHERTYPEf;
    fval = obm_pc_pm_pkt_parse_cfg->vntag_ethertype;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    fid = ENABLEf;
    fval = obm_pc_pm_pkt_parse_cfg->vntag_parsing;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));

    sid = gsheth[reg_idx];
    BCMTM_PT_DYN_INFO(pt_info, 0 , inst);
    fid = ETHERTYPEf;
    fval = obm_pc_pm_pkt_parse_cfg->gsh_ethertype;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    fid = MASKf;
    fval = obm_pc_pm_pkt_parse_cfg->gsh_ethertype_mask;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
      (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    sal_memset(ltmbuf,0,sizeof(ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief  OBM port packet parsing configuration.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_pc_cfg OBM PC configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_port_pkt_parse_set(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          bcmtm_obm_port_pkt_parse_cfg_t *obm_pc_cfg)
{
    bcmdrd_sid_t sid_list[] = {
        IDB_OBM0_PORT_CONFIGr, IDB_OBM1_PORT_CONFIGr, IDB_OBM2_PORT_CONFIGr,
        IDB_OBM3_PORT_CONFIGr, IDB_OBM4_PORT_CONFIGr };
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0}, fval;
    int pipe, pm_num, sub_port, pport;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    sub_port = bcm56780_a0_tm_get_subport(pport);
    if (bcmtm_lport_is_mgmt(unit, lport)) {
        sid = MGMT_OBM_PORT_CONFIGr;
        BCMTM_PT_DYN_INFO(pt_info, sub_port, 0);
    } else {
        pm_num = bcm56780_a0_tm_get_pipe_pm(pport);
        sid = sid_list[pm_num];
        BCMTM_PT_DYN_INFO(pt_info, sub_port, pipe);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    /* HEADER_TYPEf */
    fid = PORT_TYPEf;
    fval = obm_pc_cfg->header_type;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* DSCP_MAPPINGf */
    fid = TRUST_DSCPf;
    fval = obm_pc_cfg->enable_dscp_mapping;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* MPLS_MAPPINGf */
    fid = TRUST_MPLS_TCf;
    fval = obm_pc_cfg->enable_mpls_mapping;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* ETAG_MAPPINGf */
    fid = PHB_FROM_ETAGf;
    fval = obm_pc_cfg->enable_etag_mapping;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* INNER_TPIDf */
    fid = INNER_TPID_ENABLEf;
    fval = obm_pc_cfg->enable_inner_tpid;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* Enable OUTER_TPID_ID_0f */
    fid = OUTER_TPID_ENABLEf;
    fval = obm_pc_cfg->enable_outer_tpid[0];
    fval = fval |(obm_pc_cfg->enable_outer_tpid[1] << 1);
    fval = fval |(obm_pc_cfg->enable_outer_tpid[2] << 2);
    fval = fval |(obm_pc_cfg->enable_outer_tpid[3] << 3);

    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* PORT_PRIf */
    fid = PORT_PRIf;
    fval = obm_pc_cfg->default_pri;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* write to hardware */
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief OBM Flow control configuations.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_fc_cfg OBM flow control configurations.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_fc_set(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_lport_t lport,
                          bcmtm_obm_fc_cfg_t *obm_fc_cfg)
{
    bcmdrd_sid_t sid_list[] = {
        IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
        IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr,
        IDB_OBM4_FLOW_CONTROL_CONFIGr };
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[BCMTM_MAX_ENTRY_WSIZE] = {0}, fval;
    int pipe, pm_num, sub_port, pport;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    sub_port = bcm56780_a0_tm_get_subport(pport);

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        sid = MGMT_OBM_FLOW_CONTROL_CONFIGr;
        BCMTM_PT_DYN_INFO(pt_info, sub_port, 0);
    } else {
        pm_num = bcm56780_a0_tm_get_pipe_pm(pport);
        sid = sid_list[pm_num];
        BCMTM_PT_DYN_INFO(pt_info, sub_port, pipe);
    }
    /* FC_TYPEf */
    fid = FC_TYPEf;
    fval = obm_fc_cfg->fc_type;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* PORT_FC_EN */
    fid = PORT_FC_ENf;
    fval = obm_fc_cfg->fc;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* LOSSLESS1_FC_EN */
    fid = LOSSLESS1_FC_ENf;
    fval = obm_fc_cfg->lossless1_fc;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* LOSSLESS0_FC_EN */
    fid = LOSSLESS0_FC_ENf;
    fval = obm_fc_cfg->lossless0_fc;
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* LOSSLESS1_PRIORITY_PROFILE */
    fid = LOSSLESS1_PRIORITY_PROFILEf;
    fval = obm_fc_cfg->lossless1_cos_bmap[0];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* LOSSLESS0_PRIORITY_PROFILE */
    fid = LOSSLESS0_PRIORITY_PROFILEf;
    fval = obm_fc_cfg->lossless0_cos_bmap[0];
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));

    /* write to hardware */
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief OBM packet priority traffic class configurations.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] pri_type Packet priority type.
 * \param [in] pkt_pri Packet priority.
 * \param [in] tc OBM traffic class.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_pkt_pri_tc_set(int unit,
                                  bcmltd_sid_t ltid,
                                  bcmtm_lport_t lport,
                                  uint8_t pri_type,
                                  uint8_t pkt_pri,
                                  uint8_t tc)
{
    bcmdrd_sid_t obm_pri_map_sid[][8] =
                {{IDB_OBM0_PRI_MAP_PORT0m, IDB_OBM0_PRI_MAP_PORT1m,
                  IDB_OBM0_PRI_MAP_PORT2m, IDB_OBM0_PRI_MAP_PORT3m,
                  IDB_OBM0_PRI_MAP_PORT4m, IDB_OBM0_PRI_MAP_PORT5m,
                  IDB_OBM0_PRI_MAP_PORT6m, IDB_OBM0_PRI_MAP_PORT7m},
                 {IDB_OBM1_PRI_MAP_PORT0m, IDB_OBM1_PRI_MAP_PORT1m,
                  IDB_OBM1_PRI_MAP_PORT2m, IDB_OBM1_PRI_MAP_PORT3m,
                  IDB_OBM1_PRI_MAP_PORT4m, IDB_OBM1_PRI_MAP_PORT5m,
                  IDB_OBM1_PRI_MAP_PORT6m, IDB_OBM1_PRI_MAP_PORT7m},
                 {IDB_OBM2_PRI_MAP_PORT0m, IDB_OBM2_PRI_MAP_PORT1m,
                  IDB_OBM2_PRI_MAP_PORT2m, IDB_OBM2_PRI_MAP_PORT3m,
                  IDB_OBM2_PRI_MAP_PORT4m, IDB_OBM2_PRI_MAP_PORT5m,
                  IDB_OBM2_PRI_MAP_PORT6m, IDB_OBM2_PRI_MAP_PORT7m},
                 {IDB_OBM3_PRI_MAP_PORT0m, IDB_OBM3_PRI_MAP_PORT1m,
                  IDB_OBM3_PRI_MAP_PORT2m, IDB_OBM3_PRI_MAP_PORT3m,
                  IDB_OBM3_PRI_MAP_PORT4m, IDB_OBM3_PRI_MAP_PORT5m,
                  IDB_OBM3_PRI_MAP_PORT6m, IDB_OBM3_PRI_MAP_PORT7m},
                 {IDB_OBM4_PRI_MAP_PORT0m, IDB_OBM4_PRI_MAP_PORT1m,
                  IDB_OBM4_PRI_MAP_PORT2m, IDB_OBM4_PRI_MAP_PORT3m,
                  IDB_OBM4_PRI_MAP_PORT4m, IDB_OBM4_PRI_MAP_PORT5m,
                  IDB_OBM4_PRI_MAP_PORT6m, IDB_OBM4_PRI_MAP_PORT7m},
                 {MGMT_OBM_PRI_MAP_PORT0m, MGMT_OBM_PRI_MAP_PORT1m}};
    bcmdrd_sid_t obm_dscp_map_sid[][8] =
                {{IDB_OBM0_DSCP_MAP_PORT0m, IDB_OBM0_DSCP_MAP_PORT1m,
                  IDB_OBM0_DSCP_MAP_PORT2m, IDB_OBM0_DSCP_MAP_PORT3m,
                  IDB_OBM0_DSCP_MAP_PORT4m, IDB_OBM0_DSCP_MAP_PORT5m,
                  IDB_OBM0_DSCP_MAP_PORT6m, IDB_OBM0_DSCP_MAP_PORT7m},
                 {IDB_OBM1_DSCP_MAP_PORT0m, IDB_OBM1_DSCP_MAP_PORT1m,
                  IDB_OBM1_DSCP_MAP_PORT2m, IDB_OBM1_DSCP_MAP_PORT3m,
                  IDB_OBM1_DSCP_MAP_PORT4m, IDB_OBM1_DSCP_MAP_PORT5m,
                  IDB_OBM1_DSCP_MAP_PORT6m, IDB_OBM1_DSCP_MAP_PORT7m},
                 {IDB_OBM2_DSCP_MAP_PORT0m, IDB_OBM2_DSCP_MAP_PORT1m,
                  IDB_OBM2_DSCP_MAP_PORT2m, IDB_OBM2_DSCP_MAP_PORT3m,
                  IDB_OBM2_DSCP_MAP_PORT4m, IDB_OBM2_DSCP_MAP_PORT5m,
                  IDB_OBM2_DSCP_MAP_PORT6m, IDB_OBM2_DSCP_MAP_PORT7m},
                 {IDB_OBM3_DSCP_MAP_PORT0m, IDB_OBM3_DSCP_MAP_PORT1m,
                  IDB_OBM3_DSCP_MAP_PORT2m, IDB_OBM3_DSCP_MAP_PORT3m,
                  IDB_OBM3_DSCP_MAP_PORT4m, IDB_OBM3_DSCP_MAP_PORT5m,
                  IDB_OBM3_DSCP_MAP_PORT6m, IDB_OBM3_DSCP_MAP_PORT7m},
                 {IDB_OBM4_DSCP_MAP_PORT0m, IDB_OBM4_DSCP_MAP_PORT1m,
                  IDB_OBM4_DSCP_MAP_PORT2m, IDB_OBM4_DSCP_MAP_PORT3m,
                  IDB_OBM4_DSCP_MAP_PORT4m, IDB_OBM4_DSCP_MAP_PORT5m,
                  IDB_OBM4_DSCP_MAP_PORT6m, IDB_OBM4_DSCP_MAP_PORT7m},
                  {MGMT_OBM_DSCP_MAP_PORT0m, MGMT_OBM_DSCP_MAP_PORT1m}};

   bcmdrd_sid_t obm_mpls_map_sid[][8] =
                {{IDB_OBM0_TC_MAP_PORT0m, IDB_OBM0_TC_MAP_PORT1m,
                  IDB_OBM0_TC_MAP_PORT2m, IDB_OBM0_TC_MAP_PORT3m,
                  IDB_OBM0_TC_MAP_PORT4m, IDB_OBM0_TC_MAP_PORT5m,
                  IDB_OBM0_TC_MAP_PORT6m, IDB_OBM0_TC_MAP_PORT7m},
                 {IDB_OBM1_TC_MAP_PORT0m, IDB_OBM1_TC_MAP_PORT1m,
                  IDB_OBM1_TC_MAP_PORT2m, IDB_OBM1_TC_MAP_PORT3m,
                  IDB_OBM1_TC_MAP_PORT4m, IDB_OBM1_TC_MAP_PORT5m,
                  IDB_OBM1_TC_MAP_PORT6m, IDB_OBM1_TC_MAP_PORT7m},
                 {IDB_OBM2_TC_MAP_PORT0m, IDB_OBM2_TC_MAP_PORT1m,
                  IDB_OBM2_TC_MAP_PORT2m, IDB_OBM2_TC_MAP_PORT3m,
                  IDB_OBM2_TC_MAP_PORT4m, IDB_OBM2_TC_MAP_PORT5m,
                  IDB_OBM2_TC_MAP_PORT6m, IDB_OBM2_TC_MAP_PORT7m},
                 {IDB_OBM3_TC_MAP_PORT0m, IDB_OBM3_TC_MAP_PORT1m,
                  IDB_OBM3_TC_MAP_PORT2m, IDB_OBM3_TC_MAP_PORT3m,
                  IDB_OBM3_TC_MAP_PORT4m, IDB_OBM3_TC_MAP_PORT5m,
                  IDB_OBM3_TC_MAP_PORT6m, IDB_OBM3_TC_MAP_PORT7m},
                 {IDB_OBM4_TC_MAP_PORT0m, IDB_OBM4_TC_MAP_PORT1m,
                  IDB_OBM4_TC_MAP_PORT2m, IDB_OBM4_TC_MAP_PORT3m,
                  IDB_OBM4_TC_MAP_PORT4m, IDB_OBM4_TC_MAP_PORT5m,
                  IDB_OBM4_TC_MAP_PORT6m, IDB_OBM4_TC_MAP_PORT7m},
                 {MGMT_OBM_TC_MAP_PORT0m, MGMT_OBM_TC_MAP_PORT1m}};
   bcmdrd_sid_t obm_etag_map_sid[][8] =
                {{IDB_OBM0_ETAG_MAP_PORT0m, IDB_OBM0_ETAG_MAP_PORT1m,
                  IDB_OBM0_ETAG_MAP_PORT2m, IDB_OBM0_ETAG_MAP_PORT3m,
                  IDB_OBM0_ETAG_MAP_PORT4m, IDB_OBM0_ETAG_MAP_PORT5m,
                  IDB_OBM0_ETAG_MAP_PORT6m, IDB_OBM0_ETAG_MAP_PORT7m},
                 {IDB_OBM1_ETAG_MAP_PORT0m, IDB_OBM1_ETAG_MAP_PORT1m,
                  IDB_OBM1_ETAG_MAP_PORT2m, IDB_OBM1_ETAG_MAP_PORT3m,
                  IDB_OBM1_ETAG_MAP_PORT4m, IDB_OBM1_ETAG_MAP_PORT5m,
                  IDB_OBM1_ETAG_MAP_PORT6m, IDB_OBM1_ETAG_MAP_PORT7m},
                 {IDB_OBM2_ETAG_MAP_PORT0m, IDB_OBM2_ETAG_MAP_PORT1m,
                  IDB_OBM2_ETAG_MAP_PORT2m, IDB_OBM2_ETAG_MAP_PORT3m,
                  IDB_OBM2_ETAG_MAP_PORT4m, IDB_OBM2_ETAG_MAP_PORT5m,
                  IDB_OBM2_ETAG_MAP_PORT6m, IDB_OBM2_ETAG_MAP_PORT7m},
                 {IDB_OBM3_ETAG_MAP_PORT0m, IDB_OBM3_ETAG_MAP_PORT1m,
                  IDB_OBM3_ETAG_MAP_PORT2m, IDB_OBM3_ETAG_MAP_PORT3m,
                  IDB_OBM3_ETAG_MAP_PORT4m, IDB_OBM3_ETAG_MAP_PORT5m,
                  IDB_OBM3_ETAG_MAP_PORT6m, IDB_OBM3_ETAG_MAP_PORT7m},
                 {IDB_OBM4_ETAG_MAP_PORT0m, IDB_OBM4_ETAG_MAP_PORT1m,
                  IDB_OBM4_ETAG_MAP_PORT2m, IDB_OBM4_ETAG_MAP_PORT3m,
                  IDB_OBM4_ETAG_MAP_PORT4m, IDB_OBM4_ETAG_MAP_PORT5m,
                  IDB_OBM4_ETAG_MAP_PORT6m, IDB_OBM4_ETAG_MAP_PORT7m},
                 {MGMT_OBM_ETAG_MAP_PORT0m, MGMT_OBM_ETAG_MAP_PORT1m}};

    bcmdrd_fid_t obm_pri_fid[] = {
                  OFFSET0_OB_PRIORITYf, OFFSET1_OB_PRIORITYf,
                  OFFSET2_OB_PRIORITYf, OFFSET3_OB_PRIORITYf,
                  OFFSET4_OB_PRIORITYf, OFFSET5_OB_PRIORITYf,
                  OFFSET6_OB_PRIORITYf, OFFSET7_OB_PRIORITYf,
                  OFFSET8_OB_PRIORITYf, OFFSET9_OB_PRIORITYf,
                  OFFSET10_OB_PRIORITYf, OFFSET11_OB_PRIORITYf,
                  OFFSET12_OB_PRIORITYf, OFFSET13_OB_PRIORITYf,
                  OFFSET14_OB_PRIORITYf, OFFSET15_OB_PRIORITYf};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf, fval;
    int pport, pm_num, sub_port, pipe, idx, reg_idx, inst;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    pm_num = bcm56780_a0_tm_get_pipe_pm(pport);
    sub_port = bcm56780_a0_tm_get_subport(pport);
    fval = tc;

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        pm_num = 5;
        reg_idx = sub_port;
        sub_port = 0;
        inst = 0;
    } else {
        reg_idx = sub_port;
        inst = pipe;
    }
    switch (pri_type) {
        case OBM_PRIORITY_TYPE_VLAN:
            sid = obm_pri_map_sid[pm_num][reg_idx];
            fid = obm_pri_fid[pkt_pri];
            BCMTM_PT_DYN_INFO(pt_info, 0, inst);
            break;
        case OBM_PRIORITY_TYPE_DSCP:
            sid = obm_dscp_map_sid[pm_num][reg_idx];
            fid = obm_pri_fid[pkt_pri & 0xf];
            idx = pkt_pri >> 4;
            BCMTM_PT_DYN_INFO(pt_info, idx, inst);
            break;
        case OBM_PRIORITY_TYPE_MPLS:
            sid = obm_mpls_map_sid[pm_num][reg_idx];
            fid = obm_pri_fid[pkt_pri];
            BCMTM_PT_DYN_INFO(pt_info, sub_port, inst);
           break;
        case OBM_PRIORITY_TYPE_ETAG:
            sid = obm_etag_map_sid[pm_num][reg_idx];
            fid = obm_pri_fid[pkt_pri];
            BCMTM_PT_DYN_INFO(pt_info, sub_port, inst);
           break;
        case OBM_PRIORITY_TYPE_HIGIG3:
            sid = obm_pri_map_sid[pm_num][reg_idx];
            fid = obm_pri_fid[pkt_pri & 0x7];
            idx = pkt_pri >> 3;
            BCMTM_PT_DYN_INFO(pt_info, idx, inst);
           break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, &ltmbuf));
    SHR_IF_ERR_EXIT(bcmtm_field_set(unit, sid, fid, &ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, &ltmbuf));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief OBM port usage reset.
 *
 * Reset the IDB OBM max usage counter to zero.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [in] obm_usg_cfg OBM usage configuration.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_port_usg_set(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_lport_t lport,
                                uint64_t flags,
                                bcmtm_obm_usg_cfg_t *obm_usg_cfg)
{
    bcmdrd_sid_t obm_max_usage[][3] =
          { {IDB_OBM0_MAX_USAGEr, IDB_OBM0_MAX_USAGE_1r, IDB_OBM0_MAX_USAGE_2r},
            {IDB_OBM1_MAX_USAGEr, IDB_OBM1_MAX_USAGE_1r, IDB_OBM1_MAX_USAGE_2r},
            {IDB_OBM2_MAX_USAGEr, IDB_OBM2_MAX_USAGE_1r, IDB_OBM2_MAX_USAGE_2r},
            {IDB_OBM3_MAX_USAGEr, IDB_OBM3_MAX_USAGE_1r, IDB_OBM3_MAX_USAGE_2r},
            {IDB_OBM4_MAX_USAGEr, IDB_OBM4_MAX_USAGE_1r, IDB_OBM4_MAX_USAGE_2r},
            {MGMT_OBM_MAX_USAGEr}};
    bcmdrd_fid_t obm_max_usage_fid[] = {
          PORT0_MAX_USAGEf, PORT1_MAX_USAGEf, PORT2_MAX_USAGEf, PORT3_MAX_USAGEf,
          PORT4_MAX_USAGEf, PORT5_MAX_USAGEf, PORT6_MAX_USAGEf, PORT7_MAX_USAGEf};
    int pipe, pport, sub_port, pm_num, inst;
    bcmtm_pt_info_t pt_info = {0};
    bcmdrd_sid_t sid;
    bcmdrd_sid_t fid;
    uint32_t ltmbuf[3] = {0}, fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    inst = pipe;
    pt_info.flags = flags;
    pm_num = bcm56780_a0_tm_get_pipe_pm(pport);
    sub_port = bcm56780_a0_tm_get_subport(pport);

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        pm_num = 5;
        inst = 0;
    }
    /* IDB_OBM_MAX_USAGEr */
    sid = obm_max_usage[pm_num][sub_port/3];
    fid = obm_max_usage_fid[sub_port];
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &fval));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the OBM port usage counters.
 *
 * \param [in] unit Logical unit ID.
 * \param [in] ltid Logical table ID.
 * \param [in] lport Logical port ID.
 * \param [out] obm_usg_cfg OBM usage. Counter values for OBM.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_obm_port_usg_get(int unit,
                                bcmltd_sid_t ltid,
                                bcmtm_lport_t lport,
                                uint64_t flags,
                                bcmtm_obm_usg_cfg_t *obm_usg_cfg)
{
    bcmdrd_sid_t obm_usage[] = { IDB_OBM0_USAGEr, IDB_OBM1_USAGEr,
                                 IDB_OBM2_USAGEr, IDB_OBM3_USAGEr,
                                 IDB_OBM4_USAGEr, MGMT_OBM_USAGEr};
    bcmdrd_sid_t obm_usage1[] = { IDB_OBM0_USAGE_1r, IDB_OBM1_USAGE_1r,
                                  IDB_OBM2_USAGE_1r, IDB_OBM3_USAGE_1r,
                                  IDB_OBM4_USAGE_1r, MGMT_OBM_USAGEr};
    bcmdrd_sid_t obm_max_usage[][3] =
          { {IDB_OBM0_MAX_USAGEr, IDB_OBM0_MAX_USAGE_1r, IDB_OBM0_MAX_USAGE_2r},
            {IDB_OBM1_MAX_USAGEr, IDB_OBM1_MAX_USAGE_1r, IDB_OBM1_MAX_USAGE_2r},
            {IDB_OBM2_MAX_USAGEr, IDB_OBM2_MAX_USAGE_1r, IDB_OBM2_MAX_USAGE_2r},
            {IDB_OBM3_MAX_USAGEr, IDB_OBM3_MAX_USAGE_1r, IDB_OBM3_MAX_USAGE_2r},
            {IDB_OBM4_MAX_USAGEr, IDB_OBM4_MAX_USAGE_1r, IDB_OBM4_MAX_USAGE_2r},
            {MGMT_OBM_MAX_USAGEr}};
    bcmdrd_fid_t obm_max_usage_fid[] = {
          PORT0_MAX_USAGEf, PORT1_MAX_USAGEf, PORT2_MAX_USAGEf, PORT3_MAX_USAGEf,
          PORT4_MAX_USAGEf, PORT5_MAX_USAGEf, PORT6_MAX_USAGEf, PORT7_MAX_USAGEf};
    int pipe, pport, sub_port, pm_num, inst;
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t ltmbuf[3] = {0}, fval = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(bcmtm_lport_pport_get(unit, lport, &pport));
    SHR_IF_ERR_EXIT(bcmtm_lport_pipe_get(unit, lport, &pipe));

    pm_num = bcm56780_a0_tm_get_pipe_pm(pport);
    sub_port = bcm56780_a0_tm_get_subport(pport);
    pt_info.flags = flags;
    inst = pipe;

    if (bcmtm_lport_is_mgmt(unit, lport)) {
        pm_num = 5;
        inst = 0;
    }
    /* IDB_OBMx_USAGEr */
    sid = obm_usage[pm_num];

    BCMTM_PT_DYN_INFO(pt_info, sub_port, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, LOSSY_COUNTf, ltmbuf, &fval));
    obm_usg_cfg->lossy_byte = fval;

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, LOSSLESS0_COUNTf, ltmbuf, &fval));
    obm_usg_cfg->lossless0_byte = fval;

    /* IDB_OBMx_USAGE_1r */
    sid = obm_usage1[pm_num];
    BCMTM_PT_DYN_INFO(pt_info, sub_port, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, TOTAL_COUNTf, ltmbuf, &fval));
    obm_usg_cfg->byte = fval;

    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, LOSSLESS1_COUNTf, ltmbuf, &fval));
    obm_usg_cfg->lossless1_byte = fval;

    /* IDB_OBM_MAX_USAGEr */
    sid = obm_max_usage[pm_num][sub_port/3];
    fid = obm_max_usage_fid[sub_port];
    BCMTM_PT_DYN_INFO(pt_info, 0, inst);
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &fval));
    obm_usg_cfg->max_usage_byte = fval;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief OBM port validation.
 * This checks if the port supports OBM settings.
 *
 * \param [in] unit Logical unit number.
 * \param [in] lport Logical port number.
 *
 * \retval 1 If the port supports OBM.
 * \retval 0 If the port donot supports OBM.
 */
static int
bcm56780_a0_tm_obm_port_validation(int unit, bcmtm_lport_t lport)
{
    if (bcmtm_lport_is_fp(unit, lport) || bcmtm_lport_is_mgmt(unit, lport)) {
        return 1;
    }
    return 0;
}
/*******************************************************************************
 * Public functions
 */
int
bcm56780_a0_tm_obm_drv_get(int unit, void *obm_drv)
{
    bcmtm_obm_drv_t bcm56780_a0_obm_drv = {
        .fc_set = bcm56780_a0_tm_obm_fc_set,
        .pkt_pri_tc_set = bcm56780_a0_tm_obm_pkt_pri_tc_set,
        .port_usg_set = bcm56780_a0_tm_obm_port_usg_set,
        .port_usg_get = bcm56780_a0_tm_obm_port_usg_get,
        .port_pkt_parse_set = bcm56780_a0_tm_obm_port_pkt_parse_set,
        .policy_set = bcm56780_a0_tm_obm_pc_pm_pkt_parse_set,
        .port_validation = bcm56780_a0_tm_obm_port_validation
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(obm_drv, SHR_E_INTERNAL);
    *((bcmtm_obm_drv_t *)obm_drv) = bcm56780_a0_obm_drv;
exit:
    SHR_FUNC_EXIT();
}

int
bcm56780_a0_tm_obm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmtm_obm_usage_pbmp_info_alloc(unit, warm));
exit:
    SHR_FUNC_EXIT();
}
