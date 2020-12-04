/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_KBP_ROP_INCLUDED__

#define __ARAD_KBP_ROP_INCLUDED__




#include <soc/kbp/nlm3/arch/nlmarch.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>






#define ARAD_KBP_CPU_DATA_REGISTER_BYTE_LEN   (64)
#define ARAD_KBP_CPU_LOOKUP_REPLY_DATA_REGISTER_BYTE_LEN   (32)

#define ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES   (ARAD_PP_FLP_KBP_MAX_NUMBER_OF_RESULTS)

#define ARAD_KBP_LUT_REC_TYPE_INFO 0x1 
#define ARAD_KBP_LUT_REC_TYPE_REQUEST 0x0 

#define ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_SEQ_NUM_MODE  0x0
#define ARAD_KBP_LUT_INSTR_LUT1_REC_DATA_CONTEXID_SEQ_NUM_MODE 0x1
#define ARAD_KBP_LUT_INSTR_REC_DATA_CONTEXID_SEQ_NUM_MODE 0x2
#define ARAD_KBP_LUT_INSTR_LUT1_CONTEXID_REC_DATA_MODE 0x3
#define ARAD_KBP_LUT_INSTR_REC_DATA_CONTEXID_REC_DATA_MODE 0x4

#define ARAD_KBP_LUT_KEY_CONFIG_APPEND_INCOMING_DATA 1
#define ARAD_KBP_LUT_KEY_CONFIG_SEND_INCOMING_DATA 0


#define ARAD_KBP_LUT_KEY_SIZE 0x1 
#define ARAD_KBP_LUT_80b_KEY_SIZE 0x0 

#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_16b 0x1
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_32b 0x2
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_48b 0x3
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_64b 0x4
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_80b 0x5
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_96b 0x6
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_112b 0x7
#define ARAD_KBP_LUT_EXTR_MSB_PORTION_DATA_128b 0x8

#define ARAD_KBP_LUT_TRANSFER_INDX_ONLY 0x0
#define ARAD_KBP_LUT_TRANSFER_AD_ONLY 0x1

#define ARAD_KBP_LUT_INDX_TRANSFER_1B 0x1
#define ARAD_KBP_LUT_INDX_TRANSFER_2B 0x2
#define ARAD_KBP_LUT_INDX_TRANSFER_3B 0x3
 
#define ARAD_KBP_LUT_AD_TRANSFER_1B 0x1
#define ARAD_KBP_LUT_AD_TRANSFER_2B 0x2
#define ARAD_KBP_LUT_AD_TRANSFER_3B 0x3
#define ARAD_KBP_LUT_AD_TRANSFER_4B 0x4
#define ARAD_KBP_LUT_AD_TRANSFER_5B 0x5
#define ARAD_KBP_LUT_AD_TRANSFER_6B 0x6
#define ARAD_KBP_LUT_AD_TRANSFER_7B 0x7
#define ARAD_KBP_LUT_AD_TRANSFER_8B 0x8
#define ARAD_KBP_LUT_AD_TRANSFER_9B 0x9
#define ARAD_KBP_LUT_AD_TRANSFER_10B 0xa
#define ARAD_KBP_LUT_AD_TRANSFER_11B 0xb
#define ARAD_KBP_LUT_AD_TRANSFER_12B 0xc
#define ARAD_KBP_LUT_AD_TRANSFER_13B 0xd
#define ARAD_KBP_LUT_AD_TRANSFER_14B 0xe
#define ARAD_KBP_LUT_AD_TRANSFER_15B 0xf
#define ARAD_KBP_LUT_AD_TRANSFER_16B 0x0





#define ARAD_KBP_ROP_REVERSE_DATA(data_in, data_out, len) \
    { \
        int i; \
        for(i = 0; i < len; i++) { \
                   data_out[i] = data_in[len - 1 - i]; \
        } \
    }

