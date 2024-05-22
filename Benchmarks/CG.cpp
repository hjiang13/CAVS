#include <cmath>
#include <iostream>
#include <fstream>
#include <string>

#define T_last 3
#define naa na
#define nzz nz

void timer_clear(int i) {}
void timer_start(int i) {}
void timer_stop(int i) {}
double timer_read(int i) { return 0.0; }

void print_results(std::string bmname, char clss, int na, int nonzer, int niter, double t, double mflops, std::string optype, bool verified, std::string npbversion, std::string compiletime, std::string cs1, std::string cs2, std::string cs3, std::string cs4, std::string cs5, std::string cs6, std::string cs7) {
    std::cout << " Benchmark completed " << std::endl;
}

void conj_grad(int colidx[], int rowstr[], double x[], double z[], double a[], double p[], double q[], double r[], double& rnorm) {
    int cgit, cgitmax;
    double d, sum, rho, rho0, alpha, beta;
    cgitmax = 25;
    rho = 0.0;
    for (int j = 1; j <= naa + 1; j++) {
        q[j] = 0.0;
        z[j] = 0.0;
        r[j] = x[j];
        p[j] = r[j];
    }
    for (cgit = 1; cgit <= cgitmax; cgit++) {
        sum = 0.0;
        for (int j = 1; j <= lastrow - firstrow + 1; j++) {
            sum = sum + r[j] * r[j];
        }
        rho = sum;
        for (int j = 1; j <= lastrow - firstrow + 1; j++) {
            sum = 0.0;
            for (int k = rowstr[j]; k <= rowstr[j + 1] - 1; k++) {
                sum = sum + a[k] * p[colidx[k]];
            }
            q[j] = sum;
        }
        d = 0.0;
        for (int j = 1; j <= lastcol - firstcol + 1; j++) {
            d = d + p[j] * q[j];
        }
        alpha = rho / d;
        rho0 = rho;
        rho = 0.0;
        for (int j = 1; j <= lastcol - firstcol + 1; j++) {
            z[j] = z[j] + alpha * p[j];
            r[j] = r[j] - alpha * q[j];
            rho = rho + r[j] * r[j];
        }
        beta = rho / rho0;
        for (int j = 1; j <= lastcol - firstcol + 1; j++) {
            p[j] = r[j] + beta * p[j];
        }
    }
    sum = 0.0;
    for (int j = 1; j <= lastrow - firstrow + 1; j++) {
        double d = 0.0;
        for (int k = rowstr[j]; k <= rowstr[j + 1] - 1; k++) {
            d = d + a[k] * z[colidx[k]];
        }
        r[j] = d;
    }
    for (int j = 1; j <= lastcol - firstcol + 1; j++) {
        double d = x[j] - r[j];
        sum = sum + d * d;
    }
    rnorm = sqrt(sum);
}

void makea(int n, int nz, double a[], int colidx[], int rowstr[], int firstrow, int lastrow, int firstcol, int lastcol, int arow[], int acol[][n], double aelt[][n], int iv[]) {
    double rcond, shift;
    for (int j = 1; j <= nz; j++) {
        a[j] = 0.0;
    }
    for (int j = 1; j <= n; j++) {
        iv[j] = 0;
        rowstr[j] = 0;
    }
    for (int i = 1; i <= n + 1; i++) {
        colidx[i] = 0;
    }
    int iouter = 0;
    for (int i = 1; i <= n; i++) {
        int nnza = nonzer;
        sprnvc(n, nnza, v, iv, &(colidx[1]), &(a[1]), &(arow[1]), &(acol[1][1]), &(aelt[1][1]), firstrow, lastrow, firstcol, lastcol, rcond, shift);
        vecset(n, v, iv, &(colidx[1]), &(a[1]), &(arow[1]), &(acol[1][1]), &(aelt[1][1]), firstrow, lastrow, firstcol, lastcol, i, 0.5);
        for (int j = rowstr[i]; j <= rowstr[i + 1] - 1; j++) {
            a[j] = a[j] * rcond;
        }
    }
    sparse(a, colidx, rowstr, n, nz, &(arow[1]), &(acol[1][1]), &(aelt[1][1]), firstrow, lastrow, v, &(iv[1]), &(a[1]), rcond, shift);
}

