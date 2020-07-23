/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/

#include <soc/mem.h>

#if defined(BCM_88470_A0) && defined(INCLUDE_KBP)

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
#include <soc/dpp/JERP/JERP_PP/jerp_pp_kaps_xpt.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_kaps_xpt.h>

#include <soc/dpp/JER/JER_PP/jer_pp_kaps_entry_mgmt.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/JER/jer_sbusdma_desc.h>








#define QAX_KAPS_MAX_UINT32_WIDTH     16
#define QAX_KAPS_RPB_MAX_UINT32_WIDTH 6
#define QAX_KAPS_AD_MAX_UINT32_WIDTH  4

#define QAX_KAPS_DIRECT_RW_UINT32_WIDTH 4












typedef enum
{
    RPB_FIRST_BLK_ID   = 1,
    RPB_0_BLK_ID       = RPB_FIRST_BLK_ID,
    RPB_1_BLK_ID,
    RPB_LAST_BLK_ID    = RPB_1_BLK_ID,
    BB_FIRST_BLK_ID    = 5,
    BB_0_BLK_ID        = BB_FIRST_BLK_ID,
    BB_1_BLK_ID,
    BB_2_BLK_ID,
    BB_3_BLK_ID,
    BB_4_BLK_ID,
    BB_5_BLK_ID,
    BB_6_BLK_ID,
    BB_7_BLK_ID,
    BB_8_BLK_ID,
    BB_9_BLK_ID,
    BB_10_BLK_ID,
    BB_11_BLK_ID,
    BB_12_BLK_ID,
    BB_13_BLK_ID,
    BB_14_BLK_ID,
    BB_15_BLK_ID,
    BB_LAST_BLK_ID     = BB_15_BLK_ID,

    KAPS_BLK_ID_LAST

} KAPS_BLK_IDS;






STATIC uint8 qax_pp_kaps_utilize_desc_dma_enable[SOC_SAND_MAX_DEVICE];






uint32 qax_pp_kaps_utilize_desc_dma(int unit, uint8 enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    qax_pp_kaps_utilize_desc_dma_enable[unit] = enable;

    SOCDNX_FUNC_RETURN;
}

uint32 qax_pp_kaps_utilize_desc_dma_get(int unit, uint8 *enable)
{
    SOCDNX_INIT_FUNC_DEFS;

    *enable = qax_pp_kaps_utilize_desc_dma_enable[unit];

    SOCDNX_FUNC_RETURN;
}

kbp_status qax_pp_kaps_search(void *xpt,
                                     uint8_t *key,
                                     enum kaps_search_interface search_interface,
                                     struct kaps_search_result *kaps_result)
{
    int rv = KBP_OK,
        unit;
    uint32 func = KAPS_FUNC3,
        i = 0,j,k;
    uint32 mem_array[QAX_KAPS_MAX_UINT32_WIDTH];
    uint32 mem_index = QAX_KAPS_RPB_MAX_UINT32_WIDTH - 2;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0, "%s():  search_interface: %d, key: 0x"),
                         FUNCTION_NAME(), search_interface));
    for (j = 0; j < JER_KAPS_KEY_BUFFER_NOF_BYTES; j++) {
        LOG_CLI((BSL_META_U(0,"%02X"),key[j]));
    }
    LOG_CLI((BSL_META_U(0,"\n")));
