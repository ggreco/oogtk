CXXFLAGS = `pkg-config --cflags gtk+-2.0` -g -DOOGTK_DEBUG
LDFLAGS = `pkg-config --libs gtk+-2.0 gmodule-2.0`

MODULES = testinline testbuilder testtree testdialog testobjects \
		  testcbks testtext testbutton testuimanager

all: $(MODULES)

clean:
	rm $(MODULES)

test%: test%.cpp *.h
	g++ -o $@ $(CXXFLAGS) $@.cpp $(LDFLAGS)

ooedit: ooedit.cpp *h
	g++ -o $@ $(CXXFLAGS) ooedit.cpp $(LDFLAGS)

