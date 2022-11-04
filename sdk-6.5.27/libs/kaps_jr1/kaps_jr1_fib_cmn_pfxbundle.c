

#include <kaps_jr1_fib_cmn_pfxbundle.h>
#include <kaps_jr1_math.h>
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_ad_internal.h"
#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_fib_lsnmc.h"


#define KAPS_JR1CMROUND2_(x)     (((uint32_t)x+1)&~(uint32_t)1)
#define KAPS_JR1CMBUNDLESIZE(s1,s2)  (KAPS_JR1CMROUND2_(s1) + KAPS_JR1CMROUND2_(s2) - sizeof(uint8_t) * 2 + sizeof(kaps_jr1_pfx_bundle))

kaps_jr1_pfx_bundle *
kaps_jr1_pfx_bundle_create(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_prefix * prefix,
    uint32_t ix,
    uint32_t assocSize,
    uint32_t seqNum)
{
    uint32_t tmp = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(prefix->m_inuse);
    uint32_t sz = KAPS_JR1CMBUNDLESIZE(tmp, assocSize);
    kaps_jr1_pfx_bundle *ret = (kaps_jr1_pfx_bundle *) kaps_jr1_nlm_allocator_calloc(alloc, 1, sz);

    
    ret->m_nIndex = ix;
    ret->m_nPfxSize = prefix->m_inuse;
#ifdef KAPS_JR1_LPM_DEBUG
    ret->m_nSeqNum = seqNum;
#endif
    kaps_jr1_memcpy(ret->m_data, prefix->m_data, tmp);
    return ret;
}

kaps_jr1_pfx_bundle *
kaps_jr1_pfx_bundle_create_from_string(
    kaps_jr1_nlm_allocator * alloc,
    const uint8_t * prefix,
    uint32_t numbits,
    uint32_t ix,
    uint32_t assocSize,
    uint8_t addExtraFirstByte)
{
    uint32_t tmp = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(numbits);
    uint32_t sz;
    kaps_jr1_pfx_bundle *ret;

    if (addExtraFirstByte)
        sz = KAPS_JR1CMBUNDLESIZE(tmp + KAPS_JR1_BITS_IN_BYTE, assocSize);
    else
        sz = KAPS_JR1CMBUNDLESIZE(tmp, assocSize);

    ret = (kaps_jr1_pfx_bundle *) kaps_jr1_nlm_allocator_calloc(alloc, 1, sz);

    
    ret->m_nIndex = ix;

    ret->m_nPfxSize = (uint16_t) numbits;
    if (addExtraFirstByte)
        ret->m_nPfxSize += KAPS_JR1_BITS_IN_BYTE;

    if (prefix)
    {
        uint32_t startByte = 0;
        uint32_t endByte = tmp - 1;

        if (addExtraFirstByte)
        {
            startByte = 1;
            endByte = tmp;
        }

        kaps_jr1_memcpy(&ret->m_data[startByte], prefix, tmp);
        if (numbits & 7)
        {
            uint32_t mask = (KAPS_JR1_PREFIX_GET_BIT_MASK(numbits - 1) - 1);
            ret->m_data[endByte] &= ~mask;
        }

    }

    kaps_jr1_pfx_bundle_assert_valid(ret);

    return ret;
}


void kaps_jr1_pfx_bundle_destroy(
    kaps_jr1_pfx_bundle *self,
    kaps_jr1_nlm_allocator *alloc)
{
    if (self->m_isAdditionalIxPresent) {
        struct kaps_jr1_ix_chunk *cur_ix_chunk;
        void * tmp_ptr;
        struct kaps_jr1_ix_mgr *ix_mgr;

        tmp_ptr = KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(self);
        kaps_jr1_memcpy(&cur_ix_chunk, tmp_ptr, sizeof(struct kaps_jr1_ix_chunk*));

        ix_mgr = kaps_jr1_ix_mgr_get_mgr_for_lmpsofar_pfx(
                                cur_ix_chunk->ad_info->ad_db);
        
        kaps_jr1_ix_mgr_free(ix_mgr, cur_ix_chunk);
    }
    
    kaps_jr1_nlm_allocator_free(alloc, self);
}


