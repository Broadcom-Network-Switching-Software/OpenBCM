/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim.c
 *
 * cmicx module main implementation
 */

#ifndef CMICX_STANDALONE_SIM
 #include <unistd.h>
 #include <../systems/bde/pli/verinet.h>
 #include <../systems/sim/pcid/pcid.h>
 #include <../systems/sim/pcid/cmicx_sim.h>
 #include <../systems/sim/pcid/cmicx_sim_pcid.h>
 #include <../systems/sim/pcid/cmicx_sim_schan.h>
 #include <../systems/sim/pcid/cmicx_sim_sbus_dma.h>
 #include <../systems/sim/pcid/cmicx_sim_packet_dma.h>
#else
 #include <stdio.h>
 #include <stdlib.h>
 #include "cmicx_sim.h"
 #include "cmicx_sim_pcid.h"
 #include "cmicx_sim_schan.h"
 #include "cmicx_sim_sbus_dma.h"
 #include "cmicx_sim_packet_dma.h"
#endif
#include <string.h>
#include <time.h>


cmicx_t cmicx_data;
/* global pointer for cmicx structure */
cmicx_t *cmicx = &cmicx_data;

/* initialize the cmicx module */
bool cmicx_init(void) {

    /* variables */
    int i;
    char id_string[CMIC_SBUS_DMA_ID_STRING_LENGTH];

    /* initialize logging system */
    if (cmicx_logging_init("CMICX_SIM_LOG.log")==false) {
        cmicx_log("ERROR: Could not open log file for logging CMICx sim output.");
    }
    cmicx_log_newline();
    cmicx_log("Initializing CMICx simulation...");
    cmicx_log_newline();


    /* initialize schan data */
    strcpy(id_string,"common schan 0  ");
    for (i=0; i<CMICX_NUM_SCHAN_IN_COMMON; i++) {
        id_string[13] = ((char)(i+48));   /* add 48 to get the ascii code for the schan number */
        cmicx_schan_init(&cmicx->common.schan[i], id_string);
    }


    /* initialize schan fifo data */
    /* TBD */


    /* intialize cmcs, in preparation for sbus dma init */
    strcpy(cmicx->cmc0.id_string,"cmc0");
    cmicx->cmc0.enable_early_bresp = false;
    strcpy(cmicx->cmc1.id_string,"cmc1");
    cmicx->cmc1.enable_early_bresp = false;

    /* intialize sbus dma data */
    strcpy(id_string,"cmc0 sbus dma ch0  ");
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        id_string[16] = ((char)(i+48));   /* add 48 to get the ascii code for the sbus dma channel number */
        cmicx_sbus_dma_init(&cmicx->cmc0.sbus_dma[i], id_string);
    }
    strcpy(id_string,"cmc1 sbus dma ch0  ");
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        id_string[16] = ((char)(i+48));   /* add 48 to get the ascii code for the sbus dma channel number */
        cmicx_sbus_dma_init(&cmicx->cmc1.sbus_dma[i], id_string);
    }


    /* initialize packet dma. the global init takes care of opening input/output files for packet data */
    cmicx_packet_dma_global_init("CMICX_SIM_IP_PACKETS.out","CMICX_SIM_EP_PACKETS.in","CMICX_SIM_LOOPBACK_PACKETS.inout");
    strcpy(id_string,"cmc0 packet dma ch0  ");
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        id_string[18] = ((char)(i+48));   /* add 48 to get the ascii code for the [packet dma] number */
        cmicx_packet_dma_init(&cmicx->cmc0.packet_dma[i], id_string, i);
    }
    strcpy(id_string,"cmc1 packet dma ch0  ");
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        id_string[18] = ((char)(i+48));   /* add 48 to get the ascii code for the [packet dma] number */
        cmicx_packet_dma_init(&cmicx->cmc1.packet_dma[i], id_string, i);
    }


    /* initialize the PCID register counter and memory, and the switch register count and memory count */
    cmicx->last_pcid_register             = -1;
    cmicx->last_pcid_memory_address_index = -1;
    cmicx->last_switch_register           = -1;
    cmicx->last_switch_memory             = -1;

    /* initialize schan functionality regs. weights are 0xE by default, per the spec. */
    cmicx->enable_pio_write_non_posted = false;
    for (i=0; i<CMICX_NUM_SCHAN_IN_COMMON; i++) {
        cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[i] = 0xE;
    }
    cmicx->sbus_ring_arb_ctrl_schan_common_fifo_weight = 0xE;
    cmicx->sbus_ring_map_0_7_reg   = 0x0;
    cmicx->sbus_ring_map_8_15_reg  = 0x0;
    cmicx->sbus_ring_map_16_23_reg = 0x0;
    cmicx->sbus_ring_map_24_31_reg = 0x0;
    cmicx->sbus_ring_map_32_39_reg = 0x0;
    cmicx->sbus_ring_map_40_47_reg = 0x0;
    cmicx->sbus_ring_map_48_55_reg = 0x0;
    cmicx->sbus_ring_map_56_63_reg = 0x0;
    cmicx->sbus_timeout_reg        = 0x00002700;

    /* initialize sbus dma functionality variables. weights are 0xE by default, per the spec. */
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[i] = 0xE;
    }
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[i] = 0xE;
    }

    /* initialize packet dma functionality registers. */
    cmicx->ip_2_ep_loopback_enable = false;


    /* add cmicx sim control register */
    if (! cmicx_pcid_register_add(CMICX_SIM_CONTROL_REG_ADDR, cmicx_sim_control_reg_read, cmicx_sim_control_reg_write) ) {
        cmicx_error("could not initialize cmicx sim control register\n");
    }
    /* add pcid schan registers */
    if (! ( cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_ARB_CTRL_SCHAN_REG_ADDR,              cmic_top_sbus_ring_arb_ctrl_schan_read,               cmic_top_sbus_ring_arb_ctrl_schan_write              ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_TIMEOUT_REG_ADDR,                          cmic_top_sbus_timeout_read,                           cmic_top_sbus_timeout_write                          ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_0_7_REG_ADDR,                     cmic_top_sbus_ring_map_0_7_read,                      cmic_top_sbus_ring_map_0_7_write                     ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_8_15_REG_ADDR,                    cmic_top_sbus_ring_map_8_15_read,                     cmic_top_sbus_ring_map_8_15_write                    ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_16_23_REG_ADDR,                   cmic_top_sbus_ring_map_16_23_read,                    cmic_top_sbus_ring_map_16_23_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_24_31_REG_ADDR,                   cmic_top_sbus_ring_map_24_31_read,                    cmic_top_sbus_ring_map_24_31_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_32_39_REG_ADDR,                   cmic_top_sbus_ring_map_32_39_read,                    cmic_top_sbus_ring_map_32_39_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_40_47_REG_ADDR,                   cmic_top_sbus_ring_map_40_47_read,                    cmic_top_sbus_ring_map_40_47_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_48_55_REG_ADDR,                   cmic_top_sbus_ring_map_48_55_read,                    cmic_top_sbus_ring_map_48_55_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_56_63_REG_ADDR,                   cmic_top_sbus_ring_map_56_63_read,                    cmic_top_sbus_ring_map_56_63_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_64_71_REG_ADDR,                   cmic_top_sbus_ring_map_64_71_read,                    cmic_top_sbus_ring_map_64_71_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_72_79_REG_ADDR,                   cmic_top_sbus_ring_map_72_79_read,                    cmic_top_sbus_ring_map_72_79_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_80_87_REG_ADDR,                   cmic_top_sbus_ring_map_80_87_read,                    cmic_top_sbus_ring_map_80_87_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_88_95_REG_ADDR,                   cmic_top_sbus_ring_map_88_95_read,                    cmic_top_sbus_ring_map_88_95_write                   ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_96_103_REG_ADDR,                  cmic_top_sbus_ring_map_96_103_read,                   cmic_top_sbus_ring_map_96_103_write                  ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_104_111_REG_ADDR,                 cmic_top_sbus_ring_map_104_111_read,                  cmic_top_sbus_ring_map_104_111_write                 ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_112_119_REG_ADDR,                 cmic_top_sbus_ring_map_112_119_read,                  cmic_top_sbus_ring_map_112_119_write                 ) &&
            cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_MAP_120_127_REG_ADDR,                 cmic_top_sbus_ring_map_120_127_read,                  cmic_top_sbus_ring_map_120_127_write                 ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE0_REG_ADDR,            cmic_common_pool_schan_ch0_message0_read,             cmic_common_pool_schan_ch0_message0_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE1_REG_ADDR,            cmic_common_pool_schan_ch0_message1_read,             cmic_common_pool_schan_ch0_message1_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE2_REG_ADDR,            cmic_common_pool_schan_ch0_message2_read,             cmic_common_pool_schan_ch0_message2_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE3_REG_ADDR,            cmic_common_pool_schan_ch0_message3_read,             cmic_common_pool_schan_ch0_message3_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE4_REG_ADDR,            cmic_common_pool_schan_ch0_message4_read,             cmic_common_pool_schan_ch0_message4_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE5_REG_ADDR,            cmic_common_pool_schan_ch0_message5_read,             cmic_common_pool_schan_ch0_message5_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE6_REG_ADDR,            cmic_common_pool_schan_ch0_message6_read,             cmic_common_pool_schan_ch0_message6_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE7_REG_ADDR,            cmic_common_pool_schan_ch0_message7_read,             cmic_common_pool_schan_ch0_message7_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE8_REG_ADDR,            cmic_common_pool_schan_ch0_message8_read,             cmic_common_pool_schan_ch0_message8_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE9_REG_ADDR,            cmic_common_pool_schan_ch0_message9_read,             cmic_common_pool_schan_ch0_message9_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE10_REG_ADDR,           cmic_common_pool_schan_ch0_message10_read,            cmic_common_pool_schan_ch0_message10_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE11_REG_ADDR,           cmic_common_pool_schan_ch0_message11_read,            cmic_common_pool_schan_ch0_message11_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE12_REG_ADDR,           cmic_common_pool_schan_ch0_message12_read,            cmic_common_pool_schan_ch0_message12_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE13_REG_ADDR,           cmic_common_pool_schan_ch0_message13_read,            cmic_common_pool_schan_ch0_message13_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE14_REG_ADDR,           cmic_common_pool_schan_ch0_message14_read,            cmic_common_pool_schan_ch0_message14_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE15_REG_ADDR,           cmic_common_pool_schan_ch0_message15_read,            cmic_common_pool_schan_ch0_message15_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE16_REG_ADDR,           cmic_common_pool_schan_ch0_message16_read,            cmic_common_pool_schan_ch0_message16_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE17_REG_ADDR,           cmic_common_pool_schan_ch0_message17_read,            cmic_common_pool_schan_ch0_message17_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE18_REG_ADDR,           cmic_common_pool_schan_ch0_message18_read,            cmic_common_pool_schan_ch0_message18_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE19_REG_ADDR,           cmic_common_pool_schan_ch0_message19_read,            cmic_common_pool_schan_ch0_message19_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE20_REG_ADDR,           cmic_common_pool_schan_ch0_message20_read,            cmic_common_pool_schan_ch0_message20_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_MESSAGE21_REG_ADDR,           cmic_common_pool_schan_ch0_message21_read,            cmic_common_pool_schan_ch0_message21_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE0_REG_ADDR,            cmic_common_pool_schan_ch1_message0_read,             cmic_common_pool_schan_ch1_message0_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE1_REG_ADDR,            cmic_common_pool_schan_ch1_message1_read,             cmic_common_pool_schan_ch1_message1_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE2_REG_ADDR,            cmic_common_pool_schan_ch1_message2_read,             cmic_common_pool_schan_ch1_message2_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE3_REG_ADDR,            cmic_common_pool_schan_ch1_message3_read,             cmic_common_pool_schan_ch1_message3_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE4_REG_ADDR,            cmic_common_pool_schan_ch1_message4_read,             cmic_common_pool_schan_ch1_message4_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE5_REG_ADDR,            cmic_common_pool_schan_ch1_message5_read,             cmic_common_pool_schan_ch1_message5_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE6_REG_ADDR,            cmic_common_pool_schan_ch1_message6_read,             cmic_common_pool_schan_ch1_message6_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE7_REG_ADDR,            cmic_common_pool_schan_ch1_message7_read,             cmic_common_pool_schan_ch1_message7_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE8_REG_ADDR,            cmic_common_pool_schan_ch1_message8_read,             cmic_common_pool_schan_ch1_message8_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE9_REG_ADDR,            cmic_common_pool_schan_ch1_message9_read,             cmic_common_pool_schan_ch1_message9_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE10_REG_ADDR,           cmic_common_pool_schan_ch1_message10_read,            cmic_common_pool_schan_ch1_message10_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE11_REG_ADDR,           cmic_common_pool_schan_ch1_message11_read,            cmic_common_pool_schan_ch1_message11_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE12_REG_ADDR,           cmic_common_pool_schan_ch1_message12_read,            cmic_common_pool_schan_ch1_message12_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE13_REG_ADDR,           cmic_common_pool_schan_ch1_message13_read,            cmic_common_pool_schan_ch1_message13_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE14_REG_ADDR,           cmic_common_pool_schan_ch1_message14_read,            cmic_common_pool_schan_ch1_message14_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE15_REG_ADDR,           cmic_common_pool_schan_ch1_message15_read,            cmic_common_pool_schan_ch1_message15_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE16_REG_ADDR,           cmic_common_pool_schan_ch1_message16_read,            cmic_common_pool_schan_ch1_message16_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE17_REG_ADDR,           cmic_common_pool_schan_ch1_message17_read,            cmic_common_pool_schan_ch1_message17_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE18_REG_ADDR,           cmic_common_pool_schan_ch1_message18_read,            cmic_common_pool_schan_ch1_message18_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE19_REG_ADDR,           cmic_common_pool_schan_ch1_message19_read,            cmic_common_pool_schan_ch1_message19_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE20_REG_ADDR,           cmic_common_pool_schan_ch1_message20_read,            cmic_common_pool_schan_ch1_message20_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_MESSAGE21_REG_ADDR,           cmic_common_pool_schan_ch1_message21_read,            cmic_common_pool_schan_ch1_message21_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE0_REG_ADDR,            cmic_common_pool_schan_ch2_message0_read,             cmic_common_pool_schan_ch2_message0_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE1_REG_ADDR,            cmic_common_pool_schan_ch2_message1_read,             cmic_common_pool_schan_ch2_message1_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE2_REG_ADDR,            cmic_common_pool_schan_ch2_message2_read,             cmic_common_pool_schan_ch2_message2_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE3_REG_ADDR,            cmic_common_pool_schan_ch2_message3_read,             cmic_common_pool_schan_ch2_message3_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE4_REG_ADDR,            cmic_common_pool_schan_ch2_message4_read,             cmic_common_pool_schan_ch2_message4_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE5_REG_ADDR,            cmic_common_pool_schan_ch2_message5_read,             cmic_common_pool_schan_ch2_message5_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE6_REG_ADDR,            cmic_common_pool_schan_ch2_message6_read,             cmic_common_pool_schan_ch2_message6_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE7_REG_ADDR,            cmic_common_pool_schan_ch2_message7_read,             cmic_common_pool_schan_ch2_message7_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE8_REG_ADDR,            cmic_common_pool_schan_ch2_message8_read,             cmic_common_pool_schan_ch2_message8_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE9_REG_ADDR,            cmic_common_pool_schan_ch2_message9_read,             cmic_common_pool_schan_ch2_message9_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE10_REG_ADDR,           cmic_common_pool_schan_ch2_message10_read,            cmic_common_pool_schan_ch2_message10_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE11_REG_ADDR,           cmic_common_pool_schan_ch2_message11_read,            cmic_common_pool_schan_ch2_message11_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE12_REG_ADDR,           cmic_common_pool_schan_ch2_message12_read,            cmic_common_pool_schan_ch2_message12_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE13_REG_ADDR,           cmic_common_pool_schan_ch2_message13_read,            cmic_common_pool_schan_ch2_message13_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE14_REG_ADDR,           cmic_common_pool_schan_ch2_message14_read,            cmic_common_pool_schan_ch2_message14_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE15_REG_ADDR,           cmic_common_pool_schan_ch2_message15_read,            cmic_common_pool_schan_ch2_message15_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE16_REG_ADDR,           cmic_common_pool_schan_ch2_message16_read,            cmic_common_pool_schan_ch2_message16_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE17_REG_ADDR,           cmic_common_pool_schan_ch2_message17_read,            cmic_common_pool_schan_ch2_message17_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE18_REG_ADDR,           cmic_common_pool_schan_ch2_message18_read,            cmic_common_pool_schan_ch2_message18_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE19_REG_ADDR,           cmic_common_pool_schan_ch2_message19_read,            cmic_common_pool_schan_ch2_message19_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE20_REG_ADDR,           cmic_common_pool_schan_ch2_message20_read,            cmic_common_pool_schan_ch2_message20_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_MESSAGE21_REG_ADDR,           cmic_common_pool_schan_ch2_message21_read,            cmic_common_pool_schan_ch2_message21_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE0_REG_ADDR,            cmic_common_pool_schan_ch3_message0_read,             cmic_common_pool_schan_ch3_message0_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE1_REG_ADDR,            cmic_common_pool_schan_ch3_message1_read,             cmic_common_pool_schan_ch3_message1_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE2_REG_ADDR,            cmic_common_pool_schan_ch3_message2_read,             cmic_common_pool_schan_ch3_message2_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE3_REG_ADDR,            cmic_common_pool_schan_ch3_message3_read,             cmic_common_pool_schan_ch3_message3_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE4_REG_ADDR,            cmic_common_pool_schan_ch3_message4_read,             cmic_common_pool_schan_ch3_message4_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE5_REG_ADDR,            cmic_common_pool_schan_ch3_message5_read,             cmic_common_pool_schan_ch3_message5_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE6_REG_ADDR,            cmic_common_pool_schan_ch3_message6_read,             cmic_common_pool_schan_ch3_message6_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE7_REG_ADDR,            cmic_common_pool_schan_ch3_message7_read,             cmic_common_pool_schan_ch3_message7_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE8_REG_ADDR,            cmic_common_pool_schan_ch3_message8_read,             cmic_common_pool_schan_ch3_message8_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE9_REG_ADDR,            cmic_common_pool_schan_ch3_message9_read,             cmic_common_pool_schan_ch3_message9_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE10_REG_ADDR,           cmic_common_pool_schan_ch3_message10_read,            cmic_common_pool_schan_ch3_message10_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE11_REG_ADDR,           cmic_common_pool_schan_ch3_message11_read,            cmic_common_pool_schan_ch3_message11_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE12_REG_ADDR,           cmic_common_pool_schan_ch3_message12_read,            cmic_common_pool_schan_ch3_message12_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE13_REG_ADDR,           cmic_common_pool_schan_ch3_message13_read,            cmic_common_pool_schan_ch3_message13_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE14_REG_ADDR,           cmic_common_pool_schan_ch3_message14_read,            cmic_common_pool_schan_ch3_message14_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE15_REG_ADDR,           cmic_common_pool_schan_ch3_message15_read,            cmic_common_pool_schan_ch3_message15_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE16_REG_ADDR,           cmic_common_pool_schan_ch3_message16_read,            cmic_common_pool_schan_ch3_message16_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE17_REG_ADDR,           cmic_common_pool_schan_ch3_message17_read,            cmic_common_pool_schan_ch3_message17_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE18_REG_ADDR,           cmic_common_pool_schan_ch3_message18_read,            cmic_common_pool_schan_ch3_message18_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE19_REG_ADDR,           cmic_common_pool_schan_ch3_message19_read,            cmic_common_pool_schan_ch3_message19_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE20_REG_ADDR,           cmic_common_pool_schan_ch3_message20_read,            cmic_common_pool_schan_ch3_message20_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_MESSAGE21_REG_ADDR,           cmic_common_pool_schan_ch3_message21_read,            cmic_common_pool_schan_ch3_message21_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE0_REG_ADDR,            cmic_common_pool_schan_ch4_message0_read,             cmic_common_pool_schan_ch4_message0_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE1_REG_ADDR,            cmic_common_pool_schan_ch4_message1_read,             cmic_common_pool_schan_ch4_message1_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE2_REG_ADDR,            cmic_common_pool_schan_ch4_message2_read,             cmic_common_pool_schan_ch4_message2_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE3_REG_ADDR,            cmic_common_pool_schan_ch4_message3_read,             cmic_common_pool_schan_ch4_message3_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE4_REG_ADDR,            cmic_common_pool_schan_ch4_message4_read,             cmic_common_pool_schan_ch4_message4_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE5_REG_ADDR,            cmic_common_pool_schan_ch4_message5_read,             cmic_common_pool_schan_ch4_message5_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE6_REG_ADDR,            cmic_common_pool_schan_ch4_message6_read,             cmic_common_pool_schan_ch4_message6_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE7_REG_ADDR,            cmic_common_pool_schan_ch4_message7_read,             cmic_common_pool_schan_ch4_message7_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE8_REG_ADDR,            cmic_common_pool_schan_ch4_message8_read,             cmic_common_pool_schan_ch4_message8_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE9_REG_ADDR,            cmic_common_pool_schan_ch4_message9_read,             cmic_common_pool_schan_ch4_message9_write            ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE10_REG_ADDR,           cmic_common_pool_schan_ch4_message10_read,            cmic_common_pool_schan_ch4_message10_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE11_REG_ADDR,           cmic_common_pool_schan_ch4_message11_read,            cmic_common_pool_schan_ch4_message11_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE12_REG_ADDR,           cmic_common_pool_schan_ch4_message12_read,            cmic_common_pool_schan_ch4_message12_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE13_REG_ADDR,           cmic_common_pool_schan_ch4_message13_read,            cmic_common_pool_schan_ch4_message13_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE14_REG_ADDR,           cmic_common_pool_schan_ch4_message14_read,            cmic_common_pool_schan_ch4_message14_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE15_REG_ADDR,           cmic_common_pool_schan_ch4_message15_read,            cmic_common_pool_schan_ch4_message15_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE16_REG_ADDR,           cmic_common_pool_schan_ch4_message16_read,            cmic_common_pool_schan_ch4_message16_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE17_REG_ADDR,           cmic_common_pool_schan_ch4_message17_read,            cmic_common_pool_schan_ch4_message17_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE18_REG_ADDR,           cmic_common_pool_schan_ch4_message18_read,            cmic_common_pool_schan_ch4_message18_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE19_REG_ADDR,           cmic_common_pool_schan_ch4_message19_read,            cmic_common_pool_schan_ch4_message19_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE20_REG_ADDR,           cmic_common_pool_schan_ch4_message20_read,            cmic_common_pool_schan_ch4_message20_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_MESSAGE21_REG_ADDR,           cmic_common_pool_schan_ch4_message21_read,            cmic_common_pool_schan_ch4_message21_write           ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_CTRL_REG_ADDR,                cmic_common_pool_schan_ch0_ctrl_read,                 cmic_common_pool_schan_ch0_ctrl_write                ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_ACK_DATA_BEAT_COUNT_REG_ADDR, cmic_common_pool_schan_ch0_ack_data_beat_count_read,  cmic_common_pool_schan_ch0_ack_data_beat_count_write ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH0_ERR_REG_ADDR,                 cmic_common_pool_schan_ch0_err_read,                  cmic_common_pool_schan_ch0_err_write                 ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_CTRL_REG_ADDR,                cmic_common_pool_schan_ch1_ctrl_read,                 cmic_common_pool_schan_ch1_ctrl_write                ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_ACK_DATA_BEAT_COUNT_REG_ADDR, cmic_common_pool_schan_ch1_ack_data_beat_count_read,  cmic_common_pool_schan_ch1_ack_data_beat_count_write ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH1_ERR_REG_ADDR,                 cmic_common_pool_schan_ch1_err_read,                  cmic_common_pool_schan_ch1_err_write                 ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_CTRL_REG_ADDR,                cmic_common_pool_schan_ch2_ctrl_read,                 cmic_common_pool_schan_ch2_ctrl_write                ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_ACK_DATA_BEAT_COUNT_REG_ADDR, cmic_common_pool_schan_ch2_ack_data_beat_count_read,  cmic_common_pool_schan_ch2_ack_data_beat_count_write ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH2_ERR_REG_ADDR,                 cmic_common_pool_schan_ch2_err_read,                  cmic_common_pool_schan_ch2_err_write                 ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_CTRL_REG_ADDR,                cmic_common_pool_schan_ch3_ctrl_read,                 cmic_common_pool_schan_ch3_ctrl_write                ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_ACK_DATA_BEAT_COUNT_REG_ADDR, cmic_common_pool_schan_ch3_ack_data_beat_count_read,  cmic_common_pool_schan_ch3_ack_data_beat_count_write ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH3_ERR_REG_ADDR,                 cmic_common_pool_schan_ch3_err_read,                  cmic_common_pool_schan_ch3_err_write                 ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_CTRL_REG_ADDR,                cmic_common_pool_schan_ch4_ctrl_read,                 cmic_common_pool_schan_ch4_ctrl_write                ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_ACK_DATA_BEAT_COUNT_REG_ADDR, cmic_common_pool_schan_ch4_ack_data_beat_count_read,  cmic_common_pool_schan_ch4_ack_data_beat_count_write ) &&
            cmicx_pcid_register_add(CMIC_COMMON_POOL_SCHAN_CH4_ERR_REG_ADDR,                 cmic_common_pool_schan_ch4_err_read,                  cmic_common_pool_schan_ch4_err_write                 ) ) ) {
        cmicx_error("could not initialize schan PCID registers\n");
    }

    /* add PCID sbus dma registers */
    if (! ( cmicx_pcid_register_add(CMIC_TOP_SBUS_RING_ARB_CTRL_SBUSDMA_REG_ADDR,                               cmic_top_sbus_ring_arb_ctrl_sbusdma_read,                               cmic_top_sbus_ring_arb_ctrl_sbusdma_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_SHARED_CONFIG_REG_ADDR,                                           cmic_cmc0_shared_config_read,                                           cmic_cmc0_shared_config_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CONTROL_REG_ADDR,                                     cmic_cmc0_sbusdma_ch0_control_read,                                     cmic_cmc0_sbusdma_ch0_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_REQUEST_REG_ADDR,                                     cmic_cmc0_sbusdma_ch0_request_read,                                     cmic_cmc0_sbusdma_ch0_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_COUNT_REG_ADDR,                                       cmic_cmc0_sbusdma_ch0_count_read,                                       cmic_cmc0_sbusdma_ch0_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_OPCODE_REG_ADDR,                                      cmic_cmc0_sbusdma_ch0_opcode_read,                                      cmic_cmc0_sbusdma_ch0_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc0_sbusdma_ch0_sbus_start_address_read,                          cmic_cmc0_sbusdma_ch0_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_read,                    cmic_cmc0_sbusdma_ch0_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_read,                    cmic_cmc0_sbusdma_ch0_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc0_sbusdma_ch0_desc_start_address_lo_read,                       cmic_cmc0_sbusdma_ch0_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc0_sbusdma_ch0_desc_start_address_hi_read,                       cmic_cmc0_sbusdma_ch0_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_STATUS_REG_ADDR,                                      cmic_cmc0_sbusdma_ch0_status_read,                                      cmic_cmc0_sbusdma_ch0_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_read,                      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_read,                      cmic_cmc0_sbusdma_ch0_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc0_sbusdma_ch0_cur_sbus_address_read,                            cmic_cmc0_sbusdma_ch0_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_read,                         cmic_cmc0_sbusdma_ch0_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_read,                         cmic_cmc0_sbusdma_ch0_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_read,                  cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_read,                    cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_read,                   cmic_cmc0_sbusdma_ch0_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc0_sbusdma_ch0_sbusdma_debug_read,                               cmic_cmc0_sbusdma_ch0_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_read,                           cmic_cmc0_sbusdma_ch0_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_TIMER_REG_ADDR,                                       cmic_cmc0_sbusdma_ch0_timer_read,                                       cmic_cmc0_sbusdma_ch0_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH0_ITER_COUNT_REG_ADDR,                                  cmic_cmc0_sbusdma_ch0_iter_count_read,                                  cmic_cmc0_sbusdma_ch0_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CONTROL_REG_ADDR,                                     cmic_cmc0_sbusdma_ch1_control_read,                                     cmic_cmc0_sbusdma_ch1_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_REQUEST_REG_ADDR,                                     cmic_cmc0_sbusdma_ch1_request_read,                                     cmic_cmc0_sbusdma_ch1_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_COUNT_REG_ADDR,                                       cmic_cmc0_sbusdma_ch1_count_read,                                       cmic_cmc0_sbusdma_ch1_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_OPCODE_REG_ADDR,                                      cmic_cmc0_sbusdma_ch1_opcode_read,                                      cmic_cmc0_sbusdma_ch1_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc0_sbusdma_ch1_sbus_start_address_read,                          cmic_cmc0_sbusdma_ch1_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_read,                    cmic_cmc0_sbusdma_ch1_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_read,                    cmic_cmc0_sbusdma_ch1_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc0_sbusdma_ch1_desc_start_address_lo_read,                       cmic_cmc0_sbusdma_ch1_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc0_sbusdma_ch1_desc_start_address_hi_read,                       cmic_cmc0_sbusdma_ch1_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_STATUS_REG_ADDR,                                      cmic_cmc0_sbusdma_ch1_status_read,                                      cmic_cmc0_sbusdma_ch1_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_read,                      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_read,                      cmic_cmc0_sbusdma_ch1_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc0_sbusdma_ch1_cur_sbus_address_read,                            cmic_cmc0_sbusdma_ch1_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_read,                         cmic_cmc0_sbusdma_ch1_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_read,                         cmic_cmc0_sbusdma_ch1_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_read,                  cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_read,                    cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_read,                   cmic_cmc0_sbusdma_ch1_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc0_sbusdma_ch1_sbusdma_debug_read,                               cmic_cmc0_sbusdma_ch1_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_read,                           cmic_cmc0_sbusdma_ch1_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_TIMER_REG_ADDR,                                       cmic_cmc0_sbusdma_ch1_timer_read,                                       cmic_cmc0_sbusdma_ch1_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH1_ITER_COUNT_REG_ADDR,                                  cmic_cmc0_sbusdma_ch1_iter_count_read,                                  cmic_cmc0_sbusdma_ch1_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CONTROL_REG_ADDR,                                     cmic_cmc0_sbusdma_ch2_control_read,                                     cmic_cmc0_sbusdma_ch2_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_REQUEST_REG_ADDR,                                     cmic_cmc0_sbusdma_ch2_request_read,                                     cmic_cmc0_sbusdma_ch2_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_COUNT_REG_ADDR,                                       cmic_cmc0_sbusdma_ch2_count_read,                                       cmic_cmc0_sbusdma_ch2_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_OPCODE_REG_ADDR,                                      cmic_cmc0_sbusdma_ch2_opcode_read,                                      cmic_cmc0_sbusdma_ch2_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc0_sbusdma_ch2_sbus_start_address_read,                          cmic_cmc0_sbusdma_ch2_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_read,                    cmic_cmc0_sbusdma_ch2_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_read,                    cmic_cmc0_sbusdma_ch2_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc0_sbusdma_ch2_desc_start_address_lo_read,                       cmic_cmc0_sbusdma_ch2_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc0_sbusdma_ch2_desc_start_address_hi_read,                       cmic_cmc0_sbusdma_ch2_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_STATUS_REG_ADDR,                                      cmic_cmc0_sbusdma_ch2_status_read,                                      cmic_cmc0_sbusdma_ch2_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_read,                      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_read,                      cmic_cmc0_sbusdma_ch2_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc0_sbusdma_ch2_cur_sbus_address_read,                            cmic_cmc0_sbusdma_ch2_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_read,                         cmic_cmc0_sbusdma_ch2_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_read,                         cmic_cmc0_sbusdma_ch2_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_read,                  cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_read,                    cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_read,                   cmic_cmc0_sbusdma_ch2_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc0_sbusdma_ch2_sbusdma_debug_read,                               cmic_cmc0_sbusdma_ch2_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_read,                           cmic_cmc0_sbusdma_ch2_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_TIMER_REG_ADDR,                                       cmic_cmc0_sbusdma_ch2_timer_read,                                       cmic_cmc0_sbusdma_ch2_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH2_ITER_COUNT_REG_ADDR,                                  cmic_cmc0_sbusdma_ch2_iter_count_read,                                  cmic_cmc0_sbusdma_ch2_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CONTROL_REG_ADDR,                                     cmic_cmc0_sbusdma_ch3_control_read,                                     cmic_cmc0_sbusdma_ch3_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_REQUEST_REG_ADDR,                                     cmic_cmc0_sbusdma_ch3_request_read,                                     cmic_cmc0_sbusdma_ch3_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_COUNT_REG_ADDR,                                       cmic_cmc0_sbusdma_ch3_count_read,                                       cmic_cmc0_sbusdma_ch3_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_OPCODE_REG_ADDR,                                      cmic_cmc0_sbusdma_ch3_opcode_read,                                      cmic_cmc0_sbusdma_ch3_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc0_sbusdma_ch3_sbus_start_address_read,                          cmic_cmc0_sbusdma_ch3_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_read,                    cmic_cmc0_sbusdma_ch3_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_read,                    cmic_cmc0_sbusdma_ch3_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc0_sbusdma_ch3_desc_start_address_lo_read,                       cmic_cmc0_sbusdma_ch3_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc0_sbusdma_ch3_desc_start_address_hi_read,                       cmic_cmc0_sbusdma_ch3_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_STATUS_REG_ADDR,                                      cmic_cmc0_sbusdma_ch3_status_read,                                      cmic_cmc0_sbusdma_ch3_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_read,                      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_read,                      cmic_cmc0_sbusdma_ch3_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc0_sbusdma_ch3_cur_sbus_address_read,                            cmic_cmc0_sbusdma_ch3_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_read,                         cmic_cmc0_sbusdma_ch3_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_read,                         cmic_cmc0_sbusdma_ch3_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_read,                  cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_read,                    cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_read,                   cmic_cmc0_sbusdma_ch3_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc0_sbusdma_ch3_sbusdma_debug_read,                               cmic_cmc0_sbusdma_ch3_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_read,                           cmic_cmc0_sbusdma_ch3_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_TIMER_REG_ADDR,                                       cmic_cmc0_sbusdma_ch3_timer_read,                                       cmic_cmc0_sbusdma_ch3_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC0_SBUSDMA_CH3_ITER_COUNT_REG_ADDR,                                  cmic_cmc0_sbusdma_ch3_iter_count_read,                                  cmic_cmc0_sbusdma_ch3_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SHARED_CONFIG_REG_ADDR,                                           cmic_cmc1_shared_config_read,                                           cmic_cmc1_shared_config_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CONTROL_REG_ADDR,                                     cmic_cmc1_sbusdma_ch0_control_read,                                     cmic_cmc1_sbusdma_ch0_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_REQUEST_REG_ADDR,                                     cmic_cmc1_sbusdma_ch0_request_read,                                     cmic_cmc1_sbusdma_ch0_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_COUNT_REG_ADDR,                                       cmic_cmc1_sbusdma_ch0_count_read,                                       cmic_cmc1_sbusdma_ch0_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_OPCODE_REG_ADDR,                                      cmic_cmc1_sbusdma_ch0_opcode_read,                                      cmic_cmc1_sbusdma_ch0_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc1_sbusdma_ch0_sbus_start_address_read,                          cmic_cmc1_sbusdma_ch0_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_read,                    cmic_cmc1_sbusdma_ch0_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_read,                    cmic_cmc1_sbusdma_ch0_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc1_sbusdma_ch0_desc_start_address_lo_read,                       cmic_cmc1_sbusdma_ch0_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc1_sbusdma_ch0_desc_start_address_hi_read,                       cmic_cmc1_sbusdma_ch0_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_STATUS_REG_ADDR,                                      cmic_cmc1_sbusdma_ch0_status_read,                                      cmic_cmc1_sbusdma_ch0_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_read,                      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_read,                      cmic_cmc1_sbusdma_ch0_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc1_sbusdma_ch0_cur_sbus_address_read,                            cmic_cmc1_sbusdma_ch0_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_read,                         cmic_cmc1_sbusdma_ch0_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_read,                         cmic_cmc1_sbusdma_ch0_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_read,                  cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_read,                    cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_read,                   cmic_cmc1_sbusdma_ch0_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc1_sbusdma_ch0_sbusdma_debug_read,                               cmic_cmc1_sbusdma_ch0_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_read,                           cmic_cmc1_sbusdma_ch0_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_TIMER_REG_ADDR,                                       cmic_cmc1_sbusdma_ch0_timer_read,                                       cmic_cmc1_sbusdma_ch0_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH0_ITER_COUNT_REG_ADDR,                                  cmic_cmc1_sbusdma_ch0_iter_count_read,                                  cmic_cmc1_sbusdma_ch0_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CONTROL_REG_ADDR,                                     cmic_cmc1_sbusdma_ch1_control_read,                                     cmic_cmc1_sbusdma_ch1_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_REQUEST_REG_ADDR,                                     cmic_cmc1_sbusdma_ch1_request_read,                                     cmic_cmc1_sbusdma_ch1_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_COUNT_REG_ADDR,                                       cmic_cmc1_sbusdma_ch1_count_read,                                       cmic_cmc1_sbusdma_ch1_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_OPCODE_REG_ADDR,                                      cmic_cmc1_sbusdma_ch1_opcode_read,                                      cmic_cmc1_sbusdma_ch1_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc1_sbusdma_ch1_sbus_start_address_read,                          cmic_cmc1_sbusdma_ch1_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_read,                    cmic_cmc1_sbusdma_ch1_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_read,                    cmic_cmc1_sbusdma_ch1_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc1_sbusdma_ch1_desc_start_address_lo_read,                       cmic_cmc1_sbusdma_ch1_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc1_sbusdma_ch1_desc_start_address_hi_read,                       cmic_cmc1_sbusdma_ch1_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_STATUS_REG_ADDR,                                      cmic_cmc1_sbusdma_ch1_status_read,                                      cmic_cmc1_sbusdma_ch1_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_read,                      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_read,                      cmic_cmc1_sbusdma_ch1_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc1_sbusdma_ch1_cur_sbus_address_read,                            cmic_cmc1_sbusdma_ch1_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_read,                         cmic_cmc1_sbusdma_ch1_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_read,                         cmic_cmc1_sbusdma_ch1_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_read,                  cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_read,                    cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_read,                   cmic_cmc1_sbusdma_ch1_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc1_sbusdma_ch1_sbusdma_debug_read,                               cmic_cmc1_sbusdma_ch1_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_read,                           cmic_cmc1_sbusdma_ch1_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_TIMER_REG_ADDR,                                       cmic_cmc1_sbusdma_ch1_timer_read,                                       cmic_cmc1_sbusdma_ch1_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH1_ITER_COUNT_REG_ADDR,                                  cmic_cmc1_sbusdma_ch1_iter_count_read,                                  cmic_cmc1_sbusdma_ch1_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CONTROL_REG_ADDR,                                     cmic_cmc1_sbusdma_ch2_control_read,                                     cmic_cmc1_sbusdma_ch2_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_REQUEST_REG_ADDR,                                     cmic_cmc1_sbusdma_ch2_request_read,                                     cmic_cmc1_sbusdma_ch2_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_COUNT_REG_ADDR,                                       cmic_cmc1_sbusdma_ch2_count_read,                                       cmic_cmc1_sbusdma_ch2_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_OPCODE_REG_ADDR,                                      cmic_cmc1_sbusdma_ch2_opcode_read,                                      cmic_cmc1_sbusdma_ch2_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc1_sbusdma_ch2_sbus_start_address_read,                          cmic_cmc1_sbusdma_ch2_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_read,                    cmic_cmc1_sbusdma_ch2_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_read,                    cmic_cmc1_sbusdma_ch2_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc1_sbusdma_ch2_desc_start_address_lo_read,                       cmic_cmc1_sbusdma_ch2_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc1_sbusdma_ch2_desc_start_address_hi_read,                       cmic_cmc1_sbusdma_ch2_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_STATUS_REG_ADDR,                                      cmic_cmc1_sbusdma_ch2_status_read,                                      cmic_cmc1_sbusdma_ch2_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_read,                      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_read,                      cmic_cmc1_sbusdma_ch2_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc1_sbusdma_ch2_cur_sbus_address_read,                            cmic_cmc1_sbusdma_ch2_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_read,                         cmic_cmc1_sbusdma_ch2_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_read,                         cmic_cmc1_sbusdma_ch2_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_read,                  cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_read,                    cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_read,                   cmic_cmc1_sbusdma_ch2_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc1_sbusdma_ch2_sbusdma_debug_read,                               cmic_cmc1_sbusdma_ch2_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_read,                           cmic_cmc1_sbusdma_ch2_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_TIMER_REG_ADDR,                                       cmic_cmc1_sbusdma_ch2_timer_read,                                       cmic_cmc1_sbusdma_ch2_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH2_ITER_COUNT_REG_ADDR,                                  cmic_cmc1_sbusdma_ch2_iter_count_read,                                  cmic_cmc1_sbusdma_ch2_iter_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CONTROL_REG_ADDR,                                     cmic_cmc1_sbusdma_ch3_control_read,                                     cmic_cmc1_sbusdma_ch3_control_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_REQUEST_REG_ADDR,                                     cmic_cmc1_sbusdma_ch3_request_read,                                     cmic_cmc1_sbusdma_ch3_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_COUNT_REG_ADDR,                                       cmic_cmc1_sbusdma_ch3_count_read,                                       cmic_cmc1_sbusdma_ch3_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_OPCODE_REG_ADDR,                                      cmic_cmc1_sbusdma_ch3_opcode_read,                                      cmic_cmc1_sbusdma_ch3_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_SBUS_START_ADDRESS_REG_ADDR,                          cmic_cmc1_sbusdma_ch3_sbus_start_address_read,                          cmic_cmc1_sbusdma_ch3_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_HOSTMEM_START_ADDRESS_LO_REG_ADDR,                    cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_read,                    cmic_cmc1_sbusdma_ch3_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_HOSTMEM_START_ADDRESS_HI_REG_ADDR,                    cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_read,                    cmic_cmc1_sbusdma_ch3_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_DESC_START_ADDRESS_LO_REG_ADDR,                       cmic_cmc1_sbusdma_ch3_desc_start_address_lo_read,                       cmic_cmc1_sbusdma_ch3_desc_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_DESC_START_ADDRESS_HI_REG_ADDR,                       cmic_cmc1_sbusdma_ch3_desc_start_address_hi_read,                       cmic_cmc1_sbusdma_ch3_desc_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_STATUS_REG_ADDR,                                      cmic_cmc1_sbusdma_ch3_status_read,                                      cmic_cmc1_sbusdma_ch3_status_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_HOSTMEM_ADDRESS_LO_REG_ADDR,                      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_read,                      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_HOSTMEM_ADDRESS_HI_REG_ADDR,                      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_read,                      cmic_cmc1_sbusdma_ch3_cur_hostmem_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUS_ADDRESS_REG_ADDR,                            cmic_cmc1_sbusdma_ch3_cur_sbus_address_read,                            cmic_cmc1_sbusdma_ch3_cur_sbus_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_DESC_ADDRESS_LO_REG_ADDR,                         cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_read,                         cmic_cmc1_sbusdma_ch3_cur_desc_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_DESC_ADDRESS_HI_REG_ADDR,                         cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_read,                         cmic_cmc1_sbusdma_ch3_cur_desc_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_REQUEST_REG_ADDR,                  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_read,                  cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_request_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_COUNT_REG_ADDR,                    cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_read,                    cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_count_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_SBUS_START_ADDRESS_REG_ADDR,       cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_read,       cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_sbus_start_address_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_LO_REG_ADDR, cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_read, cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_lo_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_HOSTMEM_START_ADDRESS_HI_REG_ADDR, cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_read, cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_hostmem_start_address_hi_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_CUR_SBUSDMA_CONFIG_OPCODE_REG_ADDR,                   cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_read,                   cmic_cmc1_sbusdma_ch3_cur_sbusdma_config_opcode_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_SBUSDMA_DEBUG_REG_ADDR,                               cmic_cmc1_sbusdma_ch3_sbusdma_debug_read,                               cmic_cmc1_sbusdma_ch3_sbusdma_debug_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_SBUSDMA_DEBUG_CLR_REG_ADDR,                           cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_read,                           cmic_cmc1_sbusdma_ch3_sbusdma_debug_clr_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_TIMER_REG_ADDR,                                       cmic_cmc1_sbusdma_ch3_timer_read,                                       cmic_cmc1_sbusdma_ch3_timer_write) &&
            cmicx_pcid_register_add(CMIC_CMC1_SBUSDMA_CH3_ITER_COUNT_REG_ADDR,                                  cmic_cmc1_sbusdma_ch3_iter_count_read,                                  cmic_cmc1_sbusdma_ch3_iter_count_write) ) ) {
        cmicx_error("could not initialize schan PCID registers\n");
    }

    /* add PCID packet dma registers */
    if ( !( cmicx_pcid_register_add(CMIC_TOP_CONFIG_REG_ADDR,                        cmic_top_config_read,                        cmic_top_config_write                        ) &&
            cmicx_pcid_register_add(CMIC_TOP_EPINTF_RELEASE_ALL_CREDITS_REG_ADDR,    cmic_top_epintf_release_all_credits_read,    cmic_top_epintf_release_all_credits_write    ) &&
            cmicx_pcid_register_add(CMIC_CMC0_SHARED_IRQ_STAT0_REG_ADDR,             cmic_cmc0_shared_irq_stat0_read,             cmic_cmc0_shared_irq_stat0_write             ) &&
            cmicx_pcid_register_add(CMIC_CMC0_SHARED_IRQ_STAT_CLR0_REG_ADDR,         cmic_cmc0_shared_irq_stat_clr0_read,         cmic_cmc0_shared_irq_stat_clr0_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch0_ctrl_read,              cmic_cmc0_pktdma_ch0_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch0_desc_addr_lo_read,      cmic_cmc0_pktdma_ch0_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch0_desc_addr_hi_read,      cmic_cmc0_pktdma_ch0_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch0_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch0_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch0_stat_read,              cmic_cmc0_pktdma_ch0_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch0_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch0_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch0_intr_coal_read,         cmic_cmc0_pktdma_ch0_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch0_curr_desc_lo_read,      cmic_cmc0_pktdma_ch0_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch0_curr_desc_hi_read,      cmic_cmc0_pktdma_ch0_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch0_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH0_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch0_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch0_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch1_ctrl_read,              cmic_cmc0_pktdma_ch1_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch1_desc_addr_lo_read,      cmic_cmc0_pktdma_ch1_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch1_desc_addr_hi_read,      cmic_cmc0_pktdma_ch1_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch1_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch1_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch1_stat_read,              cmic_cmc0_pktdma_ch1_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch1_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch1_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch1_intr_coal_read,         cmic_cmc0_pktdma_ch1_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch1_curr_desc_lo_read,      cmic_cmc0_pktdma_ch1_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch1_curr_desc_hi_read,      cmic_cmc0_pktdma_ch1_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch1_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH1_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch1_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch1_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch2_ctrl_read,              cmic_cmc0_pktdma_ch2_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch2_desc_addr_lo_read,      cmic_cmc0_pktdma_ch2_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch2_desc_addr_hi_read,      cmic_cmc0_pktdma_ch2_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch2_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch2_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch2_stat_read,              cmic_cmc0_pktdma_ch2_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch2_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch2_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch2_intr_coal_read,         cmic_cmc0_pktdma_ch2_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch2_curr_desc_lo_read,      cmic_cmc0_pktdma_ch2_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch2_curr_desc_hi_read,      cmic_cmc0_pktdma_ch2_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch2_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH2_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch2_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch2_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch3_ctrl_read,              cmic_cmc0_pktdma_ch3_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch3_desc_addr_lo_read,      cmic_cmc0_pktdma_ch3_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch3_desc_addr_hi_read,      cmic_cmc0_pktdma_ch3_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch3_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch3_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch3_stat_read,              cmic_cmc0_pktdma_ch3_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch3_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch3_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch3_intr_coal_read,         cmic_cmc0_pktdma_ch3_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch3_curr_desc_lo_read,      cmic_cmc0_pktdma_ch3_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch3_curr_desc_hi_read,      cmic_cmc0_pktdma_ch3_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch3_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH3_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch3_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch3_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch4_ctrl_read,              cmic_cmc0_pktdma_ch4_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch4_desc_addr_lo_read,      cmic_cmc0_pktdma_ch4_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch4_desc_addr_hi_read,      cmic_cmc0_pktdma_ch4_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch4_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch4_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch4_stat_read,              cmic_cmc0_pktdma_ch4_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch4_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch4_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch4_intr_coal_read,         cmic_cmc0_pktdma_ch4_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch4_curr_desc_lo_read,      cmic_cmc0_pktdma_ch4_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch4_curr_desc_hi_read,      cmic_cmc0_pktdma_ch4_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch4_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH4_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch4_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch4_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch5_ctrl_read,              cmic_cmc0_pktdma_ch5_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch5_desc_addr_lo_read,      cmic_cmc0_pktdma_ch5_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch5_desc_addr_hi_read,      cmic_cmc0_pktdma_ch5_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch5_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch5_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch5_stat_read,              cmic_cmc0_pktdma_ch5_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch5_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch5_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch5_intr_coal_read,         cmic_cmc0_pktdma_ch5_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch5_curr_desc_lo_read,      cmic_cmc0_pktdma_ch5_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch5_curr_desc_hi_read,      cmic_cmc0_pktdma_ch5_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch5_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH5_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch5_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch5_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch6_ctrl_read,              cmic_cmc0_pktdma_ch6_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch6_desc_addr_lo_read,      cmic_cmc0_pktdma_ch6_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch6_desc_addr_hi_read,      cmic_cmc0_pktdma_ch6_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch6_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch6_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch6_stat_read,              cmic_cmc0_pktdma_ch6_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch6_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch6_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch6_intr_coal_read,         cmic_cmc0_pktdma_ch6_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch6_curr_desc_lo_read,      cmic_cmc0_pktdma_ch6_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch6_curr_desc_hi_read,      cmic_cmc0_pktdma_ch6_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch6_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH6_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch6_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch6_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_CTRL_REG_ADDR,              cmic_cmc0_pktdma_ch7_ctrl_read,              cmic_cmc0_pktdma_ch7_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_DESC_ADDR_LO_REG_ADDR,      cmic_cmc0_pktdma_ch7_desc_addr_lo_read,      cmic_cmc0_pktdma_ch7_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_DESC_ADDR_HI_REG_ADDR,      cmic_cmc0_pktdma_ch7_desc_addr_hi_read,      cmic_cmc0_pktdma_ch7_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_read, cmic_cmc0_pktdma_ch7_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_read, cmic_cmc0_pktdma_ch7_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_STAT_REG_ADDR,              cmic_cmc0_pktdma_ch7_stat_read,              cmic_cmc0_pktdma_ch7_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_read,     cmic_cmc0_pktdma_ch7_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_read,     cmic_cmc0_pktdma_ch7_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_INTR_COAL_REG_ADDR,         cmic_cmc0_pktdma_ch7_intr_coal_read,         cmic_cmc0_pktdma_ch7_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_CURR_DESC_LO_REG_ADDR,      cmic_cmc0_pktdma_ch7_curr_desc_lo_read,      cmic_cmc0_pktdma_ch7_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_CURR_DESC_HI_REG_ADDR,      cmic_cmc0_pktdma_ch7_curr_desc_hi_read,      cmic_cmc0_pktdma_ch7_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_read,   cmic_cmc0_pktdma_ch7_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC0_PKTDMA_CH7_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc0_pktdma_ch7_pkt_count_txpkt_read,   cmic_cmc0_pktdma_ch7_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_SHARED_IRQ_STAT0_REG_ADDR,             cmic_cmc1_shared_irq_stat0_read,             cmic_cmc1_shared_irq_stat0_write             ) &&
            cmicx_pcid_register_add(CMIC_CMC1_SHARED_IRQ_STAT_CLR0_REG_ADDR,         cmic_cmc1_shared_irq_stat_clr0_read,         cmic_cmc1_shared_irq_stat_clr0_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch0_ctrl_read,              cmic_cmc1_pktdma_ch0_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch0_desc_addr_lo_read,      cmic_cmc1_pktdma_ch0_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch0_desc_addr_hi_read,      cmic_cmc1_pktdma_ch0_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch0_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch0_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch0_stat_read,              cmic_cmc1_pktdma_ch0_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch0_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch0_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch0_intr_coal_read,         cmic_cmc1_pktdma_ch0_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch0_curr_desc_lo_read,      cmic_cmc1_pktdma_ch0_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch0_curr_desc_hi_read,      cmic_cmc1_pktdma_ch0_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch0_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH0_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch0_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch0_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch1_ctrl_read,              cmic_cmc1_pktdma_ch1_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch1_desc_addr_lo_read,      cmic_cmc1_pktdma_ch1_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch1_desc_addr_hi_read,      cmic_cmc1_pktdma_ch1_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch1_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch1_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch1_stat_read,              cmic_cmc1_pktdma_ch1_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch1_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch1_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch1_intr_coal_read,         cmic_cmc1_pktdma_ch1_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch1_curr_desc_lo_read,      cmic_cmc1_pktdma_ch1_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch1_curr_desc_hi_read,      cmic_cmc1_pktdma_ch1_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch1_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH1_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch1_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch1_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch2_ctrl_read,              cmic_cmc1_pktdma_ch2_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch2_desc_addr_lo_read,      cmic_cmc1_pktdma_ch2_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch2_desc_addr_hi_read,      cmic_cmc1_pktdma_ch2_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch2_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch2_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch2_stat_read,              cmic_cmc1_pktdma_ch2_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch2_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch2_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch2_intr_coal_read,         cmic_cmc1_pktdma_ch2_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch2_curr_desc_lo_read,      cmic_cmc1_pktdma_ch2_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch2_curr_desc_hi_read,      cmic_cmc1_pktdma_ch2_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch2_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH2_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch2_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch2_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch3_ctrl_read,              cmic_cmc1_pktdma_ch3_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch3_desc_addr_lo_read,      cmic_cmc1_pktdma_ch3_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch3_desc_addr_hi_read,      cmic_cmc1_pktdma_ch3_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch3_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch3_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch3_stat_read,              cmic_cmc1_pktdma_ch3_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch3_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch3_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch3_intr_coal_read,         cmic_cmc1_pktdma_ch3_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch3_curr_desc_lo_read,      cmic_cmc1_pktdma_ch3_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch3_curr_desc_hi_read,      cmic_cmc1_pktdma_ch3_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch3_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH3_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch3_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch3_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch4_ctrl_read,              cmic_cmc1_pktdma_ch4_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch4_desc_addr_lo_read,      cmic_cmc1_pktdma_ch4_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch4_desc_addr_hi_read,      cmic_cmc1_pktdma_ch4_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch4_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch4_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch4_stat_read,              cmic_cmc1_pktdma_ch4_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch4_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch4_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch4_intr_coal_read,         cmic_cmc1_pktdma_ch4_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch4_curr_desc_lo_read,      cmic_cmc1_pktdma_ch4_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch4_curr_desc_hi_read,      cmic_cmc1_pktdma_ch4_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch4_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH4_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch4_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch4_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch5_ctrl_read,              cmic_cmc1_pktdma_ch5_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch5_desc_addr_lo_read,      cmic_cmc1_pktdma_ch5_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch5_desc_addr_hi_read,      cmic_cmc1_pktdma_ch5_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch5_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch5_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch5_stat_read,              cmic_cmc1_pktdma_ch5_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch5_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch5_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch5_intr_coal_read,         cmic_cmc1_pktdma_ch5_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch5_curr_desc_lo_read,      cmic_cmc1_pktdma_ch5_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch5_curr_desc_hi_read,      cmic_cmc1_pktdma_ch5_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch5_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH5_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch5_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch5_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch6_ctrl_read,              cmic_cmc1_pktdma_ch6_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch6_desc_addr_lo_read,      cmic_cmc1_pktdma_ch6_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch6_desc_addr_hi_read,      cmic_cmc1_pktdma_ch6_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch6_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch6_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch6_stat_read,              cmic_cmc1_pktdma_ch6_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch6_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch6_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch6_intr_coal_read,         cmic_cmc1_pktdma_ch6_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch6_curr_desc_lo_read,      cmic_cmc1_pktdma_ch6_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch6_curr_desc_hi_read,      cmic_cmc1_pktdma_ch6_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch6_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH6_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch6_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch6_pkt_count_txpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_CTRL_REG_ADDR,              cmic_cmc1_pktdma_ch7_ctrl_read,              cmic_cmc1_pktdma_ch7_ctrl_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_DESC_ADDR_LO_REG_ADDR,      cmic_cmc1_pktdma_ch7_desc_addr_lo_read,      cmic_cmc1_pktdma_ch7_desc_addr_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_DESC_ADDR_HI_REG_ADDR,      cmic_cmc1_pktdma_ch7_desc_addr_hi_read,      cmic_cmc1_pktdma_ch7_desc_addr_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_DESC_HALT_ADDR_LO_REG_ADDR, cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_read, cmic_cmc1_pktdma_ch7_desc_halt_addr_lo_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_DESC_HALT_ADDR_HI_REG_ADDR, cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_read, cmic_cmc1_pktdma_ch7_desc_halt_addr_hi_write ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_STAT_REG_ADDR,              cmic_cmc1_pktdma_ch7_stat_read,              cmic_cmc1_pktdma_ch7_stat_write              ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_0_REG_ADDR,     cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_read,     cmic_cmc1_pktdma_ch7_cos_ctrl_rx_0_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_COS_CTRL_RX_1_REG_ADDR,     cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_read,     cmic_cmc1_pktdma_ch7_cos_ctrl_rx_1_write     ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_INTR_COAL_REG_ADDR,         cmic_cmc1_pktdma_ch7_intr_coal_read,         cmic_cmc1_pktdma_ch7_intr_coal_write         ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_CURR_DESC_LO_REG_ADDR,      cmic_cmc1_pktdma_ch7_curr_desc_lo_read,      cmic_cmc1_pktdma_ch7_curr_desc_lo_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_CURR_DESC_HI_REG_ADDR,      cmic_cmc1_pktdma_ch7_curr_desc_hi_read,      cmic_cmc1_pktdma_ch7_curr_desc_hi_write      ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_PKT_COUNT_RXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_read,   cmic_cmc1_pktdma_ch7_pkt_count_rxpkt_write   ) &&
            cmicx_pcid_register_add(CMIC_CMC1_PKTDMA_CH7_PKT_COUNT_TXPKT_REG_ADDR,   cmic_cmc1_pktdma_ch7_pkt_count_txpkt_read,   cmic_cmc1_pktdma_ch7_pkt_count_txpkt_write   ) ) ) {
        cmicx_error("could not initialize packet dma PCID registers\n");
    }

    /* add PCID schan fifo registers */
    /*if ( !( cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE0_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message1_read,    cmic_cmc0_schan_fifo_buff1_message1_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE1_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message1_read,    cmic_cmc0_schan_fifo_buff1_message1_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE2_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message2_read,    cmic_cmc0_schan_fifo_buff1_message2_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE3_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message3_read,    cmic_cmc0_schan_fifo_buff1_message3_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE4_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message4_read,    cmic_cmc0_schan_fifo_buff1_message4_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE5_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message5_read,    cmic_cmc0_schan_fifo_buff1_message5_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE6_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message6_read,    cmic_cmc0_schan_fifo_buff1_message6_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE7_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message7_read,    cmic_cmc0_schan_fifo_buff1_message7_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE8_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message8_read,    cmic_cmc0_schan_fifo_buff1_message8_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE9_REG_ADDR,  cmic_cmc0_schan_fifo_buff1_message9_read,    cmic_cmc0_schan_fifo_buff1_message9_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE10_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message10_read,   cmic_cmc0_schan_fifo_buff1_message10_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE11_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message11_read,   cmic_cmc0_schan_fifo_buff1_message11_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE12_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message12_read,   cmic_cmc0_schan_fifo_buff1_message12_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE13_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message13_read,   cmic_cmc0_schan_fifo_buff1_message13_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE14_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message14_read,   cmic_cmc0_schan_fifo_buff1_message14_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF1_MESSAGE15_REG_ADDR, cmic_cmc0_schan_fifo_buff1_message15_read,   cmic_cmc0_schan_fifo_buff1_message15_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE0_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message0_read,    cmic_cmc0_schan_fifo_buff2_message0_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE1_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message1_read,    cmic_cmc0_schan_fifo_buff2_message1_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE2_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message2_read,    cmic_cmc0_schan_fifo_buff2_message2_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE3_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message3_read,    cmic_cmc0_schan_fifo_buff2_message3_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE4_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message4_read,    cmic_cmc0_schan_fifo_buff2_message4_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE5_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message5_read,    cmic_cmc0_schan_fifo_buff2_message5_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE6_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message6_read,    cmic_cmc0_schan_fifo_buff2_message6_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE7_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message7_read,    cmic_cmc0_schan_fifo_buff2_message7_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE8_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message8_read,    cmic_cmc0_schan_fifo_buff2_message8_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE9_REG_ADDR,  cmic_cmc0_schan_fifo_buff2_message9_read,    cmic_cmc0_schan_fifo_buff2_message9_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE10_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message10_read,   cmic_cmc0_schan_fifo_buff2_message10_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE11_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message11_read,   cmic_cmc0_schan_fifo_buff2_message11_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE12_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message12_read,   cmic_cmc0_schan_fifo_buff2_message12_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE13_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message13_read,   cmic_cmc0_schan_fifo_buff2_message13_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE14_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message14_read,   cmic_cmc0_schan_fifo_buff2_message14_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_BUFF2_MESSAGE15_REG_ADDR, cmic_cmc0_schan_fifo_buff2_message15_read,   cmic_cmc0_schan_fifo_buff2_message15_write ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_STATUS_BUFF1_REG_ADDR,    cmic_cmc0_schan_fifo_status_buff1_read,      cmic_cmc0_schan_fifo_status_buff1_write    ) &&
            cmicx_pcid_register_add( CMIC_CMC0_SCHAN_FIFO_STATUS_BUFF2_REG_ADDR,    cmic_cmc0_schan_fifo_status_buff2_read,      cmic_cmc0_schan_fifo_status_buff2_write    ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE0_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message0_read,    cmic_cmc1_schan_fifo_buff1_message0_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE1_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message1_read,    cmic_cmc1_schan_fifo_buff1_message1_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE2_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message2_read,    cmic_cmc1_schan_fifo_buff1_message2_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE3_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message3_read,    cmic_cmc1_schan_fifo_buff1_message3_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE4_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message4_read,    cmic_cmc1_schan_fifo_buff1_message4_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE5_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message5_read,    cmic_cmc1_schan_fifo_buff1_message5_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE6_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message6_read,    cmic_cmc1_schan_fifo_buff1_message6_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE7_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message7_read,    cmic_cmc1_schan_fifo_buff1_message7_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE8_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message8_read,    cmic_cmc1_schan_fifo_buff1_message8_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE9_REG_ADDR,  cmic_cmc1_schan_fifo_buff1_message9_read,    cmic_cmc1_schan_fifo_buff1_message9_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE10_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message10_read,   cmic_cmc1_schan_fifo_buff1_message10_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE11_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message11_read,   cmic_cmc1_schan_fifo_buff1_message11_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE12_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message12_read,   cmic_cmc1_schan_fifo_buff1_message12_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE13_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message13_read,   cmic_cmc1_schan_fifo_buff1_message13_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE14_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message14_read,   cmic_cmc1_schan_fifo_buff1_message14_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF1_MESSAGE15_REG_ADDR, cmic_cmc1_schan_fifo_buff1_message15_read,   cmic_cmc1_schan_fifo_buff1_message15_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE0_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message0_read,    cmic_cmc1_schan_fifo_buff2_message0_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE1_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message1_read,    cmic_cmc1_schan_fifo_buff2_message1_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE2_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message2_read,    cmic_cmc1_schan_fifo_buff2_message2_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE3_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message3_read,    cmic_cmc1_schan_fifo_buff2_message3_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE4_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message4_read,    cmic_cmc1_schan_fifo_buff2_message4_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE5_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message5_read,    cmic_cmc1_schan_fifo_buff2_message5_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE6_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message6_read,    cmic_cmc1_schan_fifo_buff2_message6_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE7_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message7_read,    cmic_cmc1_schan_fifo_buff2_message7_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE8_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message8_read,    cmic_cmc1_schan_fifo_buff2_message8_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE9_REG_ADDR,  cmic_cmc1_schan_fifo_buff2_message9_read,    cmic_cmc1_schan_fifo_buff2_message9_write  ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE10_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message10_read,   cmic_cmc1_schan_fifo_buff2_message10_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE11_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message11_read,   cmic_cmc1_schan_fifo_buff2_message11_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE12_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message12_read,   cmic_cmc1_schan_fifo_buff2_message12_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE13_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message13_read,   cmic_cmc1_schan_fifo_buff2_message13_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE14_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message14_read,   cmic_cmc1_schan_fifo_buff2_message14_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_BUFF2_MESSAGE15_REG_ADDR, cmic_cmc1_schan_fifo_buff2_message15_read,   cmic_cmc1_schan_fifo_buff2_message15_write ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_STATUS_BUFF1_REG_ADDR,    cmic_cmc1_schan_fifo_status_buff1_read,      cmic_cmc1_schan_fifo_status_buff1_write    ) &&
            cmicx_pcid_register_add( CMIC_CMC1_SCHAN_FIFO_STATUS_BUFF2_REG_ADDR,    cmic_cmc1_schan_fifo_status_buff2_read,      cmic_cmc1_schan_fifo_status_buff2_write    ) ) ) {
        cmicx_error("could not initialize schan fifo PCID registers\n");
    }
    */


    /* log message of successful initialization of cmicx sim */
    cmicx_log("CMICx simulation successfully initialized.");
    cmicx_log_newline();
    return true;
}


