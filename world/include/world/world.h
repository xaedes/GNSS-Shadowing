#pragma once
#include <string>
#include "obj/objFile.h"
#include "sat/satStore.h"
#include "sat/coordinateSystems.h"
#include "shadowing/shadowing.h"
#include "common/math.h"
#include "common/timing.h"

namespace gnssShadowing {
namespace world {

    class World
    {
    public:
        World(const std::string& satStoreFilename, double startTimeUnixTimeSeconds, const std::string& objFileFilename, const std::string& buildingsGroupName="Building")
            : m_satStore(satStoreFilename)
            , m_startTimeUnixTimeSeconds(startTimeUnixTimeSeconds)
            , m_objFile(objFileFilename)
            , m_buildings(m_objFile.m_groups[buildingsGroupName])
            , m_shadowing(m_buildings)
            , m_origin( m_objFile.m_projection.latitude*D2R,
                        m_objFile.m_projection.longitude*D2R,
                        m_objFile.m_projection.elevation,
                        sat::julianDateFromUnix(m_startTimeUnixTimeSeconds) )
            , m_originECEF(m_origin.toECEF())
            , m_originENU(m_originECEF.toENU(m_origin))
        {}
        World(const std::string& satStoreFilename, const std::string& objFileFilename, const std::string& buildingsGroupName="Building")
            : World(satStoreFilename, common::now_seconds(), objFileFilename, buildingsGroupName)
        {}

        sat::SatStore m_satStore;

        double m_startTimeUnixTimeSeconds;

        obj::ObjFile m_objFile;
        obj::Object m_buildings;
        shadowing::Shadowing m_shadowing;

        sat::Geodetic m_origin;
        sat::ECEF m_originECEF;
        sat::ENU m_originENU;
    };

} // namespace world
} // namespace gnssShadowing
