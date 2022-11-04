

#ifndef __KAPS_JR1_BITMAP_H
#define __KAPS_JR1_BITMAP_H

#include "kaps_jr1_allocator.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_utility.h"

#ifdef __cplusplus
extern "C"
{
#endif





    enum kaps_jr1_bit_type
    {
        KAPS_JR1_ZERO = 0,              
        KAPS_JR1_ONE = 1,               
        KAPS_JR1_DONT_CARE = 2          
    };



    struct kaps_jr1_fast_bitmap
    {
        uint32_t *bitmap;      
        uint32_t *summary_bitmap;
                               
        uint8_t *counts;       
        int32_t num_access_bits;
                              
    };

    int kaps_jr1_get_bit(
    struct kaps_jr1_fast_bitmap *fbmp,
    int pos);

    void kaps_jr1_set_bit(
    struct kaps_jr1_fast_bitmap *fbmp,
    int pos);

    void kaps_jr1_reset_bit(
    struct kaps_jr1_fast_bitmap *fbmp,
    int pos);

    int kaps_jr1_find_first_bit_set(
    struct kaps_jr1_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);

    int kaps_jr1_find_last_bit_set(
    struct kaps_jr1_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);


    uint32_t kaps_jr1_fbmp_get_first_n_bits_set(
    struct kaps_jr1_fast_bitmap *fbmp,
    int32_t n_bits,
    uint32_t * result_bit_pos);

    int kaps_jr1_count_bits(
    struct kaps_jr1_fast_bitmap *fbmp,
    int start_bit_pos,
    int end_bit_pos);

    int kaps_jr1_find_nth_bit(
    struct kaps_jr1_fast_bitmap *fbmp,
    int n,
    int start_bit_pos,
    int end_bit_pos);

    kaps_jr1_status kaps_jr1_init_bmp(
    struct kaps_jr1_fast_bitmap *fbmp,
    struct kaps_jr1_allocator *alloc,
    int size,
    int are_bits_set);

    kaps_jr1_status kaps_jr1_update_bmp(
    struct kaps_jr1_fast_bitmap *fbmp,
    uint16_t * num_ab_slots,
    struct kaps_jr1_allocator *alloc,
    uint32_t num_ab,
    uint32_t per_ab_slots,
    uint32_t insertion_point,
    uint32_t are_bits_set);

    void kaps_jr1_free_bmp(
    struct kaps_jr1_fast_bitmap *fbmp,
    struct kaps_jr1_allocator *alloc);



    static KAPS_JR1_INLINE enum kaps_jr1_bit_type kaps_jr1_array_get_bit(
    const uint8_t * data,
    const uint8_t * mask,
    uint32_t bitno)
    {
        uint32_t byte_index = bitno / KAPS_JR1_BITS_IN_BYTE;
        uint32_t bit_index = 7 - (bitno - (byte_index * KAPS_JR1_BITS_IN_BYTE));

        if  (
    mask[byte_index] & (1 << bit_index))
                return KAPS_JR1_DONT_CARE;
        if  (
    data[byte_index] & (1 << bit_index))
                return KAPS_JR1_ONE;
            return KAPS_JR1_ZERO;
    }


    uint32_t kaps_jr1_is_subset(
    const uint8_t * set_data,
    const uint8_t * set_mask,
    const uint8_t * subset_data,
    const uint8_t * subset_mask,
    uint32_t min_pos,
    uint32_t max_pos);


    static KAPS_JR1_INLINE int kaps_jr1_array_check_bit(
    const uint8_t * array,
    uint32_t bitno)
    {
        uint32_t byte_index, bit_index;

        kaps_jr1_sassert(array != NULL);

        byte_index = bitno / KAPS_JR1_BITS_IN_BYTE;
        if (array[byte_index] == 0)
            return 0;

        bit_index = 7 - (bitno - (byte_index * KAPS_JR1_BITS_IN_BYTE));

        if (array[byte_index] & (1 << bit_index))
            return 1;
        else
            return 0;
    }



    static KAPS_JR1_INLINE void kaps_jr1_array_set_bit(
    uint8_t * array,
    uint32_t bitno,
    uint8_t value)
    {
        uint32_t byte_index = bitno / KAPS_JR1_BITS_IN_BYTE;
        uint32_t bit_index = 7 - (bitno - (byte_index * KAPS_JR1_BITS_IN_BYTE));

        if (value == 1)
            array[byte_index] |= (1 << bit_index);
        else
            array[byte_index] &= ~(1 << bit_index);
    }



    static KAPS_JR1_INLINE int32_t kaps_jr1_array_get_first_bit_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        uint32_t byte_index;
        int8_t bit_index;

        for (byte_index = 0; byte_index < bitmap_size_in_bytes; byte_index++)
        {
            if (((value == 0) && (array[byte_index] != 0xFF)) || ((value == 1) && (array[byte_index] != 0)))
            {
                break;
            }
        }

        if (byte_index == bitmap_size_in_bytes)
        {
            return bitmap_size_in_bits;
        }

        for (bit_index = KAPS_JR1_BITS_IN_BYTE - 1; bit_index >= 0; bit_index--)
        {
            if (((array[byte_index] >> bit_index) & 1) == value)
            {
                return (byte_index * KAPS_JR1_BITS_IN_BYTE + 7 - bit_index);
            }
        }
        return bitmap_size_in_bits;
    }


    static KAPS_JR1_INLINE int32_t kaps_jr1_array_get_first_n_bits_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t n_bits,
    int32_t n_bit_align,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        uint32_t byte_index;
        int8_t bit_index;
        int32_t found_num_bit_count;
        int32_t bit_align_count = 0;

        found_num_bit_count = 0;
        for (byte_index = 0; byte_index < bitmap_size_in_bytes; byte_index++)
        {
            if (((value == 0) && (array[byte_index] == 0xFF)) || ((value == 1) && (array[byte_index] == 0)))
            {
                found_num_bit_count = 0;
                continue;
            }

            bit_align_count = 0;
            if (n_bit_align > 1)
                found_num_bit_count = 0;
            for (bit_index = KAPS_JR1_BITS_IN_BYTE - 1; bit_index >= 0; bit_index--)
            {
                if (n_bit_align > 1)
                {
                    if (bit_align_count == n_bit_align)
                    {
                        found_num_bit_count = 0;
                        bit_align_count = 0;
                    }
                    else
                    {
                        kaps_jr1_sassert(bit_align_count < n_bit_align);
                    }
                }
                if (((array[byte_index] >> bit_index) & 1) == value)
                {
                    found_num_bit_count++;
                    if (found_num_bit_count == n_bits)
                        return (byte_index * KAPS_JR1_BITS_IN_BYTE + 7 - bit_index) - (n_bits - 1);
                }
                else
                {
                    found_num_bit_count = 0;
                }

                bit_align_count++;
            }
        }

        return bitmap_size_in_bits;
    }



    static KAPS_JR1_INLINE int32_t kaps_jr1_array_get_last_bit_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        int32_t byte_index;
        int8_t bit_index;

        for (byte_index = bitmap_size_in_bytes - 1; byte_index >= 0; byte_index--)
        {
            if (((value == 0) && (array[byte_index] != 0xFF)) || ((value == 1) && (array[byte_index] != 0)))
            {
                break;
            }
        }

        if (byte_index < 0)
        {
            return bitmap_size_in_bits;
        }

        for (bit_index = 0; bit_index < KAPS_JR1_BITS_IN_BYTE; bit_index++)
        {
            if (((array[byte_index] >> bit_index) & 1) == value)
            {
                return (byte_index * KAPS_JR1_BITS_IN_BYTE + 7 - bit_index);
            }
        }
        return bitmap_size_in_bits;
    }


    static KAPS_JR1_INLINE int32_t kaps_jr1_array_get_last_n_bits_with_value(
    uint8_t * array,
    uint8_t value,
    int32_t n_bits,
    int32_t n_bit_align,
    int32_t bitmap_size_in_bits)
    {
        int32_t bitmap_size_in_bytes = bitmap_size_in_bits >> 3;
        int32_t byte_index;
        int8_t bit_index;
        int32_t found_num_bit_count;
        int32_t bit_align_count = 0;

        found_num_bit_count = 0;
        for (byte_index = bitmap_size_in_bytes - 1; byte_index >= 0; byte_index--)
        {
            if (((value == 0) && (array[byte_index] == 0xFF)) || ((value == 1) && (array[byte_index] == 0)))
            {
                found_num_bit_count = 0;
                continue;
            }

            bit_align_count = 0;
            for (bit_index = 0; bit_index < KAPS_JR1_BITS_IN_BYTE; bit_index++)
            {
                if (n_bit_align > 1)
                {
                    if (bit_align_count == n_bit_align)
                    {
                        found_num_bit_count = 0;
                        bit_align_count = 0;
                    }
                    else
                    {
                        kaps_jr1_sassert(bit_align_count < n_bit_align);
                    }
                }
                if (((array[byte_index] >> bit_index) & 1) == value)
                {
                    found_num_bit_count++;
                    if (found_num_bit_count == n_bits)
                        return (byte_index * KAPS_JR1_BITS_IN_BYTE + 7 - bit_index);
                }
                else
                {
                    found_num_bit_count = 0;
                }
                bit_align_count++;
            }
        }

        return bitmap_size_in_bits;
    }



#ifdef __cplusplus
}
#endif
#endif
