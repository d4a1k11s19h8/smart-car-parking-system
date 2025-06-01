#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h> 
#include <ctype.h>

#define M 3 // Order: Max children = M, Max keys = M-1
#define MAX_SPACES 50
#define MAX_VEHICLES 150

typedef struct Vehicle {
    char v_num[20];
    char owner[50];
    char arr_date[11]; 
    char arr_time[5];  
    char dep_date[11]; 
    char dep_time[5];
    int  membership;  
    float total_hrs;
    int space_id; 
    int parks;
    float revenue;
} Vehicle;

typedef struct ParkingSpace {
    int id;
    int status;  
    float revenue;
    float hrs;
} ParkingSpace;

typedef struct BPlusTreeNode {
    int nkeys;
    bool leaf_flag;
    struct BPlusTreeNode *parent;

    // Internal Node
    char int_vkeys[M - 1][20];
    int int_skeys[M - 1];
    struct BPlusTreeNode *child[M];

    // Leaf Node
    Vehicle leaf_v[M - 1];
    ParkingSpace leaf_s[M - 1];
    struct BPlusTreeNode *next;
    struct BPlusTreeNode *prev;

} BPlusTreeNode;

BPlusTreeNode *v_root = NULL;
BPlusTreeNode *s_root = NULL;

void swapElements(void *a, void *b, int size) {
    char *p1 = (char *)a;
    char *p2 = (char *)b;
    char temp;
    for (int i = 0; i < size; i++) {
        temp = p1[i];
        p1[i] = p2[i];
        p2[i] = temp;
    }
}

int partition(void *base, int low, int high, int size, int (*compare)(const void*, const void*)) {
    void *pivot = (char *)base + high * size;
    int i = low - 1; 

    for (int j = low; j <= high - 1; j++) {
        void *current = (char *)base + j * size;
        if (compare(current, pivot) <= 0) {
            i++; 
            swapElements((char *)base + i * size, current, size);
        }
    }
    swapElements((char *)base + (i + 1) * size, (char *)base + high * size, size);
    return (i + 1); 
}

void quickSortRecursive(void *base, int low, int high, int size, int (*compare)(const void*, const void*)) {
    if (low < high) {
        
        int pi = partition(base, low, high, size, compare);
        quickSortRecursive(base, low, pi - 1, size, compare);
        quickSortRecursive(base, pi + 1, high, size, compare);
    }
}

void quickSort(void *base, int num, int size, int (*compare)(const void*, const void*)) {
    if (num > 1) { 
        quickSortRecursive(base, 0, num - 1, size, compare);
    }
}

Vehicle* findVehicle(BPlusTreeNode *node, const char *v_num);
ParkingSpace* findSpace(BPlusTreeNode *node, int s_id);
BPlusTreeNode* findLeafNodeV(BPlusTreeNode* node, const char* v_num);
BPlusTreeNode* findLeafNodeS(BPlusTreeNode* node, int s_id);
void insertVehicle(Vehicle v);
void insertSpace(ParkingSpace s);
void collectVehicles(BPlusTreeNode *node, Vehicle *v_arr, int *cnt);
void collectSpaces(BPlusTreeNode *node, ParkingSpace *s_arr, int *cnt);
void saveDataAndFree();
int compareSpacesByID(const void *a, const void *b);
void loadSpaces();
void loadVehicles();

BPlusTreeNode *createNode(bool is_leaf) {
    BPlusTreeNode *node = (BPlusTreeNode *)calloc(1, sizeof(BPlusTreeNode));
    node->leaf_flag = is_leaf;
    return node;
}

// Find Leaf Node (Vehicle)
BPlusTreeNode* findLeafNodeV(BPlusTreeNode* node, const char* v_num) {
    if (!node) return NULL;
    BPlusTreeNode* curr = node;
    while (!curr->leaf_flag) {
        int i = 0;
        while (i < curr->nkeys && strcmp(curr->int_vkeys[i], v_num) <= 0) {
            i++;
        }
        if (i > curr->nkeys || !curr->child[i]) {
             return NULL;
        }
        curr = curr->child[i];
    }
    if (!curr->leaf_flag) {
         return NULL;
    }
    return curr;
}

// Find Leaf Node (Space)
BPlusTreeNode* findLeafNodeS(BPlusTreeNode* node, int s_id) {
    if (!node) return NULL;
    BPlusTreeNode* curr = node;
    while (!curr->leaf_flag) {
        int i = 0;
        while (i < curr->nkeys && curr->int_skeys[i] <= s_id) {
            i++;
        }
         if (i > curr->nkeys || !curr->child[i]) {
             return NULL;
         }
        curr = curr->child[i];
    }
    if (!curr->leaf_flag) {
        return NULL;
    }
    return curr;
}

void insertIntoLeafV(BPlusTreeNode* l_node, Vehicle v);
void insertIntoParentV(BPlusTreeNode* left, const char* k, BPlusTreeNode* right);
void insertIntoLeafS(BPlusTreeNode* l_node, ParkingSpace s);
void insertIntoParentS(BPlusTreeNode* left, int k, BPlusTreeNode* right);


void insertIntoLeafV(BPlusTreeNode* l_node, Vehicle v) {
    int i = l_node->nkeys - 1;
    while (i >= 0 && strcmp(l_node->leaf_v[i].v_num, v.v_num) > 0) {
        l_node->leaf_v[i + 1] = l_node->leaf_v[i];
        i--;
    }
    l_node->leaf_v[i + 1] = v;
    l_node->nkeys++;
}


