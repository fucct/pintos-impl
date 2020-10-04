#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "hash.h"
#include "bitmap.h"
#include "debug.h"
#include "hex_dump.h"
#include "round.h"
#include "operation.h"

struct data_name_map {
    char name[100];
    struct list *list;
    struct hash *hash;
    struct bitmap *bitmap;
};

int getOperation(char *line);

struct list *getListByName(const struct data_name_map *data_map, const char *data_name);

struct bitmap *getBitmapByName(const struct data_name_map *name_table, const char *data_name);

struct hash *getHashByName(const struct data_name_map *name_table, const char *data_name);

int getDataStructure(const char *line);

void printConsole(char *commandLine);

enum list_operation getListOperation(const char *line);

enum data_structure getDataType(const struct data_name_map *name_table, const char *data_name);

void createList(struct data_name_map name_table[10], const char *data_name, int *data_cnt);

void createHash(struct data_name_map *name_table, const char *data_name, int *data_cnt);

void createBitmap(struct data_name_map *name_table, const char *data_name, int bitmap_size);

void dumpData(const struct data_name_map *data_name_table, const char *data_name);

void printList(struct list *picked_list);

void printBitmap(struct bitmap *pBitmap);

void printHash(struct hash *pHash);

void listPushFront(struct list *picked_list, const char *data);

void listPushBack(struct list *picked_list, const char *data);

void processListOperation(const struct data_name_map *data_name_table, const char *operation, char *args1, char *args2,
                          char *args3);

bool list_less(struct list_elem *elem1, struct list_elem *elem2, void *aux);

void init_table(struct data_name_map *data_name_table);

void create_data(int *data_cnt, struct data_name_map *data_name_table, const char *type_name, const char *data_name,
                 const char *args3);

void delete_data(int *data_cnt, struct data_name_map *data_name_table, const char *args1);

void processBitmapOperation(struct data_name_map data_name_table[10], char *operation, char *args1, char *args2,
                            char *args3);

enum bitmap_operation getBitmapOperation(char *operation);

enum hash_operation getHashOperation(char *operation);

bool getBoolean(char *args);

void printBool(bool result);

void getHashActionFunc(char *func_name);

void square(struct hash_elem *elem, void *aux);

void triple(struct hash_elem *elem, void *aux);

void minus10(struct hash_elem *elem, void *aux);


void
processHashOperation(struct data_name_map data_name_table[10], char *operation, char *args1, char *args2, char *args3);

unsigned hash_func(const struct hash_elem *e, void *aux);

bool less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux);

void delete_elem_func(struct hash_elem *e, void *aux);

int main() {
    int data_cnt = 0;
    char commandLine[100];
    struct data_name_map data_name_table[10];

    init_table(data_name_table);

    printConsole(commandLine);
    while (true) {
        char *operation = strtok(commandLine, " ");
        char *args1 = strtok(NULL, " ");
        char *args2 = strtok(NULL, " ");
        char *args3 = strtok(NULL, " ");
        if (operation == NULL) {
            printConsole(commandLine);
            continue;
        }
        switch (getOperation(operation)) {
            case CREATE:
                if (data_cnt == 10) {
                    fprintf(stderr,
                            "Data count cannot exceed 10! If you want to create more data, delete existing data.");
                    break;
                }

                if (args1 == NULL || args2 == NULL) {
                    fprintf(stderr, "You should enter 2 arguments. #1 : data type, #2 : data name.\n");
                    break;
                }

                bool flag = false;

                for (int i = 0; i < 10; i++) {
                    if (strcmp(data_name_table[i].name, args2) == 0) {
                        fprintf(stderr, "Already exists data name : %s.\n", args2);
                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    break;
                }
                create_data(&data_cnt, data_name_table, args1, args2, args3);
                break;
            case DELETE:
                delete_data(&data_cnt, data_name_table, args1);
                break;
            case DUMP_DATA:
                dumpData(data_name_table, args1);
                break;
            case LIST_OPERATION:
                processListOperation(data_name_table, operation, args1, args2, args3);
                break;
            case HASH_OPERATION:
                processHashOperation(data_name_table, operation, args1, args2, args3);
                break;
            case BITMAP_OPERATION:
                processBitmapOperation(data_name_table, operation, args1, args2, args3);
                break;
            case DEFAULT:
                printf("Unsupported Method : %s. Please Re-Enter Command Line\n", operation);
                break;
            case QUIT:
                return 0;
        }
        printConsole(commandLine);
    }
}

void delete_data(int *data_cnt, struct data_name_map *data_name_table, const char *args1) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(data_name_table[i].name, args1) == 0) {
            if (data_name_table[i].list != NULL) {
                free(data_name_table[i].list);
            } else if (data_name_table[i].bitmap != NULL) {
                free(data_name_table[i].bitmap);
            } else if (data_name_table[i].hash != NULL) {
                free(data_name_table[i].hash);
            }
            data_name_table[i].name[0] = '\0';
            break;
        }
    }
    (*data_cnt)--;
}

