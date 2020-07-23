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

#define _ERR_MSG_MODULE_NAME BSL_SOC_TDM


#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_dev_feature_manager.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_api_tdm.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_api_fabric.h>
#include <soc/dpp/ARAD/arad_api_nif.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/drv.h>




#define ARAD_EGQ_TDM_MAP_QUEUE_TO_TDM_REG_MULT_NOF_REGS          (8)
#define ARAD_TDM_PORT_NDX_MAX                                    (255)
#define ARAD_TDM_PB_CELL_SIZE_MAX                                (128)
#define ARAD_TDM_IS_TDM_MAX                                      (2)
#define ARAD_TDM_FTMH_OPT_UC_DEST_IF_MAX                         (255)
#define ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_MAX                     (1*1024-1)
#define ARAD_TDM_FTMH_OPT_MC_MC_ID_MAX                           (64*1024-1)
#define ARAD_TDM_FTMH_STANDARD_UC_DEST_FAP_PORT_MAX              (255)
#define ARAD_TDM_FTMH_STANDARD_UC_DEST_FAP_ID_MAX                (2*1024-1)
#define ARAD_TDM_FTMH_STANDARD_UC_USER_DEFINE_2_MAX              (128*1024-1)
#define ARAD_TDM_FTMH_STANDARD_MC_MC_ID_MAX                      (64*1024-1)
#define ARAD_TDM_FTMH_STANDARD_MC_USER_DEF_MAX                   (8192*1024 - 1)
#define ARAD_TDM_FTMH_INFO_ACTION_ING_MAX                        (ARAD_TDM_NOF_ING_ACTIONS-1)
#define ARAD_TDM_FTMH_INFO_ACTION_EG_MAX                         (ARAD_TDM_NOF_EG_ACTIONS-1)
#define ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_MAX                  (35)
#define ARAD_TDM_FRAGMENT_NUM_VSC_128                            (0x180)
#define ARAD_TDM_FRAGMENT_NUM_VSC_256                            (0x1)
#define ARAD_TDM_FTMH_OPT_TYPE_UC                                (0)
#define ARAD_TDM_FTMH_OPT_TYPE_MC                                (1)

#define ARAD_TDM_FTMH_PB_OPT_MODE_VAL_FLD                        (0x2)
#define ARAD_TDM_FTMH_PB_STA_MODE_VAL_FLD                        (0x1)
#define ARAD_TDM_FTMH_UNCHANGED_MODE_VAL_FLD                     (0x0)
#define ARAD_TDM_FTMH_EXTERNAL_MODE_VAL_FLD                      (0x3)
#define ARAD_TDM_FTMH_ARAD_OPT_MODE_VAL_FLD                      (SOC_IS_JERICHO(unit)?(0x2):(0x4))
#define ARAD_TDM_FTMH_ARAD_STA_MODE_VAL_FLD                      (SOC_IS_JERICHO(unit)?(0x1):(0x5))


#define ARAD_TDM_PB_FTMH_OPT_TYPE_START_BIT                         (71)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_LSB                         (66)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_MSB                         (70)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_NOF_BITS                    (ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_MSB - ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_LSB + 1)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_LSB                     (56)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_MSB                     (65)
#define ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS                (ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_MSB - ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_LSB + 1)


#define ARAD_TDM_ARAD_FTMH_OPT_TYPE_START_BIT                       (63)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_LSB                       (40)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_MSB                       (47)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_NOF_BITS                  (ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_MSB - ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_LSB                   (48)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_MSB                   (58)
#define ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS              (ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_MSB - ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_LSB + 1)


#define ARAD_TDM_FTMH_OPT_TYPE_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_TYPE_START_BIT:ARAD_TDM_ARAD_FTMH_OPT_TYPE_START_BIT)
#define ARAD_TDM_FTMH_OPT_UC_DEST_IF_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_LSB:ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_LSB)
#define ARAD_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_UC_DEST_IF_NOF_BITS:ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_IF_NOF_BITS)
#define ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_LSB:ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_LSB)
#define ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS:ARAD_TDM_ARAD_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS)


#define ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_LSB                           (56)
#define ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_MSB                           (69)
#define ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_NOF_BITS                      (ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_MSB - ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_LSB + 1)


#define ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_LSB                         (40)
#define ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_MSB                         (58)
#define ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_NOF_BITS                    (ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_MSB - ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_LSB + 1)


#define ARAD_TDM_FTMH_OPT_MC_MC_ID_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_LSB:ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_LSB)
#define ARAD_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_OPT_MC_MC_ID_NOF_BITS:ARAD_TDM_ARAD_FTMH_OPT_MC_MC_ID_NOF_BITS)


#define ARAD_TDM_PB_FTMH_HEADER_START                               (8)
#define ARAD_TDM_PB_FTMH_STA_TYPE_START_BIT                         (16 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_VERSION_LSB                            (62 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_VERSION_MSB                            (63 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_VERSION_NOF_BITS                       (ARAD_TDM_PB_FTMH_STA_VERSION_MSB - ARAD_TDM_PB_FTMH_STA_VERSION_LSB + 1)


#define ARAD_TDM_FTMH_STA_TYPE_UC                                   (0)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_LSB                  (0 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_MSB                  (15 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_MSB - ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT        (0)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_LSB                  (19 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_MSB                  (23 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_MSB - ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT        (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_LSB                  (32 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_MSB                  (42 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_MSB - ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT        (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT + ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)

#define ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2      (ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS + ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)

#define ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_LSB                     (24 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_MSB                     (31 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS                (ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_MSB - ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_LSB + 1)

#define ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_LSB                          (48 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_MSB                          (58 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_NOF_BITS                     (ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_MSB - ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_LSB + 1)


#define ARAD_TDM_ARAD_FTMH_STA_TYPE_START_BIT                       (23)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_LSB                (0)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_MSB                (22)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT      (0)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_LSB                (24)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_MSB                (30)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT      (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_LSB                (39)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_MSB                (40)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT      (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT + ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_LSB                (41)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_MSB                (57)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_GET_START_BIT      (0)

#define ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2    (ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS + ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)



#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_LSB                   (31)
#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_MSB                   (38)
#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS              (ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_LSB + 1)

#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_LSB                        (58)
#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_MSB                        (68)
#define ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_NOF_BITS                   (ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_MSB - ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_LSB + 1)


#define ARAD_TDM_FTMH_STA_TYPE_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_TYPE_START_BIT:ARAD_TDM_ARAD_FTMH_STA_TYPE_START_BIT)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_LSB:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_LSB)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_LSB:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_LSB)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_LSB:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_LSB)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2 \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2:ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2)
#define ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_LSB:ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_LSB)
#define ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS)
#define ARAD_TDM_FTMH_STA_UC_FAP_ID_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_LSB:ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_LSB)
#define ARAD_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_UC_FAP_ID_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_UC_FAP_ID_NOF_BITS)


#define ARAD_TDM_FTMH_STA_TYPE_MC                                   (1)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_LSB                  (14 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_MSB                  (15 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_MSB - ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT        (0)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_LSB                  (19 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_MSB                  (20 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_MSB - ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT        (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_LSB                  (21 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_MSB                  (23 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_MSB - ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT        (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT + ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_LSB                  (32 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_MSB                  (47 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS             (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_MSB - ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT        (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT + ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)

#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2      (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS + ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)

#define ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3      (ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2 + ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)

#define ARAD_TDM_PB_FTMH_STA_MC_MC_ID_LSB                           (0 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_MC_ID_MSB                           (13 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_MC_ID_NOF_BITS                      (ARAD_TDM_PB_FTMH_STA_MC_MC_ID_MSB - ARAD_TDM_PB_FTMH_STA_MC_MC_ID_LSB + 1)
#define ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_LSB                    (24 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_MSB                    (31 + ARAD_TDM_PB_FTMH_HEADER_START)
#define ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_NOF_BITS               (ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_MSB - ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_LSB + 1)


#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_LSB                (0)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_MSB                (3)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT      (0)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_LSB                (24)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_MSB                (30)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT      (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_LSB                (39)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_MSB                (56)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT      (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT + ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_LSB                (57)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_MSB                (59)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS           (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT      (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT + ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)

