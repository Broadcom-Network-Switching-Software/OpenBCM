/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_line_PTG.h>
#include <appl/diag/dpp/utils_error_defs.h>
#include <appl/diag/dpp/utils_dune_fpga_download.h>
#include <appl/diag/dpp/utils_front_end_host_card.h>

#include <appl/diag/dpp/dffs_driver.h>

#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_cli_files.h>

#if !DUNE_BCM

/*************
 * INCLUDES  *
 */
/* { */


#include "Utilities/include/utils_defi.h"
#include "Utilities/include/utils_dffs_cpu_mez.h"
#include "Utilities/include/utils_error_defs.h"
#include "Utilities/include/utils_dune_fpga_download.h"
#include "Utilities/include/utils_serdes_defs.h"
#include "Utilities/include/utils_line_bringup.h"
#include "Utilities/include/utils_host_board.h"
#include "Utilities/include/utils_ip_mgmt.h"
#include "Utilities/include/utils_front_end_host_card.h"
#include "Utilities/include/utils_line_PTG.h"

#include "UserInterface/include/ui_pure_defi.h"

#include "Pub/include/utils_defx.h"

#include "UserInterface/include/ui_cli_files.h"

#include "DuneDriver/SAND/SAND_FM/include/sand_chip_defines.h"
#include "DuneDriver/SAND/Utils/include/sand_os_interface.h"

#include "Bsp_drv/include/bsp_cards_consts.h"

#include "Background/include/bckg_defi.h"

#include "../../../../h/usrApp.h"

#endif


/* } */

/*************
 * DEFINES   *
 */
/* { */

#define UTILS_PTG_NOF_PHY                         (8)

#define UTILS_PTG_DEBUG_PHY                       (FALSE)
#define UTILS_PTG_DEBUG_PTG                       (FALSE)

#define UTILS_PTG_FPGA_IO_BASE_OFFSET             ((sizeof(UTILS_PTG_FPGA_IO_ACCESS) - sizeof(uint8)) * SOC_SAND_NOF_BITS_IN_BYTE)

#define UTILS_PTG_AMCC_SIZE                       (24 * 1024)
#define UTILS_PTG_AMCC_ENTRY_SIZE                 (2)

#define UTILS_PTG_FPGA_IO_INLINING                (FALSE)

/* } */

/*************
 *  MACROS   *
 */
/* { */

#define UTILS_PTG_FPGA_IO_FLD_MASK(msb, lsb)          ((2 * SOC_SAND_BIT(msb)) - (SOC_SAND_BIT(lsb)))
#define UTILS_PTG_FPGA_IO_FLD_SHFT(msb, lsb)          (lsb)

#define UTILS_PTG_FPGA_IO_FLD_VAL_GET(val, msb, lsb)  (((val) & (UTILS_PTG_FPGA_IO_FLD_MASK(msb, lsb))) >> (UTILS_PTG_FPGA_IO_FLD_SHFT(msb, lsb)))
#define UTILS_PTG_FPGA_IO_FLD_VAL_SET(val, msb, lsb)  (((val) << (UTILS_PTG_FPGA_IO_FLD_SHFT(msb, lsb))) & (UTILS_PTG_FPGA_IO_FLD_MASK(msb, lsb)))

#define UTILS_PTG_AELUROS_MICRO_CONTROLLER_START(phy_ndx)                                           \
          do                                                                                        \
          {                                                                                         \
            utils_ptg_mdio_set(phy_ndx, 0x1c04a, 0x5200);                                           \
            soc_sand_os_task_delay_milisec(56);

#define UTILS_PTG_AELUROS_MICRO_CONTROLLER_END(phy_ndx)                                             \
            utils_ptg_mdio_set(phy_ndx, 0x1ca00,0x0080);                                            \
            utils_ptg_mdio_set(phy_ndx, 0x1ca12,0x0000);                                            \
          } while(0)

#define UTILS_PTG_INDIRECT_START                                                                    \
          do                                                                                        \
          {                                                                                         \
            UTILS_PTG_FPGA_IO_REG_CLEAR((Utils_ptg_fpga_io_regs.mdio_cfg_dest.addr));               \

#define UTILS_PTG_INDIRECT_END                                                                      \
          } while(0)

#define UTILS_PTG_INDIRECT_SESSION_START                                                            \
          do                                                                                        \
          {                                                                                         \
            UTILS_PTG_FPGA_IO_REG_CLEAR((Utils_ptg_fpga_io_regs.mdio_addr[0].addr));                \
            UTILS_PTG_FPGA_IO_REG_CLEAR((Utils_ptg_fpga_io_regs.mdio_addr[1].addr));                \
            UTILS_PTG_FPGA_IO_REG_CLEAR((Utils_ptg_fpga_io_regs.mdio_cfg_trigger.addr));

#define UTILS_PTG_INDIRECT_SESSION_END(op_id)                                                       \
            UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_trigger.op_code), op_id);    \
            UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_trigger.start), 0x1);        \
            do                                                                                      \
            {                                                                                       \
              busy = UTILS_PTG_FPGA_IO_FLD_GET((Utils_ptg_fpga_io_regs.mdio_cfg_trigger.busy));     \
            } while(busy == 1);                                                                     \
          } while(0)

#define UTILS_PTG_EXIT_AND_PRINT_ERROR(error_message)                                               \
          soc_sand_os_printf(error_message);                                                            \
          return SOC_SAND_ERR;

#if UTILS_PTG_FPGA_IO_INLINING

#define UTILS_PTG_FPGA_IO_FLD_SET(fld, data)                                                        \
          do                                                                                        \
          {                                                                                         \
            UTILS_PTG_FPGA_IO_ACCESS                                                                \
              value = 0;                                                                            \
            UTILS_PTG_FPGA_IO_ACCESS                                                                \
              data2set = data;                                                                      \
            value = *((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | (&fld)->address));  \
            value = value | UTILS_PTG_FPGA_IO_FLD_VAL_SET(data2set, (&fld)->msb + UTILS_PTG_FPGA_IO_BASE_OFFSET, (&fld)->lsb + UTILS_PTG_FPGA_IO_BASE_OFFSET);\
            *((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | (&fld)->address)) = value;  \
          } while(0)

#define UTILS_PTG_FPGA_IO_FLD_GET(fld)                                                              \
          (uint8)(UTILS_PTG_FPGA_IO_FLD_VAL_GET(*((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | (&fld)->address)), (&fld)->msb + UTILS_PTG_FPGA_IO_BASE_OFFSET, (&fld)->lsb + UTILS_PTG_FPGA_IO_BASE_OFFSET))

#define UTILS_PTG_FPGA_IO_REG_CLEAR(fld)                                                            \
          do                                                                                        \
          {                                                                                         \
            *((volatile uint32 *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | (&fld)->address)) = 0;  \
          } while(0)

#else

#define UTILS_PTG_FPGA_IO_FLD_SET(fld, data)                                                        \
          utils_ptg_fpga_io_fld_set((&fld), data)

#define UTILS_PTG_FPGA_IO_FLD_GET(fld)                                                              \
          utils_ptg_fpga_io_fld_get((&fld))

#define UTILS_PTG_FPGA_IO_REG_CLEAR(fld)                                                            \
          utils_ptg_fpga_io_reg_clear((&fld))
#endif
/* } */

/*************
 * TYPE DEFS *
 */
/* { */

#if GFA_PETRA_BACK_END_IS_PETRA_GFA
  typedef uint8 UTILS_PTG_FPGA_IO_ACCESS;
