/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pp.h
 * Purpose:     
 */

#ifndef   _DIAG_PP_H_
#define   _DIAG_PP_H_

#define DIAG_PP_WORDS_PER_ENTRY    4
#define DIAG_PP_WORD_SIZE          5
#define DIAG_PP_WORD_BITS          (DIAG_PP_WORD_SIZE * 32)
#define DIAG_PP_ECC_BITS           9
#define DIAG_PP_ECC_MASK           0x1ff
#define DIAG_PP_ECC_CHUNKS         (DIAG_PP_WORD_SIZE + 1)
#define DIAG_PP_DATA_BYTES         80
#define DIAG_PP_XQ_OFFSET_BYTES    8
#define DIAG_INGBUF_EOF_BIT        0x8    
#define DIAG_INGBUF_LANES_MASK     0x7

typedef struct diag_mem_pp_word_s {
    uint32 data[DIAG_PP_WORD_SIZE];
    uint32 syndrome;
} diag_mem_pp_word_t;

typedef struct diag_mem_pp_entry_s {
    diag_mem_pp_word_t words[DIAG_PP_WORDS_PER_ENTRY];
} diag_mem_pp_entry_t;

extern int diag_dump_xq_packet(int unit, soc_block_t blk, int xq_ptr);
extern int diag_dump_cos_packets(int unit, soc_port_t port, int cos_start, 
                                 int cos_end);
extern int diag_dump_ib_packets(int unit, soc_port_t port);
extern int diag_set_cos_errors(int unit, soc_port_t port, int cos_sel, 
                               int errors, int packet,  int start_bit);

#endif /* _DIAG_PP_H_ */
