/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_

typedef int(*comparer)(const void*, const void*);
/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
	int size;
	struct node *head;
	comparer cmp;
	int ptr_flag;

} priqueue_t;

typedef struct node
{
	struct node *next;
	void *data;
} node;


void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

void * priqueue_node_at  (priqueue_t *q, int index);

#endif /* LIBPQUEUE_H_ */
