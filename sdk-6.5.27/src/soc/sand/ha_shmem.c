

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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

#include <sal/core/sync.h>
#include <shared/bsl.h>
#include <soc/sand/ha.h>
#include <soc/dnx/ha_internal.h>
#include <soc/error.h>
#include <shared/alloc.h>

#define HA_ALIGN 8
#define HA_SHMEM_SIZE_MULTIPLE_OF(size, mult_unit) ((mult_unit) * (((size) + (mult_unit) - 1) / (mult_unit)))

ha_mgr_t *ha_mem[BCM_MAX_NUM_UNITS];


int ha_mem_mgr_init(int unit, const char *name)
{
    int rv = SOC_E_NONE;
    char *file_name = strrchr(name, '/');
    sal_strncpy(ha_mem[unit]->file_name, file_name ? file_name : name, HA_FILE_NAME_SIZE - 1);
    ha_mem[unit]->file_handle = -1;
    ha_mem[unit]->new_file = 1;
    ha_mem[unit]->mutex = sal_mutex_create("ha_mutex");
    if (ha_mem[unit]->mutex == NULL) {
        rv = SOC_E_INTERNAL;
    }
    ha_mem[unit]->initialized = 0;
    return rv;
}


int ha_destroy(int unit)
{
    int rv = SOC_E_NONE;

    if (ha_mem[unit] != NULL)
    {
        sal_mutex_take(ha_mem[unit]->mutex, sal_mutex_FOREVER);
        if (ha_mem[unit]->mem_start) {
            munmap((void *)(ha_mem[unit]->mem_start), ha_mem[unit]->blk_len);
        }
        if (ha_mem[unit]->file_handle != -1) {
            close(ha_mem[unit]->file_handle);
        }
        sal_mutex_give(ha_mem[unit]->mutex);
        sal_mutex_destroy(ha_mem[unit]->mutex);
        sal_free(ha_mem[unit]);
        ha_mem[unit] = NULL;
    }

    return rv;
}


int ha_init(int unit, const char *name, int size, uint8 create_new_file)
{
    int rv = SOC_E_NONE;
    int file_size;
    void *mmap_ptr;

    
    if (ha_mem[unit]) {
        return SOC_E_INTERNAL;
    }
    ha_mem[unit] = sal_alloc2(sizeof (ha_mgr_t), SAL_ALLOC_F_ZERO, "ha memory");
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

    
    if (MAP_FAILED == (mmap_ptr = mmap(NULL, ha_mem[unit]->blk_len, 
                                       PROT_WRITE | PROT_READ, 
                                       MAP_SHARED, ha_mem[unit]->file_handle, 0))) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to mmap errno=%d\n"), errno));
        close (ha_mem[unit]->file_handle);
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


int ha_mgr_find_block(int unit, uint16 blk_id, ha_mem_blk_hdr_t **block)
{
    int rv = SOC_E_NONE;
    ha_mem_blk_hdr_t *blk_hdr;
    *block = NULL;

    blk_hdr = ha_mem[unit]->mem_start;
    
    do {
        
        if (blk_hdr->signature != HA_MEM_SIGNATURE) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("invalid Ha block signature\n")));
            return SOC_E_INTERNAL;
        }
        
        if ((blk_hdr->state == (uint8_t)ha_blk_allocated) && 
            (blk_hdr->blk_id == blk_id)) {
            *block = blk_hdr;
            return SOC_E_NONE;
        }
        
        blk_hdr = (ha_mem_blk_hdr_t *)((uint8_t *)blk_hdr + blk_hdr->length);
    } while (blk_hdr < ha_mem[unit]->mem_end); 

    return rv;
}


int ha_mgr_open_shared_mem(int unit, uint8 create_new_file)
{
    int rv = SOC_E_NONE;
    struct stat stat_buf;

    if (create_new_file) {
        shm_unlink(ha_mem[unit]->file_name);
    }

    
    ha_mem[unit]->file_handle = shm_open(ha_mem[unit]->file_name, (int)O_RDWR | (create_new_file ? (int)O_TRUNC : 0), (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
    
    
    if (ha_mem[unit]->file_handle != -1 && create_new_file) {
        if (-1 == ftruncate(ha_mem[unit]->file_handle, ha_mem[unit]->blk_len)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
            close (ha_mem[unit]->file_handle);
            return SOC_E_INTERNAL;
        }
    
    } else if (-1 == ha_mem[unit]->file_handle) {
        LOG_VERBOSE(BSL_LS_SHARED_SWSTATE, (BSL_META("new file")));
        
        ha_mem[unit]->file_handle = shm_open(ha_mem[unit]->file_name, (int)(O_CREAT | O_RDWR), 
                      (mode_t)(S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP));
        if (-1 == ha_mem[unit]->file_handle) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to open shared file errno=%d\n"), errno));
            return SOC_E_INTERNAL;
        }
        
        if (-1 == ftruncate(ha_mem[unit]->file_handle, ha_mem[unit]->blk_len)) {
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
            close (ha_mem[unit]->file_handle);
            return SOC_E_INTERNAL;
        }
    }
    
    else {   
        if (0 == fstat(ha_mem[unit]->file_handle, &stat_buf)) { 
            LOG_VERBOSE(BSL_LS_SHARED_SWSTATE, (BSL_META("file size=%u\n"), (uint32_t)stat_buf.st_size));
            if ((uint32_t)stat_buf.st_size < ha_mem[unit]->blk_len) {
                
                if (-1 == ftruncate(ha_mem[unit]->file_handle, ha_mem[unit]->blk_len)) {
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META("Failed to trancate the file errno=%d\n"), errno));
                    close (ha_mem[unit]->file_handle);
                    return SOC_E_INTERNAL;
                }
            }
            else {
                
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


int ha_mem_alloc(int unit, unsigned char mod_id, unsigned char sub_id, unsigned int *length, void **mem_ptr)
{
    int rv = SOC_E_NONE;
    
    ha_mem_blk_hdr_t *blk_hdr;
    uint16 blk_id = (mod_id << 8) | sub_id;
    uint32 space_to_allocate;
    ha_mem_blk_hdr_t *new_blk_hdr = NULL;
    
    sal_mutex_take(ha_mem[unit]->mutex, sal_mutex_FOREVER);
    
    if(ha_mgr_find_block(unit, blk_id, &blk_hdr) != SOC_E_NONE) {
        return SOC_E_INTERNAL;
    }

    if (!blk_hdr) {   
        
        space_to_allocate = HA_SHMEM_SIZE_MULTIPLE_OF(*length + sizeof(ha_mem_blk_hdr_t), HA_ALIGN);
        blk_hdr = ha_mem[unit]->free_mem;
        
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

#endif 

