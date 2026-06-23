import clang.cindex as ci
from .py_base import PyBase


class PyParameter(PyBase):
    def __init__(self, cursor: ci.Cursor):
        super().__init__(cursor)
        self.type = self._normalize_type(cursor.type.spelling)
        self.has_default_value = self._check_has_default(cursor)

    @staticmethod
    def _normalize_type(raw_type: str) -> str:
        """Remove spaces before reference (&) and pointer (*) qualifiers."""
        import re

        return re.sub(r"\s+(?=[&*])", "", raw_type)

    @staticmethod
    def _check_has_default(param_cursor: ci.Cursor) -> bool:
        """Check if a PARM_DECL cursor has a default value expression."""
        children = list(param_cursor.get_children())
        # Filter out TypeRef children (they're the type, not default values).
        non_type_children = [c for c in children if c.kind != ci.CursorKind.TYPE_REF]  # type: ignore
        return len(non_type_children) > 0

    def __repr__(self):
        return f"PyParameter(name={self.name}, type={self.type}, has_default_value={self.has_default_value})"
