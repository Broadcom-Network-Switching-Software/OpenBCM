/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#include "chip_sim.h"
#include "chip_sim_mem.h"
#include "chip_sim_cell.h"
#include "chip_sim_task.h"
#include "chip_sim_log.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
#include "chip_sim_pkt.h"
#include "chip_sim_FE200.h"
#include "chip_sim_FAP10M.h"
#include "chip_sim_FAP20V.h"
#include "chip_sim_T20E.h"
#if LINK_TIMNA_LIBRARIES
  #include "chip_sim_TIMNA.h"
#endif
#if LINK_PSS_LIBRARIES
  #include "chip_sim_FAP20M.h"
#endif
#if LINK_FAP20M_LIBRARIES
  #include <soc/dpp/SOC_SAND_FAP10M/fap10m_chip_defines.h>
#endif
#if LINK_FE200_LIBRARIES
  #include <soc/dpp/SOC_SAND_FE200/fe200_chip_defines.h>
#endif
#if LINK_TIMNA_LIBRARIES
  #include <soc/dpp/Timna/timna_chip_defines.h>
#endif
#if LINK_FAP20V_LIBRARIES
/* { */
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_chip_defines.h>
/* } */
#endif /* LINK_FAP20V_LIBRARIES */
#if LINK_FAP21V_LIBRARIES
/* { */
#include "chip_sim_FAP21V.h"
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_regs.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_tbls.h>
#include <soc/dpp/SOC_SAND_FAP21V/fap21v_chip_defines.h>
/* } */
#endif /* LINK_FAP21V_LIBRARIES */
#if LINK_FAP20M_LIBRARIES
/* { */
#include <soc/dpp/SOC_SAND_FAP20M/fap20m_chip_defines.h>
/* } */
#endif /* LINK_FAP20M_LIBRARIES */

#if LINK_FE600_LIBRARIES
	#include "chip_sim_FE600.h"
	#include <soc/dpp/SOC_SAND_FE600/fe600_chip_regs.h>
	#include <soc/dpp/SOC_SAND_FE600/fe600_chip_tbls.h>
	#include <soc/dpp/SOC_SAND_FE600/fe600_chip_defines.h>
#endif

#if LINK_T20E_LIBRARIES
#include "chip_sim_T20E.h"
#include <soc/dpp/T20E/t20e_chip_regs.h>
#include <soc/dpp/T20E/t20e_chip_tbls.h>
#include <soc/dpp/T20E/t20e_chip_defines.h>

#include <soc/dpp/OAM/oam_chip_regs.h>
#include <soc/dpp/OAM/oam_chip_tbls.h>

/* } */
#endif /* LINK_T20E_LIBRARIES */

#include <string.h>



/*
 * 0 prevent checking if the accesses address is Read/Write/Ac access right.
 * 1 check if the accesses address is Read/Write/Ac access right.
 */
#define CHIP_SIM_CHECK_X_ACCESS 0

typedef enum
{
  REMOTE_MEM_DB_TIMING_SB_REGISTER=0,
  REMOTE_MEM_DB_TIMING_3_REGISTER,
  REMOTE_MEM_DB_MODE_REGISTER,
  REMOTE_MEM_DB_EXTENDED_MODE_REGISTER,
  REMOTE_MEM_DB_CONFIGURATION_REGISTER,
  REMOTE_MEM_DB_TIMING_1_REGISTER,
  REMOTE_MEM_DB_TIMING_2_REGISTER,
  REMOTE_MEM_DB_OPERATION_REGISTER,
  REMOTE_MEM_DB_SDL_SRAM_PROG_COUNTER,
  REMOTE_MEM_DB_DFCDL_DATA_REG,

  REMOTE_MEM_BUFF_MODE_REGISTER,

  REMOTE_MEM_FA_GLOBAL_CONTROL_REGISTER,
  REMOTE_MEM_FA_GLOBAL_STATUS_REGISTER,

  REMOTE_MEM_UPLINK_CONFIGURATION_REGISTER,
  REMOTE_MEM_WIDE_C2C_CONTROL_REGISTER,
  REMOTE_MEM_NARROW_C2C_CONTROL_REGISTER,
  REMOTE_MEM_METAL_FIX_REGISTER,

  REMOTE_MEM_UPLINK_SDL_SRAM_PROG_COUNTER,
  REMOTE_MEM_UPLINK_DFCDL_DATA_REG,
  REMOTE_MEM_UPLINK_MISC_INT_CAUSE_REG_ADDR,

  CHIP_SIM_REMOTE_MEM_LAST
}CHIP_SIM_REMOTE_MEM;

