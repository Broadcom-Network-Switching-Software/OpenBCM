
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOC_INIT

#include <soc/sand/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <soc/dnx/access.h>
#include <soc/drv.h>
#include <soc/led.h>
#include <soc/mem.h>
#include <soc/sand/sand_mem.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/sand/sand_mbist.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnxc/drv.h>
#include <soc/mcm/driver.h>
#include <soc/linkctrl.h>
#include <bcm_int/dnx/port/imb/imb_ile.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#include <bcm_int/dnx/dram/buffers/buffers_quarantine_mechanism.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnxc/dnxc_adapter_reg_access.h>
#include <soc/dnxc/dnxc_cmic.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/iproc_m0ssq.h>
#include <soc/dnxc/dnxc_mem.h>
#include <soc/ipoll.h>
#include <soc/intr_cmicx.h>
#include <soc/dcb.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <bcm_int/dnx/dram/dram.h>
#include <src/bcm/dnx/cosq/scheduler/scheduler_dbal.h>

#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_link.h>
#define CMICX_HW_LINKSCAN 1
#endif

#ifdef BCM_ACCESS_SUPPORT
#include <soc/access/access.h>
#include <soc/dnxc/dnxc_access.h>
#include <soc/access/auto_generated/common_enum.h>
#endif

extern const dnx_init_step_t dnx_init_deinit_seq[];

dnx_startup_test_function_f dnx_startup_test_functions[SOC_MAX_NUM_DEVICES] = { 0 };

