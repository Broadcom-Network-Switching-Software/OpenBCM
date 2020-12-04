/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MMU/Cosq soc routines
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/debug.h>

#if defined(BCM_KATANA2_SUPPORT)
#include <soc/katana2.h>

#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>

#if defined(BCM_METROLITE_SUPPORT)
#include <soc/metrolite.h>
#include <bcm_int/esw/subport.h>
#include <bcm_int/esw/katana2.h>
#endif
#endif

#define _SOC_COE_PORT_TYPE_CASCADED             4
#define _SOC_COE_PORT_TYPE_CASCADED_LINKPHY     0
#define _SOC_COE_PORT_TYPE_CASCADED_SUBTAG      1

typedef struct lp_lls_reg_s {
    soc_reg_t   reg;
    soc_field_t field1;
    soc_field_t field2;
    soc_field_t field3;
    soc_field_t field4;
} lp_lls_reg_t;

lp_lls_reg_t lp_stream_map[] = {
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_001_000r, S1_MAPPING_DST_000f,
                                                        S1_MAPPING_DST_001f, 
                                                        INVALIDf,
                                                        INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_003_002r, S1_MAPPING_DST_002f,
                                                            S1_MAPPING_DST_003f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_005_004r, S1_MAPPING_DST_004f,
                                                            S1_MAPPING_DST_005f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_007_006r, S1_MAPPING_DST_006f,
                                                            S1_MAPPING_DST_007f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_009_008r, S1_MAPPING_DST_008f,
                                                            S1_MAPPING_DST_009f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_011_010r, S1_MAPPING_DST_010f,
                                                            S1_MAPPING_DST_011f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_013_012r, S1_MAPPING_DST_012f,
                                                            S1_MAPPING_DST_013f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_015_014r, S1_MAPPING_DST_014f,
                                                            S1_MAPPING_DST_015f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_017_016r, S1_MAPPING_DST_016f,
                                                            S1_MAPPING_DST_017f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_019_018r, S1_MAPPING_DST_018f,
                                                            S1_MAPPING_DST_019f,
                                                            INVALIDf,
                                                            INVALIDf },
    { LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_021_020r, S1_MAPPING_DST_020f,
                                                            S1_MAPPING_DST_021f,
                                                            INVALIDf,
                                                            INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_023_022r, S1_MAPPING_DST_022f,
                                                        S1_MAPPING_DST_023f, 
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_025_024r, S1_MAPPING_DST_024f,
                                                        S1_MAPPING_DST_025f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_027_026r, S1_MAPPING_DST_026f,
                                                        S1_MAPPING_DST_027f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_029_028r, S1_MAPPING_DST_028f,
                                                        S1_MAPPING_DST_029f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_031_030r, S1_MAPPING_DST_030f,
                                                        S1_MAPPING_DST_031f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_033_032r, S1_MAPPING_DST_032f,
                                                        S1_MAPPING_DST_033f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_035_034r, S1_MAPPING_DST_034f,
                                                        S1_MAPPING_DST_035f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_037_036r, S1_MAPPING_DST_036f,
                                                        S1_MAPPING_DST_037f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_039_038r, S1_MAPPING_DST_038f,
                                                        S1_MAPPING_DST_039f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_041_040r, S1_MAPPING_DST_040f,
                                                        S1_MAPPING_DST_041f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_043_042r, S1_MAPPING_DST_042f,
                                                        S1_MAPPING_DST_043f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_045_044r, S1_MAPPING_DST_044f,
                                                        S1_MAPPING_DST_045f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_047_046r, S1_MAPPING_DST_046f,
                                                        S1_MAPPING_DST_047f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_049_048r, S1_MAPPING_DST_048f,
                                                        S1_MAPPING_DST_049f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_051_050r, S1_MAPPING_DST_050f,
                                                        S1_MAPPING_DST_051f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_053_052r, S1_MAPPING_DST_052f,
                                                        S1_MAPPING_DST_053f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_055_054r, S1_MAPPING_DST_054f,
                                                        S1_MAPPING_DST_055f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_057_056r, S1_MAPPING_DST_056f,
                                                        S1_MAPPING_DST_057f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_059_058r, S1_MAPPING_DST_058f,
                                                        S1_MAPPING_DST_059f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_061_060r, S1_MAPPING_DST_060f,
                                                        S1_MAPPING_DST_061f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_063_062r, S1_MAPPING_DST_062f,
                                                        S1_MAPPING_DST_063f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_065_064r, S1_MAPPING_DST_064f,
                                                        S1_MAPPING_DST_065f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_067_066r, S1_MAPPING_DST_066f,
                                                        S1_MAPPING_DST_067f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_069_068r, S1_MAPPING_DST_068f,
                                                        S1_MAPPING_DST_069f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_071_070r, S1_MAPPING_DST_070f,
                                                        S1_MAPPING_DST_071f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_073_072r, S1_MAPPING_DST_072f,
                                                        S1_MAPPING_DST_073f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_075_074r, S1_MAPPING_DST_074f,
                                                        S1_MAPPING_DST_075f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_077_076r, S1_MAPPING_DST_076f,
                                                        S1_MAPPING_DST_077f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_079_078r, S1_MAPPING_DST_078f,
                                                        S1_MAPPING_DST_079f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_081_080r, S1_MAPPING_DST_080f,
                                                        S1_MAPPING_DST_081f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_083_082r, S1_MAPPING_DST_082f,
                                                        S1_MAPPING_DST_083f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_085_084r, S1_MAPPING_DST_084f,
                                                        S1_MAPPING_DST_085f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_087_086r, S1_MAPPING_DST_086f,
                                                        S1_MAPPING_DST_087f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_089_088r, S1_MAPPING_DST_088f,
                                                        S1_MAPPING_DST_089f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_091_090r, S1_MAPPING_DST_090f,
                                                        S1_MAPPING_DST_091f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_093_092r, S1_MAPPING_DST_092f,
                                                        S1_MAPPING_DST_093f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_095_094r, S1_MAPPING_DST_094f,
                                                        S1_MAPPING_DST_095f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_097_096r, S1_MAPPING_DST_096f,
                                                        S1_MAPPING_DST_097f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_099_098r, S1_MAPPING_DST_098f,
                                                        S1_MAPPING_DST_099f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_101_100r, S1_MAPPING_DST_100f,
                                                        S1_MAPPING_DST_101f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_103_102r, S1_MAPPING_DST_102f,
                                                        S1_MAPPING_DST_103f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_105_104r, S1_MAPPING_DST_104f,
                                                        S1_MAPPING_DST_105f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_107_106r, S1_MAPPING_DST_106f,
                                                        S1_MAPPING_DST_107f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_109_108r, S1_MAPPING_DST_108f,
                                                        S1_MAPPING_DST_109f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_111_110r, S1_MAPPING_DST_110f,
                                                        S1_MAPPING_DST_111f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_113_112r, S1_MAPPING_DST_112f,
                                                        S1_MAPPING_DST_113f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_115_114r, S1_MAPPING_DST_114f,
                                                        S1_MAPPING_DST_115f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_117_116r, S1_MAPPING_DST_116f,
                                                        S1_MAPPING_DST_117f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_119_118r, S1_MAPPING_DST_118f,
                                                        S1_MAPPING_DST_119f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_121_120r, S1_MAPPING_DST_120f,
                                                        S1_MAPPING_DST_121f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_123_122r, S1_MAPPING_DST_122f,
                                                        S1_MAPPING_DST_123f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_125_124r, S1_MAPPING_DST_124f,
                                                        S1_MAPPING_DST_125f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_127_126r, S1_MAPPING_DST_126f,
                                                        S1_MAPPING_DST_127f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_129_128r, S1_MAPPING_DST_128f,
                                                        S1_MAPPING_DST_129f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_131_130r, S1_MAPPING_DST_130f,
                                                        S1_MAPPING_DST_131f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_133_132r, S1_MAPPING_DST_132f,
                                                        S1_MAPPING_DST_133f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_135_134r, S1_MAPPING_DST_134f,
                                                        S1_MAPPING_DST_135f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_137_136r, S1_MAPPING_DST_136f,
                                                        S1_MAPPING_DST_137f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_139_138r, S1_MAPPING_DST_138f,
                                                        S1_MAPPING_DST_139f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_141_140r, S1_MAPPING_DST_140f,
                                                        S1_MAPPING_DST_141f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_143_142r, S1_MAPPING_DST_142f,
                                                        S1_MAPPING_DST_143f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_145_144r, S1_MAPPING_DST_144f,
                                                        S1_MAPPING_DST_145f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_147_146r, S1_MAPPING_DST_146f,
                                                        S1_MAPPING_DST_147f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_149_148r, S1_MAPPING_DST_148f,
                                                        S1_MAPPING_DST_149f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_151_150r, S1_MAPPING_DST_150f,
                                                        S1_MAPPING_DST_151f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_153_152r, S1_MAPPING_DST_152f,
                                                        S1_MAPPING_DST_153f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_155_154r, S1_MAPPING_DST_154f,
                                                        S1_MAPPING_DST_155f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_157_156r, S1_MAPPING_DST_156f,
                                                        S1_MAPPING_DST_157f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_159_158r, S1_MAPPING_DST_158f,
                                                        S1_MAPPING_DST_159f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_161_160r, S1_MAPPING_DST_160f,
                                                        S1_MAPPING_DST_161f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_163_162r, S1_MAPPING_DST_162f,
                                                        S1_MAPPING_DST_163f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_165_164r, S1_MAPPING_DST_164f,
                                                        S1_MAPPING_DST_165f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_167_166r, S1_MAPPING_DST_166f,
                                                        S1_MAPPING_DST_167f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_169_168r, S1_MAPPING_DST_168f,
                                                        S1_MAPPING_DST_169f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_171_170r, S1_MAPPING_DST_170f,
                                                        S1_MAPPING_DST_171f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_173_172r, S1_MAPPING_DST_172f,
                                                        S1_MAPPING_DST_173f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_175_174r, S1_MAPPING_DST_174f,
                                                        S1_MAPPING_DST_175f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_177_176r, S1_MAPPING_DST_176f,
                                                        S1_MAPPING_DST_177f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_179_178r, S1_MAPPING_DST_178f,
                                                        S1_MAPPING_DST_179f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_181_180r, S1_MAPPING_DST_180f,
                                                        S1_MAPPING_DST_181f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_183_182r, S1_MAPPING_DST_182f,
                                                        S1_MAPPING_DST_183f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_185_184r, S1_MAPPING_DST_184f,
                                                        S1_MAPPING_DST_185f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_187_186r, S1_MAPPING_DST_186f,
                                                        S1_MAPPING_DST_187f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_189_188r, S1_MAPPING_DST_188f,
                                                        S1_MAPPING_DST_189f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_191_190r, S1_MAPPING_DST_190f,
                                                        S1_MAPPING_DST_191f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_193_192r, S1_MAPPING_DST_192f,
                                                        S1_MAPPING_DST_193f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_195_194r, S1_MAPPING_DST_194f,
                                                        S1_MAPPING_DST_195f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_197_196r, S1_MAPPING_DST_196f,
                                                        S1_MAPPING_DST_197f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_199_198r, S1_MAPPING_DST_198f,
                                                        S1_MAPPING_DST_199f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_201_200r, S1_MAPPING_DST_200f,
                                                        S1_MAPPING_DST_201f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_203_202r, S1_MAPPING_DST_202f,
                                                        S1_MAPPING_DST_203f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_205_204r, S1_MAPPING_DST_204f,
                                                        S1_MAPPING_DST_205f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_207_206r, S1_MAPPING_DST_206f,
                                                        S1_MAPPING_DST_207f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_209_208r, S1_MAPPING_DST_208f,
                                                        S1_MAPPING_DST_209f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_211_210r, S1_MAPPING_DST_210f,
                                                        S1_MAPPING_DST_211f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_213_212r, S1_MAPPING_DST_212f,
                                                        S1_MAPPING_DST_213f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_215_214r, S1_MAPPING_DST_214f,
                                                        S1_MAPPING_DST_215f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_217_216r, S1_MAPPING_DST_216f,
                                                        S1_MAPPING_DST_217f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_219_218r, S1_MAPPING_DST_218f,
                                                        S1_MAPPING_DST_219f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_221_220r, S1_MAPPING_DST_220f,
                                                        S1_MAPPING_DST_221f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_223_222r, S1_MAPPING_DST_222f,
                                                        S1_MAPPING_DST_223f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_225_224r, S1_MAPPING_DST_224f,
                                                        S1_MAPPING_DST_225f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_227_226r, S1_MAPPING_DST_226f,
                                                        S1_MAPPING_DST_227f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_229_228r, S1_MAPPING_DST_228f,
                                                        S1_MAPPING_DST_229f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_231_230r, S1_MAPPING_DST_230f,
                                                        S1_MAPPING_DST_231f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_233_232r, S1_MAPPING_DST_232f,
                                                        S1_MAPPING_DST_233f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_235_234r, S1_MAPPING_DST_234f,
                                                        S1_MAPPING_DST_235f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_237_236r, S1_MAPPING_DST_236f,
                                                        S1_MAPPING_DST_237f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_239_238r, S1_MAPPING_DST_238f,
                                                        S1_MAPPING_DST_239f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_241_240r, S1_MAPPING_DST_240f,
                                                        S1_MAPPING_DST_241f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_243_242r, S1_MAPPING_DST_242f,
                                                        S1_MAPPING_DST_243f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_245_244r, S1_MAPPING_DST_244f,
                                                        S1_MAPPING_DST_245f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_247_246r, S1_MAPPING_DST_246f,
                                                        S1_MAPPING_DST_247f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_249_248r, S1_MAPPING_DST_248f,
                                                        S1_MAPPING_DST_249f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_251_250r, S1_MAPPING_DST_250f,
                                                        S1_MAPPING_DST_251f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_253_252r, S1_MAPPING_DST_252f,
                                                        S1_MAPPING_DST_253f,
                                                        INVALIDf,
                                                        INVALIDf },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_255_254r, S1_MAPPING_DST_254f,
                                                        S1_MAPPING_DST_255f,
                                                        INVALIDf,
                                                        INVALIDf },
};

