/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __JER_PP_KAPS_INCLUDED__

#define __JER_PP_KAPS_INCLUDED__





#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/kbp_legacy.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_portable.h>
#include <soc/kbp/alg_kbp/include/dma.h>
#include <soc/kbp/alg_kbp/include/kbp_hb.h>
#include <soc/dpp/JER/jer_drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>





#define JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES   (4)
#define JER_KAPS_MAX_NOF_KEY_SEGMENTS           (5)
#define JER_KAPS_AD_WIDTH_IN_BITS               (20)
#define JER_KAPS_AD_WIDTH_IN_BYTES              (3)

#define JER_KAPS_ENABLE_PRIVATE_DB(unit)        ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_private_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE_PUBLIC_DB(unit)         ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_public_ip_frwrd_table_size) > 0)
#define JER_KAPS_ENABLE_DMA(unit)               ((SOC_DPP_JER_CONFIG(unit)->pp.kaps_large_db_size) > 0)
#define JER_KAPS_ENABLE(unit)                   ((SOC_IS_JERICHO(unit)) && (JER_KAPS_ENABLE_PRIVATE_DB(unit) || JER_KAPS_ENABLE_PUBLIC_DB(unit) || JER_KAPS_ENABLE_DMA(unit)))

#define JER_KAPS_INRIF_WIDTH_IN_BITS            (15)
#define JER_KAPS_VRF_WIDTH_IN_BITS              (14)
#define JER_KAPS_INRIF_WIDTH_PADDING_IN_BITS    (1)

#define JER_KAPS_KEY_BUFFER_NOF_BYTES           (20)

#define JER_KAPS_TABLE_PREFIX_LENGTH            (2)
#define JER_KAPS_DYNAMIC_TABLE_PREFIX_LENGTH    (6)

#define JER_KAPS_IP_PUBLIC_INDEX                (JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID)

#define JER_KAPS_RPB_BLOCK_INDEX_START          (1)
#define JER_KAPS_RPB_BLOCK_INDEX_END            (SOC_IS_QAX(unit) ? 2 : 4)


#define JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_NUM   (8)
#define JER_KAPS_TABLE_PREFIX_MAX_LENGTH        (5)
#define JER_KAPS_DYNAMIC_TABLE_PREFIX_MAX_BIT   (JER_KAPS_TABLE_PREFIX_MAX_LENGTH - JER_KAPS_TABLE_PREFIX_LENGTH)


#define JER_KAPS_DMA_DB_NOF_ENTRIES             (1000)

#define JER_KAPS_DMA_DB_NOF_PMF_ENTRIES         (SOC_IS_JERICHO_PLUS_ONLY(unit) ? 16 * 1024 : (SOC_IS_QUX(unit) ? 1024 : 4096))
#define JER_KAPS_NOF_MAX_DMA_DB                 (32) 

#define JER_KAPS_DMA_DB_WIDTH_MAX               (128)
#define JER_KAPS_DMA_DB_WIDTH(_unit)            (SOC_IS_QUX(unit) ? 120 : (SOC_IS_QAX(_unit) ? 127 : (SOC_IS_JERICHO_PLUS(_unit) ? 120 : 128)))


#define JER_KAPS_TABLE_USE_DB_HANDLE            (0xFF)

#ifdef CRASH_RECOVERY_SUPPORT

#define JER_KAPS_IS_CR_MODE(unit) 0 
#define JER_KAPS_NV_MEMORY_SIZE (50*1024*1024)
#endif







typedef enum
{
    JER_KAPS_IP_CORE_0_PRIVATE_DB_ID = 0,
    JER_KAPS_IP_CORE_1_PRIVATE_DB_ID = 1,
    JER_KAPS_IP_CORE_0_PUBLIC_DB_ID  = 2,
    JER_KAPS_IP_CORE_1_PUBLIC_DB_ID  = 3,
    
    JER_KAPS_IP_NOF_DB        

} JER_KAPS_IP_DB_ID;




