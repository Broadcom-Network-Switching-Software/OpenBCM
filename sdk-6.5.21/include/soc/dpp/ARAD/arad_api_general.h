/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __ARAD_API_GENERAL_INCLUDED__

#define __ARAD_API_GENERAL_INCLUDED__




#include <soc/dpp/dpp_config_defs.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>







#define ARAD_NOF_TM_DOMAIN_IN_SYSTEM     (SOC_TMC_NOF_TM_DOMAIN_IN_SYSTEM)


#define ARAD_NOF_FAPS_IN_SYSTEM     (2048)
#define ARAD_INVALID_FAP_ID          ARAD_NOF_FAPS_IN_SYSTEM
#define ARAD_MAX_FAP_ID             (ARAD_NOF_FAPS_IN_SYSTEM-1)
#define ARAD_MAX_DEVICE_ID          (ARAD_NOF_FAPS_IN_SYSTEM-1)


#define ARAD_MAX_FAP_ID_IF_MC_16K_EN 511


#define ARAD_DEVICE_ID_IRRELEVANT   ARAD_MAX_DEVICE_ID


#define ARAD_NOF_SYS_PHYS_PORTS                   SOC_TMC_NOF_SYS_PHYS_PORTS_ARAD
#define ARAD_NOF_SYS_PHYS_PORTS_INDIRECT          SOC_TMC_NOF_SYS_PHYS_PORTS

#define ARAD_NOF_SYS_PHYS_PORTS_GET(unit) (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) ? \
    ARAD_NOF_SYS_PHYS_PORTS_INDIRECT : ARAD_NOF_SYS_PHYS_PORTS)

#define ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID          SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID_ARAD
#define ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT SOC_TMC_MAX_SYSTEM_PHYSICAL_PORT_ID


#define ARAD_SYS_PHYS_PORT_INVALID_DIRECT   ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID
#define ARAD_SYS_PHYS_PORT_INVALID_INDIRECT ARAD_MAX_SYSTEM_PHYSICAL_PORT_ID_INDIRECT
#define ARAD_SYS_PHYS_PORT_INVALID(unit) (ARAD_IS_VOQ_MAPPING_INDIRECT(unit) ? \
  ARAD_SYS_PHYS_PORT_INVALID_INDIRECT : ARAD_SYS_PHYS_PORT_INVALID_DIRECT)


#define ARAD_NOF_SYSTEM_PORTS              SOC_TMC_NOF_SYSTEM_PORTS_ARAD
#define ARAD_MAX_SYSTEM_PORT_ID            (ARAD_NOF_SYSTEM_PORTS - 1)


#define ARAD_NOF_LBP_DESTINATIONS   ARAD_NOF_SYS_PHYS_PORTS
#define ARAD_MAX_LBP_DESTINATION    (ARAD_NOF_LBP_DESTINATIONS - 1)


#define ARAD_MAX_QUEUE_ID(unit)           (SOC_DPP_DEFS_GET(unit, nof_queues) - 1)



#define ARAD_NOF_QUEUE_TYPES        16


#define ARAD_NOF_DROP_PRECEDENCE    4
#define ARAD_MAX_DROP_PRECEDENCE    (ARAD_NOF_DROP_PRECEDENCE-1)


#define ARAD_TR_CLS_MIN                0

#define ARAD_TR_CLS_MAX                ARAD_NOF_TRAFFIC_CLASSES-1


#define ARAD_NOF_TRAFFIC_CLASSES        SOC_TMC_NOF_TRAFFIC_CLASSES


#define ARAD_NOF_FAP_PORTS                  SOC_TMC_NOF_FAP_PORTS_ARAD
#define ARAD_MAX_FAP_PORT_ID                (ARAD_NOF_FAP_PORTS-1)
#define ARAD_FAP_PORT_ID_INVALID            ARAD_NOF_FAP_PORTS


#define ARAD_NOF_FAP_PORTS_EGR_MC_17BIT_CUD    (ARAD_NOF_FAP_PORTS / 2)
#define ARAD_MAX_FAP_PORT_ID_EGR_MC_17BIT_CUD (ARAD_NOF_FAP_PORTS_EGR_MC_17BIT_CUD-1)
#define ARAD_FAP_PORTS_OFFSET_EGR_MC_17BIT_CUD 128

#define ARAD_FAP_EGRESS_REPLICATION_BASE_Q_PAIR       248
#define ARAD_FAP_EGRESS_REPLICATION_IPS_PORT_ID       255

#define ARAD_FAP_OAMP_PORT_ID                         232

#define ARAD_WARP_CORE_0               0
#define ARAD_WARP_CORE_1               1
#define ARAD_WARP_CORE_2               2
#define ARAD_WARP_CORE_3               3
#define ARAD_WARP_CORE_4               4
#define ARAD_WARP_CORE_5               5
#define ARAD_WARP_CORE_6               6
#define ARAD_WARP_CORE_STAT            7
#define ARAD_NOF_WARP_CORES            8


#define ARAD_MAX_NOF_NIFS           (ARAD_NOF_MAC_LANES * ARAD_MAX_NIFS_PER_MAL)


#define ARAD_RATE_1K                           1000




