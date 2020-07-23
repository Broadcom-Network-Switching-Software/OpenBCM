/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



/*
 * cmicx_sim_sbus_dma.c
 *
 * contains implementation of sbus dma register access functions, as well as helper functions, update functions, and init functions for sbus dma.
 */


/* includes */
#include <string.h>
#ifndef CMICX_STANDALONE_SIM
 #include <../systems/sim/pcid/cmicx_sim.h>
 #include <../systems/sim/pcid/cmicx_sim_pcid.h>
 #include <../systems/sim/pcid/cmicx_sim_schan.h>
 #include <../systems/sim/pcid/cmicx_sim_sbus_dma.h>
#else
 #include "cmicx_sim.h"
 #include "cmicx_sim_pcid.h"
 #include "cmicx_sim_schan.h"
 #include "cmicx_sim_sbus_dma.h"
#endif

/* external cmicx global structure pointer */
extern cmicx_t *cmicx;


/**********************************************************************/
/* sbus dma support functions. update, init, start access, abort, etc */
/**********************************************************************/


/* init function */
bool cmicx_sbus_dma_init(cmicx_sbus_dma_data_t *sbus_dma_data,char *id_string)
{
    /* if the given id string is too long, log an error and quit */
    if ((strlen(id_string)+1)>CMIC_SBUS_DMA_ID_STRING_LENGTH) {
        cmicx_error("ERROR: given id string \"");
        cmicx_error(id_string);
        cmicx_error("\" for sbus dma unit is too long.\n");
        return false;
    }

    /* copy the id string to the storage for the schan */
    strcpy(sbus_dma_data->id_string,id_string);

    /* control register values */
    sbus_dma_data->desc_prefetch_enable    = false;
    sbus_dma_data->endianness              = false;
    sbus_dma_data->mode                    = CMIC_SBUS_DMA_REGISTER_MODE;
    sbus_dma_data->start                   = false;
    sbus_dma_data->command_is_write        = false;

    /* internal control values */
    sbus_dma_data->last_desc               = false;
    sbus_dma_data->timer_running           = false;
    sbus_dma_data->access_running          = false;
    sbus_dma_data->timer_countdown         = 0;
    sbus_dma_data->sequence_pending        = false;
    sbus_dma_data->sbus_command_countdown  = 0;
    sbus_dma_data->pend_clocks_countdown   = 0;

    /* request register values */
    sbus_dma_data->decrement                   = false;
    sbus_dma_data->req_single                  = false;
    sbus_dma_data->incr_noadd                  = false;
    sbus_dma_data->incr_shift                  = 0;
    sbus_dma_data->pend_clocks                 = 0;
    sbus_dma_data->dma_wr_to_nullspace         = false;
    sbus_dma_data->wordswap_in_64bit_sbus_data = false;
    sbus_dma_data->memwr_endianness            = 0;
    sbus_dma_data->memrd_endianness            = 0;
    sbus_dma_data->req_words                   = 0;
    sbus_dma_data->resp_words                  = 0;

    /* count register values */
    sbus_dma_data->cur_count               = 0;

    /* opcode register value */
    sbus_dma_data->opcode                  = 0;

    /* start register address value */
    sbus_dma_data->cur_sbus_address        = 0;

    /* hostmem register address value */
    sbus_dma_data->cur_hostmem_address     = 0;

    /* hostmem descriptor start address value */
    sbus_dma_data->cur_desc_address        = 0;

    /* status bits */
    sbus_dma_data->ecc_2bit_check_fail     = false;
    sbus_dma_data->sbusdma_active          = false;
    sbus_dma_data->active                  = false;
    sbus_dma_data->descrd_error            = false;
    sbus_dma_data->sbusack_timeout         = false;
    sbus_dma_data->sbusack_error           = false;
    sbus_dma_data->sbusack_nack            = false;
    sbus_dma_data->sbusack_wrong_opcode    = false;
    sbus_dma_data->sbusack_wrong_beatcount = false;
    sbus_dma_data->ser_check_fail          = false;
    sbus_dma_data->hostmemrd_error         = false;
    sbus_dma_data->hostmemwr_error         = false;
    sbus_dma_data->error                   = false;
    sbus_dma_data->done                    = false;

    /* debug register values */
    sbus_dma_data->desc_done               = false;

    /* iter count register value */
    sbus_dma_data->iter_count              = 0;

    /* timer register value */
    sbus_dma_data->timer                   = 0;

    /* current values for things (fed to cur registers) */
    sbus_dma_data->cur_sbusdma_config_request               = 0;
    sbus_dma_data->cur_sbusdma_config_count                 = 0;
    sbus_dma_data->cur_sbusdma_config_opcode                = 0;
    sbus_dma_data->cur_sbusdma_config_sbus_start_address    = 0;
    sbus_dma_data->cur_sbusdma_config_hostmem_start_address = 0;
    sbus_dma_data->cur_sbusdma_config_desc_start_address    = 0;

    /* per cmc sbusdma config values */
    sbus_dma_data->enable_sbusdma_flow_control        = false;

    /* successful initialization complete */
    cmicx_log("successfully init'd \"");
    cmicx_log(sbus_dma_data->id_string);
    cmicx_log("\".");
    cmicx_log_newline();
    return true;

}


/* update function for sbus dma */
/*
 * steps for updating an sbus dma:
 * 1) if we have no access running and we are in timer-based mode, then we may need to start a new access. perform a timer countdown and start this access
 *    once the timer reaches 0. if we already have an access running, skip to step 2.
 * 2) once we have an access running, if we are in descriptor mode and not currently running an sbus command sequence, make a memory request for a descriptor
 *    if we need to (and wait until that request comes back). otherwise, skip to step 3.
 * 3) start the internal sbus timer (counter) process, to start processing the next (or first) sbus command of a sequence.
 * 4) decrement the counter until the count expires, at which point execute the sbus command, using the current register data. if descriptor mode was enabled,
 *    the proper data will have been loaded into these registers already.
 * 5) intiate the pend_clocks counter to begin the core clock delay between subsequent sbus dma commands.
 * 6) count down the pend_clocks counter until it expires, at which point repeat this entire process, starting with step 1 if we have ended an sbus dma access,
 *    step 2 if we have only ended an sbus dma sequence, or step 3 if we haven't ended either of these.
 */
