/* assert example */
#include <stdio.h>      /* printf */
#include <assert.h>     /* assert */
#include <iostream>

void print_number(int* myInt) {
  assert (myInt!=NULL);
  printf ("%d\n",*myInt);
}

int factorial(int n)
{
  assert(n>=0);
  assert(n<=10);
  return n==0 ? 1: factorial(n-1) * n;
}

int main__ ()
{
  for(int i=1; i<99; ++i)
    std::cout << i <<" : " << factorial(i) << std::endl;

  return 0;
}
