/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



/*
 * cmicx_packet_dma.c
 *
 * contains implementation of packet dma register access functions, as well as helper functions, update functions, and init functions for packet dma.
 */


/* includes */
#include <stdio.h>
#include <string.h>
#ifndef CMICX_STANDALONE_SIM
 #include <../systems/sim/pcid/cmicx_sim.h>
 #include <../systems/sim/pcid/cmicx_sim_pcid.h>
 #include <../systems/sim/pcid/cmicx_sim_schan.h>
 #include <../systems/sim/pcid/cmicx_sim_packet_dma.h>
#else
 #include "cmicx_sim.h"
 #include "cmicx_sim_pcid.h"
 #include "cmicx_sim_schan.h"
 #include "cmicx_sim_packet_dma.h"
#endif

/* external cmicx global structure pointer */
extern cmicx_t *cmicx;

/* global files for input/output of packets and global buffer data for storing packets */
static FILE          *cmicx_packet_dma_ip_file;
static FILE          *cmicx_packet_dma_ep_file;
static FILE          *cmicx_packet_dma_loopback_write_file;
static FILE          *cmicx_packet_dma_loopback_read_file;
static unsigned char  packet_data_buffer[CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE];
static int            current_buffer_index;
static int            packet_cos;


/**********************************************************************/
/* packet dma support functions. update, init, start access, abort, etc */
/**********************************************************************/

/* global packet dma init function. note the ep file name and ip file name. this function takes care of opening the files for */
/* packet input and output into the cmicx through the packet dma hardware (among other global packet dma things). */
bool cmicx_packet_dma_global_init(char *ip_file_name,char *ep_file_name,char *loopback_file_name) {

    /* open the log file for packet output from the packet dma through tx, and packet input to the packet dma through rx. */
    /* these files correspond to the ingress and egress ports of the switch chip, respectively, and can be the same file. */
    /* in this case, it would constitute a loopback of the packet data from the tx packet port of the cmicx to the rx packet port. */
    cmicx_packet_dma_ip_file             = fopen(ip_file_name,"a");
    cmicx_packet_dma_ep_file             = fopen(ep_file_name,"r");
    cmicx_packet_dma_loopback_write_file = fopen(loopback_file_name,"a");
    cmicx_packet_dma_loopback_read_file  = fopen(loopback_file_name,"r");
    if ((cmicx_packet_dma_ip_file==NULL) || (cmicx_packet_dma_ep_file==NULL) ||  (cmicx_packet_dma_loopback_write_file==NULL) || (cmicx_packet_dma_loopback_read_file==NULL)) {
        return false;
    }
    return true;

}


/* init function for individual packet dma channel */
bool cmicx_packet_dma_init(cmicx_packet_dma_data_t *packet_dma_data,char *id_string,int channel_number)
{
    int i;

    /* if the given id string is too long, log an error and quit */
    if ((strlen(id_string)+1)>CMIC_PACKET_DMA_ID_STRING_LENGTH) {
        cmicx_error("ERROR: given id string \"");
        cmicx_error(id_string);
        cmicx_error("\" for packet dma unit is too long.\n");
        return false;
    }

    /* copy the id string and channel number to the storage for the schan */
    strcpy(packet_dma_data->id_string,id_string);
    packet_dma_data->channel_number          = channel_number;

    /* control register values */
    packet_dma_data->ctrl_direction                    = false;
    packet_dma_data->ctrl_dma_en                       = false;
    packet_dma_data->ctrl_abort_dma                    = false;
    packet_dma_data->ctrl_pktdma_endianness            = false;
    packet_dma_data->ctrl_desc_endianness              = false;
    packet_dma_data->ctrl_drop_rx_pkt_on_chain_end     = false;
    packet_dma_data->ctrl_rld_sts_upd_dis              = false;
    packet_dma_data->ctrl_desc_done_intr_mode          = false;
    packet_dma_data->ctrl_enable_continuous_dma        = false;
    packet_dma_data->ctrl_contiguous_descriptors       = false;
    packet_dma_data->ctrl_ep_to_cpu_header_size_dwords = 14;
    packet_dma_data->ctrl_ep_to_cpu_alignment_bytes    = 0;
    packet_dma_data->ctrl_ep_to_cpu_header_endianness  = false;

    /* internal control values */
    packet_dma_data->last_desc               = false;   /* indicates the last descriptor in a descriptor stream */
    packet_dma_data->last_packet_chunk       = false;   /* indicates that we are dealing with the last chunk in a packet. we either start a new packet or end access after this. */
    packet_dma_data->packet_chunk_countdown  = 0;       /* holds the current countdown for completing the transfer of this packet chunk. when this reaches 0, the chunk is marked as complete. */
    packet_dma_data->chain_pending           = false;   /* indicates that we are still chaining descriptors, regardless of whether we are mid-packet or mid-packet-chunk or not */
    packet_dma_data->waiting_on_packet       = false;   /* indicates whether this channel is currently waiting for valid packet data or not (used in the rx case). */
    packet_dma_data->header_bytes_remaining  = 0;       /* indicates the number of bytes left in the header of a packet, before the header is fully transferred. */

    /* interrupt coalescing values */
    packet_dma_data->intr_coal_enable               = false;
    packet_dma_data->intr_coal_count                = 0;
    packet_dma_data->intr_coal_timer                = 0;
    packet_dma_data->intr_coal_cur_timer            = 0;
    packet_dma_data->intr_coal_cur_desc_done_count  = 0;
    packet_dma_data->intr_coal_cur_chain_done_count = 0;

    /* stat register values */
    packet_dma_data->stat_ch_in_halt               = false;
    packet_dma_data->stat_pktwr_ecc_err            = false;
    packet_dma_data->stat_descrd_addr_decode_err   = false;
    packet_dma_data->stat_pktwrrd_addr_decode_err  = false;
    packet_dma_data->stat_stwt_addr_decode_err     = false;
    packet_dma_data->stat_dma_active               = false;
    packet_dma_data->stat_chain_done               = false;
    packet_dma_data->irq_stat_desc_done            = false;
    packet_dma_data->irq_stat_chain_done           = false;
    packet_dma_data->irq_stat_intr_coalescing_intr = false;
    packet_dma_data->irq_stat_desc_controlled_intr = false;

    /* cos control register values */
    for (i=0; i<CMIC_PACKET_DMA_COS_COUNT; i++) {
        packet_dma_data->cos_ctrl_bmp[i] = false;
    }

    /* pkt count register values */
    packet_dma_data->txpkt_count     = 0;
    packet_dma_data->rxpkt_count     = 0;

    /* descriptor address values */
    packet_dma_data->desc_addr       = 0x0;
    packet_dma_data->desc_halt_addr  = 0x0;
    packet_dma_data->cur_desc_addr   = 0x0;

    /* values garnered from descriptors */
    packet_dma_data->packet_mem_addr                     = 0x0;
    packet_dma_data->desc_byte_count                     = 0;
    packet_dma_data->desc_reload                         = false;
    packet_dma_data->desc_higig_pkt                      = false;
    packet_dma_data->desc_purge_packet                   = false;
    packet_dma_data->desc_interrupt                      = 0;
    packet_dma_data->desc_descriptors_remaining          = 0;
    packet_dma_data->desc_packet_bytes_transferred       = 0;
    packet_dma_data->desc_packet_end_bit                 = false;
    packet_dma_data->desc_packet_start_bit               = false;
    packet_dma_data->desc_packet_cell_error_bit          = false;
    packet_dma_data->desc_packet_data_read_ecc_error_bit = false;
    packet_dma_data->desc_packet_completion_error_bit    = false;
    packet_dma_data->desc_packet_done_bit                = false;

    /* storage for buffered packet data on this channel */
    packet_dma_data->current_buffer_index      = 0;
    packet_dma_data->current_buffer_read_index = 0;


    /* successful initialization complete */
    cmicx_log("successfully init'd \"");
    cmicx_log(packet_dma_data->id_string);
    cmicx_log("\".");
    cmicx_log_newline();
    return true;

}


