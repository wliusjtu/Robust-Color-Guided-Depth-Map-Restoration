#include<mex.h>
#include<math.h>

/////////////////////////////////////////////////////////////////////////

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    
    double *Depth, *MinCoorX, *MinCoorY, *MaxCoorX, *MaxCoorY, *RS;
    double tempMin, tempMax, RelSmooth;
    int patchR, i, j, m, n, s, t, i_offset_min, j_offset_min, i_offset_max, j_offset_max;
    
    Depth = mxGetPr(prhs[0]); // NOT padarray
    MinCoorX = mxGetPr(prhs[1]);
    MinCoorY = mxGetPr(prhs[2]);
    MaxCoorX = mxGetPr(prhs[3]);
    MaxCoorY = mxGetPr(prhs[4]);
    patchR = (int)mxGetScalar(prhs[5]);
    
    m = mxGetM(prhs[0]);
    n = mxGetN(prhs[0]);
    
    plhs[0] = mxCreateDoubleMatrix(m, n, mxREAL);
    RS = mxGetPr(plhs[0]);
   
    
    for(i=0;i<m;i++)
    {
        for(j=0;j<n;j++)
        {
            tempMin = 0;
            tempMax = 0;
            
            for(s=-patchR;s<=patchR;s++)
            {
                i_offset_min = MinCoorX[j*m+i] + s - 1;// -1 because the coordinate in MinCoorX is in real world coordinate
                if(i_offset_min<0) i_offset_min = 0;
                if(i_offset_min>m-1) i_offset_min = m-1;
                
                i_offset_max = MaxCoorX[j*m+i] + s - 1;
                if(i_offset_max<0) i_offset_max = 0;
                if(i_offset_max>m-1) i_offset_max = m-1;
                
                for(t=-patchR;t<=patchR;t++)
                {
                    j_offset_min = MinCoorY[j*m+i] + t - 1;
                    if(j_offset_min<0) j_offset_min = 0;
                    if(j_offset_min>n-1) j_offset_min = n-1;
                
                    j_offset_max = MaxCoorY[j*m+i] + t -1 ;
                    if(j_offset_max<0) j_offset_max = 0;
                    if(j_offset_max>n-1) j_offset_max = n-1;
                
                    
                    tempMin = tempMin + Depth[j_offset_min*m + i_offset_min];
                    tempMax = tempMax + Depth[j_offset_max*m + i_offset_max];
                       
                }
            }
            
            RelSmooth = tempMin/tempMax;
            RS[j*m + i] = RelSmooth;
            
        }
    }
    
}