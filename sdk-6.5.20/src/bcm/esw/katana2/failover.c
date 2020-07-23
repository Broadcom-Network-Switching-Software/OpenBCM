/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Katana2 failover API
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_KATANA2_SUPPORT) &&  defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>


STATIC int
_bcm_kt2_failover_nhi_get(int unit, bcm_gport_t port, int *nh_index)
{
    int vp=0xFFFF;
    ing_dvp_table_entry_t dvp;

    if (!BCM_GPORT_IS_MPLS_PORT(port) &&
         !BCM_GPORT_IS_MIM_PORT(port) ) {
        return BCM_E_PARAM;
    }

    if ( BCM_GPORT_IS_MPLS_PORT(port)) {
         vp = BCM_GPORT_MPLS_PORT_ID_GET(port);
    } else if ( BCM_GPORT_IS_MIM_PORT(port)) {
         vp = BCM_GPORT_MIM_PORT_ID_GET(port);
    }

    if (vp >= soc_mem_index_count(unit, SOURCE_VPm)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN (READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY, vp, &dvp));

    /* Next-hop index is used for multicast replication */
    *nh_index = soc_ING_DVP_TABLEm_field32_get(unit, &dvp, NEXT_HOP_INDEXf);
    return BCM_E_NONE;
}
/*
 * Function:
 *        bcm_kt2_failover_status_set
 * Purpose:
 *        Set the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_kt2_failover_status_set(int unit,
                                     bcm_failover_element_t *failover,
                                     int value)
{
    int rv = BCM_E_UNAVAIL;
    int nh_index;
    uint32 label_action = -1;
    bcm_l3_egress_t egr;
    initial_prot_group_table_entry_t  prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    initial_prot_nhi_table_1_entry_t   prot_nhi_1_entry;

    if ((value < 0) || (value > 1)) {
       return BCM_E_PARAM;
    }

    if (failover->failover_id != BCM_FAILOVER_INVALID) {
         /* Group protection for Port and Tunnel: Egress and Ingress */
         BCM_IF_ERROR_RETURN(
              bcm_tr2_failover_id_validate ( unit, failover->failover_id ));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_GROUP_TABLEm,
                   MEM_BLOCK_ANY, failover->failover_id, &prot_group_entry));

         soc_mem_field32_set(unit, INITIAL_PROT_GROUP_TABLEm,
                   &prot_group_entry, REPLACE_ENABLEf, value);

         BCM_IF_ERROR_RETURN(
              soc_mem_write(unit, INITIAL_PROT_GROUP_TABLEm,
                   MEM_BLOCK_ALL, failover->failover_id, &prot_group_entry));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
               MEM_BLOCK_ANY, failover->failover_id, &rx_prot_group_entry));

         soc_mem_field32_set(unit, RX_PROT_GROUP_TABLEm,
               &rx_prot_group_entry, DROP_DATA_ENABLEf, value);

         rv =   soc_mem_write(unit, RX_PROT_GROUP_TABLEm,
               MEM_BLOCK_ALL, failover->failover_id,  &rx_prot_group_entry);

    }else if (failover->intf != BCM_IF_INVALID) {
        /* Only Egress is applicable */
        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, failover->intf)) {
            nh_index = failover->intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_index = failover->intf  - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                              MEM_BLOCK_ANY, nh_index, &prot_nhi_1_entry));

         soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLE_1m,
                             &prot_nhi_1_entry, REPLACE_ENABLEf, value);

         rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_1_entry);
        if (rv < 0) {
            return rv;
        }

        /* handle 1+1 protection on MSP (multiplex section protection) */
        if (soc_feature(unit, soc_feature_mpls)) {
            bcm_l3_egress_t_init(&egr);
            BCM_IF_ERROR_RETURN(bcm_xgs3_l3_egress_get(unit,
                                failover->intf, &egr));
            if (BCM_SUCCESS(_bcm_esw_failover_egr_check (unit, &egr))) {
                if (_BCM_MULTICAST_IS_SET(egr.failover_mc_group)) {

                    if (BCM_XGS3_L3_MPLS_LBL_VALID(egr.mpls_label)) {
                        BCM_IF_ERROR_RETURN(
                                bcm_tr_mpls_get_label_action(unit, nh_index,
                                    &label_action));
                        switch (label_action) {
                            case _BCM_MPLS_ACTION_SWAP:
                                LOG_CLI((BSL_META_U(unit,
                                                    "swap\n")));
                                rv = _bcm_kt_egress_object_p2mp_set(unit,
                                        egr.failover_mc_group,
                                        BCM_MPLS_SWITCH_ACTION_SWAP, value);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
    } else if (failover->port != BCM_GPORT_INVALID) {
         /* Individual protection for Pseudo-wire: Egress and Ingress */
         BCM_IF_ERROR_RETURN (
               _bcm_kt2_failover_nhi_get(unit, failover->port , &nh_index));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                              MEM_BLOCK_ANY, nh_index, &prot_nhi_1_entry));

         soc_mem_field32_set(unit, INITIAL_PROT_NHI_TABLE_1m,
                             &prot_nhi_1_entry, REPLACE_ENABLEf, value);

         rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
                            MEM_BLOCK_ALL, nh_index, &prot_nhi_1_entry);

    }
    return rv;
}


