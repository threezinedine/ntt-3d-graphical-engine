import os
from .parser import *
from .py_enum import *
from pathlib import Path
import clang.cindex as ci


def register_library(force_file: str | None = None):
    """
    Register the libclang library for the current platform.
    """
    if os.name == "nt":
        assert (
            force_file is not None
        ), "On Windows, you must provide the path to libclang.dll"
        ci.Config.set_library_file(force_file)
    else:
        import subprocess

        if force_file is not None:
            ci.Config.set_library_file(force_file)
        else:
            clang_library_file = subprocess.getoutput(
                'find /usr/lib /usr/local/lib -name "libclang.so"'
            )
            ci.Config.set_library_file(clang_library_file)
