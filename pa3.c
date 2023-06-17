#include "../include/common.h"
#include "pa3.h"

// don't remove these globals
static struct trie_node *trie_root = NULL;
size_t num_msgs = 0;

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

void destroy_trie_nodes(struct trie_node* node);
struct trie_node* create_node(char val);
void destroy_history(struct list_events* history);

// return the list that contains the history of
// events on msg.
// return NULL if the msg is not present in the trie.
struct list_events* get_history(char msg[MAX_MSG_LEN])
{
  struct trie_node* current = trie_root;
  int i = 0;
  while (msg[i] != '\0') {
    int found_child = 0;
    struct trie_node* child = current->child;
    while (child != NULL) {
      if (child->val == msg[i]) {
        current = child;
        found_child = 1;
        break;
      }
      child = child->next;
    }
    if (found_child==0) {
      return NULL;
    }
    i++;
  }
  return current->history;
}

// remove all the delete and the corresponding 
// post events from the history of msg, before returning
// the head of the linked list that contains the history.
// return NULL if the msg is not present in the trie.
// For example, if the following events occur for the msg
// in the given order, where rightmost is the most recent event
// (u1, P), (u1, P), (u2, P), (u1, D), (u1, P), (u3, P), (u4, P), (u3, D), (u1, D)
// Here, u1, u2, u3 are users, P and D corresponds to post and delete
// events
// Then after the cleanup the sequence of events looks like
// (u1, P), (u2, P), (u4, P)
// Notice that the history list only contains the post events
// after the cleanup
// the order of the post events remains the same during the cleanup
struct list_events* get_clean_history(char msg[MAX_MSG_LEN])
{
  struct trie_node* current = trie_root;
  int i = 0;
  while (msg[i] != '\0') {
    int found_child = 0;
    struct trie_node* child = current->child;
    while (child != NULL) {
      if (child->val == msg[i]) {
        current = child;
        found_child = 1;
        break;
      }
      child = child->next;
    }
    if (found_child==0) {
      return NULL;
    }
    i++;
  }
  struct list_events* hist=current->history;
  struct list_events* tmp=hist;
  struct list_events* prev=NULL;
  int check=0;
  struct list_events* deleted=NULL;
  struct list_events* temp_prev=NULL;
  while (tmp != NULL) {
    if (check == 0 && tmp->action == DELETED) {
        deleted = tmp;
        check = 1;
        if (prev == NULL) {
          hist = hist->next;
          free_memory(tmp);
          tmp = hist;
        } else {
          prev->next = tmp->next;
          free_memory(tmp);
          tmp = prev->next;
        } 
        temp_prev = prev;
    } else if (check == 1 && tmp->action == POSTED && tmp->record == deleted->record) {
        if (prev == NULL) {
          hist = hist->next;
          free_memory(tmp);
          tmp = hist;
        } else {
          prev->next = tmp->next;
          free_memory(tmp);
          tmp = prev->next;
        }
        check = 0;
        prev = temp_prev;
        if (prev == NULL) {
          tmp = hist;
        } else {
          tmp = prev->next;
        }
    }else{
      prev = tmp;
      tmp = tmp->next;
    }
  }
  current->history=hist;
  return hist;
}

// the maximum length of the '\0' terminated message 
// stored in msg is MAX_MSG_LEN
// Insert the msg in the trie rooted at trie_root
// store the reference of the square node
// corresponding to the msg in the linked list r->posts,
// where r is "struct record*"
// A user can post a message several times, say k times, 
// in that case r->posts contain k entries corresponding to
// the message
// r->posts is a stack. The first node in the linked-list
// r->posts corresponds to the lasted post by the user
// A square node in trie also contains the history of post and delete
// events on the corresponding message. The history is stored in
// a linked list n->history, where n is "struct trie_node*"
// n->history is also a stack. The first node in the linked
// list corresponds to the most recent event. The linked list
// node is of type struct event the contains the type of event
// and a reference to the user's record that posted or deleted
// the message

struct trie_node* create_node(char val) {
    struct trie_node* node = (struct trie_node*)allocate_memory(sizeof(struct trie_node));
    node->val = val;
    node->child = NULL;
    node->next = NULL;
    node->parent = NULL;
    node->history = NULL;
    return node;
}

void post_a_msg(char msg[MAX_MSG_LEN], struct record *r)
{ 
    int i = 0;
    struct trie_node* par = trie_root;
    if (par == NULL) {
        trie_root = create_node('\0');
        par=trie_root;
    }
    while (msg[i] != '\0') {
        struct trie_node* start = par->child;
        while (start != NULL && start->val != msg[i]) {
            start = start->next;
        }
        if (start == NULL) {
            struct trie_node* inserted = create_node(msg[i]);
            inserted->parent = par;
            inserted->next = par->child;
            par->child = inserted;
            par = inserted;
        }else{
          par=start;
        }
        i++;
    }
    msg[i]='\0';
    struct list_posts* post = (struct list_posts*)allocate_memory(sizeof(struct list_posts));
    post->node = par;
    post->next = r->posts;
    r->posts = post;
    struct list_events* hist = (struct list_events*)allocate_memory(sizeof(struct list_events));
    hist->action = POSTED;
    hist->record = r;
    hist->next = par->history;
    par->history = hist;
}

// delete the latest post by a given user
// the first node in linked list r->posts corresponds to
// the latest post
// To delete the latest post the first node in the r->posts
// is removed
// In addition the history of the corresponding square node
// in the trie also gets updated
// a square node also contains the history of post and delete
// events on that message. The history is stored in
// a linked list n->history, where n is "struct trie_node*"
// n->history is also a stack. The first node in the linked
// list corresponds to the most recent event. The linked list
// node is of type struct event the contains the type of event
// and a reference to the user's record that posted or deleted
// the message
// return 1 if a message was deleted
// return 0 id there is nothing to delete
int delete_latest_post(struct record *r)
{
  if (r->posts == NULL) {
    return 0;
  }
  struct list_posts* head = r->posts;
  struct trie_node* square = head->node;
  struct list_events* hist = square->history;
  while (hist != NULL && hist->action != POSTED) {
    hist = hist->next;
  }
  if (hist == NULL) {
    return 0;
  }
  struct list_events* updated = allocate_memory(sizeof(struct list_events));
  updated->action = DELETED;
  updated->record=r;
  updated->next = square->history;
  square->history = updated;
  r->posts = head->next;
  free_memory(head);
  return 1;
}

void delete_all_posts(struct record *r)
{
  struct list_posts* post=r->posts;
  struct list_posts* prev=NULL;
  while (post!=NULL){
    prev=post;
    post=post->next;
    free_memory(prev);
  }
}

// store the latest post (null terminated) by the user (r) in msg
// return 0 if there is no post by r
// return 1 if msg contains the latest post
// "struct record" contains references to square nodes of
// trie in the linked list posts
// To facilitate the finding of corresponding message from 
// a trie node, a trie node contains a reference to its
// parent node, that is updated when a message is inserted
// in the trie
int read_latest_post(struct record *r, char msg[MAX_MSG_LEN])
{
  if (r->posts != NULL) {
    struct list_posts* latest=r->posts;
    struct trie_node* current=latest->node;
    int i=0;
    while (current->parent!=NULL && i<MAX_MSG_LEN-1){
      msg[i++]=current->val;
      current=current->parent;
    }
    msg[i]='\0';
    int left=0;
    int right=i-1;
    while (left<=right){
      char temp=msg[left];
      msg[left]=msg[right];
      msg[right]=temp;
      left++;
      right--;
    }
    return 1;
  }else{
    return 0;
  }
}

// Delete all the memory allocated for the trie and
// the linked list that contains the history
void destroy_trie() {
  destroy_trie_nodes(trie_root);
}

void destroy_trie_nodes(struct trie_node* node) {
  if (node == NULL) {
    return;
  }
  destroy_trie_nodes(node->next);
  destroy_trie_nodes(node->child);
  destroy_history(node->history);
  free_memory(node);
}

void destroy_history(struct list_events* history) {
  if (history == NULL) {
    return;
  }
  destroy_history(history->next);
  free_memory(history);
}