#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_DIAG



#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/ARAD/arad_diagnostics.h>
#include <soc/dpp/ARAD/arad_api_diagnostics.h>
#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_api_debug.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_nif.h>
#include <soc/dpp/ARAD/arad_debug.h>
#include <soc/dpp/ARAD/arad_reg_access.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_diag.h>






#define ARAD_NOF_DEBUG_HEADER_REGS (2)
#define ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG (512 / 32)
#define ARAD_DIAG_ITMH_BIT_START (992)
#define ARAD_DIAG_ITMH_SIZE_IN_BITS     (32)








debug_signals_t arad_debug_signals = {
    {24,22,5,11},
    {6,11,2,3},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};

debug_signals_t arad_plus_debug_signals = {
    {24,24,5,11},
    {6,11,2,3},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};

debug_signals_t jerichoplus_debug_signals = {
    {37,12,5,17},
    {10,14,3,5},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};

debug_signals_t jericho_debug_signals = {
    {37,14,5,11},
    {10,12,3,3},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};

debug_signals_t qax_debug_signals = {
    {39,13,5,18},
    {10,14,3,3},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};

debug_signals_t qux_debug_signals = {
    {39,13,5,18},
    {10,14,3,5},
    {ARAD_IHP_ID,ARAD_IHB_ID,ARAD_EGQ_ID,ARAD_EPNI_ID},
};






uint32
  arad_diag_ihp_diag_regs_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT soc_reg_t          *reg1,
    SOC_SAND_OUT soc_reg_t          *reg2
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(reg1);
  SOC_SAND_CHECK_NULL_INPUT(reg2);

  *reg1 = INVALIDr;
  *reg2 = INVALIDr;

  if (SOC_IS_JERICHO_PLUS(unit))
  {
      *reg1 = IHP_REG_0110r;
      *reg2 = IHP_REG_0111r;
  }
  else if (SOC_IS_JERICHO(unit))
  {
      *reg1 = IHP_REG_010Cr;
      *reg2 = IHP_REG_010Dr;
  }
  else
  {
      *reg1 = IHP_REG_00E0r;
      *reg2 = IHP_REG_00E1r;
  }
        
      
exit:
      SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_ihb_sample_reg_get()",0,0);
}


uint32
  arad_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    reg32,
    tmp;
  uint32
    res = SOC_SAND_OK;
  soc_reg_t
      ihp_reg1 =   INVALIDr,
      ihp_reg2 =   INVALIDr,
      ihb_00e0r = SOC_IS_JERICHO(unit)? IHB_REG_00FEr: IHB_REG_00E0r;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SAMPLE_ENABLE_SET_UNSAFE);

  SOC_SAND_IF_ERR_EXIT(arad_diag_ihp_diag_regs_get(unit, &ihp_reg1, &ihp_reg2));

  tmp = SOC_SAND_BOOL2NUM(enable);


  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, ihp_reg1, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_TO_REG(ihp_reg1, FIELD_31_31f, tmp, reg32, 15, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, ihp_reg1, REG_PORT_ANY, 0, reg32));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, ihb_00e0r, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_TO_REG(ihb_00e0r, FIELD_31_31f, tmp, reg32, 15, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_set(unit, ihb_00e0r, REG_PORT_ANY, 0, reg32));

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,READ_EPNI_DBG_COMMANDr(unit, SOC_CORE_ALL, &reg32));
  ARAD_FLD_TO_REG(EPNI_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, tmp, reg32, 55, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_EPNI_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,READ_EGQ_DBG_COMMANDr(unit, SOC_CORE_ALL, &reg32));
  ARAD_FLD_TO_REG(EGQ_DBG_COMMANDr, DBG_SAMPLE_ENABLEf, tmp, reg32, 75, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,WRITE_EGQ_DBG_COMMANDr(unit, SOC_CORE_ALL,  reg32));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_sample_enable_set_unsafe()", 0, 0);
}

