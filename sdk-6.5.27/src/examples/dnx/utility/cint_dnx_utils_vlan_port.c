/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * This file provides VLAN-Port creation and modification functions that supprt various types of ACs 
 * by supplying flags that represent the required AC type.                                           */ 

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */

/* Default values for fields */
bcm_vlan_t DEFAULT_VLAN=100;
bcm_vlan_port_match_t DEFAULT_CRITERIA=BCM_VLAN_PORT_MATCH_PORT_VLAN;
bcm_failover_t DEFAULT_INGRESS_FAILOVER_ID=5;
bcm_failover_t DEFAULT_FEC_FAILOVER_ID=25;
bcm_multicast_t DEFAULT_INGRESS_MC_GROUP=22;
uint32 DEFAULT_MACT_GROUP=3;
bcm_if_t ALLOCATED_FEC_ID=0;
bcm_gport_t DEFAULT_PORT_LEARN_CHANGE = 17;

/* Enum for AC Types */
enum ac_type_e {
    ac_type_wide_data = 0,
    ac_type_stats = 1,
    ac_type_ingress_protection = 2,
    ac_type_fec_protection = 3,
    ac_type_ingress_mc = 4,
    ac_type_mact_group = 5,
    ac_type_port_learn_change = 6,
    ac_type_symmetric_optimized = 7,
    ac_type_last = 8
};
uint32 NOF_AC_TYPES = ac_type_last;


/* AC Type additional data format */
struct ac_types_data_s {
    int is_alloc_additional_objects[NOF_AC_TYPES];
    int *type_data[NOF_AC_TYPES];
};

/* AC Type specific data formats */
struct ac_type_data_ingress_protection {
    int is_primary;
    bcm_failover_t ingress_failover_id;
};

struct ac_type_data_fec_protection {
    int is_primary;
    bcm_failover_t fec_failover_id;
    bcm_if_t fec_id;
};

struct ac_type_data_ingress_mc {
    int multicast_flags;
    bcm_multicast_t mc_group;
};

struct ac_type_data_port_learn_change {
    bcm_gport_t learnt_port;
};

/* AC Type flag manipulation utility functions
   */
uint32 vlan_port__ac_type_flag_set(ac_type_e ac_type)
{
    return (1 << ac_type);
}

int vlan_port__is_ac_type_in_flag(ac_type_e ac_type, uint32 ac_type_flags)
{
    uint32 type_flag = vlan_port__ac_type_flag_set(ac_type);
    return ((type_flag & ac_type_flags) ? TRUE : FALSE);
}

/*
 * Wrapper for the AC creation according to unique AC type configuration that may
 * result in specific result types.
 *
 * INPUT: 
 *   port: phy port
 *   vlan: Outer VID
 *   vsi: VSI to associate
 *   direction_flags: BCM_VLAN_PORT_CREATE_INGRESS_ONLY / EGRESS_ONLY
 *   ac_type_flags - The unique attributes of the AC: WIDE_DATA / STATS / INGRESS_PROTECTION etc
 *   ac_type_data - Additional data that a specific ac_type may require
 *   vlan_port - The return VLAN-Port structure of the created AC
 */
