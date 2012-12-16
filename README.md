# КОМПИЛЯЦИЯ
1) Устанавливаем пакеты

    CMake (2.6.4)
    Qt Creator (версия Qt >= 4.5)

(далее все пакеты dev)

    Eigen2 (2.0.6)
    Boost (весь) (>= 1.37)
    libpng
    Python2 (>= 2.4)
    FFTW3 (libfftw3-dev)
    libtiff 
    SIP (python-sip-dev)
    PyQt4 (python-qt4-dev)

2) Собираем OpenStructure

    cmake . -DOPTIMIZE=1
    make -j N (N - число процессоров)

3) Собираем OpenBabel

    cmake .
    make -j N

4) Собираем проект 3mview.pro

    qmake 3mview.pro
    make -j N