lp_lls_reg_t port_lp_enable[] = {
    { INVALIDr, INVALIDf, INVALIDf ,INVALIDf, INVALIDf},
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_01f, INVALIDf, 
    INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_02f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_03f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_04f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_05f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_06f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_07f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_08f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_09f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_10f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_11f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_12f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_13f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_14f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_15f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_16f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_17f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_18f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_19f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_20f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_21f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_22f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_23f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_24f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_25f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_26f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_27f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_28f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_29f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_30f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_LOWERr, LINKPHY_ENABLE_31f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_32f, INVALIDf,
        INVALIDf, INVALIDf }, 
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_33f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_34f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_35f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_36f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_37f, INVALIDf,
        INVALIDf, INVALIDf }, 
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_38f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_39f, INVALIDf,
        INVALIDf, INVALIDf },
    { LLS_PORT_COE_LINKPHY_ENABLE_UPPERr, LINKPHY_ENABLE_40f, INVALIDf,
        INVALIDf, INVALIDf },    
};

lp_lls_reg_t port_lp_config[] = {
    { INVALIDr, INVALIDf, INVALIDf, INVALIDf, INVALIDf },    
    { LLS_PORT_01_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_02_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_03_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_04_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_05_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_06_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_07_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_08_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_09_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_10_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_11_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_12_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_13_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_14_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_15_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_16_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_17_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_18_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_19_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_20_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_21_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_22_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_23_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_24_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_25_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_26_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_27_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_28_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_29_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_30_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_31_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_32_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },    
    { LLS_PORT_33_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_34_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_35_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_36_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_37_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_38_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_39_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_40_LINKPHY_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
};

#if defined(BCM_SABER2_SUPPORT)
lp_lls_reg_t sb2_lp_stream_map[] = {
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_003_000r, S1_MAPPING_DST_000f,
                                                        S1_MAPPING_DST_001f, 
                                                        S1_MAPPING_DST_002f,
                                                        S1_MAPPING_DST_003f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_007_004r, S1_MAPPING_DST_004f,
                                                        S1_MAPPING_DST_005f, 
                                                        S1_MAPPING_DST_006f,
                                                        S1_MAPPING_DST_007f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_011_008r, S1_MAPPING_DST_008f,
                                                        S1_MAPPING_DST_009f, 
                                                        S1_MAPPING_DST_010f,
                                                        S1_MAPPING_DST_011f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_015_012r, S1_MAPPING_DST_012f,
                                                        S1_MAPPING_DST_013f, 
                                                        S1_MAPPING_DST_014f,
                                                        S1_MAPPING_DST_015f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_019_016r, S1_MAPPING_DST_016f,
                                                        S1_MAPPING_DST_017f, 
                                                        S1_MAPPING_DST_018f,
                                                        S1_MAPPING_DST_019f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_023_020r, S1_MAPPING_DST_020f,
                                                        S1_MAPPING_DST_021f, 
                                                        S1_MAPPING_DST_022f,
                                                        S1_MAPPING_DST_023f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_027_024r, S1_MAPPING_DST_024f,
                                                        S1_MAPPING_DST_025f, 
                                                        S1_MAPPING_DST_026f,
                                                        S1_MAPPING_DST_027f },
{ LLS_LINKPHY_CONFIG_DST_STREAM_TO_S1_MAPPING_031_028r, S1_MAPPING_DST_028f,
                                                        S1_MAPPING_DST_029f, 
                                                        S1_MAPPING_DST_030f,
                                                        S1_MAPPING_DST_031f },

};