void create_data(int *data_cnt, struct data_name_map *data_name_table, const char *type_name, const char *data_name,
                 const char *args3) {
    switch (getDataStructure(type_name)) {
        case LIST:
            createList(data_name_table, data_name, data_cnt);
            break;
        case HASH:
            createHash(data_name_table, data_name, data_cnt);
            break;
        case BITMAP: {
            int bitmap_size = (int) strtol(args3, NULL, 10);

            createBitmap(data_name_table, data_name, bitmap_size);
            break;
        }
        case NOTHING:
            fprintf(stderr, "There is no type : %s\n", type_name);
            break;
    }
    (*data_cnt)++;
}

void init_table(struct data_name_map *data_name_table) {
    for (int i = 0; i < 10; i++) {
        data_name_table[i].name[0] = '\0';
        data_name_table[i].list = NULL;
        data_name_table[i].bitmap = NULL;
        data_name_table[i].hash = NULL;
    }
}

void processListOperation(const struct data_name_map *data_name_table, const char *operation, char *args1, char *args2,
                          char *args3) {
    enum data_structure dataType = getDataType(data_name_table, args1);
    if (dataType != LIST) {
        fprintf(stderr, "%s is not a List. Please try again.\n", args1);
        return;
    }
    struct list *pickedList = getListByName(data_name_table, args1);

    switch (getListOperation(operation)) {
        case LIST_PUSH_FRONT:
            listPushFront(pickedList, args2);
            break;
        case LIST_PUSH_BACK:
            listPushBack(pickedList, args2);
            break;
        case LIST_POP_FRONT:
            list_pop_front(pickedList);
            break;
        case LIST_POP_BACK:
            list_pop_back(pickedList);
            break;
        case LIST_FRONT: {
            struct list_elem *pElem = list_front(pickedList);
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);
            printf("%d\n", pItem->data);
            break;
        }
        case LIST_BACK: {
            struct list_elem *pElem = list_back(pickedList);
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);
            printf("%d\n", pItem->data);
            break;
        }
        case LIST_INSERT: {
            struct list_elem *before = list_begin(pickedList);
            struct list_elem *pElem = (struct list_elem *) malloc(sizeof(struct list_item));
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);
            pItem->data = (int) strtol(args3, NULL, 10);

            int index = (int) strtol(args2, NULL, 10);
            if (index == 0) {
                list_push_front(pickedList, pElem);
            } else {
                for (int i = 0; i < index; i++) {
                    before = list_next(before);
                }
                list_insert(before, pElem);
            }
            break;
        }
        case LIST_INSERT_ORDERED: {
            struct list_elem *pElem = (struct list_elem *) malloc(sizeof(struct list_item));
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);
            pItem->data = (int) strtol(args2, NULL, 10);
            bool (*less_func)() = list_less;
            list_insert_ordered(pickedList, pElem, less_func, NULL);
            break;
        }
        case LIST_EMPTY: {
            printf(list_empty(pickedList) ? "true\n" : "false\n");
            break;
        }
        case LIST_SIZE: {
            printf("%zu\n", list_size(pickedList));
            break;
        }
        case LIST_MAX: {
            bool (*less_func)() = list_less;
            struct list_elem *pElem = list_max(pickedList, less_func, NULL);
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);

            printf("%d\n", pItem->data);
            break;
        }
        case LIST_MIN: {
            bool (*less_func)() = list_less;
            struct list_elem *pElem = list_min(pickedList, less_func, NULL);
            struct list_item *pItem = list_entry(pElem, struct list_item, elem);

            printf("%d\n", pItem->data);
            break;
        }
        case LIST_REMOVE: {
            int remove_index = (int) strtol(args2, NULL, 10);
            if (remove_index >= list_size(pickedList)) {
                fprintf(stderr, "Remove index must smaller than last index.\n");
                break;
            }
            struct list_elem *iter = list_begin(pickedList);

            if (remove_index == 0) {
                list_remove(iter);
            } else {
                for (int i = 0; i < remove_index; i++) {
                    iter = list_next(iter);
                }
                list_remove(iter);
            }
            break;
        }
        case LIST_REVERSE:
            list_reverse(pickedList);
            break;
        case LIST_SHUFFLE:
            list_shuffle(pickedList);
            break;
        case LIST_SWAP: {
            int a = (int) strtol(args2, NULL, 10);
            int b = (int) strtol(args3, NULL, 10);
            struct list_elem *pA = list_get(pickedList, a);
            struct list_elem *pB = list_get(pickedList, b);
            list_swap(pA, pB);
            break;
        }
        case LIST_SORT: {
            bool (*less_func)() = list_less;
            list_sort(pickedList, less_func, NULL);
            break;
        }
        case LIST_UNIQUE: {
            bool (*less_func)() = list_less;
            if (args2 == NULL) {
                list_unique(pickedList, NULL, less_func, NULL);
            } else {
                struct list *second_list = getListByName(data_name_table, args2);
                list_unique(pickedList, second_list, less_func, NULL);
            }
            break;
        }
        case LIST_SPLICE: {
            char *args4 = strtok(NULL, " ");
            char *args5 = strtok(NULL, " ");
            int before_idx = (int) strtol(args2, NULL, 10);
            int start_add_idx = (int) strtol(args4, NULL, 10);
            int end_add_idx = (int) strtol(args5, NULL, 10);
            struct list *second_list = getListByName(data_name_table, args3);

            struct list_elem *pBefore = list_get(pickedList, before_idx);
            struct list_elem *pStart = list_get(second_list, start_add_idx);
            struct list_elem *pEnd = list_get(second_list, end_add_idx);
            list_splice(pBefore, pStart, pEnd);
            break;
        }
        case LIST_NOTHING: {
            fprintf(stderr, "Unsupported List operation. pleas try again\n");
            break;
        }

        default:
            fprintf(stderr, "Unsupported list operation : %s\n", operation);
            break;
    }
}