bool cmicx_sbus_dma_update(pcid_info_t *pcid_info, cmicx_sbus_dma_data_t *sbus_dma_data)
{

    /* variables */
    reg_t                command[CMIC_SCHAN_NUM_MESSAGE_REGS];
    int                  num_words;
    reg_t                ack_command[CMIC_SCHAN_NUM_MESSAGE_REGS];
    int                  ack_num_words;
    bool                 append                = false;
    bool                 skip                  = false;
    bool                 jump                  = false;
    /* int                  descriptors_remaining = 0; */
    reg_t                mem[CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR];   /* return memory from pcid (used for desc requests) */
    addr_t               temp_address;
    reg_t                temp;
    int                  i;

    /* print log messages for this schan */
    cmicx_log_level_set(0);
    cmicx_log("updating \"");
    cmicx_log(sbus_dma_data->id_string);
    cmicx_log("\"... ");
    cmicx_log_level_set(3);


    /* if we don't have an access running, then check if we are in timer-based mode. if we are, then perform a countdown to initiate an access. */
    /* if we are not in timer-based bode, then we just bail out of the function. */
    if (sbus_dma_data->access_running==false) {

        if (sbus_dma_data->timer_running==true) {

            /* countdown to next access start */
            if (sbus_dma_data->timer_countdown!=0) {
                sbus_dma_data->timer_countdown--;
            }
            if (sbus_dma_data->timer_countdown==0) {

                /* we have reached the end of the countdown, so now we can start a new access. */
                /* if in register mode, restore the original register settings, and start an access. */
                if (sbus_dma_data->mode==CMIC_SBUS_DMA_REGISTER_MODE) {
                    cmic_sbusdma_request_write(                                          sbus_dma_data->cur_sbusdma_config_request,               sbus_dma_data);
                    cmic_sbusdma_count_write(                                            sbus_dma_data->cur_sbusdma_config_count,                 sbus_dma_data);
                    cmic_sbusdma_opcode_write(                                           sbus_dma_data->cur_sbusdma_config_opcode,                sbus_dma_data);
                    cmic_sbusdma_sbus_start_address_write(                               sbus_dma_data->cur_sbusdma_config_sbus_start_address,    sbus_dma_data);
                    cmic_sbusdma_hostmem_start_address_lo_write(UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_hostmem_start_address),sbus_dma_data);
                    cmic_sbusdma_hostmem_start_address_hi_write(UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_hostmem_start_address),sbus_dma_data);
                    if (cmic_sbus_dma_access_start(sbus_dma_data)==false) {
                        return false;
                    }
                }

                /* if in descriptor mode, restore the original descriptor address, and start an access. */
                else {
                    cmic_sbusdma_desc_start_address_lo_write(UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_desc_start_address),sbus_dma_data);
                    cmic_sbusdma_desc_start_address_hi_write(UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_desc_start_address),sbus_dma_data);
                    if (cmic_sbus_dma_access_start(sbus_dma_data)==false) {
                        return false;
                    }
                }

            }

        }

    }

    /* if we have an access running, then handle descriptor requesting, counter setup/decrement and command completion if necessary. */
    else {


        /* if we are in descriptor mode and we do not have an sbus dma sequence pending, then */
        /* we need to initiate a memory read at this point, to get the new descriptors. */
        if ((sbus_dma_data->mode==CMIC_SBUS_DMA_DESCRIPTOR_MODE) && (sbus_dma_data->sequence_pending==false)) {

            /* we need to loop the descriptor request, because we may need to skip or jump to other descriptors, necessitating a re-request */
            do {

                /* make the memory read over the pcid to get the descriptor words */
                cmicx_pcid_mem_read(pcid_info,sbus_dma_data->cur_desc_address,(void *)mem,CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR*CMICX_BYTES_PER_WORD);
                cmicx_log("descriptor read from memory, words: ");
                for (i=0; i<CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR; i++) {
                    cmicx_log_hex(mem[i]);
                    cmicx_log(", ");
                }
                cmicx_log_newline();

                /* parse the special descriptor control word */
                sbus_dma_data->last_desc             = REG_BIT_GET(  mem[0],CMIC_SBUSDMA_CONTROL_DESC_LAST_BIT);
                               append                = REG_BIT_GET(  mem[0],CMIC_SBUSDMA_CONTROL_DESC_APPEND_BIT);
                               skip                  = REG_BIT_GET(  mem[0],CMIC_SBUSDMA_CONTROL_DESC_SKIP_BIT);
                               jump                  = REG_BIT_GET(  mem[0],CMIC_SBUSDMA_CONTROL_DESC_JUMP_BIT);
                               /* descriptors_remaining = REG_FIELD_GET(mem[0],CMIC_SBUSDMA_CONTROL_DESC_DESCRIPTORS_REMAINING_MSB,CMIC_SBUSDMA_CONTROL_DESC_DESCRIPTORS_REMAINING_LSB); */
                               /* ^^^ this prefetch parameter is not actually used at the moment, but may be used in the future */

                /* load all the descriptors into the appropriate config registers */
                /* use the existing PCID reg write helper functions to save some code */
                /* if we are in little endian, then swap the descriptors' endianness */
                if (sbus_dma_data->endianness) {
                    cmic_sbusdma_request_write(            cmicx_reverse_word_endianness(mem[1]),sbus_dma_data);
                    cmic_sbusdma_count_write(              cmicx_reverse_word_endianness(mem[2]),sbus_dma_data);
                    cmic_sbusdma_opcode_write(             cmicx_reverse_word_endianness(mem[3]),sbus_dma_data);
                    cmic_sbusdma_sbus_start_address_write( cmicx_reverse_word_endianness(mem[4]),sbus_dma_data);
                    if (append==false) {
                        cmic_sbusdma_hostmem_start_address_lo_write(cmicx_reverse_word_endianness(mem[5]),sbus_dma_data);
                        cmic_sbusdma_hostmem_start_address_hi_write(cmicx_reverse_word_endianness(mem[6]),sbus_dma_data);
                    }
                }
                else {
                    cmic_sbusdma_request_write(            mem[1],sbus_dma_data);
                    cmic_sbusdma_count_write(              mem[2],sbus_dma_data);
                    cmic_sbusdma_opcode_write(             mem[3],sbus_dma_data);
                    cmic_sbusdma_sbus_start_address_write( mem[4],sbus_dma_data);
                    if (append==false) {
                        cmic_sbusdma_hostmem_start_address_lo_write(mem[5],sbus_dma_data);
                        cmic_sbusdma_hostmem_start_address_hi_write(mem[6],sbus_dma_data);
                    }
                }

                /* update the descriptor address appropriately, including a skip/jump if needed. */
                if (sbus_dma_data->last_desc==false) {
                    if (jump==true) {
                        sbus_dma_data->cur_desc_address = sbus_dma_data->cur_hostmem_address;
                    }
                    if (skip==true) {
                        sbus_dma_data->cur_desc_address += (CMIC_SBUS_DMA_BYTES_PER_WORD*CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR);    /* bytes per word (4) times number of descriptors (8) */
                    }
                }

            }
            while (((skip==true) || (jump==true)) && (sbus_dma_data->last_desc==false));

            /* at this point we definitely have a valid descriptor, so mark the sbusdma_active bit to indicate that we have started a new descriptor. */
            sbus_dma_data->sbusdma_active = true;

        }


        /* if we are not in descriptor mode, then we need to pretend that this is the last descriptor */
        /* in a descriptor chain, so that the sbus command handling code can act appropriately */
        if (sbus_dma_data->mode==CMIC_SBUS_DMA_REGISTER_MODE) {
            sbus_dma_data->last_desc = true;
        }


        /* at this point, we definitely have valid data in the registers - we've either loaded a descriptor, or we're just using */
        /* the already existing written register data. thus, we can start sending sbus commands, if we haven't already.          */
        /* if we are not currently sending anything, then start a new command, and set the sbus dma sequence flag if we need to. */
        if ((sbus_dma_data->sbus_command_countdown==0) && (sbus_dma_data->pend_clocks_countdown==0)) {

            /* if we are starting a sequence, then we need to make sure that we have flagged that we are pending an sbus dma sequence now */
            if (sbus_dma_data->sequence_pending==false) {
                sbus_dma_data->sequence_pending = true;
            }

            /* begin a new sbus command access if possible, based on the sbus command (first word/"opcode") */
            if (cmic_schan_sbus_command_start(sbus_dma_data->opcode,&sbus_dma_data->command_is_write)==true) {

                /* set variables for successfully beginning sbus command, and print sbus-dma-specific log messages */
                sbus_dma_data->sbus_command_countdown = CMIC_SBUS_DMA_ACCESS_COUNTDOWN_VALUE;
                sbus_dma_data->pend_clocks_countdown  = sbus_dma_data->pend_clocks/50;  
                                               /* add scaling factor here ^^^, because
                                                  simulating one extra update for every
                                                  clock tick is excessive. */
                cmicx_log("command was part of sbus dma sequence on \"");
                cmicx_log(sbus_dma_data->id_string);
                cmicx_log("\". num of mem/data words with command: ");
                cmicx_log_int(sbus_dma_data->req_words);
                cmicx_log(". sbus address: ");
                cmicx_log_addr(sbus_dma_data->cur_sbus_address);
                cmicx_log_newline();

            }
            else {
                cmicx_log("ERROR: could not successfully start sbus command for sbus dma sequence.\n");
                return false;
            }

        }


        /* at this point we are just updating sbus commands that we have pending. decrement the command countdown if we can, */
        /* until it reaches 0. once it reaches 0, then we can finish an sbus command (or do the pending clocks countdown) */
        if (sbus_dma_data->sbus_command_countdown!=0) {
            sbus_dma_data->sbus_command_countdown--;
        }
        if (sbus_dma_data->sbus_command_countdown==0) {

            /* if the pending clocks countdown hasn't reached 0 yet, then decrement it (until it eventually reaches zero). */
            /* once the pending clocks countdown reaches zero, then we can complete the sbus command. */
            if (sbus_dma_data->pend_clocks_countdown!=0) {
                sbus_dma_data->pend_clocks_countdown--;
            }
            if (sbus_dma_data->pend_clocks_countdown==0) {

                /* prepare for completing the sbus command. if we have a write command, we need to pull data from memory. the amount to pull is specified by */
                /* req_words, and should match the value given in the sbus command opcode word. */
                temp_address = sbus_dma_data->cur_hostmem_address;
                command[0]   = sbus_dma_data->opcode;
                command[1]   = sbus_dma_data->cur_sbus_address;
                if (sbus_dma_data->command_is_write==true) {
                    if (sbus_dma_data->req_words!=(CMIC_SCHAN_COMMAND_STRIP_DLEN(sbus_dma_data->opcode)/CMICX_BYTES_PER_WORD)) {
                        cmicx_log("ERROR: req_words supplied on sbus dma \"");
                        cmicx_log(sbus_dma_data->id_string);
                        cmicx_log("\" is not congruent with dlen value in opcode supplied. req_words: ");
                        cmicx_log_int(sbus_dma_data->req_words);
                        cmicx_log(" dlen value: ");
                        cmicx_log_int(CMIC_SCHAN_COMMAND_STRIP_DLEN(sbus_dma_data->opcode)/CMICX_BYTES_PER_WORD);
                        cmicx_log(" sbus opcode: ");
                        cmicx_log_reg(sbus_dma_data->opcode);
                        cmicx_log_newline();
                        return false;
                    }
                    cmicx_pcid_mem_read(pcid_info,temp_address,(void *)&(command[2]),(sbus_dma_data->req_words*CMICX_BYTES_PER_WORD));
                    temp_address += (sbus_dma_data->req_words*CMICX_BYTES_PER_WORD);
                }

                /* try to complete the sbus command if we can, since we have already loaded the command opcode word, an address, and any data words to the command[] memory. */
                if (cmic_schan_sbus_command_complete(pcid_info, command,ack_command,((sbus_dma_data->command_is_write==false) ? sbus_dma_data->resp_words : 0),&num_words)==true) {

                    if (sbus_dma_data->command_is_write==false) {

                        /* check that the expected number of response data words matches the actual number from the ack (otherwise we have an error). */
                        ack_num_words = (CMIC_SCHAN_COMMAND_STRIP_DLEN(ack_command[0])/CMICX_BYTES_PER_WORD)+1;
                        if (sbus_dma_data->resp_words!=(ack_num_words-1)) {
                            cmicx_log("ERROR: number of data words recieved from sbus command ack does not match response expected from sbus dma settings. ack num words: ");
                            cmicx_log_int(ack_num_words);
                            cmicx_log(". sbus address: ");
                            cmicx_log_addr(command[1]);
                            cmicx_log_newline();
                            return false;
                        }

                        /* if we have 64-bits of data and the wordswap_in_64bit_sbusdata flag is enabled (from the request register), then we need to swap these two words before using them */
                        if ((ack_num_words==3) && (sbus_dma_data->wordswap_in_64bit_sbus_data==true)) {
                            temp = ack_command[1];
                            ack_command[1] = ack_command[2];
                            ack_command[2] = temp;
                        }

                        /* load the ack sbus command data words ONLY into host memory (if any exist), while incrementing the host memory pointer */
                        if (sbus_dma_data->dma_wr_to_nullspace==false) {
                            cmicx_pcid_mem_write(pcid_info,temp_address,(void *)&(ack_command[1]),(ack_num_words-1)*CMICX_BYTES_PER_WORD);
                            temp_address += (ack_num_words-1)*CMIC_SBUS_DMA_BYTES_PER_WORD;
                        }
                        cmicx_log(" wrote ack data words to host memory during sbus dma transaction on \"");
                        cmicx_log(sbus_dma_data->id_string);
                        cmicx_log("\". num words: ");
                        cmicx_log_int(ack_num_words-1);
                        cmicx_log("... ");

                    }
                    else {
                        cmicx_log(" sbus write command complete on \"");
                        cmicx_log(sbus_dma_data->id_string);
                        cmicx_log("\", not writing any words to memory... ");
                    }

                }
                else {
                    cmicx_log("ERROR: could not complete sbus command during sbus dma sequence. sbus address: ");
                    cmicx_log_addr(command[1]);
                    cmicx_log_newline();
                    return false;
                }


                /* decrement our command count, since we've finished a command. if we are then not at the last command, increment the sbus address register */
                /* to set up for the next sbus command to be dispatched, and assign the new current hostmem address value */
                sbus_dma_data->cur_count--;
                cmicx_log("cur_count: ");
                cmicx_log_int(sbus_dma_data->cur_count);
                cmicx_log(" ");
                cmicx_log_newline();
                if (sbus_dma_data->cur_count!=0) {

                    /* if we are supposed to increment/decrement the address, perform the increment/decrement */
                    if (sbus_dma_data->incr_noadd==false) {
                        if (sbus_dma_data->decrement==true) {
                            sbus_dma_data->cur_sbus_address -= (1<<sbus_dma_data->incr_shift);
                        }
                        else {
                            sbus_dma_data->cur_sbus_address += (1<<sbus_dma_data->incr_shift);
                        }
                    }

                    /* make sure the current hostmem address is up-to-date for the next sbus command. */
                    if (sbus_dma_data->req_single == false) {
                        sbus_dma_data->cur_hostmem_address = temp_address;
                    }
                }

                /* if we are done with the last command in the sequence, then we need to determine what to do. either end the sbus dma access altogether, */
                /* or just end the descriptor and move on to the next. */
                else {

                    /* the sbus dma sequence is done, no matter what mode we're in. but if we're in descriptor mode, then also toggle the discriptor notification bits. */
                    sbus_dma_data->sequence_pending = false;
                    if (sbus_dma_data->mode==CMIC_SBUS_DMA_DESCRIPTOR_MODE) {
                        sbus_dma_data->desc_done        = true;
                        sbus_dma_data->sbusdma_active   = false;
                    }

                    /* if this was the last desc in the sbus dma access, then mark the sbus dma access done altogether */
                    if (sbus_dma_data->last_desc) {

                        /* if timer mode is enabled, then we need to prepare for another round of the sbus dma, with the originally programmed settings. set the timer countdown. */
                        /* note 'done' is not marked, because sbus dma will never complete without abort when in timer based mode. */
                        if (sbus_dma_data->timer_running==true) {
                            sbus_dma_data->access_running  = false;
                            sbus_dma_data->timer_countdown = sbus_dma_data->timer;
                            cmicx_log("ending sbus dma access on \"");
                            cmicx_log(sbus_dma_data->id_string);
                            cmicx_log("\", but timer mode is on. a new one will start soon.");
                            cmicx_log_newline();
                        }

                        /* if timer mode is not enabled, then we are entirely done with the sbus dma */
                        else {
                            sbus_dma_data->access_running = false;
                            sbus_dma_data->done           = true;
                            cmicx_log("ending sbus dma access on \"");
                            cmicx_log(sbus_dma_data->id_string);
                            cmicx_log("\"");
                            cmicx_log_newline();
                        }
                    }

                    /* not ending the sbus dma access, but done with the descriptor. set the next current descriptor address */
                    else {
                        sbus_dma_data->cur_desc_address += (CMIC_SBUS_DMA_BYTES_PER_WORD*CMIC_SBUS_DMA_WORDS_PER_DESCRIPTOR);    /* bytes per word (4) times number of descriptors (8) */
                    }

                }

                /* increment the iteration count for the iter_count register, since we've completed another sbus command in the sequence */
                sbus_dma_data->iter_count++;

            }

        }


    }

    /* end update successfully */
    cmicx_log_level_set(0);
    cmicx_log(" ...done.");
    cmicx_log_newline();
    cmicx_log_level_set(3);
    return true;

}


