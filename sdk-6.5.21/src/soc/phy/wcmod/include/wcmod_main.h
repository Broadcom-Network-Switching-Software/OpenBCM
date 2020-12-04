/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <sal/types.h>
#include <soc/phy.h>
/* FILE: wcmod_main.h */
/* DATE: Mon Jun 13 10:28:56 PDT 2011 */


#ifndef _WCMOD_MAIN_H_
#define _WCMOD_MAIN_H_

#if defined (_DV_TRIUMPH3) || defined (_DV_ODYSSEY) || defined  (_DV_NATIVE) || defined (_DV_REDSTONE) || defined (VERILOG)
#else
#define _SDK_WCMOD_
#endif

#ifndef STATIC
#define STATIC static
#endif  /* STATIC */

#ifndef __KERNEL__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "errno.h"
#endif
#include "wcmod_defines.h"

#ifndef __KERNEL__
#if defined (_DV_TRIUMPH3) || defined (_DV_ODYSSEY) || defined (_DV_NATIVE) || defined (_DV_REDSTONE)
  #define printf io_printf
#endif

#ifdef CADENCE
#include <veriuser.h>
#endif /* CADENCE */

#ifdef SYNOPSYS
#include <vcsuser.h>
#endif /* SYNOPSYS */

#endif

#ifdef _SDK_WCMOD_
#include <sal/types.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/util.h>
#include <soc/cmic.h>
#include <soc/phyreg.h>
#include <shared/phyreg.h>
#endif

#ifdef _SDK_WCMOD_
  #define _SAL_TYPES_H
  #define printf bsl_printf
#endif

#ifndef  _SDK_WCMOD_
typedef unsigned char           uint8;          /* 8-bit quantity  */
typedef unsigned short          uint16;         /* 16-bit quantity */
typedef unsigned int            uint32;         /* 32-bit quantity */

/* from "sal/core/time.h" */
typedef uint32 sal_usecs_t;

/* from "soc/types.h" */
typedef int     soc_port_t;

/* dev 2 is almost always the testbench. */
#define SOC_MAX_NUM_DEVICES         2

/* there are up to 72 ports, max needed by trident. */
/* Most other platforms use 1, 2,  */
/* Hence a second variable called NUM_PORTS which is the actual number of */
/* ports. We set it to max for now. Not a big deal in terms of memory */

#define SOC_MAX_NUM_PORTS       72 
#define NUM_PORTS               SOC_MAX_NUM_PORTS
#endif /*#ifndef _SDK_WCMOD_ */

#ifdef  _SDK_WCMOD_
#define SOC_E_NONE            0
#define SOC_E_MULT_REG        1    /* this means no errors */
#define SOC_E_FUNC_FOUND      2    /* function found. Nothing more to do. */
#define SOC_E_FUNC_NOT_FOUND  3    /* function not found. Continue search */ 
#define SOC_E_ERROR SOC_E_INTERNAL
#endif

#define INT_PHY_SW_STATE(unit, port)  (int_phy_ctrl[unit][port])

/* these are taken from the original wc40.c */
#define WC40_REG_READ(_unit, _pc, _flags, _reg_addr, _val) \
        wcmod_reg_aer_read((_unit), (_pc),  (_reg_addr), (_val))

#define WC40_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
        wcmod_reg_aer_write((_unit), (_pc),  (_reg_addr), (_val))

#define WC40_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
        wcmod_reg_aer_modify((_unit), (_pc),  (_reg_addr), (_val),(_mask))

#define WC40_UDELAY(_time) sal_udelay(_time)

#define PRINTF_ERROR(fmt)    printf fmt /* from bcmdrv/systemInit.h */

