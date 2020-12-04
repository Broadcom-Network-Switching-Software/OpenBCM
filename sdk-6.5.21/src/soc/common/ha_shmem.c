
/*! \file ha_shmem.c
 * 
 *
 * HA shared memory manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
typedef int ha_shmem_ISO_C_forbids_an_empty_source_file;
#if !defined(__KERNEL__) && (defined (LINUX) || defined(UNIX))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <errno.h>
/* for the mutex type */
#include <sal/core/sync.h>
#include <shared/bsl.h>
#include <soc/ha.h>
#include <soc/ha_internal.h>
#include <soc/error.h>
#include <shared/alloc.h>

#define HA_ALIGN 8
#define HA_SHMEM_SIZE_MULTIPLE_OF(size, mult_unit) ((mult_unit) * (((size) + (mult_unit) - 1) / (mult_unit)))

ha_mgr_t *ha_mem[BCM_MAX_NUM_UNITS];

/*!
 * \brief
 *
 * set default values.
 *
 * \param [in] unit
 * \param [in] name - shared memory file name (should be /<name>)
 *
 * \retval SOC_E_NONE
 * \retval SOC_E_INTERNAL
 */
int ha_mem_mgr_init(int unit, const char *name)
{
    int rv = SOC_E_NONE;
    char *file_name = strrchr(name, '/');
    sal_strncpy(ha_mem[unit]->file_name, file_name ? file_name : name, HA_FILE_NAME_SIZE - 1);
    ha_mem[unit]->fh = -1;
    ha_mem[unit]->new_file = 1;
    ha_mem[unit]->mutex = sal_mutex_create("ha_mutex");
    if (ha_mem[unit]->mutex == NULL) {
        rv = SOC_E_INTERNAL;
    }
    ha_mem[unit]->initialized = 0;
    return rv;
}

/*!
 * \brief
 *
 * Free all resources associated with the ha module.
 *
 * \param [in] unit
 *
 * \retval SOC_E_NONE
 */
int ha_destroy(int unit)
{
    int rv = SOC_E_NONE;

    if (ha_mem[unit] != NULL)
    {
        sal_mutex_take(ha_mem[unit]->mutex, sal_mutex_FOREVER);
        if (ha_mem[unit]->mem_start) {
            munmap((void *)(ha_mem[unit]->mem_start), ha_mem[unit]->blk_len);
        }
        if (ha_mem[unit]->fh != -1) {
            close(ha_mem[unit]->fh);
        }
        sal_mutex_give(ha_mem[unit]->mutex);
        sal_mutex_destroy(ha_mem[unit]->mutex);
        sal_free(ha_mem[unit]);
        ha_mem[unit] = NULL;
    }

    return rv;
}

/*!
 * \brief ha_init initializes the shared memory.
 *
 * This function opens shared memory file and maps its memory to the memory
 * space of the process.
 *
 * \param [in] unit
 * \param [in] size - size of allocated memory
 * \param [in] name - shared memory file name (should be /<name>)
 * \param [in] create_new_file - indicates if to create new file or to opened existing file
 *
 * \retval SOC_E_NONE
 * \retval SOC_E_RESOURCE - failed to open shared memory file (typically the name was not of
 *         /file)
 * \retval SOC_E_MEMORY - memory mapping of the shared memory into the process memory space
 *         failed
 * \retval SOC_E_INTERNAL
 */
int ha_init(int unit, const char *name, int size, uint8 create_new_file)
{
    int rv = SOC_E_NONE;
    int file_size;
    void *mmap_ptr;

    /* ha_mem already initialized (allocated) */
    if (ha_mem[unit]) {
        return SOC_E_INTERNAL;
    }
    ha_mem[unit] = sal_alloc(sizeof (ha_mgr_t) , "ha memory");
    SOC_IF_ERROR_RETURN(ha_mem_mgr_init(unit, name));
    
    sal_mutex_take(ha_mem[unit]->mutex, sal_mutex_FOREVER);
    ha_mem[unit]->sys_page_size = sysconf(_SC_PAGESIZE);
    file_size = HA_SHMEM_SIZE_MULTIPLE_OF(size, ha_mem[unit]->sys_page_size);
    ha_mem[unit]->blk_len = file_size;

    if (SOC_E_NONE != ha_mgr_open_shared_mem(unit, create_new_file)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "open_shared_mem failed\n")));
        sal_mutex_give(ha_mem[unit]->mutex);
        return SOC_E_RESOURCE;
    }

    /* memory map the file */
    if (MAP_FAILED == (mmap_ptr = mmap(NULL, ha_mem[unit]->blk_len, 
                                       PROT_WRITE | PROT_READ, 
                                       MAP_SHARED, ha_mem[unit]->fh, 0))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to mmap errno=%d\n"), errno));
        close (ha_mem[unit]->fh);
        sal_mutex_give(ha_mem[unit]->mutex);
        return SOC_E_MEMORY;
    }
    ha_mem[unit]->mem_start = (ha_mem_blk_hdr_t *)mmap_ptr;
    ha_mem[unit]->mem_end = (ha_mem_blk_hdr_t *)((uint8 *)ha_mem[unit]->mem_start + ha_mem[unit]->blk_len);
    ha_mem[unit]->free_mem = ha_mem[unit]->mem_start;

    if (ha_mem[unit]->new_file) {
        ha_mem[unit]->mem_start->signature = HA_MEM_SIGNATURE;
        ha_mem[unit]->mem_start->length = ha_mem[unit]->blk_len;
        ha_mem[unit]->mem_start->state = (uint8)ha_blk_free;
    } else if (ha_mgr_sanity_check(unit) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "loaded file is corrupted\n")));
            return SOC_E_INTERNAL;
    }
    sal_mutex_give(ha_mem[unit]->mutex);
    ha_mem[unit]->initialized = 1;
    return rv; 
}

