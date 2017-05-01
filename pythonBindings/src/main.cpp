#include <iostream>

#include "pythonBindings/version.h"

using namespace gnssShadowing::pythonBindings;

int main(int argc, char* argv[])
{	
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;
}
