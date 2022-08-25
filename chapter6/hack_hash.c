#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "hack_link.h"
#include "hack_hash.h"

/*===========================================================================*/

/* hash 表默认及最大、最小长度 */
#define HACK_HASH_DEF_LEN 509
#define HACK_HASH_MAX_LEN 69997
#define HACK_HASH_MIN_LEN 31

/* 数据类型 */
#define HACK_HASH_DATA_ADDR 0
#define HACK_HASH_DATA_INT  1

/* 继承自 struct hack_link */
struct hack_hash_link {
    struct hack_hash_node *head;
    struct hack_hash_node *tail;

    int count;
};

/* 继承自 struct hack_link_node */
struct hack_hash_node {
    struct hack_hash_node *prev;
    struct hack_hash_node *next;

    char *key;
    int type;

    union {
        void *ptr;
        int i_data;
    };
};

struct hack_hash_map
{
    struct hack_hash_link *hash_link_list;  /* hash 链表 */
    HACK_MAP_ITERATOR *iterator;            /* 迭代器 */

    int node_count;                         /* 当前的节点总数 */
    int list_len;                           /* 散列表长度 */
    int max_link;                           /* 最长链表长度 */
    int alloc_size;                         /* 表占用的空间大小 */
};

struct hack_map_iterator
{
    HACK_HASH_MAP *map;
    struct hack_hash_node *cur;

    int link_index;
    int node_index;
};

/*===========================================================================*/

/* 字符串散列值 */
static int get_string_hash(const char *key);

/* 获取链表节点 */
static int get_hash_node(
    struct hack_hash_link *link,
    const char *key,
    struct hack_hash_node **node
);

/*===========================================================================*/

HACK_HASH_MAP *hack_hash_map_new(int len)
{
    HACK_HASH_MAP *map = NULL;
    struct hack_hash_link *hash_link_list = NULL;
    HACK_MAP_ITERATOR *iterator = NULL;
    int alloc_size = 0;

    if (!len) {
        len = HACK_HASH_DEF_LEN;
    }

    if (len < HACK_HASH_MIN_LEN) {
        len = HACK_HASH_MIN_LEN;
    }

    if (len > HACK_HASH_MAX_LEN) {
        len = HACK_HASH_MAX_LEN;
    }

    map = malloc(sizeof(HACK_HASH_MAP));
    if (!map) {
        return NULL;
    }
    memset(map, 0, sizeof(HACK_HASH_MAP));

    iterator = malloc(sizeof(HACK_MAP_ITERATOR));
    if (!map) {
        goto err;
    }
    memset(iterator, 0, sizeof(HACK_MAP_ITERATOR));

    iterator->map = map;
    iterator->cur = NULL;
    iterator->link_index = 0;
    iterator->node_index = 0;

    alloc_size = sizeof(struct hack_hash_link) * len;
    hash_link_list = (struct hack_hash_link *)malloc(alloc_size);
    if (!hash_link_list) {
        goto err;
    }
    memset(hash_link_list, 0, alloc_size);

    map->node_count = 0;
    map->list_len = len;
    map->max_link = 0;

    map->alloc_size = alloc_size;
    map->hash_link_list = hash_link_list;
    map->iterator = iterator;

    return map;

err:
    if (hash_link_list) {
        free(hash_link_list);
    }

    if (iterator) {
        free(iterator);
    }

    free(map);
    return NULL;
}

void hack_hash_map_del(HACK_HASH_MAP *map)
{
    if (!map) {
        return;
    }

    map->alloc_size = 0;
    map->max_link = 0;
    map->list_len = 0;
    map->node_count = 0;

    if (map->hash_link_list) {
        free(map->hash_link_list);
        map->hash_link_list = NULL;
    }

    if (map->iterator) {
        free(map->iterator);
        map->iterator = NULL;
    }

    free(map);
}

void hack_hash_map_clear(HACK_HASH_MAP *map)
{
    int i;

    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;

    if (!map) {
        return;
    }

    link = map->hash_link_list;

    for (i = 0; i < map->list_len; i++) {
        while (link->count > 0) {
            node = (struct hack_hash_node *)hack_link_pop((struct hack_link *)link);

            if (node) {
                free(node->key);
                free(node);
            }
        }

        link++;
    }
}

void hack_hash_map_clear_item(HACK_HASH_MAP *map, hack_hash_visit_item visit_item)
{
    int i;

    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;

    if (!map) {
        return;
    }

    link = map->hash_link_list;

    for (i = 0; i < map->list_len; i++) {
        while (link->count > 0) {
            node = (struct hack_hash_node *)hack_link_pop((struct hack_link *)link);
            if (node) {
                if (visit_item) {
                    visit_item(node->key, node->ptr);
                }

                free(node->key);
                free(node);
            }
        }

        link++;
    }
}

int hack_hash_map_node_count(HACK_HASH_MAP *map)
{
    if (map) {
        return map->node_count;
    }
    return 0;
}