#ifndef _SDK_WCMOD_
#define SOC_IF_ERROR_RETURN(op) \
    do { int __rv__; if ((__rv__ = (op)) < 0) \
            { PRINTF_ERROR((__FILE__ " : " #op " %d\n", __LINE__)); \
                    return(__rv__);} } \
                        while(0)
#endif   /*_SDK_WCMOD_ */

#define CHK_RET_VAL_FUNC(op) \
    { int rv; if ((rv=(op)) != 0) \
        { printf("ERROR %s:%d. #op# returned %d\n", __FILE__,__LINE__,rv); \
          return (rv);} }

#define WC_AER_BCST_OFS_STRAP   0x1ff
#define PHYCTRL_MDIO_ADDR_SHARE (1 << 0) /*use phy base addr. to access lanes */
#define PHYCTRL_MDIO_CL45       (1 << 1) /*use MDIO CL45 to access device */
#define PHY_AER_REG_ADDR_AER(_addr)    (((_addr) >> 16) & 0x0000FFFF)
#define PHY_AER_REG_ADDR_BLK(_addr)    (((_addr) & 0x000FFF0))
#define PHY_AER_REG_ADDR_REGAD(_addr)  ((((_addr) & 0x00008000) >> 11) | \
                                        ((_addr) & 0x0000000F))

#define PHY_AER_REG        0xFFDE
#define PHY_AER_REG_ADDR_CL45_DEVID(_addr) (((_addr) >> 27) & 0x0000001F)
#define PHY_AER_REG_ADDR_LANE_NUM(_addr)   (((_addr) >> 16) & 0x07FF)
#define PHY_AER_REG_ADDR_CL45_REGAD(_addr) (((_addr) & 0x000FFFF))

#ifndef _SDK_WCMOD_
#define DEV_CFG_PTR(_pc) (&(((WC40_DEV_DESC_t *)((_pc)->driver_data))->cfg))
#define SOC_PHY_CLAUSE45_ADDR(_devad,_regad) \
  			   ((((_devad) & 0x003F) << 16) | ((_regad) & 0xFFFF))
#endif

#ifndef _MDK_WCMOD_
  #ifndef _SDK_WCMOD_
     #define FALSE 0
     #define TRUE  1
  #endif   /*_SDK_WCMOD_ */
#else
int getLane0Phyad(int phyad);
#endif /* _MDK_WCMOD_ */

/* Borrowed from SDK.  This struct controls iterations and time durations
 * while executing a 'wait' or a 'poll'. No doxygen documentation for this.
*/
#ifndef _SDK_WCMOD_
typedef struct soc_timeout_s
{
    sal_usecs_t		expire;
    sal_usecs_t		usec;
    int			min_polls;
    int			polls;
    int			exp_delay;
} soc_timeout_t;
#endif  /* !_SDK_WCMOD_ */

#define WC40_XGXSBLK0_XGXSSTATUSr  0x8001
#define WC40_PLL_WAIT              2500

/*define the constant for bypass pma/pmd speed enforce */
#define PMA_PMD_SPEED_ENFORCE  0x1234

#define NUMBER_PCS_FUNCTION_TABLE_ENTRIES 100

/* This struct is a building block of a string associated function table.
 * It is used to override native PCS Tier1 functions. No doxygen documentation
 * for this.
 */
typedef struct strToPCSFunction {
  char* p;
  int (*fp)(wcmod_st*);
} str2PCSFn;

#include "wcmod.h"
#ifndef _SDK_WCMOD_
#include "wcmod_platform_defines.h"     
#include "wcmod_phyreg.h"    /* for WCMod only. */
#endif

extern str2PCSFn str2PCSFunc;

extern void              wcmod_init_called(int x);
extern void              wcmod_init(void);
extern int               wcmod_tier1_selector(char* s,wcmod_st* c, int *retVal);
extern void              wcmod_read_cfg(const char* fileName);
extern void              print_config(int unit, int port);
extern int               searchPCSFuncTable(char* selStr, wcmod_st* ws);
extern void              copy_wcmod_st(wcmod_st *from, wcmod_st *to);
extern int               isLaneEnabled(wcmod_lane_select ls, int lane, wcmod_st *ws);
extern wcmod_model_type  getGenericModelType(wcmod_model_type model);
extern const char*       getLaneStr(wcmod_lane_select ls);
extern wcmod_spd_intfc_set wcmod_spd_intf_s2e(char* s);

extern int wcmod_pcs_bypass_ctl(wcmod_st *pc);
extern int wcmod_master_slave_control(wcmod_st *pc);
extern int wcmod_slave_wakeup_control(wcmod_st *pc);
extern int wcmod_100fx_control(wcmod_st *pc);
extern int wcmod_tx_lane_disable(wcmod_st *pc);
extern int wcmod_power_control(wcmod_st *pc);
extern int wcmod_autoneg_set(wcmod_st *pc);
extern int wcmod_autoneg_page_set(wcmod_st *pc);
extern int wcmod_autoneg_control(wcmod_st *pc);
extern int wcmod_carrier_extension_set(wcmod_st *pc);
extern int wcmod_reg_read(wcmod_st *pc);
extern int wcmod_reg_write(wcmod_st *pc);
extern int wcmod_prbs_check(wcmod_st *pc);
extern int wcmod_cjpat_crpat_control(wcmod_st *pc);
extern int wcmod_cjpat_crpat_check(wcmod_st *pc);
extern int wcmod_diag(wcmod_st *pc);
extern int wcmod_lane_swap(wcmod_st *pc);
extern int wcmod_parallel_detect_control(wcmod_st *pc);
extern int wcmod_clause72_control(wcmod_st *pc);
extern int wcmod_pll_sequencer_control(wcmod_st *pc);
extern int wcmod_prog_data(wcmod_st *pc);
extern int wcmod_duplex_control(wcmod_st *pc);
extern int wcmod_revid_read(wcmod_st *pc);
extern int wcmod_rx_seq_control(wcmod_st *pc);
extern int wcmod_tx_rx_polarity(wcmod_st *pc);
extern int wcmod_set_port_mode(wcmod_st *pc);
extern int wcmod_prbs_control(wcmod_st *pc);
extern int wcmod_prbs_decouple_control(wcmod_st *pc);
extern int wcmod_scrambler_control(wcmod_st *pc);
extern int wcmod_rx_DFE_tap1_control(wcmod_st *pc);
extern int wcmod_rx_DFE_tap2_control(wcmod_st *pc);
extern int wcmod_rx_DFE_tap3_control(wcmod_st *pc);
extern int wcmod_rx_DFE_tap4_control(wcmod_st *pc);
extern int wcmod_rx_DFE_tap5_control(wcmod_st *pc);
extern int wcmod_rx_PF_control(wcmod_st *pc);
extern int wcmod_rx_low_freq_PF_control(wcmod_st *pc);
extern int wcmod_rx_VGA_control(wcmod_st *pc);
extern int wcmod_soft_reset(wcmod_st *pc);
extern int wcmod_tx_reset(wcmod_st *pc);
extern int wcmod_rx_reset(wcmod_st *pc);
extern int wcmod_aer_lane_select(wcmod_st *pc);
extern int wcmod_speed_intf_set(wcmod_st *pc);
extern int wcmod_speed_intf_control(wcmod_st *pc);
extern int wcmod_tx_bert_control(wcmod_st *pc);
extern int wcmod_rx_loopback_control(wcmod_st *pc);
extern int wcmod_rx_loopback_pcs_bypass(wcmod_st *pc);
extern int wcmod_tx_tap_control(wcmod_st *pc);
extern int wcmod_tx_pi_control(wcmod_st *pc);
extern int wcmod_tx_loopback_control(wcmod_st *pc);
extern int wcmod_port_stat_display(wcmod_st *pc);
extern int wcmod_core_reset(wcmod_st *pc);
extern int wcmod_tx_amp_control(wcmod_st *pc);
extern int wcmod_rx_p1_slicer_control(wcmod_st *pc);
extern int wcmod_rx_m1_slicer_control(wcmod_st *pc);
extern int wcmod_rx_d_slicer_control(wcmod_st *pc);
extern int wcmod_refclk_set(wcmod_st *pc);
extern int wcmod_asymmetric_mode_set(wcmod_st *pc);
extern int _wcmod_getRevDetails(wcmod_st* pc);
extern int _wcmod_lane_enable(wcmod_st* pc);
extern int _phy_wcmod_ind_init_common(wcmod_st *st, int fw_load_method);
extern int _phy_wcmod_independent_lane_init(wcmod_st *ws);
extern int _phy_wcmod_control_prbs_polynomial_set(wcmod_st *ws, uint32 value);
extern int _phy_wcmod_control_prbs_tx_invert_data_set(wcmod_st *ws, uint32 value);
extern int _phy_wcmod_control_prbs_enable_set(wcmod_st *ws, uint32 enable);
extern int _phy_wcmod_control_prbs_polynomial_get(wcmod_st *ws, uint32 *value);
extern int _phy_wcmod_control_prbs_tx_invert_data_get(wcmod_st *ws, uint32 *value);
extern int _phy_wcmod_control_prbs_enable_get(wcmod_st *ws, uint32 *value);
extern int _phy_wcmod_control_prbs_rx_status_get(wcmod_st *ws, uint32 *value);
extern int wcmod_firmware_mode_set(wcmod_st *pc);
extern int wcmod_EEE_pass_thru_set(wcmod_st *pc);
extern int wcmod_uc_status_dump(int unit, soc_port_t port, soc_phy_wcmod_uc_desc_t* pDesc /* the size of array should be MAX_NUM_LANES, parameter is NULL display the data using bsl_printf() */); 


/*!
\mainpage  WCMOD Documentation (Beta)

This is the documentation site for WCMod (<b>W</b>arpcore <b>C</b>onfiguration <b>MOD</b>ule)

\version 0.2

\author WCMod Team

\date 06/25/10

<B>Introduction</B><br> 

Warpcore Configuration Module (WCMod)  is a source-code/binary distributable 
multi tiered library that defines a hardware abstraction layer for various
configuration modes of the broadcom Warpcore serdes IP. This document expects
the reader to be familiar with the Warpcore architecture.

The tiered structure progressively encapsulates register accesses and sequence
hiding complexities like indirect addressing (Address extension registers or
AER) and access protocol (MDIO CL22/CL45) implementation. The Tier1 layer,
also called WCMod Procedures are building blocks to construct comprehensive
warpcore Tier2 layers also called WCMod sequences.

Tier1 procedures are fixed sequences of register accesses, accomplishing a
specific sub-configuration.  In general they are mode aware (combo or
independent) and only affect independent ports.

The full list of Tier1 procedures are tabulated below.

WCMod architecture documentation is created using doxygen and so follows the
code structure. Please use the following guidelines to use the documentation.

The documentation is stored in tabs. If you are reading this you are in the
main tab. (See the top left corner of our browser). The other tabs you will
see are 'classes' where the #wcmod_st struct is visible and 'files', where following files are visible.

\li wcmod_cfg_seq.c
\li wcmod_configuration_selector.c
\li wcmod_enum_defines.c
\li wcmod_main.c
\li wcmod_phyreg.c

\li wcmod_configuration_selector.h
\li wcmod_defines.h
\li wcmod_enum_defines.h
\li wcmod_main.h
\li wcmod_phyreg.h
\li wcmod_platform_defines.h

All Tier1 procedures have a fixed signature. They return an error code and
accept a reference to a C struct (called #wcmod_st). #wcmod_st is used to pass
input parameters to the Tier1 procedures and occassionally stores a return
value from the Tier1 procedure. #wcmod_st remembers the context of one warpcore
(not one port or one lane). If the system has multiple warpcores, the software
platform must create and maintain that many #wcmod_st structures.

Warpcores can be programmed per lane or per port (independent lane mode or
combo mode). Platforms control the lanes on which Tier1 procedures operate by
setting the #wcmod_st.lane_select element. The lane specific information is set
in the #wcmod_st.per_lane_control element which has fields dedicated to each of
the 4 lanes. In combo mode, the lane_select element should be set to zero and
any inputs to the Tier1 functions should be set in the lane 0 fields.

Tier1 procedures have no programmatic order dependencies. This implies that the
effect of a Tier1 is constant regardless of the state of the warpcore.

The exceptions are 
\li #wcmod_set_port_mode function must be called before others.
\li Order mandated by Warpcore architecture.
\li Aggregate registers which control features of multiple lanes in warpcore.

<B>WCMod interface to System Platforms</B><br> 
     
WCMod has an unambiguous interface mechanism to system platforms. Platforms can
be software systems like MDK, and SDK or customer specific software. It can
also be validation platforms including HDL (verilog/systemVerilog/Vera) based
testbenches.

WCMod functions (tier1 and tier2) operates on a single warpcore at a time. It
can be used to access multiple lanes within the warpcore, but cannot access
multiple warpcores simultaneously. To access different warpcores multiple calls
to tier1 procedures are needed.

WCMod defines a structure (called wcmod_st) which encapsulates all information
of a warpcore needed by WCMod. This structure should be created and maintained
by system platforms, one per warpcore.  Typically an array of wcmod_st structs
is created, accessible by the warpcore's unique physical address.

All Tier1 procedures are called with a reference to a wcmod_st struct. of the
warpcore being accessed. WCMod will read and write information to the struct.
as needed. Adopting platforms should treat this struct as read only and should
not modify it.

Software platforms can avail a void pointer #wcmod_st.platform_info to store
arbitrary platform specific information. Platforms will manage memory of any
associated structure. In diagnostic mode, WCMod cannot print any information
about to the void pointer related structure.

WCMod drivers that access registers require system platforms to attach local
drivers to WCMod at compile time.  Typically the drivers allow read and write to
registers. Any platform specific information needed by the drivers outside the
elements of #wcmod_st can passed to the drivers via #wcmod_st.platform_info.
WCMod will provide register address offsets, data and masks. 

Warpcore lanes can architecturally be accessed in two ways. 
\li 1.If the 'prt_ad' straps of  the warpcore are strapped to a specific
value, say 'lane_add' and the multiport_en strap is enabled, lanes are accessed
as lane_add, lane_add+1, lane_add+2 and lane_add+3. This  method is not
currently supported by WCMod.
\li 2.Lanes can be accessed by using the AER offset method. This is the
preferred method and is currently supported by WCMod.

Register access within lanes have the following pattern.

\li In independent mode tier1 procedures can be called for multiple lanes
lanes simultaneously. The lanes are chosen using #wcmod_st.lane_select
\li In dual xgxs mode two lanes form a logical port. So Tier1 procedures are
called for each dual port. Individual lanes are still accessible.
\li In combo mode, Tier1 procedures are called for a warpcore. Register access
for individual lanes are still available via #wcmod_st.lane_select.

<B>Input/Output for Tier1 procedures</B><br> 

As previously mentioned the only input to all tier1 procedures is #wcmod_st.
All Tier1 procedures return a status value. Certain procedures, such as
'read_rev_id' which have to return a value modify a general purpose element
#wcmod_st.accData.  Platforms that don't use returned information can ignore
this element. Others can pick up the information from #wcmod_st::accData

<B>WCMod Register Access Methods</B><br> 

The virtual drivers in WCMod will eventually attach to platform specific
drivers. But prior to that, the registers, fields, and thier address (which
depends on the addressing mode and cl22/cl45 considerations) are disambiguated.
The psueodo code below illustrates this breakdown.

\code
WC40_REG_READ(unit, pc, flags, reg_addr, val);
* unit ignored 
* pc is wcmod_st 
* flags ignored 
* reg_addr is the arch. address (i.e. needs to decoded) 
* val is a short int  pointer  to store the read value. 
  
  wcmod_reg_aer_read(unit, pc, reg_addr, val);
  * pc is the same wcmod_st 
  * reg_addr is unresolved addr 
  * val is the same short int  pointer  to store the read value. 

  * We fix address for dual XGXS mode. 
  * We fix address for dual strap offsets, multiport_addr_en port etc. 
  * i.e. reg_addr converted to phy_reg_addr 
  * call cl22 or cl45 drivers as needed. Assume we go cl22 way here. 

    wcmod_aer_cl22_read(pc, phy_reg_addr, val)
    * pc is the same wcmod_st 
    * phy_reg_addr is the resolved addr 
    * val is the same short int  pointer  to store the read value. 

    * Figure out broadcast/per lane.  
    * Adjust address again for per lane.  i.e. write lane num to bits 16,17 
    * Conduct a series of reg. accesses based on indirect (AER) reg. access 
    * For example, write the offset to reg. 0x1f etc. 

      wcmod_cl22_read(pc, phy_reg_addr, val)
      * pc is the same wcmod_st 
      * phy_reg_addr is the resolved addr 
      * val is the same short int  pointer  to store the read value. 

        * call the physical driver. (compiled for target platform)
        * In WC verilog, we call a DPI which calls the verilog task
        * In Trident verilog, we call a DPI which directly access the Parallel

        * control bus of the MDIO ( for simulation speedup)
        * In MDK (vxworks vers.), we call our cdk_miim_read(), BRCM medium
        * independent driver
        * In SDK(all versions) we call a function ptr. that is provided by
        * the broadcom software team. 
\endcode

\note The WCMod and documentation is work in progress. Table below 

<table cellspacing=0>

<tr><td colspan=3><B>'Current Status' bit-mappings</B></td></tr>

<tr><td><B>Tier1 </B>                 </td><td><B>Handle</B>            </td><td>PCS or PMA</td><td><B>Status</B></td></tr>
<tr><td>wcmod_aer_lane_select         </td><td> AER_LANE_SELECT         </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_lane_disable         </td><td> TX_LANE_DISABLE         </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_autoneg_page_set        </td><td> AUTONEG_PAGE_SET        </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_cjpat_crpat_control     </td><td> CJPAT_CRPAT_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_lane_swap               </td><td> LANE_SWAP               </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_pll_lock_wait           </td><td> PLL_LOCK_WAIT           </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_prbs_check              </td><td> PRBS_CHECK              </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_print_wcmod_st          </td><td> PRINT_WCMOD_ST          </td><td> NONE     </td><td>DONE   <td></tr>
<tr><td>wcmod_prog_data               </td><td> PROG_DATA               </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_reg_read                </td><td> REG_READ                </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_revid_read              </td><td> REVID_READ              </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_rx_loopback_control     </td><td> RX_LOOPBACK_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_rx_seq_control          </td><td> RX_SEQ_CONTROL          </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_port_mode           </td><td> SET_PORT_MODE           </td><td> BOTH     </td><td>DONE   <td></tr>
<tr><td>wcmod_soft_reset              </td><td> SOFT_RESET              </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_bert_control         </td><td> TX_BERT_CONTROL         </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_loopback_control     </td><td> TX_LOOPBACK_CONTROL     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_rx_packets_check     </td><td> TX_RX_PACKETS_CHECK     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_rx_polarity          </td><td> TX_RX_POLARITY          </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_100fx_control           </td><td> 100FX_CONTROL           </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_autoneg_set             </td><td> AUTONEG_SET             </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>wcmod_autoneg_control         </td><td> AUTONEG_CONTROL         </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>wcmod_cjpat_crpat_check       </td><td> CJPAT_CRPAT_CHECK       </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_clause72_control        </td><td> CLAUSE72_CONTROL        </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_core_reset              </td><td> CORE_RESET              </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>wcmod_duplex_control          </td><td> DUPLEX_CONTROL          </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_parallel_detect_control </td><td> PARALLEL_DETECT_CONTROL </td><td> PCS      </td><td>INPROG <td></tr>
<tr><td>wcmod_pll_sequencer_control   </td><td> PLL_SEQUENCER_CONTROL   </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_port_stat_display       </td><td> PORT_STAT_DISPLAY       </td><td> BOTH     </td><td>DONE   <td></tr>
<tr><td>wcmod_prbs_control            </td><td> PRBS_CONTROL            </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_speed_intf_set          </td><td> SET_SPEED_INTF(Deprecate)</td><td> BOTH    </td><td>INPROG <td></tr>
<tr><td>wcmod_speed_intf_control      </td><td> SPD_INTF_CONTROL        </td><td> BOTH     </td><td>INPROG <td></tr>
<tr><td>wcmod_Idriver_Ipredriver_set  </td><td> IDRIVER_IPREDRIVER_SET  </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_FEC_control             </td><td> FEC_CONTROL             </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_firmware_set            </td><td> FIRMWARE_SET            </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_packet_size_IPG_set     </td><td> PACKET_SIZE_IPG_SET     </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>wcmod_power_control           </td><td> POWER_CONTROL           </td><td> PMA/PMD  </td><td>NOTDONE<td></tr>
<tr><td>wcmod_rx_loopback_sfpplus     </td><td> RX_LOOPBACK_SFPPLUS     </td><td> PCS      </td><td>DONE   <td></tr>
<tr><td>wcmod_scrambler_control       </td><td> SCRAMBLER_CONTROL       </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>wcmod_set_BRCM64B66B          </td><td> SET_BRCM64B66B          </td><td> PCS      </td><td>NOTDONE<td></tr>
<tr><td>wcmod_set_rx_DFE_tap1_override</td><td> SET_RX_DFE_TAP1_OVERRIDE</td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_DFE_tap2_override</td><td> SET_RX_DFE_TAP2_OVERRIDE</td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_DFE_tap3_override</td><td> SET_RX_DFE_TAP3_OVERRIDE</td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_DFE_tap4_override</td><td> SET_RX_DFE_TAP4_OVERRIDE</td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_DFE_tap5_override</td><td> SET_RX_DFE_TAP5_OVERRIDE</td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_PF_override      </td><td> SET_RX_PF_OVERRIDE      </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_set_rx_VGA_override     </td><td> SET_RX_VGA_OVERRIDE     </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_tx_tap_control          </td><td> TX_TAP_CONTROL          </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_rx_p1_slicer_control    </td><td> RX_P1_SLICER_CONTROL    </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_rx_m1_slicer_control    </td><td> RX_M1_SLICER_CONTROL    </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_rx_d_slicer_control     </td><td> RX_D_SLICER_CONTROL     </td><td> PMA/PMD  </td><td>DONE   <td></tr>
<tr><td>wcmod_diag                    </td><td> WCMOD_DIAG              </td><td> BOTH     </td><td>INPROG <td></tr>

</table>

<B>Overriding native with Customer PCS Functions.</B><br> 

Customers can attach custom functions to WCMod. Though it seems to indicate
replacement or augmentation of Tier1 functions accessing registers in the PCS
domain only (PCS Functions), any WCMod function can be selectively replaced
with this methodology. This makes WCMod more versatile in handling PHY modules
with different PCS blocks and also locale specific interpretation of tier1
functions. The flow chart provided below shows how overriding is possible.

WCMod provides a structure, #str2PCSFn, to store a function pointer and
string pointers to identify it. A function Pointer Table #pcsFuncTable is
an array of these structures. The following steps populate #pcsFuncTable table.
\li 1. Initialize each element's string pointer to an identifying string
\li 2. Initialize same element's function pointer to a function
\li 3. Create one element for each overriding function 
\li 4. The last table entry must have string pointer and function pointer NULLed

<B>Function Execution.</B><br>
WCMod user interface provides unique strings to identify all tier1 functions.
 Execution of a Tier1 function involves the following steps.

\li 1. Use input string and query Customer PCS Function Table. 
\li 2. If the string matches and the associated function pointer is NULLed, we
    assume user does not want this Tier1 function.
\li 3. If a non-NULL function pointer is found, we execute it and evaluate the return code.
\li 4. If return code is zero, we exit the loop. Any negative return value is
  treated as an error. If return code is 1 (Augmentation mode), we continue to
search for this function in local table and execute it if found.
\li 5. If function is not found we try to find it in the local function table.

<IMG SRC="../../doc/customerPCSsupport.jpg">

*/ 

#endif   /*  _WCMOD_MAIN_H_ */