const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_1_global_register_block_registers[] = {
    DDP_REG_00C3r,
    FCT_REG_00C3r,
    FDR_REG_00C3r,
    FDTL_REG_00C3r,
    FDT_REG_00C3r,
    FMAC_REG_00C3r,
    IPS_REG_00C3r,
    IPT_REG_00C3r,
    OLP_REG_00C3r,
    RQP_REG_00C3r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_2_global_register_block_registers[] = {
    CDBM_REG_00C4r,
    CDB_REG_00C4r,
    CDPM_REG_00C4r,
    DDP_REG_00C4r,
    FCR_REG_00C4r,
    FCT_REG_00C4r,
    FDR_REG_00C4r,
    FDTL_REG_00C4r,
    FDT_REG_00C4r,
    IPS_REG_00C4r,
    IPT_REG_00C4r,
    IRE_REG_00C4r,
    MESH_TOPOLOGY_REG_00C4r,
    MSD_REG_00C4r,
    MSS_REG_00C4r,
    NMG_REG_00C4r,
    RTP_REG_00C4r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_CFG_3_global_register_block_registers[] = {
    CGM_REG_00C5r,
    DDP_REG_00C5r,
    DQM_REG_00C5r,
    ECGM_REG_00C5r,
    EPNI_REG_00C5r,
    EPRE_REG_00C5r,
    ERPP_REG_00C5r,
    ETPPA_REG_00C5r,
    ETPPB_REG_00C5r,
    ETPPC_REG_00C5r,
    EVNT_REG_00C5r,
    FDR_REG_00C5r,
    FDT_REG_00C5r,
    FQP_REG_00C5r,
    IPPB_REG_00C5r,
    IPPD_REG_00C5r,
    IPT_REG_00C5r,
    IRE_REG_00C5r,
    PQP_REG_00C5r,
    RQP_REG_00C5r,
    SPB_REG_00C5r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_GENERAL_IDB_global_register_block_registers[] = {
    CGM_REG_00C7r,
    DDP_REG_00C7r,
    DQM_REG_00C7r,
    SQM_REG_00C7r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_1_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_10_global_register_block_registers[] = {
    ERPP_REG_00F0r,
    ETPPA_REG_00F0r,
    ETPPB_REG_00F0r,
    ETPPC_REG_00F0r,
    IPPA_REG_00F0r,
    IPPB_REG_00F0r,
    IPPC_REG_00F0r,
    IPPD_REG_00F0r,
    IPPF_REG_00F0r,
    MDB_REG_00F0r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_11_global_register_block_registers[] = {
    ERPP_REG_00F1r,
    ETPPA_REG_00F1r,
    ETPPB_REG_00F1r,
    ETPPC_REG_00F1r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_12_global_register_block_registers[] = {
    ERPP_REG_00F2r,
    ETPPA_REG_00F2r,
    ETPPB_REG_00F2r,
    ETPPC_REG_00F2r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_2_global_register_block_registers[] = {
    ERPP_REG_00E8r,
    ETPPA_REG_00E8r,
    ETPPB_REG_00E8r,
    ETPPC_REG_00E8r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_3_global_register_block_registers[] = {
    ERPP_REG_00E9r,
    ETPPA_REG_00E9r,
    ETPPB_REG_00E9r,
    ETPPC_REG_00E9r,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_4_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_5_global_register_block_registers[] = {
    ERPP_REG_00EBr,
    ETPPA_REG_00EBr,
    ETPPB_REG_00EBr,
    ETPPC_REG_00EBr,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_6_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_7_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_8_global_register_block_registers[] = {
    ERPP_REG_00EEr,
    ETPPA_REG_00EEr,
    ETPPB_REG_00EEr,
    ETPPC_REG_00EEr,
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_PP_9_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2P_ECI_FAP_GLOBAL_SYS_HEADER_CFG_global_register_block_registers[] = {
    DDP_REG_00C6r,
    ERPP_REG_00C6r,
    ETPPA_REG_00C6r,
    ETPPB_REG_00C6r,
    ETPPC_REG_00C6r,
    IPPD_REG_00C6r,
    IPT_REG_00C6r,
    IRE_REG_00C6r,
    ITPPD_REG_00C6r,
    ITPP_REG_00C6r,
    OAMP_REG_00C6r,
    RQP_REG_00C6r,
    -1
};

const soc_reg_t J2P_ECI_GLOBAL_MEM_OPTIONS_global_register_block_registers[] = {
    BDM_REG_00C2r,
    CDBM_REG_00C2r,
    CDB_REG_00C2r,
    CDPM_REG_00C2r,
    CFC_REG_00C2r,
    CGM_REG_00C2r,
    CRPS_REG_00C2r,
    DDHA_REG_00C2r,
    DDHB_REG_00C2r,
    DDP_REG_00C2r,
    DQM_REG_00C2r,
    ECGM_REG_00C2r,
    EDB_REG_00C2r,
    EPNI_REG_00C2r,
    EPRE_REG_00C2r,
    EPS_REG_00C2r,
    ERPP_REG_00C2r,
    ESB_REG_00C2r,
    ETPPA_REG_00C2r,
    ETPPB_REG_00C2r,
    ETPPC_REG_00C2r,
    EVNT_REG_00C2r,
    FCR_REG_00C2r,
    FCT_REG_00C2r,
    FDA_REG_00C2r,
    FDR_REG_00C2r,
    FDTL_REG_00C2r,
    FDT_REG_00C2r,
    FMAC_REG_00C2r,
    FQP_REG_00C2r,
    FSRD_REG_00C2r,
    HBC_REG_00C2r,
    HRC_REG_00C2r,
    ILE_REG_00C2r,
    IPPA_REG_00C2r,
    IPPB_REG_00C2r,
    IPPC_REG_00C2r,
    IPPD_REG_00C2r,
    IPPE_REG_00C2r,
    IPPF_REG_00C2r,
    IPS_REG_00C2r,
    IPT_REG_00C2r,
    IQM_REG_00C2r,
    IRE_REG_00C2r,
    ITPPD_REG_00C2r,
    ITPP_REG_00C2r,
    KAPS_REG_00C2r,
    MACT_REG_00C2r,
    MCP_REG_00C2r,
    MDB_REG_00C2r,
    MRPS_REG_00C2r,
    MSD_REG_00C2r,
    MSS_REG_00C2r,
    MTM_REG_00C2r,
    NMG_REG_00C2r,
    OAMP_REG_00C2r,
    OCBM_REG_00C2r,
    OCB_REG_00C2r,
    OLP_REG_00C2r,
    PDM_REG_00C2r,
    PEM_REG_00C2r,
    PQP_REG_00C2r,
    RQP_REG_00C2r,
    RTP_REG_00C2r,
    SCH_REG_00C2r,
    SIF_REG_00C2r,
    SPB_REG_00C2r,
    SQM_REG_00C2r,
    TCAM_REG_00C2r,
    TDU_REG_00C2r,
    -1
};

const soc_reg_t J2P_ECI_GTIMER_CONFIGURATION_global_register_block_registers[] = {
    BDM_GTIMER_CONFIGURATIONr,
    CDBM_GTIMER_CONFIGURATIONr,
    CDB_GTIMER_CONFIGURATIONr,
    CDPM_GTIMER_CONFIGURATIONr,
    CFC_GTIMER_CONFIGURATIONr,
    CGM_GTIMER_CONFIGURATIONr,
    CRPS_GTIMER_CONFIGURATIONr,
    DDHA_GTIMER_CONFIGURATIONr,
    DDHB_GTIMER_CONFIGURATIONr,
    DDP_GTIMER_CONFIGURATIONr,
    DQM_GTIMER_CONFIGURATIONr,
    ECGM_GTIMER_CONFIGURATIONr,
    EDB_GTIMER_CONFIGURATIONr,
    EPNI_GTIMER_CONFIGURATIONr,
    EPRE_GTIMER_CONFIGURATIONr,
    EPS_GTIMER_CONFIGURATIONr,
    ERPP_GTIMER_CONFIGURATIONr,
    ESB_GTIMER_CONFIGURATIONr,
    ETPPA_GTIMER_CONFIGURATIONr,
    ETPPB_GTIMER_CONFIGURATIONr,
    ETPPC_GTIMER_CONFIGURATIONr,
    EVNT_GTIMER_CONFIGURATIONr,
    FCR_GTIMER_CONFIGURATIONr,
    FCT_GTIMER_CONFIGURATIONr,
    FDA_GTIMER_CONFIGURATIONr,
    FDR_GTIMER_CONFIGURATIONr,
    FDTL_GTIMER_CONFIGURATIONr,
    FDT_GTIMER_CONFIGURATIONr,
    FMAC_GTIMER_CONFIGURATIONr,
    FQP_GTIMER_CONFIGURATIONr,
    FSRD_GTIMER_CONFIGURATIONr,
    HBC_GTIMER_CONFIGURATIONr,
    HBMC_GTIMER_CONFIGURATIONr,
    HRC_GTIMER_CONFIGURATIONr,
    ILE_GTIMER_CONFIGURATIONr,
    IPPA_GTIMER_CONFIGURATIONr,
    IPPB_GTIMER_CONFIGURATIONr,
    IPPC_GTIMER_CONFIGURATIONr,
    IPPD_GTIMER_CONFIGURATIONr,
    IPPE_GTIMER_CONFIGURATIONr,
    IPPF_GTIMER_CONFIGURATIONr,
    IPS_GTIMER_CONFIGURATIONr,
    IPT_GTIMER_CONFIGURATIONr,
    IQM_GTIMER_CONFIGURATIONr,
    IRE_GTIMER_CONFIGURATIONr,
    ITPPD_GTIMER_CONFIGURATIONr,
    ITPP_GTIMER_CONFIGURATIONr,
    KAPS_GTIMER_CONFIGURATIONr,
    MACT_GTIMER_CONFIGURATIONr,
    MCP_GTIMER_CONFIGURATIONr,
    MDB_GTIMER_CONFIGURATIONr,
    MESH_TOPOLOGY_GTIMER_CONFIGURATIONr,
    MRPS_GTIMER_CONFIGURATIONr,
    MSD_GTIMER_CONFIGURATIONr,
    MSS_GTIMER_CONFIGURATIONr,
    MTM_GTIMER_CONFIGURATIONr,
    NMG_GTIMER_CONFIGURATIONr,
    OAMP_GTIMER_CONFIGURATIONr,
    OCBM_GTIMER_CONFIGURATIONr,
    OCB_GTIMER_CONFIGURATIONr,
    OLP_GTIMER_CONFIGURATIONr,
    PDM_GTIMER_CONFIGURATIONr,
    PEM_GTIMER_CONFIGURATIONr,
    PQP_GTIMER_CONFIGURATIONr,
    RQP_GTIMER_CONFIGURATIONr,
    RTP_GTIMER_CONFIGURATIONr,
    SCH_GTIMER_CONFIGURATIONr,
    SIF_GTIMER_CONFIGURATIONr,
    SPB_GTIMER_CONFIGURATIONr,
    SQM_GTIMER_CONFIGURATIONr,
    TCAM_GTIMER_CONFIGURATIONr,
    TDU_GTIMER_CONFIGURATIONr,
    -1
};

const soc_reg_t J2P_ECI_GTIMER_CYCLE_global_register_block_registers[] = {
    BDM_GTIMER_CYCLEr,
    CDBM_GTIMER_CYCLEr,
    CDB_GTIMER_CYCLEr,
    CDPM_GTIMER_CYCLEr,
    CFC_GTIMER_CYCLEr,
    CGM_GTIMER_CYCLEr,
    CRPS_GTIMER_CYCLEr,
    DDHA_GTIMER_CYCLEr,
    DDHB_GTIMER_CYCLEr,
    DDP_GTIMER_CYCLEr,
    DQM_GTIMER_CYCLEr,
    ECGM_GTIMER_CYCLEr,
    EDB_GTIMER_CYCLEr,
    EPNI_GTIMER_CYCLEr,
    EPRE_GTIMER_CYCLEr,
    EPS_GTIMER_CYCLEr,
    ERPP_GTIMER_CYCLEr,
    ESB_GTIMER_CYCLEr,
    ETPPA_GTIMER_CYCLEr,
    ETPPB_GTIMER_CYCLEr,
    ETPPC_GTIMER_CYCLEr,
    EVNT_GTIMER_CYCLEr,
    FCR_GTIMER_CYCLEr,
    FCT_GTIMER_CYCLEr,
    FDA_GTIMER_CYCLEr,
    FDR_GTIMER_CYCLEr,
    FDTL_GTIMER_CYCLEr,
    FDT_GTIMER_CYCLEr,
    FMAC_GTIMER_CYCLEr,
    FQP_GTIMER_CYCLEr,
    FSRD_GTIMER_CYCLEr,
    HBC_GTIMER_CYCLEr,
    HBMC_GTIMER_CYCLEr,
    HRC_GTIMER_CYCLEr,
    ILE_GTIMER_CYCLEr,
    IPPA_GTIMER_CYCLEr,
    IPPB_GTIMER_CYCLEr,
    IPPC_GTIMER_CYCLEr,
    IPPD_GTIMER_CYCLEr,
    IPPE_GTIMER_CYCLEr,
    IPPF_GTIMER_CYCLEr,
    IPS_GTIMER_CYCLEr,
    IPT_GTIMER_CYCLEr,
    IQM_GTIMER_CYCLEr,
    IRE_GTIMER_CYCLEr,
    ITPPD_GTIMER_CYCLEr,
    ITPP_GTIMER_CYCLEr,
    KAPS_GTIMER_CYCLEr,
    MACT_GTIMER_CYCLEr,
    MCP_GTIMER_CYCLEr,
    MDB_GTIMER_CYCLEr,
    MESH_TOPOLOGY_GTIMER_CYCLEr,
    MRPS_GTIMER_CYCLEr,
    MSD_GTIMER_CYCLEr,
    MSS_GTIMER_CYCLEr,
    MTM_GTIMER_CYCLEr,
    NMG_GTIMER_CYCLEr,
    OAMP_GTIMER_CYCLEr,
    OCBM_GTIMER_CYCLEr,
    OCB_GTIMER_CYCLEr,
    OLP_GTIMER_CYCLEr,
    PDM_GTIMER_CYCLEr,
    PEM_GTIMER_CYCLEr,
    PQP_GTIMER_CYCLEr,
    RQP_GTIMER_CYCLEr,
    RTP_GTIMER_CYCLEr,
    SCH_GTIMER_CYCLEr,
    SIF_GTIMER_CYCLEr,
    SPB_GTIMER_CYCLEr,
    SQM_GTIMER_CYCLEr,
    TCAM_GTIMER_CYCLEr,
    TDU_GTIMER_CYCLEr,
    -1
};

const soc_reg_t J2P_ECI_PCMI_0_global_register_block_registers[] = {
    CDBM_PCMI_0r,
    CDB_PCMI_0r,
    CDPM_PCMI_0r,
    CFC_PCMI_0r,
    CGM_PCMI_0r,
    DDHA_PCMI_0r,
    DDHB_PCMI_0r,
    EDB_PCMI_0r,
    EPNI_PCMI_0r,
    ERPP_PCMI_0r,
    ETPPA_PCMI_0r,
    ETPPB_PCMI_0r,
    ETPPC_PCMI_0r,
    FDA_PCMI_0r,
    FDR_PCMI_0r,
    FDTL_PCMI_0r,
    FDT_PCMI_0r,
    FQP_PCMI_0r,
    FSRD_PCMI_0r,
    HBMC_PCMI_0r,
    ILE_PCMI_0r,
    IPPA_PCMI_0r,
    IPPB_PCMI_0r,
    IPPC_PCMI_0r,
    IPPD_PCMI_0r,
    IPPE_PCMI_0r,
    IPPF_PCMI_0r,
    IPT_PCMI_0r,
    IQM_PCMI_0r,
    IRE_PCMI_0r,
    KAPS_PCMI_0r,
    MACT_PCMI_0r,
    MCP_PCMI_0r,
    MDB_PCMI_0r,
    MSD_PCMI_0r,
    MSS_PCMI_0r,
    MTM_PCMI_0r,
    OCB_PCMI_0r,
    PDM_PCMI_0r,
    PEM_PCMI_0r,
    RQP_PCMI_0r,
    SCH_PCMI_0r,
    SPB_PCMI_0r,
    TCAM_PCMI_0r,
    -1
};

const soc_reg_t J2P_ECI_PCMI_1_global_register_block_registers[] = {
    CDBM_PCMI_1r,
    CDB_PCMI_1r,
    CDPM_PCMI_1r,
    CFC_PCMI_1r,
    CGM_PCMI_1r,
    DDHA_PCMI_1r,
    DDHB_PCMI_1r,
    EDB_PCMI_1r,
    EPNI_PCMI_1r,
    ERPP_PCMI_1r,
    ETPPA_PCMI_1r,
    ETPPB_PCMI_1r,
    ETPPC_PCMI_1r,
    FDA_PCMI_1r,
    FDR_PCMI_1r,
    FDTL_PCMI_1r,
    FDT_PCMI_1r,
    FQP_PCMI_1r,
    HBMC_PCMI_1r,
    ILE_PCMI_1r,
    IPPA_PCMI_1r,
    IPPB_PCMI_1r,
    IPPC_PCMI_1r,
    IPPD_PCMI_1r,
    IPPE_PCMI_1r,
    IPPF_PCMI_1r,
    IPT_PCMI_1r,
    IQM_PCMI_1r,
    IRE_PCMI_1r,
    KAPS_PCMI_1r,
    MACT_PCMI_1r,
    MCP_PCMI_1r,
    MDB_PCMI_1r,
    MSD_PCMI_1r,
    MSS_PCMI_1r,
    MTM_PCMI_1r,
    OCB_PCMI_1r,
    PDM_PCMI_1r,
    PEM_PCMI_1r,
    RQP_PCMI_1r,
    SCH_PCMI_1r,
    SPB_PCMI_1r,
    TCAM_PCMI_1r,
    -1
};

const soc_reg_t J2P_ECI_PCMI_2_global_register_block_registers[] = {
    CDBM_PCMI_2r,
    CDB_PCMI_2r,
    CDPM_PCMI_2r,
    CFC_PCMI_2r,
    CGM_PCMI_2r,
    DDHA_PCMI_2r,
    DDHB_PCMI_2r,
    EDB_PCMI_2r,
    EPNI_PCMI_2r,
    ERPP_PCMI_2r,
    ETPPA_PCMI_2r,
    ETPPB_PCMI_2r,
    ETPPC_PCMI_2r,
    FDA_PCMI_2r,
    FDR_PCMI_2r,
    FDTL_PCMI_2r,
    FDT_PCMI_2r,
    FQP_PCMI_2r,
    HBMC_PCMI_2r,
    ILE_PCMI_2r,
    IPPA_PCMI_2r,
    IPPB_PCMI_2r,
    IPPC_PCMI_2r,
    IPPD_PCMI_2r,
    IPPE_PCMI_2r,
    IPPF_PCMI_2r,
    IPT_PCMI_2r,
    IQM_PCMI_2r,
    IRE_PCMI_2r,
    KAPS_PCMI_2r,
    MACT_PCMI_2r,
    MCP_PCMI_2r,
    MDB_PCMI_2r,
    MSD_PCMI_2r,
    MSS_PCMI_2r,
    MTM_PCMI_2r,
    OCB_PCMI_2r,
    PDM_PCMI_2r,
    PEM_PCMI_2r,
    RQP_PCMI_2r,
    SCH_PCMI_2r,
    SPB_PCMI_2r,
    TCAM_PCMI_2r,
    -1
};

const soc_reg_t J2P_ECI_PCMI_3_global_register_block_registers[] = {
    CDBM_PCMI_3r,
    CDB_PCMI_3r,
    CDPM_PCMI_3r,
    CFC_PCMI_3r,
    CGM_PCMI_3r,
    DDHA_PCMI_3r,
    DDHB_PCMI_3r,
    EDB_PCMI_3r,
    EPNI_PCMI_3r,
    ERPP_PCMI_3r,
    ETPPA_PCMI_3r,
    ETPPB_PCMI_3r,
    ETPPC_PCMI_3r,
    FDA_PCMI_3r,
    FDR_PCMI_3r,
    FDTL_PCMI_3r,
    FDT_PCMI_3r,
    FQP_PCMI_3r,
    HBMC_PCMI_3r,
    ILE_PCMI_3r,
    IPPA_PCMI_3r,
    IPPB_PCMI_3r,
    IPPC_PCMI_3r,
    IPPD_PCMI_3r,
    IPPE_PCMI_3r,
    IPPF_PCMI_3r,
    IPT_PCMI_3r,
    IQM_PCMI_3r,
    IRE_PCMI_3r,
    KAPS_PCMI_3r,
    MACT_PCMI_3r,
    MCP_PCMI_3r,
    MDB_PCMI_3r,
    MSD_PCMI_3r,
    MSS_PCMI_3r,
    MTM_PCMI_3r,
    OCB_PCMI_3r,
    PDM_PCMI_3r,
    PEM_PCMI_3r,
    RQP_PCMI_3r,
    SCH_PCMI_3r,
    SPB_PCMI_3r,
    TCAM_PCMI_3r,
    -1
};

const soc_reg_t J2P_ECI_PCMI_4_global_register_block_registers[] = {
    CDBM_PCMI_4r,
    CDB_PCMI_4r,
    CDPM_PCMI_4r,
    CFC_PCMI_4r,
    CGM_PCMI_4r,
    DDHA_PCMI_4r,
    DDHB_PCMI_4r,
    EDB_PCMI_4r,
    EPNI_PCMI_4r,
    ERPP_PCMI_4r,
    ETPPA_PCMI_4r,
    ETPPB_PCMI_4r,
    ETPPC_PCMI_4r,
    FDA_PCMI_4r,
    FDR_PCMI_4r,
    FDTL_PCMI_4r,
    FDT_PCMI_4r,
    FQP_PCMI_4r,
    HBMC_PCMI_4r,
    ILE_PCMI_4r,
    IPPA_PCMI_4r,
    IPPB_PCMI_4r,
    IPPC_PCMI_4r,
    IPPD_PCMI_4r,
    IPPE_PCMI_4r,
    IPPF_PCMI_4r,
    IPT_PCMI_4r,
    IQM_PCMI_4r,
    IRE_PCMI_4r,
    KAPS_PCMI_4r,
    MACT_PCMI_4r,
    MCP_PCMI_4r,
    MDB_PCMI_4r,
    MSD_PCMI_4r,
    MSS_PCMI_4r,
    MTM_PCMI_4r,
    OCB_PCMI_4r,
    PDM_PCMI_4r,
    PEM_PCMI_4r,
    RQP_PCMI_4r,
    SCH_PCMI_4r,
    SPB_PCMI_4r,
    TCAM_PCMI_4r,
    -1
};

const soc_reg_t J2P_ECI_PCMI_5_global_register_block_registers[] = {
    CDBM_PCMI_5r,
    CDB_PCMI_5r,
    CDPM_PCMI_5r,
    CFC_PCMI_5r,
    CGM_PCMI_5r,
    DDHA_PCMI_5r,
    DDHB_PCMI_5r,
    EDB_PCMI_5r,
    EPNI_PCMI_5r,
    ERPP_PCMI_5r,
    ETPPA_PCMI_5r,
    ETPPB_PCMI_5r,
    ETPPC_PCMI_5r,
    FDA_PCMI_5r,
    FDR_PCMI_5r,
    FDTL_PCMI_5r,
    FDT_PCMI_5r,
    FQP_PCMI_5r,
    HBMC_PCMI_5r,
    ILE_PCMI_5r,
    IPPA_PCMI_5r,
    IPPB_PCMI_5r,
    IPPC_PCMI_5r,
    IPPD_PCMI_5r,
    IPPE_PCMI_5r,
    IPPF_PCMI_5r,
    IPT_PCMI_5r,
    IQM_PCMI_5r,
    IRE_PCMI_5r,
    KAPS_PCMI_5r,
    MACT_PCMI_5r,
    MCP_PCMI_5r,
    MDB_PCMI_5r,
    MSD_PCMI_5r,
    MSS_PCMI_5r,
    MTM_PCMI_5r,
    OCB_PCMI_5r,
    PDM_PCMI_5r,
    PEM_PCMI_5r,
    RQP_PCMI_5r,
    SCH_PCMI_5r,
    SPB_PCMI_5r,
    TCAM_PCMI_5r,
    -1
};

const soc_reg_t *j2p_common_regs_addr_2_global_register_block_registers[257] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2P_ECI_GTIMER_CYCLE_global_register_block_registers,
    J2P_ECI_GTIMER_CONFIGURATION_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2P_ECI_PCMI_0_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_PCMI_1_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_PCMI_2_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_PCMI_3_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_PCMI_4_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_PCMI_5_global_register_block_registers,
    NULL,
    NULL,
    J2P_ECI_GLOBAL_MEM_OPTIONS_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_GENERAL_CFG_1_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_GENERAL_CFG_2_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_GENERAL_CFG_3_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_SYS_HEADER_CFG_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_GENERAL_IDB_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2P_ECI_FAP_GLOBAL_PP_1_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_2_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_3_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_4_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_5_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_6_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_7_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_8_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_9_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_10_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_11_global_register_block_registers,
    J2P_ECI_FAP_GLOBAL_PP_12_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

const soc_reg_t J2X_ECI_GTIMER_CYCLE_global_register_block_registers[] = {
    ARB_GTIMER_CYCLEr,
    BDM_GTIMER_CYCLEr,
    CDPM_GTIMER_CYCLEr,
    CFC_GTIMER_CYCLEr,
    CGM_GTIMER_CYCLEr,
    CRPS_GTIMER_CYCLEr,
    DDHA_GTIMER_CYCLEr,
    DDHB_GTIMER_CYCLEr,
    DDP_GTIMER_CYCLEr,
    DQM_GTIMER_CYCLEr,
    ECGM_GTIMER_CYCLEr,
    EDB_GTIMER_CYCLEr,
    EPNI_GTIMER_CYCLEr,
    EPRE_GTIMER_CYCLEr,
    EPS_GTIMER_CYCLEr,
    ERPP_GTIMER_CYCLEr,
    ESB_GTIMER_CYCLEr,
    ETPPA_GTIMER_CYCLEr,
    ETPPB_GTIMER_CYCLEr,
    ETPPC_GTIMER_CYCLEr,
    EVNT_GTIMER_CYCLEr,
    FAWR_GTIMER_CYCLEr,
    FAWT_GTIMER_CYCLEr,
    FBSW_GTIMER_CYCLEr,
    FCR_GTIMER_CYCLEr,
    FCT_GTIMER_CYCLEr,
    FDA_GTIMER_CYCLEr,
    FDCRL_GTIMER_CYCLEr,
    FDCRS_GTIMER_CYCLEr,
    FDMP_GTIMER_CYCLEr,
    FDMXA_GTIMER_CYCLEr,
    FDMXBI_GTIMER_CYCLEr,
    FDMXBJ_GTIMER_CYCLEr,
    FDMXC_GTIMER_CYCLEr,
    FDMXT_GTIMER_CYCLEr,
    FDMX_GTIMER_CYCLEr,
    FDR_GTIMER_CYCLEr,
    FDTL_GTIMER_CYCLEr,
    FDT_GTIMER_CYCLEr,
    FECPB_GTIMER_CYCLEr,
    FECRL_GTIMER_CYCLEr,
    FECRS_GTIMER_CYCLEr,
    FFLXO_GTIMER_CYCLEr,
    FFOA_GTIMER_CYCLEr,
    FFOB_GTIMER_CYCLEr,
    FGDMP_GTIMER_CYCLEr,
    FGMAP_GTIMER_CYCLEr,
    FICPB_GTIMER_CYCLEr,
    FLEXMAC_GTIMER_CYCLEr,
    FLFR_GTIMER_CYCLEr,
    FLFT_GTIMER_CYCLEr,
    FLOTN_GTIMER_CYCLEr,
    FMACR_GTIMER_CYCLEr,
    FMACT_GTIMER_CYCLEr,
    FMAC_GTIMER_CYCLEr,
    FMAP_GTIMER_CYCLEr,
    FMXAI_GTIMER_CYCLEr,
    FMXAJ_GTIMER_CYCLEr,
    FMXBI_GTIMER_CYCLEr,
    FMXBJ_GTIMER_CYCLEr,
    FMXBK_GTIMER_CYCLEr,
    FMXC_GTIMER_CYCLEr,
    FMXT_GTIMER_CYCLEr,
    FMX_GTIMER_CYCLEr,
    FOAM_GTIMER_CYCLEr,
    FODUO_GTIMER_CYCLEr,
    FOSW_GTIMER_CYCLEr,
    FPMR_GTIMER_CYCLEr,
    FPMT_GTIMER_CYCLEr,
    FPTPR_GTIMER_CYCLEr,
    FPTPT_GTIMER_CYCLEr,
    FQP_GTIMER_CYCLEr,
    FRA_GTIMER_CYCLEr,
    FRWA_GTIMER_CYCLEr,
    FRWBC_GTIMER_CYCLEr,
    FSAR_GTIMER_CYCLEr,
    FSRD_GTIMER_CYCLEr,
    HBC_GTIMER_CYCLEr,
    HBMC_GTIMER_CYCLEr,
    ILE_GTIMER_CYCLEr,
    ILU_GTIMER_CYCLEr,
    IPPA_GTIMER_CYCLEr,
    IPPB_GTIMER_CYCLEr,
    IPPC_GTIMER_CYCLEr,
    IPPD_GTIMER_CYCLEr,
    IPPE_GTIMER_CYCLEr,
    IPPF_GTIMER_CYCLEr,
    IPS_GTIMER_CYCLEr,
    IPT_GTIMER_CYCLEr,
    IQM_GTIMER_CYCLEr,
    IRE_GTIMER_CYCLEr,
    ITPPD_GTIMER_CYCLEr,
    ITPP_GTIMER_CYCLEr,
    KAPS_GTIMER_CYCLEr,
    MACT_GTIMER_CYCLEr,
    MCP_GTIMER_CYCLEr,
    MDB_GTIMER_CYCLEr,
    MESH_TOPOLOGY_GTIMER_CYCLEr,
    MRPS_GTIMER_CYCLEr,
    MSW_GTIMER_CYCLEr,
    MTM_GTIMER_CYCLEr,
    OAMP_GTIMER_CYCLEr,
    OCBM_GTIMER_CYCLEr,
    OCB_GTIMER_CYCLEr,
    OFR_GTIMER_CYCLEr,
    OFT_GTIMER_CYCLEr,
    OLP_GTIMER_CYCLEr,
    PDM_GTIMER_CYCLEr,
    PEM_GTIMER_CYCLEr,
    PQP_GTIMER_CYCLEr,
    RQP_GTIMER_CYCLEr,
    RTP_GTIMER_CYCLEr,
    SCH_GTIMER_CYCLEr,
    SIF_GTIMER_CYCLEr,
    SPB_GTIMER_CYCLEr,
    SQM_GTIMER_CYCLEr,
    TCAM_GTIMER_CYCLEr,
    TDU_GTIMER_CYCLEr,
    TSN_GTIMER_CYCLEr,
    W40_GTIMER_CYCLEr,
    WB_GTIMER_CYCLEr,
    -1
};

const soc_reg_t J2X_ECI_GTIMER_CONFIGURATION_global_register_block_registers[] = {
    ARB_GTIMER_CONFIGURATIONr,
    BDM_GTIMER_CONFIGURATIONr,
    CDPM_GTIMER_CONFIGURATIONr,
    CFC_GTIMER_CONFIGURATIONr,
    CGM_GTIMER_CONFIGURATIONr,
    CRPS_GTIMER_CONFIGURATIONr,
    DDHA_GTIMER_CONFIGURATIONr,
    DDHB_GTIMER_CONFIGURATIONr,
    DDP_GTIMER_CONFIGURATIONr,
    DQM_GTIMER_CONFIGURATIONr,
    ECGM_GTIMER_CONFIGURATIONr,
    EDB_GTIMER_CONFIGURATIONr,
    EPNI_GTIMER_CONFIGURATIONr,
    EPRE_GTIMER_CONFIGURATIONr,
    EPS_GTIMER_CONFIGURATIONr,
    ERPP_GTIMER_CONFIGURATIONr,
    ESB_GTIMER_CONFIGURATIONr,
    ETPPA_GTIMER_CONFIGURATIONr,
    ETPPB_GTIMER_CONFIGURATIONr,
    ETPPC_GTIMER_CONFIGURATIONr,
    EVNT_GTIMER_CONFIGURATIONr,
    FAWR_GTIMER_CONFIGURATIONr,
    FAWT_GTIMER_CONFIGURATIONr,
    FBSW_GTIMER_CONFIGURATIONr,
    FCR_GTIMER_CONFIGURATIONr,
    FCT_GTIMER_CONFIGURATIONr,
    FDA_GTIMER_CONFIGURATIONr,
    FDCRL_GTIMER_CONFIGURATIONr,
    FDCRS_GTIMER_CONFIGURATIONr,
    FDMP_GTIMER_CONFIGURATIONr,
    FDMXA_GTIMER_CONFIGURATIONr,
    FDMXBI_GTIMER_CONFIGURATIONr,
    FDMXBJ_GTIMER_CONFIGURATIONr,
    FDMXC_GTIMER_CONFIGURATIONr,
    FDMXT_GTIMER_CONFIGURATIONr,
    FDMX_GTIMER_CONFIGURATIONr,
    FDR_GTIMER_CONFIGURATIONr,
    FDTL_GTIMER_CONFIGURATIONr,
    FDT_GTIMER_CONFIGURATIONr,
    FECPB_GTIMER_CONFIGURATIONr,
    FECRL_GTIMER_CONFIGURATIONr,
    FECRS_GTIMER_CONFIGURATIONr,
    FFLXO_GTIMER_CONFIGURATIONr,
    FFOA_GTIMER_CONFIGURATIONr,
    FFOB_GTIMER_CONFIGURATIONr,
    FGDMP_GTIMER_CONFIGURATIONr,
    FGMAP_GTIMER_CONFIGURATIONr,
    FICPB_GTIMER_CONFIGURATIONr,
    FLEXMAC_GTIMER_CONFIGURATIONr,
    FLFR_GTIMER_CONFIGURATIONr,
    FLFT_GTIMER_CONFIGURATIONr,
    FLOTN_GTIMER_CONFIGURATIONr,
    FMACR_GTIMER_CONFIGURATIONr,
    FMACT_GTIMER_CONFIGURATIONr,
    FMAC_GTIMER_CONFIGURATIONr,
    FMAP_GTIMER_CONFIGURATIONr,
    FMXAI_GTIMER_CONFIGURATIONr,
    FMXAJ_GTIMER_CONFIGURATIONr,
    FMXBI_GTIMER_CONFIGURATIONr,
    FMXBJ_GTIMER_CONFIGURATIONr,
    FMXBK_GTIMER_CONFIGURATIONr,
    FMXC_GTIMER_CONFIGURATIONr,
    FMXT_GTIMER_CONFIGURATIONr,
    FMX_GTIMER_CONFIGURATIONr,
    FOAM_GTIMER_CONFIGURATIONr,
    FODUO_GTIMER_CONFIGURATIONr,
    FOSW_GTIMER_CONFIGURATIONr,
    FPMR_GTIMER_CONFIGURATIONr,
    FPMT_GTIMER_CONFIGURATIONr,
    FPTPR_GTIMER_CONFIGURATIONr,
    FPTPT_GTIMER_CONFIGURATIONr,
    FQP_GTIMER_CONFIGURATIONr,
    FRA_GTIMER_CONFIGURATIONr,
    FRWA_GTIMER_CONFIGURATIONr,
    FRWBC_GTIMER_CONFIGURATIONr,
    FSAR_GTIMER_CONFIGURATIONr,
    FSRD_GTIMER_CONFIGURATIONr,
    HBC_GTIMER_CONFIGURATIONr,
    HBMC_GTIMER_CONFIGURATIONr,
    ILE_GTIMER_CONFIGURATIONr,
    ILU_GTIMER_CONFIGURATIONr,
    IPPA_GTIMER_CONFIGURATIONr,
    IPPB_GTIMER_CONFIGURATIONr,
    IPPC_GTIMER_CONFIGURATIONr,
    IPPD_GTIMER_CONFIGURATIONr,
    IPPE_GTIMER_CONFIGURATIONr,
    IPPF_GTIMER_CONFIGURATIONr,
    IPS_GTIMER_CONFIGURATIONr,
    IPT_GTIMER_CONFIGURATIONr,
    IQM_GTIMER_CONFIGURATIONr,
    IRE_GTIMER_CONFIGURATIONr,
    ITPPD_GTIMER_CONFIGURATIONr,
    ITPP_GTIMER_CONFIGURATIONr,
    KAPS_GTIMER_CONFIGURATIONr,
    MACT_GTIMER_CONFIGURATIONr,
    MCP_GTIMER_CONFIGURATIONr,
    MDB_GTIMER_CONFIGURATIONr,
    MESH_TOPOLOGY_GTIMER_CONFIGURATIONr,
    MRPS_GTIMER_CONFIGURATIONr,
    MSW_GTIMER_CONFIGURATIONr,
    MTM_GTIMER_CONFIGURATIONr,
    OAMP_GTIMER_CONFIGURATIONr,
    OCBM_GTIMER_CONFIGURATIONr,
    OCB_GTIMER_CONFIGURATIONr,
    OFR_GTIMER_CONFIGURATIONr,
    OFT_GTIMER_CONFIGURATIONr,
    OLP_GTIMER_CONFIGURATIONr,
    PDM_GTIMER_CONFIGURATIONr,
    PEM_GTIMER_CONFIGURATIONr,
    PQP_GTIMER_CONFIGURATIONr,
    RQP_GTIMER_CONFIGURATIONr,
    RTP_GTIMER_CONFIGURATIONr,
    SCH_GTIMER_CONFIGURATIONr,
    SIF_GTIMER_CONFIGURATIONr,
    SPB_GTIMER_CONFIGURATIONr,
    SQM_GTIMER_CONFIGURATIONr,
    TCAM_GTIMER_CONFIGURATIONr,
    TDU_GTIMER_CONFIGURATIONr,
    TSN_GTIMER_CONFIGURATIONr,
    W40_GTIMER_CONFIGURATIONr,
    WB_GTIMER_CONFIGURATIONr,
    -1
};

const soc_reg_t J2X_ECI_SOFT_INIT_FOR_DEBUG_REG_global_register_block_registers[] = {
    ARB_REG_00C1r,
    BDM_REG_00C1r,
    CDPM_REG_00C1r,
    CFC_REG_00C1r,
    CGM_REG_00C1r,
    CRPS_REG_00C1r,
    DDHA_REG_00C1r,
    DDHB_REG_00C1r,
    DDP_REG_00C1r,
    DQM_REG_00C1r,
    ECGM_REG_00C1r,
    EDB_REG_00C1r,
    EPNI_REG_00C1r,
    EPRE_REG_00C1r,
    EPS_REG_00C1r,
    ERPP_REG_00C1r,
    ESB_REG_00C1r,
    ETPPA_REG_00C1r,
    ETPPB_REG_00C1r,
    ETPPC_REG_00C1r,
    EVNT_REG_00C1r,
    FAWR_REG_00C1r,
    FAWT_REG_00C1r,
    FBSW_REG_00C1r,
    FCR_REG_00C1r,
    FCT_REG_00C1r,
    FDA_REG_00C1r,
    FDCRL_REG_00C1r,
    FDCRS_REG_00C1r,
    FDMP_REG_00C1r,
    FDMXA_REG_00C1r,
    FDMXBI_REG_00C1r,
    FDMXBJ_REG_00C1r,
    FDMXC_REG_00C1r,
    FDMXT_REG_00C1r,
    FDMX_REG_00C1r,
    FDR_REG_00C1r,
    FDTL_REG_00C1r,
    FDT_REG_00C1r,
    FECPB_REG_00C1r,
    FECRL_REG_00C1r,
    FECRS_REG_00C1r,
    FFLXO_REG_00C1r,
    FFOA_REG_00C1r,
    FFOB_REG_00C1r,
    FGDMP_REG_00C1r,
    FGMAP_REG_00C1r,
    FICPB_REG_00C1r,
    FLEXMAC_REG_00C1r,
    FLFR_REG_00C1r,
    FLFT_REG_00C1r,
    FLOTN_REG_00C1r,
    FMACR_REG_00C1r,
    FMACT_REG_00C1r,
    FMAC_REG_00C1r,
    FMAP_REG_00C1r,
    FMXAI_REG_00C1r,
    FMXAJ_REG_00C1r,
    FMXBI_REG_00C1r,
    FMXBJ_REG_00C1r,
    FMXBK_REG_00C1r,
    FMXC_REG_00C1r,
    FMXT_REG_00C1r,
    FMX_REG_00C1r,
    FOAM_REG_00C1r,
    FODUO_REG_00C1r,
    FOSW_REG_00C1r,
    FPMR_REG_00C1r,
    FPMT_REG_00C1r,
    FPTPR_REG_00C1r,
    FPTPT_REG_00C1r,
    FQP_REG_00C1r,
    FRA_REG_00C1r,
    FRWA_REG_00C1r,
    FRWBC_REG_00C1r,
    FSAR_REG_00C1r,
    FSRD_REG_00C1r,
    HBC_REG_00C1r,
    HBMC_REG_00C1r,
    ILE_REG_00C1r,
    ILU_REG_00C1r,
    IPPA_REG_00C1r,
    IPPB_REG_00C1r,
    IPPC_REG_00C1r,
    IPPD_REG_00C1r,
    IPPE_REG_00C1r,
    IPPF_REG_00C1r,
    IPS_REG_00C1r,
    IPT_REG_00C1r,
    IQM_REG_00C1r,
    IRE_REG_00C1r,
    ITPPD_REG_00C1r,
    ITPP_REG_00C1r,
    KAPS_REG_00C1r,
    MACT_REG_00C1r,
    MCP_REG_00C1r,
    MDB_REG_00C1r,
    MRPS_REG_00C1r,
    MSW_REG_00C1r,
    MTM_REG_00C1r,
    OAMP_REG_00C1r,
    OCBM_REG_00C1r,
    OCB_REG_00C1r,
    OFR_REG_00C1r,
    OFT_REG_00C1r,
    OLP_REG_00C1r,
    PDM_REG_00C1r,
    PEM_REG_00C1r,
    PQP_REG_00C1r,
    RQP_REG_00C1r,
    RTP_REG_00C1r,
    SCH_REG_00C1r,
    SIF_REG_00C1r,
    SPB_REG_00C1r,
    SQM_REG_00C1r,
    TCAM_REG_00C1r,
    TDU_REG_00C1r,
    TSN_REG_00C1r,
    W40_REG_00C1r,
    WB_REG_00C1r,
    -1
};

const soc_reg_t J2X_ECI_GLOBAL_MEM_OPTIONS_global_register_block_registers[] = {
    ARB_REG_00C2r,
    BDM_REG_00C2r,
    CDPM_REG_00C2r,
    CFC_REG_00C2r,
    CGM_REG_00C2r,
    CRPS_REG_00C2r,
    DDHA_REG_00C2r,
    DDHB_REG_00C2r,
    DDP_REG_00C2r,
    DQM_REG_00C2r,
    ECGM_REG_00C2r,
    EDB_REG_00C2r,
    EPNI_REG_00C2r,
    EPRE_REG_00C2r,
    EPS_REG_00C2r,
    ERPP_REG_00C2r,
    ESB_REG_00C2r,
    ETPPA_REG_00C2r,
    ETPPB_REG_00C2r,
    ETPPC_REG_00C2r,
    EVNT_REG_00C2r,
    FAWR_REG_00C2r,
    FAWT_REG_00C2r,
    FBSW_REG_00C2r,
    FCR_REG_00C2r,
    FCT_REG_00C2r,
    FDA_REG_00C2r,
    FDCRL_REG_00C2r,
    FDCRS_REG_00C2r,
    FDMP_REG_00C2r,
    FDMXA_REG_00C2r,
    FDMXBI_REG_00C2r,
    FDMXBJ_REG_00C2r,
    FDMXC_REG_00C2r,
    FDMXT_REG_00C2r,
    FDMX_REG_00C2r,
    FDR_REG_00C2r,
    FDTL_REG_00C2r,
    FDT_REG_00C2r,
    FECPB_REG_00C2r,
    FECRL_REG_00C2r,
    FECRS_REG_00C2r,
    FFLXO_REG_00C2r,
    FFOA_REG_00C2r,
    FFOB_REG_00C2r,
    FGDMP_REG_00C2r,
    FGMAP_REG_00C2r,
    FICPB_REG_00C2r,
    FLEXMAC_REG_00C2r,
    FLFR_REG_00C2r,
    FLFT_REG_00C2r,
    FLOTN_REG_00C2r,
    FMACR_REG_00C2r,
    FMACT_REG_00C2r,
    FMAC_REG_00C2r,
    FMAP_REG_00C2r,
    FMXAI_REG_00C2r,
    FMXAJ_REG_00C2r,
    FMXBI_REG_00C2r,
    FMXBJ_REG_00C2r,
    FMXBK_REG_00C2r,
    FMXC_REG_00C2r,
    FMXT_REG_00C2r,
    FMX_REG_00C2r,
    FOAM_REG_00C2r,
    FODUO_REG_00C2r,
    FOSW_REG_00C2r,
    FPMR_REG_00C2r,
    FPMT_REG_00C2r,
    FPTPR_REG_00C2r,
    FPTPT_REG_00C2r,
    FQP_REG_00C2r,
    FRA_REG_00C2r,
    FRWA_REG_00C2r,
    FRWBC_REG_00C2r,
    FSAR_REG_00C2r,
    FSRD_REG_00C2r,
    HBC_REG_00C2r,
    ILE_REG_00C2r,
    ILU_REG_00C2r,
    IPPA_REG_00C2r,
    IPPB_REG_00C2r,
    IPPC_REG_00C2r,
    IPPD_REG_00C2r,
    IPPE_REG_00C2r,
    IPPF_REG_00C2r,
    IPS_REG_00C2r,
    IPT_REG_00C2r,
    IQM_REG_00C2r,
    IRE_REG_00C2r,
    ITPPD_REG_00C2r,
    ITPP_REG_00C2r,
    KAPS_REG_00C2r,
    MACT_REG_00C2r,
    MCP_REG_00C2r,
    MDB_REG_00C2r,
    MRPS_REG_00C2r,
    MSW_REG_00C2r,
    MTM_REG_00C2r,
    OAMP_REG_00C2r,
    OCBM_REG_00C2r,
    OCB_REG_00C2r,
    OFR_REG_00C2r,
    OFT_REG_00C2r,
    OLP_REG_00C2r,
    PDM_REG_00C2r,
    PEM_REG_00C2r,
    PQP_REG_00C2r,
    RQP_REG_00C2r,
    RTP_REG_00C2r,
    SCH_REG_00C2r,
    SIF_REG_00C2r,
    SPB_REG_00C2r,
    SQM_REG_00C2r,
    TCAM_REG_00C2r,
    TDU_REG_00C2r,
    TSN_REG_00C2r,
    W40_REG_00C2r,
    WB_REG_00C2r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_GENERAL_CFG_1_global_register_block_registers[] = {
    DDP_REG_00C3r,
    FCT_REG_00C3r,
    FDR_REG_00C3r,
    FDTL_REG_00C3r,
    FDT_REG_00C3r,
    FMAC_REG_00C3r,
    IPS_REG_00C3r,
    IPT_REG_00C3r,
    OLP_REG_00C3r,
    RQP_REG_00C3r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_GENERAL_CFG_2_global_register_block_registers[] = {
    CDPM_REG_00C4r,
    DDP_REG_00C4r,
    FCR_REG_00C4r,
    FCT_REG_00C4r,
    FDA_REG_00C4r,
    FDR_REG_00C4r,
    FDTL_REG_00C4r,
    FDT_REG_00C4r,
    IPS_REG_00C4r,
    IPT_REG_00C4r,
    IRE_REG_00C4r,
    MESH_TOPOLOGY_REG_00C4r,
    MSW_REG_00C4r,
    OFT_REG_00C4r,
    RTP_REG_00C4r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_GENERAL_CFG_3_global_register_block_registers[] = {
    CGM_REG_00C5r,
    DDP_REG_00C5r,
    DQM_REG_00C5r,
    ECGM_REG_00C5r,
    EPNI_REG_00C5r,
    EPRE_REG_00C5r,
    ERPP_REG_00C5r,
    ETPPA_REG_00C5r,
    ETPPB_REG_00C5r,
    ETPPC_REG_00C5r,
    EVNT_REG_00C5r,
    FQP_REG_00C5r,
    IPPB_REG_00C5r,
    IPPD_REG_00C5r,
    IPT_REG_00C5r,
    IRE_REG_00C5r,
    PQP_REG_00C5r,
    RQP_REG_00C5r,
    SPB_REG_00C5r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_SYS_HEADER_CFG_global_register_block_registers[] = {
    DDP_REG_00C6r,
    ERPP_REG_00C6r,
    ETPPA_REG_00C6r,
    ETPPB_REG_00C6r,
    ETPPC_REG_00C6r,
    IPPD_REG_00C6r,
    IPT_REG_00C6r,
    IRE_REG_00C6r,
    ITPPD_REG_00C6r,
    ITPP_REG_00C6r,
    OAMP_REG_00C6r,
    RQP_REG_00C6r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_GENERAL_IDB_global_register_block_registers[] = {
    CGM_REG_00C7r,
    DDP_REG_00C7r,
    DQM_REG_00C7r,
    SQM_REG_00C7r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_1_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_2_global_register_block_registers[] = {
    ERPP_REG_00E8r,
    ETPPA_REG_00E8r,
    ETPPB_REG_00E8r,
    ETPPC_REG_00E8r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_3_global_register_block_registers[] = {
    ERPP_REG_00E9r,
    ETPPA_REG_00E9r,
    ETPPB_REG_00E9r,
    ETPPC_REG_00E9r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_4_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_5_global_register_block_registers[] = {
    ERPP_REG_00EBr,
    ETPPA_REG_00EBr,
    ETPPB_REG_00EBr,
    ETPPC_REG_00EBr,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_6_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_7_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_8_global_register_block_registers[] = {
    ERPP_REG_00EEr,
    ETPPA_REG_00EEr,
    ETPPB_REG_00EEr,
    ETPPC_REG_00EEr,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_9_global_register_block_registers[] = {
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_10_global_register_block_registers[] = {
    ERPP_REG_00F0r,
    ETPPA_REG_00F0r,
    ETPPB_REG_00F0r,
    ETPPC_REG_00F0r,
    IPPA_REG_00F0r,
    IPPB_REG_00F0r,
    IPPC_REG_00F0r,
    IPPD_REG_00F0r,
    IPPF_REG_00F0r,
    MDB_REG_00F0r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_11_global_register_block_registers[] = {
    ERPP_REG_00F1r,
    ETPPA_REG_00F1r,
    ETPPB_REG_00F1r,
    ETPPC_REG_00F1r,
    -1
};

const soc_reg_t J2X_ECI_FAP_GLOBAL_PP_12_global_register_block_registers[] = {
    ERPP_REG_00F2r,
    ETPPA_REG_00F2r,
    ETPPB_REG_00F2r,
    ETPPC_REG_00F2r,
    -1
};

const soc_reg_t *j2x_common_regs_addr_2_global_register_block_registers[257] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2X_ECI_GTIMER_CYCLE_global_register_block_registers,
    J2X_ECI_GTIMER_CONFIGURATION_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2X_ECI_SOFT_INIT_FOR_DEBUG_REG_global_register_block_registers,
    J2X_ECI_GLOBAL_MEM_OPTIONS_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_GENERAL_CFG_1_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_GENERAL_CFG_2_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_GENERAL_CFG_3_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_SYS_HEADER_CFG_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_GENERAL_IDB_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    J2X_ECI_FAP_GLOBAL_PP_1_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_2_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_3_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_4_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_5_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_6_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_7_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_8_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_9_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_10_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_11_global_register_block_registers,
    J2X_ECI_FAP_GLOBAL_PP_12_global_register_block_registers,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

shr_error_e
soc_dnx_ledup_init(
    int unit)
{
    uint32 reg_val;
    int freq;
    uint32 clk_half_period, refresh_period;
    uint32 last_port = 0;
    SHR_FUNC_INIT_VARS(unit);

    freq = dnx_data_device.general.core_clock_khz_get(unit);
    last_port = dnx_data_nif.global.last_port_led_scan_get(
    unit);

    refresh_period = (freq * 33);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &reg_val, REFRESH_CYCLE_PERIODf, refresh_period);

    SHR_IF_ERR_EXIT(WRITE_U0_LED_REFRESH_CTRLr(unit, reg_val));

    freq = 1000 * 1000000;
    clk_half_period = (freq + 39 * 1000000) / (20000000 * 2);
    reg_val = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &reg_val, LEDCLK_HALF_PERIODf, clk_half_period);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_CLK_DIV_CTRLr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_U0_LED_ACCU_CTRLr(unit, &reg_val));
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &reg_val, LAST_PORTf, last_port);

    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &reg_val, LED_ACCU_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_U0_LED_ACCU_CTRLr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_iproc_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(WRITE_PAXB_0_FUNC0_IMAP1_7r(unit, 0xe8000001));

    if (SOC_IS_J2X(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_PAXB_0_FUNC0_IMAP1_6r(unit, 0xe9000001));
    }

#ifdef PLISIM
    if (!SAL_BOOT_PLISIM)
#endif
    {
        uint32 reg_val = 0;
        SHR_IF_ERR_EXIT(READ_PAXB_0_FUNC0_IMAP1_7r(unit, &reg_val));
        if (reg_val != 0xe8000001)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "1st BAR not working properly\n");
        }

        SHR_IF_ERR_EXIT(READ_ICFG_CHIP_ID_REGr(unit, &reg_val));
        if ((reg_val & 0xf000) != 0x8000)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "2nd BAR not working properly\n");
        }
    }

    SHR_IF_ERR_EXIT(soc_dnxc_perform_system_reset_if_needed(unit));
    {
        SHR_IF_ERR_EXIT(soc_dnx_ledup_init(unit));
        SHR_IF_ERR_EXIT(dnxc_block_instance_validity_override(unit));
    }

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_iproc_14_tx_arbiter_priority))
    {
        SHR_IF_ERR_EXIT(soc_cmicx_paxb_tx_arbiter_set(unit, PAXB_TX_REQ_SEQ_EN, PAXB_TX_ARB_PRIORITY));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

int
dnx_access_init_handle_error_skipping(
    int unit,
    int error_value)
{
    if (SHR_FAILURE(error_value))
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, " Error '%s' indicated ;\r\n"), shrextend_errmsg_get(error_value)));
        if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_stop_on_access_init_error))
            return error_value;
    }
    return _SHR_E_NONE;
}

shr_error_e
soc_dnx_ring_config(
    int unit)
{
    uint32 reg_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x34422227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x55566333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x55555555));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000066));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x22222000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000655));
    }
    else if (SOC_IS_J2C(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x30222227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x00440333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000002));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x33322000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x23333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x02222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000004));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000222));
    }
    else if (SOC_IS_Q2A(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x33000007));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x44443333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00555444));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x20000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00022222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x20022000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00000000));
    }
    else if (SOC_IS_J2P(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x00000017));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x22220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00000022));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00440000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000005));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x32220000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x00000033));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x22200000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x23233332));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x33333333));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x23200003));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00400323));
    }
    else if (SOC_IS_J2X(unit))
    {

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x00222227));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x22222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x00004402));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x00033330));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x22200000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x02222222));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x00000000));
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x00400222));
    }

    SHR_IF_ERR_EXIT(soc_dnxc_cmic_sbus_timeout_set(unit, dnx_data_device.general.core_clock_khz_get(unit),
                                                   SOC_CONTROL(unit)->schanTimeout));

    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH0_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH1_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH2_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH3_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH4_ERRr(unit, 0));
    if (SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
    {
        SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH5_ERRr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH6_ERRr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH7_ERRr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH8_ERRr(unit, 0));
        SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH9_ERRr(unit, 0));
    }

    if (!SAL_BOOT_PLISIM)
    {

        SHR_IF_ERR_EXIT_WITH_LOG(READ_ECI_VERSION_REGISTERr(unit, &reg_val), "SBUS access is not working.%s%s%s\n",
                                 EMPTY, EMPTY, EMPTY);
    }

    if (!SOC_WARM_BOOT(unit) &&
        dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable) == 2)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_eci_reset(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_soft_reset(
    int unit)
{
    uint32 reg_val;
    int disable_hard_reset = 0x0;
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));

    reg_val = 0;
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, MBU_RESETf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, TIME_SYNC_RESETf, 1);
    {
        soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 1);
    }
    soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IPROC_TS_RESETf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));

    if (disable_hard_reset != 0)
    {

        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, 0));

        if (SOC_IS_J2P(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, 0x7f));
        }
        else if (SOC_IS_J2X(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, 0xf));
        }
        else if (SOC_IS_JERICHO2(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, 0x1f));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, 0x7));
        }
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, reg_val));
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_EPINTF_RELEASE_ALL_CREDITSr(unit, 1));

        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 0);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, &reg_val));
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, WR_IP_INTF_CREDITSf, 1);
        soc_reg_field_set(unit, CMIC_TOP_IPINTF_INTERFACE_CREDITSr, &reg_val, IP_INTF_CREDITSf, 0);
        SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_IPINTF_INTERFACE_CREDITSr(unit, reg_val));

        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 0);
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 1);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
        SHR_IF_ERR_EXIT(READ_ECI_ECIC_MISC_RESETr(unit, &reg_val));
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 0);
        SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    }

    sal_usleep(5000);

    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 1));
    }
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    if (SOC_REG_IS_VALID(unit, ECI_TOP_LEVEL_SAMPLING_CFGr) &&
        !dnx_data_device.general.feature_get(unit, dnx_data_device_general_disable_gated_top_level_sampling))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_TOP_LEVEL_SAMPLING_CFGr(unit, 0));
    }
    reg_val = 0;
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_DEC_RSTNf, 1);
    soc_reg_field_set(unit, ECI_ECIC_MISC_RESETr, &reg_val, DDR_ENC_RSTNf, 1);
    {
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_ENABLEf, 1);
        soc_reg_set_field_if_exists(unit, ECI_ECIC_MISC_RESETr, &reg_val, IP_RXI_RESETf, 0);
    }
    SHR_IF_ERR_EXIT(WRITE_ECI_ECIC_MISC_RESETr(unit, reg_val));
    sal_usleep(40000);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64_val));
    sal_usleep(40000);

    SHR_IF_ERR_EXIT(soc_reg32_get(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, &reg_val));
    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 0);
    }
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, reg_val));

    sal_usleep(200);

    {
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, PIR_RSTNf, 1);
    }
    SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, reg_val));

    {
        if (SOC_IS_J2C(unit) || SOC_IS_Q2A(unit) || SOC_IS_J2X(unit))
        {
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_RXI_RESET_Nf, 1);
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_ENABLEf, 1);
        }
        else
        {
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_RESET_Nf, 1);
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_ENABLEf, 1);
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_RESET_Nf, 1);
            soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_ENABLEf, 1);
        }
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INIT_VALUEf, 0);
        soc_reg_field_set(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, TXI_CREDITS_INITf, 0);
        soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_1_RXI_ENABLEf, 1);
        soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_0_RXI_ENABLEf, 1);
        soc_reg_set_field_if_exists(unit, ECI_PIR_CONFIGURATIONSr, &reg_val, EPNI_RXI_ENABLEf, 1);
    }

    SHR_IF_ERR_EXIT(soc_reg32_set(unit, ECI_PIR_CONFIGURATIONSr, REG_PORT_ANY, 0, reg_val));
    sal_usleep(200);

    if (SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
    {
        SHR_IF_ERR_EXIT(WRITE_ECI_GLOBAL_REGISTER_ADDRESS_MAPPINGr(unit, reg_above_64_val));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_pll_configuration(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_unmask_mem_writes(
    int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 i;

    CONST STATIC soc_reg_t indirect_wr_mask_reg[] = {
        DDHA_INDIRECT_WR_MASKr,
        DDHB_INDIRECT_WR_MASKr,
        DHC_INDIRECT_WR_MASKr,
        ERPP_INDIRECT_WR_MASKr,
        IPPA_INDIRECT_WR_MASKr,
        IPPB_INDIRECT_WR_MASKr,
        IPPC_INDIRECT_WR_MASKr,
        IPPD_INDIRECT_WR_MASKr,
        IPPE_INDIRECT_WR_MASKr,
        IPPF_INDIRECT_WR_MASKr,
        KAPS_INDIRECT_WR_MASKr,
        MDB_INDIRECT_WR_MASKr,
        SCH_INDIRECT_WR_MASKr,
        TCAM_INDIRECT_WR_MASKr
    };

    CONST STATIC soc_reg_t indirect_force_bubble_reg[] = {
        CDUM_INDIRECT_FORCE_BUBBLEr,
        CDU_INDIRECT_FORCE_BUBBLEr,
        CFC_INDIRECT_FORCE_BUBBLEr,
        CGM_INDIRECT_FORCE_BUBBLEr,
        CLU_INDIRECT_FORCE_BUBBLEr,
        CDB_INDIRECT_FORCE_BUBBLEr,
        CDBM_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED
        DCC_INDIRECT_FORCE_BUBBLEr,
#endif
        CRPS_INDIRECT_FORCE_BUBBLEr,
        DDHA_INDIRECT_FORCE_BUBBLEr,
        DDHB_INDIRECT_FORCE_BUBBLEr,
        DDP_INDIRECT_FORCE_BUBBLEr,
        DHC_INDIRECT_FORCE_BUBBLEr,
        DPC_INDIRECT_FORCE_BUBBLEr,
        DQM_INDIRECT_FORCE_BUBBLEr,
        ECGM_INDIRECT_FORCE_BUBBLEr,
        ECI_INDIRECT_FORCE_BUBBLEr,
        EDB_INDIRECT_FORCE_BUBBLEr,
        EPNI_INDIRECT_FORCE_BUBBLEr,
        EPS_INDIRECT_FORCE_BUBBLEr,
        ERPP_INDIRECT_FORCE_BUBBLEr,
        ETPPA_INDIRECT_FORCE_BUBBLEr,
        ETPPB_INDIRECT_FORCE_BUBBLEr,
        ETPPC_INDIRECT_FORCE_BUBBLEr,
        EVNT_INDIRECT_FORCE_BUBBLEr,
        FCR_INDIRECT_FORCE_BUBBLEr,
        FCT_INDIRECT_FORCE_BUBBLEr,
        FDR_INDIRECT_FORCE_BUBBLEr,
        FDT_INDIRECT_FORCE_BUBBLEr,
        FSRD_INDIRECT_FORCE_BUBBLEr,
        FQP_INDIRECT_FORCE_BUBBLEr,
        HBMC_INDIRECT_FORCE_BUBBLEr,
        ILE_INDIRECT_FORCE_BUBBLEr,
        ILU_INDIRECT_FORCE_BUBBLEr,
        IPPA_INDIRECT_FORCE_BUBBLEr,
        IPPB_INDIRECT_FORCE_BUBBLEr,
        IPPC_INDIRECT_FORCE_BUBBLEr,
        IPPD_INDIRECT_FORCE_BUBBLEr,
        IPPE_INDIRECT_FORCE_BUBBLEr,
        IPPF_INDIRECT_FORCE_BUBBLEr,
        IPS_INDIRECT_FORCE_BUBBLEr,
        IPT_INDIRECT_FORCE_BUBBLEr,
        IQM_INDIRECT_FORCE_BUBBLEr,
        IRE_INDIRECT_FORCE_BUBBLEr,
        ITPPD_INDIRECT_FORCE_BUBBLEr,
        ITPP_INDIRECT_FORCE_BUBBLEr,
        KAPS_INDIRECT_FORCE_BUBBLEr,
        MACT_INDIRECT_FORCE_BUBBLEr,
        MCP_INDIRECT_FORCE_BUBBLEr,
        MDB_INDIRECT_FORCE_BUBBLEr,
        MRPS_INDIRECT_FORCE_BUBBLEr,
        MTM_INDIRECT_FORCE_BUBBLEr,
        OAMP_INDIRECT_FORCE_BUBBLEr,
        OCB_INDIRECT_FORCE_BUBBLEr,
        OCBM_INDIRECT_FORCE_BUBBLEr,
        OLP_INDIRECT_FORCE_BUBBLEr,
        PDM_INDIRECT_FORCE_BUBBLEr,
        PEM_INDIRECT_FORCE_BUBBLEr,
        PQP_INDIRECT_FORCE_BUBBLEr,
        RQP_INDIRECT_FORCE_BUBBLEr,
        RTP_INDIRECT_FORCE_BUBBLEr,
        SCH_INDIRECT_FORCE_BUBBLEr,
        SPB_INDIRECT_FORCE_BUBBLEr,
        SQM_INDIRECT_FORCE_BUBBLEr,
        TCAM_INDIRECT_FORCE_BUBBLEr,

        ESB_INDIRECT_FORCE_BUBBLEr,
#ifdef BLOCK_NOT_INITIALIZED
        FASIC_INDIRECT_FORCE_BUBBLEr,
        FLEXEWP_INDIRECT_FORCE_BUBBLEr,
        FSAR_INDIRECT_FORCE_BUBBLEr,
        FSCL_INDIRECT_FORCE_BUBBLEr,
#endif
        FEU_INDIRECT_FORCE_BUBBLEr
    };

    SHR_FUNC_INIT_VARS(unit);
    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    for (i = 0; i < sizeof(indirect_wr_mask_reg) / sizeof(indirect_wr_mask_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_wr_mask_reg[i]))
        {
            SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                            (unit,
                             soc_reg_above_64_set_all_instances(unit, indirect_wr_mask_reg[i], 0, reg_above_64_val)));
        }
    }

    for (i = 0; i < sizeof(indirect_force_bubble_reg) / sizeof(indirect_force_bubble_reg[0]); i++)
    {
        if (SOC_REG_IS_VALID(unit, indirect_force_bubble_reg[i]))
        {
            SHR_IF_ERR_EXIT(dnx_access_init_handle_error_skipping
                            (unit, soc_reg32_set_all_instances(unit, indirect_force_bubble_reg[i], 0, 0x020019)));
        }
    }

    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_scheduler_virtual_flows_soft_init(
    int unit)
{
    int bundle_flow_id;
    int core_idx;
    int actual_flow_id, i;
    sch_virtual_flows_sw_state_type_e is_enabled;
    int min_bundle_size = dnx_data_sch.flow.min_connector_bundle_size_get(unit);
    int nof_sch_flows = dnx_data_sch.flow.nof_flows_get(unit);
    uint32 data[20];
    CONST SHR_BITDCL *is_virtual_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_db.virtual_flows.is_enabled.get(unit, &is_enabled));
    if (is_enabled == SCH_VIRTUAL_FLOWS_ENABLED)
    {
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_idx)
        {
            sal_memset(data, 0, sizeof(data));
            soc_mem_field32_set(unit, SCH_FORCE_STATUS_MESSAGEm, data, MESSAGE_TYPEf,
                                DBAL_ENUM_FVAL_FLOW_SLOW_STATUS_NORMAL);
            SHR_IF_ERR_EXIT(dnx_scheduler_db.virtual_flows.is_flow_virtual.get(unit, core_idx, &is_virtual_bitmap));
            SHR_BIT_ITER(is_virtual_bitmap, nof_sch_flows / min_bundle_size, bundle_flow_id)
            {
                actual_flow_id = bundle_flow_id * min_bundle_size;
                for (i = 0; i < min_bundle_size; i++)
                {
                    soc_mem_field32_set(unit, SCH_FORCE_STATUS_MESSAGEm, data, MESSAGE_FLOW_IDf, actual_flow_id + i);
                    soc_mem_write(unit, SCH_FORCE_STATUS_MESSAGEm, SCH_BLOCK(unit, core_idx), 0, data);
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_scheduler_low_rate_update_token_count_soft_init(
    int unit)
{
    soc_mem_t mem;
    int rate_factor;
    int core;
    int flow_range_start, flow_range_end;
    int *index_start = NULL, *index_end = NULL;
    int num_elements;
    int array_index_start, array_index_end, array_i;
    uint32 *enabled = NULL;
    uint32 entry_data;
    uint32 mem_addr;
    uint8 access_type = 0;
    uint32 data[20];
    int is_update_needed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    mem = SCH_TOKEN_MEMORY_CONTROLLER_TMCm;
    num_elements = dnx_drv_soc_mem_numels(unit, mem);
    array_index_start = dnx_drv_soc_mem_first_array_index(unit, mem);
    array_index_end = (num_elements - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
    SHR_ALLOC_ERR_EXIT(index_start, dnx_data_device.general.nof_cores_get(unit) * sizeof(int), "index_start",
                       "%s%s%s\r\n", "allocation failed", EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(index_end, dnx_data_device.general.nof_cores_get(unit) * sizeof(int), "index_end", "%s%s%s\r\n",
                       "allocation failed", EMPTY, EMPTY);
    SHR_ALLOC_ERR_EXIT(enabled, dnx_data_device.general.nof_cores_get(unit) * sizeof(uint32), "enabled", "%s%s%s\r\n",
                       "allocation failed", EMPTY, EMPTY);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_sch_dbal_low_rate_range_get
                        (unit, core, &flow_range_start, &flow_range_end, &enabled[core]));
        if (enabled[core] == TRUE)
        {
            index_start[core] = flow_range_start / num_elements;
            index_end[core] = (flow_range_end - 1) / num_elements;
            is_update_needed = TRUE;
        }
    }

    if (is_update_needed == TRUE)
    {

        SHR_IF_ERR_EXIT(dnx_scheduler_low_rate_factor_get(unit, &rate_factor));
        entry_data = rate_factor * dnx_data_sch.dbal.token_count_default_get(unit);

        soc_mem_read(unit, mem, SCH_BLOCK(unit, 0), 0, data);
        soc_mem_field32_set(unit, mem, data, TOKEN_COUNTf, entry_data);

        for (array_i = array_index_start; array_i <= array_index_end; array_i++)
        {
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {

                if (enabled[core] == TRUE)
                {
                    mem_addr =
                        soc_mem_addr_get(unit, mem, array_i, SCH_BLOCK(unit, core), index_start[core], &access_type);
                    SHR_IF_ERR_EXIT(sand_mem_indirect_reset_write
                                    (unit, mem, SCH_BLOCK(unit, core), mem_addr,
                                     index_end[core] - index_start[core], data));
                }
            }

            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
            {
                if (enabled[core] == TRUE)
                {
                    SHR_IF_ERR_EXIT(sand_mem_indirect_poll_done(unit, SCH_BLOCK(unit, core), 10000));
                }
            }
        }
    }

exit:
    SHR_FREE(index_start);
    SHR_FREE(index_end);
    SHR_FREE(enabled);
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx2_soft_init_sch(
    int unit)
{

    uint32 error_check_en;
    uint32 entry[SOC_MAX_MEM_WORDS] = { 0 };
    uint32 mem_data;
    uint32 all_ones[SOC_MAX_MEM_WORDS];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(entry, 0x0, sizeof(entry));
    sal_memset(all_ones, 0xff, sizeof(all_ones));

    error_check_en = SOC_FEATURE_GET(unit, soc_feature_schan_err_check);
    SOC_FEATURE_CLEAR(unit, soc_feature_schan_err_check);
    if (SOC_IS_JERICHO2(unit))
    {
        mem_data = 0;
    }
    else
    {
        mem_data = 1;
    }
    SHR_IF_ERR_EXIT(soc_mem_write(unit, SCH_SCHEDULER_INITm, MEM_BLOCK_ALL, 0, &mem_data));

    if (soc_sand_is_emulation_system(unit))
    {
        sal_sleep(10);
    }
    else
    {
        sal_usleep(1000);
    }

    if (error_check_en)
    {
        SOC_FEATURE_SET(unit, soc_feature_schan_err_check);
    }

    SHR_IF_ERR_EXIT(dnx_sch_reserve_se_update(unit));

    if (dnxc_init_is_init_done_get(unit) == TRUE)
    {
        if (dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_low_rate_connectors_token_count_update))
        {

            SHR_IF_ERR_EXIT(dnx_scheduler_low_rate_update_token_count_soft_init(unit));
        }

        SHR_IF_ERR_EXIT(dnx_scheduler_virtual_flows_soft_init(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx2_soft_init(
    int unit,
    int without_fabric,
    int without_ile)
{
    uint32 reg_val, field_val;
    uint16 dev_id;
    uint8 rev_id;

    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t field_above_64_val = { 0 };
    soc_reg_above_64_val_t epni_field_above_64_val = { 0 };
    int array_index, array_index_min = 0, array_index_max = 0;
    soc_reg_above_64_val_t zero_field_val = { 0 };
    uint32 reset_cdu_eth[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    bcm_pbmp_t cdu_eth_pbmp[DNX_DATA_MAX_NIF_ILKN_PMS_NOF];
    int saved_rx_enable[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint64 saved_mac_ctrl[DNX_DATA_MAX_NIF_ILKN_PMS_NOF][8];
    uint32 rx_fifo_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 tx_port_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 fc_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };
    uint32 aligner_reset_store[DNX_DATA_MAX_NIF_ILKN_PMS_NOF] = { 0 };

    uint32 instru_reset_store[DNX_DATA_MAX_NIF_ETH_CDU_NOF] = { 0 };
    int is_port_enable;
    bcm_port_t ilkn_port, ilkn_core;
    bcm_pbmp_t all_ilkn_ports, all_ilkn_cores;
    dnx_algo_port_ilkn_access_info_t ilkn_access_info;
    SHR_FUNC_INIT_VARS(unit);

    array_index_min = SOC_REG_FIRST_ARRAY_INDEX(unit, SCH_DVS_NIF_CONFIGr);
    array_index_max = SOC_REG_NUMELS(unit, SCH_DVS_NIF_CONFIGr) + array_index_min;

    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 0xffffffff;
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    field_above_64_val[0] = 1;
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    if (SOC_IS_J2C(unit) && without_ile == 0 && dnxc_init_is_init_done_get(unit))
    {
        _SHR_PBMP_CLEAR(all_ilkn_cores);
        _SHR_PBMP_CLEAR(all_ilkn_ports);

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, 0, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN,
                                                   DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &all_ilkn_ports));

        _SHR_PBMP_ITER(all_ilkn_ports, ilkn_port)
        {
            SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, ilkn_port, 0, &is_port_enable));
            if (is_port_enable)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_ilkn_access_info_get(unit, ilkn_port, &ilkn_access_info));
                _SHR_PBMP_PORT_ADD(all_ilkn_cores, ilkn_access_info.ilkn_core);
            }
            else
            {
                _SHR_PBMP_PORT_REMOVE(all_ilkn_ports, ilkn_port);
            }
        }

        _SHR_PBMP_ITER(all_ilkn_cores, ilkn_core)
        {
            SHR_IF_ERR_EXIT(imb_ile_fec_global_enable_set(unit, ilkn_core, 0));
        }

        _SHR_PBMP_ITER(all_ilkn_ports, ilkn_port)
        {
            SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, ilkn_port, IMB_PORT_ENABLE_F_SKIP_FEC, 0));
        }
    }

    if (SOC_IS_Q2A_A0(unit))
    {
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0x0));
        }
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }

    SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
    if (SOC_IS_J2P(unit))
    {

        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KTM_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KTM_1f, 0);
    }

    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit) || SOC_IS_J2X(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCHf, 0);
    }
    if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_SCH_1f, 0);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHAf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPSf, 0);
        if (SOC_IS_Q2A(unit))
        {

            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FLEXEWPf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSCLf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSARf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FASICf, 0);
        }
    }
    else if (SOC_IS_JERICHO2(unit))
    {
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MDBf, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHA_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DDHB_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_KAPS_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_DHC_7f, 0);
    }

    if (SOC_IS_JERICHO2(unit) || SOC_IS_J2C(unit) || SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
    {

        if (without_fabric != 0)
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCRf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDAf, 0);
            if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_0f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDR_1f, 0);
            }
            else
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDRf, 0);
            }
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTf, 0);
            if (SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_0f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTL_1f,
                                             0);
            }
            else
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FDTLf, 0);
            }
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FCTf, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_MESH_TOPOLOGYf,
                                         0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_RTPf, 0);
        }

        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_5f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_6f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_7f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_8f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_9f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_10f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_11f, 0);
        if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_12f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_13f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_14f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_15f, 0);
            if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_16f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_17f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_18f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_19f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_20f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_21f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_22f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_23f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_24f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_25f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_26f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FMAC_27f,
                                             0);
                if (SOC_IS_J2P(unit))
                {
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_28f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_29f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_30f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_31f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_32f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_33f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_34f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_35f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_36f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_37f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_38f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_39f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_40f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_41f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_42f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_43f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_44f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_45f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_46f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FMAC_47f, 0);
                }
            }
        }
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_0f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_1f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_2f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_3f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_4f, 0);
        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_5f, 0);
        if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit) || SOC_IS_J2X(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_6f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_7f, 0);
            if (SOC_IS_JERICHO2(unit) || SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_8f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_9f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_10f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_11f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_12f,
                                             0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FSRD_13f,
                                             0);
                if (SOC_IS_J2P(unit))
                {
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_14f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_15f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_16f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_17f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_18f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_19f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_20f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_21f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_22f, 0);
                    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                 BLOCKS_SOFT_INIT_FSRD_23f, 0);
                }
            }
        }
    }

    if (without_ile != 0)
    {
        if (SOC_IS_J2X(unit))
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILEf, 0);

        }
        else
        {
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, 0);
            soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, 0);
            if (SOC_IS_J2C(unit) || SOC_IS_J2P(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, 0);
            }
            if (SOC_IS_J2C(unit))
            {
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, 0);
                soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, 0);
            }
        }

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
        {
            bcm_pbmp_t ilkn_ports_bitmap;
            bcm_port_t port = 0, active_port;
            dnx_algo_port_info_s active_port_info;
            int pm_idx, phy_id, is_active = 0;
            int nof_pm_cdu, is_over_fabric = 0;
            bcm_pbmp_t nif_phys;
            bcm_pbmp_t pm_pmbp, pm_pmbp_temp;
            soc_field_t cdu_reset_field[] = { BLOCKS_SOFT_INIT_CDU_0f, BLOCKS_SOFT_INIT_CDU_1f, BLOCKS_SOFT_INIT_CDU_2f,
                BLOCKS_SOFT_INIT_CDU_3f
            };

            nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
            for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
            {
                BCM_PBMP_CLEAR(cdu_eth_pbmp[pm_idx]);
            }
            BCM_PBMP_CLEAR(ilkn_ports_bitmap);
            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &ilkn_ports_bitmap));
            BCM_PBMP_ITER(ilkn_ports_bitmap, port)
            {

                SHR_IF_ERR_EXIT(dnx_algo_port_nif_ilkn_is_over_fabric_get(unit, port, &is_over_fabric));
                if (is_over_fabric)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
                for (pm_idx = 0; pm_idx < nof_pm_cdu; pm_idx++)
                {

                    pm_pmbp = dnx_data_nif.eth.pm_properties_get(unit, pm_idx)->phys;
                    BCM_PBMP_ASSIGN(pm_pmbp_temp, pm_pmbp);
                    BCM_PBMP_AND(pm_pmbp_temp, nif_phys);
                    if (BCM_PBMP_NOT_NULL(pm_pmbp_temp))
                    {
                        soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val,
                                                     cdu_reset_field[pm_idx], 0);

                        BCM_PBMP_REMOVE(pm_pmbp, pm_pmbp_temp);
                        BCM_PBMP_ITER(pm_pmbp, phy_id)
                        {
                            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_active_get(unit, phy_id, &is_active));
                            if (is_active)
                            {
                                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get
                                                (unit, phy_id, FALSE, 0, &active_port));
                                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, active_port, &active_port_info));
                                if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, active_port_info,
                                                                  DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)))
                                {
                                    reset_cdu_eth[pm_idx] = 1;
                                    BCM_PBMP_PORT_ADD(cdu_eth_pbmp[pm_idx], active_port);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(2000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);

        sal_memset(saved_rx_enable, 0x0, sizeof(saved_rx_enable));
        sal_memset(saved_mac_ctrl, 0x0, sizeof(saved_mac_ctrl));

        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, &rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, &tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_FC_RESETr, cdu_idx, 0, &fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, 0x1));

            SHR_IF_ERR_EXIT(soc_reg32_get(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, &aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, 0x0));

            SHR_IF_ERR_EXIT(soc_reg32_get
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, &instru_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, 0x0));

            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeIn,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_REG_IS_VALID(unit, EPNI_INIT_EPNI_NIF_TXIr))
    {

        int field_size = 0;
        field_size = soc_reg_field_length(unit, EPNI_INIT_EPNI_NIF_TXIr, INIT_EPNI_NIF_TXIf);

        SOC_REG_ABOVE_64_CREATE_MASK(epni_field_above_64_val, field_size, 0);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));

        sal_usleep(1000);

        SHR_IF_ERR_EXIT(READ_ESB_FPC_0_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_0_CONFIGr, &reg_val, FPC_0_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_0_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_1_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_1_CONFIGr, &reg_val, FPC_1_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_1_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_2_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_2_CONFIGr, &reg_val, FPC_2_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_2_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SHR_IF_ERR_EXIT(READ_ESB_FPC_3_CONFIGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_FPC_3_CONFIGr, &reg_val, FPC_3_INITf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_FPC_3_CONFIGr, REG_PORT_ANY, 0, reg_val));

        SOC_REG_ABOVE_64_CLEAR(epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        soc_reg_above_64_field_set(unit, EPNI_INIT_EPNI_NIF_TXIr, reg_above_64_val, INIT_EPNI_NIF_TXIf,
                                   epni_field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_INIT_EPNI_NIF_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        sal_usleep(5000);
    }

    if (SOC_IS_JERICHO2(unit))
    {

        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, FQP_INIT_FQP_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
        SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
        field_above_64_val[0] = field_above_64_val[1] = 0xffffffff;
        soc_reg_above_64_field_set(unit, FQP_INIT_FQP_TXIr, reg_above_64_val, INIT_FQP_TXIf, field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FQP_INIT_FQP_TXIr, REG_PORT_ANY, 0, reg_above_64_val));

        SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
        soc_reg_above_64_field_set(unit, FQP_INIT_FQP_TXIr, reg_above_64_val, INIT_FQP_TXIf, field_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FQP_INIT_FQP_TXIr, REG_PORT_ANY, 0, reg_above_64_val));
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {
        SOC_REG_ABOVE_64_CREATE_MASK(zero_field_val, 0, 0);

        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_FQPf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_EPNIf,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ESBf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    }
    sal_usleep(1000);
    if (SOC_REG_IS_VALID(unit, ESB_ESB_INITr))
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 0));
        sal_usleep(5000);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_INITr, REG_PORT_ANY, 0, 1));
        sal_usleep(5000);
    }
    if (SOC_IS_Q2A(unit) || SOC_IS_J2C(unit))
    {

        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_NMGf,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }
/* *INDENT-OFF* */
    

    if (without_ile != 0)
    {
        int block, array_iter;
        soc_reg_t reg;
        uint32 nof_block = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

        reg = ILE_ILKN_TX_PORT_FORCE_DATA_PCK_FLUSHr;
        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
        
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        for (block = 0; block < nof_block; block++)
        {
            for (array_iter = 0; array_iter < SOC_REG_NUMELS(unit, reg); array_iter++)
            {
                SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, block, array_iter, reg_above_64_val));
            }
        }
    }

    if (SOC_IS_Q2A(unit))
    {
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f,
                                   zero_field_val);
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f,
                                   zero_field_val);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);
    }
    else if (SOC_IS_J2C(unit))
    {
        
        SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CDU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_CLU_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_6f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILE_7f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_0f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_1f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_2f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_3f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_4f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_5f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_6f, zero_field_val); 
        soc_reg_above_64_field_set(unit, ECI_BLOCKS_SOFT_INITr, reg_above_64_val, BLOCKS_SOFT_INIT_ILU_7f, zero_field_val); 
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
        sal_usleep(1000);

        if (without_ile == 0 && dnxc_init_is_init_done_get(unit))
        {
            _SHR_PBMP_ITER(all_ilkn_ports, ilkn_port)
            {
                SHR_IF_ERR_EXIT(imb_ile_port_enable_set(unit, ilkn_port, IMB_PORT_ENABLE_F_SKIP_FEC, 1));
            }

            _SHR_PBMP_ITER(all_ilkn_cores, ilkn_core)
            {
                SHR_IF_ERR_EXIT(imb_ile_fec_global_enable_set(unit, ilkn_core, 1));
            }
        }
    }

    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64_val));
    sal_usleep(5000);

    if (without_ile != 0 && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_fec_units_in_cdu))
    {
        int cdu_idx, array_idx;
        bcm_port_t active_eth_port;
        int nof_pm_cdu = dnx_data_nif.eth.cdu_nof_get(unit);
        
        for (cdu_idx = 0; cdu_idx < nof_pm_cdu; cdu_idx++)
        {
            if (reset_cdu_eth[cdu_idx] == 0)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_RX_FIFOS_SRSTNr, cdu_idx, 0, rx_fifo_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, cdu_idx, 0, tx_port_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_FC_RESETr, cdu_idx, 0, fc_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_ALIGNER_MEMS_RESETr, cdu_idx, 0, aligner_reset_store[cdu_idx]));
            SHR_IF_ERR_EXIT(soc_reg32_set
                            (unit, CDU_INSTRUMENTATION_RESET_LOWr, cdu_idx, 0, instru_reset_store[cdu_idx]));
            
            array_idx = 0;
            BCM_PBMP_ITER(cdu_eth_pbmp[cdu_idx], active_eth_port)
            {
                SHR_IF_ERR_EXIT(portmod_port_soft_reset(unit, active_eth_port, portmodMacSoftResetModeOut,
                                                        &saved_rx_enable[cdu_idx][array_idx],
                                                        &saved_mac_ctrl[cdu_idx][array_idx]));
                array_idx++;
            }
        }
    }

    if (SOC_IS_Q2A_A0(unit))
    {
        
        int block_instance;
        int number_blk_instances = 2;
        for (block_instance = 0; block_instance < number_blk_instances; block_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, CDU_TX_PORTS_SRSTNr, block_instance, 0, 0xff));
        }

        SOC_REG_ABOVE_64_ALLONES(reg_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, FEU_TX_PORTS_SRSTNr, 0, 0, reg_above_64_val));
    }
    if (SOC_REG_IS_VALID(unit, ESB_ESB_CALENDER_SETTINGr))
    {
        
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(READ_ESB_ESB_CALENDER_SETTINGr(unit, SOC_CORE_ALL, &reg_val));
        soc_reg_field_set(unit, ESB_ESB_CALENDER_SETTINGr, &reg_val, ESB_CALENDAR_SWITCH_ENf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, ESB_ESB_CALENDER_SETTINGr, REG_PORT_ANY, 0, reg_val));
        sal_usleep(1000);
    }
    
    {
        SHR_IF_ERR_EXIT(soc_dnx2_soft_init_sch(unit));
    }

    if (!SAL_BOOT_PLISIM)
    { 
        uint32 prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat = 0, init_not_done = 1, i;

        for (i = 2; i && init_not_done;)
        {
            SHR_IF_ERR_EXIT(READ_RQP_RQP_BLOCK_INIT_STATUSr(unit, 0, &prp_init));
            SHR_IF_ERR_EXIT(READ_PQP_EGQ_BLOCK_INIT_STATUSr(unit, 0, &eqm_init));       
            SHR_IF_ERR_EXIT(READ_FQP_FQP_BLOCK_INIT_STATUSr(unit, 0, &user_counter_init));
            SHR_IF_ERR_EXIT(READ_EPNI_EPNI_INIT_STATUSr(unit, 0, &aligner_init));
            if (SOC_REG_IS_VALID(unit, ESB_FPC_0_STATr))
            {
                SHR_IF_ERR_EXIT(READ_ESB_FPC_0_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat = soc_reg_field_get(unit, ESB_FPC_0_STATr, init_not_done, FPC_0_INIT_STATf);
                SHR_IF_ERR_EXIT(READ_ESB_FPC_1_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_1_STATr, init_not_done, FPC_1_INIT_STATf) << 1;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_2_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_2_STATr, init_not_done, FPC_2_INIT_STATf) << 2;
                SHR_IF_ERR_EXIT(READ_ESB_FPC_3_STATr(unit, SOC_CORE_ALL, &init_not_done));
                fpc_init_stat |= soc_reg_field_get(unit, ESB_FPC_3_STATr, init_not_done, FPC_3_INIT_STATf) << 3;
            }
            
            eqm_init = eqm_init & 0x7;

            init_not_done = prp_init | eqm_init | user_counter_init | aligner_init | fpc_init_stat;
            if (init_not_done != 0 && --i != 0)
            {
                sal_usleep(5000);
                
                LOG_ERROR(BSL_LS_SOC_INIT, ("Warning: waiting for EGQ blocks to be ready\n"));
            }

        }
        if (init_not_done != 0)
        { 
            LOG_ERROR(BSL_LS_SOC_INIT,
                      (BSL_META_U
                       (unit,
                        "EGQ blocks did not become ready  prp_init=%u eqm_init=%u user_counter_init=%u aligner_init=%u fpc_init_stat=0x%x\n"),
                       prp_init, eqm_init, user_counter_init, aligner_init, fpc_init_stat));
        }
    }

    
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_NIF_CONFIGr, reg_above_64_val, NIF_FORCE_PAUSE_Nf, field_above_64_val);
    for (array_index = array_index_min; array_index < array_index_max; array_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_NIF_CONFIGr, REG_PORT_ANY, array_index, reg_above_64_val));
    }
    SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));
    SOC_REG_ABOVE_64_CLEAR(field_above_64_val);
    soc_reg_above_64_field_set(unit, SCH_DVS_CONFIGr, reg_above_64_val, FORCE_PAUSEf, field_above_64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, SCH_DVS_CONFIGr, REG_PORT_ANY, 0, reg_above_64_val));

    
    soc_cm_get_id(unit, &dev_id, &rev_id);
    SHR_IF_ERR_EXIT(READ_ECI_VERSION_REGISTERr(unit, &reg_val));
    field_val = soc_reg_field_get(unit, ECI_VERSION_REGISTERr, reg_val, CHIP_TYPEf);
    if ((field_val & 0xffff) != dev_id)
    {
        LOG_ERROR_EX(BSL_LOG_MODULE, "Chip version is wrong: unit %d expected 0x%x and found 0x%x\n%s", unit,
                     dev_id, field_val, EMPTY);
    }
    SHR_IF_ERR_EXIT(soc_dnxc_verify_device_init(unit));
    SHR_IF_ERR_EXIT(READ_SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0, &reg_val));
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, IRE_STATIC_CONFIGURATIONr, 0, 0, &reg_val));

