#include "ImageProcessModule.h"
#include <iostream>
#include <gtest/gtest.h>

#define CAMERA_ID 0


TEST(DefaultConstructor_Test, Default)
{

	ImageProcessModule test;

	EXPECT_EQ(127, test.getThresholdValue());

	EXPECT_TRUE( NULL != test.getSourceImage());
	EXPECT_TRUE( NULL != test.getBinaryImage());
	EXPECT_TRUE( NULL != test.getGrayscaleImage());

	EXPECT_TRUE(NULL != test.getCamera());
}

TEST(DefaultConstructor_Test, PassArgument_ThresholdValue)
{

	int setValue = rand()%256;
	ImageProcessModule test(setValue);

	EXPECT_EQ(setValue, test.getThresholdValue());

}


TEST(setThresholdValue_Test, positiveArgument)
{
	ImageProcessModule test;
	int argument = rand()%256;

	test.setThresholdValue(argument);

	EXPECT_EQ(argument, test.getThresholdValue());
}

TEST(setThresholdValue_Test, positiveArgument_Out_Of_Range)
{
	ImageProcessModule test;
	int argument = rand()+256;

	test.setThresholdValue(argument);

	EXPECT_EQ(255, test.getThresholdValue());
}

TEST(setThresholdValue_Test, negativeArgument)
{
	ImageProcessModule test;
	int argument = (-1)*rand();

	test.setThresholdValue(argument);

	EXPECT_EQ(0, test.getThresholdValue());
}


TEST(connectToCamera_Test, attachedCameraID)
{
	ImageProcessModule test;

	EXPECT_TRUE(test.connectToCamera(CAMERA_ID));
}

TEST(connectToCamera_Test, notAttachedCameraID)
{
	ImageProcessModule test;

	EXPECT_FALSE(test.connectToCamera(10));
}

TEST(connectToCamera_Test, negativeCameraID)
{
	ImageProcessModule test;

	EXPECT_FALSE(test.connectToCamera(-1));
}

TEST(grabImage_Test, default)
{
	ImageProcessModule test;

	EXPECT_TRUE(test.connectToCamera(CAMERA_ID));
	EXPECT_TRUE(test.grabImage());

}

TEST(imagePreProcessing, default)
{

	ImageProcessModule test;

	test.connectToCamera(CAMERA_ID);
	test.setThresholdValue(50);
	while(true)
	{
		test.grabImage();
		test.imagePreProcessing();
		test.imageSegmentation();
		cv::imshow("obraz", *(test.getBinaryImage()));
		cv::waitKey(25);
	}


}

int main(int argc, char* argv[])
{


	std::cout << "HEllo World" << std::endl;

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