#else
  typedef uint32 UTILS_PTG_FPGA_IO_ACCESS;
#endif

typedef struct
{
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } version;                         /* 0x00 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } scratch_test;                    /* 0x04 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } bridging_cards_sense;            /* 0x08 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } sfp_modules_sense;               /* 0x0C */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } optical_interface_los_alarms;    /* 0x10 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } optical_interface_lasi_alarms;   /* 0x14 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
    UTILS_PTG_FLD_ADDR phy[8];
  } optical_phys_reset;              /* 0x18 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } general_purpose_io;              /* 0x1c */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
    UTILS_PTG_FLD_ADDR reference_clock_select;
  } miscellaneous_controls;          /* 0x20 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_mux_select;                  /* 0x28 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_mux_configuration;           /* 0x2C */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_address_command;             /* 0x30 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_data_tx_rx;                  /* 0x34 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_status;                      /* 0x38 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } i2c_triggers;                    /* 0x3C */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } debug_led;                       /* 0x40 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
    UTILS_PTG_FLD_ADDR op_code;
    UTILS_PTG_FLD_ADDR start;
    UTILS_PTG_FLD_ADDR read_valid;
    UTILS_PTG_FLD_ADDR busy;
  } mdio_cfg_trigger;                /* 0x44 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
    UTILS_PTG_FLD_ADDR device_addr;
    UTILS_PTG_FLD_ADDR port_addr;
  } mdio_cfg_dest;                   /* 0x48 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } mdio_addr[2];                    /* 0x4C - 0x50 */
  struct
  {
    UTILS_PTG_FLD_ADDR addr;
  } mdio_data[2];                    /* 0x54 - 0x58 */
} __ATTRIBUTE_PACKED__ UTILS_PTG_FPGA_IO_REGS;

/* } */

/*************
 * GLOBALS   *
 */
/* { */

static DFFS_LOW_FLASH_INFO_TYPE
  Utils_ptg_flash_info;

static DFFS_DESCRIPTOR_TYPE
  Utils_ptg_flash_descriptor;

static uint8
  Utils_ptg_flash_is_initialized = FALSE;

static UTILS_PTG_FPGA_IO_REGS
  Utils_ptg_fpga_io_regs;

static uint8
  Utils_ptg_fpga_io_regs_init = FALSE;

uint8
    Utils_amcc_skip = FALSE;

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

SOC_SAND_RET
  utils_ptg_fpga_io_regs_init()
{
  if (!Utils_ptg_fpga_io_regs_init)
  {
    Utils_ptg_fpga_io_regs.version.addr.address = 0x00;
    Utils_ptg_fpga_io_regs.version.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.version.addr.msb = 7;

    Utils_ptg_fpga_io_regs.scratch_test.addr.address = 0x04;
    Utils_ptg_fpga_io_regs.scratch_test.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.scratch_test.addr.msb = 7;

    Utils_ptg_fpga_io_regs.bridging_cards_sense.addr.address = 0x08;
    Utils_ptg_fpga_io_regs.bridging_cards_sense.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.bridging_cards_sense.addr.msb = 7;

    Utils_ptg_fpga_io_regs.sfp_modules_sense.addr.address = 0x0c;
    Utils_ptg_fpga_io_regs.sfp_modules_sense.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.sfp_modules_sense.addr.msb = 7;

    Utils_ptg_fpga_io_regs.optical_interface_los_alarms.addr.address = 0x10;
    Utils_ptg_fpga_io_regs.optical_interface_los_alarms.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.optical_interface_los_alarms.addr.msb = 7;

    Utils_ptg_fpga_io_regs.optical_interface_lasi_alarms.addr.address = 0x14;
    Utils_ptg_fpga_io_regs.optical_interface_lasi_alarms.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.optical_interface_lasi_alarms.addr.msb = 7;

    Utils_ptg_fpga_io_regs.optical_phys_reset.addr.address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.optical_phys_reset.addr.msb = 7;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[0].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[0].lsb = 0;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[0].msb = 0;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[1].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[1].lsb = 1;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[1].msb = 1;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[2].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[2].lsb = 2;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[2].msb = 2;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[3].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[3].lsb = 3;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[3].msb = 3;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[4].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[4].lsb = 4;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[4].msb = 4;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[5].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[5].lsb = 5;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[5].msb = 5;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[6].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[6].lsb = 6;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[6].msb = 6;

    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[7].address = 0x18;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[7].lsb = 7;
    Utils_ptg_fpga_io_regs.optical_phys_reset.phy[7].msb = 7;

    Utils_ptg_fpga_io_regs.miscellaneous_controls.addr.address = 0x1c;
    Utils_ptg_fpga_io_regs.miscellaneous_controls.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.miscellaneous_controls.addr.msb = 7;

    Utils_ptg_fpga_io_regs.miscellaneous_controls.reference_clock_select.address = 0x1c;
    Utils_ptg_fpga_io_regs.miscellaneous_controls.reference_clock_select.lsb = 2;
    Utils_ptg_fpga_io_regs.miscellaneous_controls.reference_clock_select.msb = 2;

    Utils_ptg_fpga_io_regs.general_purpose_io.addr.address = 0x20;
    Utils_ptg_fpga_io_regs.general_purpose_io.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.general_purpose_io.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_mux_select.addr.address = 0x28;
    Utils_ptg_fpga_io_regs.i2c_mux_select.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_mux_select.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_mux_configuration.addr.address = 0x2c;
    Utils_ptg_fpga_io_regs.i2c_mux_configuration.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_mux_configuration.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_address_command.addr.address = 0x30;
    Utils_ptg_fpga_io_regs.i2c_address_command.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_address_command.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_data_tx_rx.addr.address = 0x34;
    Utils_ptg_fpga_io_regs.i2c_data_tx_rx.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_data_tx_rx.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_status.addr.address = 0x38;
    Utils_ptg_fpga_io_regs.i2c_status.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_status.addr.msb = 7;

    Utils_ptg_fpga_io_regs.i2c_triggers.addr.address = 0x3c;
    Utils_ptg_fpga_io_regs.i2c_triggers.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.i2c_triggers.addr.msb = 7;

    Utils_ptg_fpga_io_regs.debug_led.addr.address = 0x40;
    Utils_ptg_fpga_io_regs.debug_led.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.debug_led.addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.addr.address = 0x44;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.op_code.address = 0x44;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.op_code.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.op_code.msb = 1;

    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.start.address = 0x44;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.start.lsb = 2;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.start.msb = 2;

    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.read_valid.address = 0x44;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.read_valid.lsb = 4;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.read_valid.msb = 4;

    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.busy.address = 0x44;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.busy.lsb = 5;
    Utils_ptg_fpga_io_regs.mdio_cfg_trigger.busy.msb = 5;

    Utils_ptg_fpga_io_regs.mdio_cfg_dest.addr.address = 0x48;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_cfg_dest.device_addr.address = 0x48;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.device_addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.device_addr.msb = 2;

    Utils_ptg_fpga_io_regs.mdio_cfg_dest.port_addr.address = 0x48;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.port_addr.lsb = 4;
    Utils_ptg_fpga_io_regs.mdio_cfg_dest.port_addr.msb = 6;

    Utils_ptg_fpga_io_regs.mdio_addr[0].addr.address = 0x4c;
    Utils_ptg_fpga_io_regs.mdio_addr[0].addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_addr[0].addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_addr[1].addr.address = 0x50;
    Utils_ptg_fpga_io_regs.mdio_addr[1].addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_addr[1].addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_data[0].addr.address = 0x54;
    Utils_ptg_fpga_io_regs.mdio_data[0].addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_data[0].addr.msb = 7;

    Utils_ptg_fpga_io_regs.mdio_data[1].addr.address = 0x58;
    Utils_ptg_fpga_io_regs.mdio_data[1].addr.lsb = 0;
    Utils_ptg_fpga_io_regs.mdio_data[1].addr.msb = 7;

    Utils_ptg_fpga_io_regs_init = TRUE;
  }

  return SOC_SAND_OK;
}

