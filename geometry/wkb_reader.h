#ifndef _WKB_READER_H_
#define _WKB_READER_H_

#include <stdio.h>
#include <endian.h>

#include "geometry.h"

#define WKB_BIG_ENDIAN        0
#define WKB_LITTLE_ENDIAN     1
#define WKB_ENDIAN(x, e)      (((e) == WKB_BIG_ENDIAN) ? be32toh((x)) : le32toh((x)))

#define ERRWKB                -1

struct geometry_t *wkb_read_geometry(void *wkb, size_t *wkb_len);

#endif
