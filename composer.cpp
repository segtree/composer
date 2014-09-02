#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;
double rrand()
{
    return (double)(random()%1000001)/1000000;
}
class hmm
{
    private:
        int n,m;
        double **a,**b,*c;
        double calc(int *s,int t,double **f,double **g,double *q)
        {
            bool flag=0;
            if(!f||!g||!q)
            {
                flag=1;
                f=new double*[t]; g=new double*[t]; q=new double[t];
                for(int i=0;i<t;++i)
                {
                    f[i]=new double[n]; g[i]=new double[n];
                }
            }
            double u=0; q[0]=1;
            for(int i=0;i<n;++i) f[0][i]=c[i];
            for(int i=1;i<t;++i)
            {
                q[i]=0;
                for(int j=0;j<n;++j)
                {
                    f[i][j]=0;
                    for(int k=0;k<n;++k) f[i][j]+=f[i-1][k]*a[k][j]*b[j][s[i]];
                    q[i]+=f[i][j];
                }
                for(int j=0;j<n;++j) f[i][j]/=q[i];
                u+=log(q[i]);
            }
            for(int i=0;i<n;++i) g[t-1][i]=1;
            for(int i=t-2;i>=0;--i)
            {
                for(int j=0;j<n;++j)
                {
                    g[i][j]=0;
                    for(int k=0;k<n;++k) g[i][j]+=g[i+1][k]*a[j][k]*b[k][s[i+1]];
                }
                for(int j=0;j<n;++j) g[i][j]/=q[i];
            }
            if(flag)
            {
                for(int i=0;i<t;++i)
                {
                    delete f[i]; delete g[i];
                }
                delete f; delete g; delete q;
            }
            return u;
        }
    public:
        void randomize()
        {
            srand(time(0));
            for(int i=0;i<n;++i)
            {
                double s=0;
                for(int j=0;j<n;++j) s+=(a[i][j]=rand());
                for(int j=0;j<n;++j) a[i][j]/=s;
                s=0;
                for(int j=0;j<m;++j) s+=(b[i][j]=rand());
                for(int j=0;j<m;++j) b[i][j]/=s;
                c[i]=rand();
            }
            double s=0; for(int i=0;i<n;++i) s+=c[i];
            for(int i=0;i<n;++i) c[i]/=s;
        }
        hmm(int _n,int _m)
        {
            n=_n; m=_m;
            a=new double*[n]; b=new double*[n]; c=new double[n];
            for(int i=0;i<n;++i) {a[i]=new double[n]; b[i]=new double[m];}
            randomize();
        }
        double adjust(int tot,int **ss,int *len)
        {
            double ***f,***g,*p,***aa,***bb,**cc,**q;
            f=new double**[tot]; g=new double**[tot]; p=new double[tot]; q=new double*[tot];
            aa=new double**[tot]; bb=new double**[tot]; cc=new double*[tot];
            for(int i=0;i<tot;++i)
            {
                f[i]=new double*[len[i]]; g[i]=new double*[len[i]]; q[i]=new double[len[i]];
                for(int j=0;j<len[i];++j)
                {
                    f[i][j]=new double[n]; g[i][j]=new double[n];
                }
                aa[i]=new double*[n]; bb[i]=new double*[n]; cc[i]=new double[n];
                for(int j=0;j<n;++j)
                {
                    aa[i][j]=new double[n]; bb[i][j]=new double[m];
                }
            }
            #pragma omp parallel for
            for(int i=0;i<tot;++i)
            {
                p[i]=calc(ss[i],len[i],f[i],g[i],q[i]);
                for(int j=0;j<n;++j) cc[i][j]=f[i][0][j]*g[i][0][j];
                for(int j=0;j<n;++j)
                {
                    for(int k=0;k<n;++k)
                    {
                        aa[i][j][k]=0;
                        for(int u=0;u<len[i]-1;++u)
                            aa[i][j][k]+=f[i][u][j]*a[j][k]*g[i][u+1][k]*b[k][ss[i][u+1]];
                    }
                    for(int k=0;k<m;++k)
                    {
                        bb[i][j][k]=0;
                        for(int u=0;u<len[i]-1;++u) if(ss[i][u]==k)
                            bb[i][j][k]+=f[i][u][j]*g[i][u][j]*q[i][u];
                    }
                }
            }
            for(int i=0;i<n;++i)
            {
                c[i]=0;
                for(int j=0;j<n;++j) a[i][j]=0;
                for(int j=0;j<m;++j) b[i][j]=0;
            }
            for(int i=0;i<tot;++i)
                for(int j=0;j<n;++j)
                {
                    c[j]+=cc[i][j];
                    for(int k=0;k<n;++k) a[j][k]+=aa[i][j][k];
                    for(int k=0;k<m;++k) b[j][k]+=bb[i][j][k];
                }
            double s=0;
            for(int i=0;i<n;++i) s+=c[i];
            for(int i=0;i<n;++i) c[i]/=s;
            for(int i=0;i<n;++i)
            {
                s=0;
                for(int j=0;j<n;++j) s+=a[i][j];
                for(int j=0;j<n;++j) a[i][j]/=s;
                s=0;
                for(int j=0;j<m;++j) s+=b[i][j];
                for(int j=0;j<m;++j) b[i][j]/=s;
            }
            s=0;
            for(int i=0;i<tot;++i) s+=p[i];
            for(int i=0;i<tot;++i)
            {
                for(int j=0;j<len[i];++j) {delete f[i][j]; delete g[i][j];}
                for(int j=0;j<n;++j) {delete aa[i][j]; delete bb[i][j];}
            }
            for(int i=0;i<tot;++i)
            {
                delete f[i]; delete g[i]; delete q[i];
                delete aa[i]; delete bb[i]; delete cc[i];
            }
            delete f; delete g; delete p; delete q;
            delete aa; delete bb; delete cc;
            return s;
        }
        int getinit(int *s,int t)
        {
            double **f=new double*[t];
            for(int i=0;i<t;++i) f[i]=new double[n];
            for(int i=0;i<n;++i) f[0][i]=log(c[i]*b[i][s[0]]);
            for(int i=1;i<t;++i)
                for(int j=0;j<n;++j)
                {
                    f[i][j]=-1e100;
                    for(int k=0;k<n;++k) f[i][j]=max(f[i][j],f[i-1][k]+log(a[k][j]*b[j][s[i]]));
                }
            int x=0;
            for(int i=0;i<n;++i) if(f[t-1][i]>f[t-1][x]) x=i;
            for(int i=0;i<t;++i) delete f[i];
            delete f; return x;
        }
        double conjecture(int *s,int t,int x0)
        {
            double **f=new double*[t];
            int **g=new int*[t];
            for(int i=0;i<t;++i) {f[i]=new double[n]; g[i]=new int[n];}
            for(int i=0;i<n;++i) f[0][i]=log(x0==-1?c[i]:a[x0][i]);
            for(int i=1;i<t;++i)
                for(int j=0;j<n;++j)
                {
                    f[i][j]=-1e100;
                    for(int k=0;k<n;++k) if(f[i][j]<f[i-1][k]+log(a[k][j]))
                    {
                        f[i][j]=f[i-1][k]+log(a[k][j]);
                        g[i][j]=k;
                    }
                }
            int x=0;
            for(int i=0;i<n;++i) if(f[t-1][i]>f[t-1][x]) x=i;
            for(int i=t-1;i>=0;--i)
            {
                double q=rrand();
                for(int j=0;j<m;++j)
                {
                    q-=b[x][j];
                    if(q<0||j==m-1) {s[i]=j; break;}
                }
                x=g[i][x];
            }
            return f[t-1][s[t-1]];
        }
        double evolve(int *s,int t,int x)
        {
            double p=0;
            for(int i=0;i<t;++i)
            {
                double q=rrand();
                for(int j=0;j<n;++j)
                {
                    q-=(x==-1?c[j]:a[x][j]);
                    if(q<0||j==n-1)
                    {
                        p+=log(x==-1?c[j]:a[x][j]);
                        s[i]=x=j;
                        break;
                    }
                }
            }
            for(int i=0;i<t;++i)
            {
                double q=rrand();
                for(int j=0;j<m;++j)
                {
                    q-=b[s[i]][j];
                    if(q<0||j==m-1) {s[i]=j; break;}
                }
            }
            return p;
        }
        ~hmm()
        {
            for(int i=0;i<n;++i) {delete a[i]; delete b[i];}
            delete a; delete b; delete c;
        }
        double test(int *s,int len) {return calc(s,len,0,0,0);}
};
int main()
{
    hmm gen(100,14);
    int n; scanf("%d",&n);
    int *s=new int[128];
    for(int i=0;i<n;++i) scanf("%d",&s[i]);
    for(int i=0;i<100;++i) gen.adjust(1,&s,&n);
    gen.evolve(s,32,-1);
    for(int i=0;i<32;++i)
    {
        if(i) printf(" ");
        printf("%d",s[i]+7);
    }
    return 0;
}
