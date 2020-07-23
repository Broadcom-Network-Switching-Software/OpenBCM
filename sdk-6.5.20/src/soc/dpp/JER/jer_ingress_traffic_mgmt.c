#include <shared/bsl.h>
#include <soc/mcm/memregs.h> 
#if defined(BCM_88675_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ



#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h> 
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/JER/jer_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_general.h> 
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/JER/jer_cnt.h>
#include <shared/swstate/access/sw_state_access.h>

#define JER_ITM_FLUSH_BYTES_UNITS_TO_SEND   (128)

#define JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE 4


#define JER_ITM_VSQ_FC_BD_SIZE_MAX              SOC_TMC_ITM_VSQ_FC_BD_SIZE_MAX
#define JER_ITM_VSQ_GRNT_BD_SIZE_MAX            SOC_TMC_ITM_VSQ_GRNT_BD_SIZE_MAX


#define JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB   6
#define JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB   0
#define JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS (JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB - JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB + 1)

#define JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB   11
#define JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB   7
#define JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS (JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB - JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB + 1)


#define JER_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX 0x1FFFFF

#define JER_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX 0xFFFF

#define JER_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX 0x3FFFF

#define JER_ITM_SYS_RED_GLOB_RCS_THS_OCB_RNG_THS_MAX 0x3FFFF


#define JER_ITM_SCHEDULER_DELTA_MAX                 (127)
#define JER_ITM_SCHEDULER_DELTA_MIN                 (-128)
#define JER_ITM_SCHEDULER_DELTA_NOF_BITS            (8)
#define IQM_SCH_HAP_NOF_ENTRIES                     (256)



#define JER_ITM_VOQ_VOQ_GRNTD_RJCT_LOOSE_MASK       0x582
#define JER_ITM_VOQ_VOQ_GRNTD_RJCT_STRICT_MASK      0xD82
#define JER_ITM_VOQ_VSQ_GRNTD_RJCT_LOOSE_MASK       0xFE0
#define JER_ITM_VOQ_VSQ_GRNTD_RJCT_STRICT_MASK      0xFE2
#define JER_ITM_VOQ_VSQ_LOSSLESS_RJCT_MASK          0xFE3
#define JER_ITM_VOQ_ECN_CAPABLE_RJCT_MASK           (0x1 << 7)    



#define JER_ITM_VSQ_RJCT_ALL_MASK 0x3F 



#define JER_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX 15

soc_field_t sys_red_free_mnmc_db_th_f[ARAD_ITM_SYS_RED_BUFFS_RNGS - 1] = {
    FR_MNMC_DB_TH_0f,
    FR_MNMC_DB_TH_1f,
    FR_MNMC_DB_TH_2f,
};
soc_field_t sys_red_free_mnmc_rng_val_f[ARAD_ITM_SYS_RED_BUFFS_RNGS] = {
    FR_MNMC_DB_RNG_VAL_0f,
    FR_MNMC_DB_RNG_VAL_1f,
    FR_MNMC_DB_RNG_VAL_2f,
    FR_MNMC_DB_RNG_VAL_3f,
}; 
soc_field_t sys_red_free_flmc_db_th_f[ARAD_ITM_SYS_RED_BUFFS_RNGS - 1] = {
    FR_FLMC_DB_TH_0f,
    FR_FLMC_DB_TH_1f,
    FR_FLMC_DB_TH_2f,
};

soc_field_t sys_red_free_flmc_rng_val_f[ARAD_ITM_SYS_RED_BUFFS_RNGS] = {
    FR_FLMC_DB_RNG_VAL_0f,
    FR_FLMC_DB_RNG_VAL_1f,
    FR_FLMC_DB_RNG_VAL_2f,
    FR_FLMC_DB_RNG_VAL_3f,
};
soc_field_t sys_red_free_ocb_db_th_f[ARAD_ITM_SYS_RED_BUFFS_RNGS - 1] = {
    FR_OCB_TH_0f,
    FR_OCB_TH_1f,
    FR_OCB_TH_2f,
};
soc_field_t sys_red_free_ocb_rng_val_f[ARAD_ITM_SYS_RED_BUFFS_RNGS] = {
    FR_OCB_RNG_VAL_0f,
    FR_OCB_RNG_VAL_1f,
    FR_OCB_RNG_VAL_2f,
    FR_OCB_RNG_VAL_3f,
};
soc_field_t sys_red_free_dbd_db_th_f[ARAD_ITM_SYS_RED_BUFFS_RNGS - 1] = {
    FR_BDB_TH_0f,
    FR_BDB_TH_1f,
    FR_BDB_TH_2f,
};
soc_field_t sys_red_free_dbd_rng_val_f[ARAD_ITM_SYS_RED_BUFFS_RNGS] = {
    FR_BDB_RNG_VAL_0f,
    FR_BDB_RNG_VAL_1f,
    FR_BDB_RNG_VAL_2f,
    FR_BDB_RNG_VAL_3f,
};

STATIC int
  jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param,
     SOC_SAND_INOUT SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  );

STATIC int
  jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN  SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
     SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param
  );

STATIC uint32
  jer_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  );


#define JER_ITM_MC_FIFO_MIN_THRESHOLD                           (9)
#define JER_ITM_MC_HP_FIFO_DP_0_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD)
#define JER_ITM_MC_HP_FIFO_DP_1_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 1 / 4)
#define JER_ITM_MC_HP_FIFO_DP_2_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 2 / 4)
#define JER_ITM_MC_HP_FIFO_DP_3_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 3 / 4)
#define JER_ITM_MC_LP_FIFO_DP_0_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD)
#define JER_ITM_MC_LP_FIFO_DP_1_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 1 / 4)
#define JER_ITM_MC_LP_FIFO_DP_2_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 2 / 4)
#define JER_ITM_MC_LP_FIFO_DP_3_THRESHOLD(limit)                (JER_ITM_MC_FIFO_MIN_THRESHOLD + ((limit) - JER_ITM_MC_FIFO_MIN_THRESHOLD) * 3 / 4)

#define JER_ITM_NON_PHY_IF_FOR_VSQ_E_F(interface_type) (interface_type == SOC_PORT_IF_RCY || interface_type == SOC_PORT_IF_CPU || interface_type == SOC_PORT_IF_OLP)

STATIC int
  jer_itm_regs_init(int unit)
{
    uint32
        data;
    uint64
        data64;
    char 
        *propkey,
        *propval;
    uint32 
        ftmh_stmping; 
    int 
        core;
    uint32 
          mcr_limit_uc,
          mcr_limit_mc_hp;
    int
          mcr_limit_mc_hp_size,
          mcr_limit_mc_lp_size;
    soc_reg_above_64_val_t 
          fld_above_64_val, 
          reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(BCM_E_UNIT); 
    }

    
    SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
    soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, STAMP_FAP_PORTf, 1);
    soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, STAMP_FWDACTIONf, 1);
    if (SOC_DPP_CONFIG(unit)->arad->init.mirror_stamp_sys_dsp_ext) {
        soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, STAMP_DSP_EXTf, 0); 
    }
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, data64));

    SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FTMH_OUTLIF_ENABLEr(unit, &data));
    soc_reg_field_set(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, &data, STAMP_FTMH_OUTLIF_TYPE_ENf, 0x7745);
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_FTMH_OUTLIF_ENABLEr(unit, data));

    
    propkey = spn_XGS_COMPATABILITY_STAMP_CUD;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "True") == 0) {

            SOCDNX_IF_ERR_EXIT(READ_IPT_ARAD_PACKETS_FORMATr(unit, &data));
            soc_reg_field_set(unit, IPT_ARAD_PACKETS_FORMATr, &data, STAMP_OUTLIF_XGS_USR_DEF_ENf, 1);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_ARAD_PACKETS_FORMATr(unit, data));

            SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
            soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, STAMP_USER_DEFINED_LSBf, 512-72-48);
            soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, STAMP_USER_DEFINED_MSBf, 16);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, data64));

            SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_USR_DEF_OUTLIF_ENABLEr(unit, &data));
            soc_reg_field_set(unit, IPT_STAMPING_USR_DEF_OUTLIF_ENABLEr, &data, STAMP_USR_DEF_OUTLIF_TYPE_ENf, 0);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_USR_DEF_OUTLIF_ENABLEr(unit, data));

            SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FTMH_OUTLIF_ENABLEr(unit, &data));
            ftmh_stmping = soc_reg_field_get(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, data, STAMP_FTMH_OUTLIF_TYPE_ENf);
            soc_reg_field_set(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, &data, STAMP_FTMH_OUTLIF_TYPE_ENf, ftmh_stmping & ~0x3);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_FTMH_OUTLIF_ENABLEr(unit, data));

        } else if (sal_strcmp(propval, "False") == 0) {
                    
        } else {            
            return SOC_E_FAIL;
        }
    }

    SOCDNX_IF_ERR_EXIT(READ_IPT_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr(unit, &data));
    soc_reg_field_set(unit, IPT_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr, &data, SNOOP_TABLEf, SOC_SAND_BIT(ARAD_ITM_FWD_ACTION_TYPE_SNOOP));
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr(unit, data));

    
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(READ_IPS_STORED_CREDITS_USAGE_CONFIGURATIONr(unit, core, &data64));
        soc_reg64_field32_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, &data64, MUL_PKT_DEQ_BYTESf, 16);
        SOCDNX_IF_ERR_EXIT(WRITE_IPS_STORED_CREDITS_USAGE_CONFIGURATIONr(unit, core, data64));
    }
      

     BCM_DPP_CORES_ITER(BCM_CORE_ALL, core) {

         SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
         SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);

         SOCDNX_IF_ERR_EXIT(READ_IRR_MULTICAST_FIFO_THRESHOLDSr(unit, core, reg_above_64_val));

         
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, core, MCR_N_LIMIT_UCf, &mcr_limit_uc));    
         SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, core, MCR_N_LIMIT_MC_HPf, &mcr_limit_mc_hp));
         mcr_limit_mc_hp_size = mcr_limit_mc_hp;
         mcr_limit_mc_lp_size = (768 - mcr_limit_mc_hp  - mcr_limit_uc);

         

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_HP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_hp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_HP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_hp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_HP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_hp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_HP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_hp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_LP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_lp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_LP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_lp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_LP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_lp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val);

         soc_reg_above_64_field_get(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val);
         *fld_above_64_val = JER_ITM_MC_LP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_lp_size);
         soc_reg_above_64_field_set(unit, IRR_MULTICAST_FIFO_THRESHOLDSr, reg_above_64_val, MCR_N_MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val);
       
         SOCDNX_IF_ERR_EXIT(WRITE_IRR_MULTICAST_FIFO_THRESHOLDSr(unit, core, reg_above_64_val));
     }

     
     SOCDNX_IF_ERR_EXIT(READ_IRR_OUTLIF_SELECTIONr(unit, &data));
     soc_reg_field_set(unit, IRR_OUTLIF_SELECTIONr, &data, OUTLIF_MIRROR_FABRIC_MC_SOURCEf, 0x4);
     SOCDNX_IF_ERR_EXIT(WRITE_IRR_OUTLIF_SELECTIONr(unit, data));

exit:
     SOCDNX_FUNC_RETURN; 
}

