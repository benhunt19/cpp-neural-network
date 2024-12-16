#include <iostream>
#include <vector>
#include <string>

using namespace std;

void testFunc() {
	vector<int> test = { 1,2,3,4,5,6,7,8 };
	for (auto i : test) {
		cout << i << endl;
	}

	string testString = "THis is a test string";
	for (auto j : testString) {
		cout << j << endl;
	}

	int x[20] = {1,2,3,4,5};

	cout << x << endl;

	vector<int>::iterator iter = test.begin();

	cout << "Start iter test" << endl;
	while (iter != test.end()) {
		cout << *(iter) << endl;
		iter++;
	}
	
	
};