lp_lls_reg_t sb2_port_lp_enable[] = {
    { INVALIDr, INVALIDf, INVALIDf ,INVALIDf, INVALIDf},
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_01f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_02f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_03f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_04f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_05f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_06f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_07f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_08f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_09f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_10f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_11f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_12f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_13f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_14f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_15f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_16f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_17f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_18f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_19f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_20f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_21f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_22f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_23f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_24f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_25f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_26f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_27f, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_COE_ENABLEr, COE_ENABLE_28f, INVALIDf, INVALIDf, INVALIDf },
};

lp_lls_reg_t sb2_port_lp_config[] = {
    { INVALIDr, INVALIDf, INVALIDf, INVALIDf, INVALIDf },
    { LLS_PORT_01_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_02_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_03_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_04_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_05_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_06_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_07_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_08_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_09_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_10_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_11_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_12_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_13_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_14_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_15_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_16_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_17_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_18_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_19_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_20_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_21_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_22_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_23_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_24_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_25_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_26_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_27_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
    { LLS_PORT_28_COE_CONFIGr, START_S1f, END_S1f, INVALIDf, INVALIDf },
};

#endif

int
_soc_kt2_cosq_stream_mapping_set(int unit)
{
    uint32 i, rval;
    lp_lls_reg_t *map_reg;
    soc_reg_t sched_regs[] = {
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_31_0r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_63_32r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_95_64r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_127_96r
                          };
    soc_field_t sched_fields[] = {
                               SEL_SPRI_S1_IN_S0_31_0f,
                               SEL_SPRI_S1_IN_S0_63_32f,
                               SEL_SPRI_S1_IN_S0_95_64f,
                               SEL_SPRI_S1_IN_S0_127_96f
                              };

    map_reg = lp_stream_map;
    for (i = 0; i < 128; i++) {
        rval = 0;
        soc_reg_field_set(unit, map_reg[i].reg, &rval,
                          map_reg[i].field1, (i * 2));
        soc_reg_field_set(unit, map_reg[i].reg, &rval,
                          map_reg[i].field2, ((i * 2) + 1));
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, map_reg[i].reg, REG_PORT_ANY, 0, rval));
    }

    /* set the streams in spri by default */
    for (i = 0; i < 4; i++) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit,sched_regs[i], REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, sched_regs[i], &rval, sched_fields[i],
                          0xFFFFFFFF);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, sched_regs[i], REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

