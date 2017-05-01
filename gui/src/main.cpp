#include <iostream>

#include "gui/version.h"
#include "app.h"

using namespace gnssShadowing::gui;

int main(int argc, char* argv[])
{	
    std::cout << "version " << Version::getString() << std::endl;
    std::cout << "revision " << Version::getRevision() << std::endl;

    App app;
    return app.run(argc,argv);
}
