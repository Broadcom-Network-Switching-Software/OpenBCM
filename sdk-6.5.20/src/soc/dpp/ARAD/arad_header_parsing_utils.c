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

#define _ERR_MSG_MODULE_NAME BSL_SOC_HEADERS



#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>







#define ARAD_HPU_ITMH_PP_HEADER_PRESENT_MSB  31
#define ARAD_HPU_ITMH_PP_HEADER_PRESENT_LSB  30

#define ARAD_HPU_ITMH_IN_MIRROR_FLAG_MSB      29
#define ARAD_HPU_ITMH_IN_MIRROR_FLAG_LSB      29

#define ARAD_HPU_ITMH_SNOOP_CMD_MSB(mode)     ((itmh_prog_mode) ? 7 : 28 ) 
#define ARAD_HPU_ITMH_SNOOP_CMD_LSB(mode)     ((itmh_prog_mode) ? 4 : 25 )     

#define ARAD_HPU_ITMH_TR_CLS_MSB(mode)     ((itmh_prog_mode) ? 3 : 24 )           
#define ARAD_HPU_ITMH_TR_CLS_LSB(mode)     ((itmh_prog_mode) ? 1 : 22 )   

#define ARAD_HPU_ITMH_DP_MSB(mode)     ((itmh_prog_mode) ? 28 : 21 )                  
#define ARAD_HPU_ITMH_DP_LSB(mode)     ((itmh_prog_mode) ? 27 : 20 )                  




#define ARAD_HPU_ITMH_EXT_SRC_PORT_LSB            0            
#define ARAD_HPU_ITMH_EXT_SRC_PORT_MSB(mode)     ((itmh_prog_mode) ? 23 : 19 )          





























 

#define ARAD_PETRA_HPU_ITMH_DESTINATION_LSB        0
#define ARAD_PETRA_HPU_ITMH_DESTINATION_MSB        17

#define ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB  30
#define ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB  30

#define ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB      29
#define ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB      29

#define ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB      24
#define ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB      27

#define ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB        23
#define ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB        23

#define ARAD_PETRA_HPU_ITMH_TR_CLS_LSB             20
#define ARAD_PETRA_HPU_ITMH_TR_CLS_MSB             22

#define ARAD_PETRA_HPU_ITMH_DP_LSB                 18
#define ARAD_PETRA_HPU_ITMH_DP_MSB                 19


#define ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB    13
#define ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB    13

#define ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB       0
#define ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB       12





















