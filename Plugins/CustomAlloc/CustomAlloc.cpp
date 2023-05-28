#define VMEM_DEBUG_LEVEL 0
#include "VMem.cpp"

void CustomAlloc() __attribute__((constructor));

void CustomAlloc()
{
	printf("Hoodoo you doo?\n");
	printf("Custom alloc loaded!\n");
}

extern "C" void* malloc(size_t size)
{
	return VMem::Alloc(size);
}

extern "C" void* calloc(size_t num, size_t size)
{
	int total = num * size;
	void* ptr = VMem::Alloc(total);
	memset(ptr, 0, total);
	return ptr;
}

extern "C" void* realloc(void* ptr, size_t size)
{
	return VMem::Realloc(ptr, size);
}

extern "C" void free(void* ptr)
{
	VMem::Free(ptr);
}
