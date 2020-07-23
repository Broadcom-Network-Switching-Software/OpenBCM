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

#define _ERR_MSG_MODULE_NAME BSL_SOC_MIRROR
#include <shared/bsl.h>



#include <soc/mem.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_action_cmd.h>
#include <soc/dpp/ARAD/arad_api_action_cmd.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_mgmt.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h> 




#define SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,mirror_macro,snoop_macro)  (cmd_type==SOC_TMC_CMD_TYPE_MIRROR ? mirror_macro : snoop_macro)    

#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX                (ARAD_ACTION_NOF_CMD_SIZE_BYTESS-1)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX                (1023)
#define ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN                (0)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX              (0xffffffff)
#define ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN              (0)

#define ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS                      (2)
#define QAX_ACTION_CMD_MIRROR_SNOOP_SIZE_NOF_BITS                (1)

#define ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE 1


#define ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmd_type)              ( SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,(SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_dest_tm_table_offset)), (16 - (SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_dest_tm_table_offset)))))

#define ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmd_type)  ( SNOOP_MIRROR_MACRO_CHOOSER(cmd_type,0,(SOC_DPP_IMP_DEFS_GET(unit,cmd_ipt_snp_mir_cmd_map_snoop_offset))))





















STATIC 
uint8
 is_destination_of_type(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                destination,
      SOC_SAND_IN  uint32                type


      )
{

    return (type & 0xff) == destination>>((type >>8) &0xff);
}




STATIC 
uint8
 is_fap_id_in_local_device(
      SOC_SAND_IN  int                   unit,
      SOC_SAND_IN  uint32                local_fap_id,
      SOC_SAND_IN  uint32                other_fap_id
      )
{
    return (other_fap_id >= local_fap_id && other_fap_id < local_fap_id + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores) ? 1 : 0;

}




STATIC 
uint32
 decoding_command_size(
     SOC_SAND_IN  int                   unit,
     SOC_SAND_IN  uint32                   action_ndx,
     SOC_SAND_IN  uint32 reg,
     SOC_SAND_IN  uint32 field,
     SOC_TMC_ACTION_CMD_SIZE_BYTES *size,
     SOC_SAND_IN uint32 out_of_range_error_inedx
     )
{
  uint32
    res = SOC_SAND_OK,
    size_enc,
    fld_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, field, &fld_val));

    if (SOC_IS_QAX(unit)) {
        *size = (fld_val >> action_ndx) & 1 ? SOC_TMC_ACTION_CMD_SIZE_BYTES_256 : SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
        goto exit;
    }

    size_enc = SOC_SAND_GET_BITS_RANGE(fld_val, (2 * action_ndx) - 1 , (2 * action_ndx) - 2);
    switch (size_enc)
    {
      case 0x0:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
        break;
      case 0x1:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_128;
        break;
      case 0x2:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
          break;
      case 0x3:
        *size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
        break;
      default:
        SOC_SAND_SET_ERROR_CODE(out_of_range_error_inedx, 10, exit);
        break;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in irr_snoop_mirror_size_set()", action_ndx, 0);
}




STATIC 
uint32
 encoding_command_size(
     SOC_SAND_IN  int                   unit,
     SOC_SAND_IN  uint32                   action_ndx,
     SOC_SAND_IN  soc_reg_t reg,
     SOC_SAND_IN  soc_field_t field,
     SOC_SAND_IN  SOC_TMC_ACTION_CMD_SIZE_BYTES size
     )
{
  uint32
    res = SOC_SAND_OK,
    size_enc,
    reg_val[1];

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, field, reg_val));
    if (SOC_IS_QAX(unit)) {
        switch (size) {
            case SOC_TMC_ACTION_CMD_SIZE_BYTES_256:
                size_enc = 0x1;
                break;
            case SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT:
                size_enc = 0x0;
                break;
            default:
                SOC_SAND_SET_ERROR_CODE(ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR, 10, exit);
                break;
        }
        *reg_val &= ~(1 << action_ndx);
        *reg_val |= (size_enc << action_ndx);
    } else {
        switch (size) 
        {
          case SOC_TMC_ACTION_CMD_SIZE_BYTES_64:
            size_enc = 0x0;
            break;
          case SOC_TMC_ACTION_CMD_SIZE_BYTES_128:
            size_enc = 0x1;
            break;
          case SOC_TMC_ACTION_CMD_SIZE_BYTES_192:
            size_enc = 0x2;
            break;
          case SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT:
            size_enc = 0x3;
            break;
          default:
            SOC_SAND_SET_ERROR_CODE(ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR, 10, exit);
            break;
        }

        
        res = soc_sand_bitstream_set_field(reg_val, ((2 * action_ndx) - 2) * (action_ndx>0), ARAD_ACTION_CMD_SNOOP_SIZE_NOF_BITS ,size_enc);
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    }

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, field, *reg_val));



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in irr_snoop_mirror_size_set()", action_ndx, 0);
}




