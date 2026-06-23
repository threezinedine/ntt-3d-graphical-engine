from py_struct import *


def test_py_enum():
    parser = py_parse(test_code="""
enum Color {
    RED,
    GREEN,
    BLUE = 3,
};
""")

    assert len(parser.enums) == 1
    color_enum = parser.enums[0]
    assert color_enum.name == "Color"
    assert color_enum.values == {"RED": 0, "GREEN": 1, "BLUE": 3}
