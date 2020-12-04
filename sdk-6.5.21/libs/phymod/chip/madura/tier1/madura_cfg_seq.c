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
#include "bcmi_madura_defs.h"
#include "madura_cfg_seq.h"
#include "madura_sdk_interface.h"
#include "madura_serdes/common2/srds_api_enum.h"
#include "madura_serdes/falcon2_madura_src/falcon2_madura_functions.h"

extern unsigned char madura_falcon_ucode[];
extern unsigned char madura_falcon_slave_ucode[];
extern unsigned char madura_falcon_flashing_ucode[];
extern unsigned int madura_falcon_ucode_len;
extern unsigned int madura_falcon_ucode_slave_len;
extern unsigned int madura_falcon_flashing_ucode_len;

int read_1G_mode_status(const phymod_access_t *pa, unsigned char mindex, uint16_t *pll_index );

int read_madura_pll_mode(const phymod_access_t *pa, uint16_t ip, uint16_t lane_no, unsigned char mindex)
{
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PLL_CAL_CTL7r_t pll_read_val;
    uint16_t pll_index = 0, rv = 0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&pll_read_val, 0 , sizeof(BCMI_MADURA_PLL_CAL_CTL7r_t));

    rv = read_1G_mode_status(pa, mindex, &pll_index ); /*not using the return value. Only using the pll_index */
    if(rv){
        PHYMOD_DEBUG_VERBOSE((" Speed configured as 1G with mode register value as 0x%x",mindex));
    }else{
        PHYMOD_DEBUG_VERBOSE((" Speed not configured as 1G with mode register value as 0x%x",mindex));
    }
    PHYMOD_IF_ERR_RETURN (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD,lane_no, pll_index));
    PHYMOD_IF_ERR_RETURN (
    BCMI_MADURA_READ_PLL_CAL_CTL7r(pa,&pll_read_val));
    return (BCMI_MADURA_PLL_CAL_CTL7r_GET(pll_read_val) & 0x7 );
}

/**   This function is to read if 1G is configured and also to check which PLL that mode is using
 *
 *    @param pa                 Pointer to phymod access structure
 *    @param mindex             mode register value for 1G/10G/25G
 *    @param pll_index          Pll used by the mode i.e 0 or 1
 *
 *    @return 0 or 1            0 if mode is not in 1G and 1 if mode is in 1G
 */

int read_1G_mode_status(const phymod_access_t *pa, unsigned char mindex, uint16_t *pll_index )
{
    BCMI_MADURA_MST_CFG0r_t mst_cfg0;
    uint16_t mst_cfg_val = 0 ;
    int rv = 0;
    *pll_index = 0;

    PHYMOD_MEMSET(&mst_cfg0, 0 , sizeof(BCMI_MADURA_MST_CFG0r_t));

    PHYMOD_IF_ERR_RETURN (
            BCMI_MADURA_READ_MST_CFG0r(pa,&mst_cfg0));
    mst_cfg_val = BCMI_MADURA_MST_CFG0r_GET(mst_cfg0);

    switch( mindex)
    {
    case MADURA_MIDX_10GPT_0:
    case MADURA_MIDX_10GPT_0A:
        *pll_index = (( mst_cfg_val & 0x1)==0x1 )? 1 : 0 ;
        rv = (( mst_cfg_val & 0x100)==0x100 )? 1 : 0 ;
      break;
    case MADURA_MIDX_10GPT_1:
    case MADURA_MIDX_10GPT_1A:
        *pll_index = (( mst_cfg_val & 0x2)==0x2 )? 1 : 0 ;
        rv = (( mst_cfg_val & 0x200)==0x200 )? 1 : 0 ;
    break;
    case MADURA_MIDX_10GPT_2:
    case MADURA_MIDX_10GPT_2A:
        *pll_index = (( mst_cfg_val & 0x4)==0x4 )? 1 : 0 ;
        rv = (( mst_cfg_val & 0x400)==0x400 )? 1 : 0 ;
    break;
    case MADURA_MIDX_10GPT_3:
    case MADURA_MIDX_10GPT_3A:
        *pll_index = (( mst_cfg_val & 0x8)==0x8 )? 1 : 0 ;
        rv = (( mst_cfg_val & 0x800)==0x800 )? 1 : 0 ;
        break;
    default:
        break;
    }

    return rv;
}


int madura_get_chipid (const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev)
{
    BCMI_MADURA_CHIP_IDr_t cid;
    BCMI_MADURA_CHIP_REVISIONr_t rid;

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CHIP_IDr(pa, &cid));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CHIP_REVISIONr(pa, &rid));

    *chipid = (BCMI_MADURA_CHIP_REVISIONr_CHIP_ID_19_16f_GET(rid) << 16 ) |
               BCMI_MADURA_CHIP_IDr_GET(cid);
    *rev = BCMI_MADURA_CHIP_REVISIONr_CHIP_REVf_GET(rid);

    PHYMOD_DEBUG_VERBOSE(("CHIP ID: %x REv:%x\n", *chipid, *rev));
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
int _madura_wait_mst_msgout(const phymod_access_t *pa,
                            MADURA_MSGOUT_E exp_message,
                            int poll_time)
{
    int retry_count = MADURA_FW_DLOAD_RETRY_CNT;
    MADURA_MSGOUT_E msgout = MSGOUT_DONTCARE;/*0*/
    BCMI_MADURA_MST_MSGOUTr_t msgout_t;
    do {
        /* Read general control msg out  Register */
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MSGOUTr(pa,&msgout_t));
        msgout = BCMI_MADURA_MST_MSGOUTr_MST_MSGOUT_VALf_GET(msgout_t);
        /* wait before reading again */
        if (poll_time != 0) {
            /* sleep for specified seconds*/
            PHYMOD_SLEEP(poll_time);
        }
        retry_count --;
    } while ((msgout != exp_message) &&  retry_count);

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

int _madura_fw_enable(const phymod_access_t *pa, uint16_t en_dis)
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,en_dis);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));

    return PHYMOD_E_NONE;
}
int madura_micro_download(const phymod_access_t *pa, unsigned char *ucode,
                         uint32_t len, uint16_t master, uint16_t *chk_sum)
{

    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_MST_MSGINr_t msg_in;
    BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t mst_byte_cnt;
    BCMI_MADURA_MST_RUNNING_CHKSUMr_t run_chksum;
    BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t slv1_chksum;
    BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t slv2_chksum;
    BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t slv3_chksum;
    BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t slv4_chksum;

    uint16_t num_bytes = 0, j = 0;
    uint16_t num_words = 0;
    uint16_t check_sum = 0, cnt = MADURA_FW_DLOAD_RETRY_CNT, data = 0;

    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&msg_in, 0 , sizeof(BCMI_MADURA_MST_MSGINr_t));
    PHYMOD_MEMSET(&mst_byte_cnt, 0 , sizeof(BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t));
    PHYMOD_MEMSET(&run_chksum, 0 , sizeof(BCMI_MADURA_MST_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv1_chksum, 0 , sizeof(BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv2_chksum, 0 , sizeof(BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv3_chksum, 0 , sizeof(BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv4_chksum, 0 , sizeof(BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t));

    /* Send boot address if master enabled */
    if(master){
        PHYMOD_IF_ERR_RETURN(
                _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

        /* coverity[operator_confusion] */
        BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,0);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    }
    PHYMOD_IF_ERR_RETURN(
        _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

    /* Calculate the number of words */
    num_words = (len) / 2;

    /* Update message in value field with word length */
    /* coverity[operator_confusion] */
    BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,num_words);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    num_bytes = len;
    for (j = 0; j < num_bytes; j += 2) {
    /*sending next ucode data*/
        data = (ucode[j + 1] << 8) | ucode[j];
        check_sum ^= ucode[j] ^ ucode[j + 1];

        /* Send next word */
    /* coverity[operator_confusion] */
    BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,data);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
    }
    do {
        /* check download_done flags*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_UC_BOOT_PORr(pa,&boot_por));
        if (master) {
        if(BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por)==1){
                PHYMOD_DEBUG_VERBOSE(("Master Dload Done\n"));
                break;
            }
        } else {
        if(BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por)==0xb){
                PHYMOD_DEBUG_VERBOSE(("Slave Dload Done\n"));
                break;
            }
        }
    } while (--cnt);

    if (!cnt) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_INIT, (_PHYMOD_MSG("Firmware download failed")));
    }

    /* check checksum */
    check_sum = (check_sum == 0) ? 0x600D : check_sum;
    if (master)  {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_RUNNING_BYTE_CNTr(pa,&mst_byte_cnt));
       data=BCMI_MADURA_MST_RUNNING_BYTE_CNTr_MST_RUNNING_BYTE_CNT_VALf_GET(mst_byte_cnt);
        if (data != num_bytes) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,(_PHYMOD_MSG("Master Dload fail:")));
       }

        PHYMOD_DEBUG_VERBOSE(("Master Byte Cnt:%d\n", data));

        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_RUNNING_CHKSUMr(pa,&run_chksum ));

        data=BCMI_MADURA_MST_RUNNING_CHKSUMr_MST_RUNNING_CHKSUM_VALf_GET(run_chksum);
        if (check_sum != data) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                (_PHYMOD_MSG("Master Dload chksum Fail")));
        }
        PHYMOD_DEBUG_VERBOSE(("Master Chk sum:0x%x\n", data));
        *chk_sum = data;
        } else {
        /*slave checksum*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_SLV1_RUNNING_CHKSUMr(pa,&slv1_chksum));
        data=BCMI_MADURA_SLV1_RUNNING_CHKSUMr_SLV1_RUNNING_CHKSUM_VALf_GET(slv1_chksum);
        if (check_sum != data) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("Slave1 Dload chksum Fail")));
        }
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_SLV2_RUNNING_CHKSUMr(pa,&slv2_chksum));
        data=BCMI_MADURA_SLV2_RUNNING_CHKSUMr_SLV2_RUNNING_CHKSUM_VALf_GET(slv2_chksum);
        if (check_sum != data) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("Slave2 Dload chksum Fail")));
        }
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_SLV4_RUNNING_CHKSUMr(pa,&slv4_chksum));
        data=BCMI_MADURA_SLV4_RUNNING_CHKSUMr_SLV4_RUNNING_CHKSUM_VALf_GET(slv4_chksum);
        if (check_sum != data) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                    (_PHYMOD_MSG("Slave4 Dload chksum Fail")));
        }

         }
    PHYMOD_DEBUG_VERBOSE(("Slave cksum Cnt:0x%x\n", data));
        *chk_sum = data;
    return PHYMOD_E_NONE;
}


int madura_eeprom_download(const phymod_access_t *pa,
        const unsigned char *ucode,
        uint32_t len, uint16_t master, uint16_t *chk_sum,uint16_t prg_eeprom)
{

    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_MST_MSGINr_t msg_in;
    BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t mst_byte_cnt;
    BCMI_MADURA_MST_RUNNING_CHKSUMr_t run_chksum;
    BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t slv1_chksum;
    BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t slv2_chksum;
    BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t slv3_chksum;
    BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t slv4_chksum;

    uint16_t num_bytes = 0, j = 0;
    uint16_t num_words = 0;
    uint16_t check_sum = 0 /*, cnt = MADURA_FW_DLOAD_RETRY_CNT*/, data = 0;

    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&msg_in, 0 , sizeof(BCMI_MADURA_MST_MSGINr_t));
    PHYMOD_MEMSET(&mst_byte_cnt, 0 , sizeof(BCMI_MADURA_MST_RUNNING_BYTE_CNTr_t));
    PHYMOD_MEMSET(&run_chksum, 0 , sizeof(BCMI_MADURA_MST_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv1_chksum, 0 , sizeof(BCMI_MADURA_SLV1_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv2_chksum, 0 , sizeof(BCMI_MADURA_SLV2_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv3_chksum, 0 , sizeof(BCMI_MADURA_SLV3_RUNNING_CHKSUMr_t));
    PHYMOD_MEMSET(&slv4_chksum, 0 , sizeof(BCMI_MADURA_SLV4_RUNNING_CHKSUMr_t));

    /* Calculate the number of words */
    num_words = (len) / 2;
    num_bytes = len;
    if(!prg_eeprom){/*step 1 shawial check !: downloading flashing code to coderam*/
        PHYMOD_IF_ERR_RETURN(
                _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));
        /* Send data count */
        /* coverity[operator_confusion] */
        BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,num_words);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
         /*send data*/
        for (j = 0; j < num_bytes; j += 2) {
            /*sending next ucode data*/
            data = (ucode[j + 1] << 8) | ucode[j];
            check_sum ^= ucode[j] ^ ucode[j + 1];

            /* Send next word */
            /* coverity[operator_confusion] */
            BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,data);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
                    PHYMOD_USLEEP(500);
        }

        /* check checksum ; hardcoded*/
        check_sum =  0x600D ;
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_RUNNING_CHKSUMr(pa,&run_chksum ));
        data=BCMI_MADURA_MST_RUNNING_CHKSUMr_MST_RUNNING_CHKSUM_VALf_GET(run_chksum);
        if (check_sum != data) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
                (_PHYMOD_MSG("Master Dload chksum Fail")));
        }

    }else{/*step 2 and 3*/

        PHYMOD_IF_ERR_RETURN(
                _madura_wait_mst_msgout(pa, MSGOUT_FLASH, 0));
        /* coverity[operator_confusion] */
        BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,0);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
        PHYMOD_USLEEP(20*1000);
        PHYMOD_IF_ERR_RETURN(
                _madura_wait_mst_msgout(pa, MSGOUT_GET_CNT, 0));

        /* Update message in value field with word length */
        /* coverity[operator_confusion] */
        BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,num_words);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));

        PHYMOD_IF_ERR_RETURN(
                _madura_wait_mst_msgout(pa, MSGOUT_GET_CNT, 0));
        if(master){
            /* coverity[operator_confusion] */
            BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,0);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
        }else
        {
            /* coverity[operator_confusion] */
            BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,0x4000);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
        }

        for (j = 0; j < num_bytes; j += 2) {
            /*sending next ucode data*/
            data = (ucode[j + 1] << 8) | ucode[j];
            check_sum ^= ucode[j] ^ ucode[j + 1];

            if (j % 64  == 0)
            {
                PHYMOD_USLEEP(25 * 500);
            }
            PHYMOD_IF_ERR_RETURN(
                    _madura_wait_mst_msgout(pa, MSGOUT_NEXT, 0));

            /* Send next word */
            /* coverity[operator_confusion] */
            BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in,data);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_MST_MSGINr(pa,msg_in));
        }


        /* check checksum */
        check_sum = (check_sum == 0) ? 0x600D : check_sum;
    }
    return PHYMOD_E_NONE;
}

int madura_download_helper (
    const phymod_access_t     *pm_acc ,
    const unsigned char       *image,
    int                       image_len,
    int                       master_en,
    int                       slave_en,
    int                       code_broadcast_en,
    uint16_t                  mst_boot_addr,
    int                       prg_eeprom,
    int                       eeprom_download
)
{
    BCMI_MADURA_BOOTr_t boot;
    uint16_t data1 = 0, retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    uint16_t                  mst_checksum_out;
    uint16_t                  slv_checksum_out;
    uint16_t                  done = 0;
    BCMI_MADURA_GEN_CTL2r_t gen_ctrl2;
    BCMI_MADURA_SPI_CODE_LOAD_ENr_t spi_code_load;
    BCMI_MADURA_MST_MSGINr_t msg_in;
    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_FIRMWARE_VERSIONr_t fw_ver;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    BCMI_MADURA_FIRMWARE_FEATURESr_t firm_features;

    PHYMOD_MEMSET(&firm_features, 0, sizeof(BCMI_MADURA_FIRMWARE_FEATURESr_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&fw_ver, 0 , sizeof(BCMI_MADURA_FIRMWARE_VERSIONr_t));
    PHYMOD_MEMSET(&boot, 0 , sizeof(BCMI_MADURA_BOOTr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0 , sizeof(BCMI_MADURA_GEN_CTL2r_t));
    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(BCMI_MADURA_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&msg_in, 0 , sizeof(BCMI_MADURA_MST_MSGINr_t));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));

    /*validation : cfg die loop for each die L1050*/
    if (!prg_eeprom || slave_en) {/*check ! bitor for valid prg_eeprom = 1 for step 2 3 & 4; slave_en=1 for step3*/
       /*true for step 1 and 3*/
       /* Put Master under Reset */
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
        BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }

    if (!prg_eeprom) {
        /* Put Master under Reset for step 1*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
        BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro perip*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
        data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot);
    } while((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

        /* Madura always supports serial mode if Dload
         * Master followed by slave
         * Configure master_code_download_en, slave_code_download_en
         * and broadcast enable */
        if (eeprom_download) {
            BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load,0xB);/*Only 3 cores are supported for now*/
            BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load,1);
        } else
        if(slave_en){
            BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load,0xB);/*Only 3 cores are supported for now*/
        } else if(master_en){
            BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load,1);
            BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load,0);/*0 cores are supported */
        }
        BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_SET(spi_code_load,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SPI_CODE_LOAD_ENr(pm_acc,spi_code_load));

        /*Read values what is set above*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_SPI_CODE_LOAD_ENr(pm_acc,&spi_code_load));
        if (BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_GET(spi_code_load) != 1
        && BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_GET(spi_code_load)!= 1
                && BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_GET(spi_code_load) != 0xB) {
           /* return PHYMOD_E_INTERNAL; */
        }


    /* Check the not downloading branch of the flow digram
     * before writing to serboot.
     * wait_mst_msgout(chip_cfg, die, MSGOUT_NOT_DWNLD, 1, 0);*/

    /* Read / Write boot_por
     *  write to mst_dwld_done and slv_dwld_done.
     *  write to spi_port_used
     *  write to serboot */
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
    /* Force Master New Download */
    if (master_en)
        BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_SET(boot_por,0);
    /* Force Slave New Download */
    if (slave_en)
        BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_SET(boot_por,0);
    BCMI_MADURA_UC_BOOT_PORr_SERBOOTf_SET(boot_por,1);
    BCMI_MADURA_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por,eeprom_download);/*set to 0 for mdio default download on reset*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_UC_BOOT_PORr(pm_acc,boot_por));


    /* Releasing Master under Reset */
    if (!prg_eeprom) {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
        BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,1);/*release resetting M0 micro perip*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }
    if (!prg_eeprom || slave_en) {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
        BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,1);/*release resetting M0 micro*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }


    /* BEGIN : download_sequence */

    if (eeprom_download != 0) {/*Step 4*/
        PHYMOD_USLEEP(500*1000);/*cant touch this */
        PHYMOD_IF_ERR_RETURN(
            _madura_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));

    } else{
        /* Send boot address if master enabled */
        if(master_en && !prg_eeprom){
            PHYMOD_IF_ERR_RETURN(
                    _madura_wait_mst_msgout(pm_acc, MSGOUT_NEXT, 0));

            /* coverity[operator_confusion] */
            BCMI_MADURA_MST_MSGINr_MST_MSGIN_VALf_SET(msg_in, 0 /*mst_boot_addr*/);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_MST_MSGINr(pm_acc,msg_in));
        }
            PHYMOD_IF_ERR_RETURN(
                        PHYMOD_BUS_WRITE(pm_acc,0x18000,0x500));
        /* Send Image */
        if (!prg_eeprom) {  /* sending flashing code : step 1*/

            PHYMOD_IF_ERR_RETURN(
                    madura_eeprom_download(
                        pm_acc, image, image_len, 1, &mst_checksum_out,prg_eeprom));

        } else {     /*step 2 and step 3*/
            if (master_en) {
                PHYMOD_IF_ERR_RETURN(
                        madura_eeprom_download(
                            pm_acc, image, image_len, 1, &mst_checksum_out,prg_eeprom));

            } else {
                PHYMOD_IF_ERR_RETURN(
                        madura_eeprom_download(
                            pm_acc, image, image_len, 0, &slv_checksum_out,prg_eeprom));

            }
        }
        /* Wait for Program DONE*/
        if ( prg_eeprom ) {
            PHYMOD_USLEEP(500*1000);
            _madura_wait_mst_msgout(pm_acc, MSGOUT_PRGRM_DONE,0);
        }else{

                 do{
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
        /* Force Master New Download */
        if (master_en)
            done = BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por);
        /* Force Slave New Download */
        if (slave_en)
            done = BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por);
        }while(done != 1);
        }
    }

    /* END   : download_sequence */

     if (prg_eeprom && (eeprom_download == 0) && master_en) {
        /* Assert SPI Reset*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));

        BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,0);
        BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }
    /*copied from validation*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
    BCMI_MADURA_UC_BOOT_PORr_SERBOOTf_SET(boot_por,1);
    BCMI_MADURA_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por,prg_eeprom);/*set to 0 for mdio default download on reset*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_UC_BOOT_PORr(pm_acc,boot_por));

    if (eeprom_download != 0) {/*Step 4*/

        int data1;

        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_VERSIONr(pm_acc,&fw_ver));

        data1=BCMI_MADURA_FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(fw_ver);
        PHYMOD_DEBUG_VERBOSE(("FW Version:0x%x\n", data1));
    }
    return PHYMOD_E_NONE;
}



int _madura_firmware_download(const phymod_access_t *pm_acc ,
                            unsigned char *madura_master_ucode, unsigned int master_len,
                            unsigned char *madura_slave_ucode, unsigned int slave_len,
                            unsigned char prg_eeprom)
{
    uint16_t data1 = 0, retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    uint16_t eeprom_num_of_pages = 0;
    uint16_t mst_check_sum = 0, slv_check_sum = 0;
    BCMI_MADURA_SPI_CODE_LOAD_ENr_t spi_code_load;
    BCMI_MADURA_GEN_CTL2r_t gen_ctrl2;
    BCMI_MADURA_GEN_CTL3r_t gen_ctrl3;
    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    BCMI_MADURA_FIRMWARE_VERSIONr_t fw_ver;

    BCMI_MADURA_BOOTr_t boot;
    BCMI_MADURA_GPREGBr_t gregB;
    BCMI_MADURA_GPREGCr_t gregC;
    BCMI_MADURA_GPREGEr_t gregE;
    BCMI_MADURA_GPREGFr_t gregF;
    BCMI_MADURA_FIRMWARE_FEATURESr_t firm_features;

    PHYMOD_MEMSET(&firm_features, 0, sizeof(BCMI_MADURA_FIRMWARE_FEATURESr_t));
    PHYMOD_MEMSET(&spi_code_load, 0 , sizeof(BCMI_MADURA_SPI_CODE_LOAD_ENr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0 , sizeof(BCMI_MADURA_GEN_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0 , sizeof(BCMI_MADURA_GEN_CTL3r_t));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));
    PHYMOD_MEMSET(&boot, 0 , sizeof(BCMI_MADURA_BOOTr_t));
    PHYMOD_MEMSET(&gregB, 0 , sizeof(BCMI_MADURA_GPREGBr_t));
    PHYMOD_MEMSET(&gregC, 0 , sizeof(BCMI_MADURA_GPREGCr_t));
    PHYMOD_MEMSET(&gregE, 0 , sizeof(BCMI_MADURA_GPREGEr_t));
    PHYMOD_MEMSET(&gregF, 0 , sizeof(BCMI_MADURA_GPREGFr_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&fw_ver, 0 , sizeof(BCMI_MADURA_FIRMWARE_VERSIONr_t));

    /* Skip the firmware download
     *  mst_download, slv_dwld_done bits are set
     */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
   if((BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por))&&
    (BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por))){
     return PHYMOD_E_NONE; /*force DOWNLOAD*/
    }


   /* Put Master under Reset */
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro*/
    BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro perip*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

    /* Wait for any pending SPI download
     * SPI download is not interrupted by Master Reset
     * (this is a safety feature) so we need to wait its
     * completion before starting the MDIO Download */
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
        data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot);
    } while((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }

    /* Madura always supports serial mode if Dload
     * Master followed by slave
     * Configure master_code_download_en, slave_code_download_en
     * and broadcast enable */
    BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_SET(spi_code_load,0xB);/*Only 3 cores are supported for now*/
    BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_SET(spi_code_load,1);
    BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_SET(spi_code_load,1);

    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SPI_CODE_LOAD_ENr(pm_acc,spi_code_load));

    /*Read values what is set above*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_SPI_CODE_LOAD_ENr(pm_acc,&spi_code_load));
    if (BCMI_MADURA_SPI_CODE_LOAD_ENr_CODE_BROADCAST_ENf_GET(spi_code_load) != 1
    && BCMI_MADURA_SPI_CODE_LOAD_ENr_MST_CODE_DOWNLOAD_ENf_GET(spi_code_load)!= 1
            && BCMI_MADURA_SPI_CODE_LOAD_ENr_SLV_CODE_DOWNLOAD_ENf_GET(spi_code_load) != 0xB) {
        return PHYMOD_E_INTERNAL;
    }
    /* Configure GPREG for programming eeprom */
    if (prg_eeprom) {
        eeprom_num_of_pages = (uint16_t)master_len/MADURA_M0_EEPROM_PAGE_SIZE;

        /* Start Page of EEPROM for master*/
    BCMI_MADURA_GPREGEr_CLR(gregE);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_GPREGEr(pm_acc,gregE));/*set to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GPREGFr(pm_acc,&gregF));
    BCMI_MADURA_GPREGFr_GPREGF_DATAf_SET(gregF,eeprom_num_of_pages);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_GPREGFr(pm_acc,gregF));/*set to number of pages*/

    /* Start Page of EEPROM for slave*/
    BCMI_MADURA_GPREGBr_CLR(gregB);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_GPREGBr(pm_acc,gregB));/*set to 0*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GPREGCr(pm_acc,&gregC));
    BCMI_MADURA_GPREGCr_GPREGC_DATAf_SET(gregC,eeprom_num_of_pages);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_GPREGCr(pm_acc,gregC));/*set to number of pages*/

    /* Reset spi */
    /* Assert Reset */
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,0);/*resetting SPI2x rstb*/
    BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,0);/*resetting SPI rstb*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

    /* Deassert Reset */
    BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,1);
    BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }
    /* Select SPI Speed*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL3r(pm_acc,&gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_UCSPI_SLOWf_SET(gen_ctrl3,1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL3r(pm_acc,gen_ctrl3));

    /* Check the not downloading branch of the flow digram
     * before writing to serboot.
     * wait_mst_msgout(chip_cfg, die, MSGOUT_NOT_DWNLD, 1, 0);*/

    /* Read / Write boot_por
     *  write to mst_dwld_done and slv_dwld_done.
     *  write to spi_port_used
     *  write to serboot */
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
    /* Force Master New Download */
    BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_SET(boot_por,0);
    /* Force Slave New Download */
    BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_SET(boot_por,0);
    BCMI_MADURA_UC_BOOT_PORr_SERBOOTf_SET(boot_por,1);
    BCMI_MADURA_UC_BOOT_PORr_SPI_PORT_USEDf_SET(boot_por,prg_eeprom);/*set to 0 for mdio default download on reset*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_UC_BOOT_PORr(pm_acc,boot_por));

    /* RELEASE Master under Reset */
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,1);
    BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));

    /* MDI/I2C Download */
    /* Waiting for serboot_busy */
    retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
        data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot);
    } while ((data1 == 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_FAIL,
              (_PHYMOD_MSG("ERR:SERBOOT BUSY BIT SET")));
        return PHYMOD_E_FAIL;
    }
    /* Start download sequence */
    /* master boot*/
    PHYMOD_IF_ERR_RETURN(
       madura_micro_download(pm_acc, madura_master_ucode,
                             master_len, 1, &mst_check_sum));

    PHYMOD_IF_ERR_RETURN(
       madura_micro_download(pm_acc, madura_slave_ucode,
                             slave_len, 0, &slv_check_sum));
    PHYMOD_IF_ERR_RETURN (
      _madura_wait_mst_msgout(pm_acc, MSGOUT_DWNLD_DONE, 0));

    /* Check serboot_busy and serboot_done_once*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_BOOTr(pm_acc,&boot));
    data1=BCMI_MADURA_BOOTr_SERBOOT_BUSYf_GET(boot);
    if (data1 != 0) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT BUSY HAS UNEXPECTED VALUE\n"));
    }
    data1=BCMI_MADURA_BOOTr_SERBOOT_DONE_ONCEf_GET(boot);
    if (data1 != 1) {
        PHYMOD_DEBUG_ERROR(("WARN:SERBOOT DONE ONCE HAS UNEXPECTED VALUE\n"));
    }

    /* Wait for Program EEPROM*/
    if (prg_eeprom) {
        _madura_wait_mst_msgout(pm_acc, MSGOUT_PRGRM_DONE,0);
        /* Assert SPI Reset*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL2r(pm_acc,&gen_ctrl2));
    BCMI_MADURA_GEN_CTL2r_SPI2X_RSTBf_SET(gen_ctrl2,0);/*resetting SPI2x rstb*/
    BCMI_MADURA_GEN_CTL2r_SPI_RSTBf_SET(gen_ctrl2,0);/*resetting SPI rstb*/
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL2r(pm_acc,gen_ctrl2));
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_FIRMWARE_VERSIONr(pm_acc,&fw_ver));

    data1=BCMI_MADURA_FIRMWARE_VERSIONr_FIRMWARE_VERSION_VALf_GET(fw_ver);
    PHYMOD_DEBUG_VERBOSE(("FW Version:0x%x\n", data1));

    return PHYMOD_E_NONE;
}

