

#include "kaps_jr1_allocator.h"
#include "kaps_jr1_model_internal.h"
#include "kaps_jr1_model_ads.h"


kaps_jr1_status kaps_jr1_c_model_ads_create(struct kaps_jr1_c_model *m, uint32_t ads_id, struct kaps_jr1_c_model_ads_block **ads_blk_pp)
{
    struct kaps_jr1_allocator *alloc = m->alloc;
    struct kaps_jr1_c_model_ads_block *self;

    *ads_blk_pp = NULL;

    self = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_c_model_ads_block));

    if (!self)
        return KAPS_JR1_OUT_OF_MEMORY;

    self->m = m;
    self->ads_id = ads_id;

    *ads_blk_pp = self;

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_ads_destroy(struct kaps_jr1_c_model_ads_block * ads_blk)
{
    struct kaps_jr1_allocator *alloc;

    if (ads_blk) {
        alloc = ads_blk->m->alloc;
        alloc->xfree(alloc->cookie, ads_blk);
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_ads_write(struct kaps_jr1_c_model_ads_block * ads_blk, uint32_t row_nr, struct kaps_jr1_ads *ads_data)
{
    ads_blk->rows[row_nr] = *ads_data;

    return KAPS_JR1_OK;
}

kaps_jr1_status kaps_jr1_c_model_ads_read(struct kaps_jr1_c_model_ads_block * ads_blk, uint32_t row_nr, struct kaps_jr1_ads *ads_data)
{
    *ads_data = ads_blk->rows[row_nr];

    return KAPS_JR1_OK;
}



