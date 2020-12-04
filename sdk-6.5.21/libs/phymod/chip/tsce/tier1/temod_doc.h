
/*
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved.
$Id$
*/
/*!
\page temod-doc TEMod Documentation

\tableofcontents

\section temod-intro Introduction to TEMod/EagleMod

TSCE is a 28 nm quad link full physical layer interface designed to support link
speeds from 10M up to 42G. It consists of the TSC PCS sublayer and the
Eagle PMD sublayer.  TEMod, <b>T</b>DM Serdes controller <b>E</b>agle
<b>Mod</b>ule, is the software driver for TSCE. The Eagle is also used
standalone for non-PCS applications. EagleMod, <b>Eagle</b> <b>Mod</b>ule is the
software driver for Eagle.

TEMod is a source-code distributable C library that implements a hardware
abstraction layer for all the configuration modes of the 10G Broadcom phy IPs.
The phy can contain the PCS sub-layer working in concert with the PMD sub-layer,
(in which case the phy is called TSCE) or can be the PMD sub-layer only (in
which case the phy is called Eagle). The OSI reference model is shown below as a
quick reference. However this document expects the reader to be familiar with
not only general PHY architecture both the TSCE-PCS+PMD and the Eagle-PMD
architectures.

@image html osilayers.jpg "OSI LAYERS"

Acronyms used in this document are at http://www.broadcom.com/press/glossary.php

TEMod has an unambiguous interface to system platforms: Platforms can
be software platforms like MDK, SDK, customer specific software OR validation
platforms including HDL (verilog/systemVerilog/Vera) based testbenches.
TEMod has a tiered structure which progressively encapsulates register accesses
and sequences hiding complexities of register addressing, and access protocol
implementations.  The first layer, 'Tier1 functions' each partially configure
the TSCE. and are used to create comprehensive configurations in the
calling 'Tier2' layers. Tier2 layers are called by PHYMod dispatch layers which
are connected to PHYMod APIs. These APIs can be  called from customer code such
as BCM APIs  or diag shell commnds in Broadcom-SDK,

Tier1 procedures don't have a fixed argument list like in the past. They accept
variable number of arguments and return success/failure code. If additional
values need to be returned, calling layers pass references which the tier1s will
modify. For example, to get the serdes Identification, pass a integer pointer
to the tier1 which reads the serdes id.

All tier1 functions operate on a single core. It can be used to access multiple
lanes within the TSCE, sometimes simultaneously (a.k.a broadcast) but it cannot
access multiple TSCEs simultaneously. This is true even for TSCE12 (which is
built using 3 TSCE cores).

TEMod documentation is created using doxygen and follows the TEMod code
structure. The following files contain relevent documentation.

\li \ref temod_doc.h : Basic documentation not specific to the code. (this file)
\li \ref temod_cfg_seq.c : TSC PCS Tier1 sequences
\li \ref temod_enum_defines.c : Enumerated type related functions.
\li \ref temod_diagnostics.c : Diagnostic information of the PHY.
\li \ref temod_sc_lkup_table.c :  The speed look up table in PCS.
\li \ref eagle_tsc_interface.h :  Eagle PMD Tier1 sequences.

TEMod configures PHYs per logical port. Usually a specific lane of the port is
all that needs to be accessed to program the port.

-# lane 0 -> quad lane modes
-# lane 0/2 for dual modes
-# lane '0/1/2/3' for single lane modes

However the lanes can also be explicitly selected. The lane specific
information, when operating on multiple lanes, is encoded in an integer type
(usually 1 byte per lane). When the information doesn't 'fit' into 8 bits, we
pass in multiple arguments as required.

\section portmode-lane Port modes and lanes of TSCE

TSCE is a quad lane PHY. It supports one or more ports based on port-modes.

-# Single Port Mode
  -# All lanes combine to form a single port
-# Dual Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 and 3 combine to form another dual port
-# Tri1 Port Mode
  -# Lanes 2 and 3 combine to form a dual port
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
-# Tri2 Port Mode
  -# Lanes 0 and 1 combine to form a dual port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port
-# Quad Port Mode
  -# Lanes 0 forms a single lane port
  -# Lanes 1 forms a single lane port
  -# Lanes 2 forms a single lane port
  -# Lanes 3 forms a single lane port
-# Multi Core Mode (TSCE12 only)
  -# Three cores are used to form a single core.
  -# Each of these 3 cores can be independent (and have all portmodes above)
  -# They combine to form a 100G/106G port (10 lanes, 2 lanes unused)
  -# They combine to form a 120G/127G port (12 lanes)
-# QSGMII Port mode
  -# TBD.

We can have 1/2/3/4 ports per core. Each port can support different speeds.

\section regacc-sec TEMod Register Access Methods

TSCE does not use traditional MDIO bus protocol but instead uses the  TO_TSC
bus. TO_TSC is an 8 bit high speed bus, connected to the SBUS via an indirect
accessing scheme at chip level. It has a 32 bit address with the following
fields.

-# Bits 15:0   : 16-bit register address.
-# Bits 18:16 : AER mode. The ports and lanes affected are as follows.
  -# 0: Port 0 - one lane  (Logical Lane 0)
  -# 1: Port 1 - one lane  (Logical Lane 1)
  -# 2: Port 2 - one lane  (Logical Lane 2)
  -# 3: Port 3 - one lane  (Logical Lane 3)
  -# 4: Port 4 - two lanes (Logical Lanes 0 and 1)
  -# 5: Port 5 - two lanes (Logical Lanes 2 and 3)
  -# 6: Port 6 - four lane (Logical Lanes 0, 1, 2, and 3)
  -# 7: Undefined.
-# Bits 23:19 : Port ID. The TSCE has up to 4 ports.
  -# 0:Port 0 ID = prtad_strap_dst
  -# 1:Port 1 ID = prtad_strap_dst + 1
  -# 2:Port 2 ID = prtad_strap_dst + 2
  -# 3:Port 3 ID = prtad_strap_dst + 3
-# Bits 31:27 : Device ID (0 or PCS, 1 for PMD)

prtad_strap_dst is a strap used to identify an instance of a TSCE within a chip.
This value is inserted in the 23:19.

To broadcast to registers of 4 lanes, set 18:16 to '6'. There is another
elaborate broadcast mechanism involving a special register main0_prtad_bcast
(broadcast to ports whose main0_multiPRTs_en bit are set). Ignore that. It is an
artifact of TSC12 register access broadcast. The TSC12 does not have a 12 lane
broadcast facility, though this information was in the works at one time.

Typically the TSCE is embedded in port hardware. The PHY access is made by using
a port level interface (there are a couple: SBUS or PBUS) mechanism. This
mechanism writes to a memory (ucmem) in the port. Port logic will then translate
this to TSCE understandable TO_TSC protocol. This indirection is true for both
registers and memories. The next few sections explain the register and memory
access mechanisms from the port level.

\subsection bcst-sec The Port indirect address mechanism to access PHY

TSCE Register accesses are performed by accessing the PORT_WC_UCMEM_DATA memory in
the port sub-system. Software will have to access the memory and some
associated port registers.

\subsubsection irw TSC Register write.

-#  Program PORT_WC_UCMEM_CTRL.ACCESS_MODE to 0
  -# 0 to access the TSC registers
  -# 1 to access the TSC ucode memory
-#  Do a MEM_WR to the zeroth entry (16B or 128b) of PORT_WC_UCMEM_DATA memory
  -# 127-65 : Don't care
  -# 64     : 1 (reg. write)
  -# 63-48  : Write data
  -# 47-32  : Write Mask
  -# 31-0   : TSC Address

\subsubsection irr TSC Register Read

-#  Program PORT_WC_UCMEM_CTRL.ACCESS_MODE to 0
  -# 0 to access the TSC registers
  -# 1 to access the TSC ucode memory
-#  Do a MEM_WR to the zeroth entry (16B or 128b) of PORT_WC_UCMEM_DATA memory
  -# 127-65 : Don't care
  -# 64     : 0 (reg. write)
  -# 63-48  : Don't care
  -# 47-32  : Don't care
  -# 31-0   : TSC Address
-#  Do a MEM_RD to the zeroth entry (16B or 128b) of PORT_WC_UCMEM_DATA memory
  -# The read data will be in 47:32 of the zeroth entry.

\subsubsection imr TSC Memory Write

-#  Program PORT_WC_UCMEM_CTRL.ACCESS_MODE to 1
  -# 0 to access the TSC registers
  -# 1 to access the TSC ucode memory
-#  Do a MEM_WR to any of the 2K entries of PORT_WC_UCMEM_DATA memory
  -# TSC ucode mem.addr. is got from the PORT_WC_UCMEM_DATA memory entry no.
  -# 127-0  : 16 bytes of data

\subsubsection imw TSC Memory Read

-#  Program PORT_WC_UCMEM_CTRL.ACCESS_MODE to 1
  -# 0 to access the TSC registers
  -# 1 to access the TSC ucode memory
-#  Do a MEM_RD to any of the 2K entries of PORT_WC_UCMEM_DATA memory
  -# TSC ucode mem.addr. is got from the PORT_WC_UCMEM_DATA memory entry no.
  -# 127-0  : 16 bytes of data is returned on SBUS

\section regmacro-sec User Register Access

TEMod configures the PHY by accessing its registers. Since multiple platforms
adopt TEMod, it has a virtual register access method to read, write, and modify
registers.  TEMod uses a auto-generated macro set to identify registers
individually. The macros call these virtual routines which eventually attach to
platform specific register access methods, such as device driver functions or
verilog tasks.

The virtual register access function is described below using the 'read'
function as an example. Write and modify are similarly implemented.

\code
Register: PHYID2 (Adr:0x0002). This is a read only IEEE Id. register
READ_PHYID2r(pa, ...)
  -> phymod_tsc_iblk_read(pa, ...)
     -> PHYMOD_BUS_READ(pa, ...)
        -> phymod_bus_read(pa, ...)
           -> pa->read(...)
\endcode

The last function is actually a pointer to the read function supplied by the
platform. For most chips it will be a PCIE transaction which calles an internal
system bus (SBUS/PBUS for serial or parallel versions) protocol which the
hardware translates to TO_TSC protocol bus of TSCE. For chips that just use the
Eagle PHY (like FE3200), the the SBUS/PBUS translates to MDIO protocol. Note,
both PCS and PMD registers are accessed similarly, differing only in their devid
fields.

Some user level examples for broadcom-SDK diagshells are listed below. For more
details please refer to
http://confluence.broadcom.com/display/NTSWSW/PHYMOD+Simulator and click on
'SDK CLI with TSC/Eagle Simulator' The register access section has extensive
examples of how to access registers.

To read a PCS register, (0xabcd), do the following.
\code
BCM.0> phy xe0 0x0abcd  ## lane 0
BCM.0> phy xe0 0x1abcd  ## lane 1
BCM.0> phy xe0 0x2abcd  ## lane 2
BCM.0> phy xe0 0x3abcd  ## lane 3
\endcode

Similarly, to write a PCS register, (0xabcd), do the following.
\code
BCM.0> phy xe0 0x'n'abcd  ## lane 'n'
\endcode
To broadcast write a PCS register, (0xabcd), do the following.
\code
BCM.0> phy xe0 0x6abcd  ## bits 18:16 -> 6 means broadcast
\endcode

PHYMod supports symbolic register access. This makes it easier to read
registers without having to remember the addresses. Please note that the
register name is transformed to an easier name. This will be reflected in the
register documentation for TSCE.

Here are some examples of how to use symbolic names.

List the DME LOCK register. This gives the address and field info.
\code
BCM.0> phy xe0 AN_CL73_DME_LOCKr
Port xe0:
AN_CL73_DME_LOCKr [0x70109255] = 0x0000
        PD_DME_LOCK_TIMER_PERIOD<15:0>=0x0
\endcode

Let's say you know the address but not the symbolic name. You can do a reverse
lookup

\code
BCM.0> phy xe0 list 0x9255
Port xe0:
Name:     AN_CL73_DME_LOCKr (AN_X1_TIMERS_CL73_DME_LOCK)
Address:  0x9255 (1 copy only)
Fields:   1
        PD_DME_LOCK_TIMER_PERIOD<15:0>
\endcode

Now let's read the laneswap register. (Note: We will now only read the relevent
lane of the port. If there are two or four copies of these registers, you will
have access only to the active lane of that port. For example if this is a
single port mode  (like 40G) you will read the register in lane 0.

\code
BCM.0> phy xe0 raw LANE_SWAPr
Port xe0:
LANE_SWAPr [0x70109003] = 0x00e4
\endcode

Write to laneswap register
\code
BCM.0> phy xe0 LANE_SWAPr 0x1b
BCM.0> phy xe0 raw LANE_SWAPr
Port xe0:
LANE_SWAPr [0x70109003] = 0x001b
\endcode
Reset the register. i.e. reset the register to the advertised post-reset
default.
\code
BCM.0> phy xe0 reset LANE_SWAPr
\endcode
Verify that the reset took effect.
\code
BCM.0> phy xe0 raw LANE_SWAPr
Port xe0:
LANE_SWAPr [0x70109003] = 0x00e4
\endcode

Wild cards are allowed in symbolic access.
You can list all registers:
\code
BCM.0> phy xe0 list *
\endcode

You can even reset all registers.
\code
BCM.0> phy xe0 reset *
\endcode

\section funclist-set The TEMod Tier1 function list.

The list of Tier1 functions and documentation is in \ref temod_cfg_seq.c
and \ref eagle_tsc_interface.h

\section config-sec Configuration of the PHY

The TSCE PHY has two PHY sub-layer components. The PCS and the PMD sub-layer.
The PMD sub-layer is referred to as Eagle. The PCS layer is sometimes referred
to as TSCE or PCS. The PHY is configured by configuring both PCS and PMD sub
layers. The PHY driver in this case is called TEMod. In some applications where
the switch talks to the fabric interface, for example, only the PMD sub-layer is
used (aka Eagle). The PHY driver in this case is called EagleMod.

The term 'PMD' encompasses both PMD/PMA sub layers and AFE. The Eagle PMD used
in TSCE is a 40Gbps quad Serdes, basic configuration being a quad 10Gbps serdes
targeted for optical and backplane applications. It supports raw data rates from
6.25bps to 10.9375 Gbps.

\subsection pcscfg-sec PCS Configuration

PCS is configured while keeping the datapaths in reset.

-# reset
-# configure the PCS
-# release reset

PCS configuration is mainly dependent on the link baud rate.  This defines the
speed at which to run the PCS. Speed will also define the number of lanes and
portmode required.

The PCS configuration can be done in one of four ways.

-# canned configuration
-# canned+override configuration
-# custom configuragtion
-# Bypass configuration

TEMod only supports the first three methods. The fourth mode is used in debug
efforts outside of TEMods control.

\subsection cnspd canned configuration

PCS has a built in table (aka Speed config Table or SC table), for all supported
speeds. In canned configuration mode, TEMod selects the table entry of the
required speed and triggers the speed config. hardware in PCS to configure all
PCS sub-systems. This is the simplest mode of configuring the PCS. Note: A copy
of the SC table is stored in TEMod, mostly for verification and override
purposes. You can see it in \ref temod_sc_lkup_table.h as a structure
#sc_table_entry_t

\subsection cnospd canned+override speed

In canned+override speed mode PCS uses the SC table for supported speeds,
however but selectively overrides some of the configurations.

\subsection custspd custom speed

In custom speed mode PCS has four programmable SC table entries for four
custom speeds. TEMod can set the complete entries with custom configuration.

\subsection bypassspd bypass speed

In bypass configuration mode the PCS SC table is not used. All sub-systems in
PCS are to be configured by TEMod, and further the resets of PCS and PMD are to
be orchestrated by TEMod.  <b>TEMod does not support this mode</b>. Raw register
programming/scripts should be used for this purpose.

Once the PCS is fully configured by any of the 3 (non bypass) methods above,
TEMod writes to a speed register (SW_SPEED register) in PCS, and sets the
SW_SPEED_CHANGE bit.  This triggers the PCS to fully configure itself.

The next stage is to program the PMD.

\subsection epmdcfg-sec PMD Configuration

The comprehensive guide to Eagle PMD configuration is in the Eagle Programmers
Guide.
http://ingbu.broadcom.com/hsip/serdes/eagle/User%20Documentation/PMD/Eagle%20Programmers%20Guide.pdf

A significant improvement in the PMD architecture is reset management and the
relationship between resets and configuration. PMD supports the following reset
controls.

-# PMD hard reset. (power on) (comes from primary input only)
-# PMD core Datapath reset  (comes from primary input and PMD register)
-# PMD Lane Datapath reset (comes from primary input and PMD register)

All configuration to PMD is done while the datapaths are in reset. All
configuration is realized when PMD datapath is out of reset.

The resets are controlled by both PCS and by TEMod. This is shown below.

@image html pmd_resets.jpg "PMD RESETS"

The Eagle PMD IP can be used in two ways.

-# Independent (Standalone) (example SPDR2 chip)
-# Dependent (PCS linked)   (example Trident2 chip)

PMD is configured by a combination of TEMod and the PMD firmware (ucode). the
ucode gets its information either from TEMod or from a mailbox register
associated with the PCS. The configuration procedure depends on the link being
set via auto-negotiation or forced speed. So there are multiple
different procedures to bring the PMD to the required configuration (explained
further below)

Some mode/PCS agnostic PMD attributes are always configured by TEMod. For
details, please refer to the Eagle Programmer's guide.

As the general first step, TEMod takes PMD out of hard reset. This allows TEMod
to access the PMD registers. However all configuration is done with the PMD
datapaths kept in reset. The configuration takes effect, sort of atomically,
when TEMod or PCS takes PMD datapaths out of reset.

-# Take PMD out of hard reset
-# Keep PMD in  datapath reset
-# program core specific parameters
-# program lane specific parameters
-# Take PMD out of datapath reset

\subsubsection ind-prog-sec Independent Mode Programming

In this case, the TSC-PCS is not involved. The driver employed is 'EagleMod'.
The required PMD configuration is done entirely by EagleMod. To program the PMD.
The EagleMod sequence will look like so.

-# Hold the PMD in reset
-# Program the PMD as mentioned in \ref epmdcfg-sec
-# Unreset the PMD

\subsection dep-ecfg-sec PCS Dependent Mode PMD configuration

In dependent mode configuration, PMD and PCS together form a PHY.  If the PHY is
in forced speed mode PCS and PMD are fully configured by TEMod. In auto
negotiation mode PCS is programmed by TEMod and certain aspects of PMD
configuration are done by ucode using PCS hints.

Forced speed and auto negotiation links require different treatment. In forced
speed mode, PMD is configured entirely by TEMod. In auto negotiation modes, PMD
configuration must be done after auto negotiationotiation has resolved the link
rate. At this point TEMod cannot be involved, So PCS configures the PMD. The PCS
writes the configuration to a PMD register which is picked up by the ucode,
which then configures the PMD.  (ucode cannot program PMD without this info).

PCS sends both per core and per lane 20 bit encoding to ucode as shown below.

\li Core <b>dig_com_pmd_core_mode_status</b>:{Rsrvd[3:0], OTP_options[3:0], core_speed_id[7:0]}
\li Lane <b>ckrst_ctrl_pmd_pmd_lane_mode_status</b>:{Rsrvd[4:0], CL72_en, scrambler_dis, eee_mode_en, lane_speed_id[7:0]}

The PMD ucode reads the info in these registers, decodes it and configures PMD
blocks. The ucode is directed to either honor or ignore these register by TEMod.
TEMod provides this direction depending on the port being in auto negotiation or forced
modes. Two PMD fields are used for this purpose

\li core_config_from_pcs
\li lane_config_from_pcs

More details of this in the Eagle Programmer's guide.

@image html phy28-pcs-pmd-cfg.jpg "PCS PMD CONFIGURATION"

\subsection fs-mode-sec Forced speeds Mode

In forced speed mode, the PCS and PMD are configured completely by TEMod.  The
PHY configuration will be as shown below
reset.
-# Configure PMD as mentioned in \ref epmdcfg-sec
-# Program the PCS (use one of the init, set up speed)
-# (Wait for status update and read Status registers)
-# (Compare PCS Stats)
-# (PCS will release the PMD resets. PMD will start cranking)
-# (Wait for PLL_LOCK. If PMD is out of reset, pll will lock)
-# (Wait for PMD_LOCK. If PMD is out of reset and receeiving data PMD will lock)
-# Wait for PCS_LINK

-# Take PMD out of hard reset.
-# Reset the config_from_pcs (to 0), so PMD config is software controlled.
-# Reset PMD. This step is not needed if PCS is holding PMD in reset.
-# Program the PMD as mentioned in \ref epmdcfg-sec
-# Unreset PMD datapath. If PCS is holding PMD in reset, it will still be in reset.
-# Program the PCS (init, set up speed)
-# (Wait for status update and read Status registers)
-# (Compare PCS Stats)
-# (PCS will take PMD datapaths out of reset.)
-# (Wait PLL_LOCK)
-# (Wait PMD_LOCK)
-# (Wait for PCS_LINK)

Steps in parenthesis are not required, but are mentioned because they can
be added in the driver for diagnostics.

Note: QSGMII modes are different. It involves QSGMII PCS and not the TSCE PCS.

-# TEMod sets TSCE-PCS in ILKN mode. (bypasses datapath into QSGMII PCS)
-# TSC-PCS no longer controls the PMD resets.
-# TEMod configures PMD for required speed.
-# TEMod configures QSGMII-PCS (1G/100M/10M only for now)
-# (Wait for PLL_LOCK. If PMD is out of reset, pll will lock)
-# (Wait for PMD_LOCK. If PMD is out of reset and receeiving data PMD will lock)
-# QSGMII-PCS will come out of reset when it sees pmd_lock.

\subsection pmd-aneg-sec Auto-Negotiation Mode

In auto negotiation modes, the PMD is first minimally configured so that the AN pages can
be exchanged. Initially TEMod will set PMD for the relevant AN mode (CL37AN or
CL73AN).  It will keep PMD in reset, configure it, and unreset it. After this
the pages get exchanged and PCS will resolve the actual speed. It will now again
put the
PMD in reset, and reconfigure the PMD to the new resolved speed.

The TEMod programming sequence for AN will look like so.

-# TEMod configures PMD attributes.(for CL37 or CL73 exchange)
-# TEMod sets cfg_from_PCS bit. (i.e. future programming from PCS)
-# Unreset the PMD datapath.
-# Program the PCS (for CL37 or CL73 auto negotiationotiation)
-# TEMod creates a posedge on auto-neg_enable/restart bit in PCS.
-# (now AN page exchange happens and resolves to a certain speed)
-# (PCS will now reset the PMD)
-# (PCS will configure the PMD with the help of ucode.)
-# Wait for auto negotiation complete indication from PCS or PCS_LINK

-# Set the config_from_pcs (to 1), so PMD config is software controlled.
-# TEMod configures PMD.(for CL37 or CL73 exchange)
-# TEMod sets cfg_from_PCS bit. (i.e. future programming from PCS)
-# Unreset the PMD datapath.
-# Program the PCS (for CL37 or CL73 auto negotiationotiation)
-# TEMod starts auto negotiation via auto-neg_enable/restart bit in PCS.
-# (now AN page exchange happens and resolves to a certain speed)
-# (PCS will now reset the PMD)
-# (PCS will configure the PMD with the help of ucode.)
-# Wait for auto negotiation complete indication from PCS or PCS_LINK

\section els-sec Lane Swap

Lane swapping provides a means to associate any specific component of the MAC
byte stream to any physical PMD lane. In simpler terms any physical data stream
can be associated with any logical port by lane swapping. This association is
configurable in both PCS and PMD sub-layers.

The physical lanes in PMD can be switched to represent different logical lanes.
TX and RX are independently software controlled. In the Eagle PMD, only the TX
of lanes are swappable. RX is fixed.

The PCS laneswap configuration works slightly differently. It is paired so that
both TX and RX will be swapped at once.

For a complete laneswap solution both PCS and PMD laneswapping should be used.
The PCS should first be configured to 'fix' the RX. At this point, the RX paths
will be correct, but the TX will not. The PMD should now be configured to 'fix'
the TX. Thus, even though the PCS and PMD lane swapping are independent, TEMod
must ensure that swapping is meaningful.

Lane swapping also changes the configuration access. i.e. the physical lane
being configured will be different from the logical lane due to lane swapping.
Therefore swapping is configured while the datapath is in reset making it a
static configuration. Dynamic swapping is not supported.

As a simple example, consider the need to swap the physical RX lane 0 to logical
port 3. The PCS is first configured to swap the entire RX/TX Lane 0 pair to
logical port 3. The PMD is then configured to swap the tx of lane 3 to lane 0.

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

\image html phy28laneswap.jpg

\section loopback-usage Loopbacks

Loopbacks are controls to alter datapath for diagnostic purposes. At the PHY
level the following loopbacks are supported. The term 'gloop' mean global loop
backs and  'rloop' means remote loopback. In general loopbacks are supported for
all speed modes.  TSCE does not support loopbacks when autonegotiation is
enabled. TSCE does not support gloops when link training (CL72) is enabled.
For ports which use autonegotiation or link training, the port has to be
reconfigured to a similar forced speed configuration prior to loopback. When
loopback is removed, the original configuration can be reapplied.
For rloops, TSCE expects zero ppm offset with link partner. For non-zero PPM,
the ppm offset of lane 0 will be applied to all lanes, which may cause
inaccuracies.

- PCS-gloop: Loop back data path to system before entering PMD. All PCS blocks
  are included.
- PMD-gloop: Loop back data path to system before entering Analog
  serializers/deserializers. The entire PCS datapath is included along with PMD
  digital datapath.
- PMD-rloop: Loop back data path to line before entering PCS TSCE supports only
  PMD  remote loopbacks.
- PCS-rloop: Loop back data path to line before entering MAC TSCE does not
  support PCS remote loopbacks.

@image html loopbacks.jpg "Loopbacks in PHY"

\section prbs-usage PRBS and Pattern Generation.

TSCE supports PRBS generation(TX) and check(RX) on a per lane basis. The TX and
RX requiremens are decoupled. i.e. they can be on seperate PRBS patterns. This
is the default mode in setting PRBS.  Please refer to the PRBS section (4.3.1)
of the Eagle Programmer's guide for more details.

When PRBS is enabled, PCS cannot send/receive any data and the PCS link comes
down. The switch between PRBS and PCS is glitch free and controlled by TMod.
When we switch from PRBS back to PCS, PCS will establish the link again. In
other words TSCE can switch back and forth from PCS to PRBS. This is very
important for KR channels where link training happens in PCS mode. When
switching to PRBS, we need to do so without disrupting the trained
transmittors/receivers.

There are cases when the PCS is in single port mode (eg. 40G/42G MLD mode) where
a switch to PRBS will cause it not only to restart auto negotiation, but also
reset the PMD. If this is a trained link this will destroy the trained values
of the transmittors/receivers.

For single lane auto-neg lanes the problem is similar but slightly different.
Consider two partner trying to bring up PRBS on a link.  The first port to send
PRBS will bring the PCS link down on its partner, causing the partner to restart
auto negotiation.  This transition has a flatline which the The local partner
will receive  causing it to lose signal_detect.

For autonegotiated ports, therefore, we have to disable PCS from restarting
autonegotiation and resetting the PMD by overriding the PCS-to-PMD reset
controls. This is done by setting 'an_good_trap'. A feature that 'traps' the
PCS from completing OR restarting autonegiation.

Following sequence shows how to switch between PCS and PRBS. Autonegotiation
port variation is shown in parenthesis.

- (For AN) Run CL73 (e.g. 1G-KX/10GKR etc. mode)
- (For FS) Set up speed.
- Wait for PCS link up (Happens after speed negotiation and CL72 training)
- (For AN) Set an_good_trap to prevent AN from restarting.
- (For AN) Force signal detect
- Enable PRBS and check error count to reset the count.
- Run PRBS.
- Stop prbs
- (For AN) Release an_good_trap
- (For AN) Remove force signal detect
- Wait for PCS link up again.

We want the signal detect to be forced and an_good_trap to be set before PRBS is
enabled on the remote partner. This forces the PRBS sequence to have 3 steps,
which is a departure from previous procedures

- PRBS_PREP (an_good_trap/sig_det)
- PRBS_SET  (poly, yada yada)
- PRBS_GET (link, error count)

An example sequence of running PRBS in forced speeds using Broadcom SDK diag
shell is shown below.
\code
# Put xe0 and xe1 are in 10G Forced speed mode.
BCM.0> port xe0 an=f sp=10000
BCM.0> port xe1 an=f sp=10000
BCM.0> ps xe0,xe1
...
BCM.0>
BCM.0> link off
BCM.0> PHY diag xe0 prbs set p=1
BCM.0> PHY diag xe1 prbs set p=1
BCM.0>
BCM.0> PHY diag prbs xe0 get
[you may get errors the first time. Ignore output]
BCM.0> PHY diag xe0 prbs get
xe0 : PRBS OK!
BCM.0> PHY diag xe1 prbs get
[you may get errors the first time. Ignore output]
BCM.0> PHY diag xe1 prbs get
xe1 : PRBS OK!
BCM.0> PHY diag xe0 prbs clear
BCM.0> PHY diag xe1 prbs clear
\endcode

A similar example sequence of running PRBS, but in autoneg mode is shown below.
{Incomplete. FIXIE}
\code
# Put xe0 and xe1 are in Autoneg. Assume they will come up
BCM.0> port xe0 an=t ...
BCM.0> port xe1 an=t ....
BCM.0> ps xe0,xe1
...
BCM.0>
BCM.0> link off
BCM.0> PHY diag xe0 prbs set p=1
BCM.0> PHY diag xe1 prbs set p=1
BCM.0>
BCM.0> PHY diag prbs xe0 get
[you may get errors the first time. Ignore output]
BCM.0> PHY diag xe0 prbs get
xe0 : PRBS OK!
BCM.0> PHY diag xe1 prbs get
[you may get errors the first time. Ignore output]
BCM.0> PHY diag xe1 prbs get
xe1 : PRBS OK!
BCM.0> PHY diag xe0 prbs clear
BCM.0> PHY diag xe1 prbs clear
\endcode

\section cnotaft Configuration of CL72(training) and CL74(FEC) for TSCE

CL72(Training) and CL74(FEC) are enabled to improve channel signal integrity.
CL72 is physically implemented in the PMD sub-layer. CL74 is physically
implemented in the PCS sub-layer. However these features are dependant on link
data rates as well. So they are controlled by PCS or TMod as needed.  CL72 and
CL74 are closely related features and hence are discussed together in this
section.  However the controls for these features are not related and
independently configured. The defaults, and controls are different for auto
negotiation and forced speed modes. The controls can be static. For the
SDK platform, the controls can be (config or SOC properties) or
dynamic(BCM-APIs).

A general rule of thumb is that features are controlled by software (TMod) in
forced speed modes and by PMD microcode (ucode) in auto negotiation modes.
They will eventually operate on the same set of registers, but the choice is
made by the response time required. In auto-negotiation modes, where the
configuration is known only after autonegotiation completes.

We will discuss the differences in controls in the next two sub-sections.

\subsection fsccl72774 Forced Speeds Mode Control of CL72 and CL74

CL72 and CL84 are disabled by default for all forced speeds. It can
be enabled for some speeds. This is not really an IEEE mandate but a Broadcom
proprietary feature. Currently these features are  supported for 10G/10.5G
20G/21G, 40G/42G speeds. Users can control these features dynamically.

\subsubsection lnktrn Link Training (CL72)

As mentioned before users can control CL72 dynamically. As an example, for SDK,
CL72 can be controlled dynamically by BCM APIs.

\code
bcm_port_phy_control_set(0 /*unit*/,
                         p /*port*/,
                         BCM_PORT_PHY_CONTROL_CL72,
                         en /*0 -> disable, 1 -> enable */);