int _madura_core_init(const phymod_core_access_t* core,
                     const phymod_core_init_config_t* init_config)
{
    int ret_val = 0;
    uint32_t rev = 0, chip_id = 0;
    phymod_access_t pa;
    const phymod_access_t *pm_acc = &core->access;
    BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_t pad_ctrl;
    BCMI_MADURA_GEN_CTL2r_t gen_ctrl2;
    BCMI_MADURA_FIRMWARE_FEATURESr_t firm_features;
    BCMI_MADURA_UC_BOOT_PORr_t   boot_por;

    /* Bring second die out of reset */
    PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
    pa.addr &= ~(1U);
    PHYMOD_IF_ERR_RETURN(
        PHYMOD_BUS_WRITE(&pa,0x18a50,0xa06));
    PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));

    /* clear vars */
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(BCMI_MADURA_GEN_CTL2r_t));
    PHYMOD_MEMSET(&pad_ctrl, 0, sizeof(BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_t));

    PHYMOD_MEMSET(&firm_features, 0, sizeof(BCMI_MADURA_FIRMWARE_FEATURESr_t));
    PHYMOD_MEMSET(&boot_por, 0 , sizeof(BCMI_MADURA_UC_BOOT_PORr_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_FIRMWARE_FEATURESr(pm_acc,&firm_features));

    switch(init_config->firmware_load_method)
    {
        case phymodFirmwareLoadMethodInternal:
            PHYMOD_DEBUG_VERBOSE((
               "Starting Firmware download through MDIO, it takes few seconds...\n"));

            /* In case of FW Download is only for second die i.e for odd address of the die
             * To get the micro out of reset on even die(previous die); for MDIO only
             * Applicable for only duel die chips;
             * MADURA_CHIP_ID_82790 is 82790 is single die ingnore the micro out of reset
             */
            PHYMOD_IF_ERR_RETURN(madura_get_chipid(pm_acc, &chip_id, &rev));
        PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
        if (PHYMOD_CORE_INIT_F_RESET_CORE_FOR_FW_LOAD_GET(init_config)) {

            /*if Die B.
              In case only DIE_B is selected:
              - put DIE_A M0 under reset
              - Release reset of DIE_B M0*/
            if (((pm_acc->addr & 0x1) == 0x1) && (chip_id != MADURA_CHIP_ID_82864)) {
                    /* modify the mdio address to be even */
                    pa.addr &= ~(0x1);
                    /* Set the 11th and 9th bit of even die to get the micro out of reset */

            /* Put DIE_A Master under Reset
             - This ensures that Die_A is not downloading at the same time of Die_B*/
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_READ_GEN_CTL2r(&pa,&gen_ctrl2));
            BCMI_MADURA_GEN_CTL2r_MST_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro*/
            BCMI_MADURA_GEN_CTL2r_MST_UCP_RSTBf_SET(gen_ctrl2,0);/*resetting M0 micro perip*/
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_GEN_CTL2r(&pa,gen_ctrl2));

            /*Release reset of Die_B*/
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_EXT_UC_RSTB_OUT_CTLr(&pa,&pad_ctrl));
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_OUT_FRCVALf_SET(pad_ctrl,1);
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_IBOFf_SET(pad_ctrl,1);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_EXT_UC_RSTB_OUT_CTLr(&pa,pad_ctrl));
            }else{
                /*ALL Dies*/
                /*Restore ext_uc_rstb in die A*/
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_EXT_UC_RSTB_OUT_CTLr(&pa,&pad_ctrl));
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_OUT_FRCVALf_SET(pad_ctrl,0);
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_IBOFf_SET(pad_ctrl,0);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_EXT_UC_RSTB_OUT_CTLr(&pa,pad_ctrl));
            }


            /* Do chip hard reset and Resetting the Cores */
            PHYMOD_IF_ERR_RETURN(
                _madura_core_reset_set(pm_acc, phymodResetModeHard, phymodResetDirectionInOut));

        } else if (PHYMOD_CORE_INIT_F_UNTIL_FW_LOAD_GET(init_config)) {

            /* Enable broadcast */
            PHYMOD_IF_ERR_RETURN(
            PHYMOD_BUS_WRITE(pm_acc,0x18204,0x1));

        } else if (PHYMOD_CORE_INIT_F_EXECUTE_FW_LOAD_GET(init_config)) {

            ret_val = _madura_firmware_download(pm_acc,
                               madura_falcon_ucode, madura_falcon_ucode_len,
                               madura_falcon_slave_ucode, madura_falcon_ucode_slave_len,
                               0);
            if ((ret_val != PHYMOD_E_NONE) && (ret_val != MADURA_FW_ALREADY_DOWNLOADED)) {
                PHYMOD_RETURN_WITH_ERR
                    (ret_val,
                    (_PHYMOD_MSG("firmware download failed")));
            } else {
                PHYMOD_DEBUG_VERBOSE(("Firmware download through MDIO success\n"));
            }

             PHYMOD_IF_ERR_RETURN (
                        _madura_cfg_fw_ull_dp(&core->access, init_config->op_datapath));

        } else if (PHYMOD_CORE_INIT_F_RESUME_AFTER_FW_LOAD_GET(init_config)) {

            /* Disable broadcast */
            PHYMOD_IF_ERR_RETURN(
            PHYMOD_BUS_WRITE(pm_acc,0x18204,0x0));

            /* check download_done flags again */
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_UC_BOOT_PORr(pm_acc,&boot_por));
            if (BCMI_MADURA_UC_BOOT_PORr_MST_DWLD_DONEf_GET(boot_por)!= 1 || BCMI_MADURA_UC_BOOT_PORr_SLV_DWLD_DONEf_GET(boot_por) != 1) {
                PHYMOD_DEBUG_VERBOSE(("FW download successfull \n"));
            }
        }

        break;
        case phymodFirmwareLoadMethodExternal:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodFirmwareLoadMethodNone:
        break;
        case phymodFirmwareLoadMethodProgEEPROM:
        PHYMOD_IF_ERR_RETURN(
         madura_download_helper (
                              pm_acc ,
                              madura_falcon_flashing_ucode,
                              madura_falcon_flashing_ucode_len,
                              1 /* master_en */,
                              0 /* slave_en */,
                              0 /* code_broadcast_en */,
                              0 /* mst_boot_addr */,
                              0 /* prg_eeprom */,
                              0 /* eeprom_download */
                              ));
        PHYMOD_IF_ERR_RETURN(
        madura_download_helper (
                              pm_acc ,
                              madura_falcon_ucode,
                              madura_falcon_ucode_len,
                              1 /* master_en */,
                              0 /* slave_en */,
                              0 /* code_broadcast_en */,
                              0 /* mst_boot_addr */,
                              1 /* prg_eeprom */,
                              0 /* eeprom_download */
                              ));

        PHYMOD_IF_ERR_RETURN(
            madura_download_helper (
                              pm_acc ,
                              madura_falcon_slave_ucode,
                              madura_falcon_ucode_slave_len,
                              0 /* master_en */,
                              1 /* slave_en */,
                              0 /* code_broadcast_en */,
                              0 /* mst_boot_addr */,
                              1 /* prg_eeprom */,
                              0 /* eeprom_download */
                              ));
        /*ALL Dies*/
            /*Restore ext_uc_rstb in die A*/
        PHYMOD_MEMCPY(&pa, pm_acc, sizeof(phymod_access_t));
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_EXT_UC_RSTB_OUT_CTLr(&pa,&pad_ctrl));
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_OUT_FRCVALf_SET(pad_ctrl,0);
            BCMI_MADURA_EXT_UC_RSTB_OUT_CTLr_EXT_UC_RSTB_OUT_IBOFf_SET(pad_ctrl,0);
            PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_WRITE_EXT_UC_RSTB_OUT_CTLr(&pa,pad_ctrl));

        PHYMOD_IF_ERR_RETURN(
            madura_download_helper (
                              pm_acc ,
                              madura_falcon_flashing_ucode,
                              0 /* madura_falcon_flashing_ucode_len */,
                              1 /* master_en */,
                              1 /* slave_en */,
                              0 /* code_broadcast_en */,
                              0 /* mst_boot_addr */,
                              0 /* prg_eeprom : this is 0 in validation application*/,
                              1 /* eeprom_download */
                              ));


        break;
        default:
            PHYMOD_RETURN_WITH_ERR
                (PHYMOD_E_CONFIG,
                 (_PHYMOD_MSG("illegal fw load method")));
    }
    /*check if fw runs!*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_FIRMWARE_FEATURESr(pm_acc,&firm_features));

    /* Force clock to 0 temporarily */
    {

    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));

    BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x800);
    PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pm_acc,clk_ctrl));
    }
    return PHYMOD_E_NONE;
}

int _madura_set_slice_an_reg (
    const phymod_access_t* pm_acc,
    uint16_t               ip,
    uint16_t               dev_type,
    uint16_t               lane,
    uint16_t               port )
{
    BCMI_MADURA_AN_SLICEr_t slice_an_reg;
    uint16_t sys_sel=0,lin_sel=0,slice_0=0,slice_1=0;

    PHYMOD_MEMSET(&slice_an_reg, 0 , sizeof(BCMI_MADURA_AN_SLICEr_t));

    /*selecting sys or line side*/
    sys_sel = (ip==MADURA_FALCON_CORE)? 1 : 0;
    lin_sel = (ip==MADURA_FALCON_CORE)? 0 : 1;

    /*set lane*/
    if(lane<4)
    {
        slice_0=1;
    }else{
    slice_1=1;
    }


    if (dev_type == MADURA_DEV_AN) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_AN_SLICEr(pm_acc,&slice_an_reg));
        BCMI_MADURA_AN_SLICEr_SYS_SELf_SET(slice_an_reg,sys_sel);
        BCMI_MADURA_AN_SLICEr_LIN_SELf_SET(slice_an_reg,lin_sel);
    BCMI_MADURA_AN_SLICEr_SLICE_0_SELf_SET(slice_an_reg,slice_0);
    BCMI_MADURA_AN_SLICEr_SLICE_1_SELf_SET(slice_an_reg,slice_1);
        BCMI_MADURA_AN_SLICEr_REG_TYPE_SELf_SET(slice_an_reg,1);
        BCMI_MADURA_AN_SLICEr_PORT_SELf_SET(slice_an_reg,port);
        BCMI_MADURA_AN_SLICEr_LANE_SELf_SET(slice_an_reg,0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_AN_SLICEr(pm_acc,slice_an_reg));
    }

    return PHYMOD_E_NONE;
}

int _madura_set_slice_reg (
    const phymod_access_t* pm_acc,
    uint16_t               ip,
    uint16_t               dev_type,
    uint16_t               lane,
    uint16_t               pll_sel)
{
    BCMI_MADURA_SLICEr_t slice_reg;
    uint16_t sys_sel=0,lin_sel=0,lane_no=0,slice_0=0,slice_1=0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));

    /*selecting sys or line side*/
    sys_sel = (ip==MADURA_FALCON_CORE)? 1 : 0;
    lin_sel = (ip==MADURA_FALCON_CORE)? 0 : 1;

    /*set lane*/
    lane_no=1<<((lane%4));
    if(lane<4)
    {
        slice_0=1;
    }else{
        slice_1=1;
    }

    if (dev_type == MADURA_DEV_PMA_PMD) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_SLICEr(pm_acc,&slice_reg));
        BCMI_MADURA_SLICEr_PLL_SELf_SET(slice_reg,pll_sel);
        BCMI_MADURA_SLICEr_SYS_SELf_SET(slice_reg,sys_sel);
        BCMI_MADURA_SLICEr_LIN_SELf_SET(slice_reg,lin_sel);
        BCMI_MADURA_SLICEr_SLICE_0_SELf_SET(slice_reg,slice_0);
        BCMI_MADURA_SLICEr_SLICE_1_SELf_SET(slice_reg,slice_1);
        BCMI_MADURA_SLICEr_LANE_SELf_SET(slice_reg,lane_no);
        BCMI_MADURA_SLICEr_REG_TYPE_SELf_SET(slice_reg,0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_SLICEr(pm_acc,slice_reg));
    PHYMOD_DEBUG_VERBOSE(("\n[-- ip=%x slice = %x ===]\n",ip,BCMI_MADURA_SLICEr_GET(slice_reg)));
   }

    return PHYMOD_E_NONE;
}

int _madura_rx_pmd_lock_get(const phymod_access_t* pa, uint32_t* rx_pmd_locked)
{
    BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_t rx_lck;
    BCMI_MADURA_SLICEr_t slice_reg;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t temp = 0,intf_side=0;
    uint16_t lane_mask = 0, max_lane = 0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&rx_lck, 0 , sizeof(BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_t));
    if (rx_pmd_locked) {
        *rx_pmd_locked = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));

            /*check pmd rx lock*/
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                  BCMI_MADURA_READ_TLB_RX_PMD_RX_LOCK_STSr(pa,&rx_lck)));

            temp=BCMI_MADURA_TLB_RX_PMD_RX_LOCK_STSr_PMD_RX_LOCKf_GET(rx_lck);

            *rx_pmd_locked &= temp;
        }
    }

    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}

int madura_is_lane_mask_set(const phymod_phy_access_t *pa, uint16_t prt,
                           const phymod_phy_inf_config_t *config)
{
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux;
    const phymod_access_t *acc = &pa->access;

    lane_mask = pa->access.lane_mask;
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    aux = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;
    MADURA_GET_IP(acc, cfg, core) ;
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d port:%d\n", __func__, core, prt));
    if (core == MADURA_2X_FALCON_CORE) {
        if ((cfg.data_rate == MADURA_SPD_40G)||(cfg.data_rate == MADURA_SPD_100G) ||
            (cfg.data_rate == MADURA_SPD_50G)||
            (cfg.data_rate == MADURA_SPD_42G)||(cfg.data_rate == MADURA_SPD_106G)) {
            if (prt == 0) {
                /* coverity[identical_branches] */
                if (aux->pass_thru_dual_lane) {
                    if(!aux->alternate){
                        return (lane_mask  == 0x3) ? 1 : 0;
                    } else {
                        return (lane_mask  == 0x30) ? 1 : 0;
                    }
                } else if (aux->pass_thru) {
                    if(!aux->alternate){
                        return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                    }else{
                        return ((lane_mask & 0xf0) == 0xF0) ? 1 : 0 ;
                    }
                } else {
                    return ((lane_mask & 0xF) == 0xF) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru_dual_lane) {
                    if(!aux->alternate){
                        return (lane_mask  == 0xC) ? 1 : 0;
                    } else {
                        return (lane_mask  == 0xC0) ? 1 : 0;
                    }
                } else if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xF0) == 0xf0) ? 1 : 0;
                }
            }
        }
        if (cfg.data_rate == MADURA_SPD_10G || cfg.data_rate == MADURA_SPD_25G ||
            cfg.data_rate == MADURA_SPD_11G || cfg.data_rate == MADURA_SPD_1G) {
            /* For 10G PT and alternate modes*/
            if (prt == 0) {
                if (!aux->alternate) {
                    return (lane_mask & 0x1) ? 1 :0;
                }else{
                    return (lane_mask & 0x10) ? 1 :0;
                }
            } else if (prt == 1) {
                if (!aux->alternate) {
                    return (lane_mask & 0x2) ? 1 :0;
                }else{
                    return (lane_mask & 0x20) ? 1 :0;
                }
            } else if (prt == 2) {
                if (!aux->alternate) {
                    return (lane_mask & 0x4) ? 1 :0;
                } else {
                    return (lane_mask & 0x40) ? 1 :0;
                }
            } else if (prt == 3) {
                if (!aux->alternate) {
                    return (lane_mask & 0x8) ? 1 :0;
                } else {
                    return (lane_mask & 0x80) ? 1 :0;
                }
            }
        }
    } else { /*System SIDE*/
        if ((cfg.data_rate == MADURA_SPD_40G)||(cfg.data_rate == MADURA_SPD_100G) ||
            (cfg.data_rate == MADURA_SPD_50G)||
            (cfg.data_rate == MADURA_SPD_42G)||(cfg.data_rate == MADURA_SPD_106G)) {
            if (prt == 0) {
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0x3) ? 1 : 0;
                } else if (aux->pass_thru) {
                    return ((lane_mask & 0xf) == 0xF) ? 1 : 0 ;
                } else {
                    return ((lane_mask & 0x3) == 0x3) ? 1 : 0;
                }
            } else {
                if (aux->pass_thru_dual_lane) {
                    return (lane_mask  == 0xC) ? 1 : 0;
                } else if (aux->pass_thru) {
                    return 0;
                } else {
                    return ((lane_mask & 0xc) == 0xc) ? 1 : 0;
                }
            }
        }
        if (cfg.data_rate == MADURA_SPD_10G || cfg.data_rate == MADURA_SPD_25G ||
            cfg.data_rate == MADURA_SPD_11G || cfg.data_rate == MADURA_SPD_1G) {
             /* For 10G PT with  1 core disabled */
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
int _madura_config_dut_mode_reg(const phymod_phy_access_t *pa, const phymod_phy_inf_config_t* config)
{

    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    BCMI_MADURA_MST_CFG0r_t mst_cfg0;
    unsigned char              mindex0 = 0xFF;
    unsigned char              mindex1 = 0xFF;
    unsigned char              mindex2 = 0xFF;
    unsigned char              mindex3 = 0xFF;

    uint32_t alt_off = 0;
    uint16_t no_of_ports = 0, port_idx = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    uint16_t core = 0;
    uint16_t is_lane_used = 0, mst_cfg0_val = 0;
    const phymod_access_t *acc = &pa->access;
    phymod_phy_inf_config_t cfg;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_MADURA_P3_MODE_CTLr_t));
    PHYMOD_MEMSET(&mst_cfg0, 0, sizeof(BCMI_MADURA_MST_CFG0r_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    MADURA_GET_IP(acc, cfg, core);

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;

    if ((core == MADURA_2X_FALCON_CORE) && aux_mode->alternate ) {
        alt_off = 4;
    }

    MADURA_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_MST_CFG0r(&pa->access, &mst_cfg0));

    mst_cfg0_val = BCMI_MADURA_MST_CFG0r_GET(mst_cfg0);

    if(core == MADURA_2X_FALCON_CORE)
    {
    /*P1 is in Mux mode as set be the fw. SW needs to clear it always during startup*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P1_MODE_CTLr(&pa->access, &mode_ctrl1));
            if(MADURA_MIDX_40GDM_1B == BCMI_MADURA_P1_MODE_CTLr_P1_MINDEXf_GET(mode_ctrl1))
            {
                PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));
                PHYMOD_IF_ERR_RETURN(
                        BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
            }
    switch(pa->access.lane_mask)
    {
    case 0xF:
        mst_cfg0_val = 0;
        if(!(!aux_mode->pass_thru && (config->data_rate == MADURA_SPD_40G))) /* for Mux mode 1 3 0 else all */
        {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
        }else{
        /*In Mux mode*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P2_MODE_CTLr(&pa->access, &mode_ctrl2));
            if(!(MADURA_MIDX_40GDM_1B == BCMI_MADURA_P2_MODE_CTLr_P2_MINDEXf_GET(mode_ctrl2)) )
            {
                PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
                PHYMOD_IF_ERR_RETURN(
                        BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
            }
        }
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3));
        break;
    case 0xF0:
        mst_cfg0_val = 0;
        if(!(!aux_mode->pass_thru && (config->data_rate == MADURA_SPD_40G))) /* for Mux mode 1 3 2 else all */
        {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
        }else{
        /*In Mux mode*/
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P0_MODE_CTLr(&pa->access, &mode_ctrl0));
        if(!(MADURA_MIDX_40GDM_0B == BCMI_MADURA_P0_MODE_CTLr_P0_MINDEXf_GET(mode_ctrl0)))
             {
                 PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
                 PHYMOD_IF_ERR_RETURN(
                     BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
             }
        }
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3));
        break;
    case 0x3:
    case 0x30:
        mst_cfg0_val = 0;
        /*clear 0 1*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
        break;
    case 0xc:
    case 0xc0:
        mst_cfg0_val = 0;
        /*Clear 2 3*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3));
        break;

    default:
        break;
    }
    }


    if (config->data_rate != MADURA_SPD_100G &&
        config->data_rate != MADURA_SPD_106G) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P0_MODE_CTLr(&pa->access, &mode_ctrl0));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_P2_MODE_CTLr(&pa->access, &mode_ctrl2));
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == MADURA_SPD_10G  || config->data_rate == MADURA_SPD_25G  ||
               config->data_rate == MADURA_SPD_11G || cfg.data_rate == MADURA_SPD_1G ) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (1 << (port_idx + alt_off))) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if((config->data_rate == MADURA_SPD_40G || config->data_rate == MADURA_SPD_50G ||
                       config->data_rate == MADURA_SPD_42G) &&
                      aux_mode->pass_thru_dual_lane) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (0x3 << (alt_off + (port_idx * 2)))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
            } else if((config->data_rate == MADURA_SPD_40G || config->data_rate == MADURA_SPD_42G) &&
                      (!aux_mode->pass_thru)) {
                if (core == MADURA_2X_FALCON_CORE) {
                    /* MUX Mode*/
                    if (pa->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
           } else {
                is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }

            /* Program Modes */
            if (((config->data_rate == MADURA_SPD_40G  || config->data_rate == MADURA_SPD_42G || config->data_rate == MADURA_SPD_50G ) && (port_idx == 0)) ||
                 ((config->data_rate == MADURA_SPD_10G || config->data_rate == MADURA_SPD_11G || config->data_rate == MADURA_SPD_25G || cfg.data_rate == MADURA_SPD_1G ) && (port_idx < 2))) {
                if (config->data_rate == MADURA_SPD_40G) {
                  if (!aux_mode->alternate) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex0 = MADURA_MIDX_40GPT20_0;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex0 = MADURA_MIDX_40GPT10_0 ;
                    } else {
                        mindex0 = MADURA_MIDX_40GDM_0B ;
                   mindex3=0;
                    }
                  } else {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex0 = MADURA_MIDX_40GPT20_0A;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex0 = MADURA_MIDX_40GPT10_0A;
                    } else {
                        mindex0 = MADURA_MIDX_40GDM_0B ;
                   mindex3=0;
                    }
                  }
                  mindex1 = 0;
                }
                if (config->data_rate == MADURA_SPD_42G) {
                  if (!aux_mode->alternate) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex0 = MADURA_MIDX_40HGPT20_0;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex0 = MADURA_MIDX_40HGPT10_0 ;
                    } else {
                        mindex0 = MADURA_MIDX_40HGDM_0B ;
                   mindex3=0;
                    }
                  } else {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex0 = MADURA_MIDX_40HGPT20_0A;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex0 = MADURA_MIDX_40HGPT10_0A;
                    } else {
                        mindex0 = MADURA_MIDX_40HGDM_0B ;
                   mindex3=0;
                    }
                  }
                  mindex1 = 0;
                }
        if (config->data_rate == MADURA_SPD_50G) {
                  if (!aux_mode->alternate) {
                        mindex0 = MADURA_MIDX_40GPT20_0;
                   mindex3=0;
                  } else {
                        mindex0 = MADURA_MIDX_40GPT20_0A;
                   mindex3=0;
          }
        mindex1 = 0;
        }

        if ( cfg.data_rate == MADURA_SPD_1G ) { /*same as 10G mode registers */
                if ((pa->access.lane_mask & 0x1) || (pa->access.lane_mask & 0x10)) {
					  mst_cfg0_val &= ~(0x0001); /* set Pll to 0 by clearing in case of flex from 25G*/
                      mst_cfg0_val |= 0x100 ; /*enable 1G with default pll as 132 with pll_index as 0*/
                      if (!aux_mode->alternate) {
                        mindex0 = MADURA_MIDX_10GPT_0 ;
                      } else {
                        mindex0 = MADURA_MIDX_10GPT_0A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x2) || (pa->access.lane_mask & 0x20)) {
					  mst_cfg0_val &= ~(0x0002); /* set Pll to 0 by clearing in case of flex from 25G*/
                      mst_cfg0_val |= 0x200 ; /*enable 1G with default pll as 132 with pll_index as 0*/
                      if (!aux_mode->alternate) {
                        mindex1 = MADURA_MIDX_10GPT_1 ;
                      } else {
                        mindex1 = MADURA_MIDX_10GPT_1A;
                      }
                    }
                }

                if (config->data_rate == MADURA_SPD_25G || config->data_rate == MADURA_SPD_10G) {
                    if ((pa->access.lane_mask & 0x1) || (pa->access.lane_mask & 0x10)) {
                      mst_cfg0_val &= ~(0x100) ; /*cleare 1G */
                      if(config->data_rate == MADURA_SPD_25G ){
                           mst_cfg0_val |= (0x001) ; /*set pll index to 1 i.e 25G */
                      }else{
                           mst_cfg0_val &= ~(0x001); /*set pll index to 0 for 10G */
                      }
                      if (!aux_mode->alternate) {
                        mindex0 = MADURA_MIDX_10GPT_0 ;
                      } else {
                        mindex0 = MADURA_MIDX_10GPT_0A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x2) || (pa->access.lane_mask & 0x20)) {
                      mst_cfg0_val &= ~(0x200) ; /*cleare 1G */
                      if(config->data_rate == MADURA_SPD_25G ){
                          mst_cfg0_val |= (0x002) ; /*set pll index to 1 i.e 25G */
                      } else {
                          mst_cfg0_val &= ~(0x002); /*set pll index to 0 for 10G */
                      }
                     if (!aux_mode->alternate) {
                        mindex1 = MADURA_MIDX_10GPT_1 ;
                      } else {
                        mindex1 = MADURA_MIDX_10GPT_1A;
                      }
                    }
                }
                if (config->data_rate == MADURA_SPD_11G) {
                    if ((pa->access.lane_mask & 0x1) || (pa->access.lane_mask & 0x10)) {
                      if (!aux_mode->alternate) {
                        mindex0 = MADURA_MIDX_10HGPT_0 ;
                      } else {
                        mindex0 = MADURA_MIDX_10HGPT_0A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x2) || (pa->access.lane_mask & 0x20)) {
                      if (!aux_mode->alternate) {
                        mindex1 = MADURA_MIDX_10HGPT_1 ;
                      } else {
                        mindex1 = MADURA_MIDX_10HGPT_1A;
                      }
                    }
                }
            } else {
                if (config->data_rate == MADURA_SPD_40G) {
                  if (!aux_mode->alternate) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40GPT20_1;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex2 = MADURA_MIDX_40GPT10_0 ;
                    } else {
                        mindex2 = MADURA_MIDX_40GDM_1B ;
                   mindex3=0;
                    }
                  } else {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40GPT20_1A;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex2 = MADURA_MIDX_40GPT10_0A;
                    } else {
                        mindex2 = MADURA_MIDX_40GDM_1B ;
                   mindex3=0;
                    }
                  }
                }
                if (config->data_rate == MADURA_SPD_42G) {
                  if (!aux_mode->alternate) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40HGPT20_1;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex2 = MADURA_MIDX_40HGPT10_0 ;
                    } else {
                        mindex2 = MADURA_MIDX_40HGDM_1B ;
                   mindex3=0;
                    }
                  } else {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40HGPT20_1A;
                   mindex3=0;
                    } else if (aux_mode->pass_thru) {
                        mindex2 = MADURA_MIDX_40HGPT10_0A;
                    } else {
                        mindex2 = MADURA_MIDX_40HGDM_1B ;
                   mindex3=0;
                    }
                  }
                }
        if (config->data_rate == MADURA_SPD_50G) {
                  if (!aux_mode->alternate) {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40GPT20_1;
                   mindex3=0;
                    }
                  } else {
                    if (aux_mode->pass_thru_dual_lane) {
                        mindex2 = MADURA_MIDX_40GPT20_1A;
                   mindex3=0;
                    }
                  }
                }

                if (config->data_rate == MADURA_SPD_25G || config->data_rate == MADURA_SPD_10G) {
                    if ((pa->access.lane_mask & 0x4) || (pa->access.lane_mask & 0x40)) {
                      mst_cfg0_val &= ~(0x400) ; /*cleare 1G */
                      if(config->data_rate == MADURA_SPD_25G ){
                          mst_cfg0_val |= (0x004) ; /*set pll index to 1 i.e 25G */
                      } else {
                          mst_cfg0_val &= ~(0x004); /*set pll index to 0 for 10G */
                      }
                      if (!aux_mode->alternate) {
                        mindex2 = MADURA_MIDX_10GPT_2 ;
                      } else {
                        mindex2 = MADURA_MIDX_10GPT_2A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x8) || (pa->access.lane_mask & 0x80)) {
                      mst_cfg0_val &= ~(0x800) ; /*cleare 1G */
                      if(config->data_rate == MADURA_SPD_25G ){
                          mst_cfg0_val |= (0x008) ; /*set pll index to 1 i.e 25G */
                      } else {
                          mst_cfg0_val &= ~(0x008); /*set pll index to 0 for 10G */
                      }
                      if (!aux_mode->alternate) {
                        mindex3 = MADURA_MIDX_10GPT_3 ;
                      } else {
                        mindex3 = MADURA_MIDX_10GPT_3A;
                      }
                    }
                }
                if (config->data_rate == MADURA_SPD_11G) {
                    if ((pa->access.lane_mask & 0x4) || (pa->access.lane_mask & 0x40)) {
                      if (!aux_mode->alternate) {
                        mindex2 = MADURA_MIDX_10HGPT_2 ;
                      } else {
                        mindex2 = MADURA_MIDX_10HGPT_2A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x8) || (pa->access.lane_mask & 0x80)) {
                      if (!aux_mode->alternate) {
                        mindex3 = MADURA_MIDX_10HGPT_3 ;
                      } else {
                        mindex3 = MADURA_MIDX_10HGPT_3A;
                      }
                    }
                }
        if ( cfg.data_rate == MADURA_SPD_1G ) {
                    if ((pa->access.lane_mask & 0x4) || (pa->access.lane_mask & 0x40)) {
					  mst_cfg0_val &= ~(0x0004); /* set Pll to 0 by clearing in case of flex from 25G*/
                      mst_cfg0_val |= 0x400 ; /*enable 1G with default pll as 132 with pll_index as 0*/
                      if (!aux_mode->alternate) {
                        mindex2 = MADURA_MIDX_10GPT_2 ;
                      } else {
                        mindex2 = MADURA_MIDX_10GPT_2A;
                      }
                    }
                    if ((pa->access.lane_mask & 0x8) || (pa->access.lane_mask & 0x80)) {
					  mst_cfg0_val &= ~(0x0008); /* set Pll to 0 by clearing in case of flex from 25G*/
                      mst_cfg0_val |= 0x800 ; /*enable 1G with default pll as 132 with pll_index as 0*/
                      if (!aux_mode->alternate) {
                        mindex3 = MADURA_MIDX_10GPT_3 ;
                      } else {
                        mindex3 = MADURA_MIDX_10GPT_3A;
                      }
                    }
                }
            }
        }
    }

    if (mindex0 == 0xFF  && mindex2 == 0xFF ) {
        if (config->data_rate == MADURA_SPD_100G) {
            if (!aux_mode->alternate) {
                mindex0 = MADURA_MIDX_100GPT_0;
            } else {
                mindex0 = MADURA_MIDX_100GPT_0A;
            }
            mindex1 = 0;
        }
        if (config->data_rate == MADURA_SPD_106G) {
            if (!aux_mode->alternate) {
                mindex0 = MADURA_MIDX_100HGPT_0;
            } else {
                mindex0 = MADURA_MIDX_100HGPT_0A;
            }
            mindex1 = 0;
        }
    }

    PHYMOD_DEBUG_VERBOSE(("MODE CTRL0 :0x%x\n", mindex0));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL1 :0x%x\n", mindex1));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL2 :0x%x\n", mindex2));
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL3 :0x%x\n", mindex3));

    if (mindex0 != 0xFF) {
        BCMI_MADURA_P0_MODE_CTLr_P0_PVALIDf_SET(mode_ctrl0, 1);
        BCMI_MADURA_P0_MODE_CTLr_P0_MINDEXf_SET(mode_ctrl0, mindex0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
    }
    if (mindex1 != 0xFF) {
        if (config->data_rate == MADURA_SPD_10G || config->data_rate == MADURA_SPD_25G ||
            config->data_rate == MADURA_SPD_11G || cfg.data_rate == MADURA_SPD_1G ) {
            BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 1);
         }else{
            BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 0);
         }
        BCMI_MADURA_P1_MODE_CTLr_P1_MINDEXf_SET(mode_ctrl1, mindex1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
    }
    if (mindex2 != 0xFF) {
        BCMI_MADURA_P2_MODE_CTLr_P2_PVALIDf_SET(mode_ctrl2, 1);
        BCMI_MADURA_P2_MODE_CTLr_P2_MINDEXf_SET(mode_ctrl2, mindex2);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
    }
    if (mindex3 != 0xFF) {
        if (config->data_rate == MADURA_SPD_10G || config->data_rate == MADURA_SPD_25G ||
            config->data_rate == MADURA_SPD_11G|| cfg.data_rate == MADURA_SPD_1G ) {
            BCMI_MADURA_P3_MODE_CTLr_P3_PVALIDf_SET(mode_ctrl3, 1);
         }else{
                BCMI_MADURA_P3_MODE_CTLr_P3_PVALIDf_SET(mode_ctrl3, 0);
         }
        BCMI_MADURA_P3_MODE_CTLr_P3_MINDEXf_SET(mode_ctrl3, mindex3);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3));
    }

    BCMI_MADURA_MST_CFG0r_SET(mst_cfg0,mst_cfg0_val);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_MST_CFG0r(&pa->access, mst_cfg0));

    return PHYMOD_E_NONE;
}

