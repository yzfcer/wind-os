#include <math.h>
/****************************************************************************************
//Control parameter
// l: l = 0, Fourier transform; l = 1, inverse Fourier transform
// n: The number of input points is even, generally 32, 64128,..., 1024, etc
// k: In fact, k is the number of times that n sampling data can be decomposed into even power and odd power
// Input parameters:
// pr[]: When l = 0, store the real part of N-point sampling data
// When l = 1, the inverse Fourier transform stores n real parts of the original Fourier transform
// pi[]: When l = 0, store the virtual part of N-point sampling data 
// When l = 1, the inverse Fourier transform stores n imaginary parts of the original Fourier transform
// Output parameters:
// fr[]: When l = 0, return the real part of Fourier transform
// When l = 1, return the real part of inverse Fourier transform
// fi[]: When l = 0, return the imaginary part of Fourier transform
// When l = 1, return the imaginary part of inverse Fourier transform
****************************************************************************************/
void kbfft(int l,int n,int k,float *pr,float *pi,float *fr,float *fi)
{ 
    int it,m,is,i,j,nv,l0;
    float p,q,s,vr,vi,poddr,poddi;

    //sort
    for (it=0; it<=n-1; it++) 
    { 
        m=it; 
        is=0;
        for (i=0; i<=k-1; i++)
        { 
            j=m/2; 
            is=2*is+(m-2*j); 
            m=j;
            fr[it]=pr[is]; 
            fi[it]=pi[is];
        }
    }
    
    //Butterfly operation
    pr[0]=1.0; pi[0]=0.0;
    p=6.283185306/(1.0*n);
    pr[1]=cos(p); pi[1]=-sin(p);
    if (l!=0) pi[1]=-pi[1];
    for (i=2; i<=n-1; i++)
    { 
        p=pr[i-1]*pr[1]; 
        q=pi[i-1]*pi[1];
        s=(pr[i-1]+pi[i-1])*(pr[1]+pi[1]);
        pr[i]=p-q; 
        pi[i]=s-p-q;
    }
    for (it=0; it<=n-2; it=it+2)
    { 
        vr=fr[it]; 
        vi=fi[it];
        fr[it]=vr+fr[it+1]; 
        fi[it]=vi+fi[it+1];
        fr[it+1]=vr-fr[it+1]; 
        fi[it+1]=vi-fi[it+1];
    }
    m=n/2; 
    nv=2;
    for (l0=k-2; l0>=0; l0--)
    { 
        m=m/2; nv=2*nv;
        for (it=0; it<=(m-1)*nv; it=it+nv)
            for (j=0; j<=(nv/2)-1; j++)
            { 
                p=pr[m*j]*fr[it+j+nv/2];
                q=pi[m*j]*fi[it+j+nv/2];
                s=pr[m*j]+pi[m*j];
                s=s*(fr[it+j+nv/2]+fi[it+j+nv/2]);
                poddr=p-q; 
                poddi=s-p-q;
                fr[it+j+nv/2]=fr[it+j]-poddr;
                fi[it+j+nv/2]=fi[it+j]-poddi;
                fr[it+j]=fr[it+j]+poddr;
                fi[it+j]=fi[it+j]+poddi;
            }
    }
    if (l!=0)
        for (i=0; i<=n-1; i++)
        { 
            fr[i]=fr[i]/(1.0*n);
            fi[i]=fi[i]/(1.0*n);
        }
    return;
}
