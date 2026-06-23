import clang.cindex as ci


class PyTypedef:
    def __init__(self, cursor: ci.Cursor):
        self.cursor = cursor
        self.type = cursor.type.spelling
        self.underlying_type = cursor.underlying_typedef_type.spelling

    def __repr__(self):
        return f"PyTypedef(from={self.type}, to={self.underlying_type})"