void insertIntoParentV(BPlusTreeNode* left, const char* k, BPlusTreeNode* right) {
    BPlusTreeNode* p = left->parent;

    if (p == NULL) {
        BPlusTreeNode* new_root_node = createNode(false);
        strcpy(new_root_node->int_vkeys[0], k);
        new_root_node->child[0] = left;
        new_root_node->child[1] = right;
        new_root_node->nkeys = 1;
        if(left) left->parent = new_root_node;
        if(right) right->parent = new_root_node;
        v_root = new_root_node;
        return;
    }

    if (p->nkeys < M - 1) {
        int i = p->nkeys - 1;
        while (i >= 0 && strcmp(p->int_vkeys[i], k) > 0) {
            strcpy(p->int_vkeys[i + 1], p->int_vkeys[i]);
            p->child[i + 2] = p->child[i + 1];
            i--;
        }
        strcpy(p->int_vkeys[i + 1], k);
        p->child[i + 2] = right;
        if (right) right->parent = p;
        p->nkeys++;
    } else { 
        BPlusTreeNode* new_node = createNode(false);
        if (!new_node) return;
        char tmp_k[M][20]; BPlusTreeNode* tmp_c[M + 1]; int i = 0, j = 0;

        
        while (i < p->nkeys && strcmp(p->int_vkeys[i], k) < 0) {
            strcpy(tmp_k[j], p->int_vkeys[i]); tmp_c[j] = p->child[i]; i++; j++;
        }
        strcpy(tmp_k[j], k); tmp_c[j] = p->child[i]; tmp_c[j + 1] = right; j++;
        while (i < p->nkeys) {
             strcpy(tmp_k[j], p->int_vkeys[i]); tmp_c[j + 1] = p->child[i + 1]; i++; j++;
        }
        int split_idx = M / 2; char up_key[20]; strcpy(up_key, tmp_k[split_idx]);

      
        p->nkeys = split_idx;
        memset(p->int_vkeys, 0, sizeof(p->int_vkeys));
        for(i=0; i < p->nkeys; ++i) {
            strcpy(p->int_vkeys[i], tmp_k[i]);
            p->child[i] = tmp_c[i];
            if(p->child[i]) p->child[i]->parent = p;
        }
        p->child[p->nkeys] = tmp_c[split_idx];
        if(p->child[p->nkeys]) p->child[p->nkeys]->parent = p;

        new_node->nkeys = M - 1 - p->nkeys;
        new_node->parent = p->parent;
        memset(new_node->int_vkeys, 0, sizeof(new_node->int_vkeys));
        for(i=0, j=split_idx + 1; i < new_node->nkeys; ++i, ++j) {
            strcpy(new_node->int_vkeys[i], tmp_k[j]);
            new_node->child[i] = tmp_c[j];
            if(new_node->child[i]) new_node->child[i]->parent = new_node;
        }
        new_node->child[new_node->nkeys] = tmp_c[M];
        if(new_node->child[new_node->nkeys]) new_node->child[new_node->nkeys]->parent = new_node;

        
        for(i = p->nkeys + 1; i <= M; ++i) p->child[i] = NULL;
        for(i = new_node->nkeys + 1; i <= M; ++i) new_node->child[i] = NULL;

        
        insertIntoParentV(p, up_key, new_node);
    }
}

void insertVehicle(Vehicle v) {
    if (v_root == NULL) {
        v_root = createNode(true);
        v_root->leaf_v[0] = v;
        v_root->nkeys = 1;
        return;
    }

    BPlusTreeNode* l_node = findLeafNodeV(v_root, v.v_num);
    if (!l_node) {
        fprintf(stderr, "  insertV: Failed find leaf for %s\n", v.v_num);
        return;
    }

    bool found = false;
    for (int i = 0; i < l_node->nkeys; ++i) {
         if (strcmp(l_node->leaf_v[i].v_num, v.v_num) == 0) {
             l_node->leaf_v[i] = v; 
             found = true;
             i = l_node->nkeys; 
         }
     }
    if(found) return; 

    if (l_node->nkeys < M - 1) { 
        insertIntoLeafV(l_node, v);
    } else { 
        BPlusTreeNode* new_l = createNode(true);
        if (!new_l) { printf("Failed alloc new_leaf!\n"); return; }

        Vehicle tmp_v[M];
        int i = 0, j = 0;
        while (i < l_node->nkeys && strcmp(l_node->leaf_v[i].v_num, v.v_num) < 0) {
            tmp_v[j++] = l_node->leaf_v[i++];
        }
        tmp_v[j++] = v; 
        while (i < l_node->nkeys) {
            tmp_v[j++] = l_node->leaf_v[i++];
        }
        int split_pt = (int)ceil((double)M / 2.0);

        l_node->nkeys = split_pt;
        memset(l_node->leaf_v, 0, sizeof(l_node->leaf_v));
        for (i = 0; i < l_node->nkeys; i++) { l_node->leaf_v[i] = tmp_v[i]; }

        new_l->nkeys = M - split_pt;
        memset(new_l->leaf_v, 0, sizeof(new_l->leaf_v));
        for (i = 0, j = split_pt; i < new_l->nkeys; i++, j++) { new_l->leaf_v[i] = tmp_v[j]; }

        new_l->next = l_node->next;
        if (l_node->next) { l_node->next->prev = new_l; }
        l_node->next = new_l;
        new_l->prev = l_node;
        new_l->parent = l_node->parent;

        insertIntoParentV(l_node, new_l->leaf_v[0].v_num, new_l);
    }
}

