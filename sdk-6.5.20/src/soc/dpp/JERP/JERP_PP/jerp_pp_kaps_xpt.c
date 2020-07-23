/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#include <soc/mem.h>

#if defined(BCM_88680_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FORWARD
#include <soc/mem.h>



#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps.h>
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_xpt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>

#include <soc/dpp/JER/jer_sbusdma_desc.h>






#define JERP_RPB_FIRST_BLK_ID  1
#define JERP_RPB_LAST_BLK_ID   4
#define JERP_BB_FIRST_BLK_ID   5
#define JERP_BB_LAST_BLK_ID    12
#define JERP_BBS_FIRST_BLK_ID  13
#define JERP_BBS_LAST_BLK_ID   36

#define JERP_NUMELS_BBS        8

#define JERP_JER_MODE_MAX_IDX  1024


#define JERP_KAPS_HB_BYTES                2
#define JERP_KAPS_HB_STM_ENTRIES_IN_LINE  16


#define JERP_KAPS_HB_STM_REPLY_BYTES  (JERP_KAPS_HB_STM_ENTRIES_IN_LINE * JERP_KAPS_HB_BYTES)
#define JERP_KAPS_HB_STM_REPLY_UINT32 (JERP_KAPS_HB_BYTES / SOC_SAND_REG_SIZE_BYTES)
#define JERP_KAPS_HB_STM_REPLY_BITS   (BYTES2BITS(JERP_KAPS_HB_STM_ENTRIES_IN_LINE * JERP_KAPS_HB_BYTES))



#define JERP_KAPS_HB_STM_CMD_BYTES  (448 / SOC_SAND_NOF_BITS_IN_BYTE)
#define JERP_KAPS_HB_STM_CMD_UINT32 (448 / SOC_SAND_NOF_BITS_IN_UINT32)

#define JERP_KAPS_HB_HW_ROWS_PER_BB        (1024*4)
#define JERP_KAPS_HB_HW_INSTANCES_PER_CORE (16)

#define JERP_KAPS_HB_HW_BBS_PER_INSTANCE(unit)   (SOC_IS_QAX(unit) ? 1 : 2)

#define JERP_KAPS_HB_HW_BB_DUPLICATES(unit)   (SOC_IS_QAX(unit) ? 1 : 2)

#define JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID 1022
#define JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID  1023

