typedef enum
{
    JER_KAPS_IP_CORE_0_PRIVATE_IPV4_UC_TBL_ID  = 0, 
    JER_KAPS_IP_CORE_1_PRIVATE_IPV4_UC_TBL_ID  = 1, 
    JER_KAPS_IP_CORE_0_PRIVATE_IPV6_UC_TBL_ID  = 2,
    JER_KAPS_IP_CORE_1_PRIVATE_IPV6_UC_TBL_ID  = 3,
    JER_KAPS_IP_CORE_0_PRIVATE_IPV4_MC_TBL_ID  = 4,
    JER_KAPS_IP_CORE_0_PRIVATE_IPV6_MC_TBL_ID  = 5,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV4_UC_TBL_ID   = 6, 
    JER_KAPS_IP_CORE_1_PUBLIC_IPV4_UC_TBL_ID   = 7, 
    JER_KAPS_IP_CORE_0_PUBLIC_IPV6_UC_TBL_ID   = 8,
    JER_KAPS_IP_CORE_1_PUBLIC_IPV6_UC_TBL_ID   = 9,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV4_MC_TBL_ID   = 10,
    JER_KAPS_IP_CORE_0_PUBLIC_IPV6_MC_TBL_ID   = 11,
    
    JER_KAPS_IP_NOF_TABLES        

} JER_KAPS_IP_TBL_ID;

typedef enum
{
    JER_KAPS_IPV4_UC_SEARCH_ID  = 0,
    JER_KAPS_IPV6_UC_SEARCH_ID  = 1,
    JER_KAPS_IPV4_MC_SEARCH_ID  = 2,
    JER_KAPS_IPV6_MC_SEARCH_ID  = 3,
    
    JER_KAPS_NOF_SEARCHES        

} JER_KAPS_IP_SEARCH_ID;


typedef enum
{
    JER_KAPS_IPV6_MC_TABLE_PREFIX = 0,
    JER_KAPS_IPV6_UC_TABLE_PREFIX = 1,
    JER_KAPS_IPV4_MC_TABLE_PREFIX = 2,
    JER_KAPS_IPV4_UC_AND_NON_IP_TABLE_PREFIX = 3,

    JER_KAPS_NOF_TABLE_PREFIXES
}JER_KAPS_TABLE_PREFIX;











typedef struct kbp_instruction      JER_KAPS_INSTRUCTION;
typedef struct kbp_key              JER_KAPS_KEY;
typedef struct kbp_db               JER_KAPS_DB;
typedef struct kbp_ad_db            JER_KAPS_AD_DB;
typedef struct kbp_hb_db            JER_KAPS_HB_DB;
typedef struct kbp_dma_db           JER_KAPS_DMA_DB;


typedef struct
{
    
    char name[20];

    
    uint8 nof_bits;

    
    int type;

} JER_KAPS_KEY_SEGMENT;

typedef struct 
{
    
    uint32 nof_key_segments;

    
    JER_KAPS_KEY_SEGMENT key_segment[JER_KAPS_MAX_NOF_KEY_SEGMENTS];

} JER_KAPS_KEY_FIELDS;


typedef struct 
{
    
    uint8 valid;

     
    uint32 db_size;

    
    uint8 clone_of_db_id;

    
    JER_KAPS_DB *db_p;

    
    JER_KAPS_AD_DB *ad_db_p;

    
    JER_KAPS_HB_DB *hb_db_p;

    
    JER_KAPS_DMA_DB *dma_db_p;

} JER_KAPS_DB_CONFIG;


typedef struct
{
    
    uint8 db_id;

    
    uint8 clone_of_tbl_id;

    
    JER_KAPS_KEY_FIELDS key_fields;

    
    JER_KAPS_DB *tbl_p;

} JER_KAPS_TABLE_CONFIG;