uint32 arad_hpu_itmh_fwd_dest_info_build(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_DEST_INFO            destination,
    SOC_SAND_IN  uint8                     is_advanced_mode,   
    SOC_SAND_IN  uint8                     is_extension,            
    SOC_SAND_OUT uint32        *dest)
{

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(dest);

    if (is_advanced_mode)
    {
        if ((SOC_IS_JERICHO(unit))) 
        {
            if (is_extension) 
            {
                switch(destination.type) {
                case ARAD_DEST_TYPE_ISQ_FLOW_ID:
                    *dest =  ( destination.id & 0x1FFFF ) | (0x2 << 21);
                    break;
                case ARAD_DEST_TYPE_OUT_LIF:
                    *dest = ( destination.id & 0x3FFFF ) ;
                    break;
                case ARAD_DEST_TYPE_MULTICAST_FLOW_ID:
                    *dest = ( destination.id & 0x1FFFF ) | ( 0x1 << 21);
                    break;
                default:
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    break;
                }
            } else {
                switch(destination.type) {
                case ARAD_DEST_TYPE_MULTICAST:
                    *dest = destination.id | (0x2 << 17);
                    break;
                case ARAD_DEST_TYPE_SYS_PHY_PORT:
                    arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, dest);
                    *dest |= (0x1 << 16);
                    break;
                case ARAD_DEST_TYPE_FEC_PTR:
                    *dest = destination.id | (0x1 << 17);
                    break;
                case ARAD_DEST_TYPE_QUEUE:
                    *dest = destination.id | ( 0x3 << 17);
                    break;
                default:
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                        break;
                    }
            } 

        } else {

            if (is_extension) 
            {
                switch(destination.type) {
                case ARAD_DEST_TYPE_ISQ_FLOW_ID:
                    *dest =  ( destination.id & 0x1FFFF ) | (0x2 << 21);
                    break;
                case ARAD_DEST_TYPE_OUT_LIF:
                    *dest = ( destination.id & 0x3FFFF ) ;
                    break;
                case ARAD_DEST_TYPE_MULTICAST_FLOW_ID:
                    *dest = ( destination.id & 0x1FFFF ) | ( 0x1 << 21);
                    break;
                default:
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    break;
                }
            } else {
                switch(destination.type) {
                case ARAD_DEST_TYPE_MULTICAST:
                    *dest = destination.id | (0x5 << 16);
                    break;
                case ARAD_DEST_TYPE_SYS_PHY_PORT:
                    arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, dest);
                    *dest |= (0x4 << 16);
                    break;
                case ARAD_DEST_TYPE_FEC_PTR:
                    *dest = destination.id | (0x4 << 15);
                    break;
                case ARAD_DEST_TYPE_QUEUE:
                    *dest = destination.id | ( 0x3 << 17);
                    break;
                default:
                        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                        break;
                }

            }  
        } 

    } else {

        switch(destination.type) {
        case ARAD_DEST_TYPE_MULTICAST:
            *dest = destination.id;
            break;
        case ARAD_DEST_TYPE_ISQ_FLOW_ID:
            *dest = destination.id | (0x4 << 17);
            break;
        case ARAD_DEST_TYPE_OUT_LIF:
            *dest = destination.id | (0x5 << 17);
            break;
        case ARAD_DEST_TYPE_SYS_PHY_PORT:
            arad_ports_logical_sys_id_build(unit, FALSE, 0, 0, destination.id, dest);
            *dest |= (0xc << 16);
            break;
        case ARAD_DEST_TYPE_LAG:
            arad_ports_logical_sys_id_build(unit, TRUE, destination.id, 0, 0, dest);
            *dest |= (0xc << 16);
            break;
        case ARAD_DEST_TYPE_MULTICAST_FLOW_ID:
            *dest = destination.id | ( 0xd << 16);
            break;
        case ARAD_DEST_TYPE_QUEUE:
            *dest = destination.id | ( 0x7 << 17);
            break;
        default:
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
            break;
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_hpu_itmh_fwd_dest_info_build()",destination.type,destination.id);
}

uint32 arad_hpu_itmh_fwd_dest_info_parse(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                  dest_in,
    SOC_SAND_IN  uint8                     is_advanced_mode, 
    SOC_SAND_IN  uint8                     is_extension,  
    SOC_SAND_OUT  ARAD_DEST_INFO            *destination)
{
    uint32
        res,
        ext_type,
        lag_id,
        lag_member_id,
        sys_phys_port_id;
    uint32
        dest;
    uint8
        is_lag_not_phys;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(destination);

    dest = dest_in & 0xFFFFF;

    if (is_advanced_mode)
    {
        if ((SOC_IS_JERICHO(unit))) 
        {
            if (is_extension) 
            {
                ext_type = (( dest_in & 0xFFFFFF ) >> 21 );

                switch (ext_type) {
                case 0x0:
                    destination->type = ARAD_DEST_TYPE_OUT_LIF;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,17,0);
                    break;
                case 0x1:
                    destination->type = ARAD_DEST_TYPE_MULTICAST_FLOW_ID;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                    break;
                case 0x2:
                    destination->type = ARAD_DEST_TYPE_ISQ_FLOW_ID;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                    break;
                default:
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    break;
                }
            } else {
                if ((dest >> 17) == 1) {
                    destination->type = ARAD_DEST_TYPE_FEC_PTR;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);

                } else if ((dest >> 17) == 0x2) {
                    destination->type = ARAD_DEST_TYPE_MULTICAST;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);

                } else if ((dest >> 17) == 0x3 ) {
                    destination->type = ARAD_DEST_TYPE_QUEUE;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                } else if (((dest >> 16) == 0x1 )) {
                    res = arad_ports_logical_sys_id_parse(unit, dest & 0xffff, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
                    destination->type = ARAD_DEST_TYPE_SYS_PHY_PORT;
                    destination->id = sys_phys_port_id;
                }
            }  

        } else {

            if (is_extension) 
            {
                ext_type = (( dest_in & 0xFFFFFF ) >> 21 );

                switch (ext_type) {
                case 0x0:
                    destination->type = ARAD_DEST_TYPE_OUT_LIF;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,17,0);
                    break;
                case 0x1:
                    destination->type = ARAD_DEST_TYPE_MULTICAST_FLOW_ID;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                    break;
                case 0x2:
                    destination->type = ARAD_DEST_TYPE_ISQ_FLOW_ID;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                    break;
                default:
                    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
                    break;
                }
            } else {

                if ((dest >> 15) == 4) {
                    destination->type = ARAD_DEST_TYPE_FEC_PTR;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,14,0);

                } else if ((dest >> 16) == 0x5) {
                    destination->type = ARAD_DEST_TYPE_MULTICAST;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,15,0);

                } else if ((dest >> 17) == 0x3 ) {
                    destination->type = ARAD_DEST_TYPE_QUEUE;
                    destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
                } else if (((dest >> 16) == 0x4 )) {
                    res = arad_ports_logical_sys_id_parse(unit, dest & 0xffff, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
                    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
                    destination->type = ARAD_DEST_TYPE_SYS_PHY_PORT;
                    destination->id = sys_phys_port_id;
                }
            }  
        } 

    } else {

        if ((dest >> 19) == 0) {
            destination->type = ARAD_DEST_TYPE_MULTICAST;
            destination->id = SOC_SAND_GET_BITS_RANGE(dest,18,0);

        } else if ((dest >> 17) == 0x4) {
            destination->type = ARAD_DEST_TYPE_ISQ_FLOW_ID;
            destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);

        } else if ((dest >> 17) == 0x5 ) {
            destination->type = ARAD_DEST_TYPE_OUT_LIF;
            destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);

        } else if ((dest >> 16) == 0xc ) {
            
            res = arad_ports_logical_sys_id_parse(unit, dest & 0xffff, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
            SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

            if (is_lag_not_phys) {
                destination->type = ARAD_DEST_TYPE_LAG;
                destination->id = lag_id;
            } else {
                destination->type = ARAD_DEST_TYPE_SYS_PHY_PORT;
                destination->id = sys_phys_port_id;
            }

        } else if ((dest >> 16 ) == 0xd) {

            destination->type = ARAD_DEST_TYPE_MULTICAST_FLOW_ID;
            destination->id = SOC_SAND_GET_BITS_RANGE(dest,15,0);

        } else if ((dest >> 17) == 0x7) {
            destination->type = ARAD_DEST_TYPE_QUEUE;
            destination->id = SOC_SAND_GET_BITS_RANGE(dest,16,0);
        } else {
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_hpu_itmh_fwd_dest_info_parse()",dest_in,destination->type);
}

 


uint32
  arad_petra_ports_logical_sys_id_build(
    SOC_SAND_IN  uint8 is_lag_not_phys,
    SOC_SAND_IN  uint32  lag_id,
    SOC_SAND_IN  uint32  lag_member_id,
    SOC_SAND_IN  uint32  sys_phys_port_id,
    SOC_SAND_OUT uint32  *sys_logic_port_id
  )
{
  uint32
    constructed_val = 0;
 ARAD_REG_FIELD
    is_lag_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  is_lag_fld.lsb = 12;
  is_lag_fld.msb = 12;

  lag_member_id_fld.lsb = 8;
  lag_member_id_fld.msb = 11;

  lag_id_fld.lsb = 0;
  lag_id_fld.msb = 7;

  phys_port_id_fld.lsb = 0;
  phys_port_id_fld.msb = 11;

  if (is_lag_not_phys)
  {
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(0x1, is_lag_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD (lag_id, lag_id_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD (lag_member_id, lag_member_id_fld);
  }
  else
  {
    constructed_val |= ARAD_FLD_IN_PLACE_OLD(0x0, is_lag_fld);
    constructed_val |= ARAD_FLD_IN_PLACE_OLD (sys_phys_port_id, phys_port_id_fld);
  }

  *sys_logic_port_id = constructed_val;

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_petra_ports_logical_sys_id_build()",0,0);
}


uint32
  arad_petra_ports_logical_sys_id_parse(
    SOC_SAND_IN  uint32  sys_logic_port_id,
    SOC_SAND_OUT uint8 *is_lag_not_phys,
    SOC_SAND_OUT uint32  *lag_id,
    SOC_SAND_OUT uint32  *lag_member_id,
    SOC_SAND_OUT uint32  *sys_phys_port_id
  )
{
  uint32
    parsed_val = sys_logic_port_id;
  ARAD_REG_FIELD
    is_lag_fld,
    lag_member_id_fld,
    lag_id_fld,
    phys_port_id_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  
  is_lag_fld.lsb = 12;
  is_lag_fld.msb = 12;

  lag_member_id_fld.lsb = 8;
  lag_member_id_fld.msb = 11;

  lag_id_fld.lsb = 0;
  lag_id_fld.msb = 7;

  phys_port_id_fld.lsb = 0;
  phys_port_id_fld.msb = 11;


  *is_lag_not_phys = SOC_SAND_NUM2BOOL(ARAD_FLD_FROM_PLACE_OLD(parsed_val, is_lag_fld));
  if (*is_lag_not_phys)
  {
    *lag_id        = ARAD_FLD_FROM_PLACE_OLD (parsed_val, lag_id_fld);
    *lag_member_id = ARAD_FLD_FROM_PLACE_OLD (parsed_val, lag_member_id_fld);
  }
  else
  {
    *sys_phys_port_id = ARAD_FLD_FROM_PLACE_OLD (parsed_val, phys_port_id_fld);
  }

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_petra_ports_logical_sys_id_parse()",0,0);
}


uint32
  arad_petra_hpu_itmh_build_verify(
    SOC_SAND_IN  ARAD_PORTS_ITMH            *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_drop_precedence_verify(info->base.dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_traffic_class_verify(info->base.tr_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.snoop_cmd_ndx, ARAD_MAX_SNOOP_COMMAND_INDEX,
    ARAD_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR, 30, exit
  );


  if (info->extension.enable == TRUE)
  {
    if (info->extension.is_src_sys_port == TRUE)
    {

    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->extension.src_port.id, ARAD_MAX_SYSTEM_PORT_ID,
        ARAD_SYSTEM_PORT_OUT_OF_RANGE_ERR, 60, exit
      );
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_petra_hpu_itmh_build_verify()",0,0);
}


uint32
  arad_petra_hpu_itmh_build(
    SOC_SAND_IN  ARAD_PORTS_ITMH            *info,
    SOC_SAND_OUT ARAD_HPU_ITMH_HDR        *itmh
  )
{
  uint32
    res,
    sys_port,
    dest;
  uint32
    itmh_hdr_base = 0x0,
    itmh_hdr_ext_src_port = 0x0;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(itmh);

  res = arad_petra_hpu_itmh_build_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10,exit);

  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.pp_header_present,
      ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB,
      ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.out_mirror_dis,
      ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB,
      ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.snoop_cmd_ndx,
      ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB,
      ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.exclude_src,
      ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB,
      ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.tr_cls,
      ARAD_PETRA_HPU_ITMH_TR_CLS_MSB,
      ARAD_PETRA_HPU_ITMH_TR_CLS_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.dp,
      ARAD_PETRA_HPU_ITMH_DP_MSB,
      ARAD_PETRA_HPU_ITMH_DP_LSB
    );

  switch(info->base.destination.type)
  {
  case ARAD_DEST_TYPE_QUEUE:
    dest = info->base.destination.id | ( 3 << 16);
    break;
  case ARAD_DEST_TYPE_MULTICAST:
    dest = info->base.destination.id | ( 2 << 16);
    break;
  case ARAD_DEST_TYPE_SYS_PHY_PORT:
    arad_petra_ports_logical_sys_id_build(FALSE, 0, 0, info->base.destination.id, &dest);
    break;
  case ARAD_DEST_TYPE_LAG:
    arad_petra_ports_logical_sys_id_build(TRUE, info->base.destination.id, 0, 0, &dest);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    break;
  }
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      dest,
      ARAD_PETRA_HPU_ITMH_DESTINATION_MSB,
      ARAD_PETRA_HPU_ITMH_DESTINATION_LSB
    );

  if (info->extension.enable == TRUE)
  {
    itmh_hdr_ext_src_port |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.is_src_sys_port,
        ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB,
        ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB
      );

    if (info->extension.src_port.type == ARAD_DEST_SYS_PORT_TYPE_LAG)
    {
      res = arad_petra_ports_logical_sys_id_build(
              TRUE,
              info->extension.src_port.id,
              info->extension.src_port.member_id,
              0,
              &sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else  
    {
      res = arad_petra_ports_logical_sys_id_build(
              FALSE,
              0,
              0,
              info->extension.src_port.id,
              &sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    itmh_hdr_ext_src_port |=
      SOC_SAND_SET_BITS_RANGE(
        sys_port,
        ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB,
        ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB
      );
  }

  itmh->extention_src_port = itmh_hdr_ext_src_port;
  itmh->base               = itmh_hdr_base;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_petra_hpu_itmh_build()",0,0);
}


uint32
  arad_petra_hpu_itmh_parse(
    SOC_SAND_IN  ARAD_HPU_ITMH_HDR        *itmh,
    SOC_SAND_OUT ARAD_PORTS_ITMH            *info
  )
{
  uint32
    res,
    itmh_hdr_base,
    itmh_hdr_ext_src_port,
    src_port,
    dest,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);

  SOC_SAND_CHECK_NULL_INPUT(itmh);
  SOC_SAND_CHECK_NULL_INPUT(info);

  itmh_hdr_base = itmh->base;
  itmh_hdr_ext_src_port = itmh->extention_src_port;

  info->base.pp_header_present =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB,
        ARAD_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB
      )
    );
  info->base.out_mirror_dis =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB,
        ARAD_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB
      )
    );
  info->base.snoop_cmd_ndx =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB,
      ARAD_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB
    );
  info->base.exclude_src =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB,
        ARAD_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB
      )
    );
  info->base.tr_cls =
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        ARAD_PETRA_HPU_ITMH_TR_CLS_MSB,
        ARAD_PETRA_HPU_ITMH_TR_CLS_LSB
      );
  info->base.dp =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      ARAD_PETRA_HPU_ITMH_DP_MSB,
      ARAD_PETRA_HPU_ITMH_DP_LSB
    );

  dest =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      ARAD_PETRA_HPU_ITMH_DESTINATION_MSB,
      ARAD_PETRA_HPU_ITMH_DESTINATION_LSB
    );
  info->base.destination.id = SOC_SAND_GET_BITS_RANGE(dest,15,0);
  switch(dest >> 16)
  {
  case 3:
    info->base.destination.type = ARAD_DEST_TYPE_QUEUE;
    break;
  case 2:
    info->base.destination.type = ARAD_DEST_TYPE_MULTICAST;
    break;
  default:
    res = arad_petra_ports_logical_sys_id_parse(
      info->base.destination.id, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
    if(is_lag_not_phys)
    {
      info->base.destination.type = ARAD_DEST_TYPE_LAG;
      info->base.destination.id = lag_id;
    }
    else
    {
      info->base.destination.type = ARAD_DEST_TYPE_SYS_PHY_PORT;
      info->base.destination.id = sys_phys_port_id;
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
    break;
  }
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
    dest,
    ARAD_PETRA_HPU_ITMH_DESTINATION_MSB,
    ARAD_PETRA_HPU_ITMH_DESTINATION_LSB
    );


  info->extension.is_src_sys_port =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_ext_src_port,
        ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB,
        ARAD_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB
      )
    );

  src_port =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_ext_src_port,
      ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB,
      ARAD_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB
    );

   res = arad_petra_ports_logical_sys_id_parse(
           src_port,
           &is_lag_not_phys,
           &lag_id,
           &lag_member_id,
           &sys_phys_port_id
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   if (is_lag_not_phys)
   {
     info->extension.src_port.type = ARAD_DEST_SYS_PORT_TYPE_LAG;
     info->extension.src_port.id = lag_id;
     info->extension.src_port.member_id = lag_member_id;
   }
   else 
   {
     info->extension.src_port.type = ARAD_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
     info->extension.src_port.id = sys_phys_port_id;
     info->extension.src_port.member_id = 0;
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_petra_hpu_itmh_parse()",0,0);
}

 


uint32 arad_hpu_itmh_build_verify(
    SOC_SAND_IN  ARAD_PORTS_ITMH            *info)
{
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_HPU_ITMH_BUILD_VERIFY);

    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_drop_precedence_verify(info->base.dp);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = arad_traffic_class_verify(info->base.tr_cls);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(info->base.snoop_cmd_ndx, ARAD_MAX_SNOOP_COMMAND_INDEX, ARAD_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR, 30, exit);



exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_hpu_itmh_build_verify()",0,0);
}


