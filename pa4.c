#include "../include/common.h"
#include "pa4.h"


// don't remove these globals
struct heap_elem *min_heap_arr = NULL;

// don't modify this function
void initialize_min_heap_arr(struct heap_elem *arr)
{
  min_heap_arr = arr;
}

// don't modify this function
static void *allocate_memory(size_t size)
{
  return __mymalloc(size);
}

// don't modify this function
static void free_memory(void *ptr)
{
  __myfree(ptr);
}

// ---------------- Add your code below this line ----------------- //
int search_friend(struct list_records* tmp, char uid1[MAX_LEN]);
int cmp_uid(char *uid1, char *uid2);
void insert_list(struct list_records* tmp,struct record* person,struct record* friend);

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// return r->friends.
struct list_records* get_friends_list(struct record *r)
{
  return r->friends;
}

// Make r1 and r2 friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return 1 if r1 and r2 are already friends before this call.
// Otherwise, make r1 a friend of r2 and return 0.
// To make r1 a friend of r2, 
// insert r1 into the list r2->friends and insert r2 into the list r1->friends.
int make_friends(struct record *r1, struct record *r2)
{
  struct list_records* tmp1=r1->friends;
  struct list_records* tmp2=r2->friends;
  if (search_friend(tmp1,r2->uid)==0 || search_friend(tmp2,r1->uid)==0){
    insert_list(tmp1,r1,r2);
    insert_list(tmp2,r2,r1);
    return 0;
  }else{
    return 1;
  }
}

int search_friend(struct list_records* tmp, char uid1[MAX_LEN]){
  while (tmp!=NULL){
    if (cmp_uid(tmp->record->uid,uid1)==0){
      return 1;
    }
    tmp=tmp->next;
  }
  return 0;
}

int cmp_uid(char *uid1, char *uid2)
{
  int i;

  for (i = 0; i < MAX_LEN; i++) {
    if (uid1[i] > uid2[i]) {
      return 1;
    }
    else if (uid1[i] < uid2[i]) {
      return -1;
    }
  }
  return 0;
}

void insert_list(struct list_records* tmp,struct record* person,struct record* friend){
    struct list_records* new_node = (struct list_records*) allocate_memory(sizeof(struct list_records));
    new_node->record = friend;
    new_node->next = NULL;
    struct list_records* tmp_head=tmp;
    while (tmp_head!=NULL){
      tmp_head=tmp_head->next;
    }
    if (tmp == NULL) {
        person->friends=new_node;
    } else {
        struct list_records* cur = tmp;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = new_node;
    }
}

// remove all nodes from r->friends and release the corresponding memory
void delete_friends_list(struct record *r)
{
  struct list_records* friend=r->friends;
  struct list_records* prev=NULL;
  while (friend!=NULL){
    prev=friend;
    friend=friend->next;
    free_memory(prev);
  }
}

void dfs(struct list_records* vertex, struct heap_elem* heap, int* index) {
    struct list_records* adj = vertex;
    while (adj != NULL) {
        if (adj->record->status==1){
          adj = adj->next;
          continue;
        }
        adj->record->distance = INFINITY;
        adj->record->pred = NULL;
        heap[*index].r = adj->record;
        adj->record->status = 1;
        (*index)++;
        dfs(adj->record->friends, heap, index);
        adj = adj->next;
    }
    return;
}

void initialize_source(struct record* r,int* index){
    r->distance = 0;
    r->pred = NULL;
    min_heap_arr[0].r=r;
    r->status=1;
    (*index)++;
    dfs(r->friends, min_heap_arr, index);
}

// Each friends f of r is connected via an undirected
// edge whose weight is the distance between r and f.
// compute the shortest distance and a shortest path to 
// all other vertices reachable from r.
// initially, the status fields in all records are set to zero.
// distance and pred fields may contain grabage values.
// you need to use the distance function to compute the distance
// between two records.
// you can use min_heap_arr to implement a min heap or
// a linear scan to find a vertex with the minimum distance.
// In addition to distance and pred, you can also update the
// status field in your implementation.
// Don't modify other fields in struct record.
// At the end of this procedure, distance field in each record
// reachable via r, must contain the minimum distance from r;
// and the pred field must contain the predecessor on a shorest
// path.
void compute_sssp(struct record *r)
{
    int index=0;
    initialize_source(r,&index);
    while (index != 0) {
        double min_dist = INFINITY;
        int min_index = 0;
        for (int i = 0; i < index; i++) {
            if (min_dist > min_heap_arr[i].r->distance) {
                min_index = i;
                min_dist = min_heap_arr[i].r->distance;
            }
        }
        struct record* min_record = min_heap_arr[min_index].r;
        for (int i = min_index; i < index - 1; i++) {
            min_heap_arr[i] = min_heap_arr[i + 1];
        }
        index--;
        struct list_records* adjacent = min_record->friends;
        while (adjacent != NULL) {
            double dist = distance(&(adjacent->record->loc), &(min_record->loc));
            if ((adjacent->record->distance) > (min_record->distance + dist)) {
                adjacent->record->distance = min_record->distance + dist;
                adjacent->record->pred = min_record;
            }
            adjacent = adjacent->next;
        }
    }
}