STATIC void dpp_decode_destination_type_and_id(
     SOC_SAND_IN  int             unit,
     SOC_SAND_OUT   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
     SOC_SAND_IN uint32 destination
     )
{
    if (is_destination_of_type(unit, destination, SOC_DPP_IMP_DEFS_GET(unit, mirror_snoop_destination_queue_encoding)))
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_QUEUE;
      info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_QUEUE;
    }
    else if (is_destination_of_type(unit, destination, SOC_DPP_IMP_DEFS_GET(unit,mirror_snoop_destination_multicast_encoding)))
    {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_MULTICAST;
      info->cmd.dest_id.id =  destination & ~INGRESS_DESTINATION_TYPE_MULTICAST;
    }
    else if (is_destination_of_type(unit, destination, SOC_DPP_IMP_DEFS_GET(unit, mirror_snoop_destination_lag_encoding)))
    {
        info->cmd.dest_id.type = ARAD_DEST_TYPE_LAG;
        info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_LAG;
    }
    else {
      info->cmd.dest_id.type = ARAD_DEST_TYPE_SYS_PHY_PORT;
      info->cmd.dest_id.id = destination & ~INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;
    }
}


STATIC uint32 dpp_encode_destination_type_and_id(
     SOC_SAND_IN  int             unit,
     SOC_SAND_IN   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
     SOC_SAND_OUT uint32 *destination
     )
{

    SOCDNX_INIT_FUNC_DEFS;

    switch(info->cmd.dest_id.type)
    {
    case ARAD_DEST_TYPE_QUEUE:
      *destination = info->cmd.dest_id.id  == (uint32)(-1) ?
        (1 << 18) - 1 :                    
        info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_QUEUE;  
      break;
    case ARAD_DEST_TYPE_MULTICAST:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_MULTICAST;
      break;
    case ARAD_DEST_TYPE_SYS_PHY_PORT:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_SYS_PHY_PORT;

      break;

    case ARAD_DEST_TYPE_LAG:
      *destination = info->cmd.dest_id.id | INGRESS_DESTINATION_TYPE_LAG;
      break;
    default:
      SOCDNX_EXIT_WITH_ERR(SOC_E_BADID, (_BSL_SOCDNX_MSG("mirror destination type illegal\n")));
      break;
    }  

exit:
    SOCDNX_FUNC_RETURN;
}
  