/* starts a new sbus dma access for a given sbus dma module */
bool cmic_sbus_dma_access_start(cmicx_sbus_dma_data_t *sbus_dma_data) {

    /* start logging */
    cmicx_log("attempting to start new sbus dma access on \"");
    cmicx_log(sbus_dma_data->id_string);
    cmicx_log("\"... ");

    /* already running failure */
    if (sbus_dma_data->access_running==true) {
        cmicx_log(" failure: access already running.\n");
        return false;
    }

    /* set variables for successfully beginning sbus dma access */
    sbus_dma_data->access_running         = true;
    sbus_dma_data->start                  = true;
    sbus_dma_data->iter_count             = 0;
    sbus_dma_data->last_desc              = false;
    sbus_dma_data->sequence_pending       = false;
    sbus_dma_data->sbus_command_countdown = 0;
    sbus_dma_data->pend_clocks_countdown  = 0;
    sbus_dma_data->active                 = true;

    /* if we are in timer-based mode, we need to make sure timer_running is high, so that */
    /* we continue to loop sbus dma accesses after this one completes. */
    if (sbus_dma_data->timer!=0) {
        sbus_dma_data->timer_running = true;
    }

    /* sbus dma access successfully started */
    cmicx_log(" successfully started sbus dma access.");
    cmicx_log_newline();
    return true;
}


