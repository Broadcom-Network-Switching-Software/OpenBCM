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
#include "sesto_address_defines.h"
#include "sesto_reg_structs.h"
#include "sesto_cfg_seq.h"
#include "sesto_serdes/common/srds_api_enum.h"
#include "sesto_serdes/merlin_sesto_src/merlin_sesto_functions.h"
#include "sesto_serdes/falcon_furia_sesto_src/falcon_furia_sesto_functions.h"
#include <phymod/chip/sesto.h>

extern unsigned char sesto_falcon_ucode[];
extern unsigned char sesto_merlin_ucode[];
extern unsigned int sesto_falcon_ucode_len;
extern unsigned int sesto_merlin_ucode_len;

int sesto_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev) 
{
    SES_CTRL_CHIP_ID_TYPE_T chipid_lsb;
    SES_CTRL_CHIP_REVISION_TYPE_T chipid_msb_rev;
    int rv = PHYMOD_E_NONE;

    READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_CHIP_ID_ADR, chipid_lsb.data);
    READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_CHIP_REVISION_ADR, chipid_msb_rev.data);

    *chipid = (chipid_lsb.data) | (chipid_msb_rev.fields.chip_id_19_16 << 16);
    *rev = chipid_msb_rev.fields.chip_rev;

    PHYMOD_DEBUG_VERBOSE(("CHIP ID: %x REv:%x\n", *chipid, *rev));

ERR:
    return rv;
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
int _sesto_wait_mst_msgout(const phymod_access_t *pa,
                            SESTO_MSGOUT_E exp_message,
                            int poll_time)
{
    int retry_count = SESTO_FW_DLOAD_RETRY_CNT;
    SESTO_MSGOUT_E msgout = 0;
    SES_GEN_CNTRLS_MSGOUT_TYPE_T msgout_t;
    int rv = PHYMOD_E_NONE;
    
    do {
        /* Read general control msg out  Register */
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MSGOUT_ADR, msgout_t.data);
        msgout = (SESTO_MSGOUT_E) msgout_t.fields.msgout_val;
        /* wait before reading again */
        if (poll_time != 0) {
            /* sleep for specified useconds*/
            PHYMOD_USLEEP(poll_time);
        }
        retry_count --;
    } while ((msgout != exp_message) &&  retry_count);
    
    if (!retry_count) {
        if (exp_message == MSGOUT_PRGRM_DONE) { 
            PHYMOD_DEBUG_VERBOSE(("MSG OUT:%x \n",msgout));
            SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL, (_PHYMOD_MSG("Fusing Firmware failed")));
        } else {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
        }
    }

ERR:
    return rv;
}

int _sesto_fw_enable(const phymod_access_t *pa, uint16_t en_dis) 
{
    int rv = PHYMOD_E_NONE;
		
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, en_dis);

ERR:
    return rv;
}
int sesto_micro_download(const phymod_access_t *pa, unsigned char *ucode,
                         uint32_t len, uint16_t master, uint16_t *chk_sum) 
{
    SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T spi_code_load;
    SES_MICRO_BOOT_BOOT_POR_TYPE_T  boot_por;
    SES_GEN_CNTRLS_MSGIN_TYPE_T   msg_in;
    uint16_t num_bytes = 0, j = 0;
    uint16_t num_words = 0;
    uint16_t check_sum = 0, cnt = SESTO_FW_DLOAD_RETRY_CNT, data = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T));

	/* Send boot address */
    SESTO_IF_ERR_RETURN(
        _sesto_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    msg_in.data = 0;
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MSGIN_ADR, 
                              msg_in.data);
    SESTO_IF_ERR_RETURN(
        _sesto_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    /* when code_broadcast_en is enable and slave_code_download_en is enale
     * we need do msg_in and msg_our as MSGOUT_NEXT */
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_ADR, spi_code_load.data);
    if (spi_code_load.fields.code_broadcast_en && (!master)) {
        msg_in.data = 0;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MSGIN_ADR,
                              msg_in.data);

    SESTO_IF_ERR_RETURN(
        _sesto_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
    }

    /* Calculate the number of words */
    num_words = (len) / 2;

    /* Update message in value field with word length */
    msg_in.fields.msgin_val = num_words;
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MSGIN_ADR, 
                                 msg_in.data);
    num_bytes = len;
    for (j = 0; j < num_bytes; j += 2) {
/*
        SESTO_IF_ERR_RETURN
            (_sesto_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
*/
        data = (ucode[j + 1] << 8) | ucode[j];
        check_sum ^= ucode[j] ^ ucode[j + 1];
    
        /* Send next word */
        msg_in.fields.msgin_val = data;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MSGIN_ADR, 
                                 msg_in.data);
    }
    do {
	    /* check download_done flags*/
        READ_SESTO_PMA_PMD_REG(pa, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
        if (master) {
            if (boot_por.fields.mst_dwld_done == 1) {
                PHYMOD_DEBUG_VERBOSE(("Master Dload Done\n"));
                break;
            }
        } else {
            if (boot_por.fields.slv_dwld_done == 1) {
                PHYMOD_DEBUG_VERBOSE(("Slave Dload Done\n"));
                break;
            }
        }
    } while (cnt --);

	/* check checksum */
    check_sum = (check_sum == 0) ? 0x600D : check_sum;
    if (master)  {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_MST_RUNNING_BYTE_CNT, mst_running_byte_cnt_val, data);
        if (data != num_bytes) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,(_PHYMOD_MSG("Master Dload fail:")));
       } 
        PHYMOD_DEBUG_VERBOSE(("Master Byte Cnt:%d\n", data));
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_MST_RUNNING_CHKSUM, mst_running_chksum_val, data);
        if (check_sum != data) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                (_PHYMOD_MSG("Master Dload chksum Fail")));
        } 
        PHYMOD_DEBUG_VERBOSE(("Master Chk sum:0x%x\n", data));
        *chk_sum = data;
    } else {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_SLV_RUNNING_BYTE_CNT, slv_running_byte_cnt_val, data);
        if (data != num_bytes) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("SLave Dload fail:")));
        } 
        PHYMOD_DEBUG_VERBOSE(("Slave Byte Cnt:%d\n", data));
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_SLV_RUNNING_CHKSUM, slv_running_chksum_val, data);
        if (check_sum != data) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("Slave Dload chksum fail ")));
        } 
        PHYMOD_DEBUG_VERBOSE(("Slave cksum Cnt:0x%x\n", data));
        *chk_sum = data;
    }

ERR:
    return rv;
}

int _sesto_firmware_download(const phymod_access_t *pm_acc ,
                            unsigned char *sesto_master_ucode, unsigned int master_len,
                            unsigned char *sesto_slave_ucode, unsigned int slave_len)
{
    uint16_t data1 = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T spi_code_load; 
    SES_GEN_CNTRLS_GEN_CONTROL2_TYPE_T gen_ctrl2;
    SES_MICRO_BOOT_BOOT_POR_TYPE_T   boot_por;
    uint16_t mst_check_sum = 0, slv_check_sum = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T));
    PHYMOD_MEMSET(&gen_ctrl2, 0 , sizeof(SES_GEN_CNTRLS_GEN_CONTROL2_TYPE_T));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(SES_MICRO_BOOT_BOOT_POR_TYPE_T));

    /* Skip the mst_dwld_down, slv_dwld_done and chip reset check incase broadcast is enabled */
    /* Skip the firmware download
     *  mst_download, slv_dwld_done bits are set
     */
    SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_MICRO_SYNC_7, brdcst_en, data1);
    if (!data1) {
        READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
        if ((boot_por.fields.mst_dwld_done) &&
            (boot_por.fields.slv_dwld_done)) {
            return SESTO_FW_ALREADY_DOWNLOADED;
        }

        /* Do chip hard reset and Resetting the Cores */
        SESTO_IF_ERR_RETURN(
            _sesto_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));
    }
    /* Put Master under Reset */
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_rstb, 0);
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_ucp_rstb, 0);

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its 
     * completion before starting the MDIO Download */
    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
        retry_cnt--;
    } while((data1 != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    /* Sesto always supports serial mode if Dload
     * Master followed by slave 
     * Configure master_code_download_en, slave_code_download_en
     * and broadcast enable */
    spi_code_load.fields.code_broadcast_en = 0;
    spi_code_load.fields.master_code_download_en = 1;
    spi_code_load.fields.slave_code_download_en = 1;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_ADR, 
                              spi_code_load.data);

    READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_ADR, data1);
    spi_code_load.data = data1;
    if (spi_code_load.fields.code_broadcast_en != 0 && spi_code_load.fields.master_code_download_en != 1
            && spi_code_load.fields.slave_code_download_en != 1) {
        return PHYMOD_E_INTERNAL;
    }
    /* Select SPI Speed*/
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL3, 
                           ucspi_slow, 1); 

    /* Check the not downloading branch of the flow digram
     * before writing to serboot.
     * wait_mst_msgout(chip_cfg, die, MSGOUT_NOT_DWNLD, 1, 0);*/
    
    /* Read / Write boot_por
     *  write to mst_dwld_done and slv_dwld_done.
     *  write to spi_port_used
     *  write to serboot */
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
    
    /* Force Master New Download */
    boot_por.fields.mst_dwld_done = 0;
    /* Force Slave New Download */
    boot_por.fields.slv_dwld_done = 0;
    boot_por.fields.serboot = 1;
    boot_por.fields.spi_port_used = 0;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, 
                          boot_por.data);

    /* RELEASE Master under Reset */
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_ucp_rstb, 1);
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_rstb, 1);

        /* MDI/I2C Download */
        /* Waiting for serboot_busy */
    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
            retry_cnt--;
        } while ((data1 == 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }
    /* Start download sequence */
    /* master boot*/
    SESTO_IF_ERR_RETURN(
       sesto_micro_download(pm_acc, sesto_master_ucode,
                             master_len, 1, &mst_check_sum));

    SESTO_IF_ERR_RETURN(
       sesto_micro_download(pm_acc, sesto_slave_ucode,
                             slave_len, 0, &slv_check_sum));
    SESTO_IF_ERR_RETURN(
      _sesto_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));
        /*uint16_t checksum = 0;
        SESTO_IF_ERR_RETURN(
           _sesto_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_MST_RUNNING_CHKSUM, mst_running_chksum_val, checksum);
        PHYMOD_DEBUG_VERBOSE(("Master CheckSum:%x\n",checksum));
        
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_SLV_RUNNING_CHKSUM, slv_running_chksum_val, checksum);
        PHYMOD_DEBUG_VERBOSE(("Slave CheckSum:%x\n",checksum));
    }*/
    /* Check serboot_busy and serboot_done_once*/
    SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
    if (data1 != 0) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT BUSY HAS UNEXPECTED VALUE\n"));
    }
    SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_done_once, data1);
    if (data1 != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT DONE ONCE HAS UNEXPECTED VALUE\n"));
    }

    /* check download_done flags again */
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
    if (boot_por.fields.mst_dwld_done != 1 || boot_por.fields.slv_dwld_done != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:Download Done got cleared\n"));
    }

    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1);
        retry_cnt --;
    } while ((data1 != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Fireware download failed, micro controller is busy..")));
    }
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR,
                                     data1);
    PHYMOD_DEBUG_VERBOSE(("FW Version:0x%x\n", data1));

ERR:
    return rv;
}
int _sesto_eeprom_firmware_download(const phymod_access_t *pm_acc ,
                            unsigned char *sesto_ucode, unsigned int ucode_len, uint16_t master)
{
    uint16_t data1 = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    uint16_t eeprom_num_of_pages = 0;
    SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T spi_code_load;
    SES_GEN_CNTRLS_GEN_CONTROL2_TYPE_T gen_ctrl2;
    SES_MICRO_BOOT_BOOT_POR_TYPE_T   boot_por;
    uint16_t check_sum = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_TYPE_T));
    PHYMOD_MEMSET(&gen_ctrl2, 0 , sizeof(SES_GEN_CNTRLS_GEN_CONTROL2_TYPE_T));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(SES_MICRO_BOOT_BOOT_POR_TYPE_T));

    /* Do chip hard reset and Resetting the Cores */
    SESTO_IF_ERR_RETURN(
        _sesto_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));
    /* Put Master under Reset */
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_rstb, 0);
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_ucp_rstb, 0);

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its
     * completion before starting the MDIO Download */

    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
        retry_cnt--;
    } while((data1 != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
    }

    /* Sesto always supports serial mode if Dload
     * Master followed by slave
     * Configure master_code_download_en, slave_code_download_en
     * and broadcast enable */
    spi_code_load.fields.code_broadcast_en = 1;
    spi_code_load.fields.master_code_download_en = 1;
    spi_code_load.fields.slave_code_download_en = !master;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_ADR, spi_code_load.data);

    READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_SPI_CODE_LOAD_EN_ADR, spi_code_load.data);
    if (spi_code_load.fields.code_broadcast_en != 1) {
        return PHYMOD_E_INTERNAL;
    }

    /* Configure GPREG for programming eeprom */
    eeprom_num_of_pages = (uint16_t)(ucode_len/SESTO_M0_EEPROM_PAGE_SIZE);
    if (eeprom_num_of_pages > 0x1FF) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:No of EEPROM pages more than Code ROM, Fusing does not support")));
    }
    eeprom_num_of_pages = 0x1FF;

    /* Start Page of EEPROM*/
    /* For master start page is "0" and slave start page is "0x200" in eeprom */
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GPREG14_ADR, (master ? 0 : 0x200));
    /* No of eeprom pages */
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GPREG15_ADR, eeprom_num_of_pages);
    /* Select SPI Speed*/
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL3, ucspi_slow, 1);
    /* Reset spi */
    /* Assert Reset */
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2_ADR, gen_ctrl2.data);
    gen_ctrl2.fields.spi_rstb = 0;
    gen_ctrl2.fields.spi2x_rstb = 0;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2_ADR, gen_ctrl2.data);

    /* Deassert Reset */
    gen_ctrl2.fields.spi_rstb = 1;
    gen_ctrl2.fields.spi2x_rstb = 1;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2_ADR, gen_ctrl2.data);

    /* Check the not downloading branch of the flow digram
     * before writing to serboot.
     * wait_mst_msgout(chip_cfg, die, MSGOUT_NOT_DWNLD, 1, 0);*/
    /* Read / Write boot_por
     *  write to mst_dwld_done and slv_dwld_done.
     *  write to spi_port_used
     *  write to serboot */
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);

    /* Force Master New Download */
    boot_por.fields.mst_dwld_done = 0;
    /* Force Slave New Download */
    boot_por.fields.slv_dwld_done = 0;
    boot_por.fields.serboot = 1;
    boot_por.fields.spi_port_used = 0;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR,
                          boot_por.data);

    /* RELEASE Master under Reset */

    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_ucp_rstb, 1);
    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2, m0_mstr_rstb, 1);

    /* MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
            retry_cnt--;
        } while ((data1 == 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }
    /* Start download sequence */
    /* master boot*/
    SESTO_IF_ERR_RETURN(
            sesto_micro_download(pm_acc, sesto_ucode,
                                 ucode_len, master, &check_sum));
    SESTO_IF_ERR_RETURN(
            _sesto_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));

    /* Check serboot_busy and serboot_done_once*/
    SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_busy, data1);
    if (data1 != 0) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT BUSY HAS UNEXPECTED VALUE\n"));
    }
    SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_BOOT, serboot_done_once, data1);
    if (data1 != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT DONE ONCE HAS UNEXPECTED VALUE\n"));
    }

    /* check download_done flags again */
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
    if (boot_por.fields.mst_dwld_done != 1 || boot_por.fields.slv_dwld_done != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:Download Done got cleared\n"));
    }
    /* Wait for Program EEPROM*/
    SESTO_IF_ERR_RETURN(
        _sesto_wait_mst_msgout(pm_acc, MSGOUT_PRGRM_DONE,0));
        /* Assert SPI Reset*/
        gen_ctrl2.fields.spi_rstb = 0;
        gen_ctrl2.fields.spi2x_rstb = 0;
    WRITE_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_GEN_CONTROL2_ADR, gen_ctrl2.data);

    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1);
        retry_cnt --;
    } while ((data1 != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Fireware download failed, micro controller is busy..")));
    }
    READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR,
                                     data1);
    PHYMOD_DEBUG_VERBOSE(("FW Version:0x%x\n", data1));

ERR:
    return rv;
}

int _sesto_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config)
{
    int ret_val = 0; 
    uint16_t ip = 0;
    uint16_t lane = 0, data = 0;
    uint16_t max_lane = 0;
    uint32_t rev = 0, chip_id = 0;
    phymod_access_t pa;
    int rv = PHYMOD_E_NONE;
    uint16_t retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    const phymod_access_t *pm_acc = &core->access;
    DP_FMON0_MAIN_CONTROL_TYPE_T fmon_main_ctrl;
    DP_MMON0_MAIN_CONTROL_TYPE_T mmon_main_ctrl;
    SES_MICRO_BOOT_BOOT_POR_TYPE_T boot_por;
    uint32_t new_fm_version = 0;
    phymod_core_firmware_info_t fw_info;

    PHYMOD_MEMSET(&boot_por, 0 , sizeof(SES_MICRO_BOOT_BOOT_POR_TYPE_T));
    PHYMOD_MEMSET(&fmon_main_ctrl, 0 , sizeof(DP_FMON0_MAIN_CONTROL_TYPE_T));
    PHYMOD_MEMSET(&mmon_main_ctrl, 0 , sizeof(DP_MMON0_MAIN_CONTROL_TYPE_T));
    PHYMOD_MEMSET(&fw_info, 0, sizeof(phymod_core_firmware_info_t));

    SESTO_IF_ERR_RETURN(
        sesto_core_firmware_info_get(core, &fw_info));
    new_fm_version = sesto_falcon_ucode[sesto_falcon_ucode_len - 3];

    switch(init_config->firmware_load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DEBUG_VERBOSE((
               "Starting Firmware download through MDIO, it takes few seconds...\n"));

            /* In case of FW Download is only for second die i.e for odd address of the die
             * To get the micro out of reset on even die(previous die); for MDIO only
             * Applicable for only duel die chips;
             * SESTO_CHIP_ID_82790 is 82790 is single die ingnore the micro out of reset
             */
            SESTO_IF_ERR_RETURN(sesto_get_chipid(pm_acc, &chip_id, &rev));
            if (((pm_acc->addr & 0x1) == 0x1) && (chip_id != SESTO_CHIP_ID_82790)) {
                PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
                /* modify the mdio address to be even */
                pa.addr &= ~(0x1);
                /* Set the 11th and 9th bit of even die to get the micro out of reset */
                SESTO_CHIP_FIELD_WRITE(&pa, SES_PAD_CTRL_EXT_UC_RSTB_OUT_CONTROL, ext_uc_rstb_out_out_frcval, 1);
                SESTO_CHIP_FIELD_WRITE(&pa, SES_PAD_CTRL_EXT_UC_RSTB_OUT_CONTROL, ext_uc_rstb_out_ibof, 1);
            }

            if(PHYMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
                /*Dowdload firmware unicast when rev is 0xA0 that does not support broadcast*/
                if(rev == 0xA0){
                /* FW force method is force or Auto with different FW version then do chip reset */
                    if (PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_GET(init_config) ||
                       (PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config) && (new_fm_version != (fw_info.fw_version & 0xFF)))) {
                        /* Do chip hard reset and Resetting the Cores */
                        SESTO_IF_ERR_RETURN(
                            _sesto_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));
                    }
                    /*  Download FW for one core in MDIO bus, This is called for all core if no broadcast */
                    ret_val = _sesto_firmware_download(pm_acc,
                                      sesto_falcon_ucode, sesto_falcon_ucode_len,
                                      sesto_merlin_ucode, sesto_merlin_ucode_len);
                    if ((ret_val != PHYMOD_E_NONE) && (ret_val != SESTO_FW_ALREADY_DOWNLOADED)) {
                        SESTO_RETURN_WITH_ERR
                            (ret_val, (_PHYMOD_MSG("firmware download failed")));
                    } else {
                        PHYMOD_DEBUG_VERBOSE(("Firmware download through MDIO success\n"));
                    }
                }
            }

            /*  Broadcast way of FW download sequence
             *  1.  Reset core for all cores
             *  2.  Enable the broadcast for all the cores
             *  3.  Download FW for one core in MDIO bus
             *  4.  Disable the broadcast and Verify the FW on all cores
             */

            if (PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_GET(init_config)) {
                /* FW force method is force or Auto with different FW version then do chip reset */
                if (PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_GET(init_config) ||
                   (PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config) && (new_fm_version != (fw_info.fw_version & 0xFF)))) {
                    /* Do chip hard reset and Resetting the Cores */
                    SESTO_IF_ERR_RETURN(
                        _sesto_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));
                }
            } else if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {
                /* FW force method is force or Auto with different FW version then do enable broadcast */
                if (PHYMOD_CORE_INIT_F_FW_FORCE_DOWNLOAD_GET(init_config) ||
                   (PHYMOD_CORE_INIT_F_FW_AUTO_DOWNLOAD_GET(init_config) && (new_fm_version != (fw_info.fw_version & 0xFF)))) {
                    /* Enable broadcast */
                    SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_MICRO_SYNC_7, brdcst_en, 1);
                }
            } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {
                /*  Download FW for one core in MDIO bus, This is called for all core if no broadcast */
                ret_val = _sesto_firmware_download(pm_acc,
                                  sesto_falcon_ucode, sesto_falcon_ucode_len,
                                  sesto_merlin_ucode, sesto_merlin_ucode_len);
                if ((ret_val != PHYMOD_E_NONE) && (ret_val != SESTO_FW_ALREADY_DOWNLOADED)) {
                    SESTO_RETURN_WITH_ERR
                        (ret_val, (_PHYMOD_MSG("firmware download failed")));
                } else {
                    PHYMOD_DEBUG_VERBOSE(("Firmware download through MDIO success\n"));
                }
            } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {
                /* Disable broadcast */
                SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_MICRO_SYNC_7, brdcst_en, 0);
                /* check download_done flags again */
                READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
                if (boot_por.fields.mst_dwld_done != 1 || boot_por.fields.slv_dwld_done != 1) {
                    SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                }
                READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR, data);
                if (data == 0x1) {
                    SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                }
                SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_MST_RUNNING_CHKSUM, mst_running_chksum_val, data);
                if (0x600D != data) {
                    SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("Master Dload chksum Fail")));
                }
                SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_SLV_RUNNING_CHKSUM, slv_running_chksum_val, data);
                if (0x600D != data) {
                    SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                        (_PHYMOD_MSG("Slave Dload chksum Fail")));
                }
            }
        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;        
        case phymodFirmwareLoadMethodNone:
             /* Expected to download firmware from Flashed EEPROM */
             /* Firmware download works only when serboot pin is set on the chip */
             READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
             if (boot_por.fields.serboot) {
                 /* check download_done flags again */
                 READ_SESTO_PMA_PMD_REG(pm_acc, SES_MICRO_BOOT_BOOT_POR_ADR, boot_por.data);
                 if (boot_por.fields.mst_dwld_done != 1 || boot_por.fields.slv_dwld_done != 1) {
                     SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                         (_PHYMOD_MSG("ERR:Firmware Download Done got cleared")));
                 }
                 READ_SESTO_PMA_PMD_REG(pm_acc, SES_GEN_CNTRLS_FIRMWARE_VERSION_ADR, data);
                 if (data == 0x1) {
                     SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                         (_PHYMOD_MSG("ERR:Invalid Firmware version Downloaded")));
                 }
                 SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_MST_RUNNING_CHKSUM, mst_running_chksum_val, data);
                 if (0x600D != data) {
                     SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                         (_PHYMOD_MSG("Master Dload chksum Fail")));
                 }
                 SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_SLV_RUNNING_CHKSUM, slv_running_chksum_val, data);
                 if (0x600D != data) {
                     SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                         (_PHYMOD_MSG("Slave Dload chksum Fail")));
                 }
                 /* This is required When FW is already downloaded(Phy-1956), It will not affect EEPROM 
                  * Download*/
                 ret_val = SESTO_FW_ALREADY_DOWNLOADED;

             } else {
                 PHYMOD_DEBUG_ERROR(("WARN:SERBOOT PIN VALUE IS EXPECTED\n"));
             }
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
            /* EEPROM fusing starts with master fallowed by slave ucode b/w hardreset of chip */
            /* Master f/w download using mdio and then fusing to eeprom */
            PHYMOD_DEBUG_VERBOSE((
               "Starting Firmware download through EEPROM, it takes few seconds...\n"));
            /* Slave f/w download using mdio and then fusing to eeprom */
            ret_val = _sesto_eeprom_firmware_download(pm_acc,
                               sesto_merlin_ucode, sesto_merlin_ucode_len, 0);
            if ((ret_val != PHYMOD_E_NONE) && (ret_val != SESTO_FW_ALREADY_DOWNLOADED)) {
                SESTO_RETURN_WITH_ERR
                    (ret_val,
                    (_PHYMOD_MSG("Slave firmware download failed")));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Slave Firmware download through EEPROM success\n"));
            }

            ret_val = _sesto_eeprom_firmware_download(pm_acc,
                               sesto_falcon_ucode, sesto_falcon_ucode_len, 1);
            if ((ret_val != PHYMOD_E_NONE) && (ret_val != SESTO_FW_ALREADY_DOWNLOADED)) {
                SESTO_RETURN_WITH_ERR
                    (ret_val,
                    (_PHYMOD_MSG("Master firmware download failed")));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Master Firmware download through EEPROM success\n"));
            }
        break;
        default:
            SESTO_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }

    /* Disable PCS moniter at core_init as deafult is enable */
    for (ip = SESTO_MERLIN_CORE ; ip <= SESTO_FALCON_CORE ;ip++) {
        max_lane = (ip == SESTO_FALCON_CORE) ? SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;
        for (lane = 0; lane < max_lane; lane ++) {
            if (ip == SESTO_FALCON_CORE) {
                READ_SESTO_PMA_PMD_REG(pm_acc, (DP_FMON0_MAIN_CONTROL_ADR + lane), fmon_main_ctrl.data);
                fmon_main_ctrl.fields.fmon0_main_ctrl &= ~(0x1);
                WRITE_SESTO_PMA_PMD_REG(pm_acc, (DP_FMON0_MAIN_CONTROL_ADR + lane), fmon_main_ctrl.data);
                PHYMOD_DEBUG_VERBOSE(("Falcon Link monitor for Lane:%d Data:0x%x\n",
                            lane, fmon_main_ctrl.data));
            } else {
                READ_SESTO_PMA_PMD_REG(pm_acc, (DP_MMON0_MAIN_CONTROL_ADR + lane), mmon_main_ctrl.data);
                mmon_main_ctrl.fields.mmon0_main_ctrl &= ~(0x1);
                WRITE_SESTO_PMA_PMD_REG(pm_acc, (DP_MMON0_MAIN_CONTROL_ADR + lane), mmon_main_ctrl.data);
                PHYMOD_DEBUG_VERBOSE(("Merlin Link monitor for Lane:%d Data:0x%x\n",
                            lane, mmon_main_ctrl.data));
            }
        }
    }
    /* This is WAR for chip limitation: Disable the AN during init
       as we are not supposed to call interface set while AN is enabled */
        SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_GPREG11, an_enable, data);
    if (data) {
        SESTO_CHIP_FIELD_WRITE(pm_acc, SES_GEN_CNTRLS_GPREG11, an_enable, 0x0);

        /* Enable FW After configuring mode */
        SESTO_IF_ERR_RETURN(
            _sesto_fw_enable(pm_acc, SESTO_ENABLE));
        do {
            SESTO_CHIP_FIELD_READ(pm_acc, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);

            PHYMOD_DEBUG_VERBOSE(("FW Clear:%d\n",data));
            retry_cnt--;
        } while((data != 0) && (retry_cnt));
        if (retry_cnt == 0) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT,
                    (_PHYMOD_MSG("WARN:: FW Enable not cleared\n")));
        }
    }

