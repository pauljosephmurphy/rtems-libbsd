/* 
 * dhcpcd - DHCP client daemon
 * Copyright (c) 2006-2013 Roy Marples <roy@marples.name>
 * All rights reserved

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>

#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

#include "pollts.h"

int
pollts(struct pollfd *restrict fds, nfds_t nfds,
    const struct timespec *restrict ts, const sigset_t *restrict sigmask)
{
	fd_set read_fds;
	nfds_t n;
	int maxfd, r;
	struct timeval tv;
	struct timeval *tvp;

	FD_ZERO(&read_fds);
	maxfd = 0;
	for (n = 0; n < nfds; n++) {
		if (fds[n].events & POLLIN) {
			FD_SET(fds[n].fd, &read_fds);
			if (fds[n].fd > maxfd)
				maxfd = fds[n].fd;
		}
	}

	if (ts != NULL) {
		TIMESPEC_TO_TIMEVAL(&tv, ts);
		tvp = &tv;
	} else {
		tvp = NULL;
	}

	r = select(maxfd + 1, &read_fds, NULL, NULL, tvp);
	if (r > 0) {
		for (n = 0; n < nfds; n++) {
			fds[n].revents =
			    FD_ISSET(fds[n].fd, &read_fds) ? POLLIN : 0;
		}
	}

	return r;
}
