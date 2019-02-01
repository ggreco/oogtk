CXXFLAGS = `pkg-config --cflags gtk+-3.0` -g -DOOGTK_DEBUG
LDFLAGS = `pkg-config --libs gtk+-3.0 gmodule-2.0`

MODULES = testinline testbuilder testtree testdialog testobjects \
		  testcbks testtext testbutton testuimanager testwrapper \
		  testnoapp testsocket

all: $(MODULES)

clean:
	rm -f $(MODULES) ooedit make.errs

test%: test%.cpp *.h
	g++ -o $@ $(CXXFLAGS) $@.cpp $(LDFLAGS)

ooedit: ooedit.cpp *h
	g++ -o $@ $(CXXFLAGS) ooedit.cpp $(LDFLAGS)