/* aborts an sbus dma access for a given sbus dma module */
bool cmic_sbus_dma_access_abort(cmicx_sbus_dma_data_t *sbus_dma_data) {

    /* if we have an access running or we are in timer-based mode, abort the sbus dma activity and log a message. */
    /* otherwise, just log a message that we could not abort. */
    if ((sbus_dma_data->access_running==true) || (sbus_dma_data->timer!=0)) {
        sbus_dma_data->access_running         = false;
        sbus_dma_data->last_desc              = false;
        sbus_dma_data->timer_running          = false;
        sbus_dma_data->timer_countdown        = 0;
        sbus_dma_data->sequence_pending       = false;
        sbus_dma_data->sbus_command_countdown = 0;
        sbus_dma_data->pend_clocks_countdown  = 0;
        sbus_dma_data->done                   = true;
        cmicx_log("sbus dma access on \"");
        cmicx_log(sbus_dma_data->id_string);
        cmicx_log("\" successfully aborted.");
        cmicx_log_newline();
        return true;
    }
    else {
        cmicx_log("could not abort sbus dma access on \"");
        cmicx_log(sbus_dma_data->id_string);
        cmicx_log("\", no access currently pending.\n");
        return false;
    }

}



/**********************************************/
/* sbus dma register access support functions */
/**********************************************/


/* control register read support function */
bool cmic_sbusdma_control_read(reg_t *val,cmicx_sbus_dma_data_t *sbus_dma_data) {

    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONTROL_REG_DESC_PREFETCH_ENABLE_BIT,sbus_dma_data->desc_prefetch_enable);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONTROL_REG_ENDIANNESS_BIT,          sbus_dma_data->endianness          );
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONTROL_REG_MODE_BIT,                sbus_dma_data->mode                );
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONTROL_REG_ABORT_BIT,               false                              );
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONTROL_REG_START_BIT,               sbus_dma_data->start               );
    return true;

}


