
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_DNX2_AOD_SIZES_H_

#define _DNX_DATA_DNX2_AOD_SIZES_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_max_dnx2_aod_sizes.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_dnx2_aod_sizes_init(
    int unit);







typedef enum
{

    
    _dnx_data_dnx2_aod_sizes_AOD_feature_nof
} dnx_data_dnx2_aod_sizes_AOD_feature_e;



typedef int(
    *dnx_data_dnx2_aod_sizes_AOD_feature_get_f) (
    int unit,
    dnx_data_dnx2_aod_sizes_AOD_feature_e feature);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dnx2_aod_sizes_AOD_feature_get_f feature_get;
    
    dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_get_f ETPP_TRAP_CONTEXT_PORT_PROFILE_get;
    
    dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_get_f TDM_TRAP_CANDIDATE_HEADER_get;
    
    dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get_f TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get;
} dnx_data_if_dnx2_aod_sizes_AOD_t;







typedef enum
{

    
    _dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_nof
} dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_e;



typedef int(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_get_f) (
    int unit,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_e feature);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_0_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_3_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_4_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_5_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_6_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_7_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_8_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_9_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_10_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_11_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_12_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_13_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_14_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_15_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_16_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_17_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_18_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_19_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_20_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_21_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_22_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_23_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_24_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_25_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_26_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_27_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_28_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_29_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_30_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_31_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_33_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_34_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_35_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_36_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_37_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_38_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_39_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_40_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_41_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_42_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_43_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_44_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_45_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_46_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_47_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_48_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_49_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_50_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_51_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_52_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_53_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_54_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_55_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_56_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_57_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_58_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_59_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_60_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_61_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_62_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_63_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_64_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_65_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_66_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_67_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_68_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_69_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_70_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_71_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_72_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_73_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_74_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_75_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_76_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_77_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_78_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_79_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_80_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_81_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_82_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_83_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_84_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_85_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_86_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_87_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_88_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_89_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_90_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_91_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_92_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_93_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_94_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_95_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_96_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_97_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_98_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_99_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_100_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_101_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_102_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_103_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_104_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_105_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_106_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_107_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_108_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_109_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_110_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_111_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_112_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_113_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_114_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_115_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_116_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_117_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_118_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_119_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_120_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_121_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_122_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_123_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_124_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_125_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_126_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_127_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_128_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_129_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_130_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_131_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_132_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_133_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_134_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_135_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_136_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_137_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_138_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_139_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_140_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_141_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_142_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_143_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_144_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_145_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_146_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_147_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_148_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_149_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_150_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_151_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_152_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_153_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_154_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_155_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_156_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_157_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_158_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_159_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_160_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_161_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_162_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_163_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_164_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_165_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_166_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_167_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_168_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_169_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_170_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_171_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_172_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_173_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_174_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_175_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_176_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_177_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_178_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_179_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_180_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_181_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_182_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_183_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_184_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_185_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_186_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_187_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_188_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_189_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_190_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_191_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_192_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_193_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_194_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_195_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_196_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_197_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_198_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_199_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_200_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_201_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_202_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_203_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_204_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_205_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_206_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_207_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_208_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_209_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_210_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_211_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_212_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_213_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_214_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_215_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_216_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_217_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_218_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_219_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_220_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_221_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_222_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_223_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_224_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_225_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_226_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_227_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_228_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_229_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_230_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_231_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_232_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_233_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_234_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_235_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_236_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_237_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_238_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_239_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_240_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_241_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_242_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_243_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_244_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_245_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_246_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_247_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_248_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_249_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_250_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_251_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_252_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_253_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_254_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_255_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_256_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_257_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_258_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_259_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_260_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_261_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_262_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_263_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_264_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_265_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_266_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_267_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_268_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_269_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_270_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_271_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_272_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_273_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_274_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_275_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_276_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_277_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_278_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_279_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_280_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_281_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_282_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_283_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_284_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_285_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_286_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_287_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_288_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_289_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_290_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_291_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_292_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_293_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_294_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_295_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_296_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_297_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_298_get_f) (
    int unit);


