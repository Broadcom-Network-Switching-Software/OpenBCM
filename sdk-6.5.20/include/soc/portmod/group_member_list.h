/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _GROUP_MEMBER_LIST_H_
#define _GROUP_MEMBER_LIST_H_

#include <sal/types.h>

typedef struct member_entry_s member_entry_t;
typedef struct group_entry_s group_entry_t;
typedef uint32 member_entry_id_t;
typedef uint32 group_entry_id_t;

#define GROUP_MEM_LIST_END (0xffffffff)


typedef int (*group_member_list_group_set_f)(void *user_data, group_entry_id_t group_id, group_entry_t* group);
typedef int (*group_member_list_group_get_f)(void *user_data, group_entry_id_t group_id, group_entry_t* group);
typedef int (*group_member_list_member_set_f)(void *user_data, member_entry_id_t member_id, member_entry_t* member);
typedef int (*group_member_list_member_get_f)(void *user_data, member_entry_id_t member_id, member_entry_t* member);

typedef struct group_member_list_s{
    uint32 groups_count;
    uint32 members_count;
    group_member_list_group_set_f group_set;
    group_member_list_member_set_f member_set;
    group_member_list_group_get_f group_get;
    group_member_list_member_get_f member_get;
    void *user_data;
}group_member_list_t;

struct member_entry_s{
     member_entry_id_t next;
     member_entry_id_t prev;
     group_entry_id_t group;
};


typedef int (*group_member_list_members_func_f)(member_entry_id_t member_id, int param_a, int param_b);




int group_member_list_init(group_member_list_t* gml);
uint32 group_member_list_member_entry_size_get(void);
uint32 group_member_list_group_entry_size_get(void);


int group_member_list_member_add(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t member_id);


int group_member_list_members_iter_get(group_member_list_t* gml, group_entry_id_t group_id, member_entry_id_t *member_id);
int group_member_list_is_empty_group(group_member_list_t* gml, group_entry_id_t group_id, uint32 *is_empty_group);
int group_member_list_group_get(group_member_list_t* gml, member_entry_id_t member_id, group_entry_id_t *group_id);
int group_member_list_group_members_get(group_member_list_t* gml,
                                               group_entry_id_t group_id,
                                               uint32 max_members,
                                               member_entry_id_t *member_ids,
                                               uint32 *members_count);

/* call callback func for all the group members. the func get additional two params*/
int group_member_list_group_members_func(group_member_list_t* gml,
                                                group_entry_id_t group_id,
                                                group_member_list_members_func_f callback_func,
                                                int param1,
                                                int param2
 );


int group_member_list_group_remove(group_member_list_t* gml, group_entry_id_t group_id);
int group_member_list_member_remove(group_member_list_t* gml, member_entry_id_t member_id);

#endif /*_GROUP_MEMBER_LIST_H_*/
