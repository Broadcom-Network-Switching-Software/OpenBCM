/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



/*
 * cmicx_sim_schan.c
 *
 * implements support for schan operations in the cmicx simulation
 */


#include <string.h>
#ifndef CMICX_STANDALONE_SIM
 #include <../systems/sim/pcid/cmicx_sim.h>
 #include <../systems/sim/pcid/cmicx_sim_pcid.h>
 #include <../systems/sim/pcid/cmicx_sim_schan.h>
 #include <../systems/sim/pcid/cmicsim.h>
 #include <../include/soc/schanmsg.h>
#else
 #include "cmicx_sim.h"
 #include "cmicx_sim_pcid.h"
 #include "cmicx_sim_schan.h"
#endif


/* external cmicx global pointer */
extern cmicx_t *cmicx;

/* intializes an schan channel */
bool cmicx_schan_init(cmicx_schan_data_t *schan_data,char *id_string) {

    int i;

    /* if the given id string is too long, log an error and quit */
    if ((strlen(id_string)+1)>CMIC_SCHAN_ID_STRING_LENGTH) {
        cmicx_error("ERROR: given id string \"");
        cmicx_error(id_string);
        cmicx_error("\" for schan is too long.\n");
        return false;
    }

    /* copy the id string to the storage for the schan */
    strcpy(schan_data->id_string,id_string);

    /* set all control fields of the schan data structure */
    schan_data->ctrl_schan_error    = false;
    schan_data->ctrl_timeout        = false;
    schan_data->ctrl_nack           = false;
    schan_data->ctrl_ser_check_fail = false;
    schan_data->ctrl_abort          = false;
    schan_data->ctrl_msg_done       = false;
    schan_data->ctrl_msg_start      = false;

    /* error register states */
    schan_data->err_op_code   = 0;
    schan_data->err_dst_port  = 0;
    schan_data->err_src_port  = 0;
    schan_data->err_data_len  = 0;
    schan_data->err_errbit    = 0;
    schan_data->err_err_code  = 0;
    schan_data->err_nack      = 0;

    /* ack data beat count register states */
    schan_data->ack_data_beat_count = 0;

    /* message registers */
    for (i=0; i<CMIC_SCHAN_NUM_MESSAGE_REGS; i++) {
        schan_data->message_reg[i] = 0;
    }

    /* set pending access fields fo the schan structure */
    schan_data->access_running   = false;
    schan_data->access_opcode    = 0;
    schan_data->access_countdown = 0;

    /* successful initialization complete */
    cmicx_log("successfully init'd \"");
    cmicx_log(schan_data->id_string);
    cmicx_log("\".");
    cmicx_log_newline();
    return true;
}


/* updates the state of a given schan */
bool cmicx_schan_update(pcid_info_t *pcid_info, cmicx_schan_data_t *schan_data)
{
    /* variables */
    int                  num_words;
    reg_t                ack_command[CMIC_SCHAN_NUM_MESSAGE_REGS];
    int                  ack_num_words = 0;

    /* print log messages for this schan */
    cmicx_log_level_set(0);
    cmicx_log("updating \"");
    cmicx_log(schan_data->id_string);
    cmicx_log("\"... ");
    cmicx_log_level_set(3);


    /* if we have an access running, decrement the timeout and return ACK if necessary */
    /* if not, we just bail */
    if (schan_data->access_running) {
        schan_data->access_countdown--;
        if (schan_data->access_countdown==0) {

            /* complete the sbus command access if possible */
            if (cmic_schan_sbus_command_complete(pcid_info, schan_data->message_reg,ack_command,1,&num_words)==true) {

                cmicx_log(" command was completed on \"");
                cmicx_log(schan_data->id_string);
                cmicx_log("\".");
                cmicx_log_newline();
                ack_num_words = (CMIC_SCHAN_COMMAND_STRIP_DLEN(ack_command[0])/CMICX_BYTES_PER_WORD)+1;
                memcpy((void *)&(schan_data->message_reg[0]),(void *)ack_command,(ack_num_words*sizeof(reg_t)));   /* note we are overwriting the schan command data here. this is the desired behavior. */

                /* set control signals after successfully completing ack for sbus command */
                schan_data->access_running = false;
                schan_data->ctrl_msg_done  = true;
                schan_data->ctrl_msg_start = false;
                schan_data->ack_data_beat_count = (ack_num_words-1);

            }
            else {
                cmicx_log("ERROR: could not successfully start sbus command for new schan access.\n");
                return false;
            }

        }

    }

    /* end update successfully */
    cmicx_log_level_set(0);
    cmicx_log("done.");
    cmicx_log_newline();
    cmicx_log_level_set(3);
    return true;
}


/* schan command building function (from opcode, blockid/dport, etc.) */
void cmic_schan_command_build(schan_command_opcode opcode,int dport,int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command) {

    *command = 0;
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_OPCODE_MSB,  CMIC_SCHAN_COMMAND_OPCODE_LSB,  (int)opcode);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_DPORT_MSB,   CMIC_SCHAN_COMMAND_DPORT_LSB,         dport);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_DLEN_MSB,    CMIC_SCHAN_COMMAND_DLEN_LSB,           dlen);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_ERR_BIT,                                             err);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_ECODE_MSB,   CMIC_SCHAN_COMMAND_ECODE_LSB,         ecode);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_DMA_BIT,                                             dma);
    REG_FIELD_SET_TO(*command,CMIC_SCHAN_COMMAND_BANK_MSB,    CMIC_SCHAN_COMMAND_BANK_LSB,           bank);
    REG_BIT_SET_TO(  *command,CMIC_SCHAN_COMMAND_NACK_BIT,                                           nack);

}


