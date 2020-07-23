/*! \file shr_ha.c
 *
 * HA memory manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_mutex.h>
#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bsl/bsl.h>
#include "ha_internal.h"

#define BSL_LOG_MODULE BSL_LS_SHR_HA

/*******************************************************************************
 * Local definitions
 */

/* Minimum size of initial HA memory block */
#define HA_MEM_SIZE_MIN         (16 * ONE_KB)

/*!
 * \brief HA block signature.
 *
 * Each HA block starts with a control block section (see \ref
 * ha_mem_blk_hdr_t). The first word of the control block uses this
 * value.
 */
#define HA_MEM_SIGNATURE    0xa75c9b3e

/*!
 * \brief Invalid block section.
 *
 * Each HA control block (see \ref ha_mem_blk_hdr_t) specifies the
 * section it belong to.
 *
 * HA blocks that used for internal ISSU purpose are linked together
 * where the pointer of such a block contains the section number and
 * the offset of the block within this section. To specify 'NULL'
 * pointer, we use this invalid block section as the section number.
 */
#define INVALID_BLOCK_SECTION    0xFFFFFFFF

#define MSECT_VECTOR_CHUNK  10

#define BLK_HDR_NEXT(_bh) \
    (ha_mem_blk_hdr_t *)((uint8_t *)(_bh) + (_bh)->length)

/*!
 * \brief HA Block state.
 *
 * Each HA block is either free or allocated. The HA block control
 * section (see \ref ha_mem_blk_hdr_t) contains the state field that
 * indicate the state of each particular block.
 */
typedef enum {
    HA_BLK_FREE = 0,
    HA_BLK_ALLOCATED = 1,
    HA_BLK_MARKED = 0x80
} ha_mem_block_state_t;

/*!
 * \brief Memory block header type.
 *
 * Every memory block contains the following header structure.  The
 * structure contains information required for the memory manager as
 * well as for version upgrade/downgrade
 */
typedef struct {
    /*! Every block starts with signature */
    uint32_t signature;
    /*! Unique block ID */
    uint32_t blk_id;
    /*! Block state (allocated/free) */
    uint8_t state;
    /*! Block length including the header */
    uint32_t length;
    /*!
     * The offset to the begining of the previous block. In reality it
     * is also the length of the previous block.  The section start
     * block (i.e. mem_start field) maintains the length of the last
     * block of the section.
     */
    uint32_t prev_offset;
    /*! The section containing the block */
    uint32_t section;
    /*! A unique string ID of the block given the the component */
    char blk_str_id[MAX_BLK_ID_STR_LEN];
} ha_mem_blk_hdr_t;

/*!
 * \brief Section header.
 *
 * Every section of HA memory contains a control block that helps to
 * manage the section. The control block being allocated within
 * regular memory.
 */
typedef struct {
    /*! Pointer to the beginning of the HA memory block for this section */
    ha_mem_blk_hdr_t *mem_start;
    /*! Out of bounds block */
    ha_mem_blk_hdr_t *mem_end;
    /*! Pointer to the free memory block */
    ha_mem_blk_hdr_t *free_mem;
    /*! The section length in bytes */
    uint32_t sect_len;
} ha_mem_section_t;

/*!
 * \brief Control structure per unit.
 *
 * This is the main data structure that manages all the HA memory
 * associated with a single unit.
 */
typedef struct ha_private_s {
    /*! The size (in elements) of the mem_sect array */
    size_t msect_size;
    /*!
     * The index of the first free element in the array. Everything above
     * this index is also empty.
     */
    size_t msec_free_idx;
    /*! Vector contains all the HA memory sections */
    ha_mem_section_t *mem_sect;
    /*! Pointer to the begining of the free block */
    ha_mem_blk_hdr_t *free_mem;
    /*! Total allocated HA memory (used + unused) */
    uint32_t blk_len;
    /*! Indicates if created new file or opened existing file */
    bool new_file;
    /*! Protect HA memory resource */
    sal_mutex_t mutex;
    /*! Indicates if mem_init() method was called */
    bool initialized;
    /*! HA memory pool allocation callback */
    shr_ha_pool_alloc_f alloc_cb;
    /*! HA memory pool free callback */
    shr_ha_pool_free_f free_cb;
    /*! Initial HA memory pointer */
    void *init_shr_mem;
    /*! Opaque pointer provided to alloc_cb and free_cb */
    void *ctx;
} ha_private_t;

/*******************************************************************************
 * Private functions
 */

/*
 * This function is equivalent in functionality to the C++ vector::push_back.
 */
static void
push_back(ha_private_t *this, ha_mem_section_t *sec)
{
    if (this->msec_free_idx + 1 >= this->msect_size) {
        ha_mem_section_t *tmp;
        size_t alloc_size = sizeof(ha_mem_section_t) *
            (MSECT_VECTOR_CHUNK + this->msect_size);
        /* need to realloc */
        tmp = sal_alloc(alloc_size, "shrHaMemSect");
        if (!tmp) {
            return;
        }
        sal_memset(tmp, 0, alloc_size);
        sal_memcpy(tmp,
                   this->mem_sect,
                   sizeof(ha_mem_section_t) * this->msect_size);
        this->msect_size += MSECT_VECTOR_CHUNK;
        sal_free(this->mem_sect);
        this->mem_sect = tmp;
    }
    this->mem_sect[this->msec_free_idx++] = *sec;
}