void processBitmapOperation(struct data_name_map data_name_table[10], char *operation, char *args1, char *args2,
                            char *args3) {
    enum data_structure dataType = getDataType(data_name_table, args1);
    if (dataType != BITMAP) {
        fprintf(stderr, "%s is not a Bitmap. Please try again.\n", args1);
        return;
    }
    struct bitmap *pickedBitmap = getBitmapByName(data_name_table, args1);

    switch (getBitmapOperation(operation)) {
        case BITMAP_MARK: {
            size_t idx = (size_t) strtol(args2, NULL, 10);
            bitmap_mark(pickedBitmap, idx);
            break;
        }
        case BITMAP_ALL: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t end = (size_t) strtol(args3, NULL, 10);
            printBool(bitmap_all(pickedBitmap, start, end));
            break;
        }
        case BITMAP_ANY: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t end = (size_t) strtol(args3, NULL, 10);
            printBool(bitmap_any(pickedBitmap, start, end));
            break;
        }
        case BITMAP_CONTAINS: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            bool tf = getBoolean(strtok(NULL, " "));
            printBool(bitmap_contains(pickedBitmap, start, cnt, tf));
            break;
        }
        case BITMAP_COUNT: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            bool tf = getBoolean(strtok(NULL, " "));
            printf("%zu\n", bitmap_count(pickedBitmap, start, cnt, tf));
            break;
        }
        case BITMAP_DUMP: {
            bitmap_dump(pickedBitmap);
            break;
        }
        case BITMAP_EXPAND: {
            size_t size = (size_t) strtol(args2, NULL, 10);
            struct bitmap *pBitmap = bitmap_expand(pickedBitmap, size);
            for (int i = 0; i < 10; i++) {
                if (strcmp(data_name_table[i].name, args1) == 0) {
                    data_name_table->bitmap = pBitmap;
                    break;
                }
            }
            break;
        }
        case BITMAP_FLIP: {
            size_t idx = (size_t) strtol(args2, NULL, 10);
            bitmap_flip(pickedBitmap, idx);
            break;
        }
        case BITMAP_NONE: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            printf(bitmap_none(pickedBitmap, start, cnt) ? "true\n" : "false\n");
        }
        case BITMAP_RESET: {
            size_t idx = (size_t) strtol(args2, NULL, 10);
            bitmap_reset(pickedBitmap, idx);
            break;
        }
        case BITMAP_SCAN: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            bool tf = getBoolean(strtok(NULL, " "));
            printf("%zu\n", bitmap_scan(pickedBitmap, start, cnt, tf));
            break;
        }
        case BITMAP_SCAN_AND_FLIP: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            bool tf = getBoolean(strtok(NULL, " "));
            printf("%zu\n", bitmap_scan_and_flip(pickedBitmap, start, cnt, tf));
            break;
        }
        case BITMAP_SET: {
            size_t idx = (size_t) strtol(args2, NULL, 10);
            bool tf = getBoolean(args3);
            bitmap_set(pickedBitmap, idx, tf);
            break;
        }
        case BITMAP_SET_ALL: {
            bool tf = getBoolean(args2);
            bitmap_set_all(pickedBitmap, tf);
            break;
        }
        case BITMAP_SET_MULTIPLE: {
            size_t start = (size_t) strtol(args2, NULL, 10);
            size_t cnt = (size_t) strtol(args3, NULL, 10);
            bool tf = getBoolean(strtok(NULL, " "));
            bitmap_set_multiple(pickedBitmap, start, cnt, tf);
            break;
        }
        case BITMAP_SIZE: {
            printf("%zu\n", bitmap_size(pickedBitmap));
            break;
        }
        case BITMAP_TEST: {
            size_t idx = (size_t) strtol(args2, NULL, 10);
            printBool(bitmap_test(pickedBitmap, idx));
            break;
        }
        case BITMAP_NOTHING: {
            fprintf(stderr, "Unsupported Bitmap operation. pleas try again\n");
            break;
        }
        default:
            fprintf(stderr, "Unexpected Error. please try again\n");
            break;
    }
}

