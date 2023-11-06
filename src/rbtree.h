#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>
#include<stdio.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

// 삽입시 기본적으로는 red 노드를 삽입
// 5번 조건 만족을 위하여
// 삽입 후에도 경로 상의 black 개수가 변하지 않기 위하여

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

#endif  // _RBTREE_H_

/*
  RB Tree 조건
  1. 모든 노드는 red or black
  2. 루트는 Black
  3. 모든 리프(NIL)은 black
  4. 노드가 적색이면 그 노드의 자식은 모두 흑색이다
  5. 각 노드로부터 그 노드의 자손인 리프로 가는 경로들은
     모두 같은 수의 black 노드를 포함한다

  사용 이유
  이진 탐색 트리의 최악을 방지
  -> 한쪽으로 트리가 치우치는 것을 방지 가능

  avg 트리와의 비교하자면,
  양측 다 삽입 삭제 탐색 모두 O(log N)
  다만 red - black이 avg보단 삽입/삭제가 빠름
  avg가 rb Tree 보다 탐색 은 빠르다

  보통 rb tree는
  c++ std::map 등에 사용

  avl은 dictionary,
  혹은 주로 삽입/삭제는 거의 없고,
  검색을 자주하는 경우에 사용

  사실 코드 내부에서 RB-tree의
  while을 통하여 '깨지는 지'를 검사한 후
  즉시 교정(fix-up)하는 듯하다
  (일일이 검사한다기 보다는 '논리적인 case'만 검사)

  참고로 5번 속성 만족하며,
  두 자녀가 같은 색을 가진다면
  부모와 두 자녀의 색을 바꿔줘도 속성은 여전히 만족함

  삽입

  bst의 특징을 지녀야 하기에
  노드의 왼쪽 자식에는 '작은 값',
  오른쪽 자식에는 '큰 값'이 들어가야 한다
  => '회전'을 사용해야 함
  회전의 기준을 판단하고 '색깔'을 바꿔줌
  이후 회전

  case 3
  삽입된 red 노드가 부모의 왼쪽(오른쪽) 자녀
  & 부모도 red
  & 부모의 형제(왼쪽(오른쪽))는 black이라면
   : 부모와 할아버지의 색을 바꾸고,
     할아버지 기준으로 오른쪽(왼쪽) 회전

  case 2
  삽입된 red 노드가 부모의 오른쪽(왼쪽) 자녀
  & 부모도 red
  & 부모의 형제(왼쪽(오른쪽))는 black이라면
  부모를 기준으로 왼쪽 회전 후
  case3 적용

  case 1
  삽입된 red 노드가 존재하지만 부모와 부모의 형제 모두 red 인 경우,
  부모와 형제를 black으로 바꿔주고,
  할아버지를 red로 바꿔준다
  (이때, 할아버지가 루트라면 다시 black으로)

  이런식으로 절차를 완료후
  다른 규약에 걸리는지 다시 체크해야 한다

  삭제
  삭제 자체는 bst와 동일,
  삭제 후, rb 트리 속성 확인 후
  위반 시 재조정

  속성 위반 여부 확인하기
  (어떠한 색이 삭제 되었는가)

  삭제하려는 노드의 '자녀'가 없거나 하나라면,
  삭제 색 = 삭제되는 노드의 색

  자녀가 둘이라면,
  succesor(삭제되는 노드 다음으로 유사한 값을 가지는 노드,BST)
  (ex : 해당 위치에 넣어도 균형이 유지되는 노드값)
  (루트 노드 삭제 시, 오른쪽 자식의 가장 왼쪽 후손을 찾음)

  bst 삭제 방식과 유사하며,
  이에 따라, 기존 값에 덮어씌운뒤,
  succesor를 삭제하기

  어찌되었든 '삭제되는 색'이 달라지므로 그부분에 유의하기

  삭제되는 색이 red 라면,
  rb tree 속성을 위반하지 않음

  black이 삭제되면,
  2,4,5가 위반될 수 있음

  extra black : 삭제 후, 임시로 black의 수를 하나 늘려주는 방식
  doubly black : black에 extra black이 부여되어 2개를 늘려줌
  red and black : red에 extra black이 부여된 상태

  -> extra black을 어떻게 해결?

  red and black : black으로 바꾸어 줌 (간단)
  doubly black
  : 4가지 case 존재 (기준 : 형제의 색과 형제의 자녀의 색)

  1. 오른쪽 형제가 black, 형제의 오른쪽 자녀가 red
  : red를 doubly black 위로 옮기고, 옮긴 red로
  extra black을 옮겨,
  red and black 상태를 만듦
  => red and black을 black으로 만들어 해결

  두 자녀에게 extra black이 있는 경우,
  부모에 하나의 extra black을 두는것으로 해결 가능

  오른쪽(왼쪽) 형제는 부모의 색으로,
  오른쪽(왼쪽) 형제의 오른쪽(왼쪽) 자녀는 black으로,
  부모는 black으로 바꾼 뒤
  부모를 기준으로 왼쪽(오른쪽)으로 해결

  2. 오른쪽 형제가 black,
  형제의 왼쪽 자녀가 red,
  오른쪽 자녀가 black일 때,
  오른쪽 자녀가 red가 되게 만들고,
  case 4를 적용하여 해결하기


*/