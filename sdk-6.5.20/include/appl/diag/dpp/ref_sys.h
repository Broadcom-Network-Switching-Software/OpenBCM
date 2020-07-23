
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_REF_SYS_INCLUDED
/* { */
#define SOC_REF_SYS_INCLUDED
/*
 * Set 'USE_DUNE_REFERENCE_PLATFORM' to a value of 'FALSE' to
 * include definitions specific to packages (such as
 * fe200_atm package) which are independent packages and are
 * portable to other environments and operating systems.
 * When defined to a non-zero value, it indicates that the package
 * is being tested under Dune's reference system.
 */
#define USE_DUNE_REFERENCE_PLATFORM  1
#if USE_DUNE_REFERENCE_PLATFORM
/* { */
/*
 * Set 'SAND_LOW_LEVEL_SIMULATION' to a value of 'TRUE' to run the program
 * under visual studio. Se to 'FALSE' on final version.
 */
/* } */
#else
/* { */
#include "fe200_atm_sys.h"
/* } */
#endif

#if !DUNE_BCM
/* { */
#define STATUS int
#define FALSE 0
#define TRUE  1
#if  !defined(ERROR)
  #define ERROR TRUE
#endif

#if  !defined(OK)
  #define OK    0
#endif


/* } */
#endif

#if !DUNE_BCM
#else
  #define CRATE_TEST_EVAL               (0)  /*Negev-Rev-A*/
  #define CRATE_32_PORT_40G_STANDALONE  (1)  /*Gobi: SOC_SAND_FAP20V*/
  #define CRATE_32_PORT_40G_IO          (2)
  #define CRATE_FABRIC_BOX              (3)
  #define CRATE_64_PORT_10G_STANDALONE  (4)  /*Gobi: SOC_SAND_FAP10M*/
  #define CRATE_32_PORT_10G_IO          (5)
  #define CRATE_NEGEV_REV_B             (6)
#endif

/*
 * Definitions related to debugging
 * {
 */
/*
 * Set true (non-zero) for demo (snake et al.) ONLY!!!.
 * Set to zero in standard versions.
 */
#define CODE_FOR_DEMO_ONLY  1
/*
 * Set true (non-zero) for debug stage ONLY!!!. Produces debug code additions.
 */
#define DEBUG_AID           1
/*
 * Set true (non-zero) for debug stage ONLY!!!.
 * Produces debug code additions for 'ui memory' module.
 */
/*
 * Set true (non-zero) for debug stage ONLY!!!.
 * This will define PCI interrupt to be an interrupt which
 * may be controlled (i.e. injected) by SW (for debug purposes)
 */
/*
 * Set true (non-zero) for debug stage ONLY!!!.
 * Produces debug code and simulation related to DUNE chip set module.
 * There are 2 simultors :SIMULATE_DUNE_CHIP and SIMULATE_CHIP_SIM/
 *  SIMULATE_CHIP_SIM -- related to ChipSim lib.
 */
#define SIMULATE_DUNE_CHIP   0
#define SIMULATE_CHIP_SIM    0
#if SIMULATE_CHIP_SIM
/* { */
/*
 * Flag: SIMULATE FEs at initialization.
 */
/*
 * Flag: SIMULATE FAPs at initialization.
 */
/* } */
#endif
#if (SIMULATE_CHIP_SIM) & (SIMULATE_DUNE_CHIP)
#error "Only one is allowed active (SIMULATE_CHIP_SIM) & (SIMULATE_DUNE_CHIP)"
#endif

/*
 * }
 */
/*
 * Object: Buff_mem
 * This object enables the user to store 'mem read' results
 * in local memory to later be retriebed. This feature enables
 * faster working with an external script agent (like proComm).
 * Storage is always in units of char (one byte).
 * {
 */
/*
 * Size of local storage of results.
 */
#define SIZEOF_BUFF_MEM 4000
/*
 * }
 */
/*
 * Base memory address and size of SDRAM.
 * Update with final address when final board is
 * ready.
 */
#define SDRAM_BASE_ADDR 0x00000000
#define SDRAM_SIZE      0x08000000  /*Udic*/
/*
 * Base memory address and size of E2PROM.
 * Update with final address when final board is
 * ready.
 */
#define E2PROM_BASE_ADDR 0x20000000
#define E2PROM_SIZE      0x2000
/*
 * Base memory address and size of EPLD.
 * Also very basic definitions related to version and
 * board identity (to be used at early boot stages).
 * Update with final address when final board is
 * ready.
 */
#define EPLD_BASE_ADDR         0x30000000
#define EPLD_SIZE              0x10
#define NEW_MEZZANINE_WITH_PCI 0x00000020
/*
 * Base memory address and size of GP1.
 * Update with final address when final board is
 * ready.
 */