uint32
dpp_snoop_mirror_stamping_config_get(
   SOC_SAND_IN  int             unit,
   SOC_SAND_IN  SOC_TMC_CMD_TYPE cmnd_type,
   SOC_SAND_IN  int              cmd,
   SOC_SAND_OUT  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO  *info
   ) {
    uint32 data[11] = {0}, field[3] = {0};
    uint32 res = SOC_E_NONE;
    uint32 entry_offset = (cmd | ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmnd_type));
    uint64 data64;
    uint32 snp_stamp_trap_code;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    if (cmd < 0 || cmd >= SOC_DPP_IMP_DEFS_GET(unit, cmd_ipt_snp_mir_cmd_map_snoop_offset)) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    info->stamping.valid = 0;
    
    res = READ_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_FHEI_EXT_ENf, field);
    if (field[0] | field[1]) { 
        info->stamping.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI;
        
        
        res = READ_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_FHEI_EXTf, field);

        info->stamping.cpu_trap_qualifier = (field[0] >> 8) & 0xfff;
        info->stamping.cpu_trap_code = field[0] & 0xff;
    }

    res = READ_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_BASE_ENf, field);
    if (((field[0] >> 24) & 0x1) != 0) {
        res = READ_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_BASEf, field);
        info->is_out_mirror_disable = (field[0] >> 24) & 0x1;
    }

    if (((field[0] >> 28) & 0x1) != 0) {
        res = READ_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_BASEf, field);
        info->action_type = (field[0] >> 27) & 0x3;
    }

    res = READ_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_EEI_EXT_ENf, field);
    if (field[0] == 0xFFFFFF) {
        res = READ_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
        SOCDNX_IF_ERR_EXIT(res);
        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_EEI_EXTf, field);
        if (field[0] == 0x11) {
            info->is_eei_invalid = TRUE;
        }
    }

    if (cmnd_type == SOC_TMC_CMD_TYPE_SNOOP && 
        !(info->stamping.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI)) {
        
        COMPILER_64_ZERO(data64);
        if (SOC_IS_QAX(unit))
        {
            SOCDNX_IF_ERR_EXIT(READ_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
            snp_stamp_trap_code = soc_reg64_field32_get(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, data64, SNP_STAMP_TRAP_CODEf);
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
            snp_stamp_trap_code = soc_reg64_field32_get(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, data64, SNP_STAMP_TRAP_CODEf);
        }
        if (0x1 & (snp_stamp_trap_code >> cmd)) {
            info->stamping.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI;
            info->stamping.cpu_trap_code = -1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
dpp_snoop_mirror_stamping_config_set(
   SOC_SAND_IN  int              unit,
   SOC_SAND_IN  SOC_TMC_CMD_TYPE cmnd_type,
   SOC_SAND_IN  int              cmd,
   SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO  *info
   ){
    uint32 val[11] = {0};
    uint32 data[11] = {0};
    uint32 snp_stamp_trap_code;
    uint64 data64;
    uint32 res = SOC_E_NONE;
    uint32 entry_offset = (cmd | ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmnd_type));
    uint8  stamp_en;
    uint32 flags = 0;
    uint32 user_header_size, user_header_0_size, user_header_1_size;
    uint32 user_header_egress_pmf_offset_0_dummy, user_header_egress_pmf_offset_1_dummy;

    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_IS_JERICHO(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    if (cmd < 0 || cmd >= SOC_DPP_IMP_DEFS_GET(unit, cmd_ipt_snp_mir_cmd_map_snoop_offset)) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    
    if (info->stamping.cpu_trap_qualifier >= SOC_PPC_NOF_TRAP_QUALIFIERS) {
        SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
    }
    if (cmnd_type == SOC_TMC_CMD_TYPE_SNOOP) {
        if (info->stamping.cpu_trap_code >= SOC_PPC_NOF_TRAP_CODES && info->stamping.cpu_trap_code != -1) {
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
    } else {
        if (info->stamping.cpu_trap_code >= SOC_PPC_NOF_TRAP_CODES) {
            SOCDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
    }

    stamp_en = (info->stamping.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI);

    if (cmnd_type == SOC_TMC_CMD_TYPE_SNOOP) {
        COMPILER_64_ZERO(data64);
        if (SOC_IS_QAX(unit))
        {
            SOCDNX_IF_ERR_EXIT(READ_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
            snp_stamp_trap_code = soc_reg64_field32_get(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, data64, SNP_STAMP_TRAP_CODEf);
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(READ_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, &data64));
            snp_stamp_trap_code = soc_reg64_field32_get(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, data64, SNP_STAMP_TRAP_CODEf);
        }
        
        if (info->stamping.cpu_trap_code == -1 && stamp_en) {
            
            snp_stamp_trap_code |= 1 << cmd;
            stamp_en = FALSE;
        } else {
            snp_stamp_trap_code &= ~(1 << cmd);
        }

        if (SOC_IS_QAX(unit))
        {
            soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &data64, SNP_STAMP_TRAP_CODEf, snp_stamp_trap_code);
            SOCDNX_IF_ERR_EXIT(WRITE_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, data64));
        }
        else
        {
            soc_reg64_field32_set(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, &data64, SNP_STAMP_TRAP_CODEf, snp_stamp_trap_code);
            SOCDNX_IF_ERR_EXIT(WRITE_IPT_STAMPING_FABRIC_HEADER_ENABLEr(unit, data64));
        }
    }

    res = READ_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

    val[0] = 0;
    if (stamp_en) {
        val[0] = 0xFFFFF; 
    }

    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_FHEI_EXT_ENf, val);

    soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_BASE_ENf, val);

    val[0] &= ~(0x1 << 24);
    if (info->is_out_mirror_disable) {
        
        val[0] |= (0x1 << 24);
    }

    val[0] &= ~(0x7FFFF << 4);
    if (info->cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        SOCDNX_IF_ERR_EXIT(bcm_multicast_group_get(unit, info->cmd.dest_id.id, &flags));
    }

    if ((flags & BCM_MULTICAST_EGRESS_GROUP) || ((cmnd_type == SOC_TMC_CMD_TYPE_SNOOP) && (info->stamping.encap_id != 0))) {
        
        val[0] |= (0x7FFFF << 4);
    }

    if ((cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) && ((info->action_type == bcmPktDnxFtmhActionTypeInboundMirror) || (info->action_type == bcmPktDnxFtmhActionTypeOutboundMirror))) {
        val[0] |= (0x3 << 27);
    }

    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_BASE_ENf, val);

    val[0] = 0;
    if ((soc_property_get(unit, spn_STACKING_ENABLE, 0x0) || soc_property_get(unit, spn_FTMH_DSP_EXTENSION_ADD, 0x0)) && !soc_property_get(unit, spn_MIRROR_STAMP_SYS_ON_DSP_EXT, 0x0)) {
        
        val[0] = 0xFFFF;
    }
    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_DSP_EXT_ENf, val);

    if (info->is_eei_invalid == TRUE) {
        val[0] = 0xFFFFFF;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_EEI_EXT_ENf, val);

        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_BASE_ENf, val);
        
        val[0] |= 0xFFFC0000;
        val[1] |= 0xFE03;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_BASE_ENf, val);
    } else {
        val[0] = 0;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_EEI_EXT_ENf, val);

        val[1] = 0;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_BASE_ENf, val);
    }

    res = WRITE_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

    res = READ_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

    val[0] = 0;
    val[1] = 0;
    if (stamp_en) {
        
        

        val[0] = ((info->stamping.cpu_trap_qualifier & 0xfff) << 8) | (0xff & info->stamping.cpu_trap_code);
    }
    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_FHEI_EXTf, val);

    soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_BASEf, val);

    val[0] &= ~(0x1 << 24);
    if (info->is_out_mirror_disable) {
        val[0] |= (0x1 << 24);
    }

    val[0] &= ~(0x7FFFF << 4);
    if ((cmnd_type == SOC_TMC_CMD_TYPE_SNOOP) && (info->stamping.encap_id != 0)) {
        val[0] |= (info->stamping.encap_id & 0x7FFFF) << 4;
    } else if (flags & BCM_MULTICAST_EGRESS_GROUP) {
        val[0] |= (info->cmd.dest_id.id << 4);
    }

    if ((cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) && ((info->action_type == bcmPktDnxFtmhActionTypeInboundMirror) || (info->action_type == bcmPktDnxFtmhActionTypeOutboundMirror))) {
        val[0] |= (info->action_type << 27);
    }

    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_BASEf, val);

    val[0] = 0;
    if ((soc_property_get(unit, spn_STACKING_ENABLE, 0x0) || soc_property_get(unit, spn_FTMH_DSP_EXTENSION_ADD, 0x0)) && !soc_property_get(unit, spn_MIRROR_STAMP_SYS_ON_DSP_EXT, 0x0)) {
        val[0] = info->cmd.dest_id.id & 0xFFFF;
    }
    soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_DSP_EXTf, val);

    if (info->is_eei_invalid == TRUE) {
        val[0] = 0x11;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_EEI_EXTf, val);

        res = arad_pmf_db_fes_user_header_sizes_get(unit,
                                               &user_header_0_size,
                                               &user_header_1_size,
                                               &user_header_egress_pmf_offset_0_dummy,
                                               &user_header_egress_pmf_offset_1_dummy);
        SOCDNX_IF_ERR_EXIT(res);
        user_header_size = (user_header_0_size + user_header_1_size) / 8;

        soc_mem_field_get(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_BASEf, val);
        
        val[0] &= 0x3FFFF;
        val[1] &= 0xFFFF01FC;
        val[1] |= user_header_size << 9;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_BASEf, val);
    } else {
        val[0] = 0;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_EEI_EXTf, val);

        val[1] = 0;
        soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_BASEf, val);
    }

    res = WRITE_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);

