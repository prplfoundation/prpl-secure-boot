/*
Copyright (c) 2016, prpl Foundation

Permission to use, copy, modify, and/or distribute this software for any purpose with or without 
fee is hereby granted, provided that the above copyright notice and this permission notice appear 
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE 
FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

This code was written by Carlos Moratelli at Embedded System Group (GSE) at PUCRS/Brazil.

*/

/**
 * @file malloc.c
 * 
 * @section DESCRIPTION
 * 
 * Memory allocator using first-fit
 * 
 * Simple linked list of used/free areas. malloc() is slower, because free areas
 * are searched from the beginning of the heap and are coalesced on demand. Yet,
 * just one sweep through memory areas is performed, making it faster than other
 * allocators on the average case. free() is very fast, as memory areas
 * are just marked as unused.
 * 
 */

#include <axtls-lite/os_int.h>
#include <axtls-lite/os_port.h>
#include <axtls-lite/malloc.h>

#define HEAP_SIZE 4096
static unsigned char _memory_pool[HEAP_SIZE];

#define _heap_start _memory_pool
uint32_t _heap_size = HEAP_SIZE;

static uint32_t krnl_free = 0;
static void* krnl_heap = NULL;

void axtls_free(void *ptr)
{
	struct mem_block *p;

	init_mem();

	p = ((struct mem_block *)ptr) - 1;
	p->size &= ~1L;
	krnl_free += p->size + sizeof(struct mem_block);
}

void *axtls_malloc(size_t size)
{
	struct mem_block *p, *q, *r, n;
	size_t psize;

	init_mem();

	size = align4(size);
	
	p = (struct mem_block *)krnl_heap;
	q = p;
	
	while (p->next){
		while (p->size & 1){
			p = p->next;
			q = p;
		}
		while (!(p->size & 1) && p->next) p = p->next;
		if (p){
			q->size = (size_t)p - (size_t)q - sizeof(struct mem_block);
			q->next = p;
		}
		if (!p->next && q->size < size + sizeof(struct mem_block)){
			return 0;
		}
		if (q->size >= size + sizeof(struct mem_block)){
			p = q;
			break;
		}
	}
	
	psize = (p->size & ~1L) - size - sizeof(struct mem_block);
	
	r = p->next;
	p->next = (struct mem_block *)((size_t)p + size + sizeof(struct mem_block));
	p->size = size | 1;
	
	n.next = r;
	n.size = psize;
	*p->next = n;
	krnl_free -= size + sizeof(struct mem_block);
	
	return (void *)(p + 1);
}

void *axtls_calloc(size_t qty, size_t type_size)
{
	void *buf;

	init_mem();

	buf = (void *)axtls_malloc((qty * type_size));
	if (buf)
		memset(buf, 0, (qty * type_size));
	
	return (void *)buf;
}

void *axtls_realloc(void *ptr, size_t size){
	void *buf;

	init_mem();

	if ((int32_t)size < 0) return NULL;
	if (ptr == NULL)
		return (void *)axtls_malloc(size);
	
	buf = (void *)axtls_malloc(size);
	if (buf){
		memcpy(buf, ptr, size);
		axtls_free(ptr);
	}
	
	return (void *)buf;
}


void _init_mem()
{
	krnl_heap =  (struct mem_block *) ((uint32_t)(_heap_start + 8) & 0xFFFFFFFC);
	uint32_t len = ((uint32_t)(_heap_size) -8) & 0xFFFFFFFC;

	struct mem_block *p = (struct mem_block *)krnl_heap;
	struct mem_block *q = (struct mem_block *)  ((size_t)  (struct mem_block *)krnl_heap + sizeof(krnl_heap) - (sizeof(struct mem_block)));

	len = align4(len);
	p->next = q;
	p->size = len - sizeof(struct mem_block) - sizeof(struct mem_block);
	q->next = NULL;
	q->size = 0;
	krnl_free = p->size;
}

void init_mem()
{
	if (krnl_heap == NULL)
		_init_mem();
}




