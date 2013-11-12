#include <vector>                // Для хранения массивов
#include <cmath>                 // Для тригонометрии

#include <Eigen/Core>
#include <Eigen/QR>
using namespace Eigen;

#define PI 3.141592

// VectorXd - вектор-столбец с динамически задаваемым числом элементов
// MatrifXd - матрица с динамически задаваемым числом элементов

class BasisRotation {
public:
    BasisRotation(size_t axis1_, size_t axis2_, double angle_)
      : axis1(axis1_)
      , axis2(axis2_)
      , angle(angle_)
    {}
    
    size_t axis1, axis2;
    double angle;
};

class Edge {
public:
    Edge(size_t v1, size_t v2)
    : v1_(v1)
    , v2_(v2)
    {}
    
    size_t getV1() const {
        return v1_;
    }
    
    size_t getV2() const {
        return v2_;
    }
    
private:
    size_t v1_,v2_;
};

class Face {
public:
    Face()
    {
    }

    Face(const std::vector<size_t>& vertices)
    {
        vertices_ = vertices;
    }
    
    void addVertex(size_t vertex) {
        vertices_.push_back(vertex);
    }
    
    std::vector<size_t> getVertices() const {
        return vertices_;
    }
    
    size_t getVertex(size_t i) const {
        return vertices_.at(i);
    }
    
    
private:
    std::vector<size_t> vertices_;
    
};

class Space { // Многомерное пространство
public:
    Space(size_t n)
      : points_(n, 0)
    {
        // Строим базисные векторы
        size_t N = getNumDimensions();
        
        for (size_t i = 0; i < N; i++) {
            VectorXd b(N);
            for (size_t j = 0; j < N; j++) {
                b(j) = 0.0;
            }
            b(i) = 1.0;
            basis_.push_back(b);
        }
    }
    
    void buildFaceOnBasis(VectorXd offset, std::vector<size_t> base, size_t b1, size_t b2, float k1 = 1.0, float k2 = 1.0) {
        size_t N = getNumDimensions();
        
        VectorXd v0 = offset;
        for (size_t i = 0; i < base.size(); i++) v0 += basis_[base[i]];
        
        VectorXd v1 = v0 + basis_[b1] * k1;
        VectorXd v2 = v0 + basis_[b1] * k1 + basis_[b2] * k2;
        VectorXd v3 = v0 + basis_[b2] * k2;
        
        size_t f = getNumPoints();
        addPoint(v0);
        addPoint(v1);
        addPoint(v2);
        addPoint(v3);
        
        addEdge(f,f+1);
        addEdge(f+1,f+2);
        addEdge(f+2,f+3);
        addEdge(f+3,f);
        
        Face face;
        face.addVertex(f);
        face.addVertex(f+1);
        face.addVertex(f+2);
        face.addVertex(f+3);
        
        addFace(face);
    }
    
    void makeGrid(size_t numTiles, VectorXd offset) {
        size_t N = getNumDimensions();
        VectorXd v = offset;
        double step = 1.0;
        
        addPoint(v);
        
        // Добавляем точки
        for (size_t i = 0; i < N; i++) { // По каждому измерению
            size_t np = getNumPoints();
            for (size_t j = 0; j < np; j++) { // Для всех точек без учета новых
                for (size_t k = 1; k <= numTiles; k++) {
                    // дублируем их координаты, и транслируем по одной оси K раз
                    v = getPoint(j);
                    v(i) += k * step;
                    addPoint(v);
                }
            }
        }
        
        size_t np = getNumPoints();
        
        // Добавляем рёбра
        for (size_t i = 0; i < np; i++) {
            for (size_t j = i+1; j < np; j++) {
                if (pointDistance(i,j) == step) {
                    addEdge(i,j);
                }
            }
        }
    }
    
    void make5DGrid(size_t numTiles) {
        size_t N = getNumDimensions();
        //make5DHypercube();
        for (size_t i = 0; i < pow(2,N); i++) {
            VectorXd base(N);
            for (size_t k = 0; k < N; k++) base(k) = (i >> k) & 1;
            std::cout << base.transpose() << std::endl;

            make5DHypercube(base);
        }
    }
    
