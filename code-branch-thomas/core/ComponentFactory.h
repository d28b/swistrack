#ifndef HEADER_ComponentFactory
#define HEADER_ComponentFactory

template <class tComponent>
class ComponentFactory {

public:
	ComponentFactory();
	~ComponentFactory() {}

	tComponent* Create() {return new tComponent();}
};

#endif
