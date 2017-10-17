/*
 * main.cpp
 *
 *  Created on: Oct 17, 2017
 *      Author: witek
 */
#include "ImageProcessModule.h"
#include <iostream>

using namespace std;



int main(int argc, char **argv) {

	ImageProcessModule *ImgProcWorker = new ImageProcessModule();

	cout << " Thres: " << ImgProcWorker->getThresholdValue() << endl;

	ImgProcWorker->setThresholdValue(300);

	cout << " Thres: " << ImgProcWorker->getThresholdValue() << endl;




}




