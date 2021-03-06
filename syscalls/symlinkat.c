/*
 * SYSCALL_DEFINE3(symlinkat, const char __user *, oldname,
	 int, newdfd, const char __user *, newname)
 */
#include "trinity.h"
#include "sanitise.h"

struct syscall syscall_symlinkat = {
	.name = "symlinkat",
	.num_args = 3,
	.arg1name = "oldname",
	.arg1type = ARG_ADDRESS,
	.arg2name = "newdfd",
	.arg2type = ARG_FD,
	.arg3name = "newname",
	.arg3type = ARG_ADDRESS,
	.flags = NEED_ALARM,
};
