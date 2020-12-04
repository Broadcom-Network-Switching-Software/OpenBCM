/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_PPA_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PPA_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_ppa_api.h>    

EXTERN CONST
   PARAM_VAL_RULES
     ppa_api_mac_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        12,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC address parameter. 12 hex digits. No 0 no 0x Prefix",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;  

                                                                              
EXTERN CONST
   PARAM_VAL_RULES
     ppa_api_ipv6_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        32,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    IPV6 address parameter. 12 hex digits. No 0 no 0x Prefix",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;  

EXTERN CONST 
   PARAM_VAL_RULES 
     ppa_api_free_vals[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_NUMERIC,SYMB_NAME_NUMERIC, 
    { 
      { 
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE, 
        0,0, 
          /* 
           * Number of times this value can be repeated (i.e., 
           * value is an array). Must be at least 1! 
           */ 
        1, 
        (VAL_PROC_PTR)NULL 
      } 
    } 
  }, 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
EXTERN CONST 
   PARAM_VAL_RULES 
     ppa_api_empty_vals[] 
#ifdef INIT 
   = 
{ 
/* 
 * Last element. Do not remove. 
 */ 
  { 
    VAL_END_OF_LIST 
  } 
} 
#endif 
; 
/********************************************************/        
/********************************************************/        
EXTERN CONST 
   PARAM 
     ppa_api_params[] 
#ifdef INIT 
   = 
{ 
  { 
    PARAM_PPA_BRIDGE_ROUTER_APP_ID, 
    "bridge_router_app", 
    (PARAM_VAL_RULES *)&ppa_api_empty_vals[0], 
    (sizeof(ppa_api_empty_vals) / sizeof(ppa_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(2), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID, 
    "flow_based_mode", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(2), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_ID, 
    "vpls_app", 
    (PARAM_VAL_RULES *)&ppa_api_empty_vals[0], 
    (sizeof(ppa_api_empty_vals) / sizeof(ppa_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID, 
    "flow_based_mode", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID, 
    "interrupt_not_poll", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_OAM_APP_ID, 
    "oam_enable", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID, 
    "nof_mp_services", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID, 
    "nof_p2p_services", 
    (PARAM_VAL_RULES *)&ppa_api_free_vals[0], 
    (sizeof(ppa_api_free_vals) / sizeof(ppa_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    {BIT(1), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
/* 
 * Last element. Do not remove.
 */ 
  { 
    PARAM_END_OF_LIST 
  } 
} 
#endif  
;  
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_ROM_DEFI_PPA_API_INCLUDED__*/
#endif
