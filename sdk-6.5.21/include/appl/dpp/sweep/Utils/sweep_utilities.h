/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SWEEP_UTILITIES_H_INCLUDED__
/* { */
#define __SWEEP_UTILITIES_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 */
/* { */

/* } */

/*************
 * DEFINES   *
 */
/* { */
#define SWEEP_MAX_LINE_SIZE (200)

#define SWEEP_USE_RANDOM_SEED 0

/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

#if !defined(FAP20M_STANDALONE_PACKAGE) && !(DUNE_STANDALONE_PACKAGE)

unsigned int
  sweep_init_remote_access_system(
    void
  );

#endif

/* } */
/**********************************************************
* Start random mechanism. The seed can be determined either
* by the clock or by "given_seed". Enabling bug reproducing.
 */

unsigned int
  sweep_set_random_seed(
    unsigned int given_seed
  );
unsigned long
  sweep_get_random_seed(
    void
  );



#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __SWEEP_UTILITIES_H_INCLUDED__*/
#endif