/*!
 * \brief Validate the entire HA memory blocks
 *
 * This function iterates through all the HA blocks and validates the
 * following:
 *
 * a. The block signature
 * b. The block length matches the next block previous offset.
 *
 * This function is called before freeing the HA memory and its
 * purpose is to detect memory overwrites that occured during the run.
 *
 * \param [in] this The memory manager instance.
 *
 * \return None.
 */
static void
validate_memory_integrity(ha_private_t *this)
{
    ha_mem_section_t *section = this->mem_sect;
    ha_mem_blk_hdr_t *blk_hdr, *prev_hdr;
    shr_ha_mod_id mod_id;
    shr_ha_sub_id sub_id;
    uint32_t j = 0;

    while (section && j < this->msec_free_idx) {
        blk_hdr = section->mem_start;
        prev_hdr = NULL;
        while (blk_hdr < section->mem_end) {
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                if (prev_hdr) {
                    BLK_ID_RET(prev_hdr->blk_id, mod_id, sub_id);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("HA memory is corrupted, "
                                        "last block comp=%d sub=%d id=%s\n"),
                               mod_id, sub_id, prev_hdr->blk_str_id));
                } else {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("Block length and next block prev doesn't match\n")));
                }
            }
            prev_hdr = blk_hdr;
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
            if (blk_hdr < section->mem_end &&
                prev_hdr->length != blk_hdr->prev_offset) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("Block length and next block prev doesn't match\n")));
            }
        }
        if (blk_hdr > section->mem_end) {
            if (prev_hdr) {
                BLK_ID_RET(prev_hdr->blk_id, mod_id, sub_id);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("HA block length exceeds its section end, "
                                    "len=%u last block comp=%d sub=%d id=%s\n"),
                           prev_hdr->length, mod_id, sub_id, prev_hdr->blk_str_id));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("HA block length exceeds its section end\n")));
            }
        }
        section++;
        j++;
    }
}

/*!
 * \brief Consolidate sections that were allocated during previous run.
 *
 * This function runs after reopening an existing file. It goes
 * through the entire memory to verify that every block has valid
 * signature. It also "fixes" some of the prev_offset fields. These
 * needed to be fixed when the original allocation was made using
 * multiple sections. When the file being reopen it will have a single
 * section containing all the sections that were there in the original
 * memory allocation.
 *
 * \param [in] this The context of the HA memory.
 *
 * \retval true no errors.
 * \retval false the memory is corrupted.
 */
static int
consolidate_sections(ha_private_t *this)
{
    uint32_t section;
    uint32_t prev_blk_len = 0;
    ha_mem_blk_hdr_t *blk_hdr = this->mem_sect[0].mem_start;
    ha_mem_blk_hdr_t *prev_blk_hdr = NULL;

    section = blk_hdr->section;
    do {
        if (blk_hdr->signature != HA_MEM_SIGNATURE) {
            if (prev_blk_hdr) {
                shr_ha_mod_id mod_id;
                shr_ha_sub_id sub_id;
                BLK_ID_RET(prev_blk_hdr->blk_id, mod_id, sub_id);
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("HA memory is being corrupt, "
                                    "last block comp=%d sub=%d id=%s\n"),
                           mod_id, sub_id, prev_blk_hdr->blk_str_id));
            } else {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("First block is being corrupt\n")));
            }
            return SHR_E_INTERNAL;
        }
        /* Search for section transition */
        if (section != blk_hdr->section) {
            section = blk_hdr->section;
            /*
             * Consolidate adjacent free blocks that were on both sides of a
             * section boundary.
             */
            if (prev_blk_hdr && blk_hdr->state == HA_BLK_FREE &&
                prev_blk_hdr->state == HA_BLK_FREE) {
                ha_mem_blk_hdr_t *next_blk_hdr;
                prev_blk_hdr->length += blk_hdr->length;
                next_blk_hdr = BLK_HDR_NEXT(blk_hdr);
                if (next_blk_hdr < this->mem_sect[0].mem_end) {
                    next_blk_hdr->prev_offset = prev_blk_hdr->length;
                }
                blk_hdr->section = 0;
                blk_hdr = next_blk_hdr;
                continue;
            }
            blk_hdr->prev_offset = prev_blk_len;
        }
        /* Upon initialization everything belongs to the only section */
        blk_hdr->section = 0;
        prev_blk_len = blk_hdr->length;
        if (prev_blk_hdr && prev_blk_hdr->length != blk_hdr->prev_offset) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("Block insane, prev->len (%u) != blk->prev_len (%u)\n"),
                       prev_blk_hdr->length, blk_hdr->prev_offset));
        }
        prev_blk_hdr = blk_hdr;
        blk_hdr = BLK_HDR_NEXT(blk_hdr);
    } while (blk_hdr < this->mem_sect[0].mem_end);

    return SHR_E_NONE;
}

