# svm_model : [class1, class2, b, w_i[]]
import numpy as np
import copy
from HelperFunctions_ import * 
import matplotlib.pyplot as plt


def svm_linear(X_i, svm_model):
    pred = 0 

    for i in range(len(X_i)): 
        pred += (X_i[i] * svm_model[3+i]) + svm_model[2]
        #pred = np.dot(X_i, np.array(svm_model[3:len(X_i)+3]).T) + svm_model[2]
    if(pred >= 0): 
        return -1
    else:
        return 1


def index_svm_model(next_model, tree_branch, svm_model_place): 
    # Find the index for next tb, if not exisiting return -1 
    # Find the index where the next svm_model is placed. 
    svm_model_index = 0
    svm_model_name_tb_index = 0 

    for i in range(len(tree_branch)):
        if(tree_branch[i][0] == next_model): 
            svm_model_name_tb_index = i 
            break

    for i in range(len(svm_model_place)): 
        if(svm_model_place[i] == next_model):
            svm_model_index = i
            break

    return svm_model_index, svm_model_name_tb_index




def bdt_execute(X, tb, svm_models):

    pixel_label = 0
    end_of_branch = 0
    svm_model_place = [] #char array
    svm_model_name_tb_index = 0
    labeled_image = []
    
    #Definere et char array.
    #Åpner alle svm_model filene her. 
    #Alt er konvertert herfra eller noe i et float array 
    #Hvis w_mean er sant, henter bare denne, hvis ikke, tar alle W_i verdier i hver svm.

    svm_models_buffer = []
    svm_model_index = 0

    for i in range(len(X)): #Labels one pixel at the time. 
        end_of_branch = 0 
        while(end_of_branch == 0): 
            pixel_label = svm_linear(X[i], svm_models_buffer[svm_model_index])

        if(pixel_label == 1):
            svm_model_index, svm_model_name_tb_index = index_svm_model(tb[svm_model_index][1], tb, svm_model_place)

        else: 
            svm_model_index, svm_model_name_tb_index = index_svm_model(tb[svm_model_index][2], tb, svm_model_place)

        if(svm_model_name_tb_index < 0):
            
            labeled_image.append(svm_linear(X[i], svm_models_buffer[svm_model_index]))
            end_of_branch = 1

    return labeled_image


def binary_tree_binary_file(file_path, filename ,tb): 

    new_tb = np.array(tb).flatten()

    tb_string = ""

    for i in range(len(new_tb)): 
        tb_string += new_tb[i]

    file = open(file_path + filename, "wb")

    buf = struct.pack('{}s'.format(len(tb_string)), bytes(tb_string, "utf-8"))

    file.write(buf)

    file.close()


def training_data_to_binary_file(file_path ,modus, class_1, class_2, b, w_i):
    #svm_model format: [class1, class2, b, w_i[]]

    #Modus: type of svm model. Linear, rbf, sparse etc.

    svm_model_name = ""

    if(modus == 0): 
        svm_model_name = "lsm" + str(class_1) + str(class_2)

    file = open(file_path + svm_model_name + ".bin", "wb")    
    
    buf = struct.pack("BB", class_1, class_2)

    file.write(buf)

    buf = struct.pack(f'{len([b])}f', *[b])

    file.write(buf)

    buf = struct.pack(f'{len(w_i)}f', *w_i)

    file.write(buf)

    file.close()


def float_to_int_delta(value, byte):
    
    import copy

    max_value = 50
    v = copy.deepcopy(value)
    delta = max_value/(pow(2,8*byte)-1)
    converted_int = int(v/delta)

    if(converted_int >= (pow(2,8*byte)/2)-1): 
        return int(pow(2,8*byte)/2)-1
    
    elif(converted_int <= -(pow(2,8*byte)/2)+1): 
        return -int(pow(2,8*byte)/2)+1
    
    else: 
        return converted_int

def int_to_float_delta(value, byte): 
    import copy
    max_value = 50
    v = copy.deepcopy(value)
    delta = max_value/(pow(2,8*byte)-1)
    return v*delta

def int_to_svm_model_name(value): 
    first_value = int((value - value%16)/16)
    second_value = value%16
    return str(first_value), str(second_value) 

def string_int_to_hex_int(str_hex): 
    return 16*int(str_hex[0]) + int(str_hex[1]) 


def linear_train_int_bin(file_path, class_1, class_2, b, w): 
        #svm_model format: [class1, class2, b, w_i[]]

    #Modus: type of svm model. Linear, rbf, sparse etc.

    import struct 
    
    svm_model_name = ""

    c1 = str(class_1) 
    c2 = str(class_2)

    if(len(c1) == 1): 
        c1 = "0" + c1

    if(len(c2) == 1): 
        c2 = "0" + c2

    svm_model_name = "lsm" + c1 + c2

    file = open(file_path + svm_model_name, "wb")    
    
    buf = struct.pack("BB", class_1, class_2)

    file.write(buf)

    buf = struct.pack('h', b)

    file.write(buf)

    w_size = str(len(w)) + "h"

    buf = struct.pack(w_size, *w)
    
    file.write(buf)

    file.close()

    return 0



def bdt_config_bin(file_path, bdt_config_aray, config_num):
    
    import struct 

    bca = np.array(bdt_config_aray).flatten()

    bdt_config_name = "bdtc" + str(config_num)
    
    file = open(file_path + bdt_config_name, "wb")

    for i in range(len(bca)):
        buf = struct.pack(">B", bca[i]) 
        file.write(buf)

    file.close()


