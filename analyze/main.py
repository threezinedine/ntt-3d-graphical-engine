import os
import argparse
from py_struct import *

CACHE_FILE = os.path.join(os.path.dirname(__file__), ".cache")


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
        assert os.path.exists(
            CACHE_FILE
        ), "Cache file not found. Please provide a path."
        with open(CACHE_FILE, "r") as file:
            final_path = file.read().strip()

    if os.name == "nt":
        assert (
            final_path is not None
        ), "On Windows, you must provide the path to libclang.dll"
        register_library(final_path)
    else:
        if os.path.exists(final_path):
            register_library(final_path)
        else:
            register_library()

    parser = py_parse(test_code="""
enum Color {
    RED,
};
""")
    print(parser.enums)

    with open(CACHE_FILE, "w") as file:
        file.write(final_path)


if __name__ == "__main__":
    main()