STATIC int
  jer_itm_pdm_extension_config(
     SOC_SAND_IN  int  unit
     ){
    uint32
        res = SOC_E_NONE;
    uint32 pdm_data = 0;
    uint32 max_pp_port[1], max_st_vsq[1], max_hdr_comp_ptr[1];
    uint32 max_pp_ports_bit = 0, max_st_vsqs_bit = 0, max_hdr_comp_ptrs_bit = 0;
    uint32 pp_ports_mask = 0, st_vsqs_mask = 0, hdr_comp_ptrs_mask = 0;
    uint32 pp_ports_shift = 0, st_vsqs_shift = 0; 
    uint32 index;
    SOC_TMC_INIT_PDM_MODE pdm_mode = SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(BCM_E_UNIT); 
    }
    
    res = soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, WIDE_MODE_ENf, (pdm_mode != SOC_TMC_INIT_PDM_MODE_SIMPLE)); 
    SOCDNX_IF_ERR_EXIT(res);
    if (!SOC_IS_QAX(unit) && 
        pdm_mode != SOC_TMC_INIT_PDM_MODE_SIMPLE) {
        
        max_pp_port[0] = SOC_DPP_CONFIG(unit)->pdm_extension.max_pp_port;
        
        max_st_vsq[0] = SOC_DPP_CONFIG(unit)->pdm_extension.max_st_vsq;
        
        max_hdr_comp_ptr[0] = SOC_DPP_CONFIG(unit)->pdm_extension.max_hdr_comp_ptr;
        
        
        SHR_BIT_ITER(max_pp_port, SHR_BITWID, index) max_pp_ports_bit = (index + 1);
        SHR_BIT_ITER(max_st_vsq, SHR_BITWID, index) max_st_vsqs_bit = (index + 1);
        SHR_BIT_ITER(max_hdr_comp_ptr, SHR_BITWID, index) max_hdr_comp_ptrs_bit = (index + 1);
        if (max_pp_ports_bit > soc_reg_field_length(unit, IQM_PDM_BD_EXT_CFGr, BD_EXT_IPP_PORT_MASKf)) {
            LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "The number of bits needed for source-PP-port %d is too high\n"), max_pp_ports_bit));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (max_st_vsqs_bit > soc_reg_field_length(unit, IQM_PDM_BD_EXT_CFGr, BD_EXT_ST_VSQ_PTR_MASKf)) {
            LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "The number of bits needed for ST-VSQs %d is too high\n"), max_st_vsqs_bit));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (max_hdr_comp_ptrs_bit > soc_reg_field_length(unit, IQM_PDM_BD_EXT_CFGr, BD_EXT_SCH_CMP_PTR_MASKf)) {
            LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "The number of bits needed for header-compensation-pointer %d is too high\n"), max_hdr_comp_ptrs_bit));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        
        if (max_pp_ports_bit + max_st_vsqs_bit + max_hdr_comp_ptrs_bit > SOC_DPP_DEFS_GET(unit, size_of_pdm_extension)) {
            LOG_ERROR(BSL_LS_SOC_COSQ, 
                      (BSL_META_U(unit, "The sum of bits needed of source-PP-port %d, ST-VSQs %d, and header-compensation-pointer %d is %d above %d\n"), 
                       max_pp_ports_bit, 
                       max_st_vsqs_bit, 
                       max_hdr_comp_ptrs_bit, 
                       max_pp_ports_bit + max_st_vsqs_bit + max_hdr_comp_ptrs_bit,
                       SOC_DPP_DEFS_GET(unit, size_of_pdm_extension)));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        
        pp_ports_mask = ((1 << max_pp_ports_bit) - 1);
        st_vsqs_mask = ((1 << max_st_vsqs_bit) - 1);
        hdr_comp_ptrs_mask = ((1 << max_hdr_comp_ptrs_bit) - 1);
        
        pp_ports_shift = max_hdr_comp_ptrs_bit;
        st_vsqs_shift = max_hdr_comp_ptrs_bit + max_pp_ports_bit;
        

        res = READ_IQM_PDM_BD_EXT_CFGr(unit, SOC_CORE_ALL, &pdm_data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_reg_field_set(unit, IQM_PDM_BD_EXT_CFGr, &pdm_data, BD_EXT_IPP_PORT_MASKf, pp_ports_mask);
        soc_reg_field_set(unit, IQM_PDM_BD_EXT_CFGr, &pdm_data, BD_EXT_IPP_PORT_SHIFTf, pp_ports_shift);
        soc_reg_field_set(unit, IQM_PDM_BD_EXT_CFGr, &pdm_data, BD_EXT_ST_VSQ_PTR_MASKf, st_vsqs_mask);
        soc_reg_field_set(unit, IQM_PDM_BD_EXT_CFGr, &pdm_data, BD_EXT_ST_VSQ_PTR_SHIFTf, st_vsqs_shift);
        soc_reg_field_set(unit, IQM_PDM_BD_EXT_CFGr, &pdm_data, BD_EXT_SCH_CMP_PTR_MASKf, hdr_comp_ptrs_mask);

        res = WRITE_IQM_PDM_BD_EXT_CFGr(unit, SOC_CORE_ALL, pdm_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit: 
  SOCDNX_FUNC_RETURN; 
}


STATIC
int jer_itm_fc_global_thresholds_init(int unit)
{
    SOC_TMC_ITM_GLOB_RCS_FC_TH glbl_fc, glbl_fc_exact;
    int res = SOC_E_NONE;
    uint64 reg64;
    ARAD_MGMT_INIT *init;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
    SOC_TMC_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

    
    if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){
        
        glbl_fc.bdbs.hp.set       = 192;
        glbl_fc.bdbs.hp.clear     = 202;
        glbl_fc.bdbs.lp.set       = 256;
        glbl_fc.bdbs.lp.clear     = 266;

        
        glbl_fc.full_mc.hp.set    = 416;
        glbl_fc.full_mc.hp.clear  = 832;
        glbl_fc.full_mc.lp.set    = 672;
        glbl_fc.full_mc.lp.clear  = 1344;

        
        glbl_fc.mini_mc.hp.set = 1680;
        glbl_fc.mini_mc.hp.clear = 1640;
        glbl_fc.mini_mc.lp.set = 2100;
        glbl_fc.mini_mc.lp.clear = 2050;
    }

    
    glbl_fc.ocb.hp.set = 220;
    glbl_fc.ocb.hp.clear = 270;
    glbl_fc.ocb.lp.set = 270;
    glbl_fc.ocb.lp.clear = 320;

    res = arad_itm_glob_rcs_fc_set_unsafe(unit, &glbl_fc, &glbl_fc_exact);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    
    
    COMPILER_64_ZERO(reg64);
    COMPILER_64_SET(reg64, 0xffff, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_0_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_MIX_SHRD_POOL_1_RSRC_FC_THr(unit, SOC_CORE_ALL, reg64));

exit: 
    SOCDNX_FUNC_RETURN; 
}



STATIC
int jer_itm_drop_global_thresholds_init(int unit)
{
    SOC_TMC_ITM_GLOB_RCS_DROP_TH glbl_drop, glbl_drop_exact;
    ARAD_MGMT_INIT *init;
    uint32 mem_excess_size, idx, idx2;
    uint32 pdm_nof_entries, pdm_nof_entries_per_core;
    int core_index;
    int res = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    init = &(SOC_DPP_CONFIG(unit)->arad->init);

    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core_index) {

        
        SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
        SOC_TMC_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);

        
        glbl_drop.bdbs[0].set       = 128; 
        glbl_drop.bdbs[0].clear     = 130;
        glbl_drop.ocb_bdbs[0].set   = 14;
        glbl_drop.ocb_bdbs[0].clear = 16;
        glbl_drop.bdbs[1].set       = 156; 
        glbl_drop.bdbs[1].clear     = 158;
        glbl_drop.ocb_bdbs[1].set   = 20;
        glbl_drop.ocb_bdbs[1].clear = 22;
        glbl_drop.bdbs[2].set       = 192; 
        glbl_drop.bdbs[2].clear     = 194;
        glbl_drop.ocb_bdbs[2].set   = 26;
        glbl_drop.ocb_bdbs[2].clear = 28;
        glbl_drop.bdbs[3].set       = 232; 
        glbl_drop.bdbs[3].clear     = 234;
        glbl_drop.ocb_bdbs[3].set   = 32;
        glbl_drop.ocb_bdbs[3].clear = 34;


        
        res = arad_init_pdm_nof_entries_calc(unit, init->dram.pdm_mode, &pdm_nof_entries);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        pdm_nof_entries_per_core = pdm_nof_entries / (SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores);

        
        glbl_drop.bds[0].set       = ((pdm_nof_entries_per_core * 995) / 1024);  
        glbl_drop.bds[0].clear     = glbl_drop.bds[0].set; 
        glbl_drop.ocb_bds[0].set   = ((pdm_nof_entries_per_core * 1016) / 1024); 
        glbl_drop.ocb_bds[0].clear = glbl_drop.ocb_bds[0].set;
        glbl_drop.bds[1].set       = ((pdm_nof_entries_per_core * 988) / 1024);  
        glbl_drop.bds[1].clear     = glbl_drop.bds[1].set; 
        glbl_drop.ocb_bds[1].set   = ((pdm_nof_entries_per_core * 1009) / 1024); 
        glbl_drop.ocb_bds[1].clear = glbl_drop.ocb_bds[1].set;
        glbl_drop.bds[2].set       = ((pdm_nof_entries_per_core * 981) / 1024);  
        glbl_drop.bds[2].clear     = glbl_drop.bds[2].set; 
        glbl_drop.ocb_bds[2].set   = ((pdm_nof_entries_per_core * 1002) / 1024); 
        glbl_drop.ocb_bds[2].clear = glbl_drop.ocb_bds[2].set;
        glbl_drop.bds[3].set       = ((pdm_nof_entries_per_core * 981) / 1024);  
        glbl_drop.bds[3].clear     = glbl_drop.bds[3].set; 
        glbl_drop.ocb_bds[3].set   = ((pdm_nof_entries_per_core * 1002) / 1024); 
        glbl_drop.ocb_bds[3].clear = glbl_drop.ocb_bds[3].set;  
                                    
    
        
        if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){

            
            for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
            {
                 glbl_drop.full_mc[idx].set   = 160;
                 glbl_drop.full_mc[idx].clear = 320;
            }
    
            
            glbl_drop.mini_mc[0].set   = 260; 
            glbl_drop.mini_mc[0].clear = 310;
            glbl_drop.mini_mc[1].set   = 390; 
            glbl_drop.mini_mc[1].clear = 440;
            glbl_drop.mini_mc[2].set   = 520; 
            glbl_drop.mini_mc[2].clear = 570;
            glbl_drop.mini_mc[3].set   = 650; 
            glbl_drop.mini_mc[3].clear = 700;                        
            
            
            for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
            {
                glbl_drop.mem_excess[idx].set   = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
                glbl_drop.mem_excess[idx].clear = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
    
                glbl_drop.ocb_mem_excess[idx].set   = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
                glbl_drop.ocb_mem_excess[idx].clear = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
            }
        } else {
            
            mem_excess_size = (((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024 * 1024 ) / 8 ) / 128 );
            for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++) {
                glbl_drop.mem_excess[idx].set   = mem_excess_size;
                glbl_drop.mem_excess[idx].clear = mem_excess_size - (mem_excess_size / 8);
    
                glbl_drop.ocb_mem_excess[idx].set   = mem_excess_size;
                glbl_drop.ocb_mem_excess[idx].clear = mem_excess_size - (mem_excess_size / 8);
            }
        }
        
         
        for (idx = 0; idx < SOC_TMC_NOF_DROP_PRECEDENCE; idx++) {
            for (idx2 = 0; idx2 < SOC_TMC_ITM_NOF_RSRC_POOLS; idx2++) {
                glbl_drop.ocb_shrd_pool[idx2][idx].clear = SOC_TMC_ITM_SHRD_RJCT_TH_MAX;
                glbl_drop.ocb_shrd_pool[idx2][idx].set = SOC_TMC_ITM_SHRD_RJCT_TH_MAX;
                glbl_drop.mix_shrd_pool[idx2][idx].clear = SOC_TMC_ITM_SHRD_RJCT_TH_MAX;
                glbl_drop.mix_shrd_pool[idx2][idx].set = SOC_TMC_ITM_SHRD_RJCT_TH_MAX;
            }
        }

        
        glbl_drop.ocb_mc[0].set = 70;     
        glbl_drop.ocb_mc[0].clear = 120; 
        glbl_drop.ocb_mc[1].set = 130;    
        glbl_drop.ocb_mc[1].clear = 180;
        glbl_drop.ocb_mc[2].set = 190;    
        glbl_drop.ocb_mc[2].clear = 240;        
        glbl_drop.ocb_mc[3].set = 250;    
        glbl_drop.ocb_mc[3].clear = 300;          
                   
        res = jer_itm_glob_rcs_drop_set(unit, core_index, &glbl_drop, &glbl_drop_exact);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  jer_itm_global_resource_allocation_get(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN int core, 
      SOC_SAND_OUT SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt)
{
    uint32 res = SOC_E_NONE;
    uint8 is_ocb_only = 0;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE *resource;
    soc_jer_ocb_dram_dbuffs_bound_t *dbuffs_bdries;

    SOCDNX_INIT_FUNC_DEFS;

    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.get(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

    if (core == 0) { 
        ingress_congestion_mgmt->ocb.total = dbuffs_bdries->ocb_0.size;
        ingress_congestion_mgmt->dram.total = dbuffs_bdries->mnmc_0.size; 
    } else { 
        ingress_congestion_mgmt->ocb.total = dbuffs_bdries->ocb_1.size;
        ingress_congestion_mgmt->dram.total = dbuffs_bdries->mnmc_1.size; 
    }

    for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
        resource = is_ocb_only ? (&ingress_congestion_mgmt->ocb) : (&ingress_congestion_mgmt->dram);
        resource->reserved = is_ocb_only ? (ingress_reserved_resource.ocb_reserved) : (ingress_reserved_resource.dram_reserved);
        res = jer_itm_resource_allocation_get(unit, core, 0, is_ocb_only, &resource->pool_0, &resource->headroom);
        SOCDNX_IF_ERR_EXIT(res);
        res = jer_itm_resource_allocation_get(unit, core, 1, is_ocb_only, &resource->pool_1, &resource->headroom);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}



int
  jer_itm_global_resource_allocation_set(
      SOC_SAND_IN int unit, 
      SOC_SAND_IN int core, 
      SOC_SAND_IN SOC_TMC_ITM_INGRESS_CONGESTION_MGMT *ingress_congestion_mgmt)
{
    uint32 res = SOC_E_NONE;
    uint8 is_ocb_only = 0;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    const SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE *resource;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(&ingress_reserved_resource, 0x0, sizeof(ingress_reserved_resource));

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.get(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

    for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
        resource = is_ocb_only ? (&ingress_congestion_mgmt->ocb) : (&ingress_congestion_mgmt->dram);
        is_ocb_only ? (ingress_reserved_resource.ocb_reserved = resource->reserved) : (ingress_reserved_resource.dram_reserved = resource->reserved);
        res = jer_itm_resource_allocation_set(unit, core, 0, is_ocb_only, resource->pool_0, resource->headroom);
        SOCDNX_IF_ERR_EXIT(res);
        res = jer_itm_resource_allocation_set(unit, core, 1, is_ocb_only, resource->pool_1, resource->headroom);
        SOCDNX_IF_ERR_EXIT(res);
    }

    res = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.set(unit, core, &ingress_reserved_resource);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

STATIC int
soc_jer_itm_reserved_resource_init(int unit)
{
    soc_error_t rv;
    SOC_TMC_ITM_INGRESS_RESERVED_RESOURCE ingress_reserved_resource;
    int core_id;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.alloc(unit, SOC_DPP_DEFS_MAX(NOF_CORES));
    SOCDNX_IF_ERR_EXIT(rv);

    sal_memset(&ingress_reserved_resource,0,sizeof(ingress_reserved_resource));
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_id) {
        ingress_reserved_resource.dram_reserved = 0;
        ingress_reserved_resource.ocb_reserved = 0;
        rv = sw_state_access[unit].dpp.soc.jericho.tm.ingress_reserved_resource.set(unit, core_id, &ingress_reserved_resource);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  jer_itm_init(
    SOC_SAND_IN  int  unit
  )
{
    int
      res = SOC_E_NONE;
    uint32
        data32, data = 0, data1, data2, data3, data4;
    SOC_TMC_ITM_VSQ_GROUP
      vsq_group;
    SOC_TMC_ITM_VSQ_FC_INFO
      vsq_fc_info,
      exact_vsq_fc_info;
    int 
      pool_id;
    int 
        index,
        core_index;
    soc_dpp_guaranteed_q_resource_t 
        guaranteed_q_resource;
    SOC_TMC_ITM_INGRESS_CONGESTION_MGMT 
        ingress_congestion_mgmt;
    SOC_TMC_ITM_INGRESS_CONGESTION_RESOURCE 
        *resource;
    SOC_TMC_ITM_GLOB_RCS_DROP_TH
        glbl_drop_info;
    int 
        is_ocb_only;
    soc_reg_above_64_val_t 
        reg_above_64;
    uint64 
        reg64, data64;
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO 
        admition_test_template_info;
    soc_jer_ocb_dram_dbuffs_bound_t
        *dbuffs_bdries;
    int table_data[10];
    soc_mem_t mem;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(BCM_E_UNIT); 
    }

    dbuffs_bdries = &SOC_DPP_CONFIG(unit)->jer->dbuffs.dbuffs_bdries;
    
    
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr(unit, SOC_CORE_ALL, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr(unit, SOC_CORE_ALL, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_REJECT_CONFIGURATION_OCBSr(unit, SOC_CORE_ALL, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr(unit, SOC_CORE_ALL, reg_above_64));
    
    
    reg_above_64[2] = reg_above_64[1] = reg_above_64[0] = 0xffffffff;
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_GENERAL_REJECT_CONFIGURATION_B_DSr(unit, SOC_CORE_ALL, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
    
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_OCB_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr(unit, SOC_CORE_ALL, reg_above_64));
    
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_DEQ_INTERNAL_SETTINGr(unit, SOC_CORE_ALL, 0xffff));
    
    reg_above_64[2] = 3;
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_GLBL_MIX_MAX_THr(unit, SOC_CORE_ALL, reg_above_64));
    COMPILER_64_SET(reg64, 0xffff, 0xffffffff);
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, SOC_CORE_ALL, reg64));
    
    res = jer_itm_regs_init(unit);
    SOCDNX_IF_ERR_EXIT(res);
    
    res = sw_state_access[unit].dpp.soc.jericho.tm.nof_remote_faps_with_remote_credit_value.set(unit, 0);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO_clear(&admition_test_template_info);
    for (index = 0; index < SOC_TMC_ITM_ADMIT_TSTS_LAST; index++) {
        res = jer_itm_admit_test_tmplt_set(unit, SOC_CORE_ALL, index, &admition_test_template_info);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
    
    
    res = jer_itm_pdm_extension_config(unit);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    res = READ_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQA_ENf, TRUE);
    soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQB_ENf, TRUE);
    soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQC_ENf, TRUE);
    
    
    if (SOC_DPP_CONFIG(unit)->arad->init.dram.pdm_mode != SOC_TMC_INIT_PDM_MODE_SIMPLE) {
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQD_ENf, TRUE);
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQE_ENf, TRUE);
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQF_ENf, TRUE);
    } else {
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQD_ENf, FALSE);
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQE_ENf, FALSE);
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VSQF_ENf, FALSE);
    }
    
    
    soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, LOSSY_BLOCK_STRICTf, FALSE);
    soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, LOSSLESS_BLOCK_STRICTf, FALSE);
    
    
    if (SOC_DPP_CONFIG(unit)->jer->tm.cgm_mgmt_guarantee_mode == SOC_TMC_ITM_CGM_MGMT_GUARANTEE_LOOSE) {
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VOQ_GRNTD_BLOCK_STRICTf, FALSE);
    } else if (SOC_DPP_CONFIG(unit)->jer->tm.cgm_mgmt_guarantee_mode == SOC_TMC_ITM_CGM_MGMT_GUARANTEE_STRICT) {
        soc_reg_field_set(unit, IQM_STE_ENABLERSr, &data, VOQ_GRNTD_BLOCK_STRICTf, TRUE);
    }
    res = WRITE_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL, data);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    
    COMPILER_64_ZERO(reg64);
    res = READ_IQM_RJCT_GLBL_TESTS_MASKr(unit, SOC_CORE_ALL, &reg64);
    SOCDNX_IF_ERR_EXIT(res);
    if (SOC_DPP_CONFIG(unit)->jer->tm.cgm_mgmt_guarantee_mode == SOC_TMC_ITM_CGM_MGMT_GUARANTEE_LOOSE) { 
        soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, VOQ_GRNTD_RJCT_MASKf, JER_ITM_VOQ_VOQ_GRNTD_RJCT_LOOSE_MASK);
        soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, VSQ_GRNTD_RJCT_MASKf, JER_ITM_VOQ_VSQ_GRNTD_RJCT_LOOSE_MASK);
    } else if (SOC_DPP_CONFIG(unit)->jer->tm.cgm_mgmt_guarantee_mode == SOC_TMC_ITM_CGM_MGMT_GUARANTEE_STRICT) { 
        soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, VOQ_GRNTD_RJCT_MASKf, JER_ITM_VOQ_VOQ_GRNTD_RJCT_STRICT_MASK);
        soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, VSQ_GRNTD_RJCT_MASKf, JER_ITM_VOQ_VSQ_GRNTD_RJCT_STRICT_MASK);
    }
    soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, VSQ_LOSSLESS_RJCT_MASKf, JER_ITM_VOQ_VSQ_LOSSLESS_RJCT_MASK);
    soc_reg64_field32_set(unit, IQM_RJCT_GLBL_TESTS_MASKr, &reg64, ECN_RJCT_MASKf, JER_ITM_VOQ_ECN_CAPABLE_RJCT_MASK); 
    res = WRITE_IQM_RJCT_GLBL_TESTS_MASKr(unit, SOC_CORE_ALL, reg64);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    COMPILER_64_ZERO(reg64);
    res = READ_IQM_STE_RJCT_TESTS_MASKr(unit, SOC_CORE_ALL, &reg64);
    SOCDNX_IF_ERR_EXIT(res);
    
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VOQ_GRNTD_VQ_MX_SIZE_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VSQ_GRNTD_VQ_MX_SIZE_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VSQ_LOSSLESS_VQ_MX_SIZE_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, ECN_VQ_MX_SIZE_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VOQ_GRNTD_VQ_WRED_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VSQ_GRNTD_VQ_WRED_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, VSQ_LOSSLESS_VQ_WRED_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    soc_reg64_field32_set(unit, IQM_STE_RJCT_TESTS_MASKr, &reg64, ECN_VQ_WRED_RJCT_MASKf, JER_ITM_VSQ_RJCT_ALL_MASK);
    res = WRITE_IQM_STE_RJCT_TESTS_MASKr(unit, SOC_CORE_ALL, reg64);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    data = 0x0;
    mem =  SOC_IS_JERICHO_PLUS_A0(unit) ?  IPST_CRVSm :IPS_CRVSm;
    soc_mem_field32_set(unit, mem, &data, CR_VAL_BMPf, 0x0); 
    res = arad_fill_table_with_entry(unit, mem, MEM_BLOCK_ANY, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    data = 0;
    if (!SOC_IS_JERICHO_PLUS_A0(unit)) {

        res = READ_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, &data);
        SOCDNX_IF_ERR_EXIT(res);
        soc_reg_field_set(unit, IPS_CREDIT_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
        res = WRITE_IPS_CREDIT_CONFIGr(unit, SOC_CORE_ALL, data)  ; 
    }
    else
    {
        res =  READ_IPST_CREDIT_ARBITER_CONFIGr(unit, &data); 
        SOCDNX_IF_ERR_EXIT(res);
        soc_reg_field_set(unit, IPST_CREDIT_ARBITER_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
        res = WRITE_IPST_CREDIT_ARBITER_CONFIGr(unit, data) ; 
    }
    SOCDNX_IF_ERR_EXIT(res);
    
    
    
    res = jer_itm_fc_global_thresholds_init(unit);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    res = jer_itm_drop_global_thresholds_init(unit);
    SOCDNX_IF_ERR_EXIT(res);
    
    
    SOC_TMC_ITM_VSQ_FC_INFO_clear(&vsq_fc_info);
    SOC_TMC_ITM_VSQ_FC_INFO_clear(&exact_vsq_fc_info);
    vsq_fc_info.bd_size_fc.set = JER_ITM_VSQ_FC_BD_SIZE_MAX;
    vsq_fc_info.bd_size_fc.clear = JER_ITM_VSQ_FC_BD_SIZE_MAX;
    
    for (vsq_group = 0; vsq_group < SOC_TMC_NOF_VSQ_GROUPS; vsq_group++) {
        for (pool_id = 0; pool_id < ((vsq_group == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) ? SOC_TMC_ITM_NOF_RSRC_POOLS : 1); pool_id++) {
             res = jer_itm_vsq_fc_set(unit, vsq_group, 0  , pool_id, &vsq_fc_info, &exact_vsq_fc_info);
             SOCDNX_IF_ERR_EXIT(res);
        }
    }
    
    
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
        uint32 global_threshold_refernce_val;
        uint32 total_dram_buffers_for_reserved_resources_calculations;
        res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.get(unit, core_index, &guaranteed_q_resource);
        SOCDNX_IF_ERR_EXIT(res);
        
        SOCDNX_IF_ERR_EXIT(jer_itm_glob_rcs_drop_get(unit, core_index, &glbl_drop_info));
        global_threshold_refernce_val = glbl_drop_info.bds[3].set;
        if (core_index == 0)
        {
            guaranteed_q_resource.ocb.total = dbuffs_bdries->ocb_0.size;
            
            total_dram_buffers_for_reserved_resources_calculations = (dbuffs_bdries->mnmc_0.size < global_threshold_refernce_val) ? dbuffs_bdries->mnmc_0.size : global_threshold_refernce_val;
            guaranteed_q_resource.dram.total = total_dram_buffers_for_reserved_resources_calculations; 
        }
        else
        {
            
            guaranteed_q_resource.ocb.total = dbuffs_bdries->ocb_1.size;
            
            total_dram_buffers_for_reserved_resources_calculations = (dbuffs_bdries->mnmc_1.size < global_threshold_refernce_val) ? dbuffs_bdries->mnmc_1.size : global_threshold_refernce_val;
            guaranteed_q_resource.dram.total = total_dram_buffers_for_reserved_resources_calculations; 
        }
        
        res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.set(unit, core_index, &guaranteed_q_resource);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    res = soc_jer_itm_reserved_resource_init(unit);
    SOCDNX_IF_ERR_EXIT(res);

    
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index) {
        if (core_index == 0) { 
            ingress_congestion_mgmt.ocb.total = dbuffs_bdries->ocb_0.size;
            ingress_congestion_mgmt.dram.total = dbuffs_bdries->mnmc_0.size; 
        } else { 
            ingress_congestion_mgmt.ocb.total = dbuffs_bdries->ocb_1.size;
            ingress_congestion_mgmt.dram.total = dbuffs_bdries->mnmc_1.size; 
        }

        
        for (is_ocb_only = 0; is_ocb_only < 2; ++is_ocb_only) {
            resource = is_ocb_only ? (&ingress_congestion_mgmt.ocb) : (&ingress_congestion_mgmt.dram);
            resource->pool_0 = resource->total;
            resource->pool_1 = 0;
            resource->headroom = 0;
            resource->reserved = 0;
        }

        res = jer_itm_global_resource_allocation_set(unit, core_index, &ingress_congestion_mgmt);
        SOCDNX_IF_ERR_EXIT(res);
    }

    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    if (SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams == 0)
    {
        
        SHR_BITSET_RANGE(reg_above_64, 0, 512);
    }
    res = WRITE_IQM_VSQ_PG_IS_OCB_ONLYr(unit, SOC_CORE_ALL, reg_above_64);
    SOCDNX_IF_ERR_EXIT(res);

    res = jer_itm_setup_dp_map(unit);
    SOCDNX_IF_ERR_EXIT(res);

    if (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) {
        data = 1;
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_RESERVED_SPARE_2r(unit, SOC_CORE_ALL, data));
    }

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_BURST_SETTINGSr(unit, core_index, &data));
        soc_reg_field_set(unit, IQM_BURST_SETTINGSr, &data, BURST_SIZEf, (SOC_IS_JERICHO_B0(unit) || SOC_IS_QMX_B0(unit)) ? 11 : 6);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_BURST_SETTINGSr(unit, core_index, data));
    }

    
    COMPILER_64_ZERO(reg64);
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core_index) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_INTERNAL_FIFOS_THRESHOLDSr(unit, core_index, &reg64));
        soc_reg64_field32_set(unit, IQM_INTERNAL_FIFOS_THRESHOLDSr, &reg64, UPDT_FF_THf, 0x15);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_INTERNAL_FIFOS_THRESHOLDSr(unit, core_index, reg64));
    }

    
    sal_memset(table_data, 0x0, sizeof(table_data));
    for (index = 0; index < 8; index++) {
        
        if (SOC_TMC_ITM_PER_PACKET_COMPENSATION_ENABLED(unit)) {
            
            soc_mem_field32_set(unit, IQM_SCH_CCCPm, table_data, SCH_CC_CMP_BITMAPf, 0xf); 
        } else { 
            soc_mem_field32_set(unit, IQM_SCH_CCCPm, table_data, SCH_CC_CMP_BITMAPf, 0x0);
        }

        soc_mem_field32_set(unit, IQM_SCH_CCCPm, table_data, SCH_CC_CMP_PROFILEf, index); 

        SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_CCCPm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), index, table_data));
    }

    
    COMPILER_64_ZERO(data64);
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, &data64)); 
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_TRUNCATE_MODEf, 0x2);
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &data64, NWK_HDR_APPEND_MODEf, 0x2);
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, SOC_CORE_ALL, data64));


  
  
    data1 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[0] == 0) ? 0xD : 0xF;
    data2 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_pp_counter[1] == 0) ? 0xD : 0xF;
  
    data3 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[0] == 0) ? 0xD : 0xF;
    data4 = (SOC_DPP_CONFIG(unit)->jer->tm.truncate_delta_in_tm_counter[1] == 0) ? 0xD : 0xF;
    BCM_DPP_CORES_ITER(BCM_CORE_ALL, core_index)
    {
      SOCDNX_IF_ERR_EXIT(READ_IQM_IRPP_CNT_CMD_CONFIG_Ar(unit, core_index, &data));
      soc_reg_field_set(unit, IQM_IRPP_CNT_CMD_CONFIG_Ar, &data, IRPP_CNT_CMD_PKTSIZE_CMP_BITMAP_Af, data1);
      SOCDNX_IF_ERR_EXIT(WRITE_IQM_IRPP_CNT_CMD_CONFIG_Ar(unit, core_index, data));

      SOCDNX_IF_ERR_EXIT(READ_IQM_IRPP_CNT_CMD_CONFIG_Br(unit, core_index, &data));
      soc_reg_field_set(unit, IQM_IRPP_CNT_CMD_CONFIG_Br, &data, IRPP_CNT_CMD_PKTSIZE_CMP_BITMAP_Bf, data2);
      SOCDNX_IF_ERR_EXIT(WRITE_IQM_IRPP_CNT_CMD_CONFIG_Br(unit, core_index, data));

      SOCDNX_IF_ERR_EXIT(READ_IQM_IQM_CNT_CMD_CONFIG_Ar(unit, core_index, &data64));
      soc_reg64_field32_set(unit, IQM_IQM_CNT_CMD_CONFIG_Ar, &data64, IQM_CNT_CMD_PKTSIZE_CMP_BITMAP_Af, data3);
      SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_CNT_CMD_CONFIG_Ar(unit, core_index, data64));

      SOCDNX_IF_ERR_EXIT(READ_IQM_IQM_CNT_CMD_CONFIG_Br(unit, core_index, &data64));
      soc_reg64_field32_set(unit, IQM_IQM_CNT_CMD_CONFIG_Br, &data64, IQM_CNT_CMD_PKTSIZE_CMP_BITMAP_Bf, data4);
      SOCDNX_IF_ERR_EXIT(WRITE_IQM_IQM_CNT_CMD_CONFIG_Br(unit, core_index, data64));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_IDR_STATIC_CONFIGURATIONr(unit, &data));
    soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &data, HEADER_DELTA_FOR_CRPS_NEGATEf, TRUE);
    soc_reg_field_set(unit, IDR_STATIC_CONFIGURATIONr, &data, HEADER_TRUNCATE_SIZE_FOR_CRPS_NEGATEf, TRUE);
    SOCDNX_IF_ERR_EXIT(WRITE_IDR_STATIC_CONFIGURATIONr(unit, data));

    


    
    SOCDNX_IF_ERR_EXIT(READ_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, &data32));
    soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &data32, ING_0_PKTSIZE_CMP_BITMAPf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, data32));

    SOCDNX_IF_ERR_EXIT(READ_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, &data32));
    soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &data32, ING_1_PKTSIZE_CMP_BITMAPf, 0xf);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, data32));

    SOCDNX_IF_ERR_EXIT(READ_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, &data32));
    soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &data32, EGR_0_PKTSIZE_CMP_BITMAPf, 0xd);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, data32));

    SOCDNX_IF_ERR_EXIT(READ_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, &data32));
    soc_reg_field_set(unit, IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr, &data32, EGR_1_PKTSIZE_CMP_BITMAPf, 0xd);
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_RPT_PKTSIZE_COMPENSATION_SETTINGr(unit, data32));

    

