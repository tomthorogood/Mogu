#!/usr/bin/env python
import sys
import ac
import alternatives as alt

ac.get_shell_output("gcc","-dumpversion")
GCC_VERSION = \
        float(ac.get_shell_output("gcc","-dumpversion")[0])

ac.require("Python Version 2.7", 
        ((sys.version_info.major == 2) and (sys.version_info.minor==7)))
ac.require("GCC 4.7", (GCC_VERSION >= 4.7))
ac.testexe("git", required=True)
ac.testexe("sudo", required=True)
ac.testexe("make",True)
ac.testlib("turnleft")
ac.testlib("boost_signals")
ac.testlib("boost_random", header="boost/random.hpp")
ac.testlib("boost_system")
ac.testlib("boost_program_options")
ac.testlib("boost_filesystem")
ac.testlib("boost_thread")
ac.testexe("cmake")
ac.testlib("fontconfig")
ac.testlib("glib-2.0")
ac.testlib("pango-1.0")
ac.testlib("ssl")
ac.testlib("hiredis",header="hiredis/hiredis.h")
ac.testlib("wt",header="Wt/WContainerWidget")
ac.testexe("pip")
ac.clean()
