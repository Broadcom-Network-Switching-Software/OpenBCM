/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_ACTION_CMD_INCLUDED__

#define __ARAD_ACTION_CMD_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_action_cmd.h>
#include <soc/dpp/ARAD/arad_framework.h>





#define ARAD_ACTION_NDX_MIN                                      (1)
#define ARAD_ACTION_NDX_MAX                                      (15)


#define ENCODING_VALUE(mask) ((SOC_DPP_IMP_DEFS_GET(unit,mask)) & 0xff)
#define ENCODING_START(mask) ((SOC_DPP_IMP_DEFS_GET(unit,mask)>>8) & 0xff)
#define ENCODING_END(mask) ((SOC_DPP_IMP_DEFS_GET(unit,mask))>>16)
#define DESTINATION_ENCODING(mask) (ENCODING_VALUE(mask) << ENCODING_START(mask))
#define DESTINATION_ENCODING_MASK(mask) ((1<<ENCODING_END(mask) - 1<<ENCODING_START(mask)) + 1<<ENCODING_END(mask))

#define INGRESS_DESTINATION_TYPE_QUEUE   (DESTINATION_ENCODING(mirror_snoop_destination_queue_encoding))
#define INGRESS_DESTINATION_TYPE_MULTICAST  (DESTINATION_ENCODING(mirror_snoop_destination_multicast_encoding))
#define INGRESS_DESTINATION_TYPE_SYS_PHY_PORT   (DESTINATION_ENCODING(mirror_snoop_destination_sys_phy_port_encoding))
#define INGRESS_DESTINATION_TYPE_LAG  (DESTINATION_ENCODING(mirror_snoop_destination_lag_encoding))






#define ARAD_ACTION_CMD_EXT_COUNTER_MASK 0x1FFFFF
#define ARAD_ACTION_CMD_EXT_COUNTER_SHIFT 21
#define ARAD_ACTION_CMD_EXT_COUNTER1_GET(_val)   (((_val) >> ARAD_ACTION_CMD_EXT_COUNTER_SHIFT) & ARAD_ACTION_CMD_EXT_COUNTER_MASK)
#define ARAD_ACTION_CMD_EXT_COUNTER2_GET(_val)   ((_val) & ARAD_ACTION_CMD_EXT_COUNTER_MASK)

#define ARAD_ACTION_CMD_EXT_COUNTER1_SET(_val, _id)   (_val=((_id & ARAD_ACTION_CMD_EXT_COUNTER_MASK) << ARAD_ACTION_CMD_EXT_COUNTER_SHIFT) | ARAD_ACTION_CMD_EXT_COUNTER2_GET(_val))
#define ARAD_ACTION_CMD_EXT_COUNTER2_SET(_val, _id)   (_val=((_id & ARAD_ACTION_CMD_EXT_COUNTER_MASK) | (ARAD_ACTION_CMD_EXT_COUNTER1_GET(_val) << ARAD_ACTION_CMD_EXT_COUNTER_SHIFT)))














uint32
  arad_action_cmd_snoop_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO               *info
  );

uint32
  arad_action_cmd_snoop_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO               *info
  );

uint32
  arad_action_cmd_snoop_get_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx
  );


uint32
  arad_action_cmd_snoop_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO               *info,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                                     cmd_type
  );


uint32
dpp_snoop_mirror_stamping_config_get(
   SOC_SAND_IN  int             unit,
   SOC_SAND_IN  SOC_TMC_CMD_TYPE cmnd_type,
   SOC_SAND_IN  int              cmd,
   SOC_SAND_OUT  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO  *info
   );

uint32
dpp_snoop_mirror_stamping_config_set(
   SOC_SAND_IN  int              unit,
   SOC_SAND_IN  SOC_TMC_CMD_TYPE cmnd_type,
   SOC_SAND_IN  int              cmd,
   SOC_SAND_IN  SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO  *info
   );

uint32
  arad_action_cmd_mirror_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                   cmnd_type,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO              *info
  );

uint32
  arad_action_cmd_mirror_set_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO              *info
  );

uint32
  arad_action_cmd_mirror_get_verify(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx
  );


uint32
  arad_action_cmd_mirror_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              action_ndx,
    SOC_SAND_IN  SOC_TMC_CMD_TYPE                               cmnd_type,
    SOC_SAND_OUT ARAD_ACTION_CMD_SNOOP_MIRROR_INFO              *info
  );

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_ACTION_CMD_OVERRIDE_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_OVERRIDE *info
  );

uint32
  ARAD_ACTION_CMD_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD *info
  );

uint32
  ARAD_ACTION_CMD_SNOOP_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

uint32
  ARAD_ACTION_CMD_MIRROR_INFO_verify(
    SOC_SAND_IN  ARAD_ACTION_CMD_SNOOP_MIRROR_INFO *info
  );

#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif


