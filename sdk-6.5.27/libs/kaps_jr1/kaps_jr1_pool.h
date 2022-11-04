

#ifndef __KAPS_JR1_POOL_H
#define __KAPS_JR1_POOL_H

#include "kaps_jr1_allocator.h"

#ifdef __cplusplus
extern "C"
{
#endif




#define POOL_CHUNK 512



#define POOL_DEF(TYPE)                                  \
    struct TYPE##__pool                                 \
    {                                                   \
        uint32_t last;                                  \
        uint32_t saved_last;                            \
        struct kaps_jr1_allocator *allocator;                \
        struct TYPE *free_list;                         \
        uint32_t free_count;                            \
        uint32_t shrink_count;                          \
        struct TYPE##__chunk                            \
        {                                               \
            struct TYPE pool[POOL_CHUNK];               \
            int fill_count;                             \
            int saved_fill_count;                       \
            int chunk_last;                             \
            int saved_chunk_last;                       \
            struct TYPE *free_list;    \
            struct TYPE##__chunk *next;                 \
        } *head;                                        \
        struct TYPE##__chunk **ptr;                     \
        struct TYPE##__chunk **saved_ptr;               \
        struct TYPE##__chunk *last_freed_chunk;         \
        struct TYPE##__chunk *last_for_range_ptr;       \
        uint32_t last_for_range_index;                  \
    }



#define POOL_INIT(TYPE, POOL, ALLOCATOR)        \
    do {                                        \
        (POOL)->last = 0;                       \
        (POOL)->saved_last = 0;                 \
        (POOL)->allocator = ALLOCATOR;          \
        (POOL)->free_list = (struct TYPE *)0;   \
        (POOL)->free_count = 0;                 \
        (POOL)->shrink_count = 0;               \
        (POOL)->head = 0;                       \
        (POOL)->ptr = &(POOL)->head;            \
        (POOL)->saved_ptr = NULL;               \
        (POOL)->last_for_range_ptr = 0;         \
        (POOL)->last_freed_chunk = 0;           \
        (POOL)->last_for_range_index = -1;      \
    } while (0)

#ifdef USE_MALLOC



#define POOL_ALLOC(TYPE, POOL, NODE)                                    \
    ((NODE) = ((POOL)->allocator->xmalloc((POOL)->allocator->cookie, sizeof(struct TYPE))))

#define POOL_FREE(TYPE, POOL, NODE)                                     \
    ((POOL)->allocator->xfree((POOL)->allocator->cookie, (NODE)))

#else



#define POOL_ALLOC(TYPE, POOL, NODE)                                    \
    do {                                                                \
        if ((POOL)->free_list)                                          \
        {                                                               \
            NODE = (POOL)->free_list;                                   \
            (POOL)->free_list = *(struct TYPE **)((POOL)->free_list);   \
            break;                                                      \
        }                                                               \
        if ((POOL)->last % POOL_CHUNK == 0 && (POOL)->last != 0)        \
            (POOL)->ptr = &((*((POOL)->ptr))->next);                    \
        if (*((POOL)->ptr) == 0)                                        \
        {                                                               \
            *((POOL)->ptr)                                              \
                = (struct TYPE##__chunk *) (POOL)->allocator->xmalloc ((POOL)->allocator->cookie,      \
                                                                       sizeof (struct TYPE##__chunk)); \
            if (*((POOL)->ptr) == NULL) {                               \
                NODE = NULL;                                            \
                break;                                                  \
            }                                                           \
            (*((POOL)->ptr))->fill_count = 0;                           \
            (*((POOL)->ptr))->chunk_last = 0;                           \
            (*((POOL)->ptr))->next = 0;                                 \
            (*((POOL)->ptr))->free_list = NULL;                         \
        }                                                               \
        NODE = &(*((POOL)->ptr))->pool[(POOL)->last % POOL_CHUNK];      \
        (*((POOL)->ptr))->fill_count++;                                 \
        (*((POOL)->ptr))->chunk_last++;                                 \
        (POOL)->last ++;                                                \
    } while (0)



#define POOL_FREE(TYPE, POOL, NODE)                     \
    do {                                                \
        *(struct TYPE **)(NODE) = (POOL)->free_list;    \
        (POOL)->free_list = NODE;                       \
    } while (0)

#endif



#define POOL_FREE_FOR_SHRINK(TYPE, POOL, NODE)                          \
    do {                                                                \
        struct TYPE##__chunk *chunk = (POOL)->last_freed_chunk;         \
        void *vp;                                                       \
        struct TYPE **np;                                               \
        if (!(chunk &&  &(NODE) >= (&chunk->pool[0])                    \
              && &(NODE) <= (&chunk->pool[POOL_CHUNK - 1])))            \
        {                                                               \
            chunk = (POOL)->head;                                       \
            while (chunk)                                               \
            {                                                           \
                if (&(NODE) >= (&chunk->pool[0])                        \
                    && &(NODE) <= (&chunk->pool[POOL_CHUNK - 1]))       \
                {                                                       \
                    (POOL)->last_freed_chunk = chunk;                   \
                    break;                                              \
                }                                                       \
                chunk = chunk->next;                                    \
            }                                                           \
        }                                                               \
        (POOL)->shrink_count++;                                         \
        chunk->fill_count--;                                            \
        (POOL)->free_count++;                                           \
        if (chunk->free_list == NULL || &(NODE) > chunk->free_list)     \
        {                                                               \
            vp = (void*) &(NODE);                                       \
            np = (struct TYPE **) vp;                                   \
            *np =  chunk->free_list;                                    \
            chunk->free_list = &(NODE);                                 \
        }                                                               \
        else                                                            \
        {                                                               \
            struct TYPE *FPTR = chunk->free_list;                       \
            while (*(struct TYPE **)(FPTR) && *(struct TYPE **) (FPTR) > &(NODE)) \
            {                                                           \
                FPTR = *(struct TYPE **)(FPTR);                         \
            }                                                           \
            vp = (void*) &(NODE);                                       \
            np = (struct TYPE **) vp;                                   \
            *np = *(struct TYPE **) (FPTR);                             \
            *(struct TYPE **) (FPTR) = &(NODE);                         \
        }                                                               \
        break;                                                          \
    } while (0)



#define POOL_SHRINK(TYPE, POOL, FLAG)                                   \
    do {                                                                \
        struct TYPE##__chunk *chunk = (POOL)->head;                     \
        struct TYPE##__chunk *chunk_prev = NULL;                        \
        if ((POOL)->shrink_count < POOL_CHUNK || (POOL)->free_count < (POOL_CHUNK * 8)) break; \
        while (chunk)                                                   \
        {                                                               \
            if (chunk->free_list)                                       \
            {                                                           \
                struct TYPE *FPTR = chunk->free_list;                   \
                struct TYPE *FPTR_PREV = NULL;                          \
                struct TYPE *CPTR = &chunk->pool[chunk->chunk_last - 1]; \
                while (CPTR >= &chunk->pool[0])                         \
                {                                                       \
                    if (FPTR && CPTR < FPTR)                            \
                    {                                                   \
                        FPTR_PREV = FPTR;                               \
                        FPTR = *(struct TYPE **)(FPTR);                 \
                    }                                                   \
                    if (CPTR == FPTR)                                   \
                    {                                                   \
                        CPTR--;                                         \
                        continue;                                       \
                    }                                                   \
                    if (FLAG)                                           \
                    {                                                   \
                        chunk->fill_count--;                            \
                        (POOL)->free_count++;                           \
                        *(struct TYPE **)(CPTR) = FPTR;                 \
                        if (FPTR_PREV)                                  \
                        {                                               \
                            *(struct TYPE **)(FPTR_PREV) = CPTR;        \
                        }                                               \
                        else                                            \
                        {                                               \
                            chunk->free_list = CPTR;                    \
                        }                                               \
                        FPTR_PREV = FPTR;                               \
                        FPTR = CPTR;                                    \
                    }                                                   \
                    else  \
                        break;                                          \
                    CPTR--;                                             \
                }                                                       \
            }                                                           \
            if (chunk->fill_count == 0)                                 \
            {                                                           \
                if (chunk_prev == NULL && chunk->next == NULL)          \
                    break;                \
                else if (chunk_prev == NULL)                            \
                {                                                       \
                    (POOL)->head = chunk->next;                         \
                    if ((POOL)->last_freed_chunk == chunk)              \
                        (POOL)->last_freed_chunk = 0;                   \
                    (POOL)->allocator->xfree ((POOL)->allocator->cookie, chunk); \
                    chunk = (POOL)->head;                               \
                }                                                       \
                else                                                    \
                {                                                       \
                    chunk_prev->next = chunk->next;                     \
                    if ((POOL)->last_freed_chunk == chunk)              \
                        (POOL)->last_freed_chunk = 0;                   \
                    (POOL)->allocator->xfree ((POOL)->allocator->cookie, chunk); \
                    chunk = chunk_prev->next;                           \
                }                                                       \
                (POOL)->free_count -= POOL_CHUNK;                       \
            }                                                           \
            else                                                        \
            {                                                           \
                chunk_prev = chunk;                                     \
                chunk = chunk->next;                                    \
            }                                                           \
        }                                                               \
        (POOL)->shrink_count = 0;                                       \
    } while (0)



#define POOL_FINI(TYPE, POOL)                                           \
    do {                                                                \
        struct TYPE##__chunk *chunk = (POOL)->head;                     \
        while (chunk)                                                   \
        {                                                               \
            struct TYPE##__chunk *next_chunk = chunk->next;             \
            (POOL)->allocator->xfree ((POOL)->allocator->cookie, chunk); \
            chunk = next_chunk;                                         \
        }                                                               \
        (POOL)->head = 0;                                               \
        (POOL)->ptr = 0;                                                \
        (POOL)->saved_ptr = 0;                                          \
        (POOL)->last = 0;                                               \
        (POOL)->saved_last = 0;                                         \
        (POOL)->shrink_count = 0;                                       \
        (POOL)->allocator = 0;                                          \
        (POOL)->free_list = (struct TYPE *)0;                           \
        (POOL)->last_freed_chunk = 0;                                   \
        (POOL)->last_for_range_ptr = 0;                                 \
        (POOL)->last_for_range_index = -1;                              \
    } while (0)



#define POOL_RESET(TYPE, POOL)                                          \
    do {                                                                \
        struct TYPE##__chunk *chunk = (POOL)->head;                     \
        if (chunk == NULL)         \
            break;                                                      \
        chunk = chunk->next;                                            \
        while (chunk)                                                   \
        {                                                               \
            struct TYPE##__chunk *next_chunk = chunk->next;             \
            (POOL)->allocator->xfree ((POOL)->allocator->cookie, chunk); \
            chunk = next_chunk;                                         \
        }                                                               \
        (POOL)->last = 0;                                               \
        (POOL)->saved_last = 0;                                         \
        (POOL)->free_list = (struct TYPE *)0;                           \
        (POOL)->ptr = &(POOL)->head;                                    \
        (*((POOL)->ptr))->fill_count = 0;                               \
        (*((POOL)->ptr))->chunk_last = 0;                               \
        (*((POOL)->ptr))->saved_fill_count = 0;                         \
        (*((POOL)->ptr))->saved_chunk_last = 0;                         \
        (*((POOL)->ptr))->next = 0;                                     \
        (*((POOL)->ptr))->free_list = NULL;                             \
        (POOL)->saved_ptr = NULL;                                       \
        (POOL)->shrink_count = 0;                                       \
        (POOL)->head->next = NULL;                                      \
        (POOL)->last_freed_chunk = 0;                                   \
        (POOL)->last_for_range_ptr = (POOL)->head;                      \
        (POOL)->last_for_range_index = 0;                               \
    } while (0)



#define POOL_FOREACH_ITERATOR(TYPE)             \
    struct TYPE##__chunk *TYPE##__ptr = 0;



#define POOL_FOREACH(TYPE, POOL, NODE, INDEX)                           \
    for (kaps_jr1_sassert ((POOL)->free_list == NULL), TYPE##__ptr = (POOL)->head, INDEX = 0, \
             NODE = &TYPE##__ptr->pool[INDEX % POOL_CHUNK];             \
         (NODE = &TYPE##__ptr->pool[INDEX % POOL_CHUNK], INDEX < (POOL)->last); \
         INDEX++, TYPE##__ptr = INDEX % POOL_CHUNK == 0                 \
             ? TYPE##__ptr->next : TYPE##__ptr)



#define POOL_BYTES(TYPE, POOL, NBYTES)                          \
    do {                                                        \
        struct TYPE##__chunk *chunk = (POOL)->head;             \
        while (chunk)                                           \
        {                                                       \
            struct TYPE##__chunk *next_chunk = chunk->next;     \
            (NBYTES) += POOL_CHUNK * sizeof(struct TYPE);       \
            chunk = next_chunk;                                 \
        }                                                       \
    } while (0)



#define POOL_STATS(TYPE, POOL, FP)                                          \
    do {                                                                    \
        struct TYPE##__chunk *chunk = (POOL)->head;                         \
        struct TYPE *node;                                                  \
        uint32_t pool_size = 0;                                             \
        uint32_t free_slots = 0;                                            \
        uint32_t free_bytes = 0;                                            \
        uint32_t num_chunks = 0;                                            \
        while (chunk) {                                                     \
            struct TYPE##__chunk *next_chunk = chunk->next;                 \
            pool_size += POOL_CHUNK * sizeof(struct TYPE);                  \
            num_chunks++;                                                   \
            chunk = next_chunk;                                             \
        }                                                                   \
        node = (POOL)->free_list;                                           \
        while (node) {                                                      \
            free_slots++;                                                   \
            node = *(struct TYPE **)(node);                                 \
        }                                                                   \
        if ((POOL)->last % POOL_CHUNK)                                      \
            free_slots += POOL_CHUNK - ((POOL)->last % POOL_CHUNK);         \
        free_bytes = free_slots * sizeof(struct TYPE);                      \
        if (FP && pool_size) {                                              \
            kaps_jr1_fprintf(FP, "%s, %d, %d, %d, %d, %d\n", #TYPE, num_chunks,  \
                        sizeof(struct TYPE), free_slots, pool_size, pool_size - free_bytes); \
        }                                                                   \
    } while (0)



#ifdef __cplusplus
}
#endif

#endif                          