/* initiates an schan command access */
bool cmic_schan_access_start(cmicx_schan_data_t *schan_data) {

    /* variables */
    reg_t sbus_command = schan_data->message_reg[0];
    int   i;
    bool is_write;

    /* logging */
    cmicx_log("attempting to start new schan command access on \"");
    cmicx_log(schan_data->id_string);
    cmicx_log("\"... ");

    /* if there is an access currently running or software hasnt cleared the msg_done bit, then we have a problem */
    if (schan_data->access_running) {
       cmicx_log("failed. access already pending.\n");
       return false;
    }
    if (schan_data->ctrl_msg_done)
    {
       cmicx_log("failed. schan msg_done bit not cleared before starting new access (setting msg_start).\n");
       return false;
    }

    /* begin a new sbus command access if possible, based on the sbus command (first word/"opcode") */
    if (cmic_schan_sbus_command_start(sbus_command,&is_write)==true) {

        /* set variables for successfully beginning schan access, and print log messages */
        schan_data->access_running   = true;
        schan_data->access_opcode    = CMIC_SCHAN_COMMAND_STRIP_OPCODE(sbus_command);
        schan_data->access_countdown = CMIC_SCHAN_ACCESS_COUNTDOWN_VALUE;
        cmicx_log(" mem/data words with command: ");
        for (i=0; i<(CMIC_SCHAN_COMMAND_STRIP_DLEN(sbus_command)/CMICX_BYTES_PER_WORD); i++) {
            cmicx_log_reg(schan_data->message_reg[1+i]);
            cmicx_log(", ");
        }
        cmicx_log_newline();

    }
    else {
        cmicx_log("ERROR: could not successfully start sbus command for new schan access.\n");
        return false;
    }

    return true;

}


/* attempts to abort a currently active schan access */
bool cmic_schan_access_abort(cmicx_schan_data_t *schan_data) {

    /* if we have an access running, abort it and log a message, otherwise, just log a message that we could not abort. */
    if (schan_data->access_running) {
        schan_data->access_running = false;
        cmicx_log("schan access on \"");
        cmicx_log(schan_data->id_string);
        cmicx_log("\" successfully aborted.");
        cmicx_log_newline();
        return true;
    }
    else {
        cmicx_log("could not abort schan access on \"");
        cmicx_log(schan_data->id_string);
        cmicx_log("\", no access currently pending.\n");
        return false;
    }

}


/* gets an sbus ring number from a block id, using the SBUS_RING_MAP registers configured by software */
void cmic_schan_sbus_ring_get(int blockid,int *ringnum) {

    int ring_map_field_lsb = 0;
    int ring_map_field_msb = 0;

    switch (blockid%CMIC_SBUS_RINGS_PER_MAP_REG) {
        case 0:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_0_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_0_MSB;
            break;
        case 1:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_1_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_1_MSB;
            break;
        case 2:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_2_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_2_MSB;
            break;
        case 3:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_3_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_3_MSB;
            break;
        case 4:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_4_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_4_MSB;
            break;
        case 5:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_5_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_5_MSB;
            break;
        case 6:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_6_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_6_MSB;
            break;
        case 7:
            ring_map_field_lsb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_7_LSB;
            ring_map_field_msb = CMIC_SBUS_RING_MAP_RING_NUM_SBUS_ID_7_MSB;
            break;
    }

    switch (blockid/CMIC_SBUS_RINGS_PER_MAP_REG) {
        case 0:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_0_7_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 1:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_8_15_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 2:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_16_23_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 3:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_24_31_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 4:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_32_39_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 5:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_40_47_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 6:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_48_55_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 7:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_56_63_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 8:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_64_71_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 9:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_72_79_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 10:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_80_87_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 11:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_88_95_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 12:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_96_103_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 13:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_104_111_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 14:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_112_119_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        case 15:
            *ringnum = REG_FIELD_GET(cmicx->sbus_ring_map_120_127_reg,ring_map_field_msb,ring_map_field_lsb);
            return;
        default:
            cmicx_log("Bad block id number when attempting to get sbus ring number from sbus ring map\n");
            return;
    }

}

