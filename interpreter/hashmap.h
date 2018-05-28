#pragma once
#include<stdlib.h>

struct pair {
	void *key;
	void *value;
};

struct hashmap {
	pair **list;
	int buckets;
	int (*hashfun)(void *, int);
};

int hashchar(char c)
{
	return c - 'A';
}

hashmap *createhashmap(int buckets, int(*hashfun)(void *, int))
{
	hashmap *h=(hashmap*)malloc(sizeof(hashmap));
	h->buckets = buckets;
	h->list = (pair**)malloc(sizeof(pair*));
	h->hashfun = hashfun;
	return h;
}

int hashinsert(hashmap *h,void *key,void *val)
{
	int pos = h->hashfun(key,h->buckets);
	if (h->list[pos] != NULL)
	{
		h->list[pos] = (pair *)malloc(sizeof(pair));
		h->list[pos]->key = key;
		h->list[pos]->value = val;
		return 1;
	}
	return 0;
}