uint32 arad_hpu_itmh_build(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PORTS_ITMH            *info,
    SOC_SAND_OUT ARAD_HPU_ITMH_HDR        *itmh)
{
    uint32
        res,
        dest;
    uint32
        itmh_hdr_base = 0x0,
        itmh_hdr_ext = 0x0;
    uint8
        itmh_prog_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_HPU_ITMH_BUILD);

    SOC_SAND_CHECK_NULL_INPUT(info);
    SOC_SAND_CHECK_NULL_INPUT(itmh);

    itmh_prog_mode = (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) || 
                     (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "support_petra_itmh", 0))) ? TRUE : FALSE ; 

             
        res = arad_hpu_itmh_build_verify(info);
    
        SOC_SAND_CHECK_FUNC_RESULT(res, 10,exit);

        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(info->base.pp_header_present, ARAD_HPU_ITMH_PP_HEADER_PRESENT_MSB, ARAD_HPU_ITMH_PP_HEADER_PRESENT_LSB);
        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(info->base.out_mirror_dis, ARAD_HPU_ITMH_IN_MIRROR_FLAG_MSB, ARAD_HPU_ITMH_IN_MIRROR_FLAG_LSB);
        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(info->base.snoop_cmd_ndx, ARAD_HPU_ITMH_SNOOP_CMD_MSB(itmh_prog_mode), ARAD_HPU_ITMH_SNOOP_CMD_LSB(itmh_prog_mode));
        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(info->base.tr_cls, ARAD_HPU_ITMH_TR_CLS_MSB(itmh_prog_mode), ARAD_HPU_ITMH_TR_CLS_LSB(itmh_prog_mode));
        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(info->base.dp, ARAD_HPU_ITMH_DP_MSB(itmh_prog_mode), ARAD_HPU_ITMH_DP_LSB(itmh_prog_mode));


        arad_hpu_itmh_fwd_dest_info_build(unit, info->base.destination, itmh_prog_mode, FALSE,  &dest);
        SOC_SAND_CHECK_FUNC_RESULT(res, 30,exit);

        itmh_hdr_base |= SOC_SAND_SET_BITS_RANGE(dest, ARAD_HPU_ITMH_DESTINATION_MSB(itmh_prog_mode), ARAD_HPU_ITMH_DESTINATION_LSB(itmh_prog_mode));

        if (info->extension.enable == TRUE) {

            itmh_hdr_base |= ( itmh_prog_mode ) ? 0x1 : 0x0;
            dest = 0;
            arad_hpu_itmh_fwd_dest_info_build(unit, info->extension.destination, itmh_prog_mode, TRUE,  &dest);
            SOC_SAND_CHECK_FUNC_RESULT(res, 40,exit);

            itmh_hdr_ext |= SOC_SAND_SET_BITS_RANGE(dest, ARAD_HPU_ITMH_EXT_SRC_PORT_MSB(itmh_prog_mode), ARAD_HPU_ITMH_EXT_SRC_PORT_LSB);
        }

        itmh->extention_src_port = itmh_hdr_ext;
        itmh->base      = itmh_hdr_base;
 

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_hpu_itmh_build()",0,0);
}
 

