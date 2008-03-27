CXXFLAGS = `pkg-config --cflags gtk+-2.0` -g -DOOGTK_DEBUG

MODULES = testinline testbuilder testtree testdialog testobjects testcbks testtext testbutton
all: $(MODULES)

clean:
	rm $(MODULES)

test%: test%.cpp *.h
	g++ -o $@ $(CXXFLAGS) $@.cpp `pkg-config --libs gtk+-2.0 gmodule-2.0`

ooedit: ooedit.cpp *h
	g++ -o $@ $(CXXFLAGS) ooedit.cpp `pkg-config --libs gtk+-2.0 gmodule-2.0`