/* updates the cmicx to its next state, based on the current state */
bool cmicx_update(pcid_info_t *pcid_info) {

    int i;
    int j;

    /* calculate (and display) the time it's been since the last cmicx sim top-level update */
    cmicx_log_level_set(0);
    cmicx_log("time since last cmicx_update: ");
    cmicx_log_elapsed_time();
    cmicx_log_newline();
    cmicx_log_level_set(3);


    /************************************************************************/
    /* old method: loop through each unit needing an update and update once */
    /************************************************************************/

    /* update the state of each outstanding schan command coming from an schan block */
    /*
    cmicx_schan_update(pcid_info, &cmicx->cmc0.schan_data);
    cmicx_schan_update(pcid_info, &cmicx->cmc1.schan_data);
    cmicx_schan_update(pcid_info, &cmicx->common.schan_data);
    */

    /* update the state of each outstanding sbus dma unit */
    /*
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc0.sbus_dma_ch0);
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc0.sbus_dma_ch1);
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc0.sbus_dma_ch2);
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc1.sbus_dma_ch0);
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc1.sbus_dma_ch1);
    cmicx_sbus_dma_update(pcid_info, &cmicx->cmc1.sbus_dma_ch2);
    */

    /* update the state of each scha fifo unit */

    /* update the state of each packet dma channel */
    /*
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        cmicx_packet_dma_update(pcid_info, &cmicx->cmc0.packet_dma[i]);
        cmicx_packet_dma_update(pcid_info, &cmicx->cmc1.packet_dma[i]);
    }
    */


    /***********************************************************************************************************/
    /* new method: use WRR weightings in CMIC_TOP_SBUS_RING_ARB_CTRL_* registers to update Sbus master devices */
    /* a weighted number of times. update everything else that does not access the sbus once.                  */
    /***********************************************************************************************************/

    /* update all regular schan units based on their sbus arbiter weights. */
    for (i=0; i<CMICX_NUM_SCHAN_IN_COMMON; i++) {

        /* make a number of updates based on the weight value of this schan unit */
        for (j=0; j<cmicx->sbus_ring_arb_ctrl_schan_common_ch_weight[i]; j++) {
            if (cmicx_schan_update(pcid_info, &cmicx->common.schan[i])==false) {
                cmicx_log("ERROR: failure to update schan \"");
                cmicx_log(cmicx->common.schan[i].id_string);
                cmicx_log("\".\n");
                return false;
            }
        }

    }

    /* update the single schan fifo unit based on its sbus arbiter weight */
    /*
    for (i=0; i<cmicx->sbus_ring_arb_ctrl_schan_common_fifo_weight; i++) {
        cmicx_schan_fifo_update(&cmicx->common.schan_fifo);
    }
    */

    /* update all sbus dma units a number of times, based on their sbus arbiter weights. a weight of zero means no update. */
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        for (j=0; j<cmicx->sbus_ring_arb_ctrl_sbusdma_cmc0_ch_weight[i]; j++) {
            if (cmicx_sbus_dma_update(pcid_info, &cmicx->cmc0.sbus_dma[i])==false) {
                cmicx_log("ERROR: failure to update sbus dma \"");
                cmicx_log(cmicx->cmc0.sbus_dma[i].id_string);
                cmicx_log("\".\n");
                return false;
            }
        }
    }
    for (i=0; i<CMICX_NUM_SBUS_DMA_PER_CMC; i++) {
        for (j=0; j<cmicx->sbus_ring_arb_ctrl_sbusdma_cmc1_ch_weight[i]; j++) {
            if (cmicx_sbus_dma_update(pcid_info, &cmicx->cmc1.sbus_dma[i])==false) {
                cmicx_log("ERROR: failure to update sbus dma \"");
                cmicx_log(cmicx->cmc1.sbus_dma[i].id_string);
                cmicx_log("\".\n");
                return false;
            }
        }
    }

    /* update the global packet dma, and the state of each packet dma channel once. */
    /* no weighting here, since there isn't an equivalent of sbus weighting for packet dma (only dma read and write weights (to do later?)). */
    cmicx_packet_dma_global_update();
    for (i=0; i<CMICX_NUM_PACKET_DMA_PER_CMC; i++) {
        cmicx_packet_dma_update(pcid_info, &cmicx->cmc0.packet_dma[i]);
        cmicx_packet_dma_update(pcid_info, &cmicx->cmc1.packet_dma[i]);
    }

    /* successful update complete */
    cmicx_log_level_set(0);
    cmicx_log("CMICx successfully updated.");
    cmicx_log_newline();
    cmicx_log("this cmicx update took: ");
    cmicx_log_elapsed_time();
    cmicx_log_newline();
    cmicx_log_level_set(3);
    return true;

}


