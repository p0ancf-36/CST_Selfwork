#include "Item.h"

Item scan_item() {
    Item result;
    printf("Enter ID: ");
    scanf("%d", &result.id);
    printf("Enter item name: ");
    scanf("%s[^\n]", &result.name);
    printf("Enter items in stock: ");
    scanf("%d", &result.in_stock);
    printf("Enter total item count: ");
    scanf("%d", &result.count);
    printf("Enter average rent count: ");
    scanf("%d", &result.av_rent_count);
    printf("Enter average rent time: ");
    scanf("%d", &result.av_rent_time);

    return result;
}

Item fscan_item(FILE *file) {
    Item result;
    fscanf(file, "%lu64", &result.id);
    fscanf(file, "%u32", &result.count);
    fscanf(file, "%u32", &result.in_stock);
    fscanf(file, "%u32", &result.av_rent_time);
    fscanf(file, "%u32", &result.av_rent_count);
    fscanf(file, " %[^\n]\n", &result.name);

    return result;
}

void print_item(const Item *item) {
    printf("%4d\t%-32s\t%4d/%-4d\t%4d\t%4d\n", item->id, item->name, item->in_stock, item->count, item->av_rent_count, item->av_rent_time);
}

void edit_item(Item *item) {
    int answer = 0;

    do {
        printf("What you want to do:\n"
               "1) edit item id\n"
               "2) edit item name\n"
               "3) edit item in stock count\n"
               "4) edit item total count\n"
               "5) edit item average rent count\n"
               "6) edit item average rent time\n"
               "7) end editing\n");

        scanf("%d", &answer);

        if (answer == 7)
            break;

        switch (answer) {
            case 1:
                printf("Enter new item id: ");
                scanf("%lu64", &item->id);
                break;
            case 2:
                printf("Enter new item name: ");
                scanf("%s[^\n]", &item->name);
                break;
            case 3:
                printf("Enter new item in stock count: ");
                scanf("%u32", &item->in_stock);
                break;
            case 4:
                printf("Enter new item total count: ");
                scanf("%u32", &item->count);
                break;
            case 5:
                printf("Enter new item average rent count: ");
                scanf("%u32", &item->av_rent_count);
                break;
            case 6:
                printf("Enter new item average rent time: ");
                scanf("%u32", &item->av_rent_time);
                break;
            default: break;
        }
    } while (answer != 7);
}