int hack_hash_map_insert_ptr(HACK_HASH_MAP *map, const char *key, void *data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;
    char *kbuf = NULL;
    size_t klen = 0;

    if (!map || !key) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    /* 哈希节点存在则直接替换数据地址，如果不存在则重新创建 */
    if (node) {
        node->ptr = data;
        node->type = HACK_HASH_DATA_ADDR;
        return 0;
    }

    node = malloc(sizeof(struct hack_hash_node));
    if (!node) {
        return -1;
    }
    memset(node, 0, sizeof(struct hack_hash_node));

    /* 为 key 分配内存 */
    klen = strlen(key) + 1;
    kbuf = malloc(klen);
    if (!kbuf) {
        free(node);
        return -1;
    }
    memcpy(kbuf, key, klen);

    node->key = kbuf;
    node->ptr = data;
    node->type = HACK_HASH_DATA_ADDR;

    /* 将新的节点接到链表末尾 */
    hack_link_push((struct hack_link *)link, (struct hack_link_node *)node);

    map->node_count++;
    map->max_link = link->count > map->max_link ? link->count : map->max_link;
    map->alloc_size += (int)sizeof(struct hack_hash_node);
    map->alloc_size += klen;

    return 0;
}

int hack_hash_map_insert_int(HACK_HASH_MAP *map, const char *key, int data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;
    char *kbuf = NULL;
    size_t klen = 0;

    if (!map || !key) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    /* 哈希节点存在则直接替换数据地址，如果不存在则重新创建 */
    if (node) {
        node->i_data = data;
        node->type = HACK_HASH_DATA_INT;
        return 0;
    }

    node = malloc(sizeof(struct hack_hash_node));
    if (!node) {
        return -1;
    }
    memset(node, 0, sizeof(struct hack_hash_node));

    /* 为 key 分配内存 */
    klen = strlen(key) + 1;
    kbuf = malloc(klen);
    if (!kbuf) {
        free(node);
        return -1;
    }
    memcpy(kbuf, key, klen);

    node->key = kbuf;
    node->i_data = data;
    node->type = HACK_HASH_DATA_INT;

    /* 将新的节点接到链表末尾 */
    hack_link_push((struct hack_link *)link, (struct hack_link_node *)node);

    map->node_count++;
    map->max_link = link->count > map->max_link ? link->count : map->max_link;
    map->alloc_size += (int)sizeof(struct hack_hash_node);
    map->alloc_size += klen;

    return 0;
}

int hack_hash_map_remove_ptr(HACK_HASH_MAP *map, const char *key, void **ptr_data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;

    if (!map || !key || !*key) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    if (!node) {
        return 0;
    }

    if (ptr_data) {
        *ptr_data = node->ptr;
    }

    /* 将节点从链表上移除 */
    hack_link_remove_force((struct hack_link *)link, (struct hack_link_node *)node);

    map->node_count--;
    map->max_link = link->count > map->max_link ? link->count : map->max_link;
    map->alloc_size -= (int)sizeof(struct hack_hash_node);
    map->alloc_size -= (strlen(key) + 1);

    /* 释放内存 */
    free(node->key);
    free(node);

    return 0;
}

int hack_hash_map_remove_int(HACK_HASH_MAP *map, const char *key, int *i_data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;

    if (!map || !key || !*key) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    if (!node) {
        return 0;
    }

    if (i_data) {
        *i_data = node->i_data;
    }

    /* 将节点从链表上移除 */
    hack_link_remove_force((struct hack_link *)link, (struct hack_link_node *)node);

    map->node_count--;
    map->max_link = link->count > map->max_link ? link->count : map->max_link;
    map->alloc_size -= (int)sizeof(struct hack_hash_node);
    map->alloc_size -= (strlen(key) + 1);

    /* 释放内存 */
    free(node->key);
    free(node);

    return 0;
}

int hack_hash_map_find_ptr(HACK_HASH_MAP *map, const char *key, void **ptr_data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;

    if (!map || !key || !*key) {
        return -1;
    }

    if (!ptr_data) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    if (!node) {
        return -1;
    }

    *ptr_data = node->ptr;
    return 0;
}

int hack_hash_map_find_int(HACK_HASH_MAP *map, const char *key, int *i_data)
{
    struct hack_hash_link *link = NULL;
    struct hack_hash_node *node = NULL;
    int hash = 0;

    if (!map || !key || !*key) {
        return -1;
    }

    if (!i_data) {
        return -1;
    }

    if (!map->hash_link_list || !map->list_len) {
        return -1;
    }

    /* 获取表索引 */
    hash  = get_string_hash(key);
    hash %= map->list_len;

    link = map->hash_link_list + hash;

    if (get_hash_node(link, key, &node) != 0) {
        return -1;
    }

    if (!node) {
        return -1;
    }

    *i_data = node->i_data;
    return 0;
}

HACK_MAP_ITERATOR *hack_hash_map_iterator(HACK_HASH_MAP *map)
{
    if (map) {
        return map->iterator;
    }
    return NULL;
}

