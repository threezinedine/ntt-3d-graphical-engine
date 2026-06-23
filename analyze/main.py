from py_struct import *


def main():
    register_library()

    parser = py_parse(test_code="""
enum Color {
    RED,
};
""")
    print(parser.enums)


if __name__ == "__main__":
    main()
