/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_ingress_traffic_mgmt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/QAX/qax_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/mbcm.h>




#define QAX_ITM_NOF_MANTISSA_BITS  (8)

#define QAX_ITM_WORDS_RESOLUTION    (16)

#define QAX_ITM_GRNT_BYTES_MAX (0xFFFFFFFF)  
#define QAX_ITM_GRNT_SRAM_BYTES_MAX (16 * 256 * 1024) 
#define QAX_ITM_GRNT_SRAM_PDS_MAX (32 * 1024) 

#define QAX_ITM_QUEUE_SIZE_BYTES_MAX (0xFFFFFFFF)  
#define QAX_ITM_QUEUE_SIZE_SRAM_BYTES_MAX (16 * 256 * 1024) 
#define QAX_ITM_QUEUE_SIZE_SRAM_PDS_MAX (32 * 1024) 

#define QAX_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX 100
#define QAX_ITM_Q_WRED_INFO_MIN_AVRG_TH_MAX (3 * 0x80000000)
#define QAX_ITM_Q_WRED_INFO_MAX_AVRG_TH_MAX (3 * 0x80000000)

#define QAX_ITM_SYS_RED_QUEUE_TH_MAX 15
#define QAX_ITM_SYS_RED_GLOB_RCS_RNG_THS_MAX 0xfff



#define QAX_ITM_NOF_TOTAL_BYTES(unit)      ((SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries.mnmc_0.size)  \
                                            * (SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size))  
                                                
#define QAX_ITM_NOF_SRAM_BYTES(unit)    (SOC_DPP_DEFS_GET(unit, ocb_memory_size) \
                                         * (1024 * 1024 / 8))  
                                                

#define QAX_ITM_NOF_BYTES(unit)         ((SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams != 0) ? \
                                            QAX_ITM_NOF_TOTAL_BYTES(unit) : QAX_ITM_NOF_SRAM_BYTES(unit))


#define QAX_ITM_NOF_SRAM_BUFFERS(unit) (SOC_DPP_DEFS_GET(unit, ocb_memory_size) \
                                                    * (1024 * 1024 / 8  \
                                                        / SOC_DPP_CONFIG(unit)->arad->init.ocb.databuffer_size)) 

#define QAX_ITM_NOF_SRAM_PDS(unit)      (SOC_DPP_DEFS_GET(unit, nof_sram_pds))

#define QAX_ITM_NOF_SRAM_PDBS(unit)     (QAX_ITM_NOF_SRAM_PDS(unit) / 2)

#define QAX_ITM_NOF_DRAM_BDBS(unit)     (SOC_DPP_DEFS_GET(unit, nof_dram_bds) / 8)


#define QAX_ITM_VSQ_PG_OFFSET_FIELD_SIZE  (48)


#define QAX_ITM_PB_VSQ_POOL_FC_NOF_PRIO (2)



#define QAX_ITM_SCHEDULER_DELTA_MAX                 (127)
#define QAX_ITM_SCHEDULER_DELTA_MIN                 (-128)
#define QAX_ITM_SCHEDULER_DELTA_NOF_BITS            (8)
#define QAX_HAP_NOF_ENTRIES                     (256)


typedef struct {
    soc_mem_t mem_id;
    soc_field_t field_id;
    int mantissa_bits; 
    int exp_bits;
    int factor;
} itm_mantissa_exp_threshold_info;


#define QAX_SRAM_PDS_IN_GUARANTEED       0x1
#define QAX_SRAM_WORDS_IN_GUARANTEED     0x2
#define QAX_WORDS_IN_GUARANTEED          0x4


typedef enum {
    QAX_VOQ_SRAM_PDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT          = 0,
    QAX_VOQ_SRAM_PDS_MAX_SIZE_REJECT_BIT                        = 1,
    QAX_VOQ_SRAM_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT        = 2,
    QAX_VOQ_SRAM_WORDS_MAX_SIZE_REJECT_BIT                      = 3,
    QAX_VOQ_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT             = 4,
    QAX_VOQ_WORDS_MAX_SIZE_REJECT_BIT                           = 5,
    QAX_VOQ_SYSTEM_RED_REJECT_BIT                               = 6,
    QAX_VOQ_WRED_REJECT_BIT                                     = 7,
    QAX_VOQ_DRAM_BLOCK_REJECT_BIT                               = 8,
    QAX_PB_VSQ_SRAM_PDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT       = 9,
    QAX_PB_VSQ_SRAM_PDS_MAX_SIZE_REJECT_BIT                     = 10,
    QAX_VSQ_D_SRAM_PDS_MAX_SIZE_REJECT_BIT                      = 11,
    QAX_VSQ_C_SRAM_PDS_MAX_SIZE_REJECT_BIT                      = 12,
    QAX_VSQ_B_SRAM_PDS_MAX_SIZE_REJECT_BIT                      = 13,
    QAX_VSQ_A_SRAM_PDS_MAX_SIZE_REJECT_BIT                      = 14,
    QAX_PB_VSQ_SRAM_BUFFERS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT   = 15,
    QAX_PB_VSQ_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT                 = 16,
    QAX_VSQ_D_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT                  = 17,
    QAX_VSQ_C_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT                  = 18,
    QAX_VSQ_B_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT                  = 19,
    QAX_VSQ_A_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT                  = 20,
    QAX_PB_VSQ_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT          = 21,
    QAX_PB_VSQ_WORDS_MAX_SIZE_REJECT_BIT                        = 22,
    QAX_VSQ_D_WORDS_MAX_SIZE_REJECT_BIT                         = 23,
    QAX_VSQ_C_WORDS_MAX_SIZE_REJECT_BIT                         = 24,
    QAX_VSQ_B_WORDS_MAX_SIZE_REJECT_BIT                         = 25,
    QAX_VSQ_A_WORDS_MAX_SIZE_REJECT_BIT                         = 26,
    QAX_VSQ_F_WORDS_WRED_REJECT_BIT                             = 27,
    QAX_VSQ_E_WORDS_WRED_REJECT_BIT                             = 28,
    QAX_VSQ_D_WORDS_WRED_REJECT_BIT                             = 29,
    QAX_VSQ_C_WORDS_WRED_REJECT_BIT                             = 30,
    QAX_VSQ_B_WORDS_WRED_REJECT_BIT                             = 31,
    QAX_VSQ_A_WORDS_WRED_REJECT_BIT                             = 32,
    QAX_DRAM_BDBS_OCCUPANCY_REJECT_BIT                          = 33,
    QAX_SRAM_BUFFERS_OCCUPANCY_REJECT_BIT                       = 34,
    QAX_SRAM_PDBS_OCCUPANCY_REJECT_BIT                          = 35,
    QAX_CFG_MAX_DP_LEVEL_REJECT_BIT                             = 36,
    QAX_CGM_NOF_ADMISSION_TESTS                                 = 37
} cgm_reject_admission_tests_e;



#define CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED          0x1
#define CGM_ITM_WORDS_PORT_PG_SHARED_BLOCKED        0x2
#define CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED        0x4
#define CGM_ITM_WORDS_PORT_PG_HEADROOM_BLOCKED      0x8
#define CGM_ITM_WORDS_VOQ_WORDS_IN_GRNTD            0x10
#define CGM_ITM_WORDS_VSQ_GRNTD_PRE_BLOCKED         0x20
#define CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED             0x40


#define CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED          0x1
#define CGM_ITM_SRAM_PORT_PG_SHARED_BLOCKED        0x2
#define CGM_ITM_SRAM_HEADROOM_EXTENSION_BLOCKED    0x4
#define CGM_ITM_SRAM_TOTAL_HEADROOM_BLOCKED        0x8
#define CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED         0x10
#define CGM_ITM_SRAM_PG_HEADROOM_NOMINAL_BLOCKED   0x20
#define CGM_ITM_SRAM_VOQ_IN_GRNTD                  0x40
#define CGM_ITM_SRAM_VSQ_GRNTD_PRE_BLOCKED         0x80
#define CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED             0x100

#define CGM_ITM_GRNTD_DP_MASK   0x0


#define CGM_ITM_RESOURCE_LIMIT_IS_STRICT  0





STATIC  uint32 
  _qax_itm_alpha_to_field(
     int     unit,
     int     alpha
  );

STATIC int 
  _qax_itm_field_to_alpha(
     int     unit,
     uint32 alpha_field
  );

STATIC int 
qax_itm_fc_global_thresholds_init(int unit);

STATIC int
qax_itm_fc_vsq_e_defaults_set(int unit);

STATIC int
qax_itm_drop_global_thresholds_init(int unit);

STATIC void
  _qax_itm_hw_data_to_wred_info (
    SOC_SAND_IN  int                                                  unit,
    SOC_SAND_IN  uint32                                               min_avrg_th,
    SOC_SAND_IN  uint32                                               max_avrg_th,
    SOC_SAND_IN  uint32                                               c1,
    SOC_SAND_IN  uint32                                               c2,
    SOC_SAND_IN  uint32                                               c3,
    SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                          *wred_param
  );

STATIC void
  _qax_itm_wred_info_to_hw_data (
     SOC_SAND_IN   int unit,
     SOC_SAND_IN   SOC_TMC_ITM_WRED_QT_DP_INFO                          *wred_param,
     SOC_SAND_IN   uint32                                               min_avrg_th_exact,
     SOC_SAND_IN   uint32                                               max_avrg_th_exact,
     SOC_SAND_OUT  uint32*                                              c1,
     SOC_SAND_OUT  uint32*                                              c2,
     SOC_SAND_OUT  uint32*                                              c3
  );

STATIC uint32 
  _qax_itm_mantissa_exp_field_set(
    int                  unit,
    itm_mantissa_exp_threshold_info* info,
    int round_up,
    void *data,
    uint32 threshold, 
    uint32* result_threshold
    );

STATIC int
  qax_itm_vsq_rjct_man_exp_set(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    soc_field_t                         field,
    uint32                              entry_offset,
    uint32                              threshold,
    uint32                              *result_threshold
  );

STATIC int
  qax_itm_vsq_rjct_man_exp_get(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    soc_field_t                         field,
    uint32                              entry_offset,
    uint32                              *result_threshold
  );

STATIC int
  qax_itm_vsq_rjct_fadt_set(
    int                              unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type,
    soc_mem_t                        mem,
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO fadt_fields,
    uint32                           entry_offset,
    SOC_TMC_FADT_INFO                fadt_info,
    SOC_TMC_FADT_INFO                *exact_fadt_info
  );

STATIC int
  qax_itm_vsq_rjct_fadt_get(
    int                              unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type,
    soc_mem_t                        mem,
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO fadt_fields,
    uint32                           entry_offset,
    SOC_TMC_FADT_INFO                *exact_fadt_info
  );

STATIC int
  qax_itm_vsq_pg_headroom_rjct_set(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    uint32                              entry_offset,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        headroom_info,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        *exact_headroom_info
  );

STATIC int
  qax_itm_vsq_pg_headroom_rjct_get(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    uint32                              entry_offset,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        *exact_headroom_info
  );

STATIC int
  qax_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx
  );

STATIC int 
  qax_itm_category_rngs_verify( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ); 

STATIC int
  soc_qax_itm_reserved_resource_init(
    int unit
  );

STATIC int
  qax_itm_resource_allocation_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     core_id,
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  ); 

STATIC int
  qax_itm_resource_allocation_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     core_id,
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  ); 

STATIC int
  qax_itm_vsq_fc_verify(
    SOC_SAND_IN int                        unit,
    SOC_SAND_IN SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN int                        pool_id,
    SOC_SAND_IN SOC_TMC_ITM_VSQ_FC_INFO    *info
  );






itm_mantissa_exp_threshold_info voq_guaranteed_th_mant_exp[] = {
    {
        CGM_VOQ_GRNTD_PRMSm,
        GRNTD_WORDS_THf,
        8, 
        5, 
        16 
    },
    {
        CGM_VOQ_GRNTD_PRMSm,
        GRNTD_SRAM_WORDS_THf,
        8, 
        4, 
        16 
    },
    
    {
        CGM_VOQ_GRNTD_PRMSm,
        GRNTD_SRAM_PDS_THf,
        8, 
        4, 
        1 
    }
};

itm_mantissa_exp_threshold_info voq_fadt_max_th_mant_exp[] = {
    {
        CGM_VOQ_WORDS_RJCT_PRMSm,
        VOQ_FADT_MAX_THf,
        8, 
        5, 
        16 
    },
    {
        CGM_VOQ_SRAM_WORDS_RJCT_PRMSm,
        VOQ_FADT_MAX_THf,
        8, 
        5, 
        16 
    },
    
    {
        CGM_VOQ_SRAM_PDS_RJCT_PRMSm,
        VOQ_FADT_MAX_THf,
        8, 
        5, 
        1 
    }
};

itm_mantissa_exp_threshold_info voq_fadt_min_th_mant_exp[] = {
   
    {  
         CGM_VOQ_WORDS_RJCT_PRMSm,
         VOQ_FADT_MIN_THf,
         8, 
         5, 
         16 
    },
    
    { 
        CGM_VOQ_SRAM_WORDS_RJCT_PRMSm,
        VOQ_FADT_MIN_THf,
        8, 
        5, 
        16 
    },
    
    {
        CGM_VOQ_SRAM_PDS_RJCT_PRMSm,
        VOQ_FADT_MIN_THf,
        8, 
        5, 
        1  
    }
};

itm_mantissa_exp_threshold_info voq_wred_ecn_max_th_mant_exp[] = 
{
    {  
        CGM_VOQ_WORDS_RJCT_PRMSm,
        VOQ_WRED_MAX_THf,
        8, 
        5, 
        16 
    },
    {  
        CGM_CNI_PRMSm,
        WRED_MAX_THf,
        8, 
        5, 
        16 
    }
};

itm_mantissa_exp_threshold_info voq_wred_ecn_min_th_mant_exp[] = 
{
    {  
         CGM_VOQ_WORDS_RJCT_PRMSm,
         VOQ_WRED_MIN_THf,
         8, 
         5, 
         16 
    },
    {  
        CGM_CNI_PRMSm,
        WRED_MIN_THf,
        8, 
        5, 
        16 
    }
};

itm_mantissa_exp_threshold_info voq_wred_ecn_max_size_th_mant_exp = 
{  
        CGM_CNI_PRMSm,
        MAX_SIZE_THf,
        8, 
        5, 
        16 
};



itm_mantissa_exp_threshold_info voq_dram_bound_qsize_recovery_th_mant_exp = 
{
    CGM_VOQ_DRAM_BOUND_PRMSm,
    QSIZE_RECOVERY_THf,
    8, 
    4, 
    16 
};



STATIC  uint32 _qax_itm_alpha_to_field(
     int     unit,
     int     alpha)
{
    uint32 value_mask = 0x7;
    uint32 sign;
    uint32 alpha_field;

    
    if (alpha >= 0) {
        sign = 1;
    }
    else {
        sign = 0;
        alpha = -alpha;
    }
    alpha_field = (sign << 3) | (alpha & value_mask);

    return alpha_field;
}

STATIC int _qax_itm_field_to_alpha(
     int     unit,
     uint32 alpha_field)
{
    uint32 value_mask = 0x7;
    uint32 sign_mask = 1 << 3;
    int alpha;

    
 
    alpha = alpha_field & value_mask;
    if ((alpha_field & sign_mask) == 0) {
        alpha = -(alpha);
    }

    return alpha;
}


STATIC uint32  _qax_itm_mantissa_exp_field_set(
    int                  unit,
    itm_mantissa_exp_threshold_info* info,
    int round_up,
    void *data,
    uint32 threshold, 
    uint32* result_threshold
    )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit, info->mem_id, info->field_id, 
                                                      info->mantissa_bits,
                                                      info->exp_bits,
                                                      info->factor,
                                                      round_up,
                                                      data,
                                                      threshold, 
                                                      result_threshold));
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC void  _qax_itm_mantissa_exp_field_get(
    int                  unit,
    itm_mantissa_exp_threshold_info* info,
    void *data,
    uint32* result_threshold
    )
{
    tmc_itm_mantissa_exp_field_get(unit, info->mem_id, info->field_id, 
                                   info->mantissa_bits,
                                   info->exp_bits,
                                   info->factor,
                                   data,
                                   result_threshold);
}