#define GP1_BASE_ADDR 0x40000000
/*
 * Base memory address of FE.
 * Update with final address when final board is
 * ready.
 */
#if (SIMULATE_DUNE_CHIP || SIMULATE_CHIP_SIM)
/* { */
#define FE_1_BASE_ADDR (&Fe_regs[0])
#define FE_2_BASE_ADDR (&Fe_regs[1])

#define FAP10M_1_BASE_ADDR (&Fap10m_regs[0])
#define FAP10M_2_BASE_ADDR (&Fap10m_regs[1])
/* } */
#else
/* { */
#define FE_1_BASE_ADDR      (GP1_BASE_ADDR)
#define FE_2_BASE_ADDR      (GP1_BASE_ADDR | 0x100000)

#define FAP10M_1_BASE_ADDR  (GP1_BASE_ADDR)
#define FAP10M_2_BASE_ADDR  (GP1_BASE_ADDR | 0x00100000)

/* } */
#endif

/*
 * Number of FE chips on this board.
 */
#define NUM_FE_ON_BOARD 2
/*
 * Number of FE chips on this board.
 */
/*
 * Base memory address and size of GP2.
 * Update with final address when final board is
 * ready.
 */
#define GP2_BASE_ADDR 0x50000000
#define GP2_SIZE      0x800000
/*
 * Base memory address and size of GP3.
 * Update with final address when final board is
 * ready.
 */
#define GP3_BASE_ADDR 0x70000000
#define GP3_SIZE      0x400000
/*
 * Definitions added for new mezzanine card
 * {
 */
/*
 * Base memory address and size of SDRAM handled by GT64131.
 */
#define GT64131_SDRAM_BASE_ADDR 0x10000000
#define GT64131_SDRAM_SIZE      0x02000000
/*
 * Base memory address and size of registers of GT64131.
 */
#define GT64131_REGS_BASE_ADDR  0x14000000
#define GT64131_REGS_SIZE       0x00001000
/*
 * Base memory address and size of PCI I/O.
 */
#define PCI_IO_BASE_ADDR        0x18000000
#define PCI_IO_REGS_SIZE        0x08000000
/*
 * Base memory address and size of PCI memory. Bank #1
 * Prepare place of 8K bytes. Avoid exaggeration
 * here since this increases initialization time
 * at startup (vmGlobalMapInit() in usrMmuInit())
 */
#define PCI_MEM1_BASE_ADDR      0x70000000
#define PCI_MEM1_REGS_SIZE      0x00002000
/*
 * Base memory address and size of PCI memory. Bank #2
 * Prepare place of 128M bytes. Avoid exaggeration
 * here since this increases initialization time
 * at startup (vmGlobalMapInit() in usrMmuInit())
 */
#define PCI_MEM2_BASE_ADDR      0x80000000
#define PCI_MEM2_REGS_SIZE      0x08000000
/*
 * }
 */


/*
 * Set true for debug stage. Produces debug printing.
 */
#define DEBUG_PRINT                   TRUE
/*
 * Set true for telnet debug stage. Produces debug printing.
 */
#define TELNET_DEBUG_PRINT            FALSE
/*
 * Set true to display current line again after error during line mode.
 */
#define SHOW_PROMPT_LINE_AFTER_ERROR_DURING_LINE_MODE   FALSE

#ifndef _ASMLANGUAGE
/* { */
/*
 * Pointer to a procedure that returns int
 */
typedef int (*PROC_PTR)(void) ;
/*
 * Pointer to a procedure that returns int and gets unsigned long
 */
typedef int (*PROC_PTR_LONG)(unsigned long) ;
/* } */
#endif
/*
 * Definitions which are specific for reference system.
 * {
 */

/*
 * Software signals.
 *  The FEC driver uses one signal
 *  The FAP10 driver uses one signal
 */

/*
 * Number of sectors, starting from '0', assigned to
 * downloading of user program.
 */
/*
 * If include_enhanced_cli is defined then extra features, such as
 * 'parameter string with default does not require a value' are added.
 */
#define INCLUDE_ENHANCED_CLI FALSE
#ifndef _ASMLANGUAGE
/* { */
/*
 * prototypes
 */
/*
 * {
 */
void
  progStop(
    void
  ) ;
/*
 * }
 */

/*
 * 0 -- No calls to SOC_SAND_FAP20V driver
 *      So it shell not be included in the linking.
 * 1 -- Call to SOC_SAND_FAP20V driver
 * {
 */

#ifndef FAP20M_STANDALONE_PACKAGE
/*#define LINK_FAP20V_LIBRARIES 1*/
#endif
/*
 * }
 */

/* } */
#endif
/*
 * }
 */
/* } */
#endif


