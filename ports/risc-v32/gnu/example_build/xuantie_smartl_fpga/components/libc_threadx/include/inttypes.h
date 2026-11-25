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

#ifndef _PRIV_INTTYPES_H_
#define _PRIV_INTTYPES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include_next <inttypes.h>

#undef PRId32
#undef PRIi32
#undef PRIo32
#undef PRIu32
#undef PRIx32
#undef PRIX32

#undef SCNd32
#undef SCNi32
#undef SCNo32
#undef SCNu32
#undef SCNx32

#define PRId32		__STRINGIFY(d)
#define PRIi32		__STRINGIFY(i)
#define PRIo32		__STRINGIFY(o)
#define PRIu32		__STRINGIFY(u)
#define PRIx32		__STRINGIFY(x)
#define PRIX32		__STRINGIFY(X)

#define SCNd32		__STRINGIFY(d)
#define SCNi32		__STRINGIFY(i)
#define SCNo32		__STRINGIFY(o)
#define SCNu32		__STRINGIFY(u)
#define SCNx32		__STRINGIFY(x)

#ifdef __cplusplus
}
#endif

#endif
