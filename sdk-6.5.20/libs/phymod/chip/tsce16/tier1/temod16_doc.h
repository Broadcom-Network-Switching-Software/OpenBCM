
/*
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved.
$Id$
*/
/*!

\page temod16-doc TEMod Documentation

\tableofcontents

\section temod16-intro Introduction to TEMod

TEMod is <b>T</b>DM Serdes controller <b>E</b>agle <b>MOD</b>ule

TEMod  is a source-code distributable library that defines a hardware
abstraction layer for various configuration modes of the Broadcom PHY
IP TSCE and for the SerDes IP Eagle. This document expects the
reader to be familiar with the TSCE/Eagle-PMD architecture.

The tiered structure progressively encapsulates register accesses and sequence
hiding complexities of register addressing, access protocol implementations. The
Tier1 layer are a set of functions each partially configuring the
TSCE. They are used to construct more comprehensive TSCE configuration in the
calling layers (Tier2 layers).  Tier1 procedures are fixed sequences of register
accesses, accomplishing a specific sub-configuration. 

The full list of Tier1 procedures are tabulated below.

TEMod architecture documentation is created using doxygen and so follows the
code structure. Please use the following guidelines to use the documentation.

The documentation is stored in tabs. If you are reading this you are in the main
tab. (See the top left corner of our browser). The other tabs are 'classes'
where the TEMod structs are visible and 'files', which are

\li temod16_cfg_seq.c
\li temod16_enum_defines.c

\li temod16_defines.h
\li temod16_enum_defines.h
\li temod16_doc.h

Tier1 procedures don't have a fixed arg list. They return success/failure
code and accept variable number of arguments. If additional values need to be
returned, calling layes have to pass a reference which the tier1s will modify.
For example, to the serdes Id register, pass a pointer to an int to the tier1
which reads the serdes id. 

Tier1s  program TSCE per port. Usually a specific lane of the port is all that
needs to be accessed to program the port. (lane 0 -> 4 lane modes, lane 0/2
for dual modes, lane 'n' for single-lane-modes). However platforms can
explicitly select the lane (lane_select).
Some Tier1 functions can operate on multiple lanes as selected by the platform.
The lane specific information is specified (per_lane_control). In case of tier1s
handling multiple lanes, the per_lane_control has per lane fields, usually 8 
bits (1 Byte) for each lane.

\section portmode-lane Port modes and lanes of TSCE

The TSCE is a quad lane PHY. It supports one or more ports based on its port
modes. 

- Single Port Mode
  -# All lanes combine to form a single port
- Dual Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 and 3 combine to form another dual port
- Tri1 Port Mode
  -# Lanes 2 and 3 combine to form a dual port
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
- Tri2 Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port
- Quad Port Mode
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port
- Multi Core Mode (TSC12 only)
  -# Three cores are used to form a single core.
  -# Each of these 3 cores can be independent (and have all portmodes above)
  -# They combine to form a 100G/106G port (10 lanes, 2 lanes unused)
  -# They combine to form a 120G/127G port (12 lanes)
- QSGMII Port mode
  -# TBD.

So we can have 1, 2, 3, or 4 ports in each core. Each port can support many
different speeds. 3 cores can form a single port in  TSC12
<speed info. TBD>

\section intf-sec TEMod interface to System Platforms
     
TEMod has an unambiguous interface mechanism to system platforms: Software
platforms like MDK, SDK or customer specific software Or validation platforms
including HDL (verilog/systemVerilog/Vera) based testbenches.

TEMod functions operates on a single TSCE core. It can be used to access
multiple lanes within the TSCE, sometimes simultaneously (a.k.a broadcast) 
but it cannot access multiple TSCEs simultaneously.

In validation (pre-silicon DV) platforms, TEMod has a structure (temod16_st) which
encapsulates information needed to configure the TEMod. temod16_st is created
and maintained by validation platforms, typically as an array accessible by
the TSCE's unique physical address.

\section regacc-sec TEMod Register Access Methods

TSCE does not use the traditional MDIO bus protocol. TO_TSC bus is used to talk
to TSCE regs. This is a 8 bit high speed bus, connected to the SBUS via an
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
- Bits 23:19 : Port ID. The TSCE has up to 4 ports. 
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
register. You can only broadcast if that per port bit is enabled.

\subsection strap-sec What is prtad_strap_dst?

This is a register at chip level which is set to identify a TSCE.(unique to each
TSCE core)

\section regmacro-sec Register Access Macros

The essence of TEMod is to configure the PHY by accessing its registers.  Since
multiple platforms adopt TEMod, it has no register access methods.  It has
virtual methods to read, write, and modify registers.  TEMod uses a
auto-generated macro set to create these virtual routines.  The virtual routines
eventually attach to platform specific register access methods, such as device
driver functions or verilog tasks.  Prior to that, the registers, addresses,
fields, are disambiguated.

PROVIDE MORE DETAILS ON REGMACRO (MORTEN"S FLOW)

The virtual register access function is described below using the 'read'
function as an example. Write is similarly implemented.

\code
TE40_REG_READ(unit, pc, flags, reg_addr, val);
* unit ignored 
* pc is TEMod_st 
* flags ignored 
* reg_addr is the arch. address (i.e. needs to decoded) 
* val is a short int  pointer  to store the read value. 
  
  TEMod_reg_aer_read(unit, pc, reg_addr, val);
  * pc is the TEMod_st 
  * reg_addr is 32 bit addr.
  * val is the same short int  pointer to store the read value. 

    * call the physical driver. (compiled for target platform)
    * In TSCE Pre-silicon DV env., we call a DPI which calls the verilog task
    * that drives the TO_TSC bus (an 8 bit bus) that does the reg. access.
    * In MDK (vxworks vers.), we call our cdk_miim_read(), BRCM medium
    * independent driver
    * In SDK (all versions) we call a function ptr. that is provided by
    * the broadcom software team. 
\endcode

\section funclist-set The TEMod Tier1 function list.

The list of Tier1 functions and documentation is in temod16_cfg_seq.c 

\section config-sec Configuration of TSCE

@image html SC_TMod.jpg

The TSCE PHY has two PHY sub-layer components. The PCS and the PMD sub-layer.
The PMD sub-layer is referred to as Eagle. The PCS layer is sometimes referred
to as TSCE or PCS.  The PHY is configured by configuring both PCS and PMD sub
layers. 

This section deals with configuration details of both sub-layers.

The PMD configuration is controlled by both TEMod and the PMD firmware.
However some basic PMD configurations are always done by TEMod. Other speed
specific PMD configuration is done either by TEMod or firmware depending on the
speed modes. In autoneg modes, where software cannot configure PMD fast enough,
firmware gleans PMD requirements via a handshake with the PCS. (Firmware cannot
program PMD without this info). Also the PMD may or may not be accompanied by a
PCS. So we have two modes of PMD configuration.

\li PCS linked (dependent)
\li Standalone (independent)

The PCS configuration detailed further below is done in one of 3 ways.
\li TEMod selects a 'canned' set of PCS parameters (called speed_config)
\li TEMod selects a 'canned' set and selectively overriding certain fields
\li TEMod configures the PCS with a full set of coherent parameters.

\subsection pmdcfg_sec PMD configuration

The term 'PMD' encompasses both PMD/PMA sub layers and AFE. The Eagle PMD used
in TSCE is a 25Gbps Serdes, basic configuration being a quad 25Gbps serdes
targeted for optical and backplane applications. It supports data rates from
10.3125Gbps to 27.FIXME Gbps. Details of the Eagle PMD are here.

http://ingbu.broadcom.com/hsip/serdes/eagle/default.aspx

As mentioned above PMD configuration is either PCS linked or standalone.
However the following mode/PCS agnostic PMD attributes are always configured by
TEMod.

\li hearbeat_count_1us: This is the number of refclks per 1 micro second.
Typical values are 10'd625 for refclk 156.25MHz and 10'd500 for refclk (125MHz).
Reset value is 10'625.  Temod Tier1 function (SET_PORT_MODE) sets this value
based on the refclk.

\li TX Analog Parameters: Tx_amp etc. conditionally programmed based on
channel quality. Not needed by DV.

\li Lane Polarity: Both TX and RX polarity of the serdes pins can be inverted. 

\li Cfg_from_PCS:
If this mode is set, firmware will read information that came in from PCS and
configure the PMD. If not, firmware assumes that TEMod has configured the PMD.
\li Core_cfg_from_pcs (tells firmware that per core info. is coming from PCS)
\li Lane_cfg_from_pcs (tells firmware that per lane info. is coming from PCS)

\li PLL Configuration
PLL configuration is done by firmware (in autoneg mode as explained further
below) or TEMod in (forced speed modes). The Eagle PLL is a special frac'n
PLL and supports infinite PLL divider values. (The divider value controls the
data rate of the PHY). This coupled with multiple reference clocks will result
in the Eagle PLL supporting a very large number of speeds. Selected speeds
are matured over time and advertised to customers. only in forced speeds.

Hence the PLL configuration in TEMod is a superset of that in the firmware.
The table below shows an example of PLL programming for the Eagle PMD. Note the
weird data rate (10.709G). A full list of supported refclks and divider values
is yet to be provided by PMD IP group.  PLL configuration for DV also involves
programming speed up values to manage simulation time.

\code
----------------------------------------------------------------------
Ref Freq 125.000000 MHz, Data Rate 10.709000 G, Divider 85.672000  
----------------------------------------------------------------------
mmd_resetb       =    0, amscomPLL_CTRL_9[0:0]   : write adr=0xd0b9 dt=0x5202
div              = 0100, amscomPLL_CTRL_3[15:11] : write adr=0xd0b3 dt=0x2106
mmd_en           =    1, amscomPLL_CTRL_8[13:13] : write adr=0xd0b8 dt=0x6c5b
mmd_prsc4or5pwdb =   <>, amscomPLL_CTRL_8[12:12] : write adr=0xd0b8 dt=0x6c5b
mmd_prsc8or9pwdb =   <>, amscomPLL_CTRL_8[11:11] : write adr=0xd0b8 dt=0x6c5b
mmd_div_range    =   <>, amscomPLL_CTRL_8[10:10] : write adr=0xd0b8 dt=0x6c5b
i_pll_frac_mode  =   <>, amscomPLL_CTRL_9[2:1]   : write adr=0xd0b9 dt=0x5202
i_ndiv_int       =   <>, amscomPLL_CTRL_8[9:0]   : write adr=0xd0b8 dt=0x6c55
i_ndiv_frac_h    =   <>, amscomPLL_CTRL_7[13:0]  : write adr=0xd0b7 dt=0x2b02
i_ndiv_frac_l    =   <>, amscomPLL_CTRL_6[15:12] : write adr=0xd0b6 dt=0x1040
mmd_resetb       =    1, amscomPLL_CTRL_9[0:0]   : write adr=0xd0b9 dt=0x5203
i_pll_sdm_pwrdnb =    1, amscomPLL_CTRL_8[14:14] : write adr=0xd0b8 dt=0x6c55
*******************************************************************************
\endcode

\subsubsection ls-sec Lane Swap

The physical lanes in PMD can be switched to represent different logical lanes.
TX and RX are independently controlled. TEMod configures the lane swap registers
in the PMD. In the PMD, only the TX of lanes are swappable. RX is fixed. 
For a complete lane swap solution, PMD laneswap is complimented by PCS lane
swapping feature. The PCS laneswap feature is  paired, i.e., both TX and RX will
be swapped.  (more about this later)

Lane swapping also changes the configuration access. i.e. the physical lane
being programmed will be different from the logical lane due to lane swapping.
Therefore swapping is the first thing configured while the datapath is in reset.
The PCS and PMD lane swapping are independent.  TEMod must ensure that swapping
is meaningful. TEMod will in general keep PCS and PMD lane swapping
symmetrical. Lane swapping is a static configuration. Dynamic swapping is not
supported.

\subsubsection dep-cfg-sec PCS Dependent Mode PMD configuration

In dependent mode configuration, PMD and PCS form a full featured PHY.
PMD configuration assumes a PCS in Forced Speed Mode or is done by firmware
using PCS hints in Autoneg mode).

\subsubsection pmd-aneg-sec Autoneg Mode

In autoneg modes, the PMD is configured so that the AN pages can be exchanged.
Initially firmware will set the PMD for the relevant AN mode (CL37AN or CL73AN).
It will keep PMD in reset, configure it, and unreset it. After this the pages
get exchanged and PCS will resolve the actual speed. It will now again put the
PMD in reset, and reconfigure the PMD to the new resolved speed.

The TEMod programming sequence for AN will look like so. 
\li 1. TEMod keeps PMD in reset
\li 2. TEMod sets the cfg_from_PCS bit.
\li 3. TEMod configures other speed invariant attributes.
\li 4. TEMod unresets PMD.
\li 5. Program the PCS (init, setup advertisements, setup AN)
\li 6. (Wait for status update and read Status registers)
\li 7. (Compare PCS Stats)
\li 8. Wait for PCS_LINK

NOTE: Steps 6 and 7 are not strictly needed for configuration. However this is
an important part of driver functionality because we want to make sure that the
PHY device and TEMod are both on the 'same page' vis-a-vis programming.

After 5. PCS sends a per core and per lane 20 bit encoding to PMD shown below.

\li Core : {Reserved[3:0], OTP_options[3:0], core_speed_id[7:0]}
??FIXME register ??
\li Lane : {Reserved[4:0], CL72_en, scrambler_dis, eee_mode_en, lane_speed_id[7:0]} + OSR{3:0}
??FIXME register ??

\li eee_mode_en: Description pending
\li scr_en: 00 bypass scrambler
\li         01 64b66b scrambler (all 66 bits)
\li         10 8b10b scrambler (all 80 bits)
\li         11 64b66b scrambler (sync bits not scrambled)
\li osr_mode: Description pending
\li cl72_ieee_training_enable: Description pending
\li VCO rate: Description pending
\li BRDFE_ON: Baud rate DFE control. 0 for in Autoneg. '1' in some rare forced speed debug modes.
\li OSDFE_ON: '1' for CL72 channels. Almost always '0' in other cases.
\li Media_type: 2bit field for backplane or copper (KR or CR) in autoneg.

\subsubsection fs-mode-sec Forced speeds Mode

In forced speed mode, the PCS is configured completely by TEMod. The
corresponding PMD configuration is also derived by TEMod and is used to program
the PMD. The same attributes will be programmed by TEMod as accomplished by PCS
+ Firmware in autoneg. The PHY configuration will be as shown below
\li 1. TEMod keeps the PMD in reset
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
PHY device and TEMod are both on the 'same page' vis-a-vis programming.

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
TSCE does not support EEE feature. It is always set to zero for eagle.

\li scrambling_dis
\code
1: RX input data pattern not scrambled, enable pattern tolerance at the expense of some BER margin.
0: RX input data is scrambled
\endcode

\li osr_mode
If osr_mode_frc  (CKRST_CTRL_OSR_MODE_CONTROL[15]) is set OS configuration information comes from
osr_mode_frc_val (CKRST_CTRL_OSR_MODE_CONTROL[3:0]). Else OS value comes from the PMD input pins.
OS coding based on osr_mode_frc_val {osr_mode_force_val}

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

TEMod Tier1 (DECODE_SET) sets the OS value based on the baud rate of the speed mode set.

\li cl72_ieee_training_enable
\code
1: HW Clause 72 is enabled, implicitly enables DFE also
0: HW Cl72 is disabled
\endcode

\li vco_rate[4:0]
Provides the VCO rate to closest 0.25 GHz
Setting = Round(Frequency_GHz/0.25) 22
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
  done entirely by TEMod.
  to program the PMD. The TEMod sequence will look like.
\li 1. Hold the PMD in reset
\li 2. Program the PMD (init, set up user mode. Exactly  like in forced mode)
\li 3. Unreset the PMD

The programming model is similar to section 5. Exceptions description
pending (for example variable refclk)

\section pcscfg-sec PCS configuration

PCS is configured while keeping the datapaths in reset. i.e. reset, configure
the PCS, and release reset.
The PCS configuration is done in one of 3 ways.
\li TEMod selects a 'canned' set of PCS parameters (called speed_config)
\li TEMod selects a 'canned+override' set and selectively overriding certain fields
\li TEMod selects a 'custom' set and selectively overriding certain fields
\li TEMod configures the PCS with a full set of coherent parameters.

The configuration components are

\subsection crd PCS credits: This defines the rate of transmission/reception
towards the line and system sides. This can be set up by TEMod for forced
speeds.  For autoneg speeds, the PCS logic will program the credits based on the
resolved speeds.

\subsection spd Speed: This defines the speed at which to run the PCS. Speed
will also define the number of lanes and portmode required.

  - In 'canned speed' mode PCS has a table for all supported speeds, which is
    used to configure all PCS stages including credit generation.
  - In 'canned + override speed' mode PCS has the same table for supported speeds,
    however TEMod can selectively override some of the configurations. PCS
    will configure PCS stages with the overridden values.
  - In 'custom speed' mode PCS has four programmable tables for four custom
    speeds. TEMod can set the override table with custom configurations. PCS
    will configure PCS stages with the custom values.
  - In 'bypass speed' mode PCS will not configure anything. All stages have to 
    be configured explicitly by software. <b>TEMod does not support bypass</b>

The general procedure to configure the PCS for forced speed modes..

   - TEMod configures PMD as mentioned in \ref pmdcfg_sec
   - TEMod sets the SW_SPEED register. 
   - TEMod sets the SW_SPEED_CHANGE bit.  
   - TEMod will override any specific sub-configuration as needed.
     -# [now PCS relinquish reset control of PMD. PMD will initialize and get
       it's 'pmd_lock'(aka CDR) i.e. ability to get digital data at known rates]
     -# PCS will come out of reset when it sees the pmd_lock. It will eventually
       show pcs_link (the ability to understand (decode) digital data)
     -# PCS will clear the SW_SPEED_CHANGE bit.

SGMII modes are configured differently. It involves the QSGMII PCS
   - TEMod configures PMD as mentioned in \ref pmdcfg_sec
   - TEMod sets TSCE-PCS in bypass.
     -# [now the PCS relinquish reset control of PMD. PMD will initialize and
       get it's 'lock' i.e. ability to get digital data at known rates]
   - TEMod sets TSCE-PCS in ILKN mode. (bypasses datapath into QSGMII PCS)
   - TEMod configures QSGMII-PCS  (1G/100M/10M only for now)
     -# QSGMII-PCS will come out of reset when it sees pmd_lock. It will eventually
     show pcs_link (the ability to understand (decode) digital data)

The procedure is for Auto Negotiation ports. Most of the work is done by the
PCS device without software hand holding in this case.
   - TEMod configues the autoneg mode into TSC-PCS(CL37/CL73/BAM modes etc.)
   - TEMod creates a positive edge on auto-negotiation enable or restart bit.
   - TSC PCS will reset the PMD.
   - TSC PCS configures itself to do autoneg 
   - TSC PCS will unreset PMD and PMD comes to life. Autoneg begins
   - TSC PCS will eventually get the resolved speed.
   - TSC PCS will indicate to TEMod that autoneg is completed.
   - TSC PCS will reconfigure itself and the PMD for the new speed.
   - PMD will eventually lock. TSC-PCS will eventually link up.

\subsection Overriding native with Customer PCS Functions.

This capability is no longer supported. Please contact the TEMod team if you
need this. (ravick@broadcom.com)

*/ 
