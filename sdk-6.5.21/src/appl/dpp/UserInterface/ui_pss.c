/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * IPC_PRIORITY_TYPE
 */

/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * Dune RPC include file.
 */
#include <appl/diag/dpp/dune_rpc.h>
#ifdef SAND_LOW_LEVEL_SIMULATION
/* { */

/* } */
#else
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
#include <usrApp.h>

/*
 * Utilities include file.
 */
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/ui_defx.h>

#include <ui_defi.h>
#include <ui_pure_defi_pss.h>

#include <FMC/fmc_common_agent.h>

#include <appl/diag/dpp/utils_string.h>


#if LINK_PSS_LIBRARIES
/* { */

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/shell/cmdEngine.h>

#include <gtOs/gtGenTypes.h>
#include <gtOs/gtOsMem.h>

#include <galtisAgent/wrapUtil/cmdPresteraUtils.h>
#include <galtisAgent/wrapUtil/cmdNetReceive.h>
#include <galtisAgent/wrapUtil/cmdNetTransmit.h>
#include  <prestera/core/diag/gtDiag.h>
#include  <prestera/tapi/port/gtPortCtrl.h>
#include  <prestera/tapi/private/bridge/gtFdb.h>
#include  <prestera/tapi/port/gtPortStat.h>

#include  <prestera/tapi/bridge/gtBrgFdb.h>
#include  <prestera/tapi/bridge/gtBrgMcGroup.h>
#include  <prestera/tapi/bridge/gtBrgVlan.h>
#include    <prestera/tapi/private/bridge/gtFdbDb.h>

#include    <prestera/tapi/phy/gtSmi.h>
#include    <presteraIpc/common/ipcTypes.h>

extern
  GT_STATUS
    gfdbGetPortLearnStatus(
      IN  GT_LPORT logPort,
      OUT GT_BOOL  *status
    );
extern
  GT_STATUS
    osCacheDmaStatus(
      void
    );


/* } */
#endif /* LINK_PSS_LIBRARIES*/

/* } */
#endif /*SAND_LOW_LEVEL_SIMULATION*/

#if LINK_PSS_LIBRARIES
/* { */
typedef struct
{
  unsigned int gt_mac_counter_enum ;
  char         *gt_mac_counter_short ;
  char         *gt_mac_counter_long ;
} GT_MAC_COUNTERS_EXPLAINED ;
static
  GT_MAC_COUNTERS_EXPLAINED
    Gt_mac_counters_explained[GT_LAST_MAC_COUNTER_NUM] =
{
  {
    GT_GOOD_PKTS_RCV,                                            /* 01 */
    "GT_GOOD_PKTS_RCV",
    "Number of ethernet frames received that are\r\n"
    "    neither bad ethernet frames nor MAC Control pkts.\r\n"
    "    This includes Bridge Control packets (LCAP, BPDU)"
  },
  {
    GT_GOOD_OCTETS_RCV,                                          /* 01 */
    "GT_GOOD_OCTETS_RCV",
    "Sum of lengths of all good ethernet frames\r\n"
    "    received that are neither bad ethernet frames nor\r\n"
    "    MAC Control pkts. This does not include 802.3x\r\n"
    "    pause messages, but it does include Bridge Control\r\n"
    "    packets (LCAP, BPDU)"
  },
  {
    GT_BAD_PKTS_RCV,                                            /* 01 */
    "GT_BAD_PKTS_RCV",
    "number of bad ethernet frames received",
  },
  {
    GT_BAD_OCTETS_RCV,                                          /* 02 */
    "GT_BAD_OCTETS_RCV",
    "Sum of lengths of all bad ethernet frames received",
  },
  {
    GT_BRDC_PKTS_RCV,                                           /* 03 */
    "GT_BRDC_PKTS_RCV",
    "Total number of good packets received that were\r\n"
    "    directed to the broadcast address"
  },
  {
    GT_MC_PKTS_RCV,                                             /* 04 */
    "GT_MC_PKTS_RCV",
    "Total number of good packets received that were\r\n"
    "    directed to a multicast address"
  },
  {
    GT_GOOD_PKTS_SENT,                                          /* 05 */
    "GT_GOOD_PKTS_SENT",
    "Number of ethernet frames sent from this MAC.\r\n"
    "    This does not include 802.3 Flow Control packets,\r\n"
    "    packets dropped due to excessive collision or\r\n"
    "    packets with a Tx Error."
  },
  {
    GT_GOOD_OCTETS_SENT,                                        /* 06 */
    "GT_GOOD_OCTETS_SENT",
    "Sum of lengths of all good ethernet frames sent\r\n"
    "    from this MAC.\r\n"
    "    This does not include 802.3 Flow Control packets,\r\n"
    "    packets dropped due to excessive collision or\r\n"
    "    packets with a Tx Error."
  },
  {
    GT_BRDC_PKTS_SENT,                                          /* 07 */
    "GT_BRDC_PKTS_SENT",
    "Total number of good packets sent that have a\r\n"
    "    broadcast destination MAC address.\r\n"
    "    This does not include 802.3 Flow Control packets,\r\n"
    "    packets dropped due to excessive collision or\r\n"
    "    packets with a Tx Error."
  },
  {
    GT_MC_PKTS_SENT,                                            /* 08 */
    "GT_MC_PKTS_SENT",
    "Total number of good packets sent that have a\r\n"
    "    multicast destination MAC address.\r\n"
    "    This does not include 802.3 Flow Control packets,\r\n"
    "    packets dropped due to excessive collision or\r\n"
    "    packets with a Tx Error."
  },
  {
    GT_PKTS_64_OCTETS,                                          /* 09 */
    "GT_PKTS_64_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are 64 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_PKTS_65TO127_OCTETS,                                     /* 10 */
    "GT_PKTS_65TO127_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are 65 to 127 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_PKTS_128TO255_OCTETS,                                    /* 11 */
    "GT_PKTS_128TO255_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are 128 to 255 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_PKTS_256TO511_OCTETS,                                    /* 12 */
    "GT_PKTS_256TO511_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are 256 to 511 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_PKTS_512TO1023_OCTETS,                                   /* 13 */
    "GT_PKTS_512TO1023_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are 512 to 1023 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_PKTS_1024TOMAX_OCTETS,                                   /* 14 */
    "GT_PKTS_1024TOMAX_OCTETS",
    "Total bytes of received and transmitted Good and\r\n"
    "    Bad frames which are more than 1024 bytes in size.\r\n"
    "    This does not include MAC Control Frames."
  },
  {
    GT_COLLISIONS,                                              /* 15 */
    "GT_COLLISIONS",
    "Total number of collisions seen by the MAC"
  },
  {
    GT_LATE_COLLISIONS,                                         /* 16 */
    "GT_LATE_COLLISIONS",
    "Total number of late collisions seen by the MAC"
  },
  {
    GT_EXCESSIVE_COLLISIONS,                                    /* 17 */
    "GT_EXCESSIVE_COLLISIONS",
    "Number of frames dropped in the transmit MAC due\r\n"
    "    to excessive collisions. This is applicable for\r\n"
    "    Half-Duplex mode only."
  },
  {
    GT_MAC_RCV_ERROR,                                          /* 18 */
    "GT_MAC_RCV_ERROR",
    "Number of Rx Error events seen by the receive side\r\n"
    "    of the MAC"
  },
  {
    GT_MAC_TRANSMIT_ERR,                                       /* 19 */
    "GT_MAC_TRANSMIT_ERR",
    "Number of frames not transmitted correctly or\r\n"
    "    dropped due to internal MAC Tx error"
  },
  {
    GT_BAD_CRC,                                                /* 20 */
    "GT_BAD_CRC",
    "Number of CRC error events"
  },
  {
    GT_UNDERSIZE_PKTS,                                         /* 21 */
    "GT_UNDERSIZE_PKTS",
    "Number of undersize packets received"
  },
  {
    GT_FRAGMENTS_PKTS,                                         /* 22 */
    "GT_FRAGMENTS_PKTS",
    "Number of fragments received"
  },
  {
    GT_OVERSIZE_PKTS,                                          /* 23 */
    "GT_OVERSIZE_PKTS",
    "Number of oversize packets received"
  },
  {
    GT_JABBER_PKTS,                                            /* 24 */
    "GT_JABBER_PKTS",
    "Number of jabber packets received"
  },
  {
    GT_UNRECOG_MAC_CNTR_RCV,                                   /* 25 */
    "GT_UNRECOG_MAC_CNTR_RCV",
    "Number of received MAC Control frames that have an\r\n"
    "    opcode different from 00-01."
  },
  {
    GT_GOOD_FC_RCV,                                            /* 26 */
    "GT_GOOD_FC_RCV",
    "Number of good Flow Control frames received"
  },
  {
    GT_FC_SENT,                                                /* 27 */
    "GT_FC_SENT",
    "Number of Flow Control frames sent"
  },

#if DPSS_VERION_1_25_INCLUDE /*{*/
  {
    GT_DROP_EVENTS,                                                /* 28 */
    "GT_DROP_EVENTS",
    "number of instances that the port was unable toreceive packets\n\r"
    "due to insufficient bandwidth to one of the PP internal resources,\n\r"
    "such as the DRAM or buffer allocation."
  },
#endif  /*}*/

} ;
/*****************************************************
*NAME
*  display_all_mac_counters
*TYPE:
*  PROC
*DATE:
*  18/JULY/2003
*FUNCTION:
*  Print all MAC counters related to specified port.
*INPUT:
*  SOC_SAND_DIRECT:
*    unsigned int logical_port_type -
*      If '0' then this is a standard port. Otherwise, this
*      is a trunk port. In that case, only
*      'logical_port_port' is used to identify the
*      trunk.
*    unsigned int logical_port_unit -
*      Unit on which specified device resides.
*    unsigned int logical_port_device -
*      Device on which specified port resides.
*    unsigned int logical_port_port -
*      Logical port number of specified device for
*      which counters are required.
*    unsigned int display_format -
*      Format of display. If '0' then short format is
*      used. Otherwise, long format is used (full
*      explanation for each counter).
*    unsigned int clear_after_read -
*      Flag. If '0' then consecutive readings are
*      accumulative. Otherwise, counters are restarted
*      after reading.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero then some error has been
*      encountered.
*  SOC_SAND_INDIRECT:
*    Printed info
*REMARKS:
*  Intrefaces gcntGetMacCounters() and
*SEE ALSO:
 */
#define PRINT_COUNTERS_SHORT_FORMAT   0
#define PRINT_COUNTERS_WITH_NO_ZEROS  1

