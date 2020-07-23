/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
* 
*/

/*
 *         
 * 
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include <phymod/phymod_diagnostics.h>
#include <phymod/phymod_reg.h>
#include "bcmi_dino_defs.h"
#include "dino_cfg_seq.h"
#include "dino_serdes/common/srds_api_enum.h"
#include "dino_serdes/merlin_dino_src/merlin_dino_functions.h"
#include "dino_serdes/falcon2_dino_src/falcon2_dino_functions.h"

/* uController's firmware */
extern unsigned char dino_ucode_Dino[];
extern unsigned short dino_ucode_Dino_len;

int dino_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev) 
{

    BCMI_DINO_CHIP_IDr_t cid;
    BCMI_DINO_CHIP_REVISIONr_t rid;

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CHIP_IDr(pa, &cid));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_CHIP_REVISIONr(pa, &rid));

    *chipid = (BCMI_DINO_CHIP_REVISIONr_CHIP_ID_19_16f_GET(rid) << 16 ) |
               BCMI_DINO_CHIP_IDr_GET(cid);
    *rev = BCMI_DINO_CHIP_REVISIONr_CHIP_REVf_GET(rid);

    PHYMOD_DEBUG_VERBOSE(("CHIP ID: %x REV:%x\n", *chipid, *rev));

    return PHYMOD_E_NONE;
}

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
int _dino_wait_mst_msgout(const phymod_access_t *pa,
                          DINO_MSGOUT_E exp_message,
                          int poll_time)
{
    int retry_count = DINO_FW_DLOAD_RETRY_CNT;
    DINO_MSGOUT_E msgout = MSGOUT_DONTCARE;
    BCMI_DINO_MST_MSGOUTr_t msgout_t; 

    do {
        /* Read general control msg out  Register */
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_MST_MSGOUTr(pa,&msgout_t));
        msgout = BCMI_DINO_MST_MSGOUTr_MST_MSGOUT_VALf_GET(msgout_t); 
        /* wait before reading again */
        if (poll_time != 0) {
            /* sleep for specified seconds*/
            PHYMOD_SLEEP(poll_time);
        }
    } while ((msgout != exp_message) &&  --retry_count);
    
    if (!retry_count) {
        if (exp_message == MSGOUT_PRGRM_DONE) { 
            PHYMOD_DEBUG_VERBOSE(("MSG OUT:%x \n",msgout));
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Fusing Firmware failed")));
        } else {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
        }
    }

    return PHYMOD_E_NONE; 
}

int _dino_fw_enable(const phymod_access_t* pa)
{
    uint16_t retry_cnt = DINO_FW_ENABLE_RETRY_CNT;
    uint16_t data = 0;
    BCMI_DINO_FIRMWARE_ENr_t fw_en;
    PHYMOD_MEMSET(&fw_en, 0, sizeof(BCMI_DINO_FIRMWARE_ENr_t));

    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
        data = BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                               (_PHYMOD_MSG("mode config failed, micro controller is busy..")));
    }
    retry_cnt = DINO_FW_ENABLE_RETRY_CNT;
    /* Set FW_ENABLE = 1 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
    /* coverity[operator_confusion] */
    BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIRMWARE_ENr(pa, fw_en));
   do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_FIRMWARE_ENr(pa, &fw_en));
        data = BCMI_DINO_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (retry_cnt--));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("mode config failed, micro controller is busy..")));
    }

    return PHYMOD_E_NONE;
}
int _dino_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    BCMI_DINO_GEN_CTL1r_t gen_ctrl;
    PHYMOD_MEMSET(&gen_ctrl, 0, sizeof(BCMI_DINO_GEN_CTL1r_t));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl));

    if (reset_mode == phymodResetModeHard) {
        /*Reset for chip*/
        BCMI_DINO_GEN_CTL1r_RESETBf_SET(gen_ctrl, 0);    
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl));
        /*Above chip reset resets the cores also !*/
        PHYMOD_USLEEP(10000);
    } else if (reset_mode == phymodResetModeSoft) {
        /*Reset registers*/
        BCMI_DINO_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl));
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
int dino_download_prog_eeprom(const phymod_access_t *pa,
                              uint8_t *new_fw,
                              uint32_t fw_length,
                              uint8_t prg_eeprom)
{
    uint16_t num_bytes = 0;
    uint16_t j = 0;
    uint16_t data = 0;
    uint16_t data1 = 0;
    uint16_t data2 = 0;
    uint16_t check_sum = 0;
    int retry_count = DINO_FW_DLOAD_RETRY_CNT;
    FIRMWARE_VERSIONr_t  firmware_version;
    BCMI_DINO_GEN_CTL2r_t gen_ctrl2;   
    BCMI_DINO_GEN_CTL3r_t gen_ctrl3;
    BCMI_DINO_UC_BOOT_PORr_t   boot_por;
    BCMI_DINO_FIRMWARE_VERSIONr_t fw_ver;
    BCMI_DINO_BOOTr_t boot;
    BCMI_DINO_MST_MSGINr_t msg_in;
    BCMI_DINO_SPI_CODE_LOAD_ENr_t spi_code_load_en;
    BCMI_DINO_DWNLD_15r_t dwnld_15; 
    BCMI_DINO_DWNLD_16r_t dwnld_16; 
    BCMI_DINO_DWNLD_17r_t dwnld_17; 

    PHYMOD_MEMSET(&firmware_version, 0, sizeof(FIRMWARE_VERSIONr_t));
    PHYMOD_MEMSET(&boot_por, 0, sizeof(BCMI_DINO_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&msg_in, 0, sizeof(BCMI_DINO_MST_MSGINr_t));
    PHYMOD_MEMSET(&spi_code_load_en, 0, sizeof(BCMI_DINO_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(BCMI_DINO_GEN_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_DINO_GEN_CTL3r_t));
    PHYMOD_MEMSET(&boot, 0, sizeof(BCMI_DINO_BOOTr_t));
    PHYMOD_MEMSET(&fw_ver, 0, sizeof(BCMI_DINO_FIRMWARE_VERSIONr_t));
    PHYMOD_MEMSET(&spi_code_load_en, 0, sizeof(BCMI_DINO_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&dwnld_15, 0, sizeof(BCMI_DINO_DWNLD_15r_t));
    PHYMOD_MEMSET(&dwnld_16, 0, sizeof(BCMI_DINO_DWNLD_16r_t));
    PHYMOD_MEMSET(&dwnld_17, 0, sizeof(BCMI_DINO_DWNLD_17r_t));

    /* STEP 1: Put Master under Reset */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    /*resetting M0 micro*/
    BCMI_DINO_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2, 0);
    /*resetting M0 micro perip*/
    BCMI_DINO_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));

    /* STEP 2: Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its 
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_BOOTr(pa, &boot));
        data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
        PHYMOD_USLEEP(500);
    } while((data1 != 0) && (--retry_count));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
    }
 
    if(prg_eeprom) {
        /* BCM82332 and BCM82793 do not have EEPROM. This feature will be added in future for BCM82795 */ 
    }

    /* STEP 3: Program master enable, slave enable, broadcast enable bits */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_SPI_CODE_LOAD_ENr(pa, &spi_code_load_en));
    BCMI_DINO_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load_en, 1);
    BCMI_DINO_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load_en, 0x3f);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_SPI_CODE_LOAD_ENr(pa, spi_code_load_en));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_SPI_CODE_LOAD_ENr(pa, &spi_code_load_en));
    if ((BCMI_DINO_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_GET(spi_code_load_en) != 0x3f) ||
        !(BCMI_DINO_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_GET(spi_code_load_en))) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR: BROADCAST ENABLE IS NOT SET")));
    }

    /* STEP 4: Force master download done, slave download done bit to 0 
     * set external boot (serboot) to 1 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
    /* Force Master New Download */
    BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_SET(boot_por, 0);
    /* Force Slave New Download */
    BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_SET(boot_por, 0);
    /*set to 0 for mdio default download on reset*/
    BCMI_DINO_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por, 0);
    BCMI_DINO_UC_BOOT_PORr_SLV_RST_ENf_SET(boot_por, 0);
    BCMI_DINO_UC_BOOT_PORr_SERBOOTf_SET(boot_por, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_UC_BOOT_PORr(pa, boot_por));

    /* STEP 5: RELEASE Master under Reset */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    BCMI_DINO_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2, 1); 
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL2r(pa, &gen_ctrl2));
    BCMI_DINO_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_GEN_CTL2r(pa, gen_ctrl2));

    /* STEP 6: MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_BOOTr(pa, &boot));
        data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
        PHYMOD_USLEEP(500);
    } while ((data1 != 1) && (--retry_count));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT NOT SET")));
        return PHYMOD_E_FAIL;
    }

    PHYMOD_IF_ERR_RETURN
        (_dino_wait_mst_msgout(pa, MSGOUT_FLASH, 0));
    /* coverity[operator_confusion] */
    BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
    PHYMOD_IF_ERR_RETURN(
        _dino_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    /* STEP 7: Program master and slave boot address */ 
    /* coverity[operator_confusion] */
    BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));

    num_bytes = fw_length;
    /* STEP 8: Download the 32 byte header first */
    for (j = 0; j <= DINO_FW_HEADER_SIZE; j += 2) {
        /*sending next ucode data*/
        data = (new_fw[j + 1] << 8) | new_fw[j];
        check_sum ^= new_fw[j] ^ new_fw[j + 1];
        /* Send next word */
        BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, data);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
    }
    PHYMOD_USLEEP(2100);
    /* STEP 9: Download the remaining portion of the firmware */
    for (j = DINO_FW_HEADER_SIZE + 2; j < num_bytes; j += 2) {
        /*sending next ucode data*/
        data = (new_fw[j + 1] << 8) | new_fw[j];
        check_sum ^= new_fw[j] ^ new_fw[j + 1];
        /* Send next word */
        BCMI_DINO_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, data);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_MST_MSGINr(pa, msg_in));
        if ((j % 32) == 0) {
            PHYMOD_USLEEP(2100);
        }
    }
    /* STEP 10: i) Verify whether the firmware download is done for master and slaves */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
        /* check download_done flags*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
        if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por) == 1) &&
           (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) == 0x3f)){
            PHYMOD_DEBUG_VERBOSE(("Firmware Download Done\n"));
            break;
        }
    } while (--retry_count);

    if (!retry_count) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
    }

    /* STEP 11: ii) Verify serboot busy and serboot done once bits */
    retry_count = DINO_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_BOOTr(pa, &boot));
        data1 = BCMI_DINO_BOOTr_SERBOOT_BUSYf_GET(boot); 
        data2 = BCMI_DINO_BOOTr_SERBOOT_DONE_ONCEf_GET(boot); 
        PHYMOD_USLEEP(500);
    } while (((data1 != 0) && (data2 == 1)) && (--retry_count));
    if (retry_count == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_15r(pa, &dwnld_15));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_16r(pa, &dwnld_16));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DWNLD_17r(pa, &dwnld_17));

    if ((BCMI_DINO_DWNLD_15r_DOWNLOAD_15f_GET(dwnld_15) != 0x600D) ||
        (BCMI_DINO_DWNLD_16r_DOWNLOAD_16f_GET(dwnld_16) != 0x600D) ||
        (BCMI_DINO_DWNLD_17r_DOWNLOAD_17f_GET(dwnld_17) != 0x600D)) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                               (_PHYMOD_MSG("chksum Failed")));
    }
    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
    PHYMOD_DEBUG_VERBOSE(("Firmware Version: 0x%x\n", firmware_version.v[0]));

    return PHYMOD_E_NONE;
}

int _dino_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config)
{
    int rv = PHYMOD_E_NONE; 
    BCMI_DINO_DWNLD_15r_t dwnld_15; 
    BCMI_DINO_DWNLD_16r_t dwnld_16; 
    BCMI_DINO_DWNLD_17r_t dwnld_17; 
    BCMI_DINO_UC_BOOT_PORr_t   boot_por;
    BCMI_DINO_FIRMWARE_VERSIONr_t firmware_version;
    uint16_t new_firmware_version = 0;
    const phymod_access_t *pa = &core->access;
    PHYMOD_MEMSET(&dwnld_15, 0, sizeof(BCMI_DINO_DWNLD_15r_t));
    PHYMOD_MEMSET(&dwnld_16, 0, sizeof(BCMI_DINO_DWNLD_16r_t));
    PHYMOD_MEMSET(&dwnld_17, 0, sizeof(BCMI_DINO_DWNLD_17r_t));
    PHYMOD_MEMSET(&boot_por, 0, sizeof(BCMI_DINO_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&firmware_version, 0, sizeof(BCMI_DINO_FIRMWARE_VERSIONr_t));

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
    new_firmware_version = (((dino_ucode_Dino[12] & 0xff) << 8) | (dino_ucode_Dino[13] & 0xff));
    switch(init_config->firmware_load_method) {
        case phymodFirmwareLoadMethodInternal:
            /* This case is added for unicast */
            /*  Broadcast way of FW download sequence
             *  1.  Reset core for all cores
             *  2.  Enable the broadcast for all the cores
             *  3.  Download FW for one core in MDIO bus
             *  4.  Disable the broadcast and Verify the FW on all cores
             */
            if (PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_GET(init_config)){
                if (PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_GET(init_config)) {
                    /* Do chip hard reset and Resetting the Cores */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_core_reset_set(pa, phymodResetModeHard, phymodResetDirectionInOut));
                } else if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 1));
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    PHYMOD_DEBUG_VERBOSE(("Starting Firmware download through MDIO, it takes few seconds...\n"));
                    rv = dino_download_prog_eeprom(pa,
                                                   dino_ucode_Dino,
                                                   dino_ucode_Dino_len,
                                                   0);
                    if ((rv != PHYMOD_E_NONE) &&
                        (rv != DINO_FW_ALREADY_DOWNLOADED)) {
                        PHYMOD_RETURN_WITH_ERR(rv,
                            (_PHYMOD_MSG("firmware download failed")));
                    } else {
                        if (rv == DINO_FW_ALREADY_DOWNLOADED) {
                            rv = PHYMOD_E_NONE;
                        }
                        return rv;
                    }
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                    /* check download_done flags again */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
                    if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por) != 1) &&
                       (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) != 0x3f)){
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                    }
                    /* Read the firmware version */
                    PHYMOD_IF_ERR_RETURN(
                        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
                    if (firmware_version.v[0] == 0x1) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_15r(pa, &dwnld_15));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_16r(pa, &dwnld_16));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_17r(pa, &dwnld_17));
                    if ((BCMI_DINO_DWNLD_15r_DOWNLOAD_15f_GET(dwnld_15) != 0x600D) ||
                        (BCMI_DINO_DWNLD_16r_DOWNLOAD_16f_GET(dwnld_16) != 0x600D) ||
                        (BCMI_DINO_DWNLD_17r_DOWNLOAD_17f_GET(dwnld_17) != 0x600D)) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                            (_PHYMOD_MSG("chksum Failed")));
                    }
                } else if (PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)) {
                    /* disable broadcast */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 0));
                    PHYMOD_DIAG_OUT(("Firmware download success\n"));
                }
            } else if(PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config) && new_firmware_version != firmware_version.v[0]){
                if (PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_GET(init_config)) {
                    /* Do chip hard reset and Resetting the Cores */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_core_reset_set(pa, phymodResetModeHard, phymodResetDirectionInOut));
                } else if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 1));
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    PHYMOD_DEBUG_VERBOSE(("Starting Firmware download through MDIO, it takes few seconds...\n"));
                    rv = dino_download_prog_eeprom(pa,
                                                   dino_ucode_Dino,
                                                   dino_ucode_Dino_len,
                                                   0);
                    if ((rv != PHYMOD_E_NONE) &&
                        (rv != DINO_FW_ALREADY_DOWNLOADED)) {
                        PHYMOD_RETURN_WITH_ERR(rv,
                            (_PHYMOD_MSG("firmware download failed")));
                    } else {
                        if (rv == DINO_FW_ALREADY_DOWNLOADED) {
                            rv = PHYMOD_E_NONE;
                        }
                        return rv;
                    }
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                    /* check download_done flags again */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
                    if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por) != 1) &&
                       (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) != 0x3f)){
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                    }
                    /* Read the firmware version */
                    PHYMOD_IF_ERR_RETURN(
                        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
                    if (firmware_version.v[0] == 0x1) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_15r(pa, &dwnld_15));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_16r(pa, &dwnld_16));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_17r(pa, &dwnld_17));
                    if ((BCMI_DINO_DWNLD_15r_DOWNLOAD_15f_GET(dwnld_15) != 0x600D) ||
                        (BCMI_DINO_DWNLD_16r_DOWNLOAD_16f_GET(dwnld_16) != 0x600D) ||
                        (BCMI_DINO_DWNLD_17r_DOWNLOAD_17f_GET(dwnld_17) != 0x600D)) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                            (_PHYMOD_MSG("chksum Failed")));
                    }
                } else if (PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)) {
                    PHYMOD_DIAG_OUT(("Firmware download success\n"));
                    /* disable broadcast */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 0));
                }
            } else if(firmware_version.v[0] == 1){
                if (PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_GET(init_config)) {
                    /* Do chip hard reset and Resetting the Cores */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_core_reset_set(pa, phymodResetModeHard, phymodResetDirectionInOut));
                } else if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 1));
                } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                    PHYMOD_DEBUG_VERBOSE(("Starting Firmware download through MDIO, it takes few seconds...\n"));
                    rv = dino_download_prog_eeprom(pa,
                                                   dino_ucode_Dino,
                                                   dino_ucode_Dino_len,
                                                   0);
                    if ((rv != PHYMOD_E_NONE) &&
                        (rv != DINO_FW_ALREADY_DOWNLOADED)) {
                        PHYMOD_RETURN_WITH_ERR(rv,
                            (_PHYMOD_MSG("firmware download failed")));
                    } else {
                        if (rv == DINO_FW_ALREADY_DOWNLOADED) {
                            rv = PHYMOD_E_NONE;
                        }
                        return rv;
                    }
                } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                    /* check download_done flags again */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_UC_BOOT_PORr(pa, &boot_por));
                    if((BCMI_DINO_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por) != 1) &&
                       (BCMI_DINO_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) != 0x3f)){
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                    }
                    /* Read the firmware version */
                    PHYMOD_IF_ERR_RETURN(
                        READ_FIRMWARE_VERSIONr(pa, &firmware_version));
                    if (firmware_version.v[0] == 0x1) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                            (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_15r(pa, &dwnld_15));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_16r(pa, &dwnld_16));
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_DWNLD_17r(pa, &dwnld_17));
                    if ((BCMI_DINO_DWNLD_15r_DOWNLOAD_15f_GET(dwnld_15) != 0x600D) ||
                        (BCMI_DINO_DWNLD_16r_DOWNLOAD_16f_GET(dwnld_16) != 0x600D) ||
                        (BCMI_DINO_DWNLD_17r_DOWNLOAD_17f_GET(dwnld_17) != 0x600D)) {
                        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL, 
                            (_PHYMOD_MSG("chksum Failed")));
                    }
                } else if (PHYMOD_CORE_INIT_F_FW_LOAD_END_GET(init_config)) {
                    /* disable broadcast */
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 0));
                    PHYMOD_DIAG_OUT(("Firmware download success\n"));
                }
            } else {
                if (firmware_version.v[0] != 0) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_bcast_enable_set(core, 0));
                }
            }
        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;        
        case phymodFirmwareLoadMethodNone:
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            return PHYMOD_E_UNAVAIL;
        break;
        default:
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                                   (_PHYMOD_MSG("illegal fw load method")));
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_cfg_fw_ull_dp(core, init_config->op_datapath));

    return rv;
}

int _dino_set_slice_reg (const phymod_access_t* pa, DINO_SLICE_OP_TYPE slice_op_type, uint16_t if_side, uint16_t lane)
{
    BCMI_DINO_SLICEr_t slice_reg;
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    BCMI_DINO_DECD_MODE_STS1r_t mode_sts1;
    BCMI_DINO_DECD_MODE_STS2r_t mode_sts2;
    BCMI_DINO_DECD_MODE_STS3r_t mode_sts3;
    BCMI_DINO_DECD_MODE_STS4r_t mode_sts4;
    uint16_t sys_sel     = 0;
    uint16_t line_sel    = 0;
    uint16_t dev_type    = 0;
    uint16_t lane_mask   = 0;
    uint16_t slice_0     = 0;
    uint16_t slice_1     = 0;
    uint16_t slice_2     = 0;
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_DINO_SLICEr_t));
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));
    PHYMOD_MEMSET(&mode_sts1, 0, sizeof(BCMI_DINO_DECD_MODE_STS1r_t));
    PHYMOD_MEMSET(&mode_sts2, 0, sizeof(BCMI_DINO_DECD_MODE_STS2r_t));
    PHYMOD_MEMSET(&mode_sts3, 0, sizeof(BCMI_DINO_DECD_MODE_STS3r_t));
    PHYMOD_MEMSET(&mode_sts4, 0, sizeof(BCMI_DINO_DECD_MODE_STS4r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS1r(pa, &mode_sts1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS2r(pa, &mode_sts2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS3r(pa, &mode_sts3));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS4r(pa, &mode_sts4));

    if (slice_op_type == DINO_SLICE_RESET) {
        slice_0 = 1;
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_SLICEr(pa, &slice_reg));
        if ((mode_sts0.v[0] & 0xff9) ||
            (mode_sts1.v[0] & 0xf) ||
            (mode_sts2.v[0] & 0xf)) {
            slice_0 = 1;
        }
        if ((mode_sts0.v[0] & 0x2) ||
            (mode_sts1.v[0] & 0xf0) ||
            (mode_sts2.v[0] & 0xf0)) {
            slice_1 = 1;
        }
        if ((mode_sts0.v[0] & 0x4) ||
            (mode_sts1.v[0] & 0xf00) ||
            (mode_sts2.v[0] & 0xf00)) {
            slice_2 = 1;
        }
        BCMI_DINO_SLICEr_CLR(slice_reg);
        BCMI_DINO_SLICEr_SYS_SELf_SET(slice_reg, 0);
        BCMI_DINO_SLICEr_LIN_SELf_SET(slice_reg, 1);
        BCMI_DINO_SLICEr_SLICE_0_SELf_SET(slice_reg, slice_0);
        BCMI_DINO_SLICEr_SLICE_1_SELf_SET(slice_reg, slice_1);
        BCMI_DINO_SLICEr_SLICE_2_SELf_SET(slice_reg, slice_2);
        BCMI_DINO_SLICEr_LANE_SELf_SET(slice_reg, 0x1);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_SLICEr(pa, slice_reg));
        return PHYMOD_E_NONE;
    }

    if (if_side == DINO_IF_SYS) {
        sys_sel = 1;
        line_sel = 0;
    } else {
        sys_sel = 0;
        line_sel = 1;
    }

    dev_type = pa->devad;
    if(!dev_type) {
        dev_type = DINO_DEV_PMA_PMD;
    }

    if (slice_op_type == DINO_SLICE_BROADCAST) {
        if (if_side == DINO_IF_SYS) { 
            if (lane_mask & DINO_40G_PORT0_LANE_MASK) {
                slice_0 = 1;
            }
            if (lane_mask & DINO_40G_PORT1_LANE_MASK) {
                slice_1 = 1;
            }
            if (lane_mask & DINO_40G_PORT2_LANE_MASK) {
                slice_2 = 1;
            }
            if (lane_mask == DINO_40G_PORT0_LANE_MASK  ||
                lane_mask == DINO_40G_PORT1_LANE_MASK  ||
                lane_mask == DINO_40G_PORT2_LANE_MASK  ||
                lane_mask == DINO_100G_TYPE1_LANE_MASK ||
                lane_mask == DINO_100G_TYPE2_LANE_MASK ||
                lane_mask == DINO_100G_TYPE3_LANE_MASK) {
                lane_mask = DINO_40G_PORT0_LANE_MASK;
            } else {
                lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
            }
        } else {
            if (lane < DINO_MAX_FALCON_LANE) {
                slice_0 = 1;
            } else if ((lane >= DINO_MAX_FALCON_LANE) && (lane < 8)) {
                slice_1 = 1;
            } else {
                slice_2 = 1;
            }
            if (lane_mask == DINO_40G_PORT0_LANE_MASK ||
                lane_mask == DINO_40G_PORT1_LANE_MASK ||
                lane_mask == DINO_40G_PORT2_LANE_MASK) {
                lane_mask = DINO_40G_PORT0_LANE_MASK;
            } else {
                lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
            }
        }
    } else if (slice_op_type == DINO_SLICE_UNICAST) {
        if (lane < DINO_MAX_FALCON_LANE) {
            slice_0 = 1;
        } else if ((lane >= DINO_MAX_FALCON_LANE) && (lane < 8)) {
            slice_1 = 1;
        } else {
            slice_2 = 1;
        }
        lane_mask = (1 << (lane % DINO_MAX_FALCON_LANE));
    }

    if (dev_type == DINO_DEV_PMA_PMD) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_SLICEr(pa, &slice_reg));
        BCMI_DINO_SLICEr_CLR(slice_reg);
        BCMI_DINO_SLICEr_SYS_SELf_SET(slice_reg, sys_sel);
        BCMI_DINO_SLICEr_LIN_SELf_SET(slice_reg, line_sel);
        BCMI_DINO_SLICEr_SLICE_0_SELf_SET(slice_reg, slice_0);
        BCMI_DINO_SLICEr_SLICE_1_SELf_SET(slice_reg, slice_1);
        BCMI_DINO_SLICEr_SLICE_2_SELf_SET(slice_reg, slice_2);
        BCMI_DINO_SLICEr_LANE_SELf_SET(slice_reg, lane_mask);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_SLICEr(pa, slice_reg));
        PHYMOD_DEBUG_VERBOSE(("\n[-- slice = %x ===]\n", BCMI_DINO_SLICEr_GET(slice_reg)));
    } else {
        /* Needs to add AN slice sequence */
    }

    return PHYMOD_E_NONE;
}

int _dino_fifo_reset(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config) {
    uint16_t lane_mask = 0;
    uint16_t if_side   = 0;
    uint16_t lane  = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t data = 0;
    BCMI_DINO_FIFO_RST_FRC_CTL1r_t fifo_rst_frc_ln;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t fifo_rst_frcval_ln;
    BCMI_DINO_FIFO_RST_FRC_CTL0r_t fifo_rst_frc_sys;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t fifo_rst_frcval_sys;
    PHYMOD_MEMSET(&fifo_rst_frc_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frc_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL0r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);

    if ((config->data_rate == DINO_SPD_100G) || (config->data_rate == DINO_SPD_106G) || (config->data_rate == DINO_SPD_111G)) {
        lane_mask = 0xfff;
    }
    for (lane = 0; lane < DINO_MAX_LANE; lane++) {
        if (lane_mask & (1 << lane)) {
            if (if_side == DINO_IF_LINE) {
                /* Set force val to 0*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
                data = fifo_rst_frcval_ln.v[0];
                data &= ~(1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

                /* Set force to 1*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
                data = fifo_rst_frc_ln.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

                /* Set force val to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
                data = fifo_rst_frcval_ln.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));
                
                /* Set force to default val*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));

                fifo_rst_frc_ln.v[0] &= ~(1 << lane);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));
            } else {
                /* Set force val to 0*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
                data = fifo_rst_frcval_sys.v[0];
                data &= ~(1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

                /* Set force to 1*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
                data = fifo_rst_frc_sys.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));

                /* Set force val to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
                data = fifo_rst_frcval_sys.v[0];
                data |= (1 << lane);
                BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

                /* Set force to default*/
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
                fifo_rst_frc_sys.v[0] &= (1 << lane);
                BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));
            }
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    const phymod_access_t *pa = &phy->access;

   
    PHYMOD_DEBUG_VERBOSE(("Configuring interface\n"));
    /* Configure Ref Clock*/
    PHYMOD_DEBUG_VERBOSE(("Configuring REF clock %d\n",config->ref_clock));
    PHYMOD_IF_ERR_RETURN(
        _dino_configure_ref_clock(&phy->access, config->ref_clock));

    /* Configure DUT MODE */
    PHYMOD_IF_ERR_RETURN(
        _dino_config_dut_mode_reg(phy, config));
    
    /* Configure PLL Divider*/
    PHYMOD_IF_ERR_RETURN(
        _dino_config_pll_div(phy, config));

    /* Configure Interface.*/
    PHYMOD_IF_ERR_RETURN(
        _dino_interface_set(phy, config));

    /* store phy type */
    PHYMOD_IF_ERR_RETURN(
        _dino_save_phy_type(pa, config));

    /* Do fifo reset */
    PHYMOD_IF_ERR_RETURN(
        _dino_fifo_reset(phy, config));

    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _dino_interface_get(phymod_phy_access_t *phy, phymod_phy_inf_config_t *config, phymod_interface_t *intf)
{
    PHYMOD_IF_ERR_RETURN(
        _dino_restore_interface(phy, config, intf));

    return PHYMOD_E_NONE;
}

int _dino_phy_interface_config_get(phymod_phy_access_t *phy, uint32_t flags, phymod_phy_inf_config_t *config)
{
    uint16_t if_side   = 0;
    uint16_t lane_mask = 0;
    uint16_t lane = 0;
    phymod_interface_t intf = 0;
    uint16_t lane_index = 0;
    uint32_t port_speed = 0;
    uint16_t phy_type = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mer_pll_mapval  = 0;
    uint16_t falcon_pll_mode = 0;
    uint16_t reg_data = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    const phymod_access_t *pa = &phy->access;
    BCMI_DINO_F25G_PLL_CAL_CTL7r_t falcon_pll_ctrl;
    /* Get data rate, passthru, gearbox, get refclock, get intf side*/
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    BCMI_DINO_DECD_MODE_STS1r_t mode_sts1;
    BCMI_DINO_DECD_MODE_STS2r_t mode_sts2;
    BCMI_DINO_DECD_MODE_STS3r_t mode_sts3;
    BCMI_DINO_DECD_MODE_STS4r_t mode_sts4;
    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));
    PHYMOD_MEMSET(&mode_sts1, 0, sizeof(BCMI_DINO_DECD_MODE_STS1r_t));
    PHYMOD_MEMSET(&mode_sts2, 0, sizeof(BCMI_DINO_DECD_MODE_STS2r_t));
    PHYMOD_MEMSET(&mode_sts3, 0, sizeof(BCMI_DINO_DECD_MODE_STS3r_t));
    PHYMOD_MEMSET(&mode_sts4, 0, sizeof(BCMI_DINO_DECD_MODE_STS4r_t));
    PHYMOD_MEMSET(&falcon_pll_ctrl, 0, sizeof(BCMI_DINO_F25G_PLL_CAL_CTL7r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(&phy->access, &chip_id, &rev));

    if (!lane_mask) {
        PHYMOD_DEBUG_VERBOSE(("Invalid Lanemask\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS1r(pa, &mode_sts1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS2r(pa, &mode_sts2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS3r(pa, &mode_sts3));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DECD_MODE_STS4r(pa, &mode_sts4));

    if ((BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_2TO11f_GET(mode_sts0)) ||
       (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_1TO10f_GET(mode_sts0)) ||
       (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_PT_0TO9f_GET(mode_sts0))) {
        config->data_rate = DINO_SPD_100G;
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_CLR(phy);
    } else if ((BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_2TO11f_GET(mode_sts0)) ||
               (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_1TO10f_GET(mode_sts0)) ||
               (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_0TO9f_GET(mode_sts0))) {
        config->data_rate = DINO_SPD_100G;
        PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_SET(phy);
    } else if (lane_mask == DINO_40G_PORT0_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP0f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else if (lane_mask == DINO_40G_PORT1_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP1f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else if (lane_mask == DINO_40G_PORT2_LANE_MASK) {
        if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_40G_MP2f_GET(mode_sts0)) {
            config->data_rate = DINO_SPD_40G;
        }
    } else {
        for (lane_index = 0; lane_index <= DINO_MAX_LANE; lane_index ++) {
            if (lane_mask & (0x1 << lane_index)) {
                switch (lane_index) {
                    case DINO_PORT0:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT1:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT2:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT3:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP0_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP0_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT4:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT5:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT6:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT7:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP1_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP1_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT8:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_0f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_0f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT9:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_1f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_1f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT10:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_2f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_2f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                    case DINO_PORT11:
                        if (BCMI_DINO_DECD_MODE_STS1r_DECODED_MODE_10G_MP2_3f_GET(mode_sts1)) {
                            config->data_rate = DINO_SPD_10G;
                        } else if (BCMI_DINO_DECD_MODE_STS2r_DECODED_MODE_1G_MP2_3f_GET(mode_sts2)) {
                            config->data_rate = DINO_SPD_1G;
                        }
                    break;
                }
            }
        }
    } 
    port_speed = config->data_rate;

    /*Ethernet*/
    config->interface_modes = 0;

    PHYMOD_IF_ERR_RETURN(
        _dino_restore_phy_type(pa, config, &phy_type));

    /* Read the ref clock value from scratch register */
    READ_DINO_PMA_PMD_REG(pa, DINO_REF_CLK_REG_ADR, sw_gpreg_data);
    config->ref_clock = ((sw_gpreg_data & DINO_REF_CLK_VAL_MASK) >> DINO_REF_CLK_VAL_SHIFT); 

    /* Read PLL value from register */
    for (if_side = DINO_IF_LINE ; if_side <= DINO_IF_SYS ; if_side++) {
        for (lane = 0; lane < DINO_MAX_LANE ; lane += 4) {
            if (lane_mask & (0xf << lane)) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                    /* Read the current pll mode program the pll only if the mode is not configured already */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_F25G_PLL_CAL_CTL7r(pa, &falcon_pll_ctrl));
                    falcon_pll_mode = BCMI_DINO_F25G_PLL_CAL_CTL7r_PLL_MODEf_GET(falcon_pll_ctrl);
                } else {
                    /* Read current merlin PLL mode map val from scratch register*/
                    READ_DINO_PMA_PMD_REG(pa, DINO_MER_PLL_MAP_VAL_REG_ADR, reg_data);
                    mer_pll_mapval = (reg_data & (0x3 << ((2*(lane/4)) + (8*if_side)))) >> ((2*(lane/4))+(8*if_side));
                }
                PHYMOD_IF_ERR_RETURN (
                    _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
                break;
            }
        }
    }

    for (lane_index = 0; lane_index <= DINO_MAX_LANE; lane_index ++) {
        if (lane_mask & (0xf << lane_index)) {
            if (phy_type == DINO_PHY_TYPE_HIGIG) {
                switch (config->ref_clock) {
                    case phymodRefClk156Mhz: 
                    case phymodRefClk312Mhz: 
                        switch (port_speed) {
                            case DINO_SPD_10G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_140) {
                                        config->data_rate = DINO_SPD_11G;
                                    } else {
                                        config->data_rate = DINO_SPD_10G;
                                    }
                                } else {
                                    if (mer_pll_mapval == 0x3) {
                                        config->data_rate = DINO_SPD_11G;
                                    } else {
                                        config->data_rate = DINO_SPD_10G;
                                    }
                                }
                            break;
                            case DINO_SPD_40G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_140) {
                                        config->data_rate = DINO_SPD_42G;
                                    } else {
                                        config->data_rate = DINO_SPD_40G;
                                    }
                                } else {
                                    if (mer_pll_mapval == 0x3) {
                                        config->data_rate = DINO_SPD_42G;
                                    } else {
                                        config->data_rate = DINO_SPD_40G;
                                    }
                                }
                            break;
                            case DINO_SPD_100G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_175) {
                                        config->data_rate = DINO_SPD_106G;
                                    } else {
                                        config->data_rate = DINO_SPD_100G;
                                    }
                                } else {
                                    if (mer_pll_mapval == 0x3) {
                                        config->data_rate = DINO_SPD_106G;
                                    } else {
                                        config->data_rate = DINO_SPD_100G;
                                    }
                                }
                            break;
                            default:
                            break;
                        }
                    break;
                    default:
                    break;
                }    
                PHYMOD_INTF_MODES_HIGIG_SET(config);
            } else if (phy_type == DINO_PHY_TYPE_OTN) {
                switch (config->ref_clock) {
                    case phymodRefClk174Mhz: 
                    case phymodRefClk349Mhz: 
                    case phymodRefClk698Mhz: 
                        switch (port_speed) {
                            case DINO_SPD_10G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_128) {
                                        config->data_rate = DINO_SPD_11P18G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x1) {
                                        config->data_rate = DINO_SPD_11P18G;
                                    } 
                                }
                            break;
                            case DINO_SPD_100G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_160) {
                                        config->data_rate = DINO_SPD_111G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x1) {
                                        config->data_rate = DINO_SPD_111G;
                                    } 
                                }
                            break;
                            default:
                            break;
                        }
                    break;
                    default:
                    break;
                }    
            } else if (phy_type == DINO_PHY_TYPE_ETH) {
                switch (config->ref_clock) {
                    case phymodRefClk156Mhz: 
                    case phymodRefClk312Mhz: 
                        switch (port_speed) {
                            case DINO_SPD_10G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_132) {
                                        config->data_rate = DINO_SPD_10G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x2) {
                                        config->data_rate = DINO_SPD_10G;
                                    } 
                                }
                            break;
                            case DINO_SPD_40G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_132) {
                                        config->data_rate = DINO_SPD_40G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x2) {
                                        config->data_rate = DINO_SPD_40G;
                                    } 
                                }
                            break;
                            case DINO_SPD_100G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if (falcon_pll_mode == DINO_PLL_MODE_175) {
                                        config->data_rate = DINO_SPD_100G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x2) {
                                        config->data_rate = DINO_SPD_100G;
                                    } 
                                }
                            break;
                            default:
                            break;
                        }
                    break;
                    case phymodRefClk161Mhz: 
                    case phymodRefClk322Mhz: 
                    case phymodRefClk644Mhz:
                        switch (port_speed) {
                            case DINO_SPD_10G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if ((falcon_pll_mode == DINO_PLL_MODE_128) ||
                                        ((config->ref_clock == phymodRefClk322Mhz) && 
                                         (chip_id == DINO_CHIP_ID_82793) && 
                                         (falcon_pll_mode == DINO_PLL_MODE_64))) {
                                        config->data_rate = DINO_SPD_10G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x1) {
                                        config->data_rate = DINO_SPD_10G;
                                    } 
                                }
                            break;
                            case DINO_SPD_40G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if ((falcon_pll_mode == DINO_PLL_MODE_128) ||
                                        ((config->ref_clock == phymodRefClk322Mhz) && 
                                         (chip_id == DINO_CHIP_ID_82793) && 
                                         (falcon_pll_mode == DINO_PLL_MODE_64))) {
                                        config->data_rate = DINO_SPD_40G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x1) {
                                        config->data_rate = DINO_SPD_40G;
                                    } 
                                }
                            break;
                            case DINO_SPD_100G:
                                if (DINO_IS_FALCON_SIDE(if_side, lane_index)) { 
                                    if ((falcon_pll_mode == DINO_PLL_MODE_160) ||
                                        ((config->ref_clock == phymodRefClk322Mhz) &&
                                         (chip_id == DINO_CHIP_ID_82793) &&
                                         (falcon_pll_mode == DINO_PLL_MODE_80))) {
                                        config->data_rate = DINO_SPD_100G;
                                    } 
                                } else {
                                    if (mer_pll_mapval == 0x1) {
                                        config->data_rate = DINO_SPD_100G;
                                    } 
                                }
                            break;
                            default:
                            break;
                        }
                    break;
                    default:
                    break;
                }
                PHYMOD_INTF_MODES_HIGIG_CLR(config);
                PHYMOD_INTF_MODES_OTN_CLR(config);
                PHYMOD_INTF_MODES_FIBER_CLR(config);
            }
            break;
        }
    }
    PHYMOD_IF_ERR_RETURN(
        _dino_interface_get(phy, config, &intf));
    config->interface_type = intf;

    return PHYMOD_E_NONE;
}