uint32 arad_hpu_itmh_parse(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_HPU_ITMH_HDR        *itmh,
    SOC_SAND_OUT ARAD_PORTS_ITMH            *info)
{
    uint32
        res,
        itmh_hdr_base,
        dest;
    uint8
        itmh_prog_mode;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_HPU_ITMH_PARSE);

    SOC_SAND_CHECK_NULL_INPUT(itmh);
    SOC_SAND_CHECK_NULL_INPUT(info);

    itmh_prog_mode = (soc_property_get(unit, spn_ITMH_PROGRAMMABLE_MODE_ENABLE, FALSE) ||
                     (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "support_petra_itmh", 0))) ? TRUE : FALSE ; 

         

        itmh_hdr_base = itmh->base;

        info->base.pp_header_present = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(itmh_hdr_base, ARAD_HPU_ITMH_PP_HEADER_PRESENT_MSB, ARAD_HPU_ITMH_PP_HEADER_PRESENT_LSB));
        info->base.out_mirror_dis = SOC_SAND_NUM2BOOL(SOC_SAND_GET_BITS_RANGE(itmh_hdr_base, ARAD_HPU_ITMH_IN_MIRROR_FLAG_MSB, ARAD_HPU_ITMH_IN_MIRROR_FLAG_LSB));
        info->base.snoop_cmd_ndx = SOC_SAND_GET_BITS_RANGE(itmh_hdr_base,ARAD_HPU_ITMH_SNOOP_CMD_MSB(itmh_prog_mode), ARAD_HPU_ITMH_SNOOP_CMD_LSB(itmh_prog_mode));
        info->base.tr_cls =SOC_SAND_GET_BITS_RANGE(itmh_hdr_base,ARAD_HPU_ITMH_TR_CLS_MSB(itmh_prog_mode),ARAD_HPU_ITMH_TR_CLS_LSB(itmh_prog_mode));
        info->base.dp =SOC_SAND_GET_BITS_RANGE(itmh_hdr_base,ARAD_HPU_ITMH_DP_MSB(itmh_prog_mode),ARAD_HPU_ITMH_DP_LSB(itmh_prog_mode));

        dest = SOC_SAND_GET_BITS_RANGE(itmh_hdr_base,ARAD_HPU_ITMH_DESTINATION_MSB(itmh_prog_mode),ARAD_HPU_ITMH_DESTINATION_LSB(itmh_prog_mode));
        res = arad_hpu_itmh_fwd_dest_info_parse(unit, dest, itmh_prog_mode, FALSE, &(info->base.destination));
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
 
        if(info->base.destination.type == ARAD_DEST_TYPE_ISQ_FLOW_ID ||
            info->base.destination.type == ARAD_DEST_TYPE_OUT_LIF ||
            info->base.destination.type == ARAD_DEST_TYPE_MULTICAST_FLOW_ID || 
            ((itmh_prog_mode) && SOC_SAND_GET_BITS_RANGE(itmh_hdr_base,0,0)) ) {
            info->extension.enable = TRUE;
            dest = SOC_SAND_GET_BITS_RANGE(itmh->extention_src_port, ARAD_HPU_ITMH_EXT_SRC_PORT_MSB(itmh_prog_mode), ARAD_HPU_ITMH_EXT_SRC_PORT_LSB);
            arad_hpu_itmh_fwd_dest_info_parse(unit, dest, itmh_prog_mode, TRUE, &(info->extension.destination));
            SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }
        
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_hpu_itmh_parse()",0,0);
}


void
  arad_ARAD_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_ITMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_BASE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_BASE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXTENSIONS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1

void
  arad_ARAD_HPU_ITMH_HDR_print(
    SOC_SAND_IN  ARAD_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_ITMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_BASE_print(
    SOC_SAND_IN  ARAD_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_BASE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN  ARAD_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_print(
    SOC_SAND_IN  ARAD_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_FTMH_HDR_print(
    SOC_SAND_IN  ARAD_HPU_FTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_BASE_print(
    SOC_SAND_IN  ARAD_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_BASE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN  ARAD_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN  ARAD_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN  ARAD_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXTENSIONS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_print(
    SOC_SAND_IN  ARAD_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_HPU_OTMH_HDR_print(
    SOC_SAND_IN  ARAD_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