#define ARAD_SCH_MAX_RATE_MBPS(unit)        (SOC_TMC_SCH_MAX_RATE_KBPS_ARAD(unit))
#define ARAD_IF_MAX_RATE_MBPS(unit)         (SOC_TMC_IF_MAX_RATE_MBPS_ARAD(unit))


#define ARAD_IF_MAX_RATE_KBPS(unit)         (SOC_TMC_IF_MAX_RATE_KBPS_ARAD(unit))




#define ARAD_DEFAULT_QUEUE_TYPE         (0)

#define ARAD_DEFAULT_DESTINATION_FAP    (ARAD_NOF_FAPS_IN_SYSTEM-1)

#define ARAD_DEFAULT_FAP_PORT_ID       (ARAD_NOF_FAP_PORTS-1)


#define ARAD_DEFAULT_CPU_PORT_ID       (0)




#define ARAD_MAX_QUEUE_SIZE_BYTES        (2048UL * 1024 * 1024)

#define ARAD_MAX_QUEUE_SIZE_BDS        	  (3 * 512 * 1024)

#define ARAD_B0_MAX_ECN_QUEUE_BYTES 0x7e00000 


#define ARAD_MULT_ID_MIN                 0

#define ARAD_MAX_MC_ID(unit)         ((SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids > SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) ? \
                                     (SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids - 1) : (SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1))
#define ARAD_MULT_NOF_MULTICAST_GROUPS       SOC_TMC_MULT_NOF_MULTICAST_GROUPS_ARAD

#define ARAD_MULT_NOF_MULT_VLAN_ERP_GROUPS   (8*1024) 


#define ARAD_NOF_SNOOP_COMMAND_INDEX   15
#define ARAD_MAX_SNOOP_COMMAND_INDEX   (ARAD_NOF_SNOOP_COMMAND_INDEX)

#define ARAD_NOF_SIGNATURE        4
#define ARAD_MAX_SIGNATURE        (ARAD_NOF_SIGNATURE-1)


#define ARAD_NOF_PACKET_BYTES     (16*1024)
#define ARAD_MAX_PACKET_BYTES     (ARAD_NOF_PACKET_BYTES-1)


#define ARAD_NOF_CUD_ID         65536
#define ARAD_MAX_CUD_ID         (ARAD_NOF_CUD_ID-1)


#define ARAD_NOF_LAG_GROUPS_MAX          (1024)
#define ARAD_NOF_LAG_GROUPS              (arad_ports_lag_nof_lag_groups_get_unsafe(unit))
#define ARAD_MAX_LAG_GROUP_ID            (ARAD_NOF_LAG_GROUPS - 1)


#define ARAD_NOF_LAG_ENTRIES             (arad_ports_lag_nof_lag_entries_get_unsafe(unit))
#define ARAD_MAX_LAG_ENTRY_ID            (ARAD_NOF_LAG_ENTRIES - 1)


#define ARAD_NOF_CALENDAR_SETS           2

#define ARAD_CONNECTION_DIRECTION_NDX_MAX         (SOC_TMC_NOF_CONNECTION_DIRECTIONS-1)
#define ARAD_PORT_DIRECTION_NDX_MAX               (SOC_TMC_NOF_CONNECTION_DIRECTIONS-1)






#define ARAD_DO_NOTHING_AND_EXIT                       goto exit


#define ARAD_FRST_CPU_PORT_ID              0


#define ARAD_OLP_PORT_ID                   240

#define ARAD_ERP_PORT_ID                   ARAD_FAP_EGRESS_REPLICATION_IPS_PORT_ID

#define ARAD_OAMP_PORT_ID                  ARAD_FAP_OAMP_PORT_ID

#define ARAD_IS_OLP_FAP_PORT_ID(port_id) ((port_id) == ARAD_OLP_PORT_ID)



#define ARAD_IS_ERP_PORT_ID(port_id) ((port_id) == ARAD_ERP_PORT_ID)


#define ARAD_IS_OAMP_FAP_PORT_ID(port_id) ((port_id) == ARAD_OAMP_PORT_ID)

#define ARAD_IS_ERR_RES(res_)                       \
  (soc_sand_get_error_code_from_error_word(res_) != SOC_SAND_OK)


#define ARAD_IS_NIF_ID(if_id) \
  ((if_id) <= ARAD_MAX_NIF_ID)

#define ARAD_IS_NON_NIF_ID(if_id) \
  (!ARAD_IS_NIF_ID(if_id))

#define ARAD_IS_CPU_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_CPU)

#define ARAD_IS_OLP_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_OLP)

#define ARAD_IS_OAMP_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_OAMP)

#define ARAD_IS_RCY_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_RCY)

#define ARAD_IS_TM_INTERNAL_PKT_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_TM_INTERNAL_PKT)

#define ARAD_IS_ERP_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_ERP)

#define ARAD_IS_ECI_IF_ID(if_id) \
  ( ARAD_IS_CPU_IF_ID(if_id) || ARAD_IS_OLP_IF_ID(if_id) || ARAD_IS_OAMP_IF_ID(if_id) )

#define ARAD_IS_NONE_IF_ID(if_id) \
  ((if_id) == ARAD_IF_ID_NONE)