/* helper function to start an sbus command. used by schan and sbus_dma */
bool cmic_schan_sbus_command_start(reg_t sbus_command,bool *is_write) {

    /* variables */
    schan_command_opcode opcode     = CMIC_SCHAN_COMMAND_STRIP_OPCODE(sbus_command);
    int                  blockid    = CMIC_SCHAN_COMMAND_STRIP_DPORT( sbus_command);
    char                 opcode_id[16];
    int                  ringnum;

    /* get the sbus ringnum based on the blockid */
    cmic_schan_sbus_ring_get(blockid,&ringnum);

    /* switch the opcode to determine the new type of access, and check that the dlen corresponds to known access limits. */
    /* begin a new sbus command access if possible. */
    switch (opcode) {
        case SCHAN_MEM_RD:
            strcpy(opcode_id,"MEM_RD");
            *is_write = false;
            break;
        case SCHAN_MEM_WR:
            strcpy(opcode_id,"MEM_WR");
            *is_write = true;
            break;
        case SCHAN_REG_RD:
            strcpy(opcode_id,"REG_RD");
            *is_write = false;
            break;
        case SCHAN_REG_WR:
            strcpy(opcode_id,"REG_WR");
            *is_write = true;
            break;
        case SCHAN_TBL_INSERT:
            strcpy(opcode_id,"TBL_INSERT");
            *is_write = true;
            break;
        case SCHAN_TBL_DELETE:
            strcpy(opcode_id,"TBL_DELETE");
            *is_write = true;
            break;
        case SCHAN_TBL_LOOKUP:
            strcpy(opcode_id,"TBL_LOOKUP");
            *is_write = true;
            break;
        case SCHAN_PUSH:
            strcpy(opcode_id,"PUSH");
            *is_write = true;
            break;
        case SCHAN_POP:
            strcpy(opcode_id,"POP");
            *is_write = false;
            break;
        case SCHAN_MEM_RD_ACK:       /* pass-through */
        case SCHAN_MEM_WR_ACK:       /* pass-through */
        case SCHAN_REG_RD_ACK:       /* pass-through */
        case SCHAN_REG_WR_ACK:       /* pass-through */
        case SCHAN_TBL_INSERT_ACK:   /* pass-through */
        case SCHAN_TBL_DELETE_ACK:   /* pass-through */
        case SCHAN_TBL_LOOKUP_ACK:   /* pass-through */
        case SCHAN_PUSH_ACK:         /* pass-through */
        case SCHAN_POP_ACK:          /* pass-through */
        default:
            cmicx_log("ERROR: invalid sbus command opcode supplied for new sbus command access.\n");
            return false;
    }

    /* display log message for successful start of sbus command and return true */
    cmicx_log("successfully started ");
    cmicx_log(opcode_id);
    cmicx_log(" sbus command access. driving command on ring ");
    cmicx_log_int(ringnum);
    cmicx_log(". ");
    return true;

}

/* helper function to complete an sbus command. used by schan and sbus dma. command points to an array containing the actual sbus command (first word) and all */
/* associated data words/addresses. likewise, ack_command points to an array containing the response sbus command (ack) and all data wordsd that may come with it. */
/* expected resp words is the expected number of data words in a response (may not be what is actually returned). num_words_in_command tells the caller how many words */
/* in total were in the original sent sbus command. */
bool cmic_schan_sbus_command_complete(pcid_info_t *pcid_info, reg_t *command,reg_t *ack_command,int expected_resp_words,int *num_words_in_command) {

    /* variables */
    int                  dlen, opcode, dst_blk, acc_type;
    char                 opcode_id[16];
    schan_command_opcode ack_opcode;
    int                  ack_dlen;
    schan_msg_t          *msg;

    soc_schan_header_cmd_get(pcid_info->unit, (schan_header_t *)&command[0], &opcode,
                             &dst_blk, NULL, &acc_type, &dlen, NULL, NULL);
    msg = (schan_msg_t *)command;

    /* determine which ack we need to send and the ack dlen, based on the command opcode */
    switch (opcode)
    {
        case SCHAN_MEM_RD:
            (void)soc_internal_extended_read_mem(pcid_info, dst_blk, acc_type,
                                                 msg->readcmd.address,
                                                 &(ack_command[1]));
            ack_opcode = SCHAN_MEM_RD_ACK;
            ack_dlen   = (expected_resp_words*CMIC_SBUS_DMA_BYTES_PER_WORD);
            strcpy(opcode_id, "MEM_RD");
            *num_words_in_command = 2;
            break;
        case SCHAN_MEM_WR:
            (void)soc_internal_extended_write_mem(pcid_info, dst_blk, acc_type,
                                                  msg->writecmd.address,
                                                  msg->writecmd.data);
            ack_opcode = SCHAN_MEM_WR_ACK;
            ack_dlen   = 0;
            strcpy(opcode_id, "MEM_WR");
            *num_words_in_command = 2+(dlen/CMIC_SBUS_DMA_BYTES_PER_WORD);
            break;
        case SCHAN_REG_RD:
            cmicx_switch_reg_read(command[1],&ack_command[1]);
            ack_opcode = SCHAN_REG_RD_ACK;
            ack_dlen   = CMIC_SBUS_DMA_BYTES_PER_WORD;
            strcpy(opcode_id, "REG_RD");
            *num_words_in_command = 2;
            break;
        case SCHAN_REG_WR:
            cmicx_switch_reg_write(command[1],command[2]);
            ack_opcode = SCHAN_REG_WR_ACK;
            ack_dlen   = 0;
            strcpy(opcode_id, "REG_WR");
            *num_words_in_command = 3;
            break;
        case SCHAN_TBL_INSERT:
            ack_opcode = SCHAN_TBL_INSERT_ACK;
            ack_dlen   = 0;
            strcpy(opcode_id, "TBL_INSERT");
            *num_words_in_command = 3;
            break;
        case SCHAN_TBL_DELETE:
            ack_opcode = SCHAN_TBL_DELETE_ACK;
            ack_dlen   = 0;
            strcpy(opcode_id, "TBL_DELETE");
            *num_words_in_command = 2;
            break;
        case SCHAN_TBL_LOOKUP:
            ack_opcode     = SCHAN_TBL_LOOKUP_ACK;
            ack_dlen       = CMIC_SBUS_DMA_BYTES_PER_WORD;
            ack_command[1] = 0xBEEF;
            strcpy(opcode_id, "TBL_LOOKUP");
            *num_words_in_command = 2;
            break;
        case SCHAN_PUSH:
            ack_opcode = SCHAN_PUSH_ACK;
            ack_dlen   = 0;
            strcpy(opcode_id, "PUSH");
            *num_words_in_command = 2;
            break;
        case SCHAN_POP:
            ack_opcode     = SCHAN_POP_ACK;
            ack_dlen       = CMIC_SBUS_DMA_BYTES_PER_WORD;
            ack_command[1] = 0xBEEF;
            strcpy(opcode_id, "POP");
            *num_words_in_command = 1;
            break;
        default:
            cmicx_log("ERROR: invalid opcode found in sbus command during command completion. sbus command address: ");
            cmicx_log_addr(command[0]);
            cmicx_log("\n");
            return false;
    }

    /* reset the ack command data (to avoid unwritten fields carrying over data data) and then build the ack sbus command. */
    /* format for command build function is: void cmic_schan_command_build(schan_command_opcode opcode,int dport,int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command); */
    ack_command[0] = 0;
    cmic_schan_command_build(ack_opcode,1,ack_dlen, false,0,false,0,false, &ack_command[0]);

    /* log messages for completing sbus command and return */
    cmicx_log("sbus command completing, generated ");
    cmicx_log(opcode_id);
    cmicx_log(" ack. ");
    return true;

}