exit: 
    SOCDNX_FUNC_RETURN; 
}


uint32
  jer_itm_enable_ecn_set(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled 
  )
{
  uint32 res, enable_bit = enabled ? 1 : 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit_semaphore, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, ECN_ENABLE_SYSTEMf,  enable_bit)); 
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit_semaphore, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, DSCP_AND_ECN_ENf,  enable_bit)); 

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_itm_enable_ecn_set()", unit, enabled);
}


uint32
  jer_itm_enable_ecn_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled 
  )
{
  uint32 res, val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(enabled);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit_semaphore, ARAD_REG_ACCESS_ERR, READ_EPNI_ECN_REMARKr(unit, REG_PORT_ANY, &val));
  *enabled = soc_reg_field_get(unit, EPNI_ECN_REMARKr, val, ECN_ENABLE_SYSTEMf);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_itm_enable_ecn_get()", unit, 0);
}

 
uint32 
  jer_itm_ingress_shape_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core, 
    SOC_SAND_OUT SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  ) 
{ 
  uint32 
    rate_internal, 
    fld_val, 
    res, 
    dsp_pp, 
    base_port_tc, 
    nof_prio,
    flags; 
  soc_port_t 
    port; 
  soc_pbmp_t 
    ports_bm; 
  int 
    tmp_core = SOC_CORE_ALL, 
    core_id = SOC_CORE_ALL;
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info);
  
  if (SOC_IS_QAX(unit)) { 
      SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(info);
      goto exit;
  }

  if (((core < 0) || (core > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && (core != SOC_CORE_ALL)) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
  if (core == SOC_CORE_ALL) {
      core_id = 0;
  } else {
      core_id = core;
  }
  if (core_id == 0) { 
       
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_LOW_0f, &info->q_range.q_num_low);  
      SOCDNX_IF_ERR_EXIT(res); 
 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_0f, &info->q_range.q_num_high); 
      SOCDNX_IF_ERR_EXIT(res); 
  } else if (core_id == 1) { 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_LOW_1f, &info->q_range.q_num_low);  
      SOCDNX_IF_ERR_EXIT(res); 
 
      res = soc_reg_above_64_field32_read(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_1f, &info->q_range.q_num_high); 
      SOCDNX_IF_ERR_EXIT(res); 
  } else { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_PORT_IDf, &base_port_tc);  
  SOCDNX_IF_ERR_EXIT(res); 
 
   
  res = soc_port_sw_db_valid_ports_core_get(unit, core_id, 0, &ports_bm);
  SOCDNX_IF_ERR_EXIT(res); 
 
  SOC_PBMP_ITER(ports_bm, port)  
  {
      SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
      if (!(SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags))) {

          res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &tmp_core); 
          SOCDNX_IF_ERR_EXIT(res); 
     
          res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, dsp_pp, &nof_prio); 
          SOCDNX_IF_ERR_EXIT(res); 
          if (core_id != tmp_core) {
              SOCDNX_IF_ERR_EXIT(SOC_E_CONFIG);
          }
          if (base_port_tc == nof_prio) { 
              info->sch_port = dsp_pp; 
          }
      }
  } 
   
  res = soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_ENABLEf, &fld_val); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  info->enable = SOC_SAND_NUM2BOOL(fld_val); 
 
  res = soc_reg_above_64_field32_read(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, core_id, 0, ISS_MAX_CR_RATEf, &rate_internal); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  res = arad_intern_clock2rate( 
          unit, 
          rate_internal, 
          TRUE,  
          &(info->rate) 
        ); 
  SOCDNX_SAND_IF_ERR_EXIT(res); 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
 
uint32 
  jer_itm_ingress_shape_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_INGRESS_SHAPE_INFO *info 
  )  
{ 
    uint32 
        fld_val, 
        q_hi, 
        q_low, 
        q_base, 
        ihp_reg_low, 
        ihp_reg_hi, 
        base_port_tc, 
        rate_internal, 
        res; 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info); 
    if ((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 
 
    res = soc_reg_above_64_field32_read(unit, IRR_FLOW_BASE_QUEUEr, core_id, 0, FLOW_BASE_QUEUE_Nf, &q_base); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    if (info->enable) { 
        q_low = info->q_range.q_num_low; 
        q_hi  = info->q_range.q_num_high; 
    } else { 
         
        q_low = q_base + 2; 
        q_hi = q_low - 1; 
    } 
     
    if (core_id == 0) { 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_LOW_0f, q_low);  
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_0r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_0f, q_hi); 
        SOCDNX_IF_ERR_EXIT(res); 
    } 
    if (core_id == 0) { 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_LOW_1f, q_low);  
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_ISP_1r, REG_PORT_ANY, 0, ISP_QNUM_HIGH_1f, q_hi); 
        SOCDNX_IF_ERR_EXIT(res);        
    } 
     
    ihp_reg_low = q_low - q_base; 
    ihp_reg_hi  = q_hi  - q_base; 
 
    res = arad_parser_ingress_shape_state_set( 
            unit, 
            info->enable, 
            ihp_reg_low, 
            ihp_reg_hi 
          ); 
    SOCDNX_SAND_IF_ERR_EXIT(res); 
  fld_val = info->enable ? 0x1 : 0x0;
    res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_ENABLEf, fld_val); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    if (info->enable) 
    { 
        res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_id, info->sch_port, &base_port_tc); 
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, core_id, 0, INGRESS_SHAPING_PORT_IDf, base_port_tc); 
        SOCDNX_IF_ERR_EXIT(res); 
 
        res = arad_intern_rate2clock(unit, info->rate, TRUE, &rate_internal); 
        SOCDNX_SAND_IF_ERR_EXIT(res); 
 
        res = soc_reg_above_64_field32_modify(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, core_id, 0, ISS_MAX_CR_RATEf, rate_internal); 
        SOCDNX_IF_ERR_EXIT(res); 
    } 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
STATIC uint32 
  _jer_itm_category_rngs_verify( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info);    
 
  if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) || (info->vsq_ctgry1_end > info->vsq_ctgry2_end) || (info->vsq_ctgry0_end > info->vsq_ctgry2_end)) { 
    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
   
  if ((info->vsq_ctgry0_end < 0) || (info->vsq_ctgry0_end > SOC_DPP_DEFS_GET(unit, nof_queues)-1)) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
   
  if ((info->vsq_ctgry1_end < 0) || (info->vsq_ctgry1_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1))) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
   
  if ((info->vsq_ctgry2_end < 0) || (info->vsq_ctgry2_end > (SOC_DPP_DEFS_GET(unit, nof_queues)-1))) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
 
 
int 
  jer_itm_category_rngs_set( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_IN  SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
  int  
      res; 
  uint64  
      reg_val; 
  SOCDNX_INIT_FUNC_DEFS; 
  SOCDNX_NULL_CHECK(info); 
  if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  } 
 
  res = _jer_itm_category_rngs_verify(unit, core_id, info); 
  SOCDNX_IF_ERR_EXIT(res); 
  res = READ_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, &reg_val); 
  SOCDNX_IF_ERR_EXIT(res); 
 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_0f, info->vsq_ctgry0_end); 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_1f, info->vsq_ctgry1_end); 
  soc_reg64_field32_set(unit, IQM_PACKET_QUEUES_CATEGORIESr, &reg_val, TOP_PKT_Q_CAT_2f, info->vsq_ctgry2_end); 
 
  res = WRITE_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, reg_val); 
  SOCDNX_IF_ERR_EXIT(res); 

exit: 
  SOCDNX_FUNC_RETURN; 
} 
 
 
 
int 
  jer_itm_category_rngs_get( 
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  int                 core_id, 
    SOC_SAND_OUT SOC_TMC_ITM_CATEGORY_RNGS *info 
  ) 
{ 
    int  
        res; 
    uint64  
        reg_val; 
    SOCDNX_INIT_FUNC_DEFS; 
    SOCDNX_NULL_CHECK(info); 
    if (((core_id < 0) || (core_id > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && core_id != SOC_CORE_ALL) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    } 
    res = READ_IQM_PACKET_QUEUES_CATEGORIESr(unit, core_id, &reg_val); 
    SOCDNX_IF_ERR_EXIT(res); 
 
    info->vsq_ctgry0_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_0f); 
    info->vsq_ctgry1_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_1f); 
    info->vsq_ctgry2_end = soc_reg64_field32_get(unit, IQM_PACKET_QUEUES_CATEGORIESr, reg_val, TOP_PKT_Q_CAT_2f); 

exit: 
  SOCDNX_FUNC_RETURN; 
} 

uint32
  jer_itm_admit_test_tmplt_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = jer_itm_admit_test_tmplt_verify(unit, core_id, admt_tst_ndx, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(info->test_a, &test_a_in_sand_u32);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  test_a_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTA_SET_0f + test_a_index,  test_a_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(info->test_b, &test_b_in_sand_u32);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  test_b_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTB_SET_0f + test_b_index,  test_b_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_set_unsafe()",0,0);
}


STATIC uint32
  jer_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (admt_tst_ndx > (ARAD_ITM_ADMIT_TSTS_LAST-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 1, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_verify()",0,0);
}


uint32
  jer_itm_admit_test_tmplt_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32              admt_tst_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE);
  if (core_id != SOC_CORE_ALL && (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 1, exit);
  }
  SOC_SAND_CHECK_NULL_INPUT(info);
  test_a_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTA_SET_0f + test_a_index, &test_a_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_itm_convert_u32_to_admit_one_test_tmplt(test_a_in_sand_u32, &(info->test_a));
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  test_b_index = admt_tst_ndx;
  res = soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSIONr, core_id, 0, RJCT_TMPLTB_SET_0f + test_b_index, &test_b_in_sand_u32);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_convert_u32_to_admit_one_test_tmplt(test_b_in_sand_u32, &(info->test_b));
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_get_unsafe()",0,0);
}


int jer_itm_ocb_fadt_enable_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int  enable
    )
{
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, &reg_val, MC_FADT_OCB_ENABLEf, enable);
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_ocb_fadt_enable_get(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT int*  enable
    )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(enable);
    SOCDNX_IF_ERR_EXIT(READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    *enable = soc_reg_field_get(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, reg_val, MC_FADT_OCB_ENABLEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_ocb_fadt_drop_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *info,
    SOC_SAND_OUT SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *exact_info
    )
{
    uint64 data;
    uint32 alpha_value, alpha_sign;
    int mantissa_bits = 8;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"),drop_precedence_ndx ));
    }

    SOCDNX_IF_ERR_EXIT(READ_IQM_MC_FADTTm (unit, MEM_BLOCK_ANY, 
                                           rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));
    
    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit,
                                                       IQM_MC_FADTTm, STATIC_THf,
                                                       mantissa_bits,
                                                       soc_mem_field_length(unit, IQM_MC_FADTTm, STATIC_THf) - mantissa_bits, 
                                                       1, 
                                                       1, 
                                                       &data,
                                                       info->free_min_threshold,
                                                       &exact_info->free_min_threshold));

    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit,
                                                       IQM_MC_FADTTm, DYNAMIC_MIN_THf,
                                                       mantissa_bits,
                                                       soc_mem_field_length(unit, IQM_MC_FADTTm, DYNAMIC_MIN_THf) - mantissa_bits, 
                                                       1, 
                                                       1, 
                                                       &data,
                                                       info->min_threshold,
                                                       &exact_info->min_threshold));

    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit,
                                                       IQM_MC_FADTTm, DYNAMIC_MAX_THf,
                                                       mantissa_bits,
                                                       soc_mem_field_length(unit, IQM_MC_FADTTm, DYNAMIC_MAX_THf) - mantissa_bits, 
                                                       1, 
                                                       1, 
                                                       &data,
                                                       info->max_threshold,
                                                       &exact_info->max_threshold));

    
    if (info->alpha < 0) {
        alpha_value = -info->alpha;
        alpha_sign = 1;
    }
    else {
        alpha_value = info->alpha;
        alpha_sign = 0;

    }
    exact_info->alpha = info->alpha;

    soc_mem_field32_set(unit, IQM_MC_FADTTm, &data, EXP_VALf,  alpha_value);
    soc_mem_field32_set(unit, IQM_MC_FADTTm, &data, EXP_SIGNf,  alpha_sign);

    SOCDNX_IF_ERR_EXIT(WRITE_IQM_MC_FADTTm(unit, MEM_BLOCK_ANY, 
                                     rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));

exit:
    SOCDNX_FUNC_RETURN;
} 

int jer_itm_ocb_fadt_drop_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_OCB_FADT_DROP_THRESHOLD  *info
    )
{
    uint64 data;
    uint32 alpha_value, alpha_sign;
    int mantissa_bits = 8;

    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(info);

    if (rt_cls_ndx >  ARAD_ITM_QT_RT_CLS_MAX) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("rate class index %d is out of range\n"), rt_cls_ndx));
    }

    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("drop precedence index %d is out of range\n"),drop_precedence_ndx ));
    }

    SOCDNX_IF_ERR_EXIT(READ_IQM_MC_FADTTm (unit, MEM_BLOCK_ANY, 
                                           rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));
    
    tmc_itm_mantissa_exp_field_get(unit,
                                   IQM_MC_FADTTm, STATIC_THf,
                                   mantissa_bits,
                                   soc_mem_field_length(unit, IQM_MC_FADTTm, STATIC_THf) - mantissa_bits, 
                                   1, 
                                   &data,
                                   &info->free_min_threshold);
    
    tmc_itm_mantissa_exp_field_get(unit,
                                   IQM_MC_FADTTm, DYNAMIC_MIN_THf,
                                   mantissa_bits,
                                   soc_mem_field_length(unit, IQM_MC_FADTTm, DYNAMIC_MIN_THf) - mantissa_bits, 
                                   1, 
                                   &data,
                                   &info->min_threshold);
    
    tmc_itm_mantissa_exp_field_get(unit,
                                   IQM_MC_FADTTm, DYNAMIC_MAX_THf,
                                   mantissa_bits,
                                   soc_mem_field_length(unit, IQM_MC_FADTTm, DYNAMIC_MAX_THf) - mantissa_bits, 
                                   1, 
                                   &data,
                                   &info->max_threshold);
    
    alpha_value = soc_mem_field32_get(unit, IQM_MC_FADTTm, &data, EXP_VALf);
    alpha_sign = soc_mem_field32_get(unit, IQM_MC_FADTTm, &data, EXP_SIGNf);

    info->alpha = (alpha_sign ? -alpha_value : alpha_value);
    
exit:
    SOCDNX_FUNC_RETURN;
} 


int
  jer_itm_vsq_wred_gen_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    uint32 
        data;
    const soc_mem_t mem_arr_IQM_VSQ_PQWQ[SOC_TMC_NOF_VSQ_GROUPS] = 
        {IQM_VSQA_PQWQm, IQM_VSQB_PQWQm, IQM_VSQC_PQWQm, IQM_VSQD_PQWQm, IQM_VSQE_PQWQm, IQM_VSQF_PQWQm};
    int  
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }   
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (info->exp_wq > ((0x1 << soc_mem_field_length(unit, IQM_VSQA_PQWQm, VQ_WEIGHTf)) - 1)) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        if (pool_id == 0) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_0f, info->exp_wq);
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_0_ENf, info->wred_en);
        } else if (pool_id == 1) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_1f, info->exp_wq);
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_1_ENf, info->wred_en);
        }
    } else {
        soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHTf, info->exp_wq);
        soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_ENf, info->wred_en);
    }
    res = soc_mem_write(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_wred_gen_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP          vsq_group_ndx,
    SOC_SAND_IN  uint32                         vsq_rt_cls_ndx,
    SOC_SAND_IN  int                            pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_WRED_GEN_INFO  *info
  )
{
    uint32 
        data;
    const soc_mem_t mem_arr_IQM_VSQ_PQWQ[SOC_TMC_NOF_VSQ_GROUPS] = 
        {IQM_VSQA_PQWQm, IQM_VSQB_PQWQm, IQM_VSQC_PQWQm, IQM_VSQD_PQWQm, IQM_VSQE_PQWQm, IQM_VSQF_PQWQm};
    int  
        res = SOC_E_NONE;
    SOCDNX_INIT_FUNC_DEFS;
    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        if (pool_id == 0) {
            info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_0f);
            info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_0_ENf);
        } else if (pool_id == 1) {
            info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHT_POOL_1f);
            info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_POOL_1_ENf);
        }
    } else {
        info->exp_wq = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_WEIGHTf);
        info->wred_en = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PQWQ[vsq_group_ndx], &data, VQ_AVRG_SIZE_ENf);
    }
exit: 
    SOCDNX_FUNC_RETURN; 
}

int
    jer_itm_vsq_wred_get(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *info) 
{
    int  
        res = SOC_E_NONE;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
   
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    res = soc_mem_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    tbl_data.c3 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f);
    tbl_data.c2 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_2f);
    tbl_data.c1 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f);
    tbl_data.vq_wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf);   
    tbl_data.max_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf);
    tbl_data.min_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf);
    info->wred_en = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_ENf));

    res = jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(unit, &tbl_data, info);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
    jer_itm_vsq_wred_set(
        SOC_SAND_IN  int                    unit,
        SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
        SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
        SOC_SAND_IN  uint32                 drop_precedence_ndx,
        SOC_SAND_IN  int                    pool_id,
        SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO *info,
        SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO *exact_info)
{
    int  
        res = SOC_E_NONE;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  tbl_data;
    SOCDNX_INIT_FUNC_DEFS;

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 

    tbl_data.c3 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f);
    tbl_data.c1 = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f);
    tbl_data.vq_wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf);   
    tbl_data.max_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf);
    tbl_data.min_avrg_th = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf);

    res = jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(unit, info, &tbl_data);
    SOCDNX_IF_ERR_EXIT(res);

    res = jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(unit, &tbl_data, exact_info);
    SOCDNX_IF_ERR_EXIT(res);
    exact_info->wred_en = SOC_SAND_BOOL2NUM(info->wred_en);

    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_3f, tbl_data.c3);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_2f, tbl_data.c2);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_C_1f, tbl_data.c1);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_PCKT_SZ_IGNRf, tbl_data.vq_wred_pckt_sz_ignr);   
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MAX_THf, tbl_data.max_avrg_th);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_AVRG_MIN_THf, tbl_data.min_avrg_th);
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, VQ_WRED_ENf, SOC_SAND_NUM2BOOL(info->wred_en));

    res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res); 