ERR:
    return rv;
}

int _sesto_set_slice_reg (const phymod_access_t* pm_acc, SESTO_SLICE_CAST_TYPE cast_type, uint16_t ip, uint16_t dev_type,
                          uint16_t mcast_val, uint16_t lane)
{
    DEV1_SLICE_SLICE_TYPE_T slice_reg;
    int rv = PHYMOD_E_NONE;
    
    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(DEV1_SLICE_SLICE_TYPE_T));

    if (dev_type == SESTO_DEV_PMA_PMD) {
        READ_SESTO_PMA_PMD_REG(pm_acc, DEV1_SLICE_SLICE_ADR, slice_reg.data);
    } else {
        READ_SESTO_AN_REG(pm_acc, DEV7_SLICE_SLICE_ADR, slice_reg.data);
    }
    slice_reg.fields.ip   = ip;                 
    slice_reg.fields.lane = lane;                   
    slice_reg.fields.dev = dev_type;                   
    if (cast_type == SESTO_SLICE_UNICAST) {
        slice_reg.fields.cast = 0;
    } else if (cast_type == SESTO_SLICE_MULTICAST) {
        slice_reg.fields.cast = mcast_val;
    } else { 
        slice_reg.fields.cast = 8;
    }
    if (dev_type == SESTO_DEV_PMA_PMD) {
        WRITE_SESTO_PMA_PMD_REG(pm_acc, DEV1_SLICE_SLICE_ADR, slice_reg.data);
    } else {
        WRITE_SESTO_AN_REG(pm_acc, DEV7_SLICE_SLICE_ADR, slice_reg.data);
    }

ERR:
    return rv;
}

void _sesto_lane_cast_get(const phymod_phy_access_t *phy, SESTO_CORE_TYPE ip,
                          uint16_t *cast_type, uint16_t *mcast_val) 
{
    const phymod_access_t *pa = &phy->access;

    switch (pa->lane_mask) {
        case 3:
            *cast_type = SESTO_SLICE_MULTICAST;
            *mcast_val = 1;
            break;
        case 0xc:
            *cast_type = SESTO_SLICE_MULTICAST;
            *mcast_val = 2;
            break;
        case 0x30:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type = SESTO_SLICE_MULTICAST;
                *mcast_val = 3;
            } else {
                *cast_type = SESTO_CAST_INVALID;
            }
            break;
        case 0xc0:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type = SESTO_SLICE_MULTICAST;
                *mcast_val = 4;
            } else {
                *cast_type = SESTO_CAST_INVALID;
            }
            break;
        case 0x300:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type = SESTO_SLICE_MULTICAST;
                *mcast_val = 5;
            } else {
                *cast_type = SESTO_CAST_INVALID;
            }
            break;
        case 0xf:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type = SESTO_SLICE_MULTICAST;
                *mcast_val = 6;
            } else {
                *cast_type = SESTO_SLICE_BROADCAST;
            }
            break;
        case 0xf0:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type = SESTO_SLICE_MULTICAST;
                *mcast_val = 7;
            } else {
                *cast_type = SESTO_CAST_INVALID;
            }
            break;
        case 0x3ff:
            if (ip == SESTO_MERLIN_CORE) {
                *cast_type =SESTO_SLICE_BROADCAST; 
            } else {
                *cast_type = SESTO_CAST_INVALID;
            }
            break;
        default:
            *cast_type = SESTO_SLICE_UNICAST;
        break;
    }
}

int _sesto_rx_pmd_lock_get(const phymod_phy_access_t* phy, uint32_t* rx_pmd_locked)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t temp = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    if (rx_pmd_locked) {
        *rx_pmd_locked = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, 
                        ip, SESTO_DEV_PMA_PMD,  0, lane));
            if (ip == SESTO_FALCON_CORE) {
	            SESTO_CHIP_FIELD_READ(pa, F25G_TLB_RX_PMD_RX_LOCK_STATUS, pmd_rx_lock, temp);
                *rx_pmd_locked &= temp;
            } else {
                SESTO_CHIP_FIELD_READ(pa, M10G_TLB_RX_PMD_RX_LOCK_STATUS, pmd_rx_lock, temp);
                *rx_pmd_locked &= temp;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int sesto_is_lane_mask_set(const phymod_phy_access_t *phy, uint16_t prt,
                           const phymod_phy_inf_config_t *config)
{
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux;

    lane_mask = phy->access.lane_mask;
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    aux = (SESTO_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    SESTO_GET_IP(phy, cfg, core) ;
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d port:%d\n", __func__, core, prt));
    if (core == SESTO_MERLIN_CORE) {
        if (cfg.data_rate == SESTO_SPD_40G || cfg.data_rate == SESTO_SPD_42G) {
            if (prt == 0) {
                if (aux->pass_thru) {
                    if (aux->BCM84793_capablity) {
                        return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                    } else {
                        return ((lane_mask & 0x33) == 0x33) ? 1 : 0 ;
                    }
                } else {
                    return ((lane_mask & 0xF) == 0xF) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xF0) == 0xf0) ? 1 : 0;
                }
            }
        }
        if (cfg.data_rate == SESTO_SPD_20G || cfg.data_rate == SESTO_SPD_21G) {
            /* For 20G PT and MUX we are using same lane MAP*/
            if (prt == 0) {
                return ((lane_mask & 0x3) == 0x3) ? 1 :0;
            } else {
                return ((lane_mask & 0x30) == 0x30) ? 1 :0;
            }
        }
        if (cfg.data_rate == SESTO_SPD_1G || cfg.data_rate == SESTO_SPD_10G || cfg.data_rate == SESTO_SPD_11G) {
            /* For 10G PT and MUX we are using same lane MAP*/
            if (prt == 0) {
                return (lane_mask & 0x1) ? 1 :0;
            } else if (prt == 1) {
                return (lane_mask & 0x2) ? 1 :0;
            } else if (prt == 2) {
                if (aux->BCM84793_capablity) {
                    return (lane_mask & 0x4) ? 1 :0;
                } else {
                    return (lane_mask & 0x10) ? 1 :0;
                }
            } else if (prt == 3) {
                if (aux->BCM84793_capablity) {
                    return (lane_mask & 0x8) ? 1 :0;
                } else {
                    return (lane_mask & 0x20) ? 1 :0;
                }
            }
        }
    } else {
        if (cfg.data_rate == SESTO_SPD_40G || cfg.data_rate == SESTO_SPD_42G) {
            if (prt == 0) {
                if (aux->pass_thru) {
                    return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                } else {
                    return ((lane_mask & 0x3) == 0x3) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xc) == 0xc) ? 1 : 0;
                }
            }
        }
        if (cfg.data_rate == SESTO_SPD_20G || cfg.data_rate == SESTO_SPD_21G) {
            if (prt == 0) {
                if (aux->pass_thru) {
                    return ((lane_mask & 0x3) == 0x3) ? 1 :0;
                } else {
                    return (lane_mask & 0x1) ? 1 :0;
                }
            } else {
                if (aux->pass_thru) {
                    return ((lane_mask & 0xc) == 0xc) ? 1 :0;
                } else {
                    return (lane_mask & 0x4) ? 1 :0;
                }
            }
        }
        if (cfg.data_rate == SESTO_SPD_1G || cfg.data_rate == SESTO_SPD_10G || cfg.data_rate == SESTO_SPD_11G) {
            /* For 10G PT and MUX we are using same lane MAP*/
            if (prt == 0) {
                return (lane_mask & 0x1) ? 1 :0;
            } else if (prt == 1) {
                return (lane_mask & 0x2) ? 1 :0;
            } else if (prt == 2) {
                return (lane_mask & 0x4) ? 1 :0;
            } else if (prt == 3) {
                return (lane_mask & 0x8) ? 1 :0;
            }
        }
    }

    return 0;
}
/** Configure Dut Mode Register
 */
int _sesto_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config)
{
    uint16_t no_of_ports = 0, port_idx = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    DP_SESTO_MODE_CTRL0_TYPE_T mode_ctrl0;
    uint16_t core = 0;
    uint16_t is_lane_used = 0;
    phymod_phy_inf_config_t cfg;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(DP_SESTO_MODE_CTRL0_TYPE_T));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    SESTO_GET_IP(phy, cfg, core);

    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    SESTO_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);

    if (config->data_rate != SESTO_SPD_100G && config->data_rate != SESTO_SPD_106G) {
        READ_SESTO_PMA_PMD_REG(&phy->access, DP_SESTO_MODE_CTRL0_ADR, mode_ctrl0.data);
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                if ((aux_mode->BCM84793_capablity && core == SESTO_MERLIN_CORE) || core == SESTO_FALCON_CORE) {
                    if (phy->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                } else { /*  For non BCM84793 with Merlin*/
                    if (phy->access.lane_mask & (1 << ((port_idx < 2) ? port_idx : (port_idx + 2)))) {
                       is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if((config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) && (!aux_mode->pass_thru)) {
                if (core == SESTO_MERLIN_CORE) {
                    /* MUX Mode*/
                    if (phy->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (phy->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                        continue;
                    }
                }
           } else if(config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G) {
               if (aux_mode->pass_thru) {
                   if (core == SESTO_MERLIN_CORE) {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   } else {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 2))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               } else { /* MUX mode*/
                   if (core == SESTO_MERLIN_CORE) {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   } else { /* Falcon*/
                       if (phy->access.lane_mask & (0x1 << (port_idx * 2))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               }
           } else {
                is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }
           
            /* Program Modes */
            if (((config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_20G ||
                  config->data_rate == SESTO_SPD_42G || config->data_rate == SESTO_SPD_21G) && (port_idx == 0)) ||
                 ((config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G ||
                   config->data_rate == SESTO_SPD_11G) && (port_idx < 2))) {
                if (config->data_rate == SESTO_SPD_40G || (config->data_rate == SESTO_SPD_42G)) {
                        mode_ctrl0.fields.mode_multiport1_40g_mux = !(aux_mode->pass_thru);
                        /* Clearing the 20G PT & MUX bits and 10G bits*/
                        mode_ctrl0.fields.mode_multiport1_20g_passthru = 0;
                        mode_ctrl0.fields.mode_multiport1_20g_mux = 0; 
                        mode_ctrl0.fields.mode_multiport1_10g1 = 0;
                        mode_ctrl0.fields.mode_multiport1_10g2 = 0;
                }
                if (config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G) {
                    if (aux_mode->pass_thru) {
                       mode_ctrl0.fields.mode_multiport1_20g_passthru = aux_mode->pass_thru;
                       mode_ctrl0.fields.mode_multiport1_20g_mux = 0;
                    } else {
                       mode_ctrl0.fields.mode_multiport1_20g_mux = 1;
                        mode_ctrl0.fields.mode_multiport1_20g_passthru = 0;
                    }
                    /* Clearing the 40G Mux and 20G PT & MUX bits */
                    mode_ctrl0.fields.mode_multiport1_40g_mux = 0;
                    mode_ctrl0.fields.mode_multiport1_10g1 = 0;
                    mode_ctrl0.fields.mode_multiport1_10g2 = 0;
                }
                if (config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                    if (phy->access.lane_mask & 0x1) {
                        mode_ctrl0.fields.mode_multiport1_10g1 = 1;
                    } if (phy->access.lane_mask & 0x2) {
                        mode_ctrl0.fields.mode_multiport1_10g2 = 1;
                    }
                    /* Clearing the 40G Mux and 20G PT & MUX bits */
                    mode_ctrl0.fields.mode_multiport1_40g_mux = 0;
                    mode_ctrl0.fields.mode_multiport1_20g_passthru = 0;
                    mode_ctrl0.fields.mode_multiport1_20g_mux = 0; 
                }
            } else {
                if (config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) {
                    mode_ctrl0.fields.mode_multiport2_40g_mux = !(aux_mode->pass_thru);
                    /* Clearing the 20G PT & MUX bits and 10G bits*/
                    mode_ctrl0.fields.mode_multiport2_20g_passthru = 0;
                    mode_ctrl0.fields.mode_multiport2_20g_mux = 0;
                    mode_ctrl0.fields.mode_multiport2_10g1 = 0;
                    mode_ctrl0.fields.mode_multiport2_10g2 = 0;
                }
                if (config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G) {
                    if (aux_mode->pass_thru) {
                        mode_ctrl0.fields.mode_multiport2_20g_passthru = aux_mode->pass_thru;
                        mode_ctrl0.fields.mode_multiport2_20g_mux = 0;
                    } else {
                        mode_ctrl0.fields.mode_multiport2_20g_mux = 1;
                        mode_ctrl0.fields.mode_multiport2_20g_passthru = 0;
                    }
                    mode_ctrl0.fields.mode_multiport2_40g_mux = 0;
                    mode_ctrl0.fields.mode_multiport2_10g1 = 0;
                    mode_ctrl0.fields.mode_multiport2_10g2 = 0;
                }
                if (config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                    if ((phy->access.lane_mask & 0x4) || (phy->access.lane_mask & 0x10)) {
                        mode_ctrl0.fields.mode_multiport2_10g1 = 1;
                    } if ((phy->access.lane_mask & 0x8) || (phy->access.lane_mask & 0x20)) {
                        mode_ctrl0.fields.mode_multiport2_10g2 = 1;
                    }
                    /* Clearing the 40G Mux and 20G PT & MUX bits */
                    mode_ctrl0.fields.mode_multiport2_40g_mux = 0;
                    mode_ctrl0.fields.mode_multiport2_20g_passthru = 0;
                    mode_ctrl0.fields.mode_multiport2_20g_mux = 0; 
                }
            }
        }
    }
    mode_ctrl0.fields.mode_falcon_line = (aux_mode->passthru_sys_side_core == SESTO_MERLIN_CORE) ?
                                                          SESTO_FALCON_CORE : SESTO_MERLIN_CORE;

    mode_ctrl0.fields.mode_enzo_combatible_en = aux_mode->BCM84793_capablity;
    mode_ctrl0.fields.mode_100g_gbox_en = (config->data_rate == SESTO_SPD_100G || config->data_rate == SESTO_SPD_106G) ? 1 : 0;
    mode_ctrl0.fields.mode_40g_passthru_en = (config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) &&
                                             (aux_mode->pass_thru);

    PHYMOD_DEBUG_VERBOSE(("MODE CTRL0 :0x%x\n", mode_ctrl0.data));

    WRITE_SESTO_PMA_PMD_REG(&phy->access, DP_SESTO_MODE_CTRL0_ADR, mode_ctrl0.data);

ERR:
    return rv;
}

int _sesto_toggle_mer_fal_lane_reset(const phymod_access_t *pa) 
{
    int rv = PHYMOD_E_NONE;

    SESTO_CHIP_FIELD_WRITE(pa, DP_FAL_MER_LINE_SIDE_RESET_CONTROL1, mer_ln_h_rstb, 0);
    SESTO_CHIP_FIELD_WRITE(pa, DP_FAL_MER_LINE_SIDE_RESET_CONTROL1, fal_ln_h_rstb, 0);
    
    PHYMOD_USLEEP(100);
    SESTO_CHIP_FIELD_WRITE(pa, DP_FAL_MER_LINE_SIDE_RESET_CONTROL1, mer_ln_h_rstb, 0x3ff);
    SESTO_CHIP_FIELD_WRITE(pa, DP_FAL_MER_LINE_SIDE_RESET_CONTROL1, fal_ln_h_rstb, 0xf);

ERR:
    return rv;
}

/** Configure Dut Mode Register
 */
int _sesto_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config,
                        SESTO_AN_MODE mode)
{
    uint16_t no_of_ports = 0, port_idx = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    DP_SESTO_MODE_CTRL1_TYPE_T mode_ctrl1;
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint16_t is_lane_used = 0;
    int rv = PHYMOD_E_NONE;

    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(DP_SESTO_MODE_CTRL1_TYPE_T));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    SESTO_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    SESTO_GET_IP(phy, cfg, core);

    if (config->data_rate != SESTO_SPD_100G && config->data_rate != SESTO_SPD_106G) {
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                if ((aux_mode->BCM84793_capablity && core == SESTO_MERLIN_CORE) || core == SESTO_FALCON_CORE) {
                    if (phy->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                } else { /*  For non BCM84793 with Merlin*/
                    if (phy->access.lane_mask & (1 << ((port_idx < 2) ? port_idx : (port_idx + 2)))) {
                       is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if((config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) && (!aux_mode->pass_thru)) {
                if (core == SESTO_MERLIN_CORE) {
                    /* MUX Mode*/
                    if (phy->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (phy->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                    } else {
                        continue;
                    }
                }
           } else if(config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G) {
               if (aux_mode->pass_thru) {
                   if (core == SESTO_MERLIN_CORE) {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   } else {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 2))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               } else { /* MUX mode*/
                   if (core == SESTO_MERLIN_CORE) {
                       if (phy->access.lane_mask & (0x3 << (port_idx * 4))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   } else { /* Falcon*/
                       if (phy->access.lane_mask & (0x1 << (port_idx * 2))) {
                           is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
                       } else {
                           continue;
                       }
                   }
               }
           } else {
                is_lane_used =  sesto_is_lane_mask_set(phy, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }

            READ_SESTO_PMA_PMD_REG(&phy->access, DP_SESTO_MODE_CTRL1_ADR, mode_ctrl1.data);
            /* Program Modes */
            if (((config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_20G ||
                  config->data_rate == SESTO_SPD_42G || config->data_rate == SESTO_SPD_21G) && (port_idx == 0)) ||
                  ((config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G ||
                    config->data_rate == SESTO_SPD_11G) && (port_idx < 2))) {
                mode_ctrl1.fields.udms_en1 = (mode == SESTO_AN_NONE || mode == SESTO_AN_CL73) ? 1 : 0;
                if (config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G ||
                    config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) {
                    mode_ctrl1.fields.mode_dmept1_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                    if (phy->access.lane_mask & 0x1) {
                        mode_ctrl1.fields.mode_dmept1_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                    } else if (phy->access.lane_mask & 0x2) {
                        mode_ctrl1.fields.mode_dmept1b_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                    }
                }
            } else {
                mode_ctrl1.fields.udms_en2 = (mode == SESTO_AN_NONE || mode == SESTO_AN_CL73) ? 1 : 0;
                if (config->data_rate == SESTO_SPD_20G || config->data_rate == SESTO_SPD_21G ||
                    config->data_rate == SESTO_SPD_40G || config->data_rate == SESTO_SPD_42G) {
                    mode_ctrl1.fields.mode_dmept2_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == SESTO_SPD_1G || config->data_rate == SESTO_SPD_10G || config->data_rate == SESTO_SPD_11G) {
                    if (phy->access.lane_mask & 0x1) {
                        mode_ctrl1.fields.mode_dmept2_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                    } else if (phy->access.lane_mask & 0x2) {
                        mode_ctrl1.fields.mode_dmept2b_en = (mode == SESTO_AN_PROP) ? 1 : 0;
                    }
                }
            }
        }
    } else {
        mode_ctrl1.fields.udms_en1 = (mode == SESTO_AN_NONE || mode == SESTO_AN_CL73) ? 1 : 0;
    }
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL1 :0x%x\n", mode_ctrl1.data));
    WRITE_SESTO_PMA_PMD_REG(&phy->access, DP_SESTO_MODE_CTRL1_ADR, mode_ctrl1.data);

ERR:
    return rv;
}

int _sesto_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config)
{
    uint16_t phy_type = 0;
    uint16_t media_type = 2, ip = 0; /* Default to optical */
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    SES_GEN_CNTRLS_GPREG11_TYPE_T   gpreg11;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T intf_sel_spd;
    SES_GEN_CNTRLS_MICRO_SYNC_5_TYPE_T intf_sel_p0_p1;
    SES_GEN_CNTRLS_MICRO_SYNC_6_TYPE_T intf_sel_p2_p3;
    phymod_phy_inf_config_t cpy_config;
    uint16_t intf = config->interface_type;
    const phymod_access_t *pa = &phy->access;
    uint16_t dfe_en = 0, dfe_lp_en = 0;
    uint16_t port = 0, no_of_ports = 0;
    uint16_t scrambling_dis = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&gpreg11, 0, sizeof(gpreg11));
    PHYMOD_MEMSET(&intf_sel_spd, 0, sizeof(intf_sel_spd));
    PHYMOD_MEMSET(&intf_sel_p0_p1, 0, sizeof(intf_sel_p0_p1));
    PHYMOD_MEMSET(&intf_sel_p2_p3, 0, sizeof(intf_sel_p2_p3));

    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    PHYMOD_MEMCPY(&cpy_config, config, sizeof(phymod_phy_inf_config_t));
    SESTO_GET_IP(phy, cpy_config, ip);
    SESTO_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    SESTO_IF_ERR_RETURN(_sesto_port_from_lane_map_get(phy, config, &port));

    if (intf == phymodInterfaceSR4 || intf == phymodInterfaceSR10 ||
        intf == phymodInterfaceER4 || intf == phymodInterfaceLR10 ||
        intf == phymodInterfaceSR2 || intf == phymodInterfaceLR2 ||
        intf == phymodInterfaceER2 || intf == phymodInterfaceLR ||
        intf == phymodInterfaceSR || intf == phymodInterfaceER ||
        intf == phymodInterfaceSFI || intf == phymodInterfaceXLPPI) {
        media_type = 2; /*OPTICAL*/
        dfe_en = 0;
        dfe_lp_en = 0;
    } else if (intf == phymodInterfaceCR4 || intf == phymodInterfaceCR ||
               intf == phymodInterfaceCR2 || intf == phymodInterfaceCR10) {
        media_type = 1; /*Copper*/
        dfe_en = 1;
        dfe_lp_en = 0;
    } else if (intf == phymodInterfaceKR4 || intf == phymodInterfaceKR ||
               intf == phymodInterfaceKR2 || intf == phymodInterfaceKR10) {
        media_type = 0; /* Back plane */
        dfe_en = 1;
        dfe_lp_en = 0;
    } else if (intf == phymodInterfaceCAUI4 || intf == phymodInterfaceVSR ||
               intf == phymodInterfaceCAUI || intf == phymodInterfaceCAUI4_C2M ||
               intf == phymodInterfaceCAUI4_C2C) {
        media_type = 0; /* Back plane */
        dfe_en = 1;
        dfe_lp_en = 1;
    } else if (intf == phymodInterfaceXLAUI2 || intf == phymodInterfaceXLAUI ||
               intf == phymodInterfaceXFI) {
        media_type = 0; /* Back plane */
        dfe_en = 0;
        dfe_lp_en = 0;
    } else if (intf == phymodInterfaceLR4) {
        media_type = 2; /*OPTICAL*/
        dfe_en = 0;
        dfe_lp_en = 0;
        if (config->data_rate == SESTO_SPD_100G || config->data_rate == SESTO_SPD_106G) {
            dfe_en = 1;
        }
    } else {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_PARAM,
               (_PHYMOD_MSG("Invalid interface type..")));
    }

    /* For 1G support */
    if (config->data_rate == SESTO_SPD_1G) {
        scrambling_dis = 1;
    }

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
    if (no_of_ports == 1) { /* 100G/40GPT */
        if (ip == SESTO_MERLIN_CORE) {
            intf_sel_p0_p1.fields.p0_10g_media_type = media_type;
            intf_sel_p0_p1.fields.p0_10g_dfe_on = dfe_en;
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            /* 100G/40GPT has only one port clearing the MICRO_SYNC_6 */
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, 0x0);
        } else {
            intf_sel_p0_p1.fields.p0_20g_media_type = media_type;
            intf_sel_p0_p1.fields.p0_20g_dfe_on = dfe_en;
            intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = dfe_lp_en;
            intf_sel_p0_p1.fields.p0_scrambling_dis = scrambling_dis; 
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            /* 100G/40GPT has only one port clearing the MICRO_SYNC_6 */
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, 0x0);
        }
    } else if (no_of_ports == 2) { /* 40GMUX/20GMUX/20PT */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p0_p1.fields.p0_10g_media_type = media_type;
                intf_sel_p0_p1.fields.p0_10g_dfe_on = dfe_en;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                intf_sel_p0_p1.fields.p0_20g_media_type = media_type;
                intf_sel_p0_p1.fields.p0_20g_dfe_on = dfe_en;
                intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p0_p1.fields.p0_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p2_p3.fields.p2_10g_media_type = media_type;
                intf_sel_p2_p3.fields.p2_10g_dfe_on = dfe_en;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                intf_sel_p2_p3.fields.p2_20g_media_type = media_type;
                intf_sel_p2_p3.fields.p2_20g_dfe_on = dfe_en;
                intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p2_p3.fields.p2_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        }
    } else { /* 10G */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p0_p1.fields.p0_10g_media_type = media_type;
                intf_sel_p0_p1.fields.p0_10g_dfe_on = dfe_en;
                intf_sel_p0_p1.fields.p0_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                intf_sel_p0_p1.fields.p0_20g_media_type = media_type;
                intf_sel_p0_p1.fields.p0_20g_dfe_on = dfe_en;
                intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p0_p1.fields.p0_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else if (port == 1) {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p0_p1.fields.p1_10g_media_type = media_type;
                intf_sel_p0_p1.fields.p1_10g_dfe_on = dfe_en;
                intf_sel_p0_p1.fields.p1_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                intf_sel_p0_p1.fields.p1_20g_media_type = media_type;
                intf_sel_p0_p1.fields.p1_20g_dfe_on = dfe_en;
                intf_sel_p0_p1.fields.p1_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p0_p1.fields.p1_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else if (port == 2) {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p2_p3.fields.p2_10g_media_type = media_type;
                intf_sel_p2_p3.fields.p2_10g_dfe_on = dfe_en;
                intf_sel_p2_p3.fields.p2_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                intf_sel_p2_p3.fields.p2_20g_media_type = media_type;
                intf_sel_p2_p3.fields.p2_20g_dfe_on = dfe_en;
                intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p2_p3.fields.p2_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                intf_sel_p2_p3.fields.p3_10g_media_type = media_type;
                intf_sel_p2_p3.fields.p3_10g_dfe_on = dfe_en;
                intf_sel_p2_p3.fields.p3_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                intf_sel_p2_p3.fields.p3_20g_media_type = media_type;
                intf_sel_p2_p3.fields.p3_20g_dfe_on = dfe_en;
                intf_sel_p2_p3.fields.p3_20g_dfe_lp_mode = dfe_lp_en;
                intf_sel_p2_p3.fields.p3_scrambling_dis = scrambling_dis;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        }
    }  

    if (PHYMOD_INTF_MODES_OTN_GET(config)) {
        phy_type = 1;
    } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
        phy_type = 2;
    } else {
        phy_type = 0; /* Default to ethernet mode*/
    }
    /* Configure PHY Type*/
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);
    gpreg11.fields.phy_type = phy_type;
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);

    /* SES_GEN_CNTRLS_MICRO_SYNC_4_ADR:  12-15 bits are used as scratch register to configure the 1G support */
    /* 12-15 bits also used to support speed change in HIGIG interface mode */
    if ((config->data_rate == SESTO_SPD_1G) || (PHYMOD_INTF_MODES_HIGIG_GET(config) && ((config->data_rate == SESTO_SPD_10G) ||
       (config->data_rate == SESTO_SPD_20G) || (config->data_rate == SESTO_SPD_40G) || (config->data_rate == SESTO_SPD_100G)))){
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, intf_sel_spd.data);
        intf_sel_spd.data |= (1 << (12 + port));
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, intf_sel_spd.data);
    } else {
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, intf_sel_spd.data);
        intf_sel_spd.data &= ~(1 << (12 + port));
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, intf_sel_spd.data);
    }

    PHYMOD_DEBUG_VERBOSE(("GPREG11 data:%x\n",gpreg11.data));
    PHYMOD_DEBUG_VERBOSE(("SYNC_4 data:%x\n",intf_sel_spd.data));
    PHYMOD_DEBUG_VERBOSE(("SYNC_5 data:%x\n",intf_sel_p0_p1.data));
    PHYMOD_DEBUG_VERBOSE(("SYNC_6 data:%x\n",intf_sel_p2_p3.data));

    SESTO_IF_ERR_RETURN(
        _sesto_set_ieee_intf(phy, intf, config, ip));

