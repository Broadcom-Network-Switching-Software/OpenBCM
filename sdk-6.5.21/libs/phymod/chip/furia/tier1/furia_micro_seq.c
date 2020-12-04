/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/* 
 * Includes
 */
#include <phymod/phymod_system.h>
#include "furia_micro_seq.h"
#include "furia_cfg_seq.h"
#include "furia_regs_structs.h"
#include "furia_reg_access.h"
#include "furia_address_defines.h"
#include "furia_types.h"

/*
 *  Defines
 */
#define FURIA_FW_DLOAD_RETRY_CNT 50000 
#define FURIA_M0_EEPROM_PAGE_SIZE 64
/* 
 *  Types
 */

/*
 *  Macros
 */

/*
 *  Global Variables
 */

/*
 *  Functions
 */

/**   Wait master message out 
 *    This function is to ensure whether master has sent the previous message
 *    out successfully 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param exp_message        Expected message specified by user  
 *    @param poll_time          Poll interval 
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _furia_wait_mst_msgout(const phymod_access_t *pa,
                            FURIA_MSGOUT_E exp_message,
                            int poll_time)
{
    int retry_count = FURIA_FW_DLOAD_RETRY_CNT;
    FURIA_MSGOUT_E msgout = 0;
    FUR_GEN_CNTRLS_MSGOUT_t msgout_t;
    
    do {
        /* Read general control msg out  Register */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_msgout_Adr,\
                                    &msgout_t.data));
        msgout = (FURIA_MSGOUT_E) msgout_t.fields.msgout_val;
        /* wait before reading again */
        if (poll_time != 0) {
            /* sleep for specified seconds*/
            PHYMOD_USLEEP(poll_time/FURIA_FW_DLOAD_RETRY_CNT);
        }
        retry_count--;
    } while ((msgout != exp_message) &&  retry_count);
   
    if(!retry_count) {
        if(exp_message == MSGOUT_PRGRM_DONE) { 
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Fusing Firmware failed")));
        } else {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
        }
    } 
    
    return PHYMOD_E_NONE; 
}
/**   Download and Fuse firmware 
 *    This function is used to download the firmware through I2C/MDIO
 *    and fuse it to SPI EEPROM if prg_eeprom flag is set 
 *
 *    @param pa                 Pointer to phymod access structure 
 *    @param new_fw             Pointer to firmware array 
 *    @param fw_length          Length of the firmware array 
 *    @param prg_eeprom         Flag used to program EEPROM
 *
 *    @return num_bytes         number of bytes successfully downloaded
 */
