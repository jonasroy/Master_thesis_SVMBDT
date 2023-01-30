#include <stdint.h>
#include <stddef.h>

#ifndef SVMLINEAR_H
#define SVMLINEAR_H


#define SVM_MAX_CLASSES 2
#define RAND_MAX 10000
#define FILE_PATH "C:\\Users\\jonas\\Master_Thesis\\Master_thesis_SVMBDT\\Results\\Binary_Results\\"


struct SVM_Linear
{
    uint8_t classes[]; 
    float C; 
    float w[]; 
    float b;
};

struct Binary_Desion_Tree{
    uint8_t kernel_type;
    uint8_t arcitecture[][];     
};

struct Labeled_Image{
    int timestamp; 
    int xpix; 
    int ypix; 
    uint8_t classes[]; 
    uint8_t image[];  
};

//int label_write_file(char filename, char labels);



#endif