\endcode

\subsubsection fec-cntl FEC control (CL74)

The platform is assumed to know the speeds for which CL74 should be enabled.
Since enabling CL74 for non-CL74 speeds can have unpredictable results, hardware
has additional controls to simply ignore CL74 controls for non-CL74 speeds.
As an example, for SDK, CL74 can be controlled dynamically by BCM APIs.

\code
bcm_port_phy_control_set(0/*unit*/,
                         p/*port*/,
                         BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION,
                         en /*0 -> disable, 1 -> enable */);
\endcode

\subsection anccl72774 Auto-Negotiation Mode Control of CL72 and CL74

CL37 auto negotiation cannot use CL72 or CL74. They are only advertisable in
CL73 and CL37-BAM (proprietary) auto negotiation. In auto negotiation, the need to enable
these features are only known after auto negotiationotiation and speed resolution. Hence
these controls are handled by hardware/ucode rather than TMod.

\subsubsection anlnktrn Link Training (CL72)

Link training is enabled by default for CL73. It is not dynamically controlled.
It is defined as a static 'config' property, similar to the port auto
negotiation property.

\code
/* following two configs disable CL37 and enable CL73.  */
phy_an_c73_xe1=1 /* CL73 an port */
phy_an_c37_xe1=0 /* Not a CL37 an port */
/* following config enables CL72 */
phy_an_c72_xe=1
/* following config disables CL74 */
phy_an_c72_xe=0
\endcode

