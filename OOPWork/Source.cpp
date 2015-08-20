#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include "Classes.h"
using namespace std;

void main(int argc, char *argv[]){
	
	if (argc == 2 && !strcmp(argv[1], "help")){
		/*cout << "Запуск программы происходит при использовании параметра командной стрки \"run\"" << endl;
		cout << "Данные для демонстрационного примера берутся из файла input.txt." << endl;
		cout << "В первой строке данные для заполнения полей сервера, он один, определен программно." << endl;
		cout << "Во второй колличесто компьютеров для ввода в сервер, не должно превосходить SIZE." << endl;
		cout << "Далее следуют текстовые специффикаторы pc и laptop." << endl;
		cout << "А за ними соответствеющие данные." << endl;*/

		cout << endl << endl << "\tATTENTION!!!" << endl;
		cout << "Run occurs when you use the command line \" run \"" << endl;
		cout << "Data for the demo, taken from the file input.txt" << endl;
		cout << "The first line of data to populate the server, server is defined in program code" << endl;
		cout << "The second Nuber computers for input to the server, should not exceed SIZE" << endl;
		cout << "Followed by text parameters pc and laptop" << endl;
		cout << "And behind them the relevant data" << endl;
		cout << endl;
	}
	else if (argc == 2 && !strcmp(argv[1], "run")){

		
		char ip[65];
		int hard_disc;
		char screen_type[65];
		long colors;
		int x_res;
		int y_res;
		int processor;
		int speed;
		int ram;
		int comp_count;

		char comp_type[10];

		int m_dpi;
		bool tch;



		ifstream in("input.txt");
		if (!in){
			cout << "Can not open file." << endl;
		}
		else{
			in >> ip >> hard_disc >> screen_type >> colors >> x_res >> y_res >> processor >> speed >> ram;
			server serv(ip, hard_disc, screen_type, colors, x_res, y_res, processor, speed, ram);

			computer tmpcomp;
			in >> comp_count;

			//Далее следует заполнение сервера компьютерами.
			//Это классический пример динамического полиморфизма, посылки к такому его применению положены
			//в классе компьютер который является предком классов пк и лаптоп.
			//По текстовому специффикатору происходит заполнение массива соответствующими элементами, тип которых
			//выбирается динамически, но насмотря на это --->
			for (int i = 0; i < comp_count; i++){
				in >> comp_type;
				if (!strcmp(comp_type, "pc")){
					in >> m_dpi >> hard_disc >> screen_type >> colors >> x_res >> y_res >> processor >> speed >> ram;
					tmpcomp = *(new pc(m_dpi, hard_disc, screen_type, colors, x_res, y_res, processor, speed, ram));
					serv.add(tmpcomp);
				}
				if (!strcmp(comp_type, "laptop")){
					in >> tch >> hard_disc >> screen_type >> colors >> x_res >> y_res >> processor >> speed >> ram;
					tmpcomp = *(new laptop(tch, hard_disc, screen_type, colors, x_res, y_res, processor, speed, ram));
					serv.add(tmpcomp);
				}

			}
			
			cout << endl << "Server: " << endl;
			serv.show_properties(cout);
			//--->несмотря на это, метод для вывода разнотипных элементов один и тот же,
			//он реализован еще в классе предке, в компьютере, а объявлен как виртуальный, еще в базовом
			//классе
			cout << endl << serv;
		}

		in.close();
	}
	
	
}