int _dino_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk)
{
    uint32_t rev     = 0;
    uint32_t chip_id = 0;
    uint16_t sw_gpreg_data = 0;
    BCMI_DINO_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_DINO_CLK_SCALER_CTLr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
   
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x320);
        break;
        case phymodRefClk161Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
        break;
        case phymodRefClk322Mhz:
            if (chip_id == DINO_CHIP_ID_82793) {
                BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x680);
            } else {
                BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
            }
        break;
        case phymodRefClk644Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x340);
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk698Mhz:
            BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x380);
        break;
        case phymodRefClk349Mhz:
            if (chip_id == DINO_CHIP_ID_82793) {
                BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x700);
            } else {
                BCMI_DINO_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl, 0x380);
            }
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));

    /* Store the ref clock value in scratch register */
    READ_DINO_PMA_PMD_REG(pa, DINO_REF_CLK_REG_ADR, sw_gpreg_data);
    sw_gpreg_data &= ~(DINO_REF_CLK_VAL_MASK);
    sw_gpreg_data |= (ref_clk << DINO_REF_CLK_VAL_SHIFT);
    WRITE_DINO_PMA_PMD_REG(pa, DINO_REF_CLK_REG_ADR, sw_gpreg_data);
    
    return PHYMOD_E_NONE;
}

/** Configure Dut Mode Register
 */
int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config) 
{
    uint16_t no_of_ports      = 0;
    uint16_t port_idx         = 0;
    uint16_t lane_mask        = 0;
    uint16_t if_side          = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    uint16_t sw_gpreg_data    = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    BCMI_DINO_MODECTL0r_t mode_ctrl0;
    BCMI_DINO_MODECTL1r_t mode_ctrl1;
    BCMI_DINO_MODECTL2r_t mode_ctrl2;
    BCMI_DINO_MODECTL3r_t mode_ctrl3;
    phymod_phy_inf_config_t cfg;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_DINO_MODECTL0r_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_DINO_MODECTL1r_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_DINO_MODECTL2r_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_DINO_MODECTL3r_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE(config, chip_id, no_of_ports);

    /* Store the device operation type into SW GPREG */
    if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) {
        dev_op_mode = DINO_DEV_OP_MODE_GBOX;
    } else {
        dev_op_mode = DINO_DEV_OP_MODE_PT;
    }
    READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
    sw_gpreg_data &= ~(DINO_DEV_OP_MODE_MASK);  
    sw_gpreg_data |= (dev_op_mode << DINO_DEV_OP_MODE_SHIFT);  
    WRITE_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL1r(pa, &mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL2r(pa, &mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL3r(pa, &mode_ctrl3));

    if ((config->data_rate != DINO_SPD_100G) && (config->data_rate != DINO_SPD_106G) && (config->data_rate != DINO_SPD_111G)) {
        /* To clear all 100G mode settings */
        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
        for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
            /* Program Modes */
            if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                 (lane_mask == DINO_40G_PORT0_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
            } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                        (lane_mask == DINO_40G_PORT1_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
            } else if (((config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) &&
                        (lane_mask == DINO_40G_PORT2_LANE_MASK))) {
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 0);
                BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
            } else if ((config->data_rate == DINO_SPD_10G) || (config->data_rate == DINO_SPD_11G) || (config->data_rate == DINO_SPD_11P18G)) {
                if ((lane_mask & (0x1 << port_idx))) {
                    switch (port_idx) {
                        case DINO_PORT0:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT1:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT2:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT3:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT4:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT5:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT6:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT7:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT8:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT9:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT10:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT11:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                    }
                }
            } else if (config->data_rate == DINO_SPD_1G) {
                /* mode configuration for 1G */
                if ((lane_mask & (0x1 << port_idx))) {
                    switch (port_idx) {
                        case DINO_PORT0:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT1:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT2:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT3:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP0_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP0_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT4:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT5:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT6:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT7:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP1_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP1_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT8:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_0f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_0f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT9:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_1f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_1f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT10:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_2f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_2f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                        case DINO_PORT11:
                            BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
                            BCMI_DINO_MODECTL1r_MODE_10G_MP2_3f_SET(mode_ctrl1, 0);
                            BCMI_DINO_MODECTL2r_MODE_1G_MP2_3f_SET(mode_ctrl2, 1);
                            BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                        break;
                    }
                }
            } else {
                /* Throw error as un supported mode */
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Unsupported mode")));
            }
        }
    } else {
        BCMI_DINO_MODECTL1r_CLR(mode_ctrl1);
        BCMI_DINO_MODECTL2r_CLR(mode_ctrl2);
        BCMI_DINO_MODECTL3r_CLR(mode_ctrl3);
        BCMI_DINO_MODECTL0r_MODE_40G_MP0f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_40G_MP1f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_MODE_40G_MP2f_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
        BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 0);
        BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 0);
        if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) { /* 100G GBOX mode */
            if (config->interface_modes & PHYMOD_INTF_MODES_TRIPLE_CORE) {
                if (if_side == DINO_IF_SYS) {
                    if (config->interface_modes & PHYMOD_INTF_MODES_TC_442) { /*Dino system lanes 0-9*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_343) { /*Dino system lanes 1-10*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_244) { /*Dino system lanes 2-11*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    } else {
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    }
                } else {
                    if (config->interface_modes & PHYMOD_INTF_MODES_TC_442) { /*Dino system lanes 0-9*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_343) { /*Dino system lanes 1-10*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_244) { /*Dino system lanes 2-11*/
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    }
                    if ((!(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_GET(mode_ctrl0)) &&
                         !(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_GET(mode_ctrl0)) &&
                         !(BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_GET(mode_ctrl0)))) {
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                        BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                    }
                }
            } else {
                PHYMOD_DEBUG_VERBOSE(("interface mode PHYMOD_INTF_MODES_TRIPLE_CORE is not set\n"));
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
            }
        } else { /* 100G PT mode */
            if (config->interface_modes & PHYMOD_INTF_MODES_TC_442) { /*Dino system lanes 0-9*/
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
            } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_343) { /*Dino system lanes 1-10*/
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
            } else if (config->interface_modes & PHYMOD_INTF_MODES_TC_244) { /*Dino system lanes 2-11*/
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
            } else {
                BCMI_DINO_MODECTL0r_MODE_100G_PT_0TO9f_SET(mode_ctrl0, 1);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_PT_2TO11f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_0TO9f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_1TO10f_SET(mode_ctrl0, 0);
                BCMI_DINO_MODECTL0r_MODE_100G_GBOX_2TO11f_SET(mode_ctrl0, 0);
            }
        }
    }
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL1r(pa, mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL2r(pa, mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL3r(pa, mode_ctrl3));

    return PHYMOD_E_NONE;
}

int _dino_config_pll_div(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config)
{
    uint16_t if_side = 0;
    uint16_t merlin_pll_mode = 0;
    uint16_t falcon_pll_mode = 0;
    uint16_t data = 0;
    uint16_t reg_data = 0;
    uint16_t lane_mask = 0;
    uint16_t lane = 0;
    uint16_t mer_pll_mapval  = 0;
    int mer_curr_pll_mapval = 0;
    uint16_t retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
    uint8_t falcon_curr_pll_mode = 0;
    phymod_phy_access_t phy_copy;
    phymod_ref_clk_t ref_clk = config->ref_clock;
    BCMI_DINO_SLICEr_t slice_reg;
    BCMI_DINO_RST_CTL_CORE_DPr_t rst_ctl_core_dp;
    BCMI_DINO_PLL_CTL3r_t pll_ctl3;
    BCMI_DINO_PLL_CTL8r_t pll_ctl8;
    BCMI_DINO_F25G_PLL_CAL_CTL_STS0r_t falcon_pll_sts;
    BCMI_DINO_PLL_CALSTS0r_t merlin_pll_sts;
    BCMI_DINO_F25G_PLL_CAL_CTL7r_t falcon_pll_ctrl;
    phymod_access_t *pa = (phymod_access_t*)&phy->access;
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_DINO_SLICEr_t));
    PHYMOD_MEMSET(&rst_ctl_core_dp, 0, sizeof(BCMI_DINO_RST_CTL_CORE_DPr_t));
    PHYMOD_MEMSET(&pll_ctl3, 0, sizeof(BCMI_DINO_PLL_CTL3r_t));
    PHYMOD_MEMSET(&pll_ctl8, 0, sizeof(BCMI_DINO_PLL_CTL8r_t));
    PHYMOD_MEMSET(&falcon_pll_ctrl, 0, sizeof(BCMI_DINO_F25G_PLL_CAL_CTL7r_t));
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if ((config->data_rate == DINO_SPD_100G) || 
        (config->data_rate == DINO_SPD_106G) ||
        (config->data_rate == DINO_SPD_111G)) {
        lane_mask = DINO_100G_TYPE1_LANE_MASK;
    }

    /* Set pll_mode */
    PHYMOD_IF_ERR_RETURN(
       _dino_get_pll_modes(phy, ref_clk, config, &falcon_pll_mode, &merlin_pll_mode));
    for (if_side = DINO_IF_LINE ; if_side <= DINO_IF_SYS ; if_side++) {
        for (lane = 0; lane < DINO_MAX_LANE ; lane += 4) {
            if (lane_mask & (0xf << lane)) {
                PHYMOD_IF_ERR_RETURN (
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                    /* Read the current pll mode program the pll only if the mode is not configured already */
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_F25G_PLL_CAL_CTL7r(pa, &falcon_pll_ctrl));
                    falcon_curr_pll_mode = BCMI_DINO_F25G_PLL_CAL_CTL7r_PLL_MODEf_GET(falcon_pll_ctrl);
                    if (falcon_curr_pll_mode == falcon_pll_mode) {
                        continue;
                    } 
                    /* 0xd11b . enable pll . Added macro in .h file */
                    PHYMOD_IF_ERR_RETURN(
                        falcon2_dino_wrc_ams_pll_pwrdn(0)); 
                    /* write 0 to core_dp_s_rstb while pll config .  Added macro in .h file*/
                    /*  0xd184 .wrc_falcon2_dino_core_dp_s_rstb(0); */
                    PHYMOD_IF_ERR_RETURN(
                        falcon2_dino_wrc_core_dp_s_rstb(0)); 

                    /* Set PLL div */
                    switch (falcon_pll_mode) {
                        case DINO_PLL_MODE_64:
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_64x_refc322)); 
                        break;
                        case DINO_PLL_MODE_80:
                            if (config->ref_clock == phymodRefClk322Mhz) {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_80x_refc322)); 
                            } else {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_80x)); 
                            }
                        break;
                        case DINO_PLL_MODE_160:
                            if (config->data_rate == DINO_SPD_111G) {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_160x_refc174));
                            } else {
                                PHYMOD_IF_ERR_RETURN (
                                    falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_160x)); 
                            }
                        break;
                        case DINO_PLL_MODE_132: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_132x)); 
                        break;
                        case DINO_PLL_MODE_128: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_128x)); 
                        break;
                        case DINO_PLL_MODE_140: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_140x));
                        break;
                        case DINO_PLL_MODE_165: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_165x));
                        break;
                        case DINO_PLL_MODE_168: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_168x));
                        break;
                        case DINO_PLL_MODE_175: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_175x));
                        break;
                        case DINO_PLL_MODE_180: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_180x));
                        break;
                        case DINO_PLL_MODE_184: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_184x));
                        break;
                        case DINO_PLL_MODE_200: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_200x));
                        break;
                        case DINO_PLL_MODE_224: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_224x));
                        break;
                        case DINO_PLL_MODE_264: 
                            PHYMOD_IF_ERR_RETURN (
                                falcon2_dino_configure_pll(pa, FALCON2_DINO_pll_div_264x));
                        break;
                        default:
                        return PHYMOD_E_PARAM;
                    }
                    /* write 1 to release core_dp_s_rstb after pll config*/
                    PHYMOD_IF_ERR_RETURN (
                        falcon2_dino_wrc_core_dp_s_rstb(1));
                    retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
                    /* Check for the PLL lock */
                    do {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_F25G_PLL_CAL_CTL_STS0r(pa, &falcon_pll_sts));
                        data = BCMI_DINO_F25G_PLL_CAL_CTL_STS0r_PLL_LOCKf_GET(falcon_pll_sts);
                        PHYMOD_USLEEP(100);
                    } while((data != 1) && (retry_cnt--));
                    if (retry_cnt == 0) {
                           PHYMOD_DEBUG_ERROR(("PLL LOCK failed for pll\n"));       
                        return PHYMOD_E_FAIL;
                    }
                } else {
                    /* Read current merlin PLL mode map val from scratch register*/
                    READ_DINO_PMA_PMD_REG(pa, DINO_MER_PLL_MAP_VAL_REG_ADR, reg_data);
                    mer_curr_pll_mapval = (reg_data & (0x3 << ((2*(lane/4)) + (8*if_side)))) >> ((2*(lane/4))+(8*if_side));
                    mer_pll_mapval = _dino_map_mer_pll_div(merlin_pll_mode);
                    if (mer_pll_mapval == mer_curr_pll_mapval) {
                        continue;
                    }
                    /* write 0 to core_dp_s_rstb while pll config*/
                    PHYMOD_IF_ERR_RETURN(
                        wrc_core_dp_s_rstb(0));
                    switch (mer_pll_mapval) {
                        case 0x1:
                            if ((config->data_rate == DINO_SPD_111G) ||
                                (config->data_rate == DINO_SPD_11P18G)) {
                                PHYMOD_IF_ERR_RETURN(
                                    merlin_dino_configure_pll(pa, MERLIN_DINO_pll_11p181GHz_174p703MHz));
                            } else {
                                PHYMOD_IF_ERR_RETURN(
                                    merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p3125GHz_161p132MHz));
                            }
                        break;
                        case 0x2:
                            PHYMOD_IF_ERR_RETURN(
                                 merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p3125GHz_156p25MHz));
                        break;
                        case 0x3:
                            PHYMOD_IF_ERR_RETURN(
                                 merlin_dino_configure_pll(pa, MERLIN_DINO_pll_10p9375GHz_156p25MHz));
                        break;
                        default:
                            return PHYMOD_E_PARAM;
                    }
                    /* write 1 to core_dp_s_rstb after pll config*/
                    PHYMOD_IF_ERR_RETURN(
                        wrc_core_dp_s_rstb(1));

                    retry_cnt = DINO_FW_DLOAD_RETRY_CNT;
                    do {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_PLL_CALSTS0r(pa, &merlin_pll_sts));
                        data = BCMI_DINO_PLL_CALSTS0r_PLL_LOCKf_GET(merlin_pll_sts);
                        PHYMOD_USLEEP(100);
                    } while((data != 1) && (retry_cnt--));
                    if (retry_cnt == 0) {
                           PHYMOD_DEBUG_ERROR(("PLL LOCK failed for pll\n"));       
                        return PHYMOD_E_FAIL;
                    }
                    /* store merlin pll map value into sctratch reg */
                    reg_data &= ~(0x3 << ((2*(lane/4))+(8*if_side)));
                    reg_data |= (mer_pll_mapval << ((2*(lane/4))+(8*if_side)));
                    WRITE_DINO_PMA_PMD_REG(pa, DINO_MER_PLL_MAP_VAL_REG_ADR, reg_data);
                }
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_restore_phy_type(const phymod_access_t *pa, phymod_phy_inf_config_t* config, uint16_t *phy_type) {
    uint16_t sw_gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    uint16_t lane_mask = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    *phy_type = DINO_PHY_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE(config, chip_id, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G || config->data_rate == DINO_SPD_111G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            break;
        } else if (config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) {  
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), sw_gpreg_data);
                break;
            } else {
                continue;
            }
        } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G || config->data_rate == DINO_SPD_11P18G || config->data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + port_idx), sw_gpreg_data);
                break;
            } else {
                continue;
            }
        } else {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    *phy_type = ((sw_gpreg_data & DINO_PHY_TYPE_MASK) >> DINO_PHY_TYPE_SHIFT);

    return PHYMOD_E_NONE;
}
int _dino_save_phy_type(const phymod_access_t *pa, 
                         const phymod_phy_inf_config_t *config)
{
    uint16_t lane_idx = 0;
    uint16_t sw_gpreg_data = 0;
    uint16_t lane_mask = 0;
    uint16_t phy_type = DINO_PHY_TYPE_INVALID;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
        phy_type = DINO_PHY_TYPE_HIGIG;
    } else if ((config->data_rate == DINO_SPD_111G) ||
               (config->data_rate == DINO_SPD_11P18G)) {
        phy_type = DINO_PHY_TYPE_OTN;
    } else {
        phy_type = DINO_PHY_TYPE_ETH;
    }

    if ((config->data_rate == DINO_SPD_100G) || 
        (config->data_rate == DINO_SPD_106G) ||
        (config->data_rate == DINO_SPD_111G)) {
        lane_mask = 0x1;
    }

    for (lane_idx = 0; lane_idx < DINO_MAX_LANE; lane_idx++) {
        if (lane_mask & (1 << lane_idx)) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
            sw_gpreg_data &= ~(DINO_PHY_TYPE_MASK);
            sw_gpreg_data |= (phy_type << DINO_PHY_TYPE_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
        }
    }

    return PHYMOD_E_NONE;
}
int _dino_restore_interface(phymod_phy_access_t *phy, 
                            phymod_phy_inf_config_t *config,
                            phymod_interface_t *intf) {
    uint16_t if_side   = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = 0;
    uint16_t interface_type = DINO_INTF_TYPE_SR;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t media_type  = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t gpreg_data  = 0;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE(config, chip_id, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config->data_rate == DINO_SPD_100G || config->data_rate == DINO_SPD_106G || config->data_rate == DINO_SPD_111G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            break;
        } else if (config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), sw_gpreg_data);
                break;
            } else {
                continue;
            }
        } else if (config->data_rate == DINO_SPD_10G || config->data_rate == DINO_SPD_11G || config->data_rate == DINO_SPD_11P18G || config->data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + port_idx), sw_gpreg_data);
                break;
            } else {
                continue;
            }
        }
    }

    if (if_side == DINO_IF_LINE) {
        interface_type = ((sw_gpreg_data & DINO_LINE_INTF_MASK) >> DINO_LINE_INTF_SHIFT);
        media_type = ((gpreg_data & DINO_LINE_MEDIA_TYPE_MASK) >> DINO_LINE_MEDIA_TYPE_SHIFT);
    } else {
        interface_type = ((sw_gpreg_data & DINO_SYS_INTF_MASK) >> DINO_SYS_INTF_SHIFT);
        media_type = ((gpreg_data & DINO_SYS_MEDIA_TYPE_MASK) >> DINO_SYS_MEDIA_TYPE_SHIFT);
    }

    if (media_type == SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS) {
        PHYMOD_INTF_MODES_UNRELIABLE_LOS_SET(config);
    } else {
        PHYMOD_INTF_MODES_UNRELIABLE_LOS_CLR(config);
    } 

    switch (interface_type) {
        case DINO_INTF_TYPE_SR:
            if (config->data_rate == DINO_SPD_1G ||
                config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G ||
                config->data_rate == DINO_SPD_11P18G) {
                *intf = phymodInterfaceSR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceSR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G ||
                       config->data_rate == DINO_SPD_111G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceSR4;
                } else {
                    *intf = phymodInterfaceSR10;
                }
            } 
        break;
        case DINO_INTF_TYPE_LR:
            if (config->data_rate == DINO_SPD_1G ||
                config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G ||
                config->data_rate == DINO_SPD_11P18G) {
                *intf = phymodInterfaceLR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceLR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G ||
                       config->data_rate == DINO_SPD_111G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceLR4;
                } else {
                    *intf = phymodInterfaceLR10;
                }
            } 
        break;
        case DINO_INTF_TYPE_ER:
            if (config->data_rate == DINO_SPD_1G ||
                config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G ||
                config->data_rate == DINO_SPD_11P18G) {
                *intf = phymodInterfaceER;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) {
                *intf = phymodInterfaceER4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G ||
                       config->data_rate == DINO_SPD_111G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceER4;
                } else {
                    /* *intf = phymodInterfaceER10; */
                }
            } 
        break;
        case DINO_INTF_TYPE_CAUI4:
            *intf = phymodInterfaceCAUI4;
        break;
        case DINO_INTF_TYPE_CAUI:
            *intf = phymodInterfaceCAUI;
        break;
        case DINO_INTF_TYPE_CAUI4_C2C:
            *intf = phymodInterfaceCAUI4_C2C;
        break;
        case DINO_INTF_TYPE_CAUI4_C2M:
            *intf = phymodInterfaceCAUI4_C2M;
        break;
        case DINO_INTF_TYPE_XLAUI:
            *intf = phymodInterfaceXLAUI;
        break;
        case DINO_INTF_TYPE_XFI:
            *intf = phymodInterfaceXFI;
        break;
        case DINO_INTF_TYPE_KR:
            if (config->data_rate == DINO_SPD_1G  ||
                config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G ||
                config->data_rate == DINO_SPD_11P18G) {
                *intf = phymodInterfaceKR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceKR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G ||
                       config->data_rate == DINO_SPD_111G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceKR4;
                } else {
                    *intf = phymodInterfaceKR10; 
                }
            } 
        break;
        case DINO_INTF_TYPE_KX:
            if (config->data_rate == DINO_SPD_1G) {  
                *intf = phymodInterfaceKX;
            } 
        break;
        case DINO_INTF_TYPE_SFI:
            *intf = phymodInterfaceSFI;
        break;
        case DINO_INTF_TYPE_XLPPI:
            *intf = phymodInterfaceXLPPI;
        break;
        case DINO_INTF_TYPE_CX:
            *intf = phymodInterfaceCX;
        break;
        case DINO_INTF_TYPE_CR:
            if (config->data_rate == DINO_SPD_1G ||
                config->data_rate == DINO_SPD_10G ||
                config->data_rate == DINO_SPD_11G ||
                config->data_rate == DINO_SPD_11P18G) {
                *intf = phymodInterfaceCR;
            } else if (config->data_rate == DINO_SPD_40G ||
                       config->data_rate == DINO_SPD_42G) { 
                *intf = phymodInterfaceCR4;
            } else if (config->data_rate == DINO_SPD_100G ||
                       config->data_rate == DINO_SPD_106G ||
                       config->data_rate == DINO_SPD_111G) {
                if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
                    (if_side == DINO_IF_LINE)) {
                    *intf = phymodInterfaceCR4;
                } else {
                    *intf = phymodInterfaceCR10; 
                }
            }
        break;
        case DINO_INTF_TYPE_VSR:
            *intf = phymodInterfaceVSR;
        break;    
        case DINO_INTF_TYPE_OTN:
            *intf = phymodInterfaceOTN;
        break;    
        case DINO_INTF_TYPE_SGMII:
            if (config->data_rate == DINO_SPD_1G) {  
                *intf = phymodInterfaceSGMII;
            } 
        break;
        default:
        break;
    }
    return PHYMOD_E_NONE;
}
int _dino_save_interface(const phymod_phy_access_t *phy,
                         const phymod_phy_inf_config_t* config, 
                         phymod_interface_t intf)
{
    uint16_t lane_idx = 0;
    uint16_t if_side   = 0;
    uint16_t sw_gpreg_data = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = 0;
    uint16_t interface_type = DINO_INTF_TYPE_SR;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    DINO_GET_INTF_SIDE(phy, if_side);

    switch (intf) {
        case phymodInterfaceSR:
        case phymodInterfaceSR4:
        case phymodInterfaceSR10:
            interface_type = DINO_INTF_TYPE_SR;
        break;
        case phymodInterfaceLR:
        case phymodInterfaceLR4:
        case phymodInterfaceLR10:
            interface_type = DINO_INTF_TYPE_LR;
        break;
        case phymodInterfaceER:
        case phymodInterfaceER4:
        /* case phymodInterfaceER10: */
            interface_type = DINO_INTF_TYPE_ER;
        break;
        case phymodInterfaceCAUI4:
            interface_type = DINO_INTF_TYPE_CAUI4;
        break;
        case phymodInterfaceCAUI:
            interface_type = DINO_INTF_TYPE_CAUI;
        break;
        case phymodInterfaceCAUI4_C2C:
            interface_type = DINO_INTF_TYPE_CAUI4_C2C;
        break;
        case phymodInterfaceCAUI4_C2M:
            interface_type = DINO_INTF_TYPE_CAUI4_C2M;
        break;
        case phymodInterfaceXLAUI:
            interface_type = DINO_INTF_TYPE_XLAUI;
        break;
        case phymodInterfaceXFI:
            interface_type = DINO_INTF_TYPE_XFI;
        break;
        case phymodInterfaceKX:
            interface_type = DINO_INTF_TYPE_KX;
        break;
        case phymodInterfaceKR:
        case phymodInterfaceKR10:
        case phymodInterfaceKR4:
            interface_type = DINO_INTF_TYPE_KR;
        break;
        case phymodInterfaceSFI:
            interface_type = DINO_INTF_TYPE_SFI;
        break;
        case phymodInterfaceXLPPI:
            interface_type = DINO_INTF_TYPE_XLPPI;
        break;
        case phymodInterfaceCX:
            interface_type = DINO_INTF_TYPE_CX;
        break;
        case phymodInterfaceCR:
        case phymodInterfaceCR4:
        case phymodInterfaceCR10:
            interface_type = DINO_INTF_TYPE_CR;
        break;
        case phymodInterfaceVSR:
            interface_type = DINO_INTF_TYPE_VSR;
        break;
        case phymodInterfaceOTN:
            interface_type = DINO_INTF_TYPE_OTN;
        break;
        case phymodInterfaceSGMII:
            interface_type = DINO_INTF_TYPE_SGMII;
        break;
        default:
        break;
    }

    if ((config->data_rate == DINO_SPD_100G) ||
        (config->data_rate == DINO_SPD_106G) ||
        (config->data_rate == DINO_SPD_111G)) {
        lane_mask = 0x1;
    }

    for (lane_idx = 0; lane_idx < DINO_MAX_LANE; lane_idx++) {
        if (lane_mask & (1 << lane_idx)) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
            if (if_side == DINO_IF_LINE) {
                sw_gpreg_data &= ~(DINO_LINE_INTF_MASK);
                sw_gpreg_data |= (interface_type << DINO_LINE_INTF_SHIFT);
            } else {
                sw_gpreg_data &= ~(DINO_SYS_INTF_MASK);
                sw_gpreg_data |= (interface_type << DINO_SYS_INTF_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_SW_GPREG_0_ADR + lane_idx), sw_gpreg_data);
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config)
{
    uint16_t DFE_option  = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type  = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t port_idx  = 0;
    uint16_t no_of_ports = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    const phymod_access_t *pa = &phy->access;
    phymod_interface_t intf = config->interface_type; 
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE(config, chip_id, no_of_ports);

    if (intf == phymodInterfaceSR4 || intf == phymodInterfaceSR10 ||
        intf == phymodInterfaceER4 || intf == phymodInterfaceLR10 ||
        intf == phymodInterfaceLR  || intf == phymodInterfaceLR4 ||
        intf == phymodInterfaceSR  || intf == phymodInterfaceER  ||
        intf == phymodInterfaceSFI  || intf == phymodInterfaceXLPPI || intf == phymodInterfaceOTN) {
        if ((PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) &&
            (if_side == DINO_IF_LINE) &&
            (config->data_rate == DINO_SPD_111G)) {
            DFE_option = SERDES_DFE_OPTION_DFE;
        } else  {
            DFE_option = SERDES_DFE_OPTION_NO_DFE;
        }
        if (config->interface_modes & PHYMOD_INTF_MODES_UNRELIABLE_LOS) {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS; /*OPTICAL*/
        } else {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL*/
        }
    } else if (intf == phymodInterfaceCR4 || intf == phymodInterfaceCR ||
               intf == phymodInterfaceCR10 || intf == phymodInterfaceCX ||
               intf == phymodInterfaceSGMII) {
        media_type = SERDES_MEDIA_TYPE_COPPER_CABLE; /*Copper*/
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceKX || intf == phymodInterfaceKR4 || intf == phymodInterfaceKR ||
               intf == phymodInterfaceKR10) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceCAUI4 || intf == phymodInterfaceCAUI || 
               intf == phymodInterfaceCAUI4_C2M || intf == phymodInterfaceCAUI4_C2C ||
               intf == phymodInterfaceVSR) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE_LP_MODE;
    } else if (intf == phymodInterfaceXLAUI ||
               intf == phymodInterfaceXFI) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else {
         PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM,
                (_PHYMOD_MSG("Invalid interface type..")));
    }
 
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config->data_rate == DINO_SPD_100G) || 
            (config->data_rate == DINO_SPD_106G) || 
            (config->data_rate == DINO_SPD_111G)) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            /* Disable AN if it is enabled */
            if (gpreg_data & DINO_AN_MASK) {
                gpreg_data &= ~(DINO_AN_MASK);
            }
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            break;
        } else if (config->data_rate == DINO_SPD_40G || config->data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                /* Disable AN if it is enabled */
                if (gpreg_data & DINO_AN_MASK) {
                    gpreg_data &= ~(DINO_AN_MASK);
                }
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                break;
            } else {
                continue;
            }
        } else if ((config->data_rate == DINO_SPD_10G) ||
                   (config->data_rate == DINO_SPD_11G) ||
                   (config->data_rate == DINO_SPD_11P18G) ||
                   (config->data_rate == DINO_SPD_1G)) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                /* Disable AN if it is enabled */
                if (gpreg_data & DINO_AN_MASK) {
                    gpreg_data &= ~(DINO_AN_MASK);
                }
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                break;
            } else {
                continue;
            }
        } else {
            /* Throw error as un supported mode */
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    PHYMOD_IF_ERR_RETURN(
        _dino_save_interface(phy, config, intf));

    return PHYMOD_E_NONE;
}