/* control register write support function */
bool cmic_sbusdma_control_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {

    bool return_value = true;

    /* grab new bits from reg value write */
    bool new_ctrl_desc_prefetch_enable_bit = REG_BIT_GET(val,CMIC_SBUSDMA_CONTROL_REG_DESC_PREFETCH_ENABLE_BIT);
    bool new_ctrl_endianness_bit           = REG_BIT_GET(val,CMIC_SBUSDMA_CONTROL_REG_ENDIANNESS_BIT);
    bool new_ctrl_mode_bit                 = REG_BIT_GET(val,CMIC_SBUSDMA_CONTROL_REG_MODE_BIT);
    bool new_ctrl_abort_bit                = REG_BIT_GET(val,CMIC_SBUSDMA_CONTROL_REG_ABORT_BIT);
    bool new_ctrl_start_bit                = REG_BIT_GET(val,CMIC_SBUSDMA_CONTROL_REG_START_BIT);

    /* check that we are not trying to change the endianness or mode while we already have an access running */
    if (sbus_dma_data->access_running==true) {

        if (new_ctrl_mode_bit!=sbus_dma_data->mode) {
            cmicx_log("attempting to change sbus dma mode on ");
            cmicx_log(sbus_dma_data->id_string);
            cmicx_log(" while sbus dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_endianness_bit!=sbus_dma_data->endianness) {
            cmicx_log("attempting to change sbus dma endianness on ");
            cmicx_log(sbus_dma_data->id_string);
            cmicx_log(" while sbus dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_desc_prefetch_enable_bit!=sbus_dma_data->desc_prefetch_enable) {
            cmicx_log("attempting to change sbus dma desc_prefetch enable mode on ");
            cmicx_log(sbus_dma_data->id_string);
            cmicx_log(" while sbus dma is running. failure.\n");
            return_value = false;
        }

    }
    else {
        sbus_dma_data->desc_prefetch_enable = new_ctrl_desc_prefetch_enable_bit;
        sbus_dma_data->endianness           = new_ctrl_abort_bit;
        sbus_dma_data->mode                 = new_ctrl_mode_bit;
    }

    /* start a new access or abort if requested */
    if (new_ctrl_abort_bit==true) {
        if (cmic_sbus_dma_access_abort(sbus_dma_data)==false) {
            return_value = false;
        }
        if (new_ctrl_start_bit==true) {
            cmicx_log("ctrl start bit being ignored on \"");
            cmicx_log(sbus_dma_data->id_string);
            cmicx_log("\" due to simultaneous abort attempt.");
            cmicx_log_newline();
        }
    }
    else {
        if (new_ctrl_start_bit==true) {
            if (sbus_dma_data->access_running==false) {
                if (cmic_sbus_dma_access_start(sbus_dma_data)==false) {
                    return_value = false;
                }
            }
            else {
                cmicx_log("ctrl start bit being ignored on \"");
                cmicx_log(sbus_dma_data->id_string);
                cmicx_log("\" due to access already running.");
                cmicx_log_newline();
            }
        }
    }

    /* if we are trying to clear the start bit: check that it is legal, and then make all the necessary internal state changes */
    if (new_ctrl_start_bit==false) {

        /* if the done bit is high and no access is running, we can clear the start bit successfully (and most error bits) */
        if ((sbus_dma_data->done==true) && (sbus_dma_data->access_running==false)) {
            sbus_dma_data->start                   = false;
            sbus_dma_data->sbusdma_active          = false;
            sbus_dma_data->active                  = false;
            sbus_dma_data->descrd_error            = false;
            sbus_dma_data->sbusack_timeout         = false;
            sbus_dma_data->sbusack_error           = false;
            sbus_dma_data->sbusack_nack            = false;
            sbus_dma_data->sbusack_wrong_opcode    = false;
            sbus_dma_data->sbusack_wrong_beatcount = false;
            sbus_dma_data->ser_check_fail          = false;
            sbus_dma_data->hostmemrd_error         = false;
            sbus_dma_data->hostmemwr_error         = false;
            sbus_dma_data->error                   = false;
            sbus_dma_data->done                    = false;
        }

        /* invalid case for clearing start bit */
        else {
            cmicx_log("trying to clear start bit for \"");
            cmicx_log(sbus_dma_data->id_string);
            cmicx_log("\": status register done bit is not high, or sbus dma access is currently running. failure.\n");
            return_value = false;
        }

    }

    /*
    cmicx_log("cur hostmem address: \"");
    cmicx_log_
        */

    return return_value;
}



/* status register read support function */
bool cmic_sbusdma_status_read(reg_t *val,cmicx_sbus_dma_data_t *sbus_dma_data) {

    /* pack the current status bits into the register */
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_ECC_2BIT_CHECK_FAIL_BIT,    sbus_dma_data->ecc_2bit_check_fail);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSDMA_ACTIVE_BIT,         sbus_dma_data->sbusdma_active);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_ACTIVE_BIT,                 sbus_dma_data->active);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_DESCRD_ERROR_BIT,           sbus_dma_data->descrd_error);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSACK_TIMEOUT_BIT,        sbus_dma_data->sbusack_timeout);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSACK_ERROR_BIT,          sbus_dma_data->sbusack_error);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSACK_NACK_BIT,           sbus_dma_data->sbusack_nack);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSACK_WRONG_OPCODE_BIT,   sbus_dma_data->sbusack_wrong_opcode);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SBUSACK_WRONG_BEATCOUNT_BIT,sbus_dma_data->sbusack_wrong_beatcount);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_SER_CHECK_FAIL_BIT,         sbus_dma_data->ser_check_fail);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_HOSTMEMRD_ERROR_BIT,        sbus_dma_data->hostmemrd_error);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_HOSTMEMWR_ERROR_BIT,        sbus_dma_data->hostmemwr_error);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_ERROR_BIT,                  sbus_dma_data->error);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_STATUS_REG_DONE_BIT,                   sbus_dma_data->done);
    return true;

}

/* request register read helper function */
bool cmic_sbusdma_request_read(reg_t *val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_DECR_BIT,                                                sbus_dma_data->decrement);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_REQ_SINGLE_BIT,                                          sbus_dma_data->req_single);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT,                                          sbus_dma_data->incr_noadd);
    REG_FIELD_SET_TO(*val,CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_MSB, CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_LSB, sbus_dma_data->incr_shift);
    REG_FIELD_SET_TO(*val,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_MSB,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_LSB,sbus_dma_data->pend_clocks);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_DMA_WR_TO_NULLSPACE_BIT,                                 sbus_dma_data->dma_wr_to_nullspace);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_WORDSWAP_IN_64BIT_SBUSDATA_BIT,                          sbus_dma_data->wordswap_in_64bit_sbus_data);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_MEMWR_ENDIANNESS_BIT,                                    sbus_dma_data->memwr_endianness);
    REG_BIT_SET_TO(  *val,CMIC_SBUSDMA_REQUEST_REG_MEMRD_ENDIANNESS_BIT,                                    sbus_dma_data->memrd_endianness);
    REG_FIELD_SET_TO(*val,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_LSB,    sbus_dma_data->req_words);
    REG_FIELD_SET_TO(*val,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_LSB,    sbus_dma_data->resp_words);
    return true;
}

/* request register write helper function */
bool cmic_sbusdma_request_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    sbus_dma_data->decrement                   = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_DECR_BIT);
    sbus_dma_data->req_single                  = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_REQ_SINGLE_BIT);
    sbus_dma_data->incr_noadd                  = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT);
    sbus_dma_data->incr_shift                  = REG_FIELD_GET(val,CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_MSB, CMIC_SBUSDMA_REQUEST_REG_INCR_SHIFT_LSB);
    sbus_dma_data->pend_clocks                 = REG_FIELD_GET(val,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_MSB,CMIC_SBUSDMA_REQUEST_REG_PEND_CLOCKS_LSB);
    sbus_dma_data->dma_wr_to_nullspace         = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_DMA_WR_TO_NULLSPACE_BIT);
    sbus_dma_data->wordswap_in_64bit_sbus_data = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_WORDSWAP_IN_64BIT_SBUSDATA_BIT);
    sbus_dma_data->memwr_endianness            = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_INCR_NOADD_BIT);
    sbus_dma_data->memrd_endianness            = REG_BIT_GET(  val,CMIC_SBUSDMA_REQUEST_REG_MEMWR_ENDIANNESS_BIT);
    sbus_dma_data->req_words                   = REG_FIELD_GET(val,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REQ_WORDS_LSB);
    sbus_dma_data->resp_words                  = REG_FIELD_GET(val,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_MSB,CMIC_SBUSDMA_REQUEST_REG_REP_WORDS_LSB);
    sbus_dma_data->cur_sbusdma_config_request  = val;
    return true;
}

