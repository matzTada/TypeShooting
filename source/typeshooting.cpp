/*SD3年　松井忠宗
タイピングゲーム×シューティングゲーム
実装用に地味にしてこんなことをやりたいってのをわかりやすくしたver.
文字を打ってあってればと玉が飛び、玉同士が跳ね返る。また数字を打つと玉が消えるようになっている。
あってないと玉は飛びませんことよ。
*/

#include <windows.h>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string.h>
#include "TSprototype1.h"
#include <time.h>
#include <random>
#include <stdlib.h>
#include <fstream>
#include <ctime>
#include <sstream>

//shootingの方の変数
#define bulletMAX 100

typedef struct{
	int note;
	double xx;
	double yy;
	double vx;
	double vy;
	double ax;
	double ay;
	double red;
	double green;
	double blue;
} ballet_t;
ballet_t b[bulletMAX] = { NULL };
static double dt = 0.1, g = 0.5;
static int rr(12);
static double velocity(1);
static int bnumcounta(0);

//typingの方の変数
std::string name = "";//入力された単語を保持する一時的なもの
std::string dictwords[300];
std::string typedwords[300];
static int typedwordsflag(0);
static int typednotesflag(0);//いまうってる文字の何番目にいるかどうかわかる
static int misscounta(0);

static double inputnotesxx[50] = { 0 };//今売ってる文字のx座標。グラフィック用
static double inputnotesyy[50] = { 0 };

//共通の変数
static int idleflag(0);
static int tt(0);

//shooting functions
double randombyrandom(int min, int max){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(mt);
}
void newbullet(unsigned char k){//新しい球を打つ時の関数
	b[bnumcounta].note = k;
	b[bnumcounta].xx = inputnotesxx[typednotesflag];
	b[bnumcounta].yy = inputnotesyy[typednotesflag];
	b[bnumcounta].vx = randombyrandom(-20, 20);
	b[bnumcounta].vy = randombyrandom(20, 50);
	b[bnumcounta].red = (double)randombyrandom(0, 255) / 255;
	b[bnumcounta].green = (double)randombyrandom(0, 255) / 255;
	b[bnumcounta].blue = (double)randombyrandom(0, 255) / 255;
	b[bnumcounta].ax = 0;
	b[bnumcounta].ay = -g;
	bnumcounta++;
	if (bnumcounta >= bulletMAX) bnumcounta = 0; //bulletMAX以上に玉が増えないようにしている。
}
void deletebullet(unsigned char k){
	for (int i(0); i < bulletMAX; i++){
		if (b[i].note == k){
			b[i].xx = NULL;
			b[i].yy = NULL;
			b[i].vx = NULL;
			b[i].vy = NULL;
			b[i].red = NULL;
			b[i].green = NULL;
			b[i].blue = NULL;
			b[i].ax = NULL;
			b[i].ay = NULL;
		}
	}
}
void circles(double &xx, double &yy, double &vx, double &vy, double &ax, double &ay, int &note){//円の移動、壁での跳ね返りの計算の関数
	//y座標について
	vy = vy + ay*dt;
	yy = yy + vy*dt*velocity;
	if (yy - rr  < 0){ vy = -vy; yy = 0 + rr; }
	if (yy + rr  > 600){ vy = -vy; yy = 600 - rr; note = (int)randombyrandom(48, 57); }//上に当たったら数字をかえる。

	//x座標について
	vx = vx + ax*dt;
	xx = xx + vx*dt*velocity;
	if (xx - rr  < 0){ vx = -vx; xx = 0 + rr; }
	if (xx + rr  > 600){ vx = -vx; xx = 600 - rr; }
}
void avoidcirclesstanning(double &x1, double &y1, double &x2, double &y2){
	double cx, cy;
	double dx, dy;
	double costheta, sintheta;
	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;
	dx = x1 - x2; //どっちが上側にあるかも同時に判別したい
	dy = y1 - y2;
	costheta = fabs(dx) / sqrt(fabs(dx*dx + dy*dy));
	sintheta = fabs(dy) / sqrt(fabs(dx*dx + dy*dy));

	if (dx > 0){//x1の方が右側にあったとき
		x1 = cx + (rr)* costheta;
		x2 = cx - (rr)* costheta;
	}
	if (dx < 0){
		x1 = cx - (rr)* costheta;
		x2 = cx + (rr)* costheta;
	}
	if (dy > 0){//y1の方が右側にあったとき
		y1 = cy + (rr)* sintheta;
		y2 = cy - (rr)* sintheta;
	}
	if (dy < 0){
		y1 = cy - (rr)* sintheta;
		y2 = cy + (rr)* sintheta;
	}
}
void collision(double &v1x, double &v1y, double &v2x, double &v2y){//円どうしの跳ね返りの関数
	//速度について
	double vzx, vzy;
	vzx = (fabs(v1x) + fabs(v2x)) / 2;
	vzy = (fabs(v1y) + fabs(v2y)) / 2;
	v1x = (v1x / fabs(v1x))*vzx;//符号の取り出し
	v1y = (v1y / fabs(v1y))*vzy;
	v2x = (v2x / fabs(v2x))*vzx;
	v2y = (v2y / fabs(v2y))*vzy;

	//実行部分
	if (v1x != v2x){//x方向の速さの方向が違ってたら
		v1x = -v1x;
		v2x = -v2x;
	}
	if (v1y != v2y){//y方向の速さの方向が違ってたｒあ
		v1y = -v1y;
		v2y = -v2y;
	}
}

//typing functoins
void loaddictwords(){//dictionaryファイルに入ってる文字を読み込む。絶対に書き込みはしない。
	using namespace std;
	cout << "::: loading... :::" << endl;
	ifstream infile;
	infile.open("EnglishDictionary.txt");//ここで読み込む辞書ファイルを書く
	int flag(0);
	for (int i(0); i < 100; i++){
		if (infile.eof()){//確かこれが文字列の終了を読み込むところだったような気がする。
			flag = i - 1;
			break;
		}
		infile >> dictwords[i];//dictwords[]に読み込んだのをぶち込んでいく。
		cout << dictwords[i] << endl;//ちゃんと読み込まれたかの確認
	}
	infile.close();
	cout << "::: load succesfully!!! :::" << endl;
}
void matching(unsigned char k){
	if (k == dictwords[typedwordsflag].at(typednotesflag)){
		name += k;
		typednotesflag++;
		newbullet(k);
		if (dictwords[typedwordsflag].size() == typednotesflag){//dictwordsの長さとtypednotesflagの値が一緒になったら次の文字へ。
			typedwords[typedwordsflag++] = name;
			std::cout << name << std::endl;
			typednotesflag = 0;
			name = "";
		}
	}
	else{//間違えると音がなっちゃいますよっと。Beep音がなってる間は他の処理を受け付けないらしいのでペナルティにはちょうどいいかも。
		misscounta++;
		switch (misscounta % 8){
			//case : Beep(440, 150); break; //ラ
			//case : Beep(466, 150); break;
			//case : Beep(494, 150); break; //シ
		case 1: Beep(523, 150); break; //ド
			//case : Beep(554, 150); break;
		case 2: Beep(587, 150); break; //レ
			//case : Beep(622, 150); break;
		case 3: Beep(659, 150); break; //ミ
		case 4: Beep(698, 150); break; //ファ
			//case : Beep(740, 150); break;
		case 5: Beep(784, 150); break; //ソ
			//case : Beep(831, 150); break;
		case 6: Beep(880, 150); break; //ラ
			//case : Beep(932, 150); break; 
		case 7: Beep(988, 150); break; //シ
		case 0: Beep(1047, 150); break; //ド
		}
	}
}

//shooting visualising
void displaybullet(){
	using namespace SDGLibF;
	//玉を描く
	for (int i(0); i < bulletMAX; i++){
		SetColor(b[i].red, b[i].green, b[i].blue);
		DrawCircle(2.0, b[i].xx, b[i].yy, rr);
		char tempbuff[10];
		sprintf_s(tempbuff, "%c", b[i].note);
		DrawString(b[i].xx - 5, b[i].yy - 5, tempbuff);//これで文字も飛ぶように見えるはず。
	}
}

