//課題
//2020/04/17

#include <curses.h>
#include <cstdlib>
#include <time.h>

//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor { NONE, RED, BLUE, GREEN, YELLOW };

class PuyoArray{
	private:
		puyocolor *data;
		unsigned int data_line;
		unsigned int data_column;
		void Release(){
			if (data == NULL) {
				return;
			}
			delete[] data;
			data = NULL;
		};
	public:
		PuyoArray(){
			data = NULL;
			data_line = 0;
			data_column = 0;
		}
		~PuyoArray(){
			Release();
		}
		void ChangeSize(unsigned int line, unsigned int column)
		{
			Release();

			//新しいサイズでメモリ確保
			data = new puyocolor[line*column];

			data_line = line;
			data_column = column;
		}
		unsigned int GetLine()
		{
			return data_line;
		}
		unsigned int GetColumn()
		{
			return data_column;
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

//ランダムにぷよを選ぶ
enum puyocolor GenerateRandom()
{
	std::srand(time(NULL));

	int num = rand () % 4 + 1;
	//非ボイド関数エラーの終了に達する制御解決の為
	puyocolor puyo;
	switch (num) {
		case 1:
			puyo = RED;
			break;
		case 2:
			puyo = BLUE;
			break;
		case 3:
			puyo = GREEN;
			break;
		case 4:
			puyo = YELLOW;
			break;
	}
	return puyo;
}
class PuyoArrayStack:public PuyoArray{};
class PuyoArrayActive:public PuyoArray{
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
	
class PuyoControl{
//盤面に新しいぷよ生成
public:
	
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

	//着地したぷよの個数を判定する。
	//2個が着地された、となった時点で新しいぷよを生成する為。
	int landed_num = 0;
	int num_puyo1,num_puyo2 = 0;
	void GeneratePuyo(PuyoArrayActive &puyoActive)
	{
		std::srand(time(NULL));
		puyocolor newpuyo1,newpuyo2;
		num_puyo1 = rand () % 4 + 1;
		//非ボイド関数エラーの終了に達する制御解決の為
		switch (num_puyo1) {
			case 1:
				newpuyo1 = RED;
				break;
			case 2:
				newpuyo1 = BLUE;
				break;
			case 3:
				newpuyo1 = GREEN;
				break;
			case 4:
				newpuyo1 = YELLOW;
				break;
		}
		num_puyo2 = rand () % 4 + 1;
		//非ボイド関数エラーの終了に達する制御解決の為
		switch (num_puyo2) {
			case 1:
				newpuyo2 = RED;
				break;
			case 2:
				newpuyo2 = BLUE;
				break;
			case 3:
				newpuyo2 = GREEN;
				break;
			case 4:
				newpuyo2 = YELLOW;
				break;
		}
		
		//Rotateを0に設定
    	puyoActive.SetRotate(0);

		puyoActive.SetValue(0, 5, newpuyo1);
		puyoActive.SetValue(0, 6, newpuyo2);
	}
	//浮いてるstackぷよがあるかどうかの判定
	bool checkStack(PuyoArrayStack& puyoStack)
	{
		//浮いてるぷよがあるかどうかの判定のフラグ
		//浮いていればfalseを返し、そうでなければfalseを返すようにする
		bool landed = false;
		bool nonechecker = true;
		for (int y = 0; y < puyoStack.GetLine(); y++)
      	{
			for (int x = 0; x < puyoStack.GetColumn(); x++)
	  		{
				if (puyoStack.GetValue(y, x) != NONE)
				{
					nonechecker = false;
					if (puyoStack.GetValue(y+1, x) == NONE && y != puyoStack.GetLine() - 1)
					{
						landed = true;
					}
				}
			}
		}
		if (nonechecker) {
			return true;
		}
		return landed;
	}

	//動いているぷよがあるかどうかの判定
	bool checkActive(PuyoArrayActive& puyoActive)
	{
		bool move = false;
		for (int y = 0; y < puyoActive.GetLine(); y++)
      	{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
	  		{
				if (puyoActive.GetValue(y, x) != NONE){
					move = true;
				}
			}
		}
		return move;
	}
	//ぷよを下に落とす関数を実装
	//ぷよの下にぷよ及び地面が無ければ下まで落とす。
	void DropPuyo(PuyoArrayStack& stack)
	//MoveLeftなどを参考に作成
	//今までのように、一気に下に落とすようなものではなくデモ動画にあった少しずつ順番に落ちるように作成
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
	//ぷよの着地判定．着地判定があるとtrueを返す
	bool LandingPuyo(PuyoArrayActive& puyoActive, PuyoArrayStack& puyoStack)
  {
    bool landed,moveFlag = false;
	//ぷよが動いているかのフラグを追加。moveFlag
    for (int y = 0; y < puyoActive.GetLine(); y++)
      {
	for (int x = 0; x < puyoActive.GetColumn(); x++)
	  {
	    if (puyoActive.GetValue(y, x) != NONE && (y == puyoActive.GetLine() - 1 || puyoStack.GetValue(y+1, x) != NONE))
	    {
		puyoStack.SetValue(y, x, puyoActive.GetValue(y, x));
		//着地したぷよは、Activeからは削除し、同様のデータを持つぷよをStackに新規追加する。
		puyoActive.SetValue(y, x, NONE);
		landed = true;

		//停止したぷよの周りに、Active状態のぷよがあるか否かの処理
		if (puyoActive.GetValue(y-1, x) != NONE) {
			puyoStack.SetValue(y-1, x, puyoActive.GetValue(y-1, x));
			puyoActive.SetValue(y-1, x, NONE);
			landed = true;
			moveFlag = false;
		} else if (puyoActive.GetValue(y, x-1) != NONE) {
			puyoStack.SetValue(y, x-1, puyoActive.GetValue(y, x-1));
			puyoActive.SetValue(y, x-1, NONE);
			landed = true;
			moveFlag = false;
		} else if (puyoActive.GetValue(y, x+1) != NONE) {
			puyoStack.SetValue(y, x+1, puyoActive.GetValue(y, x+1));
			puyoActive.SetValue(y, x+1, NONE);
			landed = true;
			moveFlag = false;
		}
 	    } else if (puyoActive.GetValue(y, x) != NONE && puyoStack.GetValue(y+1, x) == NONE){
			landed = false;
			//動いているぷよがある時の処理
			moveFlag = true;
	    }
	  }
    }
	//着地判定を返す際、まだぷよが動いていれば確定でfalseを返す。(generatePuyoの発火をふせぐ)
    if (moveFlag){
		return false;
    }else{
    	return landed;
	}
  }
	void Rotate(PuyoArrayActive &puyoactive,PuyoArrayStack &puyostack)
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
			if(puyostack.GetValue(puyo2_y + 1, puyo2_x - 1) == NONE){
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y + 1, puyo2_x - 1, puyo2);
				//次の回転パターンの設定
				puyoactive.SetRotate(1);
				break;
			}else{
				//回転後の位置にぷよを置く
				//回転する向きは変えずに、上方向にずらす
				puyoactive.SetValue(puyo1_y, puyo1_x+1, puyo1);
				puyoactive.SetValue(puyo2_y + 1, puyo2_x, puyo2);
				//次の回転パターンの設定
				puyoactive.SetRotate(1);
				break;
			}

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
			if(puyostack.GetValue(puyo2_y - 1, puyo2_x - 1) == NONE){
				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y - 1, puyo2_x - 1, puyo2);
			}else{
				puyoactive.SetValue(puyo1_y, puyo1_x+1, puyo1);
				puyoactive.SetValue(puyo2_y - 1, puyo2_x, puyo2);
			}

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
			if(puyostack.GetValue(puyo2_y - 1, puyo2_x + 1) == NONE){
				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y - 1, puyo1_x + 1, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
			}else{
				puyoactive.SetValue(puyo1_y, puyo1_x + 1, puyo1);
				puyoactive.SetValue(puyo2_y+1, puyo2_x, puyo2);
			}
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
			
			if(puyostack.GetValue(puyo2_y + 1, puyo2_x + 1) == NONE){
				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y + 1, puyo1_x + 1, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x, puyo2);
			}else{
				//回転後の位置にぷよを置く
				puyoactive.SetValue(puyo1_y + 1, puyo1_x, puyo1);
				puyoactive.SetValue(puyo2_y, puyo2_x-1, puyo2);
			}

