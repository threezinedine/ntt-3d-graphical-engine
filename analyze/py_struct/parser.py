import clang.cindex as ci
from .py_enum import PyEnum


class PyParse:
    enums: list[PyEnum] = []


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
            args=["-std=c++17"] + ["-I" + dir for dir in include_dirs or []],
        )
    else:
        tu = index.parse(
            path="dummy.cpp",
            args=["-std=c++17"] + ["-I" + dir for dir in include_dirs or []],
            unsaved_files=[("dummy.cpp", test_code)],
        )

    for child in tu.cursor.get_children():
        if child.kind == ci.CursorKind.ENUM_DECL:
            py_parse.enums.append(PyEnum(child))

    return py_parse
