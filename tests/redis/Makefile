
root_dir = $(CURDIR)/../../
GCC := g++ -std=c++0x $(flags)
includes := -I$(root_dir)

QueryHandler: obj/QueryHandler.o
	$(GCC) $(includes) -o $@ $@.Test.cpp $< -lhiredis -lcpptest

obj/QueryHandler.o: obj
	$(GCC) $(includes) -c -o $@ $(root_dir)/src/Redis/QueryHandler/QueryHandler.cpp -lhiredis

obj:
	mkdir -pv $@

make clean:
	rm -rf obj/
	rm -f QueryHandler
