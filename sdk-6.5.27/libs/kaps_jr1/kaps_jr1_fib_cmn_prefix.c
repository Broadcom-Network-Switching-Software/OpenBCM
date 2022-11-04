

#include <kaps_jr1_fib_cmn_prefix.h>
#include "kaps_jr1_utility.h"

#define CONSTCASTINUSE(self) (*(uint16_t*)(&self->m_inuse))
#define MAX_SIZE_IN_BITS    0xffffU
#define ASSIGN_AS_U16(y, x) kaps_jr1_sassert (x <= MAX_SIZE_IN_BITS) ; y = (uint16_t) x

kaps_jr1_prefix *kaps_jr1_prefix_pvt_ctor(
    kaps_jr1_prefix * self,
    uint16_t initialAvail,
    uint16_t dataLength,
    const uint8_t * initialData);

int32_t
kaps_jr1_prefix_check_trailing_bits(
    const uint8_t * data,
    uint32_t pos,
    uint32_t end)
{
    if (pos > end)
    {
        kaps_jr1_sassert(0);        
        return 0;
    }

    for (; pos < end; pos++)
    {
        if ((pos & 7) == 0)
        {
            for (; pos < end; pos += 8)
            {
                if (data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] != 0)
                {
                    kaps_jr1_sassert(data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] == 0);
                    return 0;
                }
            }
        }
        else
        {
            if (0 != (data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] & KAPS_JR1_PREFIX_GET_BIT_MASK(pos)))
            {
                kaps_jr1_sassert(0 == (data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] & KAPS_JR1_PREFIX_GET_BIT_MASK(pos)));
                return 0;
            }
        }
    }
    return 1;
}







kaps_jr1_prefix *
kaps_jr1_prefix_create(
    kaps_jr1_nlm_allocator * allocator,
    unsigned maxBitLength,
    unsigned initialDataBitLength,
    const uint8_t * initialData)
{
    size_t prefixSize;
    uint16_t initialAvail;
    uint16_t dataLength;
    kaps_jr1_prefix *self;
    uint32_t avail32;
    uint16_t avail;

    
    ASSIGN_AS_U16(initialAvail, maxBitLength);
    ASSIGN_AS_U16(dataLength, initialDataBitLength);

    
    avail32 = initialAvail;
    avail32 = (avail32 + 7) & (~0x7);
    avail = avail32;
    initialAvail = avail;

    

    prefixSize = KAPS_JR1_PREFIX_GET_STORAGE_SIZE(avail);

    self = (kaps_jr1_prefix *) kaps_jr1_nlm_allocator_calloc(allocator, 1, prefixSize);

    if (self)
        kaps_jr1_prefix_pvt_ctor(self, initialAvail, dataLength, initialData);
    return (self);

}

void
kaps_jr1_prefix_destroy(
    kaps_jr1_prefix * self,
    kaps_jr1_nlm_allocator * alloc)
{
    if (self)
    {

        kaps_jr1_nlm_allocator_free(alloc, self);
    }
}

void
kaps_jr1_prefix_append(
    kaps_jr1_prefix * self,
    unsigned dataLength_u,      
    uint8_t * data)
{       
    uint16_t byteCounter;       
    uint16_t bitCounter;        
    uint32_t currentByte;
    uint16_t dataLength;
    uint32_t pos;
    uint32_t newLength = self->m_inuse + dataLength_u;

    NlmCmPrefix__AssertValid(self);

    kaps_jr1_sassert(self->m_avail >= newLength);

    
    ASSIGN_AS_U16(dataLength, dataLength_u);

    
    byteCounter = 0;
    bitCounter = 0;
    currentByte = data[byteCounter];

    pos = self->m_inuse;
    CONSTCASTINUSE(self) = newLength;

    while (bitCounter < dataLength)
    {

        
        if (currentByte & 0x80)
            self->m_data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] |= KAPS_JR1_PREFIX_GET_BIT_MASK(pos);
        else
            kaps_jr1_sassert(0 == (self->m_data[KAPS_JR1_PREFIX_GET_BIT_BYTE(pos)] & KAPS_JR1_PREFIX_GET_BIT_MASK(pos)));

        pos++;
        currentByte <<= 1;
        bitCounter++;
        if (((bitCounter % 8) == 0) && (bitCounter < dataLength))
        {
            byteCounter++;
            currentByte = data[byteCounter];
        }
    }

    NlmCmPrefix__AssertValid(self);
}