/* for a pcid register read, check for the address and make the necessary call. as a simple optimization, we store the last accessed address index */
/* and start looking from there, because that is most likely where we are going to access again (most recently used policy). */
bool cmicx_pcid_register_read(addr_t addr,reg_t *reg) {

    int i;
    static int last_i = 0;   /* note last_i here is static, so it persists between calls */
    bool past_last_i  = false;
    reg_t return_value;

    for (i=last_i; (past_last_i==true)?(i!=last_i):(cmicx->last_pcid_register>=0); i=(i+1)%(cmicx->last_pcid_register+1)) {
        past_last_i = true;
        if (cmicx->pcid_addrs[i]==addr) {
            last_i = i;
            return_value = (cmicx->pcid_read_funcs[i])(reg);
            cmicx_log_level_set(2);
            cmicx_log("PCID register read at: ");
            cmicx_log_uint64(addr);
            cmicx_log(". value: ");
            cmicx_log_reg(*reg);
            cmicx_log(".");
            cmicx_log_newline();
            cmicx_log_level_set(3);
            return return_value;
        }
    }

    cmicx_log("ERROR: could not read from cmicx pci register ");
    cmicx_log_addr(addr);
    cmicx_log(" - register does not exist.\n");
    /*cmicx_error("ERROR: read from invalid register. check log file.\n");*/
    return false;
}


