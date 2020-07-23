/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        utils_fe1600_card.c
 * Purpose:     fe1600 card Utilities.
 */
#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__) || defined(__DUNE_GTS_BCM_CPU__)) && defined(BCM_DFE_SUPPORT)

/*************                                                       
 * INCLUDES  *                                                       
 */                                                      
/* { */

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/i2c.h>
#include <soc/dcmn/utils_fpga.h>


#include <appl/diag/dcmn/bsp_cards_consts.h>
#include <appl/diag/dfe/utils_fe1600_card.h>
#include <sal/appl/io.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


/* } */                                                              
                                                                     
/*************                                                       
 * DEFINES   *                                                       
 */                                                      
/* { */  

/* SYNTHESIZER Defs*/ 
#define SYNTHESIZER_ID        0x60 
#define SYNTHESIZER_REG7TO12  0x07 
#define SYNTHESIZER_REG135    0x87 
#define SYNTHESIZER_REG137    0x89

/* Power */
#define  FE1600_CARD_POWER_MONITOR_CH_NUM 13
#define  FE3200_CARD_POWER_MONITOR_CH_NUM 10


#define UTILS_FE1600_INIT_ERR_DEFS(e_proc_name)  \
  uint32 m_ret = 0;                       \
  const char* m_proc_name = e_proc_name;


#define UTILS_FE1600_EXIT_IF_ERR(e_sand_err,e_err_code) \
{                                         \
  if (e_sand_err)                         \
  {                                       \
    m_ret = e_err_code ;                  \
    goto exit ;                           \
  }                                       \
}

#define UTILS_FE1600_EXIT_AND_PRINT_ERR           \
{                                          \
  if (m_ret)                               \
    {                                      \
      cli_out(\
              "\n\r    + %-60s -- ERROR %u.\n\r",m_proc_name, m_ret);\
    }                                      \
    return m_ret;                          \
}


/*************                                                       
 * TYPE DEFS *                                                       
 */                                                      
/* { */     
 
typedef struct 
{ 
  uint8 address; 
  uint8 msb; 
  uint8 lsb; 
}FE1600_CARD_FLD_ADDR; 
 
typedef uint8 (*FE1600_CARD_FPGA_IO_FIELD_GET) (const FE1600_CARD_FLD_ADDR *); 
typedef void (*FE1600_CARD_FPGA_IO_FIELD_SET) (const FE1600_CARD_FLD_ADDR *, uint8);     
 
typedef struct   
{ 
  SOC_BSP_CARDS_DEFINES           card_type; 
  char                        *agent_file; 
  FE1600_CARD_FPGA_IO_FIELD_GET io_agent_field_get_func; 
  FE1600_CARD_FPGA_IO_FIELD_SET io_agent_field_set_func; 
 
  int  
    (*i2c_syntmuxset) ( 
    const FE1600_CARD_SYNT_TYPE     target 
    ); 
 
  int   
    (*i2c_read) ( 
    uint8            unit, 
    uint8            internal_address, 
    uint8            buffer_size, 
    uint8           *buffer 
    ); 
  int  
    (*i2c_write) ( 
    uint8            unit, 
    uint8            internal_address, 
    uint8            buffer_size, 
    uint8           *buffer 
    ); 
 
}FE1600_CARD_BOARD_INFO; 
 
typedef struct 
{ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } version; /* 0x0 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } test; /* 0x1 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_1; /* 0x2 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_2; /* 0x3 */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_3; /* 0x4 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_4; /* 0x5 */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_5; /* 0x6 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } fe1600_puc_6; /* 0x7 */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR disconnect; 
    FE1600_CARD_FLD_ADDR bypass_oe; 
    FE1600_CARD_FLD_ADDR core_pll_bypass; 
    FE1600_CARD_FLD_ADDR serdes_lcpll_bypass; 
    FE1600_CARD_FLD_ADDR puc_oe; 
  } fe1600_test; /* 0x8 */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR clk_sel_oe; 
    FE1600_CARD_FLD_ADDR core_clk_sel; 
    FE1600_CARD_FLD_ADDR srd_clk0_sel; 
    FE1600_CARD_FLD_ADDR srd_clk1_sel; 
    FE1600_CARD_FLD_ADDR srd_clk2_sel; 
    FE1600_CARD_FLD_ADDR srd_clk3_sel; 
  } referance_clks_mux_select; /* 0xa */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR core_osc_oe; 
    FE1600_CARD_FLD_ADDR pcie_osc_oe; 
    FE1600_CARD_FLD_ADDR srd_osc0_oe; 
    FE1600_CARD_FLD_ADDR srd_osc1_oe; 
    FE1600_CARD_FLD_ADDR srd_osc2_oe; 
    FE1600_CARD_FLD_ADDR srd_osc3_oe; 
  } referance_clks_oe; /* 0xb */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } gpio_direction;  /* 0xc */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
  } gpio_value;  /* 0xd */  
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR fe1600_rst_n; 
    FE1600_CARD_FLD_ADDR pcie_bridge_rst_n; 
    FE1600_CARD_FLD_ADDR pcie_sw_rst_n; 
  } reset; /* 0xe */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR vps_oe; 
    FE1600_CARD_FLD_ADDR vps0; 
    FE1600_CARD_FLD_ADDR vps1; 
    FE1600_CARD_FLD_ADDR fe1600_pwr_down; 
  } power_managment; /* 0xf */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR mng_exist; 
    FE1600_CARD_FLD_ADDR mng_cpu_exist; 
    FE1600_CARD_FLD_ADDR cpu_exist; 
    FE1600_CARD_FLD_ADDR lc1_exist; 
    FE1600_CARD_FLD_ADDR lc2_exist; 
    FE1600_CARD_FLD_ADDR temp_alarm; 
  } status_1; /* 0x10 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR standalone; 
    FE1600_CARD_FLD_ADDR mng_cpu_exist; 
    FE1600_CARD_FLD_ADDR slot_id_0; 
    FE1600_CARD_FLD_ADDR slot_id_1; 
    FE1600_CARD_FLD_ADDR pzbx_mc; 
  } status_2; /* 0x11 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR cxp0_present; 
    FE1600_CARD_FLD_ADDR cxp1_present; 
    FE1600_CARD_FLD_ADDR cxp2_present; 
    FE1600_CARD_FLD_ADDR cxp3_present; 
    FE1600_CARD_FLD_ADDR cxp4_present; 
  } cxp_present; /* 0x12 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR cxp0_reset_n; 
    FE1600_CARD_FLD_ADDR cxp1_reset_n; 
    FE1600_CARD_FLD_ADDR cxp2_reset_n; 
    FE1600_CARD_FLD_ADDR cxp3_reset_n; 
    FE1600_CARD_FLD_ADDR cxp4_reset_n; 
  } cxp_reset; /* 0x13 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR cxp0_int_n; 
    FE1600_CARD_FLD_ADDR cxp1_int_n; 
    FE1600_CARD_FLD_ADDR cxp2_int_n; 
    FE1600_CARD_FLD_ADDR cxp3_int_n; 
    FE1600_CARD_FLD_ADDR cxp4_int_n; 
  } cxp_interrupt; /* 0x14 */ 
  struct 
  { 
    FE1600_CARD_FLD_ADDR addr; 
    FE1600_CARD_FLD_ADDR led_panel_active_n; 
    FE1600_CARD_FLD_ADDR led_panel_failure_n; 
  } led; /* 0x15 */ 
 
 
 
 
} FE1600_CARD_FPGA_IO_REGS;  