int qax_itm_cgm_guaranteed_reject_mask_create(int unit, int is_voq, int is_over_voq, int is_over_vsq, int words_in_guaranteed, int sram_words_in_guaranteed, int sram_pds_in_guaranteed, SHR_BITDCL* mem_mask)
{
    int in_guaranteed = words_in_guaranteed || sram_words_in_guaranteed || sram_pds_in_guaranteed;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(mem_mask);

    SHR_BITCLR_RANGE(mem_mask, 0, QAX_CGM_NOF_ADMISSION_TESTS);
    SHR_BITWRITE(mem_mask, QAX_VOQ_SRAM_PDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,          (!((is_over_voq && in_guaranteed) || sram_pds_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_SRAM_PDS_MAX_SIZE_REJECT_BIT,                        (!((is_over_voq && in_guaranteed) || sram_pds_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_SRAM_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,        (!((is_over_voq && in_guaranteed) || sram_words_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_SRAM_WORDS_MAX_SIZE_REJECT_BIT,                      (!((is_over_voq && in_guaranteed) || sram_words_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,             (!((is_over_voq && in_guaranteed) || words_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_WORDS_MAX_SIZE_REJECT_BIT,                           (!((is_over_voq && in_guaranteed) || words_in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_SYSTEM_RED_REJECT_BIT,                               1);
    SHR_BITWRITE(mem_mask, QAX_VOQ_WRED_REJECT_BIT,                                     (!(is_voq && in_guaranteed)));
    SHR_BITWRITE(mem_mask, QAX_VOQ_DRAM_BLOCK_REJECT_BIT,                               1);
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_SRAM_PDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,       (!(is_voq ? (is_over_vsq && sram_pds_in_guaranteed) : ((is_over_vsq && in_guaranteed) || sram_pds_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_SRAM_PDS_MAX_SIZE_REJECT_BIT,                     (!(is_voq ? (is_over_vsq && sram_pds_in_guaranteed) : ((is_over_vsq && in_guaranteed) || sram_pds_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_VSQ_D_SRAM_PDS_MAX_SIZE_REJECT_BIT,                      !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_C_SRAM_PDS_MAX_SIZE_REJECT_BIT,                      !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_B_SRAM_PDS_MAX_SIZE_REJECT_BIT,                      !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_A_SRAM_PDS_MAX_SIZE_REJECT_BIT,                      !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_SRAM_BUFFERS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,   (!(is_voq ? (is_over_vsq && sram_words_in_guaranteed) : ((is_over_vsq && in_guaranteed) || sram_words_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT,                 (!(is_voq ? (is_over_vsq && sram_words_in_guaranteed) : ((is_over_vsq && in_guaranteed) || sram_words_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_VSQ_D_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT,                  !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_C_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT,                  !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_B_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT,                  !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_A_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT,                  !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_WORDS_TOTAL_SHARED_OCCUPANCY_REJECT_BIT,          (!(is_voq ? (is_over_vsq && words_in_guaranteed) : ((is_over_vsq && in_guaranteed) || words_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_PB_VSQ_WORDS_MAX_SIZE_REJECT_BIT,                        (!(is_voq ? (is_over_vsq && words_in_guaranteed) : ((is_over_vsq && in_guaranteed) || words_in_guaranteed))));
    SHR_BITWRITE(mem_mask, QAX_VSQ_D_WORDS_MAX_SIZE_REJECT_BIT,                         !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_C_WORDS_MAX_SIZE_REJECT_BIT,                         !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_B_WORDS_MAX_SIZE_REJECT_BIT,                         !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_A_WORDS_MAX_SIZE_REJECT_BIT,                         !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_F_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_E_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_D_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_C_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_B_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_VSQ_A_WORDS_WRED_REJECT_BIT,                             !in_guaranteed);
    SHR_BITWRITE(mem_mask, QAX_DRAM_BDBS_OCCUPANCY_REJECT_BIT,                          1);
    SHR_BITWRITE(mem_mask, QAX_SRAM_BUFFERS_OCCUPANCY_REJECT_BIT,                       1);
    SHR_BITWRITE(mem_mask, QAX_SRAM_PDBS_OCCUPANCY_REJECT_BIT,                          1);
    SHR_BITWRITE(mem_mask, QAX_CFG_MAX_DP_LEVEL_REJECT_BIT,                             1);
        
exit:
    SOCDNX_FUNC_RETURN;
}


int qax_itm_admission_tests_defaults_set (int unit)
{
    int enforce_admission_test_loosly = 0; 
    int voq_over_voq;
    int voq_over_vsq;
    int vsq_over_voq;
    int vsq_over_vsq;
    int words_in_guaranteed;
    int sram_words_in_guaranteed;
    int sram_pds_in_guaranteed;
    int mem_index;
    int max_mem_index;
    soc_mem_t mem;
    SHR_BITDCLNAME(mem_mask, QAX_CGM_NOF_ADMISSION_TESTS);

    SOCDNX_INIT_FUNC_DEFS;

    enforce_admission_test_loosly = (SOC_DPP_CONFIG(unit)->jer->tm.cgm_mgmt_guarantee_mode == SOC_TMC_ITM_CGM_MGMT_GUARANTEE_LOOSE);
    
    voq_over_voq = voq_over_vsq = vsq_over_voq = vsq_over_vsq = enforce_admission_test_loosly;

    
    max_mem_index = soc_mem_index_max(unit, CGM_VOQ_GRNTD_RJCT_MASKm);
    mem = CGM_VOQ_GRNTD_RJCT_MASKm;
    for (mem_index = 0; mem_index <= max_mem_index; ++mem_index) 
    {
        words_in_guaranteed = mem_index & QAX_WORDS_IN_GUARANTEED;
        sram_words_in_guaranteed = mem_index & QAX_SRAM_WORDS_IN_GUARANTEED;
        sram_pds_in_guaranteed = mem_index & QAX_SRAM_PDS_IN_GUARANTEED;
        SOCDNX_IF_ERR_EXIT( qax_itm_cgm_guaranteed_reject_mask_create(unit, 1, voq_over_voq, voq_over_vsq, words_in_guaranteed, sram_words_in_guaranteed, sram_pds_in_guaranteed, mem_mask));
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index, mem_mask);
    }

    
    max_mem_index = soc_mem_index_max(unit, CGM_VSQ_GRNTD_RJCT_MASKm);
    mem = CGM_VSQ_GRNTD_RJCT_MASKm;
    for (mem_index = 0; mem_index <= max_mem_index; ++mem_index) 
    {
        words_in_guaranteed = mem_index & QAX_WORDS_IN_GUARANTEED;
        sram_words_in_guaranteed = mem_index & QAX_SRAM_WORDS_IN_GUARANTEED;
        sram_pds_in_guaranteed = mem_index & QAX_SRAM_PDS_IN_GUARANTEED;
        SOCDNX_IF_ERR_EXIT( qax_itm_cgm_guaranteed_reject_mask_create(unit, 0, vsq_over_voq, vsq_over_vsq, words_in_guaranteed, sram_words_in_guaranteed, sram_pds_in_guaranteed, mem_mask));
        soc_mem_write(unit, mem, MEM_BLOCK_ALL, mem_index, mem_mask);
    }

    
    SHR_BITSET_RANGE(mem_mask, 0, QAX_CGM_NOF_ADMISSION_TESTS);
    WRITE_CGM_PB_VSQ_RJCT_MASKm(unit, MEM_BLOCK_ALL, 0, mem_mask);
    SHR_BITCLR_RANGE(mem_mask, 0, QAX_DRAM_BDBS_OCCUPANCY_REJECT_BIT);
    SHR_BITSET(mem_mask, QAX_PB_VSQ_SRAM_PDS_MAX_SIZE_REJECT_BIT);
    SHR_BITSET(mem_mask, QAX_PB_VSQ_SRAM_BUFFERS_MAX_SIZE_REJECT_BIT);
    SHR_BITSET(mem_mask, QAX_PB_VSQ_WORDS_MAX_SIZE_REJECT_BIT);
    WRITE_CGM_PB_VSQ_RJCT_MASKm(unit, MEM_BLOCK_ALL, 8, mem_mask);

    
    SHR_BITSET_RANGE(mem_mask, 0, QAX_CGM_NOF_ADMISSION_TESTS);
    WRITE_CGM_PP_RJCT_MASKm(unit, MEM_BLOCK_ALL, 0, mem_mask);
    SHR_BITCLR_RANGE(mem_mask, 0, QAX_CGM_NOF_ADMISSION_TESTS);
    SHR_BITSET_RANGE(mem_mask, 0, QAX_CGM_NOF_ADMISSION_TESTS);
    SHR_BITCLR(mem_mask, QAX_VOQ_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_A_WORDS_MAX_SIZE_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_B_WORDS_MAX_SIZE_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_C_WORDS_MAX_SIZE_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_D_WORDS_MAX_SIZE_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_A_WORDS_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_B_WORDS_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_C_WORDS_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_D_WORDS_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_E_WORDS_WRED_REJECT_BIT);
    SHR_BITCLR(mem_mask, QAX_VSQ_F_WORDS_WRED_REJECT_BIT);
    WRITE_CGM_PP_RJCT_MASKm(unit, MEM_BLOCK_ALL, 1, mem_mask);

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qax_itm_rjct_setting_defaults_set(int unit)
{
    uint32 data = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_RJCT_SETTINGSr(unit, SOC_CORE_ALL, &data));
    
    soc_reg_field_set(unit, CGM_VOQ_RJCT_SETTINGSr, &data, VOQ_GRNTD_DP_MASKf, CGM_ITM_GRNTD_DP_MASK);
    
    soc_reg_field_set(unit, CGM_VOQ_RJCT_SETTINGSr, &data, VOQ_WORDS_IN_GRNTD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_VOQ_RJCT_SETTINGSr, &data, VOQ_SRAM_WORDS_IN_GRNTD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_RJCT_SETTINGSr(unit, SOC_CORE_ALL, data));

    SOCDNX_IF_ERR_EXIT(READ_CGM_PB_VSQ_RJCT_SETTINGSr(unit, SOC_CORE_ALL, &data));
    
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_GRNTD_DP_MASKf, CGM_ITM_GRNTD_DP_MASK);
    
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_WORDS_GRNTD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_SRAM_BUFFERS_GRNTD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PORT_WORDS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PORT_WORDS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PG_WORDS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PG_WORDS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_WORDS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_WORDS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PORT_SRAM_BUFFERS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PORT_SRAM_BUFFERS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PG_SRAM_BUFFERS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_PG_SRAM_BUFFERS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_SRAM_BUFFERS_SHRD_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    soc_reg_field_set(unit, CGM_PB_VSQ_RJCT_SETTINGSr, &data, PB_VSQ_SRAM_BUFFERS_HDRM_IS_STRICTf, CGM_ITM_RESOURCE_LIMIT_IS_STRICT);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_RJCT_SETTINGSr(unit, SOC_CORE_ALL, data));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qax_itm_rjct_map_defaults_set(int unit)
{
    int reject_bit = -1;
    int headroom_bits = 0;
    int sram_headroom_bits = 0;
    int shared_bits = 0;
    int sram_shared_bits = 0;
    int is_resource_limit_strict = CGM_ITM_RESOURCE_LIMIT_IS_STRICT;
    soc_reg_above_64_val_t rjct_bitmap;
    soc_reg_above_64_val_t sram_rjct_bitmap;
    SOCDNX_INIT_FUNC_DEFS;


    
    
    
    if (SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams != 0) {
        SOC_REG_ABOVE_64_CLEAR(rjct_bitmap);

        
        headroom_bits = CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED | CGM_ITM_WORDS_PORT_PG_HEADROOM_BLOCKED;

        if (1 == is_resource_limit_strict) {
            
            
            reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_PRE_BLOCKED | CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED | headroom_bits;
            SHR_BITSET(rjct_bitmap, reject_bit);
            reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_PRE_BLOCKED | CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED | CGM_ITM_WORDS_PORT_PG_SHARED_BLOCKED | headroom_bits;
            SHR_BITSET(rjct_bitmap, reject_bit);
        }

        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_PORT_PG_SHARED_BLOCKED | headroom_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);
        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED | headroom_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);
        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED | CGM_ITM_WORDS_PORT_PG_SHARED_BLOCKED | headroom_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);

        
        shared_bits = CGM_ITM_WORDS_TOTAL_SHARED_BLOCKED | CGM_ITM_WORDS_PORT_PG_SHARED_BLOCKED;
        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED | shared_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);
        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_PORT_PG_HEADROOM_BLOCKED | shared_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);
        reject_bit = CGM_ITM_WORDS_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_PORT_PG_HEADROOM_BLOCKED | CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED | shared_bits;
        SHR_BITSET(rjct_bitmap, reject_bit);

        
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_WORDS_RJCT_MAPm(unit, MEM_BLOCK_ALL, 0, rjct_bitmap));
    }

    
    
    
    SOC_REG_ABOVE_64_CLEAR(sram_rjct_bitmap);
    
    sram_headroom_bits = CGM_ITM_SRAM_HEADROOM_EXTENSION_BLOCKED | CGM_ITM_SRAM_TOTAL_HEADROOM_BLOCKED | CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED | CGM_ITM_SRAM_PG_HEADROOM_NOMINAL_BLOCKED;

    if (1 == is_resource_limit_strict) {
        
        
        reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_PRE_BLOCKED | CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED | sram_headroom_bits;
        SHR_BITSET(sram_rjct_bitmap, reject_bit);
        reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_PRE_BLOCKED | CGM_ITM_SRAM_PORT_PG_SHARED_BLOCKED | CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED | sram_headroom_bits;
        SHR_BITSET(sram_rjct_bitmap, reject_bit);
    }
    

    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_PORT_PG_SHARED_BLOCKED | sram_headroom_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED | sram_headroom_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_PORT_PG_SHARED_BLOCKED | CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED | sram_headroom_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);

    
    sram_shared_bits = CGM_ITM_SRAM_TOTAL_SHARED_BLOCKED | CGM_ITM_SRAM_PORT_PG_SHARED_BLOCKED;
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_TOTAL_HEADROOM_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_PG_HEADROOM_NOMINAL_BLOCKED | CGM_ITM_SRAM_HEADROOM_EXTENSION_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED | CGM_ITM_SRAM_PG_HEADROOM_NOMINAL_BLOCKED | CGM_ITM_SRAM_HEADROOM_EXTENSION_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED | CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);
    reject_bit = CGM_ITM_SRAM_VSQ_GRNTD_BLOCKED | CGM_ITM_WORDS_TOTAL_HEADROOM_BLOCKED | CGM_ITM_SRAM_PORT_HEADROOM_BLOCKED | CGM_ITM_SRAM_PG_HEADROOM_NOMINAL_BLOCKED | CGM_ITM_SRAM_HEADROOM_EXTENSION_BLOCKED | sram_shared_bits;
    SHR_BITSET(sram_rjct_bitmap, reject_bit);

    
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_SRAM_BUFFERS_RJCT_MAPm(unit, MEM_BLOCK_ALL, 0, sram_rjct_bitmap));

    
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_SRAM_PDS_RJCT_MAPm(unit, MEM_BLOCK_ALL, 0, sram_rjct_bitmap));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int qax_itm_scheduler_compensation_init(
    SOC_SAND_IN  int unit
    )
{
    int index;
    uint32 table_data[10];
    uint32 data;
    uint64 data64;
    int cgm_delta_signs_reg;

    SOCDNX_INIT_FUNC_DEFS;

    
    sal_memset(table_data, 0x0, sizeof(table_data));
    for (index = 0; index < 32; index++) { 
        
        if (SOC_TMC_ITM_COMPENSATION_LEGACY_MODE(unit)) { 
            

            
            soc_mem_field32_set(unit, CGM_SCH_HCMm, table_data, MASKf, 0x4); 
        } else {
            
            
            
            soc_mem_field32_set(unit, CGM_SCH_HCMm, table_data, MASKf, 0xf); 
        } 
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_SCH_HCMm(unit, MEM_BLOCK_ALL, index, table_data));
    }

    for (index = 0; index < 8; index++) { 

        
        
        SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_HCPm(unit, MEM_BLOCK_ANY , index, table_data));
        soc_mem_field32_set(unit, CGM_VOQ_HCPm, table_data, SCH_PROFILEf, index); 
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_HCPm(unit, MEM_BLOCK_ALL, index, table_data));
    }

    
    if (SOC_IS_QUX(unit)) {
        cgm_delta_signs_reg = CGM_REG_04F2r; 
    } else {
        cgm_delta_signs_reg = CGM_REG_04F5r; 
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, &data)); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data, FIELD_7_7f, TRUE); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data, FIELD_8_8f, TRUE); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, data));

    
    
    COMPILER_64_ZERO(data64);
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &data64)); 
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_TRUNCATE_MODEf, 0x2);
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_APPEND_MODEf, 0x2);
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, data64));

exit:
    SOCDNX_FUNC_RETURN;

}


STATIC int qax_itm_crps_compensation_init(
    SOC_SAND_IN  int unit
    )
{
    uint32 data32=0, data = 0, data1, data2, data3, data4;
    int core_index, index;
    int cgm_delta_signs_reg;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    
    data1 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[0] == 0) ? 0xE : 0xF;
    data2 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[1] == 0) ? 0xE : 0xF;
     
    data3 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[0] == 0) ? 0xE : 0xF;
    data4 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[1] == 0) ? 0xE : 0xF;             
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index)
    {
        for (index = 0; index < 32; index++) {
           
            SOCDNX_IF_ERR_EXIT(READ_CGM_IRPP_CTR_HCMm(unit, CGM_BLOCK(unit, core_index),index, &data));
            soc_mem_field32_set(unit, CGM_IRPP_CTR_HCMm, &data, MASKf, data1);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_IRPP_CTR_HCMm(unit, CGM_BLOCK(unit, core_index), index, &data));

            
            SOCDNX_IF_ERR_EXIT(READ_CGM_IRPP_CTR_HCMm(unit, CGM_BLOCK(unit, core_index), index+32, &data));
            soc_mem_field32_set(unit, CGM_IRPP_CTR_HCMm, &data, MASKf, data2);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_IRPP_CTR_HCMm(unit, CGM_BLOCK(unit, core_index), index+32, &data));

             
            SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_HCMm(unit, CGM_BLOCK(unit, core_index),index, &data32));
            soc_mem_field32_set(unit, CGM_TM_CTR_HCMm, &data32, MASKf, data3);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_TM_CTR_HCMm(unit, CGM_BLOCK(unit, core_index),index, &data32));

            
            SOCDNX_IF_ERR_EXIT(READ_CGM_TM_CTR_HCMm(unit, CGM_BLOCK(unit, core_index),index+32, &data32));
            soc_mem_field32_set(unit, CGM_TM_CTR_HCMm, &data32, MASKf, data4);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_TM_CTR_HCMm(unit, CGM_BLOCK(unit, core_index),index+32, &data32));
        }
    }

    
    if (SOC_IS_QUX(unit)) {
        cgm_delta_signs_reg = CGM_REG_04F2r; 
    } else {
        cgm_delta_signs_reg = CGM_REG_04F5r; 
    }
    data = 0;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, &data)); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data, FIELD_3_3f, TRUE); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data, FIELD_4_4f, TRUE); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, data));

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int qax_itm_stif_compensation_init(
    SOC_SAND_IN  int unit
    )
{
    uint32 data32;
    int index;
    int cgm_delta_signs_reg;
    SOCDNX_INIT_FUNC_DEFS;
    
    
    for (index = 0; index < 32; index++) {
            SOCDNX_IF_ERR_EXIT(READ_CGM_STAT_HCMm(unit, CGM_BLOCK(unit, SOC_CORE_ALL),index, &data32));
            soc_mem_field32_set(unit, CGM_STAT_HCMm, &data32, MASKf, 0xF);
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_STAT_HCMm(unit, CGM_BLOCK(unit, SOC_CORE_ALL),index, &data32));
    }

    
    if (SOC_IS_QUX(unit)) {
        cgm_delta_signs_reg = CGM_REG_04F2r; 
    } else {
        cgm_delta_signs_reg = CGM_REG_04F5r; 
    }
    data32 = 0;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, &data32)); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data32, FIELD_5_5f, TRUE); 
    soc_reg_field_set(unit, cgm_delta_signs_reg, &data32, FIELD_6_6f, TRUE); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, cgm_delta_signs_reg, REG_PORT_ANY, 0, data32));

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_init(
    SOC_SAND_IN  int  unit
  )
{
    uint32                 data = 0;
    soc_reg_above_64_val_t above64;
    uint64                 val64;
    int                    res;
    int                    core_index;
    SOC_TMC_ITM_GUARANTEED_RESOURCE guaranteed_resources; 
    SOC_TMC_ITM_INGRESS_CONGESTION_MGMT ingress_congestion_mgmt;
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE *resource = NULL;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type;

    SOCDNX_INIT_FUNC_DEFS;

    data = 0x0;
    if (!SOC_IS_QUX(unit)) {
    soc_mem_field32_set(unit, IPS_CRVSm, &data, CR_VAL_BMPf, 0x0); 
    res = arad_fill_table_with_entry(unit, IPS_CRVSm, MEM_BLOCK_ANY, &data);
    SOCDNX_IF_ERR_EXIT(res);

    data = 0;
    res = READ_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, &data); 
    SOCDNX_IF_ERR_EXIT(res);
    soc_reg_field_set(unit, IPS_CREDIT_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
    res = WRITE_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, data); 
    SOCDNX_IF_ERR_EXIT(res);
    }

    SOC_REG_ABOVE_64_CLEAR(above64); 
    soc_reg_above_64_field32_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, above64, MUL_PKT_DEQf, 0xf0f0);
    soc_reg_above_64_field32_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, above64, MUL_PKT_DEQ_BYTESf, 0x10);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, REG_PORT_ANY, 0, above64));

    data = 0;
    soc_reg_field_set(unit, CGM_FIFO_SIZESr, &data, UC_FIFO_SIZEf, 256);
    soc_reg_field_set(unit, CGM_FIFO_SIZESr, &data, MCH_FIFO_SIZEf, 256);
    soc_reg_field_set(unit, CGM_FIFO_SIZESr, &data, MCL_FIFO_SIZEf, 256);
    res = WRITE_CGM_FIFO_SIZESr(unit, SOC_CORE_ALL, data); 
    SOCDNX_IF_ERR_EXIT(res);

    
    COMPILER_64_SET(val64, 0xFFFFF, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_PRG_CTR_SETTINGSr(unit, SOC_CORE_ALL, val64));

    
    SOCDNX_IF_ERR_EXIT(READ_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, &val64));
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FAP_PORTf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FWD_ACTIONf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_LB_KEY_EXT_ENf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FWDACTION_TDMf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_DP_ENf, 0xff);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, SNP_STAMP_TRAP_CODEf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_CNI_BITf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_USER_DEFINED_LSBf, (64-9)*8+4); 
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, BACKWARD_TC_ENf, 0x0);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, BACKWARD_DP_ENf, 0x0);
    if (SOC_DPP_CONFIG(unit)->arad->init.mirror_stamp_sys_dsp_ext) 
    {
        
        soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_DSP_EXT_ENf, 0);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, val64));

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &data));
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &data));
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &data));

    
    SOCDNX_IF_ERR_EXIT(READ_IPS_AVOID_DRAM_CAM_FULLr(unit, &data));
    soc_reg_field_set(unit, IPS_AVOID_DRAM_CAM_FULLr, &data, FIELD_0_0f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_AVOID_DRAM_CAM_FULLr(unit, data));

    SOCDNX_IF_ERR_EXIT(READ_IPS_AVOID_FABRIC_CAM_FULLr(unit, &data));
    soc_reg_field_set(unit, IPS_AVOID_FABRIC_CAM_FULLr, &data, FIELD_0_0f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_AVOID_FABRIC_CAM_FULLr(unit, data));

    
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
        res = sw_state_access[unit].dpp.soc.qax.tm.guaranteed_info.get(unit, core_index, &guaranteed_resources);
        SOCDNX_IF_ERR_EXIT(res);

        

        
        guaranteed_resources.guaranteed[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].total = QAX_ITM_NOF_BYTES(unit) / QAX_ITM_WORDS_RESOLUTION;

        
        guaranteed_resources.guaranteed[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES].total = QAX_ITM_NOF_SRAM_BYTES(unit) / QAX_ITM_WORDS_RESOLUTION;

        
        guaranteed_resources.guaranteed[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].total = QAX_ITM_NOF_SRAM_PDS(unit);

        
        res = sw_state_access[unit].dpp.soc.qax.tm.guaranteed_info.set(unit, core_index, &guaranteed_resources);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VSQ_SETTINGSr(unit, SOC_CORE_ALL, &data));
    
    soc_reg_field_set(unit, CGM_VSQ_SETTINGSr, &data, PB_VSQ_MC_COUNT_ONCEf, 1);
    
    soc_reg_field_set(unit, CGM_VSQ_SETTINGSr, &data, PB_VSQ_PG_TC_WIDTHf, 0x3);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VSQ_SETTINGSr(unit, SOC_CORE_ALL, data));

    
    res = soc_qax_itm_reserved_resource_init(unit);
    SOCDNX_IF_ERR_EXIT(res);

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
        res = sw_state_access[unit].dpp.soc.qax.tm.lossless_pool_id.set(unit, core_index, 0);
        SOCDNX_IF_ERR_EXIT(res);

        ingress_congestion_mgmt.global[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].total = QAX_ITM_NOF_BYTES(unit);
        ingress_congestion_mgmt.global[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].total = QAX_ITM_NOF_SRAM_BUFFERS(unit);
        ingress_congestion_mgmt.global[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].total = QAX_ITM_NOF_SRAM_PDS(unit) - 1; 

        
        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            resource = &ingress_congestion_mgmt.global[rsrc_type];
            resource->pool_0 = resource->total; 
            resource->pool_1 = 0;
            resource->headroom = 0;
            resource->nominal_headroom = 0;
            resource->reserved = 0;
        }

        res = qax_itm_global_resource_allocation_set(unit, core_index, &ingress_congestion_mgmt);
        SOCDNX_IF_ERR_EXIT(res);
    }
	res = jer_itm_setup_dp_map(unit);
    SOCDNX_IF_ERR_EXIT(res);  

    
    SOCDNX_IF_ERR_EXIT(qax_itm_drop_global_thresholds_init(unit));
	
    
    SOCDNX_IF_ERR_EXIT(qax_itm_fc_global_thresholds_init(unit));

    
    SOCDNX_IF_ERR_EXIT(qax_itm_fc_vsq_e_defaults_set(unit));

    
    SOCDNX_IF_ERR_EXIT(qax_itm_rjct_setting_defaults_set(unit));
    SOCDNX_IF_ERR_EXIT(qax_itm_rjct_map_defaults_set(unit));
    SOCDNX_IF_ERR_EXIT(qax_itm_admission_tests_defaults_set(unit));

    
    SOCDNX_IF_ERR_EXIT(qax_itm_scheduler_compensation_init(unit));
    SOCDNX_IF_ERR_EXIT(qax_itm_crps_compensation_init(unit));
    SOCDNX_IF_ERR_EXIT(qax_itm_stif_compensation_init(unit));

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_TAR_FIFO_RJCT_THm(unit, MEM_BLOCK_ANY, 0, &data));
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, &data, MIRROR_RJCT_THf, 0x3ff);
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, &data, SNOOP_RJCT_THf, 0x3ff);
    soc_mem_field32_set(unit, CGM_TAR_FIFO_RJCT_THm, &data, FWD_RJCT_THf, 0x3ff);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, CGM_TAR_FIFO_RJCT_THm, MEM_BLOCK_ALL, &data));

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qax_itm_fc_global_thresholds_init(int unit)
{
    SOC_TMC_ITM_GLOB_RCS_FC_TH glbl_fc, glbl_fc_exact;
    int res = SOC_E_NONE;
    soc_reg_above_64_val_t above64;
    ARAD_MGMT_INIT *init;
    int index = 0, priority = 0;
    uint32 threshold = 0, dummy = 0;
    itm_mantissa_exp_threshold_info fc_info;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
    SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

    
    if (init->dram.nof_drams != 0) {
        
        glbl_fc.bdbs.hp.set       = 192;
        glbl_fc.bdbs.hp.clear     = 202;
        glbl_fc.bdbs.lp.set       = 256;
        glbl_fc.bdbs.lp.clear     = 266;
    }

    
    glbl_fc.ocb.hp.set = 220;
    glbl_fc.ocb.hp.clear = 270;
    glbl_fc.ocb.lp.set = 270;
    glbl_fc.ocb.lp.clear = 320;

    glbl_fc.ocb_pdb.hp.set = 220;
    glbl_fc.ocb_pdb.hp.clear = 270;
    glbl_fc.ocb_pdb.lp.set = 270;
    glbl_fc.ocb_pdb.lp.clear = 320;

    
    glbl_fc.hdrm.hp.set = 220;
    glbl_fc.hdrm.hp.clear = 270;

    glbl_fc.hdrm_pd.hp.set = 220;
    glbl_fc.hdrm_pd.hp.clear = 270;

    res = arad_itm_glob_rcs_fc_set_unsafe(unit, &glbl_fc, &glbl_fc_exact);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    
    for (priority = 0; priority < QAX_ITM_PB_VSQ_POOL_FC_NOF_PRIO; ++priority) {

        fc_info.mem_id = CGM_PB_VSQ_POOL_FC_THm;
        fc_info.mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
        fc_info.factor = 1;

        
        index = 2 * priority;

        SOCDNX_IF_ERR_EXIT(READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ALL, index, above64));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max);

        fc_info.field_id = WORDS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = WORDS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max);

        fc_info.field_id = SRAM_BUFFERS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = SRAM_BUFFERS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max);

        fc_info.field_id = SRAM_PDS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = SRAM_PDS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ALL, index, above64));


        
        index = 2 * priority + 1;

        SOCDNX_IF_ERR_EXIT(READ_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ALL, index, above64));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_byte_size_max);

        fc_info.field_id = WORDS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = WORDS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_size_max);

        fc_info.field_id = SRAM_BUFFERS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = SRAM_BUFFERS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        
        threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p1_pd_size_max);

        fc_info.field_id = SRAM_PDS_SET_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        fc_info.field_id = SRAM_PDS_CLR_THf;
        fc_info.exp_bits = soc_mem_field_length(unit, fc_info.mem_id, fc_info.field_id) - fc_info.mantissa_bits;
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &fc_info, 1, above64, threshold, &dummy));

        SOCDNX_IF_ERR_EXIT(WRITE_CGM_PB_VSQ_POOL_FC_THm(unit, MEM_BLOCK_ALL, index, above64));
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC
int qax_itm_fc_vsq_e_defaults_set(int unit)
{
    int core_id = 0;
    uint32 src_pp_port = 0;

    SOCDNX_INIT_FUNC_DEFS;
    for (core_id = 0; core_id < SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores; core_id++)
    {
        for (src_pp_port = 0; src_pp_port < SOC_DPP_DEFS_GET(unit, nof_pp_ports_per_core); src_pp_port++){
            
            SOCDNX_IF_ERR_EXIT
                (qax_itm_src_vsqs_mapping_set(unit, core_id, src_pp_port, 0, 0, 0 ));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int qax_itm_drop_global_thresholds_init(int unit)
{
    SOC_TMC_ITM_GLOB_RCS_DROP_TH glbl_drop, glbl_drop_exact;
    int core_index = 0;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    int dp = 0;
    uint32 rjct_th = 0;
    SOC_TMC_ITM_INGRESS_CONGESTION_MGMT ingress_congestion_mgmt;
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core_index) {

        
        SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
        SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);

        rv = qax_itm_global_resource_allocation_get(unit, core_index, &ingress_congestion_mgmt);
        SOCDNX_IF_ERR_EXIT(rv);

        
        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            
            rjct_th = ingress_congestion_mgmt.global[rsrc_type].total;

            for (dp = 0; dp < SOC_TMC_NOF_DROP_PRECEDENCE; ++dp) {
                glbl_drop.global_shrd_pool_0[rsrc_type][dp].set = rjct_th;
                glbl_drop.global_shrd_pool_0[rsrc_type][dp].clear = rjct_th;
                glbl_drop.global_shrd_pool_1[rsrc_type][dp].set = rjct_th;
                glbl_drop.global_shrd_pool_1[rsrc_type][dp].clear = rjct_th;
            }
        }

        
        
        
        glbl_drop.global_free_dram[0].set   = 128; 
        glbl_drop.global_free_dram[0].clear = 130;
        glbl_drop.global_free_dram[1].set   = 156; 
        glbl_drop.global_free_dram[1].clear = 158;
        glbl_drop.global_free_dram[2].set   = 192; 
        glbl_drop.global_free_dram[2].clear = 194;
        glbl_drop.global_free_dram[3].set   = 232; 
        glbl_drop.global_free_dram[3].clear = 234;

        
        
        glbl_drop.global_free_sram[0][0].set   = 70; 
        glbl_drop.global_free_sram[0][0].clear = 120;
        glbl_drop.global_free_sram[0][1].set   = 130; 
        glbl_drop.global_free_sram[0][1].clear = 180;
        glbl_drop.global_free_sram[0][2].set   = 190; 
        glbl_drop.global_free_sram[0][2].clear = 240;
        glbl_drop.global_free_sram[0][3].set   = 250; 
        glbl_drop.global_free_sram[0][3].clear = 300;
        
        glbl_drop.global_free_sram_only[0][0].set   = 70; 
        glbl_drop.global_free_sram_only[0][0].clear = 120;
        glbl_drop.global_free_sram_only[0][1].set   = 130; 
        glbl_drop.global_free_sram_only[0][1].clear = 180;
        glbl_drop.global_free_sram_only[0][2].set   = 190; 
        glbl_drop.global_free_sram_only[0][2].clear = 240;
        glbl_drop.global_free_sram_only[0][3].set   = 250; 
        glbl_drop.global_free_sram_only[0][3].clear = 300;

        
        
        glbl_drop.global_free_sram[1][0].set   = 70; 
        glbl_drop.global_free_sram[1][0].clear = 120;
        glbl_drop.global_free_sram[1][1].set   = 130; 
        glbl_drop.global_free_sram[1][1].clear = 180;
        glbl_drop.global_free_sram[1][2].set   = 190; 
        glbl_drop.global_free_sram[1][2].clear = 240;
        glbl_drop.global_free_sram[1][3].set   = 250; 
        glbl_drop.global_free_sram[1][3].clear = 300;
        
        glbl_drop.global_free_sram_only[1][0].set   = 70; 
        glbl_drop.global_free_sram_only[1][0].clear = 120;
        glbl_drop.global_free_sram_only[1][1].set   = 130; 
        glbl_drop.global_free_sram_only[1][1].clear = 180;
        glbl_drop.global_free_sram_only[1][2].set   = 190; 
        glbl_drop.global_free_sram_only[1][2].clear = 240;
        glbl_drop.global_free_sram_only[1][3].set   = 250; 
        glbl_drop.global_free_sram_only[1][3].clear = 300;

        rv = qax_itm_glob_rcs_drop_set(unit, core_index, &glbl_drop, &glbl_drop_exact);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  qax_itm_per_queue_info_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64, data2_above_64;
    uint32 offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOC_REG_ABOVE_64_CLEAR(data2_above_64);

    offset = queue / 4; 
    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

    switch (queue % 4) {
        
        case 0:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASSf, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASSf, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASSf, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASSf, IQM_static_tbl_data->connection_class);
            break;
        case 1:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_1f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_1f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_1f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_1f, IQM_static_tbl_data->connection_class);
            break;
        case 2:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_2f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_2f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_2f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_2f, IQM_static_tbl_data->connection_class);
            break;
        case 3:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_3f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_3f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_3f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_3f, IQM_static_tbl_data->connection_class);
            break;
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_per_queue_info_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_OUT   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64, data2_above_64;
    uint32 offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOC_REG_ABOVE_64_CLEAR(data2_above_64);
    IQM_static_tbl_data->que_signature = 0; 

    offset = queue / 4; 
    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

    switch (queue % 4) {
        
        case 0:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASSf);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASSf);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASSf);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASSf);
            break;
        case 1:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_1f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_1f);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_1f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_1f);
            break;
        case 2:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_2f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_2f);
            IQM_static_tbl_data->traffic_class  = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_2f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_2f);
            break;
        case 3:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_3f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_3f);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_3f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_3f);
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_in_pp_port_scheduler_compensation_profile_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_IN   int  scheduler_profile
  )
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit, core), in_pp_port, &data));
    soc_CGM_IPP_HCPm_field32_set(unit, &data, SCH_PROFILEf,scheduler_profile);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_IPP_HCPm(unit, CGM_BLOCK(unit, core), in_pp_port, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_in_pp_port_scheduler_compensation_profile_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_OUT  int*  scheduler_profile
  )
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_IPP_HCPm(unit, CGM_BLOCK(unit, core), in_pp_port, &data));
    *scheduler_profile = soc_CGM_IPP_HCPm_field32_get(unit, &data, SCH_PROFILEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_profile_ocb_only_set(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_IN   int is_ocb_only
  )
{
    uint32 data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));
    
    soc_mem_field32_set(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, &data, SRAM_DRAM_ONLY_MODEf, is_ocb_only);
     
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));         
        
exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_profile_ocb_only_get(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_OUT  int *is_ocb_only
  )
{
    uint32 data, field;


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));
    
    field = soc_mem_field32_get(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, &data, SRAM_DRAM_ONLY_MODEf);
    
    *is_ocb_only = (int)field;        
        
exit:
    SOCDNX_FUNC_RETURN;
}


int qax_itm_congestion_statistics_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats 
  )
{
    uint32 val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(stats);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNIT);
    } 
    if (((core < 0) || (core > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && (core != SOC_CORE_ALL)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    }

      

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->sram_buf_free = val; 	

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->sram_buf_min_free = val; 		

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->sram_pdbs_free = val; 	

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->sram_pdbs_min_free = val; 		

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->bdb_free = val; 			

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val));
    stats->min_bdb_free = val; 

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_itm_min_free_resources_stat_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_IN SOC_TMC_ITM_CGM_RSRC_STAT_TYPE type,
  SOC_SAND_OUT uint64 *value
  )
{
    uint32 val32;

    SOCDNX_INIT_FUNC_DEFS;

    switch (type) {
        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_BDB:
            
            SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val32));
            break;
        
        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_BUFFERS:
            
            SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_PDB:
            
            SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, SOC_CORE_ALL, &val32));
            break;

        default:
            SOCDNX_IF_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("Resource statistic type not supported for this device.")));
    }

    COMPILER_64_SET(*value, 0, val32);

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_itm_ingress_latency_init(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 regVal;
    uint64 reg64Val;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    for(i = 0; i < QAX_ITM_NOF_MAX_LATENCY_MEASUREMENTS; ++i) { 
        SOCDNX_IF_ERR_EXIT(READ_ITE_PACKET_LATENCY_MEASUREMENTr(unit, i, reg_above_64_val));
        
        soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, reg_above_64_val, BIG_LATENCY_ERR_THf, 0xffffffff);
        
        soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, reg_above_64_val, EN_LATENCY_ERR_REPORTf, 0x1);
        
        soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, reg_above_64_val, EN_LATENCY_DROPf, 0x0);
        
        soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, reg_above_64_val, EN_CNI_BIG_LATf, 0x0);
        
        soc_reg_above_64_field32_set(unit, ITE_PACKET_LATENCY_MEASUREMENTr, reg_above_64_val, EN_TRACK_LATENCYf, 0x1);

        SOCDNX_IF_ERR_EXIT(WRITE_ITE_PACKET_LATENCY_MEASUREMENTr(unit, i, reg_above_64_val));
    }
    
    
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, &reg64Val));
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64Val, PACKET_LATENCY_MESUREMENT_FLOW_ID_SELf, 0x6);
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, reg64Val));

    
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, &reg64Val));
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64Val, PACKET_LATENCY_MESUREMENT_SHIFT_RIGHTf, 0x5);
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, reg64Val));

    
    
    
    SOCDNX_IF_ERR_EXIT(READ_ITE_PACKET_LATENCY_FORMATr(unit, &regVal));
    soc_reg_field_set(unit, ITE_PACKET_LATENCY_FORMATr, &regVal, LATENCY_FORMATf, 0x2);
    soc_reg_field_set(unit, ITE_PACKET_LATENCY_FORMATr, &regVal, LATENCY_ACCURACYf, 0x5);
    SOCDNX_IF_ERR_EXIT(WRITE_ITE_PACKET_LATENCY_FORMATr(unit, regVal));

exit:
    SOCDNX_FUNC_RETURN;

}



int qax_itm_max_latency_pkts_get(int unit, int max_count, SOC_TMC_MAX_LATENCY_PACKETS_INFO *max_latency_packets, int *actual_count)
{
    int i,output_index=0;
    uint32 reg_val[4];
    uint32 tm_port, cosq, latency, modid;
    SOCDNX_INIT_FUNC_DEFS;


    for (i = 0; i < SOC_TMC_ING_NUMBER_OF_HIGHEST_LATENCY_PACKETS; i++)
    {
        SOCDNX_IF_ERR_EXIT(READ_ITE_MAX_LATENCYm(unit, MEM_BLOCK_ANY, i, reg_val));
        soc_mem_field_get(unit, ITE_MAX_LATENCYm, reg_val, LAT_VALUEf, &latency);
        if(latency > 0)
        {
            if(output_index >= max_count)
            {
                SOCDNX_EXIT_WITH_ERR(BCM_E_RESOURCE, (_BSL_SOC_MSG("index=%d. reach the max_count value."), output_index));
            }
            COMPILER_64_ZERO(max_latency_packets[output_index].latency);
            COMPILER_64_ADD_32(max_latency_packets[output_index].latency,latency);
            soc_mem_field_get(unit, ITE_MAX_LATENCYm, reg_val, LAT_PORT_IDf, &tm_port);
            soc_mem_field_get(unit, ITE_MAX_LATENCYm, reg_val, LAT_DEST_DEVf, &modid);
            BCM_GPORT_MODPORT_SET(max_latency_packets[output_index].dest_gport, modid, tm_port);
            soc_mem_field_get(unit, ITE_MAX_LATENCYm, reg_val, LAT_TRF_CLSf, &cosq);
            max_latency_packets[output_index].cosq = (int)cosq;
            soc_mem_field_get(unit, ITE_MAX_LATENCYm, reg_val, LAT_POINTERf, &max_latency_packets[output_index].latency_flow);
            output_index++;
        }
    }
    *actual_count = output_index;
exit:
    SOCDNX_FUNC_RETURN;
}



int
  qax_itm_dp_discard_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{     
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_itm_dp_discard_set_verify(unit, discard_dp));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, CGM_GLBL_RJCT_PRMSr, SOC_CORE_ALL, 0, DP_LEVEL_RJCT_THf, discard_dp));

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_dp_discard_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{     
    uint32 fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(discard_dp);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, CGM_GLBL_RJCT_PRMSr, SOC_CORE_ALL, 0, DP_LEVEL_RJCT_THf, &fld_val));

    *discard_dp = fld_val;

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_committed_q_size_set(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  )
{
    uint64    data;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type;
    int32 max_guaranteed_limit;
   
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);
    

    max_guaranteed_limit = QAX_ITM_GRNT_BYTES_MAX;

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }
    if (info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] > max_guaranteed_limit) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("guaranteed size %d is out of range\n"), info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES]));
    }
    if (info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES] > QAX_ITM_GRNT_SRAM_BYTES_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("guaranteed sram size %d is out of range\n"), 
                                           info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES]));
    }
    if (info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS] > QAX_ITM_GRNT_SRAM_PDS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("guaranteed sram pds %d is out of range\n"), 
                                           info->guaranteed_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS]));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_GRNTD_PRMSm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));
    
    for (thresh_type = 0; thresh_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; thresh_type++) {
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &voq_guaranteed_th_mant_exp[thresh_type],1,
                                                       &data,
                                                       info->guaranteed_size[thresh_type], 
                                                       &exact_info->guaranteed_size[thresh_type]));
    }

    
    SOCDNX_IF_ERR_EXIT( WRITE_CGM_VOQ_GRNTD_PRMSm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));


exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_committed_q_size_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info
  )
{
    uint64 data;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(exact_info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }
    
    
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_GRNTD_PRMSm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));

    for (thresh_type = 0; thresh_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; thresh_type++) {

        _qax_itm_mantissa_exp_field_get(unit, &voq_guaranteed_th_mant_exp[thresh_type],&data, &exact_info->guaranteed_size[thresh_type]);

    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_fadt_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_FADT_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *exact_info
  )
{

    soc_reg_above_64_val_t data;
    int mem_id;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type;
    uint32 max_size_limit = QAX_ITM_QUEUE_SIZE_BYTES_MAX;
    uint32 alpha_field;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);
    
    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"),drop_precedence_ndx ));
    }

    if (info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] >  max_size_limit) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max threshold %d is out of range\n"),
                                           info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES]));
    }

    if (info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES] >  QAX_ITM_QUEUE_SIZE_SRAM_BYTES_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max threshold %d is out of range\n"),
                                           info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES]));
    }

    if (info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS] >  QAX_ITM_QUEUE_SIZE_SRAM_PDS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max threshold %d is out of range\n"),
                                           info->max_threshold[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS]));
    }

    for (thresh_type = SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES; thresh_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; thresh_type++) {
        switch (thresh_type) {
            case SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES: {
                mem_id = CGM_VOQ_WORDS_RJCT_PRMSm;
                break;
            }
            case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES: {
                mem_id = CGM_VOQ_SRAM_WORDS_RJCT_PRMSm;
                break;
            }
            case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS: {
                mem_id = CGM_VOQ_SRAM_PDS_RJCT_PRMSm;
                break;
            }
            
            
            default:  {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected threshold type %d\n"), thresh_type));
            }
        }
        
        
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem_id, MEM_BLOCK_ANY, 
                                        rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, data));

        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&voq_fadt_max_th_mant_exp[thresh_type], 1, data,
                                                           info->max_threshold[thresh_type], 
                                                           &(exact_info->max_threshold[thresh_type])));
        
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&voq_fadt_min_th_mant_exp[thresh_type], 1, data,
                                                           info->min_threshold[thresh_type], 
                                                           &(exact_info->min_threshold[thresh_type])));
        
        alpha_field =  _qax_itm_alpha_to_field(unit, info->adjust_factor[thresh_type]);
        soc_mem_field32_set(unit, mem_id, &data, VOQ_FADT_ADJUST_FACTORf,
                            alpha_field);
        exact_info->adjust_factor[thresh_type] = info->adjust_factor[thresh_type];
    
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem_id, MEM_BLOCK_ANY, 
                                         rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, data));
    }
    
exit:
    SOCDNX_FUNC_RETURN;

}




int
  qax_itm_fadt_tail_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_FADT_DROP_INFO  *info
  )

{

    soc_reg_above_64_val_t data;
    int mem_id;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type;
    uint32 alpha_field;

    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx >=  SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"), drop_precedence_ndx));
    }


    for (thresh_type = SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES; thresh_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; thresh_type++) {
        switch (thresh_type) {
            case SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES: {
                mem_id = CGM_VOQ_WORDS_RJCT_PRMSm;
                break;
            }
            case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES: {
                mem_id = CGM_VOQ_SRAM_WORDS_RJCT_PRMSm;
                break;
            }
            case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS: {
                mem_id = CGM_VOQ_SRAM_PDS_RJCT_PRMSm;
                break;
            }
            
             
            default:  {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected threshold type %d\n"), thresh_type));
            }
        }
    
        
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem_id, MEM_BLOCK_ANY, 
                                        rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, data));

        _qax_itm_mantissa_exp_field_get(unit, &voq_fadt_max_th_mant_exp[thresh_type], data,&(info->max_threshold[thresh_type]));
        _qax_itm_mantissa_exp_field_get(unit, &voq_fadt_min_th_mant_exp[thresh_type], data,&(info->min_threshold[thresh_type]));

        alpha_field = soc_mem_field32_get(unit, mem_id, &data, VOQ_FADT_ADJUST_FACTORf);
        info->adjust_factor[thresh_type] = _qax_itm_field_to_alpha(unit, alpha_field);
    }

exit:
    SOCDNX_FUNC_RETURN;

}



int
  qax_itm_tail_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *exact_info
  )
{
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    
    if (drop_precedence_ndx != SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("This function should be called for ECN drop only\n")));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_CNI_PRMSm, MEM_BLOCK_ANY, 
                                    rt_cls_ndx, data));
    
    SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&voq_wred_ecn_max_size_th_mant_exp, 1, data,
                                                       info->max_inst_q_size,
                                                       &(exact_info->max_inst_q_size)));

    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_CNI_PRMSm, MEM_BLOCK_ANY, 
                                    rt_cls_ndx, data));

exit:
    SOCDNX_FUNC_RETURN;

}



int
  qax_itm_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  )
{
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    
    if (drop_precedence_ndx != SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("This function should be called for ECN drop only\n")));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_CNI_PRMSm, MEM_BLOCK_ANY, 
                                    rt_cls_ndx, data));
    
    _qax_itm_mantissa_exp_field_get(unit,&voq_wred_ecn_max_size_th_mant_exp, data,
                                    &info->max_inst_q_size);


exit:
    SOCDNX_FUNC_RETURN;
}



