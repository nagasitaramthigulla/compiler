#pragma once
#include<stdlib.h>
#include<conio.h>

int strlen(char *s)
{
	int i = 0;
	while (s[i] != '\0')
	{
		i++;
	}
	return i;
}

void strcpy(char *in, char **out)
{
	int i;
	for (i = 0; in[i]; i++)
		(*out)[i] = in[i];
	(*out)[i] = '\0';
}

int count(char *s, char c)
{
	int j = 0;
	for (int i = 0; s[i]; i++)
		if (s[i] == c)
			j++;
	return j;
}

char ** strtok(char *str, char d, int  *c)
{
	int i, j, k;
	i = j = k = 0;
	char **strs = (char **)malloc(sizeof(char*)*(count(str, d) + 1));
	strs[0] = (char *)malloc(sizeof(char) * 20);
	for (i = 0, j = 0, k = 0; str[i]; i++)
	{
		if (str[i] == d)
		{
			strs[j][k] = '\0';
			j++;
			k = 0;
			strs[j] = (char *)malloc(sizeof(char) * 20);
		}
		else {
			strs[j][k++] = str[i];
			if (k % 20 == 0)
				strs[j] = (char *)realloc(strs[j], sizeof(char) * (k + 20));
		}
	}
	*c = j+1;
	strs[j][k] = '\0';
	return strs;
}

int isnum(char  *s)
{
	if (s == 0)
		return 0;
	int i;
	for (i = 0; s[i]; i++)
		if (s[i]<'0' || s[i]>'9')
			return 0;
	return 1;
}

int strtoint(char *s)
{
	int n = 0;
	if (s == 0 || s[0] == '\0' || !isnum(s))
		return -1;
	for (int i = 0; s[i]; i++)
	{
		n = n * 10 + s[i] - '0';
	}
	return n;
}

int strcmp(char *s1, char *s2)
{
	int i;
	for (i = 0; s1[i] && s2[i]; i++)
		if (s1[i] != s2[i])
			return 0;
	if (s1[i] != s2[i])
		return 0;
	return 1;
}

void waitforenter()
{
	printf("press enter to continue:");
	while (_getch() != '\r');
}

void mergesort(void **ar, int l, int h, int(*fun)(void *, void *))
{
	int i, j, m, k;
	m = l + (h - l) / 2;
	if (l >= h)
		return;
	mergesort(ar, l, m, fun);
	mergesort(ar, m + 1, h, fun);
	void **tr = (void **)malloc(sizeof(void *)*(h + 1));
	i = l, j = m + 1, k = l;
	while (i <= m && j <= h)
	{
		if (fun(ar[i], ar[j]))
		{
			tr[k++] = ar[i++];
		}
		else
			tr[k++] = ar[j++];
	}
	while (i <= m)
		tr[k++] = ar[i++];
	while (j <= h)
		tr[k++] = ar[j++];
	while (l <= h)
		ar[l] = tr[l++];
}

int sumof(int *a, int c)
{
	int sum = 0;
	for (int i = 0; i < c; i++)
		sum += a[i];
	return sum;
}

void getline(FILE *f, char **s)
{
	int i = 0;
	char c;
	while (!feof(f)&&(c = fgetc(f)) != '\n') {
		(*s)[i++] = c;
	}
	(*s)[i] = '\0';
}