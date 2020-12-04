/*! \file bcmecmp_util_bucket.c
 *
 * Bucket sort utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <bcmecmp/bcmecmp_util_bucket.h>

typedef int (*bcmecmp_hash_f)(uint32_t key, uint32_t max_key, int table_size);
typedef int (*bcmecmp_compare_f)(uint32_t key1, uint32_t key2);

typedef struct bcmecmp_bucket_node_s
{
    uint32_t key;
    uint32_t data;
    struct bcmecmp_bucket_node_s *next;
} bcmecmp_bucket_node_t;

typedef struct bcmecmp_bucket_s
{
    int size;
    int cur_bucket;
    uint32_t max_key;
    bcmecmp_bucket_node_t **nodes;
    bcmecmp_hash_f hash_func;
    bcmecmp_compare_f compare_func;
} bcmecmp_bucket_t;

/* Default hash function. */
static int
default_hash_func(uint32_t key, uint32_t max_key, int table_size)
{
    /* The key sort from MIN to MAX in the bucket. */
    return  ((int) ((uint64_t)key * (table_size - 1) / max_key));
}

/* Default compare function. */
static int
default_compare_func(uint32_t key1, uint32_t key2)
{
    if (key1 == key2) {
        return 0;
    }

    return ((key1 > key2) ? 1 : -1);
}


bcmecmp_bucket_handle_t
bcmecmp_bucket_create(int bucket_size, uint32_t max_key)
{
    bcmecmp_bucket_t *bucket;

    if (bucket_size <= 0) {
        return BCMECMP_BUCKET_INVALID_HANDLE;
    }

    bucket = (bcmecmp_bucket_t *)sal_alloc(sizeof(bcmecmp_bucket_t),
                                           "bcmecmpBucketSort");
    if (bucket == NULL) {
        return BCMECMP_BUCKET_INVALID_HANDLE;
    }

    bucket->nodes = (bcmecmp_bucket_node_t **)
                        sal_alloc(sizeof(bcmecmp_bucket_node_t *) * bucket_size,
                                  "bcmecmpBucketSort");
    if (bucket->nodes == NULL) {
        sal_free(bucket);
        return BCMECMP_BUCKET_INVALID_HANDLE;
    }
    sal_memset(bucket->nodes, 0, sizeof(bcmecmp_bucket_node_t *) * bucket_size);

    bucket->size = bucket_size;
    bucket->max_key = max_key;
    bucket->cur_bucket = -1;
    bucket->hash_func = default_hash_func;
    bucket->compare_func = default_compare_func;

    return (bcmecmp_bucket_handle_t)bucket;
}

int
bcmecmp_bucket_destroy(bcmecmp_bucket_handle_t bucket_hdl)
{
    bcmecmp_bucket_t *bucket = (bcmecmp_bucket_t *)bucket_hdl;
    bcmecmp_bucket_node_t *cur_node;
    bcmecmp_bucket_node_t *temp_node;
    int idx;

    if (bucket == BCMECMP_BUCKET_INVALID_HANDLE) {
        return SHR_E_PARAM;
    }

    for (idx = 0; idx < bucket->size; idx++) {
        cur_node = bucket->nodes[idx];
        while (cur_node != NULL) {
            temp_node = cur_node;
            cur_node = cur_node->next;

            sal_free(temp_node);
        }
    }

    sal_free(bucket->nodes);
    sal_free(bucket);

    return SHR_E_NONE;
}

int
bcmecmp_bucket_insert(bcmecmp_bucket_handle_t bucket_hdl, uint32_t key,
                      uint32_t data)
{
    bcmecmp_bucket_t *bucket = (bcmecmp_bucket_t *)bucket_hdl;
    bcmecmp_bucket_node_t *new_node;
    int idx;

    if (bucket == BCMECMP_BUCKET_INVALID_HANDLE) {
        return SHR_E_PARAM;
    }

    new_node = (bcmecmp_bucket_node_t *)sal_alloc(sizeof(bcmecmp_bucket_node_t),
                                                  "bcmecmpBucketSort");
    if (new_node == NULL) {
        return SHR_E_MEMORY;
    }
    new_node->key = key;
    new_node->data = data;

    idx = bucket->hash_func(key, bucket->max_key, bucket->size);
    if (idx > bucket->cur_bucket) {
        bucket->cur_bucket = idx;
    }

    /* Sepcial handle at head position. */
    if (bucket->nodes[idx] == NULL ||
        bucket->compare_func(new_node->key, bucket->nodes[idx]->key) >= 0) {
        new_node->next = bucket->nodes[idx];
        bucket->nodes[idx] = new_node;
    } else {
        bcmecmp_bucket_node_t *cur_node = bucket->nodes[idx];

        /* sort from max to min in each bucket. */
        while (cur_node->next != NULL &&
               bucket->compare_func(new_node->key, cur_node->next->key) < 0) {
            cur_node = cur_node->next;
        }

        new_node->next = cur_node->next;
        cur_node->next = new_node;
    }

    return SHR_E_NONE;
}

int
bcmecmp_bucket_remove_max(bcmecmp_bucket_handle_t bucket_hdl, uint32_t *key,
                          uint32_t *data)
{
    bcmecmp_bucket_t *bucket = (bcmecmp_bucket_t *)bucket_hdl;
    bcmecmp_bucket_node_t *my_node;
    int idx;

    if (bucket == BCMECMP_BUCKET_INVALID_HANDLE) {
        return SHR_E_PARAM;
    }

    if (bucket->cur_bucket < 0) {
        return SHR_E_EMPTY;
    }

    /* Remove the node from bucket. */
    idx = bucket->cur_bucket;
    my_node = bucket->nodes[idx];
    bucket->nodes[idx] = bucket->nodes[idx]->next;
    if (bucket->nodes[idx] == NULL) {
        for (idx = (bucket->cur_bucket - 1); idx >= 0; idx--) {
            if (bucket->nodes[idx] != NULL) {
                break;
            }
        }
        bucket->cur_bucket = idx;
    }

    if (key != NULL) {
        *key = my_node->key;
    }

    if (data != NULL) {
        *data = my_node->data;
    }

    sal_free(my_node);

    return SHR_E_NONE;
}

void
bcmemcp_bucket_dump(bcmecmp_bucket_handle_t bucket_hdl,
                    bcmecmp_bucket_dump_f dump_func)
{
    bcmecmp_bucket_t *bucket = (bcmecmp_bucket_t *)bucket_hdl;
    bcmecmp_bucket_node_t *my_node;
    int idx;

    if (bucket_hdl == BCMECMP_BUCKET_INVALID_HANDLE || dump_func == NULL) {
        return;
    }

    for (idx = 0; idx < bucket->size; idx++) {
        my_node = bucket->nodes[idx];
        while (my_node != NULL) {
            dump_func(idx, my_node->key, my_node->data);
            my_node = my_node->next;
        }
    }
}
