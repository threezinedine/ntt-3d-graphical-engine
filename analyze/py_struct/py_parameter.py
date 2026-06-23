import clang.cindex as ci
from .py_base import PyBase


class PyParameter(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.type = cursor.type.spelling
        self.default_value = self._get_default_value(cursor)

    def _get_default_value(self, param_cursor: ci.Cursor) -> str | None:
        """
        Given a PARM_DECL cursor, retrieves its default argument value as a string.
        Returns None if no default value exists.
        """
        children = list(param_cursor.get_children())

        if not children:
            return None

        default_expr_node = children[-1]

        tokens = [token.spelling for token in default_expr_node.get_tokens()]

        if tokens:
            return "".join(tokens)

        return default_expr_node.spelling if default_expr_node.spelling else None

    def __repr__(self):
        return f"PyParameter(name={self.name}, type={self.type}, default_value={self.default_value})"
