/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:       switch.c
 * Purpose:    Switch common APIs
 */

#include <soc/drv.h>
#include <bcm/switch.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <appl/diag/shell.h>
#include <bcm_int/control.h>

#if defined(BCM_PETRA_SUPPORT) && defined(CRASH_RECOVERY_SUPPORT)
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif

#if (defined(BCM_DPP_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#define SOC_SYNC_WHEN_NOT_DIRTY ((SOC_IS_DPP(unit) || SOC_IS_DFE(unit)) ? 1 : 0)
#else
#define SOC_SYNC_WHEN_NOT_DIRTY 0
#endif

/*
 * Function:
 *     bcm_switch_pkt_info_t_init
 * Description:
 *     Initialize an switch packet parameter strucutre
 * Parameters:
 *     pkt_info - pointer to switch packet parameter strucutre
 * Return: none
 */
void
bcm_switch_pkt_info_t_init(bcm_switch_pkt_info_t *pkt_info)
{
    sal_memset(pkt_info, 0, sizeof(bcm_switch_pkt_info_t));
}

/*
 * Function:
 *     bcm_switch_network_group_config_t_init
 * Description:
 *     Initialize a bcm_switch_network_group_config_t structure.
 * Parameters:
 *     config - pointer to switch network group config parameter strucutre
 * Return: none
 */
extern void bcm_switch_network_group_config_t_init(
    bcm_switch_network_group_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_switch_network_group_config_t));
}

/*
 * Function:
 *     bcm_switch_service_config_t_init
 * Description:
 *     Initialize a bcm_switch_service_config_t structure.
 * Parameters:
 *     config - pointer to service config parameter strucutre
 * Return: none
 */
extern void bcm_switch_service_config_t_init(
    bcm_switch_service_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_switch_service_config_t));
}

void bcm_switch_profile_mapping_t_init(bcm_switch_profile_mapping_t *profile_mapping)
{
    if (profile_mapping != NULL) {
        sal_memset(profile_mapping, 0, sizeof(bcm_switch_profile_mapping_t));
    }
}

/*
 * Function:
 *     bcm_switch_hash_entry_config_t_init
 * Description:
 *     Initialize a bcm_switch_reflector_data_t structure.
 * Parameters:
 *     data - pointer to data parameter structure
 * Return: none
 */

void
bcm_switch_reflector_data_t_init(bcm_switch_reflector_data_t *data)
{
    if (data != NULL) {
        sal_memset(data, 0, sizeof(bcm_switch_reflector_data_t));
    }
}

/*
 * Function:
 *     bcm_switch_hash_entry_config_t_init
 * Description:
 *     Initialize a bcm_switch_hash_entry_config_t structure.
 * Parameters:
 *     config - pointer to flex hash config parameter strucutre
 * Return: none
 */

void
bcm_switch_hash_entry_config_t_init(bcm_switch_hash_entry_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(bcm_switch_hash_entry_config_t));
    }
}

/* Initialize the structure bcm_switch_olp_l2_addr_t to default values */
void bcm_switch_olp_l2_addr_t_init(
    bcm_switch_olp_l2_addr_t *l2_addr)
{
    if (l2_addr != NULL) {
        sal_memset(l2_addr, 0, sizeof(bcm_switch_olp_l2_addr_t));
    }
}    

/*
 * Function:
 *     bcm_switch_agm_info_t_init
 * Description:
 *     Initialize an switch agm info parameter strucutre
 * Parameters:
 *     agm_info - pointer to switch agm info parameter strucutre
 * Return: none
 */
void
bcm_switch_agm_info_t_init(bcm_switch_agm_info_t *agm_info)
{
    sal_memset(agm_info, 0, sizeof(bcm_switch_agm_info_t));
}

/*
 * Function:
 *     bcm_switch_agm_stat_t_init
 * Description:
 *     Initialize an switch agm stat parameter strucutre
 * Parameters:
 *     agm_info - pointer to switch agm stat parameter strucutre
 * Return: none
 */
void
bcm_switch_agm_stat_t_init(bcm_switch_agm_stat_t *agm_stat)
{
    sal_memset(agm_stat, 0, sizeof(bcm_switch_agm_stat_t));
}

/*
 * Function:
 *     bcm_switch_match_config_info_t_init
 * Description:
 *     Initialize a bcm_switch_match_config_info_t structure.
 * Parameters:
 *     config_info - pointer to match config parameter strucutre
 * Return: none
 */
