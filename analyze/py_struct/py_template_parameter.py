import clang.cindex as ci


class PyTemplateParameter:
    def __init__(self, cursor: ci.Cursor):
        self.cursor = cursor
        self.name = cursor.spelling
        self.type = self._get_template_keyword(cursor)

    @staticmethod
    def _get_template_keyword(cursor: ci.Cursor) -> str:
        """Extract 'typename' or 'class' keyword from a TEMPLATE_TYPE_PARAMETER."""
        tokens = [t.spelling for t in cursor.get_tokens()]
        return tokens[0] if tokens else "typename"

    def __repr__(self):
        return f"PyTemplateParameter(name={self.name}, type={self.type})"
