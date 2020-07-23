/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/stat.h>
#include <sal/types.h>
#include <soc/mem.h>

/*
 * Function:
 *      bcm_stat_group_mode_attr_selector_t_init
 * Description:
 *      Initialize an attribute selector of Stat Flex Group Mode
 * Parameters:
 *      attr_selector : (INOUT) Attribute Selector for Stat Flex Group Mode
 * Returns:
 *      NONE
 */
void bcm_stat_group_mode_attr_selector_t_init(
     bcm_stat_group_mode_attr_selector_t *attr_selector)
{
     if (attr_selector != NULL) {
         sal_memset(attr_selector, 0,
                    sizeof(bcm_stat_group_mode_attr_selector_t));
     }
     return;
}

/*
 * Function:
 *      bcm_stat_value_t_init
 * Description:
 *      Initialize a data structure bcm_stat_value_t
 *      void bcm_stat_value_t_init(bcm_stat_value_t *stat_value)
 * Parameters:
 *      stat_value : (INOUT) Pointer to bcm_stat_value_t structure to be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_value_t_init(
     bcm_stat_value_t *stat_value)
{
     if (stat_value != NULL) {
         sal_memset(stat_value, 0,
                    sizeof(bcm_stat_value_t));
     }
     return;
}

void bcm_stat_group_mode_id_config_t_init(
    bcm_stat_group_mode_id_config_t *stat_config)
{
     if (stat_config != NULL) {
         sal_memset(stat_config, 0,
                    sizeof(bcm_stat_group_mode_id_config_t));
     }
     return;
}

void bcm_stat_group_mode_hint_t_init(
    bcm_stat_group_mode_hint_t *stat_hint)
{
     if (stat_hint != NULL) {
         sal_memset(stat_hint, 0,
                    sizeof(bcm_stat_group_mode_hint_t));
     }
     return;
}

/*
 * Function:
 * bcm_stat_counter_input_data_t_init
 * Purpose:
 * Initialize a stat_counter_input_data object struct.
 * Parameters:
 * stat_input_data - pointer to stat_counter_input_data object struct.
 * Returns:
 * NONE
 */
void bcm_stat_counter_input_data_t_init(
    bcm_stat_counter_input_data_t *stat_input_data)
{
    if (stat_input_data != NULL) {
        sal_memset(stat_input_data, 0,
                  sizeof (bcm_stat_counter_input_data_t));
        stat_input_data->counter_source_gport = -1;
        stat_input_data->counter_source_id = -1;
    }
    return;
}

/*
 * Function:
 * bcm_stat_flex_pool_stat_info_t_init
 * Purpose:
 * Initialize a stat_flex_pool_stat_info object struct.
 * Parameters:
 * stat_flex_pool_stat_info - pointer to stat_flex_pool_stat_info object struct.
 * Returns:
 * NONE
 */
void bcm_stat_flex_pool_stat_info_t_init(
        bcm_stat_flex_pool_stat_info_t *stat_flex_pool_stat_info)
{
    if (stat_flex_pool_stat_info != NULL) {
        sal_memset(stat_flex_pool_stat_info, 0,
                    sizeof (bcm_stat_flex_pool_stat_info_t));
    }
    return;
}

/*
 * Function:
 * bcm_stat_engine_t_init
 * Purpose:
 * Initialize a stat_engine object struct.
 * Parameters:
 * stat_engine - pointer to stat_engine object struct.
 * Returns:
 * NONE
 */
void bcm_stat_engine_t_init(
        bcm_stat_engine_t *stat_engine)
{
    if (stat_engine != NULL) {
         sal_memset(stat_engine, 0,
                    sizeof(bcm_stat_engine_t));
    }
    return;
}

