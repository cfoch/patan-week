#include "qtypes.h"

#ifndef __Q_HASH_H__
#define __Q_HASH_H__

typedef struct _QHashKeyValue QHashKeyValue;
typedef struct _QHashTable QHashTable;

#include "qfunctions.h"

#define Q_HASH_KEY_VALUE(t)             ((QHashKeyValue *) t)
#define Q_HASH_TABLE(t)                 ((QHashTable *) t)
#define DEFAULT_QHASH_TABLE_SIZE        7


struct _QHashKeyValue {
  qpointer key;
  qpointer value;
};

struct _QHashTable {
  int size;
  QSList **table;
  QEqualFunc key_equal_func;
  QHashFunc hash_func;
  int (*QEqualFunc)   (qpointer a, qpointer b);
};

QHashTable*     q_hash_table_new                (QHashFunc hash_func, 
                                                 QEqualFunc equal_func);
void            q_hash_table_insert             (QHashTable * hash_table, qpointer key,
                                                 qpointer value);
QHashKeyValue*  q_hash_table_get_key_value_by_key   (QHashTable * hash_table, 
                                                 qpointer key);
QSList *        q_hash_table_lookup (QHashTable * hash_table, qpointer key);
QSList*         q_hash_table_get_key_values (QHashTable * hash_table);
QHashKeyValue * q_hash_table_get_key_value_by_data (QHashTable * hash_table,
                                                    qpointer data,
                                                    QEqualFunc func);
void            q_hash_table_foreach            (QHashTable * hash_table,
                                                QFunc func, qpointer user_data);

#endif /* __Q_HASH_H__ */
