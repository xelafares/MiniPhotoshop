#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include "Image_Class.h"

// ask the user if they want to load a new image
bool wantToLoadNew()
{
    while(true)
    {
        std::string choice{};
        std::cout << "do You want to load a new image?(yes/no): " << std::flush;
        std::getline(std::cin >> std::ws, choice);
        for (int i = 0; i < choice.length(); ++i) {
            choice[i] = std::tolower(choice[i]);
        }
        if(choice == "yes")
        {
            return true;
        }
        else if(choice == "no")
        {
            return false;
        }
        std::cout << "Please enter a valid option" << std::endl;
    }
}
//take the new image name to save it or overwrite the same image
bool savedBefore = false;
int saveImage(Image& image)
{
    std::string imageName{};
    std::cout << "Please enter the new image name to save it" << std::flush;
    std::cout << " Or type the same name to overwrite it: " << std::flush;
    std::getline(std::cin >> std::ws, imageName);
    try {
        image.saveImage(imageName);
    }
    catch (std::invalid_argument& e) {
        std::cerr << std::flush;
        std::cout << e.what() << std::endl;
        // stop the program for 100ms,
        // because std::cerr and std::cout of the new iteration get printed on the same line
        // which affects the formatting, so print the message from std::cerr the stop a bit then continue
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return -1;
    }
    return 1;
}

void save(Image& image)
{
    int imageStatus{};
    std::string saveOption{};
    do {
        std::cout << "Do you want to save the current image or discard it? (save/discard): " << std::flush;
        std::cin >> saveOption;
        for (int i = 0; i < saveOption.length(); ++i) {
            saveOption[i] = std::tolower(saveOption[i]);
        }
        if(saveOption == "save")
        {
            do {
                imageStatus = saveImage(image);
                savedBefore = true;
            } while (imageStatus == -1);
            return;
        }
        else if(saveOption == "discard")
        {
            savedBefore = false;
            return;
        }
        else
        {
            std::cout << "Please enter a valid option" << std::endl;
        }
    } while (saveOption != "save" || saveOption != "discard");
}

void resizeImage(Image& image, double xResize = 0, double yResize = 0)
{
    Image resizedImage(xResize, yResize);
    // convert the new dimensions into ratio
    xResize = std::ceil(double(xResize) / image.width * 100) / 100;
    yResize = std::ceil(double(yResize) / image.height * 100) / 100;


    int originalX{};
    int originalY{};
    for (int i = 0; i < resizedImage.width; ++i) {
        for (int j = 0; j < resizedImage.height; ++j) {
            // get the corresponding mapping between the new image and the original one
            originalX = std::floor(i / xResize);
            originalY = std::floor(j / yResize);
            if (originalX < 0)
            {
                originalX = 0;
            }
            else if (originalX > image.width - 1)
            {
                originalX = image.width - 1;
            }
            if (originalY < 0)
            {
                originalY = 0;
            }
            else if (originalY > image.height - 1)
            {
                originalY = image.height - 1;
            }
            for (int k = 0; k < 3; ++k) {
                resizedImage(i, j, k) = image(originalX, originalY, k);
            }
        }
    }
    std::swap(image.imageData, resizedImage.imageData);
    std::swap(image.width, resizedImage.width);
    std::swap(image.height, resizedImage.height);
}

// gray scale filter by getting the average of the pixel channels then assigning the average to those channels
void grayScale(Image& image)
{
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            unsigned  int avg = 0;

            for (int k = 0; k < 3; ++k) {
                avg += image(i, j, k);
            }

            avg /= 3; // Calculate average

            // Set all channels to the average value
            image(i, j, 0) = avg;
            image(i, j, 1) = avg;
            image(i, j, 2) = avg;
        }
    }

}

void brightenDarkenImage(Image& image, double multiplier){
    int color;

    //loop through each pixel and apply brightness multiplier

    for (int i = 0; i < image.width; ++i){

        for (int j = 0; j < image.height; ++j){
            
            for (int k = 0; k < 3; ++k){

                color = image(i, j, k);
                color *= multiplier;

                //make sure color value dont exceed 255 and not go below 0
                if (color >= 255){
                    color = 255;
                }

                else if (color <= 0){
                    color = 0;
                }

                else{
                    image(i, j, k) = color;
                }

            }
        }
    }
}

std::string brightenDarkenChoice(){
    //take choice from user to brighten or darken
    //and return value based on choice
    std::string choice{};
    while (true)
    {
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "| Would you like to Brighten or Darken your image? |" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "|1. Brighten.                                      |" << std::endl;
        std::cout << "|2. Darken.                                        |" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if("1" <= choice && choice <= "2" && choice.length() == 1)
        {
            return choice;
        }
        std::cout << "Please enter a valid choice" << std::endl;
    }
}

double brightenDarkenPercent(std::string choice) {
    //Ask user for percentage of brightening or darkening, and return the percentage
    std::string percent;
    while (true) {
        std::cout << "How much do you want to " << choice << " your image by percent?" << std::endl;
        std::cout << "Enter a percent number between 0 and 100: ";
        std::cin >> percent;
        
        try {
            int value = std::stoi(percent);
            if (value >= 0 && value <= 100) {
                return value;
            }
            else {
                std::cout << "Please enter a valid number between 0 and 100." << std::endl;
            }
        }
        catch (std::invalid_argument const&) {
            std::cout << "Invalid input. Please enter a valid number." << std::endl;
        }
    }
}

void brightenOrDarken(Image& image)
{
    std::string choice;
    int color;
    double percent;
    double multiplierValue;

    //take choice from user to brighten or darken
    choice = brightenDarkenChoice();

    //if choice is brighten ask for percentage of brightening and update multiplier
    if (choice == "1"){
        percent = brightenDarkenPercent("brighten");
        multiplierValue = 1 + (percent / 100);
    }
    //if choice is darken ask for percentage of darkening and update multiplier
    else{
        percent = brightenDarkenPercent("darken");
        multiplierValue = percent / 100;
    }

    brightenDarkenImage(image, multiplierValue);
    save(image);
}

void edgeDetect(Image& image){
    //convert image to grayscale to improve edge detection
    grayScale(image);

    //create new image template to output final image
    Image finalImage(image.width, image.height);

    //set threshhold for gradient
    int threshhold = 200;
    
    //loop through each pixel and calculate diffrence in gradient between it and surrounding pixels
    for(int i = 1; i < image.width - 1; ++i){

        for(int j = 1; j < image.height - 1; ++j){

            //use Sobel operators to calculate horizontal (X) and vertical (Y) gradients
            double gradX = (image(i - 1, j - 1, 0) * -1) + (image(i - 1, j, 0) * -2) + (image(i - 1, j + 1, 0) * -1)
                           + (image(i + 1, j - 1, 0)) + (image(i + 1, j, 0) * 2) + (image(i + 1, j + 1, 0));

            double gradY = (image(i - 1, j - 1, 0)) + (image(i, j - 1, 0) * 2) + (image(i + 1, j - 1, 0))
                           + (image(i - 1, j + 1, 0) * -1) + (image(i, j + 1, 0) * -2) + (image(i + 1, j + 1, 0) * -1);
                           
            //Find final gradient
            double grad = sqrt(pow(gradX, 2) + pow(gradY, 2));

            //if gradient > threshhold edge color is set to black, else edge color is set to white
            int edgeValue = grad > threshhold ? 0 : 255;

            //loop through each pixel in Final image and stamp edge color to it
            for (int k = 0; k < 3; ++k){

                finalImage(i, j, k) = edgeValue;

            }
        }
    }

    std::swap(image.imageData, finalImage.imageData);
    std::swap(image.width, finalImage.width);
    std::swap(image.height, finalImage.height);
    save(image);
}

void mergeCrop(Image& image, int& width, int& height){
    Image newImage(width, height);

    //loop through the image pixels and take the desired width and height
    //then set it to the new image's pixels
    for (int i = 0; i < width; ++i){

        for (int j = 0; j < height; ++j){

            for (int k = 0; k < 3; ++k){
                newImage(i, j, k) = image(i, j, k);
            }
        }
    }
    //swap datas of new image and image for saving image
    std::swap(image.imageData, newImage.imageData);
    std::swap(image.width, newImage.width);
    std::swap(image.height, newImage.height);
}

