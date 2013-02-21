### Содержание
1. [Установка](#-1)
2. [Разработка модулей](#--1)
3. [Организационные вопросы](#--3)

Установка
----------

### Создание рабочей копии
    git clone https://github.com/molsimmsu/3mview.git

### Установка пакетов
Ubuntu:

    sudo apt-get install libqt4-dev cmake libeigen2-dev libboost-dev libpng12-dev libfftw3-dev libtiff5-dev python-sip-dev python-qt4-dev

Скачиваем [OpenBabel](http://sourceforge.net/projects/openbabel/files/openbabel/2.3.2/openbabel-2.3.2.tar.gz/download) и распаковываем его в 3mview/openbabel-2.3.2
    
Устанавливаем OpenBabel

    cmake .
    make -j4

Компилируем Voreen

    cmake . -DVRN_BUILD_3MVIEW=ON -DVRN_BUILD_VOREENVE=ON -DVRN_BUILD_VOREENTOOL=OFF -DVRN_MODULE_MOLECULE=ON -DVRN_MODULE_STEREO=ON -DVRN_MODULE_GEOMETRY=ON -DVRN_MODULE_ELECTRONDENSITY=ON -DVRN_MODULE_FORMCHARACTERISTICS=ON
    make -j4
    
### Если при компиляции выданы ошибки, связанные с Boost

Скачиваем [Boost 1.52.0](http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_1_52_0.tar.bz2/download)

Устанавливаем Boost в каталог 3mview/boost-1.52.0

    cd path/to/boost_1_52_0
    ./bootstrap.sh --prefix=path_to_somewhere/3mview/boost-1.52.0
    ./b2 install
    
Компилируем Voreen с флагом -DUSE_CUSTOM_BOOST=ON

    cmake . -DUSE_CUSTOM_BOOST=ON -DVRN_BUILD_3MVIEW=ON -DVRN_BUILD_VOREENVE=ON -DVRN_BUILD_VOREENTOOL=OFF -DVRN_MODULE_MOLECULE=ON -DVRN_MODULE_STEREO=ON -DVRN_MODULE_GEOMETRY=ON -DVRN_MODULE_ELECTRONDENSITY=ON -DVRN_MODULE_FORMCHARACTERISTICS=ON
    make -j4
    
### Запуск программы

VoreenVE:

    ./voreenve.sh
    
3mview:

    ./3mview.sh

Разработка модулей
------------------

### Создание модуля
1. Изучаем [Adding a Module Tutorial](http://voreen.uni-muenster.de/?q=module-tutorial)
2. Убираем из примера все то, что относится к GLSL (каталог `glsl` и функцию `addShaderPath()`)

### Создание структур данных
1. В каталоге модуля создаем подкаталог `datastructures`
2. Размещаем в нем файлы стуктур. [Примеры](Voreen/custommodules/molecule/datastructures)
3. Если требуется передача структуры данных между процессорами, создаем [порт](Voreen/custommodules/molecule/ports).

### Создание модулей чтения карт ЭП
1. В каталоге модуля создаем подкаталог `io`
2. Берем за образец [модуль чтения формата MRC](Voreen/custommodules/electrondensity/io)

### Создание properties

Общая последовательность действий:

1. Создаем свойство: наследуем от [TemplateProperty](Voreen/include/voreen/core/properties/templateproperty.h)
    * [Пример1.h](Voreen/include/voreen/core/properties/floatproperty.h)
    * [Пример1.cpp](Voreen/src/core/properties/floatproperty.cpp)
2. Создаем виджет: наследуем от [QPropertyWidget](Voreen/include/voreen/qt/widgets/property/qpropertywidget.h)
    * Примеры в каталогах [include](Voreen/include/voreen/qt/widgets/property/) и [src](Voreen/src/qt/widgets/property/)
3. Создаем фабрику виждетов: наследуем от [PropertyWidgetFactory](Voreen/include/voreen/core/properties/propertywidgetfactory.h)
    * [Пример1.h](Voreen/modules/opencl/qt/openclpropertywidgetfactory.h)
    * [Пример1.cpp](Voreen/modules/opencl/qt/openclpropertywidgetfactory.cpp)
    * [Пример2.h](Voreen/include/voreen/qt/widgets/property/corepropertywidgetfactory.h)
    * [Пример2.cpp](Voreen/src/qt/widgets/property/corepropertywidgetfactory.cpp)
4. Создаем модуль Qt: наследуем от [VoreenModuleQt](Voreen/include/voreen/qt/voreenmoduleqt.h).
    * [Пример1.h](Voreen/modules/opencl/openclmoduleqt.h)
    * [Пример1.cpp](Voreen/modules/opencl/openclmoduleqt.cpp)
5. Регистрируем в конструкторе модуля Qt фабрику виджетов.

Организационные вопросы
-----------------------
### Ссылки по GitHub
* [Оформление README.md](https://github.com/github/markup/blob/master/README.md)
