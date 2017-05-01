
import gnssShadowing
import pytest
import numpy as np

def _test_geodeticDefaultConstructor():
    try:
        coord = gnssShadowing.Geodetic()
    except:
        pytest.fail("Could not 'coord = gnssShadowing.Geodetic()'")

def test_geodeticDataMembers():
    coord = gnssShadowing.Geodetic()
    coord.latitude = 1
    coord.longitude = 2
    coord.altitude = 3
    coord.julianDate = 4
    assert coord.latitude == 1
    assert coord.longitude == 2
    assert coord.altitude == 3
    assert coord.julianDate == 4

def test_geodeticCopyConstructor():
    coord1 = gnssShadowing.Geodetic()
    coord1.latitude = 1
    coord1.longitude = 2
    coord1.altitude = 3
    coord1.julianDate = 4
    coord2 = gnssShadowing.Geodetic(coord1)
    assert coord2.latitude == 1
    assert coord2.longitude == 2
    assert coord2.altitude == 3
    assert coord2.julianDate == 4

def test_geodeticValueConstructor():
    coord = gnssShadowing.Geodetic(1,2,3,4)
    assert coord.latitude == 1
    assert coord.longitude == 2
    assert coord.altitude == 3
    assert coord.julianDate == 4

def test_geodeticToECEF():
    coord = gnssShadowing.Geodetic(1,2,3,4)
    ecef = coord.toECEF()
    assert type(ecef) == gnssShadowing.ECEF

def test_horizontalDataMembers():
    coord = gnssShadowing.Horizontal()
    coord.azimuth = 1
    coord.elevation = 2
    coord.range = 3
    coord.julianDate = 4
    assert coord.azimuth == 1
    assert coord.elevation == 2
    assert coord.range == 3
    assert coord.julianDate == 4

def test_horizontalCopyConstructor():
    coord1 = gnssShadowing.Horizontal()
    coord1.azimuth = 1
    coord1.elevation = 2
    coord1.range = 3
    coord1.julianDate = 4
    coord2 = gnssShadowing.Horizontal(coord1)
    assert coord2.azimuth == 1
    assert coord2.elevation == 2
    assert coord2.range == 3
    assert coord2.julianDate == 4

def test_horizontalToENU():
    coord = gnssShadowing.Horizontal()
    enu = coord.toENU()
    assert type(enu) == gnssShadowing.ENU

def test_horizontalToSEZ():
    coord = gnssShadowing.Horizontal()
    sez = coord.toSEZ()
    assert type(sez) == gnssShadowing.SEZ

def test_enuToECEF():
    coord = gnssShadowing.ENU()
    ecef = coord.toECEF()
    assert type(ecef) == gnssShadowing.ECEF

def test_enuToSEZ():
    coord = gnssShadowing.ENU()
    sez = coord.toSEZ()
    assert type(sez) == gnssShadowing.SEZ

def test_enuToHorizontal():
    coord = gnssShadowing.ENU()
    horizontal = coord.toHorizontal()
    assert type(horizontal) == gnssShadowing.Horizontal

def test_enuECEFTransformation():
    geodetic = gnssShadowing.Geodetic()
    mat1 = gnssShadowing.ENU.ENU_ECEF(geodetic)
    assert type(mat1) == np.ndarray
    mat2 = gnssShadowing.ENU.ECEF_ENU(geodetic)
    assert type(mat2) == np.ndarray

def test_sezToECEF():
    coord = gnssShadowing.SEZ()
    ecef = coord.toECEF()
    assert type(ecef) == gnssShadowing.ECEF

def test_sezToENU():
    coord = gnssShadowing.SEZ()
    enu = coord.toENU()
    assert type(enu) == gnssShadowing.ENU

def test_sezToHorizontal():
    coord = gnssShadowing.SEZ()
    horizontal = coord.toHorizontal()
    assert type(horizontal) == gnssShadowing.Horizontal

def test_sezECEFTransformation():
    geodetic = gnssShadowing.Geodetic()
    mat1 = gnssShadowing.SEZ.SEZ_ECEF(geodetic)
    assert type(mat1) == np.ndarray
    mat2 = gnssShadowing.SEZ.ECEF_SEZ(geodetic)
    assert type(mat2) == np.ndarray

def test_ecefToENU():
    geodetic = gnssShadowing.Geodetic()
    coord = gnssShadowing.ECEF()
    enu = coord.toENU(geodetic)
    assert type(enu) == gnssShadowing.ENU

