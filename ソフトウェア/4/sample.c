/**************************************/
//  課題4
//  2022/06/27 学生番号 名前
//  ex004_sample.c
//  標準入力：なし
//  機    能：配列の要素を逆順に入れ替えて印字
/**************************************/
#include <stdio.h>

#define SIZE 5

void setArray(int index, int value);
void inverseArray();
void displayArray();

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

/*------------------------------------*/
// 関  数：setArray
// 引  数：配列arrayのインデックス index, 配列arrayに格納する値 value
// 返り値：なし
// 機  能：配列arrayのindex番目に値valueを格納する
/*------------------------------------*/
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