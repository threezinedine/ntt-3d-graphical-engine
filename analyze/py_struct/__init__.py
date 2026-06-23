from .parser import *
from .py_enum import *


def register_library():
    """
    Register the libclang library for the current platform.
    """
    import clang.cindex as ci
    import subprocess

    clang_library_file = subprocess.getoutput(
        'find /usr/lib /usr/local/lib -name "libclang.so"'
    )
    ci.Config.set_library_file(clang_library_file)
