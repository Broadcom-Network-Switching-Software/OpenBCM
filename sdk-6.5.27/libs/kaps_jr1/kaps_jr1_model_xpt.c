

#include "kaps_jr1_portable.h"
#include "kaps_jr1_model_xpt.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_model_internal.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_utility.h"

#include "kaps_jr1_default_allocator.h"



static kaps_jr1_status kaps_jr1_c_model_search(void *xpt, uint8_t *rpb_key, enum kaps_jr1_search_interface search_interface, struct kaps_jr1_search_result *kaps_jr1_result);


static void kaps_jr1_c_model_get_first_half_of_fmap(uint8_t *bytes, uint32_t nbytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads);

static void kaps_jr1_c_model_get_second_half_of_fmap(uint8_t *bytes, uint32_t nbytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads);



kaps_jr1_status
kaps_jr1_c_model_initialize_rpbs(
    struct kaps_jr1_c_model *m,
    uint32_t rpb_blk_nr,
    kaps_jr1_tcam_log_file_entry *tcam_entries_list,
    uint32_t num_elements)
{
    uint32_t i, j, row_nr, byte_nr;
    char *raw_tcam_string;
    uint8_t raw_tcam_bytes[20];
    uint8_t final_tcam_data[20];
    uint8_t tcam_byte;


    for (i = 0; i < num_elements; ++i)
    {
        row_nr = tcam_entries_list[i].row_nr;

        raw_tcam_string = tcam_entries_list[i].tcam_entry_str;

        byte_nr = 0;
        for (j = 0; j < 2 * KAPS_JR1_RPB_WIDTH_8; j += 2)
        {
            raw_tcam_bytes[byte_nr] = (kaps_jr1_hex_to_int(raw_tcam_string[j]) * 16)
                                    + kaps_jr1_hex_to_int(raw_tcam_string[j+1]);
            byte_nr++;
        }

        for (j = 0; j < KAPS_JR1_RPB_WIDTH_8; ++j)
        {
            if (j == KAPS_JR1_RPB_WIDTH_8 - 1)
                tcam_byte = (raw_tcam_bytes[j] << 2);
            else
                tcam_byte = (raw_tcam_bytes[j] << 2) | (raw_tcam_bytes[j+1] >> 6);

            final_tcam_data[j] = tcam_byte;
        }

        kaps_jr1_c_model_rpb_write(m->rpb_blocks[rpb_blk_nr], row_nr, final_tcam_data, 0x3);
    }

    return KAPS_JR1_OK;
}




static uint8_t kaps_jr1_c_model_get_format_value(struct kaps_jr1_c_model *m, struct kaps_jr1_ads *ads_data, uint32_t lpu_nr)
{
    uint8_t format_value = 0;

    if (m->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
        if (m->sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM0 || m->sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0) {
            format_value = ads_data->format_map_00;
            return format_value;
        } else {
            if (lpu_nr <= 3) {
                format_value = ads_data->format_map_00;
            } else if (4 <= lpu_nr && lpu_nr <= 7) {
                format_value = ads_data->format_map_01;
            } else if (8 <= lpu_nr && lpu_nr <= 11) {
                format_value = ads_data->format_map_02;
            } else {
                format_value = ads_data->format_map_03;
            }
            return format_value;
        }
    }

    switch (lpu_nr) {
    case 0:
        format_value = ads_data->format_map_00;
        break;

    case 1:
        format_value = ads_data->format_map_01;
        break;

    case 2:
        format_value = ads_data->format_map_02;
        break;

    case 3:
        format_value = ads_data->format_map_03;
        break;

    case 4:
        format_value = ads_data->format_map_04;
        break;

    case 5:
        format_value = ads_data->format_map_05;
        break;

    case 6:
        format_value = ads_data->format_map_06;
        break;

    case 7:
        format_value = ads_data->format_map_07;
        break;

    case 8:
        format_value = ads_data->format_map_08;
        break;

    case 9:
        format_value = ads_data->format_map_09;
        break;

    case 10:
        format_value = ads_data->format_map_10;
        break;

    case 11:
        format_value = ads_data->format_map_11;
        break;

    case 12:
        format_value = ads_data->format_map_12;
        break;

    case 13:
        format_value = ads_data->format_map_13;
        break;

    case 14:
        format_value = ads_data->format_map_14;
        break;

    case 15:
        format_value = ads_data->format_map_15;
        break;

    default:
        kaps_jr1_sassert(0);
        break;
    }

    return format_value;
}


static void kaps_jr1_c_model_increment_search_tag(struct kaps_jr1_c_model *m)
{
    m->search_tag++;
    
    if (m->search_tag >= KAPS_JR1_MAX_SEARCH_TAG)
        m->search_tag = 0;
}

