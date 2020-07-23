/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
#ifndef chip_address_defines__H
#define chip_address_defines__H

/*-----------------------------------------------*/
/*   Defines for IEEE_PMA_PMD_BLK0 Bank                    */
/*-----------------------------------------------*/
#define IEEE_PMA_PMD_BLK0_BaseAdr                                                              (0x0000)
#define IEEE_PMA_PMD_BLK0_pmd_control_register_Adr                                             (0x0000)
#define IEEE_PMA_PMD_BLK0_status_register_Adr                                                  (0x0001)
#define IEEE_PMA_PMD_BLK0_pmd_identifier_register_0_Adr                                        (0x0002)
#define IEEE_PMA_PMD_BLK0_pmd_identifier_register_1_Adr                                        (0x0003)
#define IEEE_PMA_PMD_BLK0_pmd_speed_ability_Adr                                                (0x0004)
#define IEEE_PMA_PMD_BLK0_devices_in_package_1_Adr                                             (0x0005)
#define IEEE_PMA_PMD_BLK0_devices_in_package_2_Adr                                             (0x0006)
#define IEEE_PMA_PMD_BLK0_pmd_control_2_register_Adr                                           (0x0007)
#define IEEE_PMA_PMD_BLK0_status_register_2_Adr                                                (0x0008)
#define IEEE_PMA_PMD_BLK0_pmd_transmit_disable_register_Adr                                    (0x0009)
#define IEEE_PMA_PMD_BLK0_pmd_receive_signal_detect_register_Adr                               (0x000a)
#define IEEE_PMA_PMD_BLK0_pmd_extended_ability_register_Adr                                    (0x000b)
#define IEEE_PMA_PMD_BLK0_fortyg_pmd_extended_ability_register_Adr                             (0x000d)
#define IEEE_PMA_PMD_BLK0_pmd_oui_id0_register_Adr                                             (0x000e)
#define IEEE_PMA_PMD_BLK0_pmd_oui_id1_reg_Adr                                                  (0x000f)


/*-----------------------------------------------*/
/*   Defines for FUR_M0ACCESS_ADDR Bank          */
/*-----------------------------------------------*/
#define FUR_M0ACCESS_ADDR_BASEADR                                                              (0x0000)
#define FUR_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD_ADR                                              (0x8401)
#define FUR_M0ACCESS_ADDR_SLAVE_DRAM_MEM_ADD_ADR                                               (0x8403)
/*-----------------------------------------------*/
/*   Defines for FUR_M0ACCESS_DATA Bank          */
/*-----------------------------------------------*/
#define FUR_M0ACCESS_DATA_BASEADR                                                              (0x0000)
#define FUR_M0ACCESS_DATA_MASTER_DRAM_WDATA_ADR                                                (0x8414)
#define FUR_M0ACCESS_DATA_MASTER_DRAM_RDATA_ADR                                                (0x8415)
#define FUR_M0ACCESS_DATA_SLAVE_DRAM_WDATA_ADR                                                 (0x841a)
#define FUR_M0ACCESS_DATA_SLAVE_DRAM_RDATA_ADR                                                 (0x841b)

/*-----------------------------------------------*/
/*   Defines for CL93n72_IEEE_TX Bank                    */
/*-----------------------------------------------*/
#define CL93n72_IEEE_TX_BaseAdr                                                                (0x0090)
#define CL93n72_IEEE_TX_cl93n72it_BASE_R_PMD_control_register_150_Adr                          (0x0096)
#define CL93n72_IEEE_TX_cl93n72it_BASE_R_PMD_status_register_151_Adr                           (0x0097)
#define CL93n72_IEEE_TX_cl93n72it_BASE_R_LD_coeff_update_register_154_Adr                      (0x009a)
#define CL93n72_IEEE_TX_cl93n72it_BASE_R_LD_status_report_register_155_Adr                     (0x009b)

/*-----------------------------------------------*/
/*   Defines for CL93n72_IEEE_RX Bank                    */
/*-----------------------------------------------*/
#define CL93n72_IEEE_RX_BaseAdr                                                                (0x0090)
#define CL93n72_IEEE_RX_cl93n72ir_BASE_R_LP_coeff_update_register_152_Adr                      (0x0098)
#define CL93n72_IEEE_RX_cl93n72ir_BASE_R_LP_status_report_register_153_Adr                     (0x0099)

/*-----------------------------------------------*/
/*   Defines for IEEE_PMA_PMD_BLK10 Bank                    */
/*-----------------------------------------------*/
#define IEEE_PMA_PMD_BLK10_BaseAdr                                                             (0x00a0)
#define IEEE_PMA_PMD_BLK10_tengbase_kx_control_register_Adr                                    (0x00a0)
#define IEEE_PMA_PMD_BLK10_tengbase_kx_status_register_Adr                                     (0x00a1)

/*-----------------------------------------------*/
/*   Defines for IEEE_CL91_RX Bank                    */
/*-----------------------------------------------*/
#define IEEE_CL91_RX_BaseAdr                                                                   (0x0000)
#define IEEE_CL91_RX_fec_control_Adr                                                           (0x00c8)
#define IEEE_CL91_RX_fec_status_Adr                                                            (0x00c9)
#define IEEE_CL91_RX_fec_corr_cw_cnt_lower_reg_Adr                                             (0x00ca)
#define IEEE_CL91_RX_fec_corr_cw_cnt_upper_reg_Adr                                             (0x00cb)
#define IEEE_CL91_RX_fec_uncorr_cw_cnt_lower_reg_Adr                                           (0x00cc)
#define IEEE_CL91_RX_fec_uncorr_cw_cnt_upper_reg_Adr                                           (0x00cd)
#define IEEE_CL91_RX_fec_lane_mapping_Adr                                                      (0x00ce)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_3_lower_reg_Adr                                        (0x00d2)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_3_upper_reg_Adr                                        (0x00d3)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_2_lower_reg_Adr                                        (0x00d4)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_2_upper_reg_Adr                                        (0x00d5)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_1_lower_reg_Adr                                        (0x00d6)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_1_upper_reg_Adr                                        (0x00d7)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_0_lower_reg_Adr                                        (0x00d8)
#define IEEE_CL91_RX_fec_sym_err_cnt_ln_0_upper_reg_Adr                                        (0x00d9)