/* for a pcid register write, check for the address and make the necessary call. as a simple optimization, we store the last accessed address index */
/* and start looking from there, because that is most likely where we are going to access again (most recently used policy). */
bool cmicx_pcid_register_write(addr_t addr,reg_t reg) {

    int i;
    static int last_i = 0;   /* note last_i here is static, so it persists between calls */
    bool past_last_i  = false;
    reg_t return_value;

    for (i=last_i; (past_last_i==true)?(i!=last_i):(cmicx->last_pcid_register>=0); i=(i+1)%(cmicx->last_pcid_register+1)) {
        past_last_i = true;
        if (cmicx->pcid_addrs[i]==addr) {
            last_i = i;
            return_value = (cmicx->pcid_write_funcs[i])(reg);
            cmicx_log_level_set(2);
            cmicx_log("PCID register write at: ");
            cmicx_log_uint64(addr);
            cmicx_log(". value: ");
            cmicx_log_reg(reg);
            cmicx_log(".");
            cmicx_log_newline();
            cmicx_log_level_set(3);
            return return_value;
        }
    }

    cmicx_log("ERROR: could not write to cmicx pci register ");
    cmicx_log_addr(addr);
    cmicx_log(" - register does not exist.\n");
    /*cmicx_error("ERROR: write to invalid register. check log file.\n");*/
    return false;
}


