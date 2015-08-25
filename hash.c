#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "hash.h"

static void resize(struct hash *, size_t);
enum probehow {
	PROBE_CHECK,
	PROBE_ADD,
};
static bool probe(void *, size_t, size_t, uint64_t, const void *,
    enum probehow);

void
hash_init(struct hash *h, size_t keysz, uint64_t (*hash)(const void *))
{
	h->h_keysz = keysz;
	h->h_allocated = 0;
	h->h_members = 0;
	h->h_data = NULL;
	h->h_hashkey = hash;

	resize(h, 512);
}

void
hash_add(struct hash *h, const void *pdatum)
{
	bool exists;

	if (h->h_members + 1 > (h->h_allocated / 2))
		resize(h, h->h_allocated * 2);

	exists = probe(h->h_data, h->h_allocated, h->h_keysz,
	    h->h_hashkey(pdatum), pdatum, PROBE_ADD);
	if (!exists)
		h->h_members++;
}

bool
hash_contains(struct hash *h, const void *pdatum)
{

	return (probe(h->h_data, h->h_allocated, h->h_keysz,
		h->h_hashkey(pdatum), pdatum, PROBE_CHECK));
}

static bool
probe(void *vdat, size_t allocated, size_t keysz, uint64_t hashval,
    const void *val, enum probehow how)
{
	void *zerokey;
	char *dat;

	if (how != PROBE_CHECK && how != PROBE_ADD)
		errx(EX_OSERR, "i dumb %s:%d", __FILE__, __LINE__);

	zerokey = alloca(keysz);
	memset(zerokey, 0, keysz);

	dat = vdat;

	/* Linear probe */
	hashval %= allocated;
	for (;;) {
		if (memcmp(&dat[keysz * hashval], zerokey, keysz) == 0) {
			if (how == PROBE_ADD)
				memcpy(&dat[keysz * hashval], val, keysz);
			return (false);
		} else if (memcmp(&dat[keysz * hashval], val, keysz) == 0)
			return (true);

		hashval = (hashval + 1) % allocated;
	}
}

static void
resize(struct hash *h, size_t new_alloc)
{
	const char *olddat, *ov;
	void *zerokey;
	char *newdat;
	uint64_t hv;
	unsigned i;

	if (new_alloc <= h->h_allocated)
		errx(EX_OSERR, "I dumb %s:%d", __FILE__, __LINE__);

	newdat = malloc(new_alloc * h->h_keysz);
	if (newdat == NULL)
		errx(EX_OSERR, "malloc");
	memset(newdat, 0, new_alloc * h->h_keysz);

	zerokey = alloca(h->h_keysz);
	memset(zerokey, 0, h->h_keysz);

	olddat = h->h_data;
	for (i = 0; i < h->h_allocated; i++) {
		ov = &olddat[i * h->h_keysz];
		if (memcmp(ov, zerokey, h->h_keysz) == 0)
			continue;

		hv = h->h_hashkey(ov);
		(void)probe(newdat, new_alloc, h->h_keysz, hv, ov, PROBE_ADD);
	}

	free(h->h_data);
	h->h_data = newdat;
	h->h_allocated = new_alloc;
}
