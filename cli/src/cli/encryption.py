import subprocess
def encrypt(string):
    proc = subprocess.Popen(
            ["/etc/mogu/c/clidec", "encrypt", string],
            shell=False, stdout=subprocess.PIPE)
    return proc.communicate()[0].replace("\n","")

def decrypt(string):
    proc = subprocess.Popen(
            ["/etc/mogu/c/clidec", "decrypt", string],
            shell=False, stdout=subprocess.PIPE)
    return proc.communicate()[0].replace("\n","")

