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
float hex_int_to_float_two_byte(int value){
    float con_value = value*TWO_BYTE_DELTA_UNIFORM; //Converted value
    return con_value; 
}

float hex_int_to_float_four_byte(int value){
    float con_value = value*FOUR_BYTE_DELTA_UNIFORM; //Converted value
    return con_value; 
}



uint8_t svm_linear_bdt(X_i, svm_models[]){

    //Usage with the Binary Desicion Tree Alogrithm.
    float pred = 0; 

    for(int i = 3; i < sizeof(X_i); i++){
        pred += X_i[i-3] * svm_models[i] + svm_models[2];
    }

    if(pred/sizeof(X_i) >= 0){
        return 1; 
    }
    
    else{
        return 0; 
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

int hex_to_float_1_byte(int hex_value){
    unsigned char str[4]; 
    sprintf(str, "%s%02x", "0x", hex_value);
    return hex_to_float(str);
}


float hex_array_to_float(unsigned char* hex_array[4]){
    unsigned char str[10];
    sprintf(str, "%s%02x%02x%02x%02x", "0x" , hex_array[0], hex_array[1], hex_array[3], hex_array[4]);
    return hex_to_float(str);

}



int get_svm_models(struct file_bufffer* bdt_config, float svm_models[16][160], int modus){

    char file_track[]; 
    char svm_model_name_number[5];
    char svm_model_file_name[20];  
    uint8_t check_model_exist;
    unsigned char file_buffer[160*4 + 1]; 
    unsigned char hex_array[4]; 
    int svm_model_index;

    if(modus == 0){
        for(int = 0; i < sizeof(bdt_config); i++){
            check_model_exist = 0; 
            sprintf(svm_model_name_number, "%x", bdt_config[i])
            for(int j = 0; j < sizeof(file_track); j++){
                if(file_track[j] == svm_model_name_number){
                    check_model_exist = 1; 
                }
            if(check_model_exist == 0){
                svm_model_index = 0; 
                sprintf(svm_model_file_name, LINEAR_SVM_MODEL_NAME, svm_model_name_number);

                FILE *ptr_file;
                ptr_file = fopen(svm_model_file_name, "rb"); 

                fread(file_buffer,sizeof(file_buffer),1,ptr_file); 

                svm_models[i][0] =  hex_to_float_1_byte(file_buffer[0]); //class1
                svm_models[i][1] =  hex_to_float_1_byte(file_buffer[1]); //class2

                for(int l = 0; l < BYTE_SIZE; l++){
                    hex_array[l] = file_buffer[5-l]; 
                }    
                svm_models[i][2] =  hex_array_to_float(hex_array)//b

                for(int k = 9; k < sizeof(file); k+4){
                    for(int l = 0; l < BYTE_SIZE; l++){
                        hex_array[l] = file_buffer[k-l];
                    }
                    svm_models[i][svm_model_index] = hex_array_to_float(hex_array);
                    svm_model_index++;
                }


                fclose(ptr_file); 

            }
        }
    } 
}


uint8_t svm_linear(float X_i[], float svm_model[]){
    float pred = 0; 

    for(int i = 0; i < sizeof(X_i); i++){
        pred += (X_i[i] * svm_model[3+i]) + svm_model[2];
    }

    if(pred >= 0){
        return 1; 
    }

    else{
        return 0; 
    }

}



int binary_desion_tree_predict(X[][], bdt[][]){

   
    float svm_models[][]; 
    int svm_models_index = 0;
    int previous_index = 0; 
    uint8_t pixel_holder = 0; 
    int labeled_image[];

     // SVM Load Models into array

     for(int i = 0 ; i < sizeof(X); i++){
        previous_index = svm_models_index; 
        pixel_holder = svm_linear(X[i], svm_models[svm_model_index]); 

        if(pixel_holder > 0){
            svm_model_index = bdt[svm_model_index][1]; 
        }

        else{
            svm_model_index = bdt[svm_model_index][2];
        }

        if(svm_models_index == 0){

            if(pixel_holder > 0){
                labeled_image[i] = svm_modeles[previous_index][0];
            }

            else{
                labeled_image[i] = svm_models[previous_index][1]; 
            }

            break; 


        }

     }

}

int int_to_svm_model_name(int value){
    int fist_value = (int)((value - value%16)/16); 
    int second_value = (int)(value%16); 
    unsigned char model_name[5];
    int length = 0; 

    if(first_value < 10){
        length += sprintf(models_name + length, "%d" , first_value);
    }
}


int models_names(bdt[][]){
    unsigned char svm_model_list[]; 


    for(int i = 0; i < sizeof(bdt); i++){
        unsigned char c[2][2] = int_to_svm_model_name(bdt[i][i]); 
        if()
    }

    return 0; 
}



int linear_svm_models_load(unsigned char path, bdt[][]){

    //import os
    
    float svm_models_array[]; 
    //LINEAR_SVM_MODEL_NAME



}




///////////////////////////////////////////////////
/// Main
//////////////////////////////////////////////////

/*

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

    



    svmlinear_predict(svm, pH, image);
    //svmlinear_predict(struct SVM_Linear* svm, struct process_HSI *pH, struct Labeled_Image* image)

    write_csv_file(image);

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
 
*/z
    


      

