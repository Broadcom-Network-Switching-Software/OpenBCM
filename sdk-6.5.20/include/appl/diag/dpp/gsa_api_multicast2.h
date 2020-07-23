/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
 
 
#ifndef __GSA_API_MULTICAST_INCLUDED__ 
/* { */ 
#define __GSA_API_MULTICAST_INCLUDED__ 
 
/************* 
 * INCLUDES  * 
 */
/* { */ 
#include <soc/dpp/SAND/Utils/sand_header.h>

#include "gsa_framework.h"

/* } */ 
/************* 
 * DEFINES   * 
 */
/* { */ 

 
/* } */ 
 
/************* 
 * MACROS    * 
 */ 
/* { */ 
 
/* } */ 
 
/************* 
 * TYPE DEFS * 
 */ 
/* { */ 
 


typedef enum
{
  /*
   *  Egress + Ingress multicast, based on pre-defined policy. 
   */
  GSA_P_MC_TYPE_ALL_AUTO=0,
  /*
   *  Egress multicast, based on pre-defined policy.          
   */
  GSA_P_MC_TYPE_EGR_AUTO=1,
  /*
   *  Egress multicast, VLAN membership replication (set of 
   *  system ports).                                          
   */
  GSA_P_MC_TYPE_EGR_VLAN=2,
  /*
   *  Egress multicast, General replication (set of system 
   *  ports + Copy-Unique-Data).                              
   */
  GSA_P_MC_TYPE_EGR_GEN=3,
  /*
   *  Ingress multicast, based on pre-defined policy.         
   */
  GSA_P_MC_TYPE_INGR_AUTO=4,
  /*
   *  Ingress multicast, Device-Level-Scheduled (set of 
   *  destination FAP-s).                                     
   */
  GSA_P_MC_TYPE_INGR_DEV_SCHED=5,
  /*
   *  Ingress multicast, Device-Level-Scheduled (set of 
   *  destination system ports).                              
   */
  GSA_P_MC_TYPE_INGR_PORT_SCHED=6,
  /*
   *  Fabric multicast, not scheduled by the egress.          
   */
  GSA_P_MC_TYPE_INGR_FABRIC=7,
  /*
   *  Total number of Multicast types.                        
   */
  GSA_P_NOF_MC_TYPES=8,
}GSA_MC_TYPE;

typedef enum
{
  /*
  *  System Physical FAP Port.Matching Index Range: 0  4095. 
  */
  GSA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT=0,
  /*
  *  System LAG Id. Matching Index Range: 0  255.           
  */
  GSA_DEST_SYS_PORT_TYPE_LAG=1,
  /*
  *  Total number of system-ports types.                     
  */
  GSA_DEST_SYS_PORT_NOF_TYPES=2,
}GSA_DEST_SYS_PORT_TYPE;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
    /*
     *  LAG id or system-physical-port id.                      
     */
  GSA_DEST_SYS_PORT_TYPE type;
  /*
   *  According to the System-Port type, one of the 
   *  following:LAG id. Range: 0  255.System Physical Port. 
   *  Range: 0  4K-1.                                        
   */
  uint32 id;
}GSA_DEST_SYS_PORT_INFO;

/* } */ 
 
/************* 
 * GLOBALS   * 
 */ 
/* { */ 
 
/* } */ 
 
/************* 
 * FUNCTIONS * 
 */ 
/* { */ 
 

/*********************************************************************
* NAME:         
*     gsa_mc_grp_open
* TYPE:         
*   PROC        
* FUNCTION:       
*     Open Multicast Group.                                   
* INPUT:
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access.                     
*  SOC_SAND_IN  uint32                 mc_id - 
*     The index of the multicast group to open. Range: 0 - 
*     16K-1.                                                  
*  SOC_SAND_IN  GSA_MC_TYPE               mc_type - 
*     The type of the multicast to open.                      
*  SOC_SAND_IN  uint32                 mc_grp_size - 
*     The number of members in the multicast group.           
*  SOC_SAND_IN  GSA_DEST_SYS_PORT_INFO    *mc_grp_members - 
*     A list of multicast group members; Destination Physical 
*     System Ports. The size of the array is mc_grp_size. 
*     Note: for GSA_P_MC_TYPE_INGR_FABRIC, this field is 
*     ignored and may be set to NULL.                         
*  SOC_SAND_IN  uint32                 *per_member_cud - 
*     If applicable (e.g. for GSA_P_MC_TYPE_EGR_GEN multicast 
*     type) - the Copy-Unique-Data to embed into the OTMH 
*     outlif field. Note: for Ethernet Ports, this field is 
*     interpreted as the ARP pointer. For MC types that don't 
*     need the CUD information, this field is ignored, and may 
*     be set to NULL.                                         
* REMARKS:         
*     1. In case the MC group can not be opened due to 
*     insufficient memory, the API returns error. 2. If the MC 
*     group with the specified MC index already exists, it is 
*     updated to the specified member list.                   
* RETURNS:         
*     OK or ERROR indication.
 */

uint32  
  gsa_mc_grp_open(
    SOC_SAND_IN  uint32                 mc_id,
    SOC_SAND_IN  GSA_MC_TYPE               mc_type,
    SOC_SAND_IN  uint32                 mc_grp_size,
    SOC_SAND_IN  GSA_DEST_SYS_PORT_INFO    *mc_grp_members,
    SOC_SAND_IN  uint32                 *per_member_cud
  );

