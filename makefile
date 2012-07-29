source_files := src
branch_subs := Events Core Redis Parsers Types
includes = -I$(CURDIR)/src -I/usr/local/include -I/usr/include


devel_libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft
production_libs := -lwt -lwtfcgi -lboost_signals -lhiredis -lturnleft

turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))
objects := $(patsubst %.cpp, %.o, $(cpp_files))

all: $(objects) | $(turnleft)
	g++ -Wall -o mogu-server $(objects) $(devel_libs)

production: $(objects) | $(turnleft)
	g++ -Wall -o mogu-server $(objects) $(production_libs)

install: mogu-server
	mkdir -p /etc/mogu
	cp -r cli/* /etc/mogu
	cp -r resources/ /etc/mogu/
	ln -s $(CURDIR)/mogu-server /usr/bin/mogu-server
	ln -s /etc/mogu/mogu /usr/bin/mogu

mogu-server: all

uninstall:
	unlink /usr/bin/mogu-server
	unlink /usr/bin/mogu
	rm -rf /etc/mogu

%.o:
	g++ -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 

$(turnleft):
	git clone git://www.github.com/tomthorogood/TurnLeftLib.git
	cd TurnLeftLib && $(MAKE) && sudo $(MAKE) install && $(MAKE) clean

clean:
	rm -f mogu-server 
	rm -rf $(objects)