/* global packet dma update function. the main purpose of this function is to read in rx packets from the EP input file. */
/* from there, these packets are fed to individual packet dma channels performing Rx. */
bool cmicx_packet_dma_global_update(void) {

    int      i;
    int      num_bytes;
    unsigned temp_val;
    int      return_value;
    bool     need_transfer    = false;
    bool     transfer_blocked = false;
    FILE    *input_file;
    bool     cmc0_packet_dma_need_transfer[CMICX_NUM_PACKET_DMA_PER_CMC];
    bool     cmc1_packet_dma_need_transfer[CMICX_NUM_PACKET_DMA_PER_CMC];

    /* set default values for "need transfer" bits. */
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        cmc0_packet_dma_need_transfer[i] = false;
        cmc1_packet_dma_need_transfer[i] = false;
    }

    cmicx_log_level_set(0);
    cmicx_log("packet dma global update... ");
    cmicx_log_level_set(3);

    /* if we can get a new packet because the global packet buffer is currently empty, then do it. */
    if (current_buffer_index==0) {

        /* if we have loopback enabled, we need to read in from the loopback file. */
        if (cmicx->ip_2_ep_loopback_enable==true) {
            input_file = cmicx_packet_dma_loopback_read_file;
        }
        else {
            input_file = cmicx_packet_dma_ep_file;
        }

        /* make sure that we can read from the file. */
        if (input_file==NULL) {
            /* this error (below) is going to be masked, because it is a pretty common case (didn't create the input packet file), especially when utilizing hardware loopback. */
            /*
            cmicx_log("ERROR: cannot read from packet input file. ");
            cmicx_log_newline();
            return false;
            */
            return true;
        }

        /* read the packet header, and simultaneously ensure that it is in the right format. */
        return_value = fscanf(input_file," [packet] bytes: %d COS: %d ",&num_bytes,&packet_cos);

        /* check if we've hit the end of file, without reading anything. if we have at this point, then we simply don't have any data available to read. move on. */
        if (return_value==EOF) {
            return true;
        }

        /* check for the error case (header not in the format we wanted, or we unexpectedly hit EOF. this is an unrecoverable error. */
        if ((return_value!=2) || (ferror(input_file)!=0) || (feof(input_file)!=0)) {
            cmicx_log("error: error reading packet header from packet input file into packet dma.");
            cmicx_log_newline();
            cmicx_error("error: error reading packet header from packet input file into packet dma.\n");
            return false;
        }

        /* check that we will not try to load too many bytes into the buffer. */
        if (num_bytes>CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE) {
            cmicx_log("error: too many bytes in input packet at ep interface. would overflow global packet data buffer if read.");
            cmicx_log_newline();
            cmicx_error("error: too many bytes in input packet at ep interface. would overflow global packet data buffer if read.\n");
            return false;
        }

        /* If the num_bytes is greater than the check above, we exit */
        /* read the actual packet data, byte-by-byte, into the packet data buffer. */
        /* coverity[tainted_data : FALSE] */
        for (i=0; i<num_bytes; i++) {

            /* read a byte, and then make sure we have read correctly. */
            return_value = fscanf(input_file,"%02X",&temp_val);
            packet_data_buffer[current_buffer_index] = (unsigned char)temp_val;
            if ((return_value!=1) || (ferror(input_file)!=0) || (feof(input_file)!=0)) {
                cmicx_log("error: error reading packet data from packet input file for packet dma.");
                cmicx_log_newline();
                cmicx_error("error: error reading packet data from packet input file for packet dma.\n");
                return false;
            }
            current_buffer_index++;

            /* check for end-of-packet, if we expect it. */
            if (i==(num_bytes-1)) {
                /* Error values are checked below */
                /* coverity[check_return] */
                fscanf(input_file," [/packet]");
                if ((ferror(input_file)!=0) || (feof(input_file)!=0)) {
                    cmicx_log("error: error reading packet data from packet input file for packet dma. packet did not end when expected.");
                    cmicx_log_newline();
                    cmicx_error("error: error reading packet data from packet input file for packet dma. packet did not end when expected.\n");
                    return false;
                }
            }

            /* just the common in-between byte case. this covers end-of-lines in between packet bytes and also spaces in between packet bytes on the same line. */
            else {
                /* Error values are checked below */
                /* coverity[check_return] */
                fscanf(input_file," ");
                if ((ferror(input_file)!=0) || (feof(input_file)!=0)) {
                    cmicx_log("error: error reading packet data from packet input file for packet dma. packet not in the correct format (somehow).");
                    cmicx_log_newline();
                    cmicx_error("error: error reading packet data from packet input file for packet dma. packet not in the correct format (somehow).\n");
                    return false;
                }
            }

        }

        /* at this point, we have successfully read a full packet into the global buffer */
        cmicx_log("global packet dma update: full packet read from input file.");
        cmicx_log_newline();
    }

    /* set default values for "need transfer" bits. */
    need_transfer    = false;
    transfer_blocked = false;
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        cmc0_packet_dma_need_transfer[i] = false;
        cmc1_packet_dma_need_transfer[i] = false;
    }

    /* if the packet buffer is not currently empty, then we have a packet there. distribute it to the appropriate packet dma channels for rx (if we can). */
    /* note that this operation is atomic - a packet will either get distributed to all the appropriate channels at once and the global buffer will be freed up, or it won't. */
    if (current_buffer_index!=0) {

        /* go through each packet dma channel, and see if 1) the channel is active, 2) the channel is in rx mode 3) the channel is accepting packets */
        /* with the class of service we currently have available. if all of these conditions are true, then we need to disburse the currently global packet to this channel. */
        for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {

            if ((cmicx->cmc0.packet_dma[i].ctrl_dma_en==true)              &&
                (cmicx->cmc0.packet_dma[i].ctrl_direction==false)          &&
                (cmicx->cmc0.packet_dma[i].cos_ctrl_bmp[packet_cos]==true) &&
                ((cmicx->cmc0.packet_dma[i].chain_pending==true) || (cmicx->cmc0.packet_dma[i].ctrl_drop_rx_pkt_on_chain_end==false))) {

                need_transfer                    = true;
                cmc0_packet_dma_need_transfer[i] = true;
                if (cmicx->cmc0.packet_dma[i].current_buffer_index!=0) {
                    transfer_blocked = true;
                    break;
                }

            }

            if ((cmicx->cmc1.packet_dma[i].ctrl_dma_en==true)              &&
                (cmicx->cmc1.packet_dma[i].ctrl_direction==false)          &&
                (cmicx->cmc1.packet_dma[i].cos_ctrl_bmp[packet_cos]==true) &&
                ((cmicx->cmc1.packet_dma[i].chain_pending==true) || (cmicx->cmc1.packet_dma[i].ctrl_drop_rx_pkt_on_chain_end==false))) {

                need_transfer = true;
                cmc1_packet_dma_need_transfer[i] = true;
                if (cmicx->cmc1.packet_dma[i].current_buffer_index!=0) {
                    transfer_blocked = true;
                    break;
                }

            }

        }

        /* complete the transfer of the packet in the global buffer to all necessary packet dma channels, if we can. otherwise, the packet will sit in the global buffer */
        if ((need_transfer==true) && (transfer_blocked==false)) {

            for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {

                if (cmc0_packet_dma_need_transfer[i]==true) {
                    memcpy((void *)cmicx->cmc0.packet_dma[i].packet_data_buffer,(void *)packet_data_buffer,current_buffer_index);
                    cmicx->cmc0.packet_dma[i].current_buffer_index      = current_buffer_index;
                    cmicx->cmc0.packet_dma[i].current_buffer_read_index = 0;
                }
                if (cmc1_packet_dma_need_transfer[i]==true) {
                    memcpy((void *)cmicx->cmc1.packet_dma[i].packet_data_buffer,(void *)packet_data_buffer,current_buffer_index);
                    cmicx->cmc1.packet_dma[i].current_buffer_index      = current_buffer_index;
                    cmicx->cmc1.packet_dma[i].current_buffer_read_index = 0;
                }

            }

            current_buffer_index = 0;

        }


    }

    /* successful global packet dma update */
    cmicx_log_level_set(0);
    cmicx_log(" ...done.");
    cmicx_log_newline();
    cmicx_log_level_set(3);
    return true;

}


/* update function for individual packet dma channels */
/*
 * steps for updating a packet dma:
 * 1) check if a chain is currently pending. if so, then move to step 2. if not, then we are done.
 * 2) if we are in halt mode currently, then check if the halt address has been updated and no longer matches the current descriptor address.
 *    if so, then exit halt mode and continue on with step 3. if not, then we are done.
 * 3) check if we are not currently working on a packet chunk (descriptor). if so, then we do a request for a new descriptor, and start a new packet chunk (countdown and all)
 * 4) once we reach the end of a packet chunk countdown, then we can complete the chunk, and perform tx and rx specific tasks (dma'ing, etc).
 * 5) if we've reached the end of a packet, then we can transfer an entire packet to the IP interface in the case of tx.
 * 5) if we are on the last descriptor (and hence also the last packet chunk/at the end of a packet), then we can end the packet dma chain.
 */