/* } */                                                             
                                                                    
/*************                                                      
 * GLOBALS   *                                                      
 */                                                     
/* { */

static FE1600_CARD_BOARD_INFO 
  Fe1600_card_board_info;

static FE1600_CARD_FPGA_IO_REGS
  Fe1600_card_fpga_io_regs;

static unsigned char
  Fe1600_card_fpga_io_regs_init = FALSE;    
                                                  
#ifndef __KERNEL__
static int fe1600_card_power_monitor_arr[14] = {
    0x0,
    0x00,
    0x11,
    0x02,
    0x03,
    0x14,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x1A,
    0x0B,
    0x1C,    
};
 
static char *fe1600_card_power_monitor_string_arr[] = {
    NULL,
    "FE1600 1V Core voltage",
    "FE1600 3.3V I/O and PLL analog voltage",
    "FE1600 1V/1.4V voltage for TVDD driver",
    "FE1600 1V SerDes analog supply",
    "FE1600 1V core current",
    "FE1600 3.3V current",
    "FE1600 1V/1.4V TVDD current",
    "FE1600 1V SerDes analog current",
    "TVDD10,RVDD10 segment 1 voltage",
    "PVDD10 segment 1 voltage",
    "CPU 5V voltage",
    "1V PCIe SW voltage",
    "3.3V"
};

static int fe3200_card_power_monitor_arr[14] = {
    0x0,
    0x00,
    0x11,
    0x02,
    0x03,
    0x14,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09
};
 
static char *fe3200_card_power_monitor_string_arr[] = {
    NULL,
    "FE3200 1V Core voltage",
    "FE3200 3.3V VDDO",
    "FE3200 1.25V TVDD",
    "FE3200 1V Analog supply",
    "FE3200 1.8V",
    "FE3200 1V Core Current Sense",
    "FE3200 3.3V VDDO Current Sense",
    "FE3200 1.25V TVDD Current Sense",
    "FE3200 1V Analog supply Current Sense",
    "FE3200 1.8V Current Sense"
};
#endif /* __KERNEL__ */
  
/* } */                                                             
                                                                    
/*************                                                      
 * FUNCTIONS *                                                      
 */                                                     
/* { */     

