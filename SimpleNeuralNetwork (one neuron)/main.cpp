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
	int** mul;//��������� �������
	int** weight;//������ �����
	int sizex;
	int sizey;
	int** input;//������� ���
	int limit = 9;//���������������� ��������� �����
	int sum;

	//��������� ���������
	int learning_set_size;
	string*  learning_set_imgs = new string[learning_set_size];
	int* learning_set_ansv;
	
	Web(int sizex_, int sizey_, int** weight_, int l_s_s, string* l_s_i, int* l_s_a){//��� ���������� �������� ������� ������ input.
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
		

	//���������������
	void mul_w(){
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){//��������� �� ������� ������
				mul[i][j] = input[i][j] * weight[i][j];//�������� ��� ������(0 ��� 1) �� ��� ����������� ��� � ��������� � ������
			}
		}
	}

	//��������
	void sum_(){
		sum = 0;
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){
				sum +=mul[i][j];
			}
		}
	}

	//���������
	bool rez(){
		if (sum >= limit) return true;
		else return false;
	}

	//����� �����
	void weight_print(){
		cout << " Weights:";
		print_2darray(weight, sizex, sizey);
	}

	//���������� ������
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

	//���������� ���� �� �������� �������������
	//���� ������������ ����� False, �� ���������� � ����� �������� � ����� ��������
	void incW(){
		for (int i = 0; i < sizex; i++){
			for (int j = 0; j < sizey; j++){
				weight[i][j] += input[i][j];
			}
		}
	}

	//���� ������������ ����� True, �� �������� � ����� �������� � ����� ��������
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
		//������ �� ���� ������� �����������
		this->input = img; //������ ������ img input

		bool ansv = this->recognize();//�������� ����� �������
		cout <<" "<<learning_set_ansv[f_img] << " vs " << learning_set_imgs[i_img] << " : ";

		if (ansv != (learning_set_ansv[f_img] == learning_set_ansv[i_img])){//���� ������, �� ������ ����					
			if (ansv) this->decW();
			else this->incW();
			cout << " Wrong<---------" << endl;
		}
		else cout << " Right" << endl;
	}


	void learn(){

		//�������� ��� �������� �����������
		int fix_img = 5;
		srand(time(NULL));
		int iter;

		for (int j = 0; j < 50; j++){//�������� ��������
			cout << "iter: " << j << endl;			
			iter = rand() % learning_set_size;//��������� ������� ����� �������� �� ���� ��-��.		
			onelearn(iter, fix_img);
			onelearn(fix_img, fix_img);		
			cout << endl;
		}
		this->weight_print();

		//��������� �� ���� �������
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

	//��������� ��������� ���������
	
	
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

	

	//������� ����(�� ������ �������)
	//���������� �� ����� ����
	//������ �� ���� ��������� ���������
	Web* NW1 = new Web(sizex, sizey, weifromf, l_s_s, l_s_i, l_s_a);
	NW1->learn();
	


	


	

}