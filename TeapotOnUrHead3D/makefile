all: augreal2

augreal2: glmix.o haarcascade_frontalface_alt.xml
	g++ -L/usr/local/lib -o augreal2 glmix.o  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lglut -lGL -lGLU  

glmix.o: glmix.cpp
	g++ -Wall -g -I/usr/local/include -I/usr/local/include/opencv -I/usr/local/include/opencv2 -c glmix.cpp -o glmix.o

clean:
	rm -rf *.o augreal2
