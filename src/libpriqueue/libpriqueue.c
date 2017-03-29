/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->size = 0;
	q->head = NULL;
	q->cmp = comparer;
	q->ptr_flag = 0;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	node* temp = malloc(sizeof(node));
	temp->next = NULL;
	temp->data = ptr;

	if(!q->head)
	{
		q->head = temp;
		q->size++;
		return 0;
	}
	else if(q->cmp && q->cmp(ptr, q->head->data) < 0)
	{
		node *temp1 = q->head;
		q->head = temp;
		temp->next = temp1;
		q->size++;
		return 0;
	}

	int index = 0;
	node *i = q->head;
	node *i_prev = i;
	while(i)
	{
		if(q->cmp && q->cmp(ptr, i->data) < 0)
		{
			break;
		}
		i_prev = i;
		i = i->next;
		index++;
	}
	i_prev->next = temp;
	temp->next = i;
	q->size++;
	return index;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(q->size > 0)
	{
		return q->head->data;
	}
	return NULL;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(q->size > 0)
	{
		node *i = q->head;
		void *data = q->head->data;
		q->head = q->head->next;
		free(i);
		q->size--;
		return data;
	}
	return NULL;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	if(index >= 0 && index < q->size)
	{
		int x = 0;
		node *i = q->head;
		while(x != index)
		{
			i = i->next;
			x++;
		}
		return i->data;
	}
	return NULL;
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{
	int temp = 0;
	while(q->size > 0 && q->head->data == ptr)
	{
		void *data = priqueue_poll(q);
		++temp;
	}
	if(q->size > 0)
	{
		node *i = q->head;
		node *i_prev = i;
		do
		{
			if(i->data == ptr)
			{
				i_prev->next = i->next;
				free(i);
				i = i_prev->next;
				++temp;
				q->size--;
			}
			i_prev = i;
			if(i)
			{
				i = i->next;
			}
		}while(i);
	}
	return temp;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	if(index == 0)
	{
		return priqueue_poll(q);
	}
	if(index >= 0 && index < q->size)
	{
		int x = 0;
		node *i = q->head;
		node *i_prev = i;
		while(x != index)
		{
			i_prev = i;
			i = i->next;
			x++;
		}
		void *data = i->data;
		i_prev->next = i->next;
		free(i);
		q->size--;
		return data;
	}
	return 0;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	node *i = q->head;
	while(i)
	{
		node *i_free = i;
		i = i->next;
		if(q->ptr_flag)
		{
			free(i_free->data);
		}
		free(i_free);
	}
}

void *priqueue_node_at(priqueue_t *q, int index)
{
  if(index >= 0 && index < q->size){
    int x = 0;
    node *i = q->head;
    while(x != index){
      i = i->next;
      x++;
    }
    return i;
  }
  return NULL;
}