int vlan_port__create(int unit,
                      bcm_gport_t port,
                      bcm_vlan_t vlan,
                      bcm_vlan_t vsi,
                      uint32 direction_flags,
                      uint32 ac_type_flags,
                      ac_types_data_s *ac_type_data,
                      bcm_vlan_port_t *vlan_port)
{
    char error_msg[200]={0,};
    int is_type_data_supplied = (ac_type_data == NULL) ? FALSE : TRUE;
    bcm_l3_egress_t l3_egress;
    bcm_if_t fec_standby_id, fec_primay_id, learn_fec_id;
    int is_allocate = FALSE;
    bcm_if_t fec_id = 0;
    int is_primary = TRUE;
    bcm_failover_t fec_failover_id = DEFAULT_FEC_FAILOVER_ID;
    char *proc_name = "vlan_port__create";

    bcm_vlan_port_t_init(vlan_port);

    /*
     * Configure default match 
     */
    vlan_port->port = port;
    vlan_port->criteria = DEFAULT_CRITERIA;
    vlan_port->match_vlan = vlan;
    vlan_port->vsi = vsi;

    /*
     * Configure direction flags
     */
    vlan_port->flags |= direction_flags;

    /*
     * Configure specific type fields
     */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_wide_data)) {
        vlan_port->flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_symmetric_optimized)) {
        vlan_port->flags |= BCM_VLAN_PORT_VSI_BASE_VID;
        vlan_port->criteria = BCM_VLAN_PORT_MATCH_PORT;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_stats)) {
        vlan_port->flags |= BCM_VLAN_PORT_STAT_INGRESS_ENABLE;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_protection)) {
        bcm_failover_t ing_failover_id = DEFAULT_INGRESS_FAILOVER_ID;

        if (!is_type_data_supplied) {
            is_allocate = TRUE;
        } else if (ac_type_data->type_data[ac_type_ingress_protection]) {
            ac_type_data_ingress_protection *data_ingress_protection;
            sal_memcpy(&data_ingress_protection, &(ac_type_data->type_data[ac_type_ingress_protection]), sizeof(data_ingress_protection));
            ing_failover_id = data_ingress_protection->ingress_failover_id;
            is_primary = data_ingress_protection->is_primary;
            if (ac_type_data->is_alloc_additional_objects[ac_type_ingress_protection]) {
                is_allocate = TRUE;
            }
        }

        if (is_allocate) {
            snprintf(error_msg, sizeof(error_msg), "failed for Ingress Protection with failover_id - %d", ing_failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_create(unit, BCM_FAILOVER_INGRESS | BCM_FAILOVER_WITH_ID, &ing_failover_id), error_msg);

            snprintf(error_msg, sizeof(error_msg), "failed for Ingress failover_id - %d", ing_failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_set(unit, ing_failover_id, is_primary), error_msg);
        }

        BCM_FAILOVER_SET(vlan_port->ingress_failover_id, ing_failover_id, BCM_FAILOVER_TYPE_INGRESS);
        vlan_port->ingress_failover_port_id = (is_primary) ? TRUE : FALSE;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection)) {
        ac_type_data_fec_protection *data_fec_protection = NULL;

        if (!is_type_data_supplied) {
            is_allocate = TRUE;
        } else if (ac_type_data->type_data[ac_type_fec_protection] != NULL) {
            sal_memcpy(&data_fec_protection, &(ac_type_data->type_data[ac_type_fec_protection]), sizeof(data_fec_protection));
            fec_failover_id = data_fec_protection->fec_failover_id;
            is_primary = data_fec_protection->is_primary;
            fec_id = data_fec_protection->fec_id;
            if (ac_type_data->is_alloc_additional_objects[ac_type_fec_protection]) {
                is_allocate = TRUE;
            }
        }

        if (is_allocate) {
            snprintf(error_msg, sizeof(error_msg), "failed for FEC Protection with failover_id - %d", fec_failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_create(unit, BCM_FAILOVER_FEC | BCM_FAILOVER_WITH_ID, &fec_failover_id), error_msg);
            snprintf(error_msg, sizeof(error_msg), "failed for FEC failover_id - %d", fec_failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_set(unit, fec_failover_id, !is_primary), error_msg);

            /*
             * Create a Secondary FEC
             */
            bcm_l3_egress_t_init(&l3_egress);
            l3_egress.port = port;
            l3_egress.failover_id = fec_failover_id;
            fec_standby_id = fec_id + 1;
            uint32 flags = (fec_id) ? BCM_L3_WITH_ID : 0;

            if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
            {
                l3_egress.flags2 = BCM_L3_FLAGS2_FWD_ONLY;
            }

            snprintf(error_msg, sizeof(error_msg), "failed for Secondary FEC with ID - %d", fec_standby_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | flags , &l3_egress, &fec_standby_id), error_msg);
            ALLOCATED_FEC_ID = fec_standby_id;
            printf("%s(): fec_standby_id = %d\n", proc_name, fec_standby_id);
            /*
             * Create a Primary FEC
             */
            bcm_l3_egress_t_init(&l3_egress);
            l3_egress.port = port;
            l3_egress.failover_id = fec_failover_id;
            l3_egress.failover_if_id = fec_standby_id;
            fec_primay_id = fec_standby_id - 1;
            if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
            {
                l3_egress.flags2 = BCM_L3_FLAGS2_FWD_ONLY;
            }
            snprintf(error_msg, sizeof(error_msg), "failed for Primary FEC with ID - %d", fec_primay_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3_egress, &fec_primay_id), error_msg);

            /* 
             * In IOP mode, there's a need to translate the glocal FEC-ID to local for L2 learning
             */
            int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
            if (!is_jericho2_mode) {
                snprintf(error_msg, sizeof(error_msg), "failed for FEC %d", fec_primay_id);
                BCM_IF_ERROR_RETURN_MSG(fec_id_alignment_field_add(unit, port, BCM_L3_ITF_VAL_GET(fec_primay_id)), error_msg);
            }

            printf("%s(): fec_primay_id = %d\n", proc_name, fec_primay_id);
            learn_fec_id = fec_primay_id;
            if (is_type_data_supplied) {
                data_fec_protection->fec_id = fec_primay_id;
                data_fec_protection->fec_failover_id = fec_failover_id;
            }
        } else {
            /*
             * Use the FEC-ID from the data type structure or the Default FEC-ID
             */
            learn_fec_id = (fec_id / 2) * 2;
        }

        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(vlan_port->failover_port_id, learn_fec_id);
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_mc)) {

        bcm_multicast_t mc_group_id = DEFAULT_INGRESS_MC_GROUP;
        
        if (!is_type_data_supplied) {

            /* 
             * Allocate an Ingress protection MC group
             */
            snprintf(error_msg, sizeof(error_msg), "failed for group %d", mc_group_id);
            BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &mc_group_id, 0), error_msg);
        }

        vlan_port->failover_mc_group = mc_group_id;
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_mact_group)) {
        vlan_port->group = DEFAULT_MACT_GROUP;
    }

    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_port_learn_change)) {
        vlan_port->failover_port_id = DEFAULT_PORT_LEARN_CHANGE;

        ac_type_data_port_learn_change *data_port_learn_change = NULL;

        if (is_type_data_supplied && (ac_type_data->type_data[ac_type_port_learn_change] != NULL)) {
            sal_memcpy(&data_port_learn_change, &(ac_type_data->type_data[ac_type_port_learn_change]), sizeof(data_port_learn_change));
            vlan_port->failover_port_id = data_port_learn_change->learnt_port;
        }
    }

    /* 
     * Create the VLAN-Port
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, vlan_port), "");
    printf("%s(): Created vlan_port->vlan_port_id = 0x%x\n", proc_name, vlan_port->vlan_port_id);

    /*
     * Adjustments following the VLAN-Port creation
     */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection)) {

        bcm_if_t uptate_fec_id = (is_primary) ? fec_primay_id : fec_standby_id;
        if (is_type_data_supplied && (ac_type_data->type_data[ac_type_fec_protection] != NULL)) {
            if (fec_id) {
                uptate_fec_id = (is_primary) ? fec_id : (fec_id + 1);
            }
        }

        /* Retrieve the FEC TBD */
        snprintf(error_msg, sizeof(error_msg), "failed for FEC-ID - %d", uptate_fec_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit, uptate_fec_id, &l3_egress), error_msg);

        /* Set the Out-LIF in the Updated FEC entry */
        l3_egress.intf = vlan_port->vlan_port_id;
        l3_egress.port = port;
        printf("%s(): Adjustment for FEC-ID 0x%x to Out-AC 0x%x\n", proc_name, uptate_fec_id, vlan_port->vlan_port_id);
        snprintf(error_msg, sizeof(error_msg), "failed when adding Out-AC to FEC 0x%x", uptate_fec_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID | BCM_L3_REPLACE, &l3_egress, &uptate_fec_id), error_msg);
    }

    return BCM_E_NONE;
}


