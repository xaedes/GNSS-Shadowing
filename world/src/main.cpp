#include <iostream>

#include "world/version.h"

using namespace gnssShadowing::world;

int main(int argc, char* argv[])
{	
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;
}
