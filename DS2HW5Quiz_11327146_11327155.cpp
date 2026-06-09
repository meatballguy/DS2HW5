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
#include <map>

using namespace std;

struct Record {
    char putID[10];
    char getID[10];
    float weight; 
};

class Program {
public:
    int run() {
        int bufferSize = 300;   
        string fileNo;
        while (true) {
            cout << "\n* Data Structures and Algorithms *" << endl;
            cout << "**********************************" << endl;
            cout << "* 1. External merge sort on file *" << endl;
            cout << "* 2: Construct the primary index *" << endl;
            cout << "* 3: Range search to build index *" << endl;
            cout << "* 4: Get maximum spanning forest *" << endl;
            cout << "**********************************" << endl;
            cout << "*** The buffer size is " << bufferSize << endl;
            while (true) {
                        cout << "Input a new buffer size in [300, 60000]: ";
                        if (cin >> bufferSize) {
                            if (bufferSize >= 300 && bufferSize <= 60000) break;
                            cout << "\n### It is NOT in [300,60000] ###\n";
                        } else {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                        cout << "\n";
                    }
            if (bufferSize < 300) bufferSize = 300;
            if (bufferSize > 60000) bufferSize = 60000;

            cout << "\n##################################" << endl;
            cout << "* 1. External merge sort on file *" << endl;
            cout << "##################################" << endl;
            fileNo = readValidFileNo();

            if (fileNo != "0") {
                executeTask1(fileNo, bufferSize);

                cout << "\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                cout << "* 2: Construct the primary index *" << endl;
                cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
                vector<pair<float, long long>> primaryIndex = executeTask2(fileNo);

                while (true) {
                    cout << "\n##################################" << endl;
                    cout << "* 3: Range search to build index *" << endl;
                    cout << "##################################" << endl;
                    
                    cout << "\nInput two values in (0,1] for range search." << endl;
                    // output "\n### It is NOT in [0.01,1.00] ###\n" if the input is float but not in [0.01, 1.00],
                    // if the input is not a float, it will just ignore the input and ask again, until the input is valid
                    // keep asking until the input is valid
                    float val1, val2;
                    while (true) {
                        cout << "\nInput a floating number in [0.01, 1]: ";
                        if (cin >> val1) {
                            if (val1 >= 0.01f && val1 <= 1.0f) break;
                            cout << "\n### It is NOT in [0.01,1] ###\n";
                        } else {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                    }
                    while (true) {
                        cout << "\nInput a floating number in [0.01, 1]: ";
                        if (cin >> val2) {
                            if (val2 >= 0.01f && val2 <= 1.0f) break;
                            cout << "\n### It is NOT in [0.01,1] ###\n";
                        } else {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                    }

                    float high = max(val1, val2);
                    float low = min(val1, val2);

                    executeTask3And4(fileNo, bufferSize, primaryIndex, low, high);

                    cout << "\n[3]Quit or [Any other key]continue?" << endl;
                    string cont3;
                    cin >> cont3;
                    if (cont3 == "3") break;
                }
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

    static void executeTask1(const string& fileNo, int bufferSize) {
        string inputFileName = "pairs" + fileNo + ".bin";
        string outputFileName = "order" + fileNo + ".bin";
        const int BUFFER_MAX = bufferSize;

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
            cout << "\nInput the file name: [0]Quit\n";
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

    static vector<pair<float, long long>> executeTask2(const string& fileNo) {
        vector<pair<float, long long>> index; // (weight, record index)
        string orderFile = "order" + fileNo + ".bin";
        ifstream in(orderFile, ios::binary);
        if (!in) {
            cout << "Cannot open " << orderFile << endl;
            return index;
        }

        cout << "\n<Primary index>: (key, offset)" << endl;

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
        return index;
    }

    static void executeTask3And4(const string& fileNo, int bufferSize, const vector<pair<float, long long>>& primaryIndex, float low, float high) {
        long long startOffset = -1;
        long long endOffset = -1;
        
        long long totalRecords = 0;
        if (!primaryIndex.empty()) {
            string orderFile = "order" + fileNo + ".bin";
            ifstream countFile(orderFile, ios::binary | ios::ate);
            totalRecords = countFile.tellg() / sizeof(Record);
            countFile.close();
        }

        bool found = false;
        long long end_idx = totalRecords;
        for (size_t i = 0; i < primaryIndex.size(); ++i) {
            float w = primaryIndex[i].first;
            if (w <= high && w >= low) {
                if (!found) {
                    startOffset = primaryIndex[i].second;
                    found = true;
                }
                if (i + 1 < primaryIndex.size()) {
                    end_idx = primaryIndex[i + 1].second;
                } else {
                    end_idx = totalRecords;
                }
            } else if (w < low) {
                break;
            }
        }

        if (!found) {
            cout << "\nThere are 0 records in total." << endl;
            cout << "There are 0 senders in total." << endl;
            return;
        }

        endOffset = end_idx;
        long long numRecordsInRange = endOffset - startOffset;

        string orderFile = "order" + fileNo + ".bin";
        ifstream in(orderFile, ios::binary);
        if (!in) return;

        in.seekg(startOffset * sizeof(Record), ios::beg);

        map<string, vector<long long>> secondaryIndex;
        vector<Record> buffer(bufferSize);
        long long recordsLeft = numRecordsInRange;
        long long currentOffset = startOffset;

        while (recordsLeft > 0) {
            long long toRead = min((long long)bufferSize, recordsLeft);
            in.read(reinterpret_cast<char*>(buffer.data()), toRead * sizeof(Record));
            for (long long i = 0; i < toRead; ++i) {
                string putID(buffer[i].putID);
                secondaryIndex[putID].push_back(currentOffset + i);
            }
            recordsLeft -= toRead;
            currentOffset += toRead;
        }

        cout << "\nThere are " << numRecordsInRange << " records in total." << endl;
        cout << "There are " << secondaryIndex.size() << " senders in total." << endl;

        int count = 1;
        for (auto const& [putID, offsets] : secondaryIndex) {
            cout << "[" << setw(4) << count++ << "]   " << setw(8) << putID << "\t" << setw(5) << offsets.size() << endl;
        }

        while (true) {
            cout << "\nInput a student ID ([4] Quit): ";
            string sid;
            cin >> sid;
            if (sid == "4") break;

            if (secondaryIndex.find(sid) == secondaryIndex.end()) {
                cout << "Sender " << sid << " does not exist." << endl;
            } else {
                const vector<long long>& offsets = secondaryIndex[sid];
                cout << "Sender " << sid << " has " << offsets.size() << " records." << endl;
                for (size_t i = 0; i < offsets.size(); ++i) {
                    in.seekg(offsets[i] * sizeof(Record), ios::beg);
                    Record r;
                    in.read(reinterpret_cast<char*>(&r), sizeof(Record));
                    cout << "[" << setw(3) << (i + 1) << "]   " << setw(8) << r.getID << "        " << fixed << setprecision(2) << r.weight << endl;
                }
            }
        }
        in.close();
    }
};

int main() {
    Program program;
    return program.run();
}