    void make5DHypercube(VectorXd baseOffset) {
        size_t N = getNumDimensions();
        for (size_t i = 0; i < N; i++) {
                std::vector<size_t> base0;

                std::vector<size_t> base1;
                for (size_t k = 0; k < N; k++) base1.push_back(k);

                buildFaceOnBasis(baseOffset, base0, i, (i+1) % N);
                base0.push_back(i);
                buildFaceOnBasis(baseOffset, base0, (i+1) % N, (i+4) % N);
                
                buildFaceOnBasis(baseOffset, base1, i, (i+1) % N, -1, -1);
                base1.clear();
                for (size_t k = 0; k < N; k++) if (k != i) base1.push_back(k);

                buildFaceOnBasis(baseOffset, base1, (i+1) % N, (i+4) % N, -1, -1);
        }
    }
    
    void makeQuasiSurface() {
        size_t N = getNumDimensions();
            VectorXd offset(N);
            for (size_t k = 0; k < N; k++) offset(k) = 0.0;
        
        for (size_t i = 0; i < N; i++) {
            std::vector<size_t> base;
            // Внутренняя грань
            buildFaceOnBasis(offset, base, i, (i+1) % N);

            // Внешняя грань
            base.push_back(i);
            buildFaceOnBasis(offset, base, (i+1) % N, (i+4) % N);
            
            base.clear();
            base.push_back(i);
            base.push_back((i+1) % N);
            base.push_back((i+4) % N);
            buildFaceOnBasis(offset, base, (i+2) % N, (i+3) % N, -1, -1);
            buildFaceOnBasis(offset, base, (i+2) % N, (i+1) % N, -1, -1);
            buildFaceOnBasis(offset, base, (i+3) % N, (i+4) % N, -1, -1);
        }
    }
    
    void addPoint(VectorXd v) {
        points_.conservativeResize(points_.rows(), points_.cols() + 1); // Добавляем новый столбец
        points_.col(points_.cols() - 1) = v; // Помещаем вектор в последний столбец
    }
    
    const MatrixXd getPointsMatrix() const {
        return points_;
    }
    
    VectorXd getPoint(size_t i) const {
        return points_.col(i);
    }
    
    double pointDistance(size_t i, size_t j) const {
        VectorXd v1 = points_.col(i);
        VectorXd v2 = points_.col(j);
        return (v1 - v2).norm();
    }
    
    void addEdge(size_t v1, size_t v2) {
        edges_.push_back(Edge(v1,v2));
    }
    
    size_t getNumDimensions() const {
        return points_.rows();
    }
    
    size_t getNumPoints() const {
        return points_.cols();
    }
    
    size_t getNumEdges() const {
        return edges_.size();
    }
    
    const Edge& getEdge(size_t i) const {
        return edges_[i];
    }
    
    void addFace(Face& face) {
        faces_.push_back(face);
    }
    
    size_t getNumFaces() const {
        return faces_.size();
    }
    
    const Face& getFace(size_t i) const {
        return faces_[i];
    }
    
    void printPoints() const {
        std::cout << points_ << std::endl;
    }
    
private:
    MatrixXd points_;             // Набор стоблцов, задающих точки в многомерности
    std::vector<Edge> edges_;     // Набор рёбер в многомерности
    std::vector<Face> faces_;
    std::vector<VectorXd> basis_;
};

class ProjectionSubspace {
public:
    // Инициализация ортонормированного баззиса по осям, заданным в виде индексов (начиная с 0)
    ProjectionSubspace(const Space* space, VectorXi indices)
      : basis_(MatrixXd::Zero(indices.rows(), space->getNumDimensions()))
      , space_(space)
    {
        for (size_t i = 0; i < indices.rows(); i++)
            basis_(i, indices(i)) = 1;
    }
    
    void applyRotation(BasisRotation r) {
        rotateBasis(r.axis1, r.axis2, r.angle);
        projectSpace();
    }
    
    void applyRotationSet(std::vector<BasisRotation> r) {
        size_t n = r.size();
        for (size_t i = 0; i < n; i++)
            rotateBasis(r[i].axis1, r[i].axis2, r[i].angle);
            
        projectSpace();
    }
    
    void projectSpace() {
        points_ = basis_ * space_->getPointsMatrix();
        calculateProjectionDistances();
    }
	
	MatrixXd getPointsInSpaceBasis() {
		return getBasisMatrix().transpose() * points_;
	}
    
    void calculateProjectionDistances() {
        const MatrixXd sourcePointsND = space_->getPointsMatrix();
        size_t numPoints = getNumPoints();
        distances_ = VectorXd(numPoints);
        
        MatrixXd projectedPointsND = getPointsInSpaceBasis();
        
        for (size_t i = 0; i < numPoints; i++) {
            distances_(i) = (projectedPointsND.col(i) - sourcePointsND.col(i)).norm();
        }
    }
    
