set(
    NTT_ENGINE_SOURCES
    "main_engine.cpp"
)

add_executable(
    ntt-engine
    ${NTT_ENGINE_SOURCES}
)

target_link_libraries(
    ntt-engine
    PRIVATE
    ntt-core
)