#include "qlib.h"
#include "qtypes.h"
#include "qslist.h"

#define _qslist_alloc()     qalloc(QSList, 1)

QSList*
q_slist_prepend (QSList * list, qpointer data)
{
  QSList *new_list;

  new_list = _qslist_alloc();
  new_list->next = list;
  new_list->data = data;
  return new_list;
}

QSList*
q_slist_append (QSList * list, qpointer data)
{
  QSList *new_list, *l;

  new_list = _qslist_alloc();
  new_list->next = NULL;
  new_list->data = data;

  l = list;
  if (l) {
    while (l->next)
      l = l->next;
    l->next = new_list;
  } else
    return new_list;
  return list;
}

qboolean
q_slist_is_empty (QSList *list)
{
  return !list;
}

QSList*
q_slist_find_compare (QSList * list, qpointer data, QEqualFunc func)
{
  QSList *l;
  for (l = list; l != NULL; l = l->next)
    if (func (l->data, data))
      break;
  return l;
}

static QSList *
q_slist_sorted_merge (QSList * l1, QSList * l2, QCompareFunc cmp_func)
{
  QSList *ret;
  if (!l1)
    return l2;
  if (!l2)
    return l1;

  if (cmp_func (l1->data, l2->data) >= 0) {
    ret = l2;
    ret->next = q_slist_sorted_merge (l1, l2->next, cmp_func);
  } else {
    ret = l1;
    ret->next = q_slist_sorted_merge (l1->next, l2, cmp_func);
  }

  return ret;  
}

static void
q_slist_split_in_two (QSList * list, QSList ** left, QSList ** right)
{
  QSList *slow, *fast;

  if (!list || !(list->next)) {
    *left = list;
    *right = NULL;
    return;
  }

  slow = list;
  fast = list->next;

  while (fast) {
    fast = fast->next;
    if (fast) {
      slow = slow->next;
      fast = fast->next;
    }
  }

  *right = slow->next;
  slow->next = NULL;
  *left = list;
}


QSList *
q_slist_sort (QSList * list, QCompareFunc cmp_func)
{
  QSList *left, *right;
  QSList *l, *r;

  if (!list || !(list->next)) {
    return list;
  }
  q_slist_split_in_two (list, &left, &right);

  l = q_slist_sort (left, cmp_func);
  r = q_slist_sort (right, cmp_func);

  return q_slist_sorted_merge (l, r, cmp_func);
}

/**
 * q_slist_foreach:
 *
 * Call a function for each element of the list.
 *
 **/

void
q_slist_foreach (QSList * list, QFunc func, qpointer user_data)
{
  QSList *l;
  l = list;
  while (l) {
    func (l->data, user_data);
    l = l->next;
  }
}

/**
 * q_slist_free:
 *
 * Free a list and free each data element.
 *
 * Returns: an empty list.
 **/

void
q_slist_free (QSList * list)
{
  QSList *l, *cur;

  l = list;
  while (l) {
    cur = l;
    l = cur->next;
    free (cur); 
  }
}

