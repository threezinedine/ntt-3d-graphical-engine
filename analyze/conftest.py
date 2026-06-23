import os
import pytest
from py_struct import register_library


@pytest.fixture(scope="function", autouse=True)
def register_library_fixture():
    """
    Fixture to register the library before each test function.
    """

    CACHE_FILE = os.path.join(os.path.dirname(__file__), ".cache")
    final_path = None

    if os.path.exists(CACHE_FILE):
        with open(CACHE_FILE, "r") as file:
            final_path = file.read().strip()

    if os.name == "nt":
        assert (
            final_path is not None
        ), "On Windows, you must provide the path to libclang.dll"
        register_library(final_path)
    else:
        if final_path is not None and os.path.exists(final_path):
            register_library(final_path)
        else:
            register_library()

    yield
