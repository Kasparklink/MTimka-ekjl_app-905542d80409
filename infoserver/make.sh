g++ -c main.cpp -o main.o
g++ -c sendd.cpp -o sendd.o
g++ -c errors.cpp -o errors.o
g++ -c globals.cpp -o globals.o
g++ -c server.cpp -o server.o
g++ -c lprint.cpp -o lprint.o
g++ -c functions.cpp -o functions.o
g++ -o build/infoserver main.o sendd.o errors.o globals.o server.o lprint.o functions.o -lboost_regex -lpthread 
rm main.o sendd.o errors.o globals.o server.o lprint.o
