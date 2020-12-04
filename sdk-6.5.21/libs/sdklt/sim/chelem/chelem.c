/*! \file chelem.c
 *
 * Simulation chip elements.
 *
 * This implementation is a simple chained hash table where all entry
 * records are stored in the bucket array itself.
 *
 * When a new entry is to be inserted, the elements are examined
 * starting with the index stored in the hashed-to slot (head) and
 * proceeding according to the linked list of bucket indices (list).
 *
 * When searching for an entry, the elements are scanned in the same
 * sequence, until the target record is found. If not found, then a
 * new element is added to the linked list of the hash slot.
 *
 * Note that hash entries never need to be deleted.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>

#include <sim/chelem/chelem.h>

#ifndef CHELEM_MAX
#define CHELEM_MAX              (1024 * 1024)
#endif

#ifndef CHELEM_ENTRY_NAX
#define CHELEM_ENTRY_NAX        (4 * 32)
#endif

typedef struct chelem_key_s {
    int unit;
    uint32_t addrx;
    uint32_t addr;
} chelem_key_t;

typedef struct chelem_s {
    chelem_key_t key;
    uint8_t data[CHELEM_ENTRY_NAX];
} chelem_t;

/*! Array of chip elements (hash buckets). */
static chelem_t chelem[CHELEM_MAX];

/*! Head of linked list (bucket list) indexed by hash value. */
static uint32_t chelem_head[CHELEM_MAX];

/*! Index of next entry in linked list (bucket list). */
static uint32_t chelem_list[CHELEM_MAX];

/*! Index of next free bucket. */
static uint32_t chelem_free_idx = 1; /* Start with 1, as 0 is null */

/*! End-of-linked-list marker. */
#define INDEX_NULL 0


/*!
 * \brief Calculate hash value for a chip element key.
 *
 * \param [in] key Chip element key.
 *
 * \return Hash value suitable for indexing \ref chelem_head array.
 */
static uint32_t
hash_val_get(chelem_key_t *key)
{
    return key->addr % CHELEM_MAX;
}

/*!
 * \brief Check if two chip element keys are identical.
 *
 * \param [in] key1 First chip element key.
 * \param [in] key2 Second chip element key.
 *
 * \retval 0 Keys are identical.
 * \retval 1 Keys are not identical.
 */
static inline int
compare_chelem_key(chelem_key_t *key1, chelem_key_t *key2)
{
    if (key1->addr == key2->addr &&
        key1->addrx == key2->addrx &&
        key1->unit == key2->unit) {
        return 0;
    }
    return 1;
}

/*!
 * \brief Find chip element in hash table.
 *
 * \param [in] key Chip element key.
 * \param [in] hash_val Hash value of chip element key.
 *
 * \return Pointer to matched chip element or NULL if not found.
 */
static chelem_t *
find_chelem(chelem_key_t *key, uint32_t hash_val)
{
    uint32_t idx;

    /* Get head of bucket list */
    idx = chelem_head[hash_val];

    /* Search linked list of entries with identical hash values. */
    while (idx != INDEX_NULL) {
        if (compare_chelem_key(&chelem[idx].key, key) == 0) {
            return &chelem[idx];
        }
        /* Next index in linked list */
        idx = chelem_list[idx];
    }
    return NULL;
}

/*!
 * \brief Insert new chip element into hash table.
 *
 * \param [in] key Chip element key.
 * \param [in] hash_val Hash value of chip element key.
 * \param [in] new_index Bucket index for new element.
 *
 * \return Created chip element.
 */
static inline chelem_t *
create_chelem(chelem_key_t *key, uint32_t hash_val, uint32_t new_index)
{
    /* Insert new element as head of bucket list */
    chelem_list[new_index] = chelem_head[hash_val];
    chelem_head[hash_val] = new_index;

    return &chelem[new_index];
}

/*!
 * \brief Find a given chip element or create new one.
 *
 * \param [in] key Chip element key.
 * \param [in] hash_val Hash value of chip element key.
 *
 * \return Matched chip element or new one.
 */
static chelem_t *
find_or_create_chelem(chelem_key_t *key, uint32_t hash_val)
{
    chelem_t *ch;

    if ((ch = find_chelem(key, hash_val)) == NULL) {
        if (chelem_free_idx < CHELEM_MAX) {
            ch = create_chelem(key, hash_val, chelem_free_idx++);
            assert(ch);
        } else {
            assert(0);
            return NULL;
        }
    }
    ch->key = *key;
    return ch;
}

void
chelem_clear_all(void)
{
    sal_memset(chelem, 0, sizeof(chelem));
    sal_memset(chelem_head, 0, sizeof(chelem_head));
    sal_memset(chelem_list, 0, sizeof(chelem_list));
    chelem_free_idx = 1;
}

int
chelem_read(int unit, uint32_t addrx, uint32_t addr,
            void *data, size_t size)
{
    uint32_t hash_val;
    chelem_t *ch;
    chelem_key_t key =
        {.addrx = addrx,
         .addr = addr,
         .unit = unit};

    hash_val = hash_val_get(&key);
    ch = find_chelem(&key, hash_val);
    if (ch) {
        assert(size < sizeof(ch->data));
        sal_memcpy(data, ch->data, size);
    } else {
        sal_memset(data, 0, size);
    }
    return 0;
}

int
chelem_write(int unit, uint32_t addrx, uint32_t addr,
             void *data, size_t size)
{
    uint32_t hash_val;
    chelem_t *ch;
    chelem_key_t key =
        {.addrx = addrx,
         .addr = addr,
         .unit = unit};

    hash_val = hash_val_get(&key);
    ch = find_or_create_chelem(&key, hash_val);
    if (!ch) {
        return -1;
    }
    assert(ch);
    assert(size <= sizeof(ch->data));
    sal_memcpy(ch->data, data, size);
    return 0;
}
