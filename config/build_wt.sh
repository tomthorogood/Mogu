export Boost_DIR=/usr/include
export BOOST_INCLUDEDIR=/usr/include/boost
WT_VERSION=3.3.0
BOOST_VERSION=1_49
WT_FOLDER="wt-$WT_VERSION"
WT_LINK="http://downloads.sourceforge.net/project/witty/wt/$WT_VERSION/$WT_FOLDER.tar.gz" 

#wget $WT_LINK
#tar xvf $WT_FOLDER.tar.gz
pushd $WT_FOLDER
mkdir build
pushd build
cmake \
    -DBOOST_VERSION=$BOOST_VERSION \
    ../
