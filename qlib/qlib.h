#ifndef __Q_LIB_H__
#define __Q_LIB_H__

#define INT_TO_QPOINTER(i)        ((qpointer)  (long) (i))
#define QPOINTER_TO_INT(i)        ((int) (long) (i))

#define qalloc(type, n)           ((type *) malloc (n * sizeof (type)))

#define q_min(a, b)               a < b ? a : b
#define q_is_latin_alpha(c)       (isalpha(c) || \
                                  c == 'á' || c == 'é' || c == 'í' || c == 'ó' \
                                  || c == 'ú' || \
                                  c == 'Á' || c == 'É' || c == 'Í' || c == 'Ó' \
                                  || c == 'Ú')

#ifdef DEBUG
  #define Q_DEBUG(fmt, args...) \
      printf("%s:%d-> " fmt "\n", __FILE__, __LINE__, args)
#else
  #define Q_DEBUG(fmt, args...)
#endif

#include <stdlib.h>

#include "qtypes.h"
#include "qslist.h"
#include "qhash.h"
#include "qfunctions.h"
#include "qdate.h"
#include "qlist.h"
#include "qqueue.h"

#endif /* __Q_LIB_H__ */