/*!
 * \brief find a block with matched block ID.
 *
 * Search the entire HA memory for an occupied block with matching blk_id.
 *
 * \param [in] this The context of the HA memory.
 * \param [in] blk_id Is the block ID to search for.
 *
 * \retval pointer to the block if was found.
 * \retval NULL if the block was not found.
 */
static ha_mem_blk_hdr_t *
block_find(ha_private_t *this, uint32_t blk_id)
{
    ha_mem_blk_hdr_t *blk_hdr = NULL;
    ha_mem_blk_hdr_t *last_blk_hdr;
    ha_mem_section_t *it;
    size_t j;

    /* Iterate through all the sections */
    for (j = 0; j < this->msec_free_idx; j++) {
        it = &this->mem_sect[j];
        last_blk_hdr = blk_hdr;
        blk_hdr = it->mem_start;
        /* Search each block in this section */
        do {
            /* Validate signature */
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("invalid HA block signature\n")));
                if (last_blk_hdr) {
                    shr_ha_mod_id mod_id;
                    shr_ha_sub_id sub_id;
                    BLK_ID_RET(last_blk_hdr->blk_id, mod_id, sub_id);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("Last block comp=%u, sub=%u, blk_id=%s\n"),
                              mod_id, sub_id, last_blk_hdr->blk_str_id));
                } else {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("First block is corrupted\n")));
                }
                assert(0);
                return NULL;
            }
            /*
             * If the block is allocated and the block ID matches then we
             * found it
             */
            if ((blk_hdr->state == HA_BLK_ALLOCATED) &&
                (blk_hdr->blk_id == blk_id)) {
                return blk_hdr;
            }
            last_blk_hdr = blk_hdr;
            /* Increment the block pointer to the next block */
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
            /* Bail out if block pointer points beyond the end */
        } while (blk_hdr < it->mem_end);
    }
    return NULL;
}

/*!
 * \brief Find free block of size larger or equal to min_length.
 *
 * Start searching at the section containing the free_mem block. Also
 * start searching from that free_mem block.
 *
 * In the future this module can be optimized by maintaining a linked
 * list of all the free blocks.
 *
 * \param [in] this The HA block context.
 * \param [in] min_length is the minimal length of the free block to search for.
 *
 * \retval pointer to the block if was found.
 * \retval NULL if the block was not found.
 */
static ha_mem_blk_hdr_t *
find_free_block(ha_private_t *this, uint32_t min_length)
{
    ha_mem_blk_hdr_t *blk_hdr;
    int section_idx;
    unsigned j;
    unsigned mem_free_section;

    /*
     * Search in all the memory sections
     */
    if (this->free_mem) {
        mem_free_section = this->free_mem->section;
    } else {
        mem_free_section = 0;
    }

    for (j = 0; j < this->msec_free_idx; j++) {
        section_idx = (j + mem_free_section) % this->msec_free_idx;
        /* Start the search at the mem_free block */
        blk_hdr = this->mem_sect[section_idx].free_mem;
        do {
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("invalid Ha block signature\n")));
                assert (0);
                return NULL;
            }

            if ((blk_hdr->state == (uint8_t)HA_BLK_FREE) &&
                (blk_hdr->length >= min_length)) {
                blk_hdr->section = section_idx;
                return blk_hdr;
            }
            /* Increment to the next block */
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
            if (blk_hdr >= this->mem_sect[section_idx].mem_end) {
                /* Wrap around to the beginning of the section */
                blk_hdr = this->mem_sect[section_idx].mem_start;
            }
            /* As long as we are not were we started */
        } while (blk_hdr != this->mem_sect[section_idx].free_mem);
    }
    return NULL;
}

/*!
 * \brief Find the previous block in the chain.
 *
 * If the input doesn't point to the start of the section then use the
 * prev_offset to move the block pointer accordingly.
 *
 * \param [in] this The context of the HA memory.
 * \param [in] start A reference to the block where the function
 *             is looking for its predecesor.
 *
 * \retval pointer to the block if was found.
 * \retval NULL if the block was not found.
 */
static ha_mem_blk_hdr_t *
find_prev_block(ha_private_t *this, ha_mem_blk_hdr_t *start)
{
    ha_mem_blk_hdr_t *prev_blk_hdr;
    /* Check if start points to the beginning of the section */
    if (start == this->mem_sect[start->section].mem_start) {
        return NULL;    /* No previous to the start */
    }
    prev_blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)start - start->prev_offset);
    /* Sanity check */
    if ((prev_blk_hdr < this->mem_sect[start->section].mem_start) ||
        (prev_blk_hdr > this->mem_sect[start->section].mem_end) ||
        (prev_blk_hdr->signature != HA_MEM_SIGNATURE)) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("invalid previous block\n")));
        assert(0);
    }

    prev_blk_hdr->section = start->section;
    return prev_blk_hdr;
}

/*!
 * \brief Allocate new HA memory section
 *
 * Allocate a new HA memory pool via the application-provided
 * callback. Internally, we call this HA memory pool a "section".
 *
 * Each section managed by section header that contains information
 * about the header length and free memory.
 *
 * \param [in] this The context of the HA memory.
 * \param [in] size The extension size.
 *
 * \retval Pointer to the extended block.
 * \retval NULL if something failed.
 */