void
processHashOperation(struct data_name_map data_name_table[10], char *operation, char *args1, char *args2, char *args3) {
    enum data_structure dataType = getDataType(data_name_table, args1);
    if (dataType != HASH) {
        fprintf(stderr, "%s is not a HashTable. Please try again.\n", args1);
        return;
    }
    struct hash *pickedHash = getHashByName(data_name_table, args1);

    switch (getHashOperation(operation)) {
        case HASH_INSERT: {
            int data = (int) strtol(args2, NULL, 10);
            struct hash_elem *pElem = (struct hash_elem *) malloc(sizeof(struct hash_item));
            struct hash_item *pItem = hash_entry(pElem, struct hash_item, elem);
            pItem->data = data;
            hash_insert(pickedHash, pElem);
            break;
        }
        case HASH_SIZE: {
            printf("%zu\n", hash_size(pickedHash));
            break;
        }
        case HASH_EMPTY: {
            printBool(hash_empty(pickedHash));
            break;
        }
        case HASH_CLEAR: {
            hash_clear(pickedHash, &delete_elem_func);
            break;
        }
        case HASH_FIND: {
            int data = (int) strtol(args2, NULL, 10);
            size_t size = pickedHash->bucket_cnt;
            bool flag = false;
            for (int i = 0; i < size; i++) {
                struct list_elem *elem = list_begin(&pickedHash->buckets[i]);
                while (elem != NULL) {
                    if (elem->next == NULL) {
                        break;
                    }
                    struct list_item *pItem = list_entry(elem, struct list_item, elem);
                    if (pItem->data == data) {
                        flag = true;
                        break;
                    }
                    elem = elem->next;
                }
                if (flag) {
                    break;
                }
            }
            if (flag) {
                printf("%d\n", data);
            }
            break;
        }
        case HASH_REPLACE: {
            int data = (int) strtol(args2, NULL, 10);
            struct hash_elem *pElem = (struct hash_elem *) malloc(sizeof(struct hash_item));
            struct hash_item *pItem = hash_entry(pElem, struct hash_item, elem);
            pItem->data = data;
            hash_replace(pickedHash, pElem);
            break;
        }
        case HASH_DELETE: {
            int data = (int) strtol(args2, NULL, 10);
            size_t size = pickedHash->bucket_cnt;
            for (int i = 0; i < size; i++) {
                bool flag = false;
                struct list_elem *elem = list_begin(&pickedHash->buckets[i]);
                while (elem != NULL) {
                    struct list_item *pItem = list_entry(elem, struct list_item, elem);
                    if (pItem->data == data) {
                        hash_delete(pickedHash, list_elem_to_hash_elem(elem));
                        flag = true;
                        break;
                    }
                    elem = elem->next;
                }
                if (flag) {
                    break;
                }
            }
            break;
        }
        case HASH_APPLY: {
            if (strcmp(args2, "square") == 0) {
                hash_apply(pickedHash, &square);
            } else if (strcmp(args2, "triple") == 0) {
                hash_apply(pickedHash, &triple);
            } else if (strcmp(args2, "-10") == 0) {
                hash_apply(pickedHash, &minus10);
            } else {
                fprintf(stderr, "Unknown function. Please try again");
                break;
            }
            break;
        }
        case HASH_NOTHING: {
            fprintf(stderr, "Unsupported Hash Table operation. pleas try again\n");
            break;
        }
        default:
            fprintf(stderr, "Unexpected Error. please try again\n");
            break;
    }
}