#define ARAD_IS_MAL_EQUIVALENT_ID(mal_id) \
  (ARAD_IS_ECI_IF_ID(mal_id) || ARAD_IS_RCY_IF_ID(mal_id) || ARAD_IS_ERP_IF_ID(mal_id))


#define ARAD_NIF2WC_NDX(if_id) \
  ((uint32)((if_id) / ARAD_MAX_NIFS_PER_WC))

#define ARAD_WC2NIF_NDX(wc_id) \
  ((wc_id) * ARAD_MAX_NIFS_PER_WC)

#define ARAD_IF2WC_NDX(if_id) \
  (ARAD_IS_NIF_ID(if_id)?ARAD_NIF2WC_NDX(if_id):(uint32)(if_id))

#define ARAD_WC2IF_NDX(wc_id) \
  ((wc_id)<ARAD_NOF_WRAP_CORE?ARAD_WC2NIF_NDX(wc_id):(wc_id))


#define ARAD_IS_WC_NIF_ID(if_id) \
  (uint8)( (if_id <= ARAD_INTERN_MAX_NIFS) && \
   (ARAD_WC2NIF_NDX(ARAD_NIF2WC_NDX(if_id)) == (uint32)(if_id)) )

#define ARAD_IS_DIRECTION_RX(dir) \
  SOC_SAND_NUM2BOOL(((dir) == ARAD_CONNECTION_DIRECTION_RX  ) || ((dir) == ARAD_CONNECTION_DIRECTION_BOTH))

#define ARAD_IS_DIRECTION_TX(dir) \
  SOC_SAND_NUM2BOOL(((dir) == ARAD_CONNECTION_DIRECTION_TX  ) || ((dir) == ARAD_CONNECTION_DIRECTION_BOTH))

#define ARAD_IS_DIRECTION_REC(dir) \
  SOC_SAND_NUM2BOOL((dir) == ARAD_FC_DIRECTION_REC  )

#define ARAD_IS_DIRECTION_GEN(dir) \
  SOC_SAND_NUM2BOOL((dir) == ARAD_FC_DIRECTION_GEN  )

#define ARAD_MULTICAST_DEST_INDICATION    (SOC_SAND_BIT(14))


#define ARAD_IF_NOF_NIFS 32

#define ARAD_INTERN_MAX_NIFS 64





#define ARAD_FAR_DEVICE_TYPE_FE1                          SOC_TMC_FAR_DEVICE_TYPE_FE1
#define ARAD_FAR_DEVICE_TYPE_FE2                          SOC_TMC_FAR_DEVICE_TYPE_FE2
#define ARAD_FAR_DEVICE_TYPE_FE3                          SOC_TMC_FAR_DEVICE_TYPE_FE3
#define ARAD_FAR_DEVICE_TYPE_FAP                          SOC_TMC_FAR_DEVICE_TYPE_FAP
typedef SOC_TMC_FAR_DEVICE_TYPE                                ARAD_FAR_DEVICE_TYPE;

#define ARAD_IF_TYPE_NONE                                 SOC_TMC_IF_TYPE_NONE
#define ARAD_IF_TYPE_CPU                                  SOC_TMC_IF_TYPE_CPU
#define ARAD_IF_TYPE_RCY                                  SOC_TMC_IF_TYPE_RCY
#define ARAD_IF_TYPE_OLP                                  SOC_TMC_IF_TYPE_OLP
#define ARAD_IF_TYPE_NIF                                  SOC_TMC_IF_TYPE_NIF
#define ARAD_IF_TYPE_ERP                                  SOC_TMC_IF_TYPE_ERP
#define ARAD_IF_TYPE_OAMP                                 SOC_TMC_IF_TYPE_OAMP
typedef SOC_TMC_INTERFACE_TYPE                                 ARAD_INTERFACE_TYPE;

