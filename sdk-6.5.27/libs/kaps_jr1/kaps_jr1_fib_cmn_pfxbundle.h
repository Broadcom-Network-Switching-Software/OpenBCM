



#ifndef INCLUDED_KAPS_JR1_PFXBUNDLE_H
#define INCLUDED_KAPS_JR1_PFXBUNDLE_H

#include <kaps_jr1_fib_cmn_prefix.h>
#include "kaps_jr1_utility.h"

#include <kaps_jr1_externcstart.h>

#define KAPS_JR1_LSN_NEW_INDEX                ((uint32_t)0xFFFFFFFF)

#define KAPS_JR1_STRICT_LMPSOFAR_INDEX        ((uint32_t) 0x1FFFFFFF)


typedef struct kaps_jr1_pfx_bundle
{
    struct kaps_jr1_lpm_entry *m_backPtr;
    uint32_t m_nIndex;
#ifdef KAPS_JR1_LPM_DEBUG
    uint32_t m_nSeqNum:27;
#endif
    uint32_t reconcile_used:1;   
    uint32_t m_status:1;        
    uint32_t m_isLmpsofarPfx:1;
    uint32_t m_isPfxCopy:1;     
    uint32_t m_isAdditionalIxPresent:1;
    uint32_t m_nPfxSize:8;
    uint8_t m_data[2];          
} kaps_jr1_pfx_bundle;

extern kaps_jr1_pfx_bundle *kaps_jr1_pfx_bundle_create(
    kaps_jr1_nlm_allocator *alloc,       
    kaps_jr1_prefix * prefix,       
    uint32_t ix,                
    uint32_t assocSize,         
    uint32_t seqNum);           

extern void kaps_jr1_pfx_bundle_destroy(
    kaps_jr1_pfx_bundle *self,
    kaps_jr1_nlm_allocator *alloc);
    

extern kaps_jr1_pfx_bundle *kaps_jr1_pfx_bundle_create_from_string(
    kaps_jr1_nlm_allocator *,       
    const uint8_t * prefix,     
    uint32_t numbits,           
    uint32_t ix,                
    uint32_t assocSize,         
    uint8_t addExtraFirstByte); 

kaps_jr1_pfx_bundle *kaps_jr1_pfx_bundle_create_from_pfx_bundle(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_pfx_bundle * oldPfxBundle,
    uint32_t assocSize,
    uint8_t copyAssoc);


extern int kaps_jr1_pfx_bundle_compare(
    const kaps_jr1_pfx_bundle * self,
    const kaps_jr1_pfx_bundle * other);

extern void kaps_jr1_pfx_bundle_print(
    kaps_jr1_pfx_bundle * self,
    FILE * fp);

#define kaps_jr1_pfx_bundle_assert_valid(self)       \
    NlmCmPrefix__AssertTrailingBits(            \
        KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(self),       \
    KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(self),       \
    8*KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES((self)->m_nPfxSize))


#define KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(x)   (((uint32_t)(x) + 7) >> 3)
#define KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES2(x)  ((KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(x) + 1) & ~(uint32_t)1)


#define KAPS_JR1_PFX_BUNDLE_GET_INDEX(self)      (self)->m_nIndex


#define KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(SELF)   (kaps_jr1_assert_((SELF)==(SELF), "side-effect") \
                                             ((uint8_t *)(SELF)->m_data + KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES2((SELF)->m_nPfxSize)))


#define KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(self)        (self)->m_nPfxSize


#define KAPS_JR1_PFX_BUNDLE_GET_PFX_DATA(self)        ((uint8_t *)(self)->m_data)

extern void kaps_jr1_pfx_bundle_set_bit(
    kaps_jr1_pfx_bundle * self,
    uint32_t ix,
    int32_t bit);               

#include <kaps_jr1_externcend.h>

#endif