/*
 * The function retrieves unique ac_type configurations that were applied on a
 * given gport.
 *
 * INPUT: 
 *   gport: AC gport
 *   ac_type_flags - The returned unique attributes of the AC: WIDE_DATA / STATS / INGRESS_PROTECTION etc
 */
int vlan_port__ac_type_get(int unit,
                           bcm_gport_t gport,
                           uint32 *ac_type_flags)
{
    bcm_vlan_port_t vlan_port;
    int failover_id_val;
    char error_msg[200]={0,};

    /*
     * Retrieve the VLAN-Port structure
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = gport;

    snprintf(error_msg, sizeof(error_msg), "failed for gport - %d", gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_find(unit, &vlan_port), error_msg);

    /*
     * Check what ac_types are represented by the VLAN-Port
     */
    *ac_type_flags = 0;
    if (vlan_port.flags & BCM_VLAN_PORT_INGRESS_WIDE) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_wide_data);
    }
    if (vlan_port.flags & BCM_VLAN_PORT_VSI_BASE_VID) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_symmetric_optimized);
    }
    if (vlan_port.flags & BCM_VLAN_PORT_STAT_INGRESS_ENABLE) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_stats);
    }
    if (vlan_port.ingress_failover_id) {
        BCM_FAILOVER_ID_GET(failover_id_val, vlan_port.ingress_failover_id);
        if (failover_id_val == DEFAULT_INGRESS_FAILOVER_ID) {
            *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_ingress_protection);
        }
    }
    if ((vlan_port.failover_port_id) && (BCM_GPORT_IS_FORWARD_PORT(vlan_port.failover_port_id))) {
        bcm_if_t fec_id;
        bcm_l3_egress_t l3_egress;
        int primary_fec_outlif, secondary_fec_outlif, gport_id_val;

        BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(fec_id, vlan_port.failover_port_id);

        /* Retrieve the FEC that is used for learning in order to verify it contains the gport as the Out-LIF */
        snprintf(error_msg, sizeof(error_msg), "failed for FEC-ID - %d", fec_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit, fec_id, &l3_egress), error_msg);
        primary_fec_outlif = BCM_FORWARD_ENCAP_ID_VAL_GET(l3_egress.intf);

        fec_id++;
        snprintf(error_msg, sizeof(error_msg), "failed for FEC-ID - %d", fec_id);
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit, fec_id, &l3_egress), error_msg);
        secondary_fec_outlif = BCM_FORWARD_ENCAP_ID_VAL_GET(l3_egress.intf);

        gport_id_val = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(BCM_GPORT_VLAN_PORT_ID_GET(gport));
        if ((primary_fec_outlif == gport_id_val) || (secondary_fec_outlif == gport_id_val)) {
            *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_fec_protection);
        }
    }
    if (vlan_port.failover_mc_group == DEFAULT_INGRESS_MC_GROUP) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_ingress_mc);
    }
    if (vlan_port.group == DEFAULT_MACT_GROUP) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_mact_group);
    }

    if ((vlan_port.failover_port_id) && (!BCM_GPORT_IS_FORWARD_PORT(vlan_port.failover_port_id))) {
        *ac_type_flags |= vlan_port__ac_type_flag_set(ac_type_port_learn_change);
    }

    return BCM_E_NONE;
}


