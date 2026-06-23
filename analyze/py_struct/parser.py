import clang.cindex as ci
from .py_enum import PyEnum
from .py_struct import PyStruct
from .py_typedef import PyTypedef
from .py_base import PyBase
from .py_function import PyFunction
from .py_class import PyClass


class PyParse:
    enums: list[PyEnum] = []
    structs: list[PyStruct] = []
    typedefs: list[PyTypedef] = []
    functions: list[PyFunction] = []
    classes: list[PyClass] = []

    def __init__(self):
        self.enums = []
        self.structs = []
        self.typedefs = []
        self.functions = []


def py_parse(
    source_file: str | None = None,
    test_code: str | None = None,
    include_dirs: list[str] | None = None,
) -> PyParse:
    assert source_file or test_code, "Either source_file or test_code must be provided."

    index = ci.Index.create()
    py_parse = PyParse()

    if source_file:
        tu = index.parse(
            path=source_file,
            args=["-std=c++17", "-x", "c++"]
            + ["-I" + dir for dir in include_dirs or []],
        )
    else:
        tu = index.parse(
            path="dummy.cpp",
            args=["-std=c++17"] + ["-I" + dir for dir in include_dirs or []],
            unsaved_files=[("dummy.cpp", test_code)],
        )

    for child in tu.cursor.get_children():
        _parse_elements(child, py_parse)

    return py_parse


def _parse_elements(cursor: ci.Cursor, py_parse: PyParse):
    if cursor.kind == ci.CursorKind.NAMESPACE:  # type: ignore
        PyBase.current_namespace.append(cursor.spelling)
        for child in cursor.get_children():
            _parse_elements(child, py_parse)
        PyBase.current_namespace.pop()
    elif cursor.kind == ci.CursorKind.ENUM_DECL:  # type: ignore
        py_parse.enums.append(PyEnum(cursor))
    elif cursor.kind == ci.CursorKind.STRUCT_DECL:  # type: ignore
        py_parse.structs.append(PyStruct(cursor))
    elif cursor.kind == ci.CursorKind.TYPEDEF_DECL:  # type: ignore
        py_parse.typedefs.append(PyTypedef(cursor))
    elif cursor.kind == ci.CursorKind.FUNCTION_DECL or cursor.kind == ci.CursorKind.FUNCTION_TEMPLATE:  # type: ignore
        py_parse.functions.append(PyFunction(cursor))
    elif cursor.kind == ci.CursorKind.CLASS_DECL:  # type: ignore
        py_parse.classes.append(PyClass(cursor))
