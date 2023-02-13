
#include <stdio.h>
#include "../inc/svmlinear.h"

union compressed_pixels_2{
    unsigned int short pix1 : 2;
    unsigned int short pix2 : 2;
    unsigned int short pix3 : 2;
    unsigned int short pix4 : 2; 
};

/*
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
*/

union compressed_pixels_2 compression(int labeled_string[]){
    
    union compressed_pixels_2 cp2;   

    cp2.pix1 = labeled_string[0];
    cp2.pix2 = labeled_string[1];
    cp2.pix3 = labeled_string[2];
    cp2.pix4 = labeled_string[3];
    
    return cp2;     
    }



int main(){


    int ls[4] = {3,0,3,2};

    union compressed_pixels_2 cp = compression(ls);

    FILE *ret_t; 

    ret_t = fopen(FILE_PATH"3", "wb"); 

    fwrite(&cp, sizeof(union compressed_pixels_2),1,ret_t);
    fclose(ret_t);  


    return 0;
}