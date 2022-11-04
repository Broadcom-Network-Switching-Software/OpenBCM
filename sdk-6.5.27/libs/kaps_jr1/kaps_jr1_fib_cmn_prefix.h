



#ifndef INCLUDED_KAPS_JR1_PREFIX_H
#define INCLUDED_KAPS_JR1_PREFIX_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_legacy.h"
#include "kaps_jr1_internal_errors.h"
#include "kaps_jr1_math.h"
#include <kaps_jr1_externcstart.h>  


typedef struct kaps_jr1_prefix
{

    uint16_t m_avail;           

    uint16_t m_inuse;           

    uint8_t m_data[4];          

} kaps_jr1_prefix;


#define     KAPS_JR1_PREFIX_GET_BIT_BYTE(n)      ((n) >> 3)


#define     KAPS_JR1_PREFIX_GET_BIT_MASK(n)      (0x80 >> ((n) & 7))


#define     KAPS_JR1_PREFIX_GET_NUM_BYTES(numBits)   (((numBits) + 7) >> 3)

extern kaps_jr1_prefix *kaps_jr1_prefix_create(
    kaps_jr1_nlm_allocator * allocator,
    unsigned maxBitLength,
    unsigned initialDataBitLength,
    const uint8_t * initialData);

extern void kaps_jr1_prefix_destroy(
    kaps_jr1_prefix * self,
    kaps_jr1_nlm_allocator * alloc);

extern void kaps_jr1_prefix_append(
    kaps_jr1_prefix * self,
    unsigned dataLength,
    uint8_t * data);

#define KAPS_JR1_PREFIX_PVT_GET_BIT_RAW(data,len,IX) (uint8_t)(data[(IX)>>3] & (0x80>>((IX)&7)))

#define KAPS_JR1_PREFIX_PVT_GET_BIT(data, len, location) (uint8_t)(KAPS_JR1_PREFIX_PVT_GET_BIT_RAW(data, len, location) != 0)

#define KAPS_JR1_PREFIX_GET_BIT(self,ix)        (KAPS_JR1_PREFIX_PVT_GET_BIT(self->m_data, self->m_inuse, ix))

#define KAPS_JR1_PREFIX_SET_BIT(self,ix,v)      { uint32_t __x = (ix) ; uint8_t __m = \
                          (uint8_t)KAPS_JR1_PREFIX_GET_BIT_MASK(__x), \
                          *ch = (self)->m_data+KAPS_JR1_PREFIX_GET_BIT_BYTE(__x) ;\
                          *ch = (uint8_t)((v) ? (*ch | __m) : (*ch & ~__m)) ; }

#define kaps_jr1_prefix_is_more_specific(prefix1, prefix2) ((prefix1->m_inuse <= prefix2->m_inuse ? 0 : \
    kaps_jr1_prefix_pvt_is_more_specific_equal((prefix1)->m_data, \
    (prefix1)->m_inuse, (prefix2)->m_data, (prefix2)->m_inuse)))

#define kaps_jr1_prefix_is_more_specific_equal(prefix1, prefix2) ((prefix1->m_inuse < prefix2->m_inuse) ? 0 : \
    kaps_jr1_prefix_pvt_is_more_specific_equal((prefix1)->m_data, \
    (prefix1)->m_inuse, (prefix2)->m_data, (prefix2)->m_inuse))

#define kaps_jr1_prefix_pvt_is_more_specific(pfx1data, pfx1len, pfx2data, pfx2len) ( (pfx1len <= pfx2len) ? 0 : \
    kaps_jr1_prefix_pvt_is_more_specific_equal(pfx1data, pfx1len, pfx2data, pfx2len))


int32_t kaps_jr1_prefix_pvt_is_more_specific_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len);

int32_t kaps_jr1_prefix_pvt_is_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len);

#define NlmCmPrefix__AssertTrailingBits(data, inuse, avail) \
    kaps_jr1_sassert(kaps_jr1_prefix_check_trailing_bits(data, inuse, avail))

extern int32_t kaps_jr1_prefix_check_trailing_bits(
    const uint8_t * pfxdata,
    uint32_t inuse_bits,
    uint32_t avail_bits);

#define NlmCmPrefix__AssertValid(self) \
    kaps_jr1_sassert(kaps_jr1_prefix_check_trailing_bits(self->m_data, self->m_inuse, self->m_avail))

#define NlmCmPrefix__IsValid(self)  \
    kaps_jr1_prefix_check_trailing_bits(self->m_data, self->m_inuse, self->m_avail)


extern void kaps_jr1_prefix_print_as_ip(
    kaps_jr1_prefix * self,
    FILE * fp);

#define KAPS_JR1_PREFIX_GET_STORAGE_SIZE(nBits) (KAPS_JR1_MAX((sizeof(kaps_jr1_prefix) + KAPS_JR1_PREFIX_GET_NUM_BYTES(nBits) - sizeof(((kaps_jr1_prefix*)0)->m_data)), \
                                                    (sizeof(kaps_jr1_prefix))))

#include <kaps_jr1_externcend.h>

#endif