#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2    (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS + ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)

#define ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3    (ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2 + ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)

#define ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_LSB                           (4)
#define ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_MSB                           (22)
#define ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_NOF_BITS                      (ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_LSB + 1)
#define ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_LSB                    (31)
#define ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_MSB                    (38)
#define ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_NOF_BITS               (ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_MSB - ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_LSB + 1)


#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_LSB)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_LSB)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_LSB)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_LSB)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2 \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2)
#define ARAD_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3 \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3:ARAD_TDM_ARAD_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3)
#define ARAD_TDM_FTMH_STA_MC_MC_ID_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_MC_ID_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_LSB)
#define ARAD_TDM_FTMH_STA_MC_MC_ID_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_MC_ID_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_MC_ID_NOF_BITS)
#define ARAD_TDM_FTMH_STA_MC_INTERNAL_USE_LSB \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_LSB:ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_LSB)
#define ARAD_TDM_FTMH_STA_MC_INTERNAL_USE_NOF_BITS \
    ((is_petrab_in_system) ? ARAD_TDM_PB_FTMH_STA_MC_INTERNAL_USE_NOF_BITS:ARAD_TDM_ARAD_FTMH_STA_MC_INTERNAL_USE_NOF_BITS)


#define ARAD_TDM_CONTEXT_MAP_DEFAULT_VAL                         (0x0)

#define ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE (0x2)





#define ARAD_TDM_DIRECT_ROUNTING_RPT_EACHABLE_WITH_IRE_TDM_MASK_MODE 0x2

#define ARAD_TDM_DIRECT_ROUNTING_RPT_LINK_INTEGRITY_WITH_IRE_TDM_MASK_MODE 0x3

#define ARAD_TDM_DIRECT_ROUNTING_RPT_REACHABLE_MODE(enable_rpt_reachable) \
    (enable_rpt_reachable ? ARAD_TDM_DIRECT_ROUNTING_RPT_LINK_INTEGRITY_WITH_IRE_TDM_MASK_MODE : ARAD_TDM_DIRECT_ROUNTING_RPT_EACHABLE_WITH_IRE_TDM_MASK_MODE)
#define ARAD_TDM_DIRECT_ROUNTING_RPT_REACHABLE_IS_ENABLED(enable_rpt_mode) \
    ((enable_rpt_mode == ARAD_TDM_DIRECT_ROUNTING_RPT_LINK_INTEGRITY_WITH_IRE_TDM_MASK_MODE) ? TRUE: FALSE)

















int
arad_tdm_local_to_reassembly_context_get(int unit, uint32 port, uint32 *reassembly_context)
{
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;
    
    
    if (SOC_IS_QAX(unit)) {
        int core = 0;
        rv = soc_port_sw_db_local_to_tm_port_get(unit, port, reassembly_context, &core);
        SOCDNX_IF_ERR_EXIT(rv);
    } else if (SOC_IS_JERICHO(unit)) {
        *reassembly_context = port;
    } else {
        rv = sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, port, reassembly_context);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC uint8
arad_tdm_is_petrab_in_system_get(SOC_SAND_IN int unit)
{
  if (SOC_IS_JERICHO(unit)) {
    return FALSE;
  } else {
    return arad_sw_db_is_petrab_in_system_get(unit);
  }
}



uint32 
 arad_tdm_unit_has_tdm(
        SOC_SAND_IN int unit,
        SOC_SAND_OUT uint32 *tdm_found)
{
    soc_dpp_config_tdm_t *tdm = &(SOC_DPP_CONFIG(unit)->tdm);

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(tdm_found);
    *tdm_found=0;

    if(tdm->is_bypass || tdm->is_packet) {
        *tdm_found = 1;
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_tdm_unit_has_tdm()",0,0);

}



uint32
  arad_tdm_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    vsc_mode,
    res,
    tdm_egress_priority,
    tdm_egress_dp;

  ARAD_MGMT_TDM_MODE
    tdm_mode;
  ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
    ilkn_tdm_dedicated_queuing;
  uint8
    is_local;
  uint8
    
    is_petrab_in_system;
  soc_reg_above_64_val_t
    data;
  char *propval;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_INIT);

  SOC_REG_ABOVE_64_CLEAR(data);

  tdm_mode = arad_sw_db_tdm_mode_get(unit);
  is_petrab_in_system = arad_sw_db_is_petrab_in_system_get(unit);
  ilkn_tdm_dedicated_queuing = arad_sw_db_ilkn_tdm_dedicated_queuing_get(unit);  
  tdm_egress_priority = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_priority;
  tdm_egress_dp = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_dp;  

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 9, exit,dcmn_property_get_str(unit,spn_FAP_TDM_BYPASS,&propval));

  
  if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
      
      fld_val = 0x1;
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
  }
  
  
  
  
    
  if (!(tdm_mode == ARAD_MGMT_TDM_MODE_PACKET)) 
  {
    
    fld_val = 0x1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_10r, REG_PORT_ANY, 0, EGRESS_TDM_MODEf,  fld_val));
   
        
    
    fld_val = (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)?0x1:0x0;
    
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  33,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  35,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));        
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  37,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_FTMH_OPTIMIZEDf,  fld_val));
  }
  else
  {
    fld_val = 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_10r, REG_PORT_ANY, 0, EGRESS_TDM_MODEf,  fld_val));
    
    
  }

  
  
  fld_val = 0x1;
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  64,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_MODE_ENf,  fld_val));


  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  66,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, GLBL_VSC_128_MODEf, &vsc_mode));

  fld_val = (vsc_mode) ? ARAD_TDM_FRAGMENT_NUM_VSC_128:ARAD_TDM_FRAGMENT_NUM_VSC_256;
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  68,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_10r, REG_PORT_ANY, 0, TDM_FRG_NUMf,  fld_val));   
 

  
  fld_val = SOC_SAND_BOOL2NUM(is_petrab_in_system);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  62,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PETRAB_FTMHf,  fld_val));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_STRIP_FABRIC_CRC_ENf,  fld_val));

  fld_val = SOC_SAND_BOOL2NUM_INVERSE(is_petrab_in_system);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, VSC_256_UC_TDM_CRC_ENf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, VSC_256_MC_TDM_CRC_ENf,  fld_val));

  SOC_SAND_CHECK_FUNC_RESULT(arad_tdm_unit_has_tdm(unit,&fld_val), 82,  exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  83,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_CONTEXT_MODEf,  fld_val));
  
  
  fld_val = ARAD_TDM_CELL_SIZE_MIN;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  63, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MIN_SIZEf,  fld_val));
  
  fld_val = (vsc_mode ) ? ARAD_TDM_PB_CELL_SIZE_MAX:ARAD_TDM_CELL_SIZE_MAX;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MAX_SIZEf,  fld_val));    

  
  fld_val = 0;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  63, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, TDM_REP_FORMAT_ENf,  fld_val));

  
  
  { 
    

    
    fld_val = ARAD_TDM_VERSION_ID;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FTMH_VERSIONf, fld_val));
    
    
    fld_val = 0x1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  125,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, TDM_ENf,  fld_val));
    
    
    fld_val = 0x0;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, TDM_STRIP_FABRIC_CRC_ENf,  fld_val));

    
    fld_val = ARAD_TDM_PUSH_QUEUE_TYPE;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  135,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPEf,  fld_val));
    
    fld_val = 0x1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPE_ENf,  fld_val));
  }

  
  if (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA)
  {
    is_local =
      (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP)? TRUE:FALSE;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, FORCE_ALL_LOCALf,  is_local));
  }
  
  
  if (SOC_DPP_CONFIG(unit)->tdm.is_tdm_over_primary_pipe) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_FDR_ENABLERS_REGISTER_2r, REG_PORT_ANY, 0, TDM_HEADER_PRIORITYf,  0x2));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  171,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDR_FDR_ENABLERS_REGISTER_2r, REG_PORT_ANY, 0, TDM_HEADER_PRIORITYf,  0x3));
  }
  
  fld_val = 0x2;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, FDT_LINK_BITMAP_CONFIGURATIONr, REG_PORT_ANY, 0, IRE_TDM_MASK_MODEf,  fld_val));


  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  181,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_TCf,  tdm_egress_priority));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  181,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_DPf,  tdm_egress_dp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_tdm_init()",0,0);
}


