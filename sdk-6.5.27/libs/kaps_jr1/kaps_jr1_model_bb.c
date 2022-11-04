


#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_allocator.h"
#include "kaps_jr1_model_internal.h"
#include "kaps_jr1_model_bb.h"



static kaps_jr1_status get_pfxs_in_bb(
    struct kaps_jr1_c_model_bb *bb,
    uint32_t bb_type_bit,
    uint8_t * bb_data, 
    struct kaps_jr1_c_model_bb_pfx *pfxs_in_bb, 
    uint16_t gran)
{
    uint32_t cur_source_bit = 0;
    uint32_t cur_target_bit = 0;
    int32_t num_bits_left = gran;
    uint32_t num_bits_to_process = 0;
    uint32_t value = 0, i = 0;
    uint32_t num_pfx_in_bb;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint16_t bit_pos = 0, num_unused_bits = 0;
    uint8_t bit_value = 0;
    
    if (gran == 8) {
        num_pfx_in_bb = 16;
    } else {
        num_pfx_in_bb = KAPS_JR1_BKT_WIDTH_1 / (gran + KAPS_JR1_DEFAULT_AD_WIDTH_1);
    }

    cur_source_bit = KAPS_JR1_BKT_WIDTH_1 - 1;

    for (i = 0; i < num_pfx_in_bb; ++i) {
        num_bits_left = gran;
        cur_target_bit = KAPS_JR1_BB_KEY_WIDTH_1 - 1;

        while (num_bits_left > 0) {
            num_bits_to_process = num_bits_left;

            if (num_bits_to_process > 32)
                num_bits_to_process = 32;

            value = KapsJr1ReadBitsInArrray(bb_data, KAPS_JR1_BKT_WIDTH_8,
                                     cur_source_bit, (cur_source_bit + 1) - num_bits_to_process);

            KapsJr1WriteBitsInArray(pfxs_in_bb[i].pfx_data, KAPS_JR1_BB_KEY_WIDTH_8,
                             cur_target_bit, (cur_target_bit + 1) - num_bits_to_process, value);

            num_bits_left -= num_bits_to_process;

            cur_source_bit -= num_bits_to_process;
            cur_target_bit -= num_bits_to_process;
        }

    }

    
    for (i = 0; i < num_pfx_in_bb; ++i) {
        pfxs_in_bb[i].num_bits_to_compare = KAPS_JR1_BKT_WIDTH_1 + 1;

        num_unused_bits = 8 - (gran % 8);

        num_unused_bits = num_unused_bits % 8;

        
        for (bit_pos = 0; bit_pos < gran; ++bit_pos) {
            bit_value = (uint8_t) KapsJr1ReadBitsInArrray(pfxs_in_bb[i].pfx_data, (gran + 7) / 8,
                                                 num_unused_bits + bit_pos, num_unused_bits + bit_pos);
            if (bit_value != 0) {
                pfxs_in_bb[i].num_bits_to_compare = gran - (bit_pos + 1);
                break;
            }
        }
    }

    return status;
}




kaps_jr1_status kaps_jr1_c_model_bb_create(
    struct kaps_jr1_c_model *m, 
    uint8_t abs_bkt_nr,
    uint32_t is_small_bb,
    struct kaps_jr1_c_model_bb ** bb_pp)
{
    struct kaps_jr1_c_model_bb *self_p = NULL;
    struct kaps_jr1_allocator *alloc = m->alloc;
    uint32_t num_bb_in_one_bb_cascade = m->config.num_bb_in_one_bb_cascade;

    if (num_bb_in_one_bb_cascade == 0)
        return KAPS_JR1_INVALID_ARGUMENT;

    *bb_pp = NULL;

    self_p = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_c_model_bb));
    if (!self_p)
        return KAPS_JR1_OUT_OF_MEMORY;

    self_p->m = m;
    self_p->abs_bkt_nr = abs_bkt_nr;
    self_p->bkt_nr_in_bb_cascade = abs_bkt_nr % num_bb_in_one_bb_cascade;
    self_p->lpu_nr = abs_bkt_nr / num_bb_in_one_bb_cascade ;
    self_p->bb_num_rows = m->config.num_rows_in_bb;


    if (m->config.initialize_with_invalid_bb_search_intf)
    {
        self_p->search_interface_num = KAPS_JR1_INVALID_SEARCH_INTERFACE;
    }

    *bb_pp = self_p;

    return KAPS_JR1_OK;

}



kaps_jr1_status kaps_jr1_c_model_bb_destroy(struct kaps_jr1_c_model_bb * bb)
{
    struct kaps_jr1_allocator *alloc ;

    if (bb != NULL) {
        alloc = bb->m->alloc;
        alloc->xfree(alloc->cookie, bb);
    }

    return KAPS_JR1_OK;
}



