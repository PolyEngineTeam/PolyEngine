set(SANDBOX_RENDERING_SRCS
	Src/RenderingSandbox.cpp
	Src/GameManagerSystem.cpp
	Src/GameManagerWorldComponent.cpp
)
set(SANDBOX_RENDERING_INCLUDE Src)
set(SANDBOX_RENDERING_H_FOR_IDE
	Src/RenderingSandbox.hpp
	Src/GameManagerSystem.hpp
	Src/GameManagerWorldComponent.hpp
)

add_library(sandbox_rendering SHARED ${SANDBOX_RENDERING_SRCS} ${SANDBOX_RENDERING_H_FOR_IDE})
target_compile_definitions(sandbox_rendering PRIVATE _GAME)
target_include_directories(sandbox_rendering PRIVATE ${SANDBOX_RENDERING_INCLUDE})
target_link_libraries(sandbox_rendering PRIVATE polyengine)
set_target_properties(sandbox_rendering PROPERTIES OUTPUT_NAME game)

cotire(sandbox_rendering)
