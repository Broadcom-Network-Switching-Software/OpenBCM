/*
*
* 
*
5 *
5 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
5 * 
5 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
6  * All Rights Reserved.$
7  *
8  * File: quadra28.c
   * Purpose: tier1 phymod microcode download support for Broadcom 40G Quadra28 
   * note
*/

/*
 * Includes
 */
#include <phymod/phymod.h>
#include <phymod/phymod_system.h>
#include "quadra28_reg_access.h"
#include "quadra28_types.h"
#include "bcmi_quadra28_defs.h"
#include "quadra28_cfg_seq.h"

static int _quadra28_verify_fw_download(const phymod_access_t *pa, int edc);

/* If it is single PMD, we need to clear it before downloading FW*/
int _quadra28_clear_mode_config(phymod_access_t *pa)
{
    int speed = 0;
    BCMI_QUADRA28_GENERAL_PURPOSE_3r_t gp_reg3_reg_val;
    BCMI_QUADRA28_APPS_MODE_0r_t apps_mode0_reg_val;
    PHYMOD_MEMSET(&gp_reg3_reg_val, 0, sizeof(BCMI_QUADRA28_GENERAL_PURPOSE_3r_t));
    PHYMOD_MEMSET(&apps_mode0_reg_val, 0, sizeof(BCMI_QUADRA28_APPS_MODE_0r_t));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_PURPOSE_3r(pa, &gp_reg3_reg_val));
    speed = gp_reg3_reg_val.v[0] & 0xF ;
    if (speed == 0x4 || speed == 0x7) {
        PHYMOD_IF_ERR_RETURN(
            quadra28_channel_select (pa, Q28_ALL_LANE));
        apps_mode0_reg_val.v[0] = gp_reg3_reg_val.v[0];
        BCMI_QUADRA28_APPS_MODE_0r_FINISH_CHANGEf_SET(apps_mode0_reg_val, 0);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        PHYMOD_USLEEP(100);
        BCMI_QUADRA28_APPS_MODE_0r_SET(apps_mode0_reg_val, 0x8882);
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_WRITE_APPS_MODE_0r(pa, apps_mode0_reg_val));
        PHYMOD_USLEEP(100);
        PHYMOD_IF_ERR_RETURN(
                 _quadra28_intf_update_wait_check(pa,apps_mode0_reg_val.v[0], 50000));
    }

    return PHYMOD_E_NONE;
}

static int _quadra28_verify_fw_download(const phymod_access_t *pa, int edc)
{
    MSG_OUTr_t msg_out_reg;
    BOOT_CHECKSUMr_t checksum_reg;
    PHYMOD_MEMSET(&msg_out_reg, 0, sizeof(MSG_OUTr_t));
    PHYMOD_MEMSET(&checksum_reg, 0, sizeof(BOOT_CHECKSUMr_t));
    PHYMOD_IF_ERR_RETURN((BCMI_QUADRA28_READ_MSG_OUTr(pa,&msg_out_reg)));
    msg_out_reg.v[0] = MSG_OUTr_MESSAGE_OUTf_GET(msg_out_reg);
    if (0x4321 == msg_out_reg.v[0]){
       /* step19: Wait 4ms */
        PHYMOD_USLEEP(4000);
    } else {
       return PHYMOD_E_FAIL;
    }      
    /* Intermediate step in vbs */
    PHYMOD_IF_ERR_RETURN((BCMI_QUADRA28_READ_MSG_OUTr(pa,&msg_out_reg)));
    msg_out_reg.v[0] = MSG_OUTr_MESSAGE_OUTf_GET(msg_out_reg);   
    if (0x300 != msg_out_reg.v[0]) {
       return PHYMOD_E_FAIL;
    }
    if (!edc) {
        /* step 20 Read checksum (expected 0x600D) */
        PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_READ_BOOT_CHECKSUMr(pa, &checksum_reg)); 
        checksum_reg.v[0]=BCMI_QUADRA28_BOOT_CHECKSUMr_BOOT_CHKSUMf_GET(checksum_reg); 
        if (0x600D != checksum_reg.v[0]) {
            /*Checksum is correct */
            return PHYMOD_E_FAIL;
        }
    } else {
        BCMI_QUADRA28_GP_4r_t gp_4;
        PHYMOD_MEMSET(&gp_4, 0, sizeof(BCMI_QUADRA28_GP_4r_t));
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_GP_4r(pa, &gp_4));
        if (0x600D != gp_4.v[0]) {
            /*Checksum is not correct */
            return PHYMOD_E_FAIL;
        }
    }
    
    return PHYMOD_E_NONE; 
}