static kaps_jr1_status kaps_jr1_c_model_log_rpb_info(struct kaps_jr1_c_model *m, uint32_t blk_nr, uint32_t addr_in_blk, uint8_t *info, uint8_t is_valid)
{
    uint32_t i;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "WRITE ");
        kaps_jr1_fprintf(m->log_file, "TCAM:");
        kaps_jr1_fprintf(m->log_file, "0x%x, ", blk_nr);
        kaps_jr1_fprintf(m->log_file, "Row:0x%04x, ", addr_in_blk);

        kaps_jr1_fprintf(m->log_file, "Data:0x");
        for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", info[i]);
        }

        

        if (is_valid)
            kaps_jr1_fprintf(m->log_file, ", Valid:0x3\n");
        else
            kaps_jr1_fprintf(m->log_file, ", Valid:0x0\n");

    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_log_ads_info(struct kaps_jr1_c_model *m, uint32_t blk_nr, uint32_t addr_in_blk,
                               uint8_t *bytes, uint32_t nbytes)
{
    int32_t i;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "WRITE ");

        if (m->config.is_small_bb_present) {
            kaps_jr1_fprintf(m->log_file, "ADS1:");
        } else {
            kaps_jr1_fprintf(m->log_file, "ADS:");
        }

        kaps_jr1_fprintf(m->log_file, "0x%x, ", blk_nr);
        kaps_jr1_fprintf(m->log_file, "Row:");
        kaps_jr1_fprintf(m->log_file, "0x%04x, ", addr_in_blk);

        kaps_jr1_fprintf(m->log_file, "Data:0x");
        for (i = 0; i < nbytes; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", bytes[i]);
        }

        kaps_jr1_fprintf(m->log_file, "\n");
    }

    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_log_small_bkt_block_info(struct kaps_jr1_c_model *m, uint32_t bkt_nr, uint32_t row_nr, uint8_t *data)
{
    uint32_t i;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "WRITE ");

        kaps_jr1_fprintf(m->log_file, "SMALL-BB:0x%x, ", bkt_nr);
        kaps_jr1_fprintf(m->log_file, "Row:0x%04x, ", row_nr);

        kaps_jr1_fprintf(m->log_file, "Data:0x");
        for (i = 0; i < KAPS_JR1_BKT_WIDTH_8; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", data[i]);
        }
        kaps_jr1_fprintf(m->log_file, "\n");
    }

    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_log_bkt_block_info(struct kaps_jr1_c_model *m, uint32_t bkt_nr, uint32_t row_nr, uint8_t *data)
{
    uint32_t i;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "WRITE ");

        if (m->config.is_small_bb_present) {
            kaps_jr1_fprintf(m->log_file, "LARGE-BB:0x%x, ", bkt_nr);
        } else {
            kaps_jr1_fprintf(m->log_file, "BB:0x%x, ", bkt_nr);
        }

        kaps_jr1_fprintf(m->log_file, "Row:0x%04x, ", row_nr);

        kaps_jr1_fprintf(m->log_file, "Data:0x");
        for (i = 0; i < KAPS_JR1_BKT_WIDTH_8; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", data[i]);
        }
        kaps_jr1_fprintf(m->log_file, "\n");
    }

    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_log_bb_global_config(struct kaps_jr1_c_model *m, uint32_t bkt_nr, uint8_t *data)
{
    uint32_t i;

    if (m->log_file) {
        kaps_jr1_fprintf(m->log_file, "WRITE ");

        if (m->config.is_small_bb_present) {
            kaps_jr1_fprintf(m->log_file, "Small-BB-GlobalConfig:0x%x, ", bkt_nr);
        } else {
            kaps_jr1_fprintf(m->log_file, "BB-GlobalConfig:0x%x, ", bkt_nr);
        }

        kaps_jr1_fprintf(m->log_file, "Data:0x");
        for (i = 0; i < KAPS_JR1_REGISTER_WIDTH_8; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", data[i]);
        }
        kaps_jr1_fprintf(m->log_file, "\n");
    }

    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_log_search_command(struct kaps_jr1_c_model *m, uint8_t *rpb_key,
                                     enum kaps_jr1_search_interface search_interface)
{
    uint32_t i;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "COMPARE ");

        kaps_jr1_fprintf(m->log_file, "TCAM:0x%x, ", search_interface);

        kaps_jr1_fprintf(m->log_file, "Key:0x");
        for (i = 0; i < KAPS_JR1_LPM_KEY_MAX_WIDTH_8; ++i) {
            kaps_jr1_fprintf(m->log_file, "%02x", rpb_key[i]);
        }
        kaps_jr1_fprintf(m->log_file, ", ");
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status kaps_jr1_c_model_log_search_result(struct kaps_jr1_c_model *m, struct kaps_jr1_search_result *kaps_jr1_result)
{
    uint32_t i;
    uint32_t ad_width_8 = (kaps_jr1_result->res_ad_width_1 + 7) / 8;
    uint32_t num_bits_in_last_byte;

    if (m->log_file && m->config.xpt_version == KAPS_JR1_MODEL_XPT_VERSION_1) {
        kaps_jr1_fprintf(m->log_file, "Result:0x");

        for (i = 0; i < ad_width_8 - 1; ++i)
        {
            kaps_jr1_fprintf(m->log_file, "%02x", kaps_jr1_result->ad_value[i]);
        }

        num_bits_in_last_byte = kaps_jr1_result->res_ad_width_1 % 8;

        if (num_bits_in_last_byte == 0)
            num_bits_in_last_byte = 8;

        if (num_bits_in_last_byte <= 4)
        {
            kaps_jr1_fprintf(m->log_file, "%01x, ", kaps_jr1_result->ad_value[i] >> 4);
        }
        else
        {
            kaps_jr1_fprintf(m->log_file, "%02x, ", kaps_jr1_result->ad_value[i]);
        }

        kaps_jr1_fprintf(m->log_file, "MatchLen:0x%02x", kaps_jr1_result->match_len);
        kaps_jr1_fprintf(m->log_file, "\n");
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_get_bb_key(uint8_t *input_key, uint8_t * output_key, uint16_t lopoff_1)
{
    uint32_t cur_source_bit = (KAPS_JR1_LPM_KEY_MAX_WIDTH_1 - 1) - lopoff_1;
    uint32_t cur_target_bit = KAPS_JR1_BB_KEY_WIDTH_1 - 1;
    int32_t num_bits_left = cur_source_bit + 1;
    uint32_t num_bits_to_process = 0;
    uint32_t value = 0;
    kaps_jr1_status status = KAPS_JR1_OK;

    kaps_jr1_memset(output_key, 0, KAPS_JR1_BB_KEY_WIDTH_8);

    while (num_bits_left > 0) {
        num_bits_to_process = num_bits_left;

        if (num_bits_to_process > 32)
            num_bits_to_process = 32;

        value = KapsJr1ReadBitsInArrray(input_key, KAPS_JR1_LPM_KEY_MAX_WIDTH_8, cur_source_bit, (cur_source_bit + 1) - num_bits_to_process);

        KapsJr1WriteBitsInArray(output_key, KAPS_JR1_BB_KEY_WIDTH_8,
                         cur_target_bit, (cur_target_bit + 1) - num_bits_to_process, value);

        num_bits_left -= num_bits_to_process;

        cur_source_bit -= num_bits_to_process;
        cur_target_bit -= num_bits_to_process;
    }

    return status;
}

static kaps_jr1_status kaps_jr1_c_model_write_rpb(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *key, uint8_t is_valid)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (blk_nr >= m->config.total_num_rpb) {
        kaps_jr1_assert(0, "Invalid RPB Number \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    
    if (row_nr >= 2 * m->config.num_rows_in_rpb[blk_nr]) {
        kaps_jr1_assert(0, "Invalid RPB Row \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    kaps_jr1_c_model_log_rpb_info(m, blk_nr, row_nr, key, is_valid);

    KAPS_JR1_STRY(kaps_jr1_c_model_rpb_write(m->rpb_blocks[blk_nr], row_nr, key, is_valid));

    m->stats.num_rpb_writes++;

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_write_ads(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes, uint32_t nbytes, struct kaps_jr1_ads *ads_data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (blk_nr >= m->config.total_num_ads) {
        kaps_jr1_assert(0, "Invalid ADS Number \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }


    if (row_nr >= m->config.num_rows_in_rpb[blk_nr]) {
        kaps_jr1_assert(0, "Invalid ADS Row \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    kaps_jr1_c_model_log_ads_info(m, blk_nr, row_nr, bytes, nbytes);

    KAPS_JR1_STRY(kaps_jr1_c_model_ads_write(m->ads_blocks[blk_nr], row_nr, ads_data));

    m->stats.num_ads_writes++;

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_write_bkt(
    void *xpt, 
    uint32_t bkt_nr, 
    uint32_t row_nr, 
    uint32_t is_small_bb,
    kaps_jr1_mdb_lpm_db_config_e bb_stage, 
    uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint32_t num_rows_in_bb;

    if (is_small_bb) 
    {
        if (bkt_nr >= m->config.total_num_small_bb) {
            kaps_jr1_assert(0, "Invalid Small BB Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        if (row_nr >= m->config.num_rows_in_small_bb) {
            kaps_jr1_assert(0, "Invalid Small BB Row Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        kaps_jr1_c_model_log_small_bkt_block_info(m, bkt_nr, row_nr, data);

        KAPS_JR1_STRY(kaps_jr1_c_model_bb_write(m->small_bbs[bkt_nr], row_nr, data));

        m->stats.num_small_bb_writes++;

    } 
    else {

        if (bkt_nr >= m->config.total_num_bb) {
            kaps_jr1_assert(0, "Invalid Bucket Block Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        num_rows_in_bb = m->bucket_blocks[bkt_nr]->bb_num_rows;

        if (row_nr >= num_rows_in_bb) {
            kaps_jr1_assert(0, "Invalid Bucket Block Row \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        kaps_jr1_c_model_log_bkt_block_info(m, bkt_nr, row_nr, data);

        KAPS_JR1_STRY(kaps_jr1_c_model_bb_write(m->bucket_blocks[bkt_nr], row_nr, data));

        m->stats.num_bb_writes++;
    }


    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_write_hb(void *xpt, uint32_t block_nr, uint32_t row_nr, uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t hb_block_nr;
    uint32_t large_bb_nr, num_rows_in_large_bb;

    kaps_jr1_sassert(m->is_hb_supported);

    if (block_nr >= KAPS_JR1_RPB_BLOCK_START && block_nr <= end_rpb_nr) {
        uint32_t num_rows_in_rpb = m->config.num_rows_in_rpb[block_nr - KAPS_JR1_RPB_BLOCK_START];
        uint32_t num_physical_rpb_hb_rows =  num_rows_in_rpb / KAPS_JR1_HB_ROW_WIDTH_1;

        kaps_jr1_sassert(m->id != KAPS_JR1_QUMRAN_DEVICE_ID); 

        if (row_nr >= num_physical_rpb_hb_rows) {
            kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        hb_block_nr = block_nr - KAPS_JR1_RPB_BLOCK_START;
        KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->rpb_hb_blocks[hb_block_nr], row_nr, data));


    } else {
        if (m->config.is_small_bb_present) {
            if (block_nr >= m->config.small_bb_start && block_nr <= m->config.small_bb_end) {

                if (row_nr >= m->config.num_rows_in_small_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                hb_block_nr = block_nr - m->config.small_bb_start;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->small_bb_hb_blocks[hb_block_nr], row_nr, data));

            } else if (block_nr >= m->config.large_bb_start && block_nr <= m->config.large_bb_end) {
                large_bb_nr = block_nr - m->config.large_bb_start;
                large_bb_nr = 2 * large_bb_nr;

                num_rows_in_large_bb = m->bucket_blocks[large_bb_nr]->bb_num_rows;
                if (row_nr >= num_rows_in_large_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                if (m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A000_B960
                    || m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A960_B000
                    || m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A480_B480) {

                    hb_block_nr = large_bb_nr;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->bb_hb_blocks[hb_block_nr], row_nr, data));

                    hb_block_nr = large_bb_nr + 1;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->bb_hb_blocks[hb_block_nr], row_nr, &data[KAPS_JR1_HB_ROW_WIDTH_8]));

                } else {
                    hb_block_nr = large_bb_nr;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->bb_hb_blocks[hb_block_nr], row_nr, data));
                }

            } else {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        } else {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (block_nr >= start_bb_nr && block_nr <= end_bb_nr) {
                if (row_nr >= m->config.num_rows_in_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                hb_block_nr = block_nr - start_bb_nr;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->bb_hb_blocks[hb_block_nr], row_nr, data));

            } else {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        }
    }

    m->stats.num_hb_writes++;

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_set_hb(void *xpt, uint32_t block_nr, uint32_t row_nr, uint32_t bit_num)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t hb_block_nr;

    kaps_jr1_sassert(m->is_hb_supported);

    if (block_nr >= KAPS_JR1_RPB_BLOCK_START && block_nr <= end_rpb_nr) {
        uint32_t num_rows_in_rpb = m->config.num_rows_in_rpb[block_nr - KAPS_JR1_RPB_BLOCK_START];
        uint32_t num_physical_rpb_hb_rows =  num_rows_in_rpb / KAPS_JR1_HB_ROW_WIDTH_1;

        kaps_jr1_sassert(m->id != KAPS_JR1_QUMRAN_DEVICE_ID); 

        if (row_nr >= num_physical_rpb_hb_rows) {
            kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        hb_block_nr = block_nr - KAPS_JR1_RPB_BLOCK_START;
        KAPS_JR1_STRY(kaps_jr1_c_model_hb_set(m->rpb_hb_blocks[hb_block_nr], row_nr, bit_num));

    }  
    else 
    {
        {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (block_nr >= start_bb_nr && block_nr <= end_bb_nr) {
                if (row_nr >= m->config.num_rows_in_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                hb_block_nr = block_nr - start_bb_nr;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_set(m->bb_hb_blocks[hb_block_nr], row_nr, bit_num));

            } else {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        }
    }

    return KAPS_JR1_OK;
}




static kaps_jr1_status kaps_jr1_c_model_write_bb_global_config(void *xpt, uint32_t bkt_nr, uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (m->config.is_small_bb_present) {
        if (bkt_nr >= m->config.total_num_small_bb) {
            kaps_jr1_assert(0, "Invalid Bucket Block Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }
    } else {
        if (bkt_nr >= m->config.total_num_bb) {
            kaps_jr1_assert(0, "Invalid Bucket Block Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }
    }

    kaps_jr1_c_model_log_bb_global_config(m, bkt_nr, data);

    if (m->config.is_small_bb_present) {
        KAPS_JR1_STRY(kaps_jr1_c_model_bb_global_config_write(m->small_bbs[bkt_nr], data));
    } else {
        KAPS_JR1_STRY(kaps_jr1_c_model_bb_global_config_write(m->bucket_blocks[bkt_nr], data));
    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_read_rpb(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *key, uint8_t *is_valid)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    *is_valid = 0;

    if (blk_nr >= m->config.total_num_rpb) {
        kaps_jr1_assert(0, "Invalid RPB Number \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    
    if (row_nr >= 2 * m->config.num_rows_in_rpb[blk_nr]) {
        kaps_jr1_assert(0, "Invalid RPB Row \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    KAPS_JR1_STRY(kaps_jr1_c_model_rpb_read(m->rpb_blocks[blk_nr], row_nr, key, is_valid));

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_read_ads(void *xpt, uint32_t blk_nr, uint32_t row_nr, struct kaps_jr1_ads *ads_data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (blk_nr >= m->config.total_num_ads) {
        kaps_jr1_assert(0, "Invalid ADS Number \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }


    if (row_nr >= m->config.num_rows_in_rpb[blk_nr]) {
        kaps_jr1_assert(0, "Invalid ADS Row \n");
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    KAPS_JR1_STRY(kaps_jr1_c_model_ads_read(m->ads_blocks[blk_nr], row_nr, ads_data));

    return KAPS_JR1_OK;
}



static kaps_jr1_status kaps_jr1_c_model_read_bkt(void *xpt, uint32_t bkt_nr, uint32_t row_nr, uint32_t is_small_bb, uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (is_small_bb) {
        if (bkt_nr >= m->config.total_num_small_bb) {
            kaps_jr1_assert(0, "Invalid Small BB Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        if (row_nr >= m->config.num_rows_in_small_bb) {
            kaps_jr1_assert(0, "Invalid Small BB Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        KAPS_JR1_STRY(kaps_jr1_c_model_bb_read(m->small_bbs[bkt_nr], row_nr, data));

    } else {

        if (bkt_nr >= m->config.total_num_bb) {
            kaps_jr1_assert(0, "Invalid Bucket Block Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        if (m->config.is_small_bb_present) {
            if (row_nr >= m->bucket_blocks[bkt_nr]->bb_num_rows) {
                kaps_jr1_assert(0, "Invalid Bucket Block Row \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        } else {
            if (row_nr >= m->config.num_rows_in_bb) {
                kaps_jr1_assert(0, "Invalid Bucket Block Row \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        }

        KAPS_JR1_STRY(kaps_jr1_c_model_bb_read(m->bucket_blocks[bkt_nr], row_nr, data));
    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_read_hb(void *xpt, uint32_t block_nr, uint32_t row_nr, uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t hb_block_nr;
    uint32_t large_bb_nr, num_rows_in_large_bb;

    kaps_jr1_sassert(m->is_hb_supported);

    if (block_nr >= KAPS_JR1_RPB_BLOCK_START && block_nr <= end_rpb_nr) {
        uint32_t num_rows_in_rpb = m->config.num_rows_in_rpb[block_nr - KAPS_JR1_RPB_BLOCK_START];
        uint32_t num_physical_rpb_hb_rows = num_rows_in_rpb / KAPS_JR1_HB_ROW_WIDTH_1;

        kaps_jr1_sassert(m->id != KAPS_JR1_QUMRAN_DEVICE_ID); 

        if (row_nr >= num_physical_rpb_hb_rows) {
            kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

        hb_block_nr = block_nr - KAPS_JR1_RPB_BLOCK_START;
        KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->rpb_hb_blocks[hb_block_nr], row_nr, data));


    } else {
        if (m->config.is_small_bb_present) {
            if (block_nr >= m->config.small_bb_start && block_nr <= m->config.small_bb_end) {

                if (row_nr >= m->config.num_rows_in_small_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                hb_block_nr = block_nr - m->config.small_bb_start;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->small_bb_hb_blocks[hb_block_nr], row_nr, data));

            } else if (block_nr >= m->config.large_bb_start && block_nr <= m->config.large_bb_end) {
                large_bb_nr = block_nr - m->config.large_bb_start;
                large_bb_nr = 2 * large_bb_nr;

                num_rows_in_large_bb = m->bucket_blocks[large_bb_nr]->bb_num_rows;
                if (row_nr >= num_rows_in_large_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                if (m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A000_B960
                    || m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A960_B000
                    || m->bucket_blocks[large_bb_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A480_B480) {

                    hb_block_nr = large_bb_nr;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->bb_hb_blocks[hb_block_nr], row_nr, data));

                    hb_block_nr = large_bb_nr + 1;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->bb_hb_blocks[hb_block_nr], row_nr, &data[KAPS_JR1_HB_ROW_WIDTH_8]));

                } else {
                    hb_block_nr = large_bb_nr;
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->bb_hb_blocks[hb_block_nr], row_nr, data));
                }

            } else {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        } else {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (block_nr >= start_bb_nr && block_nr <= end_bb_nr) {
                if (row_nr >= m->config.num_rows_in_bb) {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }

                hb_block_nr = block_nr - start_bb_nr;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->bb_hb_blocks[hb_block_nr], row_nr, data));

            } else {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }
        }
    }

    return KAPS_JR1_OK;
}



static void kaps_jr1_c_model_get_first_half_of_fmap(uint8_t *bytes, uint32_t nbytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads)
{
    write_ads->format_map_00 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_01 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_02 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_03 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_04 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_05 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_06 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_07 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;
}


static void kaps_jr1_c_model_get_second_half_of_fmap(uint8_t *bytes, uint32_t nbytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads)
{
    write_ads->format_map_08 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_09 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_10 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_11 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_12 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_13 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_14 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;

    write_ads->format_map_15 = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 3, cur_bit_pos);
    cur_bit_pos += 4;
}






static kaps_jr1_status kaps_jr1_c_model_write(void *xpt, enum kaps_jr1_func func, uint8_t blk_nr, uint16_t row_nr, uint32_t nbytes, uint8_t *bytes)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t i;
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t cur_bit_pos;

    if ((func == KAPS_JR1_FUNC1 || func == KAPS_JR1_FUNC4) ) {

        if (blk_nr < KAPS_JR1_RPB_BLOCK_START || blk_nr > end_rpb_nr)
            kaps_jr1_assert(0, "Incorrect RPB or ADS-1 Block Number \n");

        if (func == KAPS_JR1_FUNC1) 
        {
            {
                struct kaps_jr1_dba_entry write_rpb = {{ 0 }};
                
                kaps_jr1_sassert (nbytes == KAPS_JR1_RPB_WIDTH_8 + 1);

                for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++) {
                    write_rpb.key[i] = KapsJr1ReadBitsInArrray(bytes, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                                        ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2);
                }
                write_rpb.resv = KapsJr1ReadBitsInArrray(bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2);
                write_rpb.is_valid = KapsJr1ReadBitsInArrray(bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0);
                write_rpb.pad = KapsJr1ReadBitsInArrray(bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 167, 166);

                status = kaps_jr1_c_model_write_rpb(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr,
                                          write_rpb.key, write_rpb.is_valid);
            }

        }
        else if (func == KAPS_JR1_FUNC4) {
            struct kaps_jr1_ads write_ads = { 0 };


            kaps_jr1_sassert(nbytes == m->config.ads_width_8);

            write_ads.bpm_ad = KapsJr1ReadBitsInArrray(bytes, nbytes, 19, 0);
            write_ads.bpm_len = KapsJr1ReadBitsInArrray(bytes, nbytes, 27, 20);
            write_ads.row_offset = KapsJr1ReadBitsInArrray(bytes, nbytes, 31, 28);

            cur_bit_pos = 32;
            kaps_jr1_c_model_get_first_half_of_fmap(bytes, nbytes, cur_bit_pos, &write_ads);
            cur_bit_pos += 32;

            kaps_jr1_c_model_get_second_half_of_fmap(bytes, nbytes, cur_bit_pos, &write_ads);
            cur_bit_pos += 32;

            if (m->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
                write_ads.bkt_row = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 11, cur_bit_pos);
                cur_bit_pos += 14;
            } else if (m->id == KAPS_JR1_QUX_DEVICE_ID) {
                write_ads.bkt_row = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 7, cur_bit_pos);
                cur_bit_pos += 8;
            } else {
                write_ads.bkt_row = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 9, cur_bit_pos);
                cur_bit_pos += 11;
            }

            write_ads.reserved = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 4, cur_bit_pos);
            cur_bit_pos += 5;

            write_ads.key_shift = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 7, cur_bit_pos);
            cur_bit_pos += 8;

            if (m->id != KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
                write_ads.ecc = KapsJr1ReadBitsInArrray(bytes, nbytes, cur_bit_pos + 7, cur_bit_pos);
                cur_bit_pos += 8;
            }


            status = kaps_jr1_c_model_write_ads(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr, bytes, nbytes,
                                          &write_ads);
        }
    }
    else if (func == KAPS_JR1_FUNC2 || func == KAPS_JR1_FUNC10) {
        uint8_t *write_bkt;

        kaps_jr1_sassert(nbytes == KAPS_JR1_BKT_WIDTH_8);
        write_bkt = bytes;

        {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (blk_nr >= start_bb_nr && blk_nr <= end_bb_nr) {
                status = kaps_jr1_c_model_write_bkt(xpt, blk_nr - start_bb_nr, 
                    row_nr, 0, KAPS_JR1_MDB_LPM_DB_CONFIG_2_STAGE, write_bkt);
            }
            else {
                kaps_jr1_assert(0, "Writing to undefined block");
            }
        }
    }

    else if (func == KAPS_JR1_FUNC0) {

        {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (blk_nr >= start_bb_nr && blk_nr <= end_bb_nr) {

                if (row_nr == KAPS_JR1_BB_GLOBAL_CONFIG_REG_ADDR) {
                    kaps_jr1_sassert(nbytes == KAPS_JR1_REGISTER_WIDTH_8);

                    status = kaps_jr1_c_model_write_bb_global_config(xpt, blk_nr - start_bb_nr, bytes);
                }
            } else {
                kaps_jr1_assert(0, "Writing to undefined block");
            }
        }
    }
    else if (func == KAPS_JR1_FUNC5) {
        
    }
    else {
        kaps_jr1_assert(0, "Writing to undefined block");
    }


    


    return status;
}


static void kaps_jr1_c_model_fill_first_half_of_fmap(uint8_t *write_buf, uint32_t buf_len_in_bytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads)
{
    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_00);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_01);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_02);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_03);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_04);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_05);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_06);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_07);
    cur_bit_pos += 4;
}



static void kaps_jr1_c_model_fill_second_half_of_fmap(uint8_t *write_buf, uint32_t buf_len_in_bytes, uint32_t cur_bit_pos, struct kaps_jr1_ads *write_ads)
{
    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_08);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_09);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_10);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_11);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_12);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_13);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_14);
    cur_bit_pos += 4;

    KapsJr1WriteBitsInArray(write_buf, buf_len_in_bytes, cur_bit_pos + 3, cur_bit_pos, write_ads->format_map_15);
    cur_bit_pos += 4;
}




static kaps_jr1_status kaps_jr1_c_model_read (void *xpt, enum kaps_jr1_func func, uint8_t blk_nr, uint16_t row_nr,
                                   uint32_t n_result_bytes, uint8_t *result_bytes)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t i;
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t cur_bit_pos;


    if (func == KAPS_JR1_FUNC1 || func == KAPS_JR1_FUNC4 ) {
        if (blk_nr < KAPS_JR1_RPB_BLOCK_START || blk_nr > end_rpb_nr)
            kaps_jr1_assert(0, "Incorrect RPB or ADS-1 Block Number \n");


        if (func == KAPS_JR1_FUNC1) {
            struct kaps_jr1_dba_entry rpb_resp = {{ 0 }};
            uint8_t valid_bits;

            if (m->config.tcam_format == KAPS_JR1_MODEL_TCAM_FORMAT_2) {
                kaps_jr1_sassert (n_result_bytes == 2*(KAPS_JR1_RPB_WIDTH_8 + 1));

                
                if (row_nr % 2 == 1)
                    row_nr--;

                status = kaps_jr1_c_model_read_rpb(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr,
                                                             rpb_resp.key, &valid_bits);
                rpb_resp.is_valid = valid_bits;


                for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++) {
                    KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 168 + ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                     168 + ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4, rpb_resp.key[i]);
                }
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 168 + 3, 168 + 0, rpb_resp.resv);
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 168 + 165, 168 + 164, rpb_resp.is_valid);
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 168 + 167, 168 + 166, 0);



                status = kaps_jr1_c_model_read_rpb(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr + 1,
                                                    rpb_resp.key, &valid_bits);

                for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++) {
                    KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 3,
                                     ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 4, rpb_resp.key[i]);
                }
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 3, 0, rpb_resp.resv);
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 165, 164, rpb_resp.is_valid);
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 167, 166, 0);
            }
            else {

                kaps_jr1_sassert (n_result_bytes == KAPS_JR1_RPB_WIDTH_8 + 1);

                status = kaps_jr1_c_model_read_rpb(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr,
                                             rpb_resp.key, &valid_bits);
                rpb_resp.is_valid = valid_bits;


                for (i = 0; i < KAPS_JR1_RPB_WIDTH_8; i++) {
                    KapsJr1WriteBitsInArray(result_bytes, KAPS_JR1_RPB_WIDTH_8 + 1, ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) + 5,
                                     ((KAPS_JR1_RPB_WIDTH_8 - i) * 8) - 2, rpb_resp.key[i]);
                }
                KapsJr1WriteBitsInArray(result_bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 5, 2, rpb_resp.resv);
                KapsJr1WriteBitsInArray(result_bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 1, 0, rpb_resp.is_valid);
                KapsJr1WriteBitsInArray(result_bytes, KAPS_JR1_RPB_WIDTH_8 + 1, 167, 166, 0);
            }


        }
        else if (func == KAPS_JR1_FUNC4) {
            struct kaps_jr1_ads ads_resp = { 0 };

            kaps_jr1_memset(result_bytes, 0, n_result_bytes);

            status = kaps_jr1_c_model_read_ads(xpt, blk_nr - KAPS_JR1_RPB_BLOCK_START, row_nr,
                                         &ads_resp);

            kaps_jr1_sassert(n_result_bytes == m->config.ads_width_8);

            KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 19, 0, ads_resp.bpm_ad);
            KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 27, 20, ads_resp.bpm_len);
            KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, 31, 28, ads_resp.row_offset);

            cur_bit_pos = 32;
            kaps_jr1_c_model_fill_first_half_of_fmap(result_bytes, n_result_bytes, cur_bit_pos, &ads_resp);
            cur_bit_pos += 32;

            kaps_jr1_c_model_fill_second_half_of_fmap(result_bytes, n_result_bytes, cur_bit_pos, &ads_resp);
            cur_bit_pos += 32;

            if (m->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 11, cur_bit_pos, ads_resp.bkt_row);
                cur_bit_pos += 14;

            } else if (m->id == KAPS_JR1_QUX_DEVICE_ID) {
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 7, cur_bit_pos, ads_resp.bkt_row);
                cur_bit_pos += 8;
            } else {
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 9, cur_bit_pos, ads_resp.bkt_row);
                cur_bit_pos += 11;
            }

            KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 4, cur_bit_pos, ads_resp.reserved);
            cur_bit_pos += 5;

            KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 7, cur_bit_pos, ads_resp.key_shift);
            cur_bit_pos += 8;

            if (m->id != KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
                KapsJr1WriteBitsInArray(result_bytes, n_result_bytes, cur_bit_pos + 7, cur_bit_pos, ads_resp.ecc);
                cur_bit_pos += 8;
            }

        }
    }
    else if (func == KAPS_JR1_FUNC2 || func == KAPS_JR1_FUNC10) {

        uint8_t *bkt_resp;

        kaps_jr1_sassert(n_result_bytes == KAPS_JR1_BKT_WIDTH_8);
        bkt_resp = result_bytes;

        {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;

            if (blk_nr >= start_bb_nr && blk_nr <= end_bb_nr) {
                status = kaps_jr1_c_model_read_bkt(xpt, blk_nr - start_bb_nr, row_nr, 0, bkt_resp);
            } else {
                kaps_jr1_assert(0, "Reading from undefined block ");
            }
        }

    } 
    else {
        kaps_jr1_assert(0, "Reading from an undefined block");
    }

    return status;
}