uint32
  arad_tdm_ing_ftmh_fill_header(
    SOC_SAND_IN     int                   unit,
    SOC_SAND_IN     ARAD_TDM_FTMH                 *ftmh,
    SOC_SAND_IN     ARAD_TDM_FTMH_INFO_MODE       ftmh_mode,
    SOC_SAND_OUT    ARAD_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    tmp = 0,
    res = SOC_SAND_OK;
  uint8
    is_petrab_in_system;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_ING_FTMH_FILL_HEADER);

  SOC_SAND_CHECK_NULL_INPUT(ftmh);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  is_petrab_in_system = arad_tdm_is_petrab_in_system_get(unit);
  
  switch(ftmh_mode)
  {
    case ARAD_TDM_FTMH_INFO_MODE_OPT_UC:
      
      tmp = ARAD_TDM_FTMH_OPT_TYPE_UC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      tmp = ftmh->opt_uc.dest_if;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_OPT_UC_DEST_IF_LSB,
        ARAD_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      tmp = ftmh->opt_uc.dest_fap_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB,
        ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      break;
    case ARAD_TDM_FTMH_INFO_MODE_OPT_MC:
      
      tmp = ARAD_TDM_FTMH_OPT_TYPE_MC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      tmp = ftmh->opt_mc.mc_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_OPT_MC_MC_ID_LSB,
        ARAD_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
      break;
    case ARAD_TDM_FTMH_INFO_MODE_STANDARD_UC:
      
      tmp = ARAD_TDM_FTMH_STA_TYPE_UC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      if (is_petrab_in_system)
      {
        tmp = 0;
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_PB_FTMH_STA_VERSION_LSB,
          ARAD_TDM_PB_FTMH_STA_VERSION_NOF_BITS,
          &(tbl_data->header[0]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
      }

      tmp = ftmh->standard_uc.dest_fap_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_UC_FAP_ID_LSB,
        ARAD_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

      tmp = ftmh->standard_uc.dest_fap_port;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB,
        ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_GET_START_BIT,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
      
      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_GET_START_BIT,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

      tmp = 0;
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_uc.user_def,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_GET_START_BIT,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB,
        ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

      
      if (!is_petrab_in_system)
      {
        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &ftmh->standard_uc.user_def_2,
          ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_GET_START_BIT,
          ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_LSB,
          ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_NOF_BITS,
          &(tbl_data->header[0]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      }
      break;
    case ARAD_TDM_FTMH_INFO_MODE_STANDARD_MC:
      
      tmp = ARAD_TDM_FTMH_STA_TYPE_MC;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

      if (is_petrab_in_system)
      {
        tmp = 0;
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_PB_FTMH_STA_VERSION_LSB,
          ARAD_TDM_PB_FTMH_STA_VERSION_NOF_BITS,
          &(tbl_data->header[0]));
        SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      }

      tmp = ftmh->standard_mc.mc_id;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_MC_ID_LSB,
        ARAD_TDM_FTMH_STA_MC_MC_ID_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

      tmp = 0xff;
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_INTERNAL_USE_LSB,
        ARAD_TDM_FTMH_STA_MC_INTERNAL_USE_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);

      
      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_GET_START_BIT,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 78, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_GET_START_BIT,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_GET_START_BIT,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 86, exit);

      res = soc_sand_bitstream_get_any_field(
        &ftmh->standard_mc.user_def,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_GET_START_BIT,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
        &(tmp)
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);
      res = soc_sand_bitstream_set_any_field(
        &tmp,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB,
        ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
        &(tbl_data->header[0]));
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
      break;
    default:
      break;
  }  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ing_ftmh_fill_header()", 0, 0);
}


uint32
  arad_tdm_ing_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_ING_ACTION           action_ing,
    SOC_SAND_IN  ARAD_TDM_FTMH                 *ftmh,
    SOC_SAND_IN  uint8                   is_mc
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IRE_TDM_CONFIG_TBL_DATA
    arad_ire_tdm_config_tbl_data;
  ARAD_MGMT_TDM_MODE
    tdm_mode;
  ARAD_PORT_HEADER_TYPE
    incoming_header_type,
    outgoing_header_type;
  ARAD_TDM_FTMH_INFO_MODE
    ftmh_mode = ARAD_TDM_FTMH_INFO_MODE_OPT_UC;
  uint8
    is_petrab_in_system;
  uint32
    reassembly_context;
  int local_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_ING_FTMH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ftmh);

  if(SOC_IS_JERICHO(unit) && (action_ing == ARAD_TDM_ING_ACTION_CUSTOMER_EMBED)) {
    SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 9, exit);
  }

  res = soc_port_sw_db_tm_to_local_port_get(unit, core_id, port_ndx, &local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);
  res = arad_tdm_local_to_reassembly_context_get(unit,local_port, &reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_sand_os_memset(
    &arad_ire_tdm_config_tbl_data,
    0,
    sizeof(ARAD_IRE_TDM_CONFIG_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_ire_tdm_config_tbl_get_unsafe(
    unit,
    reassembly_context,
    &arad_ire_tdm_config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  
  res = arad_port_header_type_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &incoming_header_type,
          &outgoing_header_type
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 12, exit);
  
  if (incoming_header_type == ARAD_PORT_HEADER_TYPE_CPU)
  {
    arad_ire_tdm_config_tbl_data.cpu = 0x1;
  }

  tdm_mode = arad_sw_db_tdm_mode_get(unit);
  is_petrab_in_system = arad_tdm_is_petrab_in_system_get(unit);

  switch(action_ing)
  {
    case ARAD_TDM_ING_ACTION_ADD:
      if (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)
      {
        
        arad_ire_tdm_config_tbl_data.mode = (is_petrab_in_system) ? ARAD_TDM_FTMH_PB_OPT_MODE_VAL_FLD : ARAD_TDM_FTMH_ARAD_OPT_MODE_VAL_FLD;
        ftmh_mode = (is_mc == FALSE) ? ARAD_TDM_FTMH_INFO_MODE_OPT_UC:ARAD_TDM_FTMH_INFO_MODE_OPT_MC;
      }
      else
      {
        arad_ire_tdm_config_tbl_data.mode = (is_petrab_in_system) ? ARAD_TDM_FTMH_PB_STA_MODE_VAL_FLD : ARAD_TDM_FTMH_ARAD_STA_MODE_VAL_FLD;
        ftmh_mode = (is_mc == FALSE) ? ARAD_TDM_FTMH_INFO_MODE_STANDARD_UC:ARAD_TDM_FTMH_INFO_MODE_STANDARD_MC;
      }
      break;
    case ARAD_TDM_ING_ACTION_CUSTOMER_EMBED:
      
      arad_ire_tdm_config_tbl_data.mode = ARAD_TDM_FTMH_EXTERNAL_MODE_VAL_FLD;
      if (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)
      {
        ftmh_mode = (is_mc == FALSE) ? ARAD_TDM_FTMH_INFO_MODE_OPT_UC:ARAD_TDM_FTMH_INFO_MODE_OPT_MC;
      }
      else
      {
        ftmh_mode = (is_mc == FALSE) ? ARAD_TDM_FTMH_INFO_MODE_STANDARD_UC:ARAD_TDM_FTMH_INFO_MODE_STANDARD_MC;
      }
      break;
    case ARAD_TDM_ING_ACTION_NO_CHANGE:
      arad_ire_tdm_config_tbl_data.mode = ARAD_TDM_FTMH_UNCHANGED_MODE_VAL_FLD;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_TDM_ACTION_ING_OUT_OF_RANGE_ERR, 15, exit);
  }

  if (action_ing != ARAD_TDM_ING_ACTION_NO_CHANGE)
  {
    res = arad_tdm_ing_ftmh_fill_header(
      unit,
      ftmh,
      ftmh_mode,
      &arad_ire_tdm_config_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }  

  res = arad_ire_tdm_config_tbl_set_unsafe(
          unit,
          reassembly_context,
          &arad_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ing_ftmh_set_unsafe()", port_ndx, 0);
}


uint32
  arad_tdm_eg_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                port_ndx,
    SOC_SAND_IN  ARAD_TDM_EG_ACTION    action_eg
  )
{
  uint32
    res;
  ARAD_PORT_HEADER_TYPE
    header_type = 0;    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_EG_FTMH_SET_UNSAFE);

  if(SOC_IS_JERICHO(unit) && (action_eg == ARAD_TDM_EG_ACTION_CUSTOMER_EXTRACT)
     && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tdm_eg_editing_is_cud_stamping", 0) != 1)
     && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "tdm_eg_editing_is_cud_stamping_type1", 0) != 1)) {
      SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 9, exit);
  }

  switch(action_eg)
  {
    case ARAD_TDM_EG_ACTION_REMOVE:
      header_type = ARAD_PORT_HEADER_TYPE_TDM; 
      break;
    case ARAD_TDM_EG_ACTION_CUSTOMER_EXTRACT: 
      header_type = ARAD_PORT_HEADER_TYPE_TDM_PMM;
      break;
    case ARAD_TDM_EG_ACTION_NO_CHANGE:  
      header_type = ARAD_PORT_HEADER_TYPE_CPU;
      break;
    default:
      break;
  }
  
  res = arad_port_header_type_set_unsafe(
          unit,
          core_id,
          port_ndx,
          ARAD_PORT_DIRECTION_OUTGOING,
          header_type
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_eg_ftmh_set_unsafe()", port_ndx, 0);
}


