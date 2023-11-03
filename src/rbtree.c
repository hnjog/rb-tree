#include "rbtree.h"

#include <stdlib.h>

void delete_Node(rbtree *t,node_t* n);
void rbtree_transplant(rbtree* t, node_t* u, node_t* v);
node_t* rbtree_find_succesor(rbtree* t, node_t* z);

rbtree *new_rbtree(void)
{
  // calloc : 할당된 데이터 내부 0으로 초기화
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t*)calloc(1,sizeof(node_t));
  p->root = p->nil;

  return p;
}

void delete_rbtree(rbtree *t)
{
  if(t == NULL){
    printf("delete_rbtree in NULL point");
    return;
  }
  
  node_t* node = t->root;
  delete_Node(t,node);

  free(t->nil);
  free(t);
}

void delete_Node(rbtree *t,node_t* n)
{
  if(t == NULL || n == NULL){
    printf("delete_Node in NULL point");
    return;
  }

  if(n == t->nil)
      return;

  if (n->left != t->nil)
  {
    delete_Node(t, n->left);
    n->left = NULL;
  }

  if (n->right != t->nil)
  {
    delete_Node(t, n->right);
    n->right = NULL;
  }

  free(n);
  n = NULL;
}

void rb_left_rotate(rbtree *t, node_t *z)
{
  if(t == NULL || z == NULL){
    printf("rb_left_rotate in NULL point");
    return;
  }

  node_t *y = z->right; // 우측 자식 노드
  z->right = y->left;   // 우측 노드의 왼쪽 노드로 오른쪽 자식을 한다

  // 만약 왼쪽 노드가 nil이 아니면
  // 해당 노드의 부모로 z를 가리키도록 설정
  if (y->left != t->nil)
  {
    y->left->parent = z;
  }

  // 부모를 z가 가리키는 부모로 설정
  y->parent = z->parent;

  // z가 root 인가?
  if (z->parent == t->nil)
  {
    t->root = y;
  }
  else if (z == z->parent->left)
  { // z가 부모의 왼쪽 자식인가?
    z->parent->left = y;
  }
  else
  { // z는 부모의 오른쪽 자식이였음
    z->parent->right = y;
  }

  y->left = z;
  z->parent = y;
}

void rb_right_rotate(rbtree *t, node_t *z)
{
  if(t == NULL || z == NULL){
    printf("rb_right_rotate in NULL point");
    return;
  }

  node_t *y = z->left; // 좌측 자식 노드
  z->left = y->right;  // 오른쪽 회전이므로 z는 y의 오른쪽 인자를 왼쪽 자식으로 만든다

  // nil 이 아니라면 부모로 설정해 준다
  if (y->right != t->nil)
  {
    y->right->parent = z;
  }

  y->parent = z->parent;

  if (z->parent == t->nil)
  {
    t->root = y;
  }
  else if (z == z->parent->left)
  {
    z->parent->left = y;
  }
  else
  {
    z->parent->right = y;
  }

  y->right = z;
  z->parent = y;
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  if(t == NULL){
    printf("rbtree_insert in NULL point");
    return NULL;
  }

  node_t *newNode = (node_t *)calloc(1, sizeof(node_t));
  newNode->key = key;

  node_t *targetNode = t->root;
  node_t *prevNode = t->nil;

  // 노드 삽입 위치 찾기
  while (targetNode != t->nil)
  {
    prevNode = targetNode;
    // bst 규칙으로
    // 작은 값이라면 왼쪽으로 아니라면 오른쪽으로
    if (key < targetNode->key)
    {
      targetNode = targetNode->left;
    }
    else
    {
      targetNode = targetNode->right;
    }
  }

  // 트리가 비어있었음
  if (prevNode == t->nil)
  {
    t->root = newNode;
  }
  else if (newNode->key < prevNode->key)
  { // 키 값이 부모 노드보다 작음
    prevNode->left = newNode;
  }
  else
  {
    prevNode->right = newNode;
  }

  newNode->left = t->nil;
  newNode->right = t->nil;
  newNode->color = RBTREE_RED;

  rb_insert_fixup(t, newNode);

  return t->root;
}

