# Сборка 3mview под mac

brew install boost libtiff fftw # может еще что, не помню
#
wget http://bitbucket.org/eigen/eigen/get/2.0.17.tar.bz2
# для этого нужно make install делать, по make он не собирается
#
# sudo pip install sip # у меня не заработало так
wget http://sourceforge.net/projects/pyqt/files/sip/sip-4.14.2/sip-4.14.2.tar.gz
wget http://sourceforge.net/projects/pyqt/files/PyQt4/PyQt-4.9.6/PyQt-mac-gpl-4.9.6.tar.gz
# собрать скачанное python configure.py && make -jN && sudo make install 
#
# далее собрать ost
# перед этим подпатчить sqlite: http://translate.google.ca/translate?hl=en&sl=ja&u=http://tribelet.blogspot.com/2008/09/blog-post_08.html&ei=rQ4VTrSqL4PUiAKTsp2JDw&sa=X&oi=translate&ct=result&resnum=1&ved=0CBwQ7gEwAA&prev=/search%3Fq%3Dsqlite3%2Berror:%2B(near%2Binitialization%2Bfor%2B%2527aDateTimeFuncs%255B0%255D.pUserData%2527)%26hl%3Den%26client%3Dsafari%26rls%3Den%26prmd%3Divns
# и tiff модуль: https://www.maillist.unibas.ch/pipermail/iplt-devel/2012-August/000143.html
#
# Could you try changing the definitions and declarations of
# the CustomTIFF* functions in
# modules/io/src/img/tiff_util.hh and
# modules/io/src/img/tiff_util.cc to take a tmsize_t as
# the last argument?
#
EIGEN2_INCLUDE_DIR=/usr/local/Cellar/eigen/3.1.1/include TIFF_INCLUDE_DIR=/usr/local/Cellar/libtiff/4.0.3/include cmake .
# может не заработать с eigen3?
# openstructure собирается хорошо

# qmake 3mview.pro
# xcodebuild -project 3mview.xcodeproj -alltargets
# либо (так работает)
qmake -spec macx-g++
make