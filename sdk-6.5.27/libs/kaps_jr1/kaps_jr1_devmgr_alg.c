

#include <stdint.h>

#include "kaps_jr1_utility.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_lpm_algo.h"



#define KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, i)  \
    device->num_of_piowrs += i;                 \
    if ((device->xpt == NULL) || device->is_blackhole_mode \
       || (device->issu_in_progress == 1))      \
    {                                           \
        return KAPS_JR1_OK;                          \
    }



#define KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, i)   \
        device->num_of_piords += i;             \
        if (device->xpt == NULL)                 \
        {                                       \
            return KAPS_JR1_OK;                      \
        }




kaps_jr1_status
kaps_jr1_dm_dma_bb_write(
    struct kaps_jr1_device *device,
    uint32_t bb_num,
    uint32_t offset,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC5, device->uda_offset + bb_num, offset, length, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_dma_bb_read(
    struct kaps_jr1_device * device,
    uint32_t bb_num,
    uint32_t row_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !o_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC5, device->uda_offset + bb_num, row_num, length, o_data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_alg_reg_write(
    struct kaps_jr1_device * device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * register_data)
{
    struct kaps_jr1_xpt *xpt;
    uint32_t offset;

    if (!device || !register_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    offset = device->uda_offset;
    
    if (device->hw_res->total_small_bb)
        offset = device->small_bb_offset;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC0, bb_index + offset, reg_num, length, register_data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_alg_reg_read(
    struct kaps_jr1_device * device,
    uint32_t bb_index,
    uint32_t reg_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_jr1_xpt *xpt;
    uint32_t offset;

    if (!device || !o_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    offset = device->uda_offset;
    
    if (device->hw_res->total_small_bb)
        offset = device->small_bb_offset;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC0, bb_index + offset,
                                reg_num, length, o_data));

    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_dm_bb_write(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db,
    uint32_t bb_num,
    uint32_t row_num,
    uint8_t is_final_level,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;
    uint32_t offset;
    uint32_t hw_bb_num;
    uint32_t kaps_jr1_func;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    offset = kaps_jr1_device_get_final_level_offset(device, db);

    hw_bb_num = bb_num;
    kaps_jr1_func = KAPS_JR1_FUNC2;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, kaps_jr1_func, offset + hw_bb_num, row_num, length, data));

    return KAPS_JR1_OK;
}




kaps_jr1_status
kaps_jr1_dm_bb_read(
    struct kaps_jr1_device * device,
    uint32_t bb_num,
    uint32_t row_num,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !o_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC2, device->uda_offset + bb_num, row_num, length, o_data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_rit_write(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t rit_addr,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    if (device->kaps_jr1_shadow && device->kaps_jr1_shadow->ads_blks)
    {
        struct kaps_jr1_ads *write_ads = &device->kaps_jr1_shadow->ads_blks[blk_num].ads_rows[rit_addr];

        write_ads->bpm_ad = KapsJr1ReadBitsInArrray(data, 16, 19, 0);
        write_ads->bpm_len = KapsJr1ReadBitsInArrray(data, 16, 27, 20);
        write_ads->row_offset = KapsJr1ReadBitsInArrray(data, 16, 31, 28);

        write_ads->format_map_00 = KapsJr1ReadBitsInArrray(data, 16, 35, 32);
        write_ads->format_map_01 = KapsJr1ReadBitsInArrray(data, 16, 39, 36);
        write_ads->format_map_02 = KapsJr1ReadBitsInArrray(data, 16, 43, 40);
        write_ads->format_map_03 = KapsJr1ReadBitsInArrray(data, 16, 47, 44);
        write_ads->format_map_04 = KapsJr1ReadBitsInArrray(data, 16, 51, 48);
        write_ads->format_map_05 = KapsJr1ReadBitsInArrray(data, 16, 55, 52);
        write_ads->format_map_06 = KapsJr1ReadBitsInArrray(data, 16, 59, 56);
        write_ads->format_map_07 = KapsJr1ReadBitsInArrray(data, 16, 63, 60);
        write_ads->format_map_08 = KapsJr1ReadBitsInArrray(data, 16, 67, 64);
        write_ads->format_map_09 = KapsJr1ReadBitsInArrray(data, 16, 71, 68);
        write_ads->format_map_10 = KapsJr1ReadBitsInArrray(data, 16, 75, 72);
        write_ads->format_map_11 = KapsJr1ReadBitsInArrray(data, 16, 79, 76);
        write_ads->format_map_12 = KapsJr1ReadBitsInArrray(data, 16, 83, 80);
        write_ads->format_map_13 = KapsJr1ReadBitsInArrray(data, 16, 87, 84);
        write_ads->format_map_14 = KapsJr1ReadBitsInArrray(data, 16, 91, 88);
        write_ads->format_map_15 = KapsJr1ReadBitsInArrray(data, 16, 95, 92);

        write_ads->key_shift = KapsJr1ReadBitsInArrray(data, 16, 103, 96);
        write_ads->bkt_row = KapsJr1ReadBitsInArrray(data, 16, 112, 104);

    }

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC4, device->dba_offset + blk_num, rit_addr, length, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_iit_write(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    if (device->kaps_jr1_shadow )
    {
        struct kaps_jr1_ads *write_ads;


        write_ads = &device->kaps_jr1_shadow->ads_blks[blk_num].ads_rows[iit_addr];


        write_ads->bpm_ad = KapsJr1ReadBitsInArrray(data, 16, 19, 0);
        write_ads->bpm_len = KapsJr1ReadBitsInArrray(data, 16, 27, 20);
        write_ads->row_offset = KapsJr1ReadBitsInArrray(data, 16, 31, 28);

        write_ads->format_map_00 = KapsJr1ReadBitsInArrray(data, 16, 35, 32);
        write_ads->format_map_01 = KapsJr1ReadBitsInArrray(data, 16, 39, 36);
        write_ads->format_map_02 = KapsJr1ReadBitsInArrray(data, 16, 43, 40);
        write_ads->format_map_03 = KapsJr1ReadBitsInArrray(data, 16, 47, 44);
        write_ads->format_map_04 = KapsJr1ReadBitsInArrray(data, 16, 51, 48);
        write_ads->format_map_05 = KapsJr1ReadBitsInArrray(data, 16, 55, 52);
        write_ads->format_map_06 = KapsJr1ReadBitsInArrray(data, 16, 59, 56);
        write_ads->format_map_07 = KapsJr1ReadBitsInArrray(data, 16, 63, 60);
        write_ads->format_map_08 = KapsJr1ReadBitsInArrray(data, 16, 67, 64);
        write_ads->format_map_09 = KapsJr1ReadBitsInArrray(data, 16, 71, 68);
        write_ads->format_map_10 = KapsJr1ReadBitsInArrray(data, 16, 75, 72);
        write_ads->format_map_11 = KapsJr1ReadBitsInArrray(data, 16, 79, 76);
        write_ads->format_map_12 = KapsJr1ReadBitsInArrray(data, 16, 83, 80);
        write_ads->format_map_13 = KapsJr1ReadBitsInArrray(data, 16, 87, 84);
        write_ads->format_map_14 = KapsJr1ReadBitsInArrray(data, 16, 91, 88);
        write_ads->format_map_15 = KapsJr1ReadBitsInArrray(data, 16, 95, 92);

        write_ads->bkt_row = KapsJr1ReadBitsInArrray(data, 16, 108, 96);
        write_ads->reserved = KapsJr1ReadBitsInArrray(data, 16, 111, 109);
        write_ads->key_shift = KapsJr1ReadBitsInArrray(data, 16, 119, 112);
        write_ads->ecc = KapsJr1ReadBitsInArrray(data, 16, 127, 120);
    }

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC4, device->dba_offset + blk_num, iit_addr,
                                length, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_iit_read(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t iit_addr,
    uint32_t length,
    uint8_t * o_data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !o_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    if (device->kaps_jr1_shadow && device->nv_ptr)
    {
        struct kaps_jr1_ads *write_ads;
        write_ads = &device->kaps_jr1_shadow->ads_blks[blk_num].ads_rows[iit_addr];
        KapsJr1WriteBitsInArray(o_data, 16, 19, 0, write_ads->bpm_ad);
        KapsJr1WriteBitsInArray(o_data, 16, 27, 20, write_ads->bpm_len);
        KapsJr1WriteBitsInArray(o_data, 16, 31, 28, write_ads->row_offset);

        KapsJr1WriteBitsInArray(o_data, 16, 35, 32, write_ads->format_map_00);
        KapsJr1WriteBitsInArray(o_data, 16, 39, 36, write_ads->format_map_01);
        KapsJr1WriteBitsInArray(o_data, 16, 43, 40, write_ads->format_map_02);
        KapsJr1WriteBitsInArray(o_data, 16, 47, 44, write_ads->format_map_03);
        KapsJr1WriteBitsInArray(o_data, 16, 51, 48, write_ads->format_map_04);
        KapsJr1WriteBitsInArray(o_data, 16, 55, 52, write_ads->format_map_05);
        KapsJr1WriteBitsInArray(o_data, 16, 59, 56, write_ads->format_map_06);
        KapsJr1WriteBitsInArray(o_data, 16, 63, 60, write_ads->format_map_07);
        KapsJr1WriteBitsInArray(o_data, 16, 67, 64, write_ads->format_map_08);
        KapsJr1WriteBitsInArray(o_data, 16, 71, 68, write_ads->format_map_09);
        KapsJr1WriteBitsInArray(o_data, 16, 75, 72, write_ads->format_map_10);
        KapsJr1WriteBitsInArray(o_data, 16, 79, 76, write_ads->format_map_11);
        KapsJr1WriteBitsInArray(o_data, 16, 83, 80, write_ads->format_map_12);
        KapsJr1WriteBitsInArray(o_data, 16, 87, 84, write_ads->format_map_13);
        KapsJr1WriteBitsInArray(o_data, 16, 91, 88, write_ads->format_map_14);
        KapsJr1WriteBitsInArray(o_data, 16, 95, 92, write_ads->format_map_15);

        KapsJr1WriteBitsInArray(o_data, 16, 108, 96, write_ads->bkt_row);
        KapsJr1WriteBitsInArray(o_data, 16, 111, 109, write_ads->reserved);
        KapsJr1WriteBitsInArray(o_data, 16, 119, 112, write_ads->key_shift);
        KapsJr1WriteBitsInArray(o_data, 16, 127, 120, write_ads->ecc);
        return KAPS_JR1_OK;
    }

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC4, device->dba_offset + blk_num, iit_addr,
                                length, o_data));

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_dm_rpb_write_tcam_format_1(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit,
    uint8_t is_xy)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t write_buf[2 * (KAPS_JR1_RPB_WIDTH_8 + 1)];
    uint32_t write_buf_len_8;
    struct kaps_jr1_dba_entry write_rpb_key0, write_rpb_key1;
    uint32_t i;

    write_buf_len_8 = KAPS_JR1_RPB_WIDTH_8 + 1;
    if (device->combine_tcam_xy_write)
    {
        write_buf_len_8 = 2 * (KAPS_JR1_RPB_WIDTH_8 + 1);
    }

    xpt = device->xpt;
    
    switch (valid_bit)
    {
        case 0:
            kaps_jr1_memset(write_buf, 0, sizeof(write_buf));

            KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                        write_buf_len_8, write_buf));
            KAPS_JR1_STRY(xpt->kaps_jr1_command
                      (xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num + 1,
                       write_buf_len_8, write_buf));

            break;

        case 3:
            if (!is_xy)
            {
                kaps_jr1_convert_dm_to_xy(data, mask, write_rpb_key0.key, write_rpb_key1.key, KAPS_JR1_RPB_WIDTH_8);
            }
            else
            {
                kaps_jr1_memcpy(write_rpb_key0.key, data, KAPS_JR1_RPB_WIDTH_8);
                kaps_jr1_memcpy(write_rpb_key1.key, mask, KAPS_JR1_RPB_WIDTH_8);
            }

            write_rpb_key0.is_valid = 0x3;
            write_rpb_key1.is_valid = 0x3;
            write_rpb_key0.resv = write_rpb_key0.pad = 0;
            write_rpb_key1.resv = write_rpb_key1.pad = 0;

            kaps_jr1_memset(write_buf, 0, sizeof(write_buf));
               
            for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++)
            {
                KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key1.key[i]);
            }
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key1.resv);
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key1.is_valid);
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 167, 166, 0); 

            KAPS_JR1_STRY(xpt->kaps_jr1_command
                      (xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num + 1,
                       KAPS_JR1_RPB_WIDTH_8 + 1, write_buf));

            kaps_jr1_memset(write_buf, 0, sizeof(write_buf));

            for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
            {
                KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2, write_rpb_key0.key[i]);
            }
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2, write_rpb_key0.resv);
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0, write_rpb_key0.is_valid);
            KapsJr1WriteBitsInArray(write_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 167, 166, 0); 

            KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                        KAPS_JR1_RPB_WIDTH_8 + 1, write_buf));


            break;
        default:
            kaps_jr1_sassert(valid_bit);
            break;
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_dm_rpb_write_tcam_format_2(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit,
    uint8_t is_xy)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t write_buf[2 * (KAPS_JR1_RPB_WIDTH_8 + 1)];
    uint32_t write_buf_len_8;
    struct kaps_jr1_dba_entry write_rpb_key0, write_rpb_key1;
    uint32_t i;
    
    write_buf_len_8 = KAPS_JR1_RPB_WIDTH_8 + 1;
    if (device->combine_tcam_xy_write)
    {
        write_buf_len_8 = 2 * (KAPS_JR1_RPB_WIDTH_8 + 1);
    }

    xpt = device->xpt;

    switch (valid_bit)
    {
        case 0:
            kaps_jr1_memset(write_buf, 0, sizeof(write_buf));

            
            
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 167, 166, 0x3);

            
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 168 + 167, 168 + 166, 0x3);
             
            KAPS_JR1_STRY(xpt->kaps_jr1_command
                      (xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                       write_buf_len_8, write_buf));


            
            break;

        case 3:
            if (!is_xy)
            {
                kaps_jr1_convert_dm_to_xy(data, mask, write_rpb_key0.key, write_rpb_key1.key, KAPS_JR1_RPB_WIDTH_8);
            }
            else
            {
                kaps_jr1_memcpy(write_rpb_key0.key, data, KAPS_JR1_RPB_WIDTH_8);
                kaps_jr1_memcpy(write_rpb_key1.key, mask, KAPS_JR1_RPB_WIDTH_8);
            }

            write_rpb_key0.is_valid = 0x3;
            write_rpb_key1.is_valid = 0x3;
            write_rpb_key0.resv = write_rpb_key0.pad = 0;
            write_rpb_key1.resv = write_rpb_key1.pad = 0;

            kaps_jr1_memset(write_buf, 0, sizeof(write_buf));

            
            for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
            {
                KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 168 + ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                 168 + ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4, write_rpb_key0.key[i]);
            }

            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 168 + 3, 168 + 0, write_rpb_key0.resv);
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 168 + 165, 168 + 164, write_rpb_key0.is_valid);
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 168 + 167, 168 + 166, 0x3);        

            
            for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++)
            {
                KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4, write_rpb_key1.key[i]);
            }

            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 3, 0, write_rpb_key1.resv);

            
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 165, 164, 0x0);
            KapsJr1WriteBitsInArray(write_buf, write_buf_len_8, 167, 166, 0x3);    

            KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC1, device->dba_offset + blk_num, 2 * row_num,
                                        write_buf_len_8, write_buf));
        
            break;
            
        default:
            kaps_jr1_sassert(valid_bit);
            break;
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_dm_rpb_write(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    uint8_t * data,
    uint8_t * mask,
    uint8_t valid_bit)
{
    kaps_jr1_status status;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 2);

    if (device->kaps_jr1_shadow && device->kaps_jr1_shadow->rpb_blks)
    {
        

        {
            kaps_jr1_memcpy(device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].data, data, KAPS_JR1_RPB_WIDTH_8);
            kaps_jr1_memcpy(device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].mask, mask, KAPS_JR1_RPB_WIDTH_8);
        }


        device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_data = valid_bit;
        device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_mask = valid_bit;
    }

    if (device->tcam_format == KAPS_JR1_TCAM_FORMAT_2)
    {
        status = kaps_jr1_dm_rpb_write_tcam_format_2(device, blk_num, row_num,
                                        data, mask, valid_bit, 0);
    }
    else if (device->tcam_format == KAPS_JR1_TCAM_FORMAT_1)
    {
        status = kaps_jr1_dm_rpb_write_tcam_format_1(device, blk_num, row_num,
                                        data, mask, valid_bit, 0);
    }
    else 
    {
        kaps_jr1_assert(0, "Incorrect TCAM Format \n");
        status = KAPS_JR1_INTERNAL_ERROR;
    }

    return status;
}