/*******************************************************************/
/* helper functions to write/read to/from specific schan registers */
/*******************************************************************/

/* generic schan control read function */
bool cmic_schan_ctrl_read(reg_t *val,cmicx_schan_data_t *schan_data) {

    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_SCHAN_ERROR_BIT,   schan_data->ctrl_schan_error   );
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_TIMEOUT_BIT,       schan_data->ctrl_timeout       );
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_NACK_BIT,          schan_data->ctrl_nack          );
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_SER_CHECK_FAIL_BIT,schan_data->ctrl_ser_check_fail);
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_ABORT_BIT,         schan_data->ctrl_abort         );
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_MSG_DONE_BIT,      schan_data->ctrl_msg_done      );
    REG_BIT_SET_TO(*val,CMIC_SCHAN_CTRL_REG_MSG_START_BIT,     schan_data->ctrl_msg_start     );
    return true;

}

/* generic schan control write function */
bool cmic_schan_ctrl_write(reg_t val,cmicx_schan_data_t *schan_data) {

    bool new_ctrl_abort_bit        = REG_BIT_GET(val,CMIC_SCHAN_CTRL_REG_ABORT_BIT    );
    bool new_ctrl_msg_done_bit     = REG_BIT_GET(val,CMIC_SCHAN_CTRL_REG_MSG_DONE_BIT );
    bool new_ctrl_msg_start_bit    = REG_BIT_GET(val,CMIC_SCHAN_CTRL_REG_MSG_START_BIT);

    bool clear_error_bits          = (new_ctrl_msg_start_bit || !new_ctrl_msg_done_bit);

    if (clear_error_bits) {
        schan_data->ctrl_schan_error    = false;
        schan_data->ctrl_timeout        = false;
        schan_data->ctrl_nack           = false;
        schan_data->ctrl_ser_check_fail = false;
    }

    schan_data->ctrl_abort          = new_ctrl_abort_bit;
    schan_data->ctrl_msg_done       = new_ctrl_msg_done_bit;
    schan_data->ctrl_msg_start      = new_ctrl_msg_start_bit;   /* should this be set when software sets it? */


    if (new_ctrl_msg_done_bit==false) {
        cmicx_log("schan \"");
        cmicx_log(schan_data->id_string);
        cmicx_log("\" msg_done bit cleared, schan now ready for new command.");
        cmicx_log_newline();
    }

    if (new_ctrl_msg_start_bit) {
        cmic_schan_access_start(schan_data);
    }

    if (new_ctrl_abort_bit) {
        cmic_schan_access_abort(schan_data);
    }

    return true;
}

/* generic helper function to read from the schan error register */
bool cmic_schan_err_read(reg_t *val,cmicx_schan_data_t *schan_data) {

    REG_FIELD_SET_TO(*val,CMIC_SCHAN_ERR_REG_OP_CODE_MSB, CMIC_SCHAN_ERR_REG_OP_CODE_LSB, schan_data->err_op_code  );
    REG_FIELD_SET_TO(*val,CMIC_SCHAN_ERR_REG_DST_PORT_MSB,CMIC_SCHAN_ERR_REG_DST_PORT_LSB,schan_data->err_dst_port );
    REG_FIELD_SET_TO(*val,CMIC_SCHAN_ERR_REG_SRC_PORT_MSB,CMIC_SCHAN_ERR_REG_SRC_PORT_MSB,schan_data->err_src_port );
    REG_FIELD_SET_TO(*val,CMIC_SCHAN_ERR_REG_DATA_LEN_LSB,CMIC_SCHAN_ERR_REG_DATA_LEN_LSB,schan_data->err_data_len );
    REG_BIT_SET_TO(  *val,CMIC_SCHAN_ERR_REG_ERRBIT_BIT,                                  schan_data->err_errbit   );
    REG_FIELD_SET_TO(*val,CMIC_SCHAN_ERR_REG_ERR_CODE_LSB,CMIC_SCHAN_ERR_REG_ERR_CODE_LSB,schan_data->err_err_code );
    REG_BIT_SET_TO(  *val,CMIC_SCHAN_ERR_REG_NACK_BIT,                                    schan_data->err_nack     );
    return true;

}

/* generic helper function to read from the schan ack data beat count register */
bool cmic_schan_ack_data_beat_count_read(reg_t *val,cmicx_schan_data_t *schan_data) {

    REG_FIELD_SET_TO(*val, CMIC_SCHAN_ACK_DATA_BEAT_COUNT_REG_BEAT_COUNT_MSB,CMIC_SCHAN_ACK_DATA_BEAT_COUNT_REG_BEAT_COUNT_LSB, schan_data->ack_data_beat_count );
    return true;

}


