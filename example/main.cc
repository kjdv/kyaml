#include <iostream>
#include "kyaml.hh"

using namespace std;
using namespace kyaml;

int main()
{
  shared_ptr<document> doc = kyaml::parse(std::cin);

  if(doc)
    cout << doc << '\n';

  else
  {
    cerr << "error\n";
    return 1;
  }

  return 0;
}
