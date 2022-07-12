//課題
//2020/04/17

#include <curses.h>
#include <cstdlib>

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
class PuyoArrayActive:public PuyoArray{};
	
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
	void GeneratePuyo(PuyoArrayActive &puyoActive)
	{
		
		puyocolor newpuyo1;

		newpuyo1 = GenerateRandom();

		puyocolor newpuyo2;
		newpuyo2 = GenerateRandom();

		puyoActive.SetValue(0, 5, newpuyo1);
		puyoActive.SetValue(0, 6, newpuyo2);
	}
	//ぷよの着地判定．着地判定があるとtrueを返す
	bool LandingPuyo(PuyoArrayActive &puyoActive,PuyoArrayStack &puyoStack)
	{
		
		bool landed = false;
		for (int y = 0; y < puyoActive.GetLine(); y++)
		{
			for (int x = 0; x < puyoActive.GetColumn(); x++)
			{
				//ぷよが着地するか否かの判定を行う。
				//隣にぷよが存在する場合、隣のぷよの下にぷよがあれば停止する必要がある為その旨の記述も行った
				//今後回転機能を追加していくにあたり、回転を考慮して上方向にもぷよが存在するか否かに応じた判定を追加する必要がある。
				if (puyoActive.GetValue(y, x) != NONE && (y == puyoActive.GetLine() - 1 || 
					(puyoStack.GetValue(y+1, x) != NONE || 
					//左側にぷよがあるぷよに関しての着地判定は、まだ右側のぷよの着地判定がなされていないのでpuyoActiveを参照する必要がある。
					((puyoStack.GetValue(y+1, x+1) != NONE && puyoActive.GetValue(y, x+1) != NONE) || 
					//左側にぷよがあるぷよに関しての着地判定は、左側から順に着地判定をするため、puyoStackを参照する必要がある。
					(puyoStack.GetValue(y+1, x-1) != NONE && (puyoStack.GetValue(y, x-1) != NONE && puyoActive.GetValue(y, x+1) == NONE))))))
				{	
					landed = true;
					puyoStack.SetValue(y,x,puyoActive.GetValue(y,x));
					//着地判定されたぷよを、PuyoArrayActive(動かせるぷよ)から削除する
					puyoActive.SetValue(y, x, NONE);
				}
			}
		}

		return landed;
	}
	//下にぷよが無い判定を返す
	//下にぷよが無ければ、そのぷよを、下にぷよあるいは地面があるところまで落とす。
	bool DropPuyo(PuyoArrayStack &puyoStack){
		bool drop = false;
		for (int y = 0; y < puyoStack.GetLine(); y++)
		{
			for (int x = 0; x < puyoStack.GetColumn(); x++)
			{
				if (puyoStack.GetValue(y,x) != NONE){
					for (int i = y; i < puyoStack.GetLine(); i++){
						if(puyoStack.GetValue(i,x) == NONE){
							puyoStack.SetValue(i,x,puyoStack.GetValue(y,x));
							puyoStack.SetValue(y,x,NONE);
							drop = true;
						}
					}
				}
			}
		}
		return drop;
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

void Display(PuyoArrayActive &puyoActive,PuyoArrayStack &puyoStack)
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
	
	//情報表示
	int count = 0;
	for (int y = 0; y < puyoActive.GetLine(); y++)
	{
		for (int x = 0; x < puyoActive.GetColumn(); x++)
		{
			if (puyoActive.GetValue(y, x) != NONE)
			{
				count++;
			}
		}
	}

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", puyoActive.GetLine(), puyoActive.GetColumn(), count);
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
		case 'z':
			//ぷよ回転処理
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			//ぷよ下に移動
			control.MoveDown(puyoActive);
			
			//ぷよ着地判定
			if (control.LandingPuyo(puyoActive,puyoStack))
			{
				control.VanishPuyo(puyoStack);
				//着地していたら新しいぷよ生成
				control.GeneratePuyo(puyoActive);
			}
			//ぷよを自動的に下に落とす(宙に残さない)機能
			if(control.DropPuyo(puyoStack)){
				control.VanishPuyo(puyoStack);
			}
		}
		delay++;

		//表示
		Display(puyoActive,puyoStack);
	}

	//画面をリセット
	endwin();

	return 0;
}