/* count register write helper function */
bool cmic_sbusdma_count_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    sbus_dma_data->cur_count                 = val;
    sbus_dma_data->cur_sbusdma_config_count  = val;
    return true;
}

/* opcode register write helper function */
bool cmic_sbusdma_opcode_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    sbus_dma_data->opcode                    = val;
    sbus_dma_data->cur_sbusdma_config_opcode = val;
    return true;
}

/* sbus start address write helper function */
bool cmic_sbusdma_sbus_start_address_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    sbus_dma_data->cur_sbus_address                      = val;
    sbus_dma_data->cur_sbusdma_config_sbus_start_address = val;
    return true;
}

/* hostmem start address write helper function - lo */
bool cmic_sbusdma_hostmem_start_address_lo_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_hostmem_address)                      = val;
    UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_hostmem_start_address) = val;
    return true;
}

/* hostmem start address write helper function - hi */
bool cmic_sbusdma_hostmem_start_address_hi_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_hostmem_address)                      = val;
    UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_hostmem_start_address) = val;
    return true;
}

/* desc start address write helper function - lo */
bool cmic_sbusdma_desc_start_address_lo_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_desc_address)                      = val;
    UINT64_LOWER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_desc_start_address) = val;
    return true;
}

/* desc start address write helper function - hi */
bool cmic_sbusdma_desc_start_address_hi_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {
    UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_desc_address)                      = val;
    UINT64_UPPER32_REFERENCE(sbus_dma_data->cur_sbusdma_config_desc_start_address) = val;
    return true;
}

/* cmc config register read helper function */
bool cmic_sbusdma_shared_config_read(reg_t *val,cmicx_cmc_t *cmc_data) {
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_EARLY_BRESP_BIT,              cmc_data->enable_early_bresp);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH0_FLOW_CONTROL_BIT, cmc_data->sbus_dma[0].enable_sbusdma_flow_control);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH1_FLOW_CONTROL_BIT, cmc_data->sbus_dma[1].enable_sbusdma_flow_control);
    REG_BIT_SET_TO(*val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH2_FLOW_CONTROL_BIT, cmc_data->sbus_dma[2].enable_sbusdma_flow_control);
    return true;
}

/* cmc config register write helper function */
bool cmic_sbusdma_shared_config_write(reg_t val,cmicx_cmc_t *cmc_data) {

    /* get new values */
    bool new_sbusdma_early_bresp      = REG_BIT_GET(val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_EARLY_BRESP_BIT);
    bool new_sbusdma_flow_control_ch0 = REG_BIT_GET(val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH0_FLOW_CONTROL_BIT);
    bool new_sbusdma_flow_control_ch1 = REG_BIT_GET(val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH1_FLOW_CONTROL_BIT);
    bool new_sbusdma_flow_control_ch2 = REG_BIT_GET(val,CMIC_SBUSDMA_CONFIG_REG_ENABLE_SBUSDMA_CH2_FLOW_CONTROL_BIT);

    /* display outputs for flow controls being enabled, because this won't actually have any effect in the sim */
    if (new_sbusdma_early_bresp==true) {
        cmicx_log("enabling early bresp on \"");
    }
    else {
        cmicx_log("disabling early bresp on \"");
    }
    cmicx_log(cmc_data->id_string);
    cmicx_log("\"");
    cmicx_log_newline();
    if (new_sbusdma_flow_control_ch0==true) {
        cmicx_log("enabling flow control on \"");
    }
    else {
        cmicx_log("disabling flow control on \"");
    }
    cmicx_log(cmc_data->sbus_dma[0].id_string);
    cmicx_log("\"");
    cmicx_log_newline();
    if (new_sbusdma_flow_control_ch1==true) {
        cmicx_log("enabling flow control on \"");
    }
    else {
        cmicx_log("disabling flow control on \"");
    }
    cmicx_log(cmc_data->sbus_dma[1].id_string);
    cmicx_log("\"");
    cmicx_log_newline();
    if (new_sbusdma_flow_control_ch2==true) {
        cmicx_log("enabling flow control on \"");
    }
    else {
        cmicx_log("disabling flow control on \"");
    }
    cmicx_log(cmc_data->sbus_dma[2].id_string);
    cmicx_log("\"");
    cmicx_log_newline();

    /* set actual internal control values and exit */
    cmc_data->enable_early_bresp                      = new_sbusdma_early_bresp;
    cmc_data->sbus_dma[0].enable_sbusdma_flow_control = new_sbusdma_flow_control_ch0;
    cmc_data->sbus_dma[1].enable_sbusdma_flow_control = new_sbusdma_flow_control_ch1;
    cmc_data->sbus_dma[2].enable_sbusdma_flow_control = new_sbusdma_flow_control_ch2;
    return true;
}

/* sbus dma debug register read */
bool cmic_sbusdma_debug_read(reg_t *val,cmicx_sbus_dma_data_t *sbus_dma_data) {

   REG_BIT_SET_TO(*val,CMIC_SBUSDMA_DEBUG_REG_DESC_DONE_BIT, sbus_dma_data->desc_done);
   return true;
}

/* sbus dma debug clear register write */
bool cmic_sbusdma_debug_clr_write(reg_t val,cmicx_sbus_dma_data_t *sbus_dma_data) {

   bool new_ecc_2bit_check_fail = REG_BIT_GET(val,CMIC_SBUSDMA_DEBUG_REG_ECC_2BIT_CHECK_FAIL_BIT);
   bool new_desc_done           = REG_BIT_GET(val,CMIC_SBUSDMA_DEBUG_REG_DESC_DONE_BIT);

   if (new_ecc_2bit_check_fail==true) {
       sbus_dma_data->ecc_2bit_check_fail = false;
   }
   if (new_desc_done==true) {
       sbus_dma_data->desc_done = false;
   }

   return true;
}



/****************************************************/
/* define sbus dma register direct access functions */
/****************************************************/

/* sbus ring arbiter control read/write */
bool cmic_top_sbus_ring_arb_ctrl_sbusdma_read(reg_t *val) {
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[0]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[1]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[2]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[0]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[1]);
    REG_FIELD_SET_TO(*val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_LSB,cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[2]);
    return true;
}
bool cmic_top_sbus_ring_arb_ctrl_sbusdma_write(reg_t val) {
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[0] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH0_WEIGHT_LSB);
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[1] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH1_WEIGHT_LSB);
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[2] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC0_CH2_WEIGHT_LSB);
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[0] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH0_WEIGHT_LSB);
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[1] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH1_WEIGHT_LSB);
    cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[2] = REG_FIELD_GET(val,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_MSB,CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_CMC1_CH2_WEIGHT_LSB);
    return true;
}

/* cmc 0 shared config */
bool cmic_cmc0_shared_config_read(reg_t *val) {
    return cmic_sbusdma_shared_config_read(val,&cmicx->cmc0);
}
bool cmic_cmc0_shared_config_write(reg_t val) {
    return cmic_sbusdma_shared_config_write(val,&cmicx->cmc0);
}