uint32
  arad_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  uint32
    reg32;
  uint32
    res = SOC_SAND_OK;
  soc_reg_t
      ihp_reg1 =   INVALIDr,
      ihp_reg2 =   INVALIDr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SAMPLE_ENABLE_GET_UNSAFE);
  SOC_SAND_IF_ERR_EXIT(arad_diag_ihp_diag_regs_get(unit, &ihp_reg1, &ihp_reg2));

  SOC_SAND_CHECK_NULL_INPUT(enable);

  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR, soc_reg32_get(unit, ihp_reg1, REG_PORT_ANY, 0, &reg32));
  ARAD_FLD_FROM_REG(ihp_reg1, FIELD_31_31f, *enable, reg32, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_sample_enable_get_unsafe()", 0, 0);
}

#if ARAD_DEBUG

void
  arad_diag_sig_val_print(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8            block_id,
    SOC_SAND_IN  ARAD_PP_DIAG_REG_FIELD  *fld,
    SOC_SAND_IN  uint32           reg_val[ARAD_DIAG_DBG_VAL_LEN],
    SOC_SAND_IN  uint32           flags
  )
{
  uint32
    idx;

  LOG_CLI((BSL_META_U(unit,
                      "%02d:0x%08x:"), block_id,fld->base));
  for (idx = 0; idx < ARAD_DIAG_DBG_VAL_LEN; ++idx)
  {
    LOG_CLI((BSL_META_U(unit,
                        " 0x%08x"), reg_val[idx]));
   
  }
  LOG_CLI((BSL_META_U(unit, "\n")));
}

