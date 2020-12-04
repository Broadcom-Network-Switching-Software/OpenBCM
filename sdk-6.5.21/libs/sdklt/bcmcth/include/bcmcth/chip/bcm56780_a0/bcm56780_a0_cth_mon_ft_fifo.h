/*! \file bcm56780_a0_cth_mon_ft_fifo.h
 *
 * This file contains defines which are internal to BCM56780 A0 FT LEARN FIFO driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_CTH_MON_FT_FIFO_H
#define BCM56780_A0_CTH_MON_FT_FIFO_H

#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmdrd/chip/bcm56780_a0_defs.h>
#include <bcmbd/bcmbd_sbusdma.h>

/*! Max number of pipes in 56780 */
#define BCM56780_A0_CTH_MON_MAX_PIPES 2

/*! Number of words required to hold one entry of FT_LEARN_NOTIFY_FIFO memory */
#define BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_ENTRY_WSIZE \
                ((FT_LEARN_NOTIFY_FIFOm_SIZE + 3)/4)

/*! Number of entries in FT_LEARN_NOTIFY_FIFO memory */
#define BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_NUM_ENTRIES \
                (FT_LEARN_NOTIFY_FIFOm_MAX - FT_LEARN_NOTIFY_FIFOm_MIN+1)

/*! Get exact match indexing mode from hit index format - Bits 23 to 25 */
#define BCM56780_A0_EXACT_MATCH_MODE_FROM_HIT_IDX_GET(hidx) ((hidx >> 23) & 0x7)

/*! Extract table id from hit index - Bits 20 to 22 */
#define BCM56780_A0_EXACT_MATCH_TABLE_ID_FROM_HIT_IDX_GET(hidx) ((hidx >> 20) & 0x7)

/*! Extract bucket number from hit index - Bits 5 to 19 */
#define BCM56780_A0_EXACT_MATCH_BUCKET_NUM_FROM_HIT_IDX_GET(hidx) ((hidx >> 5) & 0x7fff)

/*! Extract entry number from hit index - Bits 0 to 4 */
#define BCM56780_A0_EXACT_MATCH_ENTRY_NUM_FROM_HIT_IDX_GET(hidx) (hidx & 0x1f)

/*!
 * SINGLE wide index mode. entry number within a bucket can be 4 values
 * represented by just 2 bits.
 * Max number of single wide entries = 128k per pipe. So total number of bits
 * required for entry_num + bucket_num + table_id = 17 bits.
 * Max number of single wide entries per table(bank) = 16K which is 14 bits.
 */
#define BCM56780_A0_EXACT_MATCH_SINGLE_WIDE_IDX_FROM_HIT_IDX_GET(hidx)\
         ((BCM56780_A0_EXACT_MATCH_TABLE_ID_FROM_HIT_IDX_GET(hidx) << 14) |\
          (BCM56780_A0_EXACT_MATCH_BUCKET_NUM_FROM_HIT_IDX_GET(hidx) << 2) |\
          (BCM56780_A0_EXACT_MATCH_ENTRY_NUM_FROM_HIT_IDX_GET(hidx) & 0x3))

/*!
 * DOUBLE wide index mode. entry number within a bucket can only be 2 values.
 * which are 0 and 2. Means only bit 1 is useful.
 * Max number of double wide entries = 64k per pipe. So total number of bits
 * required for entry_num + bucket_num + table_id = 16 bits.
 * Max number of single wide entries per table(bank) = 8K which is 13 bits
 */
#define BCM56780_A0_EXACT_MATCH_DOUBLE_WIDE_IDX_FROM_HIT_IDX_GET(hidx)\
         ((BCM56780_A0_EXACT_MATCH_TABLE_ID_FROM_HIT_IDX_GET(hidx) << 13) |\
          (BCM56780_A0_EXACT_MATCH_BUCKET_NUM_FROM_HIT_IDX_GET(hidx) << 1) |\
          ((BCM56780_A0_EXACT_MATCH_ENTRY_NUM_FROM_HIT_IDX_GET(hidx) >> 1) & 0x1))

/*!
 * QUAD wide index mode. entry number is not useful since it is always 0.
 * Only use the bucket number.
 * Max number of quad wide entries = 32k per pipe. So total number of bits
 * required for entry_num + bucket_num + table_id = 15 bits
 * Max number of single wide entries per table(bank) = 4K which is 12 bits
 */
#define BCM56780_A0_EXACT_MATCH_QUAD_WIDE_IDX_FROM_HIT_IDX_GET(hidx)\
         ((BCM56780_A0_EXACT_MATCH_TABLE_ID_FROM_HIT_IDX_GET(hidx) << 12) |\
         (BCM56780_A0_EXACT_MATCH_BUCKET_NUM_FROM_HIT_IDX_GET(hidx)))


/*! Internal structure representing the FIFO entries in one pipe */
typedef struct bcm56780_a0_ft_fifo_sw_cache_entries_s {
    /*! The SW cache of HW entries per pipe */
    FT_LEARN_NOTIFY_FIFOm_t entries[BCM56780_A0_CTH_MON_FT_LRN_NOTIF_FIFO_NUM_ENTRIES];
}bcm56780_a0_ft_fifo_sw_cache_entries_t;

/*! Internal structure representing the FIFO entries for all pipes */
typedef struct bcm56780_a0_ft_fifo_sw_cache_pipes_s {
    /*! The DMAable SW cache physical address */
    uint64_t pipe_fifo_sw_cache_paddr[BCM56780_A0_CTH_MON_MAX_PIPES];
    /*! The DMAable SW cache virtual address */
    bcm56780_a0_ft_fifo_sw_cache_entries_t *pipe_fifo_sw_cache[BCM56780_A0_CTH_MON_MAX_PIPES];
}bcm56780_a0_ft_fifo_sw_cache_pipes_t;

/*! Internal structure representing the SBUSDMA work pointers for all pipes */
typedef struct bcm56780_a0_ft_fifo_sbusdma_work_ptr_pipes_s {
    /*! The DMA work pointer used for DMAing FT_LEARN_NOTIFY_FIFO memory */
    bcmbd_sbusdma_work_t sbusdma_work[BCM56780_A0_CTH_MON_MAX_PIPES];
}bcm56780_a0_ft_fifo_sbusdma_work_ptr_pipes_t;

/*! Driver structure */
extern bcmcth_mon_flowtracker_fifo_drv_t bcm56780_a0_cth_mon_ft_fifo_drv;

#endif /* BCM56780_A0_CTH_MON_FT_FIFO_H */
