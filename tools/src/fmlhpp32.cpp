#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>
#include "fml32.h"

using namespace std;

map<string, int> field_types = { {"short",    FLD_SHORT},
                                {"long",     FLD_LONG},
                                {"char",     FLD_CHAR},
                                {"float",    FLD_FLOAT},
                                {"double",   FLD_DOUBLE},
                                {"string",   FLD_STRING},
                                {"carray",   FLD_CARRAY},
                                {"mbstring", FLD_MBSTRING},
                                {"ptr",      FLD_PTR},
                                {"fml32",    FLD_FML32},
                                {"view32",   FLD_VIEW32} };

struct field_def
{
    string name;
    int number = 0;
    FLDID32 id = 0;
    string type_str;
};

bool starts_with(string const& source, string const& pattern)
{
    if(source.size() < pattern.size())
    {
        return false;
    }
    return memcmp(source.data(), pattern.data(), pattern.size()) == 0;
}

vector<field_def> read_input(istream& is)
{
    vector<field_def> result;
    string line;
    int base = 0;
    while(getline(is, line))
    {
        if(line.empty() || line[0] == '#')
        {
            continue;
        }
        else if(starts_with(line, "*base"))
        {
            base = stoi(line.substr(5));
        }
        else
        {
            string name, type_str;
            int number = 0;
            stringstream s(line);
            s >> name >> number >> type_str;
            if(name.empty() || type_str.empty())
            {
                continue;
            }
            number += base;
            FLDID32 id = Fmkfldid32(field_types[type_str], number);
            if(id == BADFLDID)
            {
                throw runtime_error(line + "[" + Fstrerror32(Ferror32) + "]");
            }
            field_def f;
            f.name = name;
            f.number = number;
            f.id = id;
            f.type_str = type_str;
            result.push_back(f);               
        }
    }
    
    return result;
}

size_t get_max_name_size(vector<field_def> const& fields)
{
    size_t result = 0;
    for(auto&& f : fields)
    {
        result = max(result, f.name.size());
    }
    return result;
}

void write_output(ostream& os, vector<field_def> const& fields)
{
    size_t name_width = get_max_name_size(fields);
    os << R"(
#include "fml32.h"
namespace field32 {
)";
    for(auto&& f : fields)
    {
        os << "const FLDID32 " << setw(name_width) << left << f.name
           << " = " << setw(12) << f.id << "; // number: "
           << setw(12) <<  f.number << " type: " << f.type_str << "\n"; 
    }
    os << "} // end namespace" << endl;
}



int main(int argc, char** argv)
{
    try
    {
        
        string program_name = argv[0];
        if(argc < 3)
        {
            throw runtime_error("Usage: " + program_name + " INPUT_FILE OUTPUT_FILE");
        }
        string input_file_name = argv[1];
        string output_file_name = argv[2];
        ifstream is(input_file_name);
        if(!is)
        {
            throw runtime_error("error reading from " + input_file_name);
        }
        ofstream os(output_file_name);
        if(!os)
        {
            throw runtime_error("error opening " + output_file_name + " for write");
        }
        
        write_output(os, read_input(is));
   
        return 0;
    }
    catch(exception const& e)
    {
        cerr << "error: " << e.what() << endl;
        return 1;
    }
}