void insertIntoLeafS(BPlusTreeNode* l_node, ParkingSpace s) {
    int i = l_node->nkeys - 1;
    while (i >= 0 && l_node->leaf_s[i].id > s.id) {
        l_node->leaf_s[i + 1] = l_node->leaf_s[i];
        i--;
    }
    l_node->leaf_s[i + 1] = s;
    l_node->nkeys++;
}

void insertIntoParentS(BPlusTreeNode* left, int k, BPlusTreeNode* right) {
     BPlusTreeNode* p = left->parent;

    if (p == NULL) { 
        BPlusTreeNode* new_root_node = createNode(false);
        new_root_node->int_skeys[0] = k;
        new_root_node->child[0] = left;
        new_root_node->child[1] = right;
        new_root_node->nkeys = 1;
        if (left) left->parent = new_root_node;
        if (right) right->parent = new_root_node;
        s_root = new_root_node;
        return;
    }

    if (p->nkeys < M - 1) { 
        int i = p->nkeys - 1;
        while (i >= 0 && p->int_skeys[i] > k) {
            p->int_skeys[i + 1] = p->int_skeys[i];
            p->child[i + 2] = p->child[i + 1];
            i--;
        }
        p->int_skeys[i + 1] = k;
        p->child[i + 2] = right;
        if (right) right->parent = p;
        p->nkeys++;
    } else { 
        BPlusTreeNode* new_node = createNode(false);
        if (!new_node) return;

        int tmp_k[M]; BPlusTreeNode* tmp_c[M + 1]; int i = 0, j = 0;
        while (i < p->nkeys && p->int_skeys[i] < k) {
            tmp_k[j] = p->int_skeys[i]; tmp_c[j] = p->child[i]; i++; j++;
        }
        tmp_k[j] = k; tmp_c[j] = p->child[i]; tmp_c[j + 1] = right; j++;
        while (i < p->nkeys) {
             tmp_k[j] = p->int_skeys[i]; tmp_c[j + 1] = p->child[i + 1]; i++; j++;
        }

        int split_idx = M / 2; int up_key = tmp_k[split_idx];

        p->nkeys = split_idx;
        memset(p->int_skeys, 0, sizeof(p->int_skeys));
        for(i=0; i < p->nkeys; ++i) {
            p->int_skeys[i] = tmp_k[i];
            p->child[i] = tmp_c[i];
             if(p->child[i]) p->child[i]->parent = p;
        }
        p->child[p->nkeys] = tmp_c[split_idx];
        if(p->child[p->nkeys]) p->child[p->nkeys]->parent = p;

        new_node->nkeys = M - 1 - p->nkeys;
        new_node->parent = p->parent;
        memset(new_node->int_skeys, 0, sizeof(new_node->int_skeys));
        for(i=0, j=split_idx + 1; i < new_node->nkeys; ++i, ++j) {
            new_node->int_skeys[i] = tmp_k[j];
            new_node->child[i] = tmp_c[j];
            if(new_node->child[i]) new_node->child[i]->parent = new_node;
        }
        new_node->child[new_node->nkeys] = tmp_c[M];
        if(new_node->child[new_node->nkeys]) new_node->child[new_node->nkeys]->parent = new_node;

        for(i = p->nkeys + 1; i <= M; ++i) p->child[i] = NULL;
        for(i = new_node->nkeys + 1; i <= M; ++i) new_node->child[i] = NULL;
        insertIntoParentS(p, up_key, new_node);
    }
}


void insertSpace(ParkingSpace s) {
    if (s_root == NULL) {
        s_root = createNode(true);
        s_root->leaf_s[0] = s;
        s_root->nkeys = 1;
        return;
    }

    BPlusTreeNode* l_node = findLeafNodeS(s_root, s.id);
     if (!l_node) {
        fprintf(stderr, "Err: Cannot find leaf for space %d\n", s.id);
        return;
    }

     bool found = false;
     for (int i = 0; i < l_node->nkeys; ++i) {
         if (l_node->leaf_s[i].id == s.id) {
             l_node->leaf_s[i] = s; 
             found = true;
             i = l_node->nkeys;
         }
     }
     if(found) return; 

    if (l_node->nkeys < M - 1) { 
        insertIntoLeafS(l_node, s);
    } else { 
        BPlusTreeNode* new_l = createNode(true);
         if (!new_l) return;

        ParkingSpace tmp_s[M];
        int i = 0, j = 0;
        while (i < l_node->nkeys && l_node->leaf_s[i].id < s.id) {
            tmp_s[j++] = l_node->leaf_s[i++];
        }
        tmp_s[j++] = s;
        while (i < l_node->nkeys) {
            tmp_s[j++] = l_node->leaf_s[i++];
        }

        int split_pt = (int)ceil((double)M / 2.0);

        l_node->nkeys = split_pt;
        memset(l_node->leaf_s, 0, sizeof(l_node->leaf_s));
        for (i = 0; i < l_node->nkeys; i++) { l_node->leaf_s[i] = tmp_s[i]; }

        new_l->nkeys = M - split_pt;
        memset(new_l->leaf_s, 0, sizeof(new_l->leaf_s));
        for (i = 0, j = split_pt; i < new_l->nkeys; i++, j++) { new_l->leaf_s[i] = tmp_s[j]; }

        new_l->next = l_node->next;
         if (l_node->next) { l_node->next->prev = new_l; }
        l_node->next = new_l;
        new_l->prev = l_node;
        new_l->parent = l_node->parent;

        insertIntoParentS(l_node, new_l->leaf_s[0].id, new_l);
    }
}