#define ARAD_IF_ID_0                                      SOC_TMC_IF_ID_0
#define ARAD_IF_ID_1                                      SOC_TMC_IF_ID_1
#define ARAD_IF_ID_2                                      SOC_TMC_IF_ID_2
#define ARAD_IF_ID_3                                      SOC_TMC_IF_ID_3
#define ARAD_IF_ID_4                                      SOC_TMC_IF_ID_4
#define ARAD_IF_ID_5                                      SOC_TMC_IF_ID_5
#define ARAD_IF_ID_6                                      SOC_TMC_IF_ID_6
#define ARAD_IF_ID_7                                      SOC_TMC_IF_ID_7
#define ARAD_IF_ID_8                                      SOC_TMC_IF_ID_8
#define ARAD_IF_ID_9                                      SOC_TMC_IF_ID_9
#define ARAD_IF_ID_10                                     SOC_TMC_IF_ID_10
#define ARAD_IF_ID_11                                     SOC_TMC_IF_ID_11
#define ARAD_IF_ID_12                                     SOC_TMC_IF_ID_12
#define ARAD_IF_ID_13                                     SOC_TMC_IF_ID_13
#define ARAD_IF_ID_14                                     SOC_TMC_IF_ID_14
#define ARAD_IF_ID_15                                     SOC_TMC_IF_ID_15
#define ARAD_IF_ID_16                                     SOC_TMC_IF_ID_16
#define ARAD_IF_ID_17                                     SOC_TMC_IF_ID_17
#define ARAD_IF_ID_18                                     SOC_TMC_IF_ID_18
#define ARAD_IF_ID_19                                     SOC_TMC_IF_ID_19
#define ARAD_IF_ID_20                                     SOC_TMC_IF_ID_20
#define ARAD_IF_ID_21                                     SOC_TMC_IF_ID_21
#define ARAD_IF_ID_22                                     SOC_TMC_IF_ID_22
#define ARAD_IF_ID_23                                     SOC_TMC_IF_ID_23
#define ARAD_IF_ID_24                                     SOC_TMC_IF_ID_24
#define ARAD_IF_ID_25                                     SOC_TMC_IF_ID_25
#define ARAD_IF_ID_26                                     SOC_TMC_IF_ID_26
#define ARAD_IF_ID_27                                     SOC_TMC_IF_ID_27
#define ARAD_IF_ID_28                                     SOC_TMC_IF_ID_28
#define ARAD_IF_ID_29                                     SOC_TMC_IF_ID_29
#define ARAD_IF_ID_30                                     SOC_TMC_IF_ID_30
#define ARAD_IF_ID_31                                     SOC_TMC_IF_ID_31
#define ARAD_NIF_ID_XAUI_0                                SOC_TMC_NIF_ID_XAUI_0
#define ARAD_NIF_ID_XAUI_1                                SOC_TMC_NIF_ID_XAUI_1
#define ARAD_NIF_ID_XAUI_2                                SOC_TMC_NIF_ID_XAUI_2
#define ARAD_NIF_ID_XAUI_3                                SOC_TMC_NIF_ID_XAUI_3
#define ARAD_NIF_ID_XAUI_4                                SOC_TMC_NIF_ID_XAUI_4
#define ARAD_NIF_ID_XAUI_5                                SOC_TMC_NIF_ID_XAUI_5
#define ARAD_NIF_ID_XAUI_6                                SOC_TMC_NIF_ID_XAUI_6
#define ARAD_NIF_ID_XAUI_7                                SOC_TMC_NIF_ID_XAUI_7
#define ARAD_NIF_ID_RXAUI_0                               SOC_TMC_NIF_ID_RXAUI_0
#define ARAD_NIF_ID_RXAUI_1                               SOC_TMC_NIF_ID_RXAUI_1
#define ARAD_NIF_ID_RXAUI_2                               SOC_TMC_NIF_ID_RXAUI_2
#define ARAD_NIF_ID_RXAUI_3                               SOC_TMC_NIF_ID_RXAUI_3
#define ARAD_NIF_ID_RXAUI_4                               SOC_TMC_NIF_ID_RXAUI_4
#define ARAD_NIF_ID_RXAUI_5                               SOC_TMC_NIF_ID_RXAUI_5
#define ARAD_NIF_ID_RXAUI_6                               SOC_TMC_NIF_ID_RXAUI_6
#define ARAD_NIF_ID_RXAUI_7                               SOC_TMC_NIF_ID_RXAUI_7
#define ARAD_NIF_ID_RXAUI_8                               SOC_TMC_NIF_ID_RXAUI_8
#define ARAD_NIF_ID_RXAUI_9                               SOC_TMC_NIF_ID_RXAUI_9
#define ARAD_NIF_ID_RXAUI_10                              SOC_TMC_NIF_ID_RXAUI_10
#define ARAD_NIF_ID_RXAUI_11                              SOC_TMC_NIF_ID_RXAUI_11
#define ARAD_NIF_ID_RXAUI_12                              SOC_TMC_NIF_ID_RXAUI_12
#define ARAD_NIF_ID_RXAUI_13                              SOC_TMC_NIF_ID_RXAUI_13
#define ARAD_NIF_ID_RXAUI_14                              SOC_TMC_NIF_ID_RXAUI_14
#define ARAD_NIF_ID_RXAUI_15                              SOC_TMC_NIF_ID_RXAUI_15
#define ARAD_NIF_ID_SGMII_0                               SOC_TMC_NIF_ID_SGMII_0
#define ARAD_NIF_ID_SGMII_1                               SOC_TMC_NIF_ID_SGMII_1
#define ARAD_NIF_ID_SGMII_2                               SOC_TMC_NIF_ID_SGMII_2
#define ARAD_NIF_ID_SGMII_3                               SOC_TMC_NIF_ID_SGMII_3
#define ARAD_NIF_ID_SGMII_4                               SOC_TMC_NIF_ID_SGMII_4
#define ARAD_NIF_ID_SGMII_5                               SOC_TMC_NIF_ID_SGMII_5
#define ARAD_NIF_ID_SGMII_6                               SOC_TMC_NIF_ID_SGMII_6
#define ARAD_NIF_ID_SGMII_7                               SOC_TMC_NIF_ID_SGMII_7
#define ARAD_NIF_ID_SGMII_8                               SOC_TMC_NIF_ID_SGMII_8
#define ARAD_NIF_ID_SGMII_9                               SOC_TMC_NIF_ID_SGMII_9
#define ARAD_NIF_ID_SGMII_10                              SOC_TMC_NIF_ID_SGMII_10
#define ARAD_NIF_ID_SGMII_11                              SOC_TMC_NIF_ID_SGMII_11
#define ARAD_NIF_ID_SGMII_12                              SOC_TMC_NIF_ID_SGMII_12
#define ARAD_NIF_ID_SGMII_13                              SOC_TMC_NIF_ID_SGMII_13
#define ARAD_NIF_ID_SGMII_14                              SOC_TMC_NIF_ID_SGMII_14
#define ARAD_NIF_ID_SGMII_15                              SOC_TMC_NIF_ID_SGMII_15
#define ARAD_NIF_ID_SGMII_16                              SOC_TMC_NIF_ID_SGMII_16
#define ARAD_NIF_ID_SGMII_17                              SOC_TMC_NIF_ID_SGMII_17
#define ARAD_NIF_ID_SGMII_18                              SOC_TMC_NIF_ID_SGMII_18
#define ARAD_NIF_ID_SGMII_19                              SOC_TMC_NIF_ID_SGMII_19
#define ARAD_NIF_ID_SGMII_20                              SOC_TMC_NIF_ID_SGMII_20
#define ARAD_NIF_ID_SGMII_21                              SOC_TMC_NIF_ID_SGMII_21
#define ARAD_NIF_ID_SGMII_22                              SOC_TMC_NIF_ID_SGMII_22
#define ARAD_NIF_ID_SGMII_23                              SOC_TMC_NIF_ID_SGMII_23
#define ARAD_NIF_ID_SGMII_24                              SOC_TMC_NIF_ID_SGMII_24
#define ARAD_NIF_ID_SGMII_25                              SOC_TMC_NIF_ID_SGMII_25
#define ARAD_NIF_ID_SGMII_26                              SOC_TMC_NIF_ID_SGMII_26
#define ARAD_NIF_ID_SGMII_27                              SOC_TMC_NIF_ID_SGMII_27
#define ARAD_NIF_ID_SGMII_28                              SOC_TMC_NIF_ID_SGMII_28
#define ARAD_NIF_ID_SGMII_29                              SOC_TMC_NIF_ID_SGMII_29
#define ARAD_NIF_ID_SGMII_30                              SOC_TMC_NIF_ID_SGMII_30
#define ARAD_NIF_ID_SGMII_31                              SOC_TMC_NIF_ID_SGMII_31
#define ARAD_NIF_ID_ILKN_0                                SOC_TMC_NIF_ID_ILKN_0
#define ARAD_NIF_ID_ILKN_1                                SOC_TMC_NIF_ID_ILKN_1
#define ARAD_NIF_ID_ILKN_TDM_0                            SOC_TMC_NIF_ID_ILKN_TDM_0
#define ARAD_NIF_ID_ILKN_TDM_1                            SOC_TMC_NIF_ID_ILKN_TDM_1
#define ARAD_NIF_ID_CGE_0                                 SOC_TMC_NIF_ID_CGE_0
#define ARAD_NIF_ID_CGE_1                                 SOC_TMC_NIF_ID_CGE_1
#define ARAD_NIF_ID_10GBASE_R_0                           SOC_TMC_NIF_ID_10GBASE_R_0
#define ARAD_NIF_ID_10GBASE_R_1                           SOC_TMC_NIF_ID_10GBASE_R_1
#define ARAD_NIF_ID_10GBASE_R_2                           SOC_TMC_NIF_ID_10GBASE_R_2
#define ARAD_NIF_ID_10GBASE_R_3                           SOC_TMC_NIF_ID_10GBASE_R_3
#define ARAD_NIF_ID_10GBASE_R_4                           SOC_TMC_NIF_ID_10GBASE_R_4
#define ARAD_NIF_ID_10GBASE_R_5                           SOC_TMC_NIF_ID_10GBASE_R_5
#define ARAD_NIF_ID_10GBASE_R_6                           SOC_TMC_NIF_ID_10GBASE_R_6
#define ARAD_NIF_ID_10GBASE_R_7                           SOC_TMC_NIF_ID_10GBASE_R_7
#define ARAD_NIF_ID_10GBASE_R_8                           SOC_TMC_NIF_ID_10GBASE_R_8
#define ARAD_NIF_ID_10GBASE_R_9                           SOC_TMC_NIF_ID_10GBASE_R_9
#define ARAD_NIF_ID_10GBASE_R_10                          SOC_TMC_NIF_ID_10GBASE_R_10
#define ARAD_NIF_ID_10GBASE_R_11                          SOC_TMC_NIF_ID_10GBASE_R_11
#define ARAD_NIF_ID_10GBASE_R_12                          SOC_TMC_NIF_ID_10GBASE_R_12
#define ARAD_NIF_ID_10GBASE_R_13                          SOC_TMC_NIF_ID_10GBASE_R_13
#define ARAD_NIF_ID_10GBASE_R_14                          SOC_TMC_NIF_ID_10GBASE_R_14
#define ARAD_NIF_ID_10GBASE_R_15                          SOC_TMC_NIF_ID_10GBASE_R_15
#define ARAD_NIF_ID_10GBASE_R_16                          SOC_TMC_NIF_ID_10GBASE_R_16
#define ARAD_NIF_ID_10GBASE_R_17                          SOC_TMC_NIF_ID_10GBASE_R_17
#define ARAD_NIF_ID_10GBASE_R_18                          SOC_TMC_NIF_ID_10GBASE_R_18
#define ARAD_NIF_ID_10GBASE_R_19                          SOC_TMC_NIF_ID_10GBASE_R_19
#define ARAD_NIF_ID_10GBASE_R_20                          SOC_TMC_NIF_ID_10GBASE_R_20
#define ARAD_NIF_ID_10GBASE_R_21                          SOC_TMC_NIF_ID_10GBASE_R_21
#define ARAD_NIF_ID_10GBASE_R_22                          SOC_TMC_NIF_ID_10GBASE_R_22
#define ARAD_NIF_ID_10GBASE_R_23                          SOC_TMC_NIF_ID_10GBASE_R_23
#define ARAD_NIF_ID_10GBASE_R_24                          SOC_TMC_NIF_ID_10GBASE_R_24
#define ARAD_NIF_ID_10GBASE_R_25                          SOC_TMC_NIF_ID_10GBASE_R_25
#define ARAD_NIF_ID_10GBASE_R_26                          SOC_TMC_NIF_ID_10GBASE_R_26
#define ARAD_NIF_ID_10GBASE_R_27                          SOC_TMC_NIF_ID_10GBASE_R_27
#define ARAD_NIF_ID_10GBASE_R_28                          SOC_TMC_NIF_ID_10GBASE_R_28
#define ARAD_NIF_ID_10GBASE_R_29                          SOC_TMC_NIF_ID_10GBASE_R_29
#define ARAD_NIF_ID_10GBASE_R_30                          SOC_TMC_NIF_ID_10GBASE_R_30
#define ARAD_NIF_ID_10GBASE_R_31                          SOC_TMC_NIF_ID_10GBASE_R_31
#define ARAD_NIF_ID_XLGE_0                                SOC_TMC_NIF_ID_XLGE_0
#define ARAD_NIF_ID_XLGE_1                                SOC_TMC_NIF_ID_XLGE_1
#define ARAD_NIF_ID_XLGE_2                                SOC_TMC_NIF_ID_XLGE_2
#define ARAD_NIF_ID_XLGE_3                                SOC_TMC_NIF_ID_XLGE_3
#define ARAD_NIF_ID_XLGE_4                                SOC_TMC_NIF_ID_XLGE_4
#define ARAD_NIF_ID_XLGE_5                                SOC_TMC_NIF_ID_XLGE_5
#define ARAD_NIF_ID_XLGE_6                                SOC_TMC_NIF_ID_XLGE_6
#define ARAD_NIF_ID_XLGE_7                                SOC_TMC_NIF_ID_XLGE_7

