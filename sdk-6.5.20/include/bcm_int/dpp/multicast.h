/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef _BCM_INT_DPP_MULTICAST_H
#define _BCM_INT_DPP_MULTICAST_H

#include <sal/types.h>
#include <bcm/types.h>
#include <bcm/vlan.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>

#define _BCM_PETRA_MULTICAST_TM_TYPE (0)



/* Types */
typedef enum
{
  /*
   *  Multicast ingress type
   */
  BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY=0,
  BCM_DPP_MULTICAST_TYPE_INGRESS = 0,
  /*
   *  Multicast egress type
   */
  BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY=1,
  BCM_DPP_MULTICAST_TYPE_EGRESS = 1,
  /* 
   *  Multicast ingress & egress
   */ 
  BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS=2,
  /* 
   *  Multicast nof types
   */  
  BCM_DPP_MULTICAST_NOF_TYPES=3
}BCM_DPP_MULTICAST_TYPE;


/*
 * Function:
 *      _bcm_petra_multicast_group_to_id
 * Purpose:
 *      Convert a BCM-Multicast ID to Soc_petra-Multicast ID
 * Parameters:
 *      group        (IN) BCM Multicast
 *      multicast_id (OUT) Soc_petra Multicast ID
 */
int
 _bcm_petra_multicast_group_to_id(bcm_multicast_t group, SOC_TMC_MULT_ID *multicast_id);


int
 _bcm_petra_multicast_group_from_id(bcm_multicast_t *group, int type, SOC_TMC_MULT_ID multicast_id);



/*
 * Function:
 *      _bcm_petra_multicast_ingress_group_exists, _bcm_petra_multicast_egress_group_exists
 * Purpose:
 *      Check if given multicast ingress/egress group id is open, returning no errors if possible.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
_bcm_petra_multicast_ingress_group_exists(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open);

/*
 * Function:
 *      _bcm_petra_multicast_is_group_exist
 * Purpose:
 *      Check if given multicast group id is existed or not according to expected_open.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      expected_open    (IN) Expected multicast group status.
 */
int
_bcm_petra_multicast_is_group_exist(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx,uint8 expected_open);

#endif    /* !_BCM_INT_DPP_VLAN_H */
