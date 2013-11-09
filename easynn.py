# -*- coding: iso-8859-15 -*-
# Copyright (c) 2013 Pasi Heinonen, pasi.heinonen at gmail.com
# Licensed under MIT License.

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish, dis-
# tribute, sublicense, and/or sell copies of the Software, and to permit
# persons to whom the Software is furnished to do so, subject to the fol-
# lowing conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
# ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

#
# Python NeuralNet, Easy NN
#
VERSION="0.1"

import random
import cPickle as pickle
from math import exp


class EasyNN():
    def __init__(self, hidden_neurons, output_neurons,input_vector=[]):
        self.__momentum = 0
        self.__x = list(input_vector)
        self.__x.append(1)
        self.__hidden_neurons=hidden_neurons
        self.__output_neurons=output_neurons
        self.__Y = [0 for j in xrange(self.__hidden_neurons)]
        self.__o = [0 for j in xrange(self.__output_neurons)]
        self.__d_minus_o = None
        self.__w = [[0 for j in xrange(len(self.__x))] for i in xrange(self.__hidden_neurons)]
        self.__v = [[0 for j in xrange(self.__hidden_neurons)] for i in xrange(self.__output_neurons)]

    def __init_net(self):
        """
        Initializes net and loads. Required internally when loading net
        """
        for j in xrange(len(self.__Y)):
            sum = 0
            for i in xrange(len(self.__x)):
                sum = sum + self.__x[i] * self.__w[j][i]
            self.__Y[j] = 1 / (1 + exp(-sum))

        for j in xrange(len(self.__o)):
            sum = 0
            for i in xrange(len(self.__Y)):
                sum = sum + self.__Y[i] * self.__v[j][i]
            self.__o[j] = 1 / (1 + exp(-sum))

    def __init_weights(self):
        """
        Initializes weights before training
        """
        for j in xrange(len(self.__w)):
            sum = 0
            for i in xrange(len(self.__w[0])):
                self.__w[j][i] = random.uniform(-0.0005, 0.0005)

        for j in xrange(len(self.__v)):
            sum = 0
            for i in xrange(len(self.__v[0])):
                self.__v[j][i] = random.uniform(-0.0005, 0.0005)

    def __netError(self):
        """
        Calculates error-function for the net
        """
        for j in xrange(len(self.__Y)):
            sum = 0
            for i in xrange(len(self.__x)):
                sum = sum + self.__x[i] * self.__w[j][i]
            self.__Y[j] = 1 / (1 + exp(-sum))

        for j in xrange(len(self.__o)):
            sum = 0
            for i in xrange(len(self.__Y)):
                sum = sum + self.__Y[i] * self.__v[j][i]
            self.__o[j] = 1 / (1 + exp(-sum))

        return self.__d - self.__o[0]


    def __netNewWeights(self):
        """
        Weight matrices recalculation during training, DeltaW and DeltaV
        """
        prev_delta = 0
        for j in xrange(len(self.__Y)):
            for i in xrange(len(self.__x)):
                sigmoid_slopeo = self.__o[0] * (1 - self.__o[0])
                sigmoid_slopeh = self.__Y[j] * (1 - self.__Y[j])
                delta = self.__x[i] * sigmoid_slopeh * self.__v[0][j] * sigmoid_slopeo
                self.__w[j][i] = self.__w[j][i] + delta * self.__d_minus_o * self.__n + self.__momentum * prev_delta
                prev_delta = delta

        prev_delta = 0
        for j in xrange(len(self.__o)):
            for i in xrange(len(self.__Y)):
                sigmoid_slopeo = self.__o[0] * (1 - self.__o[0])
                delta = self.__Y[i] * sigmoid_slopeo + self.__momentum * prev_delta
                self.__v[j][i] = self.__v[j][i] + delta * self.__d_minus_o * self.__n
                prev_delta = delta

    def Load(self,net_path,input_vector=[]):
        """
        Loads input to the net
        """
        self.__x = list(input_vector)
        self.__w, self.__v, self.__Y =pickle.load( open( net_path, "rb" ) )
        self.__init_net()
        return self.__o[0]

    def Train(self, min_error=0.09, max_epochs=10000, learning_rate=0.05, momentum=0.3, expect=1, input_vector=None):
        """
        Backpropagation training with given input vector and expected output value
        """
        loop = True
        epoch_count = 0
        self.__d = expect
        self.__E_W = 0
        self.__d_minus_o = 0
        self.__n = learning_rate
        self.__momentum=momentum

        if input_vector!=None:
            self.__x=list(input_vector)
            self.__x.append(1)

        self.__init_net()
        self.__init_weights()

        while loop:
            epoch_count = epoch_count + 1
            self.__E_W = 0
            for j in self.__x:
                self.__d_minus_o = self.__netError()
                self.__E_W =self.__E_W + pow(self.__d_minus_o, 2)
                self.__netNewWeights()
            if (self.__d_minus_o < min_error): loop = False
            if(epoch_count>=max_epochs): loop = False
        return self.__o, self.__d_minus_o, epoch_count

    def MultiTrain(self, min_error=0.09, max_epochs=10000, learning_rate=0.05, momentum=0.3, train_set=None):
        """
        Multitraining (binary training), two input vectors and two expected outputs
        """
        if train_set==None: pass
        if len(train_set) != 2: pass
        loop = True
        epoch_count = 0
        self.__E_W = 0
        self.__E_W_set = [0,0]
        self.__d_minus_o = 0
        self.__n = learning_rate
        self.__momentum=momentum
        self.__init_net()
        self.__init_weights()
        positive=False
        negative=False

        while loop:
            epoch_count = epoch_count + 1
            self.__E_W = 0

            if epoch_count % 2==0:
                self.__x = list(train_set["train.1"]["input"])
                self.__x.append(1)
                self.__d = train_set["train.1"]["expect"]
                if ((self.__d  - self.__o[0]) < min_error): positive = True
            else:
                self.__x = list(train_set["train.2"]["input"])
                self.__x.append(-1)
                self.__d = train_set["train.2"]["expect"]
                if ((self.__o[0] - self.__d) < min_error): negative = True
            if(positive and negative): loop=False

            for j in self.__x:
                self.__d_minus_o = self.__netError()
                self.__E_W =self.__E_W + pow(self.__d_minus_o, 2)
                self.__netNewWeights()
                if epoch_count % 2==0:
                    self.__E_W_set[0]=self.__E_W
                else:
                    self.__E_W_set[1]=self.__E_W

            if (self.__E_W_set[0] < min_error and self.__E_W_set[1] < min_error): loop = False
            if (epoch_count>=max_epochs): loop = False
        return self.__o, (self.__o[0] - self.__d), (self.__d  - self.__o[0]), epoch_count

    def Save(self, net_path, input_path=None):
        """
        Save the weights and hidden neurons.
         This is used after the training explicitly
        """
        if input_path:
            pickle.dump(self.__x, open(input_path, "wb"))
        if net_path:
            pickle.dump([self.__w, self.__v, self.__Y], open(net_path, "wb"))







