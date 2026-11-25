 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SYS_SELECT_H
#define __SYS_SELECT_H

#include <sys/_timeval.h>

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

typedef unsigned long fd_mask;
typedef struct {
    unsigned long fds_bits[FD_SETSIZE / 8 / sizeof(long)];
} fd_set;

#define FD_ZERO(s)     memset((void*)(s), 0, sizeof(*(s)))
#define FD_SET(d, s)   ((s)->fds_bits[(d) / (8 * sizeof(long))] |= (1UL << ((d) % (8 * sizeof(long)))))
#define FD_CLR(d, s)   ((s)->fds_bits[(d) / (8 * sizeof(long))] &= ~(1UL << ((d) % (8 * sizeof(long)))))
#define FD_ISSET(d, s) (!!((s)->fds_bits[(d) / (8 * sizeof(long))] & (1UL << ((d) % (8 * sizeof(long))))))

#ifdef __cplusplus
extern "C" {
#endif

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
                         struct timeval *timeout);

extern int select2(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
            struct timeval *timeout, void *semaphore);

#ifdef __cplusplus
}
#endif

#endif /*__SYS_SELECT_H*/
