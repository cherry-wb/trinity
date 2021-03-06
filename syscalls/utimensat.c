/*
 * SYSCALL_DEFINE4(utimensat, int, dfd, const char __user *, filename,
	 struct timespec __user *, utimes, int, flags)
 */
#include "trinity.h"
#include "sanitise.h"

struct syscall syscall_utimensat = {
	.name = "utimensat",
	.num_args = 4,
	.arg1name = "dfd",
	.arg1type = ARG_FD,
	.arg2name = "filename",
	.arg2type = ARG_PATHNAME,
	.arg3name = "utimes",
	.arg3type = ARG_ADDRESS,
	.arg4name = "flags",
	.flags = NEED_ALARM,
};
