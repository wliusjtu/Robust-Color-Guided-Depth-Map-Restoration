#include<mex.h>
#include<math.h>

void GetBlock(int cur_i, int cur_j, int m, int n, int winR, double *Image, double *Block, double *Coor, double *BW, double *BWBlock, int InvFlag);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *DepthOri, *DepthUpd;
    double *BW, *BWAdp, *DepthWeightLUT, sigmaSData, sigmaSSmooth;
    int m, n, rData, rSmooth, winSizeData, winSizeSmooth;
    
    DepthUpd = mxGetPr(prhs[0]); // the newly updated depth map
    rSmooth = (int)mxGetScalar(prhs[1]); // radius of the smoothness term
    BW = mxGetPr(prhs[2]); // bandwidth matrix
    sigmaSSmooth = mxGetScalar(prhs[3]);// spatial sigma for the smoothness term
    BWAdp = mxGetPr(prhs[4]);
    DepthWeightLUT = mxGetPr(prhs[5]);
    
    m = mxGetM(prhs[0]); // row number
    n = mxGetN(prhs[0]); // column number
    
    int mLUT = mxGetM(prhs[5]);
    int nLUT = mxGetN(prhs[5]);
    
    winSizeSmooth = 2*rSmooth + 1; //window size of the smoothness term
    
    int i, j;
    
    ////////////////////////
   
    
    double *WeightSpatialSmooth = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));// spatial weight for the smoothness term
    double WeightSum = 0;
    
    for(i=0;i<winSizeSmooth; i++)
    {
        for(j=0;j<winSizeSmooth; j++)
        {
            WeightSpatialSmooth[j*winSizeSmooth + i] = exp(-((double)(i - rSmooth)*(double)(i - rSmooth) + 
                    (double)(j - rSmooth)*(double)(j - rSmooth))/(2*sigmaSSmooth*sigmaSSmooth)); 
            WeightSum = WeightSum + WeightSpatialSmooth[j*winSizeSmooth + i];
        }
    }
    /*for(i=0;i<winSizeSmooth; i++)
    {
        for(j=0;j<winSizeSmooth; j++)
        {
            WeightSpatialSmooth[j*winSizeSmooth + i] = WeightSpatialSmooth[j*winSizeSmooth + i]/WeightSum; 
        }
    }*/

    ///////////////
   
    /************ compute weight *************/
    int s, t, counter;
    int InvFlag = -1; // if the pixle in the window is outside the image, set the corresponding coordinate in Coor as InvFlag.
    double diff;

    
    /************* compute weight for the smoothness term **********************/
    plhs[0] = mxCreateSparse(m*n, m*n, winSizeSmooth*winSizeSmooth*m*n, mxREAL); 
    double *WeightSmoothPr = mxGetPr(plhs[0]); // the output depth weight sparse matrix for smoothness term
    mwIndex *WeightSmoothIr = mxGetIr(plhs[0]);
    mwIndex *WeightSmoothJc = mxGetJc(plhs[0]);
    
    double *BlockSmooth = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));
    double *CoorSmooth = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL)); // to label the coordinate of the pixel in the sparse matrix
    double *BWBlock = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));
    double sigmaD;
    
    WeightSmoothJc[0] = 0;
    counter = 0;
    
    for(j=0;j<n;j++)
    {
        for(i=0;i<m;i++)
        {
            GetBlock(i, j, m, n, rSmooth, DepthUpd, BlockSmooth, CoorSmooth, BW, BWBlock, InvFlag);
            
            for(t=0;t<winSizeSmooth;t++)
            {
                for(s=0;s<winSizeSmooth;s++)
                {
                    if(CoorSmooth[t*winSizeSmooth + s]!=InvFlag)
                    { 
                        diff = DepthUpd[j*m + i] - BlockSmooth[t*winSizeSmooth + s]; 
                        
                        if(BWBlock[t*winSizeSmooth + s]<BW[j*m + i])
                        {
                            sigmaD = BWBlock[t*winSizeSmooth + s];
                            //mexPrintf("sigmaD: %f\n", sigmaD);
                        }
                        else
                        {
                            sigmaD = BW[j*m + i];
                        }
                        for(int p = 0; p<nLUT; p++)
                        {
                           // mexPrintf("sigmaD: %f, BWADP: %f\n",  sigmaD ,BWAdp[p]);
                            if((double) sigmaD == (double)BWAdp[p])
                            {
                                WeightSmoothPr[counter] = DepthWeightLUT[p*mLUT + int(diff*diff)]*WeightSpatialSmooth[t*winSizeSmooth + s];
                                //mexPrintf("p*mLUT + int(diff*diff): %d\n", p*mLUT + int(diff*diff));
                                break;
                            }        
                        }
                        
                       // WeightSmoothPr[counter] = exp(-diff*diff/(2*sigmaD*sigmaD))*WeightSpatialSmooth[t*winSizeSmooth + s];
                  
                        WeightSmoothIr[counter] = (mwIndex)CoorSmooth[t*winSizeSmooth + s];

                        counter = counter + 1;
                    }
                }
            }
            WeightSmoothJc[j*m + i + 1] = counter;
        }
        
    }
  
    
}


/////////////////////////////////////////////   functions ///////////////////////////////////


/////////////////////////////////////////////////////
void GetBlock(int cur_i, int cur_j, int m, int n, int winR, double *Image, double *Block, double *Coor, double *BW, double *BWBlock, int InvFlag)
{
    int i, j, sMin, sMax, tMin, tMax, winSize;
    winSize = 2*winR + 1;
    
    for(i=0;i<winSize;i++)
    {
        for(j=0;j<winSize;j++)
        {
            Block[j*winSize + i] = 0;
            Coor[j*winSize + i] = InvFlag;
            BWBlock[j*winSize + i] = 0;
        }
    }
    
    if(cur_i < winR) sMin = winR - cur_i; 
    else sMin = 0;
    
    if(cur_i + winR > m - 1) sMax = winR + m - 1 - cur_i;
    else sMax = winSize - 1;
    
    if(cur_j < winR) tMin = winR - cur_j;
    else tMin = 0;
    
    if(cur_j + winR > n - 1) tMax = winR + n - 1 - cur_j;
    else tMax = winSize - 1;
    
    for(i=sMin;i<=sMax;i++)
    {
        for(j=tMin;j<=tMax;j++)
        {
            Block[j*winSize + i] = Image[(cur_j - winR + j)*m + cur_i - winR + i];
            Coor[j*winSize + i] = (cur_j - winR + j)*m + cur_i - winR + i;
            BWBlock[j*winSize + i] = BW[(cur_j - winR + j)*m + cur_i - winR + i];
        }
    }

}

