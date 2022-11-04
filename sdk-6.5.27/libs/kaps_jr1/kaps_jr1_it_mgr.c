

#include "kaps_jr1_it_mgr.h"
#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_db_internal.h"
#include "kaps_jr1_algo_hw.h"

static kaps_jr1_status
kaps_jr1_it_alloc(
    struct it_mgr *self,
    struct kaps_jr1_db *db,
    enum kaps_jr1_entry_status type,
    uint32_t size,
    struct kaps_jr1_ab_info *ab)
{
    struct kaps_jr1_it_mgr *kaps_jr1_mgr = &self->u.kaps_jr1;
    uint32_t bank_nr = -1;

    if (type != DECISION_TRIE_0)
        return KAPS_JR1_INTERNAL_ERROR;

    bank_nr = ab->ab_num;

    if (bank_nr == -1)
        return KAPS_JR1_INTERNAL_ERROR;

    if (kaps_jr1_mgr->bank[bank_nr].type != UNHANDLED)
        return KAPS_JR1_OUT_OF_UIT;

    kaps_jr1_mgr->bank[bank_nr].type = type;
    kaps_jr1_mgr->bank[bank_nr].is_used_bmp = 1;
    kaps_jr1_mgr->bank[bank_nr].nab = 1;
    kaps_jr1_mgr->bank[bank_nr].db = db;

    ab->base_addr = 0;
    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_it_free(
    struct it_mgr *self,
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info *ab,
    uint32_t size)
{
    uint32_t bank_nr;
    struct kaps_jr1_it_mgr *kaps_jr1_mgr = &self->u.kaps_jr1;

    (void) size;

    bank_nr = ab->ab_num;

    kaps_jr1_mgr->bank[bank_nr].type = UNHANDLED;
    kaps_jr1_mgr->bank[bank_nr].is_used_bmp = 0;
    kaps_jr1_mgr->bank[bank_nr].nab = 0;
    kaps_jr1_mgr->bank[bank_nr].db = NULL;

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_it_mgr_fit_ab(
    struct kaps_jr1_ab_info *ab,
    uint32_t * ab_width,
    uint32_t * size,
    uint32_t * max_ab_per_it_bank)
{
    uint32_t ab_width_in_bytes = *ab_width;
    struct kaps_jr1_db *db = ab->db;

    if (db->parent)
        db = db->parent;

    if (ab_width_in_bytes <= 10)
    {
        *size = 4096;
        *max_ab_per_it_bank = 1;
    }
    else if (ab_width_in_bytes <= 20)
    {
        *size = 2048;
        *max_ab_per_it_bank = 2;
    }
    else if (ab_width_in_bytes <= 40)
    {
        *size = 1024;
        *max_ab_per_it_bank = 4;
    }
    else if (ab_width_in_bytes <= 80)
    {
        *size = 512;
        *max_ab_per_it_bank = 8;
    }
    else
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INTERNAL_ERROR;
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_it_mgr_init(
    struct kaps_jr1_device * device,
    struct kaps_jr1_allocator * alloc,
    struct it_mgr ** mgr_pp)
{
    struct it_mgr *mgr;
    uint32_t i;

    mgr = alloc->xcalloc(alloc->cookie, 1, sizeof(struct it_mgr));
    if (!mgr)
        return KAPS_JR1_OUT_OF_MEMORY;

    mgr->alloc = alloc;
    mgr->device = device;

    for (i = 0; i < KAPS_JR1_MAX_NUM_ADS_BLOCKS; ++i)
    {
        mgr->u.kaps_jr1.bank[i].bank_no = i;
        mgr->u.kaps_jr1.bank[i].is_used_bmp = 0;
        mgr->u.kaps_jr1.bank[i].nab = 0;
        mgr->u.kaps_jr1.bank[i].type = UNHANDLED;
        mgr->u.kaps_jr1.bank[i].db = NULL;
    }

    *mgr_pp = mgr;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_it_mgr_wb_alloc(
    struct it_mgr * self,
    struct kaps_jr1_db * db,
    enum kaps_jr1_entry_status type,
    uint32_t ab_width_in_bytes,
    struct kaps_jr1_ab_info * ab)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;

    kaps_jr1_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    if (db->parent)
        db = db->parent;

    KAPS_JR1_STRY(kaps_jr1_it_alloc(self, db, type, size, ab));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_it_mgr_alloc(
    struct it_mgr * self,
    struct kaps_jr1_db * db,
    enum kaps_jr1_entry_status type,
    enum it_instance_id instance_id,
    uint32_t ab_width_in_bytes,
    struct kaps_jr1_ab_info * ab)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;
    uint32_t ab_width_8 = ab_width_in_bytes;

    if (ab->ab_init_done)
        return KAPS_JR1_OK;

    kaps_jr1_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    
    if (db->type == KAPS_JR1_DB_LPM)
    {
        ab_width_in_bytes = ab_width_8;
        
        if (ab_width_8 < 5)
            max_ab_per_it_bank = 1;
    }

    if (db->parent)
        db = db->parent;

    KAPS_JR1_STRY(kaps_jr1_it_alloc(self, db, type, size, ab));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_it_mgr_free(
    struct it_mgr * self,
    struct kaps_jr1_db *db,
    struct kaps_jr1_ab_info * ab,
    uint32_t ab_width_8,
    enum kaps_jr1_entry_status type)
{
    uint32_t size = 0;
    uint32_t max_ab_per_it_bank = 0;
    uint32_t ab_width_in_bytes = ab_width_8;

    kaps_jr1_it_mgr_fit_ab(ab, &ab_width_in_bytes, &size, &max_ab_per_it_bank);

    
    if (ab->db->type == KAPS_JR1_DB_LPM)
    {
        ab_width_in_bytes = ab_width_8;
        
        if (ab_width_8 < 5)
            max_ab_per_it_bank = 1;
    }

    KAPS_JR1_STRY(kaps_jr1_it_free(self, db, ab, size));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_it_mgr_destroy(
    struct it_mgr * self)
{
    if (self == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;
    else
    {
        struct kaps_jr1_allocator *alloc = self->alloc;

        alloc->xfree(alloc->cookie, self);

        return KAPS_JR1_OK;
    }
}