STATIC void
_qax_itm_hw_data_to_wred_info (
        SOC_SAND_IN  int                                                  unit,
        SOC_SAND_IN  uint32                                               min_avrg_th,
        SOC_SAND_IN  uint32                                               max_avrg_th,
        SOC_SAND_IN  uint32                                               c1,
        SOC_SAND_IN  uint32                                               c2,
        SOC_SAND_IN  uint32                                               c3,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                          *wred_param
  )
{
    SOC_SAND_U64
        u64_1,
        u64_2;
    uint32 remainder;
    uint32 avrg_th_diff, two_power_c1;
    
    
    wred_param->min_avrg_th = min_avrg_th;

    
    wred_param->max_avrg_th = max_avrg_th;
    
    
    wred_param->max_packet_size = ((c3 == 0) && (c2 == 1)) ? 0 : (0x1 << (c3)) * SOC_TMC_ITM_WRED_GRANULARITY;
    
    avrg_th_diff = (max_avrg_th - min_avrg_th) / SOC_TMC_ITM_WRED_GRANULARITY;
    
    two_power_c1 = 1<<c1;
    
    
    soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff, &u64_1);
    remainder = soc_sand_u64_devide_u64_long(&u64_1, SOC_TMC_WRED_NORMALIZE_FACTOR, &u64_2);
    soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);
    
    if(remainder > (SOC_TMC_WRED_NORMALIZE_FACTOR/2))
    {
        wred_param->max_probability++;
    }
        
    if(wred_param->max_probability > 100)
    {
        wred_param->max_probability = 100;
    }
}


STATIC void
  _qax_itm_wred_info_to_hw_data (
     SOC_SAND_IN   int unit,
     SOC_SAND_IN   SOC_TMC_ITM_WRED_QT_DP_INFO                          *wred_param,
     SOC_SAND_IN   uint32                                               min_avrg_th_exact,
     SOC_SAND_IN   uint32                                               max_avrg_th_exact,
     SOC_SAND_OUT  uint32*                                              c1,
     SOC_SAND_OUT  uint32*                                              c2,
     SOC_SAND_OUT  uint32*                                              c3
  )
{
    uint32
        max_prob,
        calc,
        max_val_c1;
    int32
        avrg_th_diff_wred_granular = 0;
    int32
        min_avrg_th_exact_wred_granular,
        max_avrg_th_exact_wred_granular;
    uint32
        trunced;
    SOC_SAND_U64
        u64_c2 = {{0}};
    
    trunced = FALSE;
    
    
    min_avrg_th_exact_wred_granular = min_avrg_th_exact / SOC_TMC_ITM_WRED_GRANULARITY;
    
    
    max_avrg_th_exact_wred_granular = max_avrg_th_exact / SOC_TMC_ITM_WRED_GRANULARITY;
    
    
    calc = wred_param->max_packet_size;
    if (calc > SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
    {
        calc = SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
    }
    calc = SOC_SAND_DIV_ROUND_UP(calc, SOC_TMC_ITM_WRED_GRANULARITY );
    *c3 = (max_avrg_th_exact == 0 ? 0 : soc_sand_log2_round_up(calc));
    
    
    max_prob = (wred_param->max_probability);
    
    
    if(max_prob>=100)
    {
        max_prob = 99;
    }
    calc = SOC_TMC_WRED_NORMALIZE_FACTOR * max_prob; 
    
    
    max_val_c1 = 31; 
    
    avrg_th_diff_wred_granular =
        (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);
    
    if(avrg_th_diff_wred_granular == 0)
    {
        *c1 = max_val_c1;
    }
    else
    {
        calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
    *c1 = soc_sand_log2_round_down(calc);
    }
    
    if(*c1 < max_val_c1)
    {
        
        uint32
            now     = 1 <<(*c1),
            changed = 1 <<(*c1+1),
            diff_with_now,
            diff_with_change;
        
        diff_with_change = changed-calc;
        diff_with_now    = calc-now;
        if( diff_with_change < diff_with_now)
        {
            *c1 += 1;
        }
    }
    
    SOC_SAND_LIMIT_FROM_ABOVE(*c1, max_val_c1);
    
    if (min_avrg_th_exact_wred_granular > 0)
    { 
        max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, min_avrg_th_exact_wred_granular);
        max_val_c1 = soc_sand_log2_round_down(max_val_c1);
        SOC_SAND_LIMIT_FROM_ABOVE(*c1, max_val_c1);
    }
    
    

    soc_sand_u64_multiply_longs(
        min_avrg_th_exact_wred_granular,
        (1 << *c1),
        &u64_c2
        );
    
    trunced = soc_sand_u64_to_long(&u64_c2, c2);
    
    if (trunced)
    {
        *c2 = 0xFFFFFFFF;
    }
    *c2 = (max_avrg_th_exact == 0 ? 1 : *c2);
    
}

void _qax_itm_wred_mem_fields_get( 
        int unit,
        int rt_cls_ndx,
        int drop_precedence_ndx, 
        int* is_ecn,
        int* mem,
        int* wred_en_field,
        int* c1_field, 
        int* c2_field, 
        int* c3_field,
        int* entry_offset
        )
{
    *is_ecn = (drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE);
    
    if (*is_ecn) {
        *mem = CGM_CNI_PRMSm;

        *wred_en_field = WRED_ENf;
        *c1_field = WRED_C_1f;
        *c2_field = WRED_C_2f;
        *c3_field = WRED_C_3f;

        
        *entry_offset = rt_cls_ndx;
    } else {
        *mem = CGM_VOQ_WORDS_RJCT_PRMSm;

        *wred_en_field = VOQ_WRED_ENf;
        *c1_field = VOQ_WRED_C_1f;
        *c2_field = VOQ_WRED_C_2f;
        *c3_field = VOQ_WRED_C_3f;

        *entry_offset = (rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    }

}


STATIC int
  _qax_itm_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);


    if (rt_cls_ndx > ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx > SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"),
                                           drop_precedence_ndx ));
    }

    if (info->min_avrg_th > info->max_avrg_th)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("min threshold %d is higher than max threshold %d\n"),
                                          info->min_avrg_th, info->max_avrg_th));
    }
    
    if (info->min_avrg_th > QAX_ITM_Q_WRED_INFO_MIN_AVRG_TH_MAX)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("min threshold %d is out of range\n"),
                                          info->min_avrg_th));
    }

    if (info->max_avrg_th > QAX_ITM_Q_WRED_INFO_MAX_AVRG_TH_MAX)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max threshold %d is out of range\n"),
                                          info->max_avrg_th));
    }

    if (info->max_probability > QAX_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max probability %d is out of range\n"),
                                          info->max_probability));
    }

    if (info->max_packet_size > SOC_TMC_ITM_WRED_MAX_PACKET_SIZE)
    {
       SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("max packet size %d is out of range\n"),
                                          info->max_packet_size));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_wred_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
    soc_reg_above_64_val_t data;
    int mem;
    int wred_en_field, c1_field, c2_field, c3_field;
    int entry_offset;
    uint32 exact_min_avrg_th = 0, exact_max_avrg_th = 0;
    uint32 c1, c2, c3;
    int is_ecn;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    SOCDNX_IF_ERR_EXIT(_qax_itm_wred_verify(unit, rt_cls_ndx, drop_precedence_ndx, info));

    _qax_itm_wred_mem_fields_get(unit, rt_cls_ndx, drop_precedence_ndx, 
                                  &is_ecn, &mem,
                                  &wred_en_field, &c1_field, &c2_field, &c3_field,
                                  &entry_offset
        );
    

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                                    entry_offset , data));
    
    SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&voq_wred_ecn_min_th_mant_exp[is_ecn], 1, data,
                                                       info->min_avrg_th,
                                                       &exact_min_avrg_th));

    SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &voq_wred_ecn_max_th_mant_exp[is_ecn], 1, data,
                                                       info->max_avrg_th,
                                                       &exact_max_avrg_th));

    _qax_itm_wred_info_to_hw_data(unit, info, exact_min_avrg_th, exact_max_avrg_th, &c1, &c2, &c3);

    soc_mem_field32_set(unit, mem, data, wred_en_field, info->wred_en);
    soc_mem_field32_set(unit, mem, data, c2_field, c2);
    soc_mem_field32_set(unit, mem, data, c3_field, c3);
    soc_mem_field32_set(unit, mem, data, c1_field, c1);
    if (!is_ecn) {
        soc_mem_field32_set(unit, mem, data, VOQ_WRED_IGNR_PKT_SIZEf, SOC_SAND_BOOL2NUM(info->ignore_packet_size));
    }
 
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, MEM_BLOCK_ANY, 
                                     entry_offset , data));
    
    _qax_itm_hw_data_to_wred_info(unit, exact_min_avrg_th, exact_max_avrg_th, c1, c2, c3, exact_info);
                       
    exact_info->wred_en = info->wred_en;

    exact_info->ignore_packet_size = (!is_ecn ? SOC_SAND_BOOL2NUM(info->ignore_packet_size) : 0);

exit:
    SOCDNX_FUNC_RETURN;
}



int
  qax_itm_wred_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
    soc_reg_above_64_val_t data;
    int mem;
    int wred_en_field, c1_field, c2_field, c3_field;
    int entry_offset;
    uint32 min_avrg_th = 0, max_avrg_th = 0;
    uint32 c1, c2, c3, wred_pckt_sz_ignr = 0;
    int is_ecn;
    uint32 wred_en;
 

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
 
    if (rt_cls_ndx > ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx > SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"),drop_precedence_ndx ));
    }
    
    _qax_itm_wred_mem_fields_get( unit,
                                  rt_cls_ndx, drop_precedence_ndx, 
                                  &is_ecn, &mem, &wred_en_field,
                                  &c1_field, &c2_field, &c3_field,
                                  &entry_offset
            );


    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, MEM_BLOCK_ANY, 
                                    entry_offset , data));


    
    _qax_itm_mantissa_exp_field_get(unit, &voq_wred_ecn_min_th_mant_exp[is_ecn], &data, &min_avrg_th);
    
    
    _qax_itm_mantissa_exp_field_get(unit, &voq_wred_ecn_max_th_mant_exp[is_ecn], &data, &max_avrg_th);

    wred_en = soc_mem_field32_get(unit, mem, data, wred_en_field);
    c2 = soc_mem_field32_get(unit, mem, data, c2_field );
    c3 = soc_mem_field32_get(unit, mem, data, c3_field);
    c1 = soc_mem_field32_get(unit, mem, data, c1_field);

    if (!is_ecn) {
        wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem, data, VOQ_WRED_IGNR_PKT_SIZEf);
    }

    _qax_itm_hw_data_to_wred_info (unit, min_avrg_th, max_avrg_th, c1, c2, c3, info);
                       
    info->wred_en = SOC_SAND_NUM2BOOL(wred_en);

    info->ignore_packet_size =
        SOC_SAND_NUM2BOOL(wred_pckt_sz_ignr);
    
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC int
  _qax_itm_wred_exp_wq_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (rt_cls_ndx > ARAD_ITM_QT_RT_CLS_MAX ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (exp_wq > ARAD_ITM_WQ_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("exp. weight %d is out of range\n"), exp_wq));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_wred_exp_wq_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 exp_wq,
    SOC_SAND_IN  uint8                  enable
  )
{
    uint32 data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT (_qax_itm_wred_exp_wq_verify(
                            unit,
                            rt_cls_ndx,
                            exp_wq
                            ));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
                           unit,
                           CGM_VOQ_AVRG_PRMSm,
                           MEM_BLOCK_ANY,
                           rt_cls_ndx,
                           &data
                           ));
    
    soc_mem_field32_set(unit, CGM_VOQ_AVRG_PRMSm, &data, AVRG_WEIGHTf, exp_wq);
    soc_mem_field32_set(unit, CGM_VOQ_AVRG_PRMSm, &data, AVRG_ENf, enable);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
                           unit,
                           CGM_VOQ_AVRG_PRMSm,
                           MEM_BLOCK_ANY,
                           rt_cls_ndx,
                           &data
                           ));
exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_wred_exp_wq_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              rt_cls_ndx,
    SOC_SAND_OUT  uint32             *exp_wq
  )
{
    uint32 data;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(exp_wq);

    if (rt_cls_ndx > ARAD_ITM_QT_RT_CLS_MAX ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }
    
    SOCDNX_IF_ERR_EXIT(soc_mem_read(
                           unit,
                           CGM_VOQ_AVRG_PRMSm,
                           MEM_BLOCK_ANY,
                           rt_cls_ndx,
                           &data
                           ));
    
   *exp_wq = soc_mem_field32_get(unit, CGM_VOQ_AVRG_PRMSm, &data, AVRG_WEIGHTf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_dyn_total_thresh_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN int    core,
    SOC_SAND_IN uint8  is_ocb_only,
                int32  reservation_increase_array[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
  )
{
    uint32
        res;
    SOC_TMC_ITM_GUARANTEED_RESOURCE guaranteed_resources;
    soc_dpp_guaranteed_pair_t *guaranteed_pair;
    int32 reservation_increase;
    int core_id;
    int dp;
    soc_reg_above_64_val_t data;

    

    itm_mantissa_exp_threshold_info voq_shared_oc_set_th_mant_exp[] = {
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            WORDS_SET_THf,
            8, 
            5, 
            1   
        },
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            SRAM_WORDS_SET_THf,
            8, 
            4, 
            1  
        },
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            SRAM_PDS_SET_THf,
            8, 
            4, 
            1 
        }
    };    

    itm_mantissa_exp_threshold_info voq_shared_oc_clr_th_mant_exp[] = {
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            WORDS_CLR_THf,
            8, 
            5, 
            1   
        },
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            SRAM_WORDS_CLR_THf,
            8, 
            4, 
            1  
        },
        {
            CGM_VOQ_SHRD_OC_RJCT_THm,
            SRAM_PDS_CLR_THf,
            8, 
            4, 
            1 
        }
    };    

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reservation_increase_array);

    if (!is_ocb_only) {
        int thresh_type;
        
        SOC_DPP_CORES_ITER(core, core_id){
            res = sw_state_access[unit].dpp.soc.qax.tm.guaranteed_info.get(unit, core_id, &guaranteed_resources);
            SOCDNX_IF_ERR_EXIT(res);
            for (thresh_type = 0; thresh_type < DPP_COSQ_INGRESS_THRESHOLD_NOF_TYPES; thresh_type++) {
                guaranteed_pair =  &(guaranteed_resources.guaranteed[thresh_type]);
                reservation_increase = reservation_increase_array[core_id][thresh_type];

                if (reservation_increase < 0 &&  ((uint32)(-reservation_increase)) > guaranteed_pair->used) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("requested change in reserved resource %d is out of range\n"), 
                                                          reservation_increase));
                }
                
                if (reservation_increase) {
                    int32 resource_left_calc =  ((int32)(guaranteed_pair->total -
                                                         (guaranteed_pair->used))) - reservation_increase;
                    uint32 resource_left =  resource_left_calc;
                    uint32 dummy;

                    int *numerator = SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_numerator;
                    int denominator = SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_denominator;

                    if (resource_left_calc < 0) { 
                        
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("out of resources. Left amount %d\n"), 
                                                          resource_left_calc));
                    }
                    
                    for (dp = 0 ; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++) {

                        
                        SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SHRD_OC_RJCT_THm(unit, MEM_BLOCK_ANY, dp, &data));
                        
                        if (resource_left) { 
                            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &voq_shared_oc_set_th_mant_exp[thresh_type], 0, data,
                                                                               resource_left/denominator*numerator[dp],
                                                                               &dummy));
                            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &voq_shared_oc_clr_th_mant_exp[thresh_type], 0, data,
                                                                               resource_left/denominator*numerator[dp],
                                                                               &dummy));
                        }
                        else {
                            soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, data, voq_shared_oc_set_th_mant_exp[thresh_type].field_id, 
                                                ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED);
                            soc_mem_field32_set(unit, CGM_VOQ_SHRD_OC_RJCT_THm, data, voq_shared_oc_clr_th_mant_exp[thresh_type].field_id, 
                                                ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED);
                        }
                        SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_SHRD_OC_RJCT_THm(unit, MEM_BLOCK_ANY, dp, data));
                    }
                    guaranteed_pair->used += reservation_increase;
                }
                res = sw_state_access[unit].dpp.soc.qax.tm.guaranteed_info.set(unit, core_id, &guaranteed_resources);
                SOCDNX_SAND_IF_ERR_EXIT(res);
            }
        }
    }

exit:
  SOCDNX_FUNC_RETURN;
}




int
  qax_itm_queue_dyn_info_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_QUEUE_DYN_INFO *info
  )  
{
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (queue_ndx > ARAD_MAX_QUEUE_ID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("queue index %d is out of range\n"), queue_ndx));
    }

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SIZEm(unit, CGM_BLOCK(unit, core), queue_ndx, data));

    info->pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = soc_mem_field32_get(unit, CGM_VOQ_SIZEm, data, WORDS_SIZEf) * 16;
    info->pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES] = soc_mem_field32_get(unit, CGM_VOQ_SIZEm, data, SRAM_WORDS_SIZEf) * 16;
    info->pq_inst_que_size[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS] = soc_mem_field32_get(unit, CGM_VOQ_SIZEm, data, SRAM_PDS_SIZEf);

exit:
  SOCDNX_FUNC_RETURN;
}




int qax_itm_dram_bound_alpha_field_get( SOC_SAND_IN int                 unit,
                                        SOC_SAND_IN SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh,
                                        SOC_SAND_IN SOC_TMC_INGRESS_THRESHOLD_TYPE_E thresh_type)
{
    int field_id = INVALIDf;

    switch (dram_thresh) {
        case SOC_TMC_INGRESS_DRAM_BOUND:
            switch (thresh_type) {
                case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                    field_id = SRAM_WORDS_BOUND_ADJUST_FACTORf;
                    break;
                case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                    field_id = SRAM_PDS_BOUND_ADJUST_FACTORf;
                    break;
                default:
                    break;
            }
            break;
        case SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE:
            switch (thresh_type) {
                case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                    field_id = SRAM_WORDS_RECOVERY_ADJUST_FACTORf;
                    break;
                case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                    field_id = SRAM_PDS_RECOVERY_ADJUST_FACTORf;
                    break;
                default:
                    break;
                }
        default:
            break;
    }

    return field_id;
}

typedef enum {
    QAX_DRAM_BOUND_THRESH_MIN = 0,
    QAX_DRAM_BOUND_THRESH_MAX,
    QAX_DRAM_BOUND_THRESH_FREE_MIN,
    QAX_DRAM_BOUND_THRESH_FREE_MAX
} qax_dram_bound_thresh_type_t;

int _qax_itm_dram_bound_mant_exp_info_get(
    int unit,
    SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E resource_type, 
    qax_dram_bound_thresh_type_t thresh_type, 
    itm_mantissa_exp_threshold_info* info
    )
{
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);

    info->mem_id = CGM_VOQ_DRAM_BOUND_PRMSm;
    info->factor = (resource_type == SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES ? 16 : 1);

    switch (thresh_type) {
        case QAX_DRAM_BOUND_THRESH_MIN:
            switch (dram_thresh) {
                case SOC_TMC_INGRESS_DRAM_BOUND:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_BOUND_MIN_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_BOUND_MIN_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                case SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_RECOVERY_MIN_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_RECOVERY_MIN_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected dram threshold %d\n"), dram_thresh));
            }
            break;
        case QAX_DRAM_BOUND_THRESH_MAX:
            switch (dram_thresh) {
                case SOC_TMC_INGRESS_DRAM_BOUND:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_BOUND_MAX_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_BOUND_MAX_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                case SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_RECOVERY_MAX_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_RECOVERY_MAX_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected dram threshold %d\n"), dram_thresh));
            }
            break;
        case QAX_DRAM_BOUND_THRESH_FREE_MIN:
            switch (dram_thresh) {
                case SOC_TMC_INGRESS_DRAM_BOUND:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_BOUND_FREE_MIN_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_BOUND_FREE_MIN_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                case SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_RECOVERY_FREE_MIN_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_RECOVERY_FREE_MIN_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected dram threshold %d\n"), dram_thresh));
            }
            break;
        case QAX_DRAM_BOUND_THRESH_FREE_MAX:
            switch (dram_thresh) {
                case SOC_TMC_INGRESS_DRAM_BOUND:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_BOUND_FREE_MAX_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_BOUND_FREE_MAX_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                case SOC_TMC_INGRESS_DRAM_BOUND_RECOVERY_FAILURE:
                    switch (resource_type) {
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES:
                            info->field_id = SRAM_WORDS_RECOVERY_FREE_MAX_THf;
                            break;
                        case SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS:
                            info->field_id = SRAM_PDS_RECOVERY_FREE_MAX_THf;
                            break;
                        default:
                            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected resource type %d\n"), resource_type));
                    }
                    break;
                default:
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unexpected dram threshold %d\n"), dram_thresh));
            }
            break;
    }

    info->mantissa_bits = 8;
    info->exp_bits = soc_mem_field_length(unit, info->mem_id, info->field_id) - info->mantissa_bits;

exit:
  SOCDNX_FUNC_RETURN;

}

int
  qax_itm_dram_bound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
                 SOC_TMC_ITM_DRAM_BOUND_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *exact_info
  )
{
    soc_reg_above_64_val_t data;
    SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E resource_type;
    uint32 alpha_field_value;
    int alpha_field_id;
 
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_VOQ_DRAM_BOUND_PRMSm, MEM_BLOCK_ANY, 
                                    rt_cls_ndx, data));
    for (resource_type = SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES; resource_type <= SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS; resource_type++) {
        for (dram_thresh = SOC_TMC_INGRESS_DRAM_BOUND; dram_thresh < SOC_TMC_INGRESS_DRAM_BOUND_NOF_TYPES; dram_thresh++) {
            itm_mantissa_exp_threshold_info mantissa_exp_info;
            SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* dram_threshold = SOC_TMC_ITM_DRAM_BOUND_INFO_thresh_get(unit, info, dram_thresh, resource_type);
            SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* exact_dram_threshold = SOC_TMC_ITM_DRAM_BOUND_INFO_thresh_get(unit, exact_info, dram_thresh, resource_type);
            

            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_MAX, &mantissa_exp_info));
            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&mantissa_exp_info, 1, data,
                                                               dram_threshold->max_threshold, 
                                                               &(exact_dram_threshold->max_threshold)));
        
            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_MIN, &mantissa_exp_info));
            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&mantissa_exp_info, 1, data,
                                                               dram_threshold->min_threshold, 
                                                               &(exact_dram_threshold->min_threshold)));

            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_FREE_MAX, &mantissa_exp_info));
            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&mantissa_exp_info, 1, data,
                                                               dram_threshold->free_max_threshold, 
                                                               &(exact_dram_threshold->free_max_threshold)));
        
            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_FREE_MIN, &mantissa_exp_info));
            SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&mantissa_exp_info, 1, data,
                                                               dram_threshold->free_min_threshold, 
                                                               &(exact_dram_threshold->free_min_threshold)));


            alpha_field_id = qax_itm_dram_bound_alpha_field_get(unit,dram_thresh, resource_type);
            if (alpha_field_id == INVALIDf) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't find alpha field name\n")));
            }
            alpha_field_value =  _qax_itm_alpha_to_field(unit, dram_threshold->alpha);
            soc_mem_field32_set(unit, CGM_VOQ_DRAM_BOUND_PRMSm, &data, alpha_field_id, alpha_field_value);
            exact_dram_threshold->alpha = dram_threshold->alpha;
    
        }
    }

    SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit,&voq_dram_bound_qsize_recovery_th_mant_exp, 1, data,
                                                       info->qsize_recovery_th,
                                                       &(exact_info->qsize_recovery_th)));

    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_VOQ_DRAM_BOUND_PRMSm, MEM_BLOCK_ANY, 
                                     rt_cls_ndx , data));