uint32
  fe1600_card_fpga_io_regs_init(void)
{
  if (!Fe1600_card_fpga_io_regs_init)
  {
    Fe1600_card_fpga_io_regs.version.addr.address = 0x00; 
    Fe1600_card_fpga_io_regs.version.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.version.addr.msb = 7;

    Fe1600_card_fpga_io_regs.test.addr.address = 0x01;
    Fe1600_card_fpga_io_regs.test.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.test.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_1.addr.address = 0x02;
    Fe1600_card_fpga_io_regs.fe1600_puc_1.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_1.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_2.addr.address = 0x03;
    Fe1600_card_fpga_io_regs.fe1600_puc_2.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_2.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_3.addr.address = 0x04;
    Fe1600_card_fpga_io_regs.fe1600_puc_3.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_3.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_4.addr.address = 0x05;
    Fe1600_card_fpga_io_regs.fe1600_puc_4.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_4.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_5.addr.address = 0x06;
    Fe1600_card_fpga_io_regs.fe1600_puc_5.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_5.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_puc_6.addr.address = 0x07;
    Fe1600_card_fpga_io_regs.fe1600_puc_6.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_puc_6.addr.msb = 7;

    Fe1600_card_fpga_io_regs.fe1600_test.addr.address = 0x08;
    Fe1600_card_fpga_io_regs.fe1600_test.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.fe1600_test.addr.msb = 7;

    Fe1600_card_fpga_io_regs.referance_clks_mux_select.addr.address = 0x0a;
    Fe1600_card_fpga_io_regs.referance_clks_mux_select.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.referance_clks_mux_select.addr.msb = 7;

    Fe1600_card_fpga_io_regs.referance_clks_oe.addr.address = 0x0b;
    Fe1600_card_fpga_io_regs.referance_clks_oe.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.referance_clks_oe.addr.msb = 7;

    Fe1600_card_fpga_io_regs.gpio_direction.addr.address = 0x0c;
    Fe1600_card_fpga_io_regs.gpio_direction.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.gpio_direction.addr.msb = 7;

    Fe1600_card_fpga_io_regs.gpio_value.addr.address = 0x0d;
    Fe1600_card_fpga_io_regs.gpio_value.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.gpio_value.addr.msb = 7;

    Fe1600_card_fpga_io_regs.reset.addr.address = 0x0e;
    Fe1600_card_fpga_io_regs.reset.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.reset.addr.msb = 7;
    Fe1600_card_fpga_io_regs.reset.fe1600_rst_n.address = 0x0e;
    Fe1600_card_fpga_io_regs.reset.fe1600_rst_n.lsb = 0;
    Fe1600_card_fpga_io_regs.reset.fe1600_rst_n.msb = 0;
    Fe1600_card_fpga_io_regs.reset.pcie_bridge_rst_n.address = 0x0e;
    Fe1600_card_fpga_io_regs.reset.pcie_bridge_rst_n.lsb = 1;
    Fe1600_card_fpga_io_regs.reset.pcie_bridge_rst_n.msb = 1;
    Fe1600_card_fpga_io_regs.reset.pcie_sw_rst_n.address = 0x0e;
    Fe1600_card_fpga_io_regs.reset.pcie_sw_rst_n.lsb = 2;
    Fe1600_card_fpga_io_regs.reset.pcie_sw_rst_n.msb = 2;

    Fe1600_card_fpga_io_regs.power_managment.addr.address = 0x0f;
    Fe1600_card_fpga_io_regs.power_managment.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.power_managment.addr.msb = 7;

    Fe1600_card_fpga_io_regs.status_1.addr.address = 0x10;
    Fe1600_card_fpga_io_regs.status_1.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.status_1.addr.msb = 7;

    Fe1600_card_fpga_io_regs.status_2.addr.address = 0x11;
    Fe1600_card_fpga_io_regs.status_2.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.status_2.addr.msb = 7;

    Fe1600_card_fpga_io_regs.cxp_present.addr.address = 0x12;
    Fe1600_card_fpga_io_regs.cxp_present.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.cxp_present.addr.msb = 7;

    Fe1600_card_fpga_io_regs.cxp_reset.addr.address = 0x13;
    Fe1600_card_fpga_io_regs.cxp_reset.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.cxp_reset.addr.msb = 7;

    Fe1600_card_fpga_io_regs.cxp_interrupt.addr.address = 0x14;
    Fe1600_card_fpga_io_regs.cxp_interrupt.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.cxp_interrupt.addr.msb = 7;

    Fe1600_card_fpga_io_regs.led.addr.address = 0x15;
    Fe1600_card_fpga_io_regs.led.addr.lsb = 0;
    Fe1600_card_fpga_io_regs.led.addr.msb = 7;
    Fe1600_card_fpga_io_regs.led.led_panel_active_n.address = 0x15;
    Fe1600_card_fpga_io_regs.led.led_panel_active_n.lsb = 0;
    Fe1600_card_fpga_io_regs.led.led_panel_active_n.msb = 0;
    Fe1600_card_fpga_io_regs.led.led_panel_failure_n.address = 0x15;
    Fe1600_card_fpga_io_regs.led.led_panel_failure_n.lsb = 1;
    Fe1600_card_fpga_io_regs.led.led_panel_failure_n.msb = 1;

    Fe1600_card_fpga_io_regs_init = TRUE;
  }

  return 0;
}

/* 
 *  Fe1600 card board info - spesific board function (i2c, access)
 */
 
void 
  fe1600_card_fpga_io_32_fld_set(
    const  FE1600_CARD_FLD_ADDR         *fld, 
    const  uint8                    data 
  ) 
{ 
#ifndef __KERNEL__
  int 
    value = 0; 
  uint8 
    val = 0; 
 
  /* cli_out("fe1600_card_fpga_io_32_fld_set(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, data=0x%x\n",fld->address,fld->msb,fld->lsb,data);   */

  cpu_i2c_read(FE1600_CARD_I2C_IO_AGENT_DEVICE_ADDR, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, &value); 

  val = (uint8)(value & 0xff); 
 
  /* cli_out("fe1600_card_fpga_io_32_fld_set(). before: value=0x%x, val=0x%x;\n",value,val);  */
       
  val &= SOC_SAND_ZERO_BITS_MASK(fld->msb,fld->lsb); 
  val |= SOC_SAND_SET_BITS_RANGE(data,fld->msb,fld->lsb); 
  
  /* cli_out("fe1600_card_fpga_io_32_fld_set(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, data=0x%x, val=0x%x\n",fld->address,fld->msb,fld->lsb,data,val);  */
 
  cpu_i2c_write(FE1600_CARD_I2C_IO_AGENT_DEVICE_ADDR, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, val); 
#endif /* __KERNEL__ */
  cli_out("This function is unavailable in Kernel mode\n");
} 
 
uint8 
  fe1600_card_fpga_io_32_fld_get( 
    const  FE1600_CARD_FLD_ADDR         *fld 
  ) 
{ 
#ifndef __KERNEL__
  int 
    value = 0; 
  uint8 
    val = 0, ret_val = 0;   

  cpu_i2c_read(FE1600_CARD_I2C_IO_AGENT_DEVICE_ADDR, fld->address, CPU_I2C_ALEN_LONG_DLEN_LONG, &value); 

  val = (uint8)(value & 0xff); 
 
  /* cli_out("fe1600_card_fpga_io_32_fld_get(). before: value=0x%x, val=0x%x\n",value,val);  */
 
  ret_val = SOC_SAND_GET_BITS_RANGE(val,fld->msb,fld->lsb); 
 
  /* cli_out("fe1600_card_fpga_io_32_fld_get(). fld->address=0x%x, fld->msb=%d, ,fld->lsb=%d, value=0x%x\n",fld->address,fld->msb,fld->lsb,ret_val);  */
 
  return ret_val; 
#endif /* __KERNEL__ */
  cli_out("This function is unavailable in Kernel mode\n");
  return 0;
} 

uint8
  fe1600_card_fpga_io_fld_get(
    const  FE1600_CARD_FLD_ADDR         *fld
  )
{
  return Fe1600_card_board_info.io_agent_field_get_func(fld);
}

void
  fe1600_card_fpga_io_fld_set(
    const  FE1600_CARD_FLD_ADDR         *fld,
    const  uint8                    data
  )
{
  Fe1600_card_board_info.io_agent_field_set_func(fld, data);
}
 