bool cmicx_packet_dma_update(pcid_info_t *pcid_info, cmicx_packet_dma_data_t *packet_dma_data) {

    bool          chain             = false;
    bool          scatter           = false;
    bool          gather            = false;
    bool          scatter_or_gather = false;
    reg_t         mem[CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR];   /* return memory from pcid (used for desc requests) */
    int           i;
    int           bytes_available;
    int           bytes_transferred;
    bool          end_bit;
    bool          start_bit;
    FILE         *output_file;
    int           packet_cos;
    unsigned char temp_packet_data[CMIC_PACKET_DMA_TEMP_PACKET_DATA_SIZE];
    int           front_diff;
    int           rear_diff;
    int           num_words;
    int           alignment_bytes;
    int           data_bytes_transferred;
    int           header_bytes_transferred;

    memset((void *)temp_packet_data,0,CMIC_PACKET_DMA_TEMP_PACKET_DATA_SIZE);

    /* print log messages for this schan */
    cmicx_log_level_set(0);
    cmicx_log("updating \"");
    cmicx_log(packet_dma_data->id_string);
    cmicx_log("\"... ");
    cmicx_log_level_set(3);

    /* handle interrupts to be sent from the timer, if interrupt coalescing is enabled. */
    if ((packet_dma_data->intr_coal_enable==true) && (packet_dma_data->intr_coal_timer!=0)) {

        packet_dma_data->intr_coal_cur_timer++;

        if (packet_dma_data->intr_coal_cur_timer==packet_dma_data->intr_coal_timer) {
            cmic_packet_dma_interrupts_post(packet_dma_data);
        }

    }

    /* if we have a chain pending, then handle descriptor requesting, counter setup/decrement and packet chunk completion */
    /* if necessary. if we don't have a chain pending, then we just bail out of the function. */
    if (packet_dma_data->chain_pending==true) {

        /* first, check if we are in halt mode currently. if so, we cannot proceed further (unless the addresses do not match anymore). */
        if (packet_dma_data->stat_ch_in_halt==true) {

            if (packet_dma_data->cur_desc_addr!=packet_dma_data->desc_halt_addr) {
                packet_dma_data->stat_ch_in_halt = false;
                cmicx_log("\"");
                cmicx_log(packet_dma_data->id_string);
                cmicx_log("\" leaving halt mode. ");
            }

        }

        /* if we are no longer in halt mode (or we never were). */
        if (packet_dma_data->stat_ch_in_halt==false) {

            /* if we do not have a packet dma packet chunk (descriptor) pending, then we need to initiate a memory read at this point, to get a new descriptor (and wait for the data to come back). */
            if ((packet_dma_data->packet_chunk_countdown==0) && (packet_dma_data->waiting_on_packet==false)) {

                /* reset the "reload" value before we start loading descriptors. */
                packet_dma_data->desc_reload = false;

                /* we need to loop the descriptor request, because we may need to reload descriptors, necessitating a re-request */
                do {

                    /* if continuous dma mode is enabled and we have reached the halt address, then we need to go into halt mode and stop (exit early). */
                    /* can only do this if we are allowed to update status fields (hence, we are not reloading a descriptor or the rld_sts_upd_dis bit is set to 0. */
                    if ((packet_dma_data->ctrl_enable_continuous_dma==true) &&
                        (packet_dma_data->cur_desc_addr==packet_dma_data->desc_halt_addr) &&
                        ((packet_dma_data->desc_reload==false) || (packet_dma_data->ctrl_rld_sts_upd_dis==false))) {

                        packet_dma_data->stat_ch_in_halt = true;
                        cmicx_log("\"");
                        cmicx_log(packet_dma_data->id_string);
                        cmicx_log("\" going into halt mode on address ");
                        cmicx_log_uint64(packet_dma_data->cur_desc_addr);
                        cmicx_log(". ...done. ");
                        cmicx_log_newline();
                        return true;

                    }

                    /* make the memory read over the pcid to get the descriptor words */
                    cmicx_pcid_mem_read(pcid_info,packet_dma_data->cur_desc_addr,(void *)mem,CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR*CMICX_BYTES_PER_WORD);
                    cmicx_log("packet dma descriptor read from memory on \"");
                    cmicx_log(packet_dma_data->id_string);
                    cmicx_log("\", words: ");
                    for (i=0; i<CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR; i++) {
                        cmicx_log_hex(mem[i]);
                        cmicx_log(", ");
                    }
                    cmicx_log_newline();

                    /* if we have little endian descriptors, reverse the bytes of the descriptor words */
                    if (packet_dma_data->ctrl_desc_endianness==true) {
                        for (i=0; i<CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR; i++) {
                            mem[i] = cmicx_reverse_word_endianness(mem[i]);
                        }
                    }

                    /* descriptor dwords 0 and 1: packet memory address */
                    UINT64_LOWER32_REFERENCE(packet_dma_data->packet_mem_addr) = mem[0];
                    UINT64_UPPER32_REFERENCE(packet_dma_data->packet_mem_addr) = mem[1];

                    /* descriptor dword 2: special descriptor control word */
                    packet_dma_data->desc_byte_count            = REG_FIELD_GET( mem[2], CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_MSB,CMIC_PKTDMA_DESC_DWORD2_BYTE_COUNT_LSB                       );
                                     chain                      = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_CHAIN_BIT                                                                   );
                                     scatter                    = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_SCATTER_BIT                                                                 );
                                     gather                     = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_GATHER_BIT                                                                  );
                    packet_dma_data->desc_reload                = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_RELOAD_BIT                                                                  );
                    packet_dma_data->desc_higig_pkt             = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_HIGIG_PKT_BIT                                                               );
                    packet_dma_data->desc_purge_packet          = REG_BIT_GET(   mem[2], CMIC_PKTDMA_DESC_DWORD2_PURGE_PACKET_BIT                                                            );
                    packet_dma_data->desc_interrupt             = REG_FIELD_GET( mem[2], CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_MSB,CMIC_PKTDMA_DESC_DWORD2_INTERRUPT_LSB                         );
                    packet_dma_data->desc_descriptors_remaining = REG_FIELD_GET( mem[2], CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_MSB,CMIC_PKTDMA_DESC_DWORD2_DESCRIPTORS_REMAINING_LSB );

                    /* possible use cases, that set certain flags. taken directly from the CMICx microarchitecture on the packet dma (section 7.4.1.1.1). */
                    scatter_or_gather = (packet_dma_data->ctrl_direction==true) ? scatter : gather;
                    if ((chain==false) && (packet_dma_data->desc_reload==false) && (scatter_or_gather==false)) {
                        packet_dma_data->last_desc         = true;
                        packet_dma_data->last_packet_chunk = true;
                    }
                    if ((chain==false) && (packet_dma_data->desc_reload==false) && (scatter_or_gather==true)) {
                        cmicx_log("error: packet dma use case not supported: chain: false. reload: false. scatter/gather: true.");
                        return false;
                    }
                    if ((chain==false) && (packet_dma_data->desc_reload==true)) {
                        /* unused case for ending a chain. still performs desc reload and updates desc address, but chain necessarily ends. */
                        packet_dma_data->last_desc         = true;
                        packet_dma_data->last_packet_chunk = true;
                    }
                    if ((chain==true) && (packet_dma_data->desc_reload==false) && (scatter_or_gather==false)) {
                        packet_dma_data->last_desc         = false;
                        packet_dma_data->last_packet_chunk = true;
                    }
                    if ((chain==true) && (packet_dma_data->desc_reload==false) && (scatter_or_gather==true)) {
                        packet_dma_data->last_desc         = false;
                        packet_dma_data->last_packet_chunk = false;
                    }
                    if ((chain==true) && (packet_dma_data->desc_reload==true)) {
                        /* the normal reload descriptor case */
                        packet_dma_data->last_desc         = false;
                        packet_dma_data->last_packet_chunk = false;
                    }

                    /* update the descriptor address appropriately if necessary and we require a reload of the descriptor. */
                    if (packet_dma_data->desc_reload==true) {
                        cmic_packet_dma_descriptor_done(pcid_info, packet_dma_data);
                        packet_dma_data->cur_desc_addr = packet_dma_data->packet_mem_addr;
                        if (packet_dma_data->last_desc==true) {
                            cmic_packet_dma_chain_done(packet_dma_data);
                            cmicx_log(" ...done. ");
                            cmicx_log_newline();
                            return true;
                        }
                    }

                }
                while (packet_dma_data->desc_reload==true);

                /* at this point, we definitely have valid data in the control values - we've just loaded a new descriptor. thus, we can start grabbing/sending a packet segment */
                /* described by that descriptor. this could include starting a new packet, or continuing one that we already have been working on. set variables for successfully */
                /* beginning a packet chunk below, and print log messages. */
                packet_dma_data->packet_chunk_countdown = CMIC_PACKET_DMA_ACCESS_COUNTDOWN_VALUE;
                cmicx_log("starting to handle new packet chunk on \"");
                cmicx_log(packet_dma_data->id_string);
                cmicx_log("\". packet chunk length: ");
                cmicx_log_int(packet_dma_data->desc_byte_count);
                cmicx_log(". packet chunk address (source or destination): ");
                cmicx_log_uint64(packet_dma_data->packet_mem_addr);
                cmicx_log(". ");
                cmicx_log_newline();

            }


            /* at this point we are just updating packet chunks (descriptors) that we have pending. decrement the chunk countdown if we can, */
            /* until it reaches 0. once it reaches 0, then we can finish a packet chunk (a descriptor). */
            if (packet_dma_data->packet_chunk_countdown!=0) {
                packet_dma_data->packet_chunk_countdown--;
            }
            if (packet_dma_data->packet_chunk_countdown==0) {


                /* at this point, we can complete the packet chunk (a descriptor). */
                /* if we are doing a tx transmission, then we need to read the packet chunk from host memory, and write it to the local packet fifo. */
                if (packet_dma_data->ctrl_direction==true) {

                    /* make sure we don't overrun the buffer in the process of writing to it */
                    if ((packet_dma_data->current_buffer_index+packet_dma_data->desc_byte_count)>=CMIC_PACKET_DMA_PACKET_DATA_BUFFER_SIZE) {
                        cmicx_log("error: attempting to write packet chunk past end of internal packet dma channel buffer on packet tx.");
                        return false;
                    }

                    /* do the dma read to a temporary buffer, because we may need to reverse word endianness. */
                    /* note that we make our read here for 32-bit word chunks (that are aligned to 32-bit words in memory), because of the endianness issue. */
                    front_diff = (packet_dma_data->packet_mem_addr%4);
                    rear_diff  = ((packet_dma_data->desc_byte_count+front_diff)%4);
                    rear_diff  = (rear_diff>0) ? (4-rear_diff) : 0;
                    num_words  = (front_diff+packet_dma_data->desc_byte_count+rear_diff)/CMICX_BYTES_PER_WORD;
                    cmicx_pcid_mem_read(pcid_info,(packet_dma_data->packet_mem_addr-front_diff),(void *)temp_packet_data,(num_words*CMICX_BYTES_PER_WORD));

                    /* if we need to swap packet endianness, then do it. */
                    if (packet_dma_data->ctrl_pktdma_endianness==true) {
                        for (i=0; i<num_words; i++) {
                            *((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])) = cmicx_reverse_word_endianness(*((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])));
                        }
                    }

                    /* finally, copy the appropriate data to the actual local packet data buffer for this channel, in preparation for transmission. */
                    /* also, log some messages that the chunk we were working on is done. */
                    memcpy((void *)&(packet_dma_data->packet_data_buffer[packet_dma_data->current_buffer_index]),(void *)&(temp_packet_data[front_diff]),packet_dma_data->desc_byte_count);
                    packet_dma_data->current_buffer_index += packet_dma_data->desc_byte_count;
                    cmicx_log("packet chunk completing. wrote ");
                    cmicx_log_int(packet_dma_data->desc_byte_count);
                    cmicx_log(" bytes from host memory to \"");
                    cmicx_log(packet_dma_data->id_string);
                    cmicx_log("\"'s internal packet buffer. ");

                    /* write the descriptor ack word for tx to memory (dword 3 of the descriptor data) */
                    mem[3] = 0;
                    REG_FIELD_SET_TO(mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_MSB,CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_LSB, packet_dma_data->desc_byte_count);
                    REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_COMPLETION_ERROR_BIT,                                                       false);
                    REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_DONE_BIT,                                                                   true);
                    if (packet_dma_data->ctrl_desc_endianness==true) {
                        mem[3] = cmicx_reverse_word_endianness(mem[3]);
                    }
                    cmicx_pcid_mem_write(pcid_info,packet_dma_data->cur_desc_addr+((CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR-1)*CMICX_BYTES_PER_WORD),(void *)&(mem[3]),CMICX_BYTES_PER_WORD);

                    /* if we are done with the packet transfer (this is the last chunk), then we can send the buffer (the whole packet) */
                    /* to the "ingress pipe" of the chip (actually file output for us), and reset the packet buffer. */
                    if (packet_dma_data->last_packet_chunk==true) {

                        /* write the data to file. if we have hardware loopback enabled, we need to write to the loopback file. otherwise, the regular output file is ok. */
                        if (cmicx->ip_2_ep_loopback_enable==true) {
                            output_file = cmicx_packet_dma_loopback_write_file;
                            /* loopback causes us to grab the cos from the packet. it comes from bits 58-63 of the packet (the last 6 bits of the 8th byte in the packet header). */
                            /* see section 7.6.2 of the CMICx microarchitecture document for more detail. */
                            packet_cos  = (int)REG_FIELD_GET(packet_dma_data->packet_data_buffer[7],5,0);
                        }
                        else {
                            output_file = cmicx_packet_dma_ip_file;
                            packet_cos  = packet_dma_data->channel_number;
                        }
                        if (output_file!=NULL) {
                            fprintf(output_file,"[packet]\n");
                            fprintf(output_file,"bytes: %d\n",packet_dma_data->current_buffer_index);
                            fprintf(output_file,"COS: %d\n",packet_cos);
                            for (i=0; i<packet_dma_data->current_buffer_index; i++) {
                                fprintf(output_file,"%02X ",(packet_dma_data->packet_data_buffer[i] & 0xFF));   /* why is this 0xFF necessary? nothing should be sign extended (but yet it is). */
                                if (((i%CMIC_PACKET_DMA_BYTES_PER_LINE)==(CMIC_PACKET_DMA_BYTES_PER_LINE-1)) || (i==(packet_dma_data->current_buffer_index-1))) {
                                    fprintf(output_file,"\n");
                                }
                                if (i==(packet_dma_data->current_buffer_index-1)) {
                                    fprintf(output_file,"[/packet]\n");
                                }
                            }
                            fflush(output_file);
                        }

                        /* log messages that we sent the packet successfully, and reset the buffer index, so it can be reused. */
                        cmicx_log("packet on \"");
                        cmicx_log(packet_dma_data->id_string);
                        cmicx_log("\" fully dma'd. sent ");
                        cmicx_log_int(packet_dma_data->current_buffer_index);
                        cmicx_log(" byte packet out over IP interface. ");
                        packet_dma_data->current_buffer_index = 0;

                    }

                    cmicx_log_newline();

                }


                /* for an rx transmission, write packet data from this channel's packet buffer to host memory */
                else {

                    /* see if we have data currently available in our local packet buffer. if not, we need to wait on a packet. */
                    if (packet_dma_data->current_buffer_index==0) {
                        packet_dma_data->waiting_on_packet = true;
                    }

                    /* else, if we do have data available, then we can start writing this data to host memory. */
                    else {

                        packet_dma_data->waiting_on_packet = false;
                        bytes_available = (packet_dma_data->current_buffer_index-packet_dma_data->current_buffer_read_index);

                        /* if we have no bytes available, we've already reached the end of the packet with previous descriptors, and this descriptor is just going to be skipped. */
                        if (bytes_available==0) {
                            bytes_transferred = 0;
                            end_bit           = false;
                            start_bit         = false;
                        }

                        /* make sure we don't read more from the buffer than we currently have in the process of reading from it. */
                        /* if we try to, then we have to mark the end of the packet, and all further descriptors (for this packet) are ignored. */
                        else if (packet_dma_data->desc_byte_count>=bytes_available) {
                            bytes_transferred = bytes_available;
                            end_bit           = true;
                            start_bit         = (packet_dma_data->current_buffer_read_index==0) ? true : false;
                        }

                        /* if we are reading less than we have in the buffer */
                        else {
                            bytes_transferred = packet_dma_data->desc_byte_count;
                            end_bit           = false;
                            start_bit         = (packet_dma_data->current_buffer_read_index==0) ? true : false;
                        }

                        /* do the actual dma write from the packet buffer directly to host memory (if we can). also log some messages. */
                        if (bytes_transferred!=0) {

                            /* if we are starting this packet, then we need to reset the size of the header that still needs to be transferred. */
                            if (start_bit==true) {
                                packet_dma_data->header_bytes_remaining = packet_dma_data->ctrl_ep_to_cpu_header_size_dwords*CMICX_BYTES_PER_WORD;
                            }

                            /* if we are transferring solely header in this descriptor (either part of the header or the whole header, but no data or alignment bytes). */
                            if ((packet_dma_data->header_bytes_remaining!=0) && (packet_dma_data->header_bytes_remaining>=bytes_transferred)) {
                                alignment_bytes          = 0;
                                header_bytes_transferred = bytes_transferred;
                                data_bytes_transferred   = 0;
                            }

                            /* if we are transferring the remainder of the header and at least 1 byte of data with this descriptor. */
                            else if ((packet_dma_data->header_bytes_remaining!=0) && (packet_dma_data->header_bytes_remaining<bytes_transferred)) {

                                /* if we have to add some space after the header, for alignment. make sure we can transfer the rest of the header, the alignment bytes, and at least one byte of data. */
                                if ((packet_dma_data->header_bytes_remaining+packet_dma_data->ctrl_ep_to_cpu_alignment_bytes+1)<=bytes_transferred) {
                                    alignment_bytes          = packet_dma_data->ctrl_ep_to_cpu_alignment_bytes;
                                    header_bytes_transferred = packet_dma_data->header_bytes_remaining;
                                    data_bytes_transferred   = (bytes_transferred-header_bytes_transferred-alignment_bytes);
                                }

                                /* we cannot accommodate the alignment spacing at this point even if it is specified, so leave it out. just transfer data bytes. */
                                else {
                                    alignment_bytes          = 0;
                                    header_bytes_transferred = packet_dma_data->header_bytes_remaining;
                                    data_bytes_transferred   = (bytes_transferred-header_bytes_transferred);
                                }

                            }

                            /* if we are only doing a data transfer. */
                            else {
                                alignment_bytes          = 0;
                                header_bytes_transferred = 0;
                                data_bytes_transferred   = bytes_transferred;
                            }

                            bytes_transferred -= alignment_bytes;


                            /* ---------- do a header transfer, if applicable. ---------- */

                            /* write the data to a temporary buffer first, in case we have to deal with endianness issues. */
                            memset((void *)temp_packet_data,0,CMIC_PACKET_DMA_TEMP_PACKET_DATA_SIZE);
                            memcpy((void *)temp_packet_data,(void *)&(packet_dma_data->packet_data_buffer[packet_dma_data->current_buffer_read_index]),header_bytes_transferred);
                            packet_dma_data->current_buffer_read_index += header_bytes_transferred;

                            /* if we have to do an endianness swap for packet header data, then do it before writing the data to host memory. */
                            if (packet_dma_data->ctrl_ep_to_cpu_header_endianness==true) {
                                if ((header_bytes_transferred%4)!=0) {
                                    cmicx_log("ERROR: trying to swap packet header endianness for incoming packet on \"");
                                    cmicx_log(packet_dma_data->id_string);
                                    cmicx_log("\", but desc byte count supplied not a multiple of 4 (header packet data cannot be broken down into 32-bit chunks for endianness swapping).");
                                    cmicx_log_newline();
                                }
                                else {
                                    for (i=0; i<(header_bytes_transferred/4); i++) {
                                        *((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])) = cmicx_reverse_word_endianness(*((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])));
                                    }
                                }
                            }

                            /* perform the actual write to memory (from our temporary buffer) */
                            cmicx_pcid_mem_write(pcid_info,packet_dma_data->packet_mem_addr,(void *)temp_packet_data,header_bytes_transferred);
                            packet_dma_data->header_bytes_remaining -= header_bytes_transferred;


                            /* ---------- do a data transfer, if applicable. ---------- */

                            /* perform data transfer to the temporary buffer. */
                            memset((void *)temp_packet_data,0,CMIC_PACKET_DMA_TEMP_PACKET_DATA_SIZE);
                            memcpy((void *)temp_packet_data,(void *)&(packet_dma_data->packet_data_buffer[packet_dma_data->current_buffer_read_index]),data_bytes_transferred);
                            packet_dma_data->current_buffer_read_index += data_bytes_transferred;

                            /* if we have to do an endianness swap for packet data, then do it before writing the data to host memory. */
                            if (packet_dma_data->ctrl_pktdma_endianness==true) {
                                if ((data_bytes_transferred%4)!=0) {
                                    cmicx_log("ERROR: trying to swap packet data endianness for incoming packet on \"");
                                    cmicx_log(packet_dma_data->id_string);
                                    cmicx_log("\", but byte count supplied not a multiple of 4 (packet data cannot be broken down into 32-bit chunks for endianness swapping).");
                                    cmicx_log_newline();
                                }
                                else {
                                    for (i=0; i<(data_bytes_transferred/4); i++) {
                                        *((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])) = cmicx_reverse_word_endianness(*((reg_t *)&(temp_packet_data[i*CMICX_BYTES_PER_WORD])));
                                    }
                                }
                            }

                            /* perform the actual write to memory (from our temporary buffer) */
                            cmicx_pcid_mem_write(pcid_info,(packet_dma_data->packet_mem_addr+header_bytes_transferred+alignment_bytes),(void *)temp_packet_data,data_bytes_transferred);


                            /* display output */
                            cmicx_log("packet chunk completing. wrote ");
                            cmicx_log_int(bytes_transferred);
                            cmicx_log(" bytes of packet to host memory from \"");
                            cmicx_log(packet_dma_data->id_string);
                            cmicx_log("\"'s internal packet buffer. ");

                        }
                        else {
                            cmicx_log("skipping descriptor on \"");
                            cmicx_log(packet_dma_data->id_string);
                            cmicx_log("\". ");
                        }

                        /* write the descriptor ack word for rx to memory (dword 3 of the descriptor data) */
                        mem[3] = 0;
                        REG_FIELD_SET_TO(mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_MSB,CMIC_PKTDMA_DESC_DWORD3_PACKET_BYTES_TRANSFERRED_LSB, bytes_transferred);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_END_BIT,                                                                    end_bit);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_START_BIT,                                                                  start_bit);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_CELL_ERROR_BIT,                                                             false);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_DATA_READ_ECC_ERROR_BIT,                                                    false);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_COMPLETION_ERROR_BIT,                                                       false);
                        REG_BIT_SET_TO(  mem[3],CMIC_PKTDMA_DESC_DWORD3_PACKET_DONE_BIT,                                                                    true);
                        if (packet_dma_data->ctrl_desc_endianness==true) {
                            mem[3] = cmicx_reverse_word_endianness(mem[3]);
                        }
                        cmicx_pcid_mem_write(pcid_info,packet_dma_data->cur_desc_addr+((CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR-1)*CMICX_BYTES_PER_WORD),(void *)&(mem[3]),CMICX_BYTES_PER_WORD);

                        /* if we are done with the packet transfer because we've read the last chunk from the buffer, then log messages that we sent */
                        /* the whole packet successfully, and reset the buffer index, so it can be reused for another incoming packet */
                        if (end_bit==true) {
                            cmicx_log("packet on \"");
                            cmicx_log(packet_dma_data->id_string);
                            cmicx_log("\" fully written to host memory. read ");
                            cmicx_log_int(packet_dma_data->current_buffer_read_index);
                            cmicx_log(" byte(s) of packet from the EP interface. ");
                            packet_dma_data->current_buffer_index      = 0;
                            packet_dma_data->current_buffer_read_index = 0;
                        }

                        /* if we're done with the packet transfer because we've hit the last designated descriptor for this packet, then */
                        /* log messages that we sent only part of the packet successfully, and reset the buffer index, so it can be reused. */
                        else if ((packet_dma_data->last_packet_chunk==true) && (bytes_available>0)) {
                            cmicx_log("packet on \"");
                            cmicx_log(packet_dma_data->id_string);
                            cmicx_log("\" partially written to host memory. read ");
                            cmicx_log_int(packet_dma_data->current_buffer_read_index);
                            cmicx_log(" byte(s) of packet from the EP interface. ");
                            packet_dma_data->current_buffer_index      = 0;
                            packet_dma_data->current_buffer_read_index = 0;
                        }

                        cmicx_log_newline();
                    }

                }


                /* we need to check that we are not currently waiting on packet data, so we know if we can complete this descriptor or not. */
                if (packet_dma_data->waiting_on_packet==false) {

                    /* at this point we are ending this descriptor, so we should indicate descriptor done status to the registers/interrupts and set the next current descriptor address appropriately. */
                    cmic_packet_dma_descriptor_done(pcid_info, packet_dma_data);
                    packet_dma_data->cur_desc_addr += (CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR*CMICX_BYTES_PER_WORD);    /* 4*4 because we have four words of four bytes each */

                    /* if we are done with the packet, then we need to determine what to do. either end the packet dma access altogether, */
                    /* or just move on to the next descriptor (starting the next packet). */
                    if (packet_dma_data->last_packet_chunk==true) {

                        if (packet_dma_data->ctrl_direction==true) {
                            packet_dma_data->txpkt_count++;
                        }
                        else {
                            packet_dma_data->rxpkt_count++;
                        }

                        /* if this was the last desc in the packet dma access, then mark the packet dma access done altogether */
                        if (packet_dma_data->last_desc==true) {
                            cmic_packet_dma_chain_done(packet_dma_data);
                            cmicx_log_newline();
                        }

                    }

                }


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


