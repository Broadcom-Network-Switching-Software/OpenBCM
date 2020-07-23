/*! \file hash_str.c
 *
 * <description> This file is the implementation of a hash table that handle
 * string keys.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_mutex.h>
#include <shr/shr_error.h>
#include <shr/shr_lmem_mgr.h>
#include <shr/shr_hash_str.h>

#define ALLOCATE_ELEMENT_CHUNK 128
#define HASH_MODULE_SIGNATURE   0xe73ca69b
#define MIN_TABLE_SIZE      5

typedef struct _element_s {
    struct _element_s *next;
    const char  *key;
    const void  *val;
} _element_t;

typedef struct shr_hash_str_s {
    uint32_t signature;
    size_t size;
    size_t element_count;
    size_t free_slots;
    _element_t **table;
    uint32_t last_search_slot;
    _element_t *last_search_element;
} shr_hash_str_t;

static shr_lmm_hdl_t local_element_hdl;

static bool local_hash_str_init = false;

size_t shr_hash_str_calc_tbl_size(size_t num_of_elements);

int shr_hash_str_init()
{
    int rv;
    if (local_hash_str_init) {
        return 0;
    }
    LMEM_MGR_INIT(_element_t,
                  local_element_hdl,
                  ALLOCATE_ELEMENT_CHUNK,
                  true,
                  rv);
    if (0 != rv) {
        return SHR_E_FAIL;
    }
    local_hash_str_init = true;
    return SHR_E_NONE;
}

void shr_hash_str_delete()
{
    if (!local_hash_str_init) {
        return;
    }
    shr_lmm_delete(local_element_hdl);
    local_hash_str_init = false;
}

int shr_hash_str_dict_alloc(size_t num_of_elements,
                                   shr_hash_str_hdl *dict_hdl)
{
    shr_hash_str_t *dict;
    size_t alloc_size;

    if (num_of_elements > BCM_SHASH_STR_MAX_ELEMENTS) {
        return SHR_E_PARAM;
    }
    if (!dict_hdl || !local_hash_str_init) {
        return SHR_E_PARAM;
    }
    num_of_elements = shr_hash_str_calc_tbl_size(num_of_elements);

    dict = (shr_hash_str_t *)sal_alloc(sizeof(shr_hash_str_t), "shrHashDict");
    if (!dict) {
        return SHR_E_MEMORY;
    }
    sal_memset(dict, 0, sizeof(shr_hash_str_t));
    alloc_size = sizeof(_element_t *) * num_of_elements;
    dict->table = (_element_t **)sal_alloc(alloc_size, "shrHashTbl");
    if (!dict->table) {
        sal_free ((void *)dict);
        return SHR_E_MEMORY;
    }
    /* reset the table to 0 */
    sal_memset(dict->table, 0, alloc_size);
    dict->size = num_of_elements;
    dict->free_slots = num_of_elements;
    dict->signature = HASH_MODULE_SIGNATURE;
    *dict_hdl = (shr_hash_str_hdl)dict;
    return SHR_E_NONE;
}


int shr_hash_str_dict_free(shr_hash_str_hdl dict_hdl)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    size_t j;
    _element_t *p_element;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    /* free all the dictionary elements */
    for (j = 0; (j < dict->size) && (dict->element_count > 0); j++) {
        if (dict->table[j]) {
            do {
                p_element = dict->table[j];
                dict->table[j] = p_element->next;
                shr_lmm_free(local_element_hdl, (void *)p_element);
                dict->element_count--;
            } while (dict->table[j]);
        }
    }

    sal_free(dict->table);
    sal_free ((void *)dict);
    return SHR_E_NONE;
}

static size_t hash_idx (const char *key, size_t modulo)
{
#define MULTIPLIER (101)
    const unsigned char *p_key = (unsigned char *)key;
    uint64_t hash = 0;

    while (*p_key != 0) {
        hash = hash * MULTIPLIER + (*p_key);
        p_key++;
    }
    return hash % modulo;
}

int shr_hash_str_dict_insert(shr_hash_str_hdl dict_hdl,
                             const char *key,
                             const void *value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    /*
    * Verify that the element is not already there
    */
    p_element = dict->table[index];
    while (p_element) {
        if (sal_strcmp(p_element->key, key) == 0) {
            return SHR_E_EXISTS;
        }
        p_element = p_element->next;
    }

    p_element = (_element_t *)shr_lmm_alloc(local_element_hdl);
    if (!p_element) {
        return SHR_E_MEMORY;
    }
    p_element->key = key;
    p_element->val = value;
    p_element->next = dict->table[index];
    if (!dict->table[index]) {
        dict->free_slots--;
    }
    dict->table[index] = p_element;
    dict->element_count++;
    return SHR_E_NONE;
}

