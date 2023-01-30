#include "../inc/svmlinear.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

////////////////////////////////////////////////////////////
/// Private functions 
///////////////////////////////////////////////////////////


int label_write_file(unsigned char *filename, unsigned char *labels[]){

    //Write labels to binary file.
    FILE *ret_t; 
    ret_t = fopen(filename, "wb"); 

    if(ret_t == NULL){
        printf("File failed!\n"); 
        return -1; 
    }

    else{
    
    fwrite(labels, sizeof(labels), 1, ret_t); 
    fclose(ret_t); 

    return 0;
    }
}


void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}





////////////////////////////////////////////////////////////
/// SVM Functions 
///////////////////////////////////////////////////////////

float hingeloss(struct SVM_Linear *svm, float *w[], float *b, float *x[], float *y[]){
    
    float reg[sizeof(w)]; 

    for(int i = 0; i < sizeof(reg); i++){
        reg[i] = 0.5 * (*w[i]) * (*w[i]); 
    }
    
    float opt_term[sizeof(y)]; 
    float loss[sizeof(reg)]; 
    float check; 

    for(int i = 0; i < sizeof(x); i++){

        for(int j = 0; j < sizeof(x); j++){
            opt_term[j] = (*y[j]) * (*w[j]) * (*x[i]) + (*b);

        }

        for(int k = 0; k < sizeof(reg); k++){
            
            check = 1 - opt_term[k]; 

            if(check > 0){
                loss[k] = reg[k] + svm->C * (check); 
            }

            else{
                loss[k] = reg[k]; 
            }
        }

    return loss[0]; 

    }
     
}

int svmlinear_fit(struct SVM_Linear *svm, float *X[], float *Y[], int batch_size, float learning_rate, int epochs){
    
    int number_of_features = sizeof(X); 
    int number_of_samples = sizeof(X); // Needs to be changed.  

    int ids[number_of_samples]; 
    for(int i = 0; i < sizeof(ids); i++){
        ids[i] = i; 
    }

    shuffle(ids, 1000); //Change the size. 

    float w[number_of_features]; 
    float *b = 0;  
    float losses[] = {};
    int gradw; 
    int gradb;
    int x;   

    for(int i = 0; i < epochs; i++){
        float l = hingeloss(svm,w,b,X,Y);
        
        losses[i] = l; 

        for(int batch_intial = 0; batch_intial < number_of_samples;  batch_intial += batch_size){
            gradw = 0; 
            gradb = 0; 

            for(int j = 0; i < batch_intial + batch_size; j++){
                if(j < number_of_samples){
                    x = ids[j]; 
                    for(int k = 0; k < sizeof(w); k++){
                        
                    }

                }
            }

        }

    }

    return 0; 
}


int svmlinear_predict(struct SVM_Linear* svm,  float* X[][], uint8_t* prediction[][]){

    int i; 
    int j;
    float prediction_buffer; 

    for(i = 0; i < sizeof(prediction[0]); i++){
        for(j = 0; j < sizeof(svm->w); j++){
            prediction_buffer = X[i][j] * w[i] + svm->b;
            if(prediction_buffer >= 0){
                prediction[i][j] = svm->classes[0]; 
            }

            else{
                prediction[i][j] = svm->classes[1]; 
            }
        }
    }
    return 0; 
}


///////////////////////////////////////////////////
/// Test scripts 
//////////////////////////////////////////////////

int label_test(){

    unsigned char *labels[10] = {"0xA","0xA","0xA","0xA","0xA",'\0'}; 

    unsigned char *filename[128]; 

    snprintf(filename, sizeof(filename), "%s%s", FILE_PATH, "1");

    label_write_file(filename, labels);

    return 0; 
}



///////////////////////////////////////////////////
/// Main
//////////////////////////////////////////////////



int main(){

    return 0; 
}