static ha_mem_blk_hdr_t *
section_resize(ha_private_t *this, int size)
{
    ha_mem_section_t new_sect;
    int last_section = this->msec_free_idx-1;
    uint32_t resize_memory_blocks = size + RESIZE_MEMORY_SIZE;
    uint32_t addition_mem_size = 0;
    ha_mem_blk_hdr_t *blk_hdr;
    void *new_memptr;

    new_memptr = this->alloc_cb(this->ctx,
                                resize_memory_blocks,
                                &addition_mem_size);
    if ((new_memptr == NULL) || (addition_mem_size < resize_memory_blocks)) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("HA memory resize failure\n")));
        return NULL;
    } else {
        /*
         * Was able to create new memory map section. Set the new section
         * header and add it to the memory section vector
         */
        new_sect.mem_start = (ha_mem_blk_hdr_t *)new_memptr;
        new_sect.mem_end = (ha_mem_blk_hdr_t *)
            ((uint8_t *)new_sect.mem_start + addition_mem_size);
        new_sect.free_mem = new_sect.mem_start;
        new_sect.sect_len = addition_mem_size;
        push_back(this, &new_sect);
        last_section++; /* Added a new section  */
        blk_hdr = new_sect.mem_start;
    }
    /* Now add the free block at the end of the previous block */
    blk_hdr->signature = HA_MEM_SIGNATURE;
    blk_hdr->length = addition_mem_size;
    blk_hdr->state = (uint8_t)HA_BLK_FREE;
    blk_hdr->section = last_section;
    this->blk_len += addition_mem_size;    /* Update the total block length */
    this->free_mem = blk_hdr;  /* Update the global free_mem to the new block */
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META("start=%p, end=%p, len=%d, free=%p\n"),
                        (void *)this->mem_sect[0].mem_start,
                        (void *)this->mem_sect[0].mem_end,
                        this->blk_len,
                        (void *)this->mem_sect[0].free_mem));
    return blk_hdr;
}

/*!
 * \brief Sanity check for the block length and prev offsets.
 *
 * This function checks that the previous and next block within the
 * chain are matching their parameters with the current block.
 *
 * \param [in] this The HA instance.
 * \param [in] blk_hdr The block under verification.
 *
 * \retval 0 on success and -1 when inconsistency was found.
 */
static int
check_sanity(ha_private_t *this, ha_mem_blk_hdr_t *blk_hdr)
{
    ha_mem_blk_hdr_t *tmp_blk_hdr;
    ha_mem_section_t *section = &this->mem_sect[blk_hdr->section];

    tmp_blk_hdr = BLK_HDR_NEXT(blk_hdr);
    if (tmp_blk_hdr < section->mem_end) {
       if (tmp_blk_hdr->prev_offset != blk_hdr->length ||
           tmp_blk_hdr->signature != HA_MEM_SIGNATURE) {
           shr_ha_mod_id mod_id;
           shr_ha_sub_id sub_id;
           BLK_ID_RET(blk_hdr->blk_id, mod_id, sub_id);
           LOG_ERROR(BSL_LOG_MODULE,
                     (BSL_META("Memory corruption, "
                               "last block comp=%u, sub=%u, blk_id=%s\n"),
                     mod_id, sub_id, blk_hdr->blk_str_id));
           return -1;
       }
    }

    if (blk_hdr == section->mem_start) {
       return 0;
    }
    tmp_blk_hdr = (ha_mem_blk_hdr_t *)
        ((uint8_t *)blk_hdr - blk_hdr->prev_offset);
    if (tmp_blk_hdr->length != blk_hdr->prev_offset ||
        tmp_blk_hdr->signature != HA_MEM_SIGNATURE) {
       return -1;
    }
    return 0;
}

/*!
 * \brief Allocate internal block with minimal length.
 *
 * Allocate an HA block that has specific length. The allocation is
 * for internal use.
 *
 * \param [in] this The HA control block.
 * \param [in] length Minimal length of the allocated block.
 *
 * \retval pointer to the block if was found.
 * \retval NULL if the block was not found.
 */
