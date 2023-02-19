#include "../inc/svmlinear.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////
/// Private functions 
///////////////////////////////////////////////////////////


/*
int read_binary_file(char* filename){
    FILE * fileptr;
    unsigned long fileLen;  
    char c; 

    unsigned char *string; 

    fileptr = fopen(filename, "rb"); 

    if(!fileptr){
        printf("Unable to open file!\n"); 
        return -1; 
    }

    fileLen = ftell(fileptr); 


    string=(char *)malloc(fileLen+1);

    if(!string){
        printf("Memory not allocated\n"); 
        fclose(fileptr); 
        return -1; 
    }

    fread(string, fileLen, 1, fileptr);
    fclose(fileptr); 

    for(int i = 0; i < 50; i++){
    printf("File: %X\n", string[i]); 
    }
    
    free(string); 

    return 0;  

}
*/



/*
float mean_calculate(float* prediction_array[]){
    float sum = 0; 
    for(int i = 0; i < sizeof(prediction_array); i++){
        sum = sum + prediction_array[i]; 
    }
    float mean_value = (float)(sum/sizeof(prediction_array)); 
    return mean_value; 
}
*/



uint8_t svm_linear_bdt(uint8_t class_1, uint8_t class_2, float x, float w_i, float b){

    //Usage with the Binary Desicion Tree Alogrithm. 
    float pred = x * w_i + b; 

    if(pred >= 0){
        return class_2; 
    }
    
    else{
        return class_1; 
    }
}



///////////////////////////////////////////////////
/// Test scripts 
//////////////////////////////////////////////////


void fill_2D_array(struct process_HSI *pH){
    for(int i = 0; i < PREDICTED_IMAGE_SIZE; i++){
            pH->X[i] = 0.5;
    }

}

void fill_svm_struct(struct SVM_Linear* svm){
    svm->classes[0] = 0; 
    svm->classes[1] = 1; 
    svm->b = -0.75;

    for(int i = 0; i < sizeof(PREDICTED_IMAGE_SIZE); i++){
        svm->w[i] = 0.5;
    } 
}


float hex_to_float(char string[]){

    uint32_t num;
    float f;
    sscanf(string, "%x", &num);  // assuming you checked input
    f = *((float*)&num);
    return f; 

}

/*
int count_char(unsigned char string[]){
    int count = 0;    
    //Counts each character except space  
    for(int i = 0; i < sizeof(string)-2; i++) {  
        if(string[i] != ' ')  
            count++;  
    }  

    return count; 
}
*/ 

//string hex_values_combined_little_endian(){

void fill_svm_linear_little_endian(struct file_buffer* fb, struct SVM_Linear* svm){

    svm->classes[0] = hex_to_float(fb->buffer[0]);
    svm->classes[1] = hex_to_float(fb->buffer[1]);  
    svm->w_mean = hex_to_float(fb->buffer[2]); 
    unsigned char str[12]; 
    float w_buffer;

    svm->b = 0.021 - 0.0382; //-0.0382; 
    int w_index = 0; 
 
    for(int i = 5; i < sizeof(fb->buffer); i = i + 4){

        sprintf(str, "%s%02x%02x%02x%02x", "0x" , fb->buffer[i+1], fb->buffer[i], fb->buffer[i+3], fb->buffer[i+2]);
        w_buffer = hex_to_float(str); 

        svm->w[w_index] = w_buffer - 0.00000546;
        
        w_index++; 
        
        if(w_index >= 155){
            break; 
        }
    }

}



void fill_data_little_endian(struct file_buffer* fd,struct process_HSI* ph){

    unsigned long x_index = 0;
    unsigned char str[12];

    for(unsigned long i = 2; i < (9025*155)-2; i = i + 4){

        sprintf(str, "%s%02x%02x%02x%02x", "0x" , fd->buffer[i+1], fd->buffer[i], fd->buffer[i+3], fd->buffer[i+2]);
        ph->X[x_index] = hex_to_float(str) - 0.00029;
        x_index++; 
    }
}


int svmlinear_predict(struct SVM_Linear* svm, struct process_HSI *pH, struct Labeled_Image* image){

    //Classify just two classe.
    float prediction_buffer;  

    for(int i = 0; i < 9025; i++){
        prediction_buffer = 0; 

        for(int j = 0; j < svm->n_features; j++){
            prediction_buffer = prediction_buffer + ((svm->w[j]) * (pH->X[i*156 + j])) + (svm->b);
        }

       // image->predicted_image[i] = (prediction_buffer/156); 
    //}
        
        if((prediction_buffer/156) >= 0){
            image->predicted_image[i] = svm->classes[1]; 
        }

        else{
            image->predicted_image[i] = svm->classes[0]; 
        }
    }
     

    return 0; 
}

