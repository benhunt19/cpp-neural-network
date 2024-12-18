// cpp-neural-network.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include "header.h"
#include "globalClasses.h"

using namespace std;

/*GLOBALS*/

const string path = "C:/Users/benhu/.cache/kagglehub/datasets/hojjatk/mnist-dataset/versions/1/train-images-idx3-ubyte";
const string filename = "/train-images-idx3-ubyte";

int main()
{
	ifstream stream = FileUtils::getFileFromPath(path, filename);
	cout << stream.peek() << endl;
	return 1;
}