#if defined (BCM_SABER2_SUPPORT)
int
_soc_sb2_cosq_stream_mapping_set(int unit)
{
    uint32 i, rval, num_reg = 8;
    lp_lls_reg_t *map_reg;

    map_reg = sb2_lp_stream_map;
#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) { 
        num_reg = 6;
    }
#endif 
    for (i = 0; i < num_reg; i++) {
        rval = 0;
        soc_reg_field_set(unit, map_reg[i].reg, &rval, 
                          map_reg[i].field1, (i * 4));
        soc_reg_field_set(unit, map_reg[i].reg, &rval, 
                          map_reg[i].field2, ((i * 4) + 1));
        soc_reg_field_set(unit, map_reg[i].reg, &rval, 
                          map_reg[i].field3, ((i * 4) + 2));
        soc_reg_field_set(unit, map_reg[i].reg, &rval, 
                          map_reg[i].field4, ((i * 4) + 3));
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, map_reg[i].reg, REG_PORT_ANY, 0, rval));
    }
#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        return SOC_E_NONE; 
    }
#endif
   
    SOC_IF_ERROR_RETURN(READ_LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r(unit, &rval));
    soc_reg_field_set(unit, LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r, &rval,
                        SEL_SPRI_S1_IN_S0f, 0xFFFF);
    SOC_IF_ERROR_RETURN(WRITE_LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r(unit, rval));
 
    return SOC_E_NONE;
}
#endif

int
soc_kt2_cosq_stream_mapping_set(int unit)
{

#if defined (BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_sb2_cosq_stream_mapping_set(unit));
    } else
#endif
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_kt2_cosq_stream_mapping_set(unit));
    }
    return SOC_E_NONE;
}

int
_soc_kt2_cosq_s0_sched_set(int unit, int hw_index, int mode)
{
    int reg_id;
    uint32 rval, sched_mode;
    soc_reg_t sched_regs[] = {
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_31_0r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_63_32r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_95_64r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_127_96r
                          };
    soc_field_t sched_fields[] = {
                               SEL_SPRI_S1_IN_S0_31_0f,
                               SEL_SPRI_S1_IN_S0_63_32f,
                               SEL_SPRI_S1_IN_S0_95_64f,
                               SEL_SPRI_S1_IN_S0_127_96f
                              };
    reg_id = hw_index / 32;

    if (reg_id > 3) {
        return SOC_E_PARAM;
    }   

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sched_regs[reg_id], REG_PORT_ANY, 0, &rval));
    sched_mode = soc_reg_field_get(unit, sched_regs[reg_id], rval,
                                   sched_fields[reg_id]);
    if (mode) {
        sched_mode |= (1 << (hw_index % 32));
    } else {
        sched_mode &= ~(1 << (hw_index % 32));
    }
    soc_reg_field_set(unit, sched_regs[reg_id], &rval, sched_fields[reg_id],
                      sched_mode);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, sched_regs[reg_id], REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int
_soc_sb2_cosq_s0_sched_set(int unit, int hw_index, int mode)
{
    uint32 rval, sched_mode;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
                REG_PORT_ANY, 0, &rval));

    sched_mode = soc_reg_field_get(unit, 
            LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
            rval, SEL_SPRI_S1_IN_S0f);

    if (mode) {
        sched_mode |= (1 << hw_index);
    } else {
        sched_mode &= ~(1 << hw_index);
    }

    soc_reg_field_set(unit, LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
            &rval,  SEL_SPRI_S1_IN_S0f, sched_mode);

    SOC_IF_ERROR_RETURN(soc_reg32_set(unit,
                LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
                REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

int
soc_kt2_cosq_s0_sched_set(int unit, int hw_index, int mode)
{

#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        return SOC_E_NONE;
    }
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_sb2_cosq_s0_sched_set(unit, hw_index, mode));
    } else 
#endif 
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_kt2_cosq_s0_sched_set(unit, hw_index, mode));
    }
    
    return SOC_E_NONE;
}

int
_soc_sb2_cosq_s0_sched_get(int unit, int hw_index, soc_kt2_sched_mode_e *mode)
{
    uint32 rval, sched_mode;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit,
                LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
                REG_PORT_ANY, 0, &rval));

    sched_mode = soc_reg_field_get(unit, 
            LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0r,
            rval, SEL_SPRI_S1_IN_S0f);

    *mode = (sched_mode & (1 << hw_index)) ? SOC_KT2_SCHED_MODE_STRICT : 
                                    SOC_KT2_SCHED_MODE_WRR;
    return SOC_E_NONE;
}

int
_soc_kt2_cosq_s0_sched_get(int unit, int hw_index, soc_kt2_sched_mode_e *mode)
{
    int reg_id;
    uint32 rval, sched_mode;
    soc_reg_t sched_regs[] = {
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_31_0r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_63_32r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_95_64r,
                           LLS_LINKPHY_CONFIG_SEL_SPRI_S1_IN_S0_127_96r
                          };
    soc_field_t sched_fields[] = {
                               SEL_SPRI_S1_IN_S0_31_0f,
                               SEL_SPRI_S1_IN_S0_63_32f,
                               SEL_SPRI_S1_IN_S0_95_64f,
                               SEL_SPRI_S1_IN_S0_127_96f
                              };
    reg_id = hw_index / 32;

    if (reg_id > 3) {
        return SOC_E_PARAM;
    }   

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sched_regs[reg_id], REG_PORT_ANY, 0, &rval));
    sched_mode = soc_reg_field_get(unit, sched_regs[reg_id], rval,
                                   sched_fields[reg_id]);
    *mode = (sched_mode & (1 << (hw_index % 32))) ? SOC_KT2_SCHED_MODE_STRICT :
                                        SOC_KT2_SCHED_MODE_WRR;

    return SOC_E_NONE;
}

int
soc_kt2_cosq_s0_sched_get(int unit, int hw_index, soc_kt2_sched_mode_e *mode)
{
    if (!mode) {
        return SOC_E_PARAM;
    }
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_sb2_cosq_s0_sched_get(unit, hw_index, mode));
    } else 
#endif 
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_RETURN(_soc_kt2_cosq_s0_sched_get(unit, hw_index, mode));
    }
    return SOC_E_NONE;
}