/*-----------------------------------------------*/
/*   Defines for IEEE_CL91_TX Bank                    */
/*-----------------------------------------------*/
#define IEEE_CL91_TX_BaseAdr                                                                   (0x0000)
#define IEEE_CL91_TX_bip_err_cnt_ln_0_reg_Adr                                                  (0x00e6)
#define IEEE_CL91_TX_bip_err_cnt_ln_1_reg_Adr                                                  (0x00e7)
#define IEEE_CL91_TX_bip_err_cnt_ln_2_reg_Adr                                                  (0x00e8)
#define IEEE_CL91_TX_bip_err_cnt_ln_3_reg_Adr                                                  (0x00e9)
#define IEEE_CL91_TX_bip_err_cnt_ln_4_reg_Adr                                                  (0x00ea)
#define IEEE_CL91_TX_bip_err_cnt_ln_5_reg_Adr                                                  (0x00eb)
#define IEEE_CL91_TX_bip_err_cnt_ln_6_reg_Adr                                                  (0x00ec)
#define IEEE_CL91_TX_bip_err_cnt_ln_7_reg_Adr                                                  (0x00ed)
#define IEEE_CL91_TX_bip_err_cnt_ln_8_reg_Adr                                                  (0x00ee)
#define IEEE_CL91_TX_bip_err_cnt_ln_9_reg_Adr                                                  (0x00ef)
#define IEEE_CL91_TX_bip_err_cnt_ln_10_reg_Adr                                                 (0x00f0)
#define IEEE_CL91_TX_bip_err_cnt_ln_11_reg_Adr                                                 (0x00f1)
#define IEEE_CL91_TX_bip_err_cnt_ln_12_reg_Adr                                                 (0x00f2)
#define IEEE_CL91_TX_bip_err_cnt_ln_13_reg_Adr                                                 (0x00f3)
#define IEEE_CL91_TX_bip_err_cnt_ln_14_reg_Adr                                                 (0x00f4)
#define IEEE_CL91_TX_bip_err_cnt_ln_15_reg_Adr                                                 (0x00f5)
#define IEEE_CL91_TX_bip_err_cnt_ln_16_reg_Adr                                                 (0x00f6)
#define IEEE_CL91_TX_bip_err_cnt_ln_17_reg_Adr                                                 (0x00f7)
#define IEEE_CL91_TX_bip_err_cnt_ln_18_reg_Adr                                                 (0x00f8)
#define IEEE_CL91_TX_bip_err_cnt_ln_19_reg_Adr                                                 (0x00f9)
#define IEEE_CL91_TX_pcs_ln_0_mapping_reg_Adr                                                  (0x00fa)
#define IEEE_CL91_TX_pcs_ln_1_mapping_reg_Adr                                                  (0x00fb)
#define IEEE_CL91_TX_pcs_ln_2_mapping_reg_Adr                                                  (0x00fc)
#define IEEE_CL91_TX_pcs_ln_3_mapping_reg_Adr                                                  (0x00fd)
#define IEEE_CL91_TX_pcs_ln_4_mapping_reg_Adr                                                  (0x00fe)
#define IEEE_CL91_TX_pcs_ln_5_mapping_reg_Adr                                                  (0x00ff)
#define IEEE_CL91_TX_pcs_ln_6_mapping_reg_Adr                                                  (0x0100)
#define IEEE_CL91_TX_pcs_ln_7_mapping_reg_Adr                                                  (0x0101)
#define IEEE_CL91_TX_pcs_ln_8_mapping_reg_Adr                                                  (0x0102)
#define IEEE_CL91_TX_pcs_ln_9_mapping_reg_Adr                                                  (0x0103)
#define IEEE_CL91_TX_pcs_ln_10_mapping_reg_Adr                                                 (0x0104)
#define IEEE_CL91_TX_pcs_ln_11_mapping_reg_Adr                                                 (0x0105)
#define IEEE_CL91_TX_pcs_ln_12_mapping_reg_Adr                                                 (0x0106)
#define IEEE_CL91_TX_pcs_ln_13_mapping_reg_Adr                                                 (0x0107)
#define IEEE_CL91_TX_pcs_ln_14_mapping_reg_Adr                                                 (0x0108)
#define IEEE_CL91_TX_pcs_ln_15_mapping_reg_Adr                                                 (0x0109)
#define IEEE_CL91_TX_pcs_ln_16_mapping_reg_Adr                                                 (0x010a)
#define IEEE_CL91_TX_pcs_ln_17_mapping_reg_Adr                                                 (0x010b)
#define IEEE_CL91_TX_pcs_ln_18_mapping_reg_Adr                                                 (0x010c)
#define IEEE_CL91_TX_pcs_ln_19_mapping_reg_Adr                                                 (0x010d)
#define IEEE_CL91_TX_pcs_align_status1_reg_Adr                                                 (0x0118)
#define IEEE_CL91_TX_pcs_align_status2_reg_Adr                                                 (0x0119)
#define IEEE_CL91_TX_pcs_align_status3_reg_Adr                                                 (0x011a)
#define IEEE_CL91_TX_pcs_align_status4_reg_Adr                                                 (0x011b)

/*-----------------------------------------------*/
/*   Defines for DEV1_SLICE Bank                    */
/*-----------------------------------------------*/
#define DEV1_SLICE_BaseAdr                                                                     (0x0000)
#define DEV1_SLICE_slice_Adr                                                                   (0x8000)

/*-----------------------------------------------*/
/*   Defines for FUR_MDICS_PMI_ARB_MASK Bank                    */
/*-----------------------------------------------*/
#define FUR_MDICS_PMI_ARB_MASK_BaseAdr                                                         (0x0000)
#define FUR_MDICS_PMI_ARB_MASK_mdics_pmi_arb_mask_Adr                                          (0x8020)

/*-----------------------------------------------*/
/*   Defines for FUR_GEN_CNTRLS Bank                    */
/*-----------------------------------------------*/
#define FUR_GEN_CNTRLS_BaseAdr                                                                 (0x0000)
#define FUR_GEN_CNTRLS_gen_control1_Adr                                                        (0x8200)
#define FUR_GEN_CNTRLS_gen_control2_Adr                                                        (0x8201)
#define FUR_GEN_CNTRLS_gen_control3_Adr                                                        (0x8202)
#define FUR_GEN_CNTRLS_gen_control4_Adr                                                        (0x8203)
#define FUR_GEN_CNTRLS_spi_mst_code_start_ptr_Adr                                              (0x8210)
#define FUR_GEN_CNTRLS_spi_slv_code_start_ptr_Adr                                              (0x8211)
#define FUR_GEN_CNTRLS_spi_code_load_en_Adr                                                    (0x8212)
#define FUR_GEN_CNTRLS_spi_eeprom_control_Adr                                                  (0x8213)
#define FUR_GEN_CNTRLS_mscr_Adr                                                                (0x8214)
#define FUR_GEN_CNTRLS_msgout_Adr                                                              (0x8215)
#define FUR_GEN_CNTRLS_msgin_Adr                                                               (0x8216)
#define FUR_GEN_CNTRLS_boot_Adr                                                                (0x8217)
#define FUR_GEN_CNTRLS_slv_msgout_Adr                                                          (0x8218)
#define FUR_GEN_CNTRLS_slv_msgin_Adr                                                           (0x8219)
#define FUR_GEN_CNTRLS_mst_misc_intr_Adr                                                       (0x821b)
#define FUR_GEN_CNTRLS_slv_misc_intr_Adr                                                       (0x821c)
#define FUR_GEN_CNTRLS_gpreg10_Adr                                                             (0x822a)
#define FUR_GEN_CNTRLS_gpreg11_Adr                                                             (0x822b)
#define FUR_GEN_CNTRLS_gpreg12_Adr                                                             (0x822c)
#define FUR_GEN_CNTRLS_gpreg13_Adr                                                             (0x822d)
#define FUR_GEN_CNTRLS_gpreg14_Adr                                                             (0x822e)
#define FUR_GEN_CNTRLS_gpreg15_Adr                                                             (0x822f)
#define FUR_GEN_CNTRLS_mst_running_chksum_Adr                                                  (0x8230)
#define FUR_GEN_CNTRLS_slv_running_chksum_Adr                                                  (0x8231)
#define FUR_GEN_CNTRLS_mst_running_byte_cnt_Adr                                                (0x8232)
#define FUR_GEN_CNTRLS_slv_running_byte_cnt_Adr                                                (0x8233)
#define FUR_GEN_CNTRLS_firmware_version_Adr                                                    (0x8234)
#define FUR_GEN_CNTRLS_firmware_enable_Adr                                                     (0x8235)
#define FUR_GEN_CNTRLS_micro_sync_7_Adr                                                        (0x8247)
#define FUR_GEN_CNTRLS_micro_sync_6_Adr                                                        (0x8246)

/*-----------------------------------------------*/
/*   Defines for FUR_MICRO_BOOT Bank                    */
/*-----------------------------------------------*/
#define FUR_MICRO_BOOT_BaseAdr                                                                 (0x0000)
#define FUR_MICRO_BOOT_boot_por_Adr                                                            (0x82ff)

/*-----------------------------------------------*/
/*   Defines for FUR_MODULE_CNTRL Bank                    */
/*-----------------------------------------------*/
#define FUR_MODULE_CNTRL_BaseAdr                                                               (0x0000)
#define FUR_MODULE_CNTRL_CONTROL_Adr                                                           (0x8800)
#define FUR_MODULE_CNTRL_STATUS_Adr                                                            (0x8801)
#define FUR_MODULE_CNTRL_XFER_COUNT_Adr                                                        (0x8802)
#define FUR_MODULE_CNTRL_ADDRESS_Adr                                                           (0x8803)
#define FUR_MODULE_CNTRL_XFER_ADDR_Adr                                                         (0x8804)
#define FUR_MODULE_CNTRL_DEV_ID_Adr                                                            (0x8805)

/*-----------------------------------------------*/
/*   Defines for AMS_RX_CTRL Registers                    */
/*-----------------------------------------------*/
#define FUR_AMS_RX_RX_CNTRL0_Adr                                                               (0xd0c0)

