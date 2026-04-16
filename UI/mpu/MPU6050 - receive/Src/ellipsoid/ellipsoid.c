#include "main.h"
void fit(const double x_data[], const int x_size[], const double y_data[], 
	     const int y_size[], const double z_data[], const int z_size[], 
			 double *VOX, double *VOY, double *VOZ, double *VRX, double *VRY, double *VRZ);  

void powerCal(const double a_data[], const int a_size[1], double y_data[], int
           y_size[1]);		 
void mpower(const double a[36], double c[36]);

//Õ÷«Ú–£—È
void ellipsoid()
{
	double X[100] = {1,257,3,54};
	double Y[100] = {6,8,589,67};
	double Z[100] = {2,562,3,3334};
	int x_size = 100, y_size = 100, z_size = 100;
	
	double OX, OY, OZ, RX, RY,RZ;
	fit(X, &x_size, Y, &y_size, Z, &z_size, &OX, &OY, &OZ, &RX, &RY, &RZ);

}




void b_sqrt(double *x)
{
    *x = sqrt(*x);
}

void fit(const double x_data[], const int x_size[], const double y_data[], 
	     const int y_size[], const double z_data[], const int z_size[], 
			 double *VOX, double *VOY, double *VOZ, double *VRX, double *VRY, double *VRZ)               
{
    double Y_data[1000];
      int Y_size[1];
      double varargin_2_data[1000];
      int varargin_2_size[1];
      int K_size_idx_0;
      int loop_ub;
      double K_data[6000];
      int i0;
      int cr;
      double y[36];
      double a_data[6000];
      int br;
      int ic;
      double a[36];
      int ar;
      int ib;
      double b_data[6000];
      int ia;
      double V[6];
      powerCal(y_data, y_size, Y_data, Y_size);
      powerCal(z_data, z_size, varargin_2_data, varargin_2_size);
      K_size_idx_0 = Y_size[0];
      loop_ub = Y_size[0];
      if (0 <= loop_ub - 1) {
        memcpy(&K_data[0], &Y_data[0], (unsigned int)(loop_ub * (int)sizeof(double)));
      }

      loop_ub = varargin_2_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        K_data[i0 + K_size_idx_0] = varargin_2_data[i0];
      }

      loop_ub = x_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        K_data[i0 + (K_size_idx_0 << 1)] = x_data[i0];
      }

      loop_ub = y_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        K_data[i0 + K_size_idx_0 * 3] = y_data[i0];
      }

      loop_ub = z_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        K_data[i0 + (K_size_idx_0 << 2)] = z_data[i0];
      }

      loop_ub = x_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        K_data[i0 + K_size_idx_0 * 5] = 1.0;
      }

      powerCal(x_data, x_size, Y_data, Y_size);
      loop_ub = Y_size[0];
      for (i0 = 0; i0 < loop_ub; i0++) {
        Y_data[i0] = -Y_data[i0];
      }

      for (i0 = 0; i0 < K_size_idx_0; i0++) {
        for (cr = 0; cr < 6; cr++) {
          a_data[cr + 6 * i0] = K_data[i0 + K_size_idx_0 * cr];
        }
      }

      if (K_size_idx_0 == 1) {
        for (i0 = 0; i0 < 6; i0++) {
          for (cr = 0; cr < 6; cr++) {
            y[i0 + 6 * cr] = 0.0;
            for (loop_ub = 0; loop_ub < 1; loop_ub++) {
              y[i0 + 6 * cr] += a_data[i0] * K_data[cr];
            }
          }
        }
      } else {
        memset(&y[0], 0, 36U * sizeof(double));
        for (cr = 0; cr <= 31; cr += 6) {
          for (ic = cr; ic + 1 <= cr + 6; ic++) {
            y[ic] = 0.0;
          }
        }

        br = 0;
        for (cr = 0; cr <= 31; cr += 6) {
          ar = -1;
          i0 = br + K_size_idx_0;
          for (ib = br; ib + 1 <= i0; ib++) {
            if (K_data[ib] != 0.0) {
              ia = ar;
              for (ic = cr; ic + 1 <= cr + 6; ic++) {
                ia++;
                y[ic] += K_data[ib] * a_data[ia];
              }
            }

            ar += 6;
          }

          br += K_size_idx_0;
        }
      }

      mpower(y, a);
      for (i0 = 0; i0 < K_size_idx_0; i0++) {
        for (cr = 0; cr < 6; cr++) {
          b_data[cr + 6 * i0] = K_data[i0 + K_size_idx_0 * cr];
        }
      }

      loop_ub = (short)K_size_idx_0;
      for (i0 = 0; i0 < loop_ub; i0++) {
        for (cr = 0; cr < 6; cr++) {
          a_data[cr + 6 * i0] = 0.0;
        }
      }

      if (K_size_idx_0 != 0) {
        loop_ub = 6 * (K_size_idx_0 - 1);
        for (cr = 0; cr <= loop_ub; cr += 6) {
          for (ic = cr + 1; ic <= cr + 6; ic++) {
            a_data[ic - 1] = 0.0;
          }
        }

        br = 0;
        for (cr = 0; cr <= loop_ub; cr += 6) {
          ar = -1;
          for (ib = br; ib + 1 <= br + 6; ib++) {
            if (b_data[ib] != 0.0) {
              ia = ar;
              for (ic = cr; ic + 1 <= cr + 6; ic++) {
                ia++;
                a_data[ic] += b_data[ib] * a[ia];
              }
            }

            ar += 6;
          }

          br += 6;
        }
      }

      if (((short)K_size_idx_0 == 1) || (Y_size[0] == 1)) {
        loop_ub = (short)K_size_idx_0;
        for (i0 = 0; i0 < 6; i0++) {
          V[i0] = 0.0;
          for (cr = 0; cr < loop_ub; cr++) {
            V[i0] += a_data[i0 + 6 * cr] * Y_data[cr];
          }
        }
      } else {
        for (ic = 0; ic < 6; ic++) {
          V[ic] = 0.0;
        }

        ar = -1;
        for (ib = 0; ib + 1 <= (short)K_size_idx_0; ib++) {
          if (Y_data[ib] != 0.0) {
            ia = ar;
            for (ic = 0; ic < 6; ic++) {
              ia++;
              V[ic] += Y_data[ib] * a_data[ia];
            }
          }

          ar += 6;
        }
      }

      *VOX = -V[2] / 2.0;

      /*  -C/2 */
      *VOY = -V[3] / V[0] / 2.0;

      /*  -D/A/2 */
      *VOZ = -V[4] / V[1] / 2.0;

      /*  -E/B/2 */
      *VRX = ((*VOX * *VOX + V[0] * (*VOY * *VOY)) + V[1] * (*VOZ * *VOZ)) - V[5];
      b_sqrt(VRX);

      /*  sqrt(VOX^2 + A*VOY^2 + B*VOZ^2 - F) */
      *VRY = *VRX * *VRX / V[0];
      b_sqrt(VRY);

      /*  sqrt(VRX^2/A); */
      *VRZ = *VRX * *VRX / V[1];
      b_sqrt(VRZ);
}