/* helper function to indicate that a packet dma descriptor has been completed. this functionality is needed during both descriptor reloads and normal descriptor completion. */
void cmic_packet_dma_descriptor_done(pcid_info_t *pcid_info, cmicx_packet_dma_data_t *packet_dma_data) {
    int i;
    reg_t         mem[CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR];   /* return memory from pcid (used for desc requests) */

    /* for actual descriptor done interrupt */
    if ((packet_dma_data->ctrl_desc_done_intr_mode==false) || (REG_BIT_GET(packet_dma_data->desc_interrupt,0)==true)) {

        /* if we have interrupt coalescing and count coalescing enabled, then we have to coalesce our interrupt. otherwise, just post it right away. */
        if ((packet_dma_data->intr_coal_enable==true) && (packet_dma_data->intr_coal_count!=0)) {
            packet_dma_data->intr_coal_cur_desc_done_count++;
            if ((packet_dma_data->intr_coal_cur_desc_done_count+packet_dma_data->intr_coal_cur_chain_done_count)==packet_dma_data->intr_coal_count) {
                cmic_packet_dma_interrupts_post(packet_dma_data);
            }
        }
        else {
            packet_dma_data->intr_coal_cur_desc_done_count++;
            cmic_packet_dma_interrupts_post(packet_dma_data);
        }

    }

    /* update status fields, if we can (when reload is set and rld_sts_upd_dis is true, we cannot). */
    if ((packet_dma_data->desc_reload==false) || (packet_dma_data->ctrl_rld_sts_upd_dis==false)) {

        /* for irq_stat bit setting */
        if ((packet_dma_data->ctrl_desc_done_intr_mode==false) || (REG_BIT_GET(packet_dma_data->desc_interrupt,1)==true)) {
            packet_dma_data->irq_stat_desc_controlled_intr = true;
        }

        /* always set the regular desc done bit */
        packet_dma_data->irq_stat_desc_done = true;

    }

    cmicx_pcid_mem_read(pcid_info,packet_dma_data->cur_desc_addr,(void *)mem,CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR*CMICX_BYTES_PER_WORD);
    cmicx_log("packet dma descriptor read from memory after descriptor completion \"");
    cmicx_log(packet_dma_data->id_string);
    cmicx_log("\", words: ");
    for (i=0; i<CMIC_PACKET_DMA_WORDS_PER_DESCRIPTOR; i++) {
         cmicx_log_hex(mem[i]);
         cmicx_log(", ");
    }
    cmicx_log_newline();
}


