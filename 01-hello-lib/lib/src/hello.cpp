#include <iostream>
#include <string_view>

bool greetings() {
    using namespace std;
    string_view output_phrase("Hello, ");
    string_view user = getenv("USER");

    if (user.empty())
        user = "User";

    cout << output_phrase << user << endl;
    return cout.good();
}
