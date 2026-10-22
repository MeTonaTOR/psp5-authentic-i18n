#ifndef RECCACHE_H
#define RECCACHE_H

/* A cache, kept on the memory card between runs, of something that takes a while to work out for a file
 * (the tags of an MP3, say). A record is only used if the file still has the same size and time.
 *
 * The file is a header and then `count` records, each the file's size, its time and a payload of a
 * fixed size. The payload starts with the path of the file as a C string, which is the key.
 *
 * Nothing here touches files: the caller reads the whole cache file into memory (reccache_open) and,
 * to save, builds the file in memory (reccache_header, reccache_record) and writes it. */

#define RECCACHE_HEADER 16 /* bytes */

typedef struct {
    const unsigned char *first; /* first record, in the caller's memory */
    unsigned count;
    unsigned stride; /* bytes per record */
    unsigned payload;
    int *table; /* hash table: record number + 1, 0 = empty */
    unsigned mask;
} RecCache;

/* Bytes of a whole cache file with `count` records. */
unsigned reccache_file_size(unsigned payload_size, unsigned count);

/* Uses `file` (len bytes, must stay valid until reccache_close) as a cache, if it has the right magic
 * number and payload size, no more than max_count records, and every key ends within its first
 * key_max bytes. Returns 1 if it is usable. */
int reccache_open(RecCache *c, const void *file, unsigned len, unsigned magic, unsigned payload_size,
                  unsigned max_count, unsigned key_max);

/* The payload of the record for `path` if its file still has this size and time, else NULL. */
const void *reccache_find(const RecCache *c, const char *path, unsigned size, unsigned time);

void reccache_close(RecCache *c);

/* Writes the header of a cache file to out[RECCACHE_HEADER], and record number i of `count` to its place. */
void reccache_header(void *out, unsigned magic, unsigned payload_size, unsigned count);
void reccache_record(void *file, unsigned payload_size, unsigned i, unsigned size, unsigned time, const void *payload);

#endif
