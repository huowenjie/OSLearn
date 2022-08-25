#ifndef HACK_HASH_H
#define HACK_HASH_H

/*===========================================================================*/
/* 字符串哈希表 */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct hack_hash_map HACK_HASH_MAP;
typedef struct hack_map_iterator HACK_MAP_ITERATOR;

typedef void (*hack_hash_visit_item)(const char *key, void *ptr);

/* 创建哈希表, 指定初始长度 */
extern HACK_HASH_MAP *hack_hash_map_new(int len);

/* 释放哈希表 */
extern void hack_hash_map_del(HACK_HASH_MAP *map);

/* 清空哈希表 */
extern void hack_hash_map_clear(HACK_HASH_MAP *map);

/* 清空哈希表并访问指针元素 */
extern void hack_hash_map_clear_item(
    HACK_HASH_MAP *map,
    hack_hash_visit_item visit_item
);

/* 获取元素数量 */
extern int hack_hash_map_node_count(HACK_HASH_MAP *map);

/* 插入数据 */
extern int hack_hash_map_insert_ptr(HACK_HASH_MAP *map, const char *key, void *data);
extern int hack_hash_map_insert_int(HACK_HASH_MAP *map, const char *key, int data);

/* 移除元素，同时返回移除的元素 */
extern int hack_hash_map_remove_ptr(HACK_HASH_MAP *map, const char *key, void **ptr_data);
extern int hack_hash_map_remove_int(HACK_HASH_MAP *map, const char *key, int *i_data);

/* 检索元素 */
extern int hack_hash_map_find_ptr(HACK_HASH_MAP *map, const char *key, void **ptr_data);
extern int hack_hash_map_find_int(HACK_HASH_MAP *map, const char *key, int *i_data);

/* 获取迭代器 */
extern HACK_MAP_ITERATOR *hack_hash_map_iterator(HACK_HASH_MAP *map);

/* 将迭代器指针定位到首个元素上 */
extern HACK_MAP_ITERATOR *hack_hash_map_first(HACK_MAP_ITERATOR *it);

/* 访问下一个元素，如果返回值为 NULL，则说明没有元素 */
extern HACK_MAP_ITERATOR *hack_hash_map_next(HACK_MAP_ITERATOR *it);

/* 获取键，len 为字符串长度，buff 为 NULL 时，len 返回键的长度（包含末尾的 '\0' 字符） */
extern const char *hack_hash_get_key(HACK_MAP_ITERATOR *it);

/* 获取数据（直接赋值） */
extern int hack_hash_get_ptr(HACK_MAP_ITERATOR *it, void **ptr_data);
extern int hack_hash_get_int(HACK_MAP_ITERATOR *it, int *i_data);

/* 设置数据，我们会重新覆盖原有的数据 */
extern int hack_hash_set_ptr(HACK_MAP_ITERATOR *it, void *ptr_data);
extern int hack_hash_set_int(HACK_MAP_ITERATOR *it, int i_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*===========================================================================*/

#endif /* HACK_HASH_H */