int furia_download_prog_eeprom(const phymod_access_t *pa,
                                 uint8_t *new_fw,
                                 uint32_t fw_length,
                                 uint8_t prg_eeprom)
{
    uint16_t data16 = 0;
    uint16_t num_bytes = 0;
    uint16_t num_words = 0;
    uint16_t j = 0;
    uint16_t ser_boot_busy = 0; 
    FUR_GEN_CNTRLS_MSGIN_t msgin;
    FUR_GEN_CNTRLS_GEN_CONTROL2_t gen_ctrl2;
    FUR_GEN_CNTRLS_GEN_CONTROL3_t gen_ctrl3;
    FUR_GEN_CNTRLS_BOOT_t gen_ctrl_boot;
    FUR_GEN_CNTRLS_SPI_CODE_LOAD_EN_t spi_code_ld_en;
    FUR_MICRO_BOOT_BOOT_POR_t micro_boot_por;
    FUR_GEN_CNTRLS_MST_RUNNING_CHKSUM_t mst_running_chksum;
    FUR_GEN_CNTRLS_SLV_RUNNING_CHKSUM_t slv_running_chksum;
    FUR_GEN_CNTRLS_MST_RUNNING_BYTE_CNT_t mst_running_byte_cnt;
    FUR_GEN_CNTRLS_SLV_RUNNING_BYTE_CNT_t slv_running_byte_cnt;
    FUR_GEN_CNTRLS_GEN_CONTROL1_t gen_ctrl1;
    FUR_GEN_CNTRLS_gpreg14_t gpreg14;
    FUR_GEN_CNTRLS_gpreg15_t gpreg15;
    uint16_t m_chksum = 0;
    uint16_t s_chksum = 0;
    uint16_t m_byte_cnt = 0;  
    uint16_t s_byte_cnt = 0;  
    uint16_t check_sum = 0;
    int retry_count = FURIA_FW_DLOAD_RETRY_CNT;
    int eeprom_num_of_pages = 0;
    phymod_tx_t tx_params;

    PHYMOD_MEMSET(&msgin, 0, sizeof(FUR_GEN_CNTRLS_MSGIN_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(FUR_GEN_CNTRLS_GEN_CONTROL2_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(FUR_GEN_CNTRLS_GEN_CONTROL3_t));
    PHYMOD_MEMSET(&gen_ctrl_boot, 0, sizeof(FUR_GEN_CNTRLS_BOOT_t));
    PHYMOD_MEMSET(&spi_code_ld_en, 0, sizeof(FUR_GEN_CNTRLS_SPI_CODE_LOAD_EN_t));
    PHYMOD_MEMSET(&micro_boot_por, 0, sizeof(FUR_MICRO_BOOT_BOOT_POR_t));
    PHYMOD_MEMSET(&mst_running_chksum, 0, sizeof(FUR_GEN_CNTRLS_MST_RUNNING_CHKSUM_t));
    PHYMOD_MEMSET(&slv_running_chksum, 0, sizeof(FUR_GEN_CNTRLS_SLV_RUNNING_CHKSUM_t));
    PHYMOD_MEMSET(&mst_running_byte_cnt, 0, sizeof(FUR_GEN_CNTRLS_MST_RUNNING_BYTE_CNT_t));
    PHYMOD_MEMSET(&slv_running_byte_cnt, 0, sizeof(FUR_GEN_CNTRLS_SLV_RUNNING_BYTE_CNT_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0 , sizeof(FUR_GEN_CNTRLS_GEN_CONTROL1_t));
    PHYMOD_MEMSET(&gpreg14, 0 , sizeof(FUR_GEN_CNTRLS_gpreg14_t));
    PHYMOD_MEMSET(&gpreg15, 0 , sizeof(FUR_GEN_CNTRLS_gpreg15_t));
    PHYMOD_MEMSET(&tx_params, 0 , sizeof(phymod_tx_t));

    /* Skip the firmware download 
     *  i) if serboot bin is low and mst_download, slv_dwld_done bits are set
     */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MICRO_BOOT_boot_por_Adr,\
                                &micro_boot_por.data));

    if (!prg_eeprom) {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_mst_running_byte_cnt_Adr,\
                                    &mst_running_byte_cnt.data));
        m_byte_cnt = mst_running_byte_cnt.fields.mst_running_byte_cnt_val;

        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_slv_running_byte_cnt_Adr,\
                                    &slv_running_byte_cnt.data));
        s_byte_cnt = slv_running_byte_cnt.fields.slv_running_byte_cnt_val;

        if (((m_byte_cnt == fw_length) || 
             (s_byte_cnt == fw_length))&&
            (micro_boot_por.fields.mst_dwld_done) &&
            (micro_boot_por.fields.slv_dwld_done)) {
            return FURIA_FW_ALREADY_DOWNLOADED;
        }
    }

    /* Set Tx driver current, FIR main/pre/post default values 
     *  Default values are 
     *  driver current = 0xf
     *  TX FIR main = 0x3c
     *  TX FIR pre = 0x0
     *  TX FIR post = 0x0
     */ 

    PHYMOD_IF_ERR_RETURN
         (furia_tx_get(pa, &tx_params)); 
    tx_params.amp = 0xf;
    tx_params.pre = 0x0;
    tx_params.post = 0x0;
    tx_params.main = 0x3c;
    PHYMOD_IF_ERR_RETURN
         (furia_tx_set(pa, &tx_params));  
    /* Do chip reset */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control1_Adr,\
                                &gen_ctrl1.data));
    gen_ctrl1.fields.resetb = 0;

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control1_Adr,\
                                 gen_ctrl1.data));

    /* Perform master reset */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control2_Adr,\
                                &gen_ctrl2.data));
    gen_ctrl2.fields.m0_mstr_rstb = 0;
    gen_ctrl2.fields.m0_mstr_ucp_rstb = 0; 

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control2_Adr,\
                                 gen_ctrl2.data));

    /* Wait for the serboot busy bit to become zero */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_boot_Adr,\
                                &gen_ctrl_boot.data));

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its 
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_boot_Adr,\
                                    &gen_ctrl_boot.data));
        ser_boot_busy = gen_ctrl_boot.fields.serboot_busy;
        PHYMOD_USLEEP(500);
        retry_count--;
    } while((ser_boot_busy != 0) && (retry_count));

    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
    }
  
    /* Program master enable, slave enable, broadcast enable bits */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_spi_code_load_en_Adr,\
                                &spi_code_ld_en.data));

    spi_code_ld_en.fields.code_broadcast_en = 1;
    spi_code_ld_en.fields.slave_code_download_en = 1; 
    spi_code_ld_en.fields.master_code_download_en = 1; 

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_spi_code_load_en_Adr,\
                                 spi_code_ld_en.data));

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_spi_code_load_en_Adr,\
                                &spi_code_ld_en.data));
    if ((spi_code_ld_en.fields.code_broadcast_en != 1) &&
        (spi_code_ld_en.fields.master_code_download_en != 1) &&
        (spi_code_ld_en.fields.slave_code_download_en != 1)) {
        return PHYMOD_E_INTERNAL;
    }
    /* Select SPI speed */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control3_Adr,\
                                &gen_ctrl3.data));
    gen_ctrl3.fields.ucspi_slow = 1; 
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control3_Adr,\
                                 gen_ctrl3.data));
    if(prg_eeprom) {
        /* Configuration for programming SPI EEPROM */
        eeprom_num_of_pages = fw_length/FURIA_M0_EEPROM_PAGE_SIZE;
        /*program GPReg 14 with eeprom start page */
        gpreg14.data = 0;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa, FUR_GEN_CNTRLS_gpreg14_Adr, gpreg14.data));
        /*program GPReg 15 with eeprom number of pages */
        gpreg15.data = eeprom_num_of_pages;
        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,
                                     FUR_GEN_CNTRLS_gpreg15_Adr,
                                     gpreg15.data));
   
        /* Assert Reset by writing zero to spi_rstb, spi2x_rstb */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gen_control2_Adr,\
                                    &gen_ctrl2.data));
        gen_ctrl2.fields.spi_rstb = 0; 
        gen_ctrl2.fields.spi2x_rstb = 0; 

        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_gen_control2_Adr,\
                                     gen_ctrl2.data));

        /* Deassert Reset by setting spi_rstb, spi2x_rstb bits */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gen_control2_Adr,\
                                    &gen_ctrl2.data));
        gen_ctrl2.fields.spi_rstb = 1; 
        gen_ctrl2.fields.spi2x_rstb = 1; 

        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_gen_control2_Adr,\
                                     gen_ctrl2.data));
    }
    /* Force master download done, slave download done bit to 0 
     * set external boot (serboot) to 1 */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MICRO_BOOT_boot_por_Adr,\
                                &micro_boot_por.data));

    micro_boot_por.fields.mst_dwld_done = 0;
    micro_boot_por.fields.slv_dwld_done = 0;
    micro_boot_por.fields.serboot = 1;
    micro_boot_por.fields.spi_port_used = 0;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_MICRO_BOOT_boot_por_Adr,\
                                 micro_boot_por.data));


    /* Bring back m0 master to normal state*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_gen_control2_Adr,\
                                &gen_ctrl2.data));

    gen_ctrl2.fields.m0_mstr_rstb = 1;
    gen_ctrl2.fields.m0_mstr_ucp_rstb = 1; 

    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_gen_control2_Adr,\
                                 gen_ctrl2.data));

    /* MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_count = FURIA_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_boot_Adr,\
                                    &gen_ctrl_boot.data));

        ser_boot_busy = gen_ctrl_boot.fields.serboot_busy;
        PHYMOD_USLEEP(500);
        retry_count--;
    } while ((ser_boot_busy == 0) && (retry_count));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
    }
    /* Program master and slave boot address */ 
    PHYMOD_IF_ERR_RETURN
        (_furia_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

    msgin.fields.msgin_val = 0x0000;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_msgin_Adr,\
                                 msgin.data));

    PHYMOD_IF_ERR_RETURN
        (_furia_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    msgin.fields.msgin_val = 0x0000;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_msgin_Adr,\
                                 msgin.data));
   
    PHYMOD_IF_ERR_RETURN
        (_furia_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

    /* Calculate the number of words */
    num_words = (fw_length) / 2;

    /* Update message in value field with word length */
    msgin.fields.msgin_val = num_words;
    PHYMOD_IF_ERR_RETURN
        (WRITE_FURIA_PMA_PMD_REG(pa,\
                                 FUR_GEN_CNTRLS_msgin_Adr,\
                                 msgin.data));

    num_bytes = fw_length;

    for (j = 0; j < num_bytes; j += 2) {
        /* Wait MSGOUT_NEXT from M0 */
        PHYMOD_IF_ERR_RETURN
            (_furia_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

        data16 = (new_fw[j + 1] << 8) | new_fw[j];
        check_sum ^= new_fw[j] ^ new_fw[j + 1];
        /* Send next word */
        msgin.fields.msgin_val = data16;

        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_msgin_Adr,\
                                     msgin.data));
    }

    check_sum = check_sum ? check_sum : 0x600D;
    /*Check whether firmware is downloaded for both master and slave micro*/
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_MICRO_BOOT_boot_por_Adr,\
                                &micro_boot_por.data));

    if((!micro_boot_por.fields.mst_dwld_done) || (!micro_boot_por.fields.slv_dwld_done)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
    } 

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_mst_running_byte_cnt_Adr,\
                                &mst_running_byte_cnt.data));
    m_byte_cnt = mst_running_byte_cnt.fields.mst_running_byte_cnt_val;

    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_slv_running_byte_cnt_Adr,\
                                &slv_running_byte_cnt.data));
    s_byte_cnt = slv_running_byte_cnt.fields.slv_running_byte_cnt_val;

    if ((m_byte_cnt != num_bytes) || (s_byte_cnt != num_bytes)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download incomplete with byte count mismatch")));
    }

    /* Compare the checksum for data integrity */ 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_mst_running_chksum_Adr,\
                                &mst_running_chksum.data));
    m_chksum = mst_running_chksum.fields.mst_running_chksum_val;
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_slv_running_chksum_Adr,\
                                &slv_running_chksum.data));
    s_chksum = slv_running_chksum.fields.slv_running_chksum_val;
 
    if((m_chksum != check_sum) || (s_chksum != check_sum)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed with checksum mismatch")));
    }

    /* To Check whether the firmware download through MDIO is done */
    PHYMOD_IF_ERR_RETURN
        (_furia_wait_mst_msgout(pa, MSGOUT_DWNLD_DONE, 0));
    /* Wait for programming EEPROM */
    if(prg_eeprom) {
        PHYMOD_IF_ERR_RETURN
            (_furia_wait_mst_msgout(pa, MSGOUT_PRGRM_DONE, 10000000));
        /* Assert Reset by writing zero to spi_rstb, spi2x_rstb */
        PHYMOD_IF_ERR_RETURN
            (READ_FURIA_PMA_PMD_REG(pa,\
                                    FUR_GEN_CNTRLS_gen_control2_Adr,\
                                    &gen_ctrl2.data));
        gen_ctrl2.fields.spi_rstb = 0;
        gen_ctrl2.fields.spi2x_rstb = 0;

        PHYMOD_IF_ERR_RETURN
            (WRITE_FURIA_PMA_PMD_REG(pa,\
                                     FUR_GEN_CNTRLS_gen_control2_Adr,\
                                     gen_ctrl2.data));
    }

    return m_byte_cnt;
}

int furia_firmware_info_get(const phymod_access_t *pa, phymod_core_firmware_info_t *fw_info)
{
    FUR_GEN_CNTRLS_FIRMWARE_VERSION_t firmware_version;
    FUR_GEN_CNTRLS_MST_RUNNING_CHKSUM_t mst_running_chksum;
    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_firmware_version_Adr,\
                                &firmware_version.data));
    fw_info->fw_version = firmware_version.data; 
    PHYMOD_IF_ERR_RETURN
        (READ_FURIA_PMA_PMD_REG(pa,\
                                FUR_GEN_CNTRLS_mst_running_chksum_Adr,\
                                &mst_running_chksum.data));
    fw_info->fw_crc = mst_running_chksum.data; 
    return PHYMOD_E_NONE;
}
