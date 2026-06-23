import pytest
from py_struct import register_library


@pytest.fixture(scope="function", autouse=True)
def register_library_fixture():
    """
    Fixture to register the library before each test function.
    """

    register_library()

    yield
