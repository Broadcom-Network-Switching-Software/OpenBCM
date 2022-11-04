

#ifndef INCLUDED_KAPS_JR1_DBLLINKLIST_H
#define INCLUDED_KAPS_JR1_DBLLINKLIST_H

#include "kaps_jr1_externcstart.h"

typedef struct kaps_jr1_dbl_link_list kaps_jr1_dbl_link_list;

struct kaps_jr1_dbl_link_list
{
#include "kaps_jr1_dbllinklistdata.h"
};

typedef void (
    *kaps_jr1_dbl_link_list_destroy_node_t) (
    kaps_jr1_dbl_link_list * node,
    void *arg);

kaps_jr1_dbl_link_list *kaps_jr1_dbl_link_list_init(
    kaps_jr1_dbl_link_list *);

void kaps_jr1_dbl_link_list_insert(
    kaps_jr1_dbl_link_list * self,
    kaps_jr1_dbl_link_list * node);

void kaps_jr1_dbl_link_list_remove(
    kaps_jr1_dbl_link_list * node,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode,
    void *arg);

void kaps_jr1_dbl_link_list_destroy(
    kaps_jr1_dbl_link_list * head,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode,
    void *arg);

#include "kaps_jr1_externcend.h"

#endif
