#include <stdio.h>
#include "hack_hash.h"

int main(int argc, char *argv[])
{
    HACK_MAP_ITERATOR *it = NULL;
    HACK_HASH_MAP *map = hack_hash_map_new(0);

    hack_hash_map_insert_int(map, "Tom", 10);
    hack_hash_map_insert_int(map, "Jerry", 10);
    hack_hash_map_insert_int(map, "Jim", 12);
    hack_hash_map_insert_int(map, "China", 960);
    hack_hash_map_insert_int(map, "World", 9999);

    it = hack_hash_map_iterator(map);
    hack_hash_map_first(it);

    do {
        const char *key = hack_hash_get_key(it);
        int data = 0;

        hack_hash_get_int(it, &data);
        printf("key = %s, data = %d\n", key, data);
    } while (hack_hash_map_next(it));

    hack_hash_map_clear(map);
    hack_hash_map_del(map);
    return 0;
}