uint32
  arad_tdm_ftmh_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  res = arad_tdm_ing_ftmh_set_unsafe(
          unit,
          core_id,
          port_ndx,
          info->action_ing,
          &info->ftmh,
          info->is_mc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_tdm_eg_ftmh_set_unsafe(
          unit,
          core_id,
          port_ndx,
          info->action_eg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_set_unsafe()", port_ndx, 0);
}

uint32
  arad_tdm_ftmh_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO            *info
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_MGMT_TDM_MODE
    tdm_mode;
  uint8
    is_petrab_in_system;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_SET_VERIFY);

  if(SOC_IS_JERICHO(unit) && (info->action_ing == ARAD_TDM_ING_ACTION_CUSTOMER_EMBED)) {
    SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_ERR, 9, exit);
  }

  tdm_mode = arad_sw_db_tdm_mode_get(unit);
  is_petrab_in_system = arad_tdm_is_petrab_in_system_get(unit);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  if (info->action_ing == ARAD_TDM_ING_ACTION_CUSTOMER_EMBED)    
  {
    if (!(is_petrab_in_system) ||  (tdm_mode != ARAD_MGMT_TDM_MODE_TDM_STA))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_TDM_CUSTOMER_EMBED_IN_OPTIMIZED_MODE_ERR, 7, exit);
    }    
  }
  ARAD_STRUCT_VERIFY(ARAD_TDM_FTMH_INFO, info, 20, exit);
  
  SOC_SAND_ERR_IF_BELOW_MIN(tdm_mode,ARAD_MGMT_TDM_MODE_TDM_OPT, ARAD_TDM_INVALID_TDM_MODE_ERR, 25, exit);

  
  SOC_SAND_CHECK_NULL_INPUT(&(info->ftmh));

  switch (tdm_mode)
  {
  case ARAD_MGMT_TDM_MODE_TDM_OPT:
    if (!info->is_mc)
    {
      ARAD_STRUCT_VERIFY(ARAD_TDM_FTMH_OPT_UC, &(info->ftmh.opt_uc), 10, exit);
    }
    else
    {
      ARAD_STRUCT_VERIFY(ARAD_TDM_FTMH_OPT_MC, &(info->ftmh.opt_mc), 11, exit);
    }
    break;
  case ARAD_MGMT_TDM_MODE_TDM_STA:
    if (!info->is_mc)
    {
      ARAD_STRUCT_VERIFY(ARAD_TDM_FTMH_STANDARD_UC, &(info->ftmh.standard_uc), 12, exit);
    }
    else
    {
      ARAD_STRUCT_VERIFY(ARAD_TDM_FTMH_STANDARD_MC, &(info->ftmh.standard_mc), 13, exit);
    }
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 20, exit);
    break;
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_set_verify()", port_ndx, 0);
}

uint32
  arad_tdm_ftmh_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  
  SOC_SAND_ERR_IF_BELOW_MIN(arad_sw_db_tdm_mode_get(unit),ARAD_MGMT_TDM_MODE_TDM_OPT, ARAD_TDM_INVALID_TDM_MODE_ERR, 25, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_get_verify()", port_ndx, 0);
}


