"""Because the object detection is written in python and Windows doesn't recreate fork/exec the same in Unix systems
   this socket code is written in python!
   Its going to wait and get a connection, then immediately start running the object detection code. When its done
   it will finally send back a 'done' message and the rest of the "network" can continue"""
   

import socket
import cv2
import numpy as np
from keras.models import load_model


HOST = "192.168.254.20"  # Standard loopback interface address (localhost)
PORT = 5422  # Port to listen on (non-privileged ports are > 1023)

usage = 0


with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    print("Binded to port! Waiting for ESP...")
    while True: #always run
        s.listen()
        conn, addr = s.accept()
        with conn:
            #if here, there was a connection made, run the model code
            print(f"Connected by {addr}") #verification in testing
            data = conn.recv(40)
            temp = data.decode()
            print("Command recieved: '",temp,"'")
            if temp == " Stop ":
                break #recieved the stop command
            #If no Stop command recieved, detect

            # Load the cat model
            model = load_model('keras_model.h5')
            # Grab the labels from the labels.txt file. This will be used later.
            labels = open('labels.txt', 'r').readlines()
            # Load the litter box model
            usingModel = load_model('Boxkeras_model.h5')
            # Grab the labels from the labels.txt file. This will be used later.
            Boxlabels = open('Boxlabels.txt', 'r').readlines()


            # CAMERA can be 0 or 1 based on default camera of your computer.
            try: #sometimes, the camera can shut off prematurely, or a network issue, this allows it to keep going through the loop 
                camera = cv2.VideoCapture('http://192.168.254.25:8000/stream.mjpg')
            
                while True:
                    # Grab the webcameras image.
                    ret, image = camera.read()
                    # Resize the raw image into (224-height,224-width) pixels.
                    image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)
                    # Make the image a numpy array and reshape it to the models input shape.
                    image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)
                    # Normalize the image array
                    image = (image / 127.5) - 1
                    # Have the model predict what the current image is. Model.predict
                    # returns an array of percentages. Example:[0.2,0.8] meaning its 20% sure
                    # it is the first label and 80% sure its the second label.
                    probabilitiesOfCat = model.predict(image)
                    probabilitiesUsingBox = usingModel.predict(image)
                    # filter out weak detections by requiring a minimum
                    # confidence
                    # Grab the highest value and check that value
                    guessCat = np.max(probabilitiesOfCat) #confidence
                    #print("GuessCat Value: ", guessCat)
                    if guessCat >= 0.9:
                        # If its here, its likely that it saw a cat walk into the room
                        # Print what the highest value probability label
                        #print("Proabliltiy Value: ",probabilitiesOfCat)
                        print("Cat: ", labels[np.argmax(probabilitiesOfCat)])
                        print("Confidence: ", guessCat)
                    elif guessCat < 0.8:
                        #if no cats in frame, leave
                        camera.release()
                        #cv2.destroyAllWindows()
                        print("Detection Complete, current count: ", usage)
                        conn.send(b"Detection Complete")
                        break
                    guessBoxindex = np.argmax(probabilitiesUsingBox) # Label index
                    guessBox = np.max(probabilitiesUsingBox) #confidence
                    #if it sees a cat going in and its relitively confident 
                    if (guessBoxindex == 0 and guessBox >=0.6): 
                        print("using box...")
                        print("Confidence: ", guessBox)
                        usage += 1
                        while True:
                            print("Waiting for cat to leave box")
                            #Loop until it sees a cat come out
                            #Prevents it from stopping while a cat is in the box and "out of view"
                            # Grab the webcameras image.
                            ret, image = camera.read()
                            # Resize the raw image into (224-height,224-width) pixels.
                            image = cv2.resize(image, (224, 224), interpolation=cv2.INTER_AREA)
                            # Make the image a numpy array and reshape it to the models input shape.
                            image = np.asarray(image, dtype=np.float32).reshape(1, 224, 224, 3)
                            # Normalize the image array
                            image = (image / 127.5) - 1
                            probabilitiesUsingBox = usingModel.predict(image)
                            guessBoxindex = np.argmax(probabilitiesUsingBox) # Label index
                            guessBox = np.max(probabilitiesUsingBox) # Confidence
                            # If it sees a cat come out and its relatively confident
                            if (guessBoxindex == 1 and guessBox >=0.6):
                                break
                            #Listen to the keyboard for presses.
                            keyboard_input = cv2.waitKey(1)
                            # 27 is the ASCII for the esc key on your keyboard.
                            if keyboard_input == 27: #For testing
                                break
                    # Uncomment to show the camera image in a window. Comment in implementation, good for testing
                    #cv2.imshow('Webcam Image', image)
                    #print("displayed")
                    # Listen to the keyboard for presses.
                    keyboard_input = cv2.waitKey(1)
                    # 27 is the ASCII for the esc key on your keyboard.
                    if keyboard_input == 27: #For testing and hard escape
                        break

            except:
                #do nothing
                cv2.destroyAllWindows() #just in case
                print("Continuing\n")
                conn.send(b"Continuing...")