int
  display_all_mac_counters(
    unsigned int logical_port_is_trunk,
    unsigned int logical_port_unit,
    unsigned int logical_port_device,
    unsigned int logical_port_port,
    unsigned int display_format,
    unsigned int clear_after_read
  )
{
  int
    ret ;
  GT_STATUS
    gt_status ;
  GT_LPORT
    logical_port ;
  GT_MAC_COUNTERS
    gt_mac_counter ;
  static
    unsigned
      long
        Monitored_logical_port = 0 ;
  static
    GT_U64
      Counter_value[
        sizeof(Gt_mac_counters_explained) /
            sizeof(Gt_mac_counters_explained[0])];
  static
    unsigned int
      Do_init = TRUE;
  char
    msg[8 * 80] ;
  unsigned
    int
      num_counters,
      counter_index,
      len ;
  ret = 0 ;
  logical_port =
    galtisLport(
      logical_port_is_trunk,logical_port_unit,
      logical_port_device,logical_port_port) ;
  len = 0 ;
  num_counters =
    sizeof(Gt_mac_counters_explained) / sizeof(Gt_mac_counters_explained[0]) ;

  if(Do_init)
  {
    memset(&Counter_value[0],0,sizeof(Counter_value)) ;
    Do_init = FALSE;
  }
  if (Monitored_logical_port != logical_port)
  {
    /*
     * If logical port has been changed then clear counter memory.
     */
    memset(&Counter_value[0],0,sizeof(Counter_value)) ;
    Monitored_logical_port = logical_port ;
  }

  if (logical_port_is_trunk)
  {
    sal_sprintf(msg,
      "\r\n"
      "MAC counters for trunk port %u\r\n",
      (unsigned short)logical_port_port
      ) ;
  }
  else
  {
    sal_sprintf(msg,
      "\r\n"
      "MAC counters for unit %u, device %u, port %u logical_port 0x%lx\r\n",
      (unsigned short)logical_port_unit,
      (unsigned short)logical_port_device,
      (unsigned short)logical_port_port,
      logical_port
      ) ;
  }
  len += strlen(&msg[len]) ;
/*
  sal_sprintf(&msg[len],
      "------------------------------------------------------------\r\n"
      "- Note: 64 bits counters - \'G\' stands for \'Giga\' -----------\r\n"
      "============================================================\r\n"
      ) ;
*/
  send_string_to_screen(msg,FALSE) ;
  for (counter_index = 0 ; counter_index < num_counters ; counter_index++)
  {
    gt_mac_counter = Gt_mac_counters_explained[counter_index].gt_mac_counter_enum ;
    gt_status = gcntGetMacCounters(logical_port,gt_mac_counter,&Counter_value[counter_index]) ;
    if (gt_status != GT_OK)
    {
      ret = (int)gt_status ;
      goto exit ;
    }
    /*
     * Note that for GT_U64, the MS long word is the SECOND!!!
     */

    if(display_format & BIT(PRINT_COUNTERS_WITH_NO_ZEROS))
    {
      if((Counter_value[counter_index].l[0] == 0 )&&
         (Counter_value[counter_index].l[1] == 0 )
        )
      {
        continue;
      }
    }
    if (display_format & BIT(PRINT_COUNTERS_SHORT_FORMAT))
    {
      /*
       * Short format
       */
      sal_sprintf(msg,
        "%02u) %-25.25s: %010lu * 4G + %010lu\r\n",
        (unsigned short)counter_index,
        Gt_mac_counters_explained[counter_index].gt_mac_counter_short,
        (unsigned long)Counter_value[counter_index].l[1],
        (unsigned long)Counter_value[counter_index].l[0]
        ) ;
    }
    else
    {
      /*
       * Long format
       */
      len = 0 ;
      sal_sprintf(msg,
        "%02u) %s\r\n",
        (unsigned short)counter_index,
        Gt_mac_counters_explained[counter_index].gt_mac_counter_long
        ) ;
      len += strlen(&msg[len]) ;
      sal_sprintf(&msg[len],
        "%02u) %-25.25s: %010lu * 4G + %010lu\r\n",
        (unsigned short)counter_index,
        Gt_mac_counters_explained[counter_index].gt_mac_counter_short,
        (unsigned long)Counter_value[counter_index].l[1],
        (unsigned long)Counter_value[counter_index].l[0]
        ) ;
    }
    send_string_to_screen(msg,FALSE) ;
  }
exit:
  if (clear_after_read)
  {
    memset(&Counter_value[0],0,sizeof(Counter_value)) ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  pss_print_vlan_tbl
*TYPE:
*  PROC
*DATE:
*  17/JULY/2003
*FUNCTION:
*  Print full VLAN table.
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    VLAN shadow table. See buildVlanShadow()
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero then some error has been
*      encountered.
*  SOC_SAND_INDIRECT:
*    Printed info
*REMARKS:
*  Intreface for gvlnGetVlanPorts() and get_max_vlan_id();
*SEE ALSO:
 */
int
  pss_print_vlan_tbl(
    void
  )
{
  int
    ret ;
  GT_STATUS
    gt_status ;
  char
    msg[8 * 80] ;
  unsigned
    int
      left,
      num_port_on_line,
      num_active_vlans,
      max_vlan,
      vlan_index,
      ii,
      ports_on_line,
      len ;
  /*
   * allocated space should be equal to
   * for mem_ports : (number of logical ports in the system)*sizeof(GT_LPORT)
   * for is_tagged : (number of logical ports in the system)*sizeof(GT_U8)
   */
  /*
   * Couldn't find an equivalent definition in DPSS (!?)
   */
#define MAX_NUM_LOGICAL_PORTS (MAX_PP_DEVICES * MAX_PP_PORTS_NUM)

  GT_LPORT
    brg_mem_ports_arr[MAX_NUM_LOGICAL_PORTS] ;
  GT_U32
    brg_mem_ports_len;
  GT_U8
    brg_is_tagged_arr[MAX_NUM_LOGICAL_PORTS] ;
  GT_BOOL
    brg_vln_cpu_member ;
  char
    *tagged_text,
    *cpu_member_text ;
  ret = FALSE ;
  num_port_on_line = 5 ;
  max_vlan = get_max_vlan_id() ;
  sal_sprintf(
    msg,
    "\r\n\n"
    "Print VLAN table. Vlan range is 1 -> %u\n\r"
    "==========================================================================\n\r"
    "Indx| Vlan | CPU  | Logical ports (hex). \'T\' stands for \'tagged\'         |\n\r"
    "    |      |      |-------------------------------------------------------\n\r"
    "    |  ID  |member|Port     T|Port     T|Port     T|Port     T|Port     T|\n\r"
    "--------------------------------------------------------------------------\n\r",
    (unsigned short)max_vlan
    ) ;
  send_string_to_screen(msg,FALSE) ;
  num_active_vlans = 0 ;
  for (vlan_index = 1 ; vlan_index <= max_vlan ; vlan_index++)
  {
    brg_mem_ports_len = sizeof(brg_mem_ports_arr) ;
    gt_status =
      gvlnGetVlanPorts(
        vlan_index,brg_mem_ports_arr,
        &brg_mem_ports_len,brg_is_tagged_arr,&brg_vln_cpu_member);
    if (gt_status != GT_OK)
    {
      if (gt_status == GT_NO_SUCH)
      {
        continue ;
      }
      else if (gt_status == GT_BAD_SIZE)
      {
        sal_sprintf(
          msg,
          "\r\n\n"
          "*** \'GT_BAD_SIZE\' indication from gvlnGetVlanPorts(). Quitting.\n\r"
        );
      }
      else
      {
        sal_sprintf(
          msg,
          "\r\n\n"
          "*** General error indication from gvlnGetVlanPorts(). Quitting.\n\r"
        );
      }
      send_string_to_screen(msg,FALSE) ;
      goto exit ;
    }
    num_active_vlans++ ;
    if (brg_vln_cpu_member)
    {
      cpu_member_text = "yes" ;
    }
    else
    {
      cpu_member_text = "no " ;
    }
    len = 0 ;
    sal_sprintf(
      msg,"%04u| %04u |  %s |",
      (unsigned short)num_active_vlans,
      (unsigned short)vlan_index,cpu_member_text
      ) ;
    len += strlen(&msg[len]) ;
    if (brg_mem_ports_len)
    {
      for (ii = 0, ports_on_line = 0 ; ii < brg_mem_ports_len ; ii++)
      {
        if (brg_is_tagged_arr[ii])
        {
          tagged_text = "Y" ;
        }
        else
        {
          tagged_text = "N" ;
        }
        len += strlen(&msg[len]) ;
        sal_sprintf(
          &msg[len],"%08lX %s|",
          (unsigned long)brg_mem_ports_arr[ii],
          tagged_text
          ) ;
        ports_on_line++ ;
        if (ports_on_line >= num_port_on_line)
        {
          ports_on_line = 0 ;
          send_string_to_screen(msg,TRUE) ;
          len = 0 ;
          msg[len] = 0 ;
          sal_sprintf(
            msg,"    |      |      |"
            ) ;
          len += strlen(&msg[len]) ;
        }
      }
      if (ports_on_line)
      {
        left = num_port_on_line - ports_on_line ;
        for (ii = 0 ; ii < left ; ii++)
        {
          len += strlen(&msg[len]) ;
          sal_sprintf(
            &msg[len],"          |"
            ) ;
        }
        send_string_to_screen(msg,TRUE) ;
      }
    }
    else
    {
      sal_sprintf(
        &msg[len],
        "          |          |          |           |          |\r\n"
        ) ;
      send_string_to_screen(msg,FALSE) ;
    }
  }
  sal_sprintf(
    msg,
    "    |      |      |          |          |          |          |          |\n\r"
    "--------------------------------------------------------------------------\n\r"
    ) ;
  send_string_to_screen(msg,FALSE) ;
  if (num_active_vlans == 0)
  {
    sal_sprintf(
      msg,
      "\r\n\n"
      "==> NO registered VLANs have been found!!!\n\r"
      ) ;
    send_string_to_screen(msg,FALSE) ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  pss_print_mac_fdb_tbl
*TYPE: PROC
*DATE: 10MJULY/2003
*FUNCTION:
*  Print the MAC table for specific VLAN.
*  Intreface for gfdbGetFdbEntryFirst() and gfdbGetFdbEntryNext();
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    gfdbGetFdbEntryFirst,
*    gfdbGetFdbEntryNext
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero then some error has been
*      encountered. E
*  SOC_SAND_INDIRECT:
*    Printed info
*REMARKS:
*  None
*SEE ALSO:
 */
int
  pss_print_mac_fdb_tbl(
    GT_U16 vlan_id
  )
{
  int
    ret,
    found_mac_i;
  GT_MAC_VL
    vlan_mode;
  GT_LPORT
    logic_port = 0;
  GT_MAC_ENTRY_STATUS
    out_state;
  GT_ETHERADDR
    mac_addr;
  GT_STATUS
    gt_status;
  char
    msg[8*80] ;
  unsigned
    int
      ui,
      len ;
  GT_U8
    source_traffic_class,
    destination_traffic_class ;
  MAC_FDB_SHADOW
    fdb_entry ;
  GT_BOOL
    multiple ;
  ret = FALSE;
  gfdbGetMacVlMode(&vlan_mode);
  if (vlan_mode != GT_IVL)
  {
    ret = TRUE;
    send_string_to_screen(
      "*** This function can be called only in 'GT_IVL - Independent VLAN Learning'\n\r"
      "    Currentlly the values are: GT_SVL - Shared VLAN Learning.",
      TRUE
    );
    goto exit;
  }
  gt_status = gfdbDbGetFirstEntry(vlan_id,&fdb_entry) ;
  if (gt_status != GT_OK)
  {
    if (gt_status == GT_NO_SUCH)
    {
      sal_sprintf(
        msg,
        "*** GT_NO_SUCH -- on vlan_id = %u\n\r",
        vlan_id
      );
    }
    else
    {
      sal_sprintf(
        msg,
        "*** Error indication from gfdbDbGetFirstEntry() --\r\n"
        "    on request for vlan_id = %u\n\r",
        vlan_id
      );
    }
    send_string_to_screen(msg, TRUE) ;
    goto exit;
  }
  sal_sprintf(
    msg,
    "--------------------------\n\r"
    " MAC table for vlan %u",
    vlan_id
  );
  send_string_to_screen(msg, TRUE);
  found_mac_i = 1;
  while (gt_status == GT_OK)
  {
    mac_addr = fdb_entry.macAddr ;
    logic_port = fdb_entry.port ;
    out_state = fdb_entry.state ;
    source_traffic_class = fdb_entry.srcTc ;
    destination_traffic_class = fdb_entry.dstTc ;
    multiple = fdb_entry.multiple ;
    /*
     * Print the current line.
     */
    sal_sprintf(msg,"%i) ", found_mac_i);
    send_string_to_screen(msg, FALSE);
    sal_sprintf(msg,
            "0x%04X%08lX ; ",
            (unsigned short)GT_HW_MAC_HIGH16(&mac_addr),
            (unsigned long)GT_HW_MAC_LOW32(&mac_addr)) ;
    send_string_to_screen(msg, FALSE) ;
    sal_sprintf(msg,"%s ; ",
            (out_state==GT_LRND ? "GT_LRND - Dynamically learnt":
            (out_state==GT_MGMT ? "GT_MGMT - Created by CPU, static":
                                  "GT_NOT_STATIC - Created by CPU, dynamic"))
            );
    len = strlen(msg) ;
    sal_sprintf(
      &msg[len],
      "CLASS (src;dest): (%u;%u)",
      (unsigned short)source_traffic_class,
      (unsigned short)destination_traffic_class
    );
    send_string_to_screen(msg, TRUE);
    if ((IS_MAC_MULTICAST(mac_addr.arEther)) || (multiple))
    {
      GT_LPORT
        port_list[50] ;
      unsigned
        long
          port_list_size ;
      GT_BOOL
        is_cpu_member ;

      port_list_size = 50;
        gt_status =
        gmcGetMcEgressPorts(
          &mac_addr,vlan_id,
          port_list,&port_list_size,
          &is_cpu_member
        ) ;
      if (gt_status != GT_OK)
      {
        if (gt_status == GT_NO_SUCH)
        {
          sal_sprintf(
            msg,
            "   MULTICAST: No definition found for this address"
          ) ;
        }
        else if (gt_status == GT_BAD_VALUE)
        {
          sal_sprintf(
            msg,
            "***************************\n\r"
            "   portListSize less than number of members in group\n\r"
            "   pss_print_mac_fdb_tbl() - ERROR:\n\r"
            "   'port_list' array has 50 entries\n\r"
            "   'gmcGetMcEgressPorts' needs more. Consider making 'port_list' array bigger.\n\r"
            "***************************\n\r"
          ) ;
        }
        else
        {
          sal_sprintf(
            msg,
            "   MULTICAST: Error indication from gmcGetMcEgressPorts()"
          ) ;
        }
      }
      else
      {
        GT_U16
          vidx;
        vidx = 0;
        gt_status =
          gmcGetMcGroupId(
            &mac_addr,
            vlan_id,
            &vidx
          );
        if (gt_status != GT_OK)
        {
        sal_sprintf(
        msg,
        "   MULTICAST: Error indication from gmcGetMcGroupId()"
       ) ;
        }
      sal_sprintf(
        msg,
        "   MULTICAST: VidX-%u.%s Defined over %lu logical ports -",
        vidx, (is_cpu_member? " Cpu is member.":""),port_list_size
       ) ;
        send_string_to_screen(msg, FALSE) ;


        len = 0 ;
        msg[len] = 0 ;
        for (ui = 0 ; ui < port_list_size ; ui++)
        {
          if ((ui % 8) == 0)
          {
          sal_sprintf(&msg[len],"\r\n   0x") ;
            send_string_to_screen(msg,FALSE) ;
            len = 0 ;
            msg[len] = 0 ;
          }
          sal_sprintf(&msg[len],"%08lX ",port_list[ui]) ;
          len += strlen(&msg[len]) ;
        }
      }
    }
    else
    {
    sal_sprintf(
        msg,
        "   Logic-port:0x%08lX, port-%u, device-%u, unit-%u (trunk-%u) ; %s",
        logic_port,
        GT_LPORT_2_PORT(logic_port),
        GT_LPORT_2_DEV(logic_port),
        GT_LPORT_2_UNIT(logic_port),
        GT_LPORT_IS_TRUNK(logic_port),
        "UNICAST"
       ) ;
    }
    send_string_to_screen(msg, TRUE);
    /*
     * Serach for next entry.
     */
    gt_status = gfdbDbGetNextEntry(vlan_id,&mac_addr,&fdb_entry) ;
    found_mac_i ++;
  }
exit:
  return ret;
}
/*****************************************************
*NAME
*  display_dpss_system_info
*TYPE: PROC
*DATE: 25/MARCH/2003
*FUNCTION:
*  Display distributed system info (each unit with
*  its devices. Each device with its ports).
*CALLING SEQUENCE:
*  display_dpss_system_info()
*INPUT:
*  SOC_SAND_DIRECT:
*    None
*  SOC_SAND_INDIRECT:
*    gt_dpss_config_params, appDemoPpConfigList,
*    globalSysConf.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      if non-zero then some error has been
*      encountered. Error description is in
*      info_string.
*  SOC_SAND_INDIRECT:
*    Printed info
*REMARKS:
*  None
*SEE ALSO:
 */
int
  display_dpss_system_info(
    void
  )
{
  char
    *c_ptr ;
  int
    ret,
    err ;
  unsigned
    int
      num_units,
      num_devices,
      num_ports,
      ui,
      uj ;
  unsigned
    long
      ip_addr,
      reg_port,
      urg_port ;
  char
    info_string[80*10],
    *ascii_unit_type,
    ascii_ip_addr[4*4+2] ;
  ret = 0 ;
  c_ptr = info_string ;
  num_units = get_num_units() ;
  sal_sprintf(c_ptr,
    "DPSS system info\r\n"
    "================\r\n"
    "Maximal number of Vlans: %u\r\n"
    "Maximal number of\r\n"
    "  MAC multicast groups : %u\r\n"
    "Number of units        : %u\r\n"
    "Number of devices      : %u\r\n"
    "Number of ports        : %u\r\n",
    (unsigned short)get_max_vlan_id(),
    (unsigned short)get_max_mac_multicast_groups(),
    (unsigned short)num_units,
    (unsigned short)get_total_num_devices(),
    (unsigned short)get_total_num_ports()
    ) ;
  send_string_to_screen(info_string,FALSE) ;
  for (ui = 0 ; ui < num_units ; ui++)
  {
    c_ptr = info_string ;
    err = get_num_devices_on_unit(ui) ;
    if (err < 0)
    {
      sal_sprintf(c_ptr,
        "*** Error value %d from \'get_num_devices_on_unit()\'\r\n",
        err) ;
      ret = 1 ;
      goto exit ;
    }
    num_devices = (unsigned int)err ;
    err =
      get_ip_of_unit(ui,&ip_addr) ;
    if (err < 0)
    {
      sal_sprintf(c_ptr,
        "*** Error value %d from \'get_ip_of_unit()\'\r\n",
        err) ;
      ret = 2 ;
      goto exit ;
    }
    sprint_ip(ascii_ip_addr, ip_addr, TRUE) ;
    err =
      get_urgent_tcp_port_of_unit(ui,&urg_port) ;
    if (err < 0)
    {
      sal_sprintf(c_ptr,
        "*** Error value %d from \'get_urgent_tcp_port_of_unit()\'\r\n",
        err) ;
      ret = 3 ;
      goto exit ;
    }
    err =
      get_regular_tcp_port_of_unit(ui,&reg_port) ;
    if (err < 0)
    {
      sal_sprintf(c_ptr,
        "*** Error value %d from \'get_regular_tcp_port_of_unit()\'\r\n",
        err) ;
      ret = 4 ;
      goto exit ;
    }
    err =
      get_type_of_unit_ascii(ui,&ascii_unit_type) ;
    if (err < 0)
    {
      sal_sprintf(c_ptr,
        "*** Error value %d from \'get_type_of_unit_ascii()\'\r\n",
        err) ;
      ret = 5 ;
      goto exit ;
    }
    sal_sprintf(c_ptr,
      "  Unit no. %u:\r\n"
      "    Unit type        : %s\r\n"
      "    IP address       : %s\r\n"
      "    TCP ports -\r\n"
      "      urgent/regular : %u,%u\r\n"
      "    Number of devices: %u\r\n",
      (unsigned short)ui,
      ascii_unit_type,
      ascii_ip_addr,
      (unsigned short)urg_port,
      (unsigned short)reg_port,
      (unsigned short)num_devices) ;
    c_ptr += strlen(c_ptr) ;
    for (uj = 0 ; uj < num_devices ; uj++)
    {
      err = get_num_ports_on_device_on_unit(ui,uj) ;
      if (err < 0)
      {
        sal_sprintf(c_ptr,
          "*** Error value %d from \'get_num_ports_on_device_on_unit()\'\r\n",
          err) ;
        ret = 6 ;
        goto exit ;
      }
      num_ports = (unsigned int)err ;
      sal_sprintf(c_ptr,
        "      Device no. %u:\r\n"
        "        Number of ports: %u\r\n",
        uj,num_ports) ;
      c_ptr += strlen(c_ptr) ;
    }
    send_string_to_screen(info_string,FALSE) ;
  }
exit:
  if (ret)
  {
    send_string_to_screen(info_string,FALSE) ;
  }
  return (ret) ;
}
/*****************************************************
*NAME
*  is_legit_mac_address
*TYPE: PROC
*DATE: 24/MARCH/2003
*FUNCTION:
*  Check whether pointed string is a legitimate
*  mac address (12 hex digits).
*CALLING SEQUENCE:
*  is_legit_mac_address(in_mac)
*INPUT:
*  SOC_SAND_DIRECT:
*    char       *in_mac -
*      This is a null terminated string: The MAC
*      address to test.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then this is a legitimate MAC
*      address.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  is_legit_mac_address(
    char       *in_mac
  )
{
  int
    ret ;
  unsigned
    int
      ui,
      len ;
  char
    *c_ptr ;
  ret = 0 ;
  len = strlen(in_mac) ;\
  if (len != 12)
  {
    goto exit ;
  }
  c_ptr = in_mac ;
  for (ui = 0 ; ui < len ; ui++)
  {
    if (!isxdigit((int)(*c_ptr++)))
    {
      goto exit ;
    }
  }
  ret = 1 ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  gt_field_to_val
*TYPE: PROC
*DATE: 26/MARCH/2003
*FUNCTION:
*  Convert Galtis field from status string (as
*  returned from 'cmdEventRun') to unsigned long
*  variable or to string variable.
*CALLING SEQUENCE:
*  gt_field_to_val(status_string,out_val,index,
*     expect_numeric,string_size)
*INPUT:
*  SOC_SAND_DIRECT:
*    char           *status_string -
*      Status string as per PSS standards (That is,
*      something like "@@@1!!!1???4???_!!!###" where
*      the hex number between,say, "!!!" and
*      "???_" is the the first 'field' (ignore the
*      underscores, they're there to make sure the
*      preprocessor doesn't interpret ??x strings as
*      trigraphs).
*    void  *out_val -
*      This procedure loads pointed memory with
*      value of indicared field within 'status_string'.
*      If 'expect_numeric' is non-zero then this is
*      a pointer to an unsigned long variable.
*      Otherwise, it is a pointer to an array of
*      characters of size 'string_size'.
*    unsigned int   index -
*      The number of the field to retrieve. Note
*      that all fields are followed by "???".
*      Numbering starts from '0'.
*    unsigned int   expect_numeric -
*      Flag. If non-zero then field is expected
*      to be numeric (unsigned long). Otherwise,
*      it is expected to be a string.
*    unsigned int   string_size -
*      Only relevant if 'expect_numeric' is zero:
*      This is the size of the string pointed by
*      out_val. Must be at least '2'.
*      This program expects this size to be
*      sufficient for containing indicated field
*      plus an ending null. If it is not, then
*      this program will flag an error (negative
*      direct output) but will return the first
*      string_size-1 (ASCII) bytes, followed by an
*      ending null.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred. if
*      negative then error was
*  SOC_SAND_INDIRECT:
*    'out_string'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  gt_field_to_val(
    char           *status_string,
    void           *out_val,
    unsigned int   index,
    unsigned int   expect_numeric,
    unsigned int   string_size
  )
{
  int
    ret ;
  char
    *out_val_char,
    cc,
    hex_number[(2 * sizeof(long)) + 1],
    *r_ptr,
    *d_ptr,
    *c_ptr,
    *galtis_field_delimiter,
    *galtis_argument_delimiter ;
  unsigned
    int
      local_index,
      code_val,
      hex_number_len,
      galtis_field_delimiter_len,
      galtis_argument_delimiter_len,
      ui ;
  unsigned
    long
      *out_val_long ;

  out_val_long = NULL;
  out_val_char = NULL;
  ret = 0 ;
  galtis_field_delimiter = GALTIS_FIELD_DELIMITER ;
  galtis_argument_delimiter = GALTIS_ARGUMENT_DELIMITER ;
  galtis_field_delimiter_len = strlen(galtis_field_delimiter) ;
  galtis_argument_delimiter_len = strlen(galtis_argument_delimiter) ;
  hex_number_len = sizeof(hex_number) - 1 ;
  if (expect_numeric)
  {
    out_val_long = (unsigned long *)out_val ;
    *out_val_long = 0 ;
  }
  else
  {
    if (string_size <= 2)
    {
      ret = 1 ;
      goto exit ;
    }
    out_val_char = (char *)out_val ;
    *out_val_char = 0 ;
  }
  /*
   * Find the first occurrence of "???". Just before it, is the first
   * field.
   */
  c_ptr = strstr(status_string,galtis_field_delimiter) ;
  if (c_ptr == (char *)0)
  {
    ret = 2 ;
    goto exit ;
  }
  /*
   * Now find the last occurrence of "!!!" preceding "???"
   */
  r_ptr = status_string ;
  while (TRUE)
  {
    d_ptr = strstr(r_ptr,galtis_argument_delimiter) ;
    if (d_ptr == (char *)0)
    {
      break ;
    }
    if (d_ptr > c_ptr)
    {
      break ;
    }
    r_ptr = d_ptr ;
    r_ptr += galtis_argument_delimiter_len ;
  }
  if (r_ptr == status_string)
  {
    ret = 3 ;
    goto exit ;
  }
  /*
   * At this point, r_ptr points to the first field
   * while c_ptr points to the "???"
   * following it.
   */
  local_index = 0 ;
  while (TRUE)
  {
    if (local_index++ == index)
    {
      if (expect_numeric)
      {
        for (ui = 0 ; ui < hex_number_len ; ui++)
        {
          cc = *r_ptr++ ;
          if (cc == galtis_field_delimiter[0])
          {
            if (strncmp(r_ptr,&galtis_field_delimiter[1],
                        galtis_field_delimiter_len - 1) != 0)
            {
              ret = 4 ;
              goto exit ;
            }
            break ;
          }
          if (!isxdigit((int)cc))
          {
            ret = 5 ;
            goto exit ;
          }
          hex_number[ui] = cc ;
        }
        if (ui == 0)
        {
          ret = 6 ;
          goto exit ;
        }
        if (ui >= hex_number_len)
        {
          if (strncmp(r_ptr,galtis_field_delimiter,
                        galtis_field_delimiter_len) != 0)
          {
            ret = 7 ;
            goto exit ;
          }
        }
        hex_number[ui] = 0 ;
        code_val = strtoul(hex_number,(char **)0,16) ;
        *out_val_long = code_val ;
        break ;
      }
      else
      {
        for (ui = 0 ; ui < string_size ; ui++)
        {
          cc = *r_ptr++ ;
          if (cc == galtis_field_delimiter[0])
          {
            if (strncmp(r_ptr,&galtis_field_delimiter[1],
                        galtis_field_delimiter_len - 1) != 0)
            {
              ret = 8 ;
              goto exit ;
            }
            break ;
          }
          *out_val_char++ = cc ;
        }
        if (ui == 0)
        {
          ret = 9 ;
          goto exit ;
        }
        else if (ui > string_size)
        {
          ret = 10 ;
          goto exit ;
        }
        else if (ui == string_size)
        {
          ret = -1 ;
          out_val_char-- ;
        }
        *out_val_char = 0 ;
        break ;
      }
    }
    else
    {
      for (ui = 0 ; ; ui++)
      {
        cc = *r_ptr++ ;
        if (cc == galtis_field_delimiter[0])
        {
          break ;
        }
        if (cc == 0)
        {
          ret = 11 ;
          goto exit ;
        }
      }
      if (ui == 0)
      {
        ret = 12 ;
        goto exit ;
      }
    }
    if (strncmp(&galtis_field_delimiter[1],r_ptr,galtis_field_delimiter_len - 1) != 0)
    {
      ret = 13 ;
      goto exit ;
    }
    r_ptr += (galtis_field_delimiter_len - 1) ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  gt_argument_to_val
*TYPE: PROC
*DATE: 27/MARCH/2003
*FUNCTION:
*  Convert Galtis argument from status string (as
*  returned from 'cmdEventRun') to unsigned long
*  variable or to string variable.
*CALLING SEQUENCE:
*  gt_argument_to_val(status_string,out_val,index,
*     expect_numeric,string_size)
*INPUT:
*  SOC_SAND_DIRECT:
*    char           *status_string -
*      Status string as per PSS standards (That is,
*      something like "@@@1!!!###" where the hex number
*      between @@@ and !!! is the first argument).
*    void           *out_val -
*      This procedure loads pointed memory with
*      value of indicared argument within 'status_string'.
*      If 'expect_numeric' is non-zero then this is
*      a pointer to an unsigned long variable.
*      Otherwise, it is a pointer to an array of
*      characters of size 'string_size'.
*    unsigned int   index -
*      The number of the argument to retrieve. Note
*      that all arguments are followed by "!!!".
*      Numbering starts from '0'.
*    unsigned int   expect_numeric -
*      Flag. If non-zero then argument is expected
*      to be numeric (unsigned long). Otherwise,
*      it is expected to be a string.
*    unsigned int   string_size -
*      Only relevant if 'expect_numeric' is zero:
*      This is the size of the string pointed by
*      out_val. This program expects this size
*      to be sufficient for containing indicated
*      field plus an ending null (If it is not,
*      then this program will flag an error).
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    'out_string'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  gt_argument_to_val(
    char           *status_string,
    void           *out_val,
    unsigned int   index,
    unsigned int   expect_numeric,
    unsigned int   string_size
  )
{
  int
    ret ;
  char
    *out_val_char,
    cc,
    hex_number[(2 * sizeof(long)) + 1],
    *r_ptr,
    *d_ptr,
    *c_ptr,
    *galtis_message_starter,
    *galtis_argument_delimiter ;
  unsigned
    int
      local_index,
      code_val,
      hex_number_len,
      galtis_message_starter_len,
      galtis_argument_delimiter_len,
      ui ;
  unsigned
    long
      *out_val_long ;

  out_val_char = NULL;
  out_val_long = NULL;
  ret = 0 ;
  galtis_message_starter = GALTIS_MESSAGE_STARTER ;
  galtis_argument_delimiter = GALTIS_ARGUMENT_DELIMITER ;
  galtis_message_starter_len = strlen(galtis_message_starter) ;
  galtis_argument_delimiter_len = strlen(galtis_argument_delimiter) ;
  hex_number_len = sizeof(hex_number) - 1 ;
  if (expect_numeric)
  {
    out_val_long = (unsigned long *)out_val ;
    *out_val_long = 0 ;
  }
  else
  {
    out_val_char = (char *)out_val ;
    *out_val_char = 0 ;
  }
  /*
   * Find the first occurrence of "!!!". Just before it, is the first
   * field.
   */
  c_ptr = strstr(status_string,galtis_argument_delimiter) ;
  if (c_ptr == (char *)0)
  {
    ret = 1 ;
    goto exit ;
  }
  /*
   * Now find the last occurrence of "@@@" preceding "!!!"
   */
  r_ptr = status_string ;
  while (TRUE)
  {
    d_ptr = strstr(r_ptr,galtis_message_starter) ;
    if (d_ptr == (char *)0)
    {
      break ;
    }
    if (d_ptr > c_ptr)
    {
      break ;
    }
    r_ptr = d_ptr ;
    r_ptr += galtis_message_starter_len ;
  }
  if (r_ptr == status_string)
  {
    ret = 2 ;
    goto exit ;
  }
  /*
   * At this point, r_ptr points to the first argument
   * while c_ptr points to the "!!!" following it.
   */
  local_index = 0 ;
  while (TRUE)
  {
    if (local_index++ == index)
    {
      if (expect_numeric)
      {
        for (ui = 0 ; ui < hex_number_len ; ui++)
        {
          cc = *r_ptr++ ;
          if (cc == galtis_argument_delimiter[0])
          {
            if (strncmp(r_ptr,&galtis_argument_delimiter[1],
                        galtis_argument_delimiter_len - 1) != 0)
            {
              ret = 3 ;
              goto exit ;
            }
            break ;
          }
          if (!isxdigit((int)cc))
          {
            ret = 4 ;
            goto exit ;
          }
          hex_number[ui] = cc ;
        }
        if (ui == 0)
        {
          ret = 5 ;
          goto exit ;
        }
        if (ui >= hex_number_len)
        {
          if (strncmp(r_ptr,galtis_argument_delimiter,
                        galtis_argument_delimiter_len) != 0)
          {
            ret = 6 ;
            goto exit ;
          }
        }
        hex_number[ui] = 0 ;
        code_val = strtoul(hex_number,(char **)0,16) ;
        *out_val_long = code_val ;
        break ;
      }
      else
      {
        for (ui = 0 ; ui < string_size ; ui++)
        {
          cc = *r_ptr++ ;
          if (cc == galtis_argument_delimiter[0])
          {
            if (strncmp(r_ptr,&galtis_argument_delimiter[1],
                        galtis_argument_delimiter_len - 1) != 0)
            {
              ret = 7 ;
              goto exit ;
            }
            break ;
          }
          *out_val_char++ = cc ;
        }
        if (ui == 0)
        {
          ret = 8 ;
          goto exit ;
        }
        if (ui >= string_size)
        {
          ret = 9 ;
          goto exit ;
        }
        *out_val_char = 0 ;
        break ;
      }
    }
    else
    {
      for (ui = 0 ; ; ui++)
      {
        cc = *r_ptr++ ;
        if (cc == galtis_argument_delimiter[0])
        {
          break ;
        }
        if (cc == 0)
        {
          ret = 10 ;
          goto exit ;
        }
      }
      if (ui == 0)
      {
        ret = 11 ;
        goto exit ;
      }
    }
    if (strncmp(&galtis_argument_delimiter[1],r_ptr,galtis_argument_delimiter_len - 1) != 0)
    {
      ret = 12 ;
      goto exit ;
    }
    r_ptr += (galtis_argument_delimiter_len - 1) ;
  }
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  gt_status_to_string
*TYPE: PROC
*DATE: 24/MARCH/2003
*FUNCTION:
*  Convert Galtis status (as returned from
*  'cmdEventRun') to ASCII string.
*CALLING SEQUENCE:
*  gt_status_to_string(status_string,out_string)
*INPUT:
*  SOC_SAND_DIRECT:
*    char       *status_string -
*      Status string as per PSS standards (That is,
*      something like "@@@1!!!###" where the hex number
*      between @@@ and !!! is the identifier of the
*      status.
*    char       *out_string -
*      This procedure loads pointed memory with
*      ASCII interpretation of status value
*      (null terminated string).
*      Caller must make sure an area of size
*      160 bytes is available. Some info is always
*      loaded, even if direct output is non-zero.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    'out_string'.
*REMARKS:
*  Galtis status is assumed to be the first argument.
*SEE ALSO:
 */
int
  gt_status_to_string(
    char       *status_string,
    char       *out_string
  )
{
  int
    err,
    ret ;
  char
    *gt_error ;
  unsigned
    long
      code_val ;
  ret = 0 ;
  err = gt_argument_to_val(status_string,&code_val,0,TRUE,0) ;
  if (err)
  {
    ret = err ;
    goto exit ;
  }
  switch (code_val)
  {
    case ((unsigned long)GT_ERROR):
    {
      gt_error = "General GT error" ;
      break ;
    }
    case GT_OK:
    {
      gt_error = "Operation succeeded" ;
      break ;
    }
    case GT_FAIL:
    {
      gt_error = "Operation failed" ;
      break ;
    }
    case GT_BAD_VALUE:
    {
      gt_error = "Illegal value" ;
      break ;
    }
    case GT_OUT_OF_RANGE:
    {
      gt_error = "Value is out of range" ;
      break ;
    }
    case GT_BAD_PARAM:
    {
      gt_error = "Illegal parameter in function called" ;
      break ;
    }
    case GT_BAD_PTR:
    {
      gt_error = "Illegal pointer value" ;
      break ;
    }
    case GT_BAD_SIZE:
    {
      gt_error = "Illegal size" ;
      break ;
    }
    case GT_BAD_STATE:
    {
      gt_error = "Illegal state of state machine" ;
      break ;
    }
    case GT_SET_ERROR:
    {
      gt_error = "Set operation failed" ;
      break ;
    }
    case GT_GET_ERROR:
    {
      gt_error = "Get operation failed" ;
      break ;
    }
    case GT_CREATE_ERROR:
    {
      gt_error = "Fail while creating an item" ;
      break ;
    }
    case GT_NOT_FOUND:
    {
      gt_error = "Item not found" ;
      break ;
    }
    case GT_NO_MORE:
    {
      gt_error = "No more items found" ;
      break ;
    }
    case GT_NO_SUCH:
    {
      gt_error = "No such item" ;
      break ;
    }
    case GT_TIMEOUT:
    {
      gt_error = "Time Out" ;
      break ;
    }
    case GT_NO_CHANGE:
    {
      gt_error = "The parameter is already in this value" ;
      break ;
    }
    case GT_NOT_SUPPORTED:
    {
      gt_error = "This request is not support" ;
      break ;
    }
    case GT_NOT_IMPLEMENTED:
    {
      gt_error = "This request is not implemented" ;
      break ;
    }
    case GT_NOT_INITIALIZED:
    {
      gt_error = "The item is not initialized" ;
      break ;
    }
    case GT_NO_RESOURCE:
    {
      gt_error = "Resource not available (memory ...)" ;
      break ;
    }
    case GT_FULL:
    {
      gt_error = "Item is full (Queue or table etc...)" ;
      break ;
    }
    case GT_EMPTY:
    {
      gt_error = "Item is empty (Queue or table etc...)" ;
      break ;
    }
    case GT_INIT_ERROR:
    {
      gt_error = "Error occured while INIT process" ;
      break ;
    }
    case GT_NOT_READY:
    {
      gt_error = "The other side is not ready yet" ;
      break ;
    }
    case GT_ALREADY_EXIST:
    {
      gt_error = "Tried to create existing item" ;
      break ;
    }
    case GT_OUT_OF_CPU_MEM:
    {
      gt_error = "Cpu memory allocation failed." ;
      break ;
    }
    default:
    {
      gt_error = "Unknown error code." ;
      break ;
    }
  }
  strcpy(out_string,gt_error) ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  cmd_status_to_string
*TYPE: PROC
*DATE: 20/MARCH/2003
*FUNCTION:
*  Convert command status (as returned from
*  'cmdEventRun') to ASCII string.
*CALLING SEQUENCE:
*  cmd_status_to_string(cmd_status,out_string)
*INPUT:
*  SOC_SAND_DIRECT:
*    CMD_STATUS cmd_status -
*      Status as per PSS standards.
*    char       *out_string -
*      This procedure loads pointed memory with
*      ASCII interpretation of status value
*      (null terminated string).
*      Caller must make sure an area of size
*      160 bytes is available. Some info is always
*      loaded, even if direct output is non-zero.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    'out_string'.
*REMARKS:
*  None
*SEE ALSO:
 */
int
  cmd_status_to_string(
    CMD_STATUS cmd_status,
    char       *out_string
  )
{
  int
    ret ;
  char
    *pss_error ;
  ret = 0 ;
  switch (cmd_status)
  {
    case CMD_OK:
    {
      pss_error = "CMD_OK" ;
      break ;
    }
    case CMD_AGENT_ERROR:
    {
      pss_error = "CMD_AGENT_ERROR" ;
      break ;
    }
    case CMD_SYNTAX_ERROR:
    {
      pss_error = "CMD_SYNTAX_ERROR" ;
      break ;
    }
    case CMD_ARG_UNDERFLOW:
    {
      pss_error = "CMD_ARG_UNDERFLOW" ;
      break ;
    }
    case CMD_ARG_OVERFLOW:
    {
      pss_error = "CMD_ARG_OVERFLOW" ;
      break ;
    }
    case CMD_FIELD_UNDERFLOW:
    {
      pss_error = "CMD_FIELD_UNDERFLOW" ;
      break ;
    }
    case CMD_FIELD_OVERFLOW:
    {
      pss_error = "CMD_FIELD_OVERFLOW" ;
      break ;
    }
    default:
    {
      pss_error = "UNKNOWN_STATUS (?" "?" "?)" ;
      break ;
    }
  }
  strcpy(out_string,pss_error) ;
  return (ret) ;
}
/*****************************************************
*NAME
*  gt_display_tx_message
*TYPE: PROC
*DATE: 30/MARCH/2003
*FUNCTION:
*  Convert Galtis fields from status string (as
*  returned from 'cmdEventRun', response to
*  'netGetTxNetIfTbl') to human readable display
*  of message to transmit.
*CALLING SEQUENCE:
*  gt_display_tx_message(status_string,err_msg)
*INPUT:
*  SOC_SAND_DIRECT:
*    char           *status_string -
*      Status string as per PSS standards (That is,
*      something like "@@@1!!!###" where the hex number
*      between @@@ and !!! is the first argument).
*    char           *err_msg -
*      This procedure loads pointed memory with
*      a description of the error condition encountered
*      (as reported by direct output). Only
*      meaningful if direct output is non-zero).
*      Caller must make sure pointed memory is of
*      minimal size of 160.
*    unsigned int   index -
*      Index of displayed message in array of messages
*      to transmit. Starting from 0.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    'err_msg'.
*REMARKS:
*  None
*SEE ALSO:
*  Related structure:
*    GT_PKT_DESC
*  Other related structures:
*    GT_PCKT_ENCAP, GT_COS, GT_TX_CMD, GT_TX_PARAMS
 */
int
  gt_display_tx_message(
    char           *status_string,
    char           *err_msg,
    unsigned int   index
  )
{
  int
    err,
    ret ;
  char
    *message_type,
    info[80*10] ;
  unsigned
    long
      entry_index,
      gt_tx_cmd ;
  unsigned
    long
      len,
      trunk_id,
      vlan_id,
      logical_port_is_trunk,
      logical_port_unit,
      logical_port_device,
      logical_port_port,
      exclude_port_type,
      exclude_port_unit,
      exclude_port_device,
      exclude_port_port,
      exclude_flag,
      pckt_data_len,
      gap,
      wait_time,
      pckts_num,
      tagged,
      append_crc,
      entry_id,
      user_priority,
      drop_precedence,
      traffic_class,
      logical_if_index,
      encapsulation ;
  char
    *logical_port_type_text,
    *exclude_port_type_text,
    *exclude_flag_text,
    mac_addr[13],
    *tagged_text,
    *append_crc_text,
    *encap_text,
    pckt_data[80] ;
  ret = 0 ;
  err_msg[0] = 0 ;
  /*
   * {
   * inArgs[1] - lCmdType:
   *    TX_BY_VLAN   = 0
   *    TX_BY_IF     = 1
   *    TX_BY_LPORT  = 2
   *
   * inFields[0] -  entryID
   * inFields[1] -  cos.userPriority
   * inFields[2] -  cos.dropPrecedence
   * inFields[3] -  cos.trafficClass
   * inFields[4] -  encap
   * inFields[5] -  appendCrc
   * inFields[6] -  tagged
   * inFields[7] -  pcktsNum
   * inFields[8] -  gap
   * inFields[9] -  waitTime
   * inFields[10] - pcktData
   * inFields[11] - pcktDataLen
   * inFields[12] - numSentPackets
   *   for  lCmdType == TX_BY_VLAN
   *   {
   *      inFields[13] - byVlan.mac
   *      inFields[14] - byVlan.vid
   *      inFields[15] - byVlan.excludeFlag
   *      inFields[16] - byVlan.excludePort - type (0x00: regular port , 0x01:trunk port)
   *      inFields[17] - byVlan.excludePort - unit
   *      inFields[18] - byVlan.excludePort - dev
   *      inFields[19] - byVlan.excludePort - port
   *   }
   *   for  lCmdType == TX_BY_IF
   *   {
   *      inFields[13] - byIf.ifIndex
   *   }
   *   for  lCmdType == TX_BY_LPORT
   *   {
   *      inFields[13] - byLPort.excludePort - type (0x00: regular port , 0x01:trunk port)
   *      inFields[14] - byLPort.excludePort - unit
   *      inFields[15] - byLPort.excludePort - dev
   *      inFields[16] - byLPort.excludePort - port
   *      inFields[17] - byLPort.vid
   *   }
   * }
   * Related structure: GT_PKT_DESC
   * Other related structures: GT_PCKT_ENCAP, GT_COS, GT_TX_CMD, GT_TX_PARAMS
   */
  entry_index = (unsigned long)index ;
  err = gt_argument_to_val(status_string,&gt_tx_cmd,1,TRUE,0) ;
  if (err)
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "*** Could not retrieve \'gt_tx_cmd\'\r\n"
      "    from \'gt_argument_to_val()\'.\r\n") ;
    ret = 1 ;
    goto exit ;
  }
  switch ((GT_TX_CMD)gt_tx_cmd)
  {
    case TX_BY_VLAN:
    {
      message_type = "TX_BY_VLAN" ;
      break ;
    }
    case TX_BY_IF:
    {
      message_type = "TX_BY_IF" ;
      break ;
    }
    case TX_BY_LPORT:
    {
      message_type = "TX_BY_LPORT" ;
      break ;
    }
    default:
    {
      sal_sprintf(err_msg,
        "\r\n\n"
        "*** Unknown \'gt_tx_cmd\' (%lu).\r\n",gt_tx_cmd) ;
      ret = 2 ;
      goto exit ;
    }
  }
  err = gt_field_to_val(status_string,&entry_id,0,TRUE,0) ;
  err |= gt_field_to_val(status_string,&user_priority,1,TRUE,0) ;
  err |= gt_field_to_val(status_string,&drop_precedence,2,TRUE,0) ;
  err |= gt_field_to_val(status_string,&traffic_class,3,TRUE,0) ;
  err |= gt_field_to_val(status_string,&encapsulation,4,TRUE,0) ;
  err |= gt_field_to_val(status_string,&append_crc,5,TRUE,0) ;
  err |= gt_field_to_val(status_string,&tagged,6,TRUE,0) ;
  err |= gt_field_to_val(status_string,&pckts_num,7,TRUE,0) ;
  err |= gt_field_to_val(status_string,&gap,8,TRUE,0) ;
  err |= gt_field_to_val(status_string,&wait_time,9,TRUE,0) ;
  err |= gt_field_to_val(status_string,pckt_data,10,FALSE,sizeof(pckt_data)) ;
  err |= gt_field_to_val(status_string,&pckt_data_len,11,TRUE,0) ;
  if (err)
  {
    sal_sprintf(err_msg,
      "\r\n\n"
      "*** Could not retrieve one or more of:\r\n"
      "    \'entry_id\', \'user_priority\', \'drop_precedence\', \'traffic_class\'\r\n"
      "    \'encapsulation\', \'append_crc\', \'tagged\', \'pckts_num\', \'gap\'\r\n"
      "    \'wait_time\', \'data\', \'data_len\'\r\n"
      "    from \'gt_argument_to_val()\'.\r\n") ;
    ret = 3 ;
    goto exit ;
  }
  if (append_crc)
  {
    append_crc_text = "Yes" ;
  }
  else
  {
    append_crc_text = "No" ;
  }
  if (tagged)
  {
    tagged_text = "Yes" ;
  }
  else
  {
    tagged_text = "No" ;
  }
  switch ((GT_PCKT_ENCAP)encapsulation)
  {
    case GT_REGULAR_PCKT:
    {
      encap_text = "Regular packet" ;
      break ;
    }
    case GT_CONTROL_PCKT:
    {
      encap_text = "Control packet" ;
      break ;
    }
    default:
    {
      encap_text = "Unknown packet type" ;
      break ;
    }
  }
  send_array_to_screen("\r\n",2) ;
  sal_sprintf(info,
    "Message at index %lu\r\n"
    "=======================\r\n"
    "  MESSAGE TYPE____: %s\r\n"
    "  Entry id        : %lu\r\n"
    "  IEEE802.1P\r\n"
    "   User priority  : %lu\r\n"
    "  Drop precedence : %lu\r\n"
    "  Traffic class   : %lu\r\n"
    "  Encapsulation   : %s\r\n"
    "  Append CRC      : %s\r\n"
    "  Mark packet\r\n"
    "   with 802.3P tag: %s\r\n"
    "  No. of packets\r\n"
    "   to transmit\r\n"
    "   in one group   : %lu\r\n"
    "  Time gap between\r\n"
    "   transmitted\r\n"
    "   packets        : %lu ms\r\n"
    "  Time gap between\r\n"
    "   transmitted\r\n"
    "   groups         : %lu ms\r\n"
    "  Packet data     : %s\r\n"
    "  Packet data len : %lu bytes\r\n",
    entry_index,
    message_type,
    entry_id,
    user_priority,
    drop_precedence,
    traffic_class,
    encap_text,
    append_crc_text,
    tagged_text,
    pckts_num,
    gap,
    wait_time,
    pckt_data,
    pckt_data_len
    ) ;
  send_string_to_screen(info,FALSE) ;
  switch ((GT_TX_CMD)gt_tx_cmd)
  {
    case TX_BY_VLAN:
    {
      err = gt_field_to_val(status_string,mac_addr,13,FALSE,sizeof(mac_addr)) ;
      err |= gt_field_to_val(status_string,&vlan_id,14,TRUE,0) ;
      err |= gt_field_to_val(status_string,&exclude_flag,15,TRUE,0) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "*** Could not retrieve one or more of:\r\n"
          "    \'mac_address\', \'vlan_id\', \'exclude_flag\'\r\n"
          "    from \'gt_argument_to_val()\'.\r\n") ;
        ret = 4 ;
        goto exit ;
      }
      if (exclude_flag)
      {
        exclude_flag_text = "Yes" ;
      }
      else
      {
        exclude_flag_text = "No" ;
      }
      sal_sprintf(info,
        "  Mac Address     : %s\r\n"
        "  Vlan id         : %lu\r\n"
        "  Exclude\r\n"
        "   indicated port : %s\r\n",
        mac_addr,
        vlan_id,
        exclude_flag_text
        ) ;
      if (exclude_flag)
      {
        err = gt_field_to_val(status_string,&exclude_port_type,16,TRUE,0) ;
        err |= gt_field_to_val(status_string,&exclude_port_unit,17,TRUE,0) ;
        err |= gt_field_to_val(status_string,&exclude_port_device,18,TRUE,0) ;
        err |= gt_field_to_val(status_string,&exclude_port_port,19,TRUE,0) ;
        if (err)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Could not retrieve one or more of:\r\n"
            "    \'port_type\', \'unit\', \'device\', \'port\'\r\n"
            "    from \'gt_argument_to_val()\'.\r\n") ;
          ret = 5 ;
          goto exit ;
        }
        len = strlen(info) ;
        if (exclude_port_type)
        {
          exclude_port_type_text = "Trunk" ;
          trunk_id = ((exclude_port_type & 0xFF) << 24) | (exclude_port_port & 0x00FFFFFF) ;
          sal_sprintf(&info[len],
            "  Port type       : %s\r\n"
            "  Trunk id        : 0x%08lX\r\n",
            exclude_port_type_text,
            trunk_id
            ) ;
        }
        else
        {
          exclude_port_type_text = "Regular" ;
          sal_sprintf(&info[len],
            "  Port type       : %s\r\n"
            "  Unit            : %lu\r\n"
            "  Device          : %lu\r\n"
            "  Port            : %lu\r\n",
            exclude_port_type_text,
            exclude_port_unit,
            exclude_port_device,
            exclude_port_port
            ) ;
        }
      }
      break ;
    }
    case TX_BY_IF:
    {
      err = gt_field_to_val(status_string,&logical_if_index,13,TRUE,0) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "*** Could not retrieve \'logical_if_index\'\r\n"
          "    from \'gt_argument_to_val()\'.\r\n") ;
        ret = 6 ;
        goto exit ;
      }
      sal_sprintf(info,
        "  Logical\r\n"
        "   interface index: %lu\r\n",
        logical_if_index
        ) ;
      break ;
    }
    case TX_BY_LPORT:
    {
      err = gt_field_to_val(status_string,&logical_port_is_trunk,13,TRUE,0) ;
      err |= gt_field_to_val(status_string,&logical_port_unit,14,TRUE,0) ;
      err |= gt_field_to_val(status_string,&logical_port_device,15,TRUE,0) ;
      err |= gt_field_to_val(status_string,&logical_port_port,16,TRUE,0) ;
      err |= gt_field_to_val(status_string,&vlan_id,17,TRUE,0) ;
      if (err)
      {
        sal_sprintf(err_msg,
          "\r\n\n"
          "*** Could not retrieve one or more of:\r\n"
          "    \'port_type\', \'unit\', \'device\', \'port\', \'vlan_id\'\r\n"
          "    from \'gt_argument_to_val()\'.\r\n") ;
        ret = 7 ;
        goto exit ;
      }
      if (logical_port_is_trunk)
      {
        logical_port_type_text = "Trunk" ;
        trunk_id = ((logical_port_is_trunk & 0xFF) << 24) | (logical_port_port & 0x00FFFFFF) ;
        sal_sprintf(info,
          "  Port type       : %s\r\n"
          "  Trunk id        : 0x%08lX\r\n",
          logical_port_type_text,
          trunk_id
          ) ;
      }
      else
      {
        logical_port_type_text = "Regular" ;
        sal_sprintf(info,
          "  Port type       : %s\r\n"
          "  Unit            : %lu\r\n"
          "  Device          : %lu\r\n"
          "  Port            : %lu\r\n"
          "  Vlan id         : %lu\r\n",
          logical_port_type_text,
          logical_port_unit,
          logical_port_device,
          logical_port_port,
          vlan_id
          ) ;
      }
      break ;
    }
  }
  send_string_to_screen(info,FALSE) ;
exit:
  return (ret) ;
}
/*****************************************************
*NAME
*  subject_pss
*TYPE: PROC
*DATE: 17/MARCH/2003
*FUNCTION:
*  Process input line which has a 'pss' subject.
*  Input line is assumed to have been checked and
*  found to be of right format.
*CALLING SEQUENCE:
*  subject_pss(current_line,current_line_ptr)
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
int
  subject_pss(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    err ;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004,
    ERR_005, ERR_006, ERR_007, ERR_008,
    ERR_009, ERR_010, ERR_011, ERR_012,
    ERR_013, ERR_014, ERR_015, ERR_016,
    ERR_017, ERR_018, ERR_019, ERR_020,
    ERR_021, ERR_022, ERR_023, ERR_024,
    ERR_025, ERR_026, ERR_027, ERR_028,
    ERR_029, ERR_030, ERR_031, ERR_032,
    ERR_033, ERR_034, ERR_035, ERR_036,
    ERR_037, ERR_038, ERR_039, ERR_040,
    ERR_041, ERR_042, ERR_043, ERR_044,
    ERR_045, ERR_046, ERR_047, ERR_048,
    ERR_049, ERR_050, ERR_051, ERR_052,
    ERR_053, ERR_054, ERR_055, ERR_056,
    ERR_057, ERR_058, ERR_059, ERR_060,
    ERR_061, ERR_062, ERR_063, ERR_064,
    ERR_065, ERR_066, ERR_067, ERR_068,
    ERR_069, ERR_070, ERR_071, ERR_072, ERR_073, ERR_074,
    ERR_075, ERR_076, ERR_077, ERR_078, ERR_079,
    ERR_080, ERR_081, ERR_082, ERR_083, ERR_084,
    ERR_085, ERR_086, ERR_087, ERR_088, ERR_089,
    ERR_090, ERR_091, ERR_092, ERR_093, ERR_094,
    ERR_095, ERR_096, ERR_097, ERR_098, ERR_099,
    ERR_100, ERR_101, ERR_102, ERR_103, ERR_104,
    ERR_105, ERR_106, ERR_107, ERR_108, ERR_109,
    ERR_110, ERR_111, ERR_112, ERR_113, ERR_114,
    ERR_115, ERR_116, ERR_117, ERR_118, ERR_119,
    ERR_120, ERR_121, ERR_122, ERR_123, ERR_124,
    ERR_125, ERR_126, ERR_127, ERR_128, ERR_129,
    ERR_130, ERR_131, ERR_132, ERR_133, ERR_134,
    ERR_135, ERR_136, ERR_137, ERR_138, ERR_139,
    ERR_140, ERR_141, ERR_142, ERR_143,
    NUM_EXIT_CODES
  } ret ;
  char
    last_command[80],
    msg[80*10],
    err_msg[12*80],
    *proc_name,
    *pss_cmd,
    *pss_cmd_fields,
    pss_error[80 * 2] ;
  unsigned
    int
      ui,
      get_val_of_err ;
  unsigned
    char
      *pss_cmd_output ;
  CMD_STATUS
    cmd_status ;
  unsigned
    int
      match_index;

  proc_name = "subject_pss" ;
  ret = NO_ERR ;
  get_val_of_err = FALSE ;
  send_string_to_screen("\r\n",FALSE) ;

  if ( (!is_master_device()) &&
       /*
        * buffer sender to tapi can be sent from all devices.
        */
       (search_param_val_pairs(current_line,&match_index,PARAM_PSS_TEST_SEND_BUFF_TO_TAPI_ID,1))
     )
  {
    /*
     * Enter if this is not a 'master' device.
     */
    send_string_to_screen(
      "\r\n\n"
      "*** Currently, all PSS commands are only implemented on a \'master\' unit.\r\n"
      "    This system is a \'slave\' unit.\r\n",TRUE) ;
    ret = ERR_001 ;
    goto exit;
  }

  if (current_line->num_param_names == 0)
  {
    /*
     * Enter if there are no parameters on the line (just 'pss').
     */
    send_array_to_screen("\r\n",2) ;
    err = display_dpss_system_info() ;
    if (err)
    {
      ret = ERR_002 ;
    }
    goto exit;
  }
  else
  {
    /*
     * Enter if there are parameters on the line (not just 'pss').
     */
    unsigned
      int
        device,
        unit,
        len,
        logical_port_is_trunk,
        logical_port_unit,
        logical_port_device,
        logical_port_port,
        num_devices,
        num_handled ;
    PARAM_VAL
      *param_val ;
    unsigned
      long
        code_val ;
    char
      cc,
      *s_ptr,
      *d_ptr ;
    char
      *mac_addr ;
    unsigned
      int
        vlan_id ;

    logical_port_unit = 0;
    num_handled = 0 ;
    logical_port_device = 0;
    mac_addr = NULL;
    vlan_id = 0;

#if !INCLUDE_DUNE_RPC
/* { */
    /*
     * Allow initialization only if DUNE_RPC has not done it anyway.
     */
    if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_MODULE_OPEN,1))
    {
      /*
       * call to GALTIS function - just to see linkage works.
       */
      gtInitSystem(0,1,0) ;
      userAppInit() ;
      send_string_to_screen("Marvell system initialized", TRUE) ;
    }
    else
