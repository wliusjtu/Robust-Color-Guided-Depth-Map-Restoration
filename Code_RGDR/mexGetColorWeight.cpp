#include<mex.h>
#include<math.h>

void GetBlock(int cur_i, int cur_j, int m, int n, int winR, double *Image, double *Block, double *Coor, int InvFlag);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    double *CR, *CG, *CB, *ColorWeightLUT;
    double sigmaC;
    int m, n, rSmooth, winSizeSmooth;
    
    CR = mxGetPr(prhs[0]);
    CG = mxGetPr(prhs[1]);
    CB = mxGetPr(prhs[2]);
    sigmaC = mxGetScalar(prhs[3]);
    rSmooth = (int)mxGetScalar(prhs[4]);
    ColorWeightLUT = mxGetPr(prhs[5]);
    
    m = mxGetM(prhs[0]); // row number
    n = mxGetN(prhs[0]); // column number
    
    winSizeSmooth = 2*rSmooth + 1; //window size of the smoothness term
    

    
    /************* compute weight for the smoothness term **********************/
    plhs[0] = mxCreateSparse(m*n, m*n, winSizeSmooth*winSizeSmooth*m*n, mxREAL); 
    double *WeightColorPr = mxGetPr(plhs[0]); // the output depth weight  sparse matrix for data term
    mwIndex *WeightColorIr = mxGetIr(plhs[0]);
    mwIndex *WeightColorJc = mxGetJc(plhs[0]);

    
    /************ compute weight *************/
    int i, j, s, t, counter;
    int InvFlag = -1; // if the pixle in the window is outside the image, set the corresponding coordinate in Coor as InvFlag.
    double diffR, diffG, diffB;
    
    double *BlockR = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));
    double *BlockG = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));
    double *BlockB = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL));
    double *Coor = mxGetPr(mxCreateDoubleMatrix(winSizeSmooth, winSizeSmooth, mxREAL)); // to label the coordinate of the pixel in the sparse matrix
    
    
    WeightColorJc[0] = 0;
    counter = 0;
    
    for(j=0;j<n;j++)
    {
        for(i=0;i<m;i++)
        {
            GetBlock(i, j, m, n, rSmooth, CR, BlockR, Coor, InvFlag);
            GetBlock(i, j, m, n, rSmooth, CG, BlockG, Coor, InvFlag);
            GetBlock(i, j, m, n, rSmooth, CB, BlockB, Coor, InvFlag);
            
            for(t=0;t<winSizeSmooth;t++)
            {
                for(s=0;s<winSizeSmooth;s++)
                {
                    if(Coor[t*winSizeSmooth + s]!=InvFlag)
                    { 
                        diffR = CR[j*m + i] - BlockR[t*winSizeSmooth + s];
                        diffG = CG[j*m + i] - BlockG[t*winSizeSmooth + s]; 
                        diffB = CB[j*m + i] - BlockB[t*winSizeSmooth + s]; 
                        
                        WeightColorPr[counter] = ColorWeightLUT[int(diffR*diffR + diffG*diffG + diffB*diffB)];

                        //WeightColorPr[counter] = exp(-(diffR*diffR + diffG*diffG + diffB*diffB)/(3*2*sigmaC*sigmaC));
                  
                        WeightColorIr[counter] = (mwIndex)Coor[t*winSizeSmooth + s];

                        counter = counter + 1;
                    }
                }
            }
            WeightColorJc[j*m + i + 1] = counter;
        }
        
    }
   
}


/////////////////////////////////////////////   functions ///////////////////////////////////
void GetBlock(int cur_i, int cur_j, int m, int n, int winR, double *Image, double *Block, double *Coor, int InvFlag)
{
    int i, j, sMin, sMax, tMin, tMax, winSize;
    winSize = 2*winR + 1;
    
    for(i=0;i<winSize;i++)
    {
        for(j=0;j<winSize;j++)
        {
            Block[j*winSize + i] = 0;
            Coor[j*winSize + i] = InvFlag;
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
        }
    }

}