/* function to add a pcid register to the list of available registers. this list maps a pcid address to a read and write function for that address */
/* note this could be switched to a red-black tree or some other faster implementation */
bool cmicx_pcid_register_add(addr_t addr,pcid_read_func_t *read_func,pcid_write_func_t *write_func) {

    int i;

    /* make sure the pcid register of the given address doesn't exist already */
    for (i=0; i<=cmicx->last_pcid_register; i++) {
        if (cmicx->pcid_addrs[i]==addr) {
            cmicx_log("ERROR: could not add pcid register with address ");
            cmicx_log_addr(addr);
            cmicx_log(" to array - already exists\n");
            return false;
        }
    }

    /* add the register if there is room */
    if (cmicx->last_pcid_register!=(NUM_PCID_REGISTERS-1)) {
        cmicx->last_pcid_register++;
#ifdef CMICX_STANDALONE_SIM
        cmicx->pcid_addrs[cmicx->last_pcid_register]       = addr;
#else
        cmicx->pcid_addrs[cmicx->last_pcid_register]       = (CMICX_SIM_PCI_BASE_ADDR | addr);   /* if we are running this as part of the sdk, then we need to use the pci base address offset. */
#endif
        cmicx->pcid_read_funcs[cmicx->last_pcid_register]  = read_func;
        cmicx->pcid_write_funcs[cmicx->last_pcid_register] = write_func;
        return true;
    }
    else {
        cmicx_log("ERROR: could not add pcid register with address ");
        cmicx_log_addr(addr);
        cmicx_log(" to array - too many registers already (would exceed bounds of array\n");
        return false;
    }

}


