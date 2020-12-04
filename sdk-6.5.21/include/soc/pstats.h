/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File for SOC Pktstats structures and routines
 */

#ifndef __SOC_PSTATS_H__
#define __SOC_PSTATS_H__

typedef struct soc_pstats_mem_desc {
    soc_mem_t mem;
    uint32 width;
    uint32 entries;
    uint32 shift;
    int mor_dma;
} soc_pstats_mem_desc_t;

#define MAX_PSTATS_MEM_PER_BLK  32

typedef struct soc_pstats_tbl_desc {
    soc_block_t blk;
    soc_mem_t bmem;
    int pipe_enum;
    int mor_dma;
    soc_pstats_mem_desc_t desc[MAX_PSTATS_MEM_PER_BLK];
    int pattern_set;
    soc_mem_t mem;
    uint32 index;
    uint32 count;
} soc_pstats_tbl_desc_t;

typedef struct soc_pstats_tbl_ctrl {
    soc_block_t blk;
    uint32 tindex;
    uint32 entries;
    uint8 *buff;
    uint32 size;
    int flags;
    sal_sem_t dma_done;
} soc_pstats_tbl_ctrl_t;

extern int soc_pstats_init(int unit);
extern int soc_pstats_deinit(int unit);
extern int soc_pstats_tbl_pattern_get(int unit, soc_pstats_tbl_desc_t *tdesc,
                                      soc_mem_t *mem, uint32 *index, uint32 *count);
extern int soc_pstats_tbl_pattern_set(int unit, soc_pstats_tbl_desc_t *tdesc,
                                      soc_mem_t mem, uint32 index, uint32 count);
extern int soc_pstats_sync(int unit);
extern int soc_pstats_mem_get(int unit, soc_mem_t mem, uint8 *buf, int sync);

#endif /* __SOC_PSTATS_H__ */

