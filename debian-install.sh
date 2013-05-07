#!/bin/sh

BOOST_VERSION=1.49
REDIS_VERSION=2.6.13
WT_VERSION=3.3.0
PYTHON_VERSION=2.7
GCC_VERSION=4.7


WT_FOLDER = wt-$WT_VERSION
REDIS_FOLDER = redis-$REDIS_VERSION
WT_LINK=http://downloads.sourceforge.net/project/witty/wt/$WT_VERSION/$WT_FOLDER.tar.gz
REDIS_LINK=http://redis.googlecode.com/files/$REDIS_FOLDER.tar.gz

# Enable the Python2.7 repository
sudo echo "deb http://ftp.us.debian.org/debian testing main contrib" \
    /etc/apt/sources.list
sudo apt-get update

sudo apt-get install \
    python$PYTHON_VERSION \
    python-pip \
    git \
    gcc-$GCC_VERSION \
    make \
    libboost-dev \
    libboost-date-time$BOOST_VERSION-dev \
    libboost-program-options$BOOST_VERSION-dev \
    libboost-random$BOOST_VERSION-dev \
    libboost-signals$BOOST_VERSION-dev \
    libboost-system$BOOST_VERSION-dev \
    libboost-filesystem$BOOST_VERSION-dev \
    libboost-thread$BOOST_VERSION-dev \
    cmake \
    cmake-curses-gui \
    libhpdf-dev \
    libfcgi-dev \
    libfontconfig-dev \
    libglib2.0-dev \
    zlib1g-dev \
    libpango1.0-dev \
    libgmlib-dev \
    libmagick-dev \
    libssl-dev


wget $REDIS_LINK
cd $REDIS_FOLDER && make && sudo make install
git clone https://github.com/redis/hiredis.git
cd hiredis && make && sudo make install

sudo pip install virtualenv
cd ~/
mkdir p27
virtualenv p27
source p27/bin/activate

sudo pip install \
    redis \
    pyboro \
    multistring

wget $WT_LINK
mkdir $WT_FOLDER/build
cd $WT_FOLDER/build
cmake ../ && make && sudo make install

cd ~/
git clone https://github.com/tomthorogood/Mogu.git
cd Mogu
checkout lex
make
