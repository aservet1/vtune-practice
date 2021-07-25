cc = g++
target = bin/k-nn
extra-credit = bin/parallel-knn
cflags = -g -Wall -Wextra -pedantic
cxxfiles =	\
	src/myIO.cpp	\
	src/tree.cpp	\
	src/ThreadPool.cpp	\
	src/knn.cpp	\
	src/parallel_queries.cpp \

ofiles = $(cxxfiles:.cpp=.o)

all: $(ofiles) $(target) $(extra-credit)

$(target): $(cxxfiles) $(cxxfiles:.cpp=.hpp)
	$(cc) $(cflags) $(ofiles) src/driver.cpp -o $(@) -lpthread

$(extra-credit): $(target) $(ofiles) $(cxxfiles) src/extra-credit-driver.cpp src/Parallel_K-nn.cpp src/Parallel_K-nn.hpp
	g++ $(cflags) $(ofiles) src/Parallel_K-nn.cpp src/extra-credit-driver.cpp -o $(@) -lpthread

sidedriver: sidedriver.cpp
	$(cc) $(cflags) $(cxxfiles) src/sidedriver.cpp -o bin/sidedriver -lpthread

src/driver.o: src/driver.cpp
	cd src && g++ $(cflags) -c driver.cpp

src/myIO.o: src/myIO.cpp
	cd src && g++ $(cflags) -c myIO.cpp

src/tree.o: src/tree.cpp
	cd src && g++ $(cflags) -c tree.cpp

src/ThreadPool.o: src/ThreadPool.cpp
	cd src && g++ $(cflags) -c ThreadPool.cpp

src/knn.o: src/knn.cpp
	cd src && g++ $(cflags) -c knn.cpp

src/parallel_queries.o: src/parallel_queries.cpp
	cd src && g++ $(cflags) -c parallel_queries.cpp

clean:
	rm -f $(target) $(ofiles) $(extra-credit)
