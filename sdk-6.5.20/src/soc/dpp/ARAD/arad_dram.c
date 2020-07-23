/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DRAM





#ifdef BCM_DDR3_SUPPORT
#include <soc/shmoo_ddr40.h>
#endif 

#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h> 
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/mbcm.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_dram.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>

#include <soc/dpp/DRC/drc_combo28_init.h>
#include <soc/dpp/DRC/drc_combo28.h>
#include <soc/dpp/DRC/drc_combo28_cb.h>

#include <soc/dpp/JER/jer_dram.h>








#define ARAD_DRAM_MR_MRS_LSB         (14)
#define ARAD_DRAM_MR_MRS_MSB         (15)
#define ARAD_DRAM_MR0_MRS            ( 0)
#define ARAD_DRAM_MR1_MRS            ( 1)
#define ARAD_DRAM_MR2_MRS            ( 2)
#define ARAD_DRAM_MR3_MRS            ( 3)


#define ARAD_DRAM_MR0_DLL_LSB        ( 8)
#define ARAD_DRAM_MR0_DLL_MSB        ( 8)


#define ARAD_DRAM_MR0_BT             ( 0)


#define ARAD_DRAM_DDR3_WL_MIN        (5)
#define ARAD_DRAM_DDR3_WL_MAX        (10)

#define ARAD_DRAM_DDR3_CL_MIN        (4)
#define ARAD_DRAM_DDR3_CL_MAX        (15)

#define ARAD_DRAM_DDR3_WR_MIN        ( 5)
#define ARAD_DRAM_DDR3_WR_MAX        (17)



#define ARAD_DRAM_DDR3_MR0_BL_16     ( 2)
#define ARAD_DRAM_DDR3_MR0_BL_32     ( 0)

#define ARAD_DRAM_DDR3_MR0_PD        ( 0)



#define ARAD_DRAM_DDR3_MR1_DLL       ( 0)

#define ARAD_DRAM_DDR3_MR1_ODS0      ( 0)

#define ARAD_DRAM_DDR3_MR1_RTT0      ( 1)

#define ARAD_DRAM_DDR3_MR1_AL        ( 0)

#define ARAD_DRAM_DDR3_MR1_ODS1      ( 0)

#define ARAD_DRAM_DDR3_MR1_RTT1      ( 0)

#define ARAD_DRAM_DDR3_MR1_WL        ( 0)

#define ARAD_DRAM_DDR3_MR1_RTT2      ( 0)

#define ARAD_DRAM_DDR3_MR1_TQDS      ( 0)

#define ARAD_DRAM_DDR3_MR1_QOFF      ( 0)



#define ARAD_DRAM_DDR3_MR2_ASR       ( 0)

#define ARAD_DRAM_DDR3_MR2_SRT       ( 0)

#define ARAD_DRAM_DDR3_MR2_RTT       ( 1)




#define ARAD_DRAM_FREQ_MULTIPLIER 1.001
#define ARAD_DRAM_FREQ_INCREMENT 1

#define ARAD_DRAM_BIST_MIN_POLLS                          (10000000)


#define SOC_JER_DRAM_MMU_IND_DATA_LEN 256
#define SOC_ARAD_DRAM_MMU_IND_DATA_LEN 64
#define SOC_ARAD_DRAM_BUFFER_NUM_MAX 0x3fffff
#define SOC_JER_DRAM_BUFFER_NUM_MAX 0x7fffff
#define SOC_ARAD_MMU_DDR_ADDR_WIDTH 26


#define SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_READ 0x0
#define SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_WRITE 0x1






#define ARAD_DRAM_VAL(val)                                 \
          (SOC_SAND_GET_FLD_FROM_PLACE(val, ARAD_DRAM_VAL_SHIFT, ARAD_DRAM_VAL_MASK))

#define ARAD_DRAM_IS_VAL_IN_CLOCKS(val)                    \
          (SOC_SAND_GET_FLD_FROM_PLACE(val, ARAD_DRAM_VAL_IS_IN_CLOCKS_SHIFT, ARAD_DRAM_VAL_IS_IN_CLOCKS_MASK))

#define ARAD_DRAM_VAL_TO_CLOCKS(value)                     \
          (ARAD_DRAM_IS_VAL_IN_CLOCKS(value) ? (ARAD_DRAM_VAL(value)) : SOC_SAND_DIV_ROUND_UP(dram_freq * ARAD_DRAM_VAL(value), 1000000))


#define ARAD_DRAM_VAL_TO_CLOCKS_DOWN(value)                     \
          (ARAD_DRAM_IS_VAL_IN_CLOCKS(value) ? (ARAD_DRAM_VAL(value)) : SOC_SAND_DIV_ROUND_DOWN(dram_freq * (ARAD_DRAM_VAL(value) / 1000), 1000))

#define ARAD_DRAM_CLOCKS_TO_VAL(value)                     \
          SOC_SAND_DIV_ROUND_UP(1000000 * (value), dram_freq)

#define ARAD_DRAM_FLD_IN_PLACE_IF_EQ(val, eq, set, fld)    \
          ARAD_FLD_IN_PLACE_OLD(( ((val) == (eq)) ? set : 0x0), fld)
          
#define ARAD_DRAM_BUFFER_TEST_PATTERN 0xaa



#define MAX_NOF_BITS_GET(number, max_nof_bits)              for((max_nof_bits) = 0; (number) != 0; ++(max_nof_bits), (number) >>=1); \
                                                            if ((max_nof_bits != 0)) { --(max_nof_bits); }





typedef struct
{
  
  uint32 mr0;
  
  uint32 mr1;
  
  uint32 mr2;
  
  uint32 mr3;
  
  uint32 mr4;
  
  uint32 mr5;
  
  uint32 mr6;

} ARAD_DRAM_INTERNAL_MR_INFO;

typedef struct
{
  
  ARAD_DRAM_BURST_SIZE burst_size;
  
  ARAD_DRAM_NUM_COLUMNS nof_cols;
  
  ARAD_DRAM_AP_POSITION ap_bit_pos;
  
  ARAD_DRAM_NUM_BANKS nof_banks;
  
  uint32 c_cas_latency;
  
  uint32 c_wr_latency;
  
  uint32 c_rc;
  
  uint32 c_rfc;
  
  uint32 c_ras;
  
  uint32 c_faw;
  
  uint32 c_rcd_rd;
  
  uint32 c_rcd_wr;
  
  uint32 c_rrd;
  
  uint32 c_rp;
  
  uint32 c_wr;
  
  uint32 c_wtr;
  
  uint32 c_ref;
  
  uint32 c_rtp;
  
  uint32 c_rst;
  
  uint32 c_pcb;
  
  uint32 c_ras_rd;
  
  uint32 c_ras_wr;
  
  uint32 c_rd_ap;
  
  uint32 c_wr_ap;
  
  uint32 c_wr_rd;
  
  uint32 c_rd_wr;

  uint32 c_init_wait_prd;

} ARAD_DRAM_INTERNAL_INFO;

typedef struct
{
  uint8      msb;
  uint8      lsb;
}  ARAD_DRAM_MR_FIELD;

typedef struct
{
  struct
  {
    
    ARAD_DRAM_MR_FIELD bl;
    
    ARAD_DRAM_MR_FIELD cl3;
    
    ARAD_DRAM_MR_FIELD bt;
    
    ARAD_DRAM_MR_FIELD cl0;
    
    ARAD_DRAM_MR_FIELD wr;
    
    ARAD_DRAM_MR_FIELD pd;

  } mr0;

  struct
  {
    
    ARAD_DRAM_MR_FIELD dll;
    
    ARAD_DRAM_MR_FIELD ods0;
    
    ARAD_DRAM_MR_FIELD rtt0;
    
    ARAD_DRAM_MR_FIELD al;
    
    ARAD_DRAM_MR_FIELD ods1;
    
    ARAD_DRAM_MR_FIELD rtt1;
    
    ARAD_DRAM_MR_FIELD wl;
    
    ARAD_DRAM_MR_FIELD rtt2;
    
    ARAD_DRAM_MR_FIELD tqds;
    
    ARAD_DRAM_MR_FIELD qoff;

  } mr1;

  struct
  {
    
    ARAD_DRAM_MR_FIELD cwl;
    
    ARAD_DRAM_MR_FIELD asr;
    
    ARAD_DRAM_MR_FIELD srt;
    
    ARAD_DRAM_MR_FIELD rtt;

  } mr2;

} ARAD_DRAM_DDR3_MODE_REGS;

typedef union
{
  
  ARAD_DRAM_DDR3_MODE_REGS ddr3;

} ARAD_DRAM_MODE_REGS;

#define ARAD_DRAM_BUFFER_LIST_NONE 0xffffffff

typedef struct buffers_error_counters_s{
    uint32 buf;
    uint32 err_cntr;
} buffers_error_counters_t;  

buffers_error_counters_t buffers_error_counters[ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR];













STATIC void
  arad_mode_reg_fld_set(
    SOC_SAND_OUT ARAD_DRAM_MR_FIELD       *field,
    SOC_SAND_IN  uint8                   msb,
    SOC_SAND_IN  uint8                   lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
}


uint32 arad_mgmt_dram_init_drc_soft_init(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 *is_valid,
    SOC_SAND_IN uint32  init)
{
    uint32
        res, i;
    uint64
        reg_64;
    const static 
        soc_field_t drc_init_field[] = {DPRCA_INITf, DPRCB_INITf, DPRCC_INITf, DPRCD_INITf, DPRCE_INITf, DPRCF_INITf, DPRCG_INITf, DPRCH_INITf};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MGMT_DRAM_INIT_DRC_SOFT_INIT);

    SOC_SAND_CHECK_NULL_INPUT(is_valid);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg_get(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, &reg_64));
    for (i=0; i < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); i++) {
        if (is_valid[i]) {
            soc_reg64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, &reg_64, drc_init_field[i], init);
        }
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_set(unit, ECI_BLOCKS_SOFT_INITr, REG_PORT_ANY, 0, reg_64));

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): Perform drc soft init=%d.\n"),FUNCTION_NAME(), init));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_mgmt_dram_init_drc_soft_init()", 0, 0);
}


STATIC uint32
  arad_dram_set_mode_regs(
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_OUT ARAD_DRAM_MODE_REGS      *mode_regs
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_DRAM_SET_MODE_REGS);

  SOC_SAND_CHECK_NULL_INPUT(mode_regs);

  if (dram_type == ARAD_DRAM_TYPE_DDR3) {
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.bl)  ,  1,  0);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.cl3)  ,  2,  2);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.bt)  ,  3,  3);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.cl0)  ,  6,  4);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.wr)  , 11,  9);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr0.pd)  , 12, 12);

    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.dll) ,  0,  0);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.ods0),  1,  1);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt0),  2,  2);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.al)  ,  4,  3);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.ods1),  5,  5);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt1),  6,  6);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.wl)  ,  7,  7);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.rtt2),  9,  9);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.tqds), 11, 11);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr1.qoff), 12, 12);

    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr2.cwl) ,  5,  3);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr2.asr) ,  6,  6);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr2.srt) ,  7,  7);
    arad_mode_reg_fld_set( &(mode_regs->ddr3.mr2.rtt) , 10,  9);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_set_mode_regs()",0,0);
}


uint32 arad_dram_read_data_dly_get(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  dram_freq)
{
    if (dram_freq < 667) {
        return 0x3;  
    } else if (dram_freq < 900) {
        return 0x4;  
    }else {
        return 0x5;   
    }
}


STATIC uint32 arad_dram_ddr3_mrs0_wr_convert(uint32 c_wr)
{
    uint32 val = 0x0;

    switch (c_wr) {
    case 5:
        val = 0x1;
        break;
    case 6:
        val = 0x2;
        break;  
    case 7:
        val = 0x3;
        break;
    case 8:
        val = 0x4;
        break;
    case 9:
    case 10:
        val = 0x5;
        break;
    case 11:
    case 12:
        val = 0x6;
        break;
    case 13:
    case 14:
        val = 0x7;
        break;
    case 15:
    case 16:
    case 17:
        val = 0x0;
        break;
    default:
        val= 0x0;
    }

    return val;
}

STATIC uint32
  arad_dram_ddr3_mrs0_get(
    SOC_SAND_IN  ARAD_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= ARAD_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, ARAD_DRAM_BURST_SIZE_16, ARAD_DRAM_DDR3_MR0_BL_16, mode_regs->mr0.bl);
  reg_val |= ARAD_DRAM_FLD_IN_PLACE_IF_EQ(c_info->burst_size, ARAD_DRAM_BURST_SIZE_32, ARAD_DRAM_DDR3_MR0_BL_32, mode_regs->mr0.bl);

  
 reg_val |= ARAD_FLD_IN_PLACE_OLD(SOC_SAND_GET_FLD_FROM_PLACE((c_info->c_cas_latency - 4), 0x3, 0x8), mode_regs->mr0.cl3);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_MR0_BT, mode_regs->mr0.bt);

  
  reg_val |= ARAD_FLD_IN_PLACE_OLD(SOC_SAND_GET_FLD_FROM_PLACE(c_info->c_cas_latency - 4, 0x0, 0x7), mode_regs->mr0.cl0);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(arad_dram_ddr3_mrs0_wr_convert(c_info->c_wr), mode_regs->mr0.wr);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR0_PD, mode_regs->mr0.pd);

  reg_val |= SOC_SAND_SET_BITS_RANGE(ARAD_DRAM_MR0_MRS, ARAD_DRAM_MR_MRS_MSB, ARAD_DRAM_MR_MRS_LSB);
  return reg_val;
}


STATIC uint32
  arad_dram_ddr3_mrs1_get(
    SOC_SAND_IN  ARAD_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_DLL , mode_regs->mr1.dll );
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_ODS0, mode_regs->mr1.ods0);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_RTT0, mode_regs->mr1.rtt0);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_AL  , mode_regs->mr1.al  );
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_ODS1, mode_regs->mr1.ods1);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_RTT1, mode_regs->mr1.rtt1);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_WL  , mode_regs->mr1.wl  );
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_RTT2, mode_regs->mr1.rtt2);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_TQDS, mode_regs->mr1.tqds);
  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR1_QOFF, mode_regs->mr1.qoff);

  reg_val |= SOC_SAND_SET_BITS_RANGE(ARAD_DRAM_MR1_MRS, ARAD_DRAM_MR_MRS_MSB, ARAD_DRAM_MR_MRS_LSB);
  return reg_val;
}


STATIC uint32
  arad_dram_ddr3_mrs2_get(
    SOC_SAND_IN  ARAD_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= ARAD_FLD_IN_PLACE_OLD(c_info->c_wr_latency - 5, mode_regs->mr2.cwl);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR2_ASR, mode_regs->mr2.asr);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR2_SRT, mode_regs->mr2.srt);

  reg_val |= ARAD_FLD_IN_PLACE_OLD(ARAD_DRAM_DDR3_MR2_RTT, mode_regs->mr2.rtt);

  reg_val |= SOC_SAND_SET_BITS_RANGE(ARAD_DRAM_MR2_MRS, ARAD_DRAM_MR_MRS_MSB, ARAD_DRAM_MR_MRS_LSB);
  return reg_val;
}


STATIC uint32
  arad_dram_ddr3_mrs3_get(
    SOC_SAND_IN  ARAD_DRAM_DDR3_MODE_REGS *mode_regs,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    reg_val = 0;

  reg_val |= SOC_SAND_SET_BITS_RANGE(ARAD_DRAM_MR3_MRS, ARAD_DRAM_MR_MRS_MSB, ARAD_DRAM_MR_MRS_LSB);
  return reg_val;
}

STATIC uint32
  arad_dram_internal_refresh_delay_period_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq
  )
{
  uint32
    value = 60 + 12 * SOC_SAND_DIV_ROUND_DOWN(dram_freq, arad_chip_mega_ticks_per_sec_get(unit));

  return value;
}


