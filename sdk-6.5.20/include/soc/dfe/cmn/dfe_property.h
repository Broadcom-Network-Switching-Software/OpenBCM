/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE PROPERTY H
 */
 
#ifndef _SOC_DFE_PROPERTY_H_
#define _SOC_DFE_PROPERTY_H_




#include <soc/error.h>
#include <soc/drv.h>





typedef struct soc_dfe_property_info_s {
    char* str;
    char* def_str;
    int def_int;
    int def_type; 
} soc_dfe_property_info_t ;

typedef struct soc_dfe_property_str_enum_s {
    int enum_val;
    char *str;
} soc_dfe_property_str_enum_t ;





#define SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING 1
#define SOC_DFE_PROPERTY_DEFAULT_TYPE_INT 2
#define SOC_DFE_PROPERTY_UNAVAIL -1





int soc_dfe_property_suffix_num_get(int unit,int num, char* soc_property_name,const char* suffix, int force_use_default, int soc_prop_default);
int soc_dfe_property_suffix_num_get_only_suffix(int unit, int num, char* soc_propert_name, const char* suffix, int force_use_default, int soc_prop_default);
int soc_dfe_property_get(int unit,char* soc_property_name,int force_use_default, int soc_prop_default);
char* soc_dfe_property_get_str(int unit,char* soc_property_name,int force_use_default, char* soc_prop_default);
char* soc_dfe_property_port_get_str(int unit, char* soc_property_name, soc_port_t port, int force_use_default, char* soc_prop_default);
int soc_dfe_property_port_get(int unit, char* soc_property_name, soc_port_t port, int force_use_default, int soc_prop_default);

void soc_dfe_check_soc_property(int unit,char* soc_property_name,int* is_supported,soc_dfe_property_info_t* soc_property_info);


soc_error_t soc_dfe_property_str_to_enum(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, char *str_val, int *int_val);
soc_error_t soc_dfe_property_enum_to_str(int unit, char *soc_property_name, const soc_dfe_property_str_enum_t *property_info, int int_val, char **str_val);





extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_device_mode[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_multicast_mode[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_load_balancing_mode[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_cell_format[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fabric_tdm_priority_min[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_fe_mc_id_range[];

extern const soc_dfe_property_str_enum_t soc_dfe_property_str_enum_backplane_serdes_encoding[];

#endif 