int quadra28_firmware_info_get(const phymod_access_t *pa, phymod_core_firmware_info_t *fw_info)
{
    uint32_t fw_ver = 0;
    BOOT_CHECKSUMr_t mst_running_chksum;

    /* Read the firmware version */
    PHYMOD_IF_ERR_RETURN(
        phymod_raw_iblk_read(pa, (0x10000 | (uint32_t) 0xC161), &fw_ver));
 
    fw_info->fw_version = (fw_ver & 0xff); 
    PHYMOD_IF_ERR_RETURN( 
        BCMI_QUADRA28_READ_BOOT_CHECKSUMr(pa, 
            &mst_running_chksum));
    fw_info->fw_crc = BCMI_QUADRA28_BOOT_CHECKSUMr_BOOT_CHKSUMf_GET(mst_running_chksum);
   
    return PHYMOD_E_NONE;
}

int quadra28_write_message(const phymod_access_t *pa, int wrdata, int *rddata)
{
    int tmp_data = 0, retry_cnt = 0;
    BCMI_QUADRA28_MESSAGE_OUTr_t msg_out;
    BCMI_QUADRA28_MESSAGE_INr_t msg_in;
    BCMI_QUADRA28_GENERAL_STATUS_1r_t   gen_sts;

    PHYMOD_MEMSET(&msg_out, 0, sizeof(BCMI_QUADRA28_MESSAGE_OUTr_t));
    PHYMOD_MEMSET(&msg_in, 0, sizeof(BCMI_QUADRA28_MESSAGE_INr_t));
    PHYMOD_MEMSET(&gen_sts, 0, sizeof(BCMI_QUADRA28_GENERAL_STATUS_1r_t));

    /* Clear any lingering Message out from Micro.*/
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_MESSAGE_OUTr(pa, &msg_out));

    /* Write SPI Control Register Write Command.*/
    BCMI_QUADRA28_MESSAGE_INr_SET(msg_in, wrdata);
    PHYMOD_IF_ERR_RETURN(
           BCMI_QUADRA28_WRITE_MESSAGE_INr(pa, msg_in));
    do {
        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_GENERAL_STATUS_1r(pa, &gen_sts));
        tmp_data = BCMI_QUADRA28_GENERAL_STATUS_1r_GET (gen_sts);
        retry_cnt ++;
    } while (((tmp_data & 0x4000) == 0) && (retry_cnt < Q28_RETRY_CNT));

    if (retry_cnt >= Q28_RETRY_CNT) {
        return PHYMOD_E_CONFIG;
    }

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_MESSAGE_OUTr(pa, &msg_out));
    *rddata = BCMI_QUADRA28_MESSAGE_OUTr_GET(msg_out);

    return PHYMOD_E_NONE;
}

int q28_get_spi_eeprom_status(const phymod_access_t *pa) 
{
    int retry_cnt = 0, retry_cnt_1 = 0;
    int SPI_READY = 1;
    int wr_data = 0, rd_data = 0;
    while ((SPI_READY == 1) && (retry_cnt_1 < Q28_RETRY_CNT)) {
        /* Set-up SPI Controller To Receive SPI EEPROM Status.
         * Write SPI Control Register Write Command.*/
        wr_data = ((Q28_WR_CPU_CTRL_REGS * 0x0100) | 0x1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        /* Write SPI Control -2 Register Address.*/
        wr_data = Q28_SPI_CTRL_2_H;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Control -2 Register Word-2.*/
        wr_data = 0x0100;	/* C402 = 0, clear all status. C403 = 1, to enable Done Status.*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

        /* Fill-up SPI Transmit Fifo To check SPI Status.
         * Write SPI Control Register Write Command.*/
        /* Write to Transmit Fifo D000-onwards.*/
        wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write Tx Fifo Register Address.*/
        wr_data = Q28_SPI_TXFIFO;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Tx Fifo Control Word-1.*/
        wr_data = ((0x0100) | Q28_MSGTYPE_HRD); /* D000 = Only 1 byte Message ( 1byte to transmit for RDSR opcode). */
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Tx Fifo Control Word-2.*/
        wr_data = Q28_RDSR_OPCODE;                  /* D002 = RDSR Opcode for transmit. */
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
                
        /* Write SPI Control Register Write Command.*/
        wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message(pa, wr_data, &rd_data));

        /* Write SPI Control -1 Register Address.*/
        wr_data = Q28_SPI_CTRL_1_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Control -1 Register Word-1.*/
        wr_data = 0x0101;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Control -1 Register Word-2.*/
        wr_data = 0x0100;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

        /*Write SPI Control Register Write Command.*/
        wr_data = ((Q28_WR_CPU_CTRL_REGS * 0x0100) | 1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

        /* Write SPI Control -1 Register Address.*/
        wr_data = Q28_SPI_CTRL_1_H;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Control -1 Register Word-2.*/
        wr_data = 0x0103;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
 
        do {
           wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1);
           PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
           wr_data = Q28_SPI_CTRL_1_L;
           PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
           retry_cnt ++;
        } while (((rd_data & 0x0100) == 0) && (retry_cnt < Q28_RETRY_CNT));
        if (retry_cnt >= Q28_RETRY_CNT) {
            PHYMOD_DIAG_OUT(("ERROR on retry cnt 1 \n"));
            return PHYMOD_E_CONFIG;
        }
        retry_cnt = 0;

        /* Write SPI Control Register Read Command.*/
        wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Control -1 Register Address.*/
        wr_data = Q28_SPI_RXFIFO;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        if ((rd_data & 1) == 0) {
            SPI_READY = 0;
        }
        retry_cnt_1 ++;
    }
    if (retry_cnt_1 >= Q28_RETRY_CNT) {
        PHYMOD_DIAG_OUT(("ERROR on retry cnt 2 \n"));
        return PHYMOD_E_CONFIG;
    }

    return PHYMOD_E_NONE;
}