static kaps_jr1_status kaps_jr1_c_model_interface (void *xpt, enum kaps_jr1_cmd cmd, enum kaps_jr1_func func, uint32_t blk_nr, uint32_t row_nr,
                                        uint32_t n_bytes, uint8_t *bytes)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    switch (cmd) {
    case KAPS_JR1_CMD_READ:
    {
        status = kaps_jr1_c_model_read(xpt, func, blk_nr, row_nr, n_bytes, bytes);
        break;
    }
    case KAPS_JR1_CMD_WRITE:
    {
        status = kaps_jr1_c_model_write(xpt, func, blk_nr, row_nr, n_bytes, bytes);
        break;
    }
    case KAPS_JR1_CMD_EXTENDED:
    {
        status = KAPS_JR1_OK;
        break;
    }
    case KAPS_JR1_CMD_ACK:
    {
        status = KAPS_JR1_OK;
        break;
    }
    default:
        return KAPS_JR1_INVALID_ARGUMENT;

    }

    return status;
}


static uint32_t kaps_jr1_c_model_check_bb_global_config(struct kaps_jr1_c_model *m, uint32_t bb_nr, enum kaps_jr1_search_interface search_interface)
{
    uint8_t *bb_config_reg;
    uint32_t is_search_allowed = 0;
    uint32_t rpb0, rpb1;
    uint32_t core0, core1;

    bb_config_reg = m->bucket_blocks[bb_nr]->bb_global_config_reg;

    if (m->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID) {
        core0 = KapsJr1ReadBitsInArrray(bb_config_reg, KAPS_JR1_REGISTER_WIDTH_8, 0, 0);
        core1 = KapsJr1ReadBitsInArrray(bb_config_reg, KAPS_JR1_REGISTER_WIDTH_8, 1, 1);

        if (core1 == 0 && core0 == 0) {
            rpb0 = 0;
            rpb1 = 1;
        } else if (core1 == 0 && core0 == 1) {
            rpb0 = 2;
            rpb1 = 1;
        } else if (core1 == 1 && core0 == 0) {
            rpb0 = 0;
            rpb1 = 3;
        } else {
            rpb0 = 2;
            rpb1 = 3;
        }

        if (search_interface == rpb0 || search_interface == rpb1) {
            is_search_allowed = 1;
        }

    } else if (m->id == KAPS_JR1_QUMRAN_DEVICE_ID || m->id == KAPS_JR1_QUX_DEVICE_ID) {
        rpb0 = KapsJr1ReadBitsInArrray(bb_config_reg, KAPS_JR1_REGISTER_WIDTH_8, 0, 0);
        if (search_interface == rpb0)
            is_search_allowed = 1;
    } else {
        rpb0 = KapsJr1ReadBitsInArrray(bb_config_reg, KAPS_JR1_REGISTER_WIDTH_8, 1, 0);
        rpb1 = KapsJr1ReadBitsInArrray(bb_config_reg, KAPS_JR1_REGISTER_WIDTH_8, 3, 2);

        if (search_interface == rpb0 || search_interface == rpb1)
            is_search_allowed = 1;
    }

    return is_search_allowed;
}


