/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: tomahawk3_cos.c
 *
 * Purpose:
 *    Maintains all the debug information for cos feature for tomahawk3
 *
 * Note: This file is auto-generated. Manual edits
 *       to it will be lost when it is auto-regenerated.
 */

#include <appl/diag/techsupport.h>
#include <soc/mcm/allenum.h>

extern char * techsupport_cos_diag_cmdlist[];
extern char * techsupport_cos_sw_dump_cmdlist[];

/* "cos" feature's diag command list valid only for tomahawk3 */
char * techsupport_cos_tomahawk3_diag_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};

#ifndef BCM_SW_STATE_DUMP_DISABLE
/* "cos" feature's software dump command list valid only for tomahawk3 */
char * techsupport_cos_tomahawk3_sw_dump_cmdlist[] = {
    NULL /* Must be the last element in this structure */
};
#endif /* BCM_SW_STATE_DUMP_DISABLE */


/* Structure that maintains memory list for
 * "cos" feature for tomahawk3 chipset. */
static soc_mem_t techsupport_cos_tomahawk3_memory_table_list[] = {
    COS_MAP_SELm,
    CPU_COS_MAP_DATA_ONLYm,
    CPU_COS_MAP_ONLYm,
    CPU_COS_MAPm,
    IFP_COS_MAPm,
    PORT_COS_MAPm,
    INVALIDm /* Must be the last element in this structure */
};

/* Structure that maintains register list for
 * "cos" feature for tomahawk3 chipset. */
techsupport_reg_t techsupport_cos_tomahawk3_reg_list[] = {
    { CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC0_SHARED_PROGRAMMABLE_COS_MASK0r, register_type_global },
    { CMIC_CMC0_SHARED_PROGRAMMABLE_COS_MASK1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_0r, register_type_global },
    { CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_1r, register_type_global },
    { CMIC_CMC1_SHARED_PROGRAMMABLE_COS_MASK0r, register_type_global },
    { CMIC_CMC1_SHARED_PROGRAMMABLE_COS_MASK1r, register_type_global },
    { CMIC_RPE_PIO_MEMDMA_COS_0r, register_type_global },
    { CMIC_RPE_PIO_MEMDMA_COS_1r, register_type_global },
    { CMIC_RPE_PKTDMA_COS_0r, register_type_global },
    { CMIC_RPE_PKTDMA_COS_1r, register_type_global },
    { CMIC_RPE_PKT_COS_QUEUES_HIr, register_type_global },
    { CMIC_RPE_PKT_COS_QUEUES_LOr, register_type_global },
    { CMIC_RPE_SCHAN_SBUSDMA_COS_0r, register_type_global },
    { CMIC_RPE_SCHAN_SBUSDMA_COS_1r, register_type_global },
    { CMIC_RPE_SHARED_PROGRAMMABLE_COS_MASK0r, register_type_global },
    { CMIC_RPE_SHARED_PROGRAMMABLE_COS_MASK1r, register_type_global },
    { CMIC_TOP_TO_MMU_COS_MASK_LSB_BITSr, register_type_global },
    { CMIC_TOP_TO_MMU_COS_MASK_MSB_BITSr, register_type_global },
    { CPU_COS_CAM_SERCTRLr, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE0r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE1r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE2r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE3r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE4r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE5r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE6r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64_PIPE7r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_CONFIG_64r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE0r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE1r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE2r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE3r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE4r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE5r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE6r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUS_PIPE7r, register_type_global },
    { CPU_COS_MAP_CAM_BIST_STATUSr, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE0r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE1r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE2r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE3r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE4r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE5r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE6r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROL_PIPE7r, register_type_global },
    { CPU_COS_MAP_CAM_CONTROLr, register_type_global },
    { EGR_SFLOW_CPU_COS_CONFIGr, register_type_global },
    { ING_MIRROR_COS_CONTROLr, register_type_global },
    { INSTRUMENT_CPU_COS_CONFIGr, register_type_global },
    { MIRROR_CPU_COS_CONFIGr, register_type_global },
    { MMU_MTP_COSr, register_type_global },
    { MMU_MTP_CPU_COSr, register_type_global },
    { MMU_QSCH_DEBUG_FORCE_CPU_COSMASKr, register_type_global },
    { INVALIDr, register_type_global } /* Must be the last element in this structure */
};

/* Structure that maintains diag cmdlist, reg_list, mem_list  for
 * "cos" feature for tomahawk3 chipset. */
techsupport_data_t techsupport_cos_tomahawk3_data = {
    techsupport_cos_diag_cmdlist,
    techsupport_cos_tomahawk3_reg_list,
    techsupport_cos_tomahawk3_memory_table_list,
    techsupport_cos_tomahawk3_diag_cmdlist
#ifndef BCM_SW_STATE_DUMP_DISABLE
    , techsupport_cos_sw_dump_cmdlist,
    techsupport_cos_tomahawk3_sw_dump_cmdlist
#endif /* BCM_SW_STATE_DUMP_DISABLE */
};
