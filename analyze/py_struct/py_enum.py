import clang.cindex as ci


class PyEnum:
    def __init__(self, cursor: ci.Cursor):
        self.cursor = cursor
        self.name = cursor.spelling
        self.values = self._get_enum_values()

    def _get_enum_values(self):
        values = {}
        for child in self.cursor.get_children():
            if child.kind == ci.CursorKind.ENUM_CONSTANT_DECL:
                values[child.spelling] = child.enum_value
        return values

    def __repr__(self):
        return f"PyEnum(name={self.name}, values={self.values})"
