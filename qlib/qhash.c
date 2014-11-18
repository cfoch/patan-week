#include "qlib.h"
#include "qtypes.h"
#include "qhash.h"

#define _qhashtable_alloc()               qalloc(QHashTable, 1)
#define _qhashtable_alloc_table(n)        qalloc(QSList *, n)

#define _qhashkeyvalue_alloc()               qalloc(QHashKeyValue, 1)

QHashTable*
q_hash_table_new (QHashFunc hash_func, QEqualFunc key_equal_func)
{
  int i;
  QHashTable *hash_table;
  hash_table = _qhashtable_alloc();
  hash_table->size = DEFAULT_QHASH_TABLE_SIZE;
  hash_table->key_equal_func = key_equal_func;
  hash_table->hash_func = hash_func;
  hash_table->table = _qhashtable_alloc_table (hash_table->size);

  for (i = 0; i < hash_table->size; i++)
    hash_table->table[i] = NULL;

  return hash_table;
}

QSList *
q_hash_table_lookup (QHashTable * hash_table, qpointer key)
{
  int index;
  index = hash_table->hash_func (hash_table->size, key);
  //printf ("index: %d\n", index);

  return q_slist_find_compare (hash_table->table[index], key,
      hash_table->key_equal_func);
}

void
q_hash_table_insert (QHashTable * hash_table, qpointer key, qpointer value)
{
  QSList *list;
  QHashKeyValue *key_val;

  list = q_hash_table_lookup (hash_table, key);

  if (!list) {
    int index;

    index = hash_table->hash_func (hash_table->size, key);

    key_val = _qhashkeyvalue_alloc ();
    key_val->key = key;
    key_val->value = value;

    hash_table->table[index] =\
        q_slist_prepend (hash_table->table[index], (qpointer) key_val);
  } else {
    key_val = list->data;
    key_val->value = value;
    /* TODO: We should *FREE* (maybe?) the previous key_val->value */
  }
}

QHashKeyValue*
q_hash_table_get_key_value_by_key (QHashTable * hash_table, qpointer key)
{
  QSList *list;

  list = q_hash_table_lookup (hash_table, key);
  return list ? Q_HASH_KEY_VALUE (list->data) : NULL;
}

QSList *
q_hash_table_get_key_values (QHashTable * hash_table)
{
  QSList *l;
  int i;

  l = NULL;
  for (i = 0; i < hash_table->size; i++) {
    QSList *iter;
    iter = hash_table->table[i];
    while (iter) {
      l = q_slist_prepend (l, iter->data);
      iter = iter->next;
    }
  }

  return l;
}

QHashKeyValue * 
q_hash_table_get_key_value_by_data (QHashTable * hash_table, qpointer data,
    QEqualFunc func)
{
  int i;
  for (i = 0; i < hash_table->size; i++) {
    QSList *l;
    l = q_slist_find_compare (hash_table->table[i], data, func);
    if (l)
      return Q_HASH_KEY_VALUE (l->data);
  }
  return NULL;
}

void
q_hash_table_foreach (QHashTable * hash_table, QFunc func, qpointer user_data)
{
  int i;
  for (i = 0; i < hash_table->size; i++)
    q_slist_foreach (hash_table->table[i], func, user_data);
}