/* helper function for ending the descriptor chain. this functionality is used in more than one place. */
void cmic_packet_dma_chain_done(cmicx_packet_dma_data_t *packet_dma_data) {

    packet_dma_data->chain_pending       = false;

    /* if we have interrupt coalescing and count coalescing enabled, then we have to coalesce our interrupt. otherwise, just post it right away. */
    if ((packet_dma_data->intr_coal_enable==true) && (packet_dma_data->intr_coal_count!=0)) {
        packet_dma_data->intr_coal_cur_chain_done_count++;
        if ((packet_dma_data->intr_coal_cur_desc_done_count+packet_dma_data->intr_coal_cur_chain_done_count)==packet_dma_data->intr_coal_count) {
            cmic_packet_dma_interrupts_post(packet_dma_data);
        }
    }
    else {
        packet_dma_data->intr_coal_cur_chain_done_count++;
        cmic_packet_dma_interrupts_post(packet_dma_data);
    }

    /* update status fields, if we can (when reload is set and rld_sts_upd_dis is true, we cannot). */
    if ((packet_dma_data->desc_reload==false) || (packet_dma_data->ctrl_rld_sts_upd_dis==false)) {
        packet_dma_data->stat_dma_active     = false;
        packet_dma_data->stat_chain_done     = true;
        packet_dma_data->irq_stat_chain_done = true;
    }

    cmicx_log("ending packet dma access on \"");
    cmicx_log(packet_dma_data->id_string);
    cmicx_log("\". ");
}

/* helper function to post all pending interrupts from a given packet dma channel. */
void cmic_packet_dma_interrupts_post(cmicx_packet_dma_data_t *packet_dma_data) {

    int i;
    for (i=0; i<packet_dma_data->intr_coal_cur_desc_done_count; i++) {
        /* post actual per-desc interrupts here (method TBD) */
    }
    for (i=0; i<packet_dma_data->intr_coal_cur_chain_done_count; i++) {
        /* post actual per-chain interrupts here (method TBD) */
    }

    /* log messages and reset counters */
    cmicx_log("posted ");
    cmicx_log_int(packet_dma_data->intr_coal_cur_desc_done_count);
    cmicx_log(" desc interrupts and ");
    cmicx_log_int(packet_dma_data->intr_coal_cur_chain_done_count);
    cmicx_log(" chain interrupts to host.");
    cmicx_log_newline();
    packet_dma_data->intr_coal_cur_timer            = 0;
    packet_dma_data->intr_coal_cur_desc_done_count  = 0;
    packet_dma_data->intr_coal_cur_chain_done_count = 0;

}