/* } */
#endif
    if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_PORT_UTILS_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss port_utils"
       */
      unsigned long
        phy_address,
        port,
        data,
        write_mask;
      BOOL
        short_display_format,
        clear_after_read,
        do_read,
        do_mask,
        disp_all_mac_counters_cmnd,
        set_default_vlan_cmnd ;
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      disp_all_mac_counters_cmnd =
        set_default_vlan_cmnd = FALSE ;

      write_mask = 0;
      short_display_format =0;
      clear_after_read =0;
      do_mask = 0;

      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_SET_DEFAULT_VLAN_ID,1))
      {
        set_default_vlan_cmnd = TRUE ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DISP_ALL_MAC_COUNTERS_ID,1))
      {
        disp_all_mac_counters_cmnd = TRUE ;
      }
      if (set_default_vlan_cmnd || disp_all_mac_counters_cmnd)
      {
        /*
         * Enter if this is a line starting with "pss port_utils set_default_vlan"
         * or with "pss port_utils display_all_mac_counters"
         */
        if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_STANDARD_PORT_TYPE_ID,1))
        {
          logical_port_is_trunk = PORT_STANDARD_EQUIVALENT ;
        }
        else if
          (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TRUNK_PORT_TYPE_ID,1))
        {
          logical_port_is_trunk = PORT_TRUNK_EQUIVALENT ;
        }
        else
        {
          /*
           * Could find neither "standard_port" nor "trunk_port"
           * on line starting with "pss port_utils set_default_vlan" or
           * or with "pss port_utils display_all_mac_counters".
           * Probably SW error...
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could find neither \"standar_port\" nor \"trunk_port\"!\r\n") ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_118 ;
          goto exit ;
        }
        if (logical_port_is_trunk == PORT_STANDARD_EQUIVALENT)
        {
          /*
           * Standar port.
           */
          logical_port_is_trunk = 0 ;
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            logical_port_device = param_val->value.ulong_value;
          }
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            logical_port_unit = param_val->value.ulong_value;
          }
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            logical_port_port = param_val->value.ulong_value;
          }
          if (err)
          {
           /*
            * Could not find "device" or "unit" or "port"
            * on line starting with "pss port_utils set_default_vlan".
            * or with "pss port_utils display_all_mac_counters".
            * Probably SW error...
            */
            get_val_of_err = TRUE ;
            ret = ERR_128 ;
            goto exit ;
          }