int 
soc_kt2_cosq_port_coe_linkphy_status_set(int unit, soc_port_t port, int enable)
{
    uint32 rval, max_valid_index = KT2_MAX_PHYSICAL_PORTS;
    lp_lls_reg_t *port_reg = port_lp_enable;
    soc_info_t *si = &SOC_INFO(unit);

#if defined(BCM_METROLITE_SUPPORT)
    if(SOC_IS_METROLITE(unit)) {
        port_reg = sb2_port_lp_enable;
        max_valid_index = ML_MAX_PHYSICAL_PORTS;
    }  else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        port_reg = sb2_port_lp_enable;
        max_valid_index = SB2_MAX_PHYSICAL_PORTS;
    }
#endif

    if ((port_reg[port].reg != INVALIDr) && (port <= max_valid_index))  {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, port_reg[port].reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, port_reg[port].reg, &rval, 
                          port_reg[port].field1, enable ? 1 : 0);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, port_reg[port].reg, REG_PORT_ANY, 0, rval));

        /* Set the port as LinkPhy only if the port in question is a part of
           the linkphy_pbm */
        if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
            if (SOC_REG_IS_VALID(unit, DEQ_EFIFO_CFGr)) {
                rval = 0;
                SOC_IF_ERROR_RETURN(READ_DEQ_EFIFO_CFGr(unit, port, &rval));
                soc_reg_field_set(unit, DEQ_EFIFO_CFGr, &rval,
                        EGRESS_FIFO_LINK_PHYf, enable ? 1 : 0);
                SOC_IF_ERROR_RETURN(WRITE_DEQ_EFIFO_CFGr(unit, port, rval));
            }
        }
    } else {
        return SOC_E_PARAM;
    }

    return SOC_E_NONE;
}

int
soc_kt2_cosq_two_s1s_in_s0_config_set(int unit, soc_port_t port, int enable)
{
    lp_lls_reg_t *port_reg = port_lp_config;
    uint32 rval = 0;

#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        uint32 two_s1s_in_s0_bmp = 0;

        if (SOC_REG_IS_VALID(unit, LLS_TWO_S1S_IN_S0_CONFIGr)) {
            SOC_IF_ERROR_RETURN(READ_LLS_TWO_S1S_IN_S0_CONFIGr(unit,
                        &rval));
            two_s1s_in_s0_bmp = soc_reg_field_get(unit,
                    LLS_TWO_S1S_IN_S0_CONFIGr,
                    rval, TWO_S1S_IN_S0f);
            if (enable) {
                two_s1s_in_s0_bmp |= (1 << (port - 1));
            } else {
                two_s1s_in_s0_bmp &= ~(1 << (port - 1));
            }

            soc_reg_field_set(unit, LLS_TWO_S1S_IN_S0_CONFIGr,
                              &rval, TWO_S1S_IN_S0f,
                              (two_s1s_in_s0_bmp & 0xf));
            SOC_IF_ERROR_RETURN(WRITE_LLS_TWO_S1S_IN_S0_CONFIGr(unit,
                                rval));
        }
    } else
#endif
    {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, port_reg[port].reg, REG_PORT_ANY, 0, &rval));
        if (SOC_REG_FIELD_VALID(unit, port_reg[port].reg, TWO_S1S_IN_S0f)) {
            soc_reg_field_set(unit, port_reg[port].reg, &rval,
                    TWO_S1S_IN_S0f, enable);
        }
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, port_reg[port].reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

uint32
soc_kt2_cosq_two_s1s_in_s0_config_get(int unit, soc_port_t port)
{
    lp_lls_reg_t *port_reg = port_lp_config;
    uint32 rval = 0;

#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        return rval;
    }
#endif
#if defined (BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)) {
        uint32 two_s1s_in_s0_bmp = 0;
        SOC_IF_ERROR_RETURN(READ_LLS_TWO_S1S_IN_S0_CONFIGr(unit,
                    &rval));
        two_s1s_in_s0_bmp = soc_reg_field_get(unit,
                LLS_TWO_S1S_IN_S0_CONFIGr,
                rval, TWO_S1S_IN_S0f);

        return (two_s1s_in_s0_bmp & (1 << (port - 1)));
    } else
#endif
    {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, port_reg[port].reg, REG_PORT_ANY, 0, &rval));
        if (SOC_REG_FIELD_VALID(unit, port_reg[port].reg, TWO_S1S_IN_S0f)) {
            return (soc_reg_field_get(unit, port_reg[port].reg, rval,
                    TWO_S1S_IN_S0f));
        }
    }

    return 0;
}

int
soc_kt2_cosq_s1_range_set(int unit, soc_port_t port,
                          int start_s1, int end_s1, int type)
{
    uint32 rval = 0, max_port = KT2_MAX_PHYSICAL_PORTS;
    lp_lls_reg_t *port_reg = port_lp_config;
#if defined(BCM_SABER2_SUPPORT)
    uint32 lp_enable_config = 0, lp_enable_bmp = 0;
#endif

#if defined (BCM_METROLITE_SUPPORT)
    if (SOC_IS_METROLITE(unit)) {
        max_port = ML_MAX_PHYSICAL_PORTS;
        port_reg = sb2_port_lp_config;
    } else
#endif
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        max_port = SB2_MAX_PHYSICAL_PORTS;
        port_reg = sb2_port_lp_config;
    }
#endif

    if ((port_reg[port].reg != INVALIDr) && (port <= max_port)) {
        soc_reg_field_set(unit, port_reg[port].reg, &rval, 
                          port_reg[port].field1, start_s1);
        soc_reg_field_set(unit, port_reg[port].reg, &rval, 
                          port_reg[port].field2, end_s1);

#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN(READ_LLS_LINK_PHY_ENABLED_CONFIGr(unit,
                        &lp_enable_config));
            lp_enable_bmp = soc_reg_field_get(unit,
                    LLS_LINK_PHY_ENABLED_CONFIGr,
                    lp_enable_config, LINKPHY_ENABLEDf);
            if (type) {
                /* Set the port in the bitmap. */
                lp_enable_bmp |= ((1 << (port - 1)) & 0xF);
            } else {
                /* Reset the port in the bitmap. */
                lp_enable_bmp &= ~((1 << (port - 1)) & 0xF);
            }
            soc_reg_field_set(unit, LLS_LINK_PHY_ENABLED_CONFIGr,
                    &lp_enable_config, LINKPHY_ENABLEDf,
                    lp_enable_bmp);
            SOC_IF_ERROR_RETURN(WRITE_LLS_LINK_PHY_ENABLED_CONFIGr(unit,
                        lp_enable_config));
#if defined(BCM_METROLITE_SUPPORT)
            if (SOC_IS_METROLITE(unit)) {
                soc_reg_field_set(unit, TOQ_LINKPHY_ENABLEr,
                        &lp_enable_config, CFG_PORT_LINK_PHY_ENf,
                        (lp_enable_bmp << 1));
                SOC_IF_ERROR_RETURN(WRITE_TOQ_LINKPHY_ENABLEr(unit,
                        lp_enable_config));
            }