ERR:
    return rv;
}

int _sesto_set_ieee_intf(const phymod_phy_access_t *phy, uint16_t intf, const phymod_phy_inf_config_t *config, uint16_t ip)
{
    uint16_t port = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T mod_sel;

    PHYMOD_MEMSET(&mod_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));
    SESTO_IF_ERR_RETURN(
        _sesto_port_from_lane_map_get(phy, config, &port));
    
    READ_SESTO_PMA_PMD_REG(pa, (SES_GEN_CNTRLS_MICRO_SYNC_0_ADR + port), mod_sel.data);
    SESTO_GET_INTF_SIDE(phy, intf_side);
    mod_sel.data = intf_side ? (mod_sel.data & 0xFF) : (mod_sel.data & 0xFF00);
    mod_sel.data |= intf_side ? (intf << 8) : (intf);
    WRITE_SESTO_PMA_PMD_REG(pa, (SES_GEN_CNTRLS_MICRO_SYNC_0_ADR + port), mod_sel.data);

ERR:
    return rv;
}

int _sesto_get_ieee_intf(const phymod_phy_access_t *phy, uint16_t ip, const phymod_phy_inf_config_t *config, uint16_t *intf)
{
    uint16_t port = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T mod_sel;

    PHYMOD_MEMSET(&mod_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));
    SESTO_IF_ERR_RETURN(
        _sesto_port_from_lane_map_get(phy, config, &port));

    READ_SESTO_PMA_PMD_REG(pa, (SES_GEN_CNTRLS_MICRO_SYNC_0_ADR + port), mod_sel.data);
    SESTO_GET_INTF_SIDE(phy, intf_side);
    *intf = intf_side ? ((mod_sel.data >> 8) & 0xFF) : (mod_sel.data & 0x00FF);

ERR:
    return rv;
}

int _sesto_pll_config_state_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, uint16_t *pll_cfg_state)
{
    phymod_phy_inf_config_t cfg;
    SESTO_DEVICE_AUX_MODE_T *aux_mode;
    uint16_t ip = 0;
    uint16_t no_of_ports = 0;
    DP_DECODED_MODE_STATUS0_TYPE_T mode_sts0;
    uint16_t fal_pll_lock = 0, mer_pll_lock = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&cfg, 0, sizeof(phymod_phy_inf_config_t));
    cfg.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &cfg));

    READ_SESTO_PMA_PMD_REG(pa, DP_DECODED_MODE_STATUS0_ADR, mode_sts0.data);
    for (ip = SESTO_MERLIN_CORE; ip <= SESTO_FALCON_CORE; ip++) {
        SESTO_IF_ERR_RETURN(
            _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip, SESTO_DEV_PMA_PMD, 0, 0));
        if (ip == SESTO_FALCON_CORE) {
            SESTO_CHIP_FIELD_READ(pa, FALCON_IF_PMD_PLL_STS, pmd_pll_lock, fal_pll_lock);
        } else {
            SESTO_CHIP_FIELD_READ(pa, MERLIN_IF_PMD_PLL_LOCK_STATUS, pmd_pll_lock, mer_pll_lock);
        }
    }
    /*
     *  PLL div is per die and will be programed only when
     *  1. if there is no pll lock on merlin or falcon
     *  2. speed change b/w 100G to 40G/20G/10G
     *  3. if ref clock freq changes(pll lock lost)
     *  4. On interface_modes changes(IEEE/OTN/HIGIG)
     */
    SESTO_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    if (((fal_pll_lock != 1) || (mer_pll_lock != 1)) || (config->ref_clock != cfg.ref_clock) ||
        ((config->interface_modes & (PHYMOD_INTF_MODES_HIGIG | PHYMOD_INTF_MODES_OTN)) != (cfg.interface_modes & (PHYMOD_INTF_MODES_HIGIG | PHYMOD_INTF_MODES_OTN))) ||
        ((no_of_ports == 1) && (config->data_rate != cfg.data_rate)) || (!mode_sts0.data) ||
        ((no_of_ports > 1) && (cfg.data_rate == SESTO_SPD_100G) && (config->data_rate != SESTO_SPD_100G)) ||
        ((no_of_ports > 1) && (cfg.data_rate == SESTO_SPD_106G) && (config->data_rate != SESTO_SPD_106G))) {
        *pll_cfg_state = 1;
    }
    /* HIGIG mode speed change */
    if ((config->interface_modes == cfg.interface_modes) && (config->data_rate != cfg.data_rate)) {
        *pll_cfg_state = 1;
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(cfg.device_aux_modes);

    return rv;
}


int _sesto_phy_interface_config_set(const phymod_phy_access_t* phy, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint16_t pll_cfg_state = 0;
    int rv = PHYMOD_E_NONE;

    /* Get the pll config state */
    SESTO_IF_ERR_RETURN(
          _sesto_pll_config_state_get(phy, config, &pll_cfg_state));

    /* Configure Ref Clock*/
    PHYMOD_DEBUG_VERBOSE(("Configuring REF clock\n"));
    SESTO_IF_ERR_RETURN(
          _sesto_configure_ref_clock(&phy->access, config->ref_clock));
    
    /* Configure DUT MODE */
    SESTO_IF_ERR_RETURN(
        _sesto_config_dut_mode_reg(phy, config));
    
    /* Enable UDMS for non-an MODE */
    PHYMOD_DEBUG_VERBOSE(("Configuring UDMS\n"));
    SESTO_IF_ERR_RETURN(
        _sesto_udms_config(phy, config, SESTO_AN_NONE));

    /* Configure PLL Divider*/
    if (pll_cfg_state) {
        PHYMOD_DEBUG_VERBOSE(("Configuring PLL divider\n"));
        SESTO_IF_ERR_RETURN(
            _sesto_config_pll_div(&phy->access, config));
    }

    /* Configure Interface */
    SESTO_IF_ERR_RETURN(
        _sesto_interface_set(phy, config));

ERR:
    return rv;
}

int _sesto_phy_interface_config_get(const phymod_phy_access_t *phy, uint32_t flags, phymod_phy_inf_config_t *config)
{
    uint16_t ip = 0, intf_side = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    DP_DECODED_MODE_STATUS0_TYPE_T mode_sts0;
    SES_GEN_CNTRLS_GPREG11_TYPE_T   gpreg11;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T ref_clk_freq;
    int rv = PHYMOD_E_NONE;
    uint16_t clock = 0, port = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = pa->lane_mask;

    PHYMOD_MEMSET(&mode_sts0, 0, sizeof(DP_DECODED_MODE_STATUS0_TYPE_T));
    PHYMOD_MEMSET(&gpreg11, 0, sizeof(SES_GEN_CNTRLS_GPREG11_TYPE_T));
    PHYMOD_MEMSET(&ref_clk_freq, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T));
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(SESTO_DEVICE_AUX_MODE_T));

    if (!lane_mask) {
        PHYMOD_DEBUG_VERBOSE(("Invalid Lanemask\n"));
        return PHYMOD_E_PARAM;
    }
    SESTO_GET_INTF_SIDE(phy, intf_side);
    READ_SESTO_PMA_PMD_REG(pa, DP_DECODED_MODE_STATUS0_ADR, mode_sts0.data);
    PHYMOD_DEBUG_VERBOSE(("MODE STS:%x\n", mode_sts0.data));
    if (intf_side == SESTO_IF_LINE) {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_FALCON_CORE : SESTO_MERLIN_CORE;
    } else {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;
    }
    if (ip == SESTO_FALCON_CORE) {
        if (mode_sts0.fields.decoded_mode_100g_gbox) {
            config->data_rate = SESTO_SPD_100G;
            aux_mode->gearbox_100g_inverse_mode = (mode_sts0.fields.decoded_mode_falcon_line) ? 0 : 1;
        } else if (lane_mask & 0x3) {
            if (mode_sts0.fields.decoded_mode_40g_mux_p1) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_40g_passthru) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 1;
                aux_mode->BCM84793_capablity = 0;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            } else if (mode_sts0.fields.decoded_mode_20g_mux_p1) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_20g_passthru_p1) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_10g_p1) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_10g_p2) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            }
        } else if (lane_mask & 0xc) {
            if (mode_sts0.fields.decoded_mode_40g_mux_p2) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_40g_passthru) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 1;
                aux_mode->BCM84793_capablity = 0;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            } else if (mode_sts0.fields.decoded_mode_20g_passthru_p2) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_20g_mux_p2) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_10g_p3) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            } else if (mode_sts0.fields.decoded_mode_10g_p4) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            }
        }
    } else {
        if (mode_sts0.fields.decoded_mode_100g_gbox) {
            config->data_rate = SESTO_SPD_100G;
            aux_mode->gearbox_100g_inverse_mode = (mode_sts0.fields.decoded_mode_falcon_line) ? 0 : 1;
        } else if (lane_mask & 0xF) {
            if ((mode_sts0.fields.decoded_mode_10g_p1) && (lane_mask & 0x1)) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            } else if ((mode_sts0.fields.decoded_mode_10g_p2) && (lane_mask & 0x2)) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                if ((mode_sts0.fields.decoded_mode_10g_p3) && (lane_mask & 0x4)) {
                    config->data_rate = SESTO_SPD_10G;
                    aux_mode->pass_thru = 1;
                    aux_mode->BCM84793_capablity = 1;
                } else if ((mode_sts0.fields.decoded_mode_10g_p4) && (lane_mask & 0x8)) { 
                    config->data_rate = SESTO_SPD_10G;
                    aux_mode->pass_thru = 1;
                    aux_mode->BCM84793_capablity = 1;
                } else if (mode_sts0.fields.decoded_mode_40g_passthru) {
                    config->data_rate = SESTO_SPD_40G;
                    aux_mode->pass_thru = 1;
                    aux_mode->BCM84793_capablity = 1;
                }
            } else if (mode_sts0.fields.decoded_mode_20g_mux_p1) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_20g_passthru_p1) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_40g_mux_p1) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_40g_passthru) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 1;
                aux_mode->BCM84793_capablity = 0;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            }
        } else if (lane_mask & 0xF0) {
            if (mode_sts0.fields.decoded_mode_10g_p3) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_10g_p4) {
                config->data_rate = SESTO_SPD_10G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_20g_mux_p2) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_20g_passthru_p2) {
                config->data_rate = SESTO_SPD_20G;
                aux_mode->pass_thru = 1;
            } else if (mode_sts0.fields.decoded_mode_40g_mux_p2) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 0;
            } else if (mode_sts0.fields.decoded_mode_40g_passthru) {
                config->data_rate = SESTO_SPD_40G;
                aux_mode->pass_thru = 1;
                if (mode_sts0.fields.decoded_mode_enzo_compatible) {
                    aux_mode->BCM84793_capablity = 1;
                }
            }
        }
    }
    aux_mode->passthru_sys_side_core =
        (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);
    if (gpreg11.fields.phy_type  == 1) {
        PHYMOD_INTF_MODES_OTN_SET(config);
    } else if (gpreg11.fields.phy_type == 2) {
        PHYMOD_INTF_MODES_HIGIG_SET(config);
        if (config->data_rate == SESTO_SPD_10G) {
            config->data_rate = SESTO_SPD_11G;
        } else if (config->data_rate == SESTO_SPD_20G) {
            config->data_rate = SESTO_SPD_21G;
        } else if (config->data_rate == SESTO_SPD_40G) {
            config->data_rate = SESTO_SPD_42G;
        } else if (config->data_rate == SESTO_SPD_100G) {
            config->data_rate = SESTO_SPD_106G;
        }
    } else {
        config->interface_modes = 0; /*Ethernet*/
    }
    SESTO_CHIP_FIELD_READ(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, clock);
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
    if (clock <= 0x2) {
        config->ref_clock = ((ref_clk_freq.data >> 8) & 0xF);
    }
    /*SES_GEN_CNTRLS_MICRO_SYNC_4_ADR 12-15 bits are defined for 1G support */
    /* 12-15 bits also used to support speed change in HIGIG interface mode */
    SESTO_IF_ERR_RETURN(_sesto_port_from_lane_map_get(phy, config, &port));
    if ((ref_clk_freq.data >> (12 + port)) & 0x1) {
        if (gpreg11.fields.phy_type == 2) {
            PHYMOD_INTF_MODES_HIGIG_SET(config);
            if (config->data_rate == SESTO_SPD_11G) {
                 config->data_rate = SESTO_SPD_10G;
            } else if (config->data_rate == SESTO_SPD_21G) {
                 config->data_rate = SESTO_SPD_20G;
            } else if (config->data_rate == SESTO_SPD_42G) {
                 config->data_rate = SESTO_SPD_40G;
            } else if (config->data_rate == SESTO_SPD_106G) {
                 config->data_rate = SESTO_SPD_100G;
            }
        } else {
            config->data_rate = SESTO_SPD_1G;
        }
    }

    SESTO_IF_ERR_RETURN(
       _sesto_get_ieee_intf(phy, ip, config, &intf_side));
    config->interface_type = intf_side;
    
ERR:
    return rv;
}

int _sesto_phy_pcs_link_get(const phymod_phy_access_t *phy, uint32_t *pcs_link)
{
    int rv = PHYMOD_E_NONE;
    uint32_t lock_lost_lh = 0, error_count = 0;

    if (pcs_link) {
        *pcs_link = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }
    SESTO_IF_ERR_RETURN(
            _sesto_get_pcs_link_status(phy, pcs_link, &lock_lost_lh, &error_count));

ERR:
    return rv;
}

int _sesto_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk)
{
    int rv = PHYMOD_E_NONE;
    uint16_t clk_scalar_code = 0x0;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T ref_clk_freq;

    PHYMOD_MEMSET(&ref_clk_freq, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T));
    /* SES_GEN_CNTRLS_MICRO_SYNC_4_ADR:  8-11 bits are used as scratch register to configure the ref_clk_freq */

    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            /* Writing to scratch reg to save in chip */
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            ref_clk_freq.data &= 0xF0FF;
            ref_clk_freq.data |= (ref_clk << 8);
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            SESTO_CHIP_FIELD_READ(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, clk_scalar_code);			
            if (clk_scalar_code != 0x02) { 
                SESTO_CHIP_FIELD_WRITE(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, 0x02);			
            }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            /* Writing to scratch reg to save in chip */
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            ref_clk_freq.data &= 0xF0FF;
            ref_clk_freq.data |= (ref_clk << 8);
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            SESTO_CHIP_FIELD_READ(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, clk_scalar_code);			
            if (clk_scalar_code != 0x01) { 
                SESTO_CHIP_FIELD_WRITE(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, 0x01);
            }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz:
        case phymodRefClk698Mhz:
            /* Writing to scratch reg to save in chip */
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            ref_clk_freq.data &= 0xF0FF;
            ref_clk_freq.data |= (ref_clk << 8);
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, ref_clk_freq.data);
            SESTO_CHIP_FIELD_READ(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, clk_scalar_code);			
            if (clk_scalar_code != 0x0) { 
                SESTO_CHIP_FIELD_WRITE(pa, SES_CTRL_CLOCK_SCALER_CTRL, clock_scaler_code, 0x0);
            }
        break;
        default:
	    PHYMOD_DEBUG_ERROR((" This Ref Clock (%d) is not supported\n", ref_clk));
	    return PHYMOD_E_PARAM;
    }

ERR:
    return rv;
}

int _sesto_get_pll_modes(phymod_ref_clk_t ref_clk,const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *mer_pll_mode) 
{
    uint32_t speed = config->data_rate;

    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            /* No OTN support on 156 and 312 */
            if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    return PHYMOD_E_PARAM;
            }
            if ((speed == SESTO_SPD_100G) || (speed == SESTO_SPD_106G)) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *fal_pll_mode = SESTO_PLL_MODE_175;
                    *mer_pll_mode = SESTO_PLL_MODE_70;
                    if (speed == SESTO_SPD_100G) {
                        *fal_pll_mode = SESTO_PLL_MODE_165;
                        *mer_pll_mode = SESTO_PLL_MODE_66;
                    }
            } else {
                    *fal_pll_mode = SESTO_PLL_MODE_165;
                    *mer_pll_mode = SESTO_PLL_MODE_66;
            }
        } else {
            /*-- Falcon Line Rate is 10G/20G*/
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    *fal_pll_mode = SESTO_PLL_MODE_140;
                    *mer_pll_mode = SESTO_PLL_MODE_70;
                    if ((speed == SESTO_SPD_40G) || (speed == SESTO_SPD_20G) || (speed == SESTO_SPD_10G)) {
                        *fal_pll_mode = SESTO_PLL_MODE_132;
                        *mer_pll_mode = SESTO_PLL_MODE_66;
                    }
                } else { /*IEEE MODE*/
                    *fal_pll_mode = SESTO_PLL_MODE_132;
                    *mer_pll_mode = SESTO_PLL_MODE_66;
            }
        }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            /* No OTN or HIGIG Support on 161/322/644 */
            if ((PHYMOD_INTF_MODES_HIGIG_GET(config)) || (PHYMOD_INTF_MODES_OTN_GET(config))) {
                    return PHYMOD_E_PARAM;
            }
            if (speed == SESTO_SPD_100G) {
                 /*IEEE MODE*/
                    *fal_pll_mode = SESTO_PLL_MODE_160;
                    *mer_pll_mode = SESTO_PLL_MODE_64;
            } else {
                    *fal_pll_mode = SESTO_PLL_MODE_128;
                    *mer_pll_mode = SESTO_PLL_MODE_64;
	        }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz: 
        case phymodRefClk698Mhz:
            if (speed == SESTO_SPD_100G) {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = SESTO_PLL_MODE_160;
                    *mer_pll_mode = SESTO_PLL_MODE_64;
                } else {
                    return PHYMOD_E_PARAM;
                }
            } else {
                if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                    *fal_pll_mode = SESTO_PLL_MODE_128;
                    *mer_pll_mode = SESTO_PLL_MODE_64;
                } else {
                    return PHYMOD_E_PARAM;
                }
            }
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

/** Map Merlin PLL Division
 * @param PLL Mode
 * @return Merlin PLL mode mapped value
 */
int _sesto_map_mer_pll_div(SESTO_PLL_MODE_E pll_mode) 
{
    switch (pll_mode) {
        case SESTO_PLL_MODE_64:
            return 1;
        case SESTO_PLL_MODE_66:
            return 2;
        case SESTO_PLL_MODE_70:
            return 3;
        default:
	        PHYMOD_DEBUG_VERBOSE(("PLLMODE %d not supported\n", pll_mode));
            return PHYMOD_E_PARAM; 
    }
}

int _sesto_config_pll_div(const phymod_access_t *pa, const phymod_phy_inf_config_t *config)
{
    uint16_t ip = 0, merlin_pll_mode = 0, falcon_pll_mode = 0;
    phymod_ref_clk_t ref_clk = config->ref_clock;
     uint16_t lock = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    int rv = PHYMOD_E_NONE;

    /*Program PLL div of all dies*/
    /* Set pll_mode */
    SESTO_IF_ERR_RETURN(
       _sesto_get_pll_modes(ref_clk, config, &falcon_pll_mode, &merlin_pll_mode));
    for (ip = SESTO_MERLIN_CORE; ip <= SESTO_FALCON_CORE; ip++) {
        PHYMOD_DEBUG_VERBOSE(("%s::  IP:%s Max_lane:%d lanemask:0x%x\n", 
                __func__, (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", 1, 0x1));
        SESTO_IF_ERR_RETURN(
          _sesto_set_slice_reg(pa, SESTO_SLICE_UNICAST, ip, SESTO_DEV_PMA_PMD, 0, 0));
        if (ip == SESTO_FALCON_CORE) {
                /* write 0 to core_dp_s_rstb while pll config*/
                wrc_falcon_furia_sesto_core_dp_s_rstb(0);
                /* default values  for falcon pll */ 
                wrc_falcon_furia_sesto_ams_pll_vco2_15g(0x0);
                wrc_falcon_furia_sesto_ams_pll_force_kvh_bw(0);
                wrc_falcon_furia_sesto_ams_pll_kvh_force(0);
                wrc_falcon_furia_sesto_ams_pll_vco_indicator(0);

                /* Set PLL div */
                switch (falcon_pll_mode) {
                    case SESTO_PLL_MODE_160:
                        if (PHYMOD_INTF_MODES_OTN_GET(config)) {
                            SESTO_IF_ERR_RETURN(
                             falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_160x_refc174));
                        } else {
                            SESTO_IF_ERR_RETURN(
                             falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_160x)); 
                        }
                    break;
                    case SESTO_PLL_MODE_132: 
                        SESTO_IF_ERR_RETURN(
                         falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_132x)); 
                    break;
                    case SESTO_PLL_MODE_128: 
                        SESTO_IF_ERR_RETURN(
                         falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_128x)); 
                    break;
                    case SESTO_PLL_MODE_140: 
                        SESTO_IF_ERR_RETURN(
                         falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_140x));
                    break;
                    case SESTO_PLL_MODE_165: 
                        SESTO_IF_ERR_RETURN(
                         falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_165x));
                    break;
                    case SESTO_PLL_MODE_175: 
                        SESTO_IF_ERR_RETURN(
                         falcon_furia_sesto_configure_pll(pa, FALCON_FURIA_pll_div_175x));
                    break;
                }
                /* write 1 to core_dp_s_rstb after pll config*/
                wrc_falcon_furia_sesto_core_dp_s_rstb(1);
        } else {
            /* write 0 to core_dp_s_rstb while pll config*/
            wrc_core_dp_s_rstb(0);
            if (_sesto_map_mer_pll_div(merlin_pll_mode) == 1) {
                /*-- PLL Mode 64*/
                SESTO_IF_ERR_RETURN(
                     merlin_sesto_configure_pll(pa, MERLIN_SESTO_pll_10p3125GHz_161p132MHz));
            } else if (_sesto_map_mer_pll_div(merlin_pll_mode) == 0x2) {
                /* PLL Mode 66*/
                SESTO_IF_ERR_RETURN(
                     merlin_sesto_configure_pll(pa, MERLIN_SESTO_pll_10p3125GHz_156p25MHz));
            } else if (_sesto_map_mer_pll_div(merlin_pll_mode) == 0x3) {
                /* PLL Mode 70*/
                SESTO_IF_ERR_RETURN(
                     merlin_sesto_configure_pll(pa, MERLIN_SESTO_pll_10p9375GHz_156p25MHz));
            } else {
                return PHYMOD_E_PARAM;
            }
            /* write 1 to core_dp_s_rstb after pll config*/
            wrc_core_dp_s_rstb(1);
        }
    }

    /*check for the pll lock and get the lanes out of reset*/
    do {
        if (ip == SESTO_FALCON_CORE) {
            SESTO_CHIP_FIELD_READ(pa, F25G_PLL_CAL_COM_CTL_STATUS_0, pll_lock, lock);
        } else {
            SESTO_CHIP_FIELD_READ(pa, M10G_PLL_CAL_PLL_CALSTS_0, pll_lock, lock);
        }
        PHYMOD_DEBUG_VERBOSE(("PLL LOCK STS:%d\n", lock));
        retry_cnt--;
    } while((lock != 1) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:PLL NOT LOCKED")));
        return PHYMOD_E_FAIL;
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}