int main() {
    int colidx[nz], rowstr[na + 1], iv[na], arow[na], acol[nz], aelt[nz];
    double a[nz], x[na + 2], z[na + 2], p[na + 2], q[na + 2], r[na + 2];
    int i, j, k, it;
    double zeta, randlc;
    double rnorm;
    double norm_temp1, norm_temp2;
    double t, mflops, tmax;
    char class;
    bool verified;
    double zeta_verify_value, epsilon, err;
    int fstatus;
    std::string t_names[T_last];
    for (i = 1; i <= T_last; i++) {
        timer_clear(i);
    }
    std::ifstream timer_file("timer.flag");
    if (timer_file.good()) {
        timeron = true;
        t_names[t_init] = "init";
        t_names[t_bench] = "benchmk";
        t_names[t_conj_grad] = "conjgd";
        timer_file.close();
    } else {
        timeron = false;
    }
    timer_start(T_init);
    firstrow = 1;
    lastrow = na;
    firstcol = 1;
    lastcol = na;
    if (na == 1400 && nonzer == 7 && niter == 15 && shift == 10.0) {
        class = 'S';
        zeta_verify_value = 8.5971775078648;
    } else if (na == 7000 && nonzer == 8 && niter == 15 && shift == 12.0) {
        class = 'W';
        zeta_verify_value = 10.362595087124;
    } else if (na == 14000 && nonzer == 11 && niter == 15 && shift == 20.0) {
        class = 'A';
        zeta_verify_value = 17.130235054029;
    } else if (na == 75000 && nonzer == 13 && niter == 75 && shift == 60.0) {
        class = 'B';
        zeta_verify_value = 22.712745482631;
    } else if (na == 150000 && nonzer == 15 && niter == 75 && shift == 110.0) {
        class = 'C';
        zeta_verify_value = 28.973605592845;
    } else if (na == 1500000 && nonzer == 21 && niter == 100 && shift == 500.0) {
        class = 'D';
        zeta_verify_value = 52.514532105794;
    } else if (na == 9000000 && nonzer == 26 && niter == 100 && shift == 1500.0) {
        class = 'E';
        zeta_verify_value = 77.522164599383;
    } else {
        class = 'U';
    }
    std::cout << std::endl;
    std::cout << " NAS Parallel Benchmarks (NPB3.3-SER) - CG Benchmark" << std::endl;
    std::cout << " Size: " << na << std::endl;
    std::cout << " Iterations: " << niter << std::endl;
    std::cout << std::endl;
    naa = na;
    nzz = nz;
    tran = 314159265.0;
    amult = 1220703125.0;
    zeta = randlc(tran, amult);
    makea(naa, nzz, a, colidx, rowstr, firstrow, lastrow, firstcol, lastcol, arow, acol, aelt, iv);
    for (j = 1; j <= lastrow - firstrow + 1; j++) {
        for (k = rowstr[j]; k <= rowstr[j + 1] - 1; k++) {
            colidx[k] = colidx[k] - firstcol + 1;
        }
    }
    for (i = 1; i <= na + 1; i++) {
        x[i] = 1.0;
    }
    for (j = 1; j <= lastcol - firstcol + 1; j++) {
        q[j] = 0.0;
        z[j] = 0.0;
        r[j] = 0.0;
        p[j] = 0.0;
    }
    zeta = 0.0;
    for (it = 1; it <= 1; it++) {
        conj_grad(colidx, rowstr, x, z, a, p, q, r, rnorm);
        norm_temp1 = 0.0;
        norm_temp2 = 0.0;
        for (j = 1; j <= lastcol - firstcol + 1; j++) {
            norm_temp1 = norm_temp1 + x[j] * z[j];
            norm_temp2 = norm_temp2 + z[j] * z[j];
        }
        norm_temp2 = 1.0 / sqrt(norm_temp2);
        for (j = 1; j <= lastcol - firstcol + 1; j++) {
            x[j] = norm_temp2 * z[j];
        }
    }
    for (i = 1; i <= na + 1; i++) {
        x[i] = 1.0;
    }
    zeta = 0.0;
    timer_stop(T_init);
    std::cout << " Initialization time = " << timer_read(T_init) << " seconds" << std::endl;
    timer_start(T_bench);
    for (it = 1; it <= niter; it++) {
        if (timeron) {
            timer_start(T_conj_grad);
        }
        conj_grad(colidx, rowstr, x, z, a, p, q, r, rnorm);
        if (timeron) {
            timer_stop(T_conj_grad);
        }
        norm_temp1 = 0.0;
        norm_temp2 = 0.0;
        for (j = 1; j <= lastcol - firstcol + 1; j++) {
            norm_temp1 = norm_temp1 + x[j] * z[j];
            norm_temp2 = norm_temp2 + z[j] * z[j];
        }
        norm_temp2 = 1.0 / sqrt(norm_temp2);
        zeta = shift + 1.0 / norm_temp1;
        if (it == 1) {
            std::cout << std::endl;
            std::cout << "   iteration           ||r||                 zeta" << std::endl;
        }
        std::cout << "    " << std::setw(5) << it << "       " << std::scientific << std::setw(20) << rnorm << "  " << std::fixed << std::setw(20) << zeta << std::endl;
        for (j = 1; j <= lastcol - firstcol + 1; j++) {
            x[j] = norm_temp2 * z[j];
        }
    }
    timer_stop(T_bench);
    t = timer_read(T_bench);
    std::cout << " Benchmark completed " << std::endl;
    epsilon = 1.0e-10;
    if (class != 'U') {
        err = std::abs(zeta - zeta_verify_value) / zeta_verify_value;
        if (err <= epsilon) {
            verified = true;
            std::cout << " VERIFICATION SUCCESSFUL " << std::endl;
            std::cout << " Zeta is    " << std::scientific << std::setw(20) << zeta << std::endl;
            std::cout << " Error is   " << std::scientific << std::setw(20) << err << std::endl;
        } else {
            verified = false;
            std::cout << " VERIFICATION FAILED" << std::endl;
            std::cout << " Zeta                " << std::scientific << std::setw(20) << zeta << std::endl;
            std::cout << " The correct zeta is " << std::scientific << std::setw(20) << zeta_verify_value << std::endl;
        }
    } else {
        verified = false;
        std::cout << " Problem size unknown" << std::endl;
        std::cout << " NO VERIFICATION PERFORMED" << std::endl;
    }
    if (t != 0.0) {
        mflops = (2 * niter * na) * (3.0 + (nonzer * (nonzer + 1)) + 25.0 * (5.0 + (nonzer * (nonzer + 1))) + 3.0) / t / 1000000.0;
    } else {
        mflops = 0.0;
    }
    print_results("CG", class, na, 0, 0, niter, t, mflops, "          floating point", verified, npbversion, compiletime, cs1, cs2, cs3, cs4, cs5, cs6, cs7);
    if (!timeron) {
        goto L999;
    }
    tmax = timer_read(T_bench);
    if (tmax == 0.0) {
        tmax = 1.0;
    }
    std::cout << "  SECTION   Time (secs)" << std::endl;
    for (i = 1; i <= T_last; i++) {
        t = timer_read(i);
        if (i == t_init) {
            std::cout << "  " << std::setw(8) << t_names[i] << ":" << std::setw(9) << t << "  (" << std::setw(6) << t * 100.0 / tmax << "%)" << std::endl;
        } else {
            std::cout << "  " << std::setw(8) << t_names[i] << ":" << std::setw(9) << t << "  (" << std::setw(6) << t * 100.0 / tmax << "%)" << std::endl;
            if (i == t_conj_grad) {
                t = tmax - t;
                std::cout << "    --> total " << std::setw(8) << t_names[i] << ":" << std::setw(9) << t << "  (" << std::setw(6) << t * 100.0 / tmax << "%)" << std::endl;
            }
        }
    }
L999:
    return 0;
}