/* sends an interrupt from the CMICx PCID sim to the SDK. */
bool cmicx_pcid_interrupt_send(pcid_info_t *pcid_info) {
    int rv;

    rv = send_interrupt(pcid_info->client->intsock,0);
    if (rv < 0) {
        cmicx_log("soc_internal_send_int failed");
        pcid_info->opt_rpc_error = 1;
    } else {
        cmicx_log("interrupt sent!");
    }
    cmicx_log_newline();
    return false;

}


/* pcid memory read function. reads a chunk of memory from the internally stored cmicx memory data (simulated memory space). */
/* does not do anything special when the memory can't be found. */
bool cmicx_pcid_mem_read(pcid_info_t *pcid_info, addr_t addr,void *mem,int amount) {

#ifdef CMICX_STANDALONE_SIM

    int i;
    int j;
    int last_j = 0;
    bool found = false;
    bool past_last_j = false;

    /* debug */
    cmicx_log_level_set(1);
    cmicx_log_newline();
    cmicx_log("mem_read: reading ");
    cmicx_log_int(amount);
    cmicx_log(" bytes from memory location ");
    cmicx_log_uint64(addr);
    cmicx_log_newline();
    cmicx_log_level_set(3);

    /* standalone sim case: read each byte from the internal cmicx sim memory */
    for (i=0; i<amount; i++) {

        /* find our mem entry in the pcid_memory[] array. notice the (last_j) start value and stop value here. */
        /* simple optimization, because our memory values will probably be in order of address internally (but it need not necessarily be that way) */
        found       = false;
        past_last_j = false;
        for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_pcid_memory_address_index>=0); j=(j+1)%(cmicx->last_pcid_memory_address_index+1)) {
            past_last_j = true;
            if (cmicx->pcid_memory_addresses[j]==addr) {
                ((unsigned char *)mem)[i] = cmicx->pcid_memory[j];
                found  = true;
                last_j = j;
                break;
            }
        }

        /* if we haven't found anything yet at this point, then this memory location does not exist in the array. we have an error trying to read it. */
        if (found==false) {
            cmicx_log("ERROR: could not find memory entry at address ");
            cmicx_log_addr(addr);
            cmicx_log(" when reading memory\n");
            return false;
        }
        else {
            addr += 1;
        }

    }

