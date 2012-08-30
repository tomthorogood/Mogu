# Runs the C/C++ City Hash script that is built when the
# CLI is installed and returns the output.
import subprocess

def to_city_hash(string):
    proc = subprocess.Popen(
            ["/etc/mogu/cli_src/clihash", string],
            shell=False, stdout=subprocess.PIPE)
    return proc.communicate()[0].replace("\n","")