void makea(int n, int nonzer, int* arow, int** acol, double** aelt, double* a, int* colidx, int* rowstr, double rcond, double shift) {
    int nn1 = 1;
    while (nn1 < n) {
        nn1 *= 2;
    }

    int* ivc = new int[nonzer + 1];
    double* vc = new double[nonzer + 1];

    for (int iouter = 0; iouter < n; iouter++) {
        int nzv = nonzer;
        sprnvc(n, nzv, nn1, vc, ivc);
        vecset(n, vc, ivc, nzv, iouter, 0.5);
        arow[iouter] = nzv;
        for (int ivelt = 0; ivelt < nzv; ivelt++) {
            acol[ivelt][iouter] = ivc[ivelt];
            aelt[ivelt][iouter] = vc[ivelt];
        }
    }

    sparse(a, colidx, rowstr, n, nonzer, nonzer, arow, acol, aelt, 0, n - 1, nullptr, rcond, shift);

    delete[] ivc;
    delete[] vc;
}

void sparse(double* a, int* colidx, int* rowstr, int n, int nz, int nonzer, int* arow, int** acol, double** aelt, int firstrow, int lastrow, int* nzloc, double rcond, double shift) {
    int nrows = lastrow - firstrow + 1;

    int* mark = new int[nrows + 1];
    for (int j = 0; j <= nrows; j++) {
        rowstr[j] = 0;
    }

    for (int i = 0; i < n; i++) {
        for (int nza = 0; nza < arow[i]; nza++) {
            int j = acol[nza][i] + 1;
            rowstr[j] += arow[i];
        }
    }

    rowstr[0] = 1;
    for (int j = 1; j <= nrows; j++) {
        rowstr[j] += rowstr[j - 1];
    }

    int nza = rowstr[nrows + 1] - 1;
    if (nza > nz) {
        std::cout << "Space for matrix elements exceeded in sparse" << std::endl;
        std::cout << "nza, nzmax = " << nza << ", " << nz << std::endl;
        return;
    }

    for (int j = 0; j < nrows; j++) {
        for (int k = rowstr[j]; k < rowstr[j + 1]; k++) {
            a[k] = 0.0;
            colidx[k] = 0;
        }
        if (nzloc != nullptr) {
            nzloc[j] = 0;
        }
    }

    double size = 1.0;
    double ratio = pow(rcond, 1.0 / static_cast<double>(n));

    for (int i = 0; i < n; i++) {
        for (int nza = 0; nza < arow[i]; nza++) {
            int j = acol[nza][i];
            double scale = size * aelt[nza][i];
            for (int nzrow = 0; nzrow < arow[i]; nzrow++) {
                int jcol = acol[nzrow][i];
                double va = aelt[nzrow][i] * scale;
                if (jcol == j && j == i) {
                    va += rcond - shift;
                }
                for (int k = rowstr[j]; k < rowstr[j + 1]; k++) {
                    if (colidx[k] > jcol) {
                        for (int kk = rowstr[j + 1] - 2; kk >= k; kk--) {
                            if (colidx[kk] > 0) {
                                a[kk + 1] = a[kk];
                                colidx[kk + 1] = colidx[kk];
                            }
                        }
                        colidx[k] = jcol;
                        a[k] = 0.0;
                        goto label40;
                    } else if (colidx[k] == 0) {
                        colidx[k] = jcol;
                        goto label40;
                    } else if (colidx[k] == jcol) {
                        if (nzloc != nullptr) {
                            nzloc[j] += 1;
                        }
                        goto label40;
                    }
                }
                std::cout << "internal error in sparse: i=" << i << std::endl;
                return;
            label40:
                a[k] += va;
            }
        label60:
            continue;
        }
    }

    for (int j = 1; j <= nrows; j++) {
        if (nzloc != nullptr) {
            nzloc[j] += nzloc[j - 1];
        }
    }

    for (int j = 0; j < nrows; j++) {
        int j1 = (j > 0) ? rowstr[j] - nzloc[j - 1] : 1;
        int j2 = rowstr[j + 1] - nzloc[j] - 1;
        nza = rowstr[j];
        for (int k = j1; k <= j2; k++) {
            a[k] = a[nza];
            colidx[k] = colidx[nza];
            nza++;
        }
    }

    for (int j = 1; j <= nrows + 1; j++) {
        rowstr[j] -= nzloc[j - 1];
    }

    nza = rowstr[nrows + 1] - 1;

    delete[] mark;
}

