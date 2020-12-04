#ifndef _SB__DQ_H_
#define _SB__DQ_H_
/* --------------------------------------------------------------------------
**
** 
**
** This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
** 
** Copyright 2007-2020 Broadcom Inc. All rights reserved.
**
** sbDq.h: doubly linked lists
**
** --------------------------------------------------------------------------*/

/*
 * Singly linked list (stack) structure
 */
#define SL_INIT(l)  do { (l) = NULL; } while (0)
#define SL_EMPTY(l) (!(l))

#define SL_INSERT_HEAD(l, e)          \
do                                    \
{	 		              \
  *((void **) (e)) = (void *) (l);    \
  ((void *) (l)) = (void *) (e);      \
} while (0)

#define SL_REMOVE_HEAD(l, e)          \
do                                    \
{				      \
  ((void *) e) = (void *) (l);	      \
  ((void *) (l)) = *((void **) (e));  \
} while (0)



/*
 * Doubly linked queue structure
 */

typedef struct dq_s *dq_p_t;
typedef struct dq_s
{
  volatile dq_p_t flink;	/* Forward link  */
  volatile dq_p_t blink;	/* Backward link */
}
dq_t;


#define DQ_INIT(q)  \
do                  \
{                   \
  (q)->flink = (q); \
  (q)->blink = (q); \
} while (0)

/* true if element (e) is not in a list */
#define DQ_NULL(e) (((e)->flink == (e)) && ((e)->blink == (e)))

/* true if queue (q) is empty */
#define DQ_EMPTY(q) ((q)->flink == (q))

#define DQ_HEAD(q, t) ((t) (q)->flink)
#define DQ_TAIL(q, t) ((t) (q)->blink)
#define DQ_NEXT(q, t) ((t) (q)->flink)
#define DQ_PREV(q, t) ((t) (q)->blink)

/*
 * Arguments are:
 *   q: pointer to queue block
 *   e: pointer to element (DQ) to insert
*/
#define DQ_INSERT_HEAD(q, e)      \
do                                \
{                                 \
  dq_p_t pElement;                \
  pElement = (dq_p_t) (e);        \
  pElement->flink = (q)->flink;   \
  pElement->blink =  (q);         \
  (q)->flink->blink = pElement;   \
  (q)->flink = pElement;          \
} while (0)


/*
 * Arguments are:
 *   q: pointer to queue block
 *   e: pointer to element (DQ) to insert
 */
#define DQ_INSERT_TAIL(q, e)      \
do                                \
{                                 \
  dq_p_t pElement;                \
  pElement = (dq_p_t) (e);        \
  pElement->flink = (q);          \
  pElement->blink = (q)->blink;   \
  (q)->blink->flink = pElement;   \
  (q)->blink = pElement;          \
} while (0)


/*
 * Arguments are:
 *   e: pointer to previous element
 *   n: pointer to new element to insert
 * CHECK If pNew is head before using
*/
#define DQ_INSERT_PREV(e, n)      \
do                                \
{                                 \
  dq_p_t pElement;                \
  dq_p_t _pNew;                    \
  dq_p_t pPrev;                   \
  pElement = (dq_p_t) (e);        \
  _pNew = (dq_p_t) (n);            \
  pPrev = pElement->blink;        \
  pPrev->flink = _pNew;            \
  _pNew->blink =  pPrev;           \
  _pNew->flink = pElement;         \
  pElement->blink = _pNew;         \
} while (0)

/*
 * Arguments are:
 *   e: pointer to next element
 *   n: pointer to new element to insert
 * CHECK If pNew is tail before using
*/
#define DQ_INSERT_NEXT(e, n)      \
do                                \
{                                 \
  dq_p_t pElement;                \
  dq_p_t _pNew;                    \
  dq_p_t pNext;                   \
  pElement = (dq_p_t) (e);        \
  _pNew = (dq_p_t) (n);            \
  pNext = pElement->flink;        \
  pNext->blink = _pNew;            \
  _pNew->flink =  pNext;           \
  pElement->flink = _pNew;         \
  _pNew->blink = pElement;         \
} while (0)

/*
 * Argument is:
 *   e: pointer to element (DQ) to remove
 */
#define DQ_REMOVE(e)                         \
do                                           \
{                                            \
  dq_p_t pElement;                           \
  pElement = (dq_p_t) (e);                   \
  pElement->blink->flink = pElement->flink;  \
  pElement->flink->blink = pElement->blink;  \
} while (0)


/*
 * Arguments are:
 *   q: pointer to queue block
 *   e: pointer to element (DQ) removed from the head of q
 */