void
  utils_ptg_fpga_io_reg_clear(
    SOC_SAND_IN  UTILS_PTG_FLD_ADDR         *fld
  )
{
#ifndef WIN32
  *((volatile uint32 *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | fld->address)) = 0;
#endif
}

void
  utils_ptg_fpga_io_fld_set(
    SOC_SAND_IN  UTILS_PTG_FLD_ADDR         *fld,
    SOC_SAND_IN  uint8                    data
  )
{
  UTILS_PTG_FPGA_IO_ACCESS
    value = 0;
  UTILS_PTG_FPGA_IO_ACCESS
    data2set = data;
#ifndef WIN32
  value = *((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | fld->address));
#endif
  value = value | UTILS_PTG_FPGA_IO_FLD_VAL_SET(data2set, fld->msb + UTILS_PTG_FPGA_IO_BASE_OFFSET, fld->lsb + UTILS_PTG_FPGA_IO_BASE_OFFSET);
#ifndef WIN32
  *((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | fld->address)) = value;
#endif
}

uint8
  utils_ptg_fpga_io_fld_get(
    SOC_SAND_IN  UTILS_PTG_FLD_ADDR         *fld
  )
{
  UTILS_PTG_FPGA_IO_ACCESS
    value = 0;
#ifndef WIN32
  value = *((volatile UTILS_PTG_FPGA_IO_ACCESS *)(UTILS_PTG_FRONT_END_IO_FPGA_BASE_ADDR | fld->address));
#endif
  value = UTILS_PTG_FPGA_IO_FLD_VAL_GET(value, fld->msb + UTILS_PTG_FPGA_IO_BASE_OFFSET, fld->lsb + UTILS_PTG_FPGA_IO_BASE_OFFSET);
  return ((uint8)value);
}

SOC_SAND_RET
  utils_ptg_utils_dffs_get_flash_info(
    SOC_SAND_OUT DFFS_LOW_FLASH_INFO_TYPE   *flash_info
  )
{

  uint32
    dev_code = 0,
    fs_version = 0;
  uint32
    base_addr = 0,
    top_addr = 0,
    sector_size = 0,
    app_size = 0,
    fs_size = 0,
    fs_addr = 0,
    data_addr = 0;

  UTILS_INIT_ERR_DEFS("utils_ptg_utils_dffs_get_flash_info");

  if (flash_info == NULL)
  {
    UTILS_SET_ERR_AND_EXIT(5);
  }

  if (boot_get_fs_version==NULL)
  {
    UTILS_SET_ERR_AND_EXIT(20);
  }

  dev_code = UTILS_PTG_FILE_SYSTEM_FLASH_TYPE;
  base_addr = UTILS_PTG_FILE_SYSTEM_FLASH_ADDR;
  sector_size = UTILS_PTG_FILE_SYSTEM_FLASH_SECTOR_SIZE;

  fs_version = boot_get_fs_version();
  if (fs_version == BT_FS_VERSION_0)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG;
  }
  else if (fs_version == BT_FS_VERSION_1)
  {
    app_size = B_DEFAULT_APP_FLASH_SIZE_8MEG_VER_C;
  }
  else
  {
    UTILS_SET_ERR_AND_EXIT(40);
  }

  app_size = 0;
  fs_addr = base_addr + app_size;

  /* make sure fs_addr is a multiple of sector_size */
  fs_addr = SOC_SAND_DIV_ROUND_UP(fs_addr, sector_size);
  fs_addr *= sector_size;

  /*
   *fs descriptor takes one sector - the next sector if the base
   *sector of the fs data
   */
  data_addr = fs_addr + sector_size;

  top_addr = UTILS_PTG_FILE_SYSTEM_FLASH_MAX_ADDR;
  /* make sure top_addr is a multiple of sector_size */
  top_addr = SOC_SAND_DIV_ROUND_DOWN(top_addr, sector_size);
  top_addr *= sector_size;
  top_addr -= sector_size;


  fs_size = top_addr - data_addr + sector_size;

  if (fs_size <= sector_size)
  {
    UTILS_SET_ERR_AND_EXIT(50);
  }

  flash_info->descriptor_sector = (uint8)((fs_addr - base_addr)   / sector_size);
  flash_info->first_data_sector = (uint8)((data_addr - base_addr) / sector_size);
  flash_info->last_data_sector  = (uint8)((top_addr - base_addr)  / sector_size);
  flash_info->dev_code          = dev_code;
  flash_info->sector_size       = sector_size;
  flash_info->use_bsp_driver    = 1;

  flash_info->bsp_flash_def_inf.f_type          = dev_code;
  flash_info->bsp_flash_def_inf.f_adrs          = base_addr;
  flash_info->bsp_flash_def_inf.f_adrs_jump     = UTILS_PTG_FILE_SYSTEM_FLASH_ADRS_JUMP;
  flash_info->bsp_flash_def_inf.f_sector_size   = sector_size;
  flash_info->bsp_flash_def_inf.struct_version  = FLASH_DEF_INF_V1;
  flash_info->bsp_flash_def_inf.flash_width     = UTILS_PTG_FILE_SYSTEM_FLASH_WIDTH;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  utils_ptg_utils_dffs_init(
  )
{
  uint32
    err = SOC_SAND_OK;
  uint8
    signature_valid = FALSE,
    crc_valid = FALSE;
  uint8
    value = 0;
  uint32
    offset;

  UTILS_INIT_ERR_DEFS("utils_ptg_utils_dffs_init");

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)|UTILS_PTG_DEVICE_FRONT_END_FLASH_RESET;
  write_epld_reg((uint8)value,(uint32)offset);

  err = utils_ptg_utils_dffs_get_flash_info(
          &(Utils_ptg_flash_info)
        );
  UTILS_EXIT_IF_ERR(err, 10);

  err = dffs_low_is_flash_invalid(
          (&Utils_ptg_flash_info)
        );
  UTILS_EXIT_AND_PRINT_IF_ERR(err, TRUE, 20, "Invalid flash version");

  err = dffs_low_read_descriptor(
          (&Utils_ptg_flash_info),
          (char*)(&(Utils_ptg_flash_descriptor)),
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 30);

  signature_valid = dffs_signature_valid(&(Utils_ptg_flash_descriptor));

  if (signature_valid)
  {
    crc_valid = dffs_drv_desc_crc_is_valid(&(Utils_ptg_flash_descriptor),TRUE);
  }

  if (!(signature_valid) || !(crc_valid))
  {
    if (!signature_valid)
    {
      UTILS_PRINT_MSG(TRUE,"DFFS not found on flash");
    }
    else if (!crc_valid)
    {
      UTILS_GEN_SVR_ERR(
        "DFFS CRC invalid - all DFFS data will be lost!!!",
        "utils_ptg_utils_dffs_init",
         DFFS_DESCRIPTOR_CRC_ERR,
         0
      );
    }

    UTILS_PRINT_MSG(TRUE,"Creating empty DFFS...");

    err = dffs_drv_set_defaults(
            &(Utils_ptg_flash_info),
            &(Utils_ptg_flash_descriptor),
            TRUE
          );
    UTILS_EXIT_IF_ERR(err, 50);

    UTILS_PRINT_MSG(TRUE,"DFFS default drive set...");

    err = dffs_low_write_descriptor(
            &(Utils_ptg_flash_info),
            (char*)(&(Utils_ptg_flash_descriptor)),
            TRUE
          );
    UTILS_EXIT_IF_ERR(err, 60);

    UTILS_PRINT_MSG(TRUE, "DFFS create - success");
  }
  else
  {
    UTILS_PRINT_MSG(TRUE,"DFFS found on flash");
  }
  Utils_ptg_flash_is_initialized = TRUE;

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  utils_ptg_burn_fpga_low(
    SOC_SAND_IN  uint8                    *mem_base,
    SOC_SAND_IN  char                       *fpga_name,
    SOC_SAND_IN  uint32                   nof_bytes
  )
{
  char
    image_name[80];


#ifndef SAND_LOW_LEVEL_SIMULATION
  if (soc_sand_os_mutex_take(I2c_bus_semaphore, WAIT_FOREVER))
  {
    return SOC_SAND_ERR;
  }

#endif

  if (utils_dune_AlteraDownload(mem_base, image_name, nof_bytes, UTILS_DUNE_FPGA_DOWNLOAD_V_2, TRUE))
  {
    return SOC_SAND_ERR;
  }

#ifndef SAND_LOW_LEVEL_SIMULATION
  soc_sand_os_mutex_give(I2c_bus_semaphore);
#endif

  return SOC_SAND_OK;
}


