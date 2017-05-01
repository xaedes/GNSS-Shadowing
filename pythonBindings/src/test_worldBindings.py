
import gnssShadowing
import pytest
import numpy as np

def test_World():
    world=gnssShadowing.World("data/current.tle","data/uni.obj","Building")
    world=gnssShadowing.World("data/current.tle",2.0,"data/uni.obj","Building")
    assert world.m_startTimeUnixTimeSeconds == 2.0

    assert type(world.m_satStore) == gnssShadowing.SatStore
    assert type(world.m_origin) == gnssShadowing.Geodetic
    assert type(world.m_originECEF) == gnssShadowing.ECEF
    assert type(world.m_originENU) == gnssShadowing.ENU
    assert type(world.m_buildings) == gnssShadowing.ObjObject
    assert type(world.m_shadowing) == gnssShadowing.Shadowing
    