int quadra28_rom_dload(const phymod_access_t *pa, uint8_t *new_fw, uint32_t fw_length)
{
    int retry_cnt = 0, retry_cnt_1 = 0;
    BCMI_QUADRA28_GENERAL_CONTROL_2r_t  gen_ctrl2;
    BCMI_QUADRA28_BOOT_CONTROLr_t   boot_ctrl;
    BCMI_QUADRA28_GENERAL_CONTROL_1r_t  gen_ctrl1;
    BCMI_QUADRA28_MSG_OUTr_t q28_msg_out ;
    int wr_data = 0, rd_data = 0, j = 0, i = 0,xfer_size = 0;
    uint32_t temp = 0;

    PHYMOD_MEMSET(&q28_msg_out, 0, sizeof(BCMI_QUADRA28_MSG_OUTr_t));
    PHYMOD_MEMSET(&gen_ctrl2, 0, sizeof(BCMI_QUADRA28_GENERAL_CONTROL_2r_t));
    PHYMOD_MEMSET(&gen_ctrl1, 0, sizeof(BCMI_QUADRA28_GENERAL_CONTROL_1r_t));
    PHYMOD_MEMSET(&boot_ctrl, 0, sizeof(BCMI_QUADRA28_BOOT_CONTROLr_t));

    /* clear bit 12 in FFD1 To use Countach address mapping */
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_CONTROL_2r(pa, &gen_ctrl2));
    BCMI_QUADRA28_GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(gen_ctrl2, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_CONTROL_2r(pa, gen_ctrl2));

     /* SPI boot from ROM*/
    BCMI_QUADRA28_BOOT_CONTROLr_SET(boot_ctrl, 0x4);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_BOOT_CONTROLr(pa, boot_ctrl));

    /* reset micro*/
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(gen_ctrl1, 1);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_CONTROL_1r(pa, gen_ctrl1));

    /* 5Ms sleep*/
    PHYMOD_USLEEP(5000);
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(gen_ctrl1, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_CONTROL_1r(pa, gen_ctrl1));

    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_READ_GENERAL_CONTROL_2r(pa, &gen_ctrl2));
    BCMI_QUADRA28_GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(gen_ctrl2, 0);
    PHYMOD_IF_ERR_RETURN(
        BCMI_QUADRA28_WRITE_GENERAL_CONTROL_2r(pa, gen_ctrl2));

    /* Sleep for 2 sec*/
    PHYMOD_SLEEP(2);

    /* Enable the LASI For Message out.*/
    PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(pa, 0x19000, 0x4));
    PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_write(pa, 0x19002, 0x4));

    /* De-assert SPI EEPROM Block Protection.
     * Write SPI Control Register Write Command.*/
    /* Write to C400, C401, C402, C403 Registers.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
    if (rd_data != 0xABCD) {
        PHYMOD_DIAG_OUT(("ERROR -MSG OUT register 0x%x It should be 0xABCD\n", rd_data));
        return PHYMOD_E_CONFIG;
    }

    /* Write SPI Control -2 Register Address*/
    wr_data = Q28_SPI_CTRL_2_L;
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Control -2 Register Word-1.*/
    wr_data = 0x8200;	 /* C400 = 0 for 0 Fill Byte. C401 = 02 for SPI Clcok to be divide by 2 and little endian fifo.*/
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
    if (rd_data != 0xABCD) {
       PHYMOD_DIAG_OUT(("ERROR -MSG OUT register 0x%x It should be 0xABCD\n", rd_data));
       return PHYMOD_E_CONFIG;
    }
    
    /* Write SPI Control -2 Register Word-2.*/
    wr_data = 0x0100;	/* C402 = 0, clear all status. C403 = 1, to enable Done Status.*/
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
    if (rd_data != 0x4321) {
       PHYMOD_DIAG_OUT(("ERROR -MSG OUT register 0x%x It should be 0x4321\n", rd_data));
       return PHYMOD_E_CONFIG;
    }

    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages.
     * Write SPI Control Register Write Command.*/
    /* Write to Transmit Fifo D000-onwards.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 4);
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write Tx Fifo Register Address.*/
    wr_data = Q28_SPI_TXFIFO;	
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Tx Fifo Control Word-1.*/
    wr_data = (0x0100 | Q28_MSGTYPE_HWR);  /* D000 = Only 1 byte Message ( 1byte to transmit for WREN opcode). */
    /* D001 = Write Message And  Higher byte of Transfer Size. */
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
        
    /* Write SPI Tx Fifo Control Word-2.*/
    wr_data = ((Q28_MSGTYPE_HWR * 0x0100) | Q28_WREN_OPCODE); /*{MSGTYPE_HWR, WREN_OPCODE}; D002 = WREN Opcode For transmit. */
    /* D003 = 2 byte WREN OPCODE Message.*/ 
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
        
    /* Write SPI Tx Fifo Control Word-3.*/
    wr_data = ((Q28_WRSR_OPCODE * 0x0100) | 0x02);  /* D004 = For this command transfer size =2 (1 byte opcode, 1 byte wr data),*/
    /* D005 = WRSR Opcode.*/
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));
           
    /* Write SPI Tx Fifo Control Word-4.*/
    wr_data = ((0x02 * 0x0100) | 0x02);     /* D006 = 0x02, byte for status register, 
                                              * Bit 7, WPEN = 0, Bit 3:2, BP1:0=0, No block protect, 
                                              * Bit 1, WEN = 1, Write Enabled.
                                              * D005 = WRSR Opcode.*/
    PHYMOD_IF_ERR_RETURN (quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Control Register Write Command.*/
    /* Write to SPI Control Register C000 onwards.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address.*/
    wr_data = Q28_SPI_CTRL_1_L;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Control -1 Register Word-1.*/
    wr_data = 0x0101;	/* C000 = 1 to clear CMD Done Status. C001 = 1 to clear CMD Done INT Status.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Control -1 Register Word-2.*/
    wr_data = 0x0003;	/* C002 = 3, Start Immidiate Command. C003 = 0, No Prepend byte for Write.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
    
    /* Wait For WRSR Command To be written*/
    do {
        wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        wr_data = Q28_SPI_CTRL_1_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        retry_cnt++;
    } while (((rd_data & 0x0100) == 0) && (retry_cnt < Q28_RETRY_CNT));
    if (retry_cnt >= Q28_RETRY_CNT) {
        return PHYMOD_E_CONFIG;
    }
    retry_cnt = 0;

    PHYMOD_IF_ERR_RETURN(q28_get_spi_eeprom_status(pa));

    for (j = 0 ; j < fw_length; j+= Q28_WR_BLOCK_SIZE) {
        /* Setup SPI Controller.*/
        /* Write SPI Control Register Write Command.*/
        /* Write to C400, C401, C402, C403 Registers.*/
        wr_data = ( (Q28_WR_CPU_CTRL_FIFO* 0x0100) | 2);  
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
        /* Write SPI Control -2 Register Address.*/
        wr_data = Q28_SPI_CTRL_2_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        
        /* Write SPI Control -2 Register Word-1.*/
        wr_data = 0x8200;    /* C400 = 0 for 0 Fill Byte. C401 = 02 for SPI Clcok to be divide by 2 and little endian fifo.*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

        /* Write SPI Control -2 Register Word-2.*/
        wr_data = 0x0100;    /* C402 = 0, clear all status. C403 = 1, to enable Done Status.*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
            
        /* Fill-up SPI Transmit Fifo.*/
        /* Write SPI Control Register Write Command.*/
        wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | (4 + (Q28_WR_BLOCK_SIZE/2)));
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        
        /* Write Tx Fifo Register Address.*/
        wr_data = Q28_SPI_TXFIFO;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        
        /* Write SPI Tx Fifo Control Word-1.*/
        wr_data = ( 0x0100 | Q28_MSGTYPE_HWR); 
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
        /* Write SPI Tx Fifo Control Word-2.*/
        wr_data = ((Q28_MSGTYPE_HWR * 0x0100) | Q28_WREN_OPCODE); 
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));          
           
        
        /* Write SPI Tx Fifo Control Word-3.*/
        wr_data = ((Q28_WR_OPCODE * 0x0100) | (0x3 + Q28_WR_BLOCK_SIZE));   
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
           
        
        /* Write SPI Tx Fifo Control Word-4.*/
        wr_data = (((j & 0x00FF) * 0x0100) | ((j & 0xFF00)/ 0x0100));
        /* D006 = SPI EEPROM Destination Address (higher byte sent out first). 
         * D007 = SPI EEPROM Destination Address (lower byte sent out later).*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

        xfer_size = fw_length - j;
        if ( xfer_size > Q28_WR_BLOCK_SIZE) { 
            xfer_size = Q28_WR_BLOCK_SIZE;  
        }
        for (i = 0 ; i<(xfer_size-1); i+=2) {
          	/* Write SPI Tx Fifo Data Word-4.*/
          	wr_data = ((new_fw [i+j+1] * 0x0100) | new_fw [i+j]);  /* D008 = Low Byte. D009 = High Byte.*/
          	PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        }
        
        /* Set-up SPI Controller To Transmit.*/
        /* Write SPI Control Register Write Command.*/
        /* Write to SPI Control Register C000 onwards.*/
        wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2); 
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

        /* Write SPI Control -1 Register Address.*/
        wr_data = Q28_SPI_CTRL_1_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        
        /* Write SPI Control -1 Register Word-1.*/
        wr_data = 0x0501;    /* C000 = 1 to clear CMD Done Status. C001 = 1 to clear CMD Done INT Status.*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        
        /* Write SPI Control -1 Register Word-2.*/
        wr_data = 0x0003;    /* C002 = 3, Start Immidiate Command. C003 = 0, No Prepend byte for Write.*/
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

        /* Wait For 64 bytes To be written. */
        if ((j % 2048) == 0) {       
            PHYMOD_DIAG_OUT(("INFO -- Write SPI EEPROM at address: 0x%x\n", j));
        }
        rd_data = 0;
        while (((rd_data & 0x0100) == 0) && (retry_cnt < Q28_RETRY_CNT)) { 
            /* Write SPI Control Register Read Command.*/
            /* Read from SPI Controller register C000.*/
            wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1); 
            PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        
            /* Write SPI Control -1 Register Address.*/
            wr_data = Q28_SPI_CTRL_1_L;
            PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
            retry_cnt ++; 
        }
        if (retry_cnt >= Q28_RETRY_CNT) {
            return PHYMOD_E_CONFIG;
        }
        retry_cnt = 0;
        PHYMOD_IF_ERR_RETURN(q28_get_spi_eeprom_status(pa));
    }
    PHYMOD_DIAG_OUT(("INFO -- Completes 0x%x bytes of SPI EEPROM write operation\n", j));

    /* Re-Assert SPI EEPROM Block Protection.*/
    /* Write SPI Control Register Write Command.*/
    /* Write to C400, C401, C402, C403 Registers.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
    
    /* Write SPI Control -2 Register Address.*/
    wr_data = Q28_SPI_CTRL_2_L;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Control -2 Register Word-1.*/
    wr_data = 0x8200;	/* C400 = 0 for 0 Fill Byte. C401 = 02 for SPI Clcok to be divide by 2 and little endian fifo.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Control -2 Register Word-2.*/
    wr_data = 0x0100;	/* C402 = 0, clear all status. C403 = 1, to enable Done Status.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
          
    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages.*/
    /* Write SPI Control Register Write Command.*/
    /* Write to Transmit Fifo D000-onwards.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 4);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write Tx Fifo Register Address.*/
    wr_data = Q28_SPI_TXFIFO;	/* D000.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Tx Fifo Control Word-1.*/
    wr_data = (0x0100 | Q28_MSGTYPE_HWR);
    /* D000 = Only 1 byte Message ( 1byte to transmit for WREN opcode). */
    /* D001 = Write Message And  Higher byte of Transfer Size. */
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Tx Fifo Control Word-2.*/
    wr_data = ((Q28_MSGTYPE_HWR *  0x0100) | Q28_WREN_OPCODE) ;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
    /* D003 = 2 byte WREN OPCODE Message*/
        
    /* Write SPI Tx Fifo Control Word-3.*/
    wr_data = ((Q28_WRSR_OPCODE * 0x0100) | 0x02);
    /* D004 = For this command transfer size =2 (1 byte opcode, 1 byte wr data),*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
    /* D005 = WRSR Opcode.*/
           
    /* Write SPI Tx Fifo Control Word-4.*/
    wr_data = ((0x0C * 0x0100) | 0x0C) ;
    /* D006 = 0x02, byte for status register, 
     * Bit 7, WPEN = 0, Bit 3:2, BP1:0=3, All block protected, 
     * Bit 1, WEN = 0, Write Enabled.
     * D005 = WRSR Opcode.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Control Register Write Command.*/
    /* Write to SPI Control Register C000 onwards.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Write SPI Control -1 Register Address.*/
    wr_data = Q28_SPI_CTRL_1_L;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
    /* Write SPI Control -1 Register Word-1.*/
    wr_data = 0x0101;	/* C000 = 1 to clear CMD Done Status. C001 = 1 to clear CMD Done INT Status.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
     
     /* Write SPI Control -1 Register Word-2.*/
    wr_data = 0x0003;	/* C002 = 3, Start Immidiate Command. C003 = 0, No Prepend byte for Write.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));

    /* Wait For WRSR Command To be written.          */
    rd_data = 0;
    while (((rd_data & 0x0100) == 0) && (retry_cnt < Q28_RETRY_CNT)) {
        /* Write SPI Control Register Read Command.*/
        /*Read from SPI Controller register C000.*/
        wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        wr_data = Q28_SPI_CTRL_1_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));
        retry_cnt ++;
    }
    if (retry_cnt >= Q28_RETRY_CNT) {
        return PHYMOD_E_CONFIG;
    }
    retry_cnt = 0;

    PHYMOD_IF_ERR_RETURN(q28_get_spi_eeprom_status(pa));

    /* Disable SPI EEPROM.*/
    /* Write SPI Control Register Write Command.*/
    /* Write to C400, C401, C402, C403 Registers.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    /* Write SPI Control -2 Register Address.*/
    wr_data = Q28_SPI_CTRL_2_L;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    /* Write SPI Control -2 Register Word-1.*/
    wr_data = 0x8200;	/* C400 = 0 for 0 Fill Byte. C401 = 02 for SPI Clcok to be divide by 2 and little endian fifo.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

    /* Write SPI Control -2 Register Word-2.*/
    wr_data = 0x0100;	/* C402 = 0, clear all status. C403 = 1, to enable Done Status.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    
    /* Fill-up SPI Transmit Fifo With SPI EEPROM Messages.*/
    /* Write SPI Control Register Write Command.*/
    wr_data = ( (Q28_WR_CPU_CTRL_FIFO* 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    /* Write Tx Fifo Register Address.*/
    wr_data = Q28_SPI_TXFIFO;	   /* D000.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    

    /* Write SPI Tx Fifo Control Word-1.*/
    wr_data = ((0x0100) | Q28_MSGTYPE_HWR); /* D000 = Only 1 byte Message ( 1byte to transmit for WRDI opcode). */
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data));             /* D001 = Write Message And  Higher byte of Transfer Size.*/

    
    /* Write SPI Tx Fifo Control Word-2.*/
    wr_data = Q28_WRDI_OPCODE; /* D002 = WRDI Opcode for transmit. */
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
       
    /* Write SPI Control Register Write Command.*/
    /* Write to SPI Control Register C000 onwards.*/
    wr_data = ((Q28_WR_CPU_CTRL_FIFO * 0x0100) | 2);
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

    /* Write SPI Control -1 Register Address.*/
    wr_data = Q28_SPI_CTRL_1_L;
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    /* Write SPI Control -1 Register Word-1.*/
    wr_data = 0x0101;	/* C000 = 1 to clear CMD Done Status. C001 = 1 to clear CMD Done INT Status.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
    
    /*Write SPI Control -1 Register Word-2.*/
    wr_data = 0x0003;	/* C002 = 3, Start Immidiate Command. C003 = 0, No Prepend byte for Write.*/
    PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 

    /* Wait For WRDI Command To be written.*/
    rd_data = 0;
    while (((rd_data & 0x0100) == 0) && (retry_cnt < Q28_RETRY_CNT)) {
       /* Write SPI Control Register Read Command.*/
       /* Read from SPI Controller register C000.*/
        wr_data = ((Q28_RD_CPU_CTRL_REGS * 0x0100) | 1);
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
        if (rd_data != 0xABCD) {
            PHYMOD_DIAG_OUT(("ERROR -MSG OUT register 0x%x It should be 0xABCD\n", rd_data));
            return PHYMOD_E_CONFIG;
        }
        /* Write SPI Control -1 Register Address.*/
        wr_data = Q28_SPI_CTRL_1_L;
        PHYMOD_IF_ERR_RETURN(quadra28_write_message (pa, wr_data, &rd_data)); 
     
        temp = 0x0000;
        while (((temp & 0x4) == 0) && (retry_cnt_1 < Q28_RETRY_CNT)) {
            PHYMOD_IF_ERR_RETURN(phymod_raw_iblk_read(pa, 0x19005, &temp));
            retry_cnt_1 ++;
        }
        if (retry_cnt_1 >= Q28_RETRY_CNT) {
            return PHYMOD_E_CONFIG;
        }
        retry_cnt_1 = 0;

        PHYMOD_IF_ERR_RETURN(
            BCMI_QUADRA28_READ_MSG_OUTr(pa, &q28_msg_out));
        wr_data	= BCMI_QUADRA28_MSG_OUTr_GET(q28_msg_out);
        retry_cnt ++;
    }
    if (retry_cnt >= Q28_RETRY_CNT) {
        return PHYMOD_E_CONFIG;
    }
    PHYMOD_DIAG_OUT(("UCODE Downloaded on EEPROM \n"));
    return PHYMOD_E_NONE;
}

