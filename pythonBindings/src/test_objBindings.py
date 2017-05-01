
import gnssShadowing
import pytest
import numpy as np

def test_Material():
    a=gnssShadowing.Material()
    assert a.name == ""
    assert a.ambient.x == 0
    assert a.ambient.y == 0
    assert a.ambient.z == 0
    assert a.diffuse.x == 0
    assert a.diffuse.y == 0
    assert a.diffuse.z == 0

def test_Mtllib():
    a=gnssShadowing.Mtllib()
    a.read("data/uni.obj.mtl")
    b=a.getMaterial("ASPHALT_0")
    assert b.name == "ASPHALT_0"

def test_ObjFile():
    a=gnssShadowing.ObjFile("data/uni.obj")

