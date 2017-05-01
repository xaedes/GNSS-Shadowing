

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>


#include "pythonBindings/arrayBindings.h"
#include "pythonBindings/cvMatBindings.h"

#include "pythonBindings/coordinateSystemBindings.h"
#include "pythonBindings/satBindings.h"
#include "pythonBindings/worldBindings.h"
#include "pythonBindings/mappingBindings.h"
#include "pythonBindings/shadowingBindings.h"
#include "pythonBindings/objBindings.h"
#include "pythonBindings/planningBindings.h"
#include "common/math.h"

using namespace boost::python;

namespace gnssShadowing {
namespace pythonBindings {


    #if (PY_VERSION_HEX >= 0x03000000)
    static void *init_ar() {
    #else
    static void init_ar() {
    #endif
        Py_Initialize();

        import_array();
        return NUMPY_IMPORT_ARRAY_RETVAL;
    }

    double deg2rad(double deg)
    {
        return deg * D2R;
    }
    double rad2deg(double rad)
    {
        return rad * R2D;
    }
    BOOST_PYTHON_MODULE(gnssShadowing)
    {
        init_ar();

        to_python_converter<cv::Mat,matToNDArrayBoostConverter>();
        matFromNDArrayBoostConverter();
        
        def("deg2rad",&deg2rad);
        def("rad2deg",&rad2deg);

        bindObj();
        bindShadowing();
        bindCoordinateSystems();
        bindSat();
        bindWorld();
        bindMapping();
        bindPlanning();

    }
    
} // namespace pythonBindings
} // namespace gnssShadowing
