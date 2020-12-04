/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/







#include <shared/bsl.h>
#include <soc/dpp/drv.h>



#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>




#define SOC_SAND_PP_DEST_TYPE_DROP_BASE               (0xffff)
#define SOC_SAND_PP_DEST_TYPE_DROP_LSB                (0)
#define SOC_SAND_PP_DEST_TYPE_DROP_MSB                (15)
#define SOC_SAND_PP_DEST_TYPE_DROP_SHIFT              (SOC_SAND_PP_DEST_TYPE_DROP_LSB)
#define SOC_SAND_PP_DEST_TYPE_DROP_MASK               (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_DROP_MSB, SOC_SAND_PP_DEST_TYPE_DROP_LSB))
                                                  
#define SOC_SAND_PP_DEST_TYPE_FLOW_BASE               (0xC000)
#define SOC_SAND_PP_DEST_TYPE_FLOW_LSB                (0)
#define SOC_SAND_PP_DEST_TYPE_FLOW_MSB                (13)
#define SOC_SAND_PP_DEST_TYPE_FLOW_SHIFT              (SOC_SAND_PP_DEST_TYPE_FLOW_LSB)
#define SOC_SAND_PP_DEST_TYPE_FLOW_MASK               (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_FLOW_MSB, SOC_SAND_PP_DEST_TYPE_FLOW_LSB))
                                                  
#define SOC_SAND_PP_DEST_TYPE_FEC_BASE                (0x8000)
#define SOC_SAND_PP_DEST_TYPE_FEC_LSB                 (0)
#define SOC_SAND_PP_DEST_TYPE_FEC_MSB                 (13)
#define SOC_SAND_PP_DEST_TYPE_FEC_SHIFT               (SOC_SAND_PP_DEST_TYPE_FEC_LSB)
#define SOC_SAND_PP_DEST_TYPE_FEC_MASK                (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_FEC_MSB, SOC_SAND_PP_DEST_TYPE_FEC_LSB))
                                                  
#define SOC_SAND_PP_DEST_TYPE_MC_BASE                 (0x4000)
#define SOC_SAND_PP_DEST_TYPE_MC_LSB                  (0)
#define SOC_SAND_PP_DEST_TYPE_MC_MSB                  (13)
#define SOC_SAND_PP_DEST_TYPE_MC_SHIFT                (SOC_SAND_PP_DEST_TYPE_MC_LSB)
#define SOC_SAND_PP_DEST_TYPE_MC_MASK                 (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_MC_MSB, SOC_SAND_PP_DEST_TYPE_MC_LSB))
                                                  
#define SOC_SAND_PP_DEST_TYPE_TRAP_BASE               (0x2000)
#define SOC_SAND_PP_DEST_TYPE_TRAP_LSB                (0)
#define SOC_SAND_PP_DEST_TYPE_TRAP_MSB                (12)
#define SOC_SAND_PP_DEST_TYPE_TRAP_SHIFT              (SOC_SAND_PP_DEST_TYPE_TRAP_LSB)
#define SOC_SAND_PP_DEST_TYPE_TRAP_MASK               (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_TRAP_MSB, SOC_SAND_PP_DEST_TYPE_TRAP_LSB))
                                                  
#define SOC_SAND_PP_DEST_TYPE_LAG_BASE                (0x1000)
#define SOC_SAND_PP_DEST_TYPE_LAG_LSB                 (0)
#define SOC_SAND_PP_DEST_TYPE_LAG_MSB                 (11)
#define SOC_SAND_PP_DEST_TYPE_LAG_SHIFT               (SOC_SAND_PP_DEST_TYPE_LAG_LSB)
#define SOC_SAND_PP_DEST_TYPE_LAG_MASK                (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_LAG_MSB, SOC_SAND_PP_DEST_TYPE_LAG_LSB))
                                                    
