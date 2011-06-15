/*
 * long sys_rt_sigreturn(struct pt_regs *regs)
 */
#include "trinity.h"
#include "sanitise.h"

struct syscall syscall_rt_sigreturn = {
	.name = "rt_sigreturn",
	.num_args = 1,
	.flags = AVOID_SYSCALL,
	.arg1name = "regs",
	.arg1type = ARG_ADDRESS,
};