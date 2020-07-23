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

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT



#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_ports.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_general.h>



























uint32
  arad_sys_phys_to_local_port_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  )
{
  uint32
    res;
  int
    int_res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  
  int_res = arad_sys_phys_to_local_port_map_verify(unit,sys_phys_port_ndx,mapped_fap_id,mapped_fap_port_id);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(int_res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_sys_phys_to_local_port_map_set_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_set()",0,0);
}


uint32
  arad_sys_phys_to_local_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SYS_PHYS_TO_LOCAL_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_id);
  SOC_SAND_CHECK_NULL_INPUT(mapped_fap_port_id);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_sys_phys_to_local_port_map_get_unsafe(
    unit,
    sys_phys_port_ndx,
    mapped_fap_id,
    mapped_fap_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_sys_phys_to_local_port_map_get()",0,0);
}


uint32
  arad_local_to_sys_phys_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_LOCAL_TO_SYS_PHYS_PORT_MAP_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_local_to_sys_phys_port_map_get_unsafe(
    unit,
    fap_ndx,
    fap_local_port_ndx,
    sys_phys_port_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_local_to_sys_phys_port_map_get()",0,0);
}



uint32
  arad_modport_to_sys_phys_port_map_get(
    SOC_SAND_IN  int     unit,
    SOC_SAND_IN  uint32  fap_id,            
    SOC_SAND_IN  uint32  tm_port,           
    SOC_SAND_OUT uint32  *sys_phys_port_id  
  )
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    SOC_SAND_CHECK_NULL_INPUT(sys_phys_port_id);
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_modport_to_sys_phys_port_map_get_unsafe(unit, fap_id, tm_port, sys_phys_port_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_modport_to_sys_phys_port_map_get()", fap_id, tm_port);
}


uint32
  arad_port_to_dynamic_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info,
    SOC_SAND_IN  uint8                    is_init
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_TO_DYNAMIC_INTERFACE_MAP_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_to_interface_map_verify(
    unit,
    port_ndx,
    direction_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_to_dynamic_interface_map_set_unsafe(
    unit,
	core_id,
    port_ndx,
    direction_ndx,
    info,
    is_init
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_to_dynamic_interface_map_set()",0,0);
}

int
  arad_port_to_interface_map_set(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  int                 unmap,
    SOC_SAND_IN  int                 is_dynamic_port
  )
{
  uint32
    tm_port,
    channel;
  ARAD_PORT2IF_MAPPING_INFO    port2if_mapping_info;
  ARAD_INTERFACE_ID            if_id;
  int 
    core;

  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
  SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));

  if (unmap) {
      SOCDNX_SAND_IF_ERR_EXIT(arad_port_to_dynamic_interface_unmap_set_unsafe(unit, core, tm_port, SOC_TMC_PORT_DIRECTION_BOTH));
  } else {
      SOC_TMC_PORT2IF_MAPPING_INFO_clear(&port2if_mapping_info);
      SOCDNX_IF_ERR_EXIT(arad_ports_port_to_nif_id_get(unit, core, tm_port, &if_id));
      port2if_mapping_info.if_id = if_id;
      port2if_mapping_info.channel_id = channel;

      SOCDNX_SAND_IF_ERR_EXIT(arad_port_to_interface_map_set_unsafe(
        unit,
        core,
        tm_port,
        SOC_TMC_PORT_DIRECTION_BOTH,
        &port2if_mapping_info,
        FALSE 
      ));
  }

exit:
    SOCDNX_FUNC_RETURN;
}


 
uint32  
     arad_ports_is_port_lag_member( 
         SOC_SAND_IN  int                                 unit, 
         SOC_SAND_IN  int                                 core_id,
         SOC_SAND_IN  uint32                                 port_id, 
        SOC_SAND_OUT uint8                                 *is_in_lag, 
        SOC_SAND_OUT uint32                                 *lag_id) 
{   
  uint32 
    res = SOC_SAND_OK; 
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0); 
   
  SOC_SAND_CHECK_DRIVER_AND_DEVICE; 
  
  SOC_SAND_CHECK_NULL_INPUT(is_in_lag); 
  SOC_SAND_CHECK_NULL_INPUT(lag_id); 
  
   SOC_SAND_TAKE_DEVICE_SEMAPHORE; 
  
  res = arad_ports_is_port_lag_member_unsafe( 
          unit, 
          core_id,
          port_id, 
          is_in_lag, 
          lag_id 
        ); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 
  
exit_semaphore: 
  SOC_SAND_GIVE_DEVICE_SEMAPHORE; 
exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_is_port_lag_member()",0,0); 
} 
  



uint32
  arad_ports_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_ports_lag_verify(
          unit,
          direction_ndx,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_set_unsafe(
          unit,
          lag_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_set()",0,0);
}


uint32
  arad_ports_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PPC_LAG_INFO      *info_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info_incoming);
  SOC_SAND_CHECK_NULL_INPUT(info_outgoing);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_get_unsafe(
          unit,
          lag_ndx,
          info_incoming,
          info_outgoing
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_get()",0,0);
}


uint32
  arad_ports_lag_member_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_MEMBER_ADD);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);
  SOC_SAND_CHECK_NULL_INPUT(success);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_member_add_unsafe( unit, lag_ndx, lag_member);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  *success = SOC_SAND_SUCCESS;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_member_add()",0,0);
}


uint32
  arad_ports_lag_sys_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  ARAD_PORTS_LAG_MEMBER  *lag_member
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_SYS_PORT_REMOVE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(lag_member);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    direction_ndx, ARAD_PORT_NOF_DIRECTIONS-1,
    ARAD_PORT_DIRECTION_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    lag_ndx, ARAD_MAX_LAG_GROUP_ID,
    ARAD_PORT_LAG_ID_OUT_OF_RANGE_ERR, 20, exit
  );

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_sys_port_remove_unsafe(unit, lag_ndx, lag_member );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_sys_port_remove()",0,0);
}


uint32
  arad_ports_lag_order_preserve_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_ports_lag_order_preserve_verify(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_order_preserve_set_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_set()",0,0);
}


uint32
  arad_ports_lag_order_preserve_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_LAG_ORDER_PRESERVE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_order_preserving);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_order_preserve_get_unsafe(
    unit,
    lag_ndx,
    is_order_preserving
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_order_preserve_get()",0,0);
}

uint32 arad_ports_lag_lb_key_range_set(
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO      *info)
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_lag_lb_key_range_set_unsafe(
    unit,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_lag_lb_key_range_set()",0,0);
}


uint32
  arad_port_header_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    header_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_HEADER_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_header_type_verify(
    unit,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_header_type_set_unsafe(
    unit,
    core_id,
    port_ndx,
    direction_ndx,
    header_type
  );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_header_type_set()",0,0);
}


uint32
  arad_port_header_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT ARAD_PORT_HEADER_TYPE    *header_type_outgoing
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_HEADER_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(header_type_incoming);
  SOC_SAND_CHECK_NULL_INPUT(header_type_outgoing);
  SOC_SAND_ERR_IF_ABOVE_NOF(port_ndx, ARAD_NOF_LOCAL_PORTS(unit), ARAD_FAP_PORT_ID_INVALID_ERR, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_header_type_get_unsafe(
          unit,
          core_id,
          port_ndx,
          header_type_incoming,
          header_type_outgoing
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_header_type_get()",0,0);
}

uint32
  arad_ports_ftmh_extension_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_ftmh_extension_set_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);
  
  goto exit;

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_set()",0,0);
}


uint32
  arad_ports_ftmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_PORTS_FTMH_EXT_OUTLIF *ext_option
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_FTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(ext_option);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_ftmh_extension_get_unsafe(
    unit,
    ext_option
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_ftmh_extension_get()",0,0);
}


uint32
  arad_ports_otmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_OTMH_EXTENSION_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_ports_otmh_extension_get_unsafe(
    unit,
    core_id,
    port_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_ports_otmh_extension_get()",0,0);
}



