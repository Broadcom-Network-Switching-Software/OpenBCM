
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PEM_MEMINFO_ACCESS_H_
#define _PEM_MEMINFO_ACCESS_H_






#define PEM_RANGE_LOW_LIMIT 0
#define PEM_RANGE_HIGH_LIMIT 1

#define DB_ROWS 0
#define DB_COLUMNS 1

#define MAX_NOF_FIELDS   16

#define IN
#define OUT

#define MAX_NOF_UNITS 20



void get_nof_entries(const int unit, int db_id);
void db_mapping_info_init(const int unit, int nof_reg);
void reg_mapping_info_init(const int unit, int nof_dbs);
void tcam_mapping_info_init(const int unit, int nof_dbs);
void em_mapping_info_init(const int unit, int nof_dbs);
void lpm_mapping_info_init(const int unit, int nof_dbs);
void vr_mapping_info_init(const int unit, int nof_vrs);

#endif 