uint32
  arad_tdm_ing_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_ING_ACTION           *action_ing,
    SOC_SAND_OUT ARAD_TDM_FTMH                 *ftmh,
    SOC_SAND_OUT uint8                   *is_mc
  )
{
  uint32
    tmp = 0,
    res = SOC_SAND_OK;
  ARAD_IRE_TDM_CONFIG_TBL_DATA
    arad_ire_tdm_config_tbl_data;
  ARAD_MGMT_TDM_MODE
    tdm_mode;
  uint8
    is_petrab_in_system;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_ING_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_ing);
  SOC_SAND_CHECK_NULL_INPUT(ftmh);
  SOC_SAND_CHECK_NULL_INPUT(is_mc);

  res = soc_sand_os_memset(
          &arad_ire_tdm_config_tbl_data,
          0,
          sizeof(ARAD_IRE_TDM_CONFIG_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = arad_ire_tdm_config_tbl_get_unsafe(
          unit,
          port_ndx,
          &arad_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (arad_ire_tdm_config_tbl_data.mode == ARAD_TDM_FTMH_EXTERNAL_MODE_VAL_FLD) {
      *action_ing = ARAD_TDM_ING_ACTION_CUSTOMER_EMBED;
  } else if (arad_ire_tdm_config_tbl_data.mode == ARAD_TDM_FTMH_UNCHANGED_MODE_VAL_FLD) {
      *action_ing = ARAD_TDM_ING_ACTION_NO_CHANGE;
  } else {
    *action_ing = ARAD_TDM_ING_ACTION_ADD;
  }
  
  if (*action_ing != ARAD_TDM_ING_ACTION_NO_CHANGE)
  {
    tdm_mode = arad_sw_db_tdm_mode_get(unit);
    is_petrab_in_system = arad_tdm_is_petrab_in_system_get(unit);

   
    if (arad_ire_tdm_config_tbl_data.mode == (is_petrab_in_system ? ARAD_TDM_FTMH_PB_OPT_MODE_VAL_FLD:ARAD_TDM_FTMH_ARAD_OPT_MODE_VAL_FLD))
    {
      
      if (tdm_mode != ARAD_MGMT_TDM_MODE_TDM_OPT)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 23, exit);
      }     
      res = soc_sand_bitstream_get_any_field(
        &(arad_ire_tdm_config_tbl_data.header[0]),
        ARAD_TDM_FTMH_OPT_TYPE_START_BIT,
        1,
        &(tmp)
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      if (tmp == ARAD_TDM_FTMH_OPT_TYPE_UC)
      {
        
        *is_mc = FALSE;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_LSB,
          ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_NOF_BITS,
          &(ftmh->opt_uc.dest_fap_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_OPT_UC_DEST_IF_LSB,
          ARAD_TDM_FTMH_OPT_UC_DEST_IF_NOF_BITS,
          &(ftmh->opt_uc.dest_if)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
      }
      else
      {
        
        *is_mc = TRUE;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_OPT_MC_MC_ID_LSB,
          ARAD_TDM_FTMH_OPT_MC_MC_ID_NOF_BITS,
          &(ftmh->opt_mc.mc_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
      }
      
    }
    else
    {
      
      if (tdm_mode != ARAD_MGMT_TDM_MODE_TDM_STA)
      {
        SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 33, exit);
      }

      res = soc_sand_bitstream_get_any_field(
        &arad_ire_tdm_config_tbl_data.header[0],
        ARAD_TDM_FTMH_STA_TYPE_START_BIT,
        1,
        &(tmp)
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

      if (tmp == ARAD_TDM_FTMH_STA_TYPE_UC)
      {
        
        *is_mc = FALSE;

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_UC_FAP_ID_LSB,
          ARAD_TDM_FTMH_STA_UC_FAP_ID_NOF_BITS,
          &tmp
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

        ftmh->standard_uc.dest_fap_id = tmp;

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_LSB,
          ARAD_TDM_FTMH_STA_UC_FAP_PORT_ID_NOF_BITS,
          &tmp
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);
        ftmh->standard_uc.dest_fap_port = tmp;

        
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_LSB,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
          &(ftmh->standard_uc.user_def)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);
        
        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_LSB,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_1_NOF_BITS,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_2_NOF_BITS,
          &(ftmh->standard_uc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_LSB,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_NOF_BITS_PART_1_2,
          ARAD_TDM_FTMH_STA_UC_USER_DEFINED_3_NOF_BITS,
          &(ftmh->standard_uc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

        
        if (!is_petrab_in_system)
        {
          tmp = 0;
          res = soc_sand_bitstream_get_any_field(
            &arad_ire_tdm_config_tbl_data.header[0],
            ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_LSB,
            ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_NOF_BITS,
            &(tmp)
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
          res = soc_sand_bitstream_set_any_field(
            &tmp,
            0,
            ARAD_TDM_ARAD_FTMH_STA_UC_USER_DEFINED_4_NOF_BITS,
            &(ftmh->standard_uc.user_def_2));
          SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
        }
      }
      else
      {
        
        *is_mc = TRUE;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_MC_MC_ID_LSB,
          ARAD_TDM_FTMH_STA_MC_MC_ID_NOF_BITS,
          &(ftmh->standard_mc.mc_id)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

        
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_LSB,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
          &(ftmh->standard_mc.user_def)
        );
        SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_LSB,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_1_NOF_BITS,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_2_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_LSB,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_2,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_3_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

        tmp = 0;
        res = soc_sand_bitstream_get_any_field(
          &arad_ire_tdm_config_tbl_data.header[0],
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_LSB,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
          &(tmp)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
        res = soc_sand_bitstream_set_any_field(
          &tmp,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_NOF_BITS_PART_1_3,
          ARAD_TDM_FTMH_STA_MC_USER_DEFINED_4_NOF_BITS,
          &(ftmh->standard_mc.user_def));
        SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
      }

    }       
    
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ing_ftmh_get_unsafe()", port_ndx, 0);
}


uint32
  arad_tdm_eg_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_EG_ACTION            *action_eg
  )
{
  uint32
    res;
  uint32
    prge_prog_select,
    base_q_pair;
  ARAD_EGQ_PCT_TBL_DATA
    egq_pct_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_EG_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(action_eg);
  

  res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit,core_id, port_ndx, &base_q_pair);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 130, exit);

  
  res = arad_egq_pct_tbl_get_unsafe(
                unit,
                core_id,
                base_q_pair,
                &(egq_pct_data)
              );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 140, exit);

  res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_SYSTEM_HEADER, &prge_prog_select);
  if ((res == SOC_E_NONE) && (egq_pct_data.prog_editor_profile == prge_prog_select)) {
      *action_eg = ARAD_TDM_EG_ACTION_REMOVE;
  } else {
      res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_REMOVE_TDM_OPT_FTMH, &prge_prog_select);
      if ((res == SOC_E_NONE) && (egq_pct_data.prog_editor_profile == prge_prog_select)) {
          *action_eg = ARAD_TDM_EG_ACTION_REMOVE;
      } else {
          res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TDM_PMM_HEADER, &prge_prog_select);
          if ((res == SOC_E_NONE) && (egq_pct_data.prog_editor_profile == prge_prog_select)) {
              *action_eg = ARAD_TDM_EG_ACTION_CUSTOMER_EXTRACT;
          } else {
              res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_NONE, &prge_prog_select);
              if ((res == SOC_E_NONE) && (egq_pct_data.prog_editor_profile == prge_prog_select)) {
                  *action_eg = ARAD_TDM_EG_ACTION_NO_CHANGE;
              } else {
                      *action_eg = ARAD_TDM_NOF_EG_ACTIONS;
              }
          }
      }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ing_ftmh_get_unsafe()", port_ndx, 0);
}

uint32
  arad_tdm_ftmh_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT ARAD_TDM_FTMH_INFO            *info
  )
{
  int local_port;
  uint32 reassembly_context;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_FTMH_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_TDM_FTMH_INFO_clear(info);

  res = soc_port_sw_db_tm_to_local_port_get(unit, core_id, port_ndx, &local_port);
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  res = arad_tdm_local_to_reassembly_context_get(unit, (uint32)local_port, &reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);

  res = arad_tdm_ing_ftmh_get_unsafe(
          unit,
          reassembly_context,
          &info->action_ing,
          &info->ftmh,
          &info->is_mc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_tdm_eg_ftmh_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &info->action_eg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ftmh_get_unsafe()", port_ndx, 0);
}

uint32
  arad_tdm_opt_size_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
  uint32
    res,
    fld_val;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_SET_UNSAFE);

  
  fld_val = cell_size;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_PKT_SIZEf,  fld_val));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_PKT_SIZEf,  fld_val));  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_set_unsafe()", cell_size, 0);
}

uint32
  arad_tdm_opt_size_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    cell_size
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_SET_VERIFY);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(cell_size, ARAD_TDM_CELL_SIZE_MIN, ARAD_TDM_CELL_SIZE_MAX, ARAD_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  
  
  if (arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_set_verify()", cell_size, 0);
}

uint32
  arad_tdm_opt_size_get_verify(
    SOC_SAND_IN  int                   unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_GET_VERIFY);
  
  
  if (arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_get_verify()", 0, 0);
}


uint32
  arad_tdm_opt_size_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT uint32                    *cell_size
  )
{
  uint32
    res,
    fld_val;     

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OPT_SIZE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cell_size);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_PKT_SIZEf, &fld_val));
  
  *cell_size = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_opt_size_get_unsafe()", 0, 0);
}


uint32
  arad_tdm_stand_size_range_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res,
    fld_val;
  int reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);  

  reg = (SOC_IS_QAX(unit)) ? IRE_TDM_CONFIGURATIONSr : IRE_TDM_SIZEr; 

  fld_val = size_range->start;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, TDM_MIN_SIZEf,  fld_val));

  fld_val = size_range->end;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, reg, REG_PORT_ANY, 0, TDM_MAX_SIZEf,  fld_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_set_unsafe()", 0, 0);
}

uint32
  arad_tdm_stand_size_range_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_SAND_U32_RANGE              *size_range
  )
{
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(size_range->end, ARAD_TDM_CELL_SIZE_MAX, ARAD_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(size_range->start, ARAD_TDM_CELL_SIZE_MIN, ARAD_TDM_CELL_SIZE_OUT_OF_RANGE_ERR, 15, exit);

  
  if (arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_STA && arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_set_verify()", 0, 0);
}

uint32
  arad_tdm_stand_size_range_get_verify(
    SOC_SAND_IN  int                   unit
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_GET_VERIFY);
  
  
  if (arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_STA && arad_sw_db_tdm_mode_get(unit) != ARAD_MGMT_TDM_MODE_TDM_OPT)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TDM_INVALID_TDM_MODE_ERR, 20, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_get_verify()", 0, 0);
}


uint32
  arad_tdm_stand_size_range_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_SAND_U32_RANGE              *size_range
  )
{
  uint32
    res,
    fld_val;
  int reg;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_STAND_SIZE_RANGE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(size_range);
     
  reg = (SOC_IS_QAX(unit)) ? IRE_TDM_CONFIGURATIONSr : IRE_TDM_SIZEr; 

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, TDM_MIN_SIZEf, &fld_val));
  size_range->start = fld_val;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, reg, REG_PORT_ANY, 0, TDM_MAX_SIZEf, &fld_val));
  size_range->end = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_stand_size_range_get_unsafe()", 0, 0);
}


