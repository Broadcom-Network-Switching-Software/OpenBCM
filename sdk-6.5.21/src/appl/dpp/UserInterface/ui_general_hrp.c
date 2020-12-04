/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Basic_include_file.
 */

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__)
/* { */

int
  app_file_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  return 0;
}


int
  fabric_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  return 0;
}


int
  line_card_slot_id_checker(
    void           *current_line_ptr,
    unsigned long  data_value,
    char           *err_msg,
    unsigned long  partial
  )
{
  return 0;
}

unsigned int
  ui_printing_policy_get(
    void
  )
{
  return 1;
}
/*} LINUX */
#endif