#define ARAD_IF_ID_CPU                                    SOC_TMC_IF_ID_CPU
#define ARAD_IF_ID_OLP                                    SOC_TMC_IF_ID_OLP
#define ARAD_IF_ID_RCY                                    SOC_TMC_IF_ID_RCY
#define ARAD_IF_ID_ERP                                    SOC_TMC_IF_ID_ERP
#define ARAD_IF_ID_OAMP                                   SOC_TMC_IF_ID_OAMP
#define ARAD_IF_ID_TM_INTERNAL_PKT                        SOC_TMC_IF_ID_TM_INTERNAL_PKT
#define ARAD_IF_ID_RESERVED                               SOC_TMC_IF_ID_RESERVED
typedef SOC_TMC_INTERFACE_ID                              ARAD_INTERFACE_ID;

#define ARAD_FC_DIRECTION_GEN                             SOC_TMC_FC_DIRECTION_GEN
#define ARAD_FC_DIRECTION_REC                             SOC_TMC_FC_DIRECTION_REC
#define ARAD_FC_NOF_DIRECTIONS                            SOC_TMC_FC_NOF_DIRECTIONS
typedef SOC_TMC_FC_DIRECTION                                   ARAD_FC_DIRECTION;

#define ARAD_COMBO_QRTT_0                                 SOC_TMC_COMBO_QRTT_0
#define ARAD_COMBO_QRTT_1                                 SOC_TMC_COMBO_QRTT_1
#define ARAD_COMBO_NOF_QRTTS                              SOC_TMC_COMBO_NOF_QRTTS
typedef SOC_TMC_COMBO_QRTT                                     ARAD_COMBO_QRTT;