#endif
        }
#endif
        if (SOC_REG_FIELD_VALID(unit, port_reg[port].reg, LINKPHY_ENABLEDf)) {
            soc_reg_field_set(unit, port_reg[port].reg, &rval,
                    LINKPHY_ENABLEDf, type);
        }

        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, port_reg[port].reg, REG_PORT_ANY, 0, rval));

        SOC_IF_ERROR_RETURN(soc_kt2_cosq_two_s1s_in_s0_config_set(unit, port,
                    type));
    } else {
        return SOC_E_PARAM;
    }
    
    return SOC_E_NONE;
}

int
soc_kt2_cosq_repl_map_set(int unit, soc_port_t port,
                          int start, int end, int enable)
{
    mmu_repl_map_tbl_entry_t map_entry;  
    uint32 pp_port;
    int i, min_index = SOC_INFO(unit).pp_port_index_min;

#if defined (BCM_METROLITE_SUPPORT)
    int pp_port_free = 0;
    pbmp_t  pp_port_unused_bitmap;
    SOC_PBMP_ASSIGN(pp_port_unused_bitmap, SOC_PORT_PP_BITMAP(unit, port));
#endif

    for (i = start; i <= end; i++) {
        sal_memset(&map_entry, 0, sizeof(mmu_repl_map_tbl_entry_t));
        if (enable) {
#if defined (BCM_METROLITE_SUPPORT)
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                SOC_PBMP_ITER(pp_port_unused_bitmap, pp_port_free) {
                    pp_port = pp_port_free;
                    SOC_PBMP_PORT_REMOVE(pp_port_unused_bitmap, pp_port_free);
                    break;
                }
            } else 
#endif
            {
                pp_port = (i + min_index);
            }
            soc_MMU_REPL_MAP_TBLm_field_set(unit, &map_entry,
                                        DEST_PPPf, &pp_port); 
        }
        SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_MAP_TBLm(unit,
                                   MEM_BLOCK_ALL, i, &map_entry));
    }

    return SOC_E_NONE;
}

int
soc_kt2_cosq_lp_repl_map_set(int unit, int stream, uint32 pp_port, int enable)
{
    mmu_repl_map_tbl_entry_t map_entry;
    uint32 pp_port_local = pp_port;

    sal_memset(&map_entry, 0, sizeof(mmu_repl_map_tbl_entry_t));

    if (enable) {
        soc_MMU_REPL_MAP_TBLm_field_set(unit, &map_entry,
                DEST_PPPf, &pp_port_local);
    }
    SOC_IF_ERROR_RETURN(WRITE_MMU_REPL_MAP_TBLm(unit,
                MEM_BLOCK_ALL, stream, &map_entry));

    return SOC_E_NONE;
}

STATIC int
_soc_kt2_port_get_cascaded_mode(int unit, int port, int *mode) {
    soc_mem_t mem = EGR_PHYSICAL_PORTm;
    int port_type;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv = SOC_E_NONE;

    if (!mode) {
        return SOC_E_PARAM;
    }
    *mode = -1;

    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, port, &entry);
    if (rv) {
        LOG_CLI((BSL_META_U(unit,
                            "Failed to read memory %d at index: %d\n"), mem, port));
        return rv;
    }

    port_type = soc_mem_field32_get(unit, mem, entry, PORT_TYPEf);
    if (port_type != _SOC_COE_PORT_TYPE_CASCADED) {
        return SOC_E_CONFIG;
    }
    *mode = soc_mem_field32_get(unit, mem, entry, CASCADED_PORT_TYPEf);
    return SOC_E_NONE;
}

STATIC
int _bcm_kt2_is_active_cascaded_port(int unit, soc_port_t port)
{
    int cascaded_mode = -1, rv = 0;
    rv = _soc_kt2_port_get_cascaded_mode(unit, port, &cascaded_mode);
    if ((rv == SOC_E_NONE) &&
        ((cascaded_mode == _SOC_COE_PORT_TYPE_CASCADED_LINKPHY) ||
        (cascaded_mode == _SOC_COE_PORT_TYPE_CASCADED_SUBTAG))) {
        return TRUE; 
    }
    return FALSE;
}

int
soc_kt2_sched_get_node_config(int unit, soc_port_t port, int level, int index,
                              int *pnum_spri, int *pfirst_child,
                              int *pfirst_mc_child, uint32 *pucmap, 
                              uint32 *pspmap)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    uint32 num_spri = 0, ucmap = 0, f1, f2;
    int first_child = -1, first_mc_child = -1, ii;
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);
    soc_info_t *si;
#if defined BCM_METROLITE_SUPPORT
    int phy_port;
#endif

    *pspmap = 0;

    si = &SOC_INFO(unit);

    if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
        mem = _SOC_KT2_NODE_CONFIG_LINKPHY_MEM(level);
        if (level == _SOC_KT2_COSQ_NODE_LVL_S0) {
            /*
             * S0 config - two S1 model per S0
             */
            *pnum_spri = 2;
            *pfirst_child = index * 2;
            *pspmap = 3;
            *pfirst_mc_child = *pucmap = 0;
            return SOC_E_NONE;

        }
        if (level == _SOC_KT2_COSQ_NODE_LVL_S1) {
            /*
             * S1 level scheduling nodes
             * each S1 is connected to only one L0
             * S1 and L0 node address are same
             */

            *pnum_spri = 1;
            *pfirst_child = index;
            *pspmap = 1;
            *pfirst_mc_child = *pucmap = 0;
            return SOC_E_NONE;

        }
    } else {
        mem = _SOC_KT2_NODE_CONFIG_MEM(level);
    }

#if defined BCM_METROLITE_SUPPORT
    if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
        phy_port = soc_ml_pp_port_get_port(unit, port);
        if ((phy_port != -1) && _bcm_kt2_is_active_cascaded_port(unit, phy_port) &&
                                    level == SOC_KT2_NODE_LVL_ROOT) {
            mem = LLS_S1_CONFIGm ;
        }
    } else
