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
    , numDimensions_("numDimensions", "Number of dimensions", 4, 3, 10)
    , numProjectionDimensions_("numProjectionDimensions", "Number of projection dimensions", 3, 2, 3)
    , axis1_("axis1", "Axis 1", 1, 1, 5)
    , axis2_("axis2", "Axis 2", 2, 1, 5)
    , rotate_("rotate", "Rotate", 0, -180, 180)
    , numTiles_("numTiles", "Num Tiles", 1, 1, 5)
    , vertexSize_("vertexSize", "Vertex size", 0.2, 0.01, 5)
    , drawEdges_("drawEdges", "Draw Edges", true)
    , edgeSize_("edgeSize", "Edge size", 0.05, 0.01, 0.1)
    , distance_("distance", "Distance", 5, 0.1, 10)
    , buildGeometry_("buildGeometry", "Build geometry")
    , randomRotation_("randomRotation", "Random Rotation")
    , geometryType_("geometryType", "Geometry type")
    , printBasis_("printBasis", "Print Basis")
    , drawQuasiEdges_("drawQuasiEdges", "Draw Quasi Edges", false)
    , quasiEdgeDistance_("quasiEdgeDistance", "Quasi Edge Distance", 5, 0.1, 10)
 {
    LWARNING("Created processor");
    addPort(outport_);
    
    geometryType_.addOption("hypercube", "Hypercube");
    
    addProperty(geometryType_);
    addProperty(numDimensions_);
    addProperty(numProjectionDimensions_);
    addProperty(numTiles_);
    addProperty(buildGeometry_);
    addProperty(vertexSize_);
    addProperty(drawEdges_);
    addProperty(edgeSize_);
    addProperty(distance_);
    addProperty(drawQuasiEdges_);
    addProperty(quasiEdgeDistance_);
    addProperty(axis1_);
    addProperty(axis2_);
    addProperty(rotate_);
    addProperty(randomRotation_);
    addProperty(printBasis_);
    
    // При вращении трехмерного базиса необходмо перестроить трехмерные проекции:
    // 1) Сдвинуть элементы, отображающие вершины (сферы)
    // 2) Изменить размеры сфер, т.к. меняется расстояние от точки до проекции
    // 3) Перекрасить сферы (по той же причине)
    // 4) Сдвинуть конечные вершины рёбер
    rotate_.onChange(ROTATE_BASIS);
    buildGeometry_.onChange(BUILD_GEOMETRY);
    randomRotation_.onChange(RANDOM_ROTATION);
    vertexSize_.onChange(REBUILD_PROJECTIONS);
    drawEdges_.onChange(REBUILD_PROJECTIONS);
    drawQuasiEdges_.onChange(REBUILD_PROJECTIONS);
    quasiEdgeDistance_.onChange(REBUILD_PROJECTIONS);
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
    
    double offset = numTiles_.get() * step * 0.5;
    
    for (size_t j = 0; j < N; j++) v(j) = -offset;
    
    space->addPoint(v);
    
    for (size_t i = 0; i < N; i++) { // По каждому измерению
        size_t np = space->getNumPoints();
        for (size_t j = 0; j < np; j++) { // Для всех точек без учета новых
            for (size_t k = 1; k <= numTiles_.get(); k++) { // дублируем их координаты, и транслируем по одной оси K раз
                if (k == 1) {
                    v = space->getPoint(j);
                    v(i) += step;
                    space->addPoint(v);
                    size_t newIndex = space->getNumPoints() - 1;
                    space->addEdge(newIndex,j);
                }
                else {
                    size_t i1 = space->getNumPoints() - 1;
                    v = space->getPoint(i1);
                    v(i) += step;
                    space->addPoint(v);
                    size_t i2 = space->getNumPoints() - 1;
                    space->addEdge(i1,i2);
                }
                
            }
        }
    }
    
    size_t np = space->getNumPoints();
    
    // Добавляем рёбра
    /*for (size_t i = 0; i < np; i++) {
        for (size_t j = i+1; j < np; j++) {
            if (space->pointDistance(i,j) == 1.0) {
                space->addEdge(i,j);
            }
        }
    }*/
    
    space->printPoints();
    
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
    
    projection->projectSpace();
    projection->printBasis();
    projection->printPoints();
    projection->printDistances();
    
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

        const MatrixXd points = projection->getPointsMatrix();
        const VectorXd distances = projection->getDistanceVector();
        size_t numPoints = projection->getNumPoints();
        size_t numEdges = projection->getNumEdges();
        // 1) Строим проекции вершин. Отображаем их кубами
        for (size_t j = 0; j < numPoints; j++) {
            double d = distances(j);
            if (d > distance_.get()) continue;
            double a = vertexSize_.get() * 1 / (1 + d) / 10;
            tgt::vec3 cubeSize(a,a,a);
            tgt::vec3 point = pointToVec3(points, j);
            geom->addMesh(MeshGeometry::createCube(point-cubeSize, point+cubeSize));
        }
        
        // 2) Строим проекции ребер. Отображаем их цилиндрами
        if (drawEdges_.get() == true)
        for (size_t j = 0; j < numEdges; j++) {
            Edge e = projection->getEdge(j);
            tgt::vec3 v1 = pointToVec3(points, e.getV1());
            tgt::vec3 v2 = pointToVec3(points, e.getV2());
            geom->addMesh(PrimitiveGeometryBuilder::createCylinder(v1, v2, edgeSize_.get() / 10, 4, tgt::vec3(1.,1.,0)));
        }
        
        // 3) Строим проекции квази-ребер (например, мозаики Пенроуза)
        if (drawQuasiEdges_.get() == true) {
        double t = 0.01;
        double d = quasiEdgeDistance_.get();
        projection->buildQuasiEdgesByDistance(d-t, d+t, distance_.get(), 0);
        for (size_t i = 0; i < projection->getNumQuasiEdgeClasses(); i++)
        for (size_t j = 0; j < projection->getNumQuasiEdges(i); j++) {
            Edge e = projection->getQuasiEdge(i, j);
            tgt::vec3 v1 = pointToVec3(points, e.getV1());
            tgt::vec3 v2 = pointToVec3(points, e.getV2());
            geom->addMesh(PrimitiveGeometryBuilder::createCylinder(v1, v2, edgeSize_.get() / 10, 4, tgt::vec3(1.,1.,0)));
        }
        }

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
    projection->applyRotation(BasisRotation(axis1_.get() - 1, axis2_.get() - 1, PI / 180));
    rebuildProjections();
    printBasis();
}