\subsubsection anfecctl FEC Control (CL74)
If the speed supports FEC, than FEC can be advertised. If both partners advertse
FEC, the link is established with FEC enabled. If one of them does <b>not</b>
advertise FEC, the link will <b>still</b> come up but with FEC disabled. So FEC
is controlled by simply not advertising FEC.

\code
/* following two configs disable CL37 and enable CL73.  */
phy_an_c73_xe1=1 /* CL73 an port */
phy_an_c37_xe1=0 /* Not a CL37 an port */
/* following config enables FEC */
phy_an_fec_xe=1
/* following config disables FEC */
phy_an_fec_xe=0
\endcode

\section pcs-func-ovr Overriding native with Customer PCS Functions.

This capability is no longer supported. Please contact the TEMod team if you
need this. (ravick@broadcom.com)

\section eyescan EyeScan

Eyescan utility measures link robustness. The SERDES slicer can be perturbed
horizontally and vertically and subsequently measure the BER at different
perturbations. Eyescan can be measured both intrusive and non-intrusive method.
In case of non-intrusive method - the link continues to be live and the traffic
can run normally. By default eyescan is running in a non-intrusive method.
In case of intrusive method the traffic is sent (mostly PRBS using internal
generator or the other link partner needs to generate the PRBS) and the
error rates are recorded for a suite of perturbations and practical BER and
margins can be extrapolated using linear fit in the Q-function domain.

