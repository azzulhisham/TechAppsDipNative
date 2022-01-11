class Index3D 
{
    public:
        int I;
        int J;
        int K;

        Index3D(int i, int j, int k)  {
            I = i;
            J = j;
            K = k;
        }

        Index3D(int v) {
            I = v;
            J = v;
            K = v;
        }
};