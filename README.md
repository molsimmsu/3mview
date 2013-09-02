### Содержание
1. [Установка](#-1)
2. [Разработка модулей](#--1)
3. [Организационные вопросы](#--3)
4. [Приложение 1: Методики тестирования](Voreen/doc/3mview/testing)
5. [Приложение 2: Документация к Voreen](Voreen/README.md)

Установка
----------

### Создание рабочей копии
    git clone https://github.com/molsimmsu/3mview.git

### Установка пакетов
Проверяем наличие компилятора gcc версии 4.6 и выше. (версия 4.4 не работает)

Ubuntu:

    sudo apt-get install libspnav-dev libqt4-dev cmake libeigen2-dev libboost-dev libpng12-dev libfftw3-dev libtiff5-dev python-sip-dev python-qt4-dev libdevil-dev libglew-dev

Скачиваем [OpenBabel](http://sourceforge.net/projects/openbabel/files/openbabel/2.3.2/openbabel-2.3.2.tar.gz/download) и распаковываем его в 3mview/openbabel-2.3.2
    
Устанавливаем OpenBabel

    cmake .
    make -j4

Компилируем Voreen

    cmake . -DVRN_BUILD_3MVIEW=ON -DVRN_BUILD_VOREENVE=ON -DVRN_BUILD_VOREENTOOL=OFF -DVRN_MODULE_MOLECULE=ON -DVRN_MODULE_STEREO=ON -DVRN_MODULE_GEOMETRY=ON -DVRN_MODULE_ELECTRONDENSITY=ON -DVRN_MODULE_FORMCHARACTERISTICS=ON -DVRN_MODULE_HOMOLOGY=ON -DVRN_MODULE_SYSTEMMODULES=ON

    make -j4
    
### Если при компиляции выданы ошибки, связанные с Boost

Скачиваем [Boost 1.52.0](http://sourceforge.net/projects/boost/files/boost/1.52.0/boost_1_52_0.tar.bz2/download)

Устанавливаем Boost в каталог 3mview/boost-1.52.0

    cd path/to/boost_1_52_0
    ./bootstrap.sh --prefix=path_to_somewhere/3mview/boost-1.52.0
    ./b2 install
    
Компилируем Voreen с флагом -DUSE_CUSTOM_BOOST=ON

    cmake . -DUSE_CUSTOM_BOOST=ON -DVRN_BUILD_3MVIEW=ON -DVRN_BUILD_VOREENVE=ON -DVRN_BUILD_VOREENTOOL=OFF -DVRN_MODULE_MOLECULE=ON -DVRN_MODULE_STEREO=ON -DVRN_MODULE_GEOMETRY=ON -DVRN_MODULE_ELECTRONDENSITY=ON -DVRN_MODULE_FORMCHARACTERISTICS=ON -DVRN_MODULE_HOMOLOGY=ON -DVRN_MODULE_SYSTEMMODULES=ON

    make -j4
    
### Запуск программы

VoreenVE:

    ./voreenve.sh
    
3mview:

    ./3mview.sh
    
### Установка hex
Скачиваем необходимый архив

    http://hex.loria.fr/dist/index.php
    
Распаковываем лишь один файл /bin/hex_setup.bin в директорию с архивом

Запускаем ./hex_setup.bin

Разработка модулей
------------------

### Создание модуля
1. Знакомимся в [принципами Voreen](http://voreen.uni-muenster.de/?q=concepts). Полезно также посмотреть [подробное описание](http://voreen.uni-muenster.de/?q=tutorial-slides).
2. Выполняем шаги, описанные в [Adding a Module Tutorial](http://voreen.uni-muenster.de/?q=module-tutorial)
3. Убираем из примера все то, что относится к GLSL (каталог `glsl` и функцию `addShaderPath()`)

### Создание процессоров
1. Берем за основу базовый процессор
2. Вносим в него следующие изменения:
    * Директивы препроцессора #ifndef и #define в заголовочном файле класса должны содержать уникальное имя процессора
    * Заменяем везде имя исходного класса с помощью автоматической замены (с учетом регистра)
    * Заполняем возвращаемые значения для функций getCategory() и setDescriptions()
    * Добавляем заголовочные файлы используемых свойств, портов, структур данных
    * Вводим порты и свойства как private-переменные. Соглашение: имена private-переменных оканчиваются на символ "_"
    * Инициализируем и регистрируем порты и свойства в конструкторе
3. Регистрируем процессор в файле модуля (см. инструкцию по созданию модуля)

### Создание структур данных
1. В каталоге модуля создаем подкаталог `datastructures`
2. Размещаем в нем файлы стуктур. [Примеры](Voreen/custommodules/molecule/datastructures)
3. Если требуется передача структуры данных между процессорами, создаем [порт](Voreen/custommodules/molecule/ports)
4. Добавляем в файл `<modulename>.cmake` пути к созданным файлам

### Создание модулей чтения карт ЭП
1. В каталоге модуля создаем подкаталог `io`
2. Берем за образец [модуль чтения формата MRC](Voreen/custommodules/electrondensity/io)
3. Регистрируем модуль чтения в конструкторе основного класса модуля с помощью функции `registerVolumeReader(new MyVolumeReader());`
    * Пример: [electrondensity.cpp](Voreen/custommodules/electrondensity/electrondensity.cpp)
4. Добавляем в файл `<modulename>.cmake` пути к созданным файлам

### Создание properties
1. В каталоге модуля создаем подкаталог `properties`
2. В каталоге `properties` создаем Property: наследуем от [TemplateProperty](Voreen/include/voreen/core/properties/templateproperty.h)
    * [stringproperty.h](Voreen/include/voreen/core/properties/stringproperty.h)
    * [stringproperty.cpp](Voreen/src/core/properties/stringproperty.cpp)
    * Функции `serialize()` и `deserialize()` можно исключить
    * Другие примеры: [include](Voreen/include/voreen/core/properties) и [src](Voreen/src/core/properties)
3. В каталоге модуля создаем подкаталог `qt`
4. В каталоге `qt` создаем QPropertyWidget: наследуем от [QPropertyWidget](Voreen/include/voreen/qt/widgets/property/qpropertywidget.h)
    * Примеры: [include](Voreen/include/voreen/qt/widgets/property/) и [src](Voreen/src/qt/widgets/property/)
5. В каталоге `qt` создаем WidgetFactory: наследуем от [PropertyWidgetFactory](Voreen/include/voreen/core/properties/propertywidgetfactory.h)
    * [openclpropertywidgetfactory.h](Voreen/modules/opencl/qt/openclpropertywidgetfactory.h)
    * [openclpropertywidgetfactory.cpp](Voreen/modules/opencl/qt/openclpropertywidgetfactory.cpp)
    * [corepropertywidgetfactory.h](Voreen/include/voreen/qt/widgets/property/corepropertywidgetfactory.h)
    * [corepropertywidgetfactory.cpp](Voreen/src/qt/widgets/property/corepropertywidgetfactory.cpp)
6. В каталоге модуля cоздаем ModuleQt: наследуем от [VoreenModuleQt](Voreen/include/voreen/qt/voreenmoduleqt.h).
    * [openclmoduleqt.h](Voreen/modules/opencl/openclmoduleqt.h)
    * [openclmoduleqt.cpp](Voreen/modules/opencl/openclmoduleqt.cpp)
7. Регистрируем WidgetFactory в конструкторе ModuleQt.
8. Добавляем в файл `<modulename>.cmake` пути к созданным файлам. 
    * Классы, использующие Qt, помещаем в специальные списки (как в [core.cmake](Voreen/modules/core/core.cmake))

Организационные вопросы
-----------------------
### Ссылки по GitHub
* [Оформление README.md](https://github.com/github/markup/blob/master/README.md)
