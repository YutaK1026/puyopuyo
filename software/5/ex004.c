#include <stdio.h>
#include "ex004_array.h"
#define SIZE 5

static int array[SIZE];

int main()
{
  /* arrayの要素を格納 */
  setArray(0, 1);
  setArray(1, 2);
  setArray(2, 3);
  setArray(3, 4);
  setArray(4, 5);

  /* arrayの要素を表示 */
  printf("before:\n");
  displayArray();

  /* arrayの要素を逆順に並べ替え */
  inverseArray();

  /* arrayの要素を表示 */
  printf("after:\n");
  displayArray();

  return 0;
}