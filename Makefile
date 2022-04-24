SOURCES := src/main.cpp src/timestamp.cpp src/files.cpp src/environment.cpp

all:
	g++ -Iinclude $(SOURCES) -o csup -Llib -lboost_filesystem-mgw8-mt-x64-1_79 -lboost_chrono-mgw8-mt-x64-1_79