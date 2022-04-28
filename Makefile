SOURCES := src/main.cpp src/timestamp.cpp src/path.cpp src/files.cpp src/dependency.cpp src/environment.cpp 

all:
	g++ -std=c++2a -Iinclude $(SOURCES) -o csup -Llib -lboost_filesystem-mgw8-mt-x64-1_79 -lboost_chrono-mgw8-mt-x64-1_79