STATIC uint32
  arad_dram_info_set_internal_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO    *c_info,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_MR_INFO *c_mr_info

  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
      field,

    reg_val;
  static soc_reg_t
    drc_ddr_controller_triggers[] = {DRCA_DDR_CONTROLLER_TRIGGERSr, DRCB_DDR_CONTROLLER_TRIGGERSr,
      DRCC_DDR_CONTROLLER_TRIGGERSr, DRCD_DDR_CONTROLLER_TRIGGERSr, DRCE_DDR_CONTROLLER_TRIGGERSr,
      DRCF_DDR_CONTROLLER_TRIGGERSr, DRCG_DDR_CONTROLLER_TRIGGERSr, DRCH_DDR_CONTROLLER_TRIGGERSr};
  static soc_reg_t
    drc_ddr_mode_register_1[] = {DRCA_DDR_MODE_REGISTER_1r, DRCB_DDR_MODE_REGISTER_1r,
      DRCC_DDR_MODE_REGISTER_1r, DRCD_DDR_MODE_REGISTER_1r, DRCE_DDR_MODE_REGISTER_1r,
      DRCF_DDR_MODE_REGISTER_1r, DRCG_DDR_MODE_REGISTER_1r, DRCH_DDR_MODE_REGISTER_1r};
  static soc_reg_t
    drc_ddr_mode_register_2[] = {DRCA_DDR_MODE_REGISTER_2r, DRCB_DDR_MODE_REGISTER_2r,
      DRCC_DDR_MODE_REGISTER_2r, DRCD_DDR_MODE_REGISTER_2r, DRCE_DDR_MODE_REGISTER_2r,
      DRCF_DDR_MODE_REGISTER_2r, DRCG_DDR_MODE_REGISTER_2r, DRCH_DDR_MODE_REGISTER_2r};
  static soc_reg_t
    drc_ddr_extended_mode_register_1[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_1r, DRCB_DDR_EXTENDED_MODE_REGISTER_1r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_1r, DRCD_DDR_EXTENDED_MODE_REGISTER_1r, DRCE_DDR_EXTENDED_MODE_REGISTER_1r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_1r, DRCG_DDR_EXTENDED_MODE_REGISTER_1r, DRCH_DDR_EXTENDED_MODE_REGISTER_1r};
  static soc_reg_t
    drc_extended_mode_wr_2_register[] = {DRCA_EXTENDED_MODE_WR_2_REGISTERr, DRCB_EXTENDED_MODE_WR_2_REGISTERr,
      DRCC_EXTENDED_MODE_WR_2_REGISTERr, DRCD_EXTENDED_MODE_WR_2_REGISTERr, DRCE_EXTENDED_MODE_WR_2_REGISTERr,
      DRCF_EXTENDED_MODE_WR_2_REGISTERr, DRCG_EXTENDED_MODE_WR_2_REGISTERr, DRCH_EXTENDED_MODE_WR_2_REGISTERr};
  static soc_reg_t
    drc_ddr_2_extended_mode_wr_3_register[] = {DRCA_DDR_2_EXTENDED_MODE_WR_3_REGISTERr, DRCB_DDR_2_EXTENDED_MODE_WR_3_REGISTERr,
      DRCC_DDR_2_EXTENDED_MODE_WR_3_REGISTERr, DRCD_DDR_2_EXTENDED_MODE_WR_3_REGISTERr, DRCE_DDR_2_EXTENDED_MODE_WR_3_REGISTERr,
      DRCF_DDR_2_EXTENDED_MODE_WR_3_REGISTERr, DRCG_DDR_2_EXTENDED_MODE_WR_3_REGISTERr, DRCH_DDR_2_EXTENDED_MODE_WR_3_REGISTERr};
  static soc_reg_t
    drc_ddr_extended_mode_register_2[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_2r, DRCB_DDR_EXTENDED_MODE_REGISTER_2r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_2r, DRCD_DDR_EXTENDED_MODE_REGISTER_2r, DRCE_DDR_EXTENDED_MODE_REGISTER_2r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_2r, DRCG_DDR_EXTENDED_MODE_REGISTER_2r, DRCH_DDR_EXTENDED_MODE_REGISTER_2r};
  static soc_reg_t
    drc_ddr_extended_mode_register_3[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_3r, DRCB_DDR_EXTENDED_MODE_REGISTER_3r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_3r, DRCD_DDR_EXTENDED_MODE_REGISTER_3r, DRCE_DDR_EXTENDED_MODE_REGISTER_3r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_3r, DRCG_DDR_EXTENDED_MODE_REGISTER_3r, DRCH_DDR_EXTENDED_MODE_REGISTER_3r};
  static soc_reg_t
    drc_ac_operating_conditions_1[] = {DRCA_AC_OPERATING_CONDITIONS_1r, DRCB_AC_OPERATING_CONDITIONS_1r,
      DRCC_AC_OPERATING_CONDITIONS_1r, DRCD_AC_OPERATING_CONDITIONS_1r, DRCE_AC_OPERATING_CONDITIONS_1r,
      DRCF_AC_OPERATING_CONDITIONS_1r, DRCG_AC_OPERATING_CONDITIONS_1r, DRCH_AC_OPERATING_CONDITIONS_1r};
  static soc_reg_t
    drc_ac_operating_conditions_2[] = {DRCA_AC_OPERATING_CONDITIONS_2r, DRCB_AC_OPERATING_CONDITIONS_2r,
      DRCC_AC_OPERATING_CONDITIONS_2r, DRCD_AC_OPERATING_CONDITIONS_2r, DRCE_AC_OPERATING_CONDITIONS_2r,
      DRCF_AC_OPERATING_CONDITIONS_2r, DRCG_AC_OPERATING_CONDITIONS_2r, DRCH_AC_OPERATING_CONDITIONS_2r};
  static soc_reg_t
    drc_ac_operating_conditions_3[] = {DRCA_AC_OPERATING_CONDITIONS_3r, DRCB_AC_OPERATING_CONDITIONS_3r,
      DRCC_AC_OPERATING_CONDITIONS_3r, DRCD_AC_OPERATING_CONDITIONS_3r, DRCE_AC_OPERATING_CONDITIONS_3r,
      DRCF_AC_OPERATING_CONDITIONS_3r, DRCG_AC_OPERATING_CONDITIONS_3r, DRCH_AC_OPERATING_CONDITIONS_3r};
  static soc_reg_t
    drc_ac_operating_conditions_4[] = {DRCA_AC_OPERATING_CONDITIONS_4r, DRCB_AC_OPERATING_CONDITIONS_4r,
      DRCC_AC_OPERATING_CONDITIONS_4r, DRCD_AC_OPERATING_CONDITIONS_4r, DRCE_AC_OPERATING_CONDITIONS_4r,
      DRCF_AC_OPERATING_CONDITIONS_4r, DRCG_AC_OPERATING_CONDITIONS_4r, DRCH_AC_OPERATING_CONDITIONS_4r};
  static soc_reg_t
    drc_init_squence_register[] = {DRCA_INIT_SEQUENCE_REGISTERr, DRCB_INIT_SEQUENCE_REGISTERr,
      DRCC_INIT_SEQUENCE_REGISTERr, DRCD_INIT_SEQUENCE_REGISTERr, DRCE_INIT_SEQUENCE_REGISTERr,
      DRCF_INIT_SEQUENCE_REGISTERr, DRCG_INIT_SEQUENCE_REGISTERr, DRCH_INIT_SEQUENCE_REGISTERr};
  static soc_reg_t
    drc_general_configurations[] = {DRCA_GENERAL_CONFIGURATIONSr, DRCB_GENERAL_CONFIGURATIONSr,
      DRCC_GENERAL_CONFIGURATIONSr, DRCD_GENERAL_CONFIGURATIONSr, DRCE_GENERAL_CONFIGURATIONSr,
      DRCF_GENERAL_CONFIGURATIONSr, DRCG_GENERAL_CONFIGURATIONSr, DRCH_GENERAL_CONFIGURATIONSr};
  static soc_reg_t
    drc_write_read_rates[] = {DRCA_WRITE_READ_RATESr, DRCB_WRITE_READ_RATESr,
      DRCC_WRITE_READ_RATESr, DRCD_WRITE_READ_RATESr, DRCE_WRITE_READ_RATESr,
      DRCF_WRITE_READ_RATESr, DRCG_WRITE_READ_RATESr, DRCH_WRITE_READ_RATESr};
  static soc_reg_t drc_dram_compliance_configuration_register[] = {
    DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};
  static soc_reg_t
    drc_odt_configuration_register[] = {DRCA_ODT_CONFIGURATION_REGISTERr, DRCB_ODT_CONFIGURATION_REGISTERr,
      DRCC_ODT_CONFIGURATION_REGISTERr, DRCD_ODT_CONFIGURATION_REGISTERr, DRCE_ODT_CONFIGURATION_REGISTERr,
      DRCF_ODT_CONFIGURATION_REGISTERr, DRCG_ODT_CONFIGURATION_REGISTERr, DRCH_ODT_CONFIGURATION_REGISTERr};
   static soc_reg_t
      drc_glue_logic_register[] = {DRCA_GLUE_LOGIC_REGISTERr, DRCB_GLUE_LOGIC_REGISTERr,
      DRCC_GLUE_LOGIC_REGISTERr, DRCD_GLUE_LOGIC_REGISTERr, DRCE_GLUE_LOGIC_REGISTERr,
      DRCF_GLUE_LOGIC_REGISTERr, DRCG_GLUE_LOGIC_REGISTERr, DRCH_GLUE_LOGIC_REGISTERr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_SET_INTERNAL_UNSAFE);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_ddr_controller_triggers[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_DDR_CONTROLLER_TRIGGERSr, &reg_val, DDR_RSTNf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_ddr_controller_triggers[dram_ndx], REG_PORT_ANY, 0, reg_val));
 
  
  reg_val = c_mr_info->mr0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_set(unit, drc_ddr_mode_register_1[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr1;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, drc_ddr_mode_register_2[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr2;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg32_set(unit, drc_ddr_extended_mode_register_1[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr3;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg32_set(unit, drc_extended_mode_wr_2_register[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr4;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg32_set(unit, drc_ddr_2_extended_mode_wr_3_register[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr5;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg32_set(unit, drc_ddr_extended_mode_register_2[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  reg_val = c_mr_info->mr6;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg32_set(unit, drc_ddr_extended_mode_register_3[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg32_get(unit, drc_ac_operating_conditions_1[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_rst;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg_val, DD_RT_RSTf, field);
  field = 0x80;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg_val, DD_RT_DLLf, field);
  field = c_info->c_rc;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_1r, &reg_val, DD_RT_RCf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, soc_reg32_set(unit, drc_ac_operating_conditions_1[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, soc_reg32_get(unit, drc_ac_operating_conditions_2[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_rrd;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg_val, DD_RT_RRDf, field);

  if (c_info->c_rfc >= (1 << 8)) {
      if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
          field = 0x1;
          soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg_val, DD_RT_RFC_MSBf, field);
          field = c_info->c_rfc % (1 << 8);
      } else {
          field = (1 << 8) - 1;
         LOG_ERROR(BSL_LS_SOC_DRAM,
                   (BSL_META_U(unit,
                               "Error in %s(): DRAM RFC out of range !!! c_rfc=0x%x. Setting DD_RT_RFC=0x%x\n"),FUNCTION_NAME(), c_info->c_rfc, field));
      }
  } else {
      field = c_info->c_rfc;
  }
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg_val, DD_RT_RFCf, field);

  field = c_info->c_rcd_rd;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg_val, DD_RT_RCDRf, field);
  field = c_info->c_rcd_wr;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_2r, &reg_val, DD_RT_RCDWf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, soc_reg32_set(unit, drc_ac_operating_conditions_2[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  
  
  
  reg_val = 0x3ff ;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, soc_reg32_set(unit, drc_init_squence_register[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, soc_reg32_get(unit, drc_ac_operating_conditions_3[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_ras_rd;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg_val, CNT_RASRDPRDf, field);
  field = c_info->c_ras_wr;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg_val, CNT_RASWRPRDf, field);
  field = c_info->c_rd_ap;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg_val, CNT_RDAPPRDf, field);
  field = c_info->c_wr_ap;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_3r, &reg_val, CNT_WRAPPRDf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, soc_reg32_set(unit, drc_ac_operating_conditions_3[dram_ndx], REG_PORT_ANY, 0, reg_val));


  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, soc_reg32_get(unit, drc_general_configurations[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->nof_cols;
  soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg_val, NUM_COLSf, field);
  field = c_info->ap_bit_pos;
  soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg_val, AP_BIT_POSf, field);
  field = arad_dram_internal_refresh_delay_period_get(unit, dram_freq);
  soc_reg_field_set(unit, DRCA_GENERAL_CONFIGURATIONSr, &reg_val, REFRESH_DELAY_PRDf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 180, exit, soc_reg32_set(unit, drc_general_configurations[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, soc_reg32_get(unit, drc_write_read_rates[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = 0x40;
  soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg_val, DD_RT_ZQCSf, field);
  field = c_info->c_faw;
  soc_reg_field_set(unit, DRCA_WRITE_READ_RATESr, &reg_val, DD_RT_FAWf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, soc_reg32_set(unit, drc_write_read_rates[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 210, exit, soc_reg32_get(unit, drc_dram_compliance_configuration_register[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_wr_latency;
  soc_reg_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg_val, WR_LATENCYf, field);
  field = (c_info->burst_size == ARAD_DRAM_BURST_SIZE_16);
  soc_reg_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg_val, BURST_SIZE_MODEf, field);
  field = dram_type;
  soc_reg_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg_val, DRAM_TYPEf, field);
  field = (c_info->nof_banks == ARAD_DRAM_NUM_BANKS_8);
  soc_reg_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg_val, ENABLE_8_BANKSf, field);
  field = 0x0;
  soc_reg_field_set(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, &reg_val, STATIC_ODT_ENf, field);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 220, exit, soc_reg32_set(unit, drc_dram_compliance_configuration_register[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, soc_reg32_get(unit, drc_ac_operating_conditions_4[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_ref;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg_val, DD_RT_REFIf, field);
  field = c_info->c_wr_rd;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg_val, CNT_WRRDPRDf, field);
  field = c_info->c_rd_wr;
  soc_reg_field_set(unit, DRCA_AC_OPERATING_CONDITIONS_4r, &reg_val, CNT_RDWRPRDf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 240, exit, soc_reg32_set(unit, drc_ac_operating_conditions_4[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 250, exit, soc_reg32_get(unit, drc_glue_logic_register[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  field = c_info->c_cas_latency - 2;
  soc_reg_field_set(unit, DRCA_GLUE_LOGIC_REGISTERr, &reg_val, READ_DELAYf, field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 260, exit, soc_reg32_set(unit, drc_glue_logic_register[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 270, exit, soc_reg32_get(unit, drc_odt_configuration_register[dram_ndx], REG_PORT_ANY, 0, &reg_val));

  
    
  
  field = 0x0;
  soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg_val, DYN_ODT_START_DELAYf, field);
  field = 0x6;
  soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg_val, DYN_ODT_LENGTHf, field);
  field = 0x0;
  soc_reg_field_set(unit, DRCA_ODT_CONFIGURATION_REGISTERr, &reg_val, DDR_3_ZQ_CALIB_GEN_PRDf,  field);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 280, exit, soc_reg32_set(unit, drc_odt_configuration_register[dram_ndx], REG_PORT_ANY, 0, reg_val));

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 290, exit, soc_reg32_get(unit, drc_ddr_controller_triggers[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_DDR_CONTROLLER_TRIGGERSr, &reg_val, DDR_RSTNf, 0x1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 300, exit, soc_reg32_set(unit, drc_ddr_controller_triggers[dram_ndx], REG_PORT_ANY, 0, reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_set_internal_unsafe()",0,0);
}

STATIC uint32
  arad_dram_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO(
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_INFO           *t_info,
    SOC_SAND_OUT ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    value,
    calc1;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_DRAM_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO);

  SOC_SAND_CHECK_NULL_INPUT(t_info);
  SOC_SAND_CHECK_NULL_INPUT(c_info);

  ARAD_CLEAR(c_info, ARAD_DRAM_INTERNAL_INFO, 1);

  value = 1;
  c_info->c_pcb = value;

  value = t_info->burst_size;
  c_info->burst_size = value;

  value = t_info->nof_cols;
  c_info->nof_cols = value;

  value = t_info->ap_bit_pos;
  c_info->ap_bit_pos = value;

  value = t_info->nof_banks;
  c_info->nof_banks = value;

  value = t_info->c_cas_latency;
  c_info->c_cas_latency = value;

  value = t_info->c_wr_latency;
  c_info->c_wr_latency = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rc);
  c_info->c_rc = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rfc);
  c_info->c_rfc = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_ras);
  c_info->c_ras = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_faw);
  c_info->c_faw = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rcd_rd);
  c_info->c_rcd_rd = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rcd_wr);
  c_info->c_rcd_wr = value;

  value = SOC_SAND_MAX(ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rrd), 4);
  c_info->c_rrd = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS_DOWN(t_info->t_ref);
  c_info->c_ref = value/32;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rp);
  c_info->c_rp = value;

  value = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_wr);
  c_info->c_wr = value;

  value = SOC_SAND_MAX(ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_wtr), 4);
  c_info->c_wtr = value;

  value = SOC_SAND_MAX(ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rtp), 4);
  c_info->c_rtp = value;

  value = 520 * dram_freq;
  c_info->c_rst = value/32;

  value = c_info->c_ras - c_info->burst_size / 2 < 1 ? 1 : c_info->c_ras - c_info->burst_size / 2;
  c_info->c_ras_rd = value;

  
  if (!(ARAD_DRAM_IS_VAL_IN_CLOCKS(t_info->t_ras) || ARAD_DRAM_IS_VAL_IN_CLOCKS(t_info->t_wr)))
  {
    calc1 = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_ras - t_info->t_wr);
  }
  else
  {
    calc1 = ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_ras) - ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_wr);
  }
  
  value = (calc1 - (c_info->burst_size / 2) - c_info->c_wr_latency < 1 ? 1 : calc1 - (c_info->burst_size / 2) - c_info->c_wr_latency);
  c_info->c_ras_wr = value;

  value = c_info->c_rp + SOC_SAND_MAX(c_info->c_rtp, 2);
  c_info->c_rd_ap = value;

  value = (c_info->burst_size / 2) + c_info->c_rp + c_info->c_wr + c_info->c_wr_latency;
  c_info->c_wr_ap = value;

  value = (c_info->burst_size / 2) + c_info->c_wtr + c_info->c_wr_latency;
  c_info->c_wr_rd = value;

  value = t_info->c_cas_latency + c_info->c_pcb + 1;
  c_info->c_rd_wr = value;

  value = c_info->c_rfc + ARAD_DRAM_VAL_TO_CLOCKS(10000 * ARAD_DRAM_FREQ_MULTIPLIER);
  c_info->c_init_wait_prd = value;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO()",0,0);
}

STATIC uint32
  arad_dram_ARAD_DRAM_MR_INFO_2_ARAD_DRAM_INTERNAL_MR_INFO(
    SOC_SAND_IN  ARAD_DRAM_TYPE              dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO              *t_info,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO     *c_info,
    SOC_SAND_OUT ARAD_DRAM_INTERNAL_MR_INFO  *c_mr_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_DRAM_MODE_REGS
    mode_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_DRAM_ARAD_DRAM_MR_INFO_2_ARAD_DRAM_INTERNAL_MR_INFO);

  SOC_SAND_CHECK_NULL_INPUT(c_info);
  SOC_SAND_CHECK_NULL_INPUT(c_mr_info);

  ARAD_CLEAR(c_mr_info, ARAD_DRAM_INTERNAL_MR_INFO, 1);

  res = arad_dram_set_mode_regs(
          dram_type,
          &mode_regs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(dram_type)
  {
    case ARAD_DRAM_TYPE_DDR3:
    {
      if (t_info->auto_mode)
      {
        c_mr_info->mr0 = arad_dram_ddr3_mrs0_get(&mode_regs.ddr3, c_info) | SOC_SAND_SET_BITS_RANGE(0x1, ARAD_DRAM_MR0_DLL_MSB, ARAD_DRAM_MR0_DLL_LSB);
        c_mr_info->mr1 = arad_dram_ddr3_mrs0_get(&mode_regs.ddr3, c_info) | SOC_SAND_SET_BITS_RANGE(0x0, ARAD_DRAM_MR0_DLL_MSB, ARAD_DRAM_MR0_DLL_LSB);
        c_mr_info->mr2 = arad_dram_ddr3_mrs1_get(&mode_regs.ddr3, c_info);
        c_mr_info->mr3 = 0x0;
        c_mr_info->mr4 = 0x0;
        c_mr_info->mr5 = arad_dram_ddr3_mrs2_get(&mode_regs.ddr3, c_info);
        c_mr_info->mr6 = arad_dram_ddr3_mrs3_get(&mode_regs.ddr3, c_info);
      }
      else
      {
        c_mr_info->mr0 = t_info->mode_regs.ddr3.mrs0_wr1;
        c_mr_info->mr1 = t_info->mode_regs.ddr3.mrs0_wr2;
        c_mr_info->mr2 = t_info->mode_regs.ddr3.mrs1_wr1;
        c_mr_info->mr3 = 0x0;
        c_mr_info->mr4 = 0x0;
        c_mr_info->mr5 = t_info->mode_regs.ddr3.mrs2_wr1;
        c_mr_info->mr6 = t_info->mode_regs.ddr3.mrs3_wr1;
      }
      break;
    }
  default:
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_INVALID_DRAM_TYPE_ERR, 40, exit)
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_ARAD_DRAM_MR_INFO_2_ARAD_DRAM_INTERNAL_MR_INFO()",0,0);
}


uint32 arad_dram_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *t_info)
{
    uint32
        res = SOC_SAND_OK;
    ARAD_DRAM_INTERNAL_INFO
        c_info, c_info_modified;
    ARAD_DRAM_INTERNAL_MR_INFO
        c_mr_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_SET_UNSAFE);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_CHECK_NULL_INPUT(t_info);

    SOC_SAND_ERR_IF_ABOVE_MAX(dram_ndx, ARAD_DRAM_NDX_MAX, ARAD_DIAG_DRAM_NDX_OUT_OF_RANGE_ERR, 10, exit);

    ARAD_CLEAR(&c_info, ARAD_DRAM_INTERNAL_INFO, 1);
    ARAD_CLEAR(&c_info_modified, ARAD_DRAM_INTERNAL_INFO, 1);
    ARAD_CLEAR(&c_mr_info, ARAD_DRAM_INTERNAL_MR_INFO, 1);

    
    res = arad_dram_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO(dram_freq, t_info, &c_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    
    res = arad_dram_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO(dram_freq + ARAD_DRAM_FREQ_INCREMENT, t_info, &c_info_modified);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    
    c_info_modified.c_ref = c_info.c_ref;

    
    res = arad_dram_ARAD_DRAM_MR_INFO_2_ARAD_DRAM_INTERNAL_MR_INFO(dram_type, t_info, &c_info, &c_mr_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_dram_info_set_internal_unsafe(unit, dram_ndx, dram_freq, dram_type, &c_info_modified, &c_mr_info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_set_unsafe()", dram_ndx, dram_freq);
}


STATIC uint32
  arad_dram_info_ddr3_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_INTERNAL_INFO  *c_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_wr_latency, ARAD_DRAM_DDR3_WL_MIN, ARAD_DRAM_DDR3_WL_MAX, ARAD_DRAM_WL_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_cas_latency, ARAD_DRAM_DDR3_CL_MIN, ARAD_DRAM_DDR3_CL_MAX, ARAD_DRAM_CL_OUT_OF_RANGE_ERR, 20, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(c_info->c_wr, ARAD_DRAM_DDR3_WR_MIN, ARAD_DRAM_DDR3_WR_MAX, ARAD_DRAM_WR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_EQUALS_VALUE(c_info->burst_size, ARAD_DRAM_BURST_SIZE_16, ARAD_DRAM_BL_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_ddr3_verify()",0,0);
}


uint32
  arad_dram_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_freq,
    SOC_SAND_IN  ARAD_DRAM_TYPE           dram_type,
    SOC_SAND_IN  ARAD_DRAM_INFO           *t_info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_DRAM_INTERNAL_INFO
    c_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_VERIFY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(t_info);

  ARAD_CLEAR(&c_info, ARAD_DRAM_INTERNAL_INFO, 1);

  
  res = arad_dram_ARAD_DRAM_INFO_2_ARAD_DRAM_INTERNAL_INFO(
          dram_freq,
          t_info,
          &c_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  switch(dram_type)
  {
   case ARAD_DRAM_TYPE_DDR3:
    {
      res = arad_dram_info_ddr3_verify(
            unit,
            dram_freq,
            &c_info
          );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit)
      break;
    }
   default:
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_INVALID_DRAM_TYPE_ERR, 40, exit)
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_verify()",0,0);
}


uint32
  arad_dram_info_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   dram_ndx,
    SOC_SAND_IN  uint32                   dram_freq,
    SOC_SAND_OUT ARAD_DRAM_TYPE           *dram_type,
    SOC_SAND_OUT ARAD_DRAM_INFO           *t_info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val;
  uint32
    c_pcb = 1;
  static soc_reg_t drc_dram_compliance_configuration_register[] = {DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCB_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCC_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCD_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCE_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCF_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, DRCG_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr,
    DRCH_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr};
  static soc_reg_t
    drc_ddr_mode_register_1[] = {DRCA_DDR_MODE_REGISTER_1r, DRCB_DDR_MODE_REGISTER_1r,
      DRCC_DDR_MODE_REGISTER_1r, DRCD_DDR_MODE_REGISTER_1r, DRCE_DDR_MODE_REGISTER_1r,
      DRCF_DDR_MODE_REGISTER_1r, DRCG_DDR_MODE_REGISTER_1r, DRCH_DDR_MODE_REGISTER_1r};
  static soc_reg_t
    drc_ddr_mode_register_2[] = {DRCA_DDR_MODE_REGISTER_2r, DRCB_DDR_MODE_REGISTER_2r,
      DRCC_DDR_MODE_REGISTER_2r, DRCD_DDR_MODE_REGISTER_2r, DRCE_DDR_MODE_REGISTER_2r,
      DRCF_DDR_MODE_REGISTER_2r, DRCG_DDR_MODE_REGISTER_2r, DRCH_DDR_MODE_REGISTER_2r};
  static soc_reg_t
    drc_ddr_extended_mode_register_1[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_1r, DRCB_DDR_EXTENDED_MODE_REGISTER_1r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_1r, DRCD_DDR_EXTENDED_MODE_REGISTER_1r, DRCE_DDR_EXTENDED_MODE_REGISTER_1r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_1r, DRCG_DDR_EXTENDED_MODE_REGISTER_1r, DRCH_DDR_EXTENDED_MODE_REGISTER_1r};
  static soc_reg_t
    drc_ddr_extended_mode_register_2[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_2r, DRCB_DDR_EXTENDED_MODE_REGISTER_2r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_2r, DRCD_DDR_EXTENDED_MODE_REGISTER_2r, DRCE_DDR_EXTENDED_MODE_REGISTER_2r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_2r, DRCG_DDR_EXTENDED_MODE_REGISTER_2r, DRCH_DDR_EXTENDED_MODE_REGISTER_2r};
  static soc_reg_t
    drc_ddr_extended_mode_register_3[] = {DRCA_DDR_EXTENDED_MODE_REGISTER_3r, DRCB_DDR_EXTENDED_MODE_REGISTER_3r,
      DRCC_DDR_EXTENDED_MODE_REGISTER_3r, DRCD_DDR_EXTENDED_MODE_REGISTER_3r, DRCE_DDR_EXTENDED_MODE_REGISTER_3r,
      DRCF_DDR_EXTENDED_MODE_REGISTER_3r, DRCG_DDR_EXTENDED_MODE_REGISTER_3r, DRCH_DDR_EXTENDED_MODE_REGISTER_3r};
  static soc_reg_t
    drc_ac_operating_conditions_1[] = {DRCA_AC_OPERATING_CONDITIONS_1r, DRCB_AC_OPERATING_CONDITIONS_1r,
      DRCC_AC_OPERATING_CONDITIONS_1r, DRCD_AC_OPERATING_CONDITIONS_1r, DRCE_AC_OPERATING_CONDITIONS_1r,
      DRCF_AC_OPERATING_CONDITIONS_1r, DRCG_AC_OPERATING_CONDITIONS_1r, DRCH_AC_OPERATING_CONDITIONS_1r};
  static soc_reg_t
    drc_ac_operating_conditions_2[] = {DRCA_AC_OPERATING_CONDITIONS_2r, DRCB_AC_OPERATING_CONDITIONS_2r,
      DRCC_AC_OPERATING_CONDITIONS_2r, DRCD_AC_OPERATING_CONDITIONS_2r, DRCE_AC_OPERATING_CONDITIONS_2r,
      DRCF_AC_OPERATING_CONDITIONS_2r, DRCG_AC_OPERATING_CONDITIONS_2r, DRCH_AC_OPERATING_CONDITIONS_2r};
  static soc_reg_t
    drc_ac_operating_conditions_3[] = {DRCA_AC_OPERATING_CONDITIONS_3r, DRCB_AC_OPERATING_CONDITIONS_3r,
      DRCC_AC_OPERATING_CONDITIONS_3r, DRCD_AC_OPERATING_CONDITIONS_3r, DRCE_AC_OPERATING_CONDITIONS_3r,
      DRCF_AC_OPERATING_CONDITIONS_3r, DRCG_AC_OPERATING_CONDITIONS_3r, DRCH_AC_OPERATING_CONDITIONS_3r};
  static soc_reg_t
    drc_ac_operating_conditions_4[] = {DRCA_AC_OPERATING_CONDITIONS_4r, DRCB_AC_OPERATING_CONDITIONS_4r,
      DRCC_AC_OPERATING_CONDITIONS_4r, DRCD_AC_OPERATING_CONDITIONS_4r, DRCE_AC_OPERATING_CONDITIONS_4r,
      DRCF_AC_OPERATING_CONDITIONS_4r, DRCG_AC_OPERATING_CONDITIONS_4r, DRCH_AC_OPERATING_CONDITIONS_4r};
  static soc_reg_t
    drc_write_read_rates[] = {DRCA_WRITE_READ_RATESr, DRCB_WRITE_READ_RATESr,
      DRCC_WRITE_READ_RATESr, DRCD_WRITE_READ_RATESr, DRCE_WRITE_READ_RATESr,
      DRCF_WRITE_READ_RATESr, DRCG_WRITE_READ_RATESr, DRCH_WRITE_READ_RATESr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(t_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_dram_compliance_configuration_register[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *dram_type = soc_reg_field_get( unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg_val, DRAM_TYPEf);

  t_info->auto_mode = FALSE;

  if (*dram_type == ARAD_DRAM_TYPE_DDR3)
  {
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg32_get(unit, drc_ddr_mode_register_1[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    t_info->mode_regs.ddr3.mrs0_wr1 = reg_val;
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg32_get(unit, drc_ddr_mode_register_2[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    t_info->mode_regs.ddr3.mrs0_wr2 = reg_val;
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, soc_reg32_get(unit, drc_ddr_extended_mode_register_1[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    t_info->mode_regs.ddr3.mrs1_wr1 = reg_val;
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, soc_reg32_get(unit, drc_ddr_extended_mode_register_2[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    t_info->mode_regs.ddr3.mrs2_wr1 = reg_val;
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, soc_reg32_get(unit, drc_ddr_extended_mode_register_3[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    t_info->mode_regs.ddr3.mrs3_wr1 = reg_val;
  }
 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, soc_reg32_get(unit, drc_dram_compliance_configuration_register[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->burst_size = soc_reg_field_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg_val, BURST_SIZE_MODEf) ? ARAD_DRAM_BURST_SIZE_16 : ARAD_DRAM_BURST_SIZE_32;
  t_info->nof_banks = soc_reg_field_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg_val, ENABLE_8_BANKSf) ? ARAD_DRAM_NUM_BANKS_8 : ARAD_DRAM_NUM_BANKS_4;

  t_info->c_wr_latency = soc_reg_field_get(unit, DRCA_DRAM_COMPLIANCE_CONFIGURATION_REGISTERr, reg_val, WR_LATENCYf);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, soc_reg32_get(unit, drc_ac_operating_conditions_4[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->c_cas_latency = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg_val, CNT_RDWRPRDf) - c_pcb - 1;
  t_info->t_ref = ARAD_DRAM_CLOCKS_TO_VAL(32 * soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg_val, DD_RT_REFIf));
  t_info->t_wtr = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_4r, reg_val, CNT_WRRDPRDf) - (t_info->burst_size / 2) - t_info->c_wr_latency);

  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 180, exit, soc_reg32_get(unit, drc_ac_operating_conditions_1[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->t_rc = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_1r, reg_val, DD_RT_RCf);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, soc_reg32_get(unit, drc_ac_operating_conditions_2[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->t_rfc = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, reg_val, DD_RT_RFCf));
  t_info->t_rcd_rd = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, reg_val, DD_RT_RCDRf));
  t_info->t_rcd_wr = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, reg_val, DD_RT_RCDWf));
  t_info->t_rrd = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, reg_val, DD_RT_RRDf));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, soc_reg32_get(unit, drc_ac_operating_conditions_3[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->t_ras = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg_val, CNT_RASRDPRDf) + t_info->burst_size / 2;
  t_info->t_wr = ARAD_DRAM_CLOCKS_TO_VAL(ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_ras) + (t_info->burst_size / 2) + t_info->c_wr_latency - soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg_val, CNT_RASWRPRDf));
  t_info->t_rp = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg_val, CNT_WRAPPRDf) - (t_info->burst_size / 2) - ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_wr) - t_info->c_wr_latency);
  t_info->t_rtp = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, reg_val, CNT_RDAPPRDf) - (t_info->burst_size / 2) - ARAD_DRAM_VAL_TO_CLOCKS(t_info->t_rp));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 210, exit, soc_reg32_get(unit, drc_write_read_rates[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  t_info->t_faw = ARAD_DRAM_CLOCKS_TO_VAL(soc_reg_field_get(unit, DRCA_WRITE_READ_RATESr, reg_val, DD_RT_FAWf));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_info_get_unsafe()",0,0);
}


uint32 arad_dram_init_drc_phy_register_set(int unit, ARAD_DRAM_TYPE dram_type, uint32 dram_freq, ARAD_DRAM_INFO t_info)
{
    uint32 res = SOC_SAND_OK;
    uint32 reg_val;
    uint32 field;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DRC_PHY_REGISTER_SET);  
  
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_DRCA_DRAM_SETr(unit, &reg_val));
    field = t_info.jedec;
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, JEDECf, 27);

    field = (dram_type == ARAD_DRAM_TYPE_DDR3) ? 0x1 : 0x0;
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, DDR_3_TYPEf, field);
    field = dram_freq;
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, DDR_MHZf, field);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, CHIP_SIZEf, 0x2);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, CHIP_WIDTHf, 0x1);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, BUS_8f, 0x0);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, BUS_16f, 0x0);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, AD_WIDTHf, 0x2);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, VOLTSf, 0x0);
    soc_reg_field_set(unit, DRCA_DRAM_SETr, &reg_val, DUAL_RANKf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_DRCBROADCAST_DRAM_SETr(unit, reg_val));

     
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_DRCA_DRAM_TIME_PARAMSr(unit, &reg_val));
    field = t_info.c_cas_latency;
    soc_reg_field_set(unit, DRCA_DRAM_TIME_PARAMSr, &reg_val, CLf, field);
    field = t_info.c_wr_latency;
    soc_reg_field_set(unit, DRCA_DRAM_TIME_PARAMSr, &reg_val, CWLf, field);
    field = ARAD_DRAM_VAL_TO_CLOCKS(t_info.t_wr);
    soc_reg_field_set(unit, DRCA_DRAM_TIME_PARAMSr, &reg_val, WRf, field);
    soc_reg_field_set(unit, DRCA_DRAM_TIME_PARAMSr, &reg_val, ALf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_DRCBROADCAST_DRAM_TIME_PARAMSr(unit, reg_val));

     
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_DRCA_DPI_POWERr(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, IDLEf, 0x0);
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, STANDBYf, 0x0);
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, PLL_PWRDNf, 0x0);
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, BYPASS_PLLf, 0x0);
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, IDDQ_ENABLEf, 0x0);
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, CK_DISf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_DRCBROADCAST_DPI_POWERr(unit, reg_val));

     
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_DRCA_VDL_CNTRLr(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_VDL_CNTRLr, &reg_val, UPDT_VDLf, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_DRCBROADCAST_VDL_CNTRLr(unit, reg_val));

     
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_DRCA_DPI_STAT_CNTRLr(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_DPI_STAT_CNTRLr, &reg_val, FIELD_0_0f, 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_DRCBROADCAST_DPI_STAT_CNTRLr(unit, reg_val));

   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): Finish DRC PHY register set.\n"),FUNCTION_NAME()));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_drc_phy_register_set()", dram_type, dram_freq);
}



