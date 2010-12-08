/*
 * SYSCALL_DEFINE5(mq_timedsend, mqd_t, mqdes, const char __user *, u_msg_ptr,
	size_t, msg_len, unsigned int, msg_prio,
	const struct timespec __user *, u_abs_timeout)
 */
{
	.name = "mq_timedsend",
	.num_args = 5,
	.arg1name = "mqdes",
	.arg2name = "u_msg_ptr",
	.arg2type = ARG_ADDRESS,
	.arg3name = "msg_len",
	.arg3type = ARG_LEN,
	.arg4name = "msg_prio",
	.arg5name = "u_abs_timeout",
	.arg5type = ARG_ADDRESS,
},