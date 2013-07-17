import ac

BOOST_VERSION="1.49"
REDIS_VERSION="2.6.13"
WT_VERSION="3.3.0"

REDIS_FOLDER = "redis-%s" % REDIS_VERSION
REDIS_LINK = "http://redis.googlecode.com/files/%s.tar.gz" % REDIS_FOLDER

WT_FOLDER = "wt-%s" % WT_VERSION
WT_LINK = "http://downloads.sourceforge.net/project/witty/wt/%s/%s.tar.gz" % \
        (WT_VERSION,WT_FOLDER)

ac.if_fail("git", ("debian","ubuntu"), "sudo apt-get install git")
ac.if_fail("git", ("centos","redhat"), "sudo yum install git")
ac.if_fail("make", ("debian","ubuntu"), "sudo apt-get install make")
ac.if_fail("make", ("centos","redhat"), "sudo yum install make")
ac.if_fail("turnleft", ac.ALL, """
git clone git://github.com/tomthorogood/TurnLeftLib
pushd TurnLeftLib
make && sudo make install
popd
rm -rf TurnLeftLib
""")

ac.if_fail("cityhash", ac.ALL, """
pushd ../utils
./configure
make
sudo make install
popd
""")

ac.if_fail("boost_system", "debian",
    "sudo apt-get install libboost-system%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_signals", "debian", 
    "sudo apt-get install libboost-signals%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_random", "debian",
    "sudo apt-get install libboost-random%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_program_options", "debian",
    "sudo apt-get install libboost-program-options%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_filesystem", "debian",
    "sudo apt-get install libboost-filesystem%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_thread", "debian",
    "sudo apt-get install libboost-thread%s-dev --yes" % BOOST_VERSION)
ac.if_fail("boost_regex", "debian",
    "sudo apt-get install libboost-regex%s-dev --yes" % BOOST_VERSION)
ac.if_fail("cmake", ("debian","ubuntu"), "sudo apt-get install cmake --yes")
ac.if_fail("cmake", ("centos","redhat"), "sudo yum install cmake --yes")
ac.if_fail("fontconfig", "debian", "sudo apt-get install libfontconfig-dev --yes")
ac.if_fail("glib-2.0", "debian", "sudo apt-get install libglib2.0-dev --yes")
ac.if_fail("redis-cli", ac.ALL, """
wget %s
tar xvf %s
pushd %s
make && sudo make install
popd
rm -rf %s
""" % (REDIS_LINK, "%s.tar.gz" % REDIS_FOLDER, REDIS_FOLDER, REDIS_FOLDER)
)

wt_alternative = None
with open("build_wt.sh") as f:
    wt_alternative = f.read()

ac.if_fail("wt", ac.ALL, wt_alternative)
ac.if_fail("pango-1.0", "debian", "sudo apt-get install libpango1.0-dev --yes")
ac.if_fail("ssl", "debian", "sudo apt-get install libssl-dev --yes")
ac.if_fail("hiredis", ac.ALL, """
git clone git://github.com/redis/hiredis.git
pushd hiredis
make && sudo make install
popd
rm -rf hiredis
""")
ac.if_fail("pip", ("debian","ubuntu"), "sudo apt-get install python-pip")