uint32 arad_dram_rbus_write( 
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint32 addr,
    SOC_SAND_IN uint32 data)
{
  uint32
    res;
  uint32
    reg_val;
  const static soc_reg_t drc_rbus_rd_result[] = {DRCA_RBUS_RD_RESULTr, DRCB_RBUS_RD_RESULTr,
    DRCC_RBUS_RD_RESULTr, DRCD_RBUS_RD_RESULTr, DRCE_RBUS_RD_RESULTr,
    DRCF_RBUS_RD_RESULTr, DRCG_RBUS_RD_RESULTr, DRCH_RBUS_RD_RESULTr};
  const static soc_reg_t drc_rbus_wdata[] = {DRCA_RBUS_WDATAr, DRCB_RBUS_WDATAr,
    DRCC_RBUS_WDATAr, DRCD_RBUS_WDATAr, DRCE_RBUS_WDATAr,
    DRCF_RBUS_WDATAr, DRCG_RBUS_WDATAr, DRCH_RBUS_WDATAr};
  const static soc_reg_t drc_rbus_addr[] = {DRCA_RBUS_ADDRr, DRCB_RBUS_ADDRr,
    DRCC_RBUS_ADDRr, DRCD_RBUS_ADDRr, DRCE_RBUS_ADDRr,
    DRCF_RBUS_ADDRr, DRCG_RBUS_ADDRr, DRCH_RBUS_ADDRr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_RUBS_WRITE);

  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): dram_ndx=%d, addr=0x%x. data=0x%x\n"),FUNCTION_NAME(), dram_ndx, addr, data));

  
  if (dram_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
      LOG_ERROR(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): Dram index is above max in device(%d)\n"), FUNCTION_NAME(), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
  }
  reg_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_rbus_rd_result[dram_ndx], REG_PORT_ANY, 0, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_WDATAr, &reg_val, RBUS_WDATAf, data);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, drc_rbus_wdata[dram_ndx], REG_PORT_ANY, 0, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_ADDRf, addr);
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_RD_ACTf, 0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg32_set(unit, drc_rbus_addr[dram_ndx], REG_PORT_ANY, 0, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_RD_RESULTr, &reg_val, RBUS_TRGf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg32_set(unit, drc_rbus_rd_result[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          drc_rbus_rd_result[dram_ndx],
          REG_PORT_ANY,
          0,
          RBUS_ACKf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_rbus_write()", addr, data);
}

uint32 arad_dram_rbus_write_br(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 last_dram_ndx,
    SOC_SAND_IN uint32 addr,
    SOC_SAND_IN uint32 data)
{
  uint32
    res;
  uint32
    reg_val;
  const static soc_reg_t drc_rbus_rd_result[] = {DRCA_RBUS_RD_RESULTr, DRCB_RBUS_RD_RESULTr,
    DRCC_RBUS_RD_RESULTr, DRCD_RBUS_RD_RESULTr, DRCE_RBUS_RD_RESULTr,
    DRCF_RBUS_RD_RESULTr, DRCG_RBUS_RD_RESULTr, DRCH_RBUS_RD_RESULTr};
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_RUBS_WRITE_BR);

 LOG_DEBUG(BSL_LS_SOC_DRAM,
           (BSL_META_U(unit,
                       "%s(): last_dram_ndx=%d, addr=0x%x. data=0x%x\n"),FUNCTION_NAME(), last_dram_ndx, addr, data));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_RD_RESULTr, &reg_val, RBUS_TRGf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_DRCBROADCAST_RBUS_RD_RESULTr(unit, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_WDATAr, &reg_val, RBUS_WDATAf, data);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_DRCBROADCAST_RBUS_WDATAr(unit, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_ADDRf, addr);
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_RD_ACTf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_DRCBROADCAST_RBUS_ADDRr(unit, reg_val));

  reg_val =0x0;
  soc_reg_field_set(unit, DRCA_RBUS_RD_RESULTr, &reg_val, RBUS_TRGf, 0x1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, WRITE_DRCBROADCAST_RBUS_RD_RESULTr(unit, reg_val));
  
  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          drc_rbus_rd_result[last_dram_ndx],
          REG_PORT_ANY,
          0,
          RBUS_ACKf,
          1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_rbus_write_br()", addr, data);
}

