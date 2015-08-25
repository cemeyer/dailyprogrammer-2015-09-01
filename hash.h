#ifndef	_HASH_H_
#define	_HASH_H_

/* Really a hash set */

struct hash {
	size_t	h_keysz;
	size_t	h_allocated;
	size_t	h_members;

	void	*h_data;
	uint64_t	(*h_hashkey)(const void *);
};

void hash_init(struct hash *, size_t /* key size */,
    uint64_t (*)(const void *));
void hash_add(struct hash *, const void *);
bool hash_contains(struct hash *, const void *);

#endif