/*!
 * \brief sanity_check verifies the memory validity.
 *
 * This function runs after reopening an existing file. It goes through 
 * the entire memory to verify that every block has valid signature. 
 *
 * \retval SOC_E_NONE - no errors
 * \retval SOC_E_INTERNAL - the memory is corrupted
 */
/*
*/
int ha_mgr_sanity_check(int unit)
{
    int rv = SOC_E_NONE;

    ha_mem_blk_hdr_t *blk_hdr = ha_mem[unit]->mem_start;

    do {
        if (blk_hdr->signature != HA_MEM_SIGNATURE) {
            return SOC_E_INTERNAL;
        }
        blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    } while (blk_hdr < ha_mem[unit]->mem_end);

    return rv;
}

/*!
 * \brief find a block with matched block ID.
 *
 * Search the entire HA memory for an accoupied block with matching blk_id
 *
 * \param [in] unit
 * \param [in] blk_id is the block ID to search for
 * \param [out] block - pointer to the block if was found
 *                      NULL if the block was not found
 *
 * \retval SOC_E_NONE
 * \retval SOC_E_INTERNAL - the memory is corrupted
 */
int ha_mgr_find_block(int unit, uint16 blk_id, ha_mem_blk_hdr_t **block)
{
    int rv = SOC_E_NONE;
    ha_mem_blk_hdr_t *blk_hdr;
    *block = NULL;

    blk_hdr = ha_mem[unit]->mem_start;
    /* seach each block in this section */
    do {
        /* validate signature */
        if (blk_hdr->signature != HA_MEM_SIGNATURE) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("invalid Ha block signature\n")));
            return SOC_E_INTERNAL;
        }
        /* if the block is allocated and the block ID matches then we found it */
        if ((blk_hdr->state == (uint8_t)ha_blk_allocated) && 
            (blk_hdr->blk_id == blk_id)) {
            *block = blk_hdr;
            return SOC_E_NONE;
        }
        /* increment the block pointer to the next block */
        blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    } while (blk_hdr < ha_mem[unit]->mem_end); /* bailed out if block pointer points beyond the end */

    return rv;
}

/*!
 * \brief Open shared memory file
 *
 * Try to open existing file first. If file was not exist create a new shared
 * file. 
 *
 * \retval SOC_E_NONE
 * \retval SOC_E_INTERNAL
 */