int _sesto_core_dp_rstb(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t config)
{
    uint16_t lane = 0, ip = 0, lock = 0;
    uint16_t max_lane = 0, lane_mask=0;
    uint16_t cast_type = 0, mcast_val=0;
    uint16_t retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    SESTO_GET_IP(phy, config, ip);

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0x3ff;
    max_lane = (ip == SESTO_FALCON_CORE) ? SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    SESTO_GET_CORE_SLICE_INFO(phy, ip, config, &cast_type, &mcast_val);
    PHYMOD_DEBUG_VERBOSE(("%s:  IP:%s Max_lane:%d lanemask:0x%x cast_type:%d\n", 
                __func__, (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask,
                 cast_type));

    if (cast_type != SESTO_CAST_INVALID) {
        for (lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, cast_type, ip,
                              SESTO_DEV_PMA_PMD, mcast_val, lane));
                if (ip == SESTO_FALCON_CORE) {
                    SESTO_IF_ERR_RETURN(
                        wrc_falcon_furia_sesto_core_dp_s_rstb(1)); 
                } else {
                    SESTO_IF_ERR_RETURN(
                       wrc_core_dp_s_rstb(1)); 
                }
                if (SESTO_IS_MULTI_BROAD_CAST_SET(cast_type)) {
                    break;
                }
            }
        }
    }

    /*check for the pll lock and get the lanes out of reset*/
    do {
        if (ip == SESTO_FALCON_CORE) {
            SESTO_CHIP_FIELD_READ(pa, F25G_PLL_CAL_COM_CTL_STATUS_0, pll_lock, lock);
        } else {
            SESTO_CHIP_FIELD_READ(pa, M10G_PLL_CAL_PLL_CALSTS_0, pll_lock, lock);
        }
        PHYMOD_DEBUG_VERBOSE(("PLL LOCK STS:%d\n", lock));
        retry_cnt--;
    } while((lock != 1) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:PLL NOT LOCKED")));
        return PHYMOD_E_FAIL;
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}

int _sesto_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    SES_GEN_CNTRLS_GEN_CONTROL1_TYPE_T gen_ctrl;
    FALCON_IF_COMMON_CONTROL_TYPE_T  fal_cmn_ctrl;
    MERLIN_IF_COMMON_CONTROL_TYPE_T  mer_cmn_ctrl;
    int rv = PHYMOD_E_NONE;

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL1_ADR, gen_ctrl.data);
    if (reset_mode == phymodResetModeHard) {
        gen_ctrl.fields.resetb = 0;
        gen_ctrl.fields.reg_rstb = 0;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL1_ADR, gen_ctrl.data);

        /* -- Resetting the Cores*/
        READ_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);
        fal_cmn_ctrl.fields.pmd_por_h_rstb_frcval = 0;
        fal_cmn_ctrl.fields.pmd_por_h_rstb_frc = 1;
        WRITE_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);

        READ_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);
        mer_cmn_ctrl.fields.pmd_por_h_rstb_frcval = 0;
        mer_cmn_ctrl.fields.pmd_por_h_rstb_frc = 1;
        WRITE_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);

        /* -- Releasing the Core resets*/
        SESTO_CHIP_FIELD_WRITE(pa, FALCON_IF_COMMON_CONTROL, pmd_por_h_rstb_frc,0);
        SESTO_CHIP_FIELD_WRITE(pa, MERLIN_IF_COMMON_CONTROL, pmd_por_h_rstb_frc,0);
        PHYMOD_USLEEP(100);
    } else {
        gen_ctrl.fields.reg_rstb = 0;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL1_ADR, gen_ctrl.data);

        /* Reset the Merlin */
        SESTO_IF_ERR_RETURN(
        _sesto_set_slice_reg (pa, SESTO_SLICE_BROADCAST,
                           SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD,
                           0, 0));
        SESTO_CHIP_FIELD_WRITE(pa, M10G_CKRST_CTRL_LANE_CLK_RESET_N_POWERDOWN_CONTROL,
                                   ln_s_rstb, 0);
        PHYMOD_USLEEP(100);
        SESTO_CHIP_FIELD_WRITE(pa, M10G_CKRST_CTRL_LANE_CLK_RESET_N_POWERDOWN_CONTROL,
                                   ln_s_rstb, 1);
        /* Reset the Falcon */
        SESTO_IF_ERR_RETURN(
        _sesto_set_slice_reg (pa, SESTO_SLICE_BROADCAST,
                           SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD,
                           0, 0));
        SESTO_CHIP_FIELD_WRITE(pa, F25G_CKRST_CTRL_RPTR_LN_S_RSTB_CONTROL,
                                   ln_s_rstb, 0);
        PHYMOD_USLEEP(100);
        SESTO_CHIP_FIELD_WRITE(pa, F25G_CKRST_CTRL_RPTR_LN_S_RSTB_CONTROL,
                                   ln_s_rstb, 1);
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}

int _sesto_pcs_link_monitor_enable_set(const phymod_phy_access_t *phy, uint16_t en_dis)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0;
    uint16_t reg_addr = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_FMON0_MAIN_CONTROL_TYPE_T fmon_main_ctrl; 
    DP_MMON0_MAIN_CONTROL_TYPE_T mmon_main_ctrl; 

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_MEMSET(&fmon_main_ctrl, 0, sizeof(DP_FMON0_MAIN_CONTROL_TYPE_T));
    PHYMOD_MEMSET(&mmon_main_ctrl, 0, sizeof(DP_MMON0_MAIN_CONTROL_TYPE_T));
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    SESTO_GET_IP(phy, config, ip);

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0x3ff;
    max_lane = (ip == SESTO_FALCON_CORE) ? SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s :: IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));
    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            if (ip == SESTO_FALCON_CORE) {
                reg_addr = DP_FMON0_MAIN_CONTROL_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, fmon_main_ctrl.data);
                fmon_main_ctrl.fields.fmon0_main_ctrl &= ~(0x1);
                fmon_main_ctrl.fields.fmon0_main_ctrl |= (en_dis ? 0x1 : 0);
                WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, fmon_main_ctrl.data);
                PHYMOD_DEBUG_VERBOSE(("Falcon Link monitor for Lane:%d Data:0x%x\n",
                            lane, fmon_main_ctrl.data));
            } else {
                reg_addr = DP_MMON0_MAIN_CONTROL_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, mmon_main_ctrl.data);
                mmon_main_ctrl.fields.mmon0_main_ctrl &= ~(0x1);
                mmon_main_ctrl.fields.mmon0_main_ctrl |= (en_dis ? 0x1 : 0);
                WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, mmon_main_ctrl.data);
                PHYMOD_DEBUG_VERBOSE(("Merlin Link monitor for Lane:%d Data:0x%x\n",
                            lane, mmon_main_ctrl.data));
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);
    return rv;
}

int _sesto_pcs_link_monitor_enable_get(const phymod_phy_access_t *phy, uint32_t *get_pcs)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0;
    uint16_t reg_addr = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_FMON0_MAIN_CONTROL_TYPE_T fmon_main_ctrl; 
    DP_MMON0_MAIN_CONTROL_TYPE_T mmon_main_ctrl; 
    
    *get_pcs = 0xffff;
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    PHYMOD_MEMSET(&fmon_main_ctrl, 0, sizeof(DP_FMON0_MAIN_CONTROL_TYPE_T));
    PHYMOD_MEMSET(&mmon_main_ctrl, 0, sizeof(DP_MMON0_MAIN_CONTROL_TYPE_T));
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    SESTO_GET_IP(phy, config, ip);

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0x3ff;
    max_lane = (ip == SESTO_FALCON_CORE) ? SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s :: IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));
    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
            if (ip == SESTO_FALCON_CORE) {
                reg_addr = DP_FMON0_MAIN_CONTROL_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, fmon_main_ctrl.data);
                *get_pcs &= fmon_main_ctrl.fields.fmon0_main_ctrl & (0x1);
                PHYMOD_DEBUG_VERBOSE(("Falcon Get Link monitor for Lane:%d Data:0x%x\n",
                            lane, *get_pcs));
            } else {
                reg_addr = DP_MMON0_MAIN_CONTROL_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, mmon_main_ctrl.data);
                *get_pcs &= mmon_main_ctrl.fields.mmon0_main_ctrl & (0x1);
                PHYMOD_DEBUG_VERBOSE(("Merlin Get Link monitor for Lane:%d Data:0x%x\n",
                            lane, *get_pcs));
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_get_pcs_link_status(const phymod_phy_access_t *phy, uint32_t *link_sts, uint32_t* lock_lost_lh, uint32_t* error_count)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0, count_msb = 0;
    uint16_t reg_addr = 0;
    uint32_t err_cnt = 0, get_pcs = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_FMON0_MAIN_STATUS_TYPE_T fmon_main_sts;
    DP_MMON0_MAIN_STATUS_TYPE_T mmon_main_sts;
    DP_FMON0_LIVE_STATUS_TYPE_T fmon_live_link;
    DP_MMON0_LIVE_STATUS_TYPE_T mmon_live_link;

    *link_sts = 0xffff;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    SESTO_IF_ERR_RETURN(
         _sesto_pcs_link_monitor_enable_get(phy, &get_pcs));

    if (get_pcs) {
        config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
        SESTO_IF_ERR_RETURN(
           _sesto_phy_interface_config_get(phy, 0, &config));
    
        SESTO_GET_IP(phy, config, ip);
    
        lane_mask = (pa->lane_mask) ? pa->lane_mask : 0x3ff;
        max_lane = (ip == SESTO_FALCON_CORE) ? SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;
    
        PHYMOD_DEBUG_VERBOSE(("IP:%s Max_lane:%d lanemask:0x%x\n", 
                    (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

        PHYMOD_DEBUG_VERBOSE(("Getting PCS Status\n"));
        for (lane = 0; lane < max_lane; lane ++) {
            if ((lane_mask & (1 << lane))) {
                if (ip == SESTO_FALCON_CORE) {
                    reg_addr = (DP_FMON0_LIVE_STATUS_ADR + lane);
                    READ_SESTO_PMA_PMD_REG(pa, reg_addr, fmon_live_link.data);
                    if (fmon_live_link.fields.fmon0_live_stat & 1) { 
                        *link_sts &= 1;
                        /* PCS lock, Now check Loss of lock*/
                        reg_addr = DP_FMON0_MAIN_STATUS_ADR + lane;
                        READ_SESTO_PMA_PMD_REG(pa, reg_addr, fmon_main_sts.data);
                        if (!(fmon_main_sts.fields.fmon0_main_stat & 0x4)) {
                            /* Loss of link : read error cntr*/
                           reg_addr = (DP_FMON0_CL49_BER_STATUS_ADR + lane);
                           READ_SESTO_PMA_PMD_REG(pa, reg_addr, err_cnt);
                           err_cnt &= 0x7F;
                           err_cnt >>= 1;
                           reg_addr = (DP_FMON0_BER_COUNT_MSB_ADR + lane);
                           READ_SESTO_PMA_PMD_REG(pa, reg_addr, count_msb);
                           err_cnt |= (count_msb) << 6;
                           *link_sts &= 0;
                           PHYMOD_DEBUG_VERBOSE(("Lane:%d Loss of lock: 1 Error Cnt:0x%x\n", 
                                       lane, err_cnt));
                        }
                    } else {
                        *link_sts &= 0;
                    }
                } else {
                    reg_addr = (DP_MMON0_LIVE_STATUS_ADR + lane);
                    READ_SESTO_PMA_PMD_REG(pa, reg_addr, mmon_live_link.data);
                    if (mmon_live_link.fields.mmon0_live_stat & 1) { 
                        *link_sts &= 1;
                        /* PCS lock, Now check Loss of lock*/
                        reg_addr = (DP_MMON0_MAIN_STATUS_ADR + lane);
                        READ_SESTO_PMA_PMD_REG(pa, reg_addr, mmon_main_sts.data);
                        if (!(mmon_main_sts.fields.mmon0_main_stat & 0x4)) {
                            /* Loss of link : read error cntr*/
                           reg_addr = (DP_MMON0_CL49_BER_STATUS_ADR + lane);
                           READ_SESTO_PMA_PMD_REG(pa, reg_addr, err_cnt);
                           err_cnt &= 0x7F;
                           err_cnt >>= 1;
                           reg_addr = (DP_MMON0_BER_COUNT_MSB_ADR + lane);
                           READ_SESTO_PMA_PMD_REG(pa, reg_addr, count_msb);
                           err_cnt |= (count_msb) << 6;
                           *link_sts &= 0;
                           PHYMOD_DEBUG_VERBOSE(("Lane:%d Loss of lock: 1 Error Cnt:0x%x\n", 
                                       lane, err_cnt));
                        }
                    } else {
                        *link_sts &= 0;
                    }
                }
            }
        }
    } else {
        PHYMOD_DEBUG_VERBOSE(("PCS MON not Enabled, Reading PMD Status\n"));
        SESTO_IF_ERR_RETURN(
                 _sesto_rx_pmd_lock_get(phy, link_sts));
    }
ERR:
    if (get_pcs) {
        PHYMOD_FREE(config.device_aux_modes);
    }

    return rv;
}
int _sesto_merlin_falcon_lane_map_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *cfg,
                                     uint32_t* mer_lane_map, uint32_t* fal_lane_map)
{
    phymod_phy_inf_config_t config;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    const phymod_access_t *pa = &phy->access;

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0x3ff;
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    SESTO_GET_IP(phy, config, core);
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d\n", __func__, core));

    if (config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) {
        *mer_lane_map = 0x3FF;
        *fal_lane_map = 0xF;
    } else if (core == SESTO_MERLIN_CORE) {
        if (config.data_rate == SESTO_SPD_40G || config.data_rate == SESTO_SPD_42G) {
            if ((aux_mode->pass_thru) && (!aux_mode->BCM84793_capablity)) {
                *mer_lane_map = 0x33;
                *fal_lane_map = 0xF;        
            } else if (aux_mode->BCM84793_capablity) {
                *mer_lane_map = 0xF;
                *fal_lane_map = 0xF;
            } 
            else { 
                *mer_lane_map = lane_mask;
                if (lane_mask == 0xF)
                    *fal_lane_map = 0x3;
                else if(lane_mask == 0xF0)
                    *fal_lane_map = 0xC;
                else
                    *fal_lane_map = 0xF;
            }
        } else if (config.data_rate == SESTO_SPD_20G || config.data_rate == SESTO_SPD_21G) {
            if (aux_mode->pass_thru) {
                *mer_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *fal_lane_map = 0x3;
                else if(lane_mask == 0x30)
                    *fal_lane_map = 0xC;
                else
                    *fal_lane_map = 0xF;
            } else {
                *mer_lane_map = lane_mask; 
                if (lane_mask == 0x3)
                    *fal_lane_map = 0x1;
                else if(lane_mask == 0x30)
                    *fal_lane_map = 0x4;
                else
                    *fal_lane_map = 0x5;
            }      
        } else { /* 10G */
            if (aux_mode->BCM84793_capablity) {
                *mer_lane_map = lane_mask;
                *fal_lane_map = lane_mask;   
            } else {
                *mer_lane_map = lane_mask;
                if ((lane_mask == 0x1) || (lane_mask == 0x2))
                    *fal_lane_map = lane_mask;
                else if((lane_mask == 0x10) || (lane_mask == 0x20))
                    *fal_lane_map = (lane_mask >> 2);
                else
                    *fal_lane_map = 0xF;
            }
        }
    } else {
        if (config.data_rate == SESTO_SPD_40G || config.data_rate == SESTO_SPD_42G) {
            if ((aux_mode->pass_thru) && (!aux_mode->BCM84793_capablity)) {
                *mer_lane_map = 0x33;
                *fal_lane_map = 0xF;
            } else if (aux_mode->BCM84793_capablity) {
                *mer_lane_map = 0xF;
                *fal_lane_map = 0xF;
            }
            else {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *mer_lane_map = 0xF;
                else if(lane_mask == 0xC)
                    *mer_lane_map = 0xF0;
                else
                    *mer_lane_map = 0xFF;
            }
        } else if (config.data_rate == SESTO_SPD_20G || config.data_rate == SESTO_SPD_21G) {
            if (aux_mode->pass_thru) {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *mer_lane_map = 0x3;
                else if(lane_mask == 0xC)
                    *mer_lane_map = 0x30;
                else
                    *mer_lane_map = 0x33;
            } else {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x1)
                    *mer_lane_map = 0x3;
                else if(lane_mask == 0x4)
                    *mer_lane_map = 0x30;
                else
                    *mer_lane_map = 0x33;
            }
        } else { /* 10G */
            if (aux_mode->BCM84793_capablity) {
                *mer_lane_map = lane_mask;
                *fal_lane_map = lane_mask;
            } else {
                *fal_lane_map = lane_mask;
                if ((lane_mask == 0x1) || (lane_mask == 0x2))
                    *mer_lane_map = lane_mask;
                else if((lane_mask == 0x4) || (lane_mask == 0x8))
                    *mer_lane_map = (lane_mask << 2);
                else
                    *mer_lane_map = 0x33;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int _sesto_merlin_lpbk_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int rv = PHYMOD_E_NONE;

    switch (loopback) {
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD:
            /* Merlin Digital loopback get */
            SESTO_CHIP_FIELD_READ(pa, M10G_TLB_RX_DIG_LPBK_CONFIG,
                                      dig_lpbk_en, (*enable)); 
        break;
        case phymodLoopbackRemotePMD:  /* Merlin Remote loopback get */
            SESTO_CHIP_FIELD_READ(pa, M10G_TLB_TX_RMT_LPBK_CONFIG,
                                      rmt_lpbk_en, (*enable)); 
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        default:
        break;
    }

ERR:
    return rv;
}

int _sesto_falcon_lpbk_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    int rv = PHYMOD_E_NONE;

    switch (loopback) {
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD:
            /* Falcon Digital loopback get */
            SESTO_CHIP_FIELD_READ(pa, F25G_TLB_RX_DIG_LPBK_CONFIG,
                                      dig_lpbk_en, (*enable)); 
        break;
        case phymodLoopbackRemotePMD: /* Falcon Remote loopback get */
            SESTO_CHIP_FIELD_READ(pa, F25G_TLB_TX_RMT_LPBK_CONFIG,
                                      rmt_lpbk_en, (*enable)); 
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        default:
        break;
    }

ERR:
    return rv;
}

int _sesto_loopback_get(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    *enable = 0; 
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON core loopback set */
                PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));
                SESTO_IF_ERR_RETURN(
                  _sesto_falcon_lpbk_get(pa, loopback, enable));
                break;
            } else {
                /* MERLIN core loopback get */
                PHYMOD_DEBUG_VERBOSE(("Merlin loopback get\n"));
                SESTO_IF_ERR_RETURN(
                  _sesto_merlin_lpbk_get(pa, loopback, enable));
                break;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_falcon_lpbk_set(const phymod_phy_access_t* phy, const phymod_phy_inf_config_t *config,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t lane = 0, data = 0;
    uint32_t mer_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(
        _sesto_merlin_falcon_lane_map_get(phy, config, &mer_lane_mask, &fal_lane_mask));

    PHYMOD_DEBUG_VERBOSE(("Falcon loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
                if (lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_rmt_lpbk(pa, enable));
                    /* Work around for RMT LOOPBACK issue */
                    if (!enable) {
                        SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_tx_pi_loop_timing_src_sel(0x1));
                    }
                    SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_ln_dp_s_rstb(0));
                    PHYMOD_USLEEP(10);
                    SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_ln_dp_s_rstb(1));
                }
            }
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
        {
            for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
                if (mer_lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(merlin_sesto_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            }
            SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);

            for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
                if (fal_lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
                    /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
                    SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_prbs_chk_en_auto_mode((!enable)));
                    /* TX disable while enabling digital loopback if falcon is on line side PHY-1912 */
                    SESTO_CHIP_FIELD_READ(pa, DP_DECODED_MODE_STATUS0, decoded_mode_falcon_line, data);
                    if (data) {
                        SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_sdk_tx_disable(enable));
                    }
                    SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_ln_dp_s_rstb(0));
                    PHYMOD_USLEEP(10);
                    SESTO_IF_ERR_RETURN(wr_falcon_furia_sesto_ln_dp_s_rstb(1));
                }
            }
            PHYMOD_USLEEP(50);
        }
        break;
        default :
        break;
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}

int _sesto_merlin_lpbk_set(const phymod_phy_access_t* phy, const phymod_phy_inf_config_t *config,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    uint16_t lane = 0, data = 0;
    uint32_t mer_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
    uint16_t lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(
        _sesto_merlin_falcon_lane_map_get(phy, config, &mer_lane_mask, &fal_lane_mask));
    PHYMOD_DEBUG_VERBOSE(("Merlin loopback set\n"));

    switch (loopback) {
        case phymodLoopbackRemotePMD:  /* Merlin remote loopback set */
            for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
                if (lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(merlin_sesto_rmt_lpbk(pa, enable));
                    SESTO_IF_ERR_RETURN(wr_ln_dp_s_rstb(0));
                    PHYMOD_USLEEP(10);
                    SESTO_IF_ERR_RETURN(wr_ln_dp_s_rstb(1));

                } 
            }
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobal:
        case phymodLoopbackGlobalPMD: /* Merlin digital loopack set */
        {
            for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
                if (fal_lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
                }
            }
            SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);

            for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
                if (mer_lane_mask & (1 << lane)) {
                    SESTO_IF_ERR_RETURN(
                      _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                                SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD,
                                0, lane));
                    SESTO_IF_ERR_RETURN(merlin_sesto_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
                    /* Disable the prbs_chk_en_auto_mode while seting digital loopback */
                    SESTO_IF_ERR_RETURN(wr_prbs_chk_en_auto_mode((!enable)));
                    /* TX disable while enabling digital loopback if Merlin is on line side  PHY-2086 */
                    SESTO_CHIP_FIELD_READ(pa, DP_DECODED_MODE_STATUS0, decoded_mode_falcon_line, data);
                    if (!data) {
                        SESTO_IF_ERR_RETURN(wr_sdk_tx_disable(enable));
                    }
                    SESTO_IF_ERR_RETURN(wr_ln_dp_s_rstb(0));
                    PHYMOD_USLEEP(10);
                    SESTO_IF_ERR_RETURN(wr_ln_dp_s_rstb(1));
                }
            }
            PHYMOD_USLEEP(50);
        }
        break;
        default :
        break;
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}
int _sesto_if_ctrl_frc_tx_disable(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config,
                                  uint32_t enable)
{
    uint16_t lane = 0;
    uint32_t mer_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL1_LANE0_TYPE_T mer_ctrl_lane;
    FALCON_IF_CONTROL1_LANE0_TYPE_T fal_ctrl_lane;

    SESTO_IF_ERR_RETURN(
        _sesto_merlin_falcon_lane_map_get(phy, config, &mer_lane_mask, &fal_lane_mask));

    /* Falcon Force clear any Tx disables set by HW */
    for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
        if (fal_lane_mask & (1 << lane)) {
            READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL1_LANE0_ADR + lane),
                                       fal_ctrl_lane.data);
            fal_ctrl_lane.fields.pmd_tx_disable_0_frc = (enable ? 1 : 0);
            fal_ctrl_lane.fields.pmd_tx_disable_0_frcval = 0;
            WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL1_LANE0_ADR + lane),
                                       fal_ctrl_lane.data);
        }
    }
    /* Merlin Force clear any Tx disables set by HW */
    for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
        if (mer_lane_mask & (1 << lane)) {
            READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL1_LANE0_ADR + lane),
                                        mer_ctrl_lane.data);
            mer_ctrl_lane.fields.pmd_tx_disable_0_frc = (enable ? 1 : 0);
            mer_ctrl_lane.fields.pmd_tx_disable_0_frcval = 0;
            WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL1_LANE0_ADR + lane),
                                        mer_ctrl_lane.data);
        }
    }

ERR:
    return rv;
}