/*
 * The function deletes an AC and any objects that were associated with it according to the unique ac_type configuration.
 *
 * INPUT: 
 *   ac_type_flags - The unique attributes of the AC: WIDE_DATA / STATS / INGRESS_PROTECTION etc
 *   type_data - Additional data that a specific ac_type may require
 *   gport: AC gport
 */
int vlan_port__delete(int unit,
                      uint32 ac_type_flags,
                      ac_types_data_s *ac_type_data,
                      bcm_gport_t gport)
{
    char error_msg[200]={0,};
    int is_type_data_supplied = (ac_type_data == NULL) ? FALSE : TRUE;
    int is_dealloc_resources = FALSE;
    bcm_vlan_port_t vlan_port;
    bcm_failover_t failover_id;
    int is_allocated = FALSE;
    int is_primary = TRUE;

    /*
     * Retrieve the VLAN-Port structure in order to be able to delete additional resources
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vlan_port_id = gport;
    snprintf(error_msg, sizeof(error_msg), "for gport - %d", gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_find(unit, &vlan_port), error_msg);

    /*
     * Delete additional resources that were allocated according to the AC-Type 
     */
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_protection)) {
        if (!is_type_data_supplied || ac_type_data->is_alloc_additional_objects[ac_type_ingress_protection])
        {
            snprintf(error_msg, sizeof(error_msg), "failed for Ingress failover ID - %d", vlan_port.ingress_failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_destroy(unit, vlan_port.ingress_failover_id), error_msg);
        }
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_fec_protection)) {
        if (!is_type_data_supplied || ac_type_data->is_alloc_additional_objects[ac_type_fec_protection]) {
            bcm_if_t fec_id;
            bcm_l3_egress_t l3_egress;

            /* Retrieve the FEC in order to obtain the FEC Failover-ID for deletion */
            BCM_GPORT_FORWARD_GROUP_TO_L3_ITF_FEC(fec_id, vlan_port.failover_port_id);
            snprintf(error_msg, sizeof(error_msg), "failed for FEC-ID - %d", fec_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit, fec_id, &l3_egress), error_msg);

            /* Delete the FEC Failover ID */
            snprintf(error_msg, sizeof(error_msg), "failed for FEC failover ID - %d", l3_egress.failover_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_failover_destroy(unit, l3_egress.failover_id), error_msg);

            /* Delete the FECs - First the Primary and later the Secondary */
            snprintf(error_msg, sizeof(error_msg), "failed for Primary FEC - %d", fec_id);
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_destroy(unit, fec_id), error_msg);

            snprintf(error_msg, sizeof(error_msg), "failed for Seconadary FEC - %d", fec_id + 1);
            BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_destroy(unit, fec_id + 1), error_msg);
        }
    }
    if (ac_type_flags & vlan_port__ac_type_flag_set(ac_type_ingress_mc)) {
        if (!is_type_data_supplied || ac_type_data->is_alloc_additional_objects[ac_type_ingress_mc]) {
            snprintf(error_msg, sizeof(error_msg), "failed for group %d", DEFAULT_INGRESS_MC_GROUP);
            BCM_IF_ERROR_RETURN_MSG(bcm_multicast_destroy(unit, DEFAULT_INGRESS_MC_GROUP), error_msg);
        }
    }

    /* Destroy the VLAN-Port */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_destroy(unit, gport), "");

    return BCM_E_NONE;
}