#define SOC_SAND_PP_DEST_TYPE_PORT_BASE               (0x0000)
#define SOC_SAND_PP_DEST_TYPE_PORT_LSB                (0)
#define SOC_SAND_PP_DEST_TYPE_PORT_MSB                (11)
#define SOC_SAND_PP_DEST_TYPE_PORT_SHIFT              (SOC_SAND_PP_DEST_TYPE_PORT_LSB)
#define SOC_SAND_PP_DEST_TYPE_PORT_MASK               (SOC_SAND_BITS_MASK(SOC_SAND_PP_DEST_TYPE_PORT_MSB, SOC_SAND_PP_DEST_TYPE_PORT_LSB))

















uint32
  soc_sand_pp_ipv4_subnet_verify(
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET *subnet
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_IPV4_SUBNET_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(subnet);


  if (subnet->prefix_len > SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PP_IPV4_SUBNET_PREF_OUT_OF_RANGE_ERR,10,exit);
  }
  if (subnet->prefix_len == 0 &&  subnet->ip_address != 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PP_IPV4_SUBNET_PREF_OUT_OF_RANGE_ERR,20,exit);
  }
  exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pp_ipv4_subnet_verify()",0,0);

}

uint32
  SOC_SAND_PP_TRILL_DEST_verify(
    SOC_SAND_IN  SOC_SAND_PP_TRILL_DEST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_SAND_PP_TRILL_DEST_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_sand_pp_ipv4_subnet_verify()",0,0);

}
void
  soc_sand_SAND_PP_DESTINATION_ID_clear(
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_DESTINATION_ID));
  info->dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
  info->dest_val = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_COS_PARAMS_clear(
    SOC_SAND_OUT SOC_SAND_PP_COS_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_COS_PARAMS));
  info->drop_precedence = 0;
  info->traffic_class = 0;
  info->user_priority = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_SYS_PORT_ID_clear(
    SOC_SAND_OUT SOC_SAND_PP_SYS_PORT_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_SYS_PORT_ID));
  info->sys_port_type = SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT;
  info->sys_id = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_SAND_PP_IPV4_SUBNET_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV4_SUBNET));
  info->ip_address = 0;
  info->prefix_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_IPV4_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV4_ADDRESS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV4_ADDRESS));
  for (ind = 0; ind < SOC_SAND_PP_IPV4_ADDRESS_NOF_UINT32S; ++ind)
  {
    info->address[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV6_ADDRESS));
  for (ind = 0; ind < SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S; ++ind)
  {
    info->address[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_clear(
  SOC_SAND_OUT SOC_SAND_PP_IPV4_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV4_MC_ROUTE_KEY));
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&(info->source));
  soc_sand_SAND_PP_IPV4_SUBNET_clear(&(info->group));
  info->vid.val = 0;
  info->vid.mask = 0;
  info->port.val = 0;
  info->port.mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  SOC_SAND_PP_DESTINATION_ID_encode(
    SOC_SAND_IN  SOC_SAND_PP_DESTINATION_ID               *dest_info,
    SOC_SAND_OUT uint32                             *encoded_dest_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(dest_info);
  SOC_SAND_CHECK_NULL_INPUT(encoded_dest_val);

  switch(dest_info->dest_type)
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_DROP_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_DROP_SHIFT, SOC_SAND_PP_DEST_TYPE_DROP_MASK);
    break;
  case SOC_SAND_PP_DEST_FEC:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_FEC_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_FEC_SHIFT, SOC_SAND_PP_DEST_TYPE_FEC_MASK);
    break;
  case SOC_SAND_PP_DEST_MULTICAST:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_MC_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_MC_SHIFT, SOC_SAND_PP_DEST_TYPE_MC_MASK);
    break;
  case SOC_SAND_PP_DEST_TRAP:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_TRAP_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_TRAP_SHIFT, SOC_SAND_PP_DEST_TYPE_TRAP_MASK);
    break;
  case SOC_SAND_PP_DEST_LAG:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_LAG_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_LAG_SHIFT, SOC_SAND_PP_DEST_TYPE_LAG_MASK);
    break;
  case SOC_SAND_PP_DEST_SINGLE_PORT:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_PORT_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_PORT_SHIFT, SOC_SAND_PP_DEST_TYPE_PORT_MASK);
    break;
  case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
    *encoded_dest_val = SOC_SAND_PP_DEST_TYPE_FLOW_BASE;
    *encoded_dest_val |= SOC_SAND_SET_FLD_IN_PLACE(dest_info->dest_val, SOC_SAND_PP_DEST_TYPE_FLOW_SHIFT, SOC_SAND_PP_DEST_TYPE_FLOW_MASK);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 10, exit);
    break;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_SAND_PP_DESTINATION_ID_encode()",0,0);
}