/*************************************************/
/* define schan register direct access functions */
/*************************************************/

/* sbus ring arb ctrl schan register read and write functions */
bool cmic_top_sbus_ring_arb_ctrl_schan_read(reg_t *val) {
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_LSB, cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[0]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_LSB, cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[1]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_LSB, cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[2]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_LSB, cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[3]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_LSB, cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[4]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_schan_common_fifo_weight);
    return true;
}
bool cmic_top_sbus_ring_arb_ctrl_schan_write(reg_t val) {
    cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[0] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH0_WEIGHT_LSB );
    cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[1] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH1_WEIGHT_LSB );
    cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[2] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH2_WEIGHT_LSB );
    cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[3] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH3_WEIGHT_LSB );
    cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[4] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_MSB, CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_CH4_WEIGHT_LSB );
    cmicx->sbus_ring_arb_ctrl_schan_common_fifo_weight  = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_COMMON_FIFO_WEIGHT_LSB);
    return true;
}

/* auto-generated read/write accessor functions for schan registers */
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_timeout_read,                  cmicx->sbus_timeout_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_timeout_write,                 cmicx->sbus_timeout_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_0_7_read,             cmicx->sbus_ring_map_0_7_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_0_7_write,            cmicx->sbus_ring_map_0_7_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_8_15_read,            cmicx->sbus_ring_map_8_15_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_8_15_write,           cmicx->sbus_ring_map_8_15_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_16_23_read,           cmicx->sbus_ring_map_16_23_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_16_23_write,          cmicx->sbus_ring_map_16_23_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_24_31_read,           cmicx->sbus_ring_map_24_31_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_24_31_write,          cmicx->sbus_ring_map_24_31_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_32_39_read,           cmicx->sbus_ring_map_32_39_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_32_39_write,          cmicx->sbus_ring_map_32_39_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_40_47_read,           cmicx->sbus_ring_map_40_47_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_40_47_write,          cmicx->sbus_ring_map_40_47_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_48_55_read,           cmicx->sbus_ring_map_48_55_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_48_55_write,          cmicx->sbus_ring_map_48_55_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_56_63_read,           cmicx->sbus_ring_map_56_63_reg)
PCID_REG_WRITE_FUNC_GEN(cmic_top_sbus_ring_map_56_63_write,          cmicx->sbus_ring_map_56_63_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_64_71_read,           cmicx->sbus_ring_map_64_71_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_64_71_write,         cmicx->sbus_ring_map_64_71_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_72_79_read,           cmicx->sbus_ring_map_72_79_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_72_79_write,         cmicx->sbus_ring_map_72_79_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_80_87_read,           cmicx->sbus_ring_map_80_87_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_80_87_write,         cmicx->sbus_ring_map_80_87_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_88_95_read,           cmicx->sbus_ring_map_88_95_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_88_95_write,         cmicx->sbus_ring_map_88_95_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_96_103_read,          cmicx->sbus_ring_map_96_103_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_96_103_write,        cmicx->sbus_ring_map_96_103_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_104_111_read,         cmicx->sbus_ring_map_104_111_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_104_111_write,       cmicx->sbus_ring_map_104_111_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_112_119_read,         cmicx->sbus_ring_map_112_119_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_112_119_write,       cmicx->sbus_ring_map_112_119_reg)
PCID_REG_READ_FUNC_GEN( cmic_top_sbus_ring_map_120_127_read,         cmicx->sbus_ring_map_120_127_reg)
PCID_REG_WRITE_FUNC_GEN( cmic_top_sbus_ring_map_120_127_write,       cmicx->sbus_ring_map_120_127_reg)
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message0_read,    cmicx->common.schan[0].message_reg[0])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message0_write,   cmicx->common.schan[0].message_reg[0])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message1_read,    cmicx->common.schan[0].message_reg[1])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message1_write,   cmicx->common.schan[0].message_reg[1])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message2_read,    cmicx->common.schan[0].message_reg[2])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message2_write,   cmicx->common.schan[0].message_reg[2])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message3_read,    cmicx->common.schan[0].message_reg[3])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message3_write,   cmicx->common.schan[0].message_reg[3])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message4_read,    cmicx->common.schan[0].message_reg[4])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message4_write,   cmicx->common.schan[0].message_reg[4])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message5_read,    cmicx->common.schan[0].message_reg[5])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message5_write,   cmicx->common.schan[0].message_reg[5])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message6_read,    cmicx->common.schan[0].message_reg[6])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message6_write,   cmicx->common.schan[0].message_reg[6])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message7_read,    cmicx->common.schan[0].message_reg[7])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message7_write,   cmicx->common.schan[0].message_reg[7])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message8_read,    cmicx->common.schan[0].message_reg[8])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message8_write,   cmicx->common.schan[0].message_reg[8])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message9_read,    cmicx->common.schan[0].message_reg[9])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message9_write,   cmicx->common.schan[0].message_reg[9])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message10_read,   cmicx->common.schan[0].message_reg[10])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message10_write,  cmicx->common.schan[0].message_reg[10])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message11_read,   cmicx->common.schan[0].message_reg[11])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message11_write,  cmicx->common.schan[0].message_reg[11])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message12_read,   cmicx->common.schan[0].message_reg[12])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message12_write,  cmicx->common.schan[0].message_reg[12])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message13_read,   cmicx->common.schan[0].message_reg[13])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message13_write,  cmicx->common.schan[0].message_reg[13])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message14_read,   cmicx->common.schan[0].message_reg[14])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message14_write,  cmicx->common.schan[0].message_reg[14])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message15_read,   cmicx->common.schan[0].message_reg[15])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message15_write,  cmicx->common.schan[0].message_reg[15])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message16_read,   cmicx->common.schan[0].message_reg[16])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message16_write,  cmicx->common.schan[0].message_reg[16])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message17_read,   cmicx->common.schan[0].message_reg[17])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message17_write,  cmicx->common.schan[0].message_reg[17])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message18_read,   cmicx->common.schan[0].message_reg[18])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message18_write,  cmicx->common.schan[0].message_reg[18])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message19_read,   cmicx->common.schan[0].message_reg[19])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message19_write,  cmicx->common.schan[0].message_reg[19])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message20_read,   cmicx->common.schan[0].message_reg[20])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message20_write,  cmicx->common.schan[0].message_reg[20])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch0_message21_read,   cmicx->common.schan[0].message_reg[21])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch0_message21_write,  cmicx->common.schan[0].message_reg[21])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message0_read,    cmicx->common.schan[1].message_reg[0])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message0_write,   cmicx->common.schan[1].message_reg[0])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message1_read,    cmicx->common.schan[1].message_reg[1])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message1_write,   cmicx->common.schan[1].message_reg[1])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message2_read,    cmicx->common.schan[1].message_reg[2])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message2_write,   cmicx->common.schan[1].message_reg[2])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message3_read,    cmicx->common.schan[1].message_reg[3])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message3_write,   cmicx->common.schan[1].message_reg[3])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message4_read,    cmicx->common.schan[1].message_reg[4])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message4_write,   cmicx->common.schan[1].message_reg[4])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message5_read,    cmicx->common.schan[1].message_reg[5])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message5_write,   cmicx->common.schan[1].message_reg[5])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message6_read,    cmicx->common.schan[1].message_reg[6])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message6_write,   cmicx->common.schan[1].message_reg[6])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message7_read,    cmicx->common.schan[1].message_reg[7])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message7_write,   cmicx->common.schan[1].message_reg[7])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message8_read,    cmicx->common.schan[1].message_reg[8])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message8_write,   cmicx->common.schan[1].message_reg[8])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message9_read,    cmicx->common.schan[1].message_reg[9])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message9_write,   cmicx->common.schan[1].message_reg[9])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message10_read,   cmicx->common.schan[1].message_reg[10])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message10_write,  cmicx->common.schan[1].message_reg[10])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message11_read,   cmicx->common.schan[1].message_reg[11])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message11_write,  cmicx->common.schan[1].message_reg[11])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message12_read,   cmicx->common.schan[1].message_reg[12])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message12_write,  cmicx->common.schan[1].message_reg[12])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message13_read,   cmicx->common.schan[1].message_reg[13])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message13_write,  cmicx->common.schan[1].message_reg[13])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message14_read,   cmicx->common.schan[1].message_reg[14])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message14_write,  cmicx->common.schan[1].message_reg[14])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message15_read,   cmicx->common.schan[1].message_reg[15])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message15_write,  cmicx->common.schan[1].message_reg[15])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message16_read,   cmicx->common.schan[1].message_reg[16])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message16_write,  cmicx->common.schan[1].message_reg[16])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message17_read,   cmicx->common.schan[1].message_reg[17])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message17_write,  cmicx->common.schan[1].message_reg[17])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message18_read,   cmicx->common.schan[1].message_reg[18])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message18_write,  cmicx->common.schan[1].message_reg[18])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message19_read,   cmicx->common.schan[1].message_reg[19])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message19_write,  cmicx->common.schan[1].message_reg[19])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message20_read,   cmicx->common.schan[1].message_reg[20])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message20_write,  cmicx->common.schan[1].message_reg[20])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch1_message21_read,   cmicx->common.schan[1].message_reg[21])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch1_message21_write,  cmicx->common.schan[1].message_reg[21])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message0_read,    cmicx->common.schan[2].message_reg[0])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message0_write,   cmicx->common.schan[2].message_reg[0])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message1_read,    cmicx->common.schan[2].message_reg[1])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message1_write,   cmicx->common.schan[2].message_reg[1])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message2_read,    cmicx->common.schan[2].message_reg[2])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message2_write,   cmicx->common.schan[2].message_reg[2])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message3_read,    cmicx->common.schan[2].message_reg[3])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message3_write,   cmicx->common.schan[2].message_reg[3])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message4_read,    cmicx->common.schan[2].message_reg[4])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message4_write,   cmicx->common.schan[2].message_reg[4])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message5_read,    cmicx->common.schan[2].message_reg[5])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message5_write,   cmicx->common.schan[2].message_reg[5])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message6_read,    cmicx->common.schan[2].message_reg[6])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message6_write,   cmicx->common.schan[2].message_reg[6])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message7_read,    cmicx->common.schan[2].message_reg[7])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message7_write,   cmicx->common.schan[2].message_reg[7])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message8_read,    cmicx->common.schan[2].message_reg[8])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message8_write,   cmicx->common.schan[2].message_reg[8])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message9_read,    cmicx->common.schan[2].message_reg[9])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message9_write,   cmicx->common.schan[2].message_reg[9])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message10_read,   cmicx->common.schan[2].message_reg[10])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message10_write,  cmicx->common.schan[2].message_reg[10])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message11_read,   cmicx->common.schan[2].message_reg[11])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message11_write,  cmicx->common.schan[2].message_reg[11])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message12_read,   cmicx->common.schan[2].message_reg[12])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message12_write,  cmicx->common.schan[2].message_reg[12])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message13_read,   cmicx->common.schan[2].message_reg[13])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message13_write,  cmicx->common.schan[2].message_reg[13])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message14_read,   cmicx->common.schan[2].message_reg[14])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message14_write,  cmicx->common.schan[2].message_reg[14])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message15_read,   cmicx->common.schan[2].message_reg[15])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message15_write,  cmicx->common.schan[2].message_reg[15])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message16_read,   cmicx->common.schan[2].message_reg[16])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message16_write,  cmicx->common.schan[2].message_reg[16])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message17_read,   cmicx->common.schan[2].message_reg[17])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message17_write,  cmicx->common.schan[2].message_reg[17])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message18_read,   cmicx->common.schan[2].message_reg[18])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message18_write,  cmicx->common.schan[2].message_reg[18])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message19_read,   cmicx->common.schan[2].message_reg[19])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message19_write,  cmicx->common.schan[2].message_reg[19])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message20_read,   cmicx->common.schan[2].message_reg[20])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message20_write,  cmicx->common.schan[2].message_reg[20])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch2_message21_read,   cmicx->common.schan[2].message_reg[21])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch2_message21_write,  cmicx->common.schan[2].message_reg[21])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message0_read,    cmicx->common.schan[3].message_reg[0])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message0_write,   cmicx->common.schan[3].message_reg[0])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message1_read,    cmicx->common.schan[3].message_reg[1])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message1_write,   cmicx->common.schan[3].message_reg[1])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message2_read,    cmicx->common.schan[3].message_reg[2])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message2_write,   cmicx->common.schan[3].message_reg[2])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message3_read,    cmicx->common.schan[3].message_reg[3])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message3_write,   cmicx->common.schan[3].message_reg[3])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message4_read,    cmicx->common.schan[3].message_reg[4])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message4_write,   cmicx->common.schan[3].message_reg[4])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message5_read,    cmicx->common.schan[3].message_reg[5])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message5_write,   cmicx->common.schan[3].message_reg[5])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message6_read,    cmicx->common.schan[3].message_reg[6])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message6_write,   cmicx->common.schan[3].message_reg[6])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message7_read,    cmicx->common.schan[3].message_reg[7])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message7_write,   cmicx->common.schan[3].message_reg[7])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message8_read,    cmicx->common.schan[3].message_reg[8])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message8_write,   cmicx->common.schan[3].message_reg[8])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message9_read,    cmicx->common.schan[3].message_reg[9])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message9_write,   cmicx->common.schan[3].message_reg[9])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message10_read,   cmicx->common.schan[3].message_reg[10])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message10_write,  cmicx->common.schan[3].message_reg[10])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message11_read,   cmicx->common.schan[3].message_reg[11])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message11_write,  cmicx->common.schan[3].message_reg[11])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message12_read,   cmicx->common.schan[3].message_reg[12])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message12_write,  cmicx->common.schan[3].message_reg[12])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message13_read,   cmicx->common.schan[3].message_reg[13])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message13_write,  cmicx->common.schan[3].message_reg[13])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message14_read,   cmicx->common.schan[3].message_reg[14])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message14_write,  cmicx->common.schan[3].message_reg[14])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message15_read,   cmicx->common.schan[3].message_reg[15])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message15_write,  cmicx->common.schan[3].message_reg[15])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message16_read,   cmicx->common.schan[3].message_reg[16])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message16_write,  cmicx->common.schan[3].message_reg[16])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message17_read,   cmicx->common.schan[3].message_reg[17])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message17_write,  cmicx->common.schan[3].message_reg[17])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message18_read,   cmicx->common.schan[3].message_reg[18])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message18_write,  cmicx->common.schan[3].message_reg[18])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message19_read,   cmicx->common.schan[3].message_reg[19])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message19_write,  cmicx->common.schan[3].message_reg[19])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message20_read,   cmicx->common.schan[3].message_reg[20])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message20_write,  cmicx->common.schan[3].message_reg[20])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch3_message21_read,   cmicx->common.schan[3].message_reg[21])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch3_message21_write,  cmicx->common.schan[3].message_reg[21])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message0_read,    cmicx->common.schan[4].message_reg[0])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message0_write,   cmicx->common.schan[4].message_reg[0])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message1_read,    cmicx->common.schan[4].message_reg[1])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message1_write,   cmicx->common.schan[4].message_reg[1])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message2_read,    cmicx->common.schan[4].message_reg[2])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message2_write,   cmicx->common.schan[4].message_reg[2])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message3_read,    cmicx->common.schan[4].message_reg[3])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message3_write,   cmicx->common.schan[4].message_reg[3])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message4_read,    cmicx->common.schan[4].message_reg[4])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message4_write,   cmicx->common.schan[4].message_reg[4])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message5_read,    cmicx->common.schan[4].message_reg[5])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message5_write,   cmicx->common.schan[4].message_reg[5])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message6_read,    cmicx->common.schan[4].message_reg[6])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message6_write,   cmicx->common.schan[4].message_reg[6])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message7_read,    cmicx->common.schan[4].message_reg[7])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message7_write,   cmicx->common.schan[4].message_reg[7])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message8_read,    cmicx->common.schan[4].message_reg[8])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message8_write,   cmicx->common.schan[4].message_reg[8])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message9_read,    cmicx->common.schan[4].message_reg[9])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message9_write,   cmicx->common.schan[4].message_reg[9])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message10_read,   cmicx->common.schan[4].message_reg[10])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message10_write,  cmicx->common.schan[4].message_reg[10])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message11_read,   cmicx->common.schan[4].message_reg[11])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message11_write,  cmicx->common.schan[4].message_reg[11])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message12_read,   cmicx->common.schan[4].message_reg[12])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message12_write,  cmicx->common.schan[4].message_reg[12])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message13_read,   cmicx->common.schan[4].message_reg[13])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message13_write,  cmicx->common.schan[4].message_reg[13])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message14_read,   cmicx->common.schan[4].message_reg[14])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message14_write,  cmicx->common.schan[4].message_reg[14])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message15_read,   cmicx->common.schan[4].message_reg[15])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message15_write,  cmicx->common.schan[4].message_reg[15])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message16_read,   cmicx->common.schan[4].message_reg[16])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message16_write,  cmicx->common.schan[4].message_reg[16])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message17_read,   cmicx->common.schan[4].message_reg[17])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message17_write,  cmicx->common.schan[4].message_reg[17])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message18_read,   cmicx->common.schan[4].message_reg[18])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message18_write,  cmicx->common.schan[4].message_reg[18])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message19_read,   cmicx->common.schan[4].message_reg[19])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message19_write,  cmicx->common.schan[4].message_reg[19])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message20_read,   cmicx->common.schan[4].message_reg[20])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message20_write,  cmicx->common.schan[4].message_reg[20])
PCID_REG_READ_FUNC_GEN( cmic_common_pool_schan_ch4_message21_read,   cmicx->common.schan[4].message_reg[21])
PCID_REG_WRITE_FUNC_GEN(cmic_common_pool_schan_ch4_message21_write,  cmicx->common.schan[4].message_reg[21])