int _dino_get_pll_modes(const phymod_phy_access_t *phy, phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *mer_pll_mode) 
{
    uint32_t speed = config->data_rate;
    uint32_t chip_id = 0;
    uint32_t rev = 0;

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(&phy->access, &chip_id, &rev));
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            /* No OTN support on 156 and 312 */
            if ((speed == DINO_SPD_111G) || (speed == DINO_SPD_11P18G)) {
                return PHYMOD_E_PARAM;
            }
            if (((speed == DINO_SPD_100G) || (speed == DINO_SPD_106G)) &&
                    (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy))) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config) && (speed == DINO_SPD_106G)) {
                    *fal_pll_mode = DINO_PLL_MODE_175;
                    *mer_pll_mode = DINO_PLL_MODE_70;
                } else {
                    *fal_pll_mode = DINO_PLL_MODE_165;
                    *mer_pll_mode = DINO_PLL_MODE_66;
                }
            } else {
                /*-- Falcon Line Rate is 10G*/
                if (PHYMOD_INTF_MODES_HIGIG_GET(config) && ((speed == DINO_SPD_11G) || (speed == DINO_SPD_42G) || (speed == DINO_SPD_106G))) {
                    *fal_pll_mode = DINO_PLL_MODE_140;
                    *mer_pll_mode = DINO_PLL_MODE_70;
                } else { /*IEEE MODE*/
                    *fal_pll_mode = DINO_PLL_MODE_132;
                    *mer_pll_mode = DINO_PLL_MODE_66;
                }
            }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            /* No OTN or HIGIG Support on 161/322/644 */
            if ((PHYMOD_INTF_MODES_HIGIG_GET(config)) ||
                ((speed == DINO_SPD_111G) || (speed == DINO_SPD_11P18G))) {
                return PHYMOD_E_PARAM;
            }
            if (speed == DINO_SPD_100G) {
                /*IEEE MODE*/
                if (PHYMOD_INTF_CONFIG_PHY_GEARBOX_ENABLE_GET(phy)) {
                    if ((ref_clk == phymodRefClk322Mhz) && (chip_id == DINO_CHIP_ID_82793)) {
                        *fal_pll_mode = DINO_PLL_MODE_80;
                        *mer_pll_mode = DINO_PLL_MODE_64;
                    } else {
                        *fal_pll_mode = DINO_PLL_MODE_160;
                        *mer_pll_mode = DINO_PLL_MODE_64;
                    }
                } else {
                    if ((ref_clk == phymodRefClk322Mhz) && (chip_id == DINO_CHIP_ID_82793)) {
                        *fal_pll_mode = DINO_PLL_MODE_64;
                        *mer_pll_mode = DINO_PLL_MODE_64;
                    } else {
                        *fal_pll_mode = DINO_PLL_MODE_128;
                        *mer_pll_mode = DINO_PLL_MODE_64;
                    }
                }
            } else {
                if ((ref_clk == phymodRefClk322Mhz) && (chip_id == DINO_CHIP_ID_82793)) {
                    *fal_pll_mode = DINO_PLL_MODE_64;
                    *mer_pll_mode = DINO_PLL_MODE_64;
                } else {
                    *fal_pll_mode = DINO_PLL_MODE_128;
                    *mer_pll_mode = DINO_PLL_MODE_64;
                }
            }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz: 
        case phymodRefClk698Mhz:
            if (speed == DINO_SPD_111G) {
                *fal_pll_mode = DINO_PLL_MODE_160;
                *mer_pll_mode = DINO_PLL_MODE_64;
            } else if (speed == DINO_SPD_11P18G) {
                *fal_pll_mode = DINO_PLL_MODE_128;
                *mer_pll_mode = DINO_PLL_MODE_64;
            } else {
                return PHYMOD_E_PARAM;
            }
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
/** Map Merlin PLL Division
 * @param pll_mode 
 * @return Merlin PLL mode mapped value
 */
int _dino_map_mer_pll_div(DINO_PLL_MODE_E pll_mode) 
{
    switch (pll_mode) {
        case DINO_PLL_MODE_64:
            return 1;
        case DINO_PLL_MODE_66:
            return 2;
        case DINO_PLL_MODE_70:
            return 3;
        default:
            PHYMOD_DEBUG_VERBOSE(("PLLMODE %d not supported\n", pll_mode));
            return PHYMOD_E_PARAM; 
    }
    return PHYMOD_E_NONE;
}

/**  PMD lock get 
 *    
 *    @param pa                 Pointer to phymod access structure
 *    @param if_side             interface side 0 - Line side 1- Sys side
 *    @param lane                lane number
 *    @param rx_pmd_locked        RX PMD locked
 *
 *    @return PHYMOD_E_NONE     successful function execution 
 */
int _dino_rx_pmd_lock_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *rx_pmd_locked) 
{
    uint8_t pmd_lock = 1;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_pmd_lock_status(pa, &pmd_lock));
        *rx_pmd_locked &= pmd_lock;
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_dino_pmd_lock_status(pa, &pmd_lock));
        *rx_pmd_locked &= pmd_lock;
    }

    return PHYMOD_E_NONE;
}

int _dino_get_rx_pmd_lock_status(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *link_sts)
{
    return _dino_rx_pmd_lock_get(pa, if_side, lane, link_sts);
}


int _dino_phy_status_dump(const phymod_access_t *pa, uint16_t if_side, uint16_t lane)
{
    uint8_t trace_mem[768];

    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("******* PHY status dump for PHY ID:%d ********\n", pa->addr));
    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("**** PHY status dump for interface side:%d ****\n", if_side));
    USR_PRINTF(("***********************************************\n"));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_core_config(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_core_state(pa));
        PHYMOD_DEBUG_VERBOSE(("Falcon Status dump\n"));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_state(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_display_lane_config(pa));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_event_log(pa, trace_mem, EVENT_LOG_HEX_AND_DECODED));
    } else {
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_core_config(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_core_state(pa));
        PHYMOD_DEBUG_VERBOSE(("Merlin Status dump\n"));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_state_hdr(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_state(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_display_lane_config(pa));
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_event_log(pa, trace_mem, EVENT_LOG_HEX_AND_DECODED));
    }

    return PHYMOD_E_NONE;
}

int _dino_loopback_set(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t data = 0;
    BCMI_DINO_FIFO_RST_FRC_CTL1r_t fifo_rst_frc_ln;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t fifo_rst_frcval_ln;
    BCMI_DINO_FIFO_RST_FRC_CTL0r_t fifo_rst_frc_sys;
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t fifo_rst_frcval_sys;
    PHYMOD_MEMSET(&fifo_rst_frc_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_ln, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_t));
    PHYMOD_MEMSET(&fifo_rst_frc_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRC_CTL0r_t));
    PHYMOD_MEMSET(&fifo_rst_frcval_sys, 0, sizeof(BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_t));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            _dino_falcon_lpbk_set(pa, if_side, lane, loopback, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(
            _dino_merlin_lpbk_set(pa, if_side, lane, loopback, enable));
    }
    /* Perform FIFO reset on ingress and egress path */
    /* Set force val to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
    data = fifo_rst_frcval_ln.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

    /* Set force to 1*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
    data = fifo_rst_frc_ln.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

    PHYMOD_USLEEP(50);
    /* Set force val to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL1r(pa, &fifo_rst_frcval_ln));
    data = fifo_rst_frcval_ln.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL1r_SET(fifo_rst_frcval_ln, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL1r(pa, fifo_rst_frcval_ln));

    /* Set force to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL1r(pa, &fifo_rst_frc_ln));
    data = fifo_rst_frc_ln.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL1r_SET(fifo_rst_frc_ln, data);

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL1r(pa, fifo_rst_frc_ln));

    /* Set force val to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
    data = fifo_rst_frcval_sys.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

    /* Set force to 1*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
    data = fifo_rst_frc_sys.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));
    PHYMOD_USLEEP(50);

    /* Set force val to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRCVAL_CTL0r(pa, &fifo_rst_frcval_sys));
    data = fifo_rst_frcval_sys.v[0];
    data |= (1 << lane);
    BCMI_DINO_FIFO_RST_FRCVAL_CTL0r_SET(fifo_rst_frcval_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRCVAL_CTL0r(pa, fifo_rst_frcval_sys));

    /* Set force to default*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_FIFO_RST_FRC_CTL0r(pa, &fifo_rst_frc_sys));
    data = fifo_rst_frc_sys.v[0];
    data &= ~(1 << lane);
    BCMI_DINO_FIFO_RST_FRC_CTL0r_SET(fifo_rst_frc_sys, data);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_FIFO_RST_FRC_CTL0r(pa, fifo_rst_frc_sys));

    return PHYMOD_E_NONE;
}

int _dino_loopback_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    PHYMOD_IF_ERR_RETURN(
        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            _dino_falcon_lpbk_get(pa, if_side, loopback, enable));
    } else {
        PHYMOD_IF_ERR_RETURN(
            _dino_merlin_lpbk_get(pa, if_side, loopback, enable));
    }
    PHYMOD_IF_ERR_RETURN(
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_falcon_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
    BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
    PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_t));
    PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_t));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
            *enable = BCMI_DINO_F25G_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Falcon Digital loopback get */
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_RX_DIG_LPBK_CFGr(pa, &dig_lpbk));
            *enable = BCMI_DINO_F25G_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk); 
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_merlin_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
    BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
    PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_t));
    PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_t));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
            *enable = BCMI_DINO_M10G_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Falcon Digital loopback get */
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_RX_DIG_LPBK_CFGr(pa, &dig_lpbk));
            *enable = BCMI_DINO_M10G_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk); 
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}


int _dino_falcon_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, 
                          phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t if_other_side = 0;
    uint16_t lane_index = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    uint16_t sw_gpreg_data    = 0;
    uint16_t other_side_start_lane = 0;
    uint16_t other_side_end_lane = 0;
    BCMI_DINO_DECD_MODE_STS0r_t mode_sts0;
    BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_t fal_tx_disable;

    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(BCMI_DINO_DECD_MODE_STS0r_t));
    PHYMOD_MEMSET(&fal_tx_disable, 0, sizeof(fal_tx_disable));
    (if_side == DINO_IF_LINE) ? (if_other_side = DINO_IF_SYS) : (if_other_side = DINO_IF_LINE); 

    PHYMOD_DEBUG_VERBOSE(("Falcon loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(falcon2_dino_rmt_lpbk(pa, enable));
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
            /* Read the GPREG to check if Gear box mode is set.
             * For GBOX mode , we need to program all system side lanes 
             */
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            if (dev_op_mode == DINO_DEV_OP_MODE_GBOX) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_DECD_MODE_STS0r(pa, &mode_sts0));
                if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_2TO11f_GET(mode_sts0)) {
                    other_side_start_lane = 2;
                    other_side_end_lane = 11;
                } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_1TO10f_GET(mode_sts0)) {
                    other_side_start_lane = 1;
                    other_side_end_lane = 10;
                } else if (BCMI_DINO_DECD_MODE_STS0r_DECODED_MODE_100G_GBOX_0TO9f_GET(mode_sts0)) {
                    other_side_start_lane = 0;
                    other_side_end_lane = 9;
                } else {
                    other_side_start_lane = 0;
                    other_side_end_lane = 9;
                }
                for (lane_index = other_side_start_lane; lane_index <= other_side_end_lane; lane_index++) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane_index));
                    PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            } else {
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
            }
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
            /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
            PHYMOD_IF_ERR_RETURN(falcon2_dino_wr_prbs_chk_en_auto_mode((!enable)));
            /* TX disable while enabling digital loopback */
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_tx_disable));
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(fal_tx_disable, 1);
            BCMI_DINO_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(fal_tx_disable, enable);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, fal_tx_disable));
            /*Tx Disable ends*/
         PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_merlin_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t if_other_side = 0;
    uint16_t other_side_start_lane = 0;
    uint16_t other_side_end_lane = 0;
    uint16_t sw_gpreg_data = 0;
    uint16_t lane_index = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;

    (if_side == DINO_IF_LINE) ? (if_other_side = DINO_IF_SYS) : (if_other_side = DINO_IF_LINE);

    PHYMOD_DEBUG_VERBOSE(("Merlin loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD:  
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(merlin_dino_rmt_lpbk(pa, enable));
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: 
            /* Read the GPREG to check if Gear box mode is set.
             * For GBOX mode , we need to program only 4 line side lanes 
             */
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            if ((if_side == DINO_IF_SYS) && (dev_op_mode == DINO_DEV_OP_MODE_GBOX)) {
                other_side_start_lane = 0;
                other_side_end_lane = 3;
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                for (lane_index = other_side_start_lane; lane_index <= other_side_end_lane; lane_index++) {
                    PHYMOD_IF_ERR_RETURN(
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane_index));
                    PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            } else {
                PHYMOD_IF_ERR_RETURN(
                    _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_other_side, lane));
                if ((if_side == DINO_IF_SYS) && (lane < DINO_MAX_CORE_LANE)) {
                    PHYMOD_IF_ERR_RETURN(falcon2_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                } else {
                    PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            }
            PHYMOD_IF_ERR_RETURN(
                _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
            PHYMOD_IF_ERR_RETURN(merlin_dino_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
            /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
            PHYMOD_IF_ERR_RETURN(wr_prbs_chk_en_auto_mode((!enable)));
            /* TX disable while enabling digital loopback on the line side*/
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN(wr_sdk_tx_disable(enable));
            }
        break;
        default :
        break;
    }
    PHYMOD_IF_ERR_RETURN(
      _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}

int _dino_phy_polarity_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t tx_polarity, uint16_t rx_polarity)
{
    BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_t fal_rx_pol;
    BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_t fal_tx_pol;
    BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_t mer_rx_pol;
    BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_t mer_tx_pol;
   
    PHYMOD_MEMSET(&fal_rx_pol, 0, sizeof(fal_rx_pol));
    PHYMOD_MEMSET(&fal_tx_pol, 0, sizeof(fal_tx_pol));
    PHYMOD_MEMSET(&mer_tx_pol, 0, sizeof(mer_tx_pol));
    PHYMOD_MEMSET(&mer_rx_pol, 0, sizeof(mer_rx_pol));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (tx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, &fal_tx_pol));
            BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(fal_tx_pol, tx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, fal_tx_pol));
        }
        if (rx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, &fal_rx_pol));
            BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(fal_rx_pol, rx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, fal_rx_pol));
        }
    } else {
        if (rx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, &mer_rx_pol));
            BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(mer_rx_pol, rx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, mer_rx_pol));
        }
        if (tx_polarity != 0xFFFF) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, &mer_tx_pol));
            BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(mer_tx_pol, tx_polarity);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, mer_tx_pol));
         }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_polarity_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t *tx_polarity, uint16_t *rx_polarity)
{
    BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_t fal_rx_pol;
    BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_t fal_tx_pol;
    BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_t mer_rx_pol;
    BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_t mer_tx_pol;
   
    PHYMOD_MEMSET(&fal_rx_pol, 0, sizeof(fal_rx_pol));
    PHYMOD_MEMSET(&fal_tx_pol, 0, sizeof(fal_tx_pol));
    PHYMOD_MEMSET(&mer_tx_pol, 0, sizeof(mer_tx_pol));
    PHYMOD_MEMSET(&mer_rx_pol, 0, sizeof(mer_rx_pol));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_F25G_TLB_TX_TLB_TX_MISC_CFGr(pa, &fal_tx_pol));
        *tx_polarity = BCMI_DINO_F25G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(fal_tx_pol);
        
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_F25G_TLB_RX_TLB_RX_MISC_CFGr(pa, &fal_rx_pol));
        *rx_polarity = BCMI_DINO_F25G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(fal_rx_pol);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_M10G_TLB_RX_TLB_RX_MISC_CFGr(pa, &mer_rx_pol));
        *rx_polarity = BCMI_DINO_M10G_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(mer_rx_pol);
            
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_READ_M10G_TLB_TX_TLB_TX_MISC_CFGr(pa, &mer_tx_pol));
        *tx_polarity = BCMI_DINO_M10G_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(mer_tx_pol);
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_fec_set(const phymod_phy_access_t* phy, uint16_t enable)
{
    BCMI_DINO_MODECTL0r_t modectrl0;

    PHYMOD_MEMSET(&modectrl0, 0, sizeof(modectrl0));

    /* this may need to modify in AN mode*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(&phy->access, &modectrl0));
    BCMI_DINO_MODECTL0r_MODE_CL91_ENf_SET(modectrl0, enable);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODECTL0r(&phy->access, modectrl0));

    PHYMOD_IF_ERR_RETURN(
        _dino_fw_enable(&phy->access));

    return PHYMOD_E_NONE;
}

int _dino_phy_fec_get(const phymod_phy_access_t* phy, uint32_t *enable)
{
    BCMI_DINO_MODECTL0r_t modectrl0;

    PHYMOD_MEMSET(&modectrl0, 0, sizeof(modectrl0));

    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MODECTL0r(&phy->access, &modectrl0));
    *enable = BCMI_DINO_MODECTL0r_MODE_CL91_ENf_GET(modectrl0);

    return PHYMOD_E_NONE;
}

int _dino_phy_power_set(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, const phymod_phy_power_t* power)
{
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0, sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0, sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (power->tx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
            if (power->tx == phymodPowerOff) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,1);
            } else if (power->tx == phymodPowerOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 0);
            } else if (power->tx == phymodPowerOffOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(fal_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
        }
        if (power->rx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
            if (power->rx == phymodPowerOff) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,1);
            } else if (power->rx == phymodPowerOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,0);
            } else if (power->rx == phymodPowerOffOn) {
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
                BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, fal_power));
        }
    } else {
        if (power->tx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
            if (power->tx == phymodPowerOff) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,1);
            } else if (power->tx == phymodPowerOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 0);
            } else if (power->tx == phymodPowerOffOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_SET(mer_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
        }
        if (power->rx != phymodPowerNoChange) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
            if (power->rx == phymodPowerOff) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,1);
            } else if (power->rx == phymodPowerOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,0);
            } else if (power->rx == phymodPowerOffOn) {
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 1);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,1);
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
                PHYMOD_USLEEP(10);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 0);
                BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,0);
            }
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, mer_power));
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_power_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_phy_power_t* power)
{
    uint16_t force = 0, pwr = 0;
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &fal_power));
        force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_GET(fal_power);
        pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_GET(fal_power);
        if (force && pwr) {
            power->tx = phymodPowerOff;
        } else {
            power->tx = phymodPowerOn;
        }
        force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(fal_power);
        pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(fal_power);
        if (force && pwr) {
            power->rx = phymodPowerOff;
        } else {
            power->rx = phymodPowerOn;
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(&phy->access, &mer_power));
        force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCf_GET(mer_power);
        pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_TX_H_PWRDN_FRCVALf_GET(mer_power);
        if (force && pwr) {
            power->tx = phymodPowerOff;
        } else {
            power->tx = phymodPowerOn;
        }
        force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(mer_power);
        pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(mer_power);
        if (force && pwr) {
            power->rx = phymodPowerOff;
        } else {
            power->rx = phymodPowerOn;
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_reset_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_phy_reset_t* reset)
{
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (reset->tx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(0));
        } else if (reset->tx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(1));
        } else if (reset->tx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(
                    falcon2_dino_wr_ln_tx_dp_s_rstb(1));
        }
        if (reset->rx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(0));
        } else if (reset->rx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(1));
        } else if (reset->rx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(
               falcon2_dino_wr_ln_rx_dp_s_rstb(1));

        }
    } else {
        if (reset->tx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(0));
        } else if (reset->tx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(1));
        } else if (reset->tx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_ln_tx_dp_s_rstb(1));
        }
        if (reset->rx == phymodResetDirectionIn) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(0));
        } else if (reset->rx == phymodResetDirectionOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(1));
        } else if (reset->rx == phymodResetDirectionInOut) {
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(0));
            PHYMOD_USLEEP(10);
            PHYMOD_IF_ERR_RETURN(wr_ln_rx_dp_s_rstb(1));
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_reset_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_reset_t* reset)
{
    uint16_t dp_reset = 0,__err = 0 ;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        dp_reset = falcon2_dino_rd_ln_tx_dp_s_rstb();
        if (!dp_reset) {
            reset->tx = phymodResetDirectionIn; 
        } else {
            reset->tx = phymodResetDirectionOut; 
        }
        dp_reset = falcon2_dino_rd_ln_rx_dp_s_rstb(); 
        if (!dp_reset) {
            reset->rx = phymodResetDirectionIn; 
        } else {
            reset->rx = phymodResetDirectionOut; 
        }
    } else {
        dp_reset = rd_ln_tx_dp_s_rstb();
        if (!dp_reset) {
            reset->tx = phymodResetDirectionIn; 
        } else {
            reset->tx = phymodResetDirectionOut; 
        }
        dp_reset = rd_ln_rx_dp_s_rstb();
        if (!dp_reset) {
            reset->rx = phymodResetDirectionIn; 
        } else {
            reset->rx = phymodResetDirectionOut; 
        }
    }

    return PHYMOD_E_NONE;
}

/*Start\Stop the sequencer*/
int _dino_core_pll_sequencer_restart(const phymod_access_t* pa, uint16_t if_side, 
                                     phymod_sequencer_operation_t operation) 
{
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    BCMI_DINO_F25G_PLL_CAL_CTL4r_t fal_pll_ctrl;
    BCMI_DINO_PLL_CALCTL5r_t mer_pll_ctrl;
    BCMI_DINO_F25G_CORE_PLL_TOP_USER_CTLr_t fal_pll_top_usr_ctrl;
    BCMI_DINO_RST_CTL_CORE_DPr_t mer_rst_ctl_core_dp;

    PHYMOD_MEMSET(&fal_pll_ctrl, 0, sizeof(BCMI_DINO_F25G_PLL_CAL_CTL4r_t));
    PHYMOD_MEMSET(&mer_pll_ctrl, 0, sizeof(BCMI_DINO_PLL_CALCTL5r));
    PHYMOD_MEMSET(&fal_pll_top_usr_ctrl, 0, sizeof(BCMI_DINO_F25G_CORE_PLL_TOP_USER_CTLr_t));
    PHYMOD_MEMSET(&mer_rst_ctl_core_dp, 0, sizeof(BCMI_DINO_RST_CTL_CORE_DPr_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    if (lane_mask == 0) {
        lane_mask = DINO_100G_TYPE1_LANE_MASK;
    }

    for (lane = 0; lane < DINO_MAX_LANE ; lane += DINO_MAX_CORE_LANE) {
        if (lane_mask & (DINO_40G_PORT0_LANE_MASK << lane)) {
            switch (operation) {
                case phymodSeqOpStart:
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_F25G_PLL_CAL_CTL4r(pa, &fal_pll_ctrl));
                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_PLL_CALCTL5r(pa, &mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 1);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                    }
                break;
                case phymodSeqOpStop:
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_F25G_PLL_CAL_CTL4r(pa, &fal_pll_ctrl));
                        BCMI_DINO_F25G_PLL_CAL_CTL4r_PLL_SEQ_STARTf_SET(fal_pll_ctrl,0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_F25G_PLL_CAL_CTL4r(pa, fal_pll_ctrl));
                    } else {
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_READ_PLL_CALCTL5r(pa, &mer_pll_ctrl));
                        BCMI_DINO_PLL_CALCTL5r_PLL_SEQ_STARTf_SET(mer_pll_ctrl, 0);
                        PHYMOD_IF_ERR_RETURN(
                                BCMI_DINO_WRITE_PLL_CALCTL5r(pa, mer_pll_ctrl));
                    }

                    break;
                case phymodSeqOpRestart: 
                    PHYMOD_IF_ERR_RETURN (
                        _dino_set_slice_reg (pa, DINO_SLICE_UNICAST, if_side, lane));
                    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_F25G_CORE_PLL_TOP_USER_CTLr(pa, &fal_pll_top_usr_ctrl));
                        BCMI_DINO_F25G_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(fal_pll_top_usr_ctrl, 0);
                        PHYMOD_IF_ERR_RETURN (
                            BCMI_DINO_WRITE_F25G_CORE_PLL_TOP_USER_CTLr(pa, fal_pll_top_usr_ctrl));
                        PHYMOD_USLEEP(1000);
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_F25G_CORE_PLL_TOP_USER_CTLr(pa, &fal_pll_top_usr_ctrl));
                        BCMI_DINO_F25G_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(fal_pll_top_usr_ctrl, 1);
                        PHYMOD_IF_ERR_RETURN (
                            BCMI_DINO_WRITE_F25G_CORE_PLL_TOP_USER_CTLr(pa, fal_pll_top_usr_ctrl));

                    } else {
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_RST_CTL_CORE_DPr(pa, &mer_rst_ctl_core_dp));
                        BCMI_DINO_RST_CTL_CORE_DPr_CORE_DP_S_RSTBf_SET(mer_rst_ctl_core_dp, 0);
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_WRITE_RST_CTL_CORE_DPr(pa, mer_rst_ctl_core_dp));
                        PHYMOD_USLEEP(1000);
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_RST_CTL_CORE_DPr(pa, &mer_rst_ctl_core_dp));
                        BCMI_DINO_RST_CTL_CORE_DPr_CORE_DP_S_RSTBf_SET(mer_rst_ctl_core_dp, 1);
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_WRITE_RST_CTL_CORE_DPr(pa, mer_rst_ctl_core_dp));
                    }

                break;
                default:
                    return PHYMOD_E_PARAM;
            }
        }
    }
    PHYMOD_IF_ERR_RETURN (
        _dino_set_slice_reg (pa, DINO_SLICE_RESET, DINO_IF_LINE, 0));

    return PHYMOD_E_NONE;
}


/*TX transmission control*/
int _dino_phy_tx_lane_control_set(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_tx_lane_control_t tx_control)
{
    BCMI_DINO_LN_DP_RST_CTL1r_t ln_dp_rst_ctrl1;
    BCMI_DINO_LN_DP_RST_CTL2r_t ln_dp_rst_ctrl2;
    PHYMOD_MEMSET(&ln_dp_rst_ctrl1, 0, sizeof(BCMI_DINO_LN_DP_RST_CTL1r_t));
    PHYMOD_MEMSET(&ln_dp_rst_ctrl2, 0, sizeof(BCMI_DINO_LN_DP_RST_CTL2r_t));

    if (if_side == DINO_IF_SYS) {
        if (tx_control == phymodTxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                    wr_sdk_tx_disable(0x1));
        } else if (tx_control == phymodTxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                    wr_sdk_tx_disable(0));
        } else if (tx_control == phymodTxReset) {
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL2r(pa, &ln_dp_rst_ctrl2)); 
                    BCMI_DINO_LN_DP_RST_CTL2r_MUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl2, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL2r(pa, ln_dp_rst_ctrl2));
            } else {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL1r(pa, &ln_dp_rst_ctrl1)); 
                    BCMI_DINO_LN_DP_RST_CTL1r_DEMUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl1, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL1r(pa, ln_dp_rst_ctrl1));
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        if (tx_control == phymodTxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                    falcon2_dino_wr_sdk_tx_disable(0x1));
        } else if (tx_control == phymodTxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                    falcon2_dino_wr_sdk_tx_disable(0));
        } else if (tx_control == phymodTxReset) {
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL2r(pa, &ln_dp_rst_ctrl2)); 
                    BCMI_DINO_LN_DP_RST_CTL2r_MUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl2, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL2r(pa, ln_dp_rst_ctrl2));
            } else {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL1r(pa, &ln_dp_rst_ctrl1)); 
                    BCMI_DINO_LN_DP_RST_CTL1r_DEMUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl1, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL1r(pa, ln_dp_rst_ctrl1));
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_tx_lane_control_get(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_tx_lane_control_t* tx_control)
{
    err_code_t __err=ERR_CODE_NONE;
    uint16_t tx_disable_l=0;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (*tx_control == phymodTxSquelchOn || *tx_control == phymodTxSquelchOff) {
            tx_disable_l=falcon2_dino_rd_sdk_tx_disable();
            if(__err==ERR_CODE_NONE){
                if (tx_disable_l) {
                    *tx_control = phymodTxSquelchOn;
                }else {
                    *tx_control = phymodTxSquelchOff;
                }
            }else{
                return PHYMOD_E_FAIL;
            }
        }else{
             return PHYMOD_E_UNAVAIL;
        }
    } else {
        if (*tx_control == phymodTxSquelchOn || *tx_control == phymodTxSquelchOff) {
            tx_disable_l=rd_sdk_tx_disable();
            if(__err==ERR_CODE_NONE){
                if (tx_disable_l) {
                    *tx_control = phymodTxSquelchOn;
                } else {
                    *tx_control = phymodTxSquelchOff;
                }
            }else{
                return PHYMOD_E_FAIL;
            }
        }else{
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;
}

/*Rx control*/
int _dino_phy_rx_lane_control_set(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_rx_lane_control_t rx_control)
{
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    BCMI_DINO_LN_DP_RST_CTL1r_t ln_dp_rst_ctrl1;
    BCMI_DINO_LN_DP_RST_CTL2r_t ln_dp_rst_ctrl2;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));
    PHYMOD_MEMSET(&ln_dp_rst_ctrl1, 0, sizeof(BCMI_DINO_LN_DP_RST_CTL1r_t));
    PHYMOD_MEMSET(&ln_dp_rst_ctrl2, 0, sizeof(BCMI_DINO_LN_DP_RST_CTL2r_t));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        if (rx_control == phymodRxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 0);
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, fal_power));

        } else if (rx_control == phymodRxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(fal_power, 1);
            BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(fal_power, 1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, fal_power));

        } else if (rx_control == phymodRxReset) {
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL1r(pa, &ln_dp_rst_ctrl1)); 
                    BCMI_DINO_LN_DP_RST_CTL1r_DEMUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl1, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL1r(pa, ln_dp_rst_ctrl1));
            } else {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL2r(pa, &ln_dp_rst_ctrl2)); 
                    BCMI_DINO_LN_DP_RST_CTL2r_MUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl2, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL2r(pa, ln_dp_rst_ctrl2));
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        if (rx_control == phymodRxSquelchOff) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,0);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 0);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, mer_power));
        } else if (rx_control == phymodRxSquelchOn) {
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_SET(mer_power, 1);
            BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_SET(mer_power,1);
            PHYMOD_IF_ERR_RETURN (
                BCMI_DINO_WRITE_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, mer_power));
        } else if (rx_control == phymodRxReset) {
            if (if_side == DINO_IF_LINE) {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL1r(pa, &ln_dp_rst_ctrl1)); 
                    BCMI_DINO_LN_DP_RST_CTL1r_DEMUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl1, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL1r(pa, ln_dp_rst_ctrl1));
            } else {
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_READ_LN_DP_RST_CTL2r(pa, &ln_dp_rst_ctrl2)); 
                    BCMI_DINO_LN_DP_RST_CTL2r_MUX_ILANE_DP_RSTBf_SET(ln_dp_rst_ctrl2, (1 << lane));
                PHYMOD_IF_ERR_RETURN (
                    BCMI_DINO_WRITE_LN_DP_RST_CTL2r(pa, ln_dp_rst_ctrl2));
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_rx_lane_control_get(const phymod_access_t* pa, uint16_t if_side,
                                  uint16_t lane, phymod_phy_rx_lane_control_t* rx_control)
{
    uint16_t force = 0, pwr = 0;
    BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_t mer_power;
    BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_t fal_power;
    PHYMOD_MEMSET(&mer_power, 0 , sizeof(mer_power));
    PHYMOD_MEMSET(&fal_power, 0 , sizeof(fal_power));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_FALCON_IF_PER_LN_CTL2_PER_LANEr(pa, &fal_power));
        if (*rx_control == phymodRxSquelchOn || *rx_control == phymodRxSquelchOff) {
            force = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(fal_power);
            pwr = BCMI_DINO_FALCON_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(fal_power);
            if (force && pwr) {
                *rx_control = phymodRxSquelchOn;
            } else {
                *rx_control = phymodRxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_MERLIN_IF_PER_LN_CTL2_PER_LANEr(pa, &mer_power));
        if (*rx_control == phymodRxSquelchOn || *rx_control == phymodRxSquelchOff) {
            force = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCf_GET(mer_power);
            pwr = BCMI_DINO_MERLIN_IF_PER_LN_CTL2_PER_LANEr_PMD_LN_RX_H_PWRDN_FRCVALf_GET(mer_power);
            if (force && pwr) {
                *rx_control = phymodRxSquelchOn;
            } else {
                *rx_control = phymodRxSquelchOff;
            }
        } else {
            return PHYMOD_E_UNAVAIL;
        }
    }

    return PHYMOD_E_NONE;
}

