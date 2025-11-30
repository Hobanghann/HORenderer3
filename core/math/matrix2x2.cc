#include "matrix2x2.h"

#include <string>
#include <vector>

namespace ho {
    std::string Matrix2x2::ToString() const {
        char res[200];
        std::snprintf(res, sizeof(res), "| %.3f , %.3f |\n| %.3f , %.3f |", (float)row0.x, (float)row0.y, (float)row1.x,
                      (float)row1.y);

        return res;
    }
}  // namespace ho