#ifndef PLISIM
    if (!SAL_BOOT_PLISIM)
    {
        
        SHR_IF_ERR_EXIT(dnx_dram_buffers_quarantine_restore_deleted_buffers_after_soft_init(unit));
    }
#endif

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_dnx_soft_init(
    int unit,
    int without_fabric,
    int without_ile)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX2(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx2_soft_init(unit, without_fabric, without_ile));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnx_perform_bist_tests(
    int unit)
{
    dnx_startup_test_function_f test_function = dnx_startup_test_functions[unit];
    utilex_seq_step_t *utilex_list = NULL;
    uint32 mbist_soc_prop_enable = dnx_data_device.general.bist_enable_get(unit);
    int need_system_reset_after_mbist_fail = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {

        
        if (SOC_IS_Q2A(unit)
            && (dnx_drv_soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "cpu2tap_enable", 1) != 0
                && (dnx_drv_soc_property_get(unit, spn_EXT_RAM_ENABLED_BITMAP, 0) != 0)))
        {
            SHR_IF_ERR_EXIT(soc_q2a_cpu2tap_init_mems(unit));
        }
        
        if (test_function != NULL)
        {
            test_function(unit);
        }
        
        if (((any_bist_performed[unit / 32] & (((uint32) 1) << (unit % 32))) ||
             mbist_soc_prop_enable) && !SAL_BOOT_PLISIM)
        {
            int step_found, i, mbist_arg = (mbist_soc_prop_enable != 1);

            int deinit_steps[] = { DNX_INIT_STEP_DMA, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT, DNX_INIT_STEP_PLL };
            int init_steps[] =
                { DNX_INIT_STEP_IPROC, DNX_INIT_STEP_HARD_RESET, DNX_INIT_STEP_SBUS, DNX_INIT_STEP_INTERRUPT,
                DNX_INIT_STEP_SOFT_RESET,
                DNX_INIT_STEP_DMA, DNX_INIT_STEP_PLL,
                DNX_INIT_STEP_DRAM_INITIAL_CONFIG
            };
            need_system_reset_after_mbist_fail = 1;
            do
            {
                if (SOC_IS_J2P(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2p(unit, mbist_arg));
                }
                else if (SOC_IS_J2C(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_j2c(unit, mbist_arg));
                }
                else if (SOC_IS_Q2A(unit))
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_q2a(unit, mbist_arg));
                }
                else
                {
                    SHR_IF_ERR_EXIT(soc_bist_all_jr2(unit, mbist_arg));
                }
            }
            while (mbist_soc_prop_enable == 9999);   
            need_system_reset_after_mbist_fail = 0;
            
            test_was_run_and_suceeded[unit / 32] |= ((uint32) 1) << (unit % 32);

            if (SOC_IS_DNX2(unit))
            {
                SHR_IF_ERR_EXIT(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &utilex_list));
            }

            if (SOC_IS_DNX2(unit))
            {
                
                for (i = 0; i < sizeof(deinit_steps) / sizeof(deinit_steps[0]); i++)
                {
                    SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, deinit_steps[i], 0, &step_found));
                    if (step_found != 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                    }
                }

                
                for (i = 0; i < sizeof(init_steps) / sizeof(init_steps[0]); i++)
                {
                    SHR_IF_ERR_EXIT(utilex_seq_run_step_by_id(unit, utilex_list, init_steps[i], 1, &step_found));
                    if (step_found != 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid step \n");
                    }
                }
            }
        }

        
        if (TRUE == dnx_data_dram.hbm.feature_get(unit, dnx_data_dram_hbm_is_supported))
        {
            int hbm_id = dnx_data_dram.general_info.max_nof_drams_get(unit);
            for (--hbm_id; hbm_id >= 0; --hbm_id)
            {
                if ((dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap & (1 << hbm_id)))
                {
                    SHR_IF_ERR_EXIT(soc_set_hbm_wrst_n_to_0(unit, hbm_id));
                }
            }
        }
    }