uint32
  SOC_SAND_PP_DESTINATION_ID_decode(
    SOC_SAND_IN  uint32                             encoded_dest_val,
    SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID               *dest_info
    )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(dest_info);

  if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_DROP_BASE) == SOC_SAND_PP_DEST_TYPE_DROP_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_TYPE_DROP;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_DROP_SHIFT, SOC_SAND_PP_DEST_TYPE_DROP_MASK);
  }
  else if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_FLOW_BASE) == SOC_SAND_PP_DEST_TYPE_FLOW_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_EXPLICIT_FLOW;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_FLOW_SHIFT, SOC_SAND_PP_DEST_TYPE_FLOW_MASK);
  }
  else if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_FEC_BASE) == SOC_SAND_PP_DEST_TYPE_FEC_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_FEC;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_FEC_SHIFT, SOC_SAND_PP_DEST_TYPE_FEC_MASK);
  }
  else if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_MC_BASE) == SOC_SAND_PP_DEST_TYPE_MC_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_MULTICAST;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_MC_SHIFT, SOC_SAND_PP_DEST_TYPE_MC_MASK);
  }
  else if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_LAG_BASE) == SOC_SAND_PP_DEST_TYPE_LAG_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_LAG;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_LAG_SHIFT, SOC_SAND_PP_DEST_TYPE_LAG_MASK);
  }
  else if ((encoded_dest_val & SOC_SAND_PP_DEST_TYPE_PORT_BASE) == SOC_SAND_PP_DEST_TYPE_PORT_BASE)
  {
    
    dest_info->dest_type = SOC_SAND_PP_DEST_SINGLE_PORT;
    dest_info->dest_val = SOC_SAND_GET_FLD_FROM_PLACE(encoded_dest_val, SOC_SAND_PP_DEST_TYPE_PORT_SHIFT, SOC_SAND_PP_DEST_TYPE_PORT_MASK);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_SAND_PP_DESTINATION_ID_encode()",0,0);
}

void
soc_sand_SAND_PP_IPV6_ROUTE_KEY_clear(
  SOC_SAND_OUT SOC_SAND_PP_IPV6_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV4_MC_ROUTE_KEY));
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&(info->dest.ipv6_address));
  info->vid.val = 0;
  info->vid.mask = 0;
  info->port.val = 0;
  info->port.mask = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_IPV6_SUBNET_clear(
    SOC_SAND_OUT SOC_SAND_PP_IPV6_SUBNET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_IPV6_SUBNET));
  soc_sand_SAND_PP_IPV6_ADDRESS_clear(&(info->ipv6_address));
  info->prefix_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_TRILL_DEST_clear(
    SOC_SAND_OUT SOC_SAND_PP_TRILL_DEST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_TRILL_DEST));
  info->is_multicast = 0;
  info->dest_nick = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_SAND_PP_VLAN_TAG_clear(
    SOC_SAND_OUT SOC_SAND_PP_VLAN_TAG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SOC_SAND_PP_VLAN_TAG));
  info->tpid = 0;
  info->vid = 0;
  info->pcp = 0;
  info->dei = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_SAND_DEBUG

const char*
  soc_sand_SAND_PP_DEST_TYPE_to_string_short(
    SOC_SAND_IN SOC_SAND_PP_DEST_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    str = "DROP";
  break;

  case SOC_SAND_PP_DEST_TYPE_ROUTER:
    str = "ROUTER";
  break;

  case SOC_SAND_PP_DEST_SINGLE_PORT:
    str = "SINGLE PORT";
    break;

  case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
    str = "EXP FLOW";
    break;

  case SOC_SAND_PP_DEST_LAG:
    str = "LAG";
    break;

  case SOC_SAND_PP_DEST_MULTICAST:
    str = "MULTICAST";
    break;

  case SOC_SAND_PP_DEST_FEC:
    str = "FEC";
    break;
  case SOC_SAND_PP_DEST_TRAP:
    str = "TRAP";
  break;
  default:
    str = "SINGLE PORT";
  }
  return str;
}

