#include "compiler.hpp"
#include "buddy.hpp"
/*
    All kinds of stubs for functions that prevented a successful linking process
   reside here.
*/
extern "C" int __cxa_atexit([[maybe_unused]] void (*func)(void *),
                            [[maybe_unused]] void *arg,
                            [[maybe_unused]] void *dso_handle)
{
    return 0;
}

// Used by the limine terminal port
USED void *alloc_mem(size_t size) { return buddy.must_alloc(size); }
USED void free_mem(void *ptr, size_t size) {}