exit:
  SOCDNX_FUNC_RETURN;
}

int
  qax_itm_dram_bound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_DRAM_BOUND_INFO  *info
  )
{
    soc_reg_above_64_val_t data;
    SOC_TMC_INGRESS_DRAM_BOUND_TYPE_E dram_thresh;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E resource_type;
    uint32 alpha_field_value;
    int alpha_field_id;
 
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_VOQ_DRAM_BOUND_PRMSm, MEM_BLOCK_ANY, 
                                    rt_cls_ndx, data));
    for (resource_type = SOC_TMC_INGRESS_THRESHOLD_SRAM_BYTES; resource_type <= SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS; resource_type++) {
 
        for (dram_thresh = SOC_TMC_INGRESS_DRAM_BOUND; dram_thresh < SOC_TMC_INGRESS_DRAM_BOUND_NOF_TYPES; dram_thresh++) {
            SOC_TMC_ITM_DRAM_BOUND_THRESHOLD* dram_threshold = SOC_TMC_ITM_DRAM_BOUND_INFO_thresh_get(unit, info, dram_thresh, resource_type);
            itm_mantissa_exp_threshold_info mantissa_exp_info;

            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_FREE_MAX, &mantissa_exp_info));
            _qax_itm_mantissa_exp_field_get(unit,&mantissa_exp_info, data,
                                                               &(dram_threshold->free_max_threshold));
        
            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_FREE_MIN, &mantissa_exp_info));
            _qax_itm_mantissa_exp_field_get(unit,&mantissa_exp_info, data,
                                                               &(dram_threshold->free_min_threshold));

            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_MAX, &mantissa_exp_info));
            _qax_itm_mantissa_exp_field_get(unit,&mantissa_exp_info, data,
                                                               &(dram_threshold->max_threshold));
        
            SOCDNX_IF_ERR_EXIT(_qax_itm_dram_bound_mant_exp_info_get(unit,dram_thresh,resource_type, QAX_DRAM_BOUND_THRESH_MIN, &mantissa_exp_info));
            _qax_itm_mantissa_exp_field_get(unit,&mantissa_exp_info, data,
                                                               &(dram_threshold->min_threshold));


            alpha_field_id = qax_itm_dram_bound_alpha_field_get(unit, dram_thresh, resource_type);
            if (alpha_field_id == INVALIDf) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("can't find alpha field name\n")));
            }
            alpha_field_value = soc_mem_field32_get(unit, CGM_VOQ_DRAM_BOUND_PRMSm, &data, alpha_field_id);
            dram_threshold->alpha = _qax_itm_field_to_alpha(unit,alpha_field_value);
    
        }
    }

    _qax_itm_mantissa_exp_field_get(unit,&voq_dram_bound_qsize_recovery_th_mant_exp, data,
                                                       &info->qsize_recovery_th);

exit:
  SOCDNX_FUNC_RETURN;
}








int
  qax_itm_vsq_pg_tc_profile_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_IN int pg_tc_profile
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, CGM_IPPPMm, &data, TC_BITMAP_INDEXf, pg_tc_profile);

    rv = WRITE_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_vsq_pg_tc_profile_mapping_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_OUT int *pg_tc_profile
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    *pg_tc_profile = soc_mem_field32_get(unit, CGM_IPPPMm, &data, TC_BITMAP_INDEXf);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_vsq_pg_tc_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
  )
{
    int rv = SOC_E_NONE;
    SHR_BITDCLNAME(pg_tc_bitmap_data, QAX_ITM_VSQ_PG_OFFSET_FIELD_SIZE);
    int tc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (pg_tc_bitmap & ~((1 << SOC_TMC_NOF_TRAFFIC_CLASSES * 3) - 1)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, 
                  (BSL_META_U(unit, "PG TC mapping bitmap is invalid %d, maximum bit nust be %d\n"), 
                   pg_tc_bitmap, SOC_TMC_NOF_TRAFFIC_CLASSES * 3));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    for (tc = 0; tc < SOC_TMC_NOF_TRAFFIC_CLASSES; ++tc) {
        
        rv = READ_CGM_VSQ_PG_TC_BITMAPm(unit, CGM_BLOCK(unit, SOC_CORE_ALL), tc, pg_tc_bitmap_data);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(pg_tc_bitmap_data, pg_tc_profile_id * 3, &pg_tc_bitmap, tc * 3, 3);

        rv = WRITE_CGM_VSQ_PG_TC_BITMAPm(unit, CGM_BLOCK(unit, SOC_CORE_ALL), tc, pg_tc_bitmap_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_vsq_pg_tc_profile_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int         pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
  )
{
    uint32 rv = SOC_E_NONE;
    SHR_BITDCLNAME(pg_tc_bitmap_data, QAX_ITM_VSQ_PG_OFFSET_FIELD_SIZE);
    uint32 pg_offset_bitmap = 0;
    int tc = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_tc_bitmap);

    for (tc = 0; tc < SOC_TMC_NOF_TRAFFIC_CLASSES; ++tc) {
        rv = READ_CGM_VSQ_PG_TC_BITMAPm(unit, CGM_BLOCK(unit, core_id), tc, pg_tc_bitmap_data);
        SOCDNX_IF_ERR_EXIT(rv);

        SHR_BITCOPY_RANGE(&pg_offset_bitmap, tc * 3, pg_tc_bitmap_data, pg_tc_profile_id * 3, 3);
    }

    *pg_tc_bitmap = pg_offset_bitmap;

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_vsq_pb_prm_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;   
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (pg_ndx >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    rv = READ_CGM_PB_VSQ_PRMSm(unit, CGM_BLOCK(unit, SOC_CORE_ALL), pg_ndx, &data);
    SOCDNX_IF_ERR_EXIT(rv);
    
    soc_mem_field32_set(unit, CGM_PB_VSQ_PRMSm, &data, IS_LOSSLESSf, pg_prm->is_lossles);
    soc_mem_field32_set(unit, CGM_PB_VSQ_PRMSm, &data, POOL_IDf, pg_prm->pool_id);
    soc_mem_field32_set(unit, CGM_PB_VSQ_PRMSm, &data, USE_PORT_GRNTDf, pg_prm->use_min_port);
    soc_mem_field32_set(unit, CGM_PB_VSQ_PRMSm, &data, ADMT_PROFILEf, pg_prm->admit_profile);

    rv = WRITE_CGM_PB_VSQ_PRMSm(unit, CGM_BLOCK(unit, SOC_CORE_ALL), pg_ndx, &data);
    SOCDNX_IF_ERR_EXIT(rv);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_pb_prm_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              pg_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (pg_ndx >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    rv = READ_CGM_PB_VSQ_PRMSm(unit, CGM_BLOCK(unit, SOC_CORE_ALL), pg_ndx, &data);
    SOCDNX_IF_ERR_EXIT(rv);
    
    pg_prm->is_lossles = soc_mem_field32_get(unit, CGM_PB_VSQ_PRMSm, &data, IS_LOSSLESSf);
    pg_prm->pool_id = soc_mem_field32_get(unit, CGM_PB_VSQ_PRMSm, &data, POOL_IDf);
    pg_prm->use_min_port = soc_mem_field32_get(unit, CGM_PB_VSQ_PRMSm, &data, USE_PORT_GRNTDf);
    pg_prm->admit_profile = soc_mem_field32_get(unit, CGM_PB_VSQ_PRMSm, &data, ADMT_PROFILEf);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_src_vsqs_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int src_pp_port,
    SOC_SAND_IN int src_port_vsq_index,
    SOC_SAND_IN int pg_base,
    SOC_SAND_IN uint8 enable
  )
{
    int rv = SOC_E_NONE, data = 0;
    SOCDNX_INIT_FUNC_DEFS;

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_pp_port < -1 || src_pp_port > SOC_MAX_NUM_PORTS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_DPP_DEFS_GET(unit, nof_vsq_e)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    soc_mem_field32_set(unit, CGM_IPPPMm, &data, NIF_PORTf, enable ? src_port_vsq_index : (SOC_DPP_DEFS_GET(unit, nof_vsq_e) - 1));

    
    
    soc_mem_field32_set(unit, CGM_IPPPMm, &data, PG_BASEf, enable ? pg_base : 0);

    rv = WRITE_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_src_vsqs_mapping_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int core_id,
    SOC_SAND_IN  int src_pp_port,
    SOC_SAND_OUT int *src_port_vsq_index,
    SOC_SAND_OUT int *pg_base,
    SOC_SAND_OUT uint8 *enable
  )
{
    int rv = SOC_E_NONE, data = 0;
    uint32 phy_port = -1, tmp_src_pp_port = -1;
    soc_port_t logical_port;
    int core_get;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(src_port_vsq_index);
    SOCDNX_NULL_CHECK(pg_base);
    SOCDNX_NULL_CHECK(enable);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_pp_port < 0 || src_pp_port >= SOC_MAX_NUM_PORTS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    
    *src_port_vsq_index = soc_mem_field32_get(unit, CGM_IPPPMm, &data, NIF_PORTf);
    
    *pg_base = soc_mem_field32_get(unit, CGM_IPPPMm, &data, PG_BASEf);

    *enable = 0;
    
    if (*src_port_vsq_index == 0 || *pg_base == 0) {
        phy_port = 1;
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        rv = soc_port_sw_db_local_to_pp_port_get(unit, logical_port, &tmp_src_pp_port, &core_get);
        SOCDNX_IF_ERR_EXIT(rv);
        
        if (tmp_src_pp_port == src_pp_port) {
            *enable = TRUE;
        }
    } else {
        *enable = TRUE;
    }

exit: 
    SOCDNX_FUNC_RETURN;
}


int
    qax_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info) 
{
    int res = SOC_E_NONE;
    uint32 entry_offset = 0;
    soc_reg_above_64_val_t data;
    const soc_mem_t mem_arr_CGM_VSQ_WRED_RJCT_PRMS[SOC_TMC_NOF_VSQ_GROUPS] = {CGM_VSQA_WRED_RJCT_PRMSm, CGM_VSQB_WRED_RJCT_PRMSm, CGM_VSQC_WRED_RJCT_PRMSm, CGM_VSQD_WRED_RJCT_PRMSm, CGM_VSQE_WRED_RJCT_PRMSm, CGM_VSQF_WRED_RJCT_PRMSm};
    itm_mantissa_exp_threshold_info wred_man_exp_info;
    uint32 min_avrg_th = 0, max_avrg_th = 0;
    uint32 c1 = 0, c2 = 0, c3 = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    sal_memset(&wred_man_exp_info, 0x0, sizeof(itm_mantissa_exp_threshold_info));

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        
        entry_offset += pool_id * (SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit) * SOC_TMC_NOF_DROP_PRECEDENCE);
    }

    res = soc_mem_read(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    wred_man_exp_info.mem_id = mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx];
    wred_man_exp_info.mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
    wred_man_exp_info.factor = SOC_TMC_ITM_WRED_GRANULARITY;

    
    wred_man_exp_info.field_id = WRED_MIN_THf;
    wred_man_exp_info.exp_bits = soc_mem_field_length(unit, wred_man_exp_info.mem_id, wred_man_exp_info.field_id) - wred_man_exp_info.mantissa_bits;
    _qax_itm_mantissa_exp_field_get(unit, &wred_man_exp_info, data, &min_avrg_th);

    
    wred_man_exp_info.field_id = WRED_MAX_THf;
    wred_man_exp_info.exp_bits = soc_mem_field_length(unit, wred_man_exp_info.mem_id, wred_man_exp_info.field_id) - wred_man_exp_info.mantissa_bits;
    _qax_itm_mantissa_exp_field_get(unit, &wred_man_exp_info, data, &max_avrg_th);

    info->wred_en = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_ENf));
    info->ignore_packet_size = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_IGNR_PKT_SIZEf);   
    c1 = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_1f);
    c2 = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_2f);
    c3 = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_3f);
    info->min_avrg_th = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_MIN_THf);
    info->max_avrg_th = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_MAX_THf);

    _qax_itm_hw_data_to_wred_info (unit, min_avrg_th, max_avrg_th, c1, c2, c3, info);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
    qax_itm_vsq_wred_set(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_IN  SOC_TMC_ITM_WRED_QT_DP_INFO *info,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info)
{
    int res = SOC_E_NONE;
    uint32 entry_offset = 0;
    soc_reg_above_64_val_t data;
    const soc_mem_t mem_arr_CGM_VSQ_WRED_RJCT_PRMS[SOC_TMC_NOF_VSQ_GROUPS] = {CGM_VSQA_WRED_RJCT_PRMSm, CGM_VSQB_WRED_RJCT_PRMSm, CGM_VSQC_WRED_RJCT_PRMSm, CGM_VSQD_WRED_RJCT_PRMSm, CGM_VSQE_WRED_RJCT_PRMSm, CGM_VSQF_WRED_RJCT_PRMSm};
    itm_mantissa_exp_threshold_info wred_man_exp_info;
    uint32 exact_min_avrg_th = 0, exact_max_avrg_th = 0;
    uint32 c1 = 0, c2 = 0, c3 = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    sal_memset(&wred_man_exp_info, 0x0, sizeof(itm_mantissa_exp_threshold_info));

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        
        entry_offset += pool_id * (SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit) * SOC_TMC_NOF_DROP_PRECEDENCE);
    }

    res = soc_mem_read(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    wred_man_exp_info.mem_id = mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx];
    wred_man_exp_info.mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
    wred_man_exp_info.factor = SOC_TMC_ITM_WRED_GRANULARITY;

    
    wred_man_exp_info.field_id = WRED_MIN_THf;
    wred_man_exp_info.exp_bits = soc_mem_field_length(unit, wred_man_exp_info.mem_id, wred_man_exp_info.field_id) - wred_man_exp_info.mantissa_bits;
    res = _qax_itm_mantissa_exp_field_set(unit, &wred_man_exp_info, 1, data, info->min_avrg_th, &exact_min_avrg_th);
    SOCDNX_IF_ERR_EXIT(res);

    
    wred_man_exp_info.field_id = WRED_MAX_THf;
    wred_man_exp_info.exp_bits = soc_mem_field_length(unit, wred_man_exp_info.mem_id, wred_man_exp_info.field_id) - wred_man_exp_info.mantissa_bits;
    res = _qax_itm_mantissa_exp_field_set(unit, &wred_man_exp_info, 1, data, info->max_avrg_th, &exact_max_avrg_th);
    SOCDNX_IF_ERR_EXIT(res);

    _qax_itm_wred_info_to_hw_data(unit, info, exact_min_avrg_th, exact_max_avrg_th, &c1, &c2, &c3);

    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_ENf, SOC_SAND_NUM2BOOL(info->wred_en));
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_IGNR_PKT_SIZEf, SOC_SAND_BOOL2NUM(info->ignore_packet_size));   
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_1f, c1);
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_2f, c2);
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], data, WRED_C_3f, c3);

    res = soc_mem_write(unit, mem_arr_CGM_VSQ_WRED_RJCT_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    _qax_itm_hw_data_to_wred_info(unit, exact_min_avrg_th, exact_max_avrg_th, c1, c2, c3, exact_info);
    exact_info->wred_en = SOC_SAND_BOOL2NUM(info->wred_en);
    exact_info->ignore_packet_size = SOC_SAND_BOOL2NUM(info->ignore_packet_size);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    int res = SOC_E_NONE;
    uint32 data;
    const soc_mem_t mem_arr_CGM_VSQ_AVRG_PRMS[SOC_TMC_NOF_VSQ_GROUPS] = 
        {CGM_VSQA_AVRG_PRMSm, CGM_VSQB_AVRG_PRMSm, CGM_VSQC_AVRG_PRMSm, CGM_VSQD_AVRG_PRMSm, CGM_VSQE_AVRG_PRMSm, CGM_VSQF_AVRG_PRMSm};
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }   

    if (info->exp_wq >= (0x1 << soc_mem_field_length(unit, CGM_VSQA_AVRG_PRMSm, AVRG_WEIGHTf))) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = soc_mem_read(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], &data, AVRG_UPDT_ENf, info->wred_en);
    soc_mem_field32_set(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], &data, AVRG_WEIGHTf, info->exp_wq);

    res = soc_mem_write(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    int res = SOC_E_NONE;
    uint32 data;
    const soc_mem_t mem_arr_CGM_VSQ_AVRG_PRMS[SOC_TMC_NOF_VSQ_GROUPS] = 
        {CGM_VSQA_AVRG_PRMSm, CGM_VSQB_AVRG_PRMSm, CGM_VSQC_AVRG_PRMSm, CGM_VSQD_AVRG_PRMSm, CGM_VSQE_AVRG_PRMSm, CGM_VSQF_AVRG_PRMSm};
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }   

    res = soc_mem_read(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], CGM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

    info->wred_en = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], &data, AVRG_UPDT_ENf);
    info->exp_wq = soc_mem_field32_get(unit, mem_arr_CGM_VSQ_AVRG_PRMS[vsq_group_ndx], &data, AVRG_WEIGHTf);

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_tail_drop_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
    int rv = SOC_E_NONE;
    uint32 entry_offset;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS] = 
       {{CGM_VSQA_WORDS_RJCT_PRMSm, CGM_VSQB_WORDS_RJCT_PRMSm, CGM_VSQC_WORDS_RJCT_PRMSm, CGM_VSQD_WORDS_RJCT_PRMSm, },
        {CGM_VSQA_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQB_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQC_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQD_SRAM_BUFFERS_RJCT_PRMSm,},
        {CGM_VSQA_SRAM_PDS_RJCT_PRMSm, CGM_VSQB_SRAM_PDS_RJCT_PRMSm, CGM_VSQC_SRAM_PDS_RJCT_PRMSm, CGM_VSQD_SRAM_PDS_RJCT_PRMSm}};
    const soc_field_t field_arr_MAX_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_SIZE_TH_DP_0f, MAX_SIZE_TH_DP_1f, MAX_SIZE_TH_DP_2f, MAX_SIZE_TH_DP_3f};
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    rv = qax_itm_vsq_tail_drop_default_get(unit, exact_info);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }   
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = vsq_rt_cls_ndx;

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type][vsq_group_ndx], 
                field_arr_MAX_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                info->max_inst_q_size_th[rsrc_type], 
                &exact_info->max_inst_q_size_th[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_fc_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO  *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max);
    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear = info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set;

    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max);
    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear = info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set;

    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max);
    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear = info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set;

    
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max);
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.min_threshold = info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.alpha = 0;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear_offset = 0;

    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max);
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.min_threshold = info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.alpha = 0;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear_offset = 0;

    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold = SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max);
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.min_threshold = info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.alpha = 0;
    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear_offset = 0;

exit:
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    uint32 max_size_exp = 0, max_size_mnt = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    
    info->max_inst_q_size = info->max_inst_q_size_bds = info->alpha = 0;

    
    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, CGM_VSQA_WORDS_RJCT_PRMSm, MAX_SIZE_TH_DP_0f)) - 1, QAX_ITM_NOF_MANTISSA_BITS, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_th[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES] = (max_size_mnt * (0x1 << max_size_exp));
    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, CGM_VSQA_SRAM_BUFFERS_RJCT_PRMSm, MAX_SIZE_TH_DP_0f)) - 1, QAX_ITM_NOF_MANTISSA_BITS, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_th[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS] = (max_size_mnt * (0x1 << max_size_exp));
    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, CGM_VSQA_SRAM_PDS_RJCT_PRMSm, MAX_SIZE_TH_DP_0f)) - 1, QAX_ITM_NOF_MANTISSA_BITS, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_th[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS] = (max_size_mnt * (0x1 << max_size_exp));

