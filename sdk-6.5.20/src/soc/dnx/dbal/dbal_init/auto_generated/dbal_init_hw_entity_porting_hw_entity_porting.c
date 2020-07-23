
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>

shr_error_e
_dbal_init_mapping_0_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_JR2_B0)
    {
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, INVALIDm , INVALIDm );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_mapping_1_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2C_A0 || DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, IPPA_MAP_PORT_TO_MEMBERSHIP_IF_VECm , IPPA_MAP_PORT_TO_MEMBERSHIP_IF_1m );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, IPPF_MAP_PORT_TO_MEMBERSHIP_IF_VECm , IPPF_MAP_PORT_TO_MEMBERSHIP_IFm );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, IPPA_VLAN_MEMBERSHIP_IF_ATTRm , IPPA_VLAN_MEMBERSHIP_IF_ATTR_1m );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, IPPA_VD_TO_VLAN_CLASSIFICATION_PROFILEm , IPPA_VD_TO_VLAN_CLASSIFICATION_PROFILE_1m );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, ERPP_PER_PORT_TABLEm , ERPP_PER_PP_PORT_TABLEm );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_mapping_2_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, CFC_NIF_PFC_MAPm , CFC_NIF_ETH_RX_PFC_MAPm );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, CFC_ILKN_PP_RX_4_CALm , CFC_PP_RX_4_CALm );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, CGM_MEMBER_TO_DSP_MAPPINGm , CGM_LAG_MAPPINGm );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_mapping_3_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_db_init_hw_entity_porting_reg_mapping_set(unit, hl_porting_info, IPPD_IN_LIF_PROFILE_MAPr , IPPD_JR_2_TO_JR_IN_LIF_PROFILE_MAPr );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_mapping_4_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    if (DBAL_IS_J2P_A0)
    {
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, CFC_NIF_PFC_MAPm , CFC_NIF_ETH_RX_PFC_MAPm );
        dbal_db_init_hw_entity_porting_mem_mapping_set(unit, hl_porting_info, CFC_ILKN_PP_RX_4_CALm , CFC_PP_RX_4_CALm );
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_hw_entity_porting_hw_entity_porting_init(
int unit,
hl_porting_info_t * hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_mapping_0_init(unit, hl_porting_info));
    SHR_IF_ERR_EXIT(_dbal_init_mapping_1_init(unit, hl_porting_info));
    SHR_IF_ERR_EXIT(_dbal_init_mapping_2_init(unit, hl_porting_info));
    SHR_IF_ERR_EXIT(_dbal_init_mapping_3_init(unit, hl_porting_info));
    SHR_IF_ERR_EXIT(_dbal_init_mapping_4_init(unit, hl_porting_info));
exit:
    SHR_FUNC_EXIT;
}
