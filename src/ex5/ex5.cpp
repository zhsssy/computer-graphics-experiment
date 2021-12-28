#include<iostream>
#include <vector>

using namespace std;

const int MAX_POINTS = 20;

// Returns x-value of point of intersection of two
float x_intersect(float x1, float y1, float x2, float y2,
                  float x3, float y3, float x4, float y4) {
    float num = (x1 * y2 - y1 * x2) * (x3 - x4) -
                (x1 - x2) * (x3 * y4 - y3 * x4);
    float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

// Returns y-value of point of intersection of
float y_intersect(float x1, float y1, float x2, float y2,
                  float x3, float y3, float x4, float y4) {
    float num = (x1 * y2 - y1 * x2) * (y3 - y4) -
                (y1 - y2) * (x3 * y4 - y3 * x4);
    float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    return num / den;
}

// This functions clips all the edges w.r.t one clip
// edge of clipping area
void clip(float poly_points[][2], int &poly_size,
          float x1, float y1, float x2, float y2) {
    float new_points[MAX_POINTS][2];
    int new_poly_size = 0;

    // (ix,iy),(kx,ky) are the co-ordinate values of
    // the points
    for (int i = 0; i < poly_size; i++) {
        // i and k form a line in polygon
        int k = (i + 1) % poly_size;
        float ix = poly_points[i][0], iy = poly_points[i][1];
        float kx = poly_points[k][0], ky = poly_points[k][1];

        // Calculating position of first point
        // w.r.t. clipper line
        float i_pos = (x2 - x1) * (iy - y1) - (y2 - y1) * (ix - x1);

        // Calculating position of second point
        // w.r.t. clipper line
        float k_pos = (x2 - x1) * (ky - y1) - (y2 - y1) * (kx - x1);

        // Case 1 : When both points are inside
        if (i_pos < 0 && k_pos < 0) {
            //Only second point is added
            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

            // Case 2: When only first point is outside
        else if (i_pos >= 0 && k_pos < 0) {
            // Point of intersection with edge
            // and the second point is added
            new_points[new_poly_size][0] = x_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;

            new_points[new_poly_size][0] = kx;
            new_points[new_poly_size][1] = ky;
            new_poly_size++;
        }

            // Case 3: When only second point is outside
        else if (i_pos < 0 && k_pos >= 0) {
            //Only point of intersection with edge is added
            new_points[new_poly_size][0] = x_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_points[new_poly_size][1] = y_intersect(x1,
                                                       y1, x2, y2, ix, iy, kx, ky);
            new_poly_size++;
        }
    }

    // Copying new points into original array
    // and changing the no. of vertices
    poly_size = new_poly_size;
    for (int i = 0; i < poly_size; i++) {
        poly_points[i][0] = new_points[i][0];
        poly_points[i][1] = new_points[i][1];
    }
}

// Implements Sutherland–Hodgman algorithm
vector<float> suthHodgClip(float poly_points[][2], int poly_size,
                           float clipper_points[][2], int clipper_size) {
    //i and k are two consecutive indexes
    for (int i = 0; i < clipper_size; i++) {
        int k = (i + 1) % clipper_size;

        // We pass the current array of vertices, it's size
        // and the end points of the selected clipper line
        clip(poly_points, poly_size, clipper_points[i][0],
             clipper_points[i][1], clipper_points[k][0],
             clipper_points[k][1]);
    }
    vector<float> ans;
    // Printing vertices of clipped polygon
    for (int i = 0; i < poly_size; i++) {
        ans.emplace_back(poly_points[i][0]),
                ans.emplace_back(poly_points[i][1]);
        cout << '(' << poly_points[i][0] <<
             ", " << poly_points[i][1] << ") ";
    }
    return ans;
}

void cover_vec_to_array(vector<float> &a, float *b) {
    int j = 0;
    for (auto i: a) {
        if ((j + 1) % 3 == 0)
            b[j++] = 0;
        b[j++] = i;
    }
    b[j] = 0;
};

//Driver code
int main() {
    // Defining polygon vertices in clockwise order
    int poly_size = 3;
    float poly_points[20][2] = {{0.3,  0.2},
                                {-0.2, 0.6},
                                {0.2,  0.1}};

    int clipper_size = 4;
    float clipper_points[][2] = {{-0.5, -0.5},
                                 {-0.5, 0.5},
                                 {0.5,  0.5},
                                 {0.5,  -0.5}};
    auto vec = suthHodgClip(poly_points, poly_size, clipper_points,
                            clipper_size);
    float x[1000];
    cover_vec_to_array(vec, x);
    cout << vec.size() << endl;
    for (int i = 0; i < 12; i++) {
        cout << x[i] << " ";
    }

    cout << endl;


    return 0;
}