

#include "kaps_jr1_pfx_hash_table.h"
#include "kaps_jr1_math.h"



static kaps_jr1_status
kaps_jr1_pfx_hash_table_ctor(
    struct pfx_hash_table *self,
    struct kaps_jr1_allocator *alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_jr1_db *db)
{
    
    kaps_jr1_sassert(initsz >= 8);
    kaps_jr1_sassert(initsz < (1 << 30));

    
    kaps_jr1_sassert(thresh > 25);
    kaps_jr1_sassert(thresh < 90);

    kaps_jr1_sassert(resizeby > thresh);
    kaps_jr1_sassert(resizeby < 1001);

    initsz = (initsz * 100) / thresh;
    initsz++;

    self->m_nThreshold = (uint16_t) thresh;
    self->m_alloc_p = alloc;

    self->m_nMaxPfxWidth_1 = max_pfx_width_1;

    self->m_nMaxSize = kaps_jr1_find_next_prime(initsz);

    self->m_nResizeBy = resizeby;

    self->m_nThreshSize = (self->m_nMaxSize * thresh) / 100;

    self->m_db = db;

    self->m_slots_p = alloc->xcalloc(alloc->cookie, self->m_nMaxSize, sizeof(struct kaps_jr1_entry *));

    if (!self->m_slots_p)
        return KAPS_JR1_OUT_OF_MEMORY;

    self->m_doffset = d_offset;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_create(
    struct kaps_jr1_allocator * alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_jr1_db * db,
    struct pfx_hash_table ** self_pp)
{
    struct pfx_hash_table *ht;
    kaps_jr1_status status;

    *self_pp = NULL;

    if (db->type != KAPS_JR1_DB_LPM)
        return KAPS_JR1_UNSUPPORTED;

    ht = alloc->xcalloc(alloc->cookie, 1, sizeof(struct pfx_hash_table));
    if (!ht)
        return KAPS_JR1_OUT_OF_MEMORY;
    status = kaps_jr1_pfx_hash_table_ctor(ht, alloc, initsz, thresh, resizeby, max_pfx_width_1, d_offset, db);

    if (status != KAPS_JR1_OK)
    {
        alloc->xfree(alloc->cookie, ht);
        return status;
    }

