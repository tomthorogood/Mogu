export Boost_DIR=/usr/include
export BOOST_INCLUDEDIR=/usr/include/boost
WT_VERSION=3.3.0
BOOST_VERSION=1_49
WT_FOLDER="wt-$WT_VERSION"
WT_LINK="http://downloads.sourceforge.net/project/witty/wt/$WT_VERSION/$WT_FOLDER.tar.gz" 

if [ -d $WT_FOLDER/build ]; then
    rm -rf $WT_FOLDER/build
fi

if [ ! -f $WT_FOLDER.tar.gz ]; then 
    wget $WT_LINK
fi

if [ ! -d $WT_FOLDER ]; then
    tar xvf $WT_FOLDER.tar.gz
fi

pushd $WT_FOLDER
mkdir build
pushd build
cmake \
    -DBOOST_DIR=/usr \
    -DBoost_FOUND=TRUE \
    ../