void mergeNewImage(Image& image){
    //take second image from user
    while(true){
        std::string imageName{};
        std::cout << "Please enter the second image name: " << std::flush;
        std::getline(std::cin >> std::ws, imageName);
        try {
            image.loadNewImage(imageName);
        }
        catch (std::invalid_argument& e) {
            std::cerr << std::flush;
            std::cout << e.what() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        break;
    }
}

void merge(Image& image1, Image& image2, int width, int height){
    int color, avg;

    //loop through each pixel's color on both images, calculate their avarage, and set
    //color for image 1 as the avarage color to preform merge to that color
    for (int i = 0; i < width; ++i){

        for (int j = 0; j < height; ++j){

            for (int k = 0; k < 3; ++k){

                color = image1(i, j, k) + image2(i, j, k);
                avg = color / 2;
                image1(i, j, k) = avg;
            }
        }
    }
}

std::string mergeChoice(){
    std::string choice;

    //ask user if how he wants to merge both images
    while (true){
        std::cout << "-------------------------------------------------------" << std::endl;
        std::cout << "|       How would you like to merge your images?       |" << std::endl;
        std::cout << "-------------------------------------------------------|" << std::endl;
        std::cout << "| 1. Resize dimentions of images to largest and merge. |" << std::endl;
        std::cout << "| 2. Resize dimentions of images to smallest and merge.|" << std::endl;
        std::cout << "-------------------------------------------------------|" << std::endl;
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        //return value based on user choice
        if ("1" <= choice && choice <= "2" && choice.length() == 1){
            return choice;
        }
        std::cout << "Please enter a valid choice." << std::endl;
    }
}

void mergeImages(Image& image){
    Image image2;
    //get second image from user
    mergeNewImage(image2);

    //get choice from user
    std::string choice = mergeChoice();

    int minWidth = std::min(image.width, image2.width);
    int minHeight = std::min(image.height, image2.height);

    int maxWidth = std::max(image.width, image2.width);
    int maxHeight = std::max(image.height, image2.height);

    //if choice is 1, then resize both images to largest dimensions and preform merge
    if (choice == "1"){
        resizeImage(image, maxWidth, maxHeight);
        resizeImage(image2, maxWidth, maxHeight);
        merge(image, image2, maxWidth, maxHeight);
    }
    //if choice is 2, then crop both images to smallest dimensions and preform merge
    else{
        mergeCrop(image, minWidth, minHeight);
        mergeCrop(image2, minWidth, minHeight);
        merge(image, image2, minWidth, minHeight);
    }
    save(image);
}

// increase value of red and blue to make purple effect
void purple(Image& image){
    int temp;

    for (int i = 0; i < image.width; ++i){

        for (int j = 0; j < image.height; ++j){

                for(int k = 0; k < 3; ++k){
                    if(k == 0 || k == 2){
                        temp = image(i, j, k) + 50;
                        
                        if(temp > 255){
                            image(i, j, k) = 255;
                        }
                        else{
                            image(i, j, k) = temp;
                        }
                    }
                }
            }
        }
    save(image);
}

int chooseFilter()
{
    // get the user choice for filters
    std::string choice{};
    while (true)
    {
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "| what filter do you want to use? |" << std::endl;
        std::cout << "-----------------------------------" << std::endl;
        std::cout << "1. Grayscale              2. Edge detect" << std::endl;
        std::cout << "3. Merge Images           4. Brighten or Darken Image" << std::endl;
        std::cout << "5. Purple" << std::endl;
        std::cout << "-------------------------------------------------" << std::endl;
        std::cout << "0. Exit the program" << std::endl;
        std::cout << "Enter choice :";
        std::getline(std::cin >> std::ws, choice);
        if(choice.length() > 1)
        {
            std::cout << "Please enter a valid choice" << std::endl;
        }
        try {
            int filter = std::stoi(choice);
            if(filter >= 0 && filter <= 5)
            {
                return filter;
            }
            std::cout << "please enter a valid choice" << std::endl;
        }
        catch (std::invalid_argument&) {
            std::cout << "Please enter a valid choice" << std::endl;
        }
    }
}

// get the image name or see if the user wants to exit
int getImage(Image& image)
{
    std::string imageName{};
    std::cout << "-------------------------------------------------" << std::endl;
    std::cout << "Please enter an image name or type exit to close:" << std::flush;
    std::getline(std::cin >> std::ws, imageName);
    std::cout << "-------------------------------------------------" << std::endl;

    if(imageName == "exit")
    {
        std::cout << "------------" << std::endl;
        std::cout << "| GOOD BYE |" << std::endl;
        std::cout << "------------" << std::endl;
        return 0;
    }

    try {
        image.loadNewImage(imageName);
    }
    catch (std::invalid_argument& e) {
        std::cerr << std::flush;
        std::cout << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return -1;
    }
    return 1;
}


int main()
{
    std::cout << "-----------------------------" << std::endl;
    std::cout << "| Welcome to Baby Photoshop |" << std::endl;
    std::cout << "-----------------------------" << std::endl;
    //- Ask the user about the image name that they want to load it
    //- load the image in "image"
    //- define "imageStatus" and use it to know if the operation of loading the image succeeded, failed, or the user wants to exit
    // imageStatus == 0 means exit the program, == 1 means image loaded, == -1 means error in loading
    int imageStatus = 1;
    Image image;
    bool loadNewImage = true;

    // second loop to do filters on the loaded image
    // and save or discard this image and load a new one
    // depends on the user choice
    while (true)
    {
        while (true){
            if(loadNewImage){
                imageStatus = getImage(image);
                if (imageStatus == 0){
                return 0;
                }
                else{
                    break;
                }
            }
            else {
                break;
            }
        }

        // check what filters does the user want
        int choice{};
        choice = chooseFilter();
        switch (choice) {
            case 0:
            std::cout << "------------" << std::endl;
            std::cout << "| GOOD BYE |" << std::endl;
            std::cout << "------------" << std::endl;
            return 0;
        case 1:
            grayScale(image);
            save(image);
            break;
        case 2:
            edgeDetect(image);
            break;
        case 3:
            mergeImages(image);
            break;
        case 4:
            brightenOrDarken(image);
            break;
        case 5:
            purple(image);
            break;
        default:
            std::cout << "You did not choose a filter" << std::endl;
        }

        loadNewImage = wantToLoadNew();
    }
}