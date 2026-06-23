import clang.cindex as ci
from .py_base import PyBase


class PyEnum(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.values = self._get_enum_values()

    def _get_enum_values(self):
        values = {}
        for child in self.cursor.get_children():
            if child.kind == ci.CursorKind.ENUM_CONSTANT_DECL:  # type: ignore
                values[child.spelling] = child.enum_value
        return values

    def __repr__(self):
        return f"PyEnum(name={self.name}, values={self.values})"