kbp_status jerp_kaps_xpt_translate_blk(int unit, uint32 blk_nr, uint32 row_nr, uint32 *blk_hw_nr, uint32 *row_hw_nr) {
    if ((blk_nr >= JERP_BB_FIRST_BLK_ID) && (blk_nr <= JERP_BBS_LAST_BLK_ID)) {
        
        *blk_hw_nr =  (blk_nr - JERP_BB_FIRST_BLK_ID)/JERP_KAPS_HB_HW_BBS_PER_INSTANCE(unit);
        *row_hw_nr = row_nr + ((blk_nr - JERP_BB_FIRST_BLK_ID) % JERP_KAPS_HB_HW_BBS_PER_INSTANCE(unit) ? JERP_KAPS_HB_HW_ROWS_PER_BB : 0);
    } else {
        *row_hw_nr = row_nr;
        if ((blk_nr == JERP_RPB_FIRST_BLK_ID) || (blk_nr == JERP_RPB_LAST_BLK_ID - 1)) {
            *blk_hw_nr =  JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID;
        } else if ((blk_nr == JERP_RPB_FIRST_BLK_ID + 1) || (blk_nr == JERP_RPB_LAST_BLK_ID)) {
            *blk_hw_nr =  JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID;
        } else {
            LOG_CLI((BSL_META_U(0,"%s() unrecognized blk_br: %d \n"),FUNCTION_NAME(), blk_nr));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }

    return KBP_OK;
}

kbp_status jerp_kaps_xpt_hb_read(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes) {
    int rv = KBP_OK, unit;
    uint32 data;
    uint32 blk_hw_nr;
    uint32 row_hw_nr;
    uint8 desc_dma_enabled = 0;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

    memset(bytes, 0 , JERP_KAPS_HB_BYTES * sizeof(uint8_t));

    rv = jerp_kaps_xpt_translate_blk(unit, blk_nr, row_nr, &blk_hw_nr, &row_hw_nr);
    if (rv != KBP_OK) {
        return rv;
    }

    rv = qax_pp_kaps_utilize_desc_dma_get(unit, &desc_dma_enabled);
    if (rv != KBP_OK) {
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    
    if (desc_dma_enabled != 0) {
        rv = jer_sbusdma_desc_wait_done(unit);
        if (rv != KBP_OK) {
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }

    if ((blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) || (blk_hw_nr == JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID)) {
        int array_index = (blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) ? 0 : 1;
        READ_KAPS_TCAM_HIT_INDICATIONm(unit, array_index, KAPS_BLOCK(unit, 0), row_hw_nr, &data);
    } else {
        
        int i;
        uint32 data_core;
        data = 0;
        for (i = 0; i < JERP_KAPS_HB_HW_BB_DUPLICATES(unit); i++) {
            
            READ_KAPS_RPB_TCAM_HIT_INDICATIONm(unit, blk_hw_nr + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i), KAPS_BLOCK(unit, 0), row_hw_nr, &data_core);
            data |= data_core;
        }
    }

    bytes[0] = (data >> SOC_SAND_NOF_BITS_IN_BYTE) & 0xFF;
    bytes[1] = data & 0xFF;

#ifdef JERP_KAPS_XPT_PRINTS_ENABLED
    {
        uint32 i =0;
        LOG_CLI((BSL_META_U(0,"%s() start\n blk_nr: %d, blk_hw_nr: %d, row_nr: %d, bytes: 0x"),FUNCTION_NAME(), blk_nr, blk_hw_nr, row_nr));
        for (i = 0; i < JERP_KAPS_HB_BYTES; i++) {
            LOG_CLI((BSL_META_U(0,"%02X "),bytes[i]));
        }
        LOG_CLI((BSL_META_U(0,"\n")));
        LOG_CLI((BSL_META_U(0,"data: 0x%08X\n"), data));
        LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
    }
#endif

    return rv;
}

kbp_status jerp_kaps_xpt_hb_write(void *xpt, uint32_t blk_nr, uint32_t row_nr, uint8_t *bytes) {
    int rv = KBP_OK, unit;
    uint32 blk_hw_nr;
    uint32 row_hw_nr;
    uint32 data = (bytes[0] << SOC_SAND_NOF_BITS_IN_BYTE) | bytes[1];
    uint8 desc_dma_enabled = 0;
    int i;
    soc_reg_above_64_val_t reg_above64_val;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    rv = jerp_kaps_xpt_translate_blk(unit, blk_nr, row_nr, &blk_hw_nr, &row_hw_nr);
    if (rv != KBP_OK) {
        return rv;
    }

    rv = qax_pp_kaps_utilize_desc_dma_get(unit, &desc_dma_enabled);
    if (rv != KBP_OK) {
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (desc_dma_enabled == 0) {
        if (!SOC_IS_QAX(unit)) {
             
            SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
            rv = WRITE_KAPS_INDIRECT_WR_MASKr(unit, reg_above64_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), WRITE_KAPS_INDIRECT_WR_MASKr failed, 0xffffffff.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }

        if ((blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) || (blk_hw_nr == JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID)) {
            int array_index = (blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) ? 0 : 1;
            WRITE_KAPS_TCAM_HIT_INDICATIONm(unit, array_index, KAPS_BLOCK(unit, 0), row_hw_nr, &data);
        } else {
            
            for (i = 0; i < JERP_KAPS_HB_HW_BB_DUPLICATES(unit); i++) {
                
                WRITE_KAPS_RPB_TCAM_HIT_INDICATIONm(unit, blk_hw_nr + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i), KAPS_BLOCK(unit, 0), row_hw_nr, &data);
            }
        }

        if (!SOC_IS_QAX(unit)) {
            SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
            rv = WRITE_KAPS_INDIRECT_WR_MASKr(unit, reg_above64_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), WRITE_KAPS_INDIRECT_WR_MASKr failed, 0x0.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
    } else {
        soc_mem_t mem;

        if (!SOC_IS_QAX(unit)) {
            
            SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
            rv = jer_sbusdma_desc_add_reg(unit, KAPS_INDIRECT_WR_MASKr, REG_PORT_ANY, 0, reg_above64_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add_reg failed, 0xffffffff.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }

        if ((blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) || (blk_hw_nr == JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID)) {
            int array_index = (blk_hw_nr == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) ? 0 : 1;
            mem = KAPS_TCAM_HIT_INDICATIONm;
            rv = jer_sbusdma_desc_add_mem(unit, mem, array_index, KAPS_BLOCK(unit, 0), row_hw_nr, &data);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add_mem failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        } else {
            
            mem = KAPS_RPB_TCAM_HIT_INDICATIONm;
            for (i = 0; i < JERP_KAPS_HB_HW_BB_DUPLICATES(unit); i++) {
                rv = jer_sbusdma_desc_add_mem(unit, mem,blk_hw_nr + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i), KAPS_BLOCK(unit, 0), row_hw_nr, &data);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add_mem failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }
        }

        if (!SOC_IS_QAX(unit)) {
            SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
            rv = jer_sbusdma_desc_add_reg(unit, KAPS_INDIRECT_WR_MASKr, REG_PORT_ANY, 0, reg_above64_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add_reg failed, 0x0.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }
    }

#ifdef JERP_KAPS_XPT_PRINTS_ENABLED
    {
        uint32 i =0;
        LOG_CLI((BSL_META_U(0,"%s() start\n blk_nr: %d, blk_hw_nr: %d, row_nr: %d, bytes: 0x"),FUNCTION_NAME(), blk_nr, blk_hw_nr, row_nr));
        for (i = 0; i < JERP_KAPS_HB_BYTES; i++) {
            LOG_CLI((BSL_META_U(0,"%02X "),bytes[i]));
        }
        LOG_CLI((BSL_META_U(0,"\n")));
        LOG_CLI((BSL_META_U(0,"data: 0x%08X\n"), data));
        LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
    }
#endif

    return rv;
}

kbp_status jerp_kaps_xpt_hb_dump_reply(int unit, uint32 start_hw_blk_num, uint32 end_hw_blk_num, 
                                       uint32 start_hw_row_num, uint32 end_hw_row_num,
                                       uint8 clear_on_read, soc_reg_above_64_val_t reply_line)
{
    int rv = KBP_OK;
    soc_reg_above_64_val_t command_line;
    soc_timeout_t to;
    uint32 reg_val;

    memset(command_line, 0 , JERP_KAPS_HB_STM_CMD_UINT32 * sizeof(command_line[0]));

    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_START_BUCKET_NUMf, start_hw_blk_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_START_ENTRYf, start_hw_row_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_END_BUCKET_NUMf, end_hw_blk_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_END_ENTRYf, end_hw_row_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_CLEARf, clear_on_read);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_DUMP_READ_ENf, 1);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_VALIDf, 1);

    rv = WRITE_KAPS_HITBIT_COMMANDS_LINEr(unit, command_line);
    if (rv != SOC_E_NONE) {
        rv = KBP_INTERNAL_ERROR;
        return rv;
    }

    if (!SAL_BOOT_PLISIM) {
        
        soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
        for(;;){
            rv = READ_KAPS_HITBIT_CONTROLr(unit, &reg_val);
            if (rv != SOC_E_NONE) {
                rv = KBP_INTERNAL_ERROR;
                return rv;
            }

            if(soc_reg_field_get(unit, KAPS_HITBIT_CONTROLr, reg_val, HITBIT_STM_DONEf) == 1) {
                break;
            }

            if (soc_timeout_check(&to)) {
                LOG_CLI((BSL_META_U(0, "%s() State machine control done signal timeout!\n"),
                                        FUNCTION_NAME()));
                rv = KBP_FATAL_TRANSPORT_ERROR;
                return rv;
            }
        }
    }

    rv = READ_KAPS_HITBIT_REPLYS_LINEr(unit, reply_line);
    if (rv != SOC_E_NONE) {
        rv = KBP_INTERNAL_ERROR;
        return rv;
    }

    return rv;
}

kbp_status jerp_kaps_xpt_hb_dump(void *xpt, uint32_t start_blk_num, uint32_t start_row_num, uint32_t end_blk_num,
                                uint32_t end_row_num, uint8_t clear_on_read, uint8_t *data) {
    soc_reg_above_64_val_t reply_line;
    uint32 replies_num;
    uint32 start_hw_blk_num, end_hw_blk_num;
    uint32 start_hw_row_num, end_hw_row_num;
    int rv = KBP_OK, unit, i, j;
    uint8 shift=0;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

    replies_num = (end_row_num - start_row_num) * (end_blk_num - start_blk_num);
    if (replies_num > JERP_KAPS_HB_STM_ENTRIES_IN_LINE) {
        LOG_CLI((BSL_META_U(0, "%s() Number of requested replies exceeds max number of replies!\n"),
                                        FUNCTION_NAME()));
        LOG_CLI((BSL_META_U(0,"start_blk_num: %d, start_row_num: %d \n"),start_blk_num, start_row_num));
        LOG_CLI((BSL_META_U(0,"end_blk_num: %d, end_row_num: %d \n"), end_blk_num, end_row_num));
        LOG_CLI((BSL_META_U(0,"replies_num: %d, max_replies_num: %d \n"), replies_num, JERP_KAPS_HB_STM_ENTRIES_IN_LINE));
        rv = KBP_FATAL_TRANSPORT_ERROR;
        return rv;
    }

    memset(reply_line, 0 , JERP_KAPS_HB_STM_CMD_UINT32 * sizeof(reply_line[0]));

    rv = jerp_kaps_xpt_translate_blk(unit, start_blk_num, start_row_num, &start_hw_blk_num, &start_hw_row_num);
    if (rv != KBP_OK) {
        return rv;
    }

    rv = jerp_kaps_xpt_translate_blk(unit, end_blk_num, end_row_num, &end_hw_blk_num, &end_hw_row_num);
    if (rv != KBP_OK) {
        return rv;
    }

    if ((start_hw_blk_num == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) || (start_hw_blk_num == JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID)) {
        rv = jerp_kaps_xpt_hb_dump_reply(unit, start_hw_blk_num, end_hw_blk_num, 
                                           start_hw_row_num, end_hw_row_num,
                                           clear_on_read, reply_line);
        if (rv != KBP_OK) {
            LOG_CLI((BSL_META_U(0,"jerp_kaps_xpt_hb_dump_reply Failed\n")));
            return rv;
        }
    } else {
        soc_reg_above_64_val_t reply_line_core;
        
        for (i = 0; i < JERP_KAPS_HB_HW_BB_DUPLICATES(unit); i++) {
             memset(reply_line_core, 0 , JERP_KAPS_HB_STM_CMD_UINT32 * sizeof(reply_line_core[0]));
            rv = jerp_kaps_xpt_hb_dump_reply(unit, start_hw_blk_num + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i),
                                               end_hw_blk_num + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i),
                                               start_hw_row_num, end_hw_row_num,
                                               clear_on_read, reply_line);
            if (rv != KBP_OK) {
                LOG_CLI((BSL_META_U(0,"jerp_kaps_xpt_hb_dump_reply Core %d Failed\n"), i));
                return rv;
            }

            SHR_BITOR_RANGE(reply_line_core, reply_line, 0, JERP_KAPS_HB_STM_REPLY_BITS, reply_line);
        }
    }

    for (i = 0; i < replies_num; i++) {
        for (j = 0; j < JERP_KAPS_HB_BYTES; j++) {
            shift = (((i % (SOC_SAND_NOF_BITS_IN_UINT32/SOC_SAND_NOF_BITS_IN_BYTE)) + j) * SOC_SAND_NOF_BITS_IN_BYTE);
            data[(i*JERP_KAPS_HB_BYTES) + j] = (shift > 31) ? 0 : (reply_line[i/(SOC_SAND_NOF_BITS_IN_UINT32/SOC_SAND_NOF_BITS_IN_BYTE)] >> shift);
        }
    }