static ha_mem_blk_hdr_t *
alloc_internal_block(ha_private_t *this, uint32_t length)
{
    uint32_t space_to_allocate;
    ha_mem_blk_hdr_t *blk_hdr;
    ha_mem_blk_hdr_t *new_blk_hdr, *next_blk_hdr;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META("allocating new buffer free=%p\n"),
                 (void *)this->free_mem));
    /* Make sure that the length is multiplication of 8 */
    space_to_allocate = ((length + sizeof(ha_mem_blk_hdr_t) + 7) >> 3) << 3;
    /* Now we assume that we will find a free block large enough. */
    blk_hdr = find_free_block(this, space_to_allocate);
    if (!blk_hdr) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META("calling section_resize\n")));
        /*
         * If we can't find free block that is large enough we need to
         * extend the HA memory.
         */
        blk_hdr = section_resize(this, space_to_allocate);
        if (!blk_hdr) {  /* HA memory extension fails? */
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("HA internal resize failure\n")));
            sal_mutex_give(this->mutex);
            return NULL;
        }
    }

    new_blk_hdr = NULL;
    next_blk_hdr = BLK_HDR_NEXT(blk_hdr);
    /*
     * Set the new free block if some space had left for new block header. In
     * such a case we split the large block to the desired block and we place
     * a new header for whatever space left. We also need to adjust the
     * previous offset of the next block to the size of the new leftover block.
     * If on the other hand the block is not sufficient enough in its size to
     * contain a new block we simply take the block as it is. No adjustments
     * are necessary.
     */
    if (blk_hdr->length > space_to_allocate + sizeof(ha_mem_blk_hdr_t)) {
        new_blk_hdr = (ha_mem_blk_hdr_t *)
            ((uint8_t *)blk_hdr + space_to_allocate);
        sal_memset(new_blk_hdr, 0, sizeof(ha_mem_blk_hdr_t));
        new_blk_hdr->length = blk_hdr->length - space_to_allocate;
        new_blk_hdr->signature = HA_MEM_SIGNATURE;
        new_blk_hdr->state = (uint8_t)HA_BLK_FREE;
        new_blk_hdr->section = blk_hdr->section;
        new_blk_hdr->prev_offset = space_to_allocate;
        if (this->mem_sect[blk_hdr->section].mem_end > next_blk_hdr) {
            next_blk_hdr->prev_offset = new_blk_hdr->length;
        }
        blk_hdr->length = space_to_allocate;
    }

    blk_hdr->state = (uint8_t)HA_BLK_ALLOCATED;
    if (blk_hdr == this->free_mem) {
        /* Find new value for the free mem  */
        if (new_blk_hdr) {
            this->free_mem = new_blk_hdr;
        } else {  /* Need to find free block */
            this->free_mem =
                find_free_block(this, (uint32_t)(sizeof(ha_mem_blk_hdr_t) + 1));
            if (!this->free_mem) { /* Make sure that free_mem is not null */
                this->free_mem = this->mem_sect[0].mem_start;
                this->free_mem->section = 0;
            }
        }
    }

    return blk_hdr;
}

/*******************************************************************************
 * Public functions
 */

int
shr_ha_mm_create(void *mmap,
                 int size,
                 shr_ha_pool_alloc_f alloc_f,
                 shr_ha_pool_free_f free_f,
                 void *context,
                 void **private)
{
    ha_private_t *this;
    size_t blk_size = sizeof(ha_mem_section_t) * MSECT_VECTOR_CHUNK;

    if (!alloc_f) {
        /* No way to allocate HA memory */
        *private = NULL;
        return SHR_E_PARAM;
    }
    /* For cold boot only we can play with the file size */
    if (!mmap && size < HA_MEM_SIZE_MIN) {
        size = HA_MEM_SIZE_MIN;
    }
    this = sal_alloc(sizeof(ha_private_t), "shrHaPriv");
    if (!this) {
        return SHR_E_MEMORY;
    }
    sal_memset(this, 0, sizeof(ha_private_t));
    this->mem_sect = sal_alloc(blk_size, "shrHaMemSect");
    if (!this->mem_sect) {
        sal_free(this);
        return SHR_E_MEMORY;
    }
    sal_memset(this->mem_sect,
               0,
               sizeof(ha_mem_section_t) * MSECT_VECTOR_CHUNK);
    this->msect_size = MSECT_VECTOR_CHUNK;
    this->msec_free_idx = 0;
    this->mutex = sal_mutex_create((char *)"ha_mutex");
    assert (this->mutex != NULL);
    this->initialized = false;
    this->blk_len = size;
    this->free_mem = NULL;
    this->init_shr_mem = mmap;
    this->alloc_cb = alloc_f;
    this->free_cb = free_f;
    this->ctx = context;
    *private = (void *)this;
    return SHR_E_NONE;
}

void
shr_ha_mm_delete(void *private)
{
    ha_private_t *this = (ha_private_t *)private;
    size_t j;
    ha_mem_section_t *it;
    uint8_t *mem_start;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    validate_memory_integrity(this);
    if (this->free_cb) {
        for (j = 0; j < this->msec_free_idx; j++) {
            it = &this->mem_sect[j];
            if (it->mem_start) {
                mem_start = (uint8_t *)it->mem_start;
                this->free_cb(this->ctx, (void *)mem_start, it->sect_len);
            }
        }
    }
    sal_free(this->mem_sect);
    sal_mutex_give(this->mutex);
    sal_mutex_destroy(this->mutex);
    sal_free(this);
}

int
shr_ha_mm_reset(void *private)
{
    ha_private_t *this = (ha_private_t *)private;
    unsigned j;
    ha_mem_blk_hdr_t *blk_hdr;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);

    for (j = 0; j < this->msec_free_idx; j++) {
        blk_hdr = this->mem_sect[j].mem_start;
        /* Check that the section is sane */
        do {
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("invalid Ha block signature\n")));
                break;
            }

            /* Increment to the next block */
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
            /* As long as we are inside the section */
        } while (blk_hdr < this->mem_sect[j].mem_end);

        blk_hdr = this->mem_sect[j].mem_start;
        sal_memset(blk_hdr+1, 0, this->mem_sect[j].sect_len - sizeof(*blk_hdr));
        blk_hdr->length = this->mem_sect[j].sect_len;
        sal_memset(blk_hdr->blk_str_id, 0, sizeof(blk_hdr->blk_str_id));
        blk_hdr->blk_id = 0;
        blk_hdr->section = j;
        if (j > 0) {
            blk_hdr->prev_offset = this->mem_sect[j-1].sect_len;
        } else {
            blk_hdr->prev_offset = this->blk_len;
        }
        blk_hdr->state = (uint8_t)HA_BLK_FREE;
        blk_hdr->signature = HA_MEM_SIGNATURE;
    }

    sal_mutex_give(this->mutex);
    return SHR_E_NONE;
}