uint32 arad_dram_rbus_read(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_IN  uint32 addr,
    SOC_SAND_OUT uint32 *data)
{
  uint32
    res;
  uint32
    reg_val;
  uint32
    rbus_err;
  const static soc_reg_t drc_rbus_rd_result[] = {DRCA_RBUS_RD_RESULTr, DRCB_RBUS_RD_RESULTr,
    DRCC_RBUS_RD_RESULTr, DRCD_RBUS_RD_RESULTr, DRCE_RBUS_RD_RESULTr,
    DRCF_RBUS_RD_RESULTr, DRCG_RBUS_RD_RESULTr, DRCH_RBUS_RD_RESULTr};
  const static soc_reg_t drc_rbus_addr[] = {DRCA_RBUS_ADDRr, DRCB_RBUS_ADDRr,
    DRCC_RBUS_ADDRr, DRCD_RBUS_ADDRr, DRCE_RBUS_ADDRr,
    DRCF_RBUS_ADDRr, DRCG_RBUS_ADDRr, DRCH_RBUS_ADDRr};
  const static soc_reg_t drc_rbus_rdata[] = {DRCA_RBUS_RDATAr, DRCB_RBUS_RDATAr,
    DRCC_RBUS_RDATAr, DRCD_RBUS_RDATAr, DRCE_RBUS_RDATAr,
    DRCF_RBUS_RDATAr, DRCG_RBUS_RDATAr, DRCH_RBUS_RDATAr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_RUBS_READ);

  SOC_SAND_CHECK_NULL_INPUT(data);

  
  if (dram_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
      LOG_ERROR(BSL_LS_SOC_DRAM,
                (BSL_META_U(unit,
                            "%s(): Dram index is above max in device(%d)\n"), FUNCTION_NAME(), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
  }

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_RD_RESULTr, &reg_val, RBUS_TRGf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_rbus_rd_result[dram_ndx], REG_PORT_ANY, 0, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_ADDRf, addr);
  soc_reg_field_set(unit, DRCA_RBUS_ADDRr, &reg_val, RBUS_RD_ACTf, 0x1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, drc_rbus_addr[dram_ndx], REG_PORT_ANY, 0, reg_val));

  reg_val = 0x0;
  soc_reg_field_set(unit, DRCA_RBUS_RD_RESULTr, &reg_val, RBUS_TRGf, 1);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg32_set(unit, drc_rbus_rd_result[dram_ndx], REG_PORT_ANY, 0, reg_val));

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          drc_rbus_rd_result[dram_ndx],
          REG_PORT_ANY,
          0,
          RBUS_ACKf,
          0x1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg32_get(unit, drc_rbus_rdata[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *data = soc_reg_field_get(unit, DRCA_RBUS_RDATAr, reg_val, RBUS_RDATAf);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg32_get(unit, drc_rbus_rd_result[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  rbus_err = soc_reg_field_get(unit, DRCA_RBUS_RD_RESULTr, reg_val, RBUS_ERRf);
  if (rbus_err) {
     LOG_ERROR(BSL_LS_SOC_DRAM,
               (BSL_META_U(unit,
                           "%s(): Rbus ERROR: dram_ndx=%d, rbus_err=0x%x, addr=0x%x. *data=0x%x\n"),FUNCTION_NAME(), dram_ndx, rbus_err, addr, *data));
      SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_RBUS_READ_ERR, 100, exit);
  }

 LOG_DEBUG(BSL_LS_SOC_DRAM,
           (BSL_META_U(unit,
                       "%s(): dram_ndx=%d, rbus_err=0x%x, addr=0x%x. *data=0x%x\n"),FUNCTION_NAME(), dram_ndx, rbus_err, addr, *data));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_rbus_read()", dram_ndx, addr);
}

uint32 arad_dram_rbus_modify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_IN  uint32 addr,
    SOC_SAND_IN  uint32 data,
    SOC_SAND_IN  uint32 mask)
{
    uint32
        res;
    uint32 
        tmp, otmp;
    uint32
        masked_data;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_RUBS_MODIFY);

   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): dram_ndx=%d, addr=0x%x. data=0x%x mask=0x%x\n"),FUNCTION_NAME(), dram_ndx, addr, data, mask));

    
    if (dram_ndx >= SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        LOG_ERROR(BSL_LS_SOC_DRAM,
                  (BSL_META_U(unit,
                              "%s(): Dram index is above max in device(%d)\n"), FUNCTION_NAME(), SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)));
    }
    masked_data = data & mask;

    res = arad_dram_rbus_read(unit, dram_ndx, addr, &tmp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    otmp = tmp;
    tmp &= ~(mask);
    tmp |= masked_data;

    if (otmp != tmp) {
        res = arad_dram_rbus_write(unit, dram_ndx, addr, tmp);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_rbus_modify()", addr, data);

}


uint32
  arad_dram_configure_bist(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint32 write_weight,
    SOC_SAND_IN uint32 read_weight,
    SOC_SAND_IN uint32 pattern_bit_mode,
    SOC_SAND_IN uint32 two_addr_mode,
    SOC_SAND_IN uint32 prbs_mode,
    SOC_SAND_IN uint32 cons_addr_8_banks,
    SOC_SAND_IN uint32 address_shift_mode,
    SOC_SAND_IN uint32 data_shift_mode,
    SOC_SAND_IN uint32 data_addr_mode,
    SOC_SAND_IN uint32 bist_num_actions,
    SOC_SAND_IN uint32 bist_start_address,
    SOC_SAND_IN uint32 bist_end_address,
    SOC_SAND_IN uint32 bist_pattern[ARAD_DRAM_BIST_NOF_PATTERNS]
  )
{
  uint32
    reg_val,
    i,
    res;
  const static soc_reg_t drc_bist_configurations[] = {DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr,
    DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr,
    DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr};
  const static soc_reg_t drc_bist_number_of_actions[] = {DRCA_BIST_NUMBER_OF_ACTIONSr, DRCB_BIST_NUMBER_OF_ACTIONSr, DRCC_BIST_NUMBER_OF_ACTIONSr,
    DRCD_BIST_NUMBER_OF_ACTIONSr, DRCE_BIST_NUMBER_OF_ACTIONSr, DRCF_BIST_NUMBER_OF_ACTIONSr,
    DRCG_BIST_NUMBER_OF_ACTIONSr, DRCH_BIST_NUMBER_OF_ACTIONSr};
  const static soc_reg_t drc_bist_start_address[] = {DRCA_BIST_START_ADDRESSr, DRCB_BIST_START_ADDRESSr, DRCC_BIST_START_ADDRESSr,
    DRCD_BIST_START_ADDRESSr, DRCE_BIST_START_ADDRESSr, DRCF_BIST_START_ADDRESSr,
    DRCG_BIST_START_ADDRESSr, DRCH_BIST_START_ADDRESSr};
  const static soc_reg_t drc_bist_end_address[] = {DRCA_BIST_END_ADDRESSr, DRCB_BIST_END_ADDRESSr, DRCC_BIST_END_ADDRESSr,
    DRCD_BIST_END_ADDRESSr, DRCE_BIST_END_ADDRESSr, DRCF_BIST_END_ADDRESSr,
    DRCG_BIST_END_ADDRESSr, DRCH_BIST_END_ADDRESSr};
  const static soc_reg_t drc_bist_pattern_words_regs[ARAD_DRAM_BIST_NOF_PATTERNS][SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)] = {{DRCA_BIST_PATTERN_WORD_0r, DRCB_BIST_PATTERN_WORD_0r, DRCC_BIST_PATTERN_WORD_0r,
      DRCD_BIST_PATTERN_WORD_0r, DRCE_BIST_PATTERN_WORD_0r, DRCF_BIST_PATTERN_WORD_0r,
      DRCG_BIST_PATTERN_WORD_0r, DRCH_BIST_PATTERN_WORD_0r},
    {DRCA_BIST_PATTERN_WORD_1r, DRCB_BIST_PATTERN_WORD_1r, DRCC_BIST_PATTERN_WORD_1r,
      DRCD_BIST_PATTERN_WORD_1r, DRCE_BIST_PATTERN_WORD_1r, DRCF_BIST_PATTERN_WORD_1r,
      DRCG_BIST_PATTERN_WORD_1r, DRCH_BIST_PATTERN_WORD_1r},
    {DRCA_BIST_PATTERN_WORD_2r, DRCB_BIST_PATTERN_WORD_2r, DRCC_BIST_PATTERN_WORD_2r,
      DRCD_BIST_PATTERN_WORD_2r, DRCE_BIST_PATTERN_WORD_2r, DRCF_BIST_PATTERN_WORD_2r,
      DRCG_BIST_PATTERN_WORD_2r, DRCH_BIST_PATTERN_WORD_2r},
    {DRCA_BIST_PATTERN_WORD_3r, DRCB_BIST_PATTERN_WORD_3r, DRCC_BIST_PATTERN_WORD_3r,
      DRCD_BIST_PATTERN_WORD_3r, DRCE_BIST_PATTERN_WORD_3r, DRCF_BIST_PATTERN_WORD_3r,
      DRCG_BIST_PATTERN_WORD_3r, DRCH_BIST_PATTERN_WORD_3r},
    {DRCA_BIST_PATTERN_WORD_4r, DRCB_BIST_PATTERN_WORD_4r, DRCC_BIST_PATTERN_WORD_4r,
      DRCD_BIST_PATTERN_WORD_4r, DRCE_BIST_PATTERN_WORD_4r, DRCF_BIST_PATTERN_WORD_4r,
      DRCG_BIST_PATTERN_WORD_4r, DRCH_BIST_PATTERN_WORD_4r},
    {DRCA_BIST_PATTERN_WORD_5r, DRCB_BIST_PATTERN_WORD_5r, DRCC_BIST_PATTERN_WORD_5r,
      DRCD_BIST_PATTERN_WORD_5r, DRCE_BIST_PATTERN_WORD_5r, DRCF_BIST_PATTERN_WORD_5r,
      DRCG_BIST_PATTERN_WORD_5r, DRCH_BIST_PATTERN_WORD_5r},
    {DRCA_BIST_PATTERN_WORD_6r, DRCB_BIST_PATTERN_WORD_6r, DRCC_BIST_PATTERN_WORD_6r,
      DRCD_BIST_PATTERN_WORD_6r, DRCE_BIST_PATTERN_WORD_6r, DRCF_BIST_PATTERN_WORD_6r,
      DRCG_BIST_PATTERN_WORD_6r, DRCH_BIST_PATTERN_WORD_6r},
    {DRCA_BIST_PATTERN_WORD_7r, DRCB_BIST_PATTERN_WORD_7r, DRCC_BIST_PATTERN_WORD_7r,
      DRCD_BIST_PATTERN_WORD_7r, DRCE_BIST_PATTERN_WORD_7r, DRCF_BIST_PATTERN_WORD_7r,
      DRCG_BIST_PATTERN_WORD_7r, DRCH_BIST_PATTERN_WORD_7r}};
  const static soc_field_t bist_pattern_field[] = {BIST_PATTERN_0f, BIST_PATTERN_1f, BIST_PATTERN_2f, BIST_PATTERN_3f,
    BIST_PATTERN_4f, BIST_PATTERN_5f, BIST_PATTERN_6f, BIST_PATTERN_7f};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_CONFIGURE_BIST);

  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): configure BIST:\n"), FUNCTION_NAME()));
  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): unit=%d, dram_ndx=%d, WriteWeight=%d, ReadWeight=%d, BistNumActions=%d ,BistStartAddress=0x%x, BistEndAddress=0x%x\n"), FUNCTION_NAME(), unit, dram_ndx, write_weight, read_weight, bist_num_actions, bist_start_address, bist_end_address));
  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): PatternBitMode=%d, TwoAddrMode=%d, PRBSMode=%d, ConsAddr8Banks=%d, AddressShiftMode=%d, DataShiftMode=%d, DataAddrMode=%d\n"), FUNCTION_NAME(), pattern_bit_mode,two_addr_mode,prbs_mode,cons_addr_8_banks,address_shift_mode,data_shift_mode,data_addr_mode));
  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): BistPattern: 0=0x%x, 1=0x%x, 2=0x%x, 3=0x%x, 4=0x%x, 5=0x%x, 6=0x%x, 7=0x%x\n"), FUNCTION_NAME(), bist_pattern[0], bist_pattern[1], bist_pattern[2], bist_pattern[3], bist_pattern[4], bist_pattern[5], bist_pattern[6], bist_pattern[7]));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, WRITE_WEIGHTf, write_weight);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, READ_WEIGHTf, read_weight);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, PATTERN_BIT_MODEf, pattern_bit_mode);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, TWO_ADDR_MODEf, two_addr_mode);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, PRBS_MODEf, prbs_mode);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, CONS_ADDR_8_BANKSf, cons_addr_8_banks);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, ADDRESS_SHIFT_MODEf, address_shift_mode); 
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, DATA_SHIFT_MODEf, data_shift_mode);
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, DATA_ADDR_MODEf, data_addr_mode);

  
  soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, IND_WR_RD_ADDR_MODEf, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_get(unit, drc_bist_number_of_actions[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &reg_val, BIST_NUM_ACTIONSf, bist_num_actions);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, drc_bist_number_of_actions[dram_ndx], REG_PORT_ANY, 0, reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg32_get(unit, drc_bist_start_address[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &reg_val, BIST_START_ADDRESSf, bist_start_address);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg32_set(unit, drc_bist_start_address[dram_ndx], REG_PORT_ANY, 0, reg_val));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg32_get(unit, drc_bist_end_address[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &reg_val, BIST_END_ADDRESSf, bist_end_address); 
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, soc_reg32_set(unit, drc_bist_end_address[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  for(i=0; i < ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg32_get(unit, drc_bist_pattern_words_regs[i][dram_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, drc_bist_pattern_words_regs[i][dram_ndx], &reg_val, bist_pattern_field[i], bist_pattern[i]);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_reg32_set(unit, drc_bist_pattern_words_regs[i][dram_ndx], REG_PORT_ANY, 0, reg_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_configure_bist()", 0, 0);
}


uint32
  arad_dram_bist_atomic_action_start(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    uint8 start)
{
  uint32
  reg_val,
  res;
  const static soc_reg_t drc_bist_configurations[] = {DRCA_BIST_CONFIGURATIONSr, DRCB_BIST_CONFIGURATIONSr, DRCC_BIST_CONFIGURATIONSr,
    DRCD_BIST_CONFIGURATIONSr, DRCE_BIST_CONFIGURATIONSr, DRCF_BIST_CONFIGURATIONSr,
    DRCG_BIST_CONFIGURATIONSr, DRCH_BIST_CONFIGURATIONSr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_ATOMIC_ACTION);
  
  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_get(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, BIST_ENf, 0); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg32_set(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, reg_val));
  
  if (start == TRUE) {
    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &reg_val, BIST_ENf, 1); 
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_bist_configurations[dram_ndx], REG_PORT_ANY, 0, reg_val));
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_atomic_action_start()", 0, 0);
}

uint32
  arad_dram_bist_atomic_action_polling(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx)
{
  uint32
  res;
  const static soc_reg_t drc_bist_statuses[] = {DRCA_BIST_STATUSESr, DRCB_BIST_STATUSESr, DRCC_BIST_STATUSESr,
  DRCD_BIST_STATUSESr, DRCE_BIST_STATUSESr, DRCF_BIST_STATUSESr,
  DRCG_BIST_STATUSESr, DRCH_BIST_STATUSESr};
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_ATOMIC_ACTION);

  res = arad_polling(
            unit,
            ARAD_TIMEOUT,
            ARAD_DRAM_BIST_MIN_POLLS,
            drc_bist_statuses[dram_ndx],
            REG_PORT_ANY,
            0,
            BIST_FINISHEDf,
            1
          );
 
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
 LOG_DEBUG(BSL_LS_SOC_DRAM,
           (BSL_META_U(unit,
                       "%s(): Bist Atomic action polling finished \n"), FUNCTION_NAME()));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_atomic_action_polling()", 0, 0);
}

uint32
  arad_dram_bist_atomic_action(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 dram_ndx,
    SOC_SAND_IN uint8  is_infinite,
    SOC_SAND_IN uint8  stop
  )
{ 
  uint32
    res; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_ATOMIC_ACTION);
  
  if (stop == FALSE) {
      res = arad_dram_bist_atomic_action_start(unit, dram_ndx, TRUE); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (is_infinite == FALSE && stop == FALSE) {  
    res = arad_dram_bist_atomic_action_polling(unit, dram_ndx);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }  
  
  if (is_infinite == FALSE || stop == TRUE) {
     res = arad_dram_bist_atomic_action_start(unit, dram_ndx, FALSE); 
     SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  
  LOG_DEBUG(BSL_LS_SOC_DRAM,
            (BSL_META_U(unit,
                        "%s(): Bist Atomic action finished \n"), FUNCTION_NAME()));    

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_atomic_action()", 0, 0);
}


uint32
  arad_dram_read_bist_err_cnt(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 dram_ndx,
    SOC_SAND_OUT uint32 *bist_err_occur,
    SOC_SAND_OUT uint32 *bist_full_err_cnt,
    SOC_SAND_OUT uint32 *bist_single_err_cnt,
    SOC_SAND_OUT uint32 *bist_global_err_cnt
  )
{
  uint32
    reg_val,
    res;
  const static soc_reg_t drc_bist_error_occurred[] = {DRCA_BIST_ERROR_OCCURREDr, DRCB_BIST_ERROR_OCCURREDr,
    DRCC_BIST_ERROR_OCCURREDr, DRCD_BIST_ERROR_OCCURREDr, DRCE_BIST_ERROR_OCCURREDr,
    DRCF_BIST_ERROR_OCCURREDr, DRCG_BIST_ERROR_OCCURREDr, DRCH_BIST_ERROR_OCCURREDr};
  const static soc_reg_t drc_bist_full_mask_error_counter[] = {DRCA_BIST_FULL_MASK_ERROR_COUNTERr, DRCB_BIST_FULL_MASK_ERROR_COUNTERr,
    DRCC_BIST_FULL_MASK_ERROR_COUNTERr, DRCD_BIST_FULL_MASK_ERROR_COUNTERr, DRCE_BIST_FULL_MASK_ERROR_COUNTERr,
    DRCF_BIST_FULL_MASK_ERROR_COUNTERr, DRCG_BIST_FULL_MASK_ERROR_COUNTERr, DRCH_BIST_FULL_MASK_ERROR_COUNTERr};
  const static soc_reg_t drc_bist_single_bit_mask_error_counter[] = {DRCA_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCB_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr,
    DRCC_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCD_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCE_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr,
    DRCF_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCG_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, DRCH_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr};
  const static soc_reg_t drc_bist_global_error_counter[] = {DRCA_BIST_GLOBAL_ERROR_COUNTERr, DRCB_BIST_GLOBAL_ERROR_COUNTERr,
    DRCC_BIST_GLOBAL_ERROR_COUNTERr, DRCD_BIST_GLOBAL_ERROR_COUNTERr, DRCE_BIST_GLOBAL_ERROR_COUNTERr,
    DRCF_BIST_GLOBAL_ERROR_COUNTERr, DRCG_BIST_GLOBAL_ERROR_COUNTERr, DRCH_BIST_GLOBAL_ERROR_COUNTERr};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_READ_BIST_ERR_CNT);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_bist_error_occurred[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *bist_err_occur = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, reg_val, ERR_OCCURREDf); 

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, drc_bist_full_mask_error_counter[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *bist_full_err_cnt = soc_reg_field_get(unit, DRCA_BIST_FULL_MASK_ERROR_COUNTERr, reg_val, FULL_ERR_CNTf); 

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_get(unit, drc_bist_single_bit_mask_error_counter[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *bist_single_err_cnt = soc_reg_field_get(unit, DRCA_BIST_SINGLE_BIT_MASK_ERROR_COUNTERr, reg_val, SINGLE_ERR_CNTf);
   
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_get(unit, drc_bist_global_error_counter[dram_ndx], REG_PORT_ANY, 0, &reg_val));
  *bist_global_err_cnt = soc_reg_field_get(unit, DRCA_BIST_GLOBAL_ERROR_COUNTERr, reg_val, GLOBAL_ERR_CNTf); 

  LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): bist_err_occur=0x%x, bist_full_err_cnt=0x%x,  bist_single_err_cnt=0x%x, bist_global_err_cnt=0x%x\n"), 
                                           FUNCTION_NAME(), *bist_err_occur,     *bist_full_err_cnt,      *bist_single_err_cnt,     *bist_global_err_cnt)); 
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_read_bist_err_cnt()", 0, 0);
}

uint32
    arad_dram_bist_test_start_verify(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 dram_ndx,
        SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    )
{
    uint32 bist_end_address,
        address_size;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_TEST_START_VERIFY);

    
    if(( (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_BIT_MODE) || (info->arad_bist_flags & ARAD_DRAM_BIST_TWO_ADDRESS_MODE))
        && (info->write_weight != 1 || info->read_weight != 1)) {
        LOG_ERROR(BSL_LS_SOC_DRAM,
                  (BSL_META_U(unit,
                              "%s(): When PatternBitMode or TwoAddrMode  are set, WriteWeight and ReadWeight should be set to 1\n"), FUNCTION_NAME()));
        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_BIST_INVALID_TEST_PARAMETERS_ERR, 20, exit);
    }

    if(info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_SHIFT_MODE){
        bist_end_address = SOC_DDR3_NUM_COLUMNS(unit) * SOC_DDR3_NUM_ROWS(unit) * SOC_DDR3_NUM_BANKS(unit)/8 - 1;
        address_size = 23 + (bist_end_address >> (24 - 1));
        if (info->write_weight >= address_size || info->read_weight >= address_size) {
            LOG_ERROR(BSL_LS_SOC_DRAM,
                      (BSL_META_U(unit,
                                  "%s(): When AddressShiftMode is set, WriteWeight and ReadWeight should be less than number of bits in address\n"), FUNCTION_NAME()));
            SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_BIST_INVALID_TEST_PARAMETERS_ERR, 20, exit);
        }
    }

    if(info->bist_num_actions == 0) {
        
        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_BIST_INVALID_TEST_PARAMETERS_ERR, 30, exit);
    }
        
    
    

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_test_start_verify()", 0, 0);
}


uint32
    arad_dram_bist_test_start_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 dram_ndx,
        SOC_SAND_IN ARAD_DRAM_BIST_TEST_RUN_INFO *info
    )
{
    uint32
        i,
        pattern_lcl[ARAD_DRAM_BIST_NOF_PATTERNS],
        res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_BIST_TEST_START_UNSAFE);

    if(info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_DIFF) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x55555555;
        }
    } else if(info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ONE) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0xffffffff;
        }
    } else if(info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ZERO) {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = 0x00000000;
        }
    } else {
        for(i=0; i <  ARAD_DRAM_BIST_NOF_PATTERNS; ++i) {
            pattern_lcl[i] = info->pattern[i];
        }
    }    
    
    if((info->arad_bist_flags & (ARAD_DRAM_BIST_STOP | ARAD_DRAM_BIST_GET_DATA)) == 0) {
        
        res = arad_dram_configure_bist(
                  unit,
                  dram_ndx,
                  info->write_weight,
                  info->read_weight,
                  (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_BIT_MODE) ? TRUE : FALSE,
                  (info->arad_bist_flags & ARAD_DRAM_BIST_TWO_ADDRESS_MODE) ? TRUE : FALSE,
                  (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS) ? TRUE : FALSE,
                  (info->arad_bist_flags & ARAD_DRAM_BIST_CONS_ADDR_8_BANKS) ? TRUE : FALSE,
                  (info->arad_bist_flags & ARAD_DRAM_BIST_ADDRESS_SHIFT_MODE) ? TRUE : FALSE,
                  (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_SHIFT_MODE) ? TRUE : FALSE,
                  (info->pattern_mode == SOC_TMC_DRAM_BIST_DATA_PATTERN_ADDR_MODE) ? TRUE : FALSE,
                  (info->arad_bist_flags & ARAD_DRAM_BIST_INFINITE) ? 0 : info->bist_num_actions,
                  info->bist_start_address,
                  info->bist_end_address,
                  pattern_lcl
            );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    if((info->arad_bist_flags & ARAD_DRAM_BIST_GET_DATA) == 0) {
      res = arad_dram_bist_atomic_action(
                unit,
                dram_ndx,
                (info->arad_bist_flags & ARAD_DRAM_BIST_INFINITE) ? TRUE : FALSE,
                (info->arad_bist_flags & ARAD_DRAM_BIST_STOP) ? TRUE : FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_bist_test_start_unsafe()", 0, 0);
}






STATIC uint32
arad_dram_init_enable_ddr_clk_dividers(
    SOC_SAND_IN int unit
    )
{
    uint32
        res;
    uint32
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_ENABLE_DDR_CLK_DIVIDERS);  

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_DRCA_SPARE_REGISTER_3r(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_SPARE_REGISTER_3r, &reg_val, DPRC_ALIGN_PHY_RSTNf, 0x0);
    soc_reg_field_set(unit, DRCA_SPARE_REGISTER_3r, &reg_val, CLK_DIV_RSTNf, 0x1);
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        soc_reg_field_set(unit, DRCA_SPARE_REGISTER_3r, &reg_val, ARAD_B_ECO_ENf, 0x0);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_DRCBROADCAST_SPARE_REGISTER_3r(unit, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_enable_ddr_clk_dividers()", 0, 0);
}




STATIC uint32
    arad_dram_init_drc_clam_shell_cfg(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32  dram_ndx,
        SOC_SAND_IN uint32  clam_shel_mode,
        SOC_SAND_IN uint32  use_clam_shell_ddr0
    )
{
    uint32
        res;
    uint32
        reg_val;
    static soc_reg_t
        drc_clam_shell[] = {DRCA_CLAM_SHELLr, DRCB_CLAM_SHELLr, DRCC_CLAM_SHELLr, DRCD_CLAM_SHELLr,
        DRCE_CLAM_SHELLr, DRCF_CLAM_SHELLr, DRCG_CLAM_SHELLr, DRCH_CLAM_SHELLr};
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DRC_CLAM_SHELL_CFG);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, drc_clam_shell[dram_ndx], REG_PORT_ANY, 0, &reg_val));

    soc_reg_field_set(unit, DRCA_CLAM_SHELLr, &reg_val, CLAM_SHELL_MODEf, clam_shel_mode);
    soc_reg_field_set(unit, DRCA_CLAM_SHELLr, &reg_val, USE_CLAM_SHELL_DDR_0f, use_clam_shell_ddr0);

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, drc_clam_shell[dram_ndx], REG_PORT_ANY, 0, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_drc_clam_shell_cfg()", 0, 0);
}


STATIC uint32
    arad_dram_init_dprc_out_of_reset(
        SOC_SAND_IN int unit
    )
{
    uint32
        res;
    uint32
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DPRC_OUT_OF_RESET);  

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_DRCA_SPARE_REGISTER_3r(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_SPARE_REGISTER_3r, &reg_val, DPRC_ALIGN_PHY_RSTNf, 0x1);
    soc_reg_field_set(unit, DRCA_SPARE_REGISTER_3r, &reg_val, CLK_DIV_RSTNf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_DRCBROADCAST_SPARE_REGISTER_3r(unit, reg_val));

   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): Take DPRC out of reset.\n"),FUNCTION_NAME()));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_dprc_out_of_reset()", 0, 0);
}


STATIC uint32
    arad_dram_init_cfg_dprcs_last_in_chain(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 last_instance_id
    )
{
    uint32
        reg_val,
        res;
    uint8
        i;
    const static soc_reg_t
        drc_sbus_last_in_chain [] = {DRCA_SBUS_LAST_IN_CHAINr, DRCB_SBUS_LAST_IN_CHAINr, DRCC_SBUS_LAST_IN_CHAINr, DRCD_SBUS_LAST_IN_CHAINr,
            DRCE_SBUS_LAST_IN_CHAINr, DRCF_SBUS_LAST_IN_CHAINr, DRCG_SBUS_LAST_IN_CHAINr, DRCH_SBUS_LAST_IN_CHAINr};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_CFG_DPRCS_LAST_IN_CHAIN);  

    for(i=0; i < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++i)
    {
        if(i != last_instance_id)
        {
            reg_val = 0;
        }
        else
        {
            reg_val = 1;
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_set(unit, drc_sbus_last_in_chain[i], REG_PORT_ANY, 0, reg_val));     
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_cfg_dprcs_last_in_chain()", 0, 0);
}



uint32
    arad_dram_init_power_down_unused_dprcs(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint8* is_valid_dram
    )
{
    uint32
        reg_val,
        fld_val,
        res;
    uint8
        i;
    const static soc_field_t
        dprc_power_down[] = {DPRCA_POWER_DOWNf, DPRCB_POWER_DOWNf, DPRCC_POWER_DOWNf, DPRCD_POWER_DOWNf,
            DPRCE_POWER_DOWNf, DPRCF_POWER_DOWNf, DPRCG_POWER_DOWNf, DPRCH_POWER_DOWNf};
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_POWER_DOWN_UNUSED_DPRCS);


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, ECI_BLOCKS_POWER_DOWNr, REG_PORT_ANY, 0, &reg_val));
    for(i=0; i < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++i) {
        if(is_valid_dram[i]) {
            fld_val = 0x0;
        } else {
            fld_val = 0x1;
        }
        soc_reg_field_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val, dprc_power_down[i], fld_val);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, ECI_BLOCKS_POWER_DOWNr, REG_PORT_ANY, 0, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_power_down_unused_dprcs()", 0, 0);
}




STATIC uint32
    arad_dram_init_write_bypass_pll_mode(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32  bypass_pll_mode
    )
{
    uint32
        res;
    uint32
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_WRITE_BYPASS_PLL_MODE);  

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_DRCA_DPI_POWERr(unit, &reg_val));
    soc_reg_field_set(unit, DRCA_DPI_POWERr, &reg_val, BYPASS_PLLf, bypass_pll_mode);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_DRCBROADCAST_DPI_POWERr(unit, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_write_bypass_pll_mode()", 0, 0);
}

STATIC uint32 arad_dram_init_drc_param_register_write(
    SOC_SAND_IN int unit,
    SOC_SAND_IN ARAD_INIT_DDR *init)
{
    uint32 res;
    int32 instance_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DRC_PARAM_REGISTER_WRITE);

    for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx) {
        if (init->is_valid[instance_idx]) {

           LOG_DEBUG(BSL_LS_SOC_DRAM,
                     (BSL_META_U(unit,
                                 "%s(): configure DRC parameters for dram info. dram_ndx=%d,  dram_freq=%d, dram_type=%d\n"),FUNCTION_NAME(), instance_idx, init->dram_conf.params_mode.dram_freq, init->dram_type));

            res = arad_dram_info_set_unsafe(
                unit,
                instance_idx,
                init->dram_conf.params_mode.dram_freq,
                init->dram_type,
                &init->dram_conf.params_mode.params);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_drc_param_register_write()", 0, 0);
}



STATIC uint32
    arad_dram_init_wait_dram_init_finished(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32  dram_ndx
    )
{
    uint32
        res;
    static soc_reg_t
        drc_dram_init_finished[] = {DRCA_DRAM_INIT_FINISHEDr, DRCB_DRAM_INIT_FINISHEDr, DRCC_DRAM_INIT_FINISHEDr, 
        DRCD_DRAM_INIT_FINISHEDr, DRCE_DRAM_INIT_FINISHEDr, DRCF_DRAM_INIT_FINISHEDr, DRCG_DRAM_INIT_FINISHEDr,
        DRCH_DRAM_INIT_FINISHEDr};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_WAIT_DRAM_INIT_FINISHED);

    res = arad_polling(
        unit,
        ARAD_TIMEOUT,
        ARAD_MIN_POLLS,
        drc_dram_init_finished[dram_ndx],
        REG_PORT_ANY,
        0,
        DRAM_INIT_FINISHEDf,
        1
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): Dram INIT Finished Successfully.\n"),FUNCTION_NAME()));
  
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_wait_dram_init_finished()", 0, 0);
}