int shr_hash_str_dict_lookup(shr_hash_str_hdl dict_hdl,
                 const char *key,
                 void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE) || !key) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    p_element = dict->table[index];
    while (p_element) {
        if (sal_strcmp(p_element->key, key) == 0) {
            break;
        }
        p_element = p_element->next;
    }
    if (p_element) {
        *value = (void *)p_element->val;
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

int shr_hash_str_dict_delete(shr_hash_str_hdl dict_hdl,
                 const char *key)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    _element_t *p_element;
    _element_t *p_tmp_element = NULL;
    size_t index;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    index = hash_idx(key, dict->size);
    p_element = dict->table[index];
    if (!p_element) {
        return SHR_E_NOT_FOUND;
    }
    if (sal_strcmp(p_element->key, key) == 0) {
        dict->table[index] = p_element->next;
        p_tmp_element = p_element;
        if (p_element == dict->last_search_element) {
            dict->last_search_element = NULL;
        }
    }
    else {
        while (p_element->next) {
            if (sal_strcmp(p_element->next->key, key) == 0) {
                p_tmp_element = p_element->next;
                p_element->next = p_tmp_element->next;
                if (p_tmp_element == dict->last_search_element) {
                   dict->last_search_element = p_element;
                }
            } else {
                p_element = p_element->next;
            }
        }
    }
    if (!p_tmp_element) {
        return SHR_E_NOT_FOUND;
    }
    shr_lmm_free(local_element_hdl, (void *)p_tmp_element);

    /* update statistics */
    if (!dict->table[index]) {
        dict->free_slots++;
    }
    dict->element_count--;
    return SHR_E_NONE;
}

int shr_hash_str_get_first(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    dict->last_search_element = NULL;
    dict->last_search_slot = 0;
    while (dict->last_search_slot < dict->size) {
        if (dict->table[dict->last_search_slot]) {
            dict->last_search_element = dict->table[dict->last_search_slot];
            break;
        }
        dict->last_search_slot++;
    }
    if (dict->last_search_element) {
        if (key) {
            *key = (char *)dict->last_search_element->key;
        }
        if (value) {
            *value = (void *)dict->last_search_element->val;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}

int shr_hash_str_get_next(shr_hash_str_hdl dict_hdl,
                                  char **key,
                                  void **value)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    bool need_increment = false;

    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    if (dict->last_search_element) {
        dict->last_search_element = dict->last_search_element->next;
        need_increment = true;
    }
    if (!dict->last_search_element) {
        if (need_increment) {
            dict->last_search_slot++;
        }
        while (dict->last_search_slot < dict->size) {
            if (dict->table[dict->last_search_slot]) {
                dict->last_search_element = dict->table[dict->last_search_slot];
                break;
            }
            dict->last_search_slot++;
        }
    }

    if (dict->last_search_element) {
        if (key) {
            *key = (char *)dict->last_search_element->key;
        }
        if (value) {
            *value = (void *)dict->last_search_element->val;
        }
        return SHR_E_NONE;
    } else {
        return SHR_E_NOT_FOUND;
    }
}


int shr_hash_str_dict_info_get(shr_hash_str_hdl dict_hdl,
                 size_t *size,
                 size_t *element_count,
                 size_t *empty_slots)
{
    shr_hash_str_t *dict = (shr_hash_str_t *)dict_hdl;
    if (!dict || (dict->signature != HASH_MODULE_SIGNATURE)) {
        return SHR_E_PARAM;
    }
    if (size) {
        *size = dict->size;
    }
    if (element_count) {
        *element_count = dict->element_count;
    }
    if (empty_slots) {
        *empty_slots = dict->free_slots;
    }
    return SHR_E_NONE;
}


/*
 * Utility function to do modular exponentiation.
 * It returns (x^y) % p
 */
static uint32_t power(uint32_t x, uint32_t y, uint32_t p)
{
    uint32_t res = 1;      /* Initialize result */
    x = x % p;  /* Update x if it is more than or equal to p */
    while (y > 0)
    {
        /* If y is odd, multiply x with result */
        if (y & 1) {
            res = (res*x) % p;
        }

        /* y must be even now */
        y = y>>1; /* y = y/2 */
        x = (x*x) % p;
    }
    return res;
}

/*
 * This function is called for all k trials. It returns
 * false if n is composite and returns true if n is
 * probably prime.
 * d is an odd number such that  d*2^r = n-1
 * for some r >= 1
 */
static bool miiller_test(uint32_t d, uint32_t n)
{
    /*
     * Pick a random number in [2..n-2]
     * Corner cases make sure that n > 4
     */
    uint32_t a = 2 + sal_rand() % (n - 4);

    /* Compute a^d % n */
    uint32_t x = power(a, d, n);

    if ((x == 1)  || (x == n-1)) {
       return true;
    }

    /*
     * Keep squaring x while one of the following doesn't happen
     * (i)   d does not reach n-1
     * (ii)  (x^2) % n is not 1
     * (iii) (x^2) % n is not n-1
     */
    while (d != n-1)
    {
        x = (x * x) % n;
        d *= 2;

        if (x == 1) {
            return false;
        }
        if (x == n-1) {
            return true;
        }
    }

    /* Return composite */
    return false;
}

/*
 * It returns false if n is composite and returns true if n
 * is probably prime.  k is an input parameter that determines
 * accuracy level. Higher value of k indicates more accuracy.
 */
static bool is_prime(uint32_t n, uint32_t k)
{
    uint32_t i;
    /* Find r such that n = 2^d * r + 1 for some r >= 1 */
    uint32_t d = n - 1;
    while (!(d & 0x1)) {
        d = d >> 1;
    }

    /* Iterate given number of 'k' times */
    for (i = 0; i < k; i++) {
         if (miiller_test(d, n) == false) {
              return false;
         }
    }

    return true;
}

#define NUM_OF_ITERATIONS 4

size_t shr_hash_str_calc_tbl_size(size_t num_of_elements)
{
    size_t rv;
    if (num_of_elements <= MIN_TABLE_SIZE) {
        return MIN_TABLE_SIZE;
    }

    for (rv = num_of_elements; rv < 2 * num_of_elements; rv++) {
        if (is_prime((uint32_t)rv, NUM_OF_ITERATIONS)) {
            return rv;
        }
    }
    return num_of_elements;
}