exit:
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    int rv = SOC_E_NONE;
    uint32 entry_offset;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS] = 
       {{CGM_VSQA_WORDS_RJCT_PRMSm, CGM_VSQB_WORDS_RJCT_PRMSm, CGM_VSQC_WORDS_RJCT_PRMSm, CGM_VSQD_WORDS_RJCT_PRMSm, },
        {CGM_VSQA_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQB_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQC_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQD_SRAM_BUFFERS_RJCT_PRMSm,},
        {CGM_VSQA_SRAM_PDS_RJCT_PRMSm, CGM_VSQB_SRAM_PDS_RJCT_PRMSm, CGM_VSQC_SRAM_PDS_RJCT_PRMSm, CGM_VSQD_SRAM_PDS_RJCT_PRMSm}};
    const soc_field_t field_arr_MAX_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_SIZE_TH_DP_0f, MAX_SIZE_TH_DP_1f, MAX_SIZE_TH_DP_2f, MAX_SIZE_TH_DP_3f};
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    rv = qax_itm_vsq_tail_drop_default_get(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(rv);

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }   
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = vsq_rt_cls_ndx;

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type][vsq_group_ndx], 
                field_arr_MAX_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                &info->max_inst_q_size_th[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_src_port_rjct_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *exact_info
  )
{
    uint32 rv = SOC_E_NONE;
    uint32 entry_offset = vsq_rt_cls_ndx;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        {CGM_VSQE_WORDS_RJCT_PRMSm, CGM_VSQE_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQE_SRAM_PDS_RJCT_PRMSm};
    const soc_field_t field_arr_MAX_GRNTD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_GRNTD_SIZE_TH_DP_0f, MAX_GRNTD_SIZE_TH_DP_1f, MAX_GRNTD_SIZE_TH_DP_2f, MAX_GRNTD_SIZE_TH_DP_3f};
    const soc_field_t field_arr_MAX_SHRD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_SHRD_SIZE_TH_DP_0f, MAX_SHRD_SIZE_TH_DP_1f, MAX_SHRD_SIZE_TH_DP_2f, MAX_SHRD_SIZE_TH_DP_3f};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    entry_offset += pool_id * SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit);

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_GRNTD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                info->max_guaranteed[rsrc_type], 
                &exact_info->max_guaranteed[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_SHRD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                info->max_shared[rsrc_type], 
                &exact_info->max_shared[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                MAX_HDRM_SIZE_THf, 
                entry_offset,
                info->max_headroom[rsrc_type], 
                &exact_info->max_headroom[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_src_port_rjct_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_SRC_PORT_INFO       *info
  )
{
    uint32 rv = SOC_E_NONE;
    uint32 entry_offset = vsq_rt_cls_ndx;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        {CGM_VSQE_WORDS_RJCT_PRMSm, CGM_VSQE_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQE_SRAM_PDS_RJCT_PRMSm};
    const soc_field_t field_arr_MAX_GRNTD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_GRNTD_SIZE_TH_DP_0f, MAX_GRNTD_SIZE_TH_DP_1f, MAX_GRNTD_SIZE_TH_DP_2f, MAX_GRNTD_SIZE_TH_DP_3f};
    const soc_field_t field_arr_MAX_SHRD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_SHRD_SIZE_TH_DP_0f, MAX_SHRD_SIZE_TH_DP_1f, MAX_SHRD_SIZE_TH_DP_2f, MAX_SHRD_SIZE_TH_DP_3f};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    entry_offset += pool_id * SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit);

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_GRNTD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                &info->max_guaranteed[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_SHRD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                &info->max_shared[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                MAX_HDRM_SIZE_THf, 
                entry_offset,
                &info->max_headroom[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_pg_rjct_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *exact_info
  )
{
    uint32 rv = SOC_E_NONE;
    uint32 entry_offset = vsq_rt_cls_ndx;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        {CGM_VSQF_WORDS_RJCT_PRMSm, CGM_VSQF_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQF_SRAM_PDS_RJCT_PRMSm};
    const soc_field_t field_arr_MAX_GRNTD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_GRNTD_SIZE_TH_DP_0f, MAX_GRNTD_SIZE_TH_DP_1f, MAX_GRNTD_SIZE_TH_DP_2f, MAX_GRNTD_SIZE_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_MAX_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_MAX_TH_DP_0f, SHRD_SIZE_FADT_MAX_TH_DP_1f, SHRD_SIZE_FADT_MAX_TH_DP_2f, SHRD_SIZE_FADT_MAX_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_MIN_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_MIN_TH_DP_0f, SHRD_SIZE_FADT_MIN_TH_DP_1f, SHRD_SIZE_FADT_MIN_TH_DP_2f, SHRD_SIZE_FADT_MIN_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_ADJUST_FACTOR_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_ADJUST_FACTOR_DP_0f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_1f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_2f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_3f};
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO pg_shared_fadt_fields = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    pg_shared_fadt_fields.max_field = field_arr_SHRD_SIZE_FADT_MAX_TH_DP[drop_precedence_ndx];
    pg_shared_fadt_fields.min_field = field_arr_SHRD_SIZE_FADT_MIN_TH_DP[drop_precedence_ndx];
    pg_shared_fadt_fields.alpha_field = field_arr_SHRD_SIZE_FADT_ADJUST_FACTOR_DP[drop_precedence_ndx];

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_GRNTD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                info->max_guaranteed[rsrc_type], 
                &exact_info->max_guaranteed[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_fadt_set(unit,
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                pg_shared_fadt_fields,
                entry_offset,
                info->max_shared[rsrc_type],
                &exact_info->max_shared[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_pg_headroom_rjct_set(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                entry_offset,
                info->max_headroom[rsrc_type], 
                &exact_info->max_headroom[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_pg_rjct_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_INFO    *info
  )
{
    int rv = SOC_E_NONE;
    uint32 entry_offset = vsq_rt_cls_ndx;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    const soc_mem_t mem_arr_CGM_VSQ_RJCT_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        {CGM_VSQF_WORDS_RJCT_PRMSm, CGM_VSQF_SRAM_BUFFERS_RJCT_PRMSm, CGM_VSQF_SRAM_PDS_RJCT_PRMSm};
    const soc_field_t field_arr_MAX_GRNTD_SIZE_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {MAX_GRNTD_SIZE_TH_DP_0f, MAX_GRNTD_SIZE_TH_DP_1f, MAX_GRNTD_SIZE_TH_DP_2f, MAX_GRNTD_SIZE_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_MAX_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_MAX_TH_DP_0f, SHRD_SIZE_FADT_MAX_TH_DP_1f, SHRD_SIZE_FADT_MAX_TH_DP_2f, SHRD_SIZE_FADT_MAX_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_MIN_TH_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_MIN_TH_DP_0f, SHRD_SIZE_FADT_MIN_TH_DP_1f, SHRD_SIZE_FADT_MIN_TH_DP_2f, SHRD_SIZE_FADT_MIN_TH_DP_3f};
    const soc_field_t field_arr_SHRD_SIZE_FADT_ADJUST_FACTOR_DP[SOC_TMC_NOF_DROP_PRECEDENCE] = 
        {SHRD_SIZE_FADT_ADJUST_FACTOR_DP_0f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_1f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_2f, SHRD_SIZE_FADT_ADJUST_FACTOR_DP_3f};
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO pg_shared_fadt_fields = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    pg_shared_fadt_fields.max_field = field_arr_SHRD_SIZE_FADT_MAX_TH_DP[drop_precedence_ndx];
    pg_shared_fadt_fields.min_field = field_arr_SHRD_SIZE_FADT_MIN_TH_DP[drop_precedence_ndx];
    pg_shared_fadt_fields.alpha_field = field_arr_SHRD_SIZE_FADT_ADJUST_FACTOR_DP[drop_precedence_ndx];

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                field_arr_MAX_GRNTD_SIZE_TH_DP[drop_precedence_ndx], 
                entry_offset,
                &info->max_guaranteed[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_fadt_get(unit,
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                pg_shared_fadt_fields,
                entry_offset,
                &info->max_shared[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_pg_headroom_rjct_get(unit, 
                rsrc_type,
                mem_arr_CGM_VSQ_RJCT_PRMS[rsrc_type], 
                entry_offset,
                &info->max_headroom[rsrc_type]);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_rjct_man_exp_set(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    soc_field_t                         field,
    uint32                              entry_offset,
    uint32                              threshold,
    uint32                              *result_threshold
  )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    itm_mantissa_exp_threshold_info rjct_man_exp_info = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    rjct_man_exp_info.mem_id = mem;

    rv = soc_mem_read(unit, rjct_man_exp_info.mem_id, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

    rjct_man_exp_info.field_id = field;
    rjct_man_exp_info.mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
    rjct_man_exp_info.exp_bits = soc_mem_field_length(unit, rjct_man_exp_info.mem_id, rjct_man_exp_info.field_id) - rjct_man_exp_info.mantissa_bits;
    rjct_man_exp_info.factor = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) ? (QAX_ITM_WORDS_RESOLUTION) : (1);

    rv = _qax_itm_mantissa_exp_field_set(unit, &rjct_man_exp_info, 1, data, threshold, result_threshold);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = soc_mem_write(unit, rjct_man_exp_info.mem_id, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_rjct_man_exp_get(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    soc_field_t                         field,
    uint32                              entry_offset,
    uint32                              *result_threshold
  )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    itm_mantissa_exp_threshold_info rjct_man_exp_info = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    rjct_man_exp_info.mem_id = mem;

    rv = soc_mem_read(unit, rjct_man_exp_info.mem_id, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

    rjct_man_exp_info.field_id = field;
    rjct_man_exp_info.mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
    rjct_man_exp_info.exp_bits = soc_mem_field_length(unit, rjct_man_exp_info.mem_id, rjct_man_exp_info.field_id) - rjct_man_exp_info.mantissa_bits;
    rjct_man_exp_info.factor = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) ? (QAX_ITM_WORDS_RESOLUTION) : (1);

    _qax_itm_mantissa_exp_field_get(unit, &rjct_man_exp_info, data, result_threshold);

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_rjct_fadt_set(
    int                              unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type,
    soc_mem_t                        mem,
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO fadt_fields,
    uint32                           entry_offset,
    SOC_TMC_FADT_INFO                fadt_info,
    SOC_TMC_FADT_INFO                *exact_fadt_info
  )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    if ((fadt_info.alpha > SOC_TMC_ITM_FADT_ALPHA_MAX) || (fadt_info.alpha < SOC_TMC_ITM_FADT_ALPHA_MIN)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "FADT's alpha must be between -7 and 7\n")));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    rv = qax_itm_vsq_rjct_man_exp_set(unit, 
            rsrc_type,
            mem, 
            fadt_fields.max_field,
            entry_offset,
            fadt_info.max_threshold,
            &exact_fadt_info->max_threshold);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_itm_vsq_rjct_man_exp_set(unit, 
            rsrc_type,
            mem, 
            fadt_fields.min_field,
            entry_offset,
            fadt_info.min_threshold,
            &exact_fadt_info->min_threshold);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_mem_read(unit, mem, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, mem, data, fadt_fields.alpha_field, _qax_itm_alpha_to_field(unit, fadt_info.alpha));

    rv = soc_mem_write(unit, mem, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

    exact_fadt_info->alpha = fadt_info.alpha;

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_rjct_fadt_get(
    int                              unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type,
    soc_mem_t                        mem,
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO fadt_fields,
    uint32                           entry_offset,
    SOC_TMC_FADT_INFO                *fadt_info
  )
{
    int rv = SOC_E_NONE;
    soc_reg_above_64_val_t data;
    uint32 alpha = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    
    rv = qax_itm_vsq_rjct_man_exp_get(unit, 
            rsrc_type,
            mem, 
            fadt_fields.max_field,
            entry_offset,
            &fadt_info->max_threshold);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_itm_vsq_rjct_man_exp_get(unit, 
            rsrc_type,
            mem, 
            fadt_fields.min_field,
            entry_offset,
            &fadt_info->min_threshold);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = soc_mem_read(unit, mem, CGM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(rv);

    alpha = soc_mem_field32_get(unit, mem, data, fadt_fields.alpha_field);    
    fadt_info->alpha = _qax_itm_field_to_alpha(unit, alpha);

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_pg_headroom_rjct_set(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    uint32                              entry_offset,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        headroom_info,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        *exact_headroom_info
  )
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = qax_itm_vsq_rjct_man_exp_set(unit, 
            rsrc_type,
            mem, 
            MAX_HDRM_SIZE_THf,
            entry_offset,
            headroom_info.max_headroom,
            &exact_headroom_info->max_headroom);
    SOCDNX_IF_ERR_EXIT(rv);

    if (rsrc_type != SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) {
        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem, 
                MAX_HDRM_NOMINAL_SIZE_THf,
                entry_offset,
                headroom_info.max_headroom_nominal,
                &exact_headroom_info->max_headroom_nominal);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                rsrc_type,
                mem, 
                MAX_HDRM_EXT_SIZE_THf,
                entry_offset,
                headroom_info.max_headroom_extension,
                &exact_headroom_info->max_headroom_extension);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_pg_headroom_rjct_get(
    int                                 unit,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    uint32                              entry_offset,
    SOC_TMC_ITM_VSQ_PG_HDRM_INFO        *headroom_info
  )
{
    int rv = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    
    rv = qax_itm_vsq_rjct_man_exp_get(unit, 
            rsrc_type,
            mem, 
            MAX_HDRM_SIZE_THf,
            entry_offset,
            &headroom_info->max_headroom);
    SOCDNX_IF_ERR_EXIT(rv);

    if (rsrc_type != SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) {
        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem, 
                MAX_HDRM_NOMINAL_SIZE_THf,
                entry_offset,
                &headroom_info->max_headroom_nominal);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                mem, 
                MAX_HDRM_EXT_SIZE_THf,
                entry_offset,
                &headroom_info->max_headroom_extension);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_qt_rt_cls_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;
    soc_mem_t mem = 0;
    uint32 entry_offset = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rv = qax_itm_vsq_qt_rt_cls_verify(unit, vsq_group_ndx, vsq_in_group_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_CTGRY) {
        if (vsq_rt_cls > SOC_TMC_ITM_VSQ_A_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    } else {
        if (vsq_rt_cls > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }

    switch(vsq_group_ndx) { 
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY:
            mem = CGM_VSQA_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
            mem = CGM_VSQB_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
            mem = CGM_VSQC_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG:
            mem = CGM_VSQD_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_SRC_PORT:
            mem = CGM_VSQE_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_PG:
            mem = CGM_VSQF_PRMSm;
            break;
        default:
            break;
    }

    entry_offset = vsq_in_group_ndx;

    rv = soc_mem_read(unit, mem, CGM_BLOCK(unit, core_id), entry_offset, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, mem, &data, RATE_CLASSf, vsq_rt_cls);

    rv = soc_mem_write(unit, mem, CGM_BLOCK(unit, core_id), entry_offset, &data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx
  )
{
    uint32 rv = SOC_E_NONE;
    uint32 vsq_in_group_size;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    rv = arad_itm_vsq_in_group_size_get(unit, vsq_group_ndx, &vsq_in_group_size);
    SOCDNX_IF_ERR_EXIT(rv);

    if (vsq_in_group_ndx >= vsq_in_group_size) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_qt_rt_cls_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint8               is_ocb_only,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_NDX    vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  )
{
    int rv = SOC_E_NONE;
    uint32 data = 0;
    soc_mem_t mem = 0;
    uint32 entry_offset = 0;
    SOCDNX_INIT_FUNC_DEFS;

    rv = qax_itm_vsq_qt_rt_cls_verify(unit, vsq_group_ndx, vsq_in_group_ndx);
    SOCDNX_IF_ERR_EXIT(rv);

    switch(vsq_group_ndx) { 
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY:
            mem = CGM_VSQA_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
            mem = CGM_VSQB_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
            mem = CGM_VSQC_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_STTSTCS_TAG:
            mem = CGM_VSQD_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_SRC_PORT:
            mem = CGM_VSQE_PRMSm;
            break;
        case SOC_TMC_ITM_VSQ_GROUP_PG:
            mem = CGM_VSQF_PRMSm;
            break;
        default:
            break;
    }

    entry_offset = vsq_in_group_ndx;

    rv = soc_mem_read(unit, mem, CGM_BLOCK(unit, core_id), entry_offset, &data);
    SOCDNX_IF_ERR_EXIT(rv);

    *vsq_rt_cls = soc_mem_field32_get(unit, mem, &data, RATE_CLASSf);

exit:
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_src_port_get(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN int    core_id,
    SOC_SAND_IN int    src_port_vsq_index,
    SOC_SAND_OUT uint32 *src_pp_port,
    SOC_SAND_OUT uint8  *enable
  )
{
    int rv;
    uint32 tmp_src_pp_port, data;
    uint8 found = 0;
    int tmp_src_port_vsq_index = 0;
    uint32 phy_port = -1;
    soc_port_t logical_port;
    int core_get;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(enable);
    SOCDNX_NULL_CHECK(src_pp_port);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_DPP_DEFS_GET(unit, nof_vsq_e)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    
    phy_port = src_port_vsq_index + 1; 
    logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    rv = soc_port_sw_db_local_to_pp_port_get(unit, logical_port, &tmp_src_pp_port, &core_get);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), tmp_src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);
    tmp_src_port_vsq_index = soc_mem_field32_get(unit, CGM_IPPPMm, &data, NIF_PORTf);

    
    if (tmp_src_port_vsq_index == src_port_vsq_index) {
        found = 1;
    }

    if (found) {
        *src_pp_port = tmp_src_pp_port;
        *enable = 1;
    } else {
        *src_pp_port = -1;
        *enable = 0;
    }

exit: 
    SOCDNX_FUNC_RETURN;
}



int
  qax_itm_vsq_pg_mapping_get(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN int     core_id,
    SOC_SAND_IN uint32  pg_vsq_base,
    SOC_SAND_IN int     cosq,
    SOC_SAND_OUT uint32 *src_pp_port,
    SOC_SAND_OUT uint8  *enable
  )
{
    int rv;
    uint32 pg_vsq_id;
    uint32 tmp_pg_vsq_base = -1, tmp_src_pp_port = 0, data = 0;
    int found = 0;
    soc_port_t logical_port;
    uint32 src_link = -1, phy_port = -1;
    int core_get;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(enable);
    SOCDNX_NULL_CHECK(src_pp_port);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (cosq < 0 || cosq >= SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    pg_vsq_id = pg_vsq_base + cosq;
    if (pg_vsq_id >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "Invalid PG VSQ ID %d\n"), pg_vsq_id));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    
    if (SOC_IS_QUX(unit) && (pg_vsq_id >= (SOC_TMC_NOF_TRAFFIC_CLASSES * 8))) {
        
        src_link = 8 + ((pg_vsq_id - (SOC_TMC_NOF_TRAFFIC_CLASSES * 8)) / SOC_TMC_NOF_TRAFFIC_CLASSES_VIPER);
    } else {
        src_link = pg_vsq_id / SOC_TMC_NOF_TRAFFIC_CLASSES;
    }
    phy_port = src_link + 1; 
    rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, phy_port, &phy_port));
    SOCDNX_IF_ERR_EXIT(rv);
    logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    rv = soc_port_sw_db_local_to_pp_port_get(unit, logical_port, &tmp_src_pp_port, &core_get);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = READ_CGM_IPPPMm(unit, CGM_BLOCK(unit, core_id), tmp_src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(rv);
    tmp_pg_vsq_base = soc_mem_field32_get(unit, CGM_IPPPMm, &data, PG_BASEf);

    
    if (tmp_pg_vsq_base == pg_vsq_base) {
        found = 1;
    }

    if (found) {
        *src_pp_port = tmp_src_pp_port;
        *enable = 1;
    } else {
        *src_pp_port = -1;
        *enable = 0;
    }

exit: 
    SOCDNX_FUNC_RETURN;
}


STATIC int 
  qax_itm_category_rngs_verify( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info);    

    if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) || (info->vsq_ctgry1_end > info->vsq_ctgry2_end) || (info->vsq_ctgry0_end > info->vsq_ctgry2_end)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    if (info->vsq_ctgry0_end > SOC_DPP_DEFS_GET(unit, nof_queues)-1) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    if (info->vsq_ctgry1_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    if (info->vsq_ctgry2_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

exit: 
    SOCDNX_FUNC_RETURN; 
} 
 

int 
  qax_itm_category_rngs_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    int rv; 
    uint64 reg_val; 
    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(info); 

    COMPILER_64_ZERO(reg_val);

    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    rv = qax_itm_category_rngs_verify(unit, info); 
    SOCDNX_IF_ERR_EXIT(rv); 

    rv = READ_CGM_VSQ_CATEGORY_RANGESr(unit, SOC_CORE_ALL, &reg_val); 
    SOCDNX_IF_ERR_EXIT(rv); 

    soc_reg64_field32_set(unit, CGM_VSQ_CATEGORY_RANGESr, &reg_val, VSQ_CATEGORY_RANGE_0f, info->vsq_ctgry0_end); 
    soc_reg64_field32_set(unit, CGM_VSQ_CATEGORY_RANGESr, &reg_val, VSQ_CATEGORY_RANGE_1f, info->vsq_ctgry1_end); 
    soc_reg64_field32_set(unit, CGM_VSQ_CATEGORY_RANGESr, &reg_val, VSQ_CATEGORY_RANGE_2f, info->vsq_ctgry2_end); 

    rv = WRITE_CGM_VSQ_CATEGORY_RANGESr(unit, SOC_CORE_ALL, reg_val); 
    SOCDNX_IF_ERR_EXIT(rv); 

exit: 
    SOCDNX_FUNC_RETURN; 
} 
 

int 
  qax_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    int rv; 
    uint64 reg_val; 
    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(info); 

    COMPILER_64_ZERO(reg_val);

    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    rv = READ_CGM_VSQ_CATEGORY_RANGESr(unit, SOC_CORE_ALL, &reg_val); 
    SOCDNX_IF_ERR_EXIT(rv); 

    info->vsq_ctgry0_end = soc_reg64_field32_get(unit, CGM_VSQ_CATEGORY_RANGESr, reg_val, VSQ_CATEGORY_RANGE_0f); 
    info->vsq_ctgry1_end = soc_reg64_field32_get(unit, CGM_VSQ_CATEGORY_RANGESr, reg_val, VSQ_CATEGORY_RANGE_1f); 
    info->vsq_ctgry2_end = soc_reg64_field32_get(unit, CGM_VSQ_CATEGORY_RANGESr, reg_val, VSQ_CATEGORY_RANGE_2f); 

exit: 
    SOCDNX_FUNC_RETURN; 
} 


STATIC
int
  qax_itm_glob_rcs_drop_threshold_verify(
    int                                 unit,
    int                                 is_dram_thresh,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem_id,
    soc_field_t                         field_id,
    const char*                         msg,
    uint32                              threshold
  )
{

    uint32 max_threshold = 0;
    int mantissa_bits, exp_bits;
    uint32 factor;
    uint32 mnt, exp;


    SOCDNX_INIT_FUNC_DEFS; 

    if (is_dram_thresh) {
        max_threshold = QAX_ITM_NOF_DRAM_BDBS(unit);
    }
    else if (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) {
        max_threshold = QAX_ITM_NOF_BYTES(unit);
    } else if (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS) {
        max_threshold = QAX_ITM_NOF_SRAM_BUFFERS(unit);
    } else if (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS) {
        max_threshold = QAX_ITM_NOF_SRAM_PDS(unit);
    }

    
    mantissa_bits = QAX_ITM_NOF_MANTISSA_BITS;
    exp_bits = soc_mem_field_length(unit, mem_id, field_id) - mantissa_bits;
    factor = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) ? (QAX_ITM_WORDS_RESOLUTION) : (1);

    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_break_to_mnt_exp_round_up(
                                SOC_SAND_DIV_ROUND_UP(max_threshold, factor),
                                mantissa_bits,
                                exp_bits,
                                0, &mnt, &exp
                                ));

    max_threshold = mnt * (factor << exp);

    if (threshold > max_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("%s threshold (%u) is above max threshold (%u)"),
                                           msg, threshold, max_threshold));
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_global_rcs_drop_verify_and_set(
    int                                 unit,
    int                                 is_dram_thresh,
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E    rsrc_type,
    soc_mem_t                           mem,
    soc_field_t                         field,
    uint32                              entry_offset,
    const char*                         msg,
    uint32                              threshold,
    uint32                              *result_threshold
  )
{

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(qax_itm_glob_rcs_drop_threshold_verify(unit, is_dram_thresh, rsrc_type, mem, field, msg, threshold));

    SOCDNX_IF_ERR_EXIT(qax_itm_vsq_rjct_man_exp_set(unit, rsrc_type, mem, field, entry_offset, threshold, result_threshold));

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
    int rv = SOC_E_NONE; 
    int color = 0, sram_type = 0;
    uint32 entry_offset = 0;
    int is_dram_thresh;

    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;

    soc_field_t field_arr_VSQ_SHRD_OC_SET_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] =
        {WORDS_SET_THf, SRAM_BUFFERS_SET_THf, SRAM_PDS_SET_THf};
    soc_field_t field_arr_VSQ_SHRD_OC_CLR_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] =
        {WORDS_CLR_THf, SRAM_BUFFERS_CLR_THf, SRAM_PDS_CLR_THf};
    soc_field_t field_arr_GLBL_FR_SRAM_SET_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES] = 
        {SRAM_PDBS_SET_THf, SRAM_BUFFERS_SET_THf};
    soc_field_t field_arr_GLBL_FR_SRAM_CLR_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES] = 
        {SRAM_PDBS_CLR_THf, SRAM_BUFFERS_CLR_THf};

    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(info); 
    SOCDNX_NULL_CHECK(exact_info); 

    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    
    is_dram_thresh = 0;
    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
            
            
            entry_offset = color;

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit,
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_VSQ_SHRD_OC_RJCT_THm,
                                                        field_arr_VSQ_SHRD_OC_SET_TH[rsrc_type],
                                                        entry_offset,
                                                        "VSQ Pool 0 SET",
                                                        info->global_shrd_pool_0[rsrc_type][color].set,
                                                        &exact_info->global_shrd_pool_0[rsrc_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit,
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_VSQ_SHRD_OC_RJCT_THm,
                                                        field_arr_VSQ_SHRD_OC_CLR_TH[rsrc_type],
                                                        entry_offset,
                                                        "VSQ Pool 0 CLEAR",
                                                        info->global_shrd_pool_0[rsrc_type][color].clear,
                                                        &exact_info->global_shrd_pool_0[rsrc_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);

            
            
            entry_offset = color + SOC_TMC_NOF_DROP_PRECEDENCE;

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit,
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_VSQ_SHRD_OC_RJCT_THm,
                                                        field_arr_VSQ_SHRD_OC_SET_TH[rsrc_type],
                                                        entry_offset,
                                                        "VSQ Pool 1 SET",
                                                        info->global_shrd_pool_1[rsrc_type][color].set,
                                                        &exact_info->global_shrd_pool_1[rsrc_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit,
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_VSQ_SHRD_OC_RJCT_THm,
                                                        field_arr_VSQ_SHRD_OC_CLR_TH[rsrc_type],
                                                        entry_offset,
                                                        "VSQ Pool 1 CLEAR",
                                                        info->global_shrd_pool_1[rsrc_type][color].clear,
                                                        &exact_info->global_shrd_pool_1[rsrc_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
    
    rsrc_type = SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS;
    for (sram_type = 0; sram_type < SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES; ++sram_type) {
        for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
            
            entry_offset = color;

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_GLBL_FR_SRAM_RJCT_THm, 
                                                        field_arr_GLBL_FR_SRAM_SET_TH[sram_type], 
                                                        entry_offset,
                                                        "Global Free SRAM SET",
                                                        info->global_free_sram[sram_type][color].set, 
                                                        &exact_info->global_free_sram[sram_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);
            
            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_GLBL_FR_SRAM_RJCT_THm, 
                                                        field_arr_GLBL_FR_SRAM_CLR_TH[sram_type], 
                                                        entry_offset,
                                                        "Global Free SRAM CLEAR",
                                                        info->global_free_sram[sram_type][color].clear, 
                                                        &exact_info->global_free_sram[sram_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);

            
            entry_offset = color + SOC_TMC_NOF_DROP_PRECEDENCE;

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_GLBL_FR_SRAM_RJCT_THm, 
                                                        field_arr_GLBL_FR_SRAM_SET_TH[sram_type], 
                                                        entry_offset,
                                                        "Global Free SRAM SET",
                                                        info->global_free_sram_only[sram_type][color].set, 
                                                        &exact_info->global_free_sram_only[sram_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                        is_dram_thresh,
                                                        rsrc_type,
                                                        CGM_GLBL_FR_SRAM_RJCT_THm, 
                                                        field_arr_GLBL_FR_SRAM_CLR_TH[sram_type], 
                                                        entry_offset,
                                                        "Global Free SRAM CLEAR",
                                                        info->global_free_sram_only[sram_type][color].clear, 
                                                        &exact_info->global_free_sram_only[sram_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
    is_dram_thresh = 1;
    for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
        entry_offset = color;

        
        rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                    is_dram_thresh,
                                                    rsrc_type,
                                                    CGM_GLBL_FR_DRAM_RJCT_THm, 
                                                    DRAM_BDBS_SET_THf, 
                                                    entry_offset,
                                                    "Global Free DRAM SET",
                                                    info->global_free_dram[color].set, 
                                                    &exact_info->global_free_dram[color].set);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = qax_itm_global_rcs_drop_verify_and_set(unit, 
                                                    is_dram_thresh,
                                                    rsrc_type,
                                                    CGM_GLBL_FR_DRAM_RJCT_THm, 
                                                    DRAM_BDBS_CLR_THf, 
                                                    entry_offset,
                                                    "Global Free DRAM CLEAR",
                                                    info->global_free_dram[color].clear, 
                                                    &exact_info->global_free_dram[color].clear);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
    int rv = SOC_E_NONE; 
    int color = 0, sram_type = 0;
    uint32 entry_offset = 0;

    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;

    soc_field_t field_arr_VSQ_SHRD_OC_SET_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] =
        {WORDS_SET_THf, SRAM_BUFFERS_SET_THf, SRAM_PDS_SET_THf};
    soc_field_t field_arr_VSQ_SHRD_OC_CLR_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] =
        {WORDS_CLR_THf, SRAM_BUFFERS_CLR_THf, SRAM_PDS_CLR_THf};
    soc_field_t field_arr_GLBL_FR_SRAM_SET_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES] = 
        {SRAM_PDBS_SET_THf, SRAM_BUFFERS_SET_THf};
    soc_field_t field_arr_GLBL_FR_SRAM_CLR_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES] = 
        {SRAM_PDBS_CLR_THf, SRAM_BUFFERS_CLR_THf};

    SOCDNX_INIT_FUNC_DEFS; 

    SOCDNX_NULL_CHECK(info); 

    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 

    
    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
            
            
            entry_offset = color;

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit,
                    rsrc_type,
                    CGM_VSQ_SHRD_OC_RJCT_THm,
                    field_arr_VSQ_SHRD_OC_SET_TH[rsrc_type],
                    entry_offset,
                    &info->global_shrd_pool_0[rsrc_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit,
                    rsrc_type,
                    CGM_VSQ_SHRD_OC_RJCT_THm,
                    field_arr_VSQ_SHRD_OC_CLR_TH[rsrc_type],
                    entry_offset,
                    &info->global_shrd_pool_0[rsrc_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);

            
            
            entry_offset = color + SOC_TMC_NOF_DROP_PRECEDENCE;

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit,
                    rsrc_type,
                    CGM_VSQ_SHRD_OC_RJCT_THm,
                    field_arr_VSQ_SHRD_OC_SET_TH[rsrc_type],
                    entry_offset,
                    &info->global_shrd_pool_1[rsrc_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit,
                    rsrc_type,
                    CGM_VSQ_SHRD_OC_RJCT_THm,
                    field_arr_VSQ_SHRD_OC_CLR_TH[rsrc_type],
                    entry_offset,
                    &info->global_shrd_pool_1[rsrc_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
    
    rsrc_type = SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS;
    for (sram_type = 0; sram_type < SOC_TMC_INGRESS_THRESHOLD_NOF_SRAM_TYPES; ++sram_type) {
        for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
            
            entry_offset = color;

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    CGM_GLBL_FR_SRAM_RJCT_THm, 
                    field_arr_GLBL_FR_SRAM_SET_TH[sram_type], 
                    entry_offset,
                    &info->global_free_sram[sram_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    CGM_GLBL_FR_SRAM_RJCT_THm, 
                    field_arr_GLBL_FR_SRAM_CLR_TH[sram_type], 
                    entry_offset,
                    &info->global_free_sram[sram_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);

            
            entry_offset = color + SOC_TMC_NOF_DROP_PRECEDENCE;

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    CGM_GLBL_FR_SRAM_RJCT_THm, 
                    field_arr_GLBL_FR_SRAM_SET_TH[sram_type], 
                    entry_offset,
                    &info->global_free_sram_only[sram_type][color].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    CGM_GLBL_FR_SRAM_RJCT_THm, 
                    field_arr_GLBL_FR_SRAM_CLR_TH[sram_type], 
                    entry_offset,
                    &info->global_free_sram_only[sram_type][color].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    
    for (color = 0; color < SOC_TMC_NOF_DROP_PRECEDENCE; ++color) {
        entry_offset = color;

        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                CGM_GLBL_FR_DRAM_RJCT_THm, 
                DRAM_BDBS_SET_THf, 
                entry_offset,
                &info->global_free_dram[color].set);

        
        rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                rsrc_type,
                CGM_GLBL_FR_DRAM_RJCT_THm, 
                DRAM_BDBS_CLR_THf, 
                entry_offset,
                &info->global_free_dram[color].clear);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


STATIC int
  qax_itm_vsq_fc_verify(
    SOC_SAND_IN int                        unit,
    SOC_SAND_IN SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN int                        pool_id,
    SOC_SAND_IN SOC_TMC_ITM_VSQ_FC_INFO    *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid VSQ group (%d)"), vsq_group_ndx));
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid rate class (%d)"), vsq_rt_cls_ndx));
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid pool id (%d)"), pool_id));
    }

    
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear > (SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control CLEAR threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear));
    }
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set > (SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control SET threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set));
    }
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control CLEAR threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear));
    }
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control SET threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set));
    }
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control CLEAR threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear));
    }
    if (info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control SET threshold is too high (%u)"), 
                    info->size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set));
    }

    
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold > (SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_byte_size_max))) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control SET fadt max threshold is too high (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.min_threshold > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control SET fadt min threshold (%u) can't be above fadt max threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.min_threshold,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.alpha > SOC_TMC_ITM_FADT_ALPHA_MAX || 
            info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.alpha < SOC_TMC_ITM_FADT_ALPHA_MIN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control SET fadt alpha must be between (%d <= alpha <= %d)"), 
                    SOC_TMC_ITM_FADT_ALPHA_MIN, SOC_TMC_ITM_FADT_ALPHA_MAX));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear_offset > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.min_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Total bytes flow control CLEAR fadt offset (%u) can't be above fadt min threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].clear_offset,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].set.min_threshold));
    }

    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control SET fadt max threshold is too high (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.min_threshold > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control SET fadt min threshold (%u) can't be above fadt max threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.min_threshold,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.alpha > SOC_TMC_ITM_FADT_ALPHA_MAX || 
            info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.alpha < SOC_TMC_ITM_FADT_ALPHA_MIN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control SET fadt alpha must be between (%d <= alpha <= %d)"), 
                    SOC_TMC_ITM_FADT_ALPHA_MIN, SOC_TMC_ITM_FADT_ALPHA_MAX));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear_offset > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.min_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-Buffers flow control CLEAR fadt offset (%u) can't be above fadt min threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].clear_offset,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].set.min_threshold));
    }

    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold > SOC_DPP_DEFS_GET(unit, itm_glob_rcs_fc_p0_pd_size_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control SET fadt max threshold is too high (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.min_threshold > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control SET fadt min threshold (%u) can't be above fadt max threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.min_threshold,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.max_threshold));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.alpha > SOC_TMC_ITM_FADT_ALPHA_MAX || 
            info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.alpha < SOC_TMC_ITM_FADT_ALPHA_MIN) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control SET fadt alpha must be between (%d <= alpha <= %d)"), 
                    SOC_TMC_ITM_FADT_ALPHA_MIN, SOC_TMC_ITM_FADT_ALPHA_MAX));
    }
    if (info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear_offset > info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.min_threshold) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("SRAM-PDs flow control CLEAR fadt offset (%u) can't be above fadt min threshold (%u)"), 
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].clear_offset,
                    info->fadt_size_fc[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].set.min_threshold));
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  qax_itm_vsq_fc_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *exact_info
  )
{
    int rv = SOC_E_NONE;
    uint32 entry_offset = 0;
    const soc_mem_t mem_arr_CGM_VSQ_FC_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_VSQ_GROUPS] = 
        { {CGM_VSQA_WORDS_FC_PRMSm, CGM_VSQB_WORDS_FC_PRMSm, CGM_VSQC_WORDS_FC_PRMSm, CGM_VSQD_WORDS_FC_PRMSm, CGM_VSQE_WORDS_FC_PRMSm, CGM_VSQF_WORDS_FC_PRMSm},
          {CGM_VSQA_SRAM_BUFFERS_FC_PRMSm, CGM_VSQB_SRAM_BUFFERS_FC_PRMSm, CGM_VSQC_SRAM_BUFFERS_FC_PRMSm, CGM_VSQD_SRAM_BUFFERS_FC_PRMSm, CGM_VSQE_SRAM_BUFFERS_FC_PRMSm, CGM_VSQF_SRAM_BUFFERS_FC_PRMSm},
          {CGM_VSQA_SRAM_PDS_FC_PRMSm, CGM_VSQB_SRAM_PDS_FC_PRMSm, CGM_VSQC_SRAM_PDS_FC_PRMSm, CGM_VSQD_SRAM_PDS_FC_PRMSm, CGM_VSQE_SRAM_PDS_FC_PRMSm, CGM_VSQF_SRAM_PDS_FC_PRMSm} };
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO pg_fc_fadt_fields;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    
    rv = qax_itm_vsq_fc_verify(unit, vsq_group_ndx, vsq_rt_cls_ndx, pool_id, info);
    SOCDNX_IF_ERR_EXIT(rv);

    entry_offset = vsq_rt_cls_ndx;
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        
        entry_offset += pool_id * SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit);
    }

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        pg_fc_fadt_fields.max_field = FADT_MAX_THf;
        pg_fc_fadt_fields.min_field = FADT_MIN_THf;
        pg_fc_fadt_fields.alpha_field = FADT_ADJUST_FACTORf;

        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            
            rv = qax_itm_vsq_rjct_fadt_set(unit,
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    pg_fc_fadt_fields,
                    entry_offset,
                    info->fadt_size_fc[rsrc_type].set,
                    &exact_info->fadt_size_fc[rsrc_type].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    FADT_OFFSETf, 
                    entry_offset,
                    info->fadt_size_fc[rsrc_type].clear_offset,
                    &exact_info->fadt_size_fc[rsrc_type].clear_offset);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } else {
        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            
            rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    SET_THf, 
                    entry_offset,
                    info->size_fc[rsrc_type].set, 
                    &exact_info->size_fc[rsrc_type].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_set(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    CLR_THf, 
                    entry_offset,
                    info->size_fc[rsrc_type].clear, 
                    &exact_info->size_fc[rsrc_type].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_vsq_fc_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *info
  )
{
    int rv = SOC_E_NONE;
    uint32 entry_offset = 0;
    const soc_mem_t mem_arr_CGM_VSQ_FC_PRMS[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES][SOC_TMC_NOF_VSQ_GROUPS] = 
        { {CGM_VSQA_WORDS_FC_PRMSm, CGM_VSQB_WORDS_FC_PRMSm, CGM_VSQC_WORDS_FC_PRMSm, CGM_VSQD_WORDS_FC_PRMSm, CGM_VSQE_WORDS_FC_PRMSm, CGM_VSQF_WORDS_FC_PRMSm},
          {CGM_VSQA_SRAM_BUFFERS_FC_PRMSm, CGM_VSQB_SRAM_BUFFERS_FC_PRMSm, CGM_VSQC_SRAM_BUFFERS_FC_PRMSm, CGM_VSQD_SRAM_BUFFERS_FC_PRMSm, CGM_VSQE_SRAM_BUFFERS_FC_PRMSm, CGM_VSQF_SRAM_BUFFERS_FC_PRMSm},
          {CGM_VSQA_SRAM_PDS_FC_PRMSm, CGM_VSQB_SRAM_PDS_FC_PRMSm, CGM_VSQC_SRAM_PDS_FC_PRMSm, CGM_VSQD_SRAM_PDS_FC_PRMSm, CGM_VSQE_SRAM_PDS_FC_PRMSm, CGM_VSQF_SRAM_PDS_FC_PRMSm} };
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOC_TMC_ITM_VSQ_FADT_FIELDS_INFO pg_fc_fadt_fields;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = vsq_rt_cls_ndx;
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        
        entry_offset += pool_id * SOC_TMC_ITM_VSQ_NOF_RATE_CLASSES(unit);
    }

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        pg_fc_fadt_fields.max_field = FADT_MAX_THf;
        pg_fc_fadt_fields.min_field = FADT_MIN_THf;
        pg_fc_fadt_fields.alpha_field = FADT_ADJUST_FACTORf;

        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            
            rv = qax_itm_vsq_rjct_fadt_get(unit,
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    pg_fc_fadt_fields,
                    entry_offset,
                    &info->fadt_size_fc[rsrc_type].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    FADT_OFFSETf, 
                    entry_offset,
                    &info->fadt_size_fc[rsrc_type].clear_offset);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    } else {
        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    SET_THf, 
                    entry_offset,
                    &info->size_fc[rsrc_type].set);
            SOCDNX_IF_ERR_EXIT(rv);

            
            rv = qax_itm_vsq_rjct_man_exp_get(unit, 
                    rsrc_type,
                    mem_arr_CGM_VSQ_FC_PRMS[rsrc_type][vsq_group_ndx], 
                    CLR_THf, 
                    entry_offset,
                    &info->size_fc[rsrc_type].clear);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit: 
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_qax_itm_reserved_resource_init(
    int unit
  )
{
    soc_error_t rv;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    int core_id;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.alloc(unit, SOC_DPP_DEFS_MAX(NOF_CORES));
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
        ingress_reserved_resource.dram_reserved = 0;
        ingress_reserved_resource.ocb_reserved = 0;
        for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
            ingress_reserved_resource.reserved[rsrc_type] = 0;
        }
        rv = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.set(unit, core_id, &ingress_reserved_resource);
        SOCDNX_IF_ERR_EXIT(rv);
    }


exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_global_resource_allocation_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  )
{
    uint32 res = SOC_E_NONE;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOCDNX_INIT_FUNC_DEFS;

    ingress_congestion_mgmt->global[SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES].total = QAX_ITM_NOF_BYTES(unit);
    ingress_congestion_mgmt->global[SOC_TMC_INGRESS_THRESHOLD_SRAM_BUFFERS].total = QAX_ITM_NOF_SRAM_BUFFERS(unit);
    ingress_congestion_mgmt->global[SOC_TMC_INGRESS_THRESHOLD_SRAM_PDS].total = QAX_ITM_NOF_SRAM_PDS(unit);

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.get(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        ingress_congestion_mgmt->global[rsrc_type].reserved = ingress_reserved_resource.reserved[rsrc_type];
    }

    
    res = qax_itm_resource_allocation_get(unit, core, ingress_congestion_mgmt);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}



int
  qax_itm_global_resource_allocation_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core, 
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  )
{
    uint32 res = SOC_E_NONE;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.get(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {
        ingress_reserved_resource.reserved[rsrc_type] = ingress_congestion_mgmt->global[rsrc_type].reserved;
    }

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.set(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = qax_itm_resource_allocation_set(unit, core, ingress_congestion_mgmt);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

STATIC int
  qax_itm_resource_allocation_set(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     core_id,
    SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  ) 
{
    int rv = SOC_E_NONE;
    const soc_reg_t reg_arr_CGM_PB_VSQ_OC_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { CGM_PB_VSQ_WORDS_OC_THr, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, CGM_PB_VSQ_SRAM_PDS_OC_THr };
    const soc_field_t field_arr_PB_VSQ_SHRD_OC_TH_POOL_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { PB_VSQ_WORDS_SHRD_OC_TH_POOL_0f, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_0f, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_0f };
    const soc_field_t field_arr_PB_VSQ_SHRD_OC_TH_POOL_1[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { PB_VSQ_WORDS_SHRD_OC_TH_POOL_1f, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_1f, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_1f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_0f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_0f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_0f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_0f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_1[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_1f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_1f };
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    soc_reg_above_64_val_t data;
    int lossless_pool = 0;
    uint32 hdrm_ext_size = 0;
    uint32 resolution;


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ingress_congestion_mgmt);

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {

        
        resolution = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES ? QAX_ITM_WORDS_RESOLUTION : 1);

        SOC_REG_ABOVE_64_CLEAR(data);

        rv = soc_reg_above_64_get(unit, reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], REG_PORT_ANY, 0, data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        soc_reg_above_64_field32_set(unit, 
                                     reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                     data, 
                                     field_arr_PB_VSQ_SHRD_OC_TH_POOL_0[rsrc_type], 
                                     ingress_congestion_mgmt->global[rsrc_type].pool_0/resolution);

        
        soc_reg_above_64_field32_set(unit, 
                                     reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                     data, 
                                     field_arr_PB_VSQ_SHRD_OC_TH_POOL_1[rsrc_type], 
                                     ingress_congestion_mgmt->global[rsrc_type].pool_1/resolution);

        
        if (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) {
            
            soc_reg_above_64_field32_set(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         PB_VSQ_WORDS_HDRM_OC_THf, 
                                         ingress_congestion_mgmt->global[rsrc_type].headroom/resolution);
        } else {
            
            soc_reg_above_64_field32_set(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         field_arr_PB_VSQ_HDRM_OC_TH_0[rsrc_type], 
                                         ingress_congestion_mgmt->global[rsrc_type].nominal_headroom/resolution);

            
            rv = sw_state_access[unit].dpp.soc.qax.tm.lossless_pool_id.get(unit, core_id, &lossless_pool);
            SOCDNX_IF_ERR_EXIT(rv);

            hdrm_ext_size = ingress_congestion_mgmt->global[rsrc_type].headroom - ingress_congestion_mgmt->global[rsrc_type].nominal_headroom;

            
            soc_reg_above_64_field32_set(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_0[rsrc_type], 
                                         (lossless_pool == 0) ? (hdrm_ext_size/resolution) : 0);

            
            soc_reg_above_64_field32_set(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_1[rsrc_type], 
                                         (lossless_pool == 1) ? (hdrm_ext_size/resolution) : 0);
        }

        rv = soc_reg_above_64_set(unit, reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], core_id, 0, data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit: 
    SOCDNX_FUNC_RETURN;
}

STATIC int
  qax_itm_resource_allocation_get(
    SOC_SAND_IN int                     unit,
    SOC_SAND_IN int                     core_id,
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt
  ) 
{
    int rv = SOC_E_NONE;
    const soc_reg_t reg_arr_CGM_PB_VSQ_OC_TH[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { CGM_PB_VSQ_WORDS_OC_THr, CGM_PB_VSQ_SRAM_BUFFERS_OC_THr, CGM_PB_VSQ_SRAM_PDS_OC_THr };
    const soc_field_t field_arr_PB_VSQ_SHRD_OC_TH_POOL_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { PB_VSQ_WORDS_SHRD_OC_TH_POOL_0f, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_0f, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_0f };
    const soc_field_t field_arr_PB_VSQ_SHRD_OC_TH_POOL_1[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { PB_VSQ_WORDS_SHRD_OC_TH_POOL_1f, PB_VSQ_SRAM_BUFFERS_SHRD_OC_TH_POOL_1f, PB_VSQ_SRAM_PDS_SHRD_OC_TH_POOL_1f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_0f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_0f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_0[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_0f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_0f };
    const soc_field_t field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_1[SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] = 
        { -1, PB_VSQ_SRAM_BUFFERS_HDRM_OC_TH_1_POOL_1f, PB_VSQ_SRAM_PDS_HDRM_OC_TH_1_POOL_1f };
    soc_field_t lossless_pool_hdrm_ext_field;
    SOC_TMC_INGRESS_THRESHOLD_TYPE_E rsrc_type = SOC_TMC_INGRESS_THRESHOLD_INVALID;
    soc_reg_above_64_val_t data;
    int lossless_pool = 0;
    uint32 hdrm_ext_size = 0;

    uint32 resolution;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(ingress_congestion_mgmt);

    if (core_id != SOC_CORE_ALL && 
       (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid core id (%d)"), core_id));
    }

    for (rsrc_type = 0; rsrc_type < SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES; ++rsrc_type) {

        resolution = (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES ? QAX_ITM_WORDS_RESOLUTION : 1);

        SOC_REG_ABOVE_64_CLEAR(data);

        rv = soc_reg_above_64_get(unit, reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], REG_PORT_ANY, 0, data);
        SOCDNX_IF_ERR_EXIT(rv);

        
        ingress_congestion_mgmt->global[rsrc_type].pool_0 = 
            soc_reg_above_64_field32_get(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         field_arr_PB_VSQ_SHRD_OC_TH_POOL_0[rsrc_type]
                                        ) * resolution;

        
        ingress_congestion_mgmt->global[rsrc_type].pool_1 = 
            soc_reg_above_64_field32_get(unit, 
                                         reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                         data, 
                                         field_arr_PB_VSQ_SHRD_OC_TH_POOL_1[rsrc_type]
                                        ) * resolution;

        
        if (rsrc_type == SOC_TMC_INGRESS_THRESHOLD_TOTAL_BYTES) {
            
            ingress_congestion_mgmt->global[rsrc_type].headroom = 
                soc_reg_above_64_field32_get(unit, 
                                             reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                             data, 
                                             PB_VSQ_WORDS_HDRM_OC_THf
                                            ) * resolution;

            
            ingress_congestion_mgmt->global[rsrc_type].nominal_headroom = 
                ingress_congestion_mgmt->global[rsrc_type].headroom;
        } else {
            
            ingress_congestion_mgmt->global[rsrc_type].nominal_headroom = 
                soc_reg_above_64_field32_get(unit, 
                                             reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                             data, 
                                             field_arr_PB_VSQ_HDRM_OC_TH_0[rsrc_type]
                                            ) * resolution;

            
            rv = sw_state_access[unit].dpp.soc.qax.tm.lossless_pool_id.get(unit, core_id, &lossless_pool);
            SOCDNX_IF_ERR_EXIT(rv);

            lossless_pool_hdrm_ext_field = (lossless_pool == 0) ? 
                (field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_0[rsrc_type]) : 
                (field_arr_PB_VSQ_HDRM_OC_TH_1_POOL_1[rsrc_type]);

            hdrm_ext_size = 
                soc_reg_above_64_field32_get(unit, 
                                             reg_arr_CGM_PB_VSQ_OC_TH[rsrc_type], 
                                             data, 
                                             lossless_pool_hdrm_ext_field
                                            ) * resolution;

            ingress_congestion_mgmt->global[rsrc_type].headroom = 
                ingress_congestion_mgmt->global[rsrc_type].nominal_headroom + hdrm_ext_size;
        }
    }

exit: 
    SOCDNX_FUNC_RETURN;
}







int qax_itm_credits_adjust_size_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO* additional_info
  ) 
{
    int core_id = 0;
    int delta_internal;
    int offset, delta_total;
    SOCDNX_INIT_FUNC_DEFS;
  
    
    if ((delta > QAX_ITM_SCHEDULER_DELTA_MAX) || (delta < QAX_ITM_SCHEDULER_DELTA_MIN)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Delta %d is out of range"), delta));
    }

    SOC_DPP_CORES_ITER(core, core_id) {

        if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_QUEUE || adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_PORT) { 
            uint32 data;
            
            data = 0;
            SOCDNX_NULL_CHECK(additional_info);
            
            if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_QUEUE) {
            
                SOCDNX_IF_ERR_EXIT(jer_itm_sched_compensation_offset_and_delta_get(unit, 
                                                                                   index, 
                                                                                   delta,  
                                                                                   additional_info->index, 
                                                                                   additional_info->value, 
                                                                                   &offset,
                                                                                   &delta_total));
            }  else {
                
                SOCDNX_IF_ERR_EXIT(jer_itm_sched_compensation_offset_and_delta_get(unit, 
                                                                                   additional_info->index, 
                                                                                   additional_info->value,  
                                                                                   index, 
                                                                                   delta, 
                                                                                   &offset,
                                                                                   &delta_total));

            }

            
            delta_internal = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta_total, EIGHT_BITS);

            soc_mem_field32_set(unit, CGM_SCH_CPMm, &data, HDR_DELTAf, delta_internal);          
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_SCH_CPMm(unit, CGM_BLOCK(unit, core_id), offset, &data));

        } else if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR) { 
            soc_reg_above_64_val_t above64;
            
            
            if (index > QAX_HAP_NOF_ENTRIES) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid append pointer id %d"), index));
            }

            
            delta_internal = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, QAX_ITM_SCHEDULER_DELTA_NOF_BITS);

            SOCDNX_IF_ERR_EXIT(READ_CGM_HAPMm(unit, CGM_BLOCK(unit, core_id), index, above64));
            soc_mem_field32_set(unit, CGM_HAPMm, above64, SCH_DELTAf, delta_internal);          
            SOCDNX_IF_ERR_EXIT(WRITE_CGM_HAPMm(unit, CGM_BLOCK(unit, core_id), index, above64));

        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid adjust_type %d"), adjust_type));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int qax_itm_credits_adjust_size_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_OUT int   *delta
  ) 
{
    int core_id = 0;
    int delta_internal;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_NULL_CHECK(delta);

    core_id = ((core == SOC_CORE_ALL) ? 0 : core);

    
    
    if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR) { 

        soc_reg_above_64_val_t above64;
        
        if (index >= QAX_HAP_NOF_ENTRIES) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid append pointer id %d"), index));
        }

        SOCDNX_IF_ERR_EXIT(READ_CGM_HAPMm(unit, CGM_BLOCK(unit, core_id), index, above64));
        delta_internal = soc_mem_field32_get(unit, CGM_HAPMm, above64, SCH_DELTAf);          

    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid adjust_type %d"), adjust_type));
    }


    
    *delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(delta_internal, QAX_ITM_SCHEDULER_DELTA_NOF_BITS);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_ingress_drop_status(
    SOC_SAND_IN int   unit,
    SOC_SAND_OUT uint32 *is_max_size 
  )
{
    uint64 val;
    uint32 is_words_max_size = 0, is_sram_words_max_size = 0, is_sram_pds_max_size = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_REJECT_STATUS_BITMAPr(unit, SOC_CORE_ALL, &val)); 
    is_words_max_size = soc_reg64_field32_get(unit, CGM_REJECT_STATUS_BITMAPr, val, VOQ_WORDS_MAX_SIZE_RJCTf);
    is_sram_words_max_size = soc_reg64_field32_get(unit, CGM_REJECT_STATUS_BITMAPr, val, VOQ_SRAM_WORDS_MAX_SIZE_RJCTf);
    is_sram_pds_max_size = soc_reg64_field32_get(unit, CGM_REJECT_STATUS_BITMAPr, val, VOQ_SRAM_PDS_MAX_SIZE_RJCTf);
    if (is_words_max_size != 0 || is_sram_words_max_size != 0 || is_sram_pds_max_size != 0) {
        *is_max_size = 1;
    } else {
        *is_max_size = 0;
    }
    
