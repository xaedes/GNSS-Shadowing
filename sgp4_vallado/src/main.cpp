#include <iostream>

#include "sgp4_vallado/version.h"

using namespace gnssShadowing::sgp4_vallado;

int main(int argc, char* argv[])
{	
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;
}