typedef uint32(
    *dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_299_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_get_f feature_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_0_get_f aod_0_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_1_get_f aod_1_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_2_get_f aod_2_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_3_get_f aod_3_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_4_get_f aod_4_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_5_get_f aod_5_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_6_get_f aod_6_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_7_get_f aod_7_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_8_get_f aod_8_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_9_get_f aod_9_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_10_get_f aod_10_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_11_get_f aod_11_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_12_get_f aod_12_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_13_get_f aod_13_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_14_get_f aod_14_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_15_get_f aod_15_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_16_get_f aod_16_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_17_get_f aod_17_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_18_get_f aod_18_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_19_get_f aod_19_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_20_get_f aod_20_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_21_get_f aod_21_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_22_get_f aod_22_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_23_get_f aod_23_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_24_get_f aod_24_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_25_get_f aod_25_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_26_get_f aod_26_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_27_get_f aod_27_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_28_get_f aod_28_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_29_get_f aod_29_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_30_get_f aod_30_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_31_get_f aod_31_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_32_get_f aod_32_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_33_get_f aod_33_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_34_get_f aod_34_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_35_get_f aod_35_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_36_get_f aod_36_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_37_get_f aod_37_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_38_get_f aod_38_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_39_get_f aod_39_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_40_get_f aod_40_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_41_get_f aod_41_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_42_get_f aod_42_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_43_get_f aod_43_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_44_get_f aod_44_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_45_get_f aod_45_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_46_get_f aod_46_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_47_get_f aod_47_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_48_get_f aod_48_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_49_get_f aod_49_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_50_get_f aod_50_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_51_get_f aod_51_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_52_get_f aod_52_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_53_get_f aod_53_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_54_get_f aod_54_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_55_get_f aod_55_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_56_get_f aod_56_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_57_get_f aod_57_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_58_get_f aod_58_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_59_get_f aod_59_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_60_get_f aod_60_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_61_get_f aod_61_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_62_get_f aod_62_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_63_get_f aod_63_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_64_get_f aod_64_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_65_get_f aod_65_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_66_get_f aod_66_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_67_get_f aod_67_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_68_get_f aod_68_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_69_get_f aod_69_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_70_get_f aod_70_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_71_get_f aod_71_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_72_get_f aod_72_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_73_get_f aod_73_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_74_get_f aod_74_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_75_get_f aod_75_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_76_get_f aod_76_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_77_get_f aod_77_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_78_get_f aod_78_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_79_get_f aod_79_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_80_get_f aod_80_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_81_get_f aod_81_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_82_get_f aod_82_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_83_get_f aod_83_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_84_get_f aod_84_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_85_get_f aod_85_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_86_get_f aod_86_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_87_get_f aod_87_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_88_get_f aod_88_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_89_get_f aod_89_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_90_get_f aod_90_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_91_get_f aod_91_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_92_get_f aod_92_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_93_get_f aod_93_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_94_get_f aod_94_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_95_get_f aod_95_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_96_get_f aod_96_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_97_get_f aod_97_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_98_get_f aod_98_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_99_get_f aod_99_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_100_get_f aod_100_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_101_get_f aod_101_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_102_get_f aod_102_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_103_get_f aod_103_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_104_get_f aod_104_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_105_get_f aod_105_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_106_get_f aod_106_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_107_get_f aod_107_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_108_get_f aod_108_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_109_get_f aod_109_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_110_get_f aod_110_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_111_get_f aod_111_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_112_get_f aod_112_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_113_get_f aod_113_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_114_get_f aod_114_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_115_get_f aod_115_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_116_get_f aod_116_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_117_get_f aod_117_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_118_get_f aod_118_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_119_get_f aod_119_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_120_get_f aod_120_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_121_get_f aod_121_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_122_get_f aod_122_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_123_get_f aod_123_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_124_get_f aod_124_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_125_get_f aod_125_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_126_get_f aod_126_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_127_get_f aod_127_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_128_get_f aod_128_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_129_get_f aod_129_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_130_get_f aod_130_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_131_get_f aod_131_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_132_get_f aod_132_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_133_get_f aod_133_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_134_get_f aod_134_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_135_get_f aod_135_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_136_get_f aod_136_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_137_get_f aod_137_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_138_get_f aod_138_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_139_get_f aod_139_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_140_get_f aod_140_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_141_get_f aod_141_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_142_get_f aod_142_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_143_get_f aod_143_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_144_get_f aod_144_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_145_get_f aod_145_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_146_get_f aod_146_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_147_get_f aod_147_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_148_get_f aod_148_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_149_get_f aod_149_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_150_get_f aod_150_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_151_get_f aod_151_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_152_get_f aod_152_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_153_get_f aod_153_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_154_get_f aod_154_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_155_get_f aod_155_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_156_get_f aod_156_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_157_get_f aod_157_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_158_get_f aod_158_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_159_get_f aod_159_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_160_get_f aod_160_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_161_get_f aod_161_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_162_get_f aod_162_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_163_get_f aod_163_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_164_get_f aod_164_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_165_get_f aod_165_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_166_get_f aod_166_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_167_get_f aod_167_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_168_get_f aod_168_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_169_get_f aod_169_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_170_get_f aod_170_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_171_get_f aod_171_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_172_get_f aod_172_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_173_get_f aod_173_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_174_get_f aod_174_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_175_get_f aod_175_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_176_get_f aod_176_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_177_get_f aod_177_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_178_get_f aod_178_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_179_get_f aod_179_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_180_get_f aod_180_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_181_get_f aod_181_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_182_get_f aod_182_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_183_get_f aod_183_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_184_get_f aod_184_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_185_get_f aod_185_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_186_get_f aod_186_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_187_get_f aod_187_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_188_get_f aod_188_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_189_get_f aod_189_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_190_get_f aod_190_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_191_get_f aod_191_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_192_get_f aod_192_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_193_get_f aod_193_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_194_get_f aod_194_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_195_get_f aod_195_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_196_get_f aod_196_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_197_get_f aod_197_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_198_get_f aod_198_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_199_get_f aod_199_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_200_get_f aod_200_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_201_get_f aod_201_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_202_get_f aod_202_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_203_get_f aod_203_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_204_get_f aod_204_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_205_get_f aod_205_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_206_get_f aod_206_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_207_get_f aod_207_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_208_get_f aod_208_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_209_get_f aod_209_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_210_get_f aod_210_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_211_get_f aod_211_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_212_get_f aod_212_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_213_get_f aod_213_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_214_get_f aod_214_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_215_get_f aod_215_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_216_get_f aod_216_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_217_get_f aod_217_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_218_get_f aod_218_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_219_get_f aod_219_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_220_get_f aod_220_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_221_get_f aod_221_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_222_get_f aod_222_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_223_get_f aod_223_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_224_get_f aod_224_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_225_get_f aod_225_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_226_get_f aod_226_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_227_get_f aod_227_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_228_get_f aod_228_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_229_get_f aod_229_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_230_get_f aod_230_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_231_get_f aod_231_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_232_get_f aod_232_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_233_get_f aod_233_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_234_get_f aod_234_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_235_get_f aod_235_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_236_get_f aod_236_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_237_get_f aod_237_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_238_get_f aod_238_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_239_get_f aod_239_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_240_get_f aod_240_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_241_get_f aod_241_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_242_get_f aod_242_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_243_get_f aod_243_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_244_get_f aod_244_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_245_get_f aod_245_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_246_get_f aod_246_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_247_get_f aod_247_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_248_get_f aod_248_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_249_get_f aod_249_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_250_get_f aod_250_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_251_get_f aod_251_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_252_get_f aod_252_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_253_get_f aod_253_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_254_get_f aod_254_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_255_get_f aod_255_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_256_get_f aod_256_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_257_get_f aod_257_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_258_get_f aod_258_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_259_get_f aod_259_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_260_get_f aod_260_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_261_get_f aod_261_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_262_get_f aod_262_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_263_get_f aod_263_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_264_get_f aod_264_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_265_get_f aod_265_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_266_get_f aod_266_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_267_get_f aod_267_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_268_get_f aod_268_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_269_get_f aod_269_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_270_get_f aod_270_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_271_get_f aod_271_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_272_get_f aod_272_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_273_get_f aod_273_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_274_get_f aod_274_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_275_get_f aod_275_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_276_get_f aod_276_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_277_get_f aod_277_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_278_get_f aod_278_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_279_get_f aod_279_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_280_get_f aod_280_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_281_get_f aod_281_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_282_get_f aod_282_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_283_get_f aod_283_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_284_get_f aod_284_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_285_get_f aod_285_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_286_get_f aod_286_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_287_get_f aod_287_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_288_get_f aod_288_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_289_get_f aod_289_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_290_get_f aod_290_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_291_get_f aod_291_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_292_get_f aod_292_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_293_get_f aod_293_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_294_get_f aod_294_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_295_get_f aod_295_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_296_get_f aod_296_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_297_get_f aod_297_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_298_get_f aod_298_get;
    
    dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_299_get_f aod_299_get;
} dnx_data_if_dnx2_aod_sizes_dynamic_AOD_t;





typedef struct
{
    
    dnx_data_if_dnx2_aod_sizes_AOD_t AOD;
    
    dnx_data_if_dnx2_aod_sizes_dynamic_AOD_t dynamic_AOD;
} dnx_data_if_dnx2_aod_sizes_t;




extern dnx_data_if_dnx2_aod_sizes_t dnx_data_dnx2_aod_sizes;


#endif 

