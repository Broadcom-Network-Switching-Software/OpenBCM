/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
#ifndef _SHR_SW_STATE_EXAMPLE_H
#define _SHR_SW_STATE_EXAMPLE_H

#include <soc/types.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>
#include <shared/swstate/sw_state.h>
#include <soc/error.h>

typedef struct mini_example_s {
    int                     regular;
    int                     *singlePtr;
    int                     staticArr[4];
    int                     staticArrArr[2][3];

	PARSER_HINT_PTR         int *ptrVar;
    PARSER_HINT_ARR         int *arrVar;
    PARSER_HINT_ARR_ARR     int **arrArr;
    PARSER_HINT_ARR_PTR     int **arrPtr;
		
    SHR_BITDCL				*shr_bitdcl_singlePtr;
	SHR_BITDCL		        **shr_bitdcl_doublePtr;
    SHR_BITDCL				shr_bitdcl_staticArr[4]; 
    SHR_BITDCL				shr_bitdcl_double_staticArr[4][5]; 
		
	SW_STATE_BUFF           *buff;
		
	bcm_pbmp_t              regularPbmp;
    bcm_pbmp_t              *pbmpPtr;
	
} mini_example_t;

typedef struct mix_stat_dyn_example_s {
	PARSER_HINT_PTR         int *staticArrDynPtr[3];
    PARSER_HINT_ARR         int *staticArrDynArr[3];
    PARSER_HINT_ARR_PTR     int **staticArrDynArrPtr[2];
    PARSER_HINT_ARR_ARR     int **staticArrArrDynArrArr[2][4];
} mix_stat_dyn_example_t;

typedef struct one_level_example_s {
    mini_example_t          oneLevelRegular;
    mini_example_t          *oneLevelSinglePtr;
    mini_example_t          oneLevelStaticArr[2];
    mini_example_t          oneLevelStaticArrArr[2][3];
	  mini_example_t          oneLevelStaticArrArr3[3][4][5];
    PARSER_HINT_PTR         mini_example_t *oneLevelPtr;
    PARSER_HINT_ARR         mini_example_t *oneLevelArr;
    PARSER_HINT_ARR_ARR     mini_example_t **oneLevelArrArr;
    PARSER_HINT_ARR_PTR     mini_example_t **oneLevelArrPtr;
} one_level_example_t;

typedef struct two_levels_example_s {
    one_level_example_t     twoLevelsRegular;
    one_level_example_t     *twoLevelsSinglePtr;
    one_level_example_t     twoLevelsStaticArrInt[2];
    one_level_example_t     twoLevelsStaticArrArr[2][3];
    PARSER_HINT_PTR         one_level_example_t *twoLevelsPtr;
    PARSER_HINT_ARR         one_level_example_t *twoLevelsArr;
    PARSER_HINT_ARR_ARR     one_level_example_t **twoLevelsArrArr;
    PARSER_HINT_ARR_PTR     one_level_example_t **twoLevelsArrPtr;
	PARSER_HINT_ALLOW_WB_ACCESS	one_level_example_t twoLevelsRegularAllowWbAccess;
	
} two_levels_example_t;


#ifndef BCM_ISSU_TEST_AFTER_ISSU
#define TEST_ISSU_MY_ARRAY_SIZE  10
#define TEST_ISSU_MY_ARRAY_SIZE_0  2
#define TEST_ISSU_MY_ARRAY_SIZE_1  3
#endif /* BCM_ISSU_TEST_AFTER_ISSU */
#ifdef BCM_ISSU_TEST_AFTER_ISSU
#define TEST_ISSU_MY_ARRAY_SIZE  20
#define TEST_ISSU_MY_ARRAY_SIZE_0  5
#define TEST_ISSU_MY_ARRAY_SIZE_1  7
#endif /* BCM_ISSU_TEST_AFTER_ISSU */

typedef struct sw_state_issu_example_s {
    uint8                   o1234567890123456789012345678901234567890123456789;
    uint8                   myUint8;
    uint16                  myUint16;
    uint32                  myArray[TEST_ISSU_MY_ARRAY_SIZE]; 
    uint32                  myArray2D[TEST_ISSU_MY_ARRAY_SIZE_0][TEST_ISSU_MY_ARRAY_SIZE_1]; 
#ifndef BCM_ISSU_TEST_AFTER_ISSU
    uint32                  myUint32; 
#endif
#ifdef BCM_ISSU_TEST_AFTER_ISSU
    uint64                  myUint64; 
#endif /* BCM_ISSU_TEST_AFTER_ISSU */
} sw_state_issu_example_t;

#endif /* _SHR_SW_STATE_EXAMPLE_H */
