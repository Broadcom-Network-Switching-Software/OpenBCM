/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    hurricane4_xflow_macsec.c
 * Purpose:Maintains all the debug information for xflow-macsec
 *         feature for Hurricane4.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>


#ifdef INCLUDE_XFLOW_MACSEC
extern char * techsupport_xflow_macsec_diag_cmdlist[];
#ifndef BCM_SW_STATE_DUMP_DISABLE
extern char * techsupport_xflow_macsec_sw_dump_cmdlist[];
#endif

/* "xflow-macsec" feature's diag command list for specific device, currently NULL */
char * techsupport_xflow_macsec_hurricane4_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "xflow-macsec" feature's software dump command list for specific device, currently NULL */
char * techsupport_xflow_macsec_hurricane4_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/* Structure that maintains register list for "xflow-macsec" feature
 * for Hurricane4 device */
techsupport_reg_t techsupport_xflow_macsec_hurricane4_reg_list[] = {
    { SP_GLB_CTRLr, register_type_global },
    { SP_GLB_INTR_ENr, register_type_global },
    { SP_GLB_INTR_STATUSr, register_type_global },
    { SP_GLB_MIB_CTRLr, register_type_global },
    { SP_GLB_MIB_STS0r, register_type_global },
    { SP_GLB_MIB_STS1r, register_type_global },
    { SP_GLB_MISC_CTRLr, register_type_global },
    { SP_GLB_OLP_RX_TYPEr, register_type_global },
    { SP_GLB_PROG_CRCr, register_type_global },
    { SP_GLB_PRESCALEr, register_type_global },
    { SP_GLB_SA_STS_SAK_EXPIRED_P0r, register_type_global },
    { SP_GLB_SA_STS_SAK_EXPIRED_P1r, register_type_global },
    { SP_GLB_SA_STS_SAK_EXPIRED_P2r, register_type_global },
    { SP_GLB_SA_STS_SAK_EXPIRED_P3r, register_type_global },
    { SP_GLB_SECTAG_ETYPE0_1r, register_type_global },
    { SP_GLB_SECTAG_ETYPE2_3r, register_type_global },
    { SP_GLB_TIMETICKr, register_type_global },
    { SP_GLB_VERSION_IDr, register_type_global },

    {ESEC_AES_CALC_ICV_REG0r, register_type_global },
    {ESEC_AES_CALC_ICV_REG1r, register_type_global },
    {ESEC_AES_CALC_ICV_REG2r, register_type_global },
    {ESEC_AES_CALC_ICV_REG3r, register_type_global },
    {ESEC_AES_CNTRLr, register_type_global },
    {ESEC_AES_STATUSr, register_type_global },
    {ESEC_CONFIGr, register_type_global },
    {ESEC_EPBUF_STSr, register_type_global },
    {ESEC_EGRESS_MTU0_1r, register_type_global },
    {ESEC_EGRESS_MTU2_3r, register_type_global },
    {ESEC_EGRESS_MTU_FOR_MGMT_PKTr, register_type_global },
    {ESEC_FIPS_CONTROLr, register_type_global },
    {ESEC_INTR_ENr, register_type_global },
    {ESEC_INTR_STATUSr, register_type_global },
    {ESEC_PKTBUF_CTLr, register_type_global },
    {ESEC_PKTBUF_STSr, register_type_global },
    {ESEC_PN_THDr, register_type_global },
    {ESEC_XPN_THD0r, register_type_global },
    {ESEC_XPN_THD1r, register_type_global },

    { ISEC_AES_CALC_ICV_REG0r,register_type_global },
    { ISEC_AES_CALC_ICV_REG1r,register_type_global },
    { ISEC_AES_CALC_ICV_REG2r,register_type_global },
    { ISEC_AES_CALC_ICV_REG3r,register_type_global },
    { ISEC_AES_CNTRLr,register_type_global },
    { ISEC_AES_ICV_FAIL_CNTr,register_type_global },
    { ISEC_AES_ICV_PASS_CNTr,register_type_global },
    { ISEC_AES_RCV_ICV_REG0r,register_type_global },
    { ISEC_AES_RCV_ICV_REG1r,register_type_global },
    { ISEC_AES_RCV_ICV_REG2r,register_type_global },
    { ISEC_AES_RCV_ICV_REG3r,register_type_global },
    { ISEC_AES_STATUSr,register_type_global },
    { ISEC_CNTRLr,register_type_global },
    { ISEC_CPU_MAC_ADDR0r,register_type_global },
    { ISEC_CPU_MAC_ADDR1r,register_type_global },
    { ISEC_ETYPE_CONFIGr,register_type_global },
    { ISEC_ETYPE_MAX_LENr,register_type_global },
    { ISEC_FRMVLDT_RULE_CFGr,register_type_global },
    { ISEC_FRM_VLDT_RULEr,register_type_global },
    { ISEC_HR4_GLB_CTRLr,register_type_global },
    { ISEC_INTR_ENr,register_type_global },
    { ISEC_INTR_STATUSr,register_type_global },
    { ISEC_MPLS_ETYPEr,register_type_global },
    { ISEC_MTU0_1r,register_type_global },
    { ISEC_MTU2_3r,register_type_global },
    { ISEC_PKTBUF_STSr,register_type_global },
    { ISEC_PN_EXPIRE_THDr,register_type_global },
    { ISEC_PP_CNTRLr,register_type_global },
    { ISEC_RUD_MGMT_RULEr,register_type_global },
    { ISEC_TPID0r,register_type_global },
    { ISEC_TPID1r,register_type_global },
    { ISEC_TPID2r,register_type_global },
    { ISEC_TPID3r,register_type_global },
    { ISEC_XPN_EXPIRE_THD0r,register_type_global },
    { ISEC_XPN_EXPIRE_THD1r,register_type_global },

    {INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains memory list for "xflow-macsec" feature
 * for Hurricane4 device */
static soc_mem_t techsupport_xflow_macsec_hurricane4_memory_table_list[] = {
    ISEC_DROP_COUNTERSm,
    ISEC_INMGMTPKTS_COUNTm,
    ISEC_MIB0m,
    ISEC_MIB0_Bm,
    ISEC_MIB1m,
    ISEC_MIB2m,
    ISEC_MISC_COUNTERSm,
    ISEC_SA_TABLEm,
    ISEC_SCTCAM_HIT_COUNTm,
    ISEC_SC_TABLEm,
    ISEC_SC_TCAMm,
    ISEC_SPTCAM_HIT_COUNTm,
    ISEC_SP_TCAMm,
    ESEC_DROP_COUNTERSm,
    ESEC_MIB0m,
    ESEC_MIB1m,
    ESEC_MISC_COUNTERSm,
    ESEC_OUTMGMTPKTS_COUNTm,
    ESEC_SA_TABLEm,
    ESEC_SC_TABLEm,
    SP_GLB_EGRESS_SA_EXPIRE_STATUSm,
    SP_GLB_EGRESS_SA_SOFT_EXPIRE_STATUSm,
    SP_GLB_INGRESS_SA_EXPIRE_STATUSm,
    SP_GLB_INGRESS_SA_SOFT_EXPIRE_STATUSm,

    INVALIDm /* Must be the last element in this structure */
};

/* xflow-macsec data structure */
techsupport_data_t techsupport_xflow_macsec_hurricane4_data = {
    techsupport_xflow_macsec_diag_cmdlist,
    techsupport_xflow_macsec_hurricane4_reg_list,
    techsupport_xflow_macsec_hurricane4_memory_table_list,
    techsupport_xflow_macsec_hurricane4_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_xflow_macsec_sw_dump_cmdlist,
    techsupport_xflow_macsec_hurricane4_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
#endif