void square(struct hash_elem *elem, void *aux) {
    struct hash_item *pItem = hash_entry(elem, struct hash_item, elem);
    int data = pItem->data;
    pItem->data = data * data;
}

void triple(struct hash_elem *elem, void *aux) {
    struct hash_item *pItem = hash_entry(elem, struct hash_item, elem);
    int data = pItem->data;
    pItem->data = data * data * data;
}

void minus10(struct hash_elem *elem, void *aux) {
    struct hash_item *pItem = hash_entry(elem, struct hash_item, elem);
    int data = pItem->data;
    pItem->data = data - 10;
}

void printBool(bool result) { printf(result ? "true\n" : "false\n"); }

bool getBoolean(char *args) {
    bool tf;
    if (strcmp(args, "true") == 0) {
        tf = true;
    } else {
        tf = false;
    }
    return tf;
}

void listPushBack(struct list *picked_list, const char *args2) {
    struct list_elem *new_elem = (struct list_elem *) malloc(sizeof(struct list_item));
    struct list_item *pItem = list_entry(new_elem, struct list_item, elem);
    pItem->data = (int) strtol(args2, NULL, 10);
    list_push_back(picked_list, new_elem);
}

void listPushFront(struct list *picked_list, const char *data) {
    struct list_elem *new_elem = (struct list_elem *) malloc(sizeof(struct list_item));
    struct list_item *pItem = list_entry(new_elem, struct list_item, elem);
    pItem->data = (int) strtol(data, NULL, 10);
    list_push_front(picked_list, new_elem);
}

