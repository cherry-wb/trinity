#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include "trinity.h"
#include "shm.h"

unsigned int seed = 0;

static void syslog_seed(int seedparam)
{
	fprintf(stderr, "Randomness reseeded to 0x%x\n", seedparam);
	openlog("trinity", LOG_CONS|LOG_PERROR, LOG_USER);
	syslog(LOG_CRIT, "Randomness reseeded to 0x%x\n", seedparam);
	closelog();
}

static int new_seed(void)
{
	struct timeval t;

	gettimeofday(&t, 0);

	return (t.tv_sec * getpid()) ^ t.tv_usec;
}

/*
 * If we passed in a seed with -s, use that. Otherwise make one up from time of day.
 */
int init_seed(unsigned int seedparam)
{
	if (user_set_seed == TRUE)
		printf("[%d] Using user passed random seed: %u (0x%x)\n", getpid(), seedparam, seedparam);
	else {
		seedparam = new_seed();

		printf("Initial random seed from time of day: %u (0x%x)\n", seedparam, seedparam);
	}

	if (do_syslog == TRUE)
		syslog_seed(seedparam);

	return seed;
}


/* Mix in the pidslot so that all children get different randomness.
 * we can't use the actual pid or anything else 'random' because otherwise reproducing
 * seeds with -s would be much harder to replicate.
 */
void set_seed(unsigned int pidslot)
{
	srand(shm->seed + (pidslot + 1));
	srandom(shm->seed + (pidslot + 1));
	shm->seeds[pidslot] = shm->seed;
}

/*
 * Periodically reseed.
 *
 * We do this so we can log a new seed every now and again, so we can cut down on the
 * amount of time necessary to reproduce a bug.
 * Caveat: Not used if we passed in our own seed with -s
 */
void reseed(void)
{
	shm->need_reseed = FALSE;

	if (getpid() != shm->parentpid) {
		output(0, "Reseeding should only happen from parent!\n");
		exit(EXIT_FAILURE);
	}

	/* don't change the seed if we passed -s */
	if (user_set_seed == TRUE)
		return;

	/* We are reseeding. */
	shm->seed = new_seed();

	output(0, "[%d] Random reseed: %u (0x%x)\n", getpid(), shm->seed, shm->seed);

	if (do_syslog == TRUE)
		syslog_seed(shm->seed);
}
