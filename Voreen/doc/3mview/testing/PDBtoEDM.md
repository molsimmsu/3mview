Методика испытаний модуля создания тестовых карт электронной плотности разного разрешения по атомистическим структурам биомакромолекул
================================================================

Схема испытаний
---------------
1. Запускаем программу VoreenVE.
2. Загружаем рабочую область PDBtoEDM.vws
3. Загружаем структурный файл биомакромолекулы.
4. Запускаем модуль построения карты электронной плотности и визуально проверяем его работу при различных параметрах.
5. Повторяем пункты 3-4 для нескольких структурных файлов.
6. Завершаем программу VoreenVE.

Методика испытаний
------------------

#### 1. Запуск программы VoreenVE

* Способ 1. Нажатием на иконку на рабочем столе или в панели быстрого запуска.
* Способ 2. Через терминал. Переходим в корневой каталог программы 3mview. И выполняем команду

    cd Voreen && ./voreenve.sh

Если появилось окно с заголовком VoreenVE, переходим к следующему пункту. В противном случае обращаемся за помощью к системному администратору.

#### 2. Загрузка рабочей области

1. В верхнем меню выбираем File -> Open Workspace.
2. В открывшемся окне переходим в каталог Test, находящийся в каталоге Voreen/resource/workspaces.
3. Выбираем файл PDBtoEDM.vws и жмем кнопку Open.

#### 3. Загрузка структурного файла

1. В меню справа выбираем пункт MoleculeSource.
2. Нажимаем кнопку Load Molecule (на данном этапе структура молекулы не отобразится). 

#### 4. Запуск модуля построения карты электронной плотности и визуальная проверка его работы при различных параметрах

1. В меню справа выбираем пункт PDBtoEDM.
2. Ползунками устанавливаем необходимые параметры создаваемой карты, где:
* "Length of calc (A)" - расстояние (в ангстремах), начиная с которого электронная плотность не считается.
* "Step" - шаг разбиения, задающий точность вычислений (в 0.1 А)
Отметим, что для больших систем расчет электронной плотности может занять длительное время. При установке соответствующих параметров необходимо следить за состоянием оперативной и видео памяти.
3. Нажимаем кнопку "Generate EDM grid".
4. Если все сделано правильно, на экране появиться изображение электронной плотности и тестирование для данной макромолекулы считается завершенным. Если этого не происходит, настраиваем переходную функцию (см. инструкцию про настройку переходных функций - TODO).

#### 5. Повтор пунктов 3-4 для нескольких структурных файлов

#### 6. Завершение программы VoreenVE
1. В верхнем меню выбираем File -> Quit или нажимаем на крестик в заголовке окна.
2. В появившемся диалоговом окне с надписью "Save the current workspace?" нажимаем кнопку No.
 
На этом тестирование завершено.