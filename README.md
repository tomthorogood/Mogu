# Mogu: Bend the Web #

## What is Mogu? #
Mogu is a web framework designed with the end user in mind. We developers, we're great and all,
but sadly our end users can't keep up with a lot of what we do. Mogu aims to change that. 

Mogu ('Moʊgu) is shorthand for "Moldable Goo", because that's what this framework gives you. After
getting it up and running, you are free to mold it to your will. No longer must you be a slave to
templates. It is, in fact, entirely possible to create a fully interactive, animated website without
writing any HTML or JavaScript. CSS and a smattering of Python (if you want) can get you up and running.

In the end, there will be no need for the scripting language backend, as it will be handled by a GUI.
Until then, the syntax is straightforward and the learning curve should be shallow.

Mogu uses [Wt](http://www.webtoolkit.eu) and [Redis](http://www.redis.io) as its two major contributing
backend handlers. Wt is in fact a framework on its own, written in C++. Redis is a nosql-style databasing
system. These two things combined may in fact break your brain. 

Mogu also comes with a toolkit of command line functions to analyze and make changes to your application
without on the fly, including export/import functionality allowing you to test once and deploy anywhere.

**Full documentation can be found at [gomogu.org](http://www.gomogu.org)**

## Building Dependencies #

Currently, the worst part about using Mogu is getting everything else setup first, namely Wt. Once you're
rolling, though, the world is a bright and shiny place. 

*I am not a Windows developer. These instructions apply to Linux. Please, please help me make this Win-friendly!*
If you're using a Mac, much of this may be equally possible using the absolutely fabulous
[Homebrew](http://mxcl.github.com/homebrew/) package management system OSX. I am slightly less inept at Mac
development as I am with Windows development, but you should still help me make this globally deployable

+ Install [Python 2.7](http://www.python.org)
    - You can check your current version by running 'python' from the command line.
+ Install [Redis](http://www.redis.io) 
    - After installing, check the tools folder to configure your server with ease.
+ Install [Redis-Py](http://www.github.com/andymccurdy/redis-py)
    - You can also use easy\_install redis-py if you have Python setuptools already
+ Install [OpenSSL](http://www.openssl.org)
+ Install [CityHash](http://code.google.com/p/cityhash/downloads/list)
+ Install [Wt](http://www.webtoolkit.eu)
    - Depending on your environment, building Wt can be an especially frustrating experience. 
    - I do plan on forking and making an easier-to-build setup of Wt. Until then, refer to their
      docs, which are very complete. If you're on Ubuntu, this process is actually nice and easy.

Additionally, if you plan on contributing to Mogu, it's recommended to have [valgrind](http://www.valgrind.org) installed as well. The valgrind visualizer, `visual valgrind` is included in the debug/ folder of the source, as well as a few bash scripts for easy testing.

## Installing Mogu #
+ Clone this Repository 
    - Either `git clone git://github.com/tomthorogood/Mogu` or [downloading the latest stable release](http://www.github.com/tomthorogood/Mogu/tags)

+ Build the source:

    cd path/to/Mogu
    make

+ Note: You can define the optimization and debug flags using `dbg` and `o` parameters with the make command. Do not try and optimize if you're also trying to debug!

    make o=3
    make dbg=on

+ Debug mode will generate copious amounts of output to your shell when mogu is running, and also will allow you to use tools such as valgrind, etc.     

+ Configure Mogu to suit your needs: 

    vim /etc/mogu/mogu.conf

+ Let 'er rip:

    mogu start

**NOTE: If you get an error about libcityhash not being found**: This is because it is looking for it in /usr/local/lib. Simply run the following command to create a link to the real libcityhash, which may be installed in /usr/lib:

    ln -s /usr/lib/libcityhash.o.0 /usr/local/lib/libcityhash.o.0

## Hello, Web! ##

To build your first Mogu web application, all you need to do is create a folder you want to work from and run

    mogu init
    
You can also set some other variables here, such as your Redis database, port, hostname, and authorization.
These commands can always bet set from the command line, or in your mogu.conf file. Use them liberally, because
with them you can easily copy your site to other databases, back up your site, test changes, and so on. 
Here are the options:

    --redis-host 0.0.0.0    #default is 'localhost'
    --redis-port 0000       #default is 6379
    --redis-select 0        #default is 0
    --redis-auth password   #default is none. Use this on the command line with care...
    --redis-flush           #when importing into your database, you can flush the database first. Don't do this without a backup!
    --assume-yes            #say 'yes' to all prompts -- be careful, especially if combined with redis-flush
    --filename              #when importing or exporting, you must set this to the (relative path to your) filename

The init script creates a single widget, called 'wrapper', which is empty, but will be the place where all other widgets
are added into your application. You can do anything you want to the wrapper with css. It's got a class. The class is called wrapper.

Now, you're ready for hello, world.

    mogu newfile hello

Will create a new file in your current directory. The file will have one line: #!/usr/bin/env python
This is really so that if you use VIM, syntax highlighting will be configured for python already, as we'll be
using Python syntax, even though you *do not need to know Python*.

To create hello world, all you need is this:

    widgets['hello_world'] = {
        type    :   "text",  
        content :   "Hello, World!"
    }
    tree['wrapper'] = ['hello_world']

Really, that's it.
You can then run:

    mogu import hello

Reload localhost:9090.
You should see "Hello World" on your screen. Yay.

The full Mogu manual is also the [Wiki](http://www.github.com/tomthorogood/mogu/wiki)
