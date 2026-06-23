import clang.cindex as ci


class PyBase:
    current_namespace: list[str] = []

    def __init__(self, cursor: ci.Cursor):
        self.cursor = cursor
        self.name = cursor.spelling
        self.namespace = PyBase.current_namespace.copy()

    def __repr__(self):
        return f"PyBase(name={self.name}, namespace={self.namespace})"
