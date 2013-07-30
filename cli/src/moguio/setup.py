from setuptools import setup, find_packages

import os
import sys

cwd = os.getcwd()
syntax_file_exists = os.path.exists(os.path.join(cwd,"moguio","syntax.py"))

if not syntax_file_exists:
    sys.exit("No syntax file found. Installation cannot continue. run 'make syntax' from Mogu's root directory.")


readme = None
with open("README.md") as readme_md:
    readme = readme_md.read()

setup(
        name = "moguio",
        version = "0.1.0a",
        description = readme,
        long_description = readme,
        author = "Tom A. Thorogood",
        author_email = "tom@tomthorogood.com",
        license = "GPLv3",
        url="http://www.github.com/tomthorogood/mogu",
        packages = find_packages(exclude=['setup.py','tests']),
        zip_safe = True,
        install_requires= ["MultiString", "pyboro", "markdown"]
)