exit: 
    SOCDNX_FUNC_RETURN; 
}

STATIC int
  jer_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN int                                                     unit,
    SOC_SAND_IN SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param,
    SOC_SAND_INOUT SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_c2 = {{0}};
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wred_param); 
    SOCDNX_NULL_CHECK(tbl_data); 
    trunced = FALSE;

    
    tbl_data->min_avrg_th = 0;
    max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,SOC_TMC_ITM_WRED_GRANULARITY);
    res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
          SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
          FALSE,
          &(tbl_data->min_avrg_th),
          &min_avrg_th_exact_wred_granular
        );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    
    
    tbl_data->max_avrg_th = 0;
    res = arad_itm_man_exp_buffer_set(
            SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,SOC_TMC_ITM_WRED_GRANULARITY),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            &(tbl_data->max_avrg_th),
            &max_avrg_th_exact_wred_granular
            );
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    
    calc = wred_param->max_packet_size;
    if (calc > SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC) {
        calc = SOC_TMC_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
    } 
    calc = SOC_SAND_DIV_ROUND_UP(calc, SOC_TMC_ITM_WRED_GRANULARITY);
    tbl_data->c3 = soc_sand_log2_round_up(calc);

    
    tbl_data->vq_wred_pckt_sz_ignr = wred_param->ignore_packet_size;
    
    max_prob = (wred_param->max_probability);
    if(max_prob>=100) {
        max_prob = 99;
    }
    
    calc = SOC_TMC_WRED_NORMALIZE_FACTOR * max_prob;
    
    max_val_c1 = 31; 
    avrg_th_diff_wred_granular = (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);
    if(avrg_th_diff_wred_granular == 0) {
        tbl_data->c1 = max_val_c1;
    } else {
        calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
        tbl_data->c1 = soc_sand_log2_round_down(calc);
    } 
    if(tbl_data->c1 < max_val_c1) {
        
        uint32
            now     = 1 <<(tbl_data->c1),
            changed = 1 <<(tbl_data->c1+1),
            diff_with_now,
            diff_with_change;
        diff_with_change = changed-calc;

        diff_with_now    = calc-now;
        if( diff_with_change < diff_with_now) {
            tbl_data->c1 += 1;
        }
    }
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
    if (max_avrg_th_16_byte > 0) {
        max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
        max_val_c1 = soc_sand_log2_round_down(max_val_c1);
        SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
    }

    

    soc_sand_u64_multiply_longs(min_avrg_th_exact_wred_granular, (1 << tbl_data->c1),&u64_c2);

    trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->c2);
    if (trunced) {
        tbl_data->c2 = 0xFFFFFFFF;
    }
exit: 
    SOCDNX_FUNC_RETURN; 
}

STATIC int
  jer_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
     SOC_SAND_IN int                                                     unit,
     SOC_SAND_IN  SOC_TMC_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
     SOC_SAND_OUT SOC_TMC_ITM_WRED_QT_DP_INFO                            *wred_param
  )
{
    uint32
      res;
    SOC_SAND_U64
      u64_1,
      u64_2;
    uint32 remainder;
    uint32 avrg_th_diff, two_power_c1;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(wred_param); 
    SOCDNX_NULL_CHECK(tbl_data); 

    res = arad_itm_man_exp_buffer_get(
            tbl_data->min_avrg_th,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            (int32*)&(wred_param->min_avrg_th)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    wred_param->min_avrg_th *= SOC_TMC_ITM_WRED_GRANULARITY;
    
    res = arad_itm_man_exp_buffer_get(
            tbl_data->max_avrg_th,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB(unit),
            SOC_TMC_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS(unit),
            FALSE,
            (int32*)&(wred_param->max_avrg_th)
          );
    SOCDNX_SAND_IF_ERR_EXIT(res);
    wred_param->max_avrg_th *= SOC_TMC_ITM_WRED_GRANULARITY;
    
    wred_param->max_packet_size = (0x1<<(tbl_data->c3))*SOC_TMC_ITM_WRED_GRANULARITY;
    
    wred_param->ignore_packet_size = SOC_SAND_NUM2BOOL(tbl_data->vq_wred_pckt_sz_ignr);

    avrg_th_diff = 
        (wred_param->max_avrg_th - wred_param->min_avrg_th) / SOC_TMC_ITM_WRED_GRANULARITY;
    
    two_power_c1 = 1<<tbl_data->c1;
    
    
    soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff, &u64_1);
    remainder = soc_sand_u64_devide_u64_long(&u64_1, SOC_TMC_WRED_NORMALIZE_FACTOR, &u64_2);
    soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);
    
    if(remainder > (SOC_TMC_WRED_NORMALIZE_FACTOR/2))
    {
        wred_param->max_probability++;
    }
    
    if(wred_param->max_probability > 100) {
      wred_param->max_probability = 100;
    }

exit: 
    SOCDNX_FUNC_RETURN; 
}


int
  jer_itm_vsq_tail_drop_set(
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
    int  
        res = SOC_E_NONE;
    uint32
        exp_man,
        entry_offset,
        array_index,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    soc_field_t field; 
    uint32
        max_size_mnt_nof_bits,
        max_size_exp_nof_bits,
        max_size_mnt,
        max_size_exp;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    res = jer_itm_vsq_tail_drop_default_get(unit, exact_info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    if (vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        if (is_headroom) {
            field = VQ_MAX_BUFF_HEADROOM_SIZEf;
        } else {
            field = VQ_MAX_BUFF_SHARED_SIZEf;
        }
    } else {
        field = VQ_MAX_QUE_BUFF_SIZEf;
    }
    max_size_mnt_nof_bits = 8;
    max_size_exp_nof_bits = soc_mem_field_length(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], field) - max_size_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(info->max_inst_q_size_bds, max_size_mnt_nof_bits, max_size_exp_nof_bits, 0, &max_size_mnt, &max_size_exp);
    SOCDNX_SAND_IF_ERR_EXIT(res);
    exact_info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));

    arad_iqm_mantissa_exponent_set(unit, max_size_mnt, max_size_exp, max_size_mnt_nof_bits, &exp_man);   
    soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field, exp_man); 

   
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        
        if (info->alpha >= 0) {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf, info->alpha);    
        } else {
            soc_mem_field32_set(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf,  (1 << 4) | (0x7 % (0 - info->alpha)));
        }
    }
    res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_tail_drop_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    uint32 max_size_exp = 0, max_size_mnt = 0;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    arad_iqm_mantissa_exponent_get(unit, (0x1 << soc_mem_field_length(unit, IQM_VSQA_PRMm, VQ_MAX_QUE_BUFF_SIZEf)) - 1, 8, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size = info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));
    info->alpha = (0x1 << soc_mem_field_length(unit, IQM_VSQE_PRMm, FADT_DROP_EXPf)) - 1;

exit: 
  SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_tail_drop_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP  vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  int                    pool_id,
    SOC_SAND_IN  int                    is_headroom,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    int  
        res = SOC_E_NONE;
    uint32
        exp_man;
    uint32
      entry_offset,
      array_index,
      data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    const soc_mem_t mem_arr_IQM_VSQ_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_PRMm, IQM_VSQB_PRMm, IQM_VSQC_PRMm, IQM_VSQD_PRMm, IQM_VSQE_PRMm, IQM_VSQF_PRMm};
    soc_field_t field;
    uint32 max_size_exp = 0;
    uint32 max_size_mnt = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if (vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    } 
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        array_index = pool_id;
    } else {
        array_index = 0;
    }
    res = jer_itm_vsq_tail_drop_default_get(unit, info);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
    res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    if (vsq_group_ndx >= SOC_TMC_NOF_NON_SRC_BASED_VSQ_GROUPS) {
        if (is_headroom) {
            field = VQ_MAX_BUFF_HEADROOM_SIZEf;
        } else {
            field = VQ_MAX_BUFF_SHARED_SIZEf;
        }
    } else {
        field = VQ_MAX_QUE_BUFF_SIZEf;
    }
    exp_man = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, field);
    arad_iqm_mantissa_exponent_get(unit, exp_man, 8, &max_size_mnt, &max_size_exp);
    info->max_inst_q_size_bds = (max_size_mnt * (1 << max_size_exp));

    if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_PG) {
        info->alpha = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_PRM[vsq_group_ndx], data, FADT_DROP_EXPf);
    }

    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

int
  jer_itm_vsq_fc_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_FC_INFO    *info,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *exact_info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    vsq_bds_th_clear_and_set_exp_nof_bits,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp,
    mnt_exp,
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE],
    array_index = 0;
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm}; 
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);
  
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
      array_index = pool_id;
  } else {
      array_index = 0;
  }
  if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (info->bd_size_fc.clear > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (info->bd_size_fc.set > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  sal_memset(data, 0x0, sizeof(data));
  res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

  vsq_bds_th_clear_and_set_mnt_nof_bits = 8;
  vsq_bds_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], SET_THRESHOLD_BDSf) - vsq_bds_th_clear_and_set_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.clear,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_bd_mnt),
          &(clear_threshold_bd_exp)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res); 

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.set,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_bd_mnt),
          &(set_threshold_bd_exp)
        );
  SOCDNX_SAND_IF_ERR_EXIT(res); 

  exact_info->bd_size_fc.clear = ((clear_threshold_bd_mnt) * (1 << (clear_threshold_bd_exp)));
  if (exact_info->bd_size_fc.clear > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      exact_info->bd_size_fc.clear = JER_ITM_VSQ_FC_BD_SIZE_MAX;
  }
  exact_info->bd_size_fc.set = ((set_threshold_bd_mnt) * (1 << (set_threshold_bd_exp)));
  if (exact_info->bd_size_fc.set > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      exact_info->bd_size_fc.set = JER_ITM_VSQ_FC_BD_SIZE_MAX;
  }

  arad_iqm_mantissa_exponent_set(unit, set_threshold_bd_mnt, set_threshold_bd_exp, vsq_bds_th_clear_and_set_mnt_nof_bits, &mnt_exp);
  soc_mem_field32_set(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, SET_THRESHOLD_BDSf, mnt_exp);

  arad_iqm_mantissa_exponent_set(unit, clear_threshold_bd_mnt, clear_threshold_bd_exp, vsq_bds_th_clear_and_set_mnt_nof_bits, &mnt_exp);
  soc_mem_field32_set(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, CLR_THRESHOLD_BDSf, mnt_exp);

  res = soc_mem_array_write(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res); 

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  jer_itm_vsq_fc_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP      vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  int                        pool_id,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO    *info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp,
    mnt_exp,
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE],
    array_index = 0;
  const soc_mem_t mem_arr_IQM_VSQ_FC_PRM[SOC_TMC_NOF_VSQ_GROUPS] = {IQM_VSQA_FC_PRMm, IQM_VSQB_FC_PRMm, IQM_VSQC_FC_PRMm, IQM_VSQD_FC_PRMm, IQM_VSQE_FC_PRMm, IQM_VSQF_FC_PRMm};
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);
  if (vsq_group_ndx < 0 || vsq_group_ndx >= SOC_TMC_NOF_VSQ_GROUPS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
  }
  if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  if (vsq_group_ndx == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
      array_index = pool_id;
  } else {
      array_index = 0;
  }
  if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  sal_memset(data, 0x0, sizeof(data));
  res = soc_mem_array_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], array_index, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  res = soc_mem_read(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, data);
  SOCDNX_IF_ERR_EXIT(res);

  vsq_bds_th_clear_and_set_mnt_nof_bits = 8;

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, SET_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, vsq_bds_th_clear_and_set_mnt_nof_bits, &(set_threshold_bd_mnt), &(set_threshold_bd_exp));

  mnt_exp = soc_mem_field32_get(unit, mem_arr_IQM_VSQ_FC_PRM[vsq_group_ndx], data, CLR_THRESHOLD_BDSf);
  arad_iqm_mantissa_exponent_get(unit, mnt_exp, vsq_bds_th_clear_and_set_mnt_nof_bits, &(clear_threshold_bd_mnt), &(clear_threshold_bd_exp));

  info->bd_size_fc.clear = ((clear_threshold_bd_mnt) * (1 << (clear_threshold_bd_exp)));
  if (info->bd_size_fc.clear > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      info->bd_size_fc.clear = JER_ITM_VSQ_FC_BD_SIZE_MAX;
  }
  info->bd_size_fc.set = ((set_threshold_bd_mnt) * (1 << (set_threshold_bd_exp)));
  if (info->bd_size_fc.set > JER_ITM_VSQ_FC_BD_SIZE_MAX) {
      info->bd_size_fc.set = JER_ITM_VSQ_FC_BD_SIZE_MAX;
  }
  info->q_size_fc.clear = 0;
  info->q_size_fc.set = 0;

exit: 
    SOCDNX_FUNC_RETURN;
}


int
  jer_itm_vsq_fc_default_get(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT SOC_TMC_ITM_VSQ_FC_INFO  *info
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);

    
    info->bd_size_fc.set = JER_ITM_VSQ_FC_BD_SIZE_MAX;
    info->bd_size_fc.clear = JER_ITM_VSQ_FC_BD_SIZE_MAX;

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
    jer_itm_vsq_src_port_set(
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_IN uint32 src_pp_port,
       SOC_SAND_IN uint8  enable
       )
{
    uint32
        res,
        data,
        field_val;
    soc_port_t logical_port;
    uint32 
        src_link = -1, 
        phy_port = -1;
    soc_port_if_t interface_type;
    uint32 ilkn_id = -1;

    int fc_enable = enable;

    SOCDNX_INIT_FUNC_DEFS;

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_DPP_DEFS_GET(unit, nof_vsq_e)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (enable) {
        if (src_pp_port > SOC_MAX_NUM_PORTS) {
            LOG_ERROR(BSL_LS_SOC_COSQ, (BSL_META_U(unit, "Invalid src_pp_port %d\n"), src_pp_port));
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }

        res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, src_pp_port, &logical_port);
        SOCDNX_IF_ERR_EXIT(res);

        res = soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type);
        SOCDNX_IF_ERR_EXIT(res);

        if (SOC_PORT_IF_ILKN == interface_type) {
            
            res = soc_port_sw_db_protocol_offset_get(unit, logical_port, 0, &ilkn_id);
            SOCDNX_IF_ERR_EXIT(res);
            res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_nif_port_get, (unit, ilkn_id, &phy_port));
            SOCDNX_IF_ERR_EXIT(res);
        } else {
            if (JER_ITM_NON_PHY_IF_FOR_VSQ_E_F(interface_type)) {
                
                fc_enable = FALSE;
            } else {
                res = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
                SOCDNX_IF_ERR_EXIT(res);
            }
        }

        src_link = phy_port ? phy_port - 1 : phy_port;
    }

    
    res = READ_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    field_val = fc_enable ? src_link : ((1 << soc_mem_field_length(unit, IQM_VSQE_MAPm, VSQE_MAP_DATAf)) - 1);
    soc_mem_field32_set(unit, IQM_VSQE_MAPm, &data, VSQE_MAP_DATAf, field_val);
    res = WRITE_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

int
    jer_itm_vsq_src_port_get(
       SOC_SAND_IN int    unit,
       SOC_SAND_IN int    core_id,
       SOC_SAND_IN int    src_port_vsq_index,
       SOC_SAND_OUT uint32 *src_pp_port,
       SOC_SAND_OUT uint8  *enable
       )
{
    int res;
    uint32
        data,
        field_val,
        phy_port,
        tmp_src_pp_port;
    soc_port_t 
        local_port;
    soc_pbmp_t 
        phy_ports;
    uint8 
        found = 0;
    soc_port_if_t interface_type;
    uint32 ilkn_id = -1;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(enable);
    SOCDNX_NULL_CHECK(src_pp_port);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_DPP_DEFS_GET(unit, nof_vsq_e)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
    res = READ_IQM_VSQE_MAPm(unit, IQM_BLOCK(unit, core_id), src_port_vsq_index, &data);
    SOCDNX_IF_ERR_EXIT(res);

    field_val = soc_mem_field32_get(unit, IQM_VSQE_MAPm, &data, VSQE_MAP_DATAf);
    *enable = (field_val != ((1 << soc_mem_field_length(unit, IQM_VSQE_MAPm, VSQE_MAP_DATAf)) - 1)) ? TRUE : FALSE;
    if (*enable) {
        for (tmp_src_pp_port = 0; tmp_src_pp_port < SOC_MAX_NUM_PORTS; tmp_src_pp_port++) {
            local_port = core_info[unit][core_id].pp_to_local_port_map[tmp_src_pp_port];
            if(local_port == SOC_MAX_NUM_PORTS) {
                continue;
            }

            res = soc_port_sw_db_interface_type_get(unit, local_port, &interface_type);
            SOCDNX_IF_ERR_EXIT(res);

            if (SOC_PORT_IF_ILKN == interface_type) {
                
                res = soc_port_sw_db_protocol_offset_get(unit, local_port, 0, &ilkn_id);
                SOCDNX_IF_ERR_EXIT(res);
                res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_nif_port_get, (unit, ilkn_id, &phy_port));
                SOCDNX_IF_ERR_EXIT(res);
                if (field_val == phy_port - 1) {
                    found = 1;
                    break;
                }
            } else {
                res = soc_port_sw_db_phy_ports_get(unit, local_port, &phy_ports);
                SOCDNX_IF_ERR_EXIT(res);

                SOC_PBMP_ITER(phy_ports, phy_port) {
                    if (field_val == phy_port - 1) {
                        found = 1;
                        break;
                    }
                }
            }
            if (found) {
                break;
            }
        }
        *src_pp_port = tmp_src_pp_port;
    } else {
        *src_pp_port = -1;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}


uint32
jer_itm_vsq_pg_mapping_set(SOC_SAND_IN int    unit,
                           SOC_SAND_IN int    core_id,
                           SOC_SAND_IN uint32 pg_vsq_base,
                           SOC_SAND_IN int    cosq,
                           SOC_SAND_IN int    src_pp_port,
                           SOC_SAND_IN uint8  enable
                           )
{
    uint32 
        pfc_cfc_index = 0,
        res,
        data;
    uint32 
        pg_vsq_id;
    soc_port_t 
        logical_port;
    uint32 
        src_link = -1,
        phy_port = -1,
        ilkn_id  = -1;
    soc_port_if_t interface_type;
    uint8 pg_enable = enable;

    SOCDNX_INIT_FUNC_DEFS;
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

    
    res = READ_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    
    if (enable) {
        res = soc_port_sw_db_pp_to_local_port_get(unit, core_id, src_pp_port, &logical_port);
        SOCDNX_IF_ERR_EXIT(res);

        res = soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type);
        SOCDNX_IF_ERR_EXIT(res);
		
        if (JER_ITM_NON_PHY_IF_FOR_VSQ_E_F(interface_type)) {
            
            pg_enable = FALSE;            
        } else {
            res = soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy_port);
            SOCDNX_IF_ERR_EXIT(res);
            if (SOC_PORT_IF_ILKN == interface_type) {
                res = soc_port_sw_db_protocol_offset_get(unit, logical_port, 0, &ilkn_id);
                SOCDNX_IF_ERR_EXIT(res);
                     
                if (SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_id)) {
                    
                    pg_enable = FALSE;
                }             
            }
        }
        
        res = MBCM_DPP_DRIVER_CALL(unit,
                mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &phy_port));
        SOCDNX_IF_ERR_EXIT(res);
        src_link = phy_port ? phy_port - 1 : phy_port;
    }

    pfc_cfc_index = pg_enable ? (src_link * SOC_TMC_NOF_TRAFFIC_CLASSES) + cosq : 
            ((1 << soc_mem_field_length(unit, IQM_VSQF_MAPm, VSQF_MAP_DATAf)) - 1);
    soc_mem_field32_set(unit, IQM_VSQF_MAPm, &data, VSQF_MAP_DATAf, pfc_cfc_index);
    
    res = WRITE_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_vsq_pg_mapping_get(SOC_SAND_IN int     unit,
                           SOC_SAND_IN int     core_id,
                           SOC_SAND_IN uint32  pg_vsq_base,
                           SOC_SAND_IN int     cosq,
                           SOC_SAND_OUT uint32 *src_pp_port,
                           SOC_SAND_OUT uint8  *enable
                           )
{
    int res;
    uint32 
        pg_vsq_id;
    uint32 
        pfc_cfc_index,
        data;
    soc_port_t 
        logical_port;
    uint32 
        src_link = -1,
        phy_port = -1;
    int 
        core_get;
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

    
    res = READ_IQM_VSQF_MAPm(unit, IQM_BLOCK(unit, core_id), pg_vsq_id, &data);
    SOCDNX_IF_ERR_EXIT(res);

    pfc_cfc_index = soc_mem_field32_get(unit, IQM_VSQF_MAPm, &data, VSQF_MAP_DATAf);

    if (pfc_cfc_index == ((1 << soc_mem_field_length(unit, IQM_VSQF_MAPm, VSQF_MAP_DATAf)) - 1)) {
        *enable = FALSE;
    } else {
        src_link = pfc_cfc_index / SOC_TMC_NOF_TRAFFIC_CLASSES;
        phy_port = src_link + 1;
        res = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_add, (unit, phy_port, &phy_port));
        SOCDNX_IF_ERR_EXIT(res);
        logical_port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        res = soc_port_sw_db_local_to_pp_port_get(unit, logical_port, src_pp_port, &core_get);
        SOCDNX_IF_ERR_EXIT(res);
        if (core_id != core_get) {
            LOG_ERROR(BSL_LS_SOC_COSQ, 
                      (BSL_META_U
                       (unit, "pfc_cfc_index %d, is not configured in the right core. Expected %d, but got %d\n"), pfc_cfc_index, core_id, core_get));
            *enable = FALSE;
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        *enable = TRUE;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_vsq_pg_tc_profile_mapping_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_IN int pg_tc_profile
   ) 
{
    int
        res;
    uint32 
        data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    soc_mem_field32_set(unit, IQM_IPPPMm, &data, PG_TC_BITMAP_INDEXf, pg_tc_profile);

    res = WRITE_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN;

}
int
jer_itm_vsq_pg_tc_profile_mapping_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32 src_pp_port,
    SOC_SAND_OUT int *pg_tc_profile
   ) 
{
    int
        res;
    uint32 
        data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    *pg_tc_profile = soc_mem_field32_get(unit, IQM_IPPPMm, &data, PG_TC_BITMAP_INDEXf);

exit: 
    SOCDNX_FUNC_RETURN;

}