HACK_MAP_ITERATOR *hack_hash_map_first(HACK_MAP_ITERATOR *it)
{
    struct hack_hash_link *link = NULL;
    HACK_HASH_MAP *map = NULL;

    int i = 0;
    int len = 0;

    if (!it) {
        return NULL;
    }

    map = it->map;
    if (!map || map->iterator != it) {
        return NULL;
    }

    /* 获取链表数组 */
    link = map->hash_link_list;
    if (!link) {
        return NULL;
    }

    /* 查找第一个有元素的节点 */
    for (len = map->list_len; i < len; i++) {
        if (link->count > 0) {
            break;
        }
        link++;
    }

    if (i == len) {
        return NULL;
    }

    it->cur = link->head;
    it->link_index = i;
    it->node_index = 0;

    return it;
}

HACK_MAP_ITERATOR *hack_hash_map_next(HACK_MAP_ITERATOR *it)
{
    struct hack_hash_link *link = NULL;
    HACK_HASH_MAP  *map  = NULL;
    struct hack_hash_node *node = NULL;

    int i = 0;
    int j = 0;
    int len = 0;

    if (!it) {
        return NULL;
    }

    map = it->map;
    if (!map || map->iterator != it) {
        return NULL;
    }

    /* 调用本方法之前必须先调用 *_first 给 cur 赋值，否则直接返回 NULL */
    node = it->cur;
    if (!node) {
        return NULL;
    }

    /* 获取链表数组 */
    link = map->hash_link_list;
    if (!link) {
        return NULL;
    }

    i = it->link_index;
    j = it->node_index;
    len = map->list_len;

    if (i >= len) {
        return NULL;
    }

    /* 沿着链表数组搜索到对应的链表 */
    link = link + i;

    /* 利用索引检测当前节点是否是链表最后一个节点 */
    if (j < (int)(link->count - 1)) {
        j++;
        node = node->next;

        it->cur = node;
        it->link_index = i;
        it->node_index = j;

        return it;
    }

    /* 如果当前的链表是 hash map 中最后一个表，则返回 NULL */
    if (i == (len - 1)) {
        return NULL;
    }

    /* 搜索下一个链表 */
    link = link + 1;

    while (++i < len) {
        if (link->count > 0) {
            break;
        }
        link++;
    }

    if (i >= len) {
        return NULL;
    }

    it->cur = link->head;
    it->link_index = i;
    it->node_index = 0;

    return it;
}

const char *hack_hash_get_key(HACK_MAP_ITERATOR *it)
{
    char *key = NULL;

    if (!it) {
        return NULL;
    }

    if (!it->cur) {
        return NULL;
    }

    key = it->cur->key;
    return key;
}

int hack_hash_get_ptr(HACK_MAP_ITERATOR *it, void **ptr_data)
{
    if (!ptr_data) {
        return -1;
    }

    if (!it) {
        return -1;
    }

    if (!it->cur) {
        return -1;
    }

    *ptr_data = it->cur->ptr;
    return 0;
}

int hack_hash_get_int(HACK_MAP_ITERATOR *it, int *i_data)
{
    if (!i_data) {
        return -1;
    }

    if (!it) {
        return -1;
    }

    if (!it->cur) {
        return -1;
    }

    *i_data = it->cur->i_data;
    return 0;
}

int hack_hash_set_ptr(HACK_MAP_ITERATOR *it, void *ptr_data)
{
    if (!it || !it->cur) {
        return -1;
    }

    it->cur->ptr = ptr_data;
    it->cur->type = HACK_HASH_DATA_ADDR;
    return 0;
}
int hack_hash_set_int(HACK_MAP_ITERATOR *it, int i_data)
{
    if (!it || !it->cur) {
        return -1;
    }

    it->cur->i_data = i_data;
    it->cur->type = HACK_HASH_DATA_INT;
    return 0;
}

/*===========================================================================*/

int get_string_hash(const char *key)
{
    /* 采用 BKDR Hash 算法 */
    int seed = 31;
    int hash = 0;

    if (!key || !*key) {
        return 0;
    }

    while (*key) {
        hash = hash * seed + *key;
        key++;
    }

    return (hash & INT_MAX);
}

int get_hash_node(
    struct hack_hash_link *link,
    const char *key,
    struct hack_hash_node **node)
{
    int i = 0;

    struct hack_hash_node *ret  = NULL;
    struct hack_hash_node *tmp  = NULL;

    if (!link)
    {
        return -1;
    }

    if (!key || !*key || !node)
    {
        return -1;
    }

    if (!link->count)
    {
        *node = NULL;
        return 0;
    }

    tmp = link->head;

    /* 检索具有相同键的链表节点 */
    while (i++ < link->count) {
        /* 比较字符串 */
        if (!strcmp(tmp->key, key)) {
            ret = tmp;
            break;
        }

        tmp = tmp->next;
    }

    *node = ret;
    return 0;
}

/*===========================================================================*/