#define ARAD_DEST_TYPE_QUEUE                              SOC_TMC_DEST_TYPE_QUEUE
#define ARAD_DEST_TYPE_MULTICAST                          SOC_TMC_DEST_TYPE_MULTICAST
#define ARAD_DEST_TYPE_SYS_PHY_PORT                       SOC_TMC_DEST_TYPE_SYS_PHY_PORT
#define ARAD_DEST_TYPE_LAG                                SOC_TMC_DEST_TYPE_LAG
#define ARAD_DEST_TYPE_ISQ_FLOW_ID                   SOC_TMC_DEST_TYPE_ISQ_FLOW_ID
#define ARAD_DEST_TYPE_OUT_LIF                           SOC_TMC_DEST_TYPE_OUT_LIF
#define ARAD_DEST_TYPE_MULTICAST_FLOW_ID       SOC_TMC_DEST_TYPE_MULTICAST_FLOW_ID
#define ARAD_NOF_DEST_TYPES                               SOC_TMC_NOF_DEST_TYPES_ARAD
#define ARAD_DEST_TYPE_FEC_PTR                            SOC_TMC_DEST_TYPE_FEC_PTR
typedef SOC_TMC_DEST_TYPE                                      ARAD_DEST_TYPE;

#define ARAD_DEST_SYS_PORT_TYPE_SYS_PHY_PORT              SOC_TMC_DEST_SYS_PORT_TYPE_SYS_PHY_PORT
#define ARAD_DEST_SYS_PORT_TYPE_LAG                       SOC_TMC_DEST_SYS_PORT_TYPE_LAG
#define ARAD_DEST_SYS_PORT_NOF_TYPES                      SOC_TMC_DEST_SYS_PORT_NOF_TYPES
typedef SOC_TMC_DEST_SYS_PORT_TYPE                             ARAD_DEST_SYS_PORT_TYPE;

