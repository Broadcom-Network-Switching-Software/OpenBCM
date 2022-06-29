
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DNX2_AOD_SIZES_H_

#define _DNX_DATA_INTERNAL_DNX2_AOD_SIZES_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_aod_sizes.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_dnx2_aod_sizes_submodule_AOD,
    dnx_data_dnx2_aod_sizes_submodule_dynamic_AOD,

    
    _dnx_data_dnx2_aod_sizes_submodule_nof
} dnx_data_dnx2_aod_sizes_submodule_e;








int dnx_data_dnx2_aod_sizes_AOD_feature_get(
    int unit,
    dnx_data_dnx2_aod_sizes_AOD_feature_e feature);



typedef enum
{
    dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE,
    dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER,
    dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG,

    
    _dnx_data_dnx2_aod_sizes_AOD_define_nof
} dnx_data_dnx2_aod_sizes_AOD_define_e;



uint32 dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_get(
    int unit);



typedef enum
{

    
    _dnx_data_dnx2_aod_sizes_AOD_table_nof
} dnx_data_dnx2_aod_sizes_AOD_table_e;









int dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_get(
    int unit,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_feature_e feature);



typedef enum
{
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_0,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_1,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_2,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_3,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_4,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_5,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_6,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_7,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_8,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_9,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_10,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_11,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_12,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_13,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_14,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_15,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_16,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_17,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_18,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_19,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_20,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_21,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_22,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_23,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_24,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_25,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_26,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_27,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_28,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_29,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_30,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_31,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_32,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_33,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_34,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_35,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_36,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_37,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_38,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_39,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_40,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_41,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_42,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_43,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_44,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_45,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_46,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_47,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_48,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_49,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_50,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_51,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_52,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_53,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_54,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_55,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_56,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_57,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_58,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_59,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_60,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_61,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_62,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_63,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_64,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_65,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_66,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_67,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_68,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_69,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_70,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_71,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_72,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_73,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_74,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_75,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_76,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_77,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_78,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_79,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_80,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_81,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_82,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_83,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_84,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_85,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_86,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_87,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_88,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_89,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_90,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_91,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_92,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_93,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_94,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_95,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_96,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_97,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_98,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_99,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_100,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_101,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_102,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_103,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_104,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_105,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_106,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_107,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_108,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_109,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_110,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_111,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_112,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_113,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_114,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_115,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_116,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_117,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_118,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_119,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_120,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_121,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_122,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_123,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_124,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_125,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_126,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_127,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_128,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_129,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_130,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_131,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_132,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_133,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_134,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_135,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_136,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_137,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_138,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_139,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_140,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_141,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_142,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_143,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_144,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_145,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_146,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_147,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_148,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_149,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_150,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_151,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_152,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_153,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_154,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_155,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_156,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_157,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_158,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_159,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_160,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_161,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_162,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_163,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_164,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_165,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_166,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_167,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_168,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_169,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_170,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_171,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_172,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_173,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_174,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_175,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_176,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_177,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_178,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_179,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_180,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_181,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_182,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_183,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_184,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_185,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_186,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_187,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_188,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_189,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_190,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_191,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_192,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_193,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_194,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_195,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_196,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_197,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_198,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_199,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_200,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_201,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_202,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_203,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_204,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_205,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_206,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_207,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_208,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_209,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_210,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_211,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_212,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_213,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_214,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_215,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_216,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_217,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_218,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_219,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_220,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_221,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_222,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_223,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_224,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_225,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_226,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_227,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_228,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_229,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_230,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_231,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_232,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_233,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_234,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_235,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_236,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_237,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_238,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_239,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_240,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_241,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_242,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_243,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_244,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_245,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_246,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_247,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_248,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_249,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_250,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_251,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_252,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_253,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_254,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_255,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_256,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_257,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_258,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_259,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_260,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_261,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_262,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_263,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_264,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_265,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_266,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_267,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_268,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_269,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_270,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_271,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_272,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_273,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_274,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_275,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_276,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_277,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_278,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_279,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_280,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_281,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_282,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_283,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_284,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_285,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_286,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_287,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_288,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_289,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_290,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_291,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_292,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_293,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_294,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_295,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_296,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_297,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_298,
    dnx_data_dnx2_aod_sizes_dynamic_AOD_define_aod_299,

    
    _dnx_data_dnx2_aod_sizes_dynamic_AOD_define_nof
} dnx_data_dnx2_aod_sizes_dynamic_AOD_define_e;



uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_0_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_1_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_2_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_3_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_4_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_5_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_6_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_7_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_8_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_9_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_10_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_11_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_12_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_13_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_14_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_15_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_16_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_17_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_18_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_19_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_20_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_21_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_22_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_23_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_24_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_25_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_26_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_27_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_28_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_29_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_30_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_31_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_32_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_33_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_34_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_35_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_36_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_37_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_38_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_39_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_40_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_41_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_42_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_43_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_44_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_45_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_46_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_47_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_48_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_49_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_50_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_51_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_52_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_53_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_54_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_55_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_56_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_57_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_58_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_59_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_60_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_61_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_62_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_63_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_64_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_65_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_66_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_67_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_68_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_69_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_70_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_71_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_72_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_73_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_74_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_75_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_76_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_77_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_78_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_79_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_80_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_81_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_82_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_83_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_84_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_85_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_86_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_87_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_88_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_89_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_90_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_91_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_92_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_93_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_94_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_95_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_96_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_97_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_98_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_99_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_100_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_101_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_102_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_103_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_104_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_105_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_106_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_107_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_108_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_109_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_110_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_111_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_112_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_113_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_114_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_115_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_116_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_117_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_118_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_119_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_120_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_121_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_122_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_123_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_124_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_125_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_126_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_127_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_128_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_129_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_130_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_131_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_132_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_133_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_134_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_135_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_136_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_137_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_138_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_139_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_140_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_141_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_142_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_143_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_144_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_145_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_146_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_147_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_148_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_149_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_150_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_151_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_152_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_153_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_154_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_155_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_156_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_157_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_158_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_159_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_160_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_161_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_162_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_163_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_164_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_165_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_166_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_167_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_168_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_169_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_170_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_171_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_172_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_173_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_174_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_175_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_176_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_177_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_178_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_179_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_180_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_181_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_182_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_183_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_184_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_185_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_186_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_187_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_188_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_189_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_190_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_191_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_192_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_193_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_194_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_195_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_196_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_197_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_198_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_199_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_200_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_201_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_202_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_203_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_204_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_205_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_206_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_207_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_208_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_209_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_210_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_211_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_212_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_213_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_214_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_215_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_216_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_217_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_218_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_219_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_220_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_221_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_222_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_223_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_224_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_225_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_226_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_227_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_228_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_229_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_230_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_231_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_232_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_233_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_234_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_235_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_236_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_237_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_238_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_239_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_240_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_241_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_242_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_243_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_244_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_245_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_246_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_247_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_248_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_249_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_250_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_251_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_252_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_253_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_254_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_255_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_256_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_257_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_258_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_259_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_260_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_261_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_262_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_263_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_264_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_265_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_266_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_267_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_268_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_269_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_270_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_271_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_272_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_273_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_274_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_275_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_276_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_277_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_278_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_279_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_280_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_281_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_282_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_283_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_284_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_285_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_286_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_287_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_288_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_289_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_290_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_291_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_292_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_293_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_294_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_295_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_296_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_297_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_298_get(
    int unit);


uint32 dnx_data_dnx2_aod_sizes_dynamic_AOD_aod_299_get(
    int unit);



typedef enum
{

    
    _dnx_data_dnx2_aod_sizes_dynamic_AOD_table_nof
} dnx_data_dnx2_aod_sizes_dynamic_AOD_table_e;






shr_error_e dnx_data_dnx2_aod_sizes_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

