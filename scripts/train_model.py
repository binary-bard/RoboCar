#!/usr/bin/env python3

from __future__ import print_function
import numpy as np
np.random.seed(1337)  # for reproducibility
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Conv2D, MaxPooling2D
from keras.preprocessing.image import ImageDataGenerator
from keras.models import load_model
from keras import backend as K

#import matplotlib.pyplot as plt
#import scipy.misc
import argparse, sys

def train_model(datadir, modelname, img_width, img_height):
  image_size=(img_height, img_width)
  if K.image_data_format() == 'channels_first':
    input_shape = (3, img_height, img_width)
  else:
    input_shape = (img_height, img_width, 3)
    
  nb_train_samples = 10000
  nb_validation_samples = 4000

  batch_size = 20
  nb_angles = 15

  train_datagen = ImageDataGenerator(rescale=1./255)
  train_generator = train_datagen.flow_from_directory(
        datadir + '/train',
        target_size=image_size,
        batch_size=batch_size,
        class_mode='categorical')

  #Validation Generator
  val_datagen = ImageDataGenerator(rescale=1./255)
  val_generator = val_datagen.flow_from_directory(
        datadir + '/val',
        target_size=image_size,
        batch_size=batch_size,
        class_mode='categorical')

  print(train_generator.class_indices)
  print(val_generator.class_indices)

  # Python2 uses iteritems() while Python3 uses items()
  inv_map = {v: k for k, v in train_generator.class_indices.items()}

  nb_epoch = 50
  nb_filters=16
  kernel_size=(3,3)
  pool_size=(2,2)

  model_file = datadir + '/model_' + modelname + '.h5'
  weights_file = datadir + '/weights_' + modelname + '.h5'
  got_weights = False
  save_model = True
  try:
    model = load_model(model_file)
    print('Model loaded')
    got_weights = True
    save_model = False
  except:
    model = Sequential()
    model.add(Conv2D(nb_filters, kernel_size, input_shape=input_shape))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=pool_size))

    model.add(Conv2D(2*nb_filters, kernel_size))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=pool_size))

    model.add(Conv2D(2*nb_filters, kernel_size))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=pool_size))

    model.add(Conv2D(4*nb_filters, kernel_size))
    model.add(Activation('relu'))
    model.add(MaxPooling2D(pool_size=pool_size))

    model.add(Flatten())
    model.add(Dense(100))
    model.add(Activation('relu'))
    model.add(Dropout(0.2))
    model.add(Dense(nb_angles, activation = 'softmax', name = 'angle_out'))
    
    model.compile(loss='categorical_crossentropy',
              optimizer='adadelta',
              metrics=['accuracy'])

    print('Model compiled')
    try:
        model.load_weights(weights_file)
        got_weights = True
        print('Weights loaded')
    except:
        got_weights = False

  model.summary()

  hist = None
  #if not got_weights:
  steps_pe = 200
  valSteps = 50
  hist = model.fit_generator(train_generator, steps_per_epoch=steps_pe,
          epochs=nb_epoch, validation_data=val_generator,
          validation_steps=valSteps)

  model.save_weights(weights_file)
  save_model = True
  print('Model trained and weights saved')

  if save_model:
    model.save(model_file)
    print('Model saved')

  score = model.evaluate_generator(val_generator, steps=100)
  print('Validation score = ' + str(score))
  print('Done')


if __name__=="__main__":
  parser = argparse.ArgumentParser(description='Train a self driving car model on given data', add_help=False)
  parser.add_argument('--datadir', '-d', help='Train Data directory')
  parser.add_argument('--modelname', '-m', default='default', help='Output model name')
  parser.add_argument('--width', '-w', default=160, type=int, help='Width of image')
  parser.add_argument('--height', '-h', default=120, type=int, help='Height of image')
  args = parser.parse_args()
  print(args.datadir, args.modelname, args.width, args.height)

  try:
    train_model(args.datadir, args.modelname, args.width, args.height)

  except KeyboardInterrupt:
    pass


