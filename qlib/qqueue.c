#include "qlib.h"
#include "qtypes.h"
#include "qlist.h"

#define _qqueue_alloc()     qalloc(QQueue, 1)

QQueue*
q_queue_new ()
{
  QQueue *queue;
  queue = _qqueue_alloc ();
  queue->head = queue->tail = NULL;
  queue->length = 0;
}

void
q_queue_push_head (QQueue * queue, qpointer data)
{
  queue->head = q_list_prepend (queue->head, data);
  if (!queue->tail)
    queue->tail = queue->head;
  queue->length++;
}

void
q_queue_push_tail (QQueue * queue, qpointer data)
{
  /* TODO: Handle error when queue == NULL */

  queue->tail = q_list_append (queue->tail, data);

  if (queue->tail->next)
    queue->tail = queue->tail->next;
  else
    queue->head = queue->tail;
  queue->length++;  
}

/**
 * q_queue_pop_head:
 * @queue: a #QQueue
 *
 * Removes the first element of the queue and returns its data.
 *
 * Returns: the data of the head of the queue. If the queue is empty then
 * it returns NULL.
 */
qpointer
q_queue_pop_head (QQueue * queue)
{
  if (queue->head) {
    QList *head = queue->head;
    qpointer data = head->data;

    queue->head = head->next;
    if (queue->head)
      queue->head->prev = NULL;
    else
      queue->tail = NULL;

    free (head);
    queue->length--;
    return data;
  }
  return NULL;
}

QList *
q_queue_find (QQueue * queue, qpointer data, QEqualFunc func)
{
  return q_list_find_compare (queue->head, data, func);
}

void
q_queue_free (QQueue * queue)
{
  q_list_free (queue->head);
  free (queue);  
}