uint32
  arad_port_egr_hdr_credit_discount_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_egr_hdr_credit_discount_type_verify(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_type_set_unsafe(
    unit,
    core,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_set()",0,0);
}



uint32
  arad_port_egr_hdr_credit_discount_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_TYPE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_type_get_unsafe(
    unit,
    port_hdr_type_ndx,
    cr_discnt_type_ndx,
    info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_type_get()",0,0);
}



uint32
  arad_port_egr_hdr_credit_discount_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_IN  ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_egr_hdr_credit_discount_select_verify(
    unit,
    tm_port,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_select_set_unsafe(
    unit,
    core,
    tm_port,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_set()",0,0);
}



uint32
  arad_port_egr_hdr_credit_discount_select_get(
    SOC_SAND_IN  int                    unit, 
    SOC_SAND_IN  int                    core, 
    SOC_SAND_IN  uint32                 tm_port, 
    SOC_SAND_OUT ARAD_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORT_EGR_HDR_DISCOUNT_SELECT_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(cr_discnt_type);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_egr_hdr_credit_discount_select_get_unsafe(
    unit,
    core,
    tm_port,
    cr_discnt_type
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_egr_hdr_credit_discount_select_get()",0,0);
}



uint32 arad_port_stacking_info_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_IN  uint32                              is_stacking,
    SOC_SAND_IN  uint32                              peer_tmd)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    res = arad_port_stacking_info_verify(unit, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_info_set_unsafe(unit, core_id, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_set()",0,0);
}

uint32 arad_port_stacking_info_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                              local_port_ndx,
    SOC_SAND_OUT  uint32                              *is_stacking,
    SOC_SAND_OUT  uint32                              *peer_tmd)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

    SOC_SAND_CHECK_NULL_INPUT(is_stacking);
    SOC_SAND_CHECK_NULL_INPUT(peer_tmd);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_info_get_unsafe(unit, core_id, local_port_ndx, is_stacking, peer_tmd);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_get()",0,0);
}


uint32 arad_port_stacking_route_history_bitmap_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  int                              core_id,
    SOC_SAND_IN  uint32                           tm_port,
    SOC_SAND_IN  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK tm_port_profile_stack,
    SOC_SAND_IN  uint32                              bitmap)
{
    uint32 res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_DRIVER_AND_DEVICE;

     SOC_SAND_TAKE_DEVICE_SEMAPHORE;

    res = arad_port_stacking_route_history_bitmap_set_unsafe(unit, core_id, tm_port, tm_port_profile_stack, bitmap);
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_port_stacking_info_get()",0,0);
}



uint32
  arad_port_pp_port_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO         *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_port_pp_port_set_verify(
          unit,
          pp_port_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_pp_port_set_unsafe(
          unit,
		  core,
          pp_port_ndx,
          info,
          success
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_set()", pp_port_ndx, 0);
}


uint32
  arad_port_pp_port_get(
    SOC_SAND_IN  int                    unit,
	SOC_SAND_IN  int 					core_id,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO     *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_port_pp_port_get_verify(
          unit,
          pp_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_pp_port_get_unsafe(
          unit,
		  core_id,
          pp_port_ndx,
          info
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_pp_port_get()", pp_port_ndx, 0);
}


uint32
  arad_port_to_pp_port_map_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_port_t             port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10,  exit, arad_port_to_pp_port_map_set_verify(unit, port, ARAD_PORT_DIRECTION_BOTH));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20,  exit, arad_port_to_pp_port_map_set_unsafe(unit,port,ARAD_PORT_DIRECTION_BOTH));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_to_pp_port_map_set()", port, 0);
}


uint32
  arad_ports_swap_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO       *info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_SET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_swap_set_verify(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_swap_set_unsafe(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_set()", port_ndx, info->enable);
}


uint32
  arad_ports_swap_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID           port_ndx,
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO       *info
  )
{
    uint32 res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PORTS_SWAP_GET);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_swap_get_verify(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_swap_get_unsafe(
            unit,
            port_ndx,
            info);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
    ARAD_DO_NOTHING_AND_EXIT;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_swap_get()", port_ndx, info->enable);
}