The slicer perturbation sweep can be both horizontal and vertical. The smallest
variation of the horizontal and vertical (there are 64 in each 'direction) is
called the 'step'. The perturbations are done across a range, defined by the
minimum and maximum values of horizontal and vertical perturbations.

When sweeps in both directions are performed, the eyescan is termed 2D (two
dimensional). In many cases the horizontal is fixed and only a vertical is
performed. This is termed 1D (one dimensional).

The general high level sequence to execute Eyescan is

- Bring links to required rates.
- Enable traffic (PRBS) for intrusive mode. This will also bring down the link.
- Run Eyescan code <-- there are different types as explained next.
- Stop PRBS and switch back links to mission mode (original configuration) for
   intrusive mode only.

TEMod, built on top of eagle PMD software infrastructure provides two types of
eyescan information. (There is much nomenculator confusion)
- The HighBER, or fast eyescan, or Type 2 eyescan
- The LowBER, or custom eyescan or Type 1 eyescan.
The next subsections will explain both methods in more detail.

\subsection highber Fast Eyescan
  This method is controlled by the ucode.  All ranges, sample times and steps
are fixed. The ucode generates the 2D info and stores it in ucode RAM and is
extracted by software and provided as a 2D integer array to customer. It can
also be printed on the screen as ASCII graphics. The ucode can be instructed to
generate eyes on one or more lanes and the data acquisition is parallel. By default
the commands run fast eyescan mode. Fast eyescan is supported in non-intrusive mode
only.

\code
The commands are

* init all  OR rc. whatever method to bring the ports up.
* ps
* Phy diag xe eyescan

\endcode

\subsection lowber Custom Eyescan
This method allows flexibility to select the steps, range, sample time etc.
It is generally slower but potentially more accurate and controllable. The ucode
is setup per user's needs and it records the errors which software will extract.
into a 2D integer array.  It can be printed on the screen as well

The newer implementation has 2 slicers and so the link can continue to be live while
the second slicer can be used  to check the eyescan. By default the eyescan is run on
the second slicer (passive mode) so there is no instrusion on the regular data traffic.
The eyescan can be run on the main link but this will be intrusive. the support is
available to run lowber custom eyescan on the live link. This requires the user to run
PRBS prior to Eyescan.
\code
The commands for custom eyescan in intrusive mode are

1) Init all  OR rc. whatever method to bring the ports up.
2) ps
3) Linkscan off.
4) Phy diag xe prbs set p=5
5) Phy diag xe prbs get
6) Phy diag xe prbs get
7) Phy diag xe eyescan  type=3
8) Phy diag pbm prbs clear