#define ARAD_CONNECTION_DIRECTION_RX                      SOC_TMC_CONNECTION_DIRECTION_RX
#define ARAD_CONNECTION_DIRECTION_TX                      SOC_TMC_CONNECTION_DIRECTION_TX
#define ARAD_CONNECTION_DIRECTION_BOTH                    SOC_TMC_CONNECTION_DIRECTION_BOTH
#define ARAD_NOF_CONNECTION_DIRECTIONS                    SOC_TMC_NOF_CONNECTION_DIRECTIONS
typedef SOC_TMC_CONNECTION_DIRECTION                           ARAD_CONNECTION_DIRECTION;

typedef SOC_TMC_DEST_SYS_PORT_INFO                             ARAD_DEST_SYS_PORT_INFO;
typedef SOC_TMC_DEST_INFO                                      ARAD_DEST_INFO;

#define ARAD_IF_ID_NONE                                   SOC_TMC_IF_ID_NONE
#define ARAD_NIF_ID_NONE                                  SOC_TMC_NIF_ID_NONE

typedef SOC_TMC_THRESH_WITH_HYST_INFO                          ARAD_THRESH_WITH_HYST_INFO;

#define ARAD_NOF_TCGS                                      SOC_TMC_NOF_TCGS                
#define ARAD_TCG_MIN                                       SOC_TMC_TCG_MIN
#define ARAD_TCG_MAX                                       SOC_TMC_TCG_MAX                


#define ARAD_TCG_NOF_PRIORITIES_SUPPORT                    (SOC_TMC_TCG_NOF_PRIORITIES_SUPPORT)


#define ARAD_NOF_TCG_IDS                                   SOC_TMC_NOF_TCG_IDS


typedef SOC_TMC_MULT_ID ARAD_MULT_ID;


typedef SOC_TMC_TR_CLS ARAD_TR_CLS;


typedef SOC_TMC_TCG_NDX ARAD_TCG_NDX;


typedef SOC_TMC_FAP_PORT_ID ARAD_FAP_PORT_ID;


typedef SOC_TMC_PON_TUNNEL_ID ARAD_PON_TUNNEL_ID;

typedef enum
{
  ARAD_OLP_ID       = 0 ,
  ARAD_IRE_ID       = 1 ,
  ARAD_IDR_ID       = 2 ,
  ARAD_IRR_ID       = 3 ,
  ARAD_IHP_ID       = 4 ,
  ARAD_QDR_ID       = 5 ,
  ARAD_IPS_ID       = 6 ,
  ARAD_IPT_ID       = 7 ,
  ARAD_DPI_A_ID     = 8 ,
  ARAD_DPI_B_ID     = 9 ,
  ARAD_DPI_C_ID     = 10,
  ARAD_DPI_D_ID     = 11,
  ARAD_DPI_E_ID     = 12,
  ARAD_DPI_F_ID     = 13,
  ARAD_RTP_ID       = 14,
  ARAD_EGQ_ID       = 15,
  ARAD_SCH_ID       = 16,
  ARAD_CFC_ID       = 17,
  ARAD_EPNI_ID      = 18,
  ARAD_IQM_ID       = 19,
  ARAD_MMU_ID       = 20,
  ARAD_IHB_ID       = 21,
  ARAD_OAMP_ID      = 22,
  ARAD_NOF_MODULES  = 23
}ARAD_MODULE_ID;


#define ARAD_MAL_ID_CPU ARAD_IF_ID_CPU
#define ARAD_MAL_ID_OLP ARAD_IF_ID_OLP
#define ARAD_MAL_ID_RCY ARAD_IF_ID_RCY
#define ARAD_MAL_ID_ERP ARAD_IF_ID_ERP
#define ARAD_MAL_ID_OAMP ARAD_IF_ID_OAMP
#define ARAD_MAL_ID_NONE ARAD_IF_ID_NONE