uint32
  arad_ports_pon_tunnel_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_IN  ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_pon_tunnel_info_set_verify(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_pon_tunnel_info_set_unsafe(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_set()", port_ndx, tunnel);
}


uint32
  arad_ports_pon_tunnel_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PON_TUNNEL_ID       tunnel,
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_pon_tunnel_info_get_verify(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_pon_tunnel_info_get_unsafe(
            unit,
            port_ndx,
            tunnel,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_pon_tunnel_info_get()", port_ndx, tunnel);
}


uint32
  arad_port_direct_lb_key_min_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id,
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 min_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_set_verify(
          unit, 
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_min_set_unsafe(
          unit, 
          core_id,
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_min_set()",0,0);
}

uint32
  arad_port_direct_lb_key_max_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_IN uint32 max_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_set_verify(
          unit, 
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_max_set_unsafe(
          unit, 
          core_id,
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_max_set()",0,0);
}

uint32
  arad_port_direct_lb_key_min_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint32* min_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(min_lb_key);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_get_verify(
          unit, 
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_min_get_unsafe(
          unit, 
          core_id,
          local_port,
          min_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_min_get()",0,0);
}

uint32
  arad_port_direct_lb_key_max_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core_id, 
    SOC_SAND_IN uint32  local_port,
    SOC_SAND_OUT uint32* max_lb_key
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(max_lb_key);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_port_direct_lb_key_get_verify(
          unit, 
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_direct_lb_key_max_get_unsafe(
          unit, 
          core_id,
          local_port,
          max_lb_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_direct_lb_key_max_get()",0,0);
}


uint32
   arad_port_synchronize_lb_key_tables_at_egress(
   SOC_SAND_IN int unit
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_port_synchronize_lb_key_tables_at_egress_unsafe(unit);
  
  if (res != SOC_E_NONE) 
  {
      res = SOC_SAND_ERR;
  }
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore); 

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_synchronize_lb_key_tables_at_egress()",0,0);
}


uint32
  arad_port_switch_lb_key_tables(
     SOC_SAND_IN int unit
    )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res =  arad_port_switch_lb_key_tables_unsafe(
          unit 
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_port_synchronize_lb_key_tables_at_egress()",0,0);
}


uint32
  arad_port_encap_config_get(
    SOC_SAND_IN  int                   unit,
	SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_L2_ENCAP_INFO       *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_port_encap_config_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_port_encap_config_get_unsafe(
            unit,
			core_id,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_encap_config_get()", port_ndx, 0);
}



uint32
  arad_port_encap_config_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_L2_ENCAP_INFO       *info
  )
{
    uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_port_encap_config_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_port_encap_config_set_unsafe(
            unit,
            core_id,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_port_encap_config_get()", port_ndx, 0);
}


uint32
  arad_ports_programs_info_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  uint32
      res = SOC_SAND_OK;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
    SOC_SAND_CHECK_NULL_INPUT(info);

    res = arad_ports_programs_info_set_verify(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
    res = arad_ports_programs_info_set_unsafe(
            unit,
            port_ndx,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_set()", port_ndx,0);
}

uint32
  arad_ports_programs_info_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  )
{
   uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_programs_info_get_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_programs_info_get_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_get()", port_ndx, 0);
}


uint32 
  arad_ports_application_mapping_info_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_IN ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    )
{
  uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_application_mapping_info_set_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_application_mapping_info_set_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_application_mapping_info_set()", port_ndx, 0);
}


uint32 
  arad_ports_application_mapping_info_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN ARAD_FAP_PORT_ID port_ndx, 
    SOC_SAND_INOUT ARAD_PORTS_APPLICATION_MAPPING_INFO *info    
    )
{
   uint32
      res = SOC_SAND_OK;

   SOC_SAND_INIT_ERROR_DEFINITIONS(0);

   SOC_SAND_CHECK_DRIVER_AND_DEVICE;
    
   SOC_SAND_CHECK_NULL_INPUT(info);

   res = arad_ports_application_mapping_info_get_verify(
           unit,
           port_ndx,
            info
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

   SOC_SAND_TAKE_DEVICE_SEMAPHORE;
    
   res = arad_ports_application_mapping_info_get_unsafe(
          unit,
          port_ndx,
          info
        );
   SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ports_programs_info_get()", port_ndx, 0);
}




void
  ARAD_PORT_PP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PORT_COUNTER_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_COUNTER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_COUNTER_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  arad_ARAD_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT ARAD_PORTS_LAG_MEMBER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_LAG_MEMBER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_SOC_PPC_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PPC_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_ITMH_clear(
    SOC_SAND_OUT ARAD_PORTS_ITMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_ITMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
arad_ARAD_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PORT_LAG_SYS_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_LAG_SYS_PORT_INFO_clear(info);

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT ARAD_PORTS_OTMH_EXTENSIONS_EN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_OTMH_EXTENSIONS_EN_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_SWAP_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_SWAP_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  arad_ARAD_PORTS_PON_TUNNEL_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PON_TUNNEL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_PON_TUNNEL_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_PROGRAMS_INFO_clear(
    SOC_SAND_OUT ARAD_PORTS_PROGRAMS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_PROGRAMS_INFO_clear(info);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_DEBUG_IS_LVL1
void
  ARAD_PORT_PP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PORT_PP_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT_PP_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  arad_ARAD_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE enum_val
  )
{
  return SOC_TMC_PORT_HEADER_TYPE_to_string(enum_val);
}
const char*
  arad_ARAD_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN  ARAD_PORTS_FTMH_EXT_OUTLIF enum_val
  )
{
  return SOC_TMC_PORTS_FTMH_EXT_OUTLIF_to_string(enum_val);
}
void
  arad_ARAD_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN  ARAD_PORT2IF_MAPPING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORT2IF_MAPPING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  arad_ARAD_PORTS_SWAP_INFO_print(
    SOC_SAND_IN  ARAD_PORTS_SWAP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_PORTS_SWAP_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}



#endif 



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 
