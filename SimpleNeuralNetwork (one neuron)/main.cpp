#include <iostream>
#include <fstream>
#include <string>
#include "bmp.h"
//#include "bmpcpp.h"
#include <time.h>
#include <stdlib.h>

using namespace std;

void print_2darray(int **out, int sizex, int sizey){
	for (int i = 0; i < sizex; i++){
		cout << endl << " ";
		for (int j = 0; j < sizey; j++)
			cout << out[i][j] << " ";
	}
	cout << endl;
}

class Web{
public:
	int** mul;//Масштабир сигналы
	int** weight;//Массив весов
	int sizex;
	int sizey;
	int** input;//Входная инф
	int limit = 9;//Эксперементально выбранный порог
	int sum;

	//Обучающее множество
	int learning_set_size;
	string*  learning_set_imgs = new string[learning_set_size];
	int* learning_set_ansv;
	
	Web(int sizex_, int sizey_, int** weight_, int l_s_s, string* l_s_i, int* l_s_a){//Еще необходимо получать входные данные input.
		sizex = sizex_;
		sizey = sizey_;
		learning_set_size = l_s_s;
		learning_set_imgs = l_s_i;
		learning_set_ansv = l_s_a;
		sum = 0;
		
		weight = new int*[sizex];
		mul = new int*[sizex];
		//input = new int*[sizex];

		for (int i = 0; i < sizex; i++){
			weight[i] = new int[sizey];
			mul[i] = new int[sizey];
			//input[i] = new int[sizey];
		}

		for (int i = 0; i < sizex; i++)
			for (int j = 0; j < sizey; j++)
				weight[i][j] = weight_[i][j]; 
	
	
	
	}
		

	//Масштабирование
	void mul_w(){
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){//Пробегаем по каждому аксону
				mul[i][j] = input[i][j] * weight[i][j];//Умножаем его сигнал(0 или 1) на его собственный вес и сохраняем в массив
			}
		}
	}

	//Сложение
	void sum_(){
		sum = 0;
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){
				sum +=mul[i][j];
			}
		}
	}

	//Сравнение
	bool rez(){
		if (sum >= limit) return true;
		else return false;
	}

	//Вывод весов
	void weight_print(){
		cout << " Weights:";
		print_2darray(weight, sizex, sizey);
	}

	//Распознаем символ
	bool recognize(){
		this->mul_w();
		this->sum_();
		if (this->rez()){
			//cout << "True, Sum = " << this->sum << endl;
			return true;
		}
		else{
			//cout << "False, Sum = " << this->sum << endl;
			return false;
		}
	}

	//Наказываем сеть за неверное распознавание
	//Если неправильный ответ False, то прибавляем к ножке значение в точке картинки
	void incW(){
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){
				weight[i][j] += input[i][j];
			}
		}
	}

	//Если неправильный ответ True, то отнимаем у ножки значение в точке картинки
	void decW(){
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){
				weight[i][j] -= input[i][j];
			}
		}
	}

	void onelearn(int i_img, int f_img){
		int** img;
		img = read_bmp(learning_set_imgs[i_img].c_str());
		print_2darray(img, sizex, sizey);
		cout << endl;
		//Подаем на вход нейрона изображение
		this->input = img; //Утечка памяти img input

		bool ansv = this->recognize();//Получаем ответ нейрона
		cout <<" "<<learning_set_ansv[f_img] << " vs " << learning_set_imgs[i_img] << " : ";

		if (ansv != (learning_set_ansv[f_img] == learning_set_ansv[i_img])){//Если ошибся, то меняем веса					
			if (ansv) this->decW();
			else this->incW();
			cout << " Wrong<---------" << endl;
		}
		else cout << " Right" << endl;
	}


	void learn(){

		//Выбираем для обучения изображение
		int fix_img = 5;
		srand(time(NULL));
		int iter;

		for (int j = 0; j < 50; j++){//Прогоним обучение
			cout << "iter: " << j << endl;			
			iter = rand() % learning_set_size;//Случайным образом берем значение из обуч мн-ва.		
			onelearn(iter, fix_img);
			onelearn(fix_img, fix_img);		
			cout << endl;
		}
		this->weight_print();

		//Проверяем на всех образах
		cout << endl << " Check all imgs:" << endl;
		for (int i = 0; i < learning_set_size; i++){
			onelearn(i, fix_img);
		}
	}

};

void main(){
	ifstream in("weights.txt");
	if (!in) {
		cout << "File can not be opened" << endl;
		return;
	}

	int sizex = 5;
	int sizey = 3;
	int** weifromf = new int*[sizex];
	int** input;
	int tmp;

	for (int i = 0; i < sizex; i++){
		weifromf[i] = new int[sizey];
		for (int j = 0; j < sizey; j++)
			in >> weifromf[i][j];
	}
	in.close();

	//Считываем обучающее множество
	
	
	in.open("images_for_learning.txt", ios::in);
	if (!in) {
		cout << "File can not be opened" << endl;
		return;
	}
	int l_s_s;
	in >> l_s_s;
	string*  l_s_i = new string[l_s_s];
	int* l_s_a = new int[l_s_s];

	for (int i = 0; i < l_s_s; i++){
		in >> l_s_i[i];
		in >> l_s_a[i];
		
		l_s_i[i] = "imgs/" + l_s_i[i]+ ".bmp";
		cout << l_s_i[i] << " ";
		cout << l_s_a[i] << endl;
	}
	in.close();

	

	//Создаем сеть(из одного нейрона)
	//Выставляем из файла веса
	//Подаем на вход обучающее множество
	Web* NW1 = new Web(sizex, sizey, weifromf, l_s_s, l_s_i, l_s_a);
	NW1->learn();
	


	


	

}