static kaps_jr1_status kaps_jr1_c_model_search(void *xpt, uint8_t *rpb_key, enum kaps_jr1_search_interface search_interface, struct kaps_jr1_search_result *kaps_jr1_result)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    int32_t rpb_hit_index;
    struct kaps_jr1_ads ads_data;
    uint8_t bb_key[KAPS_JR1_BB_KEY_WIDTH_8];
    uint16_t i, j, row_nr, lpu_nr, bb_nr, bb_gran, format_val;
    struct kaps_jr1_c_model_bb_result cur_bb_result, best_bb_result;
    uint32_t num_lpus_to_search, wrap_start_lpu, wrap_around_lpu;
    uint32_t best_bb_nr = 0, best_row = 0;
    uint32_t bb_type_bit = 0;

    kaps_jr1_c_model_log_search_command(m, rpb_key, search_interface);

    kaps_jr1_memset(kaps_jr1_result, 0, sizeof(struct kaps_jr1_search_result));
    kaps_jr1_result->res_ad_width_1 = KAPS_JR1_DEFAULT_AD_WIDTH_1;

    
    KAPS_JR1_STRY(kaps_jr1_c_model_rpb_compare(m->rpb_blocks[search_interface],
                search_interface, rpb_key, &rpb_hit_index));

    if (rpb_hit_index == -1) {
        
        kaps_jr1_c_model_log_search_result(m, kaps_jr1_result);
        kaps_jr1_c_model_increment_search_tag(m);
        m->stats.num_rpb_misses++;
        return KAPS_JR1_OK;
    }

    KAPS_JR1_STRY(kaps_jr1_c_model_ads_read(m->ads_blocks[search_interface], rpb_hit_index, &ads_data));
    kaps_jr1_c_model_get_bb_key(rpb_key, bb_key, ads_data.key_shift);

    
    num_lpus_to_search = KAPS_JR1_DEFAULT_MIN_NUM_BB;
    wrap_start_lpu = 0;
    wrap_around_lpu = KAPS_JR1_DEFAULT_MIN_NUM_BB - 1;
    lpu_nr = ads_data.row_offset;

    row_nr = ads_data.bkt_row;
    kaps_jr1_memset(&best_bb_result, 0, sizeof(struct kaps_jr1_c_model_bb_result));

    for (i = 0; i < num_lpus_to_search; ++i) {
        format_val = kaps_jr1_c_model_get_format_value(m, &ads_data, lpu_nr);

        bb_gran = m->format_map_to_gran[format_val];
        bb_nr = lpu_nr * (m->config.total_num_bb / KAPS_JR1_DEFAULT_MIN_NUM_BB);

        for (j = 0; j < m->config.total_num_bb / KAPS_JR1_DEFAULT_MIN_NUM_BB; ++j) {
            if (!kaps_jr1_c_model_check_bb_global_config(m, bb_nr, search_interface))
                continue;

            KAPS_JR1_STRY(kaps_jr1_c_model_bb_compare(m->bucket_blocks[bb_nr], search_interface, bb_key, row_nr,
                                        bb_gran, bb_type_bit, &cur_bb_result));

            if (cur_bb_result.is_hit && cur_bb_result.match_len >= best_bb_result.match_len) {
                best_bb_result = cur_bb_result;
                best_bb_nr = bb_nr;
                best_row = row_nr;
            }
            ++bb_nr;
        }

        ++lpu_nr;
        if (lpu_nr > wrap_around_lpu) {
            lpu_nr = wrap_start_lpu;
            ++row_nr;
        }

        if (row_nr >= m->config.num_rows_in_bb)
            break;
    }

    if (!best_bb_result.is_hit) {
        
        KapsJr1WriteBitsInArray(kaps_jr1_result->ad_value, KAPS_JR1_DEFAULT_AD_WIDTH_8, KAPS_JR1_AD_ARRAY_END_BIT_POS,
                            KAPS_JR1_AD_ARRAY_START_BIT_POS, ads_data.bpm_ad);

        if (m->is_hb_supported) {
            if (ads_data.bpm_ad != 0) {
                
                uint32_t rpb_hb_row = rpb_hit_index / KAPS_JR1_HB_ROW_WIDTH_1;
                uint32_t bit_pos = rpb_hit_index % KAPS_JR1_HB_ROW_WIDTH_1;

                if (m->id != KAPS_JR1_QUMRAN_DEVICE_ID) {
                    
                    kaps_jr1_c_model_set_hb(xpt, KAPS_JR1_RPB_BLOCK_START + search_interface,
                                         rpb_hb_row, bit_pos);
                }
            }
        }

        kaps_jr1_result->match_len = ads_data.bpm_len;
        kaps_jr1_c_model_log_search_result(m, kaps_jr1_result);
        kaps_jr1_c_model_increment_search_tag(m);

        m->stats.num_ads_bpm_hits++;
        if (ads_data.bpm_ad != 0) {
            m->stats.num_ads_lmpsofar_valid_hits++;
        }

        return KAPS_JR1_OK;
    }

    if (m->is_hb_supported) {
        
        uint32_t start_bb_nr = m->config.large_bb_start;
        uint32_t abs_hb_bb_nr = start_bb_nr + best_bb_nr;
        kaps_jr1_c_model_set_hb(xpt, abs_hb_bb_nr, best_row, best_bb_result.hit_index);
    }

    
    KapsJr1WriteBitsInArray(kaps_jr1_result->ad_value, KAPS_JR1_DEFAULT_AD_WIDTH_8, KAPS_JR1_AD_ARRAY_END_BIT_POS,
                        KAPS_JR1_AD_ARRAY_START_BIT_POS, best_bb_result.match_ad);
    kaps_jr1_result->match_len = best_bb_result.match_len + ads_data.key_shift;
    kaps_jr1_c_model_log_search_result(m, kaps_jr1_result);
    kaps_jr1_c_model_increment_search_tag(m);

    return KAPS_JR1_OK;
}