#ifdef JERP_KAPS_XPT_PRINTS_ENABLED
    {
        LOG_CLI((BSL_META_U(0,"%s() start\n start_blk_num: %d, start_hw_blk_num: %d, start_row_num: %d \n"),FUNCTION_NAME(), start_blk_num, start_hw_blk_num, start_row_num));
        LOG_CLI((BSL_META_U(0,"end_blk_num: %d, end_hw_blk_num: %d, end_row_num: %d, clear_on_read: %d, data: 0x"), end_blk_num, end_hw_blk_num, end_row_num, clear_on_read));
        for (i = 0; i < replies_num * JERP_KAPS_HB_BYTES; i++) {
            LOG_CLI((BSL_META_U(0,"%02X "),data[i]));
        }
        LOG_CLI((BSL_META_U(0,"\n")));
        LOG_CLI((BSL_META_U(0,"reply_line: 0x")));
        for (i = 0; i < JERP_KAPS_HB_STM_REPLY_UINT32; i++) {
            LOG_CLI((BSL_META_U(0,"%08X "),reply_line[i]));
        }
        LOG_CLI((BSL_META_U(0,"\n")));
        LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
    }
#endif

    return rv;
}

kbp_status jerp_kaps_xpt_hb_copy_write(int unit, uint32 src_hw_blk_num, uint32 dest_hw_blk_num, uint32 src_hw_row_num, uint32 dest_hw_row_num,
                                       uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear)
{
    soc_reg_above_64_val_t command_line;
    uint32 reg_val;
    soc_timeout_t to;
    int rv = KBP_OK;
    uint8 desc_dma_enabled;

    memset(command_line, 0 , JERP_KAPS_HB_STM_CMD_UINT32 * sizeof(command_line[0]));

    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_START_BUCKET_NUMf, src_hw_blk_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_START_ENTRYf, src_hw_row_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_END_BUCKET_NUMf, dest_hw_blk_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_END_ENTRYf, dest_hw_row_num);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_SOURCE_MASKf, source_mask);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_ROTATE_RIGHTf, rotate_right);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_CLEARf, perform_clear);
    soc_reg_field_set(unit, KAPS_HITBIT_COMMANDS_LINEr, command_line, HITBIT_CMD_VALIDf, 1);

    rv = qax_pp_kaps_utilize_desc_dma_get(unit, &desc_dma_enabled);
    if (rv != KBP_OK) {
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (desc_dma_enabled == 0) {
        rv = WRITE_KAPS_HITBIT_COMMANDS_LINEr(unit, command_line);
        if (rv != SOC_E_NONE) {
            rv = KBP_INTERNAL_ERROR;
            return rv;
        }

        if (!SAL_BOOT_PLISIM) {
            
            soc_timeout_init(&to, ARAD_TIMEOUT, ARAD_MIN_POLLS);
            for(;;){
                rv = READ_KAPS_HITBIT_CONTROLr(unit, &reg_val);
                if (rv != SOC_E_NONE) {
                    rv = KBP_INTERNAL_ERROR;
                    return rv;
                }

                if(soc_reg_field_get(unit, KAPS_HITBIT_CONTROLr, reg_val, HITBIT_STM_DONEf) == 1) {
                    break;
                }

                if (soc_timeout_check(&to)) {
                    LOG_CLI((BSL_META_U(0, "%s() State machine control done signal timeout!\n"),
                                            FUNCTION_NAME()));
                    rv = KBP_FATAL_TRANSPORT_ERROR;
                    return rv;
                }
            }
        }
    } else {
        
        rv = jer_sbusdma_desc_add_reg(unit, KAPS_HITBIT_COMMANDS_LINEr, REG_PORT_ANY, 0, command_line);
        if (rv != SOC_SAND_OK) {
            LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add_reg failed, 0xffffffff.\n"),FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }

    return rv;
}

kbp_status jerp_kaps_xpt_hb_copy(void *xpt, uint32_t src_blk_num, uint32_t src_row_num, uint32_t dest_blk_num,
                                uint32_t dest_row_num, uint16_t source_mask, uint8_t rotate_right, uint8_t perform_clear) {
    uint32 src_hw_blk_num, dest_hw_blk_num;
    uint32 src_hw_row_num, dest_hw_row_num;
    int rv = KBP_OK, unit, i;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

    rv = jerp_kaps_xpt_translate_blk(unit, src_blk_num, src_row_num, &src_hw_blk_num, &src_hw_row_num);
    if (rv != KBP_OK) {
        return rv;
    }

    rv = jerp_kaps_xpt_translate_blk(unit, dest_blk_num, dest_row_num, &dest_hw_blk_num, &dest_hw_row_num);
    if (rv != KBP_OK) {
        return rv;
    }

    if ((src_hw_blk_num == JERP_KAPS_HB_HW_PRIVATE_RPB_BLKID) || (src_hw_blk_num == JERP_KAPS_HB_HW_PUBLIC_RPB_BLKID)) {
        rv = jerp_kaps_xpt_hb_copy_write(unit, src_hw_blk_num, dest_hw_blk_num, src_hw_row_num, dest_hw_row_num,
                                       source_mask, rotate_right, perform_clear);
        if (rv != KBP_OK) {
            LOG_CLI((BSL_META_U(0,"jerp_kaps_xpt_hb_copy_write Failed\n")));
            return rv;
        }
    } else {
        
        for (i = 0; i < JERP_KAPS_HB_HW_BB_DUPLICATES(unit); i++) {
            rv = jerp_kaps_xpt_hb_copy_write(unit, src_hw_blk_num + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i),
                                             dest_hw_blk_num + (JERP_KAPS_HB_HW_INSTANCES_PER_CORE * i),
                                             src_hw_row_num, dest_hw_row_num,
                                             source_mask, rotate_right, perform_clear);
            if (rv != KBP_OK) {
                LOG_CLI((BSL_META_U(0,"jerp_kaps_xpt_hb_copy_write Core %d Failed\n"), i));
                return rv;
            }
        }
    }

#ifdef JERP_KAPS_XPT_PRINTS_ENABLED
    {
        LOG_CLI((BSL_META_U(0,"%s() start\n start_blk_num: %d, src_hw_blk_num: %d, start_row_num: %d \n"),FUNCTION_NAME(), src_blk_num, src_hw_blk_num, src_row_num));
        LOG_CLI((BSL_META_U(0,"dest_blk_num: %d, dest_hw_blk_num: %d, dest_row_num: %d, perform_clear: %d"), dest_blk_num, dest_hw_blk_num, dest_row_num, perform_clear));
        LOG_CLI((BSL_META_U(0,"source_mask: %d, rotate_right: %d \n"), source_mask, rotate_right));
        LOG_CLI((BSL_META_U(0,"\n")));
        LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
    }
#endif

    return rv;
}

kbp_status jerp_kaps_xpt_translate_blk_func_offset_to_mem_reg(int unit,
                                                          uint8     blk_id,
                                                          uint32     func,
                                                          uint32     offset,
                                                          soc_mem_t *mem,
                                                          soc_reg_t *reg,
                                                          uint32    *array_index,
                                                          int       *blk, 
                                                          int       *instance) 
{
    uint32 rv = KBP_OK;

    *mem = INVALIDm;
    *reg = INVALIDr;
    *array_index = 0;

    if (blk_id >= JERP_BBS_FIRST_BLK_ID && blk_id <= JERP_BBS_LAST_BLK_ID) {
        *array_index = (blk_id - JERP_BBS_FIRST_BLK_ID) % JERP_NUMELS_BBS;
        *instance = (blk_id - JERP_BBS_FIRST_BLK_ID) / JERP_NUMELS_BBS;
        *blk = KAPS_BBS_BLOCK(unit, *instance);

        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_BB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_BBS_BB_GLOBAL_CONFIGr;
                   *blk = *instance | SOC_REG_ADDR_INSTANCE_MASK;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported BB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_BUCKET_MAP_MEMORYm;
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d. KAPS_BUCKET_MAP_MEMORYm is not  supported in Jericho Plus!\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
            case KAPS_FUNC2:
            case KAPS_FUNC5:
               *mem = KAPS_BBS_BUCKET_MEMORYm;
               break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }
    } else if (blk_id >= JERP_BB_FIRST_BLK_ID && blk_id <= JERP_BB_LAST_BLK_ID) {
        *array_index = blk_id - JERP_BB_FIRST_BLK_ID;
        *blk = KAPS_BLOCK(unit, 0);
        *instance = 0;
        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_BB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_BB_GLOBAL_CONFIGr;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported BB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_BUCKET_MAP_MEMORYm;
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d. KAPS_BUCKET_MAP_MEMORYm is not  supported in Jericho Plus!\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
            case KAPS_FUNC2:
            case KAPS_FUNC5:
               *mem = KAPS_BUCKET_MEMORYm;
               break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  BB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }
    } else if (blk_id >= JERP_RPB_FIRST_BLK_ID && blk_id <= JERP_RPB_LAST_BLK_ID) {
        *array_index = blk_id - JERP_RPB_FIRST_BLK_ID;
        *blk = KAPS_BLOCK(unit, 0);
        *instance = 0;
        switch (func)
        {
           case KAPS_FUNC0:
               if (offset == KAPS_RPB_CAM_BIST_CONTROL_OFFSET) {
                   *reg = KAPS_RPB_CAM_BIST_CONTROLr;
               } else if (offset == KAPS_RPB_CAM_BIST_STATUS_OFFSET) {
                   *reg = KAPS_RPB_CAM_BIST_STATUSr;
               } else if (offset == KAPS_RPB_GLOBAL_CONFIG_OFFSET) {
                   *reg = KAPS_RPB_GLOBAL_CONFIGr;
               } else{
                   LOG_CLI((BSL_META_U(0, "%s():  unsupported RPB register offset: %d\n"),
                                    FUNCTION_NAME(), offset));
                   rv = KBP_FATAL_TRANSPORT_ERROR;
               }
               break;

            case KAPS_FUNC1:
               *mem = KAPS_RPB_TCAM_CPU_COMMANDm;
               break;

            case KAPS_FUNC4:
                *mem = KAPS_RPB_ADSm;
                break;

            default:
               LOG_CLI((BSL_META_U(0, "%s():  RPB, unsupported func: %d\n"),
                                    FUNCTION_NAME(), func));
               rv = KBP_FATAL_TRANSPORT_ERROR;
               break;
        }


    } else {
        LOG_CLI((BSL_META_U(0,"%s(), unrecognized blk_id = %d.\n"),FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return rv;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