void dumpData(const struct data_name_map *data_name_table, const char *data_name) {
    enum data_structure data_type = getDataType(data_name_table, data_name);
    struct list *picked_list;
    struct bitmap *picked_bitmap;
    struct hash *picked_hash;
    switch (data_type) {
        case LIST:
            picked_list = getListByName(data_name_table, data_name);

            if (picked_list == NULL) {
                fprintf(stderr, "There is no list name : %s\n", data_name);
                break;
            }
            printList(picked_list);
            break;
        case BITMAP:
            picked_bitmap = getBitmapByName(data_name_table, data_name);

            if (picked_bitmap == NULL) {
                fprintf(stderr, "There is no bitmap name : %s\n", data_name);
                break;
            }
            printBitmap(picked_bitmap);
            break;
        case HASH:
            picked_hash = getHashByName(data_name_table, data_name);

            if (picked_hash == NULL) {
                fprintf(stderr, "There is no hash name : %s\n", data_name);
                break;
            }
            printHash(picked_hash);
            break;
        case NOTHING:
            fprintf(stderr, "There is no type name : %s\n", data_name);
            break;
        default:
            fprintf(stderr, "Unexpected Error : %s\n", data_name);
            break;
    }
}

void printHash(struct hash *pHash) {
    if (hash_empty(pHash)) {
        return;
    }
    for (int i = 0; i < pHash->bucket_cnt; i++) {
        struct list *list = &pHash->buckets[i];
        if (list_empty(list)) {
            continue;
        }
        struct list_elem *elem = list_begin(list);
        while (elem != NULL) {
            if (elem->next == NULL) {
                break;
            }
            struct list_item *pItem = list_entry(elem, struct list_item, elem);
            printf("%d ", pItem->data);
            elem = elem->next;
        }
    }
    printf("\n");
}

void printBitmap(struct bitmap *pBitmap) {
    int bit_cnt = (int) pBitmap->bit_cnt;

    for (int j = 0; j <= bit_cnt / ELEM_BITS; j++) {
        unsigned long value = (unsigned long) pBitmap->bits[j];
        for (int i = 0; j * 64 + i < bit_cnt && i < 64; i++) {
            printf("%lu", value % 2);
            value /= 2;
        }
    }

    printf("\n");
}

void printList(struct list *picked_list) {
    if (list_empty(picked_list)) {
        return;
    }
    struct list_elem *elem = list_begin(picked_list);
    while (elem != NULL) {
        if (elem->next == NULL) {
            break;
        }
        struct list_item *pItem = list_entry(elem, struct list_item, elem);
        printf("%d ", pItem->data);
        elem = elem->next;
    }
    printf("\n");
}

void createBitmap(struct data_name_map *name_table, const char *data_name, int bitmap_size) {
    struct bitmap *new_bitmap = bitmap_create(bitmap_size);
    for (int i = 0; i < 10; i++) {
        if (name_table[i].name[0] == '\0') {
            name_table[i].bitmap = new_bitmap;
            strcpy(name_table[i].name, data_name);
            break;
        }
    }
}

void createHash(struct data_name_map *name_table, const char *data_name, int *data_cnt) {
    struct hash *new_hash = (struct hash *) malloc(sizeof(struct hash));
    hash_hash_func *func1 = &hash_func;
    hash_less_func *func2 = &less_func;
    void *aux = malloc(sizeof(long));
    hash_init(new_hash, func1, func2, aux);
    for (int i = 0; i < 10; i++) {
        if (name_table[i].name[0] == '\0') {
            name_table[i].hash = new_hash;
            strcpy(name_table[i].name, data_name);
            break;
        }
    }
}

unsigned hash_func(const struct hash_elem *e, void *aux) {
    struct hash_item *pItem = hash_entry(e, struct hash_item, elem);
    return hash_int(pItem->data);
}

bool less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
    struct hash_item *pItemA = hash_entry(a, struct hash_item, elem);
    struct hash_item *pItemB = hash_entry(b, struct hash_item, elem);

    return pItemA->data < pItemB->data;
}