int
shr_ha_mm_init(void *private)
{
    ha_mem_blk_hdr_t *first_hdr;
    ha_mem_section_t first_sect;
    int rv = SHR_E_NONE;
    ha_private_t *this = (ha_private_t *)private;
    uint32_t actual_size;

    if (this->initialized) {
        return SHR_E_INIT;
    }

    if (this->init_shr_mem == NULL) {
        /* Allocate initial HA memory pool (cold boot) */
        this->init_shr_mem = this->alloc_cb(this->ctx, this->blk_len,
                                            &actual_size);
        if (this->init_shr_mem == NULL) {
            sal_free(this->mem_sect);
            sal_free(this);
            return SHR_E_MEMORY;
        }
        this->blk_len = actual_size;
        this->new_file = true;
    }

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);

    first_sect.mem_start = (ha_mem_blk_hdr_t *)this->init_shr_mem;
    first_sect.mem_end =
        (ha_mem_blk_hdr_t *)((uint8_t *)this->init_shr_mem + this->blk_len);
    first_sect.free_mem = first_sect.mem_start;
    first_sect.sect_len = this->blk_len;
    push_back(this, &first_sect);

    /* The free memory starts at the first section mem start */
    this->free_mem = first_hdr = first_sect.mem_start;

    if (this->new_file) {
        first_hdr->signature = HA_MEM_SIGNATURE;
        first_hdr->length = this->blk_len;
        first_hdr->state = (uint8_t)HA_BLK_FREE;
        first_hdr->section = 0;
        first_hdr->prev_offset = this->blk_len;
    } else {
        rv = consolidate_sections(this);
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META("start=%p, end=%p, len=%d, free=%p\n"),
                        (void *)this->mem_sect[0].mem_start,
                        (void *)this->mem_sect[0].mem_end,
                        this->blk_len,
                        (void *)this->mem_sect[0].free_mem));
    sal_mutex_give(this->mutex);
    this->initialized = (rv == SHR_E_NONE);
    return rv;
}

void *
shr_ha_mm_alloc(void *private,
                shr_ha_mod_id mod_id,
                shr_ha_sub_id sub_id,
                const char *blk_str_id,
                uint32_t *length)
{
    /* blk_hdr points to the first allocated block */
    ha_mem_blk_hdr_t *blk_hdr;
    uint32_t blk_id = BLK_ID_GEN(mod_id, sub_id);
    ha_private_t *this = (ha_private_t *)private;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    /* Search for a block with match mod and sub ID */
    blk_hdr = block_find(this, blk_id);
    if (!blk_hdr) {   /* Buffer was not allocated before */
        /* Don't allocate zero length block */
        if (*length == 0 && this->new_file) {
            return NULL;
        }
        blk_hdr = alloc_internal_block(this, *length);
        if (!blk_hdr) {
            sal_mutex_give(this->mutex);
            return NULL;
        }
        blk_hdr->blk_id = blk_id;
    } /* Else, block was allocated before. Since we don't free we can reuse. */

    if (blk_str_id) {
        sal_strncpy(blk_hdr->blk_str_id, blk_str_id,
                    sizeof(blk_hdr->blk_str_id));
        blk_hdr->blk_str_id[sizeof(blk_hdr->blk_str_id)-1] = '\0';
    }
    /* Return consistent length in either case */
    *length = blk_hdr->length - sizeof(ha_mem_blk_hdr_t);

    if (check_sanity(this, blk_hdr) != 0) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Insane HA block\n")));
    }
    sal_mutex_give(this->mutex);
    return (void *)((uint8_t *)blk_hdr + sizeof(ha_mem_blk_hdr_t));
}

