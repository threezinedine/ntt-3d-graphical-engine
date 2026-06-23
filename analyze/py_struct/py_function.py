import clang.cindex as ci
from .py_parameter import PyParameter
from .py_base import PyBase


class PyFunction(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.return_type = cursor.result_type.spelling
        self.parameters = self._get_parameters()

    def _get_parameters(self):
        parameters = []
        for child in self.cursor.get_children():
            if child.kind == ci.CursorKind.PARM_DECL:  # type: ignore
                parameters.append(PyParameter(child))
        return parameters

    def __repr__(self):
        return f"PyFunction(name={self.name}, return_type={self.return_type}, parameters={self.parameters})"
