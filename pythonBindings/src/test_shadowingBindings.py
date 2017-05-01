
import gnssShadowing
import pytest
import numpy as np
import numpy.testing as nt

def test_Transformation():
    a=np.zeros(shape=(3,3),dtype="float32")
    b=np.ones(shape=(3,3),dtype="float32")
    c=gnssShadowing.Transformation(a,b)
    nt.assert_almost_equal(a,c.getSATSEZ())
    nt.assert_almost_equal(b,c.getSEZSAT())

def test_Shadowing():
    world=gnssShadowing.World("data/current.tle","data/uni.obj","Building")
    shadowing=world.m_shadowing
    now=gnssShadowing.unixTimeInSeconds()
    horizontal=world.m_satStore.getSat(0).findECEF(now).toENU(world.m_origin).toHorizontal()
    transformation=shadowing.computeTransformation(horizontal)
    assert type(transformation) == gnssShadowing.Transformation
    contour1=shadowing.computeContourFromTransformation(transformation)
    assert type(contour1) == gnssShadowing.Contour
    contour2=shadowing.computeContourFromHorizontal(horizontal)
    assert type(contour2) == gnssShadowing.Contour
    planeLevel=0
    eps=1e-3
    vol=shadowing.computeShadowVolume(contour1,transformation,planeLevel,eps)
    assert type(vol) == gnssShadowing.ShadowVolume