const char*
  SOC_SAND_PP_IP_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_IP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_IP_TYPE_NONE:
    str = "none";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV4_UC:
    str = "ipv4_uc";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV4_MC:
    str = "ipv4_mc";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV4:
    str = "ipv4";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV6_UC:
    str = "ipv6_uc";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV6_MC:
    str = "ipv6_mc";
  break;
  case SOC_SAND_PP_IP_TYPE_IPV6:
    str = "ipv6";
  break;
  case SOC_SAND_PP_IP_TYPE_ALL:
    str = "all";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_DEST_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_DEST_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_DEST_TYPE_DROP:
    str = "SOC_SAND_PP_DEST_TYPE_DROP";
  break;
  case SOC_SAND_PP_DEST_TYPE_ROUTER:
    str = "SOC_SAND_PP_DEST_TYPE_ROUTER";
  break;
  case SOC_SAND_PP_DEST_SINGLE_PORT:
    str = "SOC_SAND_PP_DEST_SINGLE_PORT";
  break;
  case SOC_SAND_PP_DEST_EXPLICIT_FLOW:
    str = "SOC_SAND_PP_DEST_EXPLICIT_FLOW";
  break;
  case SOC_SAND_PP_DEST_LAG:
    str = "SOC_SAND_PP_DEST_LAG";
  break;
  case SOC_SAND_PP_DEST_MULTICAST:
    str = "SOC_SAND_PP_DEST_MULTICAST";
  break;
  case SOC_SAND_PP_DEST_FEC:
    str = "SOC_SAND_PP_DEST_FEC";
  break;
  case SOC_SAND_PP_DEST_TRAP:
    str = "SOC_SAND_PP_DEST_TRAP";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_FRWRD_ACTION_TYPE_to_string(
    SOC_SAND_IN SOC_SAND_PP_FRWRD_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE:
    str = "SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL:
    str = "SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT:
    str = "SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL:
    str = "SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL";
  break;

  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}


const char*
  soc_sand_SAND_PP_FRWRD_ACTION_TYPE_to_string_short(
    SOC_SAND_IN SOC_SAND_PP_FRWRD_ACTION_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE:
    str = "NONE";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL:
    str = "NORMAL";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT:
    str = "INTRCPT";
  break;

  case SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL:
    str = "CONTROL";
  break;

  default:
    str = "None";
  }
  return str;
}