int
jer_itm_vsq_pg_tc_profile_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core_id,
    SOC_SAND_IN int pg_tc_profile_id,
    SOC_SAND_IN uint32 pg_tc_bitmap
   ) 
{
    uint32 pg_tc_bitamp_data = 0;
    int
        res;
    SOCDNX_INIT_FUNC_DEFS;

    if (pg_tc_bitmap & ~((1 << soc_mem_field_length(unit, IQM_PG_TC_BITMAPm, PG_TC_BITMAPf)) - 1)) {
        LOG_ERROR(BSL_LS_SOC_COSQ, 
                  (BSL_META_U(unit, "PG TC mapping bitmap is invalid %d, maximum bit nust be %d\n"), 
                   pg_tc_bitmap, soc_mem_field_length(unit, IQM_PG_TC_BITMAPm, PG_TC_BITMAPf)));
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    soc_mem_field32_set(unit, IQM_PG_TC_BITMAPm, &pg_tc_bitamp_data, PG_TC_BITMAPf, pg_tc_bitmap);
    res = WRITE_IQM_PG_TC_BITMAPm(unit, IQM_BLOCK(unit, core_id), pg_tc_profile_id, &pg_tc_bitamp_data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_vsq_pg_tc_profile_get(
    SOC_SAND_IN int         unit,
    SOC_SAND_IN int         core_id,
    SOC_SAND_IN int         pg_tc_profile_id,
    SOC_SAND_OUT uint32     *pg_tc_bitmap
   ) 
{
    int
        res;
    uint32 pg_tc_bitamp_data = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(pg_tc_bitmap);

    res = READ_IQM_PG_TC_BITMAPm(unit, IQM_BLOCK(unit, core_id), pg_tc_profile_id, &pg_tc_bitamp_data);
    SOCDNX_IF_ERR_EXIT(res);
    *pg_tc_bitmap = soc_mem_field32_get(unit, IQM_PG_TC_BITMAPm, &pg_tc_bitamp_data, PG_TC_BITMAPf);

exit: 
    SOCDNX_FUNC_RETURN;
}

int
  jer_itm_vsq_pg_prm_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    int  
        res = SOC_E_NONE;
    uint32
        data = 0;  
    SOC_TMC_ITM_VSQ_PG_PRM exact_pg_prm;
    int mantissa_bits = 8;
    
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    res = READ_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, IS_LOSSLESSf, pg_prm->is_lossles);
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, SHARED_POOL_NUMf, pg_prm->pool_id);
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, USE_MIN_PORTf, pg_prm->use_min_port);

    
    soc_mem_field32_set(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_EXPf, ((pg_prm->fadt_fc.sign & 0x1) << 2) |  (pg_prm->fadt_fc.exp & 0x3));

    
    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit,
                                                      IQM_VSQ_PG_PRMm, FADT_FC_OFFSETf,
                                                      mantissa_bits,
                                                      soc_mem_field_length(unit, IQM_VSQ_PG_PRMm, FADT_FC_OFFSETf) - mantissa_bits, 
                                                      1, 
                                                      1, 
                                                      &data,
                                                      pg_prm->fadt_fc.offset,
                                                      &exact_pg_prm.fadt_fc.offset));

    
    SOCDNX_IF_ERR_EXIT(tmc_itm_mantissa_exp_field_set(unit,
                                                      IQM_VSQ_PG_PRMm, FADT_FC_FLOORf,
                                                      mantissa_bits,
                                                      soc_mem_field_length(unit, IQM_VSQ_PG_PRMm, FADT_FC_FLOORf) - mantissa_bits, 
                                                      1, 
                                                      1, 
                                                      &data,
                                                      pg_prm->fadt_fc.floor,
                                                      &exact_pg_prm.fadt_fc.floor));

    res = WRITE_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_pg_prm_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_VSQ_PG_PRM *pg_prm
  )
{
    int  
        res = SOC_E_NONE;
    uint32
        data = 0;
    uint32 
        fadt_fc_exp_field_val;
    int mantissa_bits = 8;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(pg_prm);

    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
   
    res = READ_IQM_VSQ_PG_PRMm(unit, IQM_BLOCK(unit, SOC_CORE_ALL), vsq_rt_cls_ndx, &data);
    SOCDNX_IF_ERR_EXIT(res);
    
    pg_prm->is_lossles = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, IS_LOSSLESSf);
    pg_prm->pool_id = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, SHARED_POOL_NUMf);
    pg_prm->use_min_port = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, USE_MIN_PORTf);

    fadt_fc_exp_field_val = soc_mem_field32_get(unit, IQM_VSQ_PG_PRMm, &data, FADT_FC_EXPf);
    pg_prm->fadt_fc.exp = (fadt_fc_exp_field_val & 0x3);
    pg_prm->fadt_fc.sign = ((fadt_fc_exp_field_val >> 2) & 0x1);


    tmc_itm_mantissa_exp_field_get(unit,
                                   IQM_VSQ_PG_PRMm, FADT_FC_OFFSETf, 
                                   mantissa_bits,
                                   soc_mem_field_length(unit, IQM_VSQ_PG_PRMm, FADT_FC_OFFSETf) - mantissa_bits, 
                                   1, 
                                   &data,
                                   &pg_prm->fadt_fc.offset);

    tmc_itm_mantissa_exp_field_get(unit,
                                   IQM_VSQ_PG_PRMm, FADT_FC_FLOORf, 
                                   mantissa_bits,
                                   soc_mem_field_length(unit, IQM_VSQ_PG_PRMm, FADT_FC_FLOORf) - mantissa_bits, 
                                   1, 
                                   &data,
                                   &pg_prm->fadt_fc.floor);

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_pg_fadt_fc_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int              fadt_enable
  )
{
    int is_simple_fc;

    SOCDNX_INIT_FUNC_DEFS;

    is_simple_fc = (fadt_enable ? FALSE : TRUE);

    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, VSQ_TH_MODE_SELf, is_simple_fc)); 

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_vsq_pg_fadt_fc_enable_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT int                *fadt_enable
  )
{
    uint32 data;
    int is_simple_fc;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IQM_IQM_ENABLERSr(unit, SOC_CORE_DEFAULT, &data));

    is_simple_fc = soc_reg_field_get(unit, IQM_IQM_ENABLERSr, data, VSQ_TH_MODE_SELf); 

    *fadt_enable = (is_simple_fc ? FALSE : TRUE);

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_ocb_range_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32              ocb_free_threshold
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( WRITE_IQM_FR_OCB_PRM_SEL_THr(unit, SOC_CORE_ALL, ocb_free_threshold)); 

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
  jer_itm_ocb_range_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT  uint32              *ocb_free_threshold
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT( READ_IQM_FR_OCB_PRM_SEL_THr(unit, SOC_CORE_DEFAULT, ocb_free_threshold)); 

exit: 
    SOCDNX_FUNC_RETURN; 
}



uint32 
  jer_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  )
{
  uint32
	  color_resolution_mode,
	  mtr_cmd_indx,
	  in_dp_indx,
	  eth_mtr_dp_indx,
	  eth_mtr_invalid_indx,
	  mtr_dp_indx,
	  mrps_dp,
	  mrps_drop,
	  resolved_dp,
	  ingress_dp,
	  egress_dp,
	  entry_indx,
	  tbl_data;
  uint8
	  divide_egress_dp = FALSE;

  SOCDNX_INIT_FUNC_DEFS;

  color_resolution_mode = SOC_DPP_CONFIG(unit)->meter.color_resolution_mode;

  

  entry_indx = 0;
  for (mtr_cmd_indx = 0; mtr_cmd_indx < SOC_PPC_NOF_MTR_RES_USES; ++mtr_cmd_indx)  {
    for (in_dp_indx = 0; in_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++in_dp_indx)  {
      for (eth_mtr_dp_indx = 0; eth_mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++eth_mtr_dp_indx) {
		  for (eth_mtr_invalid_indx = 0; eth_mtr_invalid_indx < 2; ++eth_mtr_invalid_indx) {

			  
			  if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable 
				  && !eth_mtr_invalid_indx && 3 == eth_mtr_dp_indx)
			  {
				  mrps_dp = 3;
				  mrps_drop = 1; 
			  }
			  else 
			  {
				  mrps_dp = in_dp_indx;
				  mrps_drop = 0;
			  }
			  if (SOC_IS_QAX(unit)) {
				  
				  SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_MRPS_IN_DP_MAPPINGm, MEM_BLOCK_ANY, entry_indx, &tbl_data));

				  soc_mem_field_set(unit, CGM_MRPS_IN_DP_MAPPINGm, &tbl_data, MRPS_CMD_DPf, &mrps_dp);
				  soc_mem_field_set(unit, CGM_MRPS_IN_DP_MAPPINGm, &tbl_data, MRPS_CMD_DROPf, &mrps_drop);

				  SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_MRPS_IN_DP_MAPPINGm, MEM_BLOCK_ANY, entry_indx, &tbl_data));
			  }else{
				  
				  SOCDNX_IF_ERR_EXIT(READ_IDR_MRPS_0_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit) , entry_indx, &tbl_data));

				  soc_mem_field_set(unit, IDR_MRPS_0_IN_DP_MAPPINGm, &tbl_data, MRPS_IN_DPf, &mrps_dp);
				  soc_mem_field_set(unit, IDR_MRPS_0_IN_DP_MAPPINGm, &tbl_data, MRPS_IN_DROPf, &mrps_drop);

				  SOCDNX_IF_ERR_EXIT(WRITE_IDR_MRPS_0_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));
				  SOCDNX_IF_ERR_EXIT(WRITE_IDR_MRPS_1_IN_DP_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));
			  }

			  ++entry_indx;
		  }
	  }
	}
  }

  if ( color_resolution_mode == SOC_PPC_MTR_COLOR_RESOLUTION_MODE_DEFAULT && SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
	  
	  divide_egress_dp = TRUE;
  }

  

  entry_indx = 0;
  for (mtr_cmd_indx = 0; mtr_cmd_indx < SOC_PPC_NOF_MTR_RES_USES; ++mtr_cmd_indx)  {
    for (in_dp_indx = 0; in_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++in_dp_indx)  {
      for (eth_mtr_dp_indx = 0; eth_mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++eth_mtr_dp_indx) {
		  for (mtr_dp_indx = 0; mtr_dp_indx < ARAD_NOF_DROP_PRECEDENCE; ++mtr_dp_indx) {
			  
			  if( !SOC_DPP_CONFIG(unit)->arad->init.pp_enable)
			  {
				  ingress_dp = egress_dp = in_dp_indx;  
			  }
			  else 
			  {
				  
				  resolved_dp = mtr_dp_indx;

				  if (color_resolution_mode == SOC_PPC_MTR_COLOR_RESOLUTION_MODE_RED_DIFF) {
					  if (mtr_dp_indx == 2) {
						  
						  resolved_dp = 1;
					  } 
					  else if (mtr_dp_indx == 3) {
						  
						  resolved_dp = 2;
					  }

					  
					  if (eth_mtr_dp_indx == 3) {
						  resolved_dp = 3;
					  }
				  }

				  ingress_dp = egress_dp = resolved_dp;

				  
				  if (2 == resolved_dp) {
					  if (1 == soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_result_dp_2_to_1", 0)){
						  ingress_dp = 1;
						  egress_dp = 1;
					  }
					  else if (1 == soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_ingress_result_dp_2_to_1", 0)){
						  ingress_dp = 1;
					  }
					  else if (1 == soc_property_suffix_num_get(unit, 0, spn_CUSTOM_FEATURE, "always_map_egress_result_dp_2_to_1", 0)){
						  egress_dp = 1;
					  }
				  }

				  
				  if (mtr_cmd_indx == SOC_PPC_MTR_RES_USE_NONE) {
					  
					  ingress_dp = egress_dp = in_dp_indx; 
				  } else if (mtr_cmd_indx == SOC_PPC_MTR_RES_USE_OW_DP) {
					  
					  egress_dp = in_dp_indx; 
				  } else if (mtr_cmd_indx == SOC_PPC_MTR_RES_USE_OW_DE) {
					  
					  ingress_dp = in_dp_indx;
				  } 
			  }

			  if (divide_egress_dp) {
				  egress_dp /= 2;
			  }

			  if (SOC_IS_QAX(unit)) {
				  SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, CGM_DP_MAPPINGm, MEM_BLOCK_ANY, entry_indx, &tbl_data));

				  soc_mem_field_set(unit, CGM_DP_MAPPINGm, &tbl_data, ING_DPf, &ingress_dp);
				  soc_mem_field_set(unit, CGM_DP_MAPPINGm, &tbl_data, EGR_DPf, &egress_dp);

				  SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, CGM_DP_MAPPINGm, MEM_BLOCK_ANY, entry_indx, &tbl_data));
			  }else{
				  SOCDNX_IF_ERR_EXIT(READ_IDR_DROP_PRECEDENCE_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));

				  soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, INGRESS_DROP_PRECEDENCEf, &ingress_dp);
				  soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, EGRESS_DROP_PRECEDENCEf, &egress_dp);

				  SOCDNX_IF_ERR_EXIT(WRITE_IDR_DROP_PRECEDENCE_MAPPINGm(unit, IDR_BLOCK(unit), entry_indx, &tbl_data));
			  }

			  ++entry_indx;
		  }
	  }
	}
  }

exit:
  SOCDNX_FUNC_RETURN;
}