int _sesto_loopback_set(const phymod_phy_access_t *phy, phymod_loopback_mode_t loopback, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint32_t temp = 0;
    uint16_t data = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    SESTO_DEVICE_AUX_MODE_T aux_mode;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = &aux_mode;
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));

    PHYMOD_DEBUG_VERBOSE(("sesto looopback set\n"));
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);
    } while ((data != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
    }
    /*Work around for loopback call without enable loopback */
    SESTO_IF_ERR_RETURN(
        _sesto_loopback_get(phy, loopback, &temp));

    if(enable || temp) {
        if (ip == SESTO_FALCON_CORE) {
            /* FALCON core loopback set */
            SESTO_IF_ERR_RETURN(
                _sesto_falcon_lpbk_set(phy, &config, loopback, enable));
        } else {
            /* MERLIN core loopback set */
            SESTO_IF_ERR_RETURN(
                _sesto_merlin_lpbk_set(phy, &config, loopback, enable));
        }

        /* Set bit 13 in GPREG11 for loopback change */
        SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GPREG11, lpbk_change, 1);
        /* Clear any forces on Tx disables set by HW while disable */
        SESTO_IF_ERR_RETURN(_sesto_if_ctrl_frc_tx_disable(phy, &config, enable));

        /*  Set FW_ENABLE = 1 */
        SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, 1);
        do {
            SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);
        } while ((data != 0) && (retry_cnt --));
        if (retry_cnt == 0) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
                (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
        }
        if (enable) {
            /* Clear any forces on Tx disables set by HW */
            SESTO_IF_ERR_RETURN(_sesto_if_ctrl_frc_tx_disable(phy, &config, enable));

            if (ip == SESTO_FALCON_CORE) {
                /* FALCON core loopback set */
                SESTO_IF_ERR_RETURN(
                    _sesto_falcon_lpbk_set(phy, &config, loopback, enable));
            } else {
                /* MERLIN core loopback set */
                SESTO_IF_ERR_RETURN(
                    _sesto_merlin_lpbk_set(phy, &config, loopback, enable));
            }
        }
    }

ERR:
    return rv;
}

int _sesto_tx_rx_polarity_set(const phymod_phy_access_t *phy, uint32_t tx_polarity, uint32_t rx_polarity)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON polarity inversion */
                PHYMOD_DEBUG_VERBOSE(("Falcon polarity inversion set\n"));
                /* Write to Tx misc config register */
                SESTO_CHIP_FIELD_WRITE(pa, F25G_TLB_TX_TLB_TX_MISC_CONFIG,
                                           tx_pmd_dp_invert, tx_polarity);
                /* Write to Rx misc config register */
                SESTO_CHIP_FIELD_WRITE(pa, F25G_TLB_RX_TLB_RX_MISC_CONFIG,
                                           rx_pmd_dp_invert, rx_polarity);
            } else {
                /* MERLIN polarity inversion */
                PHYMOD_DEBUG_VERBOSE(("Merlin polarity inversion set\n"));
                /* Write to Tx misc config register */
                SESTO_CHIP_FIELD_WRITE(pa, M10G_TLB_TX_TLB_TX_MISC_CONFIG,
                                           tx_pmd_dp_invert, tx_polarity);
                /* Write to Rx misc config register */
                SESTO_CHIP_FIELD_WRITE(pa, M10G_TLB_RX_TLB_RX_MISC_CONFIG,
                                           rx_pmd_dp_invert, rx_polarity);
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_tx_rx_polarity_get(const phymod_phy_access_t *phy, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    *tx_polarity = 1;
    *rx_polarity = 1;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON polarity */
                PHYMOD_DEBUG_VERBOSE(("Falcon polarity get\n"));
                /* Read to Tx misc config register */
                SESTO_CHIP_FIELD_READ(pa, F25G_TLB_TX_TLB_TX_MISC_CONFIG,
                                          tx_pmd_dp_invert, (*tx_polarity));
                /* Read to Rx misc config register */
                SESTO_CHIP_FIELD_READ(pa, F25G_TLB_RX_TLB_RX_MISC_CONFIG,
                                          rx_pmd_dp_invert, (*rx_polarity));
                break;
            } else {
                /* MERLIN polarity */
                PHYMOD_DEBUG_VERBOSE(("Merlin polarity get\n"));
                /* Read to Tx misc config register */
                SESTO_CHIP_FIELD_READ(pa, M10G_TLB_TX_TLB_TX_MISC_CONFIG,
                                          tx_pmd_dp_invert, (*tx_polarity));
                /* Read to Rx misc config register */
                SESTO_CHIP_FIELD_READ(pa, M10G_TLB_RX_TLB_RX_MISC_CONFIG,
                                          rx_pmd_dp_invert, (*rx_polarity));
                break;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_tx_set(const phymod_phy_access_t *phy, const phymod_tx_t* tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON TXFIR */
                PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR set\n"));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_PRE, (int8_t)tx->pre));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_MAIN, (int8_t)tx->main));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_POST1, (int8_t)tx->post));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_POST2, (int8_t)tx->post2));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_POST3, (int8_t)tx->post3));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_write_tx_afe(pa, TX_AFE_AMP, (int8_t)tx->amp));
            } else {
                /* MERLIN TXFIR */
                PHYMOD_DEBUG_VERBOSE(("Merlin TXFIR set\n"));
                SESTO_IF_ERR_RETURN
                    (merlin_sesto_apply_txfir_cfg(pa, (int8_t)tx->pre, (int8_t)tx->main, (int8_t)tx->post, (int8_t)tx->post2));
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}    

int _sesto_tx_get(const phymod_phy_access_t *phy, phymod_tx_t* tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;
    int8_t value = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON TXFIR */
                PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR get\n"));
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_PRE, &value));
                     tx->pre = value;
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_MAIN, &value));
                    tx->main = value;
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_POST1, &value));
                    tx->post = value;
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_POST2, &value));
                    tx->post2 = value;
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_POST3, &value));
                      tx->post3 = value;
                SESTO_IF_ERR_RETURN
                    (falcon_furia_sesto_read_tx_afe(pa, TX_AFE_AMP, &value));
                     tx->amp = value;
                break;
            } else {
                /* MERLIN TXFIR */
                PHYMOD_DEBUG_VERBOSE(("Merlin TXFIR get\n"));
                SESTO_IF_ERR_RETURN
                    (merlin_sesto_read_tx_afe(pa, TX_AFE_PRE, &value));
                     tx->pre = value;
                SESTO_IF_ERR_RETURN
                    (merlin_sesto_read_tx_afe(pa, TX_AFE_MAIN, &value));
                    tx->main = value;
                SESTO_IF_ERR_RETURN
                    (merlin_sesto_read_tx_afe(pa, TX_AFE_POST1, &value));
                     tx->post = value;
                SESTO_IF_ERR_RETURN
                    (merlin_sesto_read_tx_afe(pa, TX_AFE_POST2, &value));
                    tx->post2 = value;
                break;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}    

int _sesto_rx_set(const phymod_phy_access_t* phy, const phymod_rx_t* rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint16_t indx = 0;
    uint8_t MAX_MERLIN_DFES_TAPS = 5;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;


    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON RXFIR set */
                PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR set\n"));

                /*params check*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
                    if(rx->dfe[indx].enable && (rx->num_of_dfe_taps > PHYMOD_NUM_DFE_TAPS)) {
                        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
                    }
                }

                /*vga set*/
                /* first stop the rx adaption */
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_stop_rx_adaptation(pa, 1));
                if (rx->vga.enable) {
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value));
                }
                /*dfe set*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
                    if(rx->dfe[indx].enable){
                        SESTO_IF_ERR_RETURN(falcon_furia_sesto_write_rx_afe(pa, RX_AFE_DFE1+indx, rx->dfe[indx].value));
                    }
                }

                /*peaking filter set*/
                if(rx->peaking_filter.enable){
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value));
                }

                if(rx->low_freq_peaking_filter.enable){
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
                }
            } else {
                /* MERLIN RXFIR set*/
                PHYMOD_DEBUG_VERBOSE(("Merlin RXFIR set\n"));

                /*params check*/
                for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
                    if(rx->dfe[indx].enable && (rx->num_of_dfe_taps > MAX_MERLIN_DFES_TAPS)) {
                        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("illegal number of DFEs to set")));
                    }
                }

                /* first stop the rx adaption */
                SESTO_IF_ERR_RETURN(merlin_sesto_stop_rx_adaptation(pa, 1));
                /*vga set*/
                if (rx->vga.enable) {
                    SESTO_IF_ERR_RETURN(merlin_sesto_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value));
                }
                /*dfe set*/
                /* PHYMOD_NUM_DFE_TAPS are 14 but MERLING core has 5 only DFEs */  
                for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
                    if(rx->dfe[indx].enable){
                        SESTO_IF_ERR_RETURN(merlin_sesto_write_rx_afe(pa, RX_AFE_DFE1+indx, rx->dfe[indx].value));
                    }
                }

                /*peaking filter set*/
                if(rx->peaking_filter.enable){
                    SESTO_IF_ERR_RETURN(merlin_sesto_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value));
                }

                if(rx->low_freq_peaking_filter.enable){
                    SESTO_IF_ERR_RETURN(merlin_sesto_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value));
                }
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}    
int _sesto_rx_adaptation_resume(const phymod_phy_access_t* phy)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;


    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                PHYMOD_DEBUG_VERBOSE(("Falcon RX RESUME set\n"));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_stop_rx_adaptation(pa, 0));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Merlin RX RESUME set\n"));
                SESTO_IF_ERR_RETURN(merlin_sesto_stop_rx_adaptation(pa, 0));
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_rx_get(const phymod_phy_access_t *phy, phymod_rx_t* rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t indx = 0;
    int8_t dfe = 0;
    int8_t vga = 0;
    int8_t pf = 0;
    int8_t low_freq_pf = 0;
    uint32_t rx_adaptation;
    uint16_t lane_mask = 0, max_lane = 0;
    int8_t MAX_MERLIN_DFES_TAPS = 5;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
             _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON RXFIR get */
                PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR get\n"));

                rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_stop_rx_adaptation(&phy->access, 1));

                /*vga get*/
                rx->num_of_dfe_taps = PHYMOD_NUM_DFE_TAPS;
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_read_rx_afe(pa, RX_AFE_VGA, &vga));
                rx->vga.value = vga; 
                rx->vga.enable = 1;

                /*dfe get*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_read_rx_afe(pa, (RX_AFE_DFE1+indx), &dfe));
                    rx->dfe[indx].value = dfe;
                    rx->dfe[indx].enable = 1;
                }

                /*peaking filter get*/
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_read_rx_afe(pa, RX_AFE_PF, &pf));
                rx->peaking_filter.value = pf;
                rx->peaking_filter.enable = 1;
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf));
                rx->low_freq_peaking_filter.value = low_freq_pf;
                rx->low_freq_peaking_filter.enable = 1;

                if (rx_adaptation) {
                    SESTO_IF_ERR_RETURN(falcon_furia_sesto_stop_rx_adaptation(&phy->access, 0));
                }

                break;
            } else {
                /* MERLIN RXFIR get */
                PHYMOD_DEBUG_VERBOSE(("Merlin RXFIR get\n"));

                rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
                SESTO_IF_ERR_RETURN(merlin_sesto_stop_rx_adaptation(&phy->access, 1));

                /*vga get*/
                /* PHYMOD_NUM_DFE_TAPS are 14 but MERLING has only 5 DFEs */  
                rx->num_of_dfe_taps = MAX_MERLIN_DFES_TAPS;
                SESTO_IF_ERR_RETURN(merlin_sesto_read_rx_afe(pa, RX_AFE_VGA, &vga));
                rx->vga.value = vga;
                rx->vga.enable = 1;

                /*dfe get*/
                for (indx = 0 ; indx < MAX_MERLIN_DFES_TAPS; indx++){
                    SESTO_IF_ERR_RETURN(merlin_sesto_read_rx_afe(pa, (RX_AFE_DFE1+indx), &dfe));
                    rx->dfe[indx].value = dfe;
                    rx->dfe[indx].enable = 1;
                }

                /*peaking filter get*/
                SESTO_IF_ERR_RETURN(merlin_sesto_read_rx_afe(pa, RX_AFE_PF, &pf));
                rx->peaking_filter.value = pf;
                 rx->peaking_filter.enable = 1;
                SESTO_IF_ERR_RETURN(merlin_sesto_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf));
                rx->low_freq_peaking_filter.value = low_freq_pf;
                rx->low_freq_peaking_filter.enable = 1;

                if (rx_adaptation) {
                    SESTO_IF_ERR_RETURN(merlin_sesto_stop_rx_adaptation(&phy->access, 0));
                }

                break;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_falcon_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    int rv = PHYMOD_E_NONE;
    F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_TYPE_T pwrdwn_ctrl;

    /* TX AFE Lane Reset */
    switch (reset->tx) {
        /* In Reset */
        case phymodResetDirectionIn:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);

        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            PHYMOD_USLEEP(10);
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        default:
        break;
    }
    /* RX AFE Lane Reset */
    switch (reset->rx) {
        /* In Reset */
        case phymodResetDirectionIn:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);

        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            PHYMOD_USLEEP(10);
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        default:
        break;
    }

ERR:
    return rv;
}

int _sesto_merlin_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    int rv = PHYMOD_E_NONE;
    M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_TYPE_T pwrdwn_ctrl;

    /* TX AFE Lane Reset */
    switch (reset->tx) {
        /* In Reset */
        case phymodResetDirectionIn:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);

        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            PHYMOD_USLEEP(10);
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_tx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        default:
        break;
    }

    /* RX AFE Lane Reset */
    switch (reset->rx) {
        /* In Reset */
        case phymodResetDirectionIn:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        /* Out Reset */
        case phymodResetDirectionOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 1;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            PHYMOD_USLEEP(10);
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc_val = 0;
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 1;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            /* -- Releasing forces -- */
            READ_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
            pwrdwn_ctrl.fields.afe_rx_reset_frc = 0;
            WRITE_SESTO_PMA_PMD_REG(pa,
                   M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL_ADR,
                   pwrdwn_ctrl.data);
        break;
        default:
        break;
    }

ERR:
    return rv;
}
int _sesto_phy_reset_set(const phymod_phy_access_t *phy, const phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset set\n"));
                SESTO_IF_ERR_RETURN(
                    _sesto_falcon_phy_reset_set(pa, reset));
            } else {
                /* MERLIN TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX/RX Reset set\n"));
                SESTO_IF_ERR_RETURN(
                    _sesto_merlin_phy_reset_set(pa, reset));
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_phy_reset_get(const phymod_phy_access_t *phy, phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t rst = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset get\n"));
                SESTO_CHIP_FIELD_READ(pa, 
                           F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL,
                           afe_tx_reset_frc_val, rst);
                if(rst == 0) {
                    /* Out of Reset */
                    reset->tx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->tx = phymodResetDirectionIn;
                }

                SESTO_CHIP_FIELD_READ(pa, 
                           F25G_CKRST_CTRL_RPTR_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL,
                           afe_rx_reset_frc_val, rst);
                if(rst == 0) {
                    /* Out of Reset */
                    reset->rx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->rx = phymodResetDirectionIn;
                }
            } else { 
                /* MERLIN TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Merlin TXF/RX Reset get\n"));
                SESTO_CHIP_FIELD_READ(pa,
                           M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL,
                           afe_tx_reset_frc_val, rst);
                if(rst == 0) {
                    /* Out of Reset */
                    reset->tx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->tx = phymodResetDirectionIn;
                }

                SESTO_CHIP_FIELD_READ(pa,
                           M10G_CKRST_CTRL_LANE_AFE_RESET_PWRDWN_CONTROL_CONTROL,
                           afe_rx_reset_frc_val, rst);
                if(rst == 0) {
                    /* Out of Reset */
                    reset->rx = phymodResetDirectionOut;
                } else {
                    /* In Reset */
                    reset->rx = phymodResetDirectionIn;
                }
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_tx_power_set(const phymod_phy_access_t *phy, uint32_t power_tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL2_LANE0_TYPE_T merlin_ctrl2_lane;
    FALCON_IF_CONTROL2_LANE0_TYPE_T falcon_ctrl2_lane;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    if (ip == SESTO_FALCON_CORE) {
        for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                /* FALCON TX Power Set */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX Power set\n"));
                switch (power_tx) {
                    case phymodPowerOff: /* Turn off power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerOn: /* Turn on power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerOffOn: /* Toggle power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        PHYMOD_USLEEP(10000);
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }
    } else {
        for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                /* MERLIN TX Power set */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX Power set\n"));
                switch (power_tx) {
                    case phymodPowerOff: /* Turn off power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerOn: /* Turn on power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerOffOn: /* Toggle power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        PHYMOD_USLEEP(10000);
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_rx_power_set(const phymod_phy_access_t *phy, uint32_t power_rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL2_LANE0_TYPE_T merlin_ctrl2_lane;
    FALCON_IF_CONTROL2_LANE0_TYPE_T falcon_ctrl2_lane;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));

    if (ip == SESTO_FALCON_CORE) {
        for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                /* FALCON RX Power Set */
                PHYMOD_DEBUG_VERBOSE(("Falcon RX Power set\n"));
                switch (power_rx) {
                    case phymodPowerOff: /* Turn off power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerOn: /* Turn on power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerOffOn: /* Toggle power */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        PHYMOD_USLEEP(10000);
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                        falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                    falcon_ctrl2_lane.data);
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }
    } else {
        for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                /* MERLIN RX Power set */
                PHYMOD_DEBUG_VERBOSE(("Merlin RX Power set\n"));
                switch (power_rx) {
                    case phymodPowerOff:  /* Turn off power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerOn:  /* Turn on power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerOffOn: /* Toggle power */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 1;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        PHYMOD_USLEEP(10000);
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 1;
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                        /* -- Releasing forces -- */
                        READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                        merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frc = 0;
                        WRITE_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                    merlin_ctrl2_lane.data);
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_tx_rx_power_get(const phymod_phy_access_t *phy, phymod_phy_power_t* power)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL2_LANE0_TYPE_T merlin_ctrl2_lane;
    FALCON_IF_CONTROL2_LANE0_TYPE_T falcon_ctrl2_lane;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON TX/RX Power Get */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Power get\n"));
                READ_SESTO_PMA_PMD_REG(pa, (FALCON_IF_CONTROL2_LANE0_ADR + lane),
                                                   falcon_ctrl2_lane.data);
                power->tx = !falcon_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval;
                power->rx = !falcon_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval;
            } else {
                /* MERLIN TX/RX Power Get */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX/RX Power get\n"));
                READ_SESTO_PMA_PMD_REG(pa, (MERLIN_IF_CONTROL2_LANE0_ADR + lane),
                                                   merlin_ctrl2_lane.data);
                power->tx = !merlin_ctrl2_lane.fields.pmd_ln_tx_h_pwrdn_0_frcval;
                power->rx = !merlin_ctrl2_lane.fields.pmd_ln_rx_h_pwrdn_0_frcval;
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_force_tx_training_set(const phymod_phy_access_t *phy, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint16_t data = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            if (ip == SESTO_FALCON_CORE) { /* Falcon TX Training set on GPREG13 Regs */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX Training set\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG13, tx_trng_20g_ln, data);
                enable ? (data |= (0x1 << lane)): (data &= (~(0x1 << lane)));
                SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GPREG13, tx_trng_20g_ln, data);
            } else { /* Merlin TX Training set on GPREG12 Regs */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX Training set\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG12, tx_trng_10g_ln, data);
                enable ? (data |= (0x1 << lane)): (data &= (~(0x1 << lane)));
                SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GPREG12, tx_trng_10g_ln, data);
            }
        }
    }

    /*  Set FW_ENABLE = 1 */
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, 1);
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);
        retry_cnt--;
    } while ((data != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Training set failed, micro controller is busy..")));
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_force_tx_training_get(const phymod_phy_access_t *phy, uint32_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint16_t training_en = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            if (ip == SESTO_FALCON_CORE) { /* Falcon TX Training get on GPREG13 Regs */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX Training get\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG13, tx_trng_20g_ln, training_en);
                *enable = ((training_en >> lane) & 0x1);
            } else { /* Merlin TX Training get on GPREG12 Regs */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX Training get\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG12, tx_trng_10g_ln, training_en);
                *enable = ((training_en >> lane) & 0x1);
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_force_tx_training_status_get(const phymod_phy_access_t *phy, phymod_cl72_status_t* status)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint16_t training_en = 0;
    uint16_t training_failure = 1;
    uint16_t trained = 1;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    status->enabled = 0x1;
    status->locked = 0x1;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                            ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) { /* Falcon TX Training status Reading from IEEE Regs */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX Training Status get\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG13, tx_trng_20g_ln, training_en);
                status->enabled &= ((training_en >> lane) & 0x1);
                SESTO_CHIP_FIELD_READ(pa, F25G_CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_STATUS_REGISTER_151,
                                          cl93n72_ieee_receiver_status, trained);
                SESTO_CHIP_FIELD_READ(pa, F25G_CL93N72_IEEE_TX_CL93N72IT_BASE_R_PMD_STATUS_REGISTER_151,
                                          cl93n72_ieee_training_failure, training_failure);
                status->locked &= ((!training_failure) && (trained));
            } else {  /* Merlin TX Training status Reading from IEEE Regs */
                PHYMOD_DEBUG_VERBOSE(("Merlin TX Training Status get\n"));
                SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_GPREG12, tx_trng_10g_ln, training_en);
                status->enabled &= ((training_en >> lane) & 0x1);
                SESTO_CHIP_FIELD_READ(pa, M10G_CL72_IEEE_TX_BASE_R_PMD_STATUS_REGISTER_151,
                                          receiver_status, trained);
                SESTO_CHIP_FIELD_READ(pa, M10G_CL72_IEEE_TX_BASE_R_PMD_STATUS_REGISTER_151,
                                          training_failure, training_failure);
                status->locked &= ((!training_failure) && (trained));
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_phy_status_dump(const phymod_phy_access_t *phy)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, intf_side = 0;
    uint16_t lane = 0, lane_mask = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);
    SESTO_GET_INTF_SIDE(phy, intf_side);

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("******* PHY status dump for PHY ID: 0x%x ********\n",pa->addr));
    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("**** PHY status dump for interface side:%d ****\n",intf_side));
    USR_PRINTF(("***********************************************\n"));
    if (ip == SESTO_FALCON_CORE) { /* Falcon core status dump */
        for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                            SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD, 0, lane));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_display_core_config(pa));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_display_core_state(pa));
                break;
            }
        }
    } else {  /* Merlin core status dump */
        for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                            SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD, 0, lane));
                SESTO_IF_ERR_RETURN(merlin_sesto_display_core_config(pa));
                SESTO_IF_ERR_RETURN(merlin_sesto_display_core_state(pa));
                break;
            }
        }
    }
    if (ip == SESTO_FALCON_CORE) { /* Falcon lanes status dump */
        for(lane = 0; lane < SESTO_MAX_FALCON_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                            SESTO_FALCON_CORE, SESTO_DEV_PMA_PMD,
                            0, lane));
                PHYMOD_DEBUG_VERBOSE(("Falcon Status dump\n"));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_display_lane_state_hdr(pa));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_display_lane_state(pa));
                SESTO_IF_ERR_RETURN(falcon_furia_sesto_display_lane_config(pa));
            }
        }
    } else {   /* Merlin lanes status dump */
        for(lane = 0; lane < SESTO_MAX_MERLIN_LANE; lane ++) {
            if (lane_mask & (1 << lane)) {
                SESTO_IF_ERR_RETURN(
                  _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                            SESTO_MERLIN_CORE, SESTO_DEV_PMA_PMD,
                            0, lane));
                PHYMOD_DEBUG_VERBOSE(("Merlin Status dump\n"));
                SESTO_IF_ERR_RETURN(merlin_sesto_display_lane_state_hdr(pa));
                SESTO_IF_ERR_RETURN(merlin_sesto_display_lane_state(pa));
                SESTO_IF_ERR_RETURN(merlin_sesto_display_lane_config(pa));
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_tx_enable_set(const phymod_phy_access_t *phy, int16_t enable) 
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, reg_addr = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL1_LANE0_TYPE_T mer_ctrl1;
    FALCON_IF_CONTROL1_LANE0_TYPE_T fal_ctrl1;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;
    
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__, 
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* TX squelch set with pmd_tx_disable_0_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            if (ip == SESTO_FALCON_CORE) {
                reg_addr = FALCON_IF_CONTROL1_LANE0_ADR + lane; 
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, fal_ctrl1.data);
                fal_ctrl1.fields.pmd_tx_disable_0_frc = 1;
                fal_ctrl1.fields.pmd_tx_disable_0_frcval = (!enable);
                WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, fal_ctrl1.data);
                /* -- Releasing forces -- */
                if (enable) {
                    READ_SESTO_PMA_PMD_REG(pa, reg_addr, fal_ctrl1.data);
                    fal_ctrl1.fields.pmd_tx_disable_0_frc = 0;
                    WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, fal_ctrl1.data);
                }
            } else {
                reg_addr = MERLIN_IF_CONTROL1_LANE0_ADR + lane; 
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, mer_ctrl1.data);
                mer_ctrl1.fields.pmd_tx_disable_0_frc = 1;
                mer_ctrl1.fields.pmd_tx_disable_0_frcval = (!enable);
                WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, mer_ctrl1.data);
                /* -- Releasing forces -- */
                if (enable) {
                    READ_SESTO_PMA_PMD_REG(pa, reg_addr, mer_ctrl1.data);
                    mer_ctrl1.fields.pmd_tx_disable_0_frc = 0;
                    WRITE_SESTO_PMA_PMD_REG(pa, reg_addr, mer_ctrl1.data);
                }
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_tx_enable_get(const phymod_phy_access_t *phy, int16_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0, reg_addr = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    MERLIN_IF_CONTROL1_LANE0_TYPE_T mer_ctrl1;
    FALCON_IF_CONTROL1_LANE0_TYPE_T fal_ctrl1;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* TX squelch get with pmd_tx_disable_0_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            if (ip == SESTO_FALCON_CORE) {
                reg_addr = FALCON_IF_CONTROL1_LANE0_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, fal_ctrl1.data);
                *enable = !fal_ctrl1.fields.pmd_tx_disable_0_frcval;
            } else {
                reg_addr = MERLIN_IF_CONTROL1_LANE0_ADR + lane;
                READ_SESTO_PMA_PMD_REG(pa, reg_addr, mer_ctrl1.data);
                *enable = !mer_ctrl1.fields.pmd_tx_disable_0_frcval;
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}


