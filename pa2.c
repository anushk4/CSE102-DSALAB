#include "../include/common.h"
#include "pa2.h"


// don't remove these globals
static struct record *bst_root = NULL;
static struct record *avl_root = NULL;
static size_t num_bst_nodes = 0;
static size_t num_avl_nodes = 0;

// don't modify this function
struct record *get_bst_root()
{
  return bst_root;
}

// don't modify this function
struct record *get_avl_root()
{
  return avl_root;
}

// don't modify this function
// compare two uids
// return -1 if uid1 < uid2
// return 0 if uid1 == uid2
// return 1 if uid1 > uid2
static int cmp_uid(char *uid1, char *uid2)
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

// don't modify this function
// compare two records
// we assume that uid is unique and
// there can't be two records with the 
// same uid
// return -1 if record1 < record2
// return 0 if record1 == record2
// return 1 if record1 > record2
static int cmp_record(struct record *r1, struct record *r2)
{
  return cmp_uid(r1->uid, r2->uid);
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
void insert_list(struct list_records* tmp,struct record* person, struct record* friend);
struct record* search_record(char uid[MAX_LEN],struct record* root);
void destroy_all_bst(struct record* bst_root);
int get_balance(struct record *n);
void set_height(struct record *n);
struct record* LL(struct record *n);
struct record* RR(struct record *n);
struct record* LR(struct record *n);
struct record* RL(struct record *n);
struct record* try_rotate(struct record *n);
struct record* insert_avl(struct record *root, struct record r);
void destroy_all_avl(struct record* avl_root);
struct record *find_min(struct record *node);
void deleteNode(struct list_records** head,struct record* r);
struct record* delete_avl(struct record *root,struct record *r);
struct record* delete(struct record *root,struct record *r);

// Return the total number of records in the
// BST rooted at bst_root.
size_t get_num_bst_records()
{
  return num_bst_nodes;
}

// Return the total number of records in the
// AVL tree rooted at avl_root.
size_t get_num_avl_records()
{
  return num_avl_nodes;
}

// Insert record r in the BST rooted at bst_root.
void insert_record_bst(struct record r)
{
    if (bst_root == NULL) {
        bst_root = allocate_memory(sizeof(struct record));
        *bst_root = r;
        bst_root->left = NULL;
        bst_root->right = NULL;
        num_bst_nodes++;
        return;
    }
    struct record *cur = bst_root;
    struct record *parent = NULL;
    while (cur != NULL) {
        parent = cur;
        if (cmp_uid(r.uid, cur->uid) < 0) {
            cur = cur->left;
        } else if (cmp_uid(r.uid, cur->uid) > 0) {
            cur = cur->right;
        } else {
            return;
        }
    }
    struct record *new_node = allocate_memory(sizeof(struct record));
    *new_node = r;
    new_node->left = NULL;
    new_node->right = NULL;
    if (cmp_uid(r.uid, parent->uid) < 0) {
        parent->left = new_node;
    } else {
        parent->right = new_node;
    }
    num_bst_nodes++;
}

// Insert record r in the AVL tree rooted at avl_root.
void insert_record_avl(struct record r)
{
    avl_root=insert_avl(avl_root,r);
    num_avl_nodes++;
}

// Search the record corresponding to uid in the BST
// rooted at bst_root.
// If the record is not present, return a dummy record
// with -1 in the status field.
struct record search_record_bst(char uid[MAX_LEN])
{
    struct record* cur = bst_root;
    while (cur != NULL) {
        if (cmp_uid(uid, cur->uid) == 0) {
            return *cur;
        } else if (cmp_uid(uid, cur->uid) < 0) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    struct record dummy = { .status = -1 };
    return dummy;
}

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return the head of the linked list of friends
// of the user with uid uid in the BST rooted at bst_root.
// If the corresponding record doesn't exist, return NULL.
struct list_records *get_friends_list_bst(char uid[MAX_LEN])
{
    struct record* found=search_record(uid,bst_root);
    if (found==NULL){
        return NULL;
    }
    struct list_records* friend = found->friends;
    return friend;
}

// Delete the record (say n) corresponding to uid from the BST.
// Remove n from the lists of friends of other records
// and release the memory for the linked list nodes.
// Release memory for all the nodes in the list of friends of n.
// Return a copy of the value of the deleted node.
// If the node is not present, return a dummy record
// with -1 in the status field.
struct record delete_record_bst(char uid[MAX_LEN])
{
    struct record* found = search_record(uid, bst_root);
    if (found == NULL) {
        struct record dummy = { .status = -1 };
        return dummy;
    }
    struct list_records* tmp = found->friends;
    struct record deleted_value = *found;
    struct list_records* prev=NULL;
    while (tmp!=NULL){
        struct record* friend_record=tmp->record;
        deleteNode(&friend_record->friends,found);
        prev=tmp;
        tmp=tmp->next;
        free_memory(prev);
    }
    free_memory(found);
    bst_root=delete(bst_root,found);
    num_bst_nodes--;
    return deleted_value;
}

// Search the record corresponding to uid in the AVL tree
// rooted at avl_root.
// If the record is not present, return a dummy record 
// with -1 in the status field.
struct record search_record_avl(char uid[MAX_LEN])
{
  struct record* cur = avl_root;
    while (cur != NULL) {
        if (cmp_uid(uid, cur->uid) == 0) {
            return *cur;
        } else if (cmp_uid(uid, cur->uid) < 0) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    struct record dummy = { .status = -1 };
    return dummy;
}

// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// Return the head of the linked list of friends
// of the user with uid uid in the AVL tree rooted at avl_root.
// If the corresponding record doesn't exist, return NULL.
struct list_records *get_friends_list_avl(char uid[MAX_LEN])
{
    struct record* found=search_record(uid,avl_root);
    struct list_records* friend = found->friends;
    return friend;
}

// Delete the record (say n) corresponding to uid from the AVL tree.
// Remove n from the lists of friends of other records
// and release the memory for the linked list nodes.
// Release memory for all the nodes in the list of friends of n.
// Return a copy of the value of the deleted node.
// If the node is not present, return a dummy record
// with -1 in the status field.
struct record delete_record_avl(char uid[MAX_LEN])
{
    struct record* found = search_record(uid, avl_root);
    if (found == NULL) {
        struct record dummy = { .status = -1 };
        return dummy;
    }
    struct list_records* tmp = found->friends;
    struct record deleted_value = *found;
    struct list_records* prev=NULL;
    while (tmp!=NULL){
        struct record* friend_record=tmp->record;
        deleteNode(&friend_record->friends,found);
        prev=tmp;
        tmp=tmp->next;
        free_memory(prev);
    }
    avl_root=delete_avl(avl_root,found);
    free_memory(found);
    num_avl_nodes--;
    return deleted_value;
}

// Release memory for all BST nodes and their lists of friends.
// Make bst_root points to an empty tree.
void destroy_bst()
{
  destroy_all_bst(bst_root);
  num_bst_nodes=0;
}

// Release memory for all AVL nodes and their lists of friends.
// Make avl_root points to an empty tree.
void destroy_avl()
{
  destroy_all_avl(avl_root);
  num_avl_nodes=0;
}

// Make users with uids uid1 and uid2 in the BST rooted at bst_root
// friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// To make the user with record A a friend of the user with record B, 
// add A to B's list of friends and add B to A's list of friends.
// Return 1 if uid1 and uid2 are already friends before this call.
// Return 0 if they become friends during this call.
int make_friends_bst(char uid1[MAX_LEN], char uid2[MAX_LEN])
{
  struct record* friend1=search_record(uid1,bst_root);
  struct record* friend2=search_record(uid2,bst_root);
  struct list_records* tmp1=friend1->friends;
  struct list_records* tmp2=friend2->friends;
  if (search_friend(tmp1,uid2)==0 || search_friend(tmp2,uid1)==0){
    insert_list(tmp1,friend1,friend2);
    insert_list(tmp2,friend2,friend1);
    return 0;
  }else{
    return 1;
  }
}

// Make users with uids uid1 and uid2 in the AVL tree rooted at avl_root
// friends of each other if they aren't already friends.
// The friends field in "struct record" stores the
// head of the linked list of friends of a given user.
// To make the user with record A a friend of the user with record B, 
// add A to B's list of friends and add B to A's list of friends.
// Return 1 if uid1 and uid2 are already friends before this call.
// Return 0 if they become friends during this call.
int make_friends_avl(char uid1[MAX_LEN], char uid2[MAX_LEN])
{
  struct record* friend1=search_record(uid1,avl_root);
  struct record* friend2=search_record(uid2,avl_root);
  struct list_records* tmp1=friend1->friends;
  struct list_records* tmp2=friend2->friends;
  if (search_friend(tmp1,uid2)==0 || search_friend(tmp2,uid1)==0){
    insert_list(tmp1,friend1,friend2);
    insert_list(tmp2,friend2,friend1);
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

void insert_list(struct list_records* tmp,struct record* person,struct record* friend){
    struct list_records* new_node = (struct list_records*) allocate_memory(sizeof(struct list_records));
    new_node->record = friend;
    new_node->next = NULL;
    struct list_records* tmp_head=tmp;
    while (tmp_head!=NULL){
        if (cmp_record(tmp_head->record,friend)==0){
            return;
        }
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

struct record* search_record(char uid[MAX_LEN],struct record* root)
{
    struct record* cur = root;
    while (cur != NULL) {
        if (cmp_uid(uid, cur->uid) == 0) {
            return cur;
        } else if (cmp_uid(uid, cur->uid) < 0) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    return NULL;
}

void destroy_all_bst(struct record* bst_root) {
    if (bst_root == NULL) {
        return;
    }
    destroy_all_bst(bst_root->left);
    destroy_all_bst(bst_root->right);
    struct list_records* friend = bst_root->friends;
    free_memory(bst_root);
    while (friend != NULL) {
        struct list_records* tmp = friend->next;
        free_memory(friend);
        friend = tmp;
    }
}

int get_balance(struct record *n) {
    int l_height = (n->left) ? n->left->height : -1;
    int r_height = (n->right) ? n->right->height : -1;
    return l_height - r_height;
}

void set_height(struct record *n) {
    int l_height = (n->left) ? n->left->height : -1;
    int r_height = (n->right) ? n->right->height : -1;
    n->height = (l_height > r_height) ? l_height + 1: r_height + 1;
}

struct record* try_rotate(struct record *n) {
    int bal = get_balance(n);
    if (bal < -1) {
        bal = get_balance(n->right);
        if (bal > 0) {
            n = RL(n);
        }else {
            n = RR(n);
        }
    }else if (bal > 1) {
        bal = get_balance(n->left);
        if (bal < 0) {
            n = LR(n);
        }
        else {
        n = LL(n);
      }
    }
    return n;
}

struct record* LL(struct record *n) {
    struct record *new_root = n->left;
    n->left = new_root->right;
    new_root->right = n;
    set_height(n);
    set_height(new_root);
    return new_root;
}

struct record* RR(struct record *n) {
    struct record *new_root = n->right;
    n->right = new_root->left;
    new_root->left = n;
    set_height(n);
    set_height(new_root);
    return new_root;
}

struct record* LR(struct record *n) {
    struct record *mid = n->left;
    struct record *new_root = mid->right;
    mid->right = new_root->left;
    n->left = new_root->right;
    new_root->left = mid;
    new_root->right = n;
    set_height(n);
    set_height(mid);
    set_height(new_root);
    return new_root;
}

struct record* RL(struct record *n) {
    struct record *mid = n->right;
    struct record *new_root = mid->left;
    mid->left = new_root->right;
    n->right = new_root->left;
    new_root->left = n;
    new_root->right = mid;
    set_height(n);
    set_height(mid);
    set_height(new_root);
    return new_root;
}

struct record* insert_avl(struct record *root, struct record r) {
    struct record *n;
    if (root == NULL) {
        root = allocate_memory(sizeof(struct record));
        *root = r;
        root->left = NULL;
        root->right = NULL;
        return root;
    }
    if (cmp_uid(r.uid , root->uid)>=0) {
        root->right = insert_avl(root->right, r);
    }else{
        root->left = insert_avl(root->left, r);
    }
    set_height(root);
    n = try_rotate(root);
    return n;
}

void destroy_all_avl(struct record* avl_root) {
    if (avl_root == NULL) {
        return;
    }
    destroy_all_avl(avl_root->left);
    destroy_all_avl(avl_root->right);
    struct list_records* friend = avl_root->friends;
    free_memory(avl_root);
    while (friend != NULL) {
        struct list_records* tmp = friend->next;
        free_memory(friend);
        friend = tmp;
    }
}

struct record* delete(struct record *root,struct record *r){
    if (root == NULL) {
      return NULL;
    }
    if (cmp_uid(root->uid,r->uid)==0) {
        if (root->left == NULL) {
            struct record *ret = root->right;
            return ret;
        }
        else if (root->right == NULL) {
            struct record *ret = root->left;
            return ret;
        }
        else if (root->right != NULL && root->left != NULL) {
            struct record *min_node = find_min(root->right); 
            root->right = delete(root->right, min_node);
            min_node->left=root->left;
            min_node->right=root->right;
            return min_node;
        }
    }
    else if (cmp_uid(r->uid,root->uid)>0) {
        root->right = delete(root->right, r);
    }
    else {
        root->left = delete(root->left, r);
    }
    return root;
}

struct record *find_min(struct record *node) {
    struct record *current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

void deleteNode(struct list_records** head,struct record* r)
{
    struct list_records* temp = *head;
    struct list_records* prev=NULL;
    if (temp != NULL && cmp_record(temp->record,r)==0) {
        *head = temp->next;
        free_memory(temp);
        return;
    }
    while (temp != NULL && cmp_record(temp->record,r)!=0) {
        prev = temp;
        temp = temp->next;
    }
    if (temp != NULL){
        prev->next = temp->next;
        free_memory(temp);
    }
}

struct record* delete_avl(struct record *root,struct record *r){
    if (root == NULL) {
      return NULL;
    }
    if (cmp_uid(root->uid,r->uid)==0) {
        if (root->left == NULL) {
            struct record *ret = root->right;
            return ret;
        }
        else if (root->right == NULL) {
            struct record *ret = root->left;
            return ret;
        }
        else if (root->right != NULL && root->left != NULL) {
            struct record *min_node = find_min(root->right); 
            root->right = delete_avl(root->right, min_node);
            min_node->left=root->left;
            min_node->right=root->right;
            set_height(min_node);
            return try_rotate(min_node);
        }
    }
    else if (cmp_uid(r->uid,root->uid)>0) {
        root->right = delete_avl(root->right, r);
    }
    else {
        root->left = delete_avl(root->left, r);
    }
    set_height(root);
    return try_rotate(root);
}