int   
  fe1600_card_i2c_read ( 
    uint8            unit, 
    uint8            internal_address, 
    uint8            buffer_size, 
    uint8           *buffer 
  )    
{
#ifndef __KERNEL__
    int 
        ret = 0, 
        value = 0, 
        i = 0;
 
    for(i=0;i<buffer_size;i++) {
        ret = cpu_i2c_read(unit, internal_address + i, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &value); 
        if (ret) { 
            cli_out("%s(): ERROR: cpu_i2c_read(). FAILED !!!\n", FUNCTION_NAME()); 
            ret = -1; 
            goto exit; 
        }

        value &= 0xff;
        *(buffer+i) = value; 
        /* cli_out("%s(): i=%d, value=0x%x, *(buffer+i)=0x%x\n", FUNCTION_NAME(), i, value, *(buffer+i));  */
    }

exit: 
  return ret; 
#endif /* __KERNEL__ */
  cli_out("This function is unavailable in Kernel mode\n");
  return -1;
} 
 
int   
  fe1600_card_i2c_write ( 
    uint8            unit, 
    uint8            internal_address, 
    uint8            buffer_size, 
    uint8           *buffer 
  )    
{
    int 
        ret = 0, 
        value = 0,
        i = 0; 

    for(i=0;i<buffer_size;i++) {
        value = (*(buffer + i)) & 0xff; 

        ret = cpu_i2c_write(unit, internal_address + i, CPU_I2C_ALEN_BYTE_DLEN_BYTE, value); 
        if (ret) { 
            cli_out("%s(): ERROR: cpu_i2c_write(). FAILED !!!\n", FUNCTION_NAME()); 
            ret = -1; 
            goto exit; 
        }
    }
 
exit: 
  return ret; 
} 
 
int  
  fe1600_card_i2c_syntmuxset ( 
    const FE1600_CARD_SYNT_TYPE     target 
  ) 
{ 
    int 
      ret = 0, 
      write_data; 
     
    switch (target)  
    { 
    case FE1600_CARD_SYNT_SERDES_0:  
      write_data = 0x04; 
      break; 
    case FE1600_CARD_SYNT_SERDES_1: 
      write_data = 0x02; 
      break; 
    case FE1600_CARD_SYNT_SERDES_2:  
      write_data = 0x01; 
      break; 
    case FE1600_CARD_SYNT_SERDES_3:  
      write_data = 0x08; 
      break; 
    case FE1600_CARD_SYNT_CORE:  
      write_data = 0x10; 
      break; 
    default :  
      write_data = 0x00; 
      break;                
    } 
 
    /* Write to the MUX */ 
    ret = cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, write_data); 
    if(ret) 
    { 
      cli_out("cpu_i2c_write to Synt MUX returned with %d\r\n", ret); 
    } 
 
    return ret; 
} 
 
uint32 
  fe1600_BOARD_SPECIFICATIONS_clear( 
    const SOC_BSP_CARDS_DEFINES  card_type 
  ) 
{ 
  Fe1600_card_board_info.card_type = card_type; 
 
  switch(card_type) 
  { 
  case FABRIC_CARD_FE1600_BCM88754:
  case FABRIC_CARD_FE1600: 
    cli_out("+. Fabric Card FE1600\n"); 
    Fe1600_card_board_info.agent_file = FE1600_CARD_IO_AGENT_FPGA_FILE; 
    Fe1600_card_board_info.io_agent_field_get_func = fe1600_card_fpga_io_32_fld_get; 
    Fe1600_card_board_info.io_agent_field_set_func = fe1600_card_fpga_io_32_fld_set; 
    Fe1600_card_board_info.i2c_read = fe1600_card_i2c_read; 
    Fe1600_card_board_info.i2c_write = fe1600_card_i2c_write; 
    Fe1600_card_board_info.i2c_syntmuxset = fe1600_card_i2c_syntmuxset; 
    break; 
  default: 
    cli_out("%s(): ERROR: UnKnown/Not supported card type=%d\n", FUNCTION_NAME(), card_type); 
    return 1; 
  } 
 
  return 0; 
}

uint32
    fe1600_card_init_i2c_devices(
        SOC_BSP_CARDS_DEFINES          card_type)
{

    unsigned
        err = 0;
        int i2c_chip_addr, i2c_data_addr, i2c_value;
        CPU_I2C_BUS_LEN i2c_alen;

    UTILS_FE1600_INIT_ERR_DEFS("fe1600_card_init_i2c_devices");

    cli_out("%s(): cofigure i2c devices\n", FUNCTION_NAME());

    /* set I2C devices: gpio */
    i2c_chip_addr = FE1600_CARD_I2C_GPIO_DEVICE_ADDR;
    i2c_alen      = CPU_I2C_ALEN_BYTE_DLEN_BYTE;
    
    i2c_data_addr = 0x3;
    i2c_value     = 0xbf;
    err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
    UTILS_FE1600_EXIT_IF_ERR(err, 90);
    sal_usleep(50000); 

    i2c_data_addr = 0x1;
    i2c_value     = 0x00;
    err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
    UTILS_FE1600_EXIT_IF_ERR(err, 90);
    sal_usleep(50000); 

exit:
  UTILS_FE1600_EXIT_AND_PRINT_ERR;
}

uint32
    fe1600_card_fpga_io_agent_start(
        SOC_BSP_CARDS_DEFINES          card_type)
{
    uint8
        err = 0;
    int i2c_chip_addr, i2c_data_addr, i2c_value;
    CPU_I2C_BUS_LEN i2c_alen;

    UTILS_FE1600_INIT_ERR_DEFS("fe1600_card_fpga_io_agent_start");

    /* io_agent reset */
    i2c_chip_addr = FE1600_CARD_I2C_GPIO_DEVICE_ADDR;
    i2c_alen      = CPU_I2C_ALEN_BYTE_DLEN_BYTE;
    i2c_data_addr = 0x01;
    i2c_value     = 0x40;
    err = cpu_i2c_write(i2c_chip_addr, i2c_data_addr, i2c_alen, i2c_value);
    UTILS_FE1600_EXIT_IF_ERR(err, 20);

  sal_msleep(50);

  if ((fe1600_card_fpga_io_fld_get(&Fe1600_card_fpga_io_regs.test.addr)) != 0x55)
  {
    cli_out("%s(): scratch_test_register != 0x55\n", FUNCTION_NAME());
    return 1;
  }
  else
  {
      cli_out("%s(): io_agent version=0x%x\n", FUNCTION_NAME(), fe1600_card_fpga_io_fld_get(&Fe1600_card_fpga_io_regs.version.addr));
  }
  
  /* take plx out of reset */
  /* fe1600_card_fpga_io_fld_set(&Fe1600_card_fpga_io_regs.reset.pcie_sw_rst_n, 0x1);  */
  /* fe1600_card_fpga_io_fld_set(&Fe1600_card_fpga_io_regs.reset.pcie_bridge_rst_n, 0x1);   */

exit:
  UTILS_FE1600_EXIT_AND_PRINT_ERR;
}

