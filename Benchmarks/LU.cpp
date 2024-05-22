
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>

using namespace std;


bool timeron;
double trecs[t_last];

//---------------------------------------------------------------------
//     Main function
//---------------------------------------------------------------------
int main()
{
    char class;
    bool verified;
    double mflops;
    double t, tmax;
    double timer_read();
    int i, fstatus;
    char t_names[t_last][8];

    //---------------------------------------------------------------------
    //     Read input data
    //---------------------------------------------------------------------
    read_input();

    //---------------------------------------------------------------------
    //     Set up domain sizes
    //---------------------------------------------------------------------
    domain();

    //---------------------------------------------------------------------
    //     Set up coefficients
    //---------------------------------------------------------------------
    setcoeff();

    //---------------------------------------------------------------------
    //     Set the boundary values for dependent variables
    //---------------------------------------------------------------------
    setbv();

    //---------------------------------------------------------------------
    //     Set the initial values for dependent variables
    //---------------------------------------------------------------------
    setiv();

    //---------------------------------------------------------------------
    //     Compute the forcing term based on prescribed exact solution
    //---------------------------------------------------------------------
    erhs();

    //---------------------------------------------------------------------
    //     Perform one SSOR iteration to touch all pages
    //---------------------------------------------------------------------
    ssor(1);

    //---------------------------------------------------------------------
    //     Reset the boundary and initial values
    //---------------------------------------------------------------------
    setbv();
    setiv();

    //---------------------------------------------------------------------
    //     Perform the SSOR iterations
    //---------------------------------------------------------------------
    ssor(itmax);

    //---------------------------------------------------------------------
    //     Compute the solution error
    //---------------------------------------------------------------------
    error();

    //---------------------------------------------------------------------
    //     Compute the surface integral
    //---------------------------------------------------------------------
    pintgr();

    //---------------------------------------------------------------------
    //     Verification test
    //---------------------------------------------------------------------
    verify(rsdnm, errnm, frc, class, verified);
    mflops = float(itmax) * (1984.77 * float(nx0) * float(ny0) * float(nz0) - 10923.3 * (float(nx0 + ny0 + nz0) / 3.) * (float(nx0 + ny0 + nz0) / 3.) + 27770.9 * float(nx0 + ny0 + nz0) / 3. - 144010.) / (maxtime * 1000000.);
    print_results("LU", class, nx0, ny0, nz0, itmax, maxtime, mflops, "          floating point", verified, npbversion, compiletime, cs1, cs2, cs3, cs4, cs5, cs6, "(none)");

    //---------------------------------------------------------------------
    //     More timers
    //---------------------------------------------------------------------
    if (!timeron)
        goto L999;
    for (i = 0; i < t_last; i++)
    {
        trecs[i] = timer_read(i);
    }
    tmax = maxtime;
    if (tmax == 0.)
        tmax = 1.0;
    cout << "  SECTION     Time (secs)" << endl;
    for (i = 0; i < t_last; i++)
    {
        cout << "  " << t_names[i] << ": " << trecs[i] << "  (" << trecs[i] * 100. / tmax << "%)" << endl;
        if (i == t_rhs)
        {
            t = trecs[t_rhsx] + trecs[t_rhsy] + trecs[t_rhsz];
            cout << "     --> total rhs: " << t << "  (" << t * 100. / tmax << "%)" << endl;
            t = trecs[i] - t;
            cout << "     --> rest rhs: " << t << "  (" << t * 100. / tmax << "%)" << endl;
        }
    }
L999:
    return 0;
}


