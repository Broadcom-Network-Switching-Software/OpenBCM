

#ifndef __KAPS_JR1_DEVICE_ALG_H
#define __KAPS_JR1_DEVICE_ALG_H

#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_bitmap.h"

#define KAPS_JR1_BKT_WIDTH_1 (480)

#define KAPS_JR1_BKT_WIDTH_8 (60)

#define KAPS_JR1_BKT_MIN_NUM_ROWS (256)

#define KAPS_JR1_BKT_DEFAULT_NUM_ROWS (1024)

#define KAPS_JR1_BKT_MAX_NUM_ROWS (16 * 1024)

#define KAPS_JR1_ADS_WIDTH_1 (128)

#define KAPS_JR1_ADS_WIDTH_8 (16)

#define KAPS_JR1_ADS_MIN_NUW_ROWS (1 * 1024)

#define KAPS_JR1_ADS_DEFAULT_NUM_ROWS (2 * 1024)

#define KAPS_JR1_MIN_NUM_ADS_BLOCKS (2)

#define KAPS_JR1_DEFAULT_NUM_ADS_BLOCKS (4)

#define KAPS_JR1_MAX_NUM_ADS_BLOCKS (8)

#define KAPS_JR1_RPB_WIDTH_1 (160)

#define KAPS_JR1_RPB_WIDTH_8 (20)

#define KAPS_JR1_RPB_MIN_BLOCK_SIZE (512)

#define KAPS_JR1_RPB_DEFAULT_BLOCK_SIZE (2 * 1024)

#define KAPS_JR1_RPB_MAX_BLOCK_SIZE (8 * 1024)

#define KAPS_JR1_MIN_NUM_RPB_BLOCKS (2)

#define KAPS_JR1_DEFAULT_NUM_RPB_BLOCKS (4)

#define KAPS_JR1_NUM_FORMATS (16)

#define KAPS_JR1_MAX_NUM_BB (256)

#define KAPS_JR1_DEFAULT_MIN_NUM_BB (16)

#define KAPS_JR1_MAX_SEARCH_TAG (0x7F)

#define KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE (16)

#define KAPS_JR1_MAX_NUM_GRANULARITIES (15)

#define KAPS_JR1_AD_ARRAY_START_BIT_POS (4)

#define KAPS_JR1_AD_ARRAY_END_BIT_POS (23)

#define KAPS_JR1_DMA_WIDTH_1 (128)

#define KAPS_JR1_DMA_NUM_ROWS (1024 * 4)

#define KAPS_JR1_DMA_TAG_MAX (255)

#define KAPS_JR1_RPB_BLOCK_START (1)


#define KAPS_JR1_BB_GLOBAL_CONFIG_REG_ADDR (0x21)

#define KAPS_JR1_ADS_MAX_NUM_ROWS (32 * 1024)

#define KAPS_JR1_TOTAL_NUM_SMALL_BB (16)

#define KAPS_JR1_SMALL_BBS_PUBLIC_PRIVATE (16)

#define KAPS_JR1_BMP_IN_SMALL_BB_8 (2)



enum KAPS_JR1_HW_BLOCK_TYPE
{
    KAPS_JR1_BLOCK_TYPE_RPB,
    KAPS_JR1_BLOCK_TYPE_ADS1,
    KAPS_JR1_BLOCK_TYPE_SMALL_BB,
    KAPS_JR1_BLOCK_TYPE_ADS2,
    KAPS_JR1_BLOCK_TYPE_LARGE_BB
};






struct kaps_jr1_ads
{
    uint32_t bpm_ad:20;     

    uint32_t bpm_len:8;         
    uint32_t row_offset:4;        

    uint32_t format_map_00:4;      
    uint32_t format_map_01:4;
    uint32_t format_map_02:4;
    uint32_t format_map_03:4;
    uint32_t format_map_04:4;
    uint32_t format_map_05:4;
    uint32_t format_map_06:4;
    uint32_t format_map_07:4;

    uint32_t format_map_08:4;
    uint32_t format_map_09:4;
    uint32_t format_map_10:4;
    uint32_t format_map_11:4;
    uint32_t format_map_12:4;
    uint32_t format_map_13:4;
    uint32_t format_map_14:4;
    uint32_t format_map_15:4;

