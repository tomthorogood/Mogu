source_files := src
branch_subs := Events Core Redis Parsers Types Perspectives crypt Sessions
includes := -I$(CURDIR)/src -I/usr/local/include -I/usr/include
executable := mogu-server

devel_libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash
production_libs := -lwt -lwtfcgi -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash

turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))
objects := $(patsubst %.cpp, %.o, $(cpp_files))
command := g++ -Wall

all: $(objects) | $(turnleft) 
ifeq ($(dbg),on)
	g++ -Wall -DTERM_ENABLED -g -o $(executable) $(objects) $(devel_libs)
else
	g++ -Wall -o $(executable) $(objects) $(devel_libs)
endif


production: $(objects) | $(turnleft)
	g++ -Wall -o mogu-server $(objects) $(production_libs)

install: 
	mkdir -p /etc/mogu
	cp -r cli/* /etc/mogu
	cp -r resources/ /etc/mogu/
	ln -s $(CURDIR)/mogu-server /usr/bin/mogu-server
	ln -s /etc/mogu/mogu /usr/bin/mogu

install-cli:
	cp -r cli/* /etc/mogu	


uninstall:
	unlink /usr/bin/mogu-server
	unlink /usr/bin/mogu
	rm -rf /etc/mogu

%.o:
ifeq ($(dbg),on)
	g++ -c -DTERM_ENABLED -g $(includes) -o $@ $(patsubst %.o, %.cpp, $@)
else
	g++ -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 
endif

$(turnleft):
	git clone git://www.github.com/tomthorogood/TurnLeftLib.git
	cd TurnLeftLib && $(MAKE) && sudo $(MAKE) install && $(MAKE) clean

upgrade:
	cp /etc/mogu/mogu.conf cli/mogu.conf
	$(MAKE) uninstall
	$(MAKE)
	$(MAKE) install

clean:
	rm -rf $(objects)
	rm -rf *.pyc

purge: clean
	rm -rf mogu-server