int quadra28_before_fw_load(const phymod_core_access_t* core,
                            const phymod_core_init_config_t *init_config,
                            uint32_t chip_id)
{
    phymod_access_t pa;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl_reg;
    uint16_t index = 0, max_lane = 0;

    max_lane = (chip_id == QUADRA28_82752_CHIP_ID) ? 1 : 3; 
    PHYMOD_MEMSET(&mdio_broadcast_ctrl_reg, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    PHYMOD_MEMCPY(&pa, &core->access, sizeof( phymod_access_t));

    PHYMOD_IF_ERR_RETURN(
        _quadra28_clear_mode_config(&pa));
    
    for (index = 0; index <= max_lane; index++) {
        pa.addr &=  ~max_lane;
        pa.addr |=  index;
        /* Step1: Enable broadcast mode */
        PHYMOD_IF_ERR_RETURN
            (READ_MDIO_BROADCAST_CONTROLr(&pa, &mdio_broadcast_ctrl_reg));
        MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(mdio_broadcast_ctrl_reg, 1);     
        PHYMOD_IF_ERR_RETURN
            (WRITE_MDIO_BROADCAST_CONTROLr(&pa, mdio_broadcast_ctrl_reg));
    }

    return PHYMOD_E_NONE;
}

int quadra28_bcast_fw_load(const phymod_core_access_t* core,
                           const phymod_core_init_config_t *init_config, 
                           uint8_t *new_fw,
                           uint32_t fw_length)
{
    phymod_access_t pa;
    GENERAL_CONTROL_2r_t general_ctrl_reg2;
    GENERAL_CONTROL_1r_t general_ctrl_reg1;
    GEN_CTRLr_t general_ctrl_reg;
    SPI_PORT_CONTROL_STATUSr_t spi_port_ctrl_reg;
    MSG_INr_t msg_in_reg;
    CL73CONTROL8r_t cl73_ctrl8_reg;
    uint32_t index = 0;
    uint16_t data16;

    PHYMOD_MEMSET(&cl73_ctrl8_reg, 0, sizeof(CL73CONTROL8r_t));
    PHYMOD_MEMSET(&general_ctrl_reg1, 0, sizeof(GENERAL_CONTROL_1r_t));
    PHYMOD_MEMSET(&general_ctrl_reg, 0, sizeof(GEN_CTRLr_t));
    PHYMOD_MEMSET(&spi_port_ctrl_reg, 0, sizeof(BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_t));
    PHYMOD_MEMSET(&msg_in_reg, 0, sizeof(MSG_INr_t));
    PHYMOD_MEMSET(&general_ctrl_reg2, 0, sizeof(GENERAL_CONTROL_2r_t));
    PHYMOD_MEMCPY(&pa, &core->access, sizeof( phymod_access_t));
   

    /* Set 0xFFD1 to read from MGT-M8051 register */
    PHYMOD_IF_ERR_RETURN
        (READ_GENERAL_CONTROL_2r(&pa,&general_ctrl_reg2));
    GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(general_ctrl_reg2, 1);
    
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_2r(&pa,general_ctrl_reg2));
    PHYMOD_MEMSET(&general_ctrl_reg2, 0, sizeof(GENERAL_CONTROL_2r_t)); 
  
    /* Step5: Select MDIO interface for microcode download */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_SPI_PORT_CONTROL_STATUSr(&pa, 
            &spi_port_ctrl_reg));

    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_PORT_USEDf_SET(spi_port_ctrl_reg, 0);                       
    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_BOOTf_SET(spi_port_ctrl_reg, 1);
    BCMI_QUADRA28_SPI_PORT_CONTROL_STATUSr_SPI_DWLD_DONEf_SET(spi_port_ctrl_reg, 0);
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_SPI_PORT_CONTROL_STATUSr(&pa, 
            spi_port_ctrl_reg));    
 
    /* Step3: Reset MGT-M8051 uc at address 1.FFD0 */
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(general_ctrl_reg1, 1);
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_1r(&pa,
            general_ctrl_reg1));

    /* Step2: Reset EDc-M8051 uc at address 1.CA10 */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_GEN_CTRLr(&pa, &general_ctrl_reg));

    BCMI_QUADRA28_GEN_CTRLr_UCRSTf_SET(general_ctrl_reg, 1);

    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_GEN_CTRLr(&pa, general_ctrl_reg));   
   
           
    /* Step8:  Release EDC-M8051 from reset */
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_READ_GEN_CTRLr(&pa, &general_ctrl_reg));
    BCMI_QUADRA28_GEN_CTRLr_UCRSTf_SET(general_ctrl_reg, 0);
    PHYMOD_IF_ERR_RETURN
        (BCMI_QUADRA28_WRITE_GEN_CTRLr(&pa, general_ctrl_reg));
    
    /* step9: Release MGT_M8051 uc from reset */    
    BCMI_QUADRA28_GENERAL_CONTROL_1r_SET(general_ctrl_reg1, 0);
    PHYMOD_IF_ERR_RETURN
        (WRITE_GENERAL_CONTROL_1r(&pa, general_ctrl_reg1));
   
    /* step10: wait 1 ms */
    PHYMOD_USLEEP(1000);

    /* step11: send initial RAM address */
    BCMI_QUADRA28_MSG_INr_SET (msg_in_reg, 0x8000);
    PHYMOD_IF_ERR_RETURN
       (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));

    /* step12: wait 15 us */
    PHYMOD_USLEEP(20);   
   
    /* step13: send microcode size to 1.ca12 */
    BCMI_QUADRA28_MSG_INr_MESSAGE_INf_SET(msg_in_reg,fw_length/2);
    PHYMOD_IF_ERR_RETURN
       (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));

    PHYMOD_USLEEP(20);
    /* step15 to step 16: send 16-bit microcode word */
    PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_READ_MSG_INr(&pa,
                &msg_in_reg)); 
    
    for (index = 0; index < (fw_length) - 1; index+=2){
        data16 = ((new_fw[index] << 8)| new_fw[index+1]);
        PHYMOD_USLEEP(15);
        BCMI_QUADRA28_MSG_INr_MESSAGE_INf_SET(msg_in_reg,data16);
        PHYMOD_IF_ERR_RETURN
            (BCMI_QUADRA28_WRITE_MSG_INr(&pa, msg_in_reg));
    }
    
    return PHYMOD_E_NONE;
}