/** Configure Dut Mode Register
 */
int _madura_udms_config(const phymod_phy_access_t *pa, const phymod_phy_inf_config_t* config,
                        MADURA_AN_MODE mode)
{
    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    unsigned char              udmsen0 = 0xFF;
    unsigned char              udmsen1 = 0xFF;
    unsigned char              udmsen2 = 0xFF;
    unsigned char              udmsen3 = 0xFF;
    unsigned char              dmept0 = 0xFF;
    unsigned char              dmept2 = 0xFF;

    uint32_t alt_off = 0;
    uint16_t no_of_ports = 0, port_idx = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    const phymod_access_t *acc;
    phymod_phy_inf_config_t cfg;
    uint16_t core = 0;
    uint16_t is_lane_used = 0;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl3, 0, sizeof(BCMI_MADURA_P3_MODE_CTLr_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));

    acc = &pa->access;
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)config->device_aux_modes;

    if ((core == MADURA_2X_FALCON_CORE) && aux_mode->alternate ) {
        alt_off = 4;
    }

    MADURA_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);
    MADURA_GET_IP(acc, cfg, core);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P0_MODE_CTLr(&pa->access, &mode_ctrl0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P1_MODE_CTLr(&pa->access, &mode_ctrl1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P2_MODE_CTLr(&pa->access, &mode_ctrl2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P3_MODE_CTLr(&pa->access, &mode_ctrl3));

    if (config->data_rate != MADURA_SPD_100G &&
        config->data_rate != MADURA_SPD_106G) {
        for (port_idx=0; port_idx < no_of_ports; port_idx++) {
            if(config->data_rate == MADURA_SPD_10G  ||config->data_rate == MADURA_SPD_25G  ||
               config->data_rate == MADURA_SPD_11G|| cfg.data_rate == MADURA_SPD_1G ) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (1 << (port_idx + alt_off))) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (1 << port_idx)) {
                       is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                }
            } else if((config->data_rate == MADURA_SPD_40G ||config->data_rate == MADURA_SPD_50G ||
                       config->data_rate == MADURA_SPD_42G) &&
                      aux_mode->pass_thru_dual_lane) {
                /* coverity[identical_branches] */
                if (core == MADURA_2X_FALCON_CORE) {
                    if (pa->access.lane_mask & (0x3 << (alt_off + (port_idx * 2)))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
            } else if((config->data_rate == MADURA_SPD_40G ||config->data_rate == MADURA_SPD_50G ||
                       config->data_rate == MADURA_SPD_42G) &&
                      (!aux_mode->pass_thru)) {
                if (core == MADURA_2X_FALCON_CORE) {
                    /* MUX Mode*/
                    if (pa->access.lane_mask & (0xF << (port_idx*4))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                       continue;
                    }
                } else {
                    if (pa->access.lane_mask & (0x3 << (port_idx * 2))) {
                        is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
                    } else {
                        continue;
                    }
                }
           } else {
                is_lane_used =  madura_is_lane_mask_set(pa, port_idx, config);
           }
           if (!is_lane_used) {
                return PHYMOD_E_PARAM;
           }

            /* Program Modes */
            if (((config->data_rate == MADURA_SPD_40G || config->data_rate == MADURA_SPD_42G || config->data_rate == MADURA_SPD_50G) && (port_idx == 0)) ||
                ((config->data_rate == MADURA_SPD_10G || config->data_rate == MADURA_SPD_11G || config->data_rate == MADURA_SPD_25G|| cfg.data_rate == MADURA_SPD_1G ) && (port_idx < 2))) {
                udmsen0 =  (mode == MADURA_AN_NONE )? 1 : 0;
                if (config->data_rate == MADURA_SPD_40G ||
                    config->data_rate == MADURA_SPD_42G) {
                    dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == MADURA_SPD_10G  || config->data_rate == MADURA_SPD_25G ||
                          config->data_rate == MADURA_SPD_11G|| cfg.data_rate == MADURA_SPD_1G ) {
                    if ((pa->access.lane_mask & 0x1) || (pa->access.lane_mask & 0x10)) {
                        dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    } else if ((pa->access.lane_mask & 0x2) || (pa->access.lane_mask & 0x20)) {
                        udmsen1 =  (mode == MADURA_AN_NONE) ? 1 : 0;
                        dmept0 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    }
                }
            } else {
                udmsen2 = (mode == MADURA_AN_NONE ) ? 1 : 0;
                if (config->data_rate == MADURA_SPD_40G ||
                    config->data_rate == MADURA_SPD_42G) {
                    dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                } else if(config->data_rate == MADURA_SPD_10G  || config->data_rate == MADURA_SPD_25G ||
                          config->data_rate == MADURA_SPD_11G|| cfg.data_rate == MADURA_SPD_1G ) {
                    if ((pa->access.lane_mask & 0x4) || (pa->access.lane_mask & 0x40)) {
                        dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    } else if ((pa->access.lane_mask & 0x8) || (pa->access.lane_mask & 0x80)) {
                        udmsen3 = (mode == MADURA_AN_NONE) ? 1 : 0;
                        dmept2 = (mode == MADURA_AN_PROP) ? 1 : 0;
                    }
                }
            }
        }
    } else {
        udmsen0 = (mode == MADURA_AN_NONE ) ? 1 : 0;
    }
    PHYMOD_DEBUG_VERBOSE(("MODE CTRL1 :0x%x\n", udmsen0));

    if (udmsen0 != 0xFF) {
    /*MADURA_STANDALONE */
        BCMI_MADURA_P0_MODE_CTLr_P0_PVALIDf_SET(mode_ctrl0, 1);
    /*MADURA_STANDALONE */
        BCMI_MADURA_P0_MODE_CTLr_P0_UDMSf_SET(mode_ctrl0, udmsen0);
        if (dmept0 != 0xFF) {
            BCMI_MADURA_P0_MODE_CTLr_P0_DPTf_SET(mode_ctrl0, dmept0);
        }
         PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_P0_MODE_CTLr(&pa->access, mode_ctrl0));
    }
    if (udmsen1 != 0xFF) {
    /*MADURA_STANDALONE */
        BCMI_MADURA_P1_MODE_CTLr_P1_PVALIDf_SET(mode_ctrl1, 1);
    /*MADURA_STANDALONE */
        BCMI_MADURA_P1_MODE_CTLr_P1_UDMSf_SET(mode_ctrl1, udmsen1);
        if (dmept0 != 0xFF) {
            BCMI_MADURA_P1_MODE_CTLr_P1_DPTf_SET(mode_ctrl1, dmept0);
        }
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P1_MODE_CTLr(&pa->access, mode_ctrl1));
    }
    if (udmsen2 != 0xFF) {
    /*MADURA_STANDALONE */
        BCMI_MADURA_P2_MODE_CTLr_P2_PVALIDf_SET(mode_ctrl2, 1);
    /*MADURA_STANDALONE */
        BCMI_MADURA_P2_MODE_CTLr_P2_UDMSf_SET(mode_ctrl2, udmsen2);
        if (dmept2 != 0xFF) {
            BCMI_MADURA_P2_MODE_CTLr_P2_DPTf_SET(mode_ctrl2, dmept2);
        }
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P2_MODE_CTLr(&pa->access, mode_ctrl2));
    }
    if (udmsen3 != 0xFF) {
        BCMI_MADURA_P3_MODE_CTLr_P3_PVALIDf_SET(mode_ctrl3, 1);
        BCMI_MADURA_P3_MODE_CTLr_P3_UDMSf_SET(mode_ctrl3, udmsen3);
        if (dmept2 != 0xFF) {
            BCMI_MADURA_P3_MODE_CTLr_P3_DPTf_SET(mode_ctrl3, dmept2);
        }
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_P3_MODE_CTLr(&pa->access, mode_ctrl3));
    }
    return PHYMOD_E_NONE;
}

int _madura_interface_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config)
{
    uint16_t DFE_option = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t intf = config->interface_type;
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  && config->data_rate != MADURA_SPD_50G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));

    /*configure media_type and DFE option. SR/ER/LR, CR,KR, CAUI, XFI*/
    if ( intf == phymodInterfaceER4 || intf == phymodInterfaceSR ||
         intf == phymodInterfaceLR || intf == phymodInterfaceER ||
         intf == phymodInterfaceSFI || intf == phymodInterfaceXLPPI) {
        if (config->interface_modes & PHYMOD_INTF_MODES_UNRELIABLE_LOS) {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS; /*OPTICAL*/
        } else {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL*/
        }
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else if (intf == phymodInterfaceCR4 || intf == phymodInterfaceCR || intf == phymodInterfaceCR2){
        media_type = SERDES_MEDIA_TYPE_COPPER_CABLE; /*Copper*/
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceKR4 || intf == phymodInterfaceKR || intf == phymodInterfaceKR2){
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE;
    } else if (intf == phymodInterfaceCAUI4 || intf == phymodInterfaceVSR ||
               intf == phymodInterfaceCAUI || intf == phymodInterfaceCAUI4_C2M ||
               intf == phymodInterfaceCAUI4_C2C) {
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_DFE_LP_MODE;
    } else if (intf == phymodInterfaceXLAUI || intf == phymodInterfaceXFI|| intf == phymodInterfaceKX){
        media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Back plane */
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
    } else if (intf == phymodInterfaceSR2 || intf == phymodInterfaceLR2 ||
               intf == phymodInterfaceSR4 || intf == phymodInterfaceLR4) {
        if (config->interface_modes & PHYMOD_INTF_MODES_UNRELIABLE_LOS) {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS; /*OPTICAL*/
        } else {
            media_type = SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS; /*OPTICAL*/
        }
        DFE_option = SERDES_DFE_OPTION_NO_DFE;
        if (config->data_rate == MADURA_SPD_100G ||
            config->data_rate == MADURA_SPD_106G) {
        DFE_option = SERDES_DFE_OPTION_DFE;
        }
    } else {
        PHYMOD_DEBUG_ERROR(("Invalid interface type as %d\n ... setting to default interface as KR\n",intf));
    }

    if (ip == MADURA_2X_FALCON_CORE) {
        config_reg = config_reg & (0xf0ff);
        /*considering sys+line side on the same interface*/
        config_reg |= (( media_type)& 0x3) << 8 ; /*line*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 10 ; /*lin*/
    } else {
        config_reg = config_reg & (0xfff0);
        /*considering sys+line side on the same interface*/
        config_reg |= ( media_type)& 0x3 ; /*sys*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 2 ; /*sys */
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    } else  if (lane_mask == 0x2) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    } else {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }

    PHYMOD_IF_ERR_RETURN(_madura_set_ieee_intf(pa, config, intf));

    return PHYMOD_E_NONE;
}

int _madura_interface_set_repeater_retimer_mode_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t Tx_timing_mode)
{
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  && config->data_rate != MADURA_SPD_50G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
            PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    if((config_reg & 0x6060 )== 0x6060 ){
        PHYMOD_DEBUG_ERROR(("ULL+Rep already set so cant set Retimer/Repeater only modes. Disable ULL to set other modes\n"));
        return PHYMOD_E_NONE;
    }

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));

    /*Tx timing mode */
    config_reg &= ~(0x6060);
    config_reg |= ((Tx_timing_mode ) & 3) << 13 ;
    config_reg |= ((Tx_timing_mode ) & 3) << 5 ; /*mode sys*/

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    } else  if (lane_mask == 0x2) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    } else {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }
    return PHYMOD_E_NONE;
}


int _madura_interface_set_ull_mode_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t Tx_timing_mode)
{
    uint16_t  Tx_timing_mode_shift = 0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    uint16_t config_reg0 = 0, config_reg1 = 0,config_reg2 = 0,config_reg3 = 0;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl1, 0, sizeof(BCMI_MADURA_P1_MODE_CTLr_t));

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {
            lane_mask >>= 4 ;
        }
    }

    Tx_timing_mode_shift  = Tx_timing_mode_shift |((Tx_timing_mode ) & 3) << 13; /*Line side*/
    Tx_timing_mode_shift  = Tx_timing_mode_shift |((Tx_timing_mode ) & 3) << 5 ; /*sys side*/

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
    config_reg0 = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    /* Update Tx timing mode */
    config_reg0 &= ~(0x6060);
    config_reg0 |=  Tx_timing_mode_shift ;
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
    config_reg1 = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    /* Update Tx timing mode */
    config_reg1 &= ~(0x6060);
    config_reg1 |=  Tx_timing_mode_shift ;
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
    config_reg2 = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    /* Update Tx timing mode */
    config_reg2 &= ~(0x6060);
    config_reg2 |=  Tx_timing_mode_shift ;
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
    config_reg3 = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    /* Update Tx timing mode */
    config_reg3 &= ~(0x6060);
    config_reg3 |=  Tx_timing_mode_shift ;

    /*
     * Workaround for firmware issue :
     * Leaving  MISC registers at init value of zero does not work
     * with the current firmware
     */

    if  (BCMI_MADURA_MST_MISC0r_GET(mst_misc0) == 0) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,0x404);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    }
    if  (BCMI_MADURA_MST_MISC1r_GET(mst_misc1) == 0) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,0x404);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    }
    if  (BCMI_MADURA_MST_MISC2r_GET(mst_misc2) == 0) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,0x404);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    }
    if  (BCMI_MADURA_MST_MISC3r_GET(mst_misc3) == 0) {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,0x404);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));


    /*write ULL on all misc registers in one go*/
    BCMI_MADURA_MST_MISC0r_SET(mst_misc0,config_reg0);
    BCMI_MADURA_MST_MISC1r_SET(mst_misc1,config_reg1);
    BCMI_MADURA_MST_MISC2r_SET(mst_misc2,config_reg2);
    BCMI_MADURA_MST_MISC3r_SET(mst_misc3,config_reg3);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
   return PHYMOD_E_NONE;
}

int _madura_force_training_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t training_set)
{
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

     if (ip == MADURA_2X_FALCON_CORE) {
        config_reg &= ~(0x1000);
    /* training_en*/
        config_reg |= ((training_set ) & 1) << 12;
    } else {
        config_reg &= ~(0x0010);
    /* training_en*/
    config_reg |= ((training_set ) & 1) << 4; /*enable sys*/
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    } else  if (lane_mask == 0x2) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
   } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
   } else {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }

    return PHYMOD_E_NONE;
}

int _madura_force_training_get(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t *training_get)
{
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    if (ip == MADURA_2X_FALCON_CORE) {
        *training_get = (config_reg & 0x1000) >> 12;
    } else {
        *training_get = (config_reg & 0x10) >> 4;
    }

    return PHYMOD_E_NONE;
}

int _madura_autoneg_set_val(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t AN_enable)
{
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    /*AN enable set on line*/
    config_reg &= ~(0x8000);
    config_reg |= ((AN_enable ) & 1) << 15;

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    } else  if (lane_mask == 0x2) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    } else {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,config_reg);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
    }
    return PHYMOD_E_NONE;
}

int _madura_autoneg_get_val(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, uint16_t *AN_get_val)
{
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;


    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    /*AN enable get on line*/
    *AN_get_val = (config_reg & 0x8000) >> 15;

   return PHYMOD_E_NONE;
}


int _madura_pll_config_state_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, uint16_t *pll_cfg_state)
{
    phymod_phy_inf_config_t cfg;
    MADURA_DEVICE_AUX_MODE_T *aux_mode;
    uint16_t ip = 0, lane_mask = 0;
    uint16_t no_of_ports = 0;
    uint16_t fal_pll_lock = 0, fal_2x_pll_lock = 0;
    const phymod_access_t *pa = &phy->access;
    int rv = PHYMOD_E_NONE;
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PMD_PLL_STSr_t pll_sts;
    uint16_t pll_sel=0;     

    /* If  clock is zero (ie, First Time) */
    {

        BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
        PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));

        PHYMOD_IF_ERR_RETURN(
                    BCMI_MADURA_READ_CLK_SCALER_CTLr(pa,&clk_ctrl));

        if (BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_GET(clk_ctrl)==0x800)
        {
            *pll_cfg_state = 1;

            return rv;
        }
    }

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));

    PHYMOD_MEMSET(&cfg, 0, sizeof(phymod_phy_inf_config_t));
    cfg.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) cfg.device_aux_modes;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);


    MADURA_IF_ERR_RETURN_FREE(cfg.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &cfg)));

    if(cfg.data_rate==MADURA_SPD_25G){
        pll_sel=1;
    }

    for (ip = MADURA_2X_FALCON_CORE; ip <= MADURA_FALCON_CORE; ip++) {
		MADURA_IF_ERR_RETURN_FREE(cfg.device_aux_modes, (
					_madura_set_slice_reg (pa,  ip, MADURA_DEV_PMA_PMD,
              ((ip == MADURA_2X_FALCON_CORE) && (lane_mask & 0xF0)) ?  4 : 0 ,pll_sel)));

        PHYMOD_MEMSET(&pll_sts, 0 , sizeof(BCMI_MADURA_PMD_PLL_STSr_t));
        MADURA_IF_ERR_RETURN_FREE(cfg.device_aux_modes, (
            BCMI_MADURA_READ_PMD_PLL_STSr(pa,&pll_sts )));

        if (ip == MADURA_FALCON_CORE) {
	        fal_pll_lock=(pll_sel==0)?BCMI_MADURA_PMD_PLL_STSr_PMD_PLL0_LOCKf_GET(pll_sts):BCMI_MADURA_PMD_PLL_STSr_PMD_PLL1_LOCKf_GET(pll_sts);
        } else {
	        fal_2x_pll_lock=(pll_sel==0)?BCMI_MADURA_PMD_PLL_STSr_PMD_PLL0_LOCKf_GET(pll_sts):BCMI_MADURA_PMD_PLL_STSr_PMD_PLL1_LOCKf_GET(pll_sts);
        }
    }
    /*
     *  PLL div is per die and will be programed only when
     *  1. if there is no pll lock on falcon
     *  2. speed change b/w 100G to 40G/20G/10G
     *  3. if ref clock freq changes(pll lock lost)
     *  4. On interface_modes changes(IEEE/OTN/HIGIG)
     */
    MADURA_GET_PORT_FROM_MODE(config, no_of_ports, aux_mode);

    if (((fal_pll_lock != 1) || (fal_2x_pll_lock != 1)) || (config->ref_clock != cfg.ref_clock) ||
        (config->interface_modes != cfg.interface_modes) || ((no_of_ports == 1) &&
        (config->data_rate != cfg.data_rate))) {
        *pll_cfg_state = 1;
    }else if(config->data_rate != cfg.data_rate){  /*1G can use either PLL 132x or 165x */
            if(((config->data_rate == MADURA_SPD_25G) && ((cfg.data_rate==MADURA_SPD_10G) || (cfg.data_rate==MADURA_SPD_11G)))||
            (((config->data_rate == MADURA_SPD_50G) ) && (cfg.data_rate==MADURA_SPD_10G))||/*stand alone default speed is 10G*/
            (((config->data_rate == MADURA_SPD_10G) || (config->data_rate == MADURA_SPD_11G)) && (cfg.data_rate==MADURA_SPD_25G))||
            ((config->data_rate == MADURA_SPD_50G) && (cfg.data_rate==MADURA_SPD_40G || cfg.data_rate==MADURA_SPD_42G))||
            ((config->data_rate == MADURA_SPD_40G || config->data_rate == MADURA_SPD_42G) && (cfg.data_rate==MADURA_SPD_50G))||
             (config->data_rate == MADURA_SPD_25G)){
            *pll_cfg_state = 1;
        }
    }

    PHYMOD_FREE(cfg.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return rv;
}

int _madura_set_ieee_intf(const phymod_access_t *pa, const phymod_phy_inf_config_t *config, uint16_t intf)
{
    uint16_t port = 0, physical_port = 0;
    uint16_t intf_side = 0;
    BCMI_MADURA_GPREG6r_t gpreg6;
    BCMI_MADURA_GPREG7r_t gpreg7;
    BCMI_MADURA_GPREG4r_t gpreg4;
    BCMI_MADURA_GPREG5r_t gpreg5;

    PHYMOD_MEMSET(&gpreg6, 0, sizeof(BCMI_MADURA_GPREG6r_t));
    PHYMOD_MEMSET(&gpreg7, 0, sizeof(BCMI_MADURA_GPREG7r_t));
    PHYMOD_MEMSET(&gpreg4, 0, sizeof(BCMI_MADURA_GPREG4r_t));
    PHYMOD_MEMSET(&gpreg5, 0, sizeof(BCMI_MADURA_GPREG5r_t));

    PHYMOD_IF_ERR_RETURN(
        _madura_port_from_lane_map_get(pa, config, &port, &physical_port));

    MADURA_GET_INTF_SIDE(pa, intf_side);

    if (port == 0) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG6r(pa, &gpreg6));
        gpreg6.v[0] = intf_side ? (gpreg6.v[0] & 0xFF) : (gpreg6.v[0] & 0xFF00);
        gpreg6.v[0] |= intf_side ? (intf << 8) : (intf);
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_WRITE_GPREG6r(pa, gpreg6));
    } else if (port == 1) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG7r(pa, &gpreg7));
        gpreg7.v[0] = intf_side ? (gpreg7.v[0] & 0xFF) : (gpreg7.v[0] & 0xFF00);
        gpreg7.v[0] |= intf_side ? (intf << 8) : (intf);
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_WRITE_GPREG7r(pa, gpreg7));
    } else if (port == 2) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG4r(pa, &gpreg4));
        gpreg4.v[0] = intf_side ? (gpreg4.v[0] & 0xFF) : (gpreg4.v[0] & 0xFF00);
        gpreg4.v[0] |= intf_side ? (intf << 8) : (intf);
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_WRITE_GPREG4r(pa, gpreg4));
    } else {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG5r(pa, &gpreg5));
        gpreg5.v[0] = intf_side ? (gpreg5.v[0] & 0xFF) : (gpreg5.v[0] & 0xFF00);
        gpreg5.v[0] |= intf_side ? (intf << 8) : (intf);
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_WRITE_GPREG5r(pa, gpreg5));
    }

    return PHYMOD_E_NONE;
}

int _madura_get_ieee_intf(const phymod_access_t *pa, const phymod_phy_inf_config_t *config, uint16_t *intf)
{
    uint16_t port = 0, physical_port = 0;
    uint16_t intf_side = 0;
    BCMI_MADURA_GPREG6r_t gpreg6;
    BCMI_MADURA_GPREG7r_t gpreg7;
    BCMI_MADURA_GPREG4r_t gpreg4;
    BCMI_MADURA_GPREG5r_t gpreg5;

    PHYMOD_MEMSET(&gpreg6, 0, sizeof(BCMI_MADURA_GPREG6r_t));
    PHYMOD_MEMSET(&gpreg7, 0, sizeof(BCMI_MADURA_GPREG7r_t));
    PHYMOD_MEMSET(&gpreg4, 0, sizeof(BCMI_MADURA_GPREG4r_t));
    PHYMOD_MEMSET(&gpreg5, 0, sizeof(BCMI_MADURA_GPREG5r_t));

    PHYMOD_IF_ERR_RETURN(
        _madura_port_from_lane_map_get(pa, config, &port, &physical_port));

    MADURA_GET_INTF_SIDE(pa, intf_side);
    if (port == 0) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG6r(pa, &gpreg6));
        *intf = intf_side ? ((gpreg6.v[0] >> 8) & 0xFF) : (gpreg6.v[0] & 0x00FF);
    } else if (port == 1) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG7r(pa, &gpreg7));
        *intf = intf_side ? ((gpreg7.v[0] >> 8) & 0xFF) : (gpreg7.v[0] & 0x00FF);
    } else if (port == 2) {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG4r(pa, &gpreg4));
        *intf = intf_side ? ((gpreg4.v[0] >> 8) & 0xFF) : (gpreg4.v[0] & 0x00FF);
    } else {
        PHYMOD_IF_ERR_RETURN(BCMI_MADURA_READ_GPREG5r(pa, &gpreg5));
        *intf = intf_side ? ((gpreg5.v[0] >> 8) & 0xFF) : (gpreg5.v[0] & 0x00FF);
    }

    return PHYMOD_E_NONE;
}

