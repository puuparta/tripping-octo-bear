# -*- coding: iso-8859-15 -*-

from easynn import EasyNN,VERSION

# luetaan EEG-signaali ja talletetaan vasen ja oikea-painokerroinmatriisit

def save(input_vector,path):
    nn = EasyNN(4,1,input_vector,bias=1)
    ret = nn.Train(0.1,100000,0.0005, 0.09, 1,input_vector) #0.000005, 0.3
    nn.Save(path)
    return ret

def test(input_vector,left_path,right_path):
    new_input = [1,2,3,4,5,6,7,8,7,6,5,4,3,2,1]
    new_input = [8,7,6,5,4,3,2,1,2,3,4,5,6,7,8]
    new_input = [1,2,3,4,5,4,2,2,2,6,5,2,3,2,2]
    nn = EasyNN(4,1,input_vector,bias=1)
    left=nn.Load(left_path,input_vector, bias=1)
    right=nn.Load(right_path,input_vector, bias=1)
    if(left>0.9 or right>0.9):
        if(left>right):
            return "Left"
        if(right>left):
            return "Right"
    else:
        return "None"

left_path=r'c:\left.bin'
right_path=r'c:\right.bin'
print save([1,2,3,4,5,6,7,8,7,6,5,4,3,2,1],left_path)
print save([8,7,6,5,4,3,2,1,2,3,4,5,6,7,8],right_path)
print test([1,2,3,4,5,6,7,8,7,6,5,4,3,2,1],left_path,right_path)
print test([8,7,6,5,4,3,2,1,2,3,4,5,6,7,8],left_path,right_path)
print test([1,2,3,4,5,4,2,2,2,6,5,2,3,2,2],left_path,right_path)

# --------------------------------------------------------------------------

# Binäärisen opetusalgoritmin testaus. Annetaan verkolle opetusaineistona 
# kaksi syöte-vasteparia ja testataan kolmella eri syötteellä lopputulosta.

set = {
        "train.1": {"input":[1,2,3,4,5,6,7,8,7,6,5,4,3,2,1], "expect":1},
        "train.2": {"input":[8,7,6,5,4,3,2,1,2,3,4,5,6,7,8], "expect":0}
}

input = [1,2,3,4,5,6,7,8,7,6,5,4,3,2,1]
nn = EasyNN(4,1,input, bias=None)
print nn.MultiTrain(0.22,100000,0.005, 0, set)
nn.Save(r'c:\pynn.bin')

input = [1,2,3,4,5,6,7,8,7,6,5,4,3,2,1]
nn2 = EasyNN(4,1,input, bias=None)
print nn2.Load(r'c:\pynn.bin',input, bias=None)

input = [8,7,6,5,4,3,2,1,2,3,4,5,6,7,8]
print nn2.Load(r'c:\pynn.bin',input, bias=None)

input = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
print nn2.Load(r'c:\pynn.bin',input, bias=None)







