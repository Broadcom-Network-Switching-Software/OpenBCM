/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim_pcid.h
 *
 * header for the cmic module
 */


#ifndef _CMICX_SIM_PCID_H_
#define _CMICX_SIM_PCID_H_


/* includes */
#include "pcid.h"


/* pcid functions */
bool cmicx_update(pcid_info_t *pcid_info);
bool cmicx_pcid_register_add(addr_t,pcid_read_func_t*,pcid_write_func_t*);
bool cmicx_pcid_register_read(addr_t addr,reg_t *reg);
bool cmicx_pcid_register_write(addr_t addr,reg_t reg);
bool cmicx_pcid_interrupt_send(pcid_info_t *pcid_info);
bool cmicx_pcid_mem_read(pcid_info_t *pcid_info, addr_t addr,void *mem,int amount);
bool cmicx_pcid_mem_write(pcid_info_t *pcid_info, addr_t addr,void *mem,int amount);
void cmicx_pcid_mem_dump(void);


/* main schan access functions */
bool cmicx_schan_init(cmicx_schan_data_t *schan_data,char *id_string);
bool cmicx_schan_update(pcid_info_t *pcid_info, cmicx_schan_data_t *schan_data);

/* schan support functions */
bool cmic_schan_access_start(cmicx_schan_data_t *schan_data);
bool cmic_schan_access_abort(cmicx_schan_data_t *schan_data);
bool cmic_schan_ctrl_read(reg_t *val,cmicx_schan_data_t *schan_data);
bool cmic_schan_ctrl_write(reg_t val,cmicx_schan_data_t *schan_data);
bool cmic_schan_err_read(reg_t *val,cmicx_schan_data_t *schan_data);
bool cmic_schan_ack_data_beat_count_read(reg_t *val,cmicx_schan_data_t *schan_data);

/* sbus specific support functions (may be used by sbus dma also) */
void cmic_schan_command_build(schan_command_opcode opcode,int dport,int dlen,bool err,char ecode,bool dma,char bank,bool nack,reg_t *command);
void cmic_schan_sbus_ring_get(int blockid,int *ringnum);
bool cmic_schan_sbus_command_start(reg_t sbus_command,bool *is_write);
bool cmic_schan_sbus_command_complete(pcid_info_t *pcid_info, reg_t *sbus_command,reg_t *sbus_command_ack,int expected_resp_words,int *num_words_in_command);

/*  main sbus_dma access functions */
bool cmicx_sbus_dma_init(cmicx_sbus_dma_data_t *sbus_dma_data,char *id_string);
bool cmicx_sbus_dma_update(pcid_info_t *pcid_info, cmicx_sbus_dma_data_t *sbus_dma_data);

/* main packet dma access functions */
bool cmicx_packet_dma_global_init(char *ip_file_name,char *ep_file_name,char *loopback_file_name);
bool cmicx_packet_dma_init(cmicx_packet_dma_data_t *packet_dma_data,char *id_string,int chan);
bool cmicx_packet_dma_global_update(void);
bool cmicx_packet_dma_update(pcid_info_t *pcid_info, cmicx_packet_dma_data_t *packet_dma_data);

/* support functions */
bool cmic_pktdma_access_start(cmicx_packet_dma_data_t *packet_dma_data);
bool cmic_pktdma_access_abort(cmicx_packet_dma_data_t *packet_dma_data);
void cmic_packet_dma_descriptor_done(pcid_info_t *pcid_info, cmicx_packet_dma_data_t *packet_dma_data);
void cmic_packet_dma_chain_done(cmicx_packet_dma_data_t *packet_dma_data);
void cmic_packet_dma_interrupts_post(cmicx_packet_dma_data_t *packet_dma_data);



#endif   /* CMICX_SIM_PCID_H */

