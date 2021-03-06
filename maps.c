#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "trinity.h"
#include "arch.h"
#include "shm.h"

static unsigned int num_mappings = 0;
static struct map *maps_list;

char *page_zeros;
char *page_0xff;
char *page_rand;
char *page_allocs;

void * alloc_shared(unsigned int size)
{
	void *ret;

	ret = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
	if (ret == MAP_FAILED)
		return NULL;

	return ret;
}

static struct map * alloc_map(void)
{
	struct map *newmap;

	newmap = malloc(sizeof(struct map));
	if (!newmap) {
		printf("Couldn't allocate maps list!\n");
		exit(EXIT_FAILURE);
	}
	memset(newmap, 0, sizeof(struct map));
	return newmap;
}

static void dump_maps(void)
{
	struct map *tmpmap = maps_list;
	unsigned int j;

	output(2, "There are %d entries in the map table\n", num_mappings);

	for (j = 0; j < num_mappings; j++) {
		output(2, " start: %p  name: %s\n", tmpmap->ptr, tmpmap->name);
		tmpmap = tmpmap->next;
	}
}

void * alloc_zero_map(struct map *map, int prot, const char *name)
{
	struct map *tmpmap = map;
	int fd;
	unsigned long size;

	if (!tmpmap)
		tmpmap = alloc_map();

	fd = open("/dev/zero", O_RDWR);
	if (!fd) {
		printf("open /dev/zero failure\n");
		exit(EXIT_FAILURE);
	}

	/* page_size * 2, so we have a guard page afterwards.
	 * This is necessary for when we want to test page boundaries.
	 * see end of _get_address() for details.
	 */

	if (rand() % 2)
		size = page_size * 2;
	else
		size = 4*1024*1024*2-1;

	tmpmap->ptr = mmap(NULL, size, prot, MAP_ANONYMOUS|MAP_SHARED, -1, 0);


	if (!tmpmap->ptr) {
		printf("mmap /dev/zero failure\n");
		exit(EXIT_FAILURE);
	}
	tmpmap->name = malloc(80);
	sprintf(tmpmap->name, "/dev/zero(%s)", name);
	num_mappings++;

	output(2, "mapping[%d]: (zeropage %s) %p\n", num_mappings - 1, name, tmpmap->ptr);

	close(fd);
	return tmpmap;
}


void setup_maps(void)
{
	struct map *tmpmap;
	unsigned int fd, i;

	tmpmap = maps_list = alloc_map();

	/* Make sure our zero page mappings are nowhere near the shm. */
	fd = open("/dev/zero", O_RDWR);
	for (i = 0; i < 50; i++)
		mmap(NULL, page_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);

	/* Add a bunch of /dev/zero mappings */
	tmpmap->next = alloc_zero_map(tmpmap, PROT_READ | PROT_WRITE, "PROT_READ | PROT_WRITE");
	tmpmap = tmpmap->next;

	tmpmap->next = alloc_zero_map(NULL, PROT_READ, "PROT_READ");
	tmpmap = tmpmap->next;

	tmpmap->next = alloc_zero_map(NULL, PROT_WRITE, "PROT_WRITE");
	tmpmap = tmpmap->next;

	output(2, "Added /dev/zero mappings.\n");
	dump_maps();
}

void * get_map(void)
{
	struct map *tmpmap = maps_list;
	unsigned int i, j;

	i = rand() % num_mappings;
	for (j = 0; j < i; j++)
		tmpmap = tmpmap->next;

	return tmpmap->ptr;
}

void destroy_maps(void)
{
	unsigned int i;
	struct map *thismap = maps_list, *next;

	for (i = 0; i < num_mappings; i++) {
		next = thismap->next;
		free(thismap->name);
		free(thismap);
		thismap = next;
	}
	num_mappings = 0;
}

void init_buffers(void)
{
	unsigned int i;

	output(2, "shm is at %p\n", shm);

	page_zeros = memalign(page_size, page_size * 2);
	if (!page_zeros)
		exit(EXIT_FAILURE);
	memset(page_zeros, 0, page_size);
	output(2, "page_zeros @ %p\n", page_zeros);

	page_0xff = memalign(page_size, page_size * 2);
	if (!page_0xff)
		exit(EXIT_FAILURE);
	memset(page_0xff, 0xff, page_size);
	output(2, "page_0xff @ %p\n", page_0xff);

	page_rand = memalign(page_size, page_size * 2);
	if (!page_rand)
		exit(EXIT_FAILURE);
	memset(page_rand, 0x55, page_size);	/* overwritten below */
	output(2, "page_rand @ %p\n", page_rand);

	page_allocs = memalign(page_size, page_size * 2);
	if (!page_allocs)
		exit(EXIT_FAILURE);
	memset(page_allocs, 0xff, page_size);
	output(2, "page_allocs @ %p\n", page_allocs);

	for (i = 0; i < (page_size / sizeof(unsigned long *)); i++)
		page_allocs[i] = (unsigned long) malloc(page_size);

	setup_maps();

	// regenerate_random_page may end up using maps, so has to be last.
	regenerate_random_page();
}
