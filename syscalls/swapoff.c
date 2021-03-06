/*
 * SYSCALL_DEFINE1(swapoff, const char __user *, specialfile)
 */
#include "trinity.h"
#include "sanitise.h"

struct syscall syscall_swapoff = {
	.name = "swapoff",
	.num_args = 1,
	.arg1name = "specialfile",
	.arg1type = ARG_ADDRESS,
};
