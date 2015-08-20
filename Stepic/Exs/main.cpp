#include <QCoreApplication>
#include <iostream>
#include "exp.hpp"

using namespace std;


int main()
{
  int a[2] = {7,8};
  int b[2] = {0,1};
  std::cout << "  a: " << a << std::endl;
  for(size_t i = 0; i<2; ++i)
    std::cout << a[i] << std::endl;

  int* b_not_const = const_cast<int *>(b);
  a[1] = 99;
  std::cout << "  b: " << b << std::endl;
  for(size_t i = 0; i<2; ++i)
    std::cout << b[i] << std::endl;

  std::cout << "  b_not_const: " << b_not_const << std::endl;
  for(size_t i = 0; i<2; ++i)
    std::cout << b_not_const[i] << std::endl;

  std::cout << "sizeof a: " << sizeof(a) << std::endl;
  std::cout << "sizeof b: " << sizeof(b) << std::endl;
  std::cout << "sizeof b_not_const: " << sizeof(b_not_const) << std::endl;


}