/*-----------------------------------------------*/
/*   Defines for FUR_MODULE_CNTRL_RAM Bank                    */
/*-----------------------------------------------*/
#define FUR_MODULE_CNTRL_RAM_BaseAdr                                                           (0x0000)
#define FUR_MODULE_CNTRL_RAM_NVR0_Adr                                                          (0x8807)
#define FUR_MODULE_CNTRL_RAM_NVR1_Adr                                                          (0x8808)
#define FUR_MODULE_CNTRL_RAM_NVR2_Adr                                                          (0x8809)
#define FUR_MODULE_CNTRL_RAM_NVR3_Adr                                                          (0x880a)
#define FUR_MODULE_CNTRL_RAM_NVR4_Adr                                                          (0x880b)
#define FUR_MODULE_CNTRL_RAM_NVR5_Adr                                                          (0x880c)
#define FUR_MODULE_CNTRL_RAM_NVR6_Adr                                                          (0x880d)
#define FUR_MODULE_CNTRL_RAM_NVR7_Adr                                                          (0x880e)
#define FUR_MODULE_CNTRL_RAM_NVR8_Adr                                                          (0x880f)
#define FUR_MODULE_CNTRL_RAM_NVR9_Adr                                                          (0x8810)
#define FUR_MODULE_CNTRL_RAM_NVR10_Adr                                                         (0x8811)
#define FUR_MODULE_CNTRL_RAM_NVR11_Adr                                                         (0x8812)
#define FUR_MODULE_CNTRL_RAM_NVR12_Adr                                                         (0x8813)
#define FUR_MODULE_CNTRL_RAM_NVR13_Adr                                                         (0x8814)
#define FUR_MODULE_CNTRL_RAM_NVR14_Adr                                                         (0x8815)
#define FUR_MODULE_CNTRL_RAM_NVR15_Adr                                                         (0x8816)
#define FUR_MODULE_CNTRL_RAM_NVR16_Adr                                                         (0x8817)
#define FUR_MODULE_CNTRL_RAM_NVR17_Adr                                                         (0x8818)
#define FUR_MODULE_CNTRL_RAM_NVR18_Adr                                                         (0x8819)
#define FUR_MODULE_CNTRL_RAM_NVR19_Adr                                                         (0x881a)
#define FUR_MODULE_CNTRL_RAM_NVR20_Adr                                                         (0x881b)
#define FUR_MODULE_CNTRL_RAM_NVR21_Adr                                                         (0x881c)
#define FUR_MODULE_CNTRL_RAM_NVR22_Adr                                                         (0x881d)
#define FUR_MODULE_CNTRL_RAM_NVR23_Adr                                                         (0x881e)
#define FUR_MODULE_CNTRL_RAM_NVR24_Adr                                                         (0x881f)
#define FUR_MODULE_CNTRL_RAM_NVR25_Adr                                                         (0x8820)
#define FUR_MODULE_CNTRL_RAM_NVR26_Adr                                                         (0x8821)
#define FUR_MODULE_CNTRL_RAM_NVR27_Adr                                                         (0x8822)
#define FUR_MODULE_CNTRL_RAM_NVR28_Adr                                                         (0x8823)
#define FUR_MODULE_CNTRL_RAM_NVR29_Adr                                                         (0x8824)
#define FUR_MODULE_CNTRL_RAM_NVR30_Adr                                                         (0x8825)
#define FUR_MODULE_CNTRL_RAM_NVR31_Adr                                                         (0x8826)
#define FUR_MODULE_CNTRL_RAM_NVR32_Adr                                                         (0x8827)
#define FUR_MODULE_CNTRL_RAM_NVR33_Adr                                                         (0x8828)
#define FUR_MODULE_CNTRL_RAM_NVR34_Adr                                                         (0x8829)
#define FUR_MODULE_CNTRL_RAM_NVR35_Adr                                                         (0x882a)
#define FUR_MODULE_CNTRL_RAM_NVR36_Adr                                                         (0x882b)
#define FUR_MODULE_CNTRL_RAM_NVR37_Adr                                                         (0x882c)
#define FUR_MODULE_CNTRL_RAM_NVR38_Adr                                                         (0x882d)
#define FUR_MODULE_CNTRL_RAM_NVR39_Adr                                                         (0x882e)
#define FUR_MODULE_CNTRL_RAM_NVR40_Adr                                                         (0x882f)
#define FUR_MODULE_CNTRL_RAM_NVR41_Adr                                                         (0x8830)
#define FUR_MODULE_CNTRL_RAM_NVR42_Adr                                                         (0x8831)
#define FUR_MODULE_CNTRL_RAM_NVR43_Adr                                                         (0x8832)
#define FUR_MODULE_CNTRL_RAM_NVR44_Adr                                                         (0x8833)
#define FUR_MODULE_CNTRL_RAM_NVR45_Adr                                                         (0x8834)
#define FUR_MODULE_CNTRL_RAM_NVR46_Adr                                                         (0x8835)
#define FUR_MODULE_CNTRL_RAM_NVR47_Adr                                                         (0x8836)
#define FUR_MODULE_CNTRL_RAM_NVR48_Adr                                                         (0x8837)
#define FUR_MODULE_CNTRL_RAM_NVR49_Adr                                                         (0x8838)
#define FUR_MODULE_CNTRL_RAM_NVR50_Adr                                                         (0x8839)
#define FUR_MODULE_CNTRL_RAM_NVR51_Adr                                                         (0x883a)
#define FUR_MODULE_CNTRL_RAM_NVR52_Adr                                                         (0x883b)
#define FUR_MODULE_CNTRL_RAM_NVR53_Adr                                                         (0x883c)
#define FUR_MODULE_CNTRL_RAM_NVR54_Adr                                                         (0x883d)
#define FUR_MODULE_CNTRL_RAM_NVR55_Adr                                                         (0x883e)
#define FUR_MODULE_CNTRL_RAM_NVR56_Adr                                                         (0x883f)
#define FUR_MODULE_CNTRL_RAM_NVR57_Adr                                                         (0x8840)
#define FUR_MODULE_CNTRL_RAM_NVR58_Adr                                                         (0x8841)
#define FUR_MODULE_CNTRL_RAM_NVR59_Adr                                                         (0x8842)
#define FUR_MODULE_CNTRL_RAM_NVR60_Adr                                                         (0x8843)
#define FUR_MODULE_CNTRL_RAM_NVR61_Adr                                                         (0x8844)
#define FUR_MODULE_CNTRL_RAM_NVR62_Adr                                                         (0x8845)
#define FUR_MODULE_CNTRL_RAM_NVR63_Adr                                                         (0x8846)
#define FUR_MODULE_CNTRL_RAM_NVR64_Adr                                                         (0x8847)
#define FUR_MODULE_CNTRL_RAM_NVR65_Adr                                                         (0x8848)
#define FUR_MODULE_CNTRL_RAM_NVR66_Adr                                                         (0x8849)
#define FUR_MODULE_CNTRL_RAM_NVR67_Adr                                                         (0x884a)
#define FUR_MODULE_CNTRL_RAM_NVR68_Adr                                                         (0x884b)
#define FUR_MODULE_CNTRL_RAM_NVR69_Adr                                                         (0x884c)
#define FUR_MODULE_CNTRL_RAM_NVR70_Adr                                                         (0x884d)
#define FUR_MODULE_CNTRL_RAM_NVR71_Adr                                                         (0x884e)
#define FUR_MODULE_CNTRL_RAM_NVR72_Adr                                                         (0x884f)
#define FUR_MODULE_CNTRL_RAM_NVR73_Adr                                                         (0x8850)
#define FUR_MODULE_CNTRL_RAM_NVR74_Adr                                                         (0x8851)
#define FUR_MODULE_CNTRL_RAM_NVR75_Adr                                                         (0x8852)
#define FUR_MODULE_CNTRL_RAM_NVR76_Adr                                                         (0x8853)
#define FUR_MODULE_CNTRL_RAM_NVR77_Adr                                                         (0x8854)
#define FUR_MODULE_CNTRL_RAM_NVR78_Adr                                                         (0x8855)
#define FUR_MODULE_CNTRL_RAM_NVR79_Adr                                                         (0x8856)
#define FUR_MODULE_CNTRL_RAM_NVR80_Adr                                                         (0x8857)
#define FUR_MODULE_CNTRL_RAM_NVR81_Adr                                                         (0x8858)
#define FUR_MODULE_CNTRL_RAM_NVR82_Adr                                                         (0x8859)
#define FUR_MODULE_CNTRL_RAM_NVR83_Adr                                                         (0x885a)
#define FUR_MODULE_CNTRL_RAM_NVR84_Adr                                                         (0x885b)
#define FUR_MODULE_CNTRL_RAM_NVR85_Adr                                                         (0x885c)
#define FUR_MODULE_CNTRL_RAM_NVR86_Adr                                                         (0x885d)
#define FUR_MODULE_CNTRL_RAM_NVR87_Adr                                                         (0x885e)
#define FUR_MODULE_CNTRL_RAM_NVR88_Adr                                                         (0x885f)
#define FUR_MODULE_CNTRL_RAM_NVR89_Adr                                                         (0x8860)
#define FUR_MODULE_CNTRL_RAM_NVR90_Adr                                                         (0x8861)
#define FUR_MODULE_CNTRL_RAM_NVR91_Adr                                                         (0x8862)
#define FUR_MODULE_CNTRL_RAM_NVR92_Adr                                                         (0x8863)
#define FUR_MODULE_CNTRL_RAM_NVR93_Adr                                                         (0x8864)
#define FUR_MODULE_CNTRL_RAM_NVR94_Adr                                                         (0x8865)
#define FUR_MODULE_CNTRL_RAM_NVR95_Adr                                                         (0x8866)
#define FUR_MODULE_CNTRL_RAM_NVR96_Adr                                                         (0x8867)
#define FUR_MODULE_CNTRL_RAM_NVR97_Adr                                                         (0x8868)
#define FUR_MODULE_CNTRL_RAM_NVR98_Adr                                                         (0x8869)
#define FUR_MODULE_CNTRL_RAM_NVR99_Adr                                                         (0x886a)
#define FUR_MODULE_CNTRL_RAM_NVR100_Adr                                                        (0x886b)
#define FUR_MODULE_CNTRL_RAM_NVR101_Adr                                                        (0x886c)
#define FUR_MODULE_CNTRL_RAM_NVR102_Adr                                                        (0x886d)
#define FUR_MODULE_CNTRL_RAM_NVR103_Adr                                                        (0x886e)
#define FUR_MODULE_CNTRL_RAM_NVR104_Adr                                                        (0x886f)
#define FUR_MODULE_CNTRL_RAM_NVR105_Adr                                                        (0x8870)
#define FUR_MODULE_CNTRL_RAM_NVR106_Adr                                                        (0x8871)
#define FUR_MODULE_CNTRL_RAM_NVR107_Adr                                                        (0x8872)
#define FUR_MODULE_CNTRL_RAM_NVR108_Adr                                                        (0x8873)
#define FUR_MODULE_CNTRL_RAM_NVR109_Adr                                                        (0x8874)
#define FUR_MODULE_CNTRL_RAM_NVR110_Adr                                                        (0x8875)
#define FUR_MODULE_CNTRL_RAM_NVR111_Adr                                                        (0x8876)
#define FUR_MODULE_CNTRL_RAM_NVR112_Adr                                                        (0x8877)
#define FUR_MODULE_CNTRL_RAM_NVR113_Adr                                                        (0x8878)
#define FUR_MODULE_CNTRL_RAM_NVR114_Adr                                                        (0x8879)
#define FUR_MODULE_CNTRL_RAM_NVR115_Adr                                                        (0x887a)
#define FUR_MODULE_CNTRL_RAM_NVR116_Adr                                                        (0x887b)
#define FUR_MODULE_CNTRL_RAM_NVR117_Adr                                                        (0x887c)
#define FUR_MODULE_CNTRL_RAM_NVR118_Adr                                                        (0x887d)
#define FUR_MODULE_CNTRL_RAM_NVR119_Adr                                                        (0x887e)
#define FUR_MODULE_CNTRL_RAM_NVR120_Adr                                                        (0x887f)
#define FUR_MODULE_CNTRL_RAM_NVR121_Adr                                                        (0x8880)
#define FUR_MODULE_CNTRL_RAM_NVR122_Adr                                                        (0x8881)
#define FUR_MODULE_CNTRL_RAM_NVR123_Adr                                                        (0x8882)
#define FUR_MODULE_CNTRL_RAM_NVR124_Adr                                                        (0x8883)
#define FUR_MODULE_CNTRL_RAM_NVR125_Adr                                                        (0x8884)
#define FUR_MODULE_CNTRL_RAM_NVR126_Adr                                                        (0x8885)
#define FUR_MODULE_CNTRL_RAM_NVR127_Adr                                                        (0x8886)
#define FUR_MODULE_CNTRL_RAM_NVR128_Adr                                                        (0x8887)
#define FUR_MODULE_CNTRL_RAM_NVR129_Adr                                                        (0x8888)
#define FUR_MODULE_CNTRL_RAM_NVR130_Adr                                                        (0x8889)
#define FUR_MODULE_CNTRL_RAM_NVR131_Adr                                                        (0x888a)
#define FUR_MODULE_CNTRL_RAM_NVR132_Adr                                                        (0x888b)
#define FUR_MODULE_CNTRL_RAM_NVR133_Adr                                                        (0x888c)
#define FUR_MODULE_CNTRL_RAM_NVR134_Adr                                                        (0x888d)
#define FUR_MODULE_CNTRL_RAM_NVR135_Adr                                                        (0x888e)
#define FUR_MODULE_CNTRL_RAM_NVR136_Adr                                                        (0x888f)
#define FUR_MODULE_CNTRL_RAM_NVR137_Adr                                                        (0x8890)
#define FUR_MODULE_CNTRL_RAM_NVR138_Adr                                                        (0x8891)
#define FUR_MODULE_CNTRL_RAM_NVR139_Adr                                                        (0x8892)
#define FUR_MODULE_CNTRL_RAM_NVR140_Adr                                                        (0x8893)
#define FUR_MODULE_CNTRL_RAM_NVR141_Adr                                                        (0x8894)
#define FUR_MODULE_CNTRL_RAM_NVR142_Adr                                                        (0x8895)
#define FUR_MODULE_CNTRL_RAM_NVR143_Adr                                                        (0x8896)
#define FUR_MODULE_CNTRL_RAM_NVR144_Adr                                                        (0x8897)
#define FUR_MODULE_CNTRL_RAM_NVR145_Adr                                                        (0x8898)
#define FUR_MODULE_CNTRL_RAM_NVR146_Adr                                                        (0x8899)
#define FUR_MODULE_CNTRL_RAM_NVR147_Adr                                                        (0x889a)
#define FUR_MODULE_CNTRL_RAM_NVR148_Adr                                                        (0x889b)
#define FUR_MODULE_CNTRL_RAM_NVR149_Adr                                                        (0x889c)
#define FUR_MODULE_CNTRL_RAM_NVR150_Adr                                                        (0x889d)
#define FUR_MODULE_CNTRL_RAM_NVR151_Adr                                                        (0x889e)
#define FUR_MODULE_CNTRL_RAM_NVR152_Adr                                                        (0x889f)
#define FUR_MODULE_CNTRL_RAM_NVR153_Adr                                                        (0x88a0)
#define FUR_MODULE_CNTRL_RAM_NVR154_Adr                                                        (0x88a1)
#define FUR_MODULE_CNTRL_RAM_NVR155_Adr                                                        (0x88a2)
#define FUR_MODULE_CNTRL_RAM_NVR156_Adr                                                        (0x88a3)
#define FUR_MODULE_CNTRL_RAM_NVR157_Adr                                                        (0x88a4)
#define FUR_MODULE_CNTRL_RAM_NVR158_Adr                                                        (0x88a5)
#define FUR_MODULE_CNTRL_RAM_NVR159_Adr                                                        (0x88a6)
#define FUR_MODULE_CNTRL_RAM_NVR160_Adr                                                        (0x88a7)
#define FUR_MODULE_CNTRL_RAM_NVR161_Adr                                                        (0x88a8)
#define FUR_MODULE_CNTRL_RAM_NVR162_Adr                                                        (0x88a9)
#define FUR_MODULE_CNTRL_RAM_NVR163_Adr                                                        (0x88aa)
#define FUR_MODULE_CNTRL_RAM_NVR164_Adr                                                        (0x88ab)
#define FUR_MODULE_CNTRL_RAM_NVR165_Adr                                                        (0x88ac)
#define FUR_MODULE_CNTRL_RAM_NVR166_Adr                                                        (0x88ad)
#define FUR_MODULE_CNTRL_RAM_NVR167_Adr                                                        (0x88ae)
#define FUR_MODULE_CNTRL_RAM_NVR168_Adr                                                        (0x88af)
#define FUR_MODULE_CNTRL_RAM_NVR169_Adr                                                        (0x88b0)
#define FUR_MODULE_CNTRL_RAM_NVR170_Adr                                                        (0x88b1)
#define FUR_MODULE_CNTRL_RAM_NVR171_Adr                                                        (0x88b2)
#define FUR_MODULE_CNTRL_RAM_NVR172_Adr                                                        (0x88b3)
#define FUR_MODULE_CNTRL_RAM_NVR173_Adr                                                        (0x88b4)
#define FUR_MODULE_CNTRL_RAM_NVR174_Adr                                                        (0x88b5)
#define FUR_MODULE_CNTRL_RAM_NVR175_Adr                                                        (0x88b6)
#define FUR_MODULE_CNTRL_RAM_NVR176_Adr                                                        (0x88b7)
#define FUR_MODULE_CNTRL_RAM_NVR177_Adr                                                        (0x88b8)
#define FUR_MODULE_CNTRL_RAM_NVR178_Adr                                                        (0x88b9)
#define FUR_MODULE_CNTRL_RAM_NVR179_Adr                                                        (0x88ba)
#define FUR_MODULE_CNTRL_RAM_NVR180_Adr                                                        (0x88bb)
#define FUR_MODULE_CNTRL_RAM_NVR181_Adr                                                        (0x88bc)
#define FUR_MODULE_CNTRL_RAM_NVR182_Adr                                                        (0x88bd)
#define FUR_MODULE_CNTRL_RAM_NVR183_Adr                                                        (0x88be)
#define FUR_MODULE_CNTRL_RAM_NVR184_Adr                                                        (0x88bf)
#define FUR_MODULE_CNTRL_RAM_NVR185_Adr                                                        (0x88c0)
#define FUR_MODULE_CNTRL_RAM_NVR186_Adr                                                        (0x88c1)
#define FUR_MODULE_CNTRL_RAM_NVR187_Adr                                                        (0x88c2)
#define FUR_MODULE_CNTRL_RAM_NVR188_Adr                                                        (0x88c3)
#define FUR_MODULE_CNTRL_RAM_NVR189_Adr                                                        (0x88c4)
#define FUR_MODULE_CNTRL_RAM_NVR190_Adr                                                        (0x88c5)
#define FUR_MODULE_CNTRL_RAM_NVR191_Adr                                                        (0x88c6)
#define FUR_MODULE_CNTRL_RAM_NVR192_Adr                                                        (0x88c7)
#define FUR_MODULE_CNTRL_RAM_NVR193_Adr                                                        (0x88c8)
#define FUR_MODULE_CNTRL_RAM_NVR194_Adr                                                        (0x88c9)
#define FUR_MODULE_CNTRL_RAM_NVR195_Adr                                                        (0x88ca)
#define FUR_MODULE_CNTRL_RAM_NVR196_Adr                                                        (0x88cb)
#define FUR_MODULE_CNTRL_RAM_NVR197_Adr                                                        (0x88cc)
#define FUR_MODULE_CNTRL_RAM_NVR198_Adr                                                        (0x88cd)
#define FUR_MODULE_CNTRL_RAM_NVR199_Adr                                                        (0x88ce)
#define FUR_MODULE_CNTRL_RAM_NVR200_Adr                                                        (0x88cf)
#define FUR_MODULE_CNTRL_RAM_NVR201_Adr                                                        (0x88d0)
#define FUR_MODULE_CNTRL_RAM_NVR202_Adr                                                        (0x88d1)
#define FUR_MODULE_CNTRL_RAM_NVR203_Adr                                                        (0x88d2)
#define FUR_MODULE_CNTRL_RAM_NVR204_Adr                                                        (0x88d3)
#define FUR_MODULE_CNTRL_RAM_NVR205_Adr                                                        (0x88d4)
#define FUR_MODULE_CNTRL_RAM_NVR206_Adr                                                        (0x88d5)
#define FUR_MODULE_CNTRL_RAM_NVR207_Adr                                                        (0x88d6)
#define FUR_MODULE_CNTRL_RAM_NVR208_Adr                                                        (0x88d7)
#define FUR_MODULE_CNTRL_RAM_NVR209_Adr                                                        (0x88d8)
#define FUR_MODULE_CNTRL_RAM_NVR210_Adr                                                        (0x88d9)
#define FUR_MODULE_CNTRL_RAM_NVR211_Adr                                                        (0x88da)
#define FUR_MODULE_CNTRL_RAM_NVR212_Adr                                                        (0x88db)
#define FUR_MODULE_CNTRL_RAM_NVR213_Adr                                                        (0x88dc)
#define FUR_MODULE_CNTRL_RAM_NVR214_Adr                                                        (0x88dd)
#define FUR_MODULE_CNTRL_RAM_NVR215_Adr                                                        (0x88de)
#define FUR_MODULE_CNTRL_RAM_NVR216_Adr                                                        (0x88df)
#define FUR_MODULE_CNTRL_RAM_NVR217_Adr                                                        (0x88e0)
#define FUR_MODULE_CNTRL_RAM_NVR218_Adr                                                        (0x88e1)
#define FUR_MODULE_CNTRL_RAM_NVR219_Adr                                                        (0x88e2)
#define FUR_MODULE_CNTRL_RAM_NVR220_Adr                                                        (0x88e3)
#define FUR_MODULE_CNTRL_RAM_NVR221_Adr                                                        (0x88e4)
#define FUR_MODULE_CNTRL_RAM_NVR222_Adr                                                        (0x88e5)
#define FUR_MODULE_CNTRL_RAM_NVR223_Adr                                                        (0x88e6)
#define FUR_MODULE_CNTRL_RAM_NVR224_Adr                                                        (0x88e7)
#define FUR_MODULE_CNTRL_RAM_NVR225_Adr                                                        (0x88e8)
#define FUR_MODULE_CNTRL_RAM_NVR226_Adr                                                        (0x88e9)
#define FUR_MODULE_CNTRL_RAM_NVR227_Adr                                                        (0x88ea)
#define FUR_MODULE_CNTRL_RAM_NVR228_Adr                                                        (0x88eb)
#define FUR_MODULE_CNTRL_RAM_NVR229_Adr                                                        (0x88ec)
#define FUR_MODULE_CNTRL_RAM_NVR230_Adr                                                        (0x88ed)
#define FUR_MODULE_CNTRL_RAM_NVR231_Adr                                                        (0x88ee)
#define FUR_MODULE_CNTRL_RAM_NVR232_Adr                                                        (0x88ef)
#define FUR_MODULE_CNTRL_RAM_NVR233_Adr                                                        (0x88f0)
#define FUR_MODULE_CNTRL_RAM_NVR234_Adr                                                        (0x88f1)
#define FUR_MODULE_CNTRL_RAM_NVR235_Adr                                                        (0x88f2)
#define FUR_MODULE_CNTRL_RAM_NVR236_Adr                                                        (0x88f3)
#define FUR_MODULE_CNTRL_RAM_NVR237_Adr                                                        (0x88f4)
#define FUR_MODULE_CNTRL_RAM_NVR238_Adr                                                        (0x88f5)
#define FUR_MODULE_CNTRL_RAM_NVR239_Adr                                                        (0x88f6)
#define FUR_MODULE_CNTRL_RAM_NVR240_Adr                                                        (0x88f7)
#define FUR_MODULE_CNTRL_RAM_NVR241_Adr                                                        (0x88f8)
#define FUR_MODULE_CNTRL_RAM_NVR242_Adr                                                        (0x88f9)
#define FUR_MODULE_CNTRL_RAM_NVR243_Adr                                                        (0x88fa)
#define FUR_MODULE_CNTRL_RAM_NVR244_Adr                                                        (0x88fb)
#define FUR_MODULE_CNTRL_RAM_NVR245_Adr                                                        (0x88fc)
#define FUR_MODULE_CNTRL_RAM_NVR246_Adr                                                        (0x88fd)
#define FUR_MODULE_CNTRL_RAM_NVR247_Adr                                                        (0x88fe)
#define FUR_MODULE_CNTRL_RAM_NVR248_Adr                                                        (0x88ff)
#define FUR_MODULE_CNTRL_RAM_NVR249_Adr                                                        (0x8900)
#define FUR_MODULE_CNTRL_RAM_NVR250_Adr                                                        (0x8901)
#define FUR_MODULE_CNTRL_RAM_NVR251_Adr                                                        (0x8902)
#define FUR_MODULE_CNTRL_RAM_NVR252_Adr                                                        (0x8903)
#define FUR_MODULE_CNTRL_RAM_NVR253_Adr                                                        (0x8904)
#define FUR_MODULE_CNTRL_RAM_NVR254_Adr                                                        (0x8905)
#define FUR_MODULE_CNTRL_RAM_NVR255_Adr                                                        (0x8906)

