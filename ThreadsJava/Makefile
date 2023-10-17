JAVAC	= javac
JAVA	= java

default:: TreeCalculation.class

clean::
	-rm -f *.class speedup.dat speedup.png

# kleines Testbeispiel
run:: TreeCalculation.class
	$(JAVA) TreeCalculation 4 1 2

TreeCalculation.class: TreeCalculation.java
	$(JAVAC) $<