void delete_elem_func(struct hash_elem *e, void *aux) {
    struct hash_item *pItem = hash_entry(e, struct hash_item, elem);
    free(pItem);
}

void createList(struct data_name_map name_table[10], const char *data_name, int *data_cnt) {
    struct list *new_list = (struct list *) malloc(sizeof(struct list));
    list_init(new_list);
    for (int i = 0; i < 10; i++) {
        if (name_table[i].name[0] == '\0') {
            name_table[i].list = new_list;
            strcpy(name_table[i].name, data_name);
            break;
        }
    }
}

enum hash_operation getHashOperation(char *operation) {
    if (strcmp(operation, "hash_apply") == 0) {
        return HASH_APPLY;
    } else if (strcmp(operation, "hash_clear") == 0) {
        return HASH_CLEAR;
    } else if (strcmp(operation, "hash_delete") == 0) {
        return HASH_DELETE;
    } else if (strcmp(operation, "hash_empty") == 0) {
        return HASH_EMPTY;
    } else if (strcmp(operation, "hash_find") == 0) {
        return HASH_FIND;
    } else if (strcmp(operation, "hash_insert") == 0) {
        return HASH_INSERT;
    } else if (strcmp(operation, "hash_replace") == 0) {
        return HASH_REPLACE;
    } else if (strcmp(operation, "hash_size") == 0) {
        return HASH_SIZE;
    } else {
        return HASH_NOTHING;
    }
}

enum bitmap_operation getBitmapOperation(char *operation) {
    if (strcmp(operation, "bitmap_mark") == 0) {
        return BITMAP_MARK;
    } else if (strcmp(operation, "bitmap_all") == 0) {
        return BITMAP_ALL;
    } else if (strcmp(operation, "bitmap_any") == 0) {
        return BITMAP_ANY;
    } else if (strcmp(operation, "bitmap_contains") == 0) {
        return BITMAP_CONTAINS;
    } else if (strcmp(operation, "bitmap_count") == 0) {
        return BITMAP_COUNT;
    } else if (strcmp(operation, "bitmap_dump") == 0) {
        return BITMAP_DUMP;
    } else if (strcmp(operation, "bitmap_expand") == 0) {
        return BITMAP_EXPAND;
    } else if (strcmp(operation, "bitmap_flip") == 0) {
        return BITMAP_FLIP;
    } else if (strcmp(operation, "bitmap_none") == 0) {
        return BITMAP_NONE;
    } else if (strcmp(operation, "bitmap_reset") == 0) {
        return BITMAP_RESET;
    } else if (strcmp(operation, "bitmap_scan") == 0) {
        return BITMAP_SCAN;
    } else if (strcmp(operation, "bitmap_scan_and_flip") == 0) {
        return BITMAP_SCAN_AND_FLIP;
    } else if (strcmp(operation, "bitmap_set") == 0) {
        return BITMAP_SET;
    } else if (strcmp(operation, "bitmap_set_all") == 0) {
        return BITMAP_SET_ALL;
    } else if (strcmp(operation, "bitmap_set_multiple") == 0) {
        return BITMAP_SET_MULTIPLE;
    } else if (strcmp(operation, "bitmap_size") == 0) {
        return BITMAP_SIZE;
    } else if (strcmp(operation, "bitmap_test") == 0) {
        return BITMAP_TEST;
    } else {
        return BITMAP_NOTHING;
    }
}

