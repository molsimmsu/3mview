### Содержание

1. Введение
2. Документация от разработчиков Voreen
  * [Concepts](http://voreen.uni-muenster.de/?q=concepts)
  * [Build instructions](http://voreen.uni-muenster.de/?q=build-instructions)
  * [Programming Tutorials](http://voreen.uni-muenster.de/?q=programming-tutorials)
3. Руководство по разработке (в процессе)
    * Базовые классы
        - Processor
        - Property
        - Observer
4. [Публикации, использующие Voreen](http://www.voreen.org/109-Publications.html)

Примечание. Есть два сайта Voreen: 
[официальный](http://voreen.uni-muenster.de) и [созданный одним из разработчиков](http://www.voreen.org).
Мы используем версию Voreen, загруженную с официального сайта.

Voreen - The Volume Rendering Engine
====================================
http://voreen.uni-muenster.de

About
-----
Voreen is an open source rapid application development framework for the interactive visualization
and analysis of multi-modal volumetric data sets. It provides GPU-based volume rendering and data 
analysis techniques and offers high flexibility when developing new analysis workflows in 
collaboration with domain experts. The Voreen framework consists of a multi-platform C++ library, 
which can be easily integrated into existing applications, and a Qt-based stand-alone application.
It is licensed under the terms of the GNU General Public License version 2. 
The Voreen project has been initiated and is maintained by the Visualization & Computer Graphics 
Research Group at the University of Muenster as part of the central project Z1 of the collaborative 
research center SFB 656 MoBil.

For build instructions and further information, please refer to the project website.

Directory Structure
-------------------
* include                - framework headers
* src                    - framework source files
* modules                - standard plugin modules
* custommodules          - location for custom (third-party) modules
* bin                    - location where compiled binaries are written to
* 
* apps                   - applications using voreen
* apps/simple            - simple Qt- and GLUT-based applications
* apps/voreenve          - main Voreen application - the Voreen Visualization Environment
* apps/voreentool        - command-line interface to the Voreen library, executing workspaces/networks
* 
* data                   - location for user-generated data (log files, settings, screenshots, ...)
* 
* resource               - contains read-only resources
* resource/fonts         - (possibly) necessary fonts
* resource/scripts       - Python scripts which can be executed from within Voreen
* resource/transferfuncs - pre-defined transfer functions
* resource/volumes       - sample volume data
* resource/workspaces    - sample Voreen workspaces
* 
* doc                    - documentation
* ext                    - external dependencies to other libraries
