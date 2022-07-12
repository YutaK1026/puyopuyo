#include "ex004_array.h"

#define SIZE 5
static int array[SIZE];

void setArray(int index, int value)
{
  /* 指定された配列インデックスが正しくないときは配列arrayへの代入を行わない */
  if(index < 0 || index >= SIZE){
    printf("Invalid index was specified.\n");
    return;
  }

  array[index] = value;
}

/*------------------------------------*/
// 関  数：inverseArray
// 引  数：なし
// 返り値：なし
// 機  能：配列arrayの要素を逆順に入れ替え
/*------------------------------------*/
void inverseArray()
{
  int i;
  int temp;

  for(i=0; i<SIZE/2; i++){
    temp = array[i];
    array[i] = array[SIZE-i-1];
    array[SIZE-i-1] = temp;
  }
}

/*------------------------------------*/
// 関  数：displayArray
// 引  数：なし
// 返り値：なし
// 機  能：配列arrayの要素を印字
/*------------------------------------*/
void displayArray()
{
  int i;

  for(i=0; i<SIZE; i++){
    printf("%d\n", array[i]);
  }
}