    void makeQuasiBasis2D() {
        size_t dim = space_->getNumDimensions();
        for (size_t i = 0; i < dim; i++) {
            basis_(0,i) = cos(2*PI*i/dim);
            basis_(1,i) = sin(2*PI*i/dim);
        }
    }
    
    void makeQuasiBasis2D_5D_v2() {
        size_t dim = space_->getNumDimensions();
        if (dim != 5) return;
        
        double t = (1 + sqrt(5)) / 2;
          
        basis_(0,0) = 2;
        basis_(0,1) = t-1;
        basis_(0,2) = -t;
        basis_(0,3) = -t;
        basis_(0,4) = t-1;
        
        basis_(1,0) = 0;
        basis_(1,1) = t;
        basis_(1,2) = 1;
        basis_(1,3) = -1;
        basis_(1,4) = -t;
    }
    
   void makeQuasiBasis3D() {
    size_t dim = space_->getNumDimensions();
        for (size_t i = 0; i < dim; i++) {
            basis_(0,i) = cos(2*PI*i/dim);
            basis_(1,i) = sin(2*PI*i/dim);
            basis_(2,i) = 1 / sqrt(2);
        }
    }
    
   void makeIcosahedronBasis6D() {
    size_t dim = space_->getNumDimensions();
        for (size_t i = 0; i < 5; i++) {
            basis_(0,i) = cos(2*PI*i/5);
            basis_(1,i) = sin(2*PI*i/5);
            basis_(2,i) = 1 / sqrt(2);
        }
        basis_(0,5) = 0;
        basis_(1,5) = 0;
        basis_(2,5) = 1;
    }
    
    void orientFirstByVector(VectorXd v) {
        for (size_t i = 0; i < basis_.rows(); i++) {
            VectorXd A = basis_.row(i);
            VectorXd Bj = v;
            VectorXd sum = (A.dot(Bj) / Bj.dot(Bj)) * Bj;
               
            for (size_t j = 0; j < i; j++) {
                Bj = basis_.row(j);
                sum += (A.dot(Bj) / Bj.dot(Bj)) * Bj;
            }
                
            basis_.row(i) -= sum;
            basis_.row(i).normalize();
        }
    }
    
    size_t getNumEdges() const {
        return space_->getNumEdges();
    }
    
    const Edge& getEdge(size_t i) const {
        return space_->getEdge(i);
    }
    
    const MatrixXd getBasisMatrix() const {
        return basis_;
    }
    
    const MatrixXd getPointsMatrix() const {
        return points_;
    }
    
    const VectorXd getDistanceVector() const {
        return distances_;
    }
    
    size_t getNumBasisDimensions() const {
        return basis_.rows();
    }
    
    size_t getNumSpaceDimensions() const {
        return basis_.cols();
    }
    
    size_t getNumPoints() const {
        return points_.cols();
    }
    
    size_t getNumFaces() const {
        return space_->getNumFaces();
    }
    
    const Face& getFace(size_t i) const {
        return space_->getFace(i);
    }
    
    void printBasis() const {
        std::cout << basis_ << std::endl;
    }
    
    void printPoints() const {
        std::cout << points_ << std::endl;
    }
    
    void printDistances() const {
        std::cout << distances_.transpose() << std::endl;
    }
    
private:
    void rotateBasis(size_t axis1, size_t axis2, double angle) {
        if (axis1 == axis2) return;
        size_t n = getNumBasisDimensions();
        // Для каждого базисного вектора
        for (size_t i = 0; i < n; i++) {
            // Выбираем 2 координаты по индексам
            double x1 = basis_(i, axis1);
            double x2 = basis_(i, axis2);
            double sina = sin(angle);
            double cosa = cos(angle);
            basis_(i, axis1) = cosa*x1 - sina*x2;
            basis_(i, axis2) = sina*x1 + cosa*x2;
        }
    }

    MatrixXd basis_;     // Набор базисных векторов
    MatrixXd points_;    // Набор проекций точек
    VectorXd distances_;
    const Space* space_;
};

/*
projectedPoints (n x Q) = basisVectors (n x N) * sourcePoints (N x Q);

//add v to each column of m
mat.colwise() += v;
*/