enum list_operation getListOperation(const char *line) {
    if (strcmp(line, "list_push_front") == 0) {
        return LIST_PUSH_FRONT;
    } else if (strcmp(line, "list_push_back") == 0) {
        return LIST_PUSH_BACK;
    } else if (strcmp(line, "list_pop_front") == 0) {
        return LIST_POP_FRONT;
    } else if (strcmp(line, "list_pop_back") == 0) {
        return LIST_POP_BACK;
    } else if (strcmp(line, "list_front") == 0) {
        return LIST_FRONT;
    } else if (strcmp(line, "list_back") == 0) {
        return LIST_BACK;
    } else if (strcmp(line, "list_insert") == 0) {
        return LIST_INSERT;
    } else if (strcmp(line, "list_insert_ordered") == 0) {
        return LIST_INSERT_ORDERED;
    } else if (strcmp(line, "list_empty") == 0) {
        return LIST_EMPTY;
    } else if (strcmp(line, "list_size") == 0) {
        return LIST_SIZE;
    } else if (strcmp(line, "list_max") == 0) {
        return LIST_MAX;
    } else if (strcmp(line, "list_min") == 0) {
        return LIST_MIN;
    } else if (strcmp(line, "list_remove") == 0) {
        return LIST_REMOVE;
    } else if (strcmp(line, "list_reverse") == 0) {
        return LIST_REVERSE;
    } else if (strcmp(line, "list_shuffle") == 0) {
        return LIST_SHUFFLE;
    } else if (strcmp(line, "list_swap") == 0) {
        return LIST_SWAP;
    } else if (strcmp(line, "list_sort") == 0) {
        return LIST_SORT;
    } else if (strcmp(line, "list_unique") == 0) {
        return LIST_UNIQUE;
    } else if (strcmp(line, "list_splice") == 0) {
        return LIST_SPLICE;
    } else {
        return LIST_NOTHING;
    }
}

void printConsole(char *commandLine) {
    commandLine[0] = '\0';
    scanf("%[^\n]", commandLine);
    getchar();
}

int getOperation(char *line) {
    if (strncmp(line, "create", 5) == 0) {
        return CREATE;
    } else if (strncmp(line, "delete", 6) == 0) {
        return DELETE;
    } else if (strncmp(line, "dumpdata", 9) == 0) {
        return DUMP_DATA;
    } else if (strncmp(line, "list_", 5) == 0) {
        return LIST_OPERATION;
    } else if (strncmp(line, "hash_", 5) == 0) {
        return HASH_OPERATION;
    } else if (strncmp(line, "bitmap_", 7) == 0) {
        return BITMAP_OPERATION;
    } else if (strcmp(line, "quit") == 0) {
        return QUIT;
    } else {
        return DEFAULT;
    }
}

int getDataStructure(const char *line) {
    if (strncmp(line, "list", 4) == 0) {
        return LIST;
    } else if (strncmp(line, "hash", 4) == 0) {
        return HASH;
    } else if (strncmp(line, "bitmap", 6) == 0) {
        return BITMAP;
    } else {
        return NOTHING;
    }
}

enum data_structure getDataType(const struct data_name_map *name_table, const char *data_name) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(name_table[i].name, data_name) == 0) {
            if (name_table[i].list != NULL) {
                return LIST;
            } else if (name_table[i].hash != NULL) {
                return HASH;
            } else if (name_table[i].bitmap != NULL) {
                return BITMAP;
            } else {
                continue;
            }
        }
    }
    return NOTHING;
}

bool list_less(struct list_elem *elem1, struct list_elem *elem2, void *aux) {
    struct list_item *pItem1 = list_entry(elem1, struct list_item, elem);
    struct list_item *pItem2 = list_entry(elem2, struct list_item, elem);

    return pItem1->data < pItem2->data;
}

struct bitmap *getBitmapByName(const struct data_name_map *name_table, const char *data_name) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(name_table[i].name, data_name) == 0) {
            return name_table[i].bitmap;
        }
    }
    return NULL;
}

struct hash *getHashByName(const struct data_name_map *name_table, const char *data_name) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(name_table[i].name, data_name) == 0) {
            return name_table[i].hash;
        }
    }
    return NULL;
}

struct list *getListByName(const struct data_name_map *data_map, const char *data_name) {
    struct list *picked_list = NULL;
    for (int i = 0; i < 10; i++) {
        if (strcmp(data_map[i].name, data_name) == 0) {
            picked_list = data_map[i].list;
            break;
        }
    }
    if (picked_list == NULL) {
        fprintf(stderr, "There is no list name : %s\n", data_name);
        return NULL;
    }
    return picked_list;
}