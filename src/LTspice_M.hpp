/*

LTspice_M.hpp
    LTspice_M ver. 0.0.4

    ** Dependency **
        - LTspice 24.1.6

    ** Class **
    dib_M
        - create .bmp file
    raw_M
        - read .raw file
        - output .csv file
    graph_M
        - create image of .raw data
    LTspice_M
        - create txt(wsring) file
        - .asc -> .net
        - .net -> .raw

    ** Vertion **
    2025-04-29 ver. 0.0.0
        - .raw(.tran)に対応。
    2025-04-30 ver. 0.0.1
        - .rawファイルの扱いをraw_Mクラスに。
        - .raw(.dc)に対応。
        - .raw(.ac)に対応。
    2025-05-02 ver. 0.0.2
        - ファイル書き込みをストリームではなくfs.write()に変更。
        - 出力.csvファイルをutf-8に変更。
        - 拡張子の自動付与を停止。
        - グラフ描画の補助機能と.bmp出力機能を追加。
    2025-05-03 ver. 0.0.3
        - LTspice_M.makeFile()でのファイル出力先を他メソッドと同じ場所に。
    2025-05-03 ver. 0.0.4
        - コンパイラの構造体最適化を停止するマクロを追加。

    ** License **
        - WTFPL

    ** Developer **
        - Ryoichi Tsukamoto

*/

#pragma
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <utility>

class dib_M {

#pragma pack(push, 1)
    struct BITMAPFILEHEADER {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    };

    struct CIEXYZ {
        int32_t ciexyzX;
        int32_t ciexyzY;
        int32_t ciexyzZ;
    };

    struct CIEXYZTRIPLE {
        CIEXYZ ciexyzRed;
        CIEXYZ ciexyzGreen;
        CIEXYZ ciexyzBlue;
    };

    struct BITMAPV4HEADER {
        uint32_t bV4Size;
        int32_t bV4Width;
        int32_t bV4Height;
        uint16_t bV4Planes;
        uint16_t bV4BitCount;
        uint32_t bV4V4Compression;
        uint32_t bV4SizeImage;
        int32_t bV4XPelsPerMeter;
        int32_t bV4YPelsPerMeter;
        uint32_t bV4ClrUsed;
        uint32_t bV4ClrImportant;
        uint32_t bV4RedMask;
        uint32_t bV4GreenMask;
        uint32_t bV4BlueMask;
        uint32_t bV4AlphaMask;
        uint32_t bV4CSType;
        CIEXYZTRIPLE bV4Endpoints;
        uint32_t bV4GammaRed;
        uint32_t bV4GammaGreen;
        uint32_t bV4GammaBlue;
    };
#pragma pack(pop)

public:

    bool create(std::string path, const void* data, long w, long h) {
        BITMAPFILEHEADER bmfh = {0};
        BITMAPV4HEADER bmvh = {0};

        uint32_t datasize = w * h * 4;
        bmfh.bfType = 'B' + 'M' * 256;
        bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV4HEADER) + datasize;
        bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPV4HEADER);
        bmvh.bV4Size = sizeof(BITMAPV4HEADER);
        bmvh.bV4Width = w;
        bmvh.bV4Height = -h;
        bmvh.bV4Planes = 1;
        bmvh.bV4BitCount = 32;
        //bmvh.bV4V4Compression = BI_BITFIELDS;
        //bmvh.bV4SizeImage = w * h * 4;
        bmvh.bV4XPelsPerMeter = 0xff;
        bmvh.bV4YPelsPerMeter = 0xff;
        //bmvh.bV4RedMask = 0x000000FF;
        //bmvh.bV4GreenMask = 0x0000FF00;
        //bmvh.bV4BlueMask = 0x00FF0000;
        bmvh.bV4AlphaMask = 0xff000000;
        bmvh.bV4CSType = 'sRGB';

        std::ofstream fs(path, std::ios::binary | std::ios::out);
        fs.write((const char*)&bmfh, sizeof(bmfh));
        fs.write((const char*)&bmvh, sizeof(bmvh));
        fs.write((const char*)data, datasize);
        fs.close();

        return true;
    }
};