#ifdef JERICHO_SPECIFIC_MIRROR_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, OAM_TS_EXT_ENf, value);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_BASE_ENf, value);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_STACK_EXT_ENf, value[0]);

            
            
            

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_EEI_EXT_ENf, value[0]);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_DSP_EXT_ENf, value[0]);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_BASE_ENf, value);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, FTMH_LB_EXT_ENf, value[0]);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_ENm, data, PPH_LEARN_EXT_ENf, value);

            SOCDNX_IF_ERR_EXIT(WRITE_IPT_ARAD_SNP_MIR_STAMP_ENm(unit, MEM_BLOCK_ANY, entry_offset, data));

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_STACK_EXTf, value[0]);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_EEI_EXTf, value[0]);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_BASEf, value);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_DSP_EXTf, value[0]);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, OAM_TS_EXTf, value);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_LEARN_EXTf, value);

            
            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_FHEI_EXTf, value);

            
            soc_mem_field32_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, FTMH_LB_EXTf, value[0]);

            
            soc_mem_field_set(unit, IPT_ARAD_SNP_MIR_STAMP_VALUESm, data, PPH_BASEf, value);

            SOCDNX_IF_ERR_EXIT(WRITE_IPT_ARAD_SNP_MIR_STAMP_VALUESm(unit, MEM_BLOCK_ANY, entry_offset, data));
        }