int
shr_ha_mm_free(void *private, void *mem)
{
    ha_mem_blk_hdr_t *blk_hdr = (ha_mem_blk_hdr_t *)
    ((uint8_t *)mem - sizeof(ha_mem_blk_hdr_t));
    ha_mem_blk_hdr_t *adj_blk_hdr;
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_section_t *section;

    if (!mem) {
        return SHR_E_PARAM;
    }
    /* Sanity check that the block is valid */
    if ((blk_hdr->signature != HA_MEM_SIGNATURE)||
       (blk_hdr->state !=(uint8_t)HA_BLK_ALLOCATED)) {
        return SHR_E_PARAM;
    }

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    if (check_sanity(this, blk_hdr) != 0) {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Insane HA block\n")));
    }
    /* Free the block */
    blk_hdr->state = (uint8_t)HA_BLK_FREE;
    section = &this->mem_sect[blk_hdr->section];
    /* If the next block is free merge them together. */
    adj_blk_hdr = BLK_HDR_NEXT(blk_hdr);
    if (adj_blk_hdr < section->mem_end) {
        if (adj_blk_hdr->signature != HA_MEM_SIGNATURE) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META("invalid HA block signature\n")));
            assert(0);
        }
        if (adj_blk_hdr->state ==(uint8_t)HA_BLK_FREE) {
            /* Concatenate the next block */
            blk_hdr->length += adj_blk_hdr->length;
            adj_blk_hdr->signature = 0;
            /* Adjust the previous offset of the next block */
            adj_blk_hdr = BLK_HDR_NEXT(blk_hdr);
            if (adj_blk_hdr < section->mem_end) {
                assert(adj_blk_hdr->state == HA_BLK_ALLOCATED);
                adj_blk_hdr->prev_offset = blk_hdr->length;
            }
            if (check_sanity(this, blk_hdr) != 0) {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Insane HA block\n")));
            }
        }
    }

    /* Now see if this block can be concatenated with the previous block */
    adj_blk_hdr = find_prev_block(this, blk_hdr);
    if (adj_blk_hdr && (adj_blk_hdr->state == HA_BLK_FREE)) {
        /* Add blk_hdr to the previous block */
        adj_blk_hdr->length += blk_hdr->length;
        /* Adjust the previous offset of the next block */
        blk_hdr = BLK_HDR_NEXT(adj_blk_hdr);
        if (blk_hdr < section->mem_end) {
            blk_hdr->prev_offset = adj_blk_hdr->length;
        }
        if (check_sanity(this, adj_blk_hdr) != 0) {
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META("Insane HA block\n")));
        }
    }
    sal_mutex_give(this->mutex);
    return SHR_E_NONE;
}

void *
shr_ha_mm_realloc(void *private,
                  void *mem,
                  uint32_t length)
{
    ha_mem_blk_hdr_t *blk_hdr;
    void *new_blk_data;
    shr_ha_mod_id mod_id;
    shr_ha_sub_id sub_id;

    if (!mem) {
        return NULL;
    }
    blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)mem - sizeof(ha_mem_blk_hdr_t));
    /* Sanity check */
    if ((blk_hdr->signature != HA_MEM_SIGNATURE) ||
        (blk_hdr->state != (uint8_t)HA_BLK_ALLOCATED)) {
        return NULL;
    }

    /* Is the new length shorter? */
    if (blk_hdr->length - sizeof(ha_mem_blk_hdr_t) >= length) {
        return mem;
    }
    /* Change the blk ID so we can allocate a new one with the same ID */
    /* First save the ID */
    BLK_ID_RET(blk_hdr->blk_id, mod_id, sub_id);

    if (blk_hdr->blk_id == 0) {
        blk_hdr->blk_id = 1;
    } else {
        blk_hdr->blk_id = 0;
    }
    new_blk_data = shr_ha_mm_alloc(private,
                                   mod_id,
                                   sub_id,
                                   blk_hdr->blk_str_id,
                                   &length);
    if (!new_blk_data) {
        return NULL;
    }
    sal_memcpy (new_blk_data,
                mem,
                blk_hdr->length - sizeof(ha_mem_blk_hdr_t));

    /* free the old block */
    shr_ha_mm_free(private, mem);

    return new_blk_data;
}

int
shr_ha_mm_usage(void *private,
                shr_ha_mod_id mod_id,
                uint32_t report_size,
                shr_ha_usage_report_t *reports)
{
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_blk_hdr_t *blk_hdr;
    ha_mem_blk_hdr_t *prev_blk_hdr;
    ha_mem_section_t *it;
    size_t j;
    shr_ha_sub_id sub_id;
    bool comp_found = false;
    shr_ha_mod_id module_id;

    if (!reports) {
        return SHR_E_PARAM;
    }
    sal_memset(reports, 0, sizeof(shr_ha_usage_report_t) * report_size);

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    /* Iterate through all the sections */
    for (j = 0; j < this->msec_free_idx; j++) {
        it = &this->mem_sect[j];
        blk_hdr = it->mem_start;
        prev_blk_hdr = NULL;
        /* Search each block in this section */
        do {
            /* Validate signature */
            if (blk_hdr->signature != HA_MEM_SIGNATURE) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META("invalid Ha block signature\n")));
                if (prev_blk_hdr) {
                    shr_ha_mod_id prev_mod_id;
                    BLK_ID_RET(prev_blk_hdr->blk_id, prev_mod_id, sub_id);
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META("HA memory is being corrupt, "
                                      "last comp=%d sub=%d\n"),
                               prev_mod_id, sub_id));

                }
                assert(0);
                sal_mutex_give(this->mutex);
                return SHR_E_NOT_FOUND;
            }
            /*
             * If the block is allocated and the block component ID matches
             * then we found it
             */
            BLK_ID_RET(blk_hdr->blk_id, module_id, sub_id);
            if ((blk_hdr->state == (uint8_t)HA_BLK_ALLOCATED) &&
                (module_id == mod_id)) {
                comp_found  = true;
                if (sub_id < report_size) {
                    reports[sub_id].size =
                            blk_hdr->length - sizeof(ha_mem_blk_hdr_t);
                    reports[sub_id].blk_id = blk_hdr->blk_str_id;
                }
            }
            /* Increment the block pointer to the next block */
            prev_blk_hdr = blk_hdr;
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
            /* Bail out if block pointer points beyond the end */
        } while (blk_hdr < it->mem_end);
    }
    sal_mutex_give(this->mutex);

    return (comp_found ? SHR_E_NONE : SHR_E_NOT_FOUND);
}

