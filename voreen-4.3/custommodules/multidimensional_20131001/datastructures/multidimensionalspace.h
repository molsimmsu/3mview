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

class Space { // Многомерное пространство
public:
    Space(size_t n)
      : points_(n, 0)
    {}
    
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
    
    void printPoints() const {
        std::cout << points_ << std::endl;
    }
    
private:
    MatrixXd points_;             // Набор стоблцов, задающих точки в многомерности
    std::vector<Edge> edges_;     // Набор рёбер в многомерности
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

    void makeQuasiBasis2D() {
        size_t N = space_->getNumDimensions();
        for (size_t i = 0; i < basis_.cols(); i++) {
            basis_(0,i) = cos(2*PI*i/N);
            basis_(1,i) = sin(2*PI*i/N);
        }
    }
    
    void buildQuasiEdgesByDistance(double dMin, double dMax, double D, int classIndex = -1) {
        size_t qi = quasiEdges_.size();
        if (classIndex < 0 || qi == 0)
            quasiEdges_.push_back(std::vector<Edge>());
        else if (classIndex < qi) {
            qi = classIndex;
            quasiEdges_[qi].clear();
        }
        
        size_t numPoints = getNumPoints();
        for (size_t i = 0; i < numPoints; i++)
        for (size_t j = i+1; j < numPoints; j++) {
            if (distances_[i] > D || distances_[j] > D) continue;
            double d = (points_.col(i) - points_.col(j)).norm();
            if (d > dMin && d < dMax)
                quasiEdges_[qi].push_back(Edge(i,j));
        }
    }
    
    size_t getNumQuasiEdgeClasses() const {
        return quasiEdges_.size();
    }
    
    size_t getNumQuasiEdges(size_t classIndex) const {
        return quasiEdges_[classIndex].size();
    }
    
    const Edge& getQuasiEdge(size_t classIndex, size_t i) const {
        return quasiEdges_[classIndex][i];
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
    
    std::vector< std::vector<Edge> > quasiEdges_;
};

/*
projectedPoints (n x Q) = basisVectors (n x N) * sourcePoints (N x Q);

//add v to each column of m
mat.colwise() += v;
*/
