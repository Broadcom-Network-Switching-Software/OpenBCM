

#include "kaps_jr1_dbllinklist.h"

kaps_jr1_dbl_link_list *
kaps_jr1_dbl_link_list_init(
    kaps_jr1_dbl_link_list * head)
{
    head->m_back_p = head;
    head->m_next_p = head;

    return head;
}

void
kaps_jr1_dbl_link_list_insert(
    kaps_jr1_dbl_link_list * self,
    kaps_jr1_dbl_link_list * node)
{
    kaps_jr1_dbl_link_list *next = (kaps_jr1_dbl_link_list *) self->m_next_p;

    self->m_next_p = node;
    node->m_back_p = self;

    next->m_back_p = node;
    node->m_next_p = next;
}

void
kaps_jr1_dbl_link_list_remove(
    kaps_jr1_dbl_link_list * node,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode,
    void *arg)
{
    kaps_jr1_dbl_link_list *back = (kaps_jr1_dbl_link_list *) node->m_back_p;
    kaps_jr1_dbl_link_list *next = (kaps_jr1_dbl_link_list *) node->m_next_p;

    back->m_next_p = next;
    next->m_back_p = back;

    destroyNode(node, arg);
}

void
kaps_jr1_dbl_link_list_destroy(
    kaps_jr1_dbl_link_list * head,
    kaps_jr1_dbl_link_list_destroy_node_t destroyNode,
    void *arg)
{
    kaps_jr1_dbl_link_list *here = (kaps_jr1_dbl_link_list *) head->m_next_p;
    kaps_jr1_dbl_link_list *next;

    while (here != head)
    {
        next = (kaps_jr1_dbl_link_list *) here->m_next_p;
        destroyNode(here, arg);

        here = next;
    }

    destroyNode(head, arg);
}