    *self_pp = ht;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_destroy(
    struct pfx_hash_table * self)
{
    if (self)
    {
        struct kaps_jr1_allocator *alloc = self->m_alloc_p;

        if (self->m_slots_p)
            alloc->xfree(alloc->cookie, self->m_slots_p);

        alloc->xfree(alloc->cookie, self);
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_insert(
    struct pfx_hash_table * self,
    struct kaps_jr1_entry * entry_to_insert)
{
    uint32_t h1, h2, ix, bound;
    int32_t is_inserted = 0;
    kaps_jr1_status status;
    uint32_t len_1;
    uint8_t *data;

    if (self->m_nCurSize >= self->m_nThreshSize)
    {
        status = kaps_jr1_pfx_hash_table_expand(self);
        if (status != KAPS_JR1_OK)
            return status;
    }

    kaps_jr1_sassert (self->m_nMaxSize);

    KAPS_JR1_STRY(self->m_db->fn_table->get_data_len(self->m_db, entry_to_insert, &len_1, &data));

    kaps_jr1_sassert(entry_to_insert != HASH_TABLE_NULL_VALUE && entry_to_insert != HASH_TABLE_INVALID_VALUE);

    h1 = kaps_jr1_crc32(len_1, data, (len_1 + 7) / 8);

    bound = self->m_nMaxSize;
    h1 %= bound;
    h2 = (h1 + bound) >> 1;

    for (ix = h1;; ix += h2)
    {
        struct kaps_jr1_entry *slot;

        if (ix >= bound)
            ix -= bound;

        slot = self->m_slots_p[ix];
        if (slot == HASH_TABLE_NULL_VALUE || slot == HASH_TABLE_INVALID_VALUE)
        {
            if (slot == HASH_TABLE_INVALID_VALUE)
                self->m_nInvalidSlots--;

            self->m_slots_p[ix] = entry_to_insert;
            self->m_nCurSize++;
            is_inserted = 1;
            break;
        }
    }

    kaps_jr1_sassert(is_inserted == 1);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_locate_internal(
    struct pfx_hash_table * self,
    uint8_t * data,
    uint32_t len_1,
    int32_t * is_loop_cnt_exceeded,
    struct kaps_jr1_entry *** ret)
{
    uint32_t h1, h2, ix, bound;
    uint32_t nBytes = (len_1 + 7) / 8;
    int32_t i = 0;
    uint32_t loop_cnt;
    uint32_t e_len_1;
    uint8_t *e_data;

    kaps_jr1_sassert(ret != 0);

    *ret = 0;
    *is_loop_cnt_exceeded = 0;

    h1 = kaps_jr1_crc32(len_1, data, (len_1 + 7) / 8);
    bound = self->m_nMaxSize;
    h1 %= bound;
    h2 = (h1 + bound) >> 1;
    loop_cnt = 0;

    for (ix = h1;; ix += h2)
    {
        struct kaps_jr1_entry *slot;

        ++loop_cnt;

        
        if (loop_cnt > self->m_nMaxSize)
        {
            *is_loop_cnt_exceeded = 1;
            break;
        }

        if (ix >= bound)
            ix -= bound;
        slot = self->m_slots_p[ix];

        if (slot == HASH_TABLE_NULL_VALUE)
        {
            break;
        }
        else if (slot == HASH_TABLE_INVALID_VALUE)
        {
            continue;
        }
        else
        {

            KAPS_JR1_STRY(self->m_db->fn_table->get_data_len(self->m_db, slot, &e_len_1, &e_data));

            if (len_1 == e_len_1)
            {
                int32_t isPfxFound = 1;

                for (i = nBytes - 1; i >= 0; --i)
                {
                    if (data[i] != e_data[i])
                    {
                        isPfxFound = 0;
                        break;
                    }
                }

                if (isPfxFound)
                {
                    *ret = self->m_slots_p + ix;
                    return KAPS_JR1_OK;
                }
            }
        }
    }

    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_pfx_hash_table_locate(
    struct pfx_hash_table * self,
    uint8_t * data,
    uint32_t len_1,
    struct kaps_jr1_entry *** ret)
{
    int32_t is_loop_cnt_exceeded = 0;

    KAPS_JR1_TRY(kaps_jr1_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));

    
    if (is_loop_cnt_exceeded)
    {
        KAPS_JR1_STRY(kaps_jr1_pfx_hash_table_reconstruct(self));
        KAPS_JR1_TRY(kaps_jr1_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));
    }

    
    if (is_loop_cnt_exceeded)
    {
        KAPS_JR1_STRY(kaps_jr1_pfx_hash_table_expand(self));
        KAPS_JR1_TRY(kaps_jr1_pfx_hash_table_locate_internal(self, data, len_1, &is_loop_cnt_exceeded, ret));
    }

    kaps_jr1_sassert(is_loop_cnt_exceeded == 0);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_delete(
    struct pfx_hash_table * self,
    struct kaps_jr1_entry ** slot)
{
    kaps_jr1_sassert(slot >= self->m_slots_p);
    kaps_jr1_sassert(slot < (self->m_slots_p + self->m_nMaxSize));

    if (*slot == HASH_TABLE_NULL_VALUE || *slot == HASH_TABLE_INVALID_VALUE)
        return KAPS_JR1_INTERNAL_ERROR;

    *slot = HASH_TABLE_INVALID_VALUE;
    self->m_nCurSize--;
    self->m_nInvalidSlots++;

    
    if (self->m_nInvalidSlots > (self->m_nMaxSize / 2))
        kaps_jr1_pfx_hash_table_reconstruct(self);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_expand(
    struct pfx_hash_table * self)
{
    struct kaps_jr1_entry **newslots;
    struct kaps_jr1_entry **oldslots = self->m_slots_p, **slot;
    uint32_t ix, oldsize = self->m_nMaxSize;
    uint32_t newsize = (oldsize * (100 + self->m_nResizeBy)) / 100;
    uint32_t num_valid_slots = self->m_nCurSize;
    struct kaps_jr1_allocator *alloc = self->m_alloc_p;

    newsize = kaps_jr1_find_next_prime(newsize);

    newslots = alloc->xcalloc(alloc->cookie, newsize, sizeof(struct kaps_jr1_entry *));
    if (!newslots)
        return KAPS_JR1_OUT_OF_MEMORY;

    self->m_nMaxSize = newsize;
    self->m_nThreshSize = (newsize * self->m_nThreshold) / 100;
    self->m_slots_p = newslots;
    self->m_nCurSize = 0;
    self->m_nInvalidSlots = 0;

    slot = oldslots;
    for (ix = 0; ix < oldsize; ix++, slot++)
    {
        if (*slot != HASH_TABLE_NULL_VALUE && *slot != HASH_TABLE_INVALID_VALUE)
        {
            kaps_jr1_pfx_hash_table_insert(self, *slot);
        }
    }

    (void) num_valid_slots;
    kaps_jr1_sassert(num_valid_slots == self->m_nCurSize);

    alloc->xfree(alloc->cookie, (void *) oldslots);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_pfx_hash_table_reconstruct(
    struct pfx_hash_table * self)
{
    struct kaps_jr1_entry **newslots;
    struct kaps_jr1_entry **oldslots = self->m_slots_p, **slot;
    uint32_t ix, original_size = self->m_nMaxSize;
    uint32_t num_valid_slots = self->m_nCurSize;
    struct kaps_jr1_allocator *alloc = self->m_alloc_p;

    newslots = alloc->xcalloc(alloc->cookie, original_size, sizeof(struct kaps_jr1_entry *));
    if (!newslots)
        return KAPS_JR1_OUT_OF_MEMORY;

    
    self->m_slots_p = newslots;
    self->m_nCurSize = 0;
    self->m_nInvalidSlots = 0;

    slot = oldslots;
    for (ix = 0; ix < original_size; ix++, slot++)
    {
        if (*slot != HASH_TABLE_NULL_VALUE && *slot != HASH_TABLE_INVALID_VALUE)
        {
            kaps_jr1_pfx_hash_table_insert(self, *slot);
        }
    }

    (void) num_valid_slots;
    kaps_jr1_sassert(num_valid_slots == self->m_nCurSize);

    alloc->xfree(alloc->cookie, (void *) oldslots);

    return KAPS_JR1_OK;
}