#endif 
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
uint32
  dpp_snoop_mirror_destination_config_set(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32          entry_offset,
    SOC_SAND_IN   SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                   cmnd_type
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 data[11] = {0};
    soc_mem_t mem;
    uint32 destination;
    uint32 entry_offset_01;
    uint32 value, new_value;
    int field_size;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(dpp_encode_destination_type_and_id(unit, info, &destination));

    
    mem = SOC_IS_QAX(unit) ? TAR_SNOOP_MIRROR_DEST_TABLEm : IRR_SNOOP_MIRROR_DEST_TABLEm;
    soc_mem_field32_set(unit, mem, data, DESTINATIONf, destination);
    soc_mem_field32_set(unit, mem, data, TCf, info->cmd.tc.value );
    soc_mem_field32_set(unit, mem, data, TC_OWf, (uint32)info->cmd.tc.enable );
    soc_mem_field32_set(unit, mem, data, DPf, info->cmd.dp.value );
    soc_mem_field32_set(unit, mem, data, DP_OWf, (uint32)info->cmd.dp.enable );

    if (SOC_IS_QAX(unit) && info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT) {
        soc_mem_field32_set(unit, mem, data, LAG_LB_KEYf, info->trunk_hash_result); 
        soc_mem_field32_set(unit, mem, data, LAG_LB_KEY_OWf, 1);
    }

    res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, data); 
    SOCDNX_IF_ERR_EXIT(res);
    sal_memset(data, 0,sizeof(data));

    if (SOC_IS_QAX(unit)) {
        mem = CGM_SNP_MRR_TM_TABLEm;
        field_size = soc_mem_field_length(unit, mem, IRPP_CTR_PTR_Af);
        if ((info->cmd.counter_ptr_1.value > SOC_SAND_BIT(field_size) && info->cmd.counter_ptr_1.value != SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) || 
            (info->cmd.counter_ptr_2.value > SOC_SAND_BIT(field_size) && info->cmd.counter_ptr_2.value != SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("incorrect value for counter pointer. should be smaller than %u bits\n"), field_size));
        }
        soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_A_OWf, (uint32)info->cmd.counter_ptr_1.enable);
        if (info->cmd.counter_ptr_1.value == SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) {
            
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_A_VALIDf, FALSE);
        } else {
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_A_VALIDf, (uint32)info->cmd.counter_ptr_1.enable);
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_Af, info->cmd.counter_ptr_1.value);
        }
        soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_A_VALID_OWf, (uint32)info->cmd.counter_ptr_1.enable);
        soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_B_OWf, (uint32)info->cmd.counter_ptr_2.enable);
        if (info->cmd.counter_ptr_2.value == SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) {
            
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_B_VALIDf, FALSE);
        } else {
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_B_VALIDf, (uint32)info->cmd.counter_ptr_2.enable);
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_Bf, info->cmd.counter_ptr_2.value);
        }
        soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, data, IRPP_CTR_PTR_B_VALID_OWf, info->cmd.counter_ptr_2.enable);
    } else {
        mem = SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_TM_TABLEm : IRR_SNOOP_MIRROR_TABLE_1m;
        field_size = soc_mem_field_length(unit, mem, COUNTER_PTR_0f);
        if ((info->cmd.counter_ptr_1.value > SOC_SAND_BIT(field_size) && info->cmd.counter_ptr_1.value != SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) || 
            (info->cmd.counter_ptr_2.value > SOC_SAND_BIT(field_size) && info->cmd.counter_ptr_2.value != SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER))
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("incorrect value for counter pointer. should be smaller than %u bits\n"), field_size));
        }
        soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0_OWf, (uint32)info->cmd.counter_ptr_1.enable);
        
        if (info->cmd.counter_ptr_1.value == SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) {
            
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0_UPDATEf, FALSE);
        } else {
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0_UPDATEf, (uint32)info->cmd.counter_ptr_1.enable);
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_0f, info->cmd.counter_ptr_1.value);
        }
        soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1_OWf, (uint32)info->cmd.counter_ptr_2.enable);
        
        if (info->cmd.counter_ptr_2.value == SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) {
            
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1_UPDATEf, FALSE);
        } else {
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1_UPDATEf, (uint32)info->cmd.counter_ptr_2.enable);
            soc_mem_field32_set(unit, mem, data, COUNTER_PTR_1f, info->cmd.counter_ptr_2.value);
        }
    }

    if (SOC_IS_JERICHO(unit)) { 
        
        if (info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_IN_PORT) {
            soc_mem_field32_set(unit, mem, data, IN_PP_PORTf, info->in_port); 
            soc_mem_field32_set(unit, mem, data, IN_PP_PORT_OWf, 1); 
        }
        
        if (info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_VSQ) {
            soc_mem_field32_set(unit, mem, data, ST_VSQ_PTRf, info->vsq); 
            soc_mem_field32_set(unit, mem, data, ST_VSQ_PTR_OWf, 1);
        }

        if (SOC_IS_QAX(unit)) { 
            
        } else { 
            
            if (info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_CNM_CANCEL) {
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, IGNORE_CPf, info->cnm_cancel); 
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, IGNORE_CP_OWf, 1);
            }
            
            if (info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT) {
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, LAG_LB_KEYf, info->trunk_hash_result); 
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, LAG_LB_KEY_OWf, 1);
            }
            
            if (info->valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_ECN_VALUE) {
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, ETH_ENCAPSULATIONf, info->ecn_value);
                soc_mem_field32_set(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, ETH_ENCAPSULATION_OWf, 1);
            }
        }
    }
    entry_offset_01 = SOC_IS_JERICHO(unit) ? entry_offset : ((entry_offset + 16) % 32); 
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset_01,data));

    if (SOC_IS_JERICHO(unit) && cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) {
        
        memset(data, 0, sizeof(data)); 
        soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, data, MIRROR_PROBABILITYf, info->outbound_prob);
        soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, data, CROP_PKTf, info->crop_pkt);
        soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, data, ADD_ORIG_HEADf, info->add_orig_head & 1);
        soc_mem_field32_set(unit, EPNI_REC_CMD_CONFm, data, IS_TRAPf, 0); 
        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_REC_CMD_CONFm(unit,  MEM_BLOCK_ANY, entry_offset, data));
        
        SOCDNX_IF_ERR_EXIT(READ_EPNI_PRGE_MIRROR_TRAP_ENr(unit, 0, &value));
        new_value = ((info->add_orig_head >> 1) & 1); 
        new_value <<= entry_offset; 
        new_value |= value & ~(((uint32)1) << entry_offset);
        if (value != new_value) {
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PRGE_MIRROR_TRAP_ENr(unit, SOC_CORE_ALL, new_value));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}





