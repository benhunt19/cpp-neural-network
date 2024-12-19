// cpp-neural-network.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include "header.h"
#include "globalClasses.h"

using namespace std;

/*GLOBALS*/
// Images
const string trainImagesPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-images-idx3-ubyte";
const string trainImagesFile = "/train-images-idx3-ubyte";
// Labels
const string trainLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-labels-idx3-ubyte";
const string trainLabelsFile = "/train-labels-idx3-ubyte";

int main()
{
	File imagesTest = File(trainImagesPath, trainImagesFile);
	imagesTest.printImageAtIndex(210);
	imagesTest.getImage(10);
	return 1;
}