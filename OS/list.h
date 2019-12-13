#ifndef LIST_H
#define LIST_H


#include <stdint.h>

struct ListItem
{
	uint32_t ItemValue;
  struct ListItem * pNext;       /**< Pointer to the next list item */
  struct ListItem * pPrev;       /**< Pointer to the previous list item */
  void * pTask;                  /**< Pointer to the Task of the item */
  void * pList;                  /**< Pointer to the list in which the item it */
};
typedef struct ListItem ListItem_t;

typedef struct
{
	uint32_t ItemValue;
  ListItem_t * pNext;     /**< Pointer to the next list item */
  ListItem_t * pPrev;     /**< Pointer to the previous list item */
} ListEnd_t;

typedef struct
{
  uint32_t numOfItems;           /**< Number of threads items in the list */
  ListItem_t * pIndex;           /**< Pointer to the current item */
  ListEnd_t listEnd;             /**< List end */
} List_t;

void List_Init(List_t * pList);
void List_Insert(List_t * pList, ListItem_t * pNewItem);
void List_Remove(ListItem_t * pItemToRemove);


void ListInsertAtItemValue(List_t * pList, ListItem_t * pNewItem);

#endif