void powerCal(const double a_data[], const int a_size[1], double y_data[], int
           y_size[1])
{
  int k;
  y_size[0] = (short)a_size[0];
  for (k = 0; k + 1 <= (short)a_size[0]; k++) {
    y_data[k] = a_data[k] * a_data[k];
  }
}


void invNxN(const double x[36], double y[36])
{
  int i1;
  double b_x[36];
  int j;
  signed char ipiv[6];
  int c;
  int iy;
  int k;
  signed char p[6];
  int ix;
  double smax;
  double s;
  int jy;
  int i;
  for (i1 = 0; i1 < 36; i1++) {
    y[i1] = 0.0;
    b_x[i1] = x[i1];
  }

  for (i1 = 0; i1 < 6; i1++) {
    ipiv[i1] = (signed char)(1 + i1);
  }

  for (j = 0; j < 5; j++) {
    c = j * 7;
    iy = 0;
    ix = c;
    smax = abs(b_x[c]);
    for (k = 2; k <= 6 - j; k++) {
      ix++;
      s = abs(b_x[ix]);
      if (s > smax) {
        iy = k - 1;
        smax = s;
      }
    }

    if (b_x[c + iy] != 0.0) {
      if (iy != 0) {
        ipiv[j] = (signed char)((j + iy) + 1);
        ix = j;
        iy += j;
        for (k = 0; k < 6; k++) {
          smax = b_x[ix];
          b_x[ix] = b_x[iy];
          b_x[iy] = smax;
          ix += 6;
          iy += 6;
        }
      }

      i1 = (c - j) + 6;
      for (i = c + 1; i + 1 <= i1; i++) {
        b_x[i] /= b_x[c];
      }
    }

    iy = c;
    jy = c + 6;
    for (i = 1; i <= 5 - j; i++) {
      smax = b_x[jy];
      if (b_x[jy] != 0.0) {
        ix = c + 1;
        i1 = (iy - j) + 12;
        for (k = 7 + iy; k + 1 <= i1; k++) {
          b_x[k] += b_x[ix] * -smax;
          ix++;
        }
      }

      jy += 6;
      iy += 6;
    }
  }

  for (i1 = 0; i1 < 6; i1++) {
    p[i1] = (signed char)(1 + i1);
  }

  for (k = 0; k < 5; k++) {
    if (ipiv[k] > 1 + k) {
      iy = p[ipiv[k] - 1];
      p[ipiv[k] - 1] = p[k];
      p[k] = (signed char)iy;
    }
  }

  for (k = 0; k < 6; k++) {
    c = p[k] - 1;
    y[k + 6 * (p[k] - 1)] = 1.0;
    for (j = k; j + 1 < 7; j++) {
      if (y[j + 6 * c] != 0.0) {
        for (i = j + 1; i + 1 < 7; i++) {
          y[i + 6 * c] -= y[j + 6 * c] * b_x[i + 6 * j];
        }
      }
    }
  }

  for (j = 0; j < 6; j++) {
    iy = 6 * j;
    for (k = 5; k >= 0; k--) {
      jy = 6 * k;
      if (y[k + iy] != 0.0) {
        y[k + iy] /= b_x[k + jy];
        for (i = 0; i + 1 <= k; i++) {
          y[i + iy] -= y[k + iy] * b_x[i + jy];
        }
      }
    }
  }
}

void mpower(const double a[36], double c[36])
{
  invNxN(a, c);
}