#endif

    sal_memset(mem_array, 0,  QAX_KAPS_MAX_UINT32_WIDTH*sizeof(uint32));
    for (k = 0; k < JER_KAPS_KEY_BUFFER_NOF_BYTES/4; k++) {
        for(j=0; j<4; j++) {
            mem_array[mem_index] |= key[i] << (3-j)*8;
            i++;
        }
        mem_index--;
    }
    mem_array[QAX_KAPS_RPB_MAX_UINT32_WIDTH - 1] |= (func << 6);

    rv = soc_mem_array_write(unit, KAPS_RPB_TCAM_CPU_COMMANDm, search_interface, KAPS_BLOCK(unit, 0), 0 , mem_array);
    if (rv != SOC_SAND_OK) {
        LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_write failed.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
        rv = jer_sbusdma_desc_wait_done(unit);
        if (rv != SOC_SAND_OK) {
            LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_wait_done failed.\n"),FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
    }

    rv = soc_mem_array_read(unit, KAPS_RPB_TCAM_CPU_COMMANDm, search_interface, KAPS_BLOCK(unit, 0), 0 , mem_array);
    if (rv != SOC_SAND_OK) {
        LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_read failed.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    if (SOC_IS_QAX(unit)) {
        kaps_result->match_len = mem_array[0] >> 24;
        kaps_result->ad_value[2]= (mem_array[1] << 4);
        kaps_result->ad_value[1]= (mem_array[1] >> 4);
        kaps_result->ad_value[0]= (mem_array[1] >> 12);
    } else {
        
        kaps_result->match_len = mem_array[0] >> 30 | ((mem_array[1] & 0x3F) << 2);
        kaps_result->ad_value[2]= (mem_array[1] >> 2);
        kaps_result->ad_value[1]= (mem_array[1] >> 10);
        kaps_result->ad_value[0]= (mem_array[1] >> 18);
    }

#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"result_length: %d, result:0x%02x%02x%02x\n"), kaps_result->match_len, kaps_result->ad_value[0], kaps_result->ad_value[1], kaps_result->ad_value[2]));
    for (j=0; j < QAX_KAPS_MAX_UINT32_WIDTH; j++) {
        LOG_CLI((BSL_META_U(0, "mem_array[%d] = 0x%08X\n"), j, mem_array[j]));
    }
    LOG_CLI((BSL_META_U(0, "%s()\n"), FUNCTION_NAME()));
#endif
    return rv;
}

kbp_status qax_kaps_register_read(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes)
{
    int rv = KBP_OK,
        i, unit;
    uint32 reg_val = 0;
    uint16 dev_id;
    uint8 rev_id;

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    if ((offset == 0) && (nbytes == 4) && (bytes != NULL)) {
        if (SOC_IS_QUX(unit)) {
            reg_val = KAPS_QUX_REVISION_REG_VALUE;
        } else if (SOC_IS_QAX(unit)) {
            reg_val = KAPS_QUMRAN_AX_REVISION_REG_VALUE;
        } else if (SOC_IS_JERICHO_PLUS(unit)) {
            soc_cm_get_id(unit, &dev_id, &rev_id);
            if (rev_id == BCM88680_A0_REV_ID) {
                
                if (dcmn_device_block_for_feature(unit, DCMN_NO_EXTENDED_LPM_FEATURE)) {
                    
                    reg_val = KAPS_JERICHO_PLUS_JER_MODE_FM0_REVISION_REG_VALUE;
                } else {
                    reg_val = KAPS_JERICHO_PLUS_FM0_REVISION_REG_VALUE;
                }
            } else {
                
                if (dcmn_device_block_for_feature(unit, DCMN_NO_EXTENDED_LPM_FEATURE)) {
                    
                    reg_val = KAPS_JERICHO_PLUS_JER_MODE_FM4_REVISION_REG_VALUE;
                } else {
                    reg_val = KAPS_JERICHO_PLUS_FM4_REVISION_REG_VALUE;
                }
            }
        } else {
           
           reg_val = KAPS_JERICHO_REVISION_REG_VALUE;
        }
    } else {
        LOG_CLI((BSL_META_U(0,"%s() only supports reading the KapsRevision register.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }
    for (i = 0; i < nbytes; i++) {
        bytes[i] = (reg_val >> ((nbytes-1-i)*8)) & 0xff;
    }

    return rv;
}

kbp_status qax_kaps_register_write(void *xpt, uint32_t offset, uint32_t nbytes, uint8_t *bytes)
{
    LOG_CLI((BSL_META_U(0,"%s() is not supported.\n"),FUNCTION_NAME()));
    return KBP_FATAL_TRANSPORT_ERROR;
}

STATIC
kbp_status qax_kaps_translate_blk_func_offset_to_mem_reg(int unit,
                                                         uint8 blk_id,
                                                         uint32 func,
                                                         uint32 offset,
                                                         soc_mem_t *mem,
                                                         soc_reg_t *reg,
                                                         uint32 *array_index,
                                                         int *blk, 
                                                         int *instance) 
{
    uint32 rv = KBP_OK;

    *mem = INVALIDm;
    *reg = INVALIDr;
    *array_index = 0;
    *blk = KAPS_BLOCK(unit, 0);
    *instance = 0;

    if (blk_id >= RPB_FIRST_BLK_ID && blk_id <= RPB_LAST_BLK_ID) {
        *array_index = blk_id - RPB_FIRST_BLK_ID;
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


    } else if (blk_id >= BB_FIRST_BLK_ID && blk_id <= BB_LAST_BLK_ID) {
        *array_index = blk_id - BB_FIRST_BLK_ID;
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
    } else {
        LOG_CLI((BSL_META_U(0,"%s(), unrecognized blk_id = %d.\n"),FUNCTION_NAME(), blk_id));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

    return rv;
}

kbp_status qax_pp_kaps_write_command(void *xpt,
                                     uint8 blk_id,
                                     uint32 cmd,
                                     uint32 func,
                                     uint32 offset,
                                     uint32 nbytes,
                                     uint8 *bytes)
{
    int rv = KBP_OK,
        unit = 0;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    uint32 reg_val = 0;
    uint32 array_index = 0;
    uint32 mem_array[QAX_KAPS_MAX_UINT32_WIDTH];
    uint32 mem_index = 0;
    uint32 byte_idx = 0;
    uint32 word_idx = 0;
    int blk = -1;
    int instance = 0;

#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s() start\n nbytes: %d, bytes: 0x"),FUNCTION_NAME(), nbytes));
    for (byte_idx = 0; byte_idx < nbytes; byte_idx++) {
        LOG_CLI((BSL_META_U(0,"%02X "),bytes[byte_idx]));
    }
    LOG_CLI((BSL_META_U(0,"\n")));
    LOG_CLI((BSL_META_U(0, "%s():  blk_id: %d, cmd: %d, func: %d, offset: %d, nbytes: %d\n"),
                         FUNCTION_NAME(), blk_id, cmd, func, offset, nbytes));
#endif

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;
    if (SOC_IS_QAX(unit)) {
        rv = qax_kaps_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk, &instance);
    }
#if defined(BCM_88680_A0) && defined(INCLUDE_KBP)
    else if (SOC_IS_JERICHO_PLUS(unit)) {
        rv = jerp_kaps_xpt_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk, &instance);
    }
#endif

    if (rv != KBP_OK) {
        LOG_CLI((BSL_META_U(0,"%s(), qax_kaps_translate_blk_func_offset_to_mem_reg failed.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }
#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s(): mem: %s, reg: %s, array_index: %d. \n"),
             FUNCTION_NAME(), SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit,reg), array_index));
#endif

    if (mem != INVALIDm) {
        
        sal_memset(mem_array, 0,  QAX_KAPS_MAX_UINT32_WIDTH * sizeof(uint32));

        
        if ((func == KAPS_FUNC5) && ((mem == KAPS_BUCKET_MEMORYm) || (mem == KAPS_BBS_BUCKET_MEMORYm))) {
            uint32 bucket_row = offset / 4;
            uint32 bucket_row_offset = (offset % 4) * 120;
            uint32 shifted_mem_array[QAX_KAPS_DIRECT_RW_UINT32_WIDTH];

            
            mem_index = QAX_KAPS_DIRECT_RW_UINT32_WIDTH - 1;

            if(nbytes == 15) {
                
                byte_idx = 0;
                for (word_idx = 0; word_idx < BYTES2WORDS(nbytes); word_idx++) {
                    if (word_idx == 0) {
                        
                        shifted_mem_array[mem_index] =  bytes[byte_idx] << 16 |
                                                        bytes[byte_idx + 1] << 8 |
                                                        bytes[byte_idx + 2];
                        byte_idx+=3;
                    } else {
                        shifted_mem_array[mem_index] = bytes[byte_idx] << 24 |
                                                       bytes[byte_idx + 1] << 16 |
                                                       bytes[byte_idx + 2] << 8 |
                                                       bytes[byte_idx + 3];
                        byte_idx+=4;
                    }
                    mem_index--;
                }
            } else {
                byte_idx = 0;
                for (word_idx = 0; word_idx < BYTES2WORDS(nbytes); word_idx++) {
                    shifted_mem_array[mem_index] = bytes[byte_idx] << 24 |
                                                   bytes[byte_idx + 1] << 16 |
                                                   bytes[byte_idx + 2] << 8 |
                                                   bytes[byte_idx + 3];
                    byte_idx+=4;
                    mem_index--;
                }
            }
            
            if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
                jer_sbusdma_desc_wait_done(unit);
            }

            rv = soc_mem_array_read(unit, mem, array_index, blk, bucket_row, mem_array);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_read failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            SHR_BITCOPY_RANGE(mem_array, bucket_row_offset, shifted_mem_array, 0, 120);

            if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
                rv = jer_sbusdma_desc_add_mem(unit, mem, array_index, blk, bucket_row, mem_array);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            } else {
                rv = soc_mem_array_write(unit, mem, array_index, blk, bucket_row, mem_array);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_write failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }

            if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
                
                READ_KAPS_BUCKET_MAP_MEMORYm(unit, array_index, blk, bucket_row, mem_array);

                shifted_mem_array[0] = bytes[0] & 0x7F;

                bucket_row_offset = (offset % 4) * 7;

                SHR_BITCOPY_RANGE(mem_array, bucket_row_offset, shifted_mem_array, 0, 7);

                mem = KAPS_BUCKET_MAP_MEMORYm;

                if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
                    rv = jer_sbusdma_desc_add_mem(unit, mem, array_index, blk, bucket_row, mem_array);
                    if (rv != SOC_SAND_OK) {
                        LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add failed.\n"),FUNCTION_NAME()));
                        return KBP_FATAL_TRANSPORT_ERROR;
                    }
                } else {
                    rv = soc_mem_array_write(unit, mem, array_index, blk, bucket_row, mem_array);
                    if (rv != SOC_SAND_OK) {
                        LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_write failed.\n"),FUNCTION_NAME()));
                        return KBP_FATAL_TRANSPORT_ERROR;
                    }
                }
            }
        } else {
            
            if (mem == KAPS_RPB_TCAM_CPU_COMMANDm) {
                mem_index = QAX_KAPS_RPB_MAX_UINT32_WIDTH - 1;
            } else if (mem == KAPS_RPB_ADSm) {
                mem_index = QAX_KAPS_AD_MAX_UINT32_WIDTH - 1;
            } else if ((mem == KAPS_BUCKET_MEMORYm) || (mem == KAPS_BBS_BUCKET_MEMORYm)) {
                mem_index = QAX_KAPS_MAX_UINT32_WIDTH - 2;
            }

            byte_idx = 0;
            if (nbytes % 4 > 0) {
                for (byte_idx = 0; byte_idx < nbytes % 4; byte_idx++) {
                    mem_array[mem_index] |= bytes[byte_idx] << (nbytes % 4 - 1 - byte_idx) * 8;
                }
                mem_index--;
            }
            if(mem_index < QAX_KAPS_MAX_UINT32_WIDTH) {
                for (word_idx = 0; word_idx < nbytes / 4; word_idx++) {
                    mem_array[mem_index] = bytes[byte_idx] << 24 |
                                           bytes[byte_idx + 1] << 16 |
                                           bytes[byte_idx + 2] << 8 |
                                           bytes[byte_idx + 3];
                    byte_idx+=4;
                    mem_index--;
                }
            }

            if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
                rv = jer_sbusdma_desc_add_mem(unit, mem, array_index, blk, offset, mem_array);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            } else {
                rv = soc_mem_array_write(unit, mem, array_index, blk, offset, mem_array);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_write failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }
        }

    } else if (reg != INVALIDr) {
        
        
        reg_val = (uint32) bytes[0] << 24 | (uint32) bytes[1] << 16 | (uint32) bytes[2] << 8 | (uint32) bytes[3];

        if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
            rv = jer_sbusdma_desc_add_reg(unit, reg, instance, array_index, &reg_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), jer_sbusdma_desc_add failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        } else {
            rv = soc_reg32_set(unit, reg, instance, array_index, reg_val);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), soc_reg32_set failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
        }

    } else {
        LOG_CLI((BSL_META_U(0,"%s(), both mem and reg are invalid.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }


#ifdef QAX_KAPS_XPT_PRINTS_ENABLED                                                                                         
    LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
#endif

    return KBP_OK;
}

   kbp_status qax_pp_kaps_read_command(void *xpt,
                                           uint32 blk_id,
                                           uint32 cmd,
                                           uint32 func,
                                           uint32 offset,
                                           uint32 nbytes,
                                           uint8 *bytes)
{
    int rv = KBP_OK,
        unit = 0;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;
    uint32 reg_val = 0;
    uint32 array_index = 0;
    uint32 mem_array[QAX_KAPS_MAX_UINT32_WIDTH];
    uint32 mem_index = 0;
    uint32 byte_idx = 0;
    uint32 word_byte = 0;
    uint32 word_idx = 0;
    int blk = -1;
    int instance = 0;
#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s() start\n nbytes: %d, bytes: 0x"),FUNCTION_NAME(), nbytes));
    for (byte_idx = 0; byte_idx < nbytes; byte_idx++) {
        LOG_CLI((BSL_META_U(0,"%02X "),bytes[byte_idx]));
    }
    LOG_CLI((BSL_META_U(0,"\n")));
    LOG_CLI((BSL_META_U(0, "%s():  blk_id: %d, cmd: %d, func: %d, offset: %d, nbytes: %d\n"),
                         FUNCTION_NAME(), blk_id, cmd, func, offset, nbytes));
#endif

    unit = ((JER_KAPS_XPT*)xpt)->jer_data.unit;

    if (SOC_IS_QAX(unit)) {
        rv = qax_kaps_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk, &instance);
    }
#if defined(BCM_88680_A0) && defined(INCLUDE_KBP)
    else if (SOC_IS_JERICHO_PLUS(unit)) {
        rv = jerp_kaps_xpt_translate_blk_func_offset_to_mem_reg(unit, blk_id, func, offset, &mem, &reg, &array_index, &blk, &instance);
    }
#endif
    if (rv != KBP_OK) {
        LOG_CLI((BSL_META_U(0,"%s(), qax_kaps_translate_blk_func_offset_to_mem_reg failed.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s(): mem: %s, reg: %s, array_index: %d. \n"),
             FUNCTION_NAME(), SOC_MEM_NAME(unit, mem), SOC_REG_NAME(unit,reg), array_index));
#endif

    if (mem != INVALIDm) {
        sal_memset(mem_array, 0,  QAX_KAPS_MAX_UINT32_WIDTH*sizeof(uint32));

        
        if ((func == KAPS_FUNC5) && ((mem == KAPS_BUCKET_MEMORYm) || (mem == KAPS_BBS_BUCKET_MEMORYm))) {
            uint32 bucket_row = offset / 4;
            uint32 bucket_row_offset = (offset % 4) * 120;
            uint32 shifted_mem_array[QAX_KAPS_DIRECT_RW_UINT32_WIDTH];

            
            mem_index = QAX_KAPS_DIRECT_RW_UINT32_WIDTH - 1;

            
            if (qax_pp_kaps_utilize_desc_dma_enable[unit]) {
                jer_sbusdma_desc_wait_done(unit);
            }

            rv = soc_mem_array_read(unit, mem, array_index, blk, bucket_row, mem_array);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_read failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }
            SHR_BITCOPY_RANGE(shifted_mem_array, 0, mem_array, bucket_row_offset, 120);
            if(nbytes == 15) {
                
                byte_idx = 0;
                for (word_idx = 0; word_idx < BYTES2WORDS(nbytes); word_idx++) {
                    for(word_byte = 0; word_byte < 4; word_byte++) {
                        if (byte_idx > nbytes) {
                            break;
                        }
                        bytes[byte_idx] = shifted_mem_array[mem_index] >> (3 - word_byte)*8;
                        byte_idx++;
                    }
                    mem_index--;
                }
                
                for(byte_idx = 0; byte_idx < nbytes; byte_idx++) {
                    bytes[byte_idx] = bytes[byte_idx + 1];
                }
            } else {
                
                byte_idx = 1;
                for (word_idx = 0; word_idx < BYTES2WORDS(nbytes); word_idx++) {
                    for(word_byte = 0; word_byte < ((word_idx == 0) ? 3 : 4); word_byte++) {
                        if (byte_idx == nbytes) {
                            break;
                        }
                        bytes[byte_idx] = shifted_mem_array[mem_index] >> (((word_idx == 0) ? 2 : 3) - word_byte)*8;
                        byte_idx++;
                    }
                    mem_index--;
                }
            }

            if (SOC_IS_QAX(unit) && !SOC_IS_QUX(unit)) {
                
                READ_KAPS_BUCKET_MAP_MEMORYm(unit, array_index, blk, bucket_row, mem_array);

                bucket_row_offset = (offset % 4) * 7;

                SHR_BITCOPY_RANGE(shifted_mem_array, 0, mem_array, bucket_row_offset, 7);

                bytes[0] = shifted_mem_array[0] & 0x7F;
            }
        } else {
            if (mem == KAPS_RPB_TCAM_CPU_COMMANDm) {
                
                mem_array[QAX_KAPS_RPB_MAX_UINT32_WIDTH - 1] |= (func << 6);
                rv = soc_mem_array_write(unit, mem, array_index, blk, offset, mem_array);
                if (rv != SOC_SAND_OK) {
                    LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_write failed.\n"),FUNCTION_NAME()));
                    return KBP_FATAL_TRANSPORT_ERROR;
                }
            }

            
            rv = soc_mem_array_read(unit, mem, array_index, blk, offset, mem_array);
            if (rv != SOC_SAND_OK) {
                LOG_CLI((BSL_META_U(0,"%s(), soc_mem_array_read failed.\n"),FUNCTION_NAME()));
                return KBP_FATAL_TRANSPORT_ERROR;
            }

            
            if (mem == KAPS_RPB_TCAM_CPU_COMMANDm) {
                mem_index = QAX_KAPS_RPB_MAX_UINT32_WIDTH - 1;
            } else if (mem == KAPS_RPB_ADSm) {
                mem_index = QAX_KAPS_AD_MAX_UINT32_WIDTH - 1;
            } else if ((mem == KAPS_BUCKET_MEMORYm) || (mem == KAPS_BBS_BUCKET_MEMORYm)) {
                mem_index = QAX_KAPS_MAX_UINT32_WIDTH - 2;
            }

            byte_idx = 0;
            if (nbytes % 4 > 0) {
                for (byte_idx = 0; byte_idx < nbytes % 4; byte_idx++) {
                    bytes[byte_idx] = mem_array[mem_index] >> ((nbytes % 4 - 1 - byte_idx) * 8);
                }
                mem_index--;
            }
            if(mem_index < QAX_KAPS_MAX_UINT32_WIDTH) {
                for (word_idx = 0; word_idx < nbytes / 4; word_idx++) {
                    for(word_byte = 0; word_byte < 4; word_byte++) {
                        bytes[byte_idx] = mem_array[mem_index] >> (3 - word_byte)*8;
                        byte_idx++;
                    }
                    mem_index--;
                }
            }
        }
    } else if (reg != INVALIDr) {
        
        
        rv = soc_reg32_get(unit, reg, instance, array_index, &reg_val);
        if (rv != SOC_SAND_OK) {
            LOG_CLI((BSL_META_U(0,"%s(), soc_reg32_get failed.\n"),FUNCTION_NAME()));
            return KBP_FATAL_TRANSPORT_ERROR;
        }
        bytes[0] = reg_val >> 24; bytes[1] = reg_val >> 16; bytes[2] = reg_val >> 8; bytes[3] = reg_val;

    } else {
        LOG_CLI((BSL_META_U(0,"%s(), both mem and reg are invalid.\n"),FUNCTION_NAME()));
        return KBP_FATAL_TRANSPORT_ERROR;
    }

#ifdef QAX_KAPS_XPT_PRINTS_ENABLED
    LOG_CLI((BSL_META_U(0,"%s() end\n"),FUNCTION_NAME()));
#endif

    return KBP_OK;
}

kbp_status qax_pp_kaps_command(void *xpt,
                               enum kaps_cmd cmd,
                               enum kaps_func func,
                               uint32_t blk_nr,
                               uint32_t row_nr,
                               uint32_t nbytes,
                               uint8_t *bytes)
{
    int rv = KBP_OK;

    switch (cmd)
    {
       case KAPS_CMD_READ:
           rv = qax_pp_kaps_read_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
           break;

       case KAPS_CMD_WRITE:
           rv = qax_pp_kaps_write_command(xpt, blk_nr, cmd, func, row_nr, nbytes, bytes);
           break;

       case KAPS_CMD_EXTENDED:
           LOG_CLI((BSL_META_U(0, "%s():  IBC interface disabled, redundant command: %d\n"), FUNCTION_NAME(), cmd));
           break;

       default:
           LOG_CLI((BSL_META_U(0, "%s():  unsupported cmd: %d\n"),
                                FUNCTION_NAME(), cmd));
           rv = KBP_FATAL_TRANSPORT_ERROR;
           break;
    }

    return rv;
}



#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