STATIC
uint32
  dpp_snoop_mirror_destination_config_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  uint32                                 entry_offset,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                       cmnd_type,
    SOC_SAND_OUT   ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
    uint32 res = SOC_SAND_OK;
    uint32 data[5] = {0};
    uint32 destination;
    uint32 entry_offset_01;
    uint8 update;
    soc_mem_t mem = SOC_IS_QAX(unit) ? TAR_SNOOP_MIRROR_DEST_TABLEm : IRR_SNOOP_MIRROR_DEST_TABLEm;

    SOCDNX_INIT_FUNC_DEFS;

    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, data);
    SOCDNX_IF_ERR_EXIT(res);
    destination = soc_mem_field32_get(unit, mem, data, DESTINATIONf);
    dpp_decode_destination_type_and_id(unit, info, destination);

    info->cmd.tc.value   = soc_mem_field32_get(unit, mem, data, TCf);
    info->cmd.tc.enable   = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, TC_OWf));
    info->cmd.dp.value   = soc_mem_field32_get(unit, mem, data, DPf);
    info->cmd.dp.enable   = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, DP_OWf));
    if (SOC_IS_QAX(unit) && soc_mem_field32_get(unit, TAR_SNOOP_MIRROR_DEST_TABLEm, data, LAG_LB_KEY_OWf)) { 
        info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT;
        info->trunk_hash_result = soc_mem_field32_get(unit, TAR_SNOOP_MIRROR_DEST_TABLEm, data, LAG_LB_KEYf);
    }

    mem = SOC_IS_JERICHO(unit) ? (SOC_IS_QAX(unit) ? CGM_SNP_MRR_TM_TABLEm : IRR_SNOOP_MIRROR_TM_TABLEm) : IRR_SNOOP_MIRROR_TABLE_1m;
    entry_offset_01 = SOC_IS_JERICHO(unit) ? entry_offset : (entry_offset + 16) % 32; 
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset_01, data));
    if (SOC_IS_QAX(unit)) {
        info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_A_OWf));
        update = soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_A_VALIDf);
        if ((update == 0) && (info->cmd.counter_ptr_1.enable== 1)) {
            
            info->cmd.counter_ptr_1.value=SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
        } else {
            info->cmd.counter_ptr_1.value = soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_Af);
        }
        
        info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_B_OWf));
        update = soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_B_VALIDf);
        if ((update == 0) && (info->cmd.counter_ptr_2.enable== 1)) {
            
            info->cmd.counter_ptr_2.value=SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
        } else {
            info->cmd.counter_ptr_2.value = soc_mem_field32_get(unit, mem, data, IRPP_CTR_PTR_Bf);
        }
    } else {
        info->cmd.counter_ptr_1.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, COUNTER_PTR_0_OWf));
        update = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_0_UPDATEf);
        if ((update == 0) && (info->cmd.counter_ptr_1.enable== 1)) {
            
            info->cmd.counter_ptr_1.value=SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
        } else {
            info->cmd.counter_ptr_1.value = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_0f);
        }
        info->cmd.counter_ptr_2.enable = SOC_SAND_NUM2BOOL(soc_mem_field32_get(unit, mem, data, COUNTER_PTR_1_OWf));
        update = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_1_UPDATEf);
        if ((update == 0) && (info->cmd.counter_ptr_2.enable== 1)) {
            
            info->cmd.counter_ptr_2.value=SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
        } else {
            info->cmd.counter_ptr_2.value = soc_mem_field32_get(unit, mem, data, COUNTER_PTR_1f);
        }
    }


    
    if (SOC_IS_JERICHO(unit)) {
        
        if (soc_mem_field32_get(unit, mem, data, IN_PP_PORT_OWf)) {
            info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_IN_PORT;
            info->in_port = soc_mem_field32_get(unit, mem, data, IN_PP_PORTf);
        }
        
        if (soc_mem_field32_get(unit, mem, data, ST_VSQ_PTR_OWf)) {
            info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_VSQ;
            info->vsq = soc_mem_field32_get(unit, mem, data, ST_VSQ_PTRf);
        }
 
        if (SOC_IS_QAX(unit)) {
            
        } else {
            
            if (soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, IGNORE_CP_OWf)) {
                info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_CNM_CANCEL;
                info->cnm_cancel = soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, IGNORE_CPf);
            }
            
            if (soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, LAG_LB_KEY_OWf)) {
                info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT;
                info->trunk_hash_result = soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, LAG_LB_KEYf);
            }
            
        if (soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, ETH_ENCAPSULATION_OWf)) {
                info->valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_ECN_VALUE;
                info->ecn_value = soc_mem_field32_get(unit, IRR_SNOOP_MIRROR_TM_TABLEm, data, ETH_ENCAPSULATIONf);
            }
        }
        if (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) {
            SOCDNX_IF_ERR_EXIT(READ_EPNI_REC_CMD_CONFm(unit, MEM_BLOCK_ANY, entry_offset, data));
            info->outbound_prob = soc_mem_field32_get(unit, EPNI_REC_CMD_CONFm, data, MIRROR_PROBABILITYf);
            info->crop_pkt = soc_mem_field32_get(unit, EPNI_REC_CMD_CONFm, data, CROP_PKTf);
            info->add_orig_head = soc_mem_field32_get(unit, EPNI_REC_CMD_CONFm, data, ADD_ORIG_HEADf);
            if (info->add_orig_head == 0) {
                
                SOCDNX_IF_ERR_EXIT(READ_EPNI_PRGE_MIRROR_TRAP_ENr(unit, 0, &destination));
                info->add_orig_head = ((destination >> entry_offset) & 1) << 1; 
            }
            info->is_trap = 0; 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}





uint32
  arad_action_cmd_snoop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                                action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);

  res = arad_action_cmd_mirror_set_unsafe(unit, action_ndx, SOC_TMC_CMD_TYPE_SNOOP, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_set_unsafe()", action_ndx, 0);
}

uint32
  arad_action_cmd_snoop_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_SNOOP_INFO, info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_set_verify()", action_ndx, 0);
}

uint32
  arad_action_cmd_snoop_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_get_verify()", action_ndx, 0);
}


uint32
  arad_action_cmd_snoop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO    *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                     cmd_type
  )
{
  uint32 res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_SNOOP_SET_UNSAFE);

  res = arad_action_cmd_mirror_get_unsafe(unit, action_ndx, cmd_type, info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_snoop_get_unsafe()", action_ndx, 0);
}


