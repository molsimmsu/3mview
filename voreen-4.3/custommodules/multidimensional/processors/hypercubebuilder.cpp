#include "hypercubebuilder.h"
#include "../utils/primitivegeometrybuilder.h"

const std::string HypercubeBuilder::loggerCat_("voreen.multidimensional.HypercubeBuilder");

#define BUILD_GEOMETRY CallMemberAction<HypercubeBuilder>(this, &HypercubeBuilder::buildGeometry)
#define ROTATE_BASIS CallMemberAction<HypercubeBuilder>(this, &HypercubeBuilder::rotateBasis)
#define RANDOM_ROTATION CallMemberAction<HypercubeBuilder>(this, &HypercubeBuilder::randomRotation)
#define REBUILD_PROJECTIONS CallMemberAction<HypercubeBuilder>(this, &HypercubeBuilder::rebuildProjections)
#define PRINT_BASIS CallMemberAction<HypercubeBuilder>(this, &HypercubeBuilder::printBasis)

HypercubeBuilder::HypercubeBuilder()
    : Processor()
    , outport_(Port::OUTPORT, "outport", "Output geometry")
    , numDimensions_("numDimensions", "Number of dimensions", 4, 3, 20)
    , numProjectionDimensions_("numProjectionDimensions", "Number of projection dimensions", 3, 2, 3)
    , axis1_("axis1", "Axis 1", 1, 1, 5)
    , axis2_("axis2", "Axis 2", 2, 1, 5)
    , rotate_("rotate", "Rotate", 0, -180, 180)
    , numTiles_("numTiles", "Num Tiles", 1, 1, 5)
    , vertexSize_("vertexSize", "Vertex size", 0.2, 0.01, 0.4)
    , drawEdges_("drawEdges", "Draw Edges", true)
    , drawFaces_("drawFaces", "Draw Faces", true)
    , edgeSize_("edgeSize", "Edge size", 0.05, 0.01, 0.1)
    , distance_("distance", "Distance", 5, 0.1, 100)
    , buildGeometry_("buildGeometry", "Build geometry")
    , randomRotation_("randomRotation", "Random Rotation")
    , geometryType_("geometryType", "Geometry type")
    , printBasis_("printBasis", "Print Basis")
    , rotationStep_("rotationStep", "Rotation step", 5, -90, 90)
    , rotateOnce_("rotateOnce", "Rotate once")
 {
    LWARNING("Created processor");
    addPort(outport_);
    
    geometryType_.addOption("grid", "Grid");
    geometryType_.addOption("hypercubeFaces", "Hypercube Faces");
    geometryType_.addOption("penroseSurface", "Penrose Surface");
    
    
    addProperty(geometryType_);
    addProperty(numDimensions_);
    addProperty(numProjectionDimensions_);
    addProperty(numTiles_);
    addProperty(buildGeometry_);
    addProperty(vertexSize_);
    addProperty(drawEdges_);
    addProperty(drawFaces_);
    addProperty(edgeSize_);
    addProperty(distance_);
    addProperty(axis1_);
    addProperty(axis2_);
    addProperty(rotationStep_);
    addProperty(rotateOnce_);
    addProperty(rotate_);
    addProperty(randomRotation_);
    addProperty(printBasis_);
    
    // При вращении трехмерного базиса необходмо перестроить трехмерные проекции:
    // 1) Сдвинуть элементы, отображающие вершины (сферы)
    // 2) Изменить размеры сфер, т.к. меняется расстояние от точки до проекции
    // 3) Перекрасить сферы (по той же причине)
    // 4) Сдвинуть конечные вершины рёбер
    rotate_.onChange(ROTATE_BASIS);
    rotateOnce_.onChange(ROTATE_BASIS);
    buildGeometry_.onChange(BUILD_GEOMETRY);
    randomRotation_.onChange(RANDOM_ROTATION);
    vertexSize_.onChange(REBUILD_PROJECTIONS);
    drawEdges_.onChange(REBUILD_PROJECTIONS);
    drawFaces_.onChange(REBUILD_PROJECTIONS);
    edgeSize_.onChange(REBUILD_PROJECTIONS);
    distance_.onChange(REBUILD_PROJECTIONS);
    printBasis_.onChange(PRINT_BASIS);
    
    buildGeometry();
}

Processor* HypercubeBuilder::create() const {
    return new HypercubeBuilder();
}