def hsi_binary(file_path, filename, X):
    
    import struct

    pH = np.array(X).flatten()
        
    file = open(file_path + filename, "wb")

    for i in range(len(pH)):
        buf = struct.pack("h", float_to_int_delta(pH[i],2)) 
        file.write(buf)

    file.close()

def models_names(bdt, model_names): 
    
    svm_model_list = []

    for i in range(len(bdt)): 
        c1, c2 = int_to_svm_model_name(bdt[i][0])
        
        if(len(c1) == 1): 
            c1 = "0" + c1

        if(len(c2) == 1): 
            c2 = "0" + c2

        svm_model_list.append(str(model_names) + str(c1) + str(c2)) 

    return svm_model_list


def linear_svm_models_load(path, bdt): 

    import os 
    import struct

    svm_models_array = []

    svm_default_name = "lsm"

    svm_model_list = models_names(bdt, svm_default_name)

    for i in range(len(svm_model_list)): 
        file = open(path + svm_model_list[i], "rb")

        unpack_opt = "2b" + str(int(os.stat(path + svm_model_list[i]).st_size/2 - 1))  + "h"
        content = struct.unpack(unpack_opt,file.read())

        svm_model = []
        svm_model.append(content[0])
        svm_model.append(content[1])

        for j in range(2,len(content)): 
            svm_model.append(int_to_float_delta(content[j],2))
        
        svm_models_array.append(svm_model)

        file.close()

    return svm_models_array  

def rotate_image(image): 
    rotated_image = []
    for i in range(len(image[0])): 
        x_array = []
        for j in range(len(image)-1,-1,-1):
            x_array.append(image[j][i])
        rotated_image.append(x_array)

    return np.array(rotated_image)


def svm_branch_models_to_bin(svm_branch_model, b_extra):
    
    sbm = copy.deepcopy(svm_branch_model)
    class_1 = sbm.classes_[0]
    class_2 = sbm.classes_[1]
    b = float_to_int_delta(sbm.intercept_[0] + b_extra ,2)
    w = sbm.coef_[0]
    tw = []

    for i in range(len(w)): 
        tw.append(float_to_int_delta(w[i],2))

    linear_train_int_bin("svm_model_files/", class_1, class_2, b, tw)


def bdt_string_to_config_format(bdt):
    
    import copy

    config_bdt = []

    for i in range(len(bdt)): 
        bdt_index = []
        bdt_i = copy.deepcopy(bdt[i][0]) 

        if(bdt_i[2] == "0"): 
            bdt_i = bdt_i[0] + bdt_i[1] + bdt_i[3] 

        if(bdt_i[0] == "0"): 
            bdt_i = bdt_i[1] + bdt_i[2]

        bdt_index.append(string_int_to_hex_int(bdt_i))
        bdt_index.append(int(bdt[i][1]))
        bdt_index.append(int(bdt[i][2]))
        config_bdt.append(bdt_index)

    return config_bdt

def bdt_execute(X, bdt): 

    # bdt : [svm_model_classes, send_index1, send_index2]
    
    svm_models = linear_svm_models_load("svm_model_files/", bdt)
    svm_models_index = 0 
    prevoius_index = 0 
    pixel_holder = 0 
    labeled_image = [] 

    for i in range(len(X)): 

        svm_models_index = 0

        while(True):

            prevoius_index = svm_models_index

            pixel_holder = svm_linear(X[i], svm_models[svm_models_index])

            if(pixel_holder >= 0): 
                svm_models_index = int(bdt[svm_models_index][1])

            else: 
                svm_models_index = int(bdt[svm_models_index][2])

            if(svm_models_index == 0): 

                if(pixel_holder >= 0): 
                    labeled_image.append(svm_models[prevoius_index][0])
                else: 
                    labeled_image.append(svm_models[prevoius_index][1])

                break

    return labeled_image, svm_models


def store_labeled_image(predicted_label, x_dim, y_dim, path): 

    hypso_color_label = { 0 : [0,0,139], 
                        1 : [13,152,186], 
                        2 : [144,238,144], 
                        3 : [1,50,32], 
                        4 : [240,0,0], 
                        5 : [20,20,20], 
                        6 : [255,255,255], 
                        7 : [194,178,128], 
                        8 : [180,180,180], 
                        9 : [100,100,100], 
                        10 : [200,200,0],   
                        11 : [0,0,0]
    }


    new_predicted_label = copy.deepcopy(predicted_label)
    new_predicted_label = np.array(new_predicted_label)

    np.save(path, new_predicted_label)

    yout_color = newColorLabels(new_predicted_label,x_dim,y_dim, hypso_color_label)

    plt.imshow(yout_color)
    plt.axis('off')
    plt.savefig(path, dpi = 300, bbox_inches="tight")
    plt.close()
        

def analyse_labeled_satellite_images(file_path):
    import struct
    with open(file_path, mode='rb') as file: # b is important -> binary
        fileContent = file.read()
    filesize = len(fileContent)
    unpack_opt = str(filesize) + "B"
    return struct.unpack(unpack_opt, fileContent)


# 0 Water 
# 1 Strange Water 
# 2 Light Forest 
# 3 Dark Forest 
# 4 Urban 
# 5 Rock 
# 6 Ice 
# 7 Sand
# 8 Thick Clouds 
# 9 Thin Clouds 
# 10 Crop 
