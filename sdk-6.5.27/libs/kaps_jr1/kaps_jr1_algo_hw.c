

#include <stdio.h>

#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_utility.h"
#include "kaps_jr1_ab.h"

static kaps_jr1_status
kaps_jr1_rit_write(
    struct kaps_jr1_device *device,
    struct kaps_jr1_ads *write_ads,
    uint32_t blk_num,
    uint32_t it_addr)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint8_t write_buf[16] = { 0 };
    uint32_t cur_bit_pos, num_bytes;

    num_bytes = device->hw_res->ads_width_8;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, 19, 0, write_ads->bpm_ad);
    KapsJr1WriteBitsInArray(write_buf, num_bytes, 27, 20, write_ads->bpm_len);
    KapsJr1WriteBitsInArray(write_buf, num_bytes, 31, 28, write_ads->row_offset);

    cur_bit_pos = 32;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_00);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_01);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_02);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_03);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_04);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_05);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_06);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_07);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_08);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_09);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_10);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_11);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_12);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_13);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_14);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_15);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, num_bytes, 103, 96, write_ads->key_shift);
    KapsJr1WriteBitsInArray(write_buf, num_bytes, 113, 104, write_ads->bkt_row);

    status = kaps_jr1_dm_rit_write(device, blk_num, it_addr, num_bytes, write_buf);

    return status;
}

kaps_jr1_status
kaps_jr1_algo_hw_write_rpt_data(
    struct kaps_jr1_device * device,
    uint8_t rpt_num,
    uint8_t * data,
    uint8_t * mask,
    uint16_t entry_nr)
{
    KAPS_JR1_STRY(kaps_jr1_dm_rpb_write(device, rpt_num, entry_nr, data, mask, 3));
    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_algo_hw_delete_rpt_data(
    struct kaps_jr1_device * device,
    uint8_t rpt_num,
    uint16_t entry_nr)
{
    uint8_t invalid_data[KAPS_JR1_RPB_WIDTH_8];

    kaps_jr1_memset(invalid_data, 0x0, KAPS_JR1_RPB_WIDTH_8);
    KAPS_JR1_STRY(kaps_jr1_dm_rpb_write(device, rpt_num, entry_nr, invalid_data, invalid_data, 0));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_algo_hw_write_pct(
    struct kaps_jr1_device * device,
    struct kaps_jr1_pct * pct,
    uint8_t pct_num,
    uint16_t entry_nr)
{
    KAPS_JR1_STRY(kaps_jr1_rit_write(device, &pct->u.kaps_jr1, pct_num, entry_nr));
    return KAPS_JR1_OK;

}
