#include "../inc/svmlinear.h"

int label_test(){

    unsigned char *labels[] = {'a','a','a','a','a','\0'}; 

    unsigned char *filename[128]; 

    snprintf(filename, sizeof(filename), "%s%s", FILE_PATH, "1");

    label_write_file(filename, labels);

    return 0; 
}
