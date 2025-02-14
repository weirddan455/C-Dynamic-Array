#include "List.h"
#include "charArray.h"

// ---------------------------------------
// DEBUG MANAGEMENT
// ---------------------------------------

void print_list_info(List *list, char *additionalInfo)
{
    CharArray strArr = create_CA(""); // utilize char array

    char buffor[255];
    for (size_t i=0; i<list->size; i++)
    {
        sprintf(buffor,"%Iu:",i);
        append_CA(&strArr, buffor);
        append_CA(&strArr, list->elementPointers[i]->ID);
        append_CA(&strArr, ", ");
        DEBUG("i: %Iu, ID: %s", i, list->elementPointers[i]->ID);
    }
    pop_CA_back(&strArr); // pop space
    pop_CA_back(&strArr); // pop comma

    DEBUG("loop done");
    printf("---------------------------------\n");
    printf("LIST INFO:\n");
    printf("size: %Iu, maxSize: %Iu\n", list->size, list->maxSize);
    printf("Content: %s\n", strArr.arrayPointer);
    if (strlen(additionalInfo) != 0) { printf("Additional info: "); printf("%s",additionalInfo); }
    printf("---------------------------------\n");

    destroy_CA(&strArr);
}


void print_listElement_info(List *list, size_t index, char *additionalInfo)
{
    ListElement *ePtr = list_get_element_ptr(list, index);

    printf("---------------------------------\n");
    printf("LIST ELEMENT INFO:\n");
    printf("dataTypeSize: %Iu\n", ePtr->dataTypeSize);
    printf("Content Type: %s\n", ePtr->ID);
    if (strlen(additionalInfo) != 0) { printf("Additional info: "); printf("%s",additionalInfo); }
    printf("---------------------------------\n");
}

// ---------------------------------------
// SIZE MANAGEMENT
// ---------------------------------------

ARR_ERR_CODE increase_list_size(List* list, size_t addSize)
{
    if (size_t_will_overflow_add(list->maxSize, addSize)) { DEBUG("Size overflow."); return ARR_ERR_OVER;}
    size_t increasedSize = list->maxSize + addSize;

    void *tmp = realloc(list->elementPointers, increasedSize);
    if (tmp == NULL) { ERROR("Unable to realloc."); return ARR_ERR_REALLOC; }
    list->elementPointers = tmp;

    list->maxSize += addSize;

    return ARR_ERR_OK;
}


ARR_ERR_CODE decrease_list_size(List* list, size_t minusSize)
{
    // If decrease more than list size just make list size 0
    if( size_t_will_overflow_minus(list->maxSize, minusSize) ) { minusSize = list->maxSize; } // Overflow check
    size_t decreasedSize = list->maxSize - minusSize;

    // Destroy elements
    if (decreasedSize < list->size)
    {
        for (size_t i = decreasedSize; i<list->size; i++)
        {
            //DEBUG("Destroying list element on index: %d",i);
            ListElement *elementPtr = list_get_element_ptr(list, i);
            destory_list_element(elementPtr);
        }
    }

    void *tmp = realloc(list->elementPointers, decreasedSize);
    if (tmp == NULL) { ERROR("Unable to realloc."); return ARR_ERR_REALLOC; }
    list->elementPointers = tmp;

    list->maxSize -= minusSize;
    if (list->size > list->maxSize){ list->size = list->maxSize; }

    return ARR_ERR_OK;
}


ARR_ERR_CODE resize_list(List* list, size_t destSize)
{
    ARR_ERR_CODE err_result = ARR_ERR_OK;

    if (destSize > list->maxSize)
    {
        size_t addSize = destSize - list->maxSize;
        err_result = increase_list_size(list, addSize);
    }
    else if (destSize < list->maxSize)
    {
        size_t minusSize = list->maxSize - destSize;
        err_result = decrease_list_size(list, minusSize);
    }

    return err_result;
}

// ---------------------------------------
// CHANGE LIST MANAGEMENT
// ---------------------------------------

ListElement* create_Element(void* data, size_t dataSize, char* ID)
{
    // Create element
    ListElement element;
    element.dataTypeSize = dataSize;

    element.ID = malloc(strlen(ID));                 // Allocate space for char array
    memcpy(element.ID, ID, strlen(ID));              // Copy char array

    element.objectPointer = malloc(dataSize);        // Allocate space for data
    memcpy(element.objectPointer, data, dataSize);   // Copy data

    // Allocate space for the element
    ListElement *Eptr = malloc(sizeof *Eptr);
    if (Eptr == NULL) { return Eptr; }

    // Copy element to allocated space
    memcpy(Eptr, &element, sizeof *Eptr);

    return Eptr;
}

ARR_ERR_CODE list_append(List* list, void* data, size_t dataSize, char* ID)
{
    ListElement *Eptr = create_Element(data, dataSize, ID);
    if (Eptr == NULL) { ERROR("Unable to malloc."); return ARR_ERR_MALLOC; }

    size_t lenCombined = list->size + 1;
    if (lenCombined > list->maxSize) { increase_list_size(list, 1); }

    list->size++;
    list->elementPointers[list->size-1] = Eptr;

    return ARR_ERR_OK;
}

// ---------------------------------------
// GET FROM LIST
// ---------------------------------------


ListElement* list_get_element_ptr(List* list, size_t index)
{
    if (index >= list->maxSize)
    {
        ERROR("Tried to get index: %Iu, when max index is %Iu in List.", index, list->maxSize-1);
        return NULL;
    }

    return list->elementPointers[index];
}

// ---------------------------------------
// CONSTRUCTOR
// ---------------------------------------

List create_list()
{
    List list;
    list.size = 0;
    list.maxSize = 0;
    list.elementPointers = malloc(0);
    if (list.elementPointers == NULL) {ERROR("Failed to malloc."); exit(1); }

    return list;
}

// ---------------------------------------
// DESTRUCTOR
// ---------------------------------------

void destory_list_element(ListElement *element)
{
    free(element->objectPointer);
    free(element->ID);
    free(element);
}
