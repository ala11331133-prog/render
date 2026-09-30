CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra

test_odczyt: test_odczyt.cpp linie.cpp osm_odczyt.cpp linie.h osm_odczyt.h obiekt.h
	$(CXX) $(CXXFLAGS) -o $@ test_odczyt.cpp linie.cpp osm_odczyt.cpp

osm2bin: osm2bin.cpp linie.cpp osm_odczyt.cpp linie.h osm_odczyt.h obiekt.h
	$(CXX) $(CXXFLAGS) -o $@ osm2bin.cpp linie.cpp osm_odczyt.cpp

linie.bin: osm2bin map.osm
	./osm2bin map.osm $@

map_wzorzec.txt: map.osm wzorzec_osm.py
	python3 wzorzec_osm.py map.osm $@

test: test_odczyt map_wzorzec.txt
	./test_odczyt map.osm map_wzorzec.txt

clean:
	rm -f test_odczyt osm2bin map_wzorzec.txt

.PHONY: test clean
