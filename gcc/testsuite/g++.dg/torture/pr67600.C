// { dg-do run }
// { dg-skip-if "requires hosted libstdc++ for iostream" { ! hostedlib } }

#include <iostream>
#include <sstream>

int main()
{
  std::basic_ostringstream<char> ostr;
  std::ostreambuf_iterator<char> iter(ostr.rdbuf());
  *iter++ = 'X';


  std::cout << ostr.str() << '\n';
  return 0;
}
