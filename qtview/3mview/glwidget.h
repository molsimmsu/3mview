#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include <QtGui/QMouseEvent>

class GLWidget : public QGLWidget
{
    Q_OBJECT

    QPoint lastPos;
    int xRot;
    int yRot;
    int zRot;
public:
    explicit GLWidget(QWidget *parent = 0);
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *);
signals:
    
public slots:
    
};

#endif // GLWIDGET_H
