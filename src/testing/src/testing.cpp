//============================================================================
// Name        : testing.cpp
// Author      : Zulhisham
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	double param1 = 1.1;
	float param2 = 2.2;
	string myname;

	cout << param1 / param2 << endl;

	cout << "enter your name : ";
	cin >> myname;
	cout << "You entered your name as : " + myname << endl;

	return 0;
}