#else

    /* sdk case: we need to perform a dma read to get the host memory */
    cmicx_log("dma: about to read ");
    cmicx_log_int(amount);
    cmicx_log(" bytes from addr ");
    cmicx_log_addr(addr);
    cmicx_log("...");
    dma_read_bytes(pcid_info->client->dmasock,addr,(uint8 *)mem,amount);
    cmicx_log(" done.");
    cmicx_log_newline();

#endif

    return true;
}


/* pcid memory write function. writes a chunk of memory from the internally stored cmicx memory data (simulated memory space). */
/* when the memory is not already found, expands the known memory internally (still uses a statically allocated array for storage, though). */
bool cmicx_pcid_mem_write(pcid_info_t *pcid_info, addr_t addr,void *mem,int amount) {

#ifdef CMICX_STANDALONE_SIM
    int i;
    int j;
    int last_j = 0;
    bool found = false;
    bool past_last_j = false;

    /* debug */
    cmicx_log_level_set(1);
    cmicx_log_newline();
    cmicx_log("mem_write: writing ");
    cmicx_log_int(amount);
    cmicx_log(" bytes to memory at location ");
    cmicx_log_uint64(addr);
    cmicx_log_newline();
    cmicx_log_level_set(3);

    /* standalone sim case: write each word to the internal cmicx sim memory */
    for (i=0; i<amount; i++) {

        /* search the memory array for this address. note the last_j start and stop point optimization. this is a simple */
        /* optimization, since the memory values will probably be in order of address (but need not necessarily be that way). */
        found = false;
        past_last_j = false;
        for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_pcid_memory_address_index>=0); j=(j+1)%(cmicx->last_pcid_memory_address_index+1)) {
            past_last_j = true;
            if (cmicx->pcid_memory_addresses[j]==addr) {
                cmicx->pcid_memory[j] = ((unsigned char *)mem)[i];
                found  = true;
                last_j = j;
                break;
            }
        }

        /* if we haven't found anything yet, then this memory location does not exist in the array. add it. */
        if (found==false) {
            if (cmicx->last_pcid_memory_address_index!=(MAX_PCID_MEM_VALUES-1)) {
                cmicx->last_pcid_memory_address_index++;
                cmicx->pcid_memory_addresses[cmicx->last_pcid_memory_address_index] = addr;
                cmicx->pcid_memory[          cmicx->last_pcid_memory_address_index] = ((char *)mem)[i];
            }
            else {
                cmicx_log("ERROR: no space in internal memory array when trying to add memory at address ");
                cmicx_log_addr(addr);
                cmicx_log("\n");
                return false;
            }
        }

        addr += 1;

    }

#else

    /* sdk case: we need to write to the host memory over pcid */
    cmicx_log("dma: about to write ");
    cmicx_log_int(amount);
    cmicx_log(" bytes to addr ");
    cmicx_log_addr(addr);
    cmicx_log("...");
    dma_write_bytes(pcid_info->client->dmasock,addr,(uint8 *)mem,amount);
    cmicx_log(" done.");
    cmicx_log_newline();

#endif

    return true;

}

/* will dump the internal pcid memory to the cmicx sim log (used for testing/debugging) */
void cmicx_pcid_mem_dump() {

    int       i;
    const int chain_limit = 4;
    int       chain_count = 0;

    cmicx_log_level_set(5);
    cmicx_log("CMICx sim local PCID memory cache dump (count ");
    cmicx_log_int(cmicx->last_pcid_memory_address_index+1);
    cmicx_log("):");
    for (i=0; i<=cmicx->last_pcid_memory_address_index; i++) {

        /* if the current address is just one more than the last address and we have not been chaining for too long, then we can chain this byte. */
        if ((i>0) && (cmicx->pcid_memory_addresses[i]==(cmicx->pcid_memory_addresses[i-1]+1)) && (chain_count<chain_limit)) {
            cmicx_log(" ");
            cmicx_log_byte(cmicx->pcid_memory[i]);
            chain_count++;
        }

        /* otherwise, we need to print the address and reset the chain count. */
        else {
            cmicx_log("\n");
            cmicx_log_uint64(cmicx->pcid_memory_addresses[i]);
            cmicx_log(": ");
            cmicx_log_byte(cmicx->pcid_memory[i]);
            chain_count = 1;
        }

    }
    cmicx_log_newline();
    cmicx_log_level_set(3);

}


