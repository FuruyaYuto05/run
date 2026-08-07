# Install script for directory: C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/CG44/project/vcpkg_installed/vcpkg/pkgs/assimp_x64-windows/debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.4-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/x64-windows-dbg/lib/assimp-vc145-mtd.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp6.0.4" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/x64-windows-dbg/bin/assimp-vc145-mtd.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/anim.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/aabb.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ai_assert.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/camera.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/color4.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/color4.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/x64-windows-dbg/code/../include/assimp/config.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ColladaMetaData.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/commonMetaData.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/defs.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/cfileio.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/light.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/material.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/material.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/matrix3x3.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/matrix3x3.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/matrix4x4.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/matrix4x4.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/mesh.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ObjMaterial.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/pbrmaterial.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/GltfMaterial.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/postprocess.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/quaternion.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/quaternion.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/scene.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/metadata.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/texture.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/types.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/vector2.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/vector2.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/vector3.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/vector3.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/version.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/cimport.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/AssertHandler.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/importerdesc.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Importer.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/DefaultLogger.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ProgressHandler.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/IOStream.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/IOSystem.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Logger.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/LogStream.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/NullLogger.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/cexport.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Exporter.hpp"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/DefaultIOStream.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/DefaultIOSystem.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ZipArchiveIOSystem.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SceneCombiner.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/fast_atof.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/qnan.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/BaseImporter.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Hash.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/MemoryIOWrapper.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ParsingUtils.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/StreamReader.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/StreamWriter.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/StringComparison.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/StringUtils.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SGSpatialSort.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/GenericProperty.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SpatialSort.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SkeletonMeshBuilder.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SmallVector.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SmoothingGroups.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/SmoothingGroups.inl"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/StandardShapes.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/RemoveComments.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Subdivision.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Vertex.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/LineSplitter.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/TinyFormatter.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Profiler.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/LogAux.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Bitmap.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/XMLTools.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/IOStreamBuffer.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/CreateAnimMesh.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/XmlParser.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/BlobIOSystem.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/MathFunctions.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Exceptional.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/ByteSwapper.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Compiler/pushpack1.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Compiler/poppack1.h"
    "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/src/v6.0.4-12c3574bf8.clean/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/CG44/project/vcpkg_installed/vcpkg/blds/assimp/x64-windows-dbg/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