/* starts a new packet dma access for a given packet dma module */
bool cmic_packet_dma_access_start(cmicx_packet_dma_data_t *packet_dma_data) {

    /* start logging */
    cmicx_log("attempting to start new packet dma access on \"");
    cmicx_log(packet_dma_data->id_string);
    cmicx_log("\"... ");

    /* already running failure */
    if (packet_dma_data->chain_pending==true) {
        cmicx_log(" failure: access already running.");
        cmicx_log_newline();
        return false;
    }

    /* dma en not cleared error */
    if (packet_dma_data->ctrl_dma_en==true) {
        cmicx_log(" failure: packet dma en bit still high (not cleared).");
        cmicx_log_newline();
        return false;
    }

    /* set variables for successfully beginning packet dma access, and print log messages */
    packet_dma_data->chain_pending          = true;
    packet_dma_data->last_desc              = false;
    packet_dma_data->last_packet_chunk      = false;
    packet_dma_data->packet_chunk_countdown = 0;
    packet_dma_data->ctrl_dma_en            = true;
    packet_dma_data->stat_dma_active        = true;
    packet_dma_data->current_buffer_index   = 0;
    packet_dma_data->cur_desc_addr          = packet_dma_data->desc_addr;
    cmicx_log(" successfully started packet dma access.");
    cmicx_log_newline();
    return true;
}


/* aborts a packet dma access for a given packet dma module */
bool cmic_packet_dma_access_abort(cmicx_packet_dma_data_t *packet_dma_data) {

    /* if we have an access running, abort it and log a message, otherwise, just log a message that we could not abort. */
    if ((packet_dma_data->chain_pending==true) && (packet_dma_data->ctrl_abort_dma==false)) {
        packet_dma_data->chain_pending   = false;
        packet_dma_data->ctrl_abort_dma  = true;
        packet_dma_data->stat_dma_active = false;
        cmicx_log("packet dma access on \"");
        cmicx_log(packet_dma_data->id_string);
        cmicx_log("\" successfully aborted.");
        cmicx_log_newline();
        return true;
    }
    else {
        cmicx_log("could not abort packet dma access on \"");
        cmicx_log(packet_dma_data->id_string);
        cmicx_log("\", no access currently pending or abort has already happened.\n");
        return false;
    }

}



/**********************************************/
/* packet dma register access support functions */
/**********************************************/

/* request register read helper function */
bool cmic_pktdma_intr_coal_read(reg_t *val,cmicx_packet_dma_data_t *packet_dma_data) {
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_INTR_COAL_REG_ENABLE_BIT,                                   packet_dma_data->intr_coal_enable);
    REG_FIELD_SET_TO(*val,CMIC_PKTDMA_INTR_COAL_REG_COUNT_MSB,CMIC_PKTDMA_INTR_COAL_REG_COUNT_LSB,packet_dma_data->intr_coal_count);
    REG_FIELD_SET_TO(*val,CMIC_PKTDMA_INTR_COAL_REG_TIMER_MSB,CMIC_PKTDMA_INTR_COAL_REG_TIMER_LSB,packet_dma_data->intr_coal_timer);
    return true;
}

/* request register write helper function */
bool cmic_pktdma_intr_coal_write(reg_t val,cmicx_packet_dma_data_t *packet_dma_data) {
    packet_dma_data->intr_coal_enable = REG_BIT_GET(  val,CMIC_PKTDMA_INTR_COAL_REG_ENABLE_BIT);
    packet_dma_data->intr_coal_count  = REG_FIELD_GET(val,CMIC_PKTDMA_INTR_COAL_REG_COUNT_MSB,CMIC_PKTDMA_INTR_COAL_REG_COUNT_LSB);
    packet_dma_data->intr_coal_timer  = REG_FIELD_GET(val,CMIC_PKTDMA_INTR_COAL_REG_TIMER_MSB,CMIC_PKTDMA_INTR_COAL_REG_TIMER_LSB);
    return true;
}

/* control register read support function */
bool cmic_pktdma_ctrl_read(reg_t *val,cmicx_packet_dma_data_t *packet_dma_data) {

    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_DIRECTION_BIT,                                                                          packet_dma_data->ctrl_direction);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT,                                                                             packet_dma_data->ctrl_dma_en);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_ABORT_DMA_BIT,                                                                          packet_dma_data->ctrl_abort_dma);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_PKTDMA_ENDIANNESS_BIT,                                                                  packet_dma_data->ctrl_pktdma_endianness);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_DESC_ENDIANNESS_BIT,                                                                    packet_dma_data->ctrl_desc_endianness);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_DROP_RX_PKT_ON_CHAIN_END_BIT,                                                           packet_dma_data->ctrl_drop_rx_pkt_on_chain_end);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_RLD_STS_UPD_DIS_BIT,                                                                    packet_dma_data->ctrl_rld_sts_upd_dis);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_DESC_DONE_INTR_MODE_BIT,                                                                packet_dma_data->ctrl_desc_done_intr_mode);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_ENABLE_CONTINUOUS_DMA_BIT,                                                              packet_dma_data->ctrl_enable_continuous_dma);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_CONTIGUOUS_DESCRIPTORS_BIT,                                                             packet_dma_data->ctrl_contiguous_descriptors);
    REG_FIELD_SET_TO(*val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_LSB, packet_dma_data->ctrl_ep_to_cpu_header_size_dwords);
    REG_FIELD_SET_TO(*val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_LSB,       packet_dma_data->ctrl_ep_to_cpu_alignment_bytes);
    REG_BIT_SET_TO(  *val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_ENDIANNESS_BIT,                                                        packet_dma_data->ctrl_ep_to_cpu_header_endianness);
    return true;

}


/* control register write support function */
bool cmic_pktdma_ctrl_write(reg_t val,cmicx_packet_dma_data_t *packet_dma_data) {

    bool return_value = true;

    /* grab new bits/fields from reg value write */
    bool new_ctrl_direction                    = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_DIRECTION_BIT               );
    bool new_ctrl_dma_en                       = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_DMA_EN_BIT                  );
    bool new_ctrl_abort_dma                    = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_ABORT_DMA_BIT               );
    bool new_ctrl_pktdma_endianness            = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_PKTDMA_ENDIANNESS_BIT       );
    bool new_ctrl_desc_endianness              = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_DESC_ENDIANNESS_BIT         );
    bool new_ctrl_drop_rx_pkt_on_chain_end     = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_DROP_RX_PKT_ON_CHAIN_END_BIT);
    bool new_ctrl_rld_sts_upd_dis              = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_RLD_STS_UPD_DIS_BIT         );
    bool new_ctrl_desc_done_intr_mode          = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_DESC_DONE_INTR_MODE_BIT     );
    bool new_ctrl_enable_continuous_dma        = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_ENABLE_CONTINUOUS_DMA_BIT   );
    bool new_ctrl_contiguous_descriptors       = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_CONTIGUOUS_DESCRIPTORS_BIT  );
    int  new_ctrl_ep_to_cpu_header_size_dwords = REG_FIELD_GET(val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_SIZE_DWORDS_LSB);
    int  new_ctrl_ep_to_cpu_alignment_bytes    = REG_FIELD_GET(val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_MSB,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_ALIGNMENT_BYTES_LSB);
    bool new_ctrl_ep_to_cpu_header_endianness  = REG_BIT_GET(  val,CMIC_PKTDMA_CTRL_REG_EP_TO_CPU_HEADER_ENDIANNESS_BIT);

    /* check that we are not trying to change anything that we cannot while we already have a chain pending */
    if (packet_dma_data->chain_pending==true) {

        if (new_ctrl_direction!=packet_dma_data->ctrl_direction) {
            cmicx_log("attempting to change packet dma direction on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_pktdma_endianness!=packet_dma_data->ctrl_pktdma_endianness) {
            cmicx_log("attempting to change packet dma endianness on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_desc_endianness!=packet_dma_data->ctrl_desc_endianness) {
            cmicx_log("attempting to change packet dma desc endianness on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_desc_done_intr_mode!=packet_dma_data->ctrl_desc_done_intr_mode) {
            cmicx_log("attempting to change packet dma descriptor done interrupt mode on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_ep_to_cpu_header_endianness!=packet_dma_data->ctrl_ep_to_cpu_header_endianness) {
            cmicx_log("attempting to change packet dma ep to cpu [packet header] endianness on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_ep_to_cpu_header_size_dwords!=packet_dma_data->ctrl_ep_to_cpu_header_size_dwords) {
            cmicx_log("attempting to change packet dma header size dwords value on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }
        if (new_ctrl_ep_to_cpu_alignment_bytes!=packet_dma_data->ctrl_ep_to_cpu_alignment_bytes) {
            cmicx_log("attempting to change packet dma alignment bytes value on ");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log(" while packet dma is running. failure.\n");
            return_value = false;
        }

    }
    else {
        packet_dma_data->ctrl_direction                    = new_ctrl_direction;
        packet_dma_data->ctrl_pktdma_endianness            = new_ctrl_pktdma_endianness;
        packet_dma_data->ctrl_desc_endianness              = new_ctrl_desc_endianness;
        packet_dma_data->ctrl_drop_rx_pkt_on_chain_end     = new_ctrl_drop_rx_pkt_on_chain_end;
        packet_dma_data->ctrl_rld_sts_upd_dis              = new_ctrl_rld_sts_upd_dis;
        packet_dma_data->ctrl_desc_done_intr_mode          = new_ctrl_desc_done_intr_mode;
        packet_dma_data->ctrl_enable_continuous_dma        = new_ctrl_enable_continuous_dma;
        packet_dma_data->ctrl_contiguous_descriptors       = new_ctrl_contiguous_descriptors;
        packet_dma_data->ctrl_ep_to_cpu_header_size_dwords = new_ctrl_ep_to_cpu_header_size_dwords;
        packet_dma_data->ctrl_ep_to_cpu_alignment_bytes    = new_ctrl_ep_to_cpu_alignment_bytes;
        packet_dma_data->ctrl_ep_to_cpu_header_endianness  = new_ctrl_ep_to_cpu_header_endianness;
    }

    /* start a new access or abort if requested */
    if (new_ctrl_dma_en==true) {
        if (cmic_packet_dma_access_start(packet_dma_data)==false) {
            return_value = false;
        }
    }
    if (new_ctrl_abort_dma==true) {
        if (cmic_packet_dma_access_abort(packet_dma_data)==false) {
            return_value = false;
        }
    }
    else {
        packet_dma_data->ctrl_abort_dma = false;
    }

    /* if we are trying to clear the dma enable bit */
    if (new_ctrl_dma_en==false) {

        /* if the chain done bit is high, then we can clear our dma enable bit, our statuses, and our errors successfully. */
        if ((packet_dma_data->stat_chain_done==true) && (packet_dma_data->chain_pending==false)) {
            packet_dma_data->ctrl_dma_en                  = false;
            packet_dma_data->stat_chain_done              = false;
            packet_dma_data->stat_dma_active              = false;
            packet_dma_data->stat_stwt_addr_decode_err    = false;
            packet_dma_data->stat_pktwrrd_addr_decode_err = false;
            packet_dma_data->stat_descrd_addr_decode_err  = false;
            packet_dma_data->stat_pktwr_ecc_err           = false;
        }

        /* invalid case for clearing dma enable bit */
        else {
            cmicx_log("trying to clear dma enable bit for \"");
            cmicx_log(packet_dma_data->id_string);
            cmicx_log("\": chain done bit is not high, or chain is still pending. failure.\n");
            return_value = false;
        }

    }

    return return_value;
}



/* status register read support function */
bool cmic_pktdma_stat_read(reg_t *val,cmicx_packet_dma_data_t *packet_dma_data) {

    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_CHAIN_DONE_BIT,              packet_dma_data->stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_DMA_ACTIVE_BIT,              packet_dma_data->stat_dma_active);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_STWT_ADDR_DECODE_ERR_BIT,    packet_dma_data->stat_stwt_addr_decode_err);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_PKTWRRD_ADDR_DECODE_ERR_BIT, packet_dma_data->stat_pktwrrd_addr_decode_err);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_DESCRD_ADDR_DECODE_ERR_BIT,  packet_dma_data->stat_descrd_addr_decode_err);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_PKTWR_ECC_ERR_BIT,           packet_dma_data->stat_pktwr_ecc_err);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_STAT_REG_CH_IN_HALT_BIT,              packet_dma_data->stat_ch_in_halt);
    return true;

}