uint32_t
kaps_jr1_c_model_get_logical_bb_id(
    struct kaps_jr1_c_model *m,
    uint32_t search_interface,
    uint32_t main_bb_nr)
{
    uint32_t logical_bb_id = 0xFFFF;
    
    return logical_bb_id;
}





static kaps_jr1_status kaps_jr1_c_model_register_read(void *xpt, uint32_t offset, uint32_t n_bytes, uint8_t *bytes)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;

    if (offset == KAPS_JR1_REVISION_REGISTER_ADDR) {
        kaps_jr1_sassert(n_bytes >= KAPS_JR1_REGISTER_WIDTH_8);
        KapsJr1WriteBitsInArray(bytes, KAPS_JR1_REGISTER_WIDTH_8, 31, 16, m->id);
        KapsJr1WriteBitsInArray(bytes, KAPS_JR1_REGISTER_WIDTH_8, 15, 0, m->sub_type);
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status kaps_jr1_c_model_register_write(void *xpt, uint32_t offset, uint32_t n_bytes, uint8_t *bytes)
{
    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_hb_dump(void *xpt, uint32_t start_blk_num, uint32_t start_row_num, uint32_t end_blk_num,
                                     uint32_t end_row_num, uint8_t clear_on_read, uint8_t *data)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint32_t offset;
    uint32_t cur_blk_num, cur_row_num;
    uint8_t clear_buf[KAPS_JR1_HB_ROW_WIDTH_8] = { 0 };
    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t hb_block_nr;
    struct kaps_jr1_c_model_hb_block *cur_hb_block = NULL;

    kaps_jr1_sassert(m->is_hb_supported);
    kaps_jr1_sassert(m->id != KAPS_JR1_QUMRAN_DEVICE_ID); 


    offset = 0;
    for (cur_blk_num = start_blk_num; cur_blk_num <= end_blk_num; ++cur_blk_num) {
        for (cur_row_num = start_row_num; cur_row_num <= end_row_num; ++cur_row_num) {

            if (cur_blk_num >= KAPS_JR1_RPB_BLOCK_START && cur_blk_num <= end_rpb_nr) {

                hb_block_nr = cur_blk_num - KAPS_JR1_RPB_BLOCK_START;
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(m->rpb_hb_blocks[hb_block_nr], cur_row_num,
                                            &data[offset]));

                offset += KAPS_JR1_HB_ROW_WIDTH_8;

                if (clear_on_read) {
                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(m->rpb_hb_blocks[hb_block_nr], cur_row_num,
                                            clear_buf));
                }

            } else {
                if (m->config.is_small_bb_present) {
                    if (cur_blk_num < m->config.large_bb_start) {
                        start_bb_nr = m->config.small_bb_start;
                        end_bb_nr = m->config.small_bb_end;
                    } else {
                        start_bb_nr = m->config.large_bb_start;
                        end_bb_nr = m->config.large_bb_end;
                    }
                } else {
                    start_bb_nr = m->config.large_bb_start;
                    end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;
                }

                if (cur_blk_num >= start_bb_nr && cur_blk_num <= end_bb_nr) {
                    hb_block_nr = cur_blk_num - start_bb_nr;

                    if (m->config.is_small_bb_present) {
                        if (cur_blk_num < m->config.large_bb_start) {
                            cur_hb_block = m->small_bb_hb_blocks[hb_block_nr];
                        } else {
                            hb_block_nr = 2 * hb_block_nr;
                            cur_hb_block = m->bb_hb_blocks[hb_block_nr];
                        }
                    } else {
                        cur_hb_block = m->bb_hb_blocks[hb_block_nr];
                    }


                    KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(cur_hb_block, cur_row_num, &data[offset]));
                    if (clear_on_read) {
                        KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(cur_hb_block, cur_row_num, clear_buf));
                    }

                    offset += KAPS_JR1_HB_ROW_WIDTH_8;

                    if (m->config.is_small_bb_present && cur_blk_num >= m->config.large_bb_start) {
                        if (m->bucket_blocks[hb_block_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A000_B960
                            || m->bucket_blocks[hb_block_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A960_B000
                            || m->bucket_blocks[hb_block_nr]->bb_config == KAPS_JR1_LARGE_BB_WIDTH_A480_B480) {

                            hb_block_nr++;
                            cur_hb_block = m->bb_hb_blocks[hb_block_nr];

                            KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(cur_hb_block, cur_row_num, &data[offset]));
                            if (clear_on_read) {
                                KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(cur_hb_block, cur_row_num, clear_buf));
                            }

                            offset += KAPS_JR1_HB_ROW_WIDTH_8;
                        }
                    }


                } else {
                    kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
                    return KAPS_JR1_INVALID_ARGUMENT;
                }
            }
        }
    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status kaps_jr1_c_model_hb_copy(void *xpt, uint32_t src_blk_num, uint32_t src_row_num, uint32_t dest_blk_num,
                                     uint32_t dest_row_num, uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear)
{
    struct kaps_jr1_c_model *m = ((struct kaps_jr1_model_xpt*) xpt)->model;
    uint8_t data[KAPS_JR1_HB_ROW_WIDTH_8];
    uint32_t end_rpb_nr = KAPS_JR1_RPB_BLOCK_START + m->config.total_num_rpb - 1;
    uint32_t start_bb_nr, end_bb_nr;
    uint32_t num_rows = 0;
    struct kaps_jr1_c_model_hb_block *src_hb_block = NULL, *dest_hb_block = NULL;

    kaps_jr1_sassert(m->is_hb_supported);

    if (src_blk_num >= KAPS_JR1_RPB_BLOCK_START && src_blk_num <= end_rpb_nr) {
        kaps_jr1_assert(0, "Performing hit bit copy on RPB");
        return KAPS_JR1_INVALID_ARGUMENT;

    } else {
        if (m->config.is_small_bb_present) {
            if (m->max_num_levels == 3) {
                kaps_jr1_assert(0, "Hit bit copy is not supported for 3 level KAPS_JR1\n");
                return KAPS_JR1_INVALID_ARGUMENT;

            } else {
                start_bb_nr = m->config.small_bb_start;
                end_bb_nr = m->config.small_bb_end;
                num_rows = m->config.num_rows_in_small_bb;
            }


        } else {
            start_bb_nr = m->config.large_bb_start;
            end_bb_nr = start_bb_nr + m->config.total_num_bb - 1;
            num_rows = m->config.num_rows_in_bb;

       }

        if (src_blk_num >= start_bb_nr && src_blk_num <= end_bb_nr
            && dest_blk_num >= start_bb_nr && dest_blk_num <= end_bb_nr) {

            src_blk_num = src_blk_num - start_bb_nr;
            dest_blk_num = dest_blk_num - start_bb_nr;

            if (src_row_num >=  num_rows || dest_row_num >= num_rows) {
                kaps_jr1_assert(0, "Invalid Hit Bit Block Row \n");
                return KAPS_JR1_INVALID_ARGUMENT;
            }

            if (m->config.is_small_bb_present && m->max_num_levels == 2) {
                src_hb_block = m->small_bb_hb_blocks[src_blk_num];
                dest_hb_block = m->small_bb_hb_blocks[dest_blk_num];
            } else {
                src_hb_block = m->bb_hb_blocks[src_blk_num];
                dest_hb_block = m->bb_hb_blocks[dest_blk_num];
            }

            KAPS_JR1_STRY(kaps_jr1_c_model_hb_read(src_hb_block, src_row_num, data));

            KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(dest_hb_block, dest_row_num, data));

            if (perform_clear) {
                kaps_jr1_memset(data, 0, KAPS_JR1_HB_ROW_WIDTH_8);
                KAPS_JR1_STRY(kaps_jr1_c_model_hb_write(src_hb_block, src_row_num, data));
            }

        } else {
            kaps_jr1_assert(0, "Invalid Hit Bit Block Number \n");
            return KAPS_JR1_INVALID_ARGUMENT;
        }

    }

    return KAPS_JR1_OK;
}




