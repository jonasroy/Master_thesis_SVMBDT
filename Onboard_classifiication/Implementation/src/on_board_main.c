#include "../inc/svmlinear.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int int_to_svm_model_name(int value, char c1[5], char c2[5]){

    short int cf1 = (short int)(value - value%16)/16; 
    short int cf2 = (short int)(value%16);
    char zero[2] = "0"; 
    char buffer[2]; 

    if(cf1 < 9){
        snprintf(c1,sizeof(&zero), zero); 
        snprintf(buffer, sizeof(buffer), "%i" , cf1); 
        strcat(c1,buffer);
    }

    else{
        snprintf(c1, sizeof(&c1), "%i", cf1);
    }

    if(cf2 < 9){
        snprintf(c2,sizeof(&zero), zero); 
        snprintf(buffer, sizeof(buffer), "%i" , cf2); 
        strcat(c2,buffer);
    }

    else{
        snprintf(c2, sizeof(&c2), "%i", cf2);
    }
     
   

    return 0; 
}


int models_names(int bdt[16][3], char model_name[10], char svm_model_list[16][10]){

    char c1[3]; 
    char c2[3]; 
    char svm_model_name[20];

    for(int i = 0; i < 16; i++){
        snprintf(svm_model_name, sizeof(model_name), model_name); 

        int_to_svm_model_name(bdt[i][0],c1, c2);
        
        strcat(svm_model_name,c1);
        strcat(svm_model_name,c2);
        snprintf(svm_model_list[i], sizeof(svm_model_name), svm_model_name); 
    }

    return 0; 

} 









float int_to_float_delta(short int value, short int byte){
    float max_value = 50.0; 
    float delta = max_value/(pow(2,8*byte)-1); 
    return (float)(value*delta); 
}


int model_load(char svm_models_list[16][10], int index,  float svm_models_array[64][160]){

        short int model_variables[30];
        short int classes_1[2];
        short int classes_2[2]; 

        char path[50] = "../../../Ground_Training/svm_model_files/"; //Make it in to macro
        char filename[80]; 

        FILE *ptr; 
        snprintf(filename, sizeof(path), path); 
        strcat(filename,svm_models_list[index]);
        //printf("%s\n", filename); 
        ptr = fopen(filename,"rb");
         
        fread(&classes_1, 1, 1, ptr);
        svm_models_array[index][0] = classes_1[0]; 
 
        fread(&classes_2, 1, 1, ptr);
        svm_models_array[index][1] = classes_2[0]; 
        
        fread(&model_variables, 6, 2, ptr);

        fclose(ptr);

        for(int j = 2; j < 6; j++){
            svm_models_array[index][j] = int_to_float_delta(model_variables[j-2], 2); 
        }

        return 0; 

}


int linear_svm_models_load(int bdt[64][3], float svm_models_array[64][160]){

    int file_size; 
    char path[50] = "../../../Ground_Training/svm_model_files/"; //Make it in to macro
    char filename[80]; 

    char svm_default_name[10] = "lsm"; 
    char svm_models_list[16][10]; 
    models_names(bdt,svm_default_name, svm_models_list); 


    struct stat st;
    stat(filename, &st);
    file_size = st.st_size;   

    for(int i = 0; i < 3; i++){
        model_load(svm_models_list, i, svm_models_array);
    }




    return 0; 
}


int hsi_images_load(char path[100], struct process_HSI* X){
    FILE *ptr; 
    ptr = fopen(path,"rb");
    short int buffer; 

    for(int i = 0; i < VIGO_SIZE; i++){
        for(int j = 0; j < 3; j++){
            fread(&buffer, 1, 2, ptr);
            X->X[i][j] = int_to_float_delta(buffer,2); 
        }
    }

    fclose(ptr); 

    return 0; 
    
}

int labeled_image_bin(char path[100] ,struct Labeled_Image *image){
    FILE *ptr; 
    ptr = fopen(path, "wb");
    uint8_t label_size = sizeof(image->predicted_image[0]); 
    unsigned char buffer[1]; 

    if(ptr == NULL) {
        printf("File creating: something went wrong");
        return -1;
    }
    
    for(int i = 0; i < VIGO_SIZE; i++){
        buffer[0] = (unsigned char)(image->predicted_image[i]); 
        fwrite(buffer,sizeof(buffer),1,ptr);
    }

    fclose(ptr); 

    return 0; 

}

int svm_linear(float X_i[160], float svm_model[160]){

    float pred = 0.0; 

    for(int i = 0; i < 3; i++){
        pred += (X_i[i]*svm_model[3+i]) + svm_model[2]; 
    }

    if(pred >= 0){
        return -1; 
    }

    else{
        return 1; 
    }
}


int bdt_execute(struct process_HSI *X, int bdt[16][3], struct Labeled_Image* image){


    float svm_models[16][160];
    linear_svm_models_load(bdt, svm_models); 

    int svm_models_index = 0; 
    int previous_index = 0; 
    int pixel_holder = 0; 
    
    for(int i = 0; i < VIGO_SIZE; i++){

        svm_models_index = 0; 

        while(1){

            previous_index = svm_models_index; 
            pixel_holder = svm_linear(X->X[i], svm_models[svm_models_index]); 

            if(pixel_holder >= 0){
                svm_models_index = bdt[svm_models_index][1]; 
            }
            else{
                svm_models_index = bdt[svm_models_index][2]; 
            }

            if(svm_models_index == 0){

                if(pixel_holder >= 0){
                    image->predicted_image[i] = svm_models[previous_index][0]; 
                }

                else{
                    image->predicted_image[i] = svm_models[previous_index][1]; 
                }

                break; 
            }
        }
    }

    return 0; 

}


int main(){

    int bdt[64][3] = {{35, 0, 0},
                      {37, 0, 0}, 
                      {120, 0, 0}};

    char model_name[10] = "lsm";
    float svm_models_array[16][160]; 



    struct Labeled_Image *image = (struct Labeled_Image*)malloc(sizeof(struct Labeled_Image)); 
    struct process_HSI *X = (struct process_HSI*)malloc(sizeof(struct process_HSI));

    char hsi_image_path[100] = "../../../Ground_Training/hsi_images/vigo";
    hsi_images_load(hsi_image_path, X);

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    bdt_execute(X, bdt, image); 

    gettimeofday(&stop, NULL);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 


    char labelel_image_path[100] = "../../../Ground_Training/label_image_bin/vigo2"; 
    labeled_image_bin(labelel_image_path,image);
 
    printf("Ferdig nå Jonas\n"); 


    free(image); 
    free(X); 

    return 0; 
}
