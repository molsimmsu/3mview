
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS StereoModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    #${MOD_DIR}/processors/stereocanvasrenderer.cpp
    ${MOD_DIR}/processors/stereomeshentryexitpoints.cpp
    ${MOD_DIR}/interaction/stereocamerainteractionhandler.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    #${MOD_DIR}/processors/stereocanvasrenderer.h
    ${MOD_DIR}/processors/stereomeshentryexitpoints.h
    ${MOD_DIR}/interaction/stereocamerainteractionhandler.h
)   