kaps_jr1_status kaps_jr1_c_model_xpt_init(enum kaps_jr1_device_pair_type pair_type,
                                uint32_t dev_id, uint32_t kaps_jr1_sub_type, uint32_t profile,
                                struct kaps_jr1_model_xpt **xpt)
{
    struct kaps_jr1_model_xpt *res;
    kaps_jr1_status status;
    uint32_t i;
    uint16_t format_map_to_gran[KAPS_JR1_MAX_NUM_GRANULARITIES + 1] = {0, 8, 16, 24, 32, 40, 48, 56, 72, 96, 136, 168, 12, 20, 28, 60};
    uint32_t num_rows_in_rpb;
    struct kaps_jr1_allocator *alloc = NULL;

    if (xpt == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    
    KAPS_JR1_STRY(kaps_jr1_default_allocator_create(&alloc));

    res = alloc->xcalloc(alloc->cookie, 1, sizeof(*res));
    if (res == NULL) {
        KAPS_JR1_STRY(kaps_jr1_default_allocator_destroy(alloc));
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    res->alloc = alloc;

    res->model = alloc->xcalloc(alloc->cookie, 1, sizeof(*res->model));
    if (res->model == NULL) {
        kaps_jr1_c_model_xpt_destroy(res);
        return KAPS_JR1_OUT_OF_MEMORY;
    }
    res->model->alloc = alloc;


    res->model->id = dev_id;
    res->model->pair_type = pair_type;
    res->model->sub_type = kaps_jr1_sub_type;
    res->model->profile = profile;

    res->model->config.id = dev_id;
    res->model->config.sub_type = kaps_jr1_sub_type;
    res->model->config.profile = profile;

    res->model->max_num_levels = 2;

    res->model->config.large_bb_start = KAPS_JR1_DEFAULT_NUM_RPB_BLOCKS + 1;

    res->model->config.is_x_y_valid_bit_common = 0;
    res->model->config.tcam_format = KAPS_JR1_MODEL_TCAM_FORMAT_1;
    res->model->config.xpt_version = KAPS_JR1_MODEL_XPT_VERSION_1;
    res->model->config.ads_width_8 = 16;

    res->model->config.num_db_groups = 1;
    res->model->config.num_dbs_per_group = 4;

    res->model->is_hb_supported = 1;
    if (res->model->id == KAPS_JR1_DEFAULT_DEVICE_ID)
        res->model->is_hb_supported = 0;

    res->xpt_common.kaps_jr1_command = kaps_jr1_c_model_interface;
    res->xpt_common.kaps_jr1_search = kaps_jr1_c_model_search;
    res->xpt_common.kaps_jr1_register_read = kaps_jr1_c_model_register_read;
    res->xpt_common.kaps_jr1_register_write = kaps_jr1_c_model_register_write;


    num_rows_in_rpb = 2048;
    res->model->config.total_num_rpb = 4;
    res->model->config.total_num_bb = 32;
    res->model->config.num_rows_in_bb = 1024;

    if (res->model->id == KAPS_JR1_QUMRAN_DEVICE_ID)
    {
        num_rows_in_rpb = 2048;
        res->model->config.total_num_rpb = 2;
        res->model->config.total_num_bb = 16;
        res->model->config.num_rows_in_bb = 1024;
    }
    else if (res->model->id == KAPS_JR1_QUX_DEVICE_ID)
    {
        num_rows_in_rpb = 512;
        res->model->config.total_num_rpb = 2;
        res->model->config.total_num_bb = 16;
        res->model->config.num_rows_in_bb = 256;
    }
    else if (res->model->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        num_rows_in_rpb = 8192;
        res->model->config.total_num_rpb = 4;
        res->model->config.total_num_bb = 32;
        res->model->config.num_rows_in_bb = 4096;

        if (res->model->sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
        {
            num_rows_in_rpb = 2048;
            res->model->config.num_rows_in_bb = 2048;
        }
        else if (res->model->sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
        {
            num_rows_in_rpb = 2048 + 256;
            res->model->config.num_rows_in_bb = 2048 + 256;
        }

    }


    for (i = 0; i < res->model->config.total_num_rpb; ++i)
    {
        res->model->config.num_rows_in_rpb[i] = num_rows_in_rpb;
    }


    res->model->config.total_num_ads = res->model->config.total_num_rpb;
    res->model->config.num_bb_in_one_bb_cascade = res->model->config.total_num_bb / KAPS_JR1_DEFAULT_MIN_NUM_BB;


    for (i = 0; i < KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS; ++i) {
        res->model->num_levels_in_search[i] = res->model->max_num_levels;
    }

    for (i = 0; i < res->model->config.total_num_rpb; ++i) {
        status = kaps_jr1_c_model_rpb_create(res->model, i, &res->model->rpb_blocks[i]);
        if (status != KAPS_JR1_OK) {
            kaps_jr1_c_model_xpt_destroy(res);
            return status;
        }
    }

    for (i = 0; i < res->model->config.total_num_rpb; ++i) {
        status = kaps_jr1_c_model_ads_create(res->model, i, &res->model->ads_blocks[i]);
        if (status != KAPS_JR1_OK) {
            kaps_jr1_c_model_xpt_destroy(res);
            return status;
        }
    }

    for (i = 0; i < res->model->config.total_num_small_bb; ++i) {
        status = kaps_jr1_c_model_bb_create(res->model, i, 1, &res->model->small_bbs[i]);
        if (status != KAPS_JR1_OK) {
            kaps_jr1_c_model_xpt_destroy(res);
            return status;
        }
    }

    if (res->model->config.total_num_small_bb) {
        for (i = 0; i < res->model->config.total_num_ads2_blks; ++i) {
            status = kaps_jr1_c_model_ads_create(res->model, i, &res->model->ads2_blocks[i]);
            if (status != KAPS_JR1_OK) {
                kaps_jr1_c_model_xpt_destroy(res);
                return status;
            }
        }
    }

    for (i = 0; i < res->model->config.total_num_bb; ++i) {
            if (i >= KAPS_JR1_MAX_NUM_BB)
                kaps_jr1_assert(0, "Incorrect BB Number during model BB creation \n");

            status = kaps_jr1_c_model_bb_create(res->model, i, 0,
                            &res->model->bucket_blocks[i]);
            if (status != KAPS_JR1_OK) {
                kaps_jr1_c_model_xpt_destroy(res);
                return status;
            }
    }


    if (res->model->is_hb_supported) {

        
        if (res->model->id != KAPS_JR1_QUMRAN_DEVICE_ID) {
            for (i = 0; i < res->model->config.total_num_rpb; ++i) {
                status = kaps_jr1_c_model_hb_create(res->model, i, &res->model->rpb_hb_blocks[i]);
                if (status != KAPS_JR1_OK) {
                    kaps_jr1_c_model_xpt_destroy(res);
                    return status;
                }
            }
        }

        for (i = 0; i < res->model->config.total_num_bb; ++i) {
            if (i >= KAPS_JR1_MAX_NUM_BB)
                kaps_jr1_assert(0, "Incorrect BB Number during model HB creation \n");

            status = kaps_jr1_c_model_hb_create(res->model, i, &res->model->bb_hb_blocks[i]);
            if (status != KAPS_JR1_OK) {
                kaps_jr1_c_model_xpt_destroy(res);
                return status;
            }
        }


        for (i = 0; i < res->model->config.total_num_small_bb; ++i) {
            status = kaps_jr1_c_model_hb_create(res->model, i, &res->model->small_bb_hb_blocks[i]);
            if (status != KAPS_JR1_OK) {
                kaps_jr1_c_model_xpt_destroy(res);
                return status;
            }
        }

    }

    if (res->model->id == KAPS_JR1_DEFAULT_DEVICE_ID) {
        res->xpt_common.kaps_jr1_hb_read = NULL;
    } else {
        res->xpt_common.kaps_jr1_hb_read = kaps_jr1_c_model_read_hb;
    }

    if (res->model->id == KAPS_JR1_DEFAULT_DEVICE_ID) {
        res->xpt_common.kaps_jr1_hb_write = NULL;
    } else {
        res->xpt_common.kaps_jr1_hb_write = kaps_jr1_c_model_write_hb;
    }

    if (res->model->id == KAPS_JR1_DEFAULT_DEVICE_ID || res->model->id == KAPS_JR1_QUMRAN_DEVICE_ID) {
        res->xpt_common.kaps_jr1_hb_dump = NULL;
    } else {
        res->xpt_common.kaps_jr1_hb_dump = kaps_jr1_c_model_hb_dump;
    }

    if (res->model->id == KAPS_JR1_DEFAULT_DEVICE_ID || res->model->id == KAPS_JR1_QUMRAN_DEVICE_ID) {
        res->xpt_common.kaps_jr1_hb_copy = NULL;
    } else {
        res->xpt_common.kaps_jr1_hb_copy = kaps_jr1_c_model_hb_copy;
    }

    kaps_jr1_memcpy(res->model->small_bb_format_map_to_gran, format_map_to_gran, sizeof(format_map_to_gran));

    kaps_jr1_memcpy(res->model->format_map_to_gran, format_map_to_gran, sizeof(format_map_to_gran));


    

    *xpt = res;

    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_c_model_xpt_set_log(struct kaps_jr1_model_xpt *xpt, FILE *fp)
{
    xpt->model->log_file = fp;

    if (fp) 
    {
        xpt->model->was_log_file_set = 1;
    }

    return KAPS_JR1_OK;
}


void kaps_jr1_c_model_print_write_stats(struct kaps_jr1_model_xpt *xpt)
{
    kaps_jr1_printf("Number of RPB writes = %d\n", xpt->model->stats.num_rpb_writes);
    kaps_jr1_printf("Number of small BB writes = %d\n", xpt->model->stats.num_small_bb_writes);
    kaps_jr1_printf("Number of BB writes = %d\n", xpt->model->stats.num_bb_writes);
    kaps_jr1_printf("Number of ADS writes = %d\n", xpt->model->stats.num_ads_writes);
    kaps_jr1_printf("Number of ADS-2 writes = %d\n", xpt->model->stats.num_ads2_writes);

}

kaps_jr1_status kaps_jr1_c_model_xpt_destroy(struct kaps_jr1_model_xpt *xpt)
{
    struct kaps_jr1_allocator *alloc = xpt->alloc;
    uint32_t i;

    if (xpt->model->was_log_file_set) {
        kaps_jr1_printf("Number of RPB misses = %d \n", xpt->model->stats.num_rpb_misses);
        kaps_jr1_printf("Number of ADS Zero length hits = %d\n", xpt->model->stats.num_ads_bpm_hits - xpt->model->stats.num_ads_lmpsofar_valid_hits);
        kaps_jr1_printf("Number of ADS lmpsofar hits = %d\n", xpt->model->stats.num_ads_lmpsofar_valid_hits);
        kaps_jr1_printf("Number of ADS-2 Zero length hits = %d\n", xpt->model->stats.num_ads2_bpm_hits - xpt->model->stats.num_ads2_lmpsofar_valid_hits);
        kaps_jr1_printf("Number of ADS-2 lmpsofar hits = %d\n", xpt->model->stats.num_ads2_lmpsofar_valid_hits);
    }

    for (i = 0; i < xpt->model->config.total_num_rpb; ++i)
        kaps_jr1_c_model_rpb_destroy(xpt->model->rpb_blocks[i]);

    for (i = 0; i < xpt->model->config.total_num_rpb; ++i)
        kaps_jr1_c_model_ads_destroy(xpt->model->ads_blocks[i]);

    for (i = 0; i < xpt->model->config.total_num_small_bb; ++i) {
        kaps_jr1_c_model_bb_destroy(xpt->model->small_bbs[i]);
    }

    if (xpt->model->config.total_num_small_bb) {
        for (i = 0; i < xpt->model->config.total_num_ads2_blks; ++i) {
            kaps_jr1_c_model_ads_destroy(xpt->model->ads2_blocks[i]);
        }
    }

    for (i = 0; i < xpt->model->config.total_num_bb; ++i) {
        kaps_jr1_c_model_bb_destroy(xpt->model->bucket_blocks[i]);
    }


    for (i = 0; i < xpt->model->config.total_num_rpb; ++i) {
        if (xpt->model->rpb_hb_blocks[i]) {
            kaps_jr1_c_model_hb_destroy(xpt->model->rpb_hb_blocks[i]);
        }
    }

    for (i = 0; i < xpt->model->config.total_num_bb; ++i) {
        if (xpt->model->bb_hb_blocks[i]) {
            kaps_jr1_c_model_hb_destroy(xpt->model->bb_hb_blocks[i]);
        }
    }

    for (i = 0; i < xpt->model->config.total_num_small_bb; ++i) {
        if (xpt->model->small_bb_hb_blocks[i]) {
            kaps_jr1_c_model_hb_destroy(xpt->model->small_bb_hb_blocks[i]);
        }
    }

    alloc->xfree(alloc->cookie, xpt->model);
    alloc->xfree(alloc->cookie, xpt);

    (void) kaps_jr1_default_allocator_destroy(alloc);

    return KAPS_JR1_OK;
}