Vehicle* findVehicle(BPlusTreeNode *node, const char *v_num) {
    BPlusTreeNode* l_node = findLeafNodeV(node, v_num);
    if (!l_node) return NULL;

    for (int i = 0; i < l_node->nkeys; i++) {
        if (strcmp(l_node->leaf_v[i].v_num, v_num) == 0) {
            return &l_node->leaf_v[i];
        }
    }
    return NULL; 
}

ParkingSpace* findSpace(BPlusTreeNode *node, int s_id) {
    BPlusTreeNode* l_node = findLeafNodeS(node, s_id);
    if (!l_node) return NULL;

    for (int i = 0; i < l_node->nkeys; i++) {
        if (l_node->leaf_s[i].id == s_id) {
            return &l_node->leaf_s[i];
        }
    }
    return NULL; 
}

int allocateSpace(int membership) {
    int start, end;
    if (membership == 2) { start = 1; end = 10; }
    else if (membership == 1) { start = 11; end = 20; }
    else { start = 21; end = MAX_SPACES; }

    for (int i = start; i <= end; i++) {
        ParkingSpace *sp = findSpace(s_root, i);
        if (sp && sp->status == 0) {
            sp->status = 1;
            printf("Allocated space %d (membership: %d)\n", i, membership);
            return i;
        }
    }
    printf("Err: No available space.\n");
    return -1;
}

float calcHours(const char *arr_dt, const char *arr_tm,
                const char *dep_dt, const char *dep_tm) {
    struct tm arr = {0}, dep = {0};
    int ad, am, ay, ah, amin;
    int dd, dm, dy, dh, dmin;

    // Check for null or placeholder strings
    if (!arr_dt || !arr_tm || !dep_dt || !dep_tm ||
        strlen(arr_dt) == 0 || strlen(arr_tm) == 0 ||
        strlen(dep_dt) == 0 || strlen(dep_tm) == 0 ||
        strcmp(arr_dt, "-") == 0 || strcmp(arr_tm, "-") == 0 ||
        strcmp(dep_dt, "-") == 0 || strcmp(dep_tm, "-") == 0)
    {
         return 0.0;
    }

    // Parse dates and times
    if (sscanf(arr_dt, "%2d%2d%4d", &ad, &am, &ay) != 3 ||
        sscanf(arr_tm, "%2d%2d", &ah, &amin) != 2 ||
        sscanf(dep_dt, "%2d%2d%4d", &dd, &dm, &dy) != 3 ||
        sscanf(dep_tm, "%2d%2d", &dh, &dmin) != 2) {
         // fprintf(stderr, "Err parse date/time in calcHours.\n");
        return 0.0;
    }

    // Populate tm structs
    arr.tm_mday = ad; arr.tm_mon = am - 1; arr.tm_year = ay - 1900;
    arr.tm_hour = ah; arr.tm_min = amin; arr.tm_isdst = -1;
    dep.tm_mday = dd; dep.tm_mon = dm - 1; dep.tm_year = dy - 1900;
    dep.tm_hour = dh; dep.tm_min = dmin; dep.tm_isdst = -1;

    // Convert to time_t
    time_t t_arr = mktime(&arr);
    time_t t_dep = mktime(&dep);

    if (t_arr == (time_t)-1 || t_dep == (time_t)-1) {
        // fprintf(stderr, "Err mktime failed.\n");
        return 0.0;
    }
    if (t_dep < t_arr) {
         // fprintf(stderr, "Warn: Dep time < Arr time.\n");
         return 0.0;
    }

    // Calculate difference in hours
    double diff_sec = difftime(t_dep, t_arr);
    return (float)(diff_sec / 3600.0);
}

float calcPay(float hrs, int membership) {
     if (hrs < 0) hrs = 0;
    float rate;
    float discount = 1.0;

    if (hrs <= 3.0) {
        rate = 100.0;
    } else {
         float extra_hrs = hrs - 3.0;
         rate = 100.0 + extra_hrs * 50.0;
    }

    if (membership == 1) discount = 0.9; // Premium
    else if (membership == 2) discount = 0.8; // Gold

    return rate * discount;
}

void checkMembership(Vehicle *v) {
    if (!v) return;
    int new_mem = 0;
    if (v->total_hrs >= 200.0) new_mem = 2; // Gold
    else if (v->total_hrs >= 100.0) new_mem = 1; // Premium

    if (new_mem > v->membership) {
         printf("Membership Upgraded! V# %s is now %s (%.2f hrs).\n",
               v->v_num, (new_mem == 2 ? "Gold" : "Premium"), v->total_hrs);
        v->membership = new_mem;
    }
}

