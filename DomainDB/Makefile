FILTER = --asym3Max 0.4 --dmin 10

PKG_CONFIG_ENV = PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
RPATHCMD = -Wl,-rpath -Wl,/usr/local/lib

all: bde bdf mom_db_create

# BALBES Domain Extractor (BDE)
bde: src/bde.cpp src/Molecule.h src/VMD.h src/DomainsDB.h
	g++ src/$(@).cpp -o bin/$(@) -Wall -I"include"

# BLAST Domain Finder (BDF)
bdf: src/bdf.cpp
	g++ src/$(@).cpp -o bin/$(@) -Wall -I"include"
	
# Form Database
mom_db_create: src/mom_db_create/main.cpp src/mom_db_create/pdb.cpp
	g++ src/mom_db_create/main.cpp src/mom_db_create/pdb.cpp -o bin/mom_db_create

testdb: bde bdf
	cd bin && \
	./bde -m interval -f 0 -l 100 $(FILTER) && \
	./bdf -m

homodb: bde bdf
	cd bin && \
	./bde -m all $(FILTER) && \
	./bdf -m

formdb:
	cd bin && \
	sh convertdir.sh ./mom_db_create ./DomainsDB/domains

clean:
	rm -f bin/bde
	rm -f bin/bdf
	rm -f bin/mom_db_create
	rm -f bin/DomainsDB -r
	
rar:
	rar a -r -x*\.svn\* -x*\.svn DomainDB.rar ../DomainDB/*