int _madura_get_media_type(const phymod_access_t *pa, const phymod_phy_inf_config_t *config, uint16_t *media_type)
{
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    uint16_t config_reg = 0;
    uint16_t ip = 0;
    uint16_t lane_mask = pa->lane_mask;
    phymod_phy_inf_config_t cfg;
    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));
    PHYMOD_MEMCPY(&cfg, config, sizeof(phymod_phy_inf_config_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    MADURA_GET_IP(pa, cfg, ip);

    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  && config->data_rate != MADURA_SPD_50G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    if (ip == MADURA_2X_FALCON_CORE) { /*LINE SIDE*/
        *media_type = (((config_reg & 0xffff) >> 8) & 0x3);
    } else { /*SYS SIDE*/
        *media_type = ((config_reg & 0xffff) & 0x3);
    }

    return PHYMOD_E_NONE;
}

int _madura_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags, const phymod_phy_inf_config_t* config)
{
    uint16_t pll_cfg_state = 0;
    phymod_phy_inf_config_t cfg;
    phymod_phy_inf_config_t cpy_config;
    MADURA_DEVICE_AUX_MODE_T *aux_mode;
    uint32_t chip_id = 0;
    uint32_t rev = 0;
    uint16_t ip = 0;
    uint8_t clear_flag = 0;

    PHYMOD_IF_ERR_RETURN(
          _madura_pll_config_state_get(pa, config, &pll_cfg_state));

    PHYMOD_MEMCPY(&cpy_config, config, sizeof(phymod_phy_inf_config_t));

    if (cpy_config.device_aux_modes == NULL) {
        cpy_config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
        clear_flag = 1;
        PHYMOD_MEMSET(cpy_config.device_aux_modes, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));
    }
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) cpy_config.device_aux_modes;
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL,
        (madura_get_chipid(&pa->access, &chip_id, &rev)));

    if (aux_mode->pass_thru == 1 || aux_mode->pass_thru_dual_lane == 1) {
        aux_mode->passthru_sys_side_core = MADURA_FALCON_CORE ;
    }

    PHYMOD_DEBUG_VERBOSE(("Configuring interface\n"));
    PHYMOD_MEMSET(&cfg, 0, sizeof(phymod_phy_inf_config_t));
    cfg.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    do {
        int loc_err ;

        loc_err = _madura_phy_interface_config_get(&pa->access, 0, &cfg);

        MADURA_GET_IP((&pa->access), cfg, ip);

        PHYMOD_FREE(cfg.device_aux_modes);

        if (loc_err != PHYMOD_E_NONE)  {
            if (clear_flag) {
                PHYMOD_FREE(cpy_config.device_aux_modes);
            }
            return loc_err ;
        }
    } while (0);

    /* Configure Ref Clock*/
    PHYMOD_DEBUG_VERBOSE(("Configuring REF clock %d\n",config->ref_clock));
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
          _madura_configure_ref_clock(&pa->access, config->ref_clock)));

    /* Configure DUT MODE */
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_config_dut_mode_reg(pa, &cpy_config)));

    /* Enable UDMS for non-an MODE */
    PHYMOD_DEBUG_VERBOSE(("Configuring UDMS\n"));
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_udms_config(pa, &cpy_config, MADURA_AN_NONE)));

    /* Configure PLL Divider*/
    if (pll_cfg_state) {
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
            _madura_config_pll_div(&pa->access, cpy_config)));
    }

    /* Configure Interface.*/
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_interface_set(&pa->access, ip,  &cpy_config)));

    /* Disable fec in speed other than 100G when prev speed is 100G */
    if(cpy_config.data_rate!=MADURA_SPD_100G /*Cur speed*/ && cfg.data_rate==MADURA_SPD_100G/* Prev speed*/){
    MADURA_IF_ERR_RETURN_FREE( (clear_flag) ? aux_mode : NULL, (
        _madura_fec_enable_set(&pa->access, 0)));
    }

    if (clear_flag) {
        PHYMOD_FREE(cpy_config.device_aux_modes);
    }

    return PHYMOD_E_NONE;
}

int _madura_phy_interface_config_get(const phymod_access_t *pa, uint32_t flags, phymod_phy_inf_config_t *config)
{
    /*get datarate,passthru, gearbox, get refclock,get intf side*/
    BCMI_MADURA_P0_MODE_CTLr_t mode_ctrl0;
    BCMI_MADURA_P1_MODE_CTLr_t mode_ctrl1;
    BCMI_MADURA_P2_MODE_CTLr_t mode_ctrl2;
    BCMI_MADURA_P3_MODE_CTLr_t mode_ctrl3;
    unsigned char              mindex0 = 0xFF;
    unsigned char              mindex1 = 0xFF;
    unsigned char              mindex2 = 0xFF;
    unsigned char              mindex3 = 0xFF;

    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;
    uint16_t ip = 0, intf_side = 0;
    uint16_t clock_ratio = 0;
    uint16_t lane_mask = pa->lane_mask;
    uint16_t intf;
    uint16_t pll_1G_index;
    uint16_t media_type = 0;

    PHYMOD_MEMSET(&mode_ctrl0, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&mode_ctrl2, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));

    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;
    if (aux_mode == NULL) {
        PHYMOD_DEBUG_VERBOSE(("AUX MODE MEM NOT ALLOC\n"));
        return PHYMOD_E_PARAM;
    }
    PHYMOD_MEMSET(aux_mode, 0, sizeof(MADURA_DEVICE_AUX_MODE_T));

    if (!lane_mask) {
      PHYMOD_DEBUG_ERROR(("Invalid Lanemap as %x. Setting to default lane map as 0xF\n",lane_mask));
        lane_mask = 0xf;
       /* return PHYMOD_E_PARAM;*/
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P0_MODE_CTLr(pa , &mode_ctrl0));
    mindex0 = BCMI_MADURA_P0_MODE_CTLr_P0_MINDEXf_GET(mode_ctrl0);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P1_MODE_CTLr(pa , &mode_ctrl1));
    mindex1 = BCMI_MADURA_P1_MODE_CTLr_P1_MINDEXf_GET(mode_ctrl1);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P2_MODE_CTLr(pa , &mode_ctrl2));
    mindex2 = BCMI_MADURA_P2_MODE_CTLr_P2_MINDEXf_GET(mode_ctrl2);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_P3_MODE_CTLr(pa , &mode_ctrl3));
    mindex3 = BCMI_MADURA_P3_MODE_CTLr_P3_MINDEXf_GET(mode_ctrl3);

    PHYMOD_DEBUG_VERBOSE(("MODE STS:%x\n", mindex0));

    MADURA_GET_INTF_SIDE(pa, intf_side);

    if (intf_side == MADURA_IF_LINE) {
        ip = MADURA_2X_FALCON_CORE;
    } else {
        ip = MADURA_FALCON_CORE;
    }
    aux_mode->alternate = 0;
    if (ip == MADURA_FALCON_CORE) {
        if (MADURA_MIDX_IS_4X25G_PLUS(mindex0)) {
            config->data_rate = MADURA_SPD_100G;
            switch(mindex0) {
                case MADURA_MIDX_100HGPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_100HGPT_0:
                    config->data_rate = MADURA_SPD_106G;
                    break;
                case MADURA_MIDX_100GPT_0A:
                    aux_mode->alternate = 1;
                    break;
            }
            aux_mode->pass_thru = 1;
        } else if (lane_mask == 0x2) {
            switch(mindex1) {
               case MADURA_MIDX_10HGPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_1:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_1:
                    if(read_1G_mode_status(pa,mindex1,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                  }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,1, mindex1 )){
            config->data_rate = MADURA_SPD_25G;
            }else{
            config->data_rate = MADURA_SPD_10G;
           }
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0x3) {
            switch(mindex0) {
                case MADURA_MIDX_40HGPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40HGPT10_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 1;
                    break;
                case MADURA_MIDX_40GPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40GPT10_0:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_0:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,0, mindex0 )){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
            }
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_0:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_0:
                    if(read_1G_mode_status(pa,mindex0,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,0, mindex0)){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
            }
                    aux_mode->pass_thru = 1;
                    break;
            }


        } else if (lane_mask == 0x8) {
            switch(mindex3) {
               case MADURA_MIDX_10HGPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_3:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_3:
                    if(read_1G_mode_status(pa,mindex3,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,3, mindex3 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
            }
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0xc) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_0A:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_1:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_1:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,2,mindex2 )){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
                    }
            aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_2:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_2:
                    if(read_1G_mode_status(pa,mindex2,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,2, mindex2)){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }

        }
        if (aux_mode->pass_thru == 1 || aux_mode->pass_thru_dual_lane == 1) {
            aux_mode->passthru_sys_side_core = MADURA_FALCON_CORE ;
            aux_mode->pass_thru = 1 ;
        }
    } else { /* ip = MADURA_2X_FALCON_CORE */
        if (MADURA_MIDX_IS_4X25G_PLUS(mindex0)) {
            config->data_rate = MADURA_SPD_100G;
            switch(mindex0) {
                case MADURA_MIDX_100HGPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_100HGPT_0:
                    config->data_rate = MADURA_SPD_106G;
                    break;
                case MADURA_MIDX_100GPT_0A:
                    aux_mode->alternate = 1;
                    break;
            }
            aux_mode->pass_thru = 1;
        } else if (lane_mask == 0x2) {
            switch(mindex1) {
               case MADURA_MIDX_10HGPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_1:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_1:
                    if(read_1G_mode_status(pa,mindex1,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex1==MADURA_MIDX_10GPT_1A )?5:1, mindex1)){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0x3) {
            switch(mindex0) {
                case MADURA_MIDX_40HGPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40HGPT10_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 1;
                    break;
                case MADURA_MIDX_40GPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40GPT10_0:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_0:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,(mindex0==MADURA_MIDX_40GPT20_0A)? 4:0, mindex0)){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
            }
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_0:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_0:
                    if(read_1G_mode_status(pa,mindex0,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex0 == MADURA_MIDX_10GPT_0A)? 4:0, mindex0)){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }

        } else if (lane_mask == 0x8) {
            switch(mindex3) {
               case MADURA_MIDX_10HGPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_3:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_3:
                    if(read_1G_mode_status(pa,mindex3,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex3 == MADURA_MIDX_10GPT_3A )?7:3, mindex3 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask & 0xc) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_1:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_1:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex2 == MADURA_MIDX_40GPT20_1A )? 6:2, mindex2 )){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
            }
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_2:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_2:
                    if(read_1G_mode_status(pa,mindex2,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex2 == MADURA_MIDX_10GPT_2A )? 6:2 , mindex2 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
            }
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask == 0xF0) {
            switch(mindex2) {
                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

                default:
                    switch(mindex0) {
                        case MADURA_MIDX_40HGPT10_0A:
                            aux_mode->alternate = 1;
                       /* coverity[fallthrough] */
                        case MADURA_MIDX_40HGPT10_0:
                            config->data_rate = MADURA_SPD_42G;
                            aux_mode->pass_thru = 1;
                            break;
                        case MADURA_MIDX_40GPT10_0A:
                            aux_mode->alternate = 1;
                       /* coverity[fallthrough] */
                        case MADURA_MIDX_40GPT10_0:
                            config->data_rate = MADURA_SPD_40G;
                            aux_mode->pass_thru = 1;
                            break;
                    }
                    break;
            }

        } else if (lane_mask == 0x20) {
            switch(mindex1) {
               case MADURA_MIDX_10HGPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_1:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_1:
                    if(read_1G_mode_status(pa,mindex1,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip, ( mindex1 == MADURA_MIDX_10GPT_1A ) ? 5 : 1, mindex1 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
            }
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask &  0x30 ) {
            switch(mindex0) {
                case MADURA_MIDX_40HGPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40HGPT10_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 1;
                    break;
                case MADURA_MIDX_40GPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40GPT10_0:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_0:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_0:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,0, mindex0 )){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
            }
                    aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_0:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_0:
                    if(read_1G_mode_status(pa,mindex0,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip, ( mindex0 == MADURA_MIDX_10GPT_0A )? 4:0, mindex0 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
            }
                    aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask == 0x80) {
            switch(mindex3) {
               case MADURA_MIDX_10HGPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_3:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_3A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_3:
                    if(read_1G_mode_status(pa,mindex3,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex3 == MADURA_MIDX_10GPT_3A )? 7:3, mindex3 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }
        } else if (lane_mask &  0xC0) {
            switch(mindex2) {
                case MADURA_MIDX_40GPT10_0A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
                case MADURA_MIDX_40GPT10_0:
                case MADURA_MIDX_40GPT10_1:
                case MADURA_MIDX_40GPT10_1A:
                case MADURA_MIDX_40HGPT10_0:
                case MADURA_MIDX_40HGPT10_0A:
                case MADURA_MIDX_40HGPT10_1:
                case MADURA_MIDX_40HGPT10_1A:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 1;
                    break;

                case MADURA_MIDX_40GDM_0:
                case MADURA_MIDX_40GDM_1:
                case MADURA_MIDX_40GDM_0B:
                case MADURA_MIDX_40GDM_1B:
                    config->data_rate = MADURA_SPD_40G;
                    aux_mode->pass_thru = 0;
                    break;
                case MADURA_MIDX_40HGDM_0:
                case MADURA_MIDX_40HGDM_1:
                case MADURA_MIDX_40HGDM_0B:
                case MADURA_MIDX_40HGDM_1B:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru = 0;
                    break;

               case MADURA_MIDX_40HGPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40HGPT20_1:
                    config->data_rate = MADURA_SPD_42G;
                    aux_mode->pass_thru_dual_lane = 1;
                    break;
               case MADURA_MIDX_40GPT20_1A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_40GPT20_1:
                    if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex2 == MADURA_MIDX_40GPT20_1A )? 6:2, mindex2 )){
                    config->data_rate = MADURA_SPD_50G;
            }else{
                    config->data_rate = MADURA_SPD_40G;
                    }
            aux_mode->pass_thru_dual_lane = 1;
                    break;

               case MADURA_MIDX_10HGPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10HGPT_2:
                    config->data_rate = MADURA_SPD_11G;
                    aux_mode->pass_thru = 1;
                    break;
               case MADURA_MIDX_10GPT_2A:
                    aux_mode->alternate = 1;
               /* coverity[fallthrough] */
               case MADURA_MIDX_10GPT_2:
                    if(read_1G_mode_status(pa,mindex2,&pll_1G_index)){
                        config->data_rate = MADURA_SPD_1G;
                    }else if(MADURA_PLL_MODE_165 == read_madura_pll_mode(pa,ip,( mindex2 == MADURA_MIDX_10GPT_2A )? 6:2, mindex2 )){
                    config->data_rate = MADURA_SPD_25G;
            }else{
                    config->data_rate = MADURA_SPD_10G;
                    }
            aux_mode->pass_thru = 1;
                    break;
            }
        }
        if (aux_mode->pass_thru == 1 || aux_mode->pass_thru_dual_lane == 1) {
            aux_mode->passthru_sys_side_core = MADURA_FALCON_CORE ;
            aux_mode->pass_thru = 1;
        }
    }

    if (config->data_rate == MADURA_SPD_11G ||
        config->data_rate == MADURA_SPD_42G ||
        config->data_rate == MADURA_SPD_106G ) {
        PHYMOD_INTF_MODES_HIGIG_SET(config);
    } else {
        config->interface_modes = 0; /*Ethernet*/
    }

    /*CLOCK_SCALEr*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CLK_SCALER_CTLr(pa,&clk_ctrl));
    clock_ratio = BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_GET(clk_ctrl);
    if (clock_ratio == 0x640) {
        config->ref_clock = phymodRefClk156Mhz;
    } else if (clock_ratio == 0x672) {
        config->ref_clock = phymodRefClk161Mhz;
    } else if (clock_ratio == 0x800) {
        config->ref_clock = 0 ;/* set to 0 in reset value 0x800. Is it fine?*/
    }

    PHYMOD_IF_ERR_RETURN(_madura_get_ieee_intf(pa, config, &intf));
    config->interface_type          = intf;
    PHYMOD_IF_ERR_RETURN(_madura_get_media_type(pa, config, &media_type));
    if (media_type == SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS) {
        PHYMOD_INTF_MODES_UNRELIABLE_LOS_SET(config);
    } else {
        PHYMOD_INTF_MODES_UNRELIABLE_LOS_CLR(config);
    } 

    return PHYMOD_E_NONE;
}

int _madura_phy_pcs_link_get(const phymod_access_t *pa, uint32_t *pcs_link)
{
    if (pcs_link) {
        *pcs_link = 0xffff;
    } else {
        return PHYMOD_E_PARAM;
    }
    PHYMOD_IF_ERR_RETURN(_madura_get_pcs_link_status(pa, pcs_link));

    return PHYMOD_E_NONE;
}

int _madura_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk)
{
    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));

    /* uint16_t clk_scalar_code = 0x0; */
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
    /*  case phymodRefClk625Mhz: */
            BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x640);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x672);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
        break;
        default:
        return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _madura_get_pll_modes(phymod_ref_clk_t ref_clk,const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *fal_2x_pll_mode)
{
    uint32_t speed = config->data_rate;
    switch (ref_clk) {
        case phymodRefClk156Mhz:
        case phymodRefClk312Mhz:
            if (speed == MADURA_SPD_100G ||
                speed == MADURA_SPD_106G) {
            if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
              *fal_pll_mode = MADURA_PLL_MODE_175;
            } else {
            *fal_pll_mode = MADURA_PLL_MODE_165;
            }
            } else if( speed == MADURA_SPD_25G ||
               speed == MADURA_SPD_50G ){
            *fal_pll_mode = MADURA_PLL_MODE_165;
            } else {
            /*-- Falcon Line Rate is 10G/20G*/
            if (!PHYMOD_INTF_MODES_HIGIG_GET(config)) { /*IEEE MODE*/
            *fal_pll_mode = MADURA_PLL_MODE_132;
            } else if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
            *fal_pll_mode = MADURA_PLL_MODE_140;
            } else {
            return PHYMOD_E_PARAM;
            }
        }
        break;
        case phymodRefClk161Mhz:
        case phymodRefClk322Mhz:
        case phymodRefClk644Mhz:
            if (speed == MADURA_SPD_100G ||
                speed == MADURA_SPD_106G) {
                if (PHYMOD_INTF_MODES_HIGIG_GET(config)) {
                    return PHYMOD_E_PARAM;
                } else {
                    *fal_pll_mode = MADURA_PLL_MODE_160;
                }
            } else {
                if (!PHYMOD_INTF_MODES_HIGIG_GET(config)) {/*IEEE MODE*/
                    *fal_pll_mode = MADURA_PLL_MODE_128;
                } else {
                    return PHYMOD_E_PARAM;
                }
        }
        break;
        case phymodRefClk174Mhz:
        case phymodRefClk349Mhz:
        case phymodRefClk698Mhz:
            if (speed == MADURA_SPD_100G ||
                speed == MADURA_SPD_106G) {
                if (config->interface_type == phymodInterfaceOTN) {
                    *fal_pll_mode = MADURA_PLL_MODE_160;
                } else {
                    return PHYMOD_E_PARAM;
                }
            } else {
                if (config->interface_type == phymodInterfaceOTN) {
                    *fal_pll_mode = MADURA_PLL_MODE_128;
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

int _madura_configure_pll(const phymod_access_t *pa, const uint16_t pll_val)
{

	PHYMOD_IF_ERR_RETURN(
			wrc_falcon2_madura_ams_pll_pwrdn(0));
	PHYMOD_IF_ERR_RETURN(
			wrc_falcon2_madura_core_dp_s_rstb(0));
	PHYMOD_IF_ERR_RETURN (
			falcon2_madura_configure_pll(pa, pll_val));
	PHYMOD_IF_ERR_RETURN(
			wrc_falcon2_madura_core_dp_s_rstb(1));

    return PHYMOD_E_NONE;
}

int _madura_config_pll_div(const phymod_access_t *pa, const phymod_phy_inf_config_t config)
{
    uint16_t ip = 0, fal_2x_pll_mode = 0, falcon_pll_mode = 0, lane_mask=0;
    phymod_ref_clk_t ref_clk = config.ref_clock;
    uint16_t data = 0, retry_cnt = 5;
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PMD_PLL_STSr_t pll_sts;
    uint16_t lane=0, max_lane=0, pll_sel=0 ;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    uint32_t programmed_pll=0;
    err_code_t __err=ERR_CODE_NONE;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&pll_sts, 0 , sizeof(BCMI_MADURA_PMD_PLL_STSr_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    /*Program PLL div of all dies*/
    /* Set pll_mode */
    PHYMOD_IF_ERR_RETURN(
       _madura_get_pll_modes(ref_clk, &config, &falcon_pll_mode, &fal_2x_pll_mode));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);


    for (ip = MADURA_2X_FALCON_CORE ; ip <= MADURA_FALCON_CORE ;ip++) {
    max_lane = ( ip == MADURA_2X_FALCON_CORE)? 8:4 ;
        for( lane =0; lane < max_lane ; lane ++)
        {
            if(1<<lane & lane_mask){
            pll_sel = (config.data_rate == 25000) ? 1:0 ;
        PHYMOD_IF_ERR_RETURN (
          _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane , pll_sel ));

			programmed_pll = rdc_falcon2_madura_pll_mode() ;
            if(__err!=ERR_CODE_NONE){
				PHYMOD_DEBUG_ERROR(("PLL read failed for pll\n"));       
				return PHYMOD_E_FAIL;
            }      

			/* Set PLL div for 156Mhz clock*/
			switch (falcon_pll_mode) {
				case MADURA_PLL_MODE_132:
					if(programmed_pll!=MADURA_PLL_MODE_132){
						PHYMOD_IF_ERR_RETURN (
								_madura_configure_pll(pa, FALCON2_MADURA_pll_div_132x));
					}
					break;
				case MADURA_PLL_MODE_140:
					if(programmed_pll!=MADURA_PLL_MODE_140){
						PHYMOD_IF_ERR_RETURN (
								_madura_configure_pll(pa, FALCON2_MADURA_pll_div_140x));
					}
					break;
				case MADURA_PLL_MODE_165: /*PLL is 165 for 100G and 25G*/
					if(programmed_pll!=MADURA_PLL_MODE_165){
						PHYMOD_IF_ERR_RETURN (
								_madura_configure_pll(pa, FALCON2_MADURA_pll_div_165x));
					}
					break;
				case MADURA_PLL_MODE_175:
					if(programmed_pll!=MADURA_PLL_MODE_175){
						PHYMOD_IF_ERR_RETURN (
								_madura_configure_pll(pa, FALCON2_MADURA_pll_div_175x));
					}
					break;
				default:
					return PHYMOD_E_PARAM;
			}
			}
		}
    }

    /*  Set FW_ENABLE = 1 */
   retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));
   do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
        data=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("PLL config failed, micro controller is busy..")));
    }

   PHYMOD_USLEEP(100000);/*100 milsec*/
   /*check pll lock */
   retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
  do {
      PHYMOD_IF_ERR_RETURN(
	  BCMI_MADURA_READ_PMD_PLL_STSr(pa,&pll_sts ));
	  data=(pll_sel==0)?BCMI_MADURA_PMD_PLL_STSr_PMD_PLL0_LOCKf_GET(pll_sts):BCMI_MADURA_PMD_PLL_STSr_PMD_PLL1_LOCKf_GET(pll_sts);
	  PHYMOD_USLEEP(100);
    } while((data == 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_DEBUG_ERROR(("PLL LOCK failed with pll_sts 0x%x\n",BCMI_MADURA_PMD_PLL_STSr_GET(pll_sts)));
        return PHYMOD_E_FAIL;
    }

    /*Reset DEV1 slice register*/
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_core_dp_rstb(const phymod_access_t *pa, const phymod_phy_inf_config_t config)
{
    return PHYMOD_E_NONE;
}

int _madura_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction)
{
    uint16_t cores=3,ip=0;
    uint16_t lane=0;
    BCMI_MADURA_GEN_CTL1r_t gen_ctrl;
    BCMI_MADURA_COMMON_CTLr_t fal_cmn_ctrl;
    BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_t ln_s_rstb;

    PHYMOD_MEMSET(&gen_ctrl, 0, sizeof(BCMI_MADURA_GEN_CTL1r_t));
    PHYMOD_MEMSET(&fal_cmn_ctrl, 0, sizeof(BCMI_MADURA_COMMON_CTLr_t));
    PHYMOD_MEMSET(&ln_s_rstb, 0, sizeof(BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_t));

    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_GEN_CTL1r(pa,&gen_ctrl));
    if (reset_mode == phymodResetModeHard) {
        /*Reset for chip*/
        BCMI_MADURA_GEN_CTL1r_RESETBf_SET(gen_ctrl,0);
        /*Reset registers*/
        BCMI_MADURA_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl,0);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_GEN_CTL1r(pa,gen_ctrl));
        /*Above chip reset resets the cores also !*/
        PHYMOD_USLEEP(10000);
    } else {
        /*Reset registers*/
        BCMI_MADURA_GEN_CTL1r_REG_RSTBf_SET(gen_ctrl,0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_GEN_CTL1r(pa,gen_ctrl));
        /* Reset per falcon of 4 falcons*/
        while(cores>0)
        {
        PHYMOD_IF_ERR_RETURN(
                _madura_set_slice_reg (pa,
                    ip, MADURA_DEV_PMA_PMD, lane, 0));
        /*Lane soft reset*/
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_RXTX_LN_S_RSTB_CTLr(pa,&ln_s_rstb));
        BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_LN_S_RSTBf_SET(ln_s_rstb,0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_RXTX_LN_S_RSTB_CTLr(pa,ln_s_rstb));
        PHYMOD_USLEEP(10000);
        BCMI_MADURA_RXTX_LN_S_RSTB_CTLr_LN_S_RSTBf_SET(ln_s_rstb,1);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_RXTX_LN_S_RSTB_CTLr(pa,ln_s_rstb));

        ip=(cores==2)?1:0;/*toggle ip for 3rd iteration*/
        lane=(ip==1)?0:4;/*toggle ip for 3rd iteration*/
           cores--;
        }

    }

    /* Force clock to 0 temporarily */
    {

    BCMI_MADURA_CLK_SCALER_CTLr_t clk_ctrl;
    PHYMOD_MEMSET(&clk_ctrl, 0, sizeof(BCMI_MADURA_CLK_SCALER_CTLr_t));

    BCMI_MADURA_CLK_SCALER_CTLr_CLOCK_SCALER_RATIOf_SET(clk_ctrl,0x800);
    PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_CLK_SCALER_CTLr(pa,clk_ctrl));
    }

    return PHYMOD_E_NONE;
}

