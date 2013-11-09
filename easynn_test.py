# -*- coding: iso-8859-15 -*-

from easynn import EasyNN,VERSION

# luetaan EEG-signaali ja talletetaan vasen ja oikea-painokerroinmatriisit

def save(input_vector,path):
    nn = EasyNN(4,1,input_vector)
    ret = nn.Train(0.1,100000,0.001, 0.9, 1,input_vector) #0.000005, 0.3
    nn.Save(path)
    return ret

def test(input_vector,left_path,right_path):
    new_input = [1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,2,3,4,5,6,7,8,7,6]
    new_input = [8,8,8,8,5,6,6,6,6,6,6,6,6,12,1,1,0,0,1,1,1,1,1,1]
    new_input = [1,2,3,4,5,4,2,2,2,6,5,2,3,2,2,2,3,4,5,6,7,8,7,6]
    nn = EasyNN(4,1,input_vector)
    left=nn.Load(left_path,input_vector)
    right=nn.Load(right_path,input_vector)
    #left = (right/(left+right))
    #right = (left/(left+right))
    if(left>(1-0.1) or right>(1-0.1)):
        if(left>right):
            return "Left"
        if(right>left):
            return "Right"
    else:
        return "None"

left_path=r'c:\left.bin'
right_path=r'c:\right.bin'
print save([1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,2,3,4,5,6,7,8,7,6],left_path)
print save([8,8,8,8,5,6,6,6,6,6,6,6,6,1,1,1,0,0,1,1,1,1,1,1],right_path)
print test([1,2,3,4,5,6,7,8,7,6,5,4,3,2,1,2,3,4,5,6,7,8,7,6],left_path,right_path)
print test([8,8,8,8,5,6,6,6,6,6,6,6,6,1,1,1,0,0,1,1,1,1,1,1],left_path,right_path)
print test([4,5,5,3,2,2,4,6,6,0,0,0,0,0,0,0,0,2,2,2,2,1,1,1],left_path,right_path)