STATIC uint32 arad_dram_init_drc_init(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN ARAD_INIT_DDR         *init,
    SOC_SAND_IN uint32                  last_instance_id)
{
    uint32 
        res,
        instance_idx;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DRC_INIT);

    for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx) {
        if (init->is_valid[instance_idx]) {
            res = arad_dram_init_drc_clam_shell_cfg(
                unit,
                instance_idx,
                init->dram_clam_shell_mode[instance_idx] == ARAD_DDR_CLAM_SHELL_MODE_DISABLED ? FALSE : TRUE,
                init->dram_clam_shell_mode[instance_idx] == ARAD_DDR_CLAM_SHELL_MODE_DRAM_0 ? 1 : 0);
            SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        }       
    }

    
    res = arad_dram_rbus_write_br(unit, last_instance_id, 0x34, 0x10028);
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
    
    
    res = arad_mgmt_dram_init_drc_soft_init(unit, init->is_valid, 1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    
    res = arad_dram_init_drc_param_register_write(unit, init);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

    
    res = arad_mgmt_dram_init_drc_soft_init(unit, init->is_valid, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    
    sal_usleep(1000);

    if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) { 
        res = arad_dram_init_wait_dram_init_finished(unit, last_instance_id);
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_drc_init()", 0, 0);
}

STATIC uint32 arad_dram_init_dprc_init(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN ARAD_INIT_DDR         *init,
    SOC_SAND_IN uint32                last_instance_id)
{
    uint32 res = 0x0;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DPRC_INIT);

    SOC_SAND_CHECK_NULL_INPUT(init);

    res = arad_dram_init_cfg_dprcs_last_in_chain(unit, last_instance_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_dram_init_power_down_unused_dprcs(unit, init->is_valid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = arad_dram_init_write_bypass_pll_mode(unit, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = arad_dram_init_enable_ddr_clk_dividers(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = arad_dram_init_dprc_out_of_reset(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = arad_dram_init_drc_phy_register_set(unit, init->dram_type, init->dram_conf.params_mode.dram_freq, init->dram_conf.params_mode.params);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_dprc_init()", 0, 0);
}


STATIC uint32 arad_dram_init_ddr_configure_mmu(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 nof_banks,
    SOC_SAND_IN uint32 nof_columns,
    SOC_SAND_IN uint32 nof_drams,
    SOC_SAND_IN uint32 dram_freq,
    SOC_SAND_IN ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size)
{
    uint32
        fld_val,
        reg_val,
        interleaving_bits,
        res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DDR_CONFIGURE_MMU)

    if (SOC_IS_QAX(unit)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("access to registers should be fixed for QAX at places we used _REG(32|64) access routines")));
    }


    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_MMU_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, &reg_val));
    fld_val = (nof_banks == ARAD_DRAM_NUM_BANKS_4) ? 0x0 : 0x1;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, DRAM_BANK_NUMf, fld_val);
    switch(nof_columns) {
        case ARAD_DRAM_NUM_COLUMNS_256:
            fld_val = 0;
            break;
        case ARAD_DRAM_NUM_COLUMNS_512:
            fld_val = 1;
            break;
        case ARAD_DRAM_NUM_COLUMNS_1024:
            fld_val = 2;
            break;
        case ARAD_DRAM_NUM_COLUMNS_2048:
            fld_val = 3;
            break;
        case ARAD_DRAM_NUM_COLUMNS_4096:
            fld_val = 4;
            break;
        case ARAD_DRAM_NUM_COLUMNS_8192:
            fld_val = 5;
            break;
        default:
            
            break;
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, DRAM_COL_NUMf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, INTERLEAVING_RVRSE_MODEf, fld_val);


    
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): dbuff_size/64=%d\n"), FUNCTION_NAME(), dbuff_size/64));    
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): nof_banks * nof_drams=%d\n"), FUNCTION_NAME(), nof_banks * nof_drams));   
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): soc_sand_log2_round_down(dbuff_size/64)=%d\n"), FUNCTION_NAME(), soc_sand_log2_round_down(dbuff_size/64)));
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): soc_sand_log2_round_down(nof_banks * nof_drams)=%d\n"), FUNCTION_NAME(), soc_sand_log2_round_down(nof_banks * nof_drams))); 
     
   interleaving_bits = SOC_SAND_MAX(soc_sand_log2_round_down(dbuff_size/64) + 2, soc_sand_log2_round_down(nof_banks * nof_drams) + 2);
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): interleaving_bits=%d\n"), FUNCTION_NAME(), interleaving_bits));  

    switch(interleaving_bits) {
        case 3:
            fld_val = 0;
            break;
        case 4:
            fld_val = 1;
            break;
        case 5:
            fld_val = 2;
            break;
        case 6:
            fld_val = 3;
            break;
        case 7:
            fld_val = 5;
            break;
        case 8:
            fld_val = 7;
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);
            break;
    }
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): fld_val=BANK_INTERLEAVING_MODEf=%d\n"), FUNCTION_NAME(), fld_val));
   SOC_DPP_CONFIG(unit)->arad->init.dram.interleaving_bits = interleaving_bits;

    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, BANK_INTERLEAVING_MODEf, fld_val);

    if (dram_freq > 933) {
        
        fld_val = (nof_drams == 6 ? 3 : 4);
    } else if ((dram_freq > 800) && (dram_freq <= 933)) {
        
        fld_val = (nof_drams == 6 ? 4 : 2);
    } else if (dram_freq <= 800) {
        
        fld_val = 3;
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, READ_FIFOS_ALLOWEDf, fld_val);

    if (dram_freq > 933) {
        
        fld_val = 10;
    } else if ((dram_freq > 800) && (dram_freq <= 933)) {
        
        fld_val = 15;
    } else if (dram_freq <= 800) {
        
        fld_val = 20;
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, MIN_READ_COMMANDSf, fld_val);

    fld_val = (nof_drams == 3) ? 1 : 0;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg_val, LAYER_PERIODICITYf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_MMU_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr(unit, &reg_val));

    if (dram_freq > 933) {
        
        fld_val = (nof_drams == 6 ? 170 : 190);
    } else if ((dram_freq > 800) && (dram_freq <= 933)) {
        
        fld_val = 190;
    } else if (dram_freq <= 800) {
        
        fld_val = 170;
    }
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, WRITE_INHIBIT_CNT_SATf, fld_val);

    fld_val = 2;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, ADDRESS_MAP_CONFIG_GENf, fld_val);

    fld_val = 2;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, ADDRESS_MAP_CONFIG_RDf, fld_val);

    if (SOC_IS_ARAD_B1_AND_BELOW(unit)) 
    {
        if (dram_freq > 933) {
            
            fld_val = (nof_drams == 6 ? 8 : 9);
        } else if ((dram_freq > 800) && (dram_freq <= 933)) {
            
            fld_val = (nof_drams == 6 ? 9 : 8);
        } else if (dram_freq <= 800) {
            
            fld_val = 6;
        }
        soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, WR_CMD_DISTANCEf, fld_val);

          if (dram_freq > 933) {
            
            fld_val = 7;
        } else if ((dram_freq > 800) && (dram_freq <= 933)) {
            
            fld_val = (nof_drams == 6 ? 8 : 7);
        } else if (dram_freq <= 800) {
            
            fld_val = 5;
        }
        soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, RD_CMD_DISTANCEf, fld_val);
    } else 
    {
        uint32
            dd_rt_rcdw,
            dd_rt_rcdr,
            cnt_wrapprd,
            cnt_rdapprd,
            other_reg_val;

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg32_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, REG_PORT_ANY, 0, &other_reg_val));
        dd_rt_rcdw = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, other_reg_val, DD_RT_RCDWf);
        dd_rt_rcdr = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_2r, other_reg_val, DD_RT_RCDRf);

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_DRCA_AC_OPERATING_CONDITIONS_3r(unit, &other_reg_val));
        cnt_wrapprd = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, other_reg_val, CNT_WRAPPRDf);
        cnt_rdapprd = soc_reg_field_get(unit, DRCA_AC_OPERATING_CONDITIONS_3r, other_reg_val, CNT_RDAPPRDf);

        fld_val = (cnt_wrapprd + dd_rt_rcdw) / 4;
        soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, WR_CMD_DISTANCEf, fld_val);

        fld_val = (cnt_rdapprd + dd_rt_rcdr) / 4;
        soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg_val, RD_CMD_DISTANCEf, fld_val);

    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr(unit, reg_val));
  
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, READ_MMU_GENERAL_CONFIGURATION_REGISTER_2r(unit, &reg_val));
    switch(nof_drams) {
        case 8:
        case 1:
            fld_val = 0x4;
            break; 
        case 2:
            fld_val = 0x3;
            break;
        case 3:
            fld_val = 0x2;
            break;
        case 4:
            fld_val = 0x1;
            break;
        case 6:
            fld_val = 0x0;
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(ARAD_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 90, exit);
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg_val, DRAM_NUMf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_MMU_GENERAL_CONFIGURATION_REGISTER_2r(unit, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_ddr_configure_mmu()", 0, 0);
}

STATIC uint32 arad_dram_init_tune_ddr(
    SOC_SAND_IN int                unit,
    SOC_SAND_IN uint8                  *is_valid)
{
    uint32
        res = SOC_E_NONE,
        auto_tune,
        instance_idx = 0,
        bist_err_occur = 0xffffffff,  
        bist_full_err_cnt, 
        bist_single_err_cnt,
        bist_global_err_cnt;
    ARAD_DRAM_BIST_TEST_RUN_INFO 
        bist_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_TUNE_DDR);

    
    auto_tune = soc_property_get(unit, spn_DDR3_AUTO_TUNE, -1);
    if (-1 == auto_tune) {
        auto_tune = soc_property_get(unit, spn_EXT_RAM_AUTO_TUNE, 0x2);
    }

    SOC_SAND_CHECK_NULL_INPUT(is_valid); 

    sal_memset(&bist_info, 0x0, sizeof(ARAD_DRAM_BIST_TEST_RUN_INFO));
    
    if (soc_feature(unit, soc_feature_ddr3)) {
        
        for (instance_idx=0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max) ;instance_idx++) {
            if (is_valid[instance_idx]) {
#if defined(BCM_DDR3_SUPPORT)
#if defined(PLISIM)
          if (!SAL_BOOT_PLISIM) 
#endif
              {
                if ((auto_tune == 0x0) || (auto_tune == 0x2)) {
                    LOG_VERBOSE(BSL_LS_SOC_DRAM,
                                (BSL_META_U(unit,
                                            "%s(): Restore Configuration. dram ndx=%d\n"), FUNCTION_NAME(), instance_idx));
                    res = soc_ddr40_shmoo_restorecfg(unit, (instance_idx * 2));
                    if ((res != SOC_E_NONE) && (auto_tune == 0x0)) {
                        LOG_ERROR(BSL_LS_SOC_DRAM,
                                  (BSL_META_U(unit,
                                              "%s(): Failed to Restore configuration for dram_ndx=%d.\n"), FUNCTION_NAME(), instance_idx));
                    }
                }

                if ((auto_tune == 0x1) || ((auto_tune == 0x2) && (res != SOC_E_NONE))) {
                    
                   LOG_VERBOSE(BSL_LS_SOC_DRAM,
                               (BSL_META_U(unit,
                                           "%s(): Shmoo dram ndx=%d\n"), FUNCTION_NAME(), instance_idx));    
                    if (soc_ddr40_shmoo_ctl(unit, (instance_idx * 2), 2, 2, 0, 0) != SOC_E_NONE) {
                       LOG_ERROR(BSL_LS_SOC_DRAM,
                                 (BSL_META_U(unit,
                                             "%s(): DDR Restore config Failed for dram_ndx=%d.\n"), FUNCTION_NAME(), instance_idx));
                        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_DDR_TUNE_FAILED_ERR, 20, exit);
                    }
                    if (soc_ddr40_shmoo_savecfg(unit, (instance_idx * 2))) {
                       LOG_ERROR(BSL_LS_SOC_DRAM,
                                 (BSL_META_U(unit,
                                             "%s(): Save dram_ndx=%d config parameters.\n"), FUNCTION_NAME(), instance_idx));
                        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_DDR_TUNE_FAILED_ERR, 30, exit);
                    }
                }
          }
#endif
          }
        }

        if (SOC_DPP_CONFIG(unit)->arad->init.dram.bist_enable == 0x1) {
            for (instance_idx=0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max) ;instance_idx++) {
                if (is_valid[instance_idx]) {
                    
                    
                    bist_info.write_weight = 255;
                    bist_info.read_weight = 255;
    #if defined(BCM_DDR3_SUPPORT)
                    bist_info.bist_end_address = SOC_DDR3_NUM_COLUMNS(unit) * SOC_DDR3_NUM_ROWS(unit) * SOC_DDR3_NUM_BANKS(unit) / 8 - 1;
                    bist_info.bist_num_actions =  (bist_info.bist_end_address + 1) * 2;
    #endif
                    bist_info.pattern_mode = SOC_TMC_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS;
                    bist_info.arad_bist_flags =  ARAD_DRAM_BIST_CONS_ADDR_8_BANKS;

                    res = arad_dram_bist_test_start_unsafe(unit, instance_idx, &bist_info);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

                    res = arad_dram_read_bist_err_cnt(unit, instance_idx, &bist_err_occur, &bist_full_err_cnt, &bist_single_err_cnt, &bist_global_err_cnt);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
                    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): instance_idx=%d, bist_err_occur=0x%x, bist_full_err_cnt=0x%x, bist_single_err_cnt=0x%x, bist_global_err_cnt=0x%x\n" ),
                                                       FUNCTION_NAME(), instance_idx,    bist_err_occur,      bist_full_err_cnt,      bist_single_err_cnt,      bist_global_err_cnt));

                    if ((bist_err_occur != 0) || (bist_full_err_cnt != 0) || (bist_single_err_cnt != 0) || (bist_global_err_cnt != 0)) 
                    {
                       LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): ERROR: dram_idx=%d Failed BIST. bist_err_occur=0x%x, bist_full_err_cnt=0x%x, bist_single_err_cnt=0x%x, bist_global_err_cnt=0x%x\n"), 
                                                          FUNCTION_NAME(), instance_idx, bist_err_occur, bist_full_err_cnt, bist_single_err_cnt, bist_global_err_cnt));
                        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_DDR_TUNE_FAILED_ERR, 70, exit);
                    }
                }
             }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_tune_ddr()", instance_idx, bist_err_occur);
}



uint32 arad_dram_init_ddr(
   SOC_SAND_IN int                unit,
   SOC_SAND_IN ARAD_INIT_DDR         *init)
{
    uint32
        fld_val = 0,
        res,val;
    uint32
        instance_idx = 0,
        last_instance_id = 0,
        nof_drams = 0,
        dram_config = 0;
    ARAD_ITM_DBUFF_SIZE_BYTES
        dbuff_size;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DRAM_INIT_DDR);

    SOC_SAND_CHECK_NULL_INPUT(init);

     
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCA_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCB_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCC_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  13,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCD_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCE_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCF_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCG_SBUS_BROADCAST_IDr(unit,  63));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  17,  exit, ARAD_REG_ACCESS_ERR,WRITE_DRCH_SBUS_BROADCAST_IDr(unit,  63));

    if (init->conf_mode == ARAD_HW_DRAM_CONF_MODE_PARAMS) {
        if (init->nof_banks != init->dram_conf.params_mode.params.nof_banks) {
            SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR, 10, exit);
        }
        if (init->nof_columns != init->dram_conf.params_mode.params.nof_cols) {
            SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_NOF_COLS_OUT_OF_RANGE_ERR, 20, exit);
        }
    }

    for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx) {
        if (init->is_valid[instance_idx]) {
            last_instance_id = instance_idx;
            dram_config |= (0x3 << (instance_idx << 1));
        }
    }
    nof_drams = init->nof_drams;

#ifdef BCM_DDR3_SUPPORT
    soc_ddr40_set_shmoo_dram_config(unit, dram_config);
#endif

    res = arad_dram_init_dprc_init(unit, init, last_instance_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef BCM_DDR3_SUPPORT
    
    soc_ddr40_phy_calibrate(unit, 0, 2, 0);
#endif

    res = arad_dram_init_drc_init(unit, init, last_instance_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    sal_usleep(10);

    
    res = arad_dram_init_tune_ddr(unit, init->is_valid);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, DBUFF_SIZEf, &fld_val));
    res = arad_itm_dbuff_internal2size(fld_val, &dbuff_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    res = arad_dram_init_ddr_configure_mmu(unit, init->nof_banks, init->nof_columns, nof_drams, init->dram_conf.params_mode.dram_freq, dbuff_size);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

    
    res = arad_dram_init_buffer_error_cntr_unsafe(unit, -1);
    SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

    res = arad_dram_crc_del_buffer_max_reclaims_get_unsafe(unit, &val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

    if(val != 0) {
        res = arad_dram_crc_delete_buffer_enable_unsafe(unit, 1);
        SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
    }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_dram_init_ddr()", 0, 0);
}


uint32
    arad_dram_mmu_indirect_get_logical_address_full_unsafe(
         SOC_SAND_IN int unit,
         SOC_SAND_IN uint32 buf_num,
         SOC_SAND_IN uint32 index,
         SOC_SAND_OUT uint32* addr_full 
    )
{
    int indx_num_bits;
    int dram_buffer_num_max = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(addr_full);

    
    dram_buffer_num_max = SOC_IS_JERICHO(unit) ? SOC_JER_DRAM_BUFFER_NUM_MAX : SOC_ARAD_DRAM_BUFFER_NUM_MAX ;
    if (buf_num > dram_buffer_num_max) {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Buffer number=%d (0x%x), Exceeded max size=%d (0x%x). \n"),  FUNCTION_NAME(), buf_num, buf_num, dram_buffer_num_max, dram_buffer_num_max));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 30, exit);
    }

    *addr_full = 0;

    switch(SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size) {
    case ARAD_ITM_DBUFF_SIZE_BYTES_4096:
        indx_num_bits = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 4 : 6;
        break;
    case ARAD_ITM_DBUFF_SIZE_BYTES_2048:
        indx_num_bits = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 3 : 5;
        break;
    case ARAD_ITM_DBUFF_SIZE_BYTES_1024:
        indx_num_bits = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 2 : 4;
        break;
    case ARAD_ITM_DBUFF_SIZE_BYTES_512:
        indx_num_bits = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 1 : 3;
        break;
    case ARAD_ITM_DBUFF_SIZE_BYTES_256:
        indx_num_bits = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? 0 : 2;
        break;
    default:
        
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): undefined dbuff_size=%d\n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        break;
    }

    *addr_full = (index)  |  ( (buf_num & dram_buffer_num_max) << indx_num_bits );

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_get_logical_address_full_unsafe()", 0, 0);
}


uint32 
arad_dram_logical2physical_addr_mapping_interleaved_cell_address_get(
    int unit,
    uint32 logical_address) 
{
    uint32
        interleaved_cell_address = 0x0,
        interleaving_bits        = SOC_DPP_CONFIG(unit)->arad->init.dram.interleaving_bits,
        logical_addr_low         = 0x0,
        logical_addr_high        = 0x0,
        xored_bit_address_tmp    = 0x0,
        xored_bit_address        = 0x0;

    logical_addr_low  = SOC_SAND_GET_BITS_RANGE(logical_address,  interleaving_bits      - 1, 0);
    logical_addr_high = SOC_SAND_GET_BITS_RANGE(logical_address, (interleaving_bits * 2) - 1, interleaving_bits);

    xored_bit_address_tmp = logical_addr_high ^ logical_addr_low;

    if (interleaving_bits == 0x7) {
        xored_bit_address  = 0x0 | SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 0 , 0);
        xored_bit_address |=      (SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 6 , 4) << 1);
        xored_bit_address |=      (SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 3 , 1) << 4);
    } else if (interleaving_bits == 0x8) {
        xored_bit_address  = 0x0 | SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 1 , 0);
        xored_bit_address |=      (SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 7 , 5) << 2);
        xored_bit_address |=      (SOC_SAND_GET_BITS_RANGE(xored_bit_address_tmp, 4 , 2) << 5);
    } else {
        xored_bit_address = xored_bit_address_tmp;
    }

    interleaved_cell_address  = 0x0 | SOC_SAND_GET_BITS_RANGE(xored_bit_address, interleaving_bits - 1,       0);
    interleaved_cell_address |=      (SOC_SAND_GET_BITS_RANGE(logical_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, interleaving_bits) << interleaving_bits);


    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): interleaving_bits=%d, logical_addr_low=0x%x, logical_addr_high=0x%x, xored_tmp=0x%x, xored=0x%x\n"), 
                          FUNCTION_NAME(), interleaving_bits, logical_addr_low, logical_addr_high, xored_bit_address_tmp, xored_bit_address));
    
    return interleaved_cell_address;
}

uint32 
arad_dram_logical2physical_addr_mapping_2dram(
    int unit, 
    uint32 logical_address) 
{
    uint32
        interleaved_cell_address = 0x0,
        dram_address= 0x0,
        phy_addr;

    interleaved_cell_address = arad_dram_logical2physical_addr_mapping_interleaved_cell_address_get(unit, logical_address);

    dram_address  = 0x0 | SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 5, 2);
    
    dram_address |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 1, 0)) << 6);
    dram_address |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH - 2, 6)) << 8);

    if (SOC_SAND_GET_BITS_RANGE(dram_address, 3, 3) == 0x1) {
        phy_addr  = 0x0 | SOC_SAND_GET_BITS_RANGE(dram_address, 2, 0);
        phy_addr |= 0x3 << 3; 
        phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(dram_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 6)) << 6);
    } else {
        phy_addr = dram_address;
    }

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): interleaved_cell_address=0x%x, dram_address=0x%x\n"), FUNCTION_NAME(), interleaved_cell_address ,dram_address));

    return phy_addr;
}


