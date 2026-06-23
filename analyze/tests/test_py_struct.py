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


def test_py_struct():
    parser = py_parse(test_code="""
struct Point {
    int x;
    int y;
};
""")

    assert len(parser.structs) == 1
    point_struct = parser.structs[0]
    assert point_struct.name == "Point"
    assert len(point_struct.attributes) == 2
    assert point_struct.attributes[0].name == "x"
    assert point_struct.attributes[0].type == "int"
    assert point_struct.attributes[1].name == "y"
    assert point_struct.attributes[1].type == "int"


def test_py_typedef():
    parser = py_parse(test_code="""
typedef int Integer;

struct Point {
    Integer x;
    Integer y;
};
""")

    assert len(parser.structs) == 1
    point_struct = parser.structs[0]
    assert point_struct.name == "Point"
    assert point_struct.namespace == []
    assert len(point_struct.attributes) == 2
    assert point_struct.attributes[0].name == "x"
    assert point_struct.attributes[0].type == "Integer"
    assert point_struct.attributes[1].name == "y"
    assert point_struct.attributes[1].type == "Integer"

    assert len(parser.typedefs) == 1
    integer_typedef = parser.typedefs[0]
    assert integer_typedef.type == "Integer"
    assert integer_typedef.underlying_type == "int"


def test_py_namespace():
    parser = py_parse(test_code="""
namespace MyNamespace {
    struct Point {
        int x;
        int y;
    };
}
""")

    assert len(parser.structs) == 1
    point_struct = parser.structs[0]
    assert point_struct.name == "Point"
    assert point_struct.namespace == ["MyNamespace"]
    assert len(point_struct.attributes) == 2
    assert point_struct.attributes[0].name == "x"
    assert point_struct.attributes[0].type == "int"
    assert point_struct.attributes[1].name == "y"
    assert point_struct.attributes[1].type == "int"


def test_py_nested_namespace():
    parser = py_parse(test_code="""
namespace Outer {
    namespace Inner {
        struct Point {
            int x;
            int y;
        };
    }
}
""")

    assert len(parser.structs) == 1
    point_struct = parser.structs[0]
    assert point_struct.name == "Point"
    assert point_struct.namespace == ["Outer", "Inner"]
    assert len(point_struct.attributes) == 2
    assert point_struct.attributes[0].name == "x"
    assert point_struct.attributes[0].type == "int"
    assert point_struct.attributes[1].name == "y"
    assert point_struct.attributes[1].type == "int"


def test_py_nested_paring():
    parser = py_parse(test_code="""
namespace First::Second {
    struct Point {
        int x;
        int y;
    };
}
""")

    assert len(parser.structs) == 1
    point_struct = parser.structs[0]
    assert point_struct.name == "Point"
    assert point_struct.namespace == ["First", "Second"]
    assert len(point_struct.attributes) == 2
    assert point_struct.attributes[0].name == "x"
    assert point_struct.attributes[0].type == "int"
    assert point_struct.attributes[1].name == "y"
    assert point_struct.attributes[1].type == "int"