#define ARAD_KBP_ROP_LUT_INSTR_CMP1_GET(ltr) \
    ((ltr & 0x3f) | (0x001 << 6) | ((ltr & 0x40) << 9))

#define ARAD_KBP_ROP_LUT_INSTR_CMP3_GET(ltr) \
    ((ltr & 0x3f) | (0x3 << 6) | ((ltr & 0x40) << 9))

#define ARAD_KBP_ROP_LUT_INSTR_CMP1_BUILD(instr, ltr) \
    instr = (ltr & 0x3f) | (0x001 << 6) | ((ltr & 0x40) << 9);

#define ARAD_KBP_ROP_LUT_INSTR_CMP2_BUILD(instr, ltr) \
    instr = (ltr & 0x3f) | (0x2 << 6) | ((ltr & 0x40) << 9);

#define ARAD_KBP_ROP_LUT_INSTR_CMP3_BUILD(instr, ltr) \
    instr = (ltr & 0x3f) | (0x3 << 6) | ((ltr & 0x40) << 9);





 
typedef enum
{
    NLM_ARAD_CB_INST_WRITE, 
    NLM_ARAD_CB_INST_CMP1,  
    NLM_ARAD_CB_INST_CMP2,  
    NLM_ARAD_CB_INST_CMP3,   
    NLM_ARAD_CB_INST_LPM,   
    NLM_ARAD_CB_INST_NONE       
} NlmAradCBInstType;

typedef enum NlmAradReadMode
{
    NLM_ARAD_READ_MODE_DATA_X, 
    NLM_ARAD_READ_MODE_DATA_Y  
}NlmAradReadMode;

typedef enum NlmAradWriteMode
{
    NLM_ARAD_WRITE_MODE_DATABASE_DM, 
    NLM_ARAD_WRITE_MODE_DATABASE_XY 
}NlmAradWriteMode;  

typedef enum NlmAradCompareResponseFormat
{
    NLM_ARAD_ONLY_INDEX_NO_AD = 0x0,  
    NLM_ARAD_INDEX_AND_32B_AD = 0x1,  
    NLM_ARAD_INDEX_AND_64B_AD = 0x2,  
    NLM_ARAD_INDEX_AND_128B_AD = 0x3, 
    NLM_ARAD_INDEX_AND_256B_AD = 0x4,  
    NLM_ARAD_NO_INDEX_NO_AD = -1  
}NlmAradCompareResponseFormat;
 







typedef struct arad_kbp_lut_data_t {
    
    uint32 rec_size;  

    
    uint32 rec_type;    

    
    uint32 rec_is_valid;




    
   
    uint32 mode;
    

  uint32 key_config;


  uint32 lut_key_data;


  uint32 instr;

    
  uint32 key_w_cpd_gt_80;


  uint32 copy_data_cfg;









    uint32  result_idx_ad_cfg[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];
    uint32  result_idx_or_ad[ARAD_KBP_MAX_NUM_OF_PARALLEL_SEARCHES];
}arad_kbp_lut_data_t;


typedef struct arad_kbp_rop_write_s {
    uint8            addr[NLMDEV_REG_ADDR_LEN_IN_BYTES];       
    uint8            data[NLM_DATA_WIDTH_BYTES];               
    uint8            mask[NLM_DATA_WIDTH_BYTES];               
    uint8            addr_short[NLMDEV_REG_ADDR_LEN_IN_BYTES]; 
    uint8            vBit;                                     
    NlmAradWriteMode writeMode;                                
    uint32          loop;
    
} arad_kbp_rop_write_t;

typedef struct arad_kbp_rop_read_s {
    uint8  addr[NLMDEV_REG_ADDR_LEN_IN_BYTES];    
    uint8  vBit;                                  
    NlmAradReadMode dataType;                     
    uint8  data[NLM_DATA_WIDTH_BYTES + 1];        
} arad_kbp_rop_read_t;

typedef struct arad_kbp_rop_cbw_s {
        
    uint8 data[NLM_MAX_CMP_DATA_LEN_BYTES];              
    uint32 data_len;   

} arad_kbp_rop_cbw_t;