The commands for custom eyescan in non-intrusive mode are

1) Init all  OR rc. whatever method to bring the ports up.
2) ps
3) Phy diag xe eyescan  type=2
\endcode

\subsection eyes_cntl Eyescan controls

At the User Interface (using SDK as an example), these parameters control the
generation of the eye.
- The type of Eyescan
 -# type=1  request fast eyescan (this is default)
 -# type=2  request custom eyescan
 -# type=3  request custom eyescan on live link.
- flag
  -# '1' implies 1D
  -# if not provided, implies 2D
  -# no other values are valid
- The limits of the sweep (defines the range)
  -# vertical_max
  -# vertical_min
  -# horizontal_max (specify this alone if requesting 1D)
  -# horizontal_min
- the resolution (i.e. steps)
  -# sample_resolution (fixed for both Vertical and horizontal simultaneously)
  -# sample_resolution_v (to provide seperate vertical step)
- counter
  -# defunct.
- sample_time, time to run traffic to collect BER specified in ms

\code
Here is a sample of eyescan output.

  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30
         : -|----|----|----|----|----|----|----|----|----|----|----|----|-
   225mV : 1111111111111111111111111111122222211111111111111111111111111
   213mV : 1111111111111111111111111222223333322221111111111111111111111
   200mV : 1111111111111111111111122233344455443322221111111111111111111
   188mV : 1111111111111111111112223344556677765433222211111111111111111
   175mV : 111111111111111111112233456677-+----7654332222111111111111111
   163mV : 1111111111111111112233456 :    :    : 76543322211111111111111
   150mV : 111111111111111112234567  :    :    :   654332222111111111111
   138mV : 1111111111111111223457    :    :    :    76543222211111111111
   131mV : 111111111111111223356:    :    :    :    :7543322221111111111
   125mV : 111111111111111223457+----+----+----+----+-654332221111111111
   119mV : 11111111111111223456 :    :    :    :    :7764432222111111111
   113mV : 11111111111111223457 :    :    :    :    :  65433222211111111
   106mV : 1111111111111223456  :    :    :    :    :  76543322211111111
   100mV : 1111111111111223467  :    :    :    :    :   7543322221111111
    94mV : 111111111111223457---+----+----+----+----+---7654332221111111
    88mV : 111111111111223467   :    :    :    :    :    764432222111111
    81mV : 11111111111223456    :    :    :    :    :    :65433222111111
    75mV : 11111111112223457    :    :    :    :    :    :76543222211111
    69mV : 1111111111223356:    :    :    :    :    :    : 6543322221111
    63mV : 11111111112234577----+----+----+----+----+----+-7654332221111
    56mV : 1111111112233467:    :    :    :    :    :    : 7754432222111
    50mV : 111111112223457 :    :    :    :    :    :    :  765433222211
    44mV : 111111112233467 :    :    :    :    :    :    :  775443222221
    38mV : 11111112223457  :    :    :    :    :    :    :   76543322222
    31mV : 11111112233467--+----+----+----+----+----+----+----7554332222
    25mV : 1111112223457   :    :    :    :    :    :    :    7654332222
    19mV : 1111112233467   :    :    :    :    :    :    :    :664433222
    13mV : 111111223456    :    :    :    :    :    :    :    : 65433322
     6mV : 111112223457    :    :    :    :    :    :    :    : 76543322
     0mV : 111112233567----+----+----+----+----+----+----+----+-76543332
    -6mV : 11111223346:    :    :    :    :    :    :    :    : 76543322
   -13mV : 111112223457    :    :    :    :    :    :    :    : 65443322
   -19mV : 1111112234567   :    :    :    :    :    :    :    :765433222
   -25mV : 1111112223457   :    :    :    :    :    :    :    7654332222
   -31mV : 1111111223456---+----+----+----+----+----+----+----7654332222
   -38mV : 11111112223457  :    :    :    :    :    :    :   76543322221
   -44mV : 111111112234567 :    :    :    :    :    :    :   75443222221
   -50mV : 11111111223345  :    :    :    :    :    :    :   65433222211
   -56mV : 1111111112234577:    :    :    :    :    :    :  654432222111
   -63mV : 1111111112233467+----+----+----+----+----+----+-7654332222111
   -69mV : 1111111111223457:    :    :    :    :    :    :76543322221111
   -75mV : 11111111112233467    :    :    :    :    :    :76543322211111
   -81mV : 11111111111223457    :    :    :    :    :    :65433222111111
   -88mV : 11111111111223346    :    :    :    :    :    765433222111111
   -94mV : 111111111111223457---+----+----+----+----+----654332221111111
  -100mV : 1111111111112233567  :    :    :    :    :   7644322221111111
  -106mV : 1111111111111223457  :    :    :    :    :   6543322211111111
  -113mV : 11111111111112233567 :    :    :    :    :  75433222211111111
  -119mV : 11111111111111223457 :    :    :    :    : 765432222111111111
  -125mV : 11111111111111223356-+----+----+----+----+-754332221111111111
  -131mV : 111111111111111223457:    :    :    :    :7644322221111111111
  -138mV : 1111111111111112223467    :    :    :    76543322211111111111
  -150mV : 11111111111111111223456   :    :    :   765432222111111111111
  -163mV : 1111111111111111112234567 :    :    :  6543322211111111111111
  -175mV : 1111111111111111111223345667---+----+765432222111111111111111
  -188mV : 11111111111111111111122334455677  776543322211111111111111111
  -200mV : 1111111111111111111111222333445555554332221111111111111111111
  -213mV : 1111111111111111111111112222233333332222111111111111111111111
  -225mV : 1111111111111111111111111112222222222111111111111111111111111
  -238mV : 1111111111111111111111111111111111111111111111111111111111111
  -250mV : 1111111111111111111111111111111111111111111111111111111111111
         : -|----|----|----|----|----|----|----|----|----|----|----|----|-
  UI/64  : -30  -25  -20  -15  -10  -5    0    5    10   15   20   25   30

\endcode
\section eee-phy EEE Features

When no information is being transmiited, upper layers can provide a hint to PHY
layers to transmit low power idles (LPIs).  EEE supported PHYs can shutdown
until 'woken up' by the network.  TSCE does <b>not</b> support full featured
EEE. It only supports partial EEE, called the pass-thru mode. It can transfer
LPIs to the upper layers only but doesn't do any power optimization. It can also
convert LPIs to idles so upper layers do not see them. PHYMod controls the
ability to convert LPIs thus.

- eee_control_set(...,  enable)
  - enable = 1 : Allow LPI pass through. i.e. do not convert LPI.
  - enable = 0 : Convert LPI to idle. So  MAC will not see it.
- eee_control_get(..., *info)
  - info = 1 : LPI is going to pass through. i.e. no conversion to LPI
  - info = 0 : LPI is being converted. So MAC will not see it.
- Default: EEE is disabled. i.e. LPI will get converted to Idle.

\section tx-rx-ctl PHY TX and RX control

The TX and RX paths of the entire TSCE can be independently controlled. There
are various components including
- Disable: This will disable PCS from generating any data. It will also not
  receive any information from the MAC.
- Reset: This will reset the PCS. This will flush the FIFOs, reset the PMD, and
  stop the PCS from receiving data from the MAC.
- Squelch: This will keep PCS active (i.e. PCS will transmit data) while
  squelching the actual transmission at the PMD/Analog level.
- Power down: Power down the blocks.

Any of these can be independantly controlled by a couple of PHYMod APIs

-# Phymod_phy_power_set: Power down TX, RX or both including pcs and PMD
-# Phymod_phy_tx_lane_control:  Enum types control TX as follows
  -# traffic disable/enable
  -# reset/unreset
  -# squelch/unsquelch
-# Phymod_phy_rx_lane_control: Enum types control RX as follows.
  -#     rx reset
  -#     rx squelch/unsquelch

When platforms (eg SDK) disable/enable a port, TX/RX squelch on/off is applied.

\section trg_phy_cfg_dis Phy Configuration Triage Reports

TEMod supports category based diagnostics printouts in the diagnostics prompt.
Currently the following categories exist.

