#include "exerciser.h"

void exercise(connection *C)
{
  // cout << "\033[0;31m query1 : 1 \033[0m" << endl;
  query1(C, 1, 35, 40, 0, 0, 0, 0, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  // cout << "\033[0;31m query1 : 2 \033[0m" << endl;
  query1(C, 0, 35, 40, 0, 0, 0, 1, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  // cout << "\033[0;31m query2 : 1 \033[0m" << endl;
  query2(C, "LightBlue");
  // cout << "\033[0;31m query2 : 2 \033[0m" << endl;
  query2(C, "Orange");
  // cout << "\033[0;31m query3 : 1 \033[0m" << endl;
  query3(C, "Duke");
  // cout << "\033[0;31m query3 : 2 \033[0m" << endl;
  query3(C, "GeorgiaTech");
  // cout << "\033[0;31m query4\033[0m" << endl;
  query4(C, "NC", "DarkBlue");
  // cout << "\033[0;31m query5 \033[0m" << endl;
  query5(C, 11);
}
