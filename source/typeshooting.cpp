/*SD3�N�@���䒉�@
�^�C�s���O�Q�[���~�V���[�e�B���O�Q�[��
�����p�ɒn���ɂ��Ă���Ȃ��Ƃ���肽�����Ă̂��킩��₷������ver.
������ł��Ă����Ă�΂Ƌʂ���сA�ʓ��m�����˕Ԃ�B�܂�������łƋʂ�������悤�ɂȂ��Ă���B
�����ĂȂ��Ƌʂ͔�т܂��񂱂Ƃ�B
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

//shooting�̕��̕ϐ�
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

//typing�̕��̕ϐ�
std::string name = "";//���͂��ꂽ�P���ێ�����ꎞ�I�Ȃ���
std::string dictwords[300];
std::string typedwords[300];
static int typedwordsflag(0);
static int typednotesflag(0);//���܂����Ă镶���̉��Ԗڂɂ��邩�ǂ����킩��
static int misscounta(0);

static double inputnotesxx[50] = { 0 };//�������Ă镶����x���W�B�O���t�B�b�N�p
static double inputnotesyy[50] = { 0 };

//���ʂ̕ϐ�
static int idleflag(0);
static int tt(0);

//shooting functions
double randombyrandom(int min, int max){
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(mt);
}
void newbullet(unsigned char k){//�V��������ł��̊֐�
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
	if (bnumcounta >= bulletMAX) bnumcounta = 0; //bulletMAX�ȏ�ɋʂ������Ȃ��悤�ɂ��Ă���B
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
void circles(double &xx, double &yy, double &vx, double &vy, double &ax, double &ay, int &note){//�~�̈ړ��A�ǂł̒��˕Ԃ�̌v�Z�̊֐�
	//y���W�ɂ���
	vy = vy + ay*dt;
	yy = yy + vy*dt*velocity;
	if (yy - rr  < 0){ vy = -vy; yy = 0 + rr; }
	if (yy + rr  > 600){ vy = -vy; yy = 600 - rr; note = (int)randombyrandom(48, 57); }//��ɓ��������琔����������B

	//x���W�ɂ���
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
	dx = x1 - x2; //�ǂ������㑤�ɂ��邩�������ɔ��ʂ�����
	dy = y1 - y2;
	costheta = fabs(dx) / sqrt(fabs(dx*dx + dy*dy));
	sintheta = fabs(dy) / sqrt(fabs(dx*dx + dy*dy));

	if (dx > 0){//x1�̕����E���ɂ������Ƃ�
		x1 = cx + (rr)* costheta;
		x2 = cx - (rr)* costheta;
	}
	if (dx < 0){
		x1 = cx - (rr)* costheta;
		x2 = cx + (rr)* costheta;
	}
	if (dy > 0){//y1�̕����E���ɂ������Ƃ�
		y1 = cy + (rr)* sintheta;
		y2 = cy - (rr)* sintheta;
	}
	if (dy < 0){
		y1 = cy - (rr)* sintheta;
		y2 = cy + (rr)* sintheta;
	}
}
void collision(double &v1x, double &v1y, double &v2x, double &v2y){//�~�ǂ����̒��˕Ԃ�̊֐�
	//���x�ɂ���
	double vzx, vzy;
	vzx = (fabs(v1x) + fabs(v2x)) / 2;
	vzy = (fabs(v1y) + fabs(v2y)) / 2;
	v1x = (v1x / fabs(v1x))*vzx;//�����̎��o��
	v1y = (v1y / fabs(v1y))*vzy;
	v2x = (v2x / fabs(v2x))*vzx;
	v2y = (v2y / fabs(v2y))*vzy;

	//���s����
	if (v1x != v2x){//x�����̑����̕���������Ă���
		v1x = -v1x;
		v2x = -v2x;
	}
	if (v1y != v2y){//y�����̑����̕���������Ă�����
		v1y = -v1y;
		v2y = -v2y;
	}
}

//typing functoins
void loaddictwords(){//dictionary�t�@�C���ɓ����Ă镶����ǂݍ��ށB��΂ɏ������݂͂��Ȃ��B
	using namespace std;
	cout << "::: loading... :::" << endl;
	ifstream infile;
	infile.open("EnglishDictionary.txt");//�����œǂݍ��ގ����t�@�C��������
	int flag(0);
	for (int i(0); i < 100; i++){
		if (infile.eof()){//�m�����ꂪ������̏I����ǂݍ��ނƂ��낾�����悤�ȋC������B
			flag = i - 1;
			break;
		}
		infile >> dictwords[i];//dictwords[]�ɓǂݍ��񂾂̂��Ԃ�����ł����B
		cout << dictwords[i] << endl;//�����Ɠǂݍ��܂ꂽ���̊m�F
	}
	infile.close();
	cout << "::: load succesfully!!! :::" << endl;
}
void matching(unsigned char k){
	if (k == dictwords[typedwordsflag].at(typednotesflag)){
		name += k;
		typednotesflag++;
		newbullet(k);
		if (dictwords[typedwordsflag].size() == typednotesflag){//dictwords�̒�����typednotesflag�̒l���ꏏ�ɂȂ����玟�̕����ցB
			typedwords[typedwordsflag++] = name;
			std::cout << name << std::endl;
			typednotesflag = 0;
			name = "";
		}
	}
	else{//�ԈႦ��Ɖ����Ȃ����Ⴂ�܂�����ƁBBeep�����Ȃ��Ă�Ԃ͑��̏������󂯕t���Ȃ��炵���̂Ńy�i���e�B�ɂ͂��傤�ǂ��������B
		misscounta++;
		switch (misscounta % 8){
			//case : Beep(440, 150); break; //��
			//case : Beep(466, 150); break;
			//case : Beep(494, 150); break; //�V
		case 1: Beep(523, 150); break; //�h
			//case : Beep(554, 150); break;
		case 2: Beep(587, 150); break; //��
			//case : Beep(622, 150); break;
		case 3: Beep(659, 150); break; //�~
		case 4: Beep(698, 150); break; //�t�@
			//case : Beep(740, 150); break;
		case 5: Beep(784, 150); break; //�\
			//case : Beep(831, 150); break;
		case 6: Beep(880, 150); break; //��
			//case : Beep(932, 150); break; 
		case 7: Beep(988, 150); break; //�V
		case 0: Beep(1047, 150); break; //�h
		}
	}
}

//shooting visualising
void displaybullet(){
	using namespace SDGLibF;
	//�ʂ�`��
	for (int i(0); i < bulletMAX; i++){
		SetColor(b[i].red, b[i].green, b[i].blue);
		DrawCircle(2.0, b[i].xx, b[i].yy, rr);
		char tempbuff[10];
		sprintf_s(tempbuff, "%c", b[i].note);
		DrawString(b[i].xx - 5, b[i].yy - 5, tempbuff);//����ŕ�������Ԃ悤�Ɍ�����͂��B
	}
}

//typing visualising
void displaysigniture(){//�����I�Ȃ���
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
void displaycurrenttime(){//���ݎ����̕\��
	//���ݎ����̎擾
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
void displaydictwords(){//dictwords�̕\���B
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
void displaytypedwords(){//typedwords�̕\���B
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
void displaymatching(){//���͂��������Ɠ��͉����̕\���B�^�C�s���O�Q�[�����ۂ�������Ƃ���
	using namespace SDGLibF;
	//string�^�ł̕\��
	//�i�X�Ɠ����ق�
	SetColor(0.7, 0.7, 0.7);//����̕���
	DrawString(250, 545 - typedwordsflag * 25, dictwords[typedwordsflag].c_str());
	SetColor(0, 0, 1);//���͂��Ă镶��
	DrawString(250, 545 - typedwordsflag * 25, name.c_str());
	SetColor(0, 0.9, 0);//����
	DrawString(180, 545 - typedwordsflag * 25, "type it:");
	DrawLine(2, 0, 545 - typedwordsflag * 25 - 5, 600, 545 - typedwordsflag * 25 - 5);

	//�����Ə�ɂ�����B
	SetColor(0.7, 0.7, 0.7);//����̕���
	DrawString(250, 545, dictwords[typedwordsflag].c_str());
	SetColor(0, 0, 1);//���͂��Ă镶��
	DrawString(250, 545, name.c_str());
	SetColor(0, 0.9, 0);//����
	DrawString(180, 545, "type it:");
	DrawLine(2, 180, 545 - 5, 400, 545 - 5);

	//����̕����̕�
	int wordlength = dictwords[typedwordsflag].length();
	for (int i(0); i < wordlength; i++){//�����ƒ������w�肵�Ȃ��ƃI�[�o�[�t���[���Ď~�܂����Ⴄ�̂ŋC�����܂��傤�B
		inputnotesxx[i] = 200 + 10 + i * 20;
		inputnotesyy[i] = 30;

		char tempn = dictwords[typedwordsflag].at(i);//���[�h���X�g��typedwordsflag+1�Ԗڂ̒P��́Atypednotesflag+1�Ԗڂ̕���
		char tempbuff[100];
		SetColor(0.3, 0.3, 0.3);//����̕���
		sprintf_s(tempbuff, "%c", tempn);
		DrawString(inputnotesxx[i], inputnotesyy[i], tempbuff);
	}

	//���͂��镶���̕�
	int namelength = name.length();
	for (int i(0); i < namelength; i++){
		char tempn1 = name.at(i);//���͂��Ă镶���B�㏑������
		char tempbuff1[100];
		SetColor(0.9, 0.9, 0.9);//
		sprintf_s(tempbuff1, "%c", tempn1);
		DrawString(inputnotesxx[i], inputnotesyy[i], tempbuff1);
	
		SetColor(0, 0.9, 0);//����
		DrawLine(2, inputnotesxx[i], inputnotesyy[i] - 5, inputnotesxx[i] +20, inputnotesyy[i] - 5);
	}
	SetColor(0, 0.9, 0);//type it:�̕���
	DrawString(inputnotesxx[0] - 80, inputnotesyy[0], "type it:");

}

//main visualiser
void displayfunc(){//�`��֐�
	using namespace SDGLibF;
	Before(); // Set drawing plane

	displaysigniture();
	displaycurrenttime();
	displaydictwords();
	displaytypedwords();
	displaymatching();

	displaybullet();
	int bulletsum(0);//bullet���v�̌v�Z
	for (int i(0); i < bulletMAX; i++) {
		if (b[i].xx > 0 && b[i].yy > 0) bulletsum++;
	}

	char buff[100];
	SetColor(0.3, 0.3, 0.3);
	sprintf_s(buff, "v=%3.1f,rr=%d,b=%d,miss:%d", velocity, rr, bulletsum, misscounta);
	DrawString(0, 0, buff);

	After(); // Set drawn plane
}

void simulation(void){//���ۂɓ������֐�
	using namespace SDGLibF;
	tt += 1.0;

	//�~�̍��W�̌v�Z�Ƃ�
	for (int i(0); i < bulletMAX; i++){
		circles(b[i].xx, b[i].yy, b[i].vx, b[i].vy, b[i].ax, b[i].ay, b[i].note);
	}

	//�~�ǂ����̏Փ˂̎���
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
		matching(k); break;// matching�̒���newbullet�����Ă���B

		//case VK_BACK: if (name.size() >= 1) { name.erase(name.size() - 1); typednotesflag--; } break; //����L���ɂ���a->bkspace->a->nkspace...���Ă���Ă�Ɩ��Gww

	case 48:case 49:case 50:case 51:case 52:
	case 53:case 54:case 55:case 56:case 57:
		deletebullet(k); break;//0-9�܂�

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
�\���̂̔z����g���Ă݂悤�B

141014char�^�ňꕶ������������̂ɋ�J���܂����B

�o�O
141012
�E�����ɂ�������o���ƁA����(x=0)�����ŏ����Ă����B
*/

