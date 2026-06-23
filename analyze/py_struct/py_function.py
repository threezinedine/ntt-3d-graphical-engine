import clang.cindex as ci
from .py_parameter import PyParameter
from .py_template_parameter import PyTemplateParameter
from .py_base import PyBase


class PyFunction(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.return_type = cursor.result_type.spelling
        self.parameters = self._get_parameters()
        self.is_template = cursor.kind == ci.CursorKind.FUNCTION_TEMPLATE  # type: ignore
        self.templates: list[PyTemplateParameter] = self._get_template_parameters()

    def _get_template_parameters(self):
        templates = []
        if self.is_template:
            for child in self.cursor.get_children():
                if child.kind in (
                    ci.CursorKind.TEMPLATE_TYPE_PARAMETER,  # type: ignore
                    ci.CursorKind.TEMPLATE_NON_TYPE_PARAMETER,  # type: ignore
                ):
                    templates.append(PyTemplateParameter(child))
        return templates

    def _get_parameters(self):
        parameters = []
        for child in self.cursor.get_children():
            if child.kind == ci.CursorKind.PARM_DECL:  # type: ignore
                parameters.append(PyParameter(child))
        return parameters

    def __repr__(self):
        return f"PyFunction(name={self.name}, return_type={self.return_type}, parameters={self.parameters})"
