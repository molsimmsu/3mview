# СОЗДАНИЕ РАБОЧЕЙ КОПИИ
    git clone https://github.com/molsimmsu/3mview.git

# КОМПИЛЯЦИЯ

Ubuntu:

    sudo apt-get install libqt4-dev cmake libeigen2-dev libboost-dev libpng12-dev libfftw3-dev libtiff5-dev python-sip-dev python-qt4-dev

Скачиваем Boost 1.52.0

    http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_1_52_0.tar.bz2/download
    
Устанавливаем Boost в каталог 3mview/boost-1.52.0

    $ cd path/to/boost_1_52_0
    $ ./bootstrap.sh --prefix=path_to_somewhere/3mview/boost-1.52.0
    $ ./b2 install
    
Скачиваем OpenBabel и распаковываем его в 3mview/openbabel-2.3.2

    http://sourceforge.net/projects/openbabel/files/openbabel/2.3.2/openbabel-2.3.2.tar.gz/download
    
Устанавливаем OpenBabel

    $ cmake .
    $ make -j4

Компилируем Voreen

    cmake . -DVRN_BUILD_3MVIEW=ON -DVRN_BUILD_VOREENVE=ON -DVRN_BUILD_VOREENTOOL=OFF -DVRN_MODULE_MOLECULE=ON -DVRN_MODULE_STEREO=ON