			//次の回転パターンの設定
			puyoactive.SetRotate(0);
			break;

		default:
			break;
		}
	}
	//下に落下する
	void ForceMoveDown(PuyoArrayActive &puyoActive ,PuyoArrayStack &puyoStack)
	{
		bool landed = false;
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				if (puyoActive.GetValue(y,x) != NONE){
					for (int i = y; i < puyoActive.GetLine(); i++){
						if(puyoStack.GetValue(i,x) != NONE){
							puyoStack.SetValue(i-1,x,puyoActive.GetValue(y,x));
							puyoActive.SetValue(y,x,NONE);
							landed = true;
							break;
						}
					}
					if (puyoActive.GetValue(y,x) != NONE){
						puyoStack.SetValue(puyoActive.GetLine()-1,x,puyoActive.GetValue(y,x));
						puyoActive.SetValue(y,x,NONE);
						landed = true;
					}
				}
			}
		}
	}
	
	//左移動
	void MoveLeft(PuyoArrayActive &puyoActive)
	{
		//一時的格納場所メモリ確保
		puyocolor *puyo_temp = new puyocolor[puyoActive.GetLine()*puyoActive.GetColumn()];

		for (int i = 0; i < puyoActive.GetLine()*puyoActive.GetColumn(); i++)
		{
			puyo_temp[i] = NONE;
		}

		//1つ左の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				if (puyoActive.GetValue(y, x) == NONE) {
					continue;
				}

				if (0 < x && puyoActive.GetValue(y, x - 1) == NONE)
				{
					puyo_temp[y*puyoActive.GetColumn() + (x - 1)] = puyoActive.GetValue(y, x);
					//コピー後に元位置のpuyoactiveのデータは消す
					puyoActive.SetValue(y, x, NONE);
				}
				else
				{
					puyo_temp[y*puyoActive.GetColumn() + x] = puyoActive.GetValue(y, x);
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				puyoActive.SetValue(y, x, puyo_temp[y*puyoActive.GetColumn() + x]);
			}
		}

		//一時的格納場所メモリ解放
		delete[] puyo_temp;
	}
	//右移動
	void MoveRight(PuyoArrayActive &puyoActive)
	{
		//一時的格納場所メモリ確保
		puyocolor *puyo_temp = new puyocolor[puyoActive.GetLine()*puyoActive.GetColumn()];

		for (int i = 0; i < puyoActive.GetLine()*puyoActive.GetColumn(); i++)
		{
			puyo_temp[i] = NONE;
		}

		//1つ右の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = puyoActive.GetColumn() - 1; x >= 0; x--)
			{
				if (puyoActive.GetValue(y, x) == NONE) {
					continue;
				}

				if (x < puyoActive.GetColumn() - 1 && puyoActive.GetValue(y, x + 1) == NONE)
				{
					puyo_temp[y*puyoActive.GetColumn() + (x + 1)] = puyoActive.GetValue(y, x);
					//コピー後に元位置のpuyoactiveのデータは消す
					puyoActive.SetValue(y, x, NONE);
				}
				else
				{
					puyo_temp[y*puyoActive.GetColumn() + x] = puyoActive.GetValue(y, x);
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー
		for (int y = 0; y <puyoActive.GetLine(); y++)
		{
			for (int x = 0; x <puyoActive.GetColumn(); x++)
			{
				puyoActive.SetValue(y, x, puyo_temp[y*puyoActive.GetColumn() + x]);
			}
		}

		//一時的格納場所メモリ解放
		delete[] puyo_temp;
	}

	//下移動
	void MoveDown(PuyoArrayActive &puyoActive)
	{
		//一時的格納場所メモリ確保
		puyocolor *puyo_temp = new puyocolor[puyoActive.GetLine()*puyoActive.GetColumn()];

		for (int i = 0; i < puyoActive.GetLine()*puyoActive.GetColumn(); i++)
		{
			puyo_temp[i] = NONE;
		}

		//1つ下の位置にpuyoactiveからpuyo_tempへとコピー
		for (int y = puyoActive.GetLine() - 1; y >= 0; y--)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				if (puyoActive.GetValue(y, x) == NONE) {
					continue;
				}

				if (y < puyoActive.GetLine() - 1 && puyoActive.GetValue(y + 1, x) == NONE)
				{
					puyo_temp[(y + 1)*puyoActive.GetColumn() + x] = puyoActive.GetValue(y, x);
					//コピー後に元位置のpuyoactiveのデータは消す
					puyoActive.SetValue(y, x, NONE);
				}
				else
				{
					puyo_temp[y*puyoActive.GetColumn() + x] = puyoActive.GetValue(y, x);
				}
			}
		}

		//puyo_tempからpuyoactiveへコピー
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				puyoActive.SetValue(y, x, puyo_temp[y*puyoActive.GetColumn() + x]);
			}
		}

		//一時的格納場所メモリ解放
		delete[] puyo_temp;
	}
};
//表示