-# TOPOLOGY: loopbacks/swaps/polarity/port modes
-# LINK STATE: sigdet/pll/pmd_lock/pcs_block/pcs_linkup
-# AUTONEG: type, abilities, resolved speed, link
-# SPEED:  speed set, hard table overrides, soft tables
-# TFC: PRBS/pkt_gen/PRTP/traffic details (only if enabled)
-# AN TIMERS: Various AN timers.

With the command below, we can get a report of the link state
\code
BCM.0> phy diag xe0 pcs link

+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01    LINK STATE                   |   LH    |   LL   |
+-----------------------+-------------------+---------------+---------+--------+
| PMD PLL LOCK   : Y    | PCS SYNC   : Y    | PCS SYNC STAT : 0000    : 0000   |
| PMD SIG DETECT : NNYY | PCS LINK   : Y    | PCS LINK STAT : 0000    : 0000   |
| PMD LOCKED     : NNYY | PCS HI BER : 0000 | PCS HIGH BER  : 0000    : 0000   |
| PMD LATCH HI   : 0000 | PCS DESKEW : 0000 | PCS DESKEW    : 0000    : 0000   |
| PMD LATCH LO   : 0000 | PCS AMLOCK : 0000 | PCS AM LOCK   : 0000    : 0000   |
| RXLOCK LATCH HI: 0000 |                   |                                  |
| RXLOCK LATCH LO: 0000 |                   |                                  |
+-----------------------+-------------------+----------------------------------+
\endcode

With the command below, we can get a report of various speed parameters

\code
BCM.0> phy diag xe0 pcs speed
+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01    SPEED                                           |
+---------------------+-----------------------------+-----------+--------------+
| ST [0]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [1]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [2]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [3]              | SPD_ILLEGAL                 | NUM LANES : 0            |
+---------------------+-----------------------------+-----------+--------------+
|                          OEN SET OVR VALUE                                   |
+----------------+----------------+--------------------------------------------+
| NUM LANES: 0x0 | FEC ENA  : 0x0 | PMA_OS_MODE_1                              |
| 64B66DECR: 0x0 | CHKEND   : 0x1 | SCR_MODE_BYPASS                            |
| FECENABL : 0x0 | REORDER  : 0x0 | ENCODE_MODE_NONE                           |
| CL36ENA  : 0x0 | SGMIISPD : 0x0 | R_DESCR1_MODE_BYPASS                       |
| CLKCNT0  : 0x0 | CLKCNT1  : 0x0 | DECODER_MODE_NONE                          |
| LP CNT0  : 0x0 | LP CNT1  : 0x0 | R_DESKEW_MODE_BYPASS                       |
| MACCRDGEN: 0x0 | REPLCNT  : 0x0 | DESC2_MODE_NONE                            |
| PCSCRDENA: 0x0 | CLK CNT  : 0x0 | R_DESC2_BYTE_DELETION_100M                 |
| PCSCRDGEN: 0x0 |                | BLOCKSYNC_MODE_NONE                        |
+----------------+--+-------------+------+------------------+------------------+
|        SPEED      |        STATS0      |      STATS1      |   CREDIT STATS   |
+-------------------+--------------------+------------------+------------------+
| SPD CHG VLD  0    |     OS MODE 1      | DESCR MODE BYPASS| SGMII SPD : 0000 |
| SPD CHG DONE 0    |    SCR MODE 64B    |DECODE MODE CL49  |  CLK CNT0 : 0033 |
| SPD RESOLVED 0028 |    ENC MODE CL49   |DESKEW MODE BYPASS|  CLK CNT1 : 0000 |
| #LN RESOLVED 0000 |BLKSYNC MODE CL49   |DESCR2 MODE CL49  |   LP CNT0 : 0001 |
| PLL DIV      0010 |   CL72  ENA : 01   |  BYTE  DEL NONE  |   LP CNT1 : 0000 |
| REF CLOCK    0000 | CHKEND  ENA : 00   |64b66DEC EN 0     |  MAC  CGC : 0004 |
|                   |    FEC  ENA : 00   |                  |  REP  CNT : 0000 |
|                   |REORDER  ENA : 00   |                  |PCS CRD EN : 0000 |
|                   |   CL36  ENA : 00   |                  |PCS CK CNT : 0000 |
|                   |                    |                  |CRDGEN CNT : 0000 |
+-------------------+--------------------+------------------+------------------+

\endcode

With the command below, we can get a report of various auto-negotiation parameters

\code
BCM.0> phy diag xe0 pcs aneg
+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01     AUTONEG                                        |
+-------------+-------------+------------------------------+-------------------+
| AN37: N     | AN73 : N    | AN HCD SPD : 1000M           |  AN LINK : DN     |
+-------------------+-------+------+-----------------------+-------------------+
| ANX4 CTRL: 0x0000 | ENS : 0x0000 | CL37 BAM:0x0000 BASE :0x0000              |
| ANX4 OVR0: 0x0000 | OVR1: 0x0000 | CL73 BAM:0x0000 BASE1:0x0000 BASE0:0x02A0 |
+-------------------+--------------+----+--------------------------------------+
|      CLAUSE 37    |      CLAUSE 73    |                                      |
+-------------------+-------------------+--------------------------------------+
| BAM ENA       : 0 | BAM  ENA     : 0  | NUM ADV LANES : 1                    |
| AN  ENA       : 0 | AN   ENA     : 0  | FAIL COUNT LIM: 0                    |
| SGMII ENA     : 0 | HPAM ENA     : 0  |                                      |
| BAM2SGMII ENA : 0 | BAM3HPAM ENA : 0  |                                      |
| SGMII2CL37 ENA: 0 | HPAM2CL73 ENA: 0  |                                      |
| AN RESTART    : 0 | AN RESTART   : 0  |                                      |
+-------------------+-------------------+--------------------------------------+
|            CL37 ABILITIES             |         CL73 ABILITIES               |
+---------------+-----------------------+-----------------+--------------------+
| SWRST DIS : 0 | ANRST DIS    :0       | NONCE OVR : 0   | NONCE VAL: 0       |
| PD 2 CL37 : 0 | NEXT PAGE    :0       | TX NONCE  : 0x15| BASE SEL : Rsvd    |
| HALF DUPLX: 0 | FULL DUPLEX  :0       | NEXT PAGE  : 0  | FEC      : 0       |
| PAUSE     : 0 | SGMII MASTER :0       | REMOTE FLT : 0  | PAUSE    : 0       |
| SGMII FDUP: 0 | SGMII SPD    :10Mb/s  |-----------------+--------------------|
| OVR1G ABIL: 0 | OVR1G PAGECNT:0       | 1000BASE KX : 0 | 10GBASE KX4 :0     |
| BAM CODE      : 0x0000                | 10GBASE KR  : 0 | 10GBASE KR4 :0     |
|---------------+-----------------------| 40GBASE CR4 : 0 | 100GBASE CR1:0     |
|                                       | HPAM_20GKR2 : 0 | BAM CODE    :0x0000|
|                                       | 20GBASE CR2 : 0 | 20GBASE KR2 : 0    |
+---------------------------------------+--------------------------------------+
|                               OVER1G ABILITIES                               |
+-------------------+--------------------+-----------------+-------------------+
| HG2         : 0   | FEC          : 0   | CL72         : 0|                   |
| 40GBASE X4  : 0   | 32P7GBASE X4 : 0   | 31P5GBASE X4 : 0| 25P455GBASE X4: 0 |
| 21GBASE X4  : 0   | 20GBASEX2 CX4: 0   | 20GBASE X2   : 0| 20GBASE X4    : 0 |
| 16GBASE X4  : 0   | 15P75GBASE X2: 0   | 15GBASE X4   : 0| 13GBASE X4    : 0 |
+-------------------+--------------------+-----------------+-------------------+
\endcode

With the command below, we can get a report of various Traffic sub-configurations

\code
BCM.0> phy diag xe0 pcs tfc
+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01    INTERNAL TFC                                    |
+------------------------------------------+-----------------------------------+
| Traffic_type: MAC                      |                                     |
+------------------------------------------+-----------------------------------+
\endcode

With the command below, we can get a report of various auto-negotiation timers

\code
BCM.0> phy diag xe0 pcs antimers
+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01    AN TIMERS                                       |
+--------------------------------------+---------------------------------------+
| CL37 RESTART          : 0x0000029A   | CL37 ACK               : 0x0000029A   |
| CL37 ERR              : 0x00000000   | CL37 LINK BREAK        : 0x000010ED   |
| CL73 ERR              : 0x00000000   | CL73 DME LOCK          : 0x000014D4   |
| LINK_UP               : 0x0000029A   | PS SD                  : 0x00000a6a   |
| SYNC STATUS           : 0x0000029A   | PD TO CL37             : 0x00000A6A   |
| IGNORE LINK           : 0x0000029A   | SGMII                  : 0x0000006B   |
| DME PAGE MIN          : 0x0000005F   | DME PAGE MAX           : 0x00000076   |
| FAIL INHIBIT W/O CL72 : 0x000014D5   | FAIL INHIBIT WITH CL72 : 0x00008382   |
+--------------------------------------+---------------------------------------+
\endcode

With the command below, we can get a report of PCS state machines