int
  arad_tdm_ofp_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  )
{
    uint32
        fld_val,
        is_tdm_val;
    uint8
        is_exists_tdm_port = FALSE;
    ARAD_MGMT_TDM_MODE
        tdm_mode;   
    soc_reg_above_64_val_t
        data,
        field_val;
    uint32
        i,
        base_q_pair,
        nof_q_pairs,
        curr_q_pair;
    int
        core;
    soc_error_t
        rv;
    SOCDNX_INIT_FUNC_DEFS;
  
    SOC_REG_ABOVE_64_CLEAR(data);
    SOC_REG_ABOVE_64_CLEAR(field_val);
    if(soc_feature(unit, soc_feature_no_tdm)) {
        SOC_EXIT;
    }    

    tdm_mode = arad_sw_db_tdm_mode_get(unit);
    is_tdm_val = SOC_SAND_BOOL2NUM(is_tdm);

    
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
  
    SOCDNX_IF_ERR_EXIT(READ_EGQ_TDM_MAP_QUEUE_TO_TDMr(unit, core, data));

    soc_reg_above_64_field_get(unit, EGQ_TDM_MAP_QUEUE_TO_TDMr, data, TDM_MAP_QUEUE_TO_TDMf,field_val);
   
    
    for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair+nof_q_pairs; curr_q_pair++)
    {
        SHR_BITCOPY_RANGE(field_val,curr_q_pair,&is_tdm_val,0,1);
    }
    
    
    soc_reg_above_64_field_set(unit, EGQ_TDM_MAP_QUEUE_TO_TDMr, data, TDM_MAP_QUEUE_TO_TDMf,field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_TDM_MAP_QUEUE_TO_TDMr(unit, core, data)); 
  
    
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        if ((tdm_mode == ARAD_MGMT_TDM_MODE_PACKET))
        {
            is_exists_tdm_port = is_tdm;
            if (is_exists_tdm_port == FALSE)
            {
                
                SOCDNX_IF_ERR_EXIT(READ_EGQ_TDM_MAP_QUEUE_TO_TDMr(unit, core, data));

                soc_reg_above_64_field_get(unit, EGQ_TDM_MAP_QUEUE_TO_TDMr, data, TDM_MAP_QUEUE_TO_TDMf,field_val);

                for (i = 0; i < ARAD_EGQ_TDM_MAP_QUEUE_TO_TDM_REG_MULT_NOF_REGS; i++)
                {        
                    fld_val = 0;
                    SHR_BITCOPY_RANGE(&fld_val,0,field_val,i*SOC_SAND_REG_SIZE_BITS,SOC_SAND_REG_SIZE_BITS);
                    if (fld_val != 0)
                    {
                        is_exists_tdm_port = TRUE;
                    }
                }
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_10r, core, 0, EGRESS_TDM_MODEf, is_exists_tdm_port));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_tdm_ofp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OFP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_tdm, ARAD_TDM_IS_TDM_MAX, ARAD_TDM_IS_TDM_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ofp_set_verify()", port_ndx, 0);
}

uint32
  arad_tdm_ofp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OFP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ofp_get_verify()", port_ndx, 0);
}


uint32
  arad_tdm_ofp_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT uint8                   *is_tdm
  )
{
  uint32
    is_tdm_val = 0;
  uint32
    base_q_pair;
  soc_reg_above_64_val_t
    data,
    field_val;
  uint32 res, tm_port;
  int core;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_OFP_GET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(data);
  SOC_REG_ABOVE_64_CLEAR(field_val);

  SOC_SAND_CHECK_NULL_INPUT(is_tdm);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1220, exit, soc_port_sw_db_local_to_tm_port_get(unit, port_ndx, &tm_port, &core));
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1221, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair));
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1230, exit, READ_EGQ_TDM_MAP_QUEUE_TO_TDMr(unit, REG_PORT_ANY, data));

  soc_reg_above_64_field_get(unit, EGQ_TDM_MAP_QUEUE_TO_TDMr, data, TDM_MAP_QUEUE_TO_TDMf,field_val);
   
  SHR_BITCOPY_RANGE(&is_tdm_val,0,field_val,base_q_pair,1);
  
  *is_tdm = SOC_SAND_NUM2BOOL(is_tdm_val);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ofp_get_unsafe()", port_ndx, 0);
}


int
  arad_tdm_ifp_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  soc_port_t            port,
    SOC_SAND_IN  uint8                 is_tdm
  )
{
    uint32
        is_tdm_val = 0,
        find_context_map_val,
        is_vsc_mode_128,
        context_map_val,
        temp_tdm_context;
    soc_reg_above_64_val_t
        data,
        field_val; 
    uint32
        reassembly_context;
    ARAD_PORT2IF_MAPPING_INFO
        info_incoming;
    ARAD_INTERFACE_ID
        used_if_id, temp_if_id;
    int tdm_context_ref_count;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    SOC_REG_ABOVE_64_CLEAR(field_val);


    
    SOCDNX_IF_ERR_EXIT(
        sw_state_access[unit].dpp.soc.arad.tm.lag.local_to_reassembly_context.get(unit, port, &reassembly_context));
    
  
    is_tdm_val = SOC_SAND_BOOL2NUM(is_tdm);

    

    
    SOCDNX_IF_ERR_EXIT(READ_IRE_SET_FTMH_VERSIONr(unit,data));

    soc_reg_above_64_field_get(unit, IRE_SET_FTMH_VERSIONr, data, SET_FTMH_VERSIONf,field_val);
   
    
    SHR_BITCOPY_RANGE(field_val,reassembly_context,&is_tdm_val,0,1);
   
    
    soc_reg_above_64_field_set(unit, IRE_SET_FTMH_VERSIONr, data, SET_FTMH_VERSIONf,field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_SET_FTMH_VERSIONr(unit,data));

    
    SOCDNX_IF_ERR_EXIT(READ_IRE_TDM_MODE_MAPr(unit,data));

    soc_reg_above_64_field_get(unit, IRE_TDM_MODE_MAPr, data, TDM_MODE_MAPf,field_val);

    
    SHR_BITCOPY_RANGE(field_val,reassembly_context,&is_tdm_val,0,1);

    
    soc_reg_above_64_field_set(unit, IRE_TDM_MODE_MAPr, data, TDM_MODE_MAPf,field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_TDM_MODE_MAPr(unit,data));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, ECI_GLOBALEr, REG_PORT_ANY, 0, GLBL_VSC_128_MODEf, &is_vsc_mode_128));

    if (!is_vsc_mode_128) 
    {  
        
        SOC_SAND_IF_ERR_EXIT(arad_port_to_interface_map_get(
            unit,
            0,
            port,
            &info_incoming.if_id,
            &info_incoming.channel_id
          ));  

        SOCDNX_IF_ERR_EXIT(READ_IRE_TDM_CONTEXT_MAPr(unit,data));

        soc_reg_above_64_field_get(unit, IRE_TDM_CONTEXT_MAPr, data, TDM_CONTEXT_MAPf,field_val);

        
        context_map_val = -1;
        for (find_context_map_val = 0; find_context_map_val < ARAD_NOF_TDM_CONTEXT_MAP; find_context_map_val++) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.get(unit,find_context_map_val,&used_if_id));

            
            if (is_tdm == FALSE) {
                if (used_if_id == info_incoming.if_id)
                {
                    
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.tdm_context_ref_count.get(unit, find_context_map_val, &tdm_context_ref_count));
                    tdm_context_ref_count--;
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.tdm_context_ref_count.set(unit, find_context_map_val, tdm_context_ref_count));

                    if (tdm_context_ref_count == 0) {
                        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.set(unit,find_context_map_val,ARAD_IF_ID_NONE));
                    }
                    break;
                }
            } else if (used_if_id == ARAD_IF_ID_NONE || used_if_id == info_incoming.if_id) {
                if (used_if_id == ARAD_IF_ID_NONE) {
                    
                    for (temp_tdm_context = find_context_map_val + 1; temp_tdm_context < ARAD_NOF_TDM_CONTEXT_MAP; temp_tdm_context++) {
                        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.get(unit, temp_tdm_context, &temp_if_id));
                        if (temp_if_id == info_incoming.if_id)
                        {
                            used_if_id = temp_if_id;
                            find_context_map_val = temp_tdm_context;
                            break;
                        }
                    }
                }
                
                context_map_val = find_context_map_val;

                
                if (used_if_id == ARAD_IF_ID_NONE) {
                    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.context_map.set(unit,find_context_map_val,info_incoming.if_id));
                }
                    
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.tdm_context_ref_count.get(unit, find_context_map_val, &tdm_context_ref_count));
                tdm_context_ref_count++;
                SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.tdm.tdm_context_ref_count.set(unit, find_context_map_val, tdm_context_ref_count));
                break;
            }
        }

        
        if (is_tdm == TRUE && context_map_val == -1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("ARAD_TDM_OUT_OF_AVAIABLE_TDM_CONTEXT_ERR")));
        }

        if (is_tdm == FALSE) 
        {
            context_map_val = ARAD_TDM_CONTEXT_MAP_DEFAULT_VAL;
        }
        

        
        SHR_BITCOPY_RANGE(field_val,reassembly_context,&context_map_val,0,1);

        
        soc_reg_above_64_field_set(unit, IRE_TDM_CONTEXT_MAPr, data, TDM_CONTEXT_MAPf,field_val);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_TDM_CONTEXT_MAPr(unit,data));
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_tdm_ifp_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  uint8                   is_tdm
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_IFP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(is_tdm, ARAD_TDM_IS_TDM_MAX, ARAD_TDM_IS_TDM_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ifp_set_verify()", port_ndx, 0);
}