/* read/write access functions for schan control regs */
bool cmic_common_pool_schan_ch0_ctrl_read(reg_t *val) {
    return cmic_schan_ctrl_read(val,&cmicx->common.schan[0]);
}
bool cmic_common_pool_schan_ch0_ctrl_write(reg_t val) {
    return cmic_schan_ctrl_write(val,&cmicx->common.schan[0]);
}
bool cmic_common_pool_schan_ch1_ctrl_read(reg_t *val) {
    return cmic_schan_ctrl_read(val,&cmicx->common.schan[1]);
}
bool cmic_common_pool_schan_ch1_ctrl_write(reg_t val) {
    return cmic_schan_ctrl_write(val,&cmicx->common.schan[1]);
}
bool cmic_common_pool_schan_ch2_ctrl_read(reg_t *val) {
    return cmic_schan_ctrl_read(val,&cmicx->common.schan[2]);
}
bool cmic_common_pool_schan_ch2_ctrl_write(reg_t val) {
    return cmic_schan_ctrl_write(val,&cmicx->common.schan[2]);
}
bool cmic_common_pool_schan_ch3_ctrl_read(reg_t *val) {
    return cmic_schan_ctrl_read(val,&cmicx->common.schan[3]);
}
bool cmic_common_pool_schan_ch3_ctrl_write(reg_t val) {
    return cmic_schan_ctrl_write(val,&cmicx->common.schan[3]);
}
bool cmic_common_pool_schan_ch4_ctrl_read(reg_t *val) {
    return cmic_schan_ctrl_read(val,&cmicx->common.schan[4]);
}
bool cmic_common_pool_schan_ch4_ctrl_write(reg_t val) {
    return cmic_schan_ctrl_write(val,&cmicx->common.schan[4]);
}