kaps_jr1_status kaps_jr1_c_model_bb_write(struct kaps_jr1_c_model_bb * bb, uint32_t row_nr, uint8_t * data)
{
    kaps_jr1_memcpy(bb->rows[row_nr].entry, data, KAPS_JR1_BKT_WIDTH_8);

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_bb_global_config_write(struct kaps_jr1_c_model_bb * bb, uint8_t * data)
{
    kaps_jr1_memcpy(bb->bb_global_config_reg, data, KAPS_JR1_REGISTER_WIDTH_8);

    return KAPS_JR1_OK;
}

kaps_jr1_status kaps_jr1_c_model_bb_read(
    struct kaps_jr1_c_model_bb * bb, 
    uint32_t row_nr, 
    uint8_t * data)
{
    kaps_jr1_memcpy(data, bb->rows[row_nr].entry, KAPS_JR1_BKT_WIDTH_8);

    return KAPS_JR1_OK;
}

kaps_jr1_status kaps_jr1_c_model_bb_compare (
    struct kaps_jr1_c_model_bb * bb,
    uint32_t search_interface,
    uint8_t * bb_key, 
    uint16_t row_nr, 
    uint16_t gran,
    uint16_t bb_type_bit,
    struct kaps_jr1_c_model_bb_result * bb_result)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t bit_pos = 0, start_bit_pos;
    uint16_t num_pfx_to_compare = 0, pfx_iter = 0;
    int32_t is_mismatch = 0, found_match = 0;
    uint32_t match_index = 0, match_len = 0, match_ad = 0;
    struct kaps_jr1_c_model_bb_pfx pfxs_in_bb[KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE];
    uint8_t *bb_data;

    (void) bb_type_bit;

    if (row_nr >= bb->bb_num_rows)
        kaps_jr1_assert(0, "Invalid BB Row Number while searching\n");

    kaps_jr1_memset(bb_result, 0, sizeof(struct kaps_jr1_c_model_bb_result));

    
    if (gran == 0)
        return KAPS_JR1_OK;

    bb_data = bb->rows[row_nr].entry;

    kaps_jr1_memset(pfxs_in_bb, 0, sizeof(struct kaps_jr1_c_model_bb_pfx) * KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE);

    
    get_pfxs_in_bb(bb, bb_type_bit, bb_data, pfxs_in_bb, gran);

    if (gran == 8) {
        num_pfx_to_compare = 16;
    } else {
        num_pfx_to_compare = KAPS_JR1_BKT_WIDTH_1 / (gran + KAPS_JR1_DEFAULT_AD_WIDTH_1);
    }


    for (pfx_iter = 0; pfx_iter < num_pfx_to_compare; ++pfx_iter) {
        uint16_t num_bytes_to_compare = pfxs_in_bb[pfx_iter].num_bits_to_compare / 8;
        uint16_t bit_iter = 0;

        
        if (found_match && (match_len > pfxs_in_bb[pfx_iter].num_bits_to_compare)) {
            continue;
        }

        
        if (pfxs_in_bb[pfx_iter].num_bits_to_compare > KAPS_JR1_BKT_WIDTH_1) {
            continue;
        }

        if (num_bytes_to_compare > 0) {
            
            if (kaps_jr1_memcmp(bb_key, pfxs_in_bb[pfx_iter].pfx_data, num_bytes_to_compare))
                continue;
        }

        is_mismatch = 0;
        for (bit_iter = 0; bit_iter < pfxs_in_bb[pfx_iter].num_bits_to_compare % 8; ++bit_iter) {
            bit_pos = 7 - bit_iter;

            if ((bb_key[num_bytes_to_compare] & (1u << bit_pos)) !=
                (pfxs_in_bb[pfx_iter].pfx_data[num_bytes_to_compare] & (1u << bit_pos))) {
                
                is_mismatch = 1;
                break;
            }
        }

        
        if (is_mismatch)
            continue;

        
        found_match = 1;
        match_index = pfx_iter;
        match_len = (uint8_t) (pfxs_in_bb[pfx_iter].num_bits_to_compare);


        start_bit_pos = match_index  * KAPS_JR1_DEFAULT_AD_WIDTH_1;
        match_ad = KapsJr1ReadBitsInArrray(bb_data, KAPS_JR1_BKT_WIDTH_8,
                                    start_bit_pos + KAPS_JR1_DEFAULT_AD_WIDTH_1 - 1, start_bit_pos) ;
        
    }


    bb_result->is_hit = found_match;

    if (bb_result->is_hit) {
        bb_result->bb_nr = bb->abs_bkt_nr;
        bb_result->row_nr = row_nr;
        bb_result->hit_index = match_index;
        bb_result->match_len = match_len;
        bb_result->match_ad = match_ad;
    }

    return status;
}


kaps_jr1_status kaps_jr1_c_model_bb_print(struct kaps_jr1_c_model_bb *bb)
{
    uint16_t bit_num = 0, byte_num = 0, bit_pos = 0;
    uint16_t row_nr = 0;
    uint8_t *bb_data;

    FILE *fp = kaps_jr1_fopen("BB.txt", "w");

    if (!fp)
        return KAPS_JR1_INVALID_FILE;

    for (row_nr = 0; row_nr < bb->m->config.num_rows_in_bb; row_nr++) {
        kaps_jr1_fprintf(fp, "Row Nr = %d \n", row_nr);

        bb_data = bb->rows[row_nr].entry;
        for (bit_num = 0; bit_num < KAPS_JR1_BKT_WIDTH_1; ++bit_num) {
            byte_num = bit_num / 8;

            bit_pos = 7 - (bit_num % 8);

            if (bb_data[byte_num] & (0x1 << bit_pos)) {
                kaps_jr1_fprintf(fp, "1");
            } else {
                kaps_jr1_fprintf(fp, "0");
            }
        }

        kaps_jr1_fprintf(fp, "\n\n");
    }

    kaps_jr1_fclose(fp);

    return KAPS_JR1_OK;
}


void kaps_jr1_c_model_bb_debug(
    struct kaps_jr1_c_model_bb *bb, 
    uint32_t bb_type_bit, 
    uint32_t row_nr, 
    uint32_t gran)
{

    struct kaps_jr1_c_model_bb_pfx pfxs_in_bb[KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE];
    uint8_t *bb_data;

    bb_data = bb->rows[row_nr].entry;

    kaps_jr1_memset(pfxs_in_bb, 0, sizeof(struct kaps_jr1_c_model_bb_pfx) * KAPS_JR1_MAX_PFX_PER_BKT_ARRAY_SIZE);

    
    get_pfxs_in_bb(bb, bb_type_bit, bb_data, pfxs_in_bb, gran);
}