void loadSpaces() {
    printf("Init parking spaces...\n");
    FILE *fp = fopen("parking-lot-data.txt", "r");
    int loaded_count = 0;
    bool loaded_ids[MAX_SPACES + 1] = {false};

    if (fp) {
        // printf("Loading from bplus-parking-lot-data.txt...\n");
        int s_id, s_stat; float s_rev, s_hrs; char line[100];

        while (fgets(line, sizeof(line), fp)) {
            if (line[0] == '\n' || line[0] == '#') continue;
            if (sscanf(line, "%d %d %f %f", &s_id, &s_stat, &s_hrs, &s_rev) == 4) {
                if (s_id > 0 && s_id <= MAX_SPACES) {
                    ParkingSpace s = {s_id, s_stat, s_rev, s_hrs};
                    insertSpace(s);
                    loaded_ids[s_id] = true;
                    loaded_count++;
                } 
            } 
        }
        fclose(fp);
        printf("Loaded %d spaces from file.\n", loaded_count);
    } else {
         printf("Info:parking-lot-data.txt not found.\n");
    }

    // int default_added = 0;
    // for (int i = 1; i <= MAX_SPACES; i++) {
    //     if (!loaded_ids[i]) {
    //         if (findSpace(s_root, i) == NULL) {
    //              ParkingSpace def_s = {i, 0, 0.0, 0.0};
    //              insertSpace(def_s);
    //              default_added++;
    //         }
    //     }
    // }
    //  if (default_added > 0) {
    //     printf("Added %d default spaces.\n", default_added);
    //  }

    //  int final_count = 0;
    //  ParkingSpace tmp_s[MAX_SPACES + 1];
    //  collectSpaces(s_root, tmp_s, &final_count);
    //  printf("Total spaces in tree: %d\n", final_count);
    //  if (final_count != MAX_SPACES) {
    //      fprintf(stderr, "ERROR: Space count mismatch!\n");
    //  }
}

void loadVehicles() {
    FILE *fp = fopen("complete-vehicle-database-100.txt", "r");
    if (!fp) {
        printf("Info: bplus-vehicle-database.txt not found.\n");
        return;
    }

    printf("Loading vehicles...\n");
    char line[256]; int count = 0; int skipped = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '\n' || line[0] == '#') continue;

        Vehicle v; memset(&v, 0, sizeof(Vehicle));
        char vn[20], own[50], ad[11], at[5], dd[11], dt[5];
        int membership, sid, np; float th, tr;

        int parsed = sscanf(line, "%19s %49[^0-9-] %10s %4s %10s %4s %d %f %d %d %f",
                            vn, own, ad, at, dd, dt, &membership, &th, &sid, &np, &tr);

        char *end = own + strlen(own) - 1;
        while(end > own && isspace((unsigned char)*end)) end--;
        *(end + 1) = 0;

        if (parsed == 11) { 
             strcpy(v.v_num, vn); strcpy(v.owner, own);
             strcpy(v.arr_date, (strcmp(ad, "-") == 0 ? "" : ad));
             strcpy(v.arr_time, (strcmp(at, "-") == 0 ? "" : at));
             strcpy(v.dep_date, (strcmp(dd, "-") == 0 ? "" : dd));
             strcpy(v.dep_time, (strcmp(dt, "-") == 0 ? "" : dt));
             v.membership = membership; v.total_hrs = th; v.space_id = sid; v.parks = np; v.revenue = tr;
             if (strlen(v.v_num) == 0) { 
                 skipped++;
                 continue; 
             }
             insertVehicle(v);
             count++;
             if (v.space_id > 0 && strlen(v.dep_date) == 0) {
                 ParkingSpace *sp = findSpace(s_root, v.space_id);
                 if (sp && sp->status == 0) {
                      sp->status = 1; 
                 } else if (!sp) {
                    fprintf(stderr, "Warn: Vehicle %s -> non-existent space %d.\n", v.v_num, v.space_id);
                 }
             }
        } else { 
           
            skipped++;
            continue;
        }
    } 
 	if (ferror(fp)) perror("Err reading vehicle file");
    printf("Loaded/Updated %d vehicles.", count);
    if (skipped > 0) printf(" Skipped %d lines.", skipped);
    printf("\n");
    fclose(fp);
}

void vehicleEntry(const char *v_num, const char *owner) {
    time_t now; struct tm *local_tm;
    char date_str[11]; char time_str[5];

    time(&now); local_tm = localtime(&now);
    if (local_tm == NULL) {
        perror("localtime err"); strcpy(date_str, "00000000"); strcpy(time_str, "0000");
    } else {
        strftime(date_str, sizeof(date_str), "%d%m%Y", local_tm);
        strftime(time_str, sizeof(time_str), "%H%M", local_tm);
    }

    Vehicle* ev = findVehicle(v_root, v_num);

    if (ev) { 
        printf("Welcome back, %s (%s)!\n", owner, v_num);
        if (ev->space_id > 0 && strlen(ev->dep_date) == 0) {
             printf("Err: Vehicle %s already parked in %d.\n", v_num, ev->space_id);
             return;
        }

        strcpy(ev->owner, owner);
        strcpy(ev->arr_date, date_str); strcpy(ev->arr_time, time_str);
        strcpy(ev->dep_date, ""); strcpy(ev->dep_time, "");

        int alloc_sp = allocateSpace(ev->membership);
        if (alloc_sp == -1) { 
            printf("Sorry %s, no space for %s.\n", owner, v_num);
             strcpy(ev->arr_date, ""); strcpy(ev->arr_time, ""); 
             return;
        }
        ev->space_id = alloc_sp;
        printf("V# %s assigned space %d on %s @ %s.\n", v_num, alloc_sp, date_str, time_str);

    } else { 
        printf("Registering new vehicle: %s (%s)\n", owner, v_num);
        Vehicle nv = {0}; 
        strcpy(nv.v_num, v_num); strcpy(nv.owner, owner);
        strcpy(nv.arr_date, date_str); strcpy(nv.arr_time, time_str);
          
        int alloc_sp = allocateSpace(nv.membership);
         if (alloc_sp == -1) { 
            printf("Sorry %s, no space for new vehicle %s.\n", owner, v_num);
            return; 
        }
        nv.space_id = alloc_sp; 
        insertVehicle(nv); 
        printf("New V# %s registered, space %d on %s @ %s.\n", v_num, alloc_sp, date_str, time_str);
    }
}

