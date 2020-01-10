#include <SDL_version.h>
#include <cstdlib>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const SDL_version& v) {
    os << static_cast<int>(v.major) << '.' << static_cast<int>(v.minor) << '.'
       << static_cast<int>(v.patch);
    return os;
}

int main() {

    SDL_version compile_time_v{ 0, 0, 0 };
    SDL_version link_time_v{ 0, 0, 0 };

    // compile time preprocessor directive,i.e. library we compiled against
    SDL_VERSION(&compile_time_v)
    // library we got linked with
    SDL_GetVersion(&link_time_v);

    std::cout << "compiled against: " << compile_time_v << std::endl;

    std::cout << "linked against: " << link_time_v << std::endl;

    return std::cout.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}