/*-----------------------------------------------*/
/*   Defines for FUR_MISC_CTRL Bank                    */
/*-----------------------------------------------*/
#define FUR_MISC_CTRL_BaseAdr                                                                  (0x0000)
#define FUR_MISC_CTRL_chip_id_Adr                                                              (0x8a00)
#define FUR_MISC_CTRL_chip_revision_Adr                                                        (0x8a01)
#define FUR_MISC_CTRL_mode_dec_frc_Adr                                                         (0x8a02)
#define FUR_MISC_CTRL_udms_phy_Adr                                                             (0x8a03)
#define FUR_MISC_CTRL_udms_link_Adr                                                            (0x8a04)
#define FUR_MISC_CTRL_mode_dec_frc_val_Adr                                                     (0x8a05)
#define FUR_MISC_CTRL_phy_type_status_Adr                                                      (0x8a06)
#define FUR_MISC_CTRL_link_type_status_Adr                                                     (0x8a07)
#define FUR_MISC_CTRL_logic_to_phy_ln0_map_Adr                                                 (0x8a08)
#define FUR_MISC_CTRL_m0_eisr_Adr                                                              (0x8a10)
#define FUR_MISC_CTRL_m0_eipr_Adr                                                              (0x8a11)
#define FUR_MISC_CTRL_m0_eimr_Adr                                                              (0x8a12)
#define FUR_MISC_CTRL_sf_eisr_Adr                                                              (0x8a13)
#define FUR_MISC_CTRL_sf_eipr_Adr                                                              (0x8a14)
#define FUR_MISC_CTRL_sf_eimr_Adr                                                              (0x8a15)
#define FUR_MISC_CTRL_lf_eisr_Adr                                                              (0x8a16)
#define FUR_MISC_CTRL_lf_eipr_Adr                                                              (0x8a17)
#define FUR_MISC_CTRL_lf_eimr_Adr                                                              (0x8a18)
#define FUR_MISC_CTRL_rx_eisr_Adr                                                              (0x8a19)
#define FUR_MISC_CTRL_rx_eipr_Adr                                                              (0x8a1a)
#define FUR_MISC_CTRL_rx_eimr_Adr                                                              (0x8a1b)
#define FUR_MISC_CTRL_tx_eisr_Adr                                                              (0x8a1c)
#define FUR_MISC_CTRL_tx_eipr_Adr                                                              (0x8a1d)
#define FUR_MISC_CTRL_tx_eimr_Adr                                                              (0x8a1e)

