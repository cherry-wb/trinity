/*
 * SYSCALL_DEFINE0(setsid)
 */
#include "trinity.h"
#include "sanitise.h"

struct syscall syscall_setsid = {
	.name = "setsid",
	.num_args = 0,
};
