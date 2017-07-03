#include<mex.h>
#include<math.h>
////////////////////////////////////////////////////

double MyMinMax(double *A, int m, int *Position, int Choice);

/////////////////////////////////////////////////////////////////////////

void GetVector(double *Input, int i, int j, int padm, int patchR, double *Vector);

/////////////////////////////////////////////////////////////////////////

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    double *Input, *Output, *Seed, *SeedX, *SeedY, *Vector, *counter, *CoorX, *CoorY;
    int m, padm, n, padn, i, padi, j, padj, patchR, patchSize, ChoiceM, ChoiceDir, Position[1];
    
    Input = mxGetPr(prhs[0]); // has been padarray
    Seed = mxGetPr(prhs[1]); // the very first min/max value for vertical/horizontal min/max performance
    SeedX = mxGetPr(prhs[2]); // the X Coordinate of the very first min/max value 
    SeedY = mxGetPr(prhs[3]); // the Y Coordinate of the very first min/max value 
    patchR = (int)mxGetScalar(prhs[4]); // the radius of the support
    ChoiceM = (int)mxGetScalar(prhs[5]); // 0 for min performance, 1 for max performance
    ChoiceDir = (int)mxGetScalar(prhs[6]); // 0 for vertical mim/max, 1 for horizontal min/max
    
    padm = mxGetM(prhs[0]);
    padn = mxGetN(prhs[0]);
    
    m = padm - 2*patchR;
    n = padn - 2*patchR;
    patchSize = 2*patchR + 1;
    
    
    plhs[0] = mxCreateDoubleMatrix(m, n, mxREAL);
    Output = mxGetPr(plhs[0]);
    
    plhs[1] = mxCreateDoubleMatrix(m, n, mxREAL);
    CoorX = mxGetPr(plhs[1]); // X coordinate of min/max value (in REAL WORLD coordinate)
    
    plhs[2] = mxCreateDoubleMatrix(m, n, mxREAL);
    CoorY = mxGetPr(plhs[2]);
    
    plhs[3] = mxCreateDoubleMatrix(1, 1, mxREAL);
    counter = mxGetPr(plhs[3]); // this is the counter for the third case (the else case)
    
    Vector = mxGetPr(mxCreateDoubleMatrix(1,patchSize, mxREAL));
    
    counter[0] = 0;
    
    if(ChoiceDir==0) // vertical min/max
    {
        for(j=0;j<n;j++)
        {
           Output[j*m] = Seed[j]; // initialize the very first min/max value
           CoorX[j*m] = SeedX[j];
           CoorY[j*m] = SeedY[j];
        }
        
        for(i=1;i<m;i++) // the first row has been initialized with the Seed.
        {
            padi= i + patchR;
            
            for(j=0;j<n;j++)
            {
                padj = j + patchR; 
                
                if(ChoiceM == 0) // min operation
                {
                    if((Output[j*m + i-1] <= Input[padj*padm + padi + patchR])&&(Output[j*m + i-1]!=Input[padj*padm + padi - patchR - 1]))
                    {
                        Output[j*m+i] = Output[j*m + i-1];
                        CoorX[j*m+i] = CoorX[j*m + i-1];
                        CoorY[j*m+i] = CoorY[j*m + i-1];
                    }
                    
                    else if(Output[j*m + i-1] > Input[padj*padm + padi + patchR])
                    {
                        Output[j*m+i]=  Input[padj*padm + padi + patchR];
                        CoorX[j*m+i] = (i + 1) + patchR; // translate the coordinate in C into real world coordinate
                        CoorY[j*m+i] = j + 1;
                    }
                    
                    else
                    {
                        GetVector(Input, i, j, padm, patchR, Vector);
                        Output[j*m+i] = MyMinMax(Vector, patchSize,Position, 0);
                        
                        CoorX[j*m+i] = (i + 1) + Position[0];  // translate the coordinate in C into real world coordinate
                        if(CoorX[j*m + i]<=0) CoorX[j*m + i] = CoorX[j*m + i] + patchR;
                        CoorY[j*m+i] = j + 1;
                        
                        counter[0] = counter[0] + 1;
                    }
                    
                }
                
                if(ChoiceM == 1) // max operation
                {
                    if((Output[j*m + i-1] >= Input[padj*padm + padi + patchR])&&(Output[j*m + i-1]!=Input[padj*padm + padi - patchR - 1]))
                    {
                        Output[j*m+i]= Output[j*m + i-1];
                        CoorX[j*m+i] = CoorX[j*m + i-1];
                        CoorY[j*m+i] = CoorY[j*m + i-1];
                    }
                    
                    else if(Input[padj*padm + padi + patchR] > Output[j*m + i-1] )
                    {
                        Output[j*m+i]= Input[padj*padm + padi + patchR];
                        CoorX[j*m+i] = (i + 1) + patchR; // translate the coordinate in C into real world coordinate
                        CoorY[j*m+i] = j + 1;
                    }
                    else
                    {
                        GetVector(Input, i, j, padm, patchR, Vector);
                        Output[j*m+i] = MyMinMax(Vector, patchSize, Position, 1);
                        
                        CoorX[j*m+i] = (i + 1) + Position[0]; // translate the coordinate in C into real world coordinate
                        if(CoorX[j*m + i]<=0) CoorX[j*m + i] = CoorX[j*m + i] + patchR;
                        CoorY[j*m+i] = j + 1;
                        
                        counter[0] = counter[0] + 1;
                    }
                    
                    
                }
            }
        }        
        
        
    }
    
    /* if(ChoiceDir==1) // horizontal min/max
    {
        for(i=0;i<m;i++)
        {
           Output[i] = Seed[i]; // initialize the very first min/max value
        }
        
        for(i=0;i<m;i++) // the first row has been initialized with the Seed.
        {
            padi = i + patchR; 
            
            for(j=1;j<n;j++) // the first column has been initialized with the Seed.
            {
                padj = j + patchR;
                
                if(ChoiceM == 0) // min operation
                {
                    if((Output[(j-1)*m + i] <= Input[(padj - patchR - 1)*padm + padi]) && (Output[j*m + i-1] <= Input[(padj + patchR)*padm + padi]))
                    {
                        Output[j*m+i]= Output[j*m + i-1];
                    }
                    
                    if((Input[(padj - patchR - 1)*padm + padi] < Output[(j-1)*m + i]) && (Input[(padj - patchR - 1)*padm + padi] <= Input[(padj + patchR)*padm + padi]))
                    {
                        Output[j*m+i]= Input[(padj - patchR - 1)*padm + padi];
                    }
                    
                    if((Input[(padj + patchR)*padm + padi] < Output[(j-1)*m + i]) && (Input[(padj + patchR)*padm + padi] < Input[(padj - patchR - 1)*padm + padi]))
                    {
                        Output[j*m+i]= Input[(padj + patchR)*padm + padi];
                    }
                    
                }
                
                if(ChoiceM == 1) // max operation
                {
                    if((Output[(j-1)*m + i] >= Input[(padj - patchR - 1)*padm + padi]) && (Output[j*m + i-1] >= Input[(padj + patchR)*padm + padi]))
                    {
                        Output[j*m+i]= Output[j*m + i-1];
                    }
                    
                    if((Input[(padj - patchR - 1)*padm + padi] > Output[(j-1)*m + i]) && (Input[(padj - patchR - 1)*padm + padi] >= Input[(padj + patchR)*padm + padi]))
                    {
                        Output[j*m+i]= Input[(padj - patchR - 1)*padm + padi];
                    }
                    
                    if((Input[(padj + patchR)*padm + padi] > Output[(j-1)*m + i]) && (Input[(padj + patchR)*padm + padi] > Input[(padj - patchR - 1)*padm + padi]))
                    {
                        Output[j*m+i]= Input[(padj + patchR)*padm + padi];
                    }
                    
                }
            }
        }
            
    }*/
        
}


double MyMinMax(double *A, int m, int *Position, int Choice)
{
    double temp = A[0];
    int i;
    
    Position[0] = -(m-1)/2;
    
    if(Choice == 0)// min
    {
        for(i=0;i<m;i++)
        {
            if(temp>A[i])
            {
                temp = A[i];
                Position[0] = i - (m-1)/2;
            }
        }
    }
    
    if(Choice == 1)// max
    {
        for(i=0;i<m;i++)
        {
            if(temp<A[i])
            {
                temp = A[i];
                Position[0] = i - (m-1)/2;
            }
        }
    }
    
    return temp;
    
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
void GetVector(double *Input, int i, int j, int padm, int patchR, double *Vector)
{
    int padi, padj, s;
    
    padi = i + patchR;
    padj = j + patchR;
    
    //if(Dir==0)//get column vector
    {
        for(s=-patchR;s<=patchR;s++)
        {Vector[s + patchR]=Input[padj*padm + padi + s];}  
    }
    
    /*if(Dir==1)//get row vector
    {
        for(s=-patchR;s<patchR;s++)
        {Vector[s + patchR]=Input[(padj + s)*padm + padi];}  
    }*/
    
}