/*-----------------------------------------------*/
/*   Defines for CL91_USER_RX Bank                    */
/*-----------------------------------------------*/
#define CL91_USER_RX_BaseAdr                                                                   (0x9400)
#define CL91_USER_RX_status_Adr                                                                (0x9400)
#define CL91_USER_RX_latched_status_Adr                                                        (0x9401)
#define CL91_USER_RX_skew_ln3_Adr                                                              (0x9402)
#define CL91_USER_RX_skew_ln2_Adr                                                              (0x9403)
#define CL91_USER_RX_skew_ln1_Adr                                                              (0x9404)
#define CL91_USER_RX_skew_ln0_Adr                                                              (0x9405)
#define CL91_USER_RX_phy_ln3_mapping_ctrl1_Adr                                                 (0x9406)
#define CL91_USER_RX_phy_ln3_mapping_ctrl2_Adr                                                 (0x9407)
#define CL91_USER_RX_phy_ln3_mapping_ctrl3_Adr                                                 (0x9408)
#define CL91_USER_RX_phy_ln2_mapping_ctrl1_Adr                                                 (0x9409)
#define CL91_USER_RX_phy_ln2_mapping_ctrl2_Adr                                                 (0x940a)
#define CL91_USER_RX_phy_ln2_mapping_ctrl3_Adr                                                 (0x940b)
#define CL91_USER_RX_phy_ln1_mapping_ctrl1_Adr                                                 (0x940c)
#define CL91_USER_RX_phy_ln1_mapping_ctrl2_Adr                                                 (0x940d)
#define CL91_USER_RX_phy_ln1_mapping_ctrl3_Adr                                                 (0x940e)
#define CL91_USER_RX_phy_ln0_mapping_ctrl1_Adr                                                 (0x940f)
#define CL91_USER_RX_phy_ln0_mapping_ctrl2_Adr                                                 (0x9410)
#define CL91_USER_RX_phy_ln0_mapping_ctrl3_Adr                                                 (0x9411)
#define CL91_USER_RX_debug_ctrl_Adr                                                            (0x9420)
#define CL91_USER_RX_debug_status1_Adr                                                         (0x9421)
#define CL91_USER_RX_debug_status2_Adr                                                         (0x9422)