uint32
  arad_tdm_ifp_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_IFP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_ifp_get_verify()", port_ndx, 0);
}


int
  arad_tdm_ifp_get(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port,
    SOC_SAND_OUT uint8           *is_tdm
  )
{
    uint32 is_tdm_val = 0;
    soc_reg_above_64_val_t data, field_val;    
    uint32 reassembly_context;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    SOC_REG_ABOVE_64_CLEAR(field_val);

    
    SOCDNX_IF_ERR_EXIT(arad_tdm_local_to_reassembly_context_get(unit, port, &reassembly_context));

    SOCDNX_IF_ERR_EXIT(READ_IRE_TDM_MODE_MAPr(unit, data));

    soc_reg_above_64_field_get(unit, IRE_TDM_MODE_MAPr, data, TDM_MODE_MAPf,field_val);
   
    is_tdm_val = SHR_BITGET(field_val, reassembly_context);
    *is_tdm = SOC_SAND_NUM2BOOL(is_tdm_val);    
  
exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  arad_tdm_direct_routing_profile_map_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile    
  )
{
  uint32
    res;
  uint32
    reassembly_context;
  ARAD_IRE_TDM_CONFIG_TBL_DATA
    arad_ire_tdm_config_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET_UNSAFE);

  res = arad_tdm_local_to_reassembly_context_get(unit,port_ndx, &reassembly_context);
SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_ire_tdm_config_tbl_get_unsafe(
          unit,
          reassembly_context,
          &arad_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  arad_ire_tdm_config_tbl_data.link_mask_ptr = direct_routing_profile;

  res = arad_ire_tdm_config_tbl_set_unsafe(
          unit,
          reassembly_context,
          &arad_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_set_unsafe()", port_ndx, 0);
}

uint32
  arad_tdm_direct_routing_profile_map_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint32                     direct_routing_profile  
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(direct_routing_profile, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_MAX, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_profile_map_set_verify()", port_ndx, 0);
}

uint32
  arad_tdm_direct_routing_profile_map_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_profile_map_get_verify()", port_ndx, 0);
}


uint32
  arad_tdm_direct_routing_profile_map_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   uint32                     port_ndx,
    SOC_SAND_OUT  uint32                    *direct_routing_profile    
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reassembly_context;
  ARAD_IRE_TDM_CONFIG_TBL_DATA
    arad_ire_tdm_config_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_profile);

  res = arad_tdm_local_to_reassembly_context_get(unit, port_ndx, &reassembly_context);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_ire_tdm_config_tbl_get_unsafe(
          unit,
          reassembly_context,
          &arad_ire_tdm_config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *direct_routing_profile = arad_ire_tdm_config_tbl_data.link_mask_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_profile_map_get_unsafe()", port_ndx, 0);
}


uint32
  arad_tdm_direct_routing_set_unsafe(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     uint32                     direct_routing_profile,
    SOC_SAND_IN     ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_IN     uint8 enable_rpt_reachable
  )
{
  uint32    
    link_bitmap[2] = {0},
    entry_offset,
    data[ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE],
    fld_val[ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE],
    fld_val32,
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_SET_UNSAFE);

  sal_memset(data,0x0,sizeof(uint32)*ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE);
  sal_memset(fld_val,0x0,sizeof(uint32)*ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE);

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_info);

  
  entry_offset = direct_routing_profile;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1580, exit, READ_FDT_IRE_TDM_MASKSm(unit, MEM_BLOCK_ANY, entry_offset, data));
  
  
  link_bitmap[0] = direct_routing_info->link_bitmap.arr[0];
  link_bitmap[1] = direct_routing_info->link_bitmap.arr[1];
  
  res = soc_sand_bitstream_set_any_field(
          link_bitmap,
          0,
          SOC_DPP_DEFS_GET(unit, nof_fabric_links),
          fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  
  soc_FDT_IRE_TDM_MASKSm_field_set(unit, data, LINKS_MASKf , fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1590, exit, WRITE_FDT_IRE_TDM_MASKSm(unit, MEM_BLOCK_ANY, entry_offset, data));
  
  fld_val32 = ARAD_TDM_DIRECT_ROUNTING_RPT_REACHABLE_MODE(enable_rpt_reachable);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, FDT_LINK_BITMAP_CONFIGURATIONr, REG_PORT_ANY, 0, IRE_TDM_MASK_MODEf,  fld_val32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_set_unsafe()", direct_routing_profile, 0);
}

uint32
  arad_tdm_direct_routing_set_verify(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     uint32                     direct_routing_profile,
    SOC_SAND_IN     ARAD_TDM_DIRECT_ROUTING_INFO  *direct_routing_info    
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(direct_routing_profile, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_MAX, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, ARAD_TDM_DIRECT_ROUTING_INFO_verify(unit, direct_routing_info));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_set_verify()", direct_routing_profile, 0);
}

uint32
  arad_tdm_direct_routing_get_verify(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     uint32                     direct_routing_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(direct_routing_profile, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_MAX, ARAD_TDM_DIRECT_ROUTING_PROFILE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_get_verify()", direct_routing_profile, 0);
}


uint32
  arad_tdm_direct_routing_get_unsafe(
    SOC_SAND_IN     int                     unit,
    SOC_SAND_IN     uint32                     direct_routing_profile,
    SOC_SAND_OUT    ARAD_TDM_DIRECT_ROUTING_INFO *direct_routing_info,
    SOC_SAND_OUT    uint8 *enable_rpt_reachable
  )
{
  uint32
    start_bit,
    link_bitmap[2] = {0},
    entry_offset,
    data[ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE],
    fld_val[ARAD_TDM_FDT_IRE_TDM_MASKS_SIZE],
    fld_val32,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_DIRECT_ROUTING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(direct_routing_info);
  ARAD_TDM_DIRECT_ROUTING_INFO_clear(direct_routing_info);

  SOC_SAND_CHECK_NULL_INPUT(enable_rpt_reachable);

    
  entry_offset = direct_routing_profile;
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1690, exit, READ_FDT_IRE_TDM_MASKSm(unit, MEM_BLOCK_ANY, entry_offset, data));
  soc_FDT_IRE_TDM_MASKSm_field_get(unit, data, LINKS_MASKf , fld_val);


  
  start_bit = 0;
  res = soc_sand_bitstream_get_any_field(
          fld_val,
          start_bit,
          SOC_DPP_DEFS_GET(unit, nof_fabric_links),
          link_bitmap
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  direct_routing_info->link_bitmap.arr[0] = link_bitmap[0];
  direct_routing_info->link_bitmap.arr[1] = link_bitmap[1];

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_read(unit, FDT_LINK_BITMAP_CONFIGURATIONr, REG_PORT_ANY, 0, IRE_TDM_MASK_MODEf,  &fld_val32));  
  *enable_rpt_reachable = ARAD_TDM_DIRECT_ROUNTING_RPT_REACHABLE_IS_ENABLED(fld_val32);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_direct_routing_get_unsafe()", direct_routing_profile, 0);
}