class graph_M {
public:

    std::vector<uint32_t> data;
    long w, h;

    graph_M(long widthSize, long heightSize) {
        w = widthSize;
        h = heightSize;
        data.resize(w * h);
        std::fill(data.begin(), data.end(), 0xff000000);
    }

    void line(int x1, int y1, int x2, int y2, uint32_t color) {

        //例外
        //ケース1：x1 > x2→(x1,y1)と(x2,y2)をswap
        //ケース2：勾配 < 0→(x1,-y1)と(x2,-y2)で計算して、(x,-y)描画
        //ケース3：勾配 > 1→(y1,x1)と(y2,x2)で計算して、(y,x)で描画
        bool r2 = false;//例外の2
        bool r3 = false;//例外の3
        if (x1 != x2) {
            float k = (float)(y2 - y1) / (x2 - x1);//傾き
            if (k < 0) {
                y1 *= -1;
                y2 *= -1;
                k *= -1;
                r2 = true;
            }
            if (k > 1) {
                std::swap(x1, y1);
                std::swap(x2, y2);
                r3 = true;
            }
        }
        else {
            std::swap(x1, y1);
            std::swap(x2, y2);
            r3 = true;
        }
        if (x1 > x2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        int dx = x2 - x1;
        int dy = y2 - y1;
        int d = 2 * dy - dx;
        int E = 2 * dy;
        int NE = 2 * (dy - dx);

        int x = x1, y = y1;
        while (1) {
            if (x == x2) break;

            if (d <= 0) {
                x++;
                d += E;
            }
            else if (d > 0) {
                x++;
                y++;
                d += NE;
            }

            //例外処理をして描画
            int px = x;
            int py = y;
            if (r3 == true)
                std::swap(px, py);
            if (r2 == true)
                py *= -1;
            if (px > 0 && px < w && py > 0 && py < h) {
                int ix = (h - py) * w + px;
                data[ix] = color;
            }
        }
    }
};

class raw_M {

    size_t find(std::wstring key, size_t offset = 0) {
        if (errorLog != "") return 0;
        size_t pos = raw.find(key, offset);
        if (pos == std::string::npos) {
            errorLog += "error : not find keyword\n";
            return 0;
        }
        return pos;
    }

    size_t getNum(std::wstring key) {
        if (errorLog != "") return 0;
        size_t sp = find(key) + key.size();
        size_t fp = find(L"\n", sp);
        std::wstring str = raw.substr(sp, fp - sp);
        return std::stoull(str);
    }


    bool writeFile(std::ostream &fs, std::wstring str) {
        if (errorLog != "") return false;
        fs.write((const char*)str.c_str(), str.size() * sizeof(wchar_t));
        return true;
    }

public:

    std::string errorLog = "";
    std::ifstream fs;
    std::wstring raw;
    std::vector<std::wstring> para;
    std::vector<float> data;

    size_t vNum = 0;
    size_t pNum = 0;
    size_t timeCh = 0;

    raw_M(std::string fileName) {
        fs.open(fileName, std::ios::binary | std::ios::in);
        if (!fs) {
            errorLog += "error : file open\n";
        }
        else {
            fs.seekg(0, std::ios::end);
            size_t size = fs.tellg();

            raw.resize(size / sizeof(wchar_t) + 1);
            fs.seekg(0);
            fs.read((char*)&raw[0], size);
        }
    }

