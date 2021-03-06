################################################################################
# Package: SimG4Full
################################################################################
gaudi_subdir(SimG4Full v1r0)

# this declaration will not be needed in the future
gaudi_depends_on_subdirs(GaudiAlg Sim/SimG4Components Sim/SimG4Interface Sim/SimG4Common)

find_package(Geant4)
include(${Geant4_USE_FILE})
find_package(ROOT)

gaudi_install_headers(SimG4Full)

gaudi_add_library(SimG4Full
                 src/lib/*.cpp
                 INCLUDE_DIRS SimG4Common FWCore SimG4Components SimG4Interface Geant4 ROOT
                 LINK_LIBRARIES GaudiAlgLib SimG4Common Geant4 ROOT
                 PUBLIC_HEADERS SimG4Full)

gaudi_add_module(SimG4FullPlugins
                 src/components/*.cpp
                 INCLUDE_DIRS Geant4 SimG4Common SimG4Full SimG4Interface
                 LINK_LIBRARIES GaudiAlgLib Geant4 SimG4Full SimG4Common)
