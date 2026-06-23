import clang.cindex as ci


class PyTemplateParameter:
    def __init__(self, cursor: ci.Cursor):
        self.cursor = cursor
        self.name = cursor.spelling
        self.type = self._get_type(cursor)

    @staticmethod
    def _get_type(cursor: ci.Cursor) -> str:
        """Extract the type keyword from a template parameter cursor.

        For TEMPLATE_TYPE_PARAMETER (e.g. ``typename T``) returns the keyword.
        For TEMPLATE_NON_TYPE_PARAMETER (e.g. ``float N``) returns the type name.
        """
        tokens = [t.spelling for t in cursor.get_tokens()]
        # tokens[0] is the keyword/type, tokens[-1] is the parameter name
        return tokens[0] if tokens else "typename"

    def __repr__(self):
        return f"PyTemplateParameter(name={self.name}, type={self.type})"