void sprnvc(int n, int nz, int nn1, double* v, int* iv) {
    double amult = 1220703125.0;
    double tran = 0.309;
    int nzv = 0;

    while (nzv < nz) {
        double vecelt = tran * amult;
        double vecloc = tran * amult;
        int i = icnvrt(vecloc, nn1) + 1;
        if (i > n) {
            continue;
        }
        bool duplicate = false;
        for (int ii = 0; ii < nzv; ii++) {
            if (iv[ii] == i) {
                duplicate = true;
                break;
            }
        }
        if (!duplicate) {
            nzv++;
            v[nzv - 1] = vecelt;
            iv[nzv - 1] = i;
        }
    }
}

int icnvrt(double x, int ipwr2) {
    return static_cast<int>(ipwr2 * x);
}

void vecset(int n, double* v, int* iv, int nzv, int i, double val) {
    bool set = false;
    for (int k = 0; k < nzv; k++) {
        if (iv[k] == i) {
            v[k] = val;
            set = true;
            break;
        }
    }
    if (!set) {
        nzv++;
        v[nzv - 1] = val;
        iv[nzv - 1] = i;
    }
}

int main() {
    int n = 10;
    int nonzer = 5;
    int* arow = new int[n];
    int** acol = new int*[nonzer + 1];
    double** aelt = new double*[nonzer + 1];
    for (int i = 0; i < nonzer + 1; i++) {
        acol[i] = new int[n];
        aelt[i] = new double[n];
    }
    double* a = new double[n];
    int* colidx = new int[n];
    int* rowstr = new int[n + 1];
    double rcond = 0.5;
    double shift = 0.0;

    makea(n, nonzer, arow, acol, aelt, a, colidx, rowstr, rcond, shift);

    // Print the resulting arrays
    std::cout << "arow: ";
    for (int i = 0; i < n; i++) {
        std::cout << arow[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "acol: " << std::endl;
    for (int i = 0; i < nonzer + 1; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << acol[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "aelt: " << std::endl;
    for (int i = 0; i < nonzer + 1; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << aelt[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "a: ";
    for (int i = 0; i < n; i++) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "colidx: ";
    for (int i = 0; i < n; i++) {
        std::cout << colidx[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "rowstr: ";
    for (int i = 0; i < n + 1; i++) {
        std::cout << rowstr[i] << " ";
    }
    std::cout << std::endl;

    delete[] arow;
    for (int i = 0; i < nonzer + 1; i++) {
        delete[] acol[i];
        delete[] aelt[i];
    }
    delete[] acol;
    delete[] aelt;
    delete[] a;
    delete[] colidx;
    delete[] rowstr;

    return 0;
}


