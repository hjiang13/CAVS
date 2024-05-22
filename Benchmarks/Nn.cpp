#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <omp.h>

#define MAX_ARGS 10
#define REC_LENGTH 49   // size of a record in db
#define REC_WINDOW 10   // number of records to read at a time
#define LATITUDE_POS 28 // location of latitude coordinates in input record
#define OPEN 10000      // initial value of nearest neighbors

struct neighbor {
    std::string entry;
    double dist;
};

int main(int argc, char *argv[]) {
    std::ifstream flist, fp;
    int i = 0, j = 0, k = 0, rec_count = 0, done = 0;
    char sandbox[REC_LENGTH * REC_WINDOW];
    std::string dbname;
    std::vector<neighbor> neighbors;
    float target_lat, target_long, tmp_lat = 0, tmp_long = 0;

    if (argc < 5) {
        std::cerr << "Invalid set of arguments\n";
        exit(-1);
    }

    flist.open(argv[1]);
    if (!flist.is_open()) {
        std::cout << "error opening flist\n";
        exit(1);
    }

    k = std::atoi(argv[2]);
    target_lat = std::atof(argv[3]);
    target_long = std::atof(argv[4]);
    neighbors.resize(k);

    for (j = 0; j < k; j++) {
        neighbors[j].dist = OPEN;
    }

    if (!(flist >> dbname)) {
        std::cerr << "error reading filelist\n";
        exit(0);
    }

    fp.open(dbname);
    if (!fp.is_open()) {
        std::cout << "error opening flist\n";
        exit(1);
    }

    std::vector<float> z(REC_WINDOW);

    while (!done) {
        fp.read(sandbox, REC_LENGTH * REC_WINDOW);
        rec_count = fp.gcount() / REC_LENGTH;
        if (rec_count != REC_WINDOW) {
            if (!fp.bad()) {
                fp.close();
                if (flist.eof())
                    done = 1;
                else {
                    if (!(flist >> dbname)) {
                        std::cerr << "error reading filelist\n";
                        exit(0);
                    }
                    fp.open(dbname);
                    if (!fp.is_open()) {
                        std::cout << "error opening a db\n";
                        exit(1);
                    }
                }
            } else {
                perror("Error");
                exit(0);
            }
        }

#pragma omp parallel for shared(z, target_lat, target_long) private(i, tmp_lat, tmp_long)
        for (i = 0; i < rec_count; i++) {
            std::sscanf(sandbox + (i * REC_LENGTH + LATITUDE_POS - 1), "%f %f", &tmp_lat, &tmp_long);
            z[i] = std::sqrt(((tmp_lat - target_lat) * (tmp_lat - target_lat)) +
                             ((tmp_long - target_long) * (tmp_long - target_long)));
        }

#pragma omp barrier
        for (i = 0; i < rec_count; i++) {
            float max_dist = -1;
            int max_idx = 0;
            for (j = 0; j < k; j++) {
                if (neighbors[j].dist > max_dist) {
                    max_dist = neighbors[j].dist;
                    max_idx = j;
                }
            }
            if (z[i] < neighbors[max_idx].dist) {
                sandbox[(i + 1) * REC_LENGTH - 1] = '\0';
                neighbors[max_idx].entry = std::string(sandbox + i * REC_LENGTH);
                neighbors[max_idx].dist = z[i];
            }
        }
    }

    if (getenv("OUTPUT")) {
        std::ofstream out("output.txt");
        out << "The " << k << " nearest neighbors are:\n";
        for (j = k - 1; j >= 0; j--) {
            if (!(neighbors[j].dist == OPEN))
                out << neighbors[j].entry << " --> " << neighbors[j].dist << "\n";
        }
    }

    flist.close();
    return 0;
}