/*Set\Get TX Parameters*/
int _dino_phy_tx_set(const phymod_access_t* pa, uint16_t if_side,
                     uint16_t lane, const phymod_tx_t* tx) 
{
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR set\n"));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_PRE, (int8_t)tx->pre));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_MAIN, (int8_t)tx->main));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_POST1, (int8_t)tx->post));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_POST2, (int8_t)tx->post2));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_POST3, (int8_t)tx->post3));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_write_tx_afe(pa, TX_AFE_AMP, (int8_t)tx->amp));
    } else {
        PHYMOD_DEBUG_VERBOSE(("Merlin TXFIR set\n"));
        PHYMOD_IF_ERR_RETURN(merlin_dino_apply_txfir_cfg(pa, tx->pre, tx->main, tx->post, tx->post2));
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_tx_get(const phymod_access_t* pa, uint16_t if_side,
                     uint16_t lane, phymod_tx_t* tx)
{
    int8_t value = 0;
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR get\n"));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_PRE, &value));
        tx->pre = value;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_MAIN, &value));
        tx->main = value;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_POST1, &value));
        tx->post = value;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_POST2, &value));
        tx->post2 = value;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_POST3, &value));
        tx->post3 = value;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_tx_afe(pa, TX_AFE_AMP, &value));
        tx->amp = value;
    } else {
        PHYMOD_DEBUG_VERBOSE(("Merlin TXFIR get\n"));
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_tx_afe(pa, TX_AFE_PRE, &value));
        tx->pre = value;
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_tx_afe(pa, TX_AFE_MAIN, &value));
        tx->main = value;
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_tx_afe(pa, TX_AFE_POST1, &value));
        tx->post = value;
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_tx_afe(pa, TX_AFE_POST2, &value));
        tx->post2 = value;
    }
    return PHYMOD_E_NONE;
}

/*Set\Get RX Parameters*/
int _dino_phy_rx_set(const phymod_access_t* pa, uint16_t if_side,
                     uint16_t lane, const phymod_rx_t* rx)
{
    uint16_t indx = 0;
    uint8_t MAX_MERLIN_DFES_TAPS = 5;
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        /* FALCON RXFIR set */
        PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR set\n"));

        /*params check*/
        for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
            if(rx->dfe[indx].enable && (rx->num_of_dfe_taps > PHYMOD_NUM_DFE_TAPS)) {
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
            }
        }

        /*vga set*/
        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(falcon2_dino_stop_rx_adaptation(pa, 1));
        if (rx->vga.enable) {
            PHYMOD_IF_ERR_RETURN(falcon2_dino_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value));
        }
        /*dfe set*/
        for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
            if(rx->dfe[indx].enable){
                PHYMOD_IF_ERR_RETURN(falcon2_dino_write_rx_afe(pa, RX_AFE_DFE1+indx, rx->dfe[indx].value));
            }
        }

        /*peaking filter set*/
        if(rx->peaking_filter.enable){
            PHYMOD_IF_ERR_RETURN(falcon2_dino_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value));
        }

        if(rx->low_freq_peaking_filter.enable){
            PHYMOD_IF_ERR_RETURN(falcon2_dino_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
                }
    } else {
        /* MERLIN RXFIR set*/
        PHYMOD_DEBUG_VERBOSE(("Merlin RXFIR set\n"));

        /*params check*/
        for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
            if(rx->dfe[indx].enable && (rx->num_of_dfe_taps > MAX_MERLIN_DFES_TAPS)) {
                PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
            }
        }

        /* first stop the rx adaption */
        PHYMOD_IF_ERR_RETURN(merlin_dino_stop_rx_adaptation(pa, 1));
        /*vga set*/
        if (rx->vga.enable) {
            PHYMOD_IF_ERR_RETURN(merlin_dino_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value));
        }
        /*dfe set*/
        /* PHYMOD_NUM_DFE_TAPS are 14 but MERLING core has 5 only DFEs */  
        for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
            if(rx->dfe[indx].enable){
                PHYMOD_IF_ERR_RETURN(merlin_dino_write_rx_afe(pa, RX_AFE_DFE1+indx, rx->dfe[indx].value));
            }
        }

        /*peaking filter set*/
        if(rx->peaking_filter.enable){
            PHYMOD_IF_ERR_RETURN(merlin_dino_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value));
        }

        if(rx->low_freq_peaking_filter.enable){
            PHYMOD_IF_ERR_RETURN(merlin_dino_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
        }
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_rx_get(const phymod_access_t* pa, uint16_t if_side,
                     uint16_t lane, phymod_rx_t* rx)
{
    uint16_t indx = 0;
    int8_t dfe = 0;
    int8_t vga = 0;
    int8_t pf = 0;
    int8_t low_freq_pf = 0;
    uint32_t rx_adaptation = 0;
    int8_t MAX_MERLIN_DFES_TAPS = 5;

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        /* FALCON RXFIR get */
        PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR get\n"));

        rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
        PHYMOD_IF_ERR_RETURN(falcon2_dino_stop_rx_adaptation(pa, 1));

        /*vga get*/
        rx->num_of_dfe_taps = PHYMOD_NUM_DFE_TAPS;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_rx_afe(pa, RX_AFE_VGA, &vga));
        rx->vga.value = vga; 
        rx->vga.enable = 1;

        /*dfe get*/
        for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
            PHYMOD_IF_ERR_RETURN(falcon2_dino_read_rx_afe(pa, (RX_AFE_DFE1+indx), &dfe));
            rx->dfe[indx].value = dfe;
            rx->dfe[indx].enable = 1;
        }

        /*peaking filter get*/
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_rx_afe(pa, RX_AFE_PF, &pf));
        rx->peaking_filter.value = pf;
        rx->peaking_filter.enable = 1;
        PHYMOD_IF_ERR_RETURN(falcon2_dino_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf));
        rx->low_freq_peaking_filter.value = low_freq_pf;
        rx->low_freq_peaking_filter.enable = 1;

        if (rx_adaptation) {
            PHYMOD_IF_ERR_RETURN(falcon2_dino_stop_rx_adaptation(pa, 0));
        }
    } else {
        /* MERLIN RXFIR get */
        PHYMOD_DEBUG_VERBOSE(("Merlin RXFIR get\n"));

        rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
        PHYMOD_IF_ERR_RETURN(merlin_dino_stop_rx_adaptation(pa, 1));

        /*vga get*/
        /* PHYMOD_NUM_DFE_TAPS are 14 but MERLING has only 5 DFEs */  
        rx->num_of_dfe_taps = MAX_MERLIN_DFES_TAPS;
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_rx_afe(pa, RX_AFE_VGA, &vga));
        rx->vga.value = vga;
        rx->vga.enable = 1;

        /*dfe get*/
        for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
            PHYMOD_IF_ERR_RETURN(merlin_dino_read_rx_afe(pa, (RX_AFE_DFE1+indx), &dfe));
            rx->dfe[indx].value = dfe;
            rx->dfe[indx].enable = 1;
        }

        /*peaking filter get*/
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_rx_afe(pa, RX_AFE_PF, &pf));
        rx->peaking_filter.value = pf;
        rx->peaking_filter.enable = 1;
        PHYMOD_IF_ERR_RETURN(merlin_dino_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf));
        rx->low_freq_peaking_filter.value = low_freq_pf;
        rx->low_freq_peaking_filter.enable = 1;

        if (rx_adaptation) {
            PHYMOD_IF_ERR_RETURN(merlin_dino_stop_rx_adaptation(pa, 0));
        }
    }
    return PHYMOD_E_NONE;
}

/*PHY Rx adaptation resume*/
int _dino_phy_rx_adaptation_resume(const phymod_access_t* pa, uint16_t if_side, uint16_t lane)
{
    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_DEBUG_VERBOSE(("Falcon RX RESUME set\n"));
        PHYMOD_IF_ERR_RETURN(falcon2_dino_stop_rx_adaptation(pa, 0));
    } else {
        PHYMOD_DEBUG_VERBOSE(("Merlin RX RESUME set\n"));
        PHYMOD_IF_ERR_RETURN(merlin_dino_stop_rx_adaptation(pa, 0));
    }
    return PHYMOD_E_NONE;
}

int _dino_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en)
{
    phymod_phy_inf_config_t config;
    uint16_t no_of_ports = 0;
    uint32_t flags = 0;
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t port_idx         = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    phymod_phy_access_t phy_copy;
    BCMI_DINO_FIRMWARE_FEATURESr_t firmware_features;
    const phymod_access_t *pa = &phy_copy.access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&firmware_features, 0, sizeof(BCMI_DINO_FIRMWARE_FEATURESr_t));
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config.data_rate == DINO_SPD_100G || config.data_rate == DINO_SPD_106G || config.data_rate == DINO_SPD_111G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_TX_TRAINING_MASK);
                gpreg_data |= (cl72_en << DINO_LINE_TX_TRAINING_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_TX_TRAINING_MASK);
                gpreg_data |= (cl72_en << DINO_SYS_TX_TRAINING_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            if ((config.data_rate == DINO_SPD_106G) &&
                (config.interface_type == phymodInterfaceCR4)) {
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_FIRMWARE_FEATURESr(pa, &firmware_features));
                firmware_features.v[0] &= ~(DINO_FRC_TRAINING_DELAY_MASK);
                firmware_features.v[0] |= (DINO_FRC_TRAINING_DELAY_6SEC << DINO_FRC_TRAINING_DELAY_SHIFT);
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_FIRMWARE_FEATURESr(pa, firmware_features));
            }
            break;
        } else if (config.data_rate == DINO_SPD_40G || config.data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_TX_TRAINING_MASK);
                    gpreg_data |= (cl72_en << DINO_LINE_TX_TRAINING_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_TX_TRAINING_MASK);
                    gpreg_data |= (cl72_en << DINO_SYS_TX_TRAINING_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                break;
            } else {
                continue;
            }
        } else if (config.data_rate == DINO_SPD_10G || config.data_rate == DINO_SPD_11G || config.data_rate == DINO_SPD_11P18G || config.data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_TX_TRAINING_MASK);
                    gpreg_data |= (cl72_en << DINO_LINE_TX_TRAINING_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_TX_TRAINING_MASK);
                    gpreg_data |= (cl72_en << DINO_SYS_TX_TRAINING_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                break;
            } else {
                continue;
            }
        } else {
            /* Throw error as un supported mode */
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }
    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _dino_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en)
{
    phymod_phy_inf_config_t config;
    uint16_t no_of_ports = 0;
    uint32_t flags = 0;
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t port_idx         = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    phymod_phy_access_t phy_copy;
    const phymod_access_t *pa = &phy_copy.access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if (config.data_rate == DINO_SPD_100G || config.data_rate == DINO_SPD_106G || config.data_rate == DINO_SPD_111G) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            break;
        } else if (config.data_rate == DINO_SPD_40G || config.data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                break;
            } else {
                continue;
            }
        } else if (config.data_rate == DINO_SPD_10G || config.data_rate == DINO_SPD_11G || config.data_rate == DINO_SPD_11P18G || config.data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                break;
            }
        } else {
            /* Throw error as un supported mode */
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    if (if_side == DINO_IF_LINE) {
        *cl72_en = (((gpreg_data & DINO_LINE_TX_TRAINING_MASK) >> DINO_LINE_TX_TRAINING_SHIFT) & 0x1);
    } else {
        *cl72_en = ((gpreg_data & DINO_SYS_TX_TRAINING_MASK) >> DINO_SYS_TX_TRAINING_SHIFT);
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_cl72_status_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_cl72_status_t* status)
{
    uint32_t cl72_en = 0;
    BCMI_DINO_CL72_STS1r_t merlin_cl72_sts;
    BCMI_DINO_CL93N72_UR_CTL0r_t falcon_cl72_sts;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&merlin_cl72_sts, 0, sizeof(BCMI_DINO_CL72_STS1r_t));
    PHYMOD_MEMSET(&falcon_cl72_sts, 0, sizeof(BCMI_DINO_CL93N72_UR_CTL0r_t));

    if (DINO_IS_FALCON_SIDE(if_side, lane)) {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_CL93N72_UR_CTL0r(pa, &falcon_cl72_sts));
        status->locked &=  BCMI_DINO_CL93N72_UR_CTL0r_CL93N72_RX_SIGNAL_OKf_GET(falcon_cl72_sts);
    } else {
        PHYMOD_IF_ERR_RETURN (
            BCMI_DINO_READ_CL72_STS1r(pa, &merlin_cl72_sts));
        status->locked &= BCMI_DINO_CL72_STS1r_CL72_SIGNAL_DETECTf_GET(merlin_cl72_sts);
    }

    PHYMOD_IF_ERR_RETURN
        (_dino_phy_cl72_get(phy, &cl72_en));

    status->enabled &= cl72_en;
    status->locked &= cl72_en;

    return PHYMOD_E_NONE;
}

int _dino_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    phymod_phy_inf_config_t config;
    uint16_t no_of_ports = 0;
    uint32_t flags = 0;
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t media_type  = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t DFE_option  = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t port_idx         = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    phymod_phy_access_t phy_copy;
    const phymod_access_t *pa = &phy_copy.access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);


    if (fw_config.DfeOn == 0){
        DFE_option =  SERDES_DFE_OPTION_NO_DFE;
    }
    if (fw_config.DfeOn == 1 && 
        fw_config.LpDfeOn == 0  &&
        fw_config.ForceBrDfe == 0) {
        DFE_option =  SERDES_DFE_OPTION_DFE;
    }
    if (fw_config.DfeOn == 1 && 
        fw_config.LpDfeOn == 0  &&
        fw_config.ForceBrDfe == 1) {
        DFE_option =  SERDES_DFE_OPTION_BRDFE;
    }
    if (fw_config.DfeOn == 1 && 
        fw_config.LpDfeOn == 1  &&
        fw_config.ForceBrDfe == 0) {
        DFE_option =  SERDES_DFE_OPTION_DFE_LP_MODE;
    }

    switch(fw_config.MediaType) {
        case phymodFirmwareMediaTypeOptics:
            if (fw_config.UnreliableLos) {
                media_type =  SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS;
            } else {
                media_type =  SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS;
            }
        break;
        case phymodFirmwareMediaTypeCopperCable:
            media_type =  SERDES_MEDIA_TYPE_COPPER_CABLE;
        break;
        case phymodFirmwareMediaTypePcbTraceBackPlane:
            media_type =  SERDES_MEDIA_TYPE_BACK_PLANE;
        break;
        default:
            break;
    }

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) || (config.data_rate == DINO_SPD_106G) || (config.data_rate == DINO_SPD_111G)) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            if (if_side == DINO_IF_LINE) {
                gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
            } else {
                gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            break;
        } else if (config.data_rate == DINO_SPD_40G || config.data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                break;
            } else {
                continue;
            }
        } else if (config.data_rate == DINO_SPD_10G || config.data_rate == DINO_SPD_11G || config.data_rate == DINO_SPD_11P18G || config.data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    gpreg_data &= ~(DINO_LINE_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_LINE_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_LINE_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_LINE_DFE_OPTION_SHIFT);
                } else {
                    gpreg_data &= ~(DINO_SYS_MEDIA_TYPE_MASK);
                    gpreg_data |= (media_type << DINO_SYS_MEDIA_TYPE_SHIFT);
                    gpreg_data &= ~(DINO_SYS_DFE_OPTION_MASK);
                    gpreg_data |= (DFE_option << DINO_SYS_DFE_OPTION_SHIFT);
                }
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                break;
            } else {
                continue;
            }
        } else {
            /* Throw error as un supported mode */
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    /* Set firmware enable */
    PHYMOD_IF_ERR_RETURN
        (_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _dino_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    phymod_phy_inf_config_t config;
    uint16_t no_of_ports = 0;
    uint32_t flags = 0;
    uint16_t if_side   = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data  = 0;
    uint16_t media_type  = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t DFE_option  = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t port_idx         = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    phymod_phy_access_t phy_copy;
    const phymod_access_t *pa = &phy_copy.access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    DINO_GET_INTF_SIDE(phy, if_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) || (config.data_rate == DINO_SPD_106G) || (config.data_rate == DINO_SPD_111G)) {
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            if (if_side == DINO_IF_LINE) {
                media_type = (gpreg_data & DINO_LINE_MEDIA_TYPE_MASK) >> DINO_LINE_MEDIA_TYPE_SHIFT;
                DFE_option = (gpreg_data & DINO_LINE_DFE_OPTION_MASK) >> DINO_LINE_DFE_OPTION_SHIFT;
            } else {
                media_type = (gpreg_data & DINO_SYS_MEDIA_TYPE_MASK) >> DINO_SYS_MEDIA_TYPE_SHIFT;
                DFE_option = (gpreg_data & DINO_SYS_DFE_OPTION_MASK) >> DINO_SYS_DFE_OPTION_SHIFT;
            }
            break;
        } else if (config.data_rate == DINO_SPD_40G || config.data_rate == DINO_SPD_42G) {
            if ((lane_mask & (0xf << (port_idx * DINO_MAX_CORE_LANE)))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (port_idx * DINO_MAX_CORE_LANE)), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    media_type = (gpreg_data & DINO_LINE_MEDIA_TYPE_MASK) >> DINO_LINE_MEDIA_TYPE_SHIFT;
                    DFE_option = (gpreg_data & DINO_LINE_DFE_OPTION_MASK) >> DINO_LINE_DFE_OPTION_SHIFT;
                } else {
                    media_type = (gpreg_data & DINO_SYS_MEDIA_TYPE_MASK) >> DINO_SYS_MEDIA_TYPE_SHIFT;
                    DFE_option = (gpreg_data & DINO_SYS_DFE_OPTION_MASK) >> DINO_SYS_DFE_OPTION_SHIFT;
                }
                break;
            } else {
                continue;
            }
        } else if (config.data_rate == DINO_SPD_10G || config.data_rate == DINO_SPD_11G || config.data_rate == DINO_SPD_11P18G || config.data_rate == DINO_SPD_1G) {
            if ((lane_mask & (0x1 << port_idx))) {
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + port_idx), gpreg_data);
                if (if_side == DINO_IF_LINE) {
                    media_type = (gpreg_data & DINO_LINE_MEDIA_TYPE_MASK) >> DINO_LINE_MEDIA_TYPE_SHIFT;
                    DFE_option = (gpreg_data & DINO_LINE_DFE_OPTION_MASK) >> DINO_LINE_DFE_OPTION_SHIFT;
                } else {
                    media_type = (gpreg_data & DINO_SYS_MEDIA_TYPE_MASK) >> DINO_SYS_MEDIA_TYPE_SHIFT;
                    DFE_option = (gpreg_data & DINO_SYS_DFE_OPTION_MASK) >> DINO_SYS_DFE_OPTION_SHIFT;
                }
                break;
            } else {
                continue;
            }
        } else {
            /* Throw error as un supported mode */
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Unsupported mode")));
        }
    }

    switch (DFE_option) {
        case SERDES_DFE_OPTION_NO_DFE:
            fw_config->DfeOn = 0;
            fw_config->ForceBrDfe = 0;
            fw_config->LpDfeOn = 0;
        break;
        case SERDES_DFE_OPTION_DFE:
            fw_config->DfeOn = 1;
            fw_config->ForceBrDfe = 0;
            fw_config->LpDfeOn = 0;
        break;
        case SERDES_DFE_OPTION_BRDFE:
            fw_config->DfeOn = 1;
            fw_config->ForceBrDfe = 1;
            fw_config->LpDfeOn = 0;
        break;
        case SERDES_DFE_OPTION_DFE_LP_MODE:
            fw_config->DfeOn = 1;
            fw_config->ForceBrDfe = 0;
            fw_config->LpDfeOn = 1;
        break;
        default:
        break;
    }

    fw_config->UnreliableLos = 0;
    switch (media_type) {
        case SERDES_MEDIA_TYPE_BACK_PLANE:
            fw_config->MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
        break;
        case SERDES_MEDIA_TYPE_COPPER_CABLE:
            fw_config->MediaType = phymodFirmwareMediaTypeCopperCable;
        break;
        case SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS:
            fw_config->MediaType = phymodFirmwareMediaTypeOptics;
        break;
        case SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS:
            fw_config->MediaType = phymodFirmwareMediaTypeOptics;
            fw_config->UnreliableLos = 1;
        break;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    phymod_phy_inf_config_t config;
    uint16_t lane_mask = 0;
    uint32_t flags = 0;
    uint16_t no_of_ports = 0;
    phymod_phy_access_t phy_copy;
    uint16_t gpreg_data  = 0;
    uint16_t gpreg_f_data = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mst_lane_reg_data  = 0;
    uint16_t port_idx         = 0;
    uint16_t an_mst_lane = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    BCMI_DINO_AN_LN_SELr_t an_mst_lane_sel;
    BCMI_DINO_MODECTL0r_t mode_ctrl0;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&an_mst_lane_sel, 0, sizeof(BCMI_DINO_AN_LN_SELr_t));
    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_DINO_MODECTL0r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    if ((config.interface_type != phymodInterfaceKR4) &&
            (config.interface_type != phymodInterfaceCR4) &&
            (config.interface_type != phymodInterfaceCR10)) {
        return PHYMOD_E_NONE;
    }

    if (an->an_mode == phymod_AN_MODE_CL73) {
        for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
            if ((config.data_rate == DINO_SPD_100G) && 
                ((config.interface_type == phymodInterfaceKR4) ||
                 (config.interface_type == phymodInterfaceCR4) ||
                 (config.interface_type == phymodInterfaceCR10))) { 
                /* Read the master lane from scratch register and program the master lane */
                READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
                dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
                READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
                an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_100G_MASK) >> DINO_AN_MST_LANE_100G_SHIFT; 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_AN_LN_SELr(pa, &an_mst_lane_sel));
                if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                    if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
                        ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
                        ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
                        if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
                            an_mst_lane += 1;
                        } 
                        if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
                            an_mst_lane += 2;
                        }
                        READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
                        gpreg_f_data &= ~(DINO_100G_CR10_AN_INSTANCE_MASK);
                        gpreg_f_data |= (an_mst_lane/DINO_MAX_CORE_LANE) << DINO_100G_CR10_AN_INSTANCE_SHIFT;
                        WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
                        switch (an_mst_lane/DINO_MAX_CORE_LANE) {
                            case 0:
                                BCMI_DINO_AN_LN_SELr_MP0_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, (an_mst_lane % DINO_MAX_CORE_LANE));
                            break;
                            case 1:
                                BCMI_DINO_AN_LN_SELr_MP1_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, (an_mst_lane % DINO_MAX_CORE_LANE));
                            break;
                            case 2:
                                BCMI_DINO_AN_LN_SELr_MP2_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, (an_mst_lane % DINO_MAX_CORE_LANE));
                            break;
                        }
                    }
                } else {
                    BCMI_DINO_AN_LN_SELr_MP0_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, (an_mst_lane % DINO_MAX_CORE_LANE));
                }

                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_AN_LN_SELr(pa, an_mst_lane_sel));
                READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
                gpreg_data &= ~(DINO_AN_MASK);
                gpreg_data |= ((an->enable)<< DINO_AN_SHIFT);
                WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
                if (an->enable) {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                        BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                    } else {
                        BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 0);
                    }
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                } else {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                }
                break;
            } else if ((config.data_rate == DINO_SPD_40G) &&
                       (lane_mask == DINO_40G_PORT0_LANE_MASK) &&
                       ((config.interface_type == phymodInterfaceKR4) || 
                        (config.interface_type == phymodInterfaceCR4))) {
                READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
                an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P0_MASK) >> DINO_AN_MST_LANE_40G_P0_SHIFT; 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_AN_LN_SELr(pa, &an_mst_lane_sel));
                BCMI_DINO_AN_LN_SELr_MP0_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, an_mst_lane); 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_AN_LN_SELr(pa, an_mst_lane_sel));
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + DINO_PORT0), gpreg_data);
                gpreg_data &= ~(DINO_AN_MASK);
                gpreg_data |= ((an->enable) << DINO_AN_SHIFT);
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + DINO_PORT0), gpreg_data);
                if (an->enable) {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 0);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                } else {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP0_ENf_SET(mode_ctrl0, 1);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                }
                break;
            } else if ((config.data_rate == DINO_SPD_40G) &&
                       (lane_mask == DINO_40G_PORT1_LANE_MASK) &&
                       ((config.interface_type == phymodInterfaceKR4) || 
                        (config.interface_type == phymodInterfaceCR4))) {
                READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
                an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P1_MASK) >> DINO_AN_MST_LANE_40G_P1_SHIFT; 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_AN_LN_SELr(pa, &an_mst_lane_sel));
                BCMI_DINO_AN_LN_SELr_MP1_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, an_mst_lane); 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_AN_LN_SELr(pa, an_mst_lane_sel));
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), gpreg_data);
                gpreg_data &= ~(DINO_AN_MASK);
                gpreg_data |= ((an->enable) << DINO_AN_SHIFT);
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), gpreg_data);
                if (an->enable) {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 0);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                } else {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 1);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                }
                BCMI_DINO_MODECTL0r_UDMS_MP1_ENf_SET(mode_ctrl0, 0);
                break;
            } else if ((config.data_rate == DINO_SPD_40G) &&
                       (lane_mask == DINO_40G_PORT2_LANE_MASK) &&
                       ((config.interface_type == phymodInterfaceKR4) || 
                        (config.interface_type == phymodInterfaceCR4))) {
                READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
                an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P2_MASK) >> DINO_AN_MST_LANE_40G_P2_SHIFT; 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_READ_AN_LN_SELr(pa, &an_mst_lane_sel));
                BCMI_DINO_AN_LN_SELr_MP2_AN_LANE_SEL0TO3f_SET(an_mst_lane_sel, an_mst_lane); 
                PHYMOD_IF_ERR_RETURN(
                    BCMI_DINO_WRITE_AN_LN_SELr(pa, an_mst_lane_sel));
                READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), gpreg_data);
                gpreg_data &= ~(DINO_AN_MASK);
                gpreg_data |= ((an->enable) << DINO_AN_SHIFT);
                WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), gpreg_data);
                if (an->enable) {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 0);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                } else {
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MODECTL0r(pa, &mode_ctrl0));
                    BCMI_DINO_MODECTL0r_UDMS_MP2_ENf_SET(mode_ctrl0, 1);
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_WRITE_MODECTL0r(pa, mode_ctrl0));
                }
                break;
            }
        }
    }

    /*Setting the FW Enable to 1*/
    if ((dev_op_mode == DINO_DEV_OP_MODE_PT) && 
        (config.data_rate == DINO_SPD_100G) &&
        (config.interface_type == phymodInterfaceCR10)) {
        if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
            ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
            ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
            PHYMOD_IF_ERR_RETURN(_dino_fw_enable(pa));
        }
    } else {
        PHYMOD_IF_ERR_RETURN(_dino_fw_enable(pa));
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t *an_done)
{
    phymod_phy_inf_config_t config;
    uint16_t lane_mask = 0;
    uint32_t flags = 0;
    uint16_t no_of_ports = 0;
    uint16_t gpreg_data  = 0;
    uint16_t port_idx         = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mst_lane_reg_data  = 0;
    uint16_t an_mst_lane = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    phymod_phy_access_t phy_copy;
    BCMI_DINO_AN_STSr_t an_sts;
    BCMI_DINO_DEV7_SLICE_SLICEr_t an_slice;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&an_sts, 0, sizeof(BCMI_DINO_AN_STSr_t));
    PHYMOD_MEMSET(&an_slice, 0, sizeof(BCMI_DINO_DEV7_SLICE_SLICEr_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
       _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    if ((config.interface_type != phymodInterfaceKR4) &&
            (config.interface_type != phymodInterfaceCR4) &&
            (config.interface_type != phymodInterfaceCR10)) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_DEV7_SLICE_SLICEr(pa, &an_slice));
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_0_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_1_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_2_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 0);
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) &&
            ((config.interface_type == phymodInterfaceKR4) ||
             (config.interface_type == phymodInterfaceCR4) ||
             (config.interface_type == phymodInterfaceCR10))) { 
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_0_ADR, gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_100G_MASK) >> DINO_AN_MST_LANE_100G_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_0_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
                    ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
                    ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
                    if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
                        an_mst_lane += 1;
                    } 
                    if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
                        an_mst_lane += 2;
                    }
                    switch(an_mst_lane/DINO_MAX_CORE_LANE) {
                        case 0:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
                        break;
                        case 1:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
                        break;
                        case 2:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
                        break;
                    }
                }
            } else {
                BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            }
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT0_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + DINO_PORT0), gpreg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P0_MASK) >> DINO_AN_MST_LANE_40G_P0_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT1_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT1 * DINO_MAX_CORE_LANE)), gpreg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P1_MASK) >> DINO_AN_MST_LANE_40G_P1_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT2_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + (DINO_PORT2 * DINO_MAX_CORE_LANE)), gpreg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P2_MASK) >> DINO_AN_MST_LANE_40G_P2_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
            break;
        }
    }

    /* AN enable status */
    an->enable = (gpreg_data & DINO_AN_MASK) >> DINO_AN_SHIFT;

    /* AN Mode */
    an->an_mode = phymod_AN_MODE_CL73;

    BCMI_DINO_DEV7_SLICE_SLICEr_LANE_SELf_SET(an_slice, (1 << (an_mst_lane % DINO_MAX_CORE_LANE)));

    /* Set the AN slice with AN master lane */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));

    /* Read the AN status */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_STSr(pa, &an_sts));

    if (BCMI_DINO_AN_STSr_AUTO_NEGOTIATIONCOMPLETEf_GET(an_sts)) {
        *an_done = 1;
    } else {
        *an_done = 0;
    }

    BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));


    return PHYMOD_E_NONE;
}

int _dino_phy_autoneg_ability_set(const phymod_phy_access_t* phy, dino_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t lane_mask = 0;
    uint32_t flags = 0;
    phymod_phy_access_t phy_copy;
    uint16_t no_of_ports = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mst_lane_reg_data  = 0;
    uint16_t gpreg_f_data = 0;
    uint16_t port_idx         = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    BCMI_DINO_AN_ADVERTISEMENT1r_t an_adv1;
    BCMI_DINO_AN_ADVERTISEMENT2r_t an_adv2;
    BCMI_DINO_AN_ADVERTISEMENT3r_t an_adv3;
    BCMI_DINO_DEV7_SLICE_SLICEr_t an_slice;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&an_adv1, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT1r_t));
    PHYMOD_MEMSET(&an_adv2, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT2r_t));
    PHYMOD_MEMSET(&an_adv3, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT3r_t));
    PHYMOD_MEMSET(&an_slice, 0, sizeof(BCMI_DINO_DEV7_SLICE_SLICEr_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
        _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    if ((config.interface_type != phymodInterfaceKR4) &&
            (config.interface_type != phymodInterfaceCR4) &&
            (config.interface_type != phymodInterfaceCR10)) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_DEV7_SLICE_SLICEr(pa, &an_slice));
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_0_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_1_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_2_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 0);
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) &&
            ((config.interface_type == phymodInterfaceKR4) ||
             (config.interface_type == phymodInterfaceCR4) ||
             (config.interface_type == phymodInterfaceCR10))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            mst_lane_reg_data &= ~(DINO_AN_MST_LANE_100G_MASK);
            mst_lane_reg_data |= ((an_ability->an_master_lane)<< DINO_AN_MST_LANE_100G_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
                    ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
                    ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
                    if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
                        an_ability->an_master_lane += 1;
                    } 
                    if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
                        an_ability->an_master_lane += 2;
                    }
                    switch (an_ability->an_master_lane/DINO_MAX_CORE_LANE) {
                        case 0:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
                        break;
                        case 1:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
                        break;
                        case 2:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
                        break;
                    }
                }
            } else {
                BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            }
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            if((gpreg_f_data & (1<<DINO_100G_SKIP_TX_TRAINING_SHIFT))!=(an_ability->cl73_adv.an_cl72<<DINO_100G_SKIP_TX_TRAINING_SHIFT)){
            gpreg_f_data &= ~(DINO_100G_SKIP_TX_TRAINING_MASK);
            gpreg_f_data |= ((an_ability->cl73_adv.an_cl72) ? 0 : 1) << DINO_100G_SKIP_TX_TRAINING_SHIFT;
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            }
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT0_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            mst_lane_reg_data &= ~(DINO_AN_MST_LANE_40G_P0_MASK);
            mst_lane_reg_data |= ((an_ability->an_master_lane)<< DINO_AN_MST_LANE_40G_P0_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            if((gpreg_f_data & (1<<DINO_40G_PORT0_SKIP_TX_TRAINING_SHIFT))!=(an_ability->cl73_adv.an_cl72<<DINO_40G_PORT0_SKIP_TX_TRAINING_SHIFT)){
            gpreg_f_data &= ~(DINO_40G_PORT0_SKIP_TX_TRAINING_MASK);
            gpreg_f_data |= ((an_ability->cl73_adv.an_cl72) ? 0 : 1) << DINO_40G_PORT0_SKIP_TX_TRAINING_SHIFT;
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            }
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                    (lane_mask == DINO_40G_PORT1_LANE_MASK) &&
                    ((config.interface_type == phymodInterfaceKR4) ||
                     (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            mst_lane_reg_data &= ~(DINO_AN_MST_LANE_40G_P1_MASK);
            mst_lane_reg_data |= ((an_ability->an_master_lane)<< DINO_AN_MST_LANE_40G_P1_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            if((gpreg_f_data & (1<<DINO_40G_PORT1_SKIP_TX_TRAINING_SHIFT))!=(an_ability->cl73_adv.an_cl72<<DINO_40G_PORT1_SKIP_TX_TRAINING_SHIFT)){
            gpreg_f_data &= ~(DINO_40G_PORT1_SKIP_TX_TRAINING_MASK);
            gpreg_f_data |= ((an_ability->cl73_adv.an_cl72) ? 0 : 1) << DINO_40G_PORT1_SKIP_TX_TRAINING_SHIFT;
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            }
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                    (lane_mask == DINO_40G_PORT2_LANE_MASK) &&
                    ((config.interface_type == phymodInterfaceKR4) ||
                     (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            mst_lane_reg_data &= ~(DINO_AN_MST_LANE_40G_P2_MASK);
            mst_lane_reg_data |= ((an_ability->an_master_lane)<< DINO_AN_MST_LANE_40G_P2_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            if((gpreg_f_data & (1<<DINO_40G_PORT2_SKIP_TX_TRAINING_SHIFT))!=(an_ability->cl73_adv.an_cl72<<DINO_40G_PORT2_SKIP_TX_TRAINING_SHIFT)){
            gpreg_f_data &= ~(DINO_40G_PORT2_SKIP_TX_TRAINING_MASK);
            gpreg_f_data |= ((an_ability->cl73_adv.an_cl72) ? 0 : 1) << DINO_40G_PORT2_SKIP_TX_TRAINING_SHIFT;
            WRITE_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
            }
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
            break;
        }
    }
    BCMI_DINO_DEV7_SLICE_SLICEr_LANE_SELf_SET(an_slice, (1 << (an_ability->an_master_lane % DINO_MAX_CORE_LANE)));
    /* Set the AN slice with AN master lane */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT1r(pa, &an_adv1));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT2r(pa, &an_adv2));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT3r(pa, &an_adv3));

    BCMI_DINO_AN_ADVERTISEMENT1r_PAUSEf_SET(an_adv1, an_ability->cl73_adv.an_pause);
    BCMI_DINO_AN_ADVERTISEMENT2r_TECHABILITYf_SET(an_adv2, an_ability->cl73_adv.an_base_speed);
	if(BCMI_DINO_AN_ADVERTISEMENT3r_FEC_REQUESTEDf_GET(an_adv3)!=an_ability->cl73_adv.an_fec){
		BCMI_DINO_AN_ADVERTISEMENT3r_FEC_REQUESTEDf_SET(an_adv3, an_ability->cl73_adv.an_fec);}

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_AN_ADVERTISEMENT1r(pa, an_adv1));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_AN_ADVERTISEMENT2r(pa, an_adv2));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_AN_ADVERTISEMENT3r(pa, an_adv3));
    BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));

    return PHYMOD_E_NONE;
}