void vehicleExit(const char *v_num) {
    time_t now; struct tm *local_tm;
    char dep_date_str[11]; char dep_time_str[5];

    time(&now); local_tm = localtime(&now);
    if (local_tm == NULL) {
        perror("localtime err");
        fprintf(stderr, "Err getting time for exit %s.\n", v_num); return;
    } else {
        strftime(dep_date_str, sizeof(dep_date_str), "%d%m%Y", local_tm);
        strftime(dep_time_str, sizeof(dep_time_str), "%H%M", local_tm);
    }

    Vehicle *v = findVehicle(v_root, v_num);

    if (!v) { printf("Err: Vehicle %s not found.\n", v_num); return; }
    if (v->space_id <= 0 || strlen(v->dep_date) > 0) {
         printf("Err: Vehicle %s not parked.\n", v_num); return;
    }
    if (strlen(v->arr_date) == 0 || strlen(v->arr_time) == 0 || strcmp(v->arr_date, "-") == 0 || strcmp(v->arr_time, "-") == 0) {
         printf("Err: V# %s has bad arrival data (%s %s).\n", v_num, v->arr_date, v->arr_time); return;
    }

    int sp_id = v->space_id; 

    float sess_hrs = calcHours(v->arr_date, v->arr_time, dep_date_str, dep_time_str);
    if (sess_hrs < 0) {
        printf("Err calculating hours (<0). Check times.\n"); return;
    }
    float sess_pay = calcPay(sess_hrs, v->membership);

    printf("V# %s exiting space %d on %s @ %s.\n", v_num, sp_id, dep_date_str, dep_time_str);
    printf("  Arr: %s %s\n", v->arr_date, v->arr_time);
    printf("  Session: %.2f hrs, Pay: %.2f\n", sess_hrs, sess_pay);

    strcpy(v->dep_date, dep_date_str); strcpy(v->dep_time, dep_time_str);
    v->total_hrs += sess_hrs; v->revenue += sess_pay; v->parks += 1;
    v->space_id = 0; 

    checkMembership(v); 

    printf("  Updated Totals: %.2f hrs, %.2f rev, %d parks, membership: %d\n",
           v->total_hrs, v->revenue, v->parks, v->membership);

    ParkingSpace *sp = findSpace(s_root, sp_id);
    if (sp) {
        if (sp->status == 0) printf("Warn: Space %d was already free for V# %s exit.\n", sp_id, v_num);
        sp->status = 0; sp->revenue += sess_pay; sp->hrs += sess_hrs;
         printf("  Space %d freed. Updated Space: %.2f hrs, %.2f rev.\n", sp_id, sp->hrs, sp->revenue);
    } else {
        fprintf(stderr, "CRITICAL Err: Cannot find space %d to free!\n", sp_id);
    }
}


int compareVByHrs(const void *a, const void *b) {
    float diff = ((Vehicle *)b)->total_hrs - ((Vehicle *)a)->total_hrs; 
    if (diff > 1e-6) return 1;  
    if (diff < -1e-6) return -1; 
    return strcmp(((Vehicle *)a)->v_num, ((Vehicle *)b)->v_num);
}
int compareVByRev(const void *a, const void *b) {
    float diff = ((Vehicle *)b)->revenue - ((Vehicle *)a)->revenue;
     if (diff > 1e-6) return 1;
     if (diff < -1e-6) return -1; 
     return strcmp(((Vehicle *)a)->v_num, ((Vehicle *)b)->v_num);
}
int compareSByHrs(const void *a, const void *b) {
    float diff = ((ParkingSpace *)b)->hrs - ((ParkingSpace *)a)->hrs;
     if (diff > 1e-6) return 1; 
     if (diff < -1e-6) return -1; 
     
     return ((ParkingSpace *)a)->id - ((ParkingSpace *)b)->id;
}
int compareSByRev(const void *a, const void *b) {
    float diff = ((ParkingSpace *)b)->revenue - ((ParkingSpace *)a)->revenue;
     if (diff > 1e-6) return 1; 
     if (diff < -1e-6) return -1; 
     return ((ParkingSpace *)a)->id - ((ParkingSpace *)b)->id;
}
int compareSpacesByID(const void *a, const void *b) {
    return ((ParkingSpace *)a)->id - ((ParkingSpace *)b)->id;
}

