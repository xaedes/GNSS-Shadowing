
import gnssShadowing
import pytest
import numpy as np

def test_satStore_getNumSats():
    sats = gnssShadowing.SatStore("data/current.tle")
    assert sats.getNumSats() > 0

def test_satStore_getSat():
    sats = gnssShadowing.SatStore("data/current.tle")
    try:
        sat = sats.getSat(0)
    except:
        pytest.fail("Could not 'sats.getSat(0)'")

def test_sat():
    sat = gnssShadowing.Sat("GPS BIIR-2  (PRN 13)",
            "1 24876U 97035A   17033.94912398  .00000083  00000-0  00000+0 0  9990",
            "2 24876  55.5844 232.3948 0038536 106.8301 253.6210  2.00561837143138")

def test_sat_getName():
    sat = gnssShadowing.Sat("GPS BIIR-2  (PRN 13)",
            "1 24876U 97035A   17033.94912398  .00000083  00000-0  00000+0 0  9990",
            "2 24876  55.5844 232.3948 0038536 106.8301 253.6210  2.00561837143138")
    assert sat.getName() == "GPS BIIR-2  (PRN 13)"

def test_sat_getSatNumber():
    sat = gnssShadowing.Sat("GPS BIIR-2  (PRN 13)",
            "1 24876U 97035A   17033.94912398  .00000083  00000-0  00000+0 0  9990",
            "2 24876  55.5844 232.3948 0038536 106.8301 253.6210  2.00561837143138")
    assert sat.getSatNumber() == 24876

def test_availableSats():
    sats = gnssShadowing.SatStore("data/current.tle")
    deg2rad = gnssShadowing.deg2rad
    observer = gnssShadowing.Geodetic(deg2rad(52),deg2rad(11),0,0)
    availableSats = gnssShadowing.AvailableSats(sats, observer, deg2rad(5))
    availableSats.update(gnssShadowing.unixTimeInSeconds())
    assert availableSats.getNumSats() > 0
