import os
import argparse
from py_struct import *

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ENGINE_DIR = os.path.join(os.path.dirname(BASE_DIR), "engine")
CACHE_FILE = os.path.join(BASE_DIR, ".cache")


def main():
    parser = argparse.ArgumentParser(description="Analyze C++ code")
    parser.add_argument(
        "--path",
        type=str,
        required=False,
        help="Path to the C++ file or directory to analyze",
    )

    args = parser.parse_args()

    final_path = args.path

    if args.path is None:
        if os.path.exists(CACHE_FILE):
            with open(CACHE_FILE, "r") as file:
                final_path = file.read().strip()

    if os.name == "nt":
        assert (
            final_path is not None
        ), "On Windows, you must provide the path to libclang.dll"
        register_library(final_path)
    else:
        if final_path is not None and os.path.exists(final_path):
            register_library(final_path)
        else:
            register_library()

    parser = py_parse(
        source_file=os.path.join(ENGINE_DIR, "core", "core.h"),
        include_dirs=[os.path.join(ENGINE_DIR, "core")],
    )
    print("Enums:")
    print("\n".join(["\t" + str(enum) for enum in parser.enums]))

    if final_path is not None:
        with open(CACHE_FILE, "w") as file:
            file.write(final_path)


if __name__ == "__main__":
    main()