    uint32_t bkt_row:14;            

    uint32_t reserved:2;            
    uint32_t key_shift:8;           
    uint32_t ecc:8;                
};


struct kaps_jr1_rpb_entry
{
    uint8_t data[KAPS_JR1_RPB_WIDTH_8]; 
    uint8_t mask[KAPS_JR1_RPB_WIDTH_8]; 
    uint8_t rpb_valid_data;         
    uint8_t rpb_valid_mask;         
    uint8_t rpb_tbl_id;             
};


struct kaps_jr1_shadow_rpb
{
    struct kaps_jr1_rpb_entry *rpb_rows;  
};

struct kaps_jr1_shadow_bkt_row
{
    uint8_t data[KAPS_JR1_BKT_WIDTH_8];
};


struct kaps_jr1_shadow_bkt
{
    struct kaps_jr1_shadow_bkt_row *bkt_rows;  
};


struct kaps_jr1_shadow_ads
{
    struct kaps_jr1_ads *ads_rows; 
};


struct kaps_jr1_shadow_ads2
{
    struct kaps_jr1_ads *ads_rows;
};


struct kaps_jr1_shadow_ads2_overlay
{
    int32_t *x_table; 
    struct kaps_jr1_fast_bitmap it_fbmp; 
    uint32_t num_free_it_slots; 
};


struct kaps_jr1_sub_ab
{
    int32_t lower_index; 
    int32_t upper_index; 
    int32_t small_bb_num; 
    int32_t row_num; 
    int32_t sub_ab_gran; 
    uint8_t free_slot_bmp[KAPS_JR1_BMP_IN_SMALL_BB_8]; 
};


struct kaps_jr1_shadow_ab_to_small_bb
{
    struct kaps_jr1_sub_ab sub_ab_bricks[KAPS_JR1_TOTAL_NUM_SMALL_BB];  
    uint32_t num_bricks; 
};






struct kaps_jr1_shadow_device
{
    struct kaps_jr1_shadow_rpb *rpb_blks; 
    struct kaps_jr1_shadow_ads *ads_blks; 
    struct kaps_jr1_shadow_bkt *bkt_blks; 
    struct kaps_jr1_shadow_bkt *small_bbs; 
    struct kaps_jr1_shadow_ads2 *ads2_blks; 
    struct kaps_jr1_shadow_ads2_overlay *ads2_overlay; 
    struct kaps_jr1_shadow_ab_to_small_bb *ab_to_small_bb; 
};





kaps_jr1_status kaps_jr1_dm_dma_bb_write(
    struct kaps_jr1_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * data);


kaps_jr1_status kaps_jr1_dm_dma_bb_read(
    struct kaps_jr1_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * o_data);


kaps_jr1_status kaps_jr1_dm_bb_write(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t bb_num,
    uint32_t offset,
    uint8_t is_final_level,
    uint32_t length,
    uint8_t * data);


kaps_jr1_status kaps_jr1_dm_bb_read(
    struct kaps_jr1_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * o_data);


kaps_jr1_status kaps_jr1_dm_alg_reg_write(
    struct kaps_jr1_device *device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * register_data);


kaps_jr1_status kaps_jr1_dm_alg_reg_read(
    struct kaps_jr1_device *device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * o_data);

kaps_jr1_status kaps_jr1_dm_rit_write(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t rit_addr,
    uint32_t length,
    uint8_t * data);


kaps_jr1_status kaps_jr1_dm_iit_write(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * data);


kaps_jr1_status kaps_jr1_dm_iit_read(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * o_data);


kaps_jr1_status kaps_jr1_dm_rpb_write(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit);


kaps_jr1_status kaps_jr1_dm_rpb_read(
    struct kaps_jr1_device *device,
    uint32_t blk_num,
    uint32_t row_num,
    struct kaps_jr1_dba_entry *o_entry_x,
    struct kaps_jr1_dba_entry *o_entry_y);


kaps_jr1_status kaps_jr1_dm_reset_blks(
    struct kaps_jr1_device *device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data);


kaps_jr1_status kaps_jr1_dm_enumerate_blks(
    struct kaps_jr1_device *device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data);



#endif