uint32
  arad_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  int      core,
    SOC_SAND_IN  uint32   flags
  )
{
  uint32
    reg_val[ARAD_DIAG_DBG_VAL_LEN];
  uint32
    i,j,k;
  ARAD_PP_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[ARAD_DIAG_DBG_VAL_LEN];
  uint8
    samp_enable;
  uint32
    res = SOC_SAND_OK;
  debug_signals_t
      *debug_signals;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_SIGNALS_DUMP_UNSAFE);
  
  ARAD_CLEAR(reg_val,uint32,ARAD_DIAG_DBG_VAL_LEN);

 
  res = arad_diag_sample_enable_get_unsafe(unit,&samp_enable);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!samp_enable)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_DIAG_SAMPLE_NOT_ENABLED_ERR, 5, exit);
  }
    
  if(SOC_IS_QUX(unit))
      debug_signals = &qux_debug_signals;
  else if (SOC_IS_QAX(unit))
      debug_signals = &qax_debug_signals;
  else if(SOC_IS_JERICHO_PLUS(unit))
      debug_signals = &jerichoplus_debug_signals;
  else if(SOC_IS_JERICHO(unit))
      debug_signals = &jericho_debug_signals;
  else if(SOC_IS_ARADPLUS(unit))
      debug_signals = &arad_plus_debug_signals;
  else
      debug_signals = &arad_debug_signals;

  for (i = 0; i < BLOCK_NUM; ++i)
  {
    for (j = 0; j <= debug_signals->block_msb_max[i]; ++j)
    {
      for (k = 0; k <= debug_signals->block_lsb_max[i]; ++k)
      {
        ARAD_PP_DIAG_FLD_READ(&fld, core, debug_signals->block_id[i],j,k,ARAD_DIAG_BLK_NOF_BITS-1,0,15);
        arad_diag_sig_val_print(unit,debug_signals->block_id[i],&fld,regs_val,flags);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_diag_signals_dump_unsafe()",0,0);
}


uint32
  arad_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_OUT ARAD_DIAG_LAST_PACKET_INFO *last_packet
  )
{
  uint32
    port_ndx,
    small_buff_ndx,
    port_context,
      fld_ndx,
      reg_ndx,
    reg_val,
    fld_val2,
    fld_val3,
    fld_val4 = 0,
    res = SOC_SAND_OK;
  uint32
    buffer_temp;
  ARAD_PORT_HEADER_TYPE  
      header_type_incoming;
  soc_reg_above_64_val_t
      fld_value,
      reg_above_64_val[ARAD_NOF_DEBUG_HEADER_REGS];
  ARAD_HPU_ITMH_HDR
    itmh;
  uint8* buffer_ptr;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_LAST_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(last_packet);

  ARAD_DIAG_LAST_PACKET_INFO_clear(last_packet);
  arad_ARAD_HPU_ITMH_HDR_clear(&itmh);

  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val[0]);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64_val[1]);
  SOC_REG_ABOVE_64_CLEAR(fld_value);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_IHP_LAST_RECEIVED_HEADER_REG_0r(unit, core, reg_above_64_val[0]));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, READ_IHP_LAST_RECEIVED_HEADER_REG_1r(unit, core, reg_above_64_val[1]));

  
  buffer_ptr = last_packet->buffer + ARAD_NOF_DEBUG_HEADER_REGS * ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG * sizeof(uint32) ;
  for (reg_ndx = 0; reg_ndx < ARAD_NOF_DEBUG_HEADER_REGS; reg_ndx++)
  {
    for (fld_ndx = 0; fld_ndx < ARAD_NOF_FLDS_IN_DEBUG_HEADER_REG; fld_ndx++)
    {
      buffer_temp = reg_above_64_val[reg_ndx][fld_ndx];
      for (small_buff_ndx = sizeof(uint32); small_buff_ndx; --small_buff_ndx)
      {
        *--buffer_ptr = (uint8)(buffer_temp & 0xff);
        buffer_temp >>= 8;
      }
    }
  }

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, core, 0, LAST_PP_PORTf, &fld_val3));
     last_packet->pp_port = fld_val3;

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 17 , exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, core, 0, LAST_PARSER_PROGRAM_POINTERf, &fld_val2 ));
     port_context = fld_val2;

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 19 , exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IHP_LAST_GENERATED_VALUESr, core, 0, LAST_SRC_SYS_PORTf, &fld_val2 ));
     last_packet->src_syst_port = fld_val2;

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20 , exit, ARAD_REG_ACCESS_ERR,READ_IHP_LAST_RECEIVED_PORTr(unit, core, &reg_val));
      ARAD_FLD_FROM_REG(IHP_LAST_RECEIVED_PORTr, LAST_RCVD_VALIDf,fld_val4, reg_val, 22 ,exit);
      last_packet->is_valid = fld_val4;
      ARAD_FLD_FROM_REG(IHP_LAST_RECEIVED_PORTr, LAST_RCVD_PORT_TERMINATION_CONTEXTf,fld_val4, reg_val, 20 ,exit);
      last_packet->tm_port = fld_val4;
      last_packet->packet_size = soc_reg_field_get(unit, IHP_LAST_RECEIVED_PORTr, reg_val, LAST_RCVD_PACKET_SIZEf);

      port_ndx = fld_val4;
    
     res = arad_port_parse_header_type_unsafe(
             unit,
             port_ndx,
             port_context,
             &header_type_incoming
           );
    
     SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    
     last_packet->port_header_type = header_type_incoming;
    
     if (header_type_incoming == ARAD_PORT_HEADER_TYPE_TM)
     {

      
      res = soc_sand_bitstream_get_any_field(
              reg_above_64_val[1],
              ARAD_DIAG_ITMH_BIT_START,
              ARAD_DIAG_ITMH_SIZE_IN_BITS,
              &(itmh.base)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    
      itmh.extention_src_port = 0;
    
      res = arad_hpu_itmh_parse(
            unit,
              &itmh,
              &(last_packet->itmh)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
     }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_last_packet_info_get_unsafe()", 0, 0);
}

uint32
  arad_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_DIAG_LAST_PACKET_INFO_GET_VERIFY);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_diag_last_packet_info_get_verify()", 0, 0);
}

#endif 

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif 
