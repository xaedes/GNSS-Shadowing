#include <iostream>
#include <string>

#include "obj/version.h"
#include "obj/objFile.h"

using namespace gnssShadowing::obj;

int main(int argc, char* argv[])
{	
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;

    if (argc < 2)
    {
        std::cout << "Usage: obj_run objfile" << std::endl;
        return -1;
    }

    std::string filename(argv[1]);
    std::cout << "Loading " << filename << std::endl;
    ObjFile obj(filename);
    std::cout << obj << std::endl;
}
