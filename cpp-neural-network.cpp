// cpp-neural-network.cpp : This file contains the 'main' function.

#include <iostream>
#include <fstream>
#include "globalFunctions.h"
#include "fileClasses.h"
#include "networkClasses.h"

using namespace std;

/*GLOBAL FILE LOCATIONS*/
// Images
const string trainImagesPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-images-idx3-ubyte";
const string trainImagesFile = "/train-images-idx3-ubyte";
const string testImagesPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/t10k-images-idx3-ubyte";
const string testImagesFile = "/t10k-images-idx3-ubyte";
// Labels
const string trainLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/train-labels-idx1-ubyte";
const string trainLabelsFile = "/train-labels-idx1-ubyte";
const string testLabelsPath = "C:/Users/benhu/PycharmProjects/Data Sets/mnist-dataset/versions/1/t10k-labels-idx1-ubyte";
const string testLabelsFile = "/t10k-labels-idx1-ubyte";

int main() {
	// Get training data
    File trainingImageData = File(trainImagesPath, trainImagesFile);
    File trainingLabelData = File(trainLabelsPath, trainLabelsFile);

    int start_index = 0;   // image index to start on
    int end_index = 60000; // image index to finish on
    int batch_size = 16;   // batch size for weight / bias adjustment
    int epoch = 3;         // epoch number, how many times to review dataset
	float alpha = 0.1;     // learning rate

    vector<Image> training_images = trainingImageData.getImageVector(start_index, end_index);
    vector<char> training_labels = trainingLabelData.getLabelVector(start_index, end_index);
    int final_layer_count = 10; // labels are 0, 1, 2, ... , 9 

    // zip together the training images and their corresponding labels
    for (int i = start_index; i < end_index; i++) {
        training_images[i].setLabel(training_labels[i]);
    }

    // create the neural network
    NeuralNetwork nn = NeuralNetwork(
        {
            trainingImageData.image_size,  // input (layer zero)
            80,						       // layer one
            20,						       // layer two
            final_layer_count              // final layer (layer three)
        },
        RElU_const,                        // activation to use
		alpha							   // learning rate
    );

    // train the neural network
    nn.trainVector(
        training_images,
        batch_size,
        epoch
    );

    // Get test data
    File testImageData = File(testImagesPath, testImagesFile);
    File testLabelData = File(testLabelsPath, testLabelsFile);

    int start_index_test = 0;
    int end_index_test = 10000;

    vector<Image> test_images = testImageData.getImageVector(start_index_test, end_index_test);
    vector<char> test_labels = testLabelData.getLabelVector(start_index_test, end_index_test);

    // zip together the test images and their corresponding labels
    for (int i = start_index_test; i < end_index_test; i++) {
        test_images[i].setLabel(test_labels[i]);
    }
    
    // Test the new parameters on the training set
    nn.testVector(test_images);

    // Save network as a JSON object
    nn.toJson();

	return 0;
}