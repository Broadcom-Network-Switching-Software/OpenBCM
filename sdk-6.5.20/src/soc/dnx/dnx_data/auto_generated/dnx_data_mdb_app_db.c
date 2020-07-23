

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mdb_app_db.h>



extern shr_error_e jr2_a0_data_mdb_app_db_attach(
    int unit);
extern shr_error_e jr2_b0_data_mdb_app_db_attach(
    int unit);
extern shr_error_e j2c_a0_data_mdb_app_db_attach(
    int unit);
extern shr_error_e q2a_a0_data_mdb_app_db_attach(
    int unit);
extern shr_error_e q2a_b0_data_mdb_app_db_attach(
    int unit);
extern shr_error_e j2p_a0_data_mdb_app_db_attach(
    int unit);



static shr_error_e
dnx_data_mdb_app_db_fields_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "fields";
    submodule_data->doc = "MDB modified fields";
    
    submodule_data->nof_features = _dnx_data_mdb_app_db_fields_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data mdb_app_db fields features");

    
    submodule_data->nof_defines = _dnx_data_mdb_app_db_fields_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data mdb_app_db fields defines");

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0].name = "numeric_mdb_field___0";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0].doc = "configures field size of numeric 0";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0___valid].name = "numeric_mdb_field___0___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0___valid].doc = "configures field validity of numeric 0";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1].name = "numeric_mdb_field___1";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1].doc = "configures field size of numeric 1";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1___valid].name = "numeric_mdb_field___1___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1___valid].doc = "configures field validity of numeric 1";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2].name = "numeric_mdb_field___2";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2].doc = "configures field size of numeric 2";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2___valid].name = "numeric_mdb_field___2___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2___valid].doc = "configures field validity of numeric 2";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3].name = "numeric_mdb_field___3";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3].doc = "configures field size of numeric 3";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3___valid].name = "numeric_mdb_field___3___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3___valid].doc = "configures field validity of numeric 3";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4].name = "numeric_mdb_field___4";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4].doc = "configures field size of numeric 4";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4___valid].name = "numeric_mdb_field___4___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4___valid].doc = "configures field validity of numeric 4";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5].name = "numeric_mdb_field___5";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5].doc = "configures field size of numeric 5";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5___valid].name = "numeric_mdb_field___5___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5___valid].doc = "configures field validity of numeric 5";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6].name = "numeric_mdb_field___6";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6].doc = "configures field size of numeric 6";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6___valid].name = "numeric_mdb_field___6___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6___valid].doc = "configures field validity of numeric 6";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7].name = "numeric_mdb_field___7";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7].doc = "configures field size of numeric 7";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7___valid].name = "numeric_mdb_field___7___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7___valid].doc = "configures field validity of numeric 7";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8].name = "numeric_mdb_field___8";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8].doc = "configures field size of numeric 8";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8___valid].name = "numeric_mdb_field___8___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8___valid].doc = "configures field validity of numeric 8";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9].name = "numeric_mdb_field___9";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9].doc = "configures field size of numeric 9";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9___valid].name = "numeric_mdb_field___9___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9___valid].doc = "configures field validity of numeric 9";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10].name = "numeric_mdb_field___10";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10].doc = "configures field size of numeric 10";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10___valid].name = "numeric_mdb_field___10___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10___valid].doc = "configures field validity of numeric 10";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11].name = "numeric_mdb_field___11";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11].doc = "configures field size of numeric 11";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11___valid].name = "numeric_mdb_field___11___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11___valid].doc = "configures field validity of numeric 11";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12].name = "numeric_mdb_field___12";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12].doc = "configures field size of numeric 12";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12___valid].name = "numeric_mdb_field___12___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12___valid].doc = "configures field validity of numeric 12";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13].name = "numeric_mdb_field___13";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13].doc = "configures field size of numeric 13";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13___valid].name = "numeric_mdb_field___13___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13___valid].doc = "configures field validity of numeric 13";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14].name = "numeric_mdb_field___14";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14].doc = "configures field size of numeric 14";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14___valid].name = "numeric_mdb_field___14___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14___valid].doc = "configures field validity of numeric 14";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15].name = "numeric_mdb_field___15";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15].doc = "configures field size of numeric 15";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15___valid].name = "numeric_mdb_field___15___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15___valid].doc = "configures field validity of numeric 15";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16].name = "numeric_mdb_field___16";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16].doc = "configures field size of numeric 16";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16___valid].name = "numeric_mdb_field___16___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16___valid].doc = "configures field validity of numeric 16";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17].name = "numeric_mdb_field___17";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17].doc = "configures field size of numeric 17";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17___valid].name = "numeric_mdb_field___17___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17___valid].doc = "configures field validity of numeric 17";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18].name = "numeric_mdb_field___18";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18].doc = "configures field size of numeric 18";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18___valid].name = "numeric_mdb_field___18___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18___valid].doc = "configures field validity of numeric 18";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19].name = "numeric_mdb_field___19";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19].doc = "configures field size of numeric 19";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19___valid].name = "numeric_mdb_field___19___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19___valid].doc = "configures field validity of numeric 19";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20].name = "numeric_mdb_field___20";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20].doc = "configures field size of numeric 20";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20___valid].name = "numeric_mdb_field___20___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20___valid].doc = "configures field validity of numeric 20";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21].name = "numeric_mdb_field___21";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21].doc = "configures field size of numeric 21";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21___valid].name = "numeric_mdb_field___21___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21___valid].doc = "configures field validity of numeric 21";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22].name = "numeric_mdb_field___22";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22].doc = "configures field size of numeric 22";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22___valid].name = "numeric_mdb_field___22___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22___valid].doc = "configures field validity of numeric 22";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23].name = "numeric_mdb_field___23";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23].doc = "configures field size of numeric 23";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23___valid].name = "numeric_mdb_field___23___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23___valid].doc = "configures field validity of numeric 23";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24].name = "numeric_mdb_field___24";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24].doc = "configures field size of numeric 24";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24___valid].name = "numeric_mdb_field___24___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24___valid].doc = "configures field validity of numeric 24";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25].name = "numeric_mdb_field___25";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25].doc = "configures field size of numeric 25";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25___valid].name = "numeric_mdb_field___25___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25___valid].doc = "configures field validity of numeric 25";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26].name = "numeric_mdb_field___26";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26].doc = "configures field size of numeric 26";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26___valid].name = "numeric_mdb_field___26___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26___valid].doc = "configures field validity of numeric 26";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27].name = "numeric_mdb_field___27";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27].doc = "configures field size of numeric 27";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27___valid].name = "numeric_mdb_field___27___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27___valid].doc = "configures field validity of numeric 27";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28].name = "numeric_mdb_field___28";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28].doc = "configures field size of numeric 28";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28___valid].name = "numeric_mdb_field___28___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28___valid].doc = "configures field validity of numeric 28";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29].name = "numeric_mdb_field___29";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29].doc = "configures field size of numeric 29";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29___valid].name = "numeric_mdb_field___29___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29___valid].doc = "configures field validity of numeric 29";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30].name = "numeric_mdb_field___30";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30].doc = "configures field size of numeric 30";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30___valid].name = "numeric_mdb_field___30___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30___valid].doc = "configures field validity of numeric 30";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31].name = "numeric_mdb_field___31";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31].doc = "configures field size of numeric 31";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31___valid].name = "numeric_mdb_field___31___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31___valid].doc = "configures field validity of numeric 31";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32].name = "numeric_mdb_field___32";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32].doc = "configures field size of numeric 32";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32___valid].name = "numeric_mdb_field___32___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32___valid].doc = "configures field validity of numeric 32";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33].name = "numeric_mdb_field___33";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33].doc = "configures field size of numeric 33";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33___valid].name = "numeric_mdb_field___33___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33___valid].doc = "configures field validity of numeric 33";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34].name = "numeric_mdb_field___34";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34].doc = "configures field size of numeric 34";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34___valid].name = "numeric_mdb_field___34___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34___valid].doc = "configures field validity of numeric 34";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35].name = "numeric_mdb_field___35";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35].doc = "configures field size of numeric 35";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35___valid].name = "numeric_mdb_field___35___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35___valid].doc = "configures field validity of numeric 35";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36].name = "numeric_mdb_field___36";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36].doc = "configures field size of numeric 36";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36___valid].name = "numeric_mdb_field___36___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36___valid].doc = "configures field validity of numeric 36";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37].name = "numeric_mdb_field___37";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37].doc = "configures field size of numeric 37";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37___valid].name = "numeric_mdb_field___37___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37___valid].doc = "configures field validity of numeric 37";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38].name = "numeric_mdb_field___38";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38].doc = "configures field size of numeric 38";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38___valid].name = "numeric_mdb_field___38___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38___valid].doc = "configures field validity of numeric 38";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39].name = "numeric_mdb_field___39";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39].doc = "configures field size of numeric 39";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39___valid].name = "numeric_mdb_field___39___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39___valid].doc = "configures field validity of numeric 39";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40].name = "numeric_mdb_field___40";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40].doc = "configures field size of numeric 40";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40___valid].name = "numeric_mdb_field___40___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40___valid].doc = "configures field validity of numeric 40";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41].name = "numeric_mdb_field___41";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41].doc = "configures field size of numeric 41";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41___valid].name = "numeric_mdb_field___41___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41___valid].doc = "configures field validity of numeric 41";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42].name = "numeric_mdb_field___42";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42].doc = "configures field size of numeric 42";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42___valid].name = "numeric_mdb_field___42___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42___valid].doc = "configures field validity of numeric 42";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43].name = "numeric_mdb_field___43";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43].doc = "configures field size of numeric 43";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43___valid].name = "numeric_mdb_field___43___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43___valid].doc = "configures field validity of numeric 43";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44].name = "numeric_mdb_field___44";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44].doc = "configures field size of numeric 44";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44___valid].name = "numeric_mdb_field___44___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44___valid].doc = "configures field validity of numeric 44";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45].name = "numeric_mdb_field___45";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45].doc = "configures field size of numeric 45";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45___valid].name = "numeric_mdb_field___45___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45___valid].doc = "configures field validity of numeric 45";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46].name = "numeric_mdb_field___46";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46].doc = "configures field size of numeric 46";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46___valid].name = "numeric_mdb_field___46___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46___valid].doc = "configures field validity of numeric 46";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47].name = "numeric_mdb_field___47";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47].doc = "configures field size of numeric 47";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47___valid].name = "numeric_mdb_field___47___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47___valid].doc = "configures field validity of numeric 47";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48].name = "numeric_mdb_field___48";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48].doc = "configures field size of numeric 48";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48___valid].name = "numeric_mdb_field___48___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48___valid].doc = "configures field validity of numeric 48";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49].name = "numeric_mdb_field___49";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49].doc = "configures field size of numeric 49";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49___valid].name = "numeric_mdb_field___49___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49___valid].doc = "configures field validity of numeric 49";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50].name = "numeric_mdb_field___50";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50].doc = "configures field size of numeric 50";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50___valid].name = "numeric_mdb_field___50___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50___valid].doc = "configures field validity of numeric 50";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51].name = "numeric_mdb_field___51";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51].doc = "configures field size of numeric 51";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51___valid].name = "numeric_mdb_field___51___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51___valid].doc = "configures field validity of numeric 51";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52].name = "numeric_mdb_field___52";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52].doc = "configures field size of numeric 52";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52___valid].name = "numeric_mdb_field___52___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52___valid].doc = "configures field validity of numeric 52";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53].name = "numeric_mdb_field___53";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53].doc = "configures field size of numeric 53";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53___valid].name = "numeric_mdb_field___53___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53___valid].doc = "configures field validity of numeric 53";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54].name = "numeric_mdb_field___54";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54].doc = "configures field size of numeric 54";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54___valid].name = "numeric_mdb_field___54___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54___valid].doc = "configures field validity of numeric 54";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55].name = "numeric_mdb_field___55";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55].doc = "configures field size of numeric 55";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55___valid].name = "numeric_mdb_field___55___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55___valid].doc = "configures field validity of numeric 55";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56].name = "numeric_mdb_field___56";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56].doc = "configures field size of numeric 56";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56___valid].name = "numeric_mdb_field___56___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56___valid].doc = "configures field validity of numeric 56";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57].name = "numeric_mdb_field___57";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57].doc = "configures field size of numeric 57";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57___valid].name = "numeric_mdb_field___57___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57___valid].doc = "configures field validity of numeric 57";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58].name = "numeric_mdb_field___58";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58].doc = "configures field size of numeric 58";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58___valid].name = "numeric_mdb_field___58___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58___valid].doc = "configures field validity of numeric 58";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59].name = "numeric_mdb_field___59";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59].doc = "configures field size of numeric 59";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59___valid].name = "numeric_mdb_field___59___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59___valid].doc = "configures field validity of numeric 59";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60].name = "numeric_mdb_field___60";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60].doc = "configures field size of numeric 60";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60___valid].name = "numeric_mdb_field___60___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60___valid].doc = "configures field validity of numeric 60";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61].name = "numeric_mdb_field___61";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61].doc = "configures field size of numeric 61";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61___valid].name = "numeric_mdb_field___61___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61___valid].doc = "configures field validity of numeric 61";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62].name = "numeric_mdb_field___62";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62].doc = "configures field size of numeric 62";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62___valid].name = "numeric_mdb_field___62___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62___valid].doc = "configures field validity of numeric 62";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63].name = "numeric_mdb_field___63";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63].doc = "configures field size of numeric 63";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63___valid].name = "numeric_mdb_field___63___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63___valid].doc = "configures field validity of numeric 63";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64].name = "numeric_mdb_field___64";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64].doc = "configures field size of numeric 64";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64___valid].name = "numeric_mdb_field___64___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64___valid].doc = "configures field validity of numeric 64";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65].name = "numeric_mdb_field___65";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65].doc = "configures field size of numeric 65";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65___valid].name = "numeric_mdb_field___65___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65___valid].doc = "configures field validity of numeric 65";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66].name = "numeric_mdb_field___66";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66].doc = "configures field size of numeric 66";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66___valid].name = "numeric_mdb_field___66___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66___valid].doc = "configures field validity of numeric 66";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67].name = "numeric_mdb_field___67";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67].doc = "configures field size of numeric 67";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67___valid].name = "numeric_mdb_field___67___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67___valid].doc = "configures field validity of numeric 67";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68].name = "numeric_mdb_field___68";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68].doc = "configures field size of numeric 68";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68___valid].name = "numeric_mdb_field___68___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68___valid].doc = "configures field validity of numeric 68";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69].name = "numeric_mdb_field___69";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69].doc = "configures field size of numeric 69";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69___valid].name = "numeric_mdb_field___69___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69___valid].doc = "configures field validity of numeric 69";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70].name = "numeric_mdb_field___70";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70].doc = "configures field size of numeric 70";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70___valid].name = "numeric_mdb_field___70___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70___valid].doc = "configures field validity of numeric 70";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71].name = "numeric_mdb_field___71";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71].doc = "configures field size of numeric 71";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71___valid].name = "numeric_mdb_field___71___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71___valid].doc = "configures field validity of numeric 71";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72].name = "numeric_mdb_field___72";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72].doc = "configures field size of numeric 72";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72___valid].name = "numeric_mdb_field___72___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72___valid].doc = "configures field validity of numeric 72";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73].name = "numeric_mdb_field___73";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73].doc = "configures field size of numeric 73";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73___valid].name = "numeric_mdb_field___73___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73___valid].doc = "configures field validity of numeric 73";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74].name = "numeric_mdb_field___74";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74].doc = "configures field size of numeric 74";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74___valid].name = "numeric_mdb_field___74___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74___valid].doc = "configures field validity of numeric 74";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75].name = "numeric_mdb_field___75";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75].doc = "configures field size of numeric 75";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75___valid].name = "numeric_mdb_field___75___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75___valid].doc = "configures field validity of numeric 75";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76].name = "numeric_mdb_field___76";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76].doc = "configures field size of numeric 76";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76___valid].name = "numeric_mdb_field___76___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76___valid].doc = "configures field validity of numeric 76";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77].name = "numeric_mdb_field___77";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77].doc = "configures field size of numeric 77";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77___valid].name = "numeric_mdb_field___77___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77___valid].doc = "configures field validity of numeric 77";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78].name = "numeric_mdb_field___78";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78].doc = "configures field size of numeric 78";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78___valid].name = "numeric_mdb_field___78___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78___valid].doc = "configures field validity of numeric 78";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79].name = "numeric_mdb_field___79";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79].doc = "configures field size of numeric 79";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79___valid].name = "numeric_mdb_field___79___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79___valid].doc = "configures field validity of numeric 79";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80].name = "numeric_mdb_field___80";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80].doc = "configures field size of numeric 80";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80___valid].name = "numeric_mdb_field___80___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80___valid].doc = "configures field validity of numeric 80";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81].name = "numeric_mdb_field___81";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81].doc = "configures field size of numeric 81";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81___valid].name = "numeric_mdb_field___81___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81___valid].doc = "configures field validity of numeric 81";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82].name = "numeric_mdb_field___82";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82].doc = "configures field size of numeric 82";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82___valid].name = "numeric_mdb_field___82___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82___valid].doc = "configures field validity of numeric 82";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83].name = "numeric_mdb_field___83";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83].doc = "configures field size of numeric 83";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83___valid].name = "numeric_mdb_field___83___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83___valid].doc = "configures field validity of numeric 83";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84].name = "numeric_mdb_field___84";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84].doc = "configures field size of numeric 84";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84___valid].name = "numeric_mdb_field___84___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84___valid].doc = "configures field validity of numeric 84";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85].name = "numeric_mdb_field___85";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85].doc = "configures field size of numeric 85";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85___valid].name = "numeric_mdb_field___85___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85___valid].doc = "configures field validity of numeric 85";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86].name = "numeric_mdb_field___86";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86].doc = "configures field size of numeric 86";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86___valid].name = "numeric_mdb_field___86___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86___valid].doc = "configures field validity of numeric 86";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87].name = "numeric_mdb_field___87";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87].doc = "configures field size of numeric 87";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87___valid].name = "numeric_mdb_field___87___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87___valid].doc = "configures field validity of numeric 87";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88].name = "numeric_mdb_field___88";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88].doc = "configures field size of numeric 88";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88___valid].name = "numeric_mdb_field___88___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88___valid].doc = "configures field validity of numeric 88";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89].name = "numeric_mdb_field___89";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89].doc = "configures field size of numeric 89";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89___valid].name = "numeric_mdb_field___89___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89___valid].doc = "configures field validity of numeric 89";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90].name = "numeric_mdb_field___90";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90].doc = "configures field size of numeric 90";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90___valid].name = "numeric_mdb_field___90___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90___valid].doc = "configures field validity of numeric 90";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91].name = "numeric_mdb_field___91";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91].doc = "configures field size of numeric 91";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91___valid].name = "numeric_mdb_field___91___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91___valid].doc = "configures field validity of numeric 91";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92].name = "numeric_mdb_field___92";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92].doc = "configures field size of numeric 92";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92___valid].name = "numeric_mdb_field___92___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92___valid].doc = "configures field validity of numeric 92";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93].name = "numeric_mdb_field___93";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93].doc = "configures field size of numeric 93";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93___valid].name = "numeric_mdb_field___93___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93___valid].doc = "configures field validity of numeric 93";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94].name = "numeric_mdb_field___94";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94].doc = "configures field size of numeric 94";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94___valid].name = "numeric_mdb_field___94___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94___valid].doc = "configures field validity of numeric 94";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95].name = "numeric_mdb_field___95";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95].doc = "configures field size of numeric 95";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95___valid].name = "numeric_mdb_field___95___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95___valid].doc = "configures field validity of numeric 95";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96].name = "numeric_mdb_field___96";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96].doc = "configures field size of numeric 96";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96___valid].name = "numeric_mdb_field___96___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96___valid].doc = "configures field validity of numeric 96";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97].name = "numeric_mdb_field___97";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97].doc = "configures field size of numeric 97";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97___valid].name = "numeric_mdb_field___97___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97___valid].doc = "configures field validity of numeric 97";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98].name = "numeric_mdb_field___98";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98].doc = "configures field size of numeric 98";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98___valid].name = "numeric_mdb_field___98___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98___valid].doc = "configures field validity of numeric 98";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99].name = "numeric_mdb_field___99";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99].doc = "configures field size of numeric 99";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99___valid].name = "numeric_mdb_field___99___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99___valid].doc = "configures field validity of numeric 99";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100].name = "numeric_mdb_field___100";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100].doc = "configures field size of numeric 100";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100___valid].name = "numeric_mdb_field___100___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100___valid].doc = "configures field validity of numeric 100";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101].name = "numeric_mdb_field___101";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101].doc = "configures field size of numeric 101";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101___valid].name = "numeric_mdb_field___101___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101___valid].doc = "configures field validity of numeric 101";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102].name = "numeric_mdb_field___102";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102].doc = "configures field size of numeric 102";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102___valid].name = "numeric_mdb_field___102___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102___valid].doc = "configures field validity of numeric 102";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103].name = "numeric_mdb_field___103";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103].doc = "configures field size of numeric 103";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103___valid].name = "numeric_mdb_field___103___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103___valid].doc = "configures field validity of numeric 103";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104].name = "numeric_mdb_field___104";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104].doc = "configures field size of numeric 104";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104___valid].name = "numeric_mdb_field___104___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104___valid].doc = "configures field validity of numeric 104";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105].name = "numeric_mdb_field___105";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105].doc = "configures field size of numeric 105";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105___valid].name = "numeric_mdb_field___105___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105___valid].doc = "configures field validity of numeric 105";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106].name = "numeric_mdb_field___106";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106].doc = "configures field size of numeric 106";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106___valid].name = "numeric_mdb_field___106___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106___valid].doc = "configures field validity of numeric 106";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107].name = "numeric_mdb_field___107";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107].doc = "configures field size of numeric 107";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107___valid].name = "numeric_mdb_field___107___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107___valid].doc = "configures field validity of numeric 107";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108].name = "numeric_mdb_field___108";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108].doc = "configures field size of numeric 108";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108___valid].name = "numeric_mdb_field___108___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108___valid].doc = "configures field validity of numeric 108";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109].name = "numeric_mdb_field___109";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109].doc = "configures field size of numeric 109";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109___valid].name = "numeric_mdb_field___109___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109___valid].doc = "configures field validity of numeric 109";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110].name = "numeric_mdb_field___110";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110].doc = "configures field size of numeric 110";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110___valid].name = "numeric_mdb_field___110___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110___valid].doc = "configures field validity of numeric 110";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111].name = "numeric_mdb_field___111";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111].doc = "configures field size of numeric 111";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111___valid].name = "numeric_mdb_field___111___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111___valid].doc = "configures field validity of numeric 111";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112].name = "numeric_mdb_field___112";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112].doc = "configures field size of numeric 112";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112___valid].name = "numeric_mdb_field___112___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112___valid].doc = "configures field validity of numeric 112";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113].name = "numeric_mdb_field___113";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113].doc = "configures field size of numeric 113";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113___valid].name = "numeric_mdb_field___113___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113___valid].doc = "configures field validity of numeric 113";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114].name = "numeric_mdb_field___114";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114].doc = "configures field size of numeric 114";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114___valid].name = "numeric_mdb_field___114___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114___valid].doc = "configures field validity of numeric 114";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115].name = "numeric_mdb_field___115";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115].doc = "configures field size of numeric 115";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115___valid].name = "numeric_mdb_field___115___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115___valid].doc = "configures field validity of numeric 115";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116].name = "numeric_mdb_field___116";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116].doc = "configures field size of numeric 116";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116___valid].name = "numeric_mdb_field___116___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116___valid].doc = "configures field validity of numeric 116";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117].name = "numeric_mdb_field___117";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117].doc = "configures field size of numeric 117";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117___valid].name = "numeric_mdb_field___117___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117___valid].doc = "configures field validity of numeric 117";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118].name = "numeric_mdb_field___118";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118].doc = "configures field size of numeric 118";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118___valid].name = "numeric_mdb_field___118___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118___valid].doc = "configures field validity of numeric 118";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119].name = "numeric_mdb_field___119";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119].doc = "configures field size of numeric 119";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119___valid].name = "numeric_mdb_field___119___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119___valid].doc = "configures field validity of numeric 119";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120].name = "numeric_mdb_field___120";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120].doc = "configures field size of numeric 120";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120___valid].name = "numeric_mdb_field___120___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120___valid].doc = "configures field validity of numeric 120";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121].name = "numeric_mdb_field___121";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121].doc = "configures field size of numeric 121";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121___valid].name = "numeric_mdb_field___121___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121___valid].doc = "configures field validity of numeric 121";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122].name = "numeric_mdb_field___122";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122].doc = "configures field size of numeric 122";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122___valid].name = "numeric_mdb_field___122___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122___valid].doc = "configures field validity of numeric 122";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123].name = "numeric_mdb_field___123";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123].doc = "configures field size of numeric 123";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123___valid].name = "numeric_mdb_field___123___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123___valid].doc = "configures field validity of numeric 123";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124].name = "numeric_mdb_field___124";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124].doc = "configures field size of numeric 124";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124___valid].name = "numeric_mdb_field___124___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124___valid].doc = "configures field validity of numeric 124";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125].name = "numeric_mdb_field___125";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125].doc = "configures field size of numeric 125";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125___valid].name = "numeric_mdb_field___125___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125___valid].doc = "configures field validity of numeric 125";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126].name = "numeric_mdb_field___126";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126].doc = "configures field size of numeric 126";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126___valid].name = "numeric_mdb_field___126___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126___valid].doc = "configures field validity of numeric 126";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127].name = "numeric_mdb_field___127";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127].doc = "configures field size of numeric 127";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127___valid].name = "numeric_mdb_field___127___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127___valid].doc = "configures field validity of numeric 127";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128].name = "numeric_mdb_field___128";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128].doc = "configures field size of numeric 128";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128___valid].name = "numeric_mdb_field___128___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128___valid].doc = "configures field validity of numeric 128";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129].name = "numeric_mdb_field___129";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129].doc = "configures field size of numeric 129";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129___valid].name = "numeric_mdb_field___129___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129___valid].doc = "configures field validity of numeric 129";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130].name = "numeric_mdb_field___130";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130].doc = "configures field size of numeric 130";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130___valid].name = "numeric_mdb_field___130___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130___valid].doc = "configures field validity of numeric 130";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131].name = "numeric_mdb_field___131";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131].doc = "configures field size of numeric 131";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131___valid].name = "numeric_mdb_field___131___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131___valid].doc = "configures field validity of numeric 131";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132].name = "numeric_mdb_field___132";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132].doc = "configures field size of numeric 132";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132___valid].name = "numeric_mdb_field___132___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132___valid].doc = "configures field validity of numeric 132";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133].name = "numeric_mdb_field___133";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133].doc = "configures field size of numeric 133";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133___valid].name = "numeric_mdb_field___133___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133___valid].doc = "configures field validity of numeric 133";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134].name = "numeric_mdb_field___134";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134].doc = "configures field size of numeric 134";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134___valid].name = "numeric_mdb_field___134___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134___valid].doc = "configures field validity of numeric 134";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135].name = "numeric_mdb_field___135";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135].doc = "configures field size of numeric 135";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135___valid].name = "numeric_mdb_field___135___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135___valid].doc = "configures field validity of numeric 135";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136].name = "numeric_mdb_field___136";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136].doc = "configures field size of numeric 136";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136___valid].name = "numeric_mdb_field___136___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136___valid].doc = "configures field validity of numeric 136";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137].name = "numeric_mdb_field___137";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137].doc = "configures field size of numeric 137";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137___valid].name = "numeric_mdb_field___137___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137___valid].doc = "configures field validity of numeric 137";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138].name = "numeric_mdb_field___138";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138].doc = "configures field size of numeric 138";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138___valid].name = "numeric_mdb_field___138___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138___valid].doc = "configures field validity of numeric 138";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139].name = "numeric_mdb_field___139";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139].doc = "configures field size of numeric 139";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139___valid].name = "numeric_mdb_field___139___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139___valid].doc = "configures field validity of numeric 139";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140].name = "numeric_mdb_field___140";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140].doc = "configures field size of numeric 140";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140___valid].name = "numeric_mdb_field___140___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140___valid].doc = "configures field validity of numeric 140";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141].name = "numeric_mdb_field___141";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141].doc = "configures field size of numeric 141";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141___valid].name = "numeric_mdb_field___141___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141___valid].doc = "configures field validity of numeric 141";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142].name = "numeric_mdb_field___142";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142].doc = "configures field size of numeric 142";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142___valid].name = "numeric_mdb_field___142___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142___valid].doc = "configures field validity of numeric 142";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143].name = "numeric_mdb_field___143";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143].doc = "configures field size of numeric 143";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143___valid].name = "numeric_mdb_field___143___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143___valid].doc = "configures field validity of numeric 143";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144].name = "numeric_mdb_field___144";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144].doc = "configures field size of numeric 144";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144___valid].name = "numeric_mdb_field___144___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144___valid].doc = "configures field validity of numeric 144";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145].name = "numeric_mdb_field___145";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145].doc = "configures field size of numeric 145";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145___valid].name = "numeric_mdb_field___145___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145___valid].doc = "configures field validity of numeric 145";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146].name = "numeric_mdb_field___146";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146].doc = "configures field size of numeric 146";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146___valid].name = "numeric_mdb_field___146___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146___valid].doc = "configures field validity of numeric 146";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147].name = "numeric_mdb_field___147";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147].doc = "configures field size of numeric 147";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147___valid].name = "numeric_mdb_field___147___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147___valid].doc = "configures field validity of numeric 147";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148].name = "numeric_mdb_field___148";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148].doc = "configures field size of numeric 148";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148___valid].name = "numeric_mdb_field___148___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148___valid].doc = "configures field validity of numeric 148";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149].name = "numeric_mdb_field___149";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149].doc = "configures field size of numeric 149";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149___valid].name = "numeric_mdb_field___149___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149___valid].doc = "configures field validity of numeric 149";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150].name = "numeric_mdb_field___150";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150].doc = "configures field size of numeric 150";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150___valid].name = "numeric_mdb_field___150___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150___valid].doc = "configures field validity of numeric 150";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151].name = "numeric_mdb_field___151";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151].doc = "configures field size of numeric 151";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151___valid].name = "numeric_mdb_field___151___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151___valid].doc = "configures field validity of numeric 151";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152].name = "numeric_mdb_field___152";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152].doc = "configures field size of numeric 152";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152___valid].name = "numeric_mdb_field___152___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152___valid].doc = "configures field validity of numeric 152";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153].name = "numeric_mdb_field___153";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153].doc = "configures field size of numeric 153";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153___valid].name = "numeric_mdb_field___153___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153___valid].doc = "configures field validity of numeric 153";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154].name = "numeric_mdb_field___154";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154].doc = "configures field size of numeric 154";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154___valid].name = "numeric_mdb_field___154___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154___valid].doc = "configures field validity of numeric 154";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155].name = "numeric_mdb_field___155";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155].doc = "configures field size of numeric 155";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155___valid].name = "numeric_mdb_field___155___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155___valid].doc = "configures field validity of numeric 155";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156].name = "numeric_mdb_field___156";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156].doc = "configures field size of numeric 156";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156___valid].name = "numeric_mdb_field___156___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156___valid].doc = "configures field validity of numeric 156";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157].name = "numeric_mdb_field___157";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157].doc = "configures field size of numeric 157";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157___valid].name = "numeric_mdb_field___157___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157___valid].doc = "configures field validity of numeric 157";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158].name = "numeric_mdb_field___158";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158].doc = "configures field size of numeric 158";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158___valid].name = "numeric_mdb_field___158___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158___valid].doc = "configures field validity of numeric 158";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159].name = "numeric_mdb_field___159";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159].doc = "configures field size of numeric 159";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159___valid].name = "numeric_mdb_field___159___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159___valid].doc = "configures field validity of numeric 159";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160].name = "numeric_mdb_field___160";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160].doc = "configures field size of numeric 160";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160___valid].name = "numeric_mdb_field___160___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160___valid].doc = "configures field validity of numeric 160";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161].name = "numeric_mdb_field___161";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161].doc = "configures field size of numeric 161";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161___valid].name = "numeric_mdb_field___161___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161___valid].doc = "configures field validity of numeric 161";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162].name = "numeric_mdb_field___162";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162].doc = "configures field size of numeric 162";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162___valid].name = "numeric_mdb_field___162___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162___valid].doc = "configures field validity of numeric 162";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163].name = "numeric_mdb_field___163";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163].doc = "configures field size of numeric 163";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163___valid].name = "numeric_mdb_field___163___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163___valid].doc = "configures field validity of numeric 163";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164].name = "numeric_mdb_field___164";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164].doc = "configures field size of numeric 164";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164___valid].name = "numeric_mdb_field___164___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164___valid].doc = "configures field validity of numeric 164";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165].name = "numeric_mdb_field___165";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165].doc = "configures field size of numeric 165";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165___valid].name = "numeric_mdb_field___165___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165___valid].doc = "configures field validity of numeric 165";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166].name = "numeric_mdb_field___166";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166].doc = "configures field size of numeric 166";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166___valid].name = "numeric_mdb_field___166___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166___valid].doc = "configures field validity of numeric 166";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167].name = "numeric_mdb_field___167";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167].doc = "configures field size of numeric 167";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167___valid].name = "numeric_mdb_field___167___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167___valid].doc = "configures field validity of numeric 167";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168].name = "numeric_mdb_field___168";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168].doc = "configures field size of numeric 168";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168___valid].name = "numeric_mdb_field___168___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168___valid].doc = "configures field validity of numeric 168";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169].name = "numeric_mdb_field___169";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169].doc = "configures field size of numeric 169";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169___valid].name = "numeric_mdb_field___169___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169___valid].doc = "configures field validity of numeric 169";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170].name = "numeric_mdb_field___170";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170].doc = "configures field size of numeric 170";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170___valid].name = "numeric_mdb_field___170___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170___valid].doc = "configures field validity of numeric 170";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171].name = "numeric_mdb_field___171";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171].doc = "configures field size of numeric 171";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171___valid].name = "numeric_mdb_field___171___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171___valid].doc = "configures field validity of numeric 171";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172].name = "numeric_mdb_field___172";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172].doc = "configures field size of numeric 172";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172___valid].name = "numeric_mdb_field___172___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172___valid].doc = "configures field validity of numeric 172";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173].name = "numeric_mdb_field___173";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173].doc = "configures field size of numeric 173";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173___valid].name = "numeric_mdb_field___173___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173___valid].doc = "configures field validity of numeric 173";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174].name = "numeric_mdb_field___174";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174].doc = "configures field size of numeric 174";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174___valid].name = "numeric_mdb_field___174___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174___valid].doc = "configures field validity of numeric 174";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175].name = "numeric_mdb_field___175";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175].doc = "configures field size of numeric 175";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175___valid].name = "numeric_mdb_field___175___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175___valid].doc = "configures field validity of numeric 175";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176].name = "numeric_mdb_field___176";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176].doc = "configures field size of numeric 176";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176___valid].name = "numeric_mdb_field___176___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176___valid].doc = "configures field validity of numeric 176";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177].name = "numeric_mdb_field___177";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177].doc = "configures field size of numeric 177";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177___valid].name = "numeric_mdb_field___177___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177___valid].doc = "configures field validity of numeric 177";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178].name = "numeric_mdb_field___178";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178].doc = "configures field size of numeric 178";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178___valid].name = "numeric_mdb_field___178___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178___valid].doc = "configures field validity of numeric 178";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179].name = "numeric_mdb_field___179";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179].doc = "configures field size of numeric 179";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179___valid].name = "numeric_mdb_field___179___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179___valid].doc = "configures field validity of numeric 179";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180].name = "numeric_mdb_field___180";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180].doc = "configures field size of numeric 180";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180___valid].name = "numeric_mdb_field___180___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180___valid].doc = "configures field validity of numeric 180";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181].name = "numeric_mdb_field___181";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181].doc = "configures field size of numeric 181";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181___valid].name = "numeric_mdb_field___181___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181___valid].doc = "configures field validity of numeric 181";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182].name = "numeric_mdb_field___182";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182].doc = "configures field size of numeric 182";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182___valid].name = "numeric_mdb_field___182___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182___valid].doc = "configures field validity of numeric 182";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183].name = "numeric_mdb_field___183";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183].doc = "configures field size of numeric 183";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183___valid].name = "numeric_mdb_field___183___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183___valid].doc = "configures field validity of numeric 183";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184].name = "numeric_mdb_field___184";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184].doc = "configures field size of numeric 184";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184___valid].name = "numeric_mdb_field___184___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184___valid].doc = "configures field validity of numeric 184";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185].name = "numeric_mdb_field___185";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185].doc = "configures field size of numeric 185";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185___valid].name = "numeric_mdb_field___185___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185___valid].doc = "configures field validity of numeric 185";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186].name = "numeric_mdb_field___186";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186].doc = "configures field size of numeric 186";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186___valid].name = "numeric_mdb_field___186___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186___valid].doc = "configures field validity of numeric 186";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187].name = "numeric_mdb_field___187";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187].doc = "configures field size of numeric 187";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187___valid].name = "numeric_mdb_field___187___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187___valid].doc = "configures field validity of numeric 187";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188].name = "numeric_mdb_field___188";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188].doc = "configures field size of numeric 188";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188___valid].name = "numeric_mdb_field___188___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188___valid].doc = "configures field validity of numeric 188";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189].name = "numeric_mdb_field___189";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189].doc = "configures field size of numeric 189";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189___valid].name = "numeric_mdb_field___189___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189___valid].doc = "configures field validity of numeric 189";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190].name = "numeric_mdb_field___190";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190].doc = "configures field size of numeric 190";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190___valid].name = "numeric_mdb_field___190___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190___valid].doc = "configures field validity of numeric 190";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191].name = "numeric_mdb_field___191";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191].doc = "configures field size of numeric 191";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191___valid].name = "numeric_mdb_field___191___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191___valid].doc = "configures field validity of numeric 191";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192].name = "numeric_mdb_field___192";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192].doc = "configures field size of numeric 192";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192___valid].name = "numeric_mdb_field___192___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192___valid].doc = "configures field validity of numeric 192";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193].name = "numeric_mdb_field___193";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193].doc = "configures field size of numeric 193";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193___valid].name = "numeric_mdb_field___193___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193___valid].doc = "configures field validity of numeric 193";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194].name = "numeric_mdb_field___194";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194].doc = "configures field size of numeric 194";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194___valid].name = "numeric_mdb_field___194___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194___valid].doc = "configures field validity of numeric 194";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195].name = "numeric_mdb_field___195";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195].doc = "configures field size of numeric 195";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195___valid].name = "numeric_mdb_field___195___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195___valid].doc = "configures field validity of numeric 195";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196].name = "numeric_mdb_field___196";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196].doc = "configures field size of numeric 196";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196___valid].name = "numeric_mdb_field___196___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196___valid].doc = "configures field validity of numeric 196";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197].name = "numeric_mdb_field___197";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197].doc = "configures field size of numeric 197";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197___valid].name = "numeric_mdb_field___197___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197___valid].doc = "configures field validity of numeric 197";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198].name = "numeric_mdb_field___198";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198].doc = "configures field size of numeric 198";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198___valid].name = "numeric_mdb_field___198___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198___valid].doc = "configures field validity of numeric 198";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199].name = "numeric_mdb_field___199";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199].doc = "configures field size of numeric 199";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199___valid].name = "numeric_mdb_field___199___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199___valid].doc = "configures field validity of numeric 199";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200].name = "numeric_mdb_field___200";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200].doc = "configures field size of numeric 200";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200___valid].name = "numeric_mdb_field___200___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200___valid].doc = "configures field validity of numeric 200";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201].name = "numeric_mdb_field___201";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201].doc = "configures field size of numeric 201";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201___valid].name = "numeric_mdb_field___201___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201___valid].doc = "configures field validity of numeric 201";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202].name = "numeric_mdb_field___202";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202].doc = "configures field size of numeric 202";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202___valid].name = "numeric_mdb_field___202___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202___valid].doc = "configures field validity of numeric 202";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203].name = "numeric_mdb_field___203";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203].doc = "configures field size of numeric 203";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203___valid].name = "numeric_mdb_field___203___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203___valid].doc = "configures field validity of numeric 203";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204].name = "numeric_mdb_field___204";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204].doc = "configures field size of numeric 204";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204___valid].name = "numeric_mdb_field___204___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204___valid].doc = "configures field validity of numeric 204";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205].name = "numeric_mdb_field___205";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205].doc = "configures field size of numeric 205";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205___valid].name = "numeric_mdb_field___205___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205___valid].doc = "configures field validity of numeric 205";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206].name = "numeric_mdb_field___206";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206].doc = "configures field size of numeric 206";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206___valid].name = "numeric_mdb_field___206___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206___valid].doc = "configures field validity of numeric 206";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207].name = "numeric_mdb_field___207";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207].doc = "configures field size of numeric 207";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207___valid].name = "numeric_mdb_field___207___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207___valid].doc = "configures field validity of numeric 207";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208].name = "numeric_mdb_field___208";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208].doc = "configures field size of numeric 208";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208___valid].name = "numeric_mdb_field___208___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208___valid].doc = "configures field validity of numeric 208";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209].name = "numeric_mdb_field___209";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209].doc = "configures field size of numeric 209";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209___valid].name = "numeric_mdb_field___209___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209___valid].doc = "configures field validity of numeric 209";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210].name = "numeric_mdb_field___210";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210].doc = "configures field size of numeric 210";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210___valid].name = "numeric_mdb_field___210___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210___valid].doc = "configures field validity of numeric 210";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211].name = "numeric_mdb_field___211";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211].doc = "configures field size of numeric 211";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211___valid].name = "numeric_mdb_field___211___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211___valid].doc = "configures field validity of numeric 211";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212].name = "numeric_mdb_field___212";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212].doc = "configures field size of numeric 212";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212___valid].name = "numeric_mdb_field___212___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212___valid].doc = "configures field validity of numeric 212";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213].name = "numeric_mdb_field___213";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213].doc = "configures field size of numeric 213";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213___valid].name = "numeric_mdb_field___213___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213___valid].doc = "configures field validity of numeric 213";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214].name = "numeric_mdb_field___214";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214].doc = "configures field size of numeric 214";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214___valid].name = "numeric_mdb_field___214___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214___valid].doc = "configures field validity of numeric 214";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215].name = "numeric_mdb_field___215";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215].doc = "configures field size of numeric 215";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215___valid].name = "numeric_mdb_field___215___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215___valid].doc = "configures field validity of numeric 215";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216].name = "numeric_mdb_field___216";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216].doc = "configures field size of numeric 216";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216___valid].name = "numeric_mdb_field___216___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216___valid].doc = "configures field validity of numeric 216";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217].name = "numeric_mdb_field___217";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217].doc = "configures field size of numeric 217";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217___valid].name = "numeric_mdb_field___217___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217___valid].doc = "configures field validity of numeric 217";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218].name = "numeric_mdb_field___218";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218].doc = "configures field size of numeric 218";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218___valid].name = "numeric_mdb_field___218___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218___valid].doc = "configures field validity of numeric 218";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219].name = "numeric_mdb_field___219";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219].doc = "configures field size of numeric 219";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219___valid].name = "numeric_mdb_field___219___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219___valid].doc = "configures field validity of numeric 219";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220].name = "numeric_mdb_field___220";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220].doc = "configures field size of numeric 220";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220___valid].name = "numeric_mdb_field___220___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220___valid].doc = "configures field validity of numeric 220";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221].name = "numeric_mdb_field___221";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221].doc = "configures field size of numeric 221";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221___valid].name = "numeric_mdb_field___221___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221___valid].doc = "configures field validity of numeric 221";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222].name = "numeric_mdb_field___222";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222].doc = "configures field size of numeric 222";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222___valid].name = "numeric_mdb_field___222___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222___valid].doc = "configures field validity of numeric 222";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223].name = "numeric_mdb_field___223";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223].doc = "configures field size of numeric 223";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223___valid].name = "numeric_mdb_field___223___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223___valid].doc = "configures field validity of numeric 223";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224].name = "numeric_mdb_field___224";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224].doc = "configures field size of numeric 224";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224___valid].name = "numeric_mdb_field___224___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224___valid].doc = "configures field validity of numeric 224";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225].name = "numeric_mdb_field___225";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225].doc = "configures field size of numeric 225";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225___valid].name = "numeric_mdb_field___225___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225___valid].doc = "configures field validity of numeric 225";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226].name = "numeric_mdb_field___226";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226].doc = "configures field size of numeric 226";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226___valid].name = "numeric_mdb_field___226___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226___valid].doc = "configures field validity of numeric 226";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227].name = "numeric_mdb_field___227";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227].doc = "configures field size of numeric 227";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227___valid].name = "numeric_mdb_field___227___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227___valid].doc = "configures field validity of numeric 227";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228].name = "numeric_mdb_field___228";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228].doc = "configures field size of numeric 228";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228___valid].name = "numeric_mdb_field___228___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228___valid].doc = "configures field validity of numeric 228";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229].name = "numeric_mdb_field___229";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229].doc = "configures field size of numeric 229";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229___valid].name = "numeric_mdb_field___229___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229___valid].doc = "configures field validity of numeric 229";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230].name = "numeric_mdb_field___230";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230].doc = "configures field size of numeric 230";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230___valid].name = "numeric_mdb_field___230___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230___valid].doc = "configures field validity of numeric 230";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231].name = "numeric_mdb_field___231";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231].doc = "configures field size of numeric 231";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231___valid].name = "numeric_mdb_field___231___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231___valid].doc = "configures field validity of numeric 231";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232].name = "numeric_mdb_field___232";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232].doc = "configures field size of numeric 232";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232___valid].name = "numeric_mdb_field___232___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232___valid].doc = "configures field validity of numeric 232";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233].name = "numeric_mdb_field___233";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233].doc = "configures field size of numeric 233";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233___valid].name = "numeric_mdb_field___233___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233___valid].doc = "configures field validity of numeric 233";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234].name = "numeric_mdb_field___234";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234].doc = "configures field size of numeric 234";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234___valid].name = "numeric_mdb_field___234___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234___valid].doc = "configures field validity of numeric 234";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235].name = "numeric_mdb_field___235";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235].doc = "configures field size of numeric 235";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235___valid].name = "numeric_mdb_field___235___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235___valid].doc = "configures field validity of numeric 235";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236].name = "numeric_mdb_field___236";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236].doc = "configures field size of numeric 236";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236___valid].name = "numeric_mdb_field___236___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236___valid].doc = "configures field validity of numeric 236";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237].name = "numeric_mdb_field___237";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237].doc = "configures field size of numeric 237";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237___valid].name = "numeric_mdb_field___237___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237___valid].doc = "configures field validity of numeric 237";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238].name = "numeric_mdb_field___238";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238].doc = "configures field size of numeric 238";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238___valid].name = "numeric_mdb_field___238___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238___valid].doc = "configures field validity of numeric 238";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239].name = "numeric_mdb_field___239";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239].doc = "configures field size of numeric 239";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239___valid].name = "numeric_mdb_field___239___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239___valid].doc = "configures field validity of numeric 239";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240].name = "numeric_mdb_field___240";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240].doc = "configures field size of numeric 240";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240___valid].name = "numeric_mdb_field___240___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240___valid].doc = "configures field validity of numeric 240";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241].name = "numeric_mdb_field___241";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241].doc = "configures field size of numeric 241";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241___valid].name = "numeric_mdb_field___241___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241___valid].doc = "configures field validity of numeric 241";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242].name = "numeric_mdb_field___242";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242].doc = "configures field size of numeric 242";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242___valid].name = "numeric_mdb_field___242___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242___valid].doc = "configures field validity of numeric 242";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243].name = "numeric_mdb_field___243";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243].doc = "configures field size of numeric 243";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243___valid].name = "numeric_mdb_field___243___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243___valid].doc = "configures field validity of numeric 243";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244].name = "numeric_mdb_field___244";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244].doc = "configures field size of numeric 244";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244___valid].name = "numeric_mdb_field___244___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244___valid].doc = "configures field validity of numeric 244";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245].name = "numeric_mdb_field___245";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245].doc = "configures field size of numeric 245";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245___valid].name = "numeric_mdb_field___245___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245___valid].doc = "configures field validity of numeric 245";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246].name = "numeric_mdb_field___246";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246].doc = "configures field size of numeric 246";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246___valid].name = "numeric_mdb_field___246___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246___valid].doc = "configures field validity of numeric 246";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247].name = "numeric_mdb_field___247";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247].doc = "configures field size of numeric 247";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247___valid].name = "numeric_mdb_field___247___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247___valid].doc = "configures field validity of numeric 247";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248].name = "numeric_mdb_field___248";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248].doc = "configures field size of numeric 248";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248___valid].name = "numeric_mdb_field___248___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248___valid].doc = "configures field validity of numeric 248";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249].name = "numeric_mdb_field___249";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249].doc = "configures field size of numeric 249";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249___valid].name = "numeric_mdb_field___249___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249___valid].doc = "configures field validity of numeric 249";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250].name = "numeric_mdb_field___250";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250].doc = "configures field size of numeric 250";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250___valid].name = "numeric_mdb_field___250___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250___valid].doc = "configures field validity of numeric 250";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251].name = "numeric_mdb_field___251";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251].doc = "configures field size of numeric 251";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251___valid].name = "numeric_mdb_field___251___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251___valid].doc = "configures field validity of numeric 251";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252].name = "numeric_mdb_field___252";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252].doc = "configures field size of numeric 252";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252___valid].name = "numeric_mdb_field___252___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252___valid].doc = "configures field validity of numeric 252";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253].name = "numeric_mdb_field___253";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253].doc = "configures field size of numeric 253";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253___valid].name = "numeric_mdb_field___253___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253___valid].doc = "configures field validity of numeric 253";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254].name = "numeric_mdb_field___254";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254].doc = "configures field size of numeric 254";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254___valid].name = "numeric_mdb_field___254___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254___valid].doc = "configures field validity of numeric 254";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255].name = "numeric_mdb_field___255";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255].doc = "configures field size of numeric 255";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255___valid].name = "numeric_mdb_field___255___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255___valid].doc = "configures field validity of numeric 255";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256].name = "numeric_mdb_field___256";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256].doc = "configures field size of numeric 256";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256___valid].name = "numeric_mdb_field___256___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256___valid].doc = "configures field validity of numeric 256";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257].name = "numeric_mdb_field___257";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257].doc = "configures field size of numeric 257";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257___valid].name = "numeric_mdb_field___257___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257___valid].doc = "configures field validity of numeric 257";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258].name = "numeric_mdb_field___258";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258].doc = "configures field size of numeric 258";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258___valid].name = "numeric_mdb_field___258___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258___valid].doc = "configures field validity of numeric 258";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259].name = "numeric_mdb_field___259";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259].doc = "configures field size of numeric 259";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259___valid].name = "numeric_mdb_field___259___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259___valid].doc = "configures field validity of numeric 259";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260].name = "numeric_mdb_field___260";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260].doc = "configures field size of numeric 260";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260___valid].name = "numeric_mdb_field___260___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260___valid].doc = "configures field validity of numeric 260";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261].name = "numeric_mdb_field___261";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261].doc = "configures field size of numeric 261";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261___valid].name = "numeric_mdb_field___261___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261___valid].doc = "configures field validity of numeric 261";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262].name = "numeric_mdb_field___262";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262].doc = "configures field size of numeric 262";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262___valid].name = "numeric_mdb_field___262___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262___valid].doc = "configures field validity of numeric 262";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263].name = "numeric_mdb_field___263";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263].doc = "configures field size of numeric 263";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263___valid].name = "numeric_mdb_field___263___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263___valid].doc = "configures field validity of numeric 263";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264].name = "numeric_mdb_field___264";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264].doc = "configures field size of numeric 264";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264___valid].name = "numeric_mdb_field___264___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264___valid].doc = "configures field validity of numeric 264";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265].name = "numeric_mdb_field___265";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265].doc = "configures field size of numeric 265";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265___valid].name = "numeric_mdb_field___265___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265___valid].doc = "configures field validity of numeric 265";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266].name = "numeric_mdb_field___266";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266].doc = "configures field size of numeric 266";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266___valid].name = "numeric_mdb_field___266___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266___valid].doc = "configures field validity of numeric 266";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267].name = "numeric_mdb_field___267";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267].doc = "configures field size of numeric 267";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267___valid].name = "numeric_mdb_field___267___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267___valid].doc = "configures field validity of numeric 267";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268].name = "numeric_mdb_field___268";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268].doc = "configures field size of numeric 268";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268___valid].name = "numeric_mdb_field___268___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268___valid].doc = "configures field validity of numeric 268";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269].name = "numeric_mdb_field___269";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269].doc = "configures field size of numeric 269";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269___valid].name = "numeric_mdb_field___269___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269___valid].doc = "configures field validity of numeric 269";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270].name = "numeric_mdb_field___270";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270].doc = "configures field size of numeric 270";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270___valid].name = "numeric_mdb_field___270___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270___valid].doc = "configures field validity of numeric 270";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271].name = "numeric_mdb_field___271";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271].doc = "configures field size of numeric 271";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271___valid].name = "numeric_mdb_field___271___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271___valid].doc = "configures field validity of numeric 271";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272].name = "numeric_mdb_field___272";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272].doc = "configures field size of numeric 272";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272___valid].name = "numeric_mdb_field___272___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272___valid].doc = "configures field validity of numeric 272";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273].name = "numeric_mdb_field___273";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273].doc = "configures field size of numeric 273";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273___valid].name = "numeric_mdb_field___273___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273___valid].doc = "configures field validity of numeric 273";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274].name = "numeric_mdb_field___274";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274].doc = "configures field size of numeric 274";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274___valid].name = "numeric_mdb_field___274___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274___valid].doc = "configures field validity of numeric 274";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275].name = "numeric_mdb_field___275";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275].doc = "configures field size of numeric 275";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275___valid].name = "numeric_mdb_field___275___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275___valid].doc = "configures field validity of numeric 275";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276].name = "numeric_mdb_field___276";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276].doc = "configures field size of numeric 276";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276___valid].name = "numeric_mdb_field___276___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276___valid].doc = "configures field validity of numeric 276";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277].name = "numeric_mdb_field___277";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277].doc = "configures field size of numeric 277";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277___valid].name = "numeric_mdb_field___277___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277___valid].doc = "configures field validity of numeric 277";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278].name = "numeric_mdb_field___278";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278].doc = "configures field size of numeric 278";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278___valid].name = "numeric_mdb_field___278___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278___valid].doc = "configures field validity of numeric 278";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279].name = "numeric_mdb_field___279";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279].doc = "configures field size of numeric 279";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279___valid].name = "numeric_mdb_field___279___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279___valid].doc = "configures field validity of numeric 279";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280].name = "numeric_mdb_field___280";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280].doc = "configures field size of numeric 280";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280___valid].name = "numeric_mdb_field___280___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280___valid].doc = "configures field validity of numeric 280";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281].name = "numeric_mdb_field___281";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281].doc = "configures field size of numeric 281";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281___valid].name = "numeric_mdb_field___281___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281___valid].doc = "configures field validity of numeric 281";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282].name = "numeric_mdb_field___282";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282].doc = "configures field size of numeric 282";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282___valid].name = "numeric_mdb_field___282___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282___valid].doc = "configures field validity of numeric 282";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283].name = "numeric_mdb_field___283";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283].doc = "configures field size of numeric 283";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283___valid].name = "numeric_mdb_field___283___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283___valid].doc = "configures field validity of numeric 283";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284].name = "numeric_mdb_field___284";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284].doc = "configures field size of numeric 284";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284___valid].name = "numeric_mdb_field___284___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284___valid].doc = "configures field validity of numeric 284";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285].name = "numeric_mdb_field___285";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285].doc = "configures field size of numeric 285";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285___valid].name = "numeric_mdb_field___285___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285___valid].doc = "configures field validity of numeric 285";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286].name = "numeric_mdb_field___286";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286].doc = "configures field size of numeric 286";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286___valid].name = "numeric_mdb_field___286___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286___valid].doc = "configures field validity of numeric 286";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287].name = "numeric_mdb_field___287";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287].doc = "configures field size of numeric 287";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287___valid].name = "numeric_mdb_field___287___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287___valid].doc = "configures field validity of numeric 287";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288].name = "numeric_mdb_field___288";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288].doc = "configures field size of numeric 288";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288___valid].name = "numeric_mdb_field___288___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288___valid].doc = "configures field validity of numeric 288";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289].name = "numeric_mdb_field___289";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289].doc = "configures field size of numeric 289";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289___valid].name = "numeric_mdb_field___289___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289___valid].doc = "configures field validity of numeric 289";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290].name = "numeric_mdb_field___290";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290].doc = "configures field size of numeric 290";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290___valid].name = "numeric_mdb_field___290___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290___valid].doc = "configures field validity of numeric 290";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291].name = "numeric_mdb_field___291";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291].doc = "configures field size of numeric 291";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291___valid].name = "numeric_mdb_field___291___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291___valid].doc = "configures field validity of numeric 291";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292].name = "numeric_mdb_field___292";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292].doc = "configures field size of numeric 292";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292___valid].name = "numeric_mdb_field___292___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292___valid].doc = "configures field validity of numeric 292";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293].name = "numeric_mdb_field___293";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293].doc = "configures field size of numeric 293";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293___valid].name = "numeric_mdb_field___293___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293___valid].doc = "configures field validity of numeric 293";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294].name = "numeric_mdb_field___294";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294].doc = "configures field size of numeric 294";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294___valid].name = "numeric_mdb_field___294___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294___valid].doc = "configures field validity of numeric 294";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295].name = "numeric_mdb_field___295";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295].doc = "configures field size of numeric 295";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295___valid].name = "numeric_mdb_field___295___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295___valid].doc = "configures field validity of numeric 295";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296].name = "numeric_mdb_field___296";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296].doc = "configures field size of numeric 296";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296___valid].name = "numeric_mdb_field___296___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296___valid].doc = "configures field validity of numeric 296";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297].name = "numeric_mdb_field___297";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297].doc = "configures field size of numeric 297";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297___valid].name = "numeric_mdb_field___297___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297___valid].doc = "configures field validity of numeric 297";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298].name = "numeric_mdb_field___298";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298].doc = "configures field size of numeric 298";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298___valid].name = "numeric_mdb_field___298___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298___valid].doc = "configures field validity of numeric 298";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299].name = "numeric_mdb_field___299";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299].doc = "configures field size of numeric 299";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299___valid].name = "numeric_mdb_field___299___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299___valid].doc = "configures field validity of numeric 299";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300].name = "numeric_mdb_field___300";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300].doc = "configures field size of numeric 300";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300___valid].name = "numeric_mdb_field___300___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300___valid].doc = "configures field validity of numeric 300";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301].name = "numeric_mdb_field___301";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301].doc = "configures field size of numeric 301";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301___valid].name = "numeric_mdb_field___301___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301___valid].doc = "configures field validity of numeric 301";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302].name = "numeric_mdb_field___302";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302].doc = "configures field size of numeric 302";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302___valid].name = "numeric_mdb_field___302___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302___valid].doc = "configures field validity of numeric 302";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303].name = "numeric_mdb_field___303";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303].doc = "configures field size of numeric 303";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303___valid].name = "numeric_mdb_field___303___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303___valid].doc = "configures field validity of numeric 303";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304].name = "numeric_mdb_field___304";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304].doc = "configures field size of numeric 304";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304___valid].name = "numeric_mdb_field___304___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304___valid].doc = "configures field validity of numeric 304";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305].name = "numeric_mdb_field___305";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305].doc = "configures field size of numeric 305";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305___valid].name = "numeric_mdb_field___305___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305___valid].doc = "configures field validity of numeric 305";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306].name = "numeric_mdb_field___306";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306].doc = "configures field size of numeric 306";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306___valid].name = "numeric_mdb_field___306___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306___valid].doc = "configures field validity of numeric 306";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307].name = "numeric_mdb_field___307";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307].doc = "configures field size of numeric 307";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307___valid].name = "numeric_mdb_field___307___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307___valid].doc = "configures field validity of numeric 307";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308].name = "numeric_mdb_field___308";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308].doc = "configures field size of numeric 308";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308___valid].name = "numeric_mdb_field___308___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308___valid].doc = "configures field validity of numeric 308";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309].name = "numeric_mdb_field___309";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309].doc = "configures field size of numeric 309";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309___valid].name = "numeric_mdb_field___309___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309___valid].doc = "configures field validity of numeric 309";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310].name = "numeric_mdb_field___310";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310].doc = "configures field size of numeric 310";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310___valid].name = "numeric_mdb_field___310___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310___valid].doc = "configures field validity of numeric 310";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311].name = "numeric_mdb_field___311";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311].doc = "configures field size of numeric 311";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311___valid].name = "numeric_mdb_field___311___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311___valid].doc = "configures field validity of numeric 311";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312].name = "numeric_mdb_field___312";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312].doc = "configures field size of numeric 312";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312___valid].name = "numeric_mdb_field___312___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312___valid].doc = "configures field validity of numeric 312";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313].name = "numeric_mdb_field___313";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313].doc = "configures field size of numeric 313";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313___valid].name = "numeric_mdb_field___313___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313___valid].doc = "configures field validity of numeric 313";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314].name = "numeric_mdb_field___314";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314].doc = "configures field size of numeric 314";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314___valid].name = "numeric_mdb_field___314___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314___valid].doc = "configures field validity of numeric 314";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315].name = "numeric_mdb_field___315";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315].doc = "configures field size of numeric 315";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315___valid].name = "numeric_mdb_field___315___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315___valid].doc = "configures field validity of numeric 315";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316].name = "numeric_mdb_field___316";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316].doc = "configures field size of numeric 316";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316___valid].name = "numeric_mdb_field___316___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316___valid].doc = "configures field validity of numeric 316";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317].name = "numeric_mdb_field___317";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317].doc = "configures field size of numeric 317";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317___valid].name = "numeric_mdb_field___317___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317___valid].doc = "configures field validity of numeric 317";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318].name = "numeric_mdb_field___318";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318].doc = "configures field size of numeric 318";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318___valid].name = "numeric_mdb_field___318___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318___valid].doc = "configures field validity of numeric 318";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319].name = "numeric_mdb_field___319";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319].doc = "configures field size of numeric 319";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319___valid].name = "numeric_mdb_field___319___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319___valid].doc = "configures field validity of numeric 319";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320].name = "numeric_mdb_field___320";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320].doc = "configures field size of numeric 320";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320___valid].name = "numeric_mdb_field___320___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320___valid].doc = "configures field validity of numeric 320";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321].name = "numeric_mdb_field___321";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321].doc = "configures field size of numeric 321";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321___valid].name = "numeric_mdb_field___321___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321___valid].doc = "configures field validity of numeric 321";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322].name = "numeric_mdb_field___322";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322].doc = "configures field size of numeric 322";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322___valid].name = "numeric_mdb_field___322___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322___valid].doc = "configures field validity of numeric 322";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323].name = "numeric_mdb_field___323";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323].doc = "configures field size of numeric 323";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323___valid].name = "numeric_mdb_field___323___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323___valid].doc = "configures field validity of numeric 323";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324].name = "numeric_mdb_field___324";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324].doc = "configures field size of numeric 324";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324___valid].name = "numeric_mdb_field___324___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324___valid].doc = "configures field validity of numeric 324";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325].name = "numeric_mdb_field___325";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325].doc = "configures field size of numeric 325";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325___valid].name = "numeric_mdb_field___325___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325___valid].doc = "configures field validity of numeric 325";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326].name = "numeric_mdb_field___326";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326].doc = "configures field size of numeric 326";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326___valid].name = "numeric_mdb_field___326___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326___valid].doc = "configures field validity of numeric 326";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327].name = "numeric_mdb_field___327";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327].doc = "configures field size of numeric 327";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327___valid].name = "numeric_mdb_field___327___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327___valid].doc = "configures field validity of numeric 327";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328].name = "numeric_mdb_field___328";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328].doc = "configures field size of numeric 328";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328___valid].name = "numeric_mdb_field___328___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328___valid].doc = "configures field validity of numeric 328";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329].name = "numeric_mdb_field___329";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329].doc = "configures field size of numeric 329";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329___valid].name = "numeric_mdb_field___329___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329___valid].doc = "configures field validity of numeric 329";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330].name = "numeric_mdb_field___330";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330].doc = "configures field size of numeric 330";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330___valid].name = "numeric_mdb_field___330___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330___valid].doc = "configures field validity of numeric 330";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331].name = "numeric_mdb_field___331";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331].doc = "configures field size of numeric 331";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331___valid].name = "numeric_mdb_field___331___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331___valid].doc = "configures field validity of numeric 331";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332].name = "numeric_mdb_field___332";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332].doc = "configures field size of numeric 332";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332___valid].name = "numeric_mdb_field___332___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332___valid].doc = "configures field validity of numeric 332";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333].name = "numeric_mdb_field___333";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333].doc = "configures field size of numeric 333";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333___valid].name = "numeric_mdb_field___333___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333___valid].doc = "configures field validity of numeric 333";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334].name = "numeric_mdb_field___334";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334].doc = "configures field size of numeric 334";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334___valid].name = "numeric_mdb_field___334___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334___valid].doc = "configures field validity of numeric 334";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335].name = "numeric_mdb_field___335";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335].doc = "configures field size of numeric 335";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335___valid].name = "numeric_mdb_field___335___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335___valid].doc = "configures field validity of numeric 335";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336].name = "numeric_mdb_field___336";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336].doc = "configures field size of numeric 336";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336___valid].name = "numeric_mdb_field___336___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336___valid].doc = "configures field validity of numeric 336";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337].name = "numeric_mdb_field___337";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337].doc = "configures field size of numeric 337";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337___valid].name = "numeric_mdb_field___337___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337___valid].doc = "configures field validity of numeric 337";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338].name = "numeric_mdb_field___338";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338].doc = "configures field size of numeric 338";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338___valid].name = "numeric_mdb_field___338___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338___valid].doc = "configures field validity of numeric 338";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339].name = "numeric_mdb_field___339";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339].doc = "configures field size of numeric 339";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339___valid].name = "numeric_mdb_field___339___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339___valid].doc = "configures field validity of numeric 339";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340].name = "numeric_mdb_field___340";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340].doc = "configures field size of numeric 340";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340___valid].name = "numeric_mdb_field___340___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340___valid].doc = "configures field validity of numeric 340";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341].name = "numeric_mdb_field___341";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341].doc = "configures field size of numeric 341";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341___valid].name = "numeric_mdb_field___341___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341___valid].doc = "configures field validity of numeric 341";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342].name = "numeric_mdb_field___342";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342].doc = "configures field size of numeric 342";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342___valid].name = "numeric_mdb_field___342___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342___valid].doc = "configures field validity of numeric 342";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343].name = "numeric_mdb_field___343";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343].doc = "configures field size of numeric 343";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343___valid].name = "numeric_mdb_field___343___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343___valid].doc = "configures field validity of numeric 343";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344].name = "numeric_mdb_field___344";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344].doc = "configures field size of numeric 344";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344___valid].name = "numeric_mdb_field___344___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344___valid].doc = "configures field validity of numeric 344";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345].name = "numeric_mdb_field___345";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345].doc = "configures field size of numeric 345";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345___valid].name = "numeric_mdb_field___345___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345___valid].doc = "configures field validity of numeric 345";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346].name = "numeric_mdb_field___346";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346].doc = "configures field size of numeric 346";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346___valid].name = "numeric_mdb_field___346___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346___valid].doc = "configures field validity of numeric 346";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347].name = "numeric_mdb_field___347";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347].doc = "configures field size of numeric 347";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347___valid].name = "numeric_mdb_field___347___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347___valid].doc = "configures field validity of numeric 347";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348].name = "numeric_mdb_field___348";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348].doc = "configures field size of numeric 348";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348___valid].name = "numeric_mdb_field___348___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348___valid].doc = "configures field validity of numeric 348";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349].name = "numeric_mdb_field___349";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349].doc = "configures field size of numeric 349";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349___valid].name = "numeric_mdb_field___349___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349___valid].doc = "configures field validity of numeric 349";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350].name = "numeric_mdb_field___350";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350].doc = "configures field size of numeric 350";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350___valid].name = "numeric_mdb_field___350___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350___valid].doc = "configures field validity of numeric 350";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351].name = "numeric_mdb_field___351";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351].doc = "configures field size of numeric 351";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351___valid].name = "numeric_mdb_field___351___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351___valid].doc = "configures field validity of numeric 351";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352].name = "numeric_mdb_field___352";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352].doc = "configures field size of numeric 352";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352___valid].name = "numeric_mdb_field___352___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352___valid].doc = "configures field validity of numeric 352";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353].name = "numeric_mdb_field___353";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353].doc = "configures field size of numeric 353";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353___valid].name = "numeric_mdb_field___353___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353___valid].doc = "configures field validity of numeric 353";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354].name = "numeric_mdb_field___354";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354].doc = "configures field size of numeric 354";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354___valid].name = "numeric_mdb_field___354___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354___valid].doc = "configures field validity of numeric 354";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355].name = "numeric_mdb_field___355";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355].doc = "configures field size of numeric 355";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355___valid].name = "numeric_mdb_field___355___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355___valid].doc = "configures field validity of numeric 355";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356].name = "numeric_mdb_field___356";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356].doc = "configures field size of numeric 356";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356___valid].name = "numeric_mdb_field___356___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356___valid].doc = "configures field validity of numeric 356";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357].name = "numeric_mdb_field___357";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357].doc = "configures field size of numeric 357";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357___valid].name = "numeric_mdb_field___357___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357___valid].doc = "configures field validity of numeric 357";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358].name = "numeric_mdb_field___358";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358].doc = "configures field size of numeric 358";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358___valid].name = "numeric_mdb_field___358___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358___valid].doc = "configures field validity of numeric 358";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359].name = "numeric_mdb_field___359";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359].doc = "configures field size of numeric 359";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359___valid].name = "numeric_mdb_field___359___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359___valid].doc = "configures field validity of numeric 359";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360].name = "numeric_mdb_field___360";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360].doc = "configures field size of numeric 360";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360___valid].name = "numeric_mdb_field___360___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360___valid].doc = "configures field validity of numeric 360";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361].name = "numeric_mdb_field___361";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361].doc = "configures field size of numeric 361";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361___valid].name = "numeric_mdb_field___361___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361___valid].doc = "configures field validity of numeric 361";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362].name = "numeric_mdb_field___362";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362].doc = "configures field size of numeric 362";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362___valid].name = "numeric_mdb_field___362___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362___valid].doc = "configures field validity of numeric 362";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363].name = "numeric_mdb_field___363";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363].doc = "configures field size of numeric 363";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363___valid].name = "numeric_mdb_field___363___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363___valid].doc = "configures field validity of numeric 363";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364].name = "numeric_mdb_field___364";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364].doc = "configures field size of numeric 364";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364___valid].name = "numeric_mdb_field___364___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364___valid].doc = "configures field validity of numeric 364";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365].name = "numeric_mdb_field___365";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365].doc = "configures field size of numeric 365";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365___valid].name = "numeric_mdb_field___365___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365___valid].doc = "configures field validity of numeric 365";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366].name = "numeric_mdb_field___366";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366].doc = "configures field size of numeric 366";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366___valid].name = "numeric_mdb_field___366___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366___valid].doc = "configures field validity of numeric 366";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367].name = "numeric_mdb_field___367";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367].doc = "configures field size of numeric 367";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367___valid].name = "numeric_mdb_field___367___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367___valid].doc = "configures field validity of numeric 367";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368].name = "numeric_mdb_field___368";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368].doc = "configures field size of numeric 368";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368___valid].name = "numeric_mdb_field___368___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368___valid].doc = "configures field validity of numeric 368";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369].name = "numeric_mdb_field___369";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369].doc = "configures field size of numeric 369";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369___valid].name = "numeric_mdb_field___369___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369___valid].doc = "configures field validity of numeric 369";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370].name = "numeric_mdb_field___370";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370].doc = "configures field size of numeric 370";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370___valid].name = "numeric_mdb_field___370___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370___valid].doc = "configures field validity of numeric 370";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371].name = "numeric_mdb_field___371";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371].doc = "configures field size of numeric 371";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371___valid].name = "numeric_mdb_field___371___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371___valid].doc = "configures field validity of numeric 371";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372].name = "numeric_mdb_field___372";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372].doc = "configures field size of numeric 372";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372___valid].name = "numeric_mdb_field___372___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372___valid].doc = "configures field validity of numeric 372";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373].name = "numeric_mdb_field___373";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373].doc = "configures field size of numeric 373";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373___valid].name = "numeric_mdb_field___373___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373___valid].doc = "configures field validity of numeric 373";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374].name = "numeric_mdb_field___374";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374].doc = "configures field size of numeric 374";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374___valid].name = "numeric_mdb_field___374___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374___valid].doc = "configures field validity of numeric 374";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375].name = "numeric_mdb_field___375";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375].doc = "configures field size of numeric 375";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375___valid].name = "numeric_mdb_field___375___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375___valid].doc = "configures field validity of numeric 375";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376].name = "numeric_mdb_field___376";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376].doc = "configures field size of numeric 376";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376___valid].name = "numeric_mdb_field___376___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376___valid].doc = "configures field validity of numeric 376";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377].name = "numeric_mdb_field___377";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377].doc = "configures field size of numeric 377";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377___valid].name = "numeric_mdb_field___377___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377___valid].doc = "configures field validity of numeric 377";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378].name = "numeric_mdb_field___378";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378].doc = "configures field size of numeric 378";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378___valid].name = "numeric_mdb_field___378___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378___valid].doc = "configures field validity of numeric 378";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379].name = "numeric_mdb_field___379";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379].doc = "configures field size of numeric 379";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379___valid].name = "numeric_mdb_field___379___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379___valid].doc = "configures field validity of numeric 379";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380].name = "numeric_mdb_field___380";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380].doc = "configures field size of numeric 380";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380___valid].name = "numeric_mdb_field___380___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380___valid].doc = "configures field validity of numeric 380";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381].name = "numeric_mdb_field___381";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381].doc = "configures field size of numeric 381";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381___valid].name = "numeric_mdb_field___381___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381___valid].doc = "configures field validity of numeric 381";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382].name = "numeric_mdb_field___382";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382].doc = "configures field size of numeric 382";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382___valid].name = "numeric_mdb_field___382___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382___valid].doc = "configures field validity of numeric 382";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383].name = "numeric_mdb_field___383";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383].doc = "configures field size of numeric 383";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383___valid].name = "numeric_mdb_field___383___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383___valid].doc = "configures field validity of numeric 383";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384].name = "numeric_mdb_field___384";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384].doc = "configures field size of numeric 384";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384___valid].name = "numeric_mdb_field___384___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384___valid].doc = "configures field validity of numeric 384";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385].name = "numeric_mdb_field___385";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385].doc = "configures field size of numeric 385";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385___valid].name = "numeric_mdb_field___385___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385___valid].doc = "configures field validity of numeric 385";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386].name = "numeric_mdb_field___386";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386].doc = "configures field size of numeric 386";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386___valid].name = "numeric_mdb_field___386___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386___valid].doc = "configures field validity of numeric 386";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387].name = "numeric_mdb_field___387";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387].doc = "configures field size of numeric 387";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387___valid].name = "numeric_mdb_field___387___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387___valid].doc = "configures field validity of numeric 387";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388].name = "numeric_mdb_field___388";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388].doc = "configures field size of numeric 388";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388___valid].name = "numeric_mdb_field___388___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388___valid].doc = "configures field validity of numeric 388";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389].name = "numeric_mdb_field___389";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389].doc = "configures field size of numeric 389";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389___valid].name = "numeric_mdb_field___389___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389___valid].doc = "configures field validity of numeric 389";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390].name = "numeric_mdb_field___390";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390].doc = "configures field size of numeric 390";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390___valid].name = "numeric_mdb_field___390___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390___valid].doc = "configures field validity of numeric 390";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391].name = "numeric_mdb_field___391";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391].doc = "configures field size of numeric 391";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391___valid].name = "numeric_mdb_field___391___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391___valid].doc = "configures field validity of numeric 391";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392].name = "numeric_mdb_field___392";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392].doc = "configures field size of numeric 392";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392___valid].name = "numeric_mdb_field___392___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392___valid].doc = "configures field validity of numeric 392";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393].name = "numeric_mdb_field___393";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393].doc = "configures field size of numeric 393";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393___valid].name = "numeric_mdb_field___393___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393___valid].doc = "configures field validity of numeric 393";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394].name = "numeric_mdb_field___394";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394].doc = "configures field size of numeric 394";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394___valid].name = "numeric_mdb_field___394___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394___valid].doc = "configures field validity of numeric 394";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395].name = "numeric_mdb_field___395";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395].doc = "configures field size of numeric 395";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395___valid].name = "numeric_mdb_field___395___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395___valid].doc = "configures field validity of numeric 395";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396].name = "numeric_mdb_field___396";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396].doc = "configures field size of numeric 396";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396___valid].name = "numeric_mdb_field___396___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396___valid].doc = "configures field validity of numeric 396";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397].name = "numeric_mdb_field___397";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397].doc = "configures field size of numeric 397";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397___valid].name = "numeric_mdb_field___397___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397___valid].doc = "configures field validity of numeric 397";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398].name = "numeric_mdb_field___398";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398].doc = "configures field size of numeric 398";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398___valid].name = "numeric_mdb_field___398___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398___valid].doc = "configures field validity of numeric 398";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399].name = "numeric_mdb_field___399";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399].doc = "configures field size of numeric 399";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399___valid].name = "numeric_mdb_field___399___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399___valid].doc = "configures field validity of numeric 399";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400].name = "numeric_mdb_field___400";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400].doc = "configures field size of numeric 400";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400___valid].name = "numeric_mdb_field___400___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400___valid].doc = "configures field validity of numeric 400";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401].name = "numeric_mdb_field___401";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401].doc = "configures field size of numeric 401";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401___valid].name = "numeric_mdb_field___401___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401___valid].doc = "configures field validity of numeric 401";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402].name = "numeric_mdb_field___402";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402].doc = "configures field size of numeric 402";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402___valid].name = "numeric_mdb_field___402___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402___valid].doc = "configures field validity of numeric 402";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403].name = "numeric_mdb_field___403";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403].doc = "configures field size of numeric 403";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403___valid].name = "numeric_mdb_field___403___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403___valid].doc = "configures field validity of numeric 403";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404].name = "numeric_mdb_field___404";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404].doc = "configures field size of numeric 404";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404___valid].name = "numeric_mdb_field___404___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404___valid].doc = "configures field validity of numeric 404";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405].name = "numeric_mdb_field___405";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405].doc = "configures field size of numeric 405";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405___valid].name = "numeric_mdb_field___405___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405___valid].doc = "configures field validity of numeric 405";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406].name = "numeric_mdb_field___406";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406].doc = "configures field size of numeric 406";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406___valid].name = "numeric_mdb_field___406___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406___valid].doc = "configures field validity of numeric 406";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407].name = "numeric_mdb_field___407";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407].doc = "configures field size of numeric 407";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407___valid].name = "numeric_mdb_field___407___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407___valid].doc = "configures field validity of numeric 407";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408].name = "numeric_mdb_field___408";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408].doc = "configures field size of numeric 408";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408___valid].name = "numeric_mdb_field___408___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408___valid].doc = "configures field validity of numeric 408";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409].name = "numeric_mdb_field___409";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409].doc = "configures field size of numeric 409";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409___valid].name = "numeric_mdb_field___409___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409___valid].doc = "configures field validity of numeric 409";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410].name = "numeric_mdb_field___410";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410].doc = "configures field size of numeric 410";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410___valid].name = "numeric_mdb_field___410___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410___valid].doc = "configures field validity of numeric 410";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411].name = "numeric_mdb_field___411";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411].doc = "configures field size of numeric 411";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411___valid].name = "numeric_mdb_field___411___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411___valid].doc = "configures field validity of numeric 411";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412].name = "numeric_mdb_field___412";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412].doc = "configures field size of numeric 412";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412___valid].name = "numeric_mdb_field___412___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412___valid].doc = "configures field validity of numeric 412";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413].name = "numeric_mdb_field___413";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413].doc = "configures field size of numeric 413";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413___valid].name = "numeric_mdb_field___413___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413___valid].doc = "configures field validity of numeric 413";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414].name = "numeric_mdb_field___414";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414].doc = "configures field size of numeric 414";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414___valid].name = "numeric_mdb_field___414___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414___valid].doc = "configures field validity of numeric 414";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415].name = "numeric_mdb_field___415";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415].doc = "configures field size of numeric 415";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415___valid].name = "numeric_mdb_field___415___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415___valid].doc = "configures field validity of numeric 415";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416].name = "numeric_mdb_field___416";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416].doc = "configures field size of numeric 416";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416___valid].name = "numeric_mdb_field___416___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416___valid].doc = "configures field validity of numeric 416";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417].name = "numeric_mdb_field___417";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417].doc = "configures field size of numeric 417";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417___valid].name = "numeric_mdb_field___417___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417___valid].doc = "configures field validity of numeric 417";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418].name = "numeric_mdb_field___418";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418].doc = "configures field size of numeric 418";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418___valid].name = "numeric_mdb_field___418___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418___valid].doc = "configures field validity of numeric 418";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419].name = "numeric_mdb_field___419";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419].doc = "configures field size of numeric 419";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419___valid].name = "numeric_mdb_field___419___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419___valid].doc = "configures field validity of numeric 419";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420].name = "numeric_mdb_field___420";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420].doc = "configures field size of numeric 420";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420___valid].name = "numeric_mdb_field___420___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420___valid].doc = "configures field validity of numeric 420";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421].name = "numeric_mdb_field___421";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421].doc = "configures field size of numeric 421";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421___valid].name = "numeric_mdb_field___421___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421___valid].doc = "configures field validity of numeric 421";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422].name = "numeric_mdb_field___422";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422].doc = "configures field size of numeric 422";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422___valid].name = "numeric_mdb_field___422___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422___valid].doc = "configures field validity of numeric 422";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423].name = "numeric_mdb_field___423";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423].doc = "configures field size of numeric 423";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423___valid].name = "numeric_mdb_field___423___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423___valid].doc = "configures field validity of numeric 423";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424].name = "numeric_mdb_field___424";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424].doc = "configures field size of numeric 424";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424___valid].name = "numeric_mdb_field___424___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424___valid].doc = "configures field validity of numeric 424";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425].name = "numeric_mdb_field___425";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425].doc = "configures field size of numeric 425";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425___valid].name = "numeric_mdb_field___425___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425___valid].doc = "configures field validity of numeric 425";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426].name = "numeric_mdb_field___426";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426].doc = "configures field size of numeric 426";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426___valid].name = "numeric_mdb_field___426___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426___valid].doc = "configures field validity of numeric 426";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427].name = "numeric_mdb_field___427";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427].doc = "configures field size of numeric 427";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427___valid].name = "numeric_mdb_field___427___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427___valid].doc = "configures field validity of numeric 427";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428].name = "numeric_mdb_field___428";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428].doc = "configures field size of numeric 428";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428___valid].name = "numeric_mdb_field___428___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428___valid].doc = "configures field validity of numeric 428";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429].name = "numeric_mdb_field___429";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429].doc = "configures field size of numeric 429";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429___valid].name = "numeric_mdb_field___429___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429___valid].doc = "configures field validity of numeric 429";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430].name = "numeric_mdb_field___430";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430].doc = "configures field size of numeric 430";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430___valid].name = "numeric_mdb_field___430___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430___valid].doc = "configures field validity of numeric 430";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431].name = "numeric_mdb_field___431";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431].doc = "configures field size of numeric 431";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431___valid].name = "numeric_mdb_field___431___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431___valid].doc = "configures field validity of numeric 431";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432].name = "numeric_mdb_field___432";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432].doc = "configures field size of numeric 432";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432___valid].name = "numeric_mdb_field___432___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432___valid].doc = "configures field validity of numeric 432";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433].name = "numeric_mdb_field___433";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433].doc = "configures field size of numeric 433";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433___valid].name = "numeric_mdb_field___433___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433___valid].doc = "configures field validity of numeric 433";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434].name = "numeric_mdb_field___434";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434].doc = "configures field size of numeric 434";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434___valid].name = "numeric_mdb_field___434___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434___valid].doc = "configures field validity of numeric 434";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435].name = "numeric_mdb_field___435";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435].doc = "configures field size of numeric 435";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435___valid].name = "numeric_mdb_field___435___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435___valid].doc = "configures field validity of numeric 435";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436].name = "numeric_mdb_field___436";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436].doc = "configures field size of numeric 436";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436___valid].name = "numeric_mdb_field___436___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436___valid].doc = "configures field validity of numeric 436";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437].name = "numeric_mdb_field___437";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437].doc = "configures field size of numeric 437";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437___valid].name = "numeric_mdb_field___437___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437___valid].doc = "configures field validity of numeric 437";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438].name = "numeric_mdb_field___438";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438].doc = "configures field size of numeric 438";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438___valid].name = "numeric_mdb_field___438___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438___valid].doc = "configures field validity of numeric 438";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439].name = "numeric_mdb_field___439";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439].doc = "configures field size of numeric 439";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439___valid].name = "numeric_mdb_field___439___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439___valid].doc = "configures field validity of numeric 439";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440].name = "numeric_mdb_field___440";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440].doc = "configures field size of numeric 440";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440___valid].name = "numeric_mdb_field___440___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440___valid].doc = "configures field validity of numeric 440";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441].name = "numeric_mdb_field___441";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441].doc = "configures field size of numeric 441";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441___valid].name = "numeric_mdb_field___441___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441___valid].doc = "configures field validity of numeric 441";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442].name = "numeric_mdb_field___442";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442].doc = "configures field size of numeric 442";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442___valid].name = "numeric_mdb_field___442___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442___valid].doc = "configures field validity of numeric 442";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443].name = "numeric_mdb_field___443";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443].doc = "configures field size of numeric 443";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443___valid].name = "numeric_mdb_field___443___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443___valid].doc = "configures field validity of numeric 443";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444].name = "numeric_mdb_field___444";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444].doc = "configures field size of numeric 444";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444___valid].name = "numeric_mdb_field___444___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444___valid].doc = "configures field validity of numeric 444";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445].name = "numeric_mdb_field___445";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445].doc = "configures field size of numeric 445";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445___valid].name = "numeric_mdb_field___445___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445___valid].doc = "configures field validity of numeric 445";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446].name = "numeric_mdb_field___446";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446].doc = "configures field size of numeric 446";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446___valid].name = "numeric_mdb_field___446___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446___valid].doc = "configures field validity of numeric 446";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447].name = "numeric_mdb_field___447";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447].doc = "configures field size of numeric 447";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447___valid].name = "numeric_mdb_field___447___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447___valid].doc = "configures field validity of numeric 447";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448].name = "numeric_mdb_field___448";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448].doc = "configures field size of numeric 448";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448___valid].name = "numeric_mdb_field___448___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448___valid].doc = "configures field validity of numeric 448";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449].name = "numeric_mdb_field___449";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449].doc = "configures field size of numeric 449";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449___valid].name = "numeric_mdb_field___449___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449___valid].doc = "configures field validity of numeric 449";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450].name = "numeric_mdb_field___450";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450].doc = "configures field size of numeric 450";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450___valid].name = "numeric_mdb_field___450___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450___valid].doc = "configures field validity of numeric 450";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451].name = "numeric_mdb_field___451";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451].doc = "configures field size of numeric 451";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451___valid].name = "numeric_mdb_field___451___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451___valid].doc = "configures field validity of numeric 451";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452].name = "numeric_mdb_field___452";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452].doc = "configures field size of numeric 452";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452___valid].name = "numeric_mdb_field___452___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452___valid].doc = "configures field validity of numeric 452";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453].name = "numeric_mdb_field___453";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453].doc = "configures field size of numeric 453";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453___valid].name = "numeric_mdb_field___453___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453___valid].doc = "configures field validity of numeric 453";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454].name = "numeric_mdb_field___454";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454].doc = "configures field size of numeric 454";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454___valid].name = "numeric_mdb_field___454___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454___valid].doc = "configures field validity of numeric 454";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455].name = "numeric_mdb_field___455";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455].doc = "configures field size of numeric 455";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455___valid].name = "numeric_mdb_field___455___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455___valid].doc = "configures field validity of numeric 455";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456].name = "numeric_mdb_field___456";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456].doc = "configures field size of numeric 456";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456___valid].name = "numeric_mdb_field___456___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456___valid].doc = "configures field validity of numeric 456";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457].name = "numeric_mdb_field___457";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457].doc = "configures field size of numeric 457";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457___valid].name = "numeric_mdb_field___457___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457___valid].doc = "configures field validity of numeric 457";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458].name = "numeric_mdb_field___458";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458].doc = "configures field size of numeric 458";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458___valid].name = "numeric_mdb_field___458___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458___valid].doc = "configures field validity of numeric 458";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459].name = "numeric_mdb_field___459";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459].doc = "configures field size of numeric 459";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459___valid].name = "numeric_mdb_field___459___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459___valid].doc = "configures field validity of numeric 459";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460].name = "numeric_mdb_field___460";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460].doc = "configures field size of numeric 460";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460___valid].name = "numeric_mdb_field___460___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460___valid].doc = "configures field validity of numeric 460";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461].name = "numeric_mdb_field___461";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461].doc = "configures field size of numeric 461";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461___valid].name = "numeric_mdb_field___461___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461___valid].doc = "configures field validity of numeric 461";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462].name = "numeric_mdb_field___462";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462].doc = "configures field size of numeric 462";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462___valid].name = "numeric_mdb_field___462___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462___valid].doc = "configures field validity of numeric 462";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463].name = "numeric_mdb_field___463";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463].doc = "configures field size of numeric 463";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463___valid].name = "numeric_mdb_field___463___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463___valid].doc = "configures field validity of numeric 463";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464].name = "numeric_mdb_field___464";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464].doc = "configures field size of numeric 464";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464___valid].name = "numeric_mdb_field___464___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464___valid].doc = "configures field validity of numeric 464";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465].name = "numeric_mdb_field___465";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465].doc = "configures field size of numeric 465";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465___valid].name = "numeric_mdb_field___465___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465___valid].doc = "configures field validity of numeric 465";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466].name = "numeric_mdb_field___466";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466].doc = "configures field size of numeric 466";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466___valid].name = "numeric_mdb_field___466___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466___valid].doc = "configures field validity of numeric 466";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467].name = "numeric_mdb_field___467";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467].doc = "configures field size of numeric 467";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467___valid].name = "numeric_mdb_field___467___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467___valid].doc = "configures field validity of numeric 467";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468].name = "numeric_mdb_field___468";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468].doc = "configures field size of numeric 468";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468___valid].name = "numeric_mdb_field___468___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468___valid].doc = "configures field validity of numeric 468";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469].name = "numeric_mdb_field___469";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469].doc = "configures field size of numeric 469";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469___valid].name = "numeric_mdb_field___469___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469___valid].doc = "configures field validity of numeric 469";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470].name = "numeric_mdb_field___470";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470].doc = "configures field size of numeric 470";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470___valid].name = "numeric_mdb_field___470___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470___valid].doc = "configures field validity of numeric 470";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471].name = "numeric_mdb_field___471";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471].doc = "configures field size of numeric 471";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471___valid].name = "numeric_mdb_field___471___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471___valid].doc = "configures field validity of numeric 471";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472].name = "numeric_mdb_field___472";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472].doc = "configures field size of numeric 472";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472___valid].name = "numeric_mdb_field___472___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472___valid].doc = "configures field validity of numeric 472";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473].name = "numeric_mdb_field___473";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473].doc = "configures field size of numeric 473";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473___valid].name = "numeric_mdb_field___473___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473___valid].doc = "configures field validity of numeric 473";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474].name = "numeric_mdb_field___474";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474].doc = "configures field size of numeric 474";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474___valid].name = "numeric_mdb_field___474___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474___valid].doc = "configures field validity of numeric 474";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475].name = "numeric_mdb_field___475";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475].doc = "configures field size of numeric 475";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475___valid].name = "numeric_mdb_field___475___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475___valid].doc = "configures field validity of numeric 475";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476].name = "numeric_mdb_field___476";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476].doc = "configures field size of numeric 476";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476___valid].name = "numeric_mdb_field___476___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476___valid].doc = "configures field validity of numeric 476";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477].name = "numeric_mdb_field___477";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477].doc = "configures field size of numeric 477";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477___valid].name = "numeric_mdb_field___477___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477___valid].doc = "configures field validity of numeric 477";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478].name = "numeric_mdb_field___478";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478].doc = "configures field size of numeric 478";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478___valid].name = "numeric_mdb_field___478___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478___valid].doc = "configures field validity of numeric 478";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479].name = "numeric_mdb_field___479";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479].doc = "configures field size of numeric 479";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479___valid].name = "numeric_mdb_field___479___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479___valid].doc = "configures field validity of numeric 479";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480].name = "numeric_mdb_field___480";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480].doc = "configures field size of numeric 480";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480___valid].name = "numeric_mdb_field___480___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480___valid].doc = "configures field validity of numeric 480";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481].name = "numeric_mdb_field___481";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481].doc = "configures field size of numeric 481";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481___valid].name = "numeric_mdb_field___481___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481___valid].doc = "configures field validity of numeric 481";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482].name = "numeric_mdb_field___482";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482].doc = "configures field size of numeric 482";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482___valid].name = "numeric_mdb_field___482___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482___valid].doc = "configures field validity of numeric 482";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483].name = "numeric_mdb_field___483";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483].doc = "configures field size of numeric 483";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483___valid].name = "numeric_mdb_field___483___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483___valid].doc = "configures field validity of numeric 483";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484].name = "numeric_mdb_field___484";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484].doc = "configures field size of numeric 484";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484___valid].name = "numeric_mdb_field___484___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484___valid].doc = "configures field validity of numeric 484";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485].name = "numeric_mdb_field___485";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485].doc = "configures field size of numeric 485";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485___valid].name = "numeric_mdb_field___485___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485___valid].doc = "configures field validity of numeric 485";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486].name = "numeric_mdb_field___486";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486].doc = "configures field size of numeric 486";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486___valid].name = "numeric_mdb_field___486___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486___valid].doc = "configures field validity of numeric 486";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487].name = "numeric_mdb_field___487";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487].doc = "configures field size of numeric 487";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487___valid].name = "numeric_mdb_field___487___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487___valid].doc = "configures field validity of numeric 487";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488].name = "numeric_mdb_field___488";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488].doc = "configures field size of numeric 488";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488___valid].name = "numeric_mdb_field___488___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488___valid].doc = "configures field validity of numeric 488";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489].name = "numeric_mdb_field___489";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489].doc = "configures field size of numeric 489";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489___valid].name = "numeric_mdb_field___489___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489___valid].doc = "configures field validity of numeric 489";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490].name = "numeric_mdb_field___490";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490].doc = "configures field size of numeric 490";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490___valid].name = "numeric_mdb_field___490___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490___valid].doc = "configures field validity of numeric 490";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491].name = "numeric_mdb_field___491";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491].doc = "configures field size of numeric 491";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491___valid].name = "numeric_mdb_field___491___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491___valid].doc = "configures field validity of numeric 491";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492].name = "numeric_mdb_field___492";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492].doc = "configures field size of numeric 492";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492___valid].name = "numeric_mdb_field___492___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492___valid].doc = "configures field validity of numeric 492";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493].name = "numeric_mdb_field___493";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493].doc = "configures field size of numeric 493";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493___valid].name = "numeric_mdb_field___493___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493___valid].doc = "configures field validity of numeric 493";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494].name = "numeric_mdb_field___494";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494].doc = "configures field size of numeric 494";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494___valid].name = "numeric_mdb_field___494___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494___valid].doc = "configures field validity of numeric 494";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495].name = "numeric_mdb_field___495";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495].doc = "configures field size of numeric 495";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495___valid].name = "numeric_mdb_field___495___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495___valid].doc = "configures field validity of numeric 495";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496].name = "numeric_mdb_field___496";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496].doc = "configures field size of numeric 496";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496___valid].name = "numeric_mdb_field___496___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496___valid].doc = "configures field validity of numeric 496";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497].name = "numeric_mdb_field___497";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497].doc = "configures field size of numeric 497";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497___valid].name = "numeric_mdb_field___497___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497___valid].doc = "configures field validity of numeric 497";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498].name = "numeric_mdb_field___498";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498].doc = "configures field size of numeric 498";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498___valid].name = "numeric_mdb_field___498___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498___valid].doc = "configures field validity of numeric 498";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498___valid].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499].name = "numeric_mdb_field___499";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499].doc = "configures field size of numeric 499";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499___valid].name = "numeric_mdb_field___499___valid";
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499___valid].doc = "configures field validity of numeric 499";
    
    submodule_data->defines[dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499___valid].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_mdb_app_db_fields_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data mdb_app_db fields tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_mdb_app_db_fields_feature_get(
    int unit,
    dnx_data_mdb_app_db_fields_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, feature);
}


uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___0_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___0___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___0___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___1___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___1___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___2___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___2___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___3___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___3___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___4_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___4___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___4___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___5_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___5___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___5___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___6_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___6___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___6___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___7_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___7___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___7___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___8_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___8___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___8___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___9_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___9___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___9___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___10_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___10___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___10___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___11_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___11___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___11___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___12_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___12___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___12___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___13_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___13___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___13___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___14_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___14___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___14___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___15_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___15___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___15___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___16_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___16___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___16___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___17_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___17___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___17___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___18_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___18___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___18___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___19_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___19___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___19___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___20_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___20___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___20___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___21_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___21___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___21___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___22_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___22___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___22___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___23_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___23___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___23___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___24_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___24___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___24___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___25_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___25___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___25___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___26_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___26___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___26___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___27_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___27___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___27___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___28_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___28___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___28___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___29_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___29___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___29___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___30_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___30___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___30___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___31_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___31___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___31___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___32_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___32___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___32___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___33_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___33___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___33___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___34_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___34___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___34___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___35_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___35___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___35___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___36_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___36___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___36___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___37_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___37___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___37___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___38_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___38___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___38___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___39_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___39___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___39___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___40_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___40___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___40___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___41_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___41___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___41___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___42_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___42___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___42___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___43_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___43___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___43___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___44_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___44___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___44___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___45_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___45___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___45___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___46_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___46___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___46___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___47_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___47___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___47___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___48_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___48___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___48___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___49_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___49___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___49___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___50_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___50___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___50___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___51_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___51___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___51___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___52_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___52___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___52___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___53_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___53___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___53___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___54_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___54___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___54___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___55_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___55___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___55___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___56_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___56___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___56___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___57_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___57___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___57___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___58_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___58___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___58___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___59_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___59___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___59___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___60_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___60___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___60___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___61_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___61___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___61___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___62_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___62___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___62___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___63_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___63___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___63___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___64_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___64___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___64___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___65_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___65___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___65___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___66_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___66___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___66___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___67_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___67___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___67___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___68_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___68___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___68___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___69_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___69___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___69___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___70_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___70___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___70___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___71_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___71___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___71___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___72_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___72___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___72___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___73_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___73___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___73___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___74_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___74___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___74___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___75_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___75___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___75___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___76_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___76___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___76___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___77_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___77___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___77___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___78_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___78___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___78___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___79_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___79___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___79___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___80_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___80___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___80___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___81_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___81___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___81___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___82_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___82___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___82___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___83_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___83___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___83___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___84_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___84___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___84___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___85_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___85___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___85___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___86_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___86___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___86___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___87_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___87___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___87___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___88_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___88___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___88___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___89_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___89___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___89___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___90_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___90___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___90___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___91_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___91___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___91___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___92_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___92___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___92___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___93_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___93___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___93___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___94_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___94___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___94___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___95_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___95___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___95___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___96_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___96___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___96___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___97_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___97___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___97___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___98_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___98___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___98___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___99_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___99___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___99___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___100_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___100___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___100___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___101_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___101___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___101___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___102_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___102___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___102___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___103_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___103___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___103___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___104_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___104___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___104___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___105_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___105___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___105___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___106_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___106___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___106___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___107_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___107___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___107___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___108_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___108___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___108___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___109_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___109___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___109___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___110_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___110___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___110___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___111_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___111___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___111___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___112_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___112___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___112___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___113_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___113___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___113___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___114_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___114___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___114___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___115_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___115___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___115___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___116_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___116___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___116___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___117_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___117___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___117___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___118_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___118___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___118___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___119_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___119___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___119___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___120_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___120___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___120___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___121_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___121___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___121___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___122_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___122___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___122___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___123_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___123___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___123___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___124_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___124___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___124___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___125_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___125___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___125___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___126_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___126___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___126___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___127_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___127___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___127___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___128_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___128___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___128___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___129_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___129___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___129___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___130_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___130___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___130___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___131_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___131___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___131___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___132_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___132___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___132___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___133_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___133___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___133___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___134_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___134___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___134___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___135_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___135___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___135___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___136_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___136___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___136___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___137_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___137___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___137___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___138_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___138___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___138___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___139_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___139___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___139___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___140_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___140___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___140___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___141_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___141___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___141___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___142_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___142___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___142___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___143_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___143___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___143___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___144_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___144___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___144___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___145_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___145___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___145___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___146_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___146___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___146___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___147_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___147___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___147___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___148_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___148___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___148___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___149_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___149___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___149___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___150_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___150___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___150___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___151_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___151___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___151___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___152_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___152___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___152___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___153_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___153___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___153___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___154_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___154___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___154___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___155_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___155___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___155___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___156_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___156___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___156___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___157_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___157___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___157___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___158_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___158___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___158___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___159_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___159___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___159___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___160_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___160___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___160___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___161_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___161___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___161___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___162_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___162___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___162___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___163_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___163___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___163___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___164_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___164___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___164___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___165_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___165___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___165___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___166_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___166___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___166___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___167_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___167___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___167___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___168_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___168___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___168___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___169_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___169___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___169___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___170_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___170___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___170___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___171_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___171___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___171___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___172_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___172___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___172___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___173_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___173___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___173___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___174_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___174___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___174___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___175_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___175___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___175___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___176_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___176___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___176___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___177_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___177___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___177___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___178_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___178___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___178___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___179_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___179___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___179___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___180_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___180___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___180___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___181_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___181___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___181___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___182_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___182___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___182___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___183_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___183___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___183___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___184_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___184___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___184___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___185_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___185___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___185___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___186_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___186___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___186___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___187_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___187___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___187___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___188_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___188___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___188___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___189_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___189___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___189___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___190_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___190___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___190___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___191_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___191___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___191___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___192_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___192___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___192___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___193_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___193___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___193___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___194_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___194___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___194___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___195_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___195___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___195___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___196_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___196___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___196___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___197_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___197___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___197___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___198_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___198___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___198___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___199_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___199___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___199___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___200_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___200___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___200___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___201_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___201___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___201___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___202_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___202___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___202___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___203_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___203___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___203___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___204_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___204___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___204___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___205_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___205___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___205___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___206_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___206___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___206___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___207_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___207___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___207___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___208_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___208___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___208___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___209_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___209___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___209___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___210_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___210___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___210___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___211_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___211___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___211___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___212_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___212___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___212___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___213_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___213___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___213___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___214_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___214___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___214___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___215_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___215___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___215___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___216_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___216___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___216___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___217_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___217___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___217___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___218_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___218___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___218___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___219_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___219___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___219___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___220_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___220___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___220___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___221_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___221___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___221___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___222_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___222___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___222___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___223_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___223___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___223___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___224_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___224___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___224___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___225_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___225___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___225___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___226_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___226___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___226___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___227_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___227___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___227___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___228_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___228___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___228___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___229_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___229___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___229___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___230_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___230___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___230___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___231_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___231___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___231___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___232_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___232___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___232___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___233_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___233___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___233___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___234_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___234___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___234___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___235_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___235___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___235___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___236_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___236___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___236___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___237_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___237___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___237___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___238_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___238___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___238___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___239_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___239___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___239___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___240_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___240___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___240___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___241_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___241___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___241___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___242_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___242___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___242___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___243_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___243___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___243___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___244_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___244___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___244___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___245_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___245___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___245___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___246_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___246___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___246___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___247_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___247___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___247___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___248_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___248___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___248___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___249_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___249___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___249___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___250_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___250___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___250___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___251_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___251___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___251___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___252_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___252___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___252___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___253_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___253___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___253___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___254_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___254___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___254___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___255_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___255___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___255___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___256_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___256___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___256___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___257_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___257___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___257___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___258_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___258___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___258___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___259_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___259___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___259___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___260_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___260___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___260___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___261_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___261___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___261___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___262_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___262___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___262___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___263_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___263___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___263___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___264_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___264___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___264___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___265_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___265___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___265___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___266_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___266___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___266___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___267_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___267___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___267___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___268_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___268___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___268___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___269_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___269___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___269___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___270_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___270___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___270___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___271_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___271___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___271___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___272_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___272___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___272___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___273_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___273___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___273___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___274_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___274___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___274___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___275_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___275___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___275___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___276_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___276___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___276___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___277_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___277___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___277___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___278_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___278___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___278___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___279_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___279___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___279___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___280_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___280___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___280___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___281_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___281___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___281___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___282_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___282___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___282___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___283_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___283___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___283___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___284_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___284___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___284___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___285_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___285___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___285___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___286_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___286___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___286___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___287_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___287___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___287___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___288_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___288___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___288___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___289_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___289___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___289___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___290_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___290___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___290___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___291_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___291___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___291___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___292_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___292___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___292___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___293_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___293___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___293___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___294_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___294___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___294___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___295_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___295___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___295___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___296_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___296___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___296___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___297_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___297___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___297___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___298_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___298___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___298___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___299_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___299___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___299___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___300_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___300___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___300___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___301_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___301___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___301___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___302_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___302___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___302___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___303_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___303___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___303___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___304_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___304___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___304___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___305_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___305___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___305___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___306_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___306___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___306___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___307_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___307___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___307___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___308_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___308___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___308___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___309_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___309___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___309___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___310_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___310___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___310___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___311_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___311___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___311___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___312_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___312___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___312___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___313_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___313___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___313___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___314_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___314___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___314___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___315_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___315___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___315___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___316_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___316___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___316___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___317_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___317___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___317___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___318_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___318___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___318___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___319_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___319___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___319___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___320_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___320___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___320___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___321_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___321___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___321___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___322_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___322___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___322___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___323_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___323___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___323___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___324_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___324___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___324___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___325_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___325___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___325___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___326_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___326___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___326___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___327_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___327___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___327___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___328_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___328___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___328___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___329_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___329___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___329___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___330_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___330___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___330___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___331_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___331___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___331___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___332_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___332___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___332___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___333_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___333___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___333___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___334_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___334___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___334___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___335_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___335___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___335___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___336_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___336___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___336___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___337_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___337___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___337___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___338_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___338___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___338___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___339_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___339___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___339___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___340_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___340___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___340___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___341_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___341___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___341___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___342_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___342___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___342___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___343_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___343___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___343___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___344_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___344___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___344___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___345_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___345___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___345___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___346_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___346___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___346___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___347_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___347___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___347___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___348_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___348___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___348___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___349_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___349___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___349___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___350_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___350___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___350___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___351_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___351___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___351___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___352_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___352___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___352___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___353_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___353___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___353___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___354_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___354___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___354___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___355_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___355___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___355___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___356_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___356___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___356___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___357_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___357___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___357___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___358_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___358___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___358___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___359_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___359___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___359___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___360_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___360___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___360___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___361_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___361___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___361___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___362_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___362___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___362___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___363_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___363___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___363___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___364_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___364___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___364___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___365_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___365___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___365___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___366_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___366___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___366___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___367_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___367___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___367___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___368_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___368___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___368___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___369_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___369___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___369___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___370_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___370___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___370___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___371_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___371___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___371___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___372_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___372___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___372___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___373_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___373___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___373___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___374_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___374___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___374___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___375_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___375___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___375___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___376_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___376___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___376___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___377_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___377___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___377___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___378_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___378___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___378___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___379_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___379___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___379___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___380_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___380___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___380___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___381_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___381___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___381___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___382_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___382___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___382___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___383_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___383___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___383___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___384_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___384___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___384___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___385_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___385___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___385___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___386_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___386___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___386___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___387_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___387___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___387___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___388_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___388___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___388___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___389_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___389___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___389___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___390_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___390___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___390___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___391_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___391___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___391___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___392_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___392___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___392___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___393_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___393___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___393___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___394_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___394___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___394___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___395_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___395___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___395___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___396_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___396___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___396___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___397_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___397___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___397___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___398_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___398___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___398___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___399_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___399___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___399___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___400_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___400___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___400___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___401_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___401___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___401___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___402_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___402___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___402___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___403_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___403___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___403___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___404_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___404___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___404___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___405_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___405___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___405___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___406_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___406___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___406___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___407_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___407___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___407___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___408_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___408___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___408___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___409_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___409___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___409___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___410_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___410___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___410___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___411_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___411___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___411___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___412_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___412___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___412___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___413_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___413___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___413___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___414_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___414___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___414___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___415_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___415___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___415___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___416_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___416___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___416___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___417_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___417___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___417___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___418_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___418___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___418___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___419_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___419___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___419___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___420_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___420___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___420___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___421_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___421___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___421___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___422_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___422___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___422___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___423_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___423___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___423___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___424_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___424___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___424___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___425_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___425___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___425___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___426_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___426___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___426___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___427_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___427___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___427___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___428_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___428___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___428___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___429_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___429___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___429___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___430_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___430___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___430___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___431_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___431___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___431___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___432_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___432___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___432___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___433_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___433___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___433___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___434_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___434___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___434___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___435_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___435___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___435___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___436_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___436___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___436___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___437_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___437___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___437___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___438_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___438___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___438___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___439_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___439___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___439___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___440_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___440___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___440___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___441_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___441___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___441___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___442_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___442___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___442___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___443_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___443___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___443___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___444_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___444___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___444___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___445_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___445___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___445___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___446_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___446___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___446___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___447_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___447___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___447___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___448_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___448___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___448___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___449_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___449___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___449___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___450_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___450___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___450___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___451_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___451___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___451___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___452_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___452___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___452___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___453_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___453___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___453___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___454_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___454___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___454___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___455_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___455___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___455___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___456_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___456___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___456___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___457_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___457___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___457___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___458_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___458___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___458___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___459_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___459___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___459___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___460_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___460___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___460___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___461_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___461___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___461___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___462_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___462___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___462___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___463_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___463___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___463___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___464_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___464___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___464___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___465_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___465___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___465___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___466_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___466___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___466___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___467_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___467___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___467___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___468_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___468___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___468___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___469_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___469___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___469___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___470_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___470___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___470___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___471_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___471___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___471___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___472_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___472___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___472___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___473_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___473___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___473___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___474_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___474___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___474___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___475_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___475___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___475___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___476_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___476___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___476___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___477_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___477___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___477___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___478_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___478___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___478___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___479_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___479___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___479___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___480_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___480___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___480___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___481_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___481___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___481___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___482_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___482___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___482___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___483_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___483___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___483___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___484_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___484___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___484___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___485_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___485___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___485___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___486_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___486___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___486___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___487_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___487___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___487___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___488_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___488___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___488___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___489_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___489___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___489___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___490_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___490___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___490___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___491_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___491___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___491___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___492_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___492___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___492___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___493_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___493___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___493___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___494_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___494___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___494___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___495_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___495___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___495___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___496_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___496___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___496___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___497_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___497___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___497___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___498_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___498___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___498___valid);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___499_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499);
}

uint32
dnx_data_mdb_app_db_fields_numeric_mdb_field___499___valid_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_mdb_app_db, dnx_data_mdb_app_db_submodule_fields, dnx_data_mdb_app_db_fields_define_numeric_mdb_field___499___valid);
}







shr_error_e
dnx_data_mdb_app_db_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "mdb_app_db";
    module_data->nof_submodules = _dnx_data_mdb_app_db_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data mdb_app_db submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_mdb_app_db_fields_init(unit, &module_data->submodules[dnx_data_mdb_app_db_submodule_fields]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_mdb_app_db_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_mdb_app_db_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_mdb_app_db_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE

