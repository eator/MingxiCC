#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct VarWithOffset {
    string var_name;
    int offset;
};

// Map variable names(i.e. strings) to offsets(integers)
// in the Env(current stack frame).
class Environment {
  private:
    vector<VarWithOffset> items;

  public:
    void set_offset(const string &var_name, int offset) {
        VarWithOffset vwo = {var_name, offset};
        items.push_back(vwo);
    }

    int get_offset(const string &var_name) {
        for (auto &i : items)
            if (i.var_name == var_name)
                return i.offset;

        cerr << "Could not find [" << var_name << "] in environment" << endl;
        return -1;
    }
};
