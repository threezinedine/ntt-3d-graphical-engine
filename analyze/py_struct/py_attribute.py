import clang.cindex as ci
from .py_base import PyBase


class PyAttribute(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.type = cursor.type.spelling

    def __repr__(self):
        return f"PyAttribute(name={self.name}, type={self.type})"
