/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */
#if LINK_PSS_LIBRARIES

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * INCLUDE FILES:
 */
/* { */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <taskLib.h>
#include <errnoLib.h>
#include <usrLib.h>
#include <tickLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <logLib.h>
#include <pipeDrv.h>
#include <timers.h>
#include <sigLib.h>
#include <cacheLib.h>
#include <drv/mem/eeprom.h>
/*
 * This file is required to complete definitions
 * related to timers and clocks. For display/debug
 * purposes only.
 */
#include <private\timerLibP.h>
#include <shellLib.h>
#include <dbgLib.h>
/*
 * Definitions specific to reference system.
 */
#include <usrapp.h>
/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * User interface internal include file.
 */
#include <appl/dpp/UserInterface/ui_defi.h>
#include <appl/dpp/UserInterface/ui_pure_defi_negev_demo.h>
#include <prestera/tapi/bridge/gtBrgFdb.h>
#include <galtisAgent/wrapUtil/cmdPresteraUtils.h>


#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_links.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_ingress_queue.h>
#include <soc/dpp/SOC_SAND_FAP10M/fap10m_api_auto_flow_management.h>
#include <soc/dpp/FMC_N/SOC_SAND_FAP10M/fmc_n_fap10m_scheduler_init.h>

#include <FMC/fmc_transport_layer.h>
#include <appl/diag/dpp/dune_rpc.h>
#include <appl/diag/dpp/dune_rpc_auto_network_learning.h>
#include <appl/diag/dpp/utils_init_dpss_for_demo.h>
#include <appl/diag/dpp/utils_string.h>

#include <Bsp_drv/bsp_cards_consts.h>


#if REPLACE_RPC_BY_DCL
/* { */
#include <appl/diag/dpp/dune_rpc.h>
#include <FMC/fmc_transport_layer_dcl.h>
/* } */
#endif

/*
 * Chassis System
 */
#include <CSystem/csystem_mngmnt.h>
#include <CSystem/csystem_commands.h>

/*
* Agent for fap10m application.
*/
#include <FMC/fap10m_app_agent.h>

#include <Serdes/serdes_config.h>

/* } */
/*
 * Set DEBUG_NEGEV_DEMO to 1 if debug printing is required.
 */
#define DEBUG_NEGEV_DEMO 0

static unsigned long
  get_slot_number_from_ip_addr(
    unsigned long ip_addr
  )
{
  return ((ip_addr & 0x000000FF) - CSYS_FIRST_SLOT_LINE_CARD + 1) ;
}


unsigned int
  negev_demo_use_absolute_dev_id(
   void
  )
{
  return TRUE;
}

/*
 * Convert entity type, as returned in connectivity report
 * by fap10m (see, e.g., fap10m_get_connectivity_map())
 * to string.
 *
 * If entity type is illegal then string is "?#?" and
 * return value is non-zero. Otherwise, return value
 * is zero and string describes entity.
 */
STATIC
  int
    entity_type_to_string(
      unsigned int entity_type,
      char         **entity_str
    )
{
  int
    ret ;
  ret = 0 ;
  *entity_str = "" ;
  switch (entity_type)
  {
    case 0x0:
    case 0x1:
    case 0x4:
    case 0x5:
    {
      *entity_str = "?#?" ;
      ret = 1 ;
      break ;
    }
    case 0x2:
    {
      *entity_str = "FE3" ;
      break ;
    }
    case 0x3:
    case 0x7:
    {
      *entity_str = "FE " ;
      break ;
    }
    case 0x6:
    {
      *entity_str = "FE1" ;
      break ;
    }
    default:
    {
      *entity_str = "?#?" ;
      ret = 1 ;
      break ;
    }
  }
  return (ret) ;
}
/*
 * {
 */

static unsigned char*
  negev_demo_fe_unit_to_index(
    unsigned int fe_unit
  )
{
    switch (fe_unit)
    {
      case(0): return "0" ;
      case(1): return "1" ;
      case(2): return "2" ;
      case(3): return "3" ;
      case(4): return "4" ;
      case(5): return "5" ;
      case(6): return "6" ;
      case(7): return "7" ;
    }
    return "?" ;
}

#if LINK_PSS_LIBRARIES
STATIC int
  negev_demo_fe_slot_id_to_unit(
    unsigned int fe_slot_id,
    unsigned int device_number
  )
{
  int
    ret ;
  ret = -1 ;
  if ((fe_slot_id > (CSYS_FIRST_SLOT_FABRIC_CARD + CSYS_MAX_NOF_FABRIC_CARDS - 1)) ||
            (fe_slot_id < CSYS_FIRST_SLOT_FABRIC_CARD))
  {
    ret = -2 ;
    goto exit ;
  }
  if ((device_number > BSP_NUM_FES_ON_BRD002A) || (device_number < 1))
  {
    ret = -3 ;
    goto exit ;
  }
  /*
   * There are no connections to even 'slot id's
   */
  if ((fe_slot_id & BIT(0)) == 0)
  {
    ret = -4 ;
    goto exit ;
  }
  ret = (fe_slot_id - CSYS_FIRST_FABRIC_SLOT_NEGEV) + (device_number - 1) ;
exit:
  return (ret) ;
}

STATIC int
  negev_demo_fe_unit_to_slot_id(
    unsigned int fe_unit,
    unsigned int *slot_id,
    unsigned int *device_number
  )
{
  unsigned
    int
      slot_index,
      device_index,
      ii ;
  int
    ret ;
  ret = 0 ;
  *slot_id = *device_number = -1 ;

  for (slot_index = 0 ;
          slot_index < CSYS_MAX_NOF_FABRIC_CARDS ; slot_index++)
  {
    if (slot_index & BIT(0))
    {
      continue ;
    }
    for (device_index = 0 ;
          device_index < BSP_NUM_FES_ON_BRD002A ; device_index++)
    {
      ii = slot_index + device_index ;
      if (fe_unit == ii)
      {
        *slot_id = slot_index + CSYS_FIRST_SLOT_FABRIC_CARD ;
        *device_number = device_index + 1 ;
        goto exit ;
      }
    }
  }
exit:
  return (ret) ;
}

#endif

/*
 * }
 */

/*
 */
STATIC void
  get_flow_and_queue_id(
    unsigned int src_fap,
    unsigned int dst_fap,
    unsigned int src_port,
    unsigned int dst_port,
    unsigned int class_id,
    unsigned int *ret_flow_id,
    unsigned int *ret_queue_id
  )
{
  unsigned long
    err_id=0;
  unsigned int
    relative_src=0,
    relative_dst=0;
  unsigned long
    fap_ip,
    fap_i,
    relative_i=0;

  for(fap_i=0,relative_i=0;fap_i<CSYS_MAX_NOF_FAP_IN_SYSTEM;fap_i++)
  {
    if(csys_mngment_is_fap_exist(fap_i,&fap_ip))
    {
      if(src_fap == fap_i)
      {
        relative_src = relative_i;
      }
      if(dst_fap == fap_i)
      {
        relative_dst = relative_i;
      }
      relative_i++;
    }
  }
  err_id = fap10m_auto_get_flow_and_queue_id(
    relative_src,
    relative_dst,
    dst_port,
    class_id,
    ret_flow_id,
    ret_queue_id
  );

  return;
}

unsigned int
  negev_demo_print_mac_addresses_help(
    void
  )
{
  char *help_str =
    "AppDemo Mac Addresses:                                                        \n\r"
    "                                                                              \n\r"
    "1.  Tagged Any to Any.                                                        \n\r"
    "                                                                              \n\r"
    "Destination MAC: 00:CF:00:00:$FAP_ID$FAP_ID: $PORT_ID$PORT_ID.                \n\r"
    "Source MAC: 00:00:00:BA:00:$CLASS.                                            \n\r"
    "                                                                              \n\r"
    "How to use:                                                                   \n\r"
    "The VLAN ID for the any to any class is 400.                                  \n\r"
    "Example: when one wants to send packets to FAP 12(0xC) port 2                 \n\r"
    "and class 3, he need to use the destination address                           \n\r"
    "00:CF:00:00:CC:22 , and the source address 00:00:00:BA:00:03.                 \n\r"
    "The protocol type should be 0x8100 ( 802.1Q VLAN bridged LAN protocol).       \n\r"
    "And the VLAN ID should be 400(0x190).                                         \n\r"
    "One can also use any other known source MAC, when filling the user            \n\r"
    "priority field in the 802.1Q protocol, with the requested Class.              \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                   \n\r"
    "The Tagged any to any scheme allows packet streaming from any                 \n\r"
    "port, to any other port in the system.                                        \n\r"
    "The scheme is also useful for class prioritizing demonstration                \n\r"
    "and testing.                                                                  \n\r"
    "                                                                              \n\r"
    "2.  Tagged Spatial Multicast.                                                 \n\r"
    "Destination MAC: 01:EE:EE:00:00:$SCHEME_ID.                                   \n\r"
    "Source MAC: 00:00:00:BA:00:$CLASS.                                            \n\r"
    "                                                                              \n\r"
    "How to use:                                                                   \n\r"
    "The VLAN ID for the any to any class is 300.                                  \n\r"
    "Example: when one wants to send packets to all the ports in all the           \n\r"
    "FAPS in the system, with class 3 (Guaranteed Spatial Multicast),              \n\r"
    "he need to use the destination address 00:EE:EE:00:00:00,                     \n\r"
    "and the source address 00:00:00:BA:00:03.                                     \n\r"
    "The protocol type should be 0x8100 ( 802.1Q VLAN bridged LAN protocol).       \n\r"
    "And the VLAN ID should be 300(0x1C2).                                         \n\r"
    "One can also use any other known source MAC, when filling the user            \n\r"
    "priority field in the 802.1Q protocol, with the requested Class.              \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                   \n\r"
    "The Tagged Spatial Multicast Schemes allow demonstration of                   \n\r"
    "the multicast features we have.                                               \n\r"
    "The scheme might also be useful for class prioritizing demonstration          \n\r"
    "and testing. For example, one might Send Guaranteed Multicast,                \n\r"
    "Best effort Multicast and unicast streams to the same port,                   \n\r"
    "and validate that the streams are prioritized in the way it                   \n\r"
    "configured in the FAP + PP.                                                   \n\r"
    "                                                                              \n\r"
    "Other Schemes:                                                                \n\r"
    "Apart of the Broadcast that results in sending the packet to all the          \n\r"
    "ports in all the FAPs, one can also use other distribution schemes.           \n\r"
    "The last segment in the destination MAC is the scheme ID.                     \n\r"
    "For example to use distribution scheme 3, the destination MAC should          \n\r"
    "be 00:EE:EE:00:00:03.                                                         \n\r"
    "                                                                              \n\r"
    "Distribution                                                                  \n\r"
    "scheme ID            Receiving ports                                          \n\r"
    "                                                                              \n\r"
    "   0                 All the ports in all the FAPs.                           \n\r"
    "   1                 All the FAPs, ports 0,4 and 8                            \n\r"
    "   2                 All the FAPs, ports 1,5 and 9                            \n\r"
    "   3                 All the FAPs, ports 2,6 and 10                           \n\r"
    "   4                 All the FAPs, ports 3,7 and 11                           \n\r"
    "   5                 FAPs 0,4,8 and 12. Ports 0,4 and 8                       \n\r"
    "   6                 FAPs 1,5,9 and 13. Ports 1,5 and 9                       \n\r"
    "   7                 FAPs 2,6,10 and 14. Ports 2,6 and 10                     \n\r"
    "   8                 FAPs 3,7,11 and 15. Ports 3,7 and 11                     \n\r"
    "   9                 All the FAPs, port 1.                                    \n\r"
    "                                                                              \n\r"
    "3.  Untagged Spatial Multicast.                                                \n\r"
    "Destination MAC: 01:E0:E0:00:00:$SCHEME_ID.                                   \n\r"
    "Source MAC: 00:00:00:BA:BA:$CLASS.                                            \n\r"
    "                                                                              \n\r"
    "Same as the tagged spatial multicast, but should be injected from port '0'    \n\r"
    "at any device.                                                                \n\r"
    "                                                                              \n\r"
    "Only the following two schemes are available                                  \n\r"
    "Distribution                                                                  \n\r"
    "scheme ID            Receiving ports                                          \n\r"
    "                                                                              \n\r"
    "   0                 All the ports in all the FAPs.                           \n\r"
    "   1                 First FAP, port 1                                         \n\r"
    "                                                                              \n\r"
    "4.  Untagged, One to All to One.                                               \n\r"
    "Destination MAC: 00:CB:CB:00:00:00.                                           \n\r"
    "Source MAC: 00:00:00:BA:BA:00.                                                \n\r"
    "                                                                              \n\r"
    "How to use:                                                                   \n\r"
    "The One to all to one scheme works in the following way:                      \n\r"
    "Port '0' in the first FAP is sending multicast packets to all                 \n\r"
    "the other ports in the system.                                                \n\r"
    "All the other port should be connected with external                          \n\r"
    "Ethernet cables, (like in the snake scheme, or in any other way).             \n\r"
    "After the packets arrive to the other ports in the system,                    \n\r"
    "they are entering again to the FAPs, through the external                     \n\r"
    "Ethernet cables. Since the Packets are now entering through other             \n\r"
    "ports, they would now be treated as unicast packets.                          \n\r"
    "The destination of all the unicast packets is the sending port.               \n\r"
    "The result of the scheme is that one port is sending multicast                \n\r"
    "packets to all the other ports in the system, and receives all                \n\r"
    "the sent packets as Unicast.                                                  \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                   \n\r"
    "1.  If the 1G sending port will send 1G traffic, up to 190G                   \n\r"
    "(16 units * 12 ports - 2) might be sent to it.                                \n\r"
    "The expected result is that all the discarded packets would                   \n\r"
    "be discarded in the FIP.                                                      \n\r"
    "2.  If the 1G sending port will send 1/(ports number - 2)                     \n\r"
    "Giga, the expected result is that the sending port will                       \n\r"
    "receive 1 Giga back.                                                          \n\r"
    "                                                                              \n\r"
    "5.  Untagged Pairs.                                                            \n\r"
    "Destination MAC: 00:FA:FA:00:00:00.                                           \n\r"
    "Source MAC: 00:00:00:BA:BA:00.                                                \n\r"
    "                                                                              \n\r"
    "How to use:                                                                   \n\r"
    "The pair scheme works in the following way:                                   \n\r"
    "This is only works when having 5 or more line cards in the system.            \n\r"
    "Port '0' in the first FAP is sending multicast packets to all the             \n\r"
    "ports of line card 1.                                                         \n\r"
    "Port '1' in the first FAP is sending multicast packets to all the             \n\r"
    "ports of line card 4.                                                         \n\r"
    "Each port X in line card 1 should be connected to port X in line card 2,      \n\r"
    "with external Ethernet cables.                                                \n\r"
    "Each port X in line card 4 should be connected to port X in line card 3,      \n\r"
    "with external Ethernet cables.                                                \n\r"
    "By that, the multicast packets arrived to port X in lines 1 and 4,            \n\r"
    "will arrive to port X in lines 2 and 3.                                       \n\r"
    "However, the PP will interpret the packet arrived to port X in line 2 as      \n\r"
    "unicast packet, with destination port X in line card 3.                       \n\r"
    "In the same way, the PP will interpret the packet arrived to port X in        \n\r"
    "line 3 as unicast packet, with destination port X in line card 2.             \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                   \n\r"
    "Bi-directional 12 Giga of unicast traffic will pass between line 2            \n\r"
    "and and line 3.                                                               \n\r"
    "                                                                              \n\r"
    "6.  Untagged Spatial Snake.                                                    \n\r"
    "Destination MAC: 00:AB:CD:00:00:00.                                           \n\r"
    "Source MAC: 00:00:00:BA:BA:00.                                                \n\r"
    "                                                                              \n\r"
    "How to use:                                                                   \n\r"
    "The following external Ethernet cable connections should be done:             \n\r"
    "Port N+1 in unit 0 should be connected to Port N in unit 1.                   \n\r"
    "That is: port 1 in unit 0 should be connected to port 0 in unit 1.            \n\r"
    " port 2 in unit 0 should be connected to port 1 in unit 1.                    \n\r"
    " port 8 in unit 0 should be connected to port 2 in unit 1.                    \n\r"
    " port 7 in unit 0 should be connected to port 8 in unit 1.                    \n\r"
    " port 6 in unit 0 should be connected to port 7 in unit 1.                    \n\r"
    " port 9 in unit 0 should be connected to port 6 in unit 1.                    \n\r"
    " port 10 in unit 0 should be connected to port 9 in unit 1.                   \n\r"
    " port 11 in unit 0 should be connected to port 10 in unit 1.                  \n\r"
    " port 4 in unit 0 should be connected to port 11 in unit 1.                   \n\r"
    " port 5 in unit 0 should be connected to port 4 in unit 1.                    \n\r"
    " port 3 in unit 0 should be connected to port 5 in unit 1.                    \n\r"
    "The traffic should be received from port 3 in any unit in                     \n\r"
    "the system.                                                                   \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                   \n\r"
    "The motivation of that spatial snake is not to demonstrate our                \n\r"
    "abilities but to test them.                                                   \n\r"
    "The spatial multicast replications are only done in the fabric                \n\r"
    "up to the FAP level. The FAP level replications are done in the PP.           \n\r"
    "Therefore, to test our fabric, we need to send the multicast packet           \n\r"
    "to only one port in each unit.                                                \n\r"
    "In the spatial snake all the packets replications are done                    \n\r"
    "in the fabric.                                                                \n\r"
    "                                                                              \n\r"
    "7.  Untagged Any to Any.                                                       \n\r"
    "Destination MAC: 00:CA:FE:00:$FAP_ID:$PORT_ID.                                \n\r"
    "Source MAC: 00:00:00:BA:BA:$CLASS.                                            \n\r"
    "                                                                              \n\r"
    "To enable backward compatibility, the any to any addresses are still          \n\r"
    "enabled over the first two line cards.                                        \n\r"
    "One can send a packet from the first two FAPs to port 2 in FAP 1,             \n\r"
    " with the destination MAC address 00:CA:FE:00:01:02.                          \n\r"
    "However, this scheme is not enabled by default.                               \n\r"
    "To enable the above MAC addresses call the CLI:                               \n\r"
    "'negev_demo mac_addresses add_untagged_any_to_any'.                           \n\r"
    "To remove it call to                                                          \n\r"
    "'negev_demo mac_addresses remove_untagged_any_to_any'.                        \n\r"
    "                                                                              \n\r"
    "8.  Snake (Untagged).                                                          \n\r"
    "Destination MAC: 00:BB:BB:00:00:00.                                            \n\r"
    "Source MAC: 00:00:00:BA:BA:00.                                                 \n\r"
    "                                                                              \n\r"
    "How to use:                                                                    \n\r"
    "The traffic should be injected to port '0' in the first FAP,                   \n\r"
    "and received from the port '1' in the first FAP.                               \n\r"
    "All the other ports should be connected with external Ethernet cables.         \n\r"
    "Each port should be connected to port near it (in the same FAP).                                 \n\r"
    "                                                                              \n\r"
    "Motivation:                                                                    \n\r"
    "The snake scheme enables traffic generation of:                                \n\r"
    "11 * Number of FAPs * injected traffic.                                        \n\r"
    "                                                                              \n\r"
    "9. Recycle Multicast (Untagged).                                               \n\r"
    "Destination MAC: 00:AA:AA:00:00:0$DEST_PORTS.                                  \n\r"
    "Source MAC: 00:00:00:BA:BA:00.                                                 \n\r"
    "                                                                              \n\r"
    "How to use:                                                                    \n\r"
    "The traffic should be injected to port 11 in the last FAP,                     \n\r"
    "and receive it from FAPs 0-3 in the following ports:                           \n\r"
    "Note, recycle multicast packets must not be chopped.                          \n\r"
    "                                                                              \n\r"
    "Address      Ports                                                            \n\r"
    "                                                                              \n\r"
    "00AAAA000000  0  6  0  6  0  6  0  6                                                  \n\r"
    "00AAAA000001  1  7  1  7  1  7  1  7                                                  \n\r"
    "00AAAA000002  2  8  2  8  2  8  2  8                                                  \n\r"
    "00AAAA000003  3  9  3  9  3  9  3  9                                                  \n\r"
    "                                                                               \n\r"
    "For example, destination MAC address 00:AA:AA:00:00:01 would result in         \n\r"
    "replicating the packet to the ports 1 and 7 in FAPs 0,1,2 and 3. (8 ports).    \n\r"
    "                                                                               \n\r"
    "Motivation:                                                                    \n\r"
    "Demonstrate the recycling multicast.                                           ";

  send_string_to_screen(help_str,TRUE) ;
  return 0;
}
/*
 */
