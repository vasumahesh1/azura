project(Physics)

# -----------  Main Target  -----------

set(
  SOURCES
  "Inc/Physics/Geometry/ClothPlane.h"
  "Inc/Physics/Geometry/ClothMesh.h"
  "Inc/Physics/Geometry/Edge.h"
  "Inc/Physics/Geometry/GenericTypes.h"
  "Inc/Physics/Geometry/IPhysicsGeometry.h"
  "Inc/Physics/Geometry/IClothPhysicsGeometry.h"
  "Inc/Physics/PBD/Cloth/ClothSolvingView.h"
  "Inc/Physics/PBD/Cloth/Constraints.h"
  "Inc/Physics/PBD/Common/SolvingView.h"

  "Src/Geometry/ClothPlane.cpp"
  "Src/Geometry/ClothMesh.cpp"
  "Src/Geometry/Edge.cpp"
  "Src/PBD/Cloth/ClothSolvingView.cpp"
  "Src/PBD/Cloth/Constraints.cpp"
  "Src/PBD/Common/SolvingView.cpp"
)

add_library(Physics ${SOURCES})

target_include_directories(Physics PUBLIC ${PROJECT_SOURCE_DIR}/Inc)

target_link_libraries(Physics PUBLIC AssetManager Common Core Math Containers)