uint8_t svmlinear_predict_pixel_mean(float pixel_mean, float w_i_mean, float b, uint8_t class_1, uint8_t class_2){
    //Returns one predicted pixels from one of class_1 or class_2 

    float prediction = pixel_mean * w_i_mean + b; 

    if(prediction >= 0){
        return class_1; 
    }

    else{
        return class_2; 
    }

}


int write_csv_file(struct Labeled_Image* image){
    FILE *fpt; 
    fpt = fopen("labeled_data.csv", "w+");

    for(int i = 0; i < 9025; i++){
        fprintf(fpt, "%d" ,image->predicted_image[i]);
    }
    
    fclose(fpt); 

    return 0; 
}

int read_binary_file(char* filename, struct file_buffer* fb){

 
    FILE *ptr_file;

    ptr_file = fopen(filename,"rb"); 

    if(!ptr_file){
        fclose(ptr_file); 
        return -1;
    }

    fread(fb->buffer,sizeof(fb->buffer),1,ptr_file); 

    /*
    for(int i = 0; i<fileLen; i++){
       printf("%x ", buffer[i]); // prints a series of bytes
    }
    */ 

    fclose(ptr_file); 

    return 0; 
}


float get_w_mean(struct file_buffer *fb){
    unsigned char str[10];
    sprintf(str, "%s%02x%02x%02x%02x", "0x" , fd->buffer[0], fd->buffer[1], fd->buffer[2], fd->buffer[3]);
    return hex_to_float(str);

}





int binary_descion_tree(struct Binary_Desion_Tree* bdt, struct Labeled_Image* image, struct process_HSI_mean pH){

    struct file_buffer *fb = (struct file_buffer*)malloc(sizeof(struct file_buffer));
    struct SVM_Linear *svm = (struct SVM_Linear*)malloc(sizeof(struct SVM_Linear));
    
    fill_svm_linear_little_endian(fb,svm); 

    read_binary_file(bdt->svm_models_filenames[i], fb);

    float X_mean_pixel[228*958];
    unsigned char X_char[4];
    int x_count = 0; 

    for(int i = 0; i < 228*958; i = i + 3){
        X_mean_pixel[x_count] = (pH->X[i] + pH->X[i + 1] + pH->X[i + 2])/3 
        x_count++; 
    } 

    for(int i = 0; i < 228*958; i++){
    
        image->predicted_image[i] = svmlinear_predict_pixel_mean(X_mean_pixel[i], svm->w_mean, svm->b, svm->classes->[0], svm->classes[1])
    }


    

    
    free(fb); 
    free(svm);


    return 0; 

}


///////////////////////////////////////////////////
/// Main
//////////////////////////////////////////////////

int main(){

    struct SVM_Linear *svm = (struct SVM_Linear*)malloc(sizeof(struct SVM_Linear));
    struct Labeled_Image *image = (struct Labeled_Image*)malloc(sizeof(struct Labeled_Image)); 
    struct process_HSI *pH = (struct process_HSI*)malloc(sizeof(struct process_HSI)); 
    struct file_buffer *fb = (struct file_buffer*)malloc(sizeof(struct file_buffer));

    read_binary_file("myfile", fb); 
    fill_svm_linear_little_endian(fb,svm); 
    

    free(fb); 
    
    
    struct file_buffer *fd = (struct file_buffer*)malloc(sizeof(struct file_buffer));
    read_binary_file("data", fd); 
    
    
    fill_data_little_endian(fd, pH); 

    


/*
    svmlinear_predict(svm, pH, image);
    //svmlinear_predict(struct SVM_Linear* svm, struct process_HSI *pH, struct Labeled_Image* image)

    write_csv_file(image);
*/ 
    //for(int i = 0; i < 10; i++){
    //printf("%f\n", svm->w[i]);
    //}

    //for(int i = 1000000; i < 1000010; i++){
    //printf("%f\n", pH->X[i]);
    //}

    //for(int i = 0; i < 5; i++){
    //    printf("%f\n", image->predicted_image[i]);
    //}

    printf("%i\n", sizeof(fb->buffer)); 

    free(fd);
    free(svm); 
    free(image); 
    free(pH); 

    printf("Finished!\n"); 

    //printf("%f\n",hex_to_float("0xbc06111e")); 

    return 0; 
}
 


    


      