exit:
    if (utilex_list != NULL)
    {
        dnx_init_step_list_destory(unit, utilex_list);
    }
    if (need_system_reset_after_mbist_fail)
    {                                         
        SHR_IF_ERR_CONT(soc_dnxc_perform_system_reset_if_needed(unit));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_hard_reset(
    int unit)
{
    int disable_hard_reset = 0x0;
    uint32 reg_val;

    SHR_FUNC_INIT_VARS(unit);

    disable_hard_reset = dnx_data_device.general.feature_get(unit, dnx_data_device_general_hard_reset_disable);

    if (disable_hard_reset != 1)
    {
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {
            SHR_IF_ERR_EXIT(access_dnxc_device_hard_reset(unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, 4));
        }

        if (SOC_IS_JERICHO2(unit))
        {       
            SHR_IF_ERR_EXIT(READ_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, &reg_val));
            soc_reg_field_set(unit, AXIIC_EXT_S0_FN_MOD_BM_ISSr, &reg_val, WRITE_ISS_OVERRIDEf, 1);
            SHR_IF_ERR_EXIT(WRITE_AXIIC_EXT_S0_FN_MOD_BM_ISSr(unit, reg_val));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_init(
    int unit)
{
    int cmc_index = 0;
    soc_reg_above_64_val_t reg_above_64_val;

    SHR_FUNC_INIT_VARS(unit);

    

    
    SOC_CMCS_NUM(unit) = dnx_data_dev_init.cmc.cmc_num_get(unit);
    
    NUM_CPU_COSQ(unit) = dnx_data_dev_init.cmc.num_cpu_cosq_get(unit);
    
    SOC_PCI_CMC(unit) = dnx_data_dev_init.cmc.cmc_pci_get(unit);
    
    SOC_PCI_CMCS_NUM(unit) = dnx_data_dev_init.cmc.pci_cmc_num_get(unit);
    
    SOC_ARM_CMC(unit, 0) = dnx_data_dev_init.cmc.cmc_uc0_get(unit);
    
    SOC_ARM_CMC(unit, 1) = dnx_data_dev_init.cmc.cmc_uc1_get(unit);
    
    NUM_CPU_ARM_COSQ(unit, 0) = dnx_data_dev_init.cmc.num_queues_pci_get(unit);
    
    NUM_CPU_ARM_COSQ(unit, 1) =
        dnx_data_dev_init.cmc.num_queues_uc0_get(unit) + dnx_data_dev_init.cmc.num_queues_uc1_get(unit);
    if ((NUM_CPU_ARM_COSQ(unit, 0) + NUM_CPU_ARM_COSQ(unit, 1)) > NUM_CPU_COSQ(unit))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(_SHR_E_PARAM,
                                 "Number of cosq reserved for CMC 0/1 exceed. num_queues_pci %u num_queues_uc0 %d\n%s",
                                 NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1), EMPTY);
    }

    
    for (cmc_index = 0; cmc_index < SOC_CMCS_NUM(unit); cmc_index++)
    {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc_index), 0, NUM_CPU_COSQ(unit));
    }

    
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 0), 0, NUM_CPU_ARM_COSQ(unit, 0));
    SHR_BITSET_RANGE(CPU_ARM_QUEUE_BITMAP(unit, 1), NUM_CPU_ARM_COSQ(unit, 0), NUM_CPU_ARM_COSQ(unit, 1));

    
    soc_dcb_unit_init(unit);

    if (SOC_WARM_BOOT(unit))
    {
        
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, FALSE));
    }
    else
    {
        SHR_IF_ERR_EXIT(soc_dma_attach(unit, TRUE));
    }