#if !CODE_FOR_DEMO_ONLY
/* { */
          num_units = get_num_units() ;
          if (logical_port_unit >= num_units)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified unit no. (%u) is beyond range for current system\r\n"
              "    (%u units altogether).\r\n",
              logical_port_unit,num_units) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_123 ;
            goto exit ;
          }
          num_devices = get_num_devices_on_unit(logical_port_unit) ;
          if ((int)num_devices < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of devices on specified unit (%u).\r\n"
              "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
              logical_port_unit,(int)num_devices) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_124 ;
            goto exit ;
          }
          if (logical_port_device >= num_devices)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified device no. (%u) is beyond range for current unit\r\n"
              "    (%u devices altogether on unit %u).\r\n",
              logical_port_device,num_devices,logical_port_unit) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_125 ;
            goto exit ;
          }
          num_ports = get_num_ports_on_device_on_unit(logical_port_unit,logical_port_device) ;
          if ((int)num_ports < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of ports on specified unit,device (%u,%u).\r\n"
              "    Error code %d from \'get_num_ports_on_device_on_unit()\'.\r\n",
              logical_port_unit,logical_port_device,(int)num_ports) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_126 ;
            goto exit ;
          }
          if (logical_port_port >= num_ports)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified port no. (%u) is beyond range for current unit,device\r\n"
              "    (%u ports altogether on unit,device %u,%u).\r\n",
              logical_port_port,num_ports,logical_port_unit,logical_port_device) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_127 ;
            goto exit ;
          }