/* cmc0, sbus dma ch0 */
bool cmic_cmc0_sbusdma_ch0_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc0.sbus_dma[0]);
}
bool cmic_cmc0_sbusdma_ch0_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc0.sbus_dma[0]);
}
bool cmic_cmc0_sbusdma_ch0_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc0.sbus_dma[0]);
}
bool cmic_cmc0_sbusdma_ch0_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_count_read,                                    cmicx->cmc0.sbus_dma[0].cur_count          )
bool cmic_cmc0_sbusdma_ch0_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_opcode_read,                                   cmicx->cmc0.sbus_dma[0].opcode             )
bool cmic_cmc0_sbusdma_ch0_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_sbus_start_address_read,                       cmicx->cmc0.sbus_dma[0].cur_sbus_address   )
bool cmic_cmc0_sbusdma_ch0_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch0_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch0_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[0]);
}
bool cmic_cmc0_sbusdma_ch0_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbus_address_read,                         cmicx->cmc0.sbus_dma[0].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_read,               cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_read,                 cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_read,                cmicx->cmc0.sbus_dma[0].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_write)
bool cmic_cmc0_sbusdma_ch0_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_read)
bool cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc0.sbus_dma[0]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_iter_count_read,                               cmicx->cmc0.sbus_dma[0].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch0_timer_read,                                    cmicx->cmc0.sbus_dma[0].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc0_sbusdma_ch0_timer_write,                                   cmicx->cmc0.sbus_dma[0].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch0_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch0_tm_control_2_write)

/* cmc0, sbus dma ch1 */
bool cmic_cmc0_sbusdma_ch1_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc0.sbus_dma[1]);
}
bool cmic_cmc0_sbusdma_ch1_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc0.sbus_dma[1]);
}
bool cmic_cmc0_sbusdma_ch1_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc0.sbus_dma[1]);
}
bool cmic_cmc0_sbusdma_ch1_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_count_read,                                    cmicx->cmc0.sbus_dma[1].cur_count          )
bool cmic_cmc0_sbusdma_ch1_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_opcode_read,                                   cmicx->cmc0.sbus_dma[1].opcode             )
bool cmic_cmc0_sbusdma_ch1_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_sbus_start_address_read,                       cmicx->cmc0.sbus_dma[1].cur_sbus_address   )
bool cmic_cmc0_sbusdma_ch1_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch1_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch1_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[1]);
}
bool cmic_cmc0_sbusdma_ch1_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbus_address_read,                         cmicx->cmc0.sbus_dma[1].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_read,               cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_read,                 cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_read,                cmicx->cmc0.sbus_dma[1].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_write)
bool cmic_cmc0_sbusdma_ch1_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_read)
bool cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc0.sbus_dma[1]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_iter_count_read,                               cmicx->cmc0.sbus_dma[1].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch1_timer_read,                                    cmicx->cmc0.sbus_dma[1].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc0_sbusdma_ch1_timer_write,                                   cmicx->cmc0.sbus_dma[1].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch1_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch1_tm_control_2_write)

/* cmc0, sbus dma ch2 */
bool cmic_cmc0_sbusdma_ch2_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc0.sbus_dma[2]);
}
bool cmic_cmc0_sbusdma_ch2_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc0.sbus_dma[2]);
}
bool cmic_cmc0_sbusdma_ch2_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc0.sbus_dma[2]);
}
bool cmic_cmc0_sbusdma_ch2_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_count_read,                                    cmicx->cmc0.sbus_dma[2].cur_count          )
bool cmic_cmc0_sbusdma_ch2_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_opcode_read,                                   cmicx->cmc0.sbus_dma[2].opcode             )
bool cmic_cmc0_sbusdma_ch2_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_sbus_start_address_read,                       cmicx->cmc0.sbus_dma[2].cur_sbus_address   )
bool cmic_cmc0_sbusdma_ch2_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch2_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch2_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[2]);
}
bool cmic_cmc0_sbusdma_ch2_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbus_address_read,                         cmicx->cmc0.sbus_dma[2].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_read,               cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_read,                 cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_read,                cmicx->cmc0.sbus_dma[2].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_write)
bool cmic_cmc0_sbusdma_ch2_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_read)
bool cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc0.sbus_dma[2]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_iter_count_read,                               cmicx->cmc0.sbus_dma[2].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch2_timer_read,                                    cmicx->cmc0.sbus_dma[2].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc0_sbusdma_ch2_timer_write,                                   cmicx->cmc0.sbus_dma[2].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch2_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch2_tm_control_2_write)


/* cmc0, sbus dma ch3 */
bool cmic_cmc0_sbusdma_ch3_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc0.sbus_dma[3]);
}
bool cmic_cmc0_sbusdma_ch3_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc0.sbus_dma[3]);
}
bool cmic_cmc0_sbusdma_ch3_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc0.sbus_dma[3]);
}
bool cmic_cmc0_sbusdma_ch3_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_count_read,                                    cmicx->cmc0.sbus_dma[3].cur_count          )
bool cmic_cmc0_sbusdma_ch3_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_opcode_read,                                   cmicx->cmc0.sbus_dma[3].opcode             )
bool cmic_cmc0_sbusdma_ch3_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_sbus_start_address_read,                       cmicx->cmc0.sbus_dma[3].cur_sbus_address   )
bool cmic_cmc0_sbusdma_ch3_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_hostmem_address))
bool cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch3_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_desc_address)   )
bool cmic_cmc0_sbusdma_ch3_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc0.sbus_dma[3]);
}
bool cmic_cmc0_sbusdma_ch3_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbus_address_read,                         cmicx->cmc0.sbus_dma[3].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_read,               cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_read,                 cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_read,                cmicx->cmc0.sbus_dma[3].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_write)
bool cmic_cmc0_sbusdma_ch3_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_read)
bool cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc0.sbus_dma[3]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_iter_count_read,                               cmicx->cmc0.sbus_dma[3].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc0_sbusdma_ch3_timer_read,                                    cmicx->cmc0.sbus_dma[3].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc0_sbusdma_ch3_timer_write,                                   cmicx->cmc0.sbus_dma[3].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc0_sbusdma_ch3_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc0_sbusdma_ch3_tm_control_2_write)


/* cmc 1 shared config */
bool cmic_cmc1_shared_config_read(reg_t *val) {
    return cmic_sbusdma_shared_config_read(val,&cmicx->cmc1);
}
bool cmic_cmc1_shared_config_write(reg_t val) {
    return cmic_sbusdma_shared_config_write(val,&cmicx->cmc1);
}

/* cmc 1, sbus dma ch0 */
bool cmic_cmc1_sbusdma_ch0_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc1.sbus_dma[0]);
}
bool cmic_cmc1_sbusdma_ch0_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc1.sbus_dma[0]);
}
bool cmic_cmc1_sbusdma_ch0_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc1.sbus_dma[0]);
}
bool cmic_cmc1_sbusdma_ch0_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_count_read,                                    cmicx->cmc1.sbus_dma[0].cur_count          )
bool cmic_cmc1_sbusdma_ch0_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_opcode_read,                                   cmicx->cmc1.sbus_dma[0].opcode             )
bool cmic_cmc1_sbusdma_ch0_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_sbus_start_address_read,                       cmicx->cmc1.sbus_dma[0].cur_sbus_address   )
bool cmic_cmc1_sbusdma_ch0_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch0_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch0_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[0]);
}
bool cmic_cmc1_sbusdma_ch0_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbus_address_read,                         cmicx->cmc1.sbus_dma[0].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_read,               cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_read,                 cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_read,                cmicx->cmc1.sbus_dma[0].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_write)
bool cmic_cmc1_sbusdma_ch0_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_read)
bool cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc1.sbus_dma[0]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_iter_count_read,                               cmicx->cmc1.sbus_dma[0].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch0_timer_read,                                    cmicx->cmc1.sbus_dma[0].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc1_sbusdma_ch0_timer_write,                                   cmicx->cmc1.sbus_dma[0].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch0_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch0_tm_control_2_write)