uint32 
  arad_action_cmd_mirror_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                            action_ndx, 
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                   cmnd_type,  
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info      
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    mem_entry[2],
    sys_fap_id_self,
    mapped_fap_port_id,
    mapped_fap_id;
    uint32 tm_table_data[5] = {0};
    uint64 ext_stat_ids=0;
    uint32 is_full_pkt[1];
   
    soc_mem_t mem = SOC_IS_QAX(unit) ? ITE_SNP_MIRR_CMD_MAPm : IPT_SNP_MIR_CMD_MAPm;
    soc_reg_t reg;
    soc_field_t field;
    uint32 data[ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE] = {0};

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_SET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(info);

    
    
    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmnd_type) | action_ndx, mem_entry);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);
    soc_mem_field32_set(unit, mem, mem_entry, OUTLIFf, info->cmd.outlif.value);

    
    if ((SOC_IS_JERICHO(unit)) || cmnd_type != SOC_TMC_CMD_TYPE_MIRROR) {
        field = cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? MIRROR_SIZEf : SNOOP_SIZEf;
        reg = SOC_IS_QAX(unit) ? TAR_SNOOP_MIRROR_SIZEr : (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? IRR_MIRROR_SIZEr : IRR_SNOOP_SIZEr);
        res = encoding_command_size(unit, action_ndx, reg, field, info->size); 
        SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
    }

    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))
    {
        
        field = (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) ? IS_FULL_PKT_MIRf : IS_FULL_PKT_SNPf;
        res = soc_reg_above_64_field32_read(unit, IPT_IS_FULL_PKT_SNOOP_MIRRORr, SOC_CORE_ALL, 0, field, is_full_pkt);
        SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
        SHR_BITWRITE(is_full_pkt, action_ndx, (info->size == SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT) ? 1 : 0);
        res = soc_reg_above_64_field32_modify(unit, IPT_IS_FULL_PKT_SNOOP_MIRRORr, SOC_CORE_ALL, 0, field, is_full_pkt[0]);
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
    }

    
    if (info->cmd.dest_id.type== ARAD_DEST_TYPE_SYS_PHY_PORT) {
      
      res = arad_sys_phys_to_local_port_map_get_unsafe(unit, info->cmd.dest_id.id, &mapped_fap_id, &mapped_fap_port_id);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get, (unit, &sys_fap_id_self)));

      if (!SOC_IS_QAX(unit) && !is_fap_id_in_local_device(unit, sys_fap_id_self, mapped_fap_id)) {
          soc_mem_field32_set(unit, IPT_SNP_MIR_CMD_MAPm, mem_entry, DSPf, info->cmd.dest_id.id);
      }
    }

    res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmnd_type) | action_ndx, mem_entry);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR ,dpp_snoop_mirror_destination_config_set(unit, ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx ,info, cmnd_type)); 

    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 50,  exit, ARAD_REG_ACCESS_ERR ,dpp_snoop_mirror_stamping_config_set(unit, cmnd_type, action_ndx, info));
    }

    if (!SOC_IS_QAX(unit)) {  
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf, &fld_val)); 
        SOC_SAND_SET_BIT(fld_val,SOC_SAND_NUM2BOOL(info->cmd.is_ing_mc), action_ndx);
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf,  fld_val));
    }
    
    if (SOC_IS_QAX(unit) && (info->ext_stat_id_update==TRUE))
    {
        
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR, soc_mem_read(unit, CGM_SNP_MRR_TM_TABLEm, MEM_BLOCK_ANY, (ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx),tm_table_data));
        if ((info->ext_stat_id[0] == BCM_MIRROR_DEST_NO_COUNT) && (info->ext_stat_id[1] == BCM_MIRROR_DEST_NO_COUNT))
        {
            
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, tm_table_data, STATISTICS_PTR_OWf, 0);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  105,  exit, ARAD_REG_ACCESS_ERR,soc_mem_write(unit, CGM_SNP_MRR_TM_TABLEm, MEM_BLOCK_ANY, (ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx),tm_table_data));
        }
        else
        {
            ARAD_ACTION_CMD_EXT_COUNTER1_SET(ext_stat_ids, info->ext_stat_id[0]);
            ARAD_ACTION_CMD_EXT_COUNTER2_SET(ext_stat_ids, info->ext_stat_id[1]);
            soc_mem_field32_set(unit, CGM_SNP_MRR_TM_TABLEm, tm_table_data, STATISTICS_PTR_OWf, 1);
            soc_mem_field64_set(unit, CGM_SNP_MRR_TM_TABLEm, tm_table_data, STATISTICS_PTRf, ext_stat_ids);
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_mem_write(unit, CGM_SNP_MRR_TM_TABLEm, MEM_BLOCK_ANY, (ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx),tm_table_data));
        }
    }

    
    mem = (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? IHB_MRR_ACT_PROFILEm : IHB_SNP_ACT_PROFILEm);
    soc_mem_field32_set(unit, mem, data, SAMPLING_PROBABILITYf, info->prob);
    res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, action_ndx, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 95, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_set_unsafe()", action_ndx, 0);
}

uint32
  arad_action_cmd_mirror_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO   *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_MIRROR_INFO, info, 20, exit);

  

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_set_verify()", action_ndx, 0);
}

