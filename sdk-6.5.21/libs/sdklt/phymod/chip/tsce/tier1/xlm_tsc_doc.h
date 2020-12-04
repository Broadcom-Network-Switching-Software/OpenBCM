
/*
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved.
$Id$
*/
/*!
\page XLMAC-TSCE-doc MAC-PHY (PM4X10) Interface Documentation

\tableofcontents

XLMAC-TSCE Interface

\section tsc-mac-intr Introduction

The physical interface of TDM based SerDes Controller (TSC) and Media Access
Controller (MAC) is a Broadcom proprietary Reconciliation Sublayer (RS). But
logically this interface complies with IEEE RS specification. RS function is
implemented in the MAC. XLMAC is a 40G Ethernet MAC and TSCE is the 28nm TSC
PHY with the Eagle (12.5G) PMA/PMD.

\section tsc-mac-clk CLOCKS

In the MAC-TSC interface (MSI) the PHY sends clocks to the MAC. This clock is
called the TSC_CLK_O. The PHY is provided with an external clock called the
REF_CLK. The PLL in the PMD sublayer of the PHY generates a TSC_CLK. If the PLL
is still waking up or is quisced for some other reason, the TSC_CLK is set to
REF_CLK. This ensures the TXP and RXP (etc.) can be controlled, reset, etc.,
with a running clock. Once the PLL is up and running, the TSC_CLK is
glitchlessly switched to the standard frequency which is the PLL_FREQ/16 (also
called VCO/16). If the PLL goes down for whatever reason, the TSC_CLK will
revert to REF_CLK, again glitchlessly.

In XLMAC-TSC design there is no source synchronous data/clock. XLMAC and TSC
(PCS portion) are synthesized with the same TSC_CLK.

The TSC_CLK also clocks the 8051 uController and associated logic within the PMD
sublayer.

\section tsc-mac-tdm TX_TDM & TX_PORT_NUMBER

TSC generates a free running TDM. It is a 2 bit counter, continuously rotating
through 0,1,2,3 - 0,1,2,3
In single port mode all these TDM slots are assigned to the same port. But in
multi-port mode the TDM slots are distributed as follow:
- 1 port mode:
        -# Port0 is assigned to TDM0-TDM3
- 4 port mode:
        -# Port0 is assigned to TDM0
        -# Port1 is assigned to TDM1
        -# Port2 is assigned to TDM2
        -# Port3 is assigned to TDM3
- 2 two port mode:
        -# Port0 gets assigned to TDM0-TDM1
        -# Port2 gets TDM2-TDM3 slots.
- 3 port mode(TRI1):
        -# Port0 gets TDM0-TDM1
        -# Port2 gets TDM2
        -# Port3 gets TDM3
- 3 port mode(TRI2):
        -# Port0 gets TDM0
        -# Port1 gets TDM1
        -# Port2 getsTDM2-TDM3

XLMAC will map the TDM numbers (slots) to the port number indicated by TSC. When
XLMAC transmits data, it will use the same TDM numbers (mac_tx_tdm[1:0]) for the
corresponding port (mac_tx_port_number[1:0]). Because of this any time TSC port
mode changes, XLMAC will need a complete reset.  XLMAC continues to respond back
mac_tx_tdm and mac_tx_port_number even when XLMAC is in soft reset to avoid any
interruption in TSC pipeline.

\section tsc-mac-cred CREDITS

The TSCE sends credits to XLMAC to controls the rate at which the XLMAC sends
data to the line. Therefore setting the speed mode in the XLAMC is for
information only, it does not have any logical impact. Each credit tells the
XLMAC to send data (IDLEs/Remote FAULTs or packet data depending on the state of
the link) to the TSCE. Credits (port_n_credit[3:0]) are generated per port
without any relationship to the tx_port_number[1:0] and tx_tdm[1:0].
Port_n_credit[3:0] is 4 bit signal. Each bit is assigned for each port.

- Port_n_credit[0] : port0
- Port_n_credit[1] : port1
- Port_n_credit[2] : port2
- Port_n_credit[3] : port3

Depending on the port configuration TSCE provides tx_port_number[1:0]. XLMAC
ignores any credits to unused ports. Ideally TSCE will generate credits only for
the ports assigned.

The XLMAC transfers data to the TSCE with some pipeline delay once it has
accumulated a credit from TSCE.

The TSCE sends data to the XLMAC based on the data rate arrives on the line
side. When the link is active, the TSCE sends data received on the wire (after
PCS processing) to the XLMAC. If the link is not active, the TSCE sends either
IDLEs (for CL36) or Local FAULTs for other speeds.

\section tsc-mac-mmod MAC MODES

The MAC can have one of the following physical interfaces with the PHY.
- GMII
- XGMII
- XLGMII
- CGMII
Note that for CGMII three sets of MSBUSs are used to communicate between the
12-lane TSC (TSC_12) and the CMAC.  TSC output TX_FORAMAT[1:0] indicates the
msbus format. Based on the TX_FORMAT, MAC will now how to decode the data. In
GMII mode all 10 bits of each Byte group are used. In GMII/XLGMII/CGMII 9 bits
are used. In XGMII mode SOP can arrive at multiple of 4 bytes (lane0 or lane4)
and in XLGMII/CGMII mode SOP will arrive in 8 lane boundaries (lane0). But BRCM
XLGMII SOP can arrive at 4 Byte boundaries as special mode.
- 00: GMII
- 01: XGMII
- 10: XLGMII/CGMII

\section tsc-mac-rxlf RX_LOCAL_FAULT

This is out of band per port link fault signaling mechanism in TSCE. If TSCE
receive link is not up or even CDR is not locked, this signal will go high. This
signal stays low if PCS link is up. Other than rx_local_fault signal, TSCE sends
local fault control sequence to the XLMAC through msbus_rx. RX_LOCAL_FAULT
signals are low if the port speed is 1G or lower or if TSCE is configured in any
cl36 speed mode (like 2.5G).

Operation of Local Fault/Remote Fault: If link is down or CDR is not locked, PHY
generates local fault to the MAC. MAC in response will generate remote fault
signals to the PHY. Remote PHY will transmit IDLES in response to the remote
fault signals being received. Any PHY receiver can have link up based on Remote
Fault Signal or IDLEs. When local PHY's PCS link is up, it will stop sending
fault signaling to the MAC and instead PHY sends IDLES to the MAC. Once Local
MAC receives IDLEs from the local PHY, it will stop transmitting remote fault
and MAC will start transmitting regular IDLEs or Data.

Note: This out of band rx_local_fault signals were defined in TSCE because TSCE
can't reliably generate local fault sequence on msbus_rx when CDR is not
locked.

Note: TSCE supports in band Fault signaling as well. In 10G XGMII mode, TSCE
generates 4 Bytes of Sequence Ordered (0x9C, 0x00, 0x00, 0x01) for fault
signaling. In XLGMII/CGMII mode TSCE generates 8 Byte Sequence Ordered Set
(0x9C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00) for fault signaling.

\section tsc-mac-msbusrx MSBUS_RX, MSBUS_RX_VALID, RX_PORT_NUMBER

TSCE receives serial data from the line and provides decoded parallel data to
the XLMAC. This data bus 80 bits wide representing 8 bytes of data.
msbus_rx[79:0] is valid when MSBUS_RX_VALID is high for the corresponding
rx_port_number[1:0] indicated. All these data are transferred with respect to
tsc_clk.
In msbus_rx[79:0], out of each 10 bits:
- Bit9: rx_error for 8b10b and ignore for other encoder.
- Bit8: data_valid (cl36) or control bit for others (1: control, 0: data)
- Bit7:0: 8bit data

\section tsc-mac-msbustx MSBUS_TX, MSBUS_TX_VALID, MAC_TX_PORT_NUMBER

XLMAC transfers data to TSCE through msbus_tx[79:0]. Data is valid if
msbus_tx_valid is high for the corresponding mac_tx_port_number[1:0]. Every port
is assigned to 1 or 2 or 4 tdm slots set by the TSCE. XLMAC binds the tdm slots
to the port numbers and transmits data on msbus_tx with the previously bind tdm
slots for that port.
In msbus_tx[79:0], out of each 10 bits:
- Bit9: tx_error for 8b10b encode and ignore for other decoder.
- Bit8: data_valid (cl36) or control bit for others clauses (1: control, 0: data)
- Bit7:0: 8bit data

\section tsc-mac-xlmactx XLMAC Transmit

XLMAC transmit rate is controlled by the credit generated by the TSCE thru
port_n_credit[3:0]. Each bit is dedicated for one port i.e. 4 bits for 4 ports.
XMLAC will accumulate these credits for each port with no relations to
tx_port_number[1:0] and tx_tdm[1:0].
XLMAC gets tx_port_number[1:0] and tx_tdm[1:0] from TSCE and transmits those
back to TSC as mac_tx_port_number[1:0] and mac_tx_tdm[1:0] maintaining
tx_port_number and tx_tdm partitioning for each port. If XLMAC had any credit
accumulated for that port, it will assert msbus_tx_valid along with the
corresponding data msbus_tx[79:0].

\section tsc-mac-macphy XLMAC<->TSCE INTERACTION

The first independent operation is for the PHY to transmit idles. The MAC must
provide idles to the PHY. MAC will only send idles if it gets credits. Hence the
following sequence must happen.

-# PHY is brought out of reset. PLL is stable. Clocks are humming
-# PHY sends credits at line rate and information (portnum, TDM, format) to MAC
-# MAC sends Idles to the PHY. (data and data_valid)
-# PHY transmits IDLES to the wire.

\section tsc-mac-lpbk XLMAC loopback

Line Local Loopback mode: XLMAC routes the TX packets back to the RX, just
before the packets are driven to the TSC interface. This loopback is implemented
from the last pipeline stage in the TX block to the first pipeline stage in the
RX block. The loopback takes place in the line clock after clock domain
crossing, and data completely traverses the TX & RX pipeline stages.

This loopback can be enabled for a port by setting the LINE_LOCAL_LPBK bits. The
transmit packet stats vector (TPSV) & receive packet stats vector (RPSV) will
account for these packets.  In order for this loopback to work, the credits from
the TSC must be available for the port that is put in loopback.

For XLMAC Line Local loopback to work, credits need to be flowing from TSCE.
Therefore Speed Control function of TSCE will be utilized to set the speed for
appropriate credits. To set a particular speed, desired speed is set at sw_speed
(sc_x4_control_control[7:0]) and then toggle sw_speed_change. i.e. Set
(sc_x4_control_control[8]) to 0 and then 1).
Then program XLMAC active port/ports, set XLMAC in Line Loopback mode, release
soft reset from the XLMAC. Assert TX_EN and RX_EN.

To turn off line loopback first apply XLMAC.SOFT_RESET, de-asset TX_EN and RX_EN
and then turn off Line Loopback. After that release XLMAC.SOFT_RESET, assert
TX_EN and RX_EN.
Once XLMAC and TSCE is completely programmed for any specific speed mode:
Set XLMAC loopback:
-# Set, in this order
  -# XLMAC.SOFT_RESET=1
  -# set TX_EN=0
  -# RX_EN=0
  -# XLMAC.LINE_LOCAL_LOOPBACK=1
-# Clear EP FIFO
-#  Release soft reset and enable TX and RX
  -# XLMAC.SOFT_RESET=0
  -# TX_EN=1
  -# then RX_EN=1
Now XLMAC is ready for MAC loopback. Run the tests.

To Release XLMAC loopback:
-# Set, in this order
  -# XLMAC.SOFT_RESET=1
  -# set TX_EN=0
  -# RX_EN=0
-# XLMAC.LINE_LOCAL_LOOPBACK=0
-# Clear EP FIFO
-#  Release
  -# XLMAC.SOFT_RESET=0
  -# set TX_EN=1
  -# RX_EN=1
At this time XLMAC is ready for normal operation.

\section tsc-mac-xlmisc XLMAC Misc

-# On TX direction XLMAC contains a Clock Domain Crossing (CDC) FIFO. Anytime a
soft reset is applied to any port, subsequently a reset needs to be applied to
  the corresponding CDC-FIFO.
-# While XLMAC is in soft reset, TSCE credits are acknowledged.
-# In GMII mode variable preamble mode need to be programmed to accept packets
  preamble + SFD less than 8 Bytes.

\section tsc-mac-pm4x10 Port Macro, PM4X10

Port Macro is a hard macro that integrates MAC, RS, PCS and PMA/PMD layers.
PM4X10 contains MIB statistics and other ancillary logic.  MAC (XLMAC) can be
configured into single/dual/triple/quad port modes. It can handle 10Mbps to
42Gbps data rate. PCS can perform CL36, CL48, CL49, CL82, CL37 (AN), CL73 (AN),
BRCM64b66b, CL74 (FEC) etc.  PMA/PMD supports per lane 1.25Gbps to 12.5Gbps.
The PM4X10 consists of TSC_4_E and PM4X10_Core. PM4X10_Core consists of XLMAC
and PORT glue logic. The TSC_4_E is based on the 28 nm Eagle Analog block. This
ANA_WARPCORE11G analog supports up to 10.3125G on a single lane. The Eagle DSC
design is used in the PMA_PMD for the TSC_4_E. This design uses a XLMAC since
support of speeds of only up to 1X40GE is required. The glue logic in the PORT
MACRO integrates the designs together, implements the MIB and EEE counters. The
glue logic also provides a mechanism to access the registers in XLMAC and
TSC_4_E. Here is block diagram of Port Macro, PM4X10.

@image html pm4x10.jpg "Basic Block diagram for PM4x10"

\section tsc-mac-bup PM4X10 bring up Sequence

This section describes general sequence of powering up a port macro.

-# Apply Digital power supply and then apply Analog power supply. At this time
TSCE, and XLMAC are in hard reset (core_rst_l). All MIB statistics are clear.
-# Once power supply is stable and a stable clock is applied, take PM4x10 out of
hard reset, then take XLMAC out of hard reset. Then when a stable reference
clock is applied, release the hard reset to TSCE. XLMAC is still at soft_reset.
-# Program the appropriate port mode in the PortMacro. This port information
will be sent to XLMAC.
-# Clear all MIB counters.
-# Initialize TSCE in proper sequence. See TEMod doc. for more details.
  -# bring up PLL
  -# program for lane_swap, (both PMD and PCS)
  -# load microcode
  -# program PMD core and lanes. TX settings, OS, polarity, and media type etc.
  -# program PCS through Speed Control register.

At this time PLL is up and TSC_CLOCK is stable.  Tx_format, port_n_credit,
tx_tdm, and tx_port_number are chugging on in the correct sequence.  MAC still
is in soft-reset. But XLMAC does respond to credits with tx_format appropriate
IDLES or remote_fault signals to the TSCE. At this time TSCE should link up with
link partner if partner is also enabled.

-# When PHY link up is seen, Initialize MAC for the packet format, port speed
and the feature specific registers (eg. IPG, preamble, runt_threshold, pad_en,
PAUSE/PFC/LLFC/E2EFC/E2ECC, 1588 and EEE registers).

-# Take MAC out of soft reset. MAC_CTRL.SOFT_RESET, and enable MAC RX_EN and TX_EN.

Note: Flexporting may never need hard reset.

\section tsc-mac-prog PM4X10 Detail programming sequence

Programming sequence starts at lower level and then moves up. First Eagle
(PMA/PMD) is configured, then PCS_TSC4E (PCS) portion is configured, then XLMAC
is programmed and then finally other associated blocks like Clock Domain
Crossing FIFO (CDC FIFO) and MIB counters are cleared.
To start with apply stable power supply and reference clock (typically 156.25MHz)
and then its resets are released.
-# Apply Digital power supply and then apply Analog power supply. At this time
TSCE, XLMAC are at hard reset (core_rst_l).
-# Core clock to PORT MACRO is toggling and stable.
-# De-assert PM4x10 hard reset core_rst_l. This brings all PORT logic out of reset.
-# XLMAC has a secondary soft reset. The default condition is that this reset is
asserted, keeping MACs in reset.
-# De-assert MAC hard reset. Clear PORT_MAC_CONTROL.XMAC0_RESET=0.
-# De-assert TSC hard reset. Set PORT_XGXS0_CTRL_REG.RSTB_HW=1.
-# Program the appropriate port mode in the PortMacro. This port information
will be sent to XLMAC.
-# Clear all MIB counters.
-# Initialize PMD Core of Eagle
  -# Clear the PMD power down pins (PCS bits: pmd_x4_control.ln_tx_h_pwrdn and
  pmd_x4_control.ln_rx_h_pwrdn)
  -# Take PMD core out of reset (PCS bits: pmd_x1_control.por_h_rstb and
  pmd_x1_control.core_dp_h_rstb). This will release pmd__por_h_rstb and
  pmd_por_h_rstb pins of PMD.
  -# Program PMD bits: hearbeat_count_1us (0x271 for 156.25MHz refclk),
  refclk_divcnt (0x186A for 156.25MHz refclk), refclk_divcnt_sel (0x2 for
  156.25MHz refclk). Note: At reset, all other timer/register fields are loaded
  with normal operating values.
  -# Load Micro code and verify. Use API.
    -# Set uc_active=1
    -# De-assert 8051 reset (micro_mdio_dw8051_reset_n=1)
    -# After 10ms, poll for uc_dsc_ready_for_cmd=1 for all lanes (minimum wait
    time will be refined in next document version)
  -# Lane Swap and Polarity inversion related configurations at PMD level.
  (Note: lane swap involves programming at PMD and PCS both).
  -# Configure PLL using API: pll_mode (0xA/0x2 for 10.3125G/6.25G VCO)
  -# Configure Core level micro RAM variables: core_cfg_from_pcs and vco_rate
  -# Release core_dp_s_rstb in PMD
  -# Do Lane Confgurations: Media Type (use default), TXFIR (use default)
  -# Release ln_dp_s_rstb.
Note: At this time initial PMD core initialization is done.
-# Release PMD lane reset
  -# Write 1 to pmd_x4_control.ln_h_rstb. This will deassert PMD pin
  pmd_ln_h_rstb[i] and enable register access to lane associated registers.
  -# Write 1 to pmd_x4_control.ln_dp_h_rstb. This will allow the Speed Control
  FSM to manage PMD lane datapath resets- pmd_ln_dp_h_rstb[i].
  -# Program PMD lane level registers and memories as required, the set of
  configuration will depend on usage of canned speeds and presence of firmware.
  This step may include: setting the over-sample mode, TX and RX lane
  polarity, cl72_ieee_training_enable, transmitter configuration, baud rate DFE,
  scrambler_dis, media_type, cl72_mode, etc.
  -# SW writes to PMD registers to release ln_dp_s_rstb - lane DP datapath soft
  resets.
-# At this point the PMD was taken out of POR and lane hard resets were release,
  PMD is fully configured by software and the firmware is waiting for
  pmd_core_dp_h_rstb and pmd_ln_dp_h_rstb release.
-# Take PCS pipeline out of reset and allow the Speed Control to manage PCS
  pipeline. [SW]
  -# Set the following bits: TX_X4_Control0_misc. rstb_tx_lane,
  TX_X4_Control0_misc.enable_tx_lane and RX_X4_Control0_pma_control_0. rstb_lane
-# Check for Master Port considerations.
  -# If configuring first port, or if change in VCO is required: select the
  master port that will perform PMD core configuration, bring down all the ports
  by clearing  sc_x4_control_control.sw_speed_change bit and reset by setting
  Main0_setup.master_port_num, then set Main0_setup.pll_reset_en bit. This will
  indicate to the Speed Control FSM to control PMD core resets. [SW]
  -# For AN and only if a single port is being enable set
  Main0_setup.single_port_enable instead of Main0_setup.pll_reset_en bit. [SW]
-# The firmware is waiting for PCS FSM to provide core and lane configuration
  information, *dp_h_rstb signals are used to indicate to FW that the
  configuration provided by PCS is valid. [FW]
-# Providing PCS configuration information to PMD and starting firmware.
  -# Forced speed/Autoneg considerations
    -# For Forced speed write desired speed ID to sc_x4_control_control.speed
    field and set the sc_x4_control_control.sw_speed_change bit. [SW]
    -#  For AN configure the abilities and enable AN. [SW]
  -# The Speed Control FSM sets pmd_core_mode signal and deasserts
  pmd_core_dp_h_rstb reset after <256> cycles delay. [HW]
  -# The Speed Control FSM drives pmd_lane_mode_i signals and deasserts
  pmd_ln_dp_h_rstb after <256> cycles delay. Note: The FSM does not wait for
  any indication from PMD before deasserting lane datapath resets, so core and
  lane resets will be deasserted simultaneously. The assumption is that PMD will
  internally handle this and lane configuration by firmware will be done only
  ofter the PLL is locked. [HW]
-# Once the pmd_core_dp_h_rstb and pmd_ln_dp_h_rstb resets are deasserted; the
firmware wakes up and uses configuration provided by PCS over the pmd_core_mode
and pmd_lane_mode_i signals to finish the configuration of PMD core and
lanes and enabling of the lanes. [FW]
-# Wait for link to come up. [SW]
-# Repeat for additional ports; make sure to not to set the
Main0_setup.pll_reset_en bit, so the Speed Control logic does not reset PMD core
again.
-# Initialize MAC for the packet format, port speed and the feature specific
registers (eg. IPG, preamble, runt_threshold, pad_en, 1588, EEE,
PAUSE/PFC/LLFC/E2EFC/E2ECC registers, etc.)
-# Remove soft reset and enable  MAC
  -# XLMAC_CTRL.SOFT_RESET=0
  -# enable MAC RX_EN
  -# enable MAC TX_EN

\section tsc-mac-flex PM4X10 Flex-porting Sequence

-# At the beginning of time: configure to a port mode, do all initialization and
send traffic over relevant lanes.
-# Apply XLMAC Dynamic Speed Change Sequence over ports that need to change:
  -# Assert EP_DISCARD, TX_DISCARD and SOFT_RESET
  -# Deassert RX_EN and TX_EN
-# Initialize port :
  -# Only those ports that are changing run through the init sequence. Remaining
  ports are untouched.
  -# For XLPORT: if there are no ports common between previous and current
  configuration, issue a reset to XLMAC using xlport_mac_control_reset.
-# Program TSCE:
  -# If VCO does not match, reset the global vars from TSC : tsc_touched,
  pcs_lane_swap_touched, pmd_touched
  -# Reset all changing ports per lane
  -# Configure all changing ports with regular TSCE init routine to bring link up
-# Initialize XLMAC per port
  -# If vco has changed, all active ports are configured
  -# If VCO changes, but there are no common ports between previous and current
  configuration, all active ports are configured.
  -# For same VCO if any ports are common between previous and current
  configuration, they will be in reset due to step (2). These will be brought
  out of reset and configured. The remaining ports remain uninterrupted and
  continue sending traffic.

\section tsc-mac-flexprog PM4X10 Flex-porting detail Sequence

Flexport consists of changing the speed or number of ports after the device has
been configured.  When changing the PORT_MODE register, it is required to not
bring down sub-ports, if any, unaffected by the port mode change, while
selectively updating the affected ports. The tsc_clk frequency will not change
during a flexport change.  An example of a flexport change is 4x10G to 2x20G,
1x40G for a given XLMAC. If flexporting from two dual 2x20G port mode to
tri-port mode {1x20G, 2x10g}, 1x20G port data should not be interrupted.

This sequence also applies when one or more links go down.  After a link goes
down, it is possible that the links can come up in a completely different speed
mode.

-# SW disables MAC RX_EN and initiates a flush on the ports affected by the
flexport change using MAC_TX_CTRL.DISCARD and MAC_TX_CTRL.EP_DISCARD.  SW
determines the affected subports are idle by polling MAC tx fifo, mmu and
edatabuf.
-# SW disables MAC TX_EN.
-# SW asserts one or more of per-port register MAC_CTRL.SOFT_RESET to prepare
the ports which will soon be active. Only those ports that are changing will be
configured. Remaining ports are not touched.
-# SW sets the new port mode by programming register PORT_MODE_REG, and the
other per-port type port registers.
-# Configure TSC with the regular TSC init routine to bring up the link.
-# Program MAC for the packet format, port speed and the feature specific
registers (eg. IPG, preamble, runt_threshold, pad_en,
PAUSE/PFC/LLFC/E2EFC/E2ECC, 1588 and EEE registers).
-# SW removes soft reset to the MAC:  MAC_CTRL.SOFT_RESET, and enables MAC RX_EN
and TX_EN.
-# SW can program the TSC registers to save the power if the corresponding PORT
will not be in use.

\section tsc-mac-portdwn Port Down
To bring down a port the following sequence should be applied.
-# Force Transmit Electrical Idle at TSCE.
-# SW disables XLMAC RX_EN to the specific port.
-# SW initiates a flush on the port/ports coming down: using MAC_TX_CTRL.DISCARD
and MAC_TX_CTRL.EP_DISCARD.  Poll until TX FIFO is empty.
-# Disable MAC transmit XLMAC TX_EN=0
-# Clear speed

-# SW disables MAC RX_EN and initiates a flush on the ports affected by the
flexport change using MAC_TX_CTRL.DISCARD MAC_TX_CTRL.EP_DISCARD.  SW determines
the affected subports are idle by polling MAC tx fifo, mmu and edatabuf.
-# SW disables MAC TX_EN.
-# SW asserts one or more of per-port register MAC_CTRL.SOFT_RESET to prepare
the ports which will soon be active. Only those ports that are changing will be
configured. Remaining ports are not touched.
*/
