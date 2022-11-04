

#ifndef __KAPS_JR1_PFX_HASH_TABLE_H
#define __KAPS_JR1_PFX_HASH_TABLE_H

#include "kaps_jr1_errors.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_allocator.h"
#include "kaps_jr1_db_internal.h"
#include "kaps_jr1_device.h"



#ifdef __cplusplus
extern "C"
{
#endif


#define HASH_TABLE_NULL_VALUE ( (struct kaps_jr1_entry *)0)


#define HASH_TABLE_INVALID_VALUE ( (struct kaps_jr1_entry *)1)


    struct pfx_hash_table
    {
        struct kaps_jr1_entry **m_slots_p;  
        struct kaps_jr1_allocator *m_alloc_p;
                                        
        struct kaps_jr1_db *m_db;           
        uint32_t m_nCurSize;           
        uint32_t m_nMaxSize;           
        uint32_t m_nThreshSize;        
        uint16_t m_nThreshold;         
        uint16_t m_nResizeBy;          
        uint32_t m_nInvalidSlots;      
        uint32_t m_nMaxPfxWidth_1;     
        uint32_t m_doffset;            
    };


    kaps_jr1_status kaps_jr1_pfx_hash_table_create(
    struct kaps_jr1_allocator *alloc,
    uint32_t initsz,
    uint32_t thresh,
    uint32_t resizeby,
    uint32_t max_pfx_width_1,
    uint32_t d_offset,
    struct kaps_jr1_db *db,
    struct pfx_hash_table **self_pp);

    kaps_jr1_status kaps_jr1_pfx_hash_table_destroy(
    struct pfx_hash_table *self);


    kaps_jr1_status kaps_jr1_pfx_hash_table_insert(
    struct pfx_hash_table *self,
    struct kaps_jr1_entry *e);


    kaps_jr1_status kaps_jr1_pfx_hash_table_locate(
    struct pfx_hash_table *self,
    uint8_t * data,
    uint32_t len_1,
    struct kaps_jr1_entry ***ret);


    kaps_jr1_status kaps_jr1_pfx_hash_table_delete(
    struct pfx_hash_table *self,
    struct kaps_jr1_entry **slot);


    kaps_jr1_status kaps_jr1_pfx_hash_table_expand(
    struct pfx_hash_table *self);


    kaps_jr1_status kaps_jr1_pfx_hash_table_reconstruct(
    struct pfx_hash_table *self);

#ifdef __cplusplus
}
#endif

#endif
