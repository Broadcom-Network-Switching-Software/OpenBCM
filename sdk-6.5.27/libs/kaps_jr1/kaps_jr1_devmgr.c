

#include "kaps_jr1_utility.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_algo_hw.h"


#define KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, i)    \
      device->num_of_piowrs += i;                 \
      if ((device->xpt == NULL) || device->is_blackhole_mode \
         || (device->issu_in_progress == 1))      \
      {                                           \
          return KAPS_JR1_OK;                          \
      }                                           \


#define KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, i)     \
      device->num_of_piords += i;                 \
      if (device->xpt == NULL)                    \
          return KAPS_JR1_OK;                          \


kaps_jr1_status
kaps_jr1_dm_init(
    struct kaps_jr1_device *device)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t reg_data[KAPS_JR1_REGISTER_WIDTH_8];

    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    xpt = device->xpt;

    if (!xpt)
    {
        device->id = KAPS_JR1_DEFAULT_DEVICE_ID;
        device->silicon_sub_type = 0;
        return KAPS_JR1_OK;
    }

    KAPS_JR1_STRY(xpt->kaps_jr1_register_read(xpt, KAPS_JR1_REVISION_REGISTER_ADDR, KAPS_JR1_REGISTER_WIDTH_8, reg_data));

    device->id = KapsJr1ReadBitsInArrray(reg_data, KAPS_JR1_REGISTER_WIDTH_8, 31, 16);

    if (device->id < KAPS_JR1_DEFAULT_DEVICE_ID 
        || device->id > KAPS_JR1_QUX_DEVICE_ID)
    {
        
        device->id = KAPS_JR1_DEFAULT_DEVICE_ID;
    }


    device->silicon_sub_type = KapsJr1ReadBitsInArrray(reg_data, KAPS_JR1_REGISTER_WIDTH_8, 15, 0);

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_dm_reg_write(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC0, device->dba_offset + blk_num, reg_num, nbytes, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_reg_read(
    struct kaps_jr1_device * device,
    uint32_t blk_num,
    uint32_t reg_num,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC0, device->dba_offset + blk_num, reg_num, nbytes, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_ad_write(
    struct kaps_jr1_device * device,
    uint32_t ab_num,
    uint32_t ad_addr,
    uint32_t nbytes,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_WRITE, KAPS_JR1_FUNC4, device->dba_offset + ab_num, ad_addr, nbytes, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_ad_read(
    struct kaps_jr1_device * device,
    uint32_t ab_num,
    uint32_t ad_addr,
    uint32_t nbytes,
    uint8_t * o_data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !o_data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_READ(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_command(xpt, KAPS_JR1_CMD_READ, KAPS_JR1_FUNC4, device->dba_offset + ab_num, ad_addr, nbytes, o_data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_search(
    struct kaps_jr1_device * device,
    uint8_t * key,
    enum kaps_jr1_search_interface search_interface,
    struct kaps_jr1_search_result * kaps_jr1_result)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !key || !kaps_jr1_result)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->xpt == NULL)
        return KAPS_JR1_OK;

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_search(xpt, key, search_interface, kaps_jr1_result));

    return KAPS_JR1_OK;
}

static kaps_jr1_status
get_hb_blk_info(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    uint32_t block_num,
    uint8_t * is_small_bb_hb,
    uint8_t * is_bb_hb,
    uint8_t * is_rpb_hb)
{
    *is_small_bb_hb = 0;
    *is_bb_hb = 0;
    *is_rpb_hb = 0;

    if (block_num >= device->uda_offset)
    {
        *is_bb_hb = 1;
    }
    else
    {
        *is_rpb_hb = 1;
    }

    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_dm_hb_read(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db, 
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, block_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (is_bb_hb)
    {
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_read(xpt, block_num, row_num, data));

    }
    else if (is_small_bb_hb)
    {
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_read(xpt, block_num, row_num, data));
    }
    else if (is_rpb_hb)
    {

        uint32_t hb_row = row_num / KAPS_JR1_HB_ROW_WIDTH_1;
        uint32_t bit_pos = row_num % KAPS_JR1_HB_ROW_WIDTH_1;

        KAPS_JR1_STRY(xpt->kaps_jr1_hb_read(xpt, block_num, hb_row, data));

        if (KapsJr1ReadBitsInArrray(data, KAPS_JR1_HB_ROW_WIDTH_8, bit_pos, bit_pos))
        {
            data[0] |= 1u << 7;
        }
        else
        {
            data[0] &= ~(1u << 7);
        }
    }
    else
    {
        kaps_jr1_assert(0, "Incorrect block specified during Hit bit read\n");
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_hb_write(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db,
    uint32_t block_num,
    uint32_t row_num,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, block_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (is_bb_hb)
    {
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_write(xpt, block_num, row_num, data));

    }
    else if (is_small_bb_hb)
    {
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_write(xpt, block_num, row_num, data));
    }
    else if (is_rpb_hb)
    {
        uint32_t hb_row = row_num / KAPS_JR1_HB_ROW_WIDTH_1;
        uint32_t bit_pos = row_num % KAPS_JR1_HB_ROW_WIDTH_1;
        uint8_t buffer[KAPS_JR1_HB_ROW_WIDTH_8];

        KAPS_JR1_STRY(xpt->kaps_jr1_hb_read(xpt, block_num, hb_row, buffer));

        KapsJr1WriteBitsInArray(buffer, KAPS_JR1_HB_ROW_WIDTH_8, bit_pos, bit_pos, data[0] >> 7);

        KAPS_JR1_STRY(xpt->kaps_jr1_hb_write(xpt, block_num, hb_row, buffer));

    }
    else
    {
        kaps_jr1_assert(0, "Incorrect block specified during Hit bit write\n");
    }

    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_dm_hb_dump(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db,
    uint32_t start_blk_num,
    uint32_t start_row_num,
    uint32_t end_blk_num,
    uint32_t end_row_num,
    uint8_t clear_on_read,
    uint8_t * data)
{
    struct kaps_jr1_xpt *xpt;

    if (!device || !data)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    KAPS_JR1_STRY(xpt->kaps_jr1_hb_dump(xpt, start_blk_num, start_row_num, end_blk_num, end_row_num, clear_on_read, data));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_dm_hb_copy(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db *db,
    uint32_t src_blk_num,
    uint32_t src_row_num,
    uint32_t dest_blk_num,
    uint32_t dest_row_num,
    uint16_t source_mask,
    uint8_t rotate_right,
    uint8_t perform_clear)
{
    struct kaps_jr1_xpt *xpt;
    uint8_t is_small_bb_hb = 0, is_bb_hb = 0, is_rpb_hb = 0;

    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_DM_KAPS_JR1_CHECK_XPT_WRITE(device, 1);

    xpt = device->xpt;

    get_hb_blk_info(device, db, src_blk_num, &is_small_bb_hb, &is_bb_hb, &is_rpb_hb);

    if (xpt->kaps_jr1_hb_copy)
    {
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_copy
                  (xpt, src_blk_num, src_row_num, dest_blk_num, dest_row_num, source_mask, rotate_right,
                   perform_clear));

    }
    else
    {
        uint8_t data[KAPS_JR1_HB_ROW_WIDTH_8];

        KAPS_JR1_STRY(xpt->kaps_jr1_hb_read(xpt, src_blk_num, src_row_num, data));
        KAPS_JR1_STRY(xpt->kaps_jr1_hb_write(xpt, dest_blk_num, dest_row_num, data));

    }

    return KAPS_JR1_OK;
}