int rbtree_erase(rbtree *t, node_t *z)
{
  if(t == NULL || z == NULL){
    printf("rbtree_erase in NULL point");
    return -1;
  }

  node_t* target = z; // 삭제할 노드
  color_t targetOriginColor = target->color;
  node_t* v = t->nil; // 대체할 노드

  if(z->left == t->nil)
  {
    v = z->right;
    rbtree_transplant(t,z,v);
  }
  else if(z->right == t->nil)
  {
    v = z->left;
    rbtree_transplant(t,z,v);
  }
  else // 삭제 노드의 자식이 2개 있는 상황
  {
    // 삭제 노드 기준의 succesor 값을 찾는다
    target = rbtree_find_succesor(t,z);
    if (target == NULL){
      printf("rbtree_erase cant find succesor!");
      return -1;
    }

    // 삭제 노드가 아니라 succesor의 색
    targetOriginColor = target->color;
    v = target->right;
    if (target->parent == z){ // succesor의 부모가 삭제 노드라면
      v->parent = target; // v는 원래 삭제될 위치로 이동될 것이기에 부모 재설정
    }
    else{
      rbtree_transplant(t,target,v);
      target->right = z->right;
      target->right->parent = target;
    }
    rbtree_transplant(t,z,target);
    target->left = z->left;
    target->left->parent = target;
    target->color = z->color;
  }

  if (targetOriginColor == RBTREE_BLACK)
  {
    rb_erase_fixup(t,v);
  }

  // 노드 삭제되었으므로 해제
  free(z);
  z = NULL;

  return 0;
}

// 노드 이식하기
// u의 위치에 v 노드 이식
void rbtree_transplant(rbtree* t, node_t* u, node_t* v)
{
  if(t == NULL || u == NULL || v == NULL){
    printf("rbtree_transplant in NULL point");
    return;
  }

  if (u->parent == t->nil)
  {
    t->root = v;
  }
  else if(u == u->parent->left)
  {
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }

  v->parent = u->parent;
}

node_t* rbtree_find_succesor(rbtree* t, node_t* z)
{
  if(t == NULL || z == NULL){
    printf("rbtree_find_succesor in NULL point");
    return NULL;
  }

  if (z == t->nil || z->right == t->nil)
  {
    return t->nil;
  }

  node_t* temp = z->right; // 오른쪽 노드의
  while (temp->left != t->nil) // 가장 왼쪽값 찾기
  {
    temp = temp->left;
  }
  
  return temp;
}

void rb_insert_fixup(rbtree *t, node_t *z)
{
  // 속성 2,4 를 위반할 수 있으니 확인한다
  // 해당 내용은 모두 '부모가 red' 일 때 발생이 가능하다
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {                                               // 부모가 조부모의 왼쪽 자식
      node_t *uncleNode = z->parent->parent->right; // 삼촌 노드(조부모의 오른쪽 자식)
      if (uncleNode->color == RBTREE_RED)
      { // 삼촌 노드가 붉은 색
        // case 1 (부모와 삼촌 모두 red)
        z->parent->color = RBTREE_BLACK;
        uncleNode->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED; // 조부모의 색을 red로 바꿔준다
        z = z->parent->parent;          // 조부모 입장에서 다시 체크해야 하기에 z를 바꿔준다
      }
      else
      {
        if (z == z->parent->right)
        { // 현재 노드가 부모의 오른쪽 자식
          // case 2
          // 부모를 기준으로 왼쪽 회전 후 case 3를 적용
          z = z->parent;
          rb_left_rotate(t,z);
        }
        // case 3
        z->parent->color = RBTREE_BLACK; // 부모색을 검게 (부모와 조부모 색 교환)
        z->parent->parent->color = RBTREE_RED;  // 조부모 색을 붉게(부모가 붉다는 가정이며, 또한 4번 규칙)
        rb_right_rotate(t,z);
      }
    }
    else // 부모가 조부모의 오른쪽 자식이므로 위의 코드의 좌우를 변경해준다
    {
      node_t *uncleNode = z->parent->parent->left; // 삼촌 노드(조부모의 왼쪽 자식)
      if (uncleNode->color == RBTREE_RED)
      { // 삼촌 노드가 붉은 색
        // case 1 (부모와 삼촌 모두 red)
        z->parent->color = RBTREE_BLACK;
        uncleNode->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED; // 조부모의 색을 red로 바꿔준다
        z = z->parent->parent;          // 조부모 입장에서 다시 체크해야 하기에 z를 바꿔준다
      }
      else
      {
        if (z == z->parent->left)
        { // 현재 노드가 부모의 왼쪽 자식
          // case 2
          // 부모를 기준으로 오른쪽 회전 후 case 3를 적용
          z = z->parent;
          rb_right_rotate(t,z);
        }
        // case 3
        z->parent->color = RBTREE_BLACK; // 부모색을 검게 (부모와 조부모 색 교환)
        z->parent->parent->color = RBTREE_RED;  // 조부모 색을 붉게(부모가 붉다는 가정이며, 또한 4번 규칙)
        rb_left_rotate(t,z);
      }
    }
  }

  // root 의 색은 black 이여야 한다 (2번 규칙)
  t->root->color = RBTREE_BLACK;
}

