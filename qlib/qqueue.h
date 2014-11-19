#include "qfunctions.h"
#include "qtypes.h"
#include "qlist.h"

#ifndef __Q_QUEUE_H__
#define __Q_QUEUE_H__

typedef struct _QQueue QQueue;

struct _QQueue {
  qpointer data;
  QList *head;
  QList *tail;
  int length;
};

QQueue*         q_queue_new             (void);
void            q_queue_push_head       (QQueue * queue, qpointer data);
void            q_queue_push_tail       (QQueue * queue, qpointer data);
qpointer        q_queue_pop_head        (QQueue * queue);
void            q_queue_free            (QQueue * queue);


#endif /* __Q_QUEUE_H__ */
