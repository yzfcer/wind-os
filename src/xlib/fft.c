/****************************************************************************************
//控制参数
// l: l = 0, 傅立叶变换; l = 1, 逆傅立叶变换
// n: 输入的点数，为偶数，一般为32，64，128，...,1024等
// k: 满足n=2^k(k>0),实质上k是n个采样数据可以分解为偶次幂和奇次幂的次数
// 入口参数：
// pr[]: l=0时，存放N点采样数据的实部
// l=1时,逆傅立叶变换 存放原傅立叶变换的N个实部
// pi[]: l=0时，存放N点采样数据的虚部 
// l=1时,逆傅立叶变换 存放原傅立叶变换的N个虚部
// 出口参数：
// fr[]: l=0, 返回傅立叶变换的实部
// l=1, 返回逆傅立叶变换的实部
// fi[]: l=0, 返回傅立叶变换的虚部
// l=1, 返回逆傅立叶变换的虚部
****************************************************************************************/
void kbfft(int l,int n,int k,float *pr,float *pi,float *fr,float *fi)
{ 
    int it,m,is,i,j,nv,l0;
    float p,q,s,vr,vi,poddr,poddi;
    
    //排序
    for (it=0; it<=n-1; it++) 
 { m=it; is=0;
 for (i=0; i<=k-1; i++)
 { j=m/2; is=2*is+(m-2*j); m=j;
 fr[it]=pr[is]; fi[it]=pi[is];
 }
 }
    
    //蝶形运算
    pr[0]=1.0; pi[0]=0.0;
    p=6.283185306/(1.0*n);
    pr[1]=cos(p); pi[1]=-sin(p);
    if (l!=0) pi[1]=-pi[1];
    for (i=2; i<=n-1; i++)
 { p=pr[i-1]*pr[1]; q=pi[i-1]*pi[1];
 s=(pr[i-1]+pi[i-1])*(pr[1]+pi[1]);
 pr[i]=p-q; pi[i]=s-p-q;
 }
    for (it=0; it<=n-2; it=it+2)
 { vr=fr[it]; vi=fi[it];
 fr[it]=vr+fr[it+1]; fi[it]=vi+fi[it+1];
 fr[it+1]=vr-fr[it+1]; fi[it+1]=vi-fi[it+1];
 }
    m=n/2; nv=2;
    for (l0=k-2; l0>=0; l0--)
 { m=m/2; nv=2*nv;
 for (it=0; it<=(m-1)*nv; it=it+nv)
  for (j=0; j<=(nv/2)-1; j++)
  { p=pr[m*j]*fr[it+j+nv/2];
  q=pi[m*j]*fi[it+j+nv/2];
  s=pr[m*j]+pi[m*j];
  s=s*(fr[it+j+nv/2]+fi[it+j+nv/2]);
  poddr=p-q; poddi=s-p-q;
  fr[it+j+nv/2]=fr[it+j]-poddr;
  fi[it+j+nv/2]=fi[it+j]-poddi;
  fr[it+j]=fr[it+j]+poddr;
  fi[it+j]=fi[it+j]+poddi;
  }
 }
    if (l!=0)
     for (i=0; i<=n-1; i++)
     { fr[i]=fr[i]/(1.0*n);
     fi[i]=fi[i]/(1.0*n);
        }
 return;
}