int _madura_pcs_link_monitor_enable_set(const phymod_access_t *pa, uint16_t en_dis)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0;
    BCMI_MADURA_PATT_MON_GEN_CTLr_t mon_ctrl;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_MEMSET(&mon_ctrl, 0, sizeof(BCMI_MADURA_PATT_MON_GEN_CTLr_t));

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
    max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
        /*config slice reg */
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_READ_PATT_MON_GEN_CTLr(pa,&mon_ctrl)));
        BCMI_MADURA_PATT_MON_GEN_CTLr_RX_MON_CTRLf_SET(mon_ctrl,(en_dis<<1));
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_WRITE_PATT_MON_GEN_CTLr(pa,mon_ctrl)));
        PHYMOD_DEBUG_VERBOSE(("Falcon Set Link monitor for Lane:%d \n",
                            lane));
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_pcs_link_monitor_enable_get(const phymod_access_t *pa, uint32_t *get_pcs)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0;
    BCMI_MADURA_PATT_MON_GEN_CTLr_t mon_ctrl;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    PHYMOD_MEMSET(&mon_ctrl, 0, sizeof(BCMI_MADURA_PATT_MON_GEN_CTLr_t));

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);
    /*MADURA_GET_INTF_SIDE(pa,intf_side); */

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
    max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    for (lane = 0; lane < max_lane; lane ++) {
        if ((lane_mask & (1 << lane))) {
        /*config slice reg */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_PATT_MON_GEN_CTLr(pa,&mon_ctrl)));
                *get_pcs |= ((BCMI_MADURA_PATT_MON_GEN_CTLr_RX_MON_CTRLf_GET(mon_ctrl))>>1)<<lane;
                PHYMOD_DEBUG_VERBOSE(("Falcon Get Link monitor for Lane:%d Data:0x%x\n",
                            lane, *get_pcs));
        }else{
        PHYMOD_DEBUG_VERBOSE(("lane %x not matching with lanemask %x get_pcs=%x\n",lane,lane_mask,*get_pcs));
        }
    }

    if(( *get_pcs != 0 ) && ( *get_pcs == lane_mask ))
    {   *get_pcs = 1 ; }
    else
    {   *get_pcs = 0 ; }

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_get_pcs_link_status(const phymod_access_t *pa, uint32_t *link_sts)
{
    uint16_t lane = 0;
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;
    uint16_t max_lane = 0, lane_mask=0 ;
    uint32_t err_cnt = 0, get_pcs = 0;
    BCMI_MADURA_CL49_BER_STSr_t ber_status;
    BCMI_MADURA_CL82_BER_CNTr_t ber_cnt;
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_PCS_MON_LIVE_STSr_t pcs_mon_liv_sts;
    BCMI_MADURA_PCS_MON_STSr_t pcs_mon_sts;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&ber_status, 0, sizeof(BCMI_MADURA_CL49_BER_STSr_t));
    PHYMOD_MEMSET(&slice_reg, 0, sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&ber_cnt, 0, sizeof(BCMI_MADURA_CL82_BER_CNTr_t));
    PHYMOD_MEMSET(&pcs_mon_liv_sts, 0, sizeof(BCMI_MADURA_PCS_MON_LIVE_STSr_t));
    PHYMOD_MEMSET(&pcs_mon_sts, 0, sizeof(BCMI_MADURA_PCS_MON_STSr_t));

    *link_sts = 0xffff;

    PHYMOD_IF_ERR_RETURN(
         _madura_pcs_link_monitor_enable_get(pa, &get_pcs));

    if (get_pcs) {
        PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
        config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
           _madura_phy_interface_config_get(pa, 0, &config)));

        MADURA_GET_IP(pa, config, ip);

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
        max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

        PHYMOD_DEBUG_VERBOSE((" Max_lane:%d lanemask:0x%x\n",
                    max_lane, lane_mask));

        PHYMOD_DEBUG_VERBOSE(("Getting PCS Status\n"));
        for (lane = 0; lane < max_lane; lane ++) {
            if ((lane_mask & (1 << lane))) {
            /*Program Slice register dev1*/
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
            /*Check PCS live status*/
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_READ_PCS_MON_LIVE_STSr(pa,&pcs_mon_liv_sts)));
            if(BCMI_MADURA_PCS_MON_LIVE_STSr_PCS_STATUSf_GET(pcs_mon_liv_sts)){
                *link_sts &= 1;
                /* Check PCS lock*/
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_PCS_MON_STSr(pa,&pcs_mon_sts)));

                if(!BCMI_MADURA_PCS_MON_STSr_BLOCK_LOCK_LLf_GET(pcs_mon_sts)){
                    /* Loss of link : read error cntr*/
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_CL49_BER_STSr(pa,&ber_status)));
                    err_cnt = BCMI_MADURA_CL49_BER_STSr_BER_COUNTf_GET(ber_status);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_CL82_BER_CNTr(pa,&ber_cnt)));

                    err_cnt=BCMI_MADURA_CL82_BER_CNTr_BER_COUNT_MSBf_GET(ber_cnt);
                    *link_sts &= 0;
                    PHYMOD_DEBUG_VERBOSE(("Lane:%d Loss of lock: 1 Error Cnt:0x%x\n",
                                lane, err_cnt));
                }
            } else {
                *link_sts &= 0;
            }
        }
    }
    PHYMOD_FREE(config.device_aux_modes);
    } else {
        PHYMOD_DEBUG_VERBOSE(("PCS MON not Enabled, Reading PMD Status\n"));
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                  _madura_rx_pmd_lock_get(pa, link_sts)));
    }
    /*Reset DEV1 slice register*/
    PHYMOD_IF_ERR_RETURN(
     BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_fal_2x_falcon_lane_map_get(
        const phymod_access_t *pa,
        const phymod_phy_inf_config_t *cfg,
        uint32_t* fal_2x_lane_map,
        uint32_t* fal_lane_map)
{

    phymod_phy_inf_config_t config;
    uint16_t core = 0;
    uint32_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    lane_mask = (pa->lane_mask) ? pa->lane_mask : 0xf;
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    MADURA_GET_IP(pa, config, core);
    PHYMOD_DEBUG_VERBOSE(("%s :: core:%d\n", __func__, core));

    if (config.data_rate == MADURA_SPD_100G ||
        config.data_rate == MADURA_SPD_106G) {
        if(!aux_mode->alternate){
            *fal_2x_lane_map = 0xF;
            *fal_lane_map = 0xF;
        } else { /*alternate*/
            *fal_2x_lane_map = 0xF0;
            *fal_lane_map = 0xF;
        }
    } else if (core == MADURA_2X_FALCON_CORE) {
        if (config.data_rate == MADURA_SPD_40G || config.data_rate == MADURA_SPD_50G ||
            config.data_rate == MADURA_SPD_42G) {
            if ((aux_mode->pass_thru_dual_lane) && (!aux_mode->alternate)) {
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask;
            } else if ((aux_mode->pass_thru_dual_lane) && (aux_mode->alternate)) {
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask >> 4;
            } else  if ((aux_mode->pass_thru) && (!aux_mode->alternate)) {
                *fal_2x_lane_map = 0xf;
                *fal_lane_map = 0xF;
            } else if ((aux_mode->pass_thru) && (aux_mode->alternate)) {
                *fal_2x_lane_map = 0xF0;
                *fal_lane_map = 0xF;
            }
            else {
                *fal_2x_lane_map = lane_mask;
                if (lane_mask == 0xF)
                    *fal_lane_map = 0x3;
                else if(lane_mask == 0xF0)
                    *fal_lane_map = 0xC;
                else
                    *fal_lane_map = 0xF;
            }
        } else { /* 10G/11G */
            if (!aux_mode->alternate) {/*passthru*/
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask;
            } else {
                *fal_2x_lane_map = lane_mask;
                *fal_lane_map = lane_mask >> 4;
            }
        }
    }
    else {
         if (config.data_rate == MADURA_SPD_40G || config.data_rate == MADURA_SPD_50G ||
             config.data_rate == MADURA_SPD_42G) {
            if ((aux_mode->pass_thru_dual_lane) && (!aux_mode->alternate)) {
                *fal_lane_map = lane_mask;
                *fal_2x_lane_map = lane_mask;
            } else if ((aux_mode->pass_thru_dual_lane) && (aux_mode->alternate)) {
                *fal_lane_map = lane_mask;
                *fal_2x_lane_map = lane_mask << 4;;
            } else if ((aux_mode->pass_thru) && (!aux_mode->alternate)) {
                *fal_2x_lane_map = 0xf;
                *fal_lane_map = 0xF;
            } else if ((aux_mode->pass_thru) && (aux_mode->alternate)) {
                *fal_2x_lane_map = 0xF0;
                *fal_lane_map = 0xF;
            }
            else {
                *fal_lane_map = lane_mask;
                if (lane_mask == 0x3)
                    *fal_2x_lane_map = 0xF;
                else if(lane_mask == 0xC)
                    *fal_2x_lane_map = 0xF0;
                else
                    *fal_2x_lane_map = 0xFF;
            }
        } else { /* 10G/11G */
            if (!aux_mode->alternate) {
                *fal_lane_map = lane_mask;
                *fal_2x_lane_map = lane_mask;
            } else {
                *fal_lane_map = lane_mask;
                *fal_2x_lane_map = lane_mask << 4;
            }
        }
    }
    return PHYMOD_E_NONE;
}

int _madura_falcon_lpbk_get(
        const phymod_access_t *pa,
        phymod_loopback_mode_t loopback,
        uint32_t* enable,
         uint16_t ip)
{

    BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_t dig_lpbk;
    BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_t rmt_lpbk;
    PHYMOD_MEMSET(&dig_lpbk, 0, sizeof(BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_t));
    PHYMOD_MEMSET(&rmt_lpbk, 0, sizeof(BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_t));
    switch (loopback) {
        case phymodLoopbackGlobal:
            if(ip == MADURA_FALCON_CORE)
        {
            return PHYMOD_E_PARAM;
        }
        /* coverity[fallthrough] */
        case phymodLoopbackGlobalPMD:
            /* Falcon Digital loopback get */
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_TLB_RX_DIG_LPBK_CFGr(pa,&dig_lpbk));
        *enable=BCMI_MADURA_TLB_RX_DIG_LPBK_CFGr_DIG_LPBK_ENf_GET(dig_lpbk);
        break;
        case phymodLoopbackRemotePMD: /* Falcon Remote loopback get */
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_TLB_TX_RMT_LPBK_CFGr(pa,&rmt_lpbk));
        *enable=BCMI_MADURA_TLB_TX_RMT_LPBK_CFGr_RMT_LPBK_ENf_GET(rmt_lpbk);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _madura_loopback_get(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t* enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;

    *enable = 1;
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));

            /* FALCON core loopback get */
            PHYMOD_DEBUG_VERBOSE(("Falcon loopback get\n"));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_falcon_lpbk_get(pa, loopback, enable,ip)));
            break;
        }
    }
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_falcon_lpbk_set(const phymod_access_t* pa, const phymod_phy_inf_config_t *config,
                           phymod_loopback_mode_t loopback, uint32_t enable)
{
    /*This function sets loop of all types for both sides*/
    uint16_t lane = 0,max_lane=0;
    uint16_t ip = 0;
    uint32_t fal_2x_lane_mask = 0;
    uint32_t fal_lane_mask = 0;
    uint32_t main_lane_mask = 0,otherside_lane_mask = 0;
#ifdef DEB
    uint32_t data1;
#endif

    MADURA_GET_IP(pa, (*config), ip);
    max_lane = (ip == MADURA_FALCON_CORE) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_IF_ERR_RETURN(
        _madura_fal_2x_falcon_lane_map_get(pa, config, &fal_2x_lane_mask, &fal_lane_mask));
    main_lane_mask = (ip == MADURA_FALCON_CORE) ? fal_lane_mask : fal_2x_lane_mask;
    otherside_lane_mask = (ip == MADURA_FALCON_CORE) ? fal_2x_lane_mask : fal_lane_mask;

    PHYMOD_DEBUG_VERBOSE(("%s at %d : main_lane_mask=%x otherside_lane_mask=%x\n",FUNCTION_NAME(),__LINE__,main_lane_mask,otherside_lane_mask));
    PHYMOD_DEBUG_VERBOSE(("Falcon loopback set\n"));

    switch (loopback) {
        case phymodLoopbackGlobal:
        if( ip == MADURA_FALCON_CORE)
        {
                return PHYMOD_E_PARAM;
        }
        /* coverity[fallthrough] */
        case phymodLoopbackRemotePMD: /* Falcon remote loopback set */
            for(lane = 0; lane < max_lane; lane ++) {
                if (main_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa,
                                ip, MADURA_DEV_PMA_PMD,
                                 lane, 0));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d172,&data1);
                        printf("[--d172-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_rmt_lpbk(pa, enable));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d172,&data1);
                        printf("[--d172-%x--]\n",data1);
#endif
                }
            }
            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
        break;
        case phymodLoopbackRemotePCS:
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodLoopbackGlobalPMD: /* Falcon digital loopback set */
        {

          /*Flipping ip,maxlane and lanemask*/
            for(lane = 0; lane < ((max_lane%8) +4)/*if 4 then 8 and vice versa*/; lane ++) {
                if (otherside_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa,
                                ((ip==0)?1:0), MADURA_DEV_PMA_PMD,  lane, 0));/*send toggled value of IP*/

#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_dig_lpbk_rptr(pa, enable, DATA_IN_SIDE));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[+-d162-%x--]\n",data1);
#endif
                }
            }
            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);

            for(lane = 0; lane < max_lane; lane ++) {
                if (main_lane_mask & (1 << lane)) {
                    PHYMOD_IF_ERR_RETURN(
                      _madura_set_slice_reg (pa,
                                ip, MADURA_DEV_PMA_PMD,
                                 lane, 0));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(falcon2_madura_dig_lpbk_rptr(pa, enable, DIG_LPBK_SIDE));
                    /* Disable the prbs_chk_en_auto_mode while seting digital loopback. Below is also not in validation.Keeping for now  */
                    PHYMOD_IF_ERR_RETURN(wr_falcon2_madura_prbs_chk_en_auto_mode((!enable)));
#ifdef DEB
                        PHYMOD_BUS_READ(pa,0x1d162,&data1);
                        printf("[--d162-%x--]\n",data1);
#endif
                    PHYMOD_IF_ERR_RETURN(wr_falcon2_madura_sdk_tx_disable(enable));
                    if (!enable) {
                        PHYMOD_IF_ERR_RETURN(wr_falcon2_madura_ln_dp_s_rstb(0));
                        PHYMOD_USLEEP(10);
                        PHYMOD_IF_ERR_RETURN(wr_falcon2_madura_ln_dp_s_rstb(1));
                    }
                }
            }
            PHYMOD_USLEEP(1000);

            MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
        }
        break;
        default :
        break;
    }

    return PHYMOD_E_NONE;
}

int _madura_loopback_set(const phymod_access_t *pa, phymod_loopback_mode_t loopback, uint32_t enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lp_bck_val_misc = 0;
    uint16_t data = 0, retry_cnt = 5;
    MADURA_DEVICE_AUX_MODE_T aux_mode;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = &aux_mode;
    PHYMOD_IF_ERR_RETURN(
            _madura_phy_interface_config_get(pa, 0, &config));
    MADURA_GET_IP(pa, config, ip);
    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON"));

    PHYMOD_DEBUG_VERBOSE(("madura looopback set\n"));
   retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
        data=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
    }

    /* FALCON core loopback set */
    PHYMOD_IF_ERR_RETURN(
            _madura_falcon_lpbk_set(pa, &config, loopback, enable));

    /* Set bit 1 for loopback change */
    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        lp_bck_val_misc = BCMI_MADURA_MST_MISC0r_MISC0f_GET(mst_misc0);
        lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC0r_MISC0f_SET(mst_misc0,lp_bck_val_misc);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC0r(pa, mst_misc0));
    } else  if (lane_mask == 0x2) {
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        lp_bck_val_misc = BCMI_MADURA_MST_MISC1r_MISC1f_GET(mst_misc1);
        lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
                BCMI_MADURA_MST_MISC1r_MISC1f_SET(mst_misc1,lp_bck_val_misc);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC1r(pa, mst_misc1));
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
             PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        lp_bck_val_misc = BCMI_MADURA_MST_MISC2r_MISC2f_GET(mst_misc2);
        lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC2r_MISC2f_SET(mst_misc2,lp_bck_val_misc);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC2r(pa, mst_misc2));
    } else {
            PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        lp_bck_val_misc = BCMI_MADURA_MST_MISC3r_MISC3f_GET(mst_misc3);
        lp_bck_val_misc  = lp_bck_val_misc | 0x80 ;/*enable loopback as 1*/
        BCMI_MADURA_MST_MISC3r_MISC3f_SET(mst_misc3,lp_bck_val_misc);
        PHYMOD_IF_ERR_RETURN(
                BCMI_MADURA_WRITE_MST_MISC3r(pa, mst_misc3));
    }

    /*  Set FW_ENABLE = 1 */
   retry_cnt = MADURA_FW_DLOAD_RETRY_CNT;
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en,1);
    PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));
   do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
        data=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("loopback config failed, micro controller is busy..")));
    }

    return PHYMOD_E_NONE;
}


int _madura_falcon_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_t pwrdwn_ctrl_tx;
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_t pwrdwn_ctrl_rx;
    PHYMOD_MEMSET(&pwrdwn_ctrl_tx, 0, sizeof(BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_t));
    PHYMOD_MEMSET(&pwrdwn_ctrl_rx, 0, sizeof(BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_t));
    /* TX AFE Lane Reset */
    switch (reset->tx) {
        /* In Reset */
        case phymodResetDirectionIn:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,1);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
        break;
        /* Out Reset */
        case phymodResetDirectionOut:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,0);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
            /* -- Releasing forces -- */
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,1);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
        PHYMOD_USLEEP(10);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,0);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
            /* -- Releasing forces -- */
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx));
        break;
        default:
        break;
    }
    /* RX AFE Lane Reset */
    switch (reset->rx) {
        /* In Reset */
        case phymodResetDirectionIn:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,1);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
        break;
        /* Out Reset */
    case phymodResetDirectionOut:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,0);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
            /* -- Releasing forces -- */
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,0);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
        break;
        /* Toggle Reset */
        case phymodResetDirectionInOut:
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,1);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
            PHYMOD_USLEEP(10);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,0);
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
            /* -- Releasing forces -- */
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx));
        BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,0);
        PHYMOD_IF_ERR_RETURN(
         BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx));
        break;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}


int _madura_fal_2x_phy_reset_set(const phymod_access_t *pa, const phymod_phy_reset_t* reset)
{
    return PHYMOD_E_NONE;
}

int _madura_phy_reset_set(const phymod_access_t *phy, const phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(phy);
    MADURA_GET_IP(phy, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
			MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (phy,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
                /* FALCON TX/RX Reset */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset set\n"));
				MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_falcon_phy_reset_set(phy, reset)));
        }
    }

    MADURA_RESET_SLICE(phy, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}
int _madura_phy_reset_get(const phymod_access_t *pa, phymod_phy_reset_t* reset)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t rst = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t rx_get;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t tx_get;

    PHYMOD_MEMSET(&rx_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t));
    PHYMOD_MEMSET(&tx_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));


    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
			MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
            /* FALCON TX/RX Reset */
            PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Reset get\n"));
			MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr(pa,&tx_get)));
            rst=BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_AFE_TX_RESETf_GET(tx_get);
            if(rst == 0) {
                /* Out of Reset */
                reset->tx = phymodResetDirectionOut;
            } else {
                /* In Reset */
                reset->tx = phymodResetDirectionIn;
            }
			MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr(pa,&rx_get)));
            rst=BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_AFE_RX_RESETf_GET(rx_get);
            if(rst == 0) {
                /* Out of Reset */
                reset->rx = phymodResetDirectionOut;
            } else {
                /* In Reset */
                reset->rx = phymodResetDirectionIn;
            }

        }
    }
    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}


int _madura_force_tx_training_set(const phymod_access_t *pa, uint32_t enable)
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t data1 = 0, retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    /* Set TX training en */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
    _madura_force_training_set(pa, ip, &config, (uint16_t)enable )));

    /* Set FW en */
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en)));

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_force_tx_training_get(const phymod_access_t *pa, uint32_t *enable)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, training_en = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    MADURA_GET_IP(pa, config, ip);
    /* Get TX training en */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
    _madura_force_training_get(pa, ip, &config, &training_en)));

    *enable = training_en ? 1 : 0 ;

    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_force_tx_training_status_get(const phymod_access_t *pa, phymod_cl72_status_t* status)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_CL93N72_UR_CTL0r_t ur_ctrl0;
    BCMI_MADURA_CL93N72_UT_CTL0r_t ut_ctrl0;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&ur_ctrl0, 0, sizeof(BCMI_MADURA_CL93N72_UR_CTL0r_t));
    PHYMOD_MEMSET(&ut_ctrl0, 0, sizeof(BCMI_MADURA_CL93N72_UT_CTL0r_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* init status */
    status->enabled = 0x1;
    status->locked = 0x1;
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_CL93N72_UR_CTL0r(pa, &ur_ctrl0)));

            status->enabled &=
            BCMI_MADURA_CL93N72_UR_CTL0r_CL93N72_RX_TRAINING_ENf_GET(ur_ctrl0);

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_CL93N72_UT_CTL0r(pa, &ut_ctrl0)));

            status->locked &=
            BCMI_MADURA_CL93N72_UT_CTL0r_CL93N72_SW_RX_TRAINEDf_GET(ut_ctrl0);
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;

}
int _madura_phy_status_dump(const phymod_access_t *pa)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", lane_mask));

    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("******* PHY status dump for PHY ID:0x%x ********\n",pa->addr));
    USR_PRINTF(("**********************************************\n"));
    USR_PRINTF(("**** PHY status dump for interface side:%d ****\n",ip));
    USR_PRINTF(("***********************************************\n"));

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_core_config(pa)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_core_state(pa)));
            break;
        }
    }
    for(lane = 0; lane < max_lane ; lane ++) {
            if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
                PHYMOD_DEBUG_VERBOSE(("Falcon Status dump\n"));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_state_hdr(pa)));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_state(pa)));
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (falcon2_madura_display_lane_config(pa)));
            }
    }

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_tx_enable_set(const phymod_access_t *pa, int16_t enable)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_t fal_ctrl1;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fal_ctrl1, 0, sizeof(BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* TX squelch set with pmd_tx_disable_0_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_ctrl1)));

            BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCf_SET(fal_ctrl1, 1);
            BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_SET(fal_ctrl1, enable == MADURA_DISABLE ? 1 : 0);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_WRITE_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, fal_ctrl1)));
       }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}

int _madura_tx_enable_get(const phymod_access_t *pa, int16_t *enable)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_t fal_ctrl1;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fal_ctrl1, 0, sizeof(BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* TX squelch get with pmd_tx_disable_0_frcval */
    *enable = MADURA_ENABLE;
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_FALCON_IF_PER_LN_CTL1_PER_LANEr(pa, &fal_ctrl1)));

            if (BCMI_MADURA_FALCON_IF_PER_LN_CTL1_PER_LANEr_PMD_TX_DISABLE_FRCVALf_GET(fal_ctrl1) )  {
                *enable = MADURA_DISABLE ;
                break;
            }

        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}


int _madura_tx_dp_reset(const phymod_access_t *pa)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_t   pwrdwn_ctrl_tx;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&pwrdwn_ctrl_tx, 0, sizeof(pwrdwn_ctrl_tx));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* TX reset with tx_reset_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,1);
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx)));
            PHYMOD_USLEEP(10);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRC_VALf_SET(pwrdwn_ctrl_tx,0);
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,1);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx)));
                /* -- Releasing forces -- */
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,&pwrdwn_ctrl_tx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_TXr_AFE_TX_RESET_FRCf_SET(pwrdwn_ctrl_tx,0);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_TXr(pa,pwrdwn_ctrl_tx)));
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_rx_enable_set(const phymod_access_t *pa, int16_t enable)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_SIGDET_CTL1r_t fal_ctrl1;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fal_ctrl1, 0, sizeof(BCMI_MADURA_SIGDET_CTL1r_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* TX squelch set with pmd_tx_disable_0_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_SIGDET_CTL1r(pa, &fal_ctrl1)));

            BCMI_MADURA_SIGDET_CTL1r_SIGNAL_DETECT_FRCf_SET(fal_ctrl1, 1);
            BCMI_MADURA_SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_SET(fal_ctrl1, enable == MADURA_DISABLE ? 0 : 1);

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_WRITE_SIGDET_CTL1r(pa, fal_ctrl1)));
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_rx_enable_get(const phymod_access_t *pa, int16_t *enable)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_SIGDET_CTL1r_t fal_ctrl1;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fal_ctrl1, 0, sizeof(BCMI_MADURA_SIGDET_CTL1r_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* RX _frcval */
    *enable = MADURA_ENABLE;
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
             BCMI_MADURA_READ_SIGDET_CTL1r(pa, &fal_ctrl1)));

            if(
              (BCMI_MADURA_SIGDET_CTL1r_SIGNAL_DETECT_FRCf_GET(fal_ctrl1)==1)&&
              (BCMI_MADURA_SIGDET_CTL1r_SIGNAL_DETECT_FRC_VALf_GET(fal_ctrl1)==0)) {
                *enable = MADURA_DISABLE;
                break;
            }
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_rx_dp_reset(const phymod_access_t *pa)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0, max_lane = 0;
    uint16_t lane_mask = 0;
    BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_t   pwrdwn_ctrl_rx;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&pwrdwn_ctrl_rx, 0, sizeof(pwrdwn_ctrl_rx));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    /* RX reset with rx_reset_frcval */
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa, ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,1);
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx)));
            PHYMOD_USLEEP(10);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRC_VALf_SET(pwrdwn_ctrl_rx,0);
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,1);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx)));
                /* -- Releasing forces -- */
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,&pwrdwn_ctrl_rx)));
            BCMI_MADURA_LN_AFE_RST_PWRDWN_CTL_CTL_RXr_AFE_RX_RESET_FRCf_SET(pwrdwn_ctrl_rx,0);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_LN_AFE_RST_PWRDWN_CTL_CTL_RXr(pa,pwrdwn_ctrl_rx)));
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}