def test_ecefToSEZ():
    geodetic = gnssShadowing.Geodetic()
    coord = gnssShadowing.ECEF()
    sez = coord.toSEZ(geodetic)
    assert type(sez) == gnssShadowing.SEZ

def test_ecefToGeodetic():
    coord = gnssShadowing.ECEF()
    geodetic = coord.toGeodetic()
    assert type(geodetic) == gnssShadowing.Geodetic


def test_carthesianSystems():
    for CarthesianSystem in [gnssShadowing.CarthesianCoordinate, gnssShadowing.ECEF, gnssShadowing.TEME, gnssShadowing.ENU, gnssShadowing.SEZ]:
        _test_carthesianCoordinateDefaultConstructor(CarthesianSystem)
        _test_carthesianCoordinateDataMembers(CarthesianSystem)
        _test_carthesianCoordinateCopyConstructor(CarthesianSystem)
        _test_carthesianCoordinateAddSub(CarthesianSystem)

def _test_carthesianCoordinateDefaultConstructor(CarthesianSystem):
    try:
        coord = CarthesianSystem()
    except:
        pytest.fail("Could not 'coord = CarthesianSystem()'")

def _test_carthesianCoordinateDataMembers(CarthesianSystem):
    coord = CarthesianSystem()
    coord.position[0] = 1
    coord.position[1] = 2
    coord.position[2] = 3
    coord.velocity[0] = 4
    coord.velocity[1] = 5
    coord.velocity[2] = 6
    coord.julianDate = 7
    assert coord.position[0] == 1
    assert coord.position[1] == 2
    assert coord.position[2] == 3
    assert coord.velocity[0] == 4
    assert coord.velocity[1] == 5
    assert coord.velocity[2] == 6
    assert coord.julianDate == 7

def _test_carthesianCoordinateCopyConstructor(CarthesianSystem):
    coord1 = CarthesianSystem()
    coord1.position[0] = 1
    coord1.position[1] = 2
    coord1.position[2] = 3
    coord1.velocity[0] = 4
    coord1.velocity[1] = 5
    coord1.velocity[2] = 6
    coord1.julianDate = 7
    coord2 = CarthesianSystem(coord1)
    assert coord2.position[0] == 1
    assert coord2.position[1] == 2
    assert coord2.position[2] == 3
    assert coord2.velocity[0] == 4
    assert coord2.velocity[1] == 5
    assert coord2.velocity[2] == 6
    assert coord2.julianDate == 7

def _test_carthesianCoordinateAddSub(CarthesianSystem):
    coord1 = CarthesianSystem()
    coord1.position[0] = 1
    coord1.position[1] = 2
    coord1.position[2] = 3
    coord1.velocity[0] = 4
    coord1.velocity[1] = 5
    coord1.velocity[2] = 6
    coord2 = CarthesianSystem()
    coord2.position[0] = 7
    coord2.position[1] = 8
    coord2.position[2] = 9
    coord2.velocity[0] = 10
    coord2.velocity[1] = 11
    coord2.velocity[2] = 12
    coord3 = CarthesianSystem(coord2)

    coord4 = coord2 - coord1
    coord2 -= coord1
    assert coord2.position[0] == 7 - 1
    assert coord2.position[1] == 8 - 2
    assert coord2.position[2] == 9 - 3
    assert coord2.velocity[0] == 10 - 4
    assert coord2.velocity[1] == 11 - 5
    assert coord2.velocity[2] == 12 - 6
    assert coord4.position[0] == 7 - 1
    assert coord4.position[1] == 8 - 2
    assert coord4.position[2] == 9 - 3
    assert coord4.velocity[0] == 10 - 4
    assert coord4.velocity[1] == 11 - 5
    assert coord4.velocity[2] == 12 - 6
    coord5 = coord3 + coord1
    coord3 += coord1
    assert coord3.position[0] == 7 + 1
    assert coord3.position[1] == 8 + 2
    assert coord3.position[2] == 9 + 3
    assert coord3.velocity[0] == 10 + 4
    assert coord3.velocity[1] == 11 + 5
    assert coord3.velocity[2] == 12 + 6
    assert coord5.position[0] == 7 + 1
    assert coord5.position[1] == 8 + 2
    assert coord5.position[2] == 9 + 3
    assert coord5.velocity[0] == 10 + 4
    assert coord5.velocity[1] == 11 + 5
    assert coord5.velocity[2] == 12 + 6

# def test_