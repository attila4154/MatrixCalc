#include "CApplication.cpp"

int main() {
    CApplication app;
    try {
        app.Run();
    }
    catch (...) {
        return 1;
    }


    return 0;
}