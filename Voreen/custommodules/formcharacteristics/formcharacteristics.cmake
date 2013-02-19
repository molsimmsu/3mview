
# module class must reside in sample/samplemodule.h + sample/samplemodule.cpp
SET(MOD_CORE_MODULECLASS FormCharacteristicsModule)

# module's core source files, path relative to module dir
SET(MOD_CORE_SOURCES
    ${MOD_DIR}/datastructures/weightedpointcloud.cpp
    ${MOD_DIR}/ports/weightedpointcloudport.cpp
    #${MOD_DIR}/processors/pointcloudcharacteristics.cpp
    ${MOD_DIR}/processors/alignbymoments.cpp
)

# module's core header files, path relative to module dir
SET(MOD_CORE_HEADERS
    ${MOD_DIR}/datastructures/weightedpointcloud.h
    ${MOD_DIR}/ports/weightedpointcloudport.h
    #${MOD_DIR}/processors/pointcloudcharacteristics.h
    ${MOD_DIR}/processors/alignbymoments.h
)   
