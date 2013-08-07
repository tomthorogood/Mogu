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

# Building Dependencies 

Within the config/ folder there is a script to help you resolve dependencies.
It requires a utility called 'AC.py' which can be installed by running
`pip install ac`. If you're on a Debian system, it should try to resolve
dependencies for you automatically. 

Once you have `ac` installed, simply run `./configure` to perform the necessary
tests and determine what you need to have installed.

It is highly recommended that you also install the package `cmake-curses-gui`
as well, as sometimes the Wt build utility (which requires CMake), has 
trouble finding Boost, and using this gui is the easiest way to tell it what
to do.

# Installing Mogu

To build mogu, simply run `make` once all of the tests from the `configure`
script come back positive. You can alter the install with the following options.

`flags` will allow you to pass additional flags to the compiler.
    
    make flags="-O2 -DDEBUG"

You may also set the following options:

+ **CONFIG_DIR** - This is where you want to install your configuration files.
  The default is `/usr/share/Mogu`. 

There are two configuration files:

### dbconfig.conf

This file allows you to change the architecture of your Redis install, and tell
Mogu where to find and store information. By default, it uses "localhost" for
all Redis prefixes, with the port at 6379. You can change this anytime, as it is
runtime dependent, not compile-time dependent, leaving you free to scale your
Mogu application databases as needed.

### mogu.conf

This file allows you to change other runtime options, and is heavily commented.

### Static Resources

Witihin mogu.conf, there is an option for the document-root. This folder should
*contain* a folder called "resources". There is a folder within the repository
titled as such, which contains many default resources for some of Wt's 
utilities, but you are more than welcome to come up with your own if you like.


# Getting started

After building you may run `make install` to copy and create symlinks of the
necessary files where they belong. Otherwise, you are welcome to manually
run the mogu server and mogu CLI utilities manually.

After installing, you can run `mogu start` to start the server, which defaults
to port 9090 (but can be changed in the configuration file). The other important
command is `mogu import`, which takes one or more arguments of .mogu files, or 
folders containing .mogu files. 

    `mogu import foo.mogu other_widgets/`

It is recommended to always make sure that your user and group data is stored
in a separate database from all other information, no matter how you devise
your redis configuration, in order to ensure that during the import process, 
nothing bad happens to your dynamic data.

### Writing MoguScript

MoguScript is the language that comes prepackaged with Mogu. It comes with a
Python API so you can do metaprogramming if you want. However, it's stupidly
simple to learn. That was the goal of the project to begin with: let's stop 
having to write at least three languages to make one application.

With Mogu, you can write MoguScript, and leave the HTML, JavaScript, and back
end up to the server.

MoguScript has built-in support for Markdown, dynamic content, and highly
interactive design, with plans for A/V support, and more "forum"-focused 
ideologies coming up very soon.

##[Hello World Tutorial](wiki/tutorial)