int _madura_tx_lane_control_set(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t tx_control)
{
    switch (tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
            return PHYMOD_E_UNAVAIL;
        case phymodTxReset: /* reset tx data path */
            PHYMOD_IF_ERR_RETURN(_madura_tx_dp_reset(pa));
        break;
        case phymodTxSquelchOn: /* squelch tx */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_set(pa, MADURA_DISABLE));
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_set(pa, MADURA_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _madura_rx_lane_control_set(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t rx_control)
{
    switch (rx_control) {
        case phymodRxReset: /* reset rx data path */
            PHYMOD_IF_ERR_RETURN(_madura_rx_dp_reset(pa));
            break;
        case phymodRxSquelchOn: /* squelch rx */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_set(pa, MADURA_DISABLE));
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_set(pa, MADURA_ENABLE));
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int _madura_tx_lane_control_get(const phymod_access_t *pa,  phymod_phy_tx_lane_control_t *tx_control)
{
    int16_t tx_ctrl = 0;
    switch (*tx_control) {
        case phymodTxTrafficDisable: /* disable tx traffic */
        case phymodTxTrafficEnable: /* enable tx traffic */
        case phymodTxReset: /* reset tx data path */
            return PHYMOD_E_UNAVAIL;
        break;
        case phymodTxSquelchOn: /* squelch tx */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_get(pa, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        case phymodTxSquelchOff: /* squelch tx off */
            PHYMOD_IF_ERR_RETURN(_madura_tx_enable_get(pa, &tx_ctrl));
            *tx_control = tx_ctrl ? phymodTxSquelchOff: phymodTxSquelchOn;
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}
int _madura_rx_lane_control_get(const phymod_access_t *pa,  phymod_phy_rx_lane_control_t *rx_control)
{
    int16_t rx_ctrl = 0;
    switch (*rx_control) {
        case phymodRxReset: /* reset rx data path */
            return PHYMOD_E_UNAVAIL;
        case phymodRxSquelchOn: /* squelch rx */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_get(pa, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        case phymodRxSquelchOff: /* squelch rx off */
            PHYMOD_IF_ERR_RETURN(_madura_rx_enable_get(pa, &rx_ctrl));
            *rx_control = rx_ctrl ? phymodRxSquelchOff: phymodRxSquelchOn;
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}


int _madura_config_hcd_link_sts (const phymod_access_t* pa, phymod_phy_inf_config_t config, uint16_t ip, uint16_t port )
{
    BCMI_MADURA_ANARXCTL3r_t ana_rxctl3;

    PHYMOD_MEMSET(&ana_rxctl3, 0, sizeof(BCMI_MADURA_ANARXCTL3r_t));

    if (pa->lane_mask & 0xF) {
		PHYMOD_IF_ERR_RETURN(
           _madura_set_slice_an_reg (pa,ip,
                                     MADURA_DEV_AN, 0, port));
    } else {
		PHYMOD_IF_ERR_RETURN(
           _madura_set_slice_an_reg (pa,ip,
                                     MADURA_DEV_AN, 4, port));
    }

    /* HCD_LINK_REG_WR */
		PHYMOD_IF_ERR_RETURN(
                            BCMI_MADURA_READ_ANARXCTL3r(pa,&ana_rxctl3));
		BCMI_MADURA_ANARXCTL3r_USE_KR4_PMD_LOCK_SMf_SET(ana_rxctl3,1);
		PHYMOD_IF_ERR_RETURN(
                            BCMI_MADURA_WRITE_ANARXCTL3r(pa,ana_rxctl3));

    return PHYMOD_E_NONE;
}

int _madura_autoneg_set(const phymod_phy_access_t* phy, const phymod_autoneg_control_t* an)
{
    const phymod_access_t* pa = &phy->access;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t data1 = 0, retry_cnt = MADURA_AN_SET_RETRY_CNT;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    if (ip == MADURA_FALCON_CORE) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    if (an->an_mode != phymod_AN_MODE_CL73) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN mode not supported */
        return PHYMOD_E_PARAM;
    }

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_AN_SET_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    /* Set AN en */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
    _madura_autoneg_set_val(pa, ip, &config, an->enable ? 1 : 0)));

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_udms_config(phy, &config,
                            an->enable ?  MADURA_AN_CL73 : MADURA_AN_NONE)));

    /* Set FW en */
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en)));

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_AN_SET_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_autoneg_get(const phymod_access_t* pa, phymod_autoneg_control_t* an, uint32_t *an_done)
{
    BCMI_MADURA_AN_STSr_t an_status;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, an_enable = 0;
    uint16_t port = 0, physical_port = 0;

    /* Init default values */
    an->enable = 0;
    *an_done   = 0;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&an_status, 0, sizeof(BCMI_MADURA_AN_STSr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    if (ip == MADURA_FALCON_CORE) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }

    /* Get AN en */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
    _madura_autoneg_get_val(pa, ip, &config, &an_enable )));

    /* Get AN en */
    an->enable = an_enable ? 1 : 0 ;

    /* AN Mode */
    an->an_mode = phymod_AN_MODE_CL73;

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_port_from_lane_map_get(pa, &config, &port, &physical_port)));
    if (pa->lane_mask & 0xF) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
           _madura_set_slice_an_reg (pa,ip,
                                     MADURA_DEV_AN, 0, physical_port)));
    } else {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
           _madura_set_slice_an_reg (pa,ip,
                                     MADURA_DEV_AN, 4, physical_port)));
    }


    /* AN done */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_AN_STSr(pa, &an_status)));

    *an_done =
        BCMI_MADURA_AN_STSr_AUTO_NEGOTIATIONCOMPLETEf_GET(an_status) ? 1 : 0;


    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_autoneg_ability_set(const phymod_access_t* pa, madura_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, port = 0, physical_port = 0;
    BCMI_MADURA_AN_ADVERTISEMENT1r_t  adv1;
    BCMI_MADURA_AN_ADVERTISEMENT2r_t  adv2;
    BCMI_MADURA_AN_ADVERTISEMENT3r_t  adv3;
    BCMI_MADURA_AN_SLICEr_t slice_reg;
    BCMI_MADURA_P0_MODE_CTLr_t p0_ctl;
    BCMI_MADURA_P2_MODE_CTLr_t p2_ctl;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_AN_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&adv1, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT1r_t));
    PHYMOD_MEMSET(&adv2, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT2r_t));
    PHYMOD_MEMSET(&adv3, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT3r_t));
    PHYMOD_MEMSET(&p0_ctl, 0, sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&p2_ctl, 0, sizeof(BCMI_MADURA_P2_MODE_CTLr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));


    MADURA_GET_IP(pa, config, ip);

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_port_from_lane_map_get(pa, &config, &port, &physical_port)));

    if (ip == MADURA_FALCON_CORE) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }
    if (pa->lane_mask & 0xF) {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 0, physical_port)));
         MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (BCMI_MADURA_READ_P0_MODE_CTLr(pa, &p0_ctl)));
         if (an_ability->an_master_lane < 4) {
             BCMI_MADURA_P0_MODE_CTLr_P0_LALf_SET(p0_ctl, an_ability->an_master_lane);
         }
         MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (BCMI_MADURA_WRITE_P0_MODE_CTLr(pa, p0_ctl)));
    } else {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 4 , physical_port)));
         MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (BCMI_MADURA_READ_P2_MODE_CTLr(pa, &p2_ctl)));
         if (an_ability->an_master_lane < 4) {
             BCMI_MADURA_P2_MODE_CTLr_P2_LALf_SET(p2_ctl, an_ability->an_master_lane);
         }
         MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (BCMI_MADURA_WRITE_P2_MODE_CTLr(pa, p2_ctl)));

    }


    /* Adv1 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT1r(pa, &adv1)));

    BCMI_MADURA_AN_ADVERTISEMENT1r_PAUSEf_SET(adv1,
                                              an_ability->cl73_adv.an_pause);

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_WRITE_AN_ADVERTISEMENT1r(pa, adv1)));

    /* Adv2 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT2r(pa, &adv2)));

    BCMI_MADURA_AN_ADVERTISEMENT2r_TECHABILITYf_SET(
                                        adv2,
                                        an_ability->cl73_adv.an_base_speed);

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_WRITE_AN_ADVERTISEMENT2r(pa, adv2)));

    /* Adv3 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT3r(pa, &adv3)));

    BCMI_MADURA_AN_ADVERTISEMENT3r_FEC_REQUESTEDf_SET(
                                        adv3,
                                        an_ability->cl73_adv.an_fec);

    BCMI_MADURA_AN_ADVERTISEMENT3r_AN_ADV_D45_D32f_SET(adv3, 0);

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_WRITE_AN_ADVERTISEMENT3r(pa, adv3)));

    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        _madura_config_hcd_link_sts (pa, config, ip, physical_port)));

    PHYMOD_FREE(config.device_aux_modes);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_AN_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}

int _madura_autoneg_ability_get(const phymod_access_t* pa, madura_an_ability_t *an_ability)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0, port = 0, physical_port = 0 ;
    BCMI_MADURA_AN_ADVERTISEMENT1r_t  adv1;
    BCMI_MADURA_AN_ADVERTISEMENT2r_t  adv2;
    BCMI_MADURA_AN_ADVERTISEMENT3r_t  adv3;
    BCMI_MADURA_AN_SLICEr_t slice_reg;
    BCMI_MADURA_P0_MODE_CTLr_t p0_mod;
    BCMI_MADURA_P2_MODE_CTLr_t p2_mod;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_AN_SLICEr_t));
    PHYMOD_MEMSET(&p0_mod, 0 , sizeof(BCMI_MADURA_P0_MODE_CTLr_t));
    PHYMOD_MEMSET(&p2_mod, 0 , sizeof(BCMI_MADURA_P2_MODE_CTLr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&adv1, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT1r_t));
    PHYMOD_MEMSET(&adv2, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT2r_t));
    PHYMOD_MEMSET(&adv3, 0, sizeof(BCMI_MADURA_AN_ADVERTISEMENT3r_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    if (ip == MADURA_FALCON_CORE) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_port_from_lane_map_get(pa, &config, &port, &physical_port)));

    if (pa->lane_mask & 0xF) {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 0, physical_port )));
    } else {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 0, physical_port )));
    }


    /* Adv1 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT1r(pa, &adv1)));

    an_ability->cl73_adv.an_pause =
        BCMI_MADURA_AN_ADVERTISEMENT1r_PAUSEf_GET(adv1);

    /* Adv2 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT2r(pa, &adv2)));

    an_ability->cl73_adv.an_base_speed =
        BCMI_MADURA_AN_ADVERTISEMENT2r_TECHABILITYf_GET(adv2);

    /* Adv3 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_ADVERTISEMENT3r(pa, &adv3)));

    an_ability->cl73_adv.an_fec =
        BCMI_MADURA_AN_ADVERTISEMENT3r_FEC_REQUESTEDf_GET(adv3);
    if (pa->lane_mask & 0xF) {
         if (an_ability->an_master_lane < 4) {
             MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                         BCMI_MADURA_READ_P0_MODE_CTLr(pa,&p0_mod)));
             an_ability->an_master_lane = BCMI_MADURA_P0_MODE_CTLr_P0_LALf_GET(p0_mod);
         }
    } else {
         if (an_ability->an_master_lane < 4) {
             MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                         BCMI_MADURA_READ_P2_MODE_CTLr(pa,&p2_mod)));
             an_ability->an_master_lane = BCMI_MADURA_P2_MODE_CTLr_P2_LALf_GET(p2_mod);
         }
    }

    PHYMOD_FREE(config.device_aux_modes);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_AN_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}


int _madura_autoneg_remote_ability_get(
    const phymod_access_t *pa,
    phymod_autoneg_ability_t* an_ability)
{

    phymod_phy_inf_config_t config;
    uint16_t ip = 0, port = 0, physical_port = 0;
    BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_1r_t  adv1;
    BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_2r_t  adv2;
    BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_3r_t  adv3;
    BCMI_MADURA_AN_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_AN_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&adv1, 0, sizeof(BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_1r_t));
    PHYMOD_MEMSET(&adv2, 0, sizeof(BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_2r_t));
    PHYMOD_MEMSET(&adv3, 0, sizeof(BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_3r_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    if (ip == MADURA_FALCON_CORE) {
        PHYMOD_FREE(config.device_aux_modes);
        /* AN can performed only on line side */
        return PHYMOD_E_PARAM;
    }
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_port_from_lane_map_get(pa, &config, &port, &physical_port)));

    if (pa->lane_mask & 0xF) {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 0 , physical_port)));
    } else {
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
          _madura_set_slice_an_reg (pa,
                    ip, MADURA_DEV_AN, 4 , physical_port)));
    }

    /* Adv1 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_LP_BASE_PAGE_ABIL_1r(pa, &adv1)));

    if (BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_1r_D15_D0f_GET(adv1) & 0x400) {
            PHYMOD_AN_CAP_ASYM_PAUSE_SET(an_ability);
    } else if (BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_1r_D15_D0f_GET(adv1) & 0x800) {
            PHYMOD_AN_CAP_SYMM_PAUSE_SET(an_ability);
    }


    /* Adv2 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_LP_BASE_PAGE_ABIL_2r(pa, &adv2)));

    switch ((BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_2r_GET(adv2) & 0xFFE0) >> 5) {
        case MADURA_CL73_40GBASE_CR4:
            PHYMOD_AN_CAP_40G_CR4_SET(an_ability->an_cap);
        break;
        case MADURA_CL73_40GBASE_KR4:
            PHYMOD_AN_CAP_40G_KR4_SET(an_ability->an_cap);
        break;
        case MADURA_CL73_100GBASE_CR4:
            PHYMOD_AN_CAP_100G_CR4_SET(an_ability->an_cap);
        break;
        case MADURA_CL73_100GBASE_KR4:
            PHYMOD_AN_CAP_100G_KR4_SET(an_ability->an_cap);
        break;
        default:
            break;
    }

    /* Adv3 */
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_AN_LP_BASE_PAGE_ABIL_3r(pa, &adv3)));

    an_ability->an_fec =
        (BCMI_MADURA_AN_LP_BASE_PAGE_ABIL_3r_GET(adv3) & 0xC000 ) >> 15 ;

    PHYMOD_FREE(config.device_aux_modes);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_AN_SLICEr(pa, slice_reg));

    return PHYMOD_E_NONE;
}

int _madura_port_from_lane_map_get(const phymod_access_t *pa, const phymod_phy_inf_config_t* cfg, uint16_t *port, uint16_t *physical_port)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane_mask = 0;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    PHYMOD_MEMCPY(&config, cfg, sizeof(phymod_phy_inf_config_t));
    aux_mode = (MADURA_DEVICE_AUX_MODE_T*)cfg->device_aux_modes;
    MADURA_GET_IP(pa, config, ip);

    PHYMOD_DEBUG_VERBOSE(("%s:: IP:%s \n", __func__,
                 (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON"));

    if (config.data_rate == MADURA_SPD_100G ||
        config.data_rate == MADURA_SPD_106G) {
        *port = 0;
        *physical_port = 0;
    } else if (config.data_rate == MADURA_SPD_40G || config.data_rate == MADURA_SPD_50G ||
               config.data_rate == MADURA_SPD_42G) {
        if (ip == MADURA_FALCON_CORE) {
            if (aux_mode->pass_thru_dual_lane  ){
                if (lane_mask == 0x3) {        /* port 0 */
                    *port = 0;
                } else if (lane_mask == 0xC) { /* port 1 */
                    *port = 1;
                }
            } else if (aux_mode->pass_thru  ){
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
        } else {/*Line side*/
            if (aux_mode->pass_thru_dual_lane  ){
                if (!aux_mode->alternate){
                    if (lane_mask == 0x3) {        /* port 0 */
                        *port = 0;
            *physical_port = 0;
                    } else if (lane_mask == 0xC) { /* port 1 */
                        *port = 1;
            *physical_port = 2;
                    }
                } else {
                    if (lane_mask == 0x30) {        /* port 0 */
                        *port = 0;
            *physical_port = 0;
                    } else if (lane_mask == 0xC0) { /* port 1 */
                        *port = 1;
            *physical_port = 2;
                    }
                }
            } else if (aux_mode->pass_thru ) {
                if (lane_mask == 0xf0) {       /* port 0 . alternate*/
                    *port = 0;
                *physical_port = 0;
                }
                else if (lane_mask == 0xF) {   /* port 0 */
                    *port = 0;
                *physical_port = 0;
                }
            } else {   /* 40G MUX */
                if (lane_mask == 0xF) {        /* port 0 */
                    *port = 0;
                *physical_port = 0;
                } else if (lane_mask == 0xF0) { /* port 1 */
                    *port = 1;
                *physical_port = 2;
                }
            }
        }
    } else /* 10G/11G if (config.data_rate == MADURA_SPD_10G)*/ {
        if (ip == MADURA_FALCON_CORE) {
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
            } else if (lane_mask == 0x4) {  /* port 2 */
                *port = 2;
            } else if (lane_mask == 0x8) {  /* port 3 */
                *port = 3;
            } else if ((lane_mask == 0x10) && (aux_mode->alternate)){
                *port = 0;
            } else if ((lane_mask == 0x20) && (aux_mode->alternate)){
                *port = 1;
            } else if ((lane_mask == 0x40) && (aux_mode->alternate)){
                *port = 2;
            } else if ((lane_mask == 0x80) && (aux_mode->alternate)){
                *port = 3;
            }
        }
    }

    return PHYMOD_E_NONE;
}

int _madura_tx_rx_polarity_set(const phymod_access_t *pa, uint32_t tx_polarity, uint32_t rx_polarity)
{
    BCMI_MADURA_SLICEr_t slice_reg;
    BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t tx_pl_cfg;
    BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t rx_pl_cfg;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0,intf_side=0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    uint32_t rx_lane_polarity = 0;
    uint32_t tx_lane_polarity = 0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&tx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t));
    PHYMOD_MEMSET(&rx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t));
    if (rx_polarity == 0xFFFF && tx_polarity == 0xFFFF) {
        return PHYMOD_E_NONE;
    }

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);
    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;


    /* FALCON polarity inversion */
    PHYMOD_DEBUG_VERBOSE(("Falcon polarity inversion set tx=%x rx=%xat %d\n",tx_polarity,rx_polarity,__LINE__));
    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
            if (rx_polarity != 0xFFFF) {
                rx_lane_polarity = (rx_polarity >> lane) & 0x1;
            /* Write to Rx misc config register */
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_READ_TLB_RX_TLB_RX_MISC_CFGr(pa,&rx_pl_cfg)));
        BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_SET(rx_pl_cfg,rx_lane_polarity);
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            BCMI_MADURA_WRITE_TLB_RX_TLB_RX_MISC_CFGr(pa,rx_pl_cfg)));
            }
            if (tx_polarity != 0xFFFF) {
                tx_lane_polarity = (tx_polarity >> lane) & 0x1;
                /* Write to Tx misc config register */
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_READ_TLB_TX_TLB_TX_MISC_CFGr(pa,&tx_pl_cfg)));
        BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_SET(tx_pl_cfg,tx_lane_polarity);
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    BCMI_MADURA_WRITE_TLB_TX_TLB_TX_MISC_CFGr(pa,tx_pl_cfg)));
            }
        }
    }
    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return PHYMOD_E_NONE;
}

int _madura_tx_rx_polarity_get(const phymod_access_t *phy, uint32_t *tx_polarity, uint32_t *rx_polarity)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t tx_pl_cfg;
    BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t rx_pl_cfg;

    *tx_polarity = 0;
    *rx_polarity = 0;

    PHYMOD_MEMSET(&tx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_t));
    PHYMOD_MEMSET(&rx_pl_cfg, 0 , sizeof(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    max_lane = (ip == MADURA_FALCON_CORE) ?
                   MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE;

    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"2X_FALCON":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
              ip, MADURA_DEV_PMA_PMD, lane, 0)));
        /* FALCON polarity */
        PHYMOD_DEBUG_VERBOSE(("Falcon polarity get\n"));
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_TLB_TX_TLB_TX_MISC_CFGr(pa,&tx_pl_cfg)));
        MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
        BCMI_MADURA_READ_TLB_RX_TLB_RX_MISC_CFGr(pa,&rx_pl_cfg)));
        /* Read to Tx misc config register */
        *tx_polarity|=(BCMI_MADURA_TLB_TX_TLB_TX_MISC_CFGr_TX_PMD_DP_INVERTf_GET(tx_pl_cfg))<<lane;
        /* Read to Rx misc config register */
        *rx_polarity|=(BCMI_MADURA_TLB_RX_TLB_RX_MISC_CFGr_RX_PMD_DP_INVERTf_GET(rx_pl_cfg))<<lane;
    }
    }

    MADURA_RESET_SLICE(pa, MADURA_DEV_PMA_PMD);
    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

 int _madura_pll_sequencer_restart(const phymod_core_access_t *core, phymod_sequencer_operation_t operation)
{
    const phymod_access_t *pa = &core->access;
    BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_t pll_seq;
    BCMI_MADURA_SLICEr_t slice_reg;
    unsigned int alt = 0,lane = 0;

    PHYMOD_MEMSET(&pll_seq, 0 , sizeof(BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_t));
    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));

    alt = ( pa->lane_mask & 0xf0 )? 1 : 0;
    lane = ( alt == 0 )? 1 : 4 ;

    switch(operation) {
        case phymodSeqOpStop:
        case phymodSeqOpStart:
            return PHYMOD_E_UNAVAIL;
        /* Toggle PLL Sequencer with pmd_core_dp_h_rstb_frcval */
        case phymodSeqOpRestart:
        PHYMOD_IF_ERR_RETURN(_madura_set_slice_reg (pa,
            0, MADURA_DEV_PMA_PMD, lane ,0));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CORE_PLL_TOP_USER_CTLr(pa,&pll_seq));
        BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(pll_seq,0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_CORE_PLL_TOP_USER_CTLr(pa,pll_seq));
        PHYMOD_IF_ERR_RETURN(_madura_set_slice_reg (pa,
            1, MADURA_DEV_PMA_PMD, 0x1, 0));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CORE_PLL_TOP_USER_CTLr(pa,&pll_seq));
        BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(pll_seq,0);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_CORE_PLL_TOP_USER_CTLr(pa,pll_seq));

        PHYMOD_USLEEP(1000);
        PHYMOD_IF_ERR_RETURN(_madura_set_slice_reg (pa,
            0, MADURA_DEV_PMA_PMD, lane, 0));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CORE_PLL_TOP_USER_CTLr(pa,&pll_seq));
        BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(pll_seq,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_CORE_PLL_TOP_USER_CTLr(pa,pll_seq));
      PHYMOD_IF_ERR_RETURN(_madura_set_slice_reg (pa,
            1, MADURA_DEV_PMA_PMD, 0x1, 0));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_CORE_PLL_TOP_USER_CTLr(pa,&pll_seq));
        BCMI_MADURA_CORE_PLL_TOP_USER_CTLr_CORE_DP_S_RSTBf_SET(pll_seq,1);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_CORE_PLL_TOP_USER_CTLr(pa,pll_seq));

        break;
        default:
        break;
    }

    return PHYMOD_E_NONE;
}

int _madura_fec_enable_set(const phymod_access_t *phy, uint32_t enable)
{
    const phymod_access_t *pa = phy;
    BCMI_MADURA_DP_CTL0r_t fec_ctl;
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    uint16_t lane_mask = 0, retry_cnt = 5, data1 = 0;
    phymod_phy_inf_config_t config;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));

    PHYMOD_MEMSET(&fec_ctl, 0, sizeof(BCMI_MADURA_DP_CTL0r_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    lane_mask = pa->lane_mask;

    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_DP_CTL0r(pa,&fec_ctl));
    if(lane_mask == 0xF){
        BCMI_MADURA_DP_CTL0r_CL91_EN_0f_SET(fec_ctl,enable);
        if(config.data_rate == MADURA_SPD_100G){
            BCMI_MADURA_DP_CTL0r_AN_AUTO_CL91_0_DISf_SET(fec_ctl,((enable & 0x1)?0:1));/*enable value can be 65536 and 65537 for 0&1 resp . PHY-3042*/  
		}
    }else{
        BCMI_MADURA_DP_CTL0r_CL91_EN_1f_SET(fec_ctl,enable);
        if(config.data_rate == MADURA_SPD_100G){
            BCMI_MADURA_DP_CTL0r_AN_AUTO_CL91_1_DISf_SET(fec_ctl,((enable & 0x1)?0:1));
        }
    }

    /* If !100G speed and fec disable then do*/
    if((config.data_rate!=MADURA_SPD_100G) && ((enable & 0x1)==0)){
        BCMI_MADURA_DP_CTL0r_CL91_EN_0f_SET(fec_ctl,0);
        BCMI_MADURA_DP_CTL0r_CL91_EN_1f_SET(fec_ctl,0);
        BCMI_MADURA_DP_CTL0r_AN_AUTO_CL91_0_DISf_SET(fec_ctl,0);
        BCMI_MADURA_DP_CTL0r_AN_AUTO_CL91_1_DISf_SET(fec_ctl,0);
    }

    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_DP_CTL0r(pa,fec_ctl));

    /* Set FW en */
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));

    /* Wait for Firmware Ready */
    do {
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (-retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    BCMI_MADURA_DP_CTL0r_DP_RSTBf_SET(fec_ctl,0x0);
    PHYMOD_USLEEP(100);
    BCMI_MADURA_DP_CTL0r_DP_RSTBf_SET(fec_ctl,0x3);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_DP_CTL0r(pa,fec_ctl));

    PHYMOD_FREE(config.device_aux_modes);
    return PHYMOD_E_NONE;
}


int _madura_fec_enable_get(const phymod_access_t *phy, uint32_t* enable)
{
    const phymod_access_t *pa = phy;
    BCMI_MADURA_DP_CTL0r_t fec_ctl;
    uint16_t lane_mask = 0;

    PHYMOD_MEMSET(&fec_ctl, 0, sizeof(BCMI_MADURA_DP_CTL0r_t));

    lane_mask = pa->lane_mask;

    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_DP_CTL0r(pa,&fec_ctl));
    if(lane_mask == 0xF){
        *enable=BCMI_MADURA_DP_CTL0r_CL91_EN_0f_GET(fec_ctl);
    }else{
        *enable=BCMI_MADURA_DP_CTL0r_CL91_EN_1f_GET(fec_ctl);
    }

    return PHYMOD_E_NONE;
}


int _madura_tx_power_set(const phymod_access_t *phy, uint32_t power_tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_t tx_pwr_dwn;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&tx_pwr_dwn, 0, sizeof(BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));
            /* FALCON TX Power Set */
            PHYMOD_DEBUG_VERBOSE(("Falcon TX Power set\n"));
            switch (power_tx) {
                case phymodPowerOff: /* Turn off power */
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn)));
                    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,1);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn)));
                    break;
                case phymodPowerOn: /* Turn on power */
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn)));
                    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,0);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn)));
                    break;
                case phymodPowerOffOn: /* Toggle power */
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,&tx_pwr_dwn)));
                    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,1);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn)));
                    PHYMOD_USLEEP(10000);
                    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_TXr_LN_TX_S_PWRDNf_SET(tx_pwr_dwn,0);
                    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                            BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_TXr(pa,tx_pwr_dwn)));
                    break;
                case phymodPowerNoChange: /* Stay where you are */
                    break;
                default:
                    break;
            }
        }
    }

    PHYMOD_FREE(config.device_aux_modes);

    /*Reset DEV1 slice register*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}


int _madura_rx_power_set(const phymod_access_t *phy, uint32_t power_rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0,max_lane=0,intf_side=0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_t rx_pwr_dwn;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&rx_pwr_dwn, 0, sizeof(BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    MADURA_GET_IP(phy, config, ip);
    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

        for(lane = 0; lane < max_lane; lane ++) {
            if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
                /* FALCON RX Power Set */
                PHYMOD_DEBUG_VERBOSE(("Falcon RX Power set\n"));
                switch (power_rx) {
                    case phymodPowerOff: /* Turn off power */
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn)));
                BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,1);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn)));
                    break;
                    case phymodPowerOn: /* Turn on power */
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn)));
                BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,0);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn)));
                    break;
                    case phymodPowerOffOn: /* Toggle power */
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn)));
                BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,1);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn)));
                PHYMOD_USLEEP(10000);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_READ_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,&rx_pwr_dwn)));
                BCMI_MADURA_LN_CLK_RST_N_PWRDWN_CTL_RXr_LN_RX_S_PWRDNf_SET(rx_pwr_dwn,0);
                MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        BCMI_MADURA_WRITE_LN_CLK_RST_N_PWRDWN_CTL_RXr(pa,rx_pwr_dwn)));
                    break;
                    case phymodPowerNoChange: /* Stay where you are */
                    break;
                    default:
                    break;
                }
            }
        }

    PHYMOD_FREE(config.device_aux_modes);

    /*Reset DEV1 slice register*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

   return PHYMOD_E_NONE;
}

int _madura_tx_rx_power_get(const phymod_access_t *phy, phymod_phy_power_t* power)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,intf_side=0;
    uint16_t lane_mask = 0, max_lane = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t rx_pwr_get;
    BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t tx_pwr_get;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&rx_pwr_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_t));
    PHYMOD_MEMSET(&tx_pwr_get, 0, sizeof(BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
              _madura_set_slice_reg (pa,
                        ip, MADURA_DEV_PMA_PMD, lane, 0)));
                /* FALCON TX/RX Power Get */
                PHYMOD_DEBUG_VERBOSE(("Falcon TX/RX Power get\n"));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                  BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr(pa,&rx_pwr_get)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                BCMI_MADURA_READ_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr(pa,&tx_pwr_get)));
                power->rx = !BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_RXr_AFE_RX_PWRDNf_GET(rx_pwr_get);
                power->tx = !BCMI_MADURA_AFE_RST_PWRDN_OSR_MODE_PIN_STS_TXr_AFE_TX_PWRDNf_GET(tx_pwr_get);
    }
    }
    PHYMOD_DEBUG_VERBOSE(("pwr tx=%d rx=%d\n",power->tx,power->rx));
    PHYMOD_FREE(config.device_aux_modes);

    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}


int _madura_tx_set(const phymod_access_t *phy, const phymod_tx_t* tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));
            PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR set\n"));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_PRE, (int8_t)tx->pre)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_MAIN, (int8_t)tx->main)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_POST1, (int8_t)tx->post)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_POST2, (int8_t)tx->post2)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_POST3, (int8_t)tx->post3)));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_write_tx_afe(pa, TX_AFE_AMP, (int8_t)tx->amp)));

     }
    }
    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}


int _madura_tx_get(const phymod_access_t *phy, phymod_tx_t* tx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    int8_t value = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));
            PHYMOD_DEBUG_VERBOSE(("Falcon TXFIR get\n"));
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_PRE, &value)));
            tx->pre = value;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_MAIN, &value)));
            tx->main = value;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_POST1, &value)));
            tx->post = value;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_POST2, &value)));
            tx->post2 = value;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_POST3, &value)));
            tx->post3 = value;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
            falcon2_madura_read_tx_afe(pa, TX_AFE_AMP, &value)));
            tx->amp = value;
     }
    }
    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_rx_set(const phymod_access_t *phy, const phymod_rx_t* rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_SLICEr_t slice_reg;
    uint16_t indx = 0;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));

            /* FALCON RXFIR set */
                PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR set\n"));

                /*params check*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
                    if(rx->dfe[indx].enable && (rx->num_of_dfe_taps > PHYMOD_NUM_DFE_TAPS)) {
                        PHYMOD_DEBUG_ERROR(("illegal number of DFEs to set"));
                    }
                }
        /*vga set*/
                /* first stop the rx adaption */
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_stop_rx_adaptation(pa, 1)));
                if (rx->vga.enable) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_write_rx_afe(pa, RX_AFE_VGA, rx->vga.value)));
                }
                /*dfe set*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
                    if(rx->dfe[indx].enable){
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_write_rx_afe(pa, RX_AFE_DFE1+indx, rx->dfe[indx].value)));
                    }
                }

                /*peaking filter set*/
                if(rx->peaking_filter.enable){
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_write_rx_afe(pa, RX_AFE_PF, rx->peaking_filter.value)));
                }

                if(rx->low_freq_peaking_filter.enable){
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_write_rx_afe(pa, RX_AFE_PF2, rx->low_freq_peaking_filter.value)));
                }

        }
    }

    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

int _madura_rx_adaptation_resume(const phymod_access_t* pa)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0 , intf_side = 0;
    uint16_t lane = 0;
    uint16_t lane_mask = 0, max_lane = 0;
    int rv = PHYMOD_E_NONE;
    BCMI_MADURA_SLICEr_t slice_reg;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(pa, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for (lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));

            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                (falcon2_madura_stop_rx_adaptation(pa, 0))));
        }
    }

    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));

    return rv;
}

int _madura_rx_get(const phymod_access_t *phy, phymod_rx_t* rx)
{
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t lane = 0,max_lane=0,intf_side=0;
    uint16_t lane_mask = 0;
    const phymod_access_t *pa = phy;
    BCMI_MADURA_SLICEr_t slice_reg;
    uint16_t indx = 0;
    int8_t dfe = 0;
    int8_t vga = 0;
    int8_t pf = 0;
    int8_t low_freq_pf = 0;
    uint32_t rx_adaptation;

    PHYMOD_MEMSET(&slice_reg, 0 , sizeof(BCMI_MADURA_SLICEr_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
       _madura_phy_interface_config_get(phy, 0, &config)));
    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    MADURA_GET_IP(phy, config, ip);

    MADURA_GET_INTF_SIDE(pa, intf_side);
    max_lane = (intf_side == MADURA_IF_SYS) ? MADURA_MAX_FALCON_LANE : MADURA_MAX_2X_FALCON_LANE ;
    PHYMOD_DEBUG_VERBOSE(("%s ::IP:%s Max_lane:%d lanemask:0x%x\n", __func__,
             (ip == MADURA_2X_FALCON_CORE)?"FALCON2X":"FALCON", max_lane, lane_mask));

    for(lane = 0; lane < max_lane; lane ++) {
        if (lane_mask & (1 << lane)) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        _madura_set_slice_reg (pa,
                            ip, MADURA_DEV_PMA_PMD, lane, 0)));

            /* FALCON RXFIR get */
                PHYMOD_DEBUG_VERBOSE(("Falcon RXFIR get\n"));

                rx_adaptation = PHYMOD_RX_ADAPTATION_ON_GET(rx);
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_stop_rx_adaptation(phy, 1)));

                /*vga get*/
                rx->num_of_dfe_taps = PHYMOD_NUM_DFE_TAPS;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_read_rx_afe(pa, RX_AFE_VGA, &vga)));
                rx->vga.value = vga;

                /*dfe get*/
                for (indx = 0 ; indx < PHYMOD_NUM_DFE_TAPS; indx++){
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_read_rx_afe(pa, (RX_AFE_DFE1+indx), &dfe)));
                    rx->dfe[indx].value = dfe;
                    rx->dfe[indx].enable = 1;
                }

                /*peaking filter get*/
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_read_rx_afe(pa, RX_AFE_PF, &pf)));
                rx->peaking_filter.value = pf;
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                    falcon2_madura_read_rx_afe(pa, RX_AFE_PF2, &low_freq_pf)));
                rx->low_freq_peaking_filter.value = low_freq_pf;

                if (rx_adaptation) {
            MADURA_IF_ERR_RETURN_FREE(config.device_aux_modes, (
                        falcon2_madura_stop_rx_adaptation(phy, 0)));
                }

        }
    }


    /*Reset DEV1 slice register*/
    PHYMOD_FREE(config.device_aux_modes);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_SLICEr(pa,slice_reg));
    return PHYMOD_E_NONE;
}

