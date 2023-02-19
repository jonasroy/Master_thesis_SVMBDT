#include <stdint.h>
#include <stddef.h>

#ifndef SVMLINEAR_H
#define SVMLINEAR_H


#define SVM_MAX_CLASSES 2
#define RAND_MAX 10000
#define MAX_CLASSES 16
#define HSI_X 100
#define HSI_Y 100
#define PREDICTED_IMAGE_SIZE 10000
#define MAX_FILE_SIZE 6000000
#define MAX_FEATURE_SIZE 240
#define FILE_PATH "C:\\Users\\jonas\\Master_Thesis\\Master_thesis_SVMBDT\\Results\\Binary_Results\\"

#define SVM_LINEAR_FILE_PATHS 

struct SVM_Linear
{
    uint8_t classes[MAX_CLASSES];
    uint8_t n_features;  
    float w_mean;
    float b;
    float w[MAX_FEATURE_SIZE];
};
   
    

struct SVM_Sparse{
    uint8_t classes[MAX_CLASSES]; 
    
};

struct process_HSI_mean{
    float X[1000000];
}

struct process_HSI{
    /*uint16_t x_width[1024]; */ 
    float X[MAX_FILE_SIZE]; 
};

struct Binary_Desion_Tree{
    unsigned char svm_models_filenames[32];
    uint8_t n_layers; 
    uint8_t max_layers_width; 
    uint8_t tree_branches[32][2]; //Classes in the branches
         
};

struct Labeled_Image{ 
    uint8_t classes[MAX_CLASSES]; 
    uint8_t predicted_image[PREDICTED_IMAGE_SIZE];  
};

struct normalize{
    float x,y,z; 
}; 

struct file_buffer{
    unsigned char buffer[MAX_FILE_SIZE]; 
};

//int label_write_file(char filename, char labels);



#endif