typedef struct
{
  UINT32 addr[CHIP_SIM_REMOTE_MEM_LAST];
}CHIP_SIM_REMOTE_ADDR;

CHIP_SIM_REMOTE_ADDR Chip_sim_remote_addr;

void
  chip_sim_init_remote_addr(
    void
  )
{
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_TIMING_SB_REGISTER    ] = REMOTE_MEM_DB_TIMING_SB_REGISTER      ;
#ifdef LINK_PSS_LIBRARIES
 /* { */
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_TIMING_3_REGISTER     ] = FAP10M_DB_TIMING_3_REGISTER      ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_MODE_REGISTER         ] = FAP10M_DB_MODE_REGISTER          ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_EXTENDED_MODE_REGISTER] = FAP10M_DB_EXTENDED_MODE_REGISTER ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_CONFIGURATION_REGISTER] = FAP10M_DB_CONFIGURATION_REGISTER ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_TIMING_1_REGISTER     ] = FAP10M_DB_TIMING_1_REGISTER      ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_TIMING_2_REGISTER     ] = FAP10M_DB_TIMING_2_REGISTER      ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_OPERATION_REGISTER    ] = FAP10M_DB_OPERATION_REGISTER     ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_SDL_SRAM_PROG_COUNTER] = FAP10M_DB_SDL_SRAM_PROG_COUNTER ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_DB_DFCDL_DATA_REG      ] = FAP10M_DB_DFCDL_DATA_REG       ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_BUFF_MODE_REGISTER ] = FAP10M_BUFF_MODE_REGISTER  ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_FA_GLOBAL_CONTROL_REGISTER   ] = FAP10M_FA_GLOBAL_CONTROL_REGISTER    ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_FA_GLOBAL_STATUS_REGISTER  ] = FAP10M_FA_GLOBAL_STATUS_REGISTER   ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_UPLINK_CONFIGURATION_REGISTER] = FAP10M_UPLINK_CONFIGURATION_REGISTER ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_WIDE_C2C_CONTROL_REGISTER    ] = FAP10M_WIDE_C2C_CONTROL_REGISTER     ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_NARROW_C2C_CONTROL_REGISTER  ] = FAP10M_NARROW_C2C_CONTROL_REGISTER   ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_METAL_FIX_REGISTER      ] = FAP10M_METAL_FIX_REGISTER       ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_UPLINK_SDL_SRAM_PROG_COUNTER] = FAP10M_UPLINK_SDL_SRAM_PROG_COUNTER ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_UPLINK_DFCDL_DATA_REG       ] = FAP10M_UPLINK_DFCDL_DATA_REG        ;
  Chip_sim_remote_addr.addr[REMOTE_MEM_UPLINK_MISC_INT_CAUSE_REG_ADDR] = FAP10M_UPLINK_MISC_INT_CAUSE_REG_ADDR ;
/* } LINK_PSS_LIBRARIES */
#endif
}

struct
{
  /*
   * The base memory of each chip. Real mem stay here.
   */
  UINT32* base[CHIP_SIM_NOF_CHIPS] ;
  /*
   * The size of address in bytes
   */
  UINT32  size ;

  /*
   * The size of core addresss in bytes
   */
  UINT32  core_size ;

  /*
   * The start address on the BSP
   */
  UINT32  start_address[CHIP_SIM_NOF_CHIPS] ;

  /*
   * Registers descriptors as in the driver.
   */
  const DESC_FE_REG *desc;

} Chip_mem;