void
  soc_sand_SAND_PP_DESTINATION_ID_print(
    SOC_SAND_IN  SOC_SAND_PP_DESTINATION_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "dest_type %s "), soc_sand_SAND_PP_DEST_TYPE_to_string(info->dest_type)));
  LOG_CLI((BSL_META_U(unit,
                      "dest_val: %u\n\r"),info->dest_val));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_SYS_PORT_ID_print(
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "sys_port_type %s "), soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(info->sys_port_type)));
  LOG_CLI((BSL_META_U(unit,
                      "sys_id: %u\n\r"),info->sys_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_COS_PARAMS_print(
    SOC_SAND_IN SOC_SAND_PP_COS_PARAMS *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "user_priority: %u\n\r"),info->user_priority));
  LOG_CLI((BSL_META_U(unit,
                      "traffic_class: %u\n\r"),info->traffic_class));
  LOG_CLI((BSL_META_U(unit,
                      "drop_precedence: %u\n\r"),info->drop_precedence));exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  soc_sand_SAND_PP_VLAN_TAG_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_VLAN_TAG_TYPE_NONE:
    str = "none";
    break;
  case SOC_SAND_PP_VLAN_TAG_TYPE_CTAG:
    str = "ctag";
    break;
  case SOC_SAND_PP_VLAN_TAG_TYPE_STAG:
    str = "stag";
    break;
  case SOC_SAND_PP_VLAN_TAG_TYPE_PRIORITY:
    str = "priority";
    break;
  case SOC_SAND_PP_VLAN_TAG_TYPE_ANY:
    str = "any";
    break;
  case SOC_SAND_PP_VLAN_TAG_TYPE_ITAG:
    str = "itag";
    break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_ETHERNET_DA_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_DA_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC:
    str = "SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_UC";
  break;
  case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC:
    str = "SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_MC";
  break;
  case SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC:
    str = "SOC_SAND_PP_ETHERNET_DA_TYPE_TYPE_BC";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_PORT_L2_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_PORT_L2_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_PORT_L2_TYPE_VBP:
    str = "SOC_SAND_PP_PORT_L2_TYPE_VBP";
  break;
  case SOC_SAND_PP_PORT_L2_TYPE_CNP:
    str = "SOC_SAND_PP_PORT_L2_TYPE_CNP";
  break;
  case SOC_SAND_PP_PORT_L2_TYPE_CEP:
    str = "SOC_SAND_PP_PORT_L2_TYPE_CEP";
  break;
  case SOC_SAND_PP_PORT_L2_TYPE_PNP:
    str = "SOC_SAND_PP_PORT_L2_TYPE_PNP";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_PORT_L2_TYPE_to_string_short(
    SOC_SAND_IN SOC_SAND_PP_PORT_L2_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_PP_PORT_L2_TYPE_VBP:
    str = "VBP";
    break;

  case SOC_SAND_PP_PORT_L2_TYPE_CNP:
    str = "CNP";
    break;

  case SOC_SAND_PP_PORT_L2_TYPE_CEP:
    str = "CEP";
    break;

  case SOC_SAND_PP_PORT_L2_TYPE_PNP:
    str = "PNP";
    break;

  default:
    str = " NONE";
  }
  return str;
}

void
  soc_sand_SAND_PP_SYS_PORT_ID_table_format_print(
    SOC_SAND_IN SOC_SAND_PP_SYS_PORT_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%-6s|"),
           soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(info->sys_port_type)
           ));
  LOG_CLI((BSL_META_U(unit,
                      "%-5d|"),info->sys_id));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}

void
  soc_sand_SAND_PP_DESTINATION_ID_table_format_print(
    SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%11s|"),
           soc_sand_SAND_PP_DEST_TYPE_to_string_short(info->dest_type)
           ));
  if(info->dest_type == SOC_SAND_PP_DEST_TRAP) { 
      LOG_CLI((BSL_META_U(unit,
                          "0x%05x|"),info->dest_val));
  }
  else{
  LOG_CLI((BSL_META_U(unit,
                      "%5d|"),info->dest_val));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


const char*
  soc_sand_pp_ip_long_to_string(
    SOC_SAND_IN   uint32    ip_addr,
    SOC_SAND_IN   uint8     short_format,
    SOC_SAND_OUT   char  decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING]

  )
{
  char
    *ret,
    *format;
  uint8
    byte_indx,
    ip_bytes[4];

  ret = decimal_ip ;

  if (short_format)
  {
    format = "%d.%d.%d.%d";
  }
  else
  {
    format = "%03d.%03d.%03d.%03d";
  }
  for (byte_indx = 0; byte_indx < 4; ++byte_indx)
  {
    ip_bytes[byte_indx] =
      (uint8)SOC_SAND_GET_BITS_RANGE(
        ip_addr,
        SOC_SAND_NOF_BITS_IN_BYTE * byte_indx + 7,
        SOC_SAND_NOF_BITS_IN_BYTE * byte_indx
      );
  }

  sal_sprintf(decimal_ip, format,
                ip_bytes[3],
                ip_bytes[2],
                ip_bytes[1],
                ip_bytes[0]) ;

  return (ret) ;
}