void collectVehicles(BPlusTreeNode *node, Vehicle *v_arr, int *cnt) {
    *cnt = 0;
    if (!node) return;

    BPlusTreeNode *curr = node;
    while (curr && !curr->leaf_flag) {
         if (curr->child[0] == NULL && curr->nkeys >= 0) { return; } 
        curr = curr->child[0];
    }

    if (!curr || !curr->leaf_flag) { return; } 

    
    bool collect_more = true;
    while (curr != NULL && collect_more) {
        for (int i = 0; i < curr->nkeys && collect_more; i++) {
            if (*cnt < MAX_VEHICLES) {
                v_arr[*cnt] = curr->leaf_v[i];
                (*cnt)++;
            } else {
                fprintf(stderr, "Warn: Exceeded MAX_VEHICLES.\n");
                collect_more = false; 
            }
        }
        if(collect_more) curr = curr->next;
    }
}

void collectSpaces(BPlusTreeNode *node, ParkingSpace *s_arr, int *cnt) {
     *cnt = 0;
    if (!node) return;

    BPlusTreeNode *curr = node;
    while (curr && !curr->leaf_flag) {
        if (curr->child[0] == NULL && curr->nkeys >= 0) { return; }
        curr = curr->child[0];
    }

     if (!curr || !curr->leaf_flag) { return; }

    bool collect_more = true;
    while (curr != NULL && collect_more) {
        for (int i = 0; i < curr->nkeys && collect_more; i++) {
            if (*cnt < MAX_SPACES) {
                s_arr[*cnt] = curr->leaf_s[i];
                (*cnt)++;
            } else {
                fprintf(stderr, "Warn: Exceeded MAX_SPACES.\n");
                collect_more = false;
            }
        }
        if(collect_more) curr = curr->next;
    }
}

void displayVByHrs(BPlusTreeNode *node) {
    if (!node) { printf("No vehicles.\n"); return; }
    Vehicle v_arr[MAX_VEHICLES]; int count = 0;
    collectVehicles(node, v_arr, &count);
    if (count == 0) { printf("No vehicles collected.\n"); return; }

    quickSort(v_arr, count, sizeof(Vehicle), compareVByHrs); 
    printf("\n--- Vehicles by Hours ---\n");
    printf("%-15s %-20s %-10s %-10s %-5s %-5s %-8s\n", "V#.","Owner","TotHrs","Revenue","Parks","membership","SpaceID");
    printf("---------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-15s %-20.20s %-10.2f %-10.2f %-5d %-5d %-8d\n",
               v_arr[i].v_num, v_arr[i].owner, v_arr[i].total_hrs, v_arr[i].revenue,
               v_arr[i].parks, v_arr[i].membership, v_arr[i].space_id);
    }
     printf("---------------------------------------------------------------------------\n");
}

void displayVByRev(BPlusTreeNode *node) {
     if (!node) { printf("No vehicles.\n"); return; }
    Vehicle v_arr[MAX_VEHICLES]; int count = 0;
    collectVehicles(node, v_arr, &count);
     if (count == 0) { printf("No vehicles collected.\n"); return; }

    quickSort(v_arr, count, sizeof(Vehicle), compareVByRev); 
    printf("\n--- Vehicles by Revenue ---\n");
    printf("%-15s %-20s %-10s %-10s %-5s %-5s %-8s\n", "V#.","Owner","Revenue","TotHrs","Parks","membership","SpaceID");
    printf("---------------------------------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
          printf("%-15s %-20.20s %-10.2f %-10.2f %-5d %-5d %-8d\n",
               v_arr[i].v_num, v_arr[i].owner, v_arr[i].revenue, v_arr[i].total_hrs,
               v_arr[i].parks, v_arr[i].membership, v_arr[i].space_id);
    }
     printf("---------------------------------------------------------------------------\n");
}

void displaySByHrs(BPlusTreeNode *node) {
    if (!node) { printf("No spaces.\n"); return; }
    ParkingSpace s_arr[MAX_SPACES]; int count = 0;
    collectSpaces(node, s_arr, &count);
     if (count == 0) { printf("No spaces collected.\n"); return; }

    quickSort(s_arr, count, sizeof(ParkingSpace), compareSByHrs); 
    printf("\n--- Spaces by Hours ---\n");
    printf("%-10s %-10s %-10s %-10s\n", "SpaceID", "Status", "Total Hrs", "Revenue");
    printf("--------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%-10d %-10s %-10.2f %-10.2f\n",
               s_arr[i].id, (s_arr[i].status == 1 ? "Occupied" : "Free"),
               s_arr[i].hrs, s_arr[i].revenue);
    }
    printf("--------------------------------------------\n");
}

void displaySByRev(BPlusTreeNode *node) {
     if (!node) { printf("No spaces.\n"); return; }
    ParkingSpace s_arr[MAX_SPACES]; int count = 0;
    collectSpaces(node, s_arr, &count);
     if (count == 0) { printf("No spaces collected.\n"); return; }

    quickSort(s_arr, count, sizeof(ParkingSpace), compareSByRev); 
    printf("\n--- Spaces by Revenue ---\n");
    printf("%-10s %-10s %-10s %-10s\n", "SpaceID", "Status", "Revenue", "Total Hrs");
    printf("--------------------------------------------\n");
    for (int i = 0; i < count; i++) {
         printf("%-10d %-10s %-10.2f %-10.2f\n",
               s_arr[i].id, (s_arr[i].status == 1 ? "Occupied" : "Free"),
               s_arr[i].revenue, s_arr[i].hrs);
    }
     printf("--------------------------------------------\n");
}