int _sesto_tx_dp_reset(const phymod_phy_access_t *phy)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_LANE_RESET_CONTROL0_TYPE_T dp_rst;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    
    SESTO_GET_IP(phy, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;
    
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__, 
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* Reset TX data path */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            READ_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            if (ip == SESTO_FALCON_CORE) {
                dp_rst.fields.demux_ilane_dp_rstb &= ~(1 << lane);
            } else {
                dp_rst.fields.mux_ilane_dp_rstb &= ~(1 << lane);
            }
            WRITE_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            PHYMOD_USLEEP(1000);
            READ_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            if (ip == SESTO_FALCON_CORE) {
                dp_rst.fields.demux_ilane_dp_rstb |= (1 << lane);
            } else {
                dp_rst.fields.mux_ilane_dp_rstb |= (1 << lane);
            }
            WRITE_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);

        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_rx_enable_set(const phymod_phy_access_t *phy, int16_t enable) 
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    M10G_SIGDET_SIGDET_CTRL_1_TYPE_T merlin_ctrl1_lane;
    F25G_SIGDET_SIGDET_CTRL_1_TYPE_T falcon_ctrl1_lane;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* RX squelch set with signal_detect_frc_val */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                READ_SESTO_PMA_PMD_REG(pa, F25G_SIGDET_SIGDET_CTRL_1_ADR, falcon_ctrl1_lane.data);
                falcon_ctrl1_lane.fields.signal_detect_frc = 1;
                falcon_ctrl1_lane.fields.signal_detect_frc_val = enable;
                WRITE_SESTO_PMA_PMD_REG(pa, F25G_SIGDET_SIGDET_CTRL_1_ADR, falcon_ctrl1_lane.data);
                /* -- Releasing forces -- */
                if (enable) {
                    READ_SESTO_PMA_PMD_REG(pa, F25G_SIGDET_SIGDET_CTRL_1_ADR, falcon_ctrl1_lane.data);
                    falcon_ctrl1_lane.fields.signal_detect_frc = 0;
                    WRITE_SESTO_PMA_PMD_REG(pa, F25G_SIGDET_SIGDET_CTRL_1_ADR, falcon_ctrl1_lane.data);
                }
            } else { 
                READ_SESTO_PMA_PMD_REG(pa, M10G_SIGDET_SIGDET_CTRL_1_ADR, merlin_ctrl1_lane.data);
                merlin_ctrl1_lane.fields.signal_detect_frc = 1;
                merlin_ctrl1_lane.fields.signal_detect_frc_val= enable;
                WRITE_SESTO_PMA_PMD_REG(pa, M10G_SIGDET_SIGDET_CTRL_1_ADR, merlin_ctrl1_lane.data);
                /* -- Releasing forces -- */
                if (enable) {
                    READ_SESTO_PMA_PMD_REG(pa, M10G_SIGDET_SIGDET_CTRL_1_ADR, merlin_ctrl1_lane.data);
                    merlin_ctrl1_lane.fields.signal_detect_frc = 0;
                    WRITE_SESTO_PMA_PMD_REG(pa, M10G_SIGDET_SIGDET_CTRL_1_ADR, merlin_ctrl1_lane.data);
                }
            }
        }
    }
ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_rx_enable_get(const phymod_phy_access_t *phy, int16_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    M10G_SIGDET_SIGDET_CTRL_1_TYPE_T merlin_ctrl1_lane;
    F25G_SIGDET_SIGDET_CTRL_1_TYPE_T falcon_ctrl1_lane;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* RX squelch get with signal_detect_frc_val */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (ip == SESTO_FALCON_CORE) {
                READ_SESTO_PMA_PMD_REG(pa, F25G_SIGDET_SIGDET_CTRL_1_ADR, falcon_ctrl1_lane.data);
                *enable = falcon_ctrl1_lane.fields.signal_detect_frc_val;
            } else {
                READ_SESTO_PMA_PMD_REG(pa, M10G_SIGDET_SIGDET_CTRL_1_ADR, merlin_ctrl1_lane.data);
                *enable = merlin_ctrl1_lane.fields.signal_detect_frc_val;
            }
        }
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_rx_dp_reset(const phymod_phy_access_t *phy)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_LANE_RESET_CONTROL0_TYPE_T dp_rst;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    
    SESTO_GET_IP(phy, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ? 
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;
    
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__, 
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    /* Reset RX data path */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            READ_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            if (ip == SESTO_FALCON_CORE) {
                dp_rst.fields.demux_ilane_dp_rstb &= ~(1 << lane);
            } else {
                dp_rst.fields.mux_ilane_dp_rstb &= ~(1 << lane);
            }
            WRITE_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            PHYMOD_USLEEP(1000);
            READ_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
            if (ip == SESTO_FALCON_CORE) {
                dp_rst.fields.demux_ilane_dp_rstb |= (1 << lane);
            } else {
                dp_rst.fields.mux_ilane_dp_rstb |= (1 << lane);
            }
            WRITE_SESTO_PMA_PMD_REG(pa, DP_LANE_RESET_CONTROL0_ADR, dp_rst.data);
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}


int _sesto_tx_lane_control_set(const phymod_phy_access_t *phy,  phymod_phy_tx_lane_control_t tx_control)
{
    int rv = PHYMOD_E_NONE;

    switch (tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
            return PHYMOD_E_UNAVAIL;
        case phymodTxReset: /* reset tx data path */
            SESTO_IF_ERR_RETURN(_sesto_tx_dp_reset(phy));
        break;
        case phymodTxSquelchOn: /* squelch tx */
            SESTO_IF_ERR_RETURN(_sesto_tx_enable_set(phy, SESTO_DISABLE));
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            SESTO_IF_ERR_RETURN(_sesto_tx_enable_set(phy, SESTO_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

ERR:
    return rv;
}
int _sesto_rx_lane_control_set(const phymod_phy_access_t *phy,  phymod_phy_rx_lane_control_t rx_control)
{
    int rv = PHYMOD_E_NONE;

    switch (rx_control) {
        case phymodRxReset: /* reset rx data path */
            SESTO_IF_ERR_RETURN(_sesto_rx_dp_reset(phy));
            break;
        case phymodRxSquelchOn: /* squelch rx */
            SESTO_IF_ERR_RETURN(_sesto_rx_enable_set(phy, SESTO_DISABLE));
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            SESTO_IF_ERR_RETURN(_sesto_rx_enable_set(phy, SESTO_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

ERR:
    return rv;
}

int _sesto_tx_lane_control_get(const phymod_phy_access_t *phy,  phymod_phy_tx_lane_control_t *tx_control)
{
    int16_t tx_ctrl = 0;
    int rv = PHYMOD_E_NONE;

    switch (*tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
        case phymodTxReset: /* reset tx data path */
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodTxSquelchOn: /* squelch tx */
            SESTO_IF_ERR_RETURN(_sesto_tx_enable_get(phy, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            SESTO_IF_ERR_RETURN(_sesto_tx_enable_get(phy, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        default:
            SESTO_IF_ERR_RETURN(_sesto_tx_enable_get(phy, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
    }

ERR:
    return rv;
}
int _sesto_rx_lane_control_get(const phymod_phy_access_t *phy,  phymod_phy_rx_lane_control_t *rx_control)
{
    int16_t rx_ctrl = 0;
    int rv = PHYMOD_E_NONE;

    switch (*rx_control) {
        case phymodRxReset: /* reset rx data path */
            return PHYMOD_E_UNAVAIL;
        case phymodRxSquelchOn: /* squelch rx */
            SESTO_IF_ERR_RETURN(_sesto_rx_enable_get(phy, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            SESTO_IF_ERR_RETURN(_sesto_rx_enable_get(phy, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        default:
            return PHYMOD_E_PARAM;
    }

ERR:
    return rv;
}
int _sesto_intr_phymod_intr(const phymod_phy_access_t *phy, uint32_t intr_type, uint16_t *intr)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN( _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    switch (intr_type) {
        case PHYMOD_INTR_LINK_EVENT:
            *intr = (ip == SESTO_FALCON_CORE) ? SESTO_INT_DEMUX_PMD_LOCK_LOST : SESTO_INT_MUX_PMD_LOCK_LOST;
        break;
        case PHYMOD_INTR_AUTONEG_EVENT:
            if (ip == SESTO_FALCON_CORE) {
                *intr = SESTO_INT_DEMUX_CL73_AN_COMPLETE;
            } else {
                if (lane_mask == 0xF) {                         /* 40G MUX P1 */
                    *intr = SESTO_INT_MUX_CL73_AN_COMPLETE_P1;
                } else if (lane_mask == 0xF0) {                 /* 40G MUX P1 */
                    *intr = SESTO_INT_MUX_CL73_AN_COMPLETE_P2;
                } else {                                        /* 40G PT based on AN Master lane */
                    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
                    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
                    if (((an_mst_sel.data & 0xF) >= 4) && ((an_mst_sel.data & 0xF) <= 5)) {
                         *intr = SESTO_INT_MUX_CL73_AN_COMPLETE_P2;
                    } else {
                         *intr = SESTO_INT_MUX_CL73_AN_COMPLETE_P1;
                    }
                }
            }
        break;
        case PHYMOD_INTR_PLL_EVENT:
            *intr = (ip == SESTO_FALCON_CORE) ? SESTO_INT_PLL_25G_LOCK_LOST : SESTO_INT_PLL_10G_LOCK_LOST;
        break;
        case PHYMOD_INTR_EMON_EVENT:
            *intr = (ip == SESTO_FALCON_CORE) ? SESTO_INT_DEMUX_PCS_MON_LOCK_LOST : SESTO_INT_MUX_PCS_MON_LOCK_LOST;
        break;
        default:
            return PHYMOD_E_NONE;
    }
ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int  _sesto_get_intr_reg(const phymod_phy_access_t *phy, uint32_t intr_type, uint16_t *bit_pos, uint16_t *int_grp)
{
    uint16_t intr = -1;
    int rv = PHYMOD_E_NONE;

    SESTO_IF_ERR_RETURN(_sesto_intr_phymod_intr(phy, intr_type, &intr));
    *int_grp = -1;

    /* Get the interrupt bit postion in the reg and interrupt group */
    switch (intr) {
        case SESTO_INT_M0_SLV_SYSRESET_REQ:          *bit_pos =  0;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_SYSRESET_REQ:          *bit_pos =  1;   *int_grp =  0; break;
        case SESTO_INT_M0_SLV_LOCKUP:                *bit_pos =  2;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_LOCKUP:                *bit_pos =  3;   *int_grp =  0; break;
        case SESTO_INT_M0_SLV_MISC_INTR:             *bit_pos =  4;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_MISC_INTR:             *bit_pos =  5;   *int_grp =  0; break;
        case SESTO_INT_M0_SLV_MSGOUT_INTR:           *bit_pos =  6;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_MSGOUT_INTR:           *bit_pos =  7;   *int_grp =  0; break;
        case SESTO_INT_M0_SLV_DED:                   *bit_pos =  8;   *int_grp =  0; break;
        case SESTO_INT_M0_SLV_SEC:                   *bit_pos =  9;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_DED:                   *bit_pos = 10;   *int_grp =  0; break;
        case SESTO_INT_M0_MST_SEC:                   *bit_pos = 11;   *int_grp =  0; break;
        case SESTO_INT_MODULE2_INTRB_LOW:            *bit_pos = 12;   *int_grp =  0; break;
        case SESTO_INT_MODULE2_INTRB_HIGH:           *bit_pos = 13;   *int_grp =  0; break;
        case SESTO_INT_MODULE1_INTRB_LOW:            *bit_pos = 14;   *int_grp =  0; break;
        case SESTO_INT_MODULE1_INTRB_HIGH:           *bit_pos = 15;   *int_grp =  0; break;
        case SESTO_INT_MUX_PMD_LOCK_LOST:            *bit_pos =  0;   *int_grp =  1; break;
        case SESTO_INT_MUX_PMD_LOCK_FOUND:           *bit_pos =  1;   *int_grp =  1; break;
        case SESTO_INT_MUX_SIGDET_LOST:              *bit_pos =  2;   *int_grp =  1; break;
        case SESTO_INT_MUX_SIGDET_FOUND:             *bit_pos =  3;   *int_grp =  1; break;
        case SESTO_INT_PLL_10G_LOCK_LOST:            *bit_pos =  4;   *int_grp =  1; break;
        case SESTO_INT_PLL_10G_LOCK_FOUND:           *bit_pos =  5;   *int_grp =  1; break;
        case SESTO_INT_MERLIN_PMI_ARB_WDOG_EXP:      *bit_pos =  6;   *int_grp =  1; break;
        case SESTO_INT_MERLIN_PMI_M0_WDOG_EXP:       *bit_pos =  7;   *int_grp =  1; break;
        case SESTO_INT_DEMUX_PMD_LOCK_LOST:          *bit_pos =  0;   *int_grp =  2; break;
        case SESTO_INT_DEMUX_PMD_LOCK_FOUND:         *bit_pos =  1;   *int_grp =  2; break;
        case SESTO_INT_DEMUX_SIGDET_LOST:            *bit_pos =  2;   *int_grp =  2; break;
        case SESTO_INT_DEMUX_SIGDET_FOUND:           *bit_pos =  3;   *int_grp =  2; break;
        case SESTO_INT_PLL_25G_LOCK_LOST:            *bit_pos =  4;   *int_grp =  2; break;
        case SESTO_INT_PLL_25G_LOCK_FOUND:           *bit_pos =  5;   *int_grp =  2; break;
        case SESTO_INT_FALCON_PMD_ARB_WDOG_EXP:      *bit_pos =  6;   *int_grp =  2; break;
        case SESTO_INT_FALCON_PMD_M0_WDOG_EXP:       *bit_pos =  7;   *int_grp =  2; break;
        case SESTO_INT_CL91_RX_ALIGN_LOST:           *bit_pos =  0;   *int_grp =  3; break;
        case SESTO_INT_CL91_RX_ALIGN_FOUND:          *bit_pos =  1;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_ONEG_INBAND_MSG_LOST:   *bit_pos =  2;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_ONEG_INBAND_MSG_FOUND:  *bit_pos =  3;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_PCS_MON_LOCK_LOST:      *bit_pos =  4;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_PCS_MON_LOCK_FOUND:     *bit_pos =  5;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_FIFOERR:                *bit_pos =  6;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_CL73_AN_RESTARTED:      *bit_pos =  7;   *int_grp =  3; break;
        case SESTO_INT_DEMUX_CL73_AN_COMPLETE:       *bit_pos =  8;   *int_grp =  3; break;
        case SESTO_INT_CL91_TX_ALIGN_LOST:           *bit_pos =  0;   *int_grp =  4; break;
        case SESTO_INT_CL91_TX_ALIGN_FOUND:          *bit_pos =  1;   *int_grp =  4; break;
        case SESTO_INT_MUX_ONEG_INBAND_MSG_LOST:     *bit_pos =  2;   *int_grp =  4; break;
        case SESTO_INT_MUX_ONEG_INBAND_MSG_FOUND:    *bit_pos =  3;   *int_grp =  4; break;
        case SESTO_INT_MUX_PCS_MON_LOCK_LOST:        *bit_pos =  4;   *int_grp =  4; break;
        case SESTO_INT_MUX_PCS_MON_LOCK_FOUND:       *bit_pos =  5;   *int_grp =  4; break;
        case SESTO_INT_MUX_FIFOERR:                  *bit_pos =  6;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_RESTARTED:        *bit_pos =  7;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_COMPLETE:         *bit_pos =  8;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_RESTARTED_P2:     *bit_pos =  9;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_RESTARTED_P1:     *bit_pos = 10;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_COMPLETE_P2:      *bit_pos = 11;   *int_grp =  4; break;
        case SESTO_INT_MUX_CL73_AN_COMPLETE_P1:      *bit_pos = 12;   *int_grp =  4; break;
        default:
            return PHYMOD_E_PARAM;
    }

ERR:
    return rv;
}
int _sesto_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    int rv = PHYMOD_E_NONE;
    SES_CTRL_M0_EIER_TYPE_T    m0_eier;
    SES_CTRL_MM_EIER_TYPE_T    mm_eier;
    SES_CTRL_DF_EIER_TYPE_T    df_eier;
    SES_CTRL_DEMUX_EIER_TYPE_T demux_eier;
    SES_CTRL_MUX_EIER_TYPE_T   mux_eier;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eier, 0, sizeof(SES_CTRL_M0_EIER_TYPE_T));
    PHYMOD_MEMSET(&mm_eier, 0, sizeof(SES_CTRL_MM_EIER_TYPE_T));
    PHYMOD_MEMSET(&df_eier, 0, sizeof(SES_CTRL_DF_EIER_TYPE_T));
    PHYMOD_MEMSET(&demux_eier, 0, sizeof(SES_CTRL_DEMUX_EIER_TYPE_T));
    PHYMOD_MEMSET(&mux_eier, 0, sizeof(SES_CTRL_MUX_EIER_TYPE_T));
    

    /* Get the interrupt bit postion in the reg and interrupt group */
    _sesto_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);
    intr_mask = enable ? (0x1 << bit_pos) : (~(0x1 << bit_pos));

    /* Interrupt enable set on specified interrupt group */
    switch(intr_grp) {
        case SESTO_M0_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_M0_EIER_ADR, m0_eier.data);
           m0_eier.data =  enable ? (intr_mask | m0_eier.data) : (intr_mask & m0_eier.data);
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_M0_EIER_ADR, m0_eier.data);
        break;
        case SESTO_MM_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MM_EIER_ADR, mm_eier.data);
           mm_eier.data =  enable ? (intr_mask | mm_eier.data) : (intr_mask & mm_eier.data);
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_MM_EIER_ADR, mm_eier.data);
        break;
        case SESTO_DF_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DF_EIER_ADR, df_eier.data);
           df_eier.data =  enable ? (intr_mask | df_eier.data) : (intr_mask & df_eier.data);
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_DF_EIER_ADR, df_eier.data);
        break;
        case SESTO_DEMUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DEMUX_EIER_ADR, demux_eier.data);
           demux_eier.data =  enable ? (intr_mask | demux_eier.data) : (intr_mask & demux_eier.data);
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_DEMUX_EIER_ADR, demux_eier.data);
        break;
        case SESTO_MUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MUX_EIER_ADR, mux_eier.data);
           mux_eier.data =  enable ? (intr_mask | mux_eier.data) : (intr_mask & mux_eier.data);
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_MUX_EIER_ADR, mux_eier.data);
        break;
        default:
            return PHYMOD_E_NONE;
    }

ERR:
    return rv;
}
int _sesto_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t *enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    int rv = PHYMOD_E_NONE;
    SES_CTRL_M0_EIER_TYPE_T    m0_eier;
    SES_CTRL_MM_EIER_TYPE_T    mm_eier;
    SES_CTRL_DF_EIER_TYPE_T    df_eier;
    SES_CTRL_DEMUX_EIER_TYPE_T demux_eier;
    SES_CTRL_MUX_EIER_TYPE_T   mux_eier;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eier, 0, sizeof(SES_CTRL_M0_EIER_TYPE_T));
    PHYMOD_MEMSET(&mm_eier, 0, sizeof(SES_CTRL_MM_EIER_TYPE_T));
    PHYMOD_MEMSET(&df_eier, 0, sizeof(SES_CTRL_DF_EIER_TYPE_T));
    PHYMOD_MEMSET(&demux_eier, 0, sizeof(SES_CTRL_DEMUX_EIER_TYPE_T));
    PHYMOD_MEMSET(&mux_eier, 0, sizeof(SES_CTRL_MUX_EIER_TYPE_T));
    *enable = 0;

    /* Get the interrupt bit postion in the reg and interrupt group */
    _sesto_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);

    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case SESTO_M0_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_M0_EIER_ADR, m0_eier.data);
           data = m0_eier.data;
        break;
        case SESTO_MM_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MM_EIER_ADR, mm_eier.data);
           data = mm_eier.data;
        break;
        case SESTO_DF_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DF_EIER_ADR, df_eier.data);
           data = df_eier.data;
        break;
        case SESTO_DEMUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DEMUX_EIER_ADR, demux_eier.data);
           data = demux_eier.data;
        break;
        case SESTO_MUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MUX_EIER_ADR, mux_eier.data);
           data = mux_eier.data;
        break;
        default:
            return PHYMOD_E_NONE;
    }
    *enable = (data & (0x1 << bit_pos)) ? 1 : 0;

ERR:
    return rv;
}

int _sesto_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t *intr_status)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    int rv = PHYMOD_E_NONE;
    SES_CTRL_M0_EIPR_TYPE_T    m0_eipr;
    SES_CTRL_MM_EIPR_TYPE_T    mm_eipr;
    SES_CTRL_DF_EIPR_TYPE_T    df_eipr;
    SES_CTRL_DEMUX_EIPR_TYPE_T demux_eipr;
    SES_CTRL_MUX_EIPR_TYPE_T   mux_eipr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eipr, 0, sizeof(SES_CTRL_M0_EIPR_TYPE_T));
    PHYMOD_MEMSET(&mm_eipr, 0, sizeof(SES_CTRL_MM_EIPR_TYPE_T));
    PHYMOD_MEMSET(&df_eipr, 0, sizeof(SES_CTRL_DF_EIPR_TYPE_T));
    PHYMOD_MEMSET(&demux_eipr, 0, sizeof(SES_CTRL_DEMUX_EIPR_TYPE_T));
    PHYMOD_MEMSET(&mux_eipr, 0, sizeof(SES_CTRL_MUX_EIPR_TYPE_T));
    *intr_status = 0;

    /* Get the interrupt bit postion in the reg and interrupt group */
    _sesto_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);

    /* Interrupt status get on specified interrupt group */
    switch(intr_grp) {
        case SESTO_M0_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_M0_EIPR_ADR, m0_eipr.data);
           data = m0_eipr.data;
        break;
        case SESTO_MM_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MM_EIPR_ADR, mm_eipr.data);
           data = mm_eipr.data;
        break;
        case SESTO_DF_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DF_EIPR_ADR, df_eipr.data);
           data = df_eipr.data;
        break;
        case SESTO_DEMUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_DEMUX_EIPR_ADR, demux_eipr.data);
           data = demux_eipr.data;
        break;
        case SESTO_MUX_INTR_GRP:
           READ_SESTO_PMA_PMD_REG(pa, SES_CTRL_MUX_EIPR_ADR, mux_eipr.data);
           data = mux_eipr.data;
        break;
        default:
            return PHYMOD_E_NONE;
    }
    *intr_status = (data & (0x1 << bit_pos)) ? 1 : 0;

ERR:
    return rv;
}

int _sesto_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    int rv = PHYMOD_E_NONE;
    SES_CTRL_M0_EISR_TYPE_T    m0_eisr;
    SES_CTRL_MM_EISR_TYPE_T    mm_eisr;
    SES_CTRL_DF_EISR_TYPE_T    df_eisr;
    SES_CTRL_DEMUX_EISR_TYPE_T demux_eisr;
    SES_CTRL_MUX_EISR_TYPE_T   mux_eisr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eisr, 0, sizeof(SES_CTRL_M0_EISR_TYPE_T));
    PHYMOD_MEMSET(&mm_eisr, 0, sizeof(SES_CTRL_MM_EISR_TYPE_T));
    PHYMOD_MEMSET(&df_eisr, 0, sizeof(SES_CTRL_DF_EISR_TYPE_T));
    PHYMOD_MEMSET(&demux_eisr, 0, sizeof(SES_CTRL_DEMUX_EISR_TYPE_T));
    PHYMOD_MEMSET(&mux_eisr, 0, sizeof(SES_CTRL_MUX_EISR_TYPE_T));

    /* Get the interrupt bit postion in the reg and interrupt group */
    _sesto_get_intr_reg(phy, intr_type, &bit_pos, &intr_grp);
    intr_mask = 0x1 << bit_pos;

    /* Interrupt status clear on specified interrupt group */
    switch(intr_grp) {
        case SESTO_M0_INTR_GRP:
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_M0_EISR_ADR, intr_mask);
        break;
        case SESTO_MM_INTR_GRP:
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_MM_EISR_ADR, intr_mask);
        break;
        case SESTO_DF_INTR_GRP:
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_DF_EISR_ADR, intr_mask);
        break;
        case SESTO_DEMUX_INTR_GRP:
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_DEMUX_EISR_ADR, intr_mask);
        break;
        case SESTO_MUX_INTR_GRP:
           WRITE_SESTO_PMA_PMD_REG(pa, SES_CTRL_MUX_EISR_ADR, intr_mask);
        break;
        default:
            return PHYMOD_E_NONE;
    }

ERR:
    return rv;
}

int _sesto_pll_sequencer_restart(const phymod_core_access_t *core, phymod_sequencer_operation_t operation)
{
    uint16_t ip = 0, intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &core->access;
    DP_DECODED_MODE_STATUS0_TYPE_T mode_sts0;
    FALCON_IF_COMMON_CONTROL_TYPE_T  fal_cmn_ctrl;
    MERLIN_IF_COMMON_CONTROL_TYPE_T  mer_cmn_ctrl;

    SESTO_GET_INTF_SIDE(core, intf_side);
    READ_SESTO_PMA_PMD_REG(pa, DP_DECODED_MODE_STATUS0_ADR, mode_sts0.data);
    if (intf_side == SESTO_IF_LINE) {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_FALCON_CORE : SESTO_MERLIN_CORE;
    } else {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;
    }
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));
    switch(operation) {
        case phymodSeqOpStop:
        case phymodSeqOpStart:
            return PHYMOD_E_UNAVAIL;
        /* Toggle PLL Sequencer with pmd_core_dp_h_rstb_frcval */
        case phymodSeqOpRestart:
            if (ip == SESTO_FALCON_CORE) {
                READ_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);
                fal_cmn_ctrl.fields.pmd_core_dp_h_rstb_frc = 1;
                fal_cmn_ctrl.fields.pmd_core_dp_h_rstb_frcval = 0;
                WRITE_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);
                PHYMOD_USLEEP(1000);
                READ_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);
                fal_cmn_ctrl.fields.pmd_core_dp_h_rstb_frc = 1;
                fal_cmn_ctrl.fields.pmd_core_dp_h_rstb_frcval = 1;
                WRITE_SESTO_PMA_PMD_REG(pa, FALCON_IF_COMMON_CONTROL_ADR, fal_cmn_ctrl.data);
            } else {
                READ_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);
                mer_cmn_ctrl.fields.pmd_core_dp_h_rstb_frc = 1;
                mer_cmn_ctrl.fields.pmd_core_dp_h_rstb_frcval = 0;
                WRITE_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);
                PHYMOD_USLEEP(1000);
                READ_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);
                mer_cmn_ctrl.fields.pmd_core_dp_h_rstb_frc = 1;
                mer_cmn_ctrl.fields.pmd_core_dp_h_rstb_frcval = 1;
                WRITE_SESTO_PMA_PMD_REG(pa, MERLIN_IF_COMMON_CONTROL_ADR, mer_cmn_ctrl.data);

            }
        break;
        default:
        break;
    }
    /* Clear FRC of rstb */
    SESTO_CHIP_FIELD_WRITE(pa, FALCON_IF_COMMON_CONTROL, pmd_core_dp_h_rstb_frc, 0);
    SESTO_CHIP_FIELD_WRITE(pa, MERLIN_IF_COMMON_CONTROL, pmd_core_dp_h_rstb_frc, 0);