/* request register read helper function */
bool cmic_pktdma_shared_irq_stat0_read(reg_t *val,cmicx_cmc_t *cmc) {
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_DONE_BIT,            cmc->packet_dma[0].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_CHAIN_DONE_BIT,           cmc->packet_dma[0].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_INTR_COALESCING_INTR_BIT, cmc->packet_dma[0].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[0].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_DONE_BIT,            cmc->packet_dma[1].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_CHAIN_DONE_BIT,           cmc->packet_dma[1].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_INTR_COALESCING_INTR_BIT, cmc->packet_dma[1].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[1].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_DONE_BIT,            cmc->packet_dma[2].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_CHAIN_DONE_BIT,           cmc->packet_dma[2].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_INTR_COALESCING_INTR_BIT, cmc->packet_dma[2].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[2].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_DONE_BIT,            cmc->packet_dma[3].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_CHAIN_DONE_BIT,           cmc->packet_dma[3].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_INTR_COALESCING_INTR_BIT, cmc->packet_dma[3].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[3].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_DONE_BIT,            cmc->packet_dma[4].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_CHAIN_DONE_BIT,           cmc->packet_dma[4].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_INTR_COALESCING_INTR_BIT, cmc->packet_dma[4].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[4].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_DONE_BIT,            cmc->packet_dma[5].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_CHAIN_DONE_BIT,           cmc->packet_dma[5].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_INTR_COALESCING_INTR_BIT, cmc->packet_dma[5].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[5].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_DONE_BIT,            cmc->packet_dma[6].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_CHAIN_DONE_BIT,           cmc->packet_dma[6].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_INTR_COALESCING_INTR_BIT, cmc->packet_dma[6].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[6].irq_stat_desc_controlled_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_DONE_BIT,            cmc->packet_dma[7].irq_stat_desc_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_CHAIN_DONE_BIT,           cmc->packet_dma[7].irq_stat_chain_done);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_INTR_COALESCING_INTR_BIT, cmc->packet_dma[7].irq_stat_intr_coalescing_intr);
    REG_BIT_SET_TO(*val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_CONTROLLED_INTR_BIT, cmc->packet_dma[7].irq_stat_desc_controlled_intr);
    return true;
}

/* request register write helper function */
bool cmic_pktdma_shared_irq_stat_clr0_write(reg_t val,cmicx_cmc_t *cmc) {
    cmc->packet_dma[0].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_DONE_BIT           );
    cmc->packet_dma[0].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_CHAIN_DONE_BIT          );
    cmc->packet_dma[0].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[0].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH0_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[1].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_DONE_BIT           );
    cmc->packet_dma[1].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_CHAIN_DONE_BIT          );
    cmc->packet_dma[1].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[1].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH1_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[2].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_DONE_BIT           );
    cmc->packet_dma[2].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_CHAIN_DONE_BIT          );
    cmc->packet_dma[2].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[2].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH2_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[3].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_DONE_BIT           );
    cmc->packet_dma[3].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_CHAIN_DONE_BIT          );
    cmc->packet_dma[3].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[3].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH3_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[4].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_DONE_BIT           );
    cmc->packet_dma[4].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_CHAIN_DONE_BIT          );
    cmc->packet_dma[4].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[4].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH4_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[5].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_DONE_BIT           );
    cmc->packet_dma[5].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_CHAIN_DONE_BIT          );
    cmc->packet_dma[5].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[5].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH5_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[6].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_DONE_BIT           );
    cmc->packet_dma[6].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_CHAIN_DONE_BIT          );
    cmc->packet_dma[6].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[6].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH6_DESC_CONTROLLED_INTR_BIT);
    cmc->packet_dma[7].irq_stat_desc_done            = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_DONE_BIT           );
    cmc->packet_dma[7].irq_stat_chain_done           = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_CHAIN_DONE_BIT          );
    cmc->packet_dma[7].irq_stat_intr_coalescing_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_INTR_COALESCING_INTR_BIT);
    cmc->packet_dma[7].irq_stat_desc_controlled_intr = REG_BIT_GET(val,CMIC_PKTDMA_SHARED_IRQ_STAT0_REG_CH7_DESC_CONTROLLED_INTR_BIT);
    return true;
}

/* cos control rx 0 read helper function */
bool cmic_pktdma_cos_ctrl_rx_0_read(reg_t *val,cmicx_packet_dma_data_t *packet_dma_data) {
    int i;
    for (i=0; i<32; i++) {
        REG_BIT_SET_TO(*val,i,packet_dma_data->cos_ctrl_bmp[i]);
    }
    return true;
}

/* cos control rx 0 write helper function */
bool cmic_pktdma_cos_ctrl_rx_0_write(reg_t val,cmicx_packet_dma_data_t *packet_dma_data) {
    int i;
    for (i=0; i<32; i++) {
        packet_dma_data->cos_ctrl_bmp[i] = REG_BIT_GET(val,i);
    }
    return true;
}

/* cos control rx 1 read helper function */
bool cmic_pktdma_cos_ctrl_rx_1_read(reg_t *val,cmicx_packet_dma_data_t *packet_dma_data) {
    int i;
    for (i=0; i<32; i++) {
        REG_BIT_SET_TO(*val,i,packet_dma_data->cos_ctrl_bmp[i+32]);
    }
    return true;
}

/* cos control rx 1 write helper function */
bool cmic_pktdma_cos_ctrl_rx_1_write(reg_t val,cmicx_packet_dma_data_t *packet_dma_data) {
    int i;
    for (i=0; i<32; i++) {
        packet_dma_data->cos_ctrl_bmp[i+32] = REG_BIT_GET(val,i);
    }
    return true;
}



/******************************************************/
/* define packet dma register direct access functions */
/******************************************************/

/* top-level cmic registers */
bool cmic_top_config_read(reg_t *val) {
    REG_BIT_SET_TO(*val,CMIC_TOP_CONFIG_REG_ENABLE_PIO_WRITE_NON_POSTED_BIT,cmicx->enable_pio_write_non_posted);
    REG_BIT_SET_TO(*val,CMIC_TOP_CONFIG_REG_IP_2_EP_LOOPBACK_ENABLE_BIT,    cmicx->ip_2_ep_loopback_enable);
    return true;
}
bool cmic_top_config_write(reg_t val) {
    cmicx->enable_pio_write_non_posted = REG_BIT_GET(val,CMIC_TOP_CONFIG_REG_ENABLE_PIO_WRITE_NON_POSTED_BIT);
    cmicx->ip_2_ep_loopback_enable     = REG_BIT_GET(val,CMIC_TOP_CONFIG_REG_IP_2_EP_LOOPBACK_ENABLE_BIT);
    cmicx_log("setting hardware loopback enable to: ");
    cmicx_log_int((int)cmicx->ip_2_ep_loopback_enable);
    cmicx_log_newline();
    return true;
}
PCID_REG_READ_NULL_FUNC_GEN(  cmic_top_epintf_release_all_credits_read)
bool cmic_top_epintf_release_all_credits_write(reg_t val) {
    /*
    cmicx->val = 0;
    */
    return true;
}

/* cmc0 shared stat registers */
bool cmic_cmc0_shared_irq_stat0_read(reg_t *val) {
    return cmic_pktdma_shared_irq_stat0_read(val,&cmicx->cmc0);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_shared_irq_stat0_write )
PCID_REG_READ_NULL_FUNC_GEN(  cmic_cmc0_shared_irq_stat_clr0_read  )
bool cmic_cmc0_shared_irq_stat_clr0_write(reg_t val) {
    return cmic_pktdma_shared_irq_stat_clr0_write(val,&cmicx->cmc0);
}