int quadra28_bcast_after_fw_load(const phymod_core_access_t* core, uint32_t chip_id)
{
    uint32_t phy_addr = 0;
    uint16_t index = 0, max_lane = 0;
    phymod_access_t pa;
    GENERAL_CONTROL_2r_t general_ctrl_reg2;
    int ret_val = 0;

    PHYMOD_MEMSET(&general_ctrl_reg2, 0, sizeof(GENERAL_CONTROL_2r_t));
    PHYMOD_MEMCPY(&pa, &core->access, sizeof( phymod_access_t));
    phy_addr = pa.addr;
    max_lane = (chip_id == QUADRA28_82752_CHIP_ID) ? 1 : 3; 
    PHYMOD_IF_ERR_RETURN
       (READ_GENERAL_CONTROL_2r(&pa,&general_ctrl_reg2));
    GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(general_ctrl_reg2, 1);
    PHYMOD_IF_ERR_RETURN
      (WRITE_GENERAL_CONTROL_2r(&pa,general_ctrl_reg2));        

    PHYMOD_USLEEP(4000);
    /* Do it from all three locations */
    /* if not read, they will be in wait and not update checksum */
    /* Q28 3 processors - flip FFD1 bits to read either from MGT-M8051 or EDC-M8051 */
    /* Verify for all processors - 3 in each */
    /* use phy address for other two EDC-M8051 lanes */
    /* Verify checksum on MGT-M8051 for a phy lane */
    for (index = 0; index <= max_lane; index+=2) {
    int ret_val = 0;
         pa.addr &=  ~max_lane;
         pa.addr |=  index;
         if ((pa.addr & 1) == 0) {
            ret_val = _quadra28_verify_fw_download(&pa, 0);
            if (ret_val != 0) {
               return ret_val;
            }
         }
    }
    pa.addr = phy_addr ;
    PHYMOD_IF_ERR_RETURN
        (READ_GENERAL_CONTROL_2r(&pa,&general_ctrl_reg2));
    GENERAL_CONTROL_2r_LEGACY_BOOT_SELf_SET(general_ctrl_reg2, 0);
    PHYMOD_IF_ERR_RETURN
       (WRITE_GENERAL_CONTROL_2r(&pa,general_ctrl_reg2));        
    PHYMOD_USLEEP(4000);
    /* Verification for EDC*/
    for (index = 0; index <= max_lane; index++) {
        pa.addr &=  ~max_lane;
        pa.addr |=  index;
        ret_val = _quadra28_verify_fw_download(&pa, 1);
        if (ret_val != 0) {
            return ret_val;
        }
    }

    return PHYMOD_E_NONE;
}

