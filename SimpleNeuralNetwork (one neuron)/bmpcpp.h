#include <iostream> 
#include <fstream> 
#include <cstdio>
#include <windows.h>

using namespace std;

int read_bmp_cpp(char *argv)
{
	setlocale(LC_ALL, "Russian");
	
	fstream in(argv, ios::in | ios::out | ios::binary);
	if (!in) {
		cout << "File can not be opened.\n";
	}

	BITMAPFILEHEADER bmpFileHeader; //Создание bmpFileHeader
	BITMAPINFOHEADER bmpInfoHeader; //Создание bmpInfoHeader

	in.read((char*)&bmpFileHeader, sizeof(bmpFileHeader)); //Считывание данных 
	in.read((char*)&bmpInfoHeader, sizeof(bmpInfoHeader)); //в структуры

	const int Height = bmpInfoHeader.biHeight, //Создание констант
		Width = bmpInfoHeader.biWidth,
		BpP = bmpInfoHeader.biBitCount,
		ImageSize = bmpFileHeader.bfSize;

	const int RowSize = BpP / 8 * Width;
	int padding = ((24 / bmpInfoHeader.biBitCount) * Width) % 4; //Паддинг - ненужное пространсво в массиве пикселей
	char pixels_orig[Height][RowSize]; //Массив пикселей

	in.seekg(bmpFileHeader.bfOffBits, ios::beg); //Перескакиваем в начало массива пикселей

	for (int i = 0; i < Height; i++) {
		in.read(pixels_orig[i], (RowSize * (BpP / 8))); //Заполнение одного рядка
		in.seekg(padding, ios::cur); //Проскакиваем паддинг
	}

	cout << "Данные " << argv[1] << ":\n"
		<< "\tРазмер файла: " << ImageSize << " байтов = " << (double)ImageSize / 1024 << " килобайтов;\n"
		<< "\tШирина изображения: " << Width << " пикселей;\n"
		<< "\tВысота изображения: " << Height << " пикселей;\n"
		<< "\tБитность: " << BpP << " битов на пиксель.\n";

	char temp;

	//Делаем далее что-то с битмапом
	/*char pixels_crypted[Height][RowSize];

	for (int i = 0; i < RowSize; i++)
		for (int j = 0; j < Height; j++)
			pixels_crypted[j][i] = pixels_orig[j][i];*/

	/*for (int i = 0; i < RowSize; i += 2)
	for (int j = 0; j < (Height / 2); j++) {
	temp = pixels_crypted[j][i];
	pixels_crypted[j][i] = pixels_crypted[(Height - 1) - j][i];
	pixels_crypted[(Height - 1) - j][i] = temp;

	//if ()
	}*/

	//Создаем файл out.bmp
	/*
	fstream out_create("out.bmp");

	out_create.close();

	fstream out("out.bmp", ios::in | ios::out | ios::binary);

	//Начинается запись в файл out.bmp

	in.seekg(0, ios::beg);

	char ch;
	for (int i = 1; i <= bmpFileHeader.bfOffBits; i++) {
		in.read(&ch, 1);
		out.write(&ch, 1);
	}

	in.close();

	for (int i = 0; i < Height; i++) {
		out.write(pixels_crypted[i], (RowSize * (BpP / 8))); //Заполнение одного рядка
		for (int i = 1; i <= padding; i++)
			out.write(0, 1); //Заполняем паддинг
	}

	out.close();*/

	return 0;
}