uint32
  fe1600_card_init_host_board(
    const  SOC_BSP_CARDS_DEFINES          card_type
  )
{
    int unit = 0; 


  if (fe1600_card_fpga_io_regs_init())
  {
    cli_out("%s(): ERROR: fe1600_card_fpga_io_regs_init(). FAILED !!!\n", FUNCTION_NAME());
    return 1;
  }

  if (fe1600_BOARD_SPECIFICATIONS_clear(card_type))
  {
    cli_out("%s(): ERROR: fe1600_BOARD_SPECIFICATIONS_clear(). FAILED !!!\n", FUNCTION_NAME());
    return 1;
  }

  if (fe1600_card_init_i2c_devices(card_type))
  {
    cli_out("%s(): ERROR: fe1600_card_init_i2c_devices(). FAILED !!!\n", FUNCTION_NAME());
    return 1;
  }

  /* burn io_agent */
  soc_dpp_fpga_load(unit, FE1600_CARD_IO_AGENT_FPGA_FILE);

  if (fe1600_card_fpga_io_agent_start(card_type)) 
  {
    cli_out("%s(): ERROR: fe1600_card_fpga_io_agent_start(). FAILED !!!\n", FUNCTION_NAME());
    return 1;
  }

  return 0;
}

/* 
 * Board Utility Functions 
 */
uint32 fe1600_card_power_monitor(int eq_sel)
{
#ifndef __KERNEL__
    unsigned
        err = 0;
    int i = 0, 
        adc_val_low = 0, 
        adc_val_high = 0,
        adc_code = 0,
        act_value = 0,
        power = 0,
        power_sum = 0;
    int act_value_arr[15];

    UTILS_FE1600_INIT_ERR_DEFS("fe1600_card_power_monitor");

    /* Set i2c mux */
    cli_out("%s(): Set i2c mux\n", FUNCTION_NAME());
    cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x40);
    
    for (i = 1; i <= FE1600_CARD_POWER_MONITOR_CH_NUM; i++) {
        cli_out("%s(): Setting %40s channel %02d (write 0x%02x in reg 0x17).", FUNCTION_NAME(), fe1600_card_power_monitor_string_arr[i], i, fe1600_card_power_monitor_arr[i]);
        err = cpu_i2c_write(0x17, 0x9, CPU_I2C_ALEN_BYTE_DLEN_BYTE, fe1600_card_power_monitor_arr[i]);
        UTILS_FE1600_EXIT_IF_ERR(err, 10);
        
        sal_usleep(1000);

        err = cpu_i2c_read(0x17, 0x7, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &adc_val_low);
        UTILS_FE1600_EXIT_IF_ERR(err, 20);
        err = cpu_i2c_read(0x17, 0x8, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &adc_val_high);
        UTILS_FE1600_EXIT_IF_ERR(err, 30);

        adc_code = (adc_val_high << 4) | ((adc_val_low & 0xf0) >> 4);
        act_value = adc_code * 2;
        act_value_arr[i] = act_value;
        /* cli_out("adc_val_high=0x%x, adc_val_low=0x%x, adc_code=0x%x, act_value=0x%x,%d\n", adc_val_high, adc_val_low, adc_code,act_value,act_value); */
        
        if (i == 5) {
            if (eq_sel == 0x2) {
                cli_out("Actual Current = %dmA\n", ((act_value * 1000) - (610 * 1000))/40);
            } else {
                cli_out("Actual Current = %dmA\n", ((act_value * 1000) - (2530 * 1000))/40);
            }
        } else if ((i == 6) || (i == 7)) {
            cli_out("Actual Current = %dmA\n", (act_value * 1000)/200);
        } else if (i == 8) {
            cli_out("Actual Current = %dmA\n", (act_value * 1000)/40);
        } else {
            cli_out("Actual Voltage = %dmV\n", act_value);        
        }
    }

    /* Power calc */
    if (eq_sel == 0x2) {
        power_sum += power = (act_value_arr[1] * (((act_value_arr[5]*1000) - (610*1000))/40))/1000;
    } else {
        power_sum += power = (act_value_arr[1] * (((act_value_arr[5]*1000) - (2530*1000))/40))/1000;
    }
    cli_out("%s(): Core Power = %dmW\n", FUNCTION_NAME(),power);
    power_sum += power = (act_value_arr[2] * ((act_value_arr[6]*1000)/200))/1000;
    cli_out("%s(): Pll Power = %dmW\n", FUNCTION_NAME(),power);
    power_sum += power = (act_value_arr[3] * ((act_value_arr[7]*1000)/200))/1000;
    cli_out("%s(): TVDD Power = %dmW\n", FUNCTION_NAME(),power);
    power_sum += power = (act_value_arr[4] * ((act_value_arr[8]*1000)/40))/1000;
    cli_out("%s(): SerDes Power = %dmW\n", FUNCTION_NAME(),power);

    cli_out("%s(): Total power = %dmW\n", FUNCTION_NAME(),power_sum);

exit:
  UTILS_FE1600_EXIT_AND_PRINT_ERR;
#endif /* __KERNEL__ */
  cli_out("This function is unavailable in Kernel mode\n");
  return 0xffffffff;
}