STATIC uint32
  soc_sand_pp_ipv4_get_char_num_val(
    SOC_SAND_IN char ipv4_char,
    SOC_SAND_OUT uint32  *num_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(ipv4_char == '0')
  {
    *num_val = 0;
  }
  else if(ipv4_char >= '1' && ipv4_char <='9')
  {
    *num_val = ipv4_char + 1 - '1';
  }
  else if(ipv4_char >= 'a' && ipv4_char <='f')
  {
    *num_val = ipv4_char + 10 - 'a';
  }
  else if(ipv4_char >= 'A' && ipv4_char <='F')
  {
    *num_val = ipv4_char + 10 - 'A';
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_IP_FORMAT, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_ipv4_get_char_num_val()",0,0);
}

STATIC uint32
  soc_sand_pp_ipv6_get_char_num_val(
    SOC_SAND_IN char ipv6_char,
    SOC_SAND_OUT uint32  *num_val
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if(ipv6_char == '0')
  {
    *num_val = 0;
  }
  else if(ipv6_char >= '1' && ipv6_char <='9')
  {
    *num_val = ipv6_char + 1 - '1';
  }
  else if(ipv6_char >= 'a' && ipv6_char <='f')
  {
    *num_val = ipv6_char + 10 - 'a';
  }
  else if(ipv6_char >= 'A' && ipv6_char <='F')
  {
    *num_val = ipv6_char + 10 - 'A';
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_IP_FORMAT, 10, exit);
  }
exit:
 SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_ipv6_get_char_num_val()",0,0);
}