kaps_jr1_pfx_bundle *
kaps_jr1_pfx_bundle_create_from_pfx_bundle(
    kaps_jr1_nlm_allocator * alloc,
    kaps_jr1_pfx_bundle * oldPfxBundle,
    uint32_t assocSize,
    uint8_t copyAssoc)
{
    uint32_t tmp = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(oldPfxBundle->m_nPfxSize);
    uint32_t sz = KAPS_JR1CMBUNDLESIZE(tmp, assocSize);
    kaps_jr1_pfx_bundle *ret = (kaps_jr1_pfx_bundle *) kaps_jr1_nlm_allocator_calloc(alloc, 1, sz);

    ret->m_backPtr = oldPfxBundle->m_backPtr;
    ret->m_isLmpsofarPfx = oldPfxBundle->m_isLmpsofarPfx;
    ret->m_isPfxCopy = oldPfxBundle->m_isPfxCopy;
    ret->m_nIndex = oldPfxBundle->m_nIndex;
    ret->m_nPfxSize = oldPfxBundle->m_nPfxSize;

#ifdef KAPS_JR1_LPM_DEBUG
    ret->m_nSeqNum = oldPfxBundle->m_nSeqNum;
#endif

    ret->m_status = oldPfxBundle->m_status;

    kaps_jr1_memcpy(ret->m_data, oldPfxBundle->m_data, tmp);

    if (copyAssoc)
    {
        kaps_jr1_memcpy(KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(ret), KAPS_JR1_PFX_BUNDLE_GET_ASSOC_PTR(oldPfxBundle), assocSize);
    }

    kaps_jr1_pfx_bundle_assert_valid(ret);
    return ret;
}

void
kaps_jr1_pfx_bundle_set_bit(
    kaps_jr1_pfx_bundle * self,
    uint32_t ix,
    int32_t bit)
{
    if (ix < self->m_nPfxSize)
    {
        if (bit)
            self->m_data[ix >> 3] |= (0x80 >> (ix & 7));
        else
            self->m_data[ix >> 3] &= ~(0x80 >> (ix & 7));
    }
    else
        kaps_jr1_sassert(0);
}

int
kaps_jr1_pfx_bundle_compare(
    const kaps_jr1_pfx_bundle * prefix1,
    const kaps_jr1_pfx_bundle * prefix2)
{
    
    uint32_t numBytes;
    int ret;

    kaps_jr1_pfx_bundle_assert_valid(prefix1);
    kaps_jr1_pfx_bundle_assert_valid(prefix2);

    numBytes = KAPS_JR1_MIN(prefix1->m_nPfxSize, prefix2->m_nPfxSize);
    numBytes = KAPS_JR1_PFX_BUNDLE_GET_NUM_PFX_BYTES(numBytes);

    
    ret = kaps_jr1_memcmp(prefix1->m_data, prefix2->m_data, numBytes);

    if (!ret)
    {
        ret = (int) (prefix1->m_nPfxSize - prefix2->m_nPfxSize);
    }

    return ret;
}

void
kaps_jr1_pfx_bundle_print(
    kaps_jr1_pfx_bundle * self,
    FILE * fp)
{
    uint32_t byteCounter;       
    uint32_t bitCounter;        
    uint32_t currentByte;
    uint32_t sz = KAPS_JR1_PFX_BUNDLE_GET_PFX_SIZE(self);

    
    byteCounter = 0;
    bitCounter = 0;
    currentByte = self->m_data[0];

    while (bitCounter < sz)
    {

        if (currentByte & 0x80)
            kaps_jr1_fprintf(fp, "1");
        else
            kaps_jr1_fprintf(fp, "0");

        currentByte <<= 1;
        bitCounter++;
        if ((bitCounter % 8) == 0)
        {
            byteCounter++;
            currentByte = self->m_data[byteCounter];
        }
    }
    kaps_jr1_fprintf(fp, "\n");
}
