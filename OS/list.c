#include "list.h"
#include "task.h"
#include "M4memMap.h"

void List_Init(List_t * const pList)
{
    pList->numOfItems = 0;
    pList->pIndex = (ListItem_t*) &(pList->listEnd);
    pList->listEnd.pNext = (ListItem_t*) &(pList->listEnd);
    pList->listEnd.pPrev = (ListItem_t*) &(pList->listEnd);
	pList->listEnd.ItemValue = 0xFFFFFFFFu;
}

void List_Insert(List_t * List,ListItem_t * pNewItem)
{
    pNewItem->pNext = List->pIndex->pNext ;
    pNewItem->pPrev = List->pIndex ;

    List->pIndex->pNext->pPrev = pNewItem;
    List->pIndex->pNext = pNewItem ;

    List->pIndex = pNewItem;

    pNewItem->pList = (void *) List ;

    (List->numOfItems)++ ;
}


void ListInsertAtItemValue(List_t * pList, ListItem_t * pNewItem)
{
  /* Temp for the insert index */
  ListItem_t * pInsertIndex = (ListItem_t *) &pList->listEnd;

  /* Get insert index, find the high item value */
  while((pInsertIndex->pNext != (ListItem_t *) &pList->listEnd)&& (pInsertIndex->pNext->ItemValue <= pNewItem->ItemValue))
  {
    pInsertIndex = pInsertIndex->pNext;
  }
  pNewItem->pNext = pInsertIndex->pNext;
  pNewItem->pPrev = pInsertIndex;
  pInsertIndex->pNext->pPrev = pNewItem;
  pInsertIndex->pNext = pNewItem;
  pNewItem->pList = (void *) pList;
  pList->numOfItems++;
}



void List_Remove(ListItem_t * pItemToRemove)
{
    List_t * pList = (List_t *)pItemToRemove->pList;
    pItemToRemove->pNext->pPrev = pItemToRemove->pPrev;
    pItemToRemove->pPrev->pNext = pItemToRemove->pNext;

    if(pList->pIndex == pItemToRemove)
    {
        pList->pIndex = pItemToRemove->pPrev;
    }
    else
    {
    }
    pItemToRemove->pList = NULL;
    (pList->numOfItems)--;
}