SOC_SAND_RET
  utils_ptg_burn_fpga(
    SOC_SAND_IN uint8                     *mem_base,
    SOC_SAND_IN  char                       *fpga_name,
    SOC_SAND_IN uint32                    gpio_value,
    SOC_SAND_IN uint32                    nof_bytes
  )
{
  uint32
    nof_bytes_to_burn = nof_bytes;
  uint32
    offset;

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_io_conf));
  write_epld_reg((uint8)0xc3, (uint32)offset);

  offset = (uint32)(&(((EPLD_REGS *)0)->gp_bus_data));
  write_epld_reg((uint8)gpio_value, (uint32)offset);

  /* Burn FPGA */
  if (utils_ptg_burn_fpga_low(mem_base, fpga_name, nof_bytes_to_burn))
  {
    return SOC_SAND_ERR;
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_fpga_download(
    SOC_SAND_IN  char                       *file_name,
    SOC_SAND_IN  uint8                    gpio_value
  )
{
  int32
    file_handle;
  uint32
    delete_cli_file = FALSE;
  char
    err_msg[8*80] = "";
  uint32
    host_ip,
    file_size,
    max_file_size;
  uint8
    *file_mem_base;

  host_ip = 0; /* !DUNE_BCM utils_ip_get_dld_host_ip(); */

  max_file_size = UTILS_PTG_FILE_SYSTEM_FLASH_SIZE_TO_USE;

  if( download_cli_file(file_name, err_msg, DEFAULT_DOWNLOAD_IP) )
  {
    return SOC_SAND_ERR;
  }

  delete_cli_file = TRUE;

  if (get_cli_file_name_handle(file_name, err_msg, &file_handle) )
  {
    return SOC_SAND_ERR;
  }

  if (get_cli_file_size(file_handle, (void *)&file_size))
  {
    return SOC_SAND_ERR;
  }

  if (get_cli_file_mem_base(file_handle, (void *)&file_mem_base))
  {
    return SOC_SAND_ERR;
  }

  if(utils_ptg_burn_fpga(file_mem_base, file_name, gpio_value, file_size))
  {
    return SOC_SAND_ERR;
  }

  if (delete_cli_file)
  {
    erase_cli_file(file_name,err_msg);
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_utils_dffs_get_file_size(
             char                       *file_name,
    SOC_SAND_OUT uint32                   *file_size
  )
{
  uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_ptg_utils_dffs_get_file_size");

  err = dffs_drv_get_file_size(
          &(Utils_ptg_flash_info),
          &(Utils_ptg_flash_descriptor),
          file_name,
          file_size,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  utils_ptg_utils_dffs_file_exists(
             char                       *file_name,
    SOC_SAND_OUT uint32                  *does_file_exist
  )
{
 uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_ptg_utils_dffs_get_file_size");

  err = dffs_drv_file_exists(
          &(Utils_ptg_flash_descriptor),
          file_name,
          does_file_exist,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  utils_ptg_utils_dffs_file_from_flash(
             char*                      file_name,
    SOC_SAND_IN  uint32                   file_size,
    SOC_SAND_OUT char*                      file_buff
  )
{
  uint32
    err = SOC_SAND_OK;

  UTILS_INIT_ERR_DEFS("utils_ptg_utils_dffs_file_from_flash");

  err = dffs_drv_file_from_flash(
          &(Utils_ptg_flash_info),
          &(Utils_ptg_flash_descriptor),
          file_name,
          file_size,
          file_buff,
          TRUE
        );
  UTILS_EXIT_IF_ERR(err, 20);

exit:
  UTILS_EXIT_AND_PRINT_ERR;
}

SOC_SAND_RET
  utils_ptg_fpga_flash_burn(
             char                       *file_name,
    SOC_SAND_IN  uint8                    gpio_value
  )
{
  uint32
    file_exists;
  char
    *flash_data = NULL;
  uint32
    file_size = 0;
  uint32
    max_file_size = UTILS_PTG_FILE_SYSTEM_FLASH_SIZE;

  if (utils_ptg_utils_dffs_file_exists(file_name, &file_exists))
  {
    return SOC_SAND_ERR;
  }

  if (!file_exists)
  {
    return SOC_SAND_ERR;
  }

  if(utils_ptg_utils_dffs_get_file_size(file_name, &file_size))
  {
    return SOC_SAND_ERR;
  }

  if (file_size>max_file_size)
  {
    return SOC_SAND_ERR;
  }

  if (NULL == (flash_data = (char*)soc_sand_os_malloc_any_size(file_size)))
  {
    return SOC_SAND_ERR;
  }

  if(utils_ptg_utils_dffs_file_from_flash(file_name, file_size, flash_data))
  {
    return SOC_SAND_ERR;
  }

  if(utils_ptg_burn_fpga((uint8 *)flash_data, file_name, gpio_value, file_size))
  {
    return SOC_SAND_ERR;
  }

  if (flash_data)
  {
    soc_sand_os_free_any_size(flash_data);
    flash_data = NULL;
  }
  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_board_core_frequency_get(
    SOC_SAND_OUT uint32                   *dram_frequency
  )
{
  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_fpga_io_agent_init(
  )
{
  uint8
    value = 0;
  uint32
    offset;

  if (utils_ptg_fpga_io_regs_init())
  {
    return SOC_SAND_ERR;
  }

  if (utils_ptg_fpga_download(UTILS_PTG_IO_AGENT_FPGA_FILE, UTILS_PTG_IO_AGENT_FPGA_GPIO))
  {
    if (utils_ptg_fpga_flash_burn(UTILS_PTG_IO_AGENT_FPGA_FILE, UTILS_PTG_IO_AGENT_FPGA_GPIO))
    {
      return SOC_SAND_ERR;
    }
  }

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)|UTILS_PTG_DEVICE_FRONT_END_IO_AGENT_RESET;
  write_epld_reg((uint8)value,(uint32)offset);

  soc_sand_os_task_delay_milisec(16);

  if (UTILS_PTG_FPGA_IO_FLD_GET((Utils_ptg_fpga_io_regs.scratch_test.addr)) != 0x55)
  {
    UTILS_PTG_EXIT_AND_PRINT_ERROR("GFA PTG IO FPGA test failed\n");
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_ptg_init(
    SOC_SAND_IN  uint8                  start_server
  )
{
  uint8
    value = 0;
  uint32
    offset;

  if (utils_ptg_fpga_download(UTILS_PTG_DEVICE_FPGA_FILE, UTILS_PTG_DEVICE_FPGA_GPIO))
  {
    if (utils_ptg_fpga_flash_burn(UTILS_PTG_DEVICE_FPGA_FILE, UTILS_PTG_DEVICE_FPGA_GPIO))
    {
      return SOC_SAND_ERR;
    }
  }

  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)|UTILS_PTG_DEVICE_FRONT_END_PTG_0_RESET|UTILS_PTG_DEVICE_FRONT_END_PTG_1_RESET;
  write_epld_reg((uint8)value,(uint32)offset);

  soc_sand_os_task_delay_milisec(16);

  /* Change serdes rate to 156.26 */
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.miscellaneous_controls.reference_clock_select), 0x1);

  if (start_server)
  {
#if !DUNE_BCM
#endif
  }

  return SOC_SAND_OK;
}

uint8
  utils_ptg_is_line_card_connected(
  )
{
  uint8
    ret_bool = FALSE;
  static uint8
    have_answer = FALSE,
    answer = FALSE;
  uint32
    front_end_type;

  if (have_answer)
  {
    ret_bool = answer;
    goto exit;
  }

  /* If FPGA exist, Check for front card type. */
  front_end_get_host_board_type(&front_end_type);

  ret_bool = (front_end_type == SOC_FRONT_END_PTG ? TRUE : FALSE);

exit:
  answer = ret_bool;
  have_answer = TRUE;
  return ret_bool;
}

uint8
  utils_ptg_is_line_card_ptg_connected(
  )
{
#if UTILS_PTG_DEBUG_PTG
  return TRUE;
#else
  static uint8
    have_answer = FALSE,
    answer = FALSE;

  if (have_answer)
  {
    return answer;
  }

  if (utils_ptg_is_line_card_connected())
  {
    answer = ((0xf0 & UTILS_PTG_FPGA_IO_FLD_GET(Utils_ptg_fpga_io_regs.bridging_cards_sense.addr)) != 0xf0 ? TRUE : FALSE);
    have_answer = TRUE;
    return answer;
  }
  return FALSE;
#endif
}

uint8
  utils_ptg_is_line_card_phy_connected(
  )
{
#if UTILS_PTG_DEBUG_PHY
  return TRUE;
#else
  static uint8
    have_answer = FALSE,
    answer = FALSE;

  if (have_answer)
  {
    return answer;
  }

  if (utils_ptg_is_line_card_connected())
  {
    answer = ((0x0f & UTILS_PTG_FPGA_IO_FLD_GET(Utils_ptg_fpga_io_regs.bridging_cards_sense.addr)) != 0x0f ? TRUE : FALSE);
    have_answer = TRUE;
    return answer;
  }
  return FALSE;
#endif
}

void
  utils_ptg_mdio_set(
    SOC_SAND_IN  uint8                    phy_ndx,
    SOC_SAND_IN  uint32                   address,
    SOC_SAND_IN  uint16                   data
  )
{
  uint8
    busy;

  UTILS_PTG_INDIRECT_START;
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_dest.port_addr), phy_ndx);
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_dest.device_addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 19, 16));

  UTILS_PTG_INDIRECT_SESSION_START;
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[0].addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 7, 0));
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[1].addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 15, 8));
  UTILS_PTG_INDIRECT_SESSION_END(0x0);

  UTILS_PTG_INDIRECT_SESSION_START;
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[0].addr), UTILS_PTG_FPGA_IO_FLD_VAL_GET(data, 7, 0));
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[1].addr), UTILS_PTG_FPGA_IO_FLD_VAL_GET(data, 15, 8));
  UTILS_PTG_INDIRECT_SESSION_END(0x1);

  UTILS_PTG_INDIRECT_END;
}

