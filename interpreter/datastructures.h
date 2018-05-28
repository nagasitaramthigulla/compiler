#pragma once
#include<stdlib.h>
struct queue {
	void **arr;
	int size;
	int f, r;
};

queue * makequeue()
{
	queue * q = (queue *)malloc(sizeof(queue));
	q->arr = (void **)malloc(sizeof(void*) * 500);
	q->size = 500;
	q->f = q->r = 0;
	return q;
}

void enqueue(queue *q, void * d)
{
	q->arr[(q->r)++] = d;
}
struct stack {
	void **arr;
	int size;
	int t;
};

stack * makestack()
{
	stack * s = (stack *)malloc(sizeof(stack));
	s->arr = (void **)malloc(sizeof(void*) * 500);
	s->size = 500;
	s->t = 0;
	return s;
}

void push(stack *s, void *d)
{
	s->arr[(s->t)++] = d;
}

void *pop(stack *s)
{
	if (s->t == 0)
	{
		return NULL;
	}
	return s->arr[--(s->t)];
}

int isempty(queue *q)
{
	if (q->f == q->r)
		return 1;
	return 0;
}

int emptystack(stack *s)
{
	if (s->t == 0)
		return 1;
	return 0;
}
void *dequeue(queue *q)
{
	if (q->f == q->r)
	{
		q->f = q->r = 0;
		return NULL;
	}
	return q->arr[(q->f)++];
}