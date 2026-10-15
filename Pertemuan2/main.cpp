#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<rapidjson/document.h>

using namespace std;
using namespace rapidjson;

int main() {
    int A;
    cout << "Input A: ";
    cin >> A;

    string movie_path = "XL/motion_movie/" + to_string(A) + ".json";
    ifstream movie_file(movie_path);
    if (!movie_file.is_open()) {
        cerr << "Gagal membuka file movie!" << endl;
        return 1;
    }
    stringstream movie_buffer;
    movie_buffer << movie_file.rdbuf();
    
    Document movie_doc;
    movie_doc.Parse(movie_buffer.str().c_str());    
    
    int unit_id = movie_doc["motion_unit"][0]["id"].GetInt();

    string unit_path = "XL/motion_unit/" + to_string(unit_id) + ".json";
    ifstream unit_file(unit_path);
    if (!unit_file.is_open()) {
        cerr << "Gagal membuka file unit: " << endl;
        return 1;
    }
    stringstream unit_buffer;
    unit_buffer << unit_file.rdbuf();

    Document unit_doc;
    unit_doc.Parse(unit_buffer.str().c_str());

    cout << endl << "---------------------------" << endl;
    cout << "motion movie id    : " << A << endl;
    cout << "motion unit id     : " << unit_id << endl;
    cout << "nama               : " << unit_doc["name"].GetString() << endl;
    cout << "total frame        : " << unit_doc["total_frame"].GetInt() << endl;
    cout << "---------------------------";

    const Value& times = unit_doc["time"];
    const Value& motion_frames = unit_doc["motion_frame"];

    SizeType total_frames = times.Size();
    for (SizeType i = 0; i < total_frames; i++) {
        cout << "\nframe " << (i + 1) << ":" << endl;
        cout << "time : " << times[i].GetInt() << endl;
        cout << "motion frame : [";

        const Value& frame_values = motion_frames[i];
        for (SizeType j = 0; j < frame_values.Size(); j++) {
            cout << frame_values[j].GetInt();
            if (j < frame_values.Size() - 1) cout << ", ";
        }
    }
        
    cout << "]" << endl;

    cout << "---------------------------" << endl;

    return 0;
}