int ha_mgr_open_shared_mem(int unit, uint8 create_new_file)
{
    int rv = SOC_E_NONE;
    struct stat stat_buf;

    if (create_new_file) {
        shm_unlink(ha_mem[unit]->file_name);
    }

    /* Try opening existing shared file */
    ha_mem[unit]->fh = shm_open(ha_mem[unit]->file_name, (int)O_RDWR | (create_new_file ? (int)O_TRUNC : 0), (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
    
    /* file exists and we are in cold boot mode */
    if (ha_mem[unit]->fh != -1 && create_new_file) {
        if (-1 == ftruncate(ha_mem[unit]->fh, ha_mem[unit]->blk_len)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
            close (ha_mem[unit]->fh);
            return SOC_E_INTERNAL;
        }
    /* file doesn't exist */
    } else if (-1 == ha_mem[unit]->fh) {
        LOG_VERBOSE(BSL_LS_SHARED_SWSTATE, (BSL_META("new file")));
        /* existing file is not there, create new file */
        ha_mem[unit]->fh = shm_open(ha_mem[unit]->file_name, (int)(O_CREAT | O_RDWR), 
                      (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
        if (-1 == ha_mem[unit]->fh) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to open shared file errno=%d\n"), errno));
            return SOC_E_INTERNAL;
        }
        /* when file created its size set to 0. Adjust it to the desired size */
        if (-1 == ftruncate(ha_mem[unit]->fh, ha_mem[unit]->blk_len)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
            close (ha_mem[unit]->fh);
            return SOC_E_INTERNAL;
        }
    }
    /* warm boot */
    else {   /* file opened successfully without the create flag */
        if (0 == fstat(ha_mem[unit]->fh, &stat_buf)) { /* get the current file size */
            LOG_VERBOSE(BSL_LS_SHARED_SWSTATE, (BSL_META("file size=%u\n"), (uint32_t)stat_buf.st_size));
            if ((uint32_t)stat_buf.st_size < ha_mem[unit]->blk_len) {
                /* increase the file size if too small */
                if (-1 == ftruncate(ha_mem[unit]->fh, ha_mem[unit]->blk_len)) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
                    close (ha_mem[unit]->fh);
                    return SOC_E_INTERNAL;
                }
            }
            else {
                /* update the blk_len according to the file size */
                ha_mem[unit]->blk_len = stat_buf.st_size;   
            }
        }
        assert(!create_new_file);
        ha_mem[unit]->new_file = 0;
        LOG_VERBOSE(BSL_LS_SHARED_SWSTATE, (BSL_META("file exist\n")));
    }
    return rv;
}

int ha_mem_is_already_alloc(int unit, unsigned char mod_id, unsigned char sub_id, uint8 *is_allocated)
{
    int rv = SOC_E_NONE;
    ha_mem_blk_hdr_t *block = NULL;
    uint16 blk_id = (mod_id << 8) | sub_id;
    *is_allocated = FALSE;
    if (ha_mgr_find_block(unit, blk_id, &block) != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }
    *is_allocated = (block != NULL);
    return rv;
}

/*!
 * \brief Allocate HA memory block for the application
 *
 * This function searches for a block with matches mod_id and sub_id.
 * If such a block was not found, the function allocates a new memory block
 * of the proper length and returns a pointer to the memory.
 *
 * \param [in] unit
 * \param [in] mod_id is the module ID
 * \param [in] sub_id is the sub module ID
 * \param [in/out] length is a pointer contains the desired length The
 *        length also contains the actual allocated memory length.
 * \parma [out] mem_ptr - pointer to the allocated block
 *                        NULL if the block was not allocated
 *
 * \retval SOC_E_NONE
 * \retval SOC_E_INTERNAL
 * \retval SOC_E_MEMORY
 */
int ha_mem_alloc(int unit, unsigned char mod_id, unsigned char sub_id, unsigned int *length, void **mem_ptr)
{
    int rv = SOC_E_NONE;
    /* blk_hdr points to the first allocated block */
    ha_mem_blk_hdr_t *blk_hdr;
    uint16 blk_id = (mod_id << 8) | sub_id;
    uint32 space_to_allocate;
    ha_mem_blk_hdr_t *new_blk_hdr = NULL;
    
    sal_mutex_take(ha_mem[unit]->mutex, sal_mutex_FOREVER);
    /* search for a block with match blk_id */
    if(ha_mgr_find_block(unit, blk_id, &blk_hdr) != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    if (!blk_hdr) {   /* buffer was not allocated before */
        /* make sure that the length is multiplication of 8 */
        space_to_allocate = HA_SHMEM_SIZE_MULTIPLE_OF(*length + sizeof(ha_mem_blk_hdr_t), HA_ALIGN);
        blk_hdr = ha_mem[unit]->free_mem;
        /* check if there is enough space for the current allocation + next header */
        if(blk_hdr->length < space_to_allocate + sizeof(ha_mem_blk_hdr_t)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "not enough space for allocation\n")));
            return SOC_E_MEMORY;
        }

        new_blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + space_to_allocate);
        new_blk_hdr->length = blk_hdr->length - space_to_allocate;
        new_blk_hdr->signature = HA_MEM_SIGNATURE;
        new_blk_hdr->state = (uint8_t)ha_blk_free;
        blk_hdr->length = space_to_allocate;

        blk_hdr->blk_id = blk_id;
        blk_hdr->state = (uint8_t)ha_blk_allocated;
        ha_mem[unit]->free_mem = new_blk_hdr;
    } else {
        *length = ((blk_hdr->length - sizeof(ha_mem_blk_hdr_t))/HA_ALIGN)*HA_ALIGN;
    }

    sal_mutex_give(ha_mem[unit]->mutex);
    *mem_ptr = (uint8 *)blk_hdr + sizeof(ha_mem_blk_hdr_t);
    
    return rv;
}

#endif /* __KERNEL__ */

