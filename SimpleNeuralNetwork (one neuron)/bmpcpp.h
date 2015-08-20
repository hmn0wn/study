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

	BITMAPFILEHEADER bmpFileHeader; //�������� bmpFileHeader
	BITMAPINFOHEADER bmpInfoHeader; //�������� bmpInfoHeader

	in.read((char*)&bmpFileHeader, sizeof(bmpFileHeader)); //���������� ������ 
	in.read((char*)&bmpInfoHeader, sizeof(bmpInfoHeader)); //� ���������

	const int Height = bmpInfoHeader.biHeight, //�������� ��������
		Width = bmpInfoHeader.biWidth,
		BpP = bmpInfoHeader.biBitCount,
		ImageSize = bmpFileHeader.bfSize;

	const int RowSize = BpP / 8 * Width;
	int padding = ((24 / bmpInfoHeader.biBitCount) * Width) % 4; //������� - �������� ����������� � ������� ��������
	char pixels_orig[Height][RowSize]; //������ ��������

	in.seekg(bmpFileHeader.bfOffBits, ios::beg); //������������� � ������ ������� ��������

	for (int i = 0; i < Height; i++) {
		in.read(pixels_orig[i], (RowSize * (BpP / 8))); //���������� ������ �����
		in.seekg(padding, ios::cur); //������������ �������
	}

	cout << "������ " << argv[1] << ":\n"
		<< "\t������ �����: " << ImageSize << " ������ = " << (double)ImageSize / 1024 << " ����������;\n"
		<< "\t������ �����������: " << Width << " ��������;\n"
		<< "\t������ �����������: " << Height << " ��������;\n"
		<< "\t��������: " << BpP << " ����� �� �������.\n";

	char temp;

	//������ ����� ���-�� � ��������
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

	//������� ���� out.bmp
	/*
	fstream out_create("out.bmp");

	out_create.close();

	fstream out("out.bmp", ios::in | ios::out | ios::binary);

	//���������� ������ � ���� out.bmp

	in.seekg(0, ios::beg);

	char ch;
	for (int i = 1; i <= bmpFileHeader.bfOffBits; i++) {
		in.read(&ch, 1);
		out.write(&ch, 1);
	}

	in.close();

	for (int i = 0; i < Height; i++) {
		out.write(pixels_crypted[i], (RowSize * (BpP / 8))); //���������� ������ �����
		for (int i = 1; i <= padding; i++)
			out.write(0, 1); //��������� �������
	}

	out.close();*/

	return 0;
}