int32_t
kaps_jr1_prefix_pvt_is_more_specific_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len)
{
    uint32_t nBytes, nBits;

    NlmCmPrefix__AssertTrailingBits(pfx1data, pfx1len, pfx1len);
    NlmCmPrefix__AssertTrailingBits(pfx2data, pfx2len, pfx2len);

    if (pfx1len < pfx2len)
        return 0;

    nBytes = pfx2len / 8;
    nBits = pfx2len % 8;
    if (0 != kaps_jr1_memcmp(pfx1data, pfx2data, nBytes))
        return 0;

    if (nBits)
    {
        uint32_t ch1 = pfx1data[nBytes];
        uint32_t ch2 = pfx2data[nBytes];
        uint32_t ix;
        if (ch1 == ch2)
            return 1;
        else
        {
            for (ix = 0; ix < nBits; ix++)
                if ((ch1 & KAPS_JR1_PREFIX_GET_BIT_MASK(ix)) != (ch2 & KAPS_JR1_PREFIX_GET_BIT_MASK(ix)))
                    return 0;
        }
    }
    return 1;
}

int32_t
kaps_jr1_prefix_pvt_is_equal(
    const uint8_t * pfx1data,
    uint32_t pfx1len,
    const uint8_t * pfx2data,
    uint32_t pfx2len)
{
    uint32_t nBytes, nBits;

    if (pfx1len != pfx2len)
        return 0;

    nBytes = pfx2len / 8;
    nBits = pfx2len % 8;
    if (0 != kaps_jr1_memcmp(pfx1data, pfx2data, nBytes))
        return 0;

    if (nBits)
    {
        uint32_t ch1 = pfx1data[nBytes];
        uint32_t ch2 = pfx2data[nBytes];
        uint32_t ix;
        if (ch1 == ch2)
            return 1;
        else
        {
            for (ix = 0; ix < nBits; ix++)
                if ((ch1 & KAPS_JR1_PREFIX_GET_BIT_MASK(ix)) != (ch2 & KAPS_JR1_PREFIX_GET_BIT_MASK(ix)))
                    return 0;
        }
    }
    return 1;
}

void
kaps_jr1_prefix_print_as_ip(
    kaps_jr1_prefix * self,
    FILE * fp)
{
    uint32_t bit = 0;
    uint32_t thisbyte;
    uint32_t i;
    uint32_t first = 1;

    while (bit < self->m_inuse)
    {
        i = 0;
        thisbyte = 0;
        while (i < 8 && bit < self->m_inuse)
        {
            thisbyte |= KAPS_JR1_PREFIX_GET_BIT(self, bit) << (7 - i);
            i++;
            bit++;
        }

        if (!first)
            kaps_jr1_fprintf(fp, ".");
        kaps_jr1_fprintf(fp, "%d", thisbyte);

        first = 0;
    }

    kaps_jr1_fprintf(fp, "/%d", self->m_inuse);
}





kaps_jr1_prefix *
kaps_jr1_prefix_pvt_ctor(
    kaps_jr1_prefix * self,         
    uint16_t initialAvail,      
    uint16_t dataLength,        
    const uint8_t * initialData)
{       
    uint8_t *data = self->m_data;

    initialAvail = (uint16_t) ((initialAvail + 7) & ~0x7);

    kaps_jr1_sassert(dataLength <= initialAvail);
    kaps_jr1_sassert(dataLength == 0 || initialData);

    self->m_avail = initialAvail;
    CONSTCASTINUSE(self) = dataLength;
    if (dataLength)
    {
        uint32_t n = KAPS_JR1_PREFIX_GET_NUM_BYTES(dataLength);
        uint32_t m = KAPS_JR1_PREFIX_GET_NUM_BYTES(initialAvail);

        kaps_jr1_memcpy(data, initialData, n);
        if (dataLength & 7)
        {
            uint32_t mask = (KAPS_JR1_PREFIX_GET_BIT_MASK(dataLength - 1) - 1);
            data[n - 1] &= ~mask;
        }

        if (m > n)
            kaps_jr1_memset(data + n, 0, m - n);

        NlmCmPrefix__AssertValid(self);
    }
    else
        kaps_jr1_memset(data, 0, KAPS_JR1_PREFIX_GET_NUM_BYTES(initialAvail));

    return (self);
}


