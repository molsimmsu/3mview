# СОЗДАНИЕ РАБОЧЕЙ КОПИИ
    git clone
    git submodule init && git submodule update

# КОМПИЛЯЦИЯ
Ubuntu:

    sudo apt-get install libqt4-dev cmake libeigen2-dev libboost-dev libpng12-dev libfftw3-dev libtiff5-dev python-sip-dev python-qt4-dev

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