kaps_jr1_status
kaps_jr1_dm_rpb_read(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t row_num,
    struct kaps_jr1_dba_entry * o_entry_x,
    struct kaps_jr1_dba_entry * o_entry_y)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t read_buf[KAPS_JR1_DBA_WIDTH_8 + 1];
    uint32_t i;

    if (!device || !o_entry_x || !o_entry_y)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 2);

    if (device->kaps_jr1_shadow && device->nv_ptr)
    {
        kaps_jr1_memcpy(o_entry_x->key, device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].data, KAPS_JR1_RPB_WIDTH_8);
        kaps_jr1_memcpy(o_entry_y->key, device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].mask, KAPS_JR1_RPB_WIDTH_8);
        o_entry_x->is_valid = device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_data;
        o_entry_y->is_valid = device->kaps_jr1_shadow->rpb_blks[blk_num].rpb_rows[row_num].rpb_valid_mask;
        return KAPS_JR1_OK;
    }


    xpt = device->xpt;

    
    kaps_jr1_memset(read_buf, 0, sizeof(read_buf));
    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC1, device->dba_offset + blk_num, row_num + 1,
                                KAPS_JR1_RPB_WIDTH_8 + 1, read_buf));

    if (device->tcam_format == KAPS_JR1_TCAM_FORMAT_2)
    {
        for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4);
        }

        o_entry_y->resv = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 3, 0);
        o_entry_y->is_valid = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
        {
            o_entry_y->key[i] = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_y->resv = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2);
        o_entry_y->is_valid = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0);

    }

    
    kaps_jr1_memset(read_buf, 0, sizeof(read_buf));
    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC1, device->dba_offset + blk_num, row_num,
                                KAPS_JR1_RPB_WIDTH_8 + 1, read_buf));

    if (device->tcam_format == KAPS_JR1_TCAM_FORMAT_2)
    {
        for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4);
        }

        o_entry_x->resv = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 3, 0);
        o_entry_x->is_valid = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 165, 164);

    }
    else
    {
        for (i = 0; i < KAPS_JR1_DBA_WIDTH_8; i++)
        {
            o_entry_x->key[i] = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                                 ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2);
        }
        o_entry_x->resv = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2);
        o_entry_x->is_valid = KapsJr1ReadBitsInArrray(read_buf, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0);

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_reset_blks(
    struct kaps_jr1_device * device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_JR1_OK;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_EXTENDED, KAPS_JR1_FUNC15, blk_nr, row_nr, nbytes, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_enumerate_blks(
    struct kaps_jr1_device * device,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_JR1_OK;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(device->xpt, KAPS_JR1_CMD_EXTENDED, KAPS_JR1_FUNC14, blk_nr, row_nr, nbytes, data));

    return KAPS_JR1_OK;
}