#endif
    {
        if (port >= SOC_INFO(unit).pp_port_index_min && level == SOC_KT2_NODE_LVL_ROOT) {
            mem = LLS_S1_CONFIGm ;
        }
    }
 
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    if (sp_vec) {
        f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
        f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
        *pspmap = f1 | (f2 << 4);
        for (ii = 0; ii < 32; ii++) {
            if ((1 << ii) & *pspmap) {
                num_spri++;
            }
        }
    } else {
        num_spri = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    }

    if (mem == LLS_L1_CONFIGm) {
        first_child = soc_mem_field32_get(unit, mem, &entry, P_START_UC_SPRIf);
        first_mc_child = soc_mem_field32_get(unit, mem, 
                                         &entry, P_START_MC_SPRIf) + 1024;
        ucmap = soc_mem_field32_get(unit, mem, &entry, P_SPRI_SELECTf);
    } else {
        first_child = soc_mem_field32_get(unit, mem, &entry, P_START_SPRIf);
        first_mc_child = 0;
    }

    if (num_spri == 0) {
        ucmap = 0;
    }

    if (pnum_spri) {
        *pnum_spri = num_spri;
    }
       
    if (pucmap) {
        *pucmap = ucmap;
    }

    if (pfirst_child) {
        *pfirst_child = first_child;
    }

    if (pfirst_mc_child) {
        *pfirst_mc_child = first_mc_child;
    }
    return SOC_E_NONE;
}

int 
soc_kt2_sched_weight_get(int unit, soc_port_t port, int level, int index, int *weight)
{
     soc_mem_t mem_weight;
     soc_info_t *si;
     uint32 entry[SOC_MAX_MEM_WORDS];

     si = &SOC_INFO(unit);

     if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
         mem_weight = _SOC_KT2_NODE_WIEGHT_LINKPHY_MEM(level);
     } else {
         mem_weight = _SOC_KT2_NODE_WIEGHT_MEM(level);
     }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem_weight, MEM_BLOCK_ALL, index, &entry));

    *weight = soc_mem_field32_get(unit, mem_weight, &entry, C_WEIGHTf);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_get L%d index=%d wt=%d\n"),
              level, index, *weight));
    return SOC_E_NONE;
}

int
soc_kt2_cosq_get_sched_mode(int unit, soc_port_t port, int level, int index,
                              soc_kt2_sched_mode_e *pmode, int *weight)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem;
    soc_kt2_sched_mode_e mode = SOC_KT2_SCHED_MODE_UNKNOWN;
    soc_info_t *si;
#if defined BCM_METROLITE_SUPPORT
    int level_iter;
#endif
    si = &SOC_INFO(unit);

    /* S0 nodes are always in RR mode */
    if ((SOC_PBMP_MEMBER(si->linkphy_pbm, port)) &&
        (level == _SOC_KT2_COSQ_NODE_LVL_S0)) {
        if (pmode) {
            *pmode = SOC_KT2_SCHED_MODE_WRR;
        }
        if (weight) {
            *weight = 1;
        }
        return SOC_E_NONE;
    }

    /* get S1 nodes scheduling */
    if ((SOC_PBMP_MEMBER(si->linkphy_pbm, port)) &&
        (level == _SOC_KT2_COSQ_NODE_LVL_S1)) {
         SOC_IF_ERROR_RETURN(soc_kt2_cosq_s0_sched_get(unit, index, pmode));
         return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(soc_kt2_sched_weight_get(unit, port, level, index, weight));
    if (*weight == 0) {
        mode = SOC_KT2_SCHED_MODE_STRICT;
    } else {
        if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
            mem = _SOC_KT2_NODE_CONFIG_LINKPHY_MEM(_SOC_KT2_COSQ_NODE_LVL_ROOT);
        } else {
            mem = _SOC_KT2_NODE_CONFIG_MEM(SOC_KT2_NODE_LVL_ROOT);
        }

#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, port)) {
                level_iter = level;
                while(level_iter != SOC_KT2_NODE_LVL_ROOT) {
                    mem = _SOC_KT2_NODE_PARENT_MEM(level_iter);
                    SOC_IF_ERROR_RETURN(
                            soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
                    index = soc_mem_field32_get(unit, mem, entry, C_PARENTf);
                    level_iter--;
                }
                mem = LLS_S1_CONFIGm ;
            } else {
                index = port;
            }
        } else
#endif
        {
            if (port >= SOC_INFO(unit).pp_port_index_min){
                mem = LLS_S1_CONFIGm; 
            }
            index = port;
        }

        SOC_IF_ERROR_RETURN(
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
        if (soc_mem_field32_get(unit, mem, entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
            mode = SOC_KT2_SCHED_MODE_WRR;
        } else {
            mode = SOC_KT2_SCHED_MODE_WDRR;
        }
    }

    if (pmode) {
        *pmode = mode;
    }
        
    return SOC_E_NONE;
}

int soc_kt2_get_child_type(int unit, soc_port_t port, int level, 
                                    int *child_type)
{
    *child_type = -1;

    if (level == SOC_KT2_NODE_LVL_ROOT) {
        *child_type = SOC_KT2_NODE_LVL_L0;
    } else if (level == SOC_KT2_NODE_LVL_L0) {
        *child_type = SOC_KT2_NODE_LVL_L1;
    } else if (level == SOC_KT2_NODE_LVL_L1) {
        *child_type = SOC_KT2_NODE_LVL_L2;
    }
    return SOC_E_NONE;
}


int _soc_kt2_invalid_parent_index(int unit, int level)
{
    int index_max = 0;

    switch (level) {
        case SOC_KT2_NODE_LVL_ROOT:
            index_max = 0;
            break;
        case SOC_KT2_NODE_LVL_L0:
            index_max = (1 << soc_mem_field_length(unit, LLS_L0_PARENTm, C_PARENTf)) - 1;
            break;
        case SOC_KT2_NODE_LVL_L1:
            index_max = (1 << soc_mem_field_length(unit, LLS_L1_PARENTm, C_PARENTf)) - 1;
            break;
        case SOC_KT2_NODE_LVL_L2:
            index_max = (1 << soc_mem_field_length(unit, LLS_L2_PARENTm, C_PARENTf)) - 1;
            break;
        default:
            break;

    }

    return index_max;
}

#define INVALID_PARENT(unit, level)   _soc_kt2_invalid_parent_index((unit),(level))

/* Programming all the entries with the Invalid Parent Pointer */
int
_soc_katana2_mmu_set_invalid_parent(int unit)
{
    int level, index_max, index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];

    for (level = SOC_KT2_NODE_LVL_L0; level <= SOC_KT2_NODE_LVL_L2;
            level++) {
        mem = _SOC_KT2_NODE_PARENT_MEM(level);
        /* coverity[negative_returns : FALSE] */
        index_max = soc_mem_index_max(unit, mem);
        for (index = 0; index <= index_max; index++) {
            SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            soc_mem_field32_set(unit, mem, entry,
                    C_PARENTf, INVALID_PARENT(unit, level));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, &entry));
        }
    }
    return SOC_E_NONE;
}