/* } */
#endif
        }
        else
        {
          /*
           * Trunk.
           */
          logical_port_is_trunk = 1 ;
          logical_port_unit = 0 ;
          logical_port_device = 0 ;
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_ID_OF_TRUNK_PORT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          logical_port_port = param_val->value.ulong_value ;
          if (err)
          {
            /*
             * Could not find "id_of_trunk_port"
             * on line starting with "pss port_utils set_default_vlan".
             * Probably SW error...
             */
            get_val_of_err = TRUE ;
            ret = ERR_119 ;
            goto exit ;
          }
        }
        if (set_default_vlan_cmnd)
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          if (!err)
          {
            vlan_id = (unsigned int)(param_val->value.ulong_value) ;
          }
          else
          {
            /*
             * Could not find "vlan_id"
             * on line starting with "pss port_utils set_default_vlan .. ".
             * Probably SW error...
             */
             get_val_of_err = TRUE ;
             ret = ERR_117 ;
             goto exit ;
          }
          {
            GT_LPORT
              logical_port ;
            GT_BOOL
              is_vlan_member ;
            logical_port =
              galtisLport(logical_port_is_trunk,logical_port_unit,
                    logical_port_device,logical_port_port) ;
            is_vlan_member = gtIsPortVlanMember(vlan_id,logical_port) ;
            if (!is_vlan_member)
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Operation failed because of ONE OR MORE of the following reasons:\r\n"
                "    1. Specified logical port (0x%08lX) is not part of this vlan\r\n"
                "    2. No such vlan has been defined\r\n",
                logical_port) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_129 ;
              goto exit ;
            }
          }
        }
        else if (disp_all_mac_counters_cmnd)
        {
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_MAC_COUNTERS_DISP_FORMAT_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            short_display_format = (unsigned int)(param_val->numeric_equivalent) ;
          }
          else
          {
            /*
             * Could not find "display_format"
             * on line starting with "pss port_utils display_all_mac_counters .. ".
             * Probably SW error...
             */
             get_val_of_err = TRUE ;
             ret = ERR_142 ;
             goto exit ;
          }
          if (short_display_format == SHORT_EQUIVALENT)
          {
            short_display_format = 1 ;
          }
          else
          {
            short_display_format = 0 ;
          }
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_CLEAR_AFTER_READ_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          if (!err)
          {
            clear_after_read = (unsigned int)(param_val->numeric_equivalent) ;
          }
          else
          {
            /*
             * Could not find "clear_after_read"
             * on line starting with "pss port_utils display_all_mac_counters .. ".
             * Probably SW error...
             */
             get_val_of_err = TRUE ;
             ret = ERR_143 ;
             goto exit ;
          }
          if (clear_after_read == OFF_EQUIVALENT)
          {
            clear_after_read = 0 ;
          }
          else
          {
            clear_after_read = 1 ;
          }
        }
        {
          if (set_default_vlan_cmnd)
          {
            sal_sprintf(err_msg,"gvlnSetPvid %u,%u,%u,%u,%u",
                  logical_port_is_trunk,logical_port_unit,
                  logical_port_device,logical_port_port,vlan_id) ;
            pss_cmd = err_msg ;
            pss_cmd_fields = "" ;
            cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
            cmd_status_to_string(cmd_status,pss_error) ;
            sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
            send_string_to_screen(err_msg,TRUE) ;
            if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
            {
              send_string_to_screen("===> Message:\r\n       ",FALSE) ;
              send_string_to_screen(pss_cmd_output,TRUE) ;
              send_string_to_screen("\r\n===> ",FALSE) ;
              err = gt_status_to_string(pss_cmd_output,err_msg) ;
              if (!err)
              {
                send_string_to_screen(err_msg,FALSE) ;
              }
              send_array_to_screen("\r\n",2) ;
              /*
               * If an error indication has been returned then status of \'gvlnSetPvid\'
               * cannot be retrieved and displayed.
               */
              err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
              if (err)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                  FALSE) ;
                ret = ERR_120 ;
                goto exit ;
              }
              if (code_val != GT_OK)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Galtis returned with error code on  request.\r\n",
                  FALSE) ;
                ret = ERR_121 ;
                goto exit ;
              }
              if (set_default_vlan_cmnd)
              {
                if (logical_port_is_trunk == 1)
                {
                  sal_sprintf(err_msg,
                    "\r\n"
                    "===> Untagged traffic on trunk port %u has been assigned,\r\n"
                    "     by default, to vlan %u.\r\n",
                      logical_port_port,vlan_id) ;
                }
                else
                {
                  sal_sprintf(err_msg,
                    "\r\n"
                    "===> Untagged traffic on unit %u, device %u, port %u has been assigned,\r\n"
                    "     by default, to vlan %u.\r\n",
                      logical_port_unit,
                      logical_port_device,logical_port_port,vlan_id) ;
                }
              }
              send_string_to_screen(err_msg,FALSE) ;
              goto exit ;
            }
            else
            {
              /*
               * Either pss_cmd_output is NULL or strlen(pss_cmd_output) is 0:
               * on response to "gvlnSetPvid ... ".
               * Probably SW error...
               */
              send_string_to_screen(
                  "\r\n\n"
                  "*** Either pss_cmd_output is NULL or strlen(pss_cmd_output) is 0.\r\n"
                  "*** This is either our SW error or Galtis SW error\r\n",
                  FALSE) ;
               ret = ERR_122 ;
               goto exit ;
            }
          }
          if (disp_all_mac_counters_cmnd)
          {
            err =
              display_all_mac_counters(
                logical_port_is_trunk,logical_port_unit,
                logical_port_device,logical_port_port,
                short_display_format,clear_after_read) ;
            if (err)
            {
              sal_sprintf(err_msg,
                    "\r\n"
                    "*** Error indication from \'disply_all_mac_counters()\': %d\r\n.",
                      err) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_141 ;
              goto exit ;
            }
          }
          else
          {
            /*
             * Could find neither "set_default_vlan_cmnd" nor "set_default_vlan_cmnd"
             * Probably SW error...
             */
            sal_sprintf(err_msg,
                "\r\n\n"
                "*** Could find neither \"disply_all_mac_counters\" nor \"set_default_vlan_cmnd\"!\r\n") ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_140 ;
            goto exit ;
          }
        }
        goto exit ;
      }
      /*
       * Reach here if this is a line NOT starting with
       * "pss port_utils set_default_vlan" or "pss port_utils display_all_mac_counters"
       */
      err =
        get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
            &param_val,VAL_NUMERIC,err_msg) ;
      if (!err)
      {
        device = param_val->value.ulong_value;
      }
      err |=
        get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
            &param_val,VAL_NUMERIC,err_msg) ;
      if (!err)
      {
        unit = param_val->value.ulong_value;
      }
      err |=
        get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
            &param_val,VAL_NUMERIC,err_msg) ;
      if (!err)
      {
        port = param_val->value.ulong_value;
      }
      if (err)
      {
       /*
        * Could not find "device" or "unit" or "port"
        * on line starting with "pss port_utils phy".
        * Probably SW error...
        */
        get_val_of_err = TRUE ;
        ret = ERR_107 ;
        goto exit ;
      }
      if (!get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_PORT_FORCE_LINK_PASS_ID,1,
            &param_val,VAL_SYMBOL,err_msg)
         )
      {
        GT_BOOL
          force_link_up;
        GT_LPORT
          logic_port;
        GT_STATUS
          gt_status;

        if (get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_PORT_FORCE_LINK_PASS_ID,1,
            &param_val,VAL_SYMBOL,err_msg)
         )
        {
         /*
          * Could not find "force_link_pass"
          * on line starting with "pss port phy write .. ".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_112 ;
          goto exit ;
        }
        force_link_up = (GT_BOOL)param_val->numeric_equivalent;

        logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);
        gt_status = gprtPortForceLinkPassEnable(
                      logic_port,
                      force_link_up
                    );
        if (gt_status != GT_OK)
        {
          send_string_to_screen(
            "\r\n\n"
            "*** ERROR: Read/Modify before writing Failed.!!\n\r",
            FALSE
          ) ;
          ret = ERR_113 ;
          goto exit ;
        }
        sal_sprintf(
          err_msg,
          "\n\rPort %lu was set.\n\r",
          port
        );
        send_string_to_screen(err_msg, TRUE);
        goto exit;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_PSS_PORT_LOOPBACK_ID,1
                )
              )
      {
        /*
         * Setting oprt in loopback.
         */

        GT_LPORT
          logic_port;
        GT_STATUS
          gt_status;

        logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);


        gt_status = psmiWritePortPhyRegister(
          logic_port,
          0, /* Control Register*/
          0x0140
        );
        send_string_to_screen(
          "--> psmiWritePortPhyRegister reg-0, data-0x4140\n\r"
          "    Loopback, 1000 Mbps, Full-Duplex, Auto-Neg-Disable\n\r",
          FALSE
        );

        gt_status = psmiWritePortPhyRegister(
          logic_port,
          0, /* Control Register*/
          0x5140
        );
        send_string_to_screen(
          "--> psmiWritePortPhyRegister reg-0, data-0x5140\n\r"
          "    Reset\n\r",
          FALSE
        );

        gt_status |= gprtPortDuplexAutoNegEnable(
                      logic_port,
                      GT_FALSE
                    );
        send_string_to_screen(
          "--> gprtPortDuplexAutoNegEnable -- GT_FALSE\n\r",
          FALSE
        );

        gt_status |= gprtPortSpeedAutoNegEnable(
                      logic_port,
                      GT_FALSE
                    );
        send_string_to_screen(
          "--> gprtPortSpeedAutoNegEnable -- GT_FALSE\n\r",
          FALSE
        );

        gt_status |= gprtSetPortDuplexMode(
                      logic_port,
                      GT_FULL_DUPLEX
                    );
        send_string_to_screen(
          "--> gprtSetPortDuplexMode -- GT_FULL_DUPLEX\n\r",
          FALSE
        );

        gt_status |= gprtSetPortSpeed(
                      logic_port,
                      GT_SPEED_1000
                    );
        send_string_to_screen(
          "--> gprtSetPortSpeed -- GT_SPEED_1000\n\r",
          FALSE
        );


        if (gt_status != GT_OK)
        {
          send_string_to_screen(
            "\r\n\n"
            "*** ERROR: loop init Failed.!!\n\r",
            FALSE
          ) ;
          ret = ERR_114 ;
          goto exit ;
        }
        sal_sprintf(
          err_msg,
          "\n\rPort %lu was set to be in loop mode.\n\r",
          port
        );
        send_string_to_screen(err_msg, TRUE);
        goto exit;
      }
      else if (!search_param_val_pairs(
                  current_line,&match_index,PARAM_PSS_PORT_STATUS_ID,1
                )
              )
      {
        /*
         * Getting Status.
         */

        GT_LPORT
          logic_port;
        GT_STATUS
          gt_status;

        logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);

        sal_sprintf(
          err_msg,
          "---------------------------------------\n\r"
          "--> Port Status (device-%u, port-%lu, unit-%u, logical port-0x%08lX)\n\r"
          "---------------------------------------",
          device,
          port,
          unit,
          logic_port
        );
        send_string_to_screen(err_msg, TRUE);

        { /* DUPLEX*/
          GT_PORT_DUPLEX
            port_duplex;
          gt_status = gprtGetPortDuplexMode(
                        logic_port,
                        &port_duplex
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  %s",
            ((port_duplex == GT_FULL_DUPLEX) ? "GT_FULL_DUPLEX" : "GT_HALF_DUPLEX")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        { /* SPEED*/
          GT_PORT_SPEED
            port_speed;

          gt_status = gprtGetPortSpeed(
                        logic_port,
                        &port_speed
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          switch (port_speed)
          {
          case GT_SPEED_10:
            sal_sprintf(err_msg, "  GT_SPEED_10");
            break;
          case GT_SPEED_100:
            sal_sprintf(err_msg, "  GT_SPEED_100");
            break;
          case GT_SPEED_1000:
            sal_sprintf(err_msg, "  GT_SPEED_1000");
            break;
          case GT_SPEED_10000:
            sal_sprintf(err_msg, "  GT_SPEED_10000");
            break;
          default:
            sal_sprintf(err_msg, "  Unknown speed");
            break;
          }
          send_string_to_screen(err_msg, TRUE);
        }
        { /* Get status of 802.3x Flow Control*/
          GT_BOOL
            flow_control;
          gt_status = gprtPortGetFlowControlEnable(
                        logic_port,
                        &flow_control
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  802.3x Flow Control %s",
            ((flow_control == GT_TRUE) ? "Enabled" : "Disabled")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        { /* Gets Link Status on a specified logical portl*/
          GT_BOOL
            is_link_up;
          gt_status = gprtGetPortLinkStatus(
                        logic_port,
                        &is_link_up
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  Link is %s",
            ((is_link_up == GT_TRUE) ? "UP" : "Down")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        { /* Gets Link Status on a specified logical portl*/
          GT_INTERFACE_MODE
            if_mode;
          gt_status = gprtGetPortInterfaceMode(
                        logic_port,
                        &if_mode
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  InterfaceMode %s",
            ((if_mode == GT_REDUCED_10BIT) ? "GT_REDUCED_10BIT" : "GT_REDUCED_GMII")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        {/* Get state of new source MAC addresses
          * learning on packets received
          * on specified port.
          */
          GT_BOOL
            learn_status;
          gt_status = gfdbGetPortLearnStatus(
                        logic_port,
                        &learn_status
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  Learn Status %s",
            ((learn_status == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
        }
        { /* Reads bits that indicate diffrent problems on specified port*/
          GT_MAC_STATUS
            mac_status;
          gt_status = gprtGetPortMacStatus(
                        logic_port,
                        &mac_status
                       );
          if (gt_status != GT_OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR in getting status!!\n\r",
              FALSE
            ) ;
            ret = ERR_115 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "  MAC status:"
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port receives pause     - %s",
            ((mac_status.isPortRxPause == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port transmit pause     - %s",
            ((mac_status.isPortTxPause == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     is in Back pressure     - %s",
            ((mac_status.isPortBackPres == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port lacking rx buffers - %s",
            ((mac_status.isPortBufFull == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port sync test failed   - %s",
            ((mac_status.isPortSyncFail == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     the port in High Error rate - %s",
            ((mac_status.isPortHiErrorRate == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port Auto Neg done      - %s",
            ((mac_status.isPortAnDone == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
          sal_sprintf(
            err_msg,
            "     port has fatal error    - %s",
            ((mac_status.isPortFatalError == GT_TRUE)? "GT_TRUE" : "GT_FALSE")
          );
          send_string_to_screen(err_msg, TRUE);
        }

        goto exit;
      }
      else if (!get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_READ_ID,1,
            &param_val,VAL_NUMERIC,err_msg)
         )
      {
        do_read = TRUE;
        phy_address = param_val->value.ulong_value;
      }
      else if (!get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_WRITE_ID,1,
            &param_val,VAL_NUMERIC,err_msg)
         )
      {
        do_read = FALSE;
        phy_address = param_val->value.ulong_value;
        if (get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_PHY_DATA_ID,1,
            &param_val,VAL_NUMERIC,err_msg)
         )
        {
         /*
          * Could not find "data"
          * on line starting with "pss port phy write .. ".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_109 ;
          goto exit ;
        }
        data = param_val->value.ulong_value;
        if (get_val_of(
            current_line,(int *)&match_index,PARAM_PSS_PHY_DATA_MASK_ID,1,
            &param_val,VAL_NUMERIC,err_msg)
         )
        {
          write_mask = 0xFFFF;
          do_mask = FALSE;
        }
        else
        {
          write_mask = param_val->value.ulong_value;
          do_mask = TRUE;
        }

      }
      else
      {
       /*
        * Could not find "read" or "write"
        * on line starting with "pss port phy".
        * Probably SW error...
        */
        get_val_of_err = TRUE ;
        ret = ERR_108 ;
        goto exit ;
      }

      /*
       * build GALTIS command.
       */
      if (do_read)
      {
        sal_sprintf(
          err_msg,
          "psmiReadPortSmiPhyRegister %u,%u,%u,%u,%u",
          0, /*0x0 regular port , 0x01:trunk port*/
          (unsigned short)unit,
          (unsigned short)device,
          (unsigned short)port,
          (unsigned short)phy_address
        ) ;
      }
      else
      {
        if (do_mask)
        {
          /*
           * Read/Modify before writing.
           */
          GT_STATUS
            gt_status;
          GT_U16
            read_data;
          gt_status = psmiReadPortSmiPhyRegister(
                        galtisLport(0, unit, device, port),
                        (GT_U16)phy_address,
                        &read_data
                      );
          if (gt_status != OK)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** ERROR: Read/Modify before writing Failed.!!\n\r",
              FALSE
            ) ;
            ret = ERR_111 ;
            goto exit ;
          }
          sal_sprintf(
            err_msg,
            "\n\rValue of register is 0x%04X, mask is 0x%04lX\n\r",
            read_data,
            write_mask
          );
          send_string_to_screen(err_msg, FALSE );
          read_data = read_data & ~write_mask;
          data = read_data | (data&write_mask);

          sal_sprintf(
            err_msg,
            "Trying to write - 0x%04lX\n\r",
            data
          );
          send_string_to_screen(err_msg, FALSE ) ;        }
        sal_sprintf(
          err_msg,
          "psmiWritePortPhyRegister %u,%u,%u,%u,%u,%u",
          0, /*0x0 regular port , 0x01:trunk port*/
          (unsigned short)unit,
          (unsigned short)device,
          (unsigned short)port,
          (unsigned short)phy_address,
          (unsigned short)data
        ) ;
      }
      pss_cmd = err_msg ;
      pss_cmd_fields = "" ;
      cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
      cmd_status_to_string(cmd_status,pss_error) ;
      sal_sprintf(err_msg,
            "\r\n"
            "===> cmdEventRun returned with: %s",pss_error) ;
      send_string_to_screen(err_msg,TRUE) ;
      if ((NULL == pss_cmd_output) || (strlen(pss_cmd_output) == 0))
      {
        send_string_to_screen(
            "\r\n\n"
            "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
            FALSE) ;
        ret = ERR_101 ;
        goto exit ;
      }
      send_string_to_screen("===> Message:\r\n       ",FALSE) ;
      send_string_to_screen(pss_cmd_output,TRUE) ;
      send_string_to_screen("\r\n===> ",FALSE) ;
      err = gt_status_to_string(pss_cmd_output,err_msg) ;
      if (!err)
      {
        send_string_to_screen(err_msg,FALSE) ;
      }
      send_array_to_screen("\r\n",2) ;

      if (do_read)
      {
        err = gt_argument_to_val(pss_cmd_output,&data,1,TRUE,0) ;
        if (err)
        {
          send_string_to_screen(
            "\r\n\n"
            "*** Could not read data values from \'gt_argument_to_val()\'.\r\n",
            FALSE) ;
          ret = ERR_110 ;
          goto exit ;
        }
        sal_sprintf(err_msg,
                  "Read value 0x%04lX.\r\n",
                  data) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Print out the values of teh fields.
         */
        switch (phy_address)
        {
        case 0:
        {
          sal_sprintf(err_msg,
                  "Control Register at %lu.\r\n"
                  "===========================\n\r",
                  phy_address) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"Reset (self clear)   - %lu",(data>>15)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Loopback             - %lu",(data>>14)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          {
            unsigned int
              tmp = ((data>>13)&0x1)|((data>>5)&0x2);
            sal_sprintf(err_msg,"Speed Selection      - %u (2-1000M, 1-100M, 0-10)", tmp);
          }
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Enable Auto-Neg Proc - %lu",(data>>12)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Power Down           - %lu",(data>>11)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Isolate              - %lu",(data>>10)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Reset Auto-Neg       - %lu",(data>>9)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Duplex Mode          - %lu",(data>>8)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Collision Test       - %lu",(data>>7)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;

          break;
        }
        case 1:
        {
          sal_sprintf(err_msg,
                  "Status Register at %lu.\r\n"
                  "===========================\n\r",
                  phy_address) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"100BASE-T4            - %lu",(data>>15)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"100BASE-X Full-duplex - %lu",(data>>14)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Auto-Neg Complete     - %lu",(data>>5)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Remote Fault          - %lu",(data>>4)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Auto-Neg Ability      - %lu",(data>>3)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Link Status           - %lu",(data>>2)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;

          break;
        }
        case 17:
        {
          sal_sprintf(err_msg,
                  "Specific Status Register at %lu.\r\n"
                  "=================================\n\r",
                  phy_address) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"speed                 - %lu (2-1000M, 1-100M, 0-10M)",(data>>14)&0x3) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Full-Duplex           - %lu",(data>>13)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Link Status; RealTime - %lu",(data>>10)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;

          break;
        }
        case 27:
        {
          sal_sprintf(err_msg,
                  "Extended PHY Specific Status at %lu.\r\n"
                  "=================================\n\r",
                  phy_address) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"Auto Fiber/Copper Select Dis   - %lu (0-ENable, 1-DISable)",(data>>15)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Select Fiber/Copper Autoneg reg- %lu (0-copper, 1-fiber)",(data>>14)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"Fiber/Copper resolution        - %lu (0-copper, 1-fiber)",(data>>13)&0x1) ;
          send_string_to_screen(err_msg,TRUE) ;
          sal_sprintf(err_msg,"MODE[3:0]                      - 0x%lX (HWCFG_MODE[3:0])",(data)&0xF) ;
          send_string_to_screen(err_msg,TRUE) ;

          break;
        }

        default:
          break;
        }
      }
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DIAGNOSTICS_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss diagnostics"
       */
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DIAGNOSTICS_TESTMEM_ID,1))
      {
         GT_PP_MEM_TYPE
           memType ;
         GT_MEM_TEST_PROFILE
           testProfile ;
         unsigned
           int
             mem_type,
             start_offset,
             test_block_size,
             test_profile ;
         char
           *mem_type_text,
           *test_profile_text ;
        /*
         * Enter if this is a line starting with "pss diagnostics test_mem"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        device = (unsigned int)(param_val->value.ulong_value) ;
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DIAGNOSTICS_MEMTYPE_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        mem_type = (unsigned int)(param_val->numeric_equivalent) ;
        mem_type_text = param_val->value.string_value ;
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DIAGNOSTICS_OFFEST_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        start_offset = (unsigned int)(param_val->value.ulong_value) ;
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DIAGNOSTICS_SIZE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        test_block_size = (unsigned int)(param_val->value.ulong_value) ;
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DIAGNOSTICS_TESTPROFILE_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        test_profile = (unsigned int)(param_val->numeric_equivalent) ;
        test_profile_text = param_val->value.string_value ;
        if (err)
        {
          /*
           * Could not find "device" or "memory_type" or "offset" or "size"
           * or "test_profile"
           * on line starting with "pss diagnostics test_mem".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_092 ;
          goto exit ;
        }
        unit = get_local_unit_num() ;
        num_devices = get_num_devices_on_unit(unit) ;
        if ((int)num_devices < 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Could not retrieve number of devices on this unit (no. %u).\r\n"
            "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
            unit,(int)num_devices) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_096 ;
          goto exit ;
        }
        if (device >= num_devices)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Specified device no. (%u) is beyond range for current unit\r\n"
            "    (%u devices altogether on this unit, no. %u).\r\n",
            device,num_devices,unit) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_097 ;
          goto exit ;
        }
        if ((start_offset & (BIT(0) | BIT(1))) || (test_block_size & (BIT(0) | BIT(1))))
        {
          /*
           * "start_offset" and "test_block_size" must be long word aligned.
           */
          send_string_to_screen(
                "\r\n\n"
                "*** \'offset\' and \'size\' MUST be multiples of 4!.\r\n",
                FALSE) ;
          ret = ERR_095 ;
          goto exit ;
        }
        /*
         * Convert memory type to internal PSS values.
         */
        switch (mem_type)
        {
          case BUFFER_DRAM_EQUIVALENT:
          {
            memType = GT_BUFFER_DRAM ;
            break ;
          }
          case WIDE_SRAM_EQUIVALENT:
          {
            memType = GT_WIDE_SRAM ;
            break ;
          }
          case NARROW_SRAM_EQUIVALENT:
          {
            memType = GT_NARROW_SRAM ;
            break ;
          }
          case FLOW_DRAM_EQUIVALENT:
          {
            memType = GT_FLOW_DRAM ;
            break ;
          }
          case MAC_TBL_MEM_EQUIVALENT:
          {
            memType = GT_MAC_TBL_MEM ;
            break ;
          }
          case VLAN_TBL_MEM_EQUIVALENT:
          {
            memType = GT_VLAN_TBL_MEM ;
            break ;
          }
          default:
          {
            /*
             * Unknown memory type. SW error.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Unknown \'memory_type\': %u. Probably SW error.\r\n",
                FALSE) ;
            ret = ERR_098 ;
            goto exit ;
            break ;
          }
        }
        /*
         * Convert memory test type to internal PSS values.
         */
        switch (test_profile)
        {
          case INCREMENTAL_EQUIVALENT:
          {
            testProfile = GT_INCREMENTAL ;
            break ;
          }
          case BIT_TOGGLE_EQUIVALENT:
          {
            testProfile = GT_BIT_TOGGLE ;
            break ;
          }
          case AA_55_EQUIVALENT:
          {
            testProfile = GT_AA_55 ;
            break ;
          }
          default:
          {
            /*
             * Unknown memory test type. SW error.
             */
            send_string_to_screen(
                "\r\n\n"
                "*** Unknown \'test_profile\': %u. Probably SW error.\r\n",
                FALSE) ;
            ret = ERR_100 ;
            goto exit ;
            break ;
          }
        }
        sal_sprintf(err_msg,
                "\r\n"
                "Going to test memory type \'%s\' on device \'%u\',\r\n"
                "starting at memory offset %lu, testing a block of %lu bytes.\r\n"
                "Required type of test: %s.\r\n\n",
                mem_type_text,(unsigned short)device,
                (unsigned long)start_offset,
                (unsigned long)test_block_size,test_profile_text
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Issue warning here since test is destructive...
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "This test is destructive. System must be reinitialized after test.!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ") ;
        err = ask_get(err_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (!err)
        {
          /*
           * Enter if user has chosen to NOT continue with test
           */
          goto exit ;
        }
        {
          /*
           * Disable traffic to prevent changes in memory under test.
           */
          sal_sprintf(err_msg,
                "\r\n"
                "1. Disable device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"sysDevEn %u,%u",
                (unsigned short)device,(unsigned short)0) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL == pss_cmd_output) || (strlen(pss_cmd_output) == 0))
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
            ret = ERR_101 ;
            goto exit ;
          }
          send_string_to_screen("===> Message:\r\n       ",FALSE) ;
          send_string_to_screen(pss_cmd_output,TRUE) ;
          send_string_to_screen("\r\n===> ",FALSE) ;
          err = gt_status_to_string(pss_cmd_output,err_msg) ;
          if (!err)
          {
            send_string_to_screen(err_msg,FALSE) ;
          }
          send_array_to_screen("\r\n",2) ;
          /*
           * If an error indication has been returned then status of \'sysDevEn\'
           * cannot be retrieved and displayed.
           */
          err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_102 ;
            goto exit ;
          }
          if (code_val != GT_OK)
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Failed to disable device using \'sysDevEn\'.",
                FALSE) ;
            ret = ERR_103 ;
            goto exit ;
          }
          sal_sprintf(err_msg,
                "\r\n"
                "2. Activate test on device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"DiagTestMem %u,%u,%u,%u,%u",
                (unsigned short)device,(unsigned short)memType,
                (unsigned short)start_offset,
                (unsigned short)test_block_size,(unsigned short)testProfile) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of \'DiagTestMem\'
             * cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_093 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              unsigned
                long
                  fault_address,
                  value_read,
                  value_written ;
              send_string_to_screen(
                "\r\n\n"
                "*** Operation failed at address: ",
                FALSE) ;
              err = gt_argument_to_val(pss_cmd_output,&fault_address,1,TRUE,0) ;
              err |= gt_argument_to_val(pss_cmd_output,&value_read,2,TRUE,0) ;
              err |= gt_argument_to_val(pss_cmd_output,&value_written,3,TRUE,0) ;
              if (err)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Could not retrieve memory address or values from \'gt_argument_to_val()\'.\r\n",
                  FALSE) ;
                ret = ERR_099 ;
                goto exit ;
              }
              sal_sprintf(err_msg,
                        "0x%08lX.\r\n"
                        "    Value written: 0x%08lX, value read: 0x%08lX\r\n\n",
                        fault_address,value_written,value_read) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
            /*
             * Ask user whether to reset system under test ???
             * ###
             */
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("-----------------", TRUE) ;
            send_string_to_screen("Consider resetting the device in hand.", TRUE) ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DIAGNOSTICS_TESTREG_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss diagnostics test_mem"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        device = (unsigned int)(param_val->value.ulong_value) ;

        if (err)
        {
          /*
           * Could not find "device" or
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_104 ;
          goto exit ;
        }
        unit = get_local_unit_num() ;
        num_devices = get_num_devices_on_unit(unit) ;
        if ((int)num_devices < 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Could not retrieve number of devices on this unit (no. %u).\r\n"
            "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
            unit,(int)num_devices) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_096 ;
          goto exit ;
        }
        if (device >= num_devices)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Specified device no. (%u) is beyond range for current unit\r\n"
            "    (%u devices altogether on this unit, no. %u).\r\n",
            device,num_devices,unit) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_097 ;
          goto exit ;
        }

        sal_sprintf(err_msg,
                "\r\n"
                "Going to test the device read/write-ability of the on-chip r/w registers\r\n"
                "\r\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Issue warning here since test is destructive...
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "This test is destructive. System must be reinitialized after test.!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ") ;
        err = ask_get(err_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (!err)
        {
          /*
           * Enter if user has chosen to NOT continue with test
           */
          goto exit ;
        }
        {
          /*
           * Disable traffic to prevent changes in memory under test.
           */
          sal_sprintf(err_msg,
                "\r\n"
                "1. Disable device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"sysDevEn %u,%u",
                (unsigned short)device,(unsigned short)0) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL == pss_cmd_output) || (strlen(pss_cmd_output) == 0))
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
            ret = ERR_101 ;
            goto exit ;
          }
          send_string_to_screen("===> Message:\r\n       ",FALSE) ;
          send_string_to_screen(pss_cmd_output,TRUE) ;
          send_string_to_screen("\r\n===> ",FALSE) ;
          err = gt_status_to_string(pss_cmd_output,err_msg) ;
          if (!err)
          {
            send_string_to_screen(err_msg,FALSE) ;
          }
          send_array_to_screen("\r\n",2) ;
          /*
           * If an error indication has been returned then status of \'sysDevEn\'
           * cannot be retrieved and displayed.
           */
          err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_102 ;
            goto exit ;
          }
          if (code_val != GT_OK)
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Failed to disable device using \'sysDevEn\'.",
                FALSE) ;
            ret = ERR_103 ;
            goto exit ;
          }
          sal_sprintf(err_msg,
                "\r\n"
                "2. Activate test on device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"DiagTestReg %u",
                (unsigned short)device) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of \'DiagTestMem\'
             * cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_093 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
                GT_U32
                  badReg,
                  readVal,
                  writeVal;

              send_string_to_screen(
                "\r\n",
                FALSE) ;
              err = gt_argument_to_val(pss_cmd_output,&badReg,   1,TRUE,0) ;
              err |= gt_argument_to_val(pss_cmd_output,&readVal, 2,TRUE,0) ;
              err |= gt_argument_to_val(pss_cmd_output,&writeVal,3,TRUE,0) ;
              if (err)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Could not retrieve memory address or values from \'gt_argument_to_val()\'.\r\n",
                  FALSE) ;
                ret = ERR_099 ;
                goto exit ;
              }
              sal_sprintf(err_msg,
                        "Address of register:      0x%08lX.\r\n"
                        "Value read from register: 0x%08lX.\r\n"
                        "Value write to register:  0x%08lX\r\n\n",
                        badReg,readVal,writeVal) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
            /*
             * Ask user whether to reset system under test ???
             * ###
             */
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("-----------------", TRUE) ;
            send_string_to_screen("Consider resetting the device in hand.", TRUE) ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DIAGNOSTICS_DUMPREG_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss diagnostics dump_reg"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        device = (unsigned int)(param_val->value.ulong_value) ;

        if (err)
        {
          /*
           * Could not find "device" or
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_104 ;
          goto exit ;
        }
        unit = get_local_unit_num() ;
        num_devices = get_num_devices_on_unit(unit) ;
        if ((int)num_devices < 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Could not retrieve number of devices on this unit (no. %u).\r\n"
            "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
            unit,(int)num_devices) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_096 ;
          goto exit ;
        }
        if (device >= num_devices)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Specified device no. (%u) is beyond range for current unit\r\n"
            "    (%u devices altogether on this unit, no. %u).\r\n",
            device,num_devices,unit) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_097 ;
          goto exit ;
        }

        sal_sprintf(err_msg,
                "\r\n"
                "Going to dump device registers value.\r\n"
                "\r\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         * Issue warning here since test is destructive...
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "This test is destructive. System must be reinitialized after test.!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ") ;
        err = ask_get(err_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (!err)
        {
          /*
           * Enter if user has chosen to NOT continue with test
           */
          goto exit ;
        }

        /*
         * Disable traffic to prevent changes in memory under test.
         */
        sal_sprintf(err_msg,
              "\r\n"
              "1. Disable device \'%u\'\r\n",
              (unsigned short)device
              ) ;
        send_string_to_screen(err_msg,FALSE) ;
        sal_sprintf(err_msg,"sysDevEn %u,%u",
              (unsigned short)device,(unsigned short)0) ;
        pss_cmd = err_msg ;
        pss_cmd_fields = "" ;
        cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
        cmd_status_to_string(cmd_status,pss_error) ;
        sal_sprintf(err_msg,
              "\r\n"
              "===> cmdEventRun returned with: %s",pss_error) ;
        send_string_to_screen(err_msg,TRUE) ;
        if ((NULL == pss_cmd_output) || (strlen(pss_cmd_output) == 0))
        {
          send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
          ret = ERR_101 ;
          goto exit ;
        }
        send_string_to_screen("===> Message:\r\n       ",FALSE) ;
        send_string_to_screen(pss_cmd_output,TRUE) ;
        send_string_to_screen("\r\n===> ",FALSE) ;
        err = gt_status_to_string(pss_cmd_output,err_msg) ;
        if (!err)
        {
          send_string_to_screen(err_msg,FALSE) ;
        }
        send_array_to_screen("\r\n",2) ;
        /*
         * If an error indication has been returned then status of \'sysDevEn\'
         * cannot be retrieved and displayed.
         */
        err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
        if (err)
        {
          send_string_to_screen(
            "\r\n\n"
            "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
            FALSE) ;
          ret = ERR_102 ;
          goto exit ;
        }
        if (code_val != GT_OK)
        {
          send_string_to_screen(
              "\r\n\n"
              "*** Failed to disable device using \'sysDevEn\'.",
              FALSE) ;
          ret = ERR_103 ;
          goto exit ;
        }
        sal_sprintf(err_msg,
              "\r\n"
              "2. Activate test on device \'%u\'\r\n",
              (unsigned short)device
              ) ;
        send_string_to_screen(err_msg,FALSE) ;
        /*
         */
        sal_sprintf(err_msg,"DiagDumpReg %u",
                (unsigned short)device) ;
        pss_cmd = err_msg ;
        pss_cmd_fields = "" ;
        cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
        cmd_status_to_string(cmd_status,pss_error) ;
        sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
        send_string_to_screen(err_msg,TRUE) ;
        goto exit ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DIAGNOSTICS_TESTMACUPDMSG_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss diagnostics test_mem"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        device = (unsigned int)(param_val->value.ulong_value) ;

        if (err)
        {
          /*
           * Could not find "device" or
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_104 ;
          goto exit ;
        }
        unit = get_local_unit_num() ;
        num_devices = get_num_devices_on_unit(unit) ;
        if ((int)num_devices < 0)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Could not retrieve number of devices on this unit (no. %u).\r\n"
            "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
            unit,(int)num_devices) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_096 ;
          goto exit ;
        }
        if (device >= num_devices)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Specified device no. (%u) is beyond range for current unit\r\n"
            "    (%u devices altogether on this unit, no. %u).\r\n",
            device,num_devices,unit) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_097 ;
          goto exit ;
        }

        sal_sprintf(err_msg,
                "\r\n"
                "Going to test the ability to NA message, send a QA message and receive QR.\r\n"
                "\r\n"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;

#if DPSS_VERION_1_25_INCLUDE /*{*/
        sal_sprintf(err_msg,
                "********************\n\r"
                "**** NOTE: This test do not support EX120B/C so the test will FAIL!!\r\n"
                "********************\n\r"
                ) ;
        send_string_to_screen(err_msg,FALSE) ;
#endif  /*}*/


        /*
         * Issue warning here since test is destructive...
         */
        sal_sprintf(err_msg,
            "\r\n\n"
            "This test is destructive. System must be reinitialized after test.!\r\n"
            "Hit \'space\' to reconfirm or any other key to\r\n"
            "cancel the operation ===> ") ;
        err = ask_get(err_msg,EXPECT_CONT_ABORT,FALSE,TRUE) ;
        if (!err)
        {
          /*
           * Enter if user has chosen to NOT continue with test
           */
          goto exit ;
        }
        {
           void *int_vec_num = NULL;
          /*
           * Disable traffic to prevent changes in memory under test.
           */
          sal_sprintf(err_msg,
                "\r\n"
                "1. Disable device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          sal_sprintf(err_msg,"sysDevEn %u,%u",
                (unsigned short)device,(unsigned short)0) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL == pss_cmd_output) || (strlen(pss_cmd_output) == 0))
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
            ret = ERR_101 ;
            goto exit ;
          }
          send_string_to_screen("===> Message:\r\n       ",FALSE) ;
          send_string_to_screen(pss_cmd_output,TRUE) ;
          send_string_to_screen("\r\n===> ",FALSE) ;
          err = gt_status_to_string(pss_cmd_output,err_msg) ;
          if (!err)
          {
            send_string_to_screen(err_msg,FALSE) ;
          }
          send_array_to_screen("\r\n",2) ;
          /*
           * If an error indication has been returned then status of \'sysDevEn\'
           * cannot be retrieved and displayed.
           */
          err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_102 ;
            goto exit ;
          }
          /*
           */
          if (code_val != GT_OK)
          {
            send_string_to_screen(
                "\r\n\n"
                "*** Failed to disable device using \'sysDevEn\'.",
                FALSE) ;
            ret = ERR_103 ;
            goto exit ;
          }
          sal_sprintf(err_msg,
                "\r\n"
                "2. Activate test on device \'%u\'\r\n",
                (unsigned short)device
                ) ;
          send_string_to_screen(err_msg,FALSE) ;
          /* DUNE_PCI_IRQ */
          extDrvGetPciIntVec(GT_PCI_INT_D,&int_vec_num);
          sal_sprintf(err_msg,"DiagTestMacUpdMsg %u,%lu",
                          (unsigned short)device,
                          (unsigned long) int_vec_num) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,
                "\r\n"
                "===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of \'DiagTestMem\'
             * cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_093 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen("\r\n",FALSE) ;
              send_string_to_screen("Test failed.\r\n",FALSE) ;
            }
            /*
             * Ask user whether to reset system under test ???
             * ###
             */
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("", TRUE) ;
            send_string_to_screen("-----------------", TRUE) ;
            send_string_to_screen("Consider resetting the device in hand.", TRUE) ;
            goto exit ;
          }
        }
      }
      else
      {
        /*
         * No parameter related to DIAGNOSTICS has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to \'diagnostics\'  has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_091 ;
        goto exit ;
      }
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_SEND_CMD_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss cmd"
       */
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      if (!get_val_of(
              current_line,(int *)&match_index, PARAM_PSS_SEND_CMD_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        pss_cmd = param_val->value.val_text ;
      }
      else
      {
        get_val_of_err = TRUE ;
        ret = ERR_003 ;
        goto exit ;
      }
      if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_CMD_FIELDS_ID,1,
              &param_val,VAL_TEXT,err_msg))
      {
        pss_cmd_fields = param_val->value.val_text ;
      }
      else
      {
        get_val_of_err = TRUE ;
        ret = ERR_004 ;
        goto exit ;
      }
      /*
       * Replace single quote by double quote as required by Galtis.
       */
      s_ptr = pss_cmd ;
      d_ptr = err_msg ;
      len = strlen(s_ptr) ;
      for (ui = 0 ; ui <= len ; ui++)
      {
        cc = *s_ptr++ ;
        if (cc == '\'')
        {
          *d_ptr++ = '\"' ;
        }
        else
        {
          *d_ptr++ = cc ;
        }
      }
      s_ptr = pss_cmd_fields ;
      d_ptr = msg ;
      len = strlen(s_ptr) ;
      for (ui = 0 ; ui <= len ; ui++)
      {
        cc = *s_ptr++ ;
        if (cc == '\'')
        {
          *d_ptr++ = '\"' ;
        }
        else
        {
          *d_ptr++ = cc ;
        }
      }
      cmd_status = cmdEventRun(err_msg,msg,&pss_cmd_output) ;
      cmd_status_to_string(cmd_status,pss_error) ;
      sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
      send_string_to_screen(err_msg,TRUE) ;
      if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
      {
        send_string_to_screen("===> Message:\r\n       ",FALSE) ;
        send_string_to_screen(pss_cmd_output,TRUE) ;
        send_string_to_screen("\r\n===> ",FALSE) ;
        err = gt_status_to_string(pss_cmd_output,err_msg) ;
        if (!err)
        {
          send_string_to_screen(err_msg,FALSE) ;
        }
      }
      send_array_to_screen("\r\n",2) ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_BRIDGE_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss bridge"
       */
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_BRIDGE_GEN_CMNDS_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss bridge general_commands"
         */
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_BRIDGE_GEN_CMNDS_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Enter with value of "pss bridge general_commands ..."
           */
          if (param_val->numeric_equivalent == SET_MAC_DA_SA_TRAP_EQUIVALENT)
          {
            char
              *dest_mac_addr,
              *src_mac_addr ;
            err =
              get_val_of(
                  current_line,(int *)&match_index,PARAM_PSS_SOURCE_MAC_ID,1,
                  &param_val,VAL_TEXT,err_msg) ;
            src_mac_addr = param_val->value.val_text ;
            err |=
              get_val_of(
                  current_line,(int *)&match_index,PARAM_PSS_DEST_MAC_ID,1,
                  &param_val,VAL_TEXT,err_msg) ;
            dest_mac_addr = param_val->value.val_text ;
            if (err)
            {
              /*
               * Could not find "source mac addr" or "destination mac addr"
               * on line starting with "pss bridge general_commands set_mac_da_sa_trap".
               * Probably SW error...
               */
              get_val_of_err = TRUE ;
              ret = ERR_005 ;
              goto exit ;
            }
            if (!is_legit_mac_address(src_mac_addr))
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Input source MAC address (%s) is not a legit. mac address\r\n"
                "    (12 hexadecimal digits)\r\n",
                src_mac_addr) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_006 ;
              goto exit ;
            }
            if (!is_legit_mac_address(dest_mac_addr))
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Input destination MAC address (%s) is not a legit. mac address\r\n"
                "    (12 hexadecimal digits)\r\n",
                dest_mac_addr) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_007 ;
              goto exit ;
            }
            {
              sal_sprintf(err_msg,"gcntSetMacDaSa \"%s\", \"%s\"",src_mac_addr,dest_mac_addr) ;
              pss_cmd = err_msg ;
              pss_cmd_fields = "" ;
              cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
              cmd_status_to_string(cmd_status,pss_error) ;
              sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
              send_string_to_screen(err_msg,TRUE) ;
              if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
              {
                send_string_to_screen("===> Message:\r\n       ",FALSE) ;
                send_string_to_screen(pss_cmd_output,TRUE) ;
                send_string_to_screen("\r\n===> ",FALSE) ;
                err = gt_status_to_string(pss_cmd_output,err_msg) ;
                if (!err)
                {
                  send_string_to_screen(err_msg,FALSE) ;
                }
                send_array_to_screen("\r\n",2) ;
                /*
                 * If an error indication has been returned then status of \'gcntSetMacDaSa\'
                 * cannot be retrieved and displayed.
                 */
                err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
                if (err)
                {
                  send_string_to_screen(
                    "\r\n\n"
                    "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                    FALSE) ;
                  ret = ERR_008 ;
                  goto exit ;
                }
                if (code_val != GT_OK)
                {
                  send_string_to_screen(
                    "\r\n\n"
                    "*** Galtis returned with error code on  request.\r\n",
                    FALSE) ;
                  ret = ERR_009 ;
                  goto exit ;
                }
              }
            }
          }
          else if (param_val->numeric_equivalent == HOST_COUNTERS_EQUIVALENT)
          {
            /*
             * Number of packets (good only) with a MAC DA matching the
             * CPU-configured MAC DA
             */
            unsigned
              long
                lGtHostInPkts ;
            /*
             * Number of octets (from good packets only) with a MAC DA matching
             * the CPU-configured MAC DA, including FCS octets
             */
            unsigned
              long
                lGtHostInOctets ;
            /*
             * Number of packets (good only) with a MAC SA matching the
             * CPU-configured MAC SA
             */
            unsigned
              long
                lGtHostOutPkts ;
            /*
             * Number of octets (from good packets only) with a MAC SA matching
             * the CPU-configured MAC SA, including FCS octets
             */
            unsigned
              long
                lGtHostOutOctets ;
            /*
             * Number of Broadcast packets (good only) with a MAC SA matching
             * the CPU-configured MAC SA
             */
            unsigned
              long
                lGtHostOutBroadcastPkts ;
            /*
             * Number of Multicast packets (good only) with a MAC SA matching
             * the CPU-configured MAC SA
             */
            unsigned
              long
                lGtHostOutMulticastPkts ;

            if (!gtIsMacDaSaValid())
            {
              send_string_to_screen(
                  "\r\n\n"
                  "==> The pair DA/SA have not been loaded yet.\r\n"
                  "    Use \'pss bridge general_commands set_mac_da_sa_trap\' first.\r\n",
                  FALSE) ;
              goto exit ;
            }
            sal_sprintf(err_msg,"brgGetHostCntrsTblFirstEntry") ;
            pss_cmd = err_msg ;
            pss_cmd_fields = "" ;
            cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
            cmd_status_to_string(cmd_status,pss_error) ;
            sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
            send_string_to_screen(err_msg,TRUE) ;
            if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
            {
              char
                da_addr[6],
                sa_addr[6];
              send_string_to_screen("===> Message:\r\n       ",FALSE) ;
              send_string_to_screen(pss_cmd_output,TRUE) ;
              send_string_to_screen("\r\n===> ",FALSE) ;
              err = gt_status_to_string(pss_cmd_output,err_msg) ;
              if (!err)
              {
                send_string_to_screen(err_msg,FALSE) ;
              }
              send_array_to_screen("\r\n",2) ;
              /*
               * If an error indication has been returned then status of
               * \'brgGetHostCntrsTblFirstEntry\' cannot be retrieved and displayed.
               */
              err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
              if (err)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                  FALSE) ;
                ret = ERR_010 ;
                goto exit ;
              }
              if (code_val != GT_OK)
              {
                send_string_to_screen(
                  "\r\n\n"
                  "*** Galtis returned with error code on \'brgGetHostCntrsTblFirstEntry\' request.\r\n",
                  FALSE) ;
                ret = ERR_011 ;
                goto exit ;
              }
              /*
               * Now get and display the siz parameters.
               */
              err = gt_field_to_val(pss_cmd_output,&lGtHostInPkts,0,TRUE,0) ;
              err |= gt_field_to_val(pss_cmd_output,&lGtHostInOctets,1,TRUE,0) ;
              err |= gt_field_to_val(pss_cmd_output,&lGtHostOutPkts,2,TRUE,0) ;
              err |= gt_field_to_val(pss_cmd_output,&lGtHostOutOctets,3,TRUE,0) ;
              err |= gt_field_to_val(pss_cmd_output,&lGtHostOutBroadcastPkts,4,TRUE,0) ;
              err |= gt_field_to_val(pss_cmd_output,&lGtHostOutMulticastPkts,5,TRUE,0) ;
              if (err)
              {
                sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Could not retrieve one or more of:\r\n"
                  "    \'lGtHostInPkts\', \'lGtHostInOctets\', \'lGtHostOutPkts\',\r\n"
                  "     \'lGtHostOutOctets\', \'encapsulation\', \'lGtHostOutBroadcastPkts\'\r\n"
                  "    from \'gt_argument_to_val()\'.\r\n") ;
                ret = ERR_012 ;
                goto exit ;
              }
              if (gtGetMacSaDa(da_addr,sa_addr))
              {
                sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Could not retrieve DA/SA pair from \'gtGetMacSaDa\'\r\n") ;
                ret = ERR_013 ;
                goto exit ;
              }

              sal_sprintf(err_msg,
                "\r\n"
                "  Host Counters for DA=0x%08lX%04X, SA=0x%08lX%04X\r\n"
                "==========================================================\r\n",
                (unsigned long)GT_HW_MAC_HIGH32((GT_ETHERADDR *)da_addr),
                (unsigned short)GT_HW_MAC_LOW16((GT_ETHERADDR *)da_addr),
                (unsigned long)GT_HW_MAC_HIGH32((GT_ETHERADDR *)sa_addr),
                (unsigned short)GT_HW_MAC_LOW16((GT_ETHERADDR *)sa_addr)
                ) ;
              send_string_to_screen(err_msg,FALSE) ;
              sal_sprintf(err_msg,
                "Number of packets (good only) with -\r\n"
                "  a MAC DA matching the CPU-configured MAC DA              : %lu\r\n"
                "  a MAC SA matching the CPU-configured MAC SA              : %lu\r\n"
                "Number of octets (from good packets only) with a MAC -\r\n"
                "  DA matching the CPU-configured MAC DA, (incl. FCS octets): %lu\r\n"
                "  SA matching the CPU-configured MAC SA, (incl. FCS octets): %lu\r\n"
                "MAC SA matching the CPU-configured MAC SA -\r\n"
                "  Number of Broadcast packets (good only)                  : %lu\r\n"
                "  Number of Multicast packets (good only)                  : %lu\r\n",
                lGtHostInPkts,
                lGtHostOutPkts,
                lGtHostInOctets,
                lGtHostOutOctets,
                lGtHostOutBroadcastPkts,
                lGtHostOutMulticastPkts
              ) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          else
          {
            /*
             * Unknown parameter value for 'general_commands'. Probably sw error...
             */
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Unknown parameter value (%u) for \'general_commands\'\r\n",
              param_val->numeric_equivalent) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_014 ;
            goto exit ;
          }
          send_array_to_screen("\r\n",2) ;
        }
        else
        {
          /*
           * Could not get parameter value of 'GENERAL_COMMANDS'
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_015 ;
          goto exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_PRINT_TABLE_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss bridge print_table"
         */
        PSS_BRIDGE_TABLE
          bridge_table;
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_PRINT_TABLE_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          /*
           * Enter with value of "pss bridge general_commands ..."
           */
          bridge_table = (PSS_BRIDGE_TABLE)param_val->numeric_equivalent;
        }
        else
        {
          /*
           * No parameter related to BRIDGE has been found on the line.
           * Probably SW error.
           */
          send_string_to_screen(
            "\r\n\n"
            "*** Parameter related to bridge table type is missing.\r\n",
            FALSE) ;
          ret = ERR_105 ;
          goto exit ;
        }
        switch (bridge_table)
        {
          case FDB_VLAN_TBL_EQUIVALENT:
          {
            /*
             * Print the table
             */
            err = pss_print_vlan_tbl();
            if (err)
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** pss_print_vlan_tbl() - return with err [%lu].\r\n",
                (unsigned long)err) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_139 ;
              goto exit ;
            }
            break;
          }
          case MAC_ADDR_TBL_EQUIVALENT:
          {
            GT_U16
              vlan_id=0;
            if (!get_val_of(
                  current_line,(int *)&match_index,PARAM_PSS_VLAN_ID,1,
                  &param_val,VAL_NUMERIC,err_msg))
            {
              vlan_id = (GT_U16)param_val->value.ulong_value;
            }
            else
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Parameter (vlan_id) related to bridge table type -mac_address_table- is missing.\r\n",
                FALSE) ;
              ret = ERR_105 ;
              goto exit ;
            }
            /*
             * Print the table
             */
            err = pss_print_mac_fdb_tbl(vlan_id);
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** pss_print_mac_fdb_tbl() - return with err.\r\n",
                FALSE) ;
              ret = ERR_105 ;
              goto exit ;
            }
            break;
          }
          default:
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Un-known bridge table type.\r\n",
              FALSE) ;
            ret = ERR_106 ;
            goto exit ;
          }
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_ADD_MAC_ENTRY_ID,1))
      {
        unsigned long
          port;
        GT_LPORT
          logic_port = 0;
        GT_MAC_ENTRY
          mac_entry;
        char
          *dest_mac_addr;
        unsigned
          int
            dest_traffic_class,
            source_traffic_class,
            nof_consecutive_macs,
            increment_unit_every_n ;
        GT_STATUS
          gt_status;

        port = 0;
        dest_mac_addr = NULL;
        dest_traffic_class = 0;
        source_traffic_class = 0;
        nof_consecutive_macs = 0;
        /*
         * Enter if this is a line starting with "pss bridge add_mac_entry"
         */
        /*
         * At this time, handle only standard port (not trunk).
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          device = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          unit = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          port = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          vlan_id = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEST_MAC_ID,1,
              &param_val,VAL_TEXT,err_msg) ;
        if (!err)
        {
          dest_mac_addr = param_val->value.val_text ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_SRC_TRAFFIC_CLASS_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          source_traffic_class = param_val->value.ulong_value ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DST_TRAFFIC_CLASS_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          dest_traffic_class = param_val->value.ulong_value ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_NOF_CONSECUTIVE_MACS_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          nof_consecutive_macs = param_val->value.ulong_value ;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_INCREMENT_DEVICE_ID_EVERY_N_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          increment_unit_every_n = param_val->value.ulong_value ;
        }

        if (err)
        {
         /*
          * Could not find "device" or "unit" or "port" or "MAC" or
          * "vlan_id" or "source_traffic_class" or "dest_traffic_class"
          * on line starting with "bridge add_mac_entry".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_107 ;
          goto exit ;
        }
#if !CODE_FOR_DEMO_ONLY
/* { */
        {
          /*
           * Make sure unit,device,port have legitimate values.
           */
          num_units = get_num_units() ;
          if (unit >= num_units)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified unit no. (%u) is beyond range for current system\r\n"
              "    (%u units altogether).\r\n",
              unit,num_units) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_130 ;
            goto exit ;
          }
          num_devices = get_num_devices_on_unit(unit) ;
          if ((int)num_devices < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of devices on specified unit (%u).\r\n"
              "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
              unit,(int)num_devices) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_131 ;
            goto exit ;
          }
          if (device >= num_devices)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified device no. (%u) is beyond range for current unit\r\n"
              "    (%u devices altogether on unit %u).\r\n",
              device,num_devices,unit) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_132 ;
            goto exit ;
          }
          num_ports = get_num_ports_on_device_on_unit(unit,device) ;
          if ((int)num_ports < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of ports on specified unit,device (%u,%u).\r\n"
              "    Error code %d from \'get_num_ports_on_device_on_unit()\'.\r\n",
              unit,device,(int)num_ports) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_133 ;
            goto exit ;
          }
          if (port >= num_ports)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified port no. (%u) is beyond range for current unit,device\r\n"
              "    (%u ports altogether on unit,device %u,%u).\r\n",
              port,num_ports,unit,device) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_134 ;
            goto exit ;
          }
        }