/*****************************************************
*NAME
*  chip_sim_mem_init
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:40:21
*FUNCTION:
* Reset 'Chip_mem'.
* INPUT:
*  SOC_SAND_DIRECT:
*    (1) void
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    void
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
void
  chip_sim_mem_init(
    void
    )
{
  int
    i;
  chip_sim_cell_init();
  chip_sim_cntr_init();
  chip_sim_interrupt_init();
  chip_sim_indirect_init();
  chip_sim_pkt_init();
  chip_sim_init_remote_addr();
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    Chip_mem.base[i] = NULL ;
    Chip_mem.start_address[i] = 0 ;
  }
  Chip_mem.size = 0 ;
  Chip_mem.core_size = Chip_mem.size;
  Chip_mem.desc = NULL ;


}

/*****************************************************
*NAME
*  chip_sim_mem_init_chip_specifics
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 15:41:19
*FUNCTION:
*  Init chips specifics.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  SOC_SAND_CHIP_TYPE chip_type
*    (2) SOC_SAND_IN  unsigned int   chip_ver
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_mem_init_chip_specifics(
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
    SOC_SAND_IN unsigned int     chip_ver,
    SOC_SAND_IN DESC_FE_REG    *desc
    )
{
  STATUS
    status = OK ;

  if (NULL == desc)
  {
    status = ERROR ;
    goto exit;
  }

  switch (chip_type)
  {
  #if LINK_FE600_LIBRARIES
	  /* { */
	  case SOC_SAND_DEV_FE600:
	    fe600_regs_init();
	    fe600_tbls_init();
	    fe600_indirect_init();
	
	    Chip_mem.size = FE600_TOTAL_SIZE_OF_REGS * sizeof(uint32);
	    Chip_mem.core_size = Chip_mem.size;
	
	    Chip_mem.desc = desc;
	    chip_sim_cntr_init_chip_specifics(Fe600_counters);
	
	    chip_sim_interrupt_init_chip_specifics(
	      Fe600_interrupts,
	      INVALID_ADDRESS, /* SOC_SAND_FE600 do not general interrupt bit */
	      0,
	      INVALID_ADDRESS, /* SOC_SAND_FE600 do not general mask bit */
	      0);
	
	    chip_sim_indirect_init_chip_specifics(Fe600_indirect_blocks);
	
	    chip_sim_cell_init_chip_specifics(1,
	      INVALID_ADDRESS  /*  SOC_SAND_OFFSETOF(FAP20V_REGS,fct_regs.detail.tx_cell_trig) */
	      );
	
	    chip_sim_cell_rx_init_chip_specifics(
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      0,
	      0,
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      INVALID_ADDRESS,
	      0,
	      0
	      );
	
	    chip_sim_pkt_init_chip_specifics(INVALID_ADDRESS);
	    break;
	/* } LINK_FE600_LIBRARIES */
  #endif
	
  #if LINK_FE200_LIBRARIES
  /* { */
  case SOC_SAND_DEV_FE200:
    Chip_mem.size = sizeof(FE_REGS) ;
    Chip_mem.core_size = Chip_mem.size;
    Chip_mem.desc = desc ;
    chip_sim_cntr_init_chip_specifics(Fe_counters);
    chip_sim_interrupt_init_chip_specifics(Fe_interrupts,
                                           0x00a0,
                                           0,
                                           0x10a0,
                                           0);
    chip_sim_indirect_init_chip_specifics(Fe_indirect_blocks
                                          );

    chip_sim_cell_init_chip_specifics(2, SOC_SAND_OFFSETOF(FE_REGS,fe_gen_regs.fe_gen_detail.tx_data_cell_trigger));
    chip_sim_cell_rx_init_chip_specifics(SOC_SAND_OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.rx_sr_data_cell),
                                         SOC_SAND_OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.rx_sr_data_cell),
                                         SOC_SAND_OFFSETOF(FE_REGS,fe_dcs_dcha_regs.fe_dcs_dcha_detail.sr_data_cell_fifo_ind_1),
                                         SOC_SAND_OFFSETOF(FE_REGS,fe_dcs_dchb_regs.fe_dcs_dchb_detail.sr_data_cell_fifo_ind_2),
                                         SRFNE_1_SHIFT,
                                         SRFNE_2_SHIFT,
                                         INVALID_ADDRESS,INVALID_ADDRESS,INVALID_ADDRESS,
                                         INVALID_ADDRESS,0,0
                                         );

    chip_sim_pkt_init_chip_specifics(INVALID_ADDRESS);

    break;
/* } LINK_FE200_LIBRARIES */
#endif
#if LINK_TIMNA_LIBRARIES
  case SOC_SAND_DEV_TIMNA:
    Chip_mem.core_size = sizeof(TIMNA_REGISTERS) ;/*Bigger than the actual size*/
    Chip_mem.size = Chip_mem.core_size + sizeof(unsigned long) * CHIP_SIM_REMOTE_MEM_LAST;
    Chip_mem.desc = desc ;
    chip_sim_cntr_init_chip_specifics(Timna_counters);

    chip_sim_interrupt_init_chip_specifics(Timna_interrupts,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP10M do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP10M do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(
      Timna_indirect_blocks
    );
    break;
#endif
#if LINK_FAP10M_LIBRARIES
/* { */
  case SOC_SAND_DEV_FAP10M:
    Chip_mem.core_size = sizeof(FAP10M_REGS) ;
    Chip_mem.size = Chip_mem.core_size + sizeof(unsigned long) * CHIP_SIM_REMOTE_MEM_LAST;
    Chip_mem.desc = desc ;
    chip_sim_cntr_init_chip_specifics(Fap10m_counters);

    chip_sim_interrupt_init_chip_specifics(Fap10m_interrupts,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP10M do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP10M do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(
      Fap10m_indirect_blocks
    );

    chip_sim_cell_init_chip_specifics(
      1,
      SOC_SAND_OFFSETOF(FAP10M_REGS,fdt_fct_cmd_regs.detail.tx_cell_trig)
    );
    chip_sim_cell_rx_init_chip_specifics(
      SOC_SAND_OFFSETOF(FAP10M_REGS,fdr_status_regs.detail.recv_sr_data_cell),
      INVALID_ADDRESS,
      SOC_SAND_OFFSETOF(FAP10M_REGS,fdr_status_regs.detail.overflow_and_fifo_statuses),
      INVALID_ADDRESS,
      FAP10M_CPU_DATA_CELL_FNE_SHIFT,
      0,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0
    );

    chip_sim_pkt_init_chip_specifics(
      SOC_SAND_OFFSETOF(FAP10M_REGS,tx_pkt_mem.detail.tx_pkt_trig)
    );
    break;
    /* } LINK_FAP10M_LIBRARIES */
#endif
#if LINK_FAP20V_LIBRARIES
/* { */
  case SOC_SAND_DEV_FAP20V:
    Chip_mem.size = sizeof(FAP20V_REGS) ;
    Chip_mem.core_size = Chip_mem.size;

    Chip_mem.desc = desc ;
    chip_sim_cntr_init_chip_specifics(Fap20v_counters);

    chip_sim_interrupt_init_chip_specifics(Fap20v_interrupts,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP20V do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP20V do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(
      (chip_ver == FAP20V_EXPECTED_CHIP_VER_01) ? Fap20v_indirect_blocks : Fap20v_b_indirect_blocks
    );

    chip_sim_cell_init_chip_specifics(
      1,
      SOC_SAND_OFFSETOF(FAP20V_REGS,fct_regs.detail.tx_cell_trig)
    );
    chip_sim_cell_rx_init_chip_specifics(
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0
    );

    chip_sim_pkt_init_chip_specifics(
      (chip_ver == FAP20V_EXPECTED_CHIP_VER_01) ?
      SOC_SAND_OFFSETOF(FAP20V_REGS,egq_regs.detail.cpu_pkt_ctrl) :
      SOC_SAND_OFFSETOF(FAP20V_REGS,egq_regs.detail_b.cpu_pkt_ctrl)
    );
    break;
/* } */
#endif /* LINK_FAP20V_LIBRARIES */
#if LINK_FAP20M_LIBRARIES
/* { */
  case SOC_SAND_DEV_FAP20M:
    Chip_mem.size = sizeof(FAP20M_REGS) ;
    Chip_mem.core_size = Chip_mem.size;

    Chip_mem.desc = desc ;
    chip_sim_cntr_init_chip_specifics(Fap20m_counters);

    chip_sim_interrupt_init_chip_specifics(Fap20m_interrupts,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP20M do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP20M do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(
       Fap20m_indirect_blocks
    );

    chip_sim_cell_init_chip_specifics(
      1,
      SOC_SAND_OFFSETOF(FAP20M_REGS,fct_regs.detail.tx_cell_trig)
    );
    chip_sim_cell_rx_init_chip_specifics(
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0
    );

    chip_sim_pkt_init_chip_specifics(
      INVALID_ADDRESS
    );
    break;
/* } */
#endif /* LINK_FAP20M_LIBRARIES */
#if LINK_FAP21V_LIBRARIES
  case SOC_SAND_DEV_FAP21V:

    fap21v_regs_module_initialize();
    fap21v_tbls_module_initialize();
    fap21v_indirect_init();

    Chip_mem.size = FAP21V_TOTAL_SIZE_OF_REGS * sizeof(uint32);
    Chip_mem.core_size = Chip_mem.size;

    Chip_mem.desc = desc;
    chip_sim_cntr_init_chip_specifics(Fap21v_counters);

    chip_sim_interrupt_init_chip_specifics(Fap21v_interrupts,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP21V do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* SOC_SAND_FAP21V do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(Fap21v_indirect_blocks);

    chip_sim_cell_init_chip_specifics(1,
                                      0x182c  /*  SOC_SAND_OFFSETOF(FAP20V_REGS,fct_regs.detail.tx_cell_trig) */
                                     );

    chip_sim_cell_rx_init_chip_specifics(
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0
    );

    chip_sim_pkt_init_chip_specifics(0x5700);
    break;
/* } */
#endif /* LINK_FAP21V_LIBRARIES */
#if LINK_T20E_LIBRARIES
  case SOC_SAND_DEV_T20E:

    t20e_regs_init();
    oam_regs_init(0x200);
    t20e_tbls_init();
    oam_tbls_init();

    t20e_indirect_init();

    Chip_mem.size = T20E_TOTAL_SIZE_OF_REGS * sizeof(uint32);
    Chip_mem.core_size = Chip_mem.size;

    Chip_mem.desc = desc;
    chip_sim_cntr_init_chip_specifics(T20e_counters);

    chip_sim_interrupt_init_chip_specifics(T20e_interrupts,
                                           INVALID_ADDRESS, /* T20E do not general interrupt bit */
                                           0,
                                           INVALID_ADDRESS, /* T20E do not general mask bit */
                                           0);

    chip_sim_indirect_init_chip_specifics(T20e_indirect_blocks);

    chip_sim_cell_init_chip_specifics(1,
                                      0x182c  /*  SOC_SAND_OFFSETOF(FAP20V_REGS,fct_regs.detail.tx_cell_trig) */
                                     );

    chip_sim_cell_rx_init_chip_specifics(
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      INVALID_ADDRESS,
      0,
      0
    );

    /* chip_sim_pkt_init_chip_specifics(SOC_SAND_OFFSETOF(FAP20V_REGS,egq_regs.detail.cpu_pkt_ctrl)); */
    break;
/* } */
#endif /* LINK_T20E_LIBRARIES */

    default:
    Chip_mem.size = 0 ;
    Chip_mem.core_size = Chip_mem.size;

    status = ERROR ;
  }

exit:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_malloc
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:38:28
*FUNCTION:
*  Allocate memory for 'Chip_mem'
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type
*    (2) SOC_SAND_IN unsigned int     chip_ver
*    (3) SOC_SAND_IN UINT32 start_address[CHIP_SIM_NOF_CHIPS]
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_mem_malloc(
    SOC_SAND_IN SOC_SAND_DEVICE_TYPE chip_type,
    SOC_SAND_IN unsigned int     chip_ver,
    SOC_SAND_IN DESC_FE_REG*     desc,
    SOC_SAND_IN UINT32           start_address[CHIP_SIM_NOF_CHIPS]
    )
{
  STATUS
    status = OK ;
  int
    i ;


  if (chip_sim_mem_init_chip_specifics(chip_type, chip_ver, desc))
  {
    status = ERROR ;
    goto chip_sim_mem_malloc_exit ;
  }

  /* indirect */
  if (chip_sim_indirect_malloc())
  {
    status = ERROR ;
    chip_sim_mem_free();
    goto chip_sim_mem_malloc_exit ;
  }


  /* counters */
  if (chip_sim_cntr_malloc())
  {
    status = ERROR ;
    chip_sim_mem_free();
    goto chip_sim_mem_malloc_exit ;
  }

  /* interrupts */
  if (chip_sim_interrupt_malloc())
  {
    status = ERROR ;
    chip_sim_mem_free();
    goto chip_sim_mem_malloc_exit ;
  }
  /* em for Soc_petra-B */
  if (chip_type == SOC_SAND_DEV_PB)
  {
  }
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    Chip_mem.base[i]          = INT_TO_PTR(start_address[i]) ;
    Chip_mem.start_address[i] = start_address[i] ;
  }

  if (chip_sim_log_malloc(10000))
  {
    status = ERROR ;
    chip_sim_mem_free();
    goto chip_sim_mem_malloc_exit ;
  }

  chip_sim_cell_malloc();

chip_sim_mem_malloc_exit:
  return status ;
}

/*****************************************************
*NAME
*   chip_sim_mem_free
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:43:17
*FUNCTION:
*  Free memory for 'Chip_mem'
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) void

*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS

*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_mem_free(
    void
    )
{
  STATUS
    status = OK ;


  chip_sim_indirect_free();
  chip_sim_cntr_free();
  chip_sim_interrupt_free();
  chip_sim_mem_init() ;
  chip_sim_log_free() ;
  chip_sim_cell_free() ;

  return status ;
}


/*****************************************************
*NAME
*   chip_sim_mem_address_to_offset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:43:42
*FUNCTION:
*  Convert from general/CPU address space
*   to a specific chip offset
* 1. Go over all simulated chips
* 2. Assuming the address space is continues one
* 3. Calculate the offset in the chip
* 4. Check with in chip address space size.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  UINT32 address
*    (2) SOC_SAND_OUT UINT32 *chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_address_to_offset(
    SOC_SAND_IN  UINT32  address,
    SOC_SAND_OUT UINT32* chip_offset,
    SOC_SAND_OUT int*    chip_ndx
    )
{
  STATUS
    status = ERROR ;
  UINT32
    tmp ;
  int
    i,
    addr_i;

  /* 1 */
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    /* 2 */
    if (address >= Chip_mem.start_address[i])
    {
      /* 3 */
      tmp = address-Chip_mem.start_address[i] ;
      /* 4 */
      if (tmp < Chip_mem.core_size)
      {
        *chip_offset = tmp ;
        *chip_ndx    = i ;
        status = OK ;
        goto chip_sim_mem_get_chip_offset_exit ;
      }
    }
  }
  for (i=0; i<CHIP_SIM_NOF_CHIPS; i++)
  {
    if(status != OK)
    {
      for(addr_i=0;addr_i<CHIP_SIM_REMOTE_MEM_LAST;addr_i++)
      {
        tmp = address-Chip_mem.start_address[i];

        if(tmp == Chip_sim_remote_addr.addr[addr_i])
        {
          *chip_offset = Chip_mem.core_size + addr_i * sizeof(long);
          *chip_ndx    = i ;
          status = OK ;
          goto chip_sim_mem_get_chip_offset_exit ;
        }
      }
    }
  }

chip_sim_mem_get_chip_offset_exit:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_chip_offset_check
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:44:21
*FUNCTION:
* 1. Check if valid chip index
* 2. Check if valid chip offset
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_chip_offset_check(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 chip_offset
    )
{
  STATUS
    status = OK ;

  /* 1 */
  if (chip_ndx<0 || chip_ndx>=CHIP_SIM_NOF_CHIPS)
  {
    status = ERROR ;
    goto chip_sim_mem_chip_offset_check_exit ;
  }
  /* 2 */
  if (chip_offset>=Chip_mem.size)
  {
    status = ERROR ;
    goto chip_sim_mem_chip_offset_check_exit ;
  }

chip_sim_mem_chip_offset_check_exit:
  return status ;
}


/*****************************************************
*NAME
*  chip_sim_mem_write
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:10
*FUNCTION:
*  Write the value to the chip.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*    (3) SOC_SAND_IN UINT32 value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_write(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 chip_offset,
    SOC_SAND_IN UINT32 value)
{
  STATUS
    status = OK ;
  UINT32
    long_offset ;

  if (chip_sim_mem_chip_offset_check(chip_ndx, chip_offset))
  {
    status = ERROR ;
    goto chip_sim_mem_write_exit ;
  }

  long_offset = chip_offset>>2 ;
  *(Chip_mem.base[chip_ndx]+long_offset) = value ;
  if (chip_sim_log_get_spy_low_mem())
  {
    char
      msg[100];
    sal_sprintf(msg, "w chip_ndx(%d) time(%lu) offset(0x%X) val(0x%X)\r\n",
            chip_ndx, (long unsigned)chip_sim_task_get_mili_time(), chip_offset, value);
    chip_sim_log_run(msg);
  }

chip_sim_mem_write_exit:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_read
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:38
*FUNCTION:
*  Read from the chip
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN  int    chip_ndx
*    (2) SOC_SAND_IN  UINT32 chip_offset
*    (3) SOC_SAND_OUT UINT32 *value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_read(
    SOC_SAND_IN  int    chip_ndx,
    SOC_SAND_IN  UINT32 chip_offset,
    SOC_SAND_OUT UINT32 *value)
{
  STATUS
    status = OK ;
  UINT32
    long_offset ;

  if (chip_sim_mem_chip_offset_check(chip_ndx, chip_offset))
  {
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }

  long_offset = chip_offset>>2 ;
  *value = *(Chip_mem.base[chip_ndx]+long_offset);

  if (chip_sim_log_get_spy_low_mem())
  {
    char
      msg[100];
    sal_sprintf(msg, "r chip_ndx(%d) time(%lu) offset(0x%X) val(0x%X)\r\n",
            chip_ndx, (long unsigned)chip_sim_task_get_mili_time(), chip_offset, *value);
    chip_sim_log_run(msg);
  }

FUNC_EXIT_POINT:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_write_field
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:38
*FUNCTION:
*  Write only the filled
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*    (3) SOC_SAND_IN UINT32 value
*    (4) SOC_SAND_IN UINT32 shift
*    (5) SOC_SAND_IN UINT32 mask
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_write_field(
    SOC_SAND_IN int    chip_ndx,
    SOC_SAND_IN UINT32 chip_offset,
    SOC_SAND_IN UINT32 value,
    SOC_SAND_IN UINT32 shift,
    SOC_SAND_IN UINT32 mask
    )
{
  STATUS
    status = OK ;
  UINT32
    reg_val ;

  if (chip_sim_mem_read(chip_ndx, chip_offset, &reg_val))
  {
    status = ERROR ;
   GOTO_FUNC_EXIT_POINT ;
  }

  reg_val &= ~mask ;
  reg_val |= SOC_SAND_SET_FLD_IN_PLACE(value, shift, mask);

  if (chip_sim_mem_write(chip_ndx, chip_offset, reg_val))
  {
    status = ERROR ;
   GOTO_FUNC_EXIT_POINT ;
  }


FUNC_EXIT_POINT:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_read_field
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:38
*FUNCTION:
*  Write only the filled
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int    chip_ndx
*    (2) SOC_SAND_IN UINT32 chip_offset
*    (3) SOC_SAND_OUT UINT32 *value
*    (4) SOC_SAND_IN UINT32 shift
*    (5) SOC_SAND_IN UINT32 mask
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_read_field(
    SOC_SAND_IN  int    chip_ndx,
    SOC_SAND_IN  UINT32 chip_offset,
    SOC_SAND_OUT UINT32 *value,
    SOC_SAND_IN  UINT32 shift,
    SOC_SAND_IN  UINT32 mask
    )
{
  STATUS
    status = OK ;
  UINT32
    reg_val ;

  if (chip_sim_mem_read(chip_ndx, chip_offset, &reg_val))
  {
    status = ERROR ;
   GOTO_FUNC_EXIT_POINT ;
  }

  reg_val = SOC_SAND_GET_FLD_FROM_PLACE(reg_val, shift, mask);
  *value = reg_val ;


FUNC_EXIT_POINT:
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_get_DESC_SAND_REG_from_chip_offset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* 1. find the memory in the address space of the chip.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     DESC_SAND_REG*  -- NULL in case not found
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  Maybe move this function to the searching accessories of DESC_SAND_REG.
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
const
  DESC_FE_REG*
    chip_sim_mem_get_DESC_SAND_REG_from_chip_offset(
      SOC_SAND_IN UINT32 chip_offset
      )
{
  const DESC_FE_REG
    *desc = NULL ;
  ELEMENT_OF_GROUP
    *element_p=NULL;
  UINT32
    start_group_offset,
    end_group_offset;

  /* 1 */
  for (desc=(&Chip_mem.desc[0]); desc->field_id!=FE_LAST_FIELD; desc++)
  {
    if (desc->reg_base_offset == chip_offset)
    {
      GOTO_FUNC_EXIT_POINT ;
    }

    /* search in the group */
    if (desc->element_of_group != NULL)
    {
      for (element_p=desc->element_of_group; element_p->num_repetitions!=0; element_p++ )
      {
        start_group_offset = element_p->byte_offset ;
        end_group_offset   = start_group_offset + sizeof(unsigned long)*(element_p->num_repetitions);
        if (chip_offset>=start_group_offset &&
            chip_offset<end_group_offset)
        {
          GOTO_FUNC_EXIT_POINT ;
        }
      }
    }
  }
  if (desc->field_id == FE_LAST_FIELD)
  {
    desc = NULL ;
  }

FUNC_EXIT_POINT:
  return desc ;
}

/*****************************************************
*NAME
*  chip_sim_mem_check_is_X_access
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* Is chip_offset as X_access defined.
* (1) find the memory in the address space of the chip.
* (2) case not found return ERROR.
* (3) case found :: check if X_access defined.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32        chip_offset
*    (2) SOC_SAND_IN unsigned char X_access
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     STATUS  ERROR - not defined
*             OK    - X_access is defined
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  Maybe move this function to the searching accessories of DESC_SAND_REG.
*     Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_check_is_X_access(
    SOC_SAND_IN UINT32 chip_offset,
    SOC_SAND_IN UINT32 X_access
    )
{
  STATUS
    status = OK ;

#if CHIP_SIM_CHECK_X_ACCESS
  /*
   * {
   */
  const
    DESC_FE_REG
      *desc = NULL ;

  /* (1) */
  desc = chip_sim_mem_get_DESC_SAND_REG_from_chip_offset(chip_offset) ;

  /* (2) */
  if (desc == NULL)
  {
    /*
     * status = ERROR ;
     * we assume that if register do not exist in the reg file,
     * it is because we didn't enter it there - not that it
     * doesn't exist - therefore the check return true.
     */
    GOTO_FUNC_EXIT_POINT;
  }

  /* (3) */
  if ( ((desc->access_type)&X_access) != X_access)
  {
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT ;
  }
  /*
   * }
   */
FUNC_EXIT_POINT:
#endif /* CHIP_SIM_CHECK_X_ACCESS */

  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_check_is_write_access
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* Is writable space
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_check_is_write_access(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  STATUS
    status = OK ;

  status = chip_sim_mem_check_is_X_access(chip_offset, ACCESS_WRITE ) ;

  return status ;
}
/*****************************************************
*NAME
*  chip_sim_mem_check_is_read_access
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* Is readable space -- in principal every place is readable, but
*  in that way we check that this address has a real meaning.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*     STATUS
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
* Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_check_is_read_access(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  STATUS
    status = OK ;

  status = chip_sim_mem_check_is_X_access(chip_offset, ACCESS_READ ) ;

  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_check_is_clear_access
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* Is clearable space .
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*SEE ALSO:
 */
STATUS
  chip_sim_mem_check_is_clear_access(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  STATUS
    status = OK ;

#if CHIP_SIM_CHECK_X_ACCESS
  status = chip_sim_mem_check_is_X_access(chip_offset, ACCESS_CLEAR ) ;
#else
    status = ERROR;
#endif
  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_check_is_indirect_access
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
* Is indirect space
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN UINT32 chip_offset
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*       Under Interrupt Halting No OS Calls
*SEE ALSO:
 */
STATUS
  chip_sim_mem_check_is_indirect_access(
    SOC_SAND_IN UINT32 chip_offset
    )
{
  STATUS
    status = OK ;

  status = chip_sim_mem_check_is_X_access(chip_offset, ACCESS_INDIRECT ) ;

  return status ;
}

/*****************************************************
*NAME
*  chip_sim_mem_reset
*Programmer name  : Zadok Shay
*TYPE: PROC
*DATE: 17-Sep-02 14:56:57
*FUNCTION:
*  Resets to defaults.
*INPUT:
*  SOC_SAND_DIRECT:
*    (1) SOC_SAND_IN int i
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
 */
STATUS
  chip_sim_mem_reset(
    SOC_SAND_IN int chip_ndx
    )
{
  STATUS
    status = OK ;

  if (chip_ndx<0 || chip_ndx>=CHIP_SIM_NOF_CHIPS)
  {
    /*
     * Invalid index
     */
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT;
  }

  if (NULL == Chip_mem.base[chip_ndx])
  {
    /*
     * Base was not allocated
     */
    status = ERROR ;
    GOTO_FUNC_EXIT_POINT;
  }

#if 0
  {
    /*
     * Setting every register with different number -- debugging aid.
     * problem-- because parts of data get copied also (most of the time).
     * If enabled: Clear all triggers.
     */
    unsigned long  filler_i;
    unsigned long* reg_i;
    unsigned long  index_i;

    filler_i = 0xCD000000;
    reg_i = Chip_mem.base[chip_ndx];
    for (index_i=0; index_i< Chip_mem.size/4; index_i++)
    {
      *reg_i = filler_i;
      reg_i ++;
      filler_i ++;
    }
  }
#else
  memset(Chip_mem.base[chip_ndx], 0x0, Chip_mem.size) ;
#endif

FUNC_EXIT_POINT:
  return status ;
}
