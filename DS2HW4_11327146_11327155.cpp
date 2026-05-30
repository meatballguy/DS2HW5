/*11327146 莊立聖 11327155 黃宇謙*/
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <cstdio>

using namespace std;

struct Record {
    char putID[10];
    char getID[10];
    float weight; 
};

bool compareRecords(const Record& a, const Record& b) {
    return a.weight > b.weight;
}

// merges two smaller sorted segmented 300 records file to a 600 sized file
void mergeTwoRuns(const string& fileA, const string& fileB, const string& fileOut) {
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

void executeTask1(const string& fileNo) {
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
            stable_sort(internalBuffer.begin(), internalBuffer.end(), compareRecords);

            string runName = "temp_run_" + fileNo + "_" + to_string(runCounter++) + ".bin";
            currentRuns.push_back(runName);
            ofstream runFile(runName, ios::binary);
            runFile.write(reinterpret_cast<char*>(internalBuffer.data()), internalBuffer.size() * sizeof(Record));
            runFile.close();

            internalBuffer.clear();
        }
    }

    if (!internalBuffer.empty()) {
        stable_sort(internalBuffer.begin(), internalBuffer.end(), compareRecords);
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

    cout << "\nThe internal sort is completed. Check the initial sorted runs!\n" << endl;

    auto startExternal = chrono::high_resolution_clock::now();

    int passCounter = 0;
    // for merging the temp files
    while (currentRuns.size() > 1) {
        cout << "Now there are " << currentRuns.size() << " runs." << endl;

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

int main() {
    string fileNo;
    while (true) {
        cout << "* Data Structures and Algorithms *" << endl;
        cout << "**********************************" << endl;
        cout << "* 1. External merge sort on file *" << endl;
        cout << "* 2: Construct the primary index *" << endl;
        cout << "**********************************" << endl;
        cout << "*** The buffer size is 300" << endl;
        cout << "##################################" << endl;
        cout << "Mission 1: External merge sort " << endl;
        cout << "##################################" << endl;
        cout << "\nInput the file name: [0]Quit" << endl;
        cin >> fileNo;

        if (fileNo == "0") {
            break;
        }

        executeTask1(fileNo);

        cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        cout << "Mission 2: Build the primary index " << endl;
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    }
    return 0;
}