int _dino_phy_autoneg_ability_get(const phymod_phy_access_t* phy, dino_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t lane_mask = 0;
    uint32_t flags = 0;
    uint16_t port_idx         = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mst_lane_reg_data  = 0;
    uint16_t gpreg_f_data = 0;
    uint16_t an_mst_lane = 0;
    uint16_t no_of_ports = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    phymod_phy_access_t phy_copy;
    BCMI_DINO_DEV7_SLICE_SLICEr_t an_slice;
    BCMI_DINO_AN_ADVERTISEMENT1r_t an_adv1;
    BCMI_DINO_AN_ADVERTISEMENT2r_t an_adv2;
    BCMI_DINO_AN_ADVERTISEMENT3r_t an_adv3;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&an_slice, 0, sizeof(BCMI_DINO_DEV7_SLICE_SLICEr_t));
    PHYMOD_MEMSET(&an_adv1, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT1r_t));
    PHYMOD_MEMSET(&an_adv2, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT2r_t));
    PHYMOD_MEMSET(&an_adv3, 0, sizeof(BCMI_DINO_AN_ADVERTISEMENT3r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
       _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    if ((config.interface_type != phymodInterfaceKR4) &&
            (config.interface_type != phymodInterfaceCR4) &&
            (config.interface_type != phymodInterfaceCR10)) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_DEV7_SLICE_SLICEr(pa, &an_slice));
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_0_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_1_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_2_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 0);
    READ_DINO_PMA_PMD_REG(pa, DINO_GPREG_F_ADR, gpreg_f_data);
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) &&
            ((config.interface_type == phymodInterfaceKR4) ||
             (config.interface_type == phymodInterfaceCR4) ||
             (config.interface_type == phymodInterfaceCR10))) { 
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_100G_MASK) >> DINO_AN_MST_LANE_100G_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
                    ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
                    ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
                    if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
                        an_mst_lane += 1;
                    } 
                    if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
                        an_mst_lane += 2;
                    }
                    switch (an_mst_lane/DINO_MAX_CORE_LANE) {
                        case 0:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
                        break;
                        case 1:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
                        break;
                        case 2:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
                        break;
                    }
                }
            } else {
                BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            }
            (gpreg_f_data & DINO_100G_SKIP_TX_TRAINING_MASK) ? (an_ability->cl73_adv.an_cl72 = 0): (an_ability->cl73_adv.an_cl72 = 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT0_LANE_MASK)&& 
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P0_MASK) >> DINO_AN_MST_LANE_40G_P0_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            (gpreg_f_data & DINO_40G_PORT0_SKIP_TX_TRAINING_MASK) ? (an_ability->cl73_adv.an_cl72 = 0): (an_ability->cl73_adv.an_cl72 = 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT1_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P1_MASK) >> DINO_AN_MST_LANE_40G_P1_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
            (gpreg_f_data & DINO_40G_PORT1_SKIP_TX_TRAINING_MASK) ? (an_ability->cl73_adv.an_cl72 = 0): (an_ability->cl73_adv.an_cl72 = 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT2_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P2_MASK) >> DINO_AN_MST_LANE_40G_P2_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
            (gpreg_f_data & DINO_40G_PORT2_SKIP_TX_TRAINING_MASK) ? (an_ability->cl73_adv.an_cl72 = 0): (an_ability->cl73_adv.an_cl72 = 1);
            break;
        }
    }
    BCMI_DINO_DEV7_SLICE_SLICEr_LANE_SELf_SET(an_slice, (1 << (an_mst_lane % DINO_MAX_CORE_LANE)));
    /* Set the AN slice with AN master lane */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));

    /* Read AN ability */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT1r(pa, &an_adv1));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT2r(pa, &an_adv2));
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_AN_ADVERTISEMENT3r(pa, &an_adv3));

    an_ability->cl73_adv.an_pause = BCMI_DINO_AN_ADVERTISEMENT1r_PAUSEf_GET(an_adv1);
    an_ability->cl73_adv.an_base_speed = BCMI_DINO_AN_ADVERTISEMENT2r_TECHABILITYf_GET(an_adv2);
    an_ability->cl73_adv.an_fec = BCMI_DINO_AN_ADVERTISEMENT3r_FEC_REQUESTEDf_GET(an_adv3);
    if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
        an_mst_lane -= 1;
    } 
    if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
        an_mst_lane -= 2;
    }
    /* Read AN master lane */
    an_ability->an_master_lane = an_mst_lane;
    
    /* Clear the slice */
    BCMI_DINO_DEV7_SLICE_SLICEr_CLR(an_slice);
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));

    return PHYMOD_E_NONE;
}

int _dino_phy_autoneg_remote_ability_get(const phymod_phy_access_t *phy, phymod_autoneg_ability_t* an_ability_get)
{
    phymod_phy_inf_config_t config;
    uint16_t lane_mask = 0;
    uint32_t flags = 0;
    uint16_t port_idx         = 0;
    uint16_t sw_gpreg_data  = 0;
    uint16_t mst_lane_reg_data  = 0;
    uint16_t an_mst_lane = 0;
    uint16_t no_of_ports = 0;
    uint16_t an_bp_abl1 = 0;
    uint16_t an_bp_abl2 = 0;
    uint16_t an_bp_abl3 = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t dev_op_mode      = DINO_DEV_OP_MODE_INVALID;
    BCMI_DINO_DEV7_SLICE_SLICEr_t an_slice;
    BCMI_DINO_AN_LP_BASE_PAGE_ABIL_1r_t base_page_ability1;
    BCMI_DINO_AN_LP_BASE_PAGE_ABIL_2r_t base_page_ability2;
    BCMI_DINO_AN_LP_BASE_PAGE_ABIL_3r_t base_page_ability3;
    const phymod_access_t *pa = &phy->access;
    phymod_phy_access_t phy_copy;
    PHYMOD_MEMSET(&phy_copy, 0, sizeof(phymod_phy_access_t));
    PHYMOD_MEMCPY(&phy_copy, phy, sizeof(phymod_phy_access_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&an_slice, 0, sizeof(BCMI_DINO_DEV7_SLICE_SLICEr_t));
    PHYMOD_MEMSET(&base_page_ability1, 0, sizeof(BCMI_DINO_AN_LP_BASE_PAGE_ABIL_1r_t));
    PHYMOD_MEMSET(&base_page_ability2, 0, sizeof(BCMI_DINO_AN_LP_BASE_PAGE_ABIL_2r_t));
    PHYMOD_MEMSET(&base_page_ability3, 0, sizeof(BCMI_DINO_AN_LP_BASE_PAGE_ABIL_3r_t));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(
       _dino_phy_interface_config_get(&phy_copy, flags, &config));
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    DINO_GET_PORT_FROM_MODE((&config), chip_id, no_of_ports);

    if ((config.interface_type != phymodInterfaceKR4) &&
            (config.interface_type != phymodInterfaceCR4) &&
            (config.interface_type != phymodInterfaceCR10)) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_READ_DEV7_SLICE_SLICEr(pa, &an_slice));
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_0_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_1_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_SLICE_2_SELf_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 0);
    BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 0);
    for (port_idx = 0; port_idx < no_of_ports; port_idx++) {
        if ((config.data_rate == DINO_SPD_100G) &&
            ((config.interface_type == phymodInterfaceKR4) ||
             (config.interface_type == phymodInterfaceCR4) ||
             (config.interface_type == phymodInterfaceCR10))) { 
            READ_DINO_PMA_PMD_REG(pa, DINO_SW_GPREG_0_ADR, sw_gpreg_data);
            dev_op_mode = (sw_gpreg_data & DINO_DEV_OP_MODE_MASK) >> DINO_DEV_OP_MODE_SHIFT;  
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_100G_MASK) >> DINO_AN_MST_LANE_100G_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            if (dev_op_mode == DINO_DEV_OP_MODE_PT) { 
                if (((lane_mask == DINO_100G_TYPE1_LANE_MASK) || ((lane_mask & 0xf) == 0xf)) ||
                    ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) ||
                    ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc))) {
                    if ((lane_mask == DINO_100G_TYPE2_LANE_MASK) || ((lane_mask & 0xe) == 0xe)) {
                        an_mst_lane += 1;
                    } 
                    if ((lane_mask == DINO_100G_TYPE3_LANE_MASK) || ((lane_mask & 0xc) == 0xc)) {
                        an_mst_lane += 2;
                    }
                    switch (an_mst_lane/DINO_MAX_CORE_LANE) {
                        case 0:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
                        break;
                        case 1:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
                        break;
                        case 2:
                            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
                        break;
                    }
                } 
            } else {
                BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            }
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT0_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P0_MASK) >> DINO_AN_MST_LANE_40G_P0_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_0f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT1_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P1_MASK) >> DINO_AN_MST_LANE_40G_P1_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_1f_SET(an_slice, 1);
            break;
        } else if ((config.data_rate == DINO_SPD_40G) &&
                   (lane_mask == DINO_40G_PORT2_LANE_MASK) &&
                   ((config.interface_type == phymodInterfaceKR4) ||
                    (config.interface_type == phymodInterfaceCR4))) {
            READ_DINO_PMA_PMD_REG(pa, DINO_AN_MST_LANE_REG_ADR, mst_lane_reg_data);
            an_mst_lane = (mst_lane_reg_data & DINO_AN_MST_LANE_40G_P2_MASK) >> DINO_AN_MST_LANE_40G_P2_SHIFT; 
            BCMI_DINO_DEV7_SLICE_SLICEr_LIN_SELf_SET(an_slice, 1);
            BCMI_DINO_DEV7_SLICE_SLICEr_AN_PORT_2f_SET(an_slice, 1);
            break;
        }
    }
    BCMI_DINO_DEV7_SLICE_SLICEr_LANE_SELf_SET(an_slice, (1 << (an_mst_lane % DINO_MAX_CORE_LANE)));
    /* Set the AN slice with AN master lane */
    PHYMOD_IF_ERR_RETURN (
        BCMI_DINO_WRITE_DEV7_SLICE_SLICEr(pa, an_slice));
    
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_AN_LP_BASE_PAGE_ABIL_1r(pa, &base_page_ability1)); 
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_AN_LP_BASE_PAGE_ABIL_2r(pa, &base_page_ability2)); 
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_AN_LP_BASE_PAGE_ABIL_3r(pa, &base_page_ability3));

    an_bp_abl1 = BCMI_DINO_AN_LP_BASE_PAGE_ABIL_1r_D15_D0f_GET(base_page_ability1);
    an_bp_abl2 = BCMI_DINO_AN_LP_BASE_PAGE_ABIL_2r_D31_16f_GET(base_page_ability2);
    an_bp_abl3 = BCMI_DINO_AN_LP_BASE_PAGE_ABIL_3r_D47_32f_GET(base_page_ability3);

    an_ability_get->an_fec = (an_bp_abl3 & 0xC000) >> 15;
    an_ability_get->an_cap = (an_bp_abl2 & 0xFFE0) >> 5;

    if (an_bp_abl1& 0x400) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get);
    } else if (an_bp_abl1 & 0x800) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get);
    }

    return PHYMOD_E_NONE;
}

int  _dino_get_intr_reg (const phymod_phy_access_t *phy, uint32_t intr_type, uint16_t *bit_pos, uint16_t *int_grp)
{
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    const phymod_access_t *pa = &phy->access;

    DINO_GET_INTF_SIDE(phy, intf_side); 
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    /* Get the interrupt bit postion in the reg and interrupt group */
    switch (intr_type) {
        case PLL_LOCK_FOUND:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  0;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  0;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  0;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  0;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  0;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  0;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;
        case PLL_LOCK_LOST:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  1;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  1;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  1;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  1;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  1;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  1;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;
        case SIGDET_FOUND:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  2;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  2;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  2;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  2;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  2;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  2;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;
        case SIGDET_LOST:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  3;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  3;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  3;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  3;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  3;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  3;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;
        case PMD_LOCK_FOUND:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  4;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  4;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  4;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  4;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  4;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  4;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;    
        case PMD_LOCK_LOST:
            if (intf_side == DINO_IF_SYS) {
                if (lane_mask & 0xf ) {
                    *bit_pos =  5;
                    *int_grp =  SYS_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  5;
                    *int_grp =  SYS_LINK_CORE_1;
                } else {
                    *bit_pos =  5;
                    *int_grp =  SYS_LINK_CORE_2;
                }
            } else {
                if (lane_mask & 0xf ) {
                    *bit_pos =  5;
                    *int_grp =  LINE_LINK_CORE_0;
                } else if (lane_mask & 0xf0) {
                    *bit_pos =  5;
                    *int_grp =  LINE_LINK_CORE_1;
                } else {
                    *bit_pos =  5;
                    *int_grp =  LINE_LINK_CORE_2;
                }
            }
        break;
        case AN_COMPLETE:
            if (lane_mask & 0xf ) {
                *bit_pos =  0;
                *int_grp =  AN_CORE_0_1_2;
            } else if (lane_mask & 0xf0) {
                *bit_pos =  1;
                *int_grp =  AN_CORE_0_1_2;
            } else {
                *bit_pos =  2;
                *int_grp =  AN_CORE_0_1_2;
            }
        break;
        case AN_RESTARTED:
            if (lane_mask & 0xf ) {
                *bit_pos =  3;
                *int_grp =  AN_CORE_0_1_2;
            } else if (lane_mask & 0xf0) {
                *bit_pos =  4;
                *int_grp =  AN_CORE_0_1_2;
            } else {
                *bit_pos =  5;
                *int_grp =  AN_CORE_0_1_2;
            }
        break;
        case INT_DEMUX_FIFOERR:
            *bit_pos =  0;
            *int_grp =  DP_MUX_DEMUX_FIFO;
        break;
        case INT_MUX_FIFOERR:
            *bit_pos =  1;
            *int_grp =  DP_MUX_DEMUX_FIFO;
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}
int _dino_phy_intr_enable_set(const phymod_phy_access_t* phy, uint32_t intr_type, uint32_t enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    uint16_t data = 0;
    const phymod_access_t *pa = &phy->access;
    BCMI_DINO_LIN_25G_0_EIERr_t line_25g_0_eier;
    BCMI_DINO_LIN_10G_1_EIERr_t line_10g_1_eier;
    BCMI_DINO_LIN_10G_2_EIERr_t line_10g_2_eier;
    BCMI_DINO_SYS_10G_0_EIERr_t sys_10g_0_eier;
    BCMI_DINO_SYS_10G_1_EIERr_t sys_10g_1_eier;
    BCMI_DINO_SYS_10G_2_EIERr_t sys_10g_2_eier;
    BCMI_DINO_LIN_MISC_2_EIERr_t line_misc_2_eier;
    BCMI_DINO_MISC_EIERr_t misc_eier;

    PHYMOD_MEMSET(&line_25g_0_eier, 0, sizeof(BCMI_DINO_LIN_25G_0_EIERr_t));
    PHYMOD_MEMSET(&line_10g_1_eier, 0, sizeof(BCMI_DINO_LIN_10G_1_EIERr_t));
    PHYMOD_MEMSET(&line_10g_2_eier, 0, sizeof(BCMI_DINO_LIN_10G_2_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_0_eier, 0, sizeof(BCMI_DINO_SYS_10G_0_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_1_eier, 0, sizeof(BCMI_DINO_SYS_10G_1_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_2_eier, 0, sizeof(BCMI_DINO_SYS_10G_2_EIERr_t));
    PHYMOD_MEMSET(&line_misc_2_eier, 0, sizeof(BCMI_DINO_LIN_MISC_2_EIERr_t));
    PHYMOD_MEMSET(&misc_eier, 0, sizeof(BCMI_DINO_MISC_EIERr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    PHYMOD_IF_ERR_RETURN(
        _dino_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp));
    intr_mask = enable ? (0x1 << bit_pos) : (~(0x1 << bit_pos));
    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case LINE_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_25G_0_EIERr(pa, &line_25g_0_eier));
            data = (enable ? (intr_mask | BCMI_DINO_LIN_25G_0_EIERr_GET(line_25g_0_eier)) : (intr_mask & BCMI_DINO_LIN_25G_0_EIERr_GET(line_25g_0_eier)));
            BCMI_DINO_LIN_25G_0_EIERr_SET(line_25g_0_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_25G_0_EIERr(pa, line_25g_0_eier));
        break;
        case LINE_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_1_EIERr(pa, &line_10g_1_eier));
            data = (enable ? (intr_mask | BCMI_DINO_LIN_10G_1_EIERr_GET(line_10g_1_eier)) : (intr_mask & BCMI_DINO_LIN_10G_1_EIERr_GET(line_10g_1_eier)));
            BCMI_DINO_LIN_10G_1_EIERr_SET(line_10g_1_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_10G_1_EIERr(pa, line_10g_1_eier));
        break;
        case LINE_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_2_EIERr(pa, &line_10g_2_eier));
            data = (enable ? (intr_mask | BCMI_DINO_LIN_10G_2_EIERr_GET(line_10g_2_eier)) : (intr_mask & BCMI_DINO_LIN_10G_2_EIERr_GET(line_10g_2_eier)));
            BCMI_DINO_LIN_10G_2_EIERr_SET(line_10g_2_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_10G_2_EIERr(pa, line_10g_2_eier));
        break;
        case SYS_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_0_EIERr(pa, &sys_10g_0_eier));
            data = (enable ? (intr_mask | BCMI_DINO_SYS_10G_0_EIERr_GET(sys_10g_0_eier)) : (intr_mask & BCMI_DINO_SYS_10G_0_EIERr_GET(sys_10g_0_eier)));
            BCMI_DINO_SYS_10G_0_EIERr_SET(sys_10g_0_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_0_EIERr(pa, sys_10g_0_eier));
        break;
        case SYS_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_1_EIERr(pa, &sys_10g_1_eier));
            data = (enable ? (intr_mask | BCMI_DINO_SYS_10G_1_EIERr_GET(sys_10g_1_eier)) : (intr_mask & BCMI_DINO_SYS_10G_1_EIERr_GET(sys_10g_1_eier)));
            BCMI_DINO_SYS_10G_1_EIERr_SET(sys_10g_1_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_1_EIERr(pa, sys_10g_1_eier));
        break;
        case SYS_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_2_EIERr(pa, &sys_10g_2_eier));
            data = (enable ? (intr_mask | BCMI_DINO_SYS_10G_2_EIERr_GET(sys_10g_2_eier)) : (intr_mask & BCMI_DINO_SYS_10G_2_EIERr_GET(sys_10g_2_eier)));
            BCMI_DINO_SYS_10G_2_EIERr_SET(sys_10g_2_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_2_EIERr(pa, sys_10g_2_eier));
        break;
        case AN_CORE_0_1_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_MISC_2_EIERr(pa, &line_misc_2_eier));
            data = (enable ? (intr_mask | BCMI_DINO_LIN_MISC_2_EIERr_GET(line_misc_2_eier)) : (intr_mask & BCMI_DINO_LIN_MISC_2_EIERr_GET(line_misc_2_eier)));
            BCMI_DINO_LIN_MISC_2_EIERr_SET(line_misc_2_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_MISC_2_EIERr(pa, line_misc_2_eier));
        break;
        case DP_MUX_DEMUX_FIFO:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MISC_EIERr(pa, &misc_eier));
            data = (enable ? (intr_mask | BCMI_DINO_MISC_EIERr_GET(misc_eier)) : (intr_mask & BCMI_DINO_MISC_EIERr_GET(misc_eier)));
            BCMI_DINO_MISC_EIERr_SET(misc_eier, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MISC_EIERr(pa, misc_eier));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_intr_enable_get(const phymod_phy_access_t* phy, uint32_t intr_type, uint32_t* enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    BCMI_DINO_LIN_25G_0_EIERr_t line_25g_0_eier;
    BCMI_DINO_LIN_10G_1_EIERr_t line_10g_1_eier;
    BCMI_DINO_LIN_10G_2_EIERr_t line_10g_2_eier;
    BCMI_DINO_SYS_10G_0_EIERr_t sys_10g_0_eier;
    BCMI_DINO_SYS_10G_1_EIERr_t sys_10g_1_eier;
    BCMI_DINO_SYS_10G_2_EIERr_t sys_10g_2_eier;
    BCMI_DINO_LIN_MISC_2_EIERr_t line_misc_2_eier;
    BCMI_DINO_MISC_EIERr_t misc_eier;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&line_25g_0_eier, 0, sizeof(BCMI_DINO_LIN_25G_0_EIERr_t));
    PHYMOD_MEMSET(&line_10g_1_eier, 0, sizeof(BCMI_DINO_LIN_10G_1_EIERr_t));
    PHYMOD_MEMSET(&line_10g_2_eier, 0, sizeof(BCMI_DINO_LIN_10G_2_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_0_eier, 0, sizeof(BCMI_DINO_SYS_10G_0_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_1_eier, 0, sizeof(BCMI_DINO_SYS_10G_1_EIERr_t));
    PHYMOD_MEMSET(&sys_10g_2_eier, 0, sizeof(BCMI_DINO_SYS_10G_2_EIERr_t));
    PHYMOD_MEMSET(&line_misc_2_eier, 0, sizeof(BCMI_DINO_LIN_MISC_2_EIERr_t));
    PHYMOD_MEMSET(&misc_eier, 0, sizeof(BCMI_DINO_MISC_EIERr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    PHYMOD_IF_ERR_RETURN(
        _dino_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp));
    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case LINE_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_25G_0_EIERr(pa, &line_25g_0_eier));
            data = BCMI_DINO_LIN_25G_0_EIERr_GET(line_25g_0_eier);
        break;
        case LINE_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_1_EIERr(pa, &line_10g_1_eier));
            data = BCMI_DINO_LIN_10G_1_EIERr_GET(line_10g_1_eier);
        break;
        case LINE_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_2_EIERr(pa, &line_10g_2_eier));
            data = BCMI_DINO_LIN_10G_2_EIERr_GET(line_10g_2_eier);
        break;
        case SYS_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_0_EIERr(pa, &sys_10g_0_eier));
            data = BCMI_DINO_SYS_10G_0_EIERr_GET(sys_10g_0_eier);
        break;
        case SYS_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_1_EIERr(pa, &sys_10g_1_eier));
            data = BCMI_DINO_SYS_10G_1_EIERr_GET(sys_10g_1_eier);
        break;
        case SYS_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_2_EIERr(pa, &sys_10g_2_eier));
            data = BCMI_DINO_SYS_10G_2_EIERr_GET(sys_10g_2_eier);
        break;
        case AN_CORE_0_1_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_MISC_2_EIERr(pa, &line_misc_2_eier));
            data = BCMI_DINO_LIN_MISC_2_EIERr_GET(line_misc_2_eier);
        break;
        case DP_MUX_DEMUX_FIFO:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MISC_EIERr(pa, &misc_eier));
            data = BCMI_DINO_MISC_EIERr_GET(misc_eier);
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    *enable = (data & (0x1 << bit_pos)) ? 1 : 0;

    return PHYMOD_E_NONE;
}

int _dino_phy_intr_status_get(const phymod_phy_access_t* phy, uint32_t* intr_status)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    uint32_t event = 0;
    uint32_t intr_type = 0;
    uint32_t intr_index = 0;
    BCMI_DINO_LIN_25G_0_EISRr_t line_25g_0_eisr;
    BCMI_DINO_LIN_10G_1_EISRr_t line_10g_1_eisr;
    BCMI_DINO_LIN_10G_2_EISRr_t line_10g_2_eisr;
    BCMI_DINO_SYS_10G_0_EISRr_t sys_10g_0_eisr;
    BCMI_DINO_SYS_10G_1_EISRr_t sys_10g_1_eisr;
    BCMI_DINO_SYS_10G_2_EISRr_t sys_10g_2_eisr;
    BCMI_DINO_LIN_MISC_2_EISRr_t line_misc_2_eisr;
    BCMI_DINO_MISC_EISRr_t misc_eisr;
    BCMI_DINO_LIN_25G_0_EIPRr_t line_25g_0_eipr;
    BCMI_DINO_LIN_10G_1_EIPRr_t line_10g_1_eipr;
    BCMI_DINO_LIN_10G_2_EIPRr_t line_10g_2_eipr;
    BCMI_DINO_SYS_10G_0_EIPRr_t sys_10g_0_eipr;
    BCMI_DINO_SYS_10G_1_EIPRr_t sys_10g_1_eipr;
    BCMI_DINO_SYS_10G_2_EIPRr_t sys_10g_2_eipr;
    BCMI_DINO_LIN_MISC_2_EIPRr_t line_misc_2_eipr;
    BCMI_DINO_MISC_EIPRr_t misc_eipr;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&line_25g_0_eisr, 0, sizeof(BCMI_DINO_LIN_25G_0_EISRr_t));
    PHYMOD_MEMSET(&line_10g_1_eisr, 0, sizeof(BCMI_DINO_LIN_10G_1_EISRr_t));
    PHYMOD_MEMSET(&line_10g_2_eisr, 0, sizeof(BCMI_DINO_LIN_10G_2_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_0_eisr, 0, sizeof(BCMI_DINO_SYS_10G_0_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_1_eisr, 0, sizeof(BCMI_DINO_SYS_10G_1_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_2_eisr, 0, sizeof(BCMI_DINO_SYS_10G_2_EISRr_t));
    PHYMOD_MEMSET(&line_misc_2_eisr, 0, sizeof(BCMI_DINO_LIN_MISC_2_EISRr_t));
    PHYMOD_MEMSET(&misc_eisr, 0, sizeof(BCMI_DINO_MISC_EISRr_t));
    PHYMOD_MEMSET(&line_25g_0_eipr, 0, sizeof(BCMI_DINO_LIN_25G_0_EIPRr_t));
    PHYMOD_MEMSET(&line_10g_1_eipr, 0, sizeof(BCMI_DINO_LIN_10G_1_EIPRr_t));
    PHYMOD_MEMSET(&line_10g_2_eipr, 0, sizeof(BCMI_DINO_LIN_10G_2_EIPRr_t));
    PHYMOD_MEMSET(&sys_10g_0_eipr, 0, sizeof(BCMI_DINO_SYS_10G_0_EIPRr_t));
    PHYMOD_MEMSET(&sys_10g_1_eipr, 0, sizeof(BCMI_DINO_SYS_10G_1_EIPRr_t));
    PHYMOD_MEMSET(&sys_10g_2_eipr, 0, sizeof(BCMI_DINO_SYS_10G_2_EIPRr_t));
    PHYMOD_MEMSET(&line_misc_2_eipr, 0, sizeof(BCMI_DINO_LIN_MISC_2_EIPRr_t));
    PHYMOD_MEMSET(&misc_eipr, 0, sizeof(BCMI_DINO_MISC_EIPRr_t));
    intr_type = (*intr_status) ? (*intr_status & 0x7FFFFFFF) : 0x7FFFFFFF;
    event = (*intr_status & 0x80000000) ? 1 : 0;
    *intr_status = 0;
    for (intr_index = 0; intr_index < DINO_MAX_INTR_SUPPORT; intr_index ++) {
        PHYMOD_IF_ERR_RETURN(
            _dino_get_intr_reg(phy, 1 << intr_index, &bit_pos, &intr_grp));
        /* Interrupt enable get on specified interrupt group */
        if (event) {
            switch(intr_grp) {
                case LINE_LINK_CORE_0:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_LIN_25G_0_EISRr(pa, &line_25g_0_eisr));
                    data = BCMI_DINO_LIN_25G_0_EISRr_GET(line_25g_0_eisr);
                break;
                case LINE_LINK_CORE_1:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_LIN_10G_1_EISRr(pa, &line_10g_1_eisr));
                    data = BCMI_DINO_LIN_10G_1_EISRr_GET(line_10g_1_eisr);
                break;
                case LINE_LINK_CORE_2:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_LIN_10G_2_EISRr(pa, &line_10g_2_eisr));
                    data = BCMI_DINO_LIN_10G_2_EISRr_GET(line_10g_2_eisr);
                break;
                case SYS_LINK_CORE_0:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_SYS_10G_0_EISRr(pa, &sys_10g_0_eisr));
                    data = BCMI_DINO_SYS_10G_0_EISRr_GET(sys_10g_0_eisr);
                break;
                case SYS_LINK_CORE_1:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_SYS_10G_1_EISRr(pa, &sys_10g_1_eisr));
                    data = BCMI_DINO_SYS_10G_1_EISRr_GET(sys_10g_1_eisr);
                break;
                case SYS_LINK_CORE_2:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_SYS_10G_2_EISRr(pa, &sys_10g_2_eisr));
                    data = BCMI_DINO_SYS_10G_2_EISRr_GET(sys_10g_2_eisr);
                break;
                case AN_CORE_0_1_2:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_LIN_MISC_2_EISRr(pa, &line_misc_2_eisr));
                    data = BCMI_DINO_LIN_MISC_2_EISRr_GET(line_misc_2_eisr);
                break;
                case DP_MUX_DEMUX_FIFO:
                    PHYMOD_IF_ERR_RETURN(
                        BCMI_DINO_READ_MISC_EISRr(pa, &misc_eisr));
                    data = BCMI_DINO_MISC_EISRr_GET(misc_eisr);
                break;
                default:
                    return PHYMOD_E_PARAM;
            }
            if (data & (0x1 << bit_pos)) {
                *intr_status |= (1 << intr_index);
            } else {
                *intr_status &= ~(1 << intr_index);
            }
        } else {
            if (intr_type & (1 << intr_index)) {
                switch(intr_grp) {
                    case LINE_LINK_CORE_0:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_LIN_25G_0_EIPRr(pa, &line_25g_0_eipr));
                        data = BCMI_DINO_LIN_25G_0_EIPRr_GET(line_25g_0_eipr);
                    break;
                    case LINE_LINK_CORE_1:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_LIN_10G_1_EIPRr(pa, &line_10g_1_eipr));
                        data = BCMI_DINO_LIN_10G_1_EIPRr_GET(line_10g_1_eipr);
                    break;
                    case LINE_LINK_CORE_2:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_LIN_10G_2_EIPRr(pa, &line_10g_2_eipr));
                        data = BCMI_DINO_LIN_10G_2_EIPRr_GET(line_10g_2_eipr);
                    break;
                    case SYS_LINK_CORE_0:
                        PHYMOD_IF_ERR_RETURN(
                           BCMI_DINO_READ_SYS_10G_0_EIPRr(pa, &sys_10g_0_eipr));
                        data = BCMI_DINO_SYS_10G_0_EIPRr_GET(sys_10g_0_eipr);
                    break;
                    case SYS_LINK_CORE_1:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_SYS_10G_1_EIPRr(pa, &sys_10g_1_eipr));
                        data = BCMI_DINO_SYS_10G_1_EIPRr_GET(sys_10g_1_eipr);
                    break;
                    case SYS_LINK_CORE_2:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_SYS_10G_2_EIPRr(pa, &sys_10g_2_eipr));
                        data = BCMI_DINO_SYS_10G_2_EIPRr_GET(sys_10g_2_eipr);
                    break;
                    case AN_CORE_0_1_2:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_LIN_MISC_2_EIPRr(pa, &line_misc_2_eipr));
                        data = BCMI_DINO_LIN_MISC_2_EIPRr_GET(line_misc_2_eipr);
                    break;
                    case DP_MUX_DEMUX_FIFO:
                        PHYMOD_IF_ERR_RETURN(
                            BCMI_DINO_READ_MISC_EIPRr(pa, &misc_eipr));
                        data = BCMI_DINO_MISC_EIPRr_GET(misc_eipr);
                    break;
                    default:
                        return PHYMOD_E_PARAM;
                }
                if (data & (0x1 << bit_pos)) {
                    *intr_status |= (1 << intr_index);
                } else {
                    *intr_status &= ~(1 << intr_index);
                }
            }
        }
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_intr_status_clear(const phymod_phy_access_t* phy, uint32_t intr_type)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    BCMI_DINO_LIN_25G_0_EISRr_t line_25g_0_eisr;
    BCMI_DINO_LIN_10G_1_EISRr_t line_10g_1_eisr;
    BCMI_DINO_LIN_10G_2_EISRr_t line_10g_2_eisr;
    BCMI_DINO_SYS_10G_0_EISRr_t sys_10g_0_eisr;
    BCMI_DINO_SYS_10G_1_EISRr_t sys_10g_1_eisr;
    BCMI_DINO_SYS_10G_2_EISRr_t sys_10g_2_eisr;
    BCMI_DINO_LIN_MISC_2_EISRr_t line_misc_2_eisr;
    BCMI_DINO_MISC_EISRr_t misc_eisr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&line_25g_0_eisr, 0, sizeof(BCMI_DINO_LIN_25G_0_EISRr_t));
    PHYMOD_MEMSET(&line_10g_1_eisr, 0, sizeof(BCMI_DINO_LIN_10G_1_EISRr_t));
    PHYMOD_MEMSET(&line_10g_2_eisr, 0, sizeof(BCMI_DINO_LIN_10G_2_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_0_eisr, 0, sizeof(BCMI_DINO_SYS_10G_0_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_1_eisr, 0, sizeof(BCMI_DINO_SYS_10G_1_EISRr_t));
    PHYMOD_MEMSET(&sys_10g_2_eisr, 0, sizeof(BCMI_DINO_SYS_10G_2_EISRr_t));
    PHYMOD_MEMSET(&line_misc_2_eisr, 0, sizeof(BCMI_DINO_LIN_MISC_2_EISRr_t));
    PHYMOD_MEMSET(&misc_eisr, 0, sizeof(BCMI_DINO_MISC_EISRr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    PHYMOD_IF_ERR_RETURN(
        _dino_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp));
    intr_mask = 0x1 << bit_pos;
    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case LINE_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_25G_0_EISRr(pa, &line_25g_0_eisr));
            BCMI_DINO_LIN_25G_0_EISRr_SET(line_25g_0_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_25G_0_EISRr(pa, line_25g_0_eisr));
        break;
        case LINE_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_1_EISRr(pa, &line_10g_1_eisr));
            BCMI_DINO_LIN_10G_1_EISRr_SET(line_10g_1_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_10G_1_EISRr(pa, line_10g_1_eisr));
        break;
        case LINE_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_10G_2_EISRr(pa, &line_10g_2_eisr));
            BCMI_DINO_LIN_10G_2_EISRr_SET(line_10g_2_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_10G_2_EISRr(pa, line_10g_2_eisr));
        break;
        case SYS_LINK_CORE_0:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_0_EISRr(pa, &sys_10g_0_eisr));
            BCMI_DINO_SYS_10G_0_EISRr_SET(sys_10g_0_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_0_EISRr(pa, sys_10g_0_eisr));
        break;
        case SYS_LINK_CORE_1:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_1_EISRr(pa, &sys_10g_1_eisr));
            BCMI_DINO_SYS_10G_1_EISRr_SET(sys_10g_1_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_1_EISRr(pa, sys_10g_1_eisr));
        break;
        case SYS_LINK_CORE_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_SYS_10G_2_EISRr(pa, &sys_10g_2_eisr));
            BCMI_DINO_SYS_10G_2_EISRr_SET(sys_10g_2_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_SYS_10G_2_EISRr(pa, sys_10g_2_eisr));
        break;
        case AN_CORE_0_1_2:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_LIN_MISC_2_EISRr(pa, &line_misc_2_eisr));
            BCMI_DINO_LIN_MISC_2_EISRr_SET(line_misc_2_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_LIN_MISC_2_EISRr(pa, line_misc_2_eisr));
        break;
        case DP_MUX_DEMUX_FIFO:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MISC_EISRr(pa, &misc_eisr));
            BCMI_DINO_MISC_EISRr_SET(misc_eisr, intr_mask);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MISC_EISRr(pa, misc_eisr));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}


