/*

main.cpp
    LTspice 24.1.6
    LTspice_M ver. 0.0.4

*/


#include <iostream>
#include "LTspice_M.hpp"

std::wstring str =
LR"(Version 4.1
SHEET 1 880 680
WIRE 176 80 -144 80
WIRE 288 80 256 80
WIRE -144 128 -144 80
WIRE 288 144 288 80
WIRE -144 256 -144 208
WIRE 288 256 288 208
WIRE 288 256 -144 256
SYMBOL voltage -144 112 R0
WINDOW 123 0 0 Left 0
WINDOW 39 0 0 Left 0
SYMATTR InstName V1
SYMATTR Value PULSE(0 1 1m 1n 1n 1m 3m)
SYMBOL res 272 64 R90
WINDOW 0 0 56 VBottom 2
WINDOW 3 32 56 VTop 2
SYMATTR InstName R1
SYMATTR Value {R}
SYMBOL cap 304 208 R180
WINDOW 0 24 56 Left 2
WINDOW 3 24 8 Left 2
SYMATTR InstName C1
SYMATTR Value 100p
TEXT 304 248 Left 2 !.step dec param R 1Meg 100Meg 10
TEXT 304 224 Left 2 !.tran 10m
)";

int main() {
    std::cout << "wait..." << std::endl;
    LTspice_M obj("test", R"(C:\Users\...\LTspice\)", R"(C:\Users\...\test\)");
    obj.makeFile(".asc", str);
    obj.makeNet(".asc");
    obj.makeRaw(".net");
    obj.rawToCsv(".raw", ".csv");

    int w = 320;
    int h = 160;
    dib_M dib;
    graph_M graph(w, h);

    std::vector<double> input;
    std::vector<float> vn2;

    raw_M raw("test.raw");
    raw.readData();
    for (size_t i = 0; i < raw.pNum; i++) {
        for (size_t j = 0; j < raw.vNum; j++) {
            size_t ix = (raw.vNum + 1) * i + j;
            if (j == 0) {
                double* s = (double*)&raw.data[ix];
                input.push_back(s[0]);
            }
            else {
                if (raw.para[j] == L"V(n002)") vn2.push_back(raw.data[ix + 1]);
            }
        }
    }

    std::vector<uint32_t> color;
    for (int i = 0; i < 40; i++) color.push_back(0xff0000ff + (i * 10 << 16));
    int color_index = -1;

    int x1, y1;
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == 0) {
            x1 = (input[i] / 0.01) * w;
            y1 = (vn2[i] / 1.1) * h;
            color_index++;
        }
        else {
            int x2 = (input[i] / 0.01) * w;
            int y2 = (vn2[i] / 1.1) * h;
            graph.line(x1, y1, x2, y2, color[color_index]);
            x1 = x2; y1 = y2;
        }
    }


    dib.create("test.bmp", graph.data.data(), w, h);
    return 0;
}

