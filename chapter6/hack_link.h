#ifndef HACK_LINK_H
#define HACK_LINK_H

/*===========================================================================*/
/* 链表 */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 双向链表主体结构 */
struct hack_link {
    struct hack_link_node *head;
    struct hack_link_node *tail;

    int count;
};

/* 链表节点 */
struct hack_link_node {
    struct hack_link_node *prev;
    struct hack_link_node *next;
};

/* 链表还原 */
extern void hack_link_reset(struct hack_link *link);

/* 将节点接入尾部 */
extern int hack_link_push(struct hack_link *link, struct hack_link_node *node);

/* 按索引插入节点，如 0 则插在第一个，依次类推，查询节点复杂度为 O(n) */
extern int hack_link_insert(
    struct hack_link *link,
    int index,
    struct hack_link_node *node
);

/* 在目标节点之前插入节点, target 不能为空 */
extern int hack_link_insert_before(
    struct hack_link *link,
    struct hack_link_node *target,
    struct hack_link_node *node
);

/* 在目标节点之后插入节点, target 不能为空 */
extern int hack_link_insert_after(
    struct hack_link *link,
    struct hack_link_node *target,
    struct hack_link_node *node
);

/* 移除尾部节点，同时返回节点地址 */
extern struct hack_link_node *hack_link_pop(struct hack_link *link);

/* 根据索引移除节点，查询节点复杂度为 O(n) */
extern struct hack_link_node *hack_link_remove(struct hack_link *link, int index);

/* 移除目标节点, 移除成功返回该节点, 移除时会先查询节点是否存在 */
extern struct hack_link_node *hack_link_remove_node(
    struct hack_link *link,
    struct hack_link_node *target
);

/* 强制移除目标节点，算法复杂度 O(1) */
extern struct hack_link_node *hack_link_remove_force(
    struct hack_link *link,
    struct hack_link_node *target
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*===========================================================================*/

#endif /* HACK_LINK_H */