void clear_input_buf() {
    int c; while ((c = getchar()) != '\n' && c != EOF);
}

void showMenu() {
    int choice; char v_num[20]; char owner[50];
    bool keep_running = true;

    while (keep_running) {
        printf("\n===== Parking System Menu =====\n");
        printf("1. Vehicle Entry\n");
        printf("2. Vehicle Exit\n");
        printf("3. List Vehicles by Hours\n");
        printf("4. List Vehicles by Revenue\n");
        printf("5. List Spaces by Hours\n");
        printf("6. List Spaces by Revenue\n");
        printf("7. Save and Exit\n");
        printf("===============================\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Enter number.\n");
            clear_input_buf();
        } else {
            clear_input_buf(); 

            switch (choice) {
                case 1: 
                    printf("Enter vehicle number: ");
                    if(scanf("%19s", v_num) != 1) { fprintf(stderr,"Bad v_num input.\n"); clear_input_buf();}
                    else {
                        clear_input_buf();
                        printf("Enter owner name: ");
                        if(scanf(" %49[^\n]", owner) != 1) { fprintf(stderr,"Bad owner input.\n"); clear_input_buf();}
                        else {
                             clear_input_buf();
                             vehicleEntry(v_num, owner);
                        }
                    }
                    break; 
                case 2: 
                    printf("Enter vehicle number: ");
                     if(scanf("%19s", v_num) != 1) { fprintf(stderr,"Bad v_num input.\n"); clear_input_buf();}
                     else {
                        clear_input_buf();
                        vehicleExit(v_num);
                     }
                    break; 
                case 3: displayVByHrs(v_root); break;
                case 4: displayVByRev(v_root); break;
                case 5: displaySByHrs(s_root); break;
                case 6: displaySByRev(s_root); break;
                case 7:
                    saveDataAndFree();
                    printf("Data saved. Exiting.\n");
                    keep_running = false; 
                    break; 
                default:
                    printf("Invalid choice.\n");
            } 

             if (keep_running) { 
                 printf("\nPress Enter...");
                 getchar(); 
             }
        } 
    } 
}



void freeTreeRecursive(BPlusTreeNode *n) {
    if (n == NULL) return;
    if (!n->leaf_flag) {
        for (int i = 0; i <= n->nkeys; i++) {
            freeTreeRecursive(n->child[i]);
            n->child[i] = NULL;
        }
    }
    free(n);
}

void saveVehiclesToFile(BPlusTreeNode *node, const char *fname) {
    FILE *fp = fopen(fname, "w");
    if (!fp) { perror("Err open vehicle file for write"); return; }

    Vehicle v_arr[MAX_VEHICLES]; int count = 0;
    collectVehicles(node, v_arr, &count);

    printf("Saving %d vehicles to %s...\n", count, fname);

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s %s %s %s %s %s %d %.2f %d %d %.2f\n",
                v_arr[i].v_num, v_arr[i].owner,
                v_arr[i].arr_date[0] ? v_arr[i].arr_date : "-",
                v_arr[i].arr_time[0] ? v_arr[i].arr_time : "-",
                v_arr[i].dep_date[0] ? v_arr[i].dep_date : "-",
                v_arr[i].dep_time[0] ? v_arr[i].dep_time : "-",
                v_arr[i].membership,
                v_arr[i].total_hrs >= 0 ? v_arr[i].total_hrs : 0.0, 
                v_arr[i].space_id, v_arr[i].parks,
                v_arr[i].revenue >= 0 ? v_arr[i].revenue : 0.0);
    }
    fclose(fp);
    printf("Vehicle save done.\n");
}

void saveSpacesToFile(BPlusTreeNode *node, const char *fname) {
    FILE *fp = fopen(fname, "w");
     if (!fp) { perror("Err open space file for write"); return; }

    ParkingSpace s_arr[MAX_SPACES]; int count = 0;
    collectSpaces(node, s_arr, &count);
    quickSort(s_arr, count, sizeof(ParkingSpace), compareSpacesByID); 

    printf("Saving %d spaces to %s...\n", count, fname);
    

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d %d %.2f %.2f\n",
                s_arr[i].id, s_arr[i].status,
                s_arr[i].hrs >= 0 ? s_arr[i].hrs : 0.0,
                s_arr[i].revenue >= 0 ? s_arr[i].revenue : 0.0);
    }
    fclose(fp);
    printf("Space save done.\n");
}

void saveDataAndFree() {
    printf("\n--- Saving Data ---\n");
    saveVehiclesToFile(v_root, "bplus-vehicle-database.txt");
    saveSpacesToFile(s_root, "bplus-parking-lot-data.txt");

    printf("\n--- Freeing Memory ---\n");
    freeTreeRecursive(v_root); v_root = NULL;
    printf("Vehicle tree freed.\n");
    freeTreeRecursive(s_root); s_root = NULL;
    printf("Space tree freed.\n");
}

int main() {
    printf("--- Init Parking System ---\n");
    loadSpaces();
    loadVehicles();
    printf("--- Init Complete ---\n");

    showMenu(); 

    printf("Program end.\n");
    return 0;
}