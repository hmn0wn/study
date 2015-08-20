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

class SimpleNeuron{
public:
	int state;

	int** mul;//Масштабир сигналы
	int** weight;//Массив весов
	int sizex;
	int sizey;
	int** input;//Входная инф
	int limit = 9;//Эксперементально выбранный порог
	int sum;	
	
	SimpleNeuron(int sizex_, int sizey_, int** weight_){//Еще необходимо получать входные данные input.
		state = 0;
		sizex = sizex_;
		sizey = sizey_;
		weight = weight_;
		
		sum = 0;
		
		//weight = new int*[sizex];
		mul = new int*[sizex];
		//input = new int*[sizex];

		for (int i = 0; i < sizex; i++){
			//weight[i] = new int[sizey];
			mul[i] = new int[sizey];
			//input[i] = new int[sizey];
		}

		/*for (int i = 0; i < sizex; i++)
			for (int j = 0; j < sizey; j++)
				weight[i][j] = weight_[i][j]; */
	
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
			sum += state;
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

	
};

class SimpleNet{
public:
	int range;
	SimpleNeuron **neurons;
	int sizex;
	int sizey;

	//Обучающее множество
	int learning_set_size;
	string*  learning_set_imgs ;
	int** learning_set_ansv ;

	SimpleNet(int range_, int sizex_, int sizey_){
		sizex = sizex_;
		sizey = sizey_;
		range = range_;

		ifstream in("weights.txt");
		if (!in) {
			cout << "File can not be opened" << endl;
			return;
		}

		//Создаем слой нейронов и заодно заполняем их веса из файла
		neurons = new SimpleNeuron*[range];
		for (int i = 0; i < range; i++){

			int** weifromf = new int*[sizex];

			for (int i = 0; i < sizex; i++){
				weifromf[i] = new int[sizey];
				for (int j = 0; j < sizey; j++)
					in >> weifromf[i][j];
			}
			neurons[i] = new SimpleNeuron(sizex, sizey, weifromf);
			//cout << "Neuron : " << i << endl;
			//neurons[i]->weight_print();
		}
		in.close();

		int l_s_s;
		//Считываем обучающее множество
		in.open("images_for_learning.txt", ios::in);
		if (!in) {
			cout << "File can not be opened" << endl;
			return;
		}
		in >> l_s_s;
		learning_set_size = l_s_s;
		learning_set_imgs = new string[learning_set_size];
		learning_set_ansv = new int*[learning_set_size];	
		
		int ansv;		
		for (int i = 0; i < learning_set_size; i++){
			in >> learning_set_imgs[i];
			in >> ansv;
			learning_set_ansv[i] = new int[range];
			for (int j = 0; j < range; j++)
				learning_set_ansv[i][j] = -1;
			learning_set_ansv[i][ansv] = ansv;
			

			learning_set_imgs[i] = "imgs/" + learning_set_imgs[i] + ".bmp";
			//cout << learning_set_imgs[i] << " ";
			//cout << learning_set_ansv[i][ansv] << endl;
		}
		in.close();
		//print_2darray(learning_set_ansv, learning_set_size, range);
	}

	bool onelearn(int i_img){
		
		
		int** img;
		img = read_bmp(learning_set_imgs[i_img].c_str());
		print_2darray(img, sizex, sizey);
		bool check = 1;

		for (int i = 0; i < range; i++){


			//Подаем на вход нейрона изображение
			this->neurons[i]->input = img; //Утечка памяти img input
			bool ansv = this->neurons[i]->recognize();//Получаем ответ нейрона

			cout << "Neuron(" << i << ") vs " << learning_set_imgs[i_img] << " : ";

			if (ansv != (learning_set_ansv[i_img][i] == i)){//Если ошибся, то меняем веса					
				if (ansv) this->neurons[i]->decW();
				else this->neurons[i]->incW();
				cout << " Wrong<---------" << endl;
				this->neurons[i]->state = 0;
				check = 0;
			}
			else cout << " Right" << endl;
			this->neurons[i]->state = 1;
		}
		return check;
	}


	void learn(){
		
		//Выбираем для обучения изображение		
		srand(time(NULL));
		int iter;

		for (int j = 0; j < 100; j++){//Прогоним обучение
			//cout << "iter: " << j << endl;			
			iter = rand() % (learning_set_size-8);//Случайным образом берем значение из обуч мн-ва.		
			onelearn(iter);			
			cout << endl;

			if (default_check()) break;
		}
		//this->weight_print();

		
		
	}

	bool default_check(){
		//Проверяем на всех образах
		bool check = 1;
		cout << endl << " Check all imgs:" << endl;
		for (int i = 0; i < learning_set_size-8; i++){
			check *= onelearn(i);
			cout << endl;
		}
		return check;
	}

};

void main(){	

	//Создаем сеть(из одного нейрона)
	//Выставляем из файла веса
	//Подаем на вход обучающее множество
	SimpleNet* SN = new SimpleNet(10,5,3);
	//for (int i = 0; i < SN->range; i++)
		//SN->onelearn(i);

	SN->learn();
	bool check = 1;
	cout << endl << " Check all imgs:" << endl;
	for (int i = 0; i < SN->learning_set_size; i++)
		SN->onelearn(i);
		cout << endl;

}