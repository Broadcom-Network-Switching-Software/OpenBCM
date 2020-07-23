/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Generic FIFO allocation and maintenance
 */

#ifndef _SHR_FIFO_H
#define _SHR_FIFO_H

typedef struct shr_fifo_s {
    uint32 fifo_size;     /* Maximum depth of FIFO */
    uint32 fifo_write;    /* Next entry to which to write */
    uint32 fifo_read;     /* Current entry from which to read */
    uint32 fifo_count;    /* Current FIFO depth */
    uint32 fifo_entry_words;  /* Number of uint32 for each FIFO element */
    uint32 fifo_flags;    /* Special handling flags to tune behavior */
    uint32 *fifo_mem;     /* Memory pointer to FIFO queue */
} shr_fifo_t;

/* If this flag is set, then any pushed entry cannot be the same as the
 * old entry data in that slot. (To handle HW which doesn't clear the 
 * old FIFO entry */
#define SHR_FIFO_FLAG_DO_NOT_PUSH_DUPLICATE      0x00000001

#define _SHR_FIFO_ADD_INDICES(_fifo_, _index1_, _index2_)               \
        (((_index1_) + (_index2_)) % ((_fifo_)->fifo_size))

#define _SHR_FIFO_NEXT_INDEX(_fifo_, _index_)                           \
        _SHR_FIFO_ADD_INDICES((_fifo_), _index_, 1)

#define _SHR_FIFO_WRITE_INC(_fifo_)                                     \
        (_fifo_)->fifo_write =                                          \
                _SHR_FIFO_NEXT_INDEX((_fifo_), (_fifo_)->fifo_write)

#define _SHR_FIFO_READ_INC(_fifo_)                                      \
        (_fifo_)->fifo_read =                                           \
             _SHR_FIFO_NEXT_INDEX((_fifo_), (_fifo_)->fifo_read)

#define _SHR_FIFO_ENTRY_INDEX_PTR(_fifo_, _index_)                      \
        (&((_fifo_)->fifo_mem[(_index_) * (_fifo_)->fifo_entry_words]))

#define _SHR_FIFO_ENTRY_BYTES(_fifo_)                                   \
        ((_fifo_)->fifo_entry_words * sizeof(uint32))

#define SHR_FIFO_ALLOC(_fifo_, _size_, _entry_bytes_, _flags_)          \
    do {                                                                \
        (_fifo_)->fifo_entry_words = ((_entry_bytes_) + 3) / 4;         \
        (_fifo_)->fifo_mem =                                            \
             sal_alloc((_fifo_)->fifo_entry_words * 4 * (_size_),       \
                       "FIFO data memory");                             \
        if ((_fifo_)->fifo_mem != NULL) {                               \
            sal_memset((_fifo_)->fifo_mem, 0,                           \
                       (_fifo_)->fifo_entry_words * 4 * (_size_));      \
            (_fifo_)->fifo_size = (_size_);                             \
            (_fifo_)->fifo_write = 0;                                   \
            (_fifo_)->fifo_read = 0;                                    \
            (_fifo_)->fifo_count = 0;                                   \
            (_fifo_)->fifo_flags = (_flags_);                           \
        } else {                                                        \
            (_fifo_)->fifo_size = 0;                                    \
        }                                                               \
    } while (0)

#define SHR_FIFO_FREE(_fifo_)                                           \
    if ((_fifo_)->fifo_mem != NULL) {                                   \
        sal_free((_fifo_)->fifo_mem);                                   \
        (_fifo_)->fifo_mem = NULL;                                      \
    }

#define SHR_FIFO_PUSH(_fifo_, _entry_ptr_)                              \
    if (((_fifo_)->fifo_count < (_fifo_)->fifo_size) &&                 \
        (!((_fifo_)->fifo_flags & SHR_FIFO_FLAG_DO_NOT_PUSH_DUPLICATE)  \
         || sal_memcmp(_SHR_FIFO_ENTRY_INDEX_PTR((_fifo_),              \
                                                 (_fifo_)->fifo_write), \
                       (_entry_ptr_),                                   \
                       _SHR_FIFO_ENTRY_BYTES(_fifo_)))) {               \
        sal_memcpy(_SHR_FIFO_ENTRY_INDEX_PTR((_fifo_),                  \
                   (_fifo_)->fifo_write), (_entry_ptr_),                \
                   _SHR_FIFO_ENTRY_BYTES(_fifo_));                      \
        _SHR_FIFO_WRITE_INC(_fifo_);                                    \
        (_fifo_)->fifo_count++;                                         \
    }

#define SHR_FIFO_POP(_fifo_, _entry_ptr_)                               \
    if ((_fifo_)->fifo_count) {                                         \
        sal_memcpy((_entry_ptr_),                                       \
                   _SHR_FIFO_ENTRY_INDEX_PTR((_fifo_),                  \
                                             (_fifo_)->fifo_read),      \
                   _SHR_FIFO_ENTRY_BYTES(_fifo_));                      \
        _SHR_FIFO_READ_INC(_fifo_);                                     \
        (_fifo_)->fifo_count--;                                         \
    } else {                                                            \
        sal_memset((_entry_ptr_), 0, _SHR_FIFO_ENTRY_BYTES(_fifo_));    \
    }

#define SHR_FIFO_IS_FULL(_fifo_)                                        \
        ((_fifo_)->fifo_count == (_fifo_)->fifo_size)
#define SHR_FIFO_IS_EMPTY(_fifo_)                                       \
        ((_fifo_)->fifo_count == 0)
#define SHR_FIFO_DEPTH(_fifo_)                                          \
        ((_fifo_)->fifo_count)
#define SHR_FIFO_SIZE(_fifo_)                                           \
        ((_fifo_)->fifo_size)

#endif	/* !_SHR_FIFO_H */