uint16
  utils_ptg_mdio_get(
    SOC_SAND_IN  uint8                    phy_ndx,
    SOC_SAND_IN  uint32                   address
  )
{
  uint8
    busy;
  uint16
    val = 0xffff;

  UTILS_PTG_INDIRECT_START;
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_dest.port_addr), phy_ndx);
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_cfg_dest.device_addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 19, 16));

  /* Start indirect session to write address */
  UTILS_PTG_INDIRECT_SESSION_START;
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[0].addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 7, 0));
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.mdio_addr[1].addr), (uint8)UTILS_PTG_FPGA_IO_FLD_VAL_GET(address, 15, 8));
  UTILS_PTG_INDIRECT_SESSION_END(0x0);

  /* Start indirect session to perform operation */
  UTILS_PTG_INDIRECT_SESSION_START;
  UTILS_PTG_INDIRECT_SESSION_END(0x3);

  if (UTILS_PTG_FPGA_IO_FLD_GET((Utils_ptg_fpga_io_regs.mdio_cfg_trigger.read_valid)))
  {
    val = 0;
    val = val | UTILS_PTG_FPGA_IO_FLD_VAL_SET(UTILS_PTG_FPGA_IO_FLD_GET((Utils_ptg_fpga_io_regs.mdio_data[0].addr)), 7, 0);
    val = val | UTILS_PTG_FPGA_IO_FLD_VAL_SET(UTILS_PTG_FPGA_IO_FLD_GET((Utils_ptg_fpga_io_regs.mdio_data[1].addr)), 15, 8);
  }
  UTILS_PTG_INDIRECT_END;
  return val;
}

uint8
  utils_ptg_phy_single_is_aeluros(
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
  if ((utils_ptg_mdio_get(phy_ndx, 0x10002) == 0x0000) && (utils_ptg_mdio_get(phy_ndx, 0x10003) == 0x0000))
  {
    return TRUE;
  }
  return FALSE;
}