/* cmc 1, sbus dma ch1 */
bool cmic_cmc1_sbusdma_ch1_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc1.sbus_dma[1]);
}
bool cmic_cmc1_sbusdma_ch1_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc1.sbus_dma[1]);
}
bool cmic_cmc1_sbusdma_ch1_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc1.sbus_dma[1]);
}
bool cmic_cmc1_sbusdma_ch1_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_count_read,                                    cmicx->cmc1.sbus_dma[1].cur_count          )
bool cmic_cmc1_sbusdma_ch1_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_opcode_read,                                   cmicx->cmc1.sbus_dma[1].opcode             )
bool cmic_cmc1_sbusdma_ch1_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_sbus_start_address_read,                       cmicx->cmc1.sbus_dma[1].cur_sbus_address   )
bool cmic_cmc1_sbusdma_ch1_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch1_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch1_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[1]);
}
bool cmic_cmc1_sbusdma_ch1_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbus_address_read,                         cmicx->cmc1.sbus_dma[1].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_read,               cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_read,                 cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_read,                cmicx->cmc1.sbus_dma[1].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_write)
bool cmic_cmc1_sbusdma_ch1_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_read)
bool cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc1.sbus_dma[1]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_iter_count_read,                               cmicx->cmc1.sbus_dma[1].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch1_timer_read,                                    cmicx->cmc1.sbus_dma[1].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc1_sbusdma_ch1_timer_write,                                   cmicx->cmc1.sbus_dma[1].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch1_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch1_tm_control_2_write)

/* cmc1, sbus dma ch2 */
bool cmic_cmc1_sbusdma_ch2_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc1.sbus_dma[2]);
}
bool cmic_cmc1_sbusdma_ch2_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc1.sbus_dma[2]);
}
bool cmic_cmc1_sbusdma_ch2_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc1.sbus_dma[2]);
}
bool cmic_cmc1_sbusdma_ch2_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_count_read,                                    cmicx->cmc1.sbus_dma[2].cur_count          )
bool cmic_cmc1_sbusdma_ch2_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_opcode_read,                                   cmicx->cmc1.sbus_dma[2].opcode             )
bool cmic_cmc1_sbusdma_ch2_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_sbus_start_address_read,                       cmicx->cmc1.sbus_dma[2].cur_sbus_address   )
bool cmic_cmc1_sbusdma_ch2_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch2_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch2_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[2]);
}
bool cmic_cmc1_sbusdma_ch2_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbus_address_read,                         cmicx->cmc1.sbus_dma[2].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_read,               cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_read,                 cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_read,                cmicx->cmc1.sbus_dma[2].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_write)
bool cmic_cmc1_sbusdma_ch2_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_read)
bool cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc1.sbus_dma[2]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_iter_count_read,                               cmicx->cmc1.sbus_dma[2].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch2_timer_read,                                    cmicx->cmc1.sbus_dma[2].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc1_sbusdma_ch2_timer_write,                                   cmicx->cmc1.sbus_dma[2].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch2_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch2_tm_control_2_write)

/* cmc1, sbus dma ch3 */
bool cmic_cmc1_sbusdma_ch3_control_read(reg_t *val) {
    return cmic_sbusdma_control_read(val,&cmicx->cmc1.sbus_dma[3]);
}
bool cmic_cmc1_sbusdma_ch3_control_write(reg_t val) {
    return cmic_sbusdma_control_write(val,&cmicx->cmc1.sbus_dma[3]);
}
bool cmic_cmc1_sbusdma_ch3_request_read(reg_t *val) {
    return cmic_sbusdma_request_read(val,&cmicx->cmc1.sbus_dma[3]);
}
bool cmic_cmc1_sbusdma_ch3_request_write(reg_t val) {
    return cmic_sbusdma_request_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_count_read,                                    cmicx->cmc1.sbus_dma[3].cur_count          )
bool cmic_cmc1_sbusdma_ch3_count_write(reg_t val) {
    return cmic_sbusdma_count_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_opcode_read,                                   cmicx->cmc1.sbus_dma[3].opcode             )
bool cmic_cmc1_sbusdma_ch3_opcode_write(reg_t val) {
    return cmic_sbusdma_opcode_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_sbus_start_address_read,                       cmicx->cmc1.sbus_dma[3].cur_sbus_address   )
bool cmic_cmc1_sbusdma_ch3_sbus_start_address_write(reg_t val) {
    return cmic_sbusdma_sbus_start_address_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_read,                 UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_read,                 UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_hostmem_address))
bool cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_hostmem_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_desc_start_address_lo_read,                    UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch3_desc_start_address_lo_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_lo_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_desc_start_address_hi_read,                    UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_desc_address)   )
bool cmic_cmc1_sbusdma_ch3_desc_start_address_hi_write(reg_t val) {
    return cmic_sbusdma_desc_start_address_hi_write(val,&cmicx->cmc1.sbus_dma[3]);
}
bool cmic_cmc1_sbusdma_ch3_status_read(reg_t *val) {
    return cmic_sbusdma_status_read(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_status_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_read,                   UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_read,                   UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_hostmem_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbus_address_read,                         cmicx->cmc1.sbus_dma[3].cur_sbus_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbus_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_read,                      UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_read,                      UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_desc_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_read,               cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_request)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_read,                 cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read,    cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_sbus_start_address)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read, UINT64_LOWER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read, UINT64_UPPER32_REFERENCE(cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_hostmem_start_address))
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_read,                cmicx->cmc1.sbus_dma[3].cur_sbusdma_config_opcode)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_write)
bool cmic_cmc1_sbusdma_ch3_sbusdma_debug_read(reg_t *val) {
    return cmic_sbusdma_debug_read(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_sbusdma_debug_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_read)
bool cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_write(reg_t val) {
    return cmic_sbusdma_debug_clr_write(val,&cmicx->cmc1.sbus_dma[3]);
}
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_lo_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_lo_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_hi_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_address_hi_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_control_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_sbusdma_eccerr_control_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_iter_count_read,                               cmicx->cmc1.sbus_dma[3].iter_count)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_iter_count_write)
PCID_REG_READ_FUNC_GEN(      cmic_cmc1_sbusdma_ch3_timer_read,                                    cmicx->cmc1.sbus_dma[3].timer)
PCID_REG_WRITE_FUNC_GEN(     cmic_cmc1_sbusdma_ch3_timer_write,                                   cmicx->cmc1.sbus_dma[3].timer)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_tm_control_0_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_tm_control_0_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_tm_control_1_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_tm_control_1_write)
PCID_REG_READ_NULL_FUNC_GEN( cmic_cmc1_sbusdma_ch3_tm_control_2_read)
PCID_REG_WRITE_NULL_FUNC_GEN(cmic_cmc1_sbusdma_ch3_tm_control_2_write)