uint32
  soc_sand_pp_ipv4_address_string_parse(
    SOC_SAND_IN char               ipv4_string[SOC_SAND_PP_IPV4_ADDRESS_STRING_LEN],
    SOC_SAND_OUT SOC_SAND_PP_IPV4_ADDRESS   *ipv4_addr
  )
{
  uint32
    byte_indx,
    char_indx,
    in_long_indx,
    input_len;
  uint32
    long_sum,
    num_val = 0;
  uint32
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  input_len = soc_sand_os_strlen((const char*)ipv4_string);

  soc_sand_os_memset(
    ipv4_addr->address,
    0x0,
    sizeof(uint32) * SOC_SAND_PP_IPV4_ADDRESS_NOF_UINT32S
  );

  byte_indx = 0;

  for(char_indx=input_len; char_indx > 0 ;char_indx-=8, ++byte_indx)
  {
    long_sum = 0;
    for (in_long_indx = 0; (char_indx - in_long_indx > 0) && (in_long_indx < 8); ++in_long_indx )
    {
      res = soc_sand_pp_ipv4_get_char_num_val(
              ipv4_string[char_indx - in_long_indx - 1],
              &num_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      soc_sand_set_field(
        &long_sum,
        (in_long_indx+1) * 4 - 1,
        (in_long_indx) * 4,
        num_val
      );
    }
    ipv4_addr->address[byte_indx] = long_sum;

    if (char_indx < 9)
    {
      break;
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_ipv4_address_string_parse()",0,0);
}


uint32
  soc_sand_pp_ipv6_address_string_parse(
    SOC_SAND_IN char               ipv6_string[SOC_SAND_PP_IPV6_ADDRESS_STRING_LEN],
    SOC_SAND_OUT SOC_SAND_PP_IPV6_ADDRESS   *ipv6_addr
  )
{
  uint32
    byte_indx,
    char_indx,
    in_long_indx,
    input_len;
  uint32
    long_sum,
    num_val = 0;
  uint32
    res;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  input_len = soc_sand_os_strlen((const char*)ipv6_string);

  soc_sand_os_memset(
    ipv6_addr->address,
    0x0,
    sizeof(uint32) * SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S
  );

  byte_indx = 0;

  for(char_indx=input_len; char_indx > 0 ;char_indx-=8, ++byte_indx)
  {
    long_sum = 0;
    for (in_long_indx = 0; (char_indx - in_long_indx > 0) && (in_long_indx < 8); ++in_long_indx )
    {
      res = soc_sand_pp_ipv6_get_char_num_val(
              ipv6_string[char_indx - in_long_indx - 1],
              &num_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      soc_sand_set_field(
        &long_sum,
        (in_long_indx+1) * 4 - 1,
        (in_long_indx) * 4,
        num_val
      );
    }
    ipv6_addr->address[byte_indx] = long_sum;

    if (char_indx < 9)
    {
      break;
    }

  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_sand_pp_ipv6_address_string_parse()",0,0);
}

void
  soc_sand_SAND_PP_IPV4_SUBNET_print(
    SOC_SAND_IN SOC_SAND_PP_IPV4_SUBNET *info
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%s"), soc_sand_pp_ip_long_to_string(info->ip_address,1,decimal_ip)));
  if (info->prefix_len < SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN)
  {
    LOG_CLI((BSL_META_U(unit,
                        "\\%u"),info->prefix_len));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_IPV4_ADDRESS_print(
    SOC_SAND_IN SOC_SAND_PP_IPV4_ADDRESS *ipv4_addr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(ipv4_addr);

  LOG_CLI((BSL_META_U(unit,
                    "%08x"), (uint32)(ipv4_addr->address[0])));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_IPV6_ADDRESS_print(
    SOC_SAND_IN SOC_SAND_PP_IPV6_ADDRESS *ipv6_addr
  )
{
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(ipv6_addr);

  for (indx = SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S; indx > 0; --indx)
  {
    LOG_CLI((BSL_META_U(unit,
                        "%08x"), (uint32)(ipv6_addr->address[indx-1])));
    if (indx > 1)
    {
      LOG_CLI((BSL_META_U(unit,
                          ":")));
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_SAND_PP_IPV6_ROUTE_KEY_print(
    SOC_SAND_IN SOC_SAND_PP_IPV6_ROUTE_KEY *info
    )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Dest-IP:  ")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&(info->dest.ipv6_address));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  if (info->vid.mask)
  {
    LOG_CLI((BSL_META_U(unit,
                        "in_vid: %u      "),info->vid.val));
    LOG_CLI((BSL_META_U(unit,
                        "vid_mask: 0x%03X\n\r"),info->vid.mask));
  }
  if (info->port.mask)
  {
    LOG_CLI((BSL_META_U(unit,
                        "in_port: %u     "),info->port.val));
    LOG_CLI((BSL_META_U(unit,
                        "port_mask: 0x%08X\n\r"),info->port.mask));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
soc_sand_SAND_PP_IPV4_MC_ROUTE_KEY_print(
  SOC_SAND_IN SOC_SAND_PP_IPV4_MC_ROUTE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "Src-IP:   ")));
  soc_sand_SAND_PP_IPV4_SUBNET_print(&(info->source));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  LOG_CLI((BSL_META_U(unit,
                      "Dest-IP:  ")));
  soc_sand_SAND_PP_IPV4_SUBNET_print(&(info->group));
  LOG_CLI((BSL_META_U(unit,
                      "\n\r")));
  if (info->vid.mask)
  {
    LOG_CLI((BSL_META_U(unit,
                        "in_vid: %u      "),info->vid.val));
    LOG_CLI((BSL_META_U(unit,
                        "vid_mask: 0x%03X\n\r"),info->vid.mask));
  }
  if (info->port.mask)
  {
    LOG_CLI((BSL_META_U(unit,
                        "in_port: %u     "),info->port.val));
    LOG_CLI((BSL_META_U(unit,
                        "port_mask: 0x%08X\n\r"),info->port.mask));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  soc_sand_SAND_PP_IPV4_SUBNET_print_short(
    SOC_SAND_IN SOC_SAND_PP_IPV4_SUBNET *info
  )
{
  char
    decimal_ip[SOC_SAND_PP_IPV4_MAX_IP_STRING];

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "%s"), soc_sand_pp_ip_long_to_string(info->ip_address,1,decimal_ip)));
  if (info->prefix_len < SOC_SAND_PP_IPV4_SUBNET_PREF_MAX_LEN)
  {
    LOG_CLI((BSL_META_U(unit,
                        "/%u"),info->prefix_len));
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  soc_sand_SAND_PP_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN SOC_SAND_PP_SYS_PORT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:
    str = "SINGLE";
  break;

  case SOC_SAND_PP_SYS_PORT_TYPE_LAG:
    str = "LAG";
  break;

  default:
    str = "Unkwn";
  }
  return str;
}


const char*
  soc_sand_SAND_PP_L4_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_L4_PORT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_L4_PORT_TYPE_UDP:
    str = "SOC_SAND_PP_L4_PORT_TYPE_UDP";
  break;
  case SOC_SAND_PP_L4_PORT_TYPE_TCP:
    str = "SOC_SAND_PP_L4_PORT_TYPE_TCP";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_L4_PRTCL_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_L4_PRTCL_TYPE_UDP:
    str = "SOC_SAND_PP_L4_PRTCL_TYPE_UDP";
  break;
  case SOC_SAND_PP_L4_PRTCL_TYPE_TCP:
    str = "SOC_SAND_PP_L4_PRTCL_TYPE_TCP";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_HUB_SPOKE_ORIENTATION_to_string(
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_HUB:
    str = "HUB";
  break;
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE:
    str = "SPOKE";
  break;
  default:
    str = "NA";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_SPLIT_HORIZON_ORIENTATION_to_string(
    SOC_SAND_IN  SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION_SPLIT:
    str = "SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION_SPLIT";
  break;
  case SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE_HORIZON:
    str = "SOC_SAND_PP_HUB_SPOKE_ORIENTATION_SPOKE_HORIZON";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_MPLS_TUNNEL_MODEL_to_string(
    SOC_SAND_IN  SOC_SAND_PP_MPLS_TUNNEL_MODEL enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_MPLS_TUNNEL_MODEL_PIPE:
  case SOC_SAND_PP_MPLS_TUNNEL_MODEL_SET:
    str = "PIPE";
  break;
  case SOC_SAND_PP_MPLS_TUNNEL_MODEL_UNIFORM:
  case SOC_SAND_PP_MPLS_TUNNEL_MODEL_COPY:
    str = "UNIFORM";
  break;
  default:
    str = " Unknown Enumerator Value";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_FEC_COMMAND_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_FEC_COMMAND_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_FEC_COMMAND_TYPE_DROP:
    str = "DROP";
  break;

  case SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE:
    str = "ROUTE";
  break;

  case SOC_SAND_PP_FEC_COMMAND_TYPE_TRAP:
    str = "TRAP";
  break;

  default:
    str = "Unkwn";
  }
  return str;
}

const char*
  soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE:
    str = "none";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG:
    str = "c_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
    str = "s_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
    str = "s_c_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG:
    str = "priority_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG:
    str = "priority_c_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_S_TAG:
    str = "c_s_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_S_TAG:
    str = "priority_s_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_S_TAG:
    str = "s_s_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_C_TAG:
    str = "c_c_tag";
  break;
  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_I_TAG:
    str = "i_tag";
  break;

  case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY:
    str = "any";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_SAND_PP_ETH_ENCAP_TYPE_to_string(
    SOC_SAND_IN  SOC_SAND_PP_ETH_ENCAP_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val) 
  {
  case SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2:
    str = "eth2";
  break;
  case SOC_SAND_PP_ETH_ENCAP_TYPE_LLC:
    str = "llc";
  break;
  case SOC_SAND_PP_ETH_ENCAP_TYPE_LLC_SNAP:
    str = "llc_snap";
  break;
  case SOC_SAND_PP_ETH_ENCAP_TYPE_OTHER:
    str = "other";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  soc_sand_SAND_PP_IPV6_SUBNET_print(
    SOC_SAND_IN  SOC_SAND_PP_IPV6_SUBNET *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "ipv6_address:")));
  soc_sand_SAND_PP_IPV6_ADDRESS_print(&(info->ipv6_address));
  LOG_CLI((BSL_META_U(unit,
                      "prefix_len: %u\n\r"), info->prefix_len));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_sand_SAND_PP_TRILL_DEST_print(
    SOC_SAND_IN  SOC_SAND_PP_TRILL_DEST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "is_multicast: %u\n\r"),info->is_multicast));
  LOG_CLI((BSL_META_U(unit,
                      "dest_nick: %u\n\r"),info->dest_nick));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_SAND_PP_VLAN_TAG_print(
    SOC_SAND_IN  SOC_SAND_PP_VLAN_TAG *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  LOG_CLI((BSL_META_U(unit,
                      "tpid: %08x  "), info->tpid));
  LOG_CLI((BSL_META_U(unit,
                      "vid: %u  "),info->vid));
  LOG_CLI((BSL_META_U(unit,
                      "pcp: %u  "), info->pcp));
  LOG_CLI((BSL_META_U(unit,
                      "dei: %u  "), info->dei));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>