void HypercubeBuilder::buildGeometry() {
    size_t N = numDimensions_.get();
    
    space = new Space(N);
    std::cout << space->getNumPoints() << std::endl;

    
    
    VectorXd v(N);
    double step = 1.0;
    for (size_t j = 0; j < N; j++) v(j) = - 0.5 * step * numTiles_.get();

    if (geometryType_.get() == "grid") space->makeGrid(numTiles_.get(), v);
    if (N == 5 && geometryType_.get() == "hypercubeFaces") space->make5DHypercube(v);
    //if (N == 5) space->make5DGrid(2);
    if (N == 5 && geometryType_.get() == "penroseSurface") space->makeQuasiSurface();
    
    //space->printPoints();
    
    size_t dim = numProjectionDimensions_.get();
    
    VectorXi indices(dim);
    if (dim == 2) indices << 0, 1;
    if (dim == 3) indices << 0, 1, 2;
    projection = new ProjectionSubspace(space, indices);
    
    VectorXd diag(N);
    double a = 1.;
    for (size_t i = 0; i < N; i++) diag(i) = a;
    //projection->orientFirstByVector(diag);
    if (dim == 2) projection->makeQuasiBasis2D();
    if (dim == 2 && N == 5) projection->makeQuasiBasis2D_5D_v2();
    if (dim == 3) projection->makeQuasiBasis3D();
    if (dim == 3 && N == 6) projection->makeIcosahedronBasis6D();
    
    projection->projectSpace();
    //projection->printBasis();
    //projection->printPoints();
    //projection->printDistances();
    
    rebuildProjections();
}

void HypercubeBuilder::printBasis() {
    std::cout << "Basis matrix:" << std::endl << projection->getBasisMatrix() << std::endl << std::endl;
}

tgt::vec3 HypercubeBuilder::pointToVec3(const MatrixXd points, size_t j) {
            tgt::vec3 point;
            
            point[0] = points(0, j);
            point[1] = points(1, j);
            if (points.rows() == 2) point[2] = 0;
            else point[2] = points(2, j);
            
            return point;
}

void HypercubeBuilder::rebuildProjections() {
    MeshListGeometry* geom = new MeshListGeometry();

    // Для каждой проекции:
    //for (size_t i = 0; i < numProjections; i++) {
        const MatrixXd points = projection->getPointsMatrix();
        const VectorXd distances = projection->getDistanceVector();
        size_t numPoints = projection->getNumPoints();
        size_t numEdges = projection->getNumEdges();
        size_t numFaces = projection->getNumFaces();
        double maxD = distance_.get();
        // 1) Строим проекции вершин. Отображаем их кубами
        for (size_t j = 0; j < numPoints; j++) {
            double d = distances(j);
            if (d > maxD) continue;
            double a = vertexSize_.get() * 1 / (1 + d) / 10;
            tgt::vec3 cubeSize(a,a,a);
            tgt::vec3 point = pointToVec3(points, j);
            geom->addMesh(MeshGeometry::createCube(point-cubeSize, point+cubeSize));
        }
        
        // 2) Строим проекции ребер. Отображаем их цилиндрами
        if (drawEdges_.get() == true)
        for (size_t j = 0; j < numEdges; j++) {
            Edge e = projection->getEdge(j);
            size_t i1 = e.getV1();
            size_t i2 = e.getV2();
            if (distances(i1) > maxD || distances(i2) > maxD) continue;
            tgt::vec3 v1 = pointToVec3(points, i1);
            tgt::vec3 v2 = pointToVec3(points, i2);
            geom->addMesh(PrimitiveGeometryBuilder::createCylinder(v1, v2, edgeSize_.get() / 10, 4, tgt::vec3(1.,1.,0)));
        }
        
        // 3) Строим проекции граней
        if (drawFaces_.get() == true) {
            MeshGeometry mesh;
            for (size_t j = 0; j < numFaces; j++) {
                Face f = projection->getFace(j);
                
                std::vector<size_t> fv = f.getVertices();
                std::vector<tgt::vec3> projections;
                for (size_t k = 0; k < fv.size(); k++) {
                    projections.push_back(pointToVec3(points, fv[k]));
                }
                
                bool br = false;
                for (size_t k = 0; k < fv.size(); k++) {
                    if (distances(fv[k]) > maxD) br = true;
                }
                if (br == true) continue;
                
                mesh.addFace(PrimitiveGeometryBuilder::createFace(projections, tgt::vec4(1,1,0,0.7)));
            }
            geom->addMesh(mesh);
        }
    //}
    // Delete old geometry and set new
    outport_.setData(geom);
}

void HypercubeBuilder::randomRotation() {
    for (size_t i = 0; i < 10; i++) {
        size_t a1 = rand() % space->getNumDimensions();
        size_t a2 = rand() % space->getNumDimensions();
        double ang = rand() / (double)RAND_MAX;
        projection->applyRotation(BasisRotation(a1, a2, ang));
    }
    rebuildProjections();
    printBasis();
}

void HypercubeBuilder::rotateBasis() {
    projection->applyRotation(BasisRotation(axis1_.get() - 1, axis2_.get() - 1, rotationStep_.get() * PI / 180));
    rebuildProjections();
    printBasis();
}
