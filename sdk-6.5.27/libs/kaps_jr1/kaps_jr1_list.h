

#ifndef __KAPS_JR1_LIST_H
#define __KAPS_JR1_LIST_H

#include "kaps_jr1_portable.h"
#include "kaps_jr1_utility.h"

#ifdef __cplusplus
extern "C"
{
#endif




#define CONTAINER_OF(ptr, type, member) (type *)((char *)ptr - offsetof(type, member))



    struct kaps_jr1_list_node
    {
        struct kaps_jr1_list_node *next;
                                  
        struct kaps_jr1_list_node *prev;
                                  
        
    };



    struct kaps_jr1_c_list
    {
        struct kaps_jr1_list_node *head;
                                  
        uint32_t count;          
    };



    struct kaps_jr1_c_list_iter
    {
        struct kaps_jr1_list_node *head;
                                 
        struct kaps_jr1_list_node *cur;
                                 
        uint32_t active;        
    };



    static KAPS_JR1_INLINE void kaps_jr1_c_list_reset(
    struct kaps_jr1_c_list *l)
    {
        l->head = NULL;
        l->count = 0;
    }


#define kaps_jr1_c_list_count(n) ((n)->count)



#define kaps_jr1_c_list_head(l) ((l)->head)



    static KAPS_JR1_INLINE void kaps_jr1_c_list_insert_before(
    struct kaps_jr1_c_list *l,
    struct kaps_jr1_list_node *node,
    struct kaps_jr1_list_node *newnode)
    {
        if (l->count > 0)
        {
            newnode->next = node;
            newnode->prev = node->prev;
            newnode->prev->next = newnode;
            node->prev = newnode;

            if (l->head == node)
            {
                l->head = newnode;
            }

        }
        else
        {
            l->head = newnode;
            newnode->next = newnode;
            newnode->prev = newnode;
        }

        l->count++;
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_add_head(
    struct kaps_jr1_c_list *l,
    struct kaps_jr1_list_node *n)
    {
        kaps_jr1_c_list_insert_before(l, l->head, n);
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_add_tail(
    struct kaps_jr1_c_list *l,
    struct kaps_jr1_list_node *n)
    {
        if (l->count > 0)
        {
            n->next = l->head;
            n->prev = l->head->prev;
            n->prev->next = n;
            l->head->prev = n;

        }
        else
        {
            l->head = n;
            n->next = n;
            n->prev = n;
        }

        l->count++;
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_remove_node(
    struct kaps_jr1_c_list *l,
    struct kaps_jr1_list_node *n,
    struct kaps_jr1_c_list_iter *it)
    {
        kaps_jr1_sassert(l->count > 0);
        if (n != n->next)
        {
            n->next->prev = n->prev;
            n->prev->next = n->next;
            if (n == l->head)
                l->head = l->head->next;
            if (it)
            {
                if (it->cur == n)
                    it->cur = n->next;
                if (it->head == n)
                {
                    it->head = l->head;
                    
                    if (it->head == it->cur)
                        it->active = 0;
                }
            }
        }
        else
        {
            l->head = NULL;
            if (it)
            {
                it->head = it->cur = NULL;
            }
        }
        l->count--;
        n->next = n->prev = NULL;
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_concat(
    struct kaps_jr1_c_list *first,
    struct kaps_jr1_c_list *second)
    {
        if (second->count == 0)
            return;

        if (first->count == 0)
        {
            first->count = second->count;
            first->head = second->head;
        }
        else
        {
            struct kaps_jr1_list_node *new_tail = second->head->prev;
            struct kaps_jr1_list_node *old_tail = first->head->prev;
            new_tail->next = first->head;
            old_tail->next = second->head;

            first->head->prev = new_tail;
            second->head->prev = old_tail;
            first->count += second->count;
        }
        kaps_jr1_c_list_reset(second);
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_concat_num(
    struct kaps_jr1_c_list *first,
    struct kaps_jr1_c_list *second,
    uint32_t nelements)
    {
        struct kaps_jr1_list_node *n, *sub_list_head, *sub_list_tail, *sec_tail;
        uint32_t count;

        if (nelements == 0)
            return;

        if (nelements == second->count)
        {
            kaps_jr1_c_list_concat(first, second);
            return;
        }

        kaps_jr1_sassert(second->count > nelements);

        count = 0;
        n = sub_list_head = second->head;
        while (count < nelements)
        {
            n = n->next;
            count++;
        }

        sec_tail = second->head->prev;
        sub_list_tail = n->prev;

        
        sec_tail->next = n;
        n->prev = sec_tail;
        second->head = n;
        second->count -= nelements;

        
        if (first->count)
        {
            first->head->prev->next = sub_list_head;
            sub_list_head->prev = first->head->prev;
            sub_list_tail->next = first->head;
            first->head->prev = sub_list_tail;
        }
        else
        {
            first->head = sub_list_head;
            sub_list_head->prev = sub_list_tail;
            sub_list_tail->next = sub_list_head;
        }

        first->count += nelements;
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_iter_init(
    const struct kaps_jr1_c_list *l,
    struct kaps_jr1_c_list_iter *it)
    {
        it->head = l->head;
        it->cur = l->head;
        it->active = 0;
    }



    static KAPS_JR1_INLINE void kaps_jr1_c_list_iter_init_at_element(
    const struct kaps_jr1_c_list *l,
    struct kaps_jr1_list_node *el,
    struct kaps_jr1_c_list_iter *it)
    {
        it->head = l->head;
        it->cur = el;
        it->active = 0;
    }



    static KAPS_JR1_INLINE struct kaps_jr1_list_node *kaps_jr1_c_list_iter_next(
    struct kaps_jr1_c_list_iter *it)
    {
        if (it->cur)
        {
            if (it->cur != it->head || !it->active)
            {
                struct kaps_jr1_list_node *res = it->cur;
                it->cur = it->cur->next;
                it->active = 1;
                return res;
            }
        }

        return NULL;
    }



#ifdef __cplusplus
}
#endif

#endif 
