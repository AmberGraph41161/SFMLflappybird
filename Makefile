CXXFLAGS := -Wall -std=c++17
CXXLINKLIBS := -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio

main: build/main.o build/gamefunctions.o build/pipe.o build/missile.o
	clang++ $(CXXFLAGS) $(CXXLINKLIBS) -o main $^

build/main.o: src/main.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

build/gamefunctions.o: src/gamefunctions.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

build/pipe.o: src/pipe.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

build/missile.o: src/missile.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^
