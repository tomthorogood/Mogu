source_files := src
branch_subs := Events Core Redis Parsers Types Perspectives crypt Sessions Validators Security

api_subs  := Crypt Redis
api_srcs := $(foreach d, $(api_subs), $(source_files)/$d)
api_objs := hash.o $(foreach source, $(api_srcs), $(wildcard $(source)/*.o))
api_hdrs := $(patsubst %.o, %.h, $(api_objs))
api_inst := $(patsubst $(source_files), $(api_dest), $(api_hdrs))
api_dest := /usr/local/include/Mogu
api_incl := $(foreach d, $(api_subs), $(api_dest)/$(d))

includes := -I$(CURDIR)/src -I/usr/local/include -I/usr/include
executable := mogu-server
o:=0
devel_libs := -lwt -lwthttp -lboost_signals -lhiredis -lturnleft -lcrypto -lcityhash
gen_config := cli/mogu_py/config_generator.py

turnleft := /usr/local/include/TurnLeftLib/TurnLeft.h
sources := $(source_files) $(foreach s, $(branch_subs), $(source_files)/$s)
cpp_files := $(foreach source, $(sources), $(wildcard $(source)/*.cpp))
objects := $(patsubst %.cpp, %.o, $(cpp_files))
command := g++ -Wall -O$(o) 



all: $(objects) | $(turnleft) 
ifeq ($(dbg),on)
	g++ -Wall -DDEBUG -DTERM_ENABLED -g -pg -o $(executable) $(objects) $(devel_libs)
else
	g++ -Wall -o $(executable) $(objects) $(devel_libs)
endif


production: $(objects) | $(turnleft)
	$(command) -o mogu-server $(objects) $(production_libs)

install: mogu.conf
	$(MAKE) uninstall
	mkdir -p /etc/mogu
	cp $< /etc/mogu
	cd cli/cli_src && $(MAKE) all
	cp -r cli/* /etc/mogu
	cp -r resources/ /etc/mogu
	ln -s $(CURDIR)/mogu-server /usr/bin/mogu-server
	ln -s /etc/mogu/mogu /usr/bin/mogu

install-cli: mogu.conf
	cd cli/cli_src && $(MAKE) all
	cp -r cli/* /etc/mogu
	cp $< /etc/mogu

uninstall:
	rm -f /usr/bin/mogu-server
	rm -f /usr/bin/mogu
	rm -rf /etc/mogu

%.o:
ifeq ($(dbg),on)
	$(command) -c -DDEBUG -DTERM_ENABLED -g -pg $(includes) -o $@ $(patsubst %.o, %.cpp, $@)
else
	$(command) -c $(includes) -o $@ $(patsubst %.o, %.cpp, $@) 
endif

$(turnleft):
	git clone git://www.github.com/tomthorogood/TurnLeftLib.git
	cd TurnLeftLib && $(MAKE) && sudo $(MAKE) install
	rm -rf TurnLeftLib

upgrade:
	$(MAKE) uninstall
	$(MAKE)
	$(MAKE) install

clean:
	cd cli/cli_src && $(MAKE) clean
	rm -rf $(objects)
	rm -rf *.pyc

purge: clean
	rm -rf mogu-server

mogu.conf:
	sudo python $(gen_config)

stage_api:
	for f in $(api_subs) ; do \
		mkdir -p $(api_dest)/$$f ; \
	done
etest:
	@echo $(api_srcs)
	@echo $(api_inst)

api: all stage_api