int
jer_itm_src_vsqs_mapping_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int src_pp_port,
       SOC_SAND_IN int src_port_vsq_index,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN uint8 enable
       ) 
{
    int 
        cosq;
    int
        res;
    uint32 
        data;
    uint8
        numq;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (src_port_vsq_index < 0 || src_port_vsq_index >= SOC_DPP_DEFS_GET(unit, nof_vsq_e)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    if (src_pp_port < -1 || src_pp_port > SOC_MAX_NUM_PORTS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    
    res = jer_itm_vsq_src_port_set(unit, core_id, src_port_vsq_index, (uint32)src_pp_port, enable);
    SOCDNX_IF_ERR_EXIT(res);

    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    
    
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, NIF_PORTf, enable ? src_port_vsq_index : 0);

    
    
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, PG_BASEf, enable ? pg_base : 0);
    
    soc_mem_field32_set(unit, IQM_IPPPMm, &data, XGS_ST_VSQf, 0);

    res = WRITE_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    
     
    res = sw_state_access[unit].dpp.soc.arad.tm.tm_info.pg_numq.get(unit, core_id, pg_base, &numq);
    SOCDNX_IF_ERR_EXIT(res);

    for (cosq = 0; cosq < numq; cosq++) {
        res = jer_itm_vsq_pg_mapping_set(unit, core_id, pg_base, cosq, src_pp_port, enable);
        SOCDNX_IF_ERR_EXIT(res);
    }
exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_src_vsqs_mapping_get(
       SOC_SAND_IN  int unit,
       SOC_SAND_IN  int core_id,
       SOC_SAND_IN  int src_pp_port,
       SOC_SAND_OUT int *src_port_vsq_index,
       SOC_SAND_OUT int *pg_base,
       SOC_SAND_OUT uint8 *enable
       ) 
{
    int
        res;
    uint32 
        data;
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
    res = READ_IQM_IPPPMm(unit, IQM_BLOCK(unit, core_id), src_pp_port, &data);
    SOCDNX_IF_ERR_EXIT(res);

    
    *src_port_vsq_index = soc_mem_field32_get(unit, IQM_IPPPMm, &data, NIF_PORTf);
    
    *pg_base = soc_mem_field32_get(unit, IQM_IPPPMm, &data, PG_BASEf);
    
    *enable = 0;
    if (!(*src_port_vsq_index == 0 && *pg_base == 0)) {
        *enable = TRUE;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}
uint32
jer_itm_vsq_pg_ocb_set(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_IN uint8* ocb_only
       )
{
    int 
        cosq;
    uint32
        res;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_data;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_field_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(ocb_only); 

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (numq < 1 || numq > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    for (cosq = 0; cosq < numq; cosq++) {
        if (ocb_only[cosq] > 1) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    
    res = READ_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_reg_above_64_field_get(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
    for (cosq = 0; cosq < numq; cosq++) {
        if (ocb_only[cosq]) {
            SHR_BITSET(vsq_pg_is_ocb_field_val, pg_base + cosq);
        } else {
            SHR_BITCLR(vsq_pg_is_ocb_field_val, pg_base + cosq);
        }
    }
    soc_reg_above_64_field_set(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
    res = WRITE_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);

exit: 
    SOCDNX_FUNC_RETURN;
}

uint32
jer_itm_vsq_pg_ocb_get(
       SOC_SAND_IN int unit,
       SOC_SAND_IN int core_id,
       SOC_SAND_IN int pg_base,
       SOC_SAND_IN int numq,
       SOC_SAND_OUT uint8* ocb_only
       )
{
    int 
        cosq;
    uint32
        res;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_data;
    soc_reg_above_64_val_t 
        vsq_pg_is_ocb_field_val;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(ocb_only);

    if (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pg_base < 0 || pg_base >= SOC_DPP_DEFS_GET(unit, nof_vsq_f)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (numq < 1 || numq > SOC_TMC_NOF_TRAFFIC_CLASSES) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
    res = READ_IQM_VSQ_PG_IS_OCB_ONLYr(unit, core_id, vsq_pg_is_ocb_data);
    SOCDNX_IF_ERR_EXIT(res);

    for (cosq = 0; cosq < numq; cosq++) {
        soc_reg_above_64_field_get(unit, IQM_VSQ_PG_IS_OCB_ONLYr, vsq_pg_is_ocb_data, VSQ_PG_IS_OCB_ONLYf, vsq_pg_is_ocb_field_val);
        ocb_only[cosq] = SHR_BITGET(vsq_pg_is_ocb_field_val, (pg_base + cosq)) ? TRUE : FALSE;
    }
exit: 
    SOCDNX_FUNC_RETURN;
}
int
  jer_itm_glob_rcs_drop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_OUT  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info
  )
{
    uint8 is_ocb_only = FALSE;
    uint8 pool_id;
    int res;
    uint32 exp_man;
    uint32 bdbs_th_mnt_nof_bits;
    uint32 bds_th_mnt_nof_bits;
    uint32 mini_mc_mnt_nof_bits;
    uint32 fmc_th_mnt_nof_bits;
    uint32 mc_ocb_mnt_nof_bits;
    uint32 dyn_mnt_nof_bits;
    uint32 mnt_val;
    uint32 exp_val;
    uint32 drop_precedence_ndx;
    soc_reg_t  shared_pool_reg = 0;
    soc_field_t shared_pool_field_set = 0;
    soc_field_t shared_pool_field_clr = 0;
    soc_reg_above_64_val_t shared_pool_data;
    const soc_field_t rjct_set_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {    MIXED_RJCT_SET_FR_BDB_TH_0f, MIXED_RJCT_SET_FR_BDB_TH_1f, MIXED_RJCT_SET_FR_BDB_TH_2f, MIXED_RJCT_SET_FR_BDB_TH_3f,
                                                                                        OCB_RJCT_SET_FR_BDB_TH_0f,   OCB_RJCT_SET_FR_BDB_TH_1f,   OCB_RJCT_SET_FR_BDB_TH_2f,   OCB_RJCT_SET_FR_BDB_TH_3f};
    const soc_field_t rjct_clr_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {    MIXED_RJCT_CLR_FR_BDB_TH_0f, MIXED_RJCT_CLR_FR_BDB_TH_1f, MIXED_RJCT_CLR_FR_BDB_TH_2f, MIXED_RJCT_CLR_FR_BDB_TH_4f,
                                                                                        OCB_RJCT_CLR_FR_BDB_TH_0f,   OCB_RJCT_CLR_FR_BDB_TH_1f,   OCB_RJCT_CLR_FR_BDB_TH_2f,   OCB_RJCT_CLR_FR_BDB_TH_4f};
    const soc_field_t rjct_set_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {     MIXED_RJCT_SET_OC_BD_TH_0f, MIXED_RJCT_SET_OC_BD_TH_1f, MIXED_RJCT_SET_OC_BD_TH_2f, MIXED_RJCT_SET_OC_BD_TH_3f,
                                                                                        OCB_RJCT_SET_OC_BD_TH_0f,   OCB_RJCT_SET_OC_BD_TH_1f,   OCB_RJCT_SET_OC_BD_TH_2f,   OCB_RJCT_SET_OC_BD_TH_3f};
    const soc_field_t rjct_clr_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {     MIXED_RJCT_CLR_OC_BD_TH_0f, MIXED_RJCT_CLR_OC_BD_TH_1f, MIXED_RJCT_CLR_OC_BD_TH_2f, MIXED_RJCT_CLR_OC_BD_TH_3f,
                                                                                        OCB_RJCT_CLR_OC_BD_TH_0f, OCB_RJCT_CLR_OC_BD_TH_1f, OCB_RJCT_CLR_OC_BD_TH_2f, OCB_RJCT_CLR_OC_BD_TH_3f};
    const soc_field_t rjct_clr_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {     RJCT_CLR_FR_DB_MNMC_TH_0f, RJCT_CLR_FR_DB_MNMC_TH_1f, RJCT_CLR_FR_DB_MNMC_TH_2f, RJCT_CLR_FR_DB_MNMC_TH_3f};
    const soc_field_t rjct_set_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {     RJCT_SET_FR_DB_MNMC_TH_0f, RJCT_SET_FR_DB_MNMC_TH_1f, RJCT_SET_FR_DB_MNMC_TH_2f, RJCT_SET_FR_DB_MNMC_TH_3f};
    const soc_field_t rjct_clr_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {    RJCT_CLR_FR_DB_FLMC_TH_0f, RJCT_CLR_FR_DB_FLMC_TH_1f, RJCT_CLR_FR_DB_FLMC_TH_2f, RJCT_CLR_FR_DB_FLMC_TH_3f};
    const soc_field_t rjct_set_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {    RJCT_SET_FR_DB_FLMC_TH_0f, RJCT_SET_FR_DB_FLMC_TH_1f, RJCT_SET_FR_DB_FLMC_TH_2f, RJCT_SET_FR_DB_FLMC_TH_3f};
    const soc_field_t rjct_dyn_size_clr_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {  MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_0f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_1f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_2f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_3f,
                                                                                        OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_3f};
    const soc_field_t rjct_dyn_size_set_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {  MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_0f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_1f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_2f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_3f,
                                                                                        OCB_DYN_WORDS_SIZE_RJCT_SET_TH_0f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_1f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_2f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_3f};
    const soc_field_t rjct_set_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {        RJCT_SET_FR_OCB_TH_0f, RJCT_SET_FR_OCB_TH_1f, RJCT_SET_FR_OCB_TH_2f, RJCT_SET_FR_OCB_TH_3f};
    const soc_field_t rjct_clr_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {        RJCT_CLR_FR_OCB_TH_0f, RJCT_CLR_FR_OCB_TH_1f, RJCT_CLR_FR_OCB_TH_2f, RJCT_CLR_FR_OCB_TH_3f};
    SOC_TMC_THRESH_WITH_HYST_INFO *shrd_pool_th;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);

    if ((core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core_id != SOC_CORE_ALL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    shrd_pool_th = &info->ocb_shrd_pool[pool_id][drop_precedence_ndx];
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                } else {
                    shrd_pool_th = &info->mix_shrd_pool[pool_id][drop_precedence_ndx];
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    }
                }
                res = soc_reg_above_64_get(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);

                shrd_pool_th->set   = soc_reg_above_64_field32_get(unit, shared_pool_reg, shared_pool_data, shared_pool_field_set);
                shrd_pool_th->clear = soc_reg_above_64_field32_get(unit, shared_pool_reg, shared_pool_data, shared_pool_field_clr);
            }
        }
    }
  
  bdbs_th_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_clr_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->bdbs[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          } else {
              info->ocb_bdbs[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          }
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_set_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bdbs_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          } else {
              info->bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          }
      }
  }
  
  bds_th_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit,
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr,
                    core_id,
                    0,
                    rjct_clr_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bds_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          } else {
              info->bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
          }
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, 
                    core_id, 
                    0, 
                    rjct_set_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, bds_th_mnt_nof_bits, &mnt_val, &exp_val);
          if (is_ocb_only) {
              info->ocb_bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          } else {
              info->bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
          }
      }
  }
  
  mini_mc_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_clr_fr_bd_mmc_th_fields[drop_precedence_ndx],  
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->mini_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      res = soc_reg_above_64_field32_read(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_set_fr_bd_mmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->mini_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
  }
  
  fmc_th_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_reg_above_64_field32_read(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_clr_fr_bd_flmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->full_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_read(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_set_fr_bd_flmc_th_fields[drop_precedence_ndx],
                &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mini_mc_mnt_nof_bits, &mnt_val, &exp_val);
      info->full_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
  }

  
  mc_ocb_mnt_nof_bits = 8;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) 
  {
      res = soc_reg_above_64_field32_read(unit, IQM_REJECT_CONFIGURATION_OCBSr, core_id, 0, rjct_set_fr_ocb_th_fields[drop_precedence_ndx], &exp_man);
      SOCDNX_IF_ERR_EXIT(res);

      arad_iqm_mantissa_exponent_get(unit, exp_man, mc_ocb_mnt_nof_bits, &mnt_val, &exp_val);
      info->ocb_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      res = soc_reg_above_64_field32_read(unit, IQM_REJECT_CONFIGURATION_OCBSr, core_id, 0, rjct_clr_fr_ocb_th_fields[drop_precedence_ndx], &exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      arad_iqm_mantissa_exponent_get(unit, exp_man, mc_ocb_mnt_nof_bits, &mnt_val, &exp_val);
      info->ocb_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
  }

  
  dyn_mnt_nof_bits = 8;
  for (is_ocb_only = 0; is_ocb_only < 2; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_clr_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, dyn_mnt_nof_bits, &mnt_val, &exp_val);

          if (is_ocb_only) {
              info->ocb_mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          } else {
              info->mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
          }

          res = soc_reg_above_64_field32_read(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    &exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          arad_iqm_mantissa_exponent_get(unit, exp_man, dyn_mnt_nof_bits, &mnt_val, &exp_val);

          if (is_ocb_only) {
              info->ocb_mem_excess[drop_precedence_ndx].set = (mnt_val) * (1<<(exp_val));
          } else {
              info->mem_excess[drop_precedence_ndx].set = (mnt_val) * (1<<(exp_val));
          }
      }
  }

exit: 
    SOCDNX_FUNC_RETURN; 
}                       

int
  jer_itm_glob_rcs_drop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  SOC_TMC_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_TMC_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
    uint8                   is_ocb_only = FALSE;
    uint8                   pool_id;
    uint32                  drop_precedence_ndx;
    int                     res;
    uint32                  exp_man;
    uint32                  bdbs_th_mnt_nof_bits;
    uint32                  bdbs_th_exp_nof_bits;
    uint32                  bds_th_mnt_nof_bits;
    uint32                  bds_th_exp_nof_bits;
    uint32                  mini_mc_mnt_nof_bits;
    uint32                  mini_mc_exp_nof_bits;
    uint32                  fmc_th_mnt_nof_bits;
    uint32                  fmc_th_exp_nof_bits;
    uint32                  mc_ocb_mnt_nof_bits;
    uint32                  mc_ocb_exp_nof_bits;
    uint32                  dyn_mnt_nof_bits;
    uint32                  dyn_exp_nof_bits;
    uint32                  mnt_val;
    uint32                  exp_val;
    soc_reg_t               shared_pool_reg = 0;
    soc_field_t             shared_pool_field_set = 0;
    soc_field_t             shared_pool_field_clr = 0;
    soc_reg_above_64_val_t  shared_pool_data;

    const soc_field_t rjct_set_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {    MIXED_RJCT_SET_FR_BDB_TH_0f, MIXED_RJCT_SET_FR_BDB_TH_1f, MIXED_RJCT_SET_FR_BDB_TH_2f, MIXED_RJCT_SET_FR_BDB_TH_3f,
                                                                                        OCB_RJCT_SET_FR_BDB_TH_0f,   OCB_RJCT_SET_FR_BDB_TH_1f,   OCB_RJCT_SET_FR_BDB_TH_2f,   OCB_RJCT_SET_FR_BDB_TH_3f};
    const soc_field_t rjct_clr_fr_bdb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {    MIXED_RJCT_CLR_FR_BDB_TH_0f, MIXED_RJCT_CLR_FR_BDB_TH_1f, MIXED_RJCT_CLR_FR_BDB_TH_2f, MIXED_RJCT_CLR_FR_BDB_TH_4f,
                                                                                        OCB_RJCT_CLR_FR_BDB_TH_0f,   OCB_RJCT_CLR_FR_BDB_TH_1f,   OCB_RJCT_CLR_FR_BDB_TH_2f,   OCB_RJCT_CLR_FR_BDB_TH_4f};
    const soc_field_t rjct_set_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {     MIXED_RJCT_SET_OC_BD_TH_0f, MIXED_RJCT_SET_OC_BD_TH_1f, MIXED_RJCT_SET_OC_BD_TH_2f, MIXED_RJCT_SET_OC_BD_TH_3f,
                                                                                        OCB_RJCT_SET_OC_BD_TH_0f,    OCB_RJCT_SET_OC_BD_TH_1f,   OCB_RJCT_SET_OC_BD_TH_2f,   OCB_RJCT_SET_OC_BD_TH_3f};
    const soc_field_t  rjct_clr_oc_bd_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {    MIXED_RJCT_CLR_OC_BD_TH_0f, MIXED_RJCT_CLR_OC_BD_TH_1f, MIXED_RJCT_CLR_OC_BD_TH_2f, MIXED_RJCT_CLR_OC_BD_TH_3f,
                                                                                        OCB_RJCT_CLR_OC_BD_TH_0f,   OCB_RJCT_CLR_OC_BD_TH_1f,   OCB_RJCT_CLR_OC_BD_TH_2f,   OCB_RJCT_CLR_OC_BD_TH_3f};
    const soc_field_t rjct_clr_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {     RJCT_CLR_FR_DB_MNMC_TH_0f, RJCT_CLR_FR_DB_MNMC_TH_1f, RJCT_CLR_FR_DB_MNMC_TH_2f, RJCT_CLR_FR_DB_MNMC_TH_3f};
    const soc_field_t rjct_set_fr_bd_mmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {     RJCT_SET_FR_DB_MNMC_TH_0f, RJCT_SET_FR_DB_MNMC_TH_1f, RJCT_SET_FR_DB_MNMC_TH_2f, RJCT_SET_FR_DB_MNMC_TH_3f};
    const soc_field_t rjct_clr_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {    RJCT_CLR_FR_DB_FLMC_TH_0f, RJCT_CLR_FR_DB_FLMC_TH_1f, RJCT_CLR_FR_DB_FLMC_TH_2f, RJCT_CLR_FR_DB_FLMC_TH_3f};
    const soc_field_t rjct_set_fr_bd_flmc_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {    RJCT_SET_FR_DB_FLMC_TH_0f, RJCT_SET_FR_DB_FLMC_TH_1f, RJCT_SET_FR_DB_FLMC_TH_2f, RJCT_SET_FR_DB_FLMC_TH_3f};
    const soc_field_t rjct_dyn_size_clr_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {  MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_0f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_1f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_2f, MIXED_DYN_WORDS_SIZE_RJCT_CLR_TH_3f,
                                                                                        OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_0f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_1f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_2f,   OCB_DYN_WORDS_SIZE_RJCT_CLR_TH_3f};
    const soc_field_t rjct_dyn_size_set_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = {  MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_0f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_1f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_2f, MIXED_DYN_WORDS_SIZE_RJCT_SET_TH_3f,
                                                                                        OCB_DYN_WORDS_SIZE_RJCT_SET_TH_0f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_1f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_2f,   OCB_DYN_WORDS_SIZE_RJCT_SET_TH_3f};
    const soc_field_t rjct_set_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {        RJCT_SET_FR_OCB_TH_0f, RJCT_SET_FR_OCB_TH_1f, RJCT_SET_FR_OCB_TH_2f, RJCT_SET_FR_OCB_TH_3f};
    soc_field_t rjct_clr_fr_ocb_th_fields[SOC_TMC_NOF_DROP_PRECEDENCE] = {              RJCT_CLR_FR_OCB_TH_0f, RJCT_CLR_FR_OCB_TH_1f, RJCT_CLR_FR_OCB_TH_2f, RJCT_CLR_FR_OCB_TH_3f};
    SOC_TMC_THRESH_WITH_HYST_INFO shrd_pool_th;


    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(info);
    SOCDNX_NULL_CHECK(exact_info);

    if ((core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) && core_id != SOC_CORE_ALL) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    if (info->ocb_shrd_pool[pool_id][drop_precedence_ndx].clear > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    } 
                    if (info->ocb_shrd_pool[pool_id][drop_precedence_ndx].set > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                } else {
                    if (info->mix_shrd_pool[pool_id][drop_precedence_ndx].clear > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                    if (info->mix_shrd_pool[pool_id][drop_precedence_ndx].set > SOC_TMC_ITM_SHRD_RJCT_TH_MAX) {
                        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                    }
                }
            }
        }
    }
    
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
            if (is_ocb_only) {
                if (info->ocb_bdbs[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (info->ocb_bdbs[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
            } else {
                if (info->bdbs[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
                if (info->bdbs[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX) {
                    SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
                }
            }
        }
    }
   
    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
            if (is_ocb_only) {
                
            } else {
                
            }
        }
    }
    
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->mini_mc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->mini_mc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->full_mc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->full_mc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }
    
    for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
        if (info->ocb_mc[drop_precedence_ndx].set > SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX) {
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
        if (info->ocb_mc[drop_precedence_ndx].clear > SOC_TMC_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX) {       
            SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
        }
    }

    for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
        for (pool_id = 0; pool_id < SOC_TMC_ITM_NOF_RSRC_POOLS; pool_id++) {
            for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
                if (is_ocb_only == TRUE) {
                    shrd_pool_th.clear = info->ocb_shrd_pool[pool_id][drop_precedence_ndx].clear;
                    shrd_pool_th.set = info->ocb_shrd_pool[pool_id][drop_precedence_ndx].set;
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_OCB_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = OCB_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = OCB_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                } else {
                    shrd_pool_th.clear = info->mix_shrd_pool[pool_id][drop_precedence_ndx].clear;
                    shrd_pool_th.set = info->mix_shrd_pool[pool_id][drop_precedence_ndx].set;
                    if (pool_id == 0) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_0_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_0_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_0_RJCT_CLR_TH_DP_3f;
                        }
                    } 
                    if (pool_id == 1) {
                        shared_pool_reg = IQM_MIX_SHRD_POOL_1_RSRC_RJCT_THr;
                        if (drop_precedence_ndx == 0) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_0f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_0f;
                        } else if (drop_precedence_ndx == 1) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_1f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_1f;
                        } else if (drop_precedence_ndx == 2) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_2f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_2f;
                        } else if (drop_precedence_ndx == 3) {
                            shared_pool_field_set = MIX_SHRD_POOL_1_RJCT_SET_TH_DP_3f;
                            shared_pool_field_clr = MIX_SHRD_POOL_1_RJCT_CLR_TH_DP_3f;
                        }
                    }
                }
                res = soc_reg_above_64_get(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);

                soc_reg_above_64_field32_set(unit, shared_pool_reg, shared_pool_data, shared_pool_field_set, shrd_pool_th.set);
                soc_reg_above_64_field32_set(unit, shared_pool_reg, shared_pool_data, shared_pool_field_clr, shrd_pool_th.clear);

                res = soc_reg_above_64_set(unit, shared_pool_reg, core_id, 0, shared_pool_data);
                SOCDNX_IF_ERR_EXIT(res);
            }
        }
    }
  
  bdbs_th_mnt_nof_bits = 8;
  bdbs_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, rjct_set_fr_bdb_th_fields[0]) - bdbs_th_mnt_nof_bits;
  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bdbs[drop_precedence_ndx].clear, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bdbs[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bdbs[drop_precedence_ndx].clear, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bdbs[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_clr_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bdbs[drop_precedence_ndx].set, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bdbs[drop_precedence_ndx].set, bdbs_th_mnt_nof_bits, bdbs_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bdbs[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bdbs_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_BD_BSr, 
                    core_id, 
                    0, 
                    rjct_set_fr_bdb_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }
  

  
  bds_th_mnt_nof_bits = 8;
  bds_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, rjct_clr_oc_bd_th_fields[0]) - bds_th_mnt_nof_bits;
  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bds[drop_precedence_ndx].clear, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bds[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bds[drop_precedence_ndx].clear, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bds[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit,
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr,
                    core_id,
                    0,
                    rjct_clr_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_bds[drop_precedence_ndx].set, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->bds[drop_precedence_ndx].set, bds_th_mnt_nof_bits, bds_th_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->bds[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, bds_th_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_GENERAL_REJECT_CONFIGURATION_B_DSr, 
                    core_id, 
                    0, 
                    rjct_set_oc_bd_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx],
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }
  

  mini_mc_mnt_nof_bits = 8;
  mini_mc_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, rjct_clr_fr_bd_mmc_th_fields[0]) - mini_mc_mnt_nof_bits;

  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_sand_break_to_mnt_exp_round_up(info->mini_mc[drop_precedence_ndx].clear, mini_mc_mnt_nof_bits, mini_mc_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->mini_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mini_mc_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_clr_fr_bd_mmc_th_fields[drop_precedence_ndx],  
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);
      
      res = soc_sand_break_to_mnt_exp_round_up(info->mini_mc[drop_precedence_ndx].set, mini_mc_mnt_nof_bits, mini_mc_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->mini_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mini_mc_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit, 
                IQM_REJECT_CONFIGURATION_MINI_MC_D_BSr, 
                core_id, 
                0, 
                rjct_set_fr_bd_mmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);
  }

  

  fmc_th_mnt_nof_bits = 8;
  fmc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr, rjct_clr_fr_bd_flmc_th_fields[0]) - fmc_th_mnt_nof_bits;

  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
      res = soc_sand_break_to_mnt_exp_round_up(info->full_mc[drop_precedence_ndx].clear, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->full_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_clr_fr_bd_flmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);

      res = soc_sand_break_to_mnt_exp_round_up(info->full_mc[drop_precedence_ndx].set, fmc_th_mnt_nof_bits, fmc_th_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->full_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));

      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, fmc_th_mnt_nof_bits, &exp_man);
      res = soc_reg_above_64_field32_modify(
                unit,
                IQM_REJECT_CONFIGURATION_FULL_MC_D_BSr,
                core_id,
                0,
                rjct_set_fr_bd_flmc_th_fields[drop_precedence_ndx],
                exp_man);
      SOCDNX_IF_ERR_EXIT(res);

  }

  
  mc_ocb_mnt_nof_bits = 8;
  mc_ocb_exp_nof_bits = soc_reg_field_length(unit, IQM_REJECT_CONFIGURATION_OCBSr, rjct_set_fr_ocb_th_fields[0]) - mc_ocb_mnt_nof_bits;
  for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) 
  {
      
      res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mc[drop_precedence_ndx].set, mc_ocb_mnt_nof_bits, mc_ocb_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->ocb_mc[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mc_ocb_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_CONFIGURATION_OCBSr, core_id, 0, rjct_set_fr_ocb_th_fields[drop_precedence_ndx], exp_man);
      SOCDNX_IF_ERR_EXIT(res);

      
      res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mc[drop_precedence_ndx].clear, mc_ocb_mnt_nof_bits, mc_ocb_exp_nof_bits, 0, &mnt_val, &exp_val);
      SOCDNX_SAND_IF_ERR_EXIT(res);

      exact_info->ocb_mc[drop_precedence_ndx].clear = (mnt_val) * (1 << (exp_val));
      arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, mc_ocb_mnt_nof_bits, &exp_man);

      res = soc_reg_above_64_field32_modify(unit, IQM_REJECT_CONFIGURATION_OCBSr, core_id, 0, rjct_clr_fr_ocb_th_fields[drop_precedence_ndx], exp_man);
      SOCDNX_IF_ERR_EXIT(res);
  }

  

  for (is_ocb_only = 0; is_ocb_only < SOC_TMC_ITM_NOF_RSRC_POOLS; is_ocb_only++) {
      for (drop_precedence_ndx = 0; drop_precedence_ndx < SOC_TMC_NOF_DROP_PRECEDENCE; drop_precedence_ndx++) {
          dyn_mnt_nof_bits = 8;
          dyn_exp_nof_bits = 
              soc_reg_field_length(
                 unit, 
                 IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                 rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx]) - dyn_mnt_nof_bits;

          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mem_excess[drop_precedence_ndx].clear, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->mem_excess[drop_precedence_ndx].clear, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->mem_excess[drop_precedence_ndx].clear = (mnt_val) * (1<<(exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          }

          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_clr_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);

          if (is_ocb_only) {
              res = soc_sand_break_to_mnt_exp_round_up(info->ocb_mem_excess[drop_precedence_ndx].set, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->ocb_mem_excess[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          } else {
              res = soc_sand_break_to_mnt_exp_round_up(info->mem_excess[drop_precedence_ndx].set, dyn_mnt_nof_bits, dyn_exp_nof_bits, 0, &mnt_val, &exp_val);
              SOCDNX_SAND_IF_ERR_EXIT(res);

              exact_info->mem_excess[drop_precedence_ndx].set = (mnt_val) * (1 << (exp_val));
              arad_iqm_mantissa_exponent_set(unit, mnt_val, exp_val, dyn_mnt_nof_bits, &exp_man);
          }
          res = soc_reg_above_64_field32_modify(
                    unit, 
                    IQM_DRAM_DYN_SIZE_RJCT_TH_CFGr, 
                    core_id, 
                    0, 
                    rjct_dyn_size_set_th_fields[(is_ocb_only * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx], 
                    exp_man);
          SOCDNX_IF_ERR_EXIT(res);
      }
  }

exit: 
    SOCDNX_FUNC_RETURN; 
}

int
jer_itm_vsq_src_reserve_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_IN  uint32                  reserved_amount,
   SOC_SAND_OUT uint32*                 exact_reserved_amount
   ) 
{
    int res;
    uint32 
        entry_offset,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    uint32
        exp_man,
        max_size_mnt_nof_bits,
        max_size_exp_nof_bits,
        max_size_mnt,
        max_size_exp;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(exact_reserved_amount);
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (reserved_amount > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    max_size_mnt_nof_bits = 8;
    max_size_exp_nof_bits = soc_mem_field_length(unit, IQM_VSQE_PRMm, VQ_MIN_THf) - max_size_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(reserved_amount, max_size_mnt_nof_bits, max_size_exp_nof_bits, 0, &max_size_mnt, &max_size_exp);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    *exact_reserved_amount = (max_size_mnt * (1 << max_size_exp));
    arad_iqm_mantissa_exponent_set(unit, max_size_mnt, max_size_exp, max_size_mnt_nof_bits, &exp_man);

    
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        res = READ_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_IQM_VSQE_PRMm_field32_set(unit, data, VQ_MIN_THf, exp_man);

        res = WRITE_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
    }
    
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_PG) {
        res = READ_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        soc_IQM_VSQF_PRMm_field32_set(unit, data, VQ_MIN_THf, exp_man);

        res = WRITE_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
    } else {
        SOCDNX_IF_ERR_EXIT(res);
    }

    *exact_reserved_amount = reserved_amount;
exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_vsq_src_reserve_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  SOC_TMC_ITM_VSQ_GROUP   vsq_type,
   SOC_SAND_IN  uint32                  vsq_rt_cls_ndx,
   SOC_SAND_IN  uint32                  drop_precedence_ndx,
   SOC_SAND_IN  int                     pool_id,
   SOC_SAND_OUT uint32                  *reserved_amount
   ) 
{
    int res;
    uint32 
        entry_offset,
        data[JER_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE] = {0};
    uint32
        exp_man,
        max_size_mnt,
        max_size_exp;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reserved_amount);
    if (pool_id < 0 || pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (vsq_rt_cls_ndx > SOC_TMC_ITM_VSQ_QT_RT_CLS_MAX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (drop_precedence_ndx >= SOC_TMC_NOF_DROP_PRECEDENCE) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    entry_offset = (vsq_rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;

    
    if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_SRC_PORT) {
        res = READ_IQM_VSQE_PRMm(unit, pool_id, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        exp_man = soc_IQM_VSQE_PRMm_field32_get(unit, data, VQ_MIN_THf);
    } else if (vsq_type == SOC_TMC_ITM_VSQ_GROUP_PG) {
        
        res = READ_IQM_VSQF_PRMm(unit, IQM_BLOCK(unit, core_id), entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);

        exp_man = soc_IQM_VSQF_PRMm_field32_get(unit, data, VQ_MIN_THf);
    } else {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }

    arad_iqm_mantissa_exponent_get(unit, exp_man, 8, &max_size_mnt, &max_size_exp);
    *reserved_amount = (max_size_mnt * (1 << max_size_exp));

exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_resource_allocation_set(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_IN  uint32                  max_shared_pool,
   SOC_SAND_IN  uint32                  max_headroom
   ) 
{
    uint32
        res;
    soc_reg_above_64_val_t 
        glbl_ocb_th_data;
    uint64 
        glbl_th_data;
    SOCDNX_INIT_FUNC_DEFS;
    if (core_id != SOC_CORE_ALL && 
       (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_shared_pool > SOC_TMC_ITM_RESOURCE_ALLOCATION_SHARED_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_headroom > SOC_TMC_ITM_RESOURCE_ALLOCATION_SHARED_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_shared_pool > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (max_headroom > JER_ITM_VSQ_GRNT_BD_SIZE_MAX) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (is_ocb_only) {
        COMPILER_64_ZERO(glbl_th_data);
        res = READ_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, &glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_SHRD_POOL_0_MAX_THf, max_shared_pool);
        } else if (pool_id == 1) {
            soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_SHRD_POOL_1_MAX_THf, max_shared_pool);
        }
        soc_reg64_field32_set(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, &glbl_th_data, GLBL_OCB_HDRM_MAX_THf, max_headroom);
        res = WRITE_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
    } else {
        res = READ_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_0_MAX_THf, max_shared_pool);
        } else if (pool_id == 1) {
            soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_1_MAX_THf, max_shared_pool);
        }
        soc_reg_above_64_field32_set(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_HDRM_MAX_THf, max_headroom);

        res = WRITE_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
    }

    SOCDNX_IF_ERR_EXIT(res);
exit: 
    SOCDNX_FUNC_RETURN;
}

int
jer_itm_resource_allocation_get(
   SOC_SAND_IN  int                     unit,
   SOC_SAND_IN  int                     core_id,
   SOC_SAND_IN  uint8                   pool_id,
   SOC_SAND_IN  uint8                   is_ocb_only,
   SOC_SAND_OUT uint32                  *max_shared_pool,
   SOC_SAND_OUT uint32                  *max_headroom
   ) 
{
    uint32
        res = BCM_E_NONE;
    soc_reg_above_64_val_t 
        glbl_ocb_th_data;
    uint64 
        glbl_th_data;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(max_shared_pool);
    SOCDNX_NULL_CHECK(max_headroom);
    if (core_id != SOC_CORE_ALL && 
       (core_id < 0 || core_id >= SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (pool_id >= SOC_TMC_ITM_NOF_RSRC_POOLS) {
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
    }
    if (is_ocb_only) {
        COMPILER_64_ZERO(glbl_th_data);
        res = READ_IQM_SRC_VSQ_GLBL_OCB_MAX_THr(unit, core_id, &glbl_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            *max_shared_pool = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_SHRD_POOL_0_MAX_THf);
        } else if (pool_id == 1) {
            *max_shared_pool = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_SHRD_POOL_1_MAX_THf);
        }
        *max_headroom = soc_reg64_field32_get(unit, IQM_SRC_VSQ_GLBL_OCB_MAX_THr, glbl_th_data, GLBL_OCB_HDRM_MAX_THf);
    } else {
        res = READ_IQM_GLBL_MIX_MAX_THr(unit, core_id, glbl_ocb_th_data);
        SOCDNX_IF_ERR_EXIT(res);
        if (pool_id == 0) {
            *max_shared_pool = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_0_MAX_THf);
        } else if (pool_id == 1) {
            *max_shared_pool = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_SHRD_POOL_1_MAX_THf);
        }
        *max_headroom = soc_reg_above_64_field32_get(unit, IQM_GLBL_MIX_MAX_THr, glbl_ocb_th_data, GLBL_MIX_HDRM_MAX_THf);
    }

exit: 
    SOCDNX_FUNC_RETURN;
}

int
  jer_itm_dyn_total_thresh_set(
    SOC_SAND_IN int    unit,
    SOC_SAND_IN int    core,
    SOC_SAND_IN uint8  is_ocb_only,
                int32  reservation_increase_array[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_INGRESS_THRESHOLD_NOF_TYPES] 
  )
{
    uint32
        res;
    soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
    soc_dpp_guaranteed_pair_t *guaranteed_pair;
    int32 reservation_increase;
    int core_id;
    int *numerator = SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_numerator;
    int denominator = SOC_DPP_CONFIG(unit)->jer->tm.shared_mem_reject_percentage_denominator;
    soc_reg_t iqm_dyn_size_rjct_th_r[2] = 
                   {IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr, 
                    IQM_OCB_DYN_SIZE_RJCT_TH_CFGr};
    soc_field_t dyn_size_rjct_set_th_f[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = 
                   {DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_0f,
                    DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_1f,
                    DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_2f,
                    DRAM_BUFF_DYN_SIZE_RJCT_SET_TH_3f,
                    OCB_DYN_SIZE_RJCT_SET_TH_0f,
                    OCB_DYN_SIZE_RJCT_SET_TH_1f,
                    OCB_DYN_SIZE_RJCT_SET_TH_2f,
                    OCB_DYN_SIZE_RJCT_SET_TH_3f};
    soc_field_t dyn_size_rjct_clr_th_f[SOC_TMC_NOF_DROP_PRECEDENCE * 2] = 
                   {DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_0f,
                    DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_1f,
                    DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_2f,
                    DRAM_BUFF_DYN_SIZE_RJCT_CLR_TH_3f,
                    OCB_DYN_SIZE_RJCT_CLR_TH_0f,
                    OCB_DYN_SIZE_RJCT_CLR_TH_1f,
                    OCB_DYN_SIZE_RJCT_CLR_TH_2f,
                    OCB_DYN_SIZE_RJCT_CLR_TH_3f};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(reservation_increase_array);

    SOC_DPP_CORES_ITER(core, core_id){
        res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.get(unit, core_id, &guaranteed_q_resource);
        SOCDNX_IF_ERR_EXIT(res);
        guaranteed_pair = (is_ocb_only ? &(guaranteed_q_resource.ocb) : &(guaranteed_q_resource.dram));
        reservation_increase = reservation_increase_array[core_id][0];

        if (reservation_increase < 0 &&  ((uint32)(-reservation_increase)) > guaranteed_pair->used) {
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        
        if (reservation_increase) {
            int32 resource_left_calc =  ((int32)(guaranteed_pair->total -
                                                 (guaranteed_pair->used))) - reservation_increase;
            uint32 resource_left =  resource_left_calc;
            if (resource_left_calc < 0) { 
                
                SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
            }

            if (SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_MEMORY) {
                
                
            } else {
                soc_reg_above_64_val_t reg_above_64_val;

                uint32 reg_dp0 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
                uint32 reg_dp1 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
                uint32 reg_dp2 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
                uint32 reg_dp3 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
                uint32 mantissa, exponent, field_val;

                SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
          
                if (resource_left) { 
                    res = soc_sand_break_to_mnt_exp_round_down(
                            (resource_left / denominator) * numerator[0],
                            ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS,
                            ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                            0,
                            &mantissa, 
                            &exponent);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
                } else {
                    field_val = reg_dp0;
                }
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_set_th_f[SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only], field_val);
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_clr_th_f[SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only], field_val);
                if (resource_left) {
                    res = soc_sand_break_to_mnt_exp_round_down(
                            (resource_left / denominator) * numerator[1],
                            ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                            ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                            0,
                            &mantissa, 
                            &exponent);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
                } else {
                    field_val = reg_dp1;
                }

                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_set_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 1], field_val);
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_clr_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 1], field_val);
                if (resource_left) {
                    res = soc_sand_break_to_mnt_exp_round_down(
                            (resource_left / denominator) * numerator[2],
                            ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                            ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                            0, 
                            &mantissa, 
                            &exponent);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
                } else {
                    field_val = reg_dp2;
                }
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_set_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 2], field_val);
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_clr_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 2], field_val);
                if (resource_left) {
                    res = soc_sand_break_to_mnt_exp_round_down(
                            (resource_left / denominator) * numerator[3],
                            ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, 
                            ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
                            0, 
                            &mantissa, 
                            &exponent);
                    SOCDNX_SAND_IF_ERR_EXIT(res);
                    field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
                } else {
                    field_val = reg_dp3;
                }
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_set_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 3], field_val);
                soc_reg_above_64_field32_set(unit, iqm_dyn_size_rjct_th_r[is_ocb_only], reg_above_64_val, dyn_size_rjct_clr_th_f[(SOC_TMC_NOF_DROP_PRECEDENCE * is_ocb_only) + 3], field_val);

                if (is_ocb_only) {
                    res = WRITE_IQM_OCB_DYN_SIZE_RJCT_TH_CFGr(unit, core_id, reg_above_64_val);
                    SOCDNX_IF_ERR_EXIT(res);
                } else {
                    res = WRITE_IQM_DRAM_BUFF_DYN_SIZE_RJCT_TH_CFGr(unit, core_id, reg_above_64_val);
                    SOCDNX_IF_ERR_EXIT(res);
                }
            }
            guaranteed_pair->used += reservation_increase;
        }
        res = sw_state_access[unit].dpp.soc.arad.tm.guaranteed_q_resource.set(unit, core_id, &guaranteed_q_resource);
        SOCDNX_SAND_IF_ERR_EXIT(res);
    }
exit:
  SOCDNX_FUNC_RETURN;
}

int
  jer_ingress_drop_status(
    SOC_SAND_IN int   unit,
    SOC_SAND_OUT uint32 *is_max_size 
  )
{
    uint64 val;
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    for (core=0; core<SOC_DPP_DEFS_GET(unit, nof_cores) ; core++) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_REJECT_STATUS_BMPr(unit, core, &val)); 
        (*is_max_size) = soc_reg64_field32_get(unit, IQM_REJECT_STATUS_BMPr, val,  VOQ_MX_QSZ_STATUSf);
        if (*is_max_size != 0){
            break;
        }
    }

    
exit:
  SOCDNX_FUNC_RETURN;
}


int
  jer_itm_sys_red_queue_size_boundaries_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);
  SOCDNX_NULL_CHECK(exact_info);

  res = arad_itm_sys_red_queue_size_boundaries_verify(
          unit,
          rt_cls_ndx,
          info
          );
  SOCDNX_SAND_IF_ERR_EXIT(res);

  res = x_itm_sys_red_queue_size_boundaries_set_unsafe(
    unit,
    rt_cls_ndx,
    info,
    exact_info,
    JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
    JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
    JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
    JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS
    );
  SOCDNX_SAND_IF_ERR_EXIT(res);
exit:
  SOCDNX_FUNC_RETURN;
}


int
  jer_itm_sys_red_queue_size_boundaries_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;
  SOCDNX_NULL_CHECK(info);

  res = x_itm_sys_red_queue_size_boundaries_get_unsafe(
    unit,
    rt_cls_ndx,
    info,
    JER_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
    JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
    JER_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
    JER_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS
    );
  SOCDNX_SAND_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}


int
  jer_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    index;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    if (info->thresholds.unicast_rng_ths[index] > JER_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    if (info->thresholds.multicast_rng_ths[index] > JER_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    if (info->thresholds.bds_rng_ths[index] > JER_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    if (info->thresholds.ocb_rng_ths[index] > JER_ITM_SYS_RED_GLOB_RCS_THS_OCB_RNG_THS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }

  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    if (info->values.unicast_rng_vals[index] > JER_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    if (info->values.multicast_rng_vals[index] > JER_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    if (info->values.bds_rng_vals[index] > JER_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    if (info->values.ocb_rng_vals[index] > JER_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX)
      SOCDNX_IF_ERR_EXIT(SOC_E_PARAM);
  }

exit:
  SOCDNX_FUNC_RETURN;
}


int
    jer_itm_sys_red_glob_rcs_set(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info)
{
    int    res;
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(info);
    res = jer_itm_sys_red_glob_rcs_verify(unit, info);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = soc_reg_above_64_field32_modify(unit, IQM_SYSTEM_RED_FREE_MNMC_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_MNMC_DB_TH_0f,  info->thresholds.unicast_rng_ths[0]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_SYSTEM_RED_FREE_MNMC_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_MNMC_DB_TH_1f,  info->thresholds.unicast_rng_ths[1]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_SYSTEM_RED_FREE_MNMC_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_MNMC_DB_TH_2f,  info->thresholds.unicast_rng_ths[2]);
    SOCDNX_IF_ERR_EXIT(res);
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_MNMC_DBUFF_RANGE_VALUESr, FR_MNMC_DB_RNG_VAL_0f, info->values.unicast_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_MNMC_DBUFF_RANGE_VALUESr, FR_MNMC_DB_RNG_VAL_1f, info->values.unicast_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_MNMC_DBUFF_RANGE_VALUESr, FR_MNMC_DB_RNG_VAL_2f, info->values.unicast_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_MNMC_DBUFF_RANGE_VALUESr, FR_MNMC_DB_RNG_VAL_3f, info->values.unicast_rng_vals[3], reg_val, 40, exit);
    res = WRITE_IQM_FREE_MNMC_DBUFF_RANGE_VALUESr(unit, SOC_CORE_ALL, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_FLMC_DB_TH_0f,  info->thresholds.multicast_rng_ths[0]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_FLMC_DB_TH_1f,  info->thresholds.multicast_rng_ths[1]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLDr, REG_PORT_ANY, 0, FR_FLMC_DB_TH_2f,  info->thresholds.multicast_rng_ths[2]);
    SOCDNX_IF_ERR_EXIT(res);
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FLMC_DB_RNG_VAL_0f, info->values.multicast_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FLMC_DB_RNG_VAL_1f, info->values.multicast_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FLMC_DB_RNG_VAL_2f, info->values.multicast_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FLMC_DB_RNG_VAL_3f, info->values.multicast_rng_vals[3], reg_val, 40, exit);
    res = WRITE_IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr(unit, SOC_CORE_ALL, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_OCB_THRESHOLDr, REG_PORT_ANY, 0, FR_OCB_TH_0f,  info->thresholds.ocb_rng_ths[0]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_OCB_THRESHOLDr, REG_PORT_ANY, 0, FR_OCB_TH_1f,  info->thresholds.ocb_rng_ths[1]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_OCB_THRESHOLDr, REG_PORT_ANY, 0, FR_OCB_TH_2f,  info->thresholds.ocb_rng_ths[2]);
    SOCDNX_IF_ERR_EXIT(res);
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_OCB_RANGE_VALUESr, FR_OCB_RNG_VAL_0f, info->values.ocb_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_OCB_RANGE_VALUESr, FR_OCB_RNG_VAL_1f, info->values.ocb_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_OCB_RANGE_VALUESr, FR_OCB_RNG_VAL_2f, info->values.ocb_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_OCB_RANGE_VALUESr, FR_OCB_RNG_VAL_3f, info->values.ocb_rng_vals[3], reg_val, 40, exit);
    res = WRITE_IQM_FREE_OCB_RANGE_VALUESr(unit, SOC_CORE_ALL, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

    
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLDr, REG_PORT_ANY, 0, FR_BDB_TH_0f,  info->thresholds.bds_rng_ths[0]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLDr, REG_PORT_ANY, 0, FR_BDB_TH_1f,  info->thresholds.bds_rng_ths[1]);
    SOCDNX_IF_ERR_EXIT(res);
    res = soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLDr, REG_PORT_ANY, 0, FR_BDB_TH_2f,  info->thresholds.bds_rng_ths[2]);
    SOCDNX_IF_ERR_EXIT(res);
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_0f, info->values.bds_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_1f, info->values.bds_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_2f, info->values.bds_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_3f, info->values.bds_rng_vals[3], reg_val, 40, exit);
    res = WRITE_IQM_FREE_BDB_RANGE_VALUESr(unit, SOC_CORE_ALL, reg_val);
    SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