/*-----------------------------------------------*/
/*   Defines for CL91_USER_TX Bank                    */
/*-----------------------------------------------*/
#define CL91_USER_TX_BaseAdr                                                                   (0x9c00)
#define CL91_USER_TX_control_Adr                                                               (0x9c00)
#define CL91_USER_TX_status1_Adr                                                               (0x9c01)
#define CL91_USER_TX_status2_Adr                                                               (0x9c02)
#define CL91_USER_TX_status3_Adr                                                               (0x9c03)
#define CL91_USER_TX_status4_Adr                                                               (0x9c04)
#define CL91_USER_TX_status5_Adr                                                               (0x9c05)
#define CL91_USER_TX_ln0_status_Adr                                                            (0x9c06)
#define CL91_USER_TX_ln1_status_Adr                                                            (0x9c07)
#define CL91_USER_TX_ln2_status_Adr                                                            (0x9c08)
#define CL91_USER_TX_ln3_status_Adr                                                            (0x9c09)
#define CL91_USER_TX_ln4_status_Adr                                                            (0x9c0a)
#define CL91_USER_TX_ln5_status_Adr                                                            (0x9c0b)
#define CL91_USER_TX_ln6_status_Adr                                                            (0x9c0c)
#define CL91_USER_TX_ln7_status_Adr                                                            (0x9c0d)
#define CL91_USER_TX_ln8_status_Adr                                                            (0x9c0e)
#define CL91_USER_TX_ln9_status_Adr                                                            (0x9c0f)
#define CL91_USER_TX_ln10_status_Adr                                                           (0x9c10)
#define CL91_USER_TX_ln11_status_Adr                                                           (0x9c11)
#define CL91_USER_TX_ln12_status_Adr                                                           (0x9c12)
#define CL91_USER_TX_ln13_status_Adr                                                           (0x9c13)
#define CL91_USER_TX_ln14_status_Adr                                                           (0x9c14)
#define CL91_USER_TX_ln15_status_Adr                                                           (0x9c15)
#define CL91_USER_TX_ln16_status_Adr                                                           (0x9c16)
#define CL91_USER_TX_ln17_status_Adr                                                           (0x9c17)
#define CL91_USER_TX_ln18_status_Adr                                                           (0x9c18)
#define CL91_USER_TX_ln19_status_Adr                                                           (0x9c19)

/*-----------------------------------------------*/
/*   Defines for XGXS_BLK0 Bank                    */
/*-----------------------------------------------*/
#define XGXS_BLK0_BaseAdr                                                                      (0xc000)
#define XGXS_BLK0_xgxsControl_Adr                                                              (0xc000)
#define XGXS_BLK0_xgxsStatus_Adr                                                               (0xc001)
#define XGXS_BLK0_miscControl1_Adr                                                             (0xc00e)

/*-----------------------------------------------*/
/*   Defines for TX_ANA_REGS Bank                    */
/*-----------------------------------------------*/
#define TX_ANA_REGS_BaseAdr                                                                    (0xc080)
#define TX_ANA_REGS_anaTxAStatus_Adr                                                           (0xc080)
#define TX_ANA_REGS_anaTxAControl0_Adr                                                         (0xc081)
#define TX_ANA_REGS_anaTxAControl7_Adr                                                         (0xc08b)

/*-----------------------------------------------*/
/*   Defines for RX_ANA_REGS Bank                    */
/*-----------------------------------------------*/
#define RX_ANA_REGS_BaseAdr                                                                    (0xc0b0)
#define RX_ANA_REGS_anaRxStatus_Adr                                                            (0xc0b0)
#define RX_ANA_REGS_anaRxControl_Adr                                                           (0xc0b1)
#define RX_ANA_REGS_anaRxSigdet_Adr                                                            (0xc0b4)
#define RX_ANA_REGS_anaRxControl2_Adr                                                          (0xc0b6)
#define RX_ANA_REGS_anaRxControl3_Adr                                                          (0xc0ba)

