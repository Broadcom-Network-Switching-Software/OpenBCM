/*! \file libc_qsort.c
 *
 * Non-recursive qsort.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_qsort

/*
 * Convenience macros to make code easier to read.
 */
#define SWAP(_a, _b) swap_chars((char *)(_a), (char *)(_b), elsz)
#define COMP(_a, _b) (*cmpfn)((void *)(_a), (void *)(_b))

/*
 * Stack size must be at least log2(numel), and since numel is of type
 * size_t, we simply use the number of bits in size_t.
 */
#define STKSZ   (sizeof(size_t) * 8)

/*
 * Subfiles of T or fewer elements will be sorted by a simple
 * insertion sort. Note! T must be at least 3.
 */
#define T       7

/*
 * Swap nbytes between a and b.
 */
static void
swap_chars(char *a, char *b, size_t nbytes)
{
   char tmp;

   do {
       tmp = *a;
       *a++ = *b;
       *b++ = tmp;
   } while (--nbytes);
}

void
sal_qsort(void *arr, size_t numel,
          size_t elsz, int (*cmpfn)(const void *, const void *))
{
   char *stack[STKSZ*2], **sp;  /* Each stack node is 2 entries   */
   char *i, *j, *limit;         /* Scan and limit pointers        */
   long thresh;                 /* Size of T elements in bytes    */
   char *base;                  /* Base pointer as char *         */

   /* Set up base pointer, threshold and stack pointer */
   base = (char *)arr;
   thresh = T * elsz;
   sp = stack;

   /* Point past end of array */
   limit = base + numel * elsz;

   while (1) {
       if (limit - base > thresh) {
           /* If more than T elements then swap base with middle */
           SWAP(((((size_t)(limit-base))/elsz)/2)*elsz+base, base);
           /* i scans left to right */
           i = base + elsz;
           /* j scans right to left */
           j = limit - elsz;
           /* Set things up so that *i <= *base <= *j */
           if (COMP(i, j) > 0) {
               SWAP(i, j);
           }
           if (COMP(base, j) > 0) {
               SWAP(base, j);
           }
           if (COMP(i, base) > 0) {
               SWAP(i, base);
           }
           while (1) {
               /* Move i right until *i >= pivot */
               do {
                   i += elsz;
               } while (COMP(i, base) < 0);
               /* Move j left until *j <= pivot */
               do {
                   j -= elsz;
               } while (COMP(j, base) > 0);
               /* Break if pointers crossed */
               if (i > j) {
                   break;
               }
               /* Swap elements and keep scanning */
               SWAP(i, j);
           }
           /* Move pivot into correct place */
           SWAP(base, j);
           if (j - base > limit - i) {
               /* Left subfile larger */
               /* Push left subfile base and limit */
               sp[0] = base;
               sp[1] = j;
               /* Sort the right subfile */
               base = i;
           } else {
               /* Right subfile larger */
               /* Push right subfile base and limit */
               sp[0] = i;
               sp[1] = limit;
               /* Sort the left subfile */
               limit = j;
           }
           /* Increment stack pointer */
           sp += 2;
       } else {
           /* Subfile is small, use insertion sort */
           for (j = base, i = j+elsz; i < limit; j = i, i += elsz) {
               for (; COMP(j, j+elsz) > 0; j -= elsz) {
                   SWAP(j, j+elsz);
                   if (j == base) {
                       break;
                   }
               }
           }
           if (sp != stack) {
               /* If any entries on stack, pop the base and limit */
               sp -= 2;
               base = sp[0];
               limit = sp[1];
           } else {
               /* Stack empty, done */
               break;
           }
       }
   }
}

#endif