typedef struct 
{
    
    
    uint8 valid_tables_num;

    
    
    uint8 max_tables_num;

    
    JER_KAPS_IP_TBL_ID tbl_id[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES];

    
    JER_KAPS_INSTRUCTION *inst_p;

} JER_KAPS_SEARCH_CONFIG;


typedef struct genericJerAppData
{
    struct kbp_allocator *dalloc_p;
    void *kaps_xpt_p;
    struct kbp_device *kaps_device_p;

} genericJerAppData;

typedef struct
{
    PARSER_HINT_PTR struct kbp_db       *db_p;
    PARSER_HINT_PTR struct kbp_ad_db    *ad_db_p;
    PARSER_HINT_PTR struct kbp_hb_db    *hb_db_p;
                    uint8                is_valid;
} JER_KAPS_DB_HANDLES;

typedef struct
{
    PARSER_HINT_PTR struct kbp_dma_db    *dma_db_p;
                    uint8                is_valid;
} JER_KAPS_DMA_DB_HANDLES;

typedef struct
{
    PARSER_HINT_PTR JER_KAPS_INSTRUCTION *inst_p;
} JER_KAPS_INSTRUCTION_HANDLES;

typedef struct
{
    PARSER_HINT_PTR struct kbp_db       *tbl_p;
} JER_KAPS_TABLE_HANDLES;













void jer_kaps_sw_init(int unit);

uint32 jer_kaps_init_app(int unit);

uint32 jer_kaps_deinit_app(int unit, int no_sync_flag);


int jer_kaps_sync(int unit);

#ifdef CRASH_RECOVERY_SUPPORT
uint32 jer_kaps_cr_transaction_cmd(int unit,
                                   uint8 is_start);
uint8 jer_kaps_cr_query_restore_status(int unit);
uint32 jer_kaps_cr_clear_restore_status(int unit);
uint32 jer_kaps_cr_db_commit(int unit,
                             uint32 tbl_idx);
#endif


int jer_kaps_autosync_set(int unit, int enable);


void jer_kaps_warmboot_register(int unit,
                                FILE *file_fp,
                                kbp_device_issu_read_fn read_fn, 
                                kbp_device_issu_write_fn write_fn);

void jer_kaps_db_get(int unit,
                     uint32 tbl_id,
                     struct kbp_db **db_p);

void jer_kaps_ad_db_get(int unit,
                        uint32 tbl_id,
                        struct kbp_ad_db **ad_db_p);

void jer_kaps_hb_db_get(int unit,
                        uint32 tbl_id,
                        struct kbp_hb_db **ad_hb_p);

void jer_kaps_dma_db_get(int unit,
                         uint32 db_id,
                         struct kbp_dma_db **db_p);

void jer_kaps_search_config_get(int unit,
								uint32 search_id,
								JER_KAPS_SEARCH_CONFIG *search_cfg_p);

int jer_kaps_search_generic(int unit, uint32 search_id, uint8 key[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][JER_KAPS_KEY_BUFFER_NOF_BYTES],
                            uint32 *return_is_matched, uint32 *return_prefix_len, uint8  return_payload[JER_KAPS_MAX_NUM_OF_PARALLEL_SEARCHES][BITS2BYTES(JER_KAPS_AD_WIDTH_IN_BITS)],
                            struct kbp_search_result *return_cmp_rslt);

void jer_kaps_table_config_get(int unit,
                               uint32 tbl_id,
                               JER_KAPS_TABLE_CONFIG *table_cfg_p);

void jer_kaps_ad_db_by_db_id_get(int unit,
                                 uint32 db_id,
                                 JER_KAPS_AD_DB **ad_db_p);

uint8 jer_kaps_clone_of_db_id_get(int unit,
                                  uint32 db_id);

void* jer_kaps_kaps_xpt_p_get(int unit);

void* jer_kaps_app_data_get(int unit);

uint32 jer_kaps_hb_timer(int unit);


#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif

