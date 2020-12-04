/*----------------------------------------------------------------------
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Broadcom Corporation
 *  Proprietary and Confidential information
 *  $Copyright: (c) 2012 Broadcom Corp.
 *  All Rights Reserved.$
 *  This source file is the property of Broadcom Corporation, and
 *  may not be copied or distributed in any isomorphic form without the
 *  prior written consent of Broadcom Corporation.
 *----------------------------------------------------------------------
 *  Description: Main doc header file for TEFMod.
 *---------------------------------------------------------------------*/
/*!
@page  tefmod-doc TEFMod Documentation
@tableofcontents
@section tefmod-intro Introduction
@version 0.1

TEFMod is <b>T</b>DM Serdes controller <b>F</b>alcon <b>MOD</b>ule

TDM Serdes controller Falcon Module (TEFMod)  is a source-code/binary
distributable library that defines a hardware abstraction layer for various
configuration modes of the broadcom serdes IP TSCF. This document expects the
reader to be familiar with the TSCF architecture.

The tiered structure progressively encapsulates register accesses and sequence
hiding complexities of register addressing, access protocol implementations. The
Tier1 layer are a set of functions each partially configuring the
TSCF. They are used to construct more comprehensive TSCF configuration in the
calling layers (Tier2 layers).  Tier1 procedures are fixed sequences of register
accesses, accomplishing a specific sub-configuration.

The full list of Tier1 procedures are tabulated below.

TEFMod architecture documentation is created using doxygen and so follows the
code structure. Please use the following guidelines to use the documentation.

\li tefmod_cfg_seq.c
\li tefmod_enum_defines.c
\li tefmod_defines.h
\li tefmod_enum_defines.h
\li tefmod_doc.h
\li falcon_tsc_interface.h

Tier1 procedures don't have a fixed arg list. They return success/failure
code and accept variable number of arguments. If additional values need to be
returned, calling layes have to pass a reference which the tier1s will modify.
For example, to the serdes Id register, pass a pointer to an int to the tier1
which reads the serdes id.

Tier1 procedures don't have a fixed arg list. They return success/failure
code and accept variable number of arguments. If additional values need to be
returned, calling layes have to pass a reference which the tier1s will modify.
For example, to the serdes Id register, pass a pointer to an int to the tier1
which reads the serdes id.

Tier1s  program TSCF per port. Usually a specific lane of the port is all that
needs to be accessed to program the port. (lane 0 -> 4 lane modes, lane 0/2
for dual modes, lane 'n' for single-lane-modes). However platforms can
explicitly select the lane (lane_select).
Some Tier1 functions can operate on multiple lanes as selected by the platform.
The lane specific information is specified (per_lane_control). In case of tier1s
handling multiple lanes, the per_lane_control has per lane fields, usually 8
bits (1 Byte) for each lane.

\section portmode-lane-tscf Port modes and lanes of TSCF

The TSCF is a quad lane PHY. It supports one or more ports based on its port
modes.

- Single Port Mode
  -# All lanes combine to form a single port
- Dual Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 and 3 combine to form another dual port
- Tri0 Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port
- Tri1 Port Mode
  -# Lanes 2 and 3 combine to form a dual port
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
- Quad Port Mode
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port

So we can have 1, 2, 3,  or 4 ports in each core. Each port can support many
different speeds.
<speed info. TBD>

\section temod-intf-sec TEFMod Interface to System Platforms

TEFMod has an unambiguous interface mechanism to system platforms: Software
platforms like MDK, SDK or customer specific software Or validation platforms
including HDL (verilog/systemVerilog/Vera) based testbenches.

TEFMod functions operates on a single TSCF core. It can be used to access
multiple lanes within the TSCF, sometimes simultaneously (a.k.a broadcast)
but it cannot access multiple TSCFs simultaneously.

In pre-silicon validation (DV) platforms, TEFMod defines a structure (called
tefmod_st) which encapsulates information needed to configure the TEFMod.
tefmod_st is created and maintained by validation platforms, typically as an
array  accessible by the TSCF's unique physical address.

\section regacc-sec TEFMod Register Access Methods

TSCF does not use the traditional MDIO bus protocol. TO_TSC bus is used to talk
to TSCF regs. This is a 8 bit high speed bus, connected to the SBUS via an
indirect accessing scheme at chip level. It has a 32 bit address bus with the
following fields.

- Bits 15:0   : 16-bit register address.
- Bits 18:16 : AER mode. The ports and lanes affected are as follows.
  -# 0: Port 0 - one lane  (Logical Lane 0)
  -# 1: Port 1 - one lane  (Logical Lane 1)
  -# 2: Port 2 - one lane  (Logical Lane 2)
  -# 3: Port 3 - one lane  (Logical Lane 3)
  -# 4: Port 0 - two lanes (Logical Lanes 0 and 1)
  -# 5: Port 2 - two lanes (Logical Lanes 2 and 3)
  -# 6: Port 0 - fouw lane (Logical Lanes 0, 1, 2, and 3)
  -# 7: Undefined.
- Bits 23:19 : Port ID. The TSCF has up to 4 ports.
  -# 0:Port 0 ID = prtad_strap_dst
  -# 1:Port 1 ID = prtad_strap_dst + 1
  -# 2:Port 2 ID = prtad_strap_dst + 2
  -# 3:Port 3 ID = prtad_strap_dst + 3
  -# 4:Broadcast = main0_prtad_bcast (broadcast to ports whose main0_multiPRTs_en bit are set)
- Bits 31:27 : Device ID (0 or PCS, 1 for PMD)

\subsection bcst-sec How to Broadcast:

There are many players involved here.  First system can set a reg:
main0_prtad_bcast to a custom value. This value, when inserted in 31:27 field of
the address enables broadcast.  However each port has a main0_multiPRTs_en
register. You can only only broadcast if that per port bit is enabled.

\subsection strap-sec What is prtad_strap_dst?

This is a register at chip level which is set to identify a TSCF.(unique to each
TSCF core)

\section regmacro-sec Register Access Macros

The essence of TEFMod is to configure the PHY by accessing its registers.  Since
multiple platforms adopt TEFMod, it has no register access methods.  It has
virtual methods to read, write, and modify registers.  TEFMod uses a
auto-generated macro set to create these virtual routines.  The virtual routines
eventually attach to platform specific register access methods, such as device
driver functions or verilog tasks.  Prior to that, the registers, addresses,
fields, are disambiguated.

The register access macros are in include/phymod/chip/bcmi_tscf_xgxs_defs.h

The virtual register access function is described below using the 'read'
function of an arbitrary register. Write is similarly implemented.

\code
Register: PHYID2 (Adr:0x0002). This is a read only IEEE Identification register
READ_PHYID2r(pa, ...)
  -> phymod_tsc_iblk_read(pa, ...)
     -> PHYMOD_BUS_READ(pa, ...)
        -> phymod_bus_read(pa, ...)
           -> pa->read(...)
\endcode
The last function is actually a pointer to a read function supplied by the
platform. For TSC based PHYs it will be an SBUS protocol tied to the TO_TSC
protocol bus of the PHY.  For PCS-less PHYs, it will be an MDIO protocol. PHYMod
will autodetect the protocol and translate the access.  Both PCS and  PMD
registers are similarly accessed differing only in their devid fields.  Note
that the Unified PHY Address scheme used in register accesses. This is a 32 bit
'single shot' register.  Refer to this documentation elsewhere [FIXME]

\section funclist-set The TEFMod Tier1 function list.

The list of Tier1 functions and documentation is in \ref tefmod_cfg_seq.c

\section config-sec Configuration of TSCF

@image html SC_TMod.jpg

The TSCF PHY has two PHY sub-layer components. The PCS and the PMD sub-layer.
The PMD sub-layer is referred to as Falcon. The PCS layer is sometimes referred
to as TSCF or PCS.  The PHY is configured by configuring both PCS and PMD sub
layers.

This section deals with configuration details of both sub-layers.

The PMD configuration is controlled by both TEFMod and the PMD firmware.
However some basic PMD configurations are always done by TEFMod. Other speed
specific PMD configuration is done either by TEFMod or firmware depending on the
speed modes. In autoneg modes, where software cannot configure PMD fast enough,
firmware gleans PMD requirements via a handshake with the PCS. (Firmware cannot
program PMD without this info). Also the PMD may or may not be accompanied by a
PCS. So we have two modes of PMD configuration.

\li PCS linked (dependent)
\li Standalone (independent)

The PCS configuration detailed further below is done in one of 3 ways.
\li TEFMod selects a 'canned' set of PCS parameters (called speed_config)
\li TEFMod selects a 'canned' set and selectively overriding certain fields
\li TEFMod configures the PCS with a full set of coherent parameters.

\subsection fpmdcfg-sec PMD configuration

The term 'PMD' encompasses both PMD/PMA sub layers and AFE. The Falcon PMD used
in TSCF is a 25Gbps Serdes, basic configuration being a quad 25Gbps serdes
targeted for optical and backplane applications. It supports data rates from
10.3125Gbps to 27.FIXME Gbps. Details of the Falcon PMD are here.

\code
At the time of this writing, we don't have a document for Falcon PMD yet
\endcode

As mentioned above PMD configuration is either PCS linked or standalone.
However the following mode/PCS agnostic PMD attributes are always configured by
TEMod.

[this section is pending documentation]
\li hearbeat_count_1us:
\li TX Analog Parameters: Tx_amp etc. conditionally programmed based on
channel quality.

\li Lane Polarity: Both TX and RX polarity of the serdes pins can be inverted.

\li Cfg_from_PCS:
If this mode is set, firmware will read information that came in from PCS and
configure the PMD. If not, firmware assumes that TEMod has configured the PMD.
\li Core_cfg_from_pcs (tells firmware that per core info. is coming from PCS)
\li Lane_cfg_from_pcs (tells firmware that per lane info. is coming from PCS)

\li PLL Configuration (pending documentation)

\subsubsection fls-sec Lane Swap

The physical lanes in PMD can be switched to represent different logical lanes.
TX and RX are independently controlled. TEFMod configures the lane swap registers
in the PMD. In the PMD, only the TX of lanes are swappable. RX is fixed.
For a complete lane swap solution, PMD laneswap is complimented by PCS lane
swapping feature. The PCS laneswap feature is  paired, i.e., both TX and RX will
be swapped.  (more about this later)

Lane swapping also changes the configuration access. i.e. the physical lane
being programmed will be different from the logical lane due to lane swapping.
Therfore swapping is the first thing configured while the datapath is in reset.
The PCS and PMD lane swapping are independent.  TEFMod must ensure that swapping
is meaningful. TEFMod will in general keep PCS and PMD lane swapping
symmetrical. Lane swapping is a static configuration. Dynamic swapping is not
supported.

From customers perspectives, they need to set the soc preperties
xgxs_tx_lane_map and xgxs_rx_lane_map to control lane swap in the chip in order
to compensate the lane swap in the board routing.   Historically the notation
for WC platforms (TD+ and TR3) xgxs_tx_lane_map is physical lane base, but
xgxs_rx_lane_map is the logic lane base.  The physical lane base notation is
that the digit positions are based on physical lane as P3P2P1P0.

- For example in WC, xgxs_tx_lane_map=0x2031 means
  - physical lane 3 is to connect to logic lane 2,
  - physical lane 2 is to connect to logic lane 0,
  - physical lane 1 is to connect to logic lane 3, and
  - physical lane 0 is to connect to logic lane 1.
- Another WC example for xgxs_rx_lane_map=0x1320 which is logic lane base:
  it has
  - logic lane 3 is to connect to physical lane 1,
  - logic lane 2 is to connect to physical lane 3,
  - logic lane 1 is to connect to physical lane 2, and
  - logic lane 0 is to connect to physical lane 0.

The notation for TD2 and TD2+ are the opposite: xgxs_tx_lane_map is logic lane
base, and xgxs_rx_lane_map is the physical lane base due to different mapping
design.

Otherwise the rest of platforms for TSCE and TSCF should assume both
xgxs_tx_lane_map and xgxs_rx_lane_map are all logic lane base.

\subsubsection dep-fcfg-sec PCS Dependent Mode PMD configuration

In dependent mode configuration, PMD and PCS form a full featured PHY.
PMD configuration assumes a PCS in Forced Speed Mode or is done by firmware
using PCS hints in Autoneg mode).

\subsubsection pmd-aneg-sec Autoneg Mode

In autoneg modes, the PMD is configured so that the AN pages can be exchanged.
Initially firmware will set the PMD for the relevant AN mode (CL37AN or CL73AN).
It will keep PMD in reset, configure it, and unreset it. After this the pages
get exchanged and PCS will resolve the actual speed. It will now again put the
PMD in reset, and reconfigure the PMD to the new resolved speed.

The TEFMod programming sequence for AN will look like so.
\li 1. TEFMod keeps PMD in reset
\li 2. TEFMod sets the cfg_from_PCS bit.
\li 3. TEFMod configures other speed invariant attributes.
\li 4. TEFMod unresets PMD.
\li 5. Program the PCS (init, setup advertisements, setup AN)
\li 6. (Wait for status update and read Status registers)
\li 7. (Compare PCS Stats)
\li 8. Wait for PCS_LINK

NOTE: Steps 6 and 7 are not strictly needed for configuration. However this is
an important part of driver functionality because we want to make sure that the
PHY device and TEFMod are both on the 'same page' vis-a-vis programming.

After 5. PCS sends a per core and per lane bit encoding to PMD as shown below.
This information is captured in registers in PMD.

PMD register: dig_com_pmd_core_mode_status
\li Core : {Reserved[3:0], OTP_options[3:0], core_speed_id[7:0]}
PMD Register: ckrst_ctrl_pmd_pmd_lane_mode_status
\li Lane : {Reserved[4:0], CL72_en, scrambler_dis, eee_mode_en, lane_speed_id[7:0]}

\li eee_mode_en: Description pending
\li scr_en: Description pending
\li osr_mode: Description pending
\li cl72_ieee_training_enable: Description pending
\li VCO rate: Description pending
\li BRDFE_ON: Baud rate DFE control. 0 for Autoneg. 1 in some rare forced speed debug modes.
\li OSDFE_ON: '1' for CL72 channels. Almost always '0' in other cases.
\li Media_type: 2bit field for backplane or copper (KR or CR) in autoneg.

\subsubsection fs-mode-sec Forced speeds Mode

In forced speed mode, the PCS is configured completely by TEFMod. The
corresponding PMD configuration is also derived by TEFMod and is used to program
the PMD. The same attributes will be programmed by TEFMod as accomplished by PCS
+ Firmware in autoneg. The PHY configuration will be as shown below
\li 1. TEFMod keeps the PMD in reset
\li 2. Program the PMD (init, set up user mode. See further below)
\li 3. Unreset the PMD
\li 4. Program the PCS (init, set up speed. See further below)
\li 5. Wait for status update and read Status registers
\li 6. Compare PCS Stats
\li 7. (Wait PLL_LOCK)
\li 8. (Wait PMD_LOCK)
\li 9. Wait for PCS_LINK

NOTE: Steps 5 and 6 are not strictly needed for configuration. However this is
an important part of driver functionality because we want to make sure that the
PHY device and TEFMod are both on the 'same page' vis-a-vis programming.

NOTE: Steps 7 and 8 are also not needed either. However this information makes
for easier debug when things do not work.

\li hearbeat_count_1us
\li TX Analog Parameters
\li Lane Polarity
\li Core_cfg_from_pcs (tells firmware that per core info. is coming from PCS)
\li Lane_cfg_from_pcs (tells firmware that per lane info. is coming from PCS)
\li PLL Configuration
\li Lane Swap
\li an_enabled

\code
1: CL73 or CL37 AN is enabled
0: AN is not enabled
\endcode

\li eee_mode_en
TSCF does not support EEE feature. It is always set to zero for eagle.

\li scrambling_dis
\code
1: RX input data not scrambled, enable pattern tolerance at the expense of some BER margin.
0: RX input data is scrambled
\endcode

\li osr_mode
If osr_mode_frc  (CKRST_CTRL_OSR_MODE_CONTROL[15]) is set OS configuration
information comes from osr_mode_frc_val (CKRST_CTRL_OSR_MODE_CONTROL[3:0]). Else
OS value comes from the PMD input pins.  OS coding based on osr_mode_frc_val
{osr_mode_force_val}

\code
0  : Oversampled by 1 mode
1  : Oversampled by 2 mode
2  : Oversampled by 4 mode
3  : Oversampled by 5 mode
6  : Oversampled by 8.25 mode
7  : Oversampled by 3.3 mode
8  : Oversampled by 10 mode
9  : Oversampled by 3 mode
10  Oversampled by 8 mode
Reset value is 0x0.
\endcode

TEFMod Tier1 (DECODE_SET) sets the OS value based on the baud rate of the speed mode set.

\li cl72_ieee_training_enable
\code
1: HW Clause 72 is enabled, implicitly enables DFE also
0: HW Cl72 is disabled
\endcode

\li vco_rate[4:0]
Provides the VCO rate to closest 0.25 GHz
Setting = Round(Frequency_GHz/0.25) to 22
Frequency_GHz = 0.25*setting + 5.5
where Round(x) is the nearest integer to x.

\code
Note - Use 10.25GHz (setting 19) for 10.3125GHz
1: VCO rate is ~5.75GHz
2: VCO rate is ~6.00GHz
...
18: VCO rate is ~10.00GHz
19: VCO rate is ~10.25GHz
22: VCO rate is ~11.00GHz
...
30: VCO rate is ~13.00GHz
31: VCO rate is ~13.25GHz
\endcode

\li brdfe_on: Sometimes turned on for debugging.
\li osdfe_on: Turned on for forced speed CL72 and other debug scenarios.
\li media_type[1:0]

\code
00  PCB traces or Backplane
01  Copper cable
10  Optics
\endcode

\li txfir_override_en: Not needed for DV. Description pending.
\li txfir_main_override: Not needed for DV. Description pending.
\li txfir_pre_override: Not needed for DV. Description pending.
\li txfir_post_override : Not needed for DV. Description pending.
\li unreliable_los: Not needed for DV.  For 'optical' media_type only.
\code
1: Assume LOS is unreliable. This is for optical modules with non-functional optical LOS.
0: Assume LOS is reliable
\endcode

\li cl72_emulation_en
Not needed for DV. Needed only for SP2. s/w CL72. Remote Device has no CL72)
This feature is used when Forced mode Clause 72 like functionality is desired,
but link partner does not have HW Cl72. This was used in a special application
for 40nm Voltron ASIC product.
\code
1: Emulation of HW CL72 (applies only if HW CL72 is disabled)
0: Emulation of HW CL72 is not performed
\endcode

\subsubsection ind-prog-sec Independent Mode Programming
  In this case, the PCS is not involved. The required PMD configuration is
  done entirely by TEFMod.
  to program the PMD. The TEFMod sequence will look like.
\li 1. Hold the PMD in reset
\li 2. Program the PMD (init, set up user mode. Exactly  like in forced mode)
\li 3. Unreset the PMD

The programming model is similar to section 5. Exceptions description
pending (for example variable refclk)

\section pcscfg-sec PCS configuration

PCS is configured while keeping the datapaths in reset. i.e. reset, configure
the PCS, and release reset.
The PCS configuration is done in one of 3 ways.
\li TEFMod selects a 'canned' set of PCS parameters (called speed_config)
\li TEFMod selects a 'canned+override' set and selectively overriding certain fields
\li TEFMod selects a 'custom' set and selectively overriding certain fields
\li TEFMod configures the PCS with a full set of coherent parameters.

The configuration components are

\subsection crd PCS credits: This defines the rate of transmission/reception towards the
line and system sides. This can be set up by TEFMod for forced speeds.
For autoneg speeds, the PCS logic will program the credits based on the resolved
speeds.

\subsection spd Speed: This defines the speed at which to run the PCS. Speed will also
define the number of lanes and portmode required.

  -  In 'canned speed' mode PCS has a table for all supported speeds, which is
     used to configure all PCS stages including credit generation.
  -  In 'canned + override speed' mode PCS has the same table for supported speeds,
     however TEFMod can selectively override some of the configurations. PCS
     will configure PCS stages with the overridden values.
  -  In 'custom speed' mode PCS has four programmable tables for four custom
     speeds. TEFMod can set the override table with custom configurations. PCS
     will configure PCS stages with the custom values.
  -  In 'bypass speed' mode PCS will not configure anything. All stages have to
     be configured explicitly by software. <b>TEFMod does not support bypass</b>

\subsubsection pcs-fs PCS configuration for forced speed modes.

   - TEFMod configures PMD as mentioned in \ref fpmdcfg-sec
   - TEFMod sets the SW_SPEED register.
   - TEFMod sets the SW_SPEED_CHANGE bit.
   - TEFMod will overwrite any specific sub-configuration as needed.
     -#  [now the PCS relinquish reset control of PMD. PMD will initialize and
       get it's 'lock' i.e. ability to get digital data at known rates]
     -# PCS will come out of reset when it sees the pmd_lock. It will eventually
     show pcs_link (the ability to understand (decode) digital data)
     -# PCS will clear the SW_SPEED_CHANGE bit.

\subsubsection pcs-aneg PCS configuration for autoneg enable
   - TEFMod will set up PMD for autoneg speed as mentioned in \ref fpmdcfg-sec
   - TEFMod will cause a positive edge on the autoneg enable or restart bits.
     -#  PCS will begin resetting the PCS RXP, TXP, and PMD
     -#  PCS will configure for autoneg speed (CL37/CL73)
     -#  PCS will start and complete credit exchange
     -#  PCS will stop resetting the PMD.
     -#  PCS will start the auto negotiation hardware.
     -#  PCS will stop resetting the PCS TXP for the auto negotiation lane.
     -#  PCS will wait for pmd_rx_lock for the auto negotiation lane.
   -  PCS will release the PCS RXP reset for the auto negotiation lane.
   -  PCS will wait for auto negotiation to complete or fail.
   -  PCS will restart auto negotiation If auto negotiation fails
   -  PCS will begin resetting the PCS RXP.
   -  PCS will begin resetting the PCS TXP.
   -  PCS will begin resetting the PMD.
   -  PCS will select the resolved speed (new speed).
   -  PCS will start the credit change sequence for the new speed..
   -  PCS will wait until the credit change completes.
   -  PCS will release the PMD reset.
   -  PMD firmware will configure the PMD for the new speed.
   -  PCS will release the PCS TXP.
   -  PCS will wait for pmd_rx_lock for all lanes in the port.
   -  PCS waits for RXP PCS to report link_up or link_fail_inhibit timer to expire.
   -  PCS will restart autoneg if the link_fail_inhbit timer expires.
   -  PCS will set the auto negotiation completed bit.

\subsubsection pcs-aneg PCS configuration for autoneg disable
   -  TEFMod will cause a negative edge on the autoneg enable.
   -  PCS will begin resetting the PCS RXP.
   -  PCS will begin resetting the PCS TXP.
   -  PCS will begin resetting the PMD.
   -  PCS will select the software speed.
   -  PCS will start the credit change sequence.
   -  PCS will wait until the credit change completes.
   -  PCS will stop resetting the PMD.
   -  The PMD will begin configuring itself for the new speed.
   -  PCS will release the PCS TXP reset.
   -  PCS will wait for pmd_rx_lock for all lanes in the port.
   -  PCS will release the PCS RXP reset.
   -  PCS will clear the auto negotiation active status bit.

\subsection Overriding native with Customer PCS Functions.

This capability is no longer supported. Please contact the TEFMod team if you
need this. (ravick@broadcom.com)
*/

The PMD data dump has similarly controls. The DSC states are shown with the
command below.

\code
BCM.0> phy diag xe0 dsc



***********************************
**** SERDES CORE DISPLAY STATE ****
***********************************

Average Die TMON_reg13bit = 5720
Temperature Force Val     = 0
Temperature Index         = 12  [56C to 64C]
Core Event Log Level      = 1

Core DP Reset State       = 0

Common Ucode Version       = 0xd107
Common Ucode Minor Version = 0x0
AFE Hardware Version       = 0xb0

LN (CDRxN  ,UC_CFG) SD LCK RXPPM PF(M,L) VGA DCO  DFE(1,2,3,4,5,6)        TXPPM TXEQ(n1,m,p1,p2,p3) EYE(L,R,U,D)  LINK_TIME
 0 (OSx1   ,0x 4)   1   1    0    0,2    16   4  48, -7,  5,  2,  0,  0     0     0,100,12, 0, 0   171,359, 82, 82    94.5


------------------------------------------------------------------------
Falcon PMD State
PARAMETER       	         LN0         LN1         LN2         LN3
------------------------------------------------------------------------
TX drv_hv_disable	           0           0           0           0
TX ana_rescal   	           8           8           8           8
TX amp_ctrl     	           8           8           8           8
TX pre_tap      	           0           0           0           0
TX main_tap     	         100         100         100         100
TX post1_tap    	          12          12          12          12
TX post2_tap    	           0           0           0           0
TX post3_tap    	           0           0           0           0
------------------------------------------------------------------------
Sigdet          	           1           1           1           1
PMD_lock        	           1           1           1           1
DSC_SM (1st read)	        3feh        3feh        3feh        3feh
DSC_SM (2nd read)	         80h         80h         80h         80h
------------------------------------------------------------------------
PPM             	           0           0           0           0
VGA             	          16          17          18          17
PF              	           0           0           0           0
PF2             	           2           1           1           2
main_tap        	         -28         -28         -28         -28
data_thresh     	          48          47          48          51
phase_thresh    	          63          63          63          63
lms_thresh      	           0           0           0           0
------------------------------------------------------------------------
d/dq hoffset    	          64          64          64          64
p/pq hoffset    	          65          65          66          66
l/lq hoffset    	          66          64          66          67
d/p hoffset     	          31          31          32          30
d/l hoffset     	           0           0           0         255
------------------------------------------------------------------------
dfe[2][a,b]     	      -7, -8      -3, -4      -4, -5      -5, -6
dfe[2][c,d]     	      -7, -6      -3, -3      -5, -5      -6, -6
dfe[3][a,b]     	       6,  6       7,  7       6,  6       5,  5
dfe[3][c,d]     	       5,  6       7,  7       6,  6       5,  6
dfe[4][a,b]     	       2,  2       1,  1       5,  5       2,  2
dfe[4][c,d]     	       2,  3       0,  1       6,  5       2,  2
dfe[5][a,b]     	       1,  1       4,  4       1,  1      -1, -1
dfe[5][c,d]     	       0,  0       4,  3       1,  1      -1,  0
dfe[6][a,b]     	       0,  1       1,  1       1,  1       1,  1
dfe[6][c,d]     	       1,  1       1,  1       1,  1       1,  1
------------------------------------------------------------------------
dfe[7][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[7][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[8][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[8][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[9][a,b]     	       0,  0       0,  0       0,  0       0,  0
dfe[9][c,d]     	       0,  0       0,  0       0,  0       0,  0
dfe[10][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[10][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[11][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[11][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[12][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[12][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[13][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[13][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[14][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[14][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[15][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[15][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[16][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[16][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[17][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[17][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[18][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[18][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[19][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[19][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[20][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[20][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[21][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[21][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[22][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[22][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[23][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[23][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[24][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[24][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[25][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[25][c,d]    	       0,  0       0,  0       0,  0       0,  0
dfe[26][a,b]    	       0,  0       0,  0       0,  0       0,  0
dfe[26][c,d]    	       0,  0       0,  0       0,  0       0,  0
------------------------------------------------------------------------
dc_offset       	           4          -7          13          12
data_p[a,b]     	      -9,  4      -1, -1      -3,  3      -9, -3
data_p[c,d]     	       2, -3      -4, -5       2,  0       1,  0
data_n[a,b]     	      -3,  0      -1,  0      -2, -9       4, -6
data_n[c,d]     	      -7,  2       1,  0      -2, -2      -4, -2
phase_p[a,b]    	      -5,  4       0, -5      -9, -6      -5, -9
phase_p[c,d]    	      -4, -1       4,  0      -4, -1       5,  9
phase_n[a,b]    	      -5,  6     -11, -1       0,  0       0, -2
phase_n[c,d]    	       2,  4      -1, -7      -3,  2      -1, -2
lms[a,b]        	     -11,  3       5,-14      -3, -1      -1,  1
lms[c,d]        	      -1, -3      -9, -4      -7, -4       1,-10
------------------------------------------------------------------------
PRBS_CHECKER    	           0           0           0           0
PRBS_ORDER      	          31          31          31          31
PRBS_LOCK       	           0           0           0           0
PRBS_ERRCNT     	  2147483648  2147483648  2147483648  2147483648
------------------------------------------------------------------------



********************************************
**** SERDES UC TRACE MEMORY DUMP ***********
********************************************

  DEBUG INFO: trace memory read index = 0x0026
  DEBUG INFO: trace memory size = 0x0300

  0x8017  0xd911  0xff00  0x0283  0xff94  0x0e81  0xfdc3  0x0e80    0
  0xfd24  0x0e82  0xfcad  0x0e80  0xd81a  0x0783  0xd818  0x0782    16
  0xd816  0x0781  0xd815  0x0700  0x0000  0x0000  0x0000  0x0000    32
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    48
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    64
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    80
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    96
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    112
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    128
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    144
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    160
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    176
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    192
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    208
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    224
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    240
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    256
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    272
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    288
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    304
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    320
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    336
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    352
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    368
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    384
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    400
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    416
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    432
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    448
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    464
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    480
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    496
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    512
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    528
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    544
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    560
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    576
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    592
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    608
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    624
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    640
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    656
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    672
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    688
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    704
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    720
  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000  0x0000    736
  0x0000  0x0000  0x0000  0x0000  0x041c  0x0df8  0x0000  0x0000    752
  Lane 0:   t= 0 ms,  Start reading event log

      2 timestamp wraparound(s). 

  Lane 3:   t= -717 ms (+5),  Rx PMD lock
  Lane 1:   t= -722 ms (+1),  Rx PMD lock
  Lane 0:   t= -723 ms (+1),  Rx PMD lock
  Lane 2:   t= -724 ms (+94),  Rx PMD lock
  Lane 0:   t= -818 ms (+0),  Exit from restart
  Lane 3:   t= -818 ms (+0),  Exit from restart
  Lane 2:   t= -818 ms (+0),  Exit from restart
  Lane 1:   t= -818 ms
========== End of Event Log ==================
\endcode

The command below is yet to be implemented.

\code
BCM.0> phy diag xe0 dsc ber
\endcode
The command below shows a variety of PMD core AND lane configurations.
\code

BCM.0> phy diag xe0 dsc config


***********************************
**** SERDES CORE CONFIGURATION ****
***********************************

uC Config VCO Rate   = 19 (10.250GHz)
Core Config from PCS = 0

Lane Addr 0          = 0
Lane Addr 1          = 1
Lane Addr 2          = 2
Lane Addr 3          = 3
TX Lane Map 0        = 0
TX Lane Map 1        = 1
TX Lane Map 2        = 2
TX Lane Map 3        = 3



*************************************
**** SERDES LANE 0 CONFIGURATION ****
*************************************

Auto-Neg Enabled      = 0
DFE on                = 1
Brdfe_on              = 0
Media Type            = 0
Unreliable LOS        = 0
Scrambling Disable    = 0
CL72 Emulation Enable = 0
Lane Config from PCS  = 0

CL72 Training Enable  = 0
EEE Mode Enable       = 0
OSR Mode Force        = 1
OSR Mode Force Val    = 0
TX Polarity Invert    = 0
RX Polarity Invert    = 0

TXFIR Post2           = 0
TXFIR Post3           = 0
TXFIR Override Enable = 0
TXFIR Main Override   = 102
TXFIR Pre Override    = 12
TXFIR Post Override   = 0

\endcode
Get CL72 specific information with this command
\code
BCM.0> phy diag ce0 dsc CL72 
 tscf_phy_pmd_info_dump:449 type = 8192 laneMask  = 0xF
 tscf_phy_pmd_info_dump:469 type = CL72


***************************
** LANE 0 CL93n72 Status **
***************************
cl93n72_signal_detect         = 1   (1 = CL93n72 training FSM in SEND_DATA state:  0 = CL93n72 in training state)
cl93n72_ieee_training_failure = 0   (1 = Training failure detected:                0 = Training failure not detected)
cl93n72_ieee_training_status  = 0   (1 = Start-up protocol in progress:            0 = Start-up protocol complete)
cl93n72_ieee_receiver_status  = 1   (1 = Receiver trained and ready to receive:    0 = Receiver training)

 tscf_phy_pmd_info_dump:469 type = CL72
\endcode
Get DSC specific information with this command
\code
BCM.0> phy diag xe0 dsc debug


************************************
**** SERDES LANE 0 DEBUG STATUS ****
************************************

Restart Count       = 5
Reset Count         = 6
PMD Lock Count      = 3

Disable Startup PF Adaptation           = 0
Disable Startup DC Adaptation           = 0
Disable Startup Slicer Offset Tuning    = 0
Disable Startup Clk90 offset Adaptation = 1
Disable Startup P1 level Tuning         = 0
Disable Startup Eye Adaptaion           = 0
Disable Startup All Adaptaion           = 0

Disable Startup DFE Tap1 Adaptation = 0
Disable Startup DFE Tap2 Adaptation = 0
Disable Startup DFE Tap3 Adaptation = 0
Disable Startup DFE Tap4 Adaptation = 0
Disable Startup DFE Tap5 Adaptation = 0
Disable Startup DFE Tap1 DCD        = 0
Disable Startup DFE Tap2 DCD        = 0

Disable Steady State PF Adaptation           = 0
Disable Steady State DC Adaptation           = 0
Disable Steady State Slicer Offset Tuning    = 0
Disable Steady State Clk90 offset Adaptation = 1
Disable Steady State P1 level Tuning         = 0
Disable Steady State Eye Adaptaion           = 0
Disable Steady State All Adaptaion           = 0

Disable Steady State DFE Tap1 Adaptation = 0
Disable Steady State DFE Tap2 Adaptation = 0
Disable Steady State DFE Tap3 Adaptation = 0
Disable Steady State DFE Tap4 Adaptation = 0
Disable Steady State DFE Tap5 Adaptation = 0
Disable Steady State DFE Tap1 DCD        = 0
Disable Steady State DFE Tap2 DCD        = 0

Retune after Reset    = 1
Clk90 offset Adjust   = 57
Clk90 offset Override = 0
Lane Event Log Level  = 2

\endcode