//typing visualising
void displaysigniture(){//署名的なもの
	using namespace SDGLibF;
	char buff3[100];
	SetColor(0.8, 0.8, 0.8);
	sprintf_s(buff3, "KEIO SD-Engineer");
	DrawString(130, 90, buff3);
	char buff6[100];
	SetColor(0.99, 0.54, 0);
	sprintf_s(buff6, "Tadanori Matsui");
	DrawString(340, 90, buff6);
	char buff4[100];
	SetColor(0.8, 0.8, 0.8);
	sprintf_s(buff4, "THANK YOU FOR PLAYING");
	DrawString(150, 60, buff4);
}
void displaycurrenttime(){//現在時刻の表示
	//現在時刻の取得
	using namespace SDGLibF;
	struct tm now;
	time_t longtime;
	longtime = time(NULL);
	char timestr[32];
	longtime = time(NULL);
	localtime_s(&now, &longtime);
	asctime_s(timestr, sizeof(timestr), &now);
	SetColor(0.2, 0.2, 0.2);
	DrawString(330, 0, timestr);
}
void displaydictwords(){//dictwordsの表示。
	using namespace SDGLibF;
	SetColor(0.5, 0.5, 0.5);
	char bufftotal[100];
	sprintf_s(bufftotal, "dictwords:");
	DrawString(0, 575, bufftotal);
	for (int i(0); i < 22; i++){
		char tempbuff[100];
		sprintf_s(tempbuff, "%d: %s", i + 1, dictwords[i].c_str());
		DrawString(0, 550 - i * 25, tempbuff);
	}
}
void displaytypedwords(){//typedwordsの表示。
	using namespace SDGLibF;
	SetColor(0, 0, 1);
	char bufftotal[100];
	sprintf_s(bufftotal, "typedwords:");
	DrawString(450, 575, bufftotal);
	for (int i(0); i < 22; i++){
		char tempbuff[100];
		sprintf_s(tempbuff, "%d: %s", i + 1, typedwords[i].c_str());
		DrawString(450, 550 - i * 25, tempbuff);
	}
}
void displaymatching(){//入力した文字と入力下線の表示。タイピングゲームっぽく見せるところ
	using namespace SDGLibF;
	//string型での表示
	//段々と動くほう
	SetColor(0.7, 0.7, 0.7);//お題の文字
	DrawString(250, 545 - typedwordsflag * 25, dictwords[typedwordsflag].c_str());
	SetColor(0, 0, 1);//入力してる文字
	DrawString(250, 545 - typedwordsflag * 25, name.c_str());
	SetColor(0, 0.9, 0);//下線
	DrawString(180, 545 - typedwordsflag * 25, "type it:");
	DrawLine(2, 0, 545 - typedwordsflag * 25 - 5, 600, 545 - typedwordsflag * 25 - 5);

	//ずっと上にいる方。
	SetColor(0.7, 0.7, 0.7);//お題の文字
	DrawString(250, 545, dictwords[typedwordsflag].c_str());
	SetColor(0, 0, 1);//入力してる文字
	DrawString(250, 545, name.c_str());
	SetColor(0, 0.9, 0);//下線
	DrawString(180, 545, "type it:");
	DrawLine(2, 180, 545 - 5, 400, 545 - 5);

	//お題の文字の方
	int wordlength = dictwords[typedwordsflag].length();
	for (int i(0); i < wordlength; i++){//ちゃんと長さを指定しないとオーバーフローして止まっちゃうので気をつけましょう。
		inputnotesxx[i] = 200 + 10 + i * 20;
		inputnotesyy[i] = 30;

		char tempn = dictwords[typedwordsflag].at(i);//ワードリストのtypedwordsflag+1番目の単語の、typednotesflag+1番目の文字
		char tempbuff[100];
		SetColor(0.3, 0.3, 0.3);//お題の文字
		sprintf_s(tempbuff, "%c", tempn);
		DrawString(inputnotesxx[i], inputnotesyy[i], tempbuff);
	}

	//入力する文字の方
	int namelength = name.length();
	for (int i(0); i < namelength; i++){
		char tempn1 = name.at(i);//入力してる文字。上書きだね
		char tempbuff1[100];
		SetColor(0.9, 0.9, 0.9);//
		sprintf_s(tempbuff1, "%c", tempn1);
		DrawString(inputnotesxx[i], inputnotesyy[i], tempbuff1);
	
		SetColor(0, 0.9, 0);//下線
		DrawLine(2, inputnotesxx[i], inputnotesyy[i] - 5, inputnotesxx[i] +20, inputnotesyy[i] - 5);
	}
	SetColor(0, 0.9, 0);//type it:の文字
	DrawString(inputnotesxx[0] - 80, inputnotesyy[0], "type it:");

}