int
shr_ha_mm_dump(void *private,
               int (*write_f)(void *buf, size_t len))
{
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_section_t *section = this->mem_sect;
    ha_mem_blk_hdr_t *blk_hdr;
    uint32_t j = 0;

    sal_mutex_take(this->mutex, SAL_MUTEX_FOREVER);
    while (section && j < this->msec_free_idx) {
        blk_hdr = section->mem_start;
        while (blk_hdr < section->mem_end) {
            if (blk_hdr->signature == HA_MEM_SIGNATURE &&
                blk_hdr->state == HA_BLK_ALLOCATED) {
                if (write_f(blk_hdr, blk_hdr->length) != (int)blk_hdr->length) {
                    sal_mutex_give(this->mutex);
                    return SHR_E_IO;
                }
            }
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
        }
        section++;
        j++;
    }
    sal_mutex_give(this->mutex);
    return SHR_E_NONE;
}

int
shr_ha_mm_comp(void *private,
               int (*read_f)(void *buf, size_t len))
{
    ha_private_t *this = (ha_private_t *)private;
    ha_mem_blk_hdr_t *blk_hdr;
    uint8_t *buf = NULL;
    size_t buf_len = 0;
    ha_mem_blk_hdr_t blk_hdr_in;
    int rv;
    int rc = SHR_E_NONE;
    uint32_t data_len;
    ha_mem_section_t *section;
    uint32_t j;

    rv = read_f(&blk_hdr_in, sizeof(blk_hdr_in));
    while (rv == sizeof(blk_hdr_in) &&
           blk_hdr_in.signature == HA_MEM_SIGNATURE) {
        data_len = blk_hdr_in.length - sizeof(blk_hdr_in);
        if (buf_len < data_len) {
            if (buf) {
                sal_free(buf);
            }
            buf = sal_alloc(data_len, "shrHaComp");
            if (!buf) {
                return SHR_E_MEMORY;
            }
            buf_len = data_len;
        }
        rv = read_f(buf, data_len);
        if (rv < (int)data_len) {
            rc = SHR_E_IO;
            break;
        }

        /* Find a block with the same ID */
        blk_hdr = block_find(this, blk_hdr_in.blk_id);
        if (blk_hdr && sal_memcmp(blk_hdr + 1, buf, data_len) != 0) {
            uint32_t k;
            uint8_t *b = (uint8_t *)(blk_hdr + 1);
            /* Find the location where the blocks starting to divert */
            for (k = 0; k < data_len; k++, b++) {
                if (*b != buf[k]) {
                    LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META("HA block mismatch module %d, sub %d, blk_id %s"
                                   " len=%u, offset=%u expected 0x%x found 0x%x\n"),
                          blk_hdr_in.blk_id >> 16, blk_hdr_in.blk_id & 0xFFFF,
                          blk_hdr->blk_str_id, data_len, k, *b, buf[k]));
                    break;
                }
            }
        }

        if (!blk_hdr) {
            LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META("HA block does not currently exist for "
                                   "module %d, sub %d, blk_id %s\n"),
                          blk_hdr_in.blk_id >> 16,
                          blk_hdr_in.blk_id & 0xFFFF,
                          blk_hdr_in.blk_str_id));
        } else {
            blk_hdr->state |= HA_BLK_MARKED;  /* Marked the block as processed */
        }
        /* Read the next block header */
        rv = read_f(&blk_hdr_in, sizeof(blk_hdr_in));
    }

    if (buf) {
        sal_free(buf);
    }

    /* Check if all the allocated blocks have been checked */
    section = this->mem_sect;
    j = 0;
    while (section && j < this->msec_free_idx) {
        blk_hdr = section->mem_start;
        while (blk_hdr < section->mem_end) {
            if (blk_hdr->signature == HA_MEM_SIGNATURE &&
                (blk_hdr->state & (~HA_BLK_MARKED)) == HA_BLK_ALLOCATED) {
                if (blk_hdr->state & HA_BLK_MARKED) {
                    /* Un-mark the block as processed */
                    blk_hdr->state = HA_BLK_ALLOCATED;
                } else {
                    LOG_WARN(BSL_LOG_MODULE,
                                 (BSL_META("HA block was not exist before."
                                           " Module %d, sub %d, blk_id %s\n"),
                                  blk_hdr->blk_id >> 16,
                                  blk_hdr->blk_id & 0xFFFF,
                                  blk_hdr->blk_str_id));
                }
            }
            blk_hdr = BLK_HDR_NEXT(blk_hdr);
        }
        section++;
        j++;
    }

    return rc;
}

int
shr_ha_mm_restore(void *private,
                  int (*read_f)(void *buf, size_t len))
{
    return SHR_E_NO_HANDLER;
}