uint32 
arad_dram_logical2physical_addr_mapping_3dram(
    int unit, 
    uint32 logical_address) 
{
    uint32
        logical_address_for_division = 0x0,
        layer_num = 0x0,
        layer_offset = 0x0,
        phy_addr = 0x0;

    logical_address_for_division = 0x0 | SOC_SAND_GET_BITS_RANGE(logical_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 6);
    layer_num = logical_address_for_division / 3;
    layer_offset  = (((SOC_SAND_GET_BITS_RANGE(logical_address, 5, 2) % 16) ^ (SOC_SAND_GET_BITS_RANGE(layer_num, 4, 1))) * 3) 
                     + (SOC_SAND_GET_BITS_RANGE(logical_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 2) % 3);

    phy_addr  = 0x0 | SOC_SAND_GET_BITS_RANGE(layer_offset, 2, 0);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(layer_offset, 5, 4)) << 3);
    
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(layer_offset, 3, 3)) << 6);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(logical_address, 1, 0)) << 7);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(layer_num, 17, 0)) << 9);

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): logical_address_for_division=0x%x, layer_num=0x%x, layer_offset=0x%x\n"), 
                          FUNCTION_NAME(), logical_address_for_division ,layer_num, layer_offset));

    return phy_addr;
}


uint32 
arad_dram_logical2physical_addr_mapping_4dram(
    int unit, 
    uint32 logical_address) 
{
    uint32
        interleaved_cell_address = 0x0,
        dram_address = 0x0,
        phy_addr = 0x0;

    interleaved_cell_address = arad_dram_logical2physical_addr_mapping_interleaved_cell_address_get(unit, logical_address);

    dram_address  = 0x0 | SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 6, 2);
    
    dram_address |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 1, 0)) << 6);
    dram_address |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH - 1, 7)) << 8);

    if (SOC_SAND_GET_BITS_RANGE(dram_address, 4, 3) == 0x2) {
        phy_addr  = 0x0 | SOC_SAND_GET_BITS_RANGE(dram_address, 2, 0);
        phy_addr |= 0x4 << 3;
        phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(dram_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 6)) << 6);
    } else {
        phy_addr = dram_address;
    }

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): interleaved_cell_address=0x%x, dram_address=0x%x\n"), FUNCTION_NAME(), interleaved_cell_address ,dram_address));

    return phy_addr;
}

uint32 
arad_dram_logical2physical_addr_mapping_6dram(
    int unit, 
    uint32 logical_address) 
{
    uint32
        logical_address_for_division = 0x0,
        layer_num = 0x0,
        layer_offset = 0x0,
        phy_addr = 0x0;

    logical_address_for_division = 0x0 | SOC_SAND_GET_BITS_RANGE(logical_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 6);
    layer_num = logical_address_for_division / 3;
    layer_offset  = (((SOC_SAND_GET_BITS_RANGE(logical_address, 5, 2) % 16) ^ (SOC_SAND_GET_BITS_RANGE(layer_num, 3, 0))) * 3) 
                     + (SOC_SAND_GET_BITS_RANGE(logical_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH, 2) % 3);

    phy_addr  = 0x0 | SOC_SAND_GET_BITS_RANGE(layer_offset, 5, 0);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(logical_address, 1, 0)) << 6);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(layer_num, 18, 0)) << 8);

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): logical_address_for_division=0x%x, layer_num=0x%x, layer_offset=0x%x\n"), 
                          FUNCTION_NAME(), logical_address_for_division ,layer_num, layer_offset));

    return phy_addr;
}

uint32 
arad_dram_logical2physical_addr_mapping_8dram(
    int unit, 
    uint32 logical_address) 
{
    uint32
        interleaved_cell_address = 0x0,
        phy_addr = 0x0;

    interleaved_cell_address = arad_dram_logical2physical_addr_mapping_interleaved_cell_address_get(unit, logical_address);

    phy_addr  = 0x0 | SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 7, 2);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, 1, 0)) << 6);
    phy_addr |=     ((SOC_SAND_GET_BITS_RANGE(interleaved_cell_address, SOC_ARAD_MMU_DDR_ADDR_WIDTH - 1, 8)) << 8);

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): interleaved_cell_address=0x%x\n"), FUNCTION_NAME(), interleaved_cell_address));

    return phy_addr;
}


 

uint32 
arad_dram_logical2physical_addr_mapping(
    int unit, 
    int buf_num,
    int index,
    uint32 *phy_addr) 
{
    uint32
        res = 0x0,
        nof_drams = SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams,
        logical_address = 0x0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = arad_dram_mmu_indirect_get_logical_address_full_unsafe(unit, buf_num, index, &logical_address);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): buf_num=0x%x, index=0x%x, logical_address=0x%x, nof_drams=%d, dbuff_size=%d\n"), 
                          FUNCTION_NAME(), buf_num ,index, logical_address, nof_drams, SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size));

    switch(nof_drams) {
    case 2:
        *phy_addr = arad_dram_logical2physical_addr_mapping_2dram(unit, logical_address);
        break;
    case 3:
        *phy_addr = arad_dram_logical2physical_addr_mapping_3dram(unit, logical_address);
        break;
    case 4:
        *phy_addr = arad_dram_logical2physical_addr_mapping_4dram(unit, logical_address);
        break;
    case 6: 
        *phy_addr = arad_dram_logical2physical_addr_mapping_6dram(unit, logical_address);
        break;
    case 8:
        *phy_addr = arad_dram_logical2physical_addr_mapping_8dram(unit, logical_address);
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_DRAM,
                  (BSL_META_U(unit,
                              "%s(): undefined bif_drams=%d\n"), FUNCTION_NAME(), nof_drams));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
        break;
    }

    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): *phy_addr=0x%x\n"), FUNCTION_NAME(), *phy_addr));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_logical2physical_addr_mapping()", buf_num, index);
}



uint32
    arad_dram_mmu_indirect_read_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        SOC_SAND_OUT soc_reg_above_64_val_t* data
    )
{
    int res;
    uint32 reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_field32_modify(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, INDIRECT_LOGICAL_MODEf, logical_mod));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_get(unit, MMU_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, addr);
    soc_reg_field_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, reg_val));

    
    if (SOC_IS_QAX(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_field32_modify(unit, MMU_DRAM_INDIRECT_ADDRESSr, REG_PORT_ANY, DRAM_INDIRECT_ADDRESSf, addr));
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_field32_modify(unit, MMU_INDIRECT_COMMANDr, REG_PORT_ANY, INDIRECT_COMMAND_TRIGGERf, 0x1));

    
    res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MMU_INDIRECT_COMMANDr, REG_PORT_ANY,  0, INDIRECT_COMMAND_COUNTf, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_above_64_get(unit, MMU_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, data[0]));
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 71, exit, soc_reg_above_64_get(unit, MMU_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, data[1])); 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 72, exit, soc_reg_above_64_get(unit, MMU_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, data[2])); 
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 72, exit, soc_reg_above_64_get(unit, MMU_INDIRECT_COMMAND_RD_DATAr, REG_PORT_ANY, 0, data[3]));       
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_read_unsafe()", 0, 0);
}


uint32
    arad_dram_mmu_indirect_write_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 logical_mod,
        SOC_SAND_IN uint32 addr,
        SOC_SAND_OUT soc_reg_above_64_val_t* data
    )
{
    int res;
    uint32 reg_val;
    uint32 enable_dynamic_memory_access_orig_val = 0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(SOC_IS_JERICHO(unit)) {
        
        soc_reg32_get(unit, MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, &reg_val);
        enable_dynamic_memory_access_orig_val = soc_reg_field_get(unit, MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr, reg_val, ENABLE_DYNAMIC_MEMORY_ACCESSf);
        soc_reg_field_set(unit, MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr, &reg_val, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        soc_reg32_set(unit, MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, reg_val);
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg_field32_modify(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, INDIRECT_LOGICAL_MODEf, logical_mod));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, soc_reg32_get(unit, MMU_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, &reg_val));
    soc_reg_field_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, addr);
    soc_reg_field_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, soc_reg32_set(unit, MMU_INDIRECT_COMMAND_ADDRESSr, REG_PORT_ANY, 0, reg_val));

    
    if (SOC_IS_QAX(unit))
    {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_field32_modify(unit, MMU_DRAM_INDIRECT_ADDRESSr, REG_PORT_ANY, DRAM_INDIRECT_ADDRESSf, addr));
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, soc_reg_above_64_set(unit, MMU_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, data[0]));
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 51, exit, soc_reg_above_64_set(unit, MMU_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, data[1]));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 52, exit, soc_reg_above_64_set(unit, MMU_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, data[2]));
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 53, exit, soc_reg_above_64_set(unit, MMU_INDIRECT_COMMAND_WR_DATAr, REG_PORT_ANY, 0, data[3]));
    }

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, soc_reg_field32_modify(unit, MMU_INDIRECT_COMMANDr, REG_PORT_ANY, INDIRECT_COMMAND_TIMEOUTf, 0x3000));

    
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, soc_reg_field32_modify(unit, MMU_INDIRECT_COMMANDr, REG_PORT_ANY, INDIRECT_COMMAND_TRIGGERf, 0x1));  
    
    
    res = arad_polling(unit, ARAD_TIMEOUT, ARAD_MIN_POLLS, MMU_INDIRECT_COMMANDr, REG_PORT_ANY, 0, INDIRECT_COMMAND_COUNTf, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    if(SOC_IS_JERICHO(unit)) {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, soc_reg_field32_modify(unit, MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, ENABLE_DYNAMIC_MEMORY_ACCESSf, enable_dynamic_memory_access_orig_val));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_mmu_indirect_write_unsafe()", 0, 0);
}


uint32
soc_arad_user_buffer_dram_access(
    SOC_SAND_IN     int unit, 
    SOC_SAND_IN     uint32 flags, 
    SOC_SAND_IN     uint32 access_type, 
    SOC_SAND_INOUT  uint8 *buf, 
    SOC_SAND_IN     int offset, 
    SOC_SAND_IN     int nbytes)
{
    int 
        res,
        logical2phy_trans = 0x0,
        write = 0x0,
        buf_num_offset = 0x0,
        buf_ndx = 0x0,
        buf_num = 0x0,
        logical_mod = 0x1,
        nbytes_done = 0x0,
        len = 0x0,
        loop_offset = 0x0,
        dbuff_size = SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size,
        max_index = 0x0,
        dram_mmu_ind_data_len = 0;
    uint32
        addr_full = 0x0;
    soc_reg_above_64_val_t
        data[SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE];


    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    
    SOC_SAND_CHECK_NULL_INPUT(buf);

    if (offset + nbytes > (SOC_DPP_CONFIG(unit)->arad->init.dram.dram_user_buffer_size_mbytes * 1000000)) {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Exceeded dram buffer size. offset=%d, nbytes=%d, dram_user_buffer_size_mbytes=%d\n"), 
                                                     FUNCTION_NAME(), offset, nbytes, SOC_DPP_CONFIG(unit)->arad->init.dram.dram_user_buffer_size_mbytes));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    }

    if ((access_type != SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_READ) && (access_type != SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_WRITE)) {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): undefined access type. access_type=%d\n"), FUNCTION_NAME(), access_type));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }

    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): flags=0x%x, access_type=0x%x, buf=%p, offset=%d, nbytes=%d\n"), FUNCTION_NAME(), flags ,access_type, buf, offset, nbytes));

    
    if (flags & SOC_ARAD_DRAM_USER_BUFFER_FLAGS_LOGICAL2PHY_TRANS) {
        logical2phy_trans = 0x1;
        logical_mod = 0x0;
    }

    
    if (access_type & SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_WRITE) {
        write = 0x1;
    }

    dram_mmu_ind_data_len = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? SOC_JER_DRAM_MMU_IND_DATA_LEN : SOC_ARAD_DRAM_MMU_IND_DATA_LEN ;

    
    max_index  = dbuff_size / dram_mmu_ind_data_len;

    
    buf_num_offset = (offset / dbuff_size);
    buf_ndx        = (offset % dbuff_size) / dram_mmu_ind_data_len;
    buf_num        = SOC_DPP_CONFIG(unit)->arad->init.dram.dram_user_buffer_start_ptr + buf_num_offset;

    LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): dbuff_size=%d, max_index=%d, buf_num_offset=%d, buf_ndx=%d, buf_num=%d, dram_user_buffer_start_ptr=%d, dram_mmu_ind_data_len=%d\n"), 
                          FUNCTION_NAME(), dbuff_size, max_index, buf_num_offset, buf_ndx, buf_num, SOC_DPP_CONFIG(unit)->arad->init.dram.dram_user_buffer_start_ptr, dram_mmu_ind_data_len));

    nbytes_done = 0;
    while (nbytes > nbytes_done) {

        loop_offset = 0;
        len = dram_mmu_ind_data_len;

        if (logical2phy_trans == 0x1) {
            res = arad_dram_logical2physical_addr_mapping(unit, buf_num, buf_ndx, &addr_full);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        } else {
            res = arad_dram_mmu_indirect_get_logical_address_full_unsafe(unit, buf_num, buf_ndx, &addr_full);
            SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        }

        loop_offset = (offset + nbytes_done) % dram_mmu_ind_data_len;
        len = SOC_SAND_MIN(dram_mmu_ind_data_len - loop_offset, nbytes - nbytes_done);

        LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): nbytes_done=%d, loop_offset=%d, len=%d, buf_num=%d, buf_ndx=%d, addr_full=0x%x\n"), 
                                                     FUNCTION_NAME(), nbytes_done ,loop_offset, len, buf_num, buf_ndx, addr_full));

        if (write == 0x1) {
            if ((loop_offset != 0x0) || (len != dram_mmu_ind_data_len)) {
                res = arad_dram_mmu_indirect_read_unsafe(unit, logical_mod, addr_full, data);
                SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
            }

            
            sal_memcpy((uint8 *)data + loop_offset, buf + nbytes_done, len);

            res = arad_dram_mmu_indirect_write_unsafe(unit, logical_mod, addr_full, data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
        } else {
            res = arad_dram_mmu_indirect_read_unsafe(unit, logical_mod, addr_full, data);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            
            sal_memcpy(buf + nbytes_done, (uint8 *)data + loop_offset, len);
        }

        nbytes_done += len;

        LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): nbytes_done=%d, buf_logical=0x%x, buf_logical_offset=%d, phy_addr=0x%x\n"), 
                                                    FUNCTION_NAME(), nbytes_done ,buf_num, buf_ndx, addr_full));

        buf_ndx++;
        if ((buf_ndx % max_index) == 0x0) {
            buf_ndx = 0;
            buf_num++;
        }
    }

    
    if (nbytes != nbytes_done) {
        LOG_ERROR(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): nbytes=%d should be equal to nbytes_done=%d\n"), FUNCTION_NAME(), nbytes ,nbytes_done));
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 90, exit);
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "soc_arad_user_buffer_dram_access()", offset, nbytes);
}

uint32
soc_arad_user_buffer_dram_read(
    SOC_SAND_IN         int unit,
    SOC_SAND_IN         uint32 flags,
    SOC_SAND_INOUT      uint8 *buf,
    SOC_SAND_IN         int offset,
    SOC_SAND_IN         int nbytes)
{
    int 
        res,
        access_type = SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_READ;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_arad_user_buffer_dram_access(unit, flags, access_type, buf, offset, nbytes);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "soc_arad_user_buffer_dram_read()", offset, nbytes);
}

uint32
soc_arad_user_buffer_dram_write(
    SOC_SAND_IN         int unit, 
    SOC_SAND_IN         uint32 flags, 
    SOC_SAND_INOUT      uint8 *buf, 
    SOC_SAND_IN         int offset, 
    SOC_SAND_IN         int nbytes)
{
    int 
        res,
        access_type = SOC_ARAD_DRAM_USER_BUFFER_ACCESS_TYPE_WRITE;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    res = soc_arad_user_buffer_dram_access(unit, flags, access_type, buf, offset, nbytes);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "soc_arad_user_buffer_dram_write()", offset, nbytes);
}




uint32
    arad_dram_crc_delete_buffer_enable_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 enable)
{
    int res;
    uint32 reg_val, field_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (enable) {
        field_val =1;
    } else {
        field_val = 0;
    }

    if(SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_DELETE_BUFF_CONFIGSr, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, DDP_DELETE_BUFF_CONFIGSr, &reg_val, DELETE_LAST_BUFF_CRC_ENf, field_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, DDP_DELETE_BUFF_CONFIGSr, REG_PORT_ANY, 0, reg_val));
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CRC_ERROR_D_BUFF_DELETE_CONFIGSr, REG_PORT_ANY, 0, &reg_val));
        soc_reg_field_set(unit, IPT_CRC_ERROR_D_BUFF_DELETE_CONFIGSr, &reg_val, IPT_PKT_CRC_ENf, field_val);
        soc_reg_field_set(unit, IPT_CRC_ERROR_D_BUFF_DELETE_CONFIGSr, &reg_val, MMU_DRAM_CRC_ENf, field_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, IPT_CRC_ERROR_D_BUFF_DELETE_CONFIGSr, REG_PORT_ANY, 0, reg_val));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_crc_delete_buffer_enable()", 0, 0);
}

uint32
    arad_dram_crc_del_buffer_max_reclaims_set_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_IN uint32 max_err)
{
    
    if (SOC_IS_DPP_DRC_COMBO28(unit)) {
        SOC_DPP_CONFIG(unit)->arad->init.drc_info.appl_max_buffer_crc_err = max_err;
    } else {
        SOC_DPP_CONFIG(unit)->arad->init.dram.dram_conf.buffer_mode.appl_max_buffer_crc_err = max_err;
    }
    
    return 0;   
}

uint32
    arad_dram_crc_del_buffer_max_reclaims_get_unsafe(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint32 *max_err)
{
    
    if (SOC_IS_DPP_DRC_COMBO28(unit)) {
        *max_err = SOC_DPP_CONFIG(unit)->arad->init.drc_info.appl_max_buffer_crc_err;
    } else {
        *max_err = SOC_DPP_CONFIG(unit)->arad->init.dram.dram_conf.buffer_mode.appl_max_buffer_crc_err;
    }
    
    return 0;   
}



        
STATIC
uint32
    arad_dram_get_buffer_channel(
        SOC_SAND_IN uint32 devise_id, 
        SOC_SAND_IN uint32 bank, 
        SOC_SAND_OUT char* channel)
{
    int res=0;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

    if(bank >= 64) {
        SOC_SAND_SET_ERROR_CODE(ARAD_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR, 40, exit); 
    }

    *channel = bank/8 + 'A';
    
    return res;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_get_buffer_channel()", 0, 0); 
}



uint32
    arad_dram_buffer_get_info_unsafe(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info)
{
    int res, i=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(SOC_IS_QAX(unit)) {
        buf_info->buf_num =  soc_reg_field_get(unit, DDP_DELETE_BUFF_FIFO_HEADr, buf, DELETED_BUFFER_NUMf);
        buf_info->bank =  0;
    } else {
        buf_info->buf_num =  soc_reg_field_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_HEADr, buf, DELETED_BUFFER_NUMf);
        buf_info->bank =  soc_reg_field_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_HEADr, buf, BANKf);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_dram_get_buffer_channel(unit, buf_info->bank, &buf_info->channel));
    buf_info->err_cntr = 0;
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_sw_db_dram_deleted_buff_list_get(unit, buf, &buf_info->is_deleted));

    for(i=0; i < ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; i++) {

        if (buffers_error_counters[i].buf == buf) {
             buf_info->err_cntr = buffers_error_counters[i].err_cntr;  
             break;
         }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_buffer_get_info_unsafe()", 0, 0);
}

uint32
    arad_dram_buffer_get_info_verify(
            SOC_SAND_IN int unit,
            SOC_SAND_IN  uint32 buf,
            SOC_SAND_OUT arad_dram_buffer_info_t *buf_info)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(buf_info);    
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_buffer_get_info_verify()", 0, 0);
}            



uint32
    arad_dram_delete_buffer_read_fifo_unsafe(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max, 
           SOC_SAND_OUT uint32 *del_buf, 
           SOC_SAND_OUT uint32 *del_buf_count)
{
    int res, cntr, arr_indx;
    uint32 fifo_depth, reg_val, saved_indx_set, saved_indx=0;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    if(SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_DELETE_BUFF_FIFO_STATUSr, REG_PORT_ANY, 0, &reg_val));
        fifo_depth = soc_reg_field_get(unit, DDP_DELETE_BUFF_FIFO_STATUSr, reg_val, DELETE_BUFF_FIFO_STATUSf);

    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_STATUSr, REG_PORT_ANY, 0, &reg_val));
        fifo_depth = soc_reg_field_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_STATUSr, reg_val, CRC_ERR_DELETED_BUFFERS_FIFO_STATUSf);
    }
    
    if (fifo_depth > del_buf_max) {
        fifo_depth = del_buf_max;
    }

    *del_buf_count = fifo_depth;
    
    for (cntr=0 ; cntr < fifo_depth ; ++cntr) {
        if(SOC_IS_QAX(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_DELETE_BUFF_FIFO_HEADr, REG_PORT_ANY, 0, &del_buf[cntr]));
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_CRC_ERR_BUFF_FIFO_FULL_CNTr, REG_PORT_ANY, 0, &reg_val));
        } else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_HEADr, REG_PORT_ANY, 0, &del_buf[cntr]));
            
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CRC_ERR_BUFF_FIFO_FULL_CNTr, REG_PORT_ANY, 0, &reg_val));
        }
         
          
        saved_indx_set =  0;       
        for (arr_indx=0; arr_indx < ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++arr_indx ) {
            
            if (buffers_error_counters[arr_indx].buf == del_buf[cntr] ) {
                ++buffers_error_counters[arr_indx].err_cntr;
                break;
            } else if (!saved_indx_set && (buffers_error_counters[arr_indx].buf == ARAD_DRAM_BUFFER_LIST_NONE)) {
                
                saved_indx = arr_indx;
                saved_indx_set =  1;
            }
                                
        }
        
        if (arr_indx == ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR && saved_indx_set ) {

            buffers_error_counters[saved_indx].buf = del_buf[cntr];
            buffers_error_counters[saved_indx].err_cntr = 1;
                      
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_read_fifo_unsafe()", 0, 0);
}

