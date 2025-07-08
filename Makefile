CXXFLAGS := -Wall -std=c++17
CXXLINKLIBS := -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio

main: obj/main.o obj/gamefunctions.o obj/pipe.o obj/missile.o
	clang++ $(CXXFLAGS) $(CXXLINKLIBS) -o main $^

obj/main.o: src/main.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

obj/gamefunctions.o: src/gamefunctions.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

obj/pipe.o: src/pipe.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^

obj/missile.o: src/missile.cpp
	clang++ $(CXXFLAGS) -c -o $@ $^