/* read/write access functions for schan err regs */
bool cmic_common_pool_schan_ch0_err_read(reg_t *val) {
    return cmic_schan_err_read(val,&cmicx->common.schan[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch0_err_write)
bool cmic_common_pool_schan_ch1_err_read(reg_t *val) {
    return cmic_schan_err_read(val,&cmicx->common.schan[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch1_err_write)
bool cmic_common_pool_schan_ch2_err_read(reg_t *val) {
    return cmic_schan_err_read(val,&cmicx->common.schan[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch2_err_write)
bool cmic_common_pool_schan_ch3_err_read(reg_t *val) {
    return cmic_schan_err_read(val,&cmicx->common.schan[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch3_err_write)
bool cmic_common_pool_schan_ch4_err_read(reg_t *val) {
    return cmic_schan_err_read(val,&cmicx->common.schan[4]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch4_err_write)

/* read/write access functions for schan ack data beat count regs */
bool cmic_common_pool_schan_ch0_ack_data_beat_count_read(reg_t *val) {
    return cmic_schan_ack_data_beat_count_read(val,&cmicx->common.schan[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch0_ack_data_beat_count_write)
bool cmic_common_pool_schan_ch1_ack_data_beat_count_read(reg_t *val) {
    return cmic_schan_ack_data_beat_count_read(val,&cmicx->common.schan[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch1_ack_data_beat_count_write)
bool cmic_common_pool_schan_ch2_ack_data_beat_count_read(reg_t *val) {
    return cmic_schan_ack_data_beat_count_read(val,&cmicx->common.schan[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch2_ack_data_beat_count_write)
bool cmic_common_pool_schan_ch3_ack_data_beat_count_read(reg_t *val) {
    return cmic_schan_ack_data_beat_count_read(val,&cmicx->common.schan[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch3_ack_data_beat_count_write)
bool cmic_common_pool_schan_ch4_ack_data_beat_count_read(reg_t *val) {
    return cmic_schan_ack_data_beat_count_read(val,&cmicx->common.schan[4]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_common_pool_schan_ch4_ack_data_beat_count_write)

