/*11327146 莊立聖 11327155 黃宇謙*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <cstdio>
#include <sstream>
#include <limits>

using namespace std;

struct Record {
    char putID[10];
    char getID[10];
    float weight; 
};

class Program {
public:
    int run() {
        ios::sync_with_stdio(false);
        cin.tie(nullptr);

        string fileNo;
        while (true) {
            cout << "\n* Data Structures and Algorithms *" << endl;
            cout << "**********************************" << endl;
            cout << "* 1. External merge sort on file *" << endl;
            cout << "* 2: Construct the primary index *" << endl;
            cout << "**********************************" << endl;
            cout << "*** The buffer size is 300" << endl;
            cout << "##################################" << endl;
            cout << "Mission 1: External merge sort " << endl;
            cout << "##################################" << endl;
            fileNo = readValidFileNo();

            if (fileNo != "0") {
                executeTask1(fileNo);

                cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                cout << "Mission 2: Build the primary index " << endl;
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                executeTask2(fileNo);
            }

            cout << "\n[0]Quit or [Any other key]continue?" << endl;
            string cont;
            cin >> cont;
            if (cont == "0") break;
        }
        return 0;
    }

private:
    static bool compareRecords(const Record& a, const Record& b) {
        return a.weight > b.weight;
    }

    // merges two smaller sorted segmented 300 records file to a 600 sized file
    static void mergeTwoRuns(const string& fileA, const string& fileB, const string& fileOut) {
        ifstream inA(fileA, ios::binary);
        ifstream inB(fileB, ios::binary);
        ofstream out(fileOut, ios::binary);

        Record recA, recB;
        bool hasA = (bool)inA.read(reinterpret_cast<char*>(&recA), sizeof(Record));
        bool hasB = (bool)inB.read(reinterpret_cast<char*>(&recB), sizeof(Record));

        while (hasA && hasB) {
            if (recA.weight >= recB.weight) {
                out.write(reinterpret_cast<char*>(&recA), sizeof(Record));
                hasA = (bool)inA.read(reinterpret_cast<char*>(&recA), sizeof(Record));
            } else {
                out.write(reinterpret_cast<char*>(&recB), sizeof(Record));
                hasB = (bool)inB.read(reinterpret_cast<char*>(&recB), sizeof(Record));
            }
        }

        while (hasA) {
            out.write(reinterpret_cast<char*>(&recA), sizeof(Record));
            hasA = (bool)inA.read(reinterpret_cast<char*>(&recA), sizeof(Record));
        }
        while (hasB) {
            out.write(reinterpret_cast<char*>(&recB), sizeof(Record));
            hasB = (bool)inB.read(reinterpret_cast<char*>(&recB), sizeof(Record));
        }

        inA.close();
        inB.close();
        out.close();
    }

    static void executeTask1(const string& fileNo) {
        string inputFileName = "pairs" + fileNo + ".bin";
        string outputFileName = "order" + fileNo + ".bin";
        const int BUFFER_MAX = 300;

        auto startInternal = chrono::high_resolution_clock::now();

        ifstream inFile(inputFileName, ios::binary);
        if (!inFile) {
            return;
        }

        vector<string> currentRuns;
        vector<Record> internalBuffer;
        internalBuffer.reserve(BUFFER_MAX);

        Record tempRec;
        int runCounter = 0;

        while (inFile.read(reinterpret_cast<char*>(&tempRec), sizeof(Record))) {
            internalBuffer.push_back(tempRec);

            if (internalBuffer.size() == BUFFER_MAX) {
                stable_sort(internalBuffer.begin(), internalBuffer.end(), Program::compareRecords);

                string runName = "temp_run_" + fileNo + "_" + to_string(runCounter++) + ".bin";
                currentRuns.push_back(runName);
                ofstream runFile(runName, ios::binary);
                runFile.write(reinterpret_cast<char*>(internalBuffer.data()), internalBuffer.size() * sizeof(Record));
                runFile.close();

                internalBuffer.clear();
            }
        }

        if (!internalBuffer.empty()) {
            stable_sort(internalBuffer.begin(), internalBuffer.end(), Program::compareRecords);
            string runName = "temp_run_" + fileNo + "_" + to_string(runCounter++) + ".bin";
            currentRuns.push_back(runName);
            ofstream runFile(runName, ios::binary);
            runFile.write(reinterpret_cast<char*>(internalBuffer.data()), internalBuffer.size() * sizeof(Record));
            runFile.close();
            internalBuffer.clear();
        }
        inFile.close();

        // after this, we will have all the 300 sized segmented file, and 1 file maybe smaller than 300
        auto endInternal = chrono::high_resolution_clock::now();
        double timeInternal = chrono::duration_cast<chrono::microseconds>(endInternal - startInternal).count() / 1000.0;

        cout << "\nThe internal sort is completed. Check the initial sorted runs! \n" << endl;

        auto startExternal = chrono::high_resolution_clock::now();

        int passCounter = 0;
        // for merging the temp files
        while (currentRuns.size() > 1) {
            cout << "Now there are " << currentRuns.size() << " runs." << endl << endl;

            vector<string> nextRuns;
            for (size_t i = 0; i < currentRuns.size(); i += 2) {
                if (i + 1 < currentRuns.size()) {
                    string outRunName = "temp_merge_" + fileNo + "_" + to_string(passCounter) + "_" + to_string(i / 2) + ".bin";
                    mergeTwoRuns(currentRuns[i], currentRuns[i + 1], outRunName);
                    nextRuns.push_back(outRunName);

                    remove(currentRuns[i].c_str());
                    remove(currentRuns[i + 1].c_str());
                } else {
                    nextRuns.push_back(currentRuns[i]);
                }
            }
            currentRuns = nextRuns;
            passCounter++;
        }

        if (!currentRuns.empty()) {
            cout << "Now there are " << currentRuns.size() << " runs." << endl;
            remove(outputFileName.c_str());
            rename(currentRuns[0].c_str(), outputFileName.c_str());
        }

        auto endExternal = chrono::high_resolution_clock::now();
        double timeExternal = chrono::duration_cast<chrono::microseconds>(endExternal - startExternal).count() / 1000.0;

        cout << "\nThe execution time ..." << endl;
        cout << fixed << setprecision(3);
        cout << "Internal Sort = " << timeInternal << " ms" << endl;
        cout << "External Sort = " << timeExternal << " ms" << endl;
        cout << "Total Execution Time = " << (timeInternal + timeExternal) << " ms" << endl;
    }

    static string formatFloat(float v) {
        std::ostringstream oss;
        oss << fixed << setprecision(2) << v;
        string s = oss.str();
        // trim trailing zeros and optional trailing dot
        if (s.find('.') != string::npos) {
            while (!s.empty() && s.back() == '0') s.pop_back();
            if (!s.empty() && s.back() == '.') s.pop_back();
        }
        return s;
    }

    static string readValidFileNo() {
        string fileNo;
        while (true) {
            cout << "\nInput the file name: [0]Quit" << endl;
            cin >> fileNo;
            if (fileNo == "0") {
                return fileNo;
            }
            string inputFileName = "pairs" + fileNo + ".bin";
            ifstream testFile(inputFileName, ios::binary);
            if (testFile) {
                return fileNo;
            }
            cout << "\npairs" << fileNo << ".bin does not exist!!!" << endl;
        }
    }

    static void executeTask2(const string& fileNo) {
        string orderFile = "order" + fileNo + ".bin";
        ifstream in(orderFile, ios::binary);
        if (!in) {
            cout << "Cannot open " << orderFile << endl;
            return;
        }

        cout << "\n<Primary index>: (key, offset)" << endl;

        vector<pair<float, long long>> index; // (weight, record index)
        Record rec;
        bool first = true;
        float lastWeight = 0.0f;
        long long recordIndex = 0;

        while (true) {
            if (!in.read(reinterpret_cast<char*>(&rec), sizeof(Record))) break;
            float w = rec.weight;
            if (first || w != lastWeight) {
                index.emplace_back(w, recordIndex);
                lastWeight = w;
                first = false;
            }
            ++recordIndex;
        }

        for (size_t i = 0; i < index.size(); ++i) {
            cout << "[" << (i + 1) << "] (" << formatFloat(index[i].first) << ", " << index[i].second << ")" << endl;
        }
    }
};

int main() {
    Program program;
    return program.run();
}