/** Get madura interrupt type number from PHYMOD intr type
 *
 * @param intr_type Select the interrupt type.
 *
 * Return val: Furia Interrupt type Number
 */
MADURA_INTR_TYPE_E convert_phymod_to_madura( uint16_t intr_type, uint16_t intf_side, uint16_t lane_mask)
{
    uint16_t converted_val=99; /*some junk value*/

    switch(intr_type) {
        case PHYMOD_INTR_PLL_EVENT:
        if( intf_side == MADURA_IF_SYS){
        converted_val = MADURA_INT_SYS_FAL_0_PLL0_LOCK_LOST;
    }else{/*line side*/
        if( lane_mask & 0XF ){/* lower falcon*/
        converted_val = MADURA_INT_LIN_FAL_0_PLL0_LOCK_LOST;
        }else{ /* upper falcon */
        converted_val = MADURA_INT_LIN_FAL_1_PLL0_LOCK_LOST;
        }
    }
     break;
    case PHYMOD_INTR_AUTONEG_EVENT:
        if( intf_side == MADURA_IF_SYS){
        converted_val = MADURA_INT_SYS_CL73_AN_COMPLETE_P1;
    }else{/*line side*/
        if( lane_mask & 0XF ){/* lower falcon*/
        converted_val = MADURA_INT_LIN_CL73_AN_COMPLETE_P1;
        }else{ /* upper falcon */
        converted_val = MADURA_INT_LIN_CL73_AN_COMPLETE_P2;
        }
    }
     break;
    case PHYMOD_INTR_EMON_EVENT:
        converted_val = ( intf_side == MADURA_IF_SYS ) ? MADURA_INT_SYS_PCS_MON_LOCK_LOST : MADURA_INT_LIN_PCS_MON_LOCK_LOST ;
     break;
    case PHYMOD_INTR_LINK_EVENT:
    converted_val = ( intf_side == MADURA_IF_SYS ) ? MADURA_INT_SYS_PMD_LOCK_LOST : MADURA_INT_LIN_PMD_LOCK_LOST ;
    break;
    default:
    return PHYMOD_E_PARAM;
    }
    return converted_val;
}

int  _madura_get_intr_reg ( const phymod_phy_access_t *phy, uint32_t intr_type, uint16_t *bit_pos, uint16_t *int_grp)
{
    uint16_t lane_mask = 0;
    uint16_t intf_side = 0;
    const phymod_access_t *acc = &phy->access;

    MADURA_GET_INTF_SIDE(acc, intf_side);
    lane_mask = PHYMOD_ACC_LANE_MASK(acc);
    intr_type = convert_phymod_to_madura(intr_type,intf_side,lane_mask);
    /* Get the interrupt bit postion in the reg and interrupt group */
    switch (intr_type) {
        case MADURA_INT_M0_MST_SEC         :          *bit_pos =  5;   *int_grp =  0; break;
        case MADURA_INT_M0_MST_DED         :          *bit_pos =  4;   *int_grp =  0; break;
        case MADURA_INT_M0_MST_MSGOUT_INTR :          *bit_pos =  3;   *int_grp =  0; break;
        case MADURA_INT_M0_MST_MISC_INTR:             *bit_pos =  2;   *int_grp =  0; break;
        case MADURA_INT_M0_MST_LOCKUP:                *bit_pos =  1;   *int_grp =  0; break;
        case MADURA_INT_M0_MST_SYSRESETREQ:           *bit_pos =  0;   *int_grp =  0; break;
        case MADURA_INT_SYS_FAL_0_PLL0_LOCK_FOUND:    *bit_pos = 15;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_0_PLL0_LOCK_LOST:     *bit_pos = 14;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_0_PLL1_LOCK_FOUND:    *bit_pos = 13;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_0_PLL1_LOCK_LOST:     *bit_pos = 12;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_1_PLL0_LOCK_FOUND:    *bit_pos = 11;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_1_PLL0_LOCK_LOST:     *bit_pos = 10;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_1_PLL1_LOCK_FOUND:    *bit_pos =  9;   *int_grp =  1; break;
        case MADURA_INT_SYS_FAL_1_PLL1_LOCK_LOST:     *bit_pos =  8;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_0_PLL0_LOCK_FOUND:    *bit_pos =  7;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_0_PLL0_LOCK_LOST:     *bit_pos =  6;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_0_PLL1_LOCK_FOUND:    *bit_pos =  5;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_0_PLL1_LOCK_LOST:     *bit_pos =  4;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_1_PLL0_LOCK_FOUND:    *bit_pos =  3;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_1_PLL0_LOCK_LOST:     *bit_pos =  2;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_1_PLL1_LOCK_FOUND:    *bit_pos =  1;   *int_grp =  1; break;
        case MADURA_INT_LIN_FAL_1_PLL1_LOCK_LOST:     *bit_pos =  0;   *int_grp =  1; break;
        case MADURA_INT_SYS_CL73_INCOMPATIBLE_LINK_0: *bit_pos = 15;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_INCOMPATIBLE_LINK_1: *bit_pos = 14;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_0:      *bit_pos = 13;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_1:      *bit_pos = 12;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_AN_COMPLETE_P1:      *bit_pos = 11;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_AN_RESTARTED_P1:     *bit_pos = 10;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_AN_COMPLETE_P2:      *bit_pos =  9;   *int_grp =  2; break;
        case MADURA_INT_SYS_CL73_AN_RESTARTED_P2:     *bit_pos =  8;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_INCOMPATIBLE_LINK_0: *bit_pos =  7;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_INCOMPATIBLE_LINK_1: *bit_pos =  6;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_0:      *bit_pos =  5;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_LINK_FAIL_INHIBIT_TIMER_DONE_1:      *bit_pos =  4;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_AN_COMPLETE_P1:      *bit_pos =  3;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_AN_RESTARTED_P1:     *bit_pos =  2;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_AN_COMPLETE_P2:      *bit_pos =  1;   *int_grp =  2; break;
        case MADURA_INT_LIN_CL73_AN_RESTARTED_P2:     *bit_pos =  0;   *int_grp =  2; break;
        case MADURA_INT_SYS_SIGDET_FOUND:             *bit_pos = 11;   *int_grp =  3; break;
        case MADURA_INT_SYS_SIGDET_LOST:              *bit_pos = 10;   *int_grp =  3; break;
        case MADURA_INT_SYS_PMD_LOCK_FOUND:           *bit_pos =  9;   *int_grp =  3; break;
        case MADURA_INT_SYS_PMD_LOCK_LOST:            *bit_pos =  8;   *int_grp =  3; break;
        case MADURA_INT_SYS_FAL_0_PMI_ARB_WDOG_EXP:   *bit_pos =  7;   *int_grp =  3; break;
        case MADURA_INT_SYS_FAL_1_PMI_ARB_WDOG_EXP:   *bit_pos =  6;   *int_grp =  3; break;
        case MADURA_INT_LIN_SIGDET_FOUND:             *bit_pos =  5;   *int_grp =  3; break;
        case MADURA_INT_LIN_SIGDET_LOST:              *bit_pos =  4;   *int_grp =  3; break;
        case MADURA_INT_LIN_PMD_LOCK_FOUND:           *bit_pos =  3;   *int_grp =  3; break;
        case MADURA_INT_LIN_PMD_LOCK_LOST:            *bit_pos =  2;   *int_grp =  3; break;
        case MADURA_INT_LIN_FAL_0_PMI_ARB_WDOG_EXP:   *bit_pos =  1;   *int_grp =  3; break;
        case MADURA_INT_LIN_FAL_1_PMI_ARB_WDOG_EXP:   *bit_pos =  0;   *int_grp =  3; break;
        case MADURA_INT_ONEG_INBAND_MSG_LOCK_FOUND:   *bit_pos = 15;   *int_grp =  4; break;
        case MADURA_INT_ONEG_INBAND_MSG_LOCK_LOST:    *bit_pos = 14;   *int_grp =  4; break;
        case MADURA_INT_SYS_PCS_MON_LOCK_FOUND:       *bit_pos = 13;   *int_grp =  4; break;
        case MADURA_INT_SYS_PCS_MON_LOCK_LOST:        *bit_pos = 12;   *int_grp =  4; break;
        case MADURA_INT_LIN_PCS_MON_LOCK_FOUND:       *bit_pos = 11;   *int_grp =  4; break;
        case MADURA_INT_LIN_PCS_MON_LOCK_LOST:        *bit_pos = 10;   *int_grp =  4; break;
        case MADURA_INT_CL91_0_RX_FEC_ALIGN_FOUND:    *bit_pos =  9;   *int_grp =  4; break;
        case MADURA_INT_CL91_0_RX_FEC_ALIGN_LOST:     *bit_pos =  8;   *int_grp =  4; break;
        case MADURA_INT_CL91_1_RX_FEC_ALIGN_FOUND:    *bit_pos =  7;   *int_grp =  4; break;
        case MADURA_INT_CL91_1_RX_FEC_ALIGN_LOST:     *bit_pos =  6;   *int_grp =  4; break;
        case MADURA_INT_CL91_0_TX_PCS_ALIGN_FOUND:    *bit_pos =  5;   *int_grp =  4; break;
        case MADURA_INT_CL91_0_TX_PCS_ALIGN_LOST:     *bit_pos =  4;   *int_grp =  4; break;
        case MADURA_INT_CL91_1_TX_PCS_ALIGN_FOUND:    *bit_pos =  3;   *int_grp =  4; break;
        case MADURA_INT_CL91_1_TX_PCS_ALIGN_LOST:     *bit_pos =  2;   *int_grp =  4; break;
        case MADURA_INT_SYS_FIFOERR:                  *bit_pos =  1;   *int_grp =  4; break;
        case MADURA_INT_LIN_FIFOERR:                  *bit_pos =  0;   *int_grp =  4; break;
        case MADURA_INT_MODULE1_INTRB_HIGH:           *bit_pos =  3;   *int_grp =  5; break;
        case MADURA_INT_MODULE1_INTRB_LOW:            *bit_pos =  2;   *int_grp =  5; break;
        case MADURA_INT_MODULE2_INTRB_HIGH:           *bit_pos =  1;   *int_grp =  5; break;
        case MADURA_INT_MODULE2_INTRB_LOW:            *bit_pos =  0;   *int_grp =  5; break;
        default:
                              return PHYMOD_E_PARAM;
    }
    return PHYMOD_E_NONE;
}

int _madura_ext_intr_enable_set(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    uint16_t data = 0;
    BCMI_MADURA_MST_EIERr_t    m0_eier;
    BCMI_MADURA_PLL_EIERr_t    pll_eier;
    BCMI_MADURA_CL73_EIERr_t   cl73_eier;
    BCMI_MADURA_MISC0_EIERr_t  misc0_eier;
    BCMI_MADURA_MISC1_EIERr_t  misc1_eier;
    BCMI_MADURA_MCTL_EIERr_t  mctrl_eier;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eier, 0, sizeof(BCMI_MADURA_MST_EIERr_t));
    PHYMOD_MEMSET(&pll_eier, 0, sizeof(BCMI_MADURA_PLL_EIERr_t));
    PHYMOD_MEMSET(&cl73_eier, 0, sizeof(BCMI_MADURA_CL73_EIERr_t));
    PHYMOD_MEMSET(&misc0_eier, 0, sizeof(BCMI_MADURA_MISC0_EIERr_t));
    PHYMOD_MEMSET(&misc1_eier, 0, sizeof(BCMI_MADURA_MISC1_EIERr_t));
    PHYMOD_MEMSET(&mctrl_eier, 0, sizeof(BCMI_MADURA_MCTL_EIERr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    _madura_get_intr_reg(phy,intr_type, &bit_pos, &intr_grp);
    intr_mask = enable ? (0x1 << bit_pos) : (~(0x1 << bit_pos));

    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case MADURA_MST_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_READ_MST_EIERr(pa,&m0_eier));
         data = (enable ? intr_mask | BCMI_MADURA_MST_EIERr_GET(m0_eier) : intr_mask & BCMI_MADURA_MST_EIERr_GET(m0_eier));
           BCMI_MADURA_MST_EIERr_SET(m0_eier,data);
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_WRITE_MST_EIERr(pa,m0_eier));
        break;
        case MADURA_PLL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_PLL_EIERr(pa,&pll_eier));
         data = (enable ? intr_mask | BCMI_MADURA_PLL_EIERr_GET(pll_eier) : intr_mask & BCMI_MADURA_PLL_EIERr_GET(pll_eier));
           BCMI_MADURA_PLL_EIERr_SET(pll_eier,data);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_PLL_EIERr(pa,pll_eier));
        break;
        case MADURA_CL73_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_CL73_EIERr(pa,&cl73_eier));
         data = (enable ? intr_mask | BCMI_MADURA_CL73_EIERr_GET(cl73_eier) : intr_mask & BCMI_MADURA_CL73_EIERr_GET(cl73_eier));
           BCMI_MADURA_CL73_EIERr_SET(cl73_eier,data);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_CL73_EIERr(pa,cl73_eier));
        break;
        case MADURA_MISC0_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC0_EIERr(pa,&misc0_eier));
         data = (enable ? intr_mask | BCMI_MADURA_MISC0_EIERr_GET(misc0_eier) : intr_mask & BCMI_MADURA_MISC0_EIERr_GET(misc0_eier));
           BCMI_MADURA_MISC0_EIERr_SET(misc0_eier,data);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MISC0_EIERr(pa,misc0_eier));
        break;
        case MADURA_MISC1_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC1_EIERr(pa,&misc1_eier));
         data = (enable ? intr_mask | BCMI_MADURA_MISC1_EIERr_GET(misc1_eier) : intr_mask & BCMI_MADURA_MISC1_EIERr_GET(misc1_eier));
           BCMI_MADURA_MISC1_EIERr_SET(misc1_eier,data);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MISC1_EIERr(pa,misc1_eier));
        break;
        case MADURA_MCTRL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MCTL_EIERr(pa,&mctrl_eier));
         data = (enable ? intr_mask | BCMI_MADURA_MCTL_EIERr_GET(mctrl_eier) : intr_mask & BCMI_MADURA_MCTL_EIERr_GET(mctrl_eier));
           BCMI_MADURA_MCTL_EIERr_SET(mctrl_eier,data);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MCTL_EIERr(pa,mctrl_eier));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}


int _madura_ext_intr_enable_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t *enable)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    BCMI_MADURA_MST_EIERr_t    m0_eier;
    BCMI_MADURA_PLL_EIERr_t    pll_eier;
    BCMI_MADURA_CL73_EIERr_t   cl73_eier;
    BCMI_MADURA_MISC0_EIERr_t  misc0_eier;
    BCMI_MADURA_MISC1_EIERr_t  misc1_eier;
    BCMI_MADURA_MCTL_EIERr_t  mctrl_eier;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eier, 0, sizeof(BCMI_MADURA_MST_EIERr_t));
    PHYMOD_MEMSET(&pll_eier, 0, sizeof(BCMI_MADURA_PLL_EIERr_t));
    PHYMOD_MEMSET(&cl73_eier, 0, sizeof(BCMI_MADURA_CL73_EIERr_t));
    PHYMOD_MEMSET(&misc0_eier, 0, sizeof(BCMI_MADURA_MISC0_EIERr_t));
    PHYMOD_MEMSET(&misc1_eier, 0, sizeof(BCMI_MADURA_MISC1_EIERr_t));
    PHYMOD_MEMSET(&mctrl_eier, 0, sizeof(BCMI_MADURA_MCTL_EIERr_t));


    /* Get the interrupt bit postion in the reg and interrupt group */
    _madura_get_intr_reg(phy,intr_type, &bit_pos, &intr_grp);

    /* Interrupt enable get on specified interrupt group */
    switch(intr_grp) {
        case MADURA_MST_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_READ_MST_EIERr(pa,&m0_eier));
           data = BCMI_MADURA_MST_EIERr_GET(m0_eier);
        break;
        case MADURA_PLL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_PLL_EIERr(pa,&pll_eier));
           data = BCMI_MADURA_PLL_EIERr_GET(pll_eier);
        break;
        case MADURA_CL73_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_CL73_EIERr(pa,&cl73_eier));
           data = BCMI_MADURA_CL73_EIERr_GET(cl73_eier);
        break;
        case MADURA_MISC0_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC0_EIERr(pa,&misc0_eier));
           data = BCMI_MADURA_MISC0_EIERr_GET(misc0_eier);
        break;
        case MADURA_MISC1_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC1_EIERr(pa,&misc1_eier));
           data = BCMI_MADURA_MISC1_EIERr_GET(misc1_eier);
        break;
        case MADURA_MCTRL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MCTL_EIERr(pa,&mctrl_eier));
           data = BCMI_MADURA_MCTL_EIERr_GET(mctrl_eier);
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    *enable = (data & (0x1 << bit_pos)) ? 1 : 0;

    return PHYMOD_E_NONE;
}


int _madura_ext_intr_status_get(const phymod_phy_access_t *phy, uint32_t intr_type, uint32_t *intr_status)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t data = 0;
    BCMI_MADURA_MST_EISRr_t    m0_eisr;
    BCMI_MADURA_PLL_EISRr_t    pll_eisr;
    BCMI_MADURA_CL73_EISRr_t   cl73_eisr;
    BCMI_MADURA_MISC0_EISRr_t  misc0_eisr;
    BCMI_MADURA_MISC1_EISRr_t  misc1_eisr;
    BCMI_MADURA_MCTL_EISRr_t  mctrl_eisr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eisr, 0, sizeof(BCMI_MADURA_MST_EISRr_t));
    PHYMOD_MEMSET(&pll_eisr, 0, sizeof(BCMI_MADURA_PLL_EISRr_t));
    PHYMOD_MEMSET(&cl73_eisr, 0, sizeof(BCMI_MADURA_CL73_EISRr_t));
    PHYMOD_MEMSET(&misc0_eisr, 0, sizeof(BCMI_MADURA_MISC0_EISRr_t));
    PHYMOD_MEMSET(&misc1_eisr, 0, sizeof(BCMI_MADURA_MISC1_EISRr_t));
    PHYMOD_MEMSET(&mctrl_eisr, 0, sizeof(BCMI_MADURA_MCTL_EISRr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    _madura_get_intr_reg(phy,intr_type, &bit_pos, &intr_grp);

    /* Interrupt status get on specified interrupt group */
     switch(intr_grp) {
        case MADURA_MST_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_READ_MST_EISRr(pa,&m0_eisr));
           data = BCMI_MADURA_MST_EISRr_GET(m0_eisr);
        break;
        case MADURA_PLL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_PLL_EISRr(pa,&pll_eisr));
           data = BCMI_MADURA_PLL_EISRr_GET(pll_eisr);
        break;
        case MADURA_CL73_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_CL73_EISRr(pa,&cl73_eisr));
           data = BCMI_MADURA_CL73_EISRr_GET(cl73_eisr);
        break;
        case MADURA_MISC0_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC0_EISRr(pa,&misc0_eisr));
           data = BCMI_MADURA_MISC0_EISRr_GET(misc0_eisr);
        break;
        case MADURA_MISC1_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC1_EISRr(pa,&misc1_eisr));
           data = BCMI_MADURA_MISC1_EISRr_GET(misc1_eisr);
        break;
        case MADURA_MCTRL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MCTL_EISRr(pa,&mctrl_eisr));
           data = BCMI_MADURA_MCTL_EISRr_GET(mctrl_eisr);
        break;
        default:
            return PHYMOD_E_PARAM;
    }
    *intr_status = (data & (0x1 << bit_pos)) ? 1 : 0;

    return PHYMOD_E_NONE;
}

int _madura_ext_intr_status_clear(const phymod_phy_access_t *phy, uint32_t intr_type)
{
    uint16_t intr_grp = 0;
    uint16_t bit_pos = 0;
    uint16_t intr_mask = 0;
    BCMI_MADURA_MST_EISRr_t    m0_eisr;
    BCMI_MADURA_PLL_EISRr_t    pll_eisr;
    BCMI_MADURA_CL73_EISRr_t   cl73_eisr;
    BCMI_MADURA_MISC0_EISRr_t  misc0_eisr;
    BCMI_MADURA_MISC1_EISRr_t  misc1_eisr;
    BCMI_MADURA_MCTL_EISRr_t  mctrl_eisr;
    const phymod_access_t *pa = &phy->access;

    PHYMOD_MEMSET(&m0_eisr, 0, sizeof(BCMI_MADURA_MST_EISRr_t));
    PHYMOD_MEMSET(&pll_eisr, 0, sizeof(BCMI_MADURA_PLL_EISRr_t));
    PHYMOD_MEMSET(&cl73_eisr, 0, sizeof(BCMI_MADURA_CL73_EISRr_t));
    PHYMOD_MEMSET(&misc0_eisr, 0, sizeof(BCMI_MADURA_MISC0_EISRr_t));
    PHYMOD_MEMSET(&misc1_eisr, 0, sizeof(BCMI_MADURA_MISC1_EISRr_t));
    PHYMOD_MEMSET(&mctrl_eisr, 0, sizeof(BCMI_MADURA_MCTL_EISRr_t));

    /* Get the interrupt bit postion in the reg and interrupt group */
    _madura_get_intr_reg(phy,intr_type, &bit_pos, &intr_grp);
    intr_mask = 0x1 << bit_pos;

    /* Interrupt status get on specified interrupt group */
     switch(intr_grp) {
        case MADURA_MST_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_READ_MST_EISRr(pa,&m0_eisr));
           BCMI_MADURA_MST_EISRr_SET(m0_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
           BCMI_MADURA_WRITE_MST_EISRr(pa,m0_eisr));
        break;
        case MADURA_PLL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_PLL_EISRr(pa,&pll_eisr));
           BCMI_MADURA_PLL_EISRr_SET(pll_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_PLL_EISRr(pa,pll_eisr));
        break;
        case MADURA_CL73_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_CL73_EISRr(pa,&cl73_eisr));
           BCMI_MADURA_CL73_EISRr_SET(cl73_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_CL73_EISRr(pa,cl73_eisr));
        break;
        case MADURA_MISC0_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC0_EISRr(pa,&misc0_eisr));
           BCMI_MADURA_MISC0_EISRr_SET(misc0_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MISC0_EISRr(pa,misc0_eisr));
        break;
        case MADURA_MISC1_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MISC1_EISRr(pa,&misc1_eisr));
           BCMI_MADURA_MISC1_EISRr_SET(misc1_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MISC1_EISRr(pa,misc1_eisr));
        break;
        case MADURA_MCTRL_INTR_GRP:
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_READ_MCTL_EISRr(pa,&mctrl_eisr));
           BCMI_MADURA_MCTL_EISRr_SET(mctrl_eisr, intr_mask );
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MCTL_EISRr(pa,mctrl_eisr));
        break;
        default:
            return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _madura_module_write(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr,
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
    uint16_t START_OF_NVRAM = 0x0;
    BCMI_MADURA_GEN_CTL3r_t gen_ctrl3;
    BCMI_MADURA_GEN_CTL1r_t gen_ctrl1;
    BCMI_MADURA_DEV_IDr_t dev_id;
    BCMI_MADURA_XFER_ADDRr_t mod_xfer_add;
    BCMI_MADURA_GPIO_1_CTLr_t gpio1_ctrl;
    BCMI_MADURA_GPIO_3_CTLr_t gpio3_ctrl;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_t io_ctrl0;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_t io_ctrl1;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_t io_ctrl2;

    PHYMOD_MEMSET(&gpio1_ctrl, 0, sizeof(BCMI_MADURA_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&gpio3_ctrl, 0, sizeof(BCMI_MADURA_GPIO_3_CTLr_t));
    PHYMOD_MEMSET(&io_ctrl0, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_t));
    PHYMOD_MEMSET(&io_ctrl1, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_t));
    PHYMOD_MEMSET(&io_ctrl2, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_MADURA_GEN_CTL3r_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0, sizeof(BCMI_MADURA_GEN_CTL1r_t));
    PHYMOD_MEMSET(&dev_id, 0, sizeof(BCMI_MADURA_DEV_IDr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_MADURA_XFER_ADDRr_t));

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }

    /*setting gpio workaround for Madura I2C*/
    BCMI_MADURA_GPIO_3_CTLr_SET(gpio3_ctrl,0x00e2);
    BCMI_MADURA_GPIO_1_CTLr_SET(gpio1_ctrl,0x00e2);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_SET(io_ctrl0,0x10);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_SET(io_ctrl1,0x1);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_SET(io_ctrl2,0x31);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GPIO_1_CTLr(pa,gpio1_ctrl));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GPIO_3_CTLr(pa,gpio3_ctrl));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL0r(pa,io_ctrl0));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL1r(pa,io_ctrl1));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL2r(pa,io_ctrl2));

    /* qsfp mode or legacy mode */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_MODEf_SET(gen_ctrl3,1);
    BCMI_MADURA_GEN_CTL3r_UCSPI_SLOWf_SET(gen_ctrl3,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));

    /* qsfp mode reset at begining */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_GEN_CTL1r(pa, &gen_ctrl1));
    BCMI_MADURA_GEN_CTL1r_MODCTRL_RSTBf_SET(gen_ctrl1,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    BCMI_MADURA_GEN_CTL1r_MODCTRL_RSTBf_SET(gen_ctrl1,1);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL1r(pa, gen_ctrl1));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);

    if (lane_mask <= 0xF) { /* QSFP module 0 */
        /* Active Low Select to read NVRAM for QSFP module 0 */
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL0Bf_SET(gen_ctrl3,0);
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL1Bf_SET(gen_ctrl3,1);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    /* Module 0 NVRAM ADDR is MADURA_MODULE_CNTRL_RAM_NVR0_ADR */
    START_OF_NVRAM = 0x0;
    } else if (lane_mask <= 0xF0) {  /* QSFP module 1 */
        /* Active Low Select to read NVRAM for QSFP module 1 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL0Bf_SET(gen_ctrl3,1);
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL1Bf_SET(gen_ctrl3,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    /* Module 1 NVRAM ADDR is MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM*/
    START_OF_NVRAM = 0x100;
     }

    /* Configure the slave device ID default is 0x50 */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_DEV_IDr(pa,&dev_id));
    BCMI_MADURA_DEV_IDr_SL_DEV_ADDf_SET(dev_id,slv_dev_addr);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_DEV_IDr(pa,dev_id));

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
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
        PHYMOD_BUS_WRITE(pa,0x10000 + MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM + start_addr + index,  write_data[index]));
    }

    if(lower_page_flag) {
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
        for (index = 0; index < (lower_page_bytes / 4); index ++) {
        PHYMOD_IF_ERR_RETURN(
                _madura_set_module_command(pa, MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index), 3, MADURA_WRITE));
        }
        if ((lower_page_bytes % 4) > 0) {
        PHYMOD_IF_ERR_RETURN(
                _madura_set_module_command(pa, MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    lower_page_start_addr + (4 * index), lower_page_start_addr + (4 * index),
                                                       ((lower_page_bytes % 4) - 1), MADURA_WRITE));
        }
        lower_page_flag = 0;
    }

    if(upper_page_flag) {
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
        for (index = 0; index < (upper_page_bytes / 4); index++) {
        PHYMOD_IF_ERR_RETURN(
                _madura_set_module_command(pa, (MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index), 3, MADURA_WRITE));
        }
        if ((upper_page_bytes%4) > 0) {
        PHYMOD_IF_ERR_RETURN(
                _madura_set_module_command(pa, (MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                    upper_page_start_addr + (4 * index)), upper_page_start_addr + (4 * index),
                                                        ((upper_page_bytes % 4) - 1), MADURA_WRITE));
        }
        upper_page_flag = 0;
    }
    return PHYMOD_E_NONE;
}