/*
 * Function:
 *        bcm_kt2_failover_status_get
 * Purpose:
 *        Get the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_kt2_failover_status_get(int unit,
                                     bcm_failover_element_t *failover,
                                     int  *value)
{
    initial_prot_group_table_entry_t  prot_group_entry;
    rx_prot_group_table_entry_t  rx_prot_group_entry;
    initial_prot_nhi_table_1_entry_t   prot_nhi_1_entry;
    int nh_index;


    if (failover->failover_id != BCM_FAILOVER_INVALID) {

         BCM_IF_ERROR_RETURN(
            bcm_tr2_failover_id_validate ( unit, failover->failover_id ));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_GROUP_TABLEm,
                   MEM_BLOCK_ANY, failover->failover_id, &prot_group_entry));

         *value =   soc_mem_field32_get(unit, INITIAL_PROT_GROUP_TABLEm,
                   &prot_group_entry, REPLACE_ENABLEf);

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, RX_PROT_GROUP_TABLEm,
               MEM_BLOCK_ANY, failover->failover_id, &rx_prot_group_entry));

         *value &=   soc_mem_field32_get(unit, RX_PROT_GROUP_TABLEm,
               &rx_prot_group_entry, DROP_DATA_ENABLEf);

    } else if (failover->intf != BCM_IF_INVALID) {

        if (BCM_XGS3_L3_EGRESS_IDX_VALID(unit, failover->intf)) {
            nh_index = failover->intf - BCM_XGS3_EGRESS_IDX_MIN(unit);
        } else {
            nh_index = failover->intf  - BCM_XGS3_DVP_EGRESS_IDX_MIN(unit);
        }

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_1_entry));

         *value =   soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLE_1m,
                   &prot_nhi_1_entry, REPLACE_ENABLEf);

    } else if (failover->port != BCM_GPORT_INVALID) {

         BCM_IF_ERROR_RETURN (
               _bcm_kt2_failover_nhi_get(unit, failover->port , &nh_index));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_1_entry));

         *value =   soc_mem_field32_get(unit, INITIAL_PROT_NHI_TABLE_1m,
                   &prot_nhi_1_entry, REPLACE_ENABLEf);

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *        bcm_kt2_failover_prot_nhi_cleanup
 * Purpose:
 *        Cleanup  the  entry  for  PROT_NHI
 * Parameters:
 *        IN :  unit
 *           IN :  Primary Next Hop Index
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_kt2_failover_prot_nhi_cleanup  (int unit, int nh_index)
{
    initial_prot_nhi_table_entry_t    prot_nhi_entry;
    initial_prot_nhi_table_1_entry_t  prot_nhi_1_entry;
    int rv;

    rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLEm,
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_entry);

    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&prot_nhi_entry, 0, sizeof(initial_prot_nhi_table_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLEm,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_entry);

    if (rv < 0){
        return rv;
    }

     rv = soc_mem_read(unit, INITIAL_PROT_NHI_TABLE_1m,
                             MEM_BLOCK_ANY, nh_index, &prot_nhi_1_entry);

    if (rv < 0){
        return BCM_E_NOT_FOUND;
    }

    sal_memset(&prot_nhi_1_entry, 0, sizeof(initial_prot_nhi_table_1_entry_t));

    rv = soc_mem_write(unit, INITIAL_PROT_NHI_TABLE_1m,
                       MEM_BLOCK_ALL, nh_index, &prot_nhi_1_entry);

    return rv;

}

#endif /* defined(BCM_KATANA2_SUPPORT) &&  defined(INCLUDE_L3) */