int _dino_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr,
                               uint32_t slv_addr, unsigned char xfer_cnt, DINO_I2CM_CMD_E cmd)
{
    BCMI_DINO_MODULE_CTL_CTLr_t module_ctrl;
    BCMI_DINO_MODULE_CTL_STSr_t mctrl_status;
    BCMI_DINO_XFER_CNTr_t mod_xfer_cnt;
    BCMI_DINO_ADDRr_t mod_add;
    BCMI_DINO_XFER_ADDRr_t mod_xfer_add;
    uint16_t retry_count = 500, data = 0;
    uint32_t wait_timeout_us = 0;
    wait_timeout_us = ((2*(xfer_cnt+1))*100)/5;

    PHYMOD_MEMSET(&module_ctrl, 0, sizeof(BCMI_DINO_MODULE_CTL_CTLr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_DINO_XFER_ADDRr_t));
    PHYMOD_MEMSET(&mod_xfer_cnt, 0, sizeof(BCMI_DINO_XFER_CNTr_t));
    PHYMOD_MEMSET(&mctrl_status, 0, sizeof(BCMI_DINO_MODULE_CTL_STSr_t));
    PHYMOD_MEMSET(&mod_add, 0, sizeof(BCMI_DINO_ADDRr_t));
    if (cmd == DINO_FLUSH) {
       BCMI_DINO_MODULE_CTL_CTLr_SET(module_ctrl, 0xC000);
       PHYMOD_IF_ERR_RETURN(
           BCMI_DINO_WRITE_MODULE_CTL_CTLr(pa, module_ctrl));
    } else {
        BCMI_DINO_XFER_ADDRr_SET(mod_xfer_add, xfer_addr);
        BCMI_DINO_XFER_CNTr_XFER_CNTf_SET(mod_xfer_cnt, xfer_cnt);
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_XFER_ADDRr(pa, mod_xfer_add));
        PHYMOD_IF_ERR_RETURN(
            BCMI_DINO_WRITE_XFER_CNTr(pa, mod_xfer_cnt)); 
        if (cmd == DINO_CURRENT_ADDRESS_READ) {
            BCMI_DINO_MODULE_CTL_CTLr_SET(module_ctrl, 0x8001);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MODULE_CTL_CTLr(pa, module_ctrl));
        } else if (cmd == DINO_RANDOM_ADDRESS_READ ) {
            BCMI_DINO_ADDRr_SET(mod_add, slv_addr);
            BCMI_DINO_MODULE_CTL_CTLr_SET(module_ctrl, 0x8003);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_ADDRr(pa,mod_add));
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));
        } else {
            BCMI_DINO_ADDRr_SET(mod_add, slv_addr);
            BCMI_DINO_MODULE_CTL_CTLr_SET(module_ctrl, 0x8022);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_ADDRr(pa, mod_add));
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MODULE_CTL_CTLr(pa, module_ctrl));
        }
    }

    if ((cmd == DINO_CURRENT_ADDRESS_READ) ||
        (cmd == DINO_RANDOM_ADDRESS_READ) ||
        (cmd == DINO_I2C_WRITE)) {
        do {
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MODULE_CTL_STSr(pa,&mctrl_status));
            data = BCMI_DINO_MODULE_CTL_STSr_XACTION_DONEf_GET(mctrl_status);
            PHYMOD_USLEEP(wait_timeout_us);
        } while((data == 0) && --retry_count);
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_TIMEOUT, (_PHYMOD_MSG("Module controller: I2C transaction failed..")));
        }
    }
    BCMI_DINO_MODULE_CTL_CTLr_SET(module_ctrl, 0x3);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MODULE_CTL_CTLr(pa, module_ctrl));

    return PHYMOD_E_NONE;
}

int _dino_phy_i2c_read(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t* read_data)
{
    uint32_t index = 0;
    uint16_t lane_mask = 0;
    uint32_t rd_data = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    uint32_t lower_page_flag = 0;
    uint16_t START_OF_NVRAM = 0x0;
    BCMI_DINO_GEN_CTL3r_t gen_ctrl3;
    BCMI_DINO_GEN_CTL1r_t gen_ctrl1;
    BCMI_DINO_DEV_IDr_t dev_id;
    BCMI_DINO_XFER_ADDRr_t mod_xfer_add;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL0r_t io_ctrl0;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL1r_t io_ctrl1;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL2r_t io_ctrl2;
    BCMI_DINO_MCTL_RAM_MDIO_CTLr_t mctl_ram_mdio_ctrl;

    PHYMOD_MEMSET(&io_ctrl0, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL0r_t));
    PHYMOD_MEMSET(&io_ctrl1, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL1r_t));
    PHYMOD_MEMSET(&io_ctrl2, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_DINO_GEN_CTL3r_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0, sizeof(BCMI_DINO_GEN_CTL1r_t));
    PHYMOD_MEMSET(&dev_id, 0, sizeof(BCMI_DINO_DEV_IDr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_DINO_XFER_ADDRr_t));
    PHYMOD_MEMSET(&mctl_ram_mdio_ctrl, 0, sizeof(BCMI_DINO_MCTL_RAM_MDIO_CTLr_t)); 

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }

    /* qsfp mode or legacy mode */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_DINO_GEN_CTL3r_QSFP_MODEf_SET(gen_ctrl3, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));

    /* qsfp mode reset at begining */
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl1));
    gen_ctrl1.v[0] &= ~(1 << 6);
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl1));
    gen_ctrl1.v[0] |= (1 << 6);
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_MCTL_RAM_MDIO_CTLr(pa, &mctl_ram_mdio_ctrl));
    switch (lane_mask) {
        case 0x3ff:
        case 0xffc:
        case 0x7fe:
        case 0xf:
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL3Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL3Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x10:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL4Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL4Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x20:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL5Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL5Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x40:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL6Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL6Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x80:
        case 0xf0:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL7Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL7Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x100:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL8Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL8Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x200:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL9Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL9Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x400:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL10Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL10Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x800:
        case 0xf00:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL11Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL11Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
    }

    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_MCTL_RAM_MDIO_CTLr(pa, mctl_ram_mdio_ctrl));
    /* Configure the slave device ID default is 0x50 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DEV_IDr(pa, &dev_id));
    BCMI_DINO_DEV_IDr_SL_DEV_ADDf_SET(dev_id, slv_dev_addr);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_DEV_IDr(pa, dev_id));

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN(
            _dino_set_module_command(pa, 0, 0, 0, DINO_FLUSH));
    }
    if ((no_of_bytes + start_addr) >= 256) {
        no_of_bytes = 255 - start_addr + 1;
    }

    /* To determine page to be written is lower page or upper page or
     * both lower and upper page
     */
    if ((start_addr+no_of_bytes - 1) > 127) {
        /* lower page */
        if (start_addr <= 127) {
            lower_page_bytes = 127 - start_addr + 1;
            lower_page_flag = 1;
            lower_page_start_addr = start_addr;
        }
        /* upper page */
        if ((start_addr + no_of_bytes) > 127) {
            upper_page_flag = 1;
            upper_page_bytes = no_of_bytes - lower_page_bytes;
            if(start_addr > 128) {
                upper_page_start_addr = start_addr;
            } else {
                upper_page_start_addr = 128;
            }
        }
    } else { /* only lower page */
        lower_page_bytes = no_of_bytes;
        lower_page_flag = 1;
        lower_page_start_addr = start_addr;
    }

    if (lower_page_flag) {
        PHYMOD_IF_ERR_RETURN(
            _dino_set_module_command(pa, DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                lower_page_start_addr, lower_page_start_addr, lower_page_bytes - 1, DINO_RANDOM_ADDRESS_READ));
        lower_page_flag = 0;
    }

    /* Need to check with chip team how we can read upper page */
    if (upper_page_flag) {
        PHYMOD_IF_ERR_RETURN(
            _dino_set_module_command(pa, DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                upper_page_start_addr, upper_page_start_addr, upper_page_bytes - 1, DINO_RANDOM_ADDRESS_READ));
        upper_page_flag = 0;
    }
   /* Read data from NVRAM using I2C*/
    for (index = 0; index < no_of_bytes; index++) {
        PHYMOD_IF_ERR_RETURN(
            PHYMOD_BUS_READ(pa, 0x10000 + DINO_MODULE_CNTRL_RAM_NVR0_ADR + start_addr + index ,  &rd_data));
        read_data[index] = (unsigned char) (rd_data & 0xff);
    }
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_MCTL_RAM_MDIO_CTLr(pa, &mctl_ram_mdio_ctrl));
    switch (lane_mask) {
        case 0x3ff:
        case 0xffc:
        case 0x7fe:
        case 0xf:
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL3Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL3Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x10:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL4Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL4Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x20:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL5Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL5Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x40:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL6Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL6Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x80:
        case 0xf0:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL7Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL7Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x100:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL8Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL8Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x200:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL9Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL9Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x400:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL10Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL10Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x800:
        case 0xf00:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL11Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL11Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
    }
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_MCTL_RAM_MDIO_CTLr(pa, mctl_ram_mdio_ctrl));

    return PHYMOD_E_NONE;
}

int _dino_phy_i2c_write(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, const uint8_t* write_data)
{
    uint16_t lane_mask = 0;
    uint32_t index = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    uint16_t START_OF_NVRAM = 0x0;
    BCMI_DINO_GEN_CTL3r_t gen_ctrl3;
    BCMI_DINO_GEN_CTL1r_t gen_ctrl1;
    BCMI_DINO_DEV_IDr_t dev_id;
    BCMI_DINO_XFER_ADDRr_t mod_xfer_add;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL0r_t io_ctrl0;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL1r_t io_ctrl1;
    BCMI_DINO_GENERAL_PURPOSE_IO_CTL2r_t io_ctrl2;
    BCMI_DINO_MCTL_RAM_MDIO_CTLr_t mctl_ram_mdio_ctrl;

    PHYMOD_MEMSET(&io_ctrl0, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL0r_t));
    PHYMOD_MEMSET(&io_ctrl1, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL1r_t));
    PHYMOD_MEMSET(&io_ctrl2, 0, sizeof(BCMI_DINO_GENERAL_PURPOSE_IO_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_DINO_GEN_CTL3r_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0, sizeof(BCMI_DINO_GEN_CTL1r_t));
    PHYMOD_MEMSET(&dev_id, 0, sizeof(BCMI_DINO_DEV_IDr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_DINO_XFER_ADDRr_t));
    PHYMOD_MEMSET(&mctl_ram_mdio_ctrl, 0, sizeof(BCMI_DINO_MCTL_RAM_MDIO_CTLr_t)); 

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }

    /* qsfp mode or legacy mode */
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_DINO_GEN_CTL3r_QSFP_MODEf_SET(gen_ctrl3, 1);
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));

    /* qsfp mode reset at begining */
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl1));
    gen_ctrl1.v[0] &= ~(1 << 6);
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL1r(pa, &gen_ctrl1));
    gen_ctrl1.v[0] |= (1 << 6);
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_MCTL_RAM_MDIO_CTLr(pa, &mctl_ram_mdio_ctrl));

    switch (lane_mask) {
        case 0x3ff:
        case 0xffc:
        case 0x7fe:
        case 0xf:
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL3Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL3Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x10:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL4Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL4Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x20:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL5Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL5Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x40:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL6Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL6Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x80:
        case 0xf0:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL7Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL7Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x100:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL8Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL8Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x200:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL9Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL9Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x400:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL10Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL10Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
        case 0x800:
        case 0xf00:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL11Bf_SET(gen_ctrl3, 0);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL11Bf_SET(mctl_ram_mdio_ctrl, 0);
        break;
    }

    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_MCTL_RAM_MDIO_CTLr(pa, mctl_ram_mdio_ctrl));

    /* Configure the slave device ID default is 0x50 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_DEV_IDr(pa, &dev_id));
    BCMI_DINO_DEV_IDr_SL_DEV_ADDf_SET(dev_id, slv_dev_addr);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_DEV_IDr(pa, dev_id));

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN(
            _dino_set_module_command(pa, 0, 0, 0, DINO_FLUSH));
    }
    /* if requested number of bytes are not within the boundary (0- 255)
     * need to calculate what maximum number of bytes can be taken into
     * account for reading or writing to module
     */
    if ((no_of_bytes + start_addr) >= 256) {
        no_of_bytes = 255 - start_addr + 1;
    }

    /* To determine page to be written is lower page or upper page or
     * both lower and upper page
     */
    if ((start_addr + no_of_bytes - 1) > 127) {
        /* lower page */
        if (start_addr <= 127) {
            lower_page_bytes = 127 - start_addr + 1;
            lower_page_flag = 1;
            lower_page_start_addr = start_addr;
        }
        /* upper page */
        if ((start_addr + no_of_bytes) > 127) {
            upper_page_flag = 1;
            upper_page_bytes = no_of_bytes - lower_page_bytes;
            if(start_addr > 128) {
                upper_page_start_addr = start_addr;
            } else {
                upper_page_start_addr = 128;
            }
        }
    } else { /* only lower page */
    lower_page_bytes = no_of_bytes;
        lower_page_flag = 1;
        lower_page_start_addr = start_addr;
    }

    /* Write data to NVRAM */
    for (index = 0; index < no_of_bytes; index++) {
        PHYMOD_IF_ERR_RETURN(
            PHYMOD_BUS_WRITE(pa,0x10000 + DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM + start_addr + index,  write_data[index]));
    }

    if(lower_page_flag) {
        for (index = 0; index < (lower_page_bytes / 4); index ++) {
            PHYMOD_IF_ERR_RETURN(
                _dino_set_module_command(pa, DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index), 3, DINO_I2C_WRITE));
        }
        if ((lower_page_bytes % 4) > 0) {
            PHYMOD_IF_ERR_RETURN(
                _dino_set_module_command(pa, DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index),
                                                       ((lower_page_bytes % 4) - 1), DINO_I2C_WRITE));
        }
        lower_page_flag = 0;
    }

    if(upper_page_flag) {
        for (index = 0; index < (upper_page_bytes / 4); index++) {
            PHYMOD_IF_ERR_RETURN(
                _dino_set_module_command(pa, (DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index), 3, DINO_I2C_WRITE));
        }
        if ((upper_page_bytes%4) > 0) {
            PHYMOD_IF_ERR_RETURN(
                _dino_set_module_command(pa, (DINO_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index),
                                                        ((upper_page_bytes % 4) - 1), DINO_I2C_WRITE));
        }
        upper_page_flag = 0;
    }
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_GEN_CTL3r(pa, &gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_READ_MCTL_RAM_MDIO_CTLr(pa, &mctl_ram_mdio_ctrl));
    switch (lane_mask) {
        case 0x3ff:
        case 0xffc:
        case 0x7fe:
        case 0xf:
        case 0x1:
        case 0x2:
        case 0x4:
        case 0x8:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL3Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL3Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x10:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL4Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL4Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x20:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL5Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL5Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x40:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL6Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL6Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x80:
        case 0xf0:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL7Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL7Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x100:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL8Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL8Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x200:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL9Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL9Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x400:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL10Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL10Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
        case 0x800:
        case 0xf00:
            BCMI_DINO_GEN_CTL3r_EXTMOD_SEL11Bf_SET(gen_ctrl3, 1);
            BCMI_DINO_MCTL_RAM_MDIO_CTLr_MD_EXTMOD_SEL11Bf_SET(mctl_ram_mdio_ctrl, 1);
        break;
    }
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    PHYMOD_IF_ERR_RETURN(BCMI_DINO_WRITE_MCTL_RAM_MDIO_CTLr(pa, mctl_ram_mdio_ctrl));

    return PHYMOD_E_NONE;
}

int _dino_phy_rptr_rtmr_mode_set(const phymod_phy_access_t* phy, uint16_t op_mode)
{
    phymod_phy_inf_config_t config_cpy;
    uint16_t rptr_rtmr_mode = 0;
    uint16_t lane_idx = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&config_cpy, 0, sizeof(phymod_phy_inf_config_t));
   
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);
    rptr_rtmr_mode = (op_mode == phymodOperationModeRetimer) ? DINO_TX_RETIMER : DINO_TX_REPEATER;

    /* Configure repeater/retimer mode on both line side and system side*/
    for (lane_idx = 0; lane_idx < num_lanes; lane_idx++) {
        if ((lane_mask >> lane_idx) & 0x1) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR+lane_idx), gpreg_data);
            if (((gpreg_data & DINO_LINE_TX_TIMING_MODE_MASK) >> DINO_LINE_TX_TIMING_MODE_SHIFT) == DINO_TX_REPEATER_ULL) {
                PHYMOD_DIAG_OUT(("ULL datapath is set:: This needs to be cleared for setting repeater/retimer mode \n"));
                return PHYMOD_E_NONE;
            } else {
                gpreg_data &= ~(DINO_LINE_TX_TIMING_MODE_MASK);
                gpreg_data |= (rptr_rtmr_mode << DINO_LINE_TX_TIMING_MODE_SHIFT);
            }
            if (((gpreg_data & DINO_SYS_TX_TIMING_MODE_MASK) >> DINO_SYS_TX_TIMING_MODE_SHIFT) == DINO_TX_REPEATER_ULL) {
                PHYMOD_DIAG_OUT(("ULL datapath is set:: This needs to be cleared for setting repeater/retimer mode \n"));
                return PHYMOD_E_NONE;
            } else {
                gpreg_data &= ~(DINO_SYS_TX_TIMING_MODE_MASK);
                gpreg_data |= (rptr_rtmr_mode << DINO_SYS_TX_TIMING_MODE_SHIFT);
            }
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR+lane_idx), gpreg_data);
        }
    }
    /* Notify the firmware about the configurations */
    PHYMOD_IF_ERR_RETURN(_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}