void rb_erase_fixup(rbtree *t, node_t *z)
{
  if(t == NULL || z == NULL){
    printf("rb_erase_fixup in NULL point!");
    return;
  }

  node_t* temp = t->nil;

  while (z != t->root && z->color == RBTREE_BLACK)
  {
    if (z == z->parent->left)
    {
      temp = z->parent->right;
      if(temp->color == RBTREE_RED)
      {
        temp->color = RBTREE_BLACK;
        z->parent->color = RBTREE_RED;
        rb_left_rotate(t,z->parent);
        temp = z->parent->right;
      }

      if(temp->left->color == RBTREE_BLACK &&
      temp->right->color == RBTREE_BLACK)
      {
          temp->color = RBTREE_RED;
          z = z->parent;
      }
      else
      {
        if(temp->right->color == RBTREE_BLACK)
        {
          temp->left->color = RBTREE_BLACK;
          temp->color = RBTREE_RED;
          rb_right_rotate(t,temp);
          temp = z->parent->right;
        }

        temp->color = z->parent->color;
        z->parent->color = RBTREE_BLACK;
        temp->right->color = RBTREE_BLACK;
        rb_left_rotate(t,z->parent);
        z = t->root;
      }
    }
    else
    {
      temp = z->parent->left;
      if(temp->color == RBTREE_RED)
      {
        temp->color = RBTREE_BLACK;
        z->parent->color = RBTREE_RED;
        rb_right_rotate(t,z->parent);
        temp = z->parent->left;
      }

      if(temp->right->color == RBTREE_BLACK &&
      temp->left->color == RBTREE_BLACK)
      {
          temp->color = RBTREE_RED;
          z = z->parent;
      }
      else
      {
        if(temp->left->color == RBTREE_BLACK)
        {
          temp->right->color = RBTREE_BLACK;
          temp->color = RBTREE_RED;
          rb_left_rotate(t,temp);
          temp = z->parent->left;
        }

        temp->color = z->parent->color;
        z->parent->color = RBTREE_BLACK;
        temp->left->color = RBTREE_BLACK;
        rb_right_rotate(t,z->parent);
        z = t->root;
      }
    }
  }
  
  z->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  if(t == NULL)
  {
    printf("rbtree_find Function in NULL point");
    return NULL;
  }

  node_t* node = t->root;

  while (node != t->nil)
  {
    if(node->key == key)
    {
      break;
    }

    if(node->key > key) // 찾고 있는 키 값보다 크다
    {
      node = node->left;
    }
    else // 현재 노드값보다 키값이 더 크다
    {
      node = node->right;
    }
  }

  if (node == t->nil)
  {
    return NULL;
  }

  return node;
}

node_t *rbtree_min(const rbtree *t)
{
  if(t == NULL)
  {
    printf("rbtree_min Function in NULL point");
    return NULL;
  }

  node_t* node = t->root;
  while (node->left != t->nil)
  {
    node = node->left;
  }

  return node;
}

node_t *rbtree_max(const rbtree *t)
{
  if(t == NULL)
  {
    printf("rbtree_max Function in NULL point");
    return NULL;
  }

  node_t* node = t->root;
  while (node->right != t->nil)
  {
    node = node->right;
  }

  return node;
}

void rbtree_inOrder_insert(const rbtree *t,const size_t size,node_t* n, key_t* key, int* index)
{
  if(t == NULL || n == NULL || key == NULL || index == NULL)
  {
    printf("rbtree_inOrder_insert Function in NULL point");
    return;
  }

  if(n == t->nil || *index >= size)
  {
    return;
  }

  rbtree_inOrder_insert(t,size,n->left,key,index);

  key[*index] = n->key;
  *index +=1;

  rbtree_inOrder_insert(t,size,n->right,key,index);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  size_t topindex = 0;
  rbtree_inOrder_insert(t,n,t->root,arr,&topindex);
  
  return 0;
}