/*-----------------------------------------------*/
/*   Defines for XGXS_BLK2 Bank                    */
/*-----------------------------------------------*/
#define XGXS_BLK2_BaseAdr                                                                      (0xc100)
#define XGXS_BLK2_laneReset_Adr                                                                (0xc10a)

/*-----------------------------------------------*/
/*   Defines for XGXS_BLK8 Bank                    */
/*-----------------------------------------------*/
#define XGXS_BLK8_BaseAdr                                                                      (0xc160)
#define XGXS_BLK8_cl73Control9_Adr                                                             (0xc162)

/*-----------------------------------------------*/
/*   Defines for PMA_PMD_GNRTL_STATUS Bank                    */
/*-----------------------------------------------*/
#define PMA_PMD_GNRTL_STATUS_BaseAdr                                                           (0xc1d0)
#define PMA_PMD_GNRTL_STATUS_GP2_0_Adr                                                         (0xc1d0)
#define PMA_PMD_GNRTL_STATUS_GP2_1_Adr                                                         (0xc1d1)
#define PMA_PMD_GNRTL_STATUS_GP2_2_Adr                                                         (0xc1d2)
#define PMA_PMD_GNRTL_STATUS_GP2_3_Adr                                                         (0xc1d3)
#define PMA_PMD_GNRTL_STATUS_GP2_4_Adr                                                         (0xc1d4)
#define PMA_PMD_GNRTL_STATUS_GP2_5_Adr                                                         (0xc1d5)
#define PMA_PMD_GNRTL_STATUS_GP2_6_Adr                                                         (0xc1d6)
#define PMA_PMD_GNRTL_STATUS_GP2_7_Adr                                                         (0xc1d7)
#define PMA_PMD_GNRTL_STATUS_GP2_8_Adr                                                         (0xc1d8)
#define PMA_PMD_GNRTL_STATUS_GP2_9_Adr                                                         (0xc1d9)

/*-----------------------------------------------*/
/*   Defines for LINE_RX_PMA_DP Bank                    */
/*-----------------------------------------------*/
#define LINE_RX_PMA_DP_BaseAdr                                                                 (0xa000)
#define LINE_RX_PMA_DP_main_ctrl_Adr                                                           (0xa000)
#define LINE_RX_PMA_DP_dp_ctrl_Adr                                                             (0xa001)
#define LINE_RX_PMA_DP_latched_status_Adr                                                      (0xa010)
#define LINE_RX_PMA_DP_cl49_latched_status_Adr                                                 (0xa011)
#define LINE_RX_PMA_DP_cl82_ber_count_MSB_Adr                                                  (0xa012)
#define LINE_RX_PMA_DP_live_status_Adr                                                         (0xa013)

/*-----------------------------------------------*/
/*   Defines for LINE_TX_PMA_DP Bank                    */
/*-----------------------------------------------*/
#define LINE_TX_PMA_DP_BaseAdr                                                                 (0xac00)
#define LINE_TX_PMA_DP_soft_rst_Adr                                                            (0xac00)
#define LINE_TX_PMA_DP_ctrl_Adr                                                                (0xac01)
#define LINE_TX_PMA_DP_pf_status_Adr                                                           (0xac10)

/*-----------------------------------------------*/
/*   Defines for SYS_RX_PMA_DP Bank                    */
/*-----------------------------------------------*/
#define SYS_RX_PMA_DP_BaseAdr                                                                  (0xb000)
#define SYS_RX_PMA_DP_main_ctrl_Adr                                                            (0xb000)
#define SYS_RX_PMA_DP_dp_ctrl_Adr                                                              (0xb001)
#define SYS_RX_PMA_DP_latched_status_Adr                                                       (0xb010)
#define SYS_RX_PMA_DP_cl49_latched_status_Adr                                                  (0xa011)
#define SYS_RX_PMA_DP_cl82_ber_count_MSB_Adr                                                   (0xb012)
#define SYS_RX_PMA_DP_live_status_Adr                                                          (0xb013)

/*-----------------------------------------------*/
/*   Defines for SYS_TX_PMA_DP Bank                    */
/*-----------------------------------------------*/
#define SYS_TX_PMA_DP_BaseAdr                                                                  (0xbc00)
#define SYS_TX_PMA_DP_soft_rst_Adr                                                             (0xbc00)
#define SYS_TX_PMA_DP_ctrl_Adr                                                                 (0xbc01)
#define SYS_TX_PMA_DP_pf_status_Adr                                                            (0xbc10)

/*-----------------------------------------------*/
/*   Defines for DSC_C Bank                    */
/*-----------------------------------------------*/
#define DSC_C_BaseAdr                                                                          (0xd050)
#define DSC_C_cdr_control_2_Adr                                                                (0xd052)
#define DSC_C_rx_pi_control_Adr                                                                (0xd053)

/*-----------------------------------------------*/
/*   Defines for DSC_D Bank                      */
/*-----------------------------------------------*/
#define DSC_D_dsc_sm_ctrl_9_Adr                                                                (0xd069)

/*   Defines for TX_PI_RPTR Bank                    */
/*-----------------------------------------------*/
#define TX_PI_RPTR_BaseAdr                                                                     (0xd0a0)
#define TX_PI_RPTR_tx_pi_control_0_Adr                                                         (0xd0a0)
#define TX_PI_RPTR_tx_pi_control_5_Adr                                                         (0xd0a5)

/*-----------------------------------------------*/
/*   Defines for CL93n72_USER_TX Bank                    */
/*-----------------------------------------------*/
#define CL93n72_USER_TX_BaseAdr                                                                (0xd090)
#define CL93n72_USER_TX_cl93n72ut_control2_register_Adr                                        (0xd094)
#define CL93n72_USER_TX_cl93n72ut_control3_register_Adr                                        (0xd095)

/*-----------------------------------------------*/
/*   Defines for CKRST_CTRL_RPTR Bank                    */
/*-----------------------------------------------*/
#define CKRST_CTRL_RPTR_BaseAdr                                                                (0xd0b0)
#define CKRST_CTRL_RPTR_OSR_MODE_CONTROL_Adr                                                   (0xd0b0)
#define CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL_Adr                                 (0xd0b1)
#define CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_Adr                              (0xd0b2)
#define CKRST_CTRL_RPTR_LANE_RESET_N_PWRDN_PIN_KILL_CONTROL_Adr                                (0xd0b3)

/*-----------------------------------------------*/
/*   Defines for PLL_CAL_COM Bank                    */
/*-----------------------------------------------*/
#define PLL_CAL_COM_BaseAdr                                                                    (0xd140)
#define PLL_CAL_COM_CTL_0_Adr                                                                  (0xd140)
#define PLL_CAL_COM_CTL_1_Adr                                                                  (0xd141)
#define PLL_CAL_COM_CTL_2_Adr                                                                  (0xd142)
#define PLL_CAL_COM_CTL_5_Adr                                                                  (0xd145)
#define PLL_CAL_COM_CTL_7_Adr                                                                  (0xd147)
#define PLL_CAL_COM_CTL_STATUS_0_Adr                                                           (0xd148)

/*-----------------------------------------------*/
/*   Defines for AMS_COM Bank                    */
/*-----------------------------------------------*/
#define AMS_COM_BaseAdr                                                                        (0xd110)
#define AMS_COM_PLL_CONTROL_1_Adr                                                              (0xd111)
#define AMS_COM_PLL_CONTROL_3_Adr                                                              (0xd113)

/*-----------------------------------------------*/
/*   Defines for PATT_GEN_COM Bank                    */
/*-----------------------------------------------*/
#define PATT_GEN_COM_BaseAdr                                                                   (0xd120)
#define PATT_GEN_COM_patt_gen_seq_0_Adr                                                        (0xd120)
#define PATT_GEN_COM_patt_gen_seq_1_Adr                                                        (0xd121)
#define PATT_GEN_COM_patt_gen_seq_2_Adr                                                        (0xd122)
#define PATT_GEN_COM_patt_gen_seq_3_Adr                                                        (0xd123)
#define PATT_GEN_COM_patt_gen_seq_4_Adr                                                        (0xd124)
#define PATT_GEN_COM_patt_gen_seq_5_Adr                                                        (0xd125)
#define PATT_GEN_COM_patt_gen_seq_6_Adr                                                        (0xd126)
#define PATT_GEN_COM_patt_gen_seq_7_Adr                                                        (0xd127)
#define PATT_GEN_COM_patt_gen_seq_8_Adr                                                        (0xd128)
#define PATT_GEN_COM_patt_gen_seq_9_Adr                                                        (0xd129)
#define PATT_GEN_COM_patt_gen_seq_10_Adr                                                       (0xd12a)
#define PATT_GEN_COM_patt_gen_seq_11_Adr                                                       (0xd12b)
#define PATT_GEN_COM_patt_gen_seq_12_Adr                                                       (0xd12c)
#define PATT_GEN_COM_patt_gen_seq_13_Adr                                                       (0xd12d)
#define PATT_GEN_COM_patt_gen_seq_14_Adr                                                       (0xd12e)

