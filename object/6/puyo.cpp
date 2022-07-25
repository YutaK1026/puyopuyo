//課題
//2020/04/17

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//ぷよの色を表すの列挙型
// //NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };


//クラスの定義
class PuyoArray
{
protected:
  //盤面状態
  puyocolor *data;
  unsigned int data_line;
  unsigned int data_column;

  //メモリ開放
  void Release()
  {
    if (data == NULL) {
      return;
    }
    
    delete[] data;
    data = NULL;
  }
  
public:
  PuyoArray()
  {
    data = NULL;
    data_line = 0;
    data_column = 0;
  }
  ~PuyoArray()
  {
    Release();
  }
  
  //盤面の行数を返す
  unsigned int GetLine()
  {
    return data_line;
  }
  
  //盤面の列数を返す
  unsigned int GetColumn()
  {
    return data_column;
  }
  
  //盤面サイズ変更
  void ChangeSize(unsigned int line, unsigned int column)
  {
    Release();
    
    //新しいサイズでメモリ確保
    data = new puyocolor[line*column];

    data_line = line;
    data_column = column;
  }
  //盤面の指定された位置の値を返す
  puyocolor GetValue(unsigned int y, unsigned int x)
  {
    if (y >= GetLine() || x >= GetColumn())
    {
      //引数の値が正しくない
      return NONE;
    }

    return data[y*GetColumn() + x];
  }
  
  //盤面の指定された位置に値を書き込む
  void SetValue(unsigned int y, unsigned int x, puyocolor value)
  {
    if (y >= GetLine() || x >= GetColumn())
      {
	//引数の値が正しくない
	return;
      }

    data[y*GetColumn() + x] = value;
  }
};


//落下中ぷよを管理するクラス
class PuyoArrayActive : public PuyoArray
{
private:
  int puyorotate;
public:
  PuyoArrayActive()
  {
    puyorotate = 0;
  }
  int GetRotate()
  {
    return puyorotate;
  }
  void SetRotate(int z)
  {
    puyorotate = z;
  }
};

//着地済みぷよを管理するクラス
class PuyoArrayStack : public PuyoArray
{
};


