CXXFLAGS = -std=c++11 `pkg-config --cflags gtk+-3.0` -g -DOOGTK_DEBUG
#-Wno-deprecated-declarations
LDFLAGS = `pkg-config --libs gtk+-3.0 gmodule-2.0`

MODULES = testinline testbuilder testtree testdialog testobjects \
		  testcbks testtext testbutton testuimanager testwrapper \
		  testnoapp testsocket testimage

all: $(MODULES)

clean:
	cd tests
	rm -f $(MODULES) *.dSYM ooedit make.errs

test%: test%.cpp *.h
	g++ -o tests/$@ $(CXXFLAGS) $@.cpp $(LDFLAGS)

ooedit: ooedit.cpp *h
	g++ -o tests/$@ $(CXXFLAGS) ooedit.cpp $(LDFLAGS)