int quadra28_bcast_disable(const phymod_core_access_t* core, uint32_t chip_id)
{
    phymod_access_t pa;
    MDIO_BROADCAST_CONTROLr_t mdio_broadcast_ctrl_reg;
    uint16_t index = 0, max_lane = 0;

    max_lane = (chip_id == QUADRA28_82752_CHIP_ID) ? 1 : 3; 
    PHYMOD_MEMSET(&mdio_broadcast_ctrl_reg, 0, sizeof(MDIO_BROADCAST_CONTROLr_t));
    PHYMOD_MEMCPY(&pa, &core->access, sizeof( phymod_access_t));

    for (index = 0; index <= max_lane; index++) {
        pa.addr &=  ~max_lane;
        pa.addr |=  index;
        /* Step1: Enable broadcast mode */
        PHYMOD_IF_ERR_RETURN
            (READ_MDIO_BROADCAST_CONTROLr(&pa, &mdio_broadcast_ctrl_reg));
        MDIO_BROADCAST_CONTROLr_MDIO_BCAST_ENf_SET(mdio_broadcast_ctrl_reg, 1);     
        PHYMOD_IF_ERR_RETURN
            (WRITE_MDIO_BROADCAST_CONTROLr(&pa, mdio_broadcast_ctrl_reg));
    }

    return PHYMOD_E_NONE;
}

