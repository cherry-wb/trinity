/*
 * SYSCALL_DEFINE6(mbind, unsigned long, start, unsigned long, len,
	unsigned long, mode, unsigned long __user *, nmask,
	unsigned long, maxnode, unsigned, flags)
 */

#include "../arch.h"

#define MPOL_F_STATIC_NODES     (1 << 15)
#define MPOL_F_RELATIVE_NODES   (1 << 14)

#include "trinity.h"
#include "sanitise.h"
#include "shm.h"

static void sanitise_mbind(int childno)
{
	unsigned long maxnode;
retry_maxnode:
	shm->a5[childno] &= ~((page_size * 8) - 1);

	maxnode = shm->a5[childno];

	if (maxnode < 2 || (maxnode) > (page_size * 8)) {
		shm->a5[childno] = get_interesting_32bit_value();
		goto retry_maxnode;
	}
}



struct syscall syscall_mbind = {
	.name = "mbind",
	.num_args = 6,
	.arg1name = "start",
	.arg1type = ARG_ADDRESS,

	.arg2name = "len",
	.arg2type = ARG_LEN,

	.arg3name = "mode",
	.arg3type = ARG_RANGE,
	.low3range = 0,
	.hi3range = 5,

	.arg4name = "nmask",
	.arg4type = ARG_ADDRESS,

	.arg5name = "maxnode",
	.arg5type = ARG_RANGE,
	.low5range = 0,
	.hi5range = 32,

	.arg6name = "flags",
	.arg6type = ARG_LIST,
	.arg6list = {
		.num = 2,
		.values = { MPOL_F_STATIC_NODES, MPOL_F_RELATIVE_NODES },
	},
	.sanitise = sanitise_mbind,
	.group = GROUP_VM,
};