//main visualiser
void displayfunc(){//描画関数
	using namespace SDGLibF;
	Before(); // Set drawing plane

	displaysigniture();
	displaycurrenttime();
	displaydictwords();
	displaytypedwords();
	displaymatching();

	displaybullet();
	int bulletsum(0);//bullet合計の計算
	for (int i(0); i < bulletMAX; i++) {
		if (b[i].xx > 0 && b[i].yy > 0) bulletsum++;
	}

	char buff[100];
	SetColor(0.3, 0.3, 0.3);
	sprintf_s(buff, "v=%3.1f,rr=%d,b=%d,miss:%d", velocity, rr, bulletsum, misscounta);
	DrawString(0, 0, buff);

	After(); // Set drawn plane
}

void simulation(void){//実際に動かす関数
	using namespace SDGLibF;
	tt += 1.0;

	//円の座標の計算とか
	for (int i(0); i < bulletMAX; i++){
		circles(b[i].xx, b[i].yy, b[i].vx, b[i].vy, b[i].ax, b[i].ay, b[i].note);
	}

	//円どうしの衝突の実装
	for (int i(0); i < bulletMAX; i++){
		for (int j(0); j < i + 1; j++){
			if (sqrt(((int)b[i].xx - (int)b[j].xx)*((int)b[i].xx - (int)b[j].xx) + ((int)b[i].yy - (int)b[j].yy)*((int)b[i].yy - (int)b[j].yy)) + 1 <= 2 * rr){
				collision(b[i].vx, b[i].vy, b[j].vx, b[j].vy);
				avoidcirclesstanning(b[i].xx, b[i].yy, b[j].xx, b[j].yy);
			}
		}
	}

	ReDraw();
}

void keyboardfunc(unsigned char k, int x, int y){
	using namespace SDGLibF;
	switch (k) {
	case 27:  exit(0); /* Escape */
	case VK_RETURN: idleflag++; if (idleflag % 2 == 1) IdleFunc(simulation); else IdleFunc(NULL); break;

	case '"': rr += 1.0; if (rr >= 20) rr = 20; break;
	case '#': rr -= 1.0; if (rr <= 5) rr = 5; break;
	case '$': velocity += 0.1; break;
	case '%': velocity -= 0.1; if (velocity <= 0.1) velocity = 0.1; break;

	//a-z
	case 97:	case 98:	case 99:	case 100:	case 101:
	case 102:	case 103:	case 104:	case 105:	case 106:
	case 107:	case 108:	case 109:	case 110:	case 111:
	case 112:	case 113:	case 114:	case 115:	case 116:
	case 117:	case 118:	case 119:	case 120:	case 121:
	case 122:
	//A-Z
	case 65:	case 66:	case 67:	case 68:	case 69:
	case 70:	case 71:	case 72:	case 73:	case 74:
	case 75:	case 76:	case 77:	case 78:	case 79:
	case 80:	case 81:	case 82:	case 83:	case 84:
	case 85:	case 86:	case 87:	case 88:	case 89:
	case 90:
		matching(k); break;// matchingの中にnewbulletを入れてある。

		//case VK_BACK: if (name.size() >= 1) { name.erase(name.size() - 1); typednotesflag--; } break; //これ有効にしてa->bkspace->a->nkspace...ってやってると無敵ww

	case 48:case 49:case 50:case 51:case 52:
	case 53:case 54:case 55:case 56:case 57:
		deletebullet(k); break;//0-9まで

	default:
		break;
	}
}

int main(void){
	loaddictwords();
	SDGLib mygraphic(600, 600, "Typing Shooting Prototype", 0, 600, 0, 600);
	mygraphic.SetCursor(GLUT_CURSOR_WAIT);
	mygraphic.Display(displayfunc);
	mygraphic.Keyboard(keyboardfunc);
	mygraphic.Start();
	return 0;
}

/*
構造体の配列を使ってみよう。

141014char型で一文字ずつ処理するのに苦労しました。

バグ
141012
・同時にたくさん出すと、左側(x=0)方向で消えていく。
*/