uint32 fe3200_card_power_monitor(int eq_sel)
{
#ifndef __KERNEL__
    int i = 0, 
        adc_val_low = 0, 
        adc_val_high = 0,
        adc_code = 0,
        act_value = 0;
    int act_value_arr[15];

    /* Set i2c mux */
    cli_out("%s(): Set i2c mux\n", FUNCTION_NAME());
    cpu_i2c_write(0x70, 0x0, CPU_I2C_ALEN_NONE_DLEN_BYTE, 0x40);
    
    for (i = 1; i <= FE3200_CARD_POWER_MONITOR_CH_NUM; i++) {
        cli_out("%s(): Setting %40s channel %02d (write 0x%02x in reg 0x17).", FUNCTION_NAME(), fe3200_card_power_monitor_string_arr[i], i, fe3200_card_power_monitor_arr[i]);
        cpu_i2c_write(0x17, 0x9, CPU_I2C_ALEN_BYTE_DLEN_BYTE, fe3200_card_power_monitor_arr[i]);
         
        sal_usleep(1000);

        cpu_i2c_read(0x17, 0x7, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &adc_val_low);
        cpu_i2c_read(0x17, 0x8, CPU_I2C_ALEN_BYTE_DLEN_BYTE, &adc_val_high);
  
        adc_code = (adc_val_high << 4) | ((adc_val_low & 0xf0) >> 4);
        act_value = adc_code * 2;
        act_value_arr[i] = act_value;
        cli_out("adc_val_high=0x%x, adc_val_low=0x%x, adc_code=0x%x, act_value=0x%x,%d\n", adc_val_high, adc_val_low, adc_code,act_value, act_value_arr[i]);
        
        if (i == 6) {
            cli_out("Actual Current = %dmA\n", ((act_value * 1000) - (610 * 1000))/40);
        } else if ((i == 7) || (i == 10)) {
            cli_out("Actual Current = %dmA\n", (act_value * 1000)/100);
        } else if (i == 8) {
            cli_out("Actual Current = %dmA\n", (act_value * 1000)/5);
        } else if (i == 9) {
            cli_out("Actual Current = %dmA\n", (act_value * 1000)/1);
        } else {
            cli_out("Actual Voltage = %dmV\n", act_value);        
        }
    }

    return 0x0;

#endif /* __KERNEL__ */
  cli_out("This function is unavailable in Kernel mode\n");
  return 0xffffffff;
}

/* 
 * Synt Functions 
 */

uint32 
  fe1600_card_board_synt_nominal_freq_get( 
    const FE1600_CARD_SYNT_TYPE   targetsynt 
    ) 
{ 
  uint32 nominalfreq = 0; 
 
  if (Fe1600_card_board_info.card_type == FABRIC_CARD_FE1600 || Fe1600_card_board_info.card_type == FABRIC_CARD_FE1600_BCM88754) 
  { 
    switch(targetsynt) 
    { 
      case FE1600_CARD_SYNT_SERDES_0: 
      case FE1600_CARD_SYNT_SERDES_1: 
      case FE1600_CARD_SYNT_SERDES_2: 
      case FE1600_CARD_SYNT_SERDES_3: 
      case FE1600_CARD_SYNT_CORE: 
        nominalfreq = 156250000; 
        break; 
      default: 
        break; 
    } 
  } 
 
  cli_out("%s(): targetsynt=%d, nominalfreq=%d\n", FUNCTION_NAME(), targetsynt, nominalfreq); 
   
  return nominalfreq; 
}

/*   
 *  Important note : the internal VCO of the synthesizer runs at a frequency in the range of 5GHz. 
 *  performing calculations in this range requires 33 bits of precision for representing frequencies in single Hz units. 
 *  for our purposes this precision is not required. 
 *  Given our CPU limitation of 32bit unsigned integers and no floating point support from standard library, 
 *  all frequencies used in this function are represented as 1/10 of the real freq. 
 *  Any way the output frequency will be achieved with better than 1ppm precision. 
 *  The nominal wake up frequency of 125MHz is going to be represented here as 12.5MHz 
 *  example : a target frequency of 156,250,000Hz would be calculated as if it was requested to be virtually 15,625,000Hz 
 *  based on the virtual 12,500,000Hz nominal frequency. 
 */ 
 