/*********************************************************************
* NAME:         
*     gsa_mc_grp_close
* TYPE:         
*   PROC        
* FUNCTION:       
*     Close a Multicast Group if exists.                      
* INPUT:
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access.                     
*  SOC_SAND_IN  GSA_MC_TYPE               mc_type - 
*     SOC_SAND_IN GSA_P_MC_TYPE mc_type                           
*  SOC_SAND_IN  uint32                 mc_id - 
*     The index of the multicast group to close. Range: 0 -
*     16K-1. SOC_SAND_IN GSA_P_MC_TYPE mc_type - The type of the 
*     multicast to close.                                     
* REMARKS:         
*     None.                                                   
* RETURNS:         
*     OK or ERROR indication.
 */

uint32  
  gsa_mc_grp_close(
    SOC_SAND_IN  GSA_MC_TYPE               mc_type,
    SOC_SAND_IN  uint32                 mc_id
  );

/*********************************************************************
* NAME:         
*     gsa_mc_grp_get
* TYPE:         
*   PROC        
* FUNCTION:       
*     Open Multicast Group.                                   
* INPUT:
*  SOC_SAND_IN  int                 unit - 
*     Identifier of the device to access.                     
*  SOC_SAND_IN  uint32                 mc_id - 
*     The index of the multicast group to open. Range: 0 -
*     16K-1.                                                  
*  SOC_SAND_IN  GSA_MC_TYPE               mc_type - 
*     The type of the multicast to open.                      
*  SOC_SAND_OUT GSA_DEST_SYS_PORT_INFO    *mc_grp_members - 
*     The list of multicast group members; Destination 
*     Physical System Ports. The size of the array is 
*     exact_mc_grp_size. Note: for SWP_P_MC_TYPE_INGR_FABRIC, 
*     this field is ignored and is set to NULL.               
*  SOC_SAND_OUT uint32                 *per_member_cud - 
*     If applicable (e.g. for SWP_P_MC_TYPE_EGR_GEN multicast 
*     type) - the Copy-Unique-Data to embed into the OTMH 
*     outlif field. Note: for Ethernet Ports, this field is 
*     interpreted as the ARP pointer. For MC types that don't 
*     need the CUD information, this field is ignored, and is 
*     set to NULL.                                            
*  SOC_SAND_OUT uint32                 *exact_mc_grp_size - 
*     The actual number of members in the multicast group.    
*  SOC_SAND_OUT uint8                 *is_open - 
*     The parameter indicates whether the group is open and 
*     can be retrieved.                                       
* REMARKS:         
*     Except the ingress fully scheduled type, it gets the 
*     egress multicast group accordingly to the policy.       
* RETURNS:         
*     OK or ERROR indication.
 */

uint32  
  gsa_mc_grp_get(
    SOC_SAND_IN  uint32                 mc_id,
    SOC_SAND_IN  GSA_MC_TYPE               mc_type,
    SOC_SAND_OUT GSA_DEST_SYS_PORT_INFO    *mc_grp_members,
    SOC_SAND_OUT uint32                 *per_member_cud,
    SOC_SAND_OUT uint32                 *exact_mc_grp_size,
    SOC_SAND_OUT uint8                 *is_open
  );

uint32  
  gsa_mc_grp_get_size(
    SOC_SAND_IN  uint32                 mc_id,
    SOC_SAND_IN  GSA_MC_TYPE               mc_type,
    SOC_SAND_OUT uint32                 *mc_grp_size
  );


#if GSA_DEBUG


const char*
  swp_p_GSA_MC_TYPE_to_string(
    SOC_SAND_IN GSA_MC_TYPE enum_val
  );

void
  gsa_GSA_DEST_SYS_PORT_INFO_print(
    SOC_SAND_IN GSA_DEST_SYS_PORT_INFO *info
  );


void
  gsa_GSA_DEST_SYS_PORT_INFO_table_format_print(
    SOC_SAND_IN GSA_DEST_SYS_PORT_INFO *info
  );

#endif /* GSA_DEBUG */
 
void
  gsa_MC_TYPE_to_swp_p(
    SOC_SAND_IN  GSA_MC_TYPE    mc_type,
    SOC_SAND_OUT SWP_P_MC_TYPE  *swp_p_mc_type
  );

void
  gsa_GSA_DEST_SYS_PORT_INFO_clear(
    SOC_SAND_OUT GSA_DEST_SYS_PORT_INFO *info
  );

void
  gsa_DEST_SYS_PORT_INFO_to_petra(
    SOC_SAND_IN  GSA_DEST_SYS_PORT_INFO   *info,
    SOC_SAND_OUT SOC_PETRA_DEST_SYS_PORT_INFO *soc_petra_info
  );

void
  gsa_DEST_SYS_PORT_INFO_from_petra(
    SOC_SAND_IN  SOC_PETRA_DEST_SYS_PORT_INFO *soc_petra_info,
    SOC_SAND_OUT GSA_DEST_SYS_PORT_INFO   *info
  );

uint32
  gsa_petra_api_multicast_group_print(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              mc_id,
    SOC_SAND_IN  GSA_MC_TYPE           mc_type
  );

/* } */ 
 
#include <soc/dpp/SAND/Utils/sand_footer.h>
 
/* } __GSA_API_MULTICAST2_INCLUDED__*/ 
#endif 
