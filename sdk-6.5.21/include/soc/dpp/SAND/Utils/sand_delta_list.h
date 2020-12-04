/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef SOC_SAND_DELTA_LIST_H
#define SOC_SAND_DELTA_LIST_H
#ifdef  __cplusplus
extern "C" {
#endif

#include <sal/core/sync.h>
#include <sal/core/thread.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

typedef struct soc_list_node_str
{
  void                 *data;
  uint32         num;
  struct soc_list_node_str *next;
} SOC_SAND_DELTA_LIST_NODE;

typedef struct
{
  uint32   current_size;
  uint32   no_of_pops;
  uint32   no_of_removes;
  uint32   no_of_inserts;
} SOC_SAND_DELTA_LIST_STATISTICS;

typedef struct
{
  SOC_SAND_DELTA_LIST_NODE       *head;
  uint32                         head_time;
  sal_mutex_t                    mutex_id;
  sal_thread_t                   mutex_owner;
  int32                          mutex_counter;
  SOC_SAND_DELTA_LIST_STATISTICS stats;
} SOC_SAND_DELTA_LIST;


sal_thread_t
  soc_sand_delta_list_get_owner(
    SOC_SAND_DELTA_LIST  *plist
  );

void
  soc_sand_delta_list_print(
    SOC_SAND_DELTA_LIST  *plist
  );

int
  soc_sand_delta_list_is_empty(
    SOC_SAND_IN   SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_RET
  soc_sand_delta_list_take_mutex(
    SOC_SAND_INOUT   SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_RET
  soc_sand_delta_list_give_mutex(
    SOC_SAND_INOUT   SOC_SAND_DELTA_LIST      *plist
  );

uint32
  soc_sand_delta_list_get_head_time(
    SOC_SAND_IN     SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_DELTA_LIST_STATISTICS
  *soc_sand_delta_list_get_statisics(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_DELTA_LIST
  *soc_sand_delta_list_create(void);

SOC_SAND_RET
  soc_sand_delta_list_destroy(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_RET
  soc_sand_delta_list_insert_d(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist,
    SOC_SAND_IN     uint32    nominal_value,
    SOC_SAND_INOUT  void            *data
  );

void
  *soc_sand_delta_list_pop_d(
    SOC_SAND_INOUT  SOC_SAND_DELTA_LIST      *plist
  );

SOC_SAND_RET
  soc_sand_delta_list_decrease_time_from_head(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_IN    uint32    time_to_substract
  );

SOC_SAND_RET
  soc_sand_delta_list_decrease_time_from_second_item(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_INOUT  uint32   time_to_substract
  );

SOC_SAND_RET
  soc_sand_delta_list_remove(
    SOC_SAND_INOUT SOC_SAND_DELTA_LIST       *plist,
    SOC_SAND_IN    void             *data
  );



#if SOC_SAND_DEBUG




void
  soc_sand_delta_list_print_DELTA_LIST_STATISTICS(
    SOC_SAND_IN SOC_SAND_DELTA_LIST_STATISTICS* delta_list_statistics
  );


void
  soc_sand_delta_list_print_DELTA_LIST(
    SOC_SAND_IN SOC_SAND_DELTA_LIST* delta_list
  );


#endif


#ifdef  __cplusplus
}
#endif

#endif