STATIC int 
_soc_kt2_dump_sched_at(int unit, int port, int level, int offset, int hw_index)
{
    soc_info_t *si = &SOC_INFO(unit);
    int num_spri, first_child, first_mc_child, rv, cindex;
    uint32 ucmap, spmap;
    soc_kt2_sched_mode_e sched_mode;
    soc_mem_t mem;
    int index_max, ii, ci, child_level;
    int wt = 0, num_child;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char *lvl_name[] = { "Root", "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};
    int ctype = 0;
    int mcstart = 0; 
#if defined BCM_METROLITE_SUPPORT
    int phy_port = -1;
#endif
    
    if ((level >= SOC_KT2_NODE_LVL_L0) && (hw_index == INVALID_PARENT(unit, level))) {
        return SOC_E_NONE;
    }
    /* get sched config */
    SOC_IF_ERROR_RETURN(
            soc_kt2_sched_get_node_config(unit, port, level, hw_index, 
                       &num_spri, &first_child, &first_mc_child, &ucmap, &spmap));
    sched_mode = 0;
    if (level != SOC_KT2_NODE_LVL_ROOT) {
        SOC_IF_ERROR_RETURN(
         soc_kt2_cosq_get_sched_mode(unit, port, level, hw_index, &sched_mode, &wt));
    }
    
    if (level == SOC_KT2_NODE_LVL_L1) {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d FMC=%d "
                            "UCMAP=0x%08x SPMAP=0x%08x MODE=%s WT=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, 
                 first_child, first_mc_child, ucmap, spmap,
                 sched_modes[sched_mode], wt));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d "
                            "SPMAP=0x%08x MODE=%s Wt=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, first_child,
                 spmap, sched_modes[sched_mode], wt));
    }
    if (port != 0 && first_child == 0) {
       return  SOC_E_NONE; 
    }

    soc_kt2_get_child_type(unit, port, level, &child_level);
    mem = _SOC_KT2_NODE_PARENT_MEM(child_level);
    
    if(mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    index_max = soc_mem_index_max(unit, mem);

    num_child = 0;
    for (ii = 0, ci = 0; ii < index_max; ii++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &entry);
        if (rv) {
            LOG_CLI((BSL_META_U(unit,
                                "Failed to read memory at index: %d\n"), ii));
            break;
        }
        
        cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);

        if (cindex == INVALID_PARENT(unit, child_level)) {
            continue;
        }

        if (child_level == SOC_KT2_NODE_LVL_L0){
         /*The ctype value gives the parent type  of this l0 node 
           if the  ctype is 1 the its parent is physical port
           If its 0 then the parent is subport .
         */
            ctype = soc_mem_field32_get(unit, mem, entry, C_TYPEf);
            if(!ctype )
            {
#if defined BCM_METROLITE_SUPPORT
                if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                    phy_port = soc_ml_pp_port_get_port(unit, port);
                    if ((phy_port != -1) &&
                            !_bcm_kt2_is_active_cascaded_port(unit, phy_port)) {
                        continue;
                    }
                } else
#endif
                {
                    if(port < si->cpu_hg_index) {
                        continue;
                    }
                }
            }
            else {
#if defined BCM_METROLITE_SUPPORT
               if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                    phy_port = soc_ml_pp_port_get_port(unit, port);
                    if ((phy_port != -1) &&
                            _bcm_kt2_is_active_cascaded_port(unit, phy_port)) {
                        continue;
                    }
               } else
#endif
               {
                    if(port >= si->cpu_hg_index) {
                        continue;
                    }
               }
            }
        }

       if (cindex == hw_index)  {
            if (child_level == SOC_KT2_NODE_LVL_L2)  {
                SOC_IF_ERROR_RETURN(soc_kt2_cosq_get_sched_mode(unit, port,
                                    SOC_KT2_NODE_LVL_L2, ii, &sched_mode, &wt));
                if (SOC_IS_SABER2(unit)) { 
                    mcstart = 512;
                } else { 
                    mcstart = 2048 ;                     
                }
                LOG_CLI((BSL_META_U(unit,
                                    "     L2.%s INDEX=%d Mode=%s WEIGHT=%d\n"), 
                         ((ii < mcstart) ? "uc" : "mc"),
                         ii, sched_modes[sched_mode], wt));
            } else {
                _soc_kt2_dump_sched_at(unit, port, child_level, ci, ii);
                ci += 1;
            }
            num_child += 1;
        }
    }
    if (num_child == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "*** No children \n")));
    }
    return SOC_E_NONE;
}

int soc_kt2_dump_port_lls(int unit, int port,int subport)
{
    soc_info_t *si = &SOC_INFO(unit);
    int rv, cascaded_mode = -1;
#if defined BCM_METROLITE_SUPPORT
    _bcm_kt2_subport_info_t info;
    int s1;
#endif

    if (!subport) {
        LOG_CLI((BSL_META_U(unit,
                "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));
#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            if (_bcm_kt2_is_active_cascaded_port(unit, port)) {
                return SOC_E_NONE; 
            }
        }
#endif
        _soc_kt2_dump_sched_at(unit, port, SOC_KT2_NODE_LVL_ROOT, 0, port);
    }
    else { 
        LOG_CLI((BSL_META_U(unit,
                "-------%s subport %d (LLS)------\n"),SOC_PORT_NAME(unit, 
                                                      (port)), subport));
#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            if (!_bcm_kt2_is_active_cascaded_port(unit, port)) {
                return SOC_E_NONE; 
            }
        } else
#endif
        {
            rv = _soc_kt2_port_get_cascaded_mode(unit, port, &cascaded_mode);
            if ((rv != 0) || (cascaded_mode == _SOC_COE_PORT_TYPE_CASCADED_LINKPHY)) {
                return SOC_E_NONE; 
            }
        }


#if defined BCM_METROLITE_SUPPORT
        if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
            SOC_IF_ERROR_RETURN
                (bcm_kt2_subport_pp_port_subport_info_get(unit, subport,
                                                                    &info));
            if (info.port_type == _BCM_KT2_SUBPORT_TYPE_SUBTAG) {
                BCM_IF_ERROR_RETURN
                    (bcm_kt2_subport_pp_port_s1_index_get(unit, subport, &s1));

                _soc_kt2_dump_sched_at(unit, subport, SOC_KT2_NODE_LVL_ROOT, 0, 
                        s1);
            }
        } else
#endif
        {
            /* lls_s1_config is 128 entries long so reducing 42 from the subport number*/
            _soc_kt2_dump_sched_at(unit, subport, SOC_KT2_NODE_LVL_ROOT, 0, 
                    (subport - si->cpu_hg_index));
        }
    }
                            
    return SOC_E_NONE;
}

#endif /* defined(BCM_KATANA2_SUPPORT) */
