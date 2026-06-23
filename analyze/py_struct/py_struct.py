import clang.cindex as ci
from .py_attribute import PyAttribute
from .py_base import PyBase


class PyStruct(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.attributes = self._get_attributes()

    def _get_attributes(self):
        attributes = []
        for child in self.cursor.get_children():
            if child.kind == ci.CursorKind.FIELD_DECL:  # type: ignore
                attributes.append(PyAttribute(child))
        return attributes

    def __repr__(self):
        return f"PyStruct(name={self.name}, attributes={self.attributes})"