/* } */
#endif
        if (!is_legit_mac_address(dest_mac_addr))
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Input destination MAC address (%s) is not a legit. mac address\r\n"
            "    (12 hexadecimal digits)\r\n",
            dest_mac_addr) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_007 ;
          goto exit ;
        }
        logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);
#if !CODE_FOR_DEMO_ONLY
        {
          /*
           * Make sure vlan_id is legitimate...
           */
          GT_BOOL
            is_vlan_member ;
          is_vlan_member = gtIsPortVlanMember(vlan_id,logic_port) ;
          if (!is_vlan_member)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Operation failed because of ONE OR MORE of the following reasons:\r\n"
              "    1. Specified logical port (0x%08lX) is not part of this vlan\r\n"
              "    2. No such vlan has been defined\r\n",
              logic_port) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_135 ;
            goto exit ;
          }
        }
#endif
        galtisMacAddr(&mac_entry.macAddr, dest_mac_addr);
        mac_entry.port    = logic_port;
        mac_entry.vlanId  = vlan_id;
        mac_entry.srcTc   = (GT_U8)source_traffic_class ;
        mac_entry.dstTc   = (GT_U8)dest_traffic_class ;
        mac_entry.daCommand = GT_MAC_TABLE_FRWRD; /* NEED TO GET FROM USER*/
        mac_entry.saCommand = GT_MAC_TABLE_FRWRD; /* NEED TO GET FROM USER*/
        mac_entry.saClass = GT_FALSE; /* NEED TO GET FROM USER*/
        mac_entry.daClass = GT_FALSE; /* NEED TO GET FROM USER*/
        mac_entry.saCib   = GT_FALSE; /* NEED TO GET FROM USER*/
        mac_entry.daCib   = GT_FALSE; /* NEED TO GET FROM USER*/
        mac_entry.daRoute = GT_FALSE; /* NEED TO GET FROM USER*/
        mac_entry.multiple= FALSE;    /* NEED TO GET FROM USER*/

        if (nof_consecutive_macs < 2)
        {
          /*
           * Only one MAC
           */
          gt_status = gfdbAddMacEntry(&mac_entry,TRUE /* STATIC*/) ;
          if (gt_status != GT_OK)
          {
              sal_sprintf(
                msg,
                "\n\r"
                "gfdbAddMacEntry() failed -- returned with %u", gt_status) ;
              send_string_to_screen(msg,TRUE) ;
          }
        }
        else
        {
          unsigned int
            mac_i;
          disableDpssOsPrintf( NULL );
          for (mac_i = 0; mac_i < nof_consecutive_macs; ++mac_i )
          {
            if (increment_unit_every_n)
            {
              if ( ((mac_i % increment_unit_every_n) == 0) &&
                   (mac_i != 0)
                 )
              {
                device ++ ;
                logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);
                mac_entry.port    = logic_port;
              }
            }
            mac_entry.macAddr.arEther[4] = mac_i/0x100;
            mac_entry.macAddr.arEther[5] = mac_i%0x100;
            gt_status = gfdbAddMacEntry(&mac_entry,TRUE /* STATIC*/) ;
            if (gt_status != GT_OK)
            {
                sal_sprintf(
                  msg,
                  "\n\r"
                  "gfdbAddMacEntry() failed om the %u MAC-- returned with %u",
                  mac_i,
                  gt_status) ;
                send_string_to_screen(msg,TRUE) ;
                break;
            }
          }
          enableDpssOsPrintf();
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_DEL_MAC_ENTRY_ID,1))
      {
        GT_LPORT
          logic_port ;
        GT_ETHERADDR
          ether_addr;
        char
          *dest_mac_addr;
        GT_STATUS
          gt_status;
        /*
         * Enter if this is a line starting with "pss bridge delete_mac_entry"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          vlan_id = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEST_MAC_ID,1,
              &param_val,VAL_TEXT,err_msg) ;
        if (!err)
        {
          dest_mac_addr = param_val->value.val_text ;
        }
        if (err)
        {
         /*
          * Could not find "MAC" or "vlan_id"
          * on line starting with "bridge delete_mac_entry".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_136 ;
          goto exit ;
        }
        if (!is_legit_mac_address(dest_mac_addr))
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Input destination MAC address (%s) is not a legit. mac address\r\n"
            "    (12 hexadecimal digits)\r\n",
            dest_mac_addr) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_137 ;
          goto exit ;
        }
        galtisMacAddr(&ether_addr, dest_mac_addr) ;
        {
          /*
           * Make sure mac/vlan_id combination is legitimate...
           */
          GT_BOOL
            found ;
          GT_MAC_ENTRY_STATUS
            state ;
          gt_status =
            gfdbFindFdbMacEntry(
              (unsigned short)vlan_id,
              &ether_addr,&found,&logic_port,&state) ;
          if (gt_status != GT_OK)
          {
            if (gt_status == GT_NO_SUCH)
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** No such mac address on this VLAN\r\n"
              ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_138 ;
              goto exit ;
            }
          }
        }
        gt_status = gfdbDelMacEntry((unsigned short)vlan_id,&ether_addr) ;
        if (gt_status != GT_OK)
        {
            sal_sprintf(
              msg,
              "\n\r"
              "gfdbDelMacEntry() failed -- returned with %u", gt_status) ;
            send_string_to_screen(msg,TRUE) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_LEARN_ENABLE_ID,1))
      {
        GT_STATUS
          gt_status;
        GT_BOOL
          learn_status;
        GT_PORT_LOCK_CMD
          lock_cmd;
        unsigned long
          port;
        GT_LPORT
          logic_port = 0;

        learn_status = 0;
        port = 0;

        /*
         * Enter if this is a line starting with "pss bridge add_mac_table"
         */
        err =
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          device = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          unit = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
        if (!err)
        {
          port = param_val->value.ulong_value;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_LEARN_ENABLE_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        if (!err)
        {
          learn_status = (GT_BOOL)param_val->numeric_equivalent;
        }
        err |=
          get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_UNKNOWN_CMD_ID,1,
              &param_val,VAL_SYMBOL,err_msg) ;
        if (!err)
        {
          lock_cmd = (GT_PORT_LOCK_CMD)param_val->numeric_equivalent;
        }

        if (err)
        {
         /*
          * Could not find "device" or "unit" or "port"
          * on line starting with "pss port phy".
          * Probably SW error...
          */
          get_val_of_err = TRUE ;
          ret = ERR_116 ;
          goto exit ;
        }


        logic_port = GT_PORT_DEV_2_LPORT(device, port, unit);
#if DPSS_VERION_1_25_INCLUDE /*{*/
        gt_status = gfdbPortLearnEnable(
#else
        gt_status = gfdbSetPortLearnState(
#endif  /*}*/
                      logic_port,
                      learn_status,
                      lock_cmd
                    );
        if (gt_status != GT_OK)
        {
            sal_sprintf(
              msg,
              "\n\r"
#if DPSS_VERION_1_25_INCLUDE /*{*/
              "gfdbPortLearnEnable() failed -- returned with %u", gt_status) ;
#else
              "gfdbSetPortLearnState() failed -- returned with %u", gt_status) ;
#endif  /*}*/
            send_string_to_screen(msg,TRUE) ;
        }

      }
      else
      {
        /*
         * No parameter related to BRIDGE has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to \'bridge\'  has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_016 ;
        goto exit ;
      }
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_USER_RX_TABLE_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss user_rx_table"
       */
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_CLEAR_RX_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_rx_table clear_all"
         */
        /*
         * Clear table via Galtis.
         */
        pss_cmd = "netClearRxNetIfTable" ;
        pss_cmd_fields = "" ;
        cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output) ;
        if (cmd_status != GT_OK)
        {
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
        if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
        {
          /*
           * Enter if there was no error in communicating with DPSS. If
           * an error had been encountered, it would show as an error
           * message on one of the previous lines.
           */
          /*
           * If an error indication has been returned then 'status of message
           * on tx queue' cannot be retrieved and displayed.
           */
          err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_088 ;
            goto exit ;
          }
          if (code_val != GT_OK)
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_string_to_screen(
              "\r\n\n"
              "*** Galtis returned with error code on \'netClearRxNetIfTable\' request.\r\n",
              FALSE) ;
            ret = ERR_089 ;
            goto exit ;
          }
        }
        else
        {
          send_string_to_screen(
            "\r\n\n"
            "*** Error communicating with Galtis (\'pss_cmd_output\' is null or has no text).\r\n",
            FALSE) ;
          ret = ERR_090 ;
          goto exit ;
        }
        {
          /*
           * Display completion message
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "==> Table of received messages has been cleared!\r\n") ;
          send_string_to_screen(err_msg,FALSE) ;
        }
        send_array_to_screen("\r\n",2) ;
      }
      else if ((!search_param_val_pairs(current_line,&match_index,PARAM_PSS_GET_FIRST_RX_ID,1)) ||
            (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_GET_NEXT_RX_ID,1)))
      {
        /*
         * Enter if this is a line starting with "pss user_rx_table get_first"
         * or with "pss user_rx_table get_next"
         */
        unsigned
          long
            cpu_code,
            vlan_id,
            is_trunk,
            port ;
        long
          table_is_empty ;
        unsigned
          int
            get_first_command ;
        char
          *galtis_cmd,
          rx_data[2000];
        if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_GET_FIRST_RX_ID,1))
        {
          get_first_command = TRUE ;
          galtis_cmd = "netGetRxNetIfTblFirstEntry" ;
        }
        else
        {
          get_first_command = FALSE ;
          galtis_cmd = "netGetRxNetIfTblNextEntry" ;
        }
        {
          /*
           * Read current first message from DPSS via Galtis.
           */
          pss_cmd = galtis_cmd ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output) ;
          if (cmd_status != GT_OK)
          {
            cmd_status_to_string(cmd_status,pss_error) ;
            sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
            send_string_to_screen(err_msg,TRUE) ;
          }
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            /*
             * Enter if there was no error in communicating with DPSS. If
             * an error had been encountered, it would show as an error
             * message on one of the previous lines.
             */
            /*
             * If an error indication has been returned then 'status of message
             * on tx queue' cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_017 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen("===> Message:\r\n       ",FALSE) ;
              send_string_to_screen(pss_cmd_output,TRUE) ;
              send_string_to_screen("\r\n===> ",FALSE) ;
              err = gt_status_to_string(pss_cmd_output,err_msg) ;
              if (!err)
              {
                send_string_to_screen(err_msg,FALSE) ;
              }
              send_string_to_screen(
                "\r\n\n"
                "*** Galtis returned with error code on \'netGetRxNetIfTblFirstEntry\' request.\r\n",
                FALSE) ;
              ret = ERR_018 ;
              goto exit ;
            }
            /*
             * If there is a second argument and it is '-1' then table is empty.
             */
            err = gt_argument_to_val(pss_cmd_output,&table_is_empty,1,TRUE,0) ;
            if (err)
            {
              /*
               * Failed to read the second argument. We take that to mean there
               * is no second argument so the table is not empty.
               */
            }
            else
            {
              /*
               * There is a second argument. If it is '-1' then table is empty.
               */
              if (table_is_empty == -1)
              {
                if (get_first_command)
                {
                  send_string_to_screen(
                    "\r\n\n"
                    "==> Table of received messages is empty!.\r\n",
                    FALSE) ;
                  goto exit ;
                }
                else
                {
                  send_string_to_screen(
                    "\r\n\n"
                    "==> Either table of received messages is empty or the whole table\r\n"
                    "    has been read (full cycle). Try reading the first message:\r\n"
                    "    (\'pss user_rx_table get_first\')\r\n",
                    FALSE) ;
                  goto exit ;
                }
              }
            }
            /*
             * At this point, there should be at least one message on the table.
             * Now get 'rx_data', 'logical_port', 'vlan_id' and 'cpu_code'.
             */
            err = gt_field_to_val(pss_cmd_output,rx_data,0,FALSE,sizeof(rx_data)) ;
            if (err > 0)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve \'rx_data\' from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_019 ;
              goto exit ;
            }
            else if (err < 0)
            {
              send_string_to_screen(
                "\r\n\n"
                "==> Display of data truncated because local buffer size is too small.\r\n"
                "    Increase size of local \'rx_data\' array..\r\n",
                FALSE) ;
            }
            err = gt_field_to_val(pss_cmd_output,&is_trunk,1,TRUE,0) ;
            err |= gt_field_to_val(pss_cmd_output,&unit,2,TRUE,0) ;
            err |= gt_field_to_val(pss_cmd_output,&device,3,TRUE,0) ;
            err |= gt_field_to_val(pss_cmd_output,&port,4,TRUE,0) ;
            err |= gt_field_to_val(pss_cmd_output,&vlan_id,5,TRUE,0) ;
            err |= gt_field_to_val(pss_cmd_output,&cpu_code,6,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve \'logical_port\', \'vlan_id\' or \'cpu_code\' from\r\n"
                "    \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_021 ;
              goto exit ;
            }
          }
          else
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Error communicating with Galtis (\'pss_cmd_output\' is null or has no text).\r\n",
              FALSE) ;
            ret = ERR_022 ;
            goto exit ;
          }
          {
            /*
             * Display selected message
             */
            unsigned
              long
                rx_data_nibble_len,
                rx_data_byte_len ;
            char
              ascii_hex[3] ;
            unsigned
              char
                tmp_byte ;
            char
              *rx_data_in_ptr,
              *rx_data_out_ptr ;
            rx_data_nibble_len = strlen(rx_data) ;
            rx_data_byte_len = rx_data_nibble_len / 2 ;
            rx_data_in_ptr = rx_data_out_ptr = rx_data ;
            for (ui = 0 ; ui < rx_data_byte_len ; ui++)
            {
              ascii_hex[0] = *rx_data_in_ptr++ ;
              ascii_hex[1] = *rx_data_in_ptr++ ;
              ascii_hex[2] = 0 ;
              tmp_byte = (unsigned char)(strtoul(ascii_hex,(char **)0,16)) ;
              *rx_data_out_ptr++ = tmp_byte ;
            }
            sal_sprintf(err_msg,
                "\r\n\n"
                "Buffer size: %lu bytes\r\n"
                "Data       :\r\n"
                "============",
                rx_data_byte_len) ;
            send_string_to_screen(err_msg,FALSE) ;
            display_memory((char *)rx_data,rx_data_byte_len,CHAR_EQUIVALENT,
                              HEX_EQUIVALENT,FALSE,FALSE,0) ;
            if (!is_trunk)
            {
              sal_sprintf(err_msg,
                "Other parameters:\r\n"
                "==================\r\n"
                "Unit    : %u\r\n"
                "Device  : %u\r\n"
                "Port    : %lu\r\n"
                "Vlan id : %lu\r\n"
                "CPU code: %lu\r\n",
                unit,device,port,vlan_id,cpu_code) ;
            }
            else
            {
              /*
               * For a trunk, identifier is in 'port' and other fields
               * are just zeros.
               */
              sal_sprintf(err_msg,
                "Other parameters:\r\n"
                "==================\r\n"
                "Trunk id: %lu\r\n"
                "Vlan id : %lu\r\n"
                "CPU code: %lu\r\n",
                port,vlan_id,cpu_code) ;
            }
            send_string_to_screen(err_msg,FALSE) ;
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_GET_RX_MODE_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_rx_table get_rx_mode"
         */
        int
          buffer_mode,
          buff_size,
          num_entries ;
        char
          *buffer_mode_text ;
        /*
         * Read current values of parameters from DPSS via Galtis.
         */
        sal_sprintf(err_msg,"cmdRxGetMode") ;
        pss_cmd = err_msg ;
        pss_cmd_fields = "" ;
        cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output) ;
        if (cmd_status != GT_OK)
        {
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
        }
        if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
        {
          /*
           * Enter if there was no error in communicating with DPSS. If
           * an error had been encountered, it would show as an error
           * message on one of the previous lines.
           */
          /*
           * If an error indication has been returned then 'status of message
           * on tx queue' cannot be retrieved and displayed.
           */
          err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_023 ;
            goto exit ;
          }
          if (code_val != GT_OK)
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_string_to_screen(
              "\r\n\n"
              "*** Galtis returned with error code on \'cmdRxGetMode\' request.\r\n",
              FALSE) ;
            ret = ERR_024 ;
            goto exit ;
          }
          /*
           * Now get 'buffer_mode', 'buff_size', 'num_entries'.
           */
          err = gt_argument_to_val(pss_cmd_output,&buffer_mode,1,TRUE,0) ;
          err |= gt_argument_to_val(pss_cmd_output,&buff_size,2,TRUE,0) ;
          err |= gt_argument_to_val(pss_cmd_output,&num_entries,3,TRUE,0) ;
          if (err)
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Could not retrieve \'buffer_mode\', \'buff_size\' or \'num_entries\'\r\n"
              "    from \'gt_argument_to_val()\'.\r\n",
              FALSE) ;
            ret = ERR_025 ;
            goto exit ;
          }
        }
        else
        {
          send_string_to_screen(
            "\r\n\n"
            "*** Error communicating with Galtis (\'pss_cmd_output\' is null or has no text).\r\n",
            FALSE) ;
          ret = ERR_026 ;
          goto exit ;
        }
        {
          /*
           * Display paramers (values as stored in dpss).
           */
          if ((GT_RX_MODE)buffer_mode == GT_RX_CYCLIC)
          {
            buffer_mode_text = "cyclic" ;
          }
          else
          {
            buffer_mode_text = "fill_once_and_stop" ;
          }
          sal_sprintf(err_msg,
              "\r\n\n"
              "Currently effective receive parameters:\r\n"
              "========================================\r\n"
              "Buffers handling mode: %s\r\n"
              "Size of each buffer  : %d\r\n"
              "Number of buffers    : %d\r\n",
              buffer_mode_text,buff_size,num_entries) ;
          send_string_to_screen(err_msg,FALSE) ;
        }
        send_array_to_screen("\r\n",2) ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_SET_RX_MODE_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_rx_table set_rx_mode"
         */
        int
          buffer_mode,
          buff_size,
          num_entries ;
        char
          *buffer_mode_text ;
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_BUFFER_MODE_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          if (param_val->numeric_equivalent == GT_CYCLIC_BUFFER_EQUIVALENT)
          {
            (GT_RX_MODE)buffer_mode = GT_RX_CYCLIC ;
          }
          else
          {
            (GT_RX_MODE)buffer_mode = GT_RX_ONCE ;
          }
          buffer_mode_text = param_val->value.string_value ;
        }
        else
        {
          /*
           * Could not find "buffer_mode" on line starting
           * with "pss user_rx_table set_rx_mode".
           * Use current value...
           */
          buffer_mode = -1 ;
        }
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_BUFF_SIZE_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          buff_size = (int)param_val->value.ulong_value ;
        }
        else
        {
          /*
           * Could not find "buff_size" on line starting
           * with "pss user_rx_table set_rx_mode".
           * Use current value...
           */
           buff_size = -1 ;
        }
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_NUM_ENTRIES_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          num_entries = (int)param_val->value.ulong_value ;
        }
        else
        {
          /*
           * Could not find "num_entries" on line starting
           * with "pss user_rx_table set_rx_mode".
           * Use current value...
           */
          num_entries = -1 ;
        }
        if ((buffer_mode == -1) || (buff_size == -1) || (num_entries == -1))
        {
          /*
           * Not all parameters were specified. Read current values of missing
           * parameters from DPSS via Galtis...
           */
          int
            loc_buffer_mode,
            loc_buff_size,
            loc_num_entries ;
          sal_sprintf(err_msg,"cmdRxGetMode") ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output) ;
          if (cmd_status != GT_OK)
          {
            cmd_status_to_string(cmd_status,pss_error) ;
            sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
            send_string_to_screen(err_msg,TRUE) ;
          }
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            /*
             * Enter if there was no error in communicating with DPSS. If
             * an error had been encountered, it would show as an error
             * message on one of the previous lines.
             */
            /*
             * If an error indication has been returned then 'status of message
             * on tx queue' cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_027 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen("===> Message:\r\n       ",FALSE) ;
              send_string_to_screen(pss_cmd_output,TRUE) ;
              send_string_to_screen("\r\n===> ",FALSE) ;
              err = gt_status_to_string(pss_cmd_output,err_msg) ;
              if (!err)
              {
                send_string_to_screen(err_msg,FALSE) ;
              }
              send_array_to_screen("\r\n",2) ;
              send_string_to_screen(
                "\r\n\n"
                "*** Galtis returned with error code on \'cmdRxGetMode\' request.\r\n",
                FALSE) ;
              ret = ERR_028 ;
              goto exit ;
            }
            /*
             * Now get 'buffer_mode', 'buff_size', 'num_entries'.
             */
            err = gt_argument_to_val(pss_cmd_output,&loc_buffer_mode,1,TRUE,0) ;
            err |= gt_argument_to_val(pss_cmd_output,&loc_buff_size,2,TRUE,0) ;
            err |= gt_argument_to_val(pss_cmd_output,&loc_num_entries,3,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve message_exists flag from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_029 ;
              goto exit ;
            }
            if (buffer_mode == -1)
            {
              buffer_mode = loc_buffer_mode ;
            }
            if (buff_size == -1)
            {
              buff_size = loc_buff_size ;
            }
            if (num_entries == -1)
            {
              num_entries = loc_num_entries ;
            }
          }
          else
          {
            send_string_to_screen(
              "\r\n\n"
              "*** Error communicating with Galtis (\'pss_cmd_output\' is null or has no text).\r\n",
              FALSE) ;
            ret = ERR_030 ;
            goto exit ;
          }
        }
        {
          sal_sprintf(err_msg,"cmdRxSetMode %d,%d,%d",
                    buffer_mode,
                    buff_size,num_entries) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of \'cmdRxSetMode\'
             * cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_031 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Galtis returned with error code on \'cmdTxSetMode\' request.\r\n",
                FALSE) ;
              ret = ERR_032 ;
              goto exit ;
            }
            {
              /*
               * Display selected paramers (since some may be values as stored in dpss).
               */
              if ((GT_RX_MODE)buffer_mode == GT_RX_CYCLIC)
              {
                buffer_mode_text = "cyclic" ;
              }
              else
              {
                buffer_mode_text = "fill_once_and_stop" ;
              }
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "Selected receive parameters:\r\n"
                  "=============================\r\n"
                  "Buffers handling mode: %s\r\n"
                  "size of each buffer  : %d\r\n"
                  "Number of buffers    : %d\r\n"
                  "\r\n"
                  "The whole table has now been cleared!!!\r\n",
                  buffer_mode_text,buff_size,num_entries) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_RX_ACTION_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_rx_table rx_action"
         */
        unsigned
          int
            start_rx ;
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_RX_ACTION_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          start_rx = (unsigned int)param_val->numeric_equivalent ;
          if (start_rx == GT_START_RX_EQUIVALENT)
          {
            /*
             * Command is: start rx capture
             */
            start_rx = TRUE ;
            pss_cmd = "cmdRxStartCapture" ;
          }
          else
          {
            /*
             * Command is: stop rx
             */
            start_rx = FALSE ;
            pss_cmd = "cmdRxStopCapture" ;
          }
        }
        else
        {
          /*
           * Could not find parameter value of "rx_action" on line starting
           * with "pss user_rx_table rx_action".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_033 ;
          goto exit ;
        }
        {
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of
             * \'cmdRxStartCapture\' or \'cmdRxStopCapture\' cannot be
             * retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_034 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Galtis returned with error code on %s request.\r\n",
                pss_cmd) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_035 ;
              goto exit ;
            }
            {
              /*
               * Display selected action.
               */
              if (start_rx)
              {
                send_string_to_screen("\r\n===> Receive capture enabled\r\n",FALSE) ;
              }
              else
              {
                send_string_to_screen("\r\n===> Receive capture disabled\r\n",FALSE) ;
              }
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else
      {
        /*
         * No parameter related to USER_RX_TABLE has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to USER_RX_TABLE  has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_036 ;
        goto exit ;
      }
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_USER_TX_TABLE_ID,1))
    {
      /*
       * Enter if this is a line starting with "pss user_tx_table"
       */
      unsigned
        int
          len,
          message_exists,
          index ;
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_SET_TX_MODE_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_tx_table set_tx_mode"
         */
        unsigned
          int
            num_cycles,
            num_packets,
            gap_between_cycles_ms ;
        char
          *num_cycles_text,
          *num_packets_text ;
        /*
         * Precaution.
         */
        num_cycles = -1 ;
        num_packets = -1 ;
        gap_between_cycles_ms = 1 ;
        num_cycles_text = NULL;
        num_packets_text = NULL;

        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_NUM_TX_CYCLES_ID,1,
              &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg))
        {
          if (param_val->val_type & VAL_SYMBOL)
          {
            if (param_val->numeric_equivalent == GT_INFINITE_CYCLES_EQUIVALENT)
            {
              num_cycles = -1 ;
              num_cycles_text = param_val->value.string_value ;
            }
          }
          else if (param_val->val_type & VAL_NUMERIC)
          {
            num_cycles = (unsigned int)param_val->value.ulong_value ;
          }
        }
        else
        {
          /*
           * Could not find "num_loop_cycles" on line starting
           * with "pss user_tx_table set_tx_mode".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_037 ;
          goto exit ;
        }
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_TOT_NUM_PACKETS_ID,1,
              &param_val,VAL_NUMERIC | VAL_SYMBOL,err_msg))
        {
          if (param_val->val_type & VAL_SYMBOL)
          {
            if (param_val->numeric_equivalent == GT_AS_SPECIFIED_PER_ENTRY_EQUIVALENT)
            {
              num_packets = -1 ;
              num_packets_text = param_val->value.string_value ;
            }
          }
          else if (param_val->val_type & VAL_NUMERIC)
          {
            num_packets = (unsigned int)param_val->value.ulong_value ;
          }
        }
        else
        {
          /*
           * Could not find "tot_num_packets" on line starting
           * with "pss user_tx_table set_tx_mode".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_038 ;
          goto exit ;
        }
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_GAP_BETWEEN_CYCLES_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          gap_between_cycles_ms = (unsigned int)param_val->value.ulong_value ;
        }
        else
        {
          /*
           * Could not find "gap_between_cycles_ms" on line starting
           * with "pss user_tx_table set_tx_mode".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_039 ;
          goto exit ;
        }
        {
          sal_sprintf(err_msg,"cmdTxSetMode 0x%lX,0x%lX,%d",
                    (unsigned long)num_cycles,
                    (unsigned long)num_packets,gap_between_cycles_ms) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status of \'cmdTxSetMode\'
             * cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_040 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Galtis returned with error code on \'cmdTxSetMode\' request.\r\n",
                FALSE) ;
              ret = ERR_041 ;
              goto exit ;
            }
            {
              /*
               * Display selected paramers (since some may be default values).
               */
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "Selected transmit parameters:\r\n"
                  "=============================\r\n"
                  "Number of cycles  : ") ;
              len = strlen(err_msg) ;
              if (num_cycles == (unsigned int)(-1))
              {
                sal_sprintf(&err_msg[len],"%s\r\n",num_cycles_text) ;
              }
              else
              {
                sal_sprintf(&err_msg[len],"%u\r\n",num_cycles) ;
              }
              len += strlen(&err_msg[len]) ;
              if (num_packets == (unsigned int)(-1))
              {
                sal_sprintf(&err_msg[len],"Number of packets : %s\r\n",num_packets_text) ;
              }
              else
              {
                sal_sprintf(&err_msg[len],"Number of packets : %u\r\n",num_packets) ;
              }
              len += strlen(&err_msg[len]) ;
              sal_sprintf(&err_msg[len],"Gap between cycles: %u ms\r\n",gap_between_cycles_ms) ;
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_ACTION_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_tx_table tx_action"
         */
        unsigned
          int
            start_tx ;
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_TX_ACTION_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          start_tx = (unsigned int)param_val->numeric_equivalent ;
          if (start_tx == GT_START_TX_EQUIVALENT)
          {
            /*
             * Command is: start tx
             */
            start_tx = TRUE ;
            pss_cmd = "cmdTxStart" ;
          }
          else
          {
            /*
             * Command is: stop tx
             */
            start_tx = FALSE ;
            pss_cmd = "cmdTxStop" ;
          }
        }
        else
        {
          /*
           * Could not find parameter value of "tx_action" on line starting
           * with "pss user_tx_table tx_action".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_042 ;
          goto exit ;
        }
        {
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then status
             * of \'cmdTxStart\' or \'cmdTxStop\' cannot be retrieved
             * and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_043 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Galtis returned with error code on %s request.\r\n",
                pss_cmd) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_044 ;
              goto exit ;
            }
            {
              /*
               * Display selected action.
               */
              if (start_tx)
              {
                send_string_to_screen("\r\n===> Transmission started\r\n",FALSE) ;
              }
              else
              {
                send_string_to_screen("\r\n===> Transmission stopped\r\n",FALSE) ;
              }
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_GET_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_tx_table get"
         */
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_INDEX_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          index = (unsigned int)param_val->value.ulong_value ;
          sal_sprintf(err_msg,"netGetTxNetIfTbl %u,%u",index,0) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then 'status of message
             * on tx queue' cannot be retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_045 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** 1. Galtis returned with error code on \'netGetTxNetIfTbl\' request.\r\n",
                FALSE) ;
              ret = ERR_046 ;
              goto exit ;
            }
            /*
             * Now get 'status of message on tx queue'.
             */
            err = gt_argument_to_val(pss_cmd_output,&message_exists,1,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** 2. Could not retrieve message_exists flag from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_047 ;
              goto exit ;
            }
            /*
             * If 'message_exists' is '-1' then either table is
             * empty or there is no such entry (currently) on the table.
             */
            if (message_exists == (unsigned int)(-1))
            {
              sal_sprintf(err_msg,
                  "\r\n"
                  "===> Either tx queue is empty or there is no message at this index (%u)\r\n",
                  index) ;
              send_string_to_screen(
                err_msg,
                FALSE) ;
            }
            else
            {
              /*
               * Message has been found. Display it.
               */
              err =
                gt_display_tx_message(pss_cmd_output,err_msg,index) ;
              if (err)
              {
                len = strlen(err_msg) ;
                sal_sprintf(&err_msg[len],
                  "\r\n\n"
                  "*** Error code %d from \'gt_display_tx_message()\'.\r\n",
                  err) ;
                send_string_to_screen(err_msg,FALSE) ;
                ret = ERR_048 ;
                goto exit ;
              }
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
        else
        {
          /*
           * Could not find "index" on line starting with "pss user_tx_table get".
           * Probably SW error...
           */
          get_val_of_err = TRUE ;
          ret = ERR_049 ;
          goto exit ;
        }
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_ADD_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss user_tx_table add"
         * Must have either one of: "tx_by_vlan", "tx_by_if", "tx_by_lport"
         */
        unsigned
          int
            num_data_to_tx,
            exclude_port_type,
            exclude_port_unit,
            exclude_port_device,
            exclude_port_port,
            exclude_flag,
            pckt_data_len,
            gap,
            wait_time,
            pckts_num,
            tagged,
            append_crc,
            entry_id,
            user_priority,
            drop_precedence,
            traffic_class,
            logical_if_index,
            encapsulation ;
        GT_TX_CMD
          gt_tx_cmd ;
        unsigned
          long
            data_to_tx ;
        char
          pckt_data[(PSS_DATA_NUM_VALS * 2 * sizeof(unsigned long)) + 1] ;

        gt_tx_cmd = 0;


        if (
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_VLAN_ID,1)) ||
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_IF_ID,1))   ||
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_LPORT_ID,1))
            )
        {
          /*
           * Enter if this is a line starting with "pss user_tx_table add tx_by_vlan"
           */
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_ENTRY_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          entry_id = (unsigned int)param_val->value.ulong_value ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_USER_PRIORITY_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          user_priority = (unsigned int)param_val->value.ulong_value ;
          err |=get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_DROP_PRECEDENCE_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          drop_precedence = (unsigned int)param_val->value.ulong_value ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_TRAFFIC_CLASS_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          traffic_class = (unsigned int)param_val->value.ulong_value ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_ENCAPSULATION_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          encapsulation = (unsigned int)param_val->numeric_equivalent ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_APPEND_CRC_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          append_crc = (unsigned int)param_val->numeric_equivalent ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_TAGGED_ID,1,
                &param_val,VAL_SYMBOL,err_msg) ;
          tagged = (unsigned int)param_val->numeric_equivalent ;
          err |=
            get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_PCKTS_NUM_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
          pckts_num = param_val->value.ulong_value ;
          err |=
            get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_PCKT_DATA_LEN_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
          pckt_data_len = param_val->value.ulong_value ;
          err |=
            get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_GAP_BET_PCKTS_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
          gap = param_val->value.ulong_value ;
          err |=
            get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_WAIT_BET_GROUPS_ID,1,
              &param_val,VAL_NUMERIC,err_msg) ;
          wait_time = param_val->value.ulong_value ;
          if (err)
          {
            /*
             * Could not find "entry_id" or "user_priority" or "drop_precedence"
             * or "traffic_class" or "encapsulation"
             * or "append_crc" or "tagged" or "pckts_num" or "data_len"
             * on line starting with "pss user_tx_table add ...".
             * Probably SW error...
             */
            get_val_of_err = TRUE ;
            ret = ERR_050 ;
            goto exit ;
          }
          {
            /*
             * Get data to transmit. There must be at least one data item.
             */
            num_data_to_tx = PSS_DATA_NUM_VALS ;
            len = 0 ;
            /*
             * The following 'for' MUST make at least one loop!
             */
            for (ui = 1 ; ui <= PSS_DATA_NUM_VALS ; ui++)
            {
              if (get_val_of(
                    current_line,(int *)&match_index,PARAM_PSS_PCKT_DATA_ID,ui,
                    &param_val,VAL_NUMERIC,err_msg))
              {
                /*
                 * If there are no more data values then quit loop.
                 */
                num_data_to_tx = ui - 1 ;
                break ;
              }
              data_to_tx = param_val->value.ulong_value ;
              /*
               * Convert to ASCII hex digits...
               */
              sal_sprintf(&pckt_data[len],"%08lX",data_to_tx) ;
              len += strlen(&pckt_data[len]) ;
            }
            if (num_data_to_tx == 0)
            {
              /*
               * Not even one value of write data could not be retrieved.
               */
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Not even one value of data to transmit could not be retrieved!\r\n"
                  ) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_051 ;
              goto exit ;
            }
            /*
             * Just precaution...
             */
            if (num_data_to_tx > PSS_DATA_NUM_VALS)
            {
              /*
               * Too many write data values have been retrieved.
               */
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Too many values of data to tx (%lu) have been retrieved!\r\n",
                  (unsigned long)num_data_to_tx) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_052 ;
              goto exit ;
            }
          }
          /*
           * Convert encapsulation to GT convention.
           */
          if (encapsulation == GT_REGULAR_ENCAPSULATION_EQUIVALENT)
          {
            (GT_PCKT_ENCAP)encapsulation = GT_REGULAR_PCKT ;
          }
          else
          {
            (GT_PCKT_ENCAP)encapsulation = GT_CONTROL_PCKT ;
          }
          if (append_crc == ON_EQUIVALENT)
          {
            append_crc = 1 ;
          }
          else
          {
            append_crc = 0 ;
          }
          if (tagged == ON_EQUIVALENT)
          {
            tagged = 1 ;
          }
          else
          {
            tagged = 0 ;
          }
          if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_VLAN_ID,1))
          {
            /*
             * Enter if this is a line starting with "pss user_tx_table add tx_by_vlan"
             */
            /*
             * Handle parameters specific to line with "tx_by_vlan".
             */
            err =
              get_val_of(
                  current_line,(int *)&match_index,PARAM_PSS_MAC_ADDR_ID_ID,1,
                  &param_val,VAL_TEXT,err_msg) ;
            mac_addr = param_val->value.val_text ;
            err |=
              get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
            vlan_id = param_val->value.ulong_value ;
            if (err)
            {
              /*
               * Could not find "vlan_id" or "mac_addr"
               * on line starting with "pss user_tx_table add tx_by_vlan".
               * Probably SW error...
               */
              get_val_of_err = TRUE ;
              ret = ERR_053 ;
              goto exit ;
            }
            if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_EXCLUDE_ONE_PORT_ID,1))
            {
              exclude_flag = ON_EQUIVALENT ;
            }
            else if
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_EXCLUDE_NO_PORT_ID,1))
            {
              exclude_flag = OFF_EQUIVALENT ;
            }
            else
            {
              /*
               * Could find neither "exclude_no_port" nor "exclude_one_port"
               * on line starting with "pss user_tx_table add tx_by_vlan".
               * Probably SW error...
               */
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Could find neither \"exclude_no_port\" nor \"exclude_one_port\"!\r\n") ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_054 ;
              goto exit ;
            }
            if (exclude_flag == ON_EQUIVALENT)
            {
              exclude_flag = 1 ;
              if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_STANDARD_PORT_TYPE_ID,1))
              {
                exclude_port_type = PORT_STANDARD_EQUIVALENT ;
              }
              else if
                (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TRUNK_PORT_TYPE_ID,1))
              {
                exclude_port_type = PORT_TRUNK_EQUIVALENT ;
              }
              else
              {
                /*
                 * Could find neither "standard_port" nor "trunk_port"
                 * on line starting with "pss user_tx_table ... exclude_one_port ".
                 * Probably SW error...
                 */
                sal_sprintf(err_msg,
                    "\r\n\n"
                    "*** Could find neither \"exclude_no_port\" nor \"exclude_one_port\"!\r\n") ;
                send_string_to_screen(err_msg,FALSE) ;
                ret = ERR_055 ;
                goto exit ;
              }
              if (exclude_port_type == PORT_STANDARD_EQUIVALENT)
              {
                exclude_port_type = 0 ;
                err =
                  get_val_of(
                      current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg) ;
                exclude_port_unit = param_val->value.ulong_value ;
                err |=
                  get_val_of(
                      current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
                      &param_val,VAL_NUMERIC,err_msg) ;
                exclude_port_device = param_val->value.ulong_value ;
                err |=
                  get_val_of(
                      current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg) ;
                exclude_port_port = param_val->value.ulong_value ;
                if (err)
                {
                  /*
                   * Could not find "exclude_port_type" or "exclude_port_unit"
                   * or "exclude_port_device" or "exclude_port_port"
                   * on line starting with "pss user_tx_table add tx_by_vlan".
                   * Probably SW error...
                   */
                  get_val_of_err = TRUE ;
                  ret = ERR_056 ;
                  goto exit ;
                }
              }
              else
              {
                /*
                 * Trunk.
                 */
                exclude_port_type = 1 ;
                exclude_port_unit = 0 ;
                exclude_port_device = 0 ;
                err =
                  get_val_of(
                      current_line,(int *)&match_index,PARAM_PSS_ID_OF_TRUNK_PORT_ID,1,
                      &param_val,VAL_NUMERIC,err_msg) ;
                exclude_port_port = param_val->value.ulong_value ;
                if (err)
                {
                  /*
                   * Could not find "exclude_port_port"
                   * on line starting with "pss user_tx_table ... exclude_one_port".
                   * Probably SW error...
                   */
                  get_val_of_err = TRUE ;
                  ret = ERR_057 ;
                  goto exit ;
                }
              }
            }
            else
            {
              exclude_flag = 0 ;
              exclude_port_type = 0 ;
              exclude_port_unit = 0 ;
              exclude_port_device = 0 ;
              exclude_port_port = 0 ;
            }
            if (!is_legit_mac_address(mac_addr))
            {
              sal_sprintf(err_msg,
                "\r\n\n"
                "*** Input MAC address field (%s) is not a legitimate mac address\r\n"
                "    (12 hexadecimal digits)\r\n",
                mac_addr) ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_058 ;
              goto exit ;
            }
            sal_sprintf(
              msg,
              "%u %u %u %u %u %u %u %u %u %u "
              "\"%s\" %u %u \"%s\" %u %u %u %u %u %u",
              entry_id,user_priority,drop_precedence,
              traffic_class,encapsulation,append_crc,
              tagged,pckts_num,gap,
              wait_time,
              pckt_data,pckt_data_len,0,
              mac_addr,vlan_id,exclude_flag,
              exclude_port_type,exclude_port_unit,exclude_port_device,
              exclude_port_port
              ) ;
            gt_tx_cmd = TX_BY_VLAN ;
          }
          else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_IF_ID,1))
          {
            /*
             * Enter if this is a line starting with "pss user_tx_table add tx_by_if"
             */
            /*
             * Handle parameters specific to line with "tx_by_if".
             */
            err =
              get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_LOGICAL_IF_INDEX_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
            logical_if_index = param_val->value.ulong_value ;
            if (err)
            {
              /*
               * Could not find "logical_if_index"
               * on line starting with "pss user_tx_table add tx_by_vlan".
               * Probably SW error...
               */
              get_val_of_err = TRUE ;
              ret = ERR_059 ;
              goto exit ;
            }
            sal_sprintf(
              msg,
              "%u %u %u %u %u %u %u %u %u %u "
              "\"%s\" %u %u %u",
              entry_id,user_priority,drop_precedence,
              traffic_class,encapsulation,append_crc,
              tagged,pckts_num,gap,
              wait_time,
              pckt_data,pckt_data_len,0,
              logical_if_index
              ) ;
            gt_tx_cmd = TX_BY_IF ;
          }
          else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TX_BY_LPORT_ID,1))
          {
            /*
             * Enter if this is a line starting with "pss user_tx_table add tx_by_lport"
             */
            /*
             * Handle parameters specific to line with "tx_by_lport".
             */
            if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_STANDARD_PORT_TYPE_ID,1))
            {
              logical_port_is_trunk = PORT_STANDARD_EQUIVALENT ;
            }
            else if
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TRUNK_PORT_TYPE_ID,1))
            {
              logical_port_is_trunk = PORT_TRUNK_EQUIVALENT ;
            }
            else
            {
              /*
               * Could find neither "standard_port" nor "trunk_port"
               * on line starting with "pss user_tx_table add tx_by_lport".
               * Probably SW error...
               */
              sal_sprintf(err_msg,
                  "\r\n\n"
                  "*** Could find neither \"standar_port\" nor \"trunk_port\"!\r\n") ;
              send_string_to_screen(err_msg,FALSE) ;
              ret = ERR_060 ;
              goto exit ;
            }
            err =
              get_val_of(
                  current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
                  &param_val,VAL_NUMERIC,err_msg) ;
            vlan_id = param_val->value.ulong_value ;
            if (err)
            {
              /*
               * Could not find "vlan id"
               * on line starting with "pss user_tx_table add tx_by_lport".
               * Probably SW error...
               */
              get_val_of_err = TRUE ;
              ret = ERR_061 ;
              goto exit ;
            }
            if (logical_port_is_trunk == PORT_STANDARD_EQUIVALENT)
            {
              logical_port_is_trunk = 0 ;
              err =
                get_val_of(
                    current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
                    &param_val,VAL_NUMERIC,err_msg) ;
              logical_port_unit = param_val->value.ulong_value ;
              err |=
                get_val_of(
                    current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
                    &param_val,VAL_NUMERIC,err_msg) ;
              logical_port_device = param_val->value.ulong_value ;
              err |=
                get_val_of(
                    current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
                    &param_val,VAL_NUMERIC,err_msg) ;
              logical_port_port = param_val->value.ulong_value ;
              if (err)
              {
                /*
                 * Could not find "logical_port_unit"
                 * or "logical_port_device" or "logical_port_port"
                 * on line starting with "pss user_tx_table add tx_by_lport".
                 * Probably SW error...
                 */
                get_val_of_err = TRUE ;
                ret = ERR_062 ;
                goto exit ;
              }
            }
            else
            {
              /*
               * Trunk.
               */
              logical_port_is_trunk = 1 ;
              logical_port_unit = 0 ;
              logical_port_device = 0 ;
              err =
                get_val_of(
                    current_line,(int *)&match_index,PARAM_PSS_ID_OF_TRUNK_PORT_ID,1,
                    &param_val,VAL_NUMERIC,err_msg) ;
              logical_port_port = param_val->value.ulong_value ;
              if (err)
              {
                /*
                 * Could not find "id_of_trunk_port"
                 * on line starting with "pss user_tx_table add tx_by_lport".
                 * Probably SW error...
                 */
                get_val_of_err = TRUE ;
                ret = ERR_063 ;
                goto exit ;
              }
            }
            sal_sprintf(
              msg,
              "%u %u %u %u %u %u %u %u %u %u "
              "\"%s\" %u %u %u %u %u %u %u",
              entry_id,user_priority,drop_precedence,
              traffic_class,encapsulation,append_crc,
              tagged,pckts_num,gap,
              wait_time,
              pckt_data,pckt_data_len,0,
              logical_port_is_trunk,logical_port_unit,logical_port_device,
              logical_port_port,vlan_id
              ) ;
            gt_tx_cmd = TX_BY_LPORT ;
          }
          {
            sal_sprintf(err_msg,"netSetTxNetIfTblEntry %u,%u",0,(unsigned short)gt_tx_cmd) ;
            pss_cmd = err_msg ;
            pss_cmd_fields = msg ;
            cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
            cmd_status_to_string(cmd_status,pss_error) ;
            sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
            send_string_to_screen(err_msg,TRUE) ;
            if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
            {
              send_string_to_screen("===> Message:\r\n       ",FALSE) ;
              send_string_to_screen(pss_cmd_output,TRUE) ;
              send_string_to_screen("\r\n===> ",FALSE) ;
              err = gt_status_to_string(pss_cmd_output,err_msg) ;
              if (!err)
              {
                send_string_to_screen(err_msg,FALSE) ;
              }
            }
            send_array_to_screen("\r\n",2) ;
          }
        }
        else
        {
          /*
           * No parameter related to PSS USER_TX_TABLE ADD has been found on the line.
           * Probably SW error.
           */
          send_string_to_screen(
            "\r\n\n"
            "*** No parameter related to \'USER_TX_TABLE ADD\' has been found on the line.\r\n",
            FALSE) ;
          ret = ERR_064 ;
          goto exit ;
        }
      }
      else
      {
        /*
         * No parameter related to USER_TX_TABLE has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to USER_TX_TABLE  has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_065 ;
        goto exit ;
      }
      goto exit ;
    }
    else if (
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_VLAN_ID,1)) ||
              (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_MULTICAST_ID,1))
            )
    {
      /*
       * Enter if this is a line starting with "pss vlan" or with "pss multicast"
       */
      unsigned
        int
          vlan_command,
          mac_multicast_command,
          learning_mode ;
      num_handled++ ;
      pss_cmd =
        pss_cmd_fields = NULL ;
      pss_cmd_output = NULL ;
      vlan_command =
        mac_multicast_command = FALSE ;
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_VLAN_ID,1))
      {
        /*
         * This is a vlan command.
         */
        vlan_command = TRUE ;
      }
      else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_MULTICAST_ID,1))
      {
        /*
         * This is not a vlan command, it is a multicast command.
         */
        mac_multicast_command = TRUE ;
      }
      if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_VLAN_SELECT_LEARNING_MODE_ID,1))
      {
        /*
         * Enter if this is a line starting with "pss vlan select_learning_mode"
         */
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_VLAN_SELECT_LEARNING_MODE_ID,1,
              &param_val,VAL_SYMBOL,err_msg))
        {
          learning_mode = (unsigned int)param_val->numeric_equivalent ;
          if (learning_mode == VLAN_INDIVIDUAL_EQUIVALENT)
          {
            learning_mode = 1 ;
          }
          else
          {
            learning_mode = 0 ;
          }
          sal_sprintf(err_msg,"gfdbSetMacVlMode %u",learning_mode) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
          }
          send_array_to_screen("\r\n",2) ;
        }
        else
        {
          get_val_of_err = TRUE ;
          ret = ERR_066 ;
          goto exit ;
        }
      }
      else if (
        (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_CREATE_NEW_ID,1)) ||
        (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_ADD_PORT_ID,1))
        )
      {
        /*
         *  Enter if this is a line starting with either "pss vlan create_new" or
         * "pss vlan add_port" or "pss multicast create_new" or
         * "pss multicast add_port"
         */
        unsigned
          int
            cpu_included,
            port_tagged,
            port_type,
            port,
            num_vlans,
            create_new ;
        unsigned
          long
            vidx ;

        port_tagged =0;


        if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_CREATE_NEW_ID,1))
        {
          create_new = 1 ;
        }
        else
        {
          create_new = 0 ;
        }
        if (!get_val_of(
              current_line,(int *)&match_index,PARAM_PSS_VLAN_ID_ID,1,
              &param_val,VAL_NUMERIC,err_msg))
        {
          vlan_id = param_val->value.ulong_value ;
        }
        else
        {
          get_val_of_err = TRUE ;
          ret = ERR_067 ;
          goto exit ;
        }
        num_vlans = get_max_vlan_id() ;
        if (vlan_id > num_vlans)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Specified Vlan id (%u) is beyond range for current system\r\n"
            "    (%u vlans altogether).\r\n",
            vlan_id,num_vlans) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_068 ;
          goto exit ;
        }
        if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_STANDARD_PORT_TYPE_ID,1))
        {
          port_type = PORT_STANDARD_EQUIVALENT ;
        }
        else if
          (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TRUNK_PORT_TYPE_ID,1))
        {
          port_type = PORT_TRUNK_EQUIVALENT ;
        }
        else
        {
          /*
           * Could find neither "standard_port" nor "trunk_port"
           * on line starting with "pss vlan create_new".
           * Probably SW error...
           */
          sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could find neither \"standar_port\" nor \"trunk_port\"!\r\n") ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_069 ;
          goto exit ;
        }
        if (port_type == PORT_STANDARD_EQUIVALENT)
        {
          port_type = 0 ;
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_UNIT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          unit = param_val->value.ulong_value ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_DEVICE_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          device = param_val->value.ulong_value ;
          err |=
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_PORT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          port = param_val->value.ulong_value ;
          if (err)
          {
            get_val_of_err = TRUE ;
            ret = ERR_070 ;
            goto exit ;
          }
#if !CODE_FOR_DEMO_ONLY
/* { */
          num_units = get_num_units() ;
          if (unit >= num_units)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified unit no. (%u) is beyond range for current system\r\n"
              "    (%u units altogether).\r\n",
              unit,num_units) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_071 ;
            goto exit ;
          }
          num_devices = get_num_devices_on_unit(unit) ;
          if ((int)num_devices < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of devices on specified unit (%u).\r\n"
              "    Error code %d from \'get_num_devices_on_unit()\'.\r\n",
              unit,(int)num_devices) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_072 ;
            goto exit ;
          }
          if (device >= num_devices)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified device no. (%u) is beyond range for current unit\r\n"
              "    (%u devices altogether on unit %u).\r\n",
              device,num_devices,unit) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_073 ;
            goto exit ;
          }
          num_ports = get_num_ports_on_device_on_unit(unit,device) ;
          if ((int)num_ports < 0)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Could not retrieve number of ports on specified unit,device (%u,%u).\r\n"
              "    Error code %d from \'get_num_ports_on_device_on_unit()\'.\r\n",
              unit,device,(int)num_ports) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_074 ;
            goto exit ;
          }
          if (port >= num_ports)
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Specified port no. (%u) is beyond range for current unit,device\r\n"
              "    (%u ports altogether on unit,device %u,%u).\r\n",
              port,num_ports,unit,device) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_075 ;
            goto exit ;
          }