/* cmicx switch register read function. reads from a switch register. if it doesnt exist, create it first with the value 0xBEEF, and then return that value  */
bool cmicx_switch_reg_read(addr_t addr,reg_t *val) {

    int j;
    static int last_j = 0;   /* note last_j here is static, so it persists between calls */
    bool past_last_j  = false;

    /* find our register entry in the switch_register[] array. notice the static (last_j) start value and stop value here. */
    /* simple optimization, because our register values will probably be in order of address internally (but it need not necessarily be that way) */
    for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_switch_register>=0); j=(j+1)%(cmicx->last_switch_register+1)) {
        past_last_j = true;
        if (cmicx->switch_register_addresses[j]==addr) {
            *val = cmicx->switch_registers[j];
            last_j = j;
            return true;
        }
    }

    /* if we haven't found anything yet at this point, then this switch register does not exist in the array. */
    /* write the value 0 to the register, and then return that value */
    if (cmicx->last_switch_register!=(CMICX_MAX_SWITCH_REGISTERS-1)) {
        cmicx->last_switch_register++;
        cmicx->switch_register_addresses[cmicx->last_switch_register] = addr;
        cmicx->switch_registers[         cmicx->last_switch_register] = 0;
        *val = cmicx->switch_registers[  cmicx->last_switch_register];
    }
    else {
        cmicx_log("ERROR: no space in switch register array when trying to add register at address ");
        cmicx_log_addr(addr);
        cmicx_log(" during read\n");
        return false;
    }

    return true;
}


/* cmicx switch register write function. writes to a switch register. if it doesnt exist, create it first. */
bool cmicx_switch_reg_write(addr_t addr,reg_t val) {

    int j;
    static int last_j = 0;   /* note last_j here is static, so it persists between calls */
    bool past_last_j  = false;

    /* search the switch register array for this address. note the last_j start and stop point optimization. this is a simple */
    /* optimization, since the register values will probably be in order of address (but need not necessarily be that way). */
    for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_switch_register>=0); j=(j+1)%(cmicx->last_switch_register+1)) {
        past_last_j = true;
        if (cmicx->switch_register_addresses[j]==addr) {
            cmicx->switch_registers[j] = val;
            last_j = j;
            return true;
        }
    }

    /* if we haven't found anything yet, then this register does not exist in the array. add it. */
    if (cmicx->last_switch_register!=(CMICX_MAX_SWITCH_REGISTERS-1)) {
        cmicx->last_switch_register++;
        cmicx->switch_register_addresses[cmicx->last_switch_register] = addr;
        cmicx->switch_registers[         cmicx->last_switch_register] = val;
    }
    else {
        cmicx_log("ERROR: no space in switch register array when trying to add register at address ");
        cmicx_log_addr(addr);
        cmicx_log("\n");
        return false;
    }

    return true;
}

/* will dump the internal cmicx switch registers to the cmicx sim log (used for testing/debugging) */
void cmicx_switch_registers_dump() {

    int i;

    cmicx_log_level_set(5);
    cmicx_log("CMICx sim switch register cache dump (count ");
    cmicx_log_int(cmicx->last_switch_register+1);
    cmicx_log("):\n");
    for (i=0; i<=cmicx->last_switch_register; i++) {
        cmicx_log_addr(cmicx->switch_register_addresses[i]);
        cmicx_log(": ");
        cmicx_log_hex(cmicx->switch_registers[i]);
        if (i!=cmicx->last_switch_register) {
            cmicx_log("\n");
        }
    }
    cmicx_log_newline();
    cmicx_log_level_set(3);

}


/* cmicx switch memory read function. reads from a switch memory value at a given address up to a certain number of words, */
/* if that memory value already exists/was allocated. if the memory value was already allocated but is of a smaller size, then */
/* only that many words are read. if the memory at this address does not exist yet, then it is created with the size of req, */
/* filled with 0xBEEF, and then returned. in all cases, req indicates the desired size of the memory (and the maximum number of */
/* words returnable), and resp indicates the number of words actually returned. thus, in all cases, resp<=req. */
bool cmicx_switch_mem_read(addr_t addr,reg_t *words,int req,int *resp) {

    int j;
    static int last_j = 0;   /* note last_j here is static, so it persists between calls */
    bool past_last_j  = false;

    cmicx_log_level_set(-1);
    cmicx_log("reading mem block of size ");
    cmicx_log_int(req);
    cmicx_log(" at address ");
    cmicx_log_addr(addr);
    cmicx_log_newline();
    cmicx_log_level_set(3);

    /* find our memory in the switch_memories[] array. notice the static (last_j) start value and stop value here. */
    /* simple optimization, because our memory values will probably be in order of address internally (but it need not necessarily be that way) */
    for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_switch_memory>=0); j=(j+1)%(cmicx->last_switch_memory+1)) {
        past_last_j = true;

        /* we have found the memory. copy as much data as we can to the target and return. */
        if (cmicx->switch_memory_addresses[j]==addr) {

            /* the 0th element of the array holds the number of valid words in this memory. this statement determines if this or req is the limiting factor in copying data. */
            if (cmicx->switch_memories[j][0]>=req) {
                *resp = req;
            }
            else {
                *resp = cmicx->switch_memories[j][0];
            }
            memcpy((void *)words,(void *)&(cmicx->switch_memories[j][1]),(*resp)*sizeof(reg_t));
            last_j = j;
            return true;
        }
    }

    /* if we haven't found anything yet at this point, then this memory does not exist in the array. */
    /* create the memory with the request size, fill it with 0, and then return this memory */
    if (cmicx->last_switch_memory!=(CMICX_MAX_SWITCH_MEMORIES-1)) {
        cmicx->last_switch_memory++;
        cmicx->switch_memory_addresses[cmicx->last_switch_memory] = addr;
        cmicx->switch_memories[        cmicx->last_switch_memory] = (reg_t *)malloc(sizeof(reg_t)*(req+1));   /* allocate (req+1) words because we will store the number of words in position [0] (usage below) */
        if (cmicx->switch_memories[    cmicx->last_switch_memory]==NULL) {
            cmicx_log("ERROR: no actual memory when trying to allocate memory for switch memory block during switch mem block read (switch mem addr=");
            cmicx_log_addr(addr);
            cmicx_log(")\n");
            return false;
        }
        cmicx->switch_memories[        cmicx->last_switch_memory][0] = req;
        for (j=1; j<=req; j++) {
            cmicx->switch_memories[cmicx->last_switch_memory][j] = 0;
        }
        last_j = cmicx->last_switch_memory;
        *resp = cmicx->switch_memories[last_j][0];
        memcpy((void *)words,(void *)&(cmicx->switch_memories[last_j][1]),(*resp)*sizeof(reg_t));
    }
    else {
        cmicx_log("ERROR: no space in switch memory array when trying to add memory at address ");
        cmicx_log_addr(addr);
        cmicx_log(" during read\n");
        return false;
    }

    return true;

}

/* cmicx switch memory writing function. writes memory of a specified size to a switch memory address. if the memory already exists */
/* as a different size, the old memory is thrown away and overwritten with new memory of the new size. */
bool cmicx_switch_mem_write(addr_t addr,reg_t *words,int num) {

    int j;
    static int last_j          = 0;   /* note last_j here is static, so it persists between calls */
    bool past_last_j           = false;
    bool memory_exists         = false;
    bool need_new_memory_block = true;

    cmicx_log_level_set(-1);
    cmicx_log("writing mem block of size ");
    cmicx_log_int(num);
    cmicx_log(" at address ");
    cmicx_log_addr(addr);
    cmicx_log_newline();
    cmicx_log_level_set(3);

    /* search the memory array for this address. note the last_j start and stop point optimization. this is a simple */
    /* optimization, since the memory values will probably be in order of address (but need not necessarily be that way). */
    for (j=last_j; (past_last_j==true)?(j!=last_j):(cmicx->last_switch_memory>=0); j=(j+1)%(cmicx->last_switch_memory+1)) {
        past_last_j = true;

        /* if we have found the memory and it is already the right size, then mark that we don't need a new memory block created. if it is not the right size, then throw away the existing block, */
        /* and allow one to be created later. in both cases we break out of the loop, and the last step of the function will copy the data to the memory block later. */
        if (cmicx->switch_memory_addresses[j]==addr) {

            if (cmicx->switch_memories[j][0]==num) {
                need_new_memory_block = false;
            }
            else {
                free(cmicx->switch_memories[j]);
                need_new_memory_block = true;
            }

            last_j        = j;
            memory_exists = true;
            break;
        }
    }

    /* if we haven't found anything yet, then this memory does not exist in the array. add it. note that the next step in the function will take care of */
    /* actually creating the memory block (because this functionality is shared with the loop above). */
    if (memory_exists==false) {
        if (cmicx->last_switch_memory!=(CMICX_MAX_SWITCH_MEMORIES-1)) {
            cmicx->last_switch_memory++;
            cmicx->switch_memory_addresses[cmicx->last_switch_memory] = addr;
            last_j                =        cmicx->last_switch_memory;
            need_new_memory_block = true;
        }
        else {
            cmicx_log("ERROR: no space in switch memory array when trying to add memory at address ");
            cmicx_log_addr(addr);
            cmicx_log("\n");
            return false;
        }
    }

    /* create a new memory block if we need it. allocate (num+1) words because we will store the number of words in the mem block in position [0] (usage below) */
    if (need_new_memory_block==true) {
        cmicx->switch_memories[last_j] = (reg_t *)malloc(sizeof(reg_t)*(num+1));
        if (cmicx->switch_memories[last_j]==NULL) {
            cmicx_log("ERROR: no actual memory when trying to allocate memory for switch memory block during switch mem block write (switch mem addr=");
            cmicx_log_addr(addr);
            cmicx_log(")\n");
            return false;
        }
    }

    /* finally, copy the data memory data to the destination, and exit successfully */
    cmicx->switch_memories[last_j][0] = num;
    memcpy((void *)&(cmicx->switch_memories[last_j][1]),(void *)words,num*sizeof(reg_t));
    return true;

}

/* will dump the internal cmicx switch memories to the cmicx sim log (used for testing/debugging) */
void cmicx_switch_memories_dump() {

    int i;
    int j;

    cmicx_log_level_set(5);
    cmicx_log("CMICx sim switch memories dump (count ");
    cmicx_log_int(cmicx->last_switch_memory+1);
    cmicx_log("):\n");
    for (i=0; i<=cmicx->last_switch_memory; i++) {
        cmicx_log_addr(cmicx->switch_memory_addresses[i]);
        cmicx_log(" (count ");
        cmicx_log_int(cmicx->switch_memories[i][0]);
        cmicx_log("):\n");
        for (j=1; j<=cmicx->switch_memories[i][0]; j++) {
            cmicx_log_hex(cmicx->switch_memories[i][j]);
            if (!((i==cmicx->last_switch_memory) && (j==cmicx->switch_memories[i][0]))) {
                cmicx_log("\n");
            }
        }
    }
    cmicx_log_newline();
    cmicx_log_level_set(3);

}


/* cmicx sim control reg read and write functions */
bool cmicx_sim_control_reg_read(reg_t *val) {
    *val = 0;
    return true;
}
bool cmicx_sim_control_reg_write(reg_t val) {

    if (REG_BIT_GET(val,CMICX_SIM_CONTROL_REG_PCID_MEM_DUMP_BIT)==true) {
        cmicx_pcid_mem_dump();
    }
    if (REG_BIT_GET(val,CMICX_SIM_CONTROL_REG_SWITCH_REGISTERS_DUMP_BIT)==true) {
        cmicx_switch_registers_dump();
    }
    if (REG_BIT_GET(val,CMICX_SIM_CONTROL_REG_SWITCH_MEMORIES_DUMP_BIT)==true) {
        cmicx_switch_memories_dump();
    }

    if (REG_BIT_GET(val,CMICX_SIM_CONTROL_REG_LOG_LEVEL_CUTOFF_SET_BIT)==true) {
        cmicx_log_level_cutoff_set((int)REG_FIELD_GET(val,CMICX_SIM_CONTROL_REG_LOG_LEVEL_CUTOFF_MSB,CMICX_SIM_CONTROL_REG_LOG_LEVEL_CUTOFF_LSB));
    }

    return true;
}