void bcm_switch_match_config_info_t_init(
    bcm_switch_match_config_info_t *config_info)
{
    if (config_info != NULL) {
        sal_memset(config_info, 0, sizeof(bcm_switch_match_config_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_match_control_info_t_init
 * Description:
 *     Initialize a bcm_switch_match_control_info_t structure.
 * Parameters:
 *     control_info - pointer to match control parameter strucutre
 * Return: none
 */
void bcm_switch_match_control_info_t_init(
    bcm_switch_match_control_info_t *control_info)
{
    if (control_info != NULL) {
        sal_memset(control_info, 0, sizeof(bcm_switch_match_control_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_encap_info_t_init
 * Description:
 *     Initialize a bcm_switch_encap_info_t structure.
 * Parameters:
 *     encap_info - pointer to switch encap parameter strucutre
 * Return: none
 */
void
bcm_switch_encap_info_t_init(bcm_switch_encap_info_t *encap_info)
{
    if (encap_info != NULL) {
        sal_memset(encap_info, 0, sizeof(bcm_switch_encap_info_t));
    }
}


/*
 * Function:
 *     bcm_switch_tpid_class_t_init
 * Description:
 *     Initialize a bcm_switch_tpid_class_t structure.
 * Parameters:
 *     tpid_class - pointer to tpid class parameter strucutre
 * Return: none
 */
void
bcm_switch_tpid_class_t_init(bcm_switch_tpid_class_t *tpid_class)
{
    if (tpid_class != NULL) {
        sal_memset(tpid_class, 0, sizeof(bcm_switch_tpid_class_t));
    }
}

/*
 * Function:
 *     bcm_switch_ser_log_info_t_init
 * Description:
 *     Initialize a bcm_switch_ser_log_info_t structure.
 * Parameters:
 *     info - pointer to ser log info parameter strucutre
 * Return: none
 */
void
bcm_switch_ser_log_info_t_init(bcm_switch_ser_log_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(bcm_switch_ser_log_info_t));
    }
}

/*
 * Function:
 *     bcm_switch_ser_error_stat_type_t_init
 * Description:
 *     Initialize a bcm_switch_ser_error_stat_type_t_init structure.
 * Parameters:
 *     stat_type - pointer to ser statistics type parameter strucutre
 * Return: none
 */
void
bcm_switch_ser_error_stat_type_t_init(bcm_switch_ser_error_stat_type_t *stat_type)
{
    if (stat_type != NULL) {
        sal_memset(stat_type, 0, sizeof(bcm_switch_ser_error_stat_type_t));
    }
}

/*
 * Function:
 *     bcm_switch_opaque_tag_config_t_init
 * Description:
 *     Initialize a bcm_switch_opaque_tag_params_t structure
 * Parameters:
 *     info - pointer to opaque tag parameters structure
 * Return: none
 */
void
bcm_switch_opaque_tag_config_t_init(bcm_switch_opaque_tag_params_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(bcm_switch_opaque_tag_params_t));
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT

/* Externs for bcm_<arch>_switch_control_get functions */
#define BCM_DLIST_ENTRY(_dtype) \
extern int bcm_##_dtype##_switch_control_set( \
    int unit, bcm_switch_control_t type, int arg);
#include <bcm_int/bcm_dlist.h>

/* Dispatch table for this function, which uses the externs above */
#define BCM_DLIST_ENTRY(_dtype)\
bcm_##_dtype##_switch_control_set,
static int (*_switch_control_set_disp[])(
    int unit, 
    bcm_switch_control_t type, 
    int arg) =
{
#include <bcm_int/bcm_dlist.h>
0
};

/* Externs for bcm_<arch>_switch_control_get functions */
#define BCM_DLIST_ENTRY(_dtype) \
extern int bcm_##_dtype##_switch_control_get( \
    int unit, bcm_switch_control_t type, int *arg);
#include <bcm_int/bcm_dlist.h>

/* Dispatch table for this function, which uses the externs above */
#define BCM_DLIST_ENTRY(_dtype)\
bcm_##_dtype##_switch_control_get,
static int (*_switch_control_get_disp[])(
    int unit, 
    bcm_switch_control_t type, 
    int *arg) =
{
#include <bcm_int/bcm_dlist.h>
0
};


/**
 * Function:
 *      _bcm_switch_state_sync
 * Purpose:
 *      Sync warm boot cache if necessary
 * Parameters:
 *      unit - (IN) Unit number.
 *      dtype - (IN) Dispatch index
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */

int
_bcm_switch_state_sync(int unit, bcm_dtype_t dtype)
{
    int rv = BCM_E_UNIT;
    int arg = 0;

    if (SOC_UNIT_VALID(unit)) {
        rv = _switch_control_get_disp[dtype](unit,
                                             bcmSwitchControlAutoSync, &arg);
        if (BCM_SUCCESS(rv) && (arg != 0)) {
            if (SOC_CONTROL(unit)->scache_dirty || SOC_SYNC_WHEN_NOT_DIRTY) {
                rv = _switch_control_set_disp[dtype](unit,
                                                     bcmSwitchControlSync, 1);
            }
        }
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcm_switch_obm_classifier_t_init
 * Purpose:
 *      Initialize the OBM  classifier struct
 * Parameters:
 *      switch_classifier - Pointer to the struct to be init'ed
 */

void
bcm_switch_obm_classifier_t_init(bcm_switch_obm_classifier_t *switch_classifier)
{
    if (switch_classifier != NULL) {
        sal_memset(switch_classifier, 0, sizeof(*switch_classifier));
    }
}