/* cmc0 packet dma ch0 */
bool cmic_cmc0_pktdma_ch0_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[0]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch0_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch0_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch0_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch0_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[0].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch0_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch0_stat_write )
bool cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[0]);
}
bool cmic_cmc0_pktdma_ch0_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[0]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch0_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[0].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch0_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch0_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[0].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch0_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[0].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch0_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[0].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch0_pkt_count_txpkt_write)

/* cmc0 packet dma ch1 */
bool cmic_cmc0_pktdma_ch1_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[1]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch1_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch1_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch1_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch1_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[1].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch1_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch1_stat_write )
bool cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[1]);
}
bool cmic_cmc0_pktdma_ch1_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[1]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch1_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[1].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch1_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch1_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[1].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch1_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[1].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch1_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[1].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch1_pkt_count_txpkt_write)

/* cmc0 packet dma ch2*/
bool cmic_cmc0_pktdma_ch2_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[2]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch2_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch2_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch2_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch2_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[2].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch2_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch2_stat_write )
bool cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[2]);
}
bool cmic_cmc0_pktdma_ch2_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[2]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch2_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[2].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch2_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch2_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[2].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch2_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[2].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch2_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[2].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch2_pkt_count_txpkt_write)

/* cmc0 packet dma ch3 */
bool cmic_cmc0_pktdma_ch3_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[3]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch3_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch3_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch3_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch3_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[3].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch3_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch3_stat_write )
bool cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[3]);
}
bool cmic_cmc0_pktdma_ch3_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[3]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch3_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[3].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch3_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch3_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[3].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch3_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[3].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch3_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[3].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch3_pkt_count_txpkt_write)

/* cmc0 packet dma ch4 */
bool cmic_cmc0_pktdma_ch4_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[4]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch4_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch4_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch4_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch4_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[4].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch4_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[4]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch4_stat_write )
bool cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[4]);
}
bool cmic_cmc0_pktdma_ch4_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[4]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch4_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[4].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch4_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch4_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[4].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch4_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[4].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch4_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[4].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch4_pkt_count_txpkt_write)

/* cmc0 packet dma ch5 */
bool cmic_cmc0_pktdma_ch5_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[5]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch5_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch5_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch5_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch5_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[5].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch5_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[5]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch5_stat_write )
bool cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[5]);
}
bool cmic_cmc0_pktdma_ch5_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[5]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch5_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[5].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch5_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch5_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[5].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch5_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[5].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch5_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[5].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch5_pkt_count_txpkt_write)

/* cmc0 packet dma ch6 */
bool cmic_cmc0_pktdma_ch6_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[6]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch6_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch6_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch6_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch6_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[6].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch6_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[6]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch6_stat_write )
bool cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[6]);
}
bool cmic_cmc0_pktdma_ch6_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[6]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch6_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[6].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch6_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch6_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[6].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch6_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[6].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch6_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[6].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch6_pkt_count_txpkt_write)

/* cmc0 packet dma ch7 */
bool cmic_cmc0_pktdma_ch7_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc0.packet_dma[7]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch7_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch7_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch7_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch7_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[7].desc_halt_addr) )
bool cmic_cmc0_pktdma_ch7_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc0.packet_dma[7]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch7_stat_write )
bool cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc0.packet_dma[7]);
}
bool cmic_cmc0_pktdma_ch7_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc0.packet_dma[7]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch7_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc0.packet_dma[7].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch7_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch7_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc0.packet_dma[7].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch7_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_read,                           cmicx->cmc0.packet_dma[7].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc0_pktdma_ch7_pkt_count_txpkt_read,                           cmicx->cmc0.packet_dma[7].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc0_pktdma_ch7_pkt_count_txpkt_write)

/* cmc1 shared stat registers */
bool cmic_cmc1_shared_irq_stat0_read(reg_t *val) {
    return cmic_pktdma_shared_irq_stat0_read(val,&cmicx->cmc1);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_shared_irq_stat0_write )
PCID_REG_READ_NULL_FUNC_GEN(  cmic_cmc1_shared_irq_stat_clr0_read  )
bool cmic_cmc1_shared_irq_stat_clr0_write(reg_t val) {
    return cmic_pktdma_shared_irq_stat_clr0_write(val,&cmicx->cmc1);
}

/* cmc1 packet dma ch0 */
bool cmic_cmc1_pktdma_ch0_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[0]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch0_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch0_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch0_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch0_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[0].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch0_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[0]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch0_stat_write )
bool cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[0]);
}
bool cmic_cmc1_pktdma_ch0_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[0]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch0_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[0].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch0_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch0_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[0].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch0_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[0].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch0_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[0].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch0_pkt_count_txpkt_write)

/* cmc1 packet dma ch1 */
bool cmic_cmc1_pktdma_ch1_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[1]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch1_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch1_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch1_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch1_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[1].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch1_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[1]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch1_stat_write )
bool cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[1]);
}
bool cmic_cmc1_pktdma_ch1_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[1]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch1_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[1].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch1_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch1_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[1].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch1_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[1].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch1_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[1].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch1_pkt_count_txpkt_write)

/* cmc1 packet dma ch2 */
bool cmic_cmc1_pktdma_ch2_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[2]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch2_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch2_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch2_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch2_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[2].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch2_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[2]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch2_stat_write )
bool cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[2]);
}
bool cmic_cmc1_pktdma_ch2_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[2]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch2_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[2].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch2_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch2_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[2].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch2_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[2].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch2_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[2].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch2_pkt_count_txpkt_write)

/* cmc1 packet dma ch3 */
bool cmic_cmc1_pktdma_ch3_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[3]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch3_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch3_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch3_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch3_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[3].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch3_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[3]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch3_stat_write )
bool cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[3]);
}
bool cmic_cmc1_pktdma_ch3_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[3]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch3_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[3].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch3_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch3_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[3].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch3_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[3].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch3_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[3].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch3_pkt_count_txpkt_write)

/* cmc0 packet dma ch4 */
bool cmic_cmc1_pktdma_ch4_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[4]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch4_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch4_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch4_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch4_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[4].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch4_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[4]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch4_stat_write )
bool cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[4]);
}
bool cmic_cmc1_pktdma_ch4_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[4]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch4_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[4].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch4_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch4_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[4].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch4_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[4].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch4_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[4].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch4_pkt_count_txpkt_write)

/* cmc1 packet dma ch5 */
bool cmic_cmc1_pktdma_ch5_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[5]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch5_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch5_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch5_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch5_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[5].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch5_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[5]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch5_stat_write )
bool cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[5]);
}
bool cmic_cmc1_pktdma_ch5_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[5]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch5_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[5].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch5_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch5_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[5].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch5_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[5].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch5_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[5].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch5_pkt_count_txpkt_write)

/* cmc1 packet dma ch6 */
bool cmic_cmc1_pktdma_ch6_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[6]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch6_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch6_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch6_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch6_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[6].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch6_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[6]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch6_stat_write )
bool cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[6]);
}
bool cmic_cmc1_pktdma_ch6_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[6]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch6_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[6].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch6_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch6_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[6].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch6_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[6].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch6_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[6].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch6_pkt_count_txpkt_write)

/* cmc1 packet dma ch7 */
bool cmic_cmc1_pktdma_ch7_ctrl_read(reg_t *val) {
    return cmic_pktdma_ctrl_read(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_ctrl_write(reg_t val) {
    return cmic_pktdma_ctrl_write(val,&cmicx->cmc1.packet_dma[7]);
}
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch7_desc_addr_lo_read,          UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch7_desc_addr_lo_write,         UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch7_desc_addr_hi_read,          UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch7_desc_addr_hi_write,         UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_write,    UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_halt_addr) )
PCID_REG_READ_FUNC_GEN(  cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_halt_addr) )
PCID_REG_WRITE_FUNC_GEN( cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_write,    UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[7].desc_halt_addr) )
bool cmic_cmc1_pktdma_ch7_stat_read(reg_t *val) {
    return cmic_pktdma_stat_read(val,&cmicx->cmc1.packet_dma[7]);
}
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch7_stat_write )
bool cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_0_read(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_0_write(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_read(reg_t *val) {
    return cmic_pktdma_cos_ctrl_rx_1_read(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_write(reg_t val) {
    return cmic_pktdma_cos_ctrl_rx_1_write(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_intr_coal_read(reg_t *val) {
    return cmic_pktdma_intr_coal_read(val,&cmicx->cmc1.packet_dma[7]);
}
bool cmic_cmc1_pktdma_ch7_intr_coal_write(reg_t val) {
    return cmic_pktdma_intr_coal_write(val,&cmicx->cmc1.packet_dma[7]);
}
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch7_curr_desc_lo_read,     UINT64_LOWER32_REFERENCE(cmicx->cmc1.packet_dma[7].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch7_curr_desc_lo_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch7_curr_desc_hi_read,     UINT64_UPPER32_REFERENCE(cmicx->cmc1.packet_dma[7].cur_desc_addr) )
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch7_curr_desc_hi_write )
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_read,                           cmicx->cmc1.packet_dma[7].rxpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_write)
PCID_REG_READ_FUNC_GEN(       cmic_cmc1_pktdma_ch7_pkt_count_txpkt_read,                           cmicx->cmc1.packet_dma[7].txpkt_count)
PCID_REG_WRITE_NULL_FUNC_GEN( cmic_cmc1_pktdma_ch7_pkt_count_txpkt_write)