/*
* Function:
* bcm_stat_counter_database_t_init
* Purpose:
* Initialize a stat_counter_database object struct.
* Parameters:
* stat_engine - pointer to stat_counter_database object struct.
* Returns:
* NONE
*/
void bcm_stat_counter_database_t_init(
     bcm_stat_counter_database_t *stat_counter_database)
{
    if (stat_counter_database != NULL) {
          sal_memset(stat_counter_database, 0,
                 sizeof(bcm_stat_counter_database_t));
    }
     return;
}

 /*
 * Function:
 *      bcm_stat_counter_explicit_input_data_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_explicit_input_data_t structure
 * Parameters:
 *      stat_counter_explicit_input_data - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_explicit_input_data_t_init(
        bcm_stat_counter_explicit_input_data_t *stat_counter_explicit_input_data)
{
    if (stat_counter_explicit_input_data != NULL) {
         sal_memset(stat_counter_explicit_input_data, 0,
                    sizeof(bcm_stat_counter_explicit_input_data_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_eviction_t_init
 * Purpose:
 *      Initialize the bcm_stat_eviction_t structure
 * Parameters:
 *      stat_eviction - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_eviction_t_init(
     bcm_stat_eviction_t *stat_eviction)
{
    if (stat_eviction != NULL) {
         sal_memset(stat_eviction, 0,
                    sizeof(bcm_stat_eviction_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_set_map_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_set_map_t structure
 * Parameters:
 *      stat_counter_set_map - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_set_map_t_init(
        bcm_stat_counter_set_map_t *stat_counter_set_map)
{
    if (stat_counter_set_map != NULL) {
         sal_memset(stat_counter_set_map, 0,
                    sizeof(bcm_stat_counter_set_map_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_expansion_data_mapping_t_init
 * Purpose:
 *      Initialize the bcm_stat_expansion_data_mapping_t structure
 * Parameters:
 *      stat_expansion_data_mapping - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
 void bcm_stat_expansion_data_mapping_t_init(
        bcm_stat_expansion_data_mapping_t *stat_expansion_data_mapping)
{
    if (stat_expansion_data_mapping != NULL) {
         sal_memset(stat_expansion_data_mapping, 0,
                    sizeof(bcm_stat_expansion_data_mapping_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_interface_key_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_interface_key_t structure
 * Parameters:
 *      stat_counter_interface_key - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_interface_key_t_init(
        bcm_stat_counter_interface_key_t *stat_counter_interface_key)
{
    if (stat_counter_interface_key != NULL) {
         sal_memset(stat_counter_interface_key, 0,
                    sizeof(bcm_stat_counter_interface_key_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_expansion_select_t_init
 * Purpose:
 *      Initialize the bcm_stat_expansion_select_t structure
 * Parameters:
 *      stat_expansion_select - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_expansion_select_t_init(
        bcm_stat_expansion_select_t *stat_expansion_select)
{
    if (stat_expansion_select != NULL) {
         sal_memset(stat_expansion_select, 0,
                    sizeof(bcm_stat_expansion_select_t));
    }
    return;
}

 /*
 * Function:
 *      bcm_stat_counter_interface_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_interface_t structure
 * Parameters:
 *      stat_counter_interface - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_interface_t_init(
        bcm_stat_counter_interface_t *stat_counter_interface)
{
    if (stat_counter_interface != NULL) {
         sal_memset(stat_counter_interface, 0,
                    sizeof(bcm_stat_counter_interface_t));
    }
    return;
}

/*
 * Function:
 *      bcm_stat_counter_enable_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_enable_t structure
 * Parameters:
 *      stat_counter_enable - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_enable_t_init(
        bcm_stat_counter_enable_t *stat_counter_enable)
{
    if (stat_counter_enable != NULL) {
         sal_memset(stat_counter_enable, 0,
                    sizeof(bcm_stat_counter_enable_t));
    }
    return;
}

/*
 * Function:
 *      bcm_stat_engine_enable_t_init
 * Purpose:
 *      Initialize the bcm_stat_engine_enable_t structure
 * Parameters:
 *      stat_engine_enable - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
 void bcm_stat_counter_output_data_t_init(
        bcm_stat_counter_output_data_t *stat_counter_output_data)
{
    if (stat_counter_output_data != NULL) {
         sal_memset(stat_counter_output_data, 0,
                    sizeof(bcm_stat_counter_output_data_t));
    }
    return;
}

/*
 * Function:
 *      bcm_stat_counter_command_id_key_t_init
 * Purpose:
 *      Initialize the bcm_stat_counter_command_id_key_t structure
 * Parameters:
 *      stat_counter_command_id_key - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_counter_command_id_key_t_init(
    bcm_stat_counter_command_id_key_t *stat_counter_command_id_key)
{
    return;
}

/*
 * Function:
 *      bcm_stat_eviction_boundaries_t_init
 * Purpose:
 *      Initialize the bcm_stat_eviction_boundaries_t structure
 * Parameters:
 *      stat_eviction_boundaries - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_eviction_boundaries_t_init(
    bcm_stat_eviction_boundaries_t *stat_eviction_boundaries)
{
    return;
}

/*
 * Function:
 *      bcm_stat_custom_counter_info_t_init 
 * Purpose:
 *      Initialize the bcm_bcm_stat_custom_counter_info_t structure
 * Parameters:
 *      counter_info - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_custom_counter_info_t_init(
     bcm_stat_custom_counter_info_t *counter_info)
{
     if (counter_info != NULL) {
         sal_memset(counter_info, 0,
                    sizeof(bcm_stat_custom_counter_info_t));
     }
     return;
}

/*
 * Function:
 *      bcm_stat_pp_profile_info_t_init
 * Purpose:
 *      Initialize the bcm_stat_pp_profile_info_t structure
 * Parameters:
 *      pp_profile - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_pp_profile_info_t_init(
    bcm_stat_pp_profile_info_t *pp_profile)
{
    sal_memset(pp_profile, 0, sizeof(bcm_stat_pp_profile_info_t));
    pp_profile->meter_command_id = 0;
    return;
}

/*
 * Function:
 *      bcm_stat_pp_info_t_init
 * Purpose:
 *      Initialize the bcm_stat_info_t structure
 * Parameters:
 *      stat_info - Pointer to structure which should be initialized
 * Returns:
 *      NONE
 */
void bcm_stat_pp_info_t_init(
    bcm_stat_pp_info_t *stat_info)
{
    sal_memset(stat_info, 0, sizeof(bcm_stat_pp_info_t));
    return;
}
void
bcm_stat_pp_metadata_info_t_init(
        bcm_stat_pp_metadata_info_t * metadata_info)
{
    sal_memset(metadata_info, 0, sizeof(bcm_stat_pp_metadata_info_t));
    return;
}