#define ARAD_MAL_TYPE_NONE         SOC_TMC_MAL_TYPE_NONE
#define ARAD_MAL_TYPE_CPU          SOC_TMC_MAL_TYPE_CPU
#define ARAD_MAL_TYPE_RCY          SOC_TMC_MAL_TYPE_RCY
#define ARAD_MAL_TYPE_OLP          SOC_TMC_MAL_TYPE_OLP
#define ARAD_MAL_TYPE_ERP          SOC_TMC_MAL_TYPE_ERP
#define ARAD_MAL_TYPE_NIF          SOC_TMC_MAL_TYPE_NIF
#define ARAD_MAL_TYPE_OAMP         SOC_TMC_MAL_TYPE_OAMP
#define ARAD_MAL_TYPE_NOF_TYPES    SOC_TMC_MAL_TYPE_NOF_TYPES
typedef SOC_TMC_MAL_EQUIVALENT_TYPE     ARAD_MAL_EQUIVALENT_TYPE;


#define ARAD_MAX_IF_ID ARAD_IF_ID_ERP

typedef enum
{
  
  ARAD_MULTICAST_CLASS_0=0,
  
  ARAD_MULTICAST_CLASS_1=1,
  
  ARAD_MULTICAST_CLASS_2=2,
  
  ARAD_MULTICAST_CLASS_3=3,
  
  ARAD_NOF_MULTICAST_CLASSES=4,
  
  ARAD_MULTICAST_CLASS_LAST

}ARAD_MULTICAST_CLASS;


#define ARAD_COMBO2NIF_GRP_IDX(combo_id) (combo_id)


#define ARAD_SWAP_MODE_4_BYTES          SOC_TMC_SWAP_MODE_4_BYTES
#define ARAD_SWAP_MODE_8_BYTES          SOC_TMC_SWAP_MODE_8_BYTES
#define ARAD_SWAP_MODES                 SOC_TMC_SWAP_MODES

typedef SOC_TMC_SWAP_MODE               ARAD_SWAP_MODE;
typedef SOC_TMC_SWAP_INFO               ARAD_SWAP_INFO;











uint32
  arad_fap_port_id_verify(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  ARAD_FAP_PORT_ID  port_id
  );

uint32
  arad_drop_precedence_verify(
    SOC_SAND_IN  uint32      dp_ndx
  );

uint32
  arad_traffic_class_verify(
    SOC_SAND_IN  uint32      tc_ndx
  );


uint32
  arad_interface_id_verify(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  ARAD_INTERFACE_ID     if_ndx
  );

void
  arad_ARAD_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_DEST_SYS_PORT_INFO *info
  );

void
  arad_ARAD_DEST_INFO_clear(
    SOC_SAND_OUT ARAD_DEST_INFO *info
  );

void
  arad_ARAD_THRESH_WITH_HYST_INFO_clear(
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO *info
  );


void
  arad_ARAD_SWAP_INFO_clear(
    SOC_SAND_OUT ARAD_SWAP_INFO *info
  );

#if ARAD_DEBUG_IS_LVL1
const char*
  arad_ARAD_COMBO_QRTT_to_string(
    SOC_SAND_IN ARAD_COMBO_QRTT enum_val
  );

const char*
  arad_ARAD_FAR_DEVICE_TYPE_to_string(
    SOC_SAND_IN ARAD_FAR_DEVICE_TYPE enum_val
  );

const char*
  arad_ARAD_INTERFACE_TYPE_to_string(
    SOC_SAND_IN ARAD_INTERFACE_TYPE enum_val
  );

const char*
  arad_ARAD_INTERFACE_ID_to_string(
    SOC_SAND_IN ARAD_INTERFACE_ID enum_val
  );

const char*
  arad_ARAD_FC_DIRECTION_to_string(
    SOC_SAND_IN ARAD_FC_DIRECTION enum_val
  );

const char*
  arad_ARAD_CONNECTION_DIRECTION_to_string(
    SOC_SAND_IN ARAD_CONNECTION_DIRECTION enum_val
  );

const char*
  arad_ARAD_DEST_TYPE_to_string(
    SOC_SAND_IN ARAD_DEST_TYPE enum_val,
    SOC_SAND_IN uint8       short_name
  );

const char*
  arad_ARAD_DEST_SYS_PORT_TYPE_to_string(
    SOC_SAND_IN ARAD_DEST_SYS_PORT_TYPE enum_val
  );

void
  arad_ARAD_INTERFACE_ID_print(
    SOC_SAND_IN ARAD_INTERFACE_ID if_ndx
  );

void
  arad_ARAD_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN ARAD_DEST_SYS_PORT_INFO *info
  );

void
  arad_ARAD_DEST_INFO_print(
    SOC_SAND_IN ARAD_DEST_INFO *info
  );

void
  arad_ARAD_THRESH_WITH_HYST_INFO_print(
    SOC_SAND_IN ARAD_THRESH_WITH_HYST_INFO *info
  );

void
  arad_ARAD_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN ARAD_DEST_SYS_PORT_INFO *info
  );


const char*
  arad_ARAD_SWAP_MODE_to_string(
     SOC_SAND_IN ARAD_SWAP_MODE enum_val
  );


void
  arad_ARAD_SWAP_INFO_print(
    SOC_SAND_IN ARAD_SWAP_INFO *info
  );

#endif 




#include <soc/dpp/SAND/Utils/sand_footer.h>



#endif