//クラスの定義
class PuyoControl
{
public:
    //盤面に新しいぷよ生成
  void GeneratePuyo(PuyoArrayActive& puyo)
  {
    // 現在時刻の情報で乱数を初期化
    srand((unsigned int)time(NULL));

    // 1から4までの乱数を発生
    int num1 = rand() % 4 + 1;
    int num2 = rand() % 4 + 1;
    
    //Rotateを0に設定
    puyo.SetRotate(0);

    puyocolor newpuyo1;
    puyocolor newpuyo2;

    // num1とnum2によってnewpuyo1とnewpuyo2をそれぞれ決定
    switch (num1)
      {
      case RED:
	{
	  newpuyo1 = RED;
	  break;
	}
      case BLUE:
	{
	  newpuyo1 = BLUE;
	  break;
	}
      case GREEN:
	{
	  newpuyo1 = GREEN;
	  break;
	}
      case YELLOW:
	{
	  newpuyo1 = YELLOW;
	  break;
	}
      }
	
    switch (num2)
      {
      case RED:
	{
	  newpuyo2 = RED;
	  break;
	}
      case BLUE:
	{
	  newpuyo2 = BLUE;
	  break;
	}
      case GREEN:
	{
	  newpuyo2 = GREEN;
	  break;
	}
      case YELLOW:
	{
	  newpuyo2 = YELLOW;
	  break;
	}
      }
    
    
    puyo.SetValue(0, 5, newpuyo1);
    puyo.SetValue(0, 6, newpuyo2);
  }
  //ぷよの着地判定．着地判定があるとtrueを返す
  bool LandingPuyo(PuyoArrayActive& puyo, PuyoArrayStack& stack)
  {
    bool landed = false;
	//ぷよが動いているかのフラグを追加
    bool activeFlag = false;
    for (int y = 0; y < puyo.GetLine(); y++)
      {
	for (int x = 0; x < puyo.GetColumn(); x++)
	  {
	    if (puyo.GetValue(y, x) != NONE && (y == puyo.GetLine() - 1 || stack.GetValue(y+1, x) != NONE))
	    {
		//stackの方に積んだぷよの情報をコピー
		stack.SetValue(y, x, puyo.GetValue(y, x));
		//積みあがったぷよはactiveの盤面から削除（activeには動いているぷよしか残さない）
		puyo.SetValue(y, x, NONE);
		landed = true;

		//stackしたぷよの周りにもactiveぷよがあった時の処理
		if (puyo.GetValue(y-1, x) != NONE) {
			stack.SetValue(y-1, x, puyo.GetValue(y-1, x));
			puyo.SetValue(y-1, x, NONE);
			landed = true;
			activeFlag = false;
		} else if (puyo.GetValue(y, x-1) != NONE) {
			stack.SetValue(y, x-1, puyo.GetValue(y, x-1));
			puyo.SetValue(y, x-1, NONE);
			landed = true;
			activeFlag = false;
		} else if (puyo.GetValue(y, x+1) != NONE) {
			stack.SetValue(y, x+1, puyo.GetValue(y, x+1));
			puyo.SetValue(y, x+1, NONE);
			landed = true;
			activeFlag = false;
		}
 	    } else if (puyo.GetValue(y, x) != NONE && stack.GetValue(y+1, x) == NONE)
	    {
	    landed = false;
		//動いているぷよがある時の処理
		activeFlag = true;
	    }
	  }
    }
	//もし一つでも動いているぷよがあればfalseを返す（ganeratePuyoを発火させないため）
    if (activeFlag)
    {
		return false;
    }
    return landed;
  }
  void stackDown(PuyoArrayStack& stack)
  {
    //一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[stack.GetLine()*stack.GetColumn()];

	for (int i = 0; i < stack.GetLine()*stack.GetColumn(); i++)
	  {
	    puyo_temp[i] = NONE;
	  }
	
	//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = stack.GetLine() - 1; y >= 0; y--)
	  {
	    for (int x = 0; x < stack.GetColumn(); x++)
	      {
		if (stack.GetValue(y, x) == NONE) {
		  continue;
		}
		
		if (y < stack.GetLine() - 1 && stack.GetValue(y + 1, x) == NONE)
		  {
		    puyo_temp[(y + 1)*stack.GetColumn() + x] = stack.GetValue(y, x);
		    //コピー後に元位置のpuyoactiveのデータは消す
		    stack.SetValue(y, x, NONE);
		  }
		else
		  {
		    puyo_temp[y*stack.GetColumn() + x] = stack.GetValue(y, x);
		  }
	      }
	  }

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y < stack.GetLine(); y++)
	  {
	    for (int x = 0; x < stack.GetColumn(); x++)
	      {
		stack.SetValue(y, x, puyo_temp[y*stack.GetColumn() + x]);
	      }
	  }

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
  }

  //左移動
  void MoveLeft(PuyoArrayActive& puyo, PuyoArrayStack& stack)
  {
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];
    
    for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
      {
	puyo_temp[i] = NONE;
      }
    
    //1つ左の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = 0; y < puyo.GetLine(); y++)
      {
		for (int x = 0; x < puyo.GetColumn(); x++)
		  {
			//stackの盤面に積みあがったぷよがあればそれは動かさない
			
		    if (stack.GetValue(y, x) != NONE ) {
		      puyo_temp[y*puyo.GetColumn() + x ] = NONE;
		      continue;
		    }
			
		    if (puyo.GetValue(y, x) == NONE) {
		      continue;
		    }


		    if (0 < x && stack.GetValue(y, x - 1) == NONE && puyo.GetValue(y, x - 1) == NONE)
		      {
			puyo_temp[y*puyo.GetColumn() + (x - 1)] = puyo.GetValue(y, x);
			//コピー後に元位置のpuyoactiveのデータは消す
			puyo.SetValue(y, x, NONE);
		      }
		    else
		      {
			puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
		      }
		  }
      }
    
    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y < puyo.GetLine(); y++)
      {
	for (int x = 0; x < puyo.GetColumn(); x++)
	  {
	    puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
	  }
      }
    
    //一時的格納場所メモリ解放
    delete[] puyo_temp;
  }

  //右移動
  void MoveRight(PuyoArrayActive& puyo, PuyoArrayStack& stack)
  {
    //一時的格納場所メモリ確保
    puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];
    
    for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
      {
	puyo_temp[i] = NONE;
      }
    
    //1つ右の位置にpuyoactiveからpuyo_tempへとコピー
    for (int y = 0; y < puyo.GetLine(); y++)
      {
	for (int x = puyo.GetColumn() - 1; x >= 0; x--)
	  {
		//stackの盤面に積みあがったぷよがあればそれは動かさない
		
	    if (stack.GetValue(y, x) != NONE) {
	      puyo_temp[y*puyo.GetColumn() + x] = NONE;
	      continue;
	    }
	    
	    if (puyo.GetValue(y, x) == NONE) {
	      continue;
	    }
	    
	    if (x < puyo.GetColumn() - 1 && stack.GetValue(y, x + 1) == NONE && puyo.GetValue(y, x + 1) == NONE)
	      {
		puyo_temp[y*puyo.GetColumn() + (x + 1)] = puyo.GetValue(y, x);
		//コピー後に元位置のpuyoactiveのデータは消す
		puyo.SetValue(y, x, NONE);
	      }
	    else
	      {
		puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
	      }
	  }
      }
    
    //puyo_tempからpuyoactiveへコピー
    for (int y = 0; y <puyo.GetLine(); y++)
      {
	for (int x = 0; x <puyo.GetColumn(); x++)
	  {
	    puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
	  }
      }
    
    //一時的格納場所メモリ解放
    delete[] puyo_temp;
  }

  //下移動
  void MoveDown(PuyoArrayActive& puyo)
  {
    //一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[puyo.GetLine()*puyo.GetColumn()];

	for (int i = 0; i < puyo.GetLine()*puyo.GetColumn(); i++)
	  {
	    puyo_temp[i] = NONE;
	  }
	
	//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = puyo.GetLine() - 1; y >= 0; y--)
	  {
	    for (int x = 0; x < puyo.GetColumn(); x++)
	      {
		if (puyo.GetValue(y, x) == NONE) {
		  continue;
		}
		
		if (y < puyo.GetLine() - 1 && puyo.GetValue(y + 1, x) == NONE)
		  {
		    puyo_temp[(y + 1)*puyo.GetColumn() + x] = puyo.GetValue(y, x);
		    //コピー後に元位置のpuyoactiveのデータは消す
		    puyo.SetValue(y, x, NONE);
		  }
		else
		  {
		    puyo_temp[y*puyo.GetColumn() + x] = puyo.GetValue(y, x);
		  }
	      }
	  }

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y < puyo.GetLine(); y++)
	  {
	    for (int x = 0; x < puyo.GetColumn(); x++)
	      {
		puyo.SetValue(y, x, puyo_temp[y*puyo.GetColumn() + x]);
	      }
	  }

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
  }
        //ぷよ消滅処理を全座標で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack)
	{
		int vanishednumber = 0;
		for (int y = 0; y < puyostack.GetLine(); y++)
		{
			for (int x = 0; x < puyostack.GetColumn(); x++)
			{
				vanishednumber += VanishPuyo(puyostack, y, x);
			}
		}

		return vanishednumber;
	}

	//ぷよ消滅処理を座標(x,y)で行う
	//消滅したぷよの数を返す
	int VanishPuyo(PuyoArrayStack &puyostack, unsigned int y, unsigned int x)
	{
		//判定個所にぷよがなければ処理終了
		if (puyostack.GetValue(y, x) == NONE)
		{
			return 0;
		}


		//判定状態を表す列挙型
		//NOCHECK判定未実施，CHECKINGが判定対象，CHECKEDが判定済み
		enum checkstate{ NOCHECK, CHECKING, CHECKED };

		//判定結果格納用の配列
		enum checkstate *field_array_check;
		field_array_check = new enum checkstate[puyostack.GetLine()*puyostack.GetColumn()];

		//配列初期化
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			field_array_check[i] = NOCHECK;
		}

		//座標(x,y)を判定対象にする
		field_array_check[y*puyostack.GetColumn() + x] = CHECKING;

		//判定対象が1つもなくなるまで，判定対象の上下左右に同じ色のぷよがあるか確認し，あれば新たな判定対象にする
		bool checkagain = true;
		while (checkagain)
		{
			checkagain = false;

			for (int yy = 0; yy < puyostack.GetLine(); yy++)
			{
				for (int xx = 0; xx < puyostack.GetColumn(); xx++)
				{
					//(xx,yy)に判定対象がある場合
					if (field_array_check[yy*puyostack.GetColumn() + xx] == CHECKING)
					{
						//(xx+1,yy)の判定
						if (xx < puyostack.GetColumn() - 1)
						{
							//(xx+1,yy)と(xx,yy)のぷよの色が同じで，(xx+1,yy)のぷよが判定未実施か確認
							if (puyostack.GetValue(yy, xx + 1) == puyostack.GetValue(yy, xx) && field_array_check[yy*puyostack.GetColumn() + (xx + 1)] == NOCHECK)
							{
								//(xx+1,yy)を判定対象にする
								field_array_check[yy*puyostack.GetColumn() + (xx + 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(xx-1,yy)の判定
						if (xx > 0)
						{
							if (puyostack.GetValue(yy, xx - 1) == puyostack.GetValue(yy, xx) && field_array_check[yy*puyostack.GetColumn() + (xx - 1)] == NOCHECK)
							{
								field_array_check[yy*puyostack.GetColumn() + (xx - 1)] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy+1)の判定
						if (yy < puyostack.GetLine() - 1)
						{
							if (puyostack.GetValue(yy + 1, xx) == puyostack.GetValue(yy, xx) && field_array_check[(yy + 1)*puyostack.GetColumn() + xx] == NOCHECK)
							{
								field_array_check[(yy + 1)*puyostack.GetColumn() + xx] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy-1)の判定
						if (yy > 0)
						{
							if (puyostack.GetValue(yy - 1, xx) == puyostack.GetValue(yy, xx) && field_array_check[(yy - 1)*puyostack.GetColumn() + xx] == NOCHECK)
							{
								field_array_check[(yy - 1)*puyostack.GetColumn() + xx] = CHECKING;
								checkagain = true;
							}
						}

						//(xx,yy)を判定済みにする
						field_array_check[yy*puyostack.GetColumn() + xx] = CHECKED;
					}
				}
			}
		}

		//判定済みの数をカウント
		int puyocount = 0;
		for (int i = 0; i < puyostack.GetLine()*puyostack.GetColumn(); i++)
		{
			if (field_array_check[i] == CHECKED)
			{
				puyocount++;
			}
		}

		//4個以上あれば，判定済み座標のぷよを消す
		int vanishednumber = 0;
		if (4 <= puyocount)
		{
			for (int yy = 0; yy < puyostack.GetLine(); yy++)
			{
				for (int xx = 0; xx < puyostack.GetColumn(); xx++)
				{
					if (field_array_check[yy*puyostack.GetColumn() + xx] == CHECKED)
					{
						puyostack.SetValue(yy, xx, NONE);

						vanishednumber++;
					}
				}
			}
		}

		//メモリ解放
		delete[] field_array_check;

		return vanishednumber;
	}
  	//回転
	void Rotate(PuyoArrayActive &puyoactive)
	{
		//フィールドをラスタ順に探索（最も上の行を左から右方向へチェックして，次に一つ下の行を左から右方向へチェックして，次にその下の行・・と繰り返す）し，先に発見される方をpuyo1, 次に発見される方をpuyo2に格納
		puyocolor puyo1, puyo2;
		int puyo1_x = 0;
		int puyo1_y = 0;
		int puyo2_x = 0;
		int puyo2_y = 0;

		bool findingpuyo1 = true;
		for (int y = 0; y < puyoactive.GetLine(); y++)
		{
			for (int x = 0; x < puyoactive.GetColumn(); x++)
			{
				if (puyoactive.GetValue(y, x) != NONE)
				{
					if (findingpuyo1)
					{
						puyo1 = puyoactive.GetValue(y, x);
						puyo1_x = x;
						puyo1_y = y;
						findingpuyo1 = false;
					}
					else
					{
						puyo2 = puyoactive.GetValue(y, x);
						puyo2_x = x;
						puyo2_y = y;
					}
				}
			}
		}


		//回転前のぷよを消す
		puyoactive.SetValue(puyo1_y, puyo1_x, NONE);
		puyoactive.SetValue(puyo2_y, puyo2_x, NONE);


		//操作中ぷよの回転
		switch (puyoactive.GetRotate())
		{
		case 0:
			//回転パターン
			//RB -> R
			//      B
			//Rがpuyo1, Bがpuyo2
			if (puyo2_x <= 0 || puyo2_y >= puyoactive.GetLine() - 1)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
			puyoactive.SetValue(puyo2_y + 1, puyo2_x - 1, puyo2);
			//次の回転パターンの設定
			puyoactive.SetRotate(1);
			break;

		case 1:
			//回転パターン
			//R -> BR
			//B
			//Rがpuyo1, Bがpuyo2
			if (puyo2_x <= 0 || puyo2_y <= 0)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
			puyoactive.SetValue(puyo2_y - 1, puyo2_x - 1, puyo2);

			//次の回転パターンの設定
			puyoactive.SetRotate(2);
			break;

		case 2:
			//回転パターン
			//      B
			//BR -> R
			//Bがpuyo1, Rがpuyo2
			if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y <= 0)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y - 1, puyo1_x + 1, puyo1);
			puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

			//次の回転パターンの設定
			puyoactive.SetRotate(3);
			break;

		case 3:
			//回転パターン
			//B
			//R -> RB
			//Bがpuyo1, Rがpuyo2
			if (puyo1_x >= puyoactive.GetColumn() - 1 || puyo1_y >= puyoactive.GetLine() - 1)	//もし回転した結果field_arrayの範囲外に出るなら回転しない
			{
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
				break;
			}

			//回転後の位置にぷよを置く
			puyoactive.SetValue(puyo1_y + 1, puyo1_x + 1, puyo1);
			puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);

			//次の回転パターンの設定
			puyoactive.SetRotate(0);
			break;

		default:
			break;
		}
	}

	void StackDown(PuyoArrayStack& stack)
  {
    //一時的格納場所メモリ確保
	puyocolor *puyo_temp = new puyocolor[stack.GetLine()*stack.GetColumn()];

	for (int i = 0; i < stack.GetLine()*stack.GetColumn(); i++)
	  {
	    puyo_temp[i] = NONE;
	  }
	
	//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
	for (int y = stack.GetLine() - 1; y >= 0; y--)
	  {
	    for (int x = 0; x < stack.GetColumn(); x++)
	      {
		if (stack.GetValue(y, x) == NONE) {
		  continue;
		}
		
		if (y < stack.GetLine() - 1 && stack.GetValue(y + 1, x) == NONE)
		  {
		    puyo_temp[(y + 1)*stack.GetColumn() + x] = stack.GetValue(y, x);
		    //コピー後に元位置のpuyoactiveのデータは消す
		    stack.SetValue(y, x, NONE);
		  }
		else
		  {
		    puyo_temp[y*stack.GetColumn() + x] = stack.GetValue(y, x);
		  }
	      }
	  }

	//puyo_tempからpuyoactiveへコピー
	for (int y = 0; y < stack.GetLine(); y++)
	  {
	    for (int x = 0; x < stack.GetColumn(); x++)
	      {
		stack.SetValue(y, x, puyo_temp[y*stack.GetColumn() + x]);
	      }
	  }

	//一時的格納場所メモリ解放
	delete[] puyo_temp;
  }
  
	//浮いてるstackぷよがあるかどうかの判定
	bool CheckStack(PuyoArrayStack& stack)
	{
		//浮いてるstackぷよがあるかどうかの判定のフラグ
		bool flag = false;
		bool nonechecker = true;
		for (int y = 0; y < stack.GetLine(); y++)
      	{
			for (int x = 0; x < stack.GetColumn(); x++)
	  		{
				if (stack.GetValue(y, x) != NONE)
				{
					nonechecker = false;
					if (stack.GetValue(y+1, x) == NONE && y != stack.GetLine() - 1)
					{
						flag = true;
					}
				}
			}
		}
		if (nonechecker) {
			return true;
		}
		return flag;
	}

	//動いているぷよがあるかどうかの判定
	bool CheckActive(PuyoArrayActive& active)
	{
		bool flag = false;
		for (int y = 0; y < active.GetLine(); y++)
      	{
			for (int x = 0; x < active.GetColumn(); x++)
	  		{
				if (active.GetValue(y, x) != NONE){
					flag = true;
				}
			}
		}
		return flag;
	}
};



//表示
void Display(PuyoArrayActive& puyo, PuyoArrayStack& stack)
{
    // 色を定義
    init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	
	//落下中ぷよ表示
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
		    //積んであるぷよはstackを参照して表示
			switch (stack.GetValue(y, x))
			{
			case NONE:
			    attrset(0);
				mvaddch(y, x, '.');
				break;
			case RED:
			    attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
			    attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
			    attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
			    attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}
            
			//落ちているぷよはactiveを参照して表示
			switch (puyo.GetValue(y, x))
			{
			case RED:
			    attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
			    attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
			    attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
			    attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			}
			
		}
	}


	//情報表示
	int count = 0;
	for (int y = 0; y < puyo.GetLine(); y++)
	{
		for (int x = 0; x < puyo.GetColumn(); x++)
		{
			if (puyo.GetValue(y, x) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", puyo.GetLine(), puyo.GetColumn(), count);
	mvaddstr(2, COLS - 35, msg);

	refresh();
}


//ここから実行される
int main(int argc, char **argv){
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);

	//インスタンス作成
	PuyoArrayActive puyoActive;
	PuyoArrayStack puyoStack;
	PuyoControl control;
	
	//初期化処理
	puyoActive.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	puyoStack.ChangeSize(LINES/2, COLS/2);  //stackの盤面も作る
	control.GeneratePuyo(puyoActive);	//最初のぷよ生成

	int delay = 0;
	int waitCount = 20000;

	int puyostate = 0;


	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
		  control.MoveLeft(puyoActive, puyoStack);
			break;
		case KEY_RIGHT:
		  control.MoveRight(puyoActive, puyoStack);
			break;
		case 'z':
		  control.Rotate(puyoActive);
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			//ぷよ下に移動
		    control.MoveDown(puyoActive);
			
			//ぷよ着地判定
			if (control.LandingPuyo(puyoActive, puyoStack))
			{
			    //Puyoが4つ以上つながったら削除
			    control.VanishPuyo(puyoStack);
			}
			//浮いているぷよがあったら強制的に落とす
			if (control.CheckStack(puyoStack)){
				control.StackDown(puyoStack);
			} else {
				//Puyoが4つ以上つながったら削除
				control.VanishPuyo(puyoStack);
				//全ての着地してかつ動いているぷよが無かったら新しいぷよ生成
				if (control.CheckActive(puyoActive) == false){
					control.GeneratePuyo(puyoActive);
				}
			}
		}
		delay++;
		//表示
		Display(puyoActive, puyoStack);
	}


	//画面をリセット
	endwin();

	return 0;
}