int _dino_bcast_enable_set(const phymod_core_access_t* core, uint16_t bcast_en)
{
    const phymod_access_t *pa = &core->access;
    BCMI_DINO_MDIO_PHYAD_CTLr_t mdio_phyad_ctl;
    PHYMOD_MEMSET(&mdio_phyad_ctl, 0, sizeof(BCMI_DINO_MDIO_PHYAD_CTLr_t));
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_READ_MDIO_PHYAD_CTLr(pa, &mdio_phyad_ctl));
    BCMI_DINO_MDIO_PHYAD_CTLr_MDIO_BRDCST_ENf_SET(mdio_phyad_ctl, bcast_en);
    PHYMOD_IF_ERR_RETURN(
        BCMI_DINO_WRITE_MDIO_PHYAD_CTLr(pa, mdio_phyad_ctl));

    return PHYMOD_E_NONE;
}
int _dino_phy_gpio_config_set(const phymod_access_t* pa, int pin_number, phymod_gpio_mode_t gpio_mode)
{
    uint16_t data = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    BCMI_DINO_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_DINO_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_DINO_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_DINO_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;
    BCMI_DINO_GPIO_4_CTLr_t pad_ctrl_gpio_4_ctrl;
    BCMI_DINO_GPIO_5_CTLr_t pad_ctrl_gpio_5_ctrl;
    BCMI_DINO_GPIO_6_CTLr_t pad_ctrl_gpio_6_ctrl;
    BCMI_DINO_GPIO_7_CTLr_t pad_ctrl_gpio_7_ctrl;
    BCMI_DINO_GPIO_8_CTLr_t pad_ctrl_gpio_8_ctrl;
    BCMI_DINO_GPIO_9_CTLr_t pad_ctrl_gpio_9_ctrl;
    BCMI_DINO_GPIO_10_CTLr_t pad_ctrl_gpio_10_ctrl;
    BCMI_DINO_GPIO_11_CTLr_t pad_ctrl_gpio_11_ctrl;
    BCMI_DINO_GPIO_12_CTLr_t pad_ctrl_gpio_12_ctrl;
    BCMI_DINO_GPIO_13_CTLr_t pad_ctrl_gpio_13_ctrl;
    BCMI_DINO_GPIO_14_CTLr_t pad_ctrl_gpio_14_ctrl;
    BCMI_DINO_GPIO_15_CTLr_t pad_ctrl_gpio_15_ctrl;
    BCMI_DINO_GPIO_16_CTLr_t pad_ctrl_gpio_16_ctrl;
    BCMI_DINO_GPIO_17_CTLr_t pad_ctrl_gpio_17_ctrl;
    BCMI_DINO_OPTXENB1_A_CTLr_t pad_ctrl_optxenb1_a;
    BCMI_DINO_OPTXENB1_B_CTLr_t pad_ctrl_optxenb1_b;
    BCMI_DINO_OPTXENB1_C_CTLr_t pad_ctrl_optxenb1_c;
    BCMI_DINO_MOD_ABS_3_CTLr_t pad_ctrl_mod_abs_3_ctrl;
    BCMI_DINO_MOD_ABS_4_CTLr_t pad_ctrl_mod_abs_4_ctrl;
    BCMI_DINO_MOD_ABS_5_CTLr_t pad_ctrl_mod_abs_5_ctrl;
    BCMI_DINO_MOD_ABS_6_CTLr_t pad_ctrl_mod_abs_6_ctrl;
    BCMI_DINO_MOD_ABS_7_CTLr_t pad_ctrl_mod_abs_7_ctrl;
    BCMI_DINO_MOD_ABS_8_CTLr_t pad_ctrl_mod_abs_8_ctrl;
    BCMI_DINO_MOD_ABS_9_CTLr_t pad_ctrl_mod_abs_9_ctrl;
    BCMI_DINO_MOD_ABS_10_CTLr_t pad_ctrl_mod_abs_10_ctrl;
    BCMI_DINO_MOD_ABS_11_CTLr_t pad_ctrl_mod_abs_11_ctrl;
    BCMI_DINO_OPRXLOS_4_CTLr_t  pad_ctrl_oprxlos_4_ctrl; 
    BCMI_DINO_OPRXLOS_5_CTLr_t  pad_ctrl_oprxlos_5_ctrl; 
    BCMI_DINO_OPRXLOS_6_CTLr_t  pad_ctrl_oprxlos_6_ctrl; 
    BCMI_DINO_OPRXLOS_7_CTLr_t  pad_ctrl_oprxlos_7_ctrl; 
    BCMI_DINO_OPRXLOS_8_CTLr_t  pad_ctrl_oprxlos_8_ctrl; 
    BCMI_DINO_OPRXLOS_9_CTLr_t  pad_ctrl_oprxlos_9_ctrl; 
    BCMI_DINO_OPRXLOS_10_CTLr_t  pad_ctrl_oprxlos_10_ctrl; 
    BCMI_DINO_OPRXLOS_11_CTLr_t  pad_ctrl_oprxlos_11_ctrl; 
    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0, sizeof(BCMI_DINO_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0, sizeof(BCMI_DINO_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0, sizeof(BCMI_DINO_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0, sizeof(BCMI_DINO_GPIO_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_4_ctrl, 0, sizeof(BCMI_DINO_GPIO_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_5_ctrl, 0, sizeof(BCMI_DINO_GPIO_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_6_ctrl, 0, sizeof(BCMI_DINO_GPIO_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_7_ctrl, 0, sizeof(BCMI_DINO_GPIO_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_8_ctrl, 0, sizeof(BCMI_DINO_GPIO_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_9_ctrl, 0, sizeof(BCMI_DINO_GPIO_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_10_ctrl, 0, sizeof(BCMI_DINO_GPIO_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_11_ctrl, 0, sizeof(BCMI_DINO_GPIO_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_12_ctrl, 0, sizeof(BCMI_DINO_GPIO_12_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_13_ctrl, 0, sizeof(BCMI_DINO_GPIO_13_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_14_ctrl, 0, sizeof(BCMI_DINO_GPIO_14_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_15_ctrl, 0, sizeof(BCMI_DINO_GPIO_15_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_16_ctrl, 0, sizeof(BCMI_DINO_GPIO_16_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_17_ctrl, 0, sizeof(BCMI_DINO_GPIO_17_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_a, 0, sizeof(BCMI_DINO_OPTXENB1_A_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_b, 0, sizeof(BCMI_DINO_OPTXENB1_B_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_c, 0, sizeof(BCMI_DINO_OPTXENB1_C_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_3_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_4_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_5_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_6_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_7_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_8_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_9_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_10_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_11_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_4_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_5_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_6_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_7_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_8_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_9_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_10_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_11_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_11_CTLr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    if (chip_id == DINO_CHIP_ID_82793 && pin_number > 11) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 11)")));
    }
    
    if (chip_id == DINO_CHIP_ID_82332 && pin_number > 37) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 37)")));
    }

    switch (pin_number) {
        case 0:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_0_CTLr(pa, &pad_ctrl_gpio_0_ctrl));
        break;
        case 1:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_1_CTLr(pa, &pad_ctrl_gpio_1_ctrl));
        break;
        case 2:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_2_CTLr(pa, &pad_ctrl_gpio_2_ctrl));
        break;
        case 3:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_3_CTLr(pa, &pad_ctrl_gpio_3_ctrl));
        break;
        case 4:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_4_CTLr(pa, &pad_ctrl_gpio_4_ctrl));
        break;
        case 5:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_5_CTLr(pa, &pad_ctrl_gpio_5_ctrl));
        break;
        case 6:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_6_CTLr(pa, &pad_ctrl_gpio_6_ctrl));
        break;
        case 7:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_7_CTLr(pa, &pad_ctrl_gpio_7_ctrl));
        break;
        case 8:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_8_CTLr(pa, &pad_ctrl_gpio_8_ctrl));
        break;
        case 9:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_9_CTLr(pa, &pad_ctrl_gpio_9_ctrl));
        break;
        case 10:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_10_CTLr(pa, &pad_ctrl_gpio_10_ctrl));
        break;
        case 11:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_11_CTLr(pa, &pad_ctrl_gpio_11_ctrl));
        break;
        case 12:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_12_CTLr(pa, &pad_ctrl_gpio_12_ctrl));
        break;
        case 13:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_13_CTLr(pa, &pad_ctrl_gpio_13_ctrl));
        break;
        case 14:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_14_CTLr(pa, &pad_ctrl_gpio_14_ctrl));
        break;
        case 15:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_15_CTLr(pa, &pad_ctrl_gpio_15_ctrl));
        break;
        case 16:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_16_CTLr(pa, &pad_ctrl_gpio_16_ctrl));
        break;
        case 17:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_17_CTLr(pa, &pad_ctrl_gpio_17_ctrl));
        break;
        case 18:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_A_CTLr(pa, &pad_ctrl_optxenb1_a));
        break;
        case 19:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_B_CTLr(pa, &pad_ctrl_optxenb1_b));
        break;
        case 20:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_C_CTLr(pa, &pad_ctrl_optxenb1_c));
        break;
        case 21:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_3_CTLr(pa, &pad_ctrl_mod_abs_3_ctrl));
        break;
        case 22:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_4_CTLr(pa, &pad_ctrl_mod_abs_4_ctrl));
        break;
        case 23:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_5_CTLr(pa, &pad_ctrl_mod_abs_5_ctrl));
        break;
        case 24:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_6_CTLr(pa, &pad_ctrl_mod_abs_6_ctrl));
        break;
        case 25:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_7_CTLr(pa, &pad_ctrl_mod_abs_7_ctrl));
        break;
        case 26:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_8_CTLr(pa, &pad_ctrl_mod_abs_8_ctrl));
        break;
        case 27:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_9_CTLr(pa, &pad_ctrl_mod_abs_9_ctrl));
        break;
        case 28:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_10_CTLr(pa, &pad_ctrl_mod_abs_10_ctrl));
        break;
        case 29:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_11_CTLr(pa, &pad_ctrl_mod_abs_11_ctrl));
        break;
        case 30:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_4_CTLr(pa, &pad_ctrl_oprxlos_4_ctrl));
        break;
        case 31:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_5_CTLr(pa, &pad_ctrl_oprxlos_5_ctrl));
        break;
        case 32:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_6_CTLr(pa, &pad_ctrl_oprxlos_6_ctrl));
        break;
        case 33:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_7_CTLr(pa, &pad_ctrl_oprxlos_7_ctrl));
        break;
        case 34:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_8_CTLr(pa, &pad_ctrl_oprxlos_8_ctrl));
        break;
        case 35:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_9_CTLr(pa, &pad_ctrl_oprxlos_9_ctrl));
        break;
        case 36:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_10_CTLr(pa, &pad_ctrl_oprxlos_10_ctrl));
        break;
        case 37:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_11_CTLr(pa, &pad_ctrl_oprxlos_11_ctrl));
        break;
        default:
          return PHYMOD_E_PARAM;
    }

    switch (gpio_mode) {
        case phymodGpioModeDisabled:
            return PHYMOD_E_NONE;
        case phymodGpioModeOutput:
            data = 0;
        break;
        case phymodGpioModeInput:
            data = 1;
        break;
        default:
          return PHYMOD_E_PARAM;
    }

    switch (pin_number) {
        case 0:
            BCMI_DINO_GPIO_0_CTLr_GPIO_0_OEBf_SET(pad_ctrl_gpio_0_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_0_CTLr(pa, pad_ctrl_gpio_0_ctrl));
        break;
        case 1:    
            BCMI_DINO_GPIO_1_CTLr_GPIO_1_OEBf_SET(pad_ctrl_gpio_1_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_1_CTLr(pa,pad_ctrl_gpio_1_ctrl));
        break;
        case 2:    
            BCMI_DINO_GPIO_2_CTLr_GPIO_2_OEBf_SET(pad_ctrl_gpio_2_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_2_CTLr(pa,pad_ctrl_gpio_2_ctrl));
        break;
        case 3:    
            BCMI_DINO_GPIO_3_CTLr_GPIO_3_OEBf_SET(pad_ctrl_gpio_3_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_3_CTLr(pa,pad_ctrl_gpio_3_ctrl));
        break;
        case 4:    
            BCMI_DINO_GPIO_4_CTLr_GPIO_4_OEBf_SET(pad_ctrl_gpio_4_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_4_CTLr(pa,pad_ctrl_gpio_4_ctrl));
        break;
        case 5:    
            BCMI_DINO_GPIO_5_CTLr_GPIO_5_OEBf_SET(pad_ctrl_gpio_5_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_5_CTLr(pa,pad_ctrl_gpio_5_ctrl));
        break;
        case 6:    
            BCMI_DINO_GPIO_6_CTLr_GPIO_6_OEBf_SET(pad_ctrl_gpio_6_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_6_CTLr(pa,pad_ctrl_gpio_6_ctrl));
        break;
        case 7:    
            BCMI_DINO_GPIO_7_CTLr_GPIO_7_OEBf_SET(pad_ctrl_gpio_7_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_7_CTLr(pa,pad_ctrl_gpio_7_ctrl));
        break;
        case 8:    
            BCMI_DINO_GPIO_8_CTLr_GPIO_8_OEBf_SET(pad_ctrl_gpio_8_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_8_CTLr(pa,pad_ctrl_gpio_8_ctrl));
        break;
        case 9:    
            BCMI_DINO_GPIO_9_CTLr_GPIO_9_OEBf_SET(pad_ctrl_gpio_9_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_9_CTLr(pa,pad_ctrl_gpio_9_ctrl));
        break;
        case 10:    
            BCMI_DINO_GPIO_10_CTLr_GPIO_10_OEBf_SET(pad_ctrl_gpio_10_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_10_CTLr(pa,pad_ctrl_gpio_10_ctrl));
        break;
        case 11:    
            BCMI_DINO_GPIO_11_CTLr_GPIO_11_OEBf_SET(pad_ctrl_gpio_11_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_11_CTLr(pa,pad_ctrl_gpio_11_ctrl));
        break;
        case 12:    
            BCMI_DINO_GPIO_12_CTLr_GPIO_12_OEBf_SET(pad_ctrl_gpio_12_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_12_CTLr(pa,pad_ctrl_gpio_12_ctrl));
        break;
        case 13:    
            BCMI_DINO_GPIO_13_CTLr_GPIO_13_OEBf_SET(pad_ctrl_gpio_13_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_13_CTLr(pa,pad_ctrl_gpio_13_ctrl));
        break;
        case 14:    
            BCMI_DINO_GPIO_14_CTLr_GPIO_14_OEBf_SET(pad_ctrl_gpio_14_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_14_CTLr(pa,pad_ctrl_gpio_14_ctrl));
        break;
        case 15:    
            BCMI_DINO_GPIO_15_CTLr_GPIO_15_OEBf_SET(pad_ctrl_gpio_15_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_15_CTLr(pa,pad_ctrl_gpio_15_ctrl));
        break;
        case 16:    
            BCMI_DINO_GPIO_16_CTLr_GPIO_16_OEBf_SET(pad_ctrl_gpio_16_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_16_CTLr(pa,pad_ctrl_gpio_16_ctrl));
        break;
        case 17:    
            BCMI_DINO_GPIO_17_CTLr_GPIO_17_OEBf_SET(pad_ctrl_gpio_17_ctrl, data);    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_17_CTLr(pa,pad_ctrl_gpio_17_ctrl));
        break;
        case 18:
            BCMI_DINO_OPTXENB1_A_CTLr_OPTXENB1_A_OEBf_SET(pad_ctrl_optxenb1_a, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_A_CTLr(pa, pad_ctrl_optxenb1_a));
        break;
        case 19:    
            BCMI_DINO_OPTXENB1_B_CTLr_OPTXENB1_B_OEBf_SET(pad_ctrl_optxenb1_b, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_B_CTLr(pa, pad_ctrl_optxenb1_b));
        break;
        case 20:    
            BCMI_DINO_OPTXENB1_C_CTLr_OPTXENB1_C_OEBf_SET(pad_ctrl_optxenb1_c, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_C_CTLr(pa, pad_ctrl_optxenb1_c));
        break;
        case 21:
            BCMI_DINO_MOD_ABS_3_CTLr_MOD_ABS_3_OEBf_SET(pad_ctrl_mod_abs_3_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_3_CTLr(pa, pad_ctrl_mod_abs_3_ctrl));
        break;
        case 22:    
            BCMI_DINO_MOD_ABS_4_CTLr_MOD_ABS_4_OEBf_SET(pad_ctrl_mod_abs_4_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_4_CTLr(pa, pad_ctrl_mod_abs_4_ctrl));
        break;
        case 23:    
            BCMI_DINO_MOD_ABS_5_CTLr_MOD_ABS_5_OEBf_SET(pad_ctrl_mod_abs_5_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_5_CTLr(pa, pad_ctrl_mod_abs_5_ctrl));
        break;
        case 24:    
            BCMI_DINO_MOD_ABS_6_CTLr_MOD_ABS_6_OEBf_SET(pad_ctrl_mod_abs_6_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_6_CTLr(pa, pad_ctrl_mod_abs_6_ctrl));
        break;
        case 25:    
            BCMI_DINO_MOD_ABS_7_CTLr_MOD_ABS_7_OEBf_SET(pad_ctrl_mod_abs_7_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_7_CTLr(pa, pad_ctrl_mod_abs_7_ctrl));
        break;
        case 26:    
            BCMI_DINO_MOD_ABS_8_CTLr_MOD_ABS_8_OEBf_SET(pad_ctrl_mod_abs_8_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_8_CTLr(pa, pad_ctrl_mod_abs_8_ctrl));
        break;
        case 27:    
            BCMI_DINO_MOD_ABS_9_CTLr_MOD_ABS_9_OEBf_SET(pad_ctrl_mod_abs_9_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_9_CTLr(pa, pad_ctrl_mod_abs_9_ctrl));
        break;
        case 28:    
            BCMI_DINO_MOD_ABS_10_CTLr_MOD_ABS_10_OEBf_SET(pad_ctrl_mod_abs_10_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_10_CTLr(pa, pad_ctrl_mod_abs_10_ctrl));
        break;
        case 29:    
            BCMI_DINO_MOD_ABS_11_CTLr_MOD_ABS_11_OEBf_SET(pad_ctrl_mod_abs_11_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_11_CTLr(pa, pad_ctrl_mod_abs_11_ctrl));
        break;
        case 30:
            BCMI_DINO_OPRXLOS_4_CTLr_OPRXLOS_4_OEBf_SET(pad_ctrl_oprxlos_4_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_4_CTLr(pa, pad_ctrl_oprxlos_4_ctrl));
        break;
        case 31:    
            BCMI_DINO_OPRXLOS_5_CTLr_OPRXLOS_5_OEBf_SET(pad_ctrl_oprxlos_5_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_5_CTLr(pa, pad_ctrl_oprxlos_5_ctrl));
        break;
        case 32:    
            BCMI_DINO_OPRXLOS_6_CTLr_OPRXLOS_6_OEBf_SET(pad_ctrl_oprxlos_6_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_6_CTLr(pa, pad_ctrl_oprxlos_6_ctrl));
        break;
        case 33:    
            BCMI_DINO_OPRXLOS_7_CTLr_OPRXLOS_7_OEBf_SET(pad_ctrl_oprxlos_7_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_7_CTLr(pa, pad_ctrl_oprxlos_7_ctrl));
        break;
        case 34:    
            BCMI_DINO_OPRXLOS_8_CTLr_OPRXLOS_8_OEBf_SET(pad_ctrl_oprxlos_8_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_8_CTLr(pa, pad_ctrl_oprxlos_8_ctrl));
        break;
        case 35:    
            BCMI_DINO_OPRXLOS_9_CTLr_OPRXLOS_9_OEBf_SET(pad_ctrl_oprxlos_9_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_9_CTLr(pa, pad_ctrl_oprxlos_9_ctrl));
        break;
        case 36:    
            BCMI_DINO_OPRXLOS_10_CTLr_OPRXLOS_10_OEBf_SET(pad_ctrl_oprxlos_10_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_10_CTLr(pa, pad_ctrl_oprxlos_10_ctrl));
        break;
        case 37:    
            BCMI_DINO_OPRXLOS_11_CTLr_OPRXLOS_11_OEBf_SET(pad_ctrl_oprxlos_11_ctrl, data);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_11_CTLr(pa, pad_ctrl_oprxlos_11_ctrl));
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_gpio_config_get(const phymod_access_t* pa, int pin_number, phymod_gpio_mode_t* gpio_mode)
{
    uint16_t data = 0;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    BCMI_DINO_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_DINO_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_DINO_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_DINO_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;
    BCMI_DINO_GPIO_4_CTLr_t pad_ctrl_gpio_4_ctrl;
    BCMI_DINO_GPIO_5_CTLr_t pad_ctrl_gpio_5_ctrl;
    BCMI_DINO_GPIO_6_CTLr_t pad_ctrl_gpio_6_ctrl;
    BCMI_DINO_GPIO_7_CTLr_t pad_ctrl_gpio_7_ctrl;
    BCMI_DINO_GPIO_8_CTLr_t pad_ctrl_gpio_8_ctrl;
    BCMI_DINO_GPIO_9_CTLr_t pad_ctrl_gpio_9_ctrl;
    BCMI_DINO_GPIO_10_CTLr_t pad_ctrl_gpio_10_ctrl;
    BCMI_DINO_GPIO_11_CTLr_t pad_ctrl_gpio_11_ctrl;
    BCMI_DINO_GPIO_12_CTLr_t pad_ctrl_gpio_12_ctrl;
    BCMI_DINO_GPIO_13_CTLr_t pad_ctrl_gpio_13_ctrl;
    BCMI_DINO_GPIO_14_CTLr_t pad_ctrl_gpio_14_ctrl;
    BCMI_DINO_GPIO_15_CTLr_t pad_ctrl_gpio_15_ctrl;
    BCMI_DINO_GPIO_16_CTLr_t pad_ctrl_gpio_16_ctrl;
    BCMI_DINO_GPIO_17_CTLr_t pad_ctrl_gpio_17_ctrl;
    BCMI_DINO_OPTXENB1_A_CTLr_t pad_ctrl_optxenb1_a;
    BCMI_DINO_OPTXENB1_B_CTLr_t pad_ctrl_optxenb1_b;
    BCMI_DINO_OPTXENB1_C_CTLr_t pad_ctrl_optxenb1_c;
    BCMI_DINO_MOD_ABS_3_CTLr_t pad_ctrl_mod_abs_3_ctrl;
    BCMI_DINO_MOD_ABS_4_CTLr_t pad_ctrl_mod_abs_4_ctrl;
    BCMI_DINO_MOD_ABS_5_CTLr_t pad_ctrl_mod_abs_5_ctrl;
    BCMI_DINO_MOD_ABS_6_CTLr_t pad_ctrl_mod_abs_6_ctrl;
    BCMI_DINO_MOD_ABS_7_CTLr_t pad_ctrl_mod_abs_7_ctrl;
    BCMI_DINO_MOD_ABS_8_CTLr_t pad_ctrl_mod_abs_8_ctrl;
    BCMI_DINO_MOD_ABS_9_CTLr_t pad_ctrl_mod_abs_9_ctrl;
    BCMI_DINO_MOD_ABS_10_CTLr_t pad_ctrl_mod_abs_10_ctrl;
    BCMI_DINO_MOD_ABS_11_CTLr_t pad_ctrl_mod_abs_11_ctrl;
    BCMI_DINO_OPRXLOS_4_CTLr_t  pad_ctrl_oprxlos_4_ctrl; 
    BCMI_DINO_OPRXLOS_5_CTLr_t  pad_ctrl_oprxlos_5_ctrl; 
    BCMI_DINO_OPRXLOS_6_CTLr_t  pad_ctrl_oprxlos_6_ctrl; 
    BCMI_DINO_OPRXLOS_7_CTLr_t  pad_ctrl_oprxlos_7_ctrl; 
    BCMI_DINO_OPRXLOS_8_CTLr_t  pad_ctrl_oprxlos_8_ctrl; 
    BCMI_DINO_OPRXLOS_9_CTLr_t  pad_ctrl_oprxlos_9_ctrl; 
    BCMI_DINO_OPRXLOS_10_CTLr_t  pad_ctrl_oprxlos_10_ctrl; 
    BCMI_DINO_OPRXLOS_11_CTLr_t  pad_ctrl_oprxlos_11_ctrl; 

    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0, sizeof(BCMI_DINO_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0, sizeof(BCMI_DINO_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0, sizeof(BCMI_DINO_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0, sizeof(BCMI_DINO_GPIO_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_4_ctrl, 0, sizeof(BCMI_DINO_GPIO_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_5_ctrl, 0, sizeof(BCMI_DINO_GPIO_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_6_ctrl, 0, sizeof(BCMI_DINO_GPIO_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_7_ctrl, 0, sizeof(BCMI_DINO_GPIO_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_8_ctrl, 0, sizeof(BCMI_DINO_GPIO_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_9_ctrl, 0, sizeof(BCMI_DINO_GPIO_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_10_ctrl, 0, sizeof(BCMI_DINO_GPIO_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_11_ctrl, 0, sizeof(BCMI_DINO_GPIO_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_12_ctrl, 0, sizeof(BCMI_DINO_GPIO_12_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_13_ctrl, 0, sizeof(BCMI_DINO_GPIO_13_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_14_ctrl, 0, sizeof(BCMI_DINO_GPIO_14_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_15_ctrl, 0, sizeof(BCMI_DINO_GPIO_15_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_16_ctrl, 0, sizeof(BCMI_DINO_GPIO_16_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_17_ctrl, 0, sizeof(BCMI_DINO_GPIO_17_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_a, 0, sizeof(BCMI_DINO_OPTXENB1_A_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_b, 0, sizeof(BCMI_DINO_OPTXENB1_B_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_c, 0, sizeof(BCMI_DINO_OPTXENB1_C_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_3_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_4_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_5_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_6_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_7_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_8_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_9_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_10_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_11_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_4_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_5_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_6_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_7_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_8_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_9_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_10_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_11_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_11_CTLr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    if (chip_id == DINO_CHIP_ID_82793 && pin_number > 11) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 11)")));
    }
    
    if (chip_id == DINO_CHIP_ID_82332 && pin_number > 37) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 37)")));
    }

    switch (pin_number) {
        case 0:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_0_CTLr(pa, &pad_ctrl_gpio_0_ctrl));
            data = BCMI_DINO_GPIO_0_CTLr_GPIO_0_OEBf_GET(pad_ctrl_gpio_0_ctrl);
        break;
        case 1:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_1_CTLr(pa, &pad_ctrl_gpio_1_ctrl));
            data = BCMI_DINO_GPIO_1_CTLr_GPIO_1_OEBf_GET(pad_ctrl_gpio_1_ctrl);
        break;
        case 2:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_2_CTLr(pa, &pad_ctrl_gpio_2_ctrl));
            data = BCMI_DINO_GPIO_2_CTLr_GPIO_2_OEBf_GET(pad_ctrl_gpio_2_ctrl);
        break;
        case 3:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_3_CTLr(pa, &pad_ctrl_gpio_3_ctrl));
            data = BCMI_DINO_GPIO_3_CTLr_GPIO_3_OEBf_GET(pad_ctrl_gpio_3_ctrl);
        break;
        case 4:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_4_CTLr(pa, &pad_ctrl_gpio_4_ctrl));
            data = BCMI_DINO_GPIO_4_CTLr_GPIO_4_OEBf_GET(pad_ctrl_gpio_4_ctrl);
        break;
        case 5:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_5_CTLr(pa, &pad_ctrl_gpio_5_ctrl));
            data = BCMI_DINO_GPIO_5_CTLr_GPIO_5_OEBf_GET(pad_ctrl_gpio_5_ctrl);
        break;
        case 6:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_6_CTLr(pa, &pad_ctrl_gpio_6_ctrl));
            data = BCMI_DINO_GPIO_6_CTLr_GPIO_6_OEBf_GET(pad_ctrl_gpio_6_ctrl);
        break;
        case 7:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_7_CTLr(pa, &pad_ctrl_gpio_7_ctrl));
            data = BCMI_DINO_GPIO_7_CTLr_GPIO_7_OEBf_GET(pad_ctrl_gpio_7_ctrl);
        break;
        case 8:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_8_CTLr(pa, &pad_ctrl_gpio_8_ctrl));
            data = BCMI_DINO_GPIO_8_CTLr_GPIO_8_OEBf_GET(pad_ctrl_gpio_8_ctrl);
        break;
        case 9:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_9_CTLr(pa, &pad_ctrl_gpio_9_ctrl));
            data = BCMI_DINO_GPIO_9_CTLr_GPIO_9_OEBf_GET(pad_ctrl_gpio_9_ctrl);
        break;
        case 10:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_10_CTLr(pa, &pad_ctrl_gpio_10_ctrl));
            data = BCMI_DINO_GPIO_10_CTLr_GPIO_10_OEBf_GET(pad_ctrl_gpio_10_ctrl);
        break;
        case 11:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_11_CTLr(pa, &pad_ctrl_gpio_11_ctrl));
            data = BCMI_DINO_GPIO_11_CTLr_GPIO_11_OEBf_GET(pad_ctrl_gpio_11_ctrl);
        break;
        case 12:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_12_CTLr(pa, &pad_ctrl_gpio_12_ctrl));
            data = BCMI_DINO_GPIO_12_CTLr_GPIO_12_OEBf_GET(pad_ctrl_gpio_12_ctrl);
        break;
        case 13:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_13_CTLr(pa, &pad_ctrl_gpio_13_ctrl));
            data = BCMI_DINO_GPIO_13_CTLr_GPIO_13_OEBf_GET(pad_ctrl_gpio_13_ctrl);
        break;
        case 14:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_14_CTLr(pa, &pad_ctrl_gpio_14_ctrl));
            data = BCMI_DINO_GPIO_14_CTLr_GPIO_14_OEBf_GET(pad_ctrl_gpio_14_ctrl);
        break;
        case 15:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_15_CTLr(pa, &pad_ctrl_gpio_15_ctrl));
            data = BCMI_DINO_GPIO_15_CTLr_GPIO_15_OEBf_GET(pad_ctrl_gpio_15_ctrl);
        break;
        case 16:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_16_CTLr(pa, &pad_ctrl_gpio_16_ctrl));
            data = BCMI_DINO_GPIO_16_CTLr_GPIO_16_OEBf_GET(pad_ctrl_gpio_16_ctrl);
        break;
        case 17:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_17_CTLr(pa, &pad_ctrl_gpio_17_ctrl));
            data = BCMI_DINO_GPIO_17_CTLr_GPIO_17_OEBf_GET(pad_ctrl_gpio_17_ctrl);
        break;
        case 18:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_A_CTLr(pa, &pad_ctrl_optxenb1_a));
            data = BCMI_DINO_OPTXENB1_A_CTLr_OPTXENB1_A_OEBf_GET(pad_ctrl_optxenb1_a);
        break;
        case 19:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_B_CTLr(pa, &pad_ctrl_optxenb1_b));
            data = BCMI_DINO_OPTXENB1_B_CTLr_OPTXENB1_B_OEBf_GET(pad_ctrl_optxenb1_b);
        break;
        case 20:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_C_CTLr(pa, &pad_ctrl_optxenb1_c));
            data = BCMI_DINO_OPTXENB1_C_CTLr_OPTXENB1_C_OEBf_GET(pad_ctrl_optxenb1_c);
        break;
        case 21:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_3_CTLr(pa, &pad_ctrl_mod_abs_3_ctrl));
            data = BCMI_DINO_MOD_ABS_3_CTLr_MOD_ABS_3_OEBf_GET(pad_ctrl_mod_abs_3_ctrl);
        break;
        case 22:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_4_CTLr(pa, &pad_ctrl_mod_abs_4_ctrl));
            data = BCMI_DINO_MOD_ABS_4_CTLr_MOD_ABS_4_OEBf_GET(pad_ctrl_mod_abs_4_ctrl);
        break;
        case 23:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_5_CTLr(pa, &pad_ctrl_mod_abs_5_ctrl));
            data = BCMI_DINO_MOD_ABS_5_CTLr_MOD_ABS_5_OEBf_GET(pad_ctrl_mod_abs_5_ctrl);
        break;
        case 24:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_6_CTLr(pa, &pad_ctrl_mod_abs_6_ctrl));
            data = BCMI_DINO_MOD_ABS_6_CTLr_MOD_ABS_6_OEBf_GET(pad_ctrl_mod_abs_6_ctrl);
        break;
        case 25:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_7_CTLr(pa, &pad_ctrl_mod_abs_7_ctrl));
            data = BCMI_DINO_MOD_ABS_7_CTLr_MOD_ABS_7_OEBf_GET(pad_ctrl_mod_abs_7_ctrl);
        break;
        case 26:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_8_CTLr(pa, &pad_ctrl_mod_abs_8_ctrl));
            data = BCMI_DINO_MOD_ABS_8_CTLr_MOD_ABS_8_OEBf_GET(pad_ctrl_mod_abs_8_ctrl);
        break;
        case 27:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_9_CTLr(pa, &pad_ctrl_mod_abs_9_ctrl));
            data = BCMI_DINO_MOD_ABS_9_CTLr_MOD_ABS_9_OEBf_GET(pad_ctrl_mod_abs_9_ctrl);
        break;
        case 28:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_10_CTLr(pa, &pad_ctrl_mod_abs_10_ctrl));
            data = BCMI_DINO_MOD_ABS_10_CTLr_MOD_ABS_10_OEBf_GET(pad_ctrl_mod_abs_10_ctrl);
        break;
        case 29:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_11_CTLr(pa, &pad_ctrl_mod_abs_11_ctrl));
            data = BCMI_DINO_MOD_ABS_11_CTLr_MOD_ABS_11_OEBf_GET(pad_ctrl_mod_abs_11_ctrl);
        break;
        case 30:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_4_CTLr(pa, &pad_ctrl_oprxlos_4_ctrl));
            data = BCMI_DINO_OPRXLOS_4_CTLr_OPRXLOS_4_OEBf_GET(pad_ctrl_oprxlos_4_ctrl);
        break;
        case 31:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_5_CTLr(pa, &pad_ctrl_oprxlos_5_ctrl));
            data = BCMI_DINO_OPRXLOS_5_CTLr_OPRXLOS_5_OEBf_GET(pad_ctrl_oprxlos_5_ctrl);
        break;
        case 32:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_6_CTLr(pa, &pad_ctrl_oprxlos_6_ctrl));
            data = BCMI_DINO_OPRXLOS_6_CTLr_OPRXLOS_6_OEBf_GET(pad_ctrl_oprxlos_6_ctrl);
        break;
        case 33:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_7_CTLr(pa, &pad_ctrl_oprxlos_7_ctrl));
            data = BCMI_DINO_OPRXLOS_7_CTLr_OPRXLOS_7_OEBf_GET(pad_ctrl_oprxlos_7_ctrl);
        break;
        case 34:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_8_CTLr(pa, &pad_ctrl_oprxlos_8_ctrl));
            data = BCMI_DINO_OPRXLOS_8_CTLr_OPRXLOS_8_OEBf_GET(pad_ctrl_oprxlos_8_ctrl);
        break;
        case 35:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_9_CTLr(pa, &pad_ctrl_oprxlos_9_ctrl));
            data = BCMI_DINO_OPRXLOS_9_CTLr_OPRXLOS_9_OEBf_GET(pad_ctrl_oprxlos_9_ctrl);
        break;
        case 36:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_10_CTLr(pa, &pad_ctrl_oprxlos_10_ctrl));
            data = BCMI_DINO_OPRXLOS_10_CTLr_OPRXLOS_10_OEBf_GET(pad_ctrl_oprxlos_10_ctrl);
        break;
        case 37:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_11_CTLr(pa, &pad_ctrl_oprxlos_11_ctrl));
            data = BCMI_DINO_OPRXLOS_11_CTLr_OPRXLOS_11_OEBf_GET(pad_ctrl_oprxlos_11_ctrl);
        break;
        default:
          return PHYMOD_E_PARAM;
    }

    switch(data) {
        case 0:
            *gpio_mode = phymodGpioModeOutput;
        break;
        case 1:
            *gpio_mode = phymodGpioModeInput;
        break;
        /* coverity[dead_error_begin] */
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_gpio_pin_value_set(const phymod_access_t* pa, int pin_number, int value)
{
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    BCMI_DINO_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_DINO_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_DINO_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_DINO_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;
    BCMI_DINO_GPIO_4_CTLr_t pad_ctrl_gpio_4_ctrl;
    BCMI_DINO_GPIO_5_CTLr_t pad_ctrl_gpio_5_ctrl;
    BCMI_DINO_GPIO_6_CTLr_t pad_ctrl_gpio_6_ctrl;
    BCMI_DINO_GPIO_7_CTLr_t pad_ctrl_gpio_7_ctrl;
    BCMI_DINO_GPIO_8_CTLr_t pad_ctrl_gpio_8_ctrl;
    BCMI_DINO_GPIO_9_CTLr_t pad_ctrl_gpio_9_ctrl;
    BCMI_DINO_GPIO_10_CTLr_t pad_ctrl_gpio_10_ctrl;
    BCMI_DINO_GPIO_11_CTLr_t pad_ctrl_gpio_11_ctrl;
    BCMI_DINO_GPIO_12_CTLr_t pad_ctrl_gpio_12_ctrl;
    BCMI_DINO_GPIO_13_CTLr_t pad_ctrl_gpio_13_ctrl;
    BCMI_DINO_GPIO_14_CTLr_t pad_ctrl_gpio_14_ctrl;
    BCMI_DINO_GPIO_15_CTLr_t pad_ctrl_gpio_15_ctrl;
    BCMI_DINO_GPIO_16_CTLr_t pad_ctrl_gpio_16_ctrl;
    BCMI_DINO_GPIO_17_CTLr_t pad_ctrl_gpio_17_ctrl;
    BCMI_DINO_OPTXENB1_A_CTLr_t pad_ctrl_optxenb1_a;
    BCMI_DINO_OPTXENB1_B_CTLr_t pad_ctrl_optxenb1_b;
    BCMI_DINO_OPTXENB1_C_CTLr_t pad_ctrl_optxenb1_c;
    BCMI_DINO_MOD_ABS_3_CTLr_t pad_ctrl_mod_abs_3_ctrl;
    BCMI_DINO_MOD_ABS_4_CTLr_t pad_ctrl_mod_abs_4_ctrl;
    BCMI_DINO_MOD_ABS_5_CTLr_t pad_ctrl_mod_abs_5_ctrl;
    BCMI_DINO_MOD_ABS_6_CTLr_t pad_ctrl_mod_abs_6_ctrl;
    BCMI_DINO_MOD_ABS_7_CTLr_t pad_ctrl_mod_abs_7_ctrl;
    BCMI_DINO_MOD_ABS_8_CTLr_t pad_ctrl_mod_abs_8_ctrl;
    BCMI_DINO_MOD_ABS_9_CTLr_t pad_ctrl_mod_abs_9_ctrl;
    BCMI_DINO_MOD_ABS_10_CTLr_t pad_ctrl_mod_abs_10_ctrl;
    BCMI_DINO_MOD_ABS_11_CTLr_t pad_ctrl_mod_abs_11_ctrl;
    BCMI_DINO_OPRXLOS_4_CTLr_t  pad_ctrl_oprxlos_4_ctrl; 
    BCMI_DINO_OPRXLOS_5_CTLr_t  pad_ctrl_oprxlos_5_ctrl; 
    BCMI_DINO_OPRXLOS_6_CTLr_t  pad_ctrl_oprxlos_6_ctrl; 
    BCMI_DINO_OPRXLOS_7_CTLr_t  pad_ctrl_oprxlos_7_ctrl; 
    BCMI_DINO_OPRXLOS_8_CTLr_t  pad_ctrl_oprxlos_8_ctrl; 
    BCMI_DINO_OPRXLOS_9_CTLr_t  pad_ctrl_oprxlos_9_ctrl; 
    BCMI_DINO_OPRXLOS_10_CTLr_t  pad_ctrl_oprxlos_10_ctrl; 
    BCMI_DINO_OPRXLOS_11_CTLr_t  pad_ctrl_oprxlos_11_ctrl; 

    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0, sizeof(BCMI_DINO_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0, sizeof(BCMI_DINO_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0, sizeof(BCMI_DINO_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0, sizeof(BCMI_DINO_GPIO_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_4_ctrl, 0, sizeof(BCMI_DINO_GPIO_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_5_ctrl, 0, sizeof(BCMI_DINO_GPIO_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_6_ctrl, 0, sizeof(BCMI_DINO_GPIO_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_7_ctrl, 0, sizeof(BCMI_DINO_GPIO_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_8_ctrl, 0, sizeof(BCMI_DINO_GPIO_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_9_ctrl, 0, sizeof(BCMI_DINO_GPIO_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_10_ctrl, 0, sizeof(BCMI_DINO_GPIO_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_11_ctrl, 0, sizeof(BCMI_DINO_GPIO_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_12_ctrl, 0, sizeof(BCMI_DINO_GPIO_12_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_13_ctrl, 0, sizeof(BCMI_DINO_GPIO_13_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_14_ctrl, 0, sizeof(BCMI_DINO_GPIO_14_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_15_ctrl, 0, sizeof(BCMI_DINO_GPIO_15_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_16_ctrl, 0, sizeof(BCMI_DINO_GPIO_16_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_17_ctrl, 0, sizeof(BCMI_DINO_GPIO_17_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_a, 0, sizeof(BCMI_DINO_OPTXENB1_A_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_b, 0, sizeof(BCMI_DINO_OPTXENB1_B_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_optxenb1_c, 0, sizeof(BCMI_DINO_OPTXENB1_C_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_3_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_3_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_4_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_5_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_6_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_7_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_8_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_9_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_10_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_mod_abs_11_ctrl, 0, sizeof(BCMI_DINO_MOD_ABS_11_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_4_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_4_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_5_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_5_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_6_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_6_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_7_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_7_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_8_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_8_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_9_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_9_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_10_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_10_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_oprxlos_11_ctrl, 0, sizeof(BCMI_DINO_OPRXLOS_11_CTLr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    if (chip_id == DINO_CHIP_ID_82793 && pin_number > 11) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 11)")));
    }
    
    if (chip_id == DINO_CHIP_ID_82332 && pin_number > 37) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 37)")));
    }

    switch (pin_number) {
        case 0:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_0_CTLr(pa, &pad_ctrl_gpio_0_ctrl));
            BCMI_DINO_GPIO_0_CTLr_GPIO_0_IBOFf_SET(pad_ctrl_gpio_0_ctrl, 1);
            BCMI_DINO_GPIO_0_CTLr_GPIO_0_OUT_FRCVALf_SET(pad_ctrl_gpio_0_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_0_CTLr(pa, pad_ctrl_gpio_0_ctrl));
        break;
        case 1:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_1_CTLr(pa, &pad_ctrl_gpio_1_ctrl));
            BCMI_DINO_GPIO_1_CTLr_GPIO_1_IBOFf_SET(pad_ctrl_gpio_1_ctrl, 1);
            BCMI_DINO_GPIO_1_CTLr_GPIO_1_OUT_FRCVALf_SET(pad_ctrl_gpio_1_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_1_CTLr(pa,pad_ctrl_gpio_1_ctrl));
        break;
        case 2:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_2_CTLr(pa, &pad_ctrl_gpio_2_ctrl));
            BCMI_DINO_GPIO_2_CTLr_GPIO_2_IBOFf_SET(pad_ctrl_gpio_2_ctrl, 1);
            BCMI_DINO_GPIO_2_CTLr_GPIO_2_OUT_FRCVALf_SET(pad_ctrl_gpio_2_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_2_CTLr(pa,pad_ctrl_gpio_2_ctrl));
        break;
        case 3:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_3_CTLr(pa, &pad_ctrl_gpio_3_ctrl));
            BCMI_DINO_GPIO_3_CTLr_GPIO_3_IBOFf_SET(pad_ctrl_gpio_3_ctrl, 1);
            BCMI_DINO_GPIO_3_CTLr_GPIO_3_OUT_FRCVALf_SET(pad_ctrl_gpio_3_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_3_CTLr(pa,pad_ctrl_gpio_3_ctrl));
        break;
        case 4:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_4_CTLr(pa, &pad_ctrl_gpio_4_ctrl));
            BCMI_DINO_GPIO_4_CTLr_GPIO_4_IBOFf_SET(pad_ctrl_gpio_4_ctrl, 1);
            BCMI_DINO_GPIO_4_CTLr_GPIO_4_OUT_FRCVALf_SET(pad_ctrl_gpio_4_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_4_CTLr(pa,pad_ctrl_gpio_4_ctrl));
        break;
        case 5:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_5_CTLr(pa, &pad_ctrl_gpio_5_ctrl));
            BCMI_DINO_GPIO_5_CTLr_GPIO_5_IBOFf_SET(pad_ctrl_gpio_5_ctrl, 1);
            BCMI_DINO_GPIO_5_CTLr_GPIO_5_OUT_FRCVALf_SET(pad_ctrl_gpio_5_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_5_CTLr(pa,pad_ctrl_gpio_5_ctrl));
        break;
        case 6:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_6_CTLr(pa, &pad_ctrl_gpio_6_ctrl));
            BCMI_DINO_GPIO_6_CTLr_GPIO_6_IBOFf_SET(pad_ctrl_gpio_6_ctrl, 1);
            BCMI_DINO_GPIO_6_CTLr_GPIO_6_OUT_FRCVALf_SET(pad_ctrl_gpio_6_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_6_CTLr(pa,pad_ctrl_gpio_6_ctrl));
        break;
        case 7:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_7_CTLr(pa, &pad_ctrl_gpio_7_ctrl));
            BCMI_DINO_GPIO_7_CTLr_GPIO_7_IBOFf_SET(pad_ctrl_gpio_7_ctrl, 1);
            BCMI_DINO_GPIO_7_CTLr_GPIO_7_OUT_FRCVALf_SET(pad_ctrl_gpio_7_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_7_CTLr(pa,pad_ctrl_gpio_7_ctrl));
        break;
        case 8:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_8_CTLr(pa, &pad_ctrl_gpio_8_ctrl));
            BCMI_DINO_GPIO_8_CTLr_GPIO_8_IBOFf_SET(pad_ctrl_gpio_8_ctrl, 1);
            BCMI_DINO_GPIO_8_CTLr_GPIO_8_OUT_FRCVALf_SET(pad_ctrl_gpio_8_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_8_CTLr(pa,pad_ctrl_gpio_8_ctrl));
        break;
        case 9:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_9_CTLr(pa, &pad_ctrl_gpio_9_ctrl));
            BCMI_DINO_GPIO_9_CTLr_GPIO_9_IBOFf_SET(pad_ctrl_gpio_9_ctrl, 1);
            BCMI_DINO_GPIO_9_CTLr_GPIO_9_OUT_FRCVALf_SET(pad_ctrl_gpio_9_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_9_CTLr(pa,pad_ctrl_gpio_9_ctrl));
        break;
        case 10:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_10_CTLr(pa, &pad_ctrl_gpio_10_ctrl));
            BCMI_DINO_GPIO_10_CTLr_GPIO_10_IBOFf_SET(pad_ctrl_gpio_10_ctrl, 1);
            BCMI_DINO_GPIO_10_CTLr_GPIO_10_OUT_FRCVALf_SET(pad_ctrl_gpio_10_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_10_CTLr(pa,pad_ctrl_gpio_10_ctrl));
        break;
        case 11:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_11_CTLr(pa, &pad_ctrl_gpio_11_ctrl));
            BCMI_DINO_GPIO_11_CTLr_GPIO_11_IBOFf_SET(pad_ctrl_gpio_11_ctrl, 1);
            BCMI_DINO_GPIO_11_CTLr_GPIO_11_OUT_FRCVALf_SET(pad_ctrl_gpio_11_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_11_CTLr(pa,pad_ctrl_gpio_11_ctrl));
        break;
        case 12:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_12_CTLr(pa, &pad_ctrl_gpio_12_ctrl));
            BCMI_DINO_GPIO_12_CTLr_GPIO_12_IBOFf_SET(pad_ctrl_gpio_12_ctrl, 1);
            BCMI_DINO_GPIO_12_CTLr_GPIO_12_OUT_FRCVALf_SET(pad_ctrl_gpio_12_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_12_CTLr(pa,pad_ctrl_gpio_12_ctrl));
        break;
        case 13:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_13_CTLr(pa, &pad_ctrl_gpio_13_ctrl));
            BCMI_DINO_GPIO_13_CTLr_GPIO_13_IBOFf_SET(pad_ctrl_gpio_13_ctrl, 1);
            BCMI_DINO_GPIO_13_CTLr_GPIO_13_OUT_FRCVALf_SET(pad_ctrl_gpio_13_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_13_CTLr(pa,pad_ctrl_gpio_13_ctrl));
        break;
        case 14:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_14_CTLr(pa, &pad_ctrl_gpio_14_ctrl));
            BCMI_DINO_GPIO_14_CTLr_GPIO_14_IBOFf_SET(pad_ctrl_gpio_14_ctrl, 1);
            BCMI_DINO_GPIO_14_CTLr_GPIO_14_OUT_FRCVALf_SET(pad_ctrl_gpio_14_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_14_CTLr(pa,pad_ctrl_gpio_14_ctrl));
        break;
        case 15:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_15_CTLr(pa, &pad_ctrl_gpio_15_ctrl));
            BCMI_DINO_GPIO_15_CTLr_GPIO_15_IBOFf_SET(pad_ctrl_gpio_15_ctrl, 1);
            BCMI_DINO_GPIO_15_CTLr_GPIO_15_OUT_FRCVALf_SET(pad_ctrl_gpio_15_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_15_CTLr(pa,pad_ctrl_gpio_15_ctrl));
        break;
        case 16:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_16_CTLr(pa, &pad_ctrl_gpio_16_ctrl));
            BCMI_DINO_GPIO_16_CTLr_GPIO_16_IBOFf_SET(pad_ctrl_gpio_16_ctrl, 1);
            BCMI_DINO_GPIO_16_CTLr_GPIO_16_OUT_FRCVALf_SET(pad_ctrl_gpio_16_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_16_CTLr(pa,pad_ctrl_gpio_16_ctrl));
        break;
        case 17:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_17_CTLr(pa, &pad_ctrl_gpio_17_ctrl));
            BCMI_DINO_GPIO_17_CTLr_GPIO_17_IBOFf_SET(pad_ctrl_gpio_17_ctrl, 1);
            BCMI_DINO_GPIO_17_CTLr_GPIO_17_OUT_FRCVALf_SET(pad_ctrl_gpio_17_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_GPIO_17_CTLr(pa,pad_ctrl_gpio_17_ctrl));
        break;
        case 18:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_A_CTLr(pa, &pad_ctrl_optxenb1_a));
            BCMI_DINO_OPTXENB1_A_CTLr_OPTXENB1_A_IBOFf_SET(pad_ctrl_optxenb1_a, 1);
            BCMI_DINO_OPTXENB1_A_CTLr_OPTXENB1_A_OUT_FRCVALf_SET(pad_ctrl_optxenb1_a, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_A_CTLr(pa, pad_ctrl_optxenb1_a));
        break;
        case 19:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_B_CTLr(pa, &pad_ctrl_optxenb1_b));
            BCMI_DINO_OPTXENB1_B_CTLr_OPTXENB1_B_IBOFf_SET(pad_ctrl_optxenb1_b, 1);
            BCMI_DINO_OPTXENB1_B_CTLr_OPTXENB1_B_OUT_FRCVALf_SET(pad_ctrl_optxenb1_b, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_B_CTLr(pa, pad_ctrl_optxenb1_b));
        break;
        case 20:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_C_CTLr(pa, &pad_ctrl_optxenb1_c));
            BCMI_DINO_OPTXENB1_C_CTLr_OPTXENB1_C_IBOFf_SET(pad_ctrl_optxenb1_c, 1);
            BCMI_DINO_OPTXENB1_C_CTLr_OPTXENB1_C_OUT_FRCVALf_SET(pad_ctrl_optxenb1_c, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPTXENB1_C_CTLr(pa, pad_ctrl_optxenb1_c));
        break;
        case 21:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_3_CTLr(pa, &pad_ctrl_mod_abs_3_ctrl));
            BCMI_DINO_MOD_ABS_3_CTLr_MOD_ABS_3_IBOFf_SET(pad_ctrl_mod_abs_3_ctrl, 1);
            BCMI_DINO_MOD_ABS_3_CTLr_MOD_ABS_3_OUT_FRCVALf_SET(pad_ctrl_mod_abs_3_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_3_CTLr(pa, pad_ctrl_mod_abs_3_ctrl));
        break;
        case 22:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_4_CTLr(pa, &pad_ctrl_mod_abs_4_ctrl));
            BCMI_DINO_MOD_ABS_4_CTLr_MOD_ABS_4_IBOFf_SET(pad_ctrl_mod_abs_4_ctrl, 1);
            BCMI_DINO_MOD_ABS_4_CTLr_MOD_ABS_4_OUT_FRCVALf_SET(pad_ctrl_mod_abs_4_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_4_CTLr(pa, pad_ctrl_mod_abs_4_ctrl));
        break;
        case 23:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_5_CTLr(pa, &pad_ctrl_mod_abs_5_ctrl));
            BCMI_DINO_MOD_ABS_5_CTLr_MOD_ABS_5_IBOFf_SET(pad_ctrl_mod_abs_5_ctrl, 1);
            BCMI_DINO_MOD_ABS_5_CTLr_MOD_ABS_5_OUT_FRCVALf_SET(pad_ctrl_mod_abs_5_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_5_CTLr(pa, pad_ctrl_mod_abs_5_ctrl));
        break;
        case 24:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_6_CTLr(pa, &pad_ctrl_mod_abs_6_ctrl));
            BCMI_DINO_MOD_ABS_6_CTLr_MOD_ABS_6_IBOFf_SET(pad_ctrl_mod_abs_6_ctrl, 1);
            BCMI_DINO_MOD_ABS_6_CTLr_MOD_ABS_6_OUT_FRCVALf_SET(pad_ctrl_mod_abs_6_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_6_CTLr(pa, pad_ctrl_mod_abs_6_ctrl));
        break;
        case 25:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_7_CTLr(pa, &pad_ctrl_mod_abs_7_ctrl));
            BCMI_DINO_MOD_ABS_7_CTLr_MOD_ABS_7_IBOFf_SET(pad_ctrl_mod_abs_7_ctrl, 1);
            BCMI_DINO_MOD_ABS_7_CTLr_MOD_ABS_7_OUT_FRCVALf_SET(pad_ctrl_mod_abs_7_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_7_CTLr(pa, pad_ctrl_mod_abs_7_ctrl));
        break;
        case 26:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_8_CTLr(pa, &pad_ctrl_mod_abs_8_ctrl));
            BCMI_DINO_MOD_ABS_8_CTLr_MOD_ABS_8_IBOFf_SET(pad_ctrl_mod_abs_8_ctrl,1 );
            BCMI_DINO_MOD_ABS_8_CTLr_MOD_ABS_8_OUT_FRCVALf_SET(pad_ctrl_mod_abs_8_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_8_CTLr(pa, pad_ctrl_mod_abs_8_ctrl));
        break;
        case 27:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_9_CTLr(pa, &pad_ctrl_mod_abs_9_ctrl));
            BCMI_DINO_MOD_ABS_9_CTLr_MOD_ABS_9_IBOFf_SET(pad_ctrl_mod_abs_9_ctrl, 1);
            BCMI_DINO_MOD_ABS_9_CTLr_MOD_ABS_9_OUT_FRCVALf_SET(pad_ctrl_mod_abs_9_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_9_CTLr(pa, pad_ctrl_mod_abs_9_ctrl));
        break;
        case 28:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_10_CTLr(pa, &pad_ctrl_mod_abs_10_ctrl));
            BCMI_DINO_MOD_ABS_10_CTLr_MOD_ABS_10_IBOFf_SET(pad_ctrl_mod_abs_10_ctrl, 1);
            BCMI_DINO_MOD_ABS_10_CTLr_MOD_ABS_10_OUT_FRCVALf_SET(pad_ctrl_mod_abs_10_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_10_CTLr(pa, pad_ctrl_mod_abs_10_ctrl));
        break;
        case 29:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_11_CTLr(pa, &pad_ctrl_mod_abs_11_ctrl));
            BCMI_DINO_MOD_ABS_11_CTLr_MOD_ABS_11_IBOFf_SET(pad_ctrl_mod_abs_11_ctrl, 1);
            BCMI_DINO_MOD_ABS_11_CTLr_MOD_ABS_11_OUT_FRCVALf_SET(pad_ctrl_mod_abs_11_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_MOD_ABS_11_CTLr(pa, pad_ctrl_mod_abs_11_ctrl));
        break;
        case 30:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_4_CTLr(pa, &pad_ctrl_oprxlos_4_ctrl));
            BCMI_DINO_OPRXLOS_4_CTLr_OPRXLOS_4_IBOFf_SET(pad_ctrl_oprxlos_4_ctrl, 1);
            BCMI_DINO_OPRXLOS_4_CTLr_OPRXLOS_4_OUT_FRCVALf_SET(pad_ctrl_oprxlos_4_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_4_CTLr(pa, pad_ctrl_oprxlos_4_ctrl));
        break;
        case 31:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_5_CTLr(pa, &pad_ctrl_oprxlos_5_ctrl));
            BCMI_DINO_OPRXLOS_5_CTLr_OPRXLOS_5_IBOFf_SET(pad_ctrl_oprxlos_5_ctrl, 1);
            BCMI_DINO_OPRXLOS_5_CTLr_OPRXLOS_5_OUT_FRCVALf_SET(pad_ctrl_oprxlos_5_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_5_CTLr(pa, pad_ctrl_oprxlos_5_ctrl));
        break;
        case 32:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_6_CTLr(pa, &pad_ctrl_oprxlos_6_ctrl));
            BCMI_DINO_OPRXLOS_6_CTLr_OPRXLOS_6_IBOFf_SET(pad_ctrl_oprxlos_6_ctrl, 1);
            BCMI_DINO_OPRXLOS_6_CTLr_OPRXLOS_6_OUT_FRCVALf_SET(pad_ctrl_oprxlos_6_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_6_CTLr(pa, pad_ctrl_oprxlos_6_ctrl));
        break;
        case 33:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_7_CTLr(pa, &pad_ctrl_oprxlos_7_ctrl));
            BCMI_DINO_OPRXLOS_7_CTLr_OPRXLOS_7_IBOFf_SET(pad_ctrl_oprxlos_7_ctrl, 1);
            BCMI_DINO_OPRXLOS_7_CTLr_OPRXLOS_7_OUT_FRCVALf_SET(pad_ctrl_oprxlos_7_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_7_CTLr(pa, pad_ctrl_oprxlos_7_ctrl));
        break;
        case 34:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_8_CTLr(pa, &pad_ctrl_oprxlos_8_ctrl));
            BCMI_DINO_OPRXLOS_8_CTLr_OPRXLOS_8_IBOFf_SET(pad_ctrl_oprxlos_8_ctrl, 1);
            BCMI_DINO_OPRXLOS_8_CTLr_OPRXLOS_8_OUT_FRCVALf_SET(pad_ctrl_oprxlos_8_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_8_CTLr(pa, pad_ctrl_oprxlos_8_ctrl));
        break;
        case 35:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_9_CTLr(pa, &pad_ctrl_oprxlos_9_ctrl));
            BCMI_DINO_OPRXLOS_9_CTLr_OPRXLOS_9_IBOFf_SET(pad_ctrl_oprxlos_9_ctrl, 1);
            BCMI_DINO_OPRXLOS_9_CTLr_OPRXLOS_9_OUT_FRCVALf_SET(pad_ctrl_oprxlos_9_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_9_CTLr(pa, pad_ctrl_oprxlos_9_ctrl));
        break;
        case 36:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_10_CTLr(pa, &pad_ctrl_oprxlos_10_ctrl));
            BCMI_DINO_OPRXLOS_10_CTLr_OPRXLOS_10_IBOFf_SET(pad_ctrl_oprxlos_10_ctrl, 1);
            BCMI_DINO_OPRXLOS_10_CTLr_OPRXLOS_10_OUT_FRCVALf_SET(pad_ctrl_oprxlos_10_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_10_CTLr(pa, pad_ctrl_oprxlos_10_ctrl));
        break;
        case 37:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_11_CTLr(pa, &pad_ctrl_oprxlos_11_ctrl));
            BCMI_DINO_OPRXLOS_11_CTLr_OPRXLOS_11_IBOFf_SET(pad_ctrl_oprxlos_11_ctrl, 1);
            BCMI_DINO_OPRXLOS_11_CTLr_OPRXLOS_11_OUT_FRCVALf_SET(pad_ctrl_oprxlos_11_ctrl, value ? 1 : 0);
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_WRITE_OPRXLOS_11_CTLr(pa, pad_ctrl_oprxlos_11_ctrl));
        break;
        default:
          return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _dino_phy_gpio_pin_value_get(const phymod_access_t* pa, int pin_number, int* value)
{
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    BCMI_DINO_GPIO_0_STSr_t pad_sts_gpio_0;
    BCMI_DINO_GPIO_1_STSr_t pad_sts_gpio_1;
    BCMI_DINO_GPIO_2_STSr_t pad_sts_gpio_2;
    BCMI_DINO_GPIO_3_STSr_t pad_sts_gpio_3;
    BCMI_DINO_GPIO_4_STSr_t pad_sts_gpio_4;
    BCMI_DINO_GPIO_5_STSr_t pad_sts_gpio_5;
    BCMI_DINO_GPIO_6_STSr_t pad_sts_gpio_6;
    BCMI_DINO_GPIO_7_STSr_t pad_sts_gpio_7;
    BCMI_DINO_GPIO_8_STSr_t pad_sts_gpio_8;
    BCMI_DINO_GPIO_9_STSr_t pad_sts_gpio_9;
    BCMI_DINO_GPIO_10_STSr_t pad_sts_gpio_10;
    BCMI_DINO_GPIO_11_STSr_t pad_sts_gpio_11;
    BCMI_DINO_GPIO_12_STSr_t pad_sts_gpio_12;
    BCMI_DINO_GPIO_13_STSr_t pad_sts_gpio_13;
    BCMI_DINO_GPIO_14_STSr_t pad_sts_gpio_14;
    BCMI_DINO_GPIO_15_STSr_t pad_sts_gpio_15;
    BCMI_DINO_GPIO_16_STSr_t pad_sts_gpio_16;
    BCMI_DINO_GPIO_17_STSr_t pad_sts_gpio_17;
    BCMI_DINO_OPTXENB1_A_STSr_t pad_sts_optxenb1_a;
    BCMI_DINO_OPTXENB1_B_STSr_t pad_sts_optxenb1_b;
    BCMI_DINO_OPTXENB1_C_STSr_t pad_sts_optxenb1_c;
    BCMI_DINO_MOD_ABS_3_STSr_t pad_sts_mod_abs_3;
    BCMI_DINO_MOD_ABS_4_STSr_t pad_sts_mod_abs_4;
    BCMI_DINO_MOD_ABS_5_STSr_t pad_sts_mod_abs_5;
    BCMI_DINO_MOD_ABS_6_STSr_t pad_sts_mod_abs_6;
    BCMI_DINO_MOD_ABS_7_STSr_t pad_sts_mod_abs_7;
    BCMI_DINO_MOD_ABS_8_STSr_t pad_sts_mod_abs_8;
    BCMI_DINO_MOD_ABS_9_STSr_t pad_sts_mod_abs_9;
    BCMI_DINO_MOD_ABS_10_STSr_t pad_sts_mod_abs_10;
    BCMI_DINO_MOD_ABS_11_STSr_t pad_sts_mod_abs_11;
    BCMI_DINO_OPRXLOS_4_STSr_t pad_sts_oprxlos_4;
    BCMI_DINO_OPRXLOS_5_STSr_t pad_sts_oprxlos_5;
    BCMI_DINO_OPRXLOS_6_STSr_t pad_sts_oprxlos_6;
    BCMI_DINO_OPRXLOS_7_STSr_t pad_sts_oprxlos_7;
    BCMI_DINO_OPRXLOS_8_STSr_t pad_sts_oprxlos_8;
    BCMI_DINO_OPRXLOS_9_STSr_t pad_sts_oprxlos_9;
    BCMI_DINO_OPRXLOS_10_STSr_t pad_sts_oprxlos_10;
    BCMI_DINO_OPRXLOS_11_STSr_t pad_sts_oprxlos_11;

    PHYMOD_MEMSET(&pad_sts_gpio_0, 0, sizeof(BCMI_DINO_GPIO_0_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_1, 0, sizeof(BCMI_DINO_GPIO_1_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_2, 0, sizeof(BCMI_DINO_GPIO_2_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_3, 0, sizeof(BCMI_DINO_GPIO_3_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_4, 0, sizeof(BCMI_DINO_GPIO_4_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_5, 0, sizeof(BCMI_DINO_GPIO_5_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_6, 0, sizeof(BCMI_DINO_GPIO_6_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_7, 0, sizeof(BCMI_DINO_GPIO_7_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_8, 0, sizeof(BCMI_DINO_GPIO_8_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_9, 0, sizeof(BCMI_DINO_GPIO_9_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_10, 0, sizeof(BCMI_DINO_GPIO_10_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_11, 0, sizeof(BCMI_DINO_GPIO_11_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_12, 0, sizeof(BCMI_DINO_GPIO_12_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_13, 0, sizeof(BCMI_DINO_GPIO_13_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_14, 0, sizeof(BCMI_DINO_GPIO_14_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_15, 0, sizeof(BCMI_DINO_GPIO_15_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_16, 0, sizeof(BCMI_DINO_GPIO_16_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_17, 0, sizeof(BCMI_DINO_GPIO_17_STSr_t));
    PHYMOD_MEMSET(&pad_sts_optxenb1_a, 0, sizeof(BCMI_DINO_OPTXENB1_A_STSr_t));
    PHYMOD_MEMSET(&pad_sts_optxenb1_b, 0, sizeof(BCMI_DINO_OPTXENB1_B_STSr_t));
    PHYMOD_MEMSET(&pad_sts_optxenb1_c, 0, sizeof(BCMI_DINO_OPTXENB1_C_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_3, 0, sizeof(BCMI_DINO_MOD_ABS_3_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_4, 0, sizeof(BCMI_DINO_MOD_ABS_4_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_5, 0, sizeof(BCMI_DINO_MOD_ABS_5_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_6, 0, sizeof(BCMI_DINO_MOD_ABS_6_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_7, 0, sizeof(BCMI_DINO_MOD_ABS_7_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_8, 0, sizeof(BCMI_DINO_MOD_ABS_8_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_9, 0, sizeof(BCMI_DINO_MOD_ABS_9_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_10, 0, sizeof(BCMI_DINO_MOD_ABS_10_STSr_t));
    PHYMOD_MEMSET(&pad_sts_mod_abs_11, 0, sizeof(BCMI_DINO_MOD_ABS_11_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_4, 0, sizeof(BCMI_DINO_OPRXLOS_4_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_5, 0, sizeof(BCMI_DINO_OPRXLOS_5_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_6, 0, sizeof(BCMI_DINO_OPRXLOS_6_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_7, 0, sizeof(BCMI_DINO_OPRXLOS_7_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_8, 0, sizeof(BCMI_DINO_OPRXLOS_8_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_9, 0, sizeof(BCMI_DINO_OPRXLOS_9_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_10, 0, sizeof(BCMI_DINO_OPRXLOS_10_STSr_t));
    PHYMOD_MEMSET(&pad_sts_oprxlos_11, 0, sizeof(BCMI_DINO_OPRXLOS_11_STSr_t));

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev));
    if (chip_id == DINO_CHIP_ID_82793 && pin_number > 11) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 11)")));
    }
    
    if (chip_id == DINO_CHIP_ID_82332 && pin_number > 37) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 37)")));
    }

    switch (pin_number) {
        case 0:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_0_STSr(pa, &pad_sts_gpio_0));
            *value = BCMI_DINO_GPIO_0_STSr_GPIO_0_DINf_GET(pad_sts_gpio_0);
        break;
        case 1:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_1_STSr(pa, &pad_sts_gpio_1));
            *value = BCMI_DINO_GPIO_1_STSr_GPIO_1_DINf_GET(pad_sts_gpio_1);
        break;
        case 2:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_2_STSr(pa, &pad_sts_gpio_2));
            *value = BCMI_DINO_GPIO_2_STSr_GPIO_2_DINf_GET(pad_sts_gpio_2);
        break;
        case 3:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_3_STSr(pa, &pad_sts_gpio_3));
            *value = BCMI_DINO_GPIO_3_STSr_GPIO_3_DINf_GET(pad_sts_gpio_3);
        break;
        case 4:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_4_STSr(pa, &pad_sts_gpio_4));
            *value = BCMI_DINO_GPIO_4_STSr_GPIO_4_DINf_GET(pad_sts_gpio_4);
        break;
        case 5:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_5_STSr(pa, &pad_sts_gpio_5));
            *value = BCMI_DINO_GPIO_5_STSr_GPIO_5_DINf_GET(pad_sts_gpio_5);
        break;
        case 6:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_6_STSr(pa, &pad_sts_gpio_6));
            *value = BCMI_DINO_GPIO_6_STSr_GPIO_6_DINf_GET(pad_sts_gpio_6);
        break;
        case 7:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_7_STSr(pa, &pad_sts_gpio_7));
            *value = BCMI_DINO_GPIO_7_STSr_GPIO_7_DINf_GET(pad_sts_gpio_7);
        break;
        case 8:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_8_STSr(pa, &pad_sts_gpio_8));
            *value = BCMI_DINO_GPIO_8_STSr_GPIO_8_DINf_GET(pad_sts_gpio_8);
        break;
        case 9:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_9_STSr(pa, &pad_sts_gpio_9));
            *value = BCMI_DINO_GPIO_9_STSr_GPIO_9_DINf_GET(pad_sts_gpio_9);
        break;
        case 10:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_10_STSr(pa, &pad_sts_gpio_10));
            *value = BCMI_DINO_GPIO_10_STSr_GPIO_10_DINf_GET(pad_sts_gpio_10);
        break;
        case 11:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_11_STSr(pa, &pad_sts_gpio_11));
            *value = BCMI_DINO_GPIO_11_STSr_GPIO_11_DINf_GET(pad_sts_gpio_11);
        break;
        case 12:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_12_STSr(pa, &pad_sts_gpio_12));
            *value = BCMI_DINO_GPIO_12_STSr_GPIO_12_DINf_GET(pad_sts_gpio_12);
        break;
        case 13:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_13_STSr(pa, &pad_sts_gpio_13));
            *value = BCMI_DINO_GPIO_13_STSr_GPIO_13_DINf_GET(pad_sts_gpio_13);
        break;
        case 14:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_14_STSr(pa, &pad_sts_gpio_14));
            *value = BCMI_DINO_GPIO_14_STSr_GPIO_14_DINf_GET(pad_sts_gpio_14);
        break;
        case 15:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_15_STSr(pa, &pad_sts_gpio_15));
            *value = BCMI_DINO_GPIO_15_STSr_GPIO_15_DINf_GET(pad_sts_gpio_15);
        break;
        case 16:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_16_STSr(pa, &pad_sts_gpio_16));
            *value = BCMI_DINO_GPIO_16_STSr_GPIO_16_DINf_GET(pad_sts_gpio_16);
        break;
        case 17:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_GPIO_17_STSr(pa, &pad_sts_gpio_17));
            *value = BCMI_DINO_GPIO_17_STSr_GPIO_17_DINf_GET(pad_sts_gpio_17);
        break;
        case 18:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_A_STSr(pa, &pad_sts_optxenb1_a));
            *value = BCMI_DINO_OPTXENB1_A_STSr_OPTXENB1_A_DINf_GET(pad_sts_optxenb1_a);
        break;
        case 19:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_B_STSr(pa, &pad_sts_optxenb1_b));
            *value = BCMI_DINO_OPTXENB1_B_STSr_OPTXENB1_B_DINf_GET(pad_sts_optxenb1_b);
        break;
        case 20:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPTXENB1_C_STSr(pa, &pad_sts_optxenb1_c));
            *value = BCMI_DINO_OPTXENB1_C_STSr_OPTXENB1_C_DINf_GET(pad_sts_optxenb1_c);
        break;
        case 21:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_3_STSr(pa, &pad_sts_mod_abs_3));
            *value = BCMI_DINO_MOD_ABS_3_STSr_MOD_ABS_3_DINf_GET(pad_sts_mod_abs_3);
        break;
        case 22:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_4_STSr(pa, &pad_sts_mod_abs_4));
            *value = BCMI_DINO_MOD_ABS_4_STSr_MOD_ABS_4_DINf_GET(pad_sts_mod_abs_4);
        break;
        case 23:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_5_STSr(pa, &pad_sts_mod_abs_5));
            *value = BCMI_DINO_MOD_ABS_5_STSr_MOD_ABS_5_DINf_GET(pad_sts_mod_abs_5);
        break;
        case 24:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_6_STSr(pa, &pad_sts_mod_abs_6));
            *value = BCMI_DINO_MOD_ABS_6_STSr_MOD_ABS_6_DINf_GET(pad_sts_mod_abs_6);
        break;
        case 25:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_7_STSr(pa, &pad_sts_mod_abs_7));
            *value = BCMI_DINO_MOD_ABS_7_STSr_MOD_ABS_7_DINf_GET(pad_sts_mod_abs_7);
        break;
        case 26:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_8_STSr(pa, &pad_sts_mod_abs_8));
            *value = BCMI_DINO_MOD_ABS_8_STSr_MOD_ABS_8_DINf_GET(pad_sts_mod_abs_8);
        break;
        case 27:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_9_STSr(pa, &pad_sts_mod_abs_9));
            *value = BCMI_DINO_MOD_ABS_9_STSr_MOD_ABS_9_DINf_GET(pad_sts_mod_abs_9);
        break;
        case 28:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_10_STSr(pa, &pad_sts_mod_abs_10));
            *value = BCMI_DINO_MOD_ABS_10_STSr_MOD_ABS_10_DINf_GET(pad_sts_mod_abs_10);
        break;
        case 29:    
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_MOD_ABS_11_STSr(pa, &pad_sts_mod_abs_11));
            *value = BCMI_DINO_MOD_ABS_11_STSr_MOD_ABS_11_DINf_GET(pad_sts_mod_abs_11);
        break;
        case 30:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_4_STSr(pa, &pad_sts_oprxlos_4));
            *value = BCMI_DINO_OPRXLOS_4_STSr_OPRXLOS_4_DINf_GET(pad_sts_oprxlos_4);    
        break;
        case 31:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_5_STSr(pa, &pad_sts_oprxlos_5));
            *value = BCMI_DINO_OPRXLOS_5_STSr_OPRXLOS_5_DINf_GET(pad_sts_oprxlos_5);    
        break;
        case 32:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_6_STSr(pa, &pad_sts_oprxlos_6));
            *value = BCMI_DINO_OPRXLOS_6_STSr_OPRXLOS_6_DINf_GET(pad_sts_oprxlos_6);    
        break;
        case 33:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_7_STSr(pa, &pad_sts_oprxlos_7));
            *value = BCMI_DINO_OPRXLOS_7_STSr_OPRXLOS_7_DINf_GET(pad_sts_oprxlos_7);    
        break;
        case 34:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_8_STSr(pa, &pad_sts_oprxlos_8));
            *value = BCMI_DINO_OPRXLOS_8_STSr_OPRXLOS_8_DINf_GET(pad_sts_oprxlos_8);    
        break;
        case 35:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_9_STSr(pa, &pad_sts_oprxlos_9));
            *value = BCMI_DINO_OPRXLOS_9_STSr_OPRXLOS_9_DINf_GET(pad_sts_oprxlos_9);    
        break;
        case 36:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_10_STSr(pa, &pad_sts_oprxlos_10));
            *value = BCMI_DINO_OPRXLOS_10_STSr_OPRXLOS_10_DINf_GET(pad_sts_oprxlos_10);    
        break;
        case 37:
            PHYMOD_IF_ERR_RETURN(
                BCMI_DINO_READ_OPRXLOS_11_STSr(pa, &pad_sts_oprxlos_11));
            *value = BCMI_DINO_OPRXLOS_11_STSr_OPRXLOS_11_DINf_GET(pad_sts_oprxlos_11);    
        break;
        default:
          return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _dino_cfg_fw_ull_dp(const phymod_core_access_t* core, uint16_t op_datapath) {
    uint16_t timing_mode = 0;
    uint16_t lane_idx = 0;
    uint16_t lane_mask   = 0;
    uint16_t gpreg_data = 0;
    int num_lanes = 0;
    uint32_t chip_id = 0;
    uint32_t rev_id = 0;
    const phymod_access_t *pa = &core->access;

    /* lane_mask = PHYMOD_ACC_LANE_MASK(pa); */
    lane_mask = 0xfff;

    PHYMOD_IF_ERR_RETURN(dino_get_chipid(pa, &chip_id, &rev_id));
    DINO_GET_NUM_LANES(chip_id, num_lanes);

    if (op_datapath == phymodDatapathNormal) {
        timing_mode = DINO_TX_REPEATER;
    } else if (op_datapath == phymodDatapathUll) {
        timing_mode = DINO_TX_REPEATER_ULL; 
    } else {
        return PHYMOD_E_PARAM;
    }

    /* Configure ULL datapath mode on the line side and system side*/
    for (lane_idx = 0; lane_idx < num_lanes; lane_idx++) {
        if ((lane_mask >> lane_idx) & 0x1) {
            READ_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane_idx), gpreg_data);
            gpreg_data &= ~(DINO_LINE_TX_TIMING_MODE_MASK);
            gpreg_data |= (timing_mode << DINO_LINE_TX_TIMING_MODE_SHIFT);
            gpreg_data &= ~(DINO_SYS_TX_TIMING_MODE_MASK);
            gpreg_data |= (timing_mode << DINO_SYS_TX_TIMING_MODE_SHIFT);
            WRITE_DINO_PMA_PMD_REG(pa, (DINO_GPREG_0_ADR + lane_idx), gpreg_data);
        }
    }
    /* Notify the firmware about the configurations */
    PHYMOD_IF_ERR_RETURN(_dino_fw_enable(pa));

    return PHYMOD_E_NONE;
}
int _dino_phy_multi_get(const phymod_access_t* pa, phymod_multi_data_t* multi_data)
{
    PHYMOD_IF_ERR_RETURN
        (_dino_phy_i2c_read(pa, multi_data->dev_addr, multi_data->offset, multi_data->max_size, multi_data->data));
    if (multi_data->actual_size) {
        (*(multi_data->actual_size)) = multi_data->max_size;
    }

    return PHYMOD_E_NONE;
}