uint32
    arad_dram_delete_buffer_read_fifo_verify(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf_max,
           SOC_SAND_OUT uint32 *del_buf_array, 
           SOC_SAND_OUT uint32 *del_buf_count)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(del_buf_array); 
    SOC_SAND_CHECK_NULL_INPUT(del_buf_count);    
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_read_fifo_verify()", 0, 0);
}           


uint32
    arad_dram_delete_buffer_test_unsafe(
           SOC_SAND_IN int unit,
           SOC_SAND_IN uint32 del_buf,
           SOC_SAND_OUT uint32 *pass_test)
{
    int res, i, del_buf_num;
    uint32 addr;
    uint32 dram_mmu_ind_data_len;
    uint32 buf_max_index;
    soc_reg_above_64_val_t data[SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE];
    soc_reg_above_64_val_t known_patern[SOC_DPP_DRAM_MMU_IND_ACCESS_MAX_BUFF_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    dram_mmu_ind_data_len = (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) ? SOC_JER_DRAM_MMU_IND_DATA_LEN : SOC_ARAD_DRAM_MMU_IND_DATA_LEN ;
    buf_max_index = SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size / dram_mmu_ind_data_len;

    sal_memset((void*)known_patern, ARAD_DRAM_BUFFER_TEST_PATTERN, sizeof(known_patern));

    if(SOC_IS_QAX(unit)) {
        del_buf_num = soc_reg_field_get(unit, DDP_DELETE_BUFF_FIFO_HEADr, del_buf, DELETED_BUFFER_NUMf);
    } else {
        del_buf_num = soc_reg_field_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_HEADr, del_buf, DELETED_BUFFER_NUMf);
    }

    *pass_test = 1;

    for (i=0; i < buf_max_index; ++i) {

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_dram_mmu_indirect_get_logical_address_full(unit, del_buf_num, i, &addr));

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, arad_dram_mmu_indirect_write(0,1,addr, known_patern));         

        SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, arad_dram_mmu_indirect_read(0,1, addr, data));

        if (sal_memcmp(data, known_patern, dram_mmu_ind_data_len) == 0x1) {
            *pass_test = 0;
             break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_test_unsafe()", 0, 0);
}

uint32
    arad_dram_delete_buffer_test_verify(
           SOC_SAND_IN int unit, 
           SOC_SAND_IN uint32 del_buf,
           SOC_SAND_OUT uint32 *is_pass)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(is_pass);  
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( " arad_dram_delete_buffer_test_verify()", 0, 0);
}           


      

uint32
    arad_dram_delete_buffer_action_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf, 
        SOC_SAND_IN uint32 should_delete)
{
    int res;
    uint32 reg_val, buf_num;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(SOC_IS_QAX(unit)) {
        buf_num = soc_reg_field_get(unit, DDP_DELETE_BUFF_FIFO_HEADr, buf, DELETED_BUFFER_NUMf);
    } else {
        buf_num = soc_reg_field_get(unit, IPT_CRC_ERR_DELETED_BUFFERS_FIFO_HEADr, buf, DELETED_BUFFER_NUMf);
    }
    if (should_delete == 0x0) {
            if(SOC_IS_QAX(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_CPU_RELEASE_BUFF_CONTROLr, REG_PORT_ANY, 0, &reg_val));
                soc_reg_field_set(unit, DDP_CPU_RELEASE_BUFF_CONTROLr, &reg_val, BUFFER_TO_RELEASEf, buf_num);
                soc_reg_field_set(unit, DDP_CPU_RELEASE_BUFF_CONTROLr, &reg_val, CPU_RELEASE_BUFFER_VALIDf, 1);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, DDP_CPU_RELEASE_BUFF_CONTROLr, REG_PORT_ANY, 0, reg_val));
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, REG_PORT_ANY, 0, &reg_val));
                soc_reg_field_set(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, &reg_val, BUFFER_2_RELEASEf, buf_num);
                soc_reg_field_set(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, &reg_val, CPU_RELEASE_BUFFER_VALIDf, 1);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, IPT_CPU_D_BUFF_RELEASE_CONTROLr, REG_PORT_ANY, 0, reg_val));
            }
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_sw_db_dram_deleted_buff_list_remove(unit, buf));

            LOG_DEBUG(BSL_LS_SOC_DRAM,
                      (BSL_META_U(unit,
                                  " buffer: %u was released\n"), buf_num ));
            

        } else {
            if(SOC_IS_QAX(unit)) {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, DDP_CPU_DELETE_BUFF_CONTROLr, REG_PORT_ANY, 0, &reg_val));
                soc_reg_field_set(unit, DDP_CPU_DELETE_BUFF_CONTROLr, &reg_val, BUFFER_TO_DELETEf, buf_num);
                soc_reg_field_set(unit, DDP_CPU_DELETE_BUFF_CONTROLr, &reg_val, CPU_DELETE_BUFFER_VALIDf, 1);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, DDP_CPU_DELETE_BUFF_CONTROLr, REG_PORT_ANY, 0, reg_val));
            } else {
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_get(unit, IPT_CPU_D_BUFF_DELETE_CONTROLr, REG_PORT_ANY, 0, &reg_val));
                soc_reg_field_set(unit, IPT_CPU_D_BUFF_DELETE_CONTROLr, &reg_val, BUFFER_2_DELETEf, buf_num);
                soc_reg_field_set(unit, IPT_CPU_D_BUFF_DELETE_CONTROLr, &reg_val, CPU_DELETE_BUFFER_VALIDf, 1);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, soc_reg32_set(unit, IPT_CPU_D_BUFF_DELETE_CONTROLr, REG_PORT_ANY, 0, reg_val));
            }
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, arad_sw_db_dram_deleted_buff_list_add(unit, buf));

            LOG_DEBUG(BSL_LS_SOC_DRAM,
                      (BSL_META_U(unit,
                                  " buffer: %u was deleted\n"), buf_num ));
        }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_action_unsafe()", 0, 0);  
}

uint32
    arad_dram_delete_buffer_action_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
        
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_delete_buffer_action_verify()", 0, 0);
}        


uint32
    arad_dram_init_buffer_error_cntr_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf)
{
    int i;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(buf == ARAD_DRAM_BUF_NUM_ALL) {
        
        sal_memset(buffers_error_counters, ARAD_DRAM_BUFFER_LIST_NONE, sizeof(buffers_error_counters_t)*ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR);          

#ifdef BCM_WARM_BOOT_SUPPORT  
    
    
        if(SOC_WARM_BOOT(unit)) {            
            uint32 deleted_buff_list[ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR];
            uint32 res, deleted_buff_num;        
            
            if (!SOC_UNIT_NUM_VALID(unit)) {
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
            }
            
            res = arad_sw_db_dram_deleted_buff_list_get_all(unit, deleted_buff_list, ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR, &deleted_buff_num);
            SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
            for(i=0; i<deleted_buff_num; ++i) {
                buffers_error_counters[i].buf = deleted_buff_list[i];
                buffers_error_counters[i].err_cntr = 0;     
            }
        }
#endif 
    } else {
        
        for(i=0; i<ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR; ++i ) {
            if(buffers_error_counters[i].buf == buf) {
                sal_memset(&buffers_error_counters[i], ARAD_DRAM_BUFFER_LIST_NONE, sizeof(buffers_error_counters_t));            
                break;
            }
        }
    }
#ifdef BCM_WARM_BOOT_SUPPORT    
exit:    
#endif   
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_init_buffer_error_cntr_unsafe()", 0, 0);   
}

uint32
    arad_dram_init_buffer_error_cntr_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 buf)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_init_buffer_error_cntr_verify()", 0, 0);
}



uint32
    arad_dram_get_buffer_error_cntr_in_list_index_unsafe(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list, 
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    *is_buf = 0;

    if ((*buf = buffers_error_counters[index_in_list].buf) !=  ARAD_DRAM_BUFFER_LIST_NONE) {
        *is_buf = TRUE;
    }

    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_get_buffer_error_cntr_info_unsafe()", 0, 0);
}

uint32
     arad_dram_get_buffer_error_cntr_in_list_index_verify(
        SOC_SAND_IN int unit, 
        SOC_SAND_IN uint32 index_in_list,
        SOC_SAND_OUT uint32 *is_buf,
        SOC_SAND_OUT uint32 *buf)
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(is_buf);
    SOC_SAND_CHECK_NULL_INPUT(buf);  
    
    SOC_SAND_ERR_IF_ABOVE_MAX(index_in_list, ARAD_DRAM_MAX_BUFFERS_IN_ERROR_CNTR, ARAD_DRAM_WL_OUT_OF_RANGE_ERR, 10, exit);  
    
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "arad_dram_get_buffer_error_cntr_info_verify()", 0, 0);
}        


int soc_arad_validate_dram_address (int unit, uint32 addr)
{
    uint32 nof_bank_bits;
    uint32 nof_dram_interface_bits;
    uint32 nof_columns;
    uint32 nof_columns_temp;
    uint32 nof_column_bits;
    uint32 nof_row_bits;
    uint32 nof_rows;
    uint32 nof_bits_in_mask;
    uint32 bank;
    uint32 dram_interface;
    uint32 column;
    uint32 row;
    uint32 first_bit;
    ARAD_INIT_DDR *dram;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_ARDON(unit)) 
    {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_validate_dram_address(unit, addr));
    } 
    else 
    {
        dram = &SOC_DPP_CONFIG(unit)->arad->init.dram;

        
        nof_bank_bits = 3;
        
        nof_dram_interface_bits = 3;
        
        nof_columns = dram->nof_columns; 

        switch(nof_columns) {
            case ARAD_DRAM_NUM_COLUMNS_256:
                nof_columns = 256;
                break;
            case ARAD_DRAM_NUM_COLUMNS_512:
                nof_columns = 512;
                break;
            case ARAD_DRAM_NUM_COLUMNS_1024:
                nof_columns = 1024;
                break;
            case ARAD_DRAM_NUM_COLUMNS_2048:
                nof_columns = 2048;
                break;
            case ARAD_DRAM_NUM_COLUMNS_4096:
                nof_columns = 4096;
                break;
            case ARAD_DRAM_NUM_COLUMNS_8192:
                nof_columns = 8192;
                break;
            default:
                
                break;
        }

        nof_columns_temp = nof_columns;
        MAX_NOF_BITS_GET(nof_columns_temp, nof_column_bits);
        nof_column_bits -= 4;
        
        nof_rows = dram->nof_rows;
        MAX_NOF_BITS_GET(nof_rows, nof_row_bits);
        nof_bits_in_mask = nof_bank_bits + nof_dram_interface_bits + nof_column_bits + nof_row_bits;

        if ((addr & SOC_SAND_ZERO_BITS_MASK(nof_bits_in_mask, 0)) != 0) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x is invalid - too long\n"), addr));
        }

        first_bit = 0;
        bank = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_bank_bits - 1, first_bit);
        if (bank >= dram->nof_banks) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid bank (bank %d)\n"), addr, bank));
        }

        first_bit += nof_bank_bits;
        dram_interface = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_dram_interface_bits - 1, first_bit);
        if (!dram->is_valid[dram_interface]) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid dram interface (dram interface %c)\n"), addr, 'A' + dram_interface));
        }

        first_bit += nof_dram_interface_bits;
        column = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_column_bits - 1, first_bit);
        if (column >= nof_columns) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid column\n"), addr));
        }

        first_bit += nof_column_bits;
        row = SOC_SAND_GET_BITS_RANGE(addr, first_bit + nof_row_bits - 1, first_bit);
        if (row >= dram->nof_rows) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("address 0x%x directs to invalid row\n"), addr));
        }

        LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s() recieved address: 0x%x directs to dram interface %c, bank number %d, column %d, row %d\n"), FUNCTION_NAME(), addr, 'A' + dram_interface, bank, column<<6, row));
    }
exit:
    SOCDNX_FUNC_RETURN;
}