void Display(PuyoArrayActive &puyoActive,PuyoArrayStack &puyoStack, int counted)
{
	//PuyoArray puyo(NULL,0,0);
	//色指定
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	
	//落下中ぷよ表示
	
	for (int y = 0; y < puyoActive.GetLine(); y++)
	{
		for (int x = 0; x < puyoActive.GetColumn(); x++)
		{
			switch (puyoActive.GetValue(y, x))
			{
			case NONE:
				attrset(COLOR_PAIR(0));
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
				attrset(COLOR_PAIR(0));
				mvaddch(y, x, '?');
				break;
			}
			//ぷよActiveが無ければ、そこに落下判定を貰って停止しているぷよが存在しうるので、その確認を行う。
			if(puyoActive.GetValue(y,x) == NONE){
			switch (puyoStack.GetValue(y, x))
			{
			case NONE:
				attrset(COLOR_PAIR(0));
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
				attrset(COLOR_PAIR(0));
				mvaddch(y, x, '?');
				break;
			}
			}
			
		}
	}
	

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", puyoActive.GetLine(), puyoActive.GetColumn(), counted);
	mvaddstr(2, COLS - 35, msg);

	refresh();
}


//ここから実行される

int main(int argc, char **argv){
	//PuyoArrayActive, PuyoArrayStackともに初期化
	PuyoArrayActive puyoActive;
	PuyoArrayStack puyoStack;
	PuyoControl control;
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


	//初期化処理
	puyoStack.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	puyoActive.ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	
	control.GeneratePuyo(puyoActive);	//最初のぷよ生成

	int delay = 0;
	int waitCount = 20000;

	int puyostate = 0;
	int counted = 0;


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
			control.MoveLeft(puyoActive);
			break;
		case KEY_RIGHT:
			control.MoveRight(puyoActive);
			break;
		case KEY_DOWN:
			//control.ForceMoveDown(puyoActive,puyoStack);
			waitCount = 5000;
			break;
		case 'z':
			//ぷよ回転処理
			control.Rotate(puyoActive,puyoStack);
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			waitCount = 20000;
			//ぷよ下に移動
			control.MoveDown(puyoActive);
			
			//ぷよ着地判定
			if (control.LandingPuyo(puyoActive,puyoStack))
			{
				control.VanishPuyo(puyoStack);
			}
			if (control.checkStack(puyoStack))
			{
				//一つずつぷよを下に落下させる。
				control.DropPuyo(puyoStack);
			}else{
				//Puyoが4つ以上つながったら削除
				counted += control.VanishPuyo(puyoStack);
				control.VanishPuyo(puyoStack);
				//全ての着地してかつ動いているぷよが無かったら新しいぷよ生成
				if (control.checkActive(puyoActive) == false)
				{
					control.GeneratePuyo(puyoActive);
				}
			}
		}
		delay++;

		//表示
		Display(puyoActive,puyoStack,counted);
	}

	//画面をリセット
	endwin();

	return 0;
}