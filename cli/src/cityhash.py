# Runs the C/C++ City Hash script that is built when the
# CLI is installed and returns the output.
import subprocess

def to_city_hash(string):
    proc = subprocess.Popen(
            ["/etc/mogu/c/clihash", string],
            shell=False, stdout=subprocess.PIPE)
    return proc.communicate()[0].replace("\n","")

class Hash:
    meta = to_city_hash("meta")
    prev = to_city_hash("prev")
    auth = to_city_hash("auth")
    session_lookup = to_city_hash("session_lookup")
    auth_lookup = to_city_hash("auth_lookup")
    salt_lookup = to_city_hash("salt")
    userid_lookup = to_city_hash("userid_lookup")
    all_auths = "4801973836335212534"
