function MexFile
mex -largeArrayDims mexGetColorWeight.cpp
mex -largeArrayDims mexGetDepthWeight.cpp
mex -largeArrayDims mexGetWeightedDepth.cpp
mex mexConstantMinMax.cpp
mex mexGetRelSmooth.cpp