exit:
  SOCDNX_FUNC_RETURN;
}



int
qax_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  )
{
    int i;
    
    soc_field_t sys_red_range_th_field[] = {
        RANGE_TH_0f,
        RANGE_TH_1f,
        RANGE_TH_2f,
        RANGE_TH_3f,
        RANGE_TH_4f,
        RANGE_TH_5f,
        RANGE_TH_6f,
        RANGE_TH_7f,
        RANGE_TH_8f,
        RANGE_TH_9f,
        RANGE_TH_10f,
        RANGE_TH_11f,
        RANGE_TH_12f,
        RANGE_TH_13f,
        RANGE_TH_14f
    };
    
    soc_reg_above_64_val_t data;
    
    itm_mantissa_exp_threshold_info voq_sys_red_range_th_mant_exp = {
        CGM_SYS_RED_QSIZE_RANGESm,
        RANGE_TH_0f,
        8, 
        5, 
        QAX_ITM_WORDS_RESOLUTION 
    };
    
    uint32 max_size_limit = QAX_ITM_QUEUE_SIZE_BYTES_MAX;
    
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);
    
    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }
    
    for (i = 0; i < ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1; i++)
    {
        if (info->queue_size_boundaries[i] >  max_size_limit) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("queue_size_boundaries %u is out of range\n"),
                                               info->queue_size_boundaries[i]));
        }
    }
        
    SOC_REG_ABOVE_64_CLEAR(data);
    
    for (i = 0; i < ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1; i++)
    {
        voq_sys_red_range_th_mant_exp.field_id = sys_red_range_th_field[i];
        SOCDNX_IF_ERR_EXIT(_qax_itm_mantissa_exp_field_set(unit, &voq_sys_red_range_th_mant_exp, 1, data,
                                                           info->queue_size_boundaries[i],
                                                           &(exact_info->queue_size_boundaries[i])));
        
    }
    
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_SYS_RED_QSIZE_RANGESm, MEM_BLOCK_ANY, rt_cls_ndx, data));


exit:
    SOCDNX_FUNC_RETURN;
}


int
  qax_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
    int i;
    
    soc_field_t sys_red_range_th_field[] = {
        RANGE_TH_0f,
        RANGE_TH_1f,
        RANGE_TH_2f,
        RANGE_TH_3f,
        RANGE_TH_4f,
        RANGE_TH_5f,
        RANGE_TH_6f,
        RANGE_TH_7f,
        RANGE_TH_8f,
        RANGE_TH_9f,
        RANGE_TH_10f,
        RANGE_TH_11f,
        RANGE_TH_12f,
        RANGE_TH_13f,
        RANGE_TH_14f
    };
    
    itm_mantissa_exp_threshold_info voq_sys_red_range_th_mant_exp = {
        CGM_SYS_RED_QSIZE_RANGESm,
        RANGE_TH_0f,
        8, 
        5, 
        QAX_ITM_WORDS_RESOLUTION 
    };
    
    soc_reg_above_64_val_t data;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    SOC_REG_ABOVE_64_CLEAR(data);
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_SYS_RED_QSIZE_RANGESm, MEM_BLOCK_ANY, rt_cls_ndx, data));

    for (i = 0; i < ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1; i++)
    {
        voq_sys_red_range_th_mant_exp.field_id = sys_red_range_th_field[i];
        _qax_itm_mantissa_exp_field_get(unit, &voq_sys_red_range_th_mant_exp, data, &(info->queue_size_boundaries[i]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_q_based_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (sys_red_dp_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("dp %d is out of range\n"), sys_red_dp_ndx));
    }

    if (info->adm_th > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("admit threshold %d is out of range\n"), info->adm_th));
    }
    if(info->prob_th > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("probability threshold %d is out of range\n"), info->prob_th));
    }
    if (info->drp_th > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop threshold %d is out of range\n"), info->drp_th));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_q_based_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
    uint64 data64;
    int entry_offset;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    SOCDNX_IF_ERR_EXIT(qax_itm_sys_red_q_based_verify(unit, rt_cls_ndx, sys_red_dp_ndx, info));

    COMPILER_64_ZERO(data64);
    
    entry_offset = (rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + sys_red_dp_ndx;

    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, ADMT_THf, info->adm_th);
    
    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, SYS_RED_ENf, info->enable);
    
    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, PROB_THf, info->prob_th);
    
    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_THf, info->drp_th );
    
    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_PROB_1_THf, info->drp_prob_low);
    
    soc_mem_field32_set(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_PROB_2_THf, info->drp_prob_high);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_SYS_RED_RJCT_PRMSm, MEM_BLOCK_ANY, entry_offset, &data64));

exit:
    SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_q_based_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
    uint64 data64;
    int entry_offset;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    COMPILER_64_ZERO(data64);
    
    entry_offset = (rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + sys_red_dp_ndx;

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_SYS_RED_RJCT_PRMSm, MEM_BLOCK_ANY, entry_offset, &data64));

    info->adm_th = soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, ADMT_THf);

    info->enable = (uint8)soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, SYS_RED_ENf);

    info->prob_th = soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, PROB_THf);

    info->drp_th = soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_THf);

    info->drp_prob_low = soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_PROB_1_THf);

    info->drp_prob_high = soc_mem_field32_get(unit, CGM_SYS_RED_RJCT_PRMSm, &data64, RJCT_PROB_2_THf);

exit:
    SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32    index;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    if (info->thresholds.bds_rng_ths[index] > QAX_ITM_SYS_RED_GLOB_RCS_RNG_THS_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("BDBs threshold #%d (%d) is out of range\n"),
                                           index, info->thresholds.bds_rng_ths[index]));

    if (info->thresholds.ocb_rng_ths[index] > QAX_ITM_SYS_RED_GLOB_RCS_RNG_THS_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OCB buffers threshold #%d (%d) is out of range\n"),
                                           index, info->thresholds.ocb_rng_ths[index]));

    if (info->thresholds.ocb_pdbs_rng_ths[index] > QAX_ITM_SYS_RED_GLOB_RCS_RNG_THS_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OCB PDBs threshold #%d (%d) is out of range\n"),
                                           index, info->thresholds.ocb_pdbs_rng_ths[index]));
  }

  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    if (info->values.bds_rng_vals[index] > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("BDBs value #%d (%d) is out of range\n"),
                                           index, info->values.bds_rng_vals[index]));

    if (info->values.ocb_rng_vals[index] > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OCB buffers value #%d (%d) is out of range\n"),
                                           index, info->values.ocb_rng_vals[index]));

    if (info->values.ocb_pdbs_rng_vals[index] > QAX_ITM_SYS_RED_QUEUE_TH_MAX)
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("OCB PDBs value #%d (%d) is out of range\n"),
                                           index, info->values.ocb_pdbs_rng_vals[index]));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_glob_rcs_set(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info)
{
    soc_reg_above_64_val_t data_above_64;
    uint64 data64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_IF_ERR_EXIT(qax_itm_sys_red_glob_rcs_verify(unit, info));

    SOCDNX_IF_ERR_EXIT(READ_CGM_SYSTEM_RED_RANGE_FR_RSRC_THr(unit, SOC_CORE_ALL, data_above_64));

    

    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_0f, info->thresholds.ocb_rng_ths[0]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_1f, info->thresholds.ocb_rng_ths[1]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_2f, info->thresholds.ocb_rng_ths[2]);

    

    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_0f, info->thresholds.ocb_pdbs_rng_ths[0]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_1f, info->thresholds.ocb_pdbs_rng_ths[1]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_2f, info->thresholds.ocb_pdbs_rng_ths[2]);


    

    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_0f, info->thresholds.bds_rng_ths[0]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_1f, info->thresholds.bds_rng_ths[1]);
    soc_reg_above_64_field32_set(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_2f, info->thresholds.bds_rng_ths[2]);

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SYSTEM_RED_RANGE_FR_RSRC_THr(unit, SOC_CORE_ALL, data_above_64));

    SOCDNX_IF_ERR_EXIT(READ_CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr(unit, SOC_CORE_ALL, &data64));

    
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_0f, info->values.ocb_rng_vals[0]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_1f, info->values.ocb_rng_vals[1]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_2f, info->values.ocb_rng_vals[2]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_3f, info->values.ocb_rng_vals[3]);


    
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_PDBS_SYS_RED_QSIZE_0f, info->values.ocb_pdbs_rng_vals[0]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_PDBS_SYS_RED_QSIZE_1f, info->values.ocb_pdbs_rng_vals[1]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_PDBS_SYS_RED_QSIZE_2f, info->values.ocb_pdbs_rng_vals[2]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_SRAM_PDBS_SYS_RED_QSIZE_3f, info->values.ocb_pdbs_rng_vals[3]);

    

    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_DRAM_BDBS_SYS_RED_QSIZE_0f, info->values.bds_rng_vals[0]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_DRAM_BDBS_SYS_RED_QSIZE_1f, info->values.bds_rng_vals[1]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_DRAM_BDBS_SYS_RED_QSIZE_2f, info->values.bds_rng_vals[2]);
    soc_reg64_field32_set(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, &data64, FR_DRAM_BDBS_SYS_RED_QSIZE_3f, info->values.bds_rng_vals[3]);

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr(unit, SOC_CORE_ALL, data64));

exit:
  SOCDNX_FUNC_RETURN;
}


int
qax_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
    soc_reg_above_64_val_t data_above_64;
    uint64 data64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    SOCDNX_IF_ERR_EXIT(READ_CGM_SYSTEM_RED_RANGE_FR_RSRC_THr(unit, SOC_CORE_ALL, data_above_64));

    

    info->thresholds.ocb_rng_ths[0] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_0f);
    info->thresholds.ocb_rng_ths[1] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_1f);
    info->thresholds.ocb_rng_ths[2] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_BUFFERS_SYS_RED_TH_2f);

    

    info->thresholds.ocb_pdbs_rng_ths[0] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_0f);
    info->thresholds.ocb_pdbs_rng_ths[1] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_1f);
    info->thresholds.ocb_pdbs_rng_ths[2] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_SRAM_PDBS_SYS_RED_TH_2f);

    

    info->thresholds.bds_rng_ths[0] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_0f);
    info->thresholds.bds_rng_ths[1] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_1f);
    info->thresholds.bds_rng_ths[2] = soc_reg_above_64_field32_get(unit, CGM_SYSTEM_RED_RANGE_FR_RSRC_THr, data_above_64, FR_DRAM_BDBS_SYS_RED_TH_2f);

    SOCDNX_IF_ERR_EXIT(READ_CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr(unit, SOC_CORE_ALL, &data64));

    
    info->values.ocb_rng_vals[0] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_0f);
    info->values.ocb_rng_vals[1] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_1f);
    info->values.ocb_rng_vals[2] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_2f);
    info->values.ocb_rng_vals[3] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_BUFFERS_SYS_RED_QSIZE_3f);

    
    info->values.ocb_pdbs_rng_vals[0] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_PDBS_SYS_RED_QSIZE_0f);
    info->values.ocb_pdbs_rng_vals[1] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_PDBS_SYS_RED_QSIZE_1f);
    info->values.ocb_pdbs_rng_vals[2] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_PDBS_SYS_RED_QSIZE_2f);
    info->values.ocb_pdbs_rng_vals[3] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_SRAM_PDBS_SYS_RED_QSIZE_3f);

    

    info->values.bds_rng_vals[0] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_DRAM_BDBS_SYS_RED_QSIZE_0f);
    info->values.bds_rng_vals[1] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_DRAM_BDBS_SYS_RED_QSIZE_1f);
    info->values.bds_rng_vals[2] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_DRAM_BDBS_SYS_RED_QSIZE_2f);
    info->values.bds_rng_vals[3] = soc_reg64_field32_get(unit, CGM_SYSTEM_RED_QSIZE_FR_RSRC_THr, data64, FR_DRAM_BDBS_SYS_RED_QSIZE_3f);

exit:
  SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

