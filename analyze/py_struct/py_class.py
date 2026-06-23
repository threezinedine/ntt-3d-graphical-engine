import clang.cindex as ci
from .py_base import PyBase
from .py_attribute import PyAttribute
from .py_function import PyFunction


class PyClassAttribute(PyAttribute):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.is_static = cursor.storage_class == ci.StorageClass.STATIC  # type: ignore
        self.is_public = cursor.access_specifier == ci.AccessSpecifier.PUBLIC  # type: ignore
        self.is_protected = cursor.access_specifier == ci.AccessSpecifier.PROTECTED  # type: ignore
        self.is_private = cursor.access_specifier == ci.AccessSpecifier.PRIVATE  # type: ignore

    def __repr__(self):
        return f"PyClassAttribute(name={self.name}, type={self.type}, is_static={self.is_static})"


class PyClassFunction(PyFunction):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.is_static = cursor.is_static_method()  # type: ignore
        self.is_public = cursor.access_specifier == ci.AccessSpecifier.PUBLIC  # type: ignore
        self.is_protected = cursor.access_specifier == ci.AccessSpecifier.PROTECTED  # type: ignore
        self.is_private = cursor.access_specifier == ci.AccessSpecifier.PRIVATE  # type: ignore

    def __repr__(self):
        return f"PyClassFunction(name={self.name}, return_type={self.return_type}, parameters={self.parameters}, is_static={self.is_static})"


class PyClass(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.attributes = self._get_attributes()
        self.functions = self._get_functions()

    def _get_attributes(self):
        attributes = []
        for child in self.cursor.get_children():
            if child.kind in (
                ci.CursorKind.FIELD_DECL,  # type: ignore
                ci.CursorKind.VAR_DECL,  # type: ignore
            ):
                attributes.append(PyClassAttribute(child))
        return attributes

    def _get_functions(self):
        functions = []
        for child in self.cursor.get_children():
            if child.kind in (
                ci.CursorKind.CXX_METHOD,  # type: ignore
                ci.CursorKind.FUNCTION_TEMPLATE,  # type: ignore
            ):
                functions.append(PyClassFunction(child))
        return functions

    def __repr__(self):
        return f"PyClass(name={self.name}, attributes={self.attributes}, functions={self.functions})"