uint8
  utils_ptg_phy_single_is_amcc(
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
  if (utils_ptg_mdio_get(phy_ndx, 0x20002) == 0x0043)
  {
    return TRUE;
  }
  else if (phy_ndx == 0)
  {
    /*
     *	For some reason, the PHY type detection does not
     *  work on PHY 0 - so just assume it is AMCC
     */
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

SOC_SAND_RET
  utils_ptg_phy_aeluros_board_configuration_init(
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
  utils_ptg_mdio_set(phy_ndx, 0x10000, 0xa040);
  /* wait for 100ms for device to get out of reset. */
  soc_sand_os_task_delay_milisec(112);

  utils_ptg_mdio_set(phy_ndx, 0x1c214, 0x8288);
  utils_ptg_mdio_set(phy_ndx, 0x10800, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1c017, 0xfeb0);
  utils_ptg_mdio_set(phy_ndx, 0x1c013, 0xf341);
  utils_ptg_mdio_set(phy_ndx, 0x1c001, 0x0428);
  utils_ptg_mdio_set(phy_ndx, 0x1c210, 0x8000);
  utils_ptg_mdio_set(phy_ndx, 0x1c210, 0x8100);
  utils_ptg_mdio_set(phy_ndx, 0x1c210, 0x8000);
  utils_ptg_mdio_set(phy_ndx, 0x1c210, 0x0000);
  /* wait for 50ms for data path reset to complete. (mandatory patch) */
  soc_sand_os_task_delay_milisec(56);

  utils_ptg_mdio_set(phy_ndx, 0x1c214, 0x1abb);
  utils_ptg_mdio_set(phy_ndx, 0x1c55b, 0x1d02);
  utils_ptg_mdio_set(phy_ndx, 0x1c003, 0x0181);
  utils_ptg_mdio_set(phy_ndx, 0x1c010, 0x448a);

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_aeluros_microcode_init(
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
  UTILS_PTG_AELUROS_MICRO_CONTROLLER_START(phy_ndx);

  utils_ptg_mdio_set(phy_ndx, 0x1cc00, 0x2ff4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc01, 0x3cd4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc02, 0x2015);
  utils_ptg_mdio_set(phy_ndx, 0x1cc03, 0x3105);
  utils_ptg_mdio_set(phy_ndx, 0x1cc04, 0x6524);
  utils_ptg_mdio_set(phy_ndx, 0x1cc05, 0x27ff);
  utils_ptg_mdio_set(phy_ndx, 0x1cc06, 0x300f);
  utils_ptg_mdio_set(phy_ndx, 0x1cc07, 0x2c8b);
  utils_ptg_mdio_set(phy_ndx, 0x1cc08, 0x300b);
  utils_ptg_mdio_set(phy_ndx, 0x1cc09, 0x4009);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0a, 0x400e);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0b, 0x2f72);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0c, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0d, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0e, 0x2172);
  utils_ptg_mdio_set(phy_ndx, 0x1cc0f, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc10, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc11, 0x25d2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc12, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc13, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc14, 0xd01e);
  utils_ptg_mdio_set(phy_ndx, 0x1cc15, 0x27d2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc16, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc17, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc18, 0x2004);
  utils_ptg_mdio_set(phy_ndx, 0x1cc19, 0x3c84);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1a, 0x6436);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1b, 0x2007);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1c, 0x3f87);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1d, 0x8676);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1e, 0x40b7);
  utils_ptg_mdio_set(phy_ndx, 0x1cc1f, 0xa746);
  utils_ptg_mdio_set(phy_ndx, 0x1cc20, 0x4047);
  utils_ptg_mdio_set(phy_ndx, 0x1cc21, 0x5673);
  utils_ptg_mdio_set(phy_ndx, 0x1cc22, 0x2982);
  utils_ptg_mdio_set(phy_ndx, 0x1cc23, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc24, 0x13d2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc25, 0x8bbd);
  utils_ptg_mdio_set(phy_ndx, 0x1cc26, 0x2862);
  utils_ptg_mdio_set(phy_ndx, 0x1cc27, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc28, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc29, 0x2092);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2a, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2b, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2c, 0x5cc3);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2d, 0x0314);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2e, 0x2942);
  utils_ptg_mdio_set(phy_ndx, 0x1cc2f, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc30, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc31, 0xd019);
  utils_ptg_mdio_set(phy_ndx, 0x1cc32, 0x2032);
  utils_ptg_mdio_set(phy_ndx, 0x1cc33, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc34, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc35, 0x2a04);
  utils_ptg_mdio_set(phy_ndx, 0x1cc36, 0x3c74);
  utils_ptg_mdio_set(phy_ndx, 0x1cc37, 0x6435);
  utils_ptg_mdio_set(phy_ndx, 0x1cc38, 0x2fa4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc39, 0x3cd4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3a, 0x6624);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3b, 0x5563);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3c, 0x2d42);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3d, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3e, 0x13d2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc3f, 0x464d);
  utils_ptg_mdio_set(phy_ndx, 0x1cc40, 0x2862);
  utils_ptg_mdio_set(phy_ndx, 0x1cc41, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc42, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc43, 0x2032);
  utils_ptg_mdio_set(phy_ndx, 0x1cc44, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc45, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc46, 0x2fb4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc47, 0x3cd4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc48, 0x6624);
  utils_ptg_mdio_set(phy_ndx, 0x1cc49, 0x5563);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4a, 0x2d42);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4b, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4c, 0x13d2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4d, 0x2ed2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4e, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc4f, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc50, 0x2fd2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc51, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc52, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc53, 0x0004);
  utils_ptg_mdio_set(phy_ndx, 0x1cc54, 0x2942);
  utils_ptg_mdio_set(phy_ndx, 0x1cc55, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc56, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc57, 0x2092);
  utils_ptg_mdio_set(phy_ndx, 0x1cc58, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc59, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5a, 0x5cc3);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5b, 0x0317);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5c, 0x2f72);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5d, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5e, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc5f, 0x2942);
  utils_ptg_mdio_set(phy_ndx, 0x1cc60, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc61, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc62, 0x22cd);
  utils_ptg_mdio_set(phy_ndx, 0x1cc63, 0x301d);
  utils_ptg_mdio_set(phy_ndx, 0x1cc64, 0x2862);
  utils_ptg_mdio_set(phy_ndx, 0x1cc65, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc66, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc67, 0x2ed2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc68, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc69, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6a, 0x2d72);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6b, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6c, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6d, 0x628f);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6e, 0x2112);
  utils_ptg_mdio_set(phy_ndx, 0x1cc6f, 0x3012);
  utils_ptg_mdio_set(phy_ndx, 0x1cc70, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc71, 0x5aa3);
  utils_ptg_mdio_set(phy_ndx, 0x1cc72, 0x2dc2);
  utils_ptg_mdio_set(phy_ndx, 0x1cc73, 0x3002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc74, 0x1312);
  utils_ptg_mdio_set(phy_ndx, 0x1cc75, 0x6f72);
  utils_ptg_mdio_set(phy_ndx, 0x1cc76, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc77, 0x2807);
  utils_ptg_mdio_set(phy_ndx, 0x1cc78, 0x31a7);
  utils_ptg_mdio_set(phy_ndx, 0x1cc79, 0x20c4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7a, 0x3c24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7b, 0x6724);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7c, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7d, 0x2807);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7e, 0x3187);
  utils_ptg_mdio_set(phy_ndx, 0x1cc7f, 0x20c4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc80, 0x3c24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc81, 0x6724);
  utils_ptg_mdio_set(phy_ndx, 0x1cc82, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc83, 0x2514);
  utils_ptg_mdio_set(phy_ndx, 0x1cc84, 0x3c64);
  utils_ptg_mdio_set(phy_ndx, 0x1cc85, 0x6436);
  utils_ptg_mdio_set(phy_ndx, 0x1cc86, 0xdff4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc87, 0x6436);
  utils_ptg_mdio_set(phy_ndx, 0x1cc88, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc89, 0x40a4);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8a, 0x643c);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8b, 0x4016);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8c, 0x8c6c);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8d, 0x2b24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8e, 0x3c24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc8f, 0x6435);
  utils_ptg_mdio_set(phy_ndx, 0x1cc90, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc91, 0x2b24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc92, 0x3c24);
  utils_ptg_mdio_set(phy_ndx, 0x1cc93, 0x643a);
  utils_ptg_mdio_set(phy_ndx, 0x1cc94, 0x4025);
  utils_ptg_mdio_set(phy_ndx, 0x1cc95, 0x8a5a);
  utils_ptg_mdio_set(phy_ndx, 0x1cc96, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cc97, 0x2731);
  utils_ptg_mdio_set(phy_ndx, 0x1cc98, 0x3011);
  utils_ptg_mdio_set(phy_ndx, 0x1cc99, 0x1001);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9a, 0xc7a0);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9b, 0x0100);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9c, 0xc502);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9d, 0x53ac);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9e, 0xc503);
  utils_ptg_mdio_set(phy_ndx, 0x1cc9f, 0xd5d5);
  utils_ptg_mdio_set(phy_ndx, 0x1cca0, 0xc600);
  utils_ptg_mdio_set(phy_ndx, 0x1cca1, 0x2a6d);
  utils_ptg_mdio_set(phy_ndx, 0x1cca2, 0xc601);
  utils_ptg_mdio_set(phy_ndx, 0x1cca3, 0x2a4c);
  utils_ptg_mdio_set(phy_ndx, 0x1cca4, 0xc602);
  utils_ptg_mdio_set(phy_ndx, 0x1cca5, 0x0111);
  utils_ptg_mdio_set(phy_ndx, 0x1cca6, 0xc60c);
  utils_ptg_mdio_set(phy_ndx, 0x1cca7, 0x5900);
  utils_ptg_mdio_set(phy_ndx, 0x1cca8, 0xc710);
  utils_ptg_mdio_set(phy_ndx, 0x1cca9, 0x0700);
  utils_ptg_mdio_set(phy_ndx, 0x1ccaa, 0xc718);
  utils_ptg_mdio_set(phy_ndx, 0x1ccab, 0x0700);
  utils_ptg_mdio_set(phy_ndx, 0x1ccac, 0xc720);
  utils_ptg_mdio_set(phy_ndx, 0x1ccad, 0x4700);
  utils_ptg_mdio_set(phy_ndx, 0x1ccae, 0xc801);
  utils_ptg_mdio_set(phy_ndx, 0x1ccaf, 0x7f50);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb0, 0xc802);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb1, 0x7760);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb2, 0xc803);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb3, 0x7fce);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb4, 0xc804);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb5, 0x5700);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb6, 0xc805);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb7, 0x5f11);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb8, 0xc806);
  utils_ptg_mdio_set(phy_ndx, 0x1ccb9, 0x4751);
  utils_ptg_mdio_set(phy_ndx, 0x1ccba, 0xc807);
  utils_ptg_mdio_set(phy_ndx, 0x1ccbb, 0x57e1);
  utils_ptg_mdio_set(phy_ndx, 0x1ccbc, 0xc808);
  utils_ptg_mdio_set(phy_ndx, 0x1ccbd, 0x2700);
  utils_ptg_mdio_set(phy_ndx, 0x1ccbe, 0xc809);
  utils_ptg_mdio_set(phy_ndx, 0x1ccbf, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc0, 0xc821);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc1, 0x0002);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc2, 0xc822);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc3, 0x0014);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc4, 0xc832);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc5, 0x1186);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc6, 0xc847);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc7, 0x1e02);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc8, 0xc013);
  utils_ptg_mdio_set(phy_ndx, 0x1ccc9, 0xf341);
  utils_ptg_mdio_set(phy_ndx, 0x1ccca, 0xc01a);
  utils_ptg_mdio_set(phy_ndx, 0x1cccb, 0x0446);
  utils_ptg_mdio_set(phy_ndx, 0x1cccc, 0xc024);
  utils_ptg_mdio_set(phy_ndx, 0x1cccd, 0x1000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccce, 0xc025);
  utils_ptg_mdio_set(phy_ndx, 0x1cccf, 0x0a00);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd0, 0xc026);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd1, 0x0c0c);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd2, 0xc027);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd3, 0x0c0c);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd4, 0xc029);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd5, 0x00a0);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd6, 0xc030);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd7, 0x0a00);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd8, 0xc03c);
  utils_ptg_mdio_set(phy_ndx, 0x1ccd9, 0x001c);
  utils_ptg_mdio_set(phy_ndx, 0x1ccda, 0xc005);
  utils_ptg_mdio_set(phy_ndx, 0x1ccdb, 0x7a06);
  utils_ptg_mdio_set(phy_ndx, 0x1ccdc, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccdd, 0x2731);
  utils_ptg_mdio_set(phy_ndx, 0x1ccde, 0x3011);
  utils_ptg_mdio_set(phy_ndx, 0x1ccdf, 0x1001);
  utils_ptg_mdio_set(phy_ndx, 0x1cce0, 0xc620);
  utils_ptg_mdio_set(phy_ndx, 0x1cce1, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1cce2, 0xc621);
  utils_ptg_mdio_set(phy_ndx, 0x1cce3, 0x003f);
  utils_ptg_mdio_set(phy_ndx, 0x1cce4, 0xc622);
  utils_ptg_mdio_set(phy_ndx, 0x1cce5, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1cce6, 0xc623);
  utils_ptg_mdio_set(phy_ndx, 0x1cce7, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1cce8, 0xc624);
  utils_ptg_mdio_set(phy_ndx, 0x1cce9, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccea, 0xc625);
  utils_ptg_mdio_set(phy_ndx, 0x1cceb, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccec, 0xc627);
  utils_ptg_mdio_set(phy_ndx, 0x1cced, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccee, 0xc628);
  utils_ptg_mdio_set(phy_ndx, 0x1ccef, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf0, 0xc62c);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf1, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf2, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf3, 0x2806);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf4, 0x3cb6);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf5, 0xc161);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf6, 0x6134);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf7, 0x6135);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf8, 0x5443);
  utils_ptg_mdio_set(phy_ndx, 0x1ccf9, 0x0303);
  utils_ptg_mdio_set(phy_ndx, 0x1ccfa, 0x6524);
  utils_ptg_mdio_set(phy_ndx, 0x1ccfb, 0x000b);
  utils_ptg_mdio_set(phy_ndx, 0x1ccfc, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1ccfd, 0x2104);
  utils_ptg_mdio_set(phy_ndx, 0x1ccfe, 0x3c24);
  utils_ptg_mdio_set(phy_ndx, 0x1ccff, 0x2105);
  utils_ptg_mdio_set(phy_ndx, 0x1cd00, 0x3805);
  utils_ptg_mdio_set(phy_ndx, 0x1cd01, 0x6524);
  utils_ptg_mdio_set(phy_ndx, 0x1cd02, 0xdff4);
  utils_ptg_mdio_set(phy_ndx, 0x1cd03, 0x4005);
  utils_ptg_mdio_set(phy_ndx, 0x1cd04, 0x6524);
  utils_ptg_mdio_set(phy_ndx, 0x1cd05, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cd06, 0x5dd3);
  utils_ptg_mdio_set(phy_ndx, 0x1cd07, 0x0306);
  utils_ptg_mdio_set(phy_ndx, 0x1cd08, 0x2ff7);
  utils_ptg_mdio_set(phy_ndx, 0x1cd09, 0x38f7);
  utils_ptg_mdio_set(phy_ndx, 0x1cd0a, 0x60b7);
  utils_ptg_mdio_set(phy_ndx, 0x1cd0b, 0xdffd);
  utils_ptg_mdio_set(phy_ndx, 0x1cd0c, 0x000a);
  utils_ptg_mdio_set(phy_ndx, 0x1cd0d, 0x1002);
  utils_ptg_mdio_set(phy_ndx, 0x1cd0e, 0x0000);

  utils_ptg_mdio_set(phy_ndx, 0x1ca00, 0x0080);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);
  utils_ptg_mdio_set(phy_ndx, 0x1ca12, 0x0000);

  UTILS_PTG_AELUROS_MICRO_CONTROLLER_END(phy_ndx);

  utils_ptg_mdio_set(phy_ndx, 0x1c012, 0x0000);

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_single_aeluros_init(
    SOC_SAND_IN  uint32                  phy_ndx,
    SOC_SAND_IN  uint8                  enable
  )
{
  if (enable)
  {
    if(utils_ptg_phy_aeluros_board_configuration_init(phy_ndx))
    {
      return SOC_SAND_ERR;
    }

    if(utils_ptg_phy_aeluros_microcode_init(phy_ndx))
    {
      return SOC_SAND_ERR;
    }

    soc_sand_os_printf("PHY %d (Aeluros) - loaded\n", phy_ndx);
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_amcc_microcode_init(
    SOC_SAND_IN  uint32                   *amcc_buff,
    SOC_SAND_IN  uint32                  size,
    SOC_SAND_IN  uint32                  phy_ndx
  )
{
  const uint32
    *amcc_buff_ptr = amcc_buff;
  uint32
    address;
  uint16
    data;
  uint32
    ind;

  for (ind = 0; ind < size; ++ind)
  {
    address = (uint32)(*amcc_buff_ptr++);
    data = (uint16)(*amcc_buff_ptr++);
    utils_ptg_mdio_set(phy_ndx, address, data);
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_single_amcc_init(
    SOC_SAND_IN  uint32                   *amcc_buff,
    SOC_SAND_IN  uint32                  size,
    SOC_SAND_IN  uint32                  phy_ndx,
    SOC_SAND_IN  uint8                  enable
  )
{
  if (enable)
  {
    if(utils_ptg_phy_amcc_microcode_init(amcc_buff, size, phy_ndx))
    {
      return SOC_SAND_ERR;
    }

    utils_ptg_mdio_set(phy_ndx, 0x1d006, 0x0012);
    utils_ptg_mdio_set(phy_ndx, 0x1d007, 0x001a);
  }

  soc_sand_os_printf("PHY %d (AMCC) - loaded\n", phy_ndx);

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_single_init(
    SOC_SAND_IN  uint32                   *amcc_buff,
    SOC_SAND_IN  uint32                  size,
    SOC_SAND_IN  uint32                  phy_ndx,
    SOC_SAND_IN  uint8                  enable
  )
{
  UTILS_PTG_FPGA_IO_FLD_SET((Utils_ptg_fpga_io_regs.optical_phys_reset.phy[phy_ndx]), 0x1);
  soc_sand_os_task_delay_milisec(112);

  if (utils_ptg_phy_single_is_aeluros(phy_ndx))
  {
    if (utils_ptg_phy_single_aeluros_init(phy_ndx, enable))
    {
      soc_sand_os_printf(
        "PHY %d (AELUROS) - not loaded\n",
        phy_ndx
      );
    }
  }
  else if (utils_ptg_phy_single_is_amcc(phy_ndx)) 
  {
    if (!Utils_amcc_skip)
    {
      if (utils_ptg_phy_single_amcc_init(amcc_buff, size, phy_ndx, enable))
      {
        soc_sand_os_printf(
          "PHY %d (AMCC) - not loaded\n",
          phy_ndx
        );
      }
    }
  }
  else
  {
    soc_sand_os_printf(
      "PHY %d (Unknown) - not loaded\n",
      phy_ndx
    );
  }

  return SOC_SAND_OK;
}

SOC_SAND_RET
  utils_ptg_phy_amcc_init(
    SOC_SAND_OUT uint32                   *amcc_buff,
    SOC_SAND_OUT uint32                  *size
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
#if !DUNE_BCM
  const char
    *file_name = "amcc_init.bin";
  char
    err_msg[8*80] = "";
  uint32
    *amcc_buff_ptr = amcc_buff;
  FILE
    *file = NULL;
  uint32
    val[2];
  uint8
    erase = FALSE;
#endif

  *size = 0;

#if !DUNE_BCM
  if(download_cli_file(file_name, &err_msg[0], 0xffffffff))
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

  if ((file = fopen(file_name, "rb")) == NULL)
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

  while (2 == fread(val, sizeof(uint32), 2, file))
  {
    *(size) += 1;
    *(amcc_buff_ptr++) = val[0];
    *(amcc_buff_ptr++) = val[1];
  }

  fclose(file);

exit:
  if (erase && erase_cli_file(file_name, err_msg))
  {
    return SOC_SAND_ERR;
  }
#endif
  return ret;
}

SOC_SAND_RET
  utils_ptg_phy_init(
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    phy_i;
  uint32
    *amcc_buff = NULL;
  uint32
    alloc_size = UTILS_PTG_AMCC_ENTRY_SIZE * UTILS_PTG_AMCC_SIZE * sizeof(uint32),
    size = 0;

  UTILS_PTG_FPGA_IO_REG_CLEAR((Utils_ptg_fpga_io_regs.optical_phys_reset.addr));
  soc_sand_os_task_delay_milisec(112);

  if ((amcc_buff = (uint32*)soc_sand_os_malloc_any_size(alloc_size)) == NULL)
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

  if (soc_sand_os_memset(amcc_buff, 0x0, alloc_size))
  {
    ret = SOC_SAND_ERR;
    goto exit;
  }

  if (utils_ptg_phy_amcc_init(amcc_buff, &size))
  {
    soc_sand_os_printf(
        "PHY (AMCC) - Configuration file missing, all disabled\r\n"   
      );
    Utils_amcc_skip = TRUE;
  }

  for (phy_i = 0; phy_i < UTILS_PTG_NOF_PHY; ++phy_i)
  {
    /* Init in case the phy is not amcc */
    if (utils_ptg_phy_single_init(amcc_buff, size, phy_i, TRUE))
    {
      ret = SOC_SAND_ERR;
      goto exit;
    }
  }

exit:
  if (amcc_buff != NULL)
  {
    soc_sand_os_free_any_size(amcc_buff);
    amcc_buff =  NULL;
  }
  return ret;
}

SOC_SAND_RET
  utils_ptg_init_host_board(
    SOC_SAND_IN  char                       board_version[B_HOST_MAX_DEFAULT_BOARD_VERSION_LEN]
  )
{
  uint8
    value = 0x0;
  uint32
    offset;

  /* Reset all devices */
  offset = (uint32)(&(((EPLD_REGS *)0)->reset_control));
  value = read_epld_reg((uint32)offset)&(0x0f);
  write_epld_reg((uint8)value,(uint32)offset);
  soc_sand_os_printf("+ .Getting all PTG devices out of reset  .....     pass\n");

  /* Initialize system flash and release it from reset */
  /* Initialize FPGA IO agent and release it from reset */
  if (utils_ptg_fpga_io_agent_init())
  {
    soc_sand_os_printf("\n+ .Loading PTG IO agent FPGA             .....     fail\n");
    return SOC_SAND_ERR;
  }
  soc_sand_os_printf("\n+ .Loading PTG IO agent FPGA             .....     pass\n");

  if (utils_ptg_is_line_card_ptg_connected())
  {
    /* Initialize PTG FPGAs and release both from reset */
    if (utils_ptg_ptg_init(TRUE))
    {
      soc_sand_os_printf("+\n. PTG is connected. Loading application .....     fail\n");
      return SOC_SAND_ERR;
    }
    soc_sand_os_printf("\n+. PTG is connected. Loading application .....     pass\n");
  }

  if (utils_ptg_is_line_card_phy_connected())
  {
    /* Initialize PHYs */
    if (utils_ptg_phy_init())
    {
      soc_sand_os_printf("+. PHY is connected. Loading application .....     fail\n");
      return SOC_SAND_ERR;
    }
    soc_sand_os_printf("+. PHY is connected. Loading application .....     pass\n");
  }

  return SOC_SAND_OK;
}

/* } */

