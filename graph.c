#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STOPS 10



typedef struct route{
    int dis,time,id;
    struct route* next;
}route;



typedef struct stop{
    char name[25];
    int id;
}stop;

typedef struct graph{
    stop stops[MAX_STOPS];
    route *routes[MAX_STOPS];
    int count;
}graph;

void addStop(graph* map, char* name){
    int id = map->stops[map->count].id = map->count;
    strcpy(map->stops[id].name,name);
    map->count++;
}

void printStop(stop* s){
    printf("\nName: %s\t\tID: %d\n",s->name,s->id);
}

void printRoute(route* head){
    if(!head){
        printf("\nNo routes added yet.\n");
        return;
    }
    for(route* temp = head; temp; temp = temp->next){
        printf("\n| Distance(in KM): %d\tTime(in mins): %d | -> DestinationID: %d\n",temp->dis, temp->time, temp->id);
    }
}

void printMap(graph* map){
    for(int i = 0; i < map->count; i++){
        printStop(&map->stops[i]);
        printRoute(map->routes[i]);
    }
}

void addRoute(graph* map, int dis, int time, int id, int des){
    route* temp = map->routes[id];
    route* newroute = malloc(sizeof(route));
    newroute->dis = dis;
    newroute->time = time;
    newroute->id = des;
    newroute->next = NULL;
    if(!temp){
        map->routes[id] = newroute;
        return;
    }
    for(;temp->next;temp = temp->next);
    temp->next = newroute;
}

void saveMapToFile(graph* map, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // Save Stops
    fprintf(file, "STOPS\n");
    for (int i = 0; i < map->count; i++) {
        fprintf(file, "%d,%s\n", map->stops[i].id, map->stops[i].name);
    }

    // Save Routes
    fprintf(file, "ROUTES\n");
    for (int i = 0; i < map->count; i++) {
        route* temp = map->routes[i];
        while (temp) {
            fprintf(file, "%d,%d,%d,%d\n", i, temp->id, temp->dis, temp->time);
            temp = temp->next;
        }
    }

    fclose(file);
    printf("Map saved to %s successfully.\n", filename);
}

void loadMapFromFile(graph* map, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    char line[100];
    int mode = 0; // 0 = Stops, 1 = Routes
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "STOPS") == 0) {
            mode = 0;
            continue;
        } else if (strcmp(line, "ROUTES") == 0) {
            mode = 1;
            continue;
        }

        if (mode == 0) {
            // Parse Stops
            int id;
            char name[20];
            sscanf(line, "%d,%[^,]", &id, name);
            addStop(map, name);
        } else if (mode == 1) {
            // Parse Routes
            int src, dest, dis, time;
            sscanf(line, "%d,%d,%d,%d", &src, &dest, &dis, &time);
            addRoute(map, dis, time, src, dest);
        }
    }

    fclose(file);
    printf("Map loaded from %s successfully.\n", filename);
}

void find(route* stack[], int des, route* cur, int* c, int *flag){
    if(!cur || cur->id == des){
        if(cur->id == des){
            *flag = 1;
        }
        return;
    }
    stack[(*c)++] = cur;
    find(stack,des,cur->next,c,flag);
    if(!(*flag)){
        stack[--(*c)] = NULL;
    }
}

void findpath(graph* map, int s, int d){
    route *routeStack[MAX_STOPS];
    int count = 0, flag = 0;
    for(int i = 0; i < MAX_STOPS; i++){
        routeStack[i] = NULL;
    }
    find(routeStack,d,map->routes[s],&count, &flag);
    printf("%d\n",count);
}

void initialize_map(graph* map){
    printf("\nInitializing Map......\n");
    map->count = 0;
    for(int i = 0; i < MAX_STOPS; i++){
        map->routes[i] = NULL;
    }
    loadMapFromFile(map, "map.csv");
    printf("Map Initialized!!\n");
}

void main() {
    graph map;
    initialize_map(&map);
    printMap(&map);
    findpath(&map,0,3);
    saveMapToFile(&map, "map.csv");
}