/* } */
#endif
        }
        else
        {
          /*
           * Trunk.
           */
          port_type = 1 ;
          unit = 0 ;
          device = 0 ;
          err =
            get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_ID_OF_TRUNK_PORT_ID,1,
                &param_val,VAL_NUMERIC,err_msg) ;
          port = param_val->value.ulong_value ;
          if (err)
          {
            /*
             * Could not find "id_of_trunk_port"
             * on line starting with "pss vlan create_new".
             * Probably SW error...
             */
            get_val_of_err = TRUE ;
            ret = ERR_076 ;
            goto exit ;
          }
        }
        if (vlan_command)
        {
          /*
           * Enter if this is a vlan command. Get indication on whether port is tagged.
           */
          if (!get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_VLAN_PORT_TAGGED_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
          {
            port_tagged = (unsigned int)param_val->numeric_equivalent ;
          }
          else
          {
            get_val_of_err = TRUE ;
            ret = ERR_077 ;
            goto exit ;
          }
          if (port_tagged == ON_EQUIVALENT)
          {
            port_tagged = 1 ;
          }
          else
          {
            /*
             * NOT tagged.
             */
            port_tagged = 0 ;
          }
        }
        if (mac_multicast_command)
        {
          /*
           * Enter if this is a mac-multicast command. Get mac address.
           */
          if (!get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_MAC_ADDR_ID_ID,1,
                &param_val,VAL_TEXT,err_msg))
          {
            mac_addr = param_val->value.val_text ;
          }
          else
          {
            get_val_of_err = TRUE ;
            ret = ERR_078 ;
            goto exit ;
          }
          if (!is_legit_mac_address(mac_addr))
          {
            sal_sprintf(err_msg,
              "\r\n\n"
              "*** Input MAC address field (%s) is not a legitimate mac address\r\n"
              "    (12 hexadecimal digits)\r\n",
              mac_addr) ;
            send_string_to_screen(err_msg,FALSE) ;
            ret = ERR_079 ;
            goto exit ;
          }
        }
        if (create_new)
        {
          if (!get_val_of(
                current_line,(int *)&match_index,PARAM_PSS_VLAN_CPU_INCLUDED_ID,1,
                &param_val,VAL_SYMBOL,err_msg))
          {
            cpu_included = (unsigned int)param_val->numeric_equivalent ;
          }
          else
          {
            get_val_of_err = TRUE ;
            ret = ERR_080 ;
            goto exit ;
          }
          if (cpu_included == ON_EQUIVALENT)
          {
            cpu_included = 1 ;
          }
          else
          {
            /*
             * CPU NOT included.
             */
            cpu_included = 0 ;
          }
        }
        else
        {
          /*
           * If Vlan already exists then 'cpu_included' is always 'false'
           */
          cpu_included = 0 ;
        }
        if (mac_multicast_command)
        {
          sal_sprintf(err_msg,"brgSetMcEgPortsTblEntry") ;
          sal_sprintf(msg,"%u %u \"%s\" %u %u %u %u %u",
              create_new,vlan_id,mac_addr,port_type,unit,device,port,cpu_included) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = msg ;
        }
        else if (vlan_command)
        {
          sal_sprintf(err_msg,"brgSetVlanPortsTblEntry") ;
          sal_sprintf(msg,"%u %u %u %u %u %u %u %u",
              create_new,vlan_id,port_type,unit,device,port,port_tagged,cpu_included) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = msg ;
        }
        last_command[sizeof(last_command) - 1] = 0 ;
        strncpy(last_command,pss_cmd,sizeof(last_command) - 1) ;
        cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
        cmd_status_to_string(cmd_status,pss_error) ;
        sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
        send_string_to_screen(err_msg,TRUE) ;
        if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
        {
          send_string_to_screen("===> Message:\r\n       ",FALSE) ;
          send_string_to_screen(pss_cmd_output,TRUE) ;
          send_string_to_screen("\r\n===> ",FALSE) ;
          err = gt_status_to_string(pss_cmd_output,err_msg) ;
          if (!err)
          {
            send_string_to_screen(err_msg,FALSE) ;
          }
        }
        send_array_to_screen("\r\n",2) ;
        err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
        if (err)
        {
          send_string_to_screen(
            "\r\n\n"
            "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
            FALSE) ;
          ret = ERR_081 ;
          goto exit ;
        }
        if (code_val != GT_OK)
        {
          sal_sprintf(err_msg,
            "\r\n\n"
            "*** Galtis returned with error code on \'%s\' request.\r\n",
            last_command) ;
          send_string_to_screen(err_msg,FALSE) ;
          ret = ERR_082 ;
          goto exit ;
        }
        /*
         * For 'mac multicast' get system assigned identifier  of newly
         * created multicast group.
         */
        if (mac_multicast_command && create_new)
        {
          send_string_to_screen(
            "\r\n"
            "===> Getting identifier of newly assigned multicast group (vidx):\r\n",FALSE) ;
          sal_sprintf(err_msg,"gmcGetMcGroupId \"%s\", %u",mac_addr,vlan_id) ;
          pss_cmd = err_msg ;
          pss_cmd_fields = "" ;
          cmd_status = cmdEventRun(pss_cmd, pss_cmd_fields,&pss_cmd_output);
          cmd_status_to_string(cmd_status,pss_error) ;
          sal_sprintf(err_msg,"===> cmdEventRun returned with: %s",pss_error) ;
          send_string_to_screen(err_msg,TRUE) ;
          if ((NULL != pss_cmd_output) && (strlen(pss_cmd_output) > 0))
          {
            send_string_to_screen("===> Message:\r\n       ",FALSE) ;
            send_string_to_screen(pss_cmd_output,TRUE) ;
            send_string_to_screen("\r\n===> ",FALSE) ;
            err = gt_status_to_string(pss_cmd_output,err_msg) ;
            if (!err)
            {
              send_string_to_screen(err_msg,FALSE) ;
            }
            send_array_to_screen("\r\n",2) ;
            /*
             * If an error indication has been returned then 'vidx' cannot be
             * retrieved and displayed.
             */
            err = gt_argument_to_val(pss_cmd_output,&code_val,0,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** Could not retrieve status from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_083 ;
              goto exit ;
            }
            if (code_val != GT_OK)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** 1. Galtis returned with error code on \'gmcGetMcGroupId\' request.\r\n",
                FALSE) ;
              ret = ERR_084 ;
              goto exit ;
            }
            /*
             * Now get vidx.
             */
            err = gt_argument_to_val(pss_cmd_output,&vidx,1,TRUE,0) ;
            if (err)
            {
              send_string_to_screen(
                "\r\n\n"
                "*** 2. Could not retrieve vidx from \'gt_argument_to_val()\'.\r\n",
                FALSE) ;
              ret = ERR_085 ;
              goto exit ;
            }
            sal_sprintf(err_msg,
                "\r\n"
                "===> Assigned vidx is %lu\r\n",vidx) ;
            send_string_to_screen(
                err_msg,
                FALSE) ;
          }
          send_array_to_screen("\r\n",2) ;
        }
      }
      else
      {
        /*
         * No parameter related to VLAN or MULTICAST has been found on the line.
         * Probably SW error.
         */
        send_string_to_screen(
          "\r\n\n"
          "*** No parameter related to VLAN or MULTICAST has been found on the line.\r\n",
          FALSE) ;
        ret = ERR_086 ;
        goto exit ;
      }
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_MODULE_CLOSE,1))
    {
      GT_U8
        list[2] ;
      list[0] = 0;
      list[0] = 1;
      num_handled++ ;

#if DPSS_VERION_1_25_INCLUDE /*{*/
      sal_sprintf(msg,"appDemoHotRemove() -- Is not included in DPSS 1.25 - check how to replace this code") ;
#else
      appDemoHotRemove(list,1) ;
#endif  /*}*/
      send_string_to_screen(msg,TRUE) ;
      send_string_to_screen("This removal did not removed the PP completely... ", TRUE) ;
      goto exit ;
    }
    else if (!search_param_val_pairs(current_line,&match_index,PARAM_PSS_TEST_DMA_STATUS_ID,1))
    {
      GT_STATUS
        gt_status ;

      start_print_services();
      gt_status = osCacheDmaStatus() ;
      if (gt_status != GT_OK)
      {
          sal_sprintf(msg,"\n\rosCacheDmaStatus() failed -- returned with %u", gt_status) ;
          send_string_to_screen(msg,TRUE) ;
      }
      end_print_services();
      goto exit ;
    }

    if (num_handled == 0)
    {
      /*
       * No PSS variable has been entered.
       * There must be at least one!
       */
      send_string_to_screen(
          "\r\n\n"
          "*** No parameter has been indicated for handling.\r\n"
          "    At least one must be entered.\r\n",
          FALSE) ;
      ret = ERR_087 ;
      goto exit ;
    }
  }
exit:
  if (get_val_of_err)
  {
    /*
     * Value of some parameter could not be retrieved.
     */
    send_string_to_screen(
        "\r\n\n"
        "*** Procedure \'get_val_of\' returned with error indication:\r\n",TRUE) ;
    send_string_to_screen(err_msg,TRUE) ;
  }
  return (ret) ;
}
/* } */
#else
/* { */
/*
 * LINK_PSS_LIBRARIES is defined to ZERO, so empty subject_pss()
 * is given.
 */
STATUS
  subject_pss(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  int
    ret ;
  ret = 0 ;
  send_array_to_screen("\r\n",2);
  send_string_to_screen(
    "'subject_pss()' function was called.\r\n"
    " Please define LINK_PSS_LIBRARIES in dune_rpc.h\r\n"
    " to 1 to activate calls to PSS.",TRUE) ;
  return (ret) ;
}
/* } */
#endif /* LINK_PSS_LIBRARIES */
