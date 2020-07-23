/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef ___UI_GENERAL_UTILS_H___
#define ___UI_GENERAL_UTILS_H___

#ifndef SAND_LOW_LEVEL_SIMULATION

int
  download_boot_block_to_flash(
    char *ip_address
  );

#endif

int
  download_application_block_to_flash(
    unsigned int app_i,
    char *ip_address
  );
int
  get_application_version(
    const char* mem_file_base,
    const int data_size,
    const int zipped,
    unsigned int *version  );
int
  get_application_version_from_zipped_file(
    const char* mem_file_name,
    const int data_size,
    unsigned int *version
  );
int
  print_flash_descriptor_info(
    void
  );
unsigned
  int
    get_num_bits_set(
      unsigned long *array_of_longs,
      unsigned int  num_longs_in_array
    );
unsigned
  int
    is_bit_set(
      unsigned long *array_of_longs,
      unsigned int  bit_to_check
    );
#endif /* ___UI_GENERAL_UTILS_H___ */