/*-----------------------------------------------*/
/*   Defines for TX_FED Bank                    */
/*-----------------------------------------------*/
#define TX_FED_BaseAdr                                                                         (0xd130)
#define TX_FED_txfir_control1_Adr                                                              (0xd131)
#define TX_FED_txfir_control2_Adr                                                              (0xd132)
#define TX_FED_txfir_misc_control1_Adr                                                         (0xd139)

/*-----------------------------------------------*/
/*   Defines for TLB_RX Bank                    */
/*-----------------------------------------------*/
#define TLB_RX_BaseAdr                                                                         (0xd160)
#define TLB_RX_prbs_chk_cnt_config_Adr                                                         (0xd160)
#define TLB_RX_prbs_chk_config_Adr                                                             (0xd161)
#define TLB_RX_dig_lpbk_config_Adr                                                             (0xd162)
#define TLB_RX_tlb_rx_misc_config_Adr                                                          (0xd163)
#define TLB_RX_prbs_chk_lock_status_Adr                                                        (0xd169)
#define TLB_RX_prbs_chk_err_cnt_msb_status_Adr                                                 (0xd16a)
#define TLB_RX_prbs_chk_err_cnt_lsb_status_Adr                                                 (0xd16b)
#define TLB_RX_pmd_rx_lock_status_Adr                                                          (0xd16c)

/*-----------------------------------------------*/
/*   Defines for TLB_TX Bank                    */
/*-----------------------------------------------*/
#define TLB_TX_BaseAdr                                                                         (0xd170)
#define TLB_TX_patt_gen_config_Adr                                                             (0xd170)
#define TLB_TX_prbs_gen_config_Adr                                                             (0xd171)
#define TLB_TX_rmt_lpbk_config_Adr                                                             (0xd172)
#define TLB_TX_tlb_tx_misc_config_Adr                                                          (0xd173)

/*-----------------------------------------------*/
/*   Defines for IEEE_AN_BLK0 Bank                    */
/*-----------------------------------------------*/
#define IEEE_AN_BLK0_BaseAdr                                                                   (0x0000)
#define IEEE_AN_BLK0_an_control_register_Adr                                                   (0x0000)
#define IEEE_AN_BLK0_an_status_register_Adr                                                    (0x0001)
#define IEEE_AN_BLK0_an_identifier_register_0_Adr                                              (0x0002)
#define IEEE_AN_BLK0_an_identifier_register_1_Adr                                              (0x0003)
#define IEEE_AN_BLK0_an_devices_in_package_1_Adr                                               (0x0005)
#define IEEE_AN_BLK0_an_devices_in_package_2_Adr                                               (0x0006)
#define IEEE_AN_BLK0_an_oui_id0_register_Adr                                                   (0x000e)
#define IEEE_AN_BLK0_an_oui_id1_reg_Adr                                                        (0x000f)

/*-----------------------------------------------*/
/*   Defines for IEEE_AN_BLK1 Bank                    */
/*-----------------------------------------------*/
#define IEEE_AN_BLK1_BaseAdr                                                                   (0x0010)
#define IEEE_AN_BLK1_an_advertisement_1_register_Adr                                           (0x0010)
#define IEEE_AN_BLK1_an_advertisement_2_register_Adr                                           (0x0011)
#define IEEE_AN_BLK1_an_advertisement_3_register_Adr                                           (0x0012)
#define IEEE_AN_BLK1_an_lp_base_page_ability_1_reg_Adr                                         (0x0013)
#define IEEE_AN_BLK1_an_lp_base_page_ability_2_reg_Adr                                         (0x0014)
#define IEEE_AN_BLK1_an_lp_base_page_ability_3_reg_Adr                                         (0x0015)
#define IEEE_AN_BLK1_an_xnp_transmit_1_register_Adr                                            (0x0016)
#define IEEE_AN_BLK1_an_xnp_transmit_2_register_Adr                                            (0x0017)
#define IEEE_AN_BLK1_an_xnp_transmit_3_register_Adr                                            (0x0018)
#define IEEE_AN_BLK1_an_lp_xnp_transmit_1_register_Adr                                         (0x0019)
#define IEEE_AN_BLK1_an_lp_xnp_transmit_2_register_Adr                                         (0x001a)
#define IEEE_AN_BLK1_an_lp_xnp_transmit_3_register_Adr                                         (0x001b)

/*-----------------------------------------------*/
/*   Defines for IEEE_AN_BLK3 Bank                    */
/*-----------------------------------------------*/
#define IEEE_AN_BLK3_BaseAdr                                                                   (0x0030)
#define IEEE_AN_BLK3_backplane_ethernet_status_register_Adr                                    (0x0030)
#define IEEE_AN_BLK3_EEE_adv_Adr                                                               (0x003c)
#define IEEE_AN_BLK3_EEE_lp_adv_Adr                                                            (0x003d)

/*-----------------------------------------------*/
/*   Defines for DEV7_SLICE Bank                    */
/*-----------------------------------------------*/
#define DEV7_SLICE_BaseAdr                                                                     (0x0000)
#define DEV7_SLICE_slice_Adr                                                                   (0x8000)

#define LINE_FALCON_IF_soft_mode0_Adr                                                          (0xa400)
#define LINE_FALCON_IF_c_and_r_cntl_Adr                                                        (0xa401)
#define LINE_FALCON_IF_soft_mode1_Adr                                                          (0xa406)
#define SYS_FALCON_IF_soft_mode0_Adr                                                           (0xb400)
#define SYS_FALCON_IF_soft_mode1_Adr                                                           (0xb406)
#define SYS_FALCON_IF_c_and_r_cntl_Adr                                                         (0xb401)
#define FUR_MISC_CTRL_furia_qsfi_misc_Adr                                                      (0x8af1)
#define FUR_MISC_CTRL_DEBUG_CTRL_Adr                                                           (0x8af0)
#define FUR_PAD_CTRL_opttxenb_control_Adr                                                      (0x8a50)
#define FUR_PAD_CTRL_opttxenb_status_Adr                                                      (0x8a51)
#define FUR_PAD_CTRL_gpio_0_control_Adr                                                        (0x8a54)
#define FUR_PAD_CTRL_gpio_0_status_Adr                                                         (0x8a55)
#define FUR_PAD_CTRL_gpio_1_control_Adr                                                        (0x8a56)
#define FUR_PAD_CTRL_gpio_1_status_Adr                                                         (0x8a57)
#define FUR_PAD_CTRL_gpio_2_control_Adr                                                        (0x8a58)
#define FUR_PAD_CTRL_gpio_2_status_Adr                                                         (0x8a59)
#define FUR_PAD_CTRL_gpio_3_control_Adr                                                        (0x8a5a)
#define FUR_PAD_CTRL_gpio_3_status_Adr                                                         (0x8a5b)
#define FUR_PAD_CTRL_gpio_4_control_Adr                                                        (0x8a5c)
#define FUR_PAD_CTRL_gpio_4_status_Adr                                                         (0x8a5d)
#define FUR_PAD_CTRL_testpad_control_Adr                                                       (0x8a5e)
#define FUR_PAD_CTRL_testpad_status_Adr                                                        (0x8a5f)
#define FUR_MISC_CTRL_general_status_IO_Adr                                                    (0x8a70)
#define FUR_PAD_CTRL_ext_uc_rstb_control_Adr                                                   (0x8a4a)
#define FUR_PAD_CTRL_mod_absent_control_Adr                                                    (0x8a4c)
#define FUR_PAD_CTRL_mod_absent_status_Adr                                                     (0x8a4d)
#define FUR_PAD_CTRL_ext_intrb_control_Adr                                                     (0x8a4e)
#define FUR_PAD_CTRL_ext_intrb_status_Adr                                                      (0x8a4f)
#define FUR_PAD_CTRL_mod_rxlos_control_Adr                                                     (0x8a52)
#define FUR_PAD_CTRL_mod_rxlos_status_Adr                                                      (0x8a53)  
#define SIGDET_SIGDET_CTRL_1_Adr                                                               (0xd0e1) 
#endif /*  chip_address_defines__H */