    bool readData() {
        if (errorLog != "") return false;

        vNum = getNum(L"No. Variables:");
        pNum = getNum(L"No. Points:");

        std::wstring key = L"Variables:\n";
        size_t sp = find(key);
        sp = sp + key.size();

        key = L"Binary:\n";
        size_t pos = find(key);


        std::wstring str = raw.substr(sp, pos - sp);

        std::wsmatch m, n;
        for (size_t i = 0; i < vNum; i++) {
            if (std::regex_search(str, m, std::wregex(LR"(.+\n)"))) {
                std::wstring t = m[0].str();

                size_t s = 0;
                while (t[s] == L'\t') { s++; }
                while (t[s] != L'\t') { s++; }
                while (t[s] == L'\t') { s++; }
                size_t f = s + 1;
                while (t[f] != L'\t') { f++; }
                para.push_back(t.substr(s, f - s));

                str = str.substr(t.size(), str.size() - t.size());
            }
            else {
                errorLog += "error : read Variables\n";
                return false;
            }
        }

        pos = pos + key.size();
        pos *= sizeof(wchar_t);

        if (para[0] == L"frequency") data.resize(vNum * pNum * 4);
        else data.resize((vNum + 1) * pNum);
        fs.seekg(pos);
        fs.read((char*)&data[0], data.size() * sizeof(float));
        return true;
    }

    bool outputCsv(std::string fileName) {
        if (errorLog != "") return false;
        std::ofstream ofs(fileName, std::ios::binary | std::ios::out);

        auto writeNum = [&](std::ostream &f, double n) {
            std::wstringstream ss;
            ss << n;
            writeFile(f, (ss.str() + L", "));
            };

        if (para[0] == L"frequency") {
            for (size_t i = 0; i < vNum; i++) {
                writeFile(ofs, (para[i] + L"_Re, "));
                writeFile(ofs, (para[i] + L"_Im, "));
            }
            writeFile(ofs, L"\n");
            size_t rawNum = vNum * 2;
            double* b = (double*)&data[0];
            for (size_t i = 0; i < pNum; i++) {
                for (size_t j = 0; j < rawNum; j++) {
                    size_t ix = rawNum * i + j;
                    writeNum(ofs, b[ix]);
                }
                writeFile(ofs, L"\n");
            }
        }
        else {
            for (size_t i = 0; i < vNum; i++) writeFile(ofs, (para[i] + L", "));
            writeFile(ofs, L"\n");
            for (size_t i = 0; i < pNum; i++) {
                for (size_t j = 0; j < vNum; j++) {
                    size_t ix = (vNum + 1) * i + j;
                    if (j == 0) {
                        double* s = (double*)&data[ix];
                        writeNum(ofs, s[0]);
                    }
                    else {
                        writeNum(ofs, data[ix + 1]);
                    }
                }
                writeFile(ofs, L"\n");
            }
        }

        ofs.close();
        return true;
    }

    ~raw_M() {
        fs.close();
    }
};

class LTspice_M {
public:

    std::string errorLog = "";
    std::string pName = "";
    std::string lPath = "";
    std::string pPath = "";

    LTspice_M(std::string projectName = "", std::string LTspicePath = "", std::string projectPath = "") {
        pName = projectName;
        lPath = LTspicePath;
        pPath = projectPath;
    }

    bool makeFile(std::string fileName, std::wstring str) {
        if (errorLog != "") return false;
        std::ofstream fs(pPath + pName + fileName, std::ios::binary | std::ios::out);
        fs.write((const char*)str.c_str(), str.size() * sizeof(wchar_t));
        fs.close();
        return true;
    }

    bool makeNet(std::string ascFileName) {
        if (errorLog != "") return false;
        std::string cmd = "cd " + lPath + " & " + "LTspice.exe -b -netlist \"" + pPath + pName + ascFileName + "\"";
        system(cmd.c_str());
        return true;
    }

    bool makeRaw(std::string netFileName) {
        if (errorLog != "") return false;
        std::string cmd = "cd " + lPath + " & " + "LTspice.exe -b \"" + pPath + pName + netFileName + "\"";
        system(cmd.c_str());
        return true;
    }

    bool rawToCsv(std::string rawFileName, std::string csvFileName) {
        if (errorLog != "") return false;
        raw_M obj(pPath + pName + rawFileName);
        obj.readData();
        obj.outputCsv(pPath + pName + csvFileName);
        if (obj.errorLog == "") return true;
        else return false;
    }
};

