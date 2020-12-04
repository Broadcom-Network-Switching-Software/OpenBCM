/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_SWEEP_APP_H_INCLUDED__
/* { */
#define __UI_SWEEP_APP_H_INCLUDED__

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
#include <appl/dpp/UserInterface/ui_pure_defi.h>

/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 *  MACROS   *
 */
/* { */

#define SWEEP_UI_REPORT_SW_ERR_AND_EXIT(err_id)  \
      if (0 != ui_ret)\
      {\
        ui_sweep_reoprt_sw_error_to_user(err_id);\
        ui_ret = TRUE ;\
        goto exit ;\
      }


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

void
  ui_sweep_reoprt_sw_error_to_user(
    unsigned int err_id
  );

void
  sweep_ui_report_driver_err(
    const unsigned long driver_ret
  );

int
  subject_fap20v_sweep_app(
    CURRENT_LINE *current_line, CURRENT_LINE **current_line_ptr
  );

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_SWEEP_APP_H_INCLUDED__*/
#endif