\code
BCM.0> phy diag xe0 pcs state
+------------------------------------------------------------------------------+
|                                 DEBUG STATE                                  |
+--------------------------------------+---------------------------------------+
| SC_DEBUG_STATE    : 0x0000ef74       |  FSM_STATUS : 0x0000EF74              |
| TLA_SEQUENCER STS : 0x00000001       |                                       |
+--------------------------------------+---------------------------------------+
\endcode

With the catchall command below, we can get a comprehensive report of all PCS.
This is ssentially all the previous commands.

\code
BCM.0> phy diag xe2 pcs
+------------------------------------------------------------------------------+
| TRG ADR : 000165 LANE: 01     TOPOLOGY                                       |
+-------------------+--------------------------+---------------+---------------+
| PCSLCL LPBK: NNNN | PCS LANE SWAP L2P : 3210 | TX POLARITY : 0 | PORT NUM : 0|
| PCSRMT LPBK: NNNN | PMD LANE ADDR IDX : 3210 |                 | SNGLMODE : 0|
| PMDDIG LPBK: NNNN | PMD TO AFE        : 3210 | RX POLARITY : 0 | PORT MODE: 0|
| PMDREM LPBK: NNNN |                          |                               |
+-------------------+--------------------------+-------------------------------+
| TRG ADR : 000165 LANE: 01    LINK STATE                   |   LH    |   LL   |
+-----------------------+-------------------+---------------+---------+--------+
| PMD PLL LOCK   : Y    | PCS SYNC   : Y    | PCS SYNC STAT : 0000    : 0000   |
| PMD SIG DETECT : NNYY | PCS LINK   : Y    | PCS LINK STAT : 0000    : 0000   |
| PMD LOCKED     : NNYY | PCS HI BER : 0000 | PCS HIGH BER  : 0000    : 0000   |
| PMD LATCH HI   : 0000 | PCS DESKEW : 0000 | PCS DESKEW    : 0000    : 0000   |
| PMD LATCH LO   : 0000 | PCS AMLOCK : 0000 | PCS AM LOCK   : 0000    : 0000   |
| RXLOCK LATCH HI: 0000 |                   |                                  |
| RXLOCK LATCH LO: 0000 |                   |                                  |
+-----------------------+-------------------+----------------------------------+
| TRG ADR : 000165 LANE: 01     AUTONEG                                        |
+-------------+-------------+------------------------------+-------------------+
| AN37: N     | AN73 : N    | AN HCD SPD : 1000M           |  AN LINK : DN     |
+-------------------+-------+------+-----------------------+-------------------+
| ANX4 CTRL: 0x0000 | ENS : 0x0000 | CL37 BAM:0x0000 BASE :0x0000              |
| ANX4 OVR0: 0x0000 | OVR1: 0x0000 | CL73 BAM:0x0000 BASE1:0x0000 BASE0:0x02A0 |
+-------------------+--------------+----+--------------------------------------+
|      CLAUSE 37    |      CLAUSE 73    |                                      |
+-------------------+-------------------+--------------------------------------+
| BAM ENA       : 0 | BAM  ENA     : 0  | NUM ADV LANES : 1                    |
| AN  ENA       : 0 | AN   ENA     : 0  | FAIL COUNT LIM: 0                    |
| SGMII ENA     : 0 | HPAM ENA     : 0  |                                      |
| BAM2SGMII ENA : 0 | BAM3HPAM ENA : 0  |                                      |
| SGMII2CL37 ENA: 0 | HPAM2CL73 ENA: 0  |                                      |
| AN RESTART    : 0 | AN RESTART   : 0  |                                      |
+-------------------+-------------------+--------------------------------------+
|            CL37 ABILITIES             |         CL73 ABILITIES               |
+---------------+-----------------------+-----------------+--------------------+
| SWRST DIS : 0 | ANRST DIS    :0       | NONCE OVR : 0   | NONCE VAL: 0       |
| PD 2 CL37 : 0 | NEXT PAGE    :0       | TX NONCE  : 0x15| BASE SEL : Rsvd    |
| HALF DUPLX: 0 | FULL DUPLEX  :0       | NEXT PAGE  : 0  | FEC      : 0       |
| PAUSE     : 0 | SGMII MASTER :0       | REMOTE FLT : 0  | PAUSE    : 0       |
| SGMII FDUP: 0 | SGMII SPD    :10Mb/s  |-----------------+--------------------|
| OVR1G ABIL: 0 | OVR1G PAGECNT:0       | 1000BASE KX : 0 | 10GBASE KX4 :0     |
| BAM CODE      : 0x0000                | 10GBASE KR  : 0 | 10GBASE KR4 :0     |
|---------------+-----------------------| 40GBASE CR4 : 0 | 100GBASE CR1:0     |
|                                       | HPAM_20GKR2 : 0 | BAM CODE    :0x0000|
|                                       | 20GBASE CR2 : 0 | 20GBASE KR2 : 0    |
+---------------------------------------+--------------------------------------+
|                               OVER1G ABILITIES                               |
+-------------------+--------------------+-----------------+-------------------+
| HG2         : 0   | FEC          : 0   | CL72         : 0|                   |
| 40GBASE X4  : 0   | 32P7GBASE X4 : 0   | 31P5GBASE X4 : 0| 25P455GBASE X4: 0 |
| 21GBASE X4  : 0   | 20GBASEX2 CX4: 0   | 20GBASE X2   : 0| 20GBASE X4    : 0 |
| 16GBASE X4  : 0   | 15P75GBASE X2: 0   | 15GBASE X4   : 0| 13GBASE X4    : 0 |
+-------------------+--------------------+-----------------+-------------------+
| TRG ADR : 000165 LANE: 01    SPEED                                           |
+---------------------+-----------------------------+-----------+--------------+
| ST [0]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [1]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [2]              | SPD_ILLEGAL                 | NUM LANES : 0            |
| ST [3]              | SPD_ILLEGAL                 | NUM LANES : 0            |
+---------------------+-----------------------------+-----------+--------------+
|                          OEN SET OVR VALUE                                   |
+----------------+----------------+--------------------------------------------+
| NUM LANES: 0x0 | FEC ENA  : 0x0 | PMA_OS_MODE_1                              |
| 64B66DECR: 0x0 | CHKEND   : 0x1 | SCR_MODE_BYPASS                            |
| FECENABL : 0x0 | REORDER  : 0x0 | ENCODE_MODE_NONE                           |
| CL36ENA  : 0x0 | SGMIISPD : 0x0 | R_DESCR1_MODE_BYPASS                       |
| CLKCNT0  : 0x0 | CLKCNT1  : 0x0 | DECODER_MODE_NONE                          |
| LP CNT0  : 0x0 | LP CNT1  : 0x0 | R_DESKEW_MODE_BYPASS                       |
| MACCRDGEN: 0x0 | REPLCNT  : 0x0 | DESC2_MODE_NONE                            |
| PCSCRDENA: 0x0 | CLK CNT  : 0x0 | R_DESC2_BYTE_DELETION_100M                 |
| PCSCRDGEN: 0x0 |                | BLOCKSYNC_MODE_NONE                        |
+----------------+--+-------------+------+------------------+------------------+
|        SPEED      |        STATS0      |      STATS1      |   CREDIT STATS   |
+-------------------+--------------------+------------------+------------------+
| SPD CHG VLD  0    |     OS MODE 1      | DESCR MODE BYPASS| SGMII SPD : 0000 |
| SPD CHG DONE 0    |    SCR MODE 64B    |DECODE MODE CL49  |  CLK CNT0 : 0033 |
| SPD RESOLVED 0028 |    ENC MODE CL49   |DESKEW MODE BYPASS|  CLK CNT1 : 0000 |
| #LN RESOLVED 0000 |BLKSYNC MODE CL49   |DESCR2 MODE CL49  |   LP CNT0 : 0001 |
| PLL DIV      0010 |   CL72  ENA : 01   |  BYTE  DEL NONE  |   LP CNT1 : 0000 |
| REF CLOCK    0000 | CHKEND  ENA : 00   |64b66DEC EN 0     |  MAC  CGC : 0004 |
|                   |    FEC  ENA : 00   |                  |  REP  CNT : 0000 |
|                   |REORDER  ENA : 00   |                  |PCS CRD EN : 0000 |
|                   |   CL36  ENA : 00   |                  |PCS CK CNT : 0000 |
|                   |                    |                  |CRDGEN CNT : 0000 |
+-------------------+--------------------+------------------+------------------+

\endcode

The PMD data dump has similarly controls. The DSC states are shown with the
command below.

\code
BCM.0> phy diag xe0 dsc

***********************************
**** SERDES CORE DISPLAY STATE ****
***********************************

Average Die TMON_reg13bit = 5999
Temperature Force Val     = 255
Temperature Index         = 10  [40C to 48C]
Core Event Log Level      = 1

Core DP Reset State       = 0

Common Ucode Version       = 0xe10e
Common Ucode Minor Version = 0x0
AFE Hardware Version       = 0x0