typedef struct NlmAradCompareResult
{
    uint8 isResultValid[NLM_MAX_NUM_RESULTS]; 
    uint8 hitOrMiss[NLM_MAX_NUM_RESULTS];     
    uint32 hitIndex[NLM_MAX_NUM_RESULTS];     
    NlmAradCompareResponseFormat responseFormat[NLM_MAX_NUM_RESULTS]; 

    
    uint8 assocData[NLM_MAX_NUM_RESULTS][NLM_MAX_SRAM_SB_WIDTH_IN_BYTES]; 

    uint8 data_raw[NLMDEV_MAX_RESP_LEN_IN_BYTES];


}NlmAradCompareResult;

typedef struct arad_kbp_rop_cbw_cmp_s {
        
    NlmAradCBInstType type;
    uint32 opcode;
    uint32 ltr;
    arad_kbp_rop_cbw_t cbw_data;  
    NlmAradCompareResult result;

} arad_kbp_rop_cbw_cmp_t;

typedef struct arad_kbp_rop_blk_s {
        
    uint32 opcode;
    uint16 loop_cnt;
    uint32 src_addr;	
    uint32 dst_addr;	
    uint8  set_not_clr; 
    uint8  copy_dir;	
    uint32 data_len;    

} arad_kbp_rop_blk_t;












void arad_kbp_format_long_integer(char *buf, uint32 *val, int nval);
void arad_kbp_array_print_debug(uint8 *data, int len);



uint32 arad_kbp_cpu_lookup_reply(
    int unit,
    uint32 core,
    soc_reg_above_64_val_t data);

uint32 arad_kbp_cpu_record_send(
    int unit,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable);

#ifdef BCM_88660
uint32 aradplus_kbp_cpu_record_send(
    int unit,
    uint32 core,
    uint32 opcode,
    soc_reg_above_64_val_t msb_data,
    soc_reg_above_64_val_t lsb_data,
    int lsb_enable,
    soc_reg_above_64_val_t read_data);
#endif



uint32 arad_kbp_lut_write(int unit, uint32 core, uint8 addr, arad_kbp_lut_data_t* lut_data, soc_reg_above_64_val_t lut_data_raw);


uint32 arad_kbp_frwrd_opcode_set(int unit, uint8 opcode, uint32 tx_data_size, uint32 tx_data_type, uint32 rx_data_size, uint32 rx_data_type);

uint32 arad_kbp_lut_read(int unit, uint32 core, uint8 addr,arad_kbp_lut_data_t* lut_data, soc_reg_above_64_val_t lut_raw_data);

uint32 arad_kbp_rop_write(
        int unit,
        uint32 core, 
        arad_kbp_rop_write_t *wr_data);

uint32 arad_kbp_rop_read(
        int unit,
        uint32 core, 
        arad_kbp_rop_read_t *rd_data);

uint32 arad_kbp_rop_cbw_cmp(
        int unit, 
        uint32 core,
        arad_kbp_rop_cbw_cmp_t *cbw_cmp_data);

uint32 arad_kbp_rop_blk_write(
        int unit, 
        uint32 core,
        arad_kbp_rop_blk_t *wr_data);

uint32 arad_kbp_rop_dma_init(int unit);
uint32 arad_kbp_rop_dma_enable(int unit);
int arad_kbp_rop_dma_configure(int unit, int kbp_trans_len, int kbp_trans_zero_fill, int kbp_change_trans_endian);
int arad_kbp_rop_dma_sync(int unit);
int arad_kbp_rop_dma_test(int unit, int debug_print);

uint32 arad_kbp_rop_ad_write(
    int device_id,
    uint32 core,
    uint8 *wr_data,
    uint16 length);

uint32 arad_kbp_rop_uda_operation(
    int unit, uint32 core,
    uint32 *data,
    uint32 data_count);




#endif
