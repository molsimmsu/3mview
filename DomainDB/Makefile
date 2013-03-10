FILTER = --asym3Max 0.4 --dmin 10

PKG_CONFIG_ENV = PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
RPATHCMD = -Wl,-rpath -Wl,/usr/local/lib

all: bde bdf

# BALBES Domain Extractor (BDE)
bde: src/bde.cpp src/Molecule.h src/VMD.h src/DomainsDB.h
	g++ src/$(@).cpp -o bin/$(@) -Wall -I"include" \
	$(RPATHCMD) `$(PKG_CONFIG_ENV) pkg-config --cflags --libs plplotd`

# BLAST Domain Finder (BDF)
bdf: src/bdf.cpp
	g++ src/$(@).cpp -o bin/$(@) -Wall -I"include"

testdb: bde bdf
	cd bin && \
	./bde -m interval -f 0 -l 100 $(FILTER) && \
	./bdf -m

fulldb: bde bdf
	cd bin && \
	./bde -m all $(FILTER) && \
	./bdf -m

clean:
	rm -f bin/bde
	rm -f bin/bdf
	rm -f bin/DomainsDB -r
	
rar:
	rar a -r -x*\.svn\* -x*\.svn DomainDB.rar ../DomainDB/*
