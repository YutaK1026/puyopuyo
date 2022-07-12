
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