uint32
  arad_tdm_port_packet_crc_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_port_t                 port_ndx,
    SOC_SAND_IN  uint8                      is_enable,         
    SOC_SAND_IN  uint8                      configure_ingress, 
    SOC_SAND_IN  uint8                      configure_egress   
  )
{
  uint32
    res = SOC_SAND_OK,
    is_crc_enable_val = SOC_SAND_BOOL2NUM(is_enable);
    int core=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_SET_UNSAFE);

  if (configure_ingress) {
      
      uint32 reassembly_context;
      ARAD_IRE_TDM_CONFIG_TBL_DATA arad_ire_tdm_config_tbl_data;

      res = arad_tdm_local_to_reassembly_context_get(unit, port_ndx, &reassembly_context);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

      res = arad_ire_tdm_config_tbl_get_unsafe(unit, reassembly_context, &arad_ire_tdm_config_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      arad_ire_tdm_config_tbl_data.add_packet_crc = is_crc_enable_val;

      res = arad_ire_tdm_config_tbl_set_unsafe(unit, reassembly_context, &arad_ire_tdm_config_tbl_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

    if(!SOC_IS_JERICHO(unit) && configure_egress) {
      

      uint32 base_q_pair;
      soc_reg_above_64_val_t data, field_val; 

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1780, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, port_ndx,  &base_q_pair));
      
      SOC_REG_ABOVE_64_CLEAR(data);
      SOC_REG_ABOVE_64_CLEAR(field_val);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1790, exit, READ_EPNI_TDM_EN_CRC_PER_PORTr(unit, data));
      soc_reg_above_64_field_get(unit, EPNI_TDM_EN_CRC_PER_PORTr, data, TDM_EN_CRC_PER_PORTf, field_val);
   
      
      SHR_BITCOPY_RANGE(field_val, base_q_pair, &is_crc_enable_val, 0, 1);
   
      
      soc_reg_above_64_field_set(unit, EPNI_TDM_EN_CRC_PER_PORTr, data, TDM_EN_CRC_PER_PORTf,field_val);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1800, exit, WRITE_EPNI_TDM_EN_CRC_PER_PORTr(unit,data));
    }
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_set_unsafe()", port_ndx, 0);
}

uint32
  arad_tdm_port_packet_crc_set_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx,
    SOC_SAND_IN  uint8                      is_enable  
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_set_verify()", port_ndx, 0);
}

uint32
  arad_tdm_port_packet_crc_get_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     port_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_TDM_PORT_NDX_MAX, ARAD_TDM_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_get_verify()", port_ndx, 0);
}


uint32
  arad_tdm_port_packet_crc_get_unsafe(
    SOC_SAND_IN   int                     unit,
    SOC_SAND_IN   soc_port_t                 port_ndx,
    SOC_SAND_OUT  uint8                      *is_ingress_enabled,
    SOC_SAND_OUT  uint8                      *is_egress_enabled
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reassembly_context;
  ARAD_IRE_TDM_CONFIG_TBL_DATA
    arad_ire_tdm_config_tbl_data;
  int core=0;
  uint32
    reg32_val=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_TDM_PORT_PACKET_CRC_GET_UNSAFE);

  if (is_ingress_enabled) { 
      res = arad_tdm_local_to_reassembly_context_get(unit, port_ndx, &reassembly_context);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
      if (!SOC_IS_QAX(unit)) {
          res = arad_ire_tdm_config_tbl_get_unsafe(unit, reassembly_context, &arad_ire_tdm_config_tbl_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      } else {
          
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,
                                               READ_IRE_GLOBAL_GENERAL_CFG_3r(unit, &reg32_val));
          arad_ire_tdm_config_tbl_data.add_packet_crc  = soc_reg_field_get(
                                                            unit,
                                                            IRE_GLOBAL_GENERAL_CFG_3r,
                                                            reg32_val,
                                                            PACKET_CRC_ENf);
      }
      *is_ingress_enabled = SOC_SAND_NUM2BOOL(arad_ire_tdm_config_tbl_data.add_packet_crc);
  }

    if(!SOC_IS_JERICHO(unit) && is_egress_enabled) {
      soc_reg_above_64_val_t data, field_val;

      uint32 base_q_pair;
      int results;
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, port_ndx,  &base_q_pair));

      SOC_REG_ABOVE_64_CLEAR(data);
      SOC_REG_ABOVE_64_CLEAR(field_val);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_EPNI_TDM_EN_CRC_PER_PORTr(unit, data));
      soc_reg_above_64_field_get(unit, EPNI_TDM_EN_CRC_PER_PORTr, data, TDM_EN_CRC_PER_PORTf, field_val);      
      SHR_BITTEST_RANGE(field_val, base_q_pair, 1, results);
      *is_egress_enabled = results ? TRUE : FALSE;
    } else if (SOC_IS_JERICHO(unit)) {
        *is_egress_enabled = TRUE;
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_tdm_port_packet_crc_get_unsafe()", port_ndx, 0);
}

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_TDM_FTMH_OPT_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_if, ARAD_TDM_FTMH_OPT_UC_DEST_IF_MAX, ARAD_TDM_DEST_IF_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_fap_id, ARAD_TDM_FTMH_OPT_UC_DEST_FAP_ID_MAX, ARAD_TDM_DEST_FAP_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_FTMH_OPT_UC_verify()",0,0);
}

uint32
  ARAD_TDM_FTMH_OPT_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_OPT_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mc_id, ARAD_TDM_FTMH_OPT_MC_MC_ID_MAX, ARAD_TDM_MC_ID_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_FTMH_OPT_MC_verify()",0,0);
}

uint32
  ARAD_TDM_FTMH_STANDARD_UC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_UC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_fap_id, ARAD_TDM_FTMH_STANDARD_UC_DEST_FAP_ID_MAX, ARAD_TDM_DEST_FAP_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->dest_fap_port, ARAD_TDM_FTMH_STANDARD_UC_DEST_FAP_PORT_MAX, ARAD_TDM_DEST_FAP_PORT_OUT_OF_RANGE_ERR, 13, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->user_def_2, ARAD_TDM_FTMH_STANDARD_UC_USER_DEFINE_2_MAX, ARAD_TDM_USER_DEFINE_2_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_FTMH_STANDARD_UC_verify()",0,0);
}

uint32
  ARAD_TDM_FTMH_STANDARD_MC_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_STANDARD_MC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mc_id, ARAD_TDM_FTMH_STANDARD_MC_MC_ID_MAX, ARAD_TDM_MC_ID_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->user_def, ARAD_TDM_FTMH_STANDARD_MC_USER_DEF_MAX, ARAD_TDM_MC_USER_DEF_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_FTMH_STANDARD_MC_verify()",0,0);
}

uint32
  ARAD_TDM_FTMH_INFO_verify(
    SOC_SAND_IN  ARAD_TDM_FTMH_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_ing, ARAD_TDM_FTMH_INFO_ACTION_ING_MAX, ARAD_TDM_ACTION_ING_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->action_eg, ARAD_TDM_FTMH_INFO_ACTION_EG_MAX, ARAD_TDM_ACTION_EG_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_FTMH_INFO_verify()",0,0);
}
uint32
  ARAD_TDM_DIRECT_ROUTING_INFO_verify(
    SOC_SAND_IN  uint32 unit,
    SOC_SAND_IN  ARAD_TDM_DIRECT_ROUTING_INFO *info
  )
{
  int port;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SHR_BIT_ITER(info->link_bitmap.arr , SOC_DPP_DEFS_GET(unit, nof_fabric_links)  , port){
      if (!IS_SFI_PORT(unit, port + FABRIC_LOGICAL_PORT_BASE(unit))){
          SOC_SAND_SET_ERROR_CODE(ARAD_TDM_LINK_BITMAP_OUT_OF_RANGE_ERR, 10, exit);
      }
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_TDM_DIRECT_ROUTING_INFO_verify()",0,0);
}
#endif 


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

