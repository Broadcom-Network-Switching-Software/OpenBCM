/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UTILS_LINE_PTG_INCLUDED__
/* { */
#define __UTILS_LINE_PTG_INCLUDED__

/*************
 * INCLUDES  *
 */
/* { */

#if !DUNE_BCM

#include "DuneDriver/SAND/Utils/include/sand_framework.h"

#endif

/* } */
/*************
 * DEFINES   *
 */
/* { */

#define GFA_PETRA_BACK_END_IS_PETRA_GFA                   (TRUE)

#if GFA_PETRA_BACK_END_IS_PETRA_GFA
  #define UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR           (0x50300000)
  #define UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_BASE_ADDR (0x50400000)
  #define UTILS_PTG_FRONT_END_DEVICE_0_BASE_ADDR          (0x50000000)
  #define UTILS_PTG_FRONT_END_DEVICE_1_BASE_ADDR          (0x50100000)
#else
  #define UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR           (0x40700000)
  #define UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_BASE_ADDR (0x50400000)
  #define UTILS_PTG_FRONT_END_DEVICE_0_BASE_ADDR          (0x50000000)
  #define UTILS_PTG_FRONT_END_DEVICE_1_BASE_ADDR          (0x50100000)
#endif

#define UTILS_PTG_FRONT_END_IO_FPGA_SIZE                  (0x00100000)
#define UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_SIZE        (0x00100000)
#define UTILS_PTG_FRONT_END_DEVICE_0_SIZE                 (0x00100000)
#define UTILS_PTG_FRONT_END_DEVICE_1_SIZE                 (0x00100000)

#define UTILS_PTG_DEVICE_FRONT_END_PTG_0_RESET_BIT        (4)
#define UTILS_PTG_DEVICE_FRONT_END_PTG_1_RESET_BIT        (5)
#define UTILS_PTG_DEVICE_FRONT_END_IO_AGENT_RESET_BIT     (6)
#define UTILS_PTG_DEVICE_FRONT_END_FLASH_RESET_BIT        (7)

#define UTILS_PTG_DEVICE_FRONT_END_PTG_0_RESET            (SOC_SAND_BIT(UTILS_PTG_DEVICE_FRONT_END_PTG_0_RESET_BIT))
#define UTILS_PTG_DEVICE_FRONT_END_PTG_1_RESET            (SOC_SAND_BIT(UTILS_PTG_DEVICE_FRONT_END_PTG_1_RESET_BIT))
#define UTILS_PTG_DEVICE_FRONT_END_IO_AGENT_RESET         (SOC_SAND_BIT(UTILS_PTG_DEVICE_FRONT_END_IO_AGENT_RESET_BIT))
#define UTILS_PTG_DEVICE_FRONT_END_FLASH_RESET            (SOC_SAND_BIT(UTILS_PTG_DEVICE_FRONT_END_FLASH_RESET_BIT))

/* FILE_SYSTEM_FLASH { */
#define UTILS_PTG_FILE_SYSTEM_FLASH_ADDR                  (UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_BASE_ADDR)
#define UTILS_PTG_FILE_SYSTEM_FLASH_MAX_ADDR              (UTILS_PTG_FILE_SYSTEM_FLASH_ADDR + UTILS_PTG_FRONT_END_FILE_SYSTEM_FLASH_SIZE - 1)
#define UTILS_PTG_FILE_SYSTEM_FLASH_SIZE                  (UTILS_PTG_FILE_SYSTEM_FLASH_MAX_ADDR - UTILS_PTG_FILE_SYSTEM_FLASH_ADDR + 1)
#define UTILS_PTG_FILE_SYSTEM_FLASH_SIZE_TO_USE           (UTILS_PTG_FILE_SYSTEM_FLASH_SIZE - 12)
#define UTILS_PTG_FILE_SYSTEM_FLASH_TYPE                  (FLASH_28F640)
#define UTILS_PTG_FILE_SYSTEM_FLASH_SECTOR_SIZE           (FLASH_28F320_SECTOR) /* two flashes*/
#define UTILS_PTG_FILE_SYSTEM_FLASH_ADRS_JUMP             (1) /* 4 bytes alignment */
#define UTILS_PTG_FILE_SYSTEM_FLASH_WIDTH                 (4) /* two flashes-> 4 bytes alignment */
/* } FILE_SYSTEM_FLASH */

/* IO_AGENT_FPGA { */
#define UTILS_PTG_IO_AGENT_FPGA_FILE                       "FPG0005A.rbf"
#define UTILS_PTG_IO_AGENT_FPGA_GPIO                      (SOC_SAND_SET_FLD_IN_PLACE(0x0, 7, SOC_SAND_BIT(7))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 6, SOC_SAND_BIT(6))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 1, SOC_SAND_BIT(1))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 0, SOC_SAND_BIT(0)))
/* } IO_AGENT_FPGA */

/* DEVICE_FPGA { */
#define UTILS_PTG_DEVICE_FPGA_FILE                         "FPG0006A.rbf"
#define UTILS_PTG_DEVICE_FPGA_GPIO                        (SOC_SAND_SET_FLD_IN_PLACE(0x0, 7, SOC_SAND_BIT(7))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 6, SOC_SAND_BIT(6))|SOC_SAND_SET_FLD_IN_PLACE(0x1, 1, SOC_SAND_BIT(1))|SOC_SAND_SET_FLD_IN_PLACE(0x0, 0, SOC_SAND_BIT(0)))
/* } DEVICE_FPGA */

/* } */

/*************
 * MACROS    *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */
typedef struct
{
  uint8 address;
  uint8 msb;
  uint8 lsb;
}UTILS_PTG_FLD_ADDR;

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

void
  utils_ptg_fpga_io_fld_set(
    SOC_SAND_IN  UTILS_PTG_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  );

uint8
  utils_ptg_fpga_io_fld_get(
    SOC_SAND_IN  UTILS_PTG_FLD_ADDR         *fld
  );

uint16
  utils_ptg_mdio_get(
    SOC_SAND_IN  uint8                    phy_ndx,
    SOC_SAND_IN  uint32                   address
  );

void
  utils_ptg_mdio_set(
    SOC_SAND_IN  uint8                    phy_ndx,
    SOC_SAND_IN  uint32                   address,
    SOC_SAND_IN  uint16                   data
  );

uint8
  utils_ptg_is_line_card_connected(
  );

uint8
  utils_ptg_is_line_card_phy_connected(
  );

uint8
  utils_ptg_is_line_card_ptg_connected(
  );

SOC_SAND_RET
  utils_ptg_ptg_init(
    SOC_SAND_IN  uint8                  start_server
  );

SOC_SAND_RET
  utils_ptg_phy_init(
  );

SOC_SAND_RET
  utils_ptg_init_host_board(
    SOC_SAND_IN  char board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN]
  );

/* } */

/* } __FAP21V_UTILS_LINE_PTG_INCLUDED__*/
#endif