ERR:
    return rv;
}

int _sesto_fec_enable_set(const phymod_phy_access_t *phy, uint32_t enable)
{
    uint16_t ip = 0, intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_DECODED_MODE_STATUS0_TYPE_T mode_sts0;
    uint16_t data = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;

    SESTO_GET_INTF_SIDE(phy, intf_side);
    READ_SESTO_PMA_PMD_REG(pa, DP_DECODED_MODE_STATUS0_ADR, mode_sts0.data);
    if (intf_side == SESTO_IF_LINE) {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_FALCON_CORE : SESTO_MERLIN_CORE;
    } else {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;
    }

    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));
    /* Enables 100G FEC in Gearbox mode */
    if (ip == SESTO_FALCON_CORE) {
        SESTO_CHIP_FIELD_WRITE(pa, DP_SESTO_MODE_CTRL0, mode_cl91_en, enable);
    } else {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_PARAM,
            (_PHYMOD_MSG("FEC Applicable for Gearbox mode and 25G SERDES only")));
    }

    /* Enable FW After FEC configuration */
    SESTO_IF_ERR_RETURN(_sesto_fw_enable(pa, SESTO_ENABLE));
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);

        PHYMOD_DEBUG_VERBOSE(("FW Clear:%d\n",data));
        retry_cnt--;
    } while((data != 0) && (retry_cnt));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_INIT,
                (_PHYMOD_MSG("WARN:: FW Enable not cleared\n")));
    }

    /* Data path reset */
    SESTO_CHIP_FIELD_WRITE(pa, DP_DP_CONTROL0, datapath_rstb, 0);
    PHYMOD_USLEEP(100);
    SESTO_CHIP_FIELD_WRITE(pa, DP_DP_CONTROL0, datapath_rstb, 1);

ERR:
    return rv;
}
int _sesto_fec_enable_get(const phymod_phy_access_t *phy, uint32_t* enable)
{
    uint16_t ip = 0, intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    DP_DECODED_MODE_STATUS0_TYPE_T mode_sts0;

    SESTO_GET_INTF_SIDE(phy, intf_side);
    READ_SESTO_PMA_PMD_REG(pa, DP_DECODED_MODE_STATUS0_ADR, mode_sts0.data);
    if (intf_side == SESTO_IF_LINE) {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_FALCON_CORE : SESTO_MERLIN_CORE;
    } else {
        ip = (mode_sts0.fields.decoded_mode_falcon_line) ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;
    }

    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));
    /* FEC status on 100G Gearbox mode */
    if (ip == SESTO_FALCON_CORE) {
        SESTO_CHIP_FIELD_READ(pa, DP_SESTO_MODE_CTRL0, mode_cl91_en, (*enable));
    } else {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_PARAM,
            (_PHYMOD_MSG("FEC Applicable for Gearbox mode and 25G SERDES only")));
    }

ERR:
    return rv;
}

int _sesto_config_hcd_link_sts (const phymod_access_t* pa, phymod_phy_inf_config_t config, uint16_t ip ) 
{
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    int rv = PHYMOD_E_NONE;
    RX_ANA_REGS_ANARXCONTROL3_TYPE_T an_rx_ctrl3;
 
    aux_mode = config.device_aux_modes;

    if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
        /* 40G Mux Mode*/
        if (pa->lane_mask & 0xF) {
           SESTO_IF_ERR_RETURN(
               _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        } else {
           SESTO_IF_ERR_RETURN(
               _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        }
    } else {
        SESTO_IF_ERR_RETURN(
               _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
    }
    
    /* HCD_LINK_REG_WR */
    READ_SESTO_AN_REG(pa, RX_ANA_REGS_ANARXCONTROL3_ADR,
                        an_rx_ctrl3.data);
    an_rx_ctrl3.fields.use_kr4_block_lock_sm = 0;
    an_rx_ctrl3.fields.use_kr4_pmd_lock_sm = 1;
    WRITE_SESTO_AN_REG(pa, RX_ANA_REGS_ANARXCONTROL3_ADR,
                        an_rx_ctrl3.data);

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    return rv;
}

int _sesto_autoneg_ability_set(const phymod_phy_access_t* phy, sesto_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, data = 0;
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_TYPE_T adv1;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_TYPE_T adv2;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_TYPE_T adv3;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
 
    PHYMOD_MEMSET(&adv1, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&adv2, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&adv3, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&an_mst_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));

    SESTO_GET_INTF_SIDE(phy, intf_side);
    if (intf_side == SESTO_IF_SYS) {
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = config.device_aux_modes; 

    SESTO_IF_ERR_RETURN(_sesto_phy_interface_config_get(phy, 0, &config));
    
    if ((config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_20G ||
         config.data_rate == SESTO_SPD_11G || config.data_rate == SESTO_SPD_21G) ||
        ((config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) && aux_mode->gearbox_100g_inverse_mode)) {
        goto ERR;
    }
    SESTO_GET_IP(phy, config, ip);
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
    /* SES_GEN_CNTRLS_MICRO_SYNC_4_ADR: 0-7 bits are used as scratch register to configure the master lane */
    if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
        /* 40G Mux Mode*/
        if (lane_mask & 0xF) {
            SESTO_IF_ERR_RETURN(_sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
            if (an_ability->an_master_lane < 4) {
                READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
                an_mst_sel.data &= 0xFFF0;
                an_mst_sel.data |= an_ability->an_master_lane;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
            } else {
                PHYMOD_DEBUG_ERROR(("Invalid AN Master lane config, should be between 0 to 3\n"));
            }
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
            if (an_ability->an_master_lane < 4) {
                READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
                an_mst_sel.data &= 0xFF0F;
                an_mst_sel.data |= (an_ability->an_master_lane << 4);
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
            } else {
                PHYMOD_DEBUG_ERROR(("Invalid AN Master lane config, should be between 0 to 3\n"));
            }
        }
    } else {
        if ((aux_mode->BCM84793_capablity) || (config.data_rate == SESTO_SPD_100G)) {
            if (an_ability->an_master_lane < 4) {
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
                an_mst_sel.data &= 0xFF00;
            an_mst_sel.data |= an_ability->an_master_lane;
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
        } else {
                PHYMOD_DEBUG_ERROR(("Invalid AN Master lane config, should be between 0 to 3 depends on mode configuration\n"));
        }
        } else if ((an_ability->an_master_lane < 2) ||
            (((an_ability->an_master_lane & 0xF) >= 4) && ((an_ability->an_master_lane & 0xF) <= 5))) {
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
            an_mst_sel.data &= 0xFF00;
            an_mst_sel.data |= an_ability->an_master_lane;
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
        } else {
            PHYMOD_DEBUG_ERROR(("Invalid AN Master lane config, should be between 0-1 or 4-5 depends on mode configuration\n"));
        }


        if (((an_ability->an_master_lane & 0xF) >= 4) && ((an_ability->an_master_lane & 0xF) <= 5)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        }
    }

    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_ADR,
                        adv3.data);
    adv3.fields.fec_requested = an_ability->cl73_adv.an_fec; 
    WRITE_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_ADR,
                        adv3.data);

    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_ADR,
                        adv2.data);
    adv2.fields.techability = an_ability->cl73_adv.an_base_speed;
    WRITE_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_ADR,
                        adv2.data);

    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_ADR,
                        adv1.data);
    adv1.fields.pause = an_ability->cl73_adv.an_pause;
    WRITE_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_ADR,
                         adv1.data);

    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    SESTO_IF_ERR_RETURN(_sesto_config_hcd_link_sts (pa, config, ip));

    /* AN without TX training set GPREG11[11] to 1 */
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, data);
    an_ability->cl73_adv.an_cl72 ? (data &= (~(0x1 << 11))) : (data |= (0x1 << 11));
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, data);

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    uint16_t retry_cnt = 0;
    uint16_t intf_side = 0, data1;
    int rv = PHYMOD_E_NONE;
    SES_GEN_CNTRLS_GPREG11_TYPE_T gpreg11;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    const phymod_access_t *pa = &phy->access;

    SESTO_GET_INTF_SIDE(phy, intf_side);
    if (intf_side == SESTO_IF_SYS) {
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");

    SESTO_IF_ERR_RETURN(_sesto_phy_interface_config_get(phy, 0, &config));
    aux_mode = config.device_aux_modes;

    if ((config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_20G ||
         config.data_rate == SESTO_SPD_11G || config.data_rate == SESTO_SPD_21G) ||
        ((config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) && aux_mode->gearbox_100g_inverse_mode)) {
        if (an->enable) {
            PHYMOD_DEBUG_ERROR(("Invalid config for CL73, AN does not support for 10G/11G, 20G/21G and 100G Inverse mode\n"));
        }
        goto ERR;
    }

    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1);
        /* ATE_GUIDELINES : Sleep 10ms*/
        retry_cnt--;
    } while ((data1 != 0) && (retry_cnt));

    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
        goto ERR;
    }

    /* Setting the master lane by reading static config at scratch register and enable AN*/
    if (an->an_mode == phymod_AN_MODE_CL73) {
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);

        if (an->enable) {
            READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG10_ADR, data1);
            data1 &= (~(0x7 << 8)); /* PMD Lock check delay through GPREG10[11:8], default value is '0' of 100ms */
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG10_ADR, data1);
        }

        if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
            /* 40G Mux Mode*/
            if (lane_mask & 0xF) {
                /* Master lane config and AN Enable */
                if ((an_mst_sel.data & 0xF) <= 3) {
                    SESTO_CHIP_FIELD_WRITE(pa, DP_MER_AN_LANE_SELECT, mer_an_lane_sel0to3, (an_mst_sel.data & 0xF));
                } else {
                    PHYMOD_DEBUG_ERROR(("Invalid AN master lane, should be b/w 0 to 3\n"));
                }
                if (an->enable) {
                    gpreg11.fields.an_enable |= 0x1;
                } else {
                    gpreg11.fields.an_enable &= ~(0x1);
                }
            } else {
                /* Master lane config and AN Enable */
                if (((an_mst_sel.data >> 4) & 0xF) <= 3) {
                    SESTO_CHIP_FIELD_WRITE(pa, DP_MER_AN_LANE_SELECT,
                                           mer_an_lane_sel4to7, ((an_mst_sel.data >> 4) & 0xF));
                } else {
                    PHYMOD_DEBUG_ERROR(("Invalid AN master lane, should be b/w 0 to 3\n"));
                }
                if (an->enable) {
                    gpreg11.fields.an_enable |= 0x2;
                } else {
                    gpreg11.fields.an_enable &= ~(0x2);
                }
            }
        } else {
            /* 40G PT and 100G GBOX */
            if (ip == SESTO_FALCON_CORE) {
                /* Master lane config  and AN Enable */
                if ((an_mst_sel.data & 0xF) <= 3) {
                    SESTO_CHIP_FIELD_WRITE(pa, DP_FAL_AN_LANE_SELECT, fal_an_lane_sel0to3, (an_mst_sel.data & 0xF));
                } else {
                    PHYMOD_DEBUG_ERROR(("Invalid AN master lane, should be b/w 0 to 3\n"));
                }
                if (an->enable) {
                    gpreg11.fields.an_enable |= 0x1;
                } else {
                    gpreg11.fields.an_enable &= ~(0x1);
                }
            } else {
                if (((an_mst_sel.data & 0xF) <= 1) || (aux_mode->BCM84793_capablity & ((an_mst_sel.data & 0xF) <= 3))) {
                    /* Master lane config  and AN Enable */
                    SESTO_CHIP_FIELD_WRITE(pa, DP_MER_AN_LANE_SELECT, mer_an_lane_sel0to3, (an_mst_sel.data & 0xF));
                    if (an->enable) {
                        gpreg11.fields.an_enable |= 0x1;
                    } else {
                        gpreg11.fields.an_enable &= ~(0x1);
                    }
                } else if (((an_mst_sel.data & 0xF) >= 4) && ((an_mst_sel.data & 0xF) <= 5)) {
                    /* Master lane config  and AN Enable */
                    SESTO_CHIP_FIELD_WRITE(pa, DP_MER_AN_LANE_SELECT, mer_an_lane_sel4to7, ((an_mst_sel.data & 0xF) - 4));
                    if (an->enable) {
                        gpreg11.fields.an_enable |= 0x2;
                    } else {
                        gpreg11.fields.an_enable &= ~(0x2);
                    }
                } else {
                    /* AN Enable */
                    PHYMOD_DEBUG_ERROR(("Invalid AN master lane, should be b/w 0-1 or 4-5 depends on mode configuration\n"));
                    if (an->enable) {
                        gpreg11.fields.an_enable |= 0x1;
                    } else {
                        gpreg11.fields.an_enable &= ~(0x1);
                    }
                }
            }
        }
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);
    } else {
        goto ERR;
    }

    /*Setting the FW Enable to 1*/
    SESTO_IF_ERR_RETURN( _sesto_fw_enable(pa, 1));

    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data1);
        /* ATE_GUIDELINES : Sleep 10ms*/
        retry_cnt--;
    } while ((data1 != 0) && (retry_cnt));

    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
        goto ERR;
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_autoneg_get(const phymod_phy_access_t* phy, phymod_autoneg_control_t* an, uint32_t *an_done)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    IEEE_AN_BLK0_AN_STATUS_REGISTER_TYPE_T an_status;
    IEEE_AN_BLK0_AN_CONTROL_REGISTER_TYPE_T an_ctrl;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
    const phymod_access_t *pa = &phy->access;

    SESTO_GET_INTF_SIDE(phy, intf_side);
    if (intf_side == SESTO_IF_SYS) {
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(&an_mst_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));
    PHYMOD_MEMSET(&an_status, 0, sizeof(IEEE_AN_BLK0_AN_STATUS_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&an_ctrl, 0, sizeof(IEEE_AN_BLK0_AN_CONTROL_REGISTER_TYPE_T));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = config.device_aux_modes;

    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    aux_mode = config.device_aux_modes;

    /* AN does not support 10G, 20G and 100G Inverse mode */
    if ((config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_20G ||
         config.data_rate == SESTO_SPD_11G || config.data_rate == SESTO_SPD_21G) ||
        ((config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) && aux_mode->gearbox_100g_inverse_mode)) {
        goto ERR;
    }

    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
    if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
        /* 40G Mux Mode*/
        if (lane_mask & 0xF) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        }
    } else {
        if (((an_mst_sel.data & 0xF) >= 4) && ((an_mst_sel.data & 0xF) <= 5)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        }
    }

    /* AN enable status */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK0_AN_CONTROL_REGISTER_ADR, an_ctrl.data);
    an->enable = an_ctrl.fields.auto_negotiationenable;

    /* AN Mode */
    an->an_mode = phymod_AN_MODE_CL73;

    /* AN complete status read AN_DONE */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK0_AN_STATUS_REGISTER_ADR, an_status.data);
    *an_done = an_status.fields.auto_negotiationcomplete;

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_autoneg_ability_get(const phymod_phy_access_t* phy, sesto_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_TYPE_T adv1;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_TYPE_T adv2;
    IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_TYPE_T adv3;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
    const phymod_access_t *pa = &phy->access;


    SESTO_GET_INTF_SIDE(phy, intf_side);
    if (intf_side == SESTO_IF_SYS) {
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(&adv1, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&adv2, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&adv3, 0, sizeof(IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_TYPE_T));
    PHYMOD_MEMSET(&an_mst_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = config.device_aux_modes;

    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    /* AN does not support 10G, 20G and 100G Inverse mode */
    if ((config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_20G ||
         config.data_rate == SESTO_SPD_11G || config.data_rate == SESTO_SPD_21G) ||
        ((config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) && aux_mode->gearbox_100g_inverse_mode)) {
        goto ERR;
    }

    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
    if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
        /* 40G Mux Mode*/
        if (lane_mask & 0xF) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        }
    } else {
        if (((an_mst_sel.data & 0xF) >= 4) && ((an_mst_sel.data & 0xF) <= 5)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        }
    }

    /* Fec ability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_3_REGISTER_ADR,
                        adv3.data);
    an_ability->cl73_adv.an_fec = adv3.fields.fec_requested;

    /* Techability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_2_REGISTER_ADR,
                        adv2.data);
    an_ability->cl73_adv.an_base_speed = adv2.fields.techability;

    /* AN ability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_ADVERTISEMENT_1_REGISTER_ADR,
                        adv1.data);
    an_ability->cl73_adv.an_pause = adv1.fields.pause;

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_autoneg_remote_ability_get(const phymod_phy_access_t *phy, phymod_autoneg_ability_t* an_ability_get)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    int rv = PHYMOD_E_NONE;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_1_REG_TYPE_T adv1;
    IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_2_REG_TYPE_T adv2;
    IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_3_REG_TYPE_T adv3;
    SES_GEN_CNTRLS_MICRO_SYNC_4_TYPE_T an_mst_sel;
    const phymod_access_t *pa = &phy->access;

    SESTO_GET_INTF_SIDE(phy, intf_side);
    if (intf_side == SESTO_IF_SYS) {
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(&adv1, 0, sizeof(IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_1_REG_TYPE_T));
    PHYMOD_MEMSET(&adv2, 0, sizeof(IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_2_REG_TYPE_T));
    PHYMOD_MEMSET(&adv3, 0, sizeof(IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_3_REG_TYPE_T));
    PHYMOD_MEMSET(&an_mst_sel, 0, sizeof(SES_GEN_CNTRLS_MICRO_SYNC_0_TYPE_T));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = config.device_aux_modes;

    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    /* AN does not support 10G, 20G and 100G Inverse mode */
    if ((config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_20G ||
         config.data_rate == SESTO_SPD_11G || config.data_rate == SESTO_SPD_21G) ||
        ((config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) && aux_mode->gearbox_100g_inverse_mode)) {
        goto ERR;
    }

    SESTO_GET_IP(phy, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", lane_mask));

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_4_ADR, an_mst_sel.data);
    if (!aux_mode->pass_thru && config.data_rate != SESTO_SPD_100G) {
        /* 40G Mux Mode*/
        if (lane_mask & 0xF) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        }
    } else {
        if (((an_mst_sel.data & 0xF) >= 4) && ((an_mst_sel.data & 0xF) <= 5)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 1));
        } else {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                                         SESTO_DEV_AN, 0, 0));
        }
    }

    /* Fec ability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_3_REG_ADR,
                        adv3.data);
    an_ability_get->an_fec = (adv3.data & 0xC000) >> 15;

    /* Techability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_2_REG_ADR,
                        adv2.data);
    an_ability_get->an_cap = (adv2.data & 0xFFE0) >> 5;

    /* AN ability */
    READ_SESTO_AN_REG(pa, IEEE_AN_BLK1_AN_LP_BASE_PAGE_ABILITY_1_REG_ADR,
                        adv1.data);
    if (adv1.data & 0x400) {
        PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability_get);
    } else if (adv1.data & 0x800) {
        PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability_get);
    }

ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}


int _sesto_port_from_lane_map_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* cfg, uint16_t *port)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    DP_SESTO_MODE_CTRL0_TYPE_T ctrl0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    SESTO_GET_IP(phy, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));

    PHYMOD_MEMSET(&ctrl0, 0, sizeof(DP_SESTO_MODE_CTRL0_TYPE_T));
    READ_SESTO_PMA_PMD_REG(pa, DP_SESTO_MODE_CTRL0_ADR,ctrl0.data);
    if (config.data_rate == SESTO_SPD_100G || config.data_rate == SESTO_SPD_106G) {
        *port = 0;
    } else if (config.data_rate == SESTO_SPD_40G || config.data_rate == SESTO_SPD_42G) {
        if (ip == SESTO_FALCON_CORE) {
            if (aux_mode->pass_thru || aux_mode->BCM84793_capablity) {
                if (lane_mask == 0xF) {        /*port 0 */
                    *port = 0;
                }
            } else {   /* 40G MUX */
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            }
        } else {
            if (aux_mode->pass_thru || aux_mode->BCM84793_capablity) {
                if (lane_mask == 0x33) {       /* port 0 */
                    *port = 0;
                }
                else if (lane_mask == 0xF) {   /* port 0 */
                    *port = 0;
                }
            } else {   /* 40G MUX */
                if (lane_mask == 0xF) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xF0) { /* port 1 */
                    *port = 1;
                }
            }
        }
        if (ctrl0.fields.mode_multiport2_40g_mux && ctrl0.fields.mode_multiport1_10g1 &&
            ctrl0.fields.mode_multiport1_10g2) {
            *port += 1;
        }
    } else if (config.data_rate == SESTO_SPD_20G || config.data_rate == SESTO_SPD_21G) {
        if (ip == SESTO_FALCON_CORE) {
            if (aux_mode->pass_thru) {
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            } else {   /* 20G MUX */
                if (lane_mask == 0x1) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0x4) { /* port 1 */
                    *port = 1;
                }
            }
        } else  { /* 20G PT & MUX */
            if (lane_mask == 0x3) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x30) {  /* port 1 */
                *port = 1;
            }
        }
        if ((ctrl0.fields.mode_multiport2_20g_mux || ctrl0.fields.mode_multiport2_20g_passthru) &&
            (ctrl0.fields.mode_multiport1_10g1 && ctrl0.fields.mode_multiport1_10g2)) {
            *port += 1;
        }
    } else if (config.data_rate == SESTO_SPD_1G || config.data_rate == SESTO_SPD_10G || config.data_rate == SESTO_SPD_11G) {
        if (ip == SESTO_FALCON_CORE) {
            if (lane_mask == 0x1) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x2) {   /* port 1 */
                *port = 1;
            } else if (lane_mask == 0x4) {   /* port 2 */
                *port = 2;
            } else if (lane_mask == 0x8) {   /* port 3 */
                *port = 3;
            }
        } else {
            if (lane_mask == 0x1) {          /* port 0 */
                *port = 0;
            } else if (lane_mask == 0x2) {   /* port 1 */
                *port = 1;
            } else if (lane_mask == 0x10) {  /* port 2 */
                *port = 2;
            } else if (lane_mask == 0x20) {  /* port 3 */
                *port = 3;
            } else if ((lane_mask == 0x4) && (aux_mode->BCM84793_capablity)){
                *port = 2;
            } else if ((lane_mask == 0x8) && (aux_mode->BCM84793_capablity)){
                *port = 3;
            }
        }
    }

ERR:
    return rv;
}
int _sesto_module_write(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr,
                            uint32_t no_of_bytes, const uint8_t *write_data)
{
    uint16_t lane_mask = 0;
    uint32_t index = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    SES_GEN_CNTRLS_GEN_CONTROL3_TYPE_T gen_ctrl;
    uint16_t START_OF_NVRAM = 0x0;
    int rv = PHYMOD_E_NONE;

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }
    /* qsfp mode or legacy mode */
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
    gen_ctrl.fields.qsfp_mode = 1;
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);

    /* qsfp mode reset at begining */
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GEN_CONTROL1, modctrl_rstb, 0);
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GEN_CONTROL1, modctrl_rstb, 1);

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    if (lane_mask <= 0xF) { /* QSFP module 0 */
        /* Active Low Select to read NVRAM for QSFP module 0 */
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        gen_ctrl.fields.qsfp_sel0b = 0;
        gen_ctrl.fields.qsfp_sel1b = 1;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        /* Module 0 NVRAM ADDR is SES_MODULE_CNTRL_RAM_NVR0_ADR */
        START_OF_NVRAM = 0x0;
    } else if (lane_mask <= 0xF0) {  /* QSFP module 1 */
        /* Active Low Select to read NVRAM for QSFP module 1 */
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        gen_ctrl.fields.qsfp_sel0b = 1;
        gen_ctrl.fields.qsfp_sel1b = 0;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        /* Module 1 NVRAM ADDR is SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM*/
        START_OF_NVRAM = 0x100;
    }

    /* Configure the slave device ID default is 0x50 */
    SESTO_CHIP_FIELD_WRITE(pa, SES_MODULE_CNTRL_DEV_ID, sl_dev_add, slv_dev_addr);

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        return rv;
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

    /* Wtite data to NVRAM */
    for (index = 0; index < no_of_bytes; index++) {
        WRITE_SESTO_PMA_PMD_REG(pa,
            (SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM + start_addr + index), write_data[index]);
    }

    if(lower_page_flag) {
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        for (index = 0; index < (lower_page_bytes / 4); index ++) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_module_command(pa, SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index), 3, SESTO_WRITE));
        }
        if ((lower_page_bytes % 4) > 0) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_module_command(pa, SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index),
                                                       ((lower_page_bytes % 4) - 1), SESTO_WRITE));
        }
        lower_page_flag = 0;
    }

    if(upper_page_flag) {
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        for (index = 0; index < (upper_page_bytes / 4); index++) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_module_command(pa, (SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index), 3, SESTO_WRITE));
        }
        if ((upper_page_bytes%4) > 0) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_module_command(pa, (SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index),
                                                        ((upper_page_bytes % 4) - 1), SESTO_WRITE));
        }
        upper_page_flag = 0;
    }