int _madura_module_read(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr,
                      uint32_t no_of_bytes, uint8_t *read_data)
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
    BCMI_MADURA_GEN_CTL3r_t gen_ctrl3;
    BCMI_MADURA_GEN_CTL1r_t gen_ctrl1;
    BCMI_MADURA_DEV_IDr_t dev_id;
    BCMI_MADURA_XFER_ADDRr_t mod_xfer_add;
    BCMI_MADURA_GPIO_1_CTLr_t gpio1_ctrl;
    BCMI_MADURA_GPIO_3_CTLr_t gpio3_ctrl;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_t io_ctrl0;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_t io_ctrl1;
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_t io_ctrl2;

    PHYMOD_MEMSET(&gpio1_ctrl, 0, sizeof(BCMI_MADURA_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&gpio3_ctrl, 0, sizeof(BCMI_MADURA_GPIO_3_CTLr_t));
    PHYMOD_MEMSET(&io_ctrl0, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_t));
    PHYMOD_MEMSET(&io_ctrl1, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_t));
    PHYMOD_MEMSET(&io_ctrl2, 0, sizeof(BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_t));
    PHYMOD_MEMSET(&gen_ctrl3, 0, sizeof(BCMI_MADURA_GEN_CTL3r_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0, sizeof(BCMI_MADURA_GEN_CTL1r_t));
    PHYMOD_MEMSET(&dev_id, 0, sizeof(BCMI_MADURA_DEV_IDr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_MADURA_XFER_ADDRr_t));

    if(start_addr > 255) {
        return PHYMOD_E_PARAM;
    }

    /*setting gpio workaround for Madura I2C*/
    BCMI_MADURA_GPIO_3_CTLr_SET(gpio3_ctrl,0x00e2);
    BCMI_MADURA_GPIO_1_CTLr_SET(gpio1_ctrl,0x00e2);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL0r_SET(io_ctrl0,0x10);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL1r_SET(io_ctrl1,0x1);
    BCMI_MADURA_GENERAL_PURPOSE_IO_CTL2r_SET(io_ctrl2,0x31);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GPIO_1_CTLr(pa,gpio1_ctrl));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GPIO_3_CTLr(pa,gpio3_ctrl));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL0r(pa,io_ctrl0));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL1r(pa,io_ctrl1));
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GENERAL_PURPOSE_IO_CTL2r(pa,io_ctrl2));

    /* qsfp mode or legacy mode */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_MODEf_SET(gen_ctrl3,1);
    BCMI_MADURA_GEN_CTL3r_UCSPI_SLOWf_SET(gen_ctrl3,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));

    /* qsfp mode reset at begining */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_GEN_CTL1r(pa, &gen_ctrl1));
    BCMI_MADURA_GEN_CTL1r_MODCTRL_RSTBf_SET(gen_ctrl1,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL1r(pa, gen_ctrl1));
    BCMI_MADURA_GEN_CTL1r_MODCTRL_RSTBf_SET(gen_ctrl1,1);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL1r(pa, gen_ctrl1));

    lane_mask = PHYMOD_ACC_LANE_MASK(pa);
    if (lane_mask <= 0xF) { /* QSFP module 0 */
        /* Active Low Select to read NVRAM for QSFP module 0 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL0Bf_SET(gen_ctrl3,0);
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL1Bf_SET(gen_ctrl3,1);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    /* Module 0 NVRAM ADDR is MADURA_MODULE_CNTRL_RAM_NVR0_ADR */
    START_OF_NVRAM = 0x0;
    } else if (lane_mask <= 0xF0) {  /* QSFP module 1 */
        /* Active Low Select to read NVRAM for QSFP module 1 */
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_GEN_CTL3r(pa, &gen_ctrl3));
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL0Bf_SET(gen_ctrl3,1);
    BCMI_MADURA_GEN_CTL3r_QSFP_SEL1Bf_SET(gen_ctrl3,0);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_GEN_CTL3r(pa, gen_ctrl3));
    /* Module 1 NVRAM ADDR is MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM*/
    START_OF_NVRAM = 0x100;
     }

    /* Configure the slave device ID default is 0x50 */
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_READ_DEV_IDr(pa,&dev_id));
    BCMI_MADURA_DEV_IDr_SL_DEV_ADDf_SET(dev_id,slv_dev_addr);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_DEV_IDr(pa,dev_id));

    if(no_of_bytes == 0) {
        /* Perform module controller reset and FLUSH */
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
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
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                lower_page_start_addr, lower_page_start_addr, lower_page_bytes - 1, MADURA_RANDOM_ADDRESS_READ));
        lower_page_flag = 0;
    }

    /* Need to check with chip team how we can read upper page */
    if (upper_page_flag) {
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, 0, 0, 0, MADURA_FLUSH));
        PHYMOD_IF_ERR_RETURN(
            _madura_set_module_command(pa, MADURA_MODULE_CNTRL_RAM_NVR0_ADR + START_OF_NVRAM +
                upper_page_start_addr, upper_page_start_addr, upper_page_bytes - 1, MADURA_RANDOM_ADDRESS_READ));
        upper_page_flag = 0;
    }
   /* Read data from NVRAM using I2C*/
    for (index = 0; index < no_of_bytes; index++) {
        PHYMOD_IF_ERR_RETURN(
       PHYMOD_BUS_READ(pa,0x10000 + MADURA_MODULE_CNTRL_RAM_NVR0_ADR + start_addr + index ,  &rd_data));
       read_data[index] = (unsigned char) (rd_data & 0xff);
    }

    return PHYMOD_E_NONE;
}

int _madura_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr,
                               uint32_t slv_addr, unsigned char xfer_cnt, MADURA_I2CM_CMD_E cmd)
{
    BCMI_MADURA_MODULE_CTL_CTLr_t module_ctrl;
    BCMI_MADURA_MODULE_CTL_STSr_t mctrl_status;
    BCMI_MADURA_XFER_CNTr_t mod_xfer_cnt;
    BCMI_MADURA_ADDRr_t mod_add;
    BCMI_MADURA_XFER_ADDRr_t mod_xfer_add;
    uint16_t retry_count = 500, data = 0;
    uint32_t wait_timeout_us = 0;
    wait_timeout_us = ((2*(xfer_cnt+1))*100)/5;

    PHYMOD_MEMSET(&module_ctrl, 0, sizeof(BCMI_MADURA_MODULE_CTL_CTLr_t));
    PHYMOD_MEMSET(&mod_xfer_add, 0, sizeof(BCMI_MADURA_XFER_ADDRr_t));
    PHYMOD_MEMSET(&mod_xfer_cnt, 0, sizeof(BCMI_MADURA_XFER_CNTr_t));
    PHYMOD_MEMSET(&mctrl_status, 0, sizeof(BCMI_MADURA_MODULE_CTL_STSr_t));
    PHYMOD_MEMSET(&mod_add, 0, sizeof(BCMI_MADURA_ADDRr_t));
    if (cmd == MADURA_FLUSH) {
       BCMI_MADURA_MODULE_CTL_CTLr_SET(module_ctrl,0xC000);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));
    } else {
        BCMI_MADURA_XFER_ADDRr_SET(mod_xfer_add,xfer_addr);
        BCMI_MADURA_XFER_CNTr_XFER_CNTf_SET(mod_xfer_cnt,xfer_cnt);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_XFER_ADDRr(pa,mod_xfer_add));
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_XFER_CNTr(pa,mod_xfer_cnt));
        if (cmd == MADURA_CURRENT_ADDRESS_READ) {
           BCMI_MADURA_MODULE_CTL_CTLr_SET(module_ctrl,0x8001);
       PHYMOD_IF_ERR_RETURN(
       BCMI_MADURA_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));
        } else if (cmd == MADURA_RANDOM_ADDRESS_READ ) {
            BCMI_MADURA_ADDRr_SET(mod_add, slv_addr);
        BCMI_MADURA_MODULE_CTL_CTLr_SET(module_ctrl,0x8003);
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_ADDRr(pa,mod_add));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));
        } else {
            BCMI_MADURA_ADDRr_SET(mod_add, slv_addr);
        BCMI_MADURA_MODULE_CTL_CTLr_SET(module_ctrl,0x8022);
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_ADDRr(pa,mod_add));
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));
        }
    }

    if ((cmd == MADURA_CURRENT_ADDRESS_READ) ||
        (cmd == MADURA_RANDOM_ADDRESS_READ) ||
        (cmd == MADURA_WRITE)) {
        do {
        PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_READ_MODULE_CTL_STSr(pa,&mctrl_status));
            data = BCMI_MADURA_MODULE_CTL_STSr_XACTION_DONEf_GET(mctrl_status);
            PHYMOD_USLEEP(wait_timeout_us);
        } while((data == 0) && --retry_count);
        if(!retry_count) {
            PHYMOD_RETURN_WITH_ERR(PHYMOD_E_TIMEOUT, (_PHYMOD_MSG("Module controller: I2C transaction failed..")));
        }
    }

    BCMI_MADURA_MODULE_CTL_CTLr_SET(module_ctrl,0x3);
    PHYMOD_IF_ERR_RETURN(
    BCMI_MADURA_WRITE_MODULE_CTL_CTLr(pa,module_ctrl));

    return PHYMOD_E_NONE;
}

int _madura_gpio_config_set(const phymod_access_t *pa, int pin_number, phymod_gpio_mode_t gpio_mode)
{
    uint16_t data = 0;
    BCMI_MADURA_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_MADURA_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_MADURA_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_MADURA_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_3_CTLr_t));
    if (pin_number > 3) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    switch(pin_number)
    {
      case 0:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_0_CTLr(pa,&pad_ctrl_gpio_0_ctrl));
      break;
      case 1:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_1_CTLr(pa,&pad_ctrl_gpio_1_ctrl));
      break;
      case 2:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_2_CTLr(pa,&pad_ctrl_gpio_2_ctrl));
      break;
      case 3:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_3_CTLr(pa,&pad_ctrl_gpio_3_ctrl));
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

    switch(pin_number)
    {
      case 0:
              BCMI_MADURA_GPIO_0_CTLr_GPIO_0_OEBf_SET(pad_ctrl_gpio_0_ctrl,data);
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_0_CTLr(pa,pad_ctrl_gpio_0_ctrl));
      break;
      case 1:
              BCMI_MADURA_GPIO_1_CTLr_GPIO_1_OEBf_SET(pad_ctrl_gpio_1_ctrl,data);
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_1_CTLr(pa,pad_ctrl_gpio_1_ctrl));
      break;
      case 2:
              BCMI_MADURA_GPIO_2_CTLr_GPIO_2_OEBf_SET(pad_ctrl_gpio_2_ctrl,data);
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_2_CTLr(pa,pad_ctrl_gpio_2_ctrl));
      break;
      case 3:
              BCMI_MADURA_GPIO_3_CTLr_GPIO_3_OEBf_SET(pad_ctrl_gpio_3_ctrl,data);
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_3_CTLr(pa,pad_ctrl_gpio_3_ctrl));
      break;
    }


    return PHYMOD_E_NONE;
}

int _madura_gpio_config_get(const phymod_access_t *pa, int pin_number, phymod_gpio_mode_t* gpio_mode)
{
    uint16_t data = 0;
    BCMI_MADURA_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_MADURA_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_MADURA_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_MADURA_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_3_CTLr_t));

    if (pin_number > 3) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    switch(pin_number)
    {
      case 0:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_0_CTLr(pa,&pad_ctrl_gpio_0_ctrl));
              data = BCMI_MADURA_GPIO_0_CTLr_GPIO_0_OEBf_GET(pad_ctrl_gpio_0_ctrl);
      break;
      case 1:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_1_CTLr(pa,&pad_ctrl_gpio_1_ctrl));
              data = BCMI_MADURA_GPIO_1_CTLr_GPIO_1_OEBf_GET(pad_ctrl_gpio_1_ctrl);
      break;
      case 2:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_2_CTLr(pa,&pad_ctrl_gpio_2_ctrl));
              data = BCMI_MADURA_GPIO_2_CTLr_GPIO_2_OEBf_GET(pad_ctrl_gpio_2_ctrl);
      break;
      case 3:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_3_CTLr(pa,&pad_ctrl_gpio_3_ctrl));
              data = BCMI_MADURA_GPIO_3_CTLr_GPIO_3_OEBf_GET(pad_ctrl_gpio_3_ctrl);
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

int _madura_gpio_pin_value_set(const phymod_access_t *pa, int pin_number, int value)
{
    BCMI_MADURA_GPIO_0_CTLr_t pad_ctrl_gpio_0_ctrl;
    BCMI_MADURA_GPIO_1_CTLr_t pad_ctrl_gpio_1_ctrl;
    BCMI_MADURA_GPIO_2_CTLr_t pad_ctrl_gpio_2_ctrl;
    BCMI_MADURA_GPIO_3_CTLr_t pad_ctrl_gpio_3_ctrl;

    PHYMOD_MEMSET(&pad_ctrl_gpio_0_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_0_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_1_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_1_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_2_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_2_CTLr_t));
    PHYMOD_MEMSET(&pad_ctrl_gpio_3_ctrl, 0 , sizeof(BCMI_MADURA_GPIO_3_CTLr_t));
    if (pin_number > 3) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    switch(pin_number)
    {
      case 0:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_0_CTLr(pa,&pad_ctrl_gpio_0_ctrl));
              BCMI_MADURA_GPIO_0_CTLr_GPIO_0_IBOFf_SET(pad_ctrl_gpio_0_ctrl,1);
              BCMI_MADURA_GPIO_0_CTLr_GPIO_0_OUT_FRCVALf_SET(pad_ctrl_gpio_0_ctrl,value ? 1 : 0 );
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_0_CTLr(pa,pad_ctrl_gpio_0_ctrl));
      break;
      case 1:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_1_CTLr(pa,&pad_ctrl_gpio_1_ctrl));
              BCMI_MADURA_GPIO_1_CTLr_GPIO_1_IBOFf_SET(pad_ctrl_gpio_1_ctrl,1);
              BCMI_MADURA_GPIO_1_CTLr_GPIO_1_OUT_FRCVALf_SET(pad_ctrl_gpio_1_ctrl,value ? 1 : 0 );
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_1_CTLr(pa,pad_ctrl_gpio_1_ctrl));
      break;
      case 2:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_2_CTLr(pa,&pad_ctrl_gpio_2_ctrl));
              BCMI_MADURA_GPIO_2_CTLr_GPIO_2_IBOFf_SET(pad_ctrl_gpio_2_ctrl,1);
              BCMI_MADURA_GPIO_2_CTLr_GPIO_2_OUT_FRCVALf_SET(pad_ctrl_gpio_2_ctrl,value ? 1 : 0 );
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_2_CTLr(pa,pad_ctrl_gpio_2_ctrl));
      break;
      case 3:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_3_CTLr(pa,&pad_ctrl_gpio_3_ctrl));
              BCMI_MADURA_GPIO_3_CTLr_GPIO_3_IBOFf_SET(pad_ctrl_gpio_3_ctrl,1);
              BCMI_MADURA_GPIO_3_CTLr_GPIO_3_OUT_FRCVALf_SET(pad_ctrl_gpio_3_ctrl,value ? 1 : 0 );
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_WRITE_GPIO_3_CTLr(pa,pad_ctrl_gpio_3_ctrl));
      break;
      default:
          return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _madura_gpio_pin_value_get(const phymod_access_t *pa, int pin_number, int* value)
{

    BCMI_MADURA_GPIO_0_STSr_t pad_sts_gpio_0;
    BCMI_MADURA_GPIO_1_STSr_t pad_sts_gpio_1;
    BCMI_MADURA_GPIO_2_STSr_t pad_sts_gpio_2;
    BCMI_MADURA_GPIO_3_STSr_t pad_sts_gpio_3;

    PHYMOD_MEMSET(&pad_sts_gpio_0, 0 , sizeof(BCMI_MADURA_GPIO_0_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_1, 0 , sizeof(BCMI_MADURA_GPIO_1_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_2, 0 , sizeof(BCMI_MADURA_GPIO_2_STSr_t));
    PHYMOD_MEMSET(&pad_sts_gpio_3, 0 , sizeof(BCMI_MADURA_GPIO_3_STSr_t));
    if (pin_number > 3) {
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
            (_PHYMOD_MSG("Invalid GPIO pin selected, Valid GPIOs are (0 - 3)")));
    }

    switch(pin_number)
    {
      case 0:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_0_STSr(pa,&pad_sts_gpio_0));
          *value = BCMI_MADURA_GPIO_0_STSr_GPIO_0_DINf_GET(pad_sts_gpio_0);
      break;
      case 1:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_1_STSr(pa,&pad_sts_gpio_1));
          *value = BCMI_MADURA_GPIO_1_STSr_GPIO_1_DINf_GET(pad_sts_gpio_1);
      break;
      case 2:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_2_STSr(pa,&pad_sts_gpio_2));
          *value = BCMI_MADURA_GPIO_2_STSr_GPIO_2_DINf_GET(pad_sts_gpio_2);
      break;
      case 3:
          PHYMOD_IF_ERR_RETURN(
          BCMI_MADURA_READ_GPIO_3_STSr(pa,&pad_sts_gpio_3));
          *value = BCMI_MADURA_GPIO_3_STSr_GPIO_3_DINf_GET(pad_sts_gpio_3);
      break;
      default:
          return PHYMOD_E_PARAM;
    }

    return PHYMOD_E_NONE;
}

int _madura_cfg_fw_ull_dp(const phymod_access_t* pa, phymod_datapath_t op_datapath)
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    phymod_phy_inf_config_t config_cpy;
    uint16_t ip = 0, Tx_timing_mode = MADURA_TX_REPEATER_ULL;
    uint16_t data1 = 0, retry_cnt = MADURA_ULL_SET_RETRY_CNT;
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&config_cpy, 0, sizeof(phymod_phy_inf_config_t));
   config_cpy.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config_cpy.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config_cpy)));

   if (op_datapath ==  phymodDatapathNormal) {
    Tx_timing_mode = MADURA_TX_REPEATER;

   }
    MADURA_GET_IP(pa, (config_cpy), ip);

    /* Wait for Firmware Ready */
    do {
    MADURA_IF_ERR_RETURN_FREE( config_cpy.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config_cpy.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    /* Set TX timing mode */
    MADURA_IF_ERR_RETURN_FREE( config_cpy.device_aux_modes, (
        _madura_interface_set_ull_mode_set(pa, ip, &config_cpy,
                         Tx_timing_mode)));

    /*not toggling the firmware En here . It will be done once config set is performed. Ull will start working after that*/

	PHYMOD_FREE(config_cpy.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_core_rptr_rtmr_mode_set(const phymod_access_t* pa, uint16_t op_mode)
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    phymod_phy_inf_config_t config_cpy;
    uint16_t ip = 0, rptr_rtmr_mode = 0;
    uint16_t data1 = 0, retry_cnt = 0;

    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));
    PHYMOD_MEMSET(&config_cpy, 0, sizeof(phymod_phy_inf_config_t));
    config_cpy.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");
    MADURA_IF_ERR_RETURN_FREE(config_cpy.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config_cpy)));

    rptr_rtmr_mode = (op_mode == phymodOperationModeRetimer) ? MADURA_TX_RETIMER : MADURA_TX_REPEATER ;
    MADURA_GET_IP(pa, (config_cpy), ip);


    /* Wait for Firmware Ready */
    retry_cnt = MADURA_RPTR_SET_RETRY_CNT;
    do {
    MADURA_IF_ERR_RETURN_FREE( config_cpy.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config_cpy.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG, (_PHYMOD_MSG("Firmware is busy..")));
    }

    /* Set TX timing mode */
    MADURA_IF_ERR_RETURN_FREE( config_cpy.device_aux_modes, (
        _madura_interface_set_repeater_retimer_mode_set(pa, ip, &config_cpy,
                         rptr_rtmr_mode)));

    /* Set FW en */
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en));

   /* Wait for Firmware Ready */
    retry_cnt = MADURA_RPTR_SET_RETRY_CNT;
    do {
    MADURA_IF_ERR_RETURN_FREE( config_cpy.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config_cpy.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

	PHYMOD_FREE(config_cpy.device_aux_modes);
    return PHYMOD_E_NONE;
}

int _madura_lane_config_DfeAndMedia_set(const phymod_access_t *pa, uint16_t ip,  const phymod_phy_inf_config_t* config, phymod_firmware_lane_config_t* fw_config)
{
    uint16_t DFE_option = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;


    if ((ip == MADURA_2X_FALCON_CORE) && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  && config->data_rate != MADURA_SPD_50G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
    }

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));

    if (fw_config != NULL) {
        if (fw_config->DfeOn == 0){
            DFE_option =  SERDES_DFE_OPTION_NO_DFE;
        }
        if (fw_config->DfeOn == 1 &&
            fw_config->LpDfeOn == 0  &&
            fw_config->ForceBrDfe == 0) {
            DFE_option =  SERDES_DFE_OPTION_DFE;
        }
        if (fw_config->DfeOn == 1 &&
            fw_config->LpDfeOn == 0  &&
            fw_config->ForceBrDfe == 1) {
            DFE_option =  SERDES_DFE_OPTION_BRDEF;
        }
        if (fw_config->DfeOn == 1 &&
            fw_config->LpDfeOn == 1  &&
            fw_config->ForceBrDfe == 0) {
            DFE_option =  SERDES_DFE_OPTION_DFE_LP_MODE;
        }

        switch(fw_config->MediaType) {
            case phymodFirmwareMediaTypeOptics:
                if (fw_config->UnreliableLos) {
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
    }


    if (ip == MADURA_2X_FALCON_CORE) {
        config_reg &= ~(0xf00);
        /*considering sys+line side on the same interface*/
        config_reg |= (( media_type)& 0x3) << 8 ; /*line*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 10 ; /*lin*/
    } else {
        config_reg &= ~(0x000f);
        /*considering sys+line side on the same interface*/
        config_reg |= ( media_type)& 0x3 ; /*sys*/
        /*considering sys+line side on the same DFE settings*/
        config_reg |= (( DFE_option ) & 0x3 ) << 2 ; /*sys */
    }

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        BCMI_MADURA_MST_MISC0r_SET(mst_misc0,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC0r(pa,mst_misc0));
    } else  if (lane_mask == 0x2) {
        BCMI_MADURA_MST_MISC1r_SET(mst_misc1,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC1r(pa,mst_misc1));
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        BCMI_MADURA_MST_MISC2r_SET(mst_misc2,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC2r(pa,mst_misc2));
    } else {
        BCMI_MADURA_MST_MISC3r_SET(mst_misc3,(config_reg ));
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_WRITE_MST_MISC3r(pa,mst_misc3));
   }

    return PHYMOD_E_NONE;
}

int _madura_lane_config_DfeAndMedia_get(const phymod_access_t *pa, uint16_t ip, const phymod_phy_inf_config_t *config, phymod_firmware_lane_config_t* fw_config)
{
    uint16_t DFE_option = SERDES_DFE_OPTION_NO_DFE; /*default to dfe disable*/
    uint16_t media_type = SERDES_MEDIA_TYPE_BACK_PLANE; /* Default to Back Plane */
    uint16_t config_reg=0;
    uint16_t lane_mask = pa->lane_mask;
    BCMI_MADURA_MST_MISC0r_t mst_misc0;
    BCMI_MADURA_MST_MISC1r_t mst_misc1;
    BCMI_MADURA_MST_MISC2r_t mst_misc2;
    BCMI_MADURA_MST_MISC3r_t mst_misc3;
    MADURA_DEVICE_AUX_MODE_T  *aux_mode;

    aux_mode = (MADURA_DEVICE_AUX_MODE_T*) config->device_aux_modes;

    if (ip == MADURA_2X_FALCON_CORE && aux_mode->alternate) {
        if ((config->data_rate != MADURA_SPD_40G  && config->data_rate != MADURA_SPD_50G  &&
             config->data_rate != MADURA_SPD_42G) ||
            aux_mode->pass_thru_dual_lane       ||
            aux_mode->pass_thru                    ) {

            lane_mask >>= 4 ;
        }
    }

    PHYMOD_MEMSET(&mst_misc0, 0, sizeof(BCMI_MADURA_MST_MISC0r_t));
    PHYMOD_MEMSET(&mst_misc1, 0, sizeof(BCMI_MADURA_MST_MISC1r_t));
    PHYMOD_MEMSET(&mst_misc2, 0, sizeof(BCMI_MADURA_MST_MISC2r_t));
    PHYMOD_MEMSET(&mst_misc3, 0, sizeof(BCMI_MADURA_MST_MISC3r_t));

    if (lane_mask == 0xF || lane_mask == 0x3 || lane_mask == 0x1 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC0r(pa, &mst_misc0));
        config_reg = BCMI_MADURA_MST_MISC0r_GET(mst_misc0);
    } else  if (lane_mask == 0x2) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC1r(pa, &mst_misc1));
        config_reg = BCMI_MADURA_MST_MISC1r_GET(mst_misc1);
    } else  if (lane_mask == 0xF0 || lane_mask == 0xC || lane_mask == 0x4 ) {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC2r(pa, &mst_misc2));
        config_reg = BCMI_MADURA_MST_MISC2r_GET(mst_misc2);
    } else {
        PHYMOD_IF_ERR_RETURN(
            BCMI_MADURA_READ_MST_MISC3r(pa, &mst_misc3));
        config_reg = BCMI_MADURA_MST_MISC3r_GET(mst_misc3);
   }

    if (ip == MADURA_2X_FALCON_CORE) {
        config_reg &= (0xf00);
        /*considering sys+line side on the same interface*/
        media_type = (config_reg & 0x300)>> 8 ;
        /*considering sys+line side on the same DFE settings*/
        DFE_option = (config_reg & 0xc00)>> 10;
    } else {
        config_reg &= (0x0f);
        media_type = (config_reg & 0x3);
        /*considering sys+line side on the same DFE settings*/
        DFE_option = (config_reg & 0xc)>> 2;
    }

    if (fw_config != NULL) {

        switch(DFE_option) {
            case SERDES_DFE_OPTION_NO_DFE:
                fw_config->DfeOn = 0;
                fw_config->LpDfeOn = 0;
                fw_config->ForceBrDfe = 0;
                break;
            case SERDES_DFE_OPTION_DFE:
                fw_config->DfeOn = 1;
                fw_config->LpDfeOn = 0;
                fw_config->ForceBrDfe = 0;
                break;
            case SERDES_DFE_OPTION_BRDEF:
                fw_config->DfeOn = 1;
                fw_config->LpDfeOn = 0;
                fw_config->ForceBrDfe = 1;
                break;
            case SERDES_DFE_OPTION_DFE_LP_MODE:
                fw_config->DfeOn = 1;
                fw_config->LpDfeOn = 1;
                fw_config->ForceBrDfe = 0;
                break;
        }

        fw_config->UnreliableLos = 0;
        switch(media_type) {
            case SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS:
                fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                break;
            case SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS:
                fw_config->MediaType = phymodFirmwareMediaTypeOptics;
                fw_config->UnreliableLos = 1;
                break;
            case SERDES_MEDIA_TYPE_COPPER_CABLE:
                fw_config->MediaType = phymodFirmwareMediaTypeCopperCable;
                break;
            case SERDES_MEDIA_TYPE_BACK_PLANE:
                fw_config->MediaType = phymodFirmwareMediaTypePcbTraceBackPlane;
                break;
        }
    }

    return PHYMOD_E_NONE;
}

int _madura_firmware_lane_config_set(const phymod_access_t* pa, phymod_firmware_lane_config_t fw_config)
{
    BCMI_MADURA_FIRMWARE_ENr_t fw_en;
    phymod_phy_inf_config_t config;
    uint16_t ip = 0;
    uint16_t data1 = 0, retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;

    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));
    PHYMOD_MEMSET(&fw_en, 0 , sizeof(BCMI_MADURA_FIRMWARE_ENr_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    /* Set DFE & Media */
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_lane_config_DfeAndMedia_set(pa, ip, &config, &fw_config)));

    /* Set FW en */
    /* coverity[operator_confusion] */
    BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_SET(fw_en, 1);
    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_WRITE_FIRMWARE_ENr(pa,fw_en)));

    /* Wait for Firmware Ready */
    retry_cnt = MADURA_FORCE_TRAIN_RETRY_CNT;
    do {
        MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        BCMI_MADURA_READ_FIRMWARE_ENr(pa,&fw_en)));
        data1=BCMI_MADURA_FIRMWARE_ENr_FW_ENABLE_VALf_GET(fw_en);
        PHYMOD_USLEEP(100);
    } while ((data1 != 0) && (--retry_cnt));
    if (retry_cnt == 0) {
        PHYMOD_FREE(config.device_aux_modes);
        PHYMOD_RETURN_WITH_ERR(PHYMOD_E_CONFIG,
               (_PHYMOD_MSG("Firmware is busy..")));
    }

    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}

int _madura_firmware_lane_config_get(const phymod_access_t* pa, phymod_firmware_lane_config_t* fw_config)
{
    uint16_t ip = 0;
    phymod_phy_inf_config_t config;

    PHYMOD_MEMSET(fw_config, 0, sizeof(phymod_firmware_lane_config_t));
    PHYMOD_MEMSET(&config, 0, sizeof(phymod_phy_inf_config_t));

    config.device_aux_modes = PHYMOD_MALLOC(sizeof(MADURA_DEVICE_AUX_MODE_T), "madura_device_aux_mode");

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
       _madura_phy_interface_config_get(pa, 0, &config)));

    MADURA_GET_IP(pa, config, ip);

    MADURA_IF_ERR_RETURN_FREE( config.device_aux_modes, (
        _madura_lane_config_DfeAndMedia_get(pa, ip, &config, fw_config)));

    PHYMOD_FREE(config.device_aux_modes);

    return PHYMOD_E_NONE;
}