LN (CDRxN  ,UC_CFG) SD LCK RXPPM CLK90 CLKP1 PF(M,L) VGA DCO P1mV M1mV
DFE(1,2,3,4,5,dcd1,dcd2)   SLICER(ze,zo,pe,po,me,mo) TXPPM TXEQ(n1,m,p1,p2)
EYE(L,R,U,D) LINK_TIME
0 (OSx8.25,0x40)   1   1    0   42    21   7, 0    45   0    0   0   0,  0,  0,
0,  0,  0,  0 -54,-54,-54,-38,-14,-54      0    12,102, 0, 0    0, 0, 0, 0
3.6
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
DFE on                = 0
Brdfe_on              = 0
Media Type            = 2
Unreliable LOS        = 1
Scrambling Disable    = 0
CL72 Emulation Enable = 0
Lane Config from PCS  = 0

CL72 Training Enable  = 0
EEE Mode Enable       = 0
OSR Mode Force        = 1
OSR Mode Force Val    = 8
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
BCM.0> phy diag xe0 dsc cl72

***********************************
**** SERDES CORE DISPLAY STATE ****
***********************************

Average Die TMON_reg13bit = 6025
Temperature Force Val     = 255
Temperature Index         = 10  [40C to 48C]
Core Event Log Level      = 1

Core DP Reset State       = 0

Common Ucode Version       = 0xe10e
Common Ucode Minor Version = 0x0
AFE Hardware Version       = 0x0

LN (CDRxN  ,UC_CFG) SD LCK RXPPM CLK90 CLKP1 PF(M,L) VGA DCO P1mV M1mV
DFE(1,2,3,4,5,dcd1,dcd2)   SLICER(ze,zo,pe,po,me,mo) TXPPM TXEQ(n1,m,p1,p2)
EYE(L,R,U,D) LINK_TIME
0 (OSx8.25,0x40)   1   1    0   42    21   7, 0    45   0    0   0   0,  0,
0,  0,  0,  0,  0 -54,-54,-54,-38,-14,-54      0    12,102, 0, 0    0, 0, 0, 0
3.6
\endcode
Get DSC specific information with this command
\code
BCM.0> phy diag xe0 dsc debug

************************************
**** SERDES LANE 0 DEBUG STATUS ****
************************************

Restart Count       = 1
Reset Count         = 1
PMD Lock Count      = 2

Disable Startup PF Adaptation           = 0
Disable Startup DC Adaptation           = 0
Disable Startup Slicer Offset Tuning    = 0
Disable Startup Clk90 offset Adaptation = 0
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
Disable Steady State Clk90 offset Adaptation = 0
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
Clk90 offset Adjust   = 135
Clk90 offset Override = 0
Lane Event Log Level  = 2
\endcode

\section TSCE12-intro TSCE12

The TSCE12 is uses 3 TSCE cores to provide 12 physical 10G lanes. It can also be
configured as three, independent TSCs, each of which support 4 lanes. In the
first case it interfaces on the system side with a CMAC and in the second case
it interfaces with three, separate, XLMACs.

When in three-core mode, the operations of the core are no different than the
TSCE. So this section discusses the case when the three cores operating with a
CMAC. Another block of logic also provides for 100G or 120G MLD.

\subsection C_cfgconst_tsc12 100G configuration constraints in TSCE12

To support 100G in three core mode, we have to select 10 out of 12 lanes. Two
lanes will be unused. So we have the notations of 4-4-2, 3-4-3, or 2-4-4 from
TSCE12.

\li The 4-4-2 means the first and second cores deploy all 4 lanes, and the
third core deploys logic lanes 0 and 1.

\li 3-4-3 means the first core provides lanes 0, 1, and 2, the second core
provides all 4 logic lanes, and the third code provides lane 0, 1, and 2.

\li 2-4-4 means the first core provides logic lane0 and lane1, and the second
and third core provide all 4 logic lanes.

The logic lane 0 of the individual core must always be active. In other words
the two unused lanes cannot be logic lane 0 in any 100G configuration.

The logic lane order is the same for data striping. Also each core has lane swap
functions within the core that could be used to accommodate board routing lane
swap applications.
<b>NOTE:</b> In TR3 and Arad 100G HW, there is an MLD reorder register that
can achieve a restricted logical lane swap cross 3 cores. But in TD2+, the
lane swap is within a single core.

\subsection tsc12-port-trn Configuration transitions

Broadcom PHY ports could be easily reconfigured to meet wide applications, such
as lane swap, speed change, port size change (flex port). But due to the 100G HW
design, there is some limitation worth noting.

In the 'before' column, the given configuration has a connection to 0-9 lanes
of the cable connector which provides 100G traffic. This type of connector is
not IEEE standard. In the after row, the given configuration has a connection to
1-10 lanes of the cable connector which provides 100G traffic. This type of
connector is for IXIA/IEEE testing. Note that the transition
table is written for a given board design/routing and lane swap is not required.

The following table shows the possible configuration transitions for 100G forced
speed modes for different cabling.

<table cellspacing=5>
<tr><td colspan=3><B>'Transition table' bit-mappings</B></td></tr>
<tr><td><B>Before(0-9)\\After(1-10)</B></td> <td><B>4-4-2</B></td> <td><B>3-4-3</B></td> <td><B>2-4-4</B>     </td></tr>
<tr><td>4-4-2</td> <td>Impossible</td> <td>OK</td>         <td>Impossible</td></tr>
<tr><td>4-4-2</td> <td>Impossible</td> <td>Impossible</td> <td>OK        </td></tr>
<tr><td>4-4-2</td> <td>Impossible</td> <td>Impossible</td> <td>Impossible</td></tr>
</table>

\li The impossible mark applies to the configuration transitions that incur due
to 0-9 vs 1-10 lane selection between two types of cabling.

\subsection cl37-100-an CL73 100G AN considerations

For auto negotiation(AN), we need to first identify logic lane 0 to carry out
page exchanges and speed negotiation. Further the CL73 AN can negotiate to 100G,
40G, 10G KR, 10G-XAUI, or even 1G.  For 40G, the design would require a 4-lane
XLMAC bandwidth. Thus for the 4-4-2 configuration, only the lane 0 of the first
or second core can be used for auto negotiationotiation if 40G is a required
advertisable speed.  For the 3-4-3 configuration , only the second core can be
used. Similarly for 2-4-4 configuration, only the second and third core's lane 0
can be used.  Proper port configuration requires to setup the correct XLMAC out
of three XLMACs for speeds less than 100G. But for 100G ports, the logic lane 0
is not always in the first core. So some of the configuration settings are BRCM
TD2+ specific and we have to ensure the future BRCM products are backward
compatible.

<table cellspacing=5>
<tr><td><B>Configuration</B></td> <td><B>Core for Lane</B></td></tr>
<tr><td>4-4-2</td> <td>1 or 2</td></tr>
<tr><td>3-4-3</td> <td>2     </td></tr>
<tr><td>2-4-4</td> <td>2 or 3</td></tr>
</table>

To support configurations mentioned above, the driver would need the platform to
provide information (for example, in SDK we sould call them SOC properties)
- Lane configuration identification 4-4-2, 3-4-3, or 2-4-4.
- The core supporting  lane 0 for auto-negotiation.
For incompatible combinations of the soc properties, the platform should default
to a known working combination or handle the error appropriately.

\section tsce_rcc TSCE Reference Clock Configuration

TSCE can configured to use 156.25MHz or 125MHz reference clock.

For PCS configuration, the reference clock frequency only matters for AN. And
used for tick generation, that is used in AN timers.
- Configure the main0_setup register specifying the reference clock.
         Register: Main0_setup (Adr:0x9000)
- In the case of 125MHz reflck only: Set the tick override enable
  and set the {tick_numerator_upper, tick_numerator_lower} = 19'd1875
  and tick_denominator = 1. For 1255MHz refclk, hardware takes care of the tick
  Register: Main0_tick_control_1(Adr:0x9007) and Main0_tick_control_0(Adr:9008)

For PMD configuration,
 - The pll_mode and the osr_mode are to be programmed based on the refclk.
        Register: PLL_CAL_COM_CTL_7(Adr:0xd127) and CKRST_CTRL_OSR_MODE_CONTROL - OSR_MODE_CONTROL(Adr:0xd080)
- For 125MHz, set the heartbeat counter to 'd500. For 156.25MHz refclk, use the
  default setting of `d625.
       Register: DIG_COM_TOP_USER_CONTROL_0(Adr:0xd0f4)
- Always use default values for refclk_divcnt  refclk_divcnt_sel registers
  This clock divider is used by the internal calibration logic to generate an
  internal slow clock that is used for calibration.  If you switch to 125MHz
  refclk, this internal clock is a little slower and total calibration time
  will take a little longer but calibration will work fine with this. These
  registers should only be used for debug to tweak calibration if needed.
  Register: PLL_CAL_COM_CTL_5(Adr:0xd125) and PLL_CAL_COM_CTL_6(Adr:0xd126)
- The PMD ucode should support the configuration of pll_mode and osrmode for AN
  speeds for the corresponding refclk.

\section  tsce-fpc TSCE-PMD fractional plldiv configuration

The PMD API "eagle_tsc_configure_pll", should be used for fractional plldiv
configuration. More details goto eagle API  #eagle_tsc_configure_pll

\section  tsce_faq TSCE frequently asked questions (FAQ)

- What are the different firmware load methods?

The two firmware load methods are external and internal. External is fast
load. Internal load is slow load. External load is default way as it is faster.
*/