ERR:
    return rv;
}

int _sesto_module_read(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr,
                      uint32_t no_of_bytes, uint8_t *read_data)
{
    uint32_t index = 0;
    uint16_t lane_mask = 0;
    uint16_t rd_data = 0;
    uint32_t lower_page_bytes = 0;
    uint32_t upper_page_bytes = 0;
    uint32_t upper_page_flag = 0;
    uint32_t lower_page_start_addr = 0;
    uint32_t upper_page_start_addr = 0;
    uint32_t lower_page_flag = 0;
    uint16_t START_OF_NVRAM = 0x0;
    int rv = PHYMOD_E_NONE;
    SES_GEN_CNTRLS_GEN_CONTROL3_TYPE_T gen_ctrl;

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }
    /* qsfp mode or legacy mode */
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
    gen_ctrl.fields.qsfp_mode = 1;
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);

    /* qsfp mode reset at begining */
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GEN_CONTROL1, modctrl_rstb, 0);
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_GEN_CONTROL1, modctrl_rstb, 1);

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    if (lane_mask <= 0xF) { /* QSFP module 0 */
        /* Active Low Select to read NVRAM for QSFP module 0 */
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        gen_ctrl.fields.qsfp_sel0b = 0;
        gen_ctrl.fields.qsfp_sel1b = 1;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        /* Module 0 NVRAM ADDR is SES_MODULE_CNTRL_RAM_NVR0_ADR */
        START_OF_NVRAM = 0x0;
    } else if (lane_mask <= 0xF0) {  /* QSFP module 1 */
        /* Active Low Select to read NVRAM for QSFP module 1 */
        READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        gen_ctrl.fields.qsfp_sel0b = 1;
        gen_ctrl.fields.qsfp_sel1b = 0;
        WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GEN_CONTROL3_ADR, gen_ctrl.data);
        /* Module 1 NVRAM ADDR is SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM*/
        START_OF_NVRAM = 0x100;
    }

    /* Configure the slave device ID default is 0x50 */
    SESTO_CHIP_FIELD_WRITE(pa, SES_MODULE_CNTRL_DEV_ID, sl_dev_add, slv_dev_addr);

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        return rv;
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
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                lower_page_start_addr, lower_page_start_addr, lower_page_bytes - 1, SESTO_RANDOM_ADDRESS_READ));
        lower_page_flag = 0;
    }

    /* Need to check with chip team how we can read upper page */
    if (upper_page_flag) {
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, 0, 0, 0, SESTO_FLUSH));
        SESTO_IF_ERR_RETURN(
            _sesto_set_module_command(pa, SES_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                upper_page_start_addr, upper_page_start_addr, upper_page_bytes - 1, SESTO_RANDOM_ADDRESS_READ));
        upper_page_flag = 0;
    }

    /* Read data from NVRAM */
    for (index = 0; index < no_of_bytes; index++) {
       READ_SESTO_PMA_PMD_REG(pa, (SES_MODULE_CNTRL_RAM_NVR0_ADR + start_addr + index), rd_data);
       read_data[index] = (unsigned char) (rd_data & 0xff);
    }

ERR:
    return rv;
}

int _sesto_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr,
                               uint32_t slv_addr, unsigned char xfer_cnt, SESTO_I2CM_CMD_E cmd)
{
    SES_MODULE_CNTRL_STATUS_TYPE_T mctrl_status;
    uint16_t retry_count = 500;
    uint32_t wait_timeout_us = 0;
    int rv = PHYMOD_E_NONE;
    wait_timeout_us = ((2*(xfer_cnt+1))*100)/5;

    if (cmd == SESTO_FLUSH) {
       WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_CONTROL_ADR, 0xC000);
    } else {
        WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_XFER_ADDRESS_ADR, xfer_addr);
        WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_XFER_COUNT_ADR, xfer_cnt);

        if (cmd == SESTO_CURRENT_ADDRESS_READ) {
            WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_CONTROL_ADR, 0x8001);
        } else if (cmd == SESTO_RANDOM_ADDRESS_READ ) {
            WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_ADDRESS_ADR, slv_addr);
            WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_CONTROL_ADR, 0x8003);
        } else {
            WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_ADDRESS_ADR, slv_addr);
            WRITE_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_CONTROL_ADR, 0x8022);
        }
    }

    if ((cmd == SESTO_CURRENT_ADDRESS_READ) ||
        (cmd == SESTO_RANDOM_ADDRESS_READ) ||
        (cmd == SESTO_WRITE)) {
        do {
            READ_SESTO_PMA_PMD_REG(pa, SES_MODULE_CNTRL_STATUS_ADR, mctrl_status.data);
            PHYMOD_USLEEP(wait_timeout_us);
        } while((mctrl_status.fields.xaction_done == 0) && --retry_count);
        if(!retry_count) {
            SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Module controller: I2C transaction failed..")));
        }
    }

ERR:
    return rv;
}

int _sesto_gpio_config_set(const phymod_access_t *pa, int pin_number, phymod_gpio_mode_t gpio_mode)
{
    int rv = PHYMOD_E_NONE;
    SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T pad_ctrl_gpio_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_ctrl, 0 , sizeof(SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T));
    if (pin_number > 3) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    READ_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_CONTROL_ADR + (pin_number * 2)),
                           pad_ctrl_gpio_ctrl.data);

    switch (gpio_mode) {
      case phymodGpioModeDisabled:
          return rv;
      case phymodGpioModeOutput:
          pad_ctrl_gpio_ctrl.fields.gpio_0_oeb = 0;
      break;
      case phymodGpioModeInput:
          pad_ctrl_gpio_ctrl.fields.gpio_0_oeb = 1;
      break;
      default:
          return PHYMOD_E_PARAM;
    }

    WRITE_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_CONTROL_ADR + (pin_number * 2)),
                           pad_ctrl_gpio_ctrl.data);
ERR:
    return rv;
}

int _sesto_gpio_config_get(const phymod_access_t *pa, int pin_number, phymod_gpio_mode_t* gpio_mode)
{
    int rv = PHYMOD_E_NONE;
    SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T pad_ctrl_gpio_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_ctrl, 0 , sizeof(SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T));
    if (pin_number > 3) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    READ_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_CONTROL_ADR + (pin_number * 2)),
                           pad_ctrl_gpio_ctrl.data);

    switch(pad_ctrl_gpio_ctrl.fields.gpio_0_oeb) {
        case 0:
            *gpio_mode = phymodGpioModeOutput;
        break;
        case 1:
            *gpio_mode = phymodGpioModeInput;
        break;
        default:
        break;
    }

ERR:
    return rv;
}

int _sesto_gpio_pin_value_set(const phymod_access_t *pa, int pin_number, int value)
{
    int rv = PHYMOD_E_NONE;
    SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T pad_ctrl_gpio_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_ctrl, 0 , sizeof(SES_PAD_CTRL_GPIO_0_CONTROL_TYPE_T));
    if (pin_number > 3) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }
    READ_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_CONTROL_ADR + (pin_number * 2)),
                           pad_ctrl_gpio_ctrl.data);

    pad_ctrl_gpio_ctrl.fields.gpio_0_ibof = 1;
    pad_ctrl_gpio_ctrl.fields.gpio_0_out_frcval = value ? 1 : 0;

    WRITE_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_CONTROL_ADR + (pin_number * 2)),
                           pad_ctrl_gpio_ctrl.data);
ERR:
    return rv;
}

int _sesto_gpio_pin_value_get(const phymod_access_t *pa, int pin_number, int* value)
{
    int rv = PHYMOD_E_NONE;
    SES_PAD_CTRL_GPIO_0_STATUS_TYPE_T pad_sts_gpio;

    PHYMOD_MEMSET(&pad_sts_gpio, 0, sizeof(SES_PAD_CTRL_GPIO_0_STATUS_TYPE_T));
    if (pin_number > 3) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    READ_SESTO_PMA_PMD_REG(pa,
                          (SES_PAD_CTRL_GPIO_0_STATUS_ADR + (pin_number * 2)),
                           pad_sts_gpio.data);
    *value = pad_sts_gpio.fields.gpio_0_din;

ERR:
    return rv;
}

int _sesto_phy_repeater_mode_set(const phymod_phy_access_t *phy, phymod_operation_mode_t repeater_mode)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t port = 0;
    uint16_t data = 0, retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    int rv = PHYMOD_E_NONE;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    SES_GEN_CNTRLS_GPREG11_TYPE_T gpreg11;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    aux_mode = config.device_aux_modes;

    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    ip = aux_mode->passthru_sys_side_core ? SESTO_MERLIN_CORE : SESTO_FALCON_CORE;

    /* ip == SESTO_MERLIN_CORE, line sie core is SESTO_FALCON_CORE */
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON"));
    PHYMOD_MEMSET(&gpreg11, 0, sizeof(SES_GEN_CNTRLS_GPREG11_TYPE_T));
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);

    SESTO_IF_ERR_RETURN(
        _sesto_port_from_lane_map_get(phy, &config, &port));


    if (ip == SESTO_FALCON_CORE) {
        if (!repeater_mode) {
            gpreg11.fields.dis_rptr_20g_to_10g |= (0x1 << port);
        } else {
            gpreg11.fields.dis_rptr_20g_to_10g &= ~(0x1 << port);
        }
    } else {
        if (!repeater_mode) {
            gpreg11.fields.dis_rptr_10g_to_20g |= (0x1 << port);
        } else {
            gpreg11.fields.dis_rptr_10g_to_20g &= ~(0x1 << port);
        }
    }
    WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_GPREG11_ADR, gpreg11.data);

    /*  Set FW_ENABLE = 1 */
    SESTO_CHIP_FIELD_WRITE(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, 1);
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);
    } while ((data != 0) && (retry_cnt --));
    if (retry_cnt == 0) {
        SESTO_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("repeater_mode_set failed, micro controller is busy..")));
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}
int _sesto_core_cfg_tx_set(const phymod_phy_access_t *phy,const phymod_tx_t* tx)
{
    phymod_tx_t tmp_tx;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&tmp_tx,0x0,sizeof(phymod_tx_t));
    SESTO_IF_ERR_RETURN(
            _sesto_tx_get(phy,&tmp_tx));
    if (!(((tx->main & 0x7F) == 0x7F) && ((tx->pre & 0x7F) == 0x7F) && ((tx->post & 0x7F) == 0x7F))) {
        tmp_tx.pre = tx->pre;
        tmp_tx.main = tx->main;
        tmp_tx.post = tx->post;

    }
    tmp_tx.amp = tx->amp;
    SESTO_IF_ERR_RETURN(
            _sesto_tx_set(phy,&tmp_tx));

ERR:
    return rv;
}

int _sesto_core_cfg_polarity_set(const phymod_phy_access_t *phy, uint32_t tx_polarity, uint32_t rx_polarity)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint32_t rx_lane_polarity = 0;
    uint32_t tx_lane_polarity = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;

    if (rx_polarity == 0 && tx_polarity == 0) {
        return rv;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
              _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST,
                        ip, SESTO_DEV_PMA_PMD, 0, lane));
            if (rx_polarity != 0) {
                rx_lane_polarity = (rx_polarity >> lane) & 0x1;
            }
            if (tx_polarity != 0) {
                tx_lane_polarity = (tx_polarity >> lane) & 0x1;
            }
            if (ip == SESTO_FALCON_CORE) {
                /* FALCON polarity inversion */
                PHYMOD_DEBUG_VERBOSE(("Falcon polarity inversion set\n"));
                if (tx_lane_polarity) {
                    /* Write to Tx misc config register */
                    SESTO_CHIP_FIELD_WRITE(pa, F25G_TLB_TX_TLB_TX_MISC_CONFIG,
                                           tx_pmd_dp_invert, tx_lane_polarity);
                }
                if (rx_lane_polarity) {
                    /* Write to Rx misc config register */
                    SESTO_CHIP_FIELD_WRITE(pa, F25G_TLB_RX_TLB_RX_MISC_CONFIG,
                                           rx_pmd_dp_invert, rx_lane_polarity);
                }
            } else {
                /* MERLIN polarity inversion */
                PHYMOD_DEBUG_VERBOSE(("Merlin polarity inversion set\n"));
                if (tx_lane_polarity) {
                    /* Write to Tx misc config register */
                    SESTO_CHIP_FIELD_WRITE(pa, M10G_TLB_TX_TLB_TX_MISC_CONFIG,
                                           tx_pmd_dp_invert, tx_lane_polarity);
                }
                if (rx_lane_polarity) {
                    /* Write to Rx misc config register */
                    SESTO_CHIP_FIELD_WRITE(pa, M10G_TLB_RX_TLB_RX_MISC_CONFIG,
                                           rx_pmd_dp_invert, rx_lane_polarity);
                }
            }
        }
    }
ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

err_code_t _sesto_merlin_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_pll_range();
  return (ERR_CODE_NONE);
}
err_code_t _sesto_falcon_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range)
{
  err_code_t __err;
  __err=ERR_CODE_NONE;
  *pll_range = rdc_falcon_furia_sesto_ams_pll_range();
  return (ERR_CODE_NONE);
}

int _sesto_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config)
{
    uint16_t ip = 0, data = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    SES_GEN_CNTRLS_MICRO_SYNC_5_TYPE_T intf_sel_p0_p1;
    SES_GEN_CNTRLS_MICRO_SYNC_6_TYPE_T intf_sel_p2_p3;
    phymod_phy_inf_config_t config;
    const phymod_access_t *pa = &phy->access;
    uint16_t retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    uint16_t port = 0, no_of_ports = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&intf_sel_p0_p1, 0, sizeof(intf_sel_p0_p1));
    PHYMOD_MEMSET(&intf_sel_p2_p3, 0, sizeof(intf_sel_p2_p3));

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)config.device_aux_modes;
    SESTO_GET_IP(phy, config, ip);
    SESTO_GET_PORT_FROM_MODE((&config), no_of_ports, aux_mode);
    SESTO_IF_ERR_RETURN(_sesto_port_from_lane_map_get(phy, &config, &port));

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
    if (no_of_ports == 1) { /* 100G/40GPT */
        if (ip == SESTO_MERLIN_CORE) {
            if (fw_config.UnreliableLos) {
                intf_sel_p0_p1.fields.p0_10g_media_type = 3;
            } else {
                intf_sel_p0_p1.fields.p0_10g_media_type = fw_config.MediaType;
            }
            intf_sel_p0_p1.fields.p0_10g_dfe_on = fw_config.DfeOn;
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
        } else {
            if (fw_config.UnreliableLos) {
                intf_sel_p0_p1.fields.p0_20g_media_type = 3;
            } else {
                intf_sel_p0_p1.fields.p0_20g_media_type = fw_config.MediaType;
            }
            intf_sel_p0_p1.fields.p0_20g_dfe_on = fw_config.DfeOn;
            intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = fw_config.LpDfeOn;
            intf_sel_p0_p1.fields.p0_scrambling_dis = fw_config.ScramblingDisable;
            WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
        }
    } else if (no_of_ports == 2) { /* 40GMUX/20GMUX/20PT */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p0_10g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p0_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p0_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p0_20g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p0_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p0_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p0_p1.fields.p0_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p2_10g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p2_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p2_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p2_20g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p2_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p2_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p2_p3.fields.p2_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        }
    } else { /* 10G */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p0_10g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p0_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p0_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p0_20g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p0_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p0_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p0_p1.fields.p0_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else if (port == 1) {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p1_10g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p1_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p1_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p0_p1.fields.p1_20g_media_type = 3;
                } else {
                    intf_sel_p0_p1.fields.p1_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p0_p1.fields.p1_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p0_p1.fields.p1_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p0_p1.fields.p1_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
            }
        } else if (port == 2) {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p2_10g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p2_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p2_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p2_20g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p2_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p2_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p2_p3.fields.p2_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p3_10g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p3_10g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p3_10g_dfe_on = fw_config.DfeOn;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            } else {
                if (fw_config.UnreliableLos) {
                    intf_sel_p2_p3.fields.p3_20g_media_type = 3;
                } else {
                    intf_sel_p2_p3.fields.p3_20g_media_type = fw_config.MediaType;
                }
                intf_sel_p2_p3.fields.p3_20g_dfe_on = fw_config.DfeOn;
                intf_sel_p2_p3.fields.p3_20g_dfe_lp_mode = fw_config.LpDfeOn;
                intf_sel_p2_p3.fields.p3_scrambling_dis = fw_config.ScramblingDisable;
                WRITE_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
            }
        }
    }

    /*Setting the FW Enable to 1*/
    SESTO_IF_ERR_RETURN( _sesto_fw_enable(pa, 1));

    retry_cnt = SESTO_FW_DLOAD_RETRY_CNT;
    do {
        SESTO_CHIP_FIELD_READ(pa, SES_GEN_CNTRLS_FIRMWARE_ENABLE, fw_enable_val, data);
        retry_cnt--;
    } while ((data != 0) && (retry_cnt));

    if (retry_cnt == 0) {
        PHYMOD_DEBUG_VERBOSE(("WARN:: FW Enable not cleared\n"));
        goto ERR;
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);
    return rv;
}

int _sesto_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config)
{
    uint16_t ip = 0;
    SESTO_DEVICE_AUX_MODE_T  *aux_mode;
    SES_GEN_CNTRLS_MICRO_SYNC_5_TYPE_T intf_sel_p0_p1;
    SES_GEN_CNTRLS_MICRO_SYNC_6_TYPE_T intf_sel_p2_p3;
    phymod_phy_inf_config_t config;
    const phymod_access_t *pa = &phy->access;
    uint16_t port = 0, no_of_ports = 0;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&intf_sel_p0_p1, 0, sizeof(intf_sel_p0_p1));
    PHYMOD_MEMSET(&intf_sel_p2_p3, 0, sizeof(intf_sel_p2_p3));

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    aux_mode = (SESTO_DEVICE_AUX_MODE_T*)config.device_aux_modes;
    SESTO_GET_IP(phy, config, ip);
    SESTO_GET_PORT_FROM_MODE((&config), no_of_ports, aux_mode);
    SESTO_IF_ERR_RETURN(_sesto_port_from_lane_map_get(phy, &config, &port));

    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_5_ADR, intf_sel_p0_p1.data);
    READ_SESTO_PMA_PMD_REG(pa, SES_GEN_CNTRLS_MICRO_SYNC_6_ADR, intf_sel_p2_p3.data);
    if (no_of_ports == 1) { /* 100G/40GPT */
        if (ip == SESTO_MERLIN_CORE) {
            if (intf_sel_p0_p1.fields.p0_10g_media_type == 3) {
                fw_config->UnreliableLos = 1;
                fw_config->MediaType = phymodFirmwareMediaTypeOptics;
            } else {
                fw_config->MediaType = intf_sel_p0_p1.fields.p0_10g_media_type;
            }
            fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_10g_dfe_on;
        } else {
            if (intf_sel_p0_p1.fields.p0_20g_media_type == 3) {
                fw_config->UnreliableLos = 1;
                fw_config->MediaType = phymodFirmwareMediaTypeOptics;
            } else {
                fw_config->MediaType = intf_sel_p0_p1.fields.p0_20g_media_type;
            }
            fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_20g_dfe_on;
            fw_config->LpDfeOn = intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode;
            fw_config->ScramblingDisable = intf_sel_p0_p1.fields.p0_scrambling_dis;
        }
    } else if (no_of_ports == 2) { /* 40GMUX/20GMUX/20PT */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p0_p1.fields.p0_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p0_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_10g_dfe_on;
            } else {
                if (intf_sel_p0_p1.fields.p0_20g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p0_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p0_p1.fields.p0_scrambling_dis;
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p2_p3.fields.p2_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p2_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p2_10g_dfe_on;
            } else {
                if (intf_sel_p2_p3.fields.p2_20g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p2_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p2_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p2_p3.fields.p2_scrambling_dis;
            }
        }
    } else { /* 10G */
        if (port == 0) {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p0_p1.fields.p0_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p0_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_10g_dfe_on;
            } else {
                if (intf_sel_p0_p1.fields.p0_20g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p0_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p0_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p0_p1.fields.p0_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p0_p1.fields.p0_scrambling_dis;
            }
        } else if (port == 1) {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p0_p1.fields.p1_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p1_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p1_10g_dfe_on;
            } else {
                if (intf_sel_p0_p1.fields.p1_20g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p0_p1.fields.p1_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p0_p1.fields.p1_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p0_p1.fields.p1_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p0_p1.fields.p1_scrambling_dis;
            }
        } else if (port == 2) {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p2_p3.fields.p2_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p2_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p2_10g_dfe_on;
            } else {
                if (intf_sel_p2_p3.fields.p2_20g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p2_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p2_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p2_p3.fields.p2_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p2_p3.fields.p2_scrambling_dis;
            }
        } else {
            if (ip == SESTO_MERLIN_CORE) {
                if (intf_sel_p2_p3.fields.p3_10g_media_type == 3) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p3_10g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p3_10g_dfe_on;
            } else {
                if (intf_sel_p2_p3.fields.p3_20g_media_type) {
                    fw_config->UnreliableLos = 1;
                    fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                } else {
                    fw_config->MediaType = intf_sel_p2_p3.fields.p3_20g_media_type;
                }
                fw_config->DfeOn =  intf_sel_p2_p3.fields.p3_20g_dfe_on;
                fw_config->LpDfeOn = intf_sel_p2_p3.fields.p3_20g_dfe_lp_mode;
                fw_config->ScramblingDisable = intf_sel_p2_p3.fields.p2_scrambling_dis;
            }
        }
    }

ERR:
    PHYMOD_FREE(config.device_aux_modes);
    return rv;
}

int _sesto_phy_short_channel_mode_set(const phymod_phy_access_t *phy, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t data = 0;
    uint16_t ip = 0, lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    if (ip == SESTO_MERLIN_CORE) {
        /* short_channel_mode can performed only on falcon side */
        return PHYMOD_E_PARAM;
    }

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            SESTO_IF_ERR_RETURN(
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip, SESTO_DEV_PMA_PMD, 0, lane));
            /* put the lane under dp reset */
            SESTO_CHIP_FIELD_WRITE(pa, F25G_CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL, ln_dp_s_rstb, 0);
            /* config RAM variable */
            SESTO_CHIP_FIELD_WRITE(pa, SES_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD, mst_dram_mem_add, (0x420 + (lane * 0x130)));
            SESTO_CHIP_FIELD_READ(pa, SES_M0ACCESS_DATA_MASTER_DRAM_RDATA, mst_dram_rdata, data);
            enable ? (data |= (0x1 << 11)): (data &= (~(0x1 << 11)));
            SESTO_CHIP_FIELD_WRITE(pa, SES_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD, mst_dram_mem_add, (0x420 + (lane * 0x130)));
            SESTO_CHIP_FIELD_WRITE(pa, SES_M0ACCESS_DATA_MASTER_DRAM_WDATA, mst_dram_wdata, data);
            /* Relase the lane under dp reset */
            SESTO_CHIP_FIELD_WRITE(pa, F25G_CKRST_CTRL_RPTR_LANE_CLK_RESET_N_POWERDOWN_CONTROL, ln_dp_s_rstb, 1);
        }
    }
ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_phy_short_channel_mode_get(const phymod_phy_access_t *phy, uint32_t *enable, uint32_t *status)
{
    phymod_phy_inf_config_t config;
    uint16_t data = 0;
    uint16_t ip = 0, lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;
    *enable = 1;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    SESTO_GET_IP(phy, config, ip);

    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    if (ip == SESTO_MERLIN_CORE) {
        /* short_channel_mode can performed only on falcon side */
        return PHYMOD_E_PARAM;
    }

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            /* config RAM variable */
            SESTO_CHIP_FIELD_WRITE(pa, SES_M0ACCESS_ADDR_MASTER_DRAM_MEM_ADD, mst_dram_mem_add, (0x420 + (lane * 0x130)));
            SESTO_CHIP_FIELD_READ(pa, SES_M0ACCESS_DATA_MASTER_DRAM_RDATA, mst_dram_rdata, data);
            *enable &= ((data >> 11) & 0x1);
        }
    }
ERR:
    SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return rv;
}

int _sesto_phy_eye_margin_est_get(const phymod_phy_access_t *phy, int *left_eye_mUI, int *right_eye_mUI, int *upper_eye_mV, int *lower_eye_mV)
{
    phymod_phy_inf_config_t config;
    
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    int rv = PHYMOD_E_NONE;
    const phymod_access_t *pa = &phy->access;
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(SESTO_DEVICE_AUX_MODE_T), "sesto_device_aux_mode");
    SESTO_IF_ERR_RETURN(
       _sesto_phy_interface_config_get(phy, 0, &config));

    SESTO_GET_IP(phy, config, ip);
    max_lane = (ip == SESTO_FALCON_CORE) ?
                   SESTO_MAX_FALCON_LANE : SESTO_MAX_MERLIN_LANE;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_DEBUG_VERBOSE(("%s::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
                (ip == SESTO_MERLIN_CORE)?"MERLIN":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane++) {
        if ((lane_mask & (1 << lane))) {
            SESTO_IF_ERR_RETURN (
                _sesto_set_slice_reg (pa, SESTO_SLICE_UNICAST, ip,
                     SESTO_DEV_PMA_PMD, 0, lane));
            PHYMOD_DEBUG_VERBOSE(("Phy Diagnostics for Lane:%d \n",lane));
            if(ip == SESTO_FALCON_CORE) {
                SESTO_IF_ERR_RETURN (
                    _falcon_furia_sesto_eye_margin_est_get(pa,left_eye_mUI,right_eye_mUI,upper_eye_mV,lower_eye_mV));
                 break;
            } else {
                SESTO_IF_ERR_RETURN (
                     _merlin_sesto_eye_margin_est_get(pa,left_eye_mUI,right_eye_mUI,upper_eye_mV,lower_eye_mV));
        break;

            }
        }
    }
    ERR:
       SESTO_RESET_SLICE(pa, SESTO_DEV_PMA_PMD);
       PHYMOD_FREE(config.device_aux_modes);
   return rv;
}


