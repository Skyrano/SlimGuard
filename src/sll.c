/*  sll.c
 *  SlimGuard
 *  Copyright (c) 2019, Beichen Liu, Virginia Tech
 *  All rights reserved
 */

#include "debug.h"
#include "sll.h"

sll_t* add_head(sll_t* node, sll_t* slist) {

  if (slist == NULL) {
    slist = node;
    slist->next = NULL;
  }  else {
    node->next = slist;
    slist = node;
  }

  return slist;
}

sll_t* add_index(sll_t* node, sll_t* slist, uint32_t index) {

  if (slist == NULL || index == 0) {
    node->next = slist;
    slist = node;
  }  else {
    sll_t* listIndex = slist;
    for (int i = 1; i < index; i++) {
        if (listIndex->next !=  NULL) {
            listIndex = listIndex->next;
        }  else{
            break;
        }
    }
    node->next = listIndex->next;
    listIndex->next = node;
  }
    return slist;
}

sll_t* remove_head(sll_t* slist) {

  if ((slist == NULL) | (slist->next == NULL))
    return NULL;

  slist = slist->next;
  return slist;
}
