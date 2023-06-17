#include "../include/common.h"
#include "pa1.h"

// don't remove these globals
static struct record *record_arr = NULL;
static size_t max_capacity = 0;
static size_t num_records = 0;

// don't modify this function
struct record *get_record_arr()
{
  return record_arr;
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
// compare two records when uid is key
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

#define NOT_IMPLEMENTED { printf("%s still needs to be implemented!\n", __func__); abort(); }

// implicit declaration for sorting functions and searching functions
void swap(struct record* record_arr,struct record* low,struct record* high);
struct record* partition(struct record* record_arr, struct record* low, struct record* high,int (cmp)(struct record*, struct record*));
void quicksort(struct record* record_arr,struct record* low,struct record* high,int (cmp)(struct record*, struct record*));
int binary_search(struct record* record_arr,char uid[MAX_LEN],int low,int high);
void mergesort(struct record* record_arr,int low, int high);
void merge(struct record* record_arr,int low, int mid,int high);
void selection_sort(struct record* record_arr);
static int compare_by_name(struct record* record_arr,struct record* dummy);
int adapter_cmp_record(struct record* a, struct record* b);
int adapter_cmp_name(struct record* a, struct record* b);
int compare_word(char name1[],char name2[]);

// return the total number of records in the
// record_arr
size_t get_num_records()
{ 
	return num_records;
}

// insert record r at the end of the record array
// if the record array is full, allocate
// a new buffer of double size, copy data
// from old buffer to the new buffer
// delete the old buffer
// make record_arr point to the new buffer
void insert_record(struct record r)
{
  if ((int)num_records==0){
    record_arr=(struct record*)allocate_memory(1*sizeof(struct record));
    record_arr[(int)num_records]=r;
    num_records++;
    max_capacity++;
    return;
  }
  int length=max_capacity;
  if ((int)num_records<length){
    record_arr[(int)num_records]=r;
    num_records++;
  }else{
    struct record* record_arr1=(struct record*)allocate_memory(2*length*sizeof(struct record));
    for (int i=0;i<(int)num_records;i++){
      record_arr1[i]=record_arr[i];
    }
    record_arr1[(int)num_records]=r;
    num_records++;
    free_memory(record_arr);
    record_arr=record_arr1;
    max_capacity=2*length;
  }
}

// return the record corresponding to the uid
// use the linear search algorithm
// you can also use cmp_uid routine defined earlier
// if none of the records contain the input uid
// return a dummy record with status == -1
struct record search_record_linear(char uid[MAX_LEN])
{
	int idx=-1;
  for (int i=0;i<(int)num_records;i++){
    if (cmp_uid(record_arr[i].uid,uid)==0){
      idx=i;
      break;
    }
  }
  if (idx==-1){
    struct record dummy_record;
    dummy_record.status=-1;
    return dummy_record;
  }
  return record_arr[idx];
}

// return the record corresponding to the uid
// use the binary search algorithm
// you can assume that the record_arr is alaredy sorted
// if none of the records contain the input uid
// return a dummy record with status == -1
struct record search_record_binary(char uid[MAX_LEN])
{
	int idx=-1;
  int find=binary_search(record_arr,uid,0,(int)num_records-1);
  if (find==-1){
    struct record dummy_record;
    dummy_record.status=-1;
    return dummy_record;
  }
  return record_arr[find];
}

// delete record correspondig to the uid
// if the corresponding record doesn't exist
// return a dummy record with status == -1
// Otherwise, return a copy of the record to the caller

// resize the record array in the following manner
// let's say, x is the size of the array
// and after deletion only x/4 
// space in the array is occupied, in this
// case, allocate an array of size x/2,
// copy records from old array to the new
// array, and delete the old array,
// make record_arr point to the new array
struct record delete_record(char uid[MAX_LEN])
{
  int idx=-1;
  for (int i=0;i<(int)num_records;i++){
      if (cmp_uid(record_arr[i].uid,uid) == 0){
          idx = i;
          break;
      }
  }
  if (idx==-1){
      struct record dummy_record;
      dummy_record.status=-1;
      return dummy_record;
  }
  struct record deleted_record=record_arr[idx];
  num_records--;
  int length=max_capacity;
  for (int i=idx;i<num_records;i++){
      record_arr[i] = record_arr[i+1];
  }
  if ((int)num_records>0 && (int)num_records<=length/4){
      struct record* record_arr1 = (struct record*)allocate_memory(length/2*sizeof(struct record));
      for (int i=0;i<(int)num_records;i++){
          record_arr1[i]=record_arr[i];
      }
      free_memory(record_arr);
      record_arr=record_arr1;
      max_capacity=length/2;
  }
  return deleted_record;
}

// delete all records in the record_arr
// free record_arr
void delete_all_records()
{
  num_records=0;
  max_capacity=0;
  free_memory(record_arr);
}

// sort the record array using quick sort
// use cmp_record implementaion to compare two records
void sort_records_quick()
{
  quicksort(record_arr,&record_arr[0],&record_arr[(int)num_records-1],adapter_cmp_record);
}

// sort the record_arr using merge sort
// use cmp_record implementaion to compare two records
void sort_records_merge()
{
	mergesort(record_arr,0,(int)num_records-1);
}

// sort the record_arr using selection sort
// use cmp_record implementaion to compare two records
void sort_records_selection()
{
	selection_sort(record_arr);
}

// return the number of records corresponding to the input name
// use linear search to obtain the results
size_t get_num_records_with_name_linear(char name[MAX_LEN])
{
  size_t count=0;
  for (int i=0;i<(int)num_records;i++){
    int f=0;
    for (int j=0;record_arr[i].name[j]!='\0' && name[j]!='\0';j++){
      if (record_arr[i].name[j]>name[j]){
        f=1;
        break;
      }else if (record_arr[i].name[j]<name[j]){
        f=-1;
        break;
      }
    }
    if (f==0){
      count++;
    }
  }
  return count;
}

// implement quick sort algorithm to sort the 
// record_arr using name as key
void rearrange_data()
{
	quicksort(record_arr,&record_arr[0],&record_arr[(int)num_records-1],adapter_cmp_name);
}

// return the number of records corresponding to the input name
// use binary search to obtain the results
// you can assume that the caller has sorted
// data using name as key before making these queries
size_t get_num_records_with_name_binary(char name[MAX_LEN])
{ 
  int left = 0, right = (int)num_records - 1;
  int first_occurrence = -1;
  while (left <= right) {
    int mid = left + (right - left) / 2;
    if (compare_word(record_arr[mid].name,name)==0) {
      first_occurrence = mid;
      right = mid - 1;
    } else if (compare_word(record_arr[mid].name,name)==-1) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  if (first_occurrence == -1) {
    return 0;
  }
  int count = 1;
  int i = first_occurrence - 1;
  while (i >= 0 && (compare_word(record_arr[i].name,name)==0)) {
    count++;
    i--;
  }
  i = first_occurrence + 1;
  while (i < (int)num_records && (compare_word(record_arr[i].name,name)==0)) {
    count++;
    i++;
  }
  return count;
}

void quicksort(struct record* record_arr,struct record* low,struct record* high,int (cmp)(struct record*, struct record*)){
  if (low>=high){
    return;
  }
  struct record* p=partition(record_arr,low,high,cmp);
  quicksort(record_arr,low,p-1,cmp);
  quicksort(record_arr,p+1,high,cmp);
}

struct record* partition(struct record* record_arr, struct record* low, struct record* high,int (cmp)(struct record*, struct record*)){
  struct record* pivot = low;
  struct record* left = low + 1;
  struct record* right = high;
  while (left<=right){
    while (left<=right && (cmp(left,pivot)==-1)){
      left+=1;
    }
    while (right>=left && (cmp(right,pivot)==1)){
      right-=1;
    }
    if (left<=right){
      swap(record_arr, left, right);
      left++; 
      right--;
    }
  }
  swap(record_arr, low, right);
  return right;
}

void swap(struct record* record_arr,struct record* low,struct record* high){
  struct record temp=*low;
  *low=*high;
  *high=temp;
}

int binary_search(struct record* record_arr,char uid[MAX_LEN],int low,int high){
  if (high < low){
    return -1;
  }
  int mid = (low + high) / 2;
  if (cmp_uid(record_arr[mid].uid,uid)==0){
    return mid;
  }
  if (cmp_uid(record_arr[mid].uid,uid)==1){
    return binary_search(record_arr, uid, low, mid-1);
  }else{
    return binary_search(record_arr, uid, mid+1, high);
  }
}

void mergesort(struct record *record_arr, int low, int high) {
    if (low>=high){
      return;
    }
      int mid = (high+low) / 2;
      mergesort(record_arr, low, mid);
      mergesort(record_arr, mid + 1, high);
      merge(record_arr, low, mid, high);
}

void merge(struct record *record_arr, int low, int mid, int high) {
    int i, j, k;
    int n1 = mid - low + 1;
    struct record* tmp1=(struct record*)allocate_memory((n1+1)*sizeof(struct record));
    for (i = 0; i < n1; i++) {
        tmp1[i] = record_arr[low + i];
    }
    i = 0;
    j = mid + 1;
    k = low;
    while (i < n1 && j <= high) {
        if (cmp_record(tmp1 + i, record_arr + j) <= 0) {
            record_arr[k] = tmp1[i];
            k++;
            i++;
        } else {
            record_arr[k] = record_arr[j];
            k++;
            j++;
        }
    }
    while (i < n1) {
        record_arr[k] = tmp1[i];
        k++;
        i++;
    }
    free_memory(tmp1);
}

void selection_sort(struct record* record_arr){
  for (int i=0;i<(int)num_records-1;i++){
    int idx_min=i;
    for (int j=i+1;j<(int)num_records;j++){
      if (cmp_record(&record_arr[idx_min],&record_arr[j])==1){
        idx_min=j;
      }
    }
    swap(record_arr,&record_arr[idx_min],&record_arr[i]);
  }
}

int compare_by_name(struct record* record_arr,struct record* dummy){
  for (int j=0;j<(int)num_records;j++){
    if (record_arr->name[j] > dummy->name[j]) {
      return 1;
    }
    else if (record_arr->name[j] < dummy->name[j]) {
      return -1;
    }
  }
  return 0;
}

// Adapter function for compare_by_name
int adapter_cmp_name(struct record* a, struct record* b) {
    return compare_by_name(a, b);
}

// Adapter function for compare_by_uid
int adapter_cmp_record(struct record* a, struct record* b) {
    return cmp_record(a, b);
}

int compare_word(char name1[],char name2[]){
  for (int j=0;name1[j]!='\0' && name2[j]!='\0';j++){
    if (name1[j]>name2[j]){
      return 1;
    }else if (name1[j]<name2[j]){
      return -1;
    }
  }
  return 0;
}