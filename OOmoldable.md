# Moldable should be abstract! #

Both performance and stability would increase by turning the Moldable class into an abstract class, and defining each widget type as a subclass of this. Not having done this already is a travesty!

## Steps to Completion #

+ Identify which member functions must be factored into the base class
+ Identify which member functions must be made virtual 
+ Identify subclasses of the Moldable base class
+ MoldableFactory should be simplified and simply create the correct Moldable object based on the widget type lookup
+ Specific lookups based on the widget should be handled in the object's constructor, not the factory.