#define DQ_REMOVE_HEAD(q, e)                               \
do                                                         \
{                                                          \
  dq_p_t pElement;                           \
  pElement = (dq_p_t) (e);                   \
  pElement = (q)->flink;                     \
  e = (void *)pElement;                      \
  pElement->blink->flink = pElement->flink;  \
  pElement->flink->blink = pElement->blink;  \
} while (0)

/*
 * Arguments are:
 *   q: pointer to queue block
 *   e: pointer to element (DQ) removed from the head of q
 */
#define DQ_REMOVE_TAIL(q, e)                               \
do                                                         \
{                                                          \
  dq_p_t pElement;                           \
  pElement = (dq_p_t) (e);                   \
  pElement = (q)->blink;                     \
  e = (void *)pElement;                      \
  pElement->blink->flink = pElement->flink;  \
  pElement->flink->blink = pElement->blink;  \
} while (0)

/*
 * Arguments new list head, old list head
 * Used to copy list head from one memory area
 * to a different memory area without affecting
 * contents of list
 */
#define DQ_SWAP_HEAD(n, o)               \
do                                       \
{                                        \
   (n)->flink = (o)->flink;              \
   (n)->blink = (o)->blink;              \
   (o)->flink->blink = (n);              \
   (o)->blink->flink = (n);              \
} while (0)                     

/*
 * This macro is a tidy way of performing subtraction to move from a
 * pointer within an object to a pointer to the object.
 *
 * Arguments are:
 *    type of object to recover (e.g. dq_p_t)
 *    pointer to object from which to recover element pointer
 *    pointer to an object of type t
 *    name of the DQ field in t through which the queue is linked
 * Returns:
 *    a pointer to the object, of type t
 */
#define DQ_ELEMENT(t, p, ep, f) \
  ((t) (((char *) (p)) - (((char *) &((ep)->f)) - ((char *) (ep)))))

/*
 * DQ_ELEMENT_GET performs the same function as DQ_ELEMENT, but does not
 * require a pointer of type (t).  This form is preferred as DQ_ELEMENT
 * typically generate Coverity errors, and the (ep) argument is unnecessary.
 *
 * Arguments are:
 *    type of object to recover (e.g. dq_p_t)
 *    pointer to object from which to recover element pointer
 *    name of the DQ field in t through which the queue is linked (t->dq_t)
 * Returns:
 *    a pointer to the object, of type t
 */
#define DQ_ELEMENT_GET(t, p, f) \
  ((t) (((char *) (p)) - (((char *) &(((t)(0))->f)))))

/*
 * Arguments:
 *   q:  head of queue on which to map f
 *   f:  function to apply to each element of q
 */
#define DQ_MAP(q, f, a)                                  \
do                                                       \
{                                                        \
  dq_p_t e, e0, q0;                                      \
  q0 = (q);                                              \
  for (e=q0->flink; e != q0; e = e0) {                   \
    e0 = e->flink;                                       \
    (void) (f)(e, a);                                    \
  }                                                      \
} while (0)

/*
 * Arguments:
 *   q:  head of queue on which to map f
 *   f:  function to apply to each element of q; should return 1 when
 *       desired queue element has been found.
 */
#define DQ_FIND(q, f, a)                                 \
do                                                       \
{                                                        \
  dq_p_t e, e0, q0;                                      \
  q0 = (q);                                              \
  for (e=q0->flink; e != q0; e = e0) {                   \
    e0 = e->flink;                                       \
    if ((f)(e, a)) break;                                \
  }                                                      \
} while (0)

/*
 * Arguments:
 *   q:  head of queue to determine length
 */
#define DQ_LENGTH(q, cnt)                                \
do                                                       \
{                                                        \
  dq_p_t e, q0;                                          \
  (cnt) = 0;                                             \
  q0 = (q);                                              \
  for (e = q0->flink; e != q0; e = e->flink) {           \
    (cnt)++;                                             \
  }                                                      \
} while (0)                                              \

/*
 * Arguments:
 *   q:  head of queue
 *   e:  each elem during traverse
 */
#define DQ_TRAVERSE(q, e)                                \
do                                                       \
{                                                        \
  dq_p_t e0, q0;                                         \
  q0 = (q);                                              \
  for (e=q0->flink; e != q0; e = e0) {                   \
    e0 = e->flink;                                       \

/*
 * Arguments:
 *   q:  head of queue
 *   e:  each elem during traverse
 */
#define DQ_BACK_TRAVERSE(q, e)                           \
do                                                       \
{                                                        \
  dq_p_t e0, q0;                                         \
  q0 = (q);                                              \
  for (e=q0->blink; e != q0; e = e0) {                   \
    e0 = e->blink;                                       \

#define DQ_TRAVERSE_END(q, e)                            \
  }                                                      \
} while (0)

#endif  /* _SB__DQ_H_ */
