
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * Dune chips include file.
 */
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/diag/dpp/dune_chips.h>

/*
 * Allocate memory for rom variables.
 * {
 */
#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN

#ifdef INIT
#undef INIT
#endif
#define INIT

/* { */
/* !DUNE_BCM does not need undef */
#undef  CONST
#define CONST
/* } */
#include <appl/dpp/UserInterface/ui_rom_defi.h>
/*
 * }
 */

/*
 * Allocate memory for ram variables.
 * {
 */
#undef  INIT
#define INIT
#undef  EXTERN
#define EXTERN

#if !DUNE_BCM
#if LINK_FAP20V_LIBRARIES
#include <appl/dpp/UserInterface/ui_rom_defi_fap20v.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fap20v_b.h>
#endif

#if LINK_FE600_LIBRARIES
#include <appl/dpp/UserInterface/ui_rom_defi_fe600_bsp.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fe600_api.h>
#endif


/* { */
#if !(defined(LINUX) || defined(UNIX))
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_line_gfa.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_line_tgs.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_line_tevb.h>
/* } */
#endif
#endif /* DUNE_BCM */


#include <appl/dpp/UserInterface/ui_ram_defi.h>

#include "ui_rom_defi.cx"

/*
 * }
 */