STATIC uint32 
  fe1600_card_board_synt_set_imp( 
   const FE1600_CARD_SYNT_TYPE   targetsynt, 
   const uint32           targetfreq, 
   const unsigned char          silent, 
   const uint8            synt_id, 
   const uint32           nominalfreq 
  ) 
{ 
  uint8  
    reg7, 
    reg8, 
    reg9, 
    reg10, 
    reg11, 
    reg12, 
    reg135, 
    reg137, 
    rawHS_DIV, 
    rawN1, 
    Ary_HS_DIV[12], 
    Ary_N1[129], 
    fFoundValues; 
  unsigned char  
    write_sucess; 
  uint32  
     HS_DIV, 
     N1, 
     RFREQ, 
     newRFREQ, 
     nominalfreqdiv10, 
     targetfreqdiv10, 
     fosc, 
     fxtal, 
     LowerFosc, 
     UpperFosc, 
     lowerRFREQ, 
     UpperRFREQ;        
  uint8  
    databuffer[6], 
    buffersize = 6; 
  uint64  
    div64; 
  uint64  
    result64;         
  unsigned  
    idx, 
    jdx; 
  uint32 
    max_loop_cnt = 100, 
    loop_cnt = 0; 
 
  UTILS_FE1600_INIT_ERR_DEFS("dfe_card_board_synt_set_imp"); 
 
  nominalfreqdiv10 = nominalfreq/10;         /* This is the real wake up frequency divided by 10 */ 
  targetfreqdiv10 = targetfreq / 10;   /* This is the real target frequency divided by 10 */ 
 
  do  
  { 
    Fe1600_card_board_info.i2c_read(synt_id,SYNTHESIZER_REG135,1,&reg135);   
 
    /* Reset synt, in order for fout to be nominal rate. */ 
    databuffer[0] = (reg135 | 0x1); 
    buffersize = 1; 
 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG135,buffersize,databuffer); 
    sal_msleep(100); 
 
    Fe1600_card_board_info.i2c_read(synt_id,SYNTHESIZER_REG137,1,&reg137);       
 
    Fe1600_card_board_info.i2c_read(synt_id,SYNTHESIZER_REG7TO12,6,databuffer);       
 
    reg7 = databuffer[0]; 
    reg8 = databuffer[1]; 
    reg9 = databuffer[2]; 
    reg10 = databuffer[3]; 
    reg11 = databuffer[4]; 
    reg12 = databuffer[5];        
    
    cli_out("%s(): reg7=0x%x, reg8=0x%x, reg9=0x%x, reg10=0x%x, reg11=0x%x, reg12=0x%x, \n", FUNCTION_NAME(), reg7, reg8, reg9, reg10, reg11, reg12);             
     
    /* extract the stored raw binary value of HS_DIV */ 
    rawHS_DIV = reg7 >> 5; 
    rawHS_DIV = rawHS_DIV & 0x07; 
 
    /* convert HS_DIV from raw binary to integer factor */ 
    switch (rawHS_DIV)  
    { 
     case 0 :  
       HS_DIV = 4; 
       break; 
     case 1 :  
       HS_DIV = 5; 
       break; 
     case 2 :  
       HS_DIV = 6; 
       break; 
     case 3 :  
       HS_DIV = 7; 
       break; 
     case 5 :  
       HS_DIV = 9; 
       break; 
     case 7 :  
       HS_DIV = 11; 
       break;  
     default: 
       return 1;  
    } 
 
    /*  
     * Extract the stored binary value of N1  
     */ 
    rawN1 = reg7 & 0x1F; 
    rawN1 = rawN1 << 2; 
    rawN1 = rawN1 | ((reg8 >> 6) & 0x03); 
 
    /*  
     * Convert N1 from synt binary to integer factor  
     */ 
    N1 = (long)rawN1 + 1; 
 
    /*  Extracting the 30 most significant bits of RFREQ from synthesizer registers 
     *  the 8 LSB bits of RFREQ (reg12) are not used here.  
     *  RFREQ is NOT going to be used as a real ("nn.nnnn") number here.  
     *  RFREQ will be used as a 30 bit binary divider. 
     */ 
    RFREQ =  (((uint32)reg8 & 0x3F) << 24); 
    RFREQ = (RFREQ | ((uint32)reg9 << 16)); 
    RFREQ = (RFREQ | ((uint32)reg10 << 8));    
    RFREQ = (RFREQ | (uint32)reg11);    
 
    /*    
     * RFREQ is now actualy a times 2^20 representation of the real REFREQ (the decimal point is between bit 19 and 20  
     * Calculate virtual internal crystal frequency (F_XTAL) from known nominal frequency (F_OUT) 
     * From device datasheet -->  Fxtal = (F_OUT * HS_DIV * N1) / RFREQ 
     */ 
    fosc = nominalfreqdiv10 * HS_DIV * N1; 
    /* create a 64bit integer by pushing fosc value 20 bits to the left */ 
    /* fosc must be shifted left the same number of bits that RFREQ is shifted left !! */    
    COMPILER_64_SET(result64, 0, fosc);
    COMPILER_64_SHL(result64, 20);
    COMPILER_64_SET(div64, 0, RFREQ);
    COMPILER_64_UDIV_64(result64, div64);
    fxtal = COMPILER_64_LO(result64); 
    if (!silent) {
        cli_out("Reading synthesizer registers.\n"); 
        cli_out("Calculated internal crystal frequency: %u0\n", fxtal); 
    }
 
    loop_cnt++; 
    if(loop_cnt > max_loop_cnt) 
    { 
        if (!silent) {
            cli_out("Synt set: Fxtal not in required interval after %u loops \n", max_loop_cnt); 
        }
      m_ret = 1; 
      goto exit; 
    } 
  } while(fxtal < 11400000 || fxtal > 11460000); 
 
  /*  
   * Set permitted values and ranges  
   */ 
  for (idx = 0; idx < 12; ++idx)  
  { 
    Ary_HS_DIV[idx] = 0xFF;  /* 0xFF will indicate unused entries in the array */ 
  } 
 
  Ary_HS_DIV[4]  = 0x00; 
  Ary_HS_DIV[5]  = 0x01; 
  Ary_HS_DIV[6]  = 0x02; 
  Ary_HS_DIV[7]  = 0x03; 
  Ary_HS_DIV[9]  = 0x05; 
  Ary_HS_DIV[11] = 0x07; 
  LowerFosc = 485000000;     /* thats 1/10 of the actual value */ 
  UpperFosc = 567000000;     /* thats 1/10 of the actual value */ 
  lowerRFREQ = 0x00000001;    
  UpperRFREQ = 0x3FFFFFFF; 
  for (idx=0;idx<129; ++idx)  
  { 
    Ary_N1[idx] = 0xFF;     /* 0xFF will indicate unused entries in the array */ 
  } 
 
  Ary_N1[1] = 0x00; 
  for (idx=2;idx <= 128; ++idx)  
  { 
    if ((idx % 2) == 0)  
    { 
      Ary_N1[idx] = (idx - 1); 
    } 
  } 
 
  /* Calculate new values for requested frequency 
    From device datasheet  --> Fosc = Fout * HS_DIV * N1 
                           --> Fosc = Fxtal * RFREQ 
                           --> RFREQ = Fosc / Fxtal 
    Should find a combination of highest HS_DIV value with lowest N1 value 
    Fosc must be in the range of 4.85GHz to 5.67GHz 
    Actual (real) RFREQ must be greater than 1/(2^28) and less than 1024 (which is between 1 to 0x3FFFFFFF in the way it is used here). 
  */ 
 
  fFoundValues = 0; 
  newRFREQ = RFREQ; 
 
  for (idx=12;idx>0;idx--) if (Ary_HS_DIV[idx-1] != 0xFF)  
  { 
    HS_DIV = idx-1; 
    for (jdx=0;jdx<=128;jdx++) if (Ary_N1[jdx] != 0xFF)  
    { 
      N1 = jdx; 
      fosc = targetfreqdiv10 * HS_DIV * N1; 
      if (fosc > UpperFosc) 
      { 
        break; 
      } 
 
      if ((fosc > LowerFosc) && (fosc < UpperFosc)) { 
        /* create a 64bit integer by pushing fosc value 20 bits to the left */ 
        /* fosc must be shifted left the same number of bits that RFREQ is shifted left !! */         
        COMPILER_64_SET(result64, 0, fosc);
        COMPILER_64_SHL(result64, 20);
        COMPILER_64_SET(div64, 0, fxtal);
        COMPILER_64_UDIV_64(result64, div64);
        newRFREQ = COMPILER_64_LO(result64); 
        if ((newRFREQ >= lowerRFREQ) && (newRFREQ <= UpperRFREQ))  
        { 
          fFoundValues = 1; 
          break; 
        } 
      } 
    } 
    if (fFoundValues)  
    { 
      break; 
    } 
  } 
 
  if (!silent) {
      cli_out("RFREQ X 2^20: %u \n", newRFREQ); 
      cli_out("Internal Oscillator / 10: %u \n", fosc); 
      cli_out("HS_DIV: %u \n", HS_DIV); 
      cli_out("N1: %u \n", N1); 
  }
 
  /* Build target registers with new values */ 
  reg7 = (Ary_HS_DIV[HS_DIV] << 5) | ((Ary_N1[N1] >> 2) & 0x1F); 
  reg8 = ((Ary_N1[N1] << 6) & 0xC0) | (uint8)((newRFREQ >> 24) & 0x3F); 
  reg9 = (uint8)((newRFREQ >> 16) & 0xFF); 
  reg10 = (uint8)((newRFREQ >> 8) & 0xFF);  
  reg11 = (uint8)(newRFREQ & 0xFF);  
  reg12 = 0x00; 
 
  loop_cnt = 0; 
  do  
  { 
    /* Freeze the DCO before modification of parameters */ 
 
    reg137 = reg137 | 0x10; 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG137,1,&reg137); 
 
    /* modify synthesizer values to generate new frequency */ 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+0,1,&reg7); 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+1,1,&reg8); 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+2,1,&reg9); 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+3,1,&reg10); 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+4,1,&reg11); 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG7TO12+5,1,&reg12); 
 
    /* Unfreeze the DCO and assert the NewFreq bit */ 
    /* Data sheet requires maximum 10ms from unfreezing the DCO to asserting the NewFreq bit */ 
 
    reg137 = reg137 & 0xEF; 
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG137,1,&reg137); 
 
    reg135 = reg135 | 0x40;  
    Fe1600_card_board_info.i2c_write(synt_id,SYNTHESIZER_REG135,1,&reg135); 
 
    /* Synthesizer should run new frequency now. */ 
    Fe1600_card_board_info.i2c_read(synt_id,SYNTHESIZER_REG7TO12,6,databuffer);       
     
    write_sucess = ((reg7  == databuffer[0]) && 
                    (reg8  == databuffer[1]) && 
                    (reg9  == databuffer[2]) && 
                    (reg10 == databuffer[3]) && 
                    (reg11 == databuffer[4]) && 
                    (reg12 == databuffer[5])); 
 
    loop_cnt++; 
    if(loop_cnt > max_loop_cnt) 
    { 
      cli_out("Synt set: write still did not succeeded after %u loops \n", max_loop_cnt); 
      m_ret = 1; 
      goto exit; 
    } 
  }while(!write_sucess); 