/*****************************************************
*NAME
*  process_demo_line
*TYPE: PROC
*DATE: 10/AUG/2003
*FUNCTION:
*  Process input line which has an 'line_card' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int process_demo_line(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  int
    err,
    ret,
    ui_ret ;
  PARAM_VAL
    *param_val ;
  unsigned
    int
      match_index,
      len,
      fe_to_access;
  char
    err_msg[80*6] = "",
    *proc_name ;
  BOOL
    get_val_of_err ;
  unsigned
    long
      remote_device_handle ;
  FMC_COMMON_OUT_STRUCT
    out_struct ;
  FMC_COMMON_IN_STRUCT
    in_struct ;
  FAP10M_AGENT_IN_STRUCT
    *fap10m_agent_in_struct ;
  FAP10M_AGENT_OUT_STRUCT
    *fap10m_agent_out_struct ;
  FE200_AGENT_IN_STRUCT
    *fe200_agent_in_struct ;
  FE200_AGENT_OUT_STRUCT
    *fe200_agent_out_struct ;
  char          *soc_sand_proc_name;

  unsigned int
    dst_fap=CSYS_ALL_THE_DEVICES;
  unsigned int
    fap_i,
    fap_exist[CSYS_MAX_NOF_FAP_IN_SYSTEM];
  FAP10M_APP_AGENT_OUT_STRUCT
    app_info[CSYS_MAX_NOF_FAP_IN_SYSTEM];


  soc_sand_proc_name = "Need to init 'soc_sand_proc_name' variable";
  proc_name = "process_demo_line" ;
  ret = 0 ;
  get_val_of_err = FALSE ;
  send_string_to_screen("",TRUE) ;
#if  LINK_PSS_LIBRARIES
  if (!is_master_device() )
  {
    sal_sprintf(err_msg,
            "\r\n\n"
            "*** \'demo commands can only be performed on the master unit.\'\r\n"
    ) ;
    send_string_to_screen(err_msg,TRUE) ;
    ret = TRUE ;
    goto exit ;

  }
#endif
  if(!search_param_val_pairs(current_line,&match_index,PARAM_DEMO_MAC_ADDRESSES_GROUP_ID,1))
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_DEMO_MAC_ADDRESSES_HELP_ID,1))
    {
      negev_demo_print_mac_addresses_help();
      ret = 0;
    }
#if  LINK_PSS_LIBRARIES
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_DEMO_MACS_ADD_UNTAGGED_ANY_TO_ANY,1))
    {
      if(dpss_get_mac_types()->untag_any_to_any)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'add_untagged_any_to_any'\' error\r\n"
                "    The MAC addresses allready exist\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 344 ;
        goto exit ;
      }
      ret = dpss_untag_any_to_any_add(dune_rpc_get_number_of_line_cards());
    }
    else if(!search_param_val_pairs(current_line,&match_index,PARAM_DEMO_MACS_REMOVE_UNTAGGED_ANY_TO_ANY,1))
    {
      if(!(dpss_get_mac_types()->untag_any_to_any))
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'remove_untagged_any_to_any'\' error\r\n"
                "    The MAC addresses not exist\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = 435 ;
        goto exit ;
      }
      ret = dpss_untag_any_to_any_remove(dune_rpc_get_number_of_line_cards());
    }
#endif
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_SET_ID,1))
  {
    FAP10M_SCH_SCHEMES
      scheduling_scheme;

    ret =
      get_val_of(
        current_line,(int *)&match_index,
        PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_SET_ID,1,
        &param_val,VAL_SYMBOL,err_msg
        ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'set_scheduling_scheme\' error\r\n"
              "    or \'get_val_of()\' failed. Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }

    scheduling_scheme = (unsigned int)(param_val->numeric_equivalent) ;
    in_struct.common_union.fap10m_app_struct.proc_id = FAP10M_SET_SCM_CMD ;
    in_struct.common_union.fap10m_app_struct.data.set_scm.sch_scehme = scheduling_scheme;
    ret =
      csys_fap10m_app_cmd(
        dst_fap,
        fap_exist,
        &in_struct.common_union.fap10m_app_struct,
        app_info,
        FALSE
      );
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_ENHANCED_ALLOCATORS_ID,1))
  {
    FAP10M_APP_SCM_ENHANCED_INFO
      scm_enhanced_info;

    soc_sand_proc_name = "enhance update allocators";
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_NEGEV_DEMO_PRIOR_2_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    scm_enhanced_info.prior_2_rate = param_val->value.ulong_value * 1000;

    UI_MACROS_GET_NUMMERIC_VAL(PARAM_NEGEV_DEMO_PRIOR_3_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    scm_enhanced_info.prior_3_rate = param_val->value.ulong_value * 1000;

    UI_MACROS_GET_NUMMERIC_VAL(PARAM_NEGEV_DEMO_PRIOR_4_RATE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    scm_enhanced_info.prior_4_rate = param_val->value.ulong_value * 1000;

    in_struct.common_union.fap10m_app_struct.proc_id =
                        FAP10M_ENHANCE_UPDATE_ALLOCATORS;
    memcpy(
      &(in_struct.common_union.fap10m_app_struct.data.scm_enhanced.inf),
      &scm_enhanced_info,
      sizeof(FAP10M_APP_SCM_ENHANCED_INFO)
    );
    ret =
      csys_fap10m_app_cmd(
        dst_fap,
        fap_exist,
        &in_struct.common_union.fap10m_app_struct,
        app_info,
        FALSE
      );
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_SCHEDULING_SCHEME_GET_ID,1))
  {
    FAP10M_SCH_SCHEMES
      scheduling_scheme;

    in_struct.common_union.fap10m_app_struct.proc_id = FAP10M_GET_SCM_CMD ;
    ret =
      csys_fap10m_app_cmd(
        dst_fap,
        fap_exist,
        &in_struct.common_union.fap10m_app_struct,
        app_info,
        FALSE
      );
    if(!ret)
    {
      for(fap_i=0;fap_i<CSYS_MAX_NOF_FAP_IN_SYSTEM;fap_i++)
      {
        if(fap_exist[fap_i])
        {
          scheduling_scheme =
            app_info[fap_i].data.get_scm.sch_scehme;
          d_printf(
            "The active scheduling scheme in fap %d is %s\n\r",
            fap_i,
            fmc_n_fap10m_FAP10M_SCH_SCHEMES_to_str(scheduling_scheme)
          );
        }
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_SHOW_FLOWS_ID,1))
  {
    unsigned int
      first_flow,
      last_flow;

    ret =
      get_val_of(
        current_line,(int *)&match_index,
        PARAM_NEGEV_DEMO_SHOW_FLOWS_FIRST_FLOW_ID,1,
        &param_val,VAL_NUMERIC,err_msg
        ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'fap10m_print_flow_range\' error: Could not find \'first_flow\' on line\r\n"
              "    or \'get_val_of()\' failed. Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    first_flow = (unsigned int)(param_val->value.ulong_value) ;

    ret =
      get_val_of(
        current_line,(int *)&match_index,
        PARAM_NEGEV_DEMO_SHOW_FLOWS_LAST_FLOW_ID,1,
        &param_val,VAL_NUMERIC,err_msg
        ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'fap10m_print_flow_range\' error: Could not find \'last_flow\' on line\r\n"
              "    or \'get_val_of()\' failed. Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    last_flow = (unsigned int)(param_val->value.ulong_value) ;

    d_printf(
      "Calling fap10m_print_flow_range with flows %d-%d\n\r",
      first_flow,
      last_flow
    );
    fap10m_print_flow_range(
      0,
      first_flow,
      last_flow
    );

  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_PRBS_TEST_ID,1))
  {
    FAP10M_ALL_LINK_CONNECTIVITY
      fap10m_all_link_connectivity ;
    FAP10M_SINGLE_LINK_CONNECTIVITY
      *fap10m_single_link_connectivity ;
    unsigned
      long
        remote_device_handle_fap,
        remote_device_handle_fe,
        remote_slot_id_fap,
        num_on_list,
        list_index,
        list_val,
        list_of_faps[CSYS_MAX_NOF_LINE_CARDS] ;
    SOC_SAND_RET
      error_value ;
    unsigned
      long
        ms_per_tick,
        some_links_are_ok,
        error_in_some,
        all_links_mask,
        mask,
        ip_addr ;
    unsigned
      int
        force_all_links,
        test_time_period_sec,
        fap_index,
        num_faps,
        link_index,
        num_links,
        fe_index,
        ui,
        recycle ;
    char
      curr_ip_str[4*4+2] ;
    char
      *entity_str;
    for (ui = 0 ; ui < (sizeof(list_of_faps)/sizeof(list_of_faps[0])) ; ui++)
    {
      list_of_faps[ui] = (unsigned long)(-1) ;
    }
    /*
     * Calculate: No. of ms per one system tick.
     */
    ms_per_tick = 1000 / CLOCKS_PER_SEC ;
    /*
     * Handle line starting with "negev_demo prbs_test ..."
     */
    ret =
      get_val_of(
        current_line,(int *)&match_index,
        PARAM_NEGEV_DEMO_PRBS_TEST_TIME_PERIOD_ID,1,
        &param_val,VAL_NUMERIC,err_msg
        ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'prbs_test\' error: Could not find \'time_period\' on line\r\n"
              "    or \'get_val_of()\' failed. Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    test_time_period_sec = (unsigned int)(param_val->value.ulong_value) ;
    ret =
      get_val_of(
        current_line,(int *)&match_index,
        PARAM_NEGEV_DEMO_PRBS_TEST_FORCE_ALL_LINKS_ID,1,
        &param_val,VAL_SYMBOL,err_msg
        ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'prbs_test\' error: Could not find \'force_all_links\' on line\r\n"
              "    or \'get_val_of()\' failed. Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    force_all_links = (unsigned int)(param_val->numeric_equivalent) ;
    num_faps = dune_rpc_get_number_of_line_cards() ;
    if (!search_param_val_pairs(current_line,
                      &match_index,PARAM_NEGEV_DEMO_PRBS_TEST_SPECIFIC_FAPS_ID,1))
    {
      /*
       * Handle line starting with "negev_demo prbs_test specific_faps ..."
       */
      {
        /*
         * Get list of FAPs to handle. There must be at least one item.
         */
        num_on_list = (sizeof(list_of_faps)/sizeof(list_of_faps[0])) ;
        /*
         * The following 'for' MUST make at least one loop!
         */
        for (ui = 1 ; ui <= (sizeof(list_of_faps)/sizeof(list_of_faps[0])) ; ui++)
        {
          if (get_val_of(
                current_line,(int *)&match_index,PARAM_NEGEV_DEMO_PRBS_TEST_FAP_LIST_ID,ui,
                &param_val,VAL_NUMERIC,err_msg))
          {
            /*
             * If there are no more data values then quit loop.
             */
            num_on_list = ui - 1 ;
            break ;
          }
          list_val = param_val->value.ulong_value ;
          if ((list_val == 0) || (list_val > num_faps))
          {
            /*
             * Not even one value of write data could not be retrieved.
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** FAP identifier on list (%lu) is out of range (1,%u)!",
                list_val,num_faps
                ) ;
            send_string_to_screen(err_msg,TRUE) ;
            ret = TRUE ;
            goto exit ;
          }
          list_of_faps[ui - 1] = list_val ;
        }
        if (num_on_list == 0)
        {
          /*
           * Not even one value of write data could not be retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Not even one value of FAP to test could not be retrieved!"
              ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
        /*
         * Just precaution...
         */
        if (num_on_list > (sizeof(list_of_faps)/sizeof(list_of_faps[0])))
        {
          /*
           * Too many write data values have been retrieved.
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Too many values of write data (%u) have been retrieved!",
              (unsigned int)num_on_list) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,
                      &match_index,PARAM_NEGEV_DEMO_PRBS_TEST_ALL_FAPS_ID,1))
    {
      /*
       * Handle line starting with "negev_demo prbs_test all_faps ..."
       */
      ui = 0 ;
      for (fap_index = 0 ; fap_index < num_faps ; fap_index++)
      {
        if (ui >= (sizeof(list_of_faps)/sizeof(list_of_faps[0])))
        {
          break ;
        }
        list_of_faps[ui] = (unsigned long)fap_index ;
        ui++ ;
      }
    }
    else
    {
      /*
       * Line does not start with recognizable key words. (software?#?)
       */
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'prbs_test\' error: Could not find either \'all_faps\' or \'specific_faps\'\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    {
      /*
       * Handle line starting with "negev_demo prbs_test ..."
       */
      send_array_to_screen("\r\n",2) ;
      sal_sprintf(err_msg,
          "PRBS testing: Both FAP and FE generate a pseudo random sequence on a\r\n"
          "specified link and each compares received sequence (generated by the\r\n"
          "other) to expected sequence.\r\n"
          "Any value other than \'0\' indicates errors on the link."
          "\r\n"
          ) ;
      send_string_to_screen(err_msg,FALSE) ;
      for (list_index = 0 ;
              list_index < (sizeof(list_of_faps)/sizeof(list_of_faps[0])) ; list_index++)
      {
        if ((int)list_of_faps[list_index] < 0)
        {
          break ;
        }
        fap_index = (unsigned int)list_of_faps[list_index] ;
        ip_addr = dune_rpc_get_ip_of_line_card_at(fap_index) ;
        remote_slot_id_fap = get_slot_number_from_ip_addr(ip_addr) ;
        sprint_ip(curr_ip_str,ip_addr, TRUE) ;
        sal_sprintf(err_msg,
          "\r\n"
          "=========   Handling line card %u: IP address %s, Slot no. %lu   =========\r\n",
          fap_index,curr_ip_str,remote_slot_id_fap
          ) ;
        remote_device_handle_fap = csys_mngment_local_get_unit(SOC_SAND_FAP10M,fap_index);
        send_string_to_screen(err_msg,FALSE) ;
#if DEBUG_NEGEV_DEMO
/* { */
        sal_sprintf(err_msg,
          "Get connectivity map from fap no. %lu (remote device handle 0x%08lX)\r\n",
          fap_index,remote_device_handle_fap
          ) ;
        send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
        in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
        fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
        fap10m_agent_in_struct->proc_id = FAP10M_GET_CONNECTIVITY_MAP ;
        fap10m_agent_in_struct->
            data.just_unit_data.unit = remote_device_handle_fap ;
        err = fmc_send_buffer_to_device(
                SOC_SAND_FAP10M,
                fap_index,
                negev_demo_use_absolute_dev_id(),
                sizeof(in_struct),
                (unsigned char*)&in_struct,
                sizeof(out_struct),
                (unsigned char*)&out_struct
              );
        if(err)
        {
          sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#1)\r\n"
          ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
        error_value =
          soc_sand_get_error_code_from_error_word(out_struct.common_union.fap10m_struct.error_id) ;
        if (error_value != SOC_SAND_OK)
        {
          sal_sprintf(
            err_msg,
            "\r\n\n"
            "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_GET_CONNECTIVITY_MAP\': %u\r\n",
            error_value
          ) ;
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
        fap10m_all_link_connectivity =
          (out_struct.common_union.fap10m_struct.data.
                    links_connectivity_data.all_link_connectivity) ;
        num_links =
          sizeof(fap10m_all_link_connectivity.single_link_connectivity) /
                  sizeof(fap10m_all_link_connectivity.single_link_connectivity[0]) ;
        all_links_mask = (BIT(num_links) - 1) ;
        sal_sprintf(err_msg,
          "Fap no. %u has %u links. ",
          fap_index,num_links
          ) ;
        error_in_some = fap10m_all_link_connectivity.error_in_some & all_links_mask ;
        some_links_are_ok = (~error_in_some) & all_links_mask ;
        if (error_in_some)
        {
          len = strlen(err_msg) ;
          sal_sprintf(&err_msg[len],
            "\r\n"
            "  Special condition is reported on links:") ;
          mask = BIT(0) ;
          fap10m_single_link_connectivity =
            &(fap10m_all_link_connectivity.single_link_connectivity[0]) ;
          for (link_index = 0 ; link_index < num_links ;
                      link_index++, fap10m_single_link_connectivity++, mask <<= 1)
          {
            if (error_in_some & mask)
            {
              len = strlen(err_msg) ;
              sal_sprintf(&err_msg[len]," %u",(unsigned short)link_index) ;
              /*
               * If ALL links are required to be tested, then, for the links
               * which are reported 'down', assume an FE is on the other side.
               */
              if (force_all_links)
              {
                fap10m_single_link_connectivity->other_entity_type = SOC_SAND_ACTUAL_FE2_VALUE ;
              }
            }
          }
        }
        else
        {
          len = strlen(err_msg) ;
          sal_sprintf(&err_msg[len],"ALL links are OK.") ;
        }
        send_string_to_screen(err_msg,TRUE) ;
        /*
         * If all links connected to this FAP are to be tested,
         * whether they are 'up' or 'down', then pretend there are all 'up'.
         */
        if (force_all_links)
        {
          unsigned
            int
              found_any,
              fe_link,
              fe_slot_id,
              device_number ;
          error_in_some = 0 ;
          mask = 0x01 ;
          fap10m_single_link_connectivity =
            &(fap10m_all_link_connectivity.single_link_connectivity[0]) ;
          for (link_index = 0 ; link_index < num_links ;
                      link_index++, fap10m_single_link_connectivity++, mask <<= 1)
          {
            /*
             * Go over all FE slots to find the FE to which this fap link
             * is connected.
             */
            found_any = FALSE ;
            for (fe_slot_id = CSYS_FIRST_SLOT_FABRIC_CARD ;
                    fe_slot_id < (CSYS_FIRST_SLOT_FABRIC_CARD + CSYS_MAX_NOF_FABRIC_CARDS) ;
                    fe_slot_id++)
            {
              err =
                serdes_get_fe_link_given_fap_link(
                  -1,remote_slot_id_fap,fe_slot_id,link_index,
                  &device_number,&fe_link) ;
              if (err > 0)
              {
                sal_sprintf(err_msg,
                        "\r\n\n"
                        "*** \'prbs_test\' error: Failed on get_fe_link_given_fap_link().\r\n"
                ) ;
                send_string_to_screen(err_msg,TRUE) ;
                ret = TRUE ;
                goto exit ;
              }
              else if (err < 0)
              {
                /*
                 * No connection for this specification. Keep trying.
                 */
                continue ;
              }
              else
              {
                /*
                 * Connection found for this specification. Quit.
                 */
                found_any = TRUE ;
                break ;
              }
            }
            if (!found_any)
            {
              error_in_some |= mask ;
              continue ;
            }
#if LINK_PSS_LIBRARIES
            fap10m_single_link_connectivity->
              other_chip_id =
                negev_demo_fe_slot_id_to_unit(fe_slot_id,device_number) ;
            fap10m_single_link_connectivity->other_link = fe_link ;
#endif
          }
          some_links_are_ok = (~error_in_some) & all_links_mask ;
        }
        if (some_links_are_ok)
        {
          unsigned
            int
              fe_link,
              other_chip_id ;
          char
            *error_text ;
          mask = BIT(0) ;
          fap10m_single_link_connectivity =
            &(fap10m_all_link_connectivity.single_link_connectivity[0]) ;
          recycle = FALSE ;
          for (link_index = 0 ; link_index < num_links ;
                      link_index++, mask <<= 1, fap10m_single_link_connectivity++)
          {
            if (link_index == 0)
            {
              sal_sprintf(err_msg,
                "=============================================================================\r\n"
                "Device|Link|Device|Slot|Dev| Checker    |Message                            |\r\n"
                "type  |    |index |    |num| counter    |                                   |\r\n"
                "      |    |ID    |    |   | value      |                                   |\r\n"
                "======|====|======|====|===|============|===================================|\r\n"
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
            if (!(error_in_some & mask))
            {
              if (entity_type_to_string(
                    fap10m_single_link_connectivity->other_entity_type,
                    &entity_str))
              {
                recycle = TRUE ;
                break ;
              }
               other_chip_id = fap10m_single_link_connectivity->other_chip_id ;
               fe_index = fap10m_single_link_connectivity->other_chip_id ;

              {
                FAP10M_B_PRBS_SETUP
                  *fap10m_prbs_setup ;
                FAP10M_AGENT_IN_PRBS_SETUP_STRUCT
                  *fap10m_agent_in_prbs_setup_struct ;
                FE200_PRBS_SETUP
                  *fe200_prbs_setup ;
                FE200_PRBS_GENERATOR_CHECKER_UNIT
                  *fe200_prbs_generator_checker_unit ;
                FE200_AGENT_IN_PRBS_SETUP_STRUCT
                  *fe200_agent_in_prbs_setup_struct ;
                unsigned
                  int
                    fe200_prbs_unit,
                    fe200_prbs_link_on_unit ;
                fe_link = fap10m_single_link_connectivity->other_link ;
#if DEBUG_NEGEV_DEMO
/* { */
                sal_sprintf(err_msg,
                  "Initiate PRBS test on link %02u (on FAP no. %u)\r\n"
                  "  which is connected to link %02u (on SOC_SAND_FE200 no. %u, configured as %s):\r\n",
                  (unsigned short)link_index,
                  (unsigned short)fap_index,
                  (unsigned short)fe_link,
                  (unsigned short)fe_index,
                  entity_str
                  ) ;
                send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                {
                  /*
                   * Set PRBS generator and checker on FAP side.
                   */
                  in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;

                  fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
                  fap10m_agent_in_struct->proc_id = FAP10M_SET_PRBS_MODE ;
                  fap10m_agent_in_prbs_setup_struct = &(fap10m_agent_in_struct->data.prbs_setup_data) ;
                  fap10m_agent_in_prbs_setup_struct->unit = remote_device_handle_fap ;
                  fap10m_prbs_setup = &(fap10m_agent_in_prbs_setup_struct->fap10m_prbs_setup) ;
                  fap10m_prbs_setup->prbs_generator_per_link  = BIT(link_index) ;
                  fap10m_prbs_setup->prbs_checker_link         = link_index ;
                  fap10m_prbs_setup->activate_prbs_checker     = TRUE ;
                  err = fmc_send_buffer_to_device(
                          SOC_SAND_FAP10M,
                          fap_index,
                          negev_demo_use_absolute_dev_id(),
                          sizeof(in_struct),
                          (unsigned char*)&in_struct,
                          sizeof(out_struct),
                          (unsigned char*)&out_struct
                        );
                  if(err)
                   {
                    sal_sprintf(err_msg,
                            "\r\n\n"
                            "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#2)\r\n"
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                  error_value =
                    soc_sand_get_error_code_from_error_word(out_struct.common_union.fap10m_struct.error_id) ;
                  if (error_value != SOC_SAND_OK)
                  {
                    sal_sprintf(
                      err_msg,
                      "\r\n\n"
                      "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_SET_PRBS_MODE\': %u\r\n",
                      error_value
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                }
                {
                  /*
                   * Set PRBS generator and checker on FE side.
                   */
                  remote_device_handle_fe =  csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_index) ;
#if DEBUG_NEGEV_DEMO
/* { */
                  sal_sprintf(err_msg,
                    "Set PRBS mode on FE  no. %lu (remote device handle 0x%08lX)\r\n",
                    fe_index,remote_device_handle_fe
                    ) ;
                  send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                  in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
                  fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
                  fe200_agent_in_struct->proc_id = FE200_SET_PRBS_MODE ;
                  fe200_agent_in_prbs_setup_struct = &(fe200_agent_in_struct->data.prbs_setup_data) ;
                  fe200_agent_in_prbs_setup_struct->unit = remote_device_handle_fe;
                  fe200_prbs_setup = &(fe200_agent_in_prbs_setup_struct->fe200_prbs_setup) ;
                  /*
                   * Make sure all PRBS generators are specified as 'not active'
                   */
                  memset(fe200_prbs_setup,0,sizeof(*fe200_prbs_setup)) ;
                  fe200_prbs_unit = fe_link / 16 ;
                  fe200_prbs_link_on_unit = fe_link - fe200_prbs_unit * 16 ;
                  fe200_prbs_generator_checker_unit =
                          &(fe200_prbs_setup->prbs_generator_checker_unit[fe200_prbs_unit]) ;
                  fe200_prbs_generator_checker_unit->
                        prbs_checker_link   = fe200_prbs_link_on_unit ;
                  fe200_prbs_generator_checker_unit->
                        prbs_generator_link = fe200_prbs_link_on_unit ;
                  fe200_prbs_generator_checker_unit->activate_prbs_generator = TRUE ;

                  err = fmc_send_buffer_to_device(
                          SOC_SAND_DEV_FE200,
                          fe_index,
                          negev_demo_use_absolute_dev_id(),
                          sizeof(in_struct),
                          (unsigned char*)&in_struct,
                          sizeof(out_struct),
                          (unsigned char*)&out_struct
                        );
                  if(err)
                  {
                    sal_sprintf(err_msg,
                            "\r\n\n"
                            "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#3)\r\n"
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                  error_value =
                    soc_sand_get_error_code_from_error_word(out_struct.common_union.fe200_struct.error_id) ;
                  if (error_value != SOC_SAND_OK)
                  {
                    sal_sprintf(
                      err_msg,
                      "\r\n\n"
                      "*** \'fmc_send_buffer_to_device\' error code from \'FE200_SET_PRBS_MODE\': %u\r\n",
                      error_value
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                }
                /*
                 * At this point, both FAP and FE are generating and checking
                 * PRBS on indicated link.
                 */
                /*
                 * Start test on both sides:
                 */
                {
                  /*
                   * Start test on FAP side
                   */
                  {
#if DEBUG_NEGEV_DEMO
/* { */
                    sal_sprintf(err_msg,
                      "Start test (checker counter) on FAP no. %lu (remote handle 0x%08lX)\r\n",
                      fap_index,remote_device_handle_fap
                      ) ;
                    send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                    in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
                    fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
                    fap10m_agent_in_struct->proc_id = FAP10M_START_PRBS_TEST ;
                    fap10m_agent_in_struct->data.just_unit_data.unit = remote_device_handle_fap ;
                    err = fmc_send_buffer_to_device(
                            SOC_SAND_FAP10M,
                            fap_index,
                            negev_demo_use_absolute_dev_id(),
                            sizeof(in_struct),
                            (unsigned char*)&in_struct,
                            sizeof(out_struct),
                            (unsigned char*)&out_struct
                          );
                    if(err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#4)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(out_struct.common_union.fap10m_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_START_PRBS_TEST\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                  }
                  {
                    /*
                     * Start test on FE side
                     */

#if DEBUG_NEGEV_DEMO
/* { */
                    sal_sprintf(err_msg,
                      "Start test (checker counter) on FE  no. %lu (remote handle 0x%08lX)\r\n",
                      fe_index,remote_device_handle_fe
                      ) ;
                    send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
                    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
                    fe200_agent_in_struct->proc_id = FE200_START_PRBS_TEST ;
                    fe200_agent_in_struct->
                      data.just_unit_data.unit = remote_device_handle_fe ;
                    err = fmc_send_buffer_to_device(
                            SOC_SAND_DEV_FE200,
                            fe_index,
                            negev_demo_use_absolute_dev_id(),
                            sizeof(in_struct),
                            (unsigned char*)&in_struct,
                            sizeof(out_struct),
                            (unsigned char*)&out_struct
                          );
                    if(err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#5)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(out_struct.common_union.fe200_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FE200_START_PRBS_TEST\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                  }
                }
                {
                  /*
                   * Wait specified test period
                   */
                  sal_sprintf(err_msg,"Wait %u seconds  ",test_time_period_sec) ;
                  send_string_to_screen(err_msg,FALSE) ;
                  for (ui = 0 ; ui < (test_time_period_sec * 5) ; ui++)
                  {
                    /*
                     * Wait 200 milli seconds.
                     */
                    d_taskDelay(200 / ms_per_tick) ;
                    send_array_to_screen(".",1) ;
                  }
                  send_array_to_screen("\r",1) ;
                }
                {
                  /*
                   * Read counters on both sides:
                   */
                  {
                    /*
                     * FAP counter:
                     */
                    FAP10M_AGENT_DIAGNOSTICS_BER_COUNTERS_REPORT_STRUCT
                      *fap10m_agent_diagnostics_ber_counters_report_struct ;
                    FAP10M_BER_COUNTERS_REPORT
                      *fap10m_ber_counters_report ;
                    FAP10M_SINGLE_BER_COUNTER
                      *fap10m_single_ber_counter ;

                    in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
                    fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
                    fap10m_agent_in_struct->proc_id = FAP10M_GET_BER_COUNTERS ;
                    fap10m_agent_in_struct->
                      data.just_unit_data.unit = remote_device_handle_fap ;
                    err = fmc_send_buffer_to_device(
                            SOC_SAND_FAP10M,
                            fap_index,
                            negev_demo_use_absolute_dev_id(),
                            sizeof(in_struct),
                            (unsigned char*)&in_struct,
                            sizeof(out_struct),
                            (unsigned char*)&out_struct
                          );
                    if(err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#6)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(
                          out_struct.common_union.fap10m_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_GET_BER_COUNTERS\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    fap10m_agent_out_struct = &(out_struct.common_union.fap10m_struct) ;
                    fap10m_agent_diagnostics_ber_counters_report_struct =
                      &(fap10m_agent_out_struct->data.diagnostics_ber_counters_report_data) ;
                    fap10m_ber_counters_report =
                      &(fap10m_agent_diagnostics_ber_counters_report_struct->ber_counters_report) ;
                    fap10m_single_ber_counter =
                      &(fap10m_ber_counters_report->single_ber_counter[0]) ;
                    if (!(fap10m_single_ber_counter->prbs_ber_valid))
                    {
                      sal_sprintf(err_msg,
                        "FAP   | %02u |  %02u  | %02lu | - |     --     |%-35s|\r\n",
                        link_index,fap_index,remote_slot_id_fap,
                        "PRBS stream is not valid!"
                        ) ;
                    }
                    else
                    {
                      if (!(fap10m_single_ber_counter->prbs_error_overflowed))
                      {
                        if (fap10m_single_ber_counter->prbs_error_counter)
                        {
                          error_text = "ERRORS DETECTED!" ;
                        }
                        else
                        {
                          error_text = "NO errors." ;
                        }
                        sal_sprintf(err_msg,
                          "FAP   | %02u |  %02u  | %02lu | - | %10lu |%-35s|\r\n",
                          link_index,fap_index,remote_slot_id_fap,
                          fap10m_single_ber_counter->prbs_error_counter,
                          error_text
                          ) ;
                      }
                      else
                      {
                        sal_sprintf(err_msg,
                          "FAP   | %02u |  %02u  | %02lu | - |     --     |%-35s|\r\n",
                          link_index,fap_index,remote_slot_id_fap,
                          "Checker counter has overflown!"
                          ) ;
                      }
                    }
                    send_string_to_screen(err_msg,FALSE) ;
                  }
                  {
                    /*
                     * FE counter:
                     */
                    unsigned
                      int
                        slot_id,
                        device_number ;
                    FE200_AGENT_DIAGNOSTICS_BER_COUNTERS_REPORT_STRUCT
                      *fe200_agent_diagnostics_ber_counters_report_struct ;
                    FE200_BER_COUNTERS_REPORT
                      *fe200_ber_counters_report ;
                    FE200_SINGLE_BER_COUNTER
                      *fe200_single_ber_counter ;
                    err =
#if LINK_PSS_LIBRARIES
                      negev_demo_fe_unit_to_slot_id(
                        other_chip_id,&slot_id,&device_number) ;
#else
                    1;
#endif
                    if (err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'negev_demo_fe_unit_to_slot_id\' error in \'prbs_test\'\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
                    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
                    fe200_agent_in_struct->proc_id = FE200_GET_BER_COUNTERS ;
                    fe200_agent_in_struct->
                      data.just_unit_data.unit = remote_device_handle_fe ;
                     err = fmc_send_buffer_to_device(
                             SOC_SAND_DEV_FE200,
                             fe_index,
                             negev_demo_use_absolute_dev_id(),
                             sizeof(in_struct),
                             (unsigned char*)&in_struct,
                             sizeof(out_struct),
                             (unsigned char*)&out_struct
                           );
                     if(err)
                     {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#7)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(
                          out_struct.common_union.fe200_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FE200_GET_BER_COUNTERS\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    fe200_agent_out_struct = &(out_struct.common_union.fe200_struct) ;
                    fe200_agent_diagnostics_ber_counters_report_struct =
                      &(fe200_agent_out_struct->data.diagnostics_ber_counters_report_data) ;
                    fe200_ber_counters_report =
                      &(fe200_agent_diagnostics_ber_counters_report_struct->ber_counters_report) ;
                    fe200_prbs_unit = fe_link / 16 ;
                    fe200_single_ber_counter =
                      &(fe200_ber_counters_report->single_ber_counter[fe200_prbs_unit]) ;
                    if (!(fe200_single_ber_counter->prbs_ber_valid))
                    {
                      sal_sprintf(err_msg,
                        "FE    | %02u |  %02u  | %02u | %1u |     --     |%-35s|\r\n",
                        fe_link,fe_index,slot_id,device_number,
                        "PRBS stream is not valid!"
                        ) ;
                    }
                    else
                    {
                      if (!(fe200_single_ber_counter->prbs_error_overflowed))
                      {
                        if (fe200_single_ber_counter->prbs_error_counter)
                        {
                          error_text = "ERRORS DETECTED!" ;
                        }
                        else
                        {
                          error_text = "NO errors." ;
                        }
                        sal_sprintf(err_msg,
                          "FE    | %02u |  %02u  | %02u | %1u | %10lu |%-35s|\r\n",
                          fe_link,fe_index,slot_id,device_number,
                          fe200_single_ber_counter->prbs_error_counter,
                          error_text
                          ) ;
                      }
                      else
                      {
                        sal_sprintf(err_msg,
                          "FE    | %02u |  %02u  | %02u | %1u |     --     |%-35s|\r\n",
                          fe_link,fe_index,slot_id,device_number,
                          "Checker counter has overflown!"
                          ) ;
                      }
                    }
                    send_string_to_screen(err_msg,FALSE) ;
                  }
                }
                {
                  /*
                   * Stop testing (counter collection).
                   */
                  {
                    /*
                     * stop test on FAP side
                     */
#if DEBUG_NEGEV_DEMO
/* { */
                    sal_sprintf(err_msg,
                      "Stop test (checker counter) on FAP no. %lu (remote handle 0x%08lX)\r\n",
                      fap_index,remote_device_handle_fap
                      ) ;
                    send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                    in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
                    fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
                    fap10m_agent_in_struct->proc_id = FAP10M_STOP_PRBS_TEST ;
                    fap10m_agent_in_struct->data.just_unit_data.unit = remote_device_handle_fap ;
                    err = fmc_send_buffer_to_device(
                            SOC_SAND_FAP10M,
                            fap_index,
                            negev_demo_use_absolute_dev_id(),
                            sizeof(in_struct),
                            (unsigned char*)&in_struct,
                            sizeof(out_struct),
                            (unsigned char*)&out_struct
                          );
                    if(err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#8)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(out_struct.common_union.fap10m_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_STOP_PRBS_TEST\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                  }
                  {
                    /*
                     * stop test on FE side
                     */
#if DEBUG_NEGEV_DEMO
/* { */
                    sal_sprintf(err_msg,
                      "Stop test (checker counter) on FE  no. %lu (remote handle 0x%08lX)\r\n",
                      fe_index,remote_device_handle_fe
                      ) ;
                    send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif

                    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
                    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
                    fe200_agent_in_struct->proc_id = FE200_STOP_PRBS_TEST ;
                    fe200_agent_in_struct->
                      data.just_unit_data.unit = remote_device_handle_fe ;
                    err = fmc_send_buffer_to_device(
                            SOC_SAND_DEV_FE200,
                            fe_index,
                            negev_demo_use_absolute_dev_id(),
                            sizeof(in_struct),
                            (unsigned char*)&in_struct,
                            sizeof(out_struct),
                            (unsigned char*)&out_struct
                          );
                    if(err)
                    {
                      sal_sprintf(err_msg,
                              "\r\n\n"
                              "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#9)\r\n"
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                    error_value =
                      soc_sand_get_error_code_from_error_word(out_struct.common_union.fe200_struct.error_id) ;
                    if (error_value != SOC_SAND_OK)
                    {
                      sal_sprintf(
                        err_msg,
                        "\r\n\n"
                        "*** \'fmc_send_buffer_to_device\' error code from \'FE200_STOP_PRBS_TEST\': %u\r\n",
                        error_value
                      ) ;
                      send_string_to_screen(err_msg,TRUE) ;
                      ret = TRUE ;
                      goto exit ;
                    }
                  }
                }
                {
                  /*
                   * Reset PRBS generator and checker on FAP side.
                   */
#if DEBUG_NEGEV_DEMO
/* { */
                  sal_sprintf(err_msg,
                    "Reset PRBS mode on FAP no. %lu (remote device handle 0x%08lX)\r\n",
                    fap_index,remote_device_handle_fap
                    ) ;
                  send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                  in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
                  fap10m_agent_in_struct = &(in_struct.common_union.fap10m_struct) ;
                  fap10m_agent_in_struct->proc_id = FAP10M_SET_PRBS_MODE ;
                  fap10m_agent_in_prbs_setup_struct = &(fap10m_agent_in_struct->data.prbs_setup_data) ;
                  fap10m_agent_in_prbs_setup_struct->unit = remote_device_handle_fap ;
                  fap10m_prbs_setup = &(fap10m_agent_in_prbs_setup_struct->fap10m_prbs_setup) ;
                  fap10m_prbs_setup->prbs_checker_link       = 0 ;
                  fap10m_prbs_setup->prbs_generator_per_link = 0 ;
                  fap10m_prbs_setup->activate_prbs_checker   = FALSE ;
                  err = fmc_send_buffer_to_device(
                          SOC_SAND_FAP10M,
                          fap_index,
                          negev_demo_use_absolute_dev_id(),
                          sizeof(in_struct),
                          (unsigned char*)&in_struct,
                          sizeof(out_struct),
                          (unsigned char*)&out_struct
                        );
                  if(err)
                  {
                    sal_sprintf(err_msg,
                            "\r\n\n"
                            "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#10)\r\n"
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                  error_value =
                    soc_sand_get_error_code_from_error_word(out_struct.common_union.fap10m_struct.error_id) ;
                  if (error_value != SOC_SAND_OK)
                  {
                    sal_sprintf(
                      err_msg,
                      "\r\n\n"
                      "*** \'fmc_send_buffer_to_device\' error code from \'FAP10M_SET_PRBS_MODE\': %u\r\n",
                      error_value
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                }
                {
                  /*
                   * Reset PRBS generator and checker on FE side.
                   */
                  remote_device_handle_fe =  csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_index) ;
#if DEBUG_NEGEV_DEMO
/* { */
                  sal_sprintf(err_msg,
                    "Reset PRBS mode on FE  no. %lu (remote device handle 0x%08lX)\r\n",
                    fe_index,remote_device_handle_fe
                    ) ;
                  send_string_to_screen(err_msg,FALSE) ;
/* } */
#endif
                  in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
                  fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
                  fe200_agent_in_struct->proc_id = FE200_SET_PRBS_MODE ;
                  fe200_agent_in_prbs_setup_struct = &(fe200_agent_in_struct->data.prbs_setup_data) ;
                  fe200_agent_in_prbs_setup_struct->unit = remote_device_handle_fe ;
                  fe200_prbs_setup = &(fe200_agent_in_prbs_setup_struct->fe200_prbs_setup) ;
                  /*
                   * Make sure all PRBS generators are specified as 'not active'
                   */
                  memset(fe200_prbs_setup,0,sizeof(*fe200_prbs_setup)) ;
                  err = fmc_send_buffer_to_device(
                          SOC_SAND_DEV_FE200,
                          fe_index,
                          negev_demo_use_absolute_dev_id(),
                          sizeof(in_struct),
                          (unsigned char*)&in_struct,
                          sizeof(out_struct),
                          (unsigned char*)&out_struct
                        );
                  if(err)
                  {
                    sal_sprintf(err_msg,
                            "\r\n\n"
                            "*** \'fmc_send_buffer_to_device\' error in \'prbs_test\' (#10)\r\n"
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                  error_value =
                    soc_sand_get_error_code_from_error_word(out_struct.common_union.fe200_struct.error_id) ;
                  if (error_value != SOC_SAND_OK)
                  {
                    sal_sprintf(
                      err_msg,
                      "\r\n\n"
                      "*** \'fmc_send_buffer_to_device\' error code from \'FE200_SET_PRBS_MODE\': %u\r\n",
                      error_value
                    ) ;
                    send_string_to_screen(err_msg,TRUE) ;
                    ret = TRUE ;
                    goto exit ;
                  }
                }
              }
            }
            if (link_index == (num_links - 1))
            {
              sal_sprintf(err_msg,
                "=============================================================================\r\n"
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          if (recycle)
          {
            continue ;
          }
        }
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_GET_CONNECTIVITY_MAP_ID,1))
  {
    FAP10M_ALL_LINK_CONNECTIVITY
      connectivity_map ;
    unsigned
      int
        ui,
        dst_fap;
    char
      *entity_str,
      *device_str ;
    /**/
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get connectivity map\' error \r\n"
              "*** or \'unknown parameter \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }

    in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
    in_struct.common_union.fap10m_struct.proc_id = FAP10M_GET_CONNECTIVITY_MAP ;

    dst_fap = param_val->value.ulong_value ;
    in_struct.common_union.fap10m_struct.data.just_unit_data.unit =
      csys_mngment_local_get_unit(SOC_SAND_FAP10M,dst_fap) ;

    err = fmc_send_buffer_to_device(
              SOC_SAND_FAP10M,
              dst_fap,
              negev_demo_use_absolute_dev_id(),
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** 'fmc_send_buffer_to_device' error = %d. \r\n",
                err
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }

    connectivity_map = out_struct.common_union.fap10m_struct.data.links_connectivity_data.all_link_connectivity ;
    fap10m_print_FAP10M_ALL_LINK_CONNECTIVITY(&connectivity_map,0);
    /*
     * lets print the report to the screen
     */
    send_string_to_screen("\r\n",TRUE) ;
    send_string_to_screen(" #|other|other|other|accpt",TRUE) ;
    send_string_to_screen(" #|type | id  |link |cell ",TRUE) ;
    send_string_to_screen(" #|     |     |     |     ",TRUE) ;
    send_string_to_screen("--+-----+-----+-----+-----",TRUE) ;
    for(ui=0 ; ui<9 ; ++ui)
    {
      entity_type_to_string(
                    connectivity_map.single_link_connectivity[ui].other_entity_type,
                    &entity_str) ;
      device_str = negev_demo_fe_unit_to_index(
        connectivity_map.single_link_connectivity[ui].other_chip_id - 1058) ;
      if (!device_str)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'get connectivity map\' error \r\n"
                "*** failed to update fe device id table.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      sal_sprintf(err_msg,"%2d| %3s |  %s  | %02d  |  %c",
        ui,entity_str,device_str,
        connectivity_map.single_link_connectivity[ui].other_link,
       (connectivity_map.single_link_connectivity[ui].accepting_cells?'+':'-')
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_POWER_DOWN_SERDES_ID,1))
  {
    if (search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1) &&
       search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FE_DEVICE_ID,1)
      )
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'power down serdes\' error \r\n"
              "*** or \'expecting fap or fe id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1))
    {
      unsigned int
        dst_fap;
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting fap id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      dst_fap = param_val->value.ulong_value ;


      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      if (FAP10M_NOF_FABRIC_LINKS <= param_val->value.ulong_value)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'fap10m has 9 serdeses [0-8] \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
      in_struct.common_union.fap10m_struct.proc_id = FAP10M_SET_SERDES_POWER_DOWN ;
      in_struct.common_union.fap10m_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fap10m_struct.data.reset_serdes_data.remain_active = TRUE ;

      in_struct.common_union.fap10m_struct.data.just_unit_data.unit =
        csys_mngment_local_get_unit(SOC_SAND_FAP10M,dst_fap) ;

      err = fmc_send_buffer_to_device(
                SOC_SAND_FAP10M,
                dst_fap,
                negev_demo_use_absolute_dev_id(),
                sizeof(in_struct),
                (unsigned char*)&in_struct,
                sizeof(out_struct),
                (unsigned char*)&out_struct
              );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power down serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FE_DEVICE_ID,1))
    {
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_FE_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting fe id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      fe_to_access = param_val->value.ulong_value;

      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
      in_struct.common_union.fe200_struct.proc_id = FE200_RESET_SINGLE_SERDES ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.remain_active = TRUE ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.unit =
        csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_to_access); ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_DEV_FE200,
              fe_to_access,
              negev_demo_use_absolute_dev_id(),
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power down serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power down serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_GRACEFUL_SHUTDOWN_FE_ID,1))
  {

    FE200_AGENT_IN_STRUCT
      *fe200_agent_in_struct ;
    ret = get_val_of(current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_FE_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_shutdown_fe\' error: \'fe#\' field not found on command line.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    fe_to_access = param_val->value.ulong_value ;

    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
    fe200_agent_in_struct->proc_id = FE200_SHUTDOWN_DEVICE ;
    in_struct.common_union.fe200_struct.data.reset_serdes_data.unit =
      csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_to_access); ;

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FE200,
            fe_to_access,
            negev_demo_use_absolute_dev_id(),
            sizeof(in_struct),
            (unsigned char*)&in_struct,
            sizeof(out_struct),
            (unsigned char*)&out_struct
          );
    if(err)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_shutdown_fe\' returned with error \r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
    else
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "--> \'graceful_shutdown_fe\' succeeded \r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_GRACEFUL_RESTORE_FE_ID,1))
  {

    FE200_AGENT_IN_STRUCT
      *fe200_agent_in_struct ;
    ret = get_val_of(current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_FE_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_restore_fe\' error: \'fe#\' field not found on command line.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    fe_to_access = param_val->value.ulong_value ;

    in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
    fe200_agent_in_struct = &(in_struct.common_union.fe200_struct) ;
    fe200_agent_in_struct->proc_id = FE200_ACTIVATE_FE_AND_LINKS ;
    in_struct.common_union.fe200_struct.data.reset_serdes_data.unit =
      csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_to_access); ;

    err = fmc_send_buffer_to_device(
            SOC_SAND_DEV_FE200,
            fe_to_access,
            negev_demo_use_absolute_dev_id(),
            sizeof(in_struct),
            (unsigned char*)&in_struct,
            sizeof(out_struct),
            (unsigned char*)&out_struct
          );
    if(err)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'graceful_restore_fe\' returned with error \r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
    else
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "--> \'graceful_restore_fe\' succeeded \r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_POWER_UP_SERDES_ID,1))
  {
    if (search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1) &&
       search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FE_DEVICE_ID,1)
      )
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'power up serdes\' error \r\n"
              "*** or \'expecting fap or fe id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1))
    {
      unsigned int
        fap_to_access;
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_FAP_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting fap id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      fap_to_access = param_val->value.ulong_value;
      remote_device_handle = csys_mngment_local_get_unit(SOC_SAND_FAP10M, fap_to_access) ;
      if ((unsigned long)-1 == remote_device_handle)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'unknown device id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      if (8 < param_val->value.ulong_value)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'fap10m has 9 serdeses [0-8] \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
      in_struct.common_union.fap10m_struct.proc_id = FAP10M_SET_SERDES_POWER_DOWN ;
      in_struct.common_union.fap10m_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fap10m_struct.data.reset_serdes_data.remain_active = FALSE ;

      in_struct.common_union.fap10m_struct.data.just_unit_data.unit =
        csys_mngment_local_get_unit(SOC_SAND_FAP10M,fap_to_access) ;

      err = fmc_send_buffer_to_device(
                SOC_SAND_FAP10M,
                fap_to_access,
                negev_demo_use_absolute_dev_id(),
                sizeof(in_struct),
                (unsigned char*)&in_struct,
                sizeof(out_struct),
                (unsigned char*)&out_struct
              );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power up serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_FE_DEVICE_ID,1))
    {
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_FE_DEVICE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting fe id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      fe_to_access = param_val->value.ulong_value;

      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SERDES_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' error \r\n"
                "*** or \'expecting serdes# \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      in_struct.out_struct_type = FE200_STRUCTURE_TYPE ;
      in_struct.common_union.fe200_struct.proc_id = FE200_RESET_SINGLE_SERDES ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.serdes_number = param_val->value.ulong_value ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.remain_active = FALSE ;
      in_struct.common_union.fe200_struct.data.reset_serdes_data.unit =
        csys_mngment_local_get_unit(SOC_SAND_DEV_FE200, fe_to_access); ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_DEV_FE200,
              fe_to_access,
              negev_demo_use_absolute_dev_id(),
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'power up serdes\' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
      else
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "--> \'power up serdes\' succeeded \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
      }
    }
  }
  else if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_GET_QOS_PARAMS_ID,1) ||
           !search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_SET_QOS_PARAMS_ID,1)
          )
  {
    unsigned int src_fap,dst_fap,dst_port,
                 class_id,flow_id,queue_id ;

#if NEGEV_DEMO_QOS_USE_SRC_PORT
    unsigned int src_port ;
#endif
    /**/
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_SOURCE_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'expecting source fap# \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    src_fap = param_val->value.ulong_value ;
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_DESTINATION_FAP_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'expecting destination fap# \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    dst_fap = param_val->value.ulong_value ;

#if NEGEV_DEMO_QOS_USE_SRC_PORT
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_SOURCE_PORT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'expecting source port# \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    src_port = param_val->value.ulong_value ;
#endif
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_DESTINATION_PORT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'expecting destination port# \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    dst_port = param_val->value.ulong_value ;
    ret = get_val_of( current_line,(int *)&match_index,
                      PARAM_NEGEV_DEMO_DESTINATION_CLASS_ID,1,
                      &param_val,VAL_NUMERIC,err_msg
                    ) ;
    if (0 != ret)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'expecting class id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    class_id = param_val->value.ulong_value ;
    /**/
    get_flow_and_queue_id(
       src_fap,dst_fap,

#if NEGEV_DEMO_QOS_USE_SRC_PORT
       src_port,
#else
       0,
#endif
       dst_port,class_id,
       &flow_id,&queue_id
    ) ;
    if (-1 == flow_id)
    {
      sal_sprintf(err_msg,
              "\r\n\n"
              "*** \'get QOS params\' error \r\n"
              "*** or \'failed to calculate flow id \'.\r\n"
              "    Probably SW error\r\n"
      ) ;
      send_string_to_screen(err_msg,TRUE) ;
      ret = TRUE ;
      goto exit ;
    }
    if (!search_param_val_pairs(current_line,&match_index,PARAM_NEGEV_DEMO_GET_QOS_PARAMS_ID,1))
    {
      /* get the flow id params:
       *    a. get the target fap remote handle.
       *    b. get (and present) it's flow params.
       */
      /*
       *calculate the remote handle of the fap
       */

      remote_device_handle = csys_mngment_local_get_unit(SOC_SAND_FAP10M,dst_fap) ;

      /**/
      in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
      in_struct.common_union.fap10m_struct.proc_id = FAP10M_GET_SCHEDULER_FLOW_PARAMS ;
      in_struct.common_union.fap10m_struct.data.get_sched_flow_params.sched_flow_id = flow_id ;

      in_struct.common_union.fap10m_struct.data.just_unit_data.unit =
        csys_mngment_local_get_unit(SOC_SAND_FAP10M,dst_fap) ;

      err = fmc_send_buffer_to_device(
              SOC_SAND_FAP10M,
              dst_fap,
              negev_demo_use_absolute_dev_id(),
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );
      if(err)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'get flow params\' returned with error \r\n"
                "   src_fap=%d,dst_fap=%d,dst_port=%d,class=%d \n\r",
                src_fap,dst_fap,dst_port,class_id
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      start_print_services() ;
      send_string_to_screen("",TRUE) ;
      fap10m_b_print_FAP10M_FLOW(
        &out_struct.common_union.fap10m_struct.data.get_scheduler_flow.flow,
        ui_printing_policy_get()
      ) ;
      send_string_to_screen("",TRUE) ;
      end_print_services() ;
    }
    else /* PARAM_NEGEV_DEMO_SET_QOS_PARAMS_ID */
    {
      FAP10M_B_FLOW  flow ;
      /*
       */
      fap10m_clear_FLOW(&flow);

      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SUB_FLOW_0_TYPE_ID,1,
                        &param_val,VAL_SYMBOL,err_msg
                      ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      flow.sub_flow[0].type = (FAP10M_SUB_FLOW_TYPE) param_val->numeric_equivalent ;
      /*
       */
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SUB_FLOW_0_VALUE_ID,1,
                        &param_val,VAL_NUMERIC,err_msg
                      ) ;
      if (0 != ret)
      {
        send_string_to_screen(err_msg,TRUE) ;
        ret = TRUE ;
        goto exit ;
      }
      flow.sub_flow[0].value = param_val->value.ulong_value ;
      /*
       */
      ret = get_val_of( current_line,(int *)&match_index,
                        PARAM_NEGEV_DEMO_SUB_FLOW_1_TYPE_ID,1,
                        &param_val,VAL_SYMBOL,err_msg
                      ) ;
      if (0 != ret)
      {
        /*
         * There is only one sub-flow.
         */
        flow.sub_flow[1].type = FAP10M_UNDEF_FLOW ;
        ret = 0 ;
      }
      else
      {
        flow.sub_flow[1].type = (FAP10M_SUB_FLOW_TYPE) param_val->numeric_equivalent ;
        ret = get_val_of( current_line,(int *)&match_index,
                          PARAM_NEGEV_DEMO_SUB_FLOW_1_VALUE_ID,1,
                          &param_val,VAL_NUMERIC,err_msg
                        ) ;
        if (0 != ret)
        {
          send_string_to_screen(err_msg,TRUE) ;
          ret = TRUE ;
          goto exit ;
        }
        flow.sub_flow[1].value = param_val->value.ulong_value ;
      }

      remote_device_handle = csys_mngment_local_get_unit(SOC_SAND_FAP10M,dst_fap) ;
      if ((unsigned long)-1 == remote_device_handle)
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'set QOS params\' error \r\n"
                "*** or \'unknown device id \'.\r\n"
                "    Probably SW error\r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      /**/
      in_struct.out_struct_type = FAP10M_STRUCTURE_TYPE ;
      in_struct.common_union.fap10m_struct.proc_id = FAP10M_UPDATE_SCHEDULER_FLOW ;
      in_struct.common_union.fap10m_struct.data.update_sched_flow_params.flow_id          = flow_id ;
      in_struct.common_union.fap10m_struct.data.update_sched_flow_params.ingress_queue_id = queue_id ;
      in_struct.common_union.fap10m_struct.data.update_sched_flow_params.sch_port_id      = dst_port ;
      in_struct.common_union.fap10m_struct.data.update_sched_flow_params.source_fap_id    = src_fap-1 ;
      in_struct.common_union.fap10m_struct.data.update_sched_flow_params.flow             = flow ;


    in_struct.common_union.fap10m_struct.data.just_unit_data.unit =
      remote_device_handle;

    err = fmc_send_buffer_to_device(
              SOC_SAND_FAP10M,
              dst_fap,
              negev_demo_use_absolute_dev_id(),
              sizeof(in_struct),
              (unsigned char*)&in_struct,
              sizeof(out_struct),
              (unsigned char*)&out_struct
            );

      if (  err ||
            ( out_struct.common_union.fe200_struct.error_id & 0x00FF )
         )
      {
        sal_sprintf(err_msg,
                "\r\n\n"
                "*** \'update flow params \' returned with error \r\n"
        ) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      start_print_services() ;
      send_string_to_screen("",TRUE) ;
      fap10m_b_print_FAP10M_FLOW(
        &out_struct.common_union.fap10m_struct.data.update_scheduler_flow.exact_flow_given,
        ui_printing_policy_get()
      );
      send_string_to_screen("",TRUE) ;
      end_print_services() ;
    }
  }
  else
  {
    send_string_to_screen(" *** Un-supported demo option",TRUE) ;
    send_string_to_screen(__FILE__,TRUE) ;
    ret= TRUE ;
    goto exit ;
  }
exit:
  return (ret) ;
}



/*
 */
/*****************************************************
*NAME
*  subject_demo
*TYPE: PROC
*DATE: 9/OCT/2003
*FUNCTION:
*  Process input line which has an 'line_card' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int subject_demo(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
  return process_demo_line(
            current_line, current_line_ptr
         ) ;

}
/*
 */
/*****************************************************
*NAME
*  subject_gobi_demo
*TYPE: PROC
*DATE: 9/OCT/2003
*FUNCTION:
*  Process input line which has an 'line_card' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int subject_gobi_demo(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{


  if (! utils_is_crate_gobi(get_crate_type()) )
  {
    d_printf("\r\n\n"
             "*** \'This is not a GOBI platform\'\r\n"
    ) ;
    return -1 ;

  }
  return process_demo_line(
            current_line, current_line_ptr
         ) ;
}
/*
 */
/*******************************************************
*NAME
*  subject_negev_demo
*TYPE: PROC
*DATE: 9/OCT/2003
*FUNCTION:
*  Process input line which has an 'line_card' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*INPUT:
*  SOC_SAND_DIRECT:
*    CURRENT_LINE *current_line -
*      Pointer to prompt line to process.
*    CURRENT_LINE **current_line_ptr -
*      Pointer to prompt line to be displayed after
*      this procedure finishes execution. Caller
*      points this variable to the pointer to
*      the next line to display. If called function wishes
*      to set the next line to display, it replaces
*      the pointer to the next line to display.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    Processing results. See 'current_line_ptr'.
*REMARKS:
*  This procedure should be carried out under 'task_safe'
*  state (i.e., task can not be deleted while this
*  procedure is being carried out).
*SEE ALSO:
 */
int subject_negev_demo(CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr)
{
    if (utils_is_crate_negev(get_crate_type()) == FALSE)
    {
      d_printf("\r\n\n"
               "*** \'This is not a NEGEV platform\'\r\n"
      ) ;
      return -1 ;

    }
  return process_demo_line(
            current_line, current_line_ptr
         ) ;

}



#endif