STATIC int 
read_fields_into_array(int unit, int size, soc_reg_t reg, soc_port_t port, soc_field_t *field_arr, uint32 *out_arr)
{
    int i,
           res = BCM_E_NONE;
    for (i = 0 ; i < size; ++i)
    {
        res = soc_reg_above_64_field32_read(unit, reg, port, 0, field_arr[i], out_arr + i);
        if (BCM_E_NONE != res)
            break;
    }
    return res;
}

int
  jer_itm_sys_red_glob_rcs_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  int
    res;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(info);

   
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS - 1, IQM_SYSTEM_RED_FREE_MNMC_DBUFF_THRESHOLDr,
          REG_PORT_ANY, sys_red_free_mnmc_db_th_f, info->thresholds.unicast_rng_ths);
  SOCDNX_IF_ERR_EXIT(res);
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS, IQM_FREE_MNMC_DBUFF_RANGE_VALUESr,
          REG_PORT_ANY, sys_red_free_mnmc_rng_val_f, info->values.unicast_rng_vals);
  SOCDNX_IF_ERR_EXIT(res);

   
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS - 1, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLDr,
          REG_PORT_ANY, sys_red_free_flmc_db_th_f, info->thresholds.multicast_rng_ths);
  SOCDNX_IF_ERR_EXIT(res);
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS, IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr,
          REG_PORT_ANY, sys_red_free_flmc_rng_val_f, info->values.multicast_rng_vals);
  SOCDNX_IF_ERR_EXIT(res);

   
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS - 1, IQM_FREE_OCB_THRESHOLDr,
          REG_PORT_ANY, sys_red_free_ocb_db_th_f, info->thresholds.ocb_rng_ths);
  SOCDNX_IF_ERR_EXIT(res);
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS, IQM_FREE_OCB_RANGE_VALUESr,
          REG_PORT_ANY, sys_red_free_ocb_rng_val_f, info->values.ocb_rng_vals);
  SOCDNX_IF_ERR_EXIT(res);

   
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS - 1, IQM_FREE_BDB_THRESHOLDr,
          REG_PORT_ANY, sys_red_free_dbd_db_th_f, info->thresholds.bds_rng_ths);
  SOCDNX_IF_ERR_EXIT(res);
  res = read_fields_into_array(unit, ARAD_ITM_SYS_RED_BUFFS_RNGS, IQM_FREE_BDB_RANGE_VALUESr,
          REG_PORT_ANY, sys_red_free_dbd_rng_val_f, info->values.bds_rng_vals);
  SOCDNX_IF_ERR_EXIT(res);

exit:
  SOCDNX_FUNC_RETURN;
}

int
  jer_itm_in_pp_port_scheduler_compensation_profile_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_IN   int  scheduler_profile
  )
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_PCMm(unit, IQM_BLOCK(unit, core), in_pp_port, &data));
    soc_IQM_SCH_PCMm_field32_set(unit, &data, IPP_SCH_CMP_PROFILEf, scheduler_profile);
    SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_PCMm(unit, IQM_BLOCK(unit, core), in_pp_port, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  jer_itm_in_pp_port_scheduler_compensation_profile_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          in_pp_port,
    SOC_SAND_OUT  int*  scheduler_profile
  )
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_PCMm(unit, IQM_BLOCK(unit, core), in_pp_port, &data));
    *scheduler_profile = soc_IQM_SCH_PCMm_field32_get(unit, &data, IPP_SCH_CMP_PROFILEf);

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_sched_compensation_offset_and_delta_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   queue_profile,
    SOC_SAND_IN int   queue_delta, 
    SOC_SAND_IN int   in_pp_port_profile,
    SOC_SAND_IN int   in_pp_port_delta, 
    SOC_SAND_OUT int* offset,
    SOC_SAND_OUT int* delta
  ) 
{
    int new_delta;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(offset);
    SOCDNX_NULL_CHECK(delta);

    *offset = (queue_profile << 3)  + in_pp_port_profile;

    new_delta = queue_delta + in_pp_port_delta;
    if ( new_delta > JER_ITM_SCHEDULER_DELTA_MAX || new_delta < JER_ITM_SCHEDULER_DELTA_MIN ) {
        new_delta = ( new_delta > JER_ITM_SCHEDULER_DELTA_MAX ?  JER_ITM_SCHEDULER_DELTA_MAX : JER_ITM_SCHEDULER_DELTA_MIN);
        LOG_WARN(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "warning: sum of compensation for queue profile %d (%d) and in_port profile %d (%d) exceeds %d. Truncating to %d\n"),
                  queue_profile, queue_delta, in_pp_port_profile, in_pp_port_delta, new_delta, new_delta));
    }
    *delta = new_delta;

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_credits_adjust_size_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_IN int   delta, 
    SOC_SAND_IN SOC_TMC_ITM_PKT_SIZE_ADJUST_INFO* additional_info
  ) 
{
    int offset = 0, core_id = 0;
    int data, delta_internal, delta_total;
    SOCDNX_INIT_FUNC_DEFS;
  
    
    if ((delta > JER_ITM_SCHEDULER_DELTA_MAX) || (delta < JER_ITM_SCHEDULER_DELTA_MIN)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Delta %d is out of range"), delta));
    }

    SOC_DPP_CORES_ITER(core, core_id) {

        data = 0;
        if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_QUEUE || adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_PORT) { 
            
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
            soc_mem_field32_set(unit, IQM_SCH_CPMm, &data, SCH_PROFILE_CMPf, delta_internal);          
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_CPMm(unit, IQM_BLOCK(unit, core_id), offset, &data));

        } else if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR) { 
            

            if (index > IQM_SCH_HAP_NOF_ENTRIES) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid append pointer id %d"), index));
            }
            
            
            delta_internal = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS);

            soc_mem_field32_set(unit, IQM_SCH_HAPMm, &data, SCH_HEADER_APPEND_CMPf, delta_internal);          
            SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_HAPMm(unit, IQM_BLOCK(unit, core_id), index, &data));
            
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid adjust_type %d"), adjust_type));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_credits_adjust_size_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   index, 
    SOC_SAND_IN SOC_TMC_ITM_CGM_PKT_SIZE_ADJUST_TYPE adjust_type,
    SOC_SAND_OUT int   *delta
  ) 
{
    int core_id = 0;
    int data = 0, delta_internal;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOCDNX_NULL_CHECK(delta);

    core_id = ((core == SOC_CORE_ALL) ? 0 : core);

    
    if (adjust_type == SOC_TMC_ITM_PKT_SIZE_ADJUST_APPEND_SIZE_PTR) { 

        if (index > IQM_SCH_HAP_NOF_ENTRIES) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid append pointer id %d"), index));
        }

        SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_HAPMm(unit, IQM_BLOCK(unit, core_id), index, &data));
        delta_internal = soc_mem_field32_get(unit, IQM_SCH_HAPMm, &data, SCH_HEADER_APPEND_CMPf);          

    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid adjust_type %d"), adjust_type));
    }


    
    *delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(delta_internal, EIGHT_BITS);

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_sch_final_delta_map_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_IN uint32   delta_profile
  )
{
    int index = 0, core_id;
    uint32 data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if ((delta > JER_ITM_SCHEDULER_DELTA_MAX) || (delta < JER_ITM_SCHEDULER_DELTA_MIN)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Delta %d is out of range"), delta));
    }

    
    index = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS);

    SOC_DPP_CORES_ITER(core, core_id) {
        SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_HCEm(unit, IQM_BLOCK(unit, core_id), index, &data));
        soc_mem_field32_set(unit, IQM_SCH_HCEm, &data, SCH_CMP_ENCf, delta_profile);
        SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_HCEm(unit, IQM_BLOCK(unit, core_id), index, &data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_sch_final_delta_map_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN int   delta,
    SOC_SAND_OUT uint32   *delta_profile
  )
{
    int index = 0, core_id;
    uint32 data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    index = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(delta, EIGHT_BITS);

    core_id = (core == SOC_CORE_ALL) ? 0 : core;

    SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_HCEm(unit, IQM_BLOCK(unit, core_id), index, &data));
    *delta_profile = soc_mem_field32_get(unit, IQM_SCH_HCEm, &data, SCH_CMP_ENCf);
     
exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_sch_final_delta_set (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_IN int   final_delta
  )
{
    int core_id;
    soc_reg_above_64_val_t 
          fld_above_64_val, 
          reg_above_64_val;
    uint32 val;

    SOCDNX_INIT_FUNC_DEFS;

    
    if ((final_delta > JER_ITM_SCHEDULER_DELTA_MAX) || (final_delta < JER_ITM_SCHEDULER_DELTA_MIN)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Final delta %d is out of range"), final_delta));
    }

    
    val = CONVERT_SIGNED_NUM_TO_TWO_COMPLEMENT_METHOD(final_delta, EIGHT_BITS);

    SOC_DPP_CORES_ITER(core, core_id) {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
        SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);

        SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_CMP_PTR_MAPr(unit, core_id, reg_above_64_val));

        
        soc_reg_above_64_field_get(unit, IQM_SCH_CMP_PTR_MAPr, reg_above_64_val, SCH_CMP_PTR_MAPf, fld_above_64_val);        
        SHR_BITCOPY_RANGE(fld_above_64_val, (delta_profile*JER_ITM_SCHEDULER_DELTA_NOF_BITS), &val, 0, JER_ITM_SCHEDULER_DELTA_NOF_BITS);
        soc_reg_above_64_field_set(unit, IQM_SCH_CMP_PTR_MAPr, reg_above_64_val, SCH_CMP_PTR_MAPf, fld_above_64_val);

        SOCDNX_IF_ERR_EXIT(WRITE_IQM_SCH_CMP_PTR_MAPr(unit, core_id, reg_above_64_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_sch_final_delta_get (
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core,
    SOC_SAND_IN uint32   delta_profile,
    SOC_SAND_OUT int   *final_delta
  )
{
    int core_id;
    soc_reg_above_64_val_t 
          fld_above_64_val, 
          reg_above_64_val;
    uint32 val;

    SOCDNX_INIT_FUNC_DEFS;

    core_id = (core == BCM_CORE_ALL) ? 0 : core;
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);

    SOCDNX_IF_ERR_EXIT(READ_IQM_SCH_CMP_PTR_MAPr(unit, core_id, reg_above_64_val));

    
    soc_reg_above_64_field_get(unit, IQM_SCH_CMP_PTR_MAPr, reg_above_64_val, SCH_CMP_PTR_MAPf, fld_above_64_val);        
    SHR_BITCOPY_RANGE(&val, 0, fld_above_64_val, (delta_profile*JER_ITM_SCHEDULER_DELTA_NOF_BITS), JER_ITM_SCHEDULER_DELTA_NOF_BITS);

    
    *final_delta = CONVERT_TWO_COMPLEMENT_INTO_SIGNED_NUM(val, 8);

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_congestion_statistics_get(
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

      
    

                                           
    SOCDNX_IF_ERR_EXIT(READ_IQM_OCCUPIED_BD_COUNTERr(unit, core, &val));
    stats->bd_occupied = val; 	

    
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_BD_COUNTERr(unit, core, &val));
    stats->bd2_free = val; 		

    
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTERr(unit, core, &val));
    stats->db_full_mul_free = val; 	

    
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_U_MNMC_DBUFFS_COUNTERr(unit, core, &val));
    stats->db_mini_mul_free = val; 		

                                                                              
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_BDB_MINIMUM_OCCUPANCYr(unit, core, &val));
    stats->free_bdb_mini_occu = val; 			

    
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr(unit, core, &val));
    stats->free_bdb_full_mul_mini_occu = val; 		

                                                            
    SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_MNMC_DBUFFS_MINIMUM_OCCUPANCYr(unit, core, &val));
    stats->free_bdb_mini_mul_mini_occu = val; 		

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_min_free_resources_stat_get(
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
                                                                                      
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_BDB_MINIMUM_OCCUPANCYr(unit, core, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_OCB_BUFFERS:
                                                                                      
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_OCB_MINIMUM_OCCUPANCYr(unit, core, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_MINI_MC_BUFFERS:
                                                                    
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_MNMC_DBUFFS_MINIMUM_OCCUPANCYr(unit, core, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_FULL_MC_BUFFERS:
            
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr(unit, core, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_VOQ_DRAM_BDB:
            
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_VOQ_DRAM_DYNAMIC_MINIMAL_OCCUPANCYr(unit, core, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_VOQ_OCB_BDB:
            
            SOCDNX_IF_ERR_EXIT(READ_IQM_FREE_VOQ_OCB_DYNAMIC_MINIMAL_OCCUPANCYr(unit, core, &val32));
            break;

        default:
            SOCDNX_IF_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("Resource statistic type not supported for this device.")));
    }

    COMPILER_64_SET(*value, 0, val32);

exit:
    SOCDNX_FUNC_RETURN;
}


int jer_itm_ingress_latency_init(int unit)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 regVal;
    uint64 reg64Val;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
    SOCDNX_IF_ERR_EXIT(READ_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, reg_above_64_val));  
    
    soc_reg_above_64_field32_set(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, reg_above_64_val, EN_LATENCY_ERR_REPORTf, 0x1);          
    
    soc_reg_above_64_field32_set(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, reg_above_64_val, EN_LATENCY_DROPf, 0x0);  
    
    soc_reg_above_64_field32_set(unit, IPT_PACKET_LATENCY_MEASURE_CFGr, reg_above_64_val, EN_FLP_FBCRC_BIG_LATf, 0x0);         
    SOCDNX_IF_ERR_EXIT(WRITE_IPT_PACKET_LATENCY_MEASURE_CFGr(unit, reg_above_64_val));    
    
    
    
    SOCDNX_IF_ERR_EXIT(READ_IPT_PACKET_LATENCY_FORMATr(unit, &regVal));
    soc_reg_field_set(unit, IPT_PACKET_LATENCY_FORMATr, &regVal, LATENCY_FORMATf, 0x2);
    
        
    if(SOC_IS_JERICHO_PLUS(unit))
    {
        soc_reg_field_set(unit, IPT_PACKET_LATENCY_FORMATr, &regVal, LATENCY_ACCURACYf, 0x0);    
    }
    else
    {
        soc_reg_field_set(unit, IPT_PACKET_LATENCY_FORMATr, &regVal, LATENCY_ACCURACYf, 0x5);
    }

    SOCDNX_IF_ERR_EXIT(WRITE_IPT_PACKET_LATENCY_FORMATr(unit, regVal));
    
    
    
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, &reg64Val)); 
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64Val, PACKET_LATENCY_MESUREMENT_FLOW_ID_SELf, 0x4);    
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, 0, reg64Val));  
    SOCDNX_IF_ERR_EXIT(READ_IHB_LBP_GENERAL_CONFIG_0r(unit, 1, &reg64Val));  
    soc_reg64_field32_set(unit, IHB_LBP_GENERAL_CONFIG_0r, &reg64Val, PACKET_LATENCY_MESUREMENT_FLOW_ID_SELf, 0x4);    
    SOCDNX_IF_ERR_EXIT(WRITE_IHB_LBP_GENERAL_CONFIG_0r(unit, 1, reg64Val));  

exit:
    SOCDNX_FUNC_RETURN;    

}




int jer_itm_max_latency_pkts_get(int unit, int max_count, SOC_TMC_MAX_LATENCY_PACKETS_INFO *max_latency_packets, int *actual_count)
{
    int i,output_index=0;
    uint32 reg_val[4];
    uint32 tm_port, cosq, latency, modid;
    SOCDNX_INIT_FUNC_DEFS;

    
    for (i = 0; i < SOC_TMC_ING_NUMBER_OF_HIGHEST_LATENCY_PACKETS; i++)
    {        
        SOCDNX_IF_ERR_EXIT(READ_IPT_MAX_LATENCYm(unit, MEM_BLOCK_ANY, i, reg_val)); 
        soc_mem_field_get(unit, IPT_MAX_LATENCYm, reg_val, LAT_VALUEf, &latency);
        if(latency > 0)
        {
            if(output_index >= max_count)
            {
                SOCDNX_IF_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_SOC_MSG("index=%d. reach the max_count value."), output_index));
            }
            COMPILER_64_ZERO(max_latency_packets[output_index].latency);        
            COMPILER_64_ADD_32(max_latency_packets[output_index].latency,latency);
            soc_mem_field_get(unit, IPT_MAX_LATENCYm, reg_val, LAT_PORT_IDf, &tm_port);        
            soc_mem_field_get(unit, IPT_MAX_LATENCYm, reg_val, LAT_DEST_DEVf, &modid);        
            BCM_GPORT_MODPORT_SET(max_latency_packets[output_index].dest_gport, modid, tm_port);
            soc_mem_field_get(unit, IPT_MAX_LATENCYm, reg_val, LAT_TRF_CLSf, &cosq);                
            max_latency_packets[output_index].cosq = (int)cosq;
            soc_mem_field_get(unit, IPT_MAX_LATENCYm, reg_val, LAT_POINTERf, &max_latency_packets[output_index].latency_flow);                 
            output_index++;
        }
    }
    *actual_count = output_index;
exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_rate_limit_mpps_set(int unit, int rate)
{
    uint32 data = 0;
    uint32 sync_count = 0x12c; 
    int sync_count_field_length;
    uint32 core_freq = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency/1000; 
    int nof_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    int core_index;
    int enable = (rate != 0); 

    SOCDNX_INIT_FUNC_DEFS;


    if (rate >= core_freq*nof_cores/2) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Rate %d is too high"), rate));

    }
    if (enable) {
        
        sync_count = SOC_SAND_DIV_ROUND_UP((6*rate - core_freq*nof_cores),(core_freq*nof_cores - 2*rate));
    }

    sync_count_field_length = soc_reg_field_length(unit, IHP_SYNC_COUNTERr, SYNC_COUNTERf);
    if (sync_count >= (1<<sync_count_field_length)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't configure rate %d --  too high"), rate));
    }

    
    soc_reg_field_set(unit, IHP_SYNC_COUNTERr , &data, SYNC_COUNTERf, sync_count);

    SOC_DPP_CORES_ITER(SOC_DPP_CORE_ALL(unit), core_index) {
        
        SOCDNX_IF_ERR_EXIT(WRITE_IHP_SYNC_COUNTERr(unit, core_index, data));

        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, IHP_IHP_ENABLERSr, core_index, FORCE_BUBBLESf, enable));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int jer_itm_rate_limit_mpps_get(int unit, int* rate)
{
    uint32 data = 0;
    uint32 sync_count;
    uint32 core_freq = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency/1000; 
    int nof_cores = SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32 enable;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(rate);

    SOCDNX_IF_ERR_EXIT(READ_IHP_IHP_ENABLERSr(unit, REG_PORT_ANY, &data));
    enable = soc_reg_field_get(unit, IHP_IHP_ENABLERSr, data, FORCE_BUBBLESf);

    if (enable) {
        SOCDNX_IF_ERR_EXIT(READ_IHP_SYNC_COUNTERr(unit, REG_PORT_ANY, &data));
        sync_count = soc_reg_field_get(unit, IHP_SYNC_COUNTERr, data,  SYNC_COUNTERf);
        
        
        *rate = (sync_count+1)*core_freq*nof_cores/(6+2*sync_count); 
    } else {
        *rate = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
jer_itm_queue_flush_set(int unit, int core, int queue_id, int timeout)
{
    uint32 data = 0;
    int core_index;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_IPS_MANUAL_QUEUE_OPERATION_QUEUE_IDr(unit, core, &data));
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATION_QUEUE_IDr, &data, MAN_QUEUE_IDf, queue_id);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_MANUAL_QUEUE_OPERATION_QUEUE_IDr(unit, core, data));

    
    SOCDNX_IF_ERR_EXIT(READ_IPS_MANUAL_QUEUE_OPERATIONr(unit, core, &data));
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATIONr, &data, FLUSH_TRIGGERf, 1);
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATIONr, &data, DEL_DQCQf, 1);
    soc_reg_field_set(unit, IPS_MANUAL_QUEUE_OPERATIONr, &data, DEQ_CMD_CR_2_SENDf, JER_ITM_FLUSH_BYTES_UNITS_TO_SEND);
    SOCDNX_IF_ERR_EXIT(WRITE_IPS_MANUAL_QUEUE_OPERATIONr(unit, core, data));

    
    if (timeout)
    {
        BCM_DPP_CORES_ITER(core, core_index)
        {
            SOCDNX_IF_ERR_EXIT(soc_dpp_polling(unit, timeout, 100, IPS_MANUAL_QUEUE_OPERATIONr, core_index, 0, FLUSH_TRIGGERf, 0x0));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
#endif 
#include <soc/dpp/SAND/Utils/sand_footer.h> 