#ifdef BCM_SBUSDMA_SUPPORT
    SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
    SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
    SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
    SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;
    
    SOC_MEM_CLEAR_CHUNK_SIZE_SET(unit, dnx_data_intr.general.mem_clear_chunk_size_get(unit));
#endif

    

    if (soc_feature(unit, soc_feature_sbusdma))
    {
        
        SHR_IF_ERR_EXIT(soc_sbusdma_lock_init(unit));   
        
        if (SOC_IS_NEW_ACCESS_INITIALIZED(unit)
            || (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))
                || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))))
        {
            
            SHR_IF_ERR_EXIT(soc_sbusdma_init(unit, dnx_data_intr.general.sbus_dma_interval_get(unit),
                                             dnx_data_intr.general.sbus_dma_intr_enable_get(unit)));
            SHR_IF_ERR_EXIT(sand_init_fill_table(unit));
        }
    }
    
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_init(unit));

    
    SHR_IF_ERR_EXIT(soc_cmicx_uc_msg_init(unit));

    
    if (soc_feature(unit, soc_feature_cmicx_v4))
    {
        SHR_IF_ERR_EXIT(soc_cmicx_icfg_sw_m0ssq_init(unit));
    }

    
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_init(unit));

    if (!SOC_WARM_BOOT(unit))
    {
        
#ifdef BCM_ACCESS_SUPPORT
        if (ACCESS_IS_INITIALIZED(unit))
        {
            access_runtime_info_t *runtime_info = access_runtime_info_get(unit);
             
            SHR_IF_ERR_EXIT(access_regmem(unit, 0, rECI_RESERVED_22, 0, 0, 0, reg_above_64_val));
            
            {
                SHR_IF_ERR_EXIT(access_field32_set(runtime_info, rECI_RESERVED_22_fFIELD_30_33, reg_above_64_val, 0));
            }
            
            SHR_IF_ERR_EXIT(access_regmem
                            (unit, FLAG_ACCESS_IS_WRITE, rECI_RESERVED_22, ACCESS_ALL_BLOCK_INSTANCES, 0, 0,
                             reg_above_64_val));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
            {
                soc_reg_above_64_field32_set(unit, ECI_RESERVED_22r, reg_above_64_val, FIELD_30_33f, 0);
            }
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, ECI_RESERVED_22r, REG_PORT_ANY, 0, reg_above_64_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_dma_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (soc_feature(unit, soc_feature_sbusdma))
    {
        
        if (SOC_IS_NEW_ACCESS_INITIALIZED(unit)
            || (soc_mem_dmaable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))
                || soc_mem_slamable(unit, CGM_DESTINATION_TABLEm, SOC_MEM_BLOCK_ANY(unit, CGM_DESTINATION_TABLEm))))

        {
            SHR_IF_ERR_EXIT(sand_deinit_fill_table(unit));
        }
        (void) soc_sbusdma_lock_deinit(unit);
    }

    
    SHR_IF_ERR_EXIT(soc_dnx_fifodma_deinit(unit));

    SHR_IF_ERR_EXIT(soc_dma_detach(unit));

    SHR_IF_ERR_EXIT(soc_sbusdma_desc_detach(unit));

    
    SHR_IF_ERR_EXIT(soc_sand_indirect_mem_access_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_mark_not_inited(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_FLAGS_SET(unit, SOC_FLAGS_GET(unit) & ~SOC_F_INITED);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
soc_dnx_rcpu_port_init(
    int unit,
    int port_i)
{
    uint32 rval = 0;

    SHR_FUNC_INIT_VARS(unit);
    {
        if (port_i < 32)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_0r(unit, &rval));
            rval |= 0x1 << port_i;
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_0r(unit, rval));
        }
        else if (port_i < 64)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_1r(unit, &rval));
            rval |= 0x1 << (port_i - 32);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_1r(unit, rval));
        }
        else if (port_i < 96)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_2r(unit, &rval));
            rval |= 0x1 << (port_i - 64);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_2r(unit, rval));
        }
        else if (port_i < 128)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_3r(unit, &rval));
            rval |= 0x1 << (port_i - 96);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_3r(unit, rval));
        }
        else if (port_i < 160)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_4r(unit, &rval));
            rval |= 0x1 << (port_i - 128);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_4r(unit, rval));
        }
        else if (port_i < 192)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_5r(unit, &rval));
            rval |= 0x1 << (port_i - 160);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_5r(unit, rval));
        }
        else if (port_i < 224)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_6r(unit, &rval));
            rval |= 0x1 << (port_i - 192);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_6r(unit, rval));
        }
        else if (port_i < 256)
        {
            SHR_IF_ERR_EXIT(READ_CMIC_RPE_PKT_PORTS_7r(unit, &rval));
            rval |= 0x1 << (port_i - 224);
            SHR_IF_ERR_EXIT(WRITE_CMIC_RPE_PKT_PORTS_7r(unit, rval));
        }
        else
        {
            LOG_ERROR(BSL_LS_SOC_INIT, (BSL_META_U(unit, "Error: RCPU port range is 0 - 255\n")));
            SHR_IF_ERR_EXIT(_SHR_E_INTERNAL);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_rcpu_init(
    int unit)
{
    int port_i = 0;
    const dnx_data_access_rcpu_rx_t *rcpu_rx;

    SHR_FUNC_INIT_VARS(unit);

    rcpu_rx = dnx_data_access.rcpu.rx_get(unit);

    SOC_PBMP_ITER(rcpu_rx->pbmp, port_i)
    {
        SHR_IF_ERR_EXIT(soc_dnx_rcpu_port_init(unit, port_i));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnx_block_enable_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (SOC_IS_Q2A(unit))
    {
        
#ifndef PLISIM
        if (!(SAL_BOOT_PLISIM))
        {
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fasic_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).flexewp_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fsar_block] = 0;
            SOC_INFO(unit).block_valid[SOC_INFO(unit).fscl_block] = 0;
        }
#endif
    }

        
#ifndef PLISIM
    if (!(SAL_BOOT_PLISIM))
    {
        int inst, nof_cdmac;
        nof_cdmac = dnx_data_nif.eth.cdu_mac_nof_get(unit) * dnx_data_nif.eth.cdu_nof_get(unit);

        for (inst = 0; inst < nof_cdmac; inst++)
        {
            SOC_INFO(unit).block_valid[CDMAC_BLOCK(unit, inst)] = 0;
        }
    }
#endif

    SHR_FUNC_EXIT;
}


int
dnx_tbl_is_dynamic(
    int unit,
    soc_mem_t mem)
{
    SOC_MEM_ALIAS_TO_ORIG(unit, mem);

    return soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem);
}
