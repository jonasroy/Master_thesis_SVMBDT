# svm_model : [class1, class2, b, w_i[]]

def svm_linear(X_i, svm_model):
    pred = 0 

    for i in range(len(w)): 
        pred += (X_i[i] * svm_model[3][i]) + svm_model[2]

    if(pred/len(w) >= 0): 
        return 1
    else:
        return 0


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


tb = [ ["01"], ["23"], ["45"], 
       ["23"], ["67"], ["89"], 
]

tb = [
    []

]

"lsm01" #Linear SVM Model 01 Name

#labeled_image : [n_classes, [class0, class1, ... , class3], [labeled_pixel0, labeled_pixel1, ..., labeled_pixeln]]
#svm_model : [class1, class2, b, w_i[]]
#svm_models_buffer = [svm_model0, svm_model1 , ... , svm_modleln]
#svm_model_place = ["01", "23", "45", "23", "67", "89"] etc. 


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