
#ifndef __HASHMAP_H
#define __HASHMAP_H

typedef void* XXdict;

extern unsigned long hash_str(char *str);
extern XXdict dict_alloc(int size);
extern void dict_free(XXdict dict);
extern void dict_set(XXdict dict, unsigned long hash, void * value);
extern void dict_remove(XXdict dict, unsigned long hash, void * value);
void * dict_get(XXdict dict, unsigned long hash);

#endif