uint32
  arad_action_cmd_mirror_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   action_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_GET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(action_ndx, ARAD_ACTION_NDX_MIN, ARAD_ACTION_NDX_MAX, ARAD_ACTION_NDX_OUT_OF_RANGE_ERR, 10, exit);

  

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_get_verify()", action_ndx, 0);
}


uint32
  arad_action_cmd_mirror_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                            action_ndx, 
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                  cmnd_type, 
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info      
  )
{
    uint32
        res = SOC_SAND_OK,
        fld_val,
        mem_entry[2];
    soc_mem_t mem = SOC_IS_QAX(unit)? ITE_SNP_MIRR_CMD_MAPm : IPT_SNP_MIR_CMD_MAPm;
    uint32 data[ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE];
    uint32 tm_table_data[5] = {0};
    uint32 stat_ptr_ow;
    uint64 ext_stat_ids=0;
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ACTION_CMD_MIRROR_GET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(info);
    ARAD_ACTION_CMD_MIRROR_INFO_clear(info);

    
    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ARAD_ACTION_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_MIRROR_OFFSET(cmnd_type) | action_ndx, mem_entry);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 1, exit);

    info->cmd.outlif.value = soc_mem_field32_get(unit, mem, mem_entry, OUTLIFf);
    info->cmd.outlif.enable = info->cmd.outlif.value ? 1 : 0;

    res = dpp_snoop_mirror_destination_config_get(unit, ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx, cmnd_type, info);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 2, exit); 
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit)) {
        res = dpp_snoop_mirror_stamping_config_get(unit, cmnd_type, action_ndx, info); 
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit); 
    }

    if (SOC_IS_QAX(unit)) { 
        info->cmd.is_ing_mc = TRUE;
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_INGRESS_MULTICAST_INDICATIONr, REG_PORT_ANY, 0, MIRROR_IS_ING_MCf, &fld_val));
        info->cmd.is_ing_mc = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BIT(fld_val, action_ndx));
    }

    
    mem = (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? IHB_MRR_ACT_PROFILEm : IHB_SNP_ACT_PROFILEm);
    res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, action_ndx, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);
    info->prob = soc_mem_field32_get(unit, IHB_MRR_ACT_PROFILEm, data, SAMPLING_PROBABILITYf);

    
    if (SOC_IS_JERICHO(unit) || cmnd_type != SOC_TMC_CMD_TYPE_MIRROR) {
        res = decoding_command_size(unit, action_ndx, SOC_IS_QAX(unit) ? TAR_SNOOP_MIRROR_SIZEr :
          (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? IRR_MIRROR_SIZEr : IRR_SNOOP_SIZEr),
          cmnd_type == SOC_TMC_CMD_TYPE_MIRROR ? MIRROR_SIZEf : SNOOP_SIZEf, &(info->size), ARAD_ACTION_CMD_SNOOP_SIZE_OUT_OF_RANGE_ERR);
        SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    } else {
        info->size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
    }
    
    if (SOC_IS_QAX(unit))
    {
        
        res = soc_mem_read(unit, CGM_SNP_MRR_TM_TABLEm, MEM_BLOCK_ANY, (ARAD_ACTION_CMD_SNOOP_MIRROR_TABLE_OFFSET(cmnd_type) + action_ndx),tm_table_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit)
        stat_ptr_ow=soc_mem_field32_get(unit, CGM_SNP_MRR_TM_TABLEm, tm_table_data, STATISTICS_PTR_OWf);
        if (stat_ptr_ow == 0)
        {
            
            info->ext_stat_id[0]=BCM_MIRROR_DEST_NO_COUNT;
            info->ext_stat_id[1]=BCM_MIRROR_DEST_NO_COUNT;
        }
        else
        {
            
            soc_mem_field64_get(unit, CGM_SNP_MRR_TM_TABLEm, tm_table_data, STATISTICS_PTRf, &ext_stat_ids);
            info->ext_stat_id[0]=ARAD_ACTION_CMD_EXT_COUNTER1_GET(ext_stat_ids);
            info->ext_stat_id[1]=ARAD_ACTION_CMD_EXT_COUNTER2_GET(ext_stat_ids);
        }
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_action_cmd_mirror_get_unsafe()", action_ndx, 0);
}

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_ACTION_CMD_OVERRIDE_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_OVERRIDE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_OVERRIDE_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->tc), 11, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->dp), 12, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_ptr_low), 13, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_ptr_up), 14, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->meter_dp), 15, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->counter_ptr_1), 16, exit);
  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD_OVERRIDE, &(info->counter_ptr_2), 17, exit);
  
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_SNOOP_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD, &(info->cmd), 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->size, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_SIZE_MAX, ARAD_ACTION_SNOOP_SIZE_OUT_OF_RANGE_ERR, 11, exit);
  
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MIN, ARAD_ACTION_CMD_SNOOP_INFO_SNOOP_PROB_MAX, ARAD_ACTION_SNOOP_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_SNOOP_INFO_verify()",0,0);
}

uint32
  ARAD_ACTION_CMD_MIRROR_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_ACTION_CMD, &(info->cmd), 10, exit);
  
  
  SOC_SAND_ERR_IF_OUT_OF_RANGE(info->prob, ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MIN, ARAD_ACTION_CMD_MIRROR_INFO_MIRROR_PROB_MAX, ARAD_ACTION_MIRROR_PROB_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_ACTION_CMD_MIRROR_INFO_verify()",0,0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

