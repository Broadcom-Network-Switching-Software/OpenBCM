


#include "kaps_jr1_allocator.h"
#include "kaps_jr1_model_internal.h"
#include "kaps_jr1_model_hb.h"






kaps_jr1_status kaps_jr1_c_model_hb_create(struct kaps_jr1_c_model *m, uint8_t abs_hb_blk_nr, struct kaps_jr1_c_model_hb_block ** hb_pp)
{
    struct kaps_jr1_c_model_hb_block *self_p = NULL;
    struct kaps_jr1_allocator *alloc = m->alloc;
    uint32_t num_bb_in_one_bb_cascade = m->config.num_bb_in_one_bb_cascade;

    if (num_bb_in_one_bb_cascade == 0)
        return KAPS_JR1_INVALID_ARGUMENT;

    *hb_pp = NULL;

    self_p = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_c_model_hb_block));
    if (!self_p)
        return KAPS_JR1_OUT_OF_MEMORY;

    self_p->m = m;
    self_p->abs_hb_blk_nr = abs_hb_blk_nr;
    self_p->blk_nr_in_bb_cascade = abs_hb_blk_nr % num_bb_in_one_bb_cascade;
    self_p->lpu_nr = abs_hb_blk_nr / num_bb_in_one_bb_cascade ;

    *hb_pp = self_p;

    return KAPS_JR1_OK;

}



kaps_jr1_status kaps_jr1_c_model_hb_destroy(struct kaps_jr1_c_model_hb_block * hb_block)
{
    struct kaps_jr1_allocator *alloc ;

    if (hb_block != NULL) {
        alloc = hb_block->m->alloc;
        alloc->xfree(alloc->cookie, hb_block);
    }

    return KAPS_JR1_OK;
}



kaps_jr1_status kaps_jr1_c_model_hb_write(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint8_t * data)
{
    kaps_jr1_memcpy(hb_block->rows[row_nr].entry, data, KAPS_JR1_HB_ROW_WIDTH_8);

    return KAPS_JR1_OK;
}



kaps_jr1_status kaps_jr1_c_model_hb_read(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint8_t * data)
{
    kaps_jr1_memcpy(data, hb_block->rows[row_nr].entry, KAPS_JR1_HB_ROW_WIDTH_8);

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_hb_set(struct kaps_jr1_c_model_hb_block * hb_block, uint32_t row_nr, uint32_t bit_num)
{
    uint32_t byte_index = bit_num / 8;
    uint32_t bit_pos_in_byte = bit_num % 8;

    kaps_jr1_sassert(byte_index < KAPS_JR1_HB_ROW_WIDTH_8);
    byte_index = (KAPS_JR1_HB_ROW_WIDTH_8 - 1) - byte_index;

    hb_block->rows[row_nr].entry[byte_index] |= 1u << bit_pos_in_byte;

    return KAPS_JR1_OK;
}