exit: 
  UTILS_FE1600_EXIT_AND_PRINT_ERR; 
} 
 
uint32 
  fe1600_card_board_synt_set( 
   const FE1600_CARD_SYNT_TYPE targetsynt, 
   const uint32                targetfreq, 
   const unsigned char         silent 
  ) 
{ 
   uint8 synt_id = 0; 
   uint32 nominalfreq = 0; 
 
   nominalfreq = fe1600_card_board_synt_nominal_freq_get(targetsynt); 
 
   if (Fe1600_card_board_info.card_type == FABRIC_CARD_FE1600 || Fe1600_card_board_info.card_type == FABRIC_CARD_FE1600_BCM88754) 
   { 
     switch(targetsynt) 
     { 
 
     case FE1600_CARD_SYNT_SERDES_0: 
     case FE1600_CARD_SYNT_SERDES_1: 
     case FE1600_CARD_SYNT_SERDES_2: 
     case FE1600_CARD_SYNT_SERDES_3: 
     case FE1600_CARD_SYNT_CORE: 
       synt_id = 0x55; 
       break; 
     default: 
       break; 
     }       
   } 
 
   Fe1600_card_board_info.i2c_syntmuxset(targetsynt); 
 
   cli_out("%s(): targetsynt=%d, targetfreq=%d, synt_id=0x%x, nominalfreq=%d\n", FUNCTION_NAME(), targetsynt, targetfreq, synt_id, nominalfreq); 
 
   fe1600_card_board_synt_set_imp(targetsynt, targetfreq, FALSE, synt_id, nominalfreq); 
 
   return 0; 
}

uint32 fe3200_card_board_synt_set(uint32 targetfreq, uint32 nominalfreq)
{ 
   uint8 synt_id = 0x55;
   const FE1600_CARD_SYNT_TYPE targetsynt = FE1600_CARD_NOF_SYNT_TYPES;

   Fe1600_card_board_info.i2c_read = fe1600_card_i2c_read; 
   Fe1600_card_board_info.i2c_write = fe1600_card_i2c_write; 
    
   cli_out("%s(): targetfreq=%d, synt_id=0x%x, nominalfreq=%d\n", FUNCTION_NAME(),  targetfreq, synt_id, nominalfreq); 
 
   fe1600_card_board_synt_set_imp(targetsynt, targetfreq, FALSE, synt_id, nominalfreq); 
 
   return 0; 
}
#else
typedef int _prevent_compilation_warning;
#endif /* (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(__DUNE_SLK_BCM_CPU__)) && defined(BCM_DFE_SUPPORT) */
