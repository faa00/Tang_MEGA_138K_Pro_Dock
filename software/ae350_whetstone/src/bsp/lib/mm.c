/*
 * ******************************************************************************************
 * File		: mm.c
 * Author 	: GowinSemicoductor
 * Chip		: AE350_SOC
 * Function	: Memory management
 * ******************************************************************************************
 */

// Includes ---------------------------------------------------------------------------------
#include "mm.h"


// Declarations -----------------------------------------------------------------------------
static unsigned int my_malloc(unsigned int size);
static unsigned char my_free(unsigned int offset);
static unsigned char mem_perused(void);


// Definitions ------------------------------------------------------------------------------

#define MEM_BLOCK_SIZE          8
#define MEM_MAX_SIZE            8192		// Self-defined
#define MEM_ALLOC_TABLE_SIZE    MEM_MAX_SIZE/MEM_BLOCK_SIZE

struct _m_malloc_dev
{
	void 			(*init)(void);
	unsigned char	(*perused)(void);
	unsigned char	*membase;
	unsigned char	*memmap;
	unsigned char	memrdy;
};

unsigned char membase[MEM_MAX_SIZE];
unsigned char memmapbase[MEM_ALLOC_TABLE_SIZE];

const unsigned int memtblsize = MEM_ALLOC_TABLE_SIZE;
const unsigned int memblksize = MEM_BLOCK_SIZE;
const unsigned int memsize = MEM_MAX_SIZE;

struct _m_malloc_dev malloc_dev =
{
	mem_init,
	mem_perused,
	membase,
	memmapbase,
	0,
};


// Initialize
void mem_init(void)
{
	mem_set(malloc_dev.memmap, 0, memtblsize*2);
	mem_set(malloc_dev.membase, 0, memsize);
	malloc_dev.memrdy = 1;
}

// Allocate
void* mem_malloc(unsigned int size)
{
	unsigned int offset;

	offset = my_malloc(size);

	if(offset == 0xFFFFFFFF)
	{
		return 0;
	}
	else
	{
		return (void*)((unsigned int)malloc_dev.membase + offset);
	}
}

// Free
void mem_free(void *ptr)
{
	unsigned int offset;

	if(!ptr)
	{
		return;
	}

	offset = (unsigned int)ptr - (unsigned int)malloc_dev.membase;

	my_free(offset);
}

// Compare
int mem_cmp(void *des, void *src, unsigned int n)
{
	int iRet = 0;
	unsigned char *xdes = des;
	unsigned char *xsrc = src;

	while(n--)
	{
		if(*xdes++ == *xsrc++)
		{
			iRet |= 0;	// Equal
		}
		else
		{
			iRet |= 1;	// Not equal
		}
	}

	return iRet;
}

// Copy
void mem_cpy(void *des, void *src, unsigned int n)
{
	unsigned char *xdes = des;
	unsigned char *xsrc = src;

	while(n--)
	{
		*xdes++ = *xsrc++;
	}
}

// Set
void mem_set(void *s, unsigned char c, unsigned int count)
{
	unsigned char *xs = s;

	while(count--)
	{
		*xs++ = c;
	}
}

// Perused
static unsigned char mem_perused(void)
{
	unsigned int used = 0;
	unsigned int i;

	for(i = 0;i < memtblsize;i++)
	{
		if(malloc_dev.memmap[i])
		{
			used++;
		}
	}

	return (used*100)/(memtblsize);
}

// Allocate
static unsigned int my_malloc(unsigned int size)
{
	signed long offset = 0;
	unsigned int nmemb;
	unsigned int cmemb=0;
	unsigned int i;

	if(!malloc_dev.memrdy)
	{
		malloc_dev.init();
	}

	if(size == 0)
	{
		return 0xFFFFFFFF;
	}

	nmemb = size/memblksize;
	if(size%memblksize)
	{
		nmemb++;
	}

	for(offset = memtblsize - 1;offset >= 0;offset--)
	{
		if(!malloc_dev.memmap[offset])
		{
			cmemb++;
		}
		else
		{
			cmemb = 0;
		}

		if(cmemb == nmemb)
		{
			for(i = 0;i < nmemb;i++)
			{
				malloc_dev.memmap[offset+i] = nmemb;
			}

			return (offset*memblksize);
		}
	}

	return 0xFFFFFFFF;
}

// Free
static unsigned char my_free(unsigned int offset)
{
	int i;

	if(!malloc_dev.memrdy)
	{
		malloc_dev.init();
		return 1;
	}

	if(offset < memsize)
	{
		int index = offset/memblksize;
		int nmemb = malloc_dev.memmap[index];

		for(i = 0;i < nmemb;i++)
		{
			malloc_dev.memmap[index+i] = 0;
		}

		return 0;
	}
	else
	{
		return 2;
	}
}
