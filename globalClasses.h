#pragma once
#include <iostream>

using namespace std;

class TestClass {
	public:
		int number;
		TestClass() {
			cout << "you have created a new test class" << endl;
		}
		TestClass(int num) {
			cout << "you have created a test class with an integer param..." << endl;
			number = num;
			cout << "number: " << number << endl;
		}
		static void testStatic() {
			cout << "This is the output of a static class to see if we can get it anywhere..." << endl;
		}
};