int soc_ardon_dram_info_verify(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    int 
        i, 
        dram_num_max=0;

    SOCDNX_INIT_FUNC_DEFS;

    SHR_BIT_ITER(drc_info->dram_bitmap, SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max), i) {
        dram_num_max = i;
    }
    if (dram_num_max > SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: max port in dram bitamp=%d is not supported."), FUNCTION_NAME(), dram_num_max));
    }

    if (drc_info->dram_num > SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max) ) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of dram=%d is not supported."),FUNCTION_NAME(), drc_info->dram_num));
    }

    switch (drc_info->nof_columns) {   
    case 512:
    case 1024:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of columns=%d is not supported."), FUNCTION_NAME(), drc_info->nof_columns));
    }

    switch (drc_info->dram_type) {   
    case SHMOO_COMBO28_DRAM_TYPE_DDR4:
    case SHMOO_COMBO28_DRAM_TYPE_GDDR5:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Dram type=%d is not supported."), FUNCTION_NAME(), drc_info->dram_type));
    }

    switch (drc_info->dram_num) {
    case 0:
    case 1:
    case 2:
    case 3:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of drams=%d is not supported."), FUNCTION_NAME(), drc_info->dram_num));
    }

    if (drc_info->dram_freq < 800 || drc_info->dram_freq > 2000) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Dram freq=%d is not supported."), FUNCTION_NAME(), drc_info->dram_freq));
    }

    switch (drc_info->nof_banks) {
    case 8:
    case 16:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of bankse=%d is not supported."), FUNCTION_NAME(), drc_info->nof_banks));
    }

    if (drc_info->dram_param.init_wait_period > 4000) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Dram init wait period=%d is not supported."), FUNCTION_NAME(), drc_info->dram_param.init_wait_period));
    }

    switch (drc_info->ref_clk_mhz) {
    case 100:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: ddr ref clk=%d(MHZ) is not supported."), FUNCTION_NAME(), drc_info->ref_clk_mhz));
    }

    switch (drc_info->auto_tune) {
    case 2:
    case 1:
    case 0:
    case 3:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Dram auto tune=%d is not supported."), FUNCTION_NAME(), drc_info->auto_tune));
    }

    switch (drc_info->bist_enable) {
    case 1:
    case 0:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Bist enable=%d is not supported."), FUNCTION_NAME(), drc_info->bist_enable));
    }
 
    switch (drc_info->gear_down_mode) {
    case 1:
    case 0:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Gear dowm mode=%d is not supported."), FUNCTION_NAME(), drc_info->gear_down_mode));
    }

    switch (drc_info->abi) {
    case 1:
    case 0:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Abi=%d is not supported."), FUNCTION_NAME(), drc_info->abi));
    }

    switch (drc_info->write_dbi) {
    case 1:
    case 0:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Write dbi=%d is not supported."), FUNCTION_NAME(), drc_info->write_dbi));
    }

    switch (drc_info->read_dbi) {
    case 1:
    case 0:
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Read dbi=%d is not supported."), FUNCTION_NAME(), drc_info->read_dbi));
    }

    if (drc_info->cmd_par_latency > 7) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Cmd par latency=%d is not supported."), FUNCTION_NAME(), drc_info->cmd_par_latency));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_dpp_ardon_dram_mmu_set(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    uint32 
        reg32_val,
        field32_val,
        bank_group_topo = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 2 : 0,
        bank_num,
        adress_map_config_gen = 0x1,
        interleaving_bits,
        one_dram_mode,
        dram_num,
        dram_swapping_enable;
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, &reg32_val));

    
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, DRAM_COL_NUMf, 1);
    
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, STOP_MMU_PIPE_INSIDE_DPRCf, 0);

    
    switch(drc_info->nof_columns) {
        case 256:
            field32_val = 0;
            break;
        case 512:
            field32_val = 1;
            break;
        case 1024:
            field32_val = 2;
            break;
        case 2048:
            field32_val = 3;
            break;
        case 4096:
            field32_val = 4;
            break;
        case 8192:
            field32_val = 5;
            break;
        default:
            
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of columns=%d is not supported."), FUNCTION_NAME(), drc_info->nof_columns));
            break;
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, DRAM_COL_NUMf, field32_val);

    

    bank_num = (bank_group_topo == 2) ? 16 : 8;
    interleaving_bits = soc_sand_log2_round_down(bank_num * drc_info->dram_num) + 
        SOC_SAND_MIN(soc_sand_log2_round_down(SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size / 64), adress_map_config_gen);
    
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): ((bank_num=%d) * (drc_info->dram_num=%d)) = %d \n"), FUNCTION_NAME(), 
                          bank_num, drc_info->dram_num, bank_num * drc_info->dram_num));   
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): soc_sand_log2_round_down(bank_num * drc_info->dram_num)=%d\n"), 
                          FUNCTION_NAME(), soc_sand_log2_round_down(bank_num * drc_info->dram_num))); 
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): dbuff_size/64=%d\n"), FUNCTION_NAME(), SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size/64));
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): soc_sand_log2_round_down(dbuff_size/64)=%d, adress_map_config_gen=%d\n"), 
                          FUNCTION_NAME(), soc_sand_log2_round_down(SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size/64), adress_map_config_gen));
     
   LOG_DEBUG(BSL_LS_SOC_DRAM,
             (BSL_META_U(unit,
                         "%s(): interleaving_bits=%d\n"), FUNCTION_NAME(), interleaving_bits));  

    switch(interleaving_bits) {
        case 3:
            field32_val = 0;
            break;
        case 4:
            field32_val = 1;
            break;
        case 5:
            field32_val = 2;
            break;
        case 6:
            field32_val = 3;
            break;
        case 7:
            field32_val = 5;
            break;
        case 8:
            field32_val = 7;
            break;
        case 9:
            field32_val = 9;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of interleaving_bits=%d is not supported."), FUNCTION_NAME(), interleaving_bits));
    }
    LOG_DEBUG(BSL_LS_SOC_DRAM,
              (BSL_META_U(unit,
                          "%s(): field32_val=BANK_INTERLEAVING_MODEf=%d\n"), FUNCTION_NAME(), field32_val));
    SOC_DPP_CONFIG(unit)->arad->init.dram.interleaving_bits = interleaving_bits;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, BANK_INTERLEAVING_MODEf, field32_val);

    

    
    field32_val = (drc_info->dram_num == 3) ? 1 : 0;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, LAYER_PERIODICITYf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 3 : 2;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, READ_FIFOS_ALLOWEDf, field32_val);

    
    field32_val = 15;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, MIN_READ_COMMANDSf, field32_val);

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, REG_PORT_ANY, 0, reg32_val));

    

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, &reg32_val));

    
    switch(drc_info->dram_num) {
    case 0:
        one_dram_mode = 0x0;
        dram_num = 0x4;
        dram_swapping_enable = 0x0;
        break; 
    case 1:
        one_dram_mode = 0x1;
        dram_num = 0x4;
        dram_swapping_enable = 0x1;
        break; 
    case 2:
        one_dram_mode = 0x0;
        dram_num = 0x3;
        dram_swapping_enable = 0x1;
        break;
    case 3:
        one_dram_mode = 0x0;
        dram_num = 0x2;
        dram_swapping_enable = 0x1;
        break;
    case 4:
        one_dram_mode = 0x0;
        dram_num = 0x1;
        dram_swapping_enable = 0x1;
        break;
    default:
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of dram_num=%d is not supported."), FUNCTION_NAME(), drc_info->dram_num));
    }

    
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, DRAM_NUMf, dram_num);

    
    field32_val = one_dram_mode;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, ONE_DRAM_MODEf, field32_val);

    
    field32_val = dram_swapping_enable;
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, DRAM_SWAPPING_ENABLEf, field32_val);

    

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, REG_PORT_ANY, 0, reg32_val));

    

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MMU_DRAM_NUMBER_SWAPPINGr, REG_PORT_ANY, 0, &reg32_val));

    switch(drc_info->dram_num) {
    case 0:
         
        break; 
    case 1:
        
        field32_val = 0x3;
        soc_reg_field_set(unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_0_MAPPINGf, field32_val);
        break; 
    case 2:
        
        field32_val = 0x2;
        soc_reg_field_set(unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_0_MAPPINGf, field32_val);
        field32_val = 0x3;
        soc_reg_field_set(unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, field32_val);
        break;
    case 3:
        
        field32_val = 0x3;
        soc_reg_field_set(unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_0_MAPPINGf, field32_val);
        break;
    case 4:
        
        break;
    default:
        
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of dram_num=%d is not supported."), FUNCTION_NAME(), drc_info->dram_num));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MMU_DRAM_NUMBER_SWAPPINGr, REG_PORT_ANY, 0, reg32_val));

    

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, 0, &reg32_val));

     

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 0xe : 0x1c;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WR_CMD_DISTANCEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 500 : 360;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WRITE_INHIBIT_CNT_SATf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 0x8 : 0x14;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, RD_CMD_DISTANCEf, field32_val);

    

    
    field32_val = 0x0;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, FIELD_26_26f, field32_val);

    

    
    field32_val = 0x1;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ALLOW_CONSECUTIVE_16_BYTEf, field32_val);

    
    field32_val = 0x1;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ADDRESS_MAP_CONFIG_RDf, field32_val);


    
    field32_val = adress_map_config_gen;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ADDRESS_MAP_CONFIG_GENf, field32_val);

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, REG_PORT_ANY, 0, reg32_val));

     

    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, REG_PORT_ANY, 0, &reg32_val));

    
    field32_val = 5;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, SWITCH_RD_PHASE_TO_PRDf, field32_val);
     
    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 1 : 0;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_FIFOS_ALLOWED_BGf, field32_val);

    
    field32_val = 2;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_BG_ALLOWEDf, field32_val);

    
    field32_val = 1;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, ENABLE_DRC_CRDT_COMPENSATEf, field32_val);

    
    field32_val = 1;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, ENABLE_BG_ALGOf, field32_val);

    
    field32_val = bank_group_topo;
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, BANK_GROUP_TOPOf, field32_val);

     SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, REG_PORT_ANY, 0, reg32_val));

     

     SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, MMU_FIFO_SIZESr, REG_PORT_ANY, 0, &reg64_val));

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 4 : 8;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WFAF_HALF_SIZEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 24 : 48;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WAF_HALF_SIZEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 8 : 16;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFDF_SIZEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 8 : 16;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFAF_SIZEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 48 : 96;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RDF_SIZEf, field32_val);

    
    field32_val = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 48 : 96;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RAF_SIZEf, field32_val);

    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, MMU_FIFO_SIZESr, REG_PORT_ANY, 0, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_dpp_jer_dram_mmu_set(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    uint32 
        reg32_val,
        field32_val,
        bank_group_topo = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 2 : 0,
        bank_num = (bank_group_topo == 2) ? 16 : 8,
        adress_map_config_gen = 0x1,
        interleaving_bits = 0,
        dram_num,
        mmu_fifo_size_factor;
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_ADDR_TRANS_CFGr(unit, &reg32_val));

    
    interleaving_bits = soc_sand_log2_round_down(bank_num * drc_info->dram_num) + 
                        SOC_SAND_MIN(soc_sand_log2_round_down(SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size / 256), adress_map_config_gen);

    switch(interleaving_bits) {
        case 3:
            field32_val = 0;
            break;
        case 4:
            field32_val = 1;
            break;
        case 5:
            field32_val = 3;
            break;
        case 6:
            field32_val = 5;
            break;
        case 7:
            field32_val = 7;
            break;
        case 8:
            field32_val = 9;
            break;
        case 9:
            field32_val = 11;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of interleaving_bits=%d is not supported."), FUNCTION_NAME(), interleaving_bits));
    }


    SOC_DPP_CONFIG(unit)->arad->init.dram.interleaving_bits = interleaving_bits;
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, BANK_INTERLEAVING_MODEf, field32_val);
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, INTERLEAVING_RVRSE_MODEf, 0);
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, LAYER_PERIODICITYf, (((drc_info->dram_num == 3) || (drc_info->dram_num == 6)) ? 1 : 0));
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, ADDRESS_MAP_CONFIG_GENf, 1);
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, BANK_GROUP_TOPOf, bank_group_topo);
    soc_reg_field_set (unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, ENABLE_BG_ALGOf, 1);
    soc_reg_field_set(unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, ONE_DRAM_MODEf, 0);

    switch(drc_info->dram_num) {
        case 0:
            dram_num = 0x4;
            break; 
        case 2:
            dram_num = 0x3;
            break;
        case 3:
            dram_num = 0x2;
            break;
        case 4:
            dram_num = 0x1;
            break;
        case 6:
            dram_num = 0x0;
            break;
        case 8:
            dram_num = 0x4;
            break;
        default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of dram_num=%d is not supported."), FUNCTION_NAME(), drc_info->dram_num));
    }

    soc_reg_field_set(unit, ECI_GLOBAL_ADDR_TRANS_CFGr, &reg32_val, DRAM_NUMf, dram_num);

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_ADDR_TRANS_CFGr(unit, reg32_val));


    
    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr(unit, &reg32_val));

    
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_0_MAPPINGf, 0);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 1);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_2_MAPPINGf, 2);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_3_MAPPINGf, 3);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_4_MAPPINGf, 4);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_5_MAPPINGf, 5);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_6_MAPPINGf, 6);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_7_MAPPINGf, 7);
    soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_SWAPPING_ENABLEf, 1);

    
    switch(drc_info->dram_num) {
    case 2: 
        soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 2);
        break;
    case 4: 
        
        if (SHR_BITGET(drc_info->dram_bitmap, 7)) {
            soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 5);
            soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_3_MAPPINGf, 7);
        }
        break;
    case 6: 
        soc_reg_field_set (unit, ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 7);
        break;
    default:
        break;
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_DRAM_NUMBER_SWAPPINGr(unit, reg32_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, &reg32_val));

    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, WAIT_FINISH_CUR_STATE_BEFORE_STOP_FOR_REFRESHf, 1);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, STOP_MMU_PIPE_INSIDE_DPRCf, 0);
    
    switch(drc_info->nof_columns) {
        case 256:
            field32_val = 0;
            break;
        case 512:
            field32_val = 1;
            break;
        case 1024:
            field32_val = 2;
            break;
        case 2048:
            field32_val = 3;
            break;
        case 4096:
            field32_val = 4;
            break;
        case 8192:
            field32_val = 5;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of columns=%d is not supported."), FUNCTION_NAME(), drc_info->nof_columns));
            break;
    }
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, DRAM_COL_NUMf, field32_val);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, READ_FIFOS_ALLOWEDf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 3 : 2);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, MIN_READ_COMMANDSf, 15);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, TRR_USE_ONLY_EVEN_ROWSf, 0);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg32_val, STOP_MMU_PIPE_INSIDE_DPRCf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 0);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_GENERAL_CONFIGURATION_REGISTERr_REG32(unit, reg32_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r(unit, &reg32_val));

    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WRITE_INHIBIT_CNT_SATf, (bank_group_topo == 2) ? 360 : 500);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ADDRESS_MAP_CONFIG_RDf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WR_CMD_DISTANCEf, (bank_group_topo == 2) ? 0x17 : 0x19);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, RD_CMD_DISTANCEf, (bank_group_topo == 2) ? 0x13 : 0xf);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ALLOW_CONSECUTIVE_16_BYTEf, 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, INDIRECT_LOGICAL_MODEf, 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, FBC_WR_PRIORITY_ENABLEf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WR_PRIORITY_MODEf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, START_CNT_AFT_NXT_CMDf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, BYPASS_BAC_BG_BUFFERf, (bank_group_topo == 0) ? 0 : 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, BALLANCE_BG_32_BYTES_SIZEf, 1);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr(unit, reg32_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r(unit, &reg32_val));

    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_FIFOS_ALLOWED_BGf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 2 : 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_BG_ALLOWEDf, 2);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, SWITCH_RD_PHASE_TO_PRDf, 5);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, FBC_WR_ACTIVATE_FIFO_THf, 7);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, FBC_RD_ACTIVATE_FIFO_THf, 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, WAIT_FINISH_CUR_STATE_BEFORE_SWITCH_TO_FBCf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, ENABLE_FBC_SEPERATE_FSMf, 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, RDF_WR_THf, 5);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, DRC_RD_FIFO_THf, 0x20);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r(unit, reg32_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_FIFO_SIZESr(unit, &reg64_val));

    mmu_fifo_size_factor = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 2 ;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WAF_HALF_SIZEf, 24 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WFAF_HALF_SIZEf, 4 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RAF_SIZEf, 32 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RDF_SIZEf, 48 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFAF_SIZEf, 8 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFDF_SIZEf, 8 * mmu_fifo_size_factor);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_FIFO_SIZESr(unit, reg64_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_FIFO_IRDY_THr(unit, &reg64_val));

    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, WAF_HALF_IRDY_THf, 1);
    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, WFAF_HALF_IRDY_THf, 1);
    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, RAF_IRDY_THf, 0);
    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, RDF_IRDY_THf, 0);
    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, RFAF_IRDY_THf, 0);
    soc_reg64_field32_set(unit, MMU_FIFO_IRDY_THr, &reg64_val, RFDF_IRDY_THf, 0);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_FIFO_IRDY_THr(unit, reg64_val));


    

    SOCDNX_IF_ERR_EXIT(READ_MMU_PDC_CONFIGURATION_REGISTERr(unit, &reg64_val));

    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, IDF_PKT_THf, 8190);
    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, LBM_MAX_ENTRIES_PER_DRAMf, 0);
    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, LBM_USE_ONLY_FULL_MCf, 0);
    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, LBM_ENf, 1);
    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_TOTAL_RAF_SIZE_THf, 267);
    soc_reg64_field32_set(unit, MMU_PDC_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_LEAKY_BUCKET_THf, 4);

    SOCDNX_IF_ERR_EXIT(WRITE_MMU_PDC_CONFIGURATION_REGISTERr(unit, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}




static combo28_shmoo_cbi_t _shmoo_cbi = {
    soc_dpp_drc_combo28_shmoo_phy_reg_read, 
    soc_dpp_drc_combo28_shmoo_phy_reg_write,
    soc_dpp_drc_combo28_shmoo_phy_reg_modify,
    soc_dpp_drc_combo28_shmoo_drc_bist_conf_set,
    soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt, 
    soc_dpp_drc_combo28_shmoo_dram_init, 
    soc_dpp_drc_combo28_shmoo_drc_pll_set,
    soc_dpp_drc_combo28_shmoo_modify_mrs,
    soc_dpp_drc_combo28_shmoo_drc_enable_adt,
    soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training,
    soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling,
    soc_dpp_drc_combo28_shmoo_drc_mpr_en,
    soc_dpp_drc_combo28_shmoo_mpr_load,
    soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol,
    soc_dpp_drc_combo28_shmoo_vendor_info_get,
    soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen,
    soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set,
    soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt,
    soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get,
    soc_dpp_drc_combo28_shmoo_enable_wr_crc,
    soc_dpp_drc_combo28_shmoo_enable_rd_crc,
    soc_dpp_drc_combo28_shmoo_enable_wr_dbi,
    soc_dpp_drc_combo28_shmoo_enable_rd_dbi,
    soc_dpp_drc_combo28_shmoo_enable_refresh,
    soc_dpp_drc_combo28_shmoo_force_dqs,
    soc_dpp_drc_combo28_soft_reset_drc_without_dram,
    soc_dpp_drc_combo28_shmoo_dram_info_access,
    soc_dpp_drc_combo28_shmoo_vendor_info_access,
    soc_dpp_drc_combo28_shmoo_cas_latency_get
};


int soc_dpp_drc_initiate_pll_dprc_with_bmap(int unit, soc_dpp_drc_combo28_info_t *drc_info, int is_master)
{
    int drc_ndx;
    uint32 max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
    SHR_BIT_DCL_CLR_NAME(dram_bmp, SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX));

    SOCDNX_INIT_FUNC_DEFS;

    if (is_master) 
    {
        SHR_BITOR_RANGE(drc_info->dram_bitmap, dram_bmp, 0, max_nof_dram_interfaces, dram_bmp);
        SHR_BITAND_RANGE(drc_info->ref_clk_bitmap, dram_bmp, 0, max_nof_dram_interfaces, dram_bmp);
    } else {
        SHR_BITNEGATE_RANGE(drc_info->ref_clk_bitmap, 0, max_nof_dram_interfaces, dram_bmp);
        SHR_BITAND_RANGE(dram_bmp, drc_info->dram_bitmap, 0, max_nof_dram_interfaces, dram_bmp);
    }

    
    SHR_BIT_ITER(dram_bmp, max_nof_dram_interfaces, drc_ndx) {
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_init_dprc_init(unit, drc_ndx, drc_info));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_initiate_phy_drc(int unit, soc_dpp_drc_combo28_info_t *drc_info, int is_master)
{
    int drc_ndx;
    uint32 max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);

    SOCDNX_INIT_FUNC_DEFS;

    
    SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, drc_ndx) {
        if ((is_master == 0x1) && (drc_info->zq_calib_map[drc_ndx] == drc_ndx)) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_init_drc_init(unit, drc_ndx, drc_info));
        } else if ((is_master != 0x1) && (drc_info->zq_calib_map[drc_ndx] != drc_ndx)) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_init_drc_init(unit, drc_ndx, drc_info));
        } else {
            LOG_DEBUG(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Currently loop on is_master=%d. drc_ndx=%d master is %d.\n"),
                FUNCTION_NAME(), is_master, drc_ndx, drc_info->zq_calib_map[drc_ndx]));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_qax_dram_mmu_set(int unit, soc_dpp_drc_combo28_info_t *drc_info) 
{
    uint32 
        reg32_val,
        field32_val,
        bank_group_topo = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 2 : 0,
        bank_num = (bank_group_topo == 2) ? 16 : 8,
        adress_map_config_gen = 0x1,
        interleaving_bits = 0,
        dram_num,
        one_dram_mode,
        mmu_fifo_size_factor;
    uint32 one_16_byte_dram_mode;
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_MMU_GENERAL_CONFIGURATION_REGISTER_2r(unit, &reg32_val));

    
    interleaving_bits = soc_sand_log2_round_down(bank_num * drc_info->dram_num) + 
                        SOC_SAND_MIN(soc_sand_log2_round_down(SOC_DPP_CONFIG(unit)->arad->init.dram.dbuff_size / 256), adress_map_config_gen);

    switch(interleaving_bits) {
        case 3:
            field32_val = 0;
            break;
        case 4:
            field32_val = 1;
            break;
        case 5:
            field32_val = 3;
            break;
        case 6:
            field32_val = 5;
            break;
        case 7:
            field32_val = 7;
            break;
        case 8:
            field32_val = 9;
            break;
        case 9:
            field32_val = 11;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of interleaving_bits=%d is not supported."), FUNCTION_NAME(), interleaving_bits));
    }


    SOC_DPP_CONFIG(unit)->arad->init.dram.interleaving_bits = interleaving_bits;
    soc_reg_field_set (unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, BANK_INTERLEAVING_MODEf, field32_val);
    soc_reg_field_set (unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, INTERLEAVING_RVRSE_MODEf, 0);
    soc_reg_field_set (unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, LAYER_PERIODICITYf, ((drc_info->dram_num == 3) ? 1 : 0));
    soc_reg_field_set (unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, ADDRESS_MAP_CONFIG_GENf, 1);

    switch(drc_info->dram_num) {
        case 0:
            dram_num = 0x4;
            one_dram_mode = 0;
            break;
        case 1:
            dram_num = 0x4;
            one_dram_mode = 1;
            break;
        case 2:
            dram_num = 0x3;
            one_dram_mode = 0;
            break;
        case 3:
            dram_num = 0x2;
            one_dram_mode = 0;
            break;
        default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of dram_num=%d is not supported."), FUNCTION_NAME(), drc_info->dram_num));
    }

    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, DRAM_NUMf, dram_num);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, ONE_DRAM_MODEf, one_dram_mode);
    soc_reg_field_set(unit, MMU_GENERAL_CONFIGURATION_REGISTER_2r, &reg32_val, TRR_USE_ONLY_EVEN_ROWSf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_GENERAL_CONFIGURATION_REGISTER_2r(unit, reg32_val));
    if (!SOC_IS_QUX(unit)) {
        
        SOCDNX_IF_ERR_EXIT(READ_MMU_DRAM_NUMBER_SWAPPINGr(unit, &reg32_val));
        
        soc_reg_field_set (unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_0_MAPPINGf, 0);
        soc_reg_field_set (unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 1);
        soc_reg_field_set (unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_2_MAPPINGf, 2);
        soc_reg_field_set (unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_SWAPPING_ENABLEf, 1);
        
        switch(drc_info->dram_num) {
        case 2: 
            if (SHR_BITGET(drc_info->dram_bitmap, 2)) {
                soc_reg_field_set (unit, MMU_DRAM_NUMBER_SWAPPINGr, &reg32_val, DRAM_1_MAPPINGf, 2);
            }
            break;
        default:
            break;
        }
        SOCDNX_IF_ERR_EXIT(WRITE_MMU_DRAM_NUMBER_SWAPPINGr(unit, reg32_val));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_MMU_GENERAL_CONFIGURATION_REGISTERr(unit, &reg64_val));

    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, WAIT_FINISH_CUR_STATE_BEFORE_STOP_FOR_REFRESHf, 1);
    
    switch(drc_info->nof_columns) {
        case 256:
            field32_val = 0;
            break;
        case 512:
            field32_val = 1;
            break;
        case 1024:
            field32_val = 2;
            break;
        case 2048:
            field32_val = 3;
            break;
        case 4096:
            field32_val = 4;
            break;
        case 8192:
            field32_val = 5;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Error in %s: Number of columns=%d is not supported."), FUNCTION_NAME(), drc_info->nof_columns));
            break;
    }
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, DRAM_COL_NUMf, field32_val);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, READ_FIFOS_ALLOWEDf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? 3 : 2);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, MIN_READ_COMMANDSf, 15);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, STOP_MMU_PIPE_INSIDE_DPRCf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 0);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_TOTAL_RAF_SIZE_THf, 400);
    soc_reg64_field32_set(unit, MMU_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, DRAM_BLOCK_LEAKY_BUCKET_THf, 4);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_GENERAL_CONFIGURATION_REGISTERr(unit, reg64_val));

     
    SOCDNX_IF_ERR_EXIT(READ_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr(unit, &reg32_val));
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WRITE_INHIBIT_CNT_SATf, (bank_group_topo == 2) ? 360 : 500);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ADDRESS_MAP_CONFIG_RDf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WR_CMD_DISTANCEf, (bank_group_topo == 2) ? 0x17 : 0x19);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, RD_CMD_DISTANCEf, (bank_group_topo == 2) ? 0x13 : 0xf);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, ALLOW_CONSECUTIVE_16_BYTEf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, INDIRECT_LOGICAL_MODEf, 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, FBC_WR_PRIORITY_ENABLEf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, WR_PRIORITY_MODEf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr, &reg32_val, DBUFF_SIZEf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONSr(unit, reg32_val));

    SOCDNX_IF_ERR_EXIT( READ_DRCA_GENERAL_CONFIGURATIONSr(unit, &reg32_val));
    one_16_byte_dram_mode = soc_reg_field_get(unit, DRCA_GENERAL_CONFIGURATIONSr, reg32_val, ONE_16_BYTE_DRAM_MODEf);

    
    SOCDNX_IF_ERR_EXIT(READ_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r(unit, &reg32_val));
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_FIFOS_ALLOWED_BGf, (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_DDR4) ? (!one_16_byte_dram_mode) : 0);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, READ_BG_ALLOWEDf, 2);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, SWITCH_RD_PHASE_TO_PRDf, 5);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, BANK_GROUP_TOPOf, bank_group_topo);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, ENABLE_BG_ALGOf, 1);
    soc_reg_field_set(unit, MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r, &reg32_val, ENABLE_DRC_CRDT_COMPENSATEf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_BANK_ACCESS_CONTROLLER_CONFIGURATIONS_2r(unit, reg32_val));

    
    
    
    
    
    
    

    
    SOCDNX_IF_ERR_EXIT(READ_MMU_FIFO_SIZESr(unit, &reg64_val));
    mmu_fifo_size_factor = (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) ? 1 : 2 ;
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WAF_HALF_SIZEf, 12 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, WFAF_HALF_SIZEf, 4 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RAF_SIZEf, 16 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RDF_SIZEf, 16 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFAF_SIZEf, 8 * mmu_fifo_size_factor);
    soc_reg64_field32_set(unit, MMU_FIFO_SIZESr, &reg64_val, RFDF_SIZEf, 8 * mmu_fifo_size_factor);
    SOCDNX_IF_ERR_EXIT(WRITE_MMU_FIFO_SIZESr(unit, reg64_val));

    
    SOCDNX_IF_ERR_EXIT( READ_MMU_IPT_DATA_FIFO_CONFIGURATION_REGISTERr(unit, &reg32_val));
    soc_reg_field_set(unit, MMU_IPT_DATA_FIFO_CONFIGURATION_REGISTERr, &reg32_val, IDF_PKT_THf, 2040);
    SOCDNX_IF_ERR_EXIT( WRITE_MMU_IPT_DATA_FIFO_CONFIGURATION_REGISTERr(unit, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_drc_combo28_dram_init(int unit, soc_dpp_drc_combo28_info_t *drc_info)
{
    int drc_ndx;
    uint32 val;
    int max_nof_dram_interfaces;
    int fsm_vdl_total_drift_dummy[SOC_DPP_DRC_COMBO28_NOF_DQ_BYTES];
    int rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_ARDON(unit) && !SOC_IS_JERICHO(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Invalid Device")));
    }

    
    SOCDNX_NULL_CHECK(drc_info);

    
    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_dram_info_verify,(unit, drc_info)));
    if (!SOC_WARM_BOOT(unit)) {
        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_sw_state_init(unit));

        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_info_prepare(unit, drc_info));

        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_mr_register_info_prepare(unit, drc_info));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_combo28_shmoo_interface_cb_register(unit, _shmoo_cbi));

     
    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_info_struct_convert(unit, drc_info, &drc_info->shmoo_info));
    if (SOC_WARM_BOOT(unit)) {
        
        max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
        SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, drc_ndx){
            rv = soc_dpp_drc_combo28_shmoo_cfg_get(unit, drc_ndx, &(drc_info->shmoo_config_param[drc_ndx]));
            if (rv == SOC_E_NOT_FOUND) {
                LOG_INFO(BSL_LS_SOC_DRAM, (BSL_META_U(unit, "%s(): Configuration was not found, Shmoo dram ndx=%d.\n"), FUNCTION_NAME(), drc_ndx));
            }
        }
    } else {
        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_initiate_pll_dprc_with_bmap(unit, drc_info, 0x1));

        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_initiate_pll_dprc_with_bmap(unit, drc_info, 0x0));

        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_initiate_phy_drc(unit, drc_info, 0x1));

        
        SOCDNX_IF_ERR_EXIT(soc_dpp_drc_initiate_phy_drc(unit, drc_info, 0x0));

        
        max_nof_dram_interfaces = SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max);
        SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, drc_ndx){
            SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_init_tune_ddr(unit, drc_ndx, drc_info));
        }

        
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enable_cdr_mechanism", 0)) {
            SHR_BIT_ITER(drc_info->dram_bitmap, max_nof_dram_interfaces, drc_ndx){
                if (drc_info->dram_type == SHMOO_COMBO28_DRAM_TYPE_GDDR5) {
                    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_init_gddr5_cdr_mechanism(unit, drc_ndx, drc_info)); 
                    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_cdr_monitor_enable_set(unit, drc_ndx, 1));
                    SOCDNX_IF_ERR_EXIT(soc_dpp_drc_combo28_update_dram_reference_fsm_vdl(unit, drc_ndx, drc_info, fsm_vdl_total_drift_dummy));
                }
            }
        }
        
        if (SOC_IS_ARDON(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_ardon_dram_mmu_set(unit, drc_info));
        } else if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_jer_dram_mmu_set(unit, drc_info));
        } else if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_dpp_qax_dram_mmu_set(unit, drc_info));
        }
        
        SOCDNX_IF_ERR_EXIT(arad_dram_init_buffer_error_cntr_unsafe(unit, -1));

        SOCDNX_IF_ERR_EXIT(arad_dram_crc_del_buffer_max_reclaims_get_unsafe(unit, &val));
        if(val != 0) {
            SOCDNX_IF_ERR_EXIT(arad_dram_crc_delete_buffer_enable_unsafe(unit, 1));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

