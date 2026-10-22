#include <stdlib.h>
#include <string.h>
#include "reccache.h"

#define REC_HEADER 8 /* size and time before the payload */

static unsigned get32(const unsigned char *p)
{
    return p[0] | p[1] << 8 | p[2] << 16 | (unsigned)p[3] << 24;
}

static void put32(unsigned char *p, unsigned v)
{
    p[0] = v, p[1] = v >> 8, p[2] = v >> 16, p[3] = v >> 24;
}

static unsigned hash(const char *s)
{
    unsigned h = 2166136261u;
    while (*s)
        h = (h ^ (unsigned char)*s++) * 16777619u;
    return h;
}

unsigned reccache_file_size(unsigned payload_size, unsigned count)
{
    return RECCACHE_HEADER + count * (REC_HEADER + payload_size);
}

void reccache_header(void *out, unsigned magic, unsigned payload_size, unsigned count)
{
    unsigned char *p = out;
    put32(p, magic);
    put32(p + 4, payload_size);
    put32(p + 8, count);
    put32(p + 12, 0);
}

void reccache_record(void *file, unsigned payload_size, unsigned i, unsigned size, unsigned time, const void *payload)
{
    unsigned char *p = (unsigned char *)file + RECCACHE_HEADER + (size_t)i * (REC_HEADER + payload_size);
    put32(p, size);
    put32(p + 4, time);
    memcpy(p + REC_HEADER, payload, payload_size);
}

int reccache_open(RecCache *c, const void *file, unsigned len, unsigned magic, unsigned payload_size,
                  unsigned max_count, unsigned key_max)
{
    memset(c, 0, sizeof(*c));
    const unsigned char *p = file;
    if (!p || len < RECCACHE_HEADER || get32(p) != magic || get32(p + 4) != payload_size || key_max > payload_size)
        return 0;
    unsigned count = get32(p + 8);
    if (count > max_count || reccache_file_size(payload_size, count) != len) /* cut short, or not ours */
        return 0;
    c->first = p + RECCACHE_HEADER;
    c->count = count;
    c->payload = payload_size;
    c->stride = REC_HEADER + payload_size;

    unsigned n = 16;
    while (n < count * 2)
        n <<= 1;
    c->table = calloc(n, sizeof(int));
    if (!c->table)
        return 0;
    c->mask = n - 1;
    for (unsigned i = 0; i < count; i++) {
        const char *key = (const char *)c->first + (size_t)i * c->stride + REC_HEADER;
        if (!memchr(key, 0, key_max)) { /* a path that never ends: a damaged file */
            reccache_close(c);
            return 0;
        }
        unsigned h = hash(key) & c->mask;
        while (c->table[h])
            h = (h + 1) & c->mask;
        c->table[h] = (int)i + 1;
    }
    return 1;
}

const void *reccache_find(const RecCache *c, const char *path, unsigned size, unsigned time)
{
    if (!c->table)
        return NULL;
    for (unsigned h = hash(path) & c->mask; c->table[h]; h = (h + 1) & c->mask) {
        const unsigned char *rec = c->first + (size_t)(c->table[h] - 1) * c->stride;
        if (!strcmp((const char *)rec + REC_HEADER, path))
            return get32(rec) == size && get32(rec + 4) == time ? rec + REC_HEADER : NULL;
    }
    return NULL;
}

void reccache_close(RecCache *c)
{
    free(